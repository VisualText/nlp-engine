/*******************************************************************************
Copyright (c) 2006-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	U_OUT.CPP
* FILE:	prim\u_out.cpp
* CR:		03/24/06 AM.
* SUBJ:	Conversions for output of Unicode to UTF-8 files.
* NOTE:	Override the standard << output operator.
*
*******************************************************************************/

#include "StdAfx.h"
#ifdef LINUX
#include <iostream>
#include <sstream>
#endif
#include "prim/libprim.h"
#include "u_out.h"
#include "prim/unicode.h"


#ifdef UNICODE


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

/**************************************************
*						GETUTF8
* FUN:	getutf8
* SUBJ:	Read UTF-8 char from file to Unicode in memory.
* NOTE:	Convenience function. May read multiple bytes.
* TODO:	Need to look for BOM.
* CR:		01/13/06 AM.
*
**************************************************/

LIBPRIM_API _TCHAR getutf8(
	_t_istream *fp
	)
{
_TCHAR ch = NULL;
int mb = fp->get();

if (fp->eof())
	return NULL;

if (mb <= 0x7F)						// Regular ascii text.
	return (_TCHAR) mb;

if ((mb & 0xE0) == 0xC0)				// 110xxxxx 10xxxxxx
	{
	ch = (mb & 0x1F) << 6;
	mb = fp->get();
	ch |= mb & 0x3F;
	}
else if ((mb & 0xF0) == 0xE0)		// 1110xxxx + 2
	{
	ch = (mb & 0x0F) << 12;
	mb = fp->get();
	ch |= (mb & 0x3F) << 6;
	mb = fp->get();
	ch |= mb & 0x3F;
	}
else if ((mb & 0xF1) == 0xF0)		// 11110xxx + 3 */
	{
	ch = (mb & 0x0F) << 18;
	mb = fp->get();
	ch |= (mb & 0x3F) << 12;
	mb = fp->get();
	ch |= (mb & 0x3F) << 6;
	mb = fp->get();
	ch |= mb & 0x3F;
	}
else if ((mb & 0xFD) == 0xF8)	// 111110xx + 4 */
	{
	ch = (mb & 0x0F) << 24;
	mb = fp->get();
	ch |= (mb & 0x0F) << 18;
	mb = fp->get();
	ch |= (mb & 0x3F) << 12;
	mb = fp->get();
	ch |= (mb & 0x3F) << 6;
	mb = fp->get();
	ch |= mb & 0x3F;
	}
else if ((mb & 0xFE) == 0xFC)	// 1111110x + 5 */
	{
	ch = (mb & 0x0F) << 30;
	mb = fp->get();
	ch |= (mb & 0x0F) << 24;
	mb = fp->get();
	ch |= (mb & 0x0F) << 18;
	mb = fp->get();
	ch |= (mb & 0x3F) << 12;
	mb = fp->get();
	ch |= (mb & 0x3F) << 6;
	mb = fp->get();
	ch |= mb & 0x3F;
	}
else
	return NULL;	// Error.

return ch;
}


/**************************************************
*						U_READBOM
* FUN:	u_readbom
* SUBJ:	Open a file for reading as UTF-8.
* NOTE:	Checking on the various Unicode file formats.
* CR:		01/14/06 AM.
* RET:	Read BOM chars if any, at head of file.
*
**************************************************/

/*
Bytes Encoding Form 
00 00 FE FF UTF-32, big-endian 
FF FE 00 00 UTF-32, little-endian 
FE FF UTF-16, big-endian 
FF FE UTF-16, little-endian 
EF BB BF UTF-8
*/
LIBPRIM_API bool u_readbom(_t_ifstream *fp)
{
if (!fp)
	return false;
short ch = fp->get();
switch (ch)
	{
	case 0xEF:
		if (fp->get() != 0xBB)
			break;
		if (fp->get() != 0xBF)
			break;
		// Got UTF-8 BOM header.
		// KEEP CURRENT POSITION IN STREAM.
		return true;
		break;
	case 0xFF:
		if (fp->get() != 0xFE)
			break;
		// Got another BOM.  Unhandled here.
		return false;
		break;
	case 0xFE:
		if (fp->get() != 0xFF)
			break;
		// Got another BOM.  Unhandled here.
		return false;
		break;
	case 0x00:
		if (fp->get() != 0x00)
			break;
		if (fp->get() != 0xFE)
			break;
		if (fp->get() != 0xFF)
			break;
		// Got another BOM. Unhandled.
		return false;
		break;
	default:
		break;
	}
// No BOM found at head of file.
fp->seekg(0);	// RESET input stream to beginning.
return true;
}


#endif
