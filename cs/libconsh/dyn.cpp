/*******************************************************************************
Copyright (c) 2000-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DYN.CPP
* FILE:	libconsh/dyn.cpp
* CR:		06/29/00 AM.
* SUBJ:	Module dealing with dynamic load of kb.dll library.
* NOTE:	THERE ARE TWO kb_setup FUNCTIONS.  ONE IS HARDWIRED, AND ONE IS IN
*	the compiled KB library.
*
*******************************************************************************/

#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <ctype.h>
#include <string.h>


#include "prim/libprim.h"
#include "cg_global.h"
#include "prim/dyn.h"
#include "dyn.h"
#ifdef LINUX
#include <dlfcn.h>
#endif

bool call_kb_setup(
	HINSTANCE hLibrary,
	void *cg	// 08/16/02 AM.
	)
{
typedef bool (* lpFunc4)(void *cg);
lpFunc4 Func4;

#ifndef LINUX
Func4 = (lpFunc4) GetProcAddress(hLibrary, "kb_setup");
#else
Func4 = (lpFunc4) dlsym(hLibrary, "kb_setup");
#endif

if (Func4 != NULL)
   return ((Func4)(cg));
else
	{
	std::_t_cerr << _T("[call_kb_setup: Error. No entry point.]") << std::endl;	// 05/07/01 AM.
	*cgerr << _T("Error in call_kb_setup") << std::endl;
	}
return false;
}
