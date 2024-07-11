/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IO.CPP
* FILE:	lite\io.cpp
* CR:		10/06/98 AM.
* SUBJ:	I/O functions.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"		// 03/09/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "prim/libprim.h"	// 01/18/06 AM.
#include "prim/str.h"		// 01/18/06 AM.

#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "std.h"
#include "chars.h"
#include "io.h"


/********************************************
*
* FN:		COPY_FILE
* CR:		10/06/98 AM.
* SUBJ:	Copy input file to output file.
* NOTE:	Presumably doesn't matter if file is unicode or not.
*			A char is a char in any case.
*
********************************************/

void copy_file(const _TCHAR *iname, const _TCHAR *oname)
{

//ifstream inFile(iname, std::ios::in | std::ios::nocreate);
std::_t_ifstream inFile(CTCHAR2CA(iname), std::ios::in);							// Upgrade.	// 01/24/01 AM.
if (!inFile)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Could not open input file '") << iname << _T("'.") << std::ends;
	errOut(&gerrStr,false);
	return;					// 06/15/99 AM.
	}

std::_t_ofstream outFile(CTCHAR2CA(oname), std::ios::out);
if (!outFile)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Could not open output file '") << oname << _T("'.") << std::ends;
	errOut(&gerrStr,false);
	return;					// 06/15/99 AM.
	}

// Echo input file to output.
// << and >> ignore whitespace!
_TCHAR ch;

// Changing handling of EOF.												// 12/16/01 AM.
//while ((ch = inFile.get()) != EOF)									// 12/16/01 AM.
//	outFile.put(ch);															// 12/16/01 AM.
for (;;)																			// 12/18/01 AM.
	{
	ch = inFile.get();														// 12/18/01 AM.
	if (inFile.eof())															// 12/18/01 AM.
		break;																	// 12/18/01 AM.
	outFile.put(ch);															// 12/18/01 AM.
	}
}


/********************************************
*
* FN:		FILE_EXISTS
* CR:		12/14/98 AM.
* SUBJ:	Check if file exists.
********************************************/

bool file_exists(const _TCHAR *iname)
{

//ifstream inFile(iname, std::ios::in | std::ios::nocreate);
std::_t_ifstream inFile(CTCHAR2CA(iname), std::ios::in);
return (inFile ? true : false);
}


/********************************************
*
* FN:		FILE_TO_BUFFER
* CR:		10/08/98 AM.
* SUBJ:	Copy input file to a buffer.
* NOTE:	Presumably doesn't matter if file is unicode or not.
*			A char is a char in any case.
*			05/28/00 AM. Length of good chars may not be equivalent to file
*			size obtained from system, so keep a count here and return it.
* ASS:	Assume buffer is large enough.  Assume buffer was allocated
*			using length of the file.
*
********************************************/

void file_to_buffer(const _TCHAR *iname, _TCHAR *buf,
	/*UP*/ long &len															// 05/28/00 AM.
	)
{

//ifstream inFile(iname, std::ios::in | std::ios::nocreate);
std::_t_ifstream inFile(CTCHAR2CA(iname), std::ios::in);							// Update.	// 01/24/01 AM.
if (!inFile)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Could not open input file '") << iname << _T("'.") << std::ends;
	errOut(&gerrStr,false);
	//exit(1);			// 06/15/99 AM.
	return;				// 06/15/99 AM.
	}

//char ch;																		// 12/16/01 AM.
_TCHAR *ptr=buf;																	// 05/28/00 AM.

// Better EOF handling etc.												// 12/16/01 AM.
//while ((ch = inFile.get()) != EOF)									// 12/16/01 AM.
//	*buf++ = ch;																// 12/16/01 AM.
//*buf = '\0';			// Terminate buffer.								// 12/16/01 AM.

--buf;	// For efficiency.												// 12/16/01 AM.
while (!inFile.eof())														// 12/16/01 AM.
	*++buf = inFile.get();													// 12/16/01 AM.
*buf = '\0';	// Terminate buffer (AND OVERWRITE EOF CHAR)		// 12/16/01 AM.

len = (unsigned long)buf - (unsigned long)ptr + 1;					// 05/28/00 AM.
}

/********************************************
* FN:		FIX_FILE_NAME
* CR:		10/23/98 AM.
* SUBJ:	Add the file name extension if absent.
* WARN:	MODIFIES GIVEN FILE NAME BUFFER.
*			IF ANY EXTENSION PRESENT, DOES NOT MODIFY FILE NAME.
* ASS:	BUFFER MUST BE BIG ENOUGH FOR ADDED EXTENSION, IF MISSING.
* Note:	May want a string handling class to do some of the ops here.
********************************************/

bool fix_file_name(
	_TCHAR *file,				// Buffer big enough to hold extension.
	_TCHAR *suff				// File name extension needed.
	)
{
if (empty(file))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[fix_file_name: Given no file name.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
if (empty(suff))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[fix_file_name: Given no file name extension.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

_TCHAR *ptr;

// 12/03/98 AM. Go backward, finding backslash or period, whichever comes
// first.
ptr = file;
while (*ptr++)
	;					// Go to forward to end of buffer.
ptr--;				// Back to end of string.

while (--ptr != file)	// Go backward now.
	{
	if (*ptr == '.')		// Found extension.
		{
		// Not modifying extension, since one is present.
		// Here's a check for some abnormal filenames, though.
		if (ptr == file || !*++ptr)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Bad file name='") << file << _T("'.]") << std::ends;
			errOut(&gerrStr,false);
			return false;
			}
		return true;			// 12/03/98 AM.
		}
	if (*ptr == DIR_CH)		// Found backslash.  Stop looking for period.
		break;
	}

// Add extension to file name.
_tcscat(file, _T("."));
_tcscat(file, suff);
return true;
}


/********************************************
* FN:		PRETTY_CHAR
* CR:		10/09/98 AM.
* SUBJ:	Get printable form for a character.
* NOTE:	User had better copy string immediately.
********************************************/


_TCHAR *pretty_char(_TCHAR x)
{
static _TCHAR twofer[2] = { '\0', '\0' };

switch (x)
	{
	case '\a':	return _T("\\a");
	case '\b':	return _T("\\b");
	case '\n':	return _T("\\n");
	case '\r':	return _T("\\r");
	case '\f':	return _T("\\f");
	case '\t':	return _T("\\t");
	case '\v':	return _T("\\v");
	case '\\':	return _T("\\\\");			// 11/24/98 AM.
	case '\0':	return _T("\\0");			// 11/18/98 AM.
	case ' ':	return _T("\\_");			// 03/24/99 AM.
	default:		twofer[0] = x; return twofer;
	}
}


/********************************************
* FN:		PRETTY_STR
* CR:		11/24/98 AM.
* SUBJ:	Get printable form for a string.
* NOTE:	User had better copy string immediately.
* RET:	User-supplied buffer for cleaned up string.
********************************************/

_TCHAR *pretty_str(
	_TCHAR *str,				// String to be prettified.
	_TCHAR *buf,				// Buffer for placing prettified string.
	long size				// Buffer size.
	)
{
*buf = '\0';
_TCHAR *end;
end = &(buf[0]);

for (; *str; ++str)		// Traverse string.
	{
	if (!strcat_e(end, pretty_char(*str), size))
		return buf;
	}
return buf;
}

/********************************************
* FN:		READ_FILE
* CR:		10/13/98 AM.
* SUBJ:	Read contents of a file into a buffer.
* ALLOC:	Creates buffer that must be freed.
********************************************/

void read_file(
	_TCHAR *fname,		// The filename
	/*UP*/
	long &len,			// Length of file.
	_TCHAR* &buf			// Buffer to create.
	)
{
// Get file length.

len = file_size(fname);
buf = 0;
if (len <= 0)
	return;
++len;		// Add one for null termination.							// 05/28/00 AM.

// Allocate a buffer to hold all the chars of file.
buf = Chars::create(len);													// 11/19/98 AM.
			// Don't include null terminator in count.				// 05/28/00 AM.

file_to_buffer(fname, buf,		// Suck in the file.
	/*UP*/ len);	// Use count from examining chars of file.	// 05/28/00 AM.
}



/********************************************
* FN:		NEXT_TOKEN
* CR:		10/13/98 AM.
* SUBJ:	Find next token on current line in buffer.
* NOTE:	Assumes whitespace, newlines and end of buffer are only delimiters.
*			buf is at lookahead char.  eol tells if end of current line seen.
*			buf returned null only at end of buffer.
*			Should also skip # signs!
*			A SPECIALIZED FUNCTION.
* WARN:	MODIFIES GIVEN BUFFER.  Places NULLS for token delimiters.
********************************************/

_TCHAR *next_token(
	/*DU*/
	_TCHAR* &buf,
	bool &eol,
	_TCHAR *comment			// Buffer for storing comment.	// 01/13/99 AM.
	)
{
_TCHAR *first;	// First char of next token.

if (!buf || !*buf)
	{
	eol = true;
	return buf;
	}

// Skip whitespace.
bool found = false;			// 09/02/99 AM.
while (_istspace((_TUCHAR)*buf) && *buf != '\n')
	++buf;

if (*buf == '\n')		// At end of current line.
	{
	eol = true;
	++buf;
	return 0;
	}
else if (*buf == '#')	// Comment
	{
	found = false;			// Haven't found nonwhite yet.	// 09/02/99 AM.
	while (*++buf && *buf != '\n'			// Find end of line.
				&& *buf != '\r')					// 02/12/99 AM.
		{
		// 09/02/99 AM. SKIP LEADING WHITESPACE IN COMMENT!
		if (found)
			*comment++ = *buf;						// 01/15/99 AM.
		else if (!_istspace((_TUCHAR)*buf))	// IGNORE LEADING WHITE. // 09/02/99 AM.
			{
			*comment++ = *buf;
			found = true;				// Found first nonwhite.	// 09/02/99 AM.
			}
		}
	*comment = '\0';								// 01/15/99 AM.
	eol = true;
	if (!*buf)
		return 0;
	++buf;	// One past newline.
	return (_TCHAR *) NULL;
	}

first = buf++;			// Found first char of next token.

while (*buf && !_istspace((_TUCHAR)*buf))	// Find next whitespace.
	++buf;															// OPT.	// 11/29/00 AM.

if (!*buf)				// End of buffer.
	{
	eol = true;
	return first;
	}

if (*buf == '\n')
	eol = true;

*buf = '\0';		// MODIFY BUFFER TO TERMINATE TOKEN.
++buf;
return first;
}


/********************************************
* FN:		EQ_STR_RANGE
* CR:		10/26/98 AM.
* SUBJ:	See if string matches character range.
* NOTE:	Defining "range" as a char substring with a start and end offset.
*			start and end are inclusive.
********************************************/

bool eq_str_range(_TCHAR *str, _TCHAR *buf, long start, long end)
{
if (empty(str) && empty(buf))		// Both empty.
	return true;
if (empty(str) || empty(buf))		// Exactly one is empty.
	return false;
if (start < 0 || end < 0 || end < start)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[eq_str_range: Bad start, end=")
		  << start << _T(",") << end << _T(".]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

_TCHAR *ptr;
_TCHAR *eptr;
ptr = &(buf[start]);
eptr = &(buf[end]);
if (!*eptr)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[eq_str_range: Endpoint shouldn't be null.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

while (*str && (*str == *ptr) && (ptr != eptr))
	{
	++str;
	++ptr;
	}

if (*str && (*str == *ptr) && (ptr == eptr))	// Exhausted range.
	return *++str ? false : true;		// If str exhausted, then equal.
return false;
}


/********************************************
* FN:		MAKE_STR
* CR:		10/28/98 AM.
* SUBJ:	Create a new string.
* ALLOC:	Creates a new char array.
* NOTE:	Doesn't create array for empty string.
********************************************/

_TCHAR *make_str(_TCHAR *str)
{
if (empty(str))
	return 0;
_TCHAR *tmp;
//tmp = new char[strlen(str) + 1];
tmp = Chars::create(_tcsclen(str) + 1);
_tcscpy(tmp, str);
return tmp;
}

/********  VARIANT ***********/

_TCHAR *make_str(
	_TCHAR *str,				// Substring to copy.
	long len					// Length of substring.
	)
{
if (empty(str) || len <= 0)
	return 0;

_TCHAR *tmp;
//tmp = new char[len + 1];
tmp = Chars::create(len + 1);
_tcsnccpy(tmp, str, len);
tmp[len] = '\0';
return tmp;
}


/********************************************
* FN:		STRCAT_E
* CR:		11/20/98 AM.
* SUBJ:	Catenate strings.
* NOTE:	Useful for multiple catenation.
*			Handling the count with termination char is tricky.
*			More straightforward would require the user to call this with
*			buffer size - 1.
* RET:	DU ptr - points to end of the newly catenated string.
*			DU count < 0 then not using this.  Else it tracks space left
*			in catenation buffer (pointed into by ptr).
********************************************/

bool strcat_e(
	/*DU*/ _TCHAR* &ptr,
	/*DN*/ _TCHAR *str,
	/*DU*/ long &count
	)
{
if (empty(str))
	return true;			// Nothing to do.

if (count == 0 || count == 1)
	{
	// Note: count == 1 means that the terminator has filled buffer.
	std::_t_strstream gerrStr;
	gerrStr << _T("[strcat_e: String overflow.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

if (count > 0)
	{
	while ((*ptr++ = *str++))
		{
		if (--count == 0)
			break;
		}
	if (*(ptr-1))			// Didn't complete copying.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[strcat_e: String overflow(2).]") << std::ends;
		errOut(&gerrStr,false);

		*(ptr-1) = '\0';	// Truncate the string in buffer.
		return false;
		}
	// DON'T COUNT THE TERMINATION CHAR.
	--ptr;					// Back up to termination char.
	}
else							// Not checking on buffer size.
	{
	while ((*ptr++ = *str++))
		;
	--ptr;					// Back up to termination char.
	}
return true;
}


/********************************************
* FN:		STRNCAT_E
* CR:		11/20/98 AM.
* SUBJ:	Catenate strings.
* NOTE:	Useful for multiple catenation.
* RET:	DU ptr - points to end of the newly catenated string.
*			DU count < 0 then not using this.  Else it tracks space left
*			in catenation buffer (pointed into by ptr).
********************************************/

bool strncat_e(
	/*DU*/ _TCHAR* &ptr,	// Pointer to terminal char in buffer.
	/*DN*/ _TCHAR *str,		// String to add.
	/*DN*/ long len,		// String length.
	/*DU*/ long &count	// Space left in buffer.
	)
{
if (empty(str) || (len <= 0))
	return true;			// Nothing to do.

if (count == 0 || count == 1)
	{
	// Note: count == 1 means that the terminator has filled buffer.
	std::_t_strstream gerrStr;
	gerrStr << _T("[strncat_e: String overflow.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

if (count > 0)				// Tracking space left in buffer.
	{
	if (len >= count)
		{
		// Note: len >= count, because one space needed for terminator.
		std::_t_strstream gerrStr;
		gerrStr << _T("[strncat_e: String overflow(1).]") << std::ends;
		errOut(&gerrStr,false);

		// Could recover by filling buffer to the max.
		// len = count - 1;
		// count = 0;
		return false;
		}

	// DON'T COUNT THE TERMINATION CHAR!
	count -= len;
	}

while (len--)
	*ptr++ = *str++;
*ptr = '\0';		// Terminate the buffer.

return true;
}


/********************************************
* FN:		FILE_NAME
* CR:		12/24/99 AM.
* SUBJ:	Given a file, get its name without path.
* WARN:	MODIFIES GIVEN FILE NAME BUFFER.
*			(Not really, but some related fns do.)
********************************************/

bool file_name(
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fname			// Pointer to the name in buffer.
	)
{
fname = 0;
if (!file || !*file)
	return false;

_TCHAR *ptr;

ptr = file;
while (*ptr++)
	;					// Go to forward to end of buffer.
ptr--;				// Back to end of string.

// Go backward, finding backslash if any.
while (--ptr != file)
	{
	if (*ptr == DIR_CH)		// Found backslash.
		break;
	}
fname = ++ptr;		// Points to filename, if any, now.
return true;
}


/********************************************
* FN:		FILE_PATH
* CR:		12/24/99 AM.
* SUBJ:	Given a file, get its path without name.
* RET:	Path without final backslash.
* WARN:	MODIFIES GIVEN FILE NAME BUFFER.
********************************************/

bool file_path(
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fpath			// Pointer to the path in buffer.
	)
{
fpath = 0;
if (!file || !*file)
	return false;

_TCHAR *ptr;

ptr = file;
while (*ptr++)
	;					// Go to forward to end of buffer.
ptr--;				// Back to end of string.

// Go backward, finding backslash if any.
while (--ptr != file)
	{
	if (*ptr == DIR_CH)		// Found backslash.
		{
		*ptr = '\0';		// Terminate path.
		fpath = file;		// Return pointer to path.
		return true;
		}
	}
// Even if first char is backslash, return null path.
return true;
}


/********************************************
* FN:		FILE_PARENT
* CR:		11/16/22
* SUBJ:	Given a file, get its parent directory name (NOT PATH)
* RET:	Parent directory name
* WARN:	MODIFIES GIVEN FILE NAME BUFFER.
********************************************/

bool file_parent(
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fparent			// Pointer to the path in buffer.
	)
{
	fparent = 0;
	if (!file || !*file)
		return false;

	_TCHAR *ptr;

	ptr = file;
	while (*ptr++)
		;					// Go to forward to end of buffer.
	ptr--;				// Back to end of string.

	// Go backward, finding backslash if any.
	int slashCount = 0;
	while (--ptr != file) {
		if (*ptr == DIR_CH) {
			slashCount++;
			if (slashCount == 1) {
				*ptr = '\0';
			}
			if (slashCount >= 2) {
				fparent = ++ptr;
				return true; 
			}
		}
	}
	// Even if first char is backslash, return null path.
	return true;
}


/********************************************
* FN:		FILE_HEAD
* CR:		12/24/99 AM.
* SUBJ:	Given a file, get its filename head.
* RET:	File name without extension.
* WARN:	MODIFIES GIVEN FILE NAME BUFFER.
********************************************/

bool file_head(
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fhead			// Pointer to the file head in buffer.
	)
{
fhead = 0;
if (!file || !*file)
	return false;

_TCHAR *ptr;

ptr = file;
while (*ptr++)
	;					// Go to forward to end of buffer.
ptr--;				// Back to end of string.

// Go backward, finding period if any.
while (--ptr != file)
	{
	if (*ptr == '.')		// Found period.
		break;
	if (*ptr == DIR_CH)		// Found backslash first.
		break;
	}

if (*ptr == DIR_CH)			// Found backslash first.
	{
	fhead = ++ptr;			// No extension in file string.
	return true;
	}

if (ptr == file)			// At beginning of file string.
	{
	if (*ptr == '.')		// One period that starts file string.
		return false;		// Return empty file head.
	// Found no period or backslash, so file is head with no extension.
	fhead = file;
	return true;
	}

// Must be at a period, not at beginning of file.
if (*ptr != '.' || ptr == file)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[file_head: Error.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

*ptr = '\0';				// Terminate file head.	MODIFIES BUFFER.

// Look for backslash of beginning of file.
while (--ptr != file)
	{
	if (*ptr == DIR_CH)		// Found backslash.
		{
		fhead = ++ptr;
		return true;		// Got file head.
		}
	}

// Got to beginning of file without finding a backslash.
fhead = file;
return true;
}



/********************************************
* FN:		FILE_TAIL
* CR:		12/24/99 AM.
* SUBJ:	Given a file, get its filename extension.
* RET:	Filename extension.
********************************************/

bool file_tail(
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &ftail			// Pointer to tail in buffer.
	)
{
ftail = 0;
if (!file || !*file)
	return false;

_TCHAR *ptr;

ptr = file;
while (*ptr++)
	;					// Go to forward to end of buffer.
ptr--;				// Back to end of string.

// Go backward, finding period if any.
while (--ptr != file)
	{
	if (*ptr == '.')		// Found the last period.
		{
		ftail = ++ptr;
		return true;
		}
	if (*ptr == DIR_CH)		// Found backslash first.
		{
		// No file tail (ie, extension).
		return false;
		}
	}
// No period or backslash in file.
return false;
}


/********************************************
* FN:		C_CHAR
* CR:		05/10/00 AM.
* SUBJ:	Get printable form for a character.
* NOTE:	User had better copy string immediately.
*			Like pretty_char, but \_ is not a good C/C++ char.
*	01/18/06 AM. Get rid of static buffer while I'm at it.
********************************************/


_TCHAR *c_char(
	_TCHAR x,
	_TCHAR *buf	// 01/18/06 AM.
	)
{
//static _TCHAR twofer[2] = { '\0', '\0' };	// 01/18/06 AM.

switch (x)
	{
	case '\a':	return _T("\\a");
	case '\b':	return _T("\\b");
	case '\n':	return _T("\\n");
	case '\r':	return _T("\\r");
	case '\f':	return _T("\\f");
	case '\t':	return _T("\\t");
	case '\v':	return _T("\\v");
	case '\\':	return _T("\\\\");
	case '\0':	return _T("\\0");
	case '"':	return _T("\\\"");								// 06/05/00 AM.
	default:
		{
		// twofer[0] = x; return twofer;	// 01/18/06 AM.
#ifndef UNICODE
		buf[0] = x;								// 01/18/06 AM.
		buf[1] = '\0';							// 01/18/06 AM.
#else
//		if ((long) x < 128)					// 01/18/06 AM.
		if ((long) x < 255)					// 01/18/06 AM.
			{
			buf[0] = x;
			buf[1] = '\0';
			}
		else
			{
			buf[0] = '\\';
			buf[1] = 'x';
			num_to_hex((long)x,/*DU*/&(buf[2]));// 01/18/06 AM.
			}
#endif
		return buf;								// 01/18/06 AM.
		}
	}
}


/********************************************
* FN:		C_STR
* CR:		05/10/00 AM.
* SUBJ:	Get printable form for a string.
* NOTE:	User had better copy string immediately.
* RET:	User-supplied buffer for cleaned up string.
********************************************/

_TCHAR *c_str(
	_TCHAR *str,				// String to be prettified.
	_TCHAR *buf,				// Buffer for placing prettified string.
	long size				// Buffer size.
	)
{
*buf = '\0';
_TCHAR *end;
end = &(buf[0]);

_TCHAR chbuf[32];	// 01/18/06 AM.
for (; *str; ++str)		// Traverse string.
	{
	if (!strcat_e(end, c_char(*str,chbuf), size))
		return buf;
	}
return buf;
}

/******************************************************************************/
