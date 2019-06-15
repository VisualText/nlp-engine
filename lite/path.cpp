/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PATH.CPP
* FILE:	c:\lite\path.cpp
* CR:		10/19/99 AM.
* SUBJ:	PAT algorithm mode for matching on all phrases in a subtree.
* NOTE:	Functions belong in PAT class, placed here for modularity.
*			
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.
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

#include "path.h"
#include "rec.h"				// 11/08/99 AM.



/********************************************
* FN:		PATHSELECT
* CR:		10/19/99 AM.
* SUBJ:	Find select node path and apply the pass rules to its phrase.
* NOTE:	Recursive.
*			USES HASHED RULES.
* ASS:	Each path should start with "_ROOT".
*			Called with first node in phrase.
********************************************/

bool Pat::pathSelect(
	Delt<Iarg> *selects,				// Names of select node path.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,						// Current analysis instance.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node || !selects)
	return true;

Iarg *select = selects->getData();
_TCHAR *pathname = select->getStr();

Delt<Iarg> *nextsel = selects->Right();	// Next part of path, if any.

Pn *pn;
_TCHAR *name;

// Traverse nodes of phrase, looking for nodes that match path component.
for (; node; node = node->Right())
	{
	pn = node->getData();
	name = pn->getName();
	if (!_tcscmp(name, pathname))		// Node is part of path.
		{
		if (nextsel)						// If next path component, get it.
			{
			if (!pathSelect(nextsel,musts,htab,parse,node->Down()))
				return false;													// 11/19/02 AM,
			}
		else									// Else, end of path, match on phrase.
			//matchSelect(node, musts, htab, parse);
			{
			if (!matchNodes(node->Down(),0,musts,htab,parse,node))// 11/18/02 AM.
				return false;													// 11/18/02 AM.
			}
		}
	}
return true;																	// 11/12/02 AM.
}



/********************************************
* FN:		RECPATHSELECT
* CR:		11/08/99 AM.
* SUBJ:	Find select node path and apply the pass rules to its phrase.
* NOTE:	Recursive.
*			USES HASHED RULES.
* ASS:	Each path should start with "_ROOT".
*			Called with first node in phrase.
********************************************/

bool Rec::RecpathSelect(
	Delt<Iarg> *selects,				// Names of select node path.
	Selt<Irule> *musts,				// Rules that must be tried.
	tHtab<Slist<Irule> > *htab,	// Hashed rule table.
	Parse *parse,						// Current analysis instance.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node || !selects)
	return true;

Iarg *select = selects->getData();
_TCHAR *pathname = select->getStr();

Delt<Iarg> *nextsel = selects->Right();	// Next part of path, if any.

Pn *pn;
_TCHAR *name;

// Traverse nodes of phrase, looking for nodes that match path component.
for (; node; node = node->Right())
	{
	pn = node->getData();
	name = pn->getName();
	if (!_tcscmp(name, pathname))		// Node is part of path.
		{
		if (nextsel)						// If next path component, get it.
			{
			if (!RecpathSelect(nextsel,musts,htab,parse,node->Down()))
				return false;
			}
		else									// Else, end of path, match on phrase.
			{
			if (!RecmatchSelect(node, musts, htab, parse))
				return false;
			}
		}
	}
return true;
}

/******************************************************************************/
//				COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		PATHSELECT
* CR:		06/08/00 AM.
* SUBJ:	Find select node path and apply the pass rules to its phrase.
* NOTE:	Recursive.
*			Compiled runtime variant.  Without hashing.
* OPT:	Should implement this type of stuff iteratively.
********************************************/

bool Pat::pathSelect(
	const _TCHAR *arr_select[],		// Names of select nodes.
	Parse *parse,						// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node)
	return true;

const _TCHAR **pselect = arr_select;
const _TCHAR *pathname = *pselect;

const _TCHAR **nextsel = pselect+1; // Next part of path, if any.

Pn *pn;
_TCHAR *name;

// Traverse nodes of phrase, looking for nodes that match path component.
for (; node; node = node->Right())
	{
	pn = node->getData();
	name = pn->getName();
	if (!_tcscmp(name, pathname))		// Node is part of path.
		{
		if (*nextsel)						// If next path component, get it.
//			pathSelect(nextsel,rules,parse,node->Down());
			{
			if (!pathSelect(nextsel,parse,pass,node->Down()))
				return false;
			}
		else									// Else, end of path, match on phrase.
			//matchSelect(node, musts, htab, parse);
//			matchNodes(node->Down(),0,rules,parse,node);
			{
			if (!matchNodes(node->Down(),0,parse,pass,node))	// 11/18/02 AM.
				return false;												// 11/18/02 AM.
			}
		}
	}
return true;																	// 11/12/02 AM.
}



/********************************************
* FN:		RECPATHSELECT
* CR:		11/08/99 AM.
* SUBJ:	Find select node path and apply the pass rules to its phrase.
* NOTE:	Recursive.
*			USES HASHED RULES.
* ASS:	Each path should start with "_ROOT".
*			Called with first node in phrase.
********************************************/

bool Rec::RecpathSelect(
	const _TCHAR *arr_select[],		// Names of select nodes.
	Parse *parse,								// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *node								// Current node of parse tree.
	)
{
if (!node)
	return true;

const _TCHAR **pselect = arr_select;
const _TCHAR *pathname = *pselect;

const _TCHAR **nextsel = pselect+1; // Next part of path, if any.

Pn *pn;
_TCHAR *name;

// Traverse nodes of phrase, looking for nodes that match path component.
for (; node; node = node->Right())
	{
	pn = node->getData();
	name = pn->getName();
	if (!_tcscmp(name, pathname))		// Node is part of path.
		{
		if (*nextsel)						// If next path component, get it.
//			RecpathSelect(nextsel,rules,parse,node->Down());
			{
			if (!RecpathSelect(nextsel,parse,pass,node->Down()))
				return false;
			}
		else									// Else, end of path, match on phrase.
//			matchNodes(node->Down(),0,rules,parse,node);
			{
			if (!matchNodes(node->Down(),0,parse,pass,node))	// 11/18/02 AM.
				return false;												// 11/18/02 AM.
			}
		}
	}
return true;
}



/********************************************
* FN:		PATHSELECT
* CR:		06/20/00 AM.
* SUBJ:	Find select node path and apply the pass rules to its phrase.
* NOTE:	Recursive.
*			Compiled runtime variant.  HASHED.
* OPT:	Should implement this type of stuff iteratively.
********************************************/

bool Pat::HpathSelect(
	const _TCHAR *arr_select[],		// Names of select nodes.
	Parse *parse,						// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *node						// Current node of parse tree.
	)
{
if (!node)
	return true;

const _TCHAR **pselect = arr_select;
const _TCHAR *pathname = *pselect;

const _TCHAR **nextsel = pselect+1; // Next part of path, if any.

Pn *pn;
_TCHAR *name;

// Traverse nodes of phrase, looking for nodes that match path component.
for (; node; node = node->Right())
	{
	pn = node->getData();
	name = pn->getName();
	if (!_tcscmp(name, pathname))		// Node is part of path.
		{
		if (*nextsel)						// If next path component, get it.
//			pathSelect(nextsel,rules,parse,node->Down());
			{
			if (!HpathSelect(nextsel,parse,pass,node->Down()))
				return false;
			}
		else									// Else, end of path, match on phrase.
			//matchSelect(node, musts, htab, parse);
//			matchNodes(node->Down(),0,rules,parse,node);
			{
			if (!HmatchNodes(node->Down(),0,parse,pass,node))		// 11/18/02 AM.
				return false;													// 11/18/02 AM.
			}
		}
	}
return true;																	// 11/12/02 AM.
}



/********************************************
* FN:		RECPATHSELECT
* CR:		06/20/00 AM.
* SUBJ:	Find select node path and apply the pass rules to its phrase.
* NOTE:	Recursive.
*			HASHED.
* ASS:	Each path should start with "_ROOT".
*			Called with first node in phrase.
********************************************/

bool Rec::HRecpathSelect(
	const _TCHAR *arr_select[],		// Names of select nodes.
	Parse *parse,								// Current analysis instance.
	const PASS *pass,					// Static pass data.						// 06/21/00 AM.
	Node<Pn> *node								// Current node of parse tree.
	)
{
if (!node)
	return true;

const _TCHAR **pselect = arr_select;
const _TCHAR *pathname = *pselect;

const _TCHAR **nextsel = pselect+1; // Next part of path, if any.

Pn *pn;
_TCHAR *name;

// Traverse nodes of phrase, looking for nodes that match path component.
for (; node; node = node->Right())
	{
	pn = node->getData();
	name = pn->getName();
	if (!_tcscmp(name, pathname))		// Node is part of path.
		{
		if (*nextsel)						// If next path component, get it.
//			RecpathSelect(nextsel,rules,parse,node->Down());
			{
			if (!HRecpathSelect(nextsel,parse,pass,node->Down()))
				return false;
			}
		else									// Else, end of path, match on phrase.
//			matchNodes(node->Down(),0,rules,parse,node);
			{
			if (!HmatchNodes(node->Down(),0,parse,pass,node))	// 11/18/02 AM.
				return false;												// 11/18/02 AM.
			}
		}
	}
return true;
}
