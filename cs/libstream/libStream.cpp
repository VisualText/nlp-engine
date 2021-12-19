// libStream.cpp : Defines the entry point for the DLL application.
//

#ifndef LINUX
#include "stdafx.h"
#endif
#include "libStream.h"

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

// This is an example of an exported variable
LIBSTREAM_API int nLibStream;

// This is an example of an exported function.
LIBSTREAM_API int fnLibStream(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see libStream.h for the class definition
CLibStream::CLibStream()
{ 
	return; 
}

