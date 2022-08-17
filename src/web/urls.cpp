/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	URLS.CPP
* FILE:	c:\apps\spider\urls.cpp
* CR:		03/02/99 AM.
* SUBJ:	Process a batch of URLs.
*
*******************************************************************************/

#include "stdafx.h"
#include "web.h"
/*
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
*/
#include "tear.h"

#include <iostream>											// Upgrade.	// 01/24/01 AM.
#include <fstream>											// Upgrade.	// 01/24/01 AM.
#include <stdlib.h>

#include "util.h"
#include "url.h"
#include "urls.h"

#define MAXLINE	512


/********************************************
* FN:		URLSPROCESS
* CR:		03/02/99 AM.
* SUBJ:	Process a file of URLs to fetch each page.
* RET:	True if successful.
* NOTE:	Ignoring redundant filenames.  Ultimately need a unique file
*			naming scheme (VC++ provides one such...)
********************************************/

WEB_API bool urlsProcess(
	_TCHAR *urlfile,
	_TCHAR *dir,
	bool flag			// True if overwrite existing file.
	)
{
//ifstream inFile(urlfile, ios::in | ios::nocreate);
_t_ifstream inFile(TCHAR2CA(urlfile), ios::in);						// Upgrade.	// 01/24/01 AM>
if (!inFile)
	{
	_t_cerr << _T("Could not open input file '") << urlfile << _T("'.") << endl;
	return false;
	}
_TCHAR line[MAXLINE];
if (!inFile.eof())
	inFile.getline(line, MAXLINE);
else
	{
	_t_cout << _T("[Empty URL file=") << urlfile << _T("]") << endl;
	return true;			// Empty file is an ok thing.
	}

LPCTSTR url = NULL;
bool warned = false;
for (;;)
	{
	// Parse the current line, fetching URL if any.
	if (line && *line && !_tcsncicmp(line, _T("http://"),7))
		{
		url_terminate(line);				// Terminate URL at whitespace.
		url = line;
		urlProcess(url, dir, flag);	// Bring in the web page.		
		}
	else if (!warned)
		{
		_t_cerr << _T("[URLs file with non-URL line]") << endl;
		warned = true;
		}

	// Get next file entry.
	if (!inFile.eof())
		inFile.getline(line, MAXLINE);
	else
		return true;
	}
return true;
}

/********************************************
* FN:		URLPROCESS
* CR:		03/11/99 AM.
* SUBJ:	Fetch one URL into a file. 
* RET:	True if successful.
* NOTE:	Ignoring redundant filenames for the moment.
********************************************/

WEB_API bool urlProcess(
	const _TCHAR *url,
	_TCHAR *dir,
	bool flag			// True if overwrite existing file.
	)
{
int retCode = 0;
//LPCTSTR url = NULL;
_TCHAR outfile[256];	// Name for output file for this URL.
_TCHAR buf[512];

if (!url || !*url)
	return false;

if (!url_to_fname(url, outfile))	// Build a filename for URL.
	{
	_t_cerr << _T("Error: Couldn't build file for URL=") << url << endl;
	return false;
	}
if (!flag && file_exists(outfile))		// Filename already present.
	return false;

// BRING IN THE WEB PAGE.
if (!dir || !*dir)	// No dir. Use current directory.
	retCode = tear_url(url, outfile);
else						// Build path for file.
	{
	_stprintf(buf, _T("%s\\%s"), dir, outfile);
	retCode = tear_url(url, buf);
	}

if (retCode)
	{
	_t_cerr << _T("[Couldn't fetch: ") << url << _T("]") << endl;
	return false;
	}
return true;
}