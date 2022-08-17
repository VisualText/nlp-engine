/*******************************************************************************
Copyright (c) 2000-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DYN.CPP
* FILE:	libprim/dyn.cpp
* CR:		06/29/00 AM.
* SUBJ:	Explicit loading of dynamic link libraries (DLL).
* NOTE:	Copied and modified lite/dyn.cpp.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "prim/dyn.h"
typedef int ( * lpFunc1)();
typedef void (* lpFunc2)();

#ifndef LINUX

LIBPRIM_API HINSTANCE load_dll(_TCHAR *path)
{
    HINSTANCE  hLibrary;

        int a = 0;
    hLibrary = LoadLibrary(path);
    
    if (hLibrary != NULL)
    {
    }
//    else cerr << "Error in Load Library" << std::endl;				// 08/20/00 AM.
	else
		std::_t_cerr << _T("[Couldn't load library: ") << path					// 08/20/00 AM.
			  << std::endl;
return hLibrary;
}


LIBPRIM_API void unload_dll(HINSTANCE hLibrary)
{
FreeLibrary(hLibrary);         // Unload DLL from memory
}

#endif
