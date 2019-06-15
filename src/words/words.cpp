// words.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "words/words.h"

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
WORDS_API int nWords=0;

// This is an example of an exported function.
WORDS_API int fnWords(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see words.h for the class definition
CWords::CWords()
{ 
	return; 
}

