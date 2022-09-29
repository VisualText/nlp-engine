/*******************************************************************************
Copyright (c) 2005-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	U_OUT.H
* FILE:	lite/u_out.h
* CR:		03/10/05 AM.
* SUBJ:	Declares for unicode output conversions.
*
*******************************************************************************/

#ifndef U_OUT_H_
#define U_OUT_H_


#ifdef UNICODE

//#ifndef IO_LIBRARIES_
//#define IO_LIBRARIES_
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#endif


#include "u_convert.h"
#ifdef LINUX
#include "unicode/utypes.h"	// 03/03/05 AM.
#include "unicode/uchar.h"
#include "unicode/uscript.h"	// 01/30/06 AM.
#else
#include "utypes.h"	// 03/03/05 AM.
#include "uchar.h"
#include "uscript.h"	// 01/30/06 AM.
#endif

// Dummy type definitions to use for casting data to be converted
// for output to a file.
//typedef struct {int x;} u_string;


// Move to libstream library.			// 01/24/06 AM.
_t_ofstream &operator<<(_t_ofstream &os, _TCHAR* &lpszBuffer);
_t_ostream &operator<<(_t_ostream &os, _TCHAR* &lpszBuffer);

extern bool alphabetic_extend(		// 01/30/06 AM.
	UChar32 ch,
	short p_gcb,
	short p_wb,
	short p_script,
	/*UP*/
	short &new_gcb,
	short &new_wb,
	short &new_script
	);


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

#else
//#include <strstream>
#include <sstream>
typedef _TCHAR u_string;	// 01/19/06 AM.
#endif

#endif
