/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#ifndef LITE_H_
#define LITE_H_

#ifdef LINUX
#define LITE_API
#else

#ifdef LITE_EXPORTS
#define LITE_API __declspec(dllexport)
#elif LITE_STATIC
// For static compile. // 09/26/01 AM.
#define LITE_API 
#else
#define LITE_API __declspec(dllimport)
#endif

#endif

#ifdef EXAMPLES_
// This class is exported from the lite.dll
class LITE_API CLite {
public:
	CLite(void);
	// TODO: add your methods here.
};

extern LITE_API int nLite;

LITE_API int fnLite(void);
#endif

#endif
