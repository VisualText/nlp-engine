/*******************************************************************************
Copyright (c) 2005 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	U_OUT.H
* FILE:	lite/u_out.h
* CR:		03/10/05 AM.
* SUBJ:	Declares for unicode output conversions.
* NOTE:	01/13/06 AM. Copied from lite project.
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

#include <windows.h>
#include <tchar.h>
#include <my_tchar.h>

#include <streamclasses.h>

#include "u_convert.h"


#endif


#ifdef _OBSO_

// #include "u_convert.h"

// Dummy type definitions to use for casting data to be converted
// for output to a file.
//typedef struct {int x;} u_string;

_TCHAR getutf8(_t_istream *);
bool u_readbom(_t_ifstream *);

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



class u_string
{
friend _t_ofstream &operator<<(_t_ofstream &os, u_string* &str);
friend _t_ostream &operator<<(_t_ostream &os, u_string* &str);

// Attributes
public:

// Operations
public:
//        u_ofstream();
//        ~u_ofstream();
};

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
