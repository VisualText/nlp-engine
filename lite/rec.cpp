/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	REC.CPP
* FILE:	lite\rec.cpp
* CR:		11/07/99 AM.
* SUBJ:	Recursive algorithm for analyzer pass.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"															// 09/26/01 AM.
#include "io.h"
#include "string.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
//#include "irule.h"
#include "parse.h"
#include "lite/code.h"
#include "htab.h"
#include "lite/nlppp.h"			// 11/20/99 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "ielt.h"
#include "slist.h"
#include "ifile.h"
#include "Eana.h"
//#include "lite/nlppp.h"			// 11/20/99 AM.
#include "pat.h"
#include "rec.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("rec");

// If user has ever been warned about wildcards at start, end, or trigger
// elements of a rule.
static bool warn_wild = false;

/********************************************
* FN:		Special Functions for the class.
********************************************/

Rec::Rec()								// Default constructor
	: Algo(algo_name /*, 0 */)
{
}

Rec::Rec(const Rec &orig)			// Copy constructor
{
name   = orig.name;
debug_ = orig.debug_;
}

/********************************************
* FN:		Access Functions
********************************************/


/********************************************
* FN:		Modify Functions
********************************************/


/********************************************
* FN:		DUP
* CR:		11/07/99 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Rec::dup()
{
Rec *ptr;
ptr = new Rec(*this);					// Copy constructor.
return (Algo &) *ptr;
}

/********************************************
* FN:		SETUP
* CR:		11/07/99 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Rec::setup(_TCHAR *s_data)
{
// This one is handled internally in Ana::addPass.
// s_data is the name of the rules file.
}


/********************************************
* FN:		EXECUTE
* CR:		11/07/99 AM.
* SUBJ:	Perform the REC pass.
* ALG:	Camp at each select node, executing one rule, then restarting at
*			beginning of phrase, till can't execute rules in the phrase.
********************************************/

bool Rec::Execute(Parse *parse, Seqn *seqn)
{

if (!parse || !seqn)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass given no parser or pass information.]") << std::ends;
	return errOut(&gerrStr,false);
	}


/////////////////////
// CODE ACTIONS
/////////////////////
// Before any other processing, pass will perform its code actions.
// Should change Rec to be ok with no rules (even empty file should be ok).
bool exitpass = false;														// 02/06/01 AM.
Code::execCode(parse, seqn,
						/*UP*/ exitpass);										// 02/06/01 AM.
if (exitpass)																	// 02/06/01 AM.
	return true;																// 02/26/02 AM.


/////////////////////
// INITIALIZE
/////////////////////

// Get the parse tree(s).
Tree<Pn> *tree = (Tree<Pn> *) parse->getTree();
//Tree<Ln> *lines;
//lines = parse->getLines();
if (!tree
// || lines
   )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass given no parse tree data.]") << std::ends;
	return errOut(&gerrStr,false);
	}

// Get the parse tree's root node.
Node<Pn> *root;
root = tree->getRoot();
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass: Tree has no root.]") << std::ends;
	return errOut(&gerrStr,false);
	}

// Get the rule list.
Dlist<Irule> *rulelist;
rulelist = seqn->getRules();
if (!rulelist)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass given no rules.]") << std::ends;
	return errOut(&gerrStr,false);
	}

// Get the rules themselves.
Delt<Irule> *rules;
rules = rulelist->getFirst();
if (!rules)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass: No rules in list.]") << std::ends;
	return errOut(&gerrStr,false);
	}

// PREPARE FOR HASHING RULES.
// Note: The RFA has no rules files.
Ifile *rulesfile = 0;
Slist<Irule> *mustlist = 0;
Selt<Irule> *musts = 0;
tHtab<Slist<Irule> > *htab = 0;
if ((rulesfile = seqn->getRulesfile()))
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
if (!selects)
	{
	// Root of the parse tree is the selected or distinguished node,
	// by default.
	select = root;

	// For each "select" node, match on its list of children.
	if (musts || htab)
		//RecmatchSelect(select, musts, htab, parse);				// 06/01/00 AM.
		RecmatchNodes(select->Down(), 0,									// 06/01/00 AM.
							musts, htab, parse, select);
	else
		{
		// Keeping the non-hashed way for now.
		// The RFA uses it at present.
		//RecmatchSelect(select, rules, parse);						// 06/01/00 AM.
		RecmatchNodes(select->Down(), 0, rules, parse, select);	// 06/01/00 AM.
		}
	}
// Check MULTI TRAVERSAL OF PARSE TREE.
else if (rulesfile && rulesfile->getFmulti())
	{
	if (musts || htab)
		RecmultiSelect(selects, musts, htab, parse, root);
	}
// Check PATH TRAVERSAL OF PARSE TREE.
else if (rulesfile && rulesfile->getFpath())
	{
	if (musts || htab)
		RecpathSelect(selects->getFirst(), musts, htab, parse, root);
	}
// PUT PREMULTI HERE WHEN READY.
else
	{
	// Traverse and find nodes matching the selected list.
	// Apply rules to the children of each.
	// (Start traversing at root.)
	if (musts || htab)
		RectraverseSelect(selects, musts, htab, parse, root);
	else
		{
		// Keeping the nonhashed method for now.
		// The RFA needs it, if noone else.
//		RectraverseSelect(selects, rules, parse, root);
		}
	}

Pat::prettyPass(root,tree,parse);										// 06/08/00 AM.
return true;
}


/********************************************
* FN:		RECTRAVERSESELECT
* CR:		11/08/99 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
********************************************/

bool Rec::RectraverseSelect(
	Dlist<Iarg> *selects,	// Names of select nodes.
	Delt<Irule> *rules,		// Rule set to match against.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
Delt<Iarg> *dselect;
Iarg *select;
Pn *pn;
_TCHAR *name;
pn = node->getData();
name = pn->getName();
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
		if (!RecmatchSelect(node, rules, parse))
			return false;
		goto listmanager;
		}
	}

// Try the rest of the tree.
// If node wasn't select and is UNSEALED, then
// look inside it.
if (pn->getUnsealed())
	{
	if (!RectraverseSelect(selects,rules,parse,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// Removes part of recursion, reducing stack overflow problems.
listmanager:
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!RectraverseSelect(selects,rules,parse,node))
			return false;
		}
	}
return true;
}


/********************************************
* FN:		RECTRAVERSESELECT
* CR:		11/08/99 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			VARIANT USES HASHED RULES.
********************************************/

bool Rec::RectraverseSelect(
	Dlist<Iarg> *selects,	// Names of select nodes.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
Delt<Iarg> *dselect;
Iarg *select;
Pn *pn;
_TCHAR *name;
pn = node->getData();
name = pn->getName();
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
		if (!RecmatchSelect(node, musts, htab, parse))
			return false;
		goto listmanager;
		}
	}

// Try the rest of the tree.
// If node wasn't select and is UNSEALED, then
// look inside it.
if (pn->getUnsealed())
	{
	if (!RectraverseSelect(selects,musts,htab,parse,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// Removes part of recursion, reducing stack overflow problems.
listmanager:
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!RectraverseSelect(selects,musts,htab,parse,node))
			return false;
		}
	}
return true;
}

/********************************************
* FN:		RECMATCHSELECT
* CR:		11/08/99 AM.
* SUBJ:	Match rules in the children of a selected subtree.
********************************************/

bool Rec::RecmatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Delt<Irule> *rules,		// Rule set to match against.
	Parse *parse				// Current analysis instance.
	)
{
Node<Pn> *nodes, *start, *end;
nodes = select->Down();
start = nodes;				// Left boundary for matching rule.
end = 0;						// No right boundary (ie, match till end of nodes.)

return RecmatchNodes(start, end, rules, parse, select);
}


/********************************************
* FN:		RECMATCHSELECT
* CR:		11/08/99 AM.
* SUBJ:	Match rules in the children of a selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
********************************************/

bool Rec::RecmatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse				// Current analysis instance.
	)
{
Node<Pn> *nodes, *start, *end;
nodes = select->Down();
start = nodes;				// Left boundary for matching rule.
end = 0;						// No right boundary (ie, match till end of nodes.)

return RecmatchNodes(start, end, musts, htab, parse, select);
}


/********************************************
* FN:		RECMATCHNODES
* CR:		11/08/99 AM.
* SUBJ:	Match rules in a range of nodes.
* ALG:	Recursive pattern match algorithm:
*			Every time a rule matches, restart at start of range.
*			When no rules match, done.
* NOTE:	May want to check a threshold of reductions that do not
*			consume nodes.
* RET:	11/18/02 AM. False if aborting pass.
********************************************/

bool Rec::RecmatchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Delt<Irule> *rules,		// Rule set to match against.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *select
	)
{
if (!start || !rules || !parse)
	return true;

// The collect list mirrors the current rule elements.  Serves as an
// instance of the rule, oveseeing its execution.
//Tree<Pn> *collect;			// List of matched rule elements.
//collect = new Tree<Pn>();	// Empty so far.

//Node<Pn> *first, *last;		// Nodes matched by rule, inclusive.
//Sem *sem = 0;					// Semantic info for rule.
//Dlist<Ipair> *dsem = 0;		// Dynamic semantics for sugg.	// 11/17/99 AM.

Delt<Irule> *prule;			// Current rules; also, the rule that matched.
Node<Pn> *node;
bool matched = false;		// If any rules matched.

// 11/20/99 AM. Using this object to collect the entire parse state.
Nlppp nlppp(															// 11/20/99 AM.
	0,								// No rule yet.
	new Tree<Pn>(),			// Empty collect_.
	parse,select,
	0,0,							// No first,last yet.
	0,								// No node yet.
	start,end,
	0,0							// No sem, dsem yet.
	);
node = nlppp.node_ = start;	// TRYING HERE!!!!	// 04/27/06 AM.
//nlppp.maxspan_ = 0;						// RECOPT1	// 04/27/06 AM.
nlppp.rightmost_ = 0;						// RECOPT2	// 07/17/06 AM.
node->getData()->setRestart(0);			// RECOPT2.	// 07/21/06 AM.

for (;;)							// WHILE RECURSING....
	{
	matched = false;				// NO MATCHES IN THIS CYCLE YET.

	while (node)					// While matching on list of nodes.
		{
		// For each rule in the rules list.
		prule = rules;				// Reset the rules list.
		if (Debug())
			*gout << _T(" Matching on node:") << *node << std::endl;

		//if (!Pat::matchRules(node,start,end,parse,select,
		//					/*DU*/ prule,collect,first,last,sem,dsem))
		if (!Pat::matchRules(&nlppp, /*DU*/ prule))		// 11/20/99 AM.
			{
			if (node == nlppp.rightmost_)	// RECOPT2.	// 07/23/06 AM.
				nlppp.rightmost_ = 0;		// RECOPT2.	// 07/23/06 AM.
			node = nlppp.node_ = Pat::nodeRight(node,end);
			}
		else
			{
			// Successful rule match.  Execute POST actions.
			nlppp.rule_ = prule->getData();						// 11/20/99 AM.
			//if (!Pat::execActions(prule->getData(),collect,first,last,
			//						parse,sem,dsem,select, /*DU*/ node, start, end))
			if (!Pat::execActions(&nlppp))						// 11/20/99 AM.
				{
				// FIX for infinite looping. // 11/10/99 AM.
				// If something went wrong with actions, abort the matching.
				// Also if nlppp->exitpass_ set.							// 11/11/02 AM.
				delete (Tree<Pn> *) nlppp.collect_;
				return nlppp.exitpass_;										// 11/18/02 AM.
				}
			// If the rule changed current phrase.						// 08/08/02 AM.
			else if (nlppp.excised_)										// 08/08/02 AM.
				{
				// Note: Current node is potentially mangled/deleted.
				nlppp.excised_ = false;										// 08/08/02 AM.
				nlppp.noop_ = false;											// 08/08/02 AM.
				}
			else if (nlppp.noop_)	// If rule was noop.			// 12/15/99 AM.
				{
				// Continue from END of noop rule match.			// 02/10/05 AM.
//				node = nlppp.node_ = Pat::nodeRight(node,end);	// 12/15/99 AM.
				node = nlppp.node_;							// FIX.	// 02/10/05 AM.
				if (nlppp.rmost_)								// RECOPT2.	// 07/23/06 AM.
					nlppp.rightmost_ = node;				// RECOPT2.	// 07/23/06 AM.
				nlppp.noop_ = false;	// Clear out.					// 12/15/99 AM.
				nlppp.sem_ = 0;		// Zero out.					// 12/15/99 AM.
				nlppp.dsem_ = 0;		// Zero out.					// 12/15/99 AM.
				continue;				// KEEP MATCHING.				// 12/15/99 AM.
				}

			matched = true;	// A RULE MATCHED IN THIS CYCLE.
			
			// Optimization.  Go back according to SPAN.				// 02/04/05 AM.
//			start = recMoveback(&nlppp);									// 02/04/05 AM.
	
			// RESET START AND END OF PHRASE.
			start = nlppp.start_ = nlppp.select_->Down();			// 02/04/05 AM.
			end = nlppp.end_ = 0;
			nlppp.sem_ = 0;		// Zero out.					// 11/20/99 AM.
			nlppp.dsem_ = 0;		// Zero out.					// 11/20/99 AM.

			node = matchRestart(&nlppp);	// RECOPT1	// 04/27/06 AM.
			break;				// RESTART AT BEGINNING OF PHRASE.
			}

		if (nlppp.exitpass_)													// 11/18/02 AM.
			{
			delete (Tree<Pn> *) nlppp.collect_;							// 11/18/02 AM.
			return false;														// 11/18/02 AM.
			}
		}
	if (!matched)
		break;						// NO NODES MATCHED, SO FINISHED.
	}
delete (Tree<Pn> *) nlppp.collect_;
// nlppp.collect_ = 0;
return true;
}

/********************************************
* FN:		RECMATCHNODES
* CR:		11/08/99 AM.
* SUBJ:	Match rules in a range of nodes.
* NOTE:	Split from matchSelect.
*			VARIANT USES HASHED RULES.
********************************************/

bool Rec::RecmatchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	Node<Pn> *select
	)
{
if (!start || !htab || !parse)
	return true;

bool ok = true;				// 11/10/99 AM.

// The collect list mirrors the current rule elements.  Serves as an
// instance of the rule, oveseeing its execution.
//Tree<Pn> *collect;			// List of matched rule elements.
//collect = new Tree<Pn>();	// Empty so far.

//Node<Pn> *first, *last;		// Nodes matched by rule, inclusive.
//Sem *sem = 0;					// Semantic info for rule.
//Dlist<Ipair> *dsem = 0;		// Dynamic semantics for sugg.	// 11/17/99 AM.

Slist<Irule> *rules = 0;	// Current rules list.
Selt<Irule> *prule = 0;		// Current rules; also, the rule that matched.
Node<Pn> *node;
bool matched = false;

// 11/20/99 AM. Using this object to collect the entire parse state.
Nlppp nlppp(															// 11/20/99 AM.
	0,								// No rule yet.
	new Tree<Pn>(),			// Empty collect_.
	parse,select,
	0,0,							// No first,last yet.
	0,								// No node yet.
	start,end,
	0,0							// No sem, dsem yet.
	);
// nlppp.maxspan_ = 0;			// Reset.						// 02/04/05 AM.

// Tracking potential infinite loop.							// 06/05/03 AM.
long state_start = 0;												// 06/05/03 AM.
long state_end = 0;													// 06/05/03 AM.
long state_count = 0;												// 06/05/03 AM.
long state_line = 0;													// 01/23/05 AM.

// Tracking a second state.										// 01/23/05 AM.
bool state_matched = false;	// If matched any state.	// 01/23/05 AM.
long state_toggle = 1;	// Point to 1 or 2 as next fill.	// 01/23/05 AM.
long state_start2 = 0;												// 01/23/05 AM.
long state_end2 = 0;													// 01/23/05 AM.
long state_count2 = 0;												// 01/23/05 AM.
long state_line2 = 0;												// 01/23/05 AM.

Pn *pn;

node = nlppp.node_ = start;			// 11/20/99 AM.	// 04/27/06 AM.
//nlppp.maxspan_ = 0;							// RECOPT1	// 04/27/06 AM.
nlppp.rightmost_ = 0;							// RECOPT2	// 07/17/06 AM.
node->getData()->setRestart(0);				// RECOPT2.	// 07/21/06 AM.

for (;;)							// WHILE RECURSING.....
	{
	matched = false;				// NO MATCHES IN THIS CYCLE YET.
	//node = start;							// 11/20/99 AM.

	while (node)					// While matching on list of nodes.
		{
		// SETUP RULES LIST FOR CURRENT NODE HERE.
		// MERGE HASH TABLE AND MUSTS LIST AS NEEDED.
		prule = Pat::resetRules(node, musts, htab, /*DU*/ rules);

		if (Debug())
			*gout << _T(" Matching on node:") << *node << std::endl;

		// For each rule in the rules list.
		//if (!Pat::matchRules(node, start, end, parse,select,
		//						/*DU*/ prule,collect,first,last,sem,dsem))
		if (!Pat::matchRules(&nlppp, /*DU*/ prule))		// 11/20/99 AM.
			{
			if (node == nlppp.rightmost_)	// RECOPT2.	// 07/23/06 AM.
				nlppp.rightmost_ = 0;		// RECOPT2.	// 07/23/06 AM.
			node = nlppp.node_ = Pat::nodeRight(node,end);
			// Clear out previous node's list.
			if (rules)
				Slist<Irule>::DeleteSlist(rules);
			rules = 0;		// GOOD TO DO!								// 12/15/99 AM.
			}
		else
			{
			// Successful rule match.
			
			// Execute POST actions.
			nlppp.rule_ = prule->getData();						// 11/20/99 AM.
			//ok = Pat::execActions(prule->getData(),collect,first,last,
			//					parse,sem,dsem,select, /*DU*/ node, start, end);
			ok = Pat::execActions(&nlppp);							// 11/20/99 AM.
			// Clear out previous node's list.
			if (rules)
				Slist<Irule>::DeleteSlist(rules);
			rules = 0;		// GOOD TO DO!								// 12/15/99 AM.

			// Exec actions may remove the current node.			// 09/22/08 AM.
			if (nlppp.node_)								// Checking	// 09/22/08 AM.
				pn = nlppp.node_->getData();							// 06/03/03 AM.
			else																// 09/22/08 AM.
				pn = 0;														// 09/22/08 AM.
			// Check for potential infinite loop.					// 06/05/03 AM.

			if (!ok)						// 11/10/99 AM.
				{
				// Something went wrong with actions. Abort processing
				// the current phrase.							// 11/10/99 AM.
				// Also if nlppp->exitpass_ set.							// 11/11/02 AM.
				delete (Tree<Pn> *) nlppp.collect_;			// 11/10/99 AM.
				return nlppp.exitpass_;										// 11/18/02 AM.
				}
			// If the rule changed current phrase.						// 08/08/02 AM.
			else if (nlppp.excised_)										// 08/08/02 AM.
				{
				// Note: Current node is potentially mangled/deleted.
				nlppp.excised_ = false;										// 08/08/02 AM.
				nlppp.noop_ = false;											// 08/08/02 AM.
				}
			else if (nlppp.noop_)	// If rule was noop.			// 12/15/99 AM.
				{
				// Continue from END of noop match.						// 02/10/05 AM.
//				node = nlppp.node_ = Pat::nodeRight(node,end);	// 12/15/99 AM.
				node = nlppp.node_;								// FIX.	// 02/10/05 AM.
				if (nlppp.rmost_)								// RECOPT2.	// 07/23/06 AM.
					nlppp.rightmost_ = node;				// RECOPT2.	// 07/23/06 AM.
				// Clear out previous node's list.
				nlppp.noop_ = false;	// Clear out.					// 12/15/99 AM.
				nlppp.sem_ = 0;		// Zero out.					// 12/15/99 AM.
				nlppp.dsem_ = 0;		// Zero out.					// 12/15/99 AM.
				continue;				// KEEP MATCHING.				// 12/15/99 AM.
				}
			else if (RecinfLoop(&nlppp,pn,parse, /*DU*/			// 06/09/05 AM.
				state_start,state_end,state_count,state_line,
				state_matched,state_toggle,state_start2,
				state_end2,state_count2,state_line2))
				{
				delete (Tree<Pn> *) nlppp.collect_;	// LEAK.		// 11/01/04 AM.
				if (rules)									// LEAK.		// 11/01/04 AM.
					Slist<Irule>::DeleteSlist(rules);// LEAK.		// 11/01/04 AM.
				nlppp.start_ = nlppp.select_->Down();				// 11/01/04 AM.
				nlppp.end_ = 0;											// 11/01/04 AM.
				nlppp.sem_ = 0;		// Zero out.					// 11/01/04 AM.
				nlppp.dsem_ = 0;		// Zero out.					// 11/01/04 AM.
				nlppp.exitpass_ = true;									// 01/02/04 AM.
//				return parse->errOut(false);							// 01/23/05 AM.
				return false;												// 01/23/05 AM.
				}

			// Optimization.  Go back according to SPAN.				// 02/04/05 AM.
//			start = recMoveback(&nlppp);									// 02/04/05 AM.

			// RESET START,END OF PHRASE.
			start = nlppp.start_ = nlppp.select_->Down();			// 02/04/05 AM.
			end = nlppp.end_ = 0;
			nlppp.sem_ = 0;		// Zero out.					// 11/20/99 AM.
			nlppp.dsem_ = 0;		// Zero out.					// 11/20/99 AM.
			matched = true;			// A RULE MATCHED IN THIS CYCLE.
			node = matchRestart(&nlppp);	// RECOPT1	// 04/27/06 AM.
			break;						// RESTART AT BEGINNING OF PHRASE.
			}

		if (nlppp.exitpass_)													// 11/18/02 AM.
			{
			delete (Tree<Pn> *) nlppp.collect_;							// 11/18/02 AM.
			return false;														// 11/18/02 AM.
			}
		}
	if (!matched)
		break;							// NO NODES MATCHED, SO FINISHED.
	}
delete (Tree<Pn> *) nlppp.collect_;
return true;
}

/********************************************
* FN:		RECINFLOOP
* CR:		06/09/05 AM.
* SUBJ:	Accounting for infinite grammar loops.
********************************************/

inline bool Rec::RecinfLoop(
	Nlppp *nlppp,
	Pn *pn,
	Parse *parse,				// Current analysis instance.
	/*DU*/
	long &state_start,
	long &state_end,
	long &state_count,
	long &state_line,

	bool &state_matched,	// If matched any state.
	long &state_toggle,	// Point to 1 or 2 as next fill.
	long &state_start2,
	long &state_end2,
	long &state_count2,
	long &state_line2
	)
{
// Check for potential infinite loop.						// 06/05/03 AM.
if (nlppp->noop_ || nlppp->excised_)						// 06/09/05 AM.
	return false; // Don't contribute to inf loop.		// 06/09/05 AM.
else if (!pn)								// Checking			// 09/22/08 AM.
	return false;							// Checking			// 09/22/08 AM.
else if (pn->getStart() == state_start						// 01/23/05 AM.
	&& pn->getEnd() == state_end	)							// 01/23/05 AM.
	{
	state_matched = true;
	++state_count;
	if (nlppp->rule_)	// 05/11/13 AM.
		state_line = nlppp->rule_->getLine();				// 01/23/05 AM.
	state_toggle = 2;
	}
else if (pn->getStart() == state_start2					// 01/23/05 AM.
	&& pn->getEnd() == state_end2	)							// 01/23/05 AM.
	{
	state_matched = true;
	++state_count2;
	if (nlppp->rule_)	// 05/11/13 AM.
		state_line2 = nlppp->rule_->getLine();				// 01/23/05 AM.
	state_toggle = 1;
	}
else	// Didn't match any state.								// 01/23/05 AM.
	{
	if (state_toggle == 1)										// 01/23/05 AM.
		{
		state_start = pn->getStart();							// 06/05/03 AM.
		state_end = pn->getEnd();								// 06/05/03 AM.
		state_count = 0;	// Reset.							// 06/05/03 AM.
		state_toggle = 2;											// 01/23/05 AM.
		}
	else if (state_toggle == 2)								// 01/23/05 AM.
		{
		state_start2 = pn->getStart();						// 01/23/05 AM.
		state_end2 = pn->getEnd();								// 01/23/05 AM.
		state_count2 = 0;	// Reset.							// 01/23/05 AM.
		state_toggle = 1;											// 01/23/05 AM.
		}
	}
if (state_matched)												// 01/23/05 AM.
	{
	state_matched = false;										// 01/23/05 AM.
	if ((state_count+state_count2) >= 100)					// 01/23/05 AM.
		{
//				parse->line_ = nlppp.rule_->getLine();	// FIX.	// 09/24/04 AM.
		if (state_count)											// 01/23/05 AM.
			{
			parse->line_ = state_line;							// 01/23/05 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Possible infinite loop.]") << std::ends;
			parse->errOut(&gerrStr,false);					// 01/23/05 AM.
			}
		if (state_count2)											// 01/23/05 AM.
			{
			parse->line_ = state_line2;						// 01/23/05 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Possible infinite loop.]") << std::ends;
			parse->errOut(&gerrStr,false);					// 01/23/05 AM.
			}
		return true;	// Flag infinite loop.
		}
	}
return false;
}


/********************************************
* FN:	MATCHRESTART
* CR:	04/27/06 AM.
* SUBJ:	Restart phrase after a match.
* RET:	Node at which to restart matching in current phrase.
* ALG:	Recursive pattern match algorithm:
* OPT:	Track maximum coverage of any match in current phrase.
*		(Even for failed rules.)
* NOTE:	Need to watch out if first node of phrase was reduced.
*	Optimization seems to be a dud.  Try without it.
*	07/17/06 AM. RECOPT2.
*		Track rightmost covered node in phrase.
*		Nodes of a FAILED match point back to the EARLIEST node
*		covering such nodes.
********************************************/

inline Node<Pn> *Rec::matchRestart(	// RECOPT1
	Nlppp *nlppp
	)
{
//#ifdef RECOPT2_
Parse *parse = nlppp->parse_;
Eana *eana = parse->getEana();
if (!eana->getFintern())
{

if (nlppp->first_)
	{
	Pn *pn = nlppp->first_->getData();
	if (pn->getRestart())
		return nlppp->node_
			= nlppp->rightmost_	// FIX.	02/14/13 AM.
			= pn->getRestart();
	// If things working correctly, then this is right.	// RECOVERHAUL.// 02/04/13 AM.
	else if (nlppp->first_)							// RECOVERHAUL.// 02/04/13 AM.
		return nlppp->node_
			= nlppp->rightmost_	// FIX.	02/14/13 AM.
			= nlppp->first_;	// RECOVERHAUL.// 02/04/13 AM.

#ifdef RECOVERHAUL_130204
	// The below will overlook recursive possibilities.	// FIX.	// 08/15/08 AM.
//	else if (nlppp->red_)
//		return nlppp->node_ = nlppp->red_;	// Start at currently reduced node!!
//	else
//		return nlppp->node_ = nlppp->first_;

	// Look backward, else restart at beginning of phrase.	// FIX	// 08/15/08 AM.
	else if (nlppp->start_)												// FIX	// 08/15/08 AM.
		return nlppp->node_
			= nlppp->rightmost_	// FIX.	02/14/13 AM.
			= nlppp->start_;						// FIX	// 08/15/08 AM.
#endif
	// ELSE FALL THROUGH. (no optimization)						// FIX	// 08/15/08 AM.
	}

}
//#endif

// OPTIMIZE COMPUTATION.


// FORGET THIS OPTIMIZATION FOR NOW.	// 04/27/06 AM.
Node<Pn> *node = nlppp->node_
	= nlppp->rightmost_	// FIX.	02/14/13 AM.
	= nlppp->select_->Down();	// FORGET OPT!	// 04/27/06 AM.
return node;							// FORGET OPT!	// 04/27/06 AM.

#ifdef RECOPT1_

// Traverse back according to max coverage of any rule.
// If at beginning, use that.
//Node<Pn> *node = nlppp->node_;	// FORGET OPT!	// 04/27/06 AM.
Node<Pn> *prev = 0;

long ii = nlppp->maxspan_;

for (;;)	// BACK UP FOR RESTART ON CURRENT PHRASE.
	{
	if (node)
		prev = node->Left();
	if (!prev)
		{
		node = nlppp->select_->Down();	// Start at beginning of phrase.
		break;
		}
	node = prev;
	if (ii <= 0)
		break;
	--ii;
	}

if (nlppp->span_ > nlppp->maxspan_)
	nlppp->maxspan_ = nlppp->span_;

nlppp->node_ = node;
return node;
#endif
}

/******************************************************************************/
//				COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		EXECUTE
* CR:		06/01/00 AM.
* SUBJ:	Compiled runtime variant of the REC pass.
********************************************/

bool Rec::Execute(
	Parse * parse,							// Current parse.
	const PASS *pass					// Static pass data.						// 06/21/00 AM.
	)
{

if (!parse)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass given no parser or pass information.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}


/////////////////////
// CODE ACTIONS
/////////////////////
// Before any other processing, pass will perform its code actions.
// Should change Rec to be ok with no rules (even empty file should be ok).
Nlppp *nlppp = parse->getNlppp();							// FIX.	// 08/04/03 AM.
nlppp->exitpass_ = false;		// Reset.					// FIX.	// 08/04/03 AM.
nlppp->region_ = REGCODE;	// Execute NLP++ in code region.		// 08/09/08 AM.

if (pass->code)
	(pass->code)(nlppp);														// 08/04/03 AM.

if (nlppp->exitpass_)		// Exiting current pass.	// FIX.	// 08/04/03 AM.
	return true;													// FIX.	// 08/04/03 AM.

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
	gerrStr << _T("[Rec pass given no parse tree data.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Get the parse tree's root node.
Node<Pn> *root;
root = tree->pRoot;
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass: Tree has no root.]") << std::ends;
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
		// For each "select" node, match on its list of children.
		//if (musts || htab)
		//	RecmatchSelect(select, musts, htab, parse);
		//else
			{
			// Keeping the non-hashed way for now.
			// The RFA uses it at present.
			//RecmatchSelect(select, rules, parse);
//			matchNodes(root->Down(),0,rules,parse,0);
			matchNodes(root->Down(),0,parse,pass,0);
			}
		break;
	case SELECTNODES:
		// Traverse and find nodes matching the selected list.
		// Apply rules to the children of each.
		// (Start traversing at root.)
		//	if (musts || htab)
		//		RectraverseSelect(selects, musts, htab, parse, root);
		//	else
				{
				// Keeping the nonhashed method for now.
				// The RFA needs it, if noone else.
		//		RectraverseSelect(selects, rules, parse, root);
//				traverseSelect(arr_select, rules, parse, root);
				traverseSelect(parse,pass,root);
				}
		break;
	case SELECTPATH:
		//	if (musts || htab)
		//		RecpathSelect(selects->getFirst(), musts, htab, parse, root);
//		RecpathSelect(arr_select,rules,parse,root);
		RecpathSelect(pass->arr_select,parse,pass,root);
		break;
	case SELECTMULTI:
		//	if (musts || htab)
		//		RecmultiSelect(selects, musts, htab, parse, root);
//		RecmultiSelect(arr_select,rules,parse,root);
		RecmultiSelect(parse,pass,root);
		break;											// FIX.	// 08/04/03 AM.
	case SELECTPREMULTI:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Rec: Premulti select traversal unimplemented.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Rec: Bad select traversal type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	}

Pat::prettyPass(root,tree,parse);										// 06/08/00 AM.
return true;
}


/********************************************
* FN:		TRAVERSESELECT
* CR:		06/01/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive. Compiled runtime variant of RectraverseSelect.
*			If found a node, not traversing its children.
********************************************/

bool Rec::traverseSelect(
	Parse *parse,						// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect = pass->arr_select;
Pn *pn;
_TCHAR *name;
pn = node->getData();
name = pn->getName();
while (*pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
//		matchNodes(node->Down(),0,rules,parse,node);
		if (!matchNodes(node->Down(),0,parse,pass,node))					// 11/18/02 AM.
			return false;																// 11/18/02 AM.
		goto listmanager;
		}
	++pselect;
	}

// Try the rest of the tree.
// If node wasn't select and is UNSEALED, then
// look inside it.
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
* CR:		06/01/00 AM.
* SUBJ:	Match rules in a range of nodes.
* ALG:	Recursive pattern match algorithm:
*			Every time a rule matches, restart at start of range.
*			When no rules match, done.
* NOTE:	May want to check a threshold of reductions that do not
*			consume nodes.
*			Compiled runtime variant of RecmatchNodes.
********************************************/

bool Rec::matchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *select
	)
{
if (!start || !pass->rules || !parse)
	return false;

//Delt<Irule> *prule;			// Current rules; also, the rule that matched.
Node<Pn> *node;
bool matched = false;		// If any rules matched.

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
//nlppp->node_	= node;					// Start at first node of phrase.
//resetRule(node, nlppp);				// Reset state of pattern matcher.
		// This is done before every rule.

// Tracking potential infinite loop.							// 05/11/13 AM.
long state_start = 0;												// 05/11/13 AM.
long state_end = 0;													// 05/11/13 AM.
long state_count = 0;												// 05/11/13 AM.
long state_line = 0;													// 05/11/13 AM.

// Tracking a second state.										// 05/11/13 AM.
bool state_matched = false;	// If matched any state.	// 05/11/13 AM.
long state_toggle = 1;	// Point to 1 or 2 as next fill.	// 05/11/13 AM.
long state_start2 = 0;												// 05/11/13 AM.
long state_end2 = 0;													// 05/11/13 AM.
long state_count2 = 0;												// 05/11/13 AM.
long state_line2 = 0;												// 05/11/13 AM.

Pn *pn;	// 05/11/13 AM.

node = nlppp->node_ = start;	// 04/27/06 AM.
//nlppp->maxspan_ = 0;		// RECOPT1	// 04/27/06 AM.
nlppp->rightmost_ = 0;						// RECOPT2	// 07/17/06 AM.
node->getData()->setRestart(0);			// RECOPT2.	// 07/21/06 AM.

for (;;)							// WHILE RECURSING....
	{
	matched = false;				// NO MATCHES IN THIS CYCLE YET.

	while (node)					// While matching on list of nodes.
		{

		// For each rule in the rules list.
		//prule = rules;				// Reset the rules list.
		//if (Debug())
		//	*gout << " Matching on node:" << *node << std::endl;

		if (!Pat::matchRules(nlppp, pass->rules))
			{
			if (node == nlppp->rightmost_)	// RECOPT2.	// 07/23/06 AM.
				nlppp->rightmost_ = 0;		// RECOPT2.	// 07/23/06 AM.
			node = nlppp->node_ = Pat::nodeRight(node,end);

			if (nlppp->exitpass_)	// Aborting pass.					// 11/18/02 AM.
				return false;													// 11/18/02 AM.
			}
		else
			{
			// Successful rule match.  Execute POST actions.
			//nlppp.rule_ = prule->getData();
			//if (!Pat::execActions(prule->getData(),collect,first,last,
			//						parse,sem,dsem,select, /*DU*/ node, start, end))
			//if (!Pat::execActions(&nlppp))
			//	{
				// FIX for infinite looping.
				// If something went wrong with actions, abort the matching.
			//	delete (Tree<Pn> *) nlppp.collect_;
			//	return;
			//	}
			//else

			// Exec actions may remove the current node.			// 05/11/13 AM.
			if (nlppp->node_)							// Checking		// 05/11/13 AM.
				pn = nlppp->node_->getData();							// 05/11/13 AM.
			else if (node)									// RECINFL	// 05/11/13 AM.
				pn = node->getData();					// RECINFL	// 05/11/13 AM.
			else																// 05/11/13 AM.
				pn = 0;														// 05/11/13 AM.
			// Check for potential infinite loop.					// 05/11/13 AM.

			if (nlppp->exitpass_)	// Aborting pass.					// 11/12/02 AM.
				return false;													// 11/12/02 AM.

			// If the rule changed current phrase.						// 08/08/02 AM.
			if (nlppp->excised_)												// 08/08/02 AM.
				{
				// Note: Current node is potentially mangled/deleted.
				nlppp->excised_ = false;									// 08/08/02 AM.
				nlppp->noop_ = false;										// 08/08/02 AM.
				}
			else if (nlppp->noop_)	// If rule was noop.
				{
				// Continue from END of noop match.						// 02/10/05 AM.
//				node = nlppp->node_ = Pat::nodeRight(node,end);
				node = nlppp->node_;								// FIX.	// 02/10/05 AM.
				if (nlppp->rmost_)							// RECOPT2.	// 07/23/06 AM.
					nlppp->rightmost_ = node;				// RECOPT2.	// 07/23/06 AM.
				nlppp->noop_ = false;	// Clear out.
				nlppp->sem_ = 0;		// Zero out.
				nlppp->dsem_ = 0;		// Zero out.
				continue;				// KEEP MATCHING.
				}
			else if (RecinfLoop(nlppp,pn,parse, /*DU*/	// 05/11/13 AM.
				state_start,state_end,state_count,state_line,
				state_matched,state_toggle,state_start2,
				state_end2,state_count2,state_line2))
				{
				delete (Tree<Pn> *) nlppp->collect_;	// 05/11/13 AM.
				//if (rules)									// 05/11/13 AM.
				//	Slist<Irule>::DeleteSlist(rules);	// 05/11/13 AM.
				nlppp->start_ = nlppp->select_->Down();// 05/11/13 AM.
				nlppp->end_ = 0;								// 05/11/13 AM.
				nlppp->sem_ = 0;		// Zero out.		// 05/11/13 AM.
				nlppp->dsem_ = 0;		// Zero out.		// 05/11/13 AM.
				nlppp->exitpass_ = true;					// 05/11/13 AM.
//				return parse->errOut(false);				// 05/11/13 AM.
				return false;									// 05/11/13 AM.
				}

			matched = true;	// A RULE MATCHED IN THIS CYCLE.
			// RESET START AND END OF PHRASE.
			start = nlppp->start_ = nlppp->select_->Down();
			end = nlppp->end_ = 0;
			nlppp->sem_ = 0;		// Zero out.
			nlppp->dsem_ = 0;		// Zero out.

			node = matchRestart(nlppp);	// RECOPT1	// 04/27/06 AM.
			break;				// RESTART AT BEGINNING OF PHRASE.
			}
		}
	if (!matched)
		break;						// NO NODES MATCHED, SO FINISHED.
	}
//delete (Tree<Pn> *) nlppp.collect_;
// nlppp.collect_ = 0;
return true;
}


/********************************************
* FN:		RECMOVEBACK
* CR:		02/04/05 AM..
* SUBJ:	Move backward to reset in recursive pass.
* RET:	start = node to restart matching at, in current phrase.
* NOTE:	Using span variables to track number of nodes matched by
*			failed rules.  Go back according to maxspan.
* OBSOLETE: Superseded by matchRestart.
********************************************/

inline Node<Pn> *Rec::recMoveback(Nlppp *nlppp)
{
if (!nlppp)
  return 0;

// First node in current phrase.
Node<Pn> *first = nlppp->select_->Down();

// First node in current rule match.
Node<Pn> *node = nlppp->first_;

// Number of nodes to move back in current phrase.
// Based on maximum number of nodes matched in any rule in current phrase.
long num = nlppp->maxspan_;
// Do not reset maxspan here, as we're still in current phrase.

for (;;)
	{
	if (!node)
		{
		nlppp->start_ = first;
		return first;
		}
	if (node == first)
		break;
	if (--num <= 0)
		break;
	node = node->Left();
	}

nlppp->start_ = node;
return node;
}


/******************************************************************************/
//				HASHED COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		HEXECUTE
* CR:		06/20/00 AM.
* SUBJ:	Compiled runtime variant of the REC pass.
********************************************/

bool Rec::Hexecute(
	Parse * parse,							// Current parse.
	const PASS *pass					// Static pass data.						// 06/21/00 AM.
	)
{

if (!parse)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass given no parser or pass information.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}


/////////////////////
// CODE ACTIONS
/////////////////////
// Before any other processing, pass will perform its code actions.
// Should change Rec to be ok with no rules (even empty file should be ok).

Nlppp *nlppp = parse->getNlppp();							// FIX.	// 08/04/03 AM.
nlppp->exitpass_ = false;		// Reset.					// FIX.	// 08/04/03 AM.
nlppp->region_ = REGCODE;	// Execute NLP++ in code region.		// 08/09/08 AM.

if (pass->code)
	(pass->code)(nlppp);														// 08/04/03 AM.

if (nlppp->exitpass_)		// Exiting current pass.	// FIX.	// 08/04/03 AM.
	return true;													// FIX.	// 08/04/03 AM.

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
	gerrStr << _T("[Rec pass given no parse tree data.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Get the parse tree's root node.
Node<Pn> *root;
root = tree->pRoot;
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Rec pass: Tree has no root.]") << std::ends;
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
		// For each "select" node, match on its list of children.
		//if (musts || htab)
		//	RecmatchSelect(select, musts, htab, parse);
		//else
			{
			// Keeping the non-hashed way for now.
			// The RFA uses it at present.
			//RecmatchSelect(select, rules, parse);
//			matchNodes(root->Down(),0,rules,parse,0);
			HmatchNodes(root->Down(),0,parse,pass,0);
			}
		break;
	case SELECTNODES:
		// Traverse and find nodes matching the selected list.
		// Apply rules to the children of each.
		// (Start traversing at root.)
		//	if (musts || htab)
		//		RectraverseSelect(selects, musts, htab, parse, root);
		//	else
				{
				// Keeping the nonhashed method for now.
				// The RFA needs it, if noone else.
		//		RectraverseSelect(selects, rules, parse, root);
//				traverseSelect(arr_select, rules, parse, root);
				HtraverseSelect(parse,pass,root);
				}
		break;
	case SELECTPATH:
		//	if (musts || htab)
		//		RecpathSelect(selects->getFirst(), musts, htab, parse, root);
//		RecpathSelect(arr_select,rules,parse,root);
		HRecpathSelect(pass->arr_select,parse,pass,root);
		break;
	case SELECTMULTI:
		//	if (musts || htab)
		//		RecmultiSelect(selects, musts, htab, parse, root);
//		RecmultiSelect(arr_select,rules,parse,root);
		HRecmultiSelect(parse,pass,root);
		break;											// FIX.	// 08/04/03 AM.
	case SELECTPREMULTI:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Rec: Premulti select traversal unimplemented.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Rec: Bad select traversal type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
		break;
	}

Pat::prettyPass(root,tree,parse);
return true;
}


/********************************************
* FN:		HTRAVERSESELECT
* CR:		06/20/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to them.
* NOTE:	Recursive. Compiled runtime variant of RectraverseSelect.
*			If found a node, not traversing its children.
********************************************/

bool Rec::HtraverseSelect(
	Parse *parse,						// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect = pass->arr_select;
Pn *pn;
_TCHAR *name;
pn = node->getData();
name = pn->getName();
while (*pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << std::endl;
//		matchNodes(node->Down(),0,rules,parse,node);
		if (!HmatchNodes(node->Down(),0,parse,pass,node))					// 11/18/02 AM.
			return false;																// 11/18/02 AM.
		goto listmanager;
		}
	++pselect;
	}

// Try the rest of the tree.
// If node wasn't select and is UNSEALED, then
// look inside it.
if (pn->getUnsealed())
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
		{
		if (!HtraverseSelect(parse,pass,node))
			return false;
		}
	}
return true;
}


/********************************************
* FN:		MATCHNODES
* CR:		06/20/00 AM.
* SUBJ:	Match rules in a range of nodes.
* ALG:	Recursive pattern match algorithm:
*			Every time a rule matches, restart at start of range.
*			When no rules match, done.
* NOTE:	May want to check a threshold of reductions that do not
*			consume nodes.
*			Compiled runtime variant of RecmatchNodes.
********************************************/

bool Rec::HmatchNodes(
	Node<Pn> *start,			// Start node of range.
	Node<Pn> *end,				// End node of range, inclusive.
									// Null means move from start till end of list.
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *select
	)
{
if (!start || !pass->rules || !parse)
	return true;

//Delt<Irule> *prule;			// Current rules; also, the rule that matched.
Node<Pn> *node;
bool matched = false;		// If any rules matched.

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
//nlppp->node_	= node;					// Start at first node of phrase.
//resetRule(node, nlppp);				// Reset state of pattern matcher.
		// This is done before every rule.


// Tracking potential infinite loop.// RECINFLOOP	// 05/10/13 AM.
long state_start = 0;					// RECINFLOOP	// 05/10/13 AM.
long state_end = 0;						// RECINFLOOP	// 05/10/13 AM.
long state_count = 0;					// RECINFLOOP	// 05/10/13 AM.
long state_line = 0;						// RECINFLOOP	// 05/10/13 AM.

// Tracking a second state.			// RECINFLOOP	// 05/10/13 AM.
bool state_matched = false;	// If matched any state.	// RECINFLOOP	// 05/10/13 AM.
long state_toggle = 1;	// Point to 1 or 2 as next fill.	// RECINFLOOP	// 05/10/13 AM.
long state_start2 = 0;					// RECINFLOOP	// 05/10/13 AM.
long state_end2 = 0;						// RECINFLOOP	// 05/10/13 AM.
long state_count2 = 0;					// RECINFLOOP	// 05/10/13 AM.
long state_line2 = 0;					// RECINFLOOP	// 05/10/13 AM.

Pn *pn;	// RECINFLOOP	// 05/10/13 AM.

int *arr=0;						// Point to start of array. (For deleting).
node = nlppp->node_ = start;	// 04/27/06 AM.
//nlppp->maxspan_ = 0;	// RECOPT1	// 04/27/06 AM.
nlppp->rightmost_ = 0;		// RECOPT2	// 07/17/06 AM.
node->getData()->setRestart(0);			// RECOPT2.	// 07/21/06 AM.

for (;;)							// WHILE RECURSING....
	{
	matched = false;				// NO MATCHES IN THIS CYCLE YET.

	while (node)					// While matching on list of nodes.
		{
		// SETUP RULES LIST FOR CURRENT NODE HERE.
		// MERGE HASH TABLE AND MUSTS LISTS AS NEEDED.
		// This will be an array of rule numbers, null terminated.
//		arr = resetRules(nlppp->node_,must,htab);
		arr = (int *)Pat::resetRules(nlppp->node_,pass);

		if (!arr || !Pat::matchRules(nlppp, arr, pass->rules))
			{
			if (node == nlppp->rightmost_)	// RECOPT2.	// 07/23/06 AM.
				nlppp->rightmost_ = 0;		// RECOPT2.	// 07/23/06 AM.
			node = nlppp->node_ = Pat::nodeRight(node,end);
			if (arr)
				delete arr;
			if (nlppp->exitpass_)	// Aborting pass.					// 11/18/02 AM.
				return false;													// 11/18/02 AM.
			}
		else
			{
			if (arr)
				delete arr;

			if (nlppp->exitpass_)	// Aborting pass.					// 11/12/02 AM.
				return false;													// 11/12/02 AM.

			// Exec actions may remove the current node.	// RECINFLOOP	// 05/11/13 AM.
			if (nlppp->node_)						// Checking	// RECINFLOOP	// 05/11/13 AM.
				pn = nlppp->node_->getData();					// RECINFLOOP	// 05/11/13 AM.
			else if (node)									// RECINFL	// 05/11/13 AM.
				pn = node->getData();					// RECINFL	// 05/11/13 AM.
			else														// RECINFLOOP	// 05/11/13 AM.
				pn = 0;												// RECINFLOOP	// 05/11/13 AM.
			// Check for potential infinite loop.			// RECINFLOOP	// 05/11/13 AM.

			// If the rule changed current phrase.						// 08/08/02 AM.
			if (nlppp->excised_)												// 08/08/02 AM.
				{
				// Note: Current node is potentially mangled/deleted.
				nlppp->excised_ = false;									// 08/08/02 AM.
				nlppp->noop_ = false;										// 08/08/02 AM.
				}
			else if (nlppp->noop_)	// If rule was noop.
				{
				// Continue from END of noop match.						// 02/10/05 AM.
//				node = nlppp->node_ = Pat::nodeRight(node,end);
				node = nlppp->node_;								// FIX.	// 02/10/05 AM.
				if (nlppp->rmost_)							// RECOPT2.	// 07/23/06 AM.
					nlppp->rightmost_ = node;				// RECOPT2.	// 07/23/06 AM.
				nlppp->noop_ = false;	// Clear out.
				nlppp->sem_ = 0;		// Zero out.
				nlppp->dsem_ = 0;		// Zero out.
				continue;				// KEEP MATCHING.
				}
			else if (RecinfLoop(nlppp,pn,parse, /*DU*/	// RECINFLOOP	// 05/10/13 AM.
				state_start,state_end,state_count,state_line,
				state_matched,state_toggle,state_start2,
				state_end2,state_count2,state_line2))
				{
				delete (Tree<Pn> *) nlppp->collect_;	// RECINFLOOP	// 05/10/13 AM.
				//if (rules)									// RECINFLOOP	// 05/10/13 AM.
				//	Slist<Irule>::DeleteSlist(rules);	// RECINFLOOP	// 05/10/13 AM.
				nlppp->start_ = nlppp->select_->Down();// RECINFLOOP	// 05/10/13 AM.
				nlppp->end_ = 0;								// RECINFLOOP	// 05/10/13 AM.
				nlppp->sem_ = 0;		// Zero out.		// RECINFLOOP	// 05/10/13 AM.
				nlppp->dsem_ = 0;		// Zero out.		// RECINFLOOP	// 05/10/13 AM.
				nlppp->exitpass_ = true;					// RECINFLOOP	// 05/10/13 AM.
//				return parse->errOut(false);				// RECINFLOOP	// 05/10/13 AM.
				return false;									// RECINFLOOP	// 05/10/13 AM.
				}

			matched = true;	// A RULE MATCHED IN THIS CYCLE.
			// RESET START AND END OF PHRASE.
			start = nlppp->start_ = nlppp->select_->Down();
			end = nlppp->end_ = 0;
			nlppp->sem_ = 0;		// Zero out.
			nlppp->dsem_ = 0;		// Zero out.

			node = matchRestart(nlppp);	// RECOPT1	// 04/27/06 AM.
			break;				// RESTART AT BEGINNING OF PHRASE.
			}
		}
	if (!matched)
		break;						// NO NODES MATCHED, SO FINISHED.
	}
//delete (Tree<Pn> *) nlppp.collect_;
// nlppp.collect_ = 0;
return true;
}
