/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	INLINE.H
* FILE:	lite/inline.h
* CR:		05/19/99 AM.
* SUBJ:	Inlined functions here.
* NOTE:	Pulling some out of global.h, since inline stuff doesn't export
*			from libraries.
*			Keep in include/API/lite, since standalone drivers use it.
*
*******************************************************************************/
#ifndef INLINE_H_
#define INLINE_H_

#ifndef LINUX
// 09/26/01 AM.
#include <tchar.h>
#endif
#include "../prim/unicu.h"

// A wrapper to keep things from blowing up if string is empty.
inline const _TCHAR *str(_TCHAR *x) { return (x ? x : STRNULL); }
//#define str(X)	((X) ? (X) : (STRNULL))


inline bool empty(_TCHAR *x) { return (!x || !*x);}

inline bool special(_TCHAR *str)
{
return (!empty(str)
	     && !_tcsncmp(str, _T("_x"), 2)		// Special, eg, "_xWHITE".
		  && _istupper(str[2])					// (Note: case-sensitive)
		  && _tcscmp(str, _T("_xNIL"))			// Not special, as far as matching.
		  );
}


/********************************************
* FN:		STRCMP_I
* CR:		09/26/01 AM.
* SUBJ:	String compare, case insensitive.
* RET:	0 if equal, +1 or -1 according to lex order.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
********************************************/

inline int strcmp_i(
	const _TCHAR *str1,
	const _TCHAR *str2
	)
{
icu::UnicodeString ustr1 = icu::UnicodeString::fromUTF8(icu::StringPiece(str1 ));
const UChar *strBuff1 = reinterpret_cast<const UChar *>(ustr1.getTerminatedBuffer());
icu::UnicodeString ustr2 = icu::UnicodeString::fromUTF8(icu::StringPiece(str2));
const UChar *strBuff2 = reinterpret_cast<const UChar *>(ustr2.getTerminatedBuffer());
int ret = u_strcasecmp(strBuff1, strBuff2, U_COMPARE_IGNORE_CASE);

return ret;
/*
#ifndef LINUX
return _tcsicmp(str1,str2);
#else
return strcasecmp(str1,str2);
#endif
*/
}



/********************************************
* FN:		STRCMP_NI
* CR:		09/26/01 AM.
* SUBJ:	String compare, numeric & case insensitive.
* RET:	0 if equal, +1 or -1 according to lex order.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
********************************************/

inline int strcmp_ni(
	const _TCHAR *str1,
	const _TCHAR *str2,
	long count
	)
{
#ifndef LINUX
return _tcsnicmp(str1,str2,count);
#else
return strncasecmp(str1,str2,count);
#endif
}


/********************************************
* FN:		STRCPY_EE
* CR:		09/27/00 AM.
* SUBJ:	String copy specialized for repeated copying.
* RET:	True if ok, else false.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
*			EE == "End,pointing to Empty".
*			10/08/03 AM. MOVED HERE FOR UCI WIN2K.
********************************************/

inline void strcpy_ee(
	_TCHAR* &ptr,			// Pointer to buffer for copying into.
							// Points to first EMPTY place.
	_TCHAR *str			// Terminated string to copy.
	)
{
while ((*ptr++ = *str++))
	;
--ptr;					// Back to overwrite newline.
}


#endif
