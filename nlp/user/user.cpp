// usr.cpp : Defines the entry point for the DLL application.
//
#ifndef LINUX
#include <windows.h>	
#endif

#include "machine.h"

#include "lite/lite.h"														// 02/13/01 AM.
#include "lite/node.h"														// 02/13/01 AM.
#include "lite/Auser.h"														// 02/13/01 AM.
#include "user.h"
#include "Ureg.h"
#include "Ucode.h"

// This is an example of an exported variable
//USER_API int nUsr=72;

// This is an example of an exported function.
//USR_API int fnUsr(void)
int fnUsr(void)
{
//cout << "in usr1 dll: nLt=" << nLt << endl;
	return 42;
}

// This is the constructor of a class that has been exported.
// see usr.h for the class definition
/*
CUsr::CUsr()
{ 
	return; 
}
*/


// Register the algorithms and actions with the Lite Library.
// (Obsolete.)																	// 01/23/02 AM.
void regUser()
{
Ureg();
}

// Note:	The Lite Library will take care of UN-registering this stuff
// when it deems appropriate.

// Function called by VisualText analyzer engine to execute
// user-defined functions in the USER project.						// 01/23/02 AM.
bool ucodeAction(_TCHAR *func, DELTS *args, Auser *auser)	// 02/13/01 AM.
{
return user::execute(func, args, auser);
}

// Obsolete handshake for user-defined algorithms.					// 01/23/02 AM.
// Build a user-defined NLP++ function in Ucode.cpp instead.	// 01/23/02 AM.
bool ucodeAlgo(Parse *parse, _TCHAR *name)								// 02/27/01 AM.
{
return true;
}
