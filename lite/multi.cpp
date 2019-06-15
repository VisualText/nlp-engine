/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MULTI.CPP
* FILE:	c:\lite\multi.cpp
* CR:		03/25/99 AM.
* SUBJ:	PAT algorithm mode for matching on all phrases in a subtree.
* NOTE:	Functions belong in PAT class, placed here for modularity.
*			10/25/99 AM. Throwing PREMULTI stuff in here as well.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"															// 09/26/01 AM.
#include "io.h"
#include "string.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/24/06 AM.
//#include "irule.h"
#include "parse.h"
#include "lite/code.h"
#include "htab.h"	
//#include "ielt.h"	
#include "slist.h"
//#include "ifile.h"
#include "lite/nlppp.h"			// 07/24/06 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "ielt.h"	
#include "ifile.h"
#include "pat.h"
#include "rec.h"				// 11/08/99 AM.

#include "multi.h"



/********************************************
* FN:		MULTISELECT
* CR:		03/25/99 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			VARIANT USES HASHED RULES.
********************************************/

bool Pat::multiSelect(
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
bool flag = false;			// If current node is select.
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
		if (!multimatchSelect(node, musts, htab, parse,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
	{
	if (!multiSelect(selects,musts,htab,parse,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!multiSelect(selects,musts,htab,parse,node))
			return false;
		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		MULTIMATCHSELECT
* CR:		03/25/99 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
*			Ok, here's where we do the post-order traversal, matching in
*			"leaf phrases" first and working our way up the parse tree.
*			RECURSIVE.
********************************************/

bool Pat::multimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	bool froot					// If current node is the root of the subtree.
	)
{
// Traverse the given subtree and do the phrases under its nodes.

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
	if (!multimatchSelect(select->Down(), musts,htab,parse,false))
		return false;

	// Did children, now do me.
	//matchSelect(select, musts,htab,parse);	// OPT			// 04/27/00 AM.
	if (!matchNodes(select->Down(),0,musts,htab,parse,select))		// 04/27/00 AM.
		return false;														// 11/12/02 AM.
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
		if (!multimatchSelect(node->Down(),musts,htab,parse,false))
			return false;

		// Did children, now do me.
		//matchSelect(node, musts,htab,parse);	// OPT			// 04/27/00 AM.
		if (!matchNodes(node->Down(),0,musts,htab,parse,node))	// 04/27/00 AM.
			return false;													// 11/12/02 AM.

		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		PREMULTISELECT
* CR:		10/25/99 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			Preorder traversal, as opposed to the "plain" multi, which does
*			a postorder traversal.
********************************************/

bool Pat::premultiSelect(
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
bool flag = false;			// If current node is select.
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
		if (!premultimatchSelect(node, musts, htab, parse,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
	{
	if (!premultiSelect(selects,musts,htab,parse,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!premultiSelect(selects,musts,htab,parse,node))
			return false;
		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		PREMULTIMATCHSELECT
* CR:		10/25/99 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
*			Doing postorder traversal here.
*			RECURSIVE.
********************************************/

bool Pat::premultimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	bool froot					// If current node is the root of the subtree.
	)
{

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
	if (!premultimatchSelect(select->Down(), musts,htab,parse,false))
		return false;

	//matchSelect(select, musts,htab,parse);	// OPT			// 04/27/00 AM.
	if (!matchNodes(select->Down(),0,musts,htab,parse,select))	// 04/27/00 AM.
		return false;														// 11/12/02 AM.
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
		if (!premultimatchSelect(node->Down(),musts,htab,parse,false))
			return false;

		// Did children, now do me.
		//matchSelect(node, musts,htab,parse);	// OPT			// 04/27/00 AM.
		if (!matchNodes(node->Down(),0,musts,htab,parse,node))	// 04/27/00 AM.
			return false;													// 11/12/02 AM.
		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		RECMULTISELECT
* CR:		11/08/99 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			VARIANT USES HASHED RULES.
********************************************/

bool Rec::RecmultiSelect(
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
bool flag = false;			// If current node is select.
for (dselect = selects->getFirst(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	if (!strcmp_i(name, select->getStr()))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
		if (!RecmultimatchSelect(node, musts, htab, parse,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
	{
	if (!RecmultiSelect(selects,musts,htab,parse,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
		{
		if (!RecmultiSelect(selects,musts,htab,parse,node))
			return false;
		}
	}
return true;
}


/********************************************
* FN:		RECMULTIMATCHSELECT
* CR:		11/08/99 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
*			Ok, here's where we do the post-order traversal, matching in
*			"leaf phrases" first and working our way up the parse tree.
*			RECURSIVE.
********************************************/

bool Rec::RecmultimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,				// Current analysis instance.
	bool froot					// If current node is the root of the subtree.
	)
{
// Traverse the given subtree and do the phrases under its nodes.

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
	if (!RecmultimatchSelect(select->Down(), musts,htab,parse,false))
		return false;

	// Did children, now do me.
	if (!RecmatchSelect(select, musts,htab,parse))
		return false;
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
		if (!RecmultimatchSelect(node->Down(),musts,htab,parse,false))
			return false;

		// Did children, now do me.
		if (!RecmatchSelect(node, musts,htab,parse))
			return false;
		}
	}
return true;																	// 11/12/02 AM.
}

/******************************************************************************/
//				COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		MULTISELECT
* CR:		06/08/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			Compiled runtime variant.  Unhashed.
********************************************/

bool Pat::multiSelect(
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect;
Pn *pn = node->getData();
_TCHAR *name = pn->getName();
bool flag = false;			// If current node is select.
for (pselect = pass->arr_select; *pselect; ++pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
//		multimatchSelect(node, rules, parse);
		if (!multimatchSelect(node,parse,pass,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
//	multiSelect(arr_select,rules,parse,node->Down());
	{
	if (!multiSelect(parse,pass,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
//		multiSelect(pass->arr_select,rules,parse,node);
		{
		if (!multiSelect(parse,pass,node))
			return false;
		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		MULTIMATCHSELECT
* CR:		06/08/00 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	
*			Ok, here's where we do the post-order traversal, matching in
*			"leaf phrases" first and working our way up the parse tree.
*			RECURSIVE.
********************************************/

bool Pat::multimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	bool froot					// If current node is the root of the subtree.
	)
{
// Traverse the given subtree and do the phrases under its nodes.

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
//	multimatchSelect(select->Down(), rules,parse,false);
	if (!multimatchSelect(select->Down(),parse,pass,false))
		return false;

	// Did children, now do me.
//	matchNodes(select->Down(),0,rules,parse,select);
	if (!matchNodes(select->Down(),0,parse,pass,select))
		return false;														// 11/12/02 AM.
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
//		multimatchSelect(node->Down(),rules,parse,false);
		if (!multimatchSelect(node->Down(),parse,pass,false))
			return false;

		// Did children, now do me.
//		matchNodes(node->Down(),0,rules,parse,node);
		if (!matchNodes(node->Down(),0,parse,pass,node))
			return false;													// 11/12/02 AM.
		}
	}
return true;																// 11/12/02 AM.
}



/********************************************
* FN:		RECMULTISELECT
* CR:		06/08/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			Compiled runtime variant.  Unhashed.
********************************************/

bool Rec::RecmultiSelect(
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect = pass->arr_select;
Pn *pn = node->getData();
_TCHAR *name = pn->getName();
bool flag = false;			// If current node is select.
for (pselect = pass->arr_select; *pselect; ++pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
//		RecmultimatchSelect(node, rules, parse,
		if (!RecmultimatchSelect(node,parse,pass,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
//	RecmultiSelect(pass->arr_select,rules,parse,node->Down());
	{
	if (!RecmultiSelect(parse,pass,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
//		RecmultiSelect(pass->arr_select,rules,parse,node);
		{
		if (!RecmultiSelect(parse,pass,node))
			return false;
		}
	}
return true;
}


/********************************************
* FN:		RECMULTIMATCHSELECT
* CR:		06/08/00 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
*			Ok, here's where we do the post-order traversal, matching in
*			"leaf phrases" first and working our way up the parse tree.
*			RECURSIVE.
********************************************/

bool Rec::RecmultimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	bool froot					// If current node is the root of the subtree.
	)
{
// Traverse the given subtree and do the phrases under its nodes.

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
//	RecmultimatchSelect(select->Down(), rules,parse,false);
	if (!RecmultimatchSelect(select->Down(),parse,pass,false))
		return false;

	// Did children, now do me.
//	matchNodes(select->Down(), 0, rules, parse, select);
// I think this is a fix.													// 11/18/02 AM.
//	if (!matchNodes(select->Down(),0,parse,pass,select))			// 11/18/02 AM.
//	if (!Pat::matchNodes(select->Down(),0,parse,pass,select))	// 11/18/02 AM.
	if (!Rec::HmatchNodes(select->Down(),0,parse,pass,select))	// FIX. // 08/07/03 AM.
		return false;															// 11/12/02 AM.
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
//		RecmultimatchSelect(node->Down(),rules,parse,false);
		if (!RecmultimatchSelect(node->Down(),parse,pass,false))
			return false;

		// Did children, now do me.
//		matchNodes(node->Down(), 0, rules, parse, node);
//		if (!matchNodes(node->Down(),0,parse,pass,node))			// 11/18/02 AM.
//		if (!Pat::matchNodes(node->Down(),0,parse,pass,node))		// 11/18/02 AM.
		if (!Rec::HmatchNodes(node->Down(),0,parse,pass,node))	// FIX. // 08/07/03 AM.
			return false;														// 11/12/02 AM.
		}
	}
return true;																	// 11/12/02 AM.
}


/******************************************************************************/
//				HASHED COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		HMULTISELECT
* CR:		06/20/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			Compiled runtime variant.  Unhashed.
********************************************/

bool Pat::HmultiSelect(
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect;
Pn *pn = node->getData();
_TCHAR *name = pn->getName();
bool flag = false;			// If current node is select.
for (pselect = pass->arr_select; *pselect; ++pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
//		multimatchSelect(node, rules, parse,
		if (!HmultimatchSelect(node,parse,pass,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
//	multiSelect(pass->arr_select,rules,parse,node->Down());
	{
	if (!HmultiSelect(parse,pass,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
//		multiSelect(pass->arr_select,rules,parse,node);
		{
		if (!HmultiSelect(parse,pass,node))
			return false;
		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		MULTIMATCHSELECT
* CR:		06/20/00 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
*			Ok, here's where we do the post-order traversal, matching in
*			"leaf phrases" first and working our way up the parse tree.
*			RECURSIVE.
********************************************/

bool Pat::HmultimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	bool froot					// If current node is the root of the subtree.
	)
{
// Traverse the given subtree and do the phrases under its nodes.

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
//	multimatchSelect(select->Down(), rules,parse,false);
	if (!HmultimatchSelect(select->Down(),parse,pass,false))
		return false;

	// Did children, now do me.
//	matchNodes(select->Down(),0,rules,parse,select);
	if (!HmatchNodes(select->Down(),0,parse,pass,select))
		return false;															// 11/12/02 AM.
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
//		multimatchSelect(node->Down(),rules,parse,false);
		if (!HmultimatchSelect(node->Down(),parse,pass,false))
			return false;

		// Did children, now do me.
//		matchNodes(node->Down(),0,rules,parse,node);
		if (!HmatchNodes(node->Down(),0,parse,pass,node))
			return false;														// 11/12/02 AM.
		}
	}
return true;																// 11/12/02 AM.
}


/********************************************
* FN:		RECMULTISELECT
* CR:		06/20/00 AM.
* SUBJ:	Find select nodes and apply the pass rules to their subtree.
* NOTE:	Recursive.
*			If found a node, not traversing its children.
*			Compiled runtime variant.  Unhashed.
********************************************/

bool Rec::HRecmultiSelect(
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	Node<Pn> *node				// Current node of parse tree.
	)
{
if (!node)
	return true;
// If current node is select, match against its children.
const _TCHAR **pselect = pass->arr_select;
Pn *pn = node->getData();
_TCHAR *name = pn->getName();
bool flag = false;			// If current node is select.
for (pselect = pass->arr_select; *pselect; ++pselect)
	{
	if (!strcmp_i(name, *pselect))		// Found one.
		{
		//*gout << "[FOUND SELECT NODE]" << endl;
		flag = true;
//		RecmultimatchSelect(node, rules, parse,
		if (!HRecmultimatchSelect(node,parse,pass,
									true))			// Calling on root of subtree.
			return false;
		break;
		}
	}

// Try the rest of the tree.
if (!flag)		// If current node wasn't select, look within it.
//	RecmultiSelect(pass->arr_select,rules,parse,node->Down());
	{
	if (!HRecmultiSelect(parse,pass,node->Down()))
		return false;
	}

// If current node is first in list, let it manage the list.
// 12/10/98 AM. Removes part of recursion, reducing stack overflow problems.
if (!(node->Left()))
	{
	for (node = node->Right(); node; node = node->Right())
//		RecmultiSelect(pass->arr_select,rules,parse,node);
		{
		if (!HRecmultiSelect(parse,pass,node))
			return false;
		}
	}
return true;
}


/********************************************
* FN:		RECMULTIMATCHSELECT
* CR:		06/20/00 AM.
* SUBJ:	Match rules in all the phrases of the selected subtree.
* NOTE:	VARIANT USES HASHED RULES.
*			Ok, here's where we do the post-order traversal, matching in
*			"leaf phrases" first and working our way up the parse tree.
*			RECURSIVE.
********************************************/

bool Rec::HRecmultimatchSelect(
	Node<Pn> *select,			// Select, or chosen tree node.
	Parse *parse,				// Current analysis instance.
	const PASS *pass,					// Static pass data.				// 06/21/00 AM.
	bool froot					// If current node is the root of the subtree.
	)
{
// Traverse the given subtree and do the phrases under its nodes.

if (!select)
	return true;

// If current is root, then do its children.
// If not leftmost in a list, do its children.
Node<Pn> *node;
if (froot || select->Left())
	{
//	RecmultimatchSelect(select->Down(), rules,parse,false);
	if (!HRecmultimatchSelect(select->Down(),parse,pass,false))
		return false;

	// Did children, now do me.
//	matchNodes(select->Down(), 0, rules, parse, select);
//	if (!HmatchNodes(select->Down(),0,parse,pass,select))			// 11/18/02 AM.
	if (!Rec::HmatchNodes(select->Down(),0,parse,pass,select))	// 11/18/02 AM.
		return false;															// 11/12/02 AM.
	}
else							// Non-root at head of list.
	{
	// Manage the list to keep recursion down to a dull roar.
	for (node = select; node; node = node->Right())
		{
//		RecmultimatchSelect(node->Down(),rules,parse,false);
		if (!HRecmultimatchSelect(node->Down(),parse,pass,false))
			return false;

		// Did children, now do me.
//		matchNodes(node->Down(), 0, rules, parse, node);
//		if (!HmatchNodes(node->Down(),0,parse,pass,node))			// 11/18/02 AM.
		if (!Rec::HmatchNodes(node->Down(),0,parse,pass,node))	// 11/18/02 AM.
			return false;														// 11/12/02 AM.
		}
	}
return true;																	// 11/12/02 AM.
}



