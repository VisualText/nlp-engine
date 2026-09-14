/*******************************************************************************
Copyright (c) 2026 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// callana.cpp
//
// callanalyzer() and callconcept(): one analyzer's passes run inside another's.
//
// WHAT IT DOES. callanalyzer(pnode, concept, "name") runs the passes of the
// named analyzer inside the analysis already in progress. The called analyzer
// gets no text or tree of its own. Its passes work on the subtree under pnode
// in the caller's parse tree, and on the caller's KB, where callconcept() gives
// them the concept they were handed to put things in. Whatever they change is
// there for the caller as soon as the call returns.
//
// WHAT IS LOADED ONCE. The first call builds the analyzer's passes (its rules
// and @DECL functions) and reads its kb/user .kbb and .dict files into the
// caller's KB. Its lazily loaded *full.kbb and *full.dict files are registered
// with that KB. The analyzer then stays in the VTRun registry, so no later call
// reads anything again. Its .kb files are not read: they are a whole saved KB,
// hierarchy and all, and reading one into a KB that already has a hierarchy
// adds its attribute values a second time.
//
// WHAT RUNS ON EACH CALL. The passes, in order, on the subtree, with these
// differences from running on a text of its own:
//   - the root they get is a stand-in named _ROOT that holds pnode's
//     children for the length of the call (see borrowChildren), so they
//     select and traverse exactly as on a text of their own;
//   - tokenizer passes are skipped, because the tree already exists;
//   - a dicttok or dicttokz pass looks up the subtree's words instead,
//     including in the lazy *full files, which only load a word once it is
//     seen;
//   - G() variables start empty and are discarded afterwards;
//   - G("$apppath") is the called analyzer's folder;
//   - no per-pass .tree dumps, which are named by pass number and would
//     overwrite the caller's.
//
// WHAT IS REFUSED. An analyzer cannot call itself, directly or around a cycle
// (A calls B calls A). Its passes are a single set of objects, and they hold
// state while they run.

#include "StdAfx.h"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <utility>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <ctime>

#include "machine.h"
#include "lite/lite.h"
#include "lite/global.h"
#include "u_out.h"
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "htab.h"
#include "kb.h"
#include "prim/libprim.h"
#include "lite/mach.h"
#include "dlist.h"
#include "node.h"
#include "tree.h"
#include "lite/Auser.h"
#include "lite/iarg.h"
#include "inline.h"
#include "str.h"
#include "chars.h"
#include "io.h"
#include "pn.h"
#include "ana.h"
#include "parse.h"
#include "Eana.h"
#include "lite/nlp.h"
#include "lite/nlppp.h"
#include "rfasem.h"
#include "arg.h"
#include "ipair.h"
#include "seqn.h"
#include "algo.h"
#include "dicttok.h"
#include "lite/vtrun.h"
#include "lite/nlpdebug.h"
#include "lite/Arun.h"
#include "fn.h"

// What callconcept() returns when the analyzer is not being called: somewhere
// to put results while developing it on its own.
static _TCHAR STANDALONE_CONCEPT[] = _T("callanalyzer");

// One callanalyzer() in progress.
struct Call
	{
	Ana *caller;		// The analyzer that made the call.
	Ana *callee;		// The analyzer whose passes are running.
	CONCEPT *con;		// What callconcept() returns to them.
	};

// Calls in progress, outermost first.
static std::vector<Call> calls;

// Each loaded analyzer paired with a KB its files have been read into.
static std::set<std::pair<NLP*, CG*> > loadedInto;

// Paths handed to the analyzers loaded here. The engine passes these around as
// bare pointers, so keep them for the life of the process rather than rely on
// every holder having made a copy. A list, so the strings never move.
static std::list<std::string> anaPaths;

static bool callError(Parse *parse, const std::string &msg)
{
_stprintf(Errbuf, _T("[callanalyzer: %.*s]"), (int)(MAXSTR - 20), msg.c_str());
parse->errOut(true);
return false;
}

static bool isAbsolute(const std::string &path)
{
if (path.empty())
	return false;
if (path[0] == '/' || path[0] == '\\')
	return true;
return path.size() > 1 && path[1] == ':';
}

static std::string trimSlashes(std::string path)
{
while (path.size() > 1 && (path.back() == '/' || path.back() == '\\'))
	path.pop_back();
return path;
}

static std::string baseName(const std::string &path)
{
size_t slash = path.find_last_of("/\\");
return slash == std::string::npos ? path : path.substr(slash + 1);
}

static std::string dirName(const std::string &path)
{
size_t slash = path.find_last_of("/\\");
return slash == std::string::npos ? std::string(".") : path.substr(0, slash);
}

static _TCHAR *keepPath(const std::string &path)
{
anaPaths.push_back(path);
return (_TCHAR *) anaPaths.back().c_str();
}

// Passes that build the parse tree from the text.
static bool isTokenizer(_TCHAR *algo)
{
return algo
	&& (!strcmp_i(algo, _T("tokenize"))
	 || !strcmp_i(algo, _T("tok"))
	 || !strcmp_i(algo, _T("token"))
	 || !strcmp_i(algo, _T("cmltokenize"))
	 || !strcmp_i(algo, _T("cmltok"))
	 || !strcmp_i(algo, _T("chartok"))
	 || !strcmp_i(algo, _T("lines"))
	 || !strcmp_i(algo, _T("line")));
}

static bool isDictTok(_TCHAR *algo)
{
return algo
	&& (!strcmp_i(algo, _T("dicttok"))
	 || !strcmp_i(algo, _T("dicttokz")));
}

/********************************************
* FN:		FINDCALLEE
* SUBJ:	Find the named analyzer, building its passes on first use.
* NOTE:	A name already in the registry is used as is -- that is what
*			makes the second call free. Otherwise an absolute path names
*			the analyzer folder, and anything else is looked for beside the
*			calling analyzer's folder, which is how analyzers sit in a
*			VisualText workspace.
********************************************/

static NLP *findCallee(Parse *parse, const std::string &given)
{
VTRun *vtrun = parse->getNLP()->getVTRun();

std::string path = trimSlashes(given);
std::string name = baseName(path);
NLP *nlp = vtrun->findAna((_TCHAR *) name.c_str());
if (nlp)
	return nlp;

std::string anadir = isAbsolute(path)
	? path
	: dirName(trimSlashes(parse->getAppdir())) + DIR_STR + path;
std::string seqfile = anadir + DIR_STR + "spec" + DIR_STR + "analyzer.seq";
if (!std::ifstream(seqfile.c_str()).good())
	{
	callError(parse, "No analyzer named " + given + ": there is no " + seqfile + ".");
	return 0;
	}

_TCHAR *dir = keepPath(anadir);
bool silent = parse->getEana()->getFsilent();
clock_t start = clock();

// The same steps as NLP_ENGINE::init, which cannot be used from inside a run:
// it resets the engine's notion of the current analyzer. The analyzer gets a
// KB object because an NLP expects one, but it is never read -- its passes run
// on the caller's KB.
nlp = vtrun->makeNLP(dir, (_TCHAR *) name.c_str(), false, silent, false);
if (!nlp)
	{
	callError(parse, "Couldn't create analyzer " + name + ".");
	return 0;
	}
if (!vtrun->makeCG(dir, false, nlp, true))
	{
	vtrun->rmAna(nlp);
	vtrun->deleteNLP(nlp);
	callError(parse, "Couldn't make a knowledge base object for " + name + ".");
	return 0;
	}
if (!nlp->make_analyzer(keepPath(seqfile), dir, false, silent, 0, false, false))
	{
	vtrun->rmAna(nlp);
	vtrun->deleteNLP(nlp);
	callError(parse, "Couldn't build analyzer " + name + ". See its logs"
		+ DIR_STR + "make_ana.log.");
	return 0;
	}

if (!silent)
	std::_t_cerr << _T("[callanalyzer: loaded ") << name.c_str() << _T(": ")
		<< (double)(clock() - start) / CLOCKS_PER_SEC << _T(" sec]") << std::endl;
return nlp;
}

// The called analyzer's kb/user files of one kind, as CG::openKBB and
// CG::openDict choose them: a consolidated all.<ext> if there is one, else
// every file with that extension.
static std::vector<std::filesystem::path> kbFiles(
	const std::filesystem::path &kbdir, const std::string &ext)
{
std::vector<std::filesystem::path> files;
std::error_code ec;
std::filesystem::path all = kbdir / ("all" + ext);
if (std::filesystem::exists(all, ec))
	{
	files.push_back(all);
	return files;
	}
for (const auto &entry : std::filesystem::directory_iterator(kbdir, ec))
	if (entry.path().extension().string() == ext)
		files.push_back(entry.path());
std::sort(files.begin(), files.end());
return files;
}

// Read the called analyzer's dictionaries and kbb files into the KB it is
// run with, once for each such KB. Lazy *full files are registered, not read.
static void loadKnowledge(NLP *callee, Ana *ana, CG *cg)
{
if (!loadedInto.insert(std::make_pair(callee, cg)).second)
	return;

std::error_code ec;
std::filesystem::path kbdir = std::filesystem::path(ana->getAppdir()) / "kb" / "user";
if (!std::filesystem::is_directory(kbdir, ec))
	return;

// kbb files first, as CG::readKB reads them, so dictionaries can pair with them.
std::vector<std::filesystem::path> kbbs = kbFiles(kbdir, ".kbb");
std::vector<std::filesystem::path> dicts = kbFiles(kbdir, ".dict");
for (const auto &file : kbbs)
	{
	if (cg->stemEndsWithFull(file.stem().string()))
		cg->openFullKBB(file.string());
	else
		cg->readKBB(file.string());
	}
for (const auto &file : dicts)
	{
	if (cg->stemEndsWithFull(file.stem().string()))
		cg->openFullDict(file.string());
	else
		cg->readDict(file.string(), kbbs);
	}
}

// Give the called analyzer a root of its own, as it has running on a text:
// named _ROOT, unsealed, with no parent or siblings, spanning node's text and
// holding node's children. @NODES, @PATH and @MULTI select by name and only
// descend into unsealed nodes, and they walk a root's siblings; node itself is
// named for what the caller built, usually sealed, and has siblings in the
// caller's tree. So without this, @NODES _ROOT selects nothing, deeper selects
// never reach inside, and passes can run on the caller's nodes next to node.
static Node<Pn> *borrowChildren(Parse *parse, Node<Pn> *node)
{
Pn *pn = node->getData();
Sym *sym = ((Htab *) parse->getNLP()->getHtab())->hsym(_T("_ROOT"));
Node<Pn> *root = Pn::makeTnode(pn->getStart(), pn->getEnd(),
	pn->getUstart(), pn->getUend(), PNNODE, pn->getText(),
	sym->getStr(), sym, pn->getLine());
root->getData()->setUnsealed(true);

Node<Pn> *children = node->Down();
root->setDown(children);
if (children)
	children->setUp(root);		// Only a first child points up.
node->setDown(0);
return root;
}

// Put the children back under node, as the called analyzer left them, and
// free the borrowed root. Variables set on the root go with it.
static void returnChildren(Node<Pn> *root, Node<Pn> *node)
{
Node<Pn> *children = root->Down();
node->setDown(children);
if (children)
	children->setUp(node);
root->setDown(0);
Node<Pn>::DeleteNodeAndData(root);
}

// Run a loaded analyzer's passes on the subtree under node.
static bool runCallee(Parse *parse, NLP *callee, Node<Pn> *node, CONCEPT *con)
{
Ana *ana = (Ana *) callee->getAna();
if (!ana || !ana->getSeq())
	return callError(parse, std::string(callee->getName()) + " has no passes.");

Ana *caller = parse->getAna();
CG *cg = caller->getCG();
loadKnowledge(callee, ana, cg);

// Passes find the KB, and their @DECL functions, through the Ana the parse is
// running, so the called analyzer's Ana is put in and pointed at this KB.
CG *ownCG = ana->getCG();
ana->setCG(cg);

TREE *tree = parse->getTree();
Delt<Seqn> *seq = parse->getSeq();
long currpass = parse->getCurrpass();
long rulepass = parse->getRulepass();
Dlist<Ipair> *vars = parse->getVars();
std::string appdir = parse->getAppdir();

Node<Pn> *root = borrowChildren(parse, node);
Tree<Pn> subtree(root);		// Does not own the nodes.
parse->setTree(&subtree);
parse->setAna(ana);
parse->setVars(0);
parse->setAppdir(ana->getAppdir());

Call call = { caller, ana, con };
calls.push_back(call);

bool ok = true;
long num = 0;
for (Delt<Seqn> *step = ana->getSeq(); step && ok; step = step->Right())
	{
	++num;
	Seqn *pass = step->getData();
	Algo *algo = pass->getAlgo();
	_TCHAR *algoname = pass->getAlgoname();
	if (!pass->getActive() || !algo || isTokenizer(algoname))
		continue;

	// What Parse::stepExecute and iniPass set, minus the dump files.
	parse->setSeq(step);
	parse->setCurrpass(num);
	parse->setRulepass(num);
	if (isDictTok(algoname))
		((DICTTok *) algo)->applyToSubtree(parse, root);
	else
		ok = algo->Execute(parse, pass);
	}

calls.pop_back();
returnChildren(root, node);

if (parse->getVars())
	Dlist<Ipair>::DeleteDlistAndData(parse->getVars());
parse->setVars(vars);
parse->setAppdir((_TCHAR *) appdir.c_str());
parse->setAna(caller);
parse->setTree(tree);
parse->setSeq(seq);
parse->setCurrpass(currpass);
parse->setRulepass(rulepass);
ana->setCG(ownCG);

if (!ok)
	return callError(parse, std::string("A pass of ") + callee->getName() + " failed.");
return true;
}

static bool callAnalyzer(Parse *parse, Node<Pn> *node, CONCEPT *con, _TCHAR *given)
{
if (!node || !con)
	return callError(parse, "Given no parse node or no concept.");
if (!given || !*given)
	return callError(parse, "Given no analyzer name.");

// Nothing the called analyzer does -- building from its rule files, or running
// its passes -- is the analyzer being debugged. Its pass numbers and lines
// would be reported against the caller's files.
bool wasArmed = NlpDebug::suspend();

bool ok = false;
NLP *callee = findCallee(parse, given);
if (callee)
	{
	Ana *ana = (Ana *) callee->getAna();
	bool running = (ana == parse->getAna());
	for (const Call &call : calls)
		if (call.caller == ana || call.callee == ana)
			running = true;
	if (running)
		callError(parse, std::string(callee->getName()) + " is already running."
			+ " An analyzer cannot call itself, directly or through another analyzer.");
	else
		ok = runCallee(parse, callee, node, con);
	}

NlpDebug::resume(wasArmed);
return ok;
}

static CONCEPT *callConcept(Parse *parse)
{
if (!calls.empty())
	return calls.back().con;
CG *cg = parse->getAna()->getCG();
CONCEPT *root = cg->findRoot();
return root ? cg->getConcept(root, STANDALONE_CONCEPT) : 0;
}


/********************************************
* FN:		FNCALLANALYZER
* CR:		09/13/26 DD.
* SUBJ:	Run another analyzer's passes on part of this parse tree.
* RET:	True if ok, else false.
*			UP - 1 if the analyzer ran, else 0.
* FORMS:	callanalyzer(pnode, concept, analyzer_str)
* NOTE:	The analyzer works on the subtree under pnode and on this KB,
*			and callconcept() gives it the concept. It is loaded on first use
*			and stays loaded.
********************************************/

bool Fn::fnCallanalyzer(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *node_sem = 0;
RFASem *con_sem = 0;
_TCHAR *name = 0;

if (!Arg::sem1(_T("callanalyzer"),nlppp,(DELTS*&)args,node_sem))
	return false;
if (!Arg::sem1(_T("callanalyzer"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("callanalyzer"),(DELTS*&)args,name))
	return false;
if (!Arg::done((DELTS*)args,_T("callanalyzer"),parse))
	return false;

Node<Pn> *node = (node_sem && node_sem->getType() == RSNODE)
	? node_sem->getNode() : 0;
CONCEPT *con = (con_sem && con_sem->getType() == RS_KBCONCEPT)
	? con_sem->getKBconcept() : 0;

sem = new RFASem(callAnalyzer(parse, node, con, name) ? 1LL : 0LL);
return true;
}


/********************************************
* FN:		FNCALLCONCEPT
* CR:		09/13/26 DD.
* SUBJ:	The concept an analyzer was handed by callanalyzer().
* RET:	True if ok, else false.
*			UP - the concept.
* FORMS:	callconcept()
* NOTE:	Run on its own, the analyzer gets the concept "callanalyzer"
*			under the KB root instead, so it can be developed without a
*			caller.
********************************************/

bool Fn::fnCallconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;
if (!Arg::done((DELTS*)args,_T("callconcept"),parse))
	return false;

CONCEPT *con = callConcept(parse);
if (con)
	sem = new RFASem(con, RS_KBCONCEPT, parse->getAna()->getCG());
return true;
}


/********************************************
* FN:		CALLANALYZER
* CR:		09/13/26 DD.
* SUBJ:	Compiled-analyzer form of callanalyzer().
********************************************/

long long Arun::callanalyzer(
	Nlppp *nlppp,
	NODE *node,
	RFASem *con_sem,
	_TCHAR *name
	)
{
CONCEPT *con = 0;
if (con_sem)
	{
	if (con_sem->getType() == RS_KBCONCEPT)
		con = con_sem->getKBconcept();
	delete con_sem;
	}
return callAnalyzer(nlppp->parse_, (Node<Pn> *) node, con, name) ? 1 : 0;
}

// VARIANT.
long long Arun::callanalyzer(
	Nlppp *nlppp,
	NODE *node,
	RFASem *con_sem,
	RFASem *name_sem
	)
{
_TCHAR *name = name_sem ? sem_to_str(name_sem) : 0;
long long ok = callanalyzer(nlppp, node, con_sem, name);
if (name_sem)
	delete name_sem;
return ok;
}

// VARIANT.
long long Arun::callanalyzer(
	Nlppp *nlppp,
	RFASem *node_sem,
	RFASem *con_sem,
	_TCHAR *name
	)
{
NODE *node = node_sem ? node_sem->sem_to_node() : 0;
if (node_sem)
	delete node_sem;
return callanalyzer(nlppp, node, con_sem, name);
}

// VARIANT.
long long Arun::callanalyzer(
	Nlppp *nlppp,
	RFASem *node_sem,
	RFASem *con_sem,
	RFASem *name_sem
	)
{
NODE *node = node_sem ? node_sem->sem_to_node() : 0;
if (node_sem)
	delete node_sem;
return callanalyzer(nlppp, node, con_sem, name_sem);
}


/********************************************
* FN:		CALLCONCEPT
* CR:		09/13/26 DD.
* SUBJ:	Compiled-analyzer form of callconcept().
********************************************/

RFASem *Arun::callconcept(
	Nlppp *nlppp
	)
{
Parse *parse = nlppp->parse_;
CONCEPT *con = callConcept(parse);
return con ? new RFASem(con, RS_KBCONCEPT, parse->getAna()->getCG()) : 0;
}
