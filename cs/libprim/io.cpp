/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									IO.C
*
* FILE:	conch.�/io.c
* SUBJ:	Input/output stuff.
* CR:	8/26/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/str.h"
#include "prim/io.h"

// Local defs.
bool resolve_file_win(_TCHAR*,_TCHAR*,_TCHAR*);		// 07/01/03 AM.
bool resolve_file_unix(_TCHAR*,_TCHAR*,_TCHAR*);	// 07/01/03 AM.

/**************************************************
*						INDENT
* FUN:	indent
* SUBJ:	Print given number of spaces.
* CR:	10/28/95 AM.
*
**************************************************/

LIBPRIM_API void
//indent(int num, FILE *out)		// 04/20/99 AM.
indent(int num, std::_t_ostream *out)		// 04/20/99 AM.
{
while (num-- > 0)
   //fprintf(out, " ");				// 04/20/99 AM.
	*out << _T(" ");						// 04/20/99 AM.
*out << std::flush;							// 04/20/99 AM.
}


/**************************************************
*						SKIP_BLANKS
* FUN:	skip_blanks
* SUBJ:	Skip whitespace in input stream.
* ASS:	Look at given char.  Don't skip newlines.
* CR:	8/26/95 AM.
* RET:	char after whitespace.
*
**************************************************/

LIBPRIM_API _TCHAR
//skip_blanks(char cc, FILE *fp)			// 04/20/99 AM.
skip_blanks(_TCHAR cc, std::_t_istream *fp)		// 04/20/99 AM.
{
for (;;)
   {
   switch (cc)
      {
      case ' ':			/* Whitespace.		*/
      case '\t':
			// 04/16/99 AM. Problems with macros in DLL libraries.
         //cc = getc(fp);		// 04/16/99 AM.
			//cc = fgetc(fp);		// 04/16/99 AM.
			cc = fp->get();				// 04/20/99 AM.
         break;
      default:			/* Newline,EOF,nonwhite. */
         return(cc);
      }
   }
}



/**************************************************
*						TAB
* FUN:	tab
* SUBJ:	Print given number of tabs.
* CR:	10/28/95 AM.
*
**************************************************/

LIBPRIM_API void
//tab(int num, FILE *out)			// 04/20/99 AM.
// Name conflicts with Gnu	// 10/27/06 AM.
vttab(int num, std::_t_ostream *out)		// 04/20/99 AM.
{
while (num-- > 0)
   //fprintf(out, "\t");		// 04/20/99 AM.
	*out << _T("\t") << std::flush;		// 04/20/99 AM.
}

/********************************************
* FN:		RESOLVE_FILE
* CR:		07/01/03 AM.
* SUBJ:	Given filename and base dir, resolve.
* RET:	True if resolved ok, else false.
* NOTE:	Buf assumed to be large enough for
*			fname + bname + a few extra.
*			Assuming basename is good, if present.
********************************************/

LIBPRIM_API bool
resolve_file(_TCHAR *fname, _TCHAR *bname, /*DU*/ _TCHAR *buf)
{
#ifndef LINUX
return resolve_file_win(fname,bname,buf);
#else
return resolve_file_unix(fname,bname,buf);
#endif
}


// VARIANT.
bool
resolve_file_win(_TCHAR *fname, _TCHAR *bname, /*DU*/ _TCHAR *buf)
{
if (Str_empty(fname))
	{
	*buf = '\0';
	return false;
	}
if ((_istalpha(*fname) && *(fname+1) == ':')	// eg, "c:"
 || (*fname == '\\') )	// Another absolute name in Windows.
	{
	_tcscpy(buf, fname);
	return true;
	}

// Relative name

if (Str_empty(bname))
	{
	// Keep it relative and hope for the best.
	_tcscpy(buf, fname);
	return true;
	}

// If basename ends in slash, catenate.
_TCHAR *ptr = bname;
while (*ptr)
	++ptr;
--ptr;	// Back up to last char.
if (*ptr == '\\')	// Base ends in slash.
	{
	_stprintf(buf,_T("%s%s"),bname,fname);
	return true;
	}
_stprintf(buf,_T("%s\\%s"),bname,fname);
return true;
}


// VARIANT.
bool
resolve_file_unix(_TCHAR *fname, _TCHAR *bname, /*DU*/ _TCHAR *buf)
{
if (Str_empty(fname))
	{
	*buf = '\0';
	return false;
	}
if (*fname == '/')	// Name starts with slash.  Absolute Linux/Unix.
	{
	_tcscpy(buf, fname);
	return true;
	}

// Relative name

if (Str_empty(bname))
	{
	// Keep it relative and hope for the best.
	_tcscpy(buf, fname);
	return true;
	}

// If basename ends in slash, catenate.
_TCHAR *ptr = bname;
while (*ptr)
	++ptr;
--ptr;	// Back up to last char.
if (*ptr == '/')	// Base ends in slash.
	{
	_stprintf(buf,_T("%s%s"),bname,fname);
	return true;
	}
_stprintf(buf,_T("%s/%s"),bname,fname);
return true;
}

/********************************************
* FN:		RUN_SILENT
* CR:		03/16/07 AM.
* SUBJ:	Run without popping up an MS-DOS Command Window.
* RET:	T
* NOTE:	Taken from Steven Szelei at
*	http://www.codeproject.com/win32/runsilent.asp?df=100&forumid=4227&exp=0&select=581591
********************************************/

LIBPRIM_API DWORD
run_silent(_TCHAR* strFunct, _TCHAR* strstrParams)
{
#ifndef LINUX
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	_TCHAR Args[4096];
	_TCHAR *pEnvCMD = NULL;
	_TCHAR *pDefaultCMD = _T("CMD.EXE");
	ULONG rc;
	
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	Args[0] = 0;

	pEnvCMD = _tgetenv(_T("COMSPEC"));	// FIX.	// 06/04/07 AM.

	if(pEnvCMD){
		
		_tcscpy(Args, pEnvCMD);
	}
	else{
		_tcscpy(Args, pDefaultCMD);
	}

	// "/c" option - Do the command then terminate the command window
	_tcscat(Args, _T(" /c ")); 
	//the application you would like to run from the command window
	_tcscat(Args, strFunct);  
	_tcscat(Args, _T(" ")); 
	//the parameters passed to the application being run from the command window.
	_tcscat(Args, strstrParams); 

	if (!CreateProcess( NULL, Args, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, 
		NULL, 
		NULL,
		&StartupInfo,
		&ProcessInfo))
	{
		return GetLastError();		
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if(!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
		rc = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	return rc;
#else
	return 0;
#endif
}

// VARIANT	// 09/15/08 AM.
LIBPRIM_API DWORD
run_silent(_TCHAR* strCMD)
{
#ifndef LINUX
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	_TCHAR Args[4096];
	_TCHAR *pEnvCMD = NULL;
	_TCHAR *pDefaultCMD = _T("CMD.EXE");
	ULONG rc;
	
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	Args[0] = 0;

	pEnvCMD = _tgetenv(_T("COMSPEC"));	// FIX.	// 06/04/07 AM.

	if(pEnvCMD){
		
		_tcscpy(Args, pEnvCMD);
	}
	else{
		_tcscpy(Args, pDefaultCMD);
	}

	// "/c" option - Do the command then terminate the command window
	_tcscat(Args, _T(" /c ")); 
	//the application PLUS params you would like to run from the command window
	_tcscat(Args, strCMD);  

	if (!CreateProcess( NULL, Args, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, 
		NULL, 
		NULL,
		&StartupInfo,
		&ProcessInfo))
	{
		return GetLastError();		
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if(!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
		rc = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	return rc;
#else
	return 0;
#endif
}

// Possible alternates.	// 01/10 AM.
// ShellExecute(0, "open", "whatever.exe", args, 0, SW_HIDE);
