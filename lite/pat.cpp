/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PAT.CPP
* FILE:	lite\pat.cpp
* CR:		10/18/98 AM.
* SUBJ:	Main pattern-based (rule-based) pass.
* ASS:	Parse tree has been built.  Lines data structure may or may not
*			exist.
* NOTE:	post.cpp has more Pat class functions.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"															// 09/26/01 AM.
#include "io.h"
#include "chars.h"			// 01/28/05 AM.
#include "xml.h"				// 01/28/05 AM.
#include "string.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "parse.h"
#include "lite/code.h"
#include "htab.h"				// 12/12/98 AM.
#include "slist.h"			// 12/17/98 AM.
//#include "ifile.h"			// 12/17/98 AM.
#include "Eana.h"				// 10/13/99 AM.
#include "lite/nlppp.h"			// 11/19/99 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "ielt.h"				// 12/17/98 AM.
#include "ifile.h"			// 12/17/98 AM.
#include "lite/Arun.h"		// 06/08/00 AM.
#include "var.h"				// 08/31/00 AM.
#include "pat.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("nlp");

// WARNING: Must match pat.h/enum Dir definition.
static _TCHAR *Dir_s[] =
	{
	_T("null"),
	_T("left"),
	_T("right"),
	_T("done")
	};

// If user has ever been warned about wildcards at start, end, or trigger
// elements of a rule.
static bool warn_wild = false;

/********************************************
* FN:		Special Functions for the class.
* CR:		10/18/98 AM.
********************************************/

Pat::Pat()			// Default constructor
	: Algo(algo_name /*, 0 */)
{
}

Pat::Pat(const Pat &orig)			// Copy constructor	// 12/03/98 AM.
{
name = orig.name;
//parse = orig.parse;		// 12/04/98 AM.
debug_ = orig.debug_;
}

/********************************************
* FN:		Access Functions
* CR:		10/18/98 AM.
********************************************/

//Delt<Irule> *Pat::getRule()	{ return rule_; }

/********************************************
* FN:		Modify Functions
* CR:		10/18/98 AM.
********************************************/

//void Pat::setRule(Delt<Irule> *x)	{ rule_ = x; }

/********************************************
* FN:		DUP
* CR:		12/03/98 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Pat::dup()
{
Pat *ptr;
ptr = new Pat(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}

/********************************************
* FN:		SETUP
* CR:		12/04/98 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Pat::setup(_TCHAR *s_data)
{
// This one is handled internally in Ana::addPass.
// s_data is the name of the rules file.
}


/********************************************
* FN:		EXECUTE
* CR:		10/18/98 AM.
* SUBJ:	Perform the pat pass.
* NOTE:	Traverse the parse tree  to match rules in the
*			internal rules representation.  When matched, execute
*			actions.
********************************************/

bool Pat::Execute(Parse *parse, Seqn *seqn)
{

if (!parse || !seqn)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass given no parser or pass information.]") << std::ends;
	return errOut(&gerrStr,false);
	}


/////////////////////
// CODE ACTIONS					// 12/07/98 AM.
/////////////////////
// Before any other processing, pass will perform its code actions.
// Should change Pat to be ok with no rules (even empty file should be ok).
bool exitpass = false;														// 02/06/01 AM.
Code::execCode(parse, seqn,
						/*UP*/ exitpass);										// 02/06/01 AM.
if (exitpass)																	// 02/06/01 AM.
	return true;																// 02/26/02 AM.


/////////////////////
// INITIALIZE
/////////////////////

// Get the parse tree(s).
// Tree<Ln> *lines parse->lines;					// Obsolete.		// 07/18/01 AM.
Tree<Pn> *tree = (Tree<Pn> *)parse->tree;												// 07/18/01 AM.
if (!tree
// || !lines																	// 07/18/01 AM.
   )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass given no parse tree data.]") << std::ends;	// 07/18/01 AM.
	return errOut(&gerrStr,false);
	}

// Get the parse tree's root node.
Node<Pn> *root;
root = tree->pRoot;									// OPT				// 05/16/00 AM.
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass: Tree has no root.]") << std::ends;
	return errOut(&gerrStr,false);
	}

Ifile *rulesfile = seqn->getRulesfile();								// 08/31/00 AM.

// Get the rule list.
Dlist<Irule> *rulelist;
rulelist = seqn->rules_;							// OPT				// 05/16/00 AM.
if (!rulelist)
	{
	// Check if any @RULES marker was in the file.					// 08/31/00 AM.
	if (rulesfile && !rulesfile->getEmpty())							// 08/31/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Pat pass given no rules.]") << std::ends;
		return errOut(&gerrStr,false);
		}
	if (seqn->displaytree_)
		prettyPass(root,tree,parse);
	return true;
	}

// Get the rules themselves.
Delt<Irule> *rules;
rules = rulelist->getFirst();
if (!rules)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass: No rules in list.]") << std::ends;
	return errOut(&gerrStr,false);
	}

// 12/17/98 AM.
// PREPARE FOR HASHING RULES.
// Note: The RFA currently has no rules files.  // 12/19/98 AM.
Slist<Irule> *mustlist = 0;
Selt<Irule> *musts = 0;
tHtab<Slist<Irule> > *htab = 0;
if (rulesfile)																	// 08/31/00 AM.
	{
	mustlist = rulesfile->getMusts();
	if (mustlist)
		musts = mustlist->getFirst();
	htab = rulesfile->getHtab();
	}

// Get the selected nodes to traverse.
Dlist<Iarg> *selects = 0;
selects = seqn->getSelects();

///////////////////////
// TRAVERSE AND MATCH
///////////////////////

// Traverse the tree, matching on each rule in turn.
// For each node in the parse tree, see if it is distinguished.

Node<Pn> *select;
if (!selects)					// 11/30/98 AM.
	{
	// Root of the parse tree is the selected or distinguished node,
	// by default.
	select = root;

	// For each "select" node, match on its list of children.
	if (musts || htab)											// 12/19/98 AM.
//		matchSelect(select, musts, htab, parse);			// 12/18/98 AM.
		matchNodes(select->Down(),0,musts,htab,parse,select);	// 04/27/00 AM.
	else
		{
		// Keeping the non-hashed way for now.
		// The RFA uses it at present.
		//matchSelect(select, rules, parse);
		matchNodes(select->Down(),0,rules,parse,select);	// 04/27/00 AM.
		}
	}
// Check MULTI TRAVERSAL OF PARSE TREE.					// 03/25/99 AM.
else if (rulesfile && rulesfile->getFmulti())			// 03/25/99 AM.
	{
	// 03/25/99 AM.
	if (musts || htab)
		multiSelect(selects, musts, htab, parse, root);
	}
// Check PATH TRAVERSAL OF PARSE TREE.						// 10/19/99 AM.
else if (rulesfile && rulesfile->getFpath())				// 10/19/99 AM.
	{
	// 10/19/99 AM.
	if (musts || htab)
		pathSelect(selects->getFirst(), musts, htab, parse, root);
	}
// 10/25/99 AM. PUT PREMULTI HERE WHEN READY.
else
	{
	// 11/30/98 AM.
	// Traverse and find nodes matching the selected list.
	// Apply rules to the children of each.
	// (Start traversing at root.)
	if (musts || htab)											// 12/19/98 AM
		traverseSelect(selects, musts, htab, parse, root);	// 12/18/98 AM.
	else
		{
		// Keeping the nonhashed method for now.
		// The RFA needs it, if noone else.
		traverseSelect(selects, rules, parse, root);
		}
	}

prettyPass(root,tree,parse);												// 06/08/00 AM.
return true;
}


/********************************************
* FN:		TRAVERSESELECT
* CR:		11/30/98 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
* RET:	False if exiting pass.	// 11/11/02 AM.
********************************************/

bool Pat::traverseSelect(
	Dlist<Iarg> *selects,	// Names of select nodes.
	Delt<Irule> *rules,		// Rule set to match against.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;																// 11/11/02 AM.
// If current node is select, match against its children.
Delt<Iarg> *dselect;
Iarg *select;
Pn *pn;
_TCHAR *name;
pn = node->getData();
name = pn->getName();
//bool flag = false;			// If current node is select.	// 10/09/99 AM.
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
		//flag = true;		// 10/09/99 AM.
		//matchSelect(node, rules, parse);
		if (!matchNodes(node->Down(),0,rules,parse,node))	// 04/27/00 AM.
			return false;											// 11/11/02 AM.
		//break;				// 10/09/99 AM.
		goto listmanager;		// 10/09/99 AM.
		}
	}

// Try the rest of the tree.
//if (!flag)		// If current node wasn't select, look within it.
// If node wasn't select and is UNSEALED, then
// look inside it.	// OPT								// 10/09/99 AM.
if (pn->getUnsealed())	// 10/11/99 AM.
	{
	if (!traverseSelect(selects,rules,parse,node->Down()))
		return false;															// 11/11/02 AM.
	}


// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
listmanager:				// 10/09/99 AM.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!traverseSelect(selects,rules,parse,node))
			return false;														// 11/11/02 AM.
		}

	}
return true;																	// 11/11/02 AM.
}


/********************************************
* FN:		TRAVERSESELECT
* CR:		12/18/98 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			VARIANT USES HASHED RULES.
********************************************/

bool Pat::traverseSelect(
	Dlist<Iarg> *selects,	// Names of select nodes.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;																// 11/11/02 AM.
// If current node is select, match against its children.
Delt<Iarg> *dselect;
Iarg *select;
Pn *pn;
_TCHAR *name;
pn = node->getData();
name = pn->getName();
//bool flag = false;			// If current node is select.	// 10/09/99 AM.
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
		//flag = true;				// 10/09/99 AM.
		//matchSelect(node, musts, htab, parse);
		if (!matchNodes(node->Down(),0,musts,htab,parse,node))	// 04/27/00 AM.
			return false;													// 11/11/02 AM.
		//break;						// 10/09/99 AM.
		goto listmanager;			// 10/09/99 AM.
		}
	}

// Try the rest of the tree.
//if (!flag)		// If current node wasn't select, look within it.
// If node wasn't select and is UNSEALED, then
// look inside it.	// OPT								// 10/09/99 AM.
if (pn->getUnsealed())	// 10/11/99 AM.
	{
	if (!traverseSelect(selects,musts,htab,parse,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
listmanager:						// 10/09/99 AM.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!traverseSelect(selects,musts,htab,parse,node))
			return false;
		}
	}
return true;																// 11/11/02 AM.
}

/********************************************
* FN:		MATCHSELECT
* CR:		10/28/98 AM.
* SUBJ:	Match rules in the children of a selected subtree.
* RET:	False if exiting pass.	// 11/11/02 AM.
********************************************/

bool Pat::matchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Delt<Irule> *rules,		// Rule set to match against.
	Parse *parse				// Current analysis instance.
	)
{
// Optimizing this fn.									// 04/27/00 AM.
// Todo: Shouldn't even call this silly fn.		// 04/27/00 AM.
//Node<Pn> *nodes, *start, *end;
//nodes = select->Down();
//start = nodes;				// Left boundary for matching rule.
//end = 0;						// No right boundary (ie, match till end of nodes.)

//matchNodes(start, end, rules, parse, select);	// 08/05/99 AM.
return														// 11/11/02 AM.
	matchNodes(						// OPT				// 04/27/00 AM.
	select->Down(),	// start
	0,						// end
	rules,
	parse,
	select
	);
}

/********************************************
* FN:		MATCHSELECT
* CR:		12/18/98 AM.
* SUBJ:	Match rules in the children of a selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
********************************************/

bool Pat::matchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse				// Current analysis instance.
	)
{
// Optimizing this fn.									// 04/27/00 AM.
// Todo: Shouldn't even call this silly fn.		// 04/27/00 AM.
//Node<Pn> *nodes, *start, *end;
//nodes = select->Down();
//start = nodes;				// Left boundary for matching rule.
//end = 0;						// No right boundary (ie, match till end of nodes.)

//matchNodes(start, end, musts, htab, parse, select);
return														// 11/11/02 AM.
matchNodes(				// OPT							// 04/27/00 AM.
	select->Down(),	// start
	0,						// end
	musts,
	htab,
	parse,
	select
	);
}


/********************************************
* FN:		MATCHNODES
* CR:		11/08/98 AM.
* SUBJ:	Match rules in a range of nodes.
* NOTE:	Split from matchSelect.
********************************************/

bool Pat::matchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Delt<Irule> *rules,		// Rule set to match against.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *select			// 08/05/99 AM.
	)
{
if (!start || !rules || !parse)
	return true;																// 11/11/02 AM.

// The collect list mirrors the current rule elements.  Serves as an
// instance of the rule, oveseeing its execution.
//Tree<Pn> *collect;			// List of matched rule elements.
//collect = new Tree<Pn>();	// Empty so far.

//Node<Pn> *first, *last;		// Nodes matched by rule, inclusive.
//Sem *sem = 0;					// Semantic info for rule.			// 11/27/98 AM.
//Dlist<Ipair> *dsem = 0;		// Dynamic semantics for sugg.	// 11/17/99 AM.

Delt<Irule> *prule;			// Current rules; also, the rule that matched.
Node<Pn> *node;				// SAVE CURRENT NODE.			// 11/20/99 AM.
node = start;

// 11/19/99 AM. Using this object to collect the entire parse state.
Nlppp nlppp(																// 11/19/99 AM.
	0,								// No rule yet.
	new Tree<Pn>(),			// Empty collect_.
	parse,select,
	0,0,							// No first,last yet.
	node,							// node_.
	start,end,
	0,0							// No sem, dsem yet.
	);

while (node)					// While matching on list of nodes.
	{
	// For each rule in the rules list.
	prule = rules;				// Reset the rules list. 11/1/98 AM.
	//if (Debug())
	//	*gout << " Matching on node:" << *node << std::endl;

	// if (!matchRules(node,start,end,parse,select,				// 11/19/99 AM.
	//					/*DU*/ prule,collect,first,last,sem, dsem))
	if (!matchRules(&nlppp, /*DU*/ prule))							// 11/19/99 AM.
		{
		node = nlppp.node_ = nodeRight(node,end);	// 11/08/98 AM. Bug fix.
		// Assume start, end unchanged.								// 11/20/99 AM.
		}
	else
		{
		// Successful rule match.  Execute POST actions.
		nlppp.node_ = nlppp.after_;		// Set up for next rule match.// ALGOSTATE // 06/06/13 AM.
		nlppp.rule_ = prule->getData();								// 11/19/99 AM.
		//execActions(prule->getData(),collect,first,last,parse,sem,dsem,select,
		//													/*DU*/ node, start, end);
		execActions(&nlppp);												// 11/19/99 AM.

		node  = nlppp.node_;												// 11/20/99 AM.
		start = nlppp.start_;											// 11/20/99 AM.
		end   = nlppp.end_;												// 11/20/99 AM.
		nlppp.sem_ = 0;		// Zero out.							// 11/20/99 AM.
		nlppp.dsem_ = 0;		// Zero out.							// 11/20/99 AM.
		}

	if (nlppp.exitpass_)	// Aborting pass.							// 11/11/02 AM.
		{
		delete (Tree<Pn> *) nlppp.collect_;							// 11/11/02 AM.
		return false;														// 11/11/02 AM.
		}

	// If actions haven't cleaned out the collect, then do it here.
	// matchCleanup(collect);
	}
delete (Tree<Pn> *) nlppp.collect_;				// 11/17/98 AM.
return true;																// 11/11/02 AM.
}

/********************************************
* FN:		MATCHNODES
* CR:		12/18/98 AM.
* SUBJ:	Match rules in a range of nodes.
* NOTE:	Split from matchSelect.
*			VARIANT USES HASHED RULES.
********************************************/

bool Pat::matchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *select			// 08/05/99 AM.
	)
{
if (!start || !htab || !parse)
	return true;															// 11/11/02 AM.

// The collect list mirrors the current rule elements.  Serves as an
// instance of the rule, oveseeing its execution.
//Tree<Pn> *collect;			// List of matched rule elements.
//collect = new Tree<Pn>();	// Empty so far.

//Node<Pn> *first, *last;		// Nodes matched by rule, inclusive.
//Sem *sem = 0;				// Semantic info for rule.			// 11/27/98 AM.
//Dlist<Ipair> *dsem = 0;	// Dynamic semantics for sugg.	// 11/17/99 AM.

Slist<Irule> *rules = 0;	// Current rules list.
Selt<Irule> *prule = 0;		// Current rules; also, the rule that matched.
Node<Pn> *node;				// SAVE CURRENT NODE.			// 11/20/99 AM.
node = start;

// 11/19/99 AM. Using this object to collect the entire parse state.
Nlppp nlppp(																// 11/19/99 AM.
	0,								// No rule yet.
	new Tree<Pn>(),			// Empty collect_.
	parse,select,
	0,0,							// No first,last yet.
	node,							// node_.
	start,end,
	0,0							// No sem, dsem yet.
	);

while (node)					// While matching on list of nodes.
	{
	// SETUP RULES LIST FOR CURRENT NODE HERE.
	// MERGE HASH TABLE AND MUSTS LIST AS NEEDED.
	prule = resetRules(nlppp.node_, musts, htab, /*DU*/ rules);

	//if (Debug())
	//	*gout << " Matching on node:" << *node << std::endl;

	// For each rule in the rules list.
	// if (!matchRules(node,start,end,parse,select,				// 11/19/99 AM.
	//					/*DU*/ prule,collect,first,last,sem, dsem))
	if (!matchRules(&nlppp, /*DU*/ prule))							// 11/19/99 AM.
		node = nlppp.node_ = nodeRight(node,end);	// 11/08/98 AM. Bug fix.
	else
		{
		// Successful rule match.  Execute POST actions.
		nlppp.node_ = nlppp.after_;		// Set up for next rule match.// ALGOSTATE // 06/06/13 AM.
		nlppp.rule_ = prule->getData();								// 11/19/99 AM.
		//execActions(prule->getData(),collect,first,last,parse,sem,dsem,select,
		//												/*DU*/ node, start, end);
		execActions(&nlppp);												// 11/19/99 AM.

		node  = nlppp.node_;												// 11/20/99 AM.
		start = nlppp.start_;											// 11/20/99 AM.
		end   = nlppp.end_;												// 11/20/99 AM.
		nlppp.sem_ = 0;		// Zero out.							// 11/20/99 AM.
		nlppp.dsem_ = 0;		// Zero out.							// 11/20/99 AM.
		}

	// If actions haven't cleaned out the collect, then do it here.
	// matchCleanup(collect);
	if (rules)
		Slist<Irule>::DeleteSlist(rules); // Clear out previous node's list.

	if (nlppp.exitpass_)	// Aborting pass.							// 11/11/02 AM.
		{
		delete (Tree<Pn> *) nlppp.collect_;							// 11/11/02 AM.
		return false;														// 11/11/02 AM.
		}
	}
delete (Tree<Pn> *) nlppp.collect_;				// 11/17/98 AM.
return true;																// 11/11/02 AM.
}
//abcdefghi

/********************************************
* FN:		RESETRULES
* CR:		12/18/98 AM.
* SUBJ:	Build a list of rules for given node to match on.
* NOTE:	Get appropriate rule lists from hash table.  Merge all into
*			one list.
********************************************/

Selt<Irule> *Pat::resetRules(
	Node<Pn> *node,					// Current node in parse tree.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab, 	// Hashed rule table.
	/*DU*/
	Slist<Irule>* &rules				// Rules list to build.
	)
{
if (musts)
	rules = new Slist<Irule>(musts, true);		// Copy constructor (2).
else
	rules = new Slist<Irule>();			// Empty list.

// Traverse down the node's subtree till it branches or till a node with
// BASE attribute is set.  For each node, merge its hashed list with the
// rules so far.
Slist<Irule> *tmp;					// List of rules.
Pn *pn = 0;
_TCHAR *nname, *deacc;									// 01/29/05 AM.
//	*dejunk;													// 09/09/11 AM.
long len;													// 01/29/05 AM.
while (node)
	{
	pn = node->getData();

	nname = pn->getName();								// 01/29/05 AM.
	tmp = 0;										// Caution	// 07/25/09 AM.
	htab->hfind_lc(nname, /*DU*/ tmp);				// Get node's rules.
	// The below "fix" eliminated deaccented matching.	// 07/25/09 AM.
//	if (!tmp)	// FIX A CRASH.	// 03/28/07 AM.
//		goto xyz;	// FIX A CRASH.	// 03/28/07 AM.
   if (tmp)									// New fix.	// 07/25/09 AM.
		Irule::mergeRules(rules, tmp);	// Merge rule lists.
	
	// Get deaccented version's rules.				// 01/29/05 AM.
	// Opt: A deaccent that counted accents would be good here...
	len = _tcsclen(nname);								// 01/29/05 AM.
	deacc = Chars::create(len + 2);					// 01/29/05 AM.
	Xml::de_accent(nname,/*DU*/deacc);				// 01/29/05 AM.
	if (_tcscmp(nname,deacc))	// If accented.	// 09/20/06 AM.
		{
		tmp = 0;								// Caution.	// 07/25/09 AM.
		htab->hfind_lc(deacc, /*DU*/ tmp);			// 01/29/05 AM.
		if (tmp)								// Caution.	// 07/25/09 AM.
			Irule::mergeRules(rules, tmp);			// 01/29/05 AM.
		}
	Chars::destroy(deacc);								// 01/29/05 AM.

#ifdef _DEJUNK_
	// Get dejunk version's rules.					// 09/09/11 AM.
	// Opt: A dejunk that counted accents would be good here...
	len = _tcsclen(nname);								// 09/09/11 AM.
	dejunk = Chars::create(len + 2);					// 09/09/11 AM.
	Xml::de_accent(nname,/*DU*/dejunk);				// 09/09/11 AM.
	if (_tcscmp(nname,dejunk))	// If accented.	// 09/09/11 AM.
		{
		tmp = 0;								// Caution.	// 09/09/11 AM.
		htab->hfind_lc(dejunk, /*DU*/ tmp);			// 09/09/11 AM.
		if (tmp)								// Caution.	// 09/09/11 AM.
			Irule::mergeRules(rules, tmp);			// 09/09/11 AM.
		}
	Chars::destroy(dejunk);								// 09/09/11 AM.
#endif

//xyz:	// 03/28/07 AM.
	if (pn->getBase())									// Bottommost singlet.
		break;
	if (!(node = node->Down()))							// Leaf.
		break;
	if (node->Right())									// Branches out.
		break;
	}

return rules->getFirst();
}

/********************************************
* FN:		EXECACTIONS
* CR:		11/25/98 AM.
* SUBJ:	Execute actions for matched rule.
* RET:	True if actions executed ok, else false.		// 11/10/99 AM.
* NOTE:	Reorganizing to handle pre actions.
*			11/10/99 AM. One fix for infinite looping in REC algorithm is
*			to tell it when actions have a problem, by returning bool.
********************************************/

bool Pat::execActions(	// 11/19/99 AM.
	Nlppp *nlppp			// 11/19/99 AM.
	)
{
// Check actions may have built semantic object.		// 11/27/98 AM.
//Sem *sem;					// Semantic object built by actions, if any.
//sem = 0;

//if (Debug())
//	*gout << "  [Execute rule actions...]" << std::endl;

if (!nlppp->rule_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[execActions: Given no rule.]") << std::ends;
	errOut(&gerrStr,false);
	//delete (Tree<Pn> *) collect;	// Caller deletes.		// 11/25/98 AM.
	return false;															// 11/10/99 AM.
	}

// Return status to avoid infinite loops in REC algorithm.	// 11/10/99 AM.
return postActions(nlppp);												// 11/19/99 AM.
}


/********************************************
* FN:		MATCHRULES
* CR:		10/28/98 AM.
* SUBJ:	Match rules to current node in parse tree.
* RET:	True if a rule matched, else false.
********************************************/

bool Pat::matchRules(		// 11/19/99 AM.
	Nlppp *nlppp,				// Parse state.
	/*DU*/
	Delt<Irule>* &rules		// DOWN. Rules to match on.
									// UP. Pointer to matched rule in list.
	)
{
Node<Pn> *node = nlppp->node_;	// SAVE NODE.		// 11/20/99 AM.


Delt<Irule> *rule;
for (rule = rules; rule; rule = rule->Right())
	{
	nlppp->node_ = node;		// RESTORE NODE.	// 11/20/99 AM.
	nlppp->rmost_ = 0;		// Reset.		// RECOPT2.	// 07/23/06 AM.
	updateRestart(nlppp,node);	// Reset.	// RECOPT2.	// 07/24/06 AM.
	//if (Debug())
	//	*gout << "  Trying rule: " << *rule << std::endl;

	// Match rule starting at current node.
	// Remember triggering, directions, collect as you go.
	//Irule *rdata;									// 11/19/99 AM.
	//rdata = rule->getData();						// 11/19/99 AM.
	nlppp->rule_ = rule->getData();				// 11/19/99 AM.
	nlppp->first_ = nlppp->last_ = 0;			// 11/19/99 AM.
	nlppp->succeed_ = nlppp->fail_ = false;							// 06/10/00 AM.

	if (matchRule(nlppp)								// 11/19/99 AM.
		 && nlppp->first_	// Requiring rule to match one node. // 11/23/98 AM.
		 && checkActions(nlppp))					// 11/19/99 AM.
		{
//		if (Debug())
//			*gout << "\n  [Matched rule.]\n" << std::endl;
		rules = rule;		// UP: Return the matched rule to caller. 11/1/98 AM.
		nlppp->node_ = node;		// RESTORE NODE.	// 11/20/99 AM.
		return true;
		}
	else								// Rule failed.
		{
//		if (nlppp->span_ > nlppp->maxspan_)								// 02/04/05 AM.
//			nlppp->maxspan_ = nlppp->span_;								// 02/04/05 AM.
		// Clean up for next rule.
		endRestart(nlppp);	// RECOPT2.	// 07/17/06 AM.
		matchCleanup(nlppp->collect_);								// 11/11/98 AM.
		if (nlppp->exitpass_)								// FIX.	// 06/26/09 AM.
			{
			nlppp->node_ = node;					// RESTORE NODE.	// 06/26/09 AM.
			return false;								// "No match".	// 06/26/09 AM.
			}
		}
	}
nlppp->node_ = node;		// RESTORE NODE.	// 11/20/99 AM.
return false;		// Exhausted rules without a match.
}


/********************************************
* FN:		MATCHRULES
* CR:		10/28/98 AM.
* SUBJ:	Match rules to current node in parse tree.
* RET:	True if a rule matched, else false.
********************************************/

bool Pat::matchRules(
	Nlppp *nlppp,				// Parse state.						// 11/19/99 AM.
	/*DU*/
	Selt<Irule>* &rules		// DOWN. Rules to match on.
									// UP. Pointer to matched rule in list.
	)
{
Node<Pn> *node = nlppp->node_;				// SAVE NODE.		// 11/20/99 AM.

Selt<Irule> *rule;
for (rule = rules; rule; rule = rule->Right())
	{
	nlppp->node_ = node;							// RESTORE NODE.	// 11/20/99 AM.
	nlppp->rmost_ = 0;		// Reset.		// RECOPT2.	// 07/23/06 AM.
	updateRestart(nlppp,node);	// Reset.	// RECOPT2.	// 07/24/06 AM.
	//if (Debug())
	//	*gout << "  Trying rule: " << *rule << std::endl;

	// Match rule starting at current node.
	// Remember triggering, directions, collect as you go.
	//Irule *rdata;														// 11/19/99 AM.
	//rdata = rule->getData();											// 11/19/99 AM.
	nlppp->rule_ = rule->getData();		// FIX					// 11/19/99 AM.
	nlppp->first_ = nlppp->last_ = 0;								// 11/19/99 AM.
	nlppp->succeed_ = nlppp->fail_ = false;							// 06/10/00 AM.
	nlppp->excised_ = false;												// 04/28/01 AM.
	nlppp->noop_ = false;													// 08/12/02 AM.
//	nlppp->span_ = 0;															// 02/04/05 AM.

	if (matchRule(nlppp)													// 11/19/99 AM.
		 && nlppp->first_	// Requiring rule to match 1 node.	// 11/23/98 AM.
		 && checkActions(nlppp))								// 11/19/99 AM.
		{
//		if (Debug())
//			*gout << "\n  [Matched rule.]\n" << std::endl;
		rules = rule;	// UP: Return matched rule to caller.	// 11/01/98 AM.
		nlppp->node_ = node;						// RESTORE NODE.	// 11/20/99 AM.
		return true;
		}
	else								// Rule failed.
		{
//		if (nlppp->span_ > nlppp->maxspan_)								// 02/04/05 AM.
//			nlppp->maxspan_ = nlppp->span_;								// 02/04/05 AM.
		// Clean up for next rule.
		endRestart(nlppp);	// RECOPT2.	// 07/17/06 AM.
		matchCleanup(nlppp->collect_);								// 11/11/98 AM.
		if (nlppp->exitpass_)								// FIX.	// 06/26/09 AM.
			{
			nlppp->node_ = node;					// RESTORE NODE.	// 06/26/09 AM.
			return false;								// "No match".	// 06/26/09 AM.
			}
		}
	}
nlppp->node_ = node;								// RESTORE NODE.	// 11/20/99 AM.
return false;		// Exhausted rules without a match.
}


/********************************************
* FN:		MATCHRULE
* CR:		10/26/98 AM.
* SUBJ:	Match the rule to current node in parse tree.
* NOTE:	Working this bottom up.  Need to account for direction,
*			triggering, rule element features, collect data as you go.
*			Try to do same collection for single- and double-tier reductions.
*			The left boundary in particular is so that we don't recursively
*			match rules within the parse tree phrase.  (May want a flag to
*			allow this to happen -- sometimes may want a truly recursive pass.
*			But then for every match, should start over in the phrase.)
* EX:		Double tier is as follows:
*				np <- det quan adj noun.
*			builds
*							np
*							 |
*				-------------------------
*				det    quan    adj     noun
*				 |       |      |        |
*				the    three   green   monkeys
*
*			That is, double-tier retains the rule elements as well as the
*			nodes from the tree.  A "two-level" structure is built on match.
*			This is useful for tracking what wildcards matched, and for more
*			info about how a rule matched the parse tree.
*	ALG:	Get the rule phrase.
*			If triggering, get the trigger elt.
*			Ie, get the rule element to start matchin on.
*			Match node(s) against each rule element.
*			Make sure pre-actions are performed also.
*			  (Per element or per rule.  May be some of both.)
*	  (It may be that the key=val pairs will take care of much of
*		what pre-actions was intended to do.  Or pre-actions may sometimes
*		be a short-hand, rather than repeating the same condition in every
*		rule element.)
*			Need a node range that we must keep within.
*
*			First, match the trigger!
*			If matched, collect appropriate data...
*			Go backward if possible.
*		10/28/98 AM. Each node in the collect phrase will mirror one RULE
*		ELT.  This will serve tracking best, even for one-tier reductions.
*		Collection in the forward and backward directions may be asymmetric.
********************************************/

bool Pat::matchRule(
	Nlppp *nlppp
	)
{

if (!nlppp->rule_ || !nlppp->node_)
	{
	Parse *parse = nlppp->parse_;	// 04/04/10 AM.
	parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[matchRule: Missing rule or node.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

// Unpack the rule.

Dlist<Ielt> *rlist;			// Rule phrase list.
Isugg *sugg;					// Suggested elt.
Delt<Ielt> *trig;				// Rule's trigger elt.
//Dlist<Ipre> *pre = rule->getPre();			// Pre-actions.
//Dlist<Ipost> *post = rule->getPost();		// Post-actions.
													// ie, the one to match first.
if (!nlppp->rule_->unpack(/*UP*/ rlist, sugg, trig))
	return false;

Delt<Ielt> *rphrase = rlist->getFirst();			// Rule phrase.
if (!rphrase)
	{
	Parse *parse = nlppp->parse_;	// 04/04/10 AM.
	parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[matchRule: Empty rule phrase.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

//nlppp->span_ = 0;		// Reset for current rule.	// RECOPT1	// 04/18/06 AM.

// Match first rule element.
Delt<Ielt> *elt;								// Current rule element.
enum Dir dir = LEFT;							// Default. 12/04/98 AM.

if (!trig)
	{
	// Start matching with first elt of rule.
	if (!matchFirst(rphrase,nlppp, /*DU*/ elt,dir))				// 11/19/99 AM.
	//if (!matchFirst(rphrase,start,end,rule,
	//								/*DU*/elt,node,collect,first,last,dir))
		return false;
	}
else
	{
//	if (Debug())
//		*gout << "   [Trying trigger.]" << std::endl;
	//if (!matchTrigger(trig,start,end,rule,						// 11/19/99 AM.
	//								/*DU*/elt,node,collect,first,last,dir))
	if (!matchTrigger(trig,nlppp, /*DU*/ elt,dir))				// 11/19/99 AM.
		return false;
	}

if (dir == DONE)
	return true;		// Done with rule by matching trigger alone.

// Tracking of pending wildcards.
Node<Pn> *wildone = 0; // First collect node that is an unmatched wildcard.
int wildmin = -1;		  // Minimum number of nodes that wildcards must match.
int wildmax = -1;		  // Total number of nodes that wildcards could cover.
							  // Warn: zero means unrestricted.

// Match, moving left from trigger. ("backward")
while (dir == LEFT)
	{
	//if (!matchLeft(trig,start,end,rule,							// 11/19/99 AM.
	//			/*DU*/ elt,node,collect,first,last,dir,wildone,wildmin,wildmax))
	if (!matchLeft(trig,nlppp,											// 11/19/99 AM.
				/*DU*/ elt,dir,wildone,wildmin,wildmax))
		return false;
	}

if (dir == DONE)
	return true;

// Reset wildcard tracking.
// If accepting trigger as wildcard, modify these based on the trigger.
wildone = 0;
wildmin = wildmax = -1;

// Match, moving right from first rule element. ("forward")
while (dir == RIGHT)
	{
	//if (!matchRight(trig,start,end,rule,							// 11/19/99 AM.
	//		/*DU*/ elt,node,collect,first,last,dir,wildone,wildmin,wildmax))
	if (!matchRight(trig,nlppp,										// 11/19/99 AM.
			/*DU*/ elt,dir,wildone,wildmin,wildmax))
		{
//		if (nlppp->span_ > nlppp->maxspan_)		// RECOPT1	// 04/19/06 AM.
//			nlppp->maxspan_ = nlppp->span_;		// RECOPT1	// 04/26/06 AM.
		return false;
		}
	}

// Successfully matched current rule.
return true;
}


/********************************************
* FN:		MATCHFIRST
* CR:		10/28/98 AM.
* SUBJ:	Match the first element of a trigger-less rule.
* RET:	True if successful match of first elt, else false.
* NOTE:	If first element is "real", can treat as though it's
*			the trigger of the rule.  If it's wild or optional, eg,
*			we'll need to handle it here.  This fn is a hook.
********************************************/

bool Pat::matchFirst(
	Delt<Ielt> *elt1,		// First rule element.
	Nlppp *nlppp,			// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir			// Direction to continue matching in.
	)
{
// Todo: Handle cases like rules starting with optionals and wildcards.

// Match some special rule elts, like start of phrase.// 01/30/99 AM.
Ielt *ee;
ee = elt1->getData();
if (!_tcscmp(ee->getName(), _T("_xSTART")))			// If it's start of phrase elt.
	{
	if (!matchSTART(ee, nlppp->node_))						// 10/05/99 AM.
		return false;
	makeCollect(nlppp->collect_, elt1, 0, 0, LEFT);
	if (!(elt = elt1->Right()))
		{
		Parse *parse = nlppp->parse_;	// 04/04/10 AM.
		parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
		std::_t_strstream gerrStr;
		gerrStr << _T("[Rule ending with _xSTART not allowed.]") << std::ends;
		parse->errOut(&gerrStr,false);
		return false;
		}
	// Set up for matching right.
	dir = RIGHT;
	return true;
	}

bool matched =															// 08/14/03 AM.
			matchTrigger(elt1,nlppp, /*DU*/ elt,dir);			// 11/19/99 AM.

if (matched)															// 08/14/03 AM.
	return true;														// 08/14/03 AM.

// Moved code here.													// 08/14/03 AM.
// Case where _xSTART is in match list.						// 08/10/03 AM.
if (!nlppp->node_->Left()											// 08/10/03 AM.
	&& eltMatches(ee, _T("_xSTART")) )								// 08/10/03 AM.
	{
	makeCollect(nlppp->collect_, elt1, 0, 0, LEFT);			// 08/10/03 AM.
	if (!(elt = elt1->Right()))									// 08/10/03 AM.
		{
		Parse *parse = nlppp->parse_;	// 04/04/10 AM.
		parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
		std::_t_strstream gerrStr;			// 08/10/03 AM.
		gerrStr << _T("[Rule ending with _xSTART not allowed.]") << std::ends;
		parse->errOut(&gerrStr,false);													// 08/10/03 AM.
		return false;													// 08/10/03 AM.
		}
	// Set up for matching right.									// 08/10/03 AM.
	dir = RIGHT;														// 08/10/03 AM.
	return true;														// 08/10/03 AM.
	}

return false;															// 08/14/03 AM.
}


/********************************************
* FN:		MATCHTRIGGER
* CR:		10/27/98 AM.
* SUBJ:	Match the trigger element of the rule.
* RET:	True if successful match of trigger, else false.
* NOTE:	Sets up the 'collect' tree.
*			Trigger must be a "real" nonoptional nonwildcard element,
*			so is handled
*			differently from other elements of the rule, which
*			can be wildcards or optional.
*			This function may become same as matchElt, to be written later.
*			Set direction for next match.  If possible
*			to go left, set that as direction.  Else, set up for right.
*			(left = backward, right = forward).
*			Need to handle modes like repetition of the trigger!
********************************************/

bool Pat::matchTrigger(	// 11/19/99 AM.
	Delt<Ielt> *trig,		// Trigger rule element.
	Nlppp *nlppp,			// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir			// Direction to continue matching in.
	)
{
if (!(elt = trig))
	{
	Parse *parse = nlppp->parse_;	// 04/04/10 AM.
	parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[matchTrigger: Given no trigger.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

Delt<Ielt> *curr;
curr = elt;				// 11/23/98 AM.


// MATCH RULE ELT WITH CURRENT NODE.
// matchNode();
//Pn *pn;

Ielt *ielt = curr->getData();		// OPT	// 10/05/99 AM.

if (wildElt(ielt))			// Trigger or first elt is a wildcard.
	{
	return warnWild(nlppp->rule_,nlppp);
	// Check on restrictions.
	// Check on range.
	// If fail, return.
	// If success, set up next element match.
	// If no next element in this dir, stretch out the wildcard match.
	// Warn user about foolish wildcard usage.
	// Success and set up continued matching.
	}

// Match trigger rule elt to current node.
//if (!Match(elt, node))
//	{
//	//if (Debug()) *gout << "  no match." << std::endl;
//	return false;
//	}

// 11/09/98 AM. Replaced above match code with below, to handle reps.
// MATCH A REAL NODE.  Handle pending wildcards, optional, repeat.
Node<Pn> *real = 0;	// First real node matched, moving right.
long tcount = 0;														// 02/04/05 AM.
//if (!matchRealRight(trig,end,0,0,0, /*DU*/ real,node))	// 10/06/99 AM.
if (!matchRealRight(ielt,
		nlppp->end_,0,0,0,nlppp, /*DU*/ real,nlppp->node_,	// 10/06/99 AM.
													tcount))				// 02/04/05 AM.
	{
//	nlppp->span_ += tcount;											// 02/04/05 AM.

	// 11/23/98 AM.
	// Trying to handle optional trigger or first element.


	if (optionalElt(ielt))		// If current rule elt is optional.
		{
		makeCollect(nlppp->collect_, curr, 0, 0, LEFT);
		if ((elt = curr->Left()))	// Move left to next rule elt.
			return true;
		// No more rule elts in left direction.
		// Succeed and try to continue right.
		// STAY AT node.
		if ((elt = curr->Right()))
			{
			dir = RIGHT;
			return true;
			}
		// Fail because this rule matched no nodes.
		return false;
		}
	return false;
	}
//else																		// 02/04/05 AM.
//	nlppp->span_ += tcount;											// 02/04/05 AM.

//if (Debug())
//	*gout << "   [Literal match]" << std::endl;

// Build a collect (tracking structure) for current rule elt.
//Node<Pn> *ncoll;	// The collect node for the trigger.	// 05/04/00 AM.
//ncoll = makeCollect(collect, trig, node, node, LEFT);
//ncoll =																	// 05/04/00 AM.
makeCollect(nlppp->collect_, trig, real, nlppp->node_, LEFT);

// Now collect incorporates first and last!
// collect->getRoot()->Down()  == first
// collect->getLast()->eDown() == last
// (Not necessarily, if some of the collect nodes are pending
// wildcards.)

// 11/09/98 AM. Handling repeats of trigger or first rule elt.
//first = last = node;			// Parse tree nodes matched by rule.
nlppp->first_ = real;				// 11/09/98 AM.
nlppp->last_  = nlppp->node_;				// 11/09 98 AM.
// Set up for match of next element.
if ((elt = trig->Left()))
	{
	dir = LEFT;
//	node = nodeLeft(node, start);	// Try to back up one node.
	nlppp->node_ = nodeLeft(real, nlppp->start_); // Try to back up one node.
	return true;
	}

// Else, try to continue to the right of trigger.
return switchRight(trig, nlppp, /*DU*/ elt, dir);			// 11/19/99 AM.
}


/********************************************
* FN:		MATCHLEFT
* CR:		10/29/98 AM.
* SUBJ:	Match backward from the trigger element of the rule.
* RET:	True if successful, else false.
* NOTE:	Just handling some simple stuff at first.
*			matchRight will be similar, but asymmetric.
* ASS:	Matching of trigger or first elt has set us up to continue.
*			Matching of prior nodes to left of trigger has set us up also.
* ALG:	For current rule elt left of the trigger, match nodes going
*			left in the parse tree.
*			Need to know if wildards are pending: if current match failed,
*			see if prior was wildcard.
********************************************/

bool Pat::matchLeft(		// 11/19/99 AM.
	Delt<Ielt> *trig,		// Trigger rule element.
	Nlppp *nlppp,			// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir,		// Direction to continue matching in.
	Node<Pn>* &wildone,	// First unfilled wildcard.
	int &wildmin,			// Min nodes that wildcards must cover.
	int &wildmax			// Total possible nodes that wildcards can cover.
	)
{
Delt<Ielt> *curr;			// Current rule elt.

curr = elt;
Ielt *ielt = curr->getData();		// OPT	// 10/05/99 AM.

// If current rule elt is a wildcard, handle it.
if (wildElt(ielt))
	//return wildLeft(trig,start,end,rule,
	//		/*DU*/ elt,node,collect,first,last,dir,wildone,wildmin,wildmax);
	return wildLeft(trig,nlppp,							// 11/19/99 AM.
			/*DU*/ elt,dir,wildone,wildmin,wildmax);

// Current rule elt is not a wildcard.

if (!nlppp->node_)
	{
	// 11/06/98 AM. Handle indefinite number of optionals at left end of rule.
	if (optionalElt(ielt))		// If current rule elt is optional, succeed.
		{
		if ((elt = curr->Left()))	// Move left to next rule elt.
			{
			makeCollect(nlppp->collect_,
										curr, 0, 0, LEFT);		// 11/24/98 AM. Fix.
			return true;
			}
		// No more rule elts in left direction.
		if (wildone)				// If wilds pending, warn and fail.
			return warnWild(nlppp->rule_,nlppp);
		// Else, succeed and try to continue right.
		makeCollect(nlppp->collect_,
									curr, 0, 0, LEFT);			// 11/24/98 AM. Fix.
		//return switchRight(trig, last, end, /*DU*/ elt, node, dir);
		return switchRight(trig, nlppp, /*DU*/ elt, dir);		// 11/19/99 AM.
		}

	// Match some special rule elts, like start of phrase.// 01/30/99 AM.
	if (matchSTART(ielt, nlppp->first_))	// FIX		// 10/07/99 AM.
//	if (matchSTART(ielt, nlppp->last_))		// FIX		// 10/07/99 AM.
		{
		if (elt->Left())
			{
			Parse *parse = nlppp->parse_;	// 04/04/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[_xSTART must be first rule element.]") << std::ends;
			parse->errOut(&gerrStr,false);
			return false;
			}
		makeCollect(nlppp->collect_, curr, 0, 0, LEFT);
		//return switchRight(trig, last, end, /*DU*/ elt, node, dir);
		return switchRight(trig, nlppp, /*DU*/ elt, dir);		// 11/19/99 AM.
		}
	// Line below is inconsistent with matchRight?  // 01/30/99 AM.
	return false;
	}

Node<Pn> *save;		// Save the current position of node.
save = nlppp->node_;

// MATCH A REAL NODE.  Handle pending wildcards, optional, repeat.
Node<Pn> *real = 0;	// First real node matched, moving left.
// OPT // 10/06/99 AM.
//if (!matchRealLeft(curr,start,wildone,wildmin,wildmax, /*DU*/ real,node))
if (!matchRealLeft(ielt,nlppp->start_,wildone,wildmin,wildmax,
				/*DU*/ real,nlppp->node_))
	{
	if (optionalElt(ielt))		// If current rule elt is optional, succeed.
		{
		nlppp->node_ = save;				// Reset current node.
		if ((elt = curr->Left()))	// Move left to next rule elt.
			{
			makeCollect(nlppp->collect_, curr, 0, 0, LEFT);	// 11/23/98 AM.
			return true;
			}
		// No more rule elts in left direction.
		if (wildone)				// If wilds pending, warn and fail.
			return warnWild(nlppp->rule_,nlppp);
		// Else, succeed and try to continue right.
		makeCollect(nlppp->collect_, curr, 0, 0, LEFT);	// 11/23/98 AM.
		//return switchRight(trig, last, end, /*DU*/ elt, node, dir);
		return switchRight(trig, nlppp, /*DU*/ elt, dir);		// 11/19/99 AM.
		}
	return false;
	}

// Build a collect (tracking structure) for current rule elt.
Node<Pn> *ncoll;
ncoll = makeCollect(nlppp->collect_, curr, nlppp->node_, real, LEFT);

// Backfill wildcards.
if (!wildFillLeft(wildone, ncoll, save, real))
	return false;				// Failed to assign to wildcards.

// Update the tracking for nodes matched to current rule.
nlppp->first_ = nlppp->node_; // Leftmost parse tree node matched by rule.
if (!nlppp->last_)
	nlppp->last_ = save;			// Eg, if trigger was a wildcard.

// Update the tracking of wildcards.
// NOTE:	Leaving this to the caller.
// 01/17/99 AM. FIX. The caller isn't clearing as needed!
wildone = 0;
wildmin = wildmax = -1;

// Set up for match of next element.
if ((elt = curr->Left()))
	{
	//dir = LEFT;						// Continue moving left.
	nlppp->node_ = nodeLeft(nlppp->node_,
		nlppp->start_);	// Try to back up one node.
	return true;						// (Wildcard elts could succeed, someday.)
	}

// Else, try to continue to the right of trigger.
//return switchRight(trig, last, end, /*DU*/ elt, node, dir);
return switchRight(trig, nlppp, /*DU*/ elt, dir);		// 11/19/99 AM.
}


/********************************************
* FN:		MATCHRIGHT
* CR:		10/29/98 AM.
* SUBJ:	Match forward from the first element of the rule.
* RET:	True if successful, else false.
* ASS:	Trigger and leftward (backward) processing have set us up.
* ALG:	Match, moving to the right of the trigger.
* WARN:	A confusing thing about these algorithms is that the left side
*			could have just processed a node in outer Siberia (way left ;-)
********************************************/

bool Pat::matchRight(	// 11/19/99 AM.
	Delt<Ielt> *trig,		// Trigger rule element. (NOT USED HERE).
	Nlppp *nlppp,			// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir,		// Direction to continue matching in.
	Node<Pn>* &wildone,	// First unfilled wildcard.
	int &wildmin,			// Min nodes that wildcards must cover.
	int &wildmax			// Total possible nodes that wildcards can cover.
	)
{
Delt<Ielt> *curr;			// Current rule elt.

curr = elt;
Ielt *ielt = curr->getData();		// OPT	// 10/05/99 AM.

// If current rule elt is a wildcard, handle it.
if (wildElt(ielt))
	//return wildRight(trig,start,end,rule,						// 11/19/99 AM.
	//		/*DU*/ elt,node,collect,first,last,dir,wildone,wildmin,wildmax);
	return wildRight(trig,nlppp,										// 11/19/99 AM.
			/*DU*/ elt,dir,wildone,wildmin,wildmax);

// Current rule elt is not a wildcard.

if (!nlppp->node_)
	{
	// 11/06/98 AM. Handle any number of optionals at right end of rule.
	if (optionalElt(ielt))		// If current rule elt is optional, succeed.
		{
		if ((elt = curr->Right()))	// Move right to next rule elt.
			{
			makeCollect(nlppp->collect_, curr, 0, 0, RIGHT);	// 11/23/98 AM.
			return true;
			}
		// No more rule elts in right direction.
		if (wildone)				// If wilds pending, warn and fail.
			return warnWild(nlppp->rule_,nlppp);
		// Else, success because last elt is optional.
		makeCollect(nlppp->collect_, curr, 0, 0, RIGHT);	// 11/23/98 AM.
		dir = DONE;					// 11/06/98 AM. Bug fix.
		return true;				// 11/06/98 AM. Bug fix.
		}

	// Match some special rule elts, like end of file.		// 11/25/98 AM.
	if (matchEOF(ielt, nlppp->last_)
	    || matchEND(ielt,nlppp->last_) // Match end of phrase.// 01/21/99 AM.
		 )
		{
		if (curr->Right())	// 01/30/99 AM.
			{
			Parse *parse = nlppp->parse_;	// 04/02/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/02/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[_xEND or _xEOF must be last rule element.]") << std::ends;
			parse->errOut(&gerrStr,false);
			return false;			// Make sure this is last rule elt.
			}
		// Matched end of file.
		makeCollect(nlppp->collect_, curr, 0, 0, RIGHT);
		dir = DONE;
		return true;
		}
	// Shouldn't there be a return false here? // 01/30/99 AM.
	}

// Unhandled cases! e.g.,  _X <- _Y _xWILD _xEOF @@			// 12/19/99 AM.
_TCHAR *nm = ielt->getName();											// 12/19/99 AM.
if (wildone &&																// 12/19/99 AM.
    (!_tcscmp(nm, _T("_xEOF")) || !_tcscmp(nm, _T("_xEND"))) )			// 12/19/99 AM.
	{
	// Must be something like ... _xWILD _xEOF @@ or _xWILD _xEND @@
	return matchRightspecial(											// 12/19/99 AM.
			nm,nlppp, /*DU*/ elt,dir,wildone,wildmin,wildmax);	// 12/19/99 AM.
	}

Node<Pn> *save;		// Save the current position of node.
save = nlppp->node_;

// MATCH A REAL NODE.  Handle pending wildcards, optional, repeat.
Node<Pn> *real = 0;	// First real node matched, moving right.
long tcount = 0;														// 02/04/05 AM.
// OPT // 10/06/99 AM.
//if (!matchRealRight(curr,end,wildone,wildmin,wildmax, /*DU*/ real,node))
if (!matchRealRight(ielt,nlppp->end_,wildone,wildmin,wildmax,nlppp,
			/*DU*/ real,nlppp->node_,tcount))
	{
//	nlppp->span_ += tcount;										// 02/04/05 AM.
	if (optionalElt(ielt))		// If current rule elt is optional, succeed.
		{
		nlppp->node_ = save;				// Reset current node.
		if ((elt = curr->Right()))	// Move right to next rule elt.
			{
			makeCollect(nlppp->collect_,
											curr, 0, 0, RIGHT);	// 11/24/98 AM. Fix.
			return true;
			}
		// No more rule elts in right direction.
		if (wildone)				// If wilds pending, warn and fail.
			return warnWild(nlppp->rule_,nlppp);
		// Else, success because last elt is optional.
		makeCollect(nlppp->collect_, curr, 0, 0, RIGHT);	// 11/24/98 AM. Fix.
		dir = DONE;					// 11/06/98 AM. Bug fix.
		return true;				// 11/06/98 AM. Bug fix.
		}
	return false;
	}
//else																		// 02/04/05 AM.
//	nlppp->span_ += tcount;											// 02/04/05 AM.

// Build a collect (tracking structure) for current rule elt.
Node<Pn> *ncoll;
//ncoll = makeCollect(collect, curr, node, real, RIGHT);	// 11/21/98 AM. Bug.
ncoll = makeCollect(nlppp->collect_,
			curr, real, nlppp->node_, RIGHT);	// 11/21/98 AM. Fix.

// Backfill wildcards.
if (!wildFillRight(wildone, ncoll, save, real))
	return false;				// Failed to assign to wildcards.

// 01/17/99 AM. FIX.  Must clear the wildcard tracking.
wildone = 0;
wildmin = wildmax = -1;

// Update the tracking for nodes matched to the current rule.
nlppp->last_ = nlppp->node_;			// Rightmost node matched by rule.
if (!nlppp->first_)
	nlppp->first_ = save;

// If first not set because of pending wildcard at rule start, may want
// to set first here also.

// Set up for match of next element.
if ((elt = curr->Right()))
	{

	// Try to go forward one node.
	if (nlppp->last_ && (nlppp->last_ != nlppp->end_))
		{
		nlppp->node_ = nlppp->last_->Right();
		updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/17/06 AM.
		}
	else
		nlppp->node_ = 0;
	return true;
	}

// Couldn't go right.
dir = DONE;
return true;
}


/********************************************
* FN:		WILDLEFT
* CR:		10/29/98 AM.
* SUBJ:	Match wildcard in the left direction. ("backward").
* RET:	True if successful, else false.
* NOTE:	
* ASS:	Given elt is a wildcard.
********************************************/

inline bool Pat::wildLeft(	// 11/19/99 AM.
	Delt<Ielt> *trig,		// Trigger rule element.
	Nlppp *nlppp,			// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir,		// Direction to continue matching in.
	Node<Pn>* &wildone,	// First unfilled wildcard.
	int &wildmin,			// Min nodes that wildcard must cover.
	int &wildmax			// Total possible nodes that wildcards can cover.
	)
{
if (!nlppp->node_)
	{
	if (elt->Left())
		{
		makeCollect(nlppp->collect_, elt, 0, 0, LEFT);	// 11/25/98 AM. Fix.
		elt = elt->Left();
		return true;		// To eventually fail (till wild start allowed).
		}
	return warnWild(nlppp->rule_,nlppp);
	}

Delt<Ielt> *curr;			// Current rule elt.

curr = elt;					// The wildcard.

// Get wildcard attributes from rule elt into collect node.
// Seems easiest for collect node to point to its corresponding
// rule elt.  (We're using the rule elts to match with, so they
// are accessible.  Compiled rules will need totally revised algorithms.)
Node<Pn> *ncoll;					// The collect node for the trigger.
ncoll = makeCollect(nlppp->collect_, curr, 0, 0, LEFT);
// Note: wildcard doesn't yet cover current node.

// Note: Not setting first to current node.  It's not matched yet.

// Set up for match of next element.
Ielt *ielt;
int min,max;
ielt = curr->getData();
ielt->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
if (min == -1)
	min = 0;					// Default.
if (max == -1)				// Default.
	max = 0;					// Unlimited is the default wildcard max.
if ((elt = curr->Left()))
	{
	dir = LEFT;

	// Note: Not backing up one node.
	// Let next elt know that wildcard is pending.
	if (!wildone)					// No unsatisfied wildcards going left.
		{
		wildone = ncoll;			// First unsatisfied wildcard.
		// wildmax = max coverage of current rule elt.  0 means unlimited.
		wildmin = min;
		wildmax = max;
		}
	else							// There are pending wildcards.
		{
		wildmin += min;		// Add to the minimum that must be covered.

		if (!max)				// Current wildcard is unlimited.
			wildmax = 0;		// Total is unlimited.
		else if (!wildmax)	// Total already unlimited.
			;						// Leave it be.
		else
			wildmax += max;	// Add limited coverage to the limited so far.
		}
	return true;
	}

// Failing for now on wildcard at start of rule.
return warnWild(nlppp->rule_,nlppp);

// Else, try to continue to the right of trigger.
//return switchRight(trig, last, end, /*DU*/ elt, node, dir);
return switchRight(trig, nlppp, /*DU*/ elt, dir);		// 11/19/99 AM.
}


/********************************************
* FN:		WILDRIGHT
* CR:		11/03/98 AM.
* SUBJ:	Match wildcard in the right direction. ("forward").
* RET:	True if successful, else false.
* NOTE:	
* ASS:	Given elt is a wildcard.
* WARN:	SIMULTANEOUS UPDATES TO wildLeft.
********************************************/

inline bool Pat::wildRight(	// 11/19/99 AM.
	Delt<Ielt> *trig,		// Trigger rule element.
	Nlppp *nlppp,			// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir,		// Direction to continue matching in.
	Node<Pn>* &wildone,	// First unfilled wildcard.
	int &wildmin,			// Min nodes that wildcard must cover.
	int &wildmax			// Total possible nodes that wildcards can cover.
	)
{
if (!nlppp->node_)
	{
	if (elt->Right())
		{
		makeCollect(nlppp->collect_, elt, 0, 0, RIGHT);	// 11/25/98 AM. Fix.
		elt = elt->Right();
		return true;		// To eventually fail (till wild end allowed).
		}
	return warnWild(nlppp->rule_,nlppp);
	}

Delt<Ielt> *curr;			// Current rule elt.

curr = elt;					// The wildcard.

// Get wildcard attributes from rule elt into collect node.
// Seems easiest for collect node to point to its corresponding
// rule elt.  (We're using the rule elts to match with, so they
// are accessible.  Compiled rules will need totally revised algorithms.)
Node<Pn> *ncoll;					// The collect node for the trigger.
ncoll = makeCollect(nlppp->collect_, curr, 0, 0, RIGHT);
// Note: wildcard doesn't yet cover current node.

// Note: Not setting first to current node.  It's not matched yet.

// Set up for match of next element.
Ielt *ielt;
int min,max;
ielt = curr->getData();
ielt->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
if (min == -1)
	min = 0;					// Default.
if (max == -1)				// Default.
	max = 0;					// Unlimited is the default wildcard max.
if ((elt = curr->Right()))
	{
	dir = RIGHT;

	// Note: Not backing up one node.
	// Let next elt know that wildcard is pending.
	if (!wildone)					// No unsatisfied wildcards going left.
		{
		wildone = ncoll;			// First unsatisfied wildcard.
		// wildmax = max coverage of current rule elt.  0 means unlimited.
		wildmin = min;
		wildmax = max;
		}
	else							// There are pending wildcards.
		{
		wildmin += min;		// Add to the minimum that must be covered.

		if (!max)				// Current wildcard is unlimited.
			wildmax = 0;		// Total is unlimited.
		else if (!wildmax)	// Total already unlimited.
			;						// Leave it be.
		else
			wildmax += max;	// Add limited coverage to the limited so far.
		}
	return true;
	}

// Failing for now on wildcard at end of rule.
return warnWild(nlppp->rule_,nlppp);
}


/********************************************
* FN:		MATCHREALLEFT
* CR:		10/30/98 AM.
* SUBJ:	Match a real parse tree node with current rule element, going left.
* RET:	True if successful match, else false.
* OPT:	Make this inline.  Not parameterizing for left/right direction.
* NOTE:	Doing "extra" work here to not skip over the item we are looking
*			for.
********************************************/

bool Pat::matchRealLeft(
	Ielt *ielt,		// Rule elt to continue matching against.
	Node<Pn> *start,		// Left bound of tree phrase (inclusive).
								// If null, no left bound.
	Node<Pn> *wildone,	// First unfilled wildcard.
	int wildmin,			// Min nodes that wildcards must cover.
	int wildmax,			// Total possible nodes that wildcards can cover.
	/*DU*/
	Node<Pn>* &real,		// First real node matched (moving left).
								// (Allows for repetition.)
	Node<Pn>* &node		// DN: Current node.  UP: Last real node matched.
	)
{
if (!node)		// This can happen now, to handle wildcard at start of rule.
	return false;

// If a wildcard is pending, look for a match to the current elt
// as far as possible.
long count;					// Count nodes that are traversed.
count = 0;
Node<Pn> *test;
test = node;
for (;;)		// Look for real match till done.
	{
	++count;

	// Actual match of rule elt to node(s).
	if (trueMatchLeft(ielt, start, /*DU*/ test, real))
		{
		// FAIL if wildcards pending and we didn't cover the minimum. (DESIGN)
		if (wildone && count <= wildmin)
			return false;

//		if (Debug())
//			*gout << "   [Real match]" << std::endl;
		node = test;
		return true;
		}

	if (!wildone)
		return false;		// No wildcards pending.  Fail.
	if (wildmax && count > wildmax)
		return false;		// Exceeded total coverage of wildcards.
	if (test == start)	// Can't move left.
		return false;
	// Didn't exceed wildcard coverage, or there's unlimited coverage.
	if (!(test = test->Left()))				// Try to move left.
		return false;
	// Successful move left of one node.  Now try to match it.
	}
}


/********************************************
* FN:		MATCHREALRIGHT
* CR:		11/03/98 AM.
* SUBJ:	Match a real parse tree node with current rule element, going right.
* RET:	True if successful match, else false.
* OPT:	Make this inline.  Not parameterizing for left/right direction.
* NOTE:	Doing "extra" work here to not skip over the item we are looking
*			for.
********************************************/

bool Pat::matchRealRight(
	Ielt *ielt,		// Rule elt to continue matching against.
	Node<Pn> *end,		// Right bound of tree phrase (inclusive).
								// If null, no right bound.
	Node<Pn> *wildone,	// First unfilled wildcard.
	int wildmin,			// Min nodes that wildcards must cover.
	int wildmax,			// Total possible nodes that wildcards can cover.
	Nlppp *nlppp,			// RECOPT1	// 04/26/06 AM.
	/*DU*/
	Node<Pn>* &real,		// First real node matched (moving right).
								// (Allows for repetition.)
	Node<Pn>* &node,		// DN: Current node.  UP: Last real node matched.
	long &tot															// 02/04/05 AM.
	)
{
if (!node)		// This can happen now, to handle wildcard at end of rule.
	return false;

// If a wildcard is pending, look for a match to the current elt
// as far as possible.
long count;					// Count nodes that are traversed.
count = 0;
Node<Pn> *test;
test = node;
int tcount = 0;	// Not sure if this should update count.	// 02/04/05 AM.
tot = 0;																		// 02/04/05 AM.
Node<Pn> *saverightmost = 0;							// RECOPT2.	// 07/23/06 AM.
for (;;)		// Look for real match till done.
	{
	++count;

	// Actual match of rule elt to node(s).
	if (trueMatchRight(ielt, end,nlppp, /*DU*/ test,real,	// OPT	// 10/06/99 AM.
														tcount))				// 02/04/05 AM.
		{
		tot += tcount;														// 02/04/05 AM.

		// FAIL if wildcards pending and we didn't cover the minimum. (DESIGN)
		if (wildone && count <= wildmin)
			return false;

//		if (Debug())
//			*gout << "   [Real match]" << std::endl;
//		nlppp->maxspan_ += count;	// RECOPT1	// 04/26/06 AM.
		node = test;
		return true;
		}
	else																		// 02/04/05 AM.
		tot += tcount;														// 02/04/05 AM.

	if (!wildone)
		return false;		// No wildcards pending.  Fail.
	if (wildmax && count > wildmax)
		{
		if (saverightmost)								// RECOPT2.	// 07/23/06 AM.
			{
			nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/23/06 AM.
			nlppp->rmost_ = 0;							// RECOPT2.	// 07/23/06 AM.
			}
		return false;		// Exceeded total coverage of wildcards.
		}
	if (test == end)	// Can't move right.
		{
		if (saverightmost)								// RECOPT2.	// 07/23/06 AM.
			{
			nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/23/06 AM.
			nlppp->rmost_ = 0;							// RECOPT2.	// 07/23/06 AM.
			}
		return false;
		}
	// Didn't exceed wildcard coverage, or there's unlimited coverage.
	if (!(test = test->Right()))				// Try to move right.
		{
		if (saverightmost)								// RECOPT2.	// 07/23/06 AM.
			{
			nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/23/06 AM.
			nlppp->rmost_ = 0;							// RECOPT2.	// 07/23/06 AM.
			}
		return false;
		}
	// Successful move right of one node.  Now try to match it.
	updateRestart(nlppp, test);					// RECOPT2	// 07/17/06 AM.
	if (nlppp->rmost_)	// Current rule is resetting rightmost.	// RECOPT2.	// 07/22/06 AM.
		saverightmost = test;						// RECOPT2.	// 07/22/06 AM.
	}
}


/********************************************
* FN:		TRUEMATCHLEFT
* CR:		10/31/98 AM.
* SUBJ:	Truly match node to rule element, with repetitions,features,etc.
* RET:	True if succeeded, else failed.
********************************************/

bool Pat::trueMatchLeft(
	Ielt *ielt,					// OPT // 10/06/99 AM.
	Node<Pn> *start,
	/*DU*/
	Node<Pn>* &node,			// DN: Current node.  UP: Last real matched node.
	Node<Pn>* &real			// First real matched node, moving left.
	)
{
// Get repetitions, conditions, features....
//Ielt *ielt;
int min, max;

if (!ielt || !node)
	return false;
//ielt = elt->getData();
ielt->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
if (min < 0 || max < 0)
	return false;
int count;			// Count the number of matches.

// (max == 0 means indefinite repeat).
count = 0;
real = node;		// In case of success, this will be the value.
						// If fail, value is ignored.
Node<Pn> *test;	// Node to be tested.
test = node;
//Pn *pn;
//pn = node->getData();			// 10/06/99 AM.
bool match;
// TODO: Get rule elt's match mode out here.
//bool singlet;
//singlet = ielt->getSinglet();

for (;;)				// While matching.
	{
	// TODO: Match according to match mode.
	// Check for special rule elements here.
	// (Check for special key=val pairs relating to match mode.)
	// (For efficiency, match mode should be built into each elt.)
	// switch (mode)
	//		{
	//		case literalMODE:	match = literalMatch....
	//		....
	//		}
	// NOTE:	May want the switch stmt and modes available to app developer.
	// At least for adding new match types.
	match = Match(ielt, test);

	if (!match)				// Failed to match.
		{
		// We FAIL if count == 0 because that means there was no "real"
		// match.  Callers will check that this was an optional element
		// and succeed anyway.  11/23/98 AM.
		if (!count || count < min)		// 11/23/98 AM. Bug fix.
			return false;
		return true;		// Matched within min-max range.
		}

	// Matched.
	++count;
	node = test;
	// if (max == 0) keep matching	// Indefinite repetition.
	if (count == max)			// Maxed out.
		return true;
	// Move left one node.
	if ((test = nodeLeft(node, start)))
		;
	else
		{
		if (count < min)
			return false;
		}
	}
return false;
}


/********************************************
* FN:		TRUEMATCHRIGHT
* CR:		11/03/98 AM.
* SUBJ:	Truly match node to rule element, with repetitions,features,etc.
* RET:	True if succeeded, else failed.
********************************************/

bool Pat::trueMatchRight(
	Ielt *ielt,					// OPT // 10/06/99 AM.
	Node<Pn> *end,
	Nlppp *nlppp,				// 04/27/06 AM.
	/*DU*/
	Node<Pn>* &node,			// DN: Current node.  UP: Last real matched node.
	Node<Pn>* &real,			// First real matched node, moving right.
	int &count					// Count the number of matches.	// 02/04/05 AM.
	)
{
// Get repetitions, conditions, features....
//Ielt *ielt;
int min, max;

if (!ielt || !node)
	return false;
//ielt = elt->getData();
ielt->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
if (min < 0 || max < 0)
	return false;
//int count;			// Count the number of matches.	// 02/04/05 AM.

// (max == 0 means indefinite repeat).
count = 0;
real = node;		// In case of success, this will be the value.
						// If fail, value is ignored.
Node<Pn> *test;	// Node to be tested.
test = node;
bool match;
Node<Pn> *saverightmost = 0;				// RECOPT2.	// 07/23/06 AM.

// TODO: Get rule elt's match mode out here.
//bool singlet;
//singlet = ielt->getSinglet();

for (;;)				// While matching.
	{
	// TODO: Match according to match mode.
	// Check for special rule elements here.
	// (Check for special key=val pairs relating to match mode.)
	// (For efficiency, match mode should be built into each elt.)
	// switch (mode)
	//		{
	//		case literalMODE:	match = literalMatch....
	//		....
	//		}
	// NOTE:	May want the switch stmt and modes available to app developer.
	// At least for adding new match types.
	match = Match(ielt, test);

	if (!match)				// Failed to match.
		{
		if (saverightmost)								// RECOPT2.	// 07/23/06 AM.
			{
			nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/23/06 AM.
			nlppp->rmost_ = 0;							// RECOPT2.	// 07/23/06 AM.
			}

		// We FAIL if count == 0 because that means there was no "real"
		// match.  Callers will check that this was an optional element
		// and succeed anyway.  11/23/98 AM.
		if (!count || count < min)		// 11/23/98 AM. Bug fix.
			{
			++count;	// For recursive opt, this node was "tried"!	// 02/04/05 AM.
			return false;
			}
//		if (min && count < min)			// 11/21/98 AM. Bug fix.
//		if (!min || count < min)		// 11/06/98 AM. Bug fix.
//			return false;
//		++count;	// For recursive opt, this node was "tried"!	// 02/04/05 AM.
//		nlppp->span_ += count;	// RECOPT1	// 04/27/06 AM.
		return true;		// Matched within min-max range.
		}

	// Matched.
//	updateRestart(nlppp,node);	// FIXREC1306	// 06/02/13 AM.

	// Need this for tracking in RECOPT2.						// 07/23/06 AM.
	if (!nlppp->first_)												// 07/23/06 AM.
		nlppp->first_ = nlppp->last_ = node;					// 07/23/06 AM.

	++count;
	node = test;
	// if (max == 0) keep matching	// Indefinite repetition.
	if (count == max)			// Maxed out.
		{
//		nlppp->span_ += count;	// RECOPT1	// 04/27/06 AM.
		return true;
		}
	// Move right one node.
	if ((test = nodeRight(node, end)))
		{
		updateRestart(nlppp,test);	// RECOPT2	// 07/17/06 AM.
		// Need to back up if failed repeat match.// RECOPT2.	// 07/23/06 AM.
		if (nlppp->rmost_)	// Curr rule.			// RECOPT2.	// 07/23/06 AM.
			saverightmost = nlppp->rmost_;			// RECOPT2.	// 07/23/06 AM.
		}
	else
		{
		if (count < min)
			return false;
		}
	}
return false;
}


/********************************************
* FN:		MATCHRIGHTSPECIAL
* CR:		12/19/99 AM.
* SUBJ:	Match special elements at end of rule.
* RET:	True if successful, else false.
* NOTE:	To handle things like
*				_comment <- \# _xWILD _xEOF @@
*			07/31/02 AM. It's about time I implemented this.
********************************************/

bool Pat::matchRightspecial(
	_TCHAR *ename,			// Elt name.
	Nlppp *nlppp,
	/*DU*/
	Delt<Ielt>* &elt,		// Rule elt to continue matching against.
	enum Dir  &dir,		// Direction to continue matching in.
	Node<Pn>* &wildone,	// First unfilled wildcard.
	int &wildmin,			// Min nodes that wildcards must cover.
	int &wildmax			// Total possible nodes that wildcards can cover.
	)
{
Node<Pn> *node = nlppp->node_;											// 07/31/02 AM.
Node<Pn> *end = nlppp->end_;												// 07/31/02 AM.

if (!wildone)																	// 07/31/02 AM.
	return false;																// 07/31/02 AM.

if (!node)																		// 07/31/02 AM.
	{
	// Todo: Check _xEOF case.
	return true;																// 07/31/02 AM.
	}

// Find the end of the current phrase.  If _xEOF, check if end of file.

long count;			// Count nodes that are traversed.				// 07/31/02 AM.
count = 0;																		// 07/31/02 AM.
Node<Pn> *test;																// 07/31/02 AM.
test = node;																	// 07/31/02 AM.
for (;;)		// Look for real match till done.						// 07/31/02 AM.
	{
	++count;																		// 07/31/02 AM.

	if (wildmax && count > wildmax)										// 07/31/02 AM.
		return false;	// Exceeded total coverage of wildcards.	// 07/31/02 AM.
	if (test == end)	// Can't move right.								// 07/31/02 AM.
		break;																	// 07/31/02 AM.
	// Didn't exceed wildcard coverage, or there's unlimited coverage.
	if (!(test->Right()))				// Try to move right.		// 07/31/02 AM.
		break;																	// 07/31/02 AM.
	test = test->Right();	// Last node so far.						// 07/31/02 AM.
	// Successful move right of one node.
	updateRestart(nlppp,test);	// RECOPT2	// 07/17/06 AM.
	}

// FAIL if wildcards pending and we didn't cover the minimum. (DESIGN)
if (count <= wildmin)														// 07/31/02 AM.
	return false;																// 07/31/02 AM.

nlppp->node_ = test;															// 07/31/02 AM.

//strstream gerrStr(Errbuf,MAXSTR,ios::out);
//gerrStr << "[matchRightspecial: UNIMPLEMENTED]" << std::ends;
//errOut(false);

// COMPLETE THE WILDCARD AT END OF RULE MATCH.						// 07/31/02 AM.

// Build a collect (tracking structure) for current rule elt.
// (ie, that matched _xEND or _xEOF).									// 07/31/02 AM.
Node<Pn> *ncoll = makeCollect(nlppp->collect_,						// 07/31/02 AM.
			elt, 0, 0, RIGHT);									// 07/31/02 AM.

// Backfill wildcards.
if (!wildFillRight(wildone, ncoll, node, 0))
	return false;				// Failed to assign to wildcards.

// 01/17/99 AM. FIX.  Must clear the wildcard tracking.
wildone = 0;
wildmin = wildmax = -1;

// Update the tracking for nodes matched to the current rule.
nlppp->last_ = nlppp->node_;			// Rightmost node matched by rule.
if (!nlppp->first_)
	nlppp->first_ = node;

//nlppp->maxspan_ += count;	// RECOPT1	// 04/27/06 AM.

// Couldn't go right.
dir = DONE;
return true;																	// 07/31/02 AM.
}


/********************************************
* FN:		WILDFILLLEFT
* CR:		10/30/98 AM.
* SUBJ:	Back fill wildcards for the left direction.
* RET:	True if succeeded, else failed.
* NOTE:	Can fail because of wildcards with restrictions or because
*			of limited coverage wildcards.  Also, not exhaustively trying
*			to make wildcards "work", because that is costly and useless.
*			(Good rules won't rely on such stuff.)
********************************************/

bool Pat::wildFillLeft(
	Node<Pn> *s_coll,			// Collect for first pending wildcard.
	Node<Pn> *e_coll,			// One past collect for last pending wildcard.
	Node<Pn> *s_node,			// First unmatched node to assign to wildcards.
	Node<Pn> *e_node			// One past last unmatched node.
	)
{
if (!s_coll || (s_node == e_node))
	return true;				// No wildcards pending, or no nodes to assign.

// Traverse from wildone to current collect, in tandem with save to real.
// Fill in each wildcard to the max, meeting restrictions.

// Opt: If only one wildcard with no restrictions, don't need to traverse
// nodes.
for (; s_coll != e_coll; s_coll = s_coll->Left())
	{
	// Get info about current collect.
	// If it's nonwild (optional) skip it.
	Pn *pn_coll;
	pn_coll = s_coll->getData();
	Delt<Ielt> *rulelt;
	rulelt = pn_coll->getRulelt();
	Ielt *data;
	data = rulelt->getData();
	if (!wildElt(data))			// Not a wildcard. // 10/05/99 AM.
		continue;						// Must be an optional node. Skip it.

	// Get data.  Get min,max.
	int min,max;
	data->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
	if (min < 0)
		min = 0;
	if (max < 0)
		max = 0;

	// If it's unlimited unrestricted wildcard, match all nodes to it.
	// If it's unlimited unrestricted wildcard, match all nodes to it.
	// Or if there's only one wildcard to fill.					// 01/12/00 AM.
	if (!max
		 || (s_coll == e_coll->Right()))								// 01/12/00 AM.
		{
		s_coll->seteDown(s_node);			// Earliest unassigned node.
		s_coll->setDown(e_node->Right());// Latest unassigned node.
		return true;														// 01/12/00 AM.
		}
	// 01/12/00 AM. The case of an explicit wildcard range was never handled.
	// break;																// 01/12/00 AM.

	// Else, traverse nodes trying to meet restrictions and max limit.
	// If success, fill collect with nodes.
	// Else, failed to match rule.
	for (s_node; s_node != e_node; s_node = s_node->Left())
		{
		// TODO.
		// If not wild, skip it.
		// If unrestricted unlimited, fill it and done.
		// Else handle limits.
		// Handle restrictions.
		}
	}

// Made it this far, successfully backfilled nodes to wildcards.
return true;
}


/********************************************
* FN:		WILDFILLRIGHT
* CR:		11/03/98 AM.
* SUBJ:	Back fill wildcards for the right direction.
* RET:	True if succeeded, else failed.
* NOTE:	Can fail because of wildcards with restrictions or because
*			of limited coverage wildcards.  Also, not exhaustively trying
*			to make wildcards "work", because that is costly and useless.
*			(Good rules won't rely on such stuff.)
********************************************/

bool Pat::wildFillRight(
	Node<Pn> *s_coll,			// Collect for first pending wildcard.
	Node<Pn> *e_coll,			// One past collect for last pending wildcard.
	Node<Pn> *s_node,			// First unmatched node to assign to wildcards.
	Node<Pn> *e_node			// One past last unmatched node.
	)
{
if (!s_coll || (s_node == e_node))
	return true;				// No wildcards pending, or no nodes to assign.

// Traverse from wildone to current collect, in tandem with save to real.
// Fill in each wildcard to the max, meeting restrictions.

// Opt: If only one wildcard with no restrictions, don't need to traverse
// nodes.
for (; s_coll != e_coll; s_coll = s_coll->Right())
	{
	// Get info about current collect.
	// If it's nonwild (optional) skip it.
	Pn *pn_coll;
	pn_coll = s_coll->getData();
	Delt<Ielt> *rulelt;
	rulelt = pn_coll->getRulelt();
	Ielt *data;
	data = rulelt->getData();
	if (!wildElt(data))			// Not a wildcard.	// 10/05/99 AM.
		continue;						// Must be an optional node. Skip it.

	// Get data.  Get min,max.
	int min,max;
	data->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
	if (min < 0)
		min = 0;
	if (max < 0)
		max = 0;

	// If it's unlimited unrestricted wildcard, match all nodes to it.
	// Or if there's only one wildcard to fill.					// 01/12/00 AM.
	if (!max
		 || (s_coll == e_coll->Left()))								// 01/12/00 AM.
		{
		// 11/05/98 AM. Bug fix.  These were not done right.
		s_coll->setDown(s_node);			// Earliest unassigned node.
		if (e_node)												// FIX	// 07/31/02 AM.
			s_coll->seteDown(e_node->Left());// Latest unassigned node.
		else
			s_coll->seteDown(s_node->last());						// 07/31/02 AM.
			// Else, get last node.										// 07/31/02 AM.
		return true;														// 01/12/00 AM.
		}
	// 01/12/00 AM. The case of an explicit wildcard range was never handled.
	//break;		// ASSUME unlimited unrestricted wildcard for now.

	// Else, traverse nodes trying to meet restrictions and max limit.
	// If success, fill collect with nodes.
	// Else, failed to match rule.
	for (s_node; s_node != e_node; s_node = s_node->Right())
		{
		// TODO.
		// If not wild, skip it.
		// If unrestricted unlimited, fill it and done.
		// Else handle limits.
		// Handle restrictions.
		}
	}

// Made it this far, successfully backfilled nodes to wildcards.
return true;
}


/********************************************
* FN:		MATCH
* CR:		11/03/98 AM.
* SUBJ:	Match rule element to parse tree node.
* RET:	True if successful match, else false.
********************************************/

bool Pat::Match(
	Ielt *ielt,				// OPT	// 10/06/99 AM.
	Node<Pn> *node
	)
{
if (!ielt || !node)
	return false;

Pn *pn;

pn   = node->getData();

//bool singlet;							// 03/24/99 AM.
//singlet = ielt->getSinglet();		// 03/24/99 AM.

	// TODO: Match according to match mode.
	// Check for special rule elements here.
	// (Check for special key=val pairs relating to match mode.)
	// (For efficiency, match mode should be built into each elt.)
	// switch (mode)
	//		{
	//		case literalMODE:	match = literalMatch....
	//		....
	//		}
	// NOTE:	May want the switch stmt and modes available to app developer.
	// At least for adding new match types.
// 01/31/99 AM. Handling preactions separately now.
bool failmode = false, success;								// 10/18/99 AM.
if (ielt->getSinglet()  || ielt->getXsinglet())		// 03/24/99 AM.
	{
	success = singletMatch(ielt, node, /*UP*/ failmode);	// 10/18/99 AM.
	if (success == failmode)										// 10/18/99 AM.
		return success;												// 10/18/99 AM.

	if (!singletExcept(ielt, node, failmode))					// 10/18/99 AM.
		return false;													// 10/18/99 AM.
	}
else if (ielt->getTree() || ielt->getXtree())		// 03/24/99 AM.
	{
	success = treeMatch(ielt, node, true, /*UP*/ failmode);	// 10/18/99 AM.
	if (success == failmode)										// 10/18/99 AM.
		return success;												// 10/18/99 AM.

	Dlist<Iarg> *elist;												// 11/09/99 AM.
	Htab *htab = ielt->getHexcepts();							// 11/09/99 AM.
	if (!(elist = ielt->getExcepts())							// 10/18/99 AM.
	    && !htab )														// 11/09/99 AM.
		return !failmode;			// Nothing has changed.		// 10/18/99 AM.
	if (!treeExcept(elist, htab, node, true,					// 11/09/99 AM.
					ielt->getDeaccent(),								// 01/28/05 AM.
					ielt->getDejunk()									// 09/09/11 AM.
					))
		return !failmode;												// 10/18/99 AM.
	else																	// 10/18/99 AM.
		return failmode;												// 10/18/99 AM.
	}
else
	{
	// Still a bug with fail + except!							// 06/08/03 AM.
//	if (!modeMatch(ielt, pn, failmode))	// OPT				// 10/05/99 AM.
//		return false;													// 01/31/99 AM.
	success = modeMatch(ielt,pn,failmode);						// 06/08/03 AM.
	if (!success && !failmode)										// 06/08/03 AM.
		return false;													// 06/08/03 AM.
	if (failmode != success)										// 06/08/03 AM.
		{
		Dlist<Iarg> *elist;											// 11/09/99 AM.
		Htab *htab = ielt->getHexcepts();						// 11/09/99 AM.
		if ((elist = ielt->getExcepts())		// FIX			// 12/15/99 AM.
			 || htab )								// FIX			// 12/15/99 AM.
			{
			bool succ = modeFails(elist,htab,					// 11/09/99 AM.
								 pn->getName(), pn->getType(),	// 10/18/99 AM.
								 ielt->getDeaccent(),				// 01/28/05 AM.
								 ielt->getDejunk()					// 09/09/11 AM.
								 );
//			if (!succ)								// FIX.			// 12/15/99 AM.
//				return false;						// FIX.			// 12/15/99 AM.
			if (failmode == succ)									// 06/08/03 AM.
				return false;											// 06/08/03 AM.
			success = true;											// 06/09/03 AM.
			}
		}
	if (!success)	// Failmode with except not matched.	// 06/09/03 AM.
		return false;													// 06/09/03 AM.
	// Succeeded so far. Check pre actions next...			// 12/15/99 AM.
	}
// For now, applying pre-actions only to the leaf node.	// 01/31/99 AM.
return preMatch(ielt, node);	// 10/05/99 AM.				// 06/16/05 AM.
}


/********************************************
* FN:		MODEMATCH
* CR:		11/03/98 AM.
* SUBJ:	Match rule element to parse tree node, according to match mode.
* RET:	True if successful match, else false.
* NOTE:	3/23/99 AM. Adding a KEEPON flag for FAILS list.  If successful,
*			that means we didn't find the thing so need to continue until
*			all singlets (in singlet) mode or all subnodes (in tree mode)
*			have been tried!
********************************************/

bool Pat::modeMatch(
	Ielt *ielt,				// OPT	// 10/05/99 AM.
	Pn *pn,					// OPT	// 10/05/99 AM.
	/*UP*/
	bool &failmode						// 03/23/99 AM.
	)
{
failmode = false;						// 03/23/99 AM.
if (!ielt || !pn)						// 10/05/99 AM.
	return false;
_TCHAR *name;

// Separating out pre-actions.  // 01/31/99 AM.
// Apply pre-actions here.	// 11/30/98 AM.
//if (!preMatch(ielt->getPres(), pn))			// 01/31/99 AM.
//	return false;										// 01/31/99 AM.

// 10/14/99 AM. Need to handle the EXCEPTS list EVERYWHERE IN HERE.

// OPT. // TESTING FOR LITERAL MATCH FIRST.	// 10/12/99 AM.
if (!(name = ielt->getName()))					// 10/12/99 AM.
	return false;
if (*name != '_'										// 10/12/99 AM.
    || *(name+1) != 'x'
	 || !is_upper((_TUCHAR)*(name+2)) )							// 12/16/01 AM.
	{
	if (!ielt->getDeaccent()												// 01/28/05 AM.
	 && !ielt->getDejunk())													// 09/09/11 AM.
		return literalMatch(name, pn->getName());						// 10/12/99 AM.
	else if (ielt->getDejunk())											// 09/09/11 AM.
		return dejunkMatch(name,pn->getName());						// 09/09/11 AM.
	return deaccentMatch(name,pn->getName());							// 01/28/05 AM.
	}

if (!_tcscmp(name, _T("_xALPHA")))	// Alpha token.
	return ((pn->getType() == PNALPHA) ? true : false);			// 07/20/04 AM.
	// To handle retokenized alphabetics.								// 07/20/04 AM.
	// return alphabetic(*pn->getName());									// 07/20/04 AM.
else if (!_tcscmp(name, _T("_xNUM")))						// Numeric token.
	return ((pn->getType() == PNNUM) ? true : false);
else if (!_tcscmp(name, _T("_xWHITE")))						// White token.
	return ((pn->getType() == PNWHITE) ? true : false);
else if (!_tcscmp(name, _T("_xBLANK")))	// Non-newline whitespace.	// 03/22/99 AM.
	{
	_TCHAR ch;
	if ((ch = *(pn->getName())) == ' ' || ch == '\t')
		return true;
	return false;
	}
else if (!_tcscmp(name, _T("_xPUNCT")))			// Punctuation token.// 12/04/98 AM.
	return ((pn->getType() == PNPUNCT) ? true : false);
else if (!_tcscmp(name, _T("_xEMOJI")))
	return ((pn->getType() == PNEMOJI) ? true : false);
else if (!_tcscmp(name, _T("_xANY")))	// Match any node.	// 12/08/98 AM.
	return true;	// Always matches.
else if (!_tcscmp(name, _T("_xCAP")))		// Match capitalized word. // 01/20/99 AM.
	return ((pn->getType() == PNALPHA)									// 07/20/04 AM.
	// return (alphabetic(*pn->getName())									// 07/20/04 AM.
				&& is_upper((_TUCHAR)*(pn->getName())) );		// 12/16/01 AM.
else if (!_tcscmp(name, _T("_xCAPLET")))	// Match cap letter.	// 07/10/09 AM.
	{
	_TCHAR *x = pn->getName();											// 07/10/09 AM.
	return (*x && !(*(x+1)) && alphabetic(*x)						// 07/10/09 AM.
		&& is_upper((_TUCHAR)*x));										// 07/10/09 AM.
	}
else if (!_tcscmp(name, _T("_xLET")))	// Match letter.			// 07/10/09 AM.
	{
	_TCHAR *x = pn->getName();											// 07/10/09 AM.
	return (*x && !(*(x+1)) && alphabetic(*x));					// 07/10/09 AM.
	}
else if (!_tcscmp(name, _T("_xCTRL")))			// Control char tok.	// 07/24/00 AM.
	return ((pn->getType() == PNCTRL) ? true : false);
else if (!_tcscmp(name, _T("_xWILD")))						// Restricted wildcard.
	{
//	if (Debug())
//		*gout << "  [Restricted wildcard.]" << std::endl;
	// 11/09/98 AM.
	// Must be restricted wildcard, or we wouldn't be here.
	// Get the match and fail lists.  Compare each.
	// Assuming that there aren't both match and fail lists.
	// 12/04/98 AM. Do a mode match on each element of the matches or fails
	// list!  Could check for _xWILD in the list, but that's benign so ignore
	// it.
	Dlist<Iarg> *list;
	Delt<Iarg> *delt;
	_TCHAR *nname;							// Node name.
	enum Pntype typ;						// Node type.
	typ = pn->getType();					// 12/04/98 AM.
	nname = pn->getName();
	Htab *htab = 0;						// 12/12/98 AM.
	bool deaccent = ielt->getDeaccent();					// 01/28/05 AM.
	bool dejunk = ielt->getDejunk();							// 09/09/11 AM.
	if ((list = ielt->getMatches()))	// 11/08/99 AM.
		{
		for (delt = list->getFirst(); delt; delt = delt->Right())
			{
			if (modeMatch1(nname, typ,
									delt->getData()->getStr(),	// 12/04/98 AM
									deaccent,						// 01/28/05 AM.
									dejunk							// 09/09/11 AM.
									))
				return true;
			}
		}
	if ((htab = ielt->getHmatches()))		// 11/08/99 AM.
		return modeHash(nname, htab,		// Check out the hash table.
						ielt->getDeaccent(),					// 03/27/05 AM.
						ielt->getDejunk());					// 09/09/11 AM.
	if (list)
		return false;			// Match mode failed.	// 11/08/99 AM.
	failmode = true;	// 03/23/99 AM.
	if ((list = ielt->getFails()))			// 11/08/99 AM.
		{
		for (delt = list->getFirst(); delt; delt = delt->Right())
			{
			if (modeMatch1(nname, typ,
								delt->getData()->getStr(),	// 12/04/98 AM
								deaccent,						// 01/28/05 AM.
								dejunk							// 09/09/11 AM.
								))
				return false;
			}
		}
	if ((htab = ielt->getHfails()))			// 11/08/99 AM.
		// If matched nothing on fails list, succeed.
		return !modeHash(nname, htab,			// Check out the hash table.
								ielt->getDeaccent(),		// 03/27/05 AM.
								ielt->getDejunk());		// 09/09/11 AM.
	if (list)
		return true;		// Failed in fail mode.		// 11/08/99 AM.

	// WILDCARD WITH @PRE IS NOT WILDCARD	// 08/16/11 AM.
	return true;	// Automatically matches this part.	// 08/16/11 AM.

	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[modeMatch: Wildcard with no match or fail list.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return false;
	}
//else			// LITERAL MATCH.		// OPT		// 10/12/99 AM.
//	return literalMatch(name, pn->getName());		// 10/12/99 AM.
return false;
}


/********************************************
* FN:		MODEMATCH1
* CR:		12/04/98 AM.
* SUBJ:	Match node to one element of a match/fail list, according to mode.
* RET:	True if successful match, else false.
********************************************/

bool Pat::modeMatch1(
	_TCHAR *nname,				// Node name.
	enum Pntype ntype,		// Node type.
	_TCHAR *ename,					// One of rule element names from a match/fail list.
	bool deaccent,				// 01/28/05 AM.
	bool dejunk					// 09/09/11 AM.
	)
{
if (!nname || !ename)
	return false;

if (!_tcscmp(ename, _T("_xALPHA")))	// Alpha token.
	return (ntype == PNALPHA) ? true : false;							// 07/20/04 AM.
	// return (alphabetic(*nname));											// 07/20/04 AM.
else if (!_tcscmp(ename, _T("_xNUM")))	// Numeric token.
	return (ntype == PNNUM) ? true : false;
else if (!_tcscmp(ename, _T("_xWHITE")))	// White token.
	return (ntype == PNWHITE) ? true : false;
else if (!_tcscmp(ename, _T("_xBLANK")))	// Non-newline whitespace.	// 03/22/99 AM.
	{
	_TCHAR ch;
	if ((ch = *nname == ' ') || ch == '\t')
		return true;
	return false;
	}
else if (!_tcscmp(ename, _T("_xPUNCT")))	// Punctuation token.
	return (ntype == PNPUNCT) ? true : false;
else if (!_tcscmp(ename, _T("_xEMOJI")))
	return (ntype == PNEMOJI) ? true : false;
else if (!_tcscmp(ename, _T("_xANY")))				// 12/08/98 AM.
	return true;					// User is being silly, but do it anyway.
else if (!_tcscmp(ename, _T("_xCAP")))	// Match capitalized word. // 01/20/99 AM.
	return ((ntype == PNALPHA)												// 07/20/04 AM.
	// return (alphabetic(*nname)												// 07/20/04 AM.
			  && is_upper((_TUCHAR)*nname) );					// 12/16/10 AM.
else if (!_tcscmp(ename, _T("_xCAPLET")))	// Match cap letter.	// 07/10/09 AM.
	return (*nname && !(*(nname+1)) && isupper((_TUCHAR)*nname)	// 07/10/09 AM.
		&& alphabetic(*nname));												// 07/10/09 AM.
else if (!_tcscmp(ename, _T("_xLET")))	// Match letter.			// 07/10/09 AM.
	return (alphabetic(*nname) && !(*(nname+1)));					// 07/10/09 AM.
else if (!_tcscmp(ename, _T("_xCTRL")))	// Control char token.
	return (ntype == PNCTRL) ? true : false;
else			// LITERAL MATCH.
	{
	if (!deaccent && !dejunk)												// 01/28/05 AM.
		return literalMatch(ename, nname);
	else if (dejunk)															// 09/09/11 AM.
		return dejunkMatch(ename,nname);									// 09/09/11 AM.
	else
		return deaccentMatch(ename,nname);								// 01/28/05 AM.
	}
}


/********************************************
* FN:		MODEHASH
* CR:		12/12/98 AM.
* SUBJ:	See if node hashes to anything on match or fail list.
* RET:	True if successful match, else false.
********************************************/

bool Pat::modeHash(
	_TCHAR *nname,				// Node name.
	Htab *htab,					// List's hash table.
	bool deaccent,				// If deaccented match.
	bool dejunk					// If dejunk match.
	)
{
if (!htab)						// 11/09/99 AM.
	return false;				// 11/09/99 AM.
if (dejunk)																		// 09/09/11 AM.
	{
	long len = _tcsclen(nname);											// 09/09/11 AM.
	_TCHAR *dbuf = Chars::create(len + 2);								// 09/09/11 AM.
	Xml::de_junk(nname,/*DU*/dbuf);										// 09/09/11 AM.
	bool mflag = htab->hfind_lc(dbuf) ? true : false;				// 09/09/11 AM.
	Chars::destroy(dbuf);													// 09/09/11 AM.
	return mflag;																// 09/09/11 AM.
	}
else
if (deaccent)																	// 03/27/05 AM.
	{
	long len = _tcsclen(nname);											// 03/27/05 AM.
	_TCHAR *dbuf = Chars::create(len + 2);								// 03/27/05 AM.
	Xml::de_accent(nname,/*DU*/dbuf);									// 03/27/05 AM.
	bool mflag = htab->hfind_lc(dbuf) ? true : false;				// 03/27/05 AM.
	Chars::destroy(dbuf);													// 03/27/05 AM.
	return mflag;																// 03/27/05 AM.
	}
if (htab->hfind_lc(nname))
	return true;
return false;
}


/********************************************
* FN:		MODEMATCHES
* CR:		10/16/99 AM.
* SUBJ:	Treat EXCEPTS list as matches.
* RET:	True if matched something on list.
********************************************/

bool Pat::modeMatches(
	Dlist<Iarg> *list,					// EXCEPTS list.
	Htab *htab,								// EXCEPTS hash table.	// 11/09/99 AM.
	_TCHAR *nname,							// Node name.
	enum Pntype typ,						// Node type.
	bool deaccent,															// 01/28/05 AM.
	bool dejunk																// 09/09/11 AM.
	)
{
Delt<Iarg> *delt;
if (htab && modeHash(nname, htab,				// 11/09/99 AM.
								deaccent,									// 03/27/05 AM.
								dejunk))										// 09/09/11 AM.
	return true;										// 11/09/99 AM.
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	if (modeMatch1(nname, typ, delt->getData()->getStr(), deaccent,dejunk))
		return true;
	}
return false;
}


/********************************************
* FN:		MODEFAILS
* CR:		10/16/99 AM.
* SUBJ:	Treat EXCEPTS list as fails.
* RET:	True if didn't match anything on list.
********************************************/

bool Pat::modeFails(
	Dlist<Iarg> *list,					// EXCEPTS list.
	Htab *htab,								// EXCEPTS hash table.	// 11/09/99 AM.
	_TCHAR *nname,							// Node name.
	enum Pntype typ,						// Node type.
	bool deaccent,															// 01/28/05 AM.
	bool dejunk																// 09/09/11 AM.
	)
{
Delt<Iarg> *delt;
if (htab && modeHash(nname, htab,				// 11/09/99 AM.
								deaccent,									// 03/27/05 AM.
								dejunk))										// 09/09/11 AM.
	return false;										// 11/09/99 AM.
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	if (modeMatch1(nname, typ, delt->getData()->getStr(),
			deaccent,														// 01/28/05 AM.
			dejunk															// 09/09/11 AM.
			))
		return false;
	}
return true;
}


/********************************************
* FN:		DEACCENTMATCH
* CR:		01/28/05 AM.
* SUBJ:	Match rule element text to deaccented parse tree node text.
* RET:	True if successful match, else false.
********************************************/

inline
bool Pat::deaccentMatch(
	_TCHAR *ename,					// Rule element name.
	_TCHAR *nname						// Node name
	)
{
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);
bool mflag = literalMatch(ename, dbuf);
Chars::destroy(dbuf);
return mflag;
}



/********************************************
* FN:		DEJUNKMATCH
* CR:		09/09/11 AM.
* SUBJ:	Match rule element text to dejunk parse tree node text.
* RET:	True if successful match, else false.
********************************************/

inline
bool Pat::dejunkMatch(
	_TCHAR *ename,					// Rule element name.
	_TCHAR *nname						// Node name
	)
{
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_junk(nname,/*DU*/dbuf);
bool mflag = literalMatch(ename, dbuf);
Chars::destroy(dbuf);
return mflag;
}


/********************************************
* FN:		LITERALMATCH
* CR:		10/27/98 AM.
* SUBJ:	Match rule element text to parse tree node text.
* RET:	True if successful match, else false.
* NOTE:	Text of parse tree node is gotten by its offset into the input
*			text buffer.  Points to start of text, but offsets are in the
*			full buffer.
* OPT:	Should get input text words into a hash table and point to them
*			there.  Then things like this match would be simplified.
********************************************/

inline
bool Pat::literalMatch(
	_TCHAR *ename,					// Rule element name.
	_TCHAR *nname						// Node name
	)
{
//Ielt *ee;

// Get rule elt's name.
//ee = delt->getData();
//if (Debug())			// OPT // 10/05/99 AM.
//	{
//	*gout << "   rule elt name=" << ename << std::endl;
//	*gout << "   node name=" << nname << std::endl;
//	}

// MATCH THEM!
//if (eq_str_range(ee->getName(),
//					  pn->getText(), 0, pn->getEnd() - pn->getStart() ))

// 12/01/98 AM. Making literal match case-insensitive.
return (strcmp_i(ename, nname) ? false : true);
}


/********************************************
* FN:		SINGLETMATCH
* CR:		11/03/98 AM.
* SUBJ:	Match rule element to a singlet node in the given subtree.
* RET:	True if successful match, else false.
* NOTE:	Per-node match will use literal match for now.  That could be
*			made more flexible.  May take a match mode as an argument.
********************************************/

bool Pat::singletMatch(
	Ielt *ielt,				// OPT	// 10/05/99 AM.
	Node<Pn> *node,
	/*UP*/
	bool &failmode						// 10/18/99 AM.
	)
{
failmode = false;						// 10/18/99 AM.
bool success = false;				// 03/23/99 AM.
Pn *pn;									// 10/05/99 AM.
while (node)		// Look down the subtree.
	{
//	if (Debug())
//		*gout << "   [IN SINGLET MATCH:]" << std::endl;
	pn = node->getData();										// 10/05/99 AM.
	success = modeMatch(ielt, pn, /*UP*/ failmode);		// 10/05/99 AM.
	if (success && !failmode)									// 03/23/99 AM.
		return true;
	if (!success && failmode)	// Matched something, durnit. // 03/23/99 AM.
		return false;				// 03/23/99 AM.

	// If we're at a base or bottom, then failed (or succeeded in fail-mode)
	if (pn->getBase())					// 10/05/99 AM.
		return success;					// 03/23/99 AM.

	// Keep going down.
	node = node->Down();

	// FIX. If branching, fail.		// 09/20/99 AM.
	if (node && node->Right())			// 09/20/99 AM.
		return success;					// 09/20/99 AM.
	}
// We've hit rock bottom.
return success;							// 03/23/99 AM.
}


/********************************************
* FN:		SINGLETEXCEPT
* CR:		10/18/99 AM.
* SUBJ:	Handle exception list in singlet mode.
* RET:	True if success, else false.
* NOTE:	Success is not matching in positive mode, matching in neg mode.
* OPT:	Some redundant work with singletMatch.  Should be able to
*			use fact that this is an except list.
********************************************/

bool Pat::singletExcept(
	Ielt *ielt,
	Node<Pn> *node,
	bool failmode
	)
{

if (!ielt || !node)
	return false;

Dlist<Iarg> *elist = ielt->getExcepts();			// 11/09/99 AM.
Htab *htab = ielt->getHexcepts();					// 11/09/99 AM.
if (!elist && !htab)										// 11/09/99 AM.
	return !failmode;			// Nothing has changed.

Pn *pn;
bool deaccent = ielt->getDeaccent();				// 01/28/05 AM.
bool dejunk = ielt->getDejunk();						// 09/11/11 AM.
while (node)		// Look down the subtree.
	{
//	if (Debug())
//		*gout << "   [IN SINGLET EXCEPT:]" << std::endl;
	pn = node->getData();
	if (modeMatches(elist,					// Successful match of list.
						 htab,								// 11/09/99 AM.
						 pn->getName(),
						 pn->getType(),
						 deaccent,							// 01/28/05 AM.
						 dejunk								// 09/09/11 AM.
						 ))
		return failmode;						// Fail/succeed appropriately.

	// We're at a base or bottom.
	if (pn->getBase())
		return !failmode;

	// Keep going down.
	node = node->Down();

	// If branching, done.
	if (node && node->Right())
		return !failmode;
	}
// We've hit rock bottom.
return !failmode;
}


/********************************************
* FN:		TREEMATCH
* CR:		03/17/99 AM.
* SUBJ:	Match rule element to ANY node in the given subtree.
* RET:	True if successful match, else false.
* NOTE:	RECURSIVE.
********************************************/

bool Pat::treeMatch(
	Ielt *ielt,			// OPT	// 10/05/99 AM.
	Node<Pn> *node,
	bool root,			// If this is the top-level node of subtree.
	/*UP*/
	bool &failmode					// 10/18/99 AM.
	)
{
// TRAVERSE THE ENTIRE SUBTREE.
bool success = false;		// 03/23/99 AM.
//bool failmode = false;		// 10/18/99 AM.

if (node == 0)
	return failmode;				// 10/18/99 AM.

success = modeMatch(ielt, node->getData(), /*UP*/ failmode); // 10/05/99 AM.
if (success != failmode)								// 10/18/99 AM.
	return success;										// 10/18/99 AM.

success = treeMatch(ielt, node->pDown, false, failmode);		// RECURSIVE.
if (success != failmode)								// 10/18/99 AM.
	return success;										// 10/18/99 AM.

// Minimal recursion in this part (optimization and resource saving!).
if (!root && !(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		success = treeMatch(ielt, node, false, failmode);	// 10/18/99 AM.
		if (success != failmode)			// 10/18/99 AM.
			return success;					// 10/18/99 AM.
		}
	}
//return false;		// BUG					// 10/18/99 AM.
return failmode;		// FIX					// 10/18/99 AM.
}


/********************************************
* FN:		TREEEXCEPT
* CR:		10/18/99 AM.
* SUBJ:	Match rule element to ANY node in the given subtree.
* RET:	True if matched something on the list.
* NOTE:	RECURSIVE.
********************************************/

bool Pat::treeExcept(
	Dlist<Iarg> *elist,
	Htab *htab,				// Excepts hash table.					// 11/09/99 AM.
	Node<Pn> *node,
	bool deaccent,															// 01/28/05 AM.
	bool dejunk,															// 09/09/11 AM.
	bool root				// If this is the top-level node of subtree.
	)
{
// TRAVERSE THE ENTIRE SUBTREE.
bool success = false;

if (node == 0)
	return false;

Pn *pn = node->getData();
if (modeMatches(elist,htab, pn->getName(), pn->getType(),
			deaccent,														// 01/28/05 AM.
			dejunk															// 09/09/11 AM.
			))
	return true;

if (treeExcept(elist,htab,node->pDown,deaccent,dejunk,false))		// RECURSIVE.
	return true;

// Minimal recursion in this part (optimization and resource saving!).
if (!root && !(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		if (treeExcept(elist,htab,node,deaccent,dejunk,false))
			return true;
		}
	}
return false;
}


/********************************************
* FN:		LEAFMATCH
* CR:		01/31/99 AM.
* SUBJ:	Perform pre-actions on leaf under current node.
* RET:	True if successful match, else false.
* NOTE:	This is an interim solution to the problem that pre actions
*			often apply to a different node than the one that matches the
*			rule element.  So if the element matches the top node, actions
*			like "uppercase" don't necessarily match the top node.  The
*			current function helps address this problem.  It's interim
*			because different pre-actions may apply to different nodes in
*			the chain, while leafMatch will only apply pre-actions to the
*			leaf node in the chain.
*			06/14/05 AM. With var() PRE action, not just looking at leaf.
*			06/16/05 AM. Removing this call. Functionality to preMatch.
********************************************/

# ifdef _OBSOLETE_050616_
bool Pat::leafMatch(
	Ielt *ielt,				// OPT	// 10/05/99 AM.
	Node<Pn> *node
	)
{
if (!node || !ielt)
	return false;
Node<Pn> *leaf;
if (!(leaf = leafNode(node)))
	return false;
Pn *pn;
pn = leaf->getData();
return preMatch(ielt->getPres(), pn);
}
#endif


/********************************************
* FN:		MATCHEOF
* CR:		11/25/98 AM.
* SUBJ:	If we matched the end-of-file rule elt.
* RET:	True if successful match, else false.
* NOTE:	Requiring rule to have matched at least one node.
********************************************/

bool Pat::matchEOF(
	Ielt *ielt,			// Current rule element.	// OPT	// 10/05/99 AM.
	Node<Pn> *last				// Last node matched by rule.
	)
{
if (!last)
	return false;				// Rule must match at least one node.

if (!eltMatches(ielt, _T("_xEOF")))
	{
	// This isn't the end of file rule element, so forget it.
	return false;
	}

if (!Tree<Pn>::rightmostNode(last))
	{
	// If not the rightmost node in the parse tree, we're not at end
	// of file.
	return false;
	}
return true;			// A winner.
}


/********************************************
* FN:		MATCHSTART
* CR:		01/30/99 AM.
* SUBJ:	If we matched the start-of-segment rule elt.
* RET:	True if successful match, else false.
* NOTE:	Shouldn't allow _xSTART to be a trigger.
********************************************/

bool Pat::matchSTART(
	Ielt *ielt,			// Current rule element.	// OPT	// 10/05/99 AM.
	Node<Pn> *last				// Last node matched by rule.
									// (Or current node, if first node.)
	)
{
// Requiring that this be called with either the first node to be
// matched by rule, or by last node matched when going left.
if (!last)
	return false;

if (!eltMatches(ielt, _T("_xSTART")))
	{
	// This isn't the start of segment rule element, so forget it.
	return false;
	}

if (last->Left())
	{
	// If not the leftmost node in the parse tree phrase.
	return false;
	}
return true;			// A winner.
}


/********************************************
* FN:		MATCHEND
* CR:		01/21/99 AM.
* SUBJ:	If we matched the end-of-segment rule elt.
* RET:	True if successful match, else false.
* NOTE:	Requiring rule to have matched at least one node.
********************************************/

bool Pat::matchEND(
	Ielt *ielt,			// Current rule element.	// OPT	// 10/05/99 AM.
	Node<Pn> *last				// Last node matched by rule.
	)
{
if (!last)
	return false;				// Rule must match at least one node.

if (!eltMatches(ielt, _T("_xEND")))
	{
	// This isn't the end of segment rule element, so forget it.
	return false;
	}

if (last->Right())
	{
	// If not the rightmost node in the parse tree phrase, we're not at end
	// of file.
	return false;
	}
return true;			// A winner.
}


/********************************************
* FN:		ELTMATCHES
* CR:		11/25/98 AM.
* SUBJ:	See if given elt matches given text.
* RET:	True if successful match, else false.
* NOTE:	Looking at the elt name and the matches pair.
********************************************/

bool Pat::eltMatches(
	Ielt *ielt,			// Current rule element.	// OPT	// 10/05/99 AM.
	_TCHAR *name
	)
{
if (!ielt || !name)
	return false;

_TCHAR *iname;		// 10/05/99 AM.
if (!strcmp_i((iname = ielt->getName()), name))	// Direct match.
	return true;

if (strcmp_i(iname, _T("_xWILD")))
	return false;				// Only allowing match list on wildcard, for now.

Dlist<Iarg> *list;
if (!(list = ielt->getMatches()))
	return false;				// No alternates to match on.

Delt<Iarg> *darg;
for (darg = list->getFirst(); darg; darg = darg->Right())
	{
	if (!strcmp_i(name, darg->getData()->getStr()))
		return true;
	}
return false;
}


/********************************************
* FN:		SWITCHRIGHT
* CR:		10/31/98 AM.
* SUBJ:	Switch matching to right of trigger.
* ASS:	Matching to left of trigger ended successfully.
* RET:	Success for the end of left side matching.
* NOTE:	Just a useful idiom.
********************************************/

bool Pat::switchRight(		// 11/19/99 AM.
	Delt<Ielt> *trig,
	Nlppp *nlppp,				// 11/19/99 AM.
	/*DU*/
	Delt<Ielt>* &elt,
	enum Dir &dir
	)
{
if ((elt = trig->Right()))
	{
	dir = RIGHT;

	// Try to go forward one node.
	if (nlppp->last_ && (nlppp->last_ != nlppp->end_))
		{
		nlppp->node_ = nlppp->last_->Right();
		updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/17/06 AM.
		}
	else
		nlppp->node_ = 0;
	return true;
	}

// Couldn't go left or right.
dir = DONE;
return true;
}


/********************************************
* FN:		MAKECOLLECT
* CR:		11/03/98 AM.
* SUBJ:	Build tracking data structure for current rule elt.
* ALLOC:	Creates Node.
********************************************/
static Pn pdata;					// 11/17/98 AM.  To reclaim these.

Node<Pn> *Pat::makeCollect(
	Tree<Pn> *collect,			// The current tracking list.
	Delt<Ielt> *rulelt,			// Current rule elt.
	Node<Pn> *left,				// Leftmost node covered by rule elt.
	Node<Pn> *right,				// Rightmost node covered by rule elt.
	enum Dir dir					// Collecting left or right.
	)
{
Node<Pn> *ncoll;					// The collect for current elt.
//data = new Pn();					// Create collect for current rule elt.
pdata.setRulelt(rulelt);		// Collect points to its corresponding rule elt.
if (dir == LEFT)
	ncoll = collect->push(&pdata);	// Push to the LEFT (front) of collect list.
else // if (dir == RIGHT)
	ncoll = collect->rpush(&pdata);	// Push at the RIGHT (end) of collect list.
ncoll->setDown(left);			// Leftmost node covered by collect.
ncoll->seteDown(right);			// Rightmost node covered by collect.
return ncoll;
}


/********************************************
* FN:		WILDELT
* CR:		10/29/98 AM.
* SUBJ:	Check if given rule elt is a wildcard.
* NOTE:	May be other ways in the future, like
*				_xFEAT [mode=wild]
*			11/09/98 AM. Treating wildcards with MATCH and FAIL lists
*			as NON wildcards.
********************************************/

inline bool Pat::wildElt(Ielt *ielt)	// OPT	// 10/05/99 AM.
{
return (ielt && ielt->wild());
}


/********************************************
* FN:		OPTIONALELT
* CR:		10/31/98 AM.
* SUBJ:	Check if given rule elt is optional.
* WARN:	Should not be called with a wildcard rule elt.
********************************************/

inline bool Pat::optionalElt(Ielt *ielt)	// OPT	// 10/05/99 AM.
{
return (ielt
		  && (!ielt->min_)		// Optional if min == 0.
		  );
}


/********************************************
* FN:		WARNWILD
* CR:		10/31/98 AM.
* SUBJ:	Warn about wildcard at start, end, or trigger of rule.
* RET:	False.  Not allowing these constructs for now.
* NOTE:	We intend to allow these, but they are not important
*			enough to deal with till the Pat algorithm is stabilized.
********************************************/

inline bool Pat::warnWild(Irule *rule,Nlppp *nlppp)
{
if (!warn_wild)		// Never issued this warning yet.
	{
	Parse *parse = nlppp->parse_;
	parse->line_ = nlppp->rule_->getLine();
	std::_t_strstream gerrStr;
	gerrStr << _T("[Unimplemented: wildcard trigger, start, or end.]") << std::ends;
	parse->errOut(&gerrStr,false);	// 09/06/08 AM.
//	gerrStr << _T("[Rule=") << *rule << _T("]") << std::ends;
//	errOut(&gerrStr,false);

	warn_wild = true;
	}
return false;			// Skip this rule.
}


/********************************************
* FN:		NODELEFT
* CR:		11/04/98 AM.
* SUBJ:	Move one node to the left if possible.
* RET:	Next node or null.
********************************************/

#ifdef OLD_
//inline									// 05/30/00 AM.
Node<Pn> *Pat::nodeLeft(
	Node<Pn> *node,			// Current node.
	Node<Pn> *start			// Left boundary, inclusive.
	)
{
if (node && node != start)
	return node->Left();
else
	return 0;
}
#endif


/********************************************
* FN:		NODERIGHT
* CR:		11/04/98 AM.
* SUBJ:	Move one node to the right if possible.
* RET:	Next node or null.
* NOTE:	05/30/00 AM. To inline this, put definition into .h file.
********************************************/

#ifdef OLD_
//inline
Node<Pn> *Pat::nodeRight(
	Node<Pn> *node,			// Current node.
	Node<Pn> *end				// Right boundary, inclusive.
	)
{
if (node && node != end)
	return node->Right();
else
	return 0;
}
#endif


/********************************************
* FN:		MATCHCLEANUP
* CR:		11/11/98 AM.
* SUBJ:	Cleanup after a rule match (failed or successful).
* RET:	True if ok, else false.
* ASS:	Matched rules that use the collect must zero it out before
*			this function is called. (eg, double-tier reduction, when
*			implemented)
********************************************/

bool Pat::matchCleanup(Tree<Pn> *collect)
{
Node<Pn> *tmp;
if ((tmp = collect->pRoot))			// OPT								// 05/16/00 AM.
	{
	// DELETE THE LIST OF COLLECT NODES.
	Node<Pn>::deleteNodes(tmp);
	collect->resetTree();		// OPT // 10/06/99 AM.
	//collect->setRoot(0);
	//collect->setLast(0);
	}
return true;
}


/********************************************
* FN:		BASENODE
* CR:		01/24/99 AM.
* SUBJ:	Get the bottommost node below given node.
* RET:	Bottommost node.
********************************************/

Node<Pn> *Pat::baseNode(Node<Pn> *node)
{
Pn *pn = 0;
Node<Pn> *child = 0;
if (!node)
	return 0;
for (;;)				// Traverse down the chain.
	{
	pn = node->getData();

	if (pn->getBase())									// Found bottommost singlet.
		break;
	if (!(child = node->Down()))							// Leaf.
		break;
	if (child->Right())									// Branches out.
		break;
	node = child;
	}
return node;
}


/********************************************
* FN:		BASESTR
* CR:		01/30/99 AM.
* SUBJ:	Given a node, return its base node's string.
********************************************/

_TCHAR *Pat::baseStr(Node<Pn> *node)
{
if (!node)
	return 0;

Node<Pn> *base;
Pn *pn;
base = baseNode(node);	// Get bottommost node.
pn = base->getData();
return pn->getName();
}


/********************************************
* FN:		LEAFNODE
* CR:		01/30/99 AM.
* SUBJ:	Get the leaf node below given node.
* RET:	Leafiest node before branching.
* NOTE:	Sometimes the base stops us from getting to the leaf when we
*			want to.
********************************************/

Node<Pn> *Pat::leafNode(Node<Pn> *node)
{
if (!node)
	return 0;
Node<Pn> *leaf;
leaf = node;
while ((node = node->Down()) && !(node->Right()))
	leaf = node;
return leaf;
}

/********************************************
* FN:		PRETTYPASS
* CR:		06/08/00 AM.
* SUBJ:	Print the parse tree after a pat pass.
* RET:	
********************************************/

void Pat::prettyPass(Node<Pn> *root, Tree<Pn> *tree, Parse *parse)
{
Eana *eana;																		// 10/13/99 AM.
eana = parse->getEana();													// 10/13/99 AM.
//if (parse->Verbose())														// 10/13/99 AM.
if (eana->getFlogfiles())													// 10/13/99 AM.
	{
	bool save;																	// 10/13/99 AM.
	save = eana->getFverbose();											// 10/13/99 AM.
	parse->setVerbose(true);												// 10/13/99 AM.
	eana->setFverbose(true);												// 10/13/99 AM.

	*gout << _T("\nPAT OUTPUT TREE:\n") << std::endl;
	tree->Traverse(root, *gout);
	Pn::TraverseSetFlags(root,false,false);	// CLEAR FLAGS.	// 05/25/01 AM.

	parse->setVerbose(save);												// 10/13/99 AM.
	eana->setFverbose(save);												// 10/13/99 AM.
	}

}

/******************************************************************************/
//				COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		EXECUTE
* CR:		05/13/00 AM.
* SUBJ:	Compiled variant of pat pass.
********************************************/

bool Pat::Execute(
	Parse *parse,							// Current parse.
	const PASS *pass								// Static pass data.	// 06/21/00 AM.
#ifdef OLD_
	bool (code)(Nlppp *),				// Gen'd fn for code region of this pass.
	const _TCHAR **arr_select,			// Array of select node names.
	enum selectType seltype,			// Traversal mode.			// 06/08/00 AM.
	bool (rules)(int, bool&, Nlppp *)		// Gen'd fn for rules of this pass.
#endif
	)
{

if (!parse)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass given no parser or pass information.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}


/////////////////////
// CODE ACTIONS
/////////////////////
// Before any other processing, pass will perform its code actions.
// Should change Pat to be ok with no rules (even empty file should be ok).

Nlppp *nlppp = parse->getNlppp();										// 08/07/02 AM.
nlppp->exitpass_ = false;		// Reset.								// 08/07/02 AM.
nlppp->region_ = REGCODE;	// Execute NLP++ in code region.		// 08/09/08 AM.

if (pass->code)
	(*(pass->code))(nlppp);													// 08/07/02 AM.

nlppp->region_ = REGNULL;	// Reset.									// 08/09/08 AM.

if (nlppp->exitpass_)		// Exiting current pass.				// 08/07/02 AM.
	return true;																// 08/07/02 AM.


/////////////////////
// INITIALIZE
/////////////////////

// Get the parse tree(s).
Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
//Tree<Ln> *lines = parse->getLines();									// 07/18/01 AM.
if (!tree
// || !lines																	// 07/18/01 AM.
	)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass given no parse tree data.]") << std::ends;	// 07/18/01 AM.
	errOut(&gerrStr,false);
	return false;
	}

// Get the parse tree's root node.
Node<Pn> *root;
root = tree->pRoot;
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass: Tree has no root.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// PREPARE FOR HASHING RULES.

// Get the selected nodes to traverse.

///////////////////////
// TRAVERSE AND MATCH
///////////////////////

// Traverse the tree, matching on each rule in turn.
// For each node in the parse tree, see if it is distinguished.

//Node<Pn> *select;
switch (pass->seltype)			// Type of traversal.				// 06/08/00 AM.
	{
	case SELECTROOT:
//		matchNodes(root->Down(),0,rules,parse,0);
		matchNodes(root->Down(),0,parse,pass,0);
		break;
	case SELECTNODES:
//		traverseSelect(arr_select, rules, parse, root);
		traverseSelect(parse,pass,root);
		break;
	case SELECTPATH:
//		pathSelect(arr_select, rules, parse, root);					// 06/08/00 AM.
		pathSelect(pass->arr_select,parse,pass,root);
		break;
	case SELECTMULTI:
//		multiSelect(arr_select, rules, parse, root);					// 06/08/00 AM.
		multiSelect(parse,pass,root);
		break;
	case SELECTPREMULTI:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Premulti select traversal unimplemented.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Bad select traversal type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	}

prettyPass(root,tree,parse);												// 06/08/00 AM.
return true;
}


/********************************************
* FN:		TRAVERSESELECT
* CR:		05/13/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive. Compiled runtime variant.
*			If found a node, not traversing its children.
* RET:	False if aborting pass.	// 11/11/02 AM.
********************************************/

bool Pat::traverseSelect(
#ifdef OLD_
	const _TCHAR *arr_select[],		// Names of select nodes.
	bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
#endif
	Parse *parse,						// Current analysis instance.
	const PASS *pass,						// Static pass data.					// 06/21/00 AM.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect = pass->arr_select;
Pn *pn = node->getData();
_TCHAR *name = pn->getName();
while (*pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
//		matchNodes(node->Down(),0,rules,parse,node);
		if (!matchNodes(node->Down(),0,parse,pass,node))
			return false;
		goto listmanager;
		}
	++pselect;
	}

// Try the rest of the tree.
//if (!flag)		// If current node wasn't select, look within it.
// If node wasn't select and is UNSEALED, then
// look inside it.	// OPT
if (pn->getUnsealed())
//	traverseSelect(arr_select,rules,parse,node->Down());
	{
	if (!traverseSelect(parse,pass,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// Removes part of recursion, reducing stack overflow problems.
listmanager:
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
//		traverseSelect(arr_select,rules,parse,node);
		{
		if (!traverseSelect(parse,pass,node))
			return false;
		}
	}
return true;
}


/********************************************
* FN:		MATCHNODES
* CR:		06/21/00 AM.
* SUBJ:	Match rules in a range of nodes.
* NOTE:	Compiled runtime variant.
*			06/21/00 AM. Keeping the variant with rules funarg, so that recurse
*			regions will continue to work.  Eventually need to hash in the
*			recurse regions also, but not a priority for now.
********************************************/

bool Pat::matchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Parse *parse,										// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *select									// Context node.
	)
{
return matchNodes(start,end,pass->rules,parse,select);
}


/********************************************
* FN:		MATCHNODES
* CR:		05/13/00 AM.
* SUBJ:	Match rules in a range of nodes.
* NOTE:	Compiled runtime variant.
********************************************/

bool Pat::matchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	bool (rules)(int,bool&,Nlppp *),	// Compiled rules.
	Parse *parse,										// Current analysis instance.
//	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *select									// Context node.
	)
{
if (!start || !parse
//	|| !pass || !pass->rules)
	|| !(long)rules)			// .NET COMPLAINT.						// 06/11/03 AM.
	return true;

//Delt<Irule> *prule;			// Current rules; also, the rule that matched.
Node<Pn> *node;				// SAVE CURRENT NODE.
node = start;

// Using this object to collect the entire parse state.
// Reset the nlppp object.
Nlppp *nlppp = parse->nlppp_;
//nlppp->rule_ = 0;				// IGNORE.
//nlppp->collect_ = 0;			// IGNORE. (Using a runtime version.)
//nlppp->parse_ = parse;		// ALREADY SET BY init_analyzer.
nlppp->select_	= select;
nlppp->start_	= start;
nlppp->end_		= end;
//nlppp->coll_	= parse->collect_;	// Runtime variant of collect.
nlppp->node_	= node;					// Start at first node of phrase.
nlppp->excised_ = false;													// 04/28/01 AM.
nlppp->noop_ = false;														// 08/12/02 AM.
//resetRule(node, nlppp);				// Reset state of pattern matcher.
		// This is done before every rule.			// 05/18/00 AM.


// NOTE: Leaving resetting of the collect elts up to the pattern matcher.

while (node)					// While matching on list of nodes.
	{
	// For each rule in the rules list.
	//prule = rules;				// Reset the rules list.
	//if (Debug())
	//	*gout << " Matching on node:" << *node << std::endl;

	// if (!matchRules(node,start,end,parse,select,
	//					/*DU*/ prule,collect,first,last,sem, dsem))
//	if (!matchRules(nlppp, (pass->rules)))
	if (!matchRules(nlppp, (rules)))
		{
		node = nlppp->node_ = nodeRight(node,end);
		// Assume start, end unchanged.
		}
	else
		{
		// Successful rule match.  Execute POST actions.
		//nlppp.rule_ = prule->getData();
		//execActions(prule->getData(),collect,first,last,parse,sem,dsem,select,
		//													/*DU*/ node, start, end);
		//execActions(&nlppp);

		nlppp->node_ = nlppp->after_;		// Set up for next rule match.// ALGOSTATE // 06/06/13 AM.
		node  = nlppp->node_;
		start = nlppp->start_;
		end   = nlppp->end_;
		nlppp->sem_ = 0;		// Zero out.
		nlppp->dsem_ = 0;		// Zero out.
		nlppp->excised_ = false;											// 04/28/01 AM.
		nlppp->noop_ = false;												// 08/12/02 AM.
		}

	// If actions haven't cleaned out the collect, then do it here.
	// matchCleanup(collect);

	if (nlppp->exitpass_)													// 11/18/02 AM.
		return false;															// 11/18/02 AM.
	}
//delete (Tree<Pn> *) nlppp->collect_;
return true;
}


/********************************************
* FN:		MATCHRULES
* CR:		05/13/00 AM.
* SUBJ:	Match rules to current node in parse tree.
* RET:	True if a rule matched, else false.
* NOTE:	Compiled runtime version.
********************************************/

bool Pat::matchRules(
	Nlppp *nlppp,				// Parse state.
	/*DU*/
	bool (rules)(int,bool&,Nlppp *)		// Rules fn to match on.
	)
{
Node<Pn> *node = nlppp->node_;	// SAVE NODE.

// Need count of rules, or some other way to end the matching on rules
// in the function.
long ruleno = 0;
bool done = false;
while (!done)
	{
	// RESET STATE OF PATTERN MATCH HERE.
	++ruleno;					// Try all the rules.
	resetRule(node, nlppp);
	nlppp->rmost_ = 0;			// Reset.	// RECOPT2.	// 07/23/06 AM.
	updateRestart(nlppp,node);	// Reset.	// RECOPT2.	// 07/24/06 AM.

	if ((rules)(ruleno, done, nlppp))	// If rule matched and actions done.
		{
		if (!nlppp->first_
			 && !nlppp->excised_)								// FIX.	// 04/28/01 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Rule match consumed no nodes.]") << std::ends;
			Parse *parse = nlppp->parse_;									// 12/18/03 AM.
			// FIX.	I believe runtime doesn't use ->rule etc		// 05/17/08 AM.
//			parse->line_ = nlppp->rule_->getLine();		// FIX.	// 05/17/08 AM.
//			errOut(false);														// 12/18/03 AM.
//			errOut(&gerrStr,false, parse->currpass_,parse->line_);			// 12/18/03 AM.
			parse->errOut(&gerrStr,false);					// FIX.	// 05/17/08 AM.
			return false;
			}
//		nlppp->node_ = nlppp->after_;		// Set up for next rule match.// PARSESTATE // 06/03/13 AM.
		return true;
		}
	else								// Rule failed.
		{
		endRestart(nlppp);	// RECOPT2.	// 07/25/06 AM.
		// Clean up for next rule.
		//matchCleanup(nlppp->collect_);
		if (nlppp->exitpass_)								// FIX.	// 02/25/14 AM.
			{
			nlppp->node_ = node;					// RESTORE NODE.	// 02/25/14 AM.
			return false;								// "No match".	// 02/25/14 AM.
			}
		}
	}
nlppp->node_ = node;		// RESTORE NODE.
return false;				// Exhausted rules without a match.
}


/********************************************
* FN:		HEXECUTE
* CR:		06/20/00 AM.
* SUBJ:	Compiled variant of pat pass.
*			HASHED VARIANT.
********************************************/

bool Pat::Hexecute(
	Parse *parse,							// Current parse.
	const PASS *pass						// Static pass data.					// 06/21/00 AM.
#ifdef OLD_
	bool (code)(Nlppp *),				// Gen'd fn for code region of this pass.
	const _TCHAR **arr_select,			// Array of select node names.
	enum selectType seltype,			// Traversal mode.
	const RHASH *must,					// Must-try rule data.
	const RHASH **htab,					// Rule hash table.
	bool (rules)(int,bool&,Nlppp *)		// Rules fn to match on.
#endif
	)
{

if (!parse)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass given no parser or pass information.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

/////////////////////
// CODE ACTIONS
/////////////////////
// Before any other processing, pass will perform its code actions.
// Should change Pat to be ok with no rules (even empty file should be ok).
Nlppp *nlppp = parse->getNlppp();										// 08/07/02 AM.
nlppp->exitpass_ = false;		// Reset.								// 08/07/02 AM.
nlppp->region_ = REGCODE;	// Execute NLP++ in code region.		// 08/09/08 AM.

if (pass->code)
	(pass->code)(nlppp);														// 08/07/02 AM.

if (nlppp->exitpass_)		// Exiting current pass.				// 08/07/02 AM.
	return true;																// 08/07/02 AM.

// Todo: Where there are preactions, should temporarily set to REGPRE.
nlppp->region_ = REGCHECK;	// Reset.									// 08/09/08 AM.

/////////////////////
// INITIALIZE
/////////////////////

// Get the parse tree(s).
Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
//Tree<Ln> *lines = parse->getLines();
if (!tree
// || !lines
   )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass given no parse tree data.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Get the parse tree's root node.
Node<Pn> *root;
root = tree->pRoot;
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass: Tree has no root.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// PREPARE FOR HASHING RULES.
// Get the selected nodes to traverse.

///////////////////////
// TRAVERSE AND MATCH
///////////////////////

// Traverse the tree, matching on each rule in turn.
// For each node in the parse tree, see if it is distinguished.

//Node<Pn> *select;
switch (pass->seltype)			// Type of traversal.
	{
	case SELECTROOT:
//		matchNodes(root->Down(),0,must,htab,rules,parse,0);
		HmatchNodes(root->Down(),0,parse,pass,0);
		break;
	case SELECTNODES:
//		traverseSelect(arr_select,must,htab,rules,parse,root);
		HtraverseSelect(parse,pass,root);
		break;
	case SELECTPATH:
//		pathSelect(arr_select,must,htab,rules,parse,root);
		HpathSelect(pass->arr_select,parse,pass,root);
		break;
	case SELECTMULTI:
//		multiSelect(arr_select,must,htab,rules,parse,root);
		HmultiSelect(parse,pass,root);
		break;
	case SELECTPREMULTI:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Premulti select traversal unimplemented.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Bad select traversal type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	}

prettyPass(root,tree,parse);
return true;
}


/********************************************
* FN:		HTRAVERSESELECT
* CR:		06/20/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive. Compiled runtime variant. HASHED.
*			If found a node, not traversing its children.
* RET:	False if exiting pass.	// 11/11/02 AM.
********************************************/

bool Pat::HtraverseSelect(
#ifdef OLD_
	const _TCHAR *arr_select[],		// Names of select nodes.
	const RHASH *must,
	const RHASH **htab,
	bool (rules)(int,bool&,Nlppp *),	// Compiled rules.
#endif
	Parse *parse,						// Current analysis instance.
	const PASS *pass,							// Static pass data.				// 06/21/00 AM.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node)
	return true;																		// 11/11/02 AM.
// If current node is select, match against its children.
const _TCHAR **pselect = pass->arr_select;
Pn *pn = node->getData();
_TCHAR *name = pn->getName();
while (*pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
//		matchNodes(node->Down(),0,must,htab,rules,parse,node);
		if (!HmatchNodes(node->Down(),0,parse,pass,node))
			return false;																// 11/11/02 AM.
		goto listmanager;
		}
	++pselect;
	}

// Try the rest of the tree.
//if (!flag)		// If current node wasn't select, look within it.
// If node wasn't select and is UNSEALED, then
// look inside it.	// OPT
if (pn->getUnsealed())
//	traverseSelect(arr_select,must,htab,rules,parse,node->Down());
	{
	if (!HtraverseSelect(parse,pass,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// Removes part of recursion, reducing stack overflow problems.
listmanager:
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
//		traverseSelect(arr_select,must,htab,rules,parse,node);
		{
		if (!HtraverseSelect(parse,pass,node))
			return false;
		}
	}
return true;																			// 11/11/02 AM.
}



/********************************************
* FN:		HMATCHNODES
* CR:		06/20/00 AM.
* SUBJ:	Match rules in a range of nodes.
* NOTE:	Compiled runtime variant.
********************************************/

bool Pat::HmatchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
#ifdef OLD_
	const RHASH *must,
	const RHASH **htab,
	bool (rules)(int,bool&,Nlppp *),	// Compiled rules.
#endif
	Parse *parse,										// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *select									// Context node.
	)
{
if (!start || !pass || !pass->rules || !parse)
	return true;

//Delt<Irule> *prule;			// Current rules; also, the rule that matched.
Node<Pn> *node;				// SAVE CURRENT NODE.
node = start;

// Using this object to collect the entire parse state.
// Reset the nlppp object.
Nlppp *nlppp = parse->nlppp_;
//nlppp->rule_ = 0;				// IGNORE.
//nlppp->collect_ = 0;			// IGNORE. (Using a runtime version.)
//nlppp->parse_ = parse;		// ALREADY SET BY init_analyzer.
nlppp->select_	= select;
nlppp->start_	= start;
nlppp->end_		= end;
//nlppp->coll_	= parse->collect_;	// Runtime variant of collect.
nlppp->node_	= node;					// Start at first node of phrase.
nlppp->excised_ = false;													// 04/28/01 AM.
nlppp->noop_ = false;														// 08/12/02 AM.
//resetRule(node, nlppp);				// Reset state of pattern matcher.
		// This is done before every rule.			// 05/18/00 AM.


const int *arr;				// Point to start of array. (For deleting).

while (node)					// While matching on list of nodes.
	{
	// SETUP RULES LIST FOR CURRENT NODE HERE.
	// MERGE HASH TABLE AND MUSTS LISTS AS NEEDED.
	// This will be an array of rule numbers, null terminated.
//	arr = resetRules(nlppp->node_,must,htab);
	arr = resetRules(nlppp->node_,pass);

	if (!arr || !matchRules(nlppp,(int *)arr,pass->rules))	
		{
		node = nlppp->node_ = nodeRight(node,end);
		// Assume start, end unchanged.
		}
	else			// Rule matched.
		{
		nlppp->node_ = nlppp->after_;		// Set up for next rule match.// ALGOSTATE // 06/06/13 AM.
		node  = nlppp->node_;
		start = nlppp->start_;
		end   = nlppp->end_;
		nlppp->sem_ = 0;		// Zero out.
		nlppp->dsem_ = 0;		// Zero out.
		nlppp->excised_ = false;											// 04/28/01 AM.
		nlppp->noop_ = false;												// 08/12/02 AM.
		}

	// Delete the merged array of rule numbers here.
	if (arr)
		delete (int *)arr;

	if (nlppp->exitpass_)													// 11/18/02 AM.
		return false;															// 11/18/02 AM.
	}
return true;
}


/********************************************
* FN:		MATCHRULES
* CR:		06/20/00 AM.
* SUBJ:	Match rules to current node in parse tree.
* RET:	True if a rule matched, else false.
* NOTE:	Compiled runtime version.
********************************************/

bool Pat::matchRules(
	Nlppp *nlppp,				// Parse state.
	int *arr,									// Array of rule numbers.
	bool (rules)(int,bool&,Nlppp *)		// Rules fn to match on.
	)
{
Node<Pn> *node = nlppp->node_;	// SAVE NODE.

// Need count of rules, or some other way to end the matching on rules
// in the function.
long ruleno = 0;
bool done = false;
while (!done)
	{
	// RESET STATE OF PATTERN MATCH HERE.
	if (!(ruleno = *arr))
		return false;							// Done with rule list.
	resetRule(node, nlppp);
	nlppp->rmost_ = 0;			// Reset.	// RECOPT2.	// 07/23/06 AM.
	updateRestart(nlppp,node);	// Reset.	// RECOPT2.	// 07/24/06 AM.

	if ((rules)(ruleno, done, nlppp))	// If rule matched and actions done.
		{
		if (!nlppp->first_
			 && !nlppp->excised_)								// FIX.	// 04/28/01 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Rule match consumed no nodes.]") << std::ends;
			Parse *parse = nlppp->parse_;									// 12/18/03 AM.
//			parse->line_ = nlppp->rule_->getLine();	// FIX.		// 05/17/08 AM.
//			errOut(false);														// 12/18/03 AM.
//			errOut(&gerrStr,false, parse->currpass_,parse->line_);			// 12/18/03 AM.
			parse->errOut(&gerrStr,false);				// FIX.		// 05/17/08 AM.
			return false;
			}
		if (nlppp->exitpass_)												// 11/19/02 AM.
			{
			// Matched a rule, but exiting pass.						// 07/28/03 AM.
//			nlppp->node_ = node;												// 11/19/02 AM.
			nlppp->node_ = 0;													// 07/28/03 AM.
			return true;														// 07/28/03 AM.
			}
		nlppp->node_ = nlppp->after_;		// Set up for next rule match.
		return true;
		}
	else if (nlppp->exitpass_)												// 11/19/02 AM.
		{
		nlppp->node_ = node;													// 11/19/02 AM.
		return false;															// 11/19/02 AM.
		}
	// Rule failed.
	endRestart(nlppp);	// RECOPT2.	// 07/25/06 AM.
	++arr;										// Try next hashed rule number.
	}
nlppp->node_ = node;		// RESTORE NODE.
return false;				// Exhausted rules without a match.
}


/********************************************
* FN:		TREETEXT
* CR:		09/25/00 AM.
* SUBJ:	Traverse subtree to compose its text.
* RET:	ok - True if success, else false.	(10/29/04 AM.)
*			UP buf - The composed text buffer.
* NOTE:	RECURSIVE.
			Only leaves contribute to text.
********************************************/

bool Pat::treeText(		// 10/29/04 AM.
	Node<Pn> *node,
	bool root,				// If this is the top-level node of subtree.
	bool spaces,			// If to insert spaces between words.
	bool &first,				// If this is the first node in the list.
	_TCHAR *buf,				// Buffer to fill.
	_TCHAR* &ptr,				// First empty spot in buffer.
	long &siz				// Empty space left in buffer	// 10/29/04 AM.
	)
{
if (node == 0)
	return true;

bool ok = true;													// 10/29/04 AM.
Pn *pn = node->getData();

// Getting text directly from literal nodes.	// FIX.	// 09/04/03 AM.
_TCHAR *name = pn->getName();						// FIX.	// 09/04/03 AM.

if (*name != '_')		// Literal node.			// FIX.	// 09/04/03 AM.
	{
	long len = _tcsclen(name);									// 10/29/04 AM.
	if (len > (siz-1))	// buf+terminator.				// 10/29/04 AM.
		return false;												// 10/29/04 AM.
	if (spaces && !first)
		strcpy_ee(ptr,_T(" "));
	first = false;
	strcpy_ee(ptr,name);								// FIX.	// 09/04/03 AM.s
	siz -= len;														// 10/29/04 AM.
	}
else if (node->pDown)
	ok = treeText(node->pDown,false,spaces,first,buf,ptr,siz);		// RECURSIVE.
else						// I am a leaf. Do my text here.
	{
	// Copy node text to buffer.
	_TCHAR *text = pn->getText();
	long start = pn->getStart();
	long end = pn->getEnd();
	long len = end-start+1;										// 10/29/04 AM.
	if (len > (siz-1))	// buf+terminator.				// 10/29/04 AM.
		return false;												// 10/29/04 AM.
	int ii;
	for (ii = start; ii <= end; ++ii)
		*ptr++ = *text++;
	siz -= len;														// 10/29/04 AM.
	}

if (root)				// Root of the whole subtree.
	{
	// Return text buffer.
	*ptr = '\0';		// Terminate buffer.
	return ok;
	}

// Minimal recursion in this part (optimization and resource saving!).
if (!(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		ok = treeText(node,false,spaces,first,buf,ptr,siz);
		if (!ok)														// 10/29/04 AM.
		  return ok;												// 10/29/04 AM.
		}
	}
return ok;															// 10/29/04 AM.
}


/**************************************************************/
/**************************************************************/


/********************************************
* FN:		RESETRULE
* CR:		05/18/00 AM.
* SUBJ:	Reset state of pattern matcher for next rule.
* NOTE:	Compiled runtime version.
********************************************/

inline void Pat::resetRule(Node<Pn> *node, Nlppp *nlppp)
{
// Dynamic.
nlppp->node_	= node;
nlppp->first_	= 0;
nlppp->last_	= 0;
nlppp->after_	= 0;
nlppp->sem_		= 0;
nlppp->dsem_	= 0;
nlppp->red_		= 0;
nlppp->used_	= false;

nlppp->ord_			= 0;
nlppp->wildord_	= 0;
nlppp->wildmin_	= -1;
nlppp->wildmax_	= -1;
nlppp->wildmark_	= 0;

nlppp->succeed_ = false;													// 06/10/00 AM.
nlppp->fail_ = false;														// 06/10/00 AM.
nlppp->excised_ = false;													// 04/28/01 AM.
nlppp->noop_ = false;														// 08/12/02 AM.

// Static.
nlppp->elts_		= 0;
nlppp->sugg_		= 0;
nlppp->len_			= -1;

}

/********************************************
* FN:		WILD_BACKFILL_LEFT
* CR:		05/30/00 AM.
* SUBJ:	Backfill wildcards after real match.
* NOTE:	Handling left direction only.
*			05/30/00 AM. Copying wild_backfill.
********************************************/

bool Pat::wild_backfill_left(
	Nlppp *nlppp)
{
if (!nlppp->wildord_)		// No wildcards to fill.
	return true;				// That's ok.

COLL *current_coll = &(nlppp->coll_[nlppp->ord_]);
Node<Pn> *prestart = nlppp->first_;
if (!prestart)					// Rule hasn't matched any nodes yet.
//	prestart = nlppp->first_->pRight;		// (Could still be null).
	prestart = nlppp->wildstart_->pRight;								// 06/01/00 AM.

Node<Pn> *postend  = (Node<Pn> *) current_coll->end;

// Need to know the number of total number of nodes in the wild region.
// Then can load up wildcard collect structures appropriately.
// Need an nlppp->wildtot_   -- track nodes in the wild region.
// Also, if wildcard is pending, processing must be a loop to look for
// the next real match.  If optionals, reset if failed.....

// Wildcards are automatically assigned their minimum.  The algorithm
// below will assign the REMAINING nodes in the wildcard node region
// to wildcards, one at a time, till all the remaining nodes are used.

// The remaining number of nodes to assign.
int remain = nlppp->wildtot_ - nlppp->wildmin_;
if (remain < 0)
	{
	Parse *parse = nlppp->parse_;	// 04/04/10 AM.
	parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Error: wild_backfill_left.(1)]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;			// Should have failed to match.
	}

// Now, traverse the wildcard parts of the collect in tandem with the
// nodes to be assigned to wildcards.

// This is the start collect struct for the wild region.
COLL *coll = &(nlppp->coll_[nlppp->wildord_]);

// The coll for real node(s) matched after wild region.
COLL *bound = &(nlppp->coll_[nlppp->ord_]);

// This holds the min,max data for each rule elt.
const ELT *elt = &(nlppp->elts_[nlppp->wildord_]);

Node<Pn> *node = prestart;
for (; coll != bound; --coll, --elt)
	{
	if (!elt->wild)	// Not a wildcard.  (eg, an optional elt)
		continue;					// Ignore this elt.

	// Fetch the min,max data for the current rule element.
	if (!remain)				// Nothing left to assign.
		;								// Just hand out the nodes.
	else if (elt->max == 0)		// Unlimited wildcard.
		{
		coll->totnodes += remain;
		remain = 0;
		}
	else
		{
		// Compute what to give this wildcard elt.
		int capacity = elt->max - elt->min;
		if (remain <= capacity)		// Assign all to current.
			{
			coll->totnodes += remain;
			remain = 0;
			}
		else								// Assign part of remaining.
			{
			coll->totnodes += capacity;
			remain -= capacity;
			}
		}

	// Assign the nodes to collect here.
	int ii = coll->totnodes;		// Number of nodes to assign.
	if (!ii)								// No nodes for this elt.
		continue;	// Keep going because this could be an unlimited
						// wildcard with limited min-max ones following it.

	if (!(node = Pat::nodeLeft(node, postend)))
		{
		if (prestart)
			{
			Parse *parse = nlppp->parse_;	// 04/04/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error: wild_backfill_left]") << std::ends;
			parse->errOut(&gerrStr,false);
			return false;
			}
		
		// Not possible, since triggered rule must have matched a node.
		if (nlppp->wildstart_)
			node = nlppp->wildstart_;										// 06/01/00 AM.
		else
			{
			Parse *parse = nlppp->parse_;	// 04/04/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error: wild_backfill_left(3)]") << std::ends;
			parse->errOut(&gerrStr,false);
			}

		return false;
		}
	// Get the last node first, cause going backwards.			// 11/18/02 AM.
//	coll->start = node;										// FIX.	// 11/18/02 AM.
	coll->end = node;											// FIX.	// 11/18/02 AM.

	// Now traverse the rest of the nodes for this collect.
	while (--ii > 0)
		{
		if (!(node = Pat::nodeLeft(node, postend)))
			{
			Parse *parse = nlppp->parse_;	// 04/04/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error: wild_backfill_left]") << std::ends;
			parse->errOut(&gerrStr,false);
			return false;
			}
		}
	coll->start = node;
	}

// RESET WILDCARD TRACKING.
nlppp->wildord_	= 0;
nlppp->wildmin_	= -1;
nlppp->wildmax_	= -1;
nlppp->wildmark_	= 0;
return true;
}


/********************************************
* FN:		WILD_BACKFILL_RIGHT
* CR:		05/18/00 AM.
* SUBJ:	Backfill wildcards after real match.
* NOTE:	Handling right direction only.
*			05/30/00 AM. Renamed from wild_backfill for consistency.
* ASS:	Assumes the collect array has minimal values already set for
*			each wildcard and that nlppp state is set up for wildcards.
*			(wildmin, wildmax).
*			Last matched position in collect is the current (real) match.
*			wildord_ position is the first wildcard position.
*			nlppp->last_ is one before the wildcard node region.
*			current collect->start is one after the wildcard node region.
********************************************/

bool Pat::wild_backfill_right(
	Nlppp *nlppp)
{
if (!nlppp->wildord_)		// No wildcards to fill.
	return true;				// That's ok.

COLL *current_coll = &(nlppp->coll_[nlppp->ord_]);
Node<Pn> *prestart = nlppp->last_;
if (!prestart)					// Rule hasn't matched any nodes yet.
//	prestart = nlppp->start_->pLeft;	// (Could still be null).	// 06/01/00 AM.
	prestart = nlppp->wildstart_->pLeft;								// 06/01/00 AM.

Node<Pn> *postend  = (Node<Pn> *) current_coll->start;

// Need to know the number of total number of nodes in the wild region.
// Then can load up wildcard collect structures appropriately.
// Need an nlppp->wildtot_   -- track nodes in the wild region.
// Also, if wildcard is pending, processing must be a loop to look for
// the next real match.  If optionals, reset if failed.....

// Wildcards are automatically assigned their minimum.  The algorithm
// below will assign the REMAINING nodes in the wildcard node region
// to wildcards, one at a time, till all the remaining nodes are used.

// The remaining number of nodes to assign.
int remain = nlppp->wildtot_ - nlppp->wildmin_;
if (remain < 0)
	{
	Parse *parse = nlppp->parse_;	// 04/04/10 AM.
	parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Error: wild_backfill_right.(1)]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;			// Should have failed to match.
	}

// Now, traverse the wildcard parts of the collect in tandem with the
// nodes to be assigned to wildcards.

// This is the start collect struct for the wild region.
COLL *coll = &(nlppp->coll_[nlppp->wildord_]);

// The coll for real node(s) matched after wild region.
COLL *bound = &(nlppp->coll_[nlppp->ord_]);

// This holds the min,max data for each rule elt.
const ELT *elt = &(nlppp->elts_[nlppp->wildord_]);

Node<Pn> *node = prestart;
for (; coll != bound; ++coll, ++elt)
	{
	if (!elt->wild)	// Not a wildcard.  (eg, an optional elt)
		continue;					// Ignore this elt.

	// Fetch the min,max data for the current rule element.
	if (!remain)				// Nothing left to assign.
		;								// Just hand out the nodes.
	else if (elt->max == 0)		// Unlimited wildcard.
		{
		coll->totnodes += remain;
		remain = 0;
		}
	else
		{
		// Compute what to give this wildcard elt.
		int capacity = elt->max - elt->min;
		if (remain <= capacity)		// Assign all to current.
			{
			coll->totnodes += remain;
			remain = 0;
			}
		else								// Assign part of remaining.
			{
			coll->totnodes += capacity;
			remain -= capacity;
			}
		}

	// Assign the nodes to collect here.
	int ii = coll->totnodes;		// Number of nodes to assign.
	if (!ii)								// No nodes for this elt.
		continue;	// Keep going because this could be an unlimited
						// wildcard with limited min-max ones following it.

	if (!(node = Pat::nodeRight(node, postend)))
		{
		if (prestart)
			{
			Parse *parse = nlppp->parse_;	// 04/04/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error: wild_backfill_right]") << std::ends;
			parse->errOut(&gerrStr,false);
		// 05/25/00 AM.
			return false;														// 05/25/00 AM.
			}
		// This is possible, if rule hasn't matched nodes yet and if
		// there are no nodes prior to nlppp->start_
		if (nlppp->wildstart_)
			node = nlppp->wildstart_;	// Start locus for wilds.	// 06/01/00 AM.
		else
			node = nlppp->start_;	// Start locus for current rule.
		}
	coll->start = node;

	// Now traverse the rest of the nodes for this collect.
	while (--ii > 0)
		{
		if (!(node = Pat::nodeRight(node, postend)))
			{
			Parse *parse = nlppp->parse_;	// 04/04/10 AM.
			parse->line_ = nlppp->rule_->getLine();	// 04/04/10 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error: wild_backfill_right]") << std::ends;
			parse->errOut(&gerrStr,false);
			return false;
			}
		}
	coll->end = node;
	}

// UPDATE STATUS, AS NEEDED.												// 06/01/00 AM.
if (!nlppp->first_)	// No "real" nodes matched till now.		// 06/01/00 AM.
	{
	nlppp->first_ = nlppp->wildstart_;
	// nlppp->last_ will be set by the real match that called us.
	}

// RESET WILDCARD TRACKING.	// FIX.									// 05/25/00 AM.
nlppp->wildord_	= 0;
nlppp->wildmin_	= -1;
nlppp->wildmax_	= -1;
nlppp->wildmark_	= 0;

return true;
}


/********************************************
* FN:		ARGS_RANGE
* CR:		06/02/00 AM.
* SUBJ:	Get node range from a number range.
* NOTE:	Compiled runtime variant of PostRFA::args_range.
********************************************/

bool Pat::args_range(
	int from,
	int to,
	COLL *coll,
	int siz,					// Size of collect.
	Nlppp *nlppp,	// 09/21/13 AM.
	/*UP*/
	Node<Pn>* &start,
	Node<Pn>* &end
	)
{
start = end = 0;
if (!from || !to || to < from || to > siz)
	return false;

// Look for leftmost coll that has nodes in it.
COLL *pcoll = &(coll[from]);
int ii;
for (ii = from; ii <= to; ++ii, ++pcoll)
	{
	if ((start = (Node<Pn> *)pcoll->start))
		break;
	}
if (!start)
	return false;

if (from == to)			// A likely case
	{
	end = (Node<Pn> *)pcoll->end;
	return true;
	}

// Now have to look for latest endpoint.
int jj;
pcoll = &(coll[to]);
for (jj = to; jj >= ii; --jj, --pcoll)
	{
	if ((end = (Node<Pn> *)pcoll->end))
		break;
	}

if (!end)
	return false;
return true;
}



/********************************************
* FN:		RESETRULES
* CR:		06/20/00 AM.
* SUBJ:	Build a list of rules for given node to match on.
* RET:	Array of rule numbers.
* NOTE:	Get appropriate rule lists from hash table.  Merge all into
*			one list.
*			HASHED COMPILED RUNTIME VARIANT.
* ALLOC:	CALLER SHOULD DELETE ARRAY.
********************************************/

const int *Pat::resetRules(
	Node<Pn> *node,			// Current node in parse tree.
	const PASS *pass					// Static pass data.						// 06/21/00 AM.
#ifdef OLD_
	const RHASH *must,
	const RHASH **htab
#endif
	)
{
const RHASH *must = pass->must;
const RHASH **htab = pass->htab;
if (!pass || (!must && !htab))
	return 0;

// Copy the must list of rules here.
int *arr = 0;
long asize=0;
if (must)
	{
	asize = must->length;
	if (asize > 0)
		{
		arr = new int [asize+1];
		int *ptr=arr;
		const int *mptr = must->rules;
		--ptr; --mptr;
		while ((*++ptr = *++mptr))
			;										// Copy the array.
		}
	}

if (!htab)	// If there's only a list of must-try rules.
	{
	// Return copy of the must array.
	return arr;
	}

// Traverse down the node's subtree till it branches or till a node with
// BASE attribute is set.  For each node, merge its hashed list with the
// rules so far.

Pn *pn = 0;
int *narr;			// Node's array of rules.
long nsize=0;
int *marr;			// Merged array.
while (node)
	{
	pn = node->getData();


	_TCHAR *nname = pn->getName();					// 03/28/05 AM.
	narr = (int *)Arun::hfind_lc(nname,htab,pass->hsize,
														/*UP*/nsize);	// Get node's rules.
	if (asize && nsize)
		{
		marr = Arun::mergeRules(arr,asize,narr,nsize);		// Merge rule lists.
		delete arr;		// Delete the previous merged array.
		arr = marr;		// The latest merged array.
		asize +=nsize;
		}
	else if (nsize)
		{
		// Copy the node's rule array.
		asize = nsize;
		arr = new int [asize+1];
		int *ptr=arr-1;
		--narr;
		while ((*++ptr = *++narr))
			;										// Copy the array.		
		}

	// Get deaccented version's rules.				// 03/28/05 AM.
	// Opt: A deaccent that counted accents would be good here...
	long len = _tcsclen(nname);						// 03/28/05 AM.
	_TCHAR *deacc = Chars::create(len + 2);		// 03/28/05 AM.
	Xml::de_accent(nname,/*DU*/deacc);				// 03/28/05 AM.

	if (_tcscmp(nname,deacc))							// 09/20/06 AM.
		{
		narr = (int *)Arun::hfind_lc(deacc,htab,pass->hsize,
														/*UP*/nsize);	// Get node's rules.
		// Now merge the deaccented rules found.	// 03/28/05 AM.
		if (asize && nsize)								// 03/28/05 AM.
			{
			marr = Arun::mergeRules(arr,asize,narr,nsize);	// Merge rule lists.
			delete arr;		// Delete the previous merged array.
			arr = marr;		// The latest merged array.
			asize +=nsize;
			}
		else if (nsize)									// 03/28/05 AM.
			{
			// Copy the node's rule array.
			asize = nsize;
			arr = new int [asize+1];
			int *ptr=arr-1;
			--narr;
			while ((*++ptr = *++narr))
				;										// Copy the array.		
			}
		}
	Chars::destroy(deacc);								// 03/28/05 AM.

#ifdef _DEJUNK_
	// Get dejunk version's rules.					// 09/09/11 AM.
	// Opt: A dejunk that counted accents would be good here...
	len = _tcsclen(nname);						// 09/09/11 AM.
	_TCHAR *dejunk = Chars::create(len + 2);		// 09/09/11 AM.
	Xml::de_junk(nname,/*DU*/dejunk);				// 09/09/11 AM.

	if (_tcscmp(nname,dejunk))							// 09/09/11 AM.
		{
		narr = (int *)Arun::hfind_lc(dejunk,htab,pass->hsize,
														/*UP*/nsize);	// Get node's rules.
		// Now merge the dejunk rules found.		// 09/09/11 AM.
		if (asize && nsize)								// 09/09/11 AM.
			{
			marr = Arun::mergeRules(arr,asize,narr,nsize);	// Merge rule lists.
			delete arr;		// Delete the previous merged array.
			arr = marr;		// The latest merged array.
			asize +=nsize;
			}
		else if (nsize)									// 09/09/11 AM.
			{
			// Copy the node's rule array.
			asize = nsize;
			arr = new int [asize+1];
			int *ptr=arr-1;
			--narr;
			while (*++ptr = *++narr)
				;										// Copy the array.		
			}
		}
	Chars::destroy(dejunk);							// 09/09/11 AM.
#endif


	if (pn->getBase())									// Bottommost singlet.
		break;
	if (!(node = node->Down()))							// Leaf.
		break;
	if (node->Right())									// Branches out.
		break;
	}

return arr;
}

/******************************************************************************/
