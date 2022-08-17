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
*									PRIM.C
*
* FILE:	conan.�/prim.c
* SUBJ:	Primitive utilities.
* NOTE:	Machine dependencies should be localized in a file called mach.c,
*		to the extent possible.  ifdefs should enable each machine and
*		environment's code to reside in the same source files, across platforms.
*		This and other system-independent files should go in a Conceptual
*		library area.
* CR:	5/04/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prim/prim.h"


/**************************************************
*						CONFIRM
* FUN:	confirm
* SUBJ:	Stop processing till user confirms.
* CR:	10/21/95 AM.
* NOTE:	To pause processing in Conan application.
*
**************************************************/

LIBPRIM_API void
confirm()
{
_TCHAR line[STD_STR];

for (;;)
   {
   //fprintf(stdout, "Proceed? (y or n) ");			// 04/20/99 AM.
   //fflush(stdout);
   //fflush(stdin);
   //scanf("%s", line);
	std::_t_cout << _T("Proceed? (y or n) ") << std::flush;			// 04/20/99 AM.
	std::_t_cin >> line;
   if (*line == 'y')
      return;
   exit(0);
   
   }
}

/**************************************************
*						FILE_CLOSE
* FUN:	file_close
* SUBJ:	Close a file.
* CR:	5/04/95 AM.
* RET:	True if successful, else false.
*
**************************************************/

#ifdef _OBSOLETE_
LIBPRIM_API bool file_close(
	_TCHAR *nam,
//	FILE *fp				// 04/20/99 AM.
	_t_ostream *fp			// 04/20/99 AM.
	)
{
delete *fp;				// 04/20/99 AM.
return true;			// 04/20/99 AM.

//if (!fclose(fp))		// 04/20/99 AM.
//   return(TRUE);
//else
//   {
//   fprintf(stderr, "Couldn't close file=%s\n", nam);
//   return(FALSE);
//   }
}
#endif

/**************************************************
*						FILE_OPEN
* FUN:	file_open
* SUBJ:	Open a file.
* CR:	5/04/95 AM.
* RET:	True if successful, else false.
* NOTE:	04/20/99 AM.	REMOVING THIS.
*
**************************************************/

#ifdef _OBSOLETE_
LIBPRIM_API bool
file_open(
	_TCHAR *nam,			/* File name.		*/
   _TCHAR *mode,			/* "r", "w", or "a" */
	/*UP*/
	//FILE **fp			// 04/20/99 AM.
	_t_ofstream **fp;		// 04/20/99 AM.
	)
{
*fp = new _t_ofstream(nam, ios::out);						// 04/20/99 AM.

//if ((*fp = fopen(nam, mode)) != (FILE *)NULL)			// 04/20/99 AM.
//   return(TRUE);
//else
//   {
//   fprintf(stderr, "Couldn't open file=%s, mode=%s\n", nam, mode);
//   return(FALSE);
//   }
}
#endif

/**************************************************
*						FILE_SIZE
* FUN:	file_size
* SUBJ:	Get size of a file.
* CR:	11/1/95 AM.
* RET:	size - Size of file in bytes, or negative if failed.
* NOTE:	No idea how to optimize this for Macintosh.
*		One optimization would be to take file ptr as argument.
*
**************************************************/

#ifdef _OBSOLETE_
LIBPRIM_API long int file_size(
	_TCHAR *nam
	)
{

FILE *fp;
long int siz;

if (!file_open(nam, _T("r"), &fp))
   {
   _ftprintf(stderr, _T("[file_size: Couldn't open %s.]\n"), nam);
   return(-1L);
   }
fseek(fp, 0L, SEEK_END);
siz = ftell(fp);
if (!file_close(nam, fp))
   {
   _ftprintf(stderr, _T("[file_size: Couldn't close %s.]\n"), nam);
   return(-1L);
   }

return(siz);
}
#endif


/**************************************************
*						FILE_TO_BUF
* FUN:	file_to_buf
* SUBJ:	Read a file into a text buffer.
* CR:	11/2/95 AM.
* RET:	True if successful.
* ASS:	User must assure file will fit in buffer.
*
**************************************************/

#ifdef _OBSOLETE_
LIBPRIM_API bool
file_to_buf(
	_TCHAR *nam,
	_TCHAR *buf
	)
{
FILE *fp;
_TCHAR cc;

if (!file_open(nam, _T("r"), &fp))
   {
   _ftprintf(stderr, _T("[file_to_buf: Couldn't open %s.]\n"), nam);
   return(FALSE);
   }

/* Read file into buf */
while ((cc = _gettc(fp)) != _TEOF)
   *buf++ = cc;
*buf = '\0';

if (!file_close(nam, fp))
   {
   _ftprintf(stderr, _T("[file_to_buf: Couldn't close %s.]\n"), nam);
   return(FALSE);
   }
return(TRUE);
}
#endif
