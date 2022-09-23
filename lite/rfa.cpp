/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	RFA.CPP
* FILE:	lite\rfa.cpp
* CR:		10/21/98 AM.
* SUBJ:	Module for the rule-file analyzer (RFA).
* NOTE:	This is a meta-analyzer, built by hand in the Lite code.
*			It parses a rules file and builds internal machinery for
*			executing the rules and actions in the file.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"				// 03/09/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/inline.h"	// Linux.	// 04/26/07 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"				// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "tree.h"			// 02/26/01 AM.
#include "node.h"	// 07/07/03 AM.
//#include "thtab.h"				// 02/26/01 AM.
#include "io.h"
#include "chars.h"
#include "ana.h"
#include "Eana.h"				// 02/26/01 AM.
#include "tok.h"
#include "line.h"
#include "lite/nlppp.h"			// 07/24/06 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "thtab.h"				// 02/26/01 AM.
#include "pat.h"
#include "intern.h"
#include "rfasem.h"
#include "postrfa.h"
#include "parse.h"		// 12/04/98 AM.
#include "rfa.h"

bool RFA::debug_ = false;

#ifndef STABLE_
int RFA::count_ = 0;
#endif

/********************************************
* FN:		Special Functions for Class
********************************************/

RFA::RFA(bool debug)
{
debug_ = debug;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

RFA::~RFA()
{
#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

/********************************************
* FN:		GETCOUNT
* CR:		11/11/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int RFA::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/11/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void RFA::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active RFA count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active RFA count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

bool RFA::Debug()	{return debug_;}


/********************************************
* FN:		Modify Functions
********************************************/

void RFA::setDebug(bool x)	{debug_ = x;}


/********************************************
* FN:		MAKE_RFA
* CR:		10/21/98 AM.
* SUBJ:	Create the sequence for the rule-file analyzer.
* NOTE:	Looks like a convention we're developing is to use underscores
*			in C functions that don't belong to any class.
*			BUILDING RF ANALYZER DEFINITION BY HAND.  (Because there's no
*			machinery for reading rules yet -- we're building it here!)
* REF:	See DEV\RULES RULES\ANALYZER.SEQ and *.PAT .
********************************************/

void RFA::make_rfa(Ana &rfa)
{

Delt<Seqn> *delt;
Seqn *seqn;																		// 11/04/99 AM.
Algo *algo;

if (rfa.Verbose())															// 02/21/02 AM.
	*gout << _T("\n**********\nBUILDING RFA\n**********") << std::endl;

// Build sequence of passes for RFA.
rfa.makeSeq();

// PASS 1: Tokenize.
algo = new Tok();
delt = Seqn::makeDelt(algo, _T(""));
seqn = delt->getData();								// 11/04/99 AM.
seqn->setAlgoname(_T("tokenize"));					// 11/04/99 AM.
rfa.addSeq(delt);

// PASS 2: Line.
algo = new Line();
delt = Seqn::makeDelt(algo, _T(""));
seqn = delt->getData();								// 11/04/99 AM.
seqn->setAlgoname(_T("line"));							// 11/04/99 AM.
rfa.addSeq(delt);

// PASS 3: Retokenize.
// REF: RETOK.PAT
rfa_retok(rfa);

// PASS 4: Big tokens (comments and strings.)
rfa_bigtok(rfa);

// PASS 5: Zap commas!
// Note:	Don't care about them.  Simplifies list collecting.
rfa_excise(_T(","), _T("x_commas"), rfa);			// 11/20/98 AM.

// PASS 6: Components of rules files. eg _ARROW (<-).
rfa_components(rfa);

// PASS 7: Zap whitespace.
// REF: X_WHITE.PAT
rfa_excise(_T("_xWHITE"), _T("x_white"), rfa);			// 11/20/98 AM.
//rfa_x_white(rfa);									// 11/20/98 AM.

// PASS 8: Parenthesized lists of items.
rfa_list(rfa);

// PASS 9: Get an action with list of args.
rfa_action(rfa);

// PASS 10: Get a key = val pair.
rfa_pair(rfa);

// PASS 11: Recognize pairs.
// REF: PAIRS.PAT
rfa_pairs(rfa);

// PASS 12: Recognize elements.
// REF: ELEMENT.PAT
rfa_element(rfa);

// PASS 13: Recognize rules.
// REF: RULE.PAT
rfa_rule(rfa);

// PASS 14: Collect rules into lists.
// REF: RULE.PAT
rfa_rules(rfa);

// PASS 15: Collect actions into lists.
rfa_actions(rfa);

// PASS 15.5: Collect select nodes into a list.	// 11/30/98 AM.
rfa_nodes(rfa);

// PASS 15.55: Collect select MULTI into a list. // 03/25/99 AM.
rfa_multi(rfa);

// PASS 15.57: Collect select PATH into a list. // 10/19/99 AM.
rfa_path(rfa);

// PASS 15.6: Build a select region.				// 11/30/98 AM.
rfa_select(rfa);

// Pass 16: Build a rules file region.			// 11/24/98 AM.
rfa_region(rfa);

// Pass 17: Build regions.							// 11/24/98 AM.
rfa_regions(rfa);

// Pass 18: Build recursive pass region.		// 11/25/98 AM.
rfa_recurse(rfa);

// Pass 19: Build recurses.						// 11/25/98 AM.
rfa_recurses(rfa);

// Pass 19.5: Build code region.
rfa_code(rfa);											// 12/07/98 AM.

// Pass 20: Build a rules file object.			// 11/24/98 AM.
rfa_rulesfile(rfa);

// PASS 21: Traverse the parse tree, internalizing rules.
algo = new Intern();
delt = Seqn::makeDelt(algo, _T("nil"));	// Name rulesfile!	// 11/04/99 AM.
seqn = delt->getData();												// 11/04/99 AM.
seqn->setAlgoname(_T("intern"));										// 11/04/99 AM.
rfa.addSeq(delt);

// Used this for building the Pat algorithms.
//rfa_test(rfa);

/// END OF RFA PASSES /////

if (rfa.Verbose())
	{
	*gout << _T("rfa sequence = ") << std::endl;
	rfa.prettySeq();
	}
}

/********************************************
* FN:		execute_RFA
* CR:		10/21/98 AM.
* SUBJ:	Execute the RFA on the given rules file.
********************************************/

bool RFA::execute_rfa(
	_TCHAR *file,				// Rules file.
	Ana &rfa,				// Rules-file analyzer
	_TCHAR *specdir,			// Directory for pat files.				// 12/03/98 AM.
	Eana *erfa,				// Executable configuration.				// 10/13/99 AM.
	_TCHAR *algo,				// Algo of input file.						// 05/31/00 AM.
	long passnum,			// Pass number of input rule file.		// 08/23/02 AM.
	/*UP*/
	Ifile* &rulesfile,		// Internal rep of rules file.		// 11/25/98 AM.
	bool &intern			// If pass file interned ok.				// 05/26/01 AM.
	)
{
if (!file || !*file || !specdir || !*specdir || !erfa)			// 10/13/99 AM.
	return false;

bool verbose	= erfa->getFverbose();									// 10/13/99 AM.
bool debug		= erfa->getFdebug();										// 10/13/99 AM.

Parse parse;					// The current text analysis instance.
parse.setAna(&rfa);			// Analyzer that the parse will use.
parse.setNLP(rfa.getNLP());												// 06/30/03 AM.
parse.setHtab(rfa.getHtab());	// Tell parser which htab.			// 06/25/03 AM.

// Tell parse how to execute.												// 10/13/99 AM.
parse.setEana(erfa);															// 10/13/99 AM.
parse.setVerbose(verbose);	// Continue using this.					// 10/13/99 AM.
parse.setAlgo(algo);			// Algo of input file.					// 05/31/00 AM.
parse.setInputpass(passnum);	// For debug info.					// 08/23/02 AM.
PostRFA::setDebug(debug);													// 10/13/99 AM.

// Check on rules file for .PAT suffix.  If absent, add it.
_TCHAR *fileOrig = file;
if (!(file = rfa_fix_file_name(specdir, file, _T("nlp")))) {
	return false;
}

std::_t_ifstream x(TCHAR2A(file), std::ios::in);
if (!x) {
	if (!(file = rfa_fix_file_name(specdir, fileOrig, _T("pat")))) {
		return false;
	}
}

if (debug)					// 10/13/99 AM.
	*gout << _T("Rules file name=") << file << std::endl;

parse.setInput(file);		// Input rules file for the parse.
//delete [] file;				// 11/19/98 AM.
Chars::destroy(file);

if (debug)
	*gout << _T("\n\nRULES-FILE ANALYZER!") << std::endl;

//parse.setVerbose(rfa.Verbose());			// 10/13/99 AM.
if (verbose)
	{
	*gout << _T("\n*******************************")
	     << _T("\n             RULES FILE")
		  << _T("\n*******************************") << std::endl;
	}
if (!parse.readFile())				// READ RULES FILE TO BUFFER.
	return false;
	
// Don't output individual passes in analyzing the rules file.	// 12/26/98 AM.
//bool savev;						// 12/26/98 AM.	// 10/13/99 AM.
//savev = parse.Verbose();	// 12/26/98 AM.		// 10/13/99 AM.
//parse.setVerbose(false);	// 12/26/98 AM.		// 10/13/99 AM.

parse.execute();			// TEXT-ANALYZE THE RULES FILE.
								// CONVERT PARSE TREE TO INTERPRETED INTERNAL FORM.

// RECORD IF SYNTAX ERRORS FOUND IN PARSE OF PASS FILE.			// 05/26/01 AM.
intern = parse.getIntern();												// 05/26/01 AM.

rulesfile = rfa_output(parse);		// GET THE INTERNALIZED RULESFILE.
												// (UP to caller).  // 11/25/98 AM.

//parse.setVerbose(savev);		// 10/13/99 AM.
if (verbose)						// 10/13/99 AM.
	{
	*gout << _T("\n********************************")
	     << _T("\nFINAL PARSE TREE FOR RULES FILE:")
		  << _T("\n********************************") << std::endl;
	((Tree<Pn> *)parse.getTree())->Dump();
	}

// CLEAN TEXT-ANALYSIS OF RULES FILE.
if (debug)
	*gout << _T("Cleaning up analysis of rules file.") << std::endl;

parse.clean();

return true;							// Success.
}


/********************************************
* FN:		RFA_FIX_FILE_NAME
* CR:		10/23/98 AM.
* SUBJ:	Fix up rules file name.
* ALLOC:	ALLOCATES A CHAR BUFFER.
* NOTE:	12/03/98 AM. Added specdir.
********************************************/

_TCHAR *RFA::rfa_fix_file_name(
	_TCHAR *specdir,
	_TCHAR *file,
	_TCHAR *suff
	)
{
int len;
len = _tcsclen(specdir) + _tcsclen(file) + _tcsclen(suff) + 3;
				// One for slash, period, and \0.
_TCHAR *buf;
//buf = new char[len];
buf = Chars::create(len);	// 11/19/98 AM.
*buf = '\0';					// Zero the buffer.
_stprintf(buf, _T("%s%c%s"), specdir, DIR_CH, file);		// 12/03/98 AM.
//strcat(buf, file);								// 12/03/98 AM.
if (!fix_file_name(buf, suff))
	return 0;
return buf;
}


/********************************************
* FN:		RFA_OUTPUT
* CR:		11/11/98 AM.
* SUBJ:	Get the rules built by the RFA.
********************************************/

Ifile *RFA::rfa_output(Parse &parse)
{
Tree<Pn> *tree = (Tree<Pn> *)parse.getTree();

if (!tree)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: No parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Node<Pn> *root;
root = tree->getRoot();

if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: Empty parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// 11/11/98 AM.
// Just grab the list of rules under the root of the tree.
Node<Pn> *node;
if (!(node = root->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: No rules in tree.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Pn *pn;
pn = node->getData();
assert(pn);
RFASem *sem;
sem = (RFASem *) pn->getSem();
if (!sem)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: No semantics for parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (sem->getType() != RSFILE)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: Couldn't parse rules file.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Ifile *rulesfile;
if (!(rulesfile = sem->getRulesfile()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: No rules found in file.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (!sem->getRules())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rfa_output: No rules.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Move the rulefile semantic object from the parse tree.
sem->setRulesfile(0);
delete sem;
pn->setSem(0);

//if (parse.Verbose())						// 10/13/99 AM.
if (parse.getEana()->getFdebug())		// 10/13/99 AM.
	*gout << _T("[rfa_output: Attached rules to current pass.]") << std::endl;

// Only return the rules when rules file recognized, and when all that
// is removed from the parse tree.
return rulesfile;			// 11/25/98 AM.
}


/********************************************
* FN:		RFA_RETOK
* CR:		11/03/98 AM.
* SUBJ:	Build the retok pass for the RFA.
********************************************/

void RFA::rfa_retok(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
//Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Dlist<Iarg> *attrs;
Iarg *arg;

/////////////////
// 10/21/99 AM. Building sentinel rule to keep from trying all the
// subsequent rules, as long as there's no backslash around.
/////////////////


Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Ielt *elt;
Delt<Ielt> *relt;						// Rule elt.
Dlist<Iarg> *fails;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  :	@POST
//					noop()
//				@RULES
//					_xNIL <- _xWILD [fail=( \\ )] @@
///////////////////////////////////////

_TCHAR *func;										// Action name or function.
func = _T("noop");
posts = Iaction::makeDlist(func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);		// Add element to phrase.
elt = relt->getData();
arg = new Iarg(_T("\\"));
fails = new Dlist<Iarg>();
fails->rpush(arg);
elt->setFails(fails);

// Create and attach list of pairs.

sugg = new Isugg(_T("_xNIL"));					// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

/////////////////
pres = 0;
posts = 0;
trig = 0;
attrs = 0;
arg = 0;


// RULE  5:   _LIT <- \\ \< @@
rfa_rule_bs_lit(_T("<"), _T("_cLANGLE"), rules);			// 11/19/98 AM.

// RULE  6:   _LIT <- \\ \> @@
rfa_rule_bs_lit(_T(">"), _T("_cRANGLE"), rules);			// 11/19/98 AM.

// RULE  7:   _LIT <- \\ \  @@    # SINGLE SPACE
rfa_rule_bs_lit(_T(" "), _T("_cSPACE"), rules);			// 11/19/98 AM.

// A GAGGLE OF RULES (ref retok.pat)	// 11/19/98 AM.

// The alphabetics are special.  They need a rename.
rfa_rule_bs_lit(_T("n"), _T("_cLF"),  rules, _T("\n"));
rfa_rule_bs_lit(_T("r"), _T("_cCR"),  rules, _T("\r"));
rfa_rule_bs_lit(_T("a"), _T("_cBEL"), rules, _T("\a"));
rfa_rule_bs_lit(_T("b"), _T("_cBS"),  rules, _T("\b"));
rfa_rule_bs_lit(_T("f"), _T("_cFF"),  rules, _T("\f"));
rfa_rule_bs_lit(_T("t"), _T("_cHT"),  rules, _T("\t"));
rfa_rule_bs_lit(_T("v"), _T("_cVT"),  rules, _T("\v"));
	
//rfa_rule_bs_lit(" ", "_cSPACE",		rules);
rfa_rule_bs_lit(_T("\'"), _T("_cSQUOTE"),	rules);
rfa_rule_bs_lit(_T("\""), _T("_cDQUOTE"),	rules);
rfa_rule_bs_lit(_T("?"), _T("_cQMARK"),		rules);
rfa_rule_bs_lit(_T("!"), _T("_cBANG"),		rules);
rfa_rule_bs_lit(_T("#"), _T("_cPOUND"),		rules);
rfa_rule_bs_lit(_T("$"), _T("_cDOLLAR"),		rules);
rfa_rule_bs_lit(_T("%"), _T("_cPERCENT"),	rules);
rfa_rule_bs_lit(_T("&"), _T("_cAMPERSAND"),	rules);
rfa_rule_bs_lit(_T("("), _T("_cLPAR"),		rules);
rfa_rule_bs_lit(_T(")"), _T("_cRPAR"),		rules);
rfa_rule_bs_lit(_T("*"), _T("_cASTERISK"),	rules);
rfa_rule_bs_lit(_T("+"), _T("_cPLUS"),		rules);
rfa_rule_bs_lit(_T(","), _T("_cCOMMA"),		rules);
rfa_rule_bs_lit(_T("-"), _T("_cDASH"),		rules);
rfa_rule_bs_lit(_T("."), _T("_cPERIOD"),		rules);
rfa_rule_bs_lit(_T("/"), _T("_cSLASH"),		rules);
rfa_rule_bs_lit(_T(":"), _T("_cCOLON"),		rules);
rfa_rule_bs_lit(_T(";"), _T("_cSEMICOLON"),	rules);
rfa_rule_bs_lit(_T("="), _T("_cEQUAL"),		rules);
rfa_rule_bs_lit(_T("@"), _T("_cATSIGN"),		rules);
rfa_rule_bs_lit(_T("["), _T("_cLBRACKET"),	rules);
rfa_rule_bs_lit(_T("]"), _T("_cRBRACKET"),	rules);
rfa_rule_bs_lit(_T("^"), _T("_cCARET"),		rules);
rfa_rule_bs_lit(_T("_"), _T("_cUNDERSCORE"),	rules);
rfa_rule_bs_lit(_T("`"), _T("_cBACKQUOTE"),	rules);
rfa_rule_bs_lit(_T("{"), _T("_cLBRACE"),		rules);
rfa_rule_bs_lit(_T("}"), _T("_cRBRACE"),		rules);
rfa_rule_bs_lit(_T("|"), _T("_cVBAR"),		rules);
rfa_rule_bs_lit(_T("~"), _T("_cTILDE"),		rules);
rfa_rule_bs_lit(_T("\\"), _T("_cBSLASH"),	rules);	// 10/25/99 AM.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	Irule::genRuleblocks(rules, _T(" "), *gout);					// 11/04/99 AM.
	//*gout << *rules;												// 11/04/99 AM.
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("retok"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_RULE_BS_LIT
* CR:		11/19/98 AM.
* SUBJ:	Build a single rule for backslashed literal.
* FORM:	    _LIT <- <SUGG>  <-  \\   <CH>  @@
* NOTE:	This is a layered rule.
*			Rather than renaming for the alphabetics, should
*			have their own action.
********************************************/

void RFA::rfa_rule_bs_lit(
	_TCHAR *chstr,						// The char as a string. (eg, ">").
	_TCHAR *suggstr,						// The suggested name (eg, "_cRANGLE").
	Dlist<Irule> *rules,				// Current rules list being built.
	_TCHAR *rename						// Rename 2nd node.
				// eg,  _cLF <- \\ n [rename="\n"] @@
	)
{
// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
Dlist<Iarg> *attrs;
Iarg *arg;

pres = 0;
posts = 0;
trig = 0;
attrs = 0;
arg = 0;

_TCHAR *func;				// Action name or function.
Dlist<Iarg> *args;

// Add semantic action.
func = _T("rfaname");
arg = new Iarg(_T("2"));					// Take the <CH> text.
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

// Putting LAYERING in this rule.
attrs = new Dlist<Iarg>();				// Empty list of attrs (ie layers).
arg = new Iarg(_T("_LIT"));
attrs->rpush(arg);
//delete arg;		// 07/02/99 AM.

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("\\"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, chstr,0,1,1, rename);	// Add elt to phrase.
// Create and attach list of pairs.
sugg = new Isugg(suggstr);				// Create suggested elt.
sugg->setAttrs(attrs);						// Add layering.			// 11/07/98 AM.
sugg->setBase(true);							// 11/07/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);
}


/********************************************
* FN:		RFA_BIGTOK
* CR:		11/19/98 AM.
* SUBJ:	Handle "big tokens" like comments and strings.
********************************************/

void RFA::rfa_bigtok(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;
Iarg *arg1, *arg2;
Dlist<Iarg> *matches;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:	@POST
//					excise(1, 2)	# Range of rule elts from 1st to 2nd.
//				@RULES
//					_xNIL <- \# _xWILD _xWILD [match=(\n _xEOF)] @@
///////////////////////////////////////

// Recognize and remove comments.  Retain the newline.

_TCHAR *func;										// Action name or function.
func = _T("excise");
Iarg *start, *end;
start = new Iarg(_T("1"));
end   = new Iarg(_T("2"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(start);
//delete start;		// 07/02/99 AM.
args->rpush(end);
//delete end;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("#"),0,1,1);			// Add element to phrase.
Ielt::addDelt(phr, _T("_xWILD"),0,0,0);		// Add element to phrase.

// 11/25/98 AM. Trying an OR with end-of-file here!
//Ielt::addDelt(phr, "\n",0,1,1);			// Add element to phrase.
relt = Ielt::addDelt(phr, _T("_xWILD"),0,1,1);		// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("\n"));
arg2 = new Iarg(_T("_xEOF"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
//delete arg1;			// 07/02/99 AM.
//delete arg2;			// 07/02/99 AM.
elt->setMatches(matches);

// Create and attach list of pairs.

sugg = new Isugg(_T("_xNIL"));					// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
// RULE  2:	@POST
//					rfastr(2)		# sem action for making string.
//             single()			# normal (single-tier) reduce.
//				@RULES
//					_STR <- \" _xWILD \" @@
///////////////////////////////////////
func = _T("rfastr");
Iarg *arg;
arg = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("\""),0,1,1);			// Add element to phrase.
Ielt::addDelt(phr, _T("_xWILD"),0,0,0);		// Add element to phrase.
Ielt::addDelt(phr, _T("\""),0,1,1);			// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_STR"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}


// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("bigtok"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_COMPONENTS
* CR:		11/20/98 AM.
* SUBJ:	Build the components of a rules file for the RFA.
********************************************/

void RFA::rfa_components(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
Dlist<Iarg> *attrs;
Iarg *arg;

pres = 0;
posts = 0;
trig = 0;
attrs = 0;
arg = 0;

_TCHAR *func;				// Action name or function.
Dlist<Iarg> *args;

///////////////////////////////////////
// GENERAL RULES
///////////////////////////////////////

///////////////////////////////////////
// RULE  1:   _LIT <- _xALPHA @@
///////////////////////////////////////

// Means that an alphabetic token is a literal.
// For example, the word "IBM" is a literal token in a rules file.

// Need an action to fill Sem object with name.
// Form:  postRFAname("1");
//				where references the alphabetic token.

func = _T("rfaname");
arg = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("_xALPHA"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_LIT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  2:   _NONLIT <- \_ _xALPHA @@
///////////////////////////////////////

// Add semantic action.				// 11/10/98 AM.
func = _T("rfanonlit");
arg = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("_"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, _T("_xALPHA"),0,1,1);	// Add elt to phrase.
// Create and attach list of pairs.
sugg = new Isugg(_T("_NONLIT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);

///////////////////////////////////////
// RULE  3:   _ARROW <- \< \- @@
///////////////////////////////////////

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("<"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, _T("-"),0,1,1);	// Add elt to phrase.
// Create and attach list of pairs.
sugg = new Isugg(_T("_ARROW"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, 0, trig);

///////////////////////////////////////
// RULE  5:   _NUM <- _xNUM @@
///////////////////////////////////////
// 11/21/98 AM.
// Recognize numeric tokens as atoms within a rules file.
// Place number in semantic structure.
// Form:  postRFAnum("1");
//				where "1" references the numeric token.

func = _T("rfanum");
arg = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("_xNUM"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_NUM"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  4: _ENDRULE <- \@ \@ _xWHITE @@
///////////////////////////////////////
// 10/22/99 AM. Building a variant to catch the end of rule before trying
// any other @@XXX rule.
// WARN: Not worrying about @@ followed by end of file, for now.
// (Actually, repeat of rule at the bottom of this function handles it.)

trig = 0;
phr = new Dlist<Ielt>();						// Create rule phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, _T("_xWHITE"),0,1,1);		// 10/22/99 AM.
// Create and attach list of pairs.
sugg = new Isugg(_T("_ENDRULE"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, 0, trig);

/////////////////
// 10/22/99 AM. Building sentinel rule to keep from trying all the
// subsequent rules, as long as there's no backslash around.
/////////////////

Ielt *elt;
Delt<Ielt> *relt;						// Rule elt.
Dlist<Iarg> *fails;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  :	@POST
//					noop()
//				@RULES
//					_xNIL <- _xWILD [one fail=( \@ )] @@
///////////////////////////////////////
// NOTE: WILD MUST MATCH ONLY ONE ELT.

func = _T("noop");
posts = Iaction::makeDlist(func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
relt = Ielt::addDelt(phr, _T("_xWILD"),0,1,1);		// Add element to phrase.
elt = relt->getData();
arg = new Iarg(_T("@"));
fails = new Dlist<Iarg>();
fails->rpush(arg);
elt->setFails(fails);

// Create and attach list of pairs.

sugg = new Isugg(_T("_xNIL"));					// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

/////////////////

///////////////////////////////////////
// KEYWORD RULES			// 11/23/98 AM.
///////////////////////////////////////

rfa_rule_key(_T("RULES"),   _T("_soRULES"),   rules);
rfa_rule_key(_T("POST"),    _T("_soPOST"),    rules);
rfa_rule_key(_T("CHECK"),	_T("_soCHECK"),	  rules);
rfa_rule_key(_T("PRE"),     _T("_soPRE"),     rules);
rfa_rule_key(_T("NODES"),   _T("_soNODES"),   rules);			// 11/30/98 AM.
rfa_rule_key(_T("MULTI"),   _T("_soMULTI"),   rules);			// 03/25/99 AM.
rfa_rule_key(_T("PATH"),    _T("_soPATH"),    rules);			// 10/19/99 AM.
rfa_rule_key(_T("CODE"),    _T("_soCODE"),    rules);			// 12/07/98 AM.
rfa_rule_key(_T("SELECT"),  _T("_soSELECT"),  rules);			// 11/30/98 AM.
rfa_rule_key(_T("RECURSE"), _T("_soRECURSE"), rules);

rfa_rule_endkey(_T("POST"),    _T("_eoPOST"),    rules);
rfa_rule_endkey(_T("CHECK"),	_T("_eoCHECK"),   rules);
rfa_rule_endkey(_T("PRE"),     _T("_eoPRE"),     rules);
rfa_rule_endkey(_T("RULES"),   _T("_eoRULES"),   rules);
rfa_rule_endkey(_T("RECURSE"), _T("_eoRECURSE"), rules);
rfa_rule_endkey(_T("SELECT"),  _T("_eoSELECT"),  rules);		// 11/30/98 AM.
rfa_rule_endkey(_T("NODES"),   _T("_eoNODES"),   rules);		// 11/30/98 AM.
rfa_rule_endkey(_T("MULTI"),   _T("_eoMULTI"),   rules);		// 03/25/99 AM.
rfa_rule_endkey(_T("PATH"),    _T("_eoPATH"),    rules);		// 10/19/99 AM.
rfa_rule_endkey(_T("CODE"),    _T("_eoCODE"),    rules);		// 12/07/98 AM.

///////////////////////////////////////
// RULE  4:   _ENDRULE <- \@ \@ @@
///////////////////////////////////////

trig = 0;
phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);	// Add elt to phrase.
// Create and attach list of pairs.
sugg = new Isugg(_T("_ENDRULE"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, 0, trig);

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("components"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_RULE_KEY
* CR:		11/23/98 AM.
* SUBJ:	Build a single rule for rules file keyword.
* FORM:	    <_KEY>  <-  \@   <KEYWORD>  @@
********************************************/

void RFA::rfa_rule_key(
	_TCHAR *keystr,						// The keyword.
	_TCHAR *suggstr,						// The suggested name (eg, "_POST").
	Dlist<Irule> *rules				// Current rules list being built.
	)
{
// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
Dlist<Iarg> *attrs;
Iarg *arg;

pres = 0;
posts = 0;
trig = 0;
attrs = 0;
arg = 0;

//char *func;				// Action name or function.
//Dlist<Iarg> *args;
// Default action.
// Should have an ignore case precondition.

phr = new Dlist<Ielt>();						// Create rule phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, keystr,0,1,1);			// Add elt to phrase.
// Create and attach list of pairs.
sugg = new Isugg(suggstr);						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);
}


/********************************************
* FN:		RFA_RULE_ENDKEY
* CR:		11/23/98 AM.
* SUBJ:	Build a single rule for rules file keyword.
* FORM:	    <_KEY>  <-  \@ \@ <KEYWORD>  @@
********************************************/

void RFA::rfa_rule_endkey(
	_TCHAR *keystr,						// The keyword.
	_TCHAR *suggstr,						// The suggested name (eg, "_POST").
	Dlist<Irule> *rules				// Current rules list being built.
	)
{
// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
Dlist<Iarg> *attrs;
Iarg *arg;

pres = 0;
posts = 0;
trig = 0;
attrs = 0;
arg = 0;

//char *func;				// Action name or function.
//Dlist<Iarg> *args;
// Default action.
// Should have an ignore case precondition.

phr = new Dlist<Ielt>();						// Create rule phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, _T("@"),0,1,1);				// Add elt to phrase.
Ielt::addDelt(phr, keystr,0,1,1);			// Add elt to phrase.
// Create and attach list of pairs.
sugg = new Isugg(suggstr);						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);
}


/********************************************
* FN:		RFA_LISTARG
* CR:		11/21/98 AM.
* SUBJ:	RECURSIVE ruleset for fixing up list arguments.
* NOTE:	Rules used by the wildcard element of the _LIST rule.
* RET:	A pass object containing the listarg built.
********************************************/

Seqn *RFA::rfa_listarg(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for list arguments.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _ARG <- _NONLIT @@
///////////////////////////////////////

// To convert random nonliterals into list args, in the appropriate
// context.  Perform semantics for this also.
// Form:  postRFAnonlitarg(1);
//				where 1 is _nonlit

func = _T("rfaarg");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_NONLIT"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ARG"));					// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  2:   _ARG <- _LIT @@
///////////////////////////////////////

// Form:  postRFAlitarg(1);

func = _T("rfaarg");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ARG"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  3:   _ARG <- _STR @@
///////////////////////////////////////

// Form:  postRFAstrarg(1);

func = _T("rfaarg");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_STR"),0,1,1);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ARG"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  4:   _ARG <- _NUM @@
///////////////////////////////////////

// Form:  postRFAnumarg(1);

func = _T("rfaarg");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_NUM"),0,1,1);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ARG"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!
Seqn *pass;
pass = new Seqn(0,0, rules, _T("listarg"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_LIST
* CR:		11/20/98 AM.
* SUBJ:	RFA pass for parenthesized list.
* NOTE:	This is a component of both actions (pre & post) and pairs.
********************************************/

void RFA::rfa_list(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

Dlist<Iarg> *matches;
Dlist<Seqn> *passes;					// Passes to RECURSE on.
Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3, *arg4;
_TCHAR *func;				// Action name or function.
Iarg *arg;
Dlist<Iarg> *args;

// BUILD RECURSIVE RULESETS FOR LIST ARGUMENTS.
passes = new Dlist<Seqn>();
pass = rfa_listarg(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;

///////////////////////////////////////
// RULE  1:   _LIST <- \( _xWILD \) @@
///////////////////////////////////////
// Action:  postRFAlist(2);
//				where 2 is the nodes matching the wildcard.

func = _T("rfalist");
arg = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("("),0,1,1);			// Add element to phrase.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);		// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_NONLIT"));
arg3 = new Iarg(_T("_STR"));
arg4 = new Iarg(_T("_NUM"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
matches->rpush(arg3);
matches->rpush(arg4);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
//delete arg4;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(passes);

Ielt::addDelt(phr, _T(")"),0,1,1);			// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_LIST"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  2:   _PREPAIR <- \< _NUM _NUM \> @@
///////////////////////////////////////
// NOTE! Commas are gone from tree!

// Action:  rfarange(2,3)
//				where 2,3 are the rule elements for numbers.

func = _T("rfarange");
arg1 = new Iarg(_T("2"));
arg2 = new Iarg(_T("3"));
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;	// 07/02/99 AM.
//delete arg2;	// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();						// Create rule phrase.
Ielt::addDelt(phr, _T("<"),0,1,1);				// Add element to phrase.
Ielt::addDelt(phr, _T("_NUM"),0,1,1);			// Add element to phrase.
Ielt::addDelt(phr, _T("_NUM"),0,1,1);			// Add element to phrase.
Ielt::addDelt(phr, _T(">"),0,1,1);				// Add element to phrase.

sugg = new Isugg(_T("_PREPAIR"));
// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("list"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_ARGTOLIST
* CR:		11/21/98 AM.
* SUBJ:	RECURSIVE ruleset for fixing up singleton pair value.
* RET:	A pass object containing the ruleset built.
********************************************/

Seqn *RFA::rfa_argtolist(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for a pair's singleton value.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _LIST <- _ARG @@
///////////////////////////////////////

// Convert a singleton argument to a list.
// Semantic actions:
// Form:  postRFAargtolist(1);
//				where 1 is _arg

func = _T("rfaargtolist");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_ARG"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_LIST"));					// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
//algo = new Pat();
//algo->setDebug(RFA::Debug());
//delt = Seqn::makeDelt(algo, "element", rules);
//rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!
Seqn *pass;
pass = new Seqn(0,0, rules, _T("argtolist"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_ACTION
* CR:		11/23/98 AM.
* SUBJ:	RFA pass for an action.
* RULES:	_ACTION <- _LIT _LIST @@
*			_ACTION <- _LIT \( \) @@
* NOTE:	Empty list could be handled as part of _LIST.
*			May allow action without arg list in the future (to be
*			handled recursively in lists of actions).
********************************************/

void RFA::rfa_action(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3;
_TCHAR *func;				// Action name or function.
Dlist<Iarg> *args;

///////////////////////////////////////
// RULE  1:   _ACTION <- _PREPAIR [opt] _LIT _LIST [trigger] @@
///////////////////////////////////////
// Action:  postRFAaction(1,2,3);

// POST ACTIONS FOR RULE.
func = _T("rfaaction");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("2"));
arg3 = new Iarg(_T("3"));
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
args->rpush(arg3);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

////////////

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_PREPAIR"),0,0,1);			// _PREPAIR [opt]
Ielt::addDelt(phr, _T("_LIT"),0,1,1);				// Add element to phrase.
trig = Ielt::addDelt(phr, _T("_LIST"),0,1,1);		// Add element to phrase.
Ielt *tr = trig->getData();						// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.

sugg = new Isugg(_T("_ACTION"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts,trig); // Create and add rule to list.
trig = 0;
posts = 0;

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("action"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_PAIR
* CR:		11/21/98 AM.
* SUBJ:	RFA pass for a key = val pair.
* NOTE:	A "pair" can also be a lone key with no value, (eg, TRIGGER).
*			Those are handled when a list of pairs is gathered (rfa_pairs).
********************************************/

void RFA::rfa_pair(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

Dlist<Iarg> *matches;
Dlist<Seqn> *passes;					// Passes to RECURSE on.
Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3, *arg4, *arg5;
_TCHAR *func;				// Action name or function.

// BUILD RECURSIVE RULESETS FOR LIST ARGUMENTS.
// Build a list, if val is a singleton value.
passes = new Dlist<Seqn>();
pass = rfa_listarg(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;

pass = rfa_argtolist(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;


///////////////////////////////////////
// RULE  1:   _PAIR <- _LIT \= _xWILD [min=1 max=1] @@
///////////////////////////////////////
// Action:  postRFApair(1,3);
//				where 1 is the key and 3 is the val.

// CREATE RECURSIVE RULE FOR
// _KEY <- _LIT  @@

// Recursive rules to gather the value.
// _VAL <- _LIST @@
// (List also comes from parenthesized list of args. see rfa_list).
// _LIST <- _ARG @@	# To handle a single unparenthesized arg.
// _ARG  <- lit,nonlit,num,arg.


// POST ACTIONS FOR RULE.
func = _T("rfapair");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("3"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

////////////

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);		// Add element to phrase.
// OPT // 10/06/99 AM. Making the equal sign the trigger.
trig = Ielt::addDelt(phr, _T("="),0,1,1);			// Add element to phrase.
Ielt *tr = trig->getData();						// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,1,1);		// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_NONLIT"));
arg3 = new Iarg(_T("_STR"));
arg4 = new Iarg(_T("_NUM"));
arg5 = new Iarg(_T("_LIST"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
matches->rpush(arg3);
matches->rpush(arg4);
matches->rpush(arg5);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
//delete arg4;		// 07/02/99 AM.
//delete arg5;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(passes);
// Create and attach list of pairs.

sugg = new Isugg(_T("_PAIR"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("pair"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_LITTOPAIR
* CR:		11/22/98 AM.
* SUBJ:	RECURSIVE ruleset for converting a literal to a pair.
* RET:	A pass object containing the ruleset built.
* NOTE:	Operates in the context of a bracketed list of pairs.
********************************************/

Seqn *RFA::rfa_littopair(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for creating a pair.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _PAIR <- _LIT @@
///////////////////////////////////////
// eg,  [trigger]

// Convert a literal to a pair.
// Semantic actions:
// Form:  postRFAlittopair(1);
//				where 1 is _lit

func = _T("rfalittopair");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_PAIR"));					// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

Seqn *pass;
pass = new Seqn(0,0, rules, _T("littopair"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_PAIRS
* CR:		11/06/98 AM.
* SUBJ:	RFA pass for recognizing key=val pairs.
* NOTE:	11/22/98 AM. Revising this.
********************************************/

void RFA::rfa_pairs(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

Dlist<Iarg> *matches;
Dlist<Seqn> *passes;					// Passes to RECURSE on.
Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3;
_TCHAR *func;				// Action name or function.

// BUILD RECURSIVE RULESETS FOR PAIR VARIATIONS.
// Recursive rules here to convert _LIT to a pair.
// Could handle all types of shorthand.
// eg, [1,1], [+] [*<] [trig]
passes = new Dlist<Seqn>();
pass = rfa_littopair(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;

///////////////////////////////////////
// RULE  1:   _PAIRS <- \[ _xWILD \] @@
///////////////////////////////////////
// Quick-and-dirty rule for now.

// Need an action to fill Sem class with pairs.
// Form:  postRFApairs(2);
//				where 2 is the nodes matching the wildcard.

func = _T("rfapairs");
Iarg *arg;
arg = new Iarg(_T("2"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("["),0,1,1);			// Add element to phrase.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);	// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_PAIR"));
arg3 = new Iarg(_T("*"));						// 03/22/99 AM.
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
matches->rpush(arg3);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(passes);

Ielt::addDelt(phr, _T("]"),0,1,1);			// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_PAIRS"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("pairs"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_ELEMENT
* CR:		11/06/98 AM.
* SUBJ:	RFA pass for recognizing rule element.
* NOTE:	Can become a rule phrase elt or a sugg.
********************************************/

void RFA::rfa_element(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1, *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _ELEMENT <- _NONLIT _PAIRS @@
///////////////////////////////////////

// To prevent all _LIT and _NONLIT from becoming _ELEMENT,
// requiring that _PAIRS be present.

// Need an action to fill Sem class with element.
// Form:  postRFAelement(1,2);
//				where 1 is _nonlit and 2 is _pairs.

func = _T("rfaelement");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_NONLIT"),0,1,1);	// Add element to phrase.
// OPT // 10/06/99 AM.
trig = Ielt::addDelt(phr, _T("_PAIRS"),0,1,1);		// Add element to phrase.
Ielt *tr = trig->getData();						// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ELEMENT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);	// Create and add rule to list.

///////////////////////////////////////
// RULE  2:   _ELEMENT <- _LIT _PAIRS @@
///////////////////////////////////////

// Need an action to fill Sem class with element.
// Form:  postRFAelement(1,2);
//				where 1 is _nonlit and 2 is _pairs.

func = _T("rfaelement");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);		// Add element to phrase.
// OPT // 10/06/99 AM.
trig = Ielt::addDelt(phr, _T("_PAIRS"),0,1,1);		// Add element to phrase.
tr = trig->getData();								// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ELEMENT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);	// Create and add rule to list.

///////////////////////////////////////
// RULE  3:   _ELEMENT <- _NUM _PAIRS @@
///////////////////////////////////////
// Handling number literals.  // 06/04/99 AM.

// Need an action to fill Sem class with element.
// Form:  postRFAelement(1,2);
//				where 1 is _nonlit and 2 is _pairs.

func = _T("rfaelement");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);
trig = 0;			// 10/06/99 AM.

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_NUM"),0,1,1);		// Add element to phrase.
trig = Ielt::addDelt(phr, _T("_PAIRS"),0,1,1);		// Add element to phrase.
tr = trig->getData();								// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.

// Create and attach list of pairs.

sugg = new Isugg(_T("_ELEMENT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("element"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_RULELT
* CR:		11/08/98 AM.
* SUBJ:	RECURSIVE ruleset for fixing up rule elements.
* NOTE:	Rules used by the wildcard element of the _RULE rule.
* RET:	A pass object containing the ruleset built.  11/09/98 AM.
********************************************/

Seqn *RFA::rfa_rulelt(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for fixing up rule elements in a wildcard.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _ELEMENT <- _NONLIT @@
///////////////////////////////////////

// To convert random nonliterals into rule elements, in the appropriate
// context.  Also, to conveniently perform semantic actions for this.

// Need an action to fill Sem class with element.
// Form:  postRFAnonlitelt(1);
//				where 1 is _nonlit

func = _T("rfanonlitelt");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_NONLIT"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ELEMENT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  2:   _ELEMENT <- _LIT @@
///////////////////////////////////////

// Need an action to fill Sem class with element.
// Form:  postRFAlitelement(1,2);
//				where 1 is _nonlit and 2 is _pairs.

func = _T("rfalitelt");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ELEMENT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
// RULE  3:   _ELEMENT <- _NUM @@
///////////////////////////////////////

// Handling number literals.  // 06/04/99 AM.
// Need an action to fill Sem class with element.
// Form:  postRFAlitelement(1,2);
//				where 1 is _nonlit and 2 is _pairs.

func = _T("rfalitelt");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_NUM"),0,1,1);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ELEMENT"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
//algo = new Pat();
//algo->setDebug(RFA::Debug());
//delt = Seqn::makeDelt(algo, "element", rules);
//rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!
Seqn *pass;
pass = new Seqn(0,0, rules, _T("rulelt"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_ELT
* CR:		11/10/98 AM.
* SUBJ:	RECURSIVE ruleset for fixing up rule elements.
* NOTE:	Rules used by the wildcard element of the _RULE rule.
* RET:	A pass object containing the ruleset built.
********************************************/

Seqn *RFA::rfa_elt(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for phrase elements in a wildcard.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _ELT <- _ELEMENT @@
///////////////////////////////////////

// To convert rule elements (either elt or sugg) specifically to elements
// of the rule's phrase (NOT the suggested element).

// Semantic actions:
// Form:  postRFAelt(1);
//				where 1 is _element

func = _T("rfaelt");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_ELEMENT"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ELT"));					// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
//algo = new Pat();
//algo->setDebug(RFA::Debug());
//delt = Seqn::makeDelt(algo, "element", rules);
//rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!
Seqn *pass;
pass = new Seqn(0,0, rules, _T("elt"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_RULELTS
* CR:		11/09/98 AM.
* SUBJ:	RECURSIVE pass for fixing up rule elements.
* NOTE:	Passes used by the wildcard element of the _RULE rule.
* RULE:	  _elements <- _element [+] @@
* RET:	A pass object containing the ruleset built.  11/09/98 AM.
********************************************/

Seqn *RFA::rfa_rulelts(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive pass for collecting rule elts in a wildcard.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _PHRASE <- _ELEMENT [+] @@
///////////////////////////////////////

// To collect rule elements into a list and perform appropriate semantics.

// Need an action to fill Sem class with element.
// Form:  postRFArulelts(1);
//				where 1 is _element

func = _T("rfarulelts");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_ELT"),0,1,0);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_PHRASE"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
//algo = new Pat();
//algo->setDebug(RFA::Debug());
//delt = Seqn::makeDelt(algo, "element", rules);
//rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!
Seqn *pass;
pass = new Seqn(0,0, rules, _T("rulelts"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_SUGG
* CR:		11/09/98 AM.
* SUBJ:	RECURSIVE pass for fixing up suggested rule element.
* NOTE:	Passes used by first wildcard element of the _RULE rule.
* RULE:	  _sugg <- _element @@
* RET:	A pass object containing the rules built.
********************************************/

Seqn *RFA::rfa_sugg(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive pass for suggested element.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _SUGG <- _ELEMENT @@
///////////////////////////////////////

// To collect rule elements into a list and perform appropriate semantics.

// Need an action to fill Sem class with element.
// Form:  postRFAsugg(1);
//				where 1 is _element

func = _T("rfasugg");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
Ielt::addDelt(phr, _T("_ELEMENT"),0,1,1);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_SUGG"));				// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
//algo = new Pat();
//algo->setDebug(RFA::Debug());
//delt = Seqn::makeDelt(algo, "element", rules);
//rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!
Seqn *pass;
pass = new Seqn(0,0, rules, _T("sugg"));	// Name recurse.	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_RULE
* CR:		11/05/98 AM.
* SUBJ:	RFA pass for recognizing single rules.
* NOTE:	Simplified rules without key=val pairs.
********************************************/

void RFA::rfa_rule(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;
_TCHAR *func;				// Action name or function.
Dlist<Iarg> *args;
Dlist<Iarg> *matches;				// List of matches.	// 11/09/98 AM.
//Dlist<Iarg> *fails;					// List of fails.		// 11/09/98 AM.
Iarg *arg1, *arg2, *arg3;

pres = 0;
posts = 0;
trig = 0;

// BUILD RECURSIVE RULESETS FOR SUGGESTED ELT.
Dlist<Seqn> *spasses;
Seqn *spass;
spasses = new Dlist<Seqn>();
spass = rfa_rulelt(rfa);
// 07/02/99 AM. No longer copies.
spasses->rpush(spass);
//spass->setAlgo(0);			// 11/17/98 AM.
//spass->setRules(0);			// 11/17/98 AM.
//delete spass;					// 11/17/98 AM.

spass = rfa_sugg(rfa);
// 07/02/99 AM. No longer copies.
spasses->rpush(spass);
//spass->setAlgo(0);			// 11/17/98 AM.
//spass->setRules(0);			// 11/17/98 AM.
//delete spass;					// 11/17/98 AM.

// BUILD RECURSIVE RULESETS FOR WILDCARD ELT. (reusing Seqn).
//Dlist<Irule> *rs_rulelt;			// 11/08/98 AM.
//rs_rulelt = rfa_rulelt();		// 11/08/98 AM.
Dlist<Seqn> *passes;					// Passes to RECURSE on.		// 11/09/98 AM.
Seqn *pass;								// A single pass.					// 11/09/98 AM.
passes = new Dlist<Seqn>();
pass = rfa_rulelt(rfa);				// Get rulelt pass.				// 11/09/98 AM.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.		// 11/09/98 AM.
//pass->setAlgo(0);			// 11/17/98 AM.
//pass->setRules(0);			// 11/17/98 AM.
//delete pass;					// 11/17/98 AM.

// 11/10/98 AM.
// Second pass to distinguish rule phrase elements from the suggested element.
pass = rfa_elt(rfa);
passes->rpush(pass);
//pass->setAlgo(0);			// 11/17/98 AM.
//pass->setRules(0);			// 11/17/98 AM.
//delete pass;					// 11/17/98 AM.

// 11/09/98 AM.
// Third pass to collect elements into a list.
pass = rfa_rulelts(rfa);
passes->rpush(pass);
//pass->setAlgo(0);			// 11/17/98 AM.
//pass->setRules(0);			// 11/17/98 AM.
//delete pass;					// 11/17/98 AM.

///////////////////////////////////////
// RULE  1:   _RULE <- _NONLIT _ARROW _xWILD _ENDRULE @@
///////////////////////////////////////
// 11/09/98 AM. Changing the way the suggested element is matched, etc:
/*
		_RULE <- _xWILD [matches=(_LIT _NONLIT _ELEMENT)
								min=1 max=1
								passes=(rulelt sugg)]
					_ARROW
					_xWILD [matches=(_LIT _NONLIT _ELEMENT)
								min=1 max=0          # max=0 means indefinite.
								passes=(rulelt rulelts)]
					_ENDRULE
				@@
*/

// Need action to warn about empty rule (wildcard matches nothing).
// warn_empty(3)   -- watch out! the underscore won't work!

// Need an action to fill Sem class with a rule.
// Form:  postRFArule(1, 3);
//				where 1 refers to suggested elt and 3 refers to the phrase.

func = _T("rfarule");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("3"));
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();					// Create rule phrase.
//relt = Ielt::addDelt(phr, "_NONLIT",0,1,1);	// Add element to phrase.
relt = Ielt::addDelt(phr, _T("_xWILD"),0,1,1);	// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_NONLIT"));
arg3 = new Iarg(_T("_ELEMENT"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
matches->rpush(arg3);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(spasses);

trig = Ielt::addDelt(phr, _T("_ARROW"),0,1,1);		// Add element to phrase.
			// 11/23/98 AM. Making this the trigger.
Ielt *tr = trig->getData();						// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.
relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);		// Add element to phrase.
// 11/08/98 AM. "Recursively" invoke a rulest to fix up rule elts.
elt = relt->getData();
//elt->setRuleset(rs_rulelt);					// A named ruleset.
elt->setPasses(passes);							// Recursive passes.	// 11/09/98 AM.

Ielt::addDelt(phr, _T("_ENDRULE"),0,1,1);		// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_RULE"));				// Create suggested elt.
sugg->setBase(true);						// 11/10/98 AM.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts,trig); // Create and add rule to list.
trig = 0;

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("rule"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_RULES
* CR:		11/11/98 AM.
* SUBJ:	RFA pass for collecting rules into lists.
* NOTE:	Simplified.
********************************************/

void RFA::rfa_rules(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Dlist<Iarg> *args;
//Dlist<Iarg> *matches;				// List of matches.	// 11/09/98 AM.
//Dlist<Iarg> *fails;					// List of fails.		// 11/09/98 AM.
Iarg *arg1;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _RULES <- _soRULES [opt] _RULE [+ trigger] _eoRULES [opt] @@
///////////////////////////////////////

// Need an action to fill Sem class with rules.
// Form:  postRFArules(2);
//				where 2 refers to _rule.

func = _T("rfarules");
arg1 = new Iarg(_T("2"));;
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soRULES"),0,0,1);			// Add element to phrase.
trig = Ielt::addDelt(phr, _T("_RULE"),0,1,0);		// Add element to phrase.
Ielt *tr = trig->getData();						// 11/05/99 AM.
tr->setTrigger(true);								// 11/05/99 AM.
Ielt::addDelt(phr, _T("_eoRULES"),0,0,1);			// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_RULES"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts, trig);// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("rules"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_LITTOACTION
* CR:		11/23/98 AM.
* SUBJ:	RECURSIVE ruleset for converting a literal to an action.
* RET:	A pass object containing the ruleset built.
* NOTE:	Operates in the context of a list of actions.
********************************************/

Seqn *RFA::rfa_littoaction(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for creating an action.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _ACTION <- _LIT @@
///////////////////////////////////////
// eg,  @POST
//				single			# This one is a _LIT
//				check("blah")
//      @RULES ....

// Convert a literal to an action.
// Semantic actions:
// Form:  postRFAlittoaction(1);
//				where 1 is _lit

func = _T("rfalittoaction");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);				// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ACTION"));						// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

Seqn *pass;
pass = new Seqn(0,0, rules, _T("littoaction")); // Name recurse. // 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_LITTOPRE
* CR:		11/29/98 AM.
* SUBJ:	RECURSIVE ruleset for converting a literal to a pre action.
* RET:	A pass object containing the ruleset built.
* NOTE:	Operates in the context of a list of actions.
********************************************/

Seqn *RFA::rfa_littopre(Ana &rfa)
{
//Delt<Seqn> *delt;
//Algo *algo;

if (rfa.Verbose())
	*gout << _T("Recursive rules for creating a pre action.") << std::endl;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
_TCHAR *func;				// Action name or function.
Iarg *arg1;
//Iarg *arg2;
Dlist<Iarg> *args;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _ACTION <- _LIT @@
///////////////////////////////////////
// eg,  @POST
//				single			# This one is a _LIT
//				check("blah")
//      @RULES ....

// Convert a literal to an action.
// Semantic actions:
// Form:  postRFAlittoaction(1);
//				where 1 is _lit

func = _T("rfalittopre");
arg1 = new Iarg(_T("1"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_LIT"),0,1,1);				// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_ACTION"));						// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

Seqn *pass;
pass = new Seqn(0,0, rules, _T("littopre"));	// Name recurse	// 11/04/99 AM.
return pass;
}


/********************************************
* FN:		RFA_ACTIONS
* CR:		11/23/98 AM.
* SUBJ:	RFA pass for collecting actions into a list.
* NOTE:	Executing this after finding rules, so that _LIT won't
*			swallow up suggested element of a rule.
********************************************/

void RFA::rfa_actions(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

Dlist<Iarg> *matches;
Dlist<Seqn> *passes;					// Passes to RECURSE on.
Seqn *pass;								// A single pass.
Iarg *arg1, *arg2;
_TCHAR *func;				// Action name or function.
Iarg *arg;
Dlist<Iarg> *args;

// BUILD RECURSIVE RULESETS FOR PAIR VARIATIONS.
// Recursive rules here to convert _LIT to an action.
passes = new Dlist<Seqn>();
pass = rfa_littoaction(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;
pass = 0;	// Reset var.

///////////////////////////////////////
// RULE  1:   _POSTS <-  _soPOST _xWILD [matches=(_ACTION _LIT)]
//									_eoPOST [optional] @@
///////////////////////////////////////

// Need an action to fill Sem class with actions.
// Form:  postRFAactions(2);
//				where 2 is the nodes matching the wildcard.

func = _T("rfaactions");
arg = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soPOST"),0,1,1);			// Add element to phrase.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);	// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_ACTION"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(passes);
matches = 0;
passes = 0;
arg1 = 0;
arg2 = 0;

Ielt::addDelt(phr, _T("_eoPOST"),0,0,1);			// Add element to phrase.
															// [OPTIONAL]

sugg = new Isugg(_T("_POSTS"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.
phr = 0;
sugg = 0;
posts = 0;
elt = 0;
func = 0;
/////////////////////////////////////////////////////////

// BUILD RECURSIVE RULESETS FOR PAIR VARIATIONS.
// Recursive rules here to convert _LIT to an action.
passes = new Dlist<Seqn>();
pass = rfa_littoaction(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;
pass = 0;

///////////////////////////////////////
// RULE  2:   _CHECKS <-  _soCHECK _xWILD [matches=(_ACTION _LIT)]
//									_eoCHECK [optional] @@
///////////////////////////////////////

// Need an action to fill Sem class with actions.
// Form:  postRFAactions(2);
//				where 2 is the nodes matching the wildcard.

func = _T("rfaactions");
arg = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soCHECK"),0,1,1);			// Add element to phrase.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);	// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_ACTION"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(passes);
matches = 0;
passes = 0;
arg1 = 0;
arg2 = 0;

Ielt::addDelt(phr, _T("_eoCHECK"),0,0,1);			// Add element to phrase.
															// [OPTIONAL]

sugg = new Isugg(_T("_CHECKS"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.
phr = 0;
sugg = 0;
posts = 0;
elt = 0;
func = 0;
/////////////////////////////////////////////////////////

// BUILD RECURSIVE RULESETS FOR PAIR VARIATIONS.
// Recursive rules here to convert _LIT to an action.
passes = new Dlist<Seqn>();
pass = rfa_littopre(rfa);			// Get pass.
// 07/02/99 AM. No longer copies.
passes->rpush(pass);					// Add to list of passes.
//pass->setAlgo(0);
//pass->setRules(0);
//delete pass;
pass = 0;

///////////////////////////////////////
// RULE  2:   _PRES <-  _soPRE _xWILD [matches=(_ACTION _LIT)]
//									_eoPRE [optional] @@
///////////////////////////////////////

// Need an action to fill Sem class with actions.
// Form:  postRFApres(2);
//				where 2 is the nodes matching the wildcard.

func = _T("rfapres");
arg = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg);
//delete arg;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soPRE"),0,1,1);				// Add element to phrase.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);	// Add element to phrase.
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_ACTION"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
elt->setMatches(matches);
elt->setPasses(passes);
matches = 0;
passes = 0;
arg1 = 0;
arg2 = 0;

Ielt::addDelt(phr, _T("_eoPRE"),0,0,1);			// Add element to phrase.
															// [OPTIONAL]

sugg = new Isugg(_T("_PRES"));				// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.
phr = 0;
sugg = 0;
posts = 0;
elt = 0;
func = 0;

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("actions"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_CODE
* CR:		12/07/98 AM.
* SUBJ:	RFA pass for building a CODE region.
********************************************/

void RFA::rfa_code(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;
Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2;
_TCHAR *func;				// Action name or function.


///////////////////////////////////////
// RULE  1:   _CODE <-  _soCODE _xWILD [match=(_ACTION)] _eoCODE [opt] @@
///////////////////////////////////////
// Only handling bonafide "code" actions for now.
// Can expand to full programming language as we go.

// Form:  rfacode(2);

func = _T("rfacode");
arg1 = new Iarg(_T("2"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soCODE"),0,1,1);			// @CODE
relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0);	// _xWILD [match=(_ACTION)]
elt = relt->getData();
arg1 = new Iarg(_T("_LIT"));
arg2 = new Iarg(_T("_ACTION"));
matches = new Dlist<Iarg>();
matches->rpush(arg1);
matches->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
elt->setMatches(matches);
//elt->setPasses(passes);
matches = 0;
//passes = 0;
arg1 = 0;
arg2 = 0;

Ielt::addDelt(phr, _T("_eoCODE"),0,0,1);			// @@CODE [opt].

sugg = new Isugg(_T("_CODE"));							// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("code"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_NODES
* CR:		11/30/98 AM.
* SUBJ:	RFA pass for collecting nodes into a list.
********************************************/

void RFA::rfa_nodes(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.

pres = 0;
posts = 0;
trig = 0;
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _NODES <-  _soNODES _NONLIT [*] _eoNODES [opt] @@
///////////////////////////////////////

// Form:  rfanodes(2);

func = _T("rfanodes");
arg1 = new Iarg(_T("2"));
arg2 = new Iarg(_T("nodes"));			// 03/25/99 AM.
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);					// 03/25/99 AM.
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soNODES"),0,1,1);			// @NODES
Ielt::addDelt(phr, _T("_NONLIT"),0,0,0);			// _NONLIT [*]
Ielt::addDelt(phr, _T("_eoNODES"),0,0,1);			// @@NODES [opt].

sugg = new Isugg(_T("_NODES"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("nodes"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_MULTI
* CR:		03/25/99 AM.
* SUBJ:	RFA pass for collecting MULTI into a list.
********************************************/

void RFA::rfa_multi(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.

pres = 0;
posts = 0;
trig = 0;
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _MULTI <-  _soMULTI _NONLIT [*] _eoMULTI [opt] @@
///////////////////////////////////////

// Form:  rfamulti(2);

func = _T("rfanodes");
arg1 = new Iarg(_T("2"));
arg2 = new Iarg(_T("multi"));			// Other value is "nodes".
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soMULTI"),0,1,1);			// @MULTI
Ielt::addDelt(phr, _T("_NONLIT"),0,0,0);			// _NONLIT [*]
Ielt::addDelt(phr, _T("_eoMULTI"),0,0,1);			// @@MULTI [opt].

sugg = new Isugg(_T("_MULTI"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("multi"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_PATH
* CR:		10/19/99 AM.
* SUBJ:	RFA pass for collecting PATH into a list.
********************************************/

void RFA::rfa_path(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.

pres = 0;
posts = 0;
trig = 0;
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _PATH <-  _soPATH _NONLIT [*] _eoPATH [opt] @@
///////////////////////////////////////

// Form:  rfapath(2);

func = _T("rfanodes");
arg1 = new Iarg(_T("2"));
arg2 = new Iarg(_T("path"));			// Other value is "path".
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soPATH"),0,1,1);			// @PATH
Ielt::addDelt(phr, _T("_NONLIT"),0,0,0);			// _NONLIT [*]
Ielt::addDelt(phr, _T("_eoPATH"),0,0,1);			// @@PATH [opt].

sugg = new Isugg(_T("_PATH"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("path"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_SELECT
* CR:		11/30/98 AM.
* SUBJ:	RFA pass for building a SELECT region.
********************************************/

void RFA::rfa_select(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1;
_TCHAR *func;				// Action name or function.
Dlist<Iarg> *args;

///////////////////////////////////////
// RULE  1:   _SELECT <-  _soSELECT [opt] _NODES _eoSELECT [opt] @@
///////////////////////////////////////

// Form:  rfaselect(2);

func = _T("rfaselect");
arg1 = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soSELECT"),0,0,1);			// @SELECT [opt]
Ielt::addDelt(phr, _T("_NODES"),0,1,1);				// _NODES
Ielt::addDelt(phr, _T("_eoSELECT"),0,0,1);			// @@SELECT [opt].

sugg = new Isugg(_T("_SELECT"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
// RULE  2:   _SELECT <-  _soSELECT [opt] _MULTI _eoSELECT [opt] @@
///////////////////////////////////////

// 03/25/99 AM. Could have done
// _SELECT <- _soSELECT [opt] _xWILD [one match=(_NODES _MULTI)]
//					_eoSELECT [opt] @@
// But this is semi-panic mode before PM demo tomorrow!

// Form:  rfaselect(2);

func = _T("rfaselect");
arg1 = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soSELECT"),0,0,1);			// @SELECT [opt]
Ielt::addDelt(phr, _T("_MULTI"),0,1,1);				// _MULTI
Ielt::addDelt(phr, _T("_eoSELECT"),0,0,1);			// @@SELECT [opt].

sugg = new Isugg(_T("_SELECT"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
// RULE  3:   _SELECT <-  _soSELECT [opt] _PATH _eoSELECT [opt] @@
///////////////////////////////////////
// 10/19/99 AM.
// Form:  rfaselect(2);

func = _T("rfaselect");
arg1 = new Iarg(_T("2"));
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soSELECT"),0,0,1);			// @SELECT [opt]
Ielt::addDelt(phr, _T("_PATH"),0,1,1);				// _PATH
Ielt::addDelt(phr, _T("_eoSELECT"),0,0,1);			// @@SELECT [opt].

sugg = new Isugg(_T("_SELECT"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("select"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_REGION
* CR:		11/24/98 AM.
* SUBJ:	RFA pass for a rules-action region.
********************************************/

void RFA::rfa_region(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *checks;				// List of check actions.		// 11/27/98 AM.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = checks = posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3, *arg4;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _REGION <-  _PRES [opt] _CHECKS [opt] _POSTS [opt] _RULES @@
///////////////////////////////////////

// Need an action to fill Sem class with actions.
// Form:  postRFAregion(1,2,3,4);

func = _T("rfaregion");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("2"));
arg3 = new Iarg(_T("3"));
arg4 = new Iarg(_T("4"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
args->rpush(arg3);
args->rpush(arg4);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
//delete arg4;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_PRES"), 0,0,1);				// _PRES   [opt]
Ielt::addDelt(phr, _T("_CHECKS"), 0,0,1);			// _CHECKS [opt]
Ielt::addDelt(phr, _T("_POSTS"),0,0,1);				// _POSTS  [opt]
Ielt::addDelt(phr, _T("_RULES"),0,1,1);				// _RULES

sugg = new Isugg(_T("_REGION"));						// _REGION  <-
sugg->setBase(true);

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("region"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_REGIONS
* CR:		11/24/98 AM.
* SUBJ:	RFA pass for collecting regions into a list.
********************************************/

void RFA::rfa_regions(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _REGIONS <-  _REGION [+] @@
///////////////////////////////////////

// Need an action to fill Sem class with actions.
// Form:  postRFAregions(1);
//				where 1 is the _region elt.

func = _T("rfaregions");
arg1 = new Iarg(_T("1"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_REGION"),0,1,0);			// _REGION [+]

sugg = new Isugg(_T("_REGIONS"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("regions"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_RECURSE
* CR:		11/25/98 AM.
* SUBJ:	RFA pass for a recursive mini-pass region.
********************************************/

void RFA::rfa_recurse(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;
//Dlist<Iarg> *attrs;

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE	1:
//
//   _RECURSE   <- _soRECURSE
//						 _LIT [attrs="_NAME"]	# REQUIRED NAME FOR MINI-PASS.
//						 _REGIONS [opt]
//						 _eoRECURSE
//						 _LIT [opt]  # If present, name must match.
//						 @@
///////////////////////////////////////

// Actions:  rfarecurse(2,3,5)	# 2=name, 3= regions(opt), 5=name(opt)
//				 single()				# Single-tier reduction (the default)

func = _T("rfarecurse");
arg1 = new Iarg(_T("2"));
arg2 = new Iarg(_T("3"));
arg3 = new Iarg(_T("5"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
args->rpush(arg3);
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

// Putting LAYERING in one rule elt.
#ifdef ELT_ATTRS_
attrs = new Dlist<Iarg>();				// Empty list of attrs (ie layers).
arg1 = new Iarg(_T("_NAME"));
attrs->rpush(arg1);		// 07/02/99 AM.
//delete arg1;
#endif

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_soRECURSE"), 0,1,1);		// _soRECURSE
Ielt::addDelt(phr, _T("_LIT"),0,1,1);				// _LIT [attrs=("_NAME")]
// elt->setAttrs(attrs);		// Not implmented for elts yet.
//attrs = 0;						// Reset for next rule, if any.
Ielt::addDelt(phr, _T("_REGIONS"),0,0,1);			// _REGIONS [opt]
Ielt::addDelt(phr, _T("_eoRECURSE"),0,1,1);		// _eoRECURSE
Ielt::addDelt(phr, _T("_LIT"),0,0,1);				// _LIT [opt]

sugg = new Isugg(_T("_RECURSE"));						// _RECURSE  <-
sugg->setBase(true);

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("recurse"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_RECURSES
* CR:		11/25/98 AM.
* SUBJ:	RFA pass for collecting recurses into a list.
********************************************/

void RFA::rfa_recurses(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _RECURSES <-  _RECURSE [+] @@
///////////////////////////////////////

// Actions:	rfarecurses(1)
//				single()

func = _T("rfarecurses");
arg1 = new Iarg(_T("1"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
//delete arg1;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_RECURSE"),0,1,0);			// _RECURSE [+]

sugg = new Isugg(_T("_RECURSES"));						// Create suggested elt.
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("recurses"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_RULESFILE
* CR:		11/24/98 AM.
* SUBJ:	RFA pass for recognizing a complete rules file.
********************************************/

void RFA::rfa_rulesfile(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules for current pass.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

//Dlist<Iarg> *matches;
//Dlist<Seqn> *passes;					// Passes to RECURSE on.
//Seqn *pass;								// A single pass.
Iarg *arg1, *arg2, *arg3, *arg4, *arg5;
_TCHAR *func;				// Action name or function.

///////////////////////////////////////
// RULE  1:   _RULESFILE  <-  
//										_DECL [opt]								// 12/19/01 AM.
//										_CODE [opt]								// 12/07/98 AM.
//										_SELECT [opt] _RECURSES [opt] _REGIONS @@
///////////////////////////////////////

// Need an action to fill Sem class with actions.
// Form:  postRFArulesfile(1,2,3);

func = _T("rfarulesfile");
arg1 = new Iarg(_T("1"));
arg2 = new Iarg(_T("2"));
arg3 = new Iarg(_T("3"));
arg4 = new Iarg(_T("4"));
arg5 = new Iarg(_T("5"));	// To handle @DECL region.					// 12/19/01 AM.
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(arg1);
args->rpush(arg2);
args->rpush(arg3);
args->rpush(arg4);
args->rpush(arg5);		// To handle @DECL region.					// 12/19/01 AM.
//delete arg1;		// 07/02/99 AM.
//delete arg2;		// 07/02/99 AM.
//delete arg3;		// 07/02/99 AM.
//delete arg4;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Do the reduce also.
func = _T("single");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();							// Create rule phrase.
Ielt::addDelt(phr, _T("_DECL"),0,0,1);				// _DECL [opt]		// 12/19/01 AM.
Ielt::addDelt(phr, _T("_CODE"),0,0,1);				// _CODE [opt]		// 12/07/98 AM.
Ielt::addDelt(phr, _T("_SELECT"),0,0,1);			// _SELECT [opt]
Ielt::addDelt(phr, _T("_RECURSES"),0,0,1);			// _RECURSES [opt]
Ielt::addDelt(phr, _T("_REGIONS"),0,1,1);			// _REGIONS

sugg = new Isugg(_T("_RULESFILE"));					// _RULESFILE  <-
sugg->setBase(true);
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////


if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("rulesfile"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);
}


/********************************************
* FN:		RFA_EXCISE
* CR:		11/20/98 AM.
* SUBJ:	Build an RFA pass for excising particular nodes from the tree.
* NOTE:	Parameterized rfa_x_white.
********************************************/

void RFA::rfa_excise(
	_TCHAR *name,							// The node name to zap. eg "_xWHITE".
	_TCHAR *passname,					// eg, "x_white".
	Ana &rfa
	)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
//Delt<Ielt> *relt;						// Rule elt.
//Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:	@POST
//					excise(1, 1)
//				@RULES
//					_xNIL <- <NAME> [+] @@
///////////////////////////////////////

// Build an excise action here.
// Form: excise(start, end) -- start,end are RULE ELEMENT ORDS.
// This means every matched node should have the number of the rule
// element that it matched (wildcards and repeats match more than one...).

_TCHAR *func;				// Action name or function.
func = _T("excise");
Iarg *start, *end;
start = new Iarg(_T("1"));
end   = new Iarg(_T("1"));
Dlist<Iarg> *args;
args = new Dlist<Iarg>();
args->rpush(start);
args->rpush(end);
//delete start;	// 07/02/99 AM.
//delete end;		// 07/02/99 AM.
posts = Iaction::makeDlist(func, args);

// Trying multiple actions.
func = _T("noop");
Iaction::addDelt(posts, func, 0);

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, name,0,1,0);	// Add element to phrase.
// Create and attach list of pairs.

sugg = new Isugg(_T("_xNIL"));				// Create suggested elt.
// Create and attach list of pairs.

Irule::addDelt(rules, phr, sugg, 0, 0, posts);	// Create and add rule to list.


///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}


// Attach pass rules to current pass.
algo = new Pat();
algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, passname, rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}


/********************************************
* FN:		RFA_CLEAN
* CR:		11/24/98 AM.
* SUBJ:	Ad hoc cleanups for the RFA analyzer.
* NOTE:	Because this is a specialized, hand-built analyzer.
********************************************/

void RFA::rfa_clean(Ana &rfa)
{
Dlist<Seqn> *list;
list = rfa.getSeqlist();
rfa_clean_seqns(list);			// RECURSIVE!!
}


/********************************************
* FN:		RFA_CLEAN_SEQS
* CR:		11/24/98 AM.
* SUBJ:	Ad hoc cleanups for the RFA analyzer.
* NOTE:	Because this is a specialized, hand-built analyzer.
* WARN:	RECURSIVE.
********************************************/

void RFA::rfa_clean_seqns(Dlist<Seqn> *list)
{
Dlist<Irule> *rules;
Delt<Seqn> *seqns;
Seqn *seqn;
Delt<Irule> *drule;
Irule *rule;
Dlist<Ielt> *delts;
Delt<Ielt> *delt;
Ielt *elt;
if (!list)
	return;
for (seqns = list->getFirst(); seqns; seqns = seqns->Right())
	{
	seqn = seqns->getData();
	if (!(rules = seqn->getRules()))
		continue;
	// Rules with recursive passes have to clean their own.
	for (drule = rules->getFirst(); drule; drule = drule->Right())
		{
		rule = drule->getData();
		delts = rule->getPhrase();
		for (delt = delts->getFirst(); delt; delt = delt->Right())
			{
			elt = delt->getData();
			RFA::rfa_clean_seqns(elt->getPasses());	// RECURSIVE.
			}
		}

	Irule::deleteOwning(rules);
	Dlist<Irule>::DeleteDlistAndData(rules);
	}
}

/********************************************
* FN:		RFA_TEST
* CR:		11/03/98 AM.
* SUBJ:	A test pass.
********************************************/

void RFA::rfa_test(Ana &rfa)
{
Delt<Seqn> *delt;
Algo *algo;

Dlist<Irule> *rules;					// List of rules.
rules = new Dlist<Irule>();		// Create empty rules list.

// Convenience variables for each rule.
Dlist<Ielt> *phr;						// Rule phrase.
Isugg *sugg;							// Suggested elt.
Dlist<Iaction> *pres;					// List of pre actions.
Dlist<Iaction> *posts;					// List of post actions.
Delt<Ielt> *trig;						// Trigger elt.
Delt<Ielt> *relt;						// Rule elt.
Ielt *elt;

pres = 0;
posts = 0;
trig = 0;

///////////////////////////////////////
// RULE  1:   _LIT <- _tALPHA @@
///////////////////////////////////////

// Means that an alphabetic token is a literal.
// For example, the word "IBM" is a literal token in a rules file.
// Absent data will lead to default actions.
// (TESTING OUT SIMPLER STUFF FIRST).

phr = new Dlist<Ielt>();				// Create rule phrase.
//Ielt::addDelt(phr, "_tALPHA");		// Add element to phrase.
Ielt::addDelt(phr, _T("_xWILD"),0,0,0);	// Add element to phrase.
// Create and attach list of pairs.
sugg = new Isugg(_T("_LIT"));				// Create suggested elt.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

Irule::addDelt(rules, phr, sugg);	// Create and add rule to list.

///////////////////////////////////////
// RULE  2:   _LIT <- _ wild  \space @@
///////////////////////////////////////

phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("_"));				// Add elt to phrase.
trig = Ielt::addDelt(phr, _T("wild"),0,0,0);	// Add elt to phrase.
Ielt::addDelt(phr, _T(" "));				// Add: white space elt.
// Create and attach list of pairs.
sugg = new Isugg(_T("_LIT"));				// Create suggested elt.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, 0, trig);

///////////////////////////////////////
// RULE  3:   _LIT <- \_ _xWILD \< [trigger] @@
///////////////////////////////////////
// Testing wildcard left match. 
phr = new Dlist<Ielt>();				// Create rule phrase.
Ielt::addDelt(phr, _T("_"),0,2,2);		// Add element to phrase.

relt = Ielt::addDelt(phr, _T("_xWILD"),0,3,0);	// Add element to phrase.
elt = relt->getData();

// Test optional elt.
relt = Ielt::addDelt(phr, _T("Optional"),0,0,1);

trig = Ielt::addDelt(phr, _T("<"));
// Create and attach list of pairs.
sugg = new Isugg(_T("_LIT"));				// Create suggested elt.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg, 0, 0, 0, trig);

///////////////////////////////////////
// RULE  4:   _TAG <- \< _xWILD \> [trigger] @@
/////////////////////////////////////// 
Isugg *sugg1;							// Suggested elt.
phr = new Dlist<Ielt>();				// Create rule phrase.
relt = Ielt::addDelt(phr, _T("<"),0,1,1);		// Add element to phrase.
elt = relt->getData();
elt->setSinglet(true);					// Look down the tree. 11/03/98 AM.

// 11/2/98 AM. Trying the rename="name" key=val pair here.
relt = Ielt::addDelt(phr, _T("_xWILD"),0,0,0, _T("label"));	// Add elt to phrase.
elt = relt->getData();

// Test optional elt.
trig = Ielt::addDelt(phr, _T(">"),0,1,1);
// Create and attach list of pairs.
sugg1 = new Isugg(_T("_TAG"));				// Create suggested elt.
sugg1->setBase(true);					// 11/3/98 AM.
// Create and attach list of pairs.

// Create pre-actions.
// Create post-actions.

// Create and add rule to list.
Irule::addDelt(rules, phr, sugg1, 0, 0, 0, trig);

///////////////////////////////////////
//      END OF RULES
///////////////////////////////////////

if (rfa.Verbose())
	{
	*gout << *rules;
	*gout << _T("******************************") << std::endl;
	}

// Attach pass rules to current pass.
algo = new Pat();
//algo->setDebug(RFA::Debug());
delt = Seqn::makeDelt(algo, _T("test"), rules);
Seqn *seqn = delt->getData();										// 11/04/99 AM.
seqn->setAlgoname(_T("nlp"));											// 11/04/99 AM.
rfa.addSeq(delt);

// Attach pat algo to the RFA pass sequence.
// Go and execute the RFA in the appropriate place!!!!!
}

/******************************************************************************/
