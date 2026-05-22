/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									PRIM.H
*
* FILE:	include/API/prim/prim.h
* SUBJ:	Declarations for utility functions.
* CR:		5/04/95 AM.
*
*******************************************************************************/

// 04/21/99 AM. Moved these definitions from mach.h to here.
// mach.h was deleted because the name conflicted with the same name
// in the Lite library.


// Need this in LINUX.	// 03/23/19 AM.
#ifdef LINUX
#define DWORD double
#endif

/* Define architecture here. */
//#define MACINTOSH			// 04/23/99 AM.
#define PC						// 04/23/99 AM.

#define _2_16		((long) 32768)
#define _32_K		_2_16

#ifndef LINUX
#if MSVC_VERSION < 1900
#define INFINITY
#endif
#endif

// Upped size by factor of 2.												// 04/20/01 AM.
#define SEG_SIZE	( ( _32_K * (long)2 ) - (long) 768)

// DIR_SEP is used by the -COMPILE emitter to join paths when writing
// generated KB files (cs/libconsh/*_gen.cpp, cs/libconsh/cmd.cpp). It
// must match the host filesystem's separator: backslash on Windows,
// forward slash on POSIX. Previously gated on PC / UNIX macros which
// were #define'd unconditionally (PC unconditional at line 26 above),
// so Linux builds were emitting backslash paths and the generated
// files landed in the analyzer root with literal '\' in their names.
// Mirrors the DIR_CH treatment in include/Api/machine-min.h.
#ifdef LINUX
#define DIR_SEP _T("/")
#else
#define DIR_SEP _T("\\")
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

#define STD_STR 256
#define PATH	STD_STR
#define TOK		STD_STR
#define STD_LINE 80

//typedef int bool;
typedef long ID;		/* For unique concept numbering. */
#define INULL  ((ID) 0)

//#ifndef TRUE
//#define TRUE	((bool) 1)
//#define FALSE	((bool) 0)
//#endif

extern LIBPRIM_API void	 confirm();

#ifdef _OBSOLETE_
extern LIBPRIM_API bool file_close(
	_TCHAR *nam,
	FILE *fp
	);
extern LIBPRIM_API bool
file_open(
	_TCHAR *nam,			/* File name.		*/
   _TCHAR *mode,			/* "r", "w", or "a" */
	/*UP*/
	FILE **fp
	);
extern LIBPRIM_API bool
file_to_buf(
	_TCHAR *nam,
	_TCHAR *buf
	);
extern LIBPRIM_API long int file_size(
	_TCHAR *nam
	);
#endif

/* Every program must define this function. */
extern LIBPRIM_API void		 prog_exit();

//#ifdef __cplusplus
//}
//#endif
