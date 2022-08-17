/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#include "StdAfx.h"				// 04/26/99 AM.
#include <stdio.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/sm.h"															// 06/11/02 AM.
#include "prim/list_s.h"
#include "prim/list.h"
//#include "prim/mach.h"


#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/Akbm.h"														// 06/11/02 AM.

#include "consh/libconsh.h"												// 06/11/02 AM.
#include "consh/Aconsh.h"													// 06/11/02 AM.
#include "consh/cg.h"

#include "ptr_ini.h"


/* 10/4/95 AM. Redoing the handshake between the ptr table management
library and the automatically generated code.
The library should know NOTHING direct about the generated code.
But the generated code shall have access to the library.
cc_ptr_ini() will be called directly by the application (eg Consh),
to initialize ptr table management.
 */

// LINUX: Distinguish internal kb load from auto-generated kb load // 03/23/19 AM.
#ifdef LINUX
bool ccx_ptr_ini(void *cgx)						// 03/23/19 AM.
#else
bool cc_ptr_ini(void *cgx)
#endif
{
PTR **segs;			/* Global seg table for ptr table mgr.		*/

CG *cg = (CG *)cgx;

segs = cg->aptr_->ptr_seg_table();	/* Get the table from ptr table mgr.	*/

// Creating an initial "hard-wired" segment.							// 06/11/02 AM.
PTR *ptr0 = (PTR *) sm_alloc(consh_PTR_SEG_SIZE,					// 06/11/02 AM.
	(long) sizeof(PTR));														// 06/11/02 AM.

// For now, kbm object holds the "hard-wired" initial tables.	// 06/11/02 AM.
AKBM *akbm = cg->kbm_;													// 06/11/02 AM.
akbm->setPtr(ptr0);															// 06/11/02 AM.

/* Fill the hard-coded ptr table segments.			*/
segs[0] = ptr0;																// 06/11/02 AM.
//segs[0] = &(Ptr0[0]);

/* Provide hard-coded values to the ptr table mgr.	*/
bool ok = cg->aptr_->ptr_hard_ini(										// 05/05/01 AM.
   (long) SEG_SIZE/ (long) sizeof(PTR),	/* Size of single ptr seg.*/
   (int) 1,						/* Total number of hard segments.	*/
   (int) 0,						/* Last hard ptr segment.			*/
//   &(Ptr0[0]) - 1			/* Last full loc in last hard seg.	*/
	ptr0 - 1						// Last full loc in last hard seg.	// 06/11/02 AM.
   );
return ok;																		// 05/05/01 AM.
}
