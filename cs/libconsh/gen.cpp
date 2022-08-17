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
*									GEN.C
*
* FILE:	conan.�/gen.c
* SUBJ:	Module for generating code.
* NOTE:	Should separate primitives from consh_gen facilities.
*		Some old stuff still infests these files.
* CR:	5/06/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"
//#include "prim/mach.h"

#include "gen.h"


/**************************************************
*						WORDS_TO_ST_FILE
* FUN:	words_to_st_file
* SUBJ:	Convert a file of words to string table files.
* ASS:	Naming string table files based on string table names.
* ALG:	Read words file.  For every word, write word to string table
*		file.  If limit of file is exceeded, make a new file.
* CR:	5/06/95 AM.
* NOTE:	8/27/95 AM. OBSOLETE. Need to at least add syms also.
*
**************************************************/

#ifdef _OBSOLETE_
void
words_to_st_file(
	_TCHAR *words_f,		/* Name of words file. 						*/
	_TCHAR *s_base		/* Basic name for string table files.	*/
	)
{
_TCHAR s_nam[PATH],		/* Name of string table file.				*/
     s_tab[16],		/* Name of string table array.			*/
     buf[STD_STR],	/* Buffer for current word.				*/
     *tail;				/* Tail for naming generated files.		*/
int count,				/* Number of string table files.			*/
    len,					/* Length of current word.					*/
    line;				/* Count chars in current line.			*/
//long ii;
long off,				/* Offset in total string table.			*/
     max;				/* Size of string table segment.			*/
std::_t_ofstream *s_fp;				/* File ptr for string table file.		*/
_t_ifstream *w_fp;				/* Words file ptr.							*/

max = SEG_SIZE;

//if (!file_open(words_f, "r", &w_fp))
//   return;
w_fp = new _t_ifstream(words_f, ios::in | ios::nocreate);	// 04/20/99 AM.

count = 0;
tail = _T("cc");
_stprintf(s_tab, _T("%s%d"), s_base, count++);
_stprintf(s_nam, _T("%s.%s"), s_tab, tail);

//if (!file_open(s_nam, "w", &s_fp))
//   return;
s_fp = new std::_t_ofstream(s_nam);			// 04/20/99 AM.

gen_array_hd(_T("char"), s_tab, _T(""), s_fp);			/* Start array.				*/
*s_fp << _T("\"");							/* Start quote for string.	*/

/* Read word.  If it fits in current file, put it there.
   Else, terminate current file. Place word in new file. */
off = 0;
line = 0;
while ((*w_fp >> buf) != _TEOF)
   {
   len = _tcsclen(buf);
   //printf("off=%ld, len=%d, max=%ld\n", off, len, max);
   //printf("seg1=%ld seg2=%ld\n", (off + (long)len)/max, off/max);
   _t_cout << _T("off=") << off
		  << _T(", len=") << len
		  << _T(", max=") << max
		  << std::endl;
   _t_cout << _T("seg1=")
		  << (off + (long)len)/max
		  << _T(" seg2=")
		  << off/max
		  << std::endl;
   if ((((off + (long)len) / max) > (off / max))  || !(off % max))
      {
      
      /* Won't fit in current segment.  Make a new one. */
      line = 0;
      *s_fp << _T("\"");							/* End quote for string.	*/
      gen_array_tl(s_fp);							/* End array.				*/

      //if (!file_close(s_nam, s_fp))
      //   return;
		delete s_fp;

      /* Open a new file. */
      _stprintf(s_tab, _T("%s%d"), s_base, count++);
      _stprintf(s_nam, _T("%s.cpp"), s_tab);

      //if (!file_open(s_nam, "w", &s_fp))
      //   return;
		s_fp = new std::_t_ofstream(s_nam);			// 04/20/99 AM.

      gen_array_hd(_T("char"), s_tab, _T(""), s_fp);		/* Start array.				*/
      *s_fp << _T("\"");							/* Start quote for string.	*/

      }
   if (line + len < STD_LINE)
      line += len + 2;							/* Include '\' and '0' chars.	*/
   else
      {
//      *s_fp << "\\\n";
		*s_fp << _T("\n");																			// 06/28/00 AM.
      line = 0;
      }
   *s_fp << buf << _T("\\0");				/* Write curr string to file.	*/
   off += len + (long)1;							/* String + terminator.		*/
   }

/* Terminate the last file. */
*s_fp << _T("\"");								/* End quote for string.	*/
gen_array_tl(s_fp);									/* End array.				*/

//if (!file_close(s_nam, s_fp))
//   return;

//if (!file_close(words_f, w_fp))
//   return;
delete s_fp;
delete w_fp;
}
#endif

/**************************************************
*						GEN_ST_EMPTY
* FUN:	gen_st_empty
* SUBJ:	Generate an empty string table.
* CR:	5/06/95 AM.
* NOTE:	8/27/95 AM. OBSOLETE, I think.
*
**************************************************/

void
gen_st_empty(
	_TCHAR *s_tab,		/* Name of string table array. */
	_TCHAR *s_nam			/* Name of file for string table array.	*/
	)
{
long ii;
std::_t_ofstream *s_fp;			/* File ptr for string table file.		*/
//int count;			/* Number of string table files.		*/

//if (!file_open(s_nam, "w", &s_fp))
//   return;
s_fp = new std::_t_ofstream(TCHAR2A(s_nam));			// 04/20/99 AM.

gen_array_hd(_T("char"), s_tab, _T(""), s_fp);			/* Start array.				*/
*s_fp << _T("\"");							/* Start quote for string.	*/

for (ii = 0; ii < SEG_SIZE; ii++)
   {
   if (!(ii % (STD_LINE / 2)))
     // *s_fp << "\\\n";				/* Extend line every 80 chars	*/
		*s_fp << _T("\n");																			// 06/28/00 AM.
   *s_fp << _T("\\0");						/* Print empty string.		*/
   }

*s_fp << _T("\"");							/* End quote for string.	*/
gen_array_tl(s_fp);								/* End array.				*/

//if (!file_close(s_nam, s_fp))
//   return;
delete s_fp;
}


/**************************************************
*						GEN_ARRAY_*
* FUN:	gen_array_*
* SUBJ:	Generate code for an array.
* CR:	5/6/95 AM.
*
**************************************************/


void gen_array_decl(
	_TCHAR *typ,
	_TCHAR *nam,
	std::_t_ofstream *fp
	)
{
*fp << typ << _T(" ") << nam << _T("[];") << std::endl;
}

void gen_array_def(
	_TCHAR *typ,
	_TCHAR *nam,
	_TCHAR *siz,
	std::_t_ofstream *fp
	)
{
*fp << typ << _T(" ") << nam << _T("[") << siz << _T("];") << std::endl;
}

void gen_array_hd(
	_TCHAR *typ,
	_TCHAR *nam,
	_TCHAR *siz,
	std::_t_ofstream *fp
	)
{
*fp << typ << _T(" ") << nam << _T("[") << siz << _T("]=\n   {") << std::endl;
}

void gen_array_tl(
	std::_t_ofstream *fp
	)
{
*fp << _T("\n   };") << std::endl;
}


/**************************************************
*						GEN_FILE_HEAD
* FUN:	gen_file_head
* SUBJ:	Generate code for generic file head.
* CR:	5/20/95 AM.
*
**************************************************/

void gen_file_head(
	std::_t_ofstream *fp
	)
{
_TCHAR str[80];

//date_str(str, 80);					// 04/29/99 AM.
_stprintf(str, _T("%s"), _T("DUMMYDATE"));
*fp << _T("/*** ")
	 << str
	 << _T(" AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/")
	 << std::endl;
}

