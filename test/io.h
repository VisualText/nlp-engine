/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	IO.H
* FILE:	lite\io.h
* CR:		10/06/98 AM.
* SUBJ:	Declares for I/O functions.
*
*******************************************************************************/

//#include "fstream.h"

void copy_file(const _TCHAR *, const _TCHAR *);
bool file_exists(const _TCHAR *iname);				// 12/14/98 AM.
void file_to_buffer(const _TCHAR *, _TCHAR *,
	/*UP*/ long &len															// 05/28/00 /AM.
	);
_TCHAR *pretty_char(_TCHAR);
_TCHAR *pretty_str(
	_TCHAR *str,				// String to be prettified.
	_TCHAR *buf,				// Buffer for placing prettified string.
	long size = -1			// Buffer size. (-1 means count not used.)
	);
_TCHAR *c_char(_TCHAR,_TCHAR*);												// 05/10/00 AM.
_TCHAR *c_str(																	// 05/10/00 AM.
	_TCHAR *str,				// String to be prettified.
	_TCHAR *buf,				// Buffer for placing prettified string.
	long size = -1			// Buffer size. (-1 means count not used.)
	);


void read_file(
	_TCHAR *fname,		// The filename
	/*UP*/
	long &len,			// Length of file.
	_TCHAR* &buf			// Buffer to create.
	)
	;

_TCHAR *next_token(/*DU*/ _TCHAR* &buf, bool &eol, _TCHAR *comment);

bool fix_file_name(
	_TCHAR *file,				// Buffer big enough to hold extension.
	_TCHAR *suff				// File name extension needed.
	)
	;

bool eq_str_range(_TCHAR *str, _TCHAR *ptr, long start, long end);

_TCHAR *make_str(_TCHAR *str);
_TCHAR *make_str(_TCHAR *str, long len);

bool strcat_e(
	/*DU*/ _TCHAR* &ptr,
	/*DN*/ _TCHAR *str,
	/*DU*/ long &count
	);


bool strncat_e(
	/*DU*/ _TCHAR* &ptr,
	/*DN*/ _TCHAR *str,
	/*DN*/ long len,
	/*DU*/ long &count
	);

_t_filebuf *directOutput(_TCHAR *fname);

bool file_name(															// 12/24/99 AM.
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fname			// Pointer to the name in buffer.
	);
bool file_path(															// 12/24/99 AM.
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fpath			// Pointer to the path in buffer.
	);
bool file_head(															// 12/24/99 AM.
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &fhead			// Pointer to the file head in buffer.
	);
bool file_tail(															// 12/24/99 AM.
	_TCHAR *file,				// Buffer with full file string.
	/*UP*/
	_TCHAR* &ftail			// Pointer to tail in buffer.
	);
