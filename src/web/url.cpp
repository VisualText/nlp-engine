/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	URL.CPP
* FILE:	c:\dev\nlplite\url.cpp
* CR:		02/11/99 AM.
* SUBJ:	Manipulators for URLs.
*
*******************************************************************************/

#include "stdafx.h"
/*
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
*/

#include <iostream>											// Upgrade.	// 01/24/01 AM.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "web.h"
#include "url.h"

// Char for directory in a path.
// In UNIX, it's a forward slash.
#ifndef UNIX
#define DIR_CH  '\\'
#else
#define DIR_CH  '/'
#endif

/********************************************
* FN:		URL_TO_FNAME
* CR:		02/11/99 AM.
* SUBJ:	Build a filename from URL.
* NOTE:	If it looks like http://www.xxx, then traverse
*			from the xxx to the next period.  If it looks
*			like http://xxx. then traverse from xxx to the
*			next period.
*			This is by no means a foolproof naming scheme.
*			At some point, we may want to build internal
*			names for files, to assure uniqueness.
*			Somewhat crude way to get a file for a single URL from a
*			website.  User should check if filename already exists.
*			url_to_path will be more principled.
* EX:		http://www.textanalysis.com/index.htm => textanalysis.htm
********************************************/

bool url_to_fname(
	const _TCHAR *url,
	_TCHAR *fname			// Buffer for filename.
	)
{
*fname = '\0';
const _TCHAR *ptr = 0;
const _TCHAR *domain = 0;
const _TCHAR *lastdot = 0;

int len = 0;	// 05/05/14 AM.

if (!url || !*url)
	return false;

if (!_tcsncicmp(url, _T("http://"), 7))	// 05/05/14 AM.
	{
	len = 7;	// 05/05/14 AM.
	}
if (!_tcsncicmp(url, _T("https://"), 8))	// 05/05/14 AM.
	{
	len = 8;	// 05/05/14 AM.
	}
else
	return false;

// Start past the http:// header.
// Or https:	// 05/05/14 AM.
ptr = url + len;	// 05/05/14 AM.

if (!*ptr)
	return false;

if (!_tcsncicmp(ptr, _T("www."), 4))
	ptr += 4;

if (!*ptr)
	return false;

// Traverse to end, period, or slash, whichever comes first.
domain = ptr;				// Start of domain name.
while (*ptr && *ptr != '/' && *ptr != '.')
	++ptr;
if (ptr == domain)
	return false;			// Empty domain name.
// --ptr;			// Back up to last char of name.

// Copy the domain name to buffer.
_TCHAR *end = 0;
end = fname;
while ((*end++ = *domain++) && (domain != ptr))
	;

// Append a .HTM ending to the name.
// Terminate the name.
_tcscpy(end, _T(".htm"));

_t_cout << _T("[FILE NAME=") << fname << _T("]") << endl;

return true;
}
/********************************************
* FN:		URL_TO_PATH
* CR:		03/02/99 AM.
* SUBJ:	Build a filename path from URL.
* NOTE:	More principled than url_to_fname.  To be used for fetching
*			an entire website.
* EX:		http://www.xyz.com/index.htm =>
*						www_xyz_com\index.htm
*			http://www.xyz.com  =>
*						www_xyz_com\www_xyz_com.htm
*			http://www.xyz.com/xxx/index.htm  =>
*						www_xyz_com\xxx\index.htm
********************************************/

bool url_to_path(
	const _TCHAR *url,
	/*UP*/
	_TCHAR *path,
	_TCHAR *dir,			// Buffer for directory path.
	_TCHAR *fname			// Buffer for filename only.
	)
{
*fname = *path = *dir = '\0';
const _TCHAR *uptr = 0;
_TCHAR *ptr = 0;

if (!url || !*url)
	return false;

if (_tcsncicmp(url, _T("http://"), 7))
	return false;

// Start past the http:// header.
uptr = url + 7;

if (!*uptr)
	return false;

// Read till slash, converting periods to underscores.
_TCHAR site_dir[256];
_TCHAR *tmp = 0;
tmp = site_dir;
while (*uptr && *uptr != '/')
   {
	if (*uptr != '.')
		*tmp++ = *uptr;
	else
		*tmp++ = '_';
	++uptr;
	}
*tmp = '\0';			// Terminate.

if (!*uptr || (*uptr == '/' && !*(uptr+1)))
	{
	// Domain name only.
	// Build a default filename.
	// eg, "www_x_com\www_x_com.htm".
	_stprintf(dir, _T("%s"), site_dir);
	_stprintf(fname, _T("%s.htm"), site_dir);
	_stprintf(path, _T("%s%c%s"), dir, DIR_CH, fname);
	return true;
	}

if (*uptr != '/')
	return false;			// Error.

_TCHAR *lastslash = (tmp-1);

// Set up top-level dir.
_TCHAR *tmp1;
tmp = dir;
tmp1 = site_dir;
while (*tmp++ = *tmp1++)
	;
*(tmp-1) = DIR_CH;	// Replace terminator with slash.
lastslash = tmp-1;

// Gather up subdirectories.
++uptr;
while (*uptr)
	{
	if (*uptr == '/')
		{
		lastslash = tmp;
		*tmp++ = DIR_CH;
		}
	else
		*tmp++ = *uptr;
	++uptr;
	}
*tmp = '\0';		// Terminate dir (or fname!).
ptr = lastslash;
*ptr++ = '\0';		// Terminate the directory.

if (!*ptr)
	{
	// No filename.
	// Not handling URL that refers to a directory.
	return false;
	}

// Now, if there's a filename, we've glommed that also.
// Should probably check for valid extension.
tmp = fname;
while (*tmp++ = *ptr++)
	;

_stprintf(path, _T("%s%c%s"), dir, DIR_CH, fname);

_t_cout << _T("[FILE NAME=") << fname << _T("]") << endl;
_t_cout << _T("[DIR=") << dir << _T("]") << endl;
_t_cout << _T("[PATH=") << path << _T("]") << endl;

return true;
}

/********************************************
* FN:		URL_TERMINATE
* CR:		02/11/99 AM.
* SUBJ:	Terminate a url string at first whitespace.
* WARN:	Modifies the given buffer.
********************************************/

void url_terminate(
	_TCHAR *url
	)
{
if (!url || !*url)
	return;

while (*url && !_istspace((_TUCHAR)*url))
	++url;
if (!*url)
	return;	// URL is terminated already.
*url = '\0';	// Terminate at whitespace char.
}

/********************************************
* FN:		PARSE_URL
* CR:		03/09/99 AM.
* SUBJ:	Unpack the pieces of a URL.
* NOTE:	The one in MFC assumes an absolute URL.  We're handling relative
*			URLs also.  Will fail on oddball inputs.
* EX:		http://www.xyz.com/abc/def/index.htm:8080 =>
*				service = http
*				server  = www.xyz.com
*				dirs    = abc/def    (no trailing slash).
*				file	  = index.htm
*				port    = 8080
* ASS:	No quotes around URL.
* WARN:	Given URL is mangled.
********************************************/

bool parse_url(
	_TCHAR *url,
	/*UP*/
	_TCHAR* &service,
	_TCHAR* &server,
	_TCHAR* &dirs,
	_TCHAR* &file,
	_TCHAR* &port
	)
{
service = server = dirs = file = port = 0;
_TCHAR *uptr = 0;
_TCHAR *ptr = 0;				// Start of latest item.
_TCHAR *lastslash = 0;	// One after last slash seen.

if (!url || !*url)
	return false;
uptr = ptr = url;

// Special-case: slough initial dot-slash.
// Special-case: reject initial dot-dot-slash.
if (*uptr == '.')
	{
	if (*(uptr+1) == '/')
		{
		uptr += 2;
		ptr = url = uptr;
		}
	else if (*(uptr+1) == '.' && *(uptr+2) == '/')
		return false;
	}

while (*uptr)
	{
	switch (*uptr)
		{
		case ':':
			service = ptr;
			*uptr = '\0';
			if (*(uptr+1) == '/' && *(uptr+2) == '/')
				{
				if (server)
					return false;		// Multiples!

				// Looks like http://  Next item is server.
				uptr += 3;	// Skip past "://".
				server = uptr;
				ptr = 0;
				}
			else if (*(uptr+1) == '/')
				return false;
			else
				{
			   // Looks like http:xxx
				//server = 0;
				++uptr;
				}
			ptr = uptr;
			break;
		case '/':
			if (!server && !dirs)	// First dir slash.
				{
				// Looks like http:xyz/
				// lastslash tracks file position (potential).
				dirs = ptr;
				// Now ignoring ptr.
				lastslash = uptr+1;
				}
			else if (!dirs)
				{
				if (!lastslash)
					{
					// Looks like http://a.b.c/
					*uptr = '\0';	// Terminate server.
					ptr = uptr+1;
					}
				else
					{
					// Looks like http://a.b.c/xxx/
					dirs = ptr;
					// Now ignoring ptr.
					}
				lastslash = uptr + 1;
				}
			else					// Last slash so far.
				// Looks like http:xyz/abc/
				// or   http://a.b.c/def/
				lastslash = uptr + 1;
			++uptr;
			break;
		default:
			++uptr;
			break;
		}
	}
file = lastslash;
if (lastslash && (lastslash != url) && *(lastslash-1) == '/')
	*(lastslash-1) = '\0';		// Terminate dirs.
return true;
}

/******************   END OF FILE   **************************/
