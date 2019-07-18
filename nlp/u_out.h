/*******************************************************************************
Copyright (c) 2005-6 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	U_OUT.H
* FILE:	lite/u_out.h
* CR:		01/24/06 AM.
* SUBJ:	Declares for unicode output conversions.
*
*******************************************************************************/

#ifndef U_OUT_H_
#define U_OUT_H_


#ifndef LINUX
#ifdef UNICODE

#ifndef LINUX
#include <windows.h>
#endif

#include "machine.h"

//#include <tchar.h>
//#include <streamclasses.h>

#include "u_convert.h"

// INCONSISTENCY WITH MS VISUAL C++ ON VARIOUS PCS.				// 02/02/01 AM.
#ifdef NONSTD_
#define STDOPERATOR	operator
#else
#define STDOPERATOR	std::operator
#endif

// Move to prim library.			// 01/24/06 AM.
_t_ofstream &STDOPERATOR<<(_t_ofstream &os, _TCHAR* &lpszBuffer);
_t_ostream &STDOPERATOR<<(_t_ostream &os, _TCHAR* &lpszBuffer);


#else
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;
#include <strstream>
#endif

#endif
#endif

