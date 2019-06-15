/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DYN.H
* FILE:	c:\lite\dyn.h
* CR:		01/29/99 AM.
* SUBJ:	Explicit loading of dynamic link libraries (DLL).
* NOTE:	To enable VisualText (GUI) to load new analyzers at will, each
*			with its own user.dll for extending the Lite Library functionality.
*
*******************************************************************************/

#ifndef DYN_H_
#define DYN_H_

//#include "lite/delt.h"

class Parse;
class Iarg;
class Auser;		// 02/13/01 AM.
class NLP;			// 01/23/02 AM.

//extern void load_dll();
#ifndef LINUX
HINSTANCE load_dll(_TCHAR *path);

bool call_ucodeAction(
	HINSTANCE hLibrary,
	_TCHAR *func,
	Delt<Iarg> *args,
	Auser *auser
//	Parse *parse
	);
bool call_runAnalyzer(														// 05/15/00 AM.
	HINSTANCE hLibrary,
	Parse *parse
	);
bool call_ucodeAlgo(															// 02/27/01 AM.
	HINSTANCE hLibrary,
	Parse *parse,
	_TCHAR *name
	);
bool call_ucodeIni(															// 01/23/02 AM.
	HINSTANCE hLibrary,
	NLP *nlp
	);
bool call_ucodeFin(															// 01/23/02 AM.
	HINSTANCE hLibrary,
	NLP *nlp
	);

void unload_dll(HINSTANCE hLibrary);
#endif

#endif
