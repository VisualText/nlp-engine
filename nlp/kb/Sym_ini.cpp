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

bool cc_sym_ini(void *xcg)
{
CG *cg = (CG *) xcg;
SYM **segs;

segs = cg->asym_->sym_seg_table();
segs[0] = &(Sym0[0]);
segs[1] = &(Sym1[0]);
segs[2] = &(Sym2[0]);
segs[3] = &(Sym3[0]);
segs[4] = &(Sym4[0]);
segs[5] = &(Sym5[0]);
segs[6] = &(Sym6[0]);
segs[7] = &(Sym7[0]);
segs[8] = &(Sym8[0]);
segs[9] = &(Sym9[0]);
segs[10] = &(Sym10[0]);
segs[11] = &(Sym11[0]);
segs[12] = &(Sym12[0]);
segs[13] = &(Sym13[0]);
segs[14] = &(Sym14[0]);
segs[15] = &(Sym15[0]);
segs[16] = &(Sym16[0]);
segs[17] = &(Sym17[0]);
segs[18] = &(Sym18[0]);
segs[19] = &(Sym19[0]);
segs[20] = &(Sym20[0]);
segs[21] = &(Sym21[0]);
segs[22] = &(Sym22[0]);
segs[23] = &(Sym23[0]);
segs[24] = &(Sym24[0]);
segs[25] = &(Sym25[0]);
segs[26] = &(Sym26[0]);
segs[27] = &(Sym27[0]);
segs[28] = &(Sym28[0]);
segs[29] = &(Sym29[0]);
segs[30] = &(Sym30[0]);
segs[31] = &(Sym31[0]);
segs[32] = &(Sym32[0]);
segs[33] = &(Sym33[0]);
segs[34] = &(Sym34[0]);
segs[35] = &(Sym35[0]);
segs[36] = &(Sym36[0]);
segs[37] = &(Sym37[0]);
segs[38] = &(Sym38[0]);
segs[39] = &(Sym39[0]);
segs[40] = &(Sym40[0]);
segs[41] = &(Sym41[0]);
segs[42] = &(Sym42[0]);
segs[43] = &(Sym43[0]);
segs[44] = &(Sym44[0]);
segs[45] = &(Sym45[0]);
segs[46] = &(Sym46[0]);
segs[47] = &(Sym47[0]);
segs[48] = &(Sym48[0]);
bool ok = cg->asym_->sym_hard_ini(
   (long) 5397,	// sym seg size
   (long) 250007,	// sym hash size
   (int) 47,	// sym hash tot
   (int) 49,	// hard segs tot
   (int) 49,	// sym segs tot
   (int) 46,	// sym seg curr
   &(Sym46[1745])	// sym seg p
   );
return ok;
}
