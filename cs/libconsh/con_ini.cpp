/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/*** CONAN: AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
/*** CONSH: HAND-EDITING OK ***/

#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"															// 06/11/02 AM.
#include "prim/list_s.h"
#include "prim/list.h"

#include "kbm/libkbm.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/Akbm.h"														// 06/11/02 AM.

#include "consh/libconsh.h"												// 06/11/02 AM.
#include "consh/Aconsh.h"													// 06/11/02 AM.
#include "consh/cg.h"														// 08/15/02 AM.

#include "con_ini.h"


/* 10/4/95 AM. Redoing the handshake between the con table management
library and the automatically generated code.
The library should know NOTHING direct about the generated code.
But the generated code shall have access to the library.
cc_con_ini() will be called directly by the application (eg Consh),
to initialize con table management.
 */

// NOTE: Renaming hardwired kb build functions to differ from auto-generated fns.
#ifdef LINUX
bool ccx_con_ini(void *xcg)	// 02/20/19 AM.
#else
bool cc_con_ini(void *xcg)	// 08/15/02 AM.
#endif
{
CON **segs;			/* Global seg table for con table mgr.		*/
CG *cg = (CG *) xcg;															// 08/15/02 AM.

segs = cg->acon_->con_seg_table();										// 08/15/02 AM.
					/* Get the table from con table mgr.	*/

// Creating an initial "hard-wired" segment.							// 06/11/02 AM.
CON *con0 = (CON *) sm_alloc(												// 06/11/02 AM.
	SEG_SIZE / sizeof(CON),													// 06/11/02 AM.
	(long) sizeof(CON));														// 06/11/02 AM.

// For now, kbm object holds the "hard-wired" initial tables.	// 06/11/02 AM.
AKBM *akbm = cg->kbm_;														// 08/15/02 AM.
akbm->setCon(con0);															// 06/11/02 AM.


/* Fill the hard-coded con table segments.			*/
//segs[0] = &(Con0[0]);														// 06/11/02 AM.
segs[0] = con0;																// 06/11/02 AM.

/* Provide hard-coded values to the con table mgr.	*/
bool ok = cg->acon_->con_hard_ini(										// 08/15/02 AM.
   (long) SEG_SIZE/ (long) sizeof(CON),	/* Size of single con seg.*/
   (int) 1,				/* Total number of hard segments.	*/
   (int) 0,				/* Last hard con segment.			*/
//  &(Con0[0]) - 1,	// Last full loc in last hard seg.			// 06/11/02 AM.
	con0 - 1,			// Last full loc in last hard seg.			// 06/11/02 AM.
   (ID) 0				/* Concept count so far.			*/
   );

#ifdef LINUX
// Pretty-print kb concepts while debugging LINUX KB load.	// 02/21/19 AM.
cg->acon_->con_tab_pretty(&std::cout);	// 02/20/19 AM.
#endif

return ok;																		// 05/05/01 AM.
}
