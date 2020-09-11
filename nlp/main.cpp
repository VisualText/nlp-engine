/*******************************************************************************
Copyright © 1999-2006 Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME: MAIN.CPP
* FILE: main.cpp
* CR:   01/29/99 AM.
* SUBJ: Sample driver program that uses the Analyzer and KB API functions.
*       Loading an analyzer invokes the user extensions library (user/user.dll).
* ENV:  APPS - a default environment variable for VisualText applications.
*
*******************************************************************************/
#ifndef LINUX
#include <windows.h>
#include <fstream>
#include <tchar.h>
#include <my_tchar.h>
#include <streamclasses.h>
#else
#include <sys/stat.h>
#include "machine.h"
#endif
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <strstream>
#include <time.h>

#include "vtapi.h"		// VISUALTEXT API INCLUDES.			// 01/31/03 AM.
#include "u_out.h"                                          // 01/24/06 AM.

#include "Uanalyze.h"

bool cmdReadArgs(int,_TCHAR*argv[],_TCHAR*&,_TCHAR*&,_TCHAR*&,bool&,bool&,bool&);
void cmdHelpargs(_TCHAR*);

#ifdef LINUX
main(
#else
_tmain(
#endif
   int argc,
   _TCHAR *argv[],
   _TCHAR *envp[]
   )
{
// Without this, can't find the error output on the console.
#ifndef LINUX
//cerr = cout;                                          // HACK. // 10/18/98 AM.
#endif

/////////////////////////////////////////////////
// READ ARGUMENTS
/////////////////////////////////////////////////
// Need to get command line arguments and/or some kind of init file.
_TCHAR *analyzerpath=0, *input=0, *output=0;
bool develop = false;    	// Development mode.
_TCHAR *sequence=0;
bool compiled=false;       	// Run compiled/interp analyzer.
bool silent=false;			// No log/debug output files.

struct stat st;

/////////////////////////////////////////////////
// GET APP INFORMATION
/////////////////////////////////////////////////

// Get analyzer name, input and output filenames from command line.
if (!cmdReadArgs(argc,argv,analyzerpath,input,output,develop,compiled,silent))
   exit(1);

_t_cout << _T("[Current date: ") << today() << _T("]") << endl;

_TCHAR *path = 0;
_TCHAR rfbdir[MAXSTR];		    // Directory for VisualText data.   // 08/28/02 AM.
rfbdir[0] = '\0';                                               // 08/28/02 AM.
_stprintf(rfbdir, _T("data%crfb%cspec"),                          // 08/28/02 AM.
                          DIR_CH,DIR_CH);         // 08/28/02 AM.

// A general temporary directory.                               // 08/28/02 AM.                                  // 08/28/02 AM.
_TCHAR logfile[MAXSTR];		 // General VT log file.             // 08/28/02 AM.
logfile[0] = '\0';                                              // 08/28/02 AM.
_stprintf(logfile, _T("%s%cvisualtext.log"),_T("temp"),DIR_CH);           // 08/28/02 AM.

// Set up subdirectories with default names.
// Could read a configuration file here.
// Could set up to label output files based on input file name
// or some user-supplied prefix.  Or create unique file names.
_TCHAR anapath[MAXSTR];
_TCHAR ananame[MAXSTR];
_TCHAR specdir[MAXSTR];
_TCHAR infile[MAXSTR];
_TCHAR outfile[MAXSTR];
_TCHAR outfileDir[MAXSTR];
_TCHAR seqfile[MAXSTR];

if (stat(anapath,&st) != 0) {
	#ifdef LINUX
	   char cwd[MAXSTR];
	   _TCHAR* ana = anapath;
       if (getcwd(cwd, sizeof(cwd)) != NULL) {
		  _t_cout << _T("[current directory: ") << cwd << _T("]") << endl;
		  _stprintf(anapath, _T("%s%sanalyzers%s%s"),cwd,DIR_STR,DIR_STR,analyzerpath);
       } else {
          _t_cerr << _T("[couldn't get current directory") << analyzerpath << _T("]") << endl;
		  return 0;
	   }
	#endif
} else {
   _stprintf(anapath, _T("%s"),analyzerpath);	
}

if (stat(anapath,&st) != 0)
	{
	_t_cerr << _T("[Analyzer path not found: ") << anapath << _T("]\n") << endl;
	return 0;
	}
_t_cout << _T("[analyzer path: ") << anapath << _T("]") << endl;

_TCHAR *ana = _tcsrchr(anapath,_T('/'));
++ana;
_stprintf(ananame, _T("%s"), ana); 
_t_cout << _T("[analyzer name: ") << ananame << _T("]") << endl;

_stprintf(specdir, _T("%s%sspec"), anapath, DIR_STR);
_t_cout << _T("[spec file: ") << specdir << _T("]") << endl;

if (!input)
   input = _T("input.txt");         // Default.
if (!output)
   output = _T("output.txt");       // Default.

// Get analyzer-sequence file name.
if (!sequence)
   sequence = _T("analyzer.seq");   // Default.

// Input file
if (stat(input,&st) == 0)
	_stprintf(infile, _T("%s"),input);
else
	_stprintf(infile, _T("%s%sinput%s%s"),anapath,DIR_STR,DIR_STR,input);
_t_cout << _T("[infile file: ") << infile << _T("]") << endl;

_stprintf(outfile, _T("%s%s%s"), anapath,DIR_STR,output);
_t_cout << _T("[outfile file: ") << outfile << _T("]") << endl;

_stprintf(outfileDir, _T("%s%s%s"), anapath,DIR_STR,_T("output"));
_t_cout << _T("[outfileDir file: ") << outfileDir << _T("]") << endl;

_stprintf(seqfile, _T("%s%s%s"), specdir,DIR_STR,sequence);
_t_cout << _T("[seqfile file: ") << seqfile << _T("]") << endl;

if (stat(outfileDir,&st) != 0)
	{
#ifdef LINUX
	mkdir(outfileDir, 777);
#else
	CreateDirectory(outfileDir,NULL);
#endif
	_t_cout << _T("[Creating output directory: ") << outfileDir << _T("]") << endl;
	}

if (stat(anapath,&st) != 0)
	{
	_t_cerr << _T("[Analyzer path not found: ") << anapath << _T("]\n") << endl;
	return 0;
	}

if (stat(infile,&st) != 0)
	{
	_t_cerr << _T("[File/directory not founds: ") << infile << _T("]") << endl;
	return 0;
	}

/////////////////////////////////////////////////
// INITIALIZE VISUALTEXT RUNTIME SYSTEM                        // 08/27/02 AM.
/////////////////////////////////////////////////
VTRun *vtrun = VTRun::makeVTRun(                               // 07/21/03 AM.
	logfile,                // Verbose/error log file.          // 08/28/02 AM.
	rfbdir,                 // VisualText dir for RFB spec.     // 08/28/02 AM.
	true                    // Build silently.                  // 08/28/02 AM.
	);

//ALIST *alist = (ALIST *)vtrun->alist_;                       // 07/21/03 AM.


/////////////////////////////////////////////////
// INITIALIZE ANALYZER RUNTIME ENGINE
/////////////////////////////////////////////////
// Create and initialize an NLP object to manage text analysis.
// NOTE: This init will dynamically load the user extensions dll at
// anadir\user\debug\user.dll
NLP *nlp = vtrun->makeNLP(                                     // 07/21/03 AM.
                     anapath,ananame,develop,silent,compiled);  // 07/21/03 AM.

/////////////////////////////////////////////////
// SET UP THE KNOWLEDGE BASE
/////////////////////////////////////////////////

CG *cg = vtrun->makeCG(                                        // 07/21/03 AM.
         anapath,
         false,     // LOAD COMPILED KB IF POSSIBLE.
         nlp);      // Associated analyzer object.             // 07/21/03 AM.

if (!cg)                                                       // 07/21/03 AM.
   {
   _t_cerr << _T("[Couldn't make knowledge base.]") << endl;          // 07/21/03 AM.
   vtrun->deleteNLP(nlp);                                      // 07/21/03 AM.
   VTRun::deleteVTRun(vtrun);                                  // 07/21/03 AM.
   return -1;
   }

_t_cout << _T("[Loaded knowledge base.]") << endl;          // 02/19/19 AM.
// Root of the KB hierarchy.
CONCEPT *root = VTRun::getKBroot(cg);                          // 12/19/03 AM.


/////////////////////////////////////////////////
// BUILD ANALYZER APPLICATION
/////////////////////////////////////////////////
// Create an analyzer dynamically using the sequence file and rules files
// under anadir\spec.

if (!nlp->make_analyzer(seqfile, anapath, develop,
	silent,              // Debug/log file output.              // 06/16/02 AM.
   0,
   false,               // false == Don't compile during load.
   compiled))                    // Compiled/interp analyzer.
   {
   _t_cerr << _T("[Couldn't build analyzer.]") << endl;
   vtrun->deleteNLP(nlp);                                      // 07/21/03 AM.
   VTRun::deleteVTRun(vtrun);                                  // 07/21/03 AM.
   return -1;
   }


/////////////////////////////
// TEST RULE GENERATION.
/////////////////////////////
#ifdef TEST_RUG_
CONCEPT *gram = cg->findConcept(root, _T("gram"));
if (!gram)
   gram = cg->makeConcept(root, _T("gram"));
RUG::rugInit(gram,cg);

// Execute rule generation.
RUG *rug = new RUG(gram, nlp, anapath, cg);
rug->rugGenerate(false,false);      // Generate ALL.
delete rug;
rug = 0;
#endif

// Analyzer can output to a stream.
_TCHAR ofstr[MAXSTR];
#ifdef LINUX
_stprintf(ofstr,_T("./dummy.txt"));
#else
_stprintf(ofstr,_T("e:\\dummy.txt"));
#endif
_t_ofstream os(TCHAR2CA(ofstr), ios::out);												// 08/07/02 AM.

// Testing output to buffer.
_TCHAR obuf[MAXSTR];															// 05/11/02 AM.


/////////////////////////////////////////////////
// ANALYZE INPUT TEXT FILES
/////////////////////////////////////////////////

if (is_file(infile))      // Input is a single file.
   {
   // If input is from a buffer, specify its name and length.
   nlp->analyze(infile, outfile, anapath, develop,
      silent,        // Debug/log output files.                // 06/16/02 AM.
      0,            // Outdir.
      0,            // Input buffer.
      0,            // Length of input buffer, or 0.
      compiled,      // If running compiled analyzer.
		&os,				// Rebind cout output stream in analyzer  // 08/07/02 AM.
		obuf,																		// 05/11/02 AM.
		MAXSTR																	// 05/11/02 AM.
      );
   }
else      // Develop mode not on.
   {
   // Analyze file, directory, or tree of directories.
   analyze_path(nlp, infile, outfile, anapath,                 // 07/15/03 AM.
	               silent,0,compiled,&os,obuf,                 // 07/15/03 AM.
						true);
   }


// Can check that buffer was filled by analyzer calls such as
// cbuf() << "hello" << "\n";
//cerr << "obuf=";                                             // 05/11/02 AM.
//cerr << obuf << endl;                                        // 05/11/02 AM.


/////////////////////////////////////////////////
// CLEANUP VISUALTEXT RUNTIME
/////////////////////////////////////////////////

// This will close the user.dll for the application also.
vtrun->deleteNLP(nlp);                                         // 07/21/03 AM.
VTRun::deleteVTRun(vtrun);                                     // 07/21/03 AM.
object_counts();    // Report memory leaks to standard output.

return 0;
}

/********************************************
* FN:		CMDREADARGS
* CR:		11/30/98 AM.
* SUBJ:	Get command line arguments.
* RET:	True if ok read, else false if failed.
* NOTE:	06/16/02 AM. Renamed from dosReadargs, moved to user project.
********************************************/

bool cmdReadArgs(
	int argc,
	_TCHAR *argv[],
    _TCHAR* &anapath,	// Analzyer name
    _TCHAR* &input,		// Input file from args.
	_TCHAR* &output,	// Output file from args.
	bool &develop,		// Development mode (output intermediate files).
	bool &compiled,		// true - compiled ana. false=interp(DEFAULT).
	bool &silent		// true == only output files specified by analyzer.
	)
{
_TCHAR *ptr;
_TCHAR *ptrPrev;
_TCHAR **parg;
bool f_ana = false;
bool f_in  = false;
bool f_out = false;
bool flag  = false;
bool compiledck = false;	// If compiled/interp arg seen.
bool doubledash = false;

anapath = input = output = 0;
develop = false;				// Default is not development mode.
compiled = false;	// INTERP ANALYZER BY DEFAULT
silent = false;	// Produce debug files, etc. by default.		// 06/16/02 AM.

for (--argc, parg = &(argv[1]); argc > 0; --argc, ++parg)
	{
	// For each command line argument.
	//*gout << "command arg=" << *parg << endl;
	_t_cout << _T("[command arg: ") << *parg << _T("]") << endl;
	ptr = *parg;
	if (*ptr == '-')	// DOS or UNIX style arg.
		{
		if (flag)
			{
			_t_cerr << _T("[Error in command line args for ") << *ptr << _T("]") << endl;
			return false;
			}
		++ptr;
		doubledash = false;
		if (*ptr == '-') {
			doubledash = true;
			++ptr;
		}
		if (doubledash) {
			if (!strcmp_i(ptr, _T("help")))
				{
				cmdHelpargs(argv[0]);
				return false;
				}
			else if (!strcmp_i(ptr, _T("version")))
				{
				_t_cerr << _T("[Not implemented yet: --version") << endl;
				return false;
				}
		}
		else if (!strcmp_i(ptr, _T("ana")))
			f_ana = flag = true;					// Expecting input file.
		else if (!strcmp_i(ptr, _T("in")))
			f_in = flag = true;					// Expecting input file.
		else if (!strcmp_i(ptr, _T("out")))
			f_out = flag = true;					// Expecting output file.
		else if (!strcmp_i(ptr, _T("dev")))		// 12/25/98 AM.
			{
			if (silent)
				{
				_t_cerr << _T("[Ignoring /dev flag.]");
				develop = false;
				}
			else
				develop = true;					// Development mode.
			}
		else if (!strcmp_i(ptr, _T("silent")))	                     // 06/16/02 AM.
			{
			if (develop)
				{
				_t_cerr << _T("[Ignoring /dev flag.]");
				develop = false;
				}
			silent = true;                                        // 06/16/02 AM.
			}
		else if (!strcmp_i(ptr, _T("interp")))	// Run interpreted analyzer.
			{
			if (compiledck)
				_t_cerr << _T("[Ignoring extra /compiled or /interp flag.]");
			else
				{
				compiledck = true;
				compiled = false;
				}
			}
		else if (!strcmp_i(ptr, _T("compiled")))	// Run compiled analyzer.
			{
			if (compiledck)
				_t_cerr << _T("[Ignoring extra /compiled or /interp flag.]");
			else
				{
				compiledck = true;
				compiled = true;
				}
			}
		}
	else if (flag)							// Expected an argument value.
		{
		if (f_ana)
			{
			if (anapath)
				{
                _t_cerr << _T("[") << argv[0] << _T(": analyzer specified twice.]");
				cmdHelpargs(argv[0]);
				return false;
				}
            anapath = ptr;
			f_ana = flag = false;
        }
		else if (f_in)
			{
			if (input)
				{
				_t_cerr << _T("[") << argv[0] << _T(": Input file specified twice.]");
				cmdHelpargs(argv[0]);
				return false;
				}
			// Grab value as input file.
			input = ptr;
			f_in = flag = false;
			}
		else if (f_out)
			{
			if (output)
				{
				_t_cerr << _T("[") << argv[0]
						  << _T(": Output file specified twice.]");
				cmdHelpargs(argv[0]);
				return false;
				}
			// Grab value as output file.
			output = ptr;
			f_out = flag = false;
			}
		}
		else										// Got a "floating" value.
		{
			if (input && output)
				{
				_t_cerr << _T("[") << argv[0] << _T(": Extra arguments.]") << endl;
				cmdHelpargs(argv[0]);
				return false;
				}
			else if (input)
				output = ptr;
			else
				input = ptr;
		}
		ptrPrev = ptr;
	}
	return true;
}


/********************************************
* FN:		CMDHELPARGS
* CR:		11/30/98 AM.
* SUBJ:	Print command line argument help.
* RET:	True if ok read, else false if failed.
* NOTE:	06/16/02 AM. Renamed from dosHelpargs, moved to user project.
********************************************/

void cmdHelpargs(_TCHAR *name)
{
_t_cout << endl
	<< _T("usage: nlp [--version] [--help]") << endl
	<< _T("           [-INTERP][-COMPILED] INTERP is the default") << endl
	<< _T("           [-ANA analyzer] name or path to NLP++ analyzer folder") << endl
	<< _T("           [-IN infile] input text file") << endl
	<< _T("           [-OUT outfile] output text file") << endl
	<< _T("           [-DEV][-SILENT] /DEV generates logs, -SILENT (default) does not") << endl
	<< _T("           [infile [outfile]] when no /IN or -OUT specified") << endl
	<< endl
	<< _T("Directories in the nlp.exe files:") << endl
	<< _T("   data        nlp engine bootstrap grammar") << endl
	<< _T("   analyzers   default location for nlp++ analyzer folders") << endl
	<< endl;
}
