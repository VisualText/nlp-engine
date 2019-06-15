/*******************************************************************************
Copyright © 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	URLS.H
* FILE:	include/API/web/url.h
* CR:		03/02/99 AM.
* SUBJ:	Manipulators for URLs.
*
*******************************************************************************/

#ifndef URLS_H_
#define URLS_H_

WEB_API bool urlsProcess(
	_TCHAR *urlfile,
	_TCHAR *dir = 0,
	bool flag = true			// True if overwrite existing file.
	);
WEB_API bool urlProcess(
	const _TCHAR *url,
	_TCHAR *dir = 0,
	bool flag = true			// True if overwrite existing file.
	);

#endif