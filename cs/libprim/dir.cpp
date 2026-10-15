/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DIR.CPP
* FILE:	libprim/dir.cpp
* CR:		05/06/99 AM.
* SUBJ:	Functions for handling directories.
*
*******************************************************************************/
#include "StdAfx.h"
#include <filesystem>		// #323.
#ifdef LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include "prim/libprim.h"
#include "prim/dir.h"

/********************************************
* FN:		MAKE_DIR
* CR:		05/06/99 AM.
* SUBJ:	Create a directory.
********************************************/

LIBPRIM_API bool
make_dir(_TCHAR *dir)
{
if (!dir || !*dir)
	return false;
std::error_code ec;
return std::filesystem::create_directory(std::filesystem::path(dir), ec);	// #323.
}


/********************************************
* FN:		RM_DIR
* CR:		05/06/99 AM.
* SUBJ:	Remove a directory.
* NOTE:	Must be valid, empty, not root, not current working dir.
********************************************/

LIBPRIM_API bool
rm_dir(_TCHAR *dir)
{
if (!dir || !*dir)
	return false;
std::error_code ec;
std::filesystem::path p(dir);
if (!std::filesystem::is_directory(p, ec))
	return false;
return std::filesystem::remove(p, ec);	// Only if empty.	// #323.
}
