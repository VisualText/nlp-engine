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
*									BIND.C
*
* FILE:	libconsh/bind.c
* SUBJ:	Bindings of C variables to knowledge base concepts.
* NOTE:	
* CR:	9/18/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/26/99 AM.
#include <stdio.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "kbm/libkbm.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/kbm.h"
#include "kbm/Akbm.h"
//#include "cc_var.h"

#include "consh/libconsh.h"	// 04/26/99 AM.
#include "consh/cg.h"
#include "consh/bind.h"

#define bind_var(var, parent, name) \
	if (!(var = cg->acon_->id_get_child(parent, name))) return(false);

#ifdef OLD_020822_
/* Put these in an include file. */
LIBCONSH_API ID
	cg_CONCEPT,		/* Root concept of the knowledge base.	*/
	con_SYS,
//	con_APP,con_GEN,
	sys_ATTRS,sys_DICT,sys_NLP,
//	nlp_ROOT,
//	nlp_LINE,
	nlp_WHT,nlp_PUNCT,
// nlp_CTRL,
	dict_ALPHA,
//	dict_UNKNOWN,
	attrs_PROXY,attrs_PPROXY,
//	attrs_COMMON,
	cg_PHRASE,			/* Attribute pointing to con's phrase.	*/
//	cg_VAR,
//	id_VALS, id_BOOLEAN, id_FALSE, id_TRUE,
	cg_ATOM			/* Attr pointing to phrase's concept.	*/
	;
#endif

/* Probably Consh-specific bindings. */
//LIBCONSH_API ID
//	id_RULES;

/* Application-specific bindings. */
//LIBCONSH_API ID
//	id_TRIGGERS,
//	app_RESUME,
//	res_SEQUENCE
//	;

/**************************************************
*						BIND_ALL
* FUN:	bind_all
* SUBJ:	Bind all kb concepts to global variables.
* CR:	10/21/95 AM.
* RET:	ok - If command successful.
* NOTE:	Conan - call this as part of system init. NO LONGER. 12/4/95 AM.
*		Consh - use only if starting with a prebuilt kb.
* ASS:	cg_CONCEPT already defined.
*
**************************************************/

bool
bind_all(CG *cg)
{
return bind_sys(cg);
}


/**************************************************
*						BIND_SYS
* FUN:	bind_sys
* SUBJ:	Bind system internal concepts with code.
* CR:	10/21/95 AM.
* RET:	ok - If command successful.
* NOTE:	Consh - Part of bootstrapping the knowledge base.
* ASS:	cg_CONCEPT, the root of hierarchy, is defined.
*
**************************************************/

LIBCONSH_API bool
bind_sys(CG *cg)
{
ID con_SYS
	, sys_ATTRS
	, sys_DICT
	, sys_NLP
	, dict_ALPHA
	, attrs_PROXY
	, attrs_PPROXY
	;
std::_t_cerr << _T("[bind_sys: Enter.]") << std::endl;
if (!cg)
  {
  std::_t_cerr << _T("[bind_sys: EMPTY cg object.]") << std::endl;
  return(false);
  }
if (!cg->cg_CONCEPT)
  {
  std::_t_cerr << _T("[bind_sys: No cg_CONCEPT.]") << std::endl;
  return(false);
  }

#ifdef LINUX // Debugging kb load.
std::_t_cerr << _T("[bind_sys]: Have cg_CONCEPT=") << cg->cg_CONCEPT << std::endl;
CON *x=	cg->acon_->Con(cg->cg_CONCEPT);
std::_t_cerr << _T("[bind_sys]: Conid to con to id= ") << x->id << std::endl;
#endif

// 04/30/99 AM. Removing Analyzer-specific stuff.
bind_var(con_SYS,				cg->cg_CONCEPT,			_T("sys"));
//std::_t_cerr << _T("[bind_sys]: Have con_SYS=") << con_SYS << std::endl;
bind_var(sys_ATTRS,			con_SYS,				_T("attrs"));
bind_var(sys_DICT,			con_SYS,				_T("dict"));
bind_var(sys_NLP,				con_SYS,				_T("nlp"));
//bind_var(id_VALS,				con_SYS,				"vals");
//bind_var(id_BOOLEAN,			id_VALS,				"boolean");
//bind_var(id_FALSE,			id_BOOLEAN,			"false");
//bind_var(id_TRUE,				id_BOOLEAN,			"true");
bind_var(cg->nlp_WHT,				sys_NLP,				_T("wht"));
bind_var(cg->nlp_PUNCT,			sys_NLP,				_T("punct"));
//bind_var(nlp_CTRL,			sys_NLP,				"ctrl");
bind_var(dict_ALPHA,			sys_DICT,			_T("a"));
//bind_var(dict_UNKNOWN,		sys_DICT,			"unknown");
bind_var(attrs_PROXY,		sys_ATTRS,			_T("proxy"));
bind_var(attrs_PPROXY,		sys_ATTRS,			_T("pproxy"));
//bind_var(attrs_COMMON,		sys_ATTRS,			"common");
bind_var(cg->cg_ATOM,				attrs_PROXY,		_T("atom"));
bind_var(cg->cg_PHRASE,			attrs_PPROXY,		_T("phrase"));
//bind_var(cg_VAR,				attrs_COMMON,		"var");

/* 4/30/96 AM. Must tell libraries about bound concepts. */
//std::_t_cerr << _T("[bind_sys: Before kbm_vars.]") << std::endl;
cg->kbm_->kbm_vars(
	cg->acon_->Con(cg->cg_CONCEPT),
	cg->acon_->Con(cg->cg_PHRASE),		// 04/16/99 AM.
	cg->acon_->Con(cg->cg_ATOM),		// 05/04/99 AM.
	cg->acon_->Con(dict_ALPHA),
	0,										// 08/22/02 AM.
	0,										// 04/30/99 AM.
	0,										// 04/30/99 AM.
	cg->acon_->Con(cg->nlp_PUNCT),
	cg->acon_->Con(cg->nlp_WHT)
	);

//std::_t_cerr << _T("[bind_sys: Exit ok.]") << std::endl;
return(true);
}


/**************************************************
*						BIND_APP
* FUN:	bind_app
* SUBJ:	Bind application-specific concepts with code.
* CR:	12/4/95 AM.
* RET:	ok - If command successful.
* NOTE:	Consh - Part of bootstrapping the knowledge base.
*
**************************************************/

LIBCONSH_API bool
bind_app()
{

// OBSOLETE.	// 07/23/01 AM.
//if (!(app_RESUME	= id_get_child(con_APP,	 	  "resume")))	return(false);
//if (!(res_SEQUENCE	= id_get_child(app_RESUME,	   "seq"  )))	return(false);

return(true);
}


/**************************************************
*						UNBIND_ALL
* FUN:	unbind_all
* SUBJ:	Bind application-specific concepts with code.
* CR:	12/4/95 AM.
* RET:	ok - If command successful.
* NOTE:	Consh - Part of bootstrapping the knowledge base.
*
**************************************************/

bool unbind_all(CG *cg)
{
cg->cg_CONCEPT =		/* Root concept of the knowledge base.	*/
//con_SYS =
// con_APP = con_GEN =
//sys_ATTRS = sys_DICT = sys_NLP = 
// nlp_ROOT = 
// nlp_LINE =
cg->nlp_WHT = 
cg->nlp_PUNCT =
// nlp_CTRL = 
//dict_ALPHA =
//  dict_UNKNOWN = 
//attrs_PROXY = attrs_PPROXY =
// attrs_COMMON =
cg->cg_PHRASE =			/* Attribute pointing to con's phrase.	*/
// cg_VAR =
//id_VALS = id_BOOLEAN = id_FALSE = id_TRUE =
cg->cg_ATOM			/* Attr pointing to phrase's concept.	*/
= 0;

/* Probably Consh-specific bindings. */
// id_RULES = 0;

/* Application-specific bindings. */
// id_TRIGGERS =
// app_RESUME =
// res_SEQUENCE
// = 0;
return true;
}
