// run.cpp : Defines the entry point for the DLL application.
//

//#include "stdafx.h"
#include "run.h"

#ifdef JUNKY_
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

#ifdef SAMPLES_
// This is an example of an exported variable
RUN_API int nRun=0;

// This is an example of an exported function.
RUN_API int fnRun(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see run.h for the class definition
CRun::CRun()
{ 
	return; 
}
#endif

void testy()
{
int a = 3+3;
}