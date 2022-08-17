/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/st.h"	// 10/30/06 AM.
#include "kbm/Akbm.h"

#include "ext/kb.h"
#include "kbm_priv.h"
#include "kbm/kbm_kb.h"


/**************************************************
*						CON_PHRASE
* FUN:	con_phrase
* SUBJ:	Get the phrase associated with the given concept.
* CR:	11/29/95 AM.
* NOTE:	Moving from libconan to libkbm.
*
**************************************************/

CON *ACON::con_phrase(
	CON *con
	)
{
return(akbm_->attr_get_c(con, c_cg_PHRASE));
}

/**************************************************
*						PHRASE_CON
* FUN:	phrase_con
* SUBJ:	Get the atomic concept associated with the given phrase.
* CR:		05/04/99 AM.
*
**************************************************/

CON *ACON::phrase_con(
	CON *phr
	)
{
return(akbm_->attr_get_c(phr, c_cg_ATOM));
}
