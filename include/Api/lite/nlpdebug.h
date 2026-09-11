/*******************************************************************************
Copyright (c) 2026 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// nlpdebug.h
//
// Rule-level debug server for NLP++.
//
// WHAT THIS IS FOR. The .tree dumps an analyzer already writes give a snapshot
// per pass, which is enough to step through a run one pass at a time. What they
// cannot show is what happened INSIDE a pass: which rules were tried at a node,
// which of them failed and how far each got before failing, and what the tree
// looked like part-way through. That is the question an NLP++ author actually
// has when a rule does not fire. This server answers it by letting the engine
// stop between rule attempts and be interrogated.
//
// HOW IT IS WIRED. Nothing happens unless nlp is started with -debug <port>.
// The engine then listens on 127.0.0.1:<port>, waits for one client to attach,
// and from then on stops at the pause points below. The hooks are placed in
// Pat::matchRules / Pat::matchRule (lite/pat.cpp) and around pass execution.
//
// COST WHEN OFF. Every hook is an inline test of one static bool, so a normal
// run pays a single predictable branch per rule attempt and nothing else. That
// matters: matchRules is the hottest loop in the engine and an analyzer can try
// millions of rules on one input.
//
// PROTOCOL. Newline-delimited JSON over TCP, in both directions. Deliberately
// NOT the Debug Adapter Protocol: DAP is a large spec with a JSON-RPC framing
// layer, and putting it in the engine would mean a dependency and a lot of
// surface to maintain in C++. The editor-side adapter already speaks DAP and
// translates, so what crosses this socket is only what the engine actually
// knows. See nlpdebug.cpp for the message catalogue.

#ifndef NLP_LITE_NLPDEBUG_H
#define NLP_LITE_NLPDEBUG_H

#include "lite/global.h"

class Nlppp;
class Parse;
class Irule;

// Why the engine stopped. Sent to the client as the "reason" of a stopped event.
enum NlpDebugStop
{
	NLPDEBUG_ENTRY,        // attached, about to run the first pass
	NLPDEBUG_STEP,         // a step command completed
	NLPDEBUG_BREAKPOINT,   // a rule on a breakpoint line was reached
	NLPDEBUG_RULE_MATCHED, // a rule matched (step-to-match, or a match breakpoint)
	NLPDEBUG_RULE_FAILED,  // a rule failed (only when the client asked to see failures)
	NLPDEBUG_PASS_START,   // a new pass began
	NLPDEBUG_PAUSE,        // the client asked to pause
	NLPDEBUG_STATEMENT     // about to run a statement in @CODE, @POST or @DECL
};

class LITE_API NlpDebug
{
public:
	// Start listening on 127.0.0.1:port and block until a client attaches.
	// Returns false if the socket could not be opened, in which case the engine
	// runs normally rather than refusing to start -- a debugger that cannot
	// attach should not stop someone from getting their analysis done.
	static bool listen(int port);

	// Close the connection and stop hooking. Safe to call when never started.
	static void shutdown();

	// Arm the pause points for the actual text analysis, and disarm afterwards.
	//
	// This matters more than it looks. Before it analyses anything the engine
	// parses its OWN grammar -- the .nlp pass files are themselves read with Pat
	// and a bootstrap rule set -- so the very same hooks fire hundreds of times
	// on rules the user never wrote, with pass numbers that mean nothing to them.
	// Arming around parse->execute() is what makes a stop correspond to the
	// analyzer the user is actually debugging.
	static void arm();
	static void disarm();

	// True once a client is attached. Read directly by the inline hooks below;
	// public so they can stay inline.
	static bool active_;

	static bool active() { return active_; }

	// ---- pause points -------------------------------------------------------
	//
	// Each is a guarded wrapper around the real implementation so that a
	// non-debug run does not even make a call.

	// A pass is about to run / has finished.
	static void passStart(Parse *parse, long passNum, const _TCHAR *passFile)
		{ if (active_) passStart_(parse, passNum, passFile); }
	static void passEnd(Parse *parse, long passNum)
		{ if (active_) passEnd_(parse, passNum); }

	// About to try `rule` at the current node. `ord` is its position in the
	// candidate list at this node, which is what makes "step to the next rule"
	// meaningful to a user reading the pass file top to bottom.
	static void ruleAttempt(Nlppp *nlppp, long ord)
		{ if (active_) ruleAttempt_(nlppp, ord); }

	// The rule matched, before its actions run, so the client can see the tree
	// as it was when the match was decided.
	static void ruleMatched(Nlppp *nlppp)
		{ if (active_) ruleMatched_(nlppp); }

	// The rule failed. How far it got is read off the collect tree inside the
	// hook -- that count is the single most useful number when a rule does not
	// fire, because it says WHERE the rule stopped agreeing with the text.
	//
	// MUST be called before matchCleanup(), which empties the collect tree.
	static void ruleFailed(Nlppp *nlppp)
		{ if (active_) ruleFailed_(nlppp); }

	// About to run one statement of @CODE, @POST or a @DECL function body.
	//
	// `line` is the statement's line in the file it was WRITTEN in, which is not
	// always the pass being executed: a @DECL function defined in one pass and
	// called from another runs with parse->currpass_ swapped to the defining
	// pass (Ifunc::eval does this for error reporting), so reading the pass at
	// the hook rather than caching it is what makes a breakpoint inside a
	// function land in the right file.
	//
	// This fires far more often than a rule attempt -- once per statement -- so
	// the guard doing nothing quickly matters even more here than elsewhere.
	static void statement(Nlppp *nlppp, long line)
		{ if (active_) statement_(nlppp, line); }

	/**
	 * A user-defined function is about to run its body.
	 *
	 * `pass` and `line` are the CALLER's, captured before Ifunc::eval swaps the
	 * current pass to the one the function was defined in -- so they say where
	 * the call was written, which is what a call stack is made of.
	 *
	 * There is deliberately no matching hook on the way out. The record is kept
	 * indexed by call depth and rewritten by the next call at that depth, so a
	 * body that returns early, errors, or exits the pass cannot leave the stack
	 * drifted -- and Ifunc::eval has several ways out.
	 */
	static void callEnter(Nlppp *nlppp, const _TCHAR *name, long pass, long line)
		{ if (active_) callEnter_(nlppp, name, pass, line); }

	// The analyzer finished. Tells the client the run is over and closes.
	static void runEnd()
		{ if (active_) runEnd_(); }

private:
	// Real implementations, out of line so the header stays cheap to include.
	static void passStart_(Parse *parse, long passNum, const _TCHAR *passFile);
	static void passEnd_(Parse *parse, long passNum);
	static void ruleAttempt_(Nlppp *nlppp, long ord);
	static void ruleMatched_(Nlppp *nlppp);
	static void ruleFailed_(Nlppp *nlppp);
	static void statement_(Nlppp *nlppp, long line);
	static void callEnter_(Nlppp *nlppp, const _TCHAR *name, long pass, long line);
	static void runEnd_();
};

#endif // NLP_LITE_NLPDEBUG_H
