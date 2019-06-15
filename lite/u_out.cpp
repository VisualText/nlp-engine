/*******************************************************************************
Copyright (c) 2005-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	U_OUT.CPP
* FILE:	lite\u_out.cpp
* CR:		03/10/05 AM.
* SUBJ:	Conversions for output of Unicode to UTF-8 files.
* NOTE:	By casting to special types, conversions will get done automatically.
*			Eg
*			out << num1
*				 << (u_string) string1
*				 << num2
*				  ;
*
*******************************************************************************/

#include "StdAfx.h"


#ifdef UNICODE
#include "machine.h"	// 10/25/06 AM.
#include "lite/global.h"
#include "u_out.h"

_t_ofstream &STDOPERATOR<<(_t_ofstream &os, u_string* &str)
{
// Recast to real type.
_TCHAR* lpszBuffer = (_TCHAR*) str;

char *lpstr8;
u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
os << lpstr8;
u_delete((LPCTSTR*&)lpstr8);
return os;
}

_t_ostream &STDOPERATOR<<(_t_ostream &os, u_string* &str)
{
// Recast to real type.
_TCHAR* lpszBuffer = (_TCHAR*) str;

char *lpstr8;
u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
os << lpstr8;
u_delete((LPCTSTR*&)lpstr8);
return os;
}

// Define these outside of any class!
_t_ofstream &STDOPERATOR<<(_t_ofstream &os, _TCHAR* &lpszBuffer)
{
char *lpstr8;
u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
os << lpstr8;
u_delete((LPCTSTR*&)lpstr8);
return os;
}

_t_ostream &STDOPERATOR<<(_t_ostream &os, _TCHAR* &lpszBuffer)
{
char *lpstr8;
u_to_mbcs((LPCWSTR)lpszBuffer, CP_UTF8, (LPCTSTR*&)lpstr8);
os << lpstr8;
u_delete((LPCTSTR*&)lpstr8);
return os;
}

/********************************************
* FN:		ALPHABETIC_EXTEND
* CR:		01/30/06 AM.
* SUBJ:	See if alphabetic word is extended.
* RET:	True if extend alphabetic.
* NOTE:	Allowing some accented chars to be alphabetic.
*			The ANSI CHARACTER SET.  Used in Windows OS.
* OPT:	Could optimize by using bits in the char, or by setting up a
*			switch statement.
********************************************/

bool alphabetic_extend(
	UChar32 ch,		// Current codepoint.
	short p_gcb,	// uchar_grapheme_cluster_break for previous codepoint.
	short p_wb,		// uchar_word_break for previous codepoint.
	short p_script,	//
	/*UP*/
	short &u_gcb,	// New char's flag.
	short &u_wb,		// New char's flag.
	short &u_script
	)
{

u_gcb = 0;		// Initialize.
u_wb = p_wb;	// By default, keep previous char's word break info.
u_script = p_script;	// By default.

if (!ch)
	return false;
//if (!u_isalpha(ch))
//	return false;

// EXTENDED CHARACTER BOUNDARY.
// or GRAPHEME CLUSTER BOUNDARY
// Note: All the Korean (Hangul) handling not really needed here.
//		Handy to copy if someone wants to parse chars of this language at some point.

u_gcb = u_getIntPropertyValue(ch, UCHAR_GRAPHEME_CLUSTER_BREAK);

switch (u_gcb)
	{
	case U_GCB_EXTEND:		/*[EX]*/
		return true;
		break;
	case U_GCB_L:				/*[L]*/
	case U_GCB_LV:				/*[LV]*/
	case U_GCB_LVT:			/*[LVT]*/
		// Prior Hangul must be an [L].
		if (p_gcb == U_GCB_L)
			return true;
		// Doesn't extend char, but extends Hangul word.
		//u_wb = u_getIntPropertyValue(ch, UCHAR_WORD_BREAK);
#ifdef _HANGUL_GLOM_
		return true;
#else
		return false;	// Each extended char is separate.
#endif
		break;
	case U_GCB_T:				/*[T]*/
		// Prior Hangul type.
		switch(p_gcb)
			{
			case U_GCB_LV:
			case U_GCB_LVT:
			case U_GCB_T:
			case U_GCB_V:
				return true;
			default:
				break;
			}
		// Doesn't extend char, but extends Hangul word.
		//u_wb = u_getIntPropertyValue(ch, UCHAR_WORD_BREAK);
#ifdef _HANGUL_GLOM_
		return true;
#else
		return false;	// Each extended char is separate.
#endif
		break;
	case U_GCB_V:				/*[V]*/
		// Prior Hangul type.
		switch(p_gcb)
			{
			case U_GCB_L:
			case U_GCB_LV:
			case U_GCB_V:
				return true;
			default:
				break;
			}
		// Doesn't extend char, but extends Hangul word.
		// u_wb = u_getIntPropertyValue(ch, UCHAR_WORD_BREAK);
#ifdef _HANGUL_GLOM_
		return true;
#else
		return false;	// Each extended char is separate.
#endif
		break;
	case U_GCB_CR:				/*[CR]*/
	case U_GCB_LF:				/*[LF]*/
	case U_GCB_CONTROL:		/*[CN]*/
	case U_GCB_COUNT:
		return false;
		break;
	case U_GCB_OTHER:			/*[XX]*/
	default:
		// Fall out to word boundary reasoning.
		break;
	}

// WORD BOUNDARY.

u_wb = u_getIntPropertyValue(ch, UCHAR_WORD_BREAK);

switch (u_wb)
	{
	case U_WB_OTHER:	/*[XX]*/
		return false;
		break;
	case U_WB_ALETTER:	/*[LE]*/
		if (p_wb != U_WB_ALETTER)
			return false;
		// Fall out to check for language change.
		break;
	case U_WB_FORMAT:	/*[FO]*/
		u_wb = p_wb;	// Reset to previous codepoint info.
		return true;
		break;
	case U_WB_KATAKANA:	/*[KA]*/
		return (p_wb == U_WB_KATAKANA) ? true : false;
		break;
	case U_WB_MIDLETTER:	/*[ML]*/
	case U_WB_MIDNUM:	/*[MN]*/
	case U_WB_NUMERIC:	/*[NU]*/
	case U_WB_EXTENDNUMLET:	/*[EX]*/
	case U_WB_COUNT:
	default:
		return false;
		break;
	}

// SCRIPT BOUNDARY
// ie, LANGUAGE BOUNDARY
// NOTE: Not part of the Unicode Standard, but important for proper tokenizing
//			of multi-language text in VisualText analyzers.
UErrorCode errorCode;
u_script = uscript_getScript(ch,&errorCode);					// 01/30/06 AM.

switch (u_script)
	{
	case USCRIPT_INVALID_CODE:
		break;
	default:
		if (u_script == p_script)
			return true;
		break;
   }

u_wb = 0;
return false;
}

#endif
