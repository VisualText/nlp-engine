// kb.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "kb.h"

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
KB_API int nKb=0;

// This is an example of an exported function.
KB_API int fnKb(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see kb.h for the class definition
CKb::CKb()
{ 
	return; 
}

