/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ATTR.CPP
* FILE:	cs/libqconsh/attr.cpp
* CR:		02/16/07 AM.
* SUBJ:	API class for managing CG KBMS attributes of concepts.
* NOTE:	Overhauls for QDBM.
*			Moving many of the attr.cpp functions from libkbm to here.
*			libkbm/attr.cpp no longer present.
*
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
* FILE:	libkbm/attr.c
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
#include <time.h>
#include "machine-min.h"
#include "prim/libprim.h"

#include "StdAfx.h"
#include <time.h>
#include "machine-min.h"
#include "prim/libprim.h"

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xcon_s.h"
#include "qkbm/xptr_s.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"

#include "attr.h"


int CGATTR::count_ = 0;



/********************************************
* FN:		Special functions for the class
********************************************/

CGATTR::CGATTR(
	CG *cg	// Want the kbms available to attr manager.
	)
{
cg_ = cg;

#ifndef STABLE_
++count_;
#endif
}

CGATTR::~CGATTR()
{
#ifndef STABLE_
--count_;
#endif
}


/************************************************************************/
/****					MICRO-MANAGEMENT FUNCTIONS					 ****/
/************************************************************************/

// Not using the micro-mgmt functions in QDBM implementation.	// 02/16/07 AM.

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

/**************************************************
*						ATTR_ADD_BY_NAME
* FUN:	attr_add_by_name
* SUBJ:	Add string value to an attribute by name.
* CR:		05/03/99 AM.
* RET:	pid - ptr id for new value.
* NOTE:	02/16/07 AM. REIMPLEMENT FOR USING QDBM.
*
**************************************************/

PTR_ID CGATTR::attr_add_by_name(
   CON_ID cid,
   _TCHAR *name,
   _TCHAR *val)			// STRING value.
{
if (!cid || !name || !*name)
	return 0;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return 0;

bool dirty = false;
STR_ID slot_sid = cg_->qkbm_->sym_get(name,dirty);
if (!slot_sid)
	{
	cg_->kbfree(con);	// 03/05/07 AM.
	return 0;
	}

// Allowing value to be null.
STR_ID val_sid = cg_->qkbm_->sym_get(val,dirty);


// Get end pointer to add new value to.
PTR_ID pid = 0;
XPTR_S *ptr = cg_->qkbm_->attr_get_last_ptr(con, slot_sid,/*UP*/ pid);	// 02/16/07 AM.
cg_->kbfree(con);	// 03/05/07 AM.
if (!ptr)
	return 0;

XPVAL vv;
vv.xstr = val_sid;

PTR_ID val_pid = 0;
XPTR_S *val_ptr = cg_->qkbm_->attr_add_val(ptr, pid, vv, pXSTR, /*UP*/ val_pid);
cg_->kbfree(ptr);	// Free up copy of object.
cg_->kbfree(val_ptr);
return val_pid;
}


/**************************************************
*						ATTR_ADD_BY_NAME_S
* FUN:	attr_add_by_name_s
* SUBJ:	Add string value to an attribute by name.
* CR:		02/26/07 AM.
* RET:	pid - ptr id for new value.
*
**************************************************/

PTR_ID CGATTR::attr_add_by_name_s(
   CON_ID cid,
   _TCHAR *name,
   STR_ID val)			
{
if (!cid || !name || !*name)
	return 0;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return 0;

bool dirty;
STR_ID slot_sid = cg_->qkbm_->sym_get(name,dirty);
if (!slot_sid)
	{
	cg_->kbfree(con);	// 03/05/07 AM.
	return 0;
	}

// Get end pointer to add new value to.
PTR_ID pid = 0;
XPTR_S *ptr = cg_->qkbm_->attr_get_last_ptr(con, slot_sid,/*UP*/ pid);	// 02/16/07 AM.
cg_->kbfree(con);	// 03/05/07 AM.
if (!ptr)
	return 0;

XPVAL vv;
vv.xstr = val;

PTR_ID val_pid = 0;
XPTR_S *val_ptr = cg_->qkbm_->attr_add_val(ptr, pid, vv, pXSTR, /*UP*/ val_pid);
cg_->kbfree(ptr);	// Free up copy of object.
cg_->kbfree(val_ptr);
return val_pid;
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

PTR_ID CGATTR::attr_add_by_name(
   CON_ID cid,
   _TCHAR *name,
   long val)			// LONG value.
{
if (!cid || !name || !*name)
	return 0;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return 0;

bool dirty;
STR_ID slot_sid = cg_->qkbm_->sym_get(name,dirty);
if (!slot_sid)
	{
	cg_->kbfree(con);	// 03/01/07 AM.
	return 0;
	}

// Get end pointer to add new value to.
PTR_ID pid = 0;
XPTR_S *ptr = cg_->qkbm_->attr_get_last_ptr(con, slot_sid,/*UP*/ pid);	// 02/16/07 AM.
cg_->kbfree(con);	// 03/01/07 AM.
if (!ptr)
	return 0;

XPVAL vv;
vv.vnum = val;

PTR_ID val_pid = 0;
XPTR_S *val_ptr = cg_->qkbm_->attr_add_val(ptr, pid, vv, pNUM, /*UP*/ val_pid);
cg_->kbfree(ptr);	// Free up copy of object.
cg_->kbfree(val_ptr);
return val_pid;
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

PTR_ID CGATTR::attr_add_by_name(
   CON_ID cid,
   _TCHAR *name,
   float val)				// FLOAT value.
{
if (!cid || !name || !*name)
	return 0;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return 0;

bool dirty;
STR_ID slot_sid = cg_->qkbm_->sym_get(name,dirty);
if (!slot_sid)
	{
	cg_->kbfree(con);	// 03/01/07 AM.
	return 0;
	}

// Get end pointer to add new value to.
PTR_ID pid = 0;
XPTR_S *ptr = cg_->qkbm_->attr_get_last_ptr(con, slot_sid,/*UP*/ pid);	// 02/16/07 AM.
cg_->kbfree(con);	// 03/01/07 AM.
if (!ptr)
	return 0;

XPVAL vv;
vv.vfloat = val;

PTR_ID val_pid = 0;
XPTR_S *val_ptr = cg_->qkbm_->attr_add_val(ptr, pid, vv, pFLOAT, /*UP*/ val_pid);
cg_->kbfree(ptr);	// Free up copy of object.
cg_->kbfree(val_ptr);
return val_pid;
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

PTR_ID CGATTR::attr_add_by_name(
   CON_ID cid,
   _TCHAR *name,
   CON_ID val)				// Concept value.
{
if (!cid || !name || !*name)
	return 0;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return 0;

bool dirty;
STR_ID slot_sid = cg_->qkbm_->sym_get(name,dirty);
if (!slot_sid)
	{
	cg_->kbfree(con);	// 03/01/07 AM.
	return 0;
	}

// Allowing value to be null.

// Get end pointer to add new value to.
PTR_ID pid = 0;
XPTR_S *ptr = cg_->qkbm_->attr_get_last_ptr(con, slot_sid,/*UP*/ pid);	// 02/16/07 AM.
cg_->kbfree(con);	// 03/01/07 AM.
if (!ptr)
	return 0;

XPVAL vv;
vv.xcon = val;

PTR_ID val_pid = 0;
XPTR_S *val_ptr = cg_->qkbm_->attr_add_val(ptr, pid, vv, pXCON, /*UP*/ val_pid);
cg_->kbfree(ptr);	// Free up copy of object.
cg_->kbfree(val_ptr);
return val_pid;
}

/**************************************************
*						ATTR_RM_BY_NAME
* FUN:	attr_rm_by_name
* SUBJ:	Remove values of the named attribute.
* CR:		05/03/99 AM.
* RET:	True if found, false if not found or other error. (5/25/01 AM.)
*
**************************************************/

bool CGATTR::attr_rm_by_name(
   CON_ID cid,
   _TCHAR *name,
   /*UP*/
   bool &dirty	// If kb has changed.	// 02/16/07 AM.
   )
{
dirty = false;

if (!cid || !name || !*name)
	return false;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return false;

bool first = false;
PTR_ID pre_pid;
PTR_ID hpid;
PTR_ID slot_pid;
XPTR_S *pre_ptr = cg_->qkbm_->attr_prefind_h(con,name, /*UP*/ first, pre_pid, hpid, slot_pid);

if (!pre_ptr && !first)
	{
	cg_->kbfree(con);
	return false;
	}

dirty = true;

// Handle to be deleted.
XPTR_S *ptr;

if (first)
	{
	// Consistency check.
	if (con->attrs != hpid)
		{
		cg_->kbfree(con);
		return false;
		}
	}
else
	{
	// Consistency check.
	if (pre_ptr->next != hpid)
		{
		cg_->kbfree(con);
		return false;
		}
	}

// Fetch handle to be deleted.
if (!cg_->qkbm_->getXptr(hpid, /*UP*/ ptr))
	{
	cg_->kbfree(con);
	if (pre_ptr)
		cg_->kbfree(pre_ptr);
	return false;
	}

// Fetch one past the handle.
PTR_ID past_pid = ptr->next;
//XPTR_S *past_ptr;
//if (past_pid && !cg_->qkbm_->getXptr(past_pid, /*UP*/ past_ptr))
//	{
//	cg_->kbfree(con);
//	if (pre_ptr)
//		cg_->kbfree(pre_ptr);
//	cg_->kbfree(ptr);
//	return false;
//	}

// Fix up handle pointers.
if (first)
	{
	con->attrs = past_pid;
	cg_->qkbm_->con_update(con->id, con);
	}
else	// if pre_ptr
	{
	pre_ptr->next = past_pid;
	cg_->qkbm_->ptr_update(pre_pid, pre_ptr);
	cg_->kbfree(pre_ptr);	// Free up copy.
	pre_ptr = 0;	// FIX.	// 03/12/07 AM.
	}
cg_->kbfree(con);	// Delete copy of concept.

ptr->next = 0;	// Just delete the handle ptr.
cg_->qkbm_->ptr_list_rm(hpid,ptr);
cg_->kbfree(ptr);	// FREE THIS COPY.	// FIX.	// 03/20/07 AM.

// Delete slot and values.
XPTR_S *slot;	// 03/11/07 AM.
if (!cg_->qkbm_->getXptr(slot_pid, /*UP*/ slot))	// 03/11/07 AM.
	return false;
cg_->qkbm_->ptr_list_rm(slot_pid,slot);	// 03/11/07 AM.
cg_->kbfree(slot);

if (pre_ptr)
	cg_->kbfree(pre_ptr);
return true;
}

/**************************************************
*						ATTR_IS_SYS
* FUN:	attrs_is_sys
* SUBJ:	Check if attr is a system internal (ATOM or PHRASE).
* CR:		10/31/00 AM.
*
**************************************************/

bool CGATTR::attr_is_sys(
	XPTR_S *ptr
   )
{
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

bool CGATTR::attr_rm(
	CON_ID cid,
   PTR_ID pid	// Id of the attr handle to be removed.
   )
{
if (!cid || !pid)
	return false;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return false;

bool first = false;
PTR_ID pre_pid = 0;
XPTR_S *pre_ptr = cg_->qkbm_->attr_prefind_h(con, pid, /*UP*/ first, pre_pid);


if (!pre_ptr && !first)
	{
	cg_->kbfree(con);
	return false;
	}

// CONSISTENCY CHECK. // 02/16/07 AM.
if (first)
	{
	if (pid != con->attrs)	// First attr pointed to by concept.
		{
		cg_->kbfree(con);
		return false;
		}
	}
else
	{
	if (pid != pre_ptr->next)
		{
		cg_->kbfree(con);
		return false;
		}
	}

// Fetch handle to be deleted.
XPTR_S *ptr;
if (!cg_->qkbm_->getXptr(pid, /*UP*/ ptr))
	{
	cg_->kbfree(con);
	if (pre_ptr)
		cg_->kbfree(pre_ptr);
	return false;
	}

// Fetch one past the handle.
PTR_ID past_pid = ptr->next;
XPTR_S *past_ptr;
if (past_pid && !cg_->qkbm_->getXptr(past_pid, /*UP*/ past_ptr))
	{
	cg_->kbfree(con);
	if (pre_ptr)
		cg_->kbfree(pre_ptr);
	cg_->kbfree(ptr);
	return false;
	}

// Fix up handle pointers.
if (first)
	{
	con->attrs = past_pid;
	cg_->qkbm_->con_update(con->id, con);
	}
else	// if pre_ptr
	{
	pre_ptr->next = past_pid;
	cg_->qkbm_->ptr_update(pre_pid, pre_ptr);
	cg_->kbfree(pre_ptr);	// Free up copy.
	}

// Now delete handle, slot and values.
cg_->qkbm_->ptr_list_rm(pid,ptr);
cg_->kbfree(ptr);	// FREE THIS COPY.	// FIX.	// 03/20/07 AM.
return true;

if (pre_ptr)
	cg_->kbfree(pre_ptr);
cg_->kbfree(con);	// Delete copy of concept.
return true;
}

/**************************************************
*						ATTR_RM
* FUN:	attr_rm
* SUBJ:	Remove an attribute from a concept. [VARIANT]
* CR:		02/17/07 AM.
* NOTE:	A convenience utility.
*
**************************************************/

bool CGATTR::attr_rm(
	XCON_S *con,	// Concept to be modified.
   XPTR_S *pre,	// Handle before the one to be freed.
   PTR_ID pre_pid,// Id of the before handle.
   bool first		// If freeing the first attribute of concept.
   )
{
if (!con)
	return false;

if (!pre && !first)
	return false;

PTR_ID hpid;	// Handle to be freed.
XPTR_S *hptr;
PTR_ID hafter;	// Handle after that being freed.
PTR_ID slot_pid;	// Id for slot to be removed.
if (pre)
	{
	hpid = pre->next;
	if (!cg_->qkbm_->getXptr(hpid, /*UP*/ hptr))
		return false;
	hafter = hptr->next;
	slot_pid = hptr->v.xptr;
	if (slot_pid)
		cg_->qkbm_->ptr_list_rm(slot_pid, 0);

	pre->next = hptr->next;
	cg_->qkbm_->ptr_update(pre_pid,pre);

	hptr->next = 0;
	cg_->qkbm_->ptr_list_rm(hpid,hptr);	// Free just the handle.
	cg_->kbfree(hptr);
	return true;
	}

// Remove the first attr of concept.
hpid = con->attrs;
if (!cg_->qkbm_->getXptr(hpid, /*UP*/ hptr))
	return false;
hafter = hptr->next;
slot_pid = hptr->v.xptr;
if (slot_pid)
	cg_->qkbm_->ptr_list_rm(slot_pid, 0);

con->attrs = hptr->next;
cg_->qkbm_->con_update(con->id, con);

hptr->next = 0;
cg_->qkbm_->ptr_list_rm(hpid,hptr);	// Free just the handle.
cg_->kbfree(hptr);
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

bool CGATTR::attr_rename(
   PTR_ID hpid,			// Handle for attribute.
	_TCHAR *rename
   )
{
if (!hpid || !rename || !*rename)
	return false;

bool dirty;
STR_ID sid = cg_->qkbm_->sym_get(rename,dirty);
if (!sid)
	return false;

// Fetch handle.
XPTR_S *hptr;
if (!cg_->qkbm_->getXptr(hpid, /*UP*/ hptr))
	return false;

PTR_ID spid = hptr->v.xptr;	// Id for slot ptr.
cg_->kbfree(hptr);

// Fetch slot ptr.
XPTR_S *sptr;
if (!cg_->qkbm_->getXptr(spid, /*UP*/ sptr))
	return false;

// Whatever slot was (con or str), change to string.
sptr->kind = pXSTR;
sptr->v.xstr = sid;

// Update slot pointer to db.
cg_->qkbm_->ptr_update(spid, sptr);
cg_->kbfree(sptr);

return true;
}

/**************************************************
*						ATTR_ADD_NEW
* FUN:	attr_add_new
* SUBJ:	Add new slot and value to given concept.
* CR:		2/18/96 AM.
* RET:	Pointer to new slot.
* NOTE:	Allows redundant slot names.  Adds new slot to end of attrs.
*			02/16/07 AM. Unused at present in VisualText, but a hook for
*			using concept names as slots.
*			03/05/07 AM. OBSOLETE? Doesn't appear to be called anywhere.
*
**************************************************/

XPTR_S *CGATTR::attr_add_new(
   XCON_S *con,
   XCON_S *slot,
   XPTR_S *vals)	/* May be NULL */
{
return 0;	// Deferring re-implement.	// 02/16/07 AM.
/*
XPTR_S *pslot;

if (!con || !slot)
   return(0);

//pslot = attr_as_add_end(con, slot);
if (!pslot)
   return(0);

pslot->next = vals;
return(pslot);
*/
}


/**************************************************
*						ATTR_ADD_C
* FUN:	attr_add_c
* SUBJ:	Add a concept-valued attribute to given concept.
* CR:		7/21/96 AM.
* RET:	Pointer to val.
* NOTE:	Convenience function.
*			Hook for concepts as slot names. 02/16/07 AM.
*
**************************************************/

XPTR_S *CGATTR::attr_add_c(
	XCON_S *con,
	XCON_S *slot_con,
	XCON_S *val
	)
{
return 0;	// 02/16/07 AM.
/*
XPVAL pv;

pv.xcon = val;
return attr_add_val(con, slot_con, pv, pXCON);
*/
}


/**************************************************
*						ATTR_ADD_N
* FUN:	attr_add_n
* SUBJ:	Add a numeric valued attribute to given concept.
* CR:		6/8/96 AM.
* RET:	Pointer to val.
* NOTE:	Convenience function.
*			Hook for concepts as slot names.	// 02/16/07 AM.
*
**************************************************/

XPTR_S *CGATTR::attr_add_n(
	XCON_S *con,
	XCON_S *slot_con,
	long num
	)
{
return 0;	// 02/16/07 AM.
/*
XPVAL pv;

pv.vnum = num;
return attr_add_val(con, slot_con, pv, pNUM);
*/
}


/**************************************************
*						ATTR_ADD_VAL
* FUN:	attr_add_val
* SUBJ:	Add a value of given type to concept's slot.
* CR:		10/9/95 AM.
* RET:	Pointer to val.
* CK:		Caller must check on appropriate match of slot and val.
* NOTE:	REIMPLEMENT.	02/16/07 AM.
* WARN:	CALLER MUST FREE RETURNED COPY OF PTR.
*
**************************************************/

XPTR_S *CGATTR::attr_add_val(
	XCON_S *con,
	XCON_S *slot_con,
	XPVAL val,
	XPKIND kind
	)
{

if (!con || !slot_con || !kind)
   return(0);

// Get ptr for adding value.
PTR_ID pid;
XPTR_S *ptr = cg_->qkbm_->attr_get_last_ptr(con, slot_con,/*UP*/ pid);
if (!ptr)
	return 0;

PTR_ID val_pid;
XPTR_S *val_ptr = cg_->qkbm_->attr_add_val(ptr, pid, val, kind, /*UP*/ val_pid);
cg_->kbfree(ptr);	// 03/05/07 AM.
return val_ptr;
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

XPTR_S *CGATTR::attr_find_slot_0(XCON_S *con, XCON_S *slot_con)
{
return 0;	// 02/17/07 AM.

/*
XPTR_S *attrs, *slot, *val;

if (!con || !slot_con)
   return(0);

attrs = con->attrs;
while (attrs)
   {
   slot = attrs->v.xptr;
   if (slot_con == slot->v.xcon)	// Found the slot concept.
      {
      if (!(val = slot->next))
         {
         _ftprintf(stderr, _T("[attr_find_slot_0: No value.]\n"));
         return(0);
         }
      if (val->kind != pNUM)
         {
         _ftprintf(stderr, _T("[attr_find_slot_0: Value must be number.]\n"));
         return(0);
         }
      if (val->v.vnum != (long) 0)
         return(0);
      return(val);			// PTR containing the zero value.
      }
   attrs = attrs->next;
   }
return(0);
*/
}


/**************************************************
*						ATTR_GET
* FUN:	attr_get
* SUBJ:	Get value(s) for given slot of given concept.
* CR:		10/20/95 AM.
* RET:	Pointer to first value.
* WARN:	USER MUST FREE THE RETURNED PTR.
*
**************************************************/

XPTR_S *CGATTR::attr_get(XCON_S *con, XCON_S *slot,/*UP*/ PTR_ID &val_pid)
{
val_pid = 0;
if (!con || !slot)
	return 0;

XPTR_S *pslot;
PTR_ID pid;
if (pslot = cg_->qkbm_->attr_find_slot(con, slot->id,/*UP*/pid))
	{
	val_pid = pslot->next;
	XPTR_S *val;
	cg_->qkbm_->getXptr(val_pid,/*UP*/val);
	cg_->kbfree(pslot);
   return val;
   }
return 0;
}

// VARIANT. Slot name is a string.			// 05/03/99 AM.
XPTR_S *CGATTR::attr_get(XCON_S *con, _TCHAR *s_slot, /*UP*/ PTR_ID &val_pid)
{
val_pid = 0;
if (!con || !s_slot || !*s_slot)
	return 0;

STR_ID sid = cg_->qkbm_->getStrx(s_slot);
if (!sid)	// String not in db yet.
	return 0;

XPTR_S *pslot;
PTR_ID pid;
if (pslot = cg_->qkbm_->attr_find_slot_s(con, sid,/*UP*/pid))
	{
	val_pid = pslot->next;
	XPTR_S *val;
	cg_->qkbm_->getXptr(val_pid,/*UP*/val);
	cg_->kbfree(pslot);
   return val;
   }
return 0;
}


/**************************************************
*						ATTR_GET_C
* FUN:	attr_get_c
* SUBJ:	Get concept value for given slot of given concept.
* CR:		10/20/95 AM.
* RET:	First value as concept.
* ASS:	Values of this attribute must be concepts.
* NOTE:	REIMPLEMENT.	// 02/17/07 AM.
*
**************************************************/

CON_ID CGATTR::attr_get_c(XCON_S *con, XCON_S *slot)
{
XPTR_S *pval;
PTR_ID val_pid;
if (!(pval = attr_get(con, slot, /*UP*/ val_pid)))
   return 0;
if (pval->kind != pXCON)
   {
   _ftprintf(stderr, _T("[attr_get_c: Not concept-valued attr.]\n"));
   cg_->kbfree(pval);
   return 0;
   }
CON_ID cid = pval->v.xcon;
cg_->kbfree(pval);
return cid;
}


/**************************************************
*						ATTR_GET_N
* FUN:	attr_get_n
* SUBJ:	Get numeric value for given slot of given concept.
* CR:		11/26/95 AM.
* RET:	First value as number.
* ASS:	Values of this attribute must be numbers.
*
**************************************************/

long CGATTR::attr_get_n(XCON_S *con, XCON_S *slot)
{
XPTR_S *pval;
PTR_ID val_pid;
if (!(pval = attr_get(con, slot, /*UP*/ val_pid)))
   return 0;
if (pval->kind != pNUM)
   {
   _ftprintf(stderr, _T("[attr_get_n: Not number-valued attr.]\n"));
   cg_->kbfree(pval);
   return 0;
   }
long num = pval->v.vnum;
cg_->kbfree(pval);
return num;
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

bool CGATTR::attr_get_ns(XCON_S *con, XCON_S *slot, int len, /*UP*/ long *arr)
{
// PROBABLY OBSOLETE.	// 02/17/07 AM.
return false;

#ifdef _OBSO_070217_
XPTR_S *pval;
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
//	if (!(pval = pval->next))
		{
		_ftprintf(stderr, _T("[attr_get_ns: Too many nums requested.]\n"));
		return false;
		}
	*++parr = pval->v.vnum;
	}
return true;
#endif
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

STR_ID CGATTR::attr_get_s(XCON_S *con, XCON_S *slot)
{
XPTR_S *pval;
PTR_ID val_pid;
if (!(pval = attr_get(con, slot, /*UP*/ val_pid)))
   return 0;
if (pval->kind != pXSTR)
   {
   _ftprintf(stderr, _T("[attr_get_s: Not sym-valued attr.]\n"));
   cg_->kbfree(pval);
   return 0;
   }
STR_ID sid = pval->v.xstr;
cg_->kbfree(pval);
return sid;
}



/**************************************************
*						ATTRS_PRET
* FUN:	attrs_pret
* SUBJ:	Pretty-print attribute list.
* CR:		10/12/95 AM.
*
**************************************************/

void CGATTR::attrs_pret(
	XPTR_S *attrs,
	//FILE *out,		// 04/20/99 AM.
	_t_ostream *out,		// 04/20/99 AM.
	_TCHAR *indent		/* String for indenting. */
	)
{
// DEFERRING, PROBLY OBSOLETE.	// 02/17/07 AM.
#ifdef _OBSO_070217_
XPTR_S *p_slot, *vals;
XCON_S *slot;
//XPVAL pv;
_TCHAR *str, *s_slot;

while (attrs)
   {
   p_slot = attrs->v.xptr;
   slot   = p_slot->v.xcon;
   vals   = p_slot->next;
   s_slot = cg_->qkbm_->con_str(slot);
   //fprintf(out, "%s%s ", indent, s_slot);			// 04/20/99 AM.
   //fflush(out);												// 04/20/99 AM.
	*out << indent << s_slot << flush;					// 04/20/99 AM.
   while (vals)
      {
      switch (vals->kind)
         {
         case pXCON:		str = cg_->qkbm_->con_str(vals->v.xcon);			break;
         case pXSTR:		str = vals->v.xstr;						break;
         case pNUM:		_stprintf(str, _T("%ld"), vals->v.vnum);	break;
         case pFLOAT:	_stprintf(str, _T("%f"), vals->v.vfloat);	break;//08/16/01 AM
         case pCHAR:		_stprintf(str, _T("%c"), vals->v.vchar);	break;
         case pXPTR:
         default:
			// 04/20/99 AM.
        //fprintf(stderr, "[attrs_pret: Bad value kind=%d]\n", vals->kind);
		  _t_cerr << _T("[attrs_pret: Bad value kind=") << vals->kind << endl;
            exit(1);
         }
      //fprintf(out, "%s ", str);	// 04/20/99 AM.
		*out << str;						// 04/20/99 AM.
      vals = vals->next;
      }
   //fprintf(out, "\n");		// 04/20/99 AM.
	*out << endl;					// 04/20/99 AM.
   attrs = attrs->next;
   }
#endif
}



/**************************************************
*						ATTRS_NAME
* FUN:	attrs_name
* SUBJ:	Get name of attribute.
* CR:		07/05/99 AM.
* WARN:	USER MUST FREE STRING.
*
**************************************************/

_TCHAR *CGATTR::attrs_name(
	PTR_ID attr_pid
	)
{
if (!attr_pid)
	return 0;

XPTR_S *attr;
cg_->qkbm_->getXptr(attr_pid,/*UP*/attr);
if (!attr)
	return 0;

PTR_ID slot_pid = attr->v.xptr;
cg_->kbfree(attr);
if (!slot_pid)
	return 0;
XPTR_S *slot;
cg_->qkbm_->getXptr(slot_pid,/*UP*/ slot);
if (!slot)
	return 0;

_TCHAR *str = 0;
switch (slot->kind)
	{
	case pXSTR:
		str = cg_->qkbm_->getXstr(slot->v.xstr);
		break;
	case pXCON:
		str = cg_->qkbm_->con_str(slot->v.xcon);
		break;
	default:
		break;
	}
cg_->kbfree(slot);
return str;
}


/**************************************************
*						ATTRS_VALS
* FUN:	attrs_name
* SUBJ:	Get values list of attribute.
* CR:		07/05/99 AM.
*
**************************************************/
#ifdef _OBSO_070217_

XPTR_S *CGATTR::attrs_vals(
	XPTR_S *attrs
	)
{
XPTR_S *p_slot;
XCON_S *slot;

if (!attrs)
	return 0;

p_slot = attrs->v.xptr;
slot   = p_slot->v.xcon;
return p_slot->next;
}
#endif

/**************************************************
*						RM_ONE_VAL
* FUN:	rm_one_val
* SUBJ:	Remove given val from its attribute.
* CR:		05/11/00 AM.
*
**************************************************/

bool CGATTR::rm_one_val(
	PTR_ID attr_pid,
   PTR_ID val_pid
   )
{
if (!attr_pid || !val_pid)
	return false;

XPTR_S *attr;
cg_->qkbm_->getXptr(attr_pid,/*UP*/attr);
if (!attr)
	return false;

// Get slot.
PTR_ID slot_pid = attr->v.xptr;
cg_->kbfree(attr);
if (!slot_pid)
	return false;

XPTR_S *slot;
cg_->qkbm_->getXptr(slot_pid,/*UP*/slot);
if (!slot)
	return false;

// Pre-find the value and remove it.
XPTR_S *pre = slot;	// Start at slot!
PTR_ID pre_pid = slot_pid;

while (pre && pre->next != val_pid)
	{
	pre_pid = pre->next;
	cg_->kbfree(pre);
	cg_->qkbm_->getXptr(pre_pid,/*UP*/ pre);
	}
if (!pre)
	return false;	// Didn't find it.
if (pre->next != val_pid)	// Sanity check.
	{
	cg_->kbfree(pre);
	return false;
	}

XPTR_S *val;
cg_->qkbm_->getXptr(val_pid,/*UP*/ val);
if (!val)
	{
	cg_->kbfree(pre);
	return false;
	}

pre->next = val->next;
cg_->qkbm_->ptr_update(pre_pid,pre);
cg_->kbfree(pre);

val->next = 0;
cg_->qkbm_->ptr_list_rm(val_pid,val);
cg_->kbfree(val);

return true;
}

/**************************************************
*						ATTR_RM_VAL_BY_NAME
* FUN:	attr_rm_val_by_name
* SUBJ:	Remove value of the named attribute.
* CR:		05/03/99 AM.
* RET:	bool - true if found and removed attr, else false.
* NOTE:	String type only, looks for first instance.
*
**************************************************/

bool CGATTR::attr_rm_val_by_name(
   CON_ID cid,
   _TCHAR *attr_s,
	_TCHAR *val_s
   )
{
if (!cid || !attr_s || !*attr_s || !val_s || !*val_s)
	return false;

// Get string from db, else error.
STR_ID val_sid = cg_->qkbm_->getStrx(val_s);
if (!val_sid)
	return false;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return false;

XPTR_S *pre_hptr;
PTR_ID pre_hpid, hpid, slot_pid;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre_hptr = cg_->qkbm_->attr_prefind_h(con, attr_s, /*UP*/ first, pre_hpid, hpid, slot_pid);

if (!first && !pre_hpid)
	{
	cg_->kbfree(con);
	return false;
	}

// Find slot.
XPTR_S *slot_ptr;
if (!cg_->qkbm_->getXptr(slot_pid, /*UP*/ slot_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	return false;
	}

// Find first value.
PTR_ID val_pid = slot_ptr->next;
if (!val_pid)	// No values.
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}
XPTR_S *val_ptr;
if (!cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}

PTR_ID next = val_ptr->next;

// If first value matches
if (val_ptr->kind == pXSTR && val_ptr->v.xstr == val_sid)	// Match.
	{
	// If only value, rm attribute.
	if (!next)
		{
		// Remove the entire attr here.
		attr_rm(con, pre_hptr, pre_hpid, first);
		cg_->kbfree(pre_hptr);
		cg_->kbfree(con);
		cg_->kbfree(slot_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}

	// Else, update slot and remove val.
	slot_ptr->next = next;
	cg_->qkbm_->ptr_update(slot_pid, slot_ptr);
	val_ptr->next = 0;
	cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
	cg_->kbfree(val_ptr);	// FREE THIS COPY.	// FIX.	// 03/20/07 AM.
	cg_->kbfree(slot_ptr);	// FIX.	// 03/21/07 AM.
	cg_->kbfree(con);			// FIX.	// 03/21/07 AM.
	return true;
	}

// First value didn't match.
cg_->kbfree(con);
cg_->kbfree(pre_hptr);
cg_->kbfree(slot_ptr);

// If just one value, done.
if (!next)
	{
	cg_->kbfree(val_ptr);
	return false;
	}

// More than one value. Pre-traverse to find first matching val and delete just that.
PTR_ID pre_pid = 0;
XPTR_S *pre_ptr = 0;
while (next)
	{
	pre_pid = val_pid;
	pre_ptr = val_ptr;

	val_pid = next;
	cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr);

	if (val_ptr->kind == pXSTR && val_ptr->v.xstr == val_sid)	// Match.
		{
		// Remove value.
		pre_ptr->next = val_ptr->next;
		cg_->qkbm_->ptr_update(pre_pid, pre_ptr);
		val_ptr->next = 0;
		cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
		
		cg_->kbfree(pre_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}
	// No match. Continue.
	cg_->kbfree(pre_ptr);
	next = val_ptr->next;
	}

cg_->kbfree(val_ptr);
return false;
}


/**************************************************
*						ATTR_RM_VAL_BY_NAME
* FUN:	attr_rm_val_by_name
* SUBJ:	Remove value of the named attribute.
* CR:		05/19/99 AM.
* NOTE:	Long int variant, looks for first instance.
*			02/18/07 AM. REIMPLEMENT.
*			Zero allowed as value to delete, as before.
*
**************************************************/

bool CGATTR::attr_rm_val_by_name(
   CON_ID cid,
   _TCHAR *attr_s,
	long val
   )
{
if (!cid || !attr_s || !*attr_s)
	return false;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return false;

XPTR_S *pre_hptr;
PTR_ID pre_hpid, hpid, slot_pid;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre_hptr = cg_->qkbm_->attr_prefind_h(con, attr_s, /*UP*/ first, pre_hpid, hpid, slot_pid);

if (!first && !pre_hpid)
	{
	cg_->kbfree(con);
	return false;
	}

// Find slot.
XPTR_S *slot_ptr;
if (!cg_->qkbm_->getXptr(slot_pid, /*UP*/ slot_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	return false;
	}

// Find first value.
PTR_ID val_pid = slot_ptr->next;
if (!val_pid)	// No values.
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}
XPTR_S *val_ptr;
if (!cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}

PTR_ID next = val_ptr->next;

// If first value matches
if (val_ptr->kind == pNUM && val_ptr->v.vnum == val)	// Match.
	{
	// If only value, rm attribute.
	if (!next)
		{
		// Remove the entire attr here.
		attr_rm(con, pre_hptr, pre_hpid, first);
		cg_->kbfree(pre_hptr);
		cg_->kbfree(con);
		cg_->kbfree(slot_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}

	// Else, update slot and remove val.
	slot_ptr->next = next;
	cg_->qkbm_->ptr_update(slot_pid, slot_ptr);
	val_ptr->next = 0;
	cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
	cg_->kbfree(val_ptr);	// FREE THIS COPY.	// FIX.	// 03/20/07 AM.
	cg_->kbfree(slot_ptr);	// FIX.	// 03/21/07 AM.
	cg_->kbfree(con);			// FIX.	// 03/21/07 AM.
	return true;
	}

// First value didn't match.
cg_->kbfree(con);
cg_->kbfree(pre_hptr);
cg_->kbfree(slot_ptr);

// If just one value, done.
if (!next)
	{
	cg_->kbfree(val_ptr);
	return false;
	}

// More than one value. Pre-traverse to find first matching val and delete just that.
PTR_ID pre_pid = 0;
XPTR_S *pre_ptr = 0;
while (next)
	{
	pre_pid = val_pid;
	pre_ptr = val_ptr;

	val_pid = next;
	cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr);

	if (val_ptr->kind == pNUM && val_ptr->v.vnum == val)	// Match.
		{
		// Remove value.
		pre_ptr->next = val_ptr->next;
		cg_->qkbm_->ptr_update(pre_pid, pre_ptr);
		val_ptr->next = 0;
		cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
		cg_->kbfree(pre_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}
	// No match. Continue.
	cg_->kbfree(pre_ptr);
	next = val_ptr->next;
	}

cg_->kbfree(val_ptr);
return false;
}



/**************************************************
*						ATTR_RM_VAL_BY_NAME
* FUN:	attr_rm_val_by_name
* SUBJ:	Remove value of the named attribute.
* CR:		03/19/03 AM.
* NOTE:	Float variant, looks for first instance.
*			02/18/07 AM. REIMPLEMENT.
*
**************************************************/

bool CGATTR::attr_rm_val_by_name(
   CON_ID cid,
   _TCHAR *attr_s,
	float val
   )
{
if (!cid || !attr_s || !*attr_s)
	return false;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return false;

XPTR_S *pre_hptr;
PTR_ID pre_hpid, hpid, slot_pid;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre_hptr = cg_->qkbm_->attr_prefind_h(con, attr_s, /*UP*/ first, pre_hpid, hpid, slot_pid);

if (!first && !pre_hpid)
	{
	cg_->kbfree(con);
	return false;
	}

// Find slot.
XPTR_S *slot_ptr;
if (!cg_->qkbm_->getXptr(slot_pid, /*UP*/ slot_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	return false;
	}

// Find first value.
PTR_ID val_pid = slot_ptr->next;
if (!val_pid)	// No values.
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}
XPTR_S *val_ptr;
if (!cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}

PTR_ID next = val_ptr->next;

// If first value matches
if (val_ptr->kind == pFLOAT && val_ptr->v.vnum >= (val - FLT_DELTA)
		&& val_ptr->v.vnum <= (val + FLT_DELTA))	// Match.
	{
	// If only value, rm attribute.
	if (!next)
		{
		// Remove the entire attr here.
		attr_rm(con, pre_hptr, pre_hpid, first);
		cg_->kbfree(pre_hptr);
		cg_->kbfree(con);
		cg_->kbfree(slot_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}

	// Else, update slot and remove val.
	slot_ptr->next = next;
	cg_->qkbm_->ptr_update(slot_pid, slot_ptr);
	val_ptr->next = 0;
	cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
	cg_->kbfree(val_ptr);	// FREE THIS COPY.	// FIX.	// 03/20/07 AM.
	return true;
	}

// First value didn't match.
cg_->kbfree(con);
cg_->kbfree(pre_hptr);
cg_->kbfree(slot_ptr);

// If just one value, done.
if (!next)
	{
	cg_->kbfree(val_ptr);
	return false;
	}

// More than one value. Pre-traverse to find first matching val and delete just that.
PTR_ID pre_pid = 0;
XPTR_S *pre_ptr = 0;
while (next)
	{
	pre_pid = val_pid;
	pre_ptr = val_ptr;

	val_pid = next;
	cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr);

	if (val_ptr->kind == pFLOAT && val_ptr->v.vnum >= (val - FLT_DELTA)
			&& val_ptr->v.vnum <= (val + FLT_DELTA))	// Match.
		{
		// Remove value.
		pre_ptr->next = val_ptr->next;
		cg_->qkbm_->ptr_update(pre_pid, pre_ptr);
		val_ptr->next = 0;
		cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
		cg_->kbfree(pre_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}
	// No match. Continue.
	cg_->kbfree(pre_ptr);
	next = val_ptr->next;
	}

cg_->kbfree(val_ptr);
return false;
}


/**************************************************
*						ATTR_RM_VAL_BY_CONCEPT
* FUN:	attr_rm_val_by_concept
* SUBJ:	Remove concept value of the named attribute.
* CR:		06/03/02 Dd.
* NOTE:	Concept variant, looks for first instance.
*			02/18/07 AM. REIMPLEMENT
*
**************************************************/

bool CGATTR::attr_rm_val_by_concept(
   CON_ID cid,
   _TCHAR *attr_s,
	CON_ID val
   )
{
if (!cid || !attr_s || !*attr_s || !val)
	return false;

XCON_S *con = cg_->qkbm_->Con(cid);
if (!con)
	return false;

XPTR_S *pre_hptr;
PTR_ID pre_hpid, hpid, slot_pid;
bool first = false;

// Prefind the attribute, in case we want to delete it.
pre_hptr = cg_->qkbm_->attr_prefind_h(con, attr_s, /*UP*/ first, pre_hpid, hpid, slot_pid);

if (!first && !pre_hpid)
	{
	cg_->kbfree(con);
	return false;
	}

// Find slot.
XPTR_S *slot_ptr;
if (!cg_->qkbm_->getXptr(slot_pid, /*UP*/ slot_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	return false;
	}

// Find first value.
PTR_ID val_pid = slot_ptr->next;
if (!val_pid)	// No values.
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}
XPTR_S *val_ptr;
if (!cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr))
	{
	cg_->kbfree(pre_hptr);
	cg_->kbfree(con);
	cg_->kbfree(slot_ptr);
	return false;
	}

PTR_ID next = val_ptr->next;

// If first value matches
if (val_ptr->kind == pXCON && val_ptr->v.xcon == val)	// Match.
	{
	// If only value, rm attribute.
	if (!next)
		{
		// Remove the entire attr here.
		attr_rm(con, pre_hptr, pre_hpid, first);
		cg_->kbfree(pre_hptr);
		cg_->kbfree(con);
		cg_->kbfree(slot_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}

	// Else, update slot and remove val.
	slot_ptr->next = next;
	cg_->qkbm_->ptr_update(slot_pid, slot_ptr);
	val_ptr->next = 0;
	cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
	cg_->kbfree(val_ptr);	// FREE THIS COPY.	// FIX.	// 03/20/07 AM.
	return true;
	}

// First value didn't match.
cg_->kbfree(con);
cg_->kbfree(pre_hptr);
cg_->kbfree(slot_ptr);

// If just one value, done.
if (!next)
	{
	cg_->kbfree(val_ptr);
	return false;
	}

// More than one value. Pre-traverse to find first matching val and delete just that.
PTR_ID pre_pid = 0;
XPTR_S *pre_ptr = 0;
while (next)
	{
	pre_pid = val_pid;
	pre_ptr = val_ptr;

	val_pid = next;
	cg_->qkbm_->getXptr(val_pid, /*UP*/ val_ptr);

	if (val_ptr->kind == pXCON && val_ptr->v.xcon == val)	// Match.
		{
		// Remove value.
		pre_ptr->next = val_ptr->next;
		cg_->qkbm_->ptr_update(pre_pid, pre_ptr);
		val_ptr->next = 0;
		cg_->qkbm_->ptr_list_rm(val_pid, val_ptr);
		cg_->kbfree(pre_ptr);
		cg_->kbfree(val_ptr);
		return true;
		}
	// No match. Continue.
	cg_->kbfree(pre_ptr);
	next = val_ptr->next;
	}

cg_->kbfree(val_ptr);
return false;
}


/**********************     END OF FILE         ***************************/
