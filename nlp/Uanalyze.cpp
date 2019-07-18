/*******************************************************************************
Copyright (c) 1998-2006 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	UANALYZE.CPP
* FILE:	\Uanalyze.cpp
* CR:		12/25/98 AM.
* SUBJ:	Sample functions for analyzing files and directories.
* ALG:	The method used here is to analyze all the files in a directory first,
*			then to retraverse the directory and analyze the files in the
*			subdirectories.  Makes for a more organized traversal and output, at
*			a small performance cost.
* NOTE:	12/25/98 AM. Moved functions here from main.cpp.
*
*******************************************************************************/
#ifndef LINUX
#include <windows.h>
#endif
//#include <io.h>	// 07/15/03 AM.
//#include <fstream>											// Upgrade.	// 02/07/01 AM.
#include <time.h>

#include "machine.h"	// 03/08/00 AM.

#include "vtapi.h"         // VISUALTEXT API INCLUDES.          // 04/05/05 AM.

#include "lite/mach.h"
#include "lite/dir.h"		// 03/30/00 AM.
#include "lite/nlp.h"


#include "Uanalyze.h"


/********************************************
* FN:		ANALYZE_PATH
* CR:		12/16/98 AM.
* SUBJ:	Analyze a file, directory, or tree of directories.
********************************************/

void analyze_path(
	NLP *nlp,
	_TCHAR *infile,		// The path (file, directory, or directory tree).
	_TCHAR *outfile,
	_TCHAR *appdir,
	bool silent,   // 07/15/03 AM.
	_TCHAR *outd,    // 07/15/03 AM.
   bool compiled, // 07/15/03 AM.
	_t_ostream *os,   // 07/15/03 AM.
	_TCHAR *obuf,     // 07/15/03 AM.
	bool tree			// True if analyzing subdirectories recursively.
	)
{
// If a file, analyze.  If a directory, analyze its files.
analyze_dir(nlp,infile,outfile,appdir,
               silent,outd,compiled,os,obuf);  // 07/15/03 AM.

// If a directory, traverse it again looking for subdirectories.
if (tree)
	analyze_tree(nlp,infile,outfile,appdir,
               silent,outd,compiled,os,obuf); // 07/15/03 AM.
}


/********************************************
* FN:		ANALYZE_DIR
* CR:		12/16/98 AM.
* SUBJ:	Analyze a directory or a file.
* NOTE:	Handle only the files in the directory.
*			Handle a simple file also.
********************************************/

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
	)
{
#ifndef LINUX
// 12/16/98 AM.
// Check if input file is a directory.
struct _tfinddata_t i_file;
struct _tfinddata_t i_tmp;	// For traversing directory.
long h_file;
_TCHAR buf[MAXSTR];

// Another way to check file existence also...
if ((h_file = _tfindfirst(infile, &i_file)) == -1L)
	cout << _T("[File or directory not found=") << infile << _T("]") << endl;
else if (i_file.attrib & _A_SUBDIR)	// Check if a directory.
	{
	//cout << "[Analyzing directory '" << infile << "']" << endl;

	// Traverse the files of the directory, analyzing each.
	// (What if filename ends with backslash?)
	_stprintf(buf, _T("%s%c*.*"), infile, DIR_CH);	// 03/08/00 AM.
	if ((h_file = _tfindfirst(buf, &i_tmp)) == -1L)
		cout << _T("[No files in directory.]") << endl;
	else
		{
		// Traverse.
		// As usual, "." and ".." are files in every directory.
		//cout << i_tmp.name << endl;					// First file.
		if (plain_file(i_tmp))
			{
			// Glom directory and file name.
			_stprintf(buf, _T("%s%c%s"), infile, DIR_CH, i_tmp.name);
//			nlp->analyze(buf, outfile, appdir, false);  // 07/15/03 AM.
         nlp->analyze(buf, outfile, appdir, false,   // 07/15/03 AM.
              silent,        // Debug/log output files.
              outd,            // Outdir.
              0,            // Input buffer.
              0,            // Length of input buffer, or 0.
              compiled,      // If running compiled analyzer.
              os,				// Rebind cout output stream in analyzer
              obuf,
              MAXSTR
              );
			}
		while (_tfindnext(h_file, &i_tmp) == 0)	// Rest of files.
			{
			//cout << i_tmp.name << endl;
			if (plain_file(i_tmp))
				{
				// Glom directory and file name.
				_stprintf(buf, _T("%s%c%s"), infile,DIR_CH, i_tmp.name);
//          nlp->analyze(buf, outfile, appdir, false);  // 07/15/03 AM.
            nlp->analyze(buf, outfile, appdir, false,   // 07/15/03 AM.
              silent,        // Debug/log output files.
              outd,            // Outdir.
              0,            // Input buffer.
              0,            // Length of input buffer, or 0.
              compiled,      // If running compiled analyzer.
              os,				// Rebind cout output stream in analyzer
              obuf,
              MAXSTR
              );
				}
			}
		//cout << "[Done with files in directory.]" << endl;
		}
	}
else
	{
	//cout << "[Found single file.]" << endl;
// nlp->analyze(buf, outfile, appdir, false);  // 07/15/03 AM.
   nlp->analyze(buf, outfile, appdir, false,   // 07/15/03 AM.
              silent,        // Debug/log output files.
              outd,            // Outdir.
              0,            // Input buffer.
              0,            // Length of input buffer, or 0.
              compiled,      // If running compiled analyzer.
              os,				// Rebind cout output stream in analyzer
              obuf,
              MAXSTR
              );
	}
#endif
}


/********************************************
* FN:		ANALYZE_TREE
* CR:		12/16/98 AM.
* SUBJ:	Analyze a tree of directories.
* NOTE:	Handle only the directories in the given path, if any.
*			Handle a simple file also.
* RECURSIVE.
********************************************/

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
	)
{
// 12/16/98 AM.
// Check if input file is a directory.
#ifndef LINUX
struct _tfinddata_t i_file;
struct _tfinddata_t i_tmp;	// For traversing directory.
long h_file;
_TCHAR buf[MAXSTR];

// Another way to check file existence also...
if ((h_file = _tfindfirst(infile, &i_file)) == -1L)
	{
	//cout << "[File or directory not found=" << infile << "]" << endl;
	}
else if (i_file.attrib & _A_SUBDIR)	// Check if a directory.
	{
	//cout << "[Found directory=" << infile << "]" << endl;
	// Traverse the directories under the directory.
	_stprintf(buf, _T("%s%c*.*"), infile, DIR_CH);
	if ((h_file = _tfindfirst(buf, &i_tmp)) == -1L)
		cout << _T("[No files in directory.]") << endl;
	else
		{
		// Traverse.
		// As usual, "." and ".." are files in every directory.
		//cout << i_tmp.name << endl;					// First file.
		if (is_dir(i_tmp))
			{
			// Glom directory and file name.
			_stprintf(buf, _T("%s%c%s"), infile,DIR_CH, i_tmp.name);
			analyze_path(nlp,buf,outfile,appdir,
                         silent,outd,compiled,os,obuf,   // 07/15/03 AM.
			                                      true);		// RECURSIVE.
			}
		while (_tfindnext(h_file, &i_tmp) == 0)	// Rest of files.
			{
			//cout << i_tmp.name << endl;
			if (is_dir(i_tmp))
				{
				// Glom directory and file name.
				_stprintf(buf, _T("%s%c%s"), infile,DIR_CH, i_tmp.name);
				analyze_path(nlp,buf,outfile,appdir,
                         silent,outd,compiled,os,obuf,   // 07/15/03 AM.
			                                      true);		// RECURSIVE.
				}
			}
		//cout << "[Done with directories under directory.]" << endl;
		}
	}
else
	{
	//cout << "[Found single file.]" << endl;
	}
#endif
}

