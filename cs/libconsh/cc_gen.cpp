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
*									CC_GEN.C
*
* FILE:	libconsh/cc_gen.c
* SUBJ:	Generate code to oversee the automatically generated stuff.
* CR:	10/1/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "prim/prim.h"
#include "gen.h"
#include "cc_gen.h"



/**************************************************
*						CC_GEN_HDR
* FUN:	cc_gen_hdr
* SUBJ:	Generate header code for kb mgr systems.
* CR:	10/1/95 AM.
*
**************************************************/

void cc_gen_hdr(
	_TCHAR *dir,		/* Output directory for gend code.		*/
   _TCHAR *tail		/* Tail for naming generated files.		*/
   )
{
std::_t_ofstream *fp;
_TCHAR s_nam[PATH];	/* Name of file.		*/

_stprintf(s_nam, _T("%s%s%s_code.h%s"), dir, DIR_SEP, consh_CC_BASE, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));		// 04/20/99 AM.

gen_file_head(fp);
*fp << _T("extern bool cc_ini(void*);") << std::endl;							// 08/16/02 AM.

//if (!file_close(s_nam, fp))
//   return;
delete fp;							// 04/20/99 AM.
}


/**************************************************
*						CC_GEN_INI
* FUN:	cc_gen_ini
* SUBJ:	Generate code to initialize kb mgr systems.
* CR:	10/1/95 AM.
*
**************************************************/

void cc_gen_ini(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail		/* Tail for naming generated files.		*/
	)
{
std::_t_ofstream *fp;
_TCHAR s_nam[PATH];	/* Name of file.		*/

_stprintf(s_nam, _T("%s%s%s_code.cpp%s"), dir, DIR_SEP, consh_CC_BASE, tail);
//if (!file_open(s_nam, "w", &fp))
//   return;
fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_file_head(fp);
// NLP-ENGINE-510: emit StdAfx.h (mixed case) on every platform. Linux is
// case-sensitive, and the compile-service (cloud + local Linux/Mac scripts)
// drops a StdAfx.h next to the generated sources; emitting a lowercase
// `#include "stdafx.h"` for LINUX broke the cloud build with
// `fatal error: stdafx.h: No such file or directory`.
*fp << _T("#include \"StdAfx.h\"") << std::endl;
*fp << _T("extern bool cc_st_ini(void*);") << std::endl;						// 08/15/02 AM.
*fp << _T("extern bool cc_sym_ini(void*);") << std::endl;					// 08/15/02 AM.
*fp << _T("extern bool cc_con_ini(void*);") << std::endl;					// 08/15/02 AM.
*fp << _T("extern bool cc_ptr_ini(void*);") << std::endl;					// 08/15/02 AM.

*fp << _T("bool") << std::endl;														// 05/05/01 AM.
*fp << _T("cc_ini(void *cg)") << std::endl;										// 08/16/02 AM.
*fp << _T("{") << std::endl;
*fp << _T("if (!cc_st_ini(cg))  return false;") << std::endl;				// 08/15/02 AM.
*fp << _T("if (!cc_sym_ini(cg)) return false;") << std::endl;				// 08/15/02 AM.
// cg->asym_->sym_reset();	// Probably not for compiled.			// 08/15/02 AM.
*fp << _T("if (!cc_con_ini(cg)) return false;") << std::endl;				// 08/15/02 AM.
*fp << _T("if (!cc_ptr_ini(cg)) return false;") << std::endl;				// 08/15/02 AM.
*fp << _T("return true;") << std::endl;											// 05/05/01 AM.
*fp << _T("}") << std::endl;

//if (!file_close(s_nam, fp))
//   return;
delete fp;
}


/**************************************************
*						CC_GEN_KB_SETUP
* FUN:	cc_gen_kb_setup
* SUBJ:	Generate kb_setup.cpp + kb_setup.h in the analyzer's kb/ folder.
* NOTE:	NLP-ENGINE-495. The engine's call_kb_setup (cs/libconsh/dyn.cpp)
*			resolves "kb_setup" via GetProcAddress / dlsym from the compiled
*			KB library. Without this generated wrapper the symbol isn't
*			present and patExecute<n> for the `kb` pass crashes in compiled
*			runs because the compiled KB data structures never get
*			initialized. Wrap in extern "C" + __declspec(dllexport) on
*			Windows so the symbol is exported from the cmake-built DLL.
**************************************************/

void cc_gen_kb_setup(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail		/* Tail for naming generated files.		*/
	)
{
std::_t_ofstream *fp;
_TCHAR s_nam[PATH];

// kb_setup.h
// Define the NLP_KB_EXPORT macro here so the declaration and the
// definition in kb_setup.cpp agree on linkage. MSVC errors out with
// "redefinition; different linkage" (C2375) if the header declares
// kb_setup without __declspec(dllexport) and the .cpp defines it with
// dllexport.
_stprintf(s_nam, _T("%s%skb_setup.h%s"), dir, DIR_SEP, tail);
fp = new std::_t_ofstream(TCHAR2A(s_nam));
gen_file_head(fp);
*fp << _T("#ifdef _WIN32") << std::endl;
*fp << _T("#define NLP_KB_EXPORT __declspec(dllexport)") << std::endl;
*fp << _T("#else") << std::endl;
*fp << _T("#define NLP_KB_EXPORT") << std::endl;
*fp << _T("#endif") << std::endl;
*fp << _T("extern \"C\" NLP_KB_EXPORT bool kb_setup(void*);") << std::endl;
delete fp;

// kb_setup.cpp — definition uses the same NLP_KB_EXPORT macro from the
// header so MSVC sees identical linkage between declaration and
// definition.
_stprintf(s_nam, _T("%s%skb_setup.cpp%s"), dir, DIR_SEP, tail);
fp = new std::_t_ofstream(TCHAR2A(s_nam));
gen_file_head(fp);
// NLP-ENGINE-510: emit StdAfx.h (mixed case) on every platform.
*fp << _T("#include \"StdAfx.h\"") << std::endl;
*fp << _T("#include \"Cc_code.h\"") << std::endl;
*fp << _T("#include \"kb_setup.h\"") << std::endl;
*fp << _T("extern \"C\" NLP_KB_EXPORT bool kb_setup(void *cg)") << std::endl;
*fp << _T("{") << std::endl;
*fp << _T("if (!cc_ini(cg)) return false;") << std::endl;
*fp << _T("return true;") << std::endl;
*fp << _T("}") << std::endl;
delete fp;
}
