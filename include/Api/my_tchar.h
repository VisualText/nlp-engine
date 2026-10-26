#ifndef NLP_INCLUDE_API_MY_TCHAR_H
#define NLP_INCLUDE_API_MY_TCHAR_H

/*******************************************************************************
Copyright (c) 2006-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// my_tchar.h
#include <string>
#include <iostream>
#include <sstream>

#ifdef  _UNICODE

#define _t_cin				wcin
#define _t_cout				wcout
#define _t_cerr				wcerr
#define _t_istream			wistream
#define _t_ostream			wostream
#define _t_ifstream			wifstream
#define _t_ofstream			wofstream
#define _t_strstream		wstringstream
#define _t_ostrstream		wostringstream

#define _t_clog				wclog
#define _t_ios				wios
#define _t_streambuf		wstreambuf
#define _t_iostream			wiostream
#define _t_stringbuf		wstringbuf
#define _t_istringstream	wistringstream
#define _t_ostringstream	wostringstream
#define _t_stringstream		wstringstream
#define _t_filebuf			wfilebuf
#define _t_fstream			wfstream

#else

#define _t_cin				cin
#define _t_cout				cout
#define _t_cerr				cerr
#define _t_clog				clog
#define _t_ios				ios
#define _t_streambuf		streambuf
#define _t_istream			istream
#define _t_ostream			ostream
#define _t_iostream			iostream
#define _t_stringbuf		stringbuf
#define _t_istringstream	istringstream
#define _t_ostringstream	ostringstream
#define _t_stringstream		stringstream
#define _t_filebuf			filebuf
#define _t_ifstream			ifstream
#define _t_ofstream			ofstream
#define _t_fstream			fstream

#ifdef LINUX
// Changing to use sstream
//#define _t_strstream		ostringstream
//#define _t_ostrstream		ostringstream
// THIS IS THE WINDOWS SIDE.  KEEP IT	// 09/27/19 AM.
#define _t_strstream		ostringstream
#define _t_ostrstream		ostringstream  // 11/17/20 AM.
#define _t_istrstream       istringstream  // 11/17/20 AM.


#define CP_UTF8	65001
#define _TCHAR char
#define _T
#define LPCTSTR char *
//#define LPCWSTR char *
// 3/3/16 AM. Supposedly empty declaration.
// 04/21/16 AM. Some complaints in lite, need this.
// #define UINT long	// 09/26/19 AM.

#define lstrlen		strlen

#define _TUCHAR		unsigned char
#define _tasctime	asctime
#define _tcsftime	strftime
#define _tremove	remove
#define _tcscat		strcat
#define _tcschr		strchr
#define _tcsrchr	strrchr
// strstr had never been mapped here, so _tcsstr resolved only via the real
// <tchar.h> on Windows and failed to compile on Linux and macOS.	// 08/03/26 DD.
#define _tcsstr		strstr
#define _tcscmp		strcmp
#define _tcscpy		strcpy
#define _tcslen		strlen
#define _tcsicmp	stricmp
#define _tcsncmp	strncmp
#define _tcsclen	strlen
#define _tcsnccpy	strncpy
#define _tgetenv	getenv
#define _ttoi		atoi
#define _tstof		atof		// nlp -TIMELIMIT seconds.	// 10/25/26 DD.
#define _tsystem	system
#define _stprintf	sprintf
#define _ftprintf	fprintf
// _istspace, _totupper etc. are defined, ASCII-only, at the end of this file.

#else
#define _t_strstream		strstream
#define _t_ostrstream		ostringstream  // 09/27/19 AM.
#define _t_istrstream       istringstream  // 11/17/20 AM.

#endif

#endif

// ASCII-only character tests.	// 10/12/26 DD.
// The engine works on UTF-8 bytes, so these tests must never depend on the
// C locale. isspace() and friends do: under a single-byte code page such as
// Windows-1252, isspace(0xA0) is true, and clean_str() then split "Voilà"
// (C3 A0) and "오전" (... EC A0 84) mid-character. That happened whenever a
// host process (e.g. Python) had called setlocale(LC_CTYPE, ""). Bytes 0x80
// and above are never letters, spaces or case-mapped here, which matches what
// nlp.exe has always done in the default "C" locale.
// On Windows, <tchar.h> is pulled in first so its own _istspace etc. are
// replaced, and a later #include <tchar.h> is a no-op.
#ifndef _UNICODE

#ifndef LINUX
#include <tchar.h>
#endif

inline int nlp_isupper(int c) { return c >= 'A' && c <= 'Z'; }
inline int nlp_islower(int c) { return c >= 'a' && c <= 'z'; }
inline int nlp_isalpha(int c) { return nlp_isupper(c) || nlp_islower(c); }
inline int nlp_isdigit(int c) { return c >= '0' && c <= '9'; }
inline int nlp_isalnum(int c) { return nlp_isalpha(c) || nlp_isdigit(c); }
inline int nlp_iscntrl(int c) { return (c >= 0 && c < 0x20) || c == 0x7F; }
inline int nlp_isspace(int c) { return c == ' ' || (c >= '\t' && c <= '\r'); }
inline int nlp_ispunct(int c)
	{ return c > 0x20 && c < 0x7F && !nlp_isalnum(c); }
inline int nlp_toupper(int c) { return nlp_islower(c) ? c - 'a' + 'A' : c; }
inline int nlp_tolower(int c) { return nlp_isupper(c) ? c - 'A' + 'a' : c; }

#undef _istalpha
#undef _istalnum
#undef _istcntrl
#undef _istdigit
#undef _istlower
#undef _istpunct
#undef _istspace
#undef _istupper
#undef _totlower
#undef _totupper

#define _istalpha	nlp_isalpha
#define _istalnum	nlp_isalnum
#define _istcntrl	nlp_iscntrl
#define _istdigit	nlp_isdigit
#define _istlower	nlp_islower
#define _istpunct	nlp_ispunct
#define _istspace	nlp_isspace
#define _istupper	nlp_isupper
#define _totlower	nlp_tolower
#define _totupper	nlp_toupper

#endif

#endif // NLP_INCLUDE_API_MY_TCHAR_H
