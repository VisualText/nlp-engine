/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STR.CPP
* FILE:	lite\str.cpp
* CR:		11/04/98 AM.
* SUBJ:	String and char functions.
*
*******************************************************************************/

#include "StdAfx.h"
//#include <tchar.h>  // 09/26/01 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "lite/iarg.h"	// 05/14/03 AM.
#include "std.h"			// 07/18/00 AM.
#include "inline.h"		// 05/19/99 AM.
#include "chars.h"		// 07/18/00 AM.
#include "node.h"	// 07/07/03 AM.
#include "parse.h"		// 05/14/03 AM.

#include "str.h"

#include "words/words.h"		// 10/16/00 AM.
#include "words/wordarrs.h"	// 10/16/00 AM.

#ifdef UNICODE
//#include "unicode/utypes.h"	// 03/03/05 AM.
//#include "unicode/uchar.h"
#include "utypes.h"	// 03/03/05 AM.
#include "uchar.h"
#endif

#ifndef BOM_STUFF_
#define BOM_STUFF_
char BOM_UTF8[] = "\xEF\xBB\xBF"; // UTF-8
#endif


/********************************************
* FN:		ALPHABETIC
* CR:		09/22/99 AM.
* SUBJ:	Handling some extended ASCII chars as alphabetic.
* RET:	True if alphabetic else false.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
* OPT:	Could optimize by using bits in the char, or by setting up a
*			switch statement.
********************************************/

#ifdef OLD_
bool alphabetic(_TCHAR ch)
{
#ifndef LINUX
_TUCHAR xx = (_TUCHAR)ch;
if (xx >= 0 && xx <= 128)
	{
	if (_istalpha((_TUCHAR)ch))
		return true;
	}
else if (

		|| (xx >= 192 && xx <= 214)
//		|| (xx >= 216 && xx <= 221)										// 12/16/01 AM.
//		|| (xx >= 224 && xx <= 246)										// 12/16/01 AM.
//		|| (xx >= 248 && xx <= 253)										// 12/16/01 AM.
		|| (xx >= 216 && xx <= 246)										// 12/16/01 AM.
		|| (xx >= 248 && xx <= 255)										// 12/16/01 AM.
		)
	return true;
return false;
#else
return _istalpha((_TUCHAR)ch);	// 03/06/00 AM.
#endif
}
#endif

// Reinstating this.  Extended ascii in non-unicode seems to need this, for now.  // 03/027/05 AM.
// New implementation.														// 12/16/01 AM.
LITE_API bool alphabetic(_TCHAR ch)
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

#ifdef _NEW_DONT_WORK_FOR_EXTENDED_ASCII
LITE_API bool alphabetic(_TCHAR ch)	// 03/03/05 AM.
{
#ifndef UNICODE
return _istalpha((_TUCHAR)ch) ? true : false;
#else
return u_isalpha(ch) ? true : false;
#endif
}
#endif

/********************************************
* FN:		IS_UPPER
* CR:		12/16/01 AM.
* SUBJ:	Handling some extended ASCII chars as uppercase.
* RET:	True if uppercase else false.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
*			Don't know what to do with German double-S char.  Can't find
*			uppercase variant.
********************************************/

LITE_API bool is_upper(_TCHAR ch)
{
#ifdef _OLD_
switch((_TUCHAR)ch)
	{
	case  65: case  66: case  67: case  68: case  69: case  70: case  71:
   case  72: case  73: case  74: case  75: case  76: case  77: case  78:
	case  79: case  80: case  81: case  82: case  83: case  84: case  85:
	case  86: case  87: case  88: case  89: case  90:

	case 138: case 140:

	case 159:

	case 192: case 193: case 194: case 195: case 196: case 197: case 198:
	case 199: case 200: case 201: case 202: case 203: case 204: case 205:
	case 206: case 207: case 208: case 209: case 210: case 211: case 212:
	case 213: case 214:

	case 216: case 217: case 218: case 219: case 220: case 221: case 222:

	case 223:	// German double-S.  Uppercase is ?
		return true;
	default:
		return false;
	}
return false;
#endif

#ifndef UNICODE
return _istupper((_TUCHAR)ch) ? true : false;
#else
return u_isupper(ch) ? true : false;
#endif
}


/********************************************
* FN:		IS_LOWER
* CR:		12/16/01 AM.
* SUBJ:	Handling some extended ASCII chars as lowercase.
* RET:	True if lowercase else false.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
*			Don't know what to do with German double-S char.  Can't find
*			uppercase variant.
********************************************/

LITE_API bool is_lower(_TCHAR ch)
{
#ifdef _OLD_
switch((_TUCHAR)ch)
	{
	case  97: case  98: case  99: case 100: case 101: case 102: case 103:
	case 104: case 105: case 106: case 107: case 108: case 109: case 110:
	case 111: case 112: case 113: case 114: case 115: case 116: case 117:
	case 118: case 119: case 120: case 121: case 122:

	case 154: case 156:

	case 223:	// German double-S.  Uppercase is ?

	case 224: case 225: case 226: case 227: case 228: case 229: case 230:
	case 231: case 232: case 233: case 234: case 235: case 236: case 237:
	case 238: case 239: case 240: case 241: case 242: case 243: case 244:
	case 245: case 246:

	case 248: case 249: case 250: case 251: case 252: case 253: case 254:

	case 255:
		return true;
	default:
		return false;
	}
return false;
#endif
#ifdef UNICODE
return u_islower(ch) ? true : false;
#else
return _istlower((_TUCHAR)ch) ? true : false;
#endif
}

/********************************************
* FN:		TO_UPPER
* CR:		12/16/01 AM.
* SUBJ:	Handling some extended ASCII chars as uppercase.
* RET:	True if uppercase else false.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
*			Don't know what to do with German double-S char.  Can't find
*			uppercase variant.
********************************************/

LITE_API _TCHAR to_upper(_TCHAR ch)
{
#ifdef _OLD_
switch((_TUCHAR)ch)
	{
	case  97: return 65;
	case  98: return 66;
	case  99: return 67;
	case 100: return 68;
	case 101: return 69;
	case 102: return 70;
	case 103: return 71;
	case 104: return 72;
	case 105: return 73;
	case 106: return 74;
	case 107: return 75;
	case 108: return 76;
	case 109: return 77;
	case 110: return 78;
	case 111: return 79;
	case 112: return 80;
	case 113: return 81;
	case 114: return 82;
	case 115: return 83;
	case 116: return 84;
	case 117: return 85;
	case 118: return 86;
	case 119: return 87;
	case 120: return 88;
	case 121: return 89;
	case 122: return 90;

	case 154: return (_TCHAR)138;
	case 156: return (_TCHAR)140;	// Lower S, OE.

	case 223: return (_TCHAR)223; // German double-S.  Uppercase is ?

	case 224: return (_TCHAR)192;
	case 225: return (_TCHAR)193;
	case 226: return (_TCHAR)194;
	case 227: return (_TCHAR)195;
	case 228: return (_TCHAR)196;
	case 229: return (_TCHAR)197;
	case 230: return (_TCHAR)198;
	case 231: return (_TCHAR)199;
	case 232: return (_TCHAR)200;
	case 233: return (_TCHAR)201;
	case 234: return (_TCHAR)202;
	case 235: return (_TCHAR)203;
	case 236: return (_TCHAR)204;
	case 237: return (_TCHAR)205;
	case 238: return (_TCHAR)206;
	case 239: return (_TCHAR)207;
	case 240: return (_TCHAR)208;
	case 241: return (_TCHAR)209;
	case 242: return (_TCHAR)210;
	case 243: return (_TCHAR)211;
	case 244: return (_TCHAR)212;
	case 245: return (_TCHAR)213;
	case 246: return (_TCHAR)214;

	case 248: return (_TCHAR)216;
	case 249: return (_TCHAR)217;
	case 250: return (_TCHAR)218;
	case 251: return (_TCHAR)219;
	case 252: return (_TCHAR)220;
	case 253: return (_TCHAR)221;
	case 254: return (_TCHAR)222;

	case 255: return (_TCHAR)159; // Lower Y umlaut.

	default:  return ch;
	}
return 0;
#endif

#ifdef UNICODE
return u_toupper(ch);
#else
return _totupper((_TUCHAR)ch);	// 03/06/00 AM.
#endif
}


/********************************************
* FN:		TO_LOWER
* CR:		12/16/01 AM.
* SUBJ:	Handling some extended ASCII chars as uppercase.
* RET:	True if uppercase else false.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
*			Don't know what to do with German double-S char.  Can't find
*			uppercase variant.
********************************************/

LITE_API _TCHAR to_lower(_TCHAR ch)
{
#ifdef _OLD_
switch((_TUCHAR)ch)
	{
	case  65: return 97;
	case  66: return 98;
	case  67: return 99;
	case  68: return 100;
	case  69: return 101;
	case  70: return 102;
	case  71: return 103;
   case  72: return 104;
	case  73: return 105;
	case  74: return 106;
	case  75: return 107;
	case  76: return 108;
	case  77: return 109;
	case  78: return 110;
	case  79: return 111;
	case  80: return 112;
	case  81: return 113;
	case  82: return 114;
	case  83: return 115;
	case  84: return 116;
	case  85: return 117;
	case  86: return 118;
	case  87: return 119;
	case  88: return 120;
	case  89: return 121;
	case  90: return 122;

	case 138: return (_TCHAR)154;
	case 140: return (_TCHAR)156;	// Upper S, OE.

	case 159: return (_TCHAR)255;	// Upper Y umlaut.

	case 192: return (_TCHAR)224;
	case 193: return (_TCHAR)225;
	case 194: return (_TCHAR)226;
	case 195: return (_TCHAR)227;
	case 196: return (_TCHAR)228;
	case 197: return (_TCHAR)229;
	case 198: return (_TCHAR)230;
	case 199: return (_TCHAR)231;
	case 200: return (_TCHAR)232;
	case 201: return (_TCHAR)233;
	case 202: return (_TCHAR)234;
	case 203: return (_TCHAR)235;
	case 204: return (_TCHAR)236;
	case 205: return (_TCHAR)237;
	case 206: return (_TCHAR)238;
	case 207: return (_TCHAR)239;
	case 208: return (_TCHAR)240;
	case 209: return (_TCHAR)241;
	case 210: return (_TCHAR)242;
	case 211: return (_TCHAR)243;
	case 212: return (_TCHAR)244;
	case 213: return (_TCHAR)245;
	case 214: return (_TCHAR)246;

	case 216: return (_TCHAR)248;
	case 217: return (_TCHAR)249;
	case 218: return (_TCHAR)250;
	case 219: return (_TCHAR)251;
	case 220: return (_TCHAR)252;
	case 221: return (_TCHAR)253;
	case 222: return (_TCHAR)254;

	case 223: return (_TCHAR)223;	// German double-S.  Uppercase is ?

	default: return ch;
	}
return 0;
#endif

#ifdef UNICODE
return u_tolower(ch);
#else
return _totlower((_TUCHAR)ch);
#endif
}


/********************************************
* FN:		CHAR_TO_DIGIT
* CR:		11/04/98 AM.
* SUBJ:	Convert a char to its integer.
* RET:	numeric.
* NOTE:	Does no error checking.
*			Provided as a hook for unicode, etc.
********************************************/

inline int char_to_digit(_TCHAR ch)
{
switch (ch)
	{
	case '0':	return 0;
	case '1':	return 1;
	case '2':	return 2;
	case '3':	return 3;
	case '4':	return 4;
	case '5':	return 5;
	case '6':	return 6;
	case '7':	return 7;
	case '8':	return 8;
	case '9':	return 9;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[char_to_digit: Given nondigit char.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return 0;
	}
}

/********************************************
* FN:		STR_TO_INT
* CR:		11/04/98 AM.
* SUBJ:	Convert a string to an integer.
* RET:	True if ok, else false.
* NOTE:	atoi doesn't error check.
*			All strings are unsigned integer.
********************************************/

bool str_to_int(_TCHAR *str, /*UP*/ int &num)
{
if (empty(str))
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));
num = 0;
UChar c = iter.first();
do {
	if (!unicu::isDigit(c))
		return false;
	num = 10 * num + char_to_digit(c);
	c = iter.next();

} while (c != icu::CharacterIterator::DONE); 
/*
while (*str)
	{
	if (!_istdigit((_TUCHAR)*str))
		return false;
	num = 10 * num + char_to_digit(*str++);
	}
*/
return true;
}

/********************************************
* FN:		STR_TO_LONG
* CR:		11/29/98 AM.
* SUBJ:	Convert a string to a long.
* RET:	True if ok, else false.
* NOTE:	atoi doesn't error check.
*			All strings are unsigned.
********************************************/

bool str_to_long(_TCHAR *str, /*UP*/ long &num)
{
if (empty(str))
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));
num = 0;
UChar c = iter.first();
do {
	if (!unicu::isDigit(c))
		return false;
	num = 10 * num + char_to_digit(c);
	c = iter.next();

} while (c != icu::CharacterIterator::DONE); 
return true;
}

bool str_to_long(_TCHAR *str, long long &num)
{
if (empty(str))
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));
num = 0;
UChar c = iter.first();
do {
	if (!unicu::isDigit(c))
		return false;
	num = 10 * num + char_to_digit(c);
	c = iter.next();

} while (c != icu::CharacterIterator::DONE); 
return true;
}


/********************************************
* FN:		ALL_UPPERCASE
* CR:		11/30/98 AM.
* SUBJ:	Check if all alphabetics are uppercase.
* RET:	True if ok, else false.
********************************************/

bool all_uppercase(_TCHAR *str)
{
icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));
UChar c = iter.first();
do {
	if (!unicu::isUpper(c))
		return false;
	c = iter.next();

} while (c != icu::CharacterIterator::DONE); 
/*
_TCHAR ch;
if (empty(str))
	return true;
while ((ch = *str++))
	{
	// 09/22/99 AM. Accepting accented ASCII chars as both uppercase and
	// lowercase!
	if (alphabetic((_TUCHAR)ch)									// 12/16/01 AM.
		 && !icu::isUpper((_TUCHAR)ch))								// 12/16/01 AM.
		return false;
	}
*/
return true;
}


/********************************************
* FN:		ALL_LOWERCASE
* CR:		01/03/00 AM.
* SUBJ:	Check if all alphabetics are lowercase.
* RET:	True if ok, else false.
********************************************/

bool all_lowercase(_TCHAR *str)
{
icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));
UChar c = iter.first();
do {
	if (!u_islower(c))
		return false;
	c = iter.next();

} while (c != icu::CharacterIterator::DONE);
/*
_TCHAR ch;
if (empty(str))
	return true;
while ((ch = *str++))
	{
	// 09/22/99 AM. Accepting accented ASCII chars as both uppercase and
	// lowercase!
	if (alphabetic((_TUCHAR)ch)									// 12/16/01 AM.
	 && !is_lower((_TUCHAR)ch))									// 12/16/01 AM.
		return false;
	}
*/
return true;
}


/********************************************
* FN:		MIXCAP
* CR:		01/15/00 AM.
* SUBJ:	Check for mixed caps and lowercase.
* RET:	True if ok, else false.
* NOTE:	Defining mixed cap as follows:
*			If first letter is cap, then must have one cap and one lowercase
*			beyond first letter.  Else, must have one cap and one lowercase
*			in the whole word. (at least).
*
********************************************/

bool mixcap(_TCHAR *str)
{
if (empty(str))
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));

bool lower = false;
bool upper = false;
UChar c = iter.first();

// If first char is uppercase, ignore it.
if (unicu::isAlphabetic(c) && unicu::isUpper(c))
	{
	if ((c = iter.next()) == icu::CharacterIterator::DONE)
		return false;
	upper = true;
	}

do {
	if (unicu::isAlphabetic(c))									// 12/16/01 AM.
		{
		if (unicu::isUpper(c))									// 12/16/01 AM.
			{
			if (lower)
				return true;
			upper = true;
			}
		else
			{
			if (upper)
				return true;
			lower = true;
			}
		}
	c = iter.next();

} while (c != icu::CharacterIterator::DONE);
/*
_TCHAR ch;
if (empty(str))
	return false;

bool lower = false;
bool upper = false;

// If first char is uppercase, ignore it.
if (alphabetic((_TUCHAR)*str)										// 12/16/10 AM.
	&& icu::isUpper((_TUCHAR)*str))									// 12/16/01 AM.
	{
	if (!*++str)
		return false;
	}

for (ch = *str; ch; ch = *++str)
	{
	if (alphabetic((_TUCHAR)ch))									// 12/16/01 AM.
		{
		if (icu::isUpper((_TUCHAR)ch))									// 12/16/01 AM.
			{
			if (lower)
				return true;
			upper = true;
			}
		else
			{
			if (upper)
				return true;
			lower = true;
			}
		}
	}
*/
return false;
}


/********************************************
* FN:		STR_TO_LOWER
* CR:		12/12/98 AM.
* SUBJ:	Convert a string to lowercase
* RET:	buf - filled with new string.
* NOTE:	atoi doesn't error check.
*			All strings are unsigned.
********************************************/

_TCHAR *str_to_lower(_TCHAR *str, _TCHAR *buf)
{
icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
ustr.toLower();
std::string converted;
ustr.toUTF8String(converted);
strcpy(buf,&converted[0]);
return buf;
}


/********************************************
* FN:		STR_TO_LOWER (in-place variant)
* CR:		12/15/14 AM.
* SUBJ:	Convert a string to lowercase, in place.
* RET:	buf - filled with new string.
* NOTE:	atoi doesn't error check.
*			All strings are unsigned.
********************************************/

_TCHAR *str_to_lower(_TCHAR *str)
{
if (empty(str))
	return 0;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
ustr.toLower();
std::string converted;
ustr.toUTF8String(converted);
strcpy(str,&converted[0]);
return str;
/*
_TCHAR *buf = str;
if (empty(str))
	return 0;
while ((*str = to_lower(*str++)));
return buf;
*/
}


/********************************************
* FN:		STR_TO_UPPER
* CR:		11/04/99 AM.
* SUBJ:	Convert a string to uppercase
* RET:	True if ok, else false.
* NOTE:	All strings are unsigned.
********************************************/

_TCHAR *str_to_upper(_TCHAR *str, _TCHAR *buf)
{
icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
ustr.toUpper();
std::string converted;
ustr.toUTF8String(converted);
strcpy(buf,&converted[0]);
return buf;
}


/********************************************
* FN:		STR_TO_UPPER (in-place variant)
* CR:		12/15/14 AM.
* SUBJ:	Convert a string to uppercase, in place.
* RET:	buf - filled with new string.
* NOTE:	All strings are unsigned.
********************************************/

_TCHAR *str_to_upper(_TCHAR *str)
{
if (empty(str))
	return 0;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
ustr.toUpper();
std::string converted;
ustr.toUTF8String(converted);
strcpy(str,&converted[0]);
return str;
/*
_TCHAR *buf = str;
while ((*str = to_upper(*str++)));
return buf;
*/
}


/********************************************
* FN:		ZAP_FINAL_WHITE
* CR:		02/12/99 AM.
* SUBJ:	Null out the final whitespace in given string.
* RET:	True if ok, else false.
* WARN:	MODIFIES GIVEN STRING.
********************************************/

bool zap_final_white(_TCHAR *str)
{
if (!str || !*str)
	return true;

_TCHAR *ptr = 0;
while (*str)
	{
	switch (*str)
		{
		case ' ': case '\r': case '\n': case '\t':
			if (!ptr)
				ptr = str;			// Record last whitespace group.
			break;
		default:
			ptr = 0;				// Current char is real.
			break;
		}
	++str;
	}
if (ptr)
	*ptr = '\0';			// Zap start of terminating whitespace.
return true;
}


/********************************************
* FN:		STR_EQUAL
* CR:		06/07/99 AM.
* SUBJ:	Compare strings.
* RET:	True if same, else false.
* WARN:	TRUE IF BOTH NULL!
********************************************/

bool str_equal(_TCHAR *str1, _TCHAR *str2)
{
if (!str1 && !str2)
	return true;
if (!str1 || !str2)
	return false;
if (!*str1 || !*str2)
	return true;
if (!*str1 || !*str2)
	return false;
if (_tcscmp(str1, str2))
	return false;
return true;
}


/********************************************
* FN:		LONG_TO_STR
* CR:		11/03/99 AM.
* SUBJ:	Convert a string to long.
* RET:	True if ok, else false.
* NOTE:	atoi doesn't error check.
*			All strings are unsigned.
********************************************/

bool long_to_str(long num, /*UP*/ _TCHAR *buf)
{
buf[0] = '\0';
#ifndef LINUX
_ltot(num, buf, 10);
#endif
return true;
}


/********************************************
* FN:		STR_TO_CSTR
* CR:		12/15/99 AM.
* SUBJ:	Convert a string to a C-string.
* RET:	True if ok, else false.
* NOTE:
********************************************/

bool str_to_cstr(
	_TCHAR *str,			// String to convert.
	/*UP*/
	_TCHAR *buf			// Supplied buffer to fill.
	)
{
buf[0] = '\0';
if (!str || !*str)
	return true;
while (*str)
	{
	switch (*str)
		{
		case '\\':
			switch (*(str+1))		// Look at next char to see what to do.
				{
				case 'a':	*buf++ = '\a';	str += 2;	break;
				case 'b':	*buf++ = '\b';	str += 2;	break;
				case 'n':	*buf++ = '\n';	str += 2;	break;
				case 'r':	*buf++ = '\r';	str += 2;	break;
				case 'f':	*buf++ = '\f';	str += 2;	break;
				case 't':	*buf++ = '\t';	str += 2;	break;
				case 'v':	*buf++ = '\v';	str += 2;	break;
				case '\\':	*buf++ = '\\';	str += 2;	break;
				case '"':	*buf++ = '\"'; str += 2;	break; // 06/05/00 AM.
				default:
					*buf++ = *str++;	// Just a plain backslash.
					break;
				}
			break;
		default:
			*buf++ = *str++;
			break;
		}
	}
*buf = '\0';		// Terminate new string.
return true;
}


/********************************************
* FN:		STRCPY_E
* CR:		12/30/99 AM.
* SUBJ:	String copy specialized for repeated copying.
* RET:	True if ok, else false.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
********************************************/

inline void strcpy_e(
	_TCHAR* &ptr,			// Pointer to buffer for copying into.
							// Points to last FILLED char.
	_TCHAR *str			// Terminated string to copy.
	)
{
while ((*++ptr = *str++))
	;
--ptr;	// Go back to last filled char.
}


/********************************************
* FN:		STRCPY_EE
* CR:		09/27/00 AM.
* SUBJ:	String copy specialized for repeated copying.
* RET:	True if ok, else false.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
*			EE == "End,pointing to Empty".
********************************************/

// Moved to inline.h	// 10/09/03 AM.

/********************************************
* FN:		CLEAN_STR
* CR:		01/25/00 AM.
* SUBJ:	Clean up whitespace in string.
* RET:	True if ok, else false.
* NOTE:	Get rid of leading and terminal whitespace.
*			Convert newlines, tabs to space.  Get rid of extra spaces
*			between stuff.
********************************************/

bool clean_str(
	_TCHAR *str,			// String to clean.
	/*UP*/
	_TCHAR *buf			// Supplied buffer to fill.
	)
{
/*
#ifdef UNICODE
	return false;
#endif
*/

*buf = '\0';
if (!str || !*str)
	return false;

// Skip leading whitespace.
if (_istspace((_TUCHAR)*str))
	{
	while (_istspace((_TUCHAR)*++str))
		;
	if (!*str)	// String was all whitespace.
		{
		*buf++ = ' ';	// Convert to one space.
		*buf = '\0';
		return true;
		}
	}

for (;;)
	{
	while (*str && !_istspace((_TUCHAR)*str))	// Copy nonwhite chars.
		*buf++ = *str++;
	while (_istspace((_TUCHAR)*str++))				// Skip whitespace region.
		;
	--str;									// Back up to last tested char.
	if (!*str)								// At end of line.
		break;								// Ignore trailing white, if any.
	*buf++ = ' ';							// Convert whites to single space.
	}

*buf = '\0';
return true;
}


/********************************************
* FN:		FIND_STR
* CR:		05/17/00 AM.
* SUBJ:	Find a string in an array of strings.
* RET:	True if found, else false.
* ASS:	Null terminated array.
********************************************/

bool find_str(
	_TCHAR *str,				// String to find.
	const _TCHAR **arr			// Array to look in.
	)
{
if (!str || !*str || !arr || !*arr)
	return false;
while (*arr)
	{
	if (!_tcscmp(str, *arr))
		return true;
	++arr;
	}
return false;
}


/********************************************
* FN:		FIND_STR_NOCASE
* CR:		06/07/00 AM.
* SUBJ:	Find a string in an array of strings.
* RET:	True if found, else false.
* ASS:	Null terminated array.
* NOTE:	Case insensitive.  For use by compiled runtime match fns, etc.
********************************************/

bool find_str_nocase(
	_TCHAR *str,				// String to find.
	const _TCHAR **arr			// Array to look in.
	)
{
if (!str || !*str || !arr || !*arr)
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
icu::UCharCharacterIterator iter(strBuf, unicu::strLen(strBuf));

UErrorCode success = U_ZERO_ERROR;
icu::Collator *collator = icu::Collator::createInstance("UTF-8", success);
collator->setStrength(icu::Collator::PRIMARY);  // This is for case insensitive

while (*arr)
	{
	icu::UnicodeString astr = icu::UnicodeString::fromUTF8(icu::StringPiece(*arr));
	if (collator->compare(astr,ustr) == icu::Collator::EComparisonResult::EQUAL)
		return true;
	++arr;
	}
return false;
}


/********************************************
* FN:		STRLEN_EQ
* CR:		05/24/00 AM.
* SUBJ:	See if string length is same as given num.
* RET:	True if matching length.
* ASS:	Null terminated array.
********************************************/

bool strlen_eq(
	_TCHAR *str,				// String.
	long len					// Length to compare.
	)
{
if (!str)
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
return (ustr.length() == len);
/*
for (; (len > 0) && *str; --len, ++str)
	;
return (len == 0 && !*str);
}
*/
}

bool strlen_eq(long len, _TCHAR *str)
{
return strlen_eq(str, len);
}

bool strlen_eq(_TCHAR *str1, _TCHAR *str2)
{
if (!str1 || !str2)		// Requiring that a string be given.
	return false;

icu::UnicodeString ustr1 = icu::UnicodeString::fromUTF8(icu::StringPiece(str1));
icu::UnicodeString ustr2 = icu::UnicodeString::fromUTF8(icu::StringPiece(str2));
return (ustr1.length() == ustr2.length());
/*

while (*str1 && *str2)
	{
	++str1;
	++str2;
	}
if (*str1 || *str2)		// Exactly one is still not empty.
	return false;
return true;
*/
}


/********************************************
* FN:		BINARY_SEARCH
* CR:		06/09/00 AM.
* SUBJ:	Binary search into sorted array of strings.
* RET:	Index into array. 0 means not found.
* ASS:	Null terminated array, 0th elt also null.
* NOTE:	Doing case-sensitive compare.
* OPT:	(For special, could slough comparison of "_x"
*			starter, but not worrying about that for now.)
********************************************/

long binary_search(
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long len					// Active length of array (excluding 0th elt).
	)
{
if (!str || !*str || !arr || len <= 0)
	return 0;
return binary_search(str, arr, 1, len);
}

// Variant.  Workhorse for binary string search.
long binary_search(
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long ii,					// "Left" end of array.
	long jj					// "Right" end of array.
	)
{
long mid;
int res;
if (!str || !*str || !arr || ii < 0 || jj < 0)
	return 0;
while (ii <= jj)
	{
	// Get middle elt and compare it.
	// If less, look in left half.
	if ((res = _tcscmp(str, arr[mid = (ii+jj)/2])) < 0)
		jj = mid - 1;
	else if (res > 0)				// If greater, look in right half.
		ii = mid + 1;
	else // if (res == 0)		// If match, done.
		return mid;					// FOUND IT.
	}
return 0;							// Failed.
}



/********************************************
* FN:		READCONVERT
* CR:		07/18/00 AM.
* SUBJ:	Read file into buffer, converting crud chars.
* RET:	UP - alloc'ed buffer for file. NULL TERMINATED!
*			UP - length of converted stuff.
* ALLOC:	CALLER MUST FREE BUFFER.
* NOTE:	\r - remove.
*			\r with no \n - insert \n.
*			maxstr - fold lines longer than some limit.
*			\0 - convert to back quote.
* WARN:	FILE LENGTH IS INSUFFICIENT.  CONVERSION MAY ADD CHARS VIA
*			INSERTING NEWLINES WHEN FOLDING LONG LINES.  So, adding some
*			cushion.
*			BECAUSE OF MICROSOFT PADDING OF \r\n, DECREMENT LINE NUMBER
*			BASED ON NUMBER OF NEWLINES!.
*			07/24/00 AM. Using binary mode solves some of the above problems.
*			Now converting all \r\n to \n, all other \r to \n -- at least,
*			as long as we're folding lines.
********************************************/

bool readConvert(
	_TCHAR *fname,
	_TCHAR* &buf,		/*UP*/
	long &len,		/*UP*/
	long maxline	// default==MAXSTR
	)
{
len = 0;
buf = 0;
if (!fname || !*fname || (maxline <= 1))
	return false;

long flen = file_size(fname);		// Get file size from system.
if (flen <= 0)
	return false;
long cushion = (flen / (maxline-1));	// Max possible newlines to be inserted.
long biglen = flen + cushion;

// ALLOCATE BUFFER HERE.
buf = Chars::create(biglen+1);		// Extra one for termination.

// Read file into buffer, converting directly.
#ifdef LINUX
std::_t_ifstream inFile(fname, std::ios::in);
#else
std::_t_ifstream inFile(TCHAR2CA(fname), std::ios::in							// Upgrade.	// 01/24/01 AM.
								| std::ios_base::binary);			// FIX.		// 02/01/01 AM.
#endif
if (!inFile)
	{
//	*gerr << "[Could not open file '" << fname << "']." << std::endl;
	len = 0;
	delete buf;
	buf = 0;
	return false;
	}
//inFile.setmode(filebuf::binary);						// Upgrade.	// 01/24/01 AM.

_TCHAR ch;
_TCHAR *ptr=buf;
int linelen = 0;
bool cr=false;		// If previous char was a carriage return.
long counter=0;

// Check if first characters are BOM.	// FIX.	// 03/25/08 AM.

for (counter = 0; counter < flen; ++counter)
	{
	ch = inFile.get();
	if (inFile.eof())															// 12/16/01 AM.
		ch = '\0';			// Replace EOF char.							// 12/16/01 AM.
	if (cr)					// If previous was a carriage return.
		{
		switch (ch)
			{
			case '\r':
				*ptr++ = '\n';						// Convert prev cr.	// 07/24/00 AM.
				linelen = 0;						// Reset.				// 07/24/00 AM.
				// cr still true.
				break;
			case '\n':
				// Slough the previous carriage return.
				*ptr++ = '\n';
				linelen = 0;	// Reset;
				cr = false;
				break;
			case '\0':		// NEW EOF HANDLER.							// 12/16/01 AM.
//			case EOF:												// FIX.	// 08/07/00 AM.
				*ptr++ = '\n';						// Convert prev cr.	// 07/24/00 AM.
				*ptr++ = '`';						// REPLACE \0 WITH BACK QUOTE.
				cr = false;
				linelen = 1;						// Reset.				// 07/24/00 AM.
				break;
			default:
				*ptr++ = '\n';						// Convert prev cr.	// 07/24/00 AM.
				*ptr++ = ch;
				cr = false;
				linelen = 1;						// Reset.				// 07/24/00 AM.
				break;
			}
		}
	else						// Previous was not a carriage return.
		{
		switch (ch)
			{
			case '\r':
				cr = true;
				break;
			case '\n':
				*ptr++ = '\n';
				linelen = 0;	// Reset;
				break;
			case '\0':
				if (++linelen >= maxline)
					{
					*ptr++ = '\n';
					linelen = 1;
					}
				*ptr++ = '`';					// REPLACE WITH BACK QUOTE.
				break;
			default:
				if ((++linelen >= maxline)
					 && _istspace((_TUCHAR)ch))	// Don't break within a word.	// 11/14/00 AM.
					{
					*ptr++ = '\n';
					linelen = 1;
					}
				*ptr++ = ch;
				break;
			}
		}
	}	// End of for loop

*ptr = '\0';			// Terminate buffer.
len = ((unsigned long)ptr - (unsigned long)buf) / sizeof(_TCHAR);

if (len > biglen)		// Sanity check.
	{
//	*gerr << "[Converted file is too long.]" << std::endl;
	len = 0;
	delete buf;
	buf = 0;
	return false;
	}

return true;
}

/********************************************
* FN:		READCONVERTU
* CR:		03/03/05 AM.
* SUBJ:	Read encoded file into buffer.
* NOTE:	File encoded to capture Unicode characters.
*			BOM protocol...
********************************************/

bool readConvertU(
	_TCHAR *fname,
	_TCHAR* &buf,		/*UP*/
	long &len,		/*UP*/
	long maxline	// default==MAXSTR
	)
{
if (!readConvert(fname,buf,len,maxline))
	return false;
if (len <= 2)
	return true;

_TCHAR *tmpbuf = buf;	// Convert from this.

char bom[4];
#ifdef LINUX
memset(bom,0,sizeof(bom));
#else
ZeroMemory(bom,sizeof(bom));
#endif
for (int n = 0; n < 3; n++)
	bom[n] = (char)(tmpbuf[n]);

if (memcmp(bom, BOM_UTF8, 3))	// If there's no BOM header.
	return true;	// No conversion to Unicode needed.

// If there's a Unicode file in non-Unicode VisualText.	// 03/24/08 AM.
#ifndef UNICODE
std::_t_strstream gerrStr;
gerrStr
	<< _T("[Warning: Unicode file in non-Unicode VisualText.]")
	<< std::ends;
errOut(&gerrStr,false);
return true;
#endif

// Else convert to Unicode for internal use.
UTL_GetUnicodeFromMBCS(
	(wchar_t* &)buf,
	&(tmpbuf[3]), len, CP_UTF8);

if (tmpbuf)
	delete tmpbuf;
return true;
}

/********************************************
* FN:		UTL_GetUnicodeFromMBCS
* CR:		03/03/05 AM.
* SUBJ:	Read encoded file into buffer.
* NOTE:	File encoded to capture Unicode characters.
*			BOM protocol...
********************************************/

bool UTL_GetUnicodeFromMBCS(
	wchar_t* &pszUnicodeString,
	LPCTSTR lpszMBCS,
	long &len,
	UINT nCodePage
	)
{
#ifndef LINUX
int nChars = lstrlen(lpszMBCS) + 1;
int nBufferSize = nChars * sizeof(wchar_t);
pszUnicodeString = new wchar_t[nChars];
char*	pszMultiByteString = new char[nChars * 2]; // why *2 ? keeps app from crashing when delete [] called below

int n =  0;
for (n = 0; n < nChars; n++)
	pszMultiByteString[n] = (char)lpszMBCS[n];

pszMultiByteString[n] = 0;
int nCharsWritten = 0;
if (pszUnicodeString && pszMultiByteString)
	{
	if (nCodePage == -1)
		nCodePage = CP_UTF8;
	try
		{
#ifdef LINUX
		memset((void*)pszUnicodeString,0,nBufferSize);
#else
		ZeroMemory((void*)pszUnicodeString, nBufferSize);
#endif
		nCharsWritten = MultiByteToWideChar(nCodePage,0,pszMultiByteString,-1,
																	pszUnicodeString,nBufferSize);
		}
	catch(...)
		{
//		TRACE(_T("Controlled exception in MultiByteToWideChar!\n"));
		}
	}

if (pszMultiByteString)
	delete [] pszMultiByteString;
len = nCharsWritten;	// Pass this up.
#endif
return true;
}
/********************************************
* FN:		WRITECONVERT
* CR:		07/18/00 AM.
* SUBJ:	Write file into buffer, converting crud chars.
* NOTE:	\r - remove.
*			\r with no \n - insert \n.
*			maxstr - fold lines longer than some limit.
*			\0 - convert to back quote.
********************************************/

bool writeConvert(
	_TCHAR *fname,
	_TCHAR *buf,
	long len,			// default==0, means stop at first null char.
	long maxline		// default==MAXSTR.
	)
{
*gerr << _T("[writeConvert: UNIMPLEMENTED.]") << std::endl;
return true;
}


#ifdef UNICODE
#define ASCII_7 0 // 0 - 0x7f
#define ASCII_EX 1 // 0 - 0xff
BOOL UTL_IsASCII(LPCWSTR lpwStr, int typeASCII)
{
	int maxVal = ASCII_7 == typeASCII ? 0x7f : 0xff;
	int len = lstrlen(lpwStr);
	for (int n = 0; n < len; n++)
		if ((int)lpwStr[n] > maxVal)
			return false;
	return true;
}

/********************************************
* FN:		UTL_GetMBCSFromUnicodeString
* CR:		03/06/05 AM.
* SUBJ:	Convert Unicode to MBCS.
********************************************/

bool UTL_GetMBCSFromUnicodeString(
	LPCWSTR lpwUnicode,
	UINT nCodePage,
	LPCTSTR* &mbcsstr
	)
{
	int nChars = wcslen(lpwUnicode) + 1;
	int nMultiByteBufferSize = nChars * 6;  // possible 6 bytes max per character for UTF-8
	wchar_t* pszUnicodeString = new wchar_t[nChars];
	char *pszMultiByteString = new char[nMultiByteBufferSize];

	lstrcpy(pszUnicodeString, lpwUnicode);

	int nCharsWritten = 0;
	if (pszUnicodeString && pszMultiByteString)
		{
#ifdef LINUX
		memset(pszMultiByteString,0,nMultiByteBufferSize);
#else
		ZeroMemory(pszMultiByteString, nMultiByteBufferSize);
#endif
		if (nCodePage == -1)
			{
//			BOOL bASCII_Ex = UTL_IsASCII(lpwUnicode, ASCII_EX);
//			nCodePage = !bASCII_Ex ? CP_UTF8 : CP_ACP;
			nCodePage = CP_UTF8;
			}

		try
			{
			nCharsWritten = WideCharToMultiByte(nCodePage, 0,
				pszUnicodeString,-1, pszMultiByteString, nMultiByteBufferSize, NULL, NULL);
			}
		catch(...)
			{
//			TRACE(_T("Controlled exception in WideCharToMultiByte!\n"));
			}
		}
//	CStringA strMBCS = CStringA((CP_UTF8 == nCodePage ? BOM_UTF8 : "")) + pszMultiByteString;
	if (pszUnicodeString)
		delete [] pszUnicodeString;
//	if (pszMultiByteString)
//		delete [] pszMultiByteString;
//	return strMBCS;
mbcsstr = (LPCTSTR *)pszMultiByteString;
return true;
}
#endif
/********************************************
* FN:		STR_TO_TITLE
* CR:		08/21/00 Dd.
* SUBJ:	Convert a string to title case
* RET:	True if ok, else false.
* NOTE:	All strings are unsigned.
*			08/31/00 AM. Installed Dave's fixes.
********************************************/

_TCHAR *str_to_title(_TCHAR *str, _TCHAR *buf)
{
buf[0] = '\0';
if (empty(str))
	return 0;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(str));
UErrorCode status = U_ZERO_ERROR;
ustr.toTitle(NULL, icu::Locale::getUS());
std::string converted;
ustr.toUTF8String(converted);
strcpy(buf,&converted[0]);
return buf;

/*
_TCHAR *ptr;
ptr = &(buf[0]);
_TCHAR lastChar = ' ';

while (*str)
  {
  if (_istspace((_TUCHAR)lastChar))
    {
    lastChar = *str;
    *ptr++ = to_upper(*str++);											// 12/16/01 AM.
    }
  else
    {
    lastChar = *str;
    *ptr++ = to_lower(*str++);											// 12/16/01 AM.
    }
  }

*ptr = '\0';
*/

return buf;
}


/********************************************
* FN:	   STR_WRAP
* CR:	   09/22/00 Dd.
* SUBJ: Convert a string to a wrapped string of given size
* RET:  True if ok, else false.
* NOTE: All strings are unsigned.
********************************************/

_TCHAR *str_wrap(_TCHAR *str, long wrapSize, _TCHAR *buf)
{
buf[0] = '\0';
if (empty(str))
	return 0;
_TCHAR *ptr;
ptr = &(buf[0]);
_TCHAR *lastSpace = NULL;
_TCHAR *lastNewSpace = NULL;
long count=0;

while (*str)
	{
	if (_istspace((_TUCHAR)*str))
		{
		lastSpace = str;
		lastNewSpace = ptr;
		if (*str == '\n')
			count = 0;
		}

	if (count >= wrapSize)
		{
		if (lastSpace)
			{
			str = lastSpace;
			ptr = lastNewSpace;
			}

		*ptr++ = '\n';
		++str;
		count = 0;
		lastSpace = NULL;
		lastNewSpace = NULL;
		}
	else
		*ptr++ = *str++;

	++count;
	}

*ptr = '\0';

return buf;
}


/********************************************
* FN:	   STR_SUBST
* CR:	   09/22/00 Dd.
* SUBJ: Substitute a substring with a new string
* RET:  True if ok, else false.
* NOTE: All strings are unsigned.
*	11/03/08 AM.	Handle EMPTY new string.
********************************************/

_TCHAR *str_subst(_TCHAR *str, _TCHAR *oldSubStr, _TCHAR *newSubStr, _TCHAR *buf)
{
buf[0] = '\0';
if (empty(str)
	|| empty(oldSubStr))	// 11/03/08 AM.
	return 0;
_TCHAR *ptr;
ptr = &(buf[0]);
_TCHAR *oldSubStrPtr;
_TCHAR *newSubStrPtr = 0;	// 11/03/08 AM.
oldSubStrPtr = &(oldSubStr[0]);

if (newSubStr && *newSubStr)	// 11/03/08 AM.
	newSubStrPtr = &(newSubStr[0]);
_TCHAR *lastMatchStart = NULL;
_TCHAR *lastPtrMatchStart = NULL;
bool matching = false;

while (*str)
	{
    if (*str == *oldSubStrPtr)
		{
		if (!matching)
	    {
	    lastMatchStart = str;
	    lastPtrMatchStart = ptr;
	    }
	matching = true;
	++oldSubStrPtr;
	++str;
	}
    else if (matching)
	{
	matching = false;
	if (newSubStr && *newSubStr)	// 11/03/08 AM.
		newSubStrPtr = &(newSubStr[0]);

	if (*oldSubStrPtr)
	    {
	    str = lastMatchStart;
	    ptr = lastPtrMatchStart;
			*ptr++ = *str++;
	    }
		else if (newSubStr && *newSubStr)	// 11/03/08 AM.
			{
			while (newSubStrPtr && *newSubStrPtr)	// 07/10/01 DD.	// 11/03/08 AM.
					*ptr++ = *newSubStrPtr++;
			}
	oldSubStrPtr = &(oldSubStr[0]);
	}
    else
	*ptr++ = *str++;
}


if (matching)
	{
	if (!*oldSubStrPtr)
		{
		if (newSubStr && *newSubStr)	// 11/03/08 AM.
			newSubStrPtr = &(newSubStr[0]);
		while (newSubStrPtr && *newSubStrPtr)						// FIX.	// 10/16/00 DD.
				*ptr++ = *newSubStrPtr++;
		}
	else
		{
		str = lastMatchStart;
		ptr = lastPtrMatchStart;
		while (*str)
				*ptr++ = *str++;
		}
	}

*ptr = '\0';

return buf;
}


/********************************************
* FN:	   STR_PIECE
* CR:	   11/18/02 AM.
* SUBJ: Get a substring of a string.
* RET:  True if ok, else false.
* NOTE: Rewriting Dave's hunk of junk.
********************************************/

bool str_piece(
	_TCHAR *str,
	long start,
	long end,
	long siz,	// Size of buf.
	/*UP*/
	_TCHAR *buf
	)
{
if (!str || !*str)
	return false;

icu::StringPiece sp(str);
const char *spd = sp.data();
int32_t length = sp.length();
long len = end-start+1;

if (end < start
 || start < 0
 || len >= siz )
	return false;

UChar32 c = 1;
int32_t s = 0;
int32_t sLast = 0;
int32_t e = 0;
int32_t i = 0;

while (c && i<=start) {
	sLast = s;
	U8_NEXT(spd, s, length, c);
	i++;
}
e = s;
s = sLast;
i = 1;
while (c && i<len) {
	U8_NEXT(spd, e, length, c);
	i++;
}
len = e-s;
_tcsnccpy(buf,&(str[s]),len);
buf[len] = '\0';
return true;
}



/********************************************
* FN:	   STR_UNIQUE_CHARS
* CR:	   10/23/22 Dd.
* SUBJ: Returns the unique characters in a string.
* RET:  True if ok, else false.
* NOTE:
********************************************/

bool str_unique_chars(
	_TCHAR *str,
	/*UP*/
	_TCHAR *buf
	)
{
if (!str || !*str)
	return false;

icu::StringPiece sp(str);
const char *spd = sp.data();
int32_t length = sp.length();
int32_t unilen = u_strlen(str);

UChar32 c = 1;
int32_t s = 0;
int32_t start = 0;
int32_t i = 0;
int32_t len = 0;
int32_t strStart = 0;
bool found = false;

while (c && i<unilen) {
	start = s;
	U8_NEXT(spd, s, length, c);

	found = false;
	if (len) {
		UChar32 cb = 1;
		int32_t sb = 0;
		icu::StringPiece spb(buf);
		const char *spdb = spb.data();
		int32_t lengthb = spb.length();
		int32_t unilenb = u_strlen(buf);

		for (int j=0; j<unilenb; j++) {
			U8_NEXT(spdb, sb, lengthb, cb);
			if (cb == c) {
				found = true;
				break;
			}
		}		
	}

	if (!found) {
		int32_t letlen = s-start;

		for (int k=0; k<letlen; k++) {
			buf[len++] = str[strStart++];
		}
		buf[len] = '\0';
	}
	strStart = s;
	i++;
}
buf[len] = '\0';
return true;
}


/********************************************
* FN:	   STR_SPELL_COMPARE
* CR:	   09/26/00 DD.
* SUBJ: Compares two strings and returns a spelling comparison weight
* RET:  Comparison weight:  0 if equal, > 0 if less equal, -1 for failure.
* NOTE: All strings are unsigned.
*			09/28/00 AM. Cleaning this up some.
********************************************/

int str_spell_compare(_TCHAR *str, _TCHAR *candidateStr)
{
if (empty(str) || empty(candidateStr))
	return -1;

_TCHAR *ptr = str;
_TCHAR *cPtr = candidateStr;
_TCHAR *nextPtr = ptr;
_TCHAR *nextCPtr = cPtr;
_TCHAR *prevPtr = NULL;
_TCHAR *prevCPtr = NULL;
int weight = 0;

while (*ptr && *cPtr)
	{
	nextPtr = ptr + 1;			// 09/28/00 AM.
	nextCPtr = cPtr + 1;			// 09/28/00 AM.

	// IF EQUAL, CONTINUE
	if (*ptr == *cPtr)
		{
		prevPtr = ptr++;
		prevCPtr = cPtr++;
		}

	// TRANSPOSED LETTERS
	else if (*ptr == *nextCPtr && *cPtr == *nextPtr)
		{
		prevPtr = ++ptr;
		++ptr;
		prevCPtr = ++cPtr;
		++cPtr;
		++weight;
		}

	// LETTER MISSING
	else if (*ptr == *nextCPtr && _tcsclen(ptr) < _tcsclen(cPtr))
		{
		prevCPtr = cPtr++;
		weight += 2;
		}

	// LETTER REPEATING (KEYBOARD BOUNCE)
	else if (prevPtr && prevCPtr && *ptr == *prevPtr && *ptr == *prevCPtr)
		{
		prevPtr = ptr++;
		++weight;
		}

	// EXTRA LETTER
	else if (nextPtr && *nextPtr == *cPtr && _tcsclen(ptr) > _tcsclen(cPtr))
		{
		prevPtr = ptr++;
		weight += 2;
		}

	// WRONG LETTER
	else
		// if (*nextPtr == *nextCPtr) // Next may be wrong also.	// 09/28/00 AM.
		{
		prevPtr = ptr++;
		prevCPtr = cPtr++;
		weight += 2;
		}
	}

// ADD WEIGHT FOR WRONG LETTERS AT THE END
weight += abs((long)(_tcsclen(ptr) - _tcsclen(cPtr))) * 2;						// 10/02/00 AM.

// IF THE NUMBER OF MISMATCHES IS GREATER THAN
// THE LENGTH OF THE STRING, THEN COMPLETE FAILURE

if ((unsigned)weight >= _tcsclen(str) * 2)								// 10/02/00 AM.
	weight = -1;

return weight;
}


/********************************************
* FN:		STR_SPELL CANDIDATE
* CR:		09/26/00 DD.
* SUBJ:	Choose the best candidate from a string list
* RET:	True if ok, else false.
* NOTE:	The string list are strings separated by spaces.
********************************************/

void str_spell_candidate(_TCHAR *str, _TCHAR *strList, _TCHAR *buf)
{
buf[0] = '\0';
if (empty(str) || empty(strList))
     return;

// Rewriting this monstrosity.											// 10/16/00 AM.
_TCHAR *save = strList;		// SAVE FOR DEBUGGING.					// 10/17/00 AM.
_TCHAR buff[SPELL_BUF_LEN];	// Buffer for current candidate.
int weight = -1;				// Weight of current candidate.
long len = 0;					// Length of current candidate.
_TCHAR *candStr=0;				// Start of current candidate in strList.

int lowestWeight = 10000;	// Weight of best candidate so far.
long lowestLen = 0;			// Length of best candidate so far.	// 10/16/00 AM.
long slen = _tcsclen(str);	// Length of original word. (ie, optimal).

for (;;)					// While traversing candidates.				// 10/16/00 AM.
	{
	// Slough any blanks at start.										// 10/16/00 AM.
	while (*strList && _istspace((_TUCHAR)*strList))
		++strList;
	if (!*strList)		// Reached end of candidate string.
		break;

	// Now we're at the start of a candidate.
	candStr = strList;

	// Let's get to the end of the candidate.
	while (*strList && !_istspace((_TUCHAR)*strList))
		++strList;

	// Now we're one past the end of a candidate.
	len = strList - candStr;
	_tcsnccpy(buff,candStr,len);
	buff[len] = '\0';		// Terminate.
	weight = str_spell_compare(str,buff);	// Compare latest to original.

	// Compare latest to best so far.
	if (spell_new_low(weight,len,lowestWeight,lowestLen,slen))
		{
		lowestWeight = weight;
		lowestLen = len;
		_tcscpy(buf,buff);		// New best candidate.
		}
	}

if (lowestWeight > 5)													// 10/02/00 AM.
	*buf = '\0';			// Reject the best candidate.
}


/********************************************
* FN:		SPELL_NEW_LOW
* CR:		10/16/00 AM.
* SUBJ:	Check for new lowest weight candidate.
* RET:	True if new lowest, else false.
********************************************/

inline bool spell_new_low(
	int weight,				// Current candidate weight
	long len,				// Current candidate length
	int low_weight,		// Best so far
	long low_len,			// Best so far
	long best_len			// Best possible length.
	)
{
return ((weight >= 0)
	&& (weight < low_weight))
	|| (weight == low_weight
		 && (abs(len - best_len) < abs(low_len - best_len)) )
	;
}


/********************************************
* FN:		BINARY_SPELL
* CR:		09/27/00 AM.
* SUBJ:	Binary search into sorted array of strings.
* RET:	Index into array. 0 means not found.
*			UP: pos - if found, position that string was found at.
				if NOT found, position before the string would have occurred.
* ASS:	Null terminated array, 0th elt also null.
* NOTE:	Doing case-sensitive compare.
********************************************/

long binary_spell(
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long len,					// Active length of array (excluding 0th elt).
	/*UP*/
	long &pos				// Position at or one prior to string location.
	)
{
if (!str || !*str || !arr || len <= 0)
	return 0;
pos = 0;		// Initial value is one before the active array.
return binary_spell(str, arr, 1, len,/*UP*/pos);
}

// Variant.  Workhorse for binary string search.
long binary_spell(
	_TCHAR *str,				// String to search.
	_TCHAR *arr[],			// Array to search.
	long ii,					// "Left" end of array.
	long jj,					// "Right" end of array.
	/*UP*/
	long &pos				// Position at or one before sought string.
	)
{
long mid;
int res;
if (!str || !*str || !arr || ii < 0 || jj < 0)
	return 0;

while (ii <= jj)
	{
	// Get middle elt and compare it.
	// If less, look in left half.
	if ((res = _tcscmp(str, arr[mid = (ii+jj)/2])) < 0)
		jj = mid - 1;
	else if (res > 0)				// If greater, look in right half.
		{
		ii = mid + 1;
		pos = ii - 1;		// one BEFORE the region.
		}
	else // if (res == 0)		// If match, done.
		return pos = mid;					// FOUND IT.
	}
return 0;							// Failed.
}


/********************************************
* FN:		SPELL_CANDIDATES
* CR:		09/27/00 AM.
* SUBJ:	Getting spelling correction candidates from dictionary arrays.
* RET:	strlist - string of candidates separated by blank space.
* ASS:	Null terminated array, 0th elt also null.
* NOTE:	Doing case-sensitive compare.
*			VARIANT.  Now the big array is split by length in words.dll.
*			This fn uses the arrays of that library implicitly.
********************************************/

_TCHAR *spell_candidates(
	_TCHAR *word,				// The word to be checked.
	_TCHAR *buf,				// Buffer for placing candidates.
	long maxbuf				// Maximum length of buffer.
	)
{
*buf = '\0';
_TCHAR *start = buf;
long siz = maxbuf;
if (!word || !*word)
	return 0;
long len = _tcsclen(word);
if (len > MAXWORDLENGTH)										// FIX	// 11/13/01 AM.
	return 0;														// FIX	// 11/13/01 AM.

// For the moment, check only precise matches to word length.
// (Then do +1 and -1, if that works.)
_TCHAR *end;			// End of active buffer.
spell_candidates(word, word_arrays[len],word_lengths[len],start,siz,
																				/*UP*/ end);
siz -= (1 + end - start);
if (siz < 2)
	return buf;
if (len > 3)								// Try the shorter words.
	{
	*end++ = ' ';
	start = end;
	spell_candidates(word, word_arrays[len-1],word_lengths[len-1],start,siz,
																				/*UP*/ end);
	}
siz -= (1 + end - start);
if (siz < 2)
	return buf;
if (len < MAXWORDLENGTH)				// Try the longer words.
	{
	*end++ = ' ';
	start = end;
	spell_candidates(word, word_arrays[len+1],word_lengths[len+1],start,siz,
																				/*UP*/ end);
	}
siz -= (1 + end - start);	// JUST FOR DEBUGGING.			// 10/17/00 AM.
return buf;
}


/********************************************
* FN:		SPELL_CANDIDATES
* CR:		09/27/00 AM.
* SUBJ:	Getting spelling correction candidates from a sorted array.
* RET:	strlist - string of candidates separated by blank space.
* ASS:	Null terminated array, 0th elt also null.
* NOTE:	Doing case-sensitive compare.
********************************************/

_TCHAR *spell_candidates(
	_TCHAR *word,				// The word to be checked.
	_TCHAR *arr[],			// Alpha-sorted, null terminated array of strings.
	long len,				// Length of array (excluding null terminator string
								// and excluding 0th elt).
	_TCHAR *buf,				// Buffer for placing candidates.
	long maxbuf,				// Maximum length of buffer.
	/*DU*/
	_TCHAR* &ptr				// UP: End of active buffer.  Points to terminator.
	)
{
ptr = buf;					// Set to first location.
*ptr = '\0';
if (!word || !*word)
	return 0;

// First do a binary search in the array, to find closest position based
// on where word should appear in array.
long pos = -1;
long index = binary_spell(word, arr, len, /*UP*/ pos);
if (index)	// Found word!
	{
	_tcscpy(buf, word);
	return buf;
	}
if (pos < 0)
	return 0;

// Get string at pos.  Compare initial match with word.
// Get length of matching chars in prefix.  That's a minimal requirement.
int usedlen = 0;		// Space used in buffer.
int minlet = 2;
int min = prefix_match_len(word, arr[pos]);
if (min < minlet)			// Only first letter matched.  Too much.
	return 0;

// BACKTRACK BY ONE LETTER.											// 09/28/00 AM.
// eg, "recieved" should also try words like "received".		// 09/28/00 AM.
if (min > minlet)															// 09/28/00 AM.
	--min;		// Allow one less than the max.					// 09/28/00 AM.

long wlen = _tcsclen(word);
int fudge = 1;			// Amount we can fudge on word length.
if (wlen >= 10)
	fudge = 2;			// Allow more fudge on long words.

int maxcand = 1000;	// Maximum number of candidates to collect.
int ncands = 0;		// Number of candidates collected.
ptr = buf;				// Slot for placing next candidate.
long ii = pos;			// Moved here.										// 05/17/02 AM.

// See if current pos is a good candidate by length.
int alen = _tcsclen(arr[pos]);
if (abs(wlen - alen) <= fudge)
	{
	// Even here, need to check buffer overflow.			// FIX.	// 05/17/02 AM.
	// Found good candidate.
	if ((usedlen + alen + 1) >= maxbuf)						// FIX.	// 05/17/02 AM.
		{
		std::_t_strstream gerrStr;		// FIX.	// 05/17/02 AM.
		gerrStr
		 << _T("[Benign note: Spell buffer overflow.]")
		 << std::ends;	// FIX.	// 05/17/02 AM.
		errOut(&gerrStr,false);												// FIX.	// 05/17/02 AM.
		goto done;													// FIX.	// 05/17/02 AM.
		}
	usedlen += alen;
	strcpy_ee(ptr, arr[pos]);	// Copy candidate into buffer.
	++ncands;
	}

// Traverse backward AND forward from pos, looking for best candidates.
// If shorter matching prefix, stop traversing in that direction.

// Traverse backward.
for (;;)
	{
	if (--ii <= 0)
		break;
	if (prefix_match_len(word, arr[ii]) >= min)
		{
		alen = _tcsclen(arr[ii]);
		if (abs(wlen - alen) <= fudge)
			{
			// Found good candidate.
			if ((usedlen + alen + 1) >= maxbuf)
				{
				std::_t_strstream gerrStr;
				gerrStr
					<< _T("[Benign note: Spell buffer overflow.]")
					<< std::ends;
				errOut(&gerrStr,false);
				goto done;
				}
			usedlen += (alen + 1);		// One for the blank space.
			if (ncands)
				strcpy_ee(ptr, _T(" "));		// Blank space separator.
			strcpy_ee(ptr, arr[ii]);
			if (++ncands >= maxcand)	// Reached max candidates.
				goto done;
			}
		}
	else
		break;
	}

// Traverse forward.
ii = pos;
for (;;)
	{
	if (++ii > len)	// Reached end of array.
		break;
	if (prefix_match_len(word, arr[ii]) >= min)
		{
		alen = _tcsclen(arr[ii]);
		if (abs(wlen - alen) <= fudge)
			{
			// Found good candidate.
			if ((usedlen + alen) >= maxbuf)
				{
				std::_t_strstream gerrStr;
				gerrStr
					<< _T("[Benign note: Spell buffer overflow.]")
					<< std::ends;
				errOut(&gerrStr,false);
				goto done;
				}
			usedlen += (alen + 1);		// One for the blank space.
			if (ncands)
				strcpy_ee(ptr, _T(" "));		// Blank space separator.
			strcpy_ee(ptr, arr[ii]);
			if (++ncands >= maxcand)	// Reached max candidates.
				goto done;
			}
		}
	else
		break;
	}

done:
	*ptr = '\0';
	return buf;
}


/********************************************
* FN:		PREFIX_MATCH_LEN
* CR:		09/27/00 AM.
* SUBJ:	Get length of matching prefix in two strings.
* RET:	len - length of matching prefix.
* NOTE:	CASE-SENSITIVE.
********************************************/

long prefix_match_len(_TCHAR *str1, _TCHAR *str2)
{
if (!str1 || !str2)
	return 0;
long count = 0;
while (*str1 && *str2 && (*str1++ == *str2++))
	++count;
return count;
}


/********************************************
* FN:		SET_ARR_LEN
* CR:		09/27/00 AM.
* SUBJ:	Set length of special arrays of strings.
* NOTE:	0th element and last element of string array are null.
*			Length is calculated discarding 0 and last elt.
********************************************/

long set_arr_len(_TCHAR *arr[])
{
long ii = 0;
_TCHAR **ptr = arr;
// Note: slough 0th element of array.
while (*++ptr)
	++ii;
return ii;
}


/********************************************
* FN:		RIGHTJUSTIFY
* CR:		10/01/00 AM.
* SUBJ:	Right justify number in given field size.
* RET:	buf - of given field size, with number right justified.
*				If number is larger than size, then wider string is returned.
* ALLOC:	USER MUST FREE THE BUFFER.
********************************************/

_TCHAR *rightjustify(
	long num,
	long fieldsize
	)
{
if (fieldsize < 1)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rightjustify: Field too small.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (num < 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rightjustify: Not handling negative nums for now.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Compute width of number.
long tmp = num;
int width = 1;
while (tmp >= 10)
	{
	tmp /= 10;
	++width;
	}

_TCHAR *buf;
if (width >= fieldsize)		// Can't justify
	{
	buf = Chars::create(width+1);
	_stprintf(buf, _T("%ld"), num);	// Convert to string.
	}
else								// Can justify.
	{
	buf = Chars::create(fieldsize+1);
	tmp = fieldsize - width;	// Amount to justify.
	_TCHAR *ptr = buf;
	while (tmp--)
		*ptr++ = ' ';				// Pad with blanks. (could parameterize).
	_stprintf(ptr, _T("%ld"), num);	// Throw in number.
	}
return buf;
}


/********************************************
* FN:		LEFTJUSTIFY
* CR:		10/02/00 AM.
* SUBJ:	Right justify STRING in given field size.
* RET:	buf - of given field size, with number right justified.
*				If number is larger than size, then wider string is returned.
* ALLOC:	USER MUST FREE THE BUFFER.
********************************************/

_TCHAR *leftjustify(
	_TCHAR *str,
	long fieldsize
	)
{
if (fieldsize < 1)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[leftjustify: Field too small.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[leftjustify: Warning: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	// Todo: Could return a buffer just padded with spaces!
	return 0;
	}

// Compute width of string.
long tmp=0;
long width = _tcsclen(str);

_TCHAR *buf;
if (width >= fieldsize)		// Can't justify
	{
	buf = Chars::create(width+1);
	_tcscpy(buf, str);
	}
else								// Can justify.
	{
	buf = Chars::create(fieldsize+1);
	tmp = fieldsize - width;	// Amount to justify.
	_TCHAR *ptr = buf;
	strcpy_ee(ptr, str);
	while (tmp--)
		*ptr++ = ' ';				// Pad with blanks. (could parameterize).
	*ptr = '\0';					// Terminate.
	}
return buf;
}

/********************************************
* FN:		STR_TRIM
* CR:		11/10/00 Dd.
* SUBJ:	Trim start and end of string of whitespace
* RET:	True if ok, else false.
* NOTE:	Get rid of leading and terminal whitespace.
*			Convert newlines, tabs to space.  Get rid of extra spaces
*			between stuff.
********************************************/

bool str_trim(
	_TCHAR *str,		      // String to clean.
	/*UP*/
	_TCHAR *buf		       // Supplied buffer to fill.
	)
{
*buf = '\0';
if (!str || !*str)
	return false;

// Skip leading whitespace.
if (_istspace((_TUCHAR)*str))
	{
	while (_istspace((_TUCHAR)*++str))
		;
	if (!*str)      // String was all whitespace.
		{
		*buf++ = ' ';   // Convert to one space.
		*buf = '\0';
		return true;
		}
	}

_tcscpy(buf,str);

int len = _tcsclen(buf)-1;
for (int i=0; i<len; i++)
	*buf++;

// Skip trailing whitespace
while (_istspace((_TUCHAR)*buf))
	*buf--;

*buf++;
*buf = '\0';
return true;
}


/********************************************
* FN:		STR_STARTS_WITH
* CR:		08/05/21 Dd.
* SUBJ:		Checks to see if the string starts with the substring
* RET:		True if the string starts with substring, else false.
* NOTE:		All strings are unsigned.
********************************************/

bool str_starts_with(_TCHAR *str, _TCHAR *startstr)
{

if (empty(str) || empty(startstr))
	return false;
long slen = _tcsclen(str);
long elen = _tcsclen(startstr);
if (slen < elen)
	return false;
_TCHAR *ptr;
_TCHAR *endPtr;
_TCHAR *startPtr;
ptr = &(str[0]);
startPtr = &(startstr[0]);
endPtr = &(startstr[elen-1]);
bool matched = false;

for (;;)
	{
	if (*ptr == *startPtr)
		{
		if (startPtr == endPtr)
			return true;
		++startPtr;
		++ptr;
		}
	else
		return false;
	}
return false;
}


/********************************************
* FN:		STR_ENDS_WITH
* CR:		11/17/00 Dd.
* SUBJ:	Checks to see if the string std::ends in the substring
* RET:	True if the string std::ends with substring, else false.
* NOTE:	All strings are unsigned.
********************************************/

bool str_ends_with(_TCHAR *str, _TCHAR *ending)
{

if (empty(str) || empty(ending))
	return false;
long slen = _tcsclen(str);										// OPT.	// 11/20/00 AM.
long elen = _tcsclen(ending);									// OPT.	// 11/20/00 AM.
if (slen < elen)
	return false;
_TCHAR *ptr;
_TCHAR *endPtr;
ptr = &(str[slen-1]);
endPtr = &(ending[elen-1]);
bool matched = false;

//while (*ptr)						// Unneeded check.					// 11/20/00 AM.
for (;;)																			// 11/20/00 AM.
	{
	if (*ptr == *endPtr)
		{
		if (endPtr == ending)									// FIX.	// 11/20/00 AM.
			return true;			// MATCHED.								// 11/20/00 AM.
		--endPtr;																// 11/20/00 AM.
		--ptr;																	// 11/20/00 AM.
		}
	else
		return false;
	}
return false;
}


/********************************************
* FN:		STR_ESCAPE
* CR:		07/26/01 AM.
* SUBJ:	Escape a string.
* RET:	True if ok.
* NOTE:	Assumes buffer is big enough.
********************************************/

bool str_escape(_TCHAR *str, _TCHAR *chars, _TCHAR escape, _TCHAR *buf)
{
--buf;	// Set up pointer to buffer.
--str;	// Set up string pointer.
_TCHAR ch;
while ((ch = *++str))
	{
	if (_tcschr(chars, ch))		// If char is to be escaped.
		*++buf = escape;			// Add escape char.
	*++buf = ch;
	}
*++buf = '\0';						// Terminate buffer.
return true;
}


/********************************************
* FN:		STR_UNESCAPE
* CR:		07/26/01 AM.
* SUBJ:	Unescape a string.
* RET:	True if ok.
* NOTE:	Assumes buffer is big enough.
********************************************/

bool str_unescape(_TCHAR *str, _TCHAR *chars, _TCHAR escape, _TCHAR *buf)
{
--buf;	// Set up pointer to buffer.
--str;	// Set up pointer to string.
_TCHAR ch;
while ((ch = *++str))
	{
	if (ch == escape && _tcschr(chars, *(str+1)))
		ch = *++str;		// Slough escape char.
	*++buf = ch;
	}
*++buf = '\0';						// Terminate buffer.
return true;
}


/********************************************
* FN:		CONVERT_TO_FLOAT
* CR:		08/19/01 AM.
* SUBJ:	Convert two strings to a float.
* RET:	True if ok.
*			UP fnum - the computed float number.
* NOTE:	E.g., convert_to_float("123","456") => 123.456
********************************************/

bool convert_to_float(_TCHAR *str1, _TCHAR *str2, /*UP*/ float &fnum)
{
fnum = 0.0;			// Default value.

float tot1 = 0.0;
int dig = 0;
if (str1)
	{
	--str1;				// Set up for loop.
	while (*++str1)
		{
		dig = (int) *str1 - (int) '0';				// ASCII dependency.
		if (dig > (int) 9 || dig < (int) 0)		// Non-digit in string.
			return(false);
		tot1 = tot1 * (float) 10.0 + (float)dig;
		}
	}

float tot2 = 0.0;
float div = 1.0;										// Divisor.
float maxdiv = 1000000000.0;						// Max divisor.
if (str2)
	{
	--str2;		// Set up loop.
	while (*++str2)
		{
		dig = (int) *str2 - (int) '0';			// ASCII dependency.
		if (dig > (int) 9 || dig < (int) 0)		// Non digit in string.
			return(false);
		tot2 = tot2 * (float) 10.0 + (float)dig;

		// If maxed out on accuracy.
		div *= (float) 10.0;
		if (div == maxdiv)
			break;
		}
	}
fnum = tot1 + tot2/div;
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
*			Copied from prim/str/s_to_f.
* ASS:	Assumes ASCII character set.
*
**************************************************/

bool
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


/********************************************
* FN:	   STRSUFFIX
* CR:	   11/23/01 AM.
* SUBJ:	See if reasonable suffix of given word.
* RET:	True if possible suffix, else false.
* FORMS:	strsuffix(str, suffix_str)
* NOTE:	Using words array to study word.  Morphological grounds only.
*			Start of linguistic-oriented work...
********************************************/

bool strsuffix(
	_TCHAR *word,
	_TCHAR *suffix
	)
{

if (!word || !*word || !suffix || !*suffix)
	return false;

long len1 = _tcsclen(word);
long len2 = _tcsclen(suffix);
long len = len1 - len2;
if (len < 2)
	return false;
if (len1 > MAXWORDLENGTH)													// 12/28/01 AM.
	return false;																// 12/28/01 AM.

_TCHAR *lcstr = Chars::create(len1 + 1);
str_to_lower(word, lcstr);
_TCHAR *lcsuff = Chars::create(len2 + 1);
str_to_lower(suffix, lcsuff);


// If given string doesn't end in suffix, then we're done!
// NEED A STRING SPLIT FUNCTION HERE.  Swiss Army Knife.
if (!str_ends_with(lcstr,lcsuff))
	{
	Chars::destroy(lcstr);
	Chars::destroy(lcsuff);
	return false;
	}

// Some special cases.
_TCHAR prevch = lcstr[len1-2];
if (len2 == 1 && *lcsuff == 's')		// Looking for S suffix.
	{
	switch (prevch)
		{
		case 's':
		case 'x':
		case 'z':
			Chars::destroy(lcstr);
			Chars::destroy(lcsuff);
			return false;
		default:
			break;
		}
	}

bool flag = false;
_TCHAR *str = Chars::create(len+1);
_tcsnccpy(str,lcstr,len);
str[len] = '\0';	// Terminate.

long pos;
bool ans = false;
if (binary_spell(str, word_arrays[len],word_lengths[len], /*UP*/pos))
	ans = true;

// Crudely assume that if suffixless is good, then the suffix is real.
flag = ans;

Chars::destroy(lcstr);
Chars::destroy(lcsuff);
Chars::destroy(str);

return flag;
}



/********************************************
* FN:	   STRSTEM
* CR:	   12/28/01 AM.
* SUBJ:	Find stem for given word.
* RET:	Reasonable stem.
* FORMS:	strstem(str)
* NOTE:	Using words array to study word.  Morphological grounds only.
*			Start of linguistic-oriented work...
*			Based on the word dictionary.  Assumes known word.
*			Assumes special cases handled elsewhere.
*			Only stemming verb inflections and noun plurals.
* WARN:	USER MUST FREE BUFFER. (Use Chars::destroy(str) for convenience).
********************************************/

_TCHAR *strstem(
	_TCHAR *word
	)
{
if (!word || !*word)
	return NULL;	// 09/26/19 AM.

long len = _tcsclen(word);

_TCHAR *lcstr = Chars::create(len + 1 + 10);	// Extra space for variants.
str_to_lower(word, lcstr);

if (len > (MAXWORDLENGTH - 3))	// Wiggle room for adding suffixes.
	return lcstr;

if (len < 4)
	return lcstr;

// Word must be in dictionary.
long pos;
bool ans = false;
if (!binary_spell(lcstr, word_arrays[len],word_lengths[len], /*UP*/pos))
	return lcstr;

_TCHAR *ptr = &(lcstr[len-1]);	// Point to last char of word.
long tlen = len;
switch (*ptr)
	{
	case 'd':	// -d, -ed, -ied.
		switch (*(ptr-1))
			{
			case 'e':	// -ed, -ied, etc.

				// Look up word minus ed.
				*--ptr = '\0';
				tlen -= 2;	// minus -ed.
				if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
					{
					// Verify with -ing.
					*ptr   = 'i';
					*++ptr = 'n';
					*++ptr = 'g';
					*++ptr = '\0';
					tlen += 3;	// plus -ing.
					if (binary_spell(lcstr,word_arrays[tlen],word_lengths[tlen],pos))
						{
						*(ptr-3) = '\0';
						return lcstr;
						}
					ptr -= 3;
					*ptr = '\0';
					tlen -= 3;
					}

				switch (*(ptr-1))
					{
					case 'i':
						// Try -ied => -y.
						*--ptr = 'y';
						if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
							return lcstr;
						*ptr++ = 'i';	// Restore.
						break;
					default:
						break;
					}

				// Look up word minus d. "coursed" => "course".
				*ptr = 'e';	// Restore.
				*++ptr = '\0';
				++tlen;
				if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
					return lcstr;

				*ptr = 'd';	// Restore.
				*++ptr = '\0';
				break;
			}
	case 'g':
		if (*(ptr-1) == 'n' && *(ptr-2) == 'i')
			{
			// Try minus -ing.
			*(ptr-2) = '\0';
			tlen -= 3;
			if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
				return lcstr;

			// Try -ing => -e
			*(ptr-2) = 'e';
			*(ptr-1) = '\0';
			tlen += 1;
			if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
				return lcstr;

			*(ptr-2) = 'i';	// Restore.
			*(ptr-1) = 'n';	// Restore.
			}
		break;
	case 's':	// -s -es -ies.
		switch (*(ptr-1))
			{
			case 'e':	// -es, -ies, etc.
				// Look up word minus s.
				*ptr = '\0';
				--tlen;
				if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
					return lcstr;

				// Look up word minus es.
				*--ptr = '\0';
				--tlen;
				if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
					return lcstr;

				switch (*(ptr-1))
					{
					case 'i':
						// Try -ies => -y.
						*--ptr = 'y';
						if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
							return lcstr;
						*ptr++ = 'i';	// Restore.
						break;
					default:
						break;
					}
				*ptr = 'e';		// Restore.
				*++ptr = 's';	// Restore.
				break;
			case 's':	// -ss
				break;
			default:
				// Look up word minus s.
				*ptr = '\0';
				--tlen;
				if (binary_spell(lcstr, word_arrays[tlen],word_lengths[tlen],pos))
					return lcstr;
				*ptr = 's';		// Restore and quit.
				break;
			}
		break;
	default:
		break;
	}

return lcstr;
}


/********************************************
* FN:	   URL_BASE
* CR:	   03/16/03 AM.
* SUBJ:	Normalize URL path.
* RET:	Normalized URL path.
* FORMS:	url_base(str, *UP* buf)
* EX:		http://www.x.com/abc/def.txt => http://www.x.com/abc
********************************************/

bool url_base(
	_TCHAR *str,
	/*UP*/
	_TCHAR *buf
	)
{
*buf = '\0';
if (!str || !*str)
	return false;
_TCHAR *ptr = str;	// Track current char.
_TCHAR *last = 0;	// Track last good endpoint.
long len = 0;		// Normalized path length.

// Skip leading blanks.
while (_istspace(*ptr))
	++ptr;
_TCHAR *first = ptr;	// Track start of normalized path.

// Find http or ftp.
bool flagh = false;	// 08/04/05 AM.
switch (*ptr)
	{
	case 'h':
		if (*++ptr != 't')
			return false;
		flagh = true;	// 08/04/05 AM.
		break;
	case 'f':
		break;
	default:
		return false;
	}
if (*++ptr != 't')
	return false;
if (*++ptr != 'p')
	return false;
if (flagh)					// 08/04/05 AM.
	{
	// Check for https.
	if (*(1+ptr) == 's')	// 08/04/05 AM.
	  ++ptr;					// 08/04/05 AM.
	}
if (*++ptr != ':')
	return false;
if (*++ptr != '/')
	return false;
if (*++ptr != '/')
	return false;

// Find machine name.
bool done = false;
while (!done)
	{
	switch (*++ptr)
		{
		case '/':
			last = (ptr - 1);	// Good path to this point.
			done = true;
			break;
		case ' ':
		case '\t':
		case '\n':
		case '\r':
		case '\0':
			last = (ptr - 1);	// Good path to this point.
			len = (long) (last - first + 1);
			_tcsnccpy(buf,first,len);
			buf[len] = '\0';	// Terminate.
			return true;
			break;
		default:
			break;
		}
	}

// Find last slash preceded by machine name or alpha with no dots.
// Else find last alpha with no dots.
bool dot = false;	// No dot in current stretch of chars.
done = false;
while (!done)
	{
	switch (*++ptr)
		{
		case '/':
			if (!dot)
				last = (ptr - 1);
			else
				done = true;	// Found a file name at end.
				// (Could check to make sure no further junk.)
			break;
		case '.':
			dot = true;
			break;
		case ' ':
		case '\t':
		case '\n':
		case '\r':
		case '\0':
			if (!dot)
				{
				if (*(ptr-1) == '/')
					last = (ptr-2);
				else
					last = (ptr-1);
				}
			done = true;
			break;
		default:
			break;
		}
	}

// Copy to buffer.
len = (long) (last - first + 1);
_tcsnccpy(buf,first,len);
buf[len] = '\0';	// Terminate.

return true;
}


/********************************************
* FN:	   RESOLVE_URL
* CR:	   05/12/03 AM.
* SUBJ:	Resolve embedded URL with base URL.
* RET:	ok - if resolved ok.
*			up buf - resolved URL.
* FORMS:	resolve_url(baseurl_str, embeddedurl_str, *UP* buf_str);
* NOTE:	Reference is http://www.zvon.org/tmRFC/RFC1808/Output/chapter2.html
********************************************/

bool resolve_url(
	_TCHAR *base_str,
	_TCHAR *url_str,
	Parse *parse,
	/*DU*/
	_TCHAR *buf
	)
{
*buf = '\0';
// <scheme>://<net_loc>/<path>;<params>?<query>#<fragment>
if (!base_str || !*base_str || !url_str || !*url_str)
	return false;

if (!base_str || !*base_str)
	{
	if (!strcmp_ni(url_str,_T("http://"),7))
		{
		_tcscpy(buf,url_str);
		return true;
		}
	return false;
	}
if (!url_str || !*url_str)
	{
	if (!strcmp_ni(base_str,_T("http://"),7))
		{
		_tcscpy(buf,url_str);
		return true;
		}
	return false;
	}


// 2.2 If embeddable has a scheme (is absolute), done.
// Only testing http for now.
if (!strcmp_ni(url_str,_T("http://"),7))
	{
	_tcscpy(buf, url_str);
	return true;
	}

// Now unpack both URLs and compare piecy parts.
_TCHAR uscheme[64];
_TCHAR unet_loc[256];
bool uabsflag = false;
long upathlen = 0;
Dlist<Iarg> *upath = 0;
bool uendslash = false;
_TCHAR uparams[256];
_TCHAR uquery[256];
_TCHAR ufragment[256];
if (!unpack_url(url_str,parse,
		/*DU*/
		uscheme,unet_loc,uabsflag,upathlen,upath,uendslash,uparams,uquery,ufragment))
	return false;

_TCHAR bscheme[64];
_TCHAR bnet_loc[256];
bool babsflag = false;
long bpathlen = 0;
Dlist<Iarg> *bpath = 0;
bool bendslash = false;
_TCHAR bparams[256];
_TCHAR bquery[256];
_TCHAR bfragment[256];
if (!unpack_url(base_str,parse,
		/*DU*/
		bscheme,bnet_loc,babsflag,bpathlen,bpath,bendslash,bparams,bquery,bfragment))
	return false;


// 3. If net_loc not empty, combine.
if (*unet_loc)
	{
	combine_url(buf,bscheme,unet_loc,babsflag,upathlen,upath,uendslash,uparams,uquery,
					ufragment);
	if (upath)
		Dlist<Iarg>::DeleteDlistAndData(upath);
	if (bpath)
		Dlist<Iarg>::DeleteDlistAndData(bpath);
	return true;
	}

// 4. If embedded is absolute.
if (uabsflag)
	{
	combine_url(buf,bscheme,bnet_loc,uabsflag,upathlen,upath,uendslash,uparams,uquery,
					ufragment);
	if (upath)
		Dlist<Iarg>::DeleteDlistAndData(upath);
	if (bpath)
		Dlist<Iarg>::DeleteDlistAndData(bpath);
	return true;
	}

// 5. If embedded path is empty.
if (!upathlen)
	{
	if (*uparams)
		combine_url(buf,bscheme,bnet_loc,babsflag,bpathlen,bpath,bendslash,
			uparams,uquery,ufragment);
	else if (*uquery)
		combine_url(buf,bscheme,bnet_loc,babsflag,bpathlen,bpath,bendslash,
			bparams,uquery,ufragment);
	else if (*ufragment)
		combine_url(buf,bscheme,bnet_loc,babsflag,bpathlen,bpath,bendslash,
			bparams,bquery,ufragment);
	if (upath)
		Dlist<Iarg>::DeleteDlistAndData(upath);
	if (bpath)
		Dlist<Iarg>::DeleteDlistAndData(bpath);
	return true;
	}

// 6. Merge paths.


// Remove and delete the filename in base path.
Delt<Iarg> *elt;

if (bpathlen && !bendslash)
	{
	elt = bpath->rpop();
	Delt<Iarg>::DeleteDeltAndData(elt);
	--bpathlen;
	}

if (!bpathlen)
	{
	combine_url(buf,bscheme,bnet_loc,babsflag,upathlen,upath,uendslash,uparams,uquery,
					ufragment);
	if (upath)
		Dlist<Iarg>::DeleteDlistAndData(upath);
	if (bpath)
		Dlist<Iarg>::DeleteDlistAndData(bpath);
	return true;
	}

// Traverse the embedded path and merge with base path.
// Using base path to collect the merged path.
_TCHAR *strx = 0;
Delt<Iarg> *darg = 0;
Iarg *iarg = 0;
bool addeddots = false;
bool newend = false;
while (upathlen > 0)
	{
	--upathlen;
	elt = upath->pop();
	iarg = elt->getData();
	strx = iarg->getStr();
	if (!strcmp_i(strx,_T(".")))
		{
		// Ignore this guy.
		Delt<Iarg>::DeleteDeltAndData(elt);
		newend = true;	// Potential end slash.
		}
	else if (!strcmp_i(strx,_T("..")))
		{
		newend = true;	// Potential end slash.
		// Remove a guy from the end of bpath.
		if (!addeddots && bpathlen)
			{
			--bpathlen;
			darg = bpath->rpop();
			Delt<Iarg>::DeleteDeltAndData(darg);
			Delt<Iarg>::DeleteDeltAndData(elt);
			}
		else
			{
			++bpathlen;
			addeddots = true;
			bpath->rpush(elt);
			}
		}
	else
		{
		++bpathlen;
		bpath->rpush(elt);
		newend = false;
		}
	}

if (newend)
	uendslash = newend;

combine_url(buf,bscheme,bnet_loc,babsflag,bpathlen,bpath,uendslash,uparams,uquery,
					ufragment);
if (upath)
	Dlist<Iarg>::DeleteDlistAndData(upath);
if (bpath)
	Dlist<Iarg>::DeleteDlistAndData(bpath);
return true;
}


/********************************************
* FN:	   UNPACK_URL
* CR:	   05/13/03 AM.
* SUBJ:	Parse a url and return the pieces.
* RET:	ok - if parsed ok.
*			up ...
* NOTE:	Reference is http://www.zvon.org/tmRFC/RFC1808/Output/chapter2.html.
*			Assuming large enough buffers are given, for now.
********************************************/

bool unpack_url(
	_TCHAR *url_str,
	Parse *parse,	// For interning strings, etc.
	/*DU*/
	_TCHAR *scheme,
	_TCHAR *net_loc,
	bool &absflag,
	long &pathlen,
	Dlist<Iarg> *&path,
	bool &endslash,
	_TCHAR *params,
	_TCHAR *query,
	_TCHAR *fragment
	)
{
*scheme = *net_loc = *params = *query = *fragment = '\0';
absflag = endslash = false;
pathlen = 0;
path = 0;
Iarg *iarg = 0;
// <scheme>://<net_loc>/<path>;<params>?<query>#<fragment>

_TCHAR *ptr = url_str;
_TCHAR *mark = ptr;	// Mark the start of something.
long len = 0;
bool done = false;

if (!*ptr)
	return false;

// scheme.
if (!strcmp_ni(ptr,_T("http://"),7))
	{
	_tcscpy(scheme,_T("http"));
	ptr += 7;	// At char following double slash.
	absflag = true;
	}
else if (!strcmp_ni(ptr,_T("https://"),8))	// 08/04/05 AM.
	{
	_tcscpy(scheme,_T("https"));
	ptr += 8;	// At char following double slash.
	absflag = true;
	}
else if (!strcmp_ni(ptr,_T("ftp://"),6))	// 08/04/05 AM.
	{
	_tcscpy(scheme,_T("ftp"));
	ptr += 6;	// At char following double slash.
	absflag = true;
	}
// Should check for unknown schemes.  // 08/04/05 AM.

// if scheme, get net_loc.
if (*scheme)
	{
	mark = ptr;
	done = false;
	while (!done)
		{
		if (!*ptr)
			break;
		switch (*ptr)
			{
			case '/':
				// End of net_loc.
				len = (long) (ptr - mark);
				_tcsnccpy(net_loc,mark,len);
				net_loc[len] = '\0';	// Terminate.
				done = true;
				break;
			case ';':
				// End of net_loc with params.
				len = (long) (ptr - mark);
				_tcsnccpy(net_loc,mark,len);
				net_loc[len] = '\0';	// Terminate.
				++ptr;	// Slough the ; char.
				goto lparams;
				break;
			case '?':
				// End of net_loc with query.
				len = (long) (ptr - mark);
				_tcsnccpy(net_loc,mark,len);
				net_loc[len] = '\0';	// Terminate.
				++ptr;	// Slough the ? char.
				goto lquery;
				break;
			case '#':
				// End of net_loc with fragment.
				len = (long) (ptr - mark);
				_tcsnccpy(net_loc,mark,len);
				net_loc[len] = '\0';	// Terminate.
				++ptr;	// Slough the # char.
				goto lfragment;
				break;
			default:
				break;
			}
		++ptr;
		if (!done	// FIX.	// 07/22/03 AM.
		 && !*ptr)
			{
			_tcscpy(net_loc,mark);
			return true;
			}
		}
	}
else	// No scheme and therefore no net_loc.
	{
	if (*ptr == '/')
		{
		absflag = true;
		++ptr;	// Slough the slash.
		}
	}

// Handle the path here.
// Either with schema after net_loc, or starting with slash or none.
if (!*ptr)
	return true;
mark = ptr;
path = new Dlist<Iarg>();
_TCHAR *strx;
for(;;)
	{
	switch (*ptr)
		{
		case '\0':
			// Add path.  Done.
			if (!*mark || *mark == '/')
				return true;
			// Will intern strings, I guess...
			len = (long) (ptr - mark);
			parse->internStr(mark,len,/*UP*/strx);
			iarg = new Iarg(strx);
			path->rpush(iarg);
			++pathlen;
			return true;	// Done with string.
			break;
		case '/':
			endslash = true;	// For now.
			// Watch out for slash-slash.
			if (*mark == '/')
				{
				mark = ptr + 1;
				break;
				}
			len = (long) (ptr - mark);
			parse->internStr(mark,len,/*UP*/strx);
			iarg = new Iarg(strx);
			path->rpush(iarg);
			++pathlen;
			mark = ptr + 1;
			break;
		case ';':
			if (*mark == '/' || *mark == ';')
				{
				mark = ++ptr;
				goto lparams;
				}
			len = (long) (ptr - mark);
			parse->internStr(mark,len,/*UP*/strx);
			iarg = new Iarg(strx);
			path->rpush(iarg);
			++pathlen;
			mark = ++ptr;
			goto lparams;
			break;
		case '?':
			if (*mark == '/' || *mark == '?' || *mark == ';')
				{
				mark = ++ptr;
				goto lquery;
				}
			len = (long) (ptr - mark);
			parse->internStr(mark,len,/*UP*/strx);
			iarg = new Iarg(strx);
			path->rpush(iarg);
			++pathlen;
			mark = ++ptr;
			goto lquery;
			break;
		case '#':
			if (*mark == '/' || *mark == '?' || *mark == ';' || *mark == '#')
				{
				mark = ++ptr;
				goto lfragment;
				}
			len = (long) (ptr - mark);
			parse->internStr(mark,len,/*UP*/strx);
			iarg = new Iarg(strx);
			path->rpush(iarg);
			++pathlen;
			mark = ++ptr;
			goto lfragment;
			break;
		default:
			endslash = false;	// For now.
			break;
		}
	++ptr;
	}

lparams:
	mark = ptr;
	while (*ptr && *ptr != '?' && *ptr != '#')
		++ptr;
	if (*ptr == '?')
		{
		len = (long) (ptr - mark);
		_tcsnccpy(params,mark,len);
		params[len] = '\0';
		++ptr;	// Slough the ? char.
		goto lquery;
		}
	else if (*ptr == '#')
		{
		len = (long) (ptr - mark);
		_tcsnccpy(params,mark,len);
		params[len] = '\0';
		++ptr;	// Slough the # char.
		goto lfragment;
		}
	else
		{
		_tcscpy(params,mark);
		return true;
		}

lquery:
	mark = ptr;
	while (*ptr && *ptr != '#')
		++ptr;
	if (*ptr == '#')
		{
		len = (long) (ptr - mark);
		_tcsnccpy(query,mark,len);
		query[len] = '\0';
		++ptr;	// Slough the # char.
		goto lfragment;
		}
	else
		{
		_tcscpy(query,mark);
		return true;
		}

lfragment:
	_tcscpy(fragment,ptr);
	return true;	// Done.

return true;
}


/********************************************
* FN:	   COMBINE_URL
* CR:	   05/14/03 AM.
* SUBJ:	Build a URL from components.
* RET:	UP buf - buffer given for filling with URL.
* NOTE:	Reference is http://www.zvon.org/tmRFC/RFC1808/Output/chapter2.html.
*			Assuming large enough buffers are given, for now.
********************************************/

bool combine_url(
	_TCHAR *buf,
	_TCHAR *scheme,
	_TCHAR *net_loc,
	bool absflag,
	long pathlen,
	Dlist<Iarg> *path,
	bool endslash,
	_TCHAR *params,
	_TCHAR *query,
	_TCHAR *fragment
	)
{
buf[0] = '\0';

_TCHAR pathbuf[1024];
pathbuf[0] = '\0';
_TCHAR *ptr = pathbuf;
Delt<Iarg> *darg;
Iarg *iarg;
_TCHAR *strx = 0;
if (path && (darg = path->getFirst()) )
	{
	iarg = darg->getData();
	strcpy_ee(ptr,iarg->getStr());
	for (darg = darg->Right();darg; darg = darg->Right())
		{
		iarg = darg->getData();
		strx = iarg->getStr();
		if (*strx)
			{
			strcpy_ee(ptr,_T("/"));	// Slash as path separator.
			strcpy_ee(ptr,strx);
			}
		}
	}

ptr = buf;

if (*scheme)
	{
	strcpy_ee(ptr,scheme);
	strcpy_ee(ptr,_T(":"));
	}
if (*net_loc)
	{
	strcpy_ee(ptr,_T("//"));
	strcpy_ee(ptr,net_loc);
	}
if (absflag)
	strcpy_ee(ptr,_T("/"));
if (*pathbuf)
	{
	strcpy_ee(ptr,pathbuf);
	if (endslash)
		strcpy_ee(ptr,_T("/"));
	}

if (*params)
	{
	strcpy_ee(ptr,_T(";"));
	strcpy_ee(ptr,params);
	}
if (*query)
	{
	strcpy_ee(ptr,_T("?"));
	strcpy_ee(ptr,query);
	}
if (*fragment)
	{
	strcpy_ee(ptr,_T("#"));
	strcpy_ee(ptr,fragment);
	}

return true;
}


/**************************************************
*						LEVENSHTEIN_INT
* FUN:	levenshtein_int
* SUBJ:	Edit distance of two strings.
* CR:		03/06/07 AM.
* RET:	int - distance. Or -1 if empty string given.
*
**************************************************/

long levenshtein_int(
	_TCHAR *s1,
	_TCHAR *s2
	)
{
if (!s1 || !*s1 || !s2 || !*s2)
	return -1;

unsigned int n1 = _tcslen(s1);	// strlen()
unsigned int n2 = _tcslen(s2);

unsigned int *p = new unsigned int[n2+1];
unsigned int *q = new unsigned int[n2+1];
unsigned int *r;

p[0] = 0;
for (unsigned int j = 1; j <= n2; ++j)
   p[j] = p[j-1] + 1;

for (unsigned int i = 1; i <= n1; ++i)
   {
   q[0] = p[0] + 1;
   for (unsigned int j = 1; j <= n2; ++j)
      {
      unsigned int d_del = p[j] + 1;
      unsigned int d_ins = q[j-1] + 1;
      unsigned int d_sub = p[j-1] + ( s1[i-1] == s2[j-1] ? 0 : 1 );
      // Get min of 3 items.	// 03/06/07 AM.
      if (d_del < d_ins)
			q[j] = ((d_sub < d_del) ? d_sub : d_del);
		else
			q[j] = ((d_sub < d_ins) ? d_sub : d_ins);
//      q[j] = min(min(d_del, d_ins), d_sub);
		}
   r = p;
   p = q;
   q = r;
   }

//unsigned int tmp = p[n2];
long tmp = p[n2];
delete[] p;
delete[] q;

return tmp;
}


/**************************************************
*						U_STRLEN
* FUN:	u_stringlen
* SUBJ:	Unicode count of chars in a string.
* CR:	06/17/22 AM.
* RET:	len = length of string in unicode bytes.
* NOTE:	Based on DDH code.
*
**************************************************/

long u_strlen(
	_TCHAR *name
	)
{
if (!name || !*name)
	return 0;

icu::StringPiece sp(name);
const char *spd = sp.data();
int32_t length = sp.length();

UChar32 c = 1;
int32_t i = 0;
int len = 0;

while (c && i<length) {
	U8_NEXT(spd, i, length, c);
	len++;
}
return len;

//icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(name));
//const UChar *strBuf = reinterpret_cast<const UChar *>(ustr.getTerminatedBuffer());
//int len = unicu::strLen(strBuf);
// return _tcsclen(name1);	[BYTE LENGTH]
}