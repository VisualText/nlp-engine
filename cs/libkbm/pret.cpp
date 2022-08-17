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
*									PRET.C
*
* FILE:	consh.�/pret.c
* SUBJ:	Pretty printers that mix modules.
* CR:	3/30/96 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prim/libprim.h"
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include "kbm/libkbm.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"

#include "ext/kb.h"

#include "kbm/kbm_kb.h"
#include "kbm/pret.h"


/**************************************************
*						CON_NTH_PROXY
* FUN:	con_nth_proxy
* SUBJ:	Get the nth proxy of given concept.
* CR:		11/15/95 AM.
* RET:	prox - nth proxy, if present.
*
**************************************************/

CON *ACON::con_nth_proxy(
	CON *con,
	long nth
	)
{
CON *phr;

if (!con || nth <= (long) 0)
   return(CNULL);
phr = con_phrase(con);		/* Get phrase of proxies. */

return(con_nth(phr, nth));
}
