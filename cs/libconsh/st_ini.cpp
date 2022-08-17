/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/*** CONAN: AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
/*** CONSH: MAY BE HAND-EDITED. ***/

#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/sm.h"														// 06/11/02 AM.
#include "prim/list_s.h"
#include "prim/list.h"
//#include "prim/mach.h"


#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/Akbm.h"														// 06/11/02 AM.

#include "consh/libconsh.h"												// 06/11/02 AM.
#include "consh/Aconsh.h"													// 06/11/02 AM.
#include "consh/cg.h"

#include "st_ini.h"


/* 9/30/95 AM. Redoing the handshake between the st management
library and the automatically generated code.
The library should know NOTHING direct about the generated code.
But the generated code shall have access to the library.
cc_st_ini() will be called directly by the application (eg Consh),
to initialize st management.
 */

// RENAME to not conflict with gend code.	// 02/26/19 AM.
#ifdef LINUX
bool ccx_st_ini(void *xcg)			// 02/26/19 AM.
#else
bool cc_st_ini(void *xcg)			// 06/11/02 AM.
#endif
{
_TCHAR **segs;			/* Global seg table for st mgr.		*/
CG *cg = (CG *) xcg;

segs = cg->ast_->st_seg_table();	/* Get the table from st mgr.		*/

// Creating an initial "hard-wired" segment.							// 06/11/02 AM.
_TCHAR *st0 = (_TCHAR *) sm_alloc(consh_ST_SEG_SIZE,					// 06/11/02 AM.
	(long) sizeof(_TCHAR));													// 06/11/02 AM.

// For now, kbm object holds the "hard-wired" initial tables.	// 06/11/02 AM.
AKBM *akbm = cg->kbm_;														// 06/11/02 AM.
akbm->setSt(st0);																// 06/11/02 AM.

/* Fill the hard-coded st segments.			*/
//segs[0] = &(St0[0]);														// 06/11/02 AM.
segs[0] = st0;																	// 06/11/02 AM.

/* Provide hard-coded values to the st mgr.	*/
bool ok = cg->ast_->st_hard_ini(											// 05/05/01 AM.
   (long) SEG_SIZE,		/* Size of a single st segment.		*/
   (int) 1,				/* Total number of hard segments.	*/
   (int) 0,				/* Last hard st segment.			*/
//   &(St0[0]) - 1	// Last full loc in last hard seg.			// 06/11/02 AM.
	st0 - 1				// Last full loc in last hard seg.			// 06/11/02 AM.
   );
return ok;																		// 05/05/01 AM.
}
