/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									STR.H
*
* FILE:	include/API/prim/str.h
* SUBJ:	Declarations for string primitives.
* CR:		5/11/95 AM.
*
*******************************************************************************/


#define cNULL	((_TCHAR *) NULL)

#ifdef __cplusplus
extern "C" {
#endif

extern LIBPRIM_API
bool cg_alphabetic(_TCHAR ch);				// 10/05/99 AM.
LIBPRIM_API
bool cg_punct(_TCHAR ch);						// 10/08/99 AM.

extern LIBPRIM_API bool
s_to_l(_TCHAR *str, /*UP*/ long *num);
extern LIBPRIM_API bool s_to_f(_TCHAR *, /*UP*/ float *);			// 08/16/01 AM.
extern LIBPRIM_API _TCHAR *
s_to_lower(_TCHAR *str);
extern LIBPRIM_API _TCHAR *
s_to_upper(_TCHAR *str);
extern LIBPRIM_API int strlen_cmp(_TCHAR *str, long num);
extern LIBPRIM_API int strstr_cmp(_TCHAR *str1, _TCHAR *str2);

extern LIBPRIM_API bool		 str_full(_TCHAR *str);
extern LIBPRIM_API bool		 str_empty(_TCHAR *str);

extern LIBPRIM_API _TCHAR *prep_str(		// 04/30/99 AM.
	_TCHAR *buf,
	_TCHAR *str				// String to convert.
	);

extern LIBPRIM_API _TCHAR *num_to_hex(long, /*UP*/ _TCHAR *);

#ifdef __cplusplus
}
#endif

/* MACROS */
#define Str_0(str)	(*(str) = '\0')
#define str_0(str)	Str_0(str)
#define Str_full(str)	((str) && *(str))
//#define str_full(str)	((str) && *(str))
#define Str_empty(str)	(!(str) || !*(str))
//#define str_empty(str)	(!(str) || !*(str))

