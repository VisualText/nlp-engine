/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									ARG.C
*
* FILE:	conch.�/arg.c
* SUBJ:	Argument parser.
* CR:	8/26/95 AM.
* NOTE:	Parse arguments, C-style.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "u_out.h"					// 01/13/06 AM.
#include "prim/libprim.h"
#include "prim/unicode.h"			// 01/24/06 AM.
#include "prim/prim.h"
#include "prim/str.h"
//#include "prim/mach.h"
#include "prim/list_s.h"
#include "prim/list.h"
//#include "prim/io.h"

#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"
#include "qconsh/arg.h"

_TCHAR skip_b(_TCHAR cc, _t_istream *fp);		// 04/20/99 AM.



/**************************************************
*						ARGS_PP
* FUN:	args_pp
* SUBJ:	Pretty-print args list.
* NOTE:	Not a complete reverse of args_read.
* CR:	8/27/95 AM.
*
**************************************************/

LIBQCONSH_API void
args_pp(
	LIST *args,
	_t_ostream *out,
   _TCHAR *buf;
	)
{
_TCHAR *str;

#ifdef UNICODE
char *lpstr8;
#endif

while (str = ALIST::list_pop_buf(&args,buf))						// 08/14/02 AM.
   {
#ifdef UNICODE
	u_to_mbcs((LPCWSTR)str, CP_UTF8, (LPCTSTR*&)lpstr8);
   if (_tcschr(str, ' '))
      *out << _T("\"") << lpstr8 << _T("\" ");
   else
      *out << lpstr8 << _T(" ");
	u_delete((LPCTSTR*&)lpstr8);
#else
   if (_tcschr(str, ' '))
      *out << _T("\"") << str << _T("\" ");
   else
      *out << str << _T(" ");
#endif
   }
*out << endl;
}



/**************************************************
*						ARGS_READ
* FUN:	args_read
* SUBJ:	Read a line of arguments from input stream.
* CR:	8/26/95 AM.
* NOTE:	As in C, backslash escapes next char, double quotes enclose strings,
*		whitespace delimits arguments.  Double quotes surrounding string
*		are not part of string. (Nice to allow backslash <cr> as well, to
*		escape newline.  Also, allowing newline within string.)
*		Implementing arg list as simply a buffer, with each arg ending with
*		a '\0' char.  Eliminates overhead of managing list-objects.
*		Not checking for control chars, for now.  iscntrl() gives true for '\n',
*		probably EOF, so watch it.
*		10/15/95 AM. Semicolon starts a comment.
* RET:	True if successful read.  False if failure due to bad input.
*		UP buf - Arg strings stored here.
*		UP args - List of args read.
*
**************************************************/

LIBQCONSH_API bool
args_read(
	_t_istream *fp,						/* Stream to read.			*/
	_t_ostream *out,						/* Stream to echo to.		*/
	bool silent_f,					/* True if not echoing.		*/
	ALIST *alist,		// List manager.									// 08/14/02 AM.
	_TCHAR *buf,						/* Buffer for args.			*/
	long bufsiz,		// Length of buffer.								// 07/22/04 AM.
	/*DU*/
	LIST **args						/* List of ptrs to args.	*/
	)
{
_TCHAR cc;						/* Char to process. 		*/
bool ok;						/* Successful read.			*/
LIST *end;						/* End of args list.		*/

*args = end = LNULL;

#ifdef UNICODE
cc = getutf8(fp);
#else
cc = fp->get();					/* Lookahead char.			*/
#endif

if (fp->eof())																	// 12/18/01 AM.
	return true;	// EOF is ok.											// 12/18/01 AM.
ok = true;						/* Everything good so far.	*/
for (;;)						/* While getting args.		*/
   {
   *buf = '\0';					/* Initialize current arg.	*/
   //cc = skip_blanks(cc, fp);	/* Skip leading blanks.		*/
	cc = skip_b(cc, fp);				// 04/21/99 AM.
   switch (cc)
      {
		case '\0':	// Using this in case skip_b found EOF.		// 12/16/01 AM.
//    case EOF:	// Matches ANSI y-umlaut, so not using.		// 12/16/01 AM.
         if (!silent_f)		/* If echoing input. */
            args_pp(*args, out, buf);
         return(false);		// 05/02/99 AM. Unexpected eof.
      case '\n':
		case '\r':				// FIX.  04/21/99 AM.
			while (cc == '\r')	// 10/31/06 AM.
#ifdef UNICODE
				cc = getutf8(fp);	// 10/31/06 AM.
#else
				cc = fp->get();	// 10/31/06 AM.
#endif
         if (!silent_f)		/* If echoing input. */
            args_pp(*args, out, buf);
         return(true);
      case ';':					/* Get comment.				*/
#ifdef UNICODE
			cc = getutf8(fp);
#else
			cc = fp->get();
#endif
         ok = arg_get_comment(fp, &cc);
         break;
      case '"':					/* Get string arg.			*/
#ifdef UNICODE
			cc = getutf8(fp);
#else
         cc = fp->get();			/* Lookahead char.			*/
#endif
         ok = arg_get_str(fp, alist, out,								// 06/21/03 AM.
													&cc, &buf, args, &end);	// 08/14/02 AM.
         break;
      default:					/* Get regular arg.			*/
         ok = arg_get(fp, alist, &cc, &buf, args, &end);			// 08/14/02 AM.
         break;
      }
      
   if (!ok)
      return(false);
   }

if (!silent_f)		/* If echoing input. */
   args_pp(*args, out, buf);
return(ok);
}


/**************************************************
*						ARG_GET
* FUN:	arg_get
* SUBJ:	Read a non-string argument from a stream.
* ASS:	First char is nonwhite.
* CR:	8/26/95 AM.
* RET:	Lookahead char.
*
**************************************************/

LIBQCONSH_API bool
arg_get(
	_t_istream *fp,			/* Stream to read from.				*/
	ALIST *alist,		// List manager.									// 08/14/02 AM.
	/*DU*/
	_TCHAR *cup,			/* Lookahead char.					*/
	_TCHAR **pos,			/* Buffer for placing arg.			*/
	LIST **args,
	LIST **end			/* Args to add to.					*/
	)
{
_TCHAR cc;			/* Lookahead char.					*/
_TCHAR *buf;			/* Buffer pointer.	*/
int f_esc;			/* If backslash escape pending.		*/
bool ok;			/* If successful read.				*/

ok = true;
cc = *cup;
if (fp->eof())																	// 12/18/01 AM.
	cc = '\0';																	// 12/18/01 AM.
buf = *pos;
f_esc = 0;
for (;;)			/* While getting arg */
   {
   if (f_esc)			/* Add to current arg. */
      {
		if (!cc)																	// 12/16/01 AM.
//      if (cc == EOF)														// 12/16/01 AM.
         {
         _t_cerr << _T("[arg_get: Can't escape EOF.]") << endl;
         return(false);
         }

      *buf++ = cc;		/* Add char to current arg. */
      f_esc = 0;
#ifdef UNICODE
		cc = getutf8(fp);
#else
		cc = fp->get();
#endif
		if (fp->eof())															// 12/18/01 AM.
			cc = '\0';															// 12/18/01 AM.
      continue;
      }
      
   switch (cc)
      {
		case '\0':																// 12/16/01 AM.
 //     case EOF:																// 12/16/01 AM.
         *buf++ = '\0';
         *args = alist->list_add(*args, (long) *pos, end);		// 08/14/02 AM.
         *cup = cc;
         *pos = buf;
         return(ok);
      case '\\':
         f_esc = 1;
         break;
      case ' ':
      case '\n':
      case '\t':
      case '\r':
         *buf++ = '\0';
         *args = alist->list_add(*args, (long) *pos, end);
         while (cc == '\r')	// In case of cr-lf format.	// 10/31/06 AM.
#ifdef UNICODE
		cc = getutf8(fp);	// 10/31/06 AM.
#else
		cc = fp->get();	// 10/31/06 AM.
#endif
         *cup = cc;
         *pos = buf;
         return(ok);
      default:
         *buf++ = cc;	/* Add char to current arg. */
         break;
      }
#ifdef UNICODE
	cc = getutf8(fp);
#else
	cc = fp->get();
#endif
   if (fp->eof())																// 12/18/01 AM.
		cc = '\0';																// 12/18/01 AM.
   }
_t_cerr << _T("[arg_get: Program error.]") << endl;
return(false);
}


/**************************************************
*						ARG_GET_COMMENT
* FUN:	arg_get_comment
* SUBJ:	Skip over a comment in input command line.
* ASS:	First char is nonwhite.
* CR:	10/15/95 AM.
* RET:	True if ok read from stream.
*		(up) Lookahead char.
*
**************************************************/

LIBQCONSH_API bool
arg_get_comment(
	_t_istream *fp,			/* Stream to read from.				*/
	/*DU*/
	_TCHAR *cup			/* Lookahead char.					*/
	)
{
_TCHAR cc;			/* Lookahead char.					*/
bool ok;			/* If successful read.				*/

ok = true;
cc = *cup;
if (fp->eof())																	// 12/18/01 AM.
	cc = '\0';																	// 12/18/01 AM.
for (;;)			/* While getting arg */
   { 
   switch (cc)
      {
		case '\0':	// Using this for EOF.								// 12/16/01 AM.
//    case EOF:																// 12/16/01 AM.
         *cup = cc;
         return(ok);
      case '\n':
      case '\r':
			while (cc == '\r')	// 10/31/06 AM.
#ifdef UNICODE
				cc = getutf8(fp);	// 10/31/06 AM.
#else
				cc = fp->get();	// 10/31/06 AM.
#endif
         *cup = cc;
         return(ok);
      default:
         break;
      }
#ifdef UNICODE
	cc = getutf8(fp);
#else
	cc = fp->get();
#endif
   if (fp->eof())																// 12/18/01 AM.
		cc = '\0';																// 12/18/01 AM.
   }
_t_cerr << _T("[arg_get_comment: Program error.]") << endl;
return(false);
}


/**************************************************
*						ARG_GET_STR
* FUN:	arg_get_str
* SUBJ:	Read a string argument from a stream.
* ASS:	First char is one past double-quote; looking for end double-quote.
* CR:	8/26/95 AM.
* RET:	Lookahead char.
*
**************************************************/

LIBQCONSH_API bool
arg_get_str(
	_t_istream *fp,			/* Stream to read from.			*/
	ALIST *alist,		// List manager.									// 08/14/02 AM.
	_t_ostream *out,		// Stream to echo to.							// 06/21/03 AM.
	/*DU*/
	_TCHAR *cup,			/* Lookahead char.				*/
	_TCHAR **pos,			/* Buffer for arg.				*/
	LIST **args,
	LIST **end		/* Args to add to.					*/
	)
{
_TCHAR cc;			/* Lookahead char.				*/
_TCHAR *buf;			/* Buffer pointer.				*/
int f_esc;			/* If backslash escape pending. */
bool ok;			/* If successful read.			*/

ok = true;
cc = *cup;
if (fp->eof())																	// 12/18/01 AM.
	cc = '\0';																	// 12/18/01 AM.
buf = *pos;
f_esc = 0;
for (;;)			/* While getting arg */
   {
   if (f_esc)			/* Add to current arg. */
      {
		if (!cc)																	// 12/16/01 AM.
//      if (cc == EOF)														// 12/16/01 AM.
         {
         *out << _T("[arg_get_str: Can't escape EOF.]") << endl;	// 06/21/03 AM.
         return(false);
         }

      *buf++ = cc;		/* Add char to current arg. */
      f_esc = 0;
#ifdef UNICODE
		cc = getutf8(fp);
#else
		cc = fp->get();
#endif
		if (fp->eof())															// 12/18/01 AM.
			cc = '\0';															// 12/18/01 AM.
      continue;
      }
      
   switch (cc)
      {
		case '\0':																// 12/16/01 AM.
//    case EOF:																// 12/16/01 AM.
         *buf++ = '\0';
          *out																	// 06/21/03 AM.
				<< _T("[arg_get_str: Error. EOF in middle of string.]") << endl;
			*out << _T("[str=") << *pos << _T("]") << endl;						// 06/21/03 AM.
        *args = alist->list_add(*args, (long)*pos, end);
         *cup = cc;
         *pos = buf;
         return(false);
      case '\\':
         f_esc = 1;
         break;
      case '"':			/* Found end of string.		*/
#ifdef UNICODE
			cc = getutf8(fp);
#else
         cc = fp->get();	/* Lookahead char.			*/
#endif
			if (fp->eof())														// 12/18/01 AM.
				cc = '\0';														// 12/18/01 AM.
         if (!_istspace((_TUCHAR)cc))
            {
            *out << _T("[arg_get_str: Arg glommed to end of string.]") << endl;
            return(false);
            }
         *buf++ = '\0';
         *args = alist->list_add(*args, (long)*pos, end);
         *cup = cc;
         *pos = buf;
         return(ok);
      default:
         *buf++ = cc;	/* Add char to current arg. */
         break;
      }
#ifdef UNICODE
	cc = getutf8(fp);
#else
	cc = fp->get();
#endif
   if (fp->eof())																// 12/18/01 AM.
		cc = '\0';																// 12/18/01 AM.
   }
*out << _T("arg_get_str: Program error.") << endl;
return(false);
}


/**************************************************
*						SKIP_B
* FUN:	skip_b
* SUBJ:	Skip whitespace in input stream.
* ASS:	Look at given char.  Don't skip newlines.
* CR:	04/21/99 AM.
* RET:	char after whitespace.
* NOTE:	Calling skip_blanks from libprim results in access
*			violation.  Not sure why.
*	12/16/01 AM. Assuming that get() returns null if EOF found.
*
**************************************************/

_TCHAR
skip_b(_TCHAR cc, _t_istream *fp)		// 04/20/99 AM.
{
if (fp->eof())																	// 12/18/01 AM.
	return '\0';																// 12/18/01 AM.
for (;;)
   {
   switch (cc)
      {
      case ' ':			/* Whitespace.		*/
      case '\t':
			// 04/16/99 AM. Problems with macros in DLL libraries.
         //cc = getc(fp);		// 04/16/99 AM.
			//cc = fgetc(fp);		// 04/16/99 AM.
#ifdef UNICODE
			cc = getutf8(fp);
#else
			cc = fp->get();				// 04/20/99 AM.
#endif
			if (fp->eof())														// 12/18/01 AM.
				cc = '\0';														// 12/18/01 AM.
         break;
      default:			/* Newline,EOF,nonwhite. */
         return(cc);
      }
   }
}
