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
*									UI.C
*
* FILE:	consh.¹/ui.c
* SUBJ:	UI primitives for Consh.
* CR:	10/15/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <fstream.h>			// 04/20/99 AM.
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <ctype.h>

#include "prim/libprim.h"
#include "cg_global.h"			// 10/05/99 AM.

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
#include "kbm/attr.h"
#include "kbm/dict.h"				// 05/30/02 AM.
#include "kbm/Akbm.h"

#include "consh/libconsh.h"		// 04/26/99 AM.
#include "consh/bind.h"
#include "consh/cc_var.h"
#include "consh/arg.h"
#include "consh/cg.h"

//#include "Con_ini.h"
#include "cc_gen.h"
#include "st_gen.h"
#include "sym_gen.h"
#include "con_gen.h"
#include "ptr_gen.h"
#include "ind.h"
#include "cmd.h"
#include "ui.h"


/**************************************************
*						PATH_TO_CON
* FUN:	path_to_con
* SUBJ:	Convert hierarchy path to a concept.
* CR:	10/15/95 AM.
* RET:	ok - If ok concept.
*
**************************************************/

bool
path_to_con(
	LIST *args,
	CG *cg,
	/*UP*/
	CON **upcon
	)
{
_TCHAR *name;
CON *con, *sub, *child;
ALIST* alist = cg->alist_;

*upcon = CNULL;

/* Set up root of hierarchy. */
name = ALIST::list_pop_buf(&args,alist->List_buffer);
if (!name || !*name)	// 10/31/06 AM.
   {
   std::_t_cerr << _T("[path_to_con: Empty args.]") << std::endl;	// 10/31/06 AM.
   return(false);	// 10/31/06 AM.
   }
if (!_tcscmp(name, CON_ROOT_NAME))
   ;
else if (!_tcscmp(name, CON_EMPTY_NAME))
   {
   /* 12/24/95 AM. Handle empty concept specially. */
   if (args)
      {
      std::_t_cerr << _T("[path_to_con: Nil concept can't have child.]") << std::endl;
      return(false);
      }
   *upcon = cg->acon_->Con((ID) 0);
   return(true);
   }
else
   {
   std::_t_cerr << _T("[path_to_con: Path must begin with '")
		  << CON_ROOT_NAME << _T("'.]") << std::endl;
   return(false);
   }
   
con = cg->acon_->c_cg_CONCEPT;											// 08/22/02 AM.

/* Find first path component not yet built. */
while ((name = ALIST::list_pop_buf(&args,alist->List_buffer)))
   {
   child = con->dn;
   sub = CNULL;
   while (child)
      {
      if ((child->kind == cBASIC && !_tcscmp(name, child->v.vname->str)) ||
          (child->kind == cWORD  && !_tcscmp(name, child->v.vsym->str )) )
         {
         sub = child;
         break;
         }
      child = child->next;
      }
   if (sub)
      con = sub;
   else
      break;
   }

if (name)
   {
   //cerr
	*cgerr
		<< _T("[path_to_con: Concept not found.]") << std::endl;	// 10/05/99 AM.
	*cgerr << _T("[name=*") << name << _T("*]") << std::endl;			// 10/05/99 AM.
	*cgerr << _T("[namech=") << (_TUCHAR) *name << _T("]") << std::endl;
	if (ALIST::list_pop(&args))	// Still more.			// 07/08/03 AM.
		return false;												// 07/08/03 AM.
	// ADD THE CONCEPT!!  (eg, newstyle numeric)			// 07/08/03 AM.
	bool dirt;														// 07/08/03 AM.
	if ((*upcon = cg->kbm_->dict_get_word(name,dirt)))	// 07/08/03 AM.
		return true;												// 07/08/03 AM.
   return(false);
   }
*upcon = con;
return(true);
}


/**************************************************
*						UI_READ_CON
* FUN:	ui_read_con
* SUBJ:	Read command line as a concept.
* CR:	10/16/95 AM.
* RET:	ok - If ok concept.
*		(up) con - Concept read in.
*
**************************************************/

bool
ui_read_con(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	CON **con
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;

if (!silent)																	// 02/21/00 AM.
	*out << prompt << std::flush;
ok  = args_read(in, out, silent,cg->alist_, buf,CMD_SIZE, &args);
if (!ok) return(false);
ok = path_to_con(args,cg, /*UP*/ con);
cg->alist_->list_free(args, LNULL);
return(ok);
}


/**************************************************
*						UI_READ_END
* FUN:	ui_read_end
* SUBJ:	Read end line of multi-line command.
* CR:	12/25/95 AM.
* RET:	ok - If ok.
*
**************************************************/

bool
ui_read_end(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	ALIST *alist
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;

if (!silent)																// 02/21/00 AM.
	*out << prompt << std::flush;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);

if (!_tcscmp(_T("end"), ALIST::list_str(&args,buf)))
   ok = true;
else
   ok = false;
alist->list_free(args, LNULL);
return(ok);
}

/**************************************************
*						UI_READ_NUM
* FUN:	ui_read_num
* SUBJ:	Read command line as a long number.
* CR:	11/15/95 AM.
* RET:	ok - If ok concept.
*		(up) num - Concept read in.
*
**************************************************/

bool
ui_read_num(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	long *num
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

if (!silent)																// 02/21/00 AM.
	*out << prompt << std::flush;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (!args)	// 10/31/06 AM.
   {
   *cgerr << _T("[ui_read_num: Number not found.]") << std::endl;	// 10/31/06 AM.
   return false;	// 10/31/06 AM.
   }
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

ok = s_to_l(ALIST::list_str(&args,buf), /*UP*/ num);
alist->list_free(args, LNULL);
return(ok);
}


/**************************************************
*						UI_READ_PKIND
* FUN:	ui_read_pkind
* SUBJ:	Read command line as a ptr kind.
* CR:	10/16/95 AM.
* RET:	ok - If ok concept.
*		(up) pkind - Concept read in.
*
**************************************************/

bool
ui_read_pkind(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	PKIND *pkind
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

if (!silent)																// 02/21/00 AM.
	*out << prompt << std::endl;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

ok = cg->aptr_->s_to_pkind(ALIST::list_str(&args,buf), /*UP*/ pkind);
alist->list_free(args, LNULL);
return(ok);
}


/**************************************************
*						UI_READ_SYM
* FUN:	ui_read_sym
* SUBJ:	Read command line as a symbol table entry.
* CR:	12/20/95 AM.
* RET:	ok - If ok concept.
*		(up) sym - Sym for string read in.
*
**************************************************/

bool
ui_read_sym(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	SYM **sym
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

ok = true;
if (!silent)																// 02/21/00 AM.
	*out << prompt << std::endl;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

bool dirt;	// 06/29/03 AM.
if (!((*sym) = cg->asym_->sym_get(ALIST::list_str(&args,buf),dirt)))
   ok = false;

alist->list_free(args, LNULL);
return(ok);
}


/**************************************************
*						UI_READ_WORD
* FUN:	ui_read_word
* SUBJ:	Read command line as a dictionary word.
* CR:		05/30/02 AM.
* RET:	ok - If ok concept.
*		(up) wordcon - Concept for word read in.
*
**************************************************/

bool
ui_read_word(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	CON **wcon
	)
{
*wcon = 0;
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

ok = true;
if (!silent)
	*out << prompt << std::endl;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

#ifdef OLD_030629_
if (!((*wcon) = cg->kbm_->dict_find_word(ALIST::list_str(&args,buf))))
	{
	if (!((*wcon) = cg->kbm_->dict_add_word(ALIST::list_str(&args,buf))))
		ok = false;
	}
#endif

bool dirt;																		// 06/29/03 AM.
*wcon = cg->kbm_->dict_get_word(ALIST::list_str(&args,buf),dirt);			// 06/29/03 AM.
if (!*wcon)																		// 06/29/03 AM.
	ok = false;																	// 06/29/03 AM.

alist->list_free(args, LNULL);
return(ok);
}
