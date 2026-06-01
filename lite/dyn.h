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

#ifndef LITE_DYN_H_
#define LITE_DYN_H_

//#include "lite/delt.h"

class Parse;
class Iarg;
class Auser;		// 02/13/01 AM.
class NLP;			// 01/23/02 AM.

//extern void load_dll();
// NLP-ENGINE-516: call_runAnalyzer is cross-platform — uses GetProcAddress
// on Windows and dlsym on Linux/macOS. load_dll/unload_dll prototypes come
// from include/Api/prim/dyn.h (also cross-platform).
bool call_runAnalyzer(
	HINSTANCE hLibrary,
	Parse *parse
	);

#ifndef LINUX
// Legacy lite-side prototypes for the Windows user.dll path. load_dll /
// unload_dll are duplicated here from prim/dyn.h; the user-extension
// helpers below are Windows-only.
HINSTANCE load_dll(_TCHAR *path);

bool call_ucodeAction(
	HINSTANCE hLibrary,
	_TCHAR *func,
	Delt<Iarg> *args,
	Auser *auser
//	Parse *parse
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
