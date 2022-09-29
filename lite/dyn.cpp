/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DYN.CPP
* FILE:	c:\lite\dyn.cpp
* CR:		01/29/99 AM.
* SUBJ:	Explicit loading of dynamic link libraries (DLL).
* NOTE:	To enable VisualText (GUI) to load new analyzers at will, each
*			with its own user.dll for extending the Lite Library functionality.
*
*******************************************************************************/

#include "StdAfx.h"
//#include <iostream> // 07/18/03 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/03/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
//#include "node.h"	// 07/07/03 AM.
//#include "lite/nlppp.h"
#include "lite/Auser.h"			// 02/13/01 AM.
#include "dyn.h"
typedef int ( * lpFunc1)();
typedef void (* lpFunc2)();	// 01/29/99 AM.
///void load_dll(char *path);

#ifndef LINUX


//HINSTANCE load_dll()
HINSTANCE load_dll(_TCHAR *path)
{
    HINSTANCE  hLibrary;
    //lpFunc1 Func1;						// 05/15/00 AM.
	 //lpFunc2 Func2;

	int a = 0;
    
	 // Load the DLL.
    //hLibrary = LoadLibrary("d:\\apps\\App\\user\\debug\\user.dll");
    hLibrary = LoadLibrary(path);
    
    if (hLibrary != NULL)
    {
			// Commenting out.	// 05/15/00 AM.
	//Func1 = (lpFunc1) GetProcAddress(hLibrary, "fnUsr");
	//if (Func1 != NULL)
	//    a = ((Func1)());
	//else *gerr << "Error in Func1 call" << std::endl;


		// REGISTER USER ALGORITHMS AND ACTIONS!	// 01/29/99 AM.
		 // Func2 = (lpFunc2) GetProcAddress(hLibrary, "regUser");
       // if (Func2 != NULL)
       //     ((Func2)());
       // else *gerr << "Error in Func2 call" << std::endl;

    }
//    else *gerr << "Error in Load Library" << std::endl;			// 08/20/00 AM.
	else
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't load library:") << path << _T("]")		// 08/20/00 AM.
					<< std::ends;
		errOut(&gerrStr,false);
		}
														// 08/20/00 AM.
    //*gerr << "a=" << a << std::endl;
    //FreeLibrary(hLibrary);	 // Unload DLL from memory
return hLibrary;
}

bool call_ucodeAction(
	HINSTANCE hLibrary,
	_TCHAR *func,
	Delt<Iarg> *args,
	Auser *auser																// 02/13/01 AM.
//	Parse *parse																// 02/09/01 AM.
	)
{
//typedef bool (* lpFunc3)(char *func, Delt<Iarg> *args, Auser *auser);
typedef bool (* lpFunc3)(_TCHAR *func, DELTS *args, Auser *auser);	// 07/07/03 AM.
lpFunc3 Func3;

Func3 = (lpFunc3) GetProcAddress(hLibrary, "ucodeAction");
if (Func3 != NULL)
   return ((Func3)(func,(DELTS*)args,auser));
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Error in call_ucodeAction") << std::ends;
	errOut(&gerrStr,false);
	}

return false;
}

// 05/15/00 AM.
bool call_runAnalyzer(
	HINSTANCE hLibrary,
	Parse *parse
	)
{
typedef bool (* lpFunc4)(Parse *parse);
lpFunc4 Func4;

Func4 = (lpFunc4) GetProcAddress(hLibrary, "run_analyzer");
if (Func4 != NULL)
   return ((Func4)(parse));
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Error in call_runAnalyzer") << std::ends;
	errOut(&gerrStr,false);
	}

return false;
}

void unload_dll(HINSTANCE hLibrary)
{
FreeLibrary(hLibrary);	 // Unload DLL from memory
}


// CALL TO USER-DEFINED PASS ALGORITHM.								// 02/27/01 AM.
bool call_ucodeAlgo(
	HINSTANCE hLibrary,
	Parse *parse,			// Current parse.
	_TCHAR *name				// Current pass name.
	)
{
typedef bool (* lpFunc5)(Parse *parse, _TCHAR *name);
lpFunc5 Func5;

Func5 = (lpFunc5) GetProcAddress(hLibrary, "ucodeAlgo");
if (Func5 != NULL)
   return ((Func5)(parse,name));
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Error in call_ucodeAlgo") << std::ends;
	errOut(&gerrStr,false);
	}

return false;
}


// For initializing USER project during analyzer load.			// 01/23/02 AM.
bool call_ucodeIni(															// 01/23/02 AM.
	HINSTANCE hLibrary,
	NLP *nlp
	)
{
typedef bool (* lpFunc6)(NLP *nlp);
lpFunc6 Func6;

Func6 = (lpFunc6) GetProcAddress(hLibrary, "ucodeIni");
if (Func6 != NULL)
   return ((Func6)(nlp));
else
	{
	std::_t_strstream gerrStr;
//	gerrStr << "Error in call_ucodeIni" << std::ends;
	gerrStr << _T("[Warning: User project probably not up to date.]") << std::ends;
	errOut(&gerrStr,false,0,0);
	}

return false;
}

// For initializing USER project during analyzer close.			// 01/23/02 AM.
bool call_ucodeFin(															// 01/23/02 AM.
	HINSTANCE hLibrary,
	NLP *nlp
	)
{
typedef bool (* lpFunc6)(NLP *nlp);
lpFunc6 Func6;

Func6 = (lpFunc6) GetProcAddress(hLibrary, "ucodeFin");
if (Func6 != NULL)
   return ((Func6)(nlp));
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning: User project probably not up to date.]") << std::ends;
	errOut(&gerrStr,false,0,0);
	}

return false;
}

#endif
