/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WORDS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WORDS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef WORDS_H_
#define WORDS_H_

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

#ifdef WORDS_EXPORTS
#define WORDS_API DECL_EXPORT
#else
#define WORDS_API DECL_IMPORT
#endif

// This class is exported from the words.dll
class WORDS_API CWords {
public:
	CWords(void);
	// TODO: add your methods here.
};

extern WORDS_API int nWords;

WORDS_API int fnWords(void);

#endif
