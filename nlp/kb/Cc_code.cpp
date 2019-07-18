/*** DUMMYDATE AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
#include "StdAfx.h"
extern bool cc_st_ini(void*);
extern bool cc_sym_ini(void*);
extern bool cc_con_ini(void*);
extern bool cc_ptr_ini(void*);
bool
cc_ini(void *cg)
{
if (!cc_st_ini(cg))  return false;
if (!cc_sym_ini(cg)) return false;
if (!cc_con_ini(cg)) return false;
if (!cc_ptr_ini(cg)) return false;
return true;
}
