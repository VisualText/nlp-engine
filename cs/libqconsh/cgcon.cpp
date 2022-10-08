/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CGCON.CPP
* FILE:	cs/libqconsh/cgcon.cpp
* CR:		02/19/07 AM.
* SUBJ:	Class for managing CG KBMS concepts.
* NOTE:	Overhauls for QDBM.
*			Moving many of the con_.cpp functions from libkbm to here.
*			libkbm/con_.cpp no longer present.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include "machine-min.h"
#include "prim/libprim.h"

#include "StdAfx.h"
#include <time.h>
#include "machine-min.h"
#include "prim/libprim.h"
#include "prim/str.h"
#include "prim/unicu.h"

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xcon_s.h"
#include "qkbm/xptr_s.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "cg_global.h"
#include "qconsh/cg.h"

#include "attr.h"
#include "cgdict.h"

#include "cgcon.h"

int compare_nodes(const void *arg1, const void *arg2);


int CGCON::count_ = 0;



/********************************************
* FN:		Special functions for the class
********************************************/

CGCON::CGCON(
	CG *cg	// Want the kbms available to attr manager.
	)
{
cg_ = cg;

// Opt: Set up tracking of recently used concept paths.	// 03/08/07 AM.
// Note: ZERO-BASED COUNTING.
lpath_ = lpath2_ = -1;		// No depth in hierarchy yet.	// 03/08/07 AM.


#ifndef STABLE_
++count_;
#endif
}

CGCON::~CGCON()
{

// Opt: Clear tracking of concept paths.	// 03/08/07 AM.
int ii;												// 03/08/07 AM.
XCON_S **ptr = path_;							// 03/08/07 AM.
_TCHAR **sptr = spath_;							// 03/08/07 AM.
for (ii = 0; ii <= lpath_; ++ii)				// 03/08/07 AM.
	{
	cg_->kbfree(*ptr++);							// 03/08/07 AM.
	cg_->kbfree(*sptr++);						// 03/08/07 AM.
	}

ptr = path2_;										// 03/08/07 AM.
sptr = spath2_;									// 03/08/07 AM.
for (ii = 0; ii <= lpath2_; ++ii)			// 03/08/07 AM.
	{
	cg_->kbfree(*ptr++);							// 03/08/07 AM.
	cg_->kbfree(*sptr++);						// 03/08/07 AM.
	}
lpath_ = lpath2_ = -1;							// 03/08/07 AM.

#ifndef STABLE_
--count_;
#endif
}




/**************************************************
*						CON_ADD_ALPHA
* FUN:	con_add_alpha
* SUBJ:	Insert given name into alphabetically ordered hierarchy phrase.
* CR:	10/24/95 AM.
* ASS:	Assumes alphabetic phrase.
*		Assumes redundant names not allowed.
* RET:	The new concept inserted into hierarchy phrase.
* NOTE:	May be the first concept in phrase.
*		Adding as a WORD concept.
*
**************************************************/

CON_ID CGCON::con_add_alpha(
	XCON_S *hier,
	_TCHAR *name
	)
{
CON_ID cid;	// New concept id.
XCON_S *pre;
_TCHAR *str;	// 03/05/07 AM.
//int res;

if (!hier || str_empty(name))
   return 0;

// Get string for concept's name.
bool dirty = false;
STR_ID sid = cg_->qkbm_->sym_get(name, dirty);
if (!sid)
	return 0;

cid = con_child_alpha(hier, name, /*UP*/ &pre);
if (cid)
   {
   str = cg_->qkbm_->con_str(hier);	// 03/05/07 AM.
   _ftprintf(stderr, _T("[con_add_alpha: hier=%s already has child=%s]\n"),
   		str, name);
   cg_->kbfree(str);	// 03/05/07 AM.
//   return 0;	// 07/21/03 AM.
	if (pre)
		cg_->kbfree(pre);
	return cid;	// RECOVER.	// 07/21/03 AM.
   }

// Make new word-concept. Update xsym, xcon.
// Insert in phrase alphabetically.
XCON_S *fst;																		// 11/02/00 AM.
if (!pre)		// Concept goes first in hier's children.
   {
	cid = cg_->qkbm_->addXcon(
				sid,			// concept val.
				cXWORD,		// kind
				0,				// flags
				0,				// attrs
				hier->id,	// up
				0,				// dn
				0,				// prev
				hier->dn,	// next
				0,				// atom
				0				// phrases
				);
	if (!cid)
		return 0;
	cg_->qkbm_->addXsym(sid,cid);	// Bind string to dict concept.

   if (hier->dn)
		{
		fst = cg_->qkbm_->Con(hier->dn);
		if (!fst)
			return 0;	// Error.
      fst->prev = cid;														// 11/02/00 AM.
		// Every concept now points to its parent.					// 02/19/07 AM.
//		fst->up = 0;											// FIX.	// 11/02/00 AM.
		cg_->qkbm_->con_update(fst->id, fst);
		cg_->kbfree(fst);
		}
   hier->dn = cid;
   cg_->qkbm_->con_update(hier->id, hier);
   }
else	/* Insert concept alphabetically. */
   {
	cid = cg_->qkbm_->addXcon(
				sid,			// concept val.
				cXWORD,		// kind
				0,				// flags
				0,				// attrs
				hier->id,	// up
				0,				// dn
				pre->id,		// prev
				pre->next,	// next
				0,				// atom
				0				// phrases
				);
	if (!cid)
		return 0;
	cg_->qkbm_->addXsym(sid,cid);	// Bind string to dict concept.

   if (pre->next)
		{
		XCON_S *cnext = cg_->qkbm_->Con(pre->next);
      cnext->prev = cid;
      cg_->qkbm_->con_update(cnext->id, cnext);
      cg_->kbfree(cnext);
      }
   pre->next = cid;
   cg_->qkbm_->con_update(pre->id, pre);
   cg_->kbfree(pre);
//	con->up = 0;							// 05/13/99 AM.
   }
return cid;
}


/**************************************************
*						CON_ADD_BASIC
* FUN:	con_add_basic
* SUBJ:	Add a basic concept to the con table.
* CR:	09/15/95 AM.
* RET:	con structure, if successful.
* ASS:	Not allowing multiple concept hierarchies.  Root of
*		hierarchy must be added in a separate place, for
*		efficiency (maybe a con_add_root fun).
* NOTE:	Adds concept to END of hierarchic children.
*
**************************************************/

CON_ID CGCON::con_add_basic(
	_TCHAR *name,				/* Descriptive name for new concept.	*/
	XCON_S *up					/* Hierarchical parent of new concept.	*/
	)
{
CON_ID cid;

if (!up)
   {
   _ftprintf(stderr, _T("[con_add_basic: No hierachical parent.]\n"));
   return 0;
   }
if (str_empty(name))
   {
   _ftprintf(stderr, _T("[con_add_basic: No name given.]\n"));
   return 0;
   }

// Get string for concept's name.
bool dirty = false;
STR_ID sid = cg_->qkbm_->sym_get(name, dirty);
if (!sid)
	return 0;

/* Add new concept to its hierarchic parent's hierarchy. */
if (!up->dn)
	{
	cid = cg_->qkbm_->addXcon(
				sid,			// concept val.
				cXBASIC,		// kind
				0,				// flags
				0,				// attrs
				up->id,		// up
				0,				// dn
				0,				// prev
				0,				// next
				0,				// atom
				0				// phrases
				);
	if (!cid)
		return 0;

   up->dn = cid;
	cg_->qkbm_->con_update(up->id, up);
	}
else
   {
   // Traverse to end of down list.
   CON_ID ccid = up->dn;
   XCON_S *cptr = cg_->qkbm_->Con(ccid);
   if (!cptr)	// 03/22/07 AM.
		{
		// KB Consistency error.
		return 0;	// 03/22/07 AM.
		}
   while (cptr->next)
		{
      ccid = cptr->next;
      cg_->kbfree(cptr);
      if (!(cptr = cg_->qkbm_->Con(ccid)))	// FIX.	// 02/27/07 AM.
			return 0;								// DB ERROR.	// 03/22/07 AM.
      }
   // Now cptr is last concept in list of siblings.
   if (!cptr)
		return 0;

	cid = cg_->qkbm_->addXcon(
				sid,			// concept val.
				cXBASIC,		// kind
				0,				// flags
				0,				// attrs
				up->id,		// up
				0,				// dn
				cptr->id,	// prev
				0,				// next
				0,				// atom
				0				// phrases
				);
	if (!cid)
		{
		cg_->kbfree(cptr);
		return 0;
		}

   cptr->next = cid;
   cg_->qkbm_->con_update(cptr->id, cptr);
   cg_->kbfree(cptr);
   }

return cid;
}

/**************************************************
*						CON_ADD_FIRST
* FUN:	con_add_first
* SUBJ:	Add a basic concept as first child.
* CR:		05/08/99 AM.
* WARN:	con should be detached from hierarchy.  Caller must ensure this.
*
**************************************************/

bool CGCON::con_add_first(XCON_S *con, XCON_S *hier)
{
if (!con || !hier)
	return false;

// Point to hierarchic parent.
con->up = hier->id;

con->next = hier->dn;	// Point to first, if any.
cg_->qkbm_->con_update(con->id, con);

if (hier->dn)
	{
	XCON_S *fst = cg_->qkbm_->Con(hier->dn);
	if (!fst)
		return false;	// Error.
	fst->prev = con->id;
	cg_->qkbm_->con_update(fst->id, fst);
	cg_->kbfree(fst);
	}

hier->dn = con->id;
cg_->qkbm_->con_update(hier->id, hier);
return true;
}

/**************************************************
*						CON_ADD_NH
* FUN:	con_add_nh
* SUBJ:	Add a basic concept to the con table, no hierarchy attachment.
* CR:	09/15/95 AM.
* RET:	con structure, if successful.
* ASS:	Not allowing multiple concept hierarchies.  Root of
*		hierarchy must be added in a separate place, for
*		efficiency (maybe a con_add_root fun).
*
**************************************************/

CON_ID CGCON::con_add_nh(
	_TCHAR *name				/* Descriptive name for new concept.	*/
	)
{
if (str_empty(name))
	return 0;

// Get string for concept's name.
bool dirty = false;
STR_ID sid = cg_->qkbm_->sym_get(name, dirty);
if (!sid)
	return 0;

CON_ID cid = cg_->qkbm_->addXcon(
			sid,			// concept val.
			cXBASIC,		// kind
			0,				// flags
			0,				// attrs
			0,				// up
			0,				// dn
			0,				// prev
			0,				// next
			0,				// atom
			0				// phrases
			);
return cid;
}


/**************************************************
*						CON_ADD_INTERNAL
* FUN:	con_add_internal
* SUBJ:	Add a basic concept to the con table, with given attachments.
* CR:		02/26/07 AM.
* RET:	concept id, if successful.
*
**************************************************/

CON_ID CGCON::con_add_internal(
	_TCHAR *name,				// Descriptive name for new concept.
	PTR_ID attrs,
	CON_ID up,
	CON_ID dn,
	CON_ID prev,
	CON_ID next,
	CON_ID phrases
	)
{
if (str_empty(name))
	return 0;

// Get string for concept's name.
bool dirty = false;
STR_ID sid = cg_->qkbm_->sym_get(name, dirty);
if (!sid)
	return 0;

CON_ID cid = cg_->qkbm_->addXcon(
			sid,			// concept val.
			cXBASIC,		// kind
			0,				// flags
			attrs,		// attrs
			up,			// up
			dn,			// dn
			prev,			// prev
			next,			// next
			0,				// atom
			phrases		// phrases
			);
return cid;
}


/**************************************************
*						CON_ADD_NTH
* FUN:	con_add_nth
* SUBJ:	Add concept to nth position among children.
* CR:		05/03/99 AM.
*	RET:	The new concept inserted into hierarchy phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*
**************************************************/

CON_ID CGCON::con_add_nth(
	XCON_S *hier,
	_TCHAR *name,
	int pos			// position to insert into.  if = 0, at end of list.
	)
{
CON_ID cid;

if (pos == 0)			// Add to end of the list.
	return con_add_basic(name, hier);

if (!hier || str_empty(name))
   return 0;

// Get string for concept's name.
bool dirty = false;
STR_ID sid;

/* Traverse to position of down list. */
bool first = false;
if (pos == 1)
	first = true;			// First position is special.

CON_ID iptr;
XCON_S *cptr;
if (!(iptr = hier->dn))
	{
	if (pos != 1 &&															// 07/01/99 AM.
		 pos != 0)													// FIX.	// 11/02/00 AM.
		return 0;															// 07/01/99 AM.

	if (!(sid = cg_->qkbm_->sym_get(name, dirty)))
		return 0;

	// Add it as a child of hier.
	cid = cg_->qkbm_->addXcon(
			sid,			// concept val.
			cXBASIC,		// kind
			0,				// flags
			0,				// attrs
			hier->id,	// up
			0,				// dn
			0,				// prev
			0,				// next
			0,				// atom
			0				// phrases
			);
	if (!cid)
		return 0;
	hier->dn = cid;															// 07/01/99 AM.
	cg_->qkbm_->con_update(hier->id, hier);
	return cid;																	// 07/01/99 AM.
	}

while (--pos > 1)
	{
	if (!iptr)
		return 0;
	if (!(cptr = cg_->qkbm_->Con(iptr)))
		return 0;
	iptr = cptr->next;	// Prior position.
	cg_->kbfree(cptr);
	}
if (!iptr)
	return 0;
if (!(cptr = cg_->qkbm_->Con(iptr)))
	return 0;

if (!(sid = cg_->qkbm_->sym_get(name, dirty)))
	return 0;

// Attach to siblings, etc.
if (first)
	{
	cg_->kbfree(cptr);	// 03/05/07 AM.
	cid = cg_->qkbm_->addXcon(
		sid,			// concept val.
		cXBASIC,		// kind
		0,				// flags
		0,				// attrs
		hier->id,	// up
		0,				// dn
		0,				// prev
		hier->dn,	// next
		0,				// atom
		0				// phrases
		);
	if (!cid)
		{
		return 0;
		}
	if (!hier->dn)
		return cid;

	// Opt: Appears to be re-fetching cptr.	// 03/05/07 AM.
	XCON_S *nxt = cg_->qkbm_->Con(hier->dn);
	if (!nxt)
		{
		return 0;	// Error.
		}
	hier->dn = cid;
	cg_->qkbm_->con_update(hier->id, hier);

	if (nxt)
		{
		nxt->prev = cid;
		cg_->qkbm_->con_update(nxt->id, nxt);
		cg_->kbfree(nxt);	// 03/05/07 AM.
		}
	}
else
	{
	// Insert.
	cid = cg_->qkbm_->addXcon(
		sid,			// concept val.
		cXBASIC,		// kind
		0,				// flags
		0,				// attrs
		hier->id,	// up
		0,				// dn
		cptr->id,	// prev
		cptr->next,	// next
		0,				// atom
		0				// phrases
		);
	if (!cid)
		{
		cg_->kbfree(cptr);
		return 0;
		}

	if (cptr->next)
		{
		XCON_S *nxt = cg_->qkbm_->Con(cptr->next);
		nxt->prev = cid;
		cg_->qkbm_->con_update(nxt->id, nxt);
		cg_->kbfree(nxt);
		}

	cptr->next = cid;
	cg_->qkbm_->con_update(cptr->id, cptr);
	cg_->kbfree(cptr);
	}
return cid;
}


/**************************************************
*						CON_ADD_NTH
* FUN:	con_add_nth
* SUBJ:	Add concept to nth position among children.
* CR:		05/13/99 AM.
* RET:	The new concept inserted into hierarchy phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	VARIANT.  Takes an existing detached concept.
*
**************************************************/

CON_ID CGCON::con_add_nth(
	XCON_S *hier,
	XCON_S *con,
	int pos			// position to insert into.  if = 0, at end of list.
	)
{

if (!hier || !con)
   return 0;
if (con->up || con->prev || con->next || con->dn)
	return 0;

CON_ID cid = con->id;
CON_ID iptr = hier->dn;
if (!iptr)
	{
	if (pos != 0 && pos != 1)		// 07/01/99 AM.
		return 0;					// 07/01/99 AM.
	// Add it as a child of hier.
	con->up = hier->id;
	cg_->qkbm_->con_update(cid, con);

	hier->dn = cid;															// 07/01/99 AM.
	cg_->qkbm_->con_update(hier->id, hier);
	return cid;																	// 07/01/99 AM.
	}

/* Traverse to position of down list. */
bool first = false;

XCON_S *cptr;
if (!(cptr = cg_->qkbm_->Con(iptr)))
	return 0;


if (pos == 0)			// Add to end of the list.
	{
	// Get last concept of list.
	while (iptr = cptr->next)
		{
		cg_->kbfree(cptr);
		if (!(cptr = cg_->qkbm_->Con(iptr)))
			return 0;
		}
		;
	}
else if (pos == 1)
	first = true;			// First position is special.
else
	{
	while (--pos > 1)
		{
		if (!(iptr = cptr->next))	// Prior position.
			return 0;
		cg_->kbfree(cptr);
		if (!(cptr = cg_->qkbm_->Con(iptr)))
			return 0;
		}
	}
if (!cptr)
	return 0;

// Attach to siblings, etc.
if (first)
	{
	cg_->kbfree(cptr);	// 03/05/07 AM.
	con->up = hier->id;
	con->next = hier->dn;
	cg_->qkbm_->con_update(cid, con);

	if (!hier->dn)
		return cid;

	// Opt: Appears to be re-fetching cptr.	// 03/05/07 AM.
	XCON_S *nxt = cg_->qkbm_->Con(hier->dn);
	if (!nxt)
		{
		return 0;	// Error.
		}
	hier->dn = cid;
	cg_->qkbm_->con_update(hier->id, hier);

	if (nxt)
		{
		nxt->prev = cid;
		cg_->qkbm_->con_update(nxt->id, nxt);
		}
	}
else
	{
	// Insert.
	con->up = hier->id;
	con->prev = cptr->id;
	con->next = cptr->next;
	cg_->qkbm_->con_update(cid, con);

	if (cptr->next)
		{
		XCON_S *nxt = cg_->qkbm_->Con(cptr->next);
		nxt->prev = cid;
		cg_->qkbm_->con_update(nxt->id, nxt);
		cg_->kbfree(nxt);
		}

	cptr->next = cid;
	cg_->qkbm_->con_update(cptr->id, cptr);
	cg_->kbfree(cptr);
	}
return cid;
}


/**************************************************
*						CON_ADD_PROXY
* FUN:	con_add_proxy
* SUBJ:	Add a proxy concept to the con table, no hierarchy attachment.
* CR:	10/19/95 AM.
* RET:	con structure, if successful.
*
**************************************************/

CON_ID CGCON::con_add_proxy(
	CON_ID gen_cid				/* The concept that proxy represents. */
	)
{
if (!gen_cid)
   return 0;

// Todo:	Could verify that the target concept exists.

CON_ID cid = cg_->qkbm_->addXcon(
			gen_cid,			// concept val.
			cXPROXY,		// kind
			0,				// flags
			0,				// attrs
			0,				// up
			0,				// dn
			0,				// prev
			0,				// next
			0,				// atom
			0				// phrases
			);
return cid;
}


/**************************************************
*						CON_ADD_WORD_FIRST
* FUN:	con_add_word_first
* SUBJ:	Add a word concept to start of hierarchic children.
* CR:	10/25/95 AM.
* RET:	con structure, if successful.
* ASS:	Conan - For quick addition of words.
*
**************************************************/

CON_ID CGCON::con_add_word_first(
	_TCHAR *name,				/* Descriptive name for new concept.	*/
	XCON_S *hier				/* Hierarchical parent of new concept.	*/
	)
{

if (!hier)
   {
   _ftprintf(stderr, _T("[con_add_word_first: No hierachical parent.]\n"));
   return 0;
   }
if (str_empty(name))
   {
   _ftprintf(stderr, _T("[con_add_word_first: No name given.]\n"));
   return 0;
   }

// Get string for concept's name.
bool dirty = false;									// 06/29/03 AM.
STR_ID sid = cg_->qkbm_->sym_get(name, dirty);
if (!sid)
	return 0;

CON_ID cid = cg_->qkbm_->addXcon(
			sid,			// val.
			cXWORD,		// kind
			0,				// flags
			0,				// attrs
			hier->id,	// up
			0,				// dn
			0,				// prev
			hier->dn,	// next.	// Rest of children. // FIX. // 02/28/07 AM.
			0,				// atom
			0				// phrases
			);
if (!cid)
	return 0;

// Todo: May want to make sure it's not already in dict hier, first....
cg_->qkbm_->addXsym(sid,cid);	// Bind string to dict concept.

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return 0;

/* Add new concept to its hierarchic parent's hierarchy. */
con_add_first(con, hier);			// 05/08/99 AM.

cg_->kbfree(con);

return cid;
}


/**************************************************
*						CON_CHILD_ALPHA
* FUN:	con_child_alpha
* SUBJ:	Find name in concept's hierarchic children (alphabetically ordered).
* CR:	10/24/95 AM.
* RET:	child - Named child, if found.
*		(up) pre - Location where named concept should be added.
*			(In case someone wants to add it if absent.)
*
**************************************************/

CON_ID CGCON::con_child_alpha(
	XCON_S *hier,
	_TCHAR *name,
	/*UP*/
	XCON_S **pre
	)
{
CON_ID cid;
XCON_S *phr;
int res;

*pre = 0;
if (!hier || str_empty(name))
   return 0;
cid = hier->dn;
phr = cg_->qkbm_->Con(cid);
if (!phr)
   return 0;

_TCHAR *str;
while (phr)
   {
   str = cg_->qkbm_->con_str(phr);
   res = _tcscmp(name, str);	/* Compare lexically. */
   cg_->kbfree(str);	// 03/05/07 AM.
   if (res < 0)
		{
		cg_->kbfree(phr);	// FIX.	// 03/19/07 AM.
      return 0;	/* Name should precede phr. */
      }
   else if (res == 0)
		{
		cid = phr->id;
		cg_->kbfree(phr);
      return cid;		/* Found it. */
      }
   if (*pre)
		cg_->kbfree(*pre);
   *pre = phr;			/* phr precedes name lexically. */
   cid = phr->next;
   phr = cg_->qkbm_->Con(cid);
   }
return 0;
}


/**************************************************
*						ID_GET_CHILD
* FUN:	id_get_child
* SUBJ:	Get immediate named child of given hierarchy concept.
* CR:	12/3/95 AM.
* RET:	id - child of given hier concept.
*
**************************************************/

CON_ID CGCON::id_get_child(
	CON_ID id,
	_TCHAR *name
	)
{
if (!id || str_empty(name))
	return 0;

XCON_S *con = cg_->qkbm_->Con(id);			// FIX.	// 03/21/07 AM.
CON_ID cid = con_find_child(con, name);	// FIX.	// 03/21/07 AM.
cg_->kbfree(con);	// FIX.	// 03/21/07 AM.
return cid;			// FIX.	// 03/21/07 AM.
}


/**************************************************
*						GET_COMPONENT
* FUN:	get_component
* SUBJ:	Get componet of a concept's path.
* CR:		04/25/00 AM.
* RET:	concept, if possible, else null.
* NOTE:	Helper function for path_to_con.
* WARN:	CALLER MUST FREE RETURNED OBJECT.
**************************************************/

XCON_S *CGCON::get_component(
	XCON_S *root,		// Parent concept to look under, if any.
	_TCHAR *compo		// Next component of path.
	)
{
if (!compo || !*compo)
	return 0;	// Flagging as error.	// 02/19/07 AM.

CON_ID cid;

if (root)			// Get compo under root.
	{
	cid = con_find_child(root, compo);
	return cg_->qkbm_->Con(cid);
	}

// Root is absent.  So compo should equal "concept".
if (!_tcscmp(compo, _T("concept")))
	return cg_->qkbm_->Con(cg_->cg_CONCEPT);	// Return root of hierarchy.

// We can be lenient and allow it to be a direct child of "concept", ie, root of hierarchy.
//_t_cerr << _T("[Path should start with \"concept\"]") << endl;
XCON_S *rcon = cg_->qkbm_->Con(cg_->cg_CONCEPT);
cid = con_find_child(rcon, compo);
cg_->kbfree(rcon);
return cg_->qkbm_->Con(cid);
}


/**************************************************
*						PATH_TO_CON
* FUN:	path_to_con
* SUBJ:	Convert hierarchy path string to concept.
* CR:		04/25/00 AM.
* RET:	concept, if possible, else null.
* NOTE:	Needed for NLP++.
*			Does NOT create concept if it doesn't already exist.
*			02/19/07 AM. REIMPLEMENT.  Note that another variant exists in UI.CPP.
* Opt:	Would like to munge string, but because of backslash escaping, need to copy
*			each component of path, in general.
*	WARN:	CALLER FREE THE RETURNED OBJECT.
*
**************************************************/

XCON_S *CGCON::path_to_con(
	_TCHAR *path
	)
{
if (!path || !*path)
	{
	_t_cerr << _T("[path_to_con: Warn: Given null path.]") << endl;
	return 0;
	}


XCON_S *con = 0;
XCON_S *prev = 0;					// 02/19/07 AM.
_TCHAR *ptr = path;				// Lookahead char.
_TCHAR *start = 0;				// First position of current component of path.
									// If not set, then at end of double quote region.
bool esc = false;				// If prev char was an escaping backslash.
bool inside = false;			// If inside double quotes.
_TCHAR buf[MAXSTR];				// Store current component of path.
buf[0] = '\0';
_TCHAR *bptr = buf;				// Ptr to component.
//con = c_cg_CONCEPT;		// Start at root.
// Traverse each piece of path.  Simultaneously traverse concept hierarchy.
for (;;)
	{
	switch (*ptr)				// Look at current char.
		{
		case '\0':
			*bptr = '\0';
			prev = con;
			con = get_component(prev, buf);	// Get final component of path, if any.
			if (!con)
				_t_cerr << _T("[path_to_con: Warn: Couldn't find con for path=") << path << _T("]") << endl;
			cg_->kbfree(prev);
			return con;			// Chars exhausted, return concept found.
		case ' ':
		case '\t':
		case '\n':
		case '\r':				// Whitespace.
			if (esc				// I suppose we'll allow escape of whitespace.
				 || inside)		// Or we're inside double quotes.
				{
				// Treat as ordinary char.
				*bptr++ = *ptr++;
				}
			else					// Else, it's a separator.
				{
				*bptr = '\0';		// Terminate component.
				prev = con;
				if (!(con = get_component(prev, buf)))	// Get latest part of path.
					{
					cg_->kbfree(prev);
					return 0;		// Failed.
					}
				cg_->kbfree(prev);
				prev = 0;
				buf[0] = '\0';		// Reset.
				bptr = buf;			// Reset.
				++ptr;
				}
			break;
		case '\"':				// Start or end of component.
			if (esc)
				*bptr++ = *ptr++;		// Ordinary char.
			else if (inside)	// End of component.
				{
				*bptr = '\0';
				prev = con;
				if (!(con = get_component(prev, buf)))	// Get latest part of path.
					{
					cg_->kbfree(prev);
					return 0;		// Failed.
					}
				cg_->kbfree(prev);
				prev = 0;
				++ptr;
				buf[0] = '\0';
				bptr = buf;			// Reset.
				inside = false;
				}
			else if (buf[0])	// In the middle of a component.
				*bptr++ = *ptr++;		// Grukky.  eg, "ab\"c".
			else							// Start of component.
				{
				inside = true;
				++ptr;
				}
			break;
		case '\\':				// Backslash.
			if (esc)				// Prev was an escaping backslash.
				{
				*bptr++ = *ptr++;	// Convert current to a simple backslash.
				}
			else
				{
				esc = true;
				++ptr;
				}
			break;
		default:					// Accumulate char.
			// Should probably have punct marks be separators, but who cares for now.
			*bptr++ = *ptr++;
			break;
		}
	}
}


/**************************************************
*						CON_FIND
* FUN:	con_find
* SUBJ:	Find concept with given name in a list with given start.
* CR:		02/20/07 AM.
* RET:	con - Concept matching name.
* NOTE:	Hard to imagine this isn't a function yet!
*
**************************************************/

XCON_S *CGCON::con_find(
	XCON_S *con,
	_TCHAR *name
	)
{
CON_ID cid;

if (!con || str_empty(name))
   {
   _ftprintf(stderr, _T("[con_find: Bad input.]\n"));
   return 0;
   }

// Get string from kb, for convenient matching.
STR_ID sid = cg_->qkbm_->getStrx(name);
if (!sid)
	return 0;	// String not in kb yet.

// Don't free the given first con in list.
bool first = true;

cid = con->id;
while (con)			// Traverse list.
   {
	switch (con->kind)
		{
		case cXBASIC:
			if (sid == con->v.vname)
				return con;
			break;
		case cXWORD:
			if (sid == con->v.vsym)
				return con;
			break;
		default:
			break;
		}
 	cid = con->next;
 	if (!first)
 		cg_->kbfree(con);
   if (!(con = cg_->qkbm_->Con(cid)))
		return 0;
	first = false;
   }
return 0;
}


/**************************************************
*						CON_FIND_CHILD
* FUN:	con_find_child
* SUBJ:	Find immediate named child of given hierarchy concept.
* CR:	10/19/95 AM.
* RET:	con - child of given hier concept.
* NOTE:	Todo: Could have a variant that returns the con, since we're fetching it.
*			02/19/07 AM. Made non-static.
*			02/19/07 AM. RENAMED from con_get_child, because we don't build a new one here.
*
**************************************************/

CON_ID CGCON::con_find_child(
	XCON_S *hier,
	_TCHAR *name
	)
{
CON_ID cid;
XCON_S *con;

if (!hier || str_empty(name))
   {
   _ftprintf(stderr, _T("[con_find_child: Bad input.]\n"));
   return 0;
   }

// Get string from kb, for convenient matching.	// 02/19/07 AM.
STR_ID sid = cg_->qkbm_->getStrx(name);			// 02/19/07 AM.
if (!sid)													// 02/19/07 AM.
	return 0;	// String not in kb yet.			// 02/19/07 AM.

cid = hier->dn;	// Get children.
CON_ID nextcid = 0;										// 02/19/07 AM.
while (cid)			// Traverse list of children.
   {
   if (!(con = cg_->qkbm_->Con(cid)))
		return 0;
	nextcid = con->next;	// 02/19/07 AM.
	switch (con->kind)	// 02/19/07 AM.
		{
		case cXBASIC:
			if (sid == con->v.vname)
				{
				cg_->kbfree(con);		// 02/27/07 AM.
				return cid;
				}
			break;
		case cXWORD:
			if (sid == con->v.vsym)
				{
				cg_->kbfree(con);		// 02/27/07 AM.
				return cid;
				}
			break;
		default:
			break;
		}
	cg_->kbfree(con);		// 021/9/07 AM.
   cid = nextcid;
   }
return 0;
}



/**************************************************
*						NODE_FIND
* FUN:	node_find
* SUBJ:	Find node with given name in a list with given start.
* CR:		02/20/07 AM.
* RET:	node - Concept matching name.
* NOTE:	Hard to imagine this isn't a function yet!
*
**************************************************/

XCON_S *CGCON::node_find(
	XCON_S *con,
	_TCHAR *name
	)
{
CON_ID cid;

if (!con || str_empty(name))
   {
   _ftprintf(stderr, _T("[con_find: Bad input.]\n"));
   return 0;
   }

if (con->kind != cXPROXY)
   {
   _ftprintf(stderr, _T("[node_find: Bad input.]\n"));
   return 0;
   }

// Get string from kb, for convenient matching.
STR_ID sid = cg_->qkbm_->getStrx(name);
if (!sid)
	return 0;	// String not in kb yet.

// Don't free the given first con in list.
bool first = true;

cid = con->id;
while (con)			// Traverse list.
   {
   // Match against owner name.
   if (sid == cg_->qkbm_->node_sid(con))
		return con;

 	cid = con->next;
 	if (!first)
 		cg_->kbfree(con);
   if (!(con = cg_->qkbm_->Con(cid)))
		return 0;
	first = false;
   }
return 0;
}



/**************************************************
*						CON_NTH_PROXY
* FUN:	con_nth_proxy
* SUBJ:	Find nth node.
* CR:		02/26/07 AM.
* RET:	nid - Desired node.
* NOTE:	REIMPLEMENTATION.
*			One-based numbering.
*
**************************************************/

CON_ID CGCON::con_nth_proxy(
	XCON_S *con,
	long pos	// If 0, find last node.
	)
{
if (!con)
	return 0;

CON_ID nid = cg_->qkbm_->con_phrase(con);
if (!nid)
	return 0;

XCON_S *node = cg_->qkbm_->Con(nid);

long ii = 1;
while (node && ii != pos)
	{
	if (!node->next)
		{
		// Reached end.
		cg_->kbfree(node);
		if (!pos)	// Found end of list.
			return nid;
		return 0;	// Desired pos not found.
		}
	nid = node->next;
	cg_->kbfree(node);	// 03/05/07 AM.
	if (nid)	// 03/05/07 AM.
		node = cg_->qkbm_->Con(nid);
	else		// 03/05/07 AM.
		node = 0;	// 03/05/07 AM.
	++ii;
	}
if (node)
	cg_->kbfree(node);
return nid;
}


/**************************************************
*						CON_KIND_STR
* FUN:	con_kind_str
* SUBJ:	Generate a kind string for the given concept.
* CR:	9/20/95 AM.
*
**************************************************/

_TCHAR *CGCON::con_kind_str(
	XCON_S *con
	)
{
_TCHAR *emp;
//XCON_S *bas;

emp = _T("");

if (!con)
   return emp;
switch (con->kind)
   {
   case cNO_CON:	return _T("nul");
   case cXPROXY:		return _T("prx");
   case cXBASIC:		return _T("bas");
   case cXWORD:		return _T("wrd");
   /*
   case cPHRASE:
   case cRULE:
   */
   case cNUM:
   case cCHAR:
   default:
      _ftprintf(stderr, _T("[con_kind_str: Unimplemented kind.]\n"));
      return emp;
   }
}





/**************************************************
*						CON_STR_PRET
* FUN:	con_str_pret
* SUBJ:	Generate a pretty string for the given concept.
* CR:		6/23/96 AM.
* WARN:	USER MUST FREE STRING (QDBM).
*
**************************************************/

_TCHAR *CGCON::con_str_pret(XCON_S *con)
{
_TCHAR *str;

str = cg_->qkbm_->con_str(con);
if (!str || !*str)
   str = _T("EMPTY STR");
else if (!_tcscmp(str, _T("\n")))
   str = _T("\\n");
else if (!_tcscmp(str, _T("\t")))
	str = _T("\\t");
else if (!_tcscmp(str, _T(" ")))
	str = _T("\\s");
return str;
}
 

/**************************************************
*						NC
* FUN:	nc
* SUBJ:	Get concept referenced by a proxy.
* CR:		11/9/95 AM.
* NOTE:	"NC" stands for "node's concept".
* WARN:	USER FREES UP RETURNED CONCEPT OBJECT.
*
**************************************************/

XCON_S *CGCON::nc(
	XCON_S *proxy
	)
{
if (!proxy)
   return 0;
if (proxy->kind != cXPROXY)
   {
   _ftprintf(stderr, _T("[nc: Not given proxy.]\n"));
   return 0;
   }
CON_ID cid;
if (!(cid = proxy->v.vcon))
   {
   _ftprintf(stderr, _T("[nc: proxy has no concept.]\n"));
   return 0;
   }
XCON_S *con;
if (!(con = cg_->qkbm_->Con(cid)))
   {
   _ftprintf(stderr, _T("[nc: proxy has no concept.]\n"));
   return 0;
   }
return con;
}
 

/**************************************************
*						PHRASE_LEN
* FUN:	phrase_len
* SUBJ:	Get length of the given phrase.
* CR:		7/8/96 AM.
*
**************************************************/

long CGCON::phrase_len(XCON_S *phr)
{

if (!phr)
	return 0;

// Don't free the concept copy you're given, but free the rest.	// 02/19/07 AM.
long len = 1;
CON_ID cid = phr->next;
while (phr = cg_->qkbm_->Con(cid))
	{
	++len;
	cid = phr->next;
	cg_->kbfree(phr);
	}
return len;
}

 
/**************************************************
*						CON_NTH
* FUN:	con_nth
* SUBJ:	Get the nth concept in a phrase.
* CR:	11/15/95 AM.
* RET:	elt - nth element, if present.
* NOTE:	05/03/99 AM. Using nth==0 to mean the end of phrase.
*
**************************************************/

CON_ID CGCON::con_nth(
	XCON_S *phr,
	long nth
	)
{
if (!phr || nth < (long) 0)		// 05/03/99 AM.
   return 0;
if (nth == 0)							// 05/03/99 AM.
	return con_last(phr);			// 05/03/99 AM.

// Don't free the concept you're given.	// 02/19/07 AM.
CON_ID cid = phr->next;				// 02/19/07 AM.
--nth;									// 02/19/07 AM.

while (phr = cg_->qkbm_->Con(cid))
   {
   if (--nth <= (long) 0)
		{
		cg_->kbfree(phr);
      return cid;
      }
   cid = phr->next;
   cg_->kbfree(phr);
   }
return 0;
}

/**************************************************
*						CON_ORD
* FUN:	con_ord
* SUBJ:	Get the position of node in its phrase.
* CR:		10/09/05 AM.
* RET:	num - position in phrase, 1 based.  0 if not a node.
* NOTE:	Don't know why not used for concepts in hier.	// 02/19/07 AM.
*
**************************************************/

long CGCON::con_ord(
	XCON_S *con
	)
{
if (!con)
   return 0;
if (con->kind != cXPROXY)
	return 0;

// Don't free the con copy you're given.			// 02/19/07 AM.
long ord = 1;
CON_ID cid = con->prev;
while (con = cg_->qkbm_->Con(cid))
	{
   ++ord;
   cid = con->prev;
   cg_->kbfree(con);
   }
return ord;
}

/**************************************************
*						NODE_OWNER
* FUN:	node_owner
* SUBJ:	Get concept owning current node and phrase.
* CR:		10/09/05 AM.
* RET:	con - owning concept, if given a node.
* NOTE:	REMPLEMENT.	02/19/07 AM.
*			Now every node points to its atomic concept (ie, owner).
*
**************************************************/

CON_ID CGCON::node_owner(
	XCON_S *node
	)
{
if (!node)
   return 0;
if (node->kind != cXPROXY)
	return 0;

return node->atom;	// 02/19/07 AM.
}


/**************************************************
*						CON_LAST
* FUN:	con_last
* SUBJ:	Get the last concept in a phrase.
* CR:		05/03/99 AM.
* RET:	last element, if present.
*
**************************************************/

CON_ID CGCON::con_last(
	XCON_S *phr
	)
{
if (!phr)
	return 0;
if (!phr->next)
	return phr->id;

// Don't free the one you're given.	// 02/19/07 AM.
CON_ID cid = phr->next;
while (phr = cg_->qkbm_->Con(cid))
	{
	if (!phr->next)
		{
		cg_->kbfree(phr);
		return cid;
		}
	cid = phr->next;
	cg_->kbfree(phr);
	}
return cid;
}


/**************************************************
*						CON_TO_PATH
* FUN:	con_to_path
* SUBJ:	Build hierarchy path string for concept.
* CR:		04/29/99 AM.
* NOTE:	04/30/99 AM. Building quoted string for every concept.
* OPT:	Using non-optimal but simple recursion.
*			Should quote only what needs it.
*
**************************************************/

_TCHAR *CGCON::con_to_path(		// 04/29/99 AM.
	XCON_S *con,
	/*UP*/
	_TCHAR *buf			// Buffer to fill with path.
	)
{
*buf = '\0';
if (!con)
	{
	_t_cerr << _T("[con_to_path: Given null concept.]") << endl;
	return (_TCHAR *) NULL;
	}

// Glom my ancestors, then glom me.
_TCHAR *end = buf;
con_to_path_r(con, /*UP*/ end);	// Recursive.
*end = '\0';							// Terminate path string.
return buf;
}


/**************************************************
*						CON_TO_PATH_R
* FUN:	con_to_path_r
* SUBJ:	Build hierarchy path string for concept. Recursive.
* CR:		04/29/99 AM.
* OPT:	Using non-optimal but simple recursion.
*
**************************************************/

void CGCON::con_to_path_r(
	XCON_S *con,
	/*UP*/
	_TCHAR* &end			// First empty loc in buffer.
	)
{
CON_ID parent_cid;
XCON_S *parent;
if (parent_cid = con_parent(con))
	{
	parent = cg_->qkbm_->Con(parent_cid);
	// Glom my ancestors.
	con_to_path_r(parent, end);
	cg_->kbfree(parent);	// 02/19/07 AM.

	// I had ancestors, so separate me from them.
	*end++ = ' ';					// Add separation space.
	}

// Glom me.
_TCHAR *str;
_TCHAR buf[2048];
str = cg_->qkbm_->con_str(con);
prep_str(buf, str);
cg_->kbfree(str);	// 03/05/07 AM.
str = buf;
*end++ = '"';
while (*end++ = *str++)	// Concept's string.
	;							// Copy string into buffer.
//end--;						// Back to first empty space.
*(end-1) = '"';
}


/**************************************************
*						CON_REMOVE
* FUN:	con_remove
* SUBJ:	Remove concept from hierarchy.
* CR:		05/03/99 AM.
* NOTE:	Not deleting nor wiping out data structures.
*			(Can't delete, but could zero it out in the concept table.)
*			02/19/07 AM. REIMPLEMENT USING QDBM.
*			Now concept and all its dependencies are deleted.
* RET:	True if successful.
*
**************************************************/

bool CGCON::con_remove(CON_ID cid)
{
return cg_->qkbm_->con_rm(cid);
}


/**************************************************
*						CON_PARENT
* FUN:	con_parent
* SUBJ:	Get concept's parent in hierarchy.
* CR:		05/13/99 AM.
* NOTE:	A long-overdue function.  Apparently, all concepts
*			in a list were getting pointer to the hierarchy parent.
*			Yet this wasn't being updated when moving concepts
*			around.
*			02/19/07 AM. REIMPLEMENT. All concepts must have their up pointer maintained.
*
**************************************************/

CON_ID CGCON::con_parent(XCON_S *con)
{
if (!con)
	return 0;
if (con->kind == cXPROXY)
	return 0;
return con->up;
}


/**************************************************
*						MAKE_PHRASE_USING_DICT
* FUN:	make_phrase
* SUBJ:	Make first node of phrase, using a dict-word concept.
* CR:		02/20/07 AM.
* RET:	nid - Created phrase node's id.
**************************************************/

CON_ID CGCON::make_phrase_using_dict(
	XCON_S *c_owner,	// Owning concept for phrase.
	_TCHAR *name		// Use to get word-concept.
	)
{
if (!c_owner || str_empty(name))
	return 0;

CON_ID i_owner = c_owner->id;

// Consistency check. If concept already has a phrase, error.
// (Disallowing dangling ptr that points to nothing.)
if (c_owner->phrases)
	return 0;

bool dirty;
CON_ID w_cid = cg_->cgdict_->dict_get_word_id(name,/*UP*/ dirty);


// Make concept to be the first node.
XCON_S node_s;
XCON_S *node = &node_s;
node->kind = cXPROXY;
node->v.vcon = w_cid;	// Node points to its "target".
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = 0;
node->next = 0;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

// Add node to db and get its id.
CON_ID nid = cg_->qkbm_->addXcon(node);

// Make a ptr to point to the first node.
PTR_ID pid = cg_->qkbm_->addXptr(nid, pXCON, 0);

// Owner points to ptr for phrase.
c_owner->phrases = pid;
cg_->qkbm_->con_update(i_owner, c_owner);	// Update changed owner to db.

return nid;
}


/**************************************************
*						PHRASE_RENAME_NODE
* FUN:	phrase_rename_node
* SUBJ:	Find and rename phrase node (using dict concept).
* CR:		02/21/07 AM.
* RET:	nid - The renamed node.
**************************************************/

CON_ID CGCON::phrase_rename_node(
	XCON_S *phr,		// First node of phrase.
	_TCHAR *oldname,	// Use for finding node in phrase.
	_TCHAR *newname	// String for rename.
	)
{
if (!phr || str_empty(oldname) || str_empty(newname))
	return 0;

XCON_S *node;
if (!(node = node_find(phr, oldname)))
	return 0;
CON_ID nid = node->id;

// Get dictionary concept for new name.
bool dirty;
CON_ID word_cid;
if (!(word_cid = cg_->cgdict_->dict_get_word_id(newname, dirty)))
	{
	cg_->kbfree(node);
	return 0;
	}

// Now "rename" the found node by assigning it a new target concept.
cg_->qkbm_->node_set_target(node, word_cid, /*UP*/ dirty);
cg_->kbfree(node);

return nid;
}



/**************************************************
*						PHRASE_RENAME_NODE
* FUN:	phrase_rename_node
* SUBJ:	Find and rename phrase node (using dict concept).
* CR:		02/21/07 AM.
* RET:	nid - The renamed node.
**************************************************/

CON_ID CGCON::phrase_rename_node(
	XCON_S *phr,		// First node of phrase.
	int pos,				// Position in phrase, 1-based.
	_TCHAR *newname	// String for rename.
	)
{
if (!phr || pos < 0 || str_empty(newname))
	return 0;

XCON_S *node;
CON_ID nid = con_nth(phr, pos);
if (!nid)
	return 0;
if (!(node = cg_->qkbm_->Con(nid)))
	return 0;

// Get dictionary concept for new name.
bool dirty;
CON_ID word_cid;
if (!(word_cid = cg_->cgdict_->dict_get_word_id(newname, dirty)))
	{
	cg_->kbfree(node);
	return 0;
	}

// Now "rename" the found node by assigning it a new target concept.
cg_->qkbm_->node_set_target(node, word_cid, /*UP*/ dirty);
cg_->kbfree(node);

return nid;
}


//  ***************************************************************************
//////////////////////////// FROM CONSH_KB ////////////////////////////////////


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

void CGCON::con_add_root()
{
XCON_S con_x;	// Build a struct, automatically freed on scope exit.	// 02/14/07 AM.
XCON_S *con = &con_x;	// 02/14/07 AM.

//SYM *sym;
//bool dirt;	// 06/29/03 AM.


// Todo:	Check that db is reset and counter is at 1.

cg_->cg_CONCEPT = 1;	// 02/14/07 AM.
STR_ID sid = cg_->qkbm_->addString(_T("concept"));

CON_ID cid = cg_->qkbm_->addXcon(sid, cXBASIC,0,0,0,0,0,0,0,0);	// 02/27/07 AM.

if (cg_->cg_CONCEPT != cid)
   {
   _t_cerr << _T("[con_add_root: Root concept must be first in kb.]") << endl;
   return;
   }

/*
con = cg_->qkbm_->con_alloc();
sym = cg_->qkbm_->sym_get(CON_ROOT_NAME,dirt);

con->kind = cXBASIC;
con->v.vname = sym;

cg_->cg_CONCEPT = con->id;	// Bind root concept to a global variable.

// Since optimized to use concepts, need to get concepts defined when
// created.															// FIX.	// 08/25/02 AM.
//cg_->qkbm_->c_cg_CONCEPT = con;								// FIX.	// 08/25/02 AM.
*/

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
*			02/19/07 AM. This was moved to libqkbm.
*
**************************************************/

//XCON_S *
//con_phrase(
//	XCON_S *con
//	)
//{
//return(attr_get_c(con, c_cg_PHRASE));
//}




/**************************************************
*						PHR_ADD_NTH
* FUN:	phr_add_nth
* SUBJ:	Add concept to nth position of phrase.
* CR:		05/03/99 AM.
*	RET:	id - new node inserted into phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*
**************************************************/


CON_ID CGCON::phr_add_nth(
	XCON_S *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	_TCHAR *name,	// Node is proxy for word-concept named here.
	int pos			// position to insert into.  if = 0, at end of list.
	)
{
CON_ID phr_cid;
CON_ID word_cid;			// Concept for given name (or string).
XCON_S *word;
//int res;

if (!owner || !name || !*name)
   return 0;

phr_cid = cg_->qkbm_->con_phrase(owner);

bool dirt;																	// 06/29/03 AM.
word = cg_->cgdict_->dict_get_word(name,dirt);				// 06/29/03 AM.
if (!word)																// 06/29/03 AM.
	return 0;																// 06/29/03 AM.
word_cid = word->id;
cg_->kbfree(word);	// 03/05/07 AM.

// Concept may have no phrase yet.
if (!phr_cid)																// 02/19/07 AM.
	{
   if (pos == 0 || pos == 1)	// Build phrase.					// 02/19/07 AM.
		return cg_->qkbm_->make_phrase(owner, word_cid);		// 02/19/07 AM.
	return 0;	// Error.												// 02/19/07 AM.
	}

// First position is special.
if (pos == 1)																// 02/19/07 AM.
	return cg_->qkbm_->phrase_add_start(owner, word_cid);		// 02/19/07 AM.

XCON_S *phr = cg_->qkbm_->Con(phr_cid);	// Head node of phrase.
if (!phr)
	return 0;	// Error.

if (pos == 0)				// Add to end of the list.
	{
	while (phr_cid = phr->next)
		{
		cg_->kbfree(phr);
		phr = cg_->qkbm_->Con(phr_cid);
		}
	if (!phr)
		return 0;	// Error.
	CON_ID nn = cg_->qkbm_->extend_phrase(phr, word_cid);
	cg_->kbfree(phr);
	return nn;
	}

// Traverse to nth node.
while (--pos > 1)			// Fix.	// 05/04/99 AM.
	{
	if (!(phr_cid = phr->next))
		{
		cg_->kbfree(phr);
		return 0;
		}
	// Prior position.
	if (!(phr = cg_->qkbm_->Con(phr_cid)))
		return 0;
	}

if (!phr)
	return 0;

// Make node and insert into phrases.
CON_ID nn = cg_->qkbm_->phrase_insert_after(phr, word_cid);
cg_->kbfree(phr);
return nn;
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


CON_ID CGCON::phr_add_nth(
	XCON_S *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	XCON_S *node,		// Node to attach.
	int pos			// position to insert into.  if = 0, at end of list.
	)
{
CON_ID phr_cid;

if (!owner || !node)
   return 0;

if (node->kind != cXPROXY)
	return 0;

phr_cid = cg_->qkbm_->con_phrase(owner);

// Concept may have no phrase yet.
if (!phr_cid)																// 02/20/07 AM.
	{
	if (pos == 0 || pos == 1)	// Build phrase.					// 02/20/07 AM.
		return cg_->qkbm_->make_phrase(owner, node);				// 02/20/07 AM.
	return 0;	// Error.												// 02/20/07 AM.
	}

// First position is special.
if (pos == 1)																// 02/20/07 AM.
	return cg_->qkbm_->phrase_add_start(owner, node);			// 02/20/07 AM.

XCON_S *phr = cg_->qkbm_->Con(phr_cid);	// Head node of phrase.
if (!phr)
	return 0;	// Error.

if (pos == 0)				// Add to end of the list.
	{
	while (phr_cid = phr->next)
		{
		cg_->kbfree(phr);
		phr = cg_->qkbm_->Con(phr_cid);
		}
	if (!phr)
		return 0;	// Error.
	CON_ID nn = cg_->qkbm_->extend_phrase(phr, node);
	cg_->kbfree(phr);
	return nn;
	}
	
// Traverse to nth node.
while (--pos > 1)			// Fix.	// 05/04/99 AM.
	{
	if (!(phr_cid = phr->next))
		{
		cg_->kbfree(phr);
		return 0;
		}
	// Prior position.
	if (!(phr = cg_->qkbm_->Con(phr_cid)))
		return 0;
	}

if (!phr)
	return 0;

// Make node and insert into phrases.
CON_ID nn = cg_->qkbm_->phrase_insert_after(phr, node);
cg_->kbfree(phr);
return nn;
}


/**************************************************
*						INSERT_NODE
* FUN:	insert_node
* SUBJ:	Insert one node before a phrase node.
* CR:		05/05/99 AM.
* RET:	The node inserted into phrase.
* ARG:	n_pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*			02/20/07 AM. Could implement specialized handlers for this,
*			but simpler to use the insert-after functions created in the
*			past few days.
*
**************************************************/

CON_ID CGCON::insert_node(
	XCON_S *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	XCON_S *node,		// Node to attach.
	XCON_S *n_pos		// position to insert before.  if = 0, at end of list.
	)
{
if (!owner || !node)
   return 0;

if (node->kind != cXPROXY)
	return 0;

if (n_pos && n_pos->kind != cXPROXY)
	return 0;

// If no position, add to end of phrase.
// Letting empty phrase be ok in this case.	// 02/20/07 AM.
return con_add_nth(owner, node, 0);				// 02/20/07 AM.

CON_ID phr_cid;
if (!(phr_cid = cg_->qkbm_->con_phrase(owner)))
	return 0;

// If the position node is first, add node to start of phrase.
if (!n_pos->prev)
	return cg_->qkbm_->phrase_add_start(owner, node);	// 02/20/07 AM.

// Get the previous position and add after that.
// Opt: Somewhat redundant fetch of a node.
XCON_S *n_pre = cg_->qkbm_->Con(n_pos->prev);			// 02/20/07 AM.
CON_ID xid = cg_->qkbm_->phrase_insert_after(n_pre, node);	// 02/20/07 AM.
cg_->kbfree(n_pre);	// 03/05/07 AM.
return xid;	// 03/05/07 AM.
}


/**************************************************
*						RM_PROXY
* FUN:	rm_proxy
* SUBJ:	Remove proxy concept from phrase.
* CR:		05/04/99 AM.
* NOTE:	Rebind phrase if it's the first node in phrase.
*			02/20/07 AM. Keeping for legacy.  Can replace calls to it and remove it.
*
**************************************************/

bool CGCON::rm_proxy(
	XCON_S *proxy
	)
{
return cg_->qkbm_->phrase_rm_node(proxy);
}


/********************************************
* FN:		SORT_PHRASE
* CR:		05/08/99 AM.
* SUBJ:	Sort nodes in a phrase alphabetically by name.
********************************************/
static CG *g_cg = 0;	// "global" cg_ pointer.

bool CGCON::sort_phrase(
	XCON_S *con			// The concept owning the phrase.
	)
{
if (!con)
	return false;

// Detach phrase from concept.
PTR_ID phrase_pid = con->phrases;
XPTR_S *phrase_ptr = 0;
XCON_S *phr = cg_->qkbm_->detach_phrase(con, /*UP*/ phrase_ptr);

if (!phr)	// Empty phrase is ok.
	{
	if (phrase_ptr)
		cg_->kbfree(phrase_ptr);
	return true;
	}

CON_ID phr_cid = phr->id;

// OPT:	For efficiency, should allocate a big block, bigger than max length
// allowed for a phrase, then cast it as pointers to concepts.  Then
// traverse it only once. (Not once for length, as below.)	// 02/19/07 AM.

// Create an array.
long len = 0;
XCON_S **arr;
len = phrase_len(phr);
arr = new XCON_S * [len];

// Make array elts point to list elts.
XCON_S **parr;
XCON_S *pnode;
for (pnode = phr, parr = arr;
	  pnode;
	  pnode = cg_->qkbm_->Con(pnode->next), ++parr
	  )
	{
	*parr = pnode;
	}

// Sort array.
g_cg = cg_;	// For handing to comparison fn.	// 02/14/07 AM.
qsort((void *)arr, (size_t)len, sizeof(XCON_S *), compare_nodes);

// GRAB THE ID OF FIRST PHRASE NODE.
CON_ID phrase_nid = (*parr)->id;		// 02/20/07 AM.

// Relink list.
// Note that the old phrase head object gets deleted here as well.
parr = arr;
(*parr)->prev = 0;		// Fix first element's left ptr.
while (--len > 0)			// For all but last elt.
	{
	(*parr)->next = (*(parr+1))->id;	// 02/20/07 AM.
	(*(parr+1))->prev = (*parr)->id;	// 02/20/07 AM.
	cg_->qkbm_->con_update((*parr)->id, *parr);	// 02/20/07 AM.
	cg_->kbfree(*parr);									// 02/20/07 AM.
	++parr;
	}
// Fix up the last element.
(*parr)->next = 0;
cg_->qkbm_->con_update((*parr)->id, *parr);	// 02/20/07 AM.
cg_->kbfree(*parr);									// 02/20/07 AM.

// Attach sorted phrase.
cg_->qkbm_->attach_phrase(phrase_pid, phrase_ptr, phrase_nid);
cg_->kbfree(phrase_ptr);									// 02/20/07 AM.

// Trash the array.
delete [] arr;
return true;
}



/********************************************
* FN:		SORT_CHILDS
* CR:		02/20/09 AM.
* SUBJ:	Sort nodes in a phrase alphabetically by name.
********************************************/

bool CGCON::sort_childs(
	XCON_S *con			// The parent concept.
	)
{
if (!con)
	return false;

// Detach phrase from concept.

PTR_ID phrase_cid = con->dn;
if (!phrase_cid)
	return false;

XCON_S *phr = cg_->qkbm_->Con(phrase_cid);

// OPT:	For efficiency, should allocate a big block, bigger than max length
// allowed for a phrase, then cast it as pointers to concepts.  Then
// traverse it only once. (Not once for length, as below.)	// 02/19/07 AM.

// Create an array.
long len = 0;
XCON_S **arr;
len = phrase_len(phr);
arr = new XCON_S * [len];

// Make array elts point to list elts.
XCON_S **parr;
XCON_S *pnode;
for (pnode = phr, parr = arr;
	  pnode;
	  pnode = cg_->qkbm_->Con(pnode->next), ++parr
	  )
	{
	*parr = pnode;
	}

// Sort array.
g_cg = cg_;	// For handing to comparison fn.	// 02/14/07 AM.
qsort((void *)arr, (size_t)len, sizeof(XCON_S *), compare_nodes);

// GRAB THE ID OF FIRST PHRASE NODE.
CON_ID phrase_nid = (*parr)->id;		// 02/20/07 AM.

// Relink list.
// Note that the old phrase head object gets deleted here as well.
parr = arr;
(*parr)->prev = 0;		// Fix first element's left ptr.
while (--len > 0)			// For all but last elt.
	{
	(*parr)->next = (*(parr+1))->id;	// 02/20/07 AM.
	(*(parr+1))->prev = (*parr)->id;	// 02/20/07 AM.
	cg_->qkbm_->con_update((*parr)->id, *parr);	// 02/20/07 AM.
	cg_->kbfree(*parr);									// 02/20/07 AM.
	++parr;
	}
// Fix up the last element.
(*parr)->next = 0;
cg_->qkbm_->con_update((*parr)->id, *parr);	// 02/20/07 AM.
cg_->kbfree(*parr);									// 02/20/07 AM.

// Attach sorted phrase.
CON_ID cid = (*arr)->id;
con->dn = cid;
(*arr)->up = con->id;

// Trash the array.
delete [] arr;
return true;
}


/********************************************
* FN:		CON_REDUCE_SUBLIST
* CR:		02/26/07 AM.
* SUBJ:	Place list of concepts under new concept.
* RET:	bool - ok if successful.
* NOTE:	Since it deals with hierarchy, belongs in libqconsh.
********************************************/

CON_ID CGCON::con_reduce_sublist(
	_TCHAR *name,		// Name for new parent concept.
	XCON_S *start,		// Start of sublist.
	XCON_S *end			// End of sublist. Can be null.
	)
{
if (str_empty(name) || !start)
	return 0;


CON_ID hier_cid = start->up;

CON_ID post_cid = 0;
XCON_S *post = 0;
if (end)
	post_cid = end->next;

CON_ID pre_cid = start->prev;
XCON_S *pre = 0;

// Make the new parent concept.
CON_ID parent_cid = CGCON::con_add_internal(
	name,
	0, //attrs
	hier_cid,	// up
	start->id,	// dn
	pre_cid,		// prev
	post_cid,	// next
	0				// phrases
	);
if (!parent_cid)
	return 0;

if (end)
	{
	post = cg_->qkbm_->Con(post_cid);
	post->prev = parent_cid;
	cg_->qkbm_->con_update(post_cid, post);
	cg_->kbfree(post);
	}

if (pre_cid)
	{
	pre = cg_->qkbm_->Con(pre_cid);
	pre->next = parent_cid;
	cg_->qkbm_->con_update(pre_cid, pre);
	cg_->kbfree(pre);	// 03/05/07 AM.
	}
else	// Attach parent to hierarchy.
	{
	XCON_S *hier = cg_->qkbm_->Con(hier_cid);
	hier->dn = parent_cid;
	cg_->qkbm_->con_update(hier_cid, hier);
	cg_->kbfree(hier);
	}

return parent_cid;
}


/**************************************************
*						CON_DETACH
* FUN:	con_detach
* SUBJ:	Detach a concept from the hierarchy.
* CR:		03/03/07 AM.
* RET:	True if detached ok.
* NOTE:	KEEPING CHILDREN ATTACHED
*
**************************************************/

bool CGCON::con_detach(XCON_S *con)
{
if (!con)
	return false;

CON_ID nxt_cid = con->next;
CON_ID prv_cid = con->prev;
CON_ID hier_cid = con->up;
if (!hier_cid)
	return false;	// Can't detach root.

if (nxt_cid)
	{
	XCON_S *nxt = cg_->qkbm_->Con(nxt_cid);
	nxt->prev = prv_cid;
	cg_->qkbm_->con_update(nxt_cid, nxt);
	cg_->kbfree(nxt);
	}

if (prv_cid)
	{
	XCON_S *prv = cg_->qkbm_->Con(prv_cid);
	prv->next = nxt_cid;
	cg_->qkbm_->con_update(prv_cid, prv);
	cg_->kbfree(prv);
	}
else	// con was first in list.
	{
	XCON_S *hier = cg_->qkbm_->Con(hier_cid);
	hier->dn = nxt_cid;
	cg_->qkbm_->con_update(hier_cid, hier);
	cg_->kbfree(hier);
	}

con->next = con->prev = con->up = 0;
cg_->qkbm_->con_update(con->id, con);
return true;
}


/**************************************************
*						CON_RM_HIER
* FUN:	con_rm_hier
* SUBJ:	Remove a concept and its subtree from the hierarchy.
* CR:		03/04/07 AM.
* RET:	True if removed ok.
*
**************************************************/

bool CGCON::con_rm_hier(XCON_S *con)
{
if (!con)
	return false;

// Detach concept from hierarchy.
if (!con_detach(con))
	return false;

// Remove children recursively.
con_rm_children(con->dn);

// Remove concept itself.
cg_->qkbm_->con_rm(con);
return true;
}


/**************************************************
*						CON_RM_CHILDREN
* FUN:	con_rm_children
* SUBJ:	Remove a complete list of children and their subtrees from the hierarchy.
* CR:		03/04/07 AM.
* RET:	True if removed ok.
* NOTE:	RECURSIVE.
*
**************************************************/

bool CGCON::con_rm_children(CON_ID cid)
{
if (!cid)
	return true;	// No children is ok.

XCON_S *con;
while (con = cg_->qkbm_->Con(cid))
	{
	// No need to detach con from hier.
	con_rm_children(con->dn);	// Remove children recursively.
	cid = con->next;
	cg_->qkbm_->con_rm(con);	// Remove concept itself.
	cg_->kbfree(con);
	}
return true;
}

/**************************************************
*						CLEAR_PATH
* FUN:	clear_path
* SUBJ:	Clear a tracked path from depth to max depth.
* CR:		03/08/07 AM.
* RET:	True if cleared ok.
* OPT:	Part of optimizations for knowledge access.
*
**************************************************/

bool CGCON::clear_path(
	int depth,
	XCON_S **path,
	_TCHAR **spath,
	int &lpath
	)
{
if (depth < 0 || lpath < 0)
	return false;
if (depth > lpath)
	return false;

int ii;
for (ii = depth; ii <= lpath; ++ii)
	{
	cg_->kbfree(path[ii]);
	cg_->kbfree(spath[ii]);
	// Don't need to zero them out.
	}
lpath = depth - 1;

return true;
}


///////////////////////////////////////////////////////////////////////////////
//                        GENERAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

// REIMPLEMENT. Changed all statics to non-static.	// 02/19/07 AM.

///////////////////////////////////////////////////////////////////////////////
//                        UTILITIES
///////////////////////////////////////////////////////////////////////////////


/********************************************
* FN:		COMPARE_NODES
* CR:		05/08/99 AM.
* SUBJ:	Compare nodes alphabetically.
* NOTE:	Used by qsort.
*			02/19/07 AM. Will see if this works as a member function.
********************************************/

int compare_nodes( const void *arg1, const void *arg2 )
{
XCON_S **a1, **a2;

a1 = (XCON_S **) arg1;
a2 = (XCON_S **) arg2;

// Get node's "real" concepts.
_TCHAR *str1 = g_cg->qkbm_->con_str(*a1);	// 03/05/07 AM.
_TCHAR *str2 = g_cg->qkbm_->con_str(*a2);	// 03/05/07 AM.

int res = unicu::strCmp(str1,str2);
/*
#ifndef LINUX
int res = _tcsicmp(str1, str2);
#else
int res = strcasecmp(str1, str2);
#endif
*/
g_cg->kbfree(str1);	// 03/05/07 AM.
g_cg->kbfree(str2);	// 03/05/07 AM.
return res;				// 03/05/07 AM.
}


/**********************     END OF FILE         ***************************/
