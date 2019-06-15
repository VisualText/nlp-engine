/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBPRIM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBPRIM_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef LIBPRIM_H_
#define LIBPRIM_H_

#ifdef LINUX
#define LIBPRIM_API
#else

#ifdef LIBPRIM_EXPORTS
#define LIBPRIM_API __declspec(dllexport)
#else
#define LIBPRIM_API __declspec(dllimport)
#endif

#endif

// This class is exported from the libprim.dll
class LIBPRIM_API CLibprim {
public:
	CLibprim(void);
	// TODO: add your methods here.
};

extern LIBPRIM_API int nLibprim;

LIBPRIM_API int fnLibprim(void);

#endif
#include "my_tchar.h"
#include "streamClasses.h"
