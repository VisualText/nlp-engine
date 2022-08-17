/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									BIND.C
*
* FILE:	consh.�/bind.c
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

/*
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
*/

#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"


#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"
#include "cgcon.h"
#include "qconsh/bind.h"

#define bind_var(var, parent, name) \
	if (!(var = cg->cgcon_->id_get_child(parent, name))) return(false);


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

LIBQCONSH_API bool
bind_sys(CG *cg)
{
CON_ID con_SYS
	, sys_ATTRS
	, sys_DICT
//	, sys_NLP		// 02/28/07 AM.
//	, dict_ALPHA	// 02/27/07 AM.
	, attrs_PROXY
	, attrs_PPROXY
	;
if (!cg->cg_CONCEPT)		return(false);

// 04/30/99 AM. Removing Analyzer-specific stuff.
bind_var(con_SYS,				cg->cg_CONCEPT,			_T("sys"));
bind_var(sys_ATTRS,			con_SYS,				_T("attrs"));
bind_var(sys_DICT,			con_SYS,				_T("dict"));
//bind_var(sys_NLP,				con_SYS,				_T("nlp"));	// 02/28/07 AM.
bind_var(cg->nlp_WHT,				sys_DICT,				_T("wht"));	// 02/28/07 AM.
bind_var(cg->nlp_PUNCT,			sys_DICT,				_T("pnct"));	// 02/28/07 AM.
//bind_var(nlp_CTRL,			sys_NLP,				"ctrl");
bind_var(cg->dict_ALPHA,			sys_DICT,			_T("a"));	// 02/27/07 AM.
bind_var(attrs_PROXY,		sys_ATTRS,			_T("proxy"));
bind_var(attrs_PPROXY,		sys_ATTRS,			_T("pproxy"));

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

LIBQCONSH_API bool
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
cg->nlp_WHT = 
cg->nlp_PUNCT =
// nlp_CTRL = 
//dict_ALPHA =
 0;
return true;
}
