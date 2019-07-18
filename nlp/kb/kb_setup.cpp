
// ENTRY POINT TO DYNAMICALLY LOADED KB.DLL												// 06/29/00 AM.
#include "StdAfx.h"
#include "Cc_code.h"
#include "kb_setup.h"

bool kb_setup(void *cg)	// 08/16/02 AM.
{
cc_ini(cg);	// 08/16/02 AM.
return true;
}

// KB_STATIC_SETUP. For kbs statically compiled with analyzer.	// 02/20/19 AM.
// PART OF LINUX PORT.
bool kb_static_setup(void *cg)	// 02/20/19 AM.
{
return kb_setup(cg);
}
