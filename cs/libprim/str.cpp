/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									STR.C
*
* FILE:	cs/libstream/str.c
* SUBJ:	Primitive string functions.
* NOTE:	
* CR:		5/10/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"

#ifdef LINUX
#include <stdio.h>
#else
//#include <stdio.h>
#endif

//#include <stdlib.h>
//#include <ctype.h>
#ifdef LINUX
//#include <_t_strstream.h>		// 01/17/06 AM.
#include <iostream>
#include <sstream>				// 10/23/06 AM.
#else
#ifdef UNICODE
#include <sstream>				// 01/17/06 AM.
#else
#include <strstream>				// 01/17/06 AM.
#endif
#endif
#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/str.h"

#ifdef UNICODE
#include "unicode/utypes.h"	// 03/03/05 AM.
#include "unicode/uchar.h"
#endif

/********************************************
* FN:		CG_ALPHABETIC
* CR:		10/05/99 AM.
* SUBJ:	Handling some extended ASCII chars as alphabetic.
* RET:	True if alphabetic else false.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
*			TAKEN FROM lite/str/alphabetic.  Should have a global library
*			of primitives.
* OPT:	Could optimize by using bits in the char, or by setting up a
*			switch statement.
********************************************/

#ifdef _OLD_
LIBPRIM_API
bool cg_alphabetic(_TCHAR ch)
{
#ifndef LINUX
_TUCHAR xx = (_TUCHAR)ch;
if (xx >= 0 && xx <= 128)
	{
	if (_istalpha(ch))
		return true;
	}
else if (xx >= 192 && xx <= 214
		|| xx >= 216 && xx <= 221
		|| xx >= 224 && xx <= 246
		|| xx >= 248 && xx <= 253
		)
	return true;
return false;
#else
return _istalpha((_TUCHAR)ch);
#endif
}
#endif

// Taken from lite\alphabetic	// 01/17/06 AM.
LIBPRIM_API bool cg_alphabetic(_TCHAR ch)
{
//#ifndef LINUX
#ifndef UNICODE
switch((_TUCHAR)ch)
	{
	case  65: case  66: case  67: case  68: case  69: case  70: case  71:
   case  72: case  73: case  74: case  75: case  76: case  77: case  78:
	case  79: case  80: case  81: case  82: case  83: case  84: case  85:
	case  86: case  87: case  88: case  89: case  90:

	case  97: case  98: case  99: case 100: case 101: case 102: case 103:
	case 104: case 105: case 106: case 107: case 108: case 109: case 110:
	case 111: case 112: case 113: case 114: case 115: case 116: case 117:
	case 118: case 119: case 120: case 121: case 122:

	case 138: case 140:	// Upper S, OE.

	case 154: case 156:	// Lower S, OE.

	case 159:	// Upper Y umlaut.

	case 192: case 193: case 194: case 195: case 196: case 197: case 198:
	case 199: case 200: case 201: case 202: case 203: case 204: case 205:
	case 206: case 207: case 208: case 209: case 210: case 211: case 212:
	case 213: case 214:

	case 216: case 217: case 218: case 219: case 220: case 221: case 222:

	case 223:	// German double-S.  Uppercase is ?
	
	case 224: case 225: case 226: case 227: case 228: case 229: case 230:
	case 231: case 232: case 233: case 234: case 235: case 236: case 237:
	case 238: case 239: case 240: case 241: case 242: case 243: case 244:
	case 245: case 246:

	case 248: case 249: case 250: case 251: case 252: case 253: case 254:

	case 255:	// Lower Y umlaut.
		return true;
	default:
		return false;
	}
return false;
#else
//return _istalpha((_TUCHAR)ch);	// 03/06/00 AM.
return u_isalpha(ch) ? true : false;	// 03/27/05 AM.
#endif 
}

/********************************************
* FN:		CG_PUNCT
* CR:		10/08/99 AM.
* SUBJ:	Handling some extended ASCII chars as punctuation.
* RET:	True if punctuation else false.
* NOTE:	The ANSI CHARACTER SET.  Used in Windows OS.
*			Should have a global library of primitives.
* OPT:	Could optimize by using bits in the char, or by setting up a
*			switch statement.
********************************************/

LIBPRIM_API
bool cg_punct(_TCHAR ch)
{
#ifndef LINUX
_TUCHAR xx = (_TUCHAR)ch;
if (xx >= 0 && xx <= 128)
	{
	if (_istpunct((_TUCHAR)ch))
		return true;
	}
else if (xx >= 130 && xx <= 137
		|| xx == 139
		|| xx >= 145 && xx <= 153
		|| xx == 155
		|| xx == 159
		|| xx >= 161 && xx <= 191
		|| xx == 215
		|| xx == 247
		)
	return true;
return false;
#else
return _istpunct((_TUCHAR)ch);
#endif
}


/**************************************************
*						S_TO_L
* FUN:	s_to_l
* SUBJ:	Convert string to long.
* CR:		10/12/95 AM.
* RET:	ok - true if a numeric string, else false.
*			(up) num - converted long integer.
* NOTE:	Not using atol because I need better checking.
*			Leading zeros allowed.
* ASS:	Assumes ASCII character set.
*
**************************************************/

LIBPRIM_API bool
s_to_l(_TCHAR *str, /*UP*/ long *num)
{
long tot, dig;

*num = -1;
if (!str || !*str)
   return(false);

/* Strip leading zeros */
while (*str == '0')												// OPT.	// 01/17/03 AM.
   ++str;															// OPT.	// 08/14/02 AM.

// Handle negative integers.									// FIX.	// 01/17/03 AM.
bool bneg = false;															// 01/17/03 AM.
switch (*str)																	// 01/17/03 AM.
	{
	case '-':																	// 01/17/03 AM.
		if (!*++str)			// Slough the minus sign.				// 01/17/03 AM.
			return false;														// 01/17/03 AM.
		bneg = true;															// 01/17/03 AM.
		break;
	case '+':																	// 01/17/03 AM.
		if (!*++str)			// Slough the plus sign.				// 01/17/03 AM.
		break;
	default:																		// 01/17/03 AM.
		break;
	}

tot = (long) 0;

while (str && *str)
   {
   dig = (long) *str - (long) '0';			/* ASCII dependency */
   if (dig > (long) 9 || dig < (long) 0)	/* Non digit in string. */
      return(false);
   tot = tot * (long) 10 + dig;
   ++str;															// OPT.	// 08/14/02 AM.
   }
*num = (bneg ? (- tot) : tot);											// 01/17/03 AM.
return(true);
}


/**************************************************
*						S_TO_F
* FUN:	s_to_f
* SUBJ:	Convert string to float.
* CR:		08/16/01 AM.
* RET:	ok - true if a float string, else false.
*			(up) fnum - converted float.
* NOTE:	Not using atol because I need better checking.
*			Leading zeros allowed.
* ASS:	Assumes ASCII character set.
*
**************************************************/

LIBPRIM_API bool
s_to_f(_TCHAR *str, /*UP*/ float *fnum)
{
long dig;
float tot;

*fnum = -1.0;
if (!str || !*str)
   return(false);

/* Strip leading zeros */
while (*str == '0')
   ++str;

tot = 0.0;

*fnum = 0.0;
while (_istdigit(*str))
   {
   dig = (long) *str - (long) '0';			/* ASCII dependency */
   tot = tot * (float)10.0 + dig;
   ++str;
   }
*fnum = (float) tot;
if (*str && *str == '.')
	++str;											// Slough period.
tot = 0.0;											// RESET.
long div = 1;										// Divisor.
long maxdiv = 1000000000;						// Max divisor.
while (_istdigit(*str))
   {
   dig = (long) *str - (long) '0';			/* ASCII dependency */
   tot = tot * (float)10.0 + dig;

	// If maxed out on accuracy.
	div *= (long) 10;
	if (div == maxdiv)
		break;
   ++str;
   }
if (*str && !_istdigit(*str))					// Nondigit in string.
	{
	*fnum = 0.0;
	return false;
	}
*fnum += ( tot / ((float) div) );
return(true);
}



/**************************************************
*						S_TO_LOWER
* FUN:	s_to_lower
* SUBJ:	Convert string to lower case.
* CR:		10/25/95 AM.
* RET:	str - Lowercased string.
* WARN:	Replaces string in place.  User must copy as needed.
* ASS:	Assumes ASCII character set.
*
**************************************************/

LIBPRIM_API _TCHAR *
s_to_lower(_TCHAR *str)
{
_TCHAR *save;

if (!str)
   return(str);
save = str;
while (*str)
   {
   *str = _totlower(*str);
   ++str;															// OPT.	// 08/14/02 AM.
   }
return(save);
}


/**************************************************
*						S_TO_UPPER
* FUN:	s_to_upper
* SUBJ:	Convert string to upper case.
* CR:		10/25/95 AM.
* RET:	str - Uppercased string.
* WARN:	Replaces string in place.  User must copy as needed.
* ASS:	Assumes ASCII character set.
*
**************************************************/

LIBPRIM_API _TCHAR *
s_to_upper(_TCHAR *str)
{
_TCHAR *save;

if (!str)
   return(str);
save = str;
while (*str)
   {
   *str = _totupper(*str);
   ++str;															// OPT.	// 08/14/02 AM.
   }
return(save);
}


/**************************************************
*						STRLEN_CMP
* FUN:	strlen_cmp
* SUBJ:	Compare length of a string to an integer.
* CR:		5/10/95 AM.
* RET:	+ if strlen >  num
*			0 if strlen == num
*			- if strlen <  num
* NOTE:	Treating NULL and "" identically.
* OPT:	Minimizes getting full length of a string.
*
**************************************************/

LIBPRIM_API int strlen_cmp(_TCHAR *str, long num)
{
if (num < 0)
   {
//   _ftprintf(stderr, _T("[strlen_cmp: Error. len=%ld]\n"), num); // 09/18/20 AM. hack comment.
   return(1);			/* Recover.	*/
   }
if (!str)
   return(-num);

while (*str++ && num--)
   ;

if (!*(str-1))
   return(-num);		/* strlen <= num	*/
else
   return(1);			/* strlen > num */
}


/**************************************************
*						STRSTR_CMP
* FUN:	strstr_cmp
* SUBJ:	Compare length of two strings.
* CR:		5/11/95 AM.
* RET:	+ if str1 >  str2
*			0 if str1 == str2
*			- if str1 <  str2
* NOTE:	Treating NULL and "" identically.
* OPT:	Minimizes getting full length of a string.
*
**************************************************/

LIBPRIM_API int strstr_cmp(_TCHAR *str1, _TCHAR *str2)
{
if (str1 && str2)
   {
   while (*str1++ && *str2++)
      ;
   if (!*(str1-1))
      return(-*str2);
   else
      return(*(str1-1));
   }
else if (!str1)
   return(-*str2);
else	/* str1 && !str2 */
   return(1);
}



/* MISCELLANEOUS STRING PRIMITIVES */
/* These force evaluation of the string argument, unlike the macros. */
/* The corresponding macros shouldn't be used if the str argument is
a complex expression (especially with side-effects) */

// 04/16/99 AM. The macros don't cross the library boundary, so putting
// them inline in these functions.

LIBPRIM_API bool str_empty(_TCHAR *str)
{
//return(Str_empty(str));			// 04/16/99 AM.
return (!(str) || !*(str));		// 04/16/99 AM.
}

LIBPRIM_API bool str_full(_TCHAR *str)
{
//return(Str_full(str));			// 04/16/99 AM.
return ((str) && *(str));
}


/**************************************************
*						PREP_STR
* FUN:	prep_str
* SUBJ:	Build a string with special chars escaped.
* CR:		04/30/99 AM.
* RET:	Buffer holding new string.
*
**************************************************/

LIBPRIM_API _TCHAR *prep_str(
	_TCHAR *buf,
	_TCHAR *str				// String to convert.
	)
{
*buf = '\0';
if (!str || !*str)
	return buf;
_TCHAR *ptr;
ptr = buf;
while (*str)
	{
	switch (*str)
		{
		case '"':
		case '\\':
			*ptr++ = '\\';			// Escape.
			*ptr++ = *str++;
			break;
		default:
			*ptr++ = *str++;
			break;
		}
	}
*ptr = '\0';			// Terminate string.
return buf;
}

/**************************************************
*						NUM_TO_HEX
* FUN:	num_to_hex
* SUBJ:	Convert number to hex string.
* CR:		01/17/06 AM.
* RET:	Buffer holding hex string.
*
**************************************************/

LIBPRIM_API _TCHAR *num_to_hex(
	long num,
	_TCHAR *buf				// Buffer for result.
	)
{
if (!buf || num < 0)
	return 0;

_TCHAR tmp[32];
_TCHAR *ptr = tmp;
int dig;
int count = 0;
while (num > 0)
	{
	++count;
	dig = (num % 16);
	switch (dig)
		{
		case 15: *ptr++ = 'F';	break;
		case 14:	*ptr++ = 'E';	break;
		case 13:	*ptr++ = 'D';	break;
		case 12:	*ptr++ = 'C';	break;
		case 11:	*ptr++ = 'B';	break;
		case 10:	*ptr++ = 'A';	break;
		case  9:	*ptr++ = '9';	break;
		case  8:	*ptr++ = '8';	break;
		case  7:	*ptr++ = '7';	break;
		case  6: *ptr++ = '6';	break;
		case  5: *ptr++ = '5';	break;
		case  4: *ptr++ = '4';	break;
		case  3:	*ptr++ = '3';	break;
		case  2: *ptr++ = '2';	break;
		case  1:	*ptr++ = '1';	break;
		case  0:
		default:
					*ptr++ = '0';	break;
		}
	num /= 16; // Or shift right 4 places.
	}
// Reverse string.
_TCHAR *ptr2 = buf;
while (count > 0)
	{
	--count;
	*ptr2++ = tmp[count];
	}
*ptr2 = '\0';
return buf;
}
