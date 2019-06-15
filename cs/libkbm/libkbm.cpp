/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// libkbm.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "kbm/libkbm.h"

#ifndef LINUX
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif

// This is an example of an exported variable
LIBKBM_API int nLibkbm=0;

// This is an example of an exported function.
LIBKBM_API int fnLibkbm(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see libkbm.h for the class definition
CLibkbm::CLibkbm()
{ 
	return; 
}

