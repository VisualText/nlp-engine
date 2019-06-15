/*******************************************************************************
Copyright © 2007-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:  DIRFILE.H
* FILE:  include/API/lite/dirfile.h
* CR:    11/20/07 AM.
*
* SUBJ:  Class to manage a file/directory within a directory.
*
* NOTE:  Internalize operating-specific file and directory handling.
*			For MS Visual C++ 2005 upgrade.
*
* API:   Use for iterating in a directory or directory tree.
*
*******************************************************************************/

#ifndef DIRFILE_H_
#define DIRFILE_H_


// Forward reference.
//class XX;

/********************************************
* CLASS: DIRFILE
* CR:    11/20/07 AM.
* SUBJ:  Class for traversing directory & file trees.
* NOTE:  
* OPT:   
********************************************/

class LITE_API Dirfile
{
public:
   Dirfile();                 // Default constructor.
   ~Dirfile();                // Destructor.

	// Access.
	void *gets_file();
	long geth_file();

	// Update.
	void sets_file(void*);
	void seth_file(long);

	// Members.

	// Get name of current file or directory.
	_TCHAR *name();

	// Get a current file or directory. If directory, get first file/dir within it.
	// Note: A way to check for file existence also.
	bool first(_TCHAR *path);

	// Path must be a directory.  Get first plain directory within it.
	bool sub_dir(_TCHAR *path);

	// Path must be a directory.  Get first plain file within it.
	bool sub_file(_TCHAR *path);

	// Get next file or directory within current directory.
	bool next();

	// Get next directory within current directory.
	bool next_dir();

	// Get next plain file in current directory.
	bool next_file();

	// If current pathi is a plain file/dir (ie, not '.' and not '..').
	bool is_plain();

	// If current path is a plain directory (excluding '.' and '..').
	bool is_dir();      // Moved from dir.h.	# 11/23/07 AM.

	// Is it a plain file (ie, not a directory)
	// Renamed from plain_file.
	bool is_file();  // Moved from dir.h.	# 11/23/07 AM.

	///////////////////////////////////////////////////////////
	// RAW FUNCTIONS = don't care if they find '.' and '..'.
	///////////////////////////////////////////////////////////

	// Get first file or directory.
	bool first_raw(_TCHAR *path);

	// Path must be a directory.  Get first directory within it.
	bool sub_dir_raw(_TCHAR *path);

	// Path must be a directory.  Get first file within it. (same as sub_file)
	bool sub_file_raw(_TCHAR *path);

	// Get next file or directory in current directory.
	bool next_raw();

	// Get next raw directory in current directory.
	bool next_dir_raw();

	// Get next file (same as next_file).
	bool next_file_raw();

	// If current pathi is a raw file/dir.
	bool is_raw();

	// If current path is a raw directory.
	bool is_dir_raw();

	// If current path is a file. (Same as is_file)
	bool is_file_raw();

private:

	// Operating system specific directory and file handling.
	// Microsoft Windows = uses _tfinddata_t struct and functions internally.
	void *s_file;	// Data structure for file information.
	long  h_file;	// Current file handle.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif

};

#endif
/*************************      END OF FILE      ******************************/
