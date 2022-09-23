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

long file_size(_TCHAR *fname)
{
long len=0;

/* Open a file */
// CHANGING THIS TO A BETTER MEASURE.									// 07/18/00 AM.
#ifdef LINUX
std::_t_ifstream x;					// 10/26/06 AM.
x.open(fname,std::ios::binary);	// 10/26/06 AM.
x.seekg(0,std::ios::end);			// 10/26/06 AM.
len = x.tellg();				// 10/26/06 AM.
x.close();						// 10/26/06 AM.
#else
struct _stat buf;
int result = _tstat(fname, &buf);
if (result == 0) {
       len = buf.st_size;
}
#endif

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
