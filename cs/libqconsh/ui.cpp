/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									UI.C
*
* FILE:	consh.�/ui.c
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

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xcon_s.h"
#include "qkbm/xptr_s.h"
#include "qkbm/Aqkbm.h"

/*
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
*/
#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "cgdict.h"
#include "qconsh/cg.h"
#include "qconsh/bind.h"
//#include "qconsh/consh_kb.h"		// OBSOLETE.	02/19/07 AM.
#include "qconsh/arg.h"
#include "cgcon.h"	// 03/08/07 AM.

//#include "Con_ini.h"
//#include "cc_gen.h"
//#include "st_gen.h"
//#include "sym_gen.h"
//#include "con_gen.h"
//#include "ptr_gen.h"
#include "ind.h"
#include "cmd.h"
#include "ui.h"


/**************************************************
*						PATH_TO_CON
* FUN:	path_to_con
* SUBJ:	Convert hierarchy path to a concept.
* CR:	10/15/95 AM.
* RET:	ok - If ok concept.
*			UP con - con struct from db mgr.
* WARN:	USER MUST FREE THE FETCHED CONCEPT STRUCT. 02/14/07 AM.
*
**************************************************/

bool
path_to_con(
	LIST *args,
	CG *cg,
	/*UP*/
	XCON_S **upcon
	)
{

_TCHAR *name;
XCON_S *con, *sub, *child, *tmp;
CON_ID cid, sid;
_TCHAR *buf = cg->alist_->List_buffer;

*upcon = 0;

/* Set up root of hierarchy. */
name = ALIST::list_pop_buf(&args,buf);
if (!name || !*name)	// 10/31/06 AM.
   {
   _t_cerr << _T("[path_to_con: Empty args.]") << endl;	// 10/31/06 AM.
   return(false);	// 10/31/06 AM.
   }
if (!_tcscmp(name, CON_ROOT_NAME))
   ;
else if (!_tcscmp(name, CON_EMPTY_NAME))
   {
   /* 12/24/95 AM. Handle empty concept specially. */
   if (args)
      {
      _t_cerr << _T("[path_to_con: Nil concept can't have child.]") << endl;
      return(false);
      }
   *upcon = (XCON_S *) 0;
   return(true);
   }
else
   {
   _t_cerr << _T("[path_to_con: Path must begin with '")
		  << CON_ROOT_NAME << _T("'.]") << endl;
   return(false);
   }
   
con = cg->findRoot_c();
cid = sid = (CON_ID) 1;	// Root concept id.

/* Find first path component not yet built. */
_TCHAR *nm;
while (name = ALIST::list_pop_buf(&args,buf))
   {
   sid = con->dn;
   child = cg->qkbm_->Con(sid);
   sub = CNULL;
   while (child)
      {
      if (child->kind == cXBASIC || child->kind == cXWORD)
			{
			nm = cg->qkbm_->con_str(child);
			if (!_tcscmp(name, nm))
				{
				cg->qkbm_->kbfree(nm);
				sub = child;
				break;
				}
			cg->qkbm_->kbfree(nm);
			}
		tmp = child;
		child = cg->qkbm_->Con(child->next);
		cg->kbfree((char *)tmp);
      }
   if (sub)
		{
      con = sub;
      cid = sid;
      }
   else
      break;
   }

XCON_S *wcon;	// 02/15/07 AM.
if (name)
   {
   //cerr
	*cgerr
		<< _T("[path_to_con: Concept not found.]") << endl;	// 10/05/99 AM.
	*cgerr << _T("[name=*") << name << _T("*]") << endl;			// 10/05/99 AM.
	*cgerr << _T("[namech=") << (_TUCHAR) *name << _T("]") << endl;
	if (ALIST::list_pop_buf(&args,buf))	// Still more.			// 07/08/03 AM.
		return false;												// 07/08/03 AM.
	// ADD THE CONCEPT!!  (eg, newstyle numeric)			// 07/08/03 AM.
	bool dirt;														// 07/08/03 AM.
	if (wcon = cg->cgdict_->dict_get_word(name,dirt))	// 07/08/03 AM.
		{
		*upcon = wcon;							// 02/15/07 AM.
		return true;												// 07/08/03 AM.
		}
   return(false);
   }
*upcon = con;
return(true);
}



/**************************************************
*						PATH_TO_CON
* FUN:	path_to_con
* SUBJ:	Convert hierarchy path to a concept. [VARIANT]
* CR:		03/08/07 AM.
* RET:	ok - If ok concept.
*			UP con - User gets con from path[lpath].
*			UP str = User gets con str from spath[lpath]
*
**************************************************/

bool
path_to_con(
	LIST *args,
	CG *cg,
	XCON_S **path,
	_TCHAR **spath,
	int &lpath
	)
{

_TCHAR *name;
XCON_S *con, *sub, *child, *tmp;
_TCHAR *buf = cg->alist_->List_buffer;
//CON_ID cid, sid;


/* Set up root of hierarchy. */
name = ALIST::list_pop_buf(&args,buf);
if (!name || !*name)	// 10/31/06 AM.
   {
   _t_cerr << _T("[path_to_con: Empty args.]") << endl;	// 10/31/06 AM.
   return(false);	// 10/31/06 AM.
   }
if (!_tcscmp(name, CON_ROOT_NAME))
   ;
else if (!_tcscmp(name, CON_EMPTY_NAME))
   {
   /* 12/24/95 AM. Handle empty concept specially. */
   if (args)
      {
      _t_cerr << _T("[path_to_con: Nil concept can't have child.]") << endl;
      return(false);
      }
//   *upcon = (XCON_S *) 0;
   return(true);
   }
else
   {
   _t_cerr << _T("[path_to_con: Path must begin with '")
		  << CON_ROOT_NAME << _T("'.]") << endl;
   return(false);
   }

// Set up match to root concept.	// 03/08/07 AM.
int depth = 0;	// Root is at depth zero. // ZERO-BASED.
if (lpath < 0)	// No tracking yet.
	{
	con = cg->findRoot_c();
//	cid = sid = (CON_ID) 1;	// Root concept id.
	lpath = 0;
	path[lpath] = con;
	// For formality, get string as well.
	spath[lpath] = cg->qkbm_->con_str(con);
	}
else	// Get root concept from tracking.
	{
	con = path[depth];
//	cid = sid = con->id;
	}

/* Find first path component not yet built. */
_TCHAR *nm;
while (name = ALIST::list_pop_buf(&args,buf))
   {
//   sid = con->dn;
   ++depth;
   // If within tracked path and matched, then keep going.
   if (depth <= lpath && !_tcscmp(name, spath[depth]))
		{
		con = path[depth];
		}
 	else
		{
		// Clear the tracked path from point of difference, if any.
		if (depth <= lpath)
			cg->cgcon_->clear_path(depth,path,spath,lpath);

		// Past the depth of tracked path.  Append to tracked path when child found.
		child = cg->qkbm_->Con(con->dn);	// 03/08/07 AM.
		sub = 0;
		while (child)
			{
			if (child->kind == cXBASIC || child->kind == cXWORD)
				{
				nm = cg->qkbm_->con_str(child);
				if (!_tcscmp(name, nm))
					{
					// Found a matching child.  Add to tracking.
					++lpath;
					path[lpath] = child;
					spath[lpath] = nm;
					sub = child;
					break;
					}
				cg->qkbm_->kbfree(nm);
				}
			tmp = child;
			child = cg->qkbm_->Con(child->next);
			cg->kbfree((char *)tmp);
			}
		if (sub)
			{
			con = sub;
//			cid = sid;
			}
		else
			break;
		}
   }

if (name)
   {
   //cerr
	*cgerr
		<< _T("[path_to_con: Concept not found.]") << endl;	// 10/05/99 AM.
	*cgerr << _T("[name=*") << name << _T("*]") << endl;			// 10/05/99 AM.
	*cgerr << _T("[namech=") << (_TUCHAR) *name << _T("]") << endl;
	// NOT ADDING MISSING CONCEPT.	// 03/08/07 AM.
   return(false);
   }
//*upcon = con;
return(true);
}



/**************************************************
*						UI_READ_CON
* FUN:	ui_read_con
* SUBJ:	Read command line as a concept.
* CR:	10/16/95 AM.
* RET:	ok - If ok concept.
*		(up) con - Concept read in.
* WARN:	USER MUST FREE CONCEPT.
*
**************************************************/

bool
ui_read_con(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	XCON_S **con
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;

if (!silent)																	// 02/21/00 AM.
	*out << prompt << flush;
ok  = args_read(in, out, silent,cg->alist_, buf,CMD_SIZE, &args);
if (!ok) return(false);
ok = path_to_con(args,cg, /*UP*/ con);
cg->alist_->list_free(args, LNULL);
return(ok);
}




/**************************************************
*						UI_READ_CON
* FUN:	ui_read_con
* SUBJ:	Read command line as a concept. [VARIANT]
* CR:		03/08/07 AM.
* RET:	ok - If ok concept.
*		(up) con - User can get concept from path[lpath].  DON'T FREE IT!
*			  str - User can get con str from spath[lpath]. DON'T FREE IT!
* OPT:	Using structures to the latest concept path in hierarchy.
*
**************************************************/

bool
ui_read_con(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	XCON_S **path,
	_TCHAR **spath,
	int &lpath
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;

if (!silent)																	// 02/21/00 AM.
	*out << prompt << flush;
ok  = args_read(in, out, silent,cg->alist_, buf,CMD_SIZE, &args);
if (!ok) return(false);
ok = path_to_con(args,cg,path,spath,lpath);
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
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	ALIST *alist
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;

if (!silent)																// 02/21/00 AM.
	*out << prompt << flush;
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
	_t_istream *in,
	_t_ostream *out,
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
	*out << prompt << flush;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (!args)	// 10/31/06 AM.
   {
   *cgerr << _T("[ui_read_num: Number not found.]") << endl;	// 10/31/06 AM.
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
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	XPKIND *pkind
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

if (!silent)																// 02/21/00 AM.
	*out << prompt << endl;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

ok = cg->qkbm_->s_to_pkind(ALIST::list_str(&args,buf), /*UP*/ pkind);
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
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	STR_ID *sym
	)
{
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

ok = true;
if (!silent)																// 02/21/00 AM.
	*out << prompt << endl;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

bool dirt;	// 06/29/03 AM.
if (!((*sym) = cg->qkbm_->sym_get(ALIST::list_str(&args,buf),dirt)))
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
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	/*UP*/
	XCON_S **wcon
	)
{
*wcon = 0;
_TCHAR buf[CMD_SIZE];
LIST *args;
bool ok;
ALIST *alist = cg->alist_;

ok = true;
if (!silent)
	*out << prompt << endl;
ok  = args_read(in, out, silent,alist, buf,CMD_SIZE, &args);
if (!ok) return(false);
if (args->next)
   {
   alist->list_free(args, LNULL);
   return(false);			/* Too many args. */
   }

#ifdef OLD_030629_
if (!((*wcon) = cg->cgdict_->dict_find_word(ALIST::list_str(&args,buf))))
	{
	if (!((*wcon) = cg->cgdict_->dict_add_word(ALIST::list_str(&args,buf))))
		ok = false;
	}
#endif

bool dirt;																		// 06/29/03 AM.
*wcon = cg->cgdict_->dict_get_word((_TCHAR*)args->val,dirt);			// 06/29/03 AM.
if (!*wcon)																		// 06/29/03 AM.
	ok = false;																	// 06/29/03 AM.

alist->list_free(args, LNULL);
return(ok);
}
