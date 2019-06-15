/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// libconsh.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "consh/libconsh.h"

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


#ifdef JUNK_

// This is an example of an exported variable
LIBCONSH_API int nLibconsh=0;

// This is an example of an exported function.
LIBCONSH_API int fnLibconsh(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see libconsh.h for the class definition
CLibconsh::CLibconsh()
{ 
	return; 
}

#endif


