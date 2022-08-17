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
*									KBM.C
*
* FILE:	ext.�/kbm.c
* SUBJ:	Knowledge base management.
* CR:	9/26/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <string.h>

#include "prim/prim.h"
//#include "prim/mach.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/st.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/kbm.h"
#include "kbm/Akbm.h"

//#include "pn_s.h"	// Don't want libconan here.	// 04/16/99 AM.
//#include "pn.h"		// Don't want libconan here.	// 04/16/99 AM.

/* Keep these two files in sync. */
#include "kbm_priv.h"
#include "kbm_def.h"


/**************************************************
*						KBM_VARS
* FUN:	kbm_vars
* SUBJ:	Get global kb variables needed by this library.
* CR:		3/31/96 AM.
* NOTE:	Consh and Conan applications have different semantics for "ID".
*			In Consh, they are id numbers, in Conan apps, they are (CON *).
*			Since most libraries will be used by both, the nature of these
*			globals must be hidden from the libraries.  Thus, any global
*			variable used by the kbm library must be handed to kbm_vars().
*
**************************************************/

void AKBM::kbm_vars(
	CON *cg_concept,
	CON *cg_phrase,		// 04/16/99 AM.
	CON *cg_atom,			// 05/04/99 AM.
	CON *dict_alpha,
	CON *dict_unknown,
	CON *id_action,
	CON *id_rest,
	CON *nlp_punct,
	CON *nlp_wht
	)
{
acon_->c_cg_CONCEPT		= cg_concept;
acon_->c_cg_PHRASE			= cg_phrase;		// 04/16/99 AM.
acon_->c_cg_ATOM			= cg_atom;			// 05/04/99 AM.
acon_->c_dict_ALPHA		= dict_alpha;
acon_->c_dict_UNKNOWN		= dict_unknown;
//acon_->c_id_ACTION			= id_action;
//acon_->c_id_REST			= id_rest;
acon_->c_nlp_PUNCT			= nlp_punct;
acon_->c_nlp_WHT			= nlp_wht;
}


/**************************************************
*						KBM_CLEAN
* FUN:	kbm_ini
* SUBJ:	Clean up kb mgr systems prior to exit.
* CR:	9/26/95 AM.
*
**************************************************/

void AKBM::kbm_clean()
{
//list_clean();	// DO THIS OUTSIDE THE KBM.						// 08/14/02 AM.
if (ast_)	// 07/28/03 AM.
	ast_->st_clean();
if (asym_)	// 07/28/03 AM.
	asym_->sym_clean();
if (acon_)	// 07/28/03 AM.
	acon_->con_clean();
if (aptr_)	// 07/28/03 AM.
	aptr_->ptr_clean();
}

/**************************************************
*						KBM_INI
* FUN:	kbm_ini
* SUBJ:	Initialize kb mgr systems.
* CR:	9/26/95 AM.
*
**************************************************/

#ifdef OLD_020815_
LIBKBM_API bool																// 05/05/01 AM.
kbm_ini()
{
// DO THIS OUTSIDE THE KBM.												// 08/14/02 AM.
//if (!list_ini()) return false;											// 05/05/01 AM.
/*
st_ini();
sym_ini();
con_ini();
ptr_ini();
*/
return true;																	// 05/05/01 AM.
}
#endif
