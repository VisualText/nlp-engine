/*** DUMMYDATE AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;
#include "prim/libprim.h"
#include "prim/prim.h"
#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "St_ini.h"

bool cc_st_ini(void *xcg)
{
_TCHAR **segs;

CG *cg = (CG *) xcg;
segs = cg->ast_->st_seg_table();
segs[0] = &(St0[0]);
bool ok = cg->ast_->st_hard_ini(
   (long) 64768,	/* st seg size */
   (int) 1,	/* st segs tot */
   (int) 0,	/* st seg curr */
   &(St0[2613])	/* st seg p */
   );
return ok;
}
