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
#ifdef LINUX
*fp << _T("#include \"stdafx.h\"") << std::endl;								// 05/05/01 AM.
#else
*fp << _T("#include \"StdAfx.h\"") << std::endl;								// 05/05/01 AM.
#endif
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
