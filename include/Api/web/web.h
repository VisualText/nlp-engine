/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WEB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WEB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef WEB_H_
#define WEB_H_

#ifdef LINUX
#define LITE_API
#else

#ifdef WEB_EXPORTS
#define WEB_API __declspec(dllexport)
#else
#define WEB_API __declspec(dllimport)
#endif

#endif

// This class is exported from the web.dll
class WEB_API CWeb {
public:
	CWeb(void);
	// TODO: add your methods here.
};

extern WEB_API int nWeb;

WEB_API int fnWeb(void);

#endif
