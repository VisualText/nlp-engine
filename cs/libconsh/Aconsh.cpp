/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ACONSH.CPP
* FILE:	c:/cs/libconsh/Aconsh.cpp
* CR:		12/09/98 AM.
* SUBJ:	API functions for Conceptual Grammar KBMS.
*
*******************************************************************************/

#include "StdAfx.h"
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "consh/libconsh.h"

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/str.h"
#include "prim/list_s.h"
#include "prim/list.h"


#include "kbm/libkbm.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/kbm.h"
#include "kbm/st.h"	// 10/30/06 AM.
#include "kbm/Akbm.h"			// 06/11/02 AM.

#include "cc_code.h"
#include "consh/bind.h"			// 06/09/99 AM.
#include "consh/Aconsh.h"
#include "consh/cg.h"

/********************************************
* FN:		CONSH_INI
* CR:		04/26/99 AM.
* SUBJ:	Initialize kb structures.
********************************************/
LIBCONSH_API bool																// 05/05/01 AM.
consh_ini(void *cg)															// 06/11/02 AM.
{
/* Initializations */
#ifdef LINUX
if (!ccx_ini(cg)) return false;	// Generated code inits.		// 03/23/19 AM.
#else
if (!cc_ini(cg)) return false;	// Generated code inits.		// 08/15/02 AM.
#endif
//if (!kbm_ini()) return false;											// 05/05/01 AM.
return true;																	// 05/05/01 AM.
}


/********************************************
* FN:		CONSH_CLEAN
* CR:		04/26/99 AM.
* SUBJ:	Clean up kb structures.
********************************************/
LIBCONSH_API void
consh_clean(CG *cg)
{
if (cg->kbm_)	// 07/28/03 AM.
	cg->kbm_->kbm_clean();

// NEED TO CLEAN UP ALL GLOBALS. // 06/09/99 AM.
unbind_all(cg);																// 08/22/02 AM.
}
