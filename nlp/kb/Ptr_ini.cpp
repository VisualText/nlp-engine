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

bool cc_ptr_ini(void *xcg)
{
PTR **segs;

CG *cg = (CG *) xcg;
segs = cg->aptr_->ptr_seg_table();
segs[0] = &(Ptr0[0]);
bool ok = cg->aptr_->ptr_hard_ini(
   (long) 5397,	/* ptr seg size */
   (int) 1,	/* ptr segs tot */   (int) 0,	/* ptr seg curr */
   &(Ptr0[209])	/* ptr seg p */
   );
return ok;
}
