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

#include "lite/nlp_engine.h"
#include "version.h"

#define NLP_ENGINE_VERSION "2.14.3"

bool cmdReadArgs(int,_TCHAR*argv[],_TCHAR*&,_TCHAR*&,_TCHAR*&,_TCHAR*&,bool&,bool&,bool&);
void cmdHelpargs(_TCHAR*);

#ifdef LINUX
int main(
#else
int _tmain(
#endif
   int argc,
   _TCHAR *argv[],
   _TCHAR *envp[]
   )
{

/////////////////////////////////////////////////
// READ ARGUMENTS
/////////////////////////////////////////////////
// Need to get command line arguments and/or some kind of init file.
_TCHAR *analyzerpath=0, *input=0, *output=0, *workdir=0;
bool develop = false;    	// Development mode.
_TCHAR *sequence=0;
bool compiled=false;       	// Run compiled/interp analyzer.
bool silent=false;			// No log/debug output files.

/////////////////////////////////////////////////
// GET APP INFORMATION
/////////////////////////////////////////////////

// Get analyzer name, input and output filenames from command line.
if (!cmdReadArgs(argc,argv,analyzerpath,input,output,workdir,develop,compiled,silent))
   exit(1);

NLP_ENGINE *nlpEngine = new NLP_ENGINE(workdir);
nlpEngine->analyze(analyzerpath,input,output,develop,silent,compiled);
delete nlpEngine;

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
	_TCHAR* &workdir,	// Working directory from args.
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
bool f_work = false;
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
	//*gout << "command arg=" << *parg << std::endl;
	ptr = *parg;

	if ((ptr[0] == '"' && ptr[strlen(ptr) - 1] == '"') || (ptr[0] == '\'' && ptr[strlen(ptr) - 1] == '\'')) {
        ptr = &ptr[1];
        ptr[strlen(ptr) - 1] = '\0';
    }

	if (strcmp_i(ptr, _T("--version")) && strcmp_i(ptr, _T("--help")))
		std::_t_cout << _T("[command arg: ") << ptr << _T("]") << std::endl;

	if (*ptr == '-')	// DOS or UNIX style arg.
		{
		if (flag)
			{
			std::_t_cerr << _T("[Error in command line args for ") << *ptr << _T("]") << std::endl;
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
#ifdef LINUX
				//std::_t_cout << "Version " << Version() << std::std::endl;
				std::_t_cout << _T(NLP_ENGINE_VERSION) << std::endl;
#else
				std::_t_cout << _T(NLP_ENGINE_VERSION) << std::endl;
#endif
				return false;
				}
		}
		else if (!strcmp_i(ptr, _T("ana")))
			f_ana = flag = true;					// Expecting input file.
		else if (!strcmp_i(ptr, _T("in")))
			f_in = flag = true;					// Expecting input file.
		else if (!strcmp_i(ptr, _T("out")))
			f_out = flag = true;					// Expecting output file.
		else if (!strcmp_i(ptr, _T("work")))
			f_work = flag = true;					// Expecting output file.
		else if (!strcmp_i(ptr, _T("dev")))		// 12/25/98 AM.
			{
			if (silent)
				{
				std::_t_cerr << _T("[Ignoring /dev flag.]");
				develop = false;
				}
			else
				develop = true;					// Development mode.
			}
		else if (!strcmp_i(ptr, _T("silent")))	                     // 06/16/02 AM.
			{
			if (develop)
				{
				std::_t_cerr << _T("[Ignoring /dev flag.]");
				develop = false;
				}
			silent = true;                                        // 06/16/02 AM.
			}
		else if (!strcmp_i(ptr, _T("interp")))	// Run interpreted analyzer.
			{
			if (compiledck)
				std::_t_cerr << _T("[Ignoring extra /compiled or /interp flag.]");
			else
				{
				compiledck = true;
				compiled = false;
				}
			}
		else if (!strcmp_i(ptr, _T("compiled")))	// Run compiled analyzer.
			{
			if (compiledck)
				std::_t_cerr << _T("[Ignoring extra /compiled or /interp flag.]");
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
                std::_t_cerr << _T("[") << argv[0] << _T(": analyzer specified twice.]");
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
				std::_t_cerr << _T("[") << argv[0] << _T(": Input file specified twice.]");
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
				std::_t_cerr << _T("[") << argv[0]
						  << _T(": Output file specified twice.]");
				cmdHelpargs(argv[0]);
				return false;
				}
			// Grab value as output file.
			output = ptr;
			f_out = flag = false;
			}
		else if (f_work)
			{
			if (workdir)
				{
				std::_t_cerr << _T("[") << argv[0]
						  << _T(": Output file specified twice.]");
				cmdHelpargs(argv[0]);
				return false;
				}
			// Grab value as output file.
			workdir = ptr;
			f_work = flag = false;
			}
		}
		else										// Got a "floating" value.
		{
			if (input && output)
				{
				std::_t_cerr << _T("[") << argv[0] << _T(": Extra arguments.]") << std::endl;
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
std::_t_cout << std::endl
	<< _T("usage: nlp [--version] [--help]") << std::endl
	<< _T("           [-INTERP][-COMPILED] INTERP is the default") << std::endl
	<< _T("           [-ANA analyzer] name or path to NLP++ analyzer folder") << std::endl
	<< _T("           [-IN infile] input text file path") << std::endl
	<< _T("           [-OUT outdir] output directory") << std::endl
	<< _T("           [-WORK workdir] working directory") << std::endl
	<< _T("           [-DEV][-SILENT] -DEV generates logs, -SILENT (default) does not") << std::endl
	<< _T("           [infile [outfile]] when no -IN or -OUT specified") << std::endl
	<< std::endl
	<< _T("Directories in the nlp.exe files:") << std::endl
	<< _T("   data        nlp engine bootstrap grammar") << std::endl
	<< _T("   analyzers   default location for nlp++ analyzer folders") << std::endl
	<< _T("   visualtext  common files for the VisualText IDE") << std::endl
	<< std::endl;
}
