/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// lite.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "lite/lite.h"
#include <stdlib.h>
//#include <iostream.h>														// 02/07/01 AM.

#ifndef LINUX
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			// Initializations here.	// 11/30/98 AM.
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			
			// Cleanups here.				// 11/30/98 AM.
			break;
    }
    return TRUE;
}
#endif

#ifdef EXAMPLES_
// This is an example of an exported variable
LITE_API int nLite=0;

// This is an example of an exported function.
LITE_API int fnLite(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see lite.h for the class definition
CLite::CLite()
{ 
	return; 
}
#endif
