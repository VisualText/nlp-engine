/*******************************************************************************
Copyright � 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DIR.H
* FILE:	include/API/lite/dir.h
* CR:		03/11/99 AM.
* SUBJ:	Handling files and directories.
*
*******************************************************************************/

#ifndef DIR_H_
#define DIR_H_

#include <vector>
#include <filesystem>
#include <regex>

LITE_API void rm_path(
	_TCHAR *infile,		// The path (file, directory, or directory tree).
	bool tree			// True if removing subdirectories recursively.
	);
LITE_API void rm_dir_files(
	_TCHAR *infile		// The directory.
	);
LITE_API void rm_dir_tree(
	_TCHAR *infile		// The path (file, directory, or directory tree).
	);
LITE_API bool safe_dir(_TCHAR *path);

LITE_API bool read_files(
	_TCHAR *dir,
	_TCHAR *filter,
	std::vector<std::filesystem::path>& files
	);

#endif
