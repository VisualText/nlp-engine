/*******************************************************************************
Copyright © 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	URL.H
* FILE:	include/API/web/url.h
* CR:		02/11/99 AM.
* SUBJ:	Manipulators for URLs.
*
*******************************************************************************/

#ifndef URL_H_
#define URL_H_

bool url_to_fname(
	const _TCHAR *url,
	_TCHAR *fname			// Buffer for filename.
	);
bool url_to_path(
	const _TCHAR *url,
	/*UP*/
	_TCHAR *path,
	_TCHAR *dir,			// Buffer for directory path.
	_TCHAR *fname			// Buffer for filename only.
	);
void url_terminate(
	_TCHAR *url
	);
WEB_API bool parse_url(
	_TCHAR *url,
	/*UP*/
	_TCHAR* &service,
	_TCHAR* &server,
	_TCHAR* &dirs,
	_TCHAR* &file,
	_TCHAR* &port
	);

#endif
