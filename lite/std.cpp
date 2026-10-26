/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STD.CPP
* FILE:	lite\std.c
* CR:		10/08/98 AM.
* SUBJ:	Stuff that seems to require C rather than C++.
*
*******************************************************************************/


#include "StdAfx.h"
#include <filesystem>		// #323.
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"

#include "std.h"

/********************************************
* FN:		FILE_SIZE
* CR:		10/08/98 AM.
* SUBJ:	Get the length of a file.
* NOTE:	Could do (_O_RDONLY | _O_TEXT), but count seems unaffected.
********************************************/

long long file_size(_TCHAR *fname)
{
long long len=0;	// Was long: a file over 2 GB went negative on Windows.	// 10/25/26 DD.

// One way on every platform (#323). Was a seek to the end of an
// opened file on Linux and _tstat elsewhere.
std::error_code ec;
std::uintmax_t sz = std::filesystem::file_size(std::filesystem::path(fname), ec);
if (!ec)
	len = (long long) sz;

#ifdef VCPPONLY_
//int fh;
//if((fh = _open(fname, _O_RDONLY)) == -1)
//	return (long) 0;

//len = _filelength(fh);
//_close(fh);

#endif
//cout << "[file_size: len=" << len << "]" << std::endl;
if (len <= 0)
   {
	std::_t_cout << _T("[file_size: Empty or no file=") << fname << _T("]") << std::endl;
	len = 0;
	}

return len;
}


/********************************************
* FN:		OPEN_OUTFILE
* CR:		12/01/98 AM.
* SUBJ:	Open a file for output.
* NOTE:	Trying to rebind stdout conveniently, so all output statements
*			won't have to be rewritten.
* RET:	A file handle.
********************************************/

#ifdef JUNKY_
int open_outfile(_TCHAR *fname)
{
int fh = -1;

#ifndef LINUX
fh = _topen( fname, _O_WRONLY | _O_CREAT, _S_IREAD | _S_IWRITE );
#endif
return fh;
}
#endif


/********************************************
* FN:		CLOSE_OUTFILE
* CR:		12/01/98 AM.
* SUBJ:	Close a file for output.
* NOTE:	Trying to rebind stdout conveniently, so all output statements
*			won't have to be rewritten.
********************************************/

#ifdef JUNKY_
void close_outfile(int fh)
{
#ifndef LINUX
if (fh > -1)
	_close(fh);
#endif
}
#endif
