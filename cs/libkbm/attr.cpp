/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/
/*******************************************************************************
*
*									ATTR.C
*
* FILE:	libkbm.¹/attr.c
* SUBJ:	Attribute manager for concepts.
* CR:		10/9/95 AM.
* NOTE:	Layered over the PTR manager.
*			'attr' refers specifically to the organizing PTR structure.
*			'slot' refers to the first PTR pointed to by attr.
*			'val' refers to subsequent PTRs following slot.
*			Attr mgmt is slightly tedious because of strict typing of PTR struct.
*			Using "a", "s", "v" for micro-management commands.
*			May want commands to add alphabetically, add at beginning.
*			(Alphabetic is important, at least for display in the UI.)
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#ifdef LINUX
#include <sstream>
#endif
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include	"prim/list_s.h"
#include "prim/list.h"

#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/dict.h"			// 05/03/99 AM.

#include "kbm_priv.h"			// 10/31/00 AM.
#include "kbm/st.h"	// 10/30/06 AM.
#include "kbm/Akbm.h"

// Not sure what this was about.  Very old.	// 08/21/02 AM.
//#define DEBUG


/**************************************************
*						ATTR_ADD_BY_NAME
* FUN:	attr_add_by_name
* SUBJ:	Add values to an attribute by name.
* CR:		05/03/99 AM.
* RET:	Pointer to new slot.
* NOTE:	Allows redundant slot names.  Adds new slot to end of attrs.
*
**************************************************/

PTR *AKBM::attr_add_by_name(
   CON *con,
   _TCHAR *name,
   _TCHAR *val)			// STRING value.
{
if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
PTR *ptr;

if (!(ptr = attr_get_h(con, name)))
	return PNULL;

// Get or make sym.
SYM *sym;
bool dirty;									// 06/29/03 AM.
if (!(sym = asym_->sym_get(val,dirty)))
	return PNULL;

// Get slot.
p_slot = ptr->v.vptr;

// Add data struct for value.
PTR *pval;
pval = attr_v_add_end(p_slot);

// Fill with string.
pval->v.vst = sym->str;
pval->kind = pST;

return ptr;
}

/**************************************************
*						ATTR_ADD_BY_NAME
* FUN:	attr_add_by_name
* SUBJ:	Add values to an attribute by name.
* CR:		05/03/99 AM.
* RET:	Pointer to new slot.
* NOTE:	Variant.  Takes number value.
*
**************************************************/

PTR *AKBM::attr_add_by_name(
   CON *con,
   _TCHAR *name,
   long long val)			// LONG value.
{
if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
PTR *ptr;

if (!(ptr = attr_get_h(con, name)))
	return PNULL;

// Get slot.
p_slot = ptr->v.vptr;

// Add data struct for value.
PTR *pval;
pval = attr_v_add_end(p_slot);

// Fill with string.
pval->v.vnum = val;
pval->kind = pNUM;

return ptr;
}

bool AKBM::attr_has_n_val(
   CON *con,
   _TCHAR *name,
   long long val)
{
if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
PTR *ptr;

if (!(ptr = attr_get_h(con, name)))
	return PNULL;

// Get slot.
p_slot = ptr->v.vptr;

// Search if value already exists

PTR *vals;
vals = p_slot;
while (vals) {
	if (vals->kind == pNUM && vals->v.vnum == val)
		return true;
   vals = vals->next;
}

return false;
}

/**************************************************
*						ATTR_ADD_BY_NAME
* FUN:	attr_add_by_name
* SUBJ:	Add values to an attribute by name.
* CR:		08/16/01 AM.
* RET:	Pointer to new slot.
* NOTE:	Variant.  Takes FLOAT value.
*
**************************************************/

PTR *AKBM::attr_add_by_name(
   CON *con,
   _TCHAR *name,
   float val)				// FLOAT value.
{
if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
PTR *ptr;

if (!(ptr = attr_get_h(con, name)))
	return PNULL;

// Get slot.
p_slot = ptr->v.vptr;

// Add data struct for value.
PTR *pval;
pval = attr_v_add_end(p_slot);

// Fill with string.
pval->v.vfloat = val;
pval->kind = pFLOAT;

return ptr;
}

/**************************************************
*						ATTR_ADD_BY_NAME
* FUN:	attr_add_by_name
* SUBJ:	Add values to an attribute by name.
* CR:		08/11/00 AM.
* RET:	Pointer to new slot.
* NOTE:	Variant.  Takes concept as value.
*
**************************************************/

PTR *AKBM::attr_add_by_name(
   CON *con,
   _TCHAR *name,
   CON *val)				// Concept value.
{
if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
PTR *ptr;

if (!(ptr = attr_get_h(con, name)))
	return PNULL;

// Get slot.
p_slot = ptr->v.vptr;

// Add data struct for value.
PTR *pval;
pval = attr_v_add_end(p_slot);

// Fill with concept.
pval->v.vcon = val;
pval->kind = pCON;

return ptr;
}

/**************************************************
*						ATTR_RM_BY_NAME
* FUN:	attr_rm_by_name
* SUBJ:	Remove values of the named attribute.
* CR:		05/03/99 AM.
* RET:	True if found, false if not found or other error. (5/25/01 AM.)
*
**************************************************/

bool AKBM::attr_rm_by_name(
   CON *con,
   _TCHAR *name
   )
{
PTR *ptr, *pre;
if (!con || !name || !*name)
	return false;

bool first = false;
pre = attr_prefind_h(con, name, /*UP*/ first);
if (pre)
	{
	ptr = pre->next;				// The attribute handle.
	pre->next = ptr->next;		// Zap attribute from list.
	}
else if (first)					// Deleting first attr is special.
	{
	ptr = con->attrs;
	con->attrs = ptr->next;
	}
else
	return false;					// Couldn't find attr.

// Can wipe out the attribute links and values here, if you like.
// (Note: they are in the PTR table, and are detached from the CG,
// so writing and reading the CG to files will remove all trace of
// these.  May still want to zero out everything, in case someone is
// pointing into them.)
return true;
}


/**************************************************
*						ATTRS_RM
* FUN:	attrs_rm
* SUBJ:	Remove all normal attributes of a concept.
* CR:		10/31/00 AM.
*
**************************************************/

bool AKBM::attrs_rm(
	CON *con,
	/*UP*/
	bool &dirty		// If kb has changed.
   )
{
dirty = false;

if (!con)
	return false;

PTR *ptr = con->attrs;

if (!ptr)	// No attributes is ok.
	return true;

// Go down the list of attrs looking for "keepers".
// Only the ATOM and PHRASE attribute are special and must
// be kept.  (Concept only has one of these at most.)

// If first is a keeper, get rid of the rest of the list.
// (Note: No actual deletions, since all this stuff is in
// tables.  But should probably be zeroed out.)
if (attr_is_sys(ptr))
	{
	ptr->next = PNULL;
	dirty = true;
	return true;
	}

PTR *pre = ptr;
while (pre->next)
	{
	if (attr_is_sys(pre->next))
		{
		// Found the only keeper.
		con->attrs = pre;				// Keep the keeper.
		pre->next->next = PNULL;	// Detach from list.
		pre->next = PNULL;
		// The rest of the list is floating.
		dirty = true;
		return true;
		}
	pre = pre->next;
	}

// Didn't find a keeper, so trash the whole list.
con->attrs = PNULL;
dirty = true;
return true;
}


/**************************************************
*						ATTR_IS_SYS
* FUN:	attrs_is_sys
* SUBJ:	Check if attr is a system internal (ATOM or PHRASE).
* CR:		10/31/00 AM.
*
**************************************************/

bool AKBM::attr_is_sys(
	PTR *ptr
   )
{
if (!ptr)
	return false;

PTR *p_slot = ptr->v.vptr;
if (!p_slot)
	return false;

CON *con = p_slot->v.vcon;		// Slot concept.
//if (con == c_cg_PHRASE || con == c_cg_ATOM)						// 08/15/02 AM.
if (acon_->sysAttr(con))													// 08/15/02 AM.
	return true;
return false;
}


/**************************************************
*						ATTR_RM
* FUN:	attr_rm
* SUBJ:	Remove values of attribute.
* CR:		07/12/99 AM.
* NOTE:	Should actually remove the ptr from list.
*
**************************************************/

bool AKBM::attr_rm(
	CON *con,
   PTR *ptr
   )
{
if (!con || !ptr)
	return false;

PTR *pre;
bool first = false;

pre = attr_prefind_h(con, ptr, /*UP*/ first);
if (first)
	{
	ptr = con->attrs;
	con->attrs = ptr->next;
	}
else if (pre)
	{
	ptr = pre->next;
	pre->next = ptr->next;
	}
else
	return false;			// Not found.
// Todo: Zero out attr and values here, if you like.
return true;
}

/**************************************************
*						ATTR_RENAME
* FUN:	attr_rename
* SUBJ:	Change name of attribute.
* CR:		07/12/99 AM.
* NOTE:	No checking.
*
**************************************************/

bool AKBM::attr_rename(
   PTR *ptr,			// Handle for attribute.
	_TCHAR *rename
   )
{
if (!ptr || !rename || !*rename)
	return false;

// INTERN STRING. Who knows where the given string has been?
// Get or make sym.
CON *slot;
SYM *sym;
bool dirty;									// 06/29/03 AM.
// Make slot concept.
// (Note: NOT registering attribute concept, etc.)
if (!(slot = dict_add_word(rename)))
	{
	sym = asym_->sym_get(rename,dirty);
	slot = sym->con;
	}

PTR *p_slot;

// Get slot.
p_slot = ptr->v.vptr;
p_slot->v.vcon = slot;

return true;
}
/**************************************************
*						ATTR_ADD_NEW
* FUN:	attr_add_new
* SUBJ:	Add new slot and value to given concept.
* CR:		2/18/96 AM.
* RET:	Pointer to new slot.
* NOTE:	Allows redundant slot names.  Adds new slot to end of attrs.
*
**************************************************/

PTR *AKBM::attr_add_new(
   CON *con,
   CON *slot,
   PTR *vals)	/* May be NULL */
{
PTR *pslot;

if (!con || !slot)
   return(PNULL);

pslot = attr_as_add_end(con, slot);
if (!pslot)
   return(PNULL);

pslot->next = vals;
return(pslot);
}


/**************************************************
*						ATTR_ADD_C
* FUN:	attr_add_c
* SUBJ:	Add a concept-valued attribute to given concept.
* CR:		7/21/96 AM.
* RET:	Pointer to val.
* NOTE:	Convenience function.
*
**************************************************/

PTR *AKBM::attr_add_c(
	CON *con,
	CON *slot_con,
	CON *val
	)
{
PVAL pv;

pv.vcon = val;
return attr_add_val(con, slot_con, pv, pCON);
}


/**************************************************
*						ATTR_ADD_N
* FUN:	attr_add_n
* SUBJ:	Add a numeric valued attribute to given concept.
* CR:		6/8/96 AM.
* RET:	Pointer to val.
* NOTE:	Convenience function.
*
**************************************************/

PTR *AKBM::attr_add_n(
	CON *con,
	CON *slot_con,
	long num
	)
{
PVAL pv;

pv.vnum = num;
return attr_add_val(con, slot_con, pv, pNUM);
}


/**************************************************
*						ATTR_ADD_VAL
* FUN:	attr_add_val
* SUBJ:	Add a value of given type to concept's slot.
* CR:		10/9/95 AM.
* RET:	Pointer to val.
* CK:		Caller must check on appropriate match of slot and val.
*
**************************************************/

PTR *AKBM::attr_add_val(
	CON *con,
	CON *slot_con,
	PVAL val,
	PKIND kind
	)
{
PTR *pv;

if (!con || !slot_con)
   return(PNULL);

pv = attr_asv_add_end(con, slot_con);
if (!pv)
   {
   _ftprintf(stderr, _T("[attr_add_con: Couldn't add value.]\n"));
   return(PNULL);
   }

switch (kind)
   {
   case pCON:	 pv->v.vcon   = val.vcon;		break;
   case pPTR:	 pv->v.vptr   = val.vptr;		break;
   case pSYM:	 pv->v.vsym   = val.vsym;		break;
   case pST:	 pv->v.vst    = val.vst;		break;
   case pNUM:	 pv->v.vnum   = val.vnum;		break;
	case pFLOAT: pv->v.vfloat = val.vfloat;	break;				// 08/16/01 AM.
   case pCHAR:	 pv->v.vchar  = val.vchar;		break;
   default:
      _ftprintf(stderr, _T("[attr_add_val: Bad value kind=%d]\n"), kind);
      exit(1);
   }
pv->kind = kind;
return(pv);
}


/**************************************************
*						ATTR_FIND_H
* FUN:	attr_find_h
* SUBJ:	Find specified attr of given concept.
* CR:		07/12/99 AM.
* RET:	Pointer to attribute handle.
* NOTE:	Return first argument that matches.
*			'h' is short for handle.
*
**************************************************/

PTR *AKBM::attr_find_h(CON *con, _TCHAR *name)
{

if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
CON *slot;
_TCHAR *s_slot;
PTR *attrs;
attrs = con->attrs;

while (attrs)
   {
   p_slot = attrs->v.vptr;
   slot   = p_slot->v.vcon;
   s_slot = ACON::con_str(slot);
	if (!_tcscmp(name, s_slot))
		return attrs;
   attrs = attrs->next;
   }
return 0;
}


/**************************************************
*						ATTR_FIND
* FUN:	attr_find
* SUBJ:	Find value(s) for given slot of given concept.
* CR:		08/12/99 AM.
* RET:	Pointer to first value.
*
**************************************************/

PTR *AKBM::attr_find(CON *con, _TCHAR *s_slot)
{
PTR *ptr, *slot;
if ((ptr = attr_find_h(con, s_slot))
	 && (slot = ptr->v.vptr)
	 )
	{
	return slot->next;
	}
return PNULL;
}


/**************************************************
*						ATTR_PREFIND_H
* FUN:	attr_prefind_h
* SUBJ:	Find specified attr of given concept.
* CR:		07/12/99 AM.
* RET:	Pointer to attribute handle of PRIOR attribute.
*			UP first - if attr is first in list.
* NOTE:	Return first argument that matches.
*			'h' is short for handle.
*
**************************************************/

PTR *AKBM::attr_prefind_h(CON *con, _TCHAR *name, /*UP*/ bool &first)
{
first = false;
if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
CON *slot;
_TCHAR *s_slot;
PTR *attrs, *pre;

if (!(attrs = con->attrs))		// FIX.	// 09/24/99 AM.
	return PNULL;					// FIX.	// 09/24/99 AM.

p_slot = attrs->v.vptr;
slot   = p_slot->v.vcon;
s_slot = ACON::con_str(slot);
if (!_tcscmp(name, s_slot))
	{
	first = true;			// First in list.
	return PNULL;
	}

pre = attrs;				// Pointer to previous.
while ((attrs = attrs->next))
   {
   p_slot = attrs->v.vptr;
   slot   = p_slot->v.vcon;
   s_slot = ACON::con_str(slot);
	if (!_tcscmp(name, s_slot))
		return pre;			// Pointer to PREVIOUS.
   pre = attrs;
   }
return PNULL;
}


/**************************************************
*						ATTR_PREFIND_H
* FUN:	attr_prefind_h
* SUBJ:	Find specified attribute for deleting.  VARIANT.
* CR:		07/12/99 AM.
* RET:	Pointer to attribute handle of PRIOR attribute.
*			UP first - if attr is first in list.
* NOTE:	'h' is short for handle.
*
**************************************************/

PTR *AKBM::attr_prefind_h(CON *con, PTR *ptr, /*UP*/ bool &first)
{
first = false;
if (!ptr || !con)
	return PNULL;

PTR *attrs, *pre;

if (!(attrs = con->attrs))		// FIX.	// 09/24/99 AM.
	return PNULL;					// FIX.	// 09/24/99 AM.

if (attrs == ptr)
	{
	first = true;			// First in list.
	return PNULL;
	}

pre = attrs;				// Pointer to previous.
while ((attrs = attrs->next))
   {
	if (attrs == ptr)
		return pre;
   pre = attrs;
   }
return PNULL;
}

/**************************************************
*						ATTR_FIND_SLOT
* FUN:	attr_find_slot
* SUBJ:	Find given slot of given concept, if present.
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
*
**************************************************/

PTR *AKBM::attr_find_slot(CON *con, CON *slot_con)
{
PTR *attrs, *slot;

if (!con || !slot_con)
   return(PNULL);

attrs = con->attrs;
while (attrs)
   {
   slot = attrs->v.vptr;
   if (slot_con == slot->v.vcon)	/* Found the slot concept. */
      return(slot);
   attrs = attrs->next;
   }
return(PNULL);
}


/**************************************************
*						ATTR_FIND_SLOT_0
* FUN:	attr_find_slot_0
* SUBJ:	Find concept's slot with 0 value. (internals).
* CR:		2/18/96 AM.
* RET:	Pointer to the zero value.
* NOTE:	For use in rule-action subsystem.
*
**************************************************/

PTR *AKBM::attr_find_slot_0(CON *con, CON *slot_con)
{
PTR *attrs, *slot, *val;

if (!con || !slot_con)
   return(PNULL);

attrs = con->attrs;
while (attrs)
   {
   slot = attrs->v.vptr;
   if (slot_con == slot->v.vcon)	/* Found the slot concept. */
      {
      if (!(val = slot->next))
         {
         _ftprintf(stderr, _T("[attr_find_slot_0: No value.]\n"));
         return(PNULL);
         }
      if (val->kind != pNUM)
         {
         _ftprintf(stderr, _T("[attr_find_slot_0: Value must be number.]\n"));
         return(PNULL);
         }
      if (val->v.vnum != (long) 0)
         return(PNULL);
      return(val);			/* PTR containing the zero value. */
      }
   attrs = attrs->next;
   }
return(PNULL);
}


/**************************************************
*						ATTR_GET
* FUN:	attr_get
* SUBJ:	Get value(s) for given slot of given concept.
* CR:		10/20/95 AM.
* RET:	Pointer to first value.
*
**************************************************/

PTR *AKBM::attr_get(CON *con, CON *slot)
{
PTR *pslot;

if ((pslot = attr_find_slot(con, slot)))
   return(pslot->next);
return(PNULL);
}

// VARIANT. Slot name is a string.			// 05/03/99 AM.
PTR *AKBM::attr_get(CON *con, _TCHAR *s_slot)
{
PTR *ptr, *slot;
if ((ptr = attr_get_h(con, s_slot))
	 && (slot = ptr->v.vptr)
	 )
	{
	return slot->next;
	}
return PNULL;
}

/**************************************************
*						ATTR_GET_H
* FUN:	attr_get_h
* SUBJ:	Find or make specified attr of given concept.
* CR:		05/03/99 AM.
* RET:	Pointer to first value.
* ARG:	Argument is a string, not a concept.
* NOTE:	Return first argument that matches.
*			'h' is short for handle.
*
**************************************************/

PTR *AKBM::attr_get_h(CON *con, _TCHAR *name)
{

if (!con || !name || !*name)
	return PNULL;

PTR *p_slot;
CON *slot;
PTR *attrs;
bool dirty;									// 06/29/03 AM.

/*
char *s_slot;
attrs = con->attrs;
while (attrs)
   {
   p_slot = attrs->v.vptr;
   slot   = p_slot->v.vcon;
   s_slot = con_str(slot);
	if (!strcmp(name, s_slot))
		return attrs;
   attrs = attrs->next;
   }
*/
if ((attrs = attr_find_h(con, name)))				// 07/12/99 AM.
	return attrs;										// 07/12/99 AM.

// Couldn't find it, so make it.
// (Note: NOT registering attribute concept, etc.)
if (!(slot = dict_add_word(name)))
	{
	SYM *sym;
	sym = asym_->sym_get(name,dirty);
	slot = sym->con;
	}

attrs = attr_a_add_end(con);				// Make new handle.
p_slot = attr_s_add(attrs, slot);	// Make new slot.
return attrs;
}


/**************************************************
*						ATTR_GET_C
* FUN:	attr_get_c
* SUBJ:	Get concept value for given slot of given concept.
* CR:		10/20/95 AM.
* RET:	First value as concept.
* ASS:	Values of this attribute must be concepts.
*
**************************************************/

CON *AKBM::attr_get_c(CON *con, CON *slot)
{
PTR *pval;

if (!(pval = attr_get(con, slot)))
   return(CNULL);
if (pval->kind != pCON)
   {
   _ftprintf(stderr, _T("[attr_get_c: Not concept-valued attr.]\n"));
   return(CNULL);
   }
return(pval->v.vcon);
}


/**************************************************
*						ATTR_GET_N
* FUN:	attr_get_n
* SUBJ:	Get numeric value for given slot of given concept.
* CR:		11/26/95 AM.
* RET:	First value as number, else -1.
* ASS:	Values of this attribute must be numbers.
*
**************************************************/

long AKBM::attr_get_n(CON *con, CON *slot)
{
PTR *pval;

if (!(pval = attr_get(con, slot)))
   return((long) -1);
if (pval->kind != pNUM)
   {
   _ftprintf(stderr, _T("[attr_get_n: Not number-valued attr.]\n"));
   return((long) -1);
   }
return(pval->v.vnum);
}


/**************************************************
*						ATTR_GET_NS
* FUN:	attr_get_ns
* SUBJ:	Get numeric values for given slot of given concept.
* CR:		6/30/96 AM.
* RET:	ok - true if executed correctly.
*			(up) arr - array with numbers filled in.
* ASS:	Caller must ensure long enough array to catch all values.
* CK:		Only checking that first is numeric value.
*			Getting less than the numbers in attr is ok.
*
**************************************************/

bool AKBM::attr_get_ns(CON *con, CON *slot, int len, /*UP*/ long *arr)
{
PTR *pval;
//int ii;
long *parr;

if (len < 0)
	return false;
if (len == 0)
	return true;	/* Nothing to fetch is ok. */
if (!(pval = attr_get(con, slot)))
   return false;
if (pval->kind != pNUM)
   {
   _ftprintf(stderr, _T("[attr_get_ns: Need number-valued attr.]\n"));
   return false;
   }

parr = &(arr[0]);
*parr = pval->v.vnum;
while (--len)
	{
	if (!(pval = pval->next))
		{
		_ftprintf(stderr, _T("[attr_get_ns: Too many nums requested.]\n"));
		return false;
		}
	*++parr = pval->v.vnum;
	}
return true;
}


/**************************************************
*						ATTR_GET_S
* FUN:	attr_get_s
* SUBJ:	Get sym value for given slot of given concept.
* CR:		11/30/95 AM.
* RET:	First value as sym.
* ASS:	Values of this attribute must be numbers.
*
**************************************************/

SYM *AKBM::attr_get_s(CON *con, CON *slot)
{
//SYM *sval;
PTR *pval;

if (!(pval = attr_get(con, slot)))
   return(SNULL);
if (pval->kind != pSYM)
   {
   _ftprintf(stderr, _T("[attr_get_s: Not sym-valued attr.]\n"));
   return(SNULL);
   }
return(pval->v.vsym);
}


/**************************************************
*						ATTR_GET_SLOT
* FUN:	attr_get_slot
* SUBJ:	Find or add given slot of given concept.
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
*
**************************************************/

PTR *AKBM::attr_get_slot(CON *con, CON *slot_con)
{
//PTR *attrs;
PTR *slot;

if (!con || !slot_con)
   return(PNULL);

if ((slot = attr_find_slot(con, slot_con)))
   return(slot);
return(attr_as_add_end(con, slot_con));
}


/**************************************************
*						ATTRS_PRET
* FUN:	attrs_pret
* SUBJ:	Pretty-print attribute list.
* CR:		10/12/95 AM.
*
**************************************************/

void AKBM::attrs_pret(
	PTR *attrs,
	//FILE *out,		// 04/20/99 AM.
	std::_t_ostream *out,		// 04/20/99 AM.
	_TCHAR *indent		/* String for indenting. */
	)
{
PTR *p_slot, *vals;
CON *slot;
//PVAL pv;
_TCHAR *str, *s_slot;

while (attrs)
   {
   p_slot = attrs->v.vptr;
   slot   = p_slot->v.vcon;
   vals   = p_slot->next;
   s_slot = ACON::con_str(slot);
   //fprintf(out, "%s%s ", indent, s_slot);			// 04/20/99 AM.
   //fflush(out);												// 04/20/99 AM.
	*out << indent << s_slot << std::flush;					// 04/20/99 AM.
   while (vals)
      {
      switch (vals->kind)
         {
         case pCON:		str = ACON::con_str(vals->v.vcon);			break;
         case pSYM:		str = vals->v.vsym->str;				break;
         case pST:		str = vals->v.vst;						break;
         case pNUM:		_stprintf(str, _T("%llu"), vals->v.vnum);	break;
         case pFLOAT:	_stprintf(str, _T("%f"), vals->v.vfloat);	break;//08/16/01 AM
         case pCHAR:		_stprintf(str, _T("%c"), vals->v.vchar);	break;
         case pPTR:
         default:
			// 04/20/99 AM.
        //fprintf(stderr, "[attrs_pret: Bad value kind=%d]\n", vals->kind);
		  std::_t_cerr << _T("[attrs_pret: Bad value kind=") << vals->kind << std::endl;
            exit(1);
         }
      //fprintf(out, "%s ", str);	// 04/20/99 AM.
		*out << str;						// 04/20/99 AM.
      vals = vals->next;
      }
   //fprintf(out, "\n");		// 04/20/99 AM.
	*out << std::endl;					// 04/20/99 AM.
   attrs = attrs->next;
   }
}


/************************************************************************/
/****					MICRO-MANAGEMENT FUNCTIONS					 ****/
/************************************************************************/



/**************************************************
*						ATTR_A_ADD_END
* FUN:	attr_a_add_end
* SUBJ:	Add an attr struct to end of the given concept's attr list.
* CR:		10/9/95 AM.
* RET:	Pointer to attr
* NOTE:	Modularity fn.
* OPT:	Could be a macro.
*
**************************************************/

PTR *AKBM::attr_a_add_end(CON *con)
{
if (con)
   return(aptr_->ptr_add_end(&(con->attrs)));
return(PNULL);
}


/**************************************************
*						ATTR_AS_ADD_END
* FUN:	attr_as_add_end
* SUBJ:	Add an attr and slot at end of concept's attrs.
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
* CK:		Not checking if this attr slot already exists.
*
**************************************************/

PTR *AKBM::attr_as_add_end(
	CON *con,		/* Concept to add attribute to. */
   CON *slot_con	/* Slot concept.				*/
   )
{
PTR *attr;

if (!con || !slot_con)
   return(PNULL);

attr = attr_a_add_end(con);
if (!attr)
   {
   _ftprintf(stderr, _T("[attr_as_add_end: Error.]\n"));
   return(PNULL);
   }
return(attr_s_add(attr, slot_con));
}


/**************************************************
*						ATTR_ASV_ADD_END
* FUN:	attr_asv_add_end
* SUBJ:	Add attr, slot, and val PTR structures to given concept.
* CR:		10/9/95 AM.
* RET:	Pointer to val.
* CK:		Checking if this attr slot already exists.
* ASS:	Don't care if slot exists.  Merely add new value.
* NOTE:	"end" means add the value at the end.  If absent, slot also
*			added at the end.
*
**************************************************/

PTR *AKBM::attr_asv_add_end(
	CON *con,		/* Concept to add attribute to. */
   CON *slot_con	/* Slot concept.				*/
   )
{
PTR *slot;

slot = attr_get_slot(con, slot_con);
return(attr_v_add_end(slot));
}

/**************************************************
*						ATTR_S_ADD
* FUN:	attr_s_add
* SUBJ:	Add a slot to the given concept attr.
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
* NOTE:	Attr's value is a pointer to the slot!
* CK:		Not checking if this attr slot already exists.
*			Not checking for prior values of attr.
*
**************************************************/

PTR *AKBM::attr_s_add(
	PTR *attr,
	CON *slot_con		/* Concept representing slot, if any.	*/
	)
{
PTR *slot;

if (!attr || !slot_con)
   return(PNULL);

slot = aptr_->ptr_alloc();

attr->kind = pPTR;
attr->v.vptr = slot;
slot->kind = pCON;	/* Slot kind is always a concept.	*/
slot->v.vcon = slot_con;
return(slot);
}


/**************************************************
*						ATTR_V_ADD_END
* FUN:	attr_v_add_end
* SUBJ:	Add a value at end of vals, for the given attr slot.
* CR:		10/9/95 AM.
* RET:	Pointer to value.
* NOTE:	User must fill val struct with appropriate kind, pval.
* CK:		Not checking on given slot.
*
**************************************************/

PTR *AKBM::attr_v_add_end(PTR *slot)
{
PTR *val, *vals;

if (!slot)
   return(PNULL);

val = aptr_->ptr_alloc();
vals = slot;
while (vals->next)	/* Get last val in list. */
   vals = vals->next;
vals->next = val;
return(val);
}


/**************************************************
*						ATTRS_NAME
* FUN:	attrs_name
* SUBJ:	Get name of attribute.
* CR:		07/05/99 AM.
*
**************************************************/

_TCHAR *AKBM::attrs_name(
	PTR *attrs
	)
{
PTR *p_slot;
CON *slot;
_TCHAR *s_slot;

if (!attrs)
	return 0;

p_slot = attrs->v.vptr;
slot   = p_slot->v.vcon;
s_slot = ACON::con_str(slot);

return s_slot;
}



/**************************************************
*						ATTRS_VALS
* FUN:	attrs_name
* SUBJ:	Get values list of attribute.
* CR:		07/05/99 AM.
*
**************************************************/

PTR *AKBM::attrs_vals(
	PTR *attrs
	)
{
PTR *p_slot;
CON *slot;

if (!attrs)
	return 0;

p_slot = attrs->v.vptr;
slot   = p_slot->v.vcon;
return p_slot->next;
}


/**************************************************
*						RM_ONE_VAL
* FUN:	rm_one_val
* SUBJ:	Remove given val from its attribute.
* CR:		05/11/00 AM.
*
**************************************************/

bool AKBM::rm_one_val(
	PTR *attr,
   PTR *val
   )
{
if (!attr || !val)
	return false;

// Get attributes values.
PTR *vals = attrs_vals(attr);
if (!vals)
	return false;

// Special case if val is first in list.
PTR *p_slot;
if (vals == val)			// Matches first in list.
	{
	p_slot = attr->v.vptr;
	p_slot->next = p_slot->next->next;
	val->next = 0;		// Remove val from the list.
	// Todo: If val was the only value on the list, could rm the attr here.
	return true;
	}

// Look for val as next in list.
while (vals && (vals->next != val))
	vals = vals->next;
if (!vals)
	return false;		// Val wasn't on the list of values.

// vals->next == val;
vals->next = vals->next->next;
val->next = 0;
return true;
}


/**************************************************
*						ATTR_RM_VAL_BY_NAME
* FUN:	attr_rm_val_by_name
* SUBJ:	Remove value of the named attribute.
* CR:		05/03/99 AM.
* NOTE:	String type only, looks for first instance.
*
**************************************************/

bool AKBM::attr_rm_val_by_name(
   CON *con,
   _TCHAR *attr_s,
	_TCHAR *val_s
   )
{
if (!con || !attr_s || !*attr_s || !val_s || !*val_s)
	return false;

PTR *p_slot;
PTR *ptr, *pre;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre = attr_prefind_h(con, attr_s, /*UP*/ first);
if (pre)
	ptr = pre->next;				// The attribute handle.
else if (first)					// Deleting first attr is special.
	ptr = con->attrs;
else
	return false;					// Couldn't find attr.


// Get or make sym.
SYM *sym;
bool dirty;									// 06/29/03 AM.
if (!(sym = asym_->sym_get(val_s,dirty)))
	return false;
_TCHAR *str = sym->str;

// Get slot.
p_slot = ptr->v.vptr;

// Find matching value, if any, and delete.
PTR *pval, *preval;
preval = p_slot;
while ((pval = preval->next))
	{
	if (pval->kind == pST && pval->v.vst
		 && !_tcscmp(str, pval->v.vst))
		{
		// Remove val.
		preval->next = preval->next->next;
		pval->next = PNULL;
		// If no other vals, delete the whole attr here.
		if (!p_slot->next)
			{
			if (pre)
				pre->next = ptr->next;		// Zap attribute from list.
			else if (first)					// Deleting first attr is special.
				con->attrs = ptr->next;
			}
		return true;
		}
	preval = pval;		// Move to next value.
	}

return true;
}


/**************************************************
*						ATTR_RM_VAL_BY_NAME
* FUN:	attr_rm_val_by_name
* SUBJ:	Remove value of the named attribute.
* CR:		05/19/99 AM.
* NOTE:	Long int variant, looks for first instance.
*
**************************************************/

bool AKBM::attr_rm_val_by_name(
   CON *con,
   _TCHAR *attr_s,
	long long val
   )
{
if (!con || !attr_s || !*attr_s
//	|| !val			// val == 0 is OK!						// FIX.	// 06/19/02 AM.
	)
	return false;

PTR *p_slot;
PTR *ptr, *pre;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre = attr_prefind_h(con, attr_s, /*UP*/ first);
if (pre)
	ptr = pre->next;				// The attribute handle.
else if (first)					// Deleting first attr is special.
	ptr = con->attrs;
else
	{
	// SPECIAL.  Since numeric 0 equates to NULL, going to allow this as
	// ok deletion.
	if (val == 0)																// 06/19/02 AM.
		return true;															// 06/19/02 AM.
	return false;					// Couldn't find attr.
	}

// Get slot.
p_slot = ptr->v.vptr;

// Find matching value, if any, and delete.
PTR *pval, *preval;
preval = p_slot;
while ((pval = preval->next))
	{
	if (pval->kind == pNUM
		 && (val == pval->v.vnum))
		{
		// Remove val.
		preval->next = preval->next->next;
		pval->next = PNULL;
		// If no other vals, delete the whole attr here.
		if (!p_slot->next)
			{
			if (pre)
				pre->next = ptr->next;		// Zap attribute from list.
			else if (first)					// Deleting first attr is special.
				con->attrs = ptr->next;
			}
		return true;
		}
	preval = pval;		// Move to next value.
	}

return true;
}



/**************************************************
*						ATTR_RM_VAL_BY_NAME
* FUN:	attr_rm_val_by_name
* SUBJ:	Remove value of the named attribute.
* CR:		03/19/03 AM.
* NOTE:	Float variant, looks for first instance.
*
**************************************************/

bool AKBM::attr_rm_val_by_name(
   CON *con,
   _TCHAR *attr_s,
	float val
   )
{
if (!con || !attr_s || !*attr_s
//	|| !val			// val == 0 is OK!						// FIX.	// 06/19/02 AM.
	)
	return false;

PTR *p_slot;
PTR *ptr, *pre;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre = attr_prefind_h(con, attr_s, /*UP*/ first);
if (pre)
	ptr = pre->next;				// The attribute handle.
else if (first)					// Deleting first attr is special.
	ptr = con->attrs;
else
	{
	// SPECIAL.  Since numeric 0 equates to NULL, going to allow this as
	// ok deletion.
	if (val == 0)																// 06/19/02 AM.
		return true;															// 06/19/02 AM.
	return false;					// Couldn't find attr.
	}

// Get slot.
p_slot = ptr->v.vptr;

// Find matching value, if any, and delete.
PTR *pval, *preval;
preval = p_slot;
while ((pval = preval->next))
	{
	if (pval->kind == pNUM
		 && (val >= (pval->v.vfloat - 0.0001)
		     && val <= (pval->v.vfloat + 0.0001)	// Some kinda tolerance.
		 ))
		{
		// Remove val.
		preval->next = preval->next->next;
		pval->next = PNULL;
		// If no other vals, delete the whole attr here.
		if (!p_slot->next)
			{
			if (pre)
				pre->next = ptr->next;		// Zap attribute from list.
			else if (first)					// Deleting first attr is special.
				con->attrs = ptr->next;
			}
		return true;
		}
	preval = pval;		// Move to next value.
	}

return true;
}


/**************************************************
*						ATTR_RM_VAL_BY_CONCEPT
* FUN:	attr_rm_val_by_concept
* SUBJ:	Remove concept value of the named attribute.
* CR:		06/03/02 Dd.
* NOTE:	Concept variant, looks for first instance.
*
**************************************************/

bool AKBM::attr_rm_val_by_concept(
   CON *con,
   _TCHAR *attr_s,
	CON *val
   )
{
if (!con || !attr_s || !*attr_s || !val)
	return false;

PTR *p_slot;
PTR *ptr, *pre;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre = attr_prefind_h(con, attr_s, /*UP*/ first);
if (pre)
	ptr = pre->next;				// The attribute handle.
else if (first)					// Deleting first attr is special.
	ptr = con->attrs;
else
	return false;					// Couldn't find attr.

// Get slot.
p_slot = ptr->v.vptr;

// Find matching value, if any, and delete.
PTR *pval, *preval;
preval = p_slot;
while ((pval = preval->next))
	{
	if (pval->kind == pCON
		 && (val == pval->v.vcon))
		{
		// Remove val.
		preval->next = preval->next->next;
		pval->next = PNULL;
		// If no other vals, delete the whole attr here.
		if (!p_slot->next)
			{
			if (pre)
				pre->next = ptr->next;		// Zap attribute from list.
			else if (first)					// Deleting first attr is special.
				con->attrs = ptr->next;
			}
		return true;
		}
	preval = pval;		// Move to next value.
	}

return true;
}
