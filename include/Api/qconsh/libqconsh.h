/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBQCONSH_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBQCONSH_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBQCONSH_EXPORTS
#define LIBQCONSH_API __declspec(dllexport)
#else
#define LIBQCONSH_API __declspec(dllimport)
#endif

// This class is exported from the libqconsh.dll
//class LIBQCONSH_API Clibqconsh {
//public:
//	Clibqconsh(void);
	// TODO: add your methods here.
//};

//extern LIBQCONSH_API int nlibqconsh;

//LIBQCONSH_API int fnlibqconsh(void);
