/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									IND.C
*
* FILE:	consh.�/ind.c
* SUBJ:	Higher-level knowledge addition commands.
* CR:		10/15/95 AM.
* NOTE:	Support multi-line addition commands, with some abstraction from kb.
*			If interactive, support query for concepts.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <ctype.h>

#include "prim/libprim.h"
#include "cg_global.h"		// 10/05/99 AM.

#include "prim/prim.h"
#include "prim/str.h"
//#include "prim/mach.h"
#include "prim/list_s.h"
#include "prim/list.h"

/*
#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/ra.h"
#include "kbm/rest.h"
#include "kbm/attr.h"
#include "kbm/dict.h"
#include "kbm/pret.h"
#include "kbm/Akbm.h"
*/
#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "cgcon.h"
#include "cgdict.h"
#include "qconsh/cg.h"
#include "qconsh/arg.h"

//#include "consh/cc_var.h"

//#include "Con_ini.h"
//#include "cc_gen.h"
//#include "st_gen.h"
//#include "sym_gen.h"
//#include "con_gen.h"
//#include "ptr_gen.h"

#include "ind.h"
#include "ui.h"
#include "cmd.h"


/**************************************************
*						IND_ACTION
* FUN:	ind_action
* SUBJ:	Add rule-action attribute to concept.
* CR:		12/20/95 AM.
* RET:	ok - If command successful.
* FORM:	ind action
*			concept ....
*			ord			(0 if concept, else nth in concept's phrase)
*			action type (concept)
*			data struct (concept)
*			ds slot		(concept)
*			ds val		(concept)
*			from			(num)
*			to				(num)
*			end ind
* OPT:	Collect values as list, then add in one shot.
* NOTE:	Should handle multiple slots.
*
**************************************************/

#ifdef OLD_020815_
bool ind_action(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con, *typ, *ds, *slot, *val;
//XCON_S *c_val;
long from, to;
//XPVAL p_val;
//XPKIND p_kind;
//char buf[CMD_SIZE];
//bool added;
//LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */

if (args)
   {
   *out << _T("Too many args in IND ACTION command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok) return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok) return(false);

/* Get action-type concept.	*/
ok = ui_read_con(_T("type> "), in, out, i_flag, silent,cg, /*UP*/ &typ);
if (!ok) return(false);

/* Get data-structure concept.	*/
ok = ui_read_con(_T("data struct> "), in, out, i_flag, silent,cg, /*UP*/ &ds);
if (!ok) return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok) return(false);

/* Get value */
#ifdef old
ok = ui_read_sym(_T("slot-val> "), in, out, i_flag, silent, /*UP*/ &val);
#endif
ok = ui_read_con(_T("slot-val> "), in, out, i_flag, silent,cg, /*UP*/ &val);
if (!ok) return(false);

/* Get "from" num */
ok = ui_read_num(_T("from> "), in, out, i_flag, silent,cg, /*UP*/ &from);
if (!ok) return(false);

/* Get "to" num */
ok = ui_read_num(_T("to> "), in, out, i_flag, silent,cg, /*UP*/ &to);
if (!ok) return(false);

ok = ui_read_end(_T("end> "), in, out, i_flag, silent,cg);
if (!ok) return(false);

/*** If proxy concept, get it. ***/
if (ord)
	{
	con = cg->cgcon_->con_nth_proxy(con, ord);
	if (!con) return(false);
	}

/*** ADD THE RULE ACTION ***/
return(ra_add(con, typ, ds, slot, val, from, to));
}
#endif

/**************************************************
*						IND_ATTR
* FUN:	ind_attr
* SUBJ:	Add attribute to concept or proxy.
* CR:		10/15/95 AM.
* RET:	ok - If command successful.
* FORM:	ind attr
*			concept ....
*			ordinality of proxy, or 0 for concept itself.
*			slot concept ....
*			value kind
*			value 1
*			....
*			end ind
* OPT:	Collect values as list, then add in one shot.
* NOTE:	Should handle multiple slots.
*			Don't like the way these are written.  Should check that
*			everything's ok before modifying the kb.
*			7/6/96 AM. Merging ind_attr and ind_proxy.
*			02/14/07 AM. Reworking for QDBM_. Add each val to db separately.
*
**************************************************/

bool ind_attr(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con, *slot, *c_val;
XPVAL p_val;
XPKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg,
	cg->cgcon_->path_, cg->cgcon_->spath_, cg->cgcon_->lpath_);	// 03/08/07 AM.
if (!ok)
	return(false);
con = cg->cgcon_->path_[cg->cgcon_->lpath_];	// 03/08/07 AM.
XCON_S *safe_con = con;	// 03/08/07 AM.

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok)
	return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg,
	cg->cgcon_->path2_, cg->cgcon_->spath2_, cg->cgcon_->lpath2_);	// 03/08/07 AM.
if (!ok)
	return(false);
slot = cg->cgcon_->path2_[cg->cgcon_->lpath2_];	// 03/08/07 AM.

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent,cg, /*UP*/ &p_kind);
if (!ok)
	return(false);

/*** If proxy concept, get it. ***/
CON_ID nid;
XCON_S *node;
if (ord)
	{
	nid = cg->cgcon_->con_nth_proxy(con, ord);
	node = cg->qkbm_->Con(nid);
	if (!node)
		return(false);
	con = node;	// Now concept to be used/freed is the node.	// 02/14/07 AM.
	}

// OPT:	If absent, make an empty attr slot.					// 02/15/07 AM.
//			If present, find last value for quick append.	// 02/15/07 AM.
PTR_ID pid = 0;	// Track id of last ptr.					// 02/15/07 AM.
XPTR_S *ptr = cg->qkbm_->attr_get_last_ptr(con, slot, /*UP*/ pid);	// 02/15/07 AM.
XPTR_S *oldlast = 0;													// 02/15/07 AM.
PTR_ID oldpid = 0;													// 02/15/07 AM.

// Now have ptr, so don't need concept and slot.
if (con->id != safe_con->id)	// 03/08/07 AM.
	cg->kbfree((char *)con);	// 02/15/07 AM.
if (!ptr)	// Error.
	return(false);

// Get values.
added = false;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok)
		{
		cg->kbfree((char *)ptr);	// 02/15/07 AM.
		return(false);
		}
   
   if (!vals
		 || (!_tcscmp(_T("end"), ALIST::list_str(&vals,buf))
			  && vals->next										// PATCH	// 10/26/00 AM.
			  && !_tcscmp(_T("ind"),ALIST::list_str(&vals->next,buf)))	// PATCH	// 10/26/00 AM.
		)
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == pXCON)
      {
      ok = path_to_con(vals,cg, &c_val);
      alist->list_free(vals, LNULL);
      p_val.xcon = c_val->id;
      cg->kbfree(c_val);
      if (!ok)
			{
			cg->kbfree((char *)ptr);	// 02/15/07 AM.
			return(false);
			}
      }
   else
      {
      ok = cg->qkbm_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok)
			{
			cg->kbfree((char *)ptr);	// 02/15/07 AM.
			return(false);
			}
      }

   /* ADD THE ATTRIBUTE! */
	// Extend the pointer so far.										// 02/15/07 AM.
	oldlast = ptr;															// 02/15/07 AM.
	oldpid  = pid;															// 02/15/07 AM.
	ptr = cg->qkbm_->attr_add_val(									// 02/15/07 AM.
										oldlast,
										oldpid,
										p_val,
										p_kind,
										/*UP*/
										pid
										);

	cg->kbfree(oldlast);														// 02/15/07 AM.
//   cg->qkbm_->attr_add_val(con, slot, p_val, p_kind);
   added = true;
   }

// Free the copy of the last value's ptr.
cg->kbfree((char *)ptr);	// 02/15/07 AM.
return(added);
}


/**************************************************
*						IND_NATTR
* FUN:	ind_nattr
* SUBJ:	Add attribute to concept or proxy.
* CR:		10/09/05 AM.
* RET:	ok - If command successful.
* FORM:	ind attr
*			concept ....
*			ordinality of proxy, or 0 for concept itself.
*			slot concept ....
*			value kind
*			value 1
*			....
*			end ind
* NOTE:	Upgrades ind_attr to handle a NODE as a value.
*			If value kind = concept, it looks like:
*				...
*			   pcon
*				concept path
*				concept/node number (0 means concept)
*				...
*			ind_attr kept for backward compatibility, also
*			for efficiency when values are not kb nodes.
*
**************************************************/

bool ind_nattr(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con, *slot, *c_val;
XPVAL p_val;
XPKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */
long val_ord = 0;	// If concept/node is value, get ord.				// 10/09/05 AM.
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok)
	return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok)
	{
	cg->kbfree((char *)con);	// 02/14/07 AM.
	return(false);
	}

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok)
	{
	cg->kbfree((char *)con);	// 02/14/07 AM.
	return(false);
	}

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent,cg, /*UP*/ &p_kind);
if (!ok)
	{
	cg->kbfree((char *)con);	// 02/14/07 AM.
	cg->kbfree((char *)slot);	// 02/15/07 AM.
	return(false);
	}

/*** If proxy concept, get it. ***/
CON_ID nid;
XCON_S *node;	// 02/14/07 AM.
if (ord)
	{
	nid = cg->cgcon_->con_nth_proxy(con, ord);
	node = cg->qkbm_->Con(nid);
	cg->kbfree((char *)con);	// 02/14/07 AM.
	if (!node)
		{
		cg->kbfree((char *)slot);	// 02/15/07 AM.
		return(false);
		}
	con = node;	// 02/14/07 AM.
	}

// OPT:	If absent, make an empty attr slot.					// 02/15/07 AM.
//			If present, find last value for quick append.	// 02/15/07 AM.
PTR_ID pid = 0;	// Track id of last ptr.					// 02/15/07 AM.
XPTR_S *ptr = cg->qkbm_->attr_get_last_ptr(con, slot, /*UP*/ pid);	// 02/15/07 AM.
XPTR_S *oldlast = 0;													// 02/15/07 AM.
PTR_ID oldpid = 0;													// 02/15/07 AM.

// Now have ptr, so don't need concept and slot.
cg->kbfree((char *)con);	// 02/15/07 AM.
cg->kbfree((char *)slot);	// 02/15/07 AM.
if (!ptr)	// Error.
	return(false);

// Get values.
added = false;
CON_ID xid;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok)
		{
		cg->kbfree((char *)ptr);	// 02/14/07 AM.
		return(false);
		}
   
   if (!vals
		 || (!_tcscmp(_T("end"), ALIST::list_str(&vals,buf))
			  && vals->next										// PATCH	// 10/26/00 AM.
			  && !_tcscmp(_T("ind"),ALIST::list_str(&vals->next,buf)))	// PATCH	// 10/26/00 AM.
		)
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == pXCON)
      {
      // Reworking this.			// 10/09/05 AM.
      ok = path_to_con(vals,cg, &c_val);
      if (!ok)
			{
			alist->list_free(vals, LNULL);
			cg->kbfree((char *)ptr);	// 02/14/07 AM.
			return(false);
			}

		// Get node number.  0 = concept.								// 10/09/05 AM.
		ok = ui_read_num(_T("valord> "),in,out,i_flag,silent,cg,	// 10/09/05 AM.
															/*UP*/ &val_ord);	// 10/09/05 AM.
		alist->list_free(vals, LNULL);									// 10/09/05 AM.
		if (!ok)																	// 10/09/05 AM.
			{
			cg->kbfree((char *)ptr);	// 02/14/07 AM.
			return(false);
			}

		if (val_ord)	// Get node.										// 10/09/05 AM.
			{
			xid = cg->cgcon_->con_nth_proxy(c_val, val_ord);		// 10/09/05 AM.
			c_val = cg->qkbm_->Con(xid);
			if (!c_val)															// 10/09/05 AM.
				{
				cg->kbfree((char *)ptr);	// 02/14/07 AM.
				return(false);
				}
			}

      p_val.xcon = c_val->id;
      }
   else
      {
      ok = cg->qkbm_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok)
			{
			cg->kbfree((char *)ptr);	// 02/14/07 AM.
			return(false);
			}
      }

   /* ADD THE ATTRIBUTE! */
	// Extend the pointer so far.										// 02/15/07 AM.
	oldlast = ptr;															// 02/15/07 AM.
	oldpid  = pid;															// 02/15/07 AM.
	ptr = cg->qkbm_->attr_add_val(									// 02/15/07 AM.
										oldlast,
										oldpid,
										p_val,
										p_kind,
										/*UP*/
										pid
										);

	cg->kbfree(oldlast);														// 02/15/07 AM.
//   cg->qkbm_->attr_add_val(con, slot, p_val, p_kind);
   added = true;
   }

cg->kbfree((char *)ptr);	// 02/14/07 AM.
return(added);
}


/**************************************************
*						IND_WATTR
* FUN:	ind_wattr
* SUBJ:	Add attribute to word-concept.
* CR:		05/30/02 AM.
* RET:	ok - If command successful.
* FORM:	ind attr
*			concept ....
*			ordinality of proxy, or 0 for concept itself.
*			slot concept ....
*			value kind
*			value 1
*			....
*			end ind
* OPT:	Collect values as list, then add in one shot.
* NOTE:	05/30/02 AM. Convenient variant of ind_attr.
*
**************************************************/

bool ind_wattr(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *wcon, *slot, *c_val;
XPVAL p_val;
XPKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_word(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &wcon);
if (!ok)
	return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok)
	{
	cg->kbfree((char *)wcon);	// 02/14/07 AM.
	return(false);
	}

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok)
	{
	cg->kbfree((char *)wcon);	// 02/14/07 AM.
	return(false);
	}

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent,cg, /*UP*/ &p_kind);
if (!ok)
	{
	cg->kbfree((char *)wcon);	// 02/14/07 AM.
	cg->kbfree((char *)slot);	// 02/14/07 AM.
	return(false);
	}

/*** If proxy concept, get it. ***/
if (ord)
	{
	// Word concept can't be a proxy.  Should delete that line.
	// (Keeping it for consistency.)
	cg->kbfree((char *)wcon);	// 02/14/07 AM.
	cg->kbfree((char *)slot);	// 02/14/07 AM.
	return(false);
	}

// OPT:	If absent, make an empty attr slot.					// 02/15/07 AM.
//			If present, find last value for quick append.	// 02/15/07 AM.
PTR_ID pid = 0;	// Track id of last ptr.					// 02/15/07 AM.
XPTR_S *ptr = cg->qkbm_->attr_get_last_ptr(wcon, slot, /*UP*/ pid);	// 02/15/07 AM.
XPTR_S *oldlast = 0;													// 02/15/07 AM.
PTR_ID oldpid = 0;													// 02/15/07 AM.

// Now have ptr, so don't need concept and slot.
cg->kbfree((char *)wcon);	// 02/15/07 AM.
cg->kbfree((char *)slot);	// 02/15/07 AM.
if (!ptr)	// Error.
	return(false);

// Get values.
added = false;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok)
		{
		cg->kbfree((char *)ptr);	// 02/15/07 AM.
		return(false);
		}
   
   if (!vals
		 || (!_tcscmp(_T("end"), ALIST::list_str(&vals,buf))
			  && vals->next										// PATCH	// 10/26/00 AM.
			  && !_tcscmp(_T("ind"),ALIST::list_str(&vals->next,buf)))	// PATCH	// 10/26/00 AM.
		)
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == pXCON)
      {
      ok = path_to_con(vals,cg, &c_val);
      alist->list_free(vals, LNULL);
      p_val.xcon = c_val->id;
      if (!ok)
			{
			cg->kbfree((char *)ptr);	// 02/15/07 AM.
			return(false);
			}
      }
   else
      {
      ok = cg->qkbm_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok)
			{
			cg->kbfree((char *)ptr);	// 02/15/07 AM.
			return(false);
			}
      }

   /* ADD THE ATTRIBUTE! */
	// Extend the pointer so far.										// 02/15/07 AM.
	oldlast = ptr;															// 02/15/07 AM.
	oldpid  = pid;															// 02/15/07 AM.
	ptr = cg->qkbm_->attr_add_val(									// 02/15/07 AM.
										oldlast,
										oldpid,
										p_val,
										p_kind,
										/*UP*/
										pid
										);

	cg->kbfree(oldlast);														// 02/15/07 AM.
//   cg->qkbm_->attr_add_val(wcon, slot, p_val, p_kind);
   added = true;
   }

cg->kbfree((char *)ptr);	// 02/15/07 AM.
return(added);
}

/**************************************************
*						IND_CHILDS
* FUN:	ind_childs
* SUBJ:	Add children to a hierarchy concept.
* CR:		10/22/95 AM.
* RET:	ok - If command successful.
* FORM:	ind childs
*			concept ....
*			name1 name2 ...
*			end ind
* OPT:	Collect values as list, then add in one shot.     
*
**************************************************/

bool ind_childs(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con;
_TCHAR buf[CMD_SIZE], nbuf[CMD_SIZE];
_TCHAR *str;
LIST *names, *save, *ends;
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND CHILDS command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok) return(false);

*out << _T("childs> ");
ok  = args_read(in, out, silent,alist, nbuf,CMD_SIZE, &names);
if (!ok) return(false);

*out << _T("end>  ");
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &ends);
if (!ok)
   {
   alist->list_free(names, LNULL);
   return(false);
   }

if (!ends || _tcscmp(_T("end"), ALIST::list_str(&ends,buf)))
   *out << _T("ind childs: Missing 'end ind'.") << endl;
alist->list_free(ends, LNULL);

/* ADD CHILDREN TO HIERARCHY */
save = names;
while (str = ALIST::list_pop_buf(&names,nbuf))
   cg->cgcon_->con_add_basic(str, con);

alist->list_free(save, LNULL);
return(true);
}


/**************************************************
*						IND_NAMED_PHRASE
* FUN:	ind_named_phrase
* SUBJ:	Add a named phrase to concept.
* CR:		11/11/95 AM.
* RET:	ok - If command successful.
* FORM:	ind named_phrase
*			concept ....
*			name1
*			name2
*			....
*			end ind
* OPT:	Collect values as list, then add in one shot.
* NOTE:	Taken from ind_phrase.  Would like each name to be an
*			arbitrary phrase, in the future.   
*
**************************************************/

bool ind_named_phrase(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con;
//XCON_S *proxy;
CON_ID word;
_TCHAR buf[CMD_SIZE];
CON_ID elts[MAX_PHRASE_ELTS];
int count,			/* Count of elements. */
    ii;
//XPVAL pv;
LIST *list;
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok) return(false);

/* Get phrase.			*/
count = 0;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("elt>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &list);
   if (!ok)
		{
		cg->kbfree((char *)con);	// 02/14/07 AM.
		return(false);
		}
   
   /* First word = "end" terminates the addition. */
   if (!list || !_tcscmp(_T("end"), ALIST::list_str(&list,buf)))
      {
      alist->list_free(list, LNULL);
      break;
      }

   if (list->next)
      {
      _t_cerr << _T("ind named_phrase: Not handling multi-word names.") << endl;
      alist->list_free(list, LNULL);
		cg->kbfree((char *)con);	// 02/14/07 AM.
		return(false);
      }

   word = cg->cgdict_->dict_add_word(ALIST::list_str(&list,buf));
   alist->list_free(list, LNULL);
   if (!word) return(false);
   elts[count++] = word;
   
   if (count > MAX_PHRASE_ELTS)
      {
      _t_cerr << _T("ind named_phrase: More than ")
			  << MAX_PHRASE_ELTS
			  << _T(" elements.")
			  << endl;
		cg->kbfree((char *)con);	// 02/14/07 AM.
		return(false);
      }
   }

if (!count)
	{
	cg->kbfree((char *)con);	// 02/14/07 AM.
	return(false);
	}

// Everything checked out, so build the phrase.

// Build and bind first node of phrase. (owner con, id, target id)
CON_ID nid = cg->qkbm_->make_phrase(con, elts[0]);	// 02/14/07 AM.
XCON_S *last;

// Extend the phrase.
for (ii = 1; ii < count; ii++)
   {
   // Fetch, extend, and trash the last node successively.
   // OPT. Todo: Can reuse the concept struct for last node repeatedly.
   last = cg->qkbm_->Con(nid);				// 02/14/07 AM.
	nid = cg->qkbm_->extend_phrase(last, elts[ii]);	// 02/14/07 AM.
	cg->kbfree((char *) last);					// 02/14/07 AM.
   }

/*
proxy = cg->qkbm_->con_add_proxy(elts[0]);

// Bind concept to first element of phrase.
pv.vcon = proxy;
cg->qkbm_->attr_add_val(con, cg->qkbm_->c_cg_PHRASE, pv, pXCON);
pv.vcon = con;
cg->qkbm_->attr_add_val(proxy, cg->qkbm_->c_cg_ATOM, pv, pXCON);

for (ii = 1; ii < count; ii++)
   {
   elt = cg->qkbm_->con_add_proxy(elts[ii]);
   proxy->next = elt;
   elt->prev = proxy;
   proxy = elt;
   }
*/

cg->kbfree((char *)con);	// 02/14/07 AM.
return(true);
}

/**************************************************
*						IND_PHRASE
* FUN:	ind_phrase
* SUBJ:	Add a phrase to concept.
* CR:		10/19/95 AM.
* RET:	ok - If command successful.
* FORM:	ind phrase
*			concept ....
*			phr concept 1
*			....
*			end ind
* OPT:	Collect values as list, then add in one shot.     
*
**************************************************/

bool ind_phrase(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con;
XCON_S *target;	// Target concept for node.	// 02/14/07 AM.
//CON_ID elt;
_TCHAR buf[CMD_SIZE];
CON_ID elts[MAX_PHRASE_ELTS];
int count,			/* Count of elements. */
    ii;
//XPVAL pv;
LIST *list;
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok)
	{
	*cgerr << _T("[ind phrase: Can't read concept.]") << endl;
	return(false);
	}

/* Get phrase.			*/
count = 0;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("elt>  ");
//	*cgerr << "elt>  "; // 10/05/99 AM.
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &list);
   if (!ok)
		{
		*cgerr << _T("[ind phrase: Can't get elt.]") << endl;
		cg->kbfree((char *)con);	// 02/14/07 AM.
		return(false);
		}
   
   if (!list || !_tcscmp(_T("end"), ALIST::list_str(&list,buf)))
      {
      alist->list_free(list, LNULL);
      break;
      }

   ok = path_to_con(list,cg, &target);	// 02/14/07 AM.
   elts[count++] = target->id;			// 02/14/07 AM.
   cg->kbfree((char *) target);			// 02/14/07 AM.
//   ok = path_to_con(list,cg, &(elts[count++]));
   alist->list_free(list, LNULL);
   if (!ok)
		{
		*cgerr << _T("[ind phrase: Can't convert path to con.]") << endl;
		cg->kbfree((char *)con);	// 02/14/07 AM.
		return(false);
		}
   
   if (count > MAX_PHRASE_ELTS)
      {
      //cerr
		*cgerr		// 10/05/99 AM.
			  <<_T("ind phrase: More than ")
			  << MAX_PHRASE_ELTS
			  << _T(" elements.")
			  << endl;
		cg->kbfree((char *)con);	// 02/14/07 AM.
		return(false);
      }
   }

if (!count)
	{
	*cgerr << _T("[ind phrase: No count.]") << endl;	// 10/05/99 AM.
	cg->kbfree((char *)con);	// 02/14/07 AM.
	return(false);
	}

// Everything checked out, so build the phrase.

// Build and bind first node of phrase. (owner con, id, target id)
CON_ID nid = cg->qkbm_->make_phrase(con, elts[0]);	// 02/14/07 AM.
XCON_S *last;

// Extend the phrase.
for (ii = 1; ii < count; ii++)
   {
   // Fetch, extend, and trash the last node successively.
   // OPT. Todo: Can reuse the concept struct for last node repeatedly.
   last = cg->qkbm_->Con(nid);				// 02/14/07 AM.
	nid = cg->qkbm_->extend_phrase(last, elts[ii]);	// 02/14/07 AM.
	cg->kbfree((char *) last);					// 02/14/07 AM.
   }

/*
proxy = cg->qkbm_->con_add_proxy(elts[0]);

// Bind concept to first element of phrase.
pv.vcon = proxy;
cg->qkbm_->attr_add_val(con, cg->qkbm_->c_cg_PHRASE, pv, pXCON);
pv.vcon = con;
cg->qkbm_->attr_add_val(proxy, cg->qkbm_->c_cg_ATOM, pv, pXCON);

for (ii = 1; ii < count; ii++)
   {
   elt = cg->qkbm_->con_add_proxy(elts[ii]);
   proxy->next = elt;
   elt->prev = proxy;
   proxy = elt;
   }
*/

cg->kbfree((char *)con);	// 02/14/07 AM.
return(true);
}


#ifdef OBSOLETE
/**************************************************
*						IND_PROXY
* FUN:	ind_proxy
* SUBJ:	Add attribute to proxy concept.
* CR:		11/15/95 AM.
* RET:	ok - If command successful.
* FORM:	ind proxy
*			concept ....
*			ordinality of proxy in concept's phrase.
*			slot concept ....
*			value kind
*			value 1
*			....
*			end ind
* OPT:	Collect values as list, then add in one shot.
* NOTE:	Should handle multiple slots.
*			Taken from ind_attr
*			Don't like the way these are written.  Should check that
*			everything's ok before modifying the kb.
*
**************************************************/

bool ind_proxy(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent
	)
{
bool ok;
XCON_S *con, *slot, *c_val, *prox;
XPVAL p_val;
XPKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in phrase. */

if (args)
   {
   *out << _T("Too many args in IND PROXY command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok) return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok) return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok) return(false);

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent, /*UP*/ &p_kind);
if (!ok) return(false);

/*** Get the proxy concept ***/
prox = cg->cgcon_->con_nth_proxy(con, ord);
if (!prox) return(false);

/* Get values.			*/
added = false;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  "));
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok) return(false);
   
   if (!vals || !_tcscmp(_T("end"), ALIST::list_str(&vals,buf)))
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == PCON)
      {
      ok = path_to_con(vals,cg, &c_val);
      alist->list_free(vals, LNULL);
      p_val.vcon = c_val;
      if (!ok) return(false);
      }
   else
      {
      ok = cg->qkbm_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok) return(false);
      }

   /* ADD THE ATTRIBUTE! */
   attr_add_val(prox, slot, p_val, p_kind);
   added = true;
   }
return(added);
}
#endif


/**************************************************
*						IND_REST
* FUN:	ind_rest
* SUBJ:	Add restriction attribute to concept.
* CR:		7/7/96 AM.
* RET:	ok - If command successful.
* FORM:	ind action
*			concept ....
*			ord			(0 if concept, else nth in concept's phrase)
*			action type (concept)
*			data struct (concept)
*			ds slot		(concept)
*			ds val		(concept)
*			from			(num)
*			to				(num)
*			end ind
* OPT:	Collect values as list, then add in one shot.
* NOTE:	Should handle multiple slots.
*			Copied from ind_action.
*
**************************************************/

#ifdef OLD_020815_
bool ind_rest(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
XCON_S *con, *typ, *ds, *slot, *val;
//XCON_S *c_val;
long from, to;
//XPVAL p_val;
//XPKIND p_kind;
//char buf[CMD_SIZE];
//bool added;
//LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */

if (args)
   {
   *out << _T("Too many args in IND ACTION command.") << endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok) return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok) return(false);

/* Get action-type concept.	*/
ok = ui_read_con(_T("type> "), in, out, i_flag, silent,cg, /*UP*/ &typ);
if (!ok) return(false);

/* Get data-structure concept.	*/
ok = ui_read_con(_T("data struct> "), in, out, i_flag, silent,cg, /*UP*/ &ds);
if (!ok) return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok) return(false);

/* Get value */
#ifdef old
ok = ui_read_sym(_T("slot-val> "), in, out, i_flag, silent, /*UP*/ &val);
#endif
ok = ui_read_con(_T("slot-val> "), in, out, i_flag, silent,cg, /*UP*/ &val);
if (!ok) return(false);

/* Get "from" num */
ok = ui_read_num(_T("from> "), in, out, i_flag, silent,cg, /*UP*/ &from);
if (!ok) return(false);

/* Get "to" num */
ok = ui_read_num(_T("to> "), in, out, i_flag, silent,cg, /*UP*/ &to);
if (!ok) return(false);

ok = ui_read_end(_T("end> "), in, out, i_flag, silent,cg);
if (!ok) return(false);

/*** If proxy concept, get it. ***/
if (ord)
	{
	con = cg->cgcon_->con_nth_proxy(con, ord);
	if (!con) return(false);
	}

/*** ADD THE RULE ACTION ***/
return(rest_add(con, typ, ds, slot, val, from, to));
}
#endif
