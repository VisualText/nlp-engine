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
#define _tcscmp		strcmp
#define _tcscpy		strcpy
#define _tcslen		strlen
#define _tcsicmp	stricmp
#define _tcsncmp	strncmp
#define _tcsclen	strlen
#define _tcsnccpy	strncpy
#define _tgetenv	getenv
#define _ttoi		atoi
#define _tsystem	system
#define _stprintf	sprintf
#define _ftprintf	fprintf
#define _totlower	tolower
#define _totupper	toupper

#define _istalpha	isalpha
#define _istalnum	isalnum
#define _istcntrl	iscntrl
#define _istdigit	isdigit
#define _istlower	islower
#define _istpunct	ispunct
#define _istspace	isspace
#define _istupper	isupper

#else
#define _t_strstream		strstream
#define _t_ostrstream		ostringstream  // 09/27/19 AM.
#define _t_istrstream       istringstream  // 11/17/20 AM.

#endif

#endif
