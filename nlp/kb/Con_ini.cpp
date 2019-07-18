/*** DUMMYDATE AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;
#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/str.h"
#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "St_ini.h"
#include "Sym_ini.h"
#include "Con_ini.h"
#include "Ptr_ini.h"
#include "Cc_code.h"

bool cc_con_ini(void *xcg)
{
CON **segs;

CG *cg = (CG *) xcg;
segs = cg->acon_->con_seg_table();
segs[0] = &(Con0[0]);
bool ok = cg->acon_->con_hard_ini(
   (long) 1799,	/* con seg size */
   (int) 1,	/* con segs tot */
   (int) 0,	/* con seg curr */
   &(Con0[832]),	/* con seg p */
   (ID) 833	/* concept count */
   );
return ok;
}
