/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									ARG.H
*
* FILE:	conch.�/arg.h
* SUBJ:	Declarations for command-line parsing.
* CR:	8/20/95 AM.
*
*******************************************************************************/

extern LIBQCONSH_API bool
arg_get(
	_t_istream *fp,			/* Stream to read from.				*/
	ALIST *alist,		// List manager.									// 08/14/02 AM.
	/*DU*/
	_TCHAR *cup,			/* Lookahead char.					*/
	_TCHAR **pos,			/* Buffer for placing arg.			*/
	LIST **args,
	LIST **end			/* Args to add to.					*/
	);
extern LIBQCONSH_API bool
arg_get_comment(
	_t_istream *fp,			/* Stream to read from.				*/
	/*DU*/
	_TCHAR *cup			/* Lookahead char.					*/
	);
extern LIBQCONSH_API bool
arg_get_str(
	_t_istream *fp,			/* Stream to read from.			*/
	ALIST *alist,		// List manager.									// 08/14/02 AM.
	_t_ostream *out,																// 06/21/03 AM.
	/*DU*/
	_TCHAR *cup,			/* Lookahead char.				*/
	_TCHAR **pos,			/* Buffer for arg.				*/
	LIST **args,
	LIST **end		/* Args to add to.					*/
	);
extern LIBQCONSH_API void
args_pp(
	LIST *args,
	_t_ostream *out,
	_TCHAR *buf
	);
extern LIBQCONSH_API bool
args_read(
	_t_istream *fp,						/* Stream to read.			*/
	_t_ostream *out,						/* Stream to echo to.		*/
	bool silent_f,					/* True if not echoing.		*/
	ALIST *alist,		// List manager.									// 08/14/02 AM.
	_TCHAR *buf,						/* Buffer for args.			*/
	long bufsiz,		// Length of buffer.								// 07/22/04 AM.
	/*DU*/
	LIST **args						/* List of ptrs to args.	*/
	);
