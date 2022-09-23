/*******************************************************************************
Copyright (c) 2005-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	U_OUT.H
* FILE:	prim/u_out.h
* CR:		03/10/05 AM.
* SUBJ:	Declares for unicode output conversions.
*
*******************************************************************************/

#ifndef U_OUT_H_
#define U_OUT_H_


#ifdef UNICODE

#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <iostream>
#include <fstream>
#include <sstream>
#endif

#ifndef LINUX
#include <windows.h>
#endif
#include <tchar.h>
#include <my_tchar.h>

#include <streamclasses.h>

#include "u_convert.h"

// INCONSISTENCY WITH MS VISUAL C++ ON VARIOUS PCS.				// 02/02/01 AM.
#ifdef NONSTD_
#define STDOPERATOR	operator
#else
#define STDOPERATOR	std::operator
#endif


#ifndef _U_OUTOVERRIDE_
#define _U_OUTOVERRIDE_
// Move to prim library.			// 01/24/06 AM.
_t_ofstream &STDOPERATOR<<(_t_ofstream &os, _TCHAR* &lpszBuffer);
_t_ostream &STDOPERATOR<<(_t_ostream &os, _TCHAR* &lpszBuffer);
#endif


#else
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#ifdef LINUX
#include <sstream>
#else
#include <strstream>
#endif

#endif

#endif

