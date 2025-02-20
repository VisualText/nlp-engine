/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CG.CPP
* FILE:	c:/cs/libconsh/cg.cpp
* CR:		04/28/99 AM.
* SUBJ:	API class for Conceptual Grammar representation system (libconsh).
* NOTE:	May want to rename it libcg -- makes more sense.  Consh is a
*			user interface system.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include <filesystem>
#include "machine-min.h"					// 03/08/00 AM.
#include "prim/libprim.h"
#include "lite/global.h"
#include "cg_global.h"				// 09/16/99 AM.

#include "prim/unicode.h"					// 01/13/06 AM.
#include "prim/prim.h"
#include "prim/str.h"				// 04/30/99 AM.
#include "prim/list_s.h"
#include "prim/list.h"
#include "prim/dir.h"				// 05/06/99 AM.
#include "prim/dyn.h"				// 06/29/00 AM.
#include "prim/unicu.h"

#include "kbm/libkbm.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/kbm_kb.h"			// 04/30/99 AM.
#include "kbm/dict.h"			// 05/04/99 AM.
#include "kbm/Akbm.h"			// API for LIBKBM.					// 06/11/02 AM.

#include "ext/kb.h"

#include "consh/libconsh.h"
#include "consh/cc_var.h"
#include "consh/Aconsh.h"
#include "consh/cg.h"
#include "consh/bind.h"			// 05/02/99 AM.
#include "consh/consh_kb.h"	// 05/04/99 AM.

#include "io.h"
#include "cmd.h"
#include "dyn.h"					// 06/29/00 AM.
#include "lite/dir.h"

#include "prim/unicu.h"
using namespace unicu;

// Number of ind attr commands per file.								// 07/01/03 AM.
// Segmenting the attr commands into multiple cmd files.			// 07/01/03 AM.
#define ATTRS_PER_FILE 25000

int CG::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

CG::CG(
	_TCHAR *appdir,
	bool compiled,		// TRY loading compiled kb (kb.dll).		// 04/27/01 AM.
	ALIST *alist		// List Manager.									// 08/14/02 AM.
	)
{

// Init vars.																	// 08/22/02 AM.
cg_CONCEPT =
cg_PHRASE =
cg_ATOM =
nlp_PUNCT =
0;

dirty_ = false;																// 05/12/00 AM.
#ifndef LINUX
hkbdll_ = 0;																	// 06/29/00 AM.
#endif
appdir_[0] = '\0';
kbdir_[0] = '\0';
if (appdir && *appdir) {
	_tcscpy(appdir_, appdir);
	_stprintf(kbdir_, _T("%s%ckb%cuser"), appdir_, DIR_CH, DIR_CH);
} else
	{
	std::_t_cerr << _T("[CG: No app dir given.]") << std::endl;
	return;
	}
if (!appdir_ || !*appdir_)			// CONFIRM.							// 05/07/01 AM.
	{
	std::_t_cerr << _T("[CG: BAD APPDIR]") << std::endl;									// 05/07/01 AM.
	return;																		// 05/07/01 AM.
	}

if (!alist)																		// 08/14/02 AM.
	{
	std::_t_cerr << _T("[List manager needed for CG object.]") << std::endl;		// 08/14/02 AM.
	return;																		// 08/14/02 AM.
	}
alist_ = alist;																// 08/14/02 AM.

//cerr << "[CG got appdir=" << appdir_ << "]" << std::endl;			// 02/19/02 AM.

// DIVERT GLOBAL ERROR STREAM TO LOG FILE.							// 09/16/99 AM.
// Commented out. // 04/04/01 AM.
//sprintf(errout_, "%s%coutput%ccgerr.log", appdir_,				// 09/16/99 AM.
//			DIR_CH, DIR_CH);													// 03/08/00 AM.
// PUTTING THIS ELSEWHERE SO IT WON'T CAUSE TROUBLE FOR GUI.	// 04/04/01 AM.
// (For now, place at top level folder for analyzer app.			// 04/04/01 AM.
// Moving this to the logs directory.									// 02/21/02 AM.

// Allowing multiple KBs/instances to use the same log file.	// 08/22/02 AM.
if (count_ == 0)																// 08/22/02 AM.
	{
	_stprintf(errout_, _T("%s%clogs%ccgerr.log"),
													appdir_,DIR_CH,DIR_CH);	// 02/21/02 AM.
//	cgfileErr(errout_, /*DU*/ ferr_, serr_);							// 09/16/99 AM.
	cgfileErr(errout_);														// 07/18/03 AM.
	}
else																				// 08/22/02 AM.
	{
	*cgerr << _T("[Starting up KB number ") << (count_ + 1)			// 08/22/02 AM.
		<< _T("]") << std::endl;														// 08/22/02 AM.
	}

kbm_ = 0;
ast_ = 0;
aptr_ = 0;
asym_ = 0;
acon_ = 0;

// Create a KB Manager object to manage bindings to kb,etc.		// 06/11/02 AM.
kbm_ = new AKBM();															// 06/11/02 AM.

// Convenience references.													// 08/15/02 AM.
ast_ = kbm_->ast_;
aptr_ = kbm_->aptr_;
asym_ = kbm_->asym_;
acon_ = kbm_->acon_;

bool loaded = false;															// 06/29/00 AM.
//#ifndef LINUX

// TRY TO LOAD A COMPILED KNOWLEDGE BASE.								// 06/29/00 AM.
// Each library config will have a different DLL name.					// 01/21/06 AM.
#ifdef _DEBUG

#ifndef UNICODE
_TCHAR *fname = _T("kbd");										// kbd.dll		// 01/21/06 AM.
#else
_TCHAR *fname = _T("kbdu");									// kbdu.dll		// 01/17/06 AM.
#endif

#else	// RELEASE CONFIG

#ifndef UNICODE
_TCHAR *fname = _T("kb");										// kb.dll		// 01/21/06 AM.
#else
_TCHAR *fname = _T("kbu");										// kbu.dll		// 01/17/06 AM.
#endif

#endif

#ifdef EMBEDED_KB
if (compiled)																	// 04/27/01 AM.
	{
	*cgerr << _T("[CG: Trying to load compiled KB.]") << std::endl;		// 02/19/02 AM.
	_TCHAR bin[MAXSTR];		// The real file in bin directory.		// 05/06/01 AM.
#ifndef LINUX
	_stprintf(bin, _T("%s%cbin%c%s.dll"),									// 05/06/01 AM.
				appdir,DIR_CH,
				DIR_CH,
				fname);																// 01/21/06 AM.
#else
	_stprintf(bin, _T("%s%clib%c%s.a"),									// 05/06/01 AM.
				appdir,DIR_CH,
				DIR_CH,
				fname);																// 01/21/06 AM.
			*cgerr << _T("[Compiled kb=") << bin << _T("]") << std::endl;			// 02/19/02 AM.
#endif

// 02/19/19 AM.
#ifndef LINUX
	_TCHAR buf[MAXSTR];
	_stprintf(buf, _T("%s%ckb%c%s.dll"), appdir,						// 04/27/01 AM.
				DIR_CH,DIR_CH,													// 03/08/00 AM.
				fname);																// 01/21/06 AM.
#endif

// No longer having VisualText move the KB.DLL file around.				// 01/23/06 AM.
#ifdef _OLDWAY_
	// CHECK IF NEW LIB FILE EXISTS.										// 05/06/01 AM.
	if (f_exists(buf))														// 05/06/01 AM.
		{
		*cgerr << _T("[Found updated KB library.]") << std::endl;			// 02/19/02 AM.
		_TCHAR cmd[MAXSTR*2];													// 05/06/01 AM.
		// Delete the current kb.dll file, if in bin directory.	// 05/06/01 AM.
		if (f_exists(bin))													// 05/06/01 AM.
			{
			*cgerr << _T("[Deleting old kb library.]") << std::endl;			// 02/19/02 AM.
			_stprintf(cmd, _T("del /F /Q %s"), bin);							// 05/06/01 AM.
			if (_tsystem(cmd) < 0)												// 05/06/01 AM.
				{
				*cgerr << _T("[Error. Can't delete kb library.]")		// 05/06/01 AM.
						 << std::endl;
				goto interp;	// Recover.									// 05/06/01 AM.
				}
			}

		// Move from the project to the bin directory.				// 05/06/01 AM.
		*cgerr << _T("[Moving new KB library into place.]") << std::endl;	// 02/19/02 AM.
		_stprintf(cmd, _T("move %s %s"), buf, bin);							// 05/06/01 AM.
		if (_tsystem(cmd) < 0)													// 05/06/01 AM.
			{
			*cgerr << _T("[Error. Can't move kb library.]")				// 05/06/01 AM.
					 << std::endl;
			goto interp;		// Recover.									// 05/06/01 AM.
			}
		}
	else
#endif

// TRYING TO LOAD COMPILED KB IN LINUX.	// 02/19/19 AM.
// No compiled in Linux for now.	// 05/15/07 AM.
#ifndef LINUX
	if (!f_exists(bin))												// 05/06/01 AM.
		{
		// No kb.dll file exists for this config.						// 05/06/01 AM.
		*cgerr << _T("[Error. No compiled kb for ")						// 05/06/01 AM.
			<< fname
			<< _T(".dll .]")
			<< std::endl;
		loaded = false;														// 05/06/01 AM.
		goto interp;															// 05/06/01 AM.
		}
// ASSUME COMPILED KB.	// 02/19/19 AM.
#endif

	*cgerr << _T("[Loading compiled kb: ") << bin << _T("]") << std::endl;			// 05/06/01 AM.
#ifndef LINUX
	hkbdll_ = load_dll(bin);												// 05/06/01 AM.
	if (hkbdll_)
#else
   if (true)	// 11/13/06 AM.
#endif
		{
		*cgerr << _T("[Loaded compiled kb library]") << std::endl;			// 02/19/02 AM.
		loaded = true;
		// NEED TO BIND VARIABLES & WHATEVER ELSE IS NEEDED TO
		// GET THIS KB UP.
#ifndef LINUX
		if (!call_kb_setup(hkbdll_,this))								// 08/16/02 AM.
#else
		// if (false)	// 11/13/06 AM.
		if (!call_kb_setup(hkbdll_,(void*)this))								// 08/16/02 AM.
#endif
			{
			loaded = false;													// 05/07/01 AM.
			std::_t_cerr << _T("[Error in calling kb library.]") << std::endl;		// 05/07/01 AM.
			*cgerr << _T("[Error in calling kb library.]") << std::endl;	// 05/07/01 AM.
			goto interp;														// 05/07/01 AM.
			}
		cg_CONCEPT = 1;		// BIND ROOT OF KB.						// 06/29/00 AM.
		if (!bind_all(this))
			{
			*cgerr << _T("[Couldn't bind vars to compiled kb.]") << std::endl;
			loaded = false;
#ifndef LINUX
			unload_dll(hkbdll_);
#endif
			}
		}
	else																			// 05/07/01 AM.
		{
		std::_t_cerr << _T("[Error. Failed to load KB library.]") << std::endl;	// 05/07/01 AM.
		*cgerr << _T("[Error. Failed to load KB library.]") << std::endl;	// 02/19/02 AM.
		}
interp:																			// 05/06/01 AM.
	;		// Empty statement for the label :-)						// 05/06/01 AM.
	}
//#endif
#endif

// THIS IS THE HARD-WIRED INITIALIZATION OF AN EMPTY KB.
if (!loaded)		// No kb.dll file was loaded.						// 06/29/00 AM.
	{
	*cgerr << _T("[Loading interpreted kb.]") << std::endl;					// 07/25/00 AM.
//	cerr << "[Loading interpreted kb.]" << std::endl;						// 05/07/01 AM.
	if (!consh_ini(this))													// 08/15/02 AM.
		*cgerr << _T("[ERROR INITIALIZING KB.]") << std::endl;				// 05/05/01 AM.
	}
// Now the KB user must hand it a List Manager.						// 08/14/02 AM.
//else										// FIX.	// 09/17/01 DD.	// 08/14/02 AM.
//	alist_->list_ini();					// FIX.	// 09/17/01 DD.	// 08/14/02 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

CG::~CG()
{
#ifdef DEBUGGING_
asym_->sym_pp(cgerr);														// 06/29/03 AM.
#endif

consh_clean(this);

if (kbm_)																		// 06/11/02 AM.
	delete kbm_;																// 06/11/02 AM.

// alist_ is just a reference.  DO NOT FREE IT.						// 08/14/02 AM.

// RESET ERROR STREAM.														// 09/16/99 AM.

if (count_ == 1)																// 08/22/02 AM.
//	cgresetErr(ferr_, serr_);												// 09/16/99 AM.
	cgresetErr();																// 07/18/03 AM.
else
	*cgerr << _T("[Closing KB number ") << count_ << _T("]") << std::endl;	// 08/22/02 AM.

#ifndef LINUX
// FREE UP COMPILED KB, IF ANY.											// 06/29/00 AM.
if (hkbdll_)																	// 06/29/00 AM.
	{
	unload_dll(hkbdll_);														// 06/29/00 AM.
	hkbdll_ = 0;																// 06/29/00 AM.
	}
#endif

#ifndef STABLE_
--count_;
#endif
}

/********************************************
* FN:		MAKECG, DELETECG
* CR:		07/28/03 AM..
* NOTE:	Class function.
********************************************/

CG *CG::makeCG(                                                // 07/28/03 AM.
	   ALIST *alist,
      _TCHAR *appdir,
		bool compiled
		)
{
return new CG(appdir,compiled,alist);
}

void CG::deleteCG(CG *cg)                                      // 07/28/03 AM.
{
if (cg)
	delete cg;
}


/********************************************
* FN:		Access Functions
********************************************/

_TCHAR	*CG::getAppdir()	{return appdir_;}
bool	 CG::getDirty()	{return dirty_;}								// 05/12/00 AM.
AKBM	*CG::getKBM()		{return kbm_;}									// 06/11/02 AM.
ALIST *CG::getALIST()	{return alist_;}								// 08/14/02 AM.
#ifndef LINUX
HINSTANCE CG::getHkbdll()	{return hkbdll_;}
#endif


/********************************************
* FN:		Class-wide Functions
********************************************/
void	CG::setAppdir(_TCHAR *x)	{_tcscpy(appdir_, x);	}
void	CG::setDirty(bool x)		{dirty_ = x;			}				// 05/12/00 AM.
void	CG::setKBM(AKBM *x)		{kbm_	= x;				}				// 06/11/02 AM.
void	CG::setALIST(ALIST *x)	{alist_ = x;			}
#ifndef LINUX
void	CG::setHkbdll(HINSTANCE x) {hkbdll_ = x;}
#endif


/********************************************
* FN:		GETCOUNT
* CR:		12/14/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int CG::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void CG::prettyCount()
{
if (count_)
	std::_t_cout << _T("Active CG count=") << count_ << std::endl;
}
#endif


	/////////////////////
	// OVERALL KB FUNS
	/////////////////////

/********************************************
* FN:		WRITEKB
* SUBJ:	Write the knowledge base in memory to files.
* CR:		04/28/99 AM.
* NOTE:	Save latest KB (from memory to files).
*			If dir is present, kb is written to $appdir/kb/dir.
* METH:	Traverse the concept hierarchy.  Write out one file for:
*			hier, word, attr, phrase.
********************************************/

bool CG::writeKB(_TCHAR *dir)
{
*cgerr << _T("[In writeKB]") << std::endl;
clock_t s_time, e_time;										// 10/20/99 AM.
s_time = clock();												// 10/20/99 AM.

if (!dir || !*dir)
	{
	*cgerr << _T("[writeKB: Given no output directory.]") << std::endl;
	return false;
	}

// 10/20/99 AM. Moved this up so kb files won't get wiped out if there's
// a problem.
// Grab hold of the root of the hierarchy.
CONCEPT *root;
if (!(root = findRoot()))
	{
	*cgerr << _T("[writeKB: No root of concept hierarchy.]") << std::endl;
	return false;
	}

// TRAP A BAD KB WRITE!!!								// 10/20/99 AM.
if (!findConcept(root, _T("gram")))						// 10/20/99 AM.
	{
	*cgerr << _T("[writeKB: No GRAM concept in hierarchy.]") << std::endl;
	return false;
	}

// For example, path= C:\apps\Resume\kb\user.
_TCHAR *kbdir = _T("kb");
_TCHAR path[MAXPATH];
_stprintf(path, _T("%s%c%s%c%s"), getAppdir(), DIR_CH, kbdir, DIR_CH, dir);
*cgerr << _T("[writeKB: path=") << path << _T("]") << std::endl;				// 02/19/02 AM.

// If directory doesn't exist, create it.
make_dir(path);	// 05/06/99 AM.

// Prep and open up the output files.
_TCHAR o_hier[MAXPATH*2];
_TCHAR o_word[MAXPATH*2];
_TCHAR o_attr[MAXPATH*2];
_TCHAR o_phr[MAXPATH*2];

_TCHAR *suff;
suff = _T("kb");		// Kb file suffix.

_stprintf(o_hier, _T("%s%chier.%s"), path, DIR_CH, suff);
_stprintf(o_word, _T("%s%cword.%s"), path, DIR_CH, suff);
_stprintf(o_phr,  _T("%s%cphr.%s"),  path, DIR_CH, suff);

// Open the files for output.
std::_t_ofstream f_hier(TCHAR2A(o_hier));
std::_t_ofstream f_word(TCHAR2A(o_word));
std::_t_ofstream f_phr(TCHAR2A(o_phr));

// May open multiples of these as needed.								// 07/01/03 AM.
long n_attr = 1;	// Count attr#.kb files.							// 07/01/03 AM.
long c_attr = 0;	// Count of attributes.								// 07/01/03 AM.
_stprintf(o_attr, _T("%s%cattr%ld.%s"),path,DIR_CH,n_attr,suff);		// 07/01/03 AM.
std::_t_ofstream *f_attr = new std::_t_ofstream(TCHAR2A(o_attr));								// 07/01/03 AM.

// KB BOOTSTRAP COMMANDS. (Axiomatics)									// 08/06/01 AM.
// (Moved out of writeTree).												// 08/06/01 AM.
f_hier << _T("add empty") << std::endl;											// 08/06/01 AM.
f_hier << _T("add root") << std::endl;												// 08/06/01 AM.

// Traverse the concept hierarchy.  Write out the goodies.
if (!writeTree(root, true, f_hier,f_word,f_phr, _T(""),
			f_attr,n_attr,c_attr,o_attr,path,suff))					// 07/01/03 AM.
	{
	delete f_attr;																// 08/07/03 AM.
	return false;
	}

// Terminate the command files.			// 05/02/99 AM.
f_hier << std::endl << _T("quit") << std::endl << std::endl;
f_word << std::endl << _T("quit") << std::endl << std::endl;
f_phr  << std::endl << _T("quit") << std::endl << std::endl;

// Shut down the last attr kb file.										// 07/01/03 AM.
*f_attr << std::endl << _T("quit") << std::endl << std::endl;							// 07/01/03 AM.
delete f_attr;																	// 07/01/03 AM.
f_attr = 0;																		// 07/01/03 AM.

// Write a top-level command file						// 07/01/03 AM.
writeKBmain(n_attr,path,suff);							// 07/01/03 AM.

e_time = clock();												// 10/20/99 AM.
*cgerr << _T("[WRITE KB time=")								// 10/20/99 AM.
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC	// 10/20/99 AM.
	  << _T(" sec]") << std::endl;									// 10/20/99 AM.

return true;
}

/********************************************
* FN:		WRITEKBMAIN
* SUBJ:	Write top-level KB command file.
* CR:		07/01/03 AM.

********************************************/

bool CG::writeKBmain(
	long n_attr,	// Number of attr files.
	_TCHAR *path,		// Path for files.
	_TCHAR *suff		// File suffix.
	)
{
_TCHAR o_main[4096];
_stprintf(o_main, _T("%s%cmain.%s"), path, DIR_CH, suff);
std::_t_ofstream f_main(TCHAR2A(o_main));

#ifdef LINUX
f_main << _T("take \"kb\\\\user\\\\hier.kb\"") << std::endl;
f_main << _T("bind sys") << std::endl;
f_main << _T("take \"kb\\\\user\\\\word.kb\"") << std::endl;
f_main << _T("take \"kb\\\\user\\\\phr.kb\"") << std::endl;

long ii;
for (ii=1; ii <= n_attr; ++ii)
	f_main
		<< _T("take \"kb\\\\user\\\\attr")
		<< ii
		<< _T(".kb\"")
		<< std::endl;
f_main << _T("quit") << std::endl;
#else

f_main << _T("take \"kb/user/hier.kb\"") << std::endl;
f_main << _T("bind sys") << std::endl;
f_main << _T("take \"kb/user/word.kb\"") << std::endl;
f_main << _T("take \"kb/user/phr.kb\"") << std::endl;

long ii;
for (ii=1; ii <= n_attr; ++ii)
	f_main
		<< _T("take \"kb/user/attr")
		<< ii
		<< _T(".kb\"")
		<< std::endl;
f_main << _T("quit") << std::endl;
#endif

return true;
}


/********************************************
* FN:		READKB
* SUBJ:	Read the knowledge base from files to memory.
* CR:		05/02/99 AM.
********************************************/

bool CG::readKB(_TCHAR *dir)
{
*cgerr << _T("[readKB:]") << std::endl;											// 02/19/02 AM.
clock_t s_time;										// 10/20/99 AM.
s_time = clock();												// 10/20/99 AM.

if (!dir || !*dir)
	{
	*cgerr << _T("[readKB: Given no input directory.]") << std::endl;
	return false;
	}

if (!kbm_ || !ast_ || !aptr_ || !asym_ || !acon_)	// 07/29/03 AM.
	{
	*cgerr << _T("[readKB: Internal error. KB tables not initialized.]") << std::endl;
	return false;
	}

// ASSUME COMPILED KB.	// 02/19/19 AM.
// #ifndef LINUX
hkbdll_ = 0;
if (hkbdll_)
	{
	*cgerr << _T("[Using compiled kb. readKB ignored.]") << std::endl;	// 06/29/00 AM.
	return true;		// This is just info, ok.						// 06/29/00 AM.
	}
// ASSUME COMPILED KB.	// 02/19/19 AM.
//#endif

// For example, path= C:\apps\Resume\kb\user.
_TCHAR *kbdir = _T("kb");
_TCHAR path[MAXPATH];
_stprintf(path, _T("%s%c%s%c%s"), getAppdir(),DIR_CH, kbdir,DIR_CH, dir);
// 08/11/99 AM. Checking out Purify warnings.
*cgerr << _T("[readKB: path=")													// 02/19/02 AM.
		 << path
		 << _T("]")
		 << std::endl;

// Prep and open up the input file.
// hier, word, phr, attr.
_TCHAR infile[MAXPATH*2];
_TCHAR *suff;
suff = _T("kb");		// Kb file suffix.
std::vector<std::filesystem::path> kbfiles;
std::vector<std::filesystem::path> dictfiles;
bool kbLoaded = false;
bool bound = false;

openKBB(kbfiles);
openDict(dictfiles);

if (kbfiles.size() > 0) {
	if (!bound) {
		_stprintf(infile, _T("%s%chier.%s"), path,DIR_CH, suff);
		if (!readFile(infile))
			return false;
		bind_sys(this);
		con_add_root(this);
	}
	readKBBs(kbfiles);
	outputTime(_T("[READ kbb files time="),s_time);
	s_time = clock();
	kbLoaded = true;
}

if (dictfiles.size() > 0) {
	_stprintf(infile, _T("%s%chier.%s"), path,DIR_CH, suff);
	if (!readFile(infile))
		return false;
	con_add_root(this);
	bind_sys(this);
	bound = true;
	readDicts(dictfiles,kbfiles);
	outputTime(_T("[READ dict files time="),s_time);
	s_time = clock();
	kbLoaded = true;
}

if (!kbLoaded) {

	// Using a master take file for readin kb.			// 07/01/03 AM.
	_stprintf(infile, _T("%s%cmain.%s"),path,DIR_CH,suff);	// 07/01/03 AM.
	if (f_exists(infile))										// 07/01/03 AM.
		{
		if (!readFile(infile))									// 07/01/03 AM.
			return false;											// 07/01/03 AM.
		}
	else
		{
		// The old way.											// 07/01/03 AM.

		// Read in the HIER.KB file.
		_stprintf(infile, _T("%s%chier.%s"), path,DIR_CH, suff);
		if (!readFile(infile))
			return false;

		// BIND SYSTEM CONCEPTS TO CODE VARIABLES.
		bind_sys(this);

		// Read in the WORD.KB file.
		_stprintf(infile, _T("%s%cword.%s"), path, DIR_CH, suff);
		if (!readFile(infile))
			return false;

		// Read in the PHR.KB file.
		_stprintf(infile, _T("%s%cphr.%s"), path, DIR_CH, suff);
		if (!readFile(infile))
			return false;

		// Read in the ATTR.KB file.
		_stprintf(infile, _T("%s%cattr.%s"), path, DIR_CH, suff);
		if (!readFile(infile))
			return false;
			
		outputTime(_T("Original KB files"),s_time);
	}
}

return true;
}

void CG::outputTime(_TCHAR *timeMsg, clock_t s_time) {
clock_t e_time = clock();
*cgerr << timeMsg << _T(" ")
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC
	  << _T(" sec]") << std::endl;
}

/********************************************
* FN:		READSYSTEMKB
* SUBJ:	Read in the "factory default" knowledge base from files.
* CR:		05/03/99 AM.
* NOTE:	Same as readKB("sys");
********************************************/

bool CG::readSystemKB()
{
return readKB(_T("sys"));
}

/********************************************
* FN:		GENKB
* SUBJ:	Generate code for the knowledge base tables.
* CR:		06/28/00 AM.
* NOTE:	REVIVING VERY OLD CODE!
*			First will dump files to a local tmp directory.  If everything
*			ok, then copy files to the app/kb folder.  Compile into kb.dll.
* METH:	Traverse the concept hierarchy.  Write out one file for:
*			hier, word, attr, phrase.
********************************************/

bool CG::genKB()
{
// For example, path= d:\apps\resume\kb\ .
_TCHAR *kbdir = _T("kb");
_TCHAR path[MAXPATH];
_stprintf(path, _T("%s%c%s"), getAppdir(),DIR_CH, kbdir);

if (!cmd_gen_all(path,0,cgerr,this))
	return false;
// Move generated code to cg folder.
// Optionally compile here.
return true;
}


/********************************************
* FN:		COMPILEKB
* SUBJ:	Compile the knowledge base library.
* CR:		09/13/00 AM.
* NOTE:	Call genKB first.
*			First will dump files to a local tmp directory.  If everything
*			ok, then copy files to the app/kb folder.  Compile into kb.dll.
* METH:	Traverse the concept hierarchy.  Write out one file for:
*			hier, word, attr, phrase.
********************************************/

bool CG::compileKB(bool releaseflag)
{

_TCHAR cmd[MAXSTR];
cmd[0] = '\0';
#ifndef LINUX
if (releaseflag)
	_stprintf(cmd,
// Generating and using makefile.										// 07/28/01 AM.
//		"msdev %s%ckb%ckb.dsp /MAKE \"kb - Win32 Release\" /REBUILD",
//		getAppdir(), DIR_CH,DIR_CH);
		_T("%s%ckb%ckb_release.bat"), getAppdir(), DIR_CH, DIR_CH);
else
	_stprintf(cmd,
// Generating and using makefile.										// 07/28/01 AM.
//		"msdev %s%ckb%ckb.dsp /MAKE \"kb - Win32 Debug\" /REBUILD",
//		"NMAKE /f \"%s%ckb%ckb.mak\" CFG=\"kb - Win32 Debug\" > c:\\xxx.txt",
//		getAppdir(), DIR_CH,DIR_CH);
		_T("%s%ckb%ckb_debug.bat"), getAppdir(), DIR_CH, DIR_CH);
if (_tsystem(cmd) < 0)															// 04/27/01 AM.
	return false;																// 04/27/01 AM.
#endif
return true;
}


/********************************************
* FN:		UNLOADLIB
* SUBJ:	Unload the knowledge base library.
* CR:		09/13/00 AM.
* WARN:	Code may still have bindings into the kb!
*			Just a primitive function.
********************************************/

bool CG::unloadLib()
{
#ifndef LINUX
// FREE UP COMPILED KB, IF ANY.
if (hkbdll_)
	{
	unload_dll(hkbdll_);
	hkbdll_ = 0;
	}
#endif
return true;
}


/********************************************
* FN:		LOADLIB
* SUBJ:	Load the knowledge base library.
* CR:		09/13/00 AM.
* WARN:	Doesn't do anything with code bindings into the kb!
*			Just a primitive function.
********************************************/

bool CG::loadLib()
{
#ifndef LINUX
if (hkbdll_)
	{
	*cgerr << _T("[Load kb lib: Already loaded.]") << std::endl;
	return false;
	}

// TRY TO LOAD A COMPILED KNOWLEDGE BASE.
#ifdef _DEBUG

#ifndef UNICODE
_TCHAR *fname = _T("kbd");										// kbd.dll		// 01/21/06 AM.
#else
_TCHAR *fname = _T("kbdu");									// kbdu.dll		// 01/17/06 AM.
#endif

#else	// RELEASE CONFIG

#ifndef UNICODE
_TCHAR *fname = _T("kb");										// kb.dll		// 01/21/06 AM.
#else
_TCHAR *fname = _T("kbu");										// kbu.dll		// 01/17/06 AM.
#endif

#endif

_TCHAR buf[MAXSTR];
_stprintf(buf, _T("%s%ckb%c%s.dll"),										// 04/27/01 AM.
			getAppdir(),
			DIR_CH,DIR_CH,
			fname);																		// 01/21/06 AM.

if (!f_exists(buf))															// 05/06/01 AM.
	{
	*cgerr << _T("[loadLib: Error. Can't load compiled ")			// 05/06/01 AM.
			 << fname															// 01/21/06 AM.
			 << _T(".dll]")													// 01/21/06 AM.
			 << std::endl;
	return false;																// 05/06/01 AM.
	}

*cgerr << _T("[Loading compiled kb: ") << buf << _T("]") << std::endl;
hkbdll_ = load_dll(buf);
#endif
return true;
}

	//////////////////////
	// FIND THINGS
	//////////////////////

// Find the root concept of hierarchy.
CONCEPT *CG::findRoot()
{
return (CONCEPT *) kbm_->acon_->c_cg_CONCEPT;						// 08/22/02 AM.
}

// Find the named concept under given parent.
// Wend your way down from "concept", the root concept.
CONCEPT *CG::findConcept(
	CONCEPT *parent,
	_TCHAR *name
	)
{
return (CONCEPT *) kbm_->acon_->con_get_child((CON *)parent, name);
}

// Find the nth concept under parent.
CONCEPT *CG::findConcept(CONCEPT *parent, int pos)
{
CON *phr;
if (!parent)
	return 0;
phr = ((CON *) parent)->dn;
if (!phr)
	return 0;
return (CONCEPT *) kbm_->acon_->con_nth((CON *)phr, pos);
}

// Find concept's attribute.				// 07/12/99 AM.
ATTR *CG::findAttr(CONCEPT *conc, _TCHAR *name)
{
return kbm_->attr_find_h((CON *)conc, name);
}

// Fetch the list of attributes for the concept.
ATTR *CG::findAttrs(CONCEPT *conc)
{
if (!conc)
	return (ATTR *) 0;
return (ATTR *) ((CON *)conc)->attrs;
}

// Fetch attribute name into given buffer.	// 07/05/99 AM.
bool CG::attrName(ATTR *attr, _TCHAR *buf, int bufsiz)
{
*buf = '\0';

if (!attr)
	return false;

_TCHAR *name;
if (!(name = kbm_->attrs_name((PTR *) attr)))
	return false;

if (_tcsclen(name) >= (unsigned)bufsiz)
	return false;
_tcscpy(buf, name);
return true;
}

int CG::attrValType(CONCEPT *con, _TCHAR *attr) {
int type = 0;
PTR *pval;
if (!(pval = kbm_->attr_find((CON *)con, attr)))
	return false;
switch (pval->kind)
	{
	case pST:
		type = 0;
		break;
	case pNUM:
		type = 1;
		break;
	case pCON:
		type = 2;
		break;
	case pFLOAT:
		type = 3;
		break;
	default:
		break;
	}

return type;
}

// Fetch attribute values.				// 07/05/99 AM.
VAL *CG::attrVals(ATTR *attr)
{
return kbm_->attrs_vals((PTR *) attr);
}

// Fetch the list of values of concept's attribute.
VAL *CG::findVals(CONCEPT *conc, _TCHAR *s_attr)
{
//return (VAL *) attr_get((CON *)conc, s_attr);		// FIX.	// 08/12/99 AM.
return (VAL *) kbm_->attr_find((CON *)conc, s_attr);		// FIX.	// 08/12/99 AM.
}

// Fetch the first value of given attribute of a concept.
// Treat as appropriate type if possible.
bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ _TCHAR *val)
{
*val = '\0';

PTR *pval;
//if (!(pval = attr_get((CON *)conc, s_attr)))	// FIX.	// 08/12/99 AM.
if (!(pval = kbm_->attr_find((CON *)conc, s_attr)))	// FIX.	// 08/12/99 AM.
	return false;
if (pval->kind != pST)
	{
	*cgerr << _T("[findVal: Attribute value not a string.]") << std::endl;
	return false;
	}
_tcscpy(val, pval->v.vst);
return true;
}


bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ long long &val)
{
val = -1;

PTR *pval;
//if (!(pval = attr_get((CON *)conc, s_attr)))	// FIX.	// 08/12/99 AM.
if (!(pval = kbm_->attr_find((CON *)conc, s_attr)))	// FIX.	// 08/12/99 AM.
	return false;
switch (pval->kind)	// 12/27/06 AM.
	{
	case pNUM:			// 12/27/06 AM.
		val = pval->v.vnum;
		break;
	case pFLOAT:		// 12/27/06 AM.
		val = (long long) pval->v.vfloat;	// Truncate/convert.	// 12/27/06 AM.
		break;
	default:				// 12/27/06 AM.
		*cgerr << _T("[findVal: Attribute value not a number.]") << std::endl;
		return false;
		break;
	}
return true;
}

// VARIANT. 08/16/01 AM.
bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ float &val)
{
val = -1.0;

PTR *pval;
if (!(pval = kbm_->attr_find((CON *)conc, s_attr)))
	return false;
switch (pval->kind)	// 12/27/06 AM.
	{
	case pFLOAT:		// 12/27/06 AM.
		val = pval->v.vfloat;
		break;
	case pNUM:			// 12/27/06 AM.
		val = pval->v.vnum;	// Convert.	// 12/27/06 AM.
		break;
	default:				// 12/27/06 AM.
		*cgerr << _T("[findVal: Attribute value not a float.]") << std::endl;
		return false;
		break;
	}
return true;
}

// VARIANT.	// 08/12/00 AM.
bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ CONCEPT* &val)
{
val = 0;

PTR *pval;
if (!(pval = kbm_->attr_find((CON *)conc, s_attr)))
	return false;
if (pval->kind != pCON)
	{
	*cgerr << _T("[findVal: Attribute value not a concept.]") << std::endl;
	return false;
	}
val = pval->v.vcon;
return true;
}

// GET STRING-VALUED ATTRIBUTE WITHOUT COPYING.					// 11/12/00 AM.
// OPT: Strings are already "interned" in the kb, so don't
// need to copy them.
_TCHAR *CG::strVal(CONCEPT *conc, _TCHAR *s_attr)
{
PTR *pval;
if (!(pval = kbm_->attr_find((CON *)conc, s_attr)))	// FIX.	// 08/12/99 AM.
	return 0;
if (pval->kind != pST)
	{
	*cgerr << _T("[strVal: Attribute value not a string.]") << std::endl;
	return 0;
	}
return pval->v.vst;
}


// Look for an attribute up the hierarchy. (INHERIT).	// 08/12/99 AM.
// hier is the top of the concept hierarchy to be searched.
// hier = 0 means search up to root of the entire tree.
// RET:	false if no value was found or other error.
//			Oh yeah, this one returns a boolean value.
// Boolean return is now just if attr was found.					// 02/05/01 AM.
bool CG::inheritVal(
	CONCEPT *con,
	_TCHAR *attr,
	CONCEPT *hier,
	/*UP*/
	_TCHAR *valbuf
	)
{
*valbuf = '\0';
if (!con || !attr || !*attr)
	return false;
if (!hier)
	hier = findRoot();	// Root of entire hierarchy.


// Traverse up the tree, starting with current concept.
for (; con; con = Up(con))
	{
	*valbuf = '\0';
	findVal(con, attr, /*UP*/ valbuf);
	if (valbuf && *valbuf)
		{
//		if (!strcmp(valbuf, "true"))										// 02/05/01 AM.
//			return true;														// 02/05/01 AM.
//		else																		// 02/05/01 AM.
//			return false;														// 02/05/01 AM.
		return true;															// 02/05/01 AM.
		}
	if (con == hier)		// Reached the top.
		return false;
	}
return false;			// Error to be here.
}


// Find hierarchy concept.  Given a name, look through the given subtree
// for it.  Also look through node names.  If a node, return its owning
// concept.  If tree == 0, start at root of concept tree.
CONCEPT *CG::findHierConcept(_TCHAR *name, CONCEPT *tree)
{
if (!name || !*name)
	return 0;
if (!tree)
	tree = findRoot();

return hierConcept(name, tree, true);
}

// Recursive workhorse for findHierConcept.
CONCEPT *CG::hierConcept(_TCHAR *name, CONCEPT *tree, bool isroot)
{
if (!tree)
	return 0;

// Look at me.
//char buf[128];
_TCHAR buf[MAXSTR];	// 12/17/03 AM.
conceptName(tree, /*UP*/ buf);
if (!_tcscmp(name, buf))
	return tree;

// Look at my phrase.
CON *node;
node = (CON *) findPhrase(tree);
for (; node; node = node->next)
	{
	conceptName(node, /*UP*/ buf);
	if (!_tcscmp(name, buf))
		return tree;				// Return the concept owning the phrase.
	}

// If not root and first in list, look at my siblings.
CON *tmp = NULL;
CONCEPT *found;
if (!isroot && !((CON *)tree)->prev)
	{
	for (tmp = ((CON *)tree)->next; tmp; tmp = tmp->next)
		{
		if ((found = hierConcept(name, tmp, false)))
			return found;
		}
	}

// Look at my children.
return hierConcept(name, ((CON *)tree)->dn, false);
}


/********************************************
* FN:		SORTHIER
* SUBJ:	Sort a subhierarchy in the KB.
* CR:		02/20/09 AM.
* RET:	true if ok, else false.
* NOTE:	Each local list of concepts made alphabetical.
********************************************/

bool CG::sortHier(CONCEPT *tree)
{
if (!tree)
	return false;

return sort_hier_rec(tree, true);
}

// Recursive workhorse for sortHier.
bool CG::sort_hier_rec(CONCEPT *tree, bool isroot)
{
if (!tree)
	return true;


// Look at my children.
sort_hier_rec(((CON *)tree)->dn, false);

// Look at me.
sortChilds(tree);

// If not root and first in list, look at my siblings.
CON *tmp = NULL;
bool ok = true;
if (!isroot && !((CON *)tree)->prev)
	{
	for (tmp = ((CON *)tree)->next; tmp; tmp = tmp->next)
		{
		// Current sibling.
		sort_hier_rec((CON *)tmp, true);
		}
	}
return true;
}


/********************************************
* FN:		CONCEPTNAME
* SUBJ:	Get name of given concept.
* CR:		04/28/99 AM.
* ARG:	User supplies buffer.
********************************************/

bool CG::conceptName(CONCEPT *con, /*UP*/ _TCHAR *name,
	long bufsiz)																// 06/18/02 AM.
{
*name = '\0';

if (!con)
	return false;

_TCHAR *tmp;
SYM *sym;

switch (((CON *)con)->kind)
	{
	case cBASIC:
		sym = ((CON *)con)->v.vname;
		break;
	case cWORD:
		sym = ((CON *)con)->v.vsym;
		break;
	case cPROXY:
		CON *owner;
		owner = ((CON *)con)->v.vcon;
		sym = owner->v.vsym;
		break;
	case cNO_CON:
	default:
		*cgerr << _T("[conceptName: Not basic or word concept.]") << std::endl;
		return false;
	}

if (!sym)
	{
	*cgerr << _T("[conceptName: Concept has no sym.]") << std::endl;
	return false;
	}
tmp = sym->str;
if (!tmp)
	{
	*cgerr << _T("[conceptName: Concept has no name.]") << std::endl;
	return false;
	}

long len = _tcsclen(tmp);														// 06/18/02 AM.
if (len >= bufsiz)															// 06/18/02 AM.
	{
	_tcsnccpy(name,tmp, bufsiz-1);											// 06/18/02 AM.
	name[bufsiz-1] = '\0';	// Terminate.								// 06/18/02 AM.
	}
else																				// 06/18/02 AM.
	_tcscpy(name, tmp);
return true;
}


/********************************************
* FN:		CONCEPTPATH
* SUBJ:	Get path to given concept.
* CR:		07/21/99 AM.
* ARG:	User supplies buffer and buffer size.
* NOTE:	Path is a string of double-quoted concept names separated
*			by blanks.  eg "root" "gram" "Simple".
* WARN:	bufsize not handled internally.
********************************************/

bool CG::conceptPath(CONCEPT *conc, /*DU*/ _TCHAR *buf, int bufsize)
{
*buf = '\0';
if (!conc || bufsize < PATHSIZ)
	return false;
kbm_->acon_->con_to_path((CON *) conc, /*DU*/ buf);
if (!*buf)
	return false;
buf[bufsize-1] = '\0';	// At least be sure the string terminates somewhere.
return true;
}


/********************************************
* FN:		PATHCONCEPT
* SUBJ:	Convert concept to path.
* CR:		04/25/00 AM.
* NOTE:	Path is a string of double-quoted concept names separated
*			by blanks.  eg "root" "gram" "Simple".
********************************************/

CONCEPT *CG::pathConcept(_TCHAR *path)
{
if (!path || !*path)
	return 0;
return kbm_->acon_->path_to_con(path);
}


/********************************************
* FN:		WORDPATH
* SUBJ:	Get path to given word's concept.
* CR:		03/06/00 AM.
* ARG:	User supplies buffer and buffer size.
* NOTE:	Path is a string of double-quoted concept names separated
*			by blanks.  eg "root" "gram" "Simple".
* WARN:	bufsize not handled internally.
********************************************/

bool CG::wordPath(_TCHAR *str, /*DU*/ _TCHAR *buf, int bufsize)
{
*buf = '\0';
if (!str || !*str || bufsize < PATHSIZ)
	return false;

#ifdef OLD_030629_
if (!(word = kbm_->dict_find_word(str)))
	{
	if (!(word = kbm_->dict_add_word(str)))									// 05/12/00 AM.
		return false;
	dirty_ = true;																// 05/12/00 AM.
	}
#endif

bool dirt;																		// 06/29/03 AM.
CON *word = kbm_->dict_get_word(str,dirt);							// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return false;																// 06/29/03 AM.
if (dirt)																		// 06/29/03 AM.
	dirty_ = true;																// 06/29/03 AM.

kbm_->acon_->con_to_path(word, /*DU*/ buf);
if (!*buf)
	return false;
buf[bufsize-1] = '\0';	// At least be sure the string terminates somewhere.
return true;
}


/********************************************
* FN:		FINDWORDPATH
* SUBJ:	Find path to given word's concept.
* CR:		05/30/00 AM.
* ARG:	User supplies buffer and buffer size.
* NOTE:	Path is a string of double-quoted concept names separated
*			by blanks.  eg "root" "gram" "Simple".
*			Unlike wordPath, does not add the word if it is not already in
*			the dictionary.  (Per Dave's request.)
* WARN:	bufsize not handled internally.
********************************************/

bool CG::findWordpath(_TCHAR *str, /*DU*/ _TCHAR *buf, int bufsize)
{
*buf = '\0';
if (!str || !*str || bufsize < PATHSIZ)
	return false;
CON *word;
if (!(word = kbm_->dict_find_word(str)))
	return false;
kbm_->acon_->con_to_path(word, /*DU*/ buf);
if (!*buf)
	return false;
buf[bufsize-1] = '\0';	// At least be sure the string terminates somewhere.
return true;
}


/********************************************
* FN:		WORDINDEX
* SUBJ:	Get index concept where word would be placed.
* CR:		05/31/00 AM.
* NOTE:	Kb internals.
********************************************/

CONCEPT *CG::wordIndex(_TCHAR *str)
{

if (!str || !*str)
	return 0;
return (CONCEPT *) kbm_->dict_find_index(str);
}


/********************************************
* FN:		ATTRWITHVAL
* SUBJ:	See if attr and value are in concept.
* CR:		05/02/00 AM.
********************************************/

bool CG::attrWithval(CONCEPT *conc, _TCHAR *attr, _TCHAR *val)
{
if (!conc || !attr || !*attr || !val || !*val)
	return false;

// Fetch attribute values.
PTR *ptr = kbm_->attr_find((CON *)conc, attr);
if (!ptr)
	return false;

// Look through them for value.
for (; ptr; ptr = ptr->next)
	{
	if (ptr->kind == pST									// FIX.					// 08/07/00 AM.
		&& unicu::strCmp(val,ptr->v.vst))
/*
#ifndef LINUX
		 && !_tcsicmp(val, ptr->v.vst))
#else
		 && !strcasecmp(val, ptr->v.vst))
#endif
*/
		return true;		// Found matching value. 
	}
return false;				// Did not find value.
}


/********************************************
* FN:		ATTREXISTS
* SUBJ:	See if attribute with value exists in given tree.
* CR:		08/06/99 AM.
* NOTE:	Undefined results if val of 0 is given.
********************************************/

bool CG::attrExists(CONCEPT *tree, _TCHAR *attr, _TCHAR *val)
{
if (!tree || !attr || !*attr || !val || !*val)
	return false;
return attrExistsrec((CON *)tree, true, attr, val);
}


bool CG::attrExistsrec(CONCEPT *tree, bool root, _TCHAR *attr, _TCHAR *val)
{
if (!tree)
	return false;

_TCHAR ptr[MAXPATH];				// Fix.	// 10/01/99 AM.
ptr[0] = '\0';						// Fix.	// 10/01/99 AM.
if (findVal(tree, attr, /*UP*/ ptr))
	{
	if (ptr && *ptr && !_tcscmp(val, ptr))			// FOUND IT!
		return true;
	}

// If first in list of concepts, manage it.
if (!root && !((CON *)tree)->prev)
	{
	CON *conc;
	for (conc = ((CON *)tree)->next; conc; conc = conc->next)
		{
		if (attrExistsrec(conc, false, attr, val))
			return true;
		}
	}

// Look down the tree.
return attrExistsrec(((CON *)tree)->dn, false, attr, val);
}


/********************************************
* FN:		NODEATTREXISTS
* SUBJ:	See if node attribute with value exists in given tree.
* CR:		04/30/00 AM.
* NOTE:	Undefined results if val of 0 is given.
********************************************/

bool CG::nodeAttrexists(CONCEPT *tree, _TCHAR *attr, _TCHAR *val)
{
if (!tree || !attr || !*attr || !val || !*val)
	return false;
return nodeAttrexistsrec((CON *)tree, true, attr, val);
}


bool CG::nodeAttrexistsrec(CONCEPT *tree, bool root, _TCHAR *attr, _TCHAR *val)
{
if (!tree)
	return false;

// Look for attribute in concept's phrase.
_TCHAR ptr[MAXPATH];
CON *node;
if ((node = kbm_->acon_->con_phrase((CON *) tree)))
	{
	for (; node; node = node->next)
		{
		ptr[0] = '\0';
		if (findVal(node, attr, /*UP*/ ptr))
			{
			if (ptr && *ptr && !_tcscmp(val, ptr))		// FOUND IT!
				return true;
			}
		}
	}

// If first in list of concepts, manage it.
if (!root && !((CON *)tree)->prev)
	{
	CON *conc;
	for (conc = ((CON *)tree)->next; conc; conc = conc->next)
		{
		if (nodeAttrexistsrec(conc, false, attr, val))
			return true;
		}
	}

// Look down the tree.
return nodeAttrexistsrec(((CON *)tree)->dn, false, attr, val);
}


/********************************************
* FN:		ATTRCHANGE
* SUBJ:	Change all values of attribute to the new value, in a subtree.
* CR:		08/06/99 AM.
********************************************/

void CG::attrChange(CONCEPT *tree, _TCHAR *attr,_TCHAR *val,_TCHAR *newval)
{
if (!tree || !attr || !*attr)
	return;

// Intern the new string value.
SYM *sym;
_TCHAR *str = 0;
bool dirt;	// 06/29/03 AM.
if (newval && *newval && (sym = kbm_->asym_->sym_get(newval,dirt)))	// FIX.		// 08/12/99 AM.
	str = sym->str;

attrChangerec(tree, true, attr, val, str);
}

// OPT:	Doing redundant work.  Should find the attribute and value only
//			once and then replace the value.
void CG::attrChangerec(
	CONCEPT *tree,
	bool root,
	_TCHAR *attr,
	_TCHAR *val,
	_TCHAR *newval
	)
{
if (!tree)
	return;

_TCHAR ptr[MAXPATH];		// FIX.	// 08/12/99 AM.
ptr[0] = '\0';				// FIX.	// 08/12/99 AM.
// Look for attribute in the current concept.
if (findVal(tree, attr, /*UP*/ ptr))
	{
	if (ptr && *ptr && !_tcscmp(val, ptr))			// FOUND IT!
		{
		// Replace the value.
		replaceVal(tree, attr, newval);
		dirty_ = true;															// 05/12/00 AM.
		}
	}

// Look for attribute in concept's phrase.	// FIX				// 08/12/99 AM.
CON *node;
if ((node = kbm_->acon_->con_phrase((CON *) tree)))
	{
	for (; node; node = node->next)
		{
		ptr[0] = '\0';
		if (findVal(node, attr, /*UP*/ ptr))
			{
			if (ptr && *ptr && !_tcscmp(val, ptr))		// FOUND IT!
				{
				// Replace the value.
				replaceVal((CONCEPT *)node, attr, newval);
				dirty_ = true;													// 05/12/00 AM.
				}
			}
		}
	}


// If first in list of concepts, manage it.
if (!root && !((CON *)tree)->prev)
	{
	CON *conc;
	for (conc = ((CON *)tree)->next; conc; conc = conc->next)
		{
		attrChangerec(conc, false, attr, val, newval);
		}
	}

// Look down the tree.
attrChangerec(((CON *)tree)->dn, false, attr, val, newval);
}


	//////////////////////
	// TRAVERSE THINGS
	//////////////////////

// Get a concept attached to the given concept.
CONCEPT *CG::Down(CONCEPT *conc)
{
return conc ? (CONCEPT *) ((CON *)conc)->dn : (CONCEPT *) 0;
}

CONCEPT *CG::Up(CONCEPT *conc)
{
// Must get to first concept of list, then its up.
// Fix. 05/13/99 AM.
return (CONCEPT *) kbm_->acon_->con_parent((CON *) conc);
}

CONCEPT *CG::Next(CONCEPT *conc)
{
return conc ? (CONCEPT *) ((CON *)conc)->next : (CONCEPT *) 0;
}

CONCEPT *CG::Prev(CONCEPT *conc)
{
return conc ? (CONCEPT *) ((CON *)conc)->prev : (CONCEPT *) 0;
}

// Traverse a list of attributes.
ATTR *CG::popAttr(ATTR* &attrs)
{
if (!attrs)
	return (ATTR *) 0;
ATTR *ptr;
ptr = attrs;
attrs = (ATTR *) ((PTR *) attrs)->next;
return ptr;
}

// Traverse a list of values.												// 03/02/00 AM.
VAL *CG::nextVal(VAL *val)													// 03/02/00 AM.
{
if (!val)
	return 0;
return (VAL *) ((PTR *)val)->next;
}

// Pop attribute value as a string.		// 07/09/99 AM.
void CG::popSval(VAL *&vals, /*UP*/ _TCHAR *buf)
{
*buf = '\0';
long num = 0;
float fnum = 0.0;																// 08/16/01 AM.
if (!vals)
	return;
PTR *ptr;
ptr = (PTR *) vals;

// Pop (even if error, to forestall infinite loops).
vals = (VAL *) ((PTR *)vals)->next;
switch (ptr->kind)
	{
	case pST:
		_tcscpy(buf, ptr->v.vst);
		return;
	case pNUM:
		num = ptr->v.vnum;
		_stprintf(buf, _T("%ld"), num);	// Convert to string.
		return;
	case pFLOAT:																// 08/16/01 AM.
		fnum = ptr->v.vfloat;												// 08/16/01 AM.
		_stprintf(buf, _T("%f"), fnum);											// 08/16/01 AM.
		return;																	// 08/16/01 AM.
	default:
		*cgerr << _T("[popSval: Can't convert to string.]") << std::endl;
		return;
	}
}

// Pop the next value from a list of attributes.
void CG::popVal(VAL* &vals, /*UP*/ _TCHAR *val)
{
*val = '\0';
if (!vals)
	return;
PTR *ptr;
ptr = (PTR *) vals;

// Pop (even if error, to forestall infinite loops).
vals = (VAL *) ((PTR *)vals)->next;

if (ptr->kind != pST)
	{
	*cgerr << _T("[popVal: Attribute not a string.]") << std::endl;
	return;
	}

_tcscpy(val, ptr->v.vst);
}


void CG::popVal(VAL* &vals, /*UP*/ long long &val)
{
val = -1;
if (!vals)
	return;
PTR *ptr;
ptr = (PTR *) vals;

vals = (VAL *) ((PTR *)vals)->next;

if (ptr->kind != pNUM)
	{
	*cgerr << _T("[popVal: Attribute not a num.]") << std::endl;
	return;
	}
val = ptr->v.vnum;
}

// VARIANT. 08/16/01 AM.
void CG::popVal(VAL* &vals, /*UP*/ float &val)
{
val = -1.0;
if (!vals)
	return;
PTR *ptr;
ptr = (PTR *) vals;

vals = (VAL *) ((PTR *)vals)->next;

if (ptr->kind != pFLOAT)
	{
	*cgerr << _T("[popVal: Attribute not a float.]") << std::endl;
	return;
	}
val = ptr->v.vfloat;
}

// VARIANT.	// 08/12/00 AM.
void CG::popVal(VAL* &vals, /*UP*/ CONCEPT* &val)
{
val = 0;
if (!vals)
	return;
PTR *ptr;
ptr = (PTR *) vals;

vals = (VAL *) ((PTR *)vals)->next;

if (ptr->kind != pCON)
	{
	*cgerr << _T("[popVal: Attribute not a concept.]") << std::endl;
	return;
	}
val = ptr->v.vcon;
}

// 6/3/02 Dd.
bool CG::isValCon(VAL* &vals)
{
if (!vals)
	return false;
PTR *ptr;
ptr = (PTR *) vals;
if (ptr->kind != pCON)
	return false;
return true;
}

bool CG::isValStr(VAL* &vals)
{
if (!vals)
	return false;
PTR *ptr;
ptr = (PTR *) vals;
if (ptr->kind != pST)
	return false;
return true;
}

bool CG::isValNum(VAL* &vals)
{
if (!vals)
	return false;
PTR *ptr;
ptr = (PTR *) vals;
if (ptr->kind != pNUM)
	return false;
return true;
}

	//////////////////////
	// MAKE THINGS
	//////////////////////

// Make named concept under given parent.
// Concept is placed at end of list of children.
CONCEPT *CG::makeConcept(CONCEPT *parent, _TCHAR *name)
{
dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) kbm_->acon_->con_add_basic(name, (CON *) parent);
}

// Make named concept at given position under parent.
// pos == 0 means at add end of list.
CONCEPT *CG::makeConcept(CONCEPT *parent, _TCHAR *name, int pos)
{
dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) kbm_->acon_->con_add_nth((CON *) parent, name, pos);
}

// Add concept to nth position in parent's children.
// pos == 0 means at end of list.
CONCEPT *CG::addConcept(CONCEPT *parent, CONCEPT *conc, int pos)
{
dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) kbm_->acon_->con_add_nth((CON *) parent, (CON *)conc, pos);
}

// Add an empty attribute. (For an editor.)	// 07/12/99 AM.
ATTR *CG::addAttr(CONCEPT *conc, _TCHAR *attr)
{
dirty_ = true;																	// 05/12/00 AM.
return kbm_->attr_get_h((CON *)conc, attr);
}


// Add value as a string type.	// 07/09/99 AM.
bool CG::addSval(CONCEPT *con, _TCHAR *attr, _TCHAR *val)
{
dirty_ = true;																	// 05/12/00 AM.
return addVal(con, attr, val);
}


bool CG::addSval(CONCEPT *con, _TCHAR *attr, long long val)
{
_TCHAR buf[256];
dirty_ = true;																	// 05/12/00 AM.
_stprintf(buf, _T("%llu"), val);
return addVal(con, attr, buf);
}


// Add string value at end of attribute's values.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, _TCHAR *val)
{
dirty_ = true;																	// 05/12/00 AM.
if (kbm_->attr_add_by_name((CON *) conc, attr, val))
	return true;
return false;
}

// Add num value at end of attribute's values.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, long long val)
{
dirty_ = true;																	// 05/12/00 AM.
if (kbm_->attr_add_by_name((CON *) conc, attr, val))
	return true;
return false;

}

// Add num value at end of attribute's values IF UNIQUE.
bool CG::addValUnique(CONCEPT *conc, _TCHAR *attr, long long val)
{
dirty_ = true;
if (!kbm_->attr_has_n_val((CON *) conc, attr, val)) {
	if (kbm_->attr_add_by_name((CON *) conc, attr, val))
		return true;
}
return false;
}

// Add float value at end of attribute's values.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, float val)				// 06/11/03 AM.
{
dirty_ = true;
if (kbm_->attr_add_by_name((CON *) conc, attr, val))
	return true;
return false;

}


// Add CONCEPT values at end of attr's values.						// 08/11/00 AM.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, CONCEPT *val)			// 08/11/00 AM.
{
dirty_ = true;
if (kbm_->attr_add_by_name((CON *)conc, attr, (CON *)val))
	return true;
return false;
}


CONCEPT *CG::addWord(_TCHAR *str)
{
if (!str || !*str)
	return 0;

#ifdef OLD_030629_
CON *word;
if (!(word = kbm_->dict_find_word(str)))
	{
	if (!(word = kbm_->dict_add_word(str)) )
		return 0;
	dirty_ = true;																// 05/12/00 AM.
	}
#endif

bool dirt;																		// 06/29/03 AM.
CON *word = kbm_->dict_get_word(str,dirt);							// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return 0;																	// 06/29/03 AM.
if (dirt)																		// 06/29/03 AM.
	dirty_ = true;																// 06/29/03 AM.

return (CONCEPT *) word;
}

	//////////////////////
	// FIND OR MAKE THINGS
	//////////////////////

// Find or make concept under given parent.
CONCEPT *CG::getConcept(CONCEPT *parent, _TCHAR *name)
{
CONCEPT *con = (CONCEPT *) kbm_->acon_->con_get_child((CON *) parent, name);
if (con)																// FIX.	// 08/07/00 AM.
	return con;														// FIX.	// 08/07/00 AM.
dirty_ = true;		// Todo: check if made concept.					// 05/12/00 AM.
return (CONCEPT *) kbm_->acon_->con_add_basic(name, (CON *) parent);			// 08/07/00 AM.
}
	//////////////////////
	// DELETE THINGS
	//////////////////////

// For compatibility with kbms based on QDBM, which needs to free some objects.
// Eg, strings need to be freed or interned, as appropriate.	// 02/11/07 AM.
bool CG::kbfree(void *obj)
{
// Our in-memory kbms already has strings interned and taken care of.
return true;
}

// Delete the given concept.
bool CG::rmConcept(CONCEPT *conc)
{
dirty_ = true;																	// 05/12/00 AM.
return kbm_->acon_->con_remove((CON *) conc);
}


// Delete the named concept from its parent.
bool CG::rmConcept(CONCEPT *parent, _TCHAR *name)
{
CONCEPT *ptr;
dirty_ = true;																	// 05/12/00 AM.
//if (ptr = getConcept(parent, name))	// CREATES!!	// FIX	// 02/06/01 AM.
if ((ptr = findConcept(parent, name)))						// FIX.	// 02/06/01 AM.
	return rmConcept(ptr);
return false;
}

// Delete nth concept from its parent.
bool CG::rmConcept(CONCEPT *parent, int pos)
{
CON *ptr;
if (!parent)
	return false;
if (!(ptr = ((CON *)parent)->dn))
	return false;
CON *pcon;
if (!(pcon = kbm_->acon_->con_nth(ptr, pos)))
	return false;
dirty_ = true;																	// 05/12/00 AM.
return rmConcept(pcon);
}

// Delete values of concept's attribute.
// 08/26/99 AM. Don't care if attribute is missing.
bool CG::rmVals(CONCEPT *conc, _TCHAR *attr)
{
if (!kbm_->attr_rm_by_name((CON *) conc, attr))						// 05/25/01 AM.
	return false;																// 05/25/01 AM.

dirty_ = true;																	// 05/12/00 AM.
return true;																	// 08/26/99 AM.
}

bool CG::rmVal(ATTR *attr, VAL *val)							// 05/11/00 AM.
{
dirty_ = true;																	// 05/12/00 AM.
return kbm_->rm_one_val((PTR *)attr, (PTR *)val);
}


// Delete an attribute.			// 07/12/99 AM.
bool CG::rmAttr(CONCEPT *con, _TCHAR *attr)
{
dirty_ = true;																	// 05/12/00 AM.
return rmVals(con, attr);
}

bool CG::rmAttr(CONCEPT *conc, ATTR *attr)
{
dirty_ = true;																	// 05/12/00 AM.
return kbm_->attr_rm((CON *)conc, (PTR *) attr);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, _TCHAR *val_s)		// 05/17/00 AM.
{
dirty_ = true;
return kbm_->attr_rm_val_by_name((CON *)con, attr_s, val_s);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, long long val)			// 05/19/00 AM.
{
dirty_ = true;
return kbm_->attr_rm_val_by_name((CON *)con, attr_s, val);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, float val)		// 06/11/03 AM.
{
dirty_ = true;
return kbm_->attr_rm_val_by_name((CON *)con, attr_s, val);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, CONCEPT *val)			// 05/19/00 AM.
{
dirty_ = true;
return kbm_->attr_rm_val_by_concept((CON *)con, attr_s, (CON *)val);
}

// REMOVE ALL "NORMAL" ATTRIBUTES OF CONCEPT.						// 10/31/00 AM.
bool CG::rmAttrs(CONCEPT *conc)
{
bool dirty = false;
bool ok = kbm_->attrs_rm((CON *)conc, /*UP*/ dirty);
if (dirty)
	dirty_ = true;
return ok;
}



// Remove all the subtree below given concept, including the
// phrase under the concept.  // 08/10/99 AM.
// If given a node (ie sample), does nothing.
bool CG::rmChildren(CONCEPT *conc)
{
if (!conc)
	return false;
if (((CON *)conc)->kind == cPROXY)
	return false;			// Do nothing if it's a node.

if (!rmCphrase(conc))	// Remove concept's phrase.
	return false;
dirty_ = true;																	// 05/12/00 AM.

// Detach concept from its children.
//return rmConcept(Down(conc));											// 11/02/00 DD.
CON *child = ((CON *)conc)->dn;											// 11/02/00 DD.
if (child)																		// 11/02/00 AM.
	child->up = CNULL;														// 11/02/00 DD.
((CON *)conc)->dn = CNULL;													// 11/02/00 DD.
return true;
}

// Remove all the phrases (ie samples) from the given subtree.
// If given a node (ie sample), does nothing.	// 08/10/99 AM.
bool CG::prunePhrases(CONCEPT *tree)
{
if (!tree)
	return false;
prunePhrasesRec((CON *)tree, true);
dirty_ = true;																	// 05/12/00 AM.
return true;
}

// Recursive prune phrases from subtree.	// 08/10/99 AM.
void CG::prunePhrasesRec(CONCEPT *conc, bool root)
{
if (!conc)
	return;

// Remove concept's phrase, if any.
rmCphrase(conc);

// If not root and start of list, manage the list.
if (!root && !((CON *)conc)->prev)
	{
	CON *tmp = NULL;
	for (tmp = ((CON *)conc)->next; tmp; tmp = tmp->next)
		prunePhrasesRec(tmp, false);
	}

// Prune phrases from concept's children.
prunePhrasesRec(((CON *)conc)->dn, false);
}



// Remove word.																// 07/17/00 AM.
bool CG::rmWord(_TCHAR *str)
{
if (!str || !*str)
	return false;

dirty_ = true;
return kbm_->dict_rm_word(str);
}


	//////////////////////
	// REPLACE THINGS
	//////////////////////

// Replace attribute value with given value.
bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, _TCHAR *val)
{
//*cgerr << "[replaceVal: attr=" << attr << ",val=" << val << "]" << std::endl;
rmVals(con, attr);															// 05/28/01 AM.
dirty_ = true;																	// 05/12/00 AM.
return addVal(con, attr, val);
}

bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, long long val)
{
rmVals(con, attr);															// 05/28/01 AM.
dirty_ = true;																	// 05/12/00 AM.
return addVal(con, attr, val);
}

bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, float val)			// 06/11/03 AM.
{
rmVals(con, attr);
dirty_ = true;
return addVal(con, attr, val);
}

bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, CONCEPT *val)		// 08/12/00 AM.
{
rmVals(con, attr);															// 05/28/01 AM.
dirty_ = true;
return addVal(con, attr, val);
}

bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, CONCEPT *oldval, CONCEPT *newVal)		// 06/04/02 Dd.
{
if (!rmAttrval(con, attr, oldval))
	return false;
return addVal(con, attr, newVal);
}

// Multi-valued attr replacement.
bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, _TCHAR *oldval, _TCHAR *newval)
{
if (!rmAttrval(con, attr, oldval))
	return false;
return addVal(con, attr, newval);
}

// Multi-valued attr replacement.
bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, long long oldval, long long newval)
{
if (!rmAttrval(con, attr, oldval))
	return false;
dirty_ = true;
return addVal(con, attr, newval);
}

// Multi-valued attr replacement.										// 06/11/03 AM.
bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, float oldval, float newval)
{
if (!rmAttrval(con, attr, oldval))
	return false;
dirty_ = true;
return addVal(con, attr, newval);
}

bool CG::renameConcept(CONCEPT *con, _TCHAR *name)
{
// Make sure it's a reasonable concept.
if (!con || ((CON *)con)->kind != cBASIC)
	return false;
SYM *sym;
bool dirt;	// 06/29/03 AM.
sym = kbm_->asym_->sym_get(name,dirt);
((CON *)con)->v.vname = sym;
dirty_ = true;																	// 05/12/00 AM.
return true;
}

bool CG::renameConcept(CONCEPT *parent, int pos, _TCHAR *name)
{
CONCEPT *conc;
if (!(conc = findConcept(parent, pos)))
	return false;
return renameConcept(conc, name);
}


// Rename an attribute. // 07/12/99 AM.
bool CG::renameAttr(CONCEPT *con, _TCHAR *attr, _TCHAR *rename)
{
ATTR *ptr;
if (!(ptr = findAttr(con, attr)))
	return false;
return renameAttr(ptr, rename);
}

bool CG::renameAttr(ATTR *attr, _TCHAR *rename)
{
dirty_ = true;																	// 05/12/00 AM.
return kbm_->attr_rename((PTR *) attr, rename);
}

	//////////////////////
	// MOVE THINGS
	//////////////////////

// "Bubble" concept left.
bool CG::moveCleft(CONCEPT *con)
{
if (!con || ((CON *) con)->kind != cBASIC)
	return false;


CON *after;				// Prior concept in list.
if (!(after = ((CON *)con)->prev))
	return false;

dirty_ = true;																	// 05/12/00 AM.
if (!rmConcept(con))
	return false;

// Reattach between the two previous siblings.
CON *before;
if (!(before = after->prev))
	{
	// Add to beginning of parent's list.
	return kbm_->acon_->con_add_first((CON *)con, after->up);
	}
else				// Insert into list of siblings.
	{
	((CON *)con)->prev = before;
	((CON *)con)->next = after;
	before->next = ((CON *)con);
	after->prev = ((CON *)con);
	return true;
	}
}


// "Bubble" concept right.
bool CG::moveCright(CONCEPT *con)
{
if (!con || ((CON *) con)->kind != cBASIC)
	return false;

CON *before;
if (!(before = ((CON *)con)->next))
	return false;

CON *parent = ((CON *)con)->up;								// FIX	// 10/31/00 DD.

dirty_ = true;																	// 05/12/00 AM.
if (!rmConcept(con))
	return false;

CON *after;
after = before->next;

// Reattach.
((CON *)con)->prev = before;
((CON *)con)->next = after;
before->next = ((CON *)con);

if (after)
	after->prev = ((CON *)con);

if (parent)															// FIX.	// 10/31/00 AM.
	{
	before->up = parent;											// FIX.	// 10/31/00 DD.
	parent->dn = before;											// FIX.	// 10/31/00 AM.
	((CON *)con)->up = CNULL;									// FIX.	// 10/31/00 AM.
	}

return true;
}

// Create new parent concept and place existing concepts underneath
CONCEPT *CG::makeParentConcept(_TCHAR *name, CONCEPT *conStart, CONCEPT *conEnd)
{
if (!conStart || !conEnd || ((CON *) conStart)->kind != cBASIC || ((CON *) conEnd)->kind != cBASIC)
	return NULL;

// GET POS OF FIRST CONCEPT
CON *sibling = ((CON *)conStart);

int posStart = 1;
while (sibling->up == NULL) {
   sibling = sibling->prev;
   posStart++;
}
CON *parent = sibling->up;

sibling = (CON *)conStart;
bool isSibling = false;
int count = 0;
while (sibling) {
	if (sibling == conEnd) {
		isSibling = true;
		break;
	}
	sibling = sibling->next;
	count++;
}

if (!isSibling)
	return NULL;

dirty_ = true;

CON *newParent = kbm_->acon_->con_add_nth((CON *) parent, name, posStart);

sibling = (CON *)conStart;
CON *next = NULL;

while (sibling) {
	next = ((CON *)sibling)->next;
	if (!rmConcept(sibling))
		return NULL;
	kbm_->acon_->con_add_nth((CON *) newParent, (CON *)sibling, 0);
	if (sibling == ((CON *)conEnd))
		break;
	sibling = next;
}

return (CONCEPT *)newParent;
}

// Adopt concepts under new concept
void CG::adoptConcepts(CONCEPT *conParent, CONCEPT *conStart, CONCEPT *conEnd)
{
if (!conParent || !conStart || !conEnd ||
	((CON *) conParent)->kind != cBASIC || ((CON *) conStart)->kind != cBASIC || ((CON *) conEnd)->kind != cBASIC)
	return;

dirty_ = true;

CON *sibling = (CON *)conStart;
CON *next = NULL;

while (sibling) {
	next = ((CON *)sibling)->next;
	if (!rmConcept(sibling))
		return;
	kbm_->acon_->con_add_nth((CON *) conParent, (CON *)sibling, 0);
	if (sibling == ((CON *)conEnd))
		break;
	sibling = next;
}
}

// Sort concept's children.	[NEW!]
bool CG::sortChilds(CONCEPT *conc)	// 02/20/09 AM.
{
if (!conc)
	return false;
dirty_ = true;
return sort_childs((CON *) conc,this);
}

	//////////////////////
	// PHRASE MGMT
	//////////////////////

// Find concept's phrase.
PHRASE *CG::findPhrase(CONCEPT *conc)
{
return (CONCEPT *) kbm_->acon_->con_phrase((CON *) conc);
}

// Sort concept's phrase.	[NEW!]
bool CG::sortPhrase(CONCEPT *conc)
{
if (!conc)
	return false;
dirty_ = true;																	// 05/12/00 AM.
return sort_phrase((CON *) conc,this);
}

long CG::phraseLength(CONCEPT *conc)
{
return kbm_->acon_->phrase_len((CON *)findPhrase(conc));
}

// FIND CONCEPT THAT THIS NODE IS A PROXY FOR.						// 12/03/02 AM.
// Changed functionality.													// 12/03/02 AM.
CONCEPT *CG::nodeConcept(CONCEPT *node)
{
if (node)
	return kbm_->acon_->nc((struct con *)node);
return 0;
}

// Find node's owning concept.			// 08/27/99 AM.
// RENAMED THIS FUNCTIONALITY FROM nodeconcept.						// 12/03/02 AM.
CONCEPT *CG::nodeOwner(CONCEPT *node)
{
CONCEPT *fst;

if (!(fst = listNode(node)))
	return 0;
return kbm_->attr_get_c((CON *)fst, kbm_->acon_->c_cg_ATOM);
}

// Find nth phrase node.  pos = 0 means find last node.
CONCEPT *CG::findNode(PHRASE  *phr,  int pos)
{
return (CONCEPT *) kbm_->acon_->con_nth((CON *) phr, pos);
}


// Find first node that matches.
CONCEPT *CG::findNode(PHRASE *phr, _TCHAR    *name)
{
CON *nodes;

if (!(nodes = (CON *) phr))
	return 0;
SYM *sym;
bool dirt;	// 06/29/03 AM.
if (!(sym = kbm_->asym_->sym_get(name,dirt)))
	return 0;
CON *con;
for (; nodes; nodes = nodes->next)
	{
	con = kbm_->acon_->nc(nodes);
	if (sym == con->v.vname)
		return (CONCEPT *) nodes;
	}
return 0;
}

// Given a node, find first node in list.	// 08/31/99 AM.
CONCEPT *CG::listNode(CONCEPT *node)
{
CON *nn;
nn = (CON *) node;
if (!nn || nn->kind != cPROXY)
	{
	*cgerr << _T("[listNode: Given bad node.]") << std::endl;
	return 0;
	}
CON *tmp = NULL;
while ((tmp = nn->prev))
	nn = tmp;
return (CONCEPT *)nn;
}

// Conveniences.
CONCEPT *CG::firstNode(PHRASE  *phr)
{
if (!phr || ((CON *)phr)->prev)
	{
	*cgerr << _T("[firstNode: Must be called with a phrase.]") << std::endl;
	return 0;
	}
return (CONCEPT *) phr;
}

CONCEPT *CG::lastNode( PHRASE  *phr)
{
if (!phr || ((CON *)phr)->prev)
	{
	*cgerr << _T("[lastNode: Must be called with a phrase.]") << std::endl;
	return 0;
	}
return (CONCEPT *) kbm_->acon_->con_nth((CON *) phr, 0);
}

// Create a phrase under concept by adding first node.
PHRASE *CG::makePhrase(CONCEPT *con, _TCHAR *name)
{
if (!con || !name || !*name)
	return 0;

#ifdef OLD_030629_
CON *word;
if (!(word = kbm_->dict_find_word(name))
	 && !(word = kbm_->dict_add_word(name)) )
	return 0;
#endif

bool dirt;																		// 06/29/03 AM.
CON *word = kbm_->dict_get_word(name,dirt);							// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return NULL;																// 06/29/03 AM.

CON *proxy;
proxy = kbm_->acon_->con_add_proxy(word);

/* Bind concept to first element of phrase. */
PVAL pv;
pv.vcon = proxy;
kbm_->attr_add_val((CON *)con, kbm_->acon_->c_cg_PHRASE, pv, pCON);
pv.vcon = (CON *)con;
kbm_->attr_add_val(proxy, kbm_->acon_->c_cg_ATOM, pv, pCON);
dirty_ = true;																	// 05/12/00 AM.
return (PHRASE *) proxy;
}

// NEW!  Sorry, has to be named differently (everything is really void).
CONCEPT *CG::addCnode(CONCEPT *con, _TCHAR *name)
{
PHRASE *phr;
phr = 0;
return addNode(con, name, /*DU*/ phr);
}


CONCEPT *CG::addCnode(CONCEPT *con, CONCEPT *ncon)	// 11/27/02 AM.
{
if (!con || !ncon)
	return 0;

dirty_ = true;
CON *proxy = kbm_->acon_->con_add_proxy((CON *)ncon);
return (CONCEPT *) phr_add_nth((CON *)con, proxy, 0, this);
}

// Add a node to a concept's phrase. Create phrase as needed.
// Phrase can be passed down, or 0 if empty or not known.
// Phrase will be passed back up by the function.
// New node is returned by the function.
CONCEPT *CG::addNode(CONCEPT *con, _TCHAR *name, /*DU*/ PHRASE* &phr)
{
if (!con || !name || !*name)
	return 0;

if (phr)									// Use the given phrase.
	return addNode(phr, name);

dirty_ = true;																	// 05/12/00 AM.
if (!(phr = findPhrase(con)))		// Empty phrase.
	{
	phr = makePhrase(con, name);
	return firstNode(phr);
	}
else
	return addNode(phr, name);
}


// Add node to phrase. pos == 0, the default, means add to end
// of phrase.
CONCEPT *CG::addNode(PHRASE *phr, _TCHAR    *name, int pos)
{
CON *owner;
if (!(owner = kbm_->attr_get_c((CON *)phr, kbm_->acon_->c_cg_ATOM)))
	return 0;

dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) phr_add_nth(owner, name, pos, this);
}


// Add detached node at given position.
CONCEPT *CG::addNode(PHRASE *phr, CONCEPT *node,  int pos)
{
CON *owner;
if (!(owner = kbm_->attr_get_c((CON *)phr, kbm_->acon_->c_cg_ATOM)))
	return 0;

dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) phr_add_nth(owner, (CON *)node, pos, this);
}


// Add node before n_pos.  If n_pos == 0, add at end of phrase.
// RENAMED.	// 02/13/07 AM.
CONCEPT *CG::addNode_c(PHRASE *phr, CONCEPT *node, CONCEPT *n_pos)
{
if (!phr || !node)
	return 0;

CON *owner;
if (!(owner = kbm_->attr_get_c((CON *)phr, kbm_->acon_->c_cg_ATOM)))
	return 0;

dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) insert_node(owner, (CON *) node, (CON *) n_pos, this);
}

// Move node from one position to another. from,to=0 end position.
// Note: Whether data structures are switched or their content is
// replaced is undefined.  Don't rely on one or the other.
CONCEPT *CG::moveNode(PHRASE *phr, int        from, int        to)
{
CONCEPT *n_from, *n_to;
n_from = n_to = 0;

// Find both nodes.
if (!(n_from = findNode(phr, from)))
	return 0;

if (to == 0)					// Moving to last position.
	n_to = 0;
else if (!(n_to = findNode(phr, to)))
	return 0;

if (n_from == n_to)			// No op.
	return n_from;

dirty_ = true;																	// 05/12/00 AM.
if (!rmNode(phr, n_from)) // Remove the from node.
	return 0;

// Add node in proper position.
return addNode_c(phr, n_from, n_to);
}

// If s_to == 0, move to end.
CONCEPT *CG::moveNode(PHRASE *phr, _TCHAR    *s_from, _TCHAR    *s_to)
{
CONCEPT *n_from, *n_to;
n_from = n_to = 0;

// Find both nodes.
if (!(n_from = findNode(phr, s_from)))
	return 0;

if (s_to == 0)					// Moving to last position.
	n_to = 0;
else if (!(n_to = findNode(phr, s_to)))
	return 0;

dirty_ = true;																	// 05/12/00 AM.
return moveNode(phr, n_from, n_to);
}

CONCEPT *CG::moveNode(PHRASE *phr, CONCEPT *n_from, CONCEPT *n_to)
{
if (!phr)
	return 0;

if (n_from == n_to)			// No op.
	return n_from;

if (!rmNode(phr, n_from)) // Remove the from node.
	return 0;

// Add node in proper position.
dirty_ = true;																	// 05/12/00 AM.
return addNode_c(phr, n_from, n_to);

}


// "Bubble" node left.
bool CG::moveNleft(CONCEPT *con)
{
if (!con || ((CON *) con)->kind != cPROXY)
	return false;

CON *after;				// Prior concept in list.
if (!(after = ((CON *)con)->prev))
	return false;

dirty_ = true;																	// 05/12/00 AM.
if (!rmConcept(con))
	return false;

// Reattach between the two previous siblings.
CON *before;
if (!(before = after->prev))
	{
	// Add as first phrase node.
	// after == phrase and first phrase node.
	if (addNode_c(after, con, after))
		return true;
	return false;
	}
else				// Insert into list of siblings.
	{
	((CON *)con)->prev = before;
	((CON *)con)->next = after;
	before->next = ((CON *)con);
	after->prev = ((CON *)con);
	return true;
	}
}


// "Bubble" node right.
bool CG::moveNright(CONCEPT *con)
{
if (!con || ((CON *) con)->kind != cPROXY)
	return false;

CON *before;
if (!(before = ((CON *)con)->next))
	return false;

dirty_ = true;																	// 05/12/00 AM.
if (!rmNode(con))
	return false;

CON *after;
after = before->next;

// Reattach.
((CON *)con)->prev = before;
((CON *)con)->next = after;
before->next = ((CON *)con);
if (after)
	after->prev = ((CON *)con);
return true;
}


// Delete a node.
bool CG::rmNode(PHRASE* &phr,  _TCHAR *name)
{
CON *con;
if (!(con = (CON *) findNode(phr, name)))
	return false;

// If removing first node in phrase, set caller's phrase properly.
PHRASE *tmp = NULL;
if (phr == (PHRASE *)con)
	tmp = (PHRASE *) con->next;

dirty_ = true;																	// 05/12/00 AM.
if (!rm_proxy(con,this))			// Remove phrase node.
	return false;

phr = tmp;
return true;
}

bool CG::rmNode(PHRASE* &phr,  int    pos)
{
CON *con;
if (!(con = (CON *) findNode(phr, pos)))
	return false;

// If removing first node in phrase, set caller's phrase properly.
PHRASE *tmp = NULL;
if (phr == (PHRASE *)con)
	tmp = (PHRASE *) con->next;

dirty_ = true;																	// 05/12/00 AM.
if (!rm_proxy(con,this))			// Remove phrase node.
	return false;

phr = tmp;
return true;
}

// Remove node from its phrase.
bool CG::rmNode(PHRASE* &phr, CONCEPT *node)
{
if (!node)
	return false;
if (((CON *)node)->kind != cPROXY)
	return false;

// If removing first node in phrase, set caller's phrase properly.
PHRASE *tmp = NULL;
if (phr == (PHRASE *)node)
	tmp = (PHRASE *) ((CON *)node)->next;

dirty_ = true;																	// 05/12/00 AM.
if (!rm_proxy((CON *) node,this))
	return false;

phr = tmp;
return true;
}

// Remove node from its phrase.
bool CG::rmNode(CONCEPT *node)
{
if (!node)
	return false;
if (((CON *)node)->kind != cPROXY)
	return false;
dirty_ = true;																	// 05/12/00 AM.
if (!rm_proxy((CON *) node,this))
	return false;
return true;
}


// Delete phrase.
bool CG::rmPhrase(PHRASE* &phr)
{
CON *owner;
if (!(owner = kbm_->attr_get_c((CON *)phr, kbm_->acon_->c_cg_ATOM)))
	return false;

// Unbind phrase.
dirty_ = true;																	// 05/12/00 AM.
kbm_->attr_rm_by_name((CON *)phr, _T("atom"));
kbm_->attr_rm_by_name(owner, _T("phrase"));
phr = 0;				// Zero out caller's phrase pointer.
return true;
}

bool CG::rmCphrase(CONCEPT *owner)
{
CON *phr;
if (!(phr = kbm_->attr_get_c((CON *)owner, kbm_->acon_->c_cg_PHRASE)))
	return true;			// NO PHRASE IS OK.							// 11/02/00 AM.

// Unbind phrase.
dirty_ = true;																	// 05/12/00 AM.
kbm_->attr_rm_by_name(phr, _T("atom"));
kbm_->attr_rm_by_name((CON *)owner, _T("phrase"));
return true;
}

// Rename node.
bool CG::renameNode(PHRASE  *phr, _TCHAR *name, _TCHAR *rename)
{
CON *node;
if (!(node = (CON *) findNode(phr, name)))
	return false;

// Make sure it's a reasonable concept.
if (((CON *)node)->kind != cPROXY)
	return false;

#ifdef OLD_030629_
CON *word;
if (!(word = kbm_->dict_find_word(rename))
	 && !(word = kbm_->dict_add_word(rename)))
	return false;
#endif

bool dirt;																		// 06/29/03 AM.
CON *word = kbm_->dict_get_word(rename,dirt);						// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return false;																// 06/29/03 AM.

((CON *)node)->v.vcon = word;
dirty_ = true;																	// 05/12/00 AM.
return true;
}

bool CG::renameNode(PHRASE  *phr, int    pos, _TCHAR *rename)
{
CON *node;
if (!(node = (CON *) findNode(phr, pos)))
	return false;

// Make sure it's a reasonable concept.
if (((CON *)node)->kind != cPROXY)
	return false;

#ifdef OLD_030629_
CON *word;
if (!(word = kbm_->dict_find_word(rename))
	 && !(word = kbm_->dict_add_word(rename)))
	return false;
#endif

bool dirt;																		// 06/29/03 AM.
CON *word = kbm_->dict_get_word(rename,dirt);						// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return false;																// 06/29/03 AM.

((CON *)node)->v.vcon = word;
dirty_ = true;																	// 05/12/00 AM.
return true;
}


/********************************************
* FN:		DUMPTREE
* SUBJ:	Modular dump of KB subtree to a file.
* CR:		08/06/01 AM.
* NOTE:	Patterned after writeKB.
* ASS:	Assumes we're not dumping in the system hierarchy.
* METH:	Traverse the concept subtree.  Write out one file for:
*			hier, attr, phrase.  For each concept and node name, write
*			an "add word" command to word file.  Then catenate the 4
*			files to given file path.
********************************************/

// Modular dump of a kb subtree.											// 08/06/01 AM.
bool CG::dumpTree(															// 08/06/01 AM.
	CONCEPT *root,		// Root of subtree.
	_TCHAR *file			// Dump file (should end in .kb).
	)
{
*cgerr << _T("[Starting modular kb dump.]") << std::endl;

if (!root)					// Check for good concept.
	return false;

if (!goodKbfile(file))	// Check for good kb filename.
	return false;

// Todo: Check to make sure we're not in a bad hierarchy.
// (ie, one that has dictionary in it....) Won't hurt too badly,
// but may get lots of "add word" redundancy if so.

// Make a tmp directory if absent.
_TCHAR *dir = _T("tmp");

// For example, path= C:\apps\Resume\tmp.
_TCHAR path[MAXPATH];
_stprintf(path, _T("%s%c%s"), getAppdir(), DIR_CH, dir);

// If tmp directory absent, create it.
make_dir(path);

// Prep and open up the output files.
_TCHAR o_hier[MAXPATH*2];
_TCHAR o_word[MAXPATH*2];
_TCHAR o_attr[MAXPATH*2];
_TCHAR o_phr[MAXPATH*2];

_TCHAR *suff;
suff = _T("kb");		// Kb file suffix.

_stprintf(o_hier, _T("%s%chier.%s"), path, DIR_CH, suff);
_stprintf(o_word, _T("%s%cword.%s"), path, DIR_CH, suff);
_stprintf(o_attr, _T("%s%cattr.%s"), path, DIR_CH, suff);
_stprintf(o_phr,  _T("%s%cphr.%s"),  path, DIR_CH, suff);

// Open the files for output.
std::_t_ofstream f_hier(TCHAR2A(o_hier));
std::_t_ofstream f_word(TCHAR2A(o_word));
std::_t_ofstream f_phr(TCHAR2A(o_phr));

std::_t_ofstream *f_attr = new std::_t_ofstream(TCHAR2A(o_attr));	// 07/01/03 AM.

// GET PATH TO "root" CONCEPT.
CONCEPT *parent = Up(root);
_TCHAR spath[MAXPATH];
if (!parent)			// Root of hierarchy
	{
	spath[0] ='\0';
	f_hier << _T("add empty") << std::endl;
	f_hier << _T("add root") << std::endl;
	}
else
	kbm_->acon_->con_to_path((struct con *)parent, /*UP*/ spath);

// Traverse the subtree.  Write out the goodies.
long n_attr=0;												// 07/01/03 AM.
long c_attr=0;												// 07/01/03 AM.
if (!writeTree(root, true, f_hier,f_word,f_phr, spath,
			f_attr,n_attr,c_attr,0,0))					// 07/01/03 AM.
	{
	delete f_attr;											// 08/07/03 AM.
	return false;
	}
delete f_attr;												// 08/07/03 AM.

// Terminate the last command file.
f_phr  << std::endl << _T("quit") << std::endl << std::endl;

// TODO: CATENATE THE COMMAND FILES TO THE OUTPUT FILE.
#ifndef LINUX
// Do the catenate in C++.
_TCHAR buf[MAXMSG];
std::_t_ofstream *f_dump = new std::_t_ofstream(TCHAR2A(file));	// Final output file.
std::_t_ifstream *f_in = new std::_t_ifstream(TCHAR2A(o_hier));
while (f_in->getline(buf, MAXMSG))		// Copy first file.
	*f_dump << buf << std::endl;
delete f_in;

*f_dump << std::endl;								// Separate commands.
f_in = new std::_t_ifstream(TCHAR2A(o_word));
while (f_in->getline(buf, MAXMSG))		// Copy first file.
	*f_dump << buf << std::endl;
delete f_in;

*f_dump << std::endl;								// Separate commands.
f_in = new std::_t_ifstream(TCHAR2A(o_attr));
while (f_in->getline(buf, MAXMSG))		// Copy first file.
	*f_dump << buf << std::endl;
delete f_in;

*f_dump << std::endl;								// Separate commands.
f_in = new std::_t_ifstream(TCHAR2A(o_phr));
while (f_in->getline(buf, MAXMSG))		// Copy first file.
	*f_dump << buf << std::endl;
delete f_in;

delete f_dump;

#else
_TCHAR cmd[MAXPATH*9];
_TCHAR *cp = _T("cat");
_stprintf(cmd, _T("%s %s %s %s %s > %s"),
	cp, o_hier, o_word, o_attr, o_phr, file);
int systemRet = _tsystem(cmd);
#endif

*cgerr << _T("[Done with modular kb dump.]") << std::endl;
return true;
}


///////////////////////////////////////////////////////////////////////////
/////////                      UTILITIES                  /////////////////
///////////////////////////////////////////////////////////////////////////

/********************************************
* FN:		WRITETREE
* SUBJ:	Traverse and write the knowledge base in memory to files.
* CR:		04/28/99 AM.
* METH:	Traverse the concept hierarchy.  Write out one file for:
*			hier, word, attr, phrase.
********************************************/

bool CG::writeTree(
	CONCEPT *root,
	bool flag,				// true if root of subtree.
	std::_t_ofstream &hier,
	std::_t_ofstream &word,
	std::_t_ofstream &phr,
	_TCHAR *path,				// Hierarchy path down to parent.
	std::_t_ofstream *&attr,		// Current attr output stream.			// 07/01/03 AM.
	long &n_attr,			// # of attr files.						// 07/01/03 AM.
	long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
	_TCHAR *o_attr,			// Buff for building attr filenames.	// 07/01/03 AM.
	_TCHAR *fpath,			// Path for attr file.						// 07/01/03 AM.
	_TCHAR *suff				// Filename suffix.							// 07/01/03 AM.
	)
{
if (!root)
	return true;

//char name[128];
_TCHAR name[MAXSTR];	// 12/17/03 AM.
if (!conceptName(root, name))
	{
	*cgerr << _T("[writeTree: No concept name.]") << std::endl;
	return false;
	}

_TCHAR buf[MAXMSG];
prep_str(buf, name);

// Print information for the current concept.
CON *con;
con = (CON *)root;
switch (con->kind)
	{
	case cBASIC:
		if (path && *path)	// So KB ROOT won't be redundant.	// 02/20/02 AM.
			{
			// 01/13/06 AM. TESTING UNICODE.
			#ifdef UNICODE
			char *lpstr8;
			#endif

			hier << _T("add hier ");

			#ifdef UNICODE
			u_to_mbcs((LPCWSTR)path, CP_UTF8, (LPCTSTR*&)lpstr8);
			hier << lpstr8;
			u_delete((LPCTSTR*&)lpstr8);
			#else
			hier << path;
			#endif

			hier << _T(" \"");

			#ifdef UNICODE
			u_to_mbcs((LPCWSTR)buf, CP_UTF8, (LPCTSTR*&)lpstr8);
			hier << lpstr8;
			u_delete((LPCTSTR*&)lpstr8);
			#else
			hier << buf;
			#endif

			hier << _T("\"");
			hier << std::endl;
			}
//		else
//			{
			// Axiomatics.
//			hier << "add empty" << std::endl;
//			hier << "add root" << std::endl;
//			}
		break;
	case cWORD:
		// Need to handle some chars specially.
		{
#ifndef UNICODE
		word << _T("add word \"") << buf << _T("\"") << std::endl;
#else
		char *lpstr8;
		word << _T("add word \"");
		u_to_mbcs((LPCWSTR)buf, CP_UTF8, (LPCTSTR*&)lpstr8);
		word << lpstr8;
		u_delete((LPCTSTR*&)lpstr8);
		word << _T("\"");
		word << std::endl;
#endif
		}
		break;
	default:
		*cgerr << _T("[writeTree: Not word or basic concept.]") << std::endl;
		return false;
	}

// Make path to concept itself.
_TCHAR conpath[MAXPATH*2];
if (path && *path)
	_stprintf(conpath, _T("%s \"%s\""), path, buf);
else if (*buf && !_tcscmp(buf,_T("concept")))				// CHECK.	// 12/28/01 AM.
	_stprintf(conpath, _T("\"%s\""), buf);
else																				// 12/28/01 AM.
	{
	*cgerr << _T("[writeTree:  Bad path.]") << std::endl;						// 12/28/01 AM.
	return false;																// 12/28/01 AM.
	}
	//strcpy(conpath, name);

// Print attributes for current concept.
// Print phrase for current concept.
// For each attr, traverse all its values.
if (!writeAttrs(con, 0, attr, conpath,
			n_attr,c_attr,o_attr,fpath,suff))							// 07/01/03 AM.
	{
	*cgerr << _T("[writeTree: Couldn't write attrs for con=") << name << _T("]")
		  << std::endl;
	return false;
	}

// TODO: If concept has a phrase, traverse that separately...
if (!writePhrase(con, hier,word,attr,phr,conpath,
						n_attr,c_attr,o_attr,fpath,suff))				// 07/01/03 AM.
	{
	*cgerr << _T("[Error calling writePhrase]") << std::endl;
	return false;
	}

// Traverse children of concept.
if (!writeTree(con->dn, false, hier,word,phr,conpath,
			attr,n_attr,c_attr,o_attr,fpath,suff))						// 07/01/03 AM.
	{
	*cgerr << _T("[Error calling writeTree]") << std::endl;
	return false;
	}

// If concept is first in a list, traverse its siblings.
if (!flag && !(con->prev))
	{
	CON *sib;
	bool ok;
	for (sib = con->next; sib; sib = sib->next)
		{
		// Handle sibling as root of a subtree.
		ok = writeTree(sib, true, hier,word,phr,path,
			attr,n_attr,c_attr,o_attr,fpath,suff);						// 07/01/03 AM.
		if (!ok)
			{
			*cgerr << _T("[Error in sibling call of writeTree]") << std::endl;
			return false;
			}
		}
	}

return true;
}


/********************************************
* FN:		WRITEATTRS
* SUBJ:	Traverse and write out attributes for given concept.
* CR:		04/29/99 AM.
********************************************/

bool CG::writeAttrs(
	CONCEPT *conc,			// Concept (or node).
	int pos,					// 0 for concept, positive int for each
								// node of concept's phrase.
	std::_t_ofstream *&o_attr,		// File to write attributes to.	// 07/01/03 AM.
	_TCHAR *conpath,			// Hierarchy path down to (node's) concept.
	long &n_attr,			// # of attr files.						// 07/01/03 AM.
	long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
	_TCHAR *b_attr,															// 07/01/03 AM.
	_TCHAR *fpath,															// 07/01/03 AM.
	_TCHAR *suff																// 07/01/03 AM.
	)
{
CON *con;

if (!(con = (CON *) conc))
	{
	*cgerr << _T("[writeAttrs: Given null concept.]") << std::endl;
	return false;
	}

PTR *attrs = 0;
if (!(attrs = con->attrs))
	return true;				// No attrs. That's OK.

PTR *p_slot, *vals;
CON *slot;
//PVAL pv;
_TCHAR *str, *s_slot;
_TCHAR *s_kind;
_TCHAR buf[MAXPATH];					// 04/29/99 AM.
_TCHAR sbuf[MAXPATH];					// 04/29/99 AM.
_TCHAR strbuf[MAXPATH*2];				// 04/30/99 AM.
int val_pos = 0;	// If concept value is a node.	// 10/09/05 AM.

while (attrs)
   {
   p_slot = attrs->v.vptr;
   slot   = p_slot->v.vcon;

	if (slot->id == cg_PHRASE
		 || slot->id == cg_ATOM		// Fix. 05/03/99 AM.
		 )
		{
		// Phrases are special. Handle elsewhere.
		attrs = attrs->next;
		continue;
		}

   vals   = p_slot->next;
	kbm_->acon_->con_to_path(slot, /*UP*/ sbuf);		// 04/29/99 AM.
   s_slot = sbuf;								// 04/29/99 AM.
   while (vals)
      {
		/*
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		;; lastblank rule has trig_flag == true
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		ind attr
		concept app resume rules zone_lines lastblank
		0						; concept itself.
		concept sys attrs proxy trig_flag
		pcon
		concept sys vals boolean true
		end ind
		*/
		// New command for kb node as value.	// 10/09/05 AM.
		/*
		ind nattr
		concept app resume rules zone_lines lastblank
		0						; concept itself.
		concept sys attrs proxy trig_flag
		pcon
		concept sys vals boolean true
		1						; node number
		end ind
		*/

		val_pos = 0;	// Reset node indicator.							// 10/09/05 AM.
		buf[0] = '\0';

      switch (vals->kind)
         {
         case pCON:
            {
				// Handle case where value is a KB NODE.					// 10/09/05 AM.
				// val_pos = node number...									// 10/09/05 AM.
				struct con *qcon = vals->v.vcon;								// 10/09/05 AM.
				struct con *hcon = kbm_->acon_->node_owner(qcon);		// 10/09/05 AM.
				if (hcon)		// Original concept is a node.			// 10/09/05 AM.
					{
					// Get position of the node.								// 10/09/05 AM.
					val_pos = kbm_->acon_->con_ord(qcon);					// 10/09/05 AM.
					kbm_->acon_->con_to_path(hcon, /*UP*/ buf);			// 10/09/05 AM.
					}
				else																	// 10/09/05 AM.
					kbm_->acon_->con_to_path(vals->v.vcon, /*UP*/ buf);// 04/29/99 AM.
				//str = kbm_->acon_->con_str(vals->v.vcon);				// 04/29/99 AM.
				str = buf;															// 04/29/99 AM.
				s_kind = _T("pcon");
				}
				break;
         case pSYM:
				str = vals->v.vsym->str;
				prep_str(buf, str);											// 04/30/99 AM.
				_stprintf(strbuf, _T("\"%s\""), buf);							// 04/30/99 AM.
				str = strbuf;													// 04/30/99 AM.
				s_kind = _T("psym");
				break;
         case pST:
				str = vals->v.vst;
				prep_str(buf, str);											// 04/30/99 AM.
				_stprintf(strbuf, _T("\"%s\""), buf);							// 04/30/99 AM.
				str = strbuf;													// 04/30/99 AM.
				s_kind = _T("pst");
				break;
         case pNUM:
				_stprintf(buf, _T("%llu"), vals->v.vnum);
				str = buf;
				s_kind = _T("pnum");
				break;
         case pFLOAT:														// 08/16/01 AM.
				_stprintf(buf, _T("%f"), vals->v.vfloat);						// 08/16/01 AM.
				str = buf;														// 08/16/01 AM.
				s_kind = _T("pfloat");											// 08/16/01 AM.
				break;															// 08/16/01 AM.
         case pCHAR:
				_stprintf(buf, _T("%c"), vals->v.vchar);
				str = buf;
				s_kind = _T("pchar");
				break;
         case pPTR:
         default:
			// 04/20/99 AM.
        //fprintf(stderr, "[attrs_pret: Bad value kind=%d]\n", vals->kind);
		  *cgerr << _T("[attrs_pret: Bad value kind=") << vals->kind << std::endl;
            exit(1);
         }

		// For backward compatibility, avoid ind nattr.				// 10/09/05 AM.
		if (!val_pos)															// 10/09/05 AM.
			*o_attr << _T("ind attr") << std::endl;
		else																		// 10/09/05 AM.
			*o_attr << _T("ind nattr") << std::endl;							// 10/09/05 AM.
		*o_attr << conpath << std::endl;
		*o_attr << pos << std::endl;
		*o_attr << s_slot << std::endl;
		*o_attr << s_kind << std::endl;
		*o_attr << str << std::endl;												// 04/29/99 AM.
		if (val_pos)															// 10/09/05 AM.
			*o_attr << val_pos << std::endl;									// 10/09/05 AM.
		*o_attr << _T("end ind") << std::endl << std::endl;

		// Manage current attr kb file.									// 07/01/03 AM.
		if (n_attr	// If zero, keep the old way.						// 07/01/03 AM.
		 && ++c_attr >= ATTRS_PER_FILE )									// 07/01/03 AM.
			{
			// Close up the attr file and open a new one.			// 07/01/03 AM.
			++n_attr;	// Increment file count.						// 07/01/03 AM.
			c_attr = 0;	// Reset count of attributes.					// 07/01/03 AM.
			*o_attr << std::endl << _T("quit") << std::endl << std::endl;				// 07/01/03 AM.
			delete o_attr;	// Maxed out in the old file.				// 07/01/03 AM.
			_stprintf(b_attr, _T("%s%cattr%ld.%s"),fpath,DIR_CH,n_attr,suff);
			o_attr = new std::_t_ofstream(TCHAR2A(b_attr));								// 07/01/03 AM.
			}

      vals = vals->next;
      }
   //fprintf(out, "\n");													// 04/20/99 AM.
	*o_attr << std::endl;															// 04/20/99 AM.
   attrs = attrs->next;
   }

return true;
}


/********************************************
* FN:		WRITEPHRASE
* SUBJ:	Traverse and write out concept's phrase.
* CR:		04/30/99 AM.
********************************************/

bool CG::writePhrase(			// 04/30/99 AM.
	CONCEPT *conc,
	std::_t_ofstream &o_hier,
	std::_t_ofstream &o_word,
	std::_t_ofstream *&o_attr,
	std::_t_ofstream &o_phr,
	_TCHAR *path,				// Hierarchy path down to parent.
	long &n_attr,			// # of attr files.						// 07/01/03 AM.
	long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
	_TCHAR *b_attr,															// 07/01/03 AM.
	_TCHAR *fpath,															// 07/01/03 AM.
	_TCHAR *suff																// 07/01/03 AM.
	)
{
CON *con;
if (!(con = (CON *) conc))
	{
	*cgerr << _T("[writePhrase: no concept]") << std::endl;
	return false;
	}
CON *phr;
if (!(phr = kbm_->acon_->con_phrase(con)))
	return true;			// No phrase.  That's OK.

CON *ptr;				// For traversing.
CON *bas;				// Node's basic concept.
_TCHAR buf[MAXPATH];

// Traverse phrase and write it out.
o_phr << _T("ind phrase") << std::endl;
o_phr << path << std::endl;
for (ptr = phr; ptr; ptr = ptr->next)
	{
	bas = kbm_->acon_->nc(ptr);
	kbm_->acon_->con_to_path(bas, /*UP*/ buf);
	o_phr << buf << std::endl;
	}
o_phr << _T("end ind") << std::endl << std::endl;

// Retraverse phrase to write out attributes.
// (Opt: not optimal, but simple and modular.)
int pos = 0;
for (ptr = phr; ptr; ptr = ptr->next)
	{
	if (!writeAttrs(ptr, ++pos, o_attr, path,
							n_attr,c_attr,b_attr,fpath,suff))		// 07/01/03 AM.
		{
		*cgerr << _T("[writeTree: Couldn't write attrs for con=") << path
			  << _T(", node=") << pos
			  << _T("]") << std::endl;
		return false;
		}
	}

return true;
}

/********************************************
* FN:		READFILE
* SUBJ:	Read a kb file into memory.
* CR:		05/02/99 AM.
********************************************/

bool CG::readFile(
	_TCHAR *infile
	)
{
//cout << "infile=" << infile << std::endl;
//ifstream fin(infile, std::ios::in | std::ios::nocreate);	// Upgrade. // 01/24/01 AM.
std::_t_ifstream fin(infile, std::ios::in);							// Upgrade.	// 01/24/01 AM.
#ifdef UNICODE
if (!u_readbom(&fin))
	{
	*cgerr << _T("[readFile: Unhandled Unicode format=") << infile << _T("]") << std::endl;
	return false;
	}
#endif

if (!fin)
	{
	*cgerr << _T("[readFile: File not found=") << infile << _T("]") << std::endl;
	return false;
	}

// "Take" the input command file.
//if (!cmd_top(&fin, &cout, "> ", false))
//if (!cmd_top(&fin, &cout, "> ", false,true))						// 03/27/00 AM.
if (!cmd_top(&fin, cgerr, _T("> "), false,this,true))					// 02/19/02 AM.
	{
	*cgerr << _T("[readFile: Error in reading file=") << infile << _T("]") << std::endl;
	return false;
	}
return true;
}


/********************************************
* FN:           FINDWORDCONCEPT
* SUBJ:	Find the concept for a word
* CR:		04/18/00 Dd.
* ARG:  User supplies the word string
* NOTE:	06/29/03 AM. Renamed this from getwordconcept.
********************************************/

CONCEPT *CG::findWordConcept(_TCHAR *str)
{
CONCEPT *word;
if (!(word = kbm_->dict_find_word(str)) )
        return 0;
return word;
}

CONCEPT *CG::matchDictKB(std::string dictFilename, std::vector<std::filesystem::path> kbfiles) {
	if (kbfiles.size() == 0) return NULL;
	std::vector<std::filesystem::path>::iterator ptr;
	
	CONCEPT *con, *dictcon;
	std::filesystem::path filePath(dictFilename);
	std::string head = removeExtension(filePath.filename().string());

    for (ptr = kbfiles.begin(); ptr < kbfiles.end(); ptr++) {
		std::filesystem::path kbpath(ptr->string());
		std::string headkb = removeExtension(kbpath.filename().string());

        if (strcmp(head.c_str(), headkb.c_str()) == 0) {
			con = findRoot();
			dictcon = findConcept(con,"dictionary");
			if (dictcon) {
				return dictcon;
			}
		}
	}
	return NULL;
}

std::string CG::removeExtension(const std::string& filename) {
    size_t lastDot = filename.find_last_of(".");
    if (lastDot == std::string::npos)
        return filename;
    return filename.substr(0, lastDot);
}

bool CG::openDict(std::vector<std::filesystem::path>& files) {
	bool found = false;
	files.clear();

	std::filesystem::path p(kbdir_);
	p /= _T("all.dict");
	allDictStream_.open(p.string(), std::ios::in);

	if (allDictStream_) {
		files.push_back(p);
		allDictStream_.close();
		found = true;
	} else {
		read_files(kbdir_,_T("(.*\\.dict)$"),files);
		found = files.size() ? true : false;
	}

	return found;
}

bool CG::readDicts(std::vector<std::filesystem::path> files, std::vector<std::filesystem::path> kbfiles) {
	std::vector<std::filesystem::path>::iterator ptr;
	if (files.size() == 0) return false;
    for (ptr = files.begin(); ptr < files.end(); ptr++) {
        readDict(ptr->string(), kbfiles);
	}
	return true;
}

bool CG::readDict(std::string file, std::vector<std::filesystem::path> kbfiles) {
	bool dirty;
	CONCEPT *wordCon, *parentCon;
	_TCHAR buf[MAXMSG];
	_TCHAR token[MAXSTR];
	_TCHAR attr[MAXSTR];
	_TCHAR val[MAXSTR];
	int lineCount = 0;
	DICT_CALL caller;

	CONCEPT *ambigKB = matchDictKB(file, kbfiles);

	// For error printouts
	std::size_t botDirPos = file.find_last_of(DIR_CH);
	std::string filename = file.substr(botDirPos+1, file.length()-2);

	allDictStream_.open(file, std::ios::in);
		
	while (allDictStream_.getline(buf, MAXMSG)) {
		lineCount++;
		if (unicu::isStrWhiteSpace(buf))
			continue;

		caller.line = lineCount;
		icu::StringPiece sp(buf);
		const char *line = sp.data();
		int32_t length = sp.length();

		UChar32 c = 1;
		int32_t e = 0;
		int32_t eLast = 0;
		int32_t ulen = 0;
		int start = e;
		int wordCount = 0;
		int eqSign = -1;
		bool suggestedAttr = false;
		bool suggestedLit = false;
		bool comment = false;
		int begins[30];
		int lens[30];
		bool lastWhite = false;
		bool backSlash = false;
		bool inWord = false;
		bool inString = false;
		bool firstTime = true;
		bool doNext = true;
		int tokint = 0;
		int startQuote = 0;

		for (int i = 0; i<30; i++) {
			begins[i] = -1;
			lens[i] = -1;
		}

		U8_NEXT(line, e, length, c);

		// get tokens
		while (c) {
			doNext =  true;
			if (c == '"') {
				if (backSlash && !inString) {
					begins[tokint] = e - 2;
					lens[tokint++] = 2;
					inWord = false;
					lastWhite = false;
					backSlash = false;
				} else if (!backSlash) {
					if (inString) {
						lens[tokint++] = e - begins[tokint];
						inString = false;
					} else {
						begins[tokint] = startQuote = e - 1;
						inString = true;
					}					
				}
				backSlash = false;
			}
			else if (!inString) {
				if (c == '#') {
					comment = true;
					break;
				}
				else if (c == '\\') {
					backSlash = true;
				}
				else if (unicu::isWhiteSpace(c)) {
					if (inWord) {
						lens[tokint] = e - begins[tokint] - 1;
						tokint++;
						inWord = false;
					}
					start = e - 1;
					lastWhite = true;
				}
				else if (unicu::isSingle(c)) {
					if (unicu::isEmoji(c)) {
						int32_t eSave = e;
						U8_NEXT(line, e, length, c);
						if (unicu::isEmojiVariation(c) || unicu::isEmojiJoiner(c)) {
							bool joining = false;
							while (c && (unicu::isEmojiVariation(c) || unicu::isEmojiJoiner(c) || joining)) {
								joining = unicu::isEmojiJoiner(c);
								eSave = e;
								U8_NEXT(line, e, length, c);
							}
						}
						e = eSave;
						doNext = false;
					}
					begins[tokint] = firstTime ? 0 : eLast;
					lens[tokint] = e - eLast;
					tokint++;
				}
				else if (c != '_' && (unicu::isPunct(c) || c == '=')) {
					if (inWord) {
						lens[tokint] = e - begins[tokint] - 1;
						tokint++;
					}
					if (c == '=' && eqSign == -1 && !backSlash) {
						eqSign = tokint;
					}
					begins[tokint] = backSlash ? e - 2 : e - 1;
					lens[tokint++] = backSlash ? 2 : 1;
					inWord = false;
					lastWhite = false;
					backSlash = false;
				}
				else {
					if (!inWord) {
						inWord = true;
						begins[tokint] = firstTime ? 0 : e - 1;
					}
					lastWhite = false;
					backSlash = false;
				}
			}
			eLast = e;
			if (doNext)
				U8_NEXT(line, e, length, c);
			firstTime = false;
		}
		if (inWord) {
			lens[tokint] = e - begins[tokint] - 1;
			tokint++;
		}
		
		// If comment, skip line
		if (comment)
			continue;
			
		if (inString) {
			sprintf(errout_, "%d %d [missing ending double quote - %s]", lineCount, startQuote, filename.c_str());
			*cgerr << errout_ << std::endl;
			return false;
		}

		if (eqSign == -1) {
			sprintf(errout_, "%d 1 [missing equal sign - %s]", lineCount, filename.c_str());
			*cgerr << errout_ << std::endl;
			return false;
		}

		// Loop through tokens

		bool isPhrase = false;
		parentCon = NULL;
		bool attrFlag = false;
		_TCHAR cc = 0;
		bool addValue = false;

		caller.file = file;
		caller.type = DICT_CALL_FILE;

		for (int i=0; i<tokint; i++) {
			addValue = false;
			int len = lens[i];
			int beg = begins[i];
			_tcsnccpy(token, &line[beg],len);
			token[len] = '\0';
			cc = token[0];
			if (cc == '\\') {
				beg++;
				len--;
				_tcsnccpy(token, &line[beg],len);
				token[len] = '\0';
				cc = token[0];
			}

			// Tokens before the first token before the equals sign
			if (i < eqSign - 1) {
				wordCon = kbm_->dict_get_word(token,dirty,caller);
				if (parentCon) {
					parentCon = getConcept(parentCon,token);
					isPhrase = true;
				} else {
					parentCon = wordCon;
				}
				// add ambigous concept to word concept
				if (ambigKB) {
					CONCEPT *kbcon = findConcept(ambigKB,token);
					if (kbcon) {
						CONCEPT *ambigCon = NULL;
						if (!findVal(wordCon,"meaning",ambigCon)) {
							addVal(wordCon,_T("meaning"),kbcon);
						}
					}
				}

			} else if (cc == '=') {
				int donothing = 1;

			} else if (cc == '"') {
				int len = lens[i] - 2;
				_tcsnccpy(val, &token[1], len);
				val[len] = '\0';
				addValue = true;

			} else if (attrFlag) {
				_tcsnccpy(val, token, lens[i]);
				val[lens[i]] = '\0';
				addValue = true;
				attrFlag = false;

			} else {
				_tcsnccpy(attr, token, lens[i]);
				attr[lens[i]] = '\0';
				attrFlag = true;
				if (!_tcsicmp(attr, _T("l"))) {
					suggestedLit = true;
				}
				else if (!_tcsicmp(attr, _T("s"))) {
					suggestedAttr = true;
				}
			}

			if (addValue) {
				if (unicu::isNumeric(val)) {
					long long vnum = 0;
					unicu::strToLong(val,vnum);
					addVal(parentCon,attr,vnum);
				} else
					addSval(parentCon,attr,val);
				attrFlag = false;
				addValue = false;
			}
		}

		if (attrFlag) {
			sprintf(errout_, "%d %d [missing attribute value - %s]", lineCount, begins[tokint-1]+1, filename.c_str());
			*cgerr << errout_ << std::endl;
			return false;
		}

		if (isPhrase && suggestedLit) {
			addSval(parentCon,_T("l"),val);
		} else if (isPhrase && !suggestedAttr) {
			addSval(parentCon,_T("s"),_T("phrase"));
		}
	}

	allDictStream_.close();

	return true;
}

bool CG::openKBB(std::vector<std::filesystem::path>& files) {
	bool found = false;
	files.clear();

	std::filesystem::path p(kbdir_);
	p /= _T("all.kbb");
	allDictStream_.open(p.string(), std::ios::in);

	if (allDictStream_) {
		files.push_back(p);
		allDictStream_.close();
		found = true;
	} else {
		read_files(kbdir_,_T("(.*\\.kbb)$"),files);
		found = files.size() ? true : false;
	}

	return found;
}

bool CG::readKBBs(std::vector<std::filesystem::path> files) {
	std::vector<std::filesystem::path>::iterator ptr;
	if (files.size() == 0) return false;
    for (ptr = files.begin(); ptr < files.end(); ptr++) {
        readKBB(ptr->string());
	}
	return true;
}

bool CG::readKBB(std::string file) {
	bool found;
	CONCEPT *con, *parent;
	std::vector<CONCEPT *> cons;
	std::vector<std::pair<int,int>> conIndices;
	_TCHAR buf[MAXMSG];
	_TCHAR word[MAXSTR];
	_TCHAR attr[MAXSTR];
	_TCHAR val[MAXSTR];

	std::ifstream streamer;
	streamer.open(file, std::ios::in);
	int32_t indent = 0;
	int32_t lastIndent = 0;
	int32_t index = 0;
	bool starting = true;
	bool attrFlag = false;
	bool openSquare = false;
	bool openDouble = false;
	bool collectingConcept = false;
		
	while (streamer.peek() != EOF) {
		streamer.getline(buf,MAXMSG);
		icu::StringPiece sp(buf);
		const char *line = sp.data();
		int32_t length = sp.length();
		if (!length)
			continue;

		UChar32 c = 1;
		int32_t end = 0;
		int32_t start = 0;
		int32_t ulen = 0;
		found = false;
		U8_NEXT(line, end, length, c);

		// Get indent
		int spaces = 0;
		while (c) {
			if (unicu::isWhiteSpace(c)) {
				spaces++;
				U8_NEXT(line, end, length, c);
			} else {
				break;
			}
		}
		if (c == '#')
			continue;
					
		lastIndent = index;
		indent = spaces;

		attrFlag = false;
		openSquare = false;
		openDouble = false;
		collectingConcept = false;

		index = indent/2;
		start = indent;
		if (indent == 0) {
			cons.clear();
			lastIndent = 0;
		}

		bool conceptDone = false;

		// Get the rest
		while (c) {
			if (c == '#') {
				break;
			}
			else if (conceptDone && !openDouble && unicu::isWhiteSpace(c)) {
				int doNothing = 1;
				start = end;
			}
			else if (conceptDone && !openDouble && c == '[') {
				openSquare = true;
				start = end;
				attrFlag = true;			
			}
			else if (conceptDone && openSquare && collectingConcept && (c == ']' || c ==',')) {
				CONCEPT *conPath = addConceptByPath(line,conIndices);
				addVal(con,attr,conPath);
				start = end;
				if (c != ',') {
					openDouble = false;
					openSquare = false;
					collectingConcept = false;
					attrFlag = false;
				}
				conIndices.clear();		
			}
			else if (conceptDone && openSquare && openDouble && c == '"') {
				_tcsnccpy(val, &line[start],end-start-1);
				val[end-start-1] = '\0';
				if (!_tcscmp(val, _T("concept")) || collectingConcept) {
					collectingConcept = true;
					conIndices.push_back(std::make_pair(start,end-1));
				} else {
					start = end;
					addSval(con,attr,val);
				}
				start = end;
				// Add to concept path
				openDouble = false;
			}
			else if (conceptDone && openSquare && c == '"') {
				openDouble = true;
				start = end;
				attrFlag = true;			
			}
			// If attribute starts with a double quote
			else if (conceptDone && !attrFlag && !openDouble && c == '"') {
				openDouble = true;
				start = end;
			}
			else if (conceptDone && attrFlag && !openDouble && c == '"') {
				openDouble = true;
				start = end;
			}
			else if (conceptDone && !attrFlag && c == ':') {
				start = end;
			}
			else if (conceptDone && attrFlag &&
						((openSquare && !openDouble && (c == ',' || c == ']')) ||
						 (!openSquare && !openDouble && c == ',') ||
						 (openDouble && c == '"') ||
						  end == length)) {
				_tcsnccpy(val, &line[start],end-start);
				int adjust = length && c != ']' && c != '"' && c != ',' ? 0 : 1;
				val[end-start-adjust] = '\0';
				addSval(con,attr,val);
				start = end;
				if (end >= length) {
					conceptDone = false;
					attrFlag = false;
					openSquare = false;
				}
				else if (c == ']') {
					openSquare = false;
					attrFlag = false;
				// Comma separating attributes
				} else if (c == ',' && attrFlag && !openDouble && !openSquare) {
					attrFlag = false;
				}
				openDouble = false;			
			}
			else if (conceptDone && c == '=') {
				_tcsnccpy(attr, &line[start],end-start-1);
				int adjust = openDouble ? 2 : 1;  // This is in case the attribute was double quoted
				attr[end-start-adjust] = '\0';
				openDouble = false;
				start = end;
				attrFlag = true;			
			}
			else if (!conceptDone && (c == ':' || end == length || (openDouble && !attrFlag && c == '"'))) {
				_tcsnccpy(word, &line[start],end-start);
				int adjust = (c == ':' || c == '"') ? 1 : 0;
				openDouble = false;
				word[end-start-adjust] = '\0';
				if (index == 0)
					parent = findRoot();
				else
					parent = cons[index-1];
				con = makeConcept(parent,word);
				// Backup if indent less than last
				for (int j=lastIndent; j && j>=index; j--)
					cons.pop_back();
				cons.push_back(con);
				conceptDone = true;
				start = end;
			}
			else if (!conceptDone && c == '"') {
				openDouble = true;
				start = end;		
			}
			U8_NEXT(line, end, length, c);
		}
	}

	allDictStream_.close();

	return true;
}

CONCEPT *CG::addConceptByPath(const char *line, std::vector<std::pair<int,int>> conIndices) {
	CONCEPT *con = findRoot();
	int i = 0;
	_TCHAR word[MAXMSG];
    for (auto indice: conIndices) {
		_tcsnccpy(word, &line[indice.first],indice.second);
		word[indice.second-indice.first] = '\0';
		if (i++)
			con = makeConcept(con,word);
    }
	return con;
}

/********************************************
* FN:           FINDDICTCONCEPT
* SUBJ:	Find the concept for a word from a *.dict file
* ARG:  User supplies the word string
********************************************/

CONCEPT *CG::findDictConcept(_TCHAR *str)
{
bool dirty = false;
CONCEPT *word = kbm_->dict_get_word(str,dirty);

allDictStream_.clear();
allDictStream_.seekg(0);

_TCHAR buf[MAXMSG];
_TCHAR attr[MAXSTR];
_TCHAR val[MAXSTR];
bool found = false;

int32_t c2 = 1;
int32_t e2 = 0;
icu::StringPiece strp(str);
const char *strd = strp.data();
int32_t length2 = strp.length();
bool done = false;

while (allDictStream_.getline(buf, MAXMSG) && !done) {
	icu::StringPiece sp(buf);
	const char *line = sp.data();
	int32_t length = sp.length();

	UChar32 c = 1;
	int32_t e = 0;
	int32_t ulen = 0;
	c2 = 1;
	e2 = 0;
	U8_NEXT(strd, e2, length2, c2);

	while (c && !done) {
		U8_NEXT(line, e, length, c);
		if (unicu::isWhiteSpace(c)) {
			if (c2 == 0) {
				found = true;
			}
			break;
		} else if (c > c2) {
			done = true;
			break;
		}
		else if (c != c2) {
			break;
		}
		U8_NEXT(strd, e2, length2, c2);
	}

	// If found, parse the attributes and add
	if (found) {
		int start = e;
		bool attrFlag = false;
		bool doubleQuote = false;
		bool backslash = false;

		while (c) {
			U8_NEXT(line, e, length, c);
			if (c == '\\') {
				backslash = true;
			}
			else if (!doubleQuote && attrFlag && !backslash && c == '"') {
				doubleQuote = true;
				start++;
			}
			else if (doubleQuote && !backslash && c == '"') {
				bool nothing = true;
			}
			else if (attrFlag && ((!doubleQuote && unicu::isWhiteSpace(c)) || !c)) {
				_tcsnccpy(val, &line[start],e-start-1);
				e -= doubleQuote ? 1 : 0;
				val[e-start-1] = '\0';
				addSval(word,attr,val);
				attrFlag = false;
				start = e;
			} else if (c == '=') {
				_tcsnccpy(attr, &line[start],e-start-1);
				attr[e-start-1] = '\0';
				start = e;
				attrFlag = true;
			} else if (backslash) {
				backslash = false;
			}
		}

		found = false;
	}
}

return word;
}


/********************************************
* FN:				GETWORDCONCEPT
* SUBJ:	Gets the concept for a word
* CR:		06/29/03 AM.
* NOTE:	Now ADD word if couldn't FIND it.
********************************************/

CONCEPT *CG::getWordConcept(_TCHAR *str)
{
CONCEPT *word;
bool dirt;	// 06/29/03 AM.
if (!(word = kbm_->dict_get_word(str,dirt)) )
        return 0;
return word;
}

/********************************************
* FN:           GOODKBFILE
* SUBJ: See if path is a good kb filename.
* CR:   08/06/01 AM.
********************************************/

bool CG::goodKbfile(_TCHAR *file)
{
if (!file || !*file)
   return false;

_TCHAR *end = file-1;				// Set up for traversing string.
long count = 0;
while (*++end)
	++count;
if (count < 4)
	return false;

// Now end is at string terminator.
// Check for ".kb".
if (*--end != 'b')
	return false;
if (*--end != 'k')
	return false;
if (*--end != '.')
	return false;
return true;
}


/********************************************
* FN:		DICTFIRST
* SUBJ:	Get first alphabetic word-concept in dictionary.
* CR:		03/21/03 AM..
* NOTE:	To support dict traversal.
********************************************/

CONCEPT *CG::dictFirst()
{
return (CONCEPT *) kbm_->dict_first();
}


/********************************************
* FN:		DICTNEXT
* SUBJ:	Get next alphabetic word-concept in dictionary.
* CR:		03/21/03 AM..
* NOTE:	To support dict traversal.
********************************************/

CONCEPT *CG::dictNext(CONCEPT *con)
{
if (!con)
	return 0;
return (CONCEPT *) kbm_->dict_next((CON*)con);
}

/**********************     END OF FILE         ***************************/