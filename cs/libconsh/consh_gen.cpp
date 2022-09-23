/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CONSH_GEN.C
*
* FILE:	conan.�/consh_gen.c
* SUBJ:	Module for generating code specifically for Consh.
* CR:	10/7/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"
//#include "prim/mach.h"
#include "prim/str.h"


#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"

#include "consh/libconsh.h"	// 04/26/99 AM.
#include "consh/Aconsh.h"		// 04/26/99 AM.
#include "consh/cg.h"

#include "gen.h"
#include "cc_gen.h"
#include "st_gen.h"
#include "sym_gen.h"
#include "con_gen.h"
#include "ptr_gen.h"
#include "consh_gen.h"


/**************************************************
*						CONSH_GEN_INCLUDES
* FUN:	consh_gen_includes
* SUBJ:	Generate includes for gend code files.
* CR:	10/5/95 AM.
*
**************************************************/

void
consh_gen_includes(
	std::_t_ofstream *fp
	)
{
#ifdef LINUX
*fp << _T("#include \"stdafx.h\"") << std::endl;														// 06/28/00 AM.
*fp << _T("#include <stdio.h>") << std::endl;
*fp << _T("#include <stdlib.h>") << std::endl;
//*fp << "#include <fstream.h>" << std::endl;								// 04/23/99 AM.
*fp << _T("#include <iostream>") << std::endl;					// Upgrade.	// 01/24/01 AM.
*fp << _T("#include <fstream>") << std::endl;					// Upgrade. // 01/24/01 AM.
*fp << _T("#include \"prim\\libprim.h\"") << std::endl;
//*fp << "#include \"prim\\mach.h\"" << std::endl;	// 04/23/99 AM.
*fp << _T("#include \"prim\\prim.h\"") << std::endl;
*fp << _T("#include \"prim\\str.h\"") << std::endl;
//*fp << "#include \"cc_var.h\"" << std::endl;
*fp << _T("#include \"kbm\\libkbm.h\"") << std::endl;
*fp << _T("#include \"kbm\\st.h\"") << std::endl;
*fp << _T("#include \"kbm\\sym_s.h\"") << std::endl;
*fp << _T("#include \"kbm\\sym.h\"") << std::endl;
*fp << _T("#include \"kbm\\con_s.h\"") << std::endl;
*fp << _T("#include \"kbm\\con_.h\"") << std::endl;
*fp << _T("#include \"kbm\\ptr_s.h\"") << std::endl;
*fp << _T("#include \"kbm\\ptr.h\"") << std::endl;
*fp << _T("#include \"consh/libconsh.h\"") << std::endl;						// 08/16/02 AM.
*fp << _T("#include \"consh/cg.h\"") << std::endl;								// 08/16/02 AM.
*fp << _T("#include \"") << consh_ST_BASE  << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_SYM_BASE << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_CON_BASE << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_PTR_BASE << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_CC_BASE  << _T("_code.h\"") << std::endl;

#else
*fp << _T("#include \"StdAfx.h\"") << std::endl;														// 06/28/00 AM.
*fp << _T("#include <stdio.h>") << std::endl;
*fp << _T("#include <stdlib.h>") << std::endl;
//*fp << "#include <fstream.h>" << std::endl;								// 04/23/99 AM.
*fp << _T("#include <iostream>") << std::endl;					// Upgrade.	// 01/24/01 AM.
*fp << _T("#include <fstream>") << std::endl;					// Upgrade. // 01/24/01 AM.
*fp << _T("#include \"prim/libprim.h\"") << std::endl;
//*fp << "#include \"prim/mach.h\"" << std::endl;	// 04/23/99 AM.
*fp << _T("#include \"prim/prim.h\"") << std::endl;
*fp << _T("#include \"prim/str.h\"") << std::endl;
//*fp << "#include \"cc_var.h\"" << std::endl;
*fp << _T("#include \"kbm/libkbm.h\"") << std::endl;
*fp << _T("#include \"kbm/st.h\"") << std::endl;
*fp << _T("#include \"kbm/sym_s.h\"") << std::endl;
*fp << _T("#include \"kbm/sym.h\"") << std::endl;
*fp << _T("#include \"kbm/con_s.h\"") << std::endl;
*fp << _T("#include \"kbm/con_.h\"") << std::endl;
*fp << _T("#include \"kbm/ptr_s.h\"") << std::endl;
*fp << _T("#include \"kbm/ptr.h\"") << std::endl;
*fp << _T("#include \"consh/libconsh.h\"") << std::endl;						// 08/16/02 AM.
*fp << _T("#include \"consh/cg.h\"") << std::endl;								// 08/16/02 AM.
*fp << _T("#include \"") << consh_ST_BASE  << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_SYM_BASE << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_CON_BASE << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_PTR_BASE << _T("_ini.h\"") << std::endl;
*fp << _T("#include \"") << consh_CC_BASE  << _T("_code.h\"") << std::endl;
#endif
}

