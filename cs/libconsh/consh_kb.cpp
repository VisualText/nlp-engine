/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CONSH_KB.C
*
* FILE:	consh.�/consh_kb.c
* SUBJ:	Consh-specific knowledge base handlers.
* CR:	11/28/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/26/99 AM.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>			// 03/07/00 AM.
#include <iostream>											// Upgrade.	// 01/30/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/list_s.h"
#include "prim/list.h"
#include "prim/unicu.h"

#include "kbm/libkbm.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/kbm_kb.h"			// 05/04/99 AM.
#include "kbm/dict.h"			// 05/04/99 AM.
#include "kbm/Akbm.h"

#include "ext/kb.h"

#include "consh/libconsh.h"		// 04/26/99 AM.
#include "consh/bind.h"
#include "consh/cc_var.h"
#include "consh/cg.h"
#include "consh/consh_kb.h"


/**************************************************
*						CON_ADD_ROOT
* FUN:	con_add_root
* SUBJ:	Add the hierarchical root concept to the con table.
* CR:	09/18/95 AM.
* EFF:	cg_CONCEPT - set to the root concept of the kb hierarchy.
* NOTE:	Special handling and assumptions for the root concept.
*		11/28/95 AM. Moved here from con.c. Consh only.
* ASS:	For Consh.  Don't do this in a Conan app.
*
**************************************************/

void
con_add_root(CG *cg)
{
CON *con;
SYM *sym;
bool dirt;	// 06/29/03 AM.

if (cg->cg_CONCEPT)
   {
   std::_t_cerr << _T("[con_add_root: Root concept of kb already exists.]") << std::endl;
   return;
   }

con = cg->acon_->con_alloc();
sym = cg->asym_->sym_get(CON_ROOT_NAME,dirt);

con->kind = cBASIC;
con->v.vname = sym;

cg->cg_CONCEPT = con->id;	/* Bind root concept to a global variable. */

// Since optimized to use concepts, need to get concepts defined when
// created.															// FIX.	// 08/25/02 AM.
cg->acon_->c_cg_CONCEPT = con;								// FIX.	// 08/25/02 AM.
}


/**************************************************
*						CON_PHRASE
* FUN:	con_phrase
* SUBJ:	Get the phrase associated with the given concept.
* CR:	11/29/95 AM.
* NOTE:	Consh and Conan to get different versions of this function.
*			04/23/99 AM. Because we're only going to use CONSH and not CONAN,
*			moving some things that would differ in both back into the
*			libraries.
*
**************************************************/

//CON *
//con_phrase(
//	CON *con
//	)
//{
//return(attr_get_c(con, c_cg_PHRASE));
//}




/**************************************************
*						PHR_ADD_NTH
* FUN:	phr_add_nth
* SUBJ:	Add concept to nth position of phrase.
* CR:		05/03/99 AM.
*	RET:	The new concept inserted into phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*
**************************************************/

LIBCONSH_API CON *
phr_add_nth(
	CON *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	_TCHAR *name,
	int pos,			// position to insert into.  if = 0, at end of list.
	CG *cg
	)
{
CON *phr;
CON *word;			// Concept for given name (or string).
//int res;

if (!owner || !name || !*name)
   return(CNULL);

if (!(phr = cg->acon_->con_phrase(owner)))
	return CNULL;

#ifdef OLD_030629_
if (!(word = cg->kbm_->dict_find_word(name))		/* Fetch con for given name. 	*/
	 && !(word = cg->kbm_->dict_add_word(name)) )
	return CNULL;
#endif

bool dirt;																		// 06/29/03 AM.
word = cg->kbm_->dict_get_word(name,dirt);							// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return CNULL;																// 06/29/03 AM.

/* Traverse to position of down list. */
bool first = false;
if (pos == 1)
	first = true;			// First position is special.

CON *cptr = 0;
cptr = phr;
if (pos == 0)				// Add to end of the list.
	{
	while (cptr->next)
		cptr = cptr->next;	// Traverse to last node.
	}
else								// Traverse to nth node.
	{
	while (--pos > 1)			// Fix.	// 05/04/99 AM.
		{
		cptr = cptr->next;	// Prior position.
		}
	}

CON *con;
con = cg->acon_->con_alloc();			/* Get empty concept struct.	*/

con->kind = cPROXY;
con->v.vcon = word;

// Attach to siblings, etc.
if (first)
	{
	// Unbind phrase from concept.
	cg->kbm_->attr_rm_by_name(phr, _T("atom"));
	cg->kbm_->attr_rm_by_name(owner, _T("phrase"));

	// Attach concept to start of phrase.
	con->next = phr;
	phr->prev = con;
	phr = con;

	// Rebind phrase to atomic concept (owner).
	PVAL pv;
	pv.vcon = phr;
	cg->kbm_->attr_add_val(owner, cg->acon_->c_cg_PHRASE, pv, pCON);
	pv.vcon = owner;
	cg->kbm_->attr_add_val(phr, cg->acon_->c_cg_ATOM, pv, pCON);
	}
else
	{
	// Insert.
	con->next = cptr->next;
	con->prev = cptr;
	if (cptr->next)
		cptr->next->prev = con;
	cptr->next = con;
	}
return(con);
}


/**************************************************
*						PHR_ADD_NTH
* FUN:	phr_add_nth
* SUBJ:	Add node to nth position of phrase. (Variant).
* CR:		05/05/99 AM.
*RET:	The new concept inserted into phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*
**************************************************/

LIBCONSH_API CON *
phr_add_nth(
	CON *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	CON *node,		// Node to attach.
	int pos,			// position to insert into.  if = 0, at end of list.
	CG *cg
	)
{
CON *phr;

if (!owner || !node)
   return(CNULL);

if (!(phr = cg->acon_->con_phrase(owner)))
	return CNULL;

if (node->kind != cPROXY)
	return CNULL;

/* Traverse to position of down list. */
bool first = false;
if (pos == 1)
	first = true;			// First position is special.

CON *cptr = 0;
cptr = phr;
if (pos == 0)				// Add to end of the list.
	{
	while (cptr->next)
		cptr = cptr->next;	// Traverse to last node.
	}
else								// Traverse to nth node.
	{
	while (--pos > 1)			// Fix.	// 05/04/99 AM.
		{
		cptr = cptr->next;	// Prior position.
		}
	}

CON *con;
con = node;

// Attach to siblings, etc.
if (first)
	{
	// Unbind phrase from concept.
	cg->kbm_->attr_rm_by_name(phr, _T("atom"));
	cg->kbm_->attr_rm_by_name(owner, _T("phrase"));

	// Attach concept to start of phrase.
	con->next = phr;
	phr->prev = con;
	phr = con;

	// Rebind phrase to atomic concept (owner).
	PVAL pv;
	pv.vcon = phr;
	cg->kbm_->attr_add_val(owner, cg->acon_->c_cg_PHRASE, pv, pCON);
	pv.vcon = owner;
	cg->kbm_->attr_add_val(phr, cg->acon_->c_cg_ATOM, pv, pCON);
	}
else
	{
	// Insert.
	con->next = cptr->next;
	con->prev = cptr;
	if (cptr->next)
		cptr->next->prev = con;
	cptr->next = con;
	}
return(con);
}


/**************************************************
*						INSERT_NODE
* FUN:	insert_node
* SUBJ:	Insert one node before a phrase node.
* CR:		05/05/99 AM.
* RET:	The node inserted into phrase.
* ARG:	n_pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*
**************************************************/

LIBCONSH_API CON *
insert_node(
	CON *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	CON *node,		// Node to attach.
	CON *n_pos,		// position to insert before.  if = 0, at end of list.
	CG *cg
	)
{
CON *phr;

if (!owner || !node)
   return(CNULL);

if (!(phr = cg->acon_->con_phrase(owner)))
	return CNULL;

if (node->kind != cPROXY)
	return CNULL;

if (n_pos && n_pos->kind != cPROXY)
	return CNULL;

/* Traverse to position of down list. */
bool first = false;
if (n_pos == phr)
	first = true;			// First position is special.

CON *cptr = 0;
cptr = phr;
if (n_pos == 0)				// Add to end of the list.
	{
	while (cptr->next)
		cptr = cptr->next;	// Traverse to last node.
	}
else								// Traverse to nth node.
	{
	cptr = n_pos->prev;
	}

CON *con;
con = node;

// Attach to siblings, etc.
if (first)
	{
	// Unbind phrase from concept.
	cg->kbm_->attr_rm_by_name(phr, _T("atom"));
	cg->kbm_->attr_rm_by_name(owner, _T("phrase"));

	// Attach concept to start of phrase.
	con->next = phr;
	phr->prev = con;
	phr = con;

	// Rebind phrase to atomic concept (owner).
	PVAL pv;
	pv.vcon = phr;
	cg->kbm_->attr_add_val(owner, cg->acon_->c_cg_PHRASE, pv, pCON);
	pv.vcon = owner;
	cg->kbm_->attr_add_val(phr, cg->acon_->c_cg_ATOM, pv, pCON);
	}
else
	{
	// Insert.
	con->next = cptr->next;
	con->prev = cptr;
	if (cptr->next)
		cptr->next->prev = con;
	cptr->next = con;
	}
return(con);
}


/**************************************************
*						RM_PROXY
* FUN:	rm_proxy
* SUBJ:	Remove proxy concept from phrase.
* CR:		05/04/99 AM.
* NOTE:	Rebind phrase if it's the first node in phrase.
*
**************************************************/

LIBCONSH_API bool
rm_proxy(
	CON *proxy,
	CG *cg
	)
{
if (!proxy)
	return false;

if (proxy->prev)		// Not the first node.
	{
	// Detach it.
	proxy->prev->next = proxy->next;
	if (proxy->next)
		proxy->next->prev = proxy->prev;
	proxy->prev = 0;
	proxy->next = 0;
	}
else						// First node in phrase.
	{
	CON *owner;
	CON *phr;			// Next node in phrase.
	if (!(owner = cg->kbm_->attr_get_c(proxy, cg->acon_->c_cg_ATOM)))
		return false;
	phr = proxy->next;

	// Unbind phrase.
	cg->kbm_->attr_rm_by_name(proxy, _T("atom"));
	cg->kbm_->attr_rm_by_name(owner, _T("phrase"));

	if (!phr)			// The last node was removed.
		return true;

	// Detach node.
	proxy->next = 0;
	phr->prev = 0;

	// Rebind phrase.
	PVAL pv;
	pv.vcon = phr;
	cg->kbm_->attr_add_val(owner, cg->acon_->c_cg_PHRASE, pv, pCON);
	pv.vcon = owner;
	cg->kbm_->attr_add_val(phr, cg->acon_->c_cg_ATOM, pv, pCON);
	}
return true;
}


/********************************************
* FN:		SORT_PHRASE
* CR:		05/08/99 AM.
* SUBJ:	Sort nodes in a phrase alphabetically by name.
********************************************/

bool sort_phrase(
	CON *con,			// The concept owning the phrase.
	CG *cg
	)
{
if (!con)
	return false;

CON *phr;
if (!(phr = cg->acon_->con_phrase(con)))
	return true;	// Empty phrase is ok.

// Detach phrase from concept.
if (!detach_phrase(con,cg))
	return false;

// Create an array.
long len = 0;
CON **arr;
len = cg->acon_->phrase_len(phr);
arr = new CON * [len];

// Make array elts point to list elts.
CON **parr = 0;
CON *pnode = 0;
for (pnode = phr, parr = arr;
	  pnode;
	  pnode = pnode->next, ++parr
	  )
	{
	*parr = pnode;
	}

// Sort array.
qsort((void *)arr, (size_t)len, sizeof(CON *), compare_nodes);

// Relink list.
parr = arr;
(*parr)->prev = 0;		// Fix first element's left ptr.
while (--len > 0)			// For all but last elt.
	{
	(*parr)->next = *(parr+1);
	(*(parr+1))->prev = *parr;
	++parr;
	}
// Fix up the last element.
(*parr)->next = 0;

// Attach sorted phrase.
attach_phrase(con, *arr,cg);

// Trash the array.
delete [] arr;
return true;
}



/********************************************
* FN:		SORT_CHILDS
* CR:		02/20/09 AM.
* SUBJ:	Sort concept's children alphabetically by name.
********************************************/

bool sort_childs(
	CON *con,			// The parent concept.
	CG *cg
	)
{
if (!con)
	return false;

CON *phr;
// if (!(phr = cg->acon_->con_phrase(con)))
if (!(phr = con->dn))
	return true;	// Empty phrase is ok.

// Detach children from concept.
con->dn = 0;
phr->up = 0;	// I believe only first element of list points up.

// Create an array.
long len = 0;
CON **arr;
len = cg->acon_->phrase_len(phr);
arr = new CON * [len];

// Make array elts point to list elts.
CON **parr = 0;
CON *pnode = 0;
for (pnode = phr, parr = arr;
	  pnode;
	  pnode = pnode->next, ++parr
	  )
	{
	*parr = pnode;
	}

// Sort array.
qsort((void *)arr, (size_t)len, sizeof(CON *), compare_nodes);

// Relink list.
parr = arr;
(*parr)->prev = 0;		// Fix first element's left ptr.
while (--len > 0)			// For all but last elt.
	{
	(*parr)->next = *(parr+1);
	(*(parr+1))->prev = *parr;
	++parr;
	}
// Fix up the last element.
(*parr)->next = 0;

// Attach sorted phrase.
con->dn = *arr;
(*arr)->up = con;

// Trash the array.
delete [] arr;
return true;
}


/********************************************
* FN:		COMPARE_NODES
* CR:		05/08/99 AM.
* SUBJ:	Compare nodes alphabetically.
* NOTE:	Used by qsort.
********************************************/

int compare_nodes( const void *arg1, const void *arg2 )
{
CON **a1, **a2;

a1 = (CON **) arg1;
a2 = (CON **) arg2;

// Get node's "real" concepts.
return unicu::strCmp(ACON::con_str(*a1),ACON::con_str(*a2));
/*
#ifndef LINUX
return _tcsicmp(ACON::con_str(*a1), ACON::con_str(*a2));
#else
return strcasecmp(ACON::con_str(*a1), ACON::con_str(*a2));	// 03/07/00 AM.
#endif
*/
}


/********************************************
* FN:		ATTACH_PHRASE
* CR:		05/08/99 AM.
* SUBJ:	Attach phrase to hierarchy concept.
* NOTE:	May want a version without so much checking.
			May want to return the attr.
********************************************/

LIBCONSH_API bool
attach_phrase(CON *con, CON *node,
	CG *cg)
{
if (!con || !node)
	return false;
if (cg->acon_->con_phrase(con))		// Concept already has phrase.
	return false;
if (cg->kbm_->attr_get(node, cg->acon_->c_cg_ATOM))	// Phrase is attached somewhere.
	return false;

PVAL pv;
pv.vcon = node;
cg->kbm_->attr_add_val((CON *)con, cg->acon_->c_cg_PHRASE, pv, pCON);
pv.vcon = (CON *)con;
cg->kbm_->attr_add_val(node, cg->acon_->c_cg_ATOM, pv, pCON);
return true;
}



/********************************************
* FN:		DETACH_PHRASE
* CR:		05/08/99 AM.
* SUBJ:	Detach phrase from hierarchy concept.
* RET:	Phrase.
* NOTE:	May want a version without so much checking.
			May want to return the attr.
********************************************/

LIBCONSH_API CON *
detach_phrase(CON *con, CG *cg)
{
if (!con)
	return CNULL;
CON *phr;
if (!(phr = cg->acon_->con_phrase(con)))
	return CNULL;
cg->kbm_->attr_rm_by_name(phr, _T("atom"));
cg->kbm_->attr_rm_by_name(con, _T("phrase"));
return phr;
}
