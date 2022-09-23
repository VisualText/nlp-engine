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

#include "consh/libconsh.h"	// 04/26/99 AM.
#include "consh/cc_var.h"
#include "consh/arg.h"
#include "consh/cg.h"

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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con, *typ, *ds, *slot, *val;
//CON *c_val;
long from, to;
//PVAL p_val;
//PKIND p_kind;
//char buf[CMD_SIZE];
//bool added;
//LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */

if (args)
   {
   *out << _T("Too many args in IND ACTION command.") << std::endl;
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
	con = cg->acon_->con_nth_proxy(con, ord);
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
*
**************************************************/

bool ind_attr(
	LIST *args,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con, *slot, *c_val;
PVAL p_val;
PKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << std::endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok)
	return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok)
	return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok)
	return(false);

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent,cg, /*UP*/ &p_kind);
if (!ok)
	return(false);

/*** If proxy concept, get it. ***/
if (ord)
	{
	con = cg->acon_->con_nth_proxy(con, ord);
	if (!con)
		return(false);
	}

/* Get values.			*/
added = false;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok)
		return(false);
   
   if (!vals
		 || (!_tcscmp(_T("end"), ALIST::list_str(&vals,buf))
			  && vals->next										// PATCH	// 10/26/00 AM.
			  && !_tcscmp(_T("ind"),ALIST::list_str(&vals->next,buf)))	// PATCH	// 10/26/00 AM.
		)
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == pCON)
      {
      ok = path_to_con(vals,cg, &c_val);
      alist->list_free(vals, LNULL);
      p_val.vcon = c_val;
      if (!ok)
			return(false);
      }
   else
      {
      ok = cg->aptr_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok)
			return(false);
      }

   /* ADD THE ATTRIBUTE! */
   cg->kbm_->attr_add_val(con, slot, p_val, p_kind);
   added = true;
   }
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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con, *slot, *c_val;
PVAL p_val;
PKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */
long val_ord = 0;	// If concept/node is value, get ord.				// 10/09/05 AM.
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << std::endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok)
	return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok)
	return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok)
	return(false);

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent,cg, /*UP*/ &p_kind);
if (!ok)
	return(false);

/*** If proxy concept, get it. ***/
if (ord)
	{
	con = cg->acon_->con_nth_proxy(con, ord);
	if (!con)
		return(false);
	}

/* Get values.			*/
added = false;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok)
		return(false);
   
   if (!vals
		 || (!_tcscmp(_T("end"), ALIST::list_str(&vals,buf))
			  && vals->next										// PATCH	// 10/26/00 AM.
			  && !_tcscmp(_T("ind"),ALIST::list_str(&vals->next,buf)))	// PATCH	// 10/26/00 AM.
		)
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == pCON)
      {
      // Reworking this.			// 10/09/05 AM.
      ok = path_to_con(vals,cg, &c_val);
      if (!ok)
			{
			alist->list_free(vals, LNULL);
			return(false);
			}

		// Get node number.  0 = concept.								// 10/09/05 AM.
		ok = ui_read_num(_T("valord> "),in,out,i_flag,silent,cg,	// 10/09/05 AM.
															/*UP*/ &val_ord);	// 10/09/05 AM.
		alist->list_free(vals, LNULL);									// 10/09/05 AM.
		if (!ok)																	// 10/09/05 AM.
			return(false);														// 10/09/05 AM.

		if (val_ord)	// Get node.										// 10/09/05 AM.
			{
			c_val = cg->acon_->con_nth_proxy(c_val, val_ord);		// 10/09/05 AM.
			if (!c_val)															// 10/09/05 AM.
				return(false);													// 10/09/05 AM.
			}

      p_val.vcon = c_val;
      }
   else
      {
      ok = cg->aptr_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok)
			return(false);
      }

   /* ADD THE ATTRIBUTE! */
   cg->kbm_->attr_add_val(con, slot, p_val, p_kind);
   added = true;
   }
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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *wcon, *slot, *c_val;
PVAL p_val;
PKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << std::endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_word(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &wcon);
if (!ok)
	return(false);

/* Get ordinality of proxy in phrase.		*/
ok = ui_read_num(_T("ord> "), in, out, i_flag, silent,cg, /*UP*/ &ord);
if (!ok)
	return(false);

/* Get slot concept.	*/
ok = ui_read_con(_T("slot> "), in, out, i_flag, silent,cg, /*UP*/ &slot);
if (!ok)
	return(false);

/* Get value kind.		*/
ok = ui_read_pkind(_T("kind> "), in, out, i_flag, silent,cg, /*UP*/ &p_kind);
if (!ok)
	return(false);

/*** If proxy concept, get it. ***/
if (ord)
	{
	// Word concept can't be a proxy.  Should delete that line.
	// (Keeping it for consistency.)
	return(false);
	}

/* Get values.			*/
added = false;
for (;;)
   {
	if (!silent)															// 02/21/00 AM.
		*out << _T("val>  ");
   ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &vals);
   if (!ok)
		return(false);
   
   if (!vals
		 || (!_tcscmp(_T("end"), ALIST::list_str(&vals,buf))
			  && vals->next										// PATCH	// 10/26/00 AM.
			  && !_tcscmp(_T("ind"),ALIST::list_str(&vals->next,buf)))	// PATCH	// 10/26/00 AM.
		)
      {
      alist->list_free(vals, LNULL);
      break;
      }

   if (p_kind == pCON)
      {
      ok = path_to_con(vals,cg, &c_val);
      alist->list_free(vals, LNULL);
      p_val.vcon = c_val;
      if (!ok)
			return(false);
      }
   else
      {
      ok = cg->aptr_->s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
      alist->list_free(vals, LNULL);
      if (!ok)
			return(false);
      }

   /* ADD THE ATTRIBUTE! */
   cg->kbm_->attr_add_val(wcon, slot, p_val, p_kind);
   added = true;
   }
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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con;
_TCHAR buf[CMD_SIZE], nbuf[CMD_SIZE];
_TCHAR *str;
LIST *names, *save, *ends;
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND CHILDS command.") << std::endl;
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
   *out << _T("ind childs: Missing 'end ind'.") << std::endl;
alist->list_free(ends, LNULL);

/* ADD CHILDREN TO HIERARCHY */
save = names;
while ((str = ALIST::list_str(&names,nbuf)))
   cg->acon_->con_add_basic(str, con);

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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con, *proxy, *elt, *word;
_TCHAR buf[CMD_SIZE];
CON *elts[MAX_PHRASE_ELTS];
int count,			/* Count of elements. */
    ii;
PVAL pv;
LIST *list;
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << std::endl;
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
   if (!ok) return(false);
   
   /* First word = "end" terminates the addition. */
   if (!list || !_tcscmp(_T("end"), ALIST::list_str(&list,buf)))
      {
      alist->list_free(list, LNULL);
      break;
      }

   if (list->next)
      {
      std::_t_cerr << _T("ind named_phrase: Not handling multi-word names.") << std::endl;
      alist->list_free(list, LNULL);
      return(false);
      }

   word = cg->kbm_->dict_add_word(ALIST::list_str(&list,buf));
   alist->list_free(list, LNULL);
   if (!word) return(false);
   elts[count++] = word;
   
   if (count > MAX_PHRASE_ELTS)
      {
      std::_t_cerr << _T("ind named_phrase: More than ")
			  << MAX_PHRASE_ELTS
			  << _T(" elements.")
			  << std::endl;
      return(false);
      }
   }

if (!count) return(false);

/* Everything checked out, so build the phrase. */

proxy = cg->acon_->con_add_proxy(elts[0]);

/* Bind concept to first element of phrase. */
pv.vcon = proxy;
cg->kbm_->attr_add_val(con, cg->acon_->c_cg_PHRASE, pv, pCON);
pv.vcon = con;
cg->kbm_->attr_add_val(proxy, cg->acon_->c_cg_ATOM, pv, pCON);

for (ii = 1; ii < count; ii++)
   {
   elt = cg->acon_->con_add_proxy(elts[ii]);
   proxy->next = elt;
   elt->prev = proxy;
   proxy = elt;
   }
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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con, *proxy, *elt;
_TCHAR buf[CMD_SIZE];
CON *elts[MAX_PHRASE_ELTS];
int count,			/* Count of elements. */
    ii;
PVAL pv;
LIST *list;
ALIST *alist = cg->alist_;

if (args)
   {
   *out << _T("Too many args in IND ATTR command.") << std::endl;
   return(false);
   }

/* Get concept to add attribute to */
ok = ui_read_con(_T("con>  "), in, out, i_flag, silent,cg, /*UP*/ &con);
if (!ok)
	{
	*cgerr << _T("[ind phrase: Can't read concept.]") << std::endl;
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
		*cgerr << _T("[ind phrase: Can't get elt.]") << std::endl;
		return(false);
		}
   
   if (!list || !_tcscmp(_T("end"), ALIST::list_str(&list,buf)))
      {
      alist->list_free(list, LNULL);
      break;
      }

   ok = path_to_con(list,cg, &(elts[count++]));
   alist->list_free(list, LNULL);
   if (!ok)
		{
		*cgerr << _T("[ind phrase: Can't convert path to con.]") << std::endl;
		return(false);
		}
   
   if (count > MAX_PHRASE_ELTS)
      {
      //cerr
		*cgerr		// 10/05/99 AM.
			  <<_T("ind phrase: More than ")
			  << MAX_PHRASE_ELTS
			  << _T(" elements.")
			  << std::endl;
      return(false);
      }
   }

if (!count)
	{
	*cgerr << _T("[ind phrase: No count.]") << std::endl;	// 10/05/99 AM.
	return(false);
	}

/* Everything checked out, so build the phrase. */

proxy = cg->acon_->con_add_proxy(elts[0]);

/* Bind concept to first element of phrase. */
pv.vcon = proxy;
cg->kbm_->attr_add_val(con, cg->acon_->c_cg_PHRASE, pv, pCON);
pv.vcon = con;
cg->kbm_->attr_add_val(proxy, cg->acon_->c_cg_ATOM, pv, pCON);

for (ii = 1; ii < count; ii++)
   {
   elt = cg->acon_->con_add_proxy(elts[ii]);
   proxy->next = elt;
   elt->prev = proxy;
   proxy = elt;
   }
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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent
	)
{
bool ok;
CON *con, *slot, *c_val, *prox;
PVAL p_val;
PKIND p_kind;
_TCHAR buf[CMD_SIZE];
bool added;
LIST *vals;
long ord;			/* Ordinality of proxy in phrase. */

if (args)
   {
   *out << _T("Too many args in IND PROXY command.") << std::endl;
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
prox = cg->acon_->con_nth_proxy(con, ord);
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
      ok = s_to_pval(ALIST::list_str(&vals,buf), p_kind, &p_val);
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
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg
	)
{
bool ok;
CON *con, *typ, *ds, *slot, *val;
//CON *c_val;
long from, to;
//PVAL p_val;
//PKIND p_kind;
//char buf[CMD_SIZE];
//bool added;
//LIST *vals;
long ord;			/* Ordinality of proxy in con's phrase. */

if (args)
   {
   *out << _T("Too many args in IND ACTION command.") << std::endl;
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
	con = cg->acon_->con_nth_proxy(con, ord);
	if (!con) return(false);
	}

/*** ADD THE RULE ACTION ***/
return(rest_add(con, typ, ds, slot, val, from, to));
}
#endif
