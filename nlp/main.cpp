/*******************************************************************************
Copyright © 1999-2006 Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME: MAIN.CPP
* FILE: main.cpp
* CR:   01/29/99 AM.
* SUBJ: Sample driver program that uses the Analyzer and KB API functions.
*       Loading an analyzer invokes the user extensions library (user/user.dll).
* ENV:  APPS - a default environment variable for VisualText applications.
*
*******************************************************************************/

#include "lite/nlp_engine.h"
#include "version.h"

#include "lite/unitest.h"
#include "unicode/unistr.h"

using icu::UnicodeString;

bool cmdReadArgs(int, _TCHAR *argv[], _TCHAR *&, _TCHAR *&, _TCHAR *&, _TCHAR *&, bool &, bool &, bool &);
void cmdHelpargs(_TCHAR *);

#ifdef LINUX
int main(
#else
int _tmain(
#endif
	int argc,
	_TCHAR *argv[],
	_TCHAR *envp[])
{
	moose();
}
