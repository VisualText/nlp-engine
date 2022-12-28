/*******************************************************************************
Copyright � 2000-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DYN.H
* FILE:	include/API/prim/dyn.h
* CR:		06/29/00 AM.
* SUBJ:	Explicit loading of dynamic link libraries (DLL).
* NOTE:	Copied from lite/dyn.h.
*
*******************************************************************************/

#ifndef DYN_H_
#define DYN_H_

#include <filesystem>

#ifndef LINUX
LIBPRIM_API HINSTANCE load_dll(std::filesystem::path p);

LIBPRIM_API void unload_dll(HINSTANCE hLibrary);
#endif

#endif
