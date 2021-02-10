/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XML.CPP
* FILE:	lite/xml.cpp
* CR:		12/29/99 AM.
* SUBJ:	Utilities for XML.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "chars.h"
#include "io.h"
#include "string.h"
#include "htab.h"

inline void xstrcpy_e(													// 12/30/99 AM.
	_TCHAR* &ptr,			// Pointer to buffer for copying into.
							// Points to last FILLED char.
	_TCHAR *str			// Terminated string to copy.
	);

#include "xml.h"


/********************************************
* FN:		TEXT2XML
* CR:		12/29/99 AM.
* SUBJ:	Convert a text buffer to XML.
* NOTE:	Just finding things like & and ".
*			VARIANT.
*********************************************/

_TCHAR *Xml::text2xml(
	_TCHAR *text,				// Text to convert.
	/*DU*/
	_TCHAR *xml				// BUFFER to fill.
	)
{
if (!xml || !text)
	return 0;
_TCHAR *save = xml;
*xml = '\0';

--xml;	// Just a convenience to start here.
while (*text)
	xmlchar(xml,text);		// Convert char to xml.			// 12/30/99 AM.

*++xml = '\0';			// Terminate.
return save;
}

/********************************************
* FN:		TEXT2XML
* CR:		12/29/99 AM.
* SUBJ:	Convert a text buffer to XML.
* NOTE:	Just finding things like & and ".
*			VARIANT.
*********************************************/

_TCHAR *Xml::text2xml(
	_TCHAR *text,				// Text to convert.
	long len,				// Length of text to convert.
	/*DU*/
	_TCHAR *xml				// BUFFER to fill.
	)
{
if (!xml || !text || len <= 0)
	return 0;
_TCHAR *save = xml;
*xml = '\0';

--xml;	// Just a convenience to start here.
while (len--)					// For the given length.
	xmlchar(xml,text);		// Convert char to xml.			// 12/30/99 AM.
*++xml = '\0';			// Terminate.
return save;
}


/********************************************
* FN:		TEXT2XML
* CR:		12/29/99 AM.
* SUBJ:	Convert a text buffer to XML.
* NOTE:	Interns the string in given hash table.
*			VARIANT.
*********************************************/

_TCHAR *Xml::text2xml(
	_TCHAR *text,				// Buffer with text to convert.
	long len,				// Length of text to convert.
	Htab *htab				// Hash table for interning text.
	)
{
if (!text || !*text || !htab || len <= 0)
	return 0;

// Create temporary buffer for text.
// char to &#...; => NEED SIX TIMES LARGER.	// FIX.	// 11/11/14 AM.
//_TCHAR *buf = Chars::create(2*len + 100);	// Extra space for conversion.
_TCHAR *buf = Chars::create(10*len + 100);	// Extra space for conversion.

text2xml(text,len,/*DU*/buf);					// Convert to XML.
_TCHAR *xml = htab->getStr(buf);				// Intern string.
Chars::destroy(buf);								// Destroy temporary buffer.
return xml;											// Return string.
}


/********************************************
* FN:		XMLCHAR
* CR:		12/30/99 AM.
* UPD:		03/16/10 AM.
* SUBJ:	Convert a text char to XML equivalent.
* NOTE:	Interns the string in given hash table.
*			VARIANT.
*********************************************/

inline void Xml::xmlchar(
	_TCHAR* &xml,				// Pointer to last filled char in xml buffer.
	_TCHAR* &text				// Pointer to next char to convert in text buffer.
	)
{
_TCHAR buf[32];	// Buffer for char converted to number.

switch (*text)
	{
	// Presumably the commented ones don't need to be converted.
//	case ' ':	xstrcpy_e(xml, "&sp;");			++text;	break;
//	case '!':	xstrcpy_e(xml, "&excl;");		++text;	break;
	case '"':	xstrcpy_e(xml, _T("&quot;"));		++text;	break;
//	case '#':	xstrcpy_e(xml, "&num;");			++text;	break;
//	case '$':	xstrcpy_e(xml, "&dollar;");		++text;	break;
//	case '%':	xstrcpy_e(xml, "&percnt;");		++text;	break;
	case '&':	xstrcpy_e(xml, _T("&amp;"));			++text;	break;
//	case '\'':	xstrcpy_e(xml, "&apos;");		++text;	break;
//	case '(':	xstrcpy_e(xml, "&lpar;");		++text;	break;
//	case ')':	xstrcpy_e(xml, "&rpar;");		++text;	break;
//	case '*':	xstrcpy_e(xml, "&ast;");			++text;	break;
//	case '+':	xstrcpy_e(xml, "&plus;");		++text;	break;
//	case ',':	xstrcpy_e(xml, "&comma;");		++text;	break;
//	case '-':	xstrcpy_e(xml, "&hyphen;");		++text;	break; // &minus;
//	case '.':	xstrcpy_e(xml, "&period;");		++text;	break;
//	case '/':	xstrcpy_e(xml, "&sol;");			++text;	break;
//	case ':':	xstrcpy_e(xml, "&colon;");		++text;	break;
//	case ';':	xstrcpy_e(xml, "&semi;");		++text;	break;
	case '<':	xstrcpy_e(xml, _T("&lt;"));			++text;	break;
//	case '=':	xstrcpy_e(xml, "&equals;");		++text;	break;
	case '>':	xstrcpy_e(xml, _T("&gt;"));			++text;	break;
//	case '?':	xstrcpy_e(xml, "&quest;");		++text;	break;
//	case '@':	xstrcpy_e(xml, "&commat;");		++text;	break;
//	case '[':	xstrcpy_e(xml, "&lsqb;");		++text;	break;
//	case '\\':	xstrcpy_e(xml, "&bsol;");		++text;	break;
//	case ']':	xstrcpy_e(xml, "&rsqb;");		++text;	break;
//	case '^':	xstrcpy_e(xml, "&circ;");		++text;	break;
//	case '_':	xstrcpy_e(xml, "&lowbar;");		++text;	break; // &horbar;
//	case '`':	xstrcpy_e(xml, "&grave;");		++text;	break;
//	case '{':	xstrcpy_e(xml, "&lcub;");		++text;	break;
//	case '|':	xstrcpy_e(xml, "&verbar;");		++text;	break;
//	case '}':	xstrcpy_e(xml, "&rcub;");		++text;	break;
//	case '~':	xstrcpy_e(xml, "&tilde;");		++text;	break;
// del  &#127;
	case '\x82':	// xstrcpy_e(xml, _T("&lsquor;"));		++text;	break;
	case '\x83':	// xstrcpy_e(xml, _T("&fnof;"));		++text;	break;
	case '\x84':	// xstrcpy_e(xml, _T("&ldquor;"));		++text;	break;
	case '\x85':	// xstrcpy_e(xml, _T("&hellip;"));		++text;	break; // &ldots;
	case '\x86':	// xstrcpy_e(xml, _T("&dagger;"));		++text;	break;
	case '\x87':	// xstrcpy_e(xml, _T("&Dagger;"));		++text;	break;
//	case '\x88':	// xstrcpy_e(xml, "&#136;");		++text;	break;
	case '\x89':	// xstrcpy_e(xml, _T("&permil;"));		++text;	break;
	case '\x8A':	// xstrcpy_e(xml, _T("&Scaron;"));		++text;	break;
	case '\x8B':	// xstrcpy_e(xml, _T("&lsaquo;"));		++text;	break;
	case '\x8C':	// xstrcpy_e(xml, _T("&OElig;"));		++text;	break;
	case '\x91':	// xstrcpy_e(xml, _T("&lsquo;"));		++text;	break; // &lsquor;
	case '\x92':	// xstrcpy_e(xml, _T("&rsquo;"));		++text;	break;
	case '\x93':	// xstrcpy_e(xml, _T("&ldquo;"));		++text;	break; // &rdquor;
	case '\x94':	// xstrcpy_e(xml, _T("&rdquo;"));		++text;	break;
	case '\x95':	// xstrcpy_e(xml, _T("&bull;"));		++text;	break;
	case '\x96':	// xstrcpy_e(xml, _T("&ndash;"));		++text;	break; // &endash;
	case '\x97':	// xstrcpy_e(xml, _T("&mdash;"));		++text;	break; // &emdash;
	case '\x98':	// xstrcpy_e(xml, _T("&tilde;"));		++text;	break; // ****
	case '\x99':	// xstrcpy_e(xml, _T("&trade;"));		++text;	break;
	case '\x9A':	// xstrcpy_e(xml, _T("&scaron;"));		++text;	break;
	case '\x9B':	// xstrcpy_e(xml, _T("&rsaquo;"));		++text;	break;
	case '\x9C':	// xstrcpy_e(xml, _T("&oelig;"));		++text;	break;
	case '\x9F':	// xstrcpy_e(xml, _T("&Yuml;"));		++text;	break;
		xstrcpy_e(xml, _T("&#"));
		long_to_str((unsigned char)*text,buf); xstrcpy_e(xml, buf); ++text;
		xstrcpy_e(xml, _T(";"));
		break;

	case '\xA0':	xstrcpy_e(xml, _T("&nbsp;"));		++text;	break;

	case '\xA1':	// xstrcpy_e(xml, _T("&iexcl;"));		++text;	break;
	case '\xA2':	// xstrcpy_e(xml, _T("&cent;"));		++text;	break;
	case '\xA3':	// xstrcpy_e(xml, _T("&pound;"));		++text;	break;
	case '\xA4':	// xstrcpy_e(xml, _T("&curren;"));		++text;	break;
	case '\xA5':	// xstrcpy_e(xml, _T("&yen;"));			++text;	break;
	case '\xA6':	// xstrcpy_e(xml, _T("&brvbar;"));		++text;	break; // &brkbar;
	case '\xA7':	// xstrcpy_e(xml, _T("&sect;"));		++text;	break;
	case '\xA8':	// xstrcpy_e(xml, _T("&uml;"));			++text;	break; // &die;
	case '\xA9':	// xstrcpy_e(xml, _T("&copy;"));		++text;	break;
	case '\xAA':	// xstrcpy_e(xml, _T("&ordf;"));		++text;	break;
	case '\xAB':	// xstrcpy_e(xml, _T("&laquo;"));		++text;	break;
	case '\xAC':	// xstrcpy_e(xml, _T("&not;"));			++text;	break;
	case '\xAD':	// xstrcpy_e(xml, _T("&shy;"));			++text;	break;
	case '\xAE':	// xstrcpy_e(xml, _T("&reg;"));			++text;	break;
	case '\xAF':	// xstrcpy_e(xml, _T("&macr;"));		++text;	break; // &hibar;

	case '\xB0':	// xstrcpy_e(xml, _T("&deg;"));			++text;	break;
	case '\xB1':	// xstrcpy_e(xml, _T("&plusmn;"));		++text;	break;
	case '\xB2':	// xstrcpy_e(xml, _T("&sup2;"));		++text;	break;
	case '\xB3':	// xstrcpy_e(xml, _T("&sup3;"));		++text;	break;
	case '\xB4':	// xstrcpy_e(xml, _T("&acute;"));		++text;	break;
	case '\xB5':	// xstrcpy_e(xml, _T("&micro;"));		++text;	break;
	case '\xB6':	// xstrcpy_e(xml, _T("&para;"));		++text;	break;
	case '\xB7':	// xstrcpy_e(xml, _T("&middot;"));		++text;	break;
	case '\xB8':	// xstrcpy_e(xml, _T("&cedil;"));		++text;	break;
	case '\xB9':	// xstrcpy_e(xml, _T("&sup1;"));		++text;	break;
	case '\xBA':	// xstrcpy_e(xml, _T("&ordm;"));		++text;	break;
	case '\xBB':	// xstrcpy_e(xml, _T("&raquo;"));		++text;	break;
	case '\xBC':	// xstrcpy_e(xml, _T("&frac14;"));		++text;	break;
	case '\xBD':	// xstrcpy_e(xml, _T("&frac12;"));		++text;	break; // &half;
	case '\xBE':	// xstrcpy_e(xml, _T("&frac34;"));		++text;	break;
	case '\xBF':	// xstrcpy_e(xml, _T("&iquest;"));		++text;	break;

	// Upper Case Latin-1 Letters
	case '\xC0':	// xstrcpy_e(xml, _T("&Agrave;"));		++text;	break;
	case '\xC1':	// xstrcpy_e(xml, _T("&Aacute;"));		++text;	break;
	case '\xC2':	// xstrcpy_e(xml, _T("&Acirc;"));		++text;	break;
	case '\xC3':	// xstrcpy_e(xml, _T("&Atilde;"));		++text;	break;
	case '\xC4':	// xstrcpy_e(xml, _T("&Auml;"));		++text;	break;
	case '\xC5':	// xstrcpy_e(xml, _T("&Aring;"));		++text;	break;
	case '\xC6':	// xstrcpy_e(xml, _T("&AElig;"));		++text;	break;
	case '\xC7':	// xstrcpy_e(xml, _T("&Ccedil;"));		++text;	break;
	case '\xC8':	// xstrcpy_e(xml, _T("&Egrave;"));		++text;	break;
	case '\xC9':	// xstrcpy_e(xml, _T("&Eacute;"));		++text;	break;
	case '\xCA':	// xstrcpy_e(xml, _T("&Ecirc;"));		++text;	break;
	case '\xCB':	// xstrcpy_e(xml, _T("&Euml;"));		++text;	break;
	case '\xCC':	// xstrcpy_e(xml, _T("&Igrave;"));		++text;	break;
	case '\xCD':	// xstrcpy_e(xml, _T("&Iacute;"));		++text;	break;
	case '\xCE':	// xstrcpy_e(xml, _T("&Icirc;"));		++text;	break;
	case '\xCF':	// xstrcpy_e(xml, _T("&Iuml;"));		++text;	break;

	case '\xD0':	// xstrcpy_e(xml, _T("&ETH;"));			++text;	break;
	case '\xD1':	// xstrcpy_e(xml, _T("&Ntilde;"));		++text;	break;
	case '\xD2':	// xstrcpy_e(xml, _T("&Ograve;"));		++text;	break;
	case '\xD3':	// xstrcpy_e(xml, _T("&Oacute;"));		++text;	break;
	case '\xD4':	// xstrcpy_e(xml, _T("&Ocirc;"));		++text;	break;
	case '\xD5':	// xstrcpy_e(xml, _T("&Otilde;"));		++text;	break;
	case '\xD6':	// xstrcpy_e(xml, _T("&Ouml;"));		++text;	break;
	case '\xD7':	// xstrcpy_e(xml, _T("&times;"));		++text;	break;
	case '\xD8':	// xstrcpy_e(xml, _T("&Oslash;"));		++text;	break;
	case '\xD9':	// xstrcpy_e(xml, _T("&Ugrave;"));		++text;	break;
	case '\xDA':	// xstrcpy_e(xml, _T("&Uacute;"));		++text;	break;
	case '\xDB':	// xstrcpy_e(xml, _T("&Ucirc;"));		++text;	break;
	case '\xDC':	// xstrcpy_e(xml, _T("&Uuml;"));		++text;	break;
	case '\xDD':	// xstrcpy_e(xml, _T("&Yacute;"));		++text;	break;
	case '\xDE':	// xstrcpy_e(xml, _T("&THORN;"));		++text;	break;
	case '\xDF':	// xstrcpy_e(xml, _T("&szlig;"));		++text;	break;
	
	// Lower Case Latin-1 Letters
	case '\xE0':	// xstrcpy_e(xml, _T("&agrave;"));		++text;	break;
	case '\xE1':	// xstrcpy_e(xml, _T("&aacute;"));		++text;	break;
	case '\xE2':	// xstrcpy_e(xml, _T("&acirc;"));		++text;	break;
	case '\xE3':	// xstrcpy_e(xml, _T("&atilde;"));		++text;	break;
	case '\xE4':	// xstrcpy_e(xml, _T("&auml;"));		++text;	break;
	case '\xE5':	// xstrcpy_e(xml, _T("&aring;"));		++text;	break;
	case '\xE6':	// xstrcpy_e(xml, _T("&aelig;"));		++text;	break;
	case '\xE7':	// xstrcpy_e(xml, _T("&ccedil;"));		++text;	break;
	case '\xE8':	// xstrcpy_e(xml, _T("&egrave;"));		++text;	break;
	case '\xE9':	// xstrcpy_e(xml, _T("&eacute;"));		++text;	break;
	case '\xEA':	// xstrcpy_e(xml, _T("&ecirc;"));		++text;	break;
	case '\xEB':	// xstrcpy_e(xml, _T("&euml;"));		++text;	break;
	case '\xEC':	// xstrcpy_e(xml, _T("&igrave;"));		++text;	break;
	case '\xED':	// xstrcpy_e(xml, _T("&iacute;"));		++text;	break;
	case '\xEE':	// xstrcpy_e(xml, _T("&icirc;"));		++text;	break;
	case '\xEF':	// xstrcpy_e(xml, _T("&iuml;"));		++text;	break;

	case '\xF0':	// xstrcpy_e(xml, _T("&eth;"));			++text;	break;
	case '\xF1':	// xstrcpy_e(xml, _T("&ntilde;"));		++text;	break;
	case '\xF2':	// xstrcpy_e(xml, _T("&ograve;"));		++text;	break;
	case '\xF3':	// xstrcpy_e(xml, _T("&oacute;"));		++text;	break;
	case '\xF4':	// xstrcpy_e(xml, _T("&ocirc;"));		++text;	break;
	case '\xF5':	// xstrcpy_e(xml, _T("&otilde;"));		++text;	break;
	case '\xF6':	// xstrcpy_e(xml, _T("&ouml;"));		++text;	break;
	case '\xF7':	// xstrcpy_e(xml, _T("&divide;"));		++text;	break;
	case '\xF8':	// xstrcpy_e(xml, _T("&oslash;"));		++text;	break;
	case '\xF9':	// xstrcpy_e(xml, _T("&ugrave;"));		++text;	break;
	case '\xFA':	// xstrcpy_e(xml, _T("&uacute;"));		++text;	break;
	case '\xFB':	// xstrcpy_e(xml, _T("&ucirc;"));		++text;	break;
	case '\xFC':	// xstrcpy_e(xml, _T("&uuml;"));		++text;	break;
	case '\xFD':	// xstrcpy_e(xml, _T("&yacute;"));		++text;	break;
	case '\xFE':	// xstrcpy_e(xml, _T("&thorn;"));		++text;	break;
	case '\xFF':	// xstrcpy_e(xml, _T("&yuml;"));		++text;	break;
		xstrcpy_e(xml, _T("&#"));
		long_to_str((unsigned char)*text,buf); xstrcpy_e(xml, buf); ++text;
		xstrcpy_e(xml, _T(";"));
		break;

	default:		// All other chars.
		*++xml = *text++;
		break;
	}
}


/********************************************
* FN:		HTMLCHAR
* CR:		12/30/99 AM.
* SUBJ:	Convert a text char to XML equivalent.
* NOTE:	Interns the string in given hash table.
*			VARIANT.
*			Renamed from XMLCHAR. 03/16/10 AM.
*********************************************/

inline void Xml::htmlchar(
	_TCHAR* &xml,				// Pointer to last filled char in xml buffer.
	_TCHAR* &text				// Pointer to next char to convert in text buffer.
	)
{
switch (*text)
	{
	// Presumably the commented ones don't need to be converted.
//	case ' ':	xstrcpy_e(xml, "&sp;");			++text;	break;
//	case '!':	xstrcpy_e(xml, "&excl;");		++text;	break;
	case '"':	xstrcpy_e(xml, _T("&quot;"));		++text;	break;
//	case '#':	xstrcpy_e(xml, "&num;");			++text;	break;
//	case '$':	xstrcpy_e(xml, "&dollar;");		++text;	break;
//	case '%':	xstrcpy_e(xml, "&percnt;");		++text;	break;
	case '&':	xstrcpy_e(xml, _T("&amp;"));			++text;	break;
//	case '\'':	xstrcpy_e(xml, "&apos;");		++text;	break;
//	case '(':	xstrcpy_e(xml, "&lpar;");		++text;	break;
//	case ')':	xstrcpy_e(xml, "&rpar;");		++text;	break;
//	case '*':	xstrcpy_e(xml, "&ast;");			++text;	break;
//	case '+':	xstrcpy_e(xml, "&plus;");		++text;	break;
//	case ',':	xstrcpy_e(xml, "&comma;");		++text;	break;
//	case '-':	xstrcpy_e(xml, "&hyphen;");		++text;	break; // &minus;
//	case '.':	xstrcpy_e(xml, "&period;");		++text;	break;
//	case '/':	xstrcpy_e(xml, "&sol;");			++text;	break;
//	case ':':	xstrcpy_e(xml, "&colon;");		++text;	break;
//	case ';':	xstrcpy_e(xml, "&semi;");		++text;	break;
	case '<':	xstrcpy_e(xml, _T("&lt;"));			++text;	break;
//	case '=':	xstrcpy_e(xml, "&equals;");		++text;	break;
	case '>':	xstrcpy_e(xml, _T("&gt;"));			++text;	break;
//	case '?':	xstrcpy_e(xml, "&quest;");		++text;	break;
//	case '@':	xstrcpy_e(xml, "&commat;");		++text;	break;
//	case '[':	xstrcpy_e(xml, "&lsqb;");		++text;	break;
//	case '\\':	xstrcpy_e(xml, "&bsol;");		++text;	break;
//	case ']':	xstrcpy_e(xml, "&rsqb;");		++text;	break;
//	case '^':	xstrcpy_e(xml, "&circ;");		++text;	break;
//	case '_':	xstrcpy_e(xml, "&lowbar;");		++text;	break; // &horbar;
//	case '`':	xstrcpy_e(xml, "&grave;");		++text;	break;
//	case '{':	xstrcpy_e(xml, "&lcub;");		++text;	break;
//	case '|':	xstrcpy_e(xml, "&verbar;");		++text;	break;
//	case '}':	xstrcpy_e(xml, "&rcub;");		++text;	break;
//	case '~':	xstrcpy_e(xml, "&tilde;");		++text;	break;
// del  &#127;
	case '\x82':	xstrcpy_e(xml, _T("&lsquor;"));		++text;	break;
	case '\x83':	xstrcpy_e(xml, _T("&fnof;"));		++text;	break;
	case '\x84':	xstrcpy_e(xml, _T("&ldquor;"));		++text;	break;
	case '\x85':	xstrcpy_e(xml, _T("&hellip;"));		++text;	break; // &ldots;
	case '\x86':	xstrcpy_e(xml, _T("&dagger;"));		++text;	break;
	case '\x87':	xstrcpy_e(xml, _T("&Dagger;"));		++text;	break;
//	case '\x88':	xstrcpy_e(xml, "&#136;");		++text;	break;
	case '\x89':	xstrcpy_e(xml, _T("&permil;"));		++text;	break;
	case '\x8A':	xstrcpy_e(xml, _T("&Scaron;"));		++text;	break;
	case '\x8B':	xstrcpy_e(xml, _T("&lsaquo;"));		++text;	break;
	case '\x8C':	xstrcpy_e(xml, _T("&OElig;"));		++text;	break;
	case '\x91':	xstrcpy_e(xml, _T("&lsquo;"));		++text;	break; // &lsquor;
	case '\x92':	xstrcpy_e(xml, _T("&rsquo;"));		++text;	break;
	case '\x93':	xstrcpy_e(xml, _T("&ldquo;"));		++text;	break; // &rdquor;
	case '\x94':	xstrcpy_e(xml, _T("&rdquo;"));		++text;	break;
	case '\x95':	xstrcpy_e(xml, _T("&bull;"));		++text;	break;
	case '\x96':	xstrcpy_e(xml, _T("&ndash;"));		++text;	break; // &endash;
	case '\x97':	xstrcpy_e(xml, _T("&mdash;"));		++text;	break; // &emdash;
	case '\x98':	xstrcpy_e(xml, _T("&tilde;"));		++text;	break; // ****
	case '\x99':	xstrcpy_e(xml, _T("&trade;"));		++text;	break;
	case '\x9A':	xstrcpy_e(xml, _T("&scaron;"));		++text;	break;
	case '\x9B':	xstrcpy_e(xml, _T("&rsaquo;"));		++text;	break;
	case '\x9C':	xstrcpy_e(xml, _T("&oelig;"));		++text;	break;
	case '\x9F':	xstrcpy_e(xml, _T("&Yuml;"));		++text;	break;

	case '\xA0':	xstrcpy_e(xml, _T("&nbsp;"));		++text;	break;
	case '\xA1':	xstrcpy_e(xml, _T("&iexcl;"));		++text;	break;
	case '\xA2':	xstrcpy_e(xml, _T("&cent;"));		++text;	break;
	case '\xA3':	xstrcpy_e(xml, _T("&pound;"));		++text;	break;
	case '\xA4':	xstrcpy_e(xml, _T("&curren;"));		++text;	break;
	case '\xA5':	xstrcpy_e(xml, _T("&yen;"));			++text;	break;
	case '\xA6':	xstrcpy_e(xml, _T("&brvbar;"));		++text;	break; // &brkbar;
	case '\xA7':	xstrcpy_e(xml, _T("&sect;"));		++text;	break;
	case '\xA8':	xstrcpy_e(xml, _T("&uml;"));			++text;	break; // &die;
	case '\xA9':	xstrcpy_e(xml, _T("&copy;"));		++text;	break;
	case '\xAA':	xstrcpy_e(xml, _T("&ordf;"));		++text;	break;
	case '\xAB':	xstrcpy_e(xml, _T("&laquo;"));		++text;	break;
	case '\xAC':	xstrcpy_e(xml, _T("&not;"));			++text;	break;
	case '\xAD':	xstrcpy_e(xml, _T("&shy;"));			++text;	break;
	case '\xAE':	xstrcpy_e(xml, _T("&reg;"));			++text;	break;
	case '\xAF':	xstrcpy_e(xml, _T("&macr;"));		++text;	break; // &hibar;

	case '\xB0':	xstrcpy_e(xml, _T("&deg;"));			++text;	break;
	case '\xB1':	xstrcpy_e(xml, _T("&plusmn;"));		++text;	break;
	case '\xB2':	xstrcpy_e(xml, _T("&sup2;"));		++text;	break;
	case '\xB3':	xstrcpy_e(xml, _T("&sup3;"));		++text;	break;
	case '\xB4':	xstrcpy_e(xml, _T("&acute;"));		++text;	break;
	case '\xB5':	xstrcpy_e(xml, _T("&micro;"));		++text;	break;
	case '\xB6':	xstrcpy_e(xml, _T("&para;"));		++text;	break;
	case '\xB7':	xstrcpy_e(xml, _T("&middot;"));		++text;	break;
	case '\xB8':	xstrcpy_e(xml, _T("&cedil;"));		++text;	break;
	case '\xB9':	xstrcpy_e(xml, _T("&sup1;"));		++text;	break;
	case '\xBA':	xstrcpy_e(xml, _T("&ordm;"));		++text;	break;
	case '\xBB':	xstrcpy_e(xml, _T("&raquo;"));		++text;	break;
	case '\xBC':	xstrcpy_e(xml, _T("&frac14;"));		++text;	break;
	case '\xBD':	xstrcpy_e(xml, _T("&frac12;"));		++text;	break; // &half;
	case '\xBE':	xstrcpy_e(xml, _T("&frac34;"));		++text;	break;
	case '\xBF':	xstrcpy_e(xml, _T("&iquest;"));		++text;	break;

	// Upper Case Latin-1 Letters
	case '\xC0':	xstrcpy_e(xml, _T("&Agrave;"));		++text;	break;
	case '\xC1':	xstrcpy_e(xml, _T("&Aacute;"));		++text;	break;
	case '\xC2':	xstrcpy_e(xml, _T("&Acirc;"));		++text;	break;
	case '\xC3':	xstrcpy_e(xml, _T("&Atilde;"));		++text;	break;
	case '\xC4':	xstrcpy_e(xml, _T("&Auml;"));		++text;	break;
	case '\xC5':	xstrcpy_e(xml, _T("&Aring;"));		++text;	break;
	case '\xC6':	xstrcpy_e(xml, _T("&AElig;"));		++text;	break;
	case '\xC7':	xstrcpy_e(xml, _T("&Ccedil;"));		++text;	break;
	case '\xC8':	xstrcpy_e(xml, _T("&Egrave;"));		++text;	break;
	case '\xC9':	xstrcpy_e(xml, _T("&Eacute;"));		++text;	break;
	case '\xCA':	xstrcpy_e(xml, _T("&Ecirc;"));		++text;	break;
	case '\xCB':	xstrcpy_e(xml, _T("&Euml;"));		++text;	break;
	case '\xCC':	xstrcpy_e(xml, _T("&Igrave;"));		++text;	break;
	case '\xCD':	xstrcpy_e(xml, _T("&Iacute;"));		++text;	break;
	case '\xCE':	xstrcpy_e(xml, _T("&Icirc;"));		++text;	break;
	case '\xCF':	xstrcpy_e(xml, _T("&Iuml;"));		++text;	break;

	case '\xD0':	xstrcpy_e(xml, _T("&ETH;"));			++text;	break;
	case '\xD1':	xstrcpy_e(xml, _T("&Ntilde;"));		++text;	break;
	case '\xD2':	xstrcpy_e(xml, _T("&Ograve;"));		++text;	break;
	case '\xD3':	xstrcpy_e(xml, _T("&Oacute;"));		++text;	break;
	case '\xD4':	xstrcpy_e(xml, _T("&Ocirc;"));		++text;	break;
	case '\xD5':	xstrcpy_e(xml, _T("&Otilde;"));		++text;	break;
	case '\xD6':	xstrcpy_e(xml, _T("&Ouml;"));		++text;	break;
	case '\xD7':	xstrcpy_e(xml, _T("&times;"));		++text;	break;
	case '\xD8':	xstrcpy_e(xml, _T("&Oslash;"));		++text;	break;
	case '\xD9':	xstrcpy_e(xml, _T("&Ugrave;"));		++text;	break;
	case '\xDA':	xstrcpy_e(xml, _T("&Uacute;"));		++text;	break;
	case '\xDB':	xstrcpy_e(xml, _T("&Ucirc;"));		++text;	break;
	case '\xDC':	xstrcpy_e(xml, _T("&Uuml;"));		++text;	break;
	case '\xDD':	xstrcpy_e(xml, _T("&Yacute;"));		++text;	break;
	case '\xDE':	xstrcpy_e(xml, _T("&THORN;"));		++text;	break;
	case '\xDF':	xstrcpy_e(xml, _T("&szlig;"));		++text;	break;
	
	// Lower Case Latin-1 Letters
	case '\xE0':	xstrcpy_e(xml, _T("&agrave;"));		++text;	break;
	case '\xE1':	xstrcpy_e(xml, _T("&aacute;"));		++text;	break;
	case '\xE2':	xstrcpy_e(xml, _T("&acirc;"));		++text;	break;
	case '\xE3':	xstrcpy_e(xml, _T("&atilde;"));		++text;	break;
	case '\xE4':	xstrcpy_e(xml, _T("&auml;"));		++text;	break;
	case '\xE5':	xstrcpy_e(xml, _T("&aring;"));		++text;	break;
	case '\xE6':	xstrcpy_e(xml, _T("&aelig;"));		++text;	break;
	case '\xE7':	xstrcpy_e(xml, _T("&ccedil;"));		++text;	break;
	case '\xE8':	xstrcpy_e(xml, _T("&egrave;"));		++text;	break;
	case '\xE9':	xstrcpy_e(xml, _T("&eacute;"));		++text;	break;
	case '\xEA':	xstrcpy_e(xml, _T("&ecirc;"));		++text;	break;
	case '\xEB':	xstrcpy_e(xml, _T("&euml;"));		++text;	break;
	case '\xEC':	xstrcpy_e(xml, _T("&igrave;"));		++text;	break;
	case '\xED':	xstrcpy_e(xml, _T("&iacute;"));		++text;	break;
	case '\xEE':	xstrcpy_e(xml, _T("&icirc;"));		++text;	break;
	case '\xEF':	xstrcpy_e(xml, _T("&iuml;"));		++text;	break;

	case '\xF0':	xstrcpy_e(xml, _T("&eth;"));			++text;	break;
	case '\xF1':	xstrcpy_e(xml, _T("&ntilde;"));		++text;	break;
	case '\xF2':	xstrcpy_e(xml, _T("&ograve;"));		++text;	break;
	case '\xF3':	xstrcpy_e(xml, _T("&oacute;"));		++text;	break;
	case '\xF4':	xstrcpy_e(xml, _T("&ocirc;"));		++text;	break;
	case '\xF5':	xstrcpy_e(xml, _T("&otilde;"));		++text;	break;
	case '\xF6':	xstrcpy_e(xml, _T("&ouml;"));		++text;	break;
	case '\xF7':	xstrcpy_e(xml, _T("&divide;"));		++text;	break;
	case '\xF8':	xstrcpy_e(xml, _T("&oslash;"));		++text;	break;
	case '\xF9':	xstrcpy_e(xml, _T("&ugrave;"));		++text;	break;
	case '\xFA':	xstrcpy_e(xml, _T("&uacute;"));		++text;	break;
	case '\xFB':	xstrcpy_e(xml, _T("&ucirc;"));		++text;	break;
	case '\xFC':	xstrcpy_e(xml, _T("&uuml;"));		++text;	break;
	case '\xFD':	xstrcpy_e(xml, _T("&yacute;"));		++text;	break;
	case '\xFE':	xstrcpy_e(xml, _T("&thorn;"));		++text;	break;
	case '\xFF':	xstrcpy_e(xml, _T("&yuml;"));		++text;	break;

	default:		// All other chars.
		*++xml = *text++;
		break;
	}
}


/********************************************
* FN:		XSTRCPY_E
* CR:		12/30/99 AM.
* SUBJ:	String copy specialized for repeated copying.
* RET:	True if ok, else false.
* NOTE:	No error checking, for max speed.
*			Assumes string has at least a null char.
* WARN:	Must be defined in this file, or compiler fails.
*			(Because of inline of inline, probably.)
********************************************/

inline void xstrcpy_e(
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
* FN:		TEXT2SQL
* CR:		07/22/03 AM.
* SUBJ:	Convert a text buffer to SQL.
* NOTE:	VARIANT.
*********************************************/

_TCHAR *Xml::text2sql(
	_TCHAR *text,				// Text to convert.
	/*DU*/
	_TCHAR *sql				// BUFFER to fill.
	)
{
if (!sql || !text)
	return 0;
_TCHAR *save = sql;
*sql = '\0';

--sql;	// Just a convenience to start here.
--text;
while ((*++sql = *++text))
	{
	if (*text == '\'')
		*++sql = '\'';
	}

*++sql = '\0';			// Terminate.
return save;
}


/********************************************
* FN:		TEXT2SQL
* CR:		07/22/03 AM.
* SUBJ:	Convert a text buffer to SQL string.
* NOTE:	Interns the string in given hash table.
*			VARIANT.
*			Slapping this in here quick and dirty.
*********************************************/

_TCHAR *Xml::text2sql(
	_TCHAR *text,				// Buffer with text to convert.
	long len,				// Length of text to convert.
	Htab *htab				// Hash table for interning text.
	)
{
if (!text || !*text || !htab || len <= 0)
	return 0;

// Create temporary buffer for text.
_TCHAR *buf = Chars::create(2*len + 100);	// Extra space for conversion.

text2sql(text,len,/*DU*/buf);					// Convert to SQL.
_TCHAR *sql = htab->getStr(buf);				// Intern string.
Chars::destroy(buf);								// Destroy temporary buffer.
return sql;											// Return string.
}



/********************************************
* FN:		TEXT2SQL
* CR:		07/22/03 AM.
* SUBJ:	Convert a text buffer to XML.
* NOTE:	Just finding things like & and ".
*			VARIANT.
*********************************************/

_TCHAR *Xml::text2sql(
	_TCHAR *text,				// Text to convert.
	long len,				// Length of text to convert.
	/*DU*/
	_TCHAR *sql				// BUFFER to fill.
	)
{
if (!sql || !text || len <= 0)
	return 0;
_TCHAR *save = sql;
*sql = '\0';

--sql;	// Just a convenience to start here.
--text;	// Convenient start.
++len;	// Convenient start.
while (--len)					// For the given length.
	{
	*++sql = *++text;
	if (*text == '\'')
		*++sql = '\'';
	}
*++sql = '\0';			// Terminate.
return save;
}

//////////////////////////////

/********************************************
* FN:		DE_ACCENT
* CR:		09/07/03 AM.
* SUBJ:	Convert accented chars in a text.
* NOTE:	VARIANT.
*********************************************/

_TCHAR *Xml::de_accent(
	_TCHAR *text,				// Text to convert.
	/*DU*/
	_TCHAR *xml				// BUFFER to fill.
	)
{
if (!xml || !text)
	return 0;
_TCHAR *save = xml;
*xml = '\0';

--xml;	// Just a convenience to start here.
while (*text)
	de_accent_ch(xml,text);

*++xml = '\0';			// Terminate.
return save;
}

/********************************************
* FN:		DE_ACCENT
* CR:		09/07/03 AM.
* SUBJ:	Convert accented chars in a text.
* NOTE:	VARIANT.
*********************************************/

_TCHAR *Xml::de_accent(
	_TCHAR *text,				// Text to convert.
	long len,				// Length of text to convert.
	/*DU*/
	_TCHAR *xml				// BUFFER to fill.
	)
{
if (!xml || !text || len <= 0)
	return 0;
_TCHAR *save = xml;
*xml = '\0';

--xml;	// Just a convenience to start here.
while (len--)					// For the given length.
	de_accent_ch(xml,text);
*++xml = '\0';			// Terminate.
return save;
}


/********************************************
* FN:		DE_ACCENT
* CR:		09/07/03 AM.
* SUBJ:	Convert accented chars in a text.
* NOTE:	Interns the string in given hash table.
*			VARIANT.
*********************************************/

_TCHAR *Xml::de_accent(
	_TCHAR *text,				// Buffer with text to convert.
	long len,				// Length of text to convert.
	Htab *htab				// Hash table for interning text.
	)
{
if (!text || !*text || !htab || len <= 0)
	return 0;

// Create temporary buffer for text.
_TCHAR *buf = Chars::create(2*len + 100);	// Extra space for conversion.

de_accent(text,len,/*DU*/buf);
_TCHAR *xml = htab->getStr(buf);				// Intern string.
Chars::destroy(buf);								// Destroy temporary buffer.
return xml;											// Return string.
}


/********************************************
* FN:		DE_ACCENT_CH
* CR:		09/06/03 AM.
* SUBJ:	Convert a text char to de-accented equivalent.
*********************************************/

inline void Xml::de_accent_ch(
	_TCHAR* &xml,				// Pointer to last filled char in converted buffer.
	_TCHAR* &text				// Pointer to next char to convert in text buffer.
	)
{
switch (*text)
	{
	case '\x8A':	xstrcpy_e(xml, _T("S"));		++text;	break;	// Scaron
	case '\x8C':	xstrcpy_e(xml, _T("E"));		++text;	break;	// OElig
	case '\x9A':	xstrcpy_e(xml, _T("s"));		++text;	break;	// scaron
	case '\x9C':	xstrcpy_e(xml, _T("e"));		++text;	break;	// oelig
	case '\x9F':	xstrcpy_e(xml, _T("Y"));		++text;	break;	// Yuml

	// Upper Case Latin-1 Letters
	case '\xC0':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Agrave
	case '\xC1':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Aacute
	case '\xC2':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Acirc
	case '\xC3':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Atilde
	case '\xC4':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Auml
	case '\xC5':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Aring
	case '\xC6':	xstrcpy_e(xml, _T("E"));		++text;	break;	// AElig
	case '\xC7':	xstrcpy_e(xml, _T("C"));		++text;	break;	// Ccedil
	case '\xC8':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Egrave
	case '\xC9':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Eacute
	case '\xCA':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Ecirc
	case '\xCB':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Euml
	case '\xCC':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Igrave
	case '\xCD':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Iacute
	case '\xCE':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Icirc
	case '\xCF':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Iuml

//	case '\xD0':	xstrcpy_e(xml, "&ETH;");			++text;	break;
	case '\xD1':	xstrcpy_e(xml, _T("N"));		++text;	break;	// Ntilde
	case '\xD2':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Ograve
	case '\xD3':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Oacute
	case '\xD4':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Ocirc
	case '\xD5':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Otilde
	case '\xD6':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Ouml
	case '\xD8':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Oslash
	case '\xD9':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Ugrave
	case '\xDA':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Uacute
	case '\xDB':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Ucirc
	case '\xDC':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Uuml
	case '\xDD':	xstrcpy_e(xml, _T("Y"));		++text;	break;	// Yacute
	
	// Lower Case Latin-1 Letters
	case '\xE0':	xstrcpy_e(xml, _T("a"));		++text;	break;	// agrave
	case '\xE1':	xstrcpy_e(xml, _T("a"));		++text;	break;	// aacute
	case '\xE2':	xstrcpy_e(xml, _T("a"));		++text;	break;	// acirc
	case '\xE3':	xstrcpy_e(xml, _T("a"));		++text;	break;	// atilde
	case '\xE4':	xstrcpy_e(xml, _T("a"));		++text;	break;	// auml
	case '\xE5':	xstrcpy_e(xml, _T("a"));		++text;	break;	// aring
	case '\xE6':	xstrcpy_e(xml, _T("e"));		++text;	break;	// aelig
	case '\xE7':	xstrcpy_e(xml, _T("c"));		++text;	break;	// ccedil
	case '\xE8':	xstrcpy_e(xml, _T("e"));		++text;	break;	// egrave
	case '\xE9':	xstrcpy_e(xml, _T("e"));		++text;	break;	// eacute
	case '\xEA':	xstrcpy_e(xml, _T("e"));		++text;	break;	// ecirc
	case '\xEB':	xstrcpy_e(xml, _T("e"));		++text;	break;	// euml
	case '\xEC':	xstrcpy_e(xml, _T("i"));		++text;	break;	// igrave
	case '\xED':	xstrcpy_e(xml, _T("i"));		++text;	break;	// iacute
	case '\xEE':	xstrcpy_e(xml, _T("i"));		++text;	break;	// icirc
	case '\xEF':	xstrcpy_e(xml, _T("i"));		++text;	break;	// iuml

	case '\xF1':	xstrcpy_e(xml, _T("n"));		++text;	break;	// ntilde
	case '\xF2':	xstrcpy_e(xml, _T("o"));		++text;	break;	// ograve
	case '\xF3':	xstrcpy_e(xml, _T("o"));		++text;	break;	// oacute
	case '\xF4':	xstrcpy_e(xml, _T("o"));		++text;	break;	// ocirc
	case '\xF5':	xstrcpy_e(xml, _T("o"));		++text;	break;	// otilde
	case '\xF6':	xstrcpy_e(xml, _T("o"));		++text;	break;	// ouml
	case '\xF8':	xstrcpy_e(xml, _T("o"));		++text;	break;	// oslash
	case '\xF9':	xstrcpy_e(xml, _T("u"));		++text;	break;	// ugrave
	case '\xFA':	xstrcpy_e(xml, _T("u"));		++text;	break;	// uacute
	case '\xFB':	xstrcpy_e(xml, _T("u"));		++text;	break;	// ucirc
	case '\xFC':	xstrcpy_e(xml, _T("u"));		++text;	break;	// uuml
	case '\xFD':	xstrcpy_e(xml, _T("y"));		++text;	break;	// yacute
	case '\xFF':	xstrcpy_e(xml, _T("y"));		++text;	break;	// yuml

	default:		// All other chars.
		*++xml = *text++;
		break;
	}
}


//////////////////////////////

/********************************************
* FN:		DE_JUNK
* CR:		09/09/11 AM.
* SUBJ:	Convert non-ASCII chars in a text.
* NOTE:	VARIANT.
*********************************************/

_TCHAR *Xml::de_junk(
	_TCHAR *text,				// Text to convert.
	/*DU*/
	_TCHAR *xml				// BUFFER to fill.
	)
{
if (!xml || !text)
	return 0;
_TCHAR *save = xml;
*xml = '\0';

--xml;	// Just a convenience to start here.
while (*text)
	de_junk_ch(xml,text);

*++xml = '\0';			// Terminate.
return save;
}

/********************************************
* FN:		DE_JUNK
* CR:		09/09/11 AM.
* SUBJ:	Convert non-ASCII chars in a text.
* NOTE:	VARIANT.
*********************************************/

_TCHAR *Xml::de_junk(
	_TCHAR *text,				// Text to convert.
	long len,				// Length of text to convert.
	/*DU*/
	_TCHAR *xml				// BUFFER to fill.
	)
{
if (!xml || !text || len <= 0)
	return 0;
_TCHAR *save = xml;
*xml = '\0';

--xml;	// Just a convenience to start here.
while (len--)					// For the given length.
	de_junk_ch(xml,text);
*++xml = '\0';			// Terminate.
return save;
}


/********************************************
* FN:		DE_JUNK
* CR:		09/09/11 AM.
* SUBJ:	Convert accented chars in a text.
* NOTE:	Interns the string in given hash table.
*			VARIANT.
*********************************************/

_TCHAR *Xml::de_junk(
	_TCHAR *text,				// Buffer with text to convert.
	long len,				// Length of text to convert.
	Htab *htab				// Hash table for interning text.
	)
{
if (!text || !*text || !htab || len <= 0)
	return 0;

// Create temporary buffer for text.
_TCHAR *buf = Chars::create(2*len + 100);	// Extra space for conversion.

de_junk(text,len,/*DU*/buf);
_TCHAR *xml = htab->getStr(buf);				// Intern string.
Chars::destroy(buf);								// Destroy temporary buffer.
return xml;											// Return string.
}


/********************************************
* FN:		DE_JUNK_CH
* CR:		09/09/11 AM.
* SUBJ:	Convert a text char to de-junk equivalent.
*********************************************/

inline void Xml::de_junk_ch(
	_TCHAR* &xml,				// Pointer to last filled char in converted buffer.
	_TCHAR* &text				// Pointer to next char to convert in text buffer.
	)
{
switch (*text)
	{	

	case '\x80':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x81':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x82':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x83':	xstrcpy_e(xml, _T("f"));		++text;	break;	// // DEJUNK.
	case '\x84':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x85':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x86':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x87':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x88':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x89':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x8A':	xstrcpy_e(xml, _T("S"));		++text;	break;	// Scaron
	case '\x8B':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x8C':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x8D':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x8E':	xstrcpy_e(xml, _T("Z"));		++text;	break;	// // DEJUNK.
	case '\x8F':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.

	case '\x90':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x91':	xstrcpy_e(xml, _T("'"));		++text;	break;	// // DEJUNK.
	case '\x92':	xstrcpy_e(xml, _T("'"));		++text;	break;	// // DEJUNK.
	case '\x93':	xstrcpy_e(xml, _T("\""));		++text;	break;	// // DEJUNK.
	case '\x94':	xstrcpy_e(xml, _T("\""));		++text;	break;	// // DEJUNK.
	case '\x95':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x96':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x97':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x98':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x99':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x9A':	xstrcpy_e(xml, _T("?"));		++text;	break;	// scaron
	case '\x9B':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x9C':	xstrcpy_e(xml, _T("?"));		++text;	break;	// oelig
	case '\x9D':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x9E':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\x9F':	xstrcpy_e(xml, _T("Y"));		++text;	break;	// Yuml

	case '\xA0':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA1':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA2':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA3':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA4':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA5':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA6':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA7':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA8':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xA9':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xAA':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xAB':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xAC':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xAD':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xAE':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.
	case '\xAF':	xstrcpy_e(xml, _T("?"));		++text;	break;	// // DEJUNK.

	case '\xB0':	xstrcpy_e(xml, _T("?"));		++text;	break;	// degree	// DEJUNK.
	case '\xB1':	xstrcpy_e(xml, _T("?"));		++text;	break;	// plus-minus	// DEJUNK.
	case '\xB2':	xstrcpy_e(xml, _T("?"));		++text;	break;	// super 2	// DEJUNK.
	case '\xB3':	xstrcpy_e(xml, _T("?"));		++text;	break;	// super 3	// DEJUNK.
	case '\xB4':	xstrcpy_e(xml, _T("'"));		++text;	break;	// acute	// DEJUNK.
	case '\xB5':	xstrcpy_e(xml, _T("?"));		++text;	break;	// micro (mu)	// DEJUNK.
	case '\xB6':	xstrcpy_e(xml, _T("?"));		++text;	break;	// Pilcrow	// DEJUNK.
	case '\xB7':	xstrcpy_e(xml, _T("?"));		++text;	break;	// middle dot	// DEJUNK.
	case '\xB8':	xstrcpy_e(xml, _T("?"));		++text;	break;	// cedilla	// DEJUNK.
	case '\xB9':	xstrcpy_e(xml, _T("?"));		++text;	break;	// super 1	// DEJUNK.
	case '\xBA':	xstrcpy_e(xml, _T("?"));		++text;	break;	// Masculine o ord	// DEJUNK.
	case '\xBB':	xstrcpy_e(xml, _T("?"));		++text;	break;	// >>	// DEJUNK.
	case '\xBC':	xstrcpy_e(xml, _T("?"));		++text;	break;	// 1/4	// DEJUNK.
	case '\xBD':	xstrcpy_e(xml, _T("?"));		++text;	break;	// 1/2	// DEJUNK.
	case '\xBE':	xstrcpy_e(xml, _T("?"));		++text;	break;	// 3/4	// DEJUNK.
	case '\xBF':	xstrcpy_e(xml, _T("?"));		++text;	break;	// invert qmark	// DEJUNK.

	// Upper Case Latin-1 Letters
	case '\xC0':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Agrave
	case '\xC1':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Aacute
	case '\xC2':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Acirc
	case '\xC3':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Atilde
	case '\xC4':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Auml
	case '\xC5':	xstrcpy_e(xml, _T("A"));		++text;	break;	// Aring
	case '\xC6':	xstrcpy_e(xml, _T("E"));		++text;	break;	// AElig
	case '\xC7':	xstrcpy_e(xml, _T("C"));		++text;	break;	// Ccedil
	case '\xC8':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Egrave
	case '\xC9':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Eacute
	case '\xCA':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Ecirc
	case '\xCB':	xstrcpy_e(xml, _T("E"));		++text;	break;	// Euml
	case '\xCC':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Igrave
	case '\xCD':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Iacute
	case '\xCE':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Icirc
	case '\xCF':	xstrcpy_e(xml, _T("I"));		++text;	break;	// Iuml

	case '\xD0':	xstrcpy_e(xml, _T("?"));		++text;	break;	// eth	// DEJUNK.
	case '\xD1':	xstrcpy_e(xml, _T("N"));		++text;	break;	// Ntilde
	case '\xD2':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Ograve
	case '\xD3':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Oacute
	case '\xD4':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Ocirc
	case '\xD5':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Otilde
	case '\xD6':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Ouml
	case '\xD7':	xstrcpy_e(xml, _T("x"));		++text;	break;	// multiplication sign	// DEJUNK.
	case '\xD8':	xstrcpy_e(xml, _T("O"));		++text;	break;	// Oslash
	case '\xD9':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Ugrave
	case '\xDA':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Uacute
	case '\xDB':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Ucirc
	case '\xDC':	xstrcpy_e(xml, _T("U"));		++text;	break;	// Uuml
	case '\xDD':	xstrcpy_e(xml, _T("Y"));		++text;	break;	// Yacute
	case '\xDE':	xstrcpy_e(xml, _T("?"));		++text;	break;	// thorn	// DEJUNK.
	case '\xDF':	xstrcpy_e(xml, _T("?"));		++text;	break;	// German sharp S	// DEJUNK.
	
	// Lower Case Latin-1 Letters
	case '\xE0':	xstrcpy_e(xml, _T("a"));		++text;	break;	// agrave
	case '\xE1':	xstrcpy_e(xml, _T("a"));		++text;	break;	// aacute
	case '\xE2':	xstrcpy_e(xml, _T("a"));		++text;	break;	// acirc
	case '\xE3':	xstrcpy_e(xml, _T("a"));		++text;	break;	// atilde
	case '\xE4':	xstrcpy_e(xml, _T("a"));		++text;	break;	// auml
	case '\xE5':	xstrcpy_e(xml, _T("a"));		++text;	break;	// aring
	case '\xE6':	xstrcpy_e(xml, _T("e"));		++text;	break;	// aelig
	case '\xE7':	xstrcpy_e(xml, _T("c"));		++text;	break;	// ccedil
	case '\xE8':	xstrcpy_e(xml, _T("e"));		++text;	break;	// egrave
	case '\xE9':	xstrcpy_e(xml, _T("e"));		++text;	break;	// eacute
	case '\xEA':	xstrcpy_e(xml, _T("e"));		++text;	break;	// ecirc
	case '\xEB':	xstrcpy_e(xml, _T("e"));		++text;	break;	// euml
	case '\xEC':	xstrcpy_e(xml, _T("i"));		++text;	break;	// igrave
	case '\xED':	xstrcpy_e(xml, _T("i"));		++text;	break;	// iacute
	case '\xEE':	xstrcpy_e(xml, _T("i"));		++text;	break;	// icirc
	case '\xEF':	xstrcpy_e(xml, _T("i"));		++text;	break;	// iuml

	case '\xF0':	xstrcpy_e(xml, _T("?"));		++text;	break;	// eth	// DEJUNK.
	case '\xF1':	xstrcpy_e(xml, _T("n"));		++text;	break;	// ntilde
	case '\xF2':	xstrcpy_e(xml, _T("o"));		++text;	break;	// ograve
	case '\xF3':	xstrcpy_e(xml, _T("o"));		++text;	break;	// oacute
	case '\xF4':	xstrcpy_e(xml, _T("o"));		++text;	break;	// ocirc
	case '\xF5':	xstrcpy_e(xml, _T("o"));		++text;	break;	// otilde
	case '\xF6':	xstrcpy_e(xml, _T("o"));		++text;	break;	// ouml
	case '\xF7':	xstrcpy_e(xml, _T("?"));		++text;	break;	// division sign.	// DEJUNK.
	case '\xF8':	xstrcpy_e(xml, _T("o"));		++text;	break;	// oslash
	case '\xF9':	xstrcpy_e(xml, _T("u"));		++text;	break;	// ugrave
	case '\xFA':	xstrcpy_e(xml, _T("u"));		++text;	break;	// uacute
	case '\xFB':	xstrcpy_e(xml, _T("u"));		++text;	break;	// ucirc
	case '\xFC':	xstrcpy_e(xml, _T("u"));		++text;	break;	// uuml
	case '\xFD':	xstrcpy_e(xml, _T("y"));		++text;	break;	// yacute
	case '\xFE':	xstrcpy_e(xml, _T("?"));		++text;	break;	// thorn	// DEJUNK.
	case '\xFF':	xstrcpy_e(xml, _T("y"));		++text;	break;	// yuml

	default:		// All other chars.
		*++xml = *text++;
		break;
	}
}
