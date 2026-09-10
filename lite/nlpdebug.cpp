/*******************************************************************************
Copyright (c) 2026 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// nlpdebug.cpp
//
// Rule-level debug server. See nlpdebug.h for what this is for and how it is
// wired in; this file is the transport, the protocol and the state dump.
//
// MESSAGE CATALOGUE
//
// Both directions are newline-delimited JSON objects, UTF-8, one per line.
//
// Engine -> client (unsolicited events):
//   {"event":"stopped","reason":"entry|step|breakpoint|matched|failed|passStart|pause",
//    "pass":15,"passName":"moneyAttributes","line":17,"ruleOrd":3,
//    "node":"_money","nodeStart":163,"nodeEnd":166,"eltsMatched":2}
//   {"event":"output","text":"..."}
//   {"event":"terminated"}
//
// Client -> engine (each carries a "seq"; the reply echoes it):
//   {"seq":1,"command":"continue"}                  run until a breakpoint
//   {"seq":2,"command":"stepRule"}                  stop at the next rule tried
//   {"seq":3,"command":"stepMatch"}                 stop at the next rule that matches
//   {"seq":4,"command":"stepPass"}                  stop at the start of the next pass
//   {"seq":5,"command":"setBreakpoints","pass":15,"lines":[17,23]}
//   {"seq":6,"command":"stopOnFailure","value":true}  also stop when a rule fails
//   {"seq":7,"command":"state"}                     current pass/rule/node
//   {"seq":8,"command":"tree","depth":3}            parse tree from the root
//   {"seq":9,"command":"node"}                      current node, its children and attributes
//   {"seq":10,"command":"rule"}                     the rule being tried, element by element
//   {"seq":11,"command":"detach"}                   let the run finish unhooked
//
// Replies are {"seq":N,"ok":true,...} or {"seq":N,"ok":false,"error":"..."}.
//
// WHY A HAND-ROLLED JSON WRITER. The engine has no JSON dependency and this
// file emits perhaps a dozen shapes, all of them ours. Pulling in a library to
// serialise them would be a bigger change to the build than the debugger is to
// the engine. Only strings need real care, and escapeJson below does that.
//
// THREADING. There is none. The engine stops inside a hook and reads commands
// on the same thread, so there is no shared state and no lock. The cost is that
// an asynchronous "pause" cannot arrive mid-pass -- the client can only pause at
// the next hook, which for a running analyzer is microseconds away.

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/lite.h"
#include "lite/global.h"
#include "dlist.h"
#include "inline.h"
#include "io.h"
#include "chars.h"
#include "string.h"
#include "node.h"
#include "tree.h"
#include "parse.h"
#include "htab.h"
#include "slist.h"
#include "lite/nlppp.h"
#include "gen.h"
#include "irule.h"
#include "ielt.h"
#include "isugg.h"
#include "ielement.h"
#include "pn.h"
#include "lite/nlpdebug.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <cctype>   // isspace, isdigit
#include <cstdio>   // sprintf
#include <cstdlib>  // strtol
#include <cstring>  // memset

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET nlp_socket_t;
#define NLP_INVALID_SOCKET INVALID_SOCKET
#define nlp_closesocket closesocket
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
typedef int nlp_socket_t;
#define NLP_INVALID_SOCKET (-1)
#define nlp_closesocket close
#endif

bool NlpDebug::active_ = false;

namespace
{

// ---- connection state -------------------------------------------------------

nlp_socket_t g_listen = NLP_INVALID_SOCKET;
nlp_socket_t g_client = NLP_INVALID_SOCKET;
std::string g_inbuf; // bytes read but not yet consumed as a whole line

// How the engine should behave until the next stop.
enum RunMode
{
	MODE_RUN,        // only breakpoints stop us
	MODE_STEP_RULE,  // stop at the next rule attempt
	MODE_STEP_MATCH, // stop at the next rule that matches
	MODE_STEP_PASS   // stop at the start of the next pass
};

RunMode g_mode = MODE_STEP_PASS; // so arming stops at the first pass
bool g_stopOnFailure = false;
bool g_detached = false;

// Set only while the analyzer is running over the input text. The hooks are
// inert outside that window -- see NlpDebug::arm().
bool g_armed = false;

// pass number -> lines with a breakpoint on them.
std::map<long, std::set<long> > g_breakpoints;

// Where we are, refreshed at each hook so a state/tree/node request can be
// answered without the hook having to pass anything along.
Parse *g_parse = 0;
Nlppp *g_nlppp = 0;
long g_pass = 0;
std::string g_passName;
long g_ruleOrd = 0;
long g_eltsMatched = -1;

// ---- narrow-string helpers --------------------------------------------------

// The engine is _TCHAR-based and may be built for UNICODE; the wire is UTF-8.
// TCHAR2A is the codebase's own conversion and is the identity in narrow builds.
std::string narrow(const _TCHAR *s)
{
	if (!s) return std::string();
#ifdef UNICODE
	LPCSTR p = CTCHAR2CA(s);
	return p ? std::string(p) : std::string();
#else
	return std::string(s);
#endif
}

std::string escapeJson(const std::string &in)
{
	std::string out;
	out.reserve(in.size() + 8);
	for (size_t i = 0; i < in.size(); ++i)
	{
		unsigned char c = (unsigned char)in[i];
		switch (c)
		{
		case '"':  out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\b': out += "\\b";  break;
		case '\f': out += "\\f";  break;
		case '\n': out += "\\n";  break;
		case '\r': out += "\\r";  break;
		case '\t': out += "\\t";  break;
		default:
			if (c < 0x20)
			{
				// Control characters must be escaped; anything >= 0x20 passes
				// through, which keeps already-valid UTF-8 intact.
				char buf[8];
				sprintf(buf, "\\u%04x", c);
				out += buf;
			}
			else
				out += (char)c;
		}
	}
	return out;
}

std::string jstr(const std::string &s) { return "\"" + escapeJson(s) + "\""; }
std::string jnum(long n)
{
	std::ostringstream o;
	o << n;
	return o.str();
}

// ---- socket plumbing --------------------------------------------------------

bool sendLine(const std::string &line)
{
	if (g_client == NLP_INVALID_SOCKET) return false;
	std::string payload = line;
	payload += "\n";
	size_t sent = 0;
	while (sent < payload.size())
	{
		int n = (int)send(g_client, payload.data() + sent, (int)(payload.size() - sent), 0);
		if (n <= 0) return false;
		sent += (size_t)n;
	}
	return true;
}

// Block until a whole line arrives. Returns false if the peer went away, which
// the caller treats as a detach rather than an error: losing the debugger must
// never take the analysis down with it.
bool readLine(std::string &out)
{
	for (;;)
	{
		size_t nl = g_inbuf.find('\n');
		if (nl != std::string::npos)
		{
			out = g_inbuf.substr(0, nl);
			g_inbuf.erase(0, nl + 1);
			if (!out.empty() && out[out.size() - 1] == '\r') out.erase(out.size() - 1);
			return true;
		}
		char buf[4096];
		int n = (int)recv(g_client, buf, sizeof(buf), 0);
		if (n <= 0) return false;
		g_inbuf.append(buf, (size_t)n);
	}
}

// ---- a deliberately small JSON reader ---------------------------------------
//
// The engine only ever parses messages this file documents: flat objects whose
// values are strings, numbers, booleans, or an array of numbers. Rather than
// carry a parser for the whole grammar, these pull one named field out of a
// line. Anything unrecognised is ignored, so an older engine and a newer client
// degrade to the commands they share instead of failing to talk at all.

std::string fieldRaw(const std::string &msg, const std::string &key)
{
	std::string needle = "\"" + key + "\"";
	size_t k = msg.find(needle);
	if (k == std::string::npos) return std::string();
	size_t colon = msg.find(':', k + needle.size());
	if (colon == std::string::npos) return std::string();
	size_t i = colon + 1;
	while (i < msg.size() && isspace((unsigned char)msg[i])) ++i;
	if (i >= msg.size()) return std::string();

	if (msg[i] == '"')
	{
		std::string out;
		++i;
		while (i < msg.size() && msg[i] != '"')
		{
			if (msg[i] == '\\' && i + 1 < msg.size())
			{
				++i;
				switch (msg[i])
				{
				case 'n': out += '\n'; break;
				case 't': out += '\t'; break;
				case 'r': out += '\r'; break;
				default:  out += msg[i]; break;
				}
			}
			else
				out += msg[i];
			++i;
		}
		return out;
	}
	// number, boolean, or array: take up to the next delimiter
	size_t start = i;
	int depth = 0;
	while (i < msg.size())
	{
		char c = msg[i];
		if (c == '[') ++depth;
		else if (c == ']') { if (depth == 0) break; --depth; }
		else if ((c == ',' || c == '}') && depth == 0) break;
		++i;
	}
	std::string out = msg.substr(start, i - start);
	while (!out.empty() && isspace((unsigned char)out[out.size() - 1])) out.erase(out.size() - 1);
	return out;
}

std::string fieldStr(const std::string &msg, const std::string &key)
{
	return fieldRaw(msg, key);
}

long fieldNum(const std::string &msg, const std::string &key, long dflt)
{
	std::string raw = fieldRaw(msg, key);
	if (raw.empty()) return dflt;
	return strtol(raw.c_str(), 0, 10);
}

bool fieldBool(const std::string &msg, const std::string &key, bool dflt)
{
	std::string raw = fieldRaw(msg, key);
	if (raw == "true") return true;
	if (raw == "false") return false;
	return dflt;
}

// "[17, 23, 48]" -> {17,23,48}
std::set<long> fieldNumArray(const std::string &msg, const std::string &key)
{
	std::set<long> out;
	std::string raw = fieldRaw(msg, key);
	size_t i = 0;
	while (i < raw.size())
	{
		while (i < raw.size() && !isdigit((unsigned char)raw[i]) && raw[i] != '-') ++i;
		if (i >= raw.size()) break;
		char *end = 0;
		long v = strtol(raw.c_str() + i, &end, 10);
		out.insert(v);
		i = (size_t)(end - raw.c_str());
	}
	return out;
}

// ---- state serialisation ----------------------------------------------------

// One node as a JSON object. Children are included only while depth remains --
// a full parse tree can be tens of thousands of nodes and the client asks for
// what it can display.
std::string nodeJson(Node<Pn> *node, int depth)
{
	if (!node) return "null";
	Pn *pn = node->getData();
	if (!pn) return "null";

	std::ostringstream o;
	o << "{\"name\":" << jstr(narrow(pn->getName()))
	  << ",\"type\":" << jstr(narrow(Pn::typeName(pn->getType())))
	  << ",\"start\":" << jnum(pn->getStart())
	  << ",\"end\":" << jnum(pn->getEnd())
	  << ",\"ustart\":" << jnum(pn->getUstart())
	  << ",\"uend\":" << jnum(pn->getUend())
	  << ",\"passNum\":" << jnum(pn->getPassnum())
	  << ",\"ruleLine\":" << jnum(pn->getRuleline())
	  << ",\"fired\":" << (pn->getFired() ? "true" : "false")
	  << ",\"built\":" << (pn->getBuilt() ? "true" : "false");

	if (depth > 0)
	{
		o << ",\"children\":[";
		bool first = true;
		for (Node<Pn> *c = node->Down(); c; c = c->Right())
		{
			if (!first) o << ",";
			first = false;
			o << nodeJson(c, depth - 1);
		}
		o << "]";
	}
	else
	{
		// Say how many were withheld so the client can offer to expand.
		long n = 0;
		for (Node<Pn> *c = node->Down(); c; c = c->Right()) ++n;
		o << ",\"childCount\":" << jnum(n);
	}
	o << "}";
	return o.str();
}

// The rule being tried, element by element. With eltsMatched from a failure this
// is what shows an author exactly which element stopped agreeing with the text.
std::string ruleJson(Irule *rule)
{
	if (!rule) return "null";
	std::ostringstream o;
	o << "{\"line\":" << jnum(rule->getLine())
	  << ",\"num\":" << jnum(rule->getNum());

	Isugg *sugg = rule->getSugg();
	if (sugg)
		o << ",\"builds\":" << jstr(narrow(sugg->getName()));

	o << ",\"elements\":[";
	Dlist<Ielt> *phrase = rule->getPhrase();
	bool first = true;
	if (phrase)
	{
		for (Delt<Ielt> *e = phrase->getFirst(); e; e = e->Right())
		{
			Ielt *elt = e->getData();
			if (!elt) continue;
			if (!first) o << ",";
			first = false;
			o << "{\"name\":" << jstr(narrow(elt->getName()))
			  << ",\"min\":" << jnum(elt->getMin())
			  << ",\"max\":" << jnum(elt->getMax()) << "}";
		}
	}
	o << "]}";
	return o.str();
}

std::string currentStateJson()
{
	std::ostringstream o;
	o << "\"pass\":" << jnum(g_pass)
	  << ",\"passName\":" << jstr(g_passName)
	  << ",\"ruleOrd\":" << jnum(g_ruleOrd);

	Irule *rule = g_nlppp ? g_nlppp->getRule() : 0;
	o << ",\"line\":" << jnum(rule ? rule->getLine() : 0);

	Node<Pn> *node = g_nlppp ? g_nlppp->getNode() : 0;
	Pn *pn = node ? node->getData() : 0;
	if (pn)
	{
		o << ",\"node\":" << jstr(narrow(pn->getName()))
		  << ",\"nodeStart\":" << jnum(pn->getStart())
		  << ",\"nodeEnd\":" << jnum(pn->getEnd());
	}
	if (g_eltsMatched >= 0)
		o << ",\"eltsMatched\":" << jnum(g_eltsMatched);
	return o.str();
}

const char *reasonName(NlpDebugStop reason)
{
	switch (reason)
	{
	case NLPDEBUG_ENTRY:        return "entry";
	case NLPDEBUG_STEP:         return "step";
	case NLPDEBUG_BREAKPOINT:   return "breakpoint";
	case NLPDEBUG_RULE_MATCHED: return "matched";
	case NLPDEBUG_RULE_FAILED:  return "failed";
	case NLPDEBUG_PASS_START:   return "passStart";
	case NLPDEBUG_PAUSE:        return "pause";
	}
	return "step";
}

// ---- the stop loop ----------------------------------------------------------

void reply(long seq, const std::string &body)
{
	std::ostringstream o;
	o << "{\"seq\":" << jnum(seq) << ",\"ok\":true";
	if (!body.empty()) o << "," << body;
	o << "}";
	sendLine(o.str());
}

void replyError(long seq, const std::string &msg)
{
	std::ostringstream o;
	o << "{\"seq\":" << jnum(seq) << ",\"ok\":false,\"error\":" << jstr(msg) << "}";
	sendLine(o.str());
}

// Announce a stop, then serve requests until the client resumes us. Returns when
// the engine should carry on.
void stopAndServe(NlpDebugStop reason)
{
	if (!NlpDebug::active_) return;

	{
		std::ostringstream o;
		o << "{\"event\":\"stopped\",\"reason\":\"" << reasonName(reason) << "\","
		  << currentStateJson() << "}";
		if (!sendLine(o.str()))
		{
			// Client vanished. Detach and let the analysis finish.
			NlpDebug::shutdown();
			return;
		}
	}

	for (;;)
	{
		std::string line;
		if (!readLine(line))
		{
			NlpDebug::shutdown();
			return;
		}
		if (line.empty()) continue;

		long seq = fieldNum(line, "seq", 0);
		std::string cmd = fieldStr(line, "command");

		if (cmd == "continue")      { g_mode = MODE_RUN;        reply(seq, ""); return; }
		if (cmd == "stepRule")      { g_mode = MODE_STEP_RULE;  reply(seq, ""); return; }
		if (cmd == "stepMatch")     { g_mode = MODE_STEP_MATCH; reply(seq, ""); return; }
		if (cmd == "stepPass")      { g_mode = MODE_STEP_PASS;  reply(seq, ""); return; }

		if (cmd == "setBreakpoints")
		{
			long pass = fieldNum(line, "pass", 0);
			g_breakpoints[pass] = fieldNumArray(line, "lines");
			reply(seq, "\"count\":" + jnum((long)g_breakpoints[pass].size()));
			continue;
		}
		if (cmd == "stopOnFailure")
		{
			g_stopOnFailure = fieldBool(line, "value", false);
			reply(seq, "");
			continue;
		}
		if (cmd == "state")
		{
			reply(seq, currentStateJson());
			continue;
		}
		if (cmd == "rule")
		{
			reply(seq, "\"rule\":" + ruleJson(g_nlppp ? g_nlppp->getRule() : 0));
			continue;
		}
		if (cmd == "node")
		{
			Node<Pn> *node = g_nlppp ? g_nlppp->getNode() : 0;
			reply(seq, "\"node\":" + nodeJson(node, 1));
			continue;
		}
		if (cmd == "tree")
		{
			int depth = (int)fieldNum(line, "depth", 3);
			// Parse::getTree() is declared as an opaque TREE* (void*) in the
			// public header; the cast is how the rest of lite reaches it.
			Node<Pn> *root = 0;
			if (g_parse)
			{
				Tree<Pn> *tree = (Tree<Pn> *)g_parse->getTree();
				if (tree) root = tree->getRoot();
			}
			reply(seq, "\"tree\":" + nodeJson(root, depth));
			continue;
		}
		if (cmd == "detach")
		{
			reply(seq, "");
			g_detached = true;
			NlpDebug::shutdown();
			return;
		}

		replyError(seq, "unknown command: " + cmd);
	}
}

bool breakpointHere(long pass, long line)
{
	std::map<long, std::set<long> >::const_iterator it = g_breakpoints.find(pass);
	if (it == g_breakpoints.end()) return false;
	return it->second.find(line) != it->second.end();
}

} // namespace

// ---- lifecycle --------------------------------------------------------------

bool NlpDebug::listen(int port)
{
#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		*gerr << _T("[debug: WSAStartup failed; running without the debugger.]") << std::endl;
		return false;
	}
#endif

	g_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listen == NLP_INVALID_SOCKET)
	{
		*gerr << _T("[debug: could not create socket; running without the debugger.]") << std::endl;
		return false;
	}

	int yes = 1;
	setsockopt(g_listen, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)port);
	// Loopback only. This gives an attached client the ability to read the text
	// being analysed, so it must not be reachable from off the machine.
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (bind(g_listen, (struct sockaddr *)&addr, sizeof(addr)) != 0 ||
		 ::listen(g_listen, 1) != 0)
	{
		*gerr << _T("[debug: could not listen on port ") << port
				<< _T("; running without the debugger.]") << std::endl;
		nlp_closesocket(g_listen);
		g_listen = NLP_INVALID_SOCKET;
		return false;
	}

	*gout << _T("[debug: waiting for a client on 127.0.0.1:") << port << _T("]") << std::endl;

	g_client = accept(g_listen, 0, 0);
	if (g_client == NLP_INVALID_SOCKET)
	{
		*gerr << _T("[debug: accept failed; running without the debugger.]") << std::endl;
		nlp_closesocket(g_listen);
		g_listen = NLP_INVALID_SOCKET;
		return false;
	}

	active_ = true;
	g_detached = false;
	g_mode = MODE_STEP_PASS;
	return true;
}

void NlpDebug::shutdown()
{
	active_ = false;
	if (g_client != NLP_INVALID_SOCKET)
	{
		if (!g_detached) sendLine("{\"event\":\"terminated\"}");
		nlp_closesocket(g_client);
		g_client = NLP_INVALID_SOCKET;
	}
	if (g_listen != NLP_INVALID_SOCKET)
	{
		nlp_closesocket(g_listen);
		g_listen = NLP_INVALID_SOCKET;
	}
#ifdef _WIN32
	WSACleanup();
#endif
}

// ---- pause points -----------------------------------------------------------

void NlpDebug::passStart_(Parse *parse, long passNum, const _TCHAR *passFile)
{
	if (!g_armed) return;
	g_parse = parse;
	g_nlppp = 0;
	g_pass = passNum;
	g_passName = narrow(passFile);
	g_ruleOrd = 0;
	g_eltsMatched = -1;

	if (g_mode == MODE_STEP_PASS)
		stopAndServe(NLPDEBUG_PASS_START);
}

void NlpDebug::passEnd_(Parse *parse, long passNum)
{
	if (!g_armed) return;
	// Nothing to stop for, but drop the stale pointers so a later request
	// cannot walk a tree that has been rebuilt underneath it.
	(void)parse;
	(void)passNum;
	g_nlppp = 0;
	g_eltsMatched = -1;
}

void NlpDebug::ruleAttempt_(Nlppp *nlppp, long ord)
{
	if (!g_armed) return;
	g_nlppp = nlppp;
	g_ruleOrd = ord;
	g_eltsMatched = -1;
	if (nlppp && nlppp->getParse()) g_parse = nlppp->getParse();

	Irule *rule = nlppp ? nlppp->getRule() : 0;
	long line = rule ? rule->getLine() : 0;

	if (breakpointHere(g_pass, line))
	{
		stopAndServe(NLPDEBUG_BREAKPOINT);
		return;
	}
	if (g_mode == MODE_STEP_RULE)
		stopAndServe(NLPDEBUG_STEP);
}

void NlpDebug::ruleMatched_(Nlppp *nlppp)
{
	if (!g_armed) return;
	g_nlppp = nlppp;
	g_eltsMatched = -1;
	if (g_mode == MODE_STEP_MATCH)
		stopAndServe(NLPDEBUG_RULE_MATCHED);
}

void NlpDebug::ruleFailed_(Nlppp *nlppp)
{
	if (!g_armed) return;
	g_nlppp = nlppp;
	// How many rule elements matched before the rule gave up.
	//
	// The collected elements are a SIBLING LIST hanging off the collect tree's
	// root -- makeCollect() appends to it and matchCleanup() calls it "the list
	// of collect nodes" -- so the count walks Right() from the root. Descending
	// into the root's children instead counts whatever that first matched node
	// happens to span, which for a wildcard is most of the document.
	//
	// This is read here rather than passed in because the caller must invoke the
	// hook before matchCleanup() empties the list, and doing the counting here
	// keeps that ordering requirement in one place.
	g_eltsMatched = 0;
	if (nlppp && nlppp->getCollect())
	{
		for (Node<Pn> *c = nlppp->getCollect()->getRoot(); c; c = c->Right())
			++g_eltsMatched;
	}
	// Failures are the common case -- most rules do not match most nodes -- so
	// stopping on them is opt-in. When it is on, the client gets the element
	// count with the stop, which is the whole point of watching failures.
	if (g_stopOnFailure)
		stopAndServe(NLPDEBUG_RULE_FAILED);
}

void NlpDebug::arm()
{
	g_armed = true;
}

void NlpDebug::disarm()
{
	g_armed = false;
}

void NlpDebug::runEnd_()
{
	sendLine("{\"event\":\"terminated\"}");
	shutdown();
}
