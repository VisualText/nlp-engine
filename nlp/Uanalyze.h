/*******************************************************************************
Copyright (c) 1998-2006 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	UANALYZE.H
* FILE:	\Uanalyze.h
* CR:		12/25/98 AM.
* SUBJ:	Sample functions for analyzing files and directories.
*
*******************************************************************************/

#ifndef UANALYZE_H_
#define UANALYZE_H_

void analyze_path(
	NLP *nlp,
	_TCHAR *infile,
	_TCHAR *outfile,
	_TCHAR *appdir,
	bool silent,   // 07/15/03 AM.
	_TCHAR *outd,    // 07/15/03 AM.
   bool compiled, // 07/15/03 AM.
	_t_ostream *os,   // 07/15/03 AM.
	_TCHAR *obuf,     // 07/15/03 AM.
	bool tree			// True if analyzing subdirectories recursively.
	);
void analyze_dir(
	NLP *nlp,
	_TCHAR *infile,		// The path (file, directory, or directory tree).
	_TCHAR *outfile,
	_TCHAR *appdir,
	bool silent,   // 07/15/03 AM.
	_TCHAR *outd,    // 07/15/03 AM.
   bool compiled, // 07/15/03 AM.
	_t_ostream *os,   // 07/15/03 AM.
	_TCHAR *obuf     // 07/15/03 AM.
	);
void analyze_tree(
	NLP *nlp,
	_TCHAR *infile,		// The path (file, directory, or directory tree).
	_TCHAR *outfile,
	_TCHAR *appdir,
	bool silent,   // 07/15/03 AM.
	_TCHAR *outd,    // 07/15/03 AM.
   bool compiled, // 07/15/03 AM.
	_t_ostream *os,   // 07/15/03 AM.
	_TCHAR *obuf     // 07/15/03 AM.
	);

#endif

