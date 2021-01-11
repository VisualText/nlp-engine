/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBCONSH_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBCONSH_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef LIBCONSH_H_
#define LIBCONSH_H_

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

#ifdef LIBCONSH_EXPORTS
#define LIBCONSH_API DECL_EXPORT
#else
#define LIBCONSH_API DECL_IMPORT
#endif

#ifdef JUNK_
// This class is exported from the libconsh.dll
class LIBCONSH_API CLibconsh {
public:
	CLibconsh(void);
	// TODO: add your methods here.
};

extern LIBCONSH_API int nLibconsh;

LIBCONSH_API int fnLibconsh(void);
#endif


#endif
