/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									IO.H
*
* FILE:	conch./io.h
* SUBJ:	Declarations for input/output.
* CR:	8/20/95 AM.
*
*******************************************************************************/

extern LIBPRIM_API void
indent(int num, std::_t_ostream *out);

extern LIBPRIM_API _TCHAR
skip_blanks(_TCHAR cc, std::_t_istream *fp);


//extern LIBPRIM_API void
//tab(int num, ostream *out);

extern LIBPRIM_API bool resolve_file(_TCHAR*,_TCHAR*,_TCHAR*);

#ifndef DWORD
#define DWORD double
#endif

// Run without popping up DOS window.	// 03/16/07 AM.
LIBPRIM_API DWORD
run_silent(_TCHAR* strFunct, _TCHAR* strstrParams);

// Run without popping up DOS window.	// 09/15/08 AM.
LIBPRIM_API DWORD
run_silent(_TCHAR* strCMD);
