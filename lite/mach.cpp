/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MACH.CPP
* FILE:	lite\mach.cpp
* CR:		10/29/98 AM.
* SUBJ:	Things that may be machine specific.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#ifndef LINUX
#include <direct.h>
#endif

#include <sys/types.h>			// WORKAROUND.		// 02/25/00 AM.
#include <sys/timeb.h>			// WORKAROUND.		// 02/25/00 AM.

#include "machine.h"	// 10/25/06 AM.
#include "prim/libprim.h"	// 01/24/06 AM.
#include "u_out.h"		// 01/24/06 AM.
#include "lite/global.h"
#include "inline.h"															// 09/26/01 AM.
#include "lite/mach.h"
#include "lite/dir.h"

/********************************************
* FN:		TODAY
* CR:		10/29/98 AM.
* SUBJ:	Get the current date and time.
* NOTE:	Annoying that ctime includes a carriage return at end of string.
********************************************/

LITE_API _TCHAR *today()
{
static _TCHAR buf[64];
_TCHAR *tptr;				// Debugging mem leak.							// 02/25/00 AM.
time_t ltime;
struct tm *newtime;	// FIX MEM LEAK!!									// 02/25/00 AM.

time(&ltime);
newtime = localtime(&ltime);	// TRYING.								// 02/25/00 AM.
tptr = _tasctime(newtime);		// TRYING.								// 02/25/00 AM.
//tptr = ctime(&ltime);														// 02/25/00 AM.
_tcscpy(buf, tptr);															// 02/25/00 AM.
_TCHAR *ptr;
ptr = _tcschr(buf, '\n');
if (ptr)
	*ptr = '\0';		// Wipe out first newline.
return buf;
}


/**************************************************
*						DATE_STR
* FUN:	date_str
* SUBJ:	Get current date-time, nicely formatted.
* CR:	10/7/95 AM.
* RET:	str - formatted date-time.
* NOTE:	04/20/99 AM. Taken from LIBPRIM (Consh&Conan), so as to remove
*			the mach.cpp file from that library.
*
**************************************************/

LITE_API void
date_str(
	_TCHAR str[],		/* Buffer to fill with date-time. */
	int siz		/* Max size of buffer. */
	)
{
time_t now;
struct tm *date;

/* Get the current calendar time. */
time(&now);

/* Break down time. */
date = localtime(&now);

/* Convert to string. */
_tcsftime(str, siz, _T("%x %H:%M"), date);
}

/********************************************
* FN:		DOSREADARGS
* CR:		11/30/98 AM.
* SUBJ:	Get command line arguments.
* RET:	True if ok read, else false if failed.
********************************************/

LITE_API bool dosReadargs(
	int argc,
	_TCHAR *argv[],
	/*UP*/
	_TCHAR* &input,				// Input file from args.
	_TCHAR* &output,				// Output file from args.
	bool &develop,				// Development mode (output intermediate files).
	bool &compiled	// true - compiled ana. false=interp(DEFAULT).
	)
{
_TCHAR *ptr;
_TCHAR **parg;
bool f_in  = false;
bool f_out = false;
bool flag  = false;
bool compiledck = false;	// If compiled/interp arg seen.		// 07/05/00 AM.

input = output = 0;
develop = false;				// Default is not development mode. // 12/25/98 AM.
compiled = false;	// INTERP ANALYZER BY DEFAULT.					// 07/05/00 AM.

for (--argc, parg = &(argv[1]); argc > 0; --argc, ++parg)
	{
	// For each command line argument.
	//*gout << "command arg=" << *parg << std::endl;
	ptr = *parg;
	if (*ptr == '/' || *ptr == '-')	// DOS or UNIX style arg.
		{
		if (flag)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error in command line args for ") << argv[0]
						  << _T("]") << std::ends;
			errOut(&gerrStr,false);
			return false;
			}
		++ptr;
		if (!strcmp_i(ptr, _T("in")))
			f_in = flag = true;					// Expecting input file.
		else if (!strcmp_i(ptr, _T("out")))
			f_out = flag = true;					// Expecting output file.
		else if (!strcmp_i(ptr, _T("dev")))		// 12/25/98 AM.
			develop = true;						// Development mode.
		else if (!strcmp_i(ptr, _T("interp")))	// Run interpreted analyzer.
			{
			if (compiledck)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Ignoring extra /compiled or /interp flag.]") << std::ends;
				errOut(&gerrStr,false);
				}

			else
				{
				compiledck = true;
				compiled = false;
				}
			}
		else if (!strcmp_i(ptr, _T("compiled")))	// Run compiled analyzer.
			{
			if (compiledck)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Ignoring extra /compiled or /interp flag.]") << std::ends;
				errOut(&gerrStr,false);
				}

			else
				{
				compiledck = true;
				compiled = true;
				}
			}
		}
	else if (flag)							// Expected an argument value.
		{
		if (f_in)
			{
			if (input)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[") << argv[0] << _T(": Input file specified twice.]")
						  << std::ends;
				errOut(&gerrStr,false);
				dosHelpargs(argv[0]);
				return false;
				}
			// Grab value as input file.
			input = ptr;
			f_in = flag = false;
			}
		else if (f_out)
			{
			if (output)
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[") << argv[0]
						  << _T(": Output file specified twice.]") << std::ends;
				errOut(&gerrStr,false);
				dosHelpargs(argv[0]);
				return false;
				}
			// Grab value as output file.
			output = ptr;
			f_out = flag = false;
			}
		}
	else										// Got a "floating" value.
		{
		if (input && output)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[") << argv[0] << _T(": Extra arguments.]") << std::ends;
			errOut(&gerrStr,false);
			dosHelpargs(argv[0]);
			return false;
			}
		else if (input)
			output = ptr;
		else
			input = ptr;
		}
	}
return true;
}


/********************************************
* FN:		DOSHELPARGS
* CR:		11/30/98 AM.
* SUBJ:	Print command line argument help.
* RET:	True if ok read, else false if failed.
********************************************/

LITE_API void dosHelpargs(_TCHAR *name)
{
std::_t_cout << name
<< _T(" [/INTERP][/COMPILED][/IN infile] [/OUT outfile] [/DEV] [infile [outfile]]")
	  << std::endl
	  << _T("Note: /INTERP, the interpreted analyzer, is default.")
	  << std::endl;
}

////////////////////////////////////////////////////////////
#ifdef TIMING_REFERENCE_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void _tmain( void ){
   time_t   start, finish;
	   long loop;
		   double   result, elapsed_time;
   _tprintf( _T("Multiplying 2 floating point numbers 10 million times...\n") );   
   time( &start );
	   for( loop = 0; loop < 10000000; loop++ )
      result = 3.63 * 5.27;
		    time( &finish );
   elapsed_time = difftime( finish, start );
   _tprintf( _T("\nProgram takes %6.0f seconds.\n"), elapsed_time );
	}
#endif
////////////////////////////////////////////////////////////////


/********************************************
* FN:		DIR_EXISTS
* CR:		03/10/99 AM.
* SUBJ:	See if given directory exists.
* RET:	True if exists.
* OPT:	Probably not the most efficient or principled method.
********************************************/

LITE_API bool dir_exists(_TCHAR *name)
{
_TCHAR cwd[MAXSTR];

/* Get the current working directory: */
#ifndef LINUX
if(_tgetcwd( cwd, MAXSTR) == NULL)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[dir_exists: Couldn't get working dir.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

if(_tchdir(name))
	return false;

// Was able to change to it. Now change back.
_tchdir(cwd);
#endif
return true;
}


/********************************************
* FN:		MAKE_DIR
* CR:		03/10/99 AM.
* SUBJ:	Make given directory.
* RET:	True if exists.
* OPT:	Probably not the most efficient or principled method.
********************************************/

LITE_API bool make_dir(_TCHAR *name)
{
#ifndef LINUX
if(_tmkdir(name) == 0)
	return true;
#endif
return false;
}


/********************************************
* FN:		RM_DIR
* CR:		03/10/99 AM.
* SUBJ:	Remove given directory.  Must be empty.
* RET:	True if exists.
* OPT:	Probably not the most efficient or principled method.
********************************************/

LITE_API bool rm_dir(_TCHAR *name)
{
#ifndef LINUX
if(_trmdir(name) == 0)
	return true;
#endif
return false;
}


/********************************************
* FN:		REMOVE_PATH
* CR:		03/10/99 AM.
* SUBJ:	Remove given path.
* RET:	True if success.
* NOTE:	Remove file or remove directory.
********************************************/

LITE_API bool remove_path(_TCHAR *name)
{
if (!safe_dir(name))
	return false;
if(_tremove(name) == 0)
	return true;
return false;
}

/********************************************
* FN:		TODAY1
* CR:		02/25/00 AM.
* SUBJ:	Get the current date and time.
* NOTE:	Trying workaround to spurious error message from Purify 6.5,
*			about memory leak in asctime when that fn is in a dll.
********************************************/

LITE_API _TCHAR *today1()
{
static _TCHAR buf[256];

_TCHAR tmpbuf1[128], tmpbuf2[128];

/* Set time zone from TZ environment variable. If TZ is not set,
* the operating system is queried to obtain the default value 
* for the variable. 
*/
#ifndef LINUX
_tzset();

/* Display operating system-style date and time. */
_tstrtime( tmpbuf1 );
_tstrdate( tmpbuf2 );
_stprintf(buf, _T("%s %s"), tmpbuf1, tmpbuf2);
return buf;
#else
return today();
#endif
}
