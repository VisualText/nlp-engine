/*******************************************************************************
Copyright � 2007-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DIRFILE.CPP
* FILE:	lite/dirfile.h
* CR:		11/20/07 AM.
*
* SUBJ:  Class to manage a file/directory within a directory.
*
* NOTE:  Internalize operating-specific file and directory handling.
*			For MS Visual C++ 2005 upgrade.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include "machine.h"
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"
#include "dlist.h"					// 07/07/03 AM.
#include "lite/global.h"
#include	"lite/slist.h"		// 01/06/03 AM.
#include "node.h"				// 07/07/03 AM.
#include "lite/ana.h"
#include "lite/nlp.h"
#include "prim/libprim.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "htab.h"
#include "rfa.h"
#include "func_defs.h"
#include "ifunc.h"
#include "str.h"	// For str_ends_with()	// 05/20/08 AM.

#ifndef QDBM_
#include "consh/libconsh.h"	// 07/18/03 AM.
#include	"consh/cg.h"			// 07/18/03 AM.
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <string.h>
#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/Aqkbm.h"
#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"
#endif

#include "lite/dirfile.h"


int Dirfile::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

Dirfile::Dirfile(
	)
{
#ifndef LINUX

// Check if input file is a directory.
struct _tfinddata_t *i_file = new _tfinddata_t;
s_file = (void *) i_file;
#else
// UNIMPLEMENTED IN LINUX.  Todo.
s_file = 0;
#endif

long h_file = 0;


#ifndef STABLE_
++count_;
#endif

}


Dirfile::~Dirfile()
{
#ifndef LINUX
struct _tfinddata_t *i_file = (_tfinddata_t *) s_file;
if (s_file)
	delete s_file;
#else
#endif

s_file = 0;
h_file = 0;

#ifndef STABLE_
--count_;
#endif
}


//*******************************************
long  Dirfile::geth_file() {return h_file;}
void *Dirfile::gets_file() {return s_file;}

void Dirfile::seth_file(long x)  {h_file = x;}
void Dirfile::sets_file(void *x) {s_file = x;}

//*******************************************
//*******************************************


//*******************************************
_TCHAR *Dirfile::name()
{
#ifndef LINUX
return ((_tfinddata_t *) s_file)->name;
#else
#endif
}


//*******************************************
// SUBJ:	Get the first "plain" file or directory, if any.	// 05/18/08 AM.
bool Dirfile::first(
	_TCHAR *path    // Starting file or directory.
	)
{
if (!path && !*path)
	{
	h_file = 0;
	return false;
	}

#ifndef LINUX
// Another way to check file existence also...
if ((h_file = _tfindfirst(path, (_tfinddata_t *) s_file)) == -1L)
	{
//	cout << _T("[File or directory not found=") << path << _T("]") << endl;
	h_file = 0;
	return false;
	}

// If the first is "plain" file or dir, done.  That is, not "." or "..".	// 05/18/08 AM.
if (is_plain())	// FIX.	// 05/18/08 AM.
  return true;		// FIX.	// 05/18/08 AM.

// Else, return the next plain file or dir.	// FIX.	// 05/18/08 AM.
return next();		// FIX.	// 05/18/08 AM.

#else
#endif

return false;
}

//*******************************************
// Find the first "plain" directory below given path.	// 05/18/08 AM.
bool Dirfile::sub_dir(
	_TCHAR *path    // Starting file or directory.
	)
{
if (!path && !*path)
	{
	h_file = 0;
	return false;
	}

#ifndef LINUX

_TCHAR buf[MAXSTR];
_stprintf(buf, _T("%s%c*.*"), path, DIR_CH);

// Another way to check file existence also...
if ((h_file = _tfindfirst(buf, (_tfinddata_t *) s_file)) == -1L)
	{
//	cout << _T("[Directory not found=") << buf << _T("]") << endl;
	h_file = 0;
	return false;
	}
// If a plain directory, done.
if (is_dir())
	return true;

// Else find the next plain dir, if any.
return next_dir();
#else
#endif

return false;
}

//*******************************************
bool Dirfile::sub_file(
	_TCHAR *path    // Starting file or directory.
	)
{
if (!path && !*path)
	{
	h_file = 0;
	return false;
	}

return fs::is_regular_file(path);
}


//*******************************************
bool Dirfile::next()
{
#ifndef LINUX
// Fixing to return the next plain dir or file.	// 05/18/08 AM.
while (_tfindnext(h_file, (_tfinddata_t *) s_file) == 0)	// Found file
	{
	if (is_plain())	// If a plain file or directory, done.
		return true;
	}
#else
#endif
return false;
}

//*******************************************
bool Dirfile::next_dir()
{
#ifndef LINUX
while (_tfindnext(h_file, (_tfinddata_t *) s_file) == 0)	// Found file
	{
	if (is_dir())	// If a plain directory, done.
		return true;
	}
#else
#endif

return false;
}

//*******************************************
bool Dirfile::next_file()
{
#ifndef LINUX
while (_tfindnext(h_file, (_tfinddata_t *) s_file) == 0)	// Found file
	{
	if (is_file())	// If a plain directory, done.
		return true;
	}
#else
#endif

return false;
}


/********************************************
* FN:		IS_DIR
* CR:		12/16/98 AM.
* SUBJ:	See if file is a directory.
* NOTE:	11/23/07 AM. Hiding Microsoft-specific finddata code.  Moved from dir.h
********************************************/

LITE_API bool Dirfile::is_dir()
{
if (!s_file || !h_file)
	return false;

#ifndef LINUX
_TCHAR *nm = ((_tfinddata_t *) s_file)->name;	// 05/20/08 AM.
if (!_tcscmp(nm, _T("."))
	 || !_tcscmp(nm, _T(".."))
	 || str_ends_with(nm,_T("_log"))						// 05/20/08 AM.
	 || !(((_tfinddata_t *) s_file)->attrib & _A_SUBDIR)
	 )
	return false;
return true;
#else
return false;
#endif
}



/********************************************
* FN:		IS_FILE
* CR:		12/16/98 AM.
* SUBJ:	See if file is a plain computer file.
* NOTE:	Check for . .. and directories.  Doesn't check for binary files,
*			etc.  Would be nice to be able to do that.  Or, could require a
*			prefix (or set of prefixes:  txt htm html, etc.) in order to
*			analyze a file.
*			11/23/07 AM. Hiding Microsoft-specific finddata code.  Moved from dir.h
*			11/23/07 AM. Renamed from PLAIN_FILE plain_file.
********************************************/

LITE_API bool Dirfile::is_file()
{
if (!s_file || !h_file)
	return false;

#ifndef LINUX
_TCHAR *nm = ((_tfinddata_t *) s_file)->name;	// 05/20/08 AM.
if (!_tcscmp(nm, _T("."))
	 || !_tcscmp(nm, _T(".."))
	 || str_ends_with(nm,_T("_log"))						// 05/20/08 AM.
	 || (((_tfinddata_t *) s_file)->attrib & _A_SUBDIR)
	 )
	return false;
return true;
#else
return false;
#endif
}



/********************************************
* FN:		IS_PLAIN
* CR:		11/23/07 AM.
* SUBJ:	See if normal file or directory.
* NOTE:	Check for . .. and directories.
********************************************/

LITE_API bool Dirfile::is_plain()
{
if (!s_file || !h_file)
	return false;

#ifndef LINUX
_TCHAR *nm = ((_tfinddata_t *) s_file)->name;	// 05/20/08 AM.
if (!_tcscmp(nm, _T("."))
	 || !_tcscmp(nm, _T(".."))
	 || str_ends_with(nm,_T("_log"))						// 05/20/08 AM.
	 )
	return false;
return true;
#else
return false;
#endif
}

//******************************************
//          RAW FUNCTIONS
//******************************************

//*******************************************
bool Dirfile::first_raw(
	_TCHAR *path    // Starting file or directory.
	)
{
if (!path && !*path)
	{
	h_file = 0;
	return false;
	}

#ifndef LINUX
// Another way to check file existence also...
if ((h_file = _tfindfirst(path, (_tfinddata_t *) s_file)) == -1L)
	{
//	cout << _T("[File or directory not found=") << path << _T("]") << endl;
	h_file = 0;
	return false;
	}
return true;
#else
return false;
#endif
}

//*******************************************
// TODO: Use the Char create class.
// TODO: Handle paths with a slash or backslash as last character.
bool Dirfile::sub_dir_raw(
	_TCHAR *path    // Starting file or directory.
	)
{
if (!path && !*path)
	{
	h_file = 0;
	return false;
	}

#ifndef LINUX

_TCHAR buf[MAXSTR];
_stprintf(buf, _T("%s%c*.*"), path, DIR_CH);

// Another way to check file existence also...
if ((h_file = _tfindfirst(buf, (_tfinddata_t *) s_file)) == -1L)
	{
//	cout << _T("[Directory not found=") << buf << _T("]") << endl;
	h_file = 0;
	return false;
	}
if (is_dir_raw())	// If first subdir is a directory, done.
	return true;

return next_dir_raw();

#else
#endif

return false;
}

//*******************************************
bool Dirfile::sub_file_raw(
	_TCHAR *path    // Starting file or directory.
	)
{
if (!path && !*path)
	{
	h_file = 0;
	return false;
	}

#ifndef LINUX

_TCHAR buf[MAXSTR];
_stprintf(buf, _T("%s%c*.*"), path, DIR_CH);

// Another way to check file existence also...
if ((h_file = _tfindfirst(buf, (_tfinddata_t *) s_file)) == -1L)
	{
//	cout << _T("[File not found=") << buf << _T("]") << endl;
	h_file = 0;
	return false;
	}
if (is_file_raw())	// If first sub is a file, done.
	return true;

return next_file_raw();

#else
#endif

return false;
}


//*******************************************
bool Dirfile::next_raw()
{
#ifndef LINUX
return (_tfindnext(h_file, (_tfinddata_t *) s_file) == 0) ? true : false;
#else
return 0;
#endif
}


//*******************************************
bool Dirfile::next_dir_raw()
{
#ifndef LINUX
while (_tfindnext(h_file, (_tfinddata_t *) s_file) == 0)	// Found file
	{
	if (is_dir_raw())	// If a raw directory, done.
		return true;
	}
#else
#endif

return false;
}

//*******************************************
bool Dirfile::next_file_raw()
{
#ifndef LINUX
while (_tfindnext(h_file, (_tfinddata_t *) s_file) == 0)	// Found file
	{
	if (is_file_raw())	// If a raw directory, done.
		return true;
	}
#else
#endif

return false;
}

//*******************************************
bool Dirfile::is_dir_raw()
{
if (!s_file || !h_file)
	return false;

#ifndef LINUX
if (!_tcscmp(((_tfinddata_t *) s_file)->name, _T("."))
	 || !_tcscmp(((_tfinddata_t *) s_file)->name, _T(".."))
	 || (((_tfinddata_t *) s_file)->attrib & _A_SUBDIR)
	 )
	return true;
#else
#endif

return false;
}

//*******************************************
bool Dirfile::is_file_raw()
{
if (!s_file || !h_file)
	return false;

#ifndef LINUX
if (!_tcscmp(((_tfinddata_t *) s_file)->name, _T("."))
	 || !_tcscmp(((_tfinddata_t *) s_file)->name, _T(".."))
	 || (((_tfinddata_t *) s_file)->attrib & _A_SUBDIR)
	 )
	return false;
return true;
#else
return false;
#endif
}

//*******************************************
bool Dirfile::is_raw()
{
return (s_file && h_file);
}

/*************************      END OF FILE      ******************************/
