/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	DIR.CPP
* FILE:	lite\dir.cpp
* CR:		03/11/99 AM.
* SUBJ:	Functions for handling directories.
* NOTE:	Pulled some old stuff from webco\Uanalyze.cpp.
*			Somewhat inefficient, in that it traverses directories twice.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"
#include "lite/lite.h"
#include "lite/mach.h"

#include "lite/dir.h"


/********************************************
* FN:		RM_PATH
* CR:		03/11/99 AM.
* SUBJ:	Remove a file, directory, or tree of directories.
********************************************/

LITE_API void rm_path(
	_TCHAR *infile,		// The path (file, directory, or directory tree).
	bool tree			// True if removing subdirectories recursively.
	)
{
if (!safe_dir(infile))
	return;

// If a file, remove.  If a directory, remove its files.
rm_dir_files(infile);

// If a directory, traverse it again looking for subdirectories.
if (tree)
	rm_dir_tree(infile);
}

/********************************************
* FN:		RM_DIR_FILES
* CR:		03/11/99 AM.
* SUBJ:	Remove the files in the given directory.  If given file, removes it.
* NOTE:	Not recursive. Ignores folders.
* PC:		Filename/dirname can't contain any of the following chars
*					\ / : * ? " < > |
********************************************/

LITE_API void rm_dir_files(
	_TCHAR *infile		// The directory.
	)
{
#ifndef LINUX
// Check if input file is a directory.
struct _tfinddata_t i_file;
struct _tfinddata_t i_tmp;	// For traversing directory.
long h_file;
_TCHAR buf[MAXSTR];

if (!safe_dir(infile))
	return;

// Another way to check file existence also...
if ((h_file = _tfindfirst(infile, &i_file)) == -1L)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Directory not found=") << infile << _T("]") << ends;
	errOut(&gerrStr,false);
	}

else if (i_file.attrib & _A_SUBDIR)	// Check if a directory.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Removing from directory '") << infile << _T("']") << ends;
	errOut(&gerrStr,false);

	// Traverse the files of the directory, analyzing each.
	_stprintf(buf, _T("%s\\*.*"), infile);
	if ((h_file = _tfindfirst(buf, &i_tmp)) == -1L)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[No files in directory.]") << ends;
		errOut(&gerrStr,false);
		}

	else
		{
		// Traverse.
		// As usual, "." and ".." are files in every directory.
		//*gerr << i_tmp.name << endl;					// First file.
		if (plain_file(i_tmp))
			{
			// Glom directory and file name.
			_stprintf(buf, _T("%s\\%s"), infile, i_tmp.name);
			_t_strstream gerrStr;
			gerrStr << _T("Remove ") << buf << ends;
			errOut(&gerrStr,false);
			remove_path(buf);
			}
		while (_tfindnext(h_file, &i_tmp) == 0)	// Rest of files.
			{
			//*gerr << i_tmp.name << endl;
			if (plain_file(i_tmp))
				{
				// Glom directory and file name.
				_stprintf(buf, _T("%s\\%s"), infile, i_tmp.name);
				_t_strstream gerrStr;
				gerrStr << _T("Remove ") << buf << ends;
				errOut(&gerrStr,false);
				remove_path(buf);
				}
			}
		//*gerr << "[Done with files in directory.]" << endl;
		}
	}
else
	{
	_t_strstream gerrStr;
	gerrStr << _T("Removing file ") << infile << ends;
	errOut(&gerrStr,false);
	remove_path(infile);
	}
#endif
}


/********************************************
* FN:		RM_DIR_TREE
* CR:		03/11/99 AM.
* SUBJ:	Remove a tree of directories.
* NOTE:	Handle only the directories in the given path, if any.
*			Handle a simple file also.
* RECURSIVE.
********************************************/

LITE_API void rm_dir_tree(
	_TCHAR *infile		// The path (file, directory, or directory tree).
	)
{
#ifndef LINUX
// Check if input file is a directory.
struct _tfinddata_t i_file;
struct _tfinddata_t i_tmp;	// For traversing directory.
long h_file;
_TCHAR buf[MAXSTR];

if (!safe_dir(infile))
	return;

// Another way to check file existence also...
if ((h_file = _tfindfirst(infile, &i_file)) == -1L)
	{
	//*gerr << "[File or directory not found=" << infile << "]" << endl;
	}
else if (i_file.attrib & _A_SUBDIR)	// Check if a directory.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Found directory=") << infile << _T("]") << ends;
	errOut(&gerrStr,false);

	// Traverse the directories under the directory.
	_stprintf(buf, _T("%s\\*.*"), infile);
	if ((h_file = _tfindfirst(buf, &i_tmp)) == -1L)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[No files in directory.]") << ends;
		errOut(&gerrStr,false);
		}

	else
		{
		// Traverse.
		// As usual, "." and ".." are files in every directory.
		//*gerr << i_tmp.name << endl;					// First file.
		if (is_dir(i_tmp))
			{
			// Glom directory and file name.
			_stprintf(buf, _T("%s\\%s"), infile, i_tmp.name);
			_t_strstream gerrStr;
			gerrStr << _T("Removing ") << buf << ends;
			errOut(&gerrStr,false);
			rm_path(buf, true);		// RECURSIVE.
			}
		while (_tfindnext(h_file, &i_tmp) == 0)	// Rest of files.
			{
			//*gerr << i_tmp.name << endl;
			if (is_dir(i_tmp))
				{
				// Glom directory and file name.
				_stprintf(buf, _T("%s\\%s"), infile, i_tmp.name);
				_t_strstream gerrStr;
				gerrStr << _T("Removing ") << buf << ends;
				errOut(&gerrStr,false);
				rm_path(buf, true);	// RECURSIVE.
				}
			}
		//*gerr << "[Done with directories under directory.]" << endl;
		}

	gerrStr << _T("Removing directory ") << infile << ends;
	errOut(&gerrStr,false);
	rm_dir(infile);
	}
else
	{
	//*gerr << "[Found single file.]" << endl;
	{
	_t_strstream gerrStr;
	gerrStr << _T("Removing ") << infile << ends;
	errOut(&gerrStr,false);
	}

	remove_path(infile);
	}
#endif
}


/********************************************
* FN:		PLAIN_FILE
* CR:		12/16/98 AM.
* SUBJ:	See if file is a plain computer file.
* NOTE:	Check for . .. and directories.  Doesn't check for binary files,
*			etc.  Would be nice to be able to do that.  Or, could require a
*			prefix (or set of prefixes:  txt htm html, etc.) in order to
*			analyze a file.
********************************************/

#ifndef LINUX
LITE_API bool plain_file(struct _tfinddata_t &fileinfo)
{
if (!_tcscmp(fileinfo.name, _T("."))
	 || !_tcscmp(fileinfo.name, _T(".."))
	 || (fileinfo.attrib & _A_SUBDIR)
	 )
	return false;
return true;
}
#endif


/********************************************
* FN:		IS_DIR
* CR:		12/16/98 AM.
* SUBJ:	See if file is a directory.
********************************************/

#ifndef LINUX
LITE_API bool is_dir(struct _tfinddata_t &fileinfo)
{
if (!_tcscmp(fileinfo.name, _T("."))
	 || !_tcscmp(fileinfo.name, _T(".."))
	 || !(fileinfo.attrib & _A_SUBDIR)
	 )
	return false;
return true;
}
#endif


/********************************************
* FN:		IS_FILE
* CR:		01/04/99 AM.
* SUBJ:	See if a plain file, from scratch.
********************************************/

LITE_API bool is_file(_TCHAR *name)
{
#ifndef LINUX
struct _tfinddata_t i_file;
long h_file;

// Another way to check file existence also...
if ((h_file = _tfindfirst(name, &i_file)) == -1L)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[File or directory not found=") << name << _T("]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
return plain_file(i_file);
#endif
}


/********************************************
* FN:		PATH_EXISTS
* CR:		12/16/98 AM.
* SUBJ:	See if file or directory exists.
* NOTE:	Equivalent to file_exists in Lite.
********************************************/

LITE_API bool path_exists(_TCHAR *name)
{
if (!name || !*name)
	return false;
#ifndef LINUX
struct _tfinddata_t i_file;
if (_tfindfirst(name, &i_file) == -1L)
	return false;
#endif
return true;
}

/********************************************
* FN:		SAFE_DIR
* CR:		03/11/99 AM.
* SUBJ:	See if head of path is safe for removal.
* NOTE:	To prevent catastrophic removal of files, as has already
*			taken place.
********************************************/

LITE_API bool safe_dir(_TCHAR *path)
{
#ifndef LINUX
if (!path || !*path)
	return false;
switch (*path)
	{
	case '\\':	// This one led to catastrophe.
	case '.':	// This could be serious trouble.
	case '*':
	case '/':
	case '?':
	case '"':
	case '<':
	case '>':
	case '|':
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Unsafe filename: ") << path << _T("]") << ends;
		return errOut(&gerrStr,false);	// Not allowing files that start this way.
		}
	default:
		break;
	}
if (!strcmp_ni(path, _T("C:\\WIN"), 6)) // Don't want to go near these.
	{
	*gerr << _T("[Unsafe filename: ") << path << _T("]") << endl;
	return false;
	}
#endif
return true;
}
