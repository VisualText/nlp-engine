
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the USR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// USR_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifndef _USER_H_
#define _USER_H_

#ifdef LINUX
#define USER_API
#else

#ifdef USER_EXPORTS
#define USER_API __declspec(dllexport)
#else
#define USER_API __declspec(dllimport)
#endif

#endif

#include "lite/delt.h"

#ifdef UNICODE
//#define gerrStr(a,b,c) gerrStr(a,c)
//#define gerrStr1(a,b,c) gerrStr1(a,c)
//#define gerrStr2(a,b,c) gerrStr2(a,c)
//#define gerrStr3(a,b,c) gerrStr3(a,c)
#define outStr(a,b,c) outStr(a,c)
#define ocbuf(a,b,c) ocbuf(a,c)
#endif

// Forward references.
class Iarg;
class Parse;
class Auser;			// 02/13/01 AM.
class NLP;				// 01/23/02 AM.

// This class is exported from the usr.dll
#ifdef SAMPLES_
class USER_API CUsr {
public:
	CUsr(void);
	// TODO: add your methods here.
};

extern USER_API int nUsr;
#endif

//USR_API int fnUsr(void);
extern "C" int  fnUsr(void);   //By default this is a __cdecl convetion

extern "C" void regUser();

extern "C" bool ucodeAction(_TCHAR *func, DELTS *args, Auser *auser);

// 02/27/01 AM.
extern "C" bool ucodeAlgo(Parse *parse, _TCHAR *name);

// Initialize USER project during analyzer load.					// 01/23/02 AM.
extern "C" bool ucodeIni(NLP *nlp);										// 01/23/02 AM.

// Cleanups for USER project during analyzer close.				// 01/23/02 AM.
extern "C" bool ucodeFin(NLP *nlp);										// 01/23/02 AM.

#endif
