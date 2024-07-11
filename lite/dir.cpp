/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
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
#include <sys/stat.h>
#include "io.h"
#include "lite/dir.h"

#include <vector>

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

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
try {
	for (const auto& entry : fs::directory_iterator(infile)) {
		if (fs::is_regular_file(entry)) {
			fs::remove(entry);
		}
	}
} catch (const std::exception& e) {
	std::cerr << "Error removing files: " << e.what() << std::endl;
}
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

try {
	for (const auto& entry : fs::directory_iterator(infile)) {
		if (fs::is_directory(entry)) {
			fs::remove_all(entry.path());
		}
	}
} catch (const std::exception& e) {
	std::cerr << "Error deleting subdirectories: " << e.what() << std::endl;
}
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
		std::_t_strstream gerrStr;
		gerrStr << _T("[Unsafe filename: ") << path << _T("]") << std::ends;
		return errOut(&gerrStr,false);	// Not allowing files that start this way.
		}
	default:
		break;
	}
if (!strcmp_ni(path, _T("C:\\WIN"), 6)) // Don't want to go near these.
	{
	*gerr << _T("[Unsafe filename: ") << path << _T("]") << std::endl;
	return false;
	}
#endif
return true;
}

LITE_API bool
read_files(_TCHAR *dir, _TCHAR *filter, std::vector<std::filesystem::path>& files) 
{
    files.clear();
	const std::regex filt{filter};

	for (const auto & entry : std::filesystem::directory_iterator(dir)) {
        if (std::filesystem::is_regular_file(entry.path())) {
			if (!filter[0] || std::regex_match(entry.path().string(),filt))
            	files.push_back(entry.path());
        }
	}

	return files.size() > 0 ? true : false;
}
