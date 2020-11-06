
// ENTRY POINT TO DYNAMICALLY LOADED KB.DLL												// 06/29/00 AM.
#include "stdafx.h"
#include "Cc_code.h"
#include "kb_setup.h"

bool kb_setup(void *cg)	// 08/16/02 AM.
{
cc_ini(cg);	// 08/16/02 AM.
return true;
}
