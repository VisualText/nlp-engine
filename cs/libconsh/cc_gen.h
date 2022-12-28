/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CC_GEN.H
*
* FILE:	consh.�/cc_gen.h
* SUBJ:	Declarations for overall code generation.
* CR:	10/1/95 AM.
*
*******************************************************************************/

#include <filesystem>

#define consh_CC_BASE _T("Cc")

extern void cc_gen_hdr(
	std::filesystem::path dir,		/* Output directory for gend code.		*/
   _TCHAR *tail		/* Tail for naming generated files.		*/
   );
extern void cc_gen_ini(
	std::filesystem::path dir,		/* Output directory for gend code.		*/
	_TCHAR *tail		/* Tail for naming generated files.		*/
	);
