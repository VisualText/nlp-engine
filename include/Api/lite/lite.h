/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#ifndef LITE_H_
#define LITE_H_

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

#ifdef LITE_EXPORTS
#define LITE_API DECL_EXPORT
#else
#define LITE_API DECL_IMPORT
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
