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

#ifdef LINUX
// On Linux, HINSTANCE holds the dlopen handle. void* matches sizeof(double)
// on x86_64 so this is ABI-safe vs. the previous typedef.
typedef void *HINSTANCE;
#endif

LIBPRIM_API HINSTANCE load_dll(_TCHAR *path);

LIBPRIM_API void unload_dll(HINSTANCE hLibrary);

#endif
