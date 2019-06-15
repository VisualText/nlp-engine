/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#ifndef LIBKBM_H_
#define LIBKBM_H_

#ifdef LINUX
#define LIBKBM_API
#else

#ifdef LIBKBM_EXPORTS
#define LIBKBM_API __declspec(dllexport)
#else
#define LIBKBM_API __declspec(dllimport)
#endif

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
