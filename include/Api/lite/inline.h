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
* FN:		ASCII_LOWER
* CR:		08/03/26 AM.
* SUBJ:	Locale-independent lowercase for a 7-bit char.
* NOTE:	Helper for the ASCII fast paths below.  Deliberately NOT
*			locale-aware: it is only ever applied to bytes < 0x80.
********************************************/

inline unsigned char ascii_lower(unsigned char ch)
{
return (ch >= 'A' && ch <= 'Z') ? (unsigned char)(ch + ('a' - 'A')) : ch;
}

inline unsigned char ascii_upper(unsigned char ch)
{
return (ch >= 'a' && ch <= 'z') ? (unsigned char)(ch - ('a' - 'A')) : ch;
}


/********************************************
* FN:		ASCII_ONLY
* CR:		08/03/26 AM.
* SUBJ:	True if the string is entirely 7-bit.
* NOTE:	Helper for the ASCII fast paths.  One cheap pass lets callers skip
*			the ICU round trip entirely for the common case.
********************************************/

inline bool ascii_only(const _TCHAR *str)
{
const unsigned char *p = (const unsigned char *) str;
while (*p)
	{
	if (*p & 0x80)
		return false;
	++p;
	}
return true;
}


/********************************************
* FN:		STRCMP_I
* CR:		09/26/01 AM.
* SUBJ:	String compare, case insensitive.
* RET:	0 if equal, +1 or -1 according to lex order.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
* OPT:	08/03/26 AM. ASCII fast path.  This is the innermost function of
*			the rule matcher (see Pat::literalMatch), and building two ICU
*			UnicodeStrings per comparison -- transcoding UTF-8 to UTF-16 both
*			ways -- dominated the cost of a pass.  Compare bytewise while both
*			strings stay 7-bit and only fall through to ICU when a byte >= 0x80
*			actually shows up.
*			The fast path is exact, not an approximation: u_strcasecmp with
*			U_COMPARE_IGNORE_CASE does Unicode *case folding*, which is
*			locale-independent and which maps A-Z to a-z and nothing else
*			within the 7-bit range.
********************************************/

inline int strcmp_i(
	const _TCHAR *str1,
	const _TCHAR *str2
	)
{
// icu::StringPiece treats a null pointer as the empty string; preserve that
// rather than dereferencing it below.
if (!str1)
	str1 = _T("");
if (!str2)
	str2 = _T("");

const unsigned char *p1 = (const unsigned char *) str1;
const unsigned char *p2 = (const unsigned char *) str2;
for (;;)
	{
	unsigned char c1 = *p1;
	unsigned char c2 = *p2;
	if ((c1 | c2) & 0x80)
		break;					// Non-ASCII in play.  Hand off to ICU.
	if (c1 != c2)
		{
		c1 = ascii_lower(c1);
		c2 = ascii_lower(c2);
		if (c1 != c2)
			return (c1 < c2) ? -1 : 1;
		}
	else if (!c1)
		return 0;				// Both ended together.  Equal.
	++p1;
	++p2;
	}

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
