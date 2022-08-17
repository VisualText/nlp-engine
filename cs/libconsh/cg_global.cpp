/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CG_GLOBAL.CPP
* FILE:	libconsh/cg_global.cpp
* CR:		09/16/99 AM.
* SUBJ:	Global variables in LIBCONSH library.
* NOTE:	Doing a global output and error output which can be diverted to
*			files and so on.  Taken from the same setup in Lite library.
*			(I believe these output things don't cross the library boundary
*			nicely.)
*
*******************************************************************************/


#include "StdAfx.h"
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <ctype.h>
#include <string.h>

#include "prim/libprim.h"
#include "cg_global.h"

// VC FEATURE. NULL string bombs when printed out.
// Changed from SNULL so as not to conflict with libkbm def.	// 02/15/01 AM.
const _TCHAR *strNULL = _T("");

// A global output stream.
// This way, output will be dynamically controlled.
std::_t_ostream *cgout = &std::_t_cout;			// Default is standard output.

// Global error stream.
// This way, error output will be dynamically controlled.
//ostream *cgerr = &cerr;			// Default is standard error.
std::_t_ofstream *cgerr = 0;	// 07/18/03 AM.

/********************************************
* FN:		CGFILEOUT
* CR:		09/16/99 AM.
* SUBJ:	Set global output to a file.
********************************************/

void cgfileOut(_TCHAR *fname, /*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
fout = new std::_t_ofstream(TCHAR2A(fname), std::ios::out);
sout = cgout;				// Save current output stream.
cgout = fout;				// Bind output to file.
}


/********************************************
* FN:		CGRESETOUT
* CR:		09/16/99 AM.
* SUBJ:	Reset global output.
********************************************/

void cgresetOut(/*DU*/ std::_t_ofstream* &fout, std::_t_ostream* &sout)
{
cgout = sout;				// Restore current output stream.
delete fout;				// Done with file output stream.
fout = 0;
sout = 0;
}


/********************************************
* FN:		FILEERR
* CR:		03/22/99 AM.
* SUBJ:	Set global error output to a file.
********************************************/

//void cgfileErr(char *fname, /*DU*/ ofstream* &fout, ostream* &sout)
void cgfileErr(_TCHAR *fname)	// 07/18/03 AM.
{
//fout = new ofstream(fname, std::ios::out);
//sout = cgerr;				// Save current output stream.
//cgerr = fout;				// Bind output to file.
cgerr = new std::_t_ofstream(TCHAR2A(fname), std::ios::out);	// 07/18/03 AM.
}


/********************************************
* FN:		RESETERR
* CR:		03/22/99 AM.
* SUBJ:	Reset global error output.
********************************************/

//void cgresetErr(/*DU*/ ofstream* &fout, ostream* &sout)
void cgresetErr()	// 07/18/03 AM.
{
//cgerr = sout;				// Restore current output stream.
//delete fout;				// Done with file output stream.
//fout = 0;
//sout = 0;
if (cgerr)	// 07/18/03 AM.
	{
	delete cgerr;	// 07/18/03 AM.
	cgerr = 0;
	}
}
