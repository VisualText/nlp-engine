/*******************************************************************************
Copyright (c) 1998,1999 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
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
u#include "consh/libconsh.h"

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/str.h"
#include "prim/list_s.h"
#include "prim/list.h"

/*
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
*/
#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"
#include "qconsh/bind.h"

/********************************************
* FN:		CONSH_INI
* CR:		04/26/99 AM.
* SUBJ:	Initialize kb structures.
********************************************/
LIBQCONSH_API bool																// 05/05/01 AM.
consh_ini(void *cg)															// 06/11/02 AM.
{
/* Initializations */
//if (!cc_ini(cg)) return false;	// Generated code inits.		// 08/15/02 AM.	// 02/14/07 AM.
//if (!kbm_ini()) return false;											// 05/05/01 AM.
return true;																	// 05/05/01 AM.
}


/********************************************
* FN:		CONSH_CLEAN
* CR:		04/26/99 AM.
* SUBJ:	Clean up kb structures.
********************************************/
LIBQCONSH_API void
consh_clean(CG *cg)
{
//if (cg->qkbm_)	// 07/28/03 AM.
//	cg->qkbm_->kbm_clean();

// NEED TO CLEAN UP ALL GLOBALS. // 06/09/99 AM.
unbind_all(cg);																// 08/22/02 AM.
}
