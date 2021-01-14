/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#ifndef LIBKBM_H_
#define LIBKBM_H_

#ifdef SHARED_LIB
 #ifdef _MSC_VER
  #define DECL_EXPORT __declspec(dllexport)
  #define DECL_IMPORT __declspec(dllimport)
 #elif __GNUC__ >= 4
  #define DECL_EXPORT __attribute__ ((visibility ("default")))
  #define DECL_IMPORT __attribute__ ((visibility ("default")))
 #endif
//  add MacOS
#else
 #define DECL_EXPORT
 #define DECL_IMPORT
#endif

#ifdef LIBKBM_EXPORTS
#define LIBKBM_API DECL_EXPORT
#else
#define LIBKBM_API DECL_IMPORT
#endif

#include "my_tchar.h"

// This class is exported from the libkbm.dll
class LIBKBM_API CLibkbm {
public:
	CLibkbm(void);
	// TODO: add your methods here.
};

extern LIBKBM_API int nLibkbm;

LIBKBM_API int fnLibkbm(void);

#endif
