/*******************************************************************************
Copyright (c) 2007-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBQKBM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBQKBM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBQKBM_EXPORTS
#define LIBQKBM_API __declspec(dllexport)
#else
#define LIBQKBM_API __declspec(dllimport)
#endif

// This class is exported from the libqkbm.dll
//class LIBQKBM_API Clibqkbm {
//public:
//	Clibqkbm(void);
	// TODO: add your methods here.
//};

//extern LIBQKBM_API int nlibqkbm;

//LIBQKBM_API int fnlibqkbm(void);
