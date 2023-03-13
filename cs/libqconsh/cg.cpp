/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AQCONSH.CPP
* FILE:	cs/libqconsh/cg.cpp
* CR:		02/08/07 AM.
* SUBJ:	API class for the Conceptual Grammar KBMS based on QDBM.
* NOTE:	There is a test_libqconsh program in the dev\projects area.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include <direct.h>							// 03/05/07 AM.
#include "machine-min.h"					// 03/08/00 AM.
#include "prim/libprim.h"
#include "lite/global.h"
#include "cg_global.h"				// 09/16/99 AM.

#include "prim/unicode.h"					// 01/13/06 AM.
#include "prim/prim.h"
#include "prim/io.h"					// 03/16/07 AM.
#include "prim/str.h"				// 04/30/99 AM.
#include "prim/list_s.h"
#include "prim/list.h"
#include "prim/dir.h"				// 05/06/99 AM.
#include "prim/dyn.h"				// 06/29/00 AM.

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xcon_s.h"
#include "qkbm/xptr_s.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "attr.h"						// 02/16/07 AM.
#include "cgcon.h"					// 02/19/07 AM.
#include "cgdict.h"					// 02/18/07 AM.
#include "qconsh/cg.h"
#include "qconsh/bind.h"			// 05/02/99 AM.
//#include "qconsh/consh_kb.h"		// 05/04/99 AM.	// OBSOLETE.	02/19/07 AM.

#include "io.h"
#include "cmd.h"

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
con_SYS =
sys_DICT =
dict_ALPHA =
nlp_PUNCT =
nlp_WHT =
0;

dirty_ = false;																// 05/12/00 AM.
#ifndef LINUX
hkbdll_ = 0;																	// 06/29/00 AM.
#endif
appdir_[0] = '\0';
if (appdir && *appdir)
	_tcscpy(appdir_, appdir);
else
	{
	_t_cerr << _T("[CG: No app dir given.]") << endl;
	return;
	}
if (!appdir_ || !*appdir_)			// CONFIRM.							// 05/07/01 AM.
	{
	_t_cerr << _T("[CG: BAD APPDIR]") << endl;									// 05/07/01 AM.
	return;																		// 05/07/01 AM.
	}

if (!alist)																		// 08/14/02 AM.
	{
	_t_cerr << _T("[List manager needed for CG object.]") << endl;		// 08/14/02 AM.
	return;																		// 08/14/02 AM.
	}
alist_ = alist;																// 08/14/02 AM.

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
		<< _T("]") << endl;														// 08/22/02 AM.
	}



// Create a concept-function manager.	// 02/19/07 AM.
cgcon_ = new CGCON(this);					// 02/19/07 AM.

// Create an attribute manager object.	// 02/16/07 AM.
cgattr_ = new CGATTR(this);				// 02/16/07 AM.

// Create a dictionary manager object.	// 02/18/07 AM.
cgdict_ = new CGDICT(this);				// 02/18/07 AM.


// Create a KB Manager object.
qkbm_ = new AQKBM();


// As before, interp can mean rebuild kb from command files.
// Compiled can mean reuse the qdbm database already in place.

_TCHAR cmd[4096];																// 03/04/07 AM.
_TCHAR kbdir[2048];															// 02/26/07 AM.
_stprintf(kbdir, _T("%s%ckb%cqkbm"),appdir,DIR_CH,DIR_CH);		// 02/26/07 AM.
_TCHAR tmpdir[2048];															// 03/04/07 AM.
_stprintf(tmpdir, _T("%s%ckb%ctmp"),appdir,DIR_CH,DIR_CH);		// 03/04/07 AM.


loaded_ = false;																// 03/04/07 AM.

// 03/08/07 AM. NOT USING KB IN TMP. OFTEN IS UNCLOSED, CORRUPT.  Trash it.
// Todo: check that tmpdir exists.
#ifndef _WINDOWS
_stprintf(cmd, "rmdir /s /q %s", tmpdir);	// 03/08/07 AM.
_tsystem(cmd);					// 03/08/07 AM.
#else
_stprintf(cmd, "/s /q %s", tmpdir);	// 03/16/07 AM.
run_silent(_T("rmdir"), cmd);			// 03/16/07 AM.
#endif

// Make sure these directories exist. Create if absent.			// 03/05/07 AM.
_tmkdir(kbdir);																// 03/05/07 AM.
_tmkdir(tmpdir);																// 03/05/07 AM.

if (compiled)
	{
	*cgerr << _T("[Loading compiled kb library]") << endl;

	// Got to bind kb to code.	// 03/04/07 AM.
	cg_CONCEPT = 1;		// BIND ROOT OF KB.	// 06/29/00 AM.	// 03/04/07 AM.


	if (qkbm_->iskbms(kbdir))	// 03/04/07 AM.
		{
		// In permanent area; copy and use.
#ifndef _WINDOWS
		_stprintf(cmd, "xcopy /e /r /q %s %s", kbdir, tmpdir);
		_tsystem(cmd);					// 03/04/07 AM.
#else
		_stprintf(cmd, "/e /r /q %s %s", kbdir, tmpdir);
		run_silent(_T("xcopy"),cmd);	// 03/16/07 AM.
#endif
		qkbm_->openkbms(tmpdir);		// 03/04/07 AM.
		qkbm_->initkbms();				// 03/04/07 AM.
		if (!bind_all(this))
			*cgerr << _T("[Couldn't bind vars to compiled kb.]") << endl;
		else
			loaded_ = true;				// 03/04/07 AM.
		}
	else
		{
		*cgerr << _T("[Couldn't load compiled kb library]") << endl;
		}
	}

if (!loaded_) // Load interpreted.		// 03/04/07 AM.
	{
	qkbm_->openkbms(tmpdir);														// 02/26/07 AM.
	qkbm_->resetkbms();		// EMPTY IT OUT.								// 02/26/07 AM.


	// Load from command files.
	*cgerr << _T("[Loading interpreted kb.]") << endl;
	if (!consh_ini(this))
		*cgerr << _T("[ERROR INITIALIZING KB.]") << endl;
	}

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

CG::~CG()
{
#ifdef DEBUGGING_
qkbm_->sym_pp(cgerr);														// 06/29/03 AM.
#endif

consh_clean(this);

if (cgattr_)
	{
	delete cgattr_;															// 02/19/07 AM.
	cgattr_ = 0;																// 03/13/07 AM.
	}
if (cgdict_)
	{
	delete cgdict_;
	cgdict_ = 0;																// 03/13/07 AM.
	}
if (cgcon_)
	{
	delete cgcon_;
	cgcon_ = 0;																	// 03/13/07 AM.
	}

// CLOSE THIS LAST.	// FIX.	// 03/13/07 AM.
if (qkbm_)																		// 02/18/07 AM.
	{
	// REDUNDANT.	CLOSING QKBM_ CLOSES THE DB.	// FIX.	// 03/13/07 AM.
//	qkbm_->closekbms();														// 02/26/07 AM.
	delete qkbm_;																// 02/18/07 AM.
	qkbm_ = 0;																	// 03/13/07 AM.
	}

// alist_ is just a reference.  DO NOT FREE IT.						// 08/14/02 AM.

// RESET ERROR STREAM.														// 09/16/99 AM.

if (count_ == 1)																// 08/22/02 AM.
//	cgresetErr(ferr_, serr_);												// 09/16/99 AM.
	cgresetErr();																// 07/18/03 AM.
else
	*cgerr << _T("[Closing KB number ") << count_ << _T("]") << endl;	// 08/22/02 AM.

#ifndef LINUX
// FREE UP COMPILED KB, IF ANY.											// 06/29/00 AM.
if (hkbdll_)																	// 06/29/00 AM.
	{
	unload_dll(hkbdll_);														// 06/29/00 AM.
	hkbdll_ = 0;																// 06/29/00 AM.
	}
#endif

// Remove the tmp area (QDBM).											// 03/04/07 AM.
_TCHAR tmpdir[2048];															// 03/04/07 AM.
_stprintf(tmpdir, _T("%s%ckb%ctmp"),appdir_,DIR_CH,DIR_CH);		// 03/04/07 AM.
_TCHAR cmd[4096];																// 03/04/07 AM.
#ifndef _WINDOWS
_stprintf(cmd, "rmdir /s /q %s", tmpdir);								// 03/04/07 AM.
_tsystem(cmd);																	// 03/04/07 AM.
// Rebuild the empty folder.
_stprintf(cmd, "mkdir %s", tmpdir);										// 03/04/07 AM.
_tsystem(cmd);																	// 03/04/07 AM.
#else
_stprintf(cmd, "/s /q %s", tmpdir);										// 03/16/07 AM.
run_silent(_T("rmdir"),cmd);												// 03/16/07 AM.
// Rebuild the empty folder.
_stprintf(cmd, "%s", tmpdir);												// 03/16/07 AM.
run_silent(_T("mkdir"),cmd);												// 03/16/07 AM.
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
bool	 CG::getLoaded()	{return loaded_;}								// 03/04/07 AM.
AQKBM	*CG::getKBM()		{return qkbm_;}									// 06/11/02 AM.
ALIST *CG::getALIST()	{return alist_;}								// 08/14/02 AM.
#ifndef LINUX
HINSTANCE CG::getHkbdll()	{return hkbdll_;}
#endif


/********************************************
* FN:		Class-wide Functions
********************************************/
void	CG::setAppdir(_TCHAR *x)	{_tcscpy(appdir_, x);	}
void	CG::setDirty(bool x)		{dirty_ = x;			}				// 05/12/00 AM.
void	CG::setLoaded(bool x)	{loaded_ = x;}							// 03/04/07 AM.
void	CG::setKBM(AQKBM *x)		{qkbm_	= x;				}				// 06/11/02 AM.
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
	_t_cout << _T("Active CG count=") << count_ << endl;
}
#endif


	/////////////////////
	// OVERALL KB FUNS
	/////////////////////

/********************************************
* FN:		SAVEKBMS
* SUBJ:	Save kbms based on qdbm.
* CR:		03/04/07 AM.
* NOTE:	Save kbms built in latest VisualText session for current analyzer.
********************************************/

bool CG::saveKBMS()
{
// CLOSE THE DB BEFORE COPYING FILES.
if (qkbm_)
	qkbm_->closekbms();

_TCHAR cmd[4096];
_TCHAR kbdir[2048];
_stprintf(kbdir, _T("%s%ckb%cqkbm"),appdir_,DIR_CH,DIR_CH);
_TCHAR tmpdir[2048];
_stprintf(tmpdir, _T("%s%ckb%ctmp"),appdir_,DIR_CH,DIR_CH);
bool loaded = false;

// Copy from tmp to permanent area.
#ifndef _WINDOWS
_stprintf(cmd, "xcopy /e /r /q %s %s", tmpdir, kbdir);
_tsystem(cmd);					// 03/04/07 AM.
#else
_stprintf(cmd, "/e /r /q %s %s", tmpdir, kbdir);
run_silent(_T("xcopy"),cmd);		// 03/16/07 AM.
#endif

// REOPEN THE DB, IN CASE USER IS CONTINUING TO WORK WITH IT.
if (qkbm_)
	qkbm_->openkbms(tmpdir);

return true;
}


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
*cgerr << _T("[In writeKB]") << endl;
clock_t s_time, e_time;										// 10/20/99 AM.
s_time = clock();												// 10/20/99 AM.

if (!dir || !*dir)
	{
	*cgerr << _T("[writeKB: Given no output directory.]") << endl;
	return false;
	}

// 10/20/99 AM. Moved this up so kb files won't get wiped out if there's
// a problem.
// Grab hold of the root of the hierarchy.
CON_ID root_id;	// 03/08/07 AM.
if (!(root_id = (CON_ID) findRoot()))
	{
	*cgerr << _T("[writeKB: No root of concept hierarchy.]") << endl;
	return false;
	}

// TRAP A BAD KB WRITE!!!								// 10/20/99 AM.
if (!findConcept((CONCEPT*)root_id, _T("gram")))						// 10/20/99 AM.
	{
	*cgerr << _T("[writeKB: No GRAM concept in hierarchy.]") << endl;
	return false;
	}

// For example, path= C:\apps\Resume\kb\user.
_TCHAR *kbdir = _T("kb");
_TCHAR path[FNAMESIZ];
_stprintf(path, _T("%s%c%s%c%s"), getAppdir(), DIR_CH, kbdir, DIR_CH, dir);
*cgerr << _T("[writeKB: path=") << path << _T("]") << endl;				// 02/19/02 AM.

// If directory doesn't exist, create it.
make_dir(path);	// 05/06/99 AM.

// Prep and open up the output files.
_TCHAR o_hier[FNAMESIZ];
_TCHAR o_word[FNAMESIZ];
_TCHAR o_attr[FNAMESIZ];
_TCHAR o_phr[FNAMESIZ];

_TCHAR *suff;
suff = _T("kb");		// Kb file suffix.

_stprintf(o_hier, _T("%s%chier.%s"), path, DIR_CH, suff);
_stprintf(o_word, _T("%s%cword.%s"), path, DIR_CH, suff);
_stprintf(o_phr,  _T("%s%cphr.%s"),  path, DIR_CH, suff);

// Open the files for output.
_t_ofstream f_hier(TCHAR2A(o_hier));
_t_ofstream f_word(TCHAR2A(o_word));
_t_ofstream f_phr(TCHAR2A(o_phr));

// May open multiples of these as needed.								// 07/01/03 AM.
long n_attr = 1;	// Count attr#.kb files.							// 07/01/03 AM.
long c_attr = 0;	// Count of attributes.								// 07/01/03 AM.
_stprintf(o_attr, _T("%s%cattr%ld.%s"),path,DIR_CH,n_attr,suff);		// 07/01/03 AM.
_t_ofstream *f_attr = new _t_ofstream(TCHAR2A(o_attr));								// 07/01/03 AM.

// KB BOOTSTRAP COMMANDS. (Axiomatics)									// 08/06/01 AM.
// (Moved out of writeTree).												// 08/06/01 AM.
f_hier << _T("add empty") << endl;											// 08/06/01 AM.
f_hier << _T("add root") << endl;												// 08/06/01 AM.

// Traverse the concept hierarchy.  Write out the goodies.
XCON_S *root = qkbm_->Con(root_id);	// 03/08/07 AM.
bool ok = writeTree(root, true, f_hier,f_word,f_phr, _T(""),
			f_attr,n_attr,c_attr,o_attr,path,suff);					// 07/01/03 AM.
kbfree(root);	// 03/08/07 AM.
if (!ok)			// 03/08/07 AM.
	{
	delete f_attr;																// 08/07/03 AM.
	return false;
	}

// Terminate the command files.			// 05/02/99 AM.
f_hier << endl << _T("quit") << endl << endl;
f_word << endl << _T("quit") << endl << endl;
f_phr  << endl << _T("quit") << endl << endl;

// Shut down the last attr kb file.										// 07/01/03 AM.
*f_attr << endl << _T("quit") << endl << endl;							// 07/01/03 AM.
delete f_attr;																	// 07/01/03 AM.
f_attr = 0;																		// 07/01/03 AM.

// Write a top-level command file						// 07/01/03 AM.
writeKBmain(n_attr,path,suff);							// 07/01/03 AM.

e_time = clock();												// 10/20/99 AM.
*cgerr << _T("[WRITE KB time=")								// 10/20/99 AM.
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC	// 10/20/99 AM.
	  << _T(" sec]") << endl;									// 10/20/99 AM.

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
_TCHAR o_main[FNAMESIZ];
_stprintf(o_main, _T("%s%cmain.%s"), path, DIR_CH, suff);
_t_ofstream f_main(TCHAR2A(o_main));

f_main << _T("take \"kb/user/hier.kb\"") << endl;
f_main << _T("bind sys") << endl;
f_main << _T("take \"kb/user/word.kb\"") << endl;
f_main << _T("take \"kb/user/phr.kb\"") << endl;

long ii;
for (ii=1; ii <= n_attr; ++ii)
	f_main
		<< _T("take \"kb/user/attr")
		<< ii
		<< _T(".kb\"")
		<< endl;
f_main << _T("quit") << endl;
return true;
}


/********************************************
* FN:		READKB
* SUBJ:	Read the knowledge base from files to memory.
* CR:		05/02/99 AM.
********************************************/

bool CG::readKB(_TCHAR *dir)
{
*cgerr << _T("[readKB:]") << endl;											// 02/19/02 AM.
clock_t s_time, e_time;										// 10/20/99 AM.
s_time = clock();												// 10/20/99 AM.

if (!dir || !*dir)
	{
	*cgerr << _T("[readKB: Given no input directory.]") << endl;
	return false;
	}

if (!qkbm_)
	{
	*cgerr << _T("[readKB: Internal error. KB tables not initialized.]") << endl;
	return false;
	}

#ifndef LINUX
if (hkbdll_)
	{
	*cgerr << _T("[Using compiled kb. readKB ignored.]") << endl;	// 06/29/00 AM.
	return true;		// This is just info, ok.						// 06/29/00 AM.
	}
#endif

// For example, path= C:\apps\Resume\kb\user.
_TCHAR *kbdir = _T("kb");
_TCHAR path[FNAMESIZ];
_stprintf(path, _T("%s%c%s%c%s"), getAppdir(),DIR_CH, kbdir,DIR_CH, dir);
// 08/11/99 AM. Checking out Purify warnings.
*cgerr << _T("[readKB: path=")													// 02/19/02 AM.
		 << path
		 << _T("]")
		 << endl;

// Prep and open up the input file.
// hier, word, phr, attr.
_TCHAR infile[FNAMESIZ];

_TCHAR *suff;
suff = _T("kb");		// Kb file suffix.


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
	}

#ifdef QDBM_
cgdict_->dict_load_fin();
#endif

e_time = clock();												// 10/20/99 AM.
*cgerr << _T("[READ KB time=")									// 10/20/99 AM.
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC	// 10/20/99 AM.
	  << _T(" sec]") << endl;									// 10/20/99 AM.

return true;
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
_TCHAR path[FNAMESIZ];
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
#ifndef QDBM_
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

//if (_tsystem(cmd) < 0)															// 04/27/01 AM.
//	return false;																// 04/27/01 AM.
run_silent(cmd,0);		// 03/16/07 AM.

#endif
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
	*cgerr << _T("[Load kb lib: Already loaded.]") << endl;
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
			 << endl;
	return false;																// 05/06/01 AM.
	}

*cgerr << _T("[Loading compiled kb: ") << buf << _T("]") << endl;
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
return (CONCEPT *) cg_CONCEPT;	// 02/19/07 AM.
}

XCON_S *CG::findRoot_c()	// INTERNAL variant of findRoot.  USER MUST FREE STRUCT.	// 02/14/07 AM.
{
return qkbm_->Con(1);
}

// Find the named concept under given parent.
// Wend your way down from "concept", the root concept.
CONCEPT *CG::findConcept(
	CONCEPT *parent,
	_TCHAR *name
	)
{
if (!parent)
	return 0;
XCON_S *parent_con = qkbm_->Con((CON_ID)parent);
CON_ID child_cid = cgcon_->con_find_child(parent_con, name);	// 02/19/07 AM.
kbfree(parent_con);
return (CONCEPT *) child_cid;
}

// Find the nth concept under parent.
CONCEPT *CG::findConcept(CONCEPT *parent, int pos)
{
CON_ID phr;
if (!parent)
	return 0;
phr = qkbm_->get_dn((CON_ID) parent);	// 02/09/07 AM.
if (!phr)
	return 0;
XCON_S *phr_con = qkbm_->Con(phr);
CON_ID cid = cgcon_->con_nth(phr_con, pos);
kbfree(phr_con);
return (CONCEPT *) cid;
}

// NOOP function for debugging in caller.	// 03/01/07 AM.
void CG::noopCon(CONCEPT *conc)				// 03/01/07 AM.
{
if (!conc)
	return;
XCON_S *con = qkbm_->Con((CON_ID)conc);
kbfree(con);
}

// Find concept's attribute.				// 07/12/99 AM.
ATTR *CG::findAttr(CONCEPT *conc, _TCHAR *name)
{
if (!conc)		// 03/08/07 AM.
	return 0;	// 03/08/07 AM.

XCON_S *con = qkbm_->Con((CON_ID)conc);		// 03/08/07 AM.
PTR_ID pid = 0;										// 03/08/07 AM.
XPTR_S *ptr = qkbm_->attr_find_h(con, name,/*UP*/pid);	// 03/08/07 AM.
kbfree(con);				// 03/08/07 AM.
kbfree(ptr);				// 03/08/07 AM.
return (ATTR *) pid;		// 03/08/07 AM.
}

// Fetch the list of attributes for the concept.
ATTR *CG::findAttrs(CONCEPT *conc)
{
if (!conc)
	return (PTR_ID) 0;
return (ATTR *) qkbm_->con_get_attrs((CON_ID)conc);
}

// Fetch attribute name into given buffer.	// 07/05/99 AM.
bool CG::attrName(ATTR *attr, _TCHAR *buf, int bufsiz)
{
*buf = '\0';

if (!attr)
	return false;

_TCHAR *name;
if (!(name = cgattr_->attrs_name((PTR_ID)attr)))
	return false;

if (_tcsclen(name) >= (unsigned)bufsiz)
	{
	kbfree(name);	// 03/05/07 AM.
	return false;
	}
_tcscpy(buf, name);
kbfree(name);	// 03/05/07 AM.
return true;
}

// Fetch attribute values.				// 07/05/99 AM.
VAL *CG::attrVals(ATTR *attr)
{
XPTR_S *pattr = 0;
qkbm_->getXptr((PTR_ID)attr,/*UP*/ pattr);
if (!pattr)
	return 0;
if (pattr->kind != pXPTR)
	{
	kbfree(pattr);	// 02/28/07 AM.
	return 0;	// 02/28/07 AM.
	}

PTR_ID slot_pid = pattr->v.xptr;	// 02/28/07 AM.
kbfree(pattr);
if (!slot_pid)							// 02/28/07 AM.
	return 0;							// 02/28/07 AM.
XPTR_S *slot = 0;
qkbm_->getXptr(slot_pid,/*UP*/ slot);	// 02/28/07 AM.
if (!slot)
	return 0;							// 02/28/07 AM.

PTR_ID vals = slot->next;			// 02/28/07 AM.
kbfree(slot);							// 02/28/07 AM.
return (VAL *) vals;
//return (VAL *) cgattr_->attrs_vals((PTR_ID) attr);
}

// Fetch the list of values of concept's attribute.
VAL *CG::findVals(CONCEPT *conc, _TCHAR *s_attr)
{
//return (VAL_ID) attr_get((CON_ID)conc, s_attr);		// FIX.	// 08/12/99 AM.
return (VAL *) qkbm_->attr_find((CON_ID)conc, s_attr);		// FIX.	// 08/12/99 AM.
}

// Fetch the first value of given attribute of a concept.
// Treat as appropriate type if possible.
// WARN:	In QDBM_, user supplies buffer.
bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ _TCHAR *val)
{
*val = '\0';

// Get string-valued first value of given concept attribute.
return qkbm_->attr_find_str((CON_ID)conc,s_attr,val);	// 02/09/07 AM.
}

// WARN: USER MUST FREE STRING WITH cg_->kbfree.
bool CG::findVal_db(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ _TCHAR *&val)
{
// Get string-valued first value of given concept attribute.
*val = '\0';
return qkbm_->attr_find_str_db((CON_ID)conc,s_attr, val);
}

bool CG::findVal_id(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ STR_ID &sid)
{
sid = qkbm_->attr_find_str((CON_ID)conc,s_attr);
return sid ? true : false;
}

bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ long &val)
{
val = 0;
return qkbm_->attr_find_num((CON_ID)conc,s_attr, /*UP*/ val);
}

// VARIANT.
bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ float &val)
{
val = 0.0;
return qkbm_->attr_find_flt((CON_ID)conc,s_attr, /*UP*/ val);
}

// VARIANT.
bool CG::findVal(CONCEPT *conc, _TCHAR *s_attr, /*UP*/ CONCEPT *&val)
{
val = 0;
CON_ID v;
bool ok = qkbm_->attr_find_con((CON_ID)conc,s_attr, /*UP*/ v);
val = (CONCEPT *) v;

if (!ok)
	*cgerr << _T("[findVal: Attribute value not a concept.]") << endl;
return ok;
}

// GRAB THE STRING FROM KB.	// 02/13/07 AM.
// WARN:	USER MUST USE kbfree or qkbm_->kbfree() to free the string.
_TCHAR *CG::strVal(CONCEPT *conc, _TCHAR *s_attr)
{
char *str;
bool ok = qkbm_->attr_find_str_db((CON_ID)conc,s_attr, /*UP*/ str);	// 02/14/07 AM.

if (!ok)
	{
	*cgerr << _T("[strVal: Attribute value not a string.]") << endl;
	return 0;
	}
return str;
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
CON_ID node;
node = (CON_ID) findPhrase(tree);
for (; node; node = qkbm_->get_next(node))
	{
	conceptName((CONCEPT*)node, /*UP*/ buf);
	if (!_tcscmp(name, buf))
		return tree;				// Return the concept owning the phrase.
	}

// If not root and first in list, look at my siblings.
CON_ID tmp;
CONCEPT *found;
if (!isroot && !qkbm_->get_prev((CON_ID)tree))
	{
	for (tmp = qkbm_->get_next((CON_ID)tree); tmp; tmp = qkbm_->get_next(tmp))
		{
		if (found = hierConcept(name, (CONCEPT*)tmp, false))
			return found;
		}
	}

// Look at my children.
return hierConcept(name, (CONCEPT*)qkbm_->get_dn((CON_ID)tree), false);
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
CON_ID dni = ((XCON_S *)tree)->dn;
XCON_S *dnc = qkbm_->Con(dni);
sort_hier_rec(dnc, false);

// Look at me.
sortChilds(tree);

// If not root and first in list, look at my siblings.
CON_ID tmpi;
XCON_S *tmp;

bool ok = true;
if (!isroot && !((XCON_S *)tree)->prev)
	{
	for (tmpi = ((XCON_S *)tree)->next; tmpi; tmpi = tmp->next)
		{
		tmp = qkbm_->Con(tmpi);
		// Current sibling.
		sort_hier_rec((XCON_S *)tmp, true);
		}
	}
}


/********************************************
* FN:		CONCEPTNAME
* SUBJ:	Get name of given concept.
* CR:		04/28/99 AM.
* ARG:	User supplies buffer.
* OPT:	Todo: users should take kb string when feasible and free it, for efficiency.
********************************************/

bool CG::conceptName(CONCEPT *con, /*UP*/ _TCHAR *name,
	long bufsiz)																// 06/18/02 AM.
{
*name = '\0';
if (!con)
	return false;

STR_ID sid = qkbm_->con_get_name((CON_ID)con);
if (!sid)
	{
	*cgerr << _T("[conceptName: Concept has no name.]") << endl;
	return false;
	}

// Fetch the string from kb.
_TCHAR *tmp = qkbm_->getXstr(sid);
if (!tmp || !*tmp)
	{
	*cgerr << _T("[conceptName: Concept has no name.]") << endl;
	return false;
	}

size_t len = _tcsclen(tmp);
if (len >= (size_t) bufsiz)
	{
	_tcsncpy(name,tmp, bufsiz-1);
	name[bufsiz-1] = '\0';	// Terminate.
	}
else
	_tcscpy(name, tmp);

qkbm_->kbfree(tmp);		// Free kb string.

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
XCON_S *con = qkbm_->Con((CON_ID) conc);	// 02/19/07 AM.
cgcon_->con_to_path(con, /*DU*/ buf);
kbfree(con);										// 02/19/07 AM.
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
XCON_S *con = cgcon_->path_to_con(path);
if (!con)
	return 0;
CON_ID cid = con->id;
kbfree(con);	// OPT:	Inefficient.
return (CONCEPT *) cid;
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

bool dirt;																		// 06/29/03 AM.
XCON_S *word = cgdict_->dict_get_word(str,dirt);							// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return false;																// 06/29/03 AM.
if (dirt)																		// 06/29/03 AM.
	dirty_ = true;																// 06/29/03 AM.

cgcon_->con_to_path(word, /*DU*/ buf);
kbfree(word);																	// 02/19/07 AM.
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
CON_ID word;
if (!(word = cgdict_->dict_find_word(str)))
	return false;
XCON_S *xcon = qkbm_->Con(word);	// 02/19/07 AM.
if (!xcon)	// 03/08/07 AM.
	return false;	// 03/08/07 AM.
cgcon_->con_to_path(xcon, /*DU*/ buf);
kbfree(xcon);		// 02/19/07 AM.
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
return (CONCEPT *) cgdict_->dict_find_index(str);
}


/********************************************
* FN:		ATTRWITHVAL
* SUBJ:	See if attr and value are in concept.
* CR:		05/02/00 AM.
********************************************/

bool CG::attrWithval(CONCEPT *conc, _TCHAR *attr, _TCHAR *val)
{
return qkbm_->find_attr_val((CON_ID)conc, attr, val);
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
return attrExistsrec(tree, true, attr, val);
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
if (!root && !qkbm_->get_prev((CON_ID)tree))
	{
	CON_ID conc;
	for (conc = qkbm_->get_next((CON_ID)tree); conc; conc = qkbm_->get_next(conc))
		{
		if (attrExistsrec((CONCEPT*)conc, false, attr, val))
			return true;
		}
	}

// Look down the tree.
return attrExistsrec((CONCEPT*)qkbm_->get_dn((CON_ID)tree), false, attr, val);
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
return nodeAttrexistsrec(tree, true, attr, val);
}


bool CG::nodeAttrexistsrec(CONCEPT *tree, bool root, _TCHAR *attr, _TCHAR *val)
{
if (!tree)
	return false;

// Look for attribute in concept's phrase.
_TCHAR ptr[MAXPATH];
CON_ID node;
if (node = qkbm_->con_phrase((CON_ID) tree))
	{
	for (; node; node = qkbm_->get_next(node))
		{
		ptr[0] = '\0';
		if (findVal((CONCEPT*)node, attr, /*UP*/ ptr))
			{
			if (ptr && *ptr && !_tcscmp(val, ptr))		// FOUND IT!
				return true;
			}
		}
	}

// If first in list of concepts, manage it.
if (!root && !qkbm_->get_prev((CON_ID)tree))
	{
	CON_ID conc;
	for (conc = qkbm_->get_next((CON_ID)tree); conc; conc = qkbm_->get_next((CON_ID)conc))
		{
		if (nodeAttrexistsrec((CONCEPT*)conc, false, attr, val))
			return true;
		}
	}

// Look down the tree.
return nodeAttrexistsrec((CONCEPT*)qkbm_->get_dn((CON_ID)tree), false, attr, val);
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

bool dirt;	// 06/29/03 AM.

// Get the old value.
STR_ID ov_id = qkbm_->sym_get(val, dirt);

// Intern the new string value.
_TCHAR *str = 0;
STR_ID nv_id = qkbm_->sym_get(newval, dirt);

attrChangerec(tree, true, attr, ov_id, nv_id);
}

// OPT:	Doing redundant work.  Should find the attribute and value only
//			once and then replace the value.
void CG::attrChangerec(
	CONCEPT *tree,
	bool root,
	_TCHAR *attr,
	STR_ID val,
	STR_ID newval
	)
{
if (!tree)
	return;

STR_ID ptr = 0;

// Look for attribute in the current concept.
if (findVal_id(tree, attr, /*UP*/ ptr))
	{
	if (ptr && val == ptr)			// FOUND IT!
		{
		// Replace the value.
		replaceVal_id(tree, attr, newval);
		dirty_ = true;															// 05/12/00 AM.
		}
	}

// Look for attribute in concept's phrase.	// FIX				// 08/12/99 AM.
CON_ID node;
if (node = qkbm_->con_phrase((CON_ID) tree))
	{
	for (; node; node = qkbm_->get_next(node))
		{
		ptr = 0;
		if (findVal_id((CONCEPT*)node, attr, /*UP*/ ptr))
			{
			if (ptr && val == ptr)		// FOUND IT!
				{
				// Replace the value.
				replaceVal_id((CONCEPT *)node, attr, newval);
				dirty_ = true;													// 05/12/00 AM.
				}
			}
		}
	}


// If first in list of concepts, manage it.
if (!root && !qkbm_->get_prev((CON_ID)tree))
	{
	CON_ID conc;
	for (conc = qkbm_->get_next((CON_ID)tree); conc; conc = qkbm_->get_next((CON_ID)conc))
		{
		attrChangerec((CONCEPT*)conc, false, attr, val, newval);
		}
	}

// Look down the tree.
attrChangerec((CONCEPT*)qkbm_->get_dn((CON_ID)tree), false, attr, val, newval);
}


	//////////////////////
	// TRAVERSE THINGS
	//////////////////////

// Get a concept attached to the given concept.
CONCEPT *CG::Down(CONCEPT *conc)
{
return conc ? (CONCEPT *) qkbm_->get_dn((CON_ID)conc) : (CONCEPT *) 0;
}

CONCEPT *CG::Up(CONCEPT *conc)
{
// Must get to first concept of list, then its up.
// Fix. 05/13/99 AM.
// REIMPLEMENT. All hier concepts have parent point up-to-date.	// 02/19/07 AM.
if (!conc)
	return 0;
XCON_S *con = qkbm_->Con((CON_ID)conc);	// 02/19/07 AM.
CON_ID parent_cid = con->up;					// 02/19/07 AM.
kbfree(con);										// 02/19/07 AM.
return (CONCEPT *) parent_cid;				// 02/19/07 AM.
}

CONCEPT *CG::Next(CONCEPT *conc)
{
return conc ? (CONCEPT *) qkbm_->get_next((CON_ID)conc) : (CONCEPT *) 0;
}

CONCEPT *CG::Prev(CONCEPT *conc)
{
return conc ? (CONCEPT *) qkbm_->get_prev((CON_ID)conc) : (CONCEPT *) 0;
}

// Traverse a list of attributes.
ATTR *CG::popAttr(ATTR *&attrs)
{
if (!attrs)
	return (PTR_ID) 0;
PTR_ID ptr = (PTR_ID) attrs;
attrs = (ATTR *) qkbm_->ptr_get_next((PTR_ID) attrs);
return (ATTR *) ptr;
}

// Traverse a list of values.												// 03/02/00 AM.
VAL *CG::nextVal(VAL *val)													// 03/02/00 AM.
{
if (!val)
	return 0;
return (VAL *) qkbm_->ptr_get_next((PTR_ID)val);
}

// Pop attribute value as a string.		// 07/09/99 AM.
void CG::popSval(VAL *&vals, /*UP*/ _TCHAR *buf)
{
*buf = '\0';
long num = 0;
float fnum = 0.0;																// 08/16/01 AM.
if (!vals)
	return;
PTR_ID ptr;
ptr = (PTR_ID) vals;

// Pop (even if error, to forestall infinite loops).
vals = (VAL *) qkbm_->ptr_get_next((PTR_ID)vals);

if (!qkbm_->ptr_val_as_str(ptr, buf))
	*cgerr << _T("[popSval: Can't convert to string.]") << endl;
}

// Pop the next value from a list of attributes.
// WARN: USER SUPPLIES BUFFER.	// 02/13/07 AM.
void CG::popVal(VAL *&vals, /*UP*/ _TCHAR *val)
{
*val = '\0';
if (!vals)
	return;
PTR_ID ptr;
ptr = (PTR_ID) vals;

// Pop (even if error, to forestall infinite loops).
vals = (VAL *) qkbm_->ptr_get_next((PTR_ID)vals);

if (!qkbm_->ptr_get_str(ptr, /*UP*/ val))
	*cgerr << _T("[popVal: Attribute not a string.]") << endl;
}


void CG::popVal(VAL *&vals, /*UP*/ long &val)
{
val = -1;
if (!vals)
	return;
PTR_ID ptr;
ptr = (PTR_ID) vals;

vals = (VAL *) qkbm_->ptr_get_next((PTR_ID)vals);

if (!qkbm_->ptr_get_num(ptr,val))
	*cgerr << _T("[popVal: Attribute not a num.]") << endl;
}

// VARIANT. 08/16/01 AM.
void CG::popVal(VAL *&vals, /*UP*/ float &val)
{
val = -1.0;
if (!vals)
	return;
PTR_ID ptr;
ptr = (PTR_ID) vals;

vals = (VAL *) qkbm_->ptr_get_next((PTR_ID)vals);

if (!qkbm_->ptr_get_flt(ptr,val))
	*cgerr << _T("[popVal: Attribute not a float.]") << endl;
}

// VARIANT.	// 08/12/00 AM.
void CG::popVal(VAL *&vals, /*UP*/ CONCEPT *&val)
{
val = 0;
if (!vals)
	return;
PTR_ID ptr;
ptr = (PTR_ID) vals;

vals = (VAL *) qkbm_->ptr_get_next((PTR_ID)vals);

if (!(val = (CONCEPT *) qkbm_->ptr_get_con(ptr)))
	*cgerr << _T("[popVal: Attribute not a concept.]") << endl;
}

// 6/3/02 Dd.
bool CG::isValConcept(VAL *&vals)
{
if (!vals)
	return false;
XPTR_S *ptr;
qkbm_->getXptr((PTR_ID) vals, /*DU*/ ptr);
bool ok = ((ptr && ptr->kind == pXCON) ? true : false);
kbfree(ptr);

return ok;
}

	//////////////////////
	// MAKE THINGS
	//////////////////////

// Make named concept under given parent.
// Concept is placed at end of list of children.
CONCEPT *CG::makeConcept(CONCEPT *parent, _TCHAR *name)
{
dirty_ = true;																	// 05/12/00 AM.
XCON_S *con = qkbm_->Con((CON_ID)parent);			// FIX.	// 03/21/07 AM.
CON_ID cid = cgcon_->con_add_basic(name, con);	// FIX.	// 03/21/07 AM.
kbfree(con);				// FIX.	// 03/21/07 AM.
return (CONCEPT *) cid;	// FIX.	// 03/21/07 AM.
}

// Make named concept at given position under parent.
// pos == 0 means at add end of list.
CONCEPT *CG::makeConcept(CONCEPT *parent, _TCHAR *name, int pos)
{
dirty_ = true;																	// 05/12/00 AM.
XCON_S *con = qkbm_->Con((CON_ID)parent);
CON_ID cid = cgcon_->con_add_nth(con, name, pos);
kbfree(con);
return (CONCEPT *) cid;
}

// Add concept to nth position in parent's children.
// pos == 0 means at end of list.
CONCEPT *CG::addConcept(CONCEPT *parent, CONCEPT *conc, int pos)
{
dirty_ = true;																	// 05/12/00 AM.
XCON_S *pcon = qkbm_->Con((CON_ID) parent);
XCON_S *con = qkbm_->Con((CON_ID) conc);
CON_ID res = cgcon_->con_add_nth(pcon, con, pos);
kbfree(con);
kbfree(pcon);
return (CONCEPT *) res;
}

// Add an empty attribute. (For an editor.)	// 07/12/99 AM.
ATTR *CG::addAttr(CONCEPT *conc, _TCHAR *attr)
{
dirty_ = true;																	// 05/12/00 AM.
XCON_S *con = qkbm_->Con((CON_ID)conc);
PTR_ID pid = 0;	// 03/19/07 AM.
XPTR_S *ptr = qkbm_->attr_get_h(con, attr,/*UP*/pid);
if (ptr)
	kbfree(ptr);	// 03/19/07 AM.
kbfree(con);	// FIX.	// 03/19/07 AM.
return (ATTR *) pid;	// 03/19/07 AM.
}


// Add value as a string type.	// 07/09/99 AM.
bool CG::addSval(CONCEPT *con, _TCHAR *attr, _TCHAR *val)
{
dirty_ = true;																	// 05/12/00 AM.
return addVal(con, attr, val);
}


bool CG::addSval(CONCEPT *con, _TCHAR *attr, long val)
{
_TCHAR buf[256];
dirty_ = true;																	// 05/12/00 AM.
_stprintf(buf, _T("%ld"), val);
return addVal(con, attr, buf);
}


// Add string value at end of attribute's values.
bool CG::addVal_id(CONCEPT *conc, _TCHAR *attr, STR_ID val)
{
dirty_ = true;																	// 05/12/00 AM.
if (cgattr_->attr_add_by_name_s((CON_ID) conc, attr, val))
	return true;
return false;
}

bool CG::addVal(CONCEPT *conc, _TCHAR *attr, _TCHAR *val)
{
dirty_ = true;																	// 05/12/00 AM.
if (cgattr_->attr_add_by_name((CON_ID) conc, attr, val))
	return true;
return false;
}

// Add num value at end of attribute's values.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, long val)
{
dirty_ = true;																	// 05/12/00 AM.
if (cgattr_->attr_add_by_name((CON_ID) conc, attr, val))
	return true;
return false;

}

// Add float value at end of attribute's values.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, float val)				// 06/11/03 AM.
{
dirty_ = true;
if (cgattr_->attr_add_by_name((CON_ID) conc, attr, val))
	return true;
return false;

}


// Add CONCEPT values at end of attr's values.						// 08/11/00 AM.
bool CG::addVal(CONCEPT *conc, _TCHAR *attr, CONCEPT *val)			// 08/11/00 AM.
{
dirty_ = true;
if (cgattr_->attr_add_by_name((CON_ID)conc, attr, (CON_ID)val))
	return true;
return false;
}


CONCEPT *CG::addWord(_TCHAR *str)
{
if (!str || !*str)
	return 0;

#ifdef OLD_030629_
CON_ID word;
if (!(word = cgdict_->dict_find_word(str)))
	{
	if (!(word = cgdict_->dict_add_word(str)) )
		return 0;
	dirty_ = true;																// 05/12/00 AM.
	}
#endif

bool dirt;																		// 06/29/03 AM.
XCON_S *word = cgdict_->dict_get_word(str,dirt);							// 06/29/03 AM.
if (!word)																		// 06/29/03 AM.
	return 0;																	// 06/29/03 AM.
if (dirt)																		// 06/29/03 AM.
	dirty_ = true;																// 06/29/03 AM.

CON_ID cid = word->id;	// 03/20/07 AM.
kbfree(word);				// 03/20/07 AM.

return (CONCEPT *) cid;
}

	//////////////////////
	// FIND OR MAKE THINGS
	//////////////////////

// Find or make concept under given parent.
CONCEPT *CG::getConcept(CONCEPT *parent_cid, _TCHAR *name)
{
XCON_S *parent_con = qkbm_->Con((CON_ID)parent_cid);
if (!parent_con)
	return 0;
CON_ID cid = cgcon_->con_find_child(parent_con, name);			// 02/19/07 AM.
if (cid)																// FIX.	// 08/07/00 AM.
	{
	kbfree(parent_con);														// 02/19/07 AM.
	return (CONCEPT *) cid;										// FIX.	// 08/07/00 AM.
	}
// Didn't find it, so make it.
dirty_ = true;		// Todo: check if made concept.					// 05/12/00 AM.
cid = cgcon_->con_add_basic(name, parent_con);						// 08/07/00 AM.
kbfree(parent_con);
return (CONCEPT *) cid;
}
	//////////////////////
	// DELETE THINGS
	//////////////////////

// Free function for QDBM objects.		`	// 02/11/07 AM.
// For example, delete strings from kbms.	// 02/11/07 AM.
bool CG::kbfree(void *obj)
{
if (!obj)
	return true;	// That's ok.

qkbm_->kbfree(obj);	// 03/13/07 AM.
return true;
}

// Delete the given concept.
bool CG::rmConcept(CONCEPT *conc)
{
if (!conc)
	return false;

XCON_S *con = qkbm_->Con((CON_ID) conc);
cgcon_->con_rm_hier(con);
dirty_ = true;	
kbfree(con);
return true;
}


// Delete the named concept from its parent.
bool CG::rmConcept(CONCEPT *parent_cid, _TCHAR *name)
{
if (!parent_cid || str_empty(name))
	return false;

XCON_S *parent = qkbm_->Con((CON_ID)parent_cid);
CON_ID cid = cgcon_->con_find_child(parent, name);
kbfree(parent);
if (!cid)
	return false;

XCON_S * con = qkbm_->Con(cid);
bool ok = cgcon_->con_rm_hier(con);
kbfree(con);
dirty_ = true;
return ok;
}

// Delete nth concept from its parent.
bool CG::rmConcept(CONCEPT *parent, int pos)
{
if (!parent)
	return false;

XCON_S *parent_con;
if (!(parent_con = qkbm_->Con((CON_ID)parent)))
	return false;
XCON_S *child_con = qkbm_->Con(parent_con->dn);
kbfree(parent_con);
if (!child_con)
	return false;

CON_ID pcon = cgcon_->con_nth(child_con, pos);
kbfree(child_con);
if (!pcon)
	return false;
XCON_S *con = qkbm_->Con(pcon);
bool ok = cgcon_->con_rm_hier(con);
kbfree(con);
if (ok)																				// 02/19/07 AM.
	dirty_ = true;																	// 05/12/00 AM.
return ok;																			// 02/19/07 AM.
}

// Delete values of concept's attribute.
// 08/26/99 AM. Don't care if attribute is missing.
bool CG::rmVals(CONCEPT *conc, _TCHAR *attr)
{
bool dirty = false;
if (!cgattr_->attr_rm_by_name((CON_ID) conc, attr, /*UP*/ dirty))						// 05/25/01 AM.
	return false;																// 05/25/01 AM.

dirty_ = dirty;																	// 05/12/00 AM.
return true;																	// 08/26/99 AM.
}

bool CG::rmVal(ATTR *attr, VAL *val)							// 05/11/00 AM.
{
dirty_ = true;																	// 05/12/00 AM.
return cgattr_->rm_one_val((PTR_ID)attr, (PTR_ID)val);
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
return cgattr_->attr_rm((CON_ID)conc, (PTR_ID) attr);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, _TCHAR *val_s)		// 05/17/00 AM.
{
dirty_ = true;
return cgattr_->attr_rm_val_by_name((CON_ID)con, attr_s, val_s);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, long long val)			// 05/19/00 AM.
{
dirty_ = true;
return cgattr_->attr_rm_val_by_name((CON_ID)con, attr_s, val);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, float val)		// 06/11/03 AM.
{
dirty_ = true;
return cgattr_->attr_rm_val_by_name((CON_ID)con, attr_s, val);
}

bool CG::rmAttrval(CONCEPT *con, _TCHAR *attr_s, CONCEPT *val)			// 05/19/00 AM.
{
dirty_ = true;
return cgattr_->attr_rm_val_by_concept((CON_ID)con, attr_s, (CON_ID)val);
}

// REMOVE ALL "NORMAL" ATTRIBUTES OF CONCEPT.						// 10/31/00 AM.
bool CG::rmAttrs(CONCEPT *conc)
{
bool dirty = false;
bool ok = qkbm_->attrs_rm((CON_ID)conc, /*UP*/ dirty);
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

XCON_S *con = qkbm_->Con((CON_ID) conc);

CON_ID down = con->dn;

if (!down && !con->phrases)
	return true;	// Ok if concept has no phrase or children.

dirty_ = true;																	// 05/12/00 AM.

qkbm_->con_rm_phrases(con);

if (!down)
	return true;

// Detach children from concept.
con->dn = 0;
qkbm_->con_update(con->id, con);
kbfree(con);

// Really remove concepts and their data from kb, including their phrases.
cgcon_->con_rm_children(down);
return true;
}

// Remove all the phrases (ie samples) from the given subtree.
// If given a node (ie sample), does nothing.	// 08/10/99 AM.
bool CG::prunePhrases(CONCEPT *tree)
{
if (!tree)
	return false;
prunePhrasesRec(tree, true);
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
if (!root && !qkbm_->get_prev((CON_ID)conc))
	{
	CON_ID tmp;
	for (tmp = qkbm_->get_next((CON_ID)conc); tmp; tmp = qkbm_->get_next((CON_ID)tmp))
		prunePhrasesRec((CONCEPT*)tmp, false);
	}

// Prune phrases from concept's children.
prunePhrasesRec((CONCEPT*)qkbm_->get_dn((CON_ID)conc), false);
}



// Remove word.																// 07/17/00 AM.
bool CG::rmWord(_TCHAR *str)
{
if (!str || !*str)
	return false;

dirty_ = true;
return cgdict_->dict_rm_word(str);
}


	//////////////////////
	// REPLACE THINGS
	//////////////////////

// Replace attribute value with given value.
bool CG::replaceVal_id(CONCEPT *con, _TCHAR *attr, STR_ID val)	// 02/14/07 AM.
{
rmVals(con, attr);
dirty_ = true;
return addVal_id(con, attr, val);
}

bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, _TCHAR *val)
{
//*cgerr << "[replaceVal: attr=" << attr << ",val=" << val << "]" << endl;
rmVals(con, attr);															// 05/28/01 AM.
dirty_ = true;																	// 05/12/00 AM.
return addVal(con, attr, val);
}

bool CG::replaceVal(CONCEPT *con, _TCHAR *attr, long val)
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
bool dirt;
STR_ID strid = qkbm_->sym_get(name,dirt);				// 02/09/07 AM.
return qkbm_->con_set_name((CON_ID)con, strid, dirty_);		// 02/09/07 AM.
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
PTR_ID ptr;
if (!(ptr = (PTR_ID) findAttr(con, attr)))
	return false;
return renameAttr((ATTR *) ptr, rename);
}

bool CG::renameAttr(ATTR *attr, _TCHAR *rename)
{
dirty_ = true;																	// 05/12/00 AM.
return cgattr_->attr_rename((PTR_ID)attr, rename);
}

	//////////////////////
	// MOVE THINGS
	//////////////////////

// "Bubble" concept left.
bool CG::moveCleft(CONCEPT *con)
{
// Move node handled separately.

bool ok = qkbm_->con_move_left((CON_ID) con);
if (ok)
	dirty_ = true;
return ok;
}


// "Bubble" concept right.
bool CG::moveCright(CONCEPT *con)
{
// Move node handled separately.

bool ok = qkbm_->con_move_right((CON_ID) con);
if (ok)
	dirty_ = true;
return ok;
}

// Create new parent concept and place existing concepts underneath
// Allowing end to be empty.	// 02/26/07 AM.
CONCEPT *CG::makeParentConcept(_TCHAR *name, CONCEPT *conStart, CONCEPT *conEnd)
{
if (!conStart || str_empty(name))
	return false;

XCON_S *start = qkbm_->Con((CON_ID)conStart);
XCON_S *end = qkbm_->Con((CON_ID)conEnd);
CON_ID parent_cid = cgcon_->con_reduce_sublist(name, start, end);
kbfree(start);
kbfree(end);
return (CONCEPT *) parent_cid;
}

// Adopt concepts under new concept
// 02/13/07 AM. I believe this is obsolete. Not re-implementing for now.
void CG::adoptConcepts(CONCEPT *conParent, CONCEPT *conStart, CONCEPT *conEnd)
{
/*
if (!conParent || !conStart || !conEnd ||
	((CON_ID) conParent)->kind != cXBASIC || ((CON_ID) conStart)->kind != cXBASIC || ((CON_ID) conEnd)->kind != cXBASIC)
	return;

dirty_ = true;

CON_ID sibling = (CON_ID)conStart;
CON_ID next = NULL;

while (sibling) {
	next = qkbm_->get_next(sibling);
	if (!rmConcept(sibling))
		return;
	qkbm_->con_add_nth((CON_ID) conParent, (CON_ID)sibling, 0);
	if (sibling == ((CON_ID)conEnd))
		break;
	sibling = next;
}
*/
}

// Sort concept's children.	[NEW!]
bool CG::sortChilds(CONCEPT *conc)										 // 02/20/09 AM.
{
if (!conc)
	return false;
dirty_ = true;
XCON_S *con = qkbm_->Con((CON_ID)conc);
bool ok = cgcon_->sort_childs(con);
kbfree(con);
return ok;
}

	//////////////////////
	// PHRASE MGMT
	//////////////////////

// Find concept's phrase.
PHRASE *CG::findPhrase(CONCEPT *conc)
{
return (CONCEPT *) qkbm_->con_phrase((CON_ID) conc);
}

// Sort concept's phrase.	[NEW!]
bool CG::sortPhrase(CONCEPT *conc)
{
if (!conc)
	return false;
dirty_ = true;																	// 05/12/00 AM.
XCON_S *con = qkbm_->Con((CON_ID)conc);
bool ok = cgcon_->sort_phrase(con);
kbfree(con);
return ok;
}

long CG::phraseLength(CONCEPT *conc)
{
CON_ID phr = qkbm_->con_phrase((CON_ID) conc);	// 03/09/07 AM.
XCON_S *phrc = qkbm_->Con(phr);
long len = cgcon_->phrase_len(phrc);
kbfree(phrc);
return len;
}

// FIND CONCEPT THAT THIS NODE IS A PROXY FOR.						// 12/03/02 AM.
// Changed functionality.													// 12/03/02 AM.
CONCEPT *CG::nodeConcept(CONCEPT *node)
{
if (node)
	{
	XCON_S *node_c = qkbm_->Con((CON_ID)node);						// 02/19/07 AM.
	if (node_c->kind != cXPROXY)											// 02/19/07 AM.
		return 0;																// 02/19/07 AM.
	CON_ID target_cid = node_c->v.vcon;
	kbfree(node_c);		// Delete this copy.							// 02/19/07 AM.
	return (CONCEPT *) target_cid;										// 02/19/07 AM.
//	return (CONCEPT *) cgcon_->nc((CON_ID)node);
	}
return 0;
}

// Find node's owning concept.			// 08/27/99 AM.
// RENAMED THIS FUNCTIONALITY FROM nodeconcept.						// 12/03/02 AM.
CONCEPT *CG::nodeOwner(CONCEPT *node)
{
if (!node)
	return 0;
XCON_S *node_c = qkbm_->Con((CON_ID)node);
if (!node_c)
	return 0;
CON_ID owner_id = cgcon_->node_owner(node_c);	// 02/19/07 AM.
kbfree(node_c);
return (CONCEPT *) owner_id;

/*
CONCEPT *fst;
if (!(fst = listNode(node)))
	return 0;
return cgattr_->attr_get_c((CON_ID)fst, qkbm_->c_cg_ATOM);
*/
}

// Find nth phrase node.  pos = 0 means find last node.
CONCEPT *CG::findNode(PHRASE *phr, int pos)
{
if (!phr)
	return 0;

XCON_S *phr_con = qkbm_->Con((CON_ID) phr);
if (!phr_con)
	return 0;

CON_ID cid = cgcon_->con_nth(phr_con, pos);
kbfree(phr_con);
return (CONCEPT *) cid;
}


// Find first node that matches.
CONCEPT *CG::findNode(PHRASE *phr, _TCHAR    *name)
{
XCON_S *phrc = qkbm_->Con((CON_ID)phr);
XCON_S *con = cgcon_->node_find(phrc, name);
CON_ID cid;
if (con)
	{
	cid = con->id;
	kbfree(con);
	return (CONCEPT *) cid;
	}
return 0;
}

// Given a node, find first node in list.	// 08/31/99 AM.
CONCEPT *CG::listNode(CONCEPT *node)
{
// Rather than traverse backward, every node now points to the owning
// concept.  So grab that and get the first phrase node from there.
if (!node)
	return 0;
XCON_S *nn = qkbm_->Con((CON_ID)node);
if (!nn)
	return 0;
if (nn->kind != cXPROXY)
	{
	kbfree(nn);
	return 0;
	}
CON_ID nid = qkbm_->con_phrase(nn->atom);
kbfree(nn);
return (CONCEPT *) nid;
}

// Conveniences.
CONCEPT *CG::firstNode(PHRASE *phr)
{
if (!phr || qkbm_->get_prev((CON_ID)phr))
	{
	*cgerr << _T("[firstNode: Must be called with a phrase.]") << endl;
	return 0;
	}
return (CONCEPT *) phr;
}

CONCEPT *CG::lastNode( PHRASE *phr)
{
if (!phr)	// 02/19/07 AM.
	return 0;
XCON_S *phr_con = qkbm_->Con((CON_ID)phr);
if (!phr_con)
	return 0;

// "Phrase" is represented by first node in list.
if (phr_con->prev)
	{
	*cgerr << _T("[lastNode: Must be called with a phrase.]") << endl;
	kbfree(phr_con);
	return 0;
	}

CON_ID cid = cgcon_->con_nth(phr_con, 0);
kbfree(phr_con);
return (CONCEPT *) cid;
}

// Create a phrase under concept by adding first node.
PHRASE *CG::makePhrase(CONCEPT *conc, _TCHAR *name)
{
XCON_S *con = qkbm_->Con((CON_ID)conc);
CON_ID cid = cgcon_->make_phrase_using_dict(con, name);
kbfree(con);
if (cid)
	dirty_ = true;
return (PHRASE *) cid;
}

// NEW!  Sorry, has to be named differently (everything is really void).
CONCEPT *CG::addCnode(CONCEPT *conc, _TCHAR *name)
{
XCON_S *owner = qkbm_->Con((CON_ID)conc);
CON_ID nid = cgcon_->phr_add_nth(owner, name, 0);
kbfree(owner);
return (CONCEPT *) nid;
}


CONCEPT *CG::addCnode(CONCEPT *conc, CONCEPT *nconc)	// 11/27/02 AM.
{
XCON_S *con;
if (!(con = qkbm_->Con((CON_ID)conc)))
	return 0;
XCON_S *ncon;
if (!(ncon = qkbm_->Con((CON_ID)nconc)))
	{
	kbfree(con);
	return 0;
	}

CON_ID cid = cgcon_->phr_add_nth(con, ncon, 0);
kbfree(con);
kbfree(ncon);
if (cid)
	dirty_ = true;
return (CONCEPT *) cid;
}

// Add a node to a concept's phrase. Create phrase as needed.
// Phrase can be passed down, or 0 if empty or not known.
// Phrase will be passed back up by the function.
// New node is returned by the function.
CONCEPT *CG::addNode(CONCEPT *con, _TCHAR *name, /*DU*/ PHRASE *&phr)
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
XCON_S *phrc;
if (!(phrc = qkbm_->Con((CON_ID)phr)))
	return 0;

// Get owner.	// FIX.	// 03/08/07 AM.
XCON_S *owner = qkbm_->Con(phrc->atom);	// FIX.	// 03/08/07 AM.
kbfree(phrc);
CON_ID cid = cgcon_->phr_add_nth(owner, name, pos);
kbfree(owner);
if (cid)
	dirty_ = true;
return (CONCEPT *) cid;

/*
CON_ID owner;
if (!(owner = cgattr_->attr_get_c((CON_ID)phr, qkbm_->c_cg_ATOM)))
	return 0;

dirty_ = true;																	// 05/12/00 AM.
return (CONCEPT *) cgcon_->phr_add_nth(owner, name, pos, this);
*/
}


// Add detached node at given position.
CONCEPT *CG::addNode(PHRASE *phr, CONCEPT *node,  int pos)
{
XCON_S *phrc = qkbm_->Con((CON_ID) phr);
if (!phrc)
	return 0;

CON_ID owner = cgcon_->node_owner(phrc);
if (!owner)
	{
	kbfree(phrc);
	return false;
	}
XCON_S *ownerc = qkbm_->Con(owner);
XCON_S *nodec = qkbm_->Con((CON_ID)node);
CON_ID nid = cgcon_->phr_add_nth(ownerc, nodec, pos);
if (nid)
	dirty_ = true;
kbfree(nodec);
kbfree(ownerc);
kbfree(phrc);
return (CONCEPT *) nid;
}


// Add node before n_pos.  If n_pos == 0, add at end of phrase.
// RENAMED FROM addNode.	// 02/13/07 AM.
CONCEPT *CG::addNode(PHRASE *phr, CONCEPT *node, CONCEPT *n_pos)
{
if (!phr || !node)
	return 0;

XCON_S *phr_c = qkbm_->Con((CON_ID)phr);
if (!phr_c)
	return 0;

CON_ID owner = cgcon_->node_owner(phr_c);
if (!owner)
	{
	kbfree(phr_c);
	return 0;
	}
XCON_S *own_con = qkbm_->Con(owner);
XCON_S *nc = qkbm_->Con((CON_ID)node);
XCON_S *pos_con = qkbm_->Con((CON_ID)n_pos);

dirty_ = true;																	// 05/12/00 AM.
CON_ID res = cgcon_->insert_node(own_con, nc, pos_con);
kbfree(pos_con);
kbfree(nc);
kbfree(own_con);
kbfree(phr_c);
return (CONCEPT *) res;
}

// Move node from one position to another. from,to=0 end position.
// Note: Whether data structures are switched or their content is
// replaced is undefined.  Don't rely on one or the other.
CONCEPT *CG::moveNode(PHRASE *phr, int        from, int        to)
{
CON_ID n_from, n_to;
n_from = n_to = 0;

// Find both nodes.
if (!(n_from = (CON_ID) findNode(phr, from)))
	return 0;

if (to == 0)					// Moving to last position.
	n_to = 0;
else if (!(n_to = (CON_ID) findNode(phr, to)))
	return 0;

if (n_from == n_to)			// No op.
	return (CONCEPT *) n_from;

dirty_ = true;																	// 05/12/00 AM.
if (!rmNode_cid(phr, (CONCEPT *)n_from)) // Remove the from node.
	return 0;

// Add node in proper position.
return addNode(phr, (CONCEPT*)n_from, (CONCEPT*)n_to);
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

if (!rmNode_cid(phr, n_from)) // Remove the from node.
	return 0;

// Add node in proper position.
dirty_ = true;																	// 05/12/00 AM.
return addNode(phr, n_from, n_to);

}


// "Bubble" node left.
bool CG::moveNleft(CONCEPT *nid)
{
if (!nid)
	return false;

XCON_S *node = qkbm_->Con((CON_ID)nid);
if (!node)
	return false;

// Get the node before.
CON_ID before_nid = node->prev;
if (!before_nid)	// can't move node to the left.
	{
	kbfree(node);
	return false;
	}

if (!qkbm_->phrase_detach_node(node))
	{
	kbfree(node);
	return false;
	}

XCON_S *before = qkbm_->Con(before_nid);

// Get the one before that!
CON_ID pre_nid = before->prev;
kbfree(before);

XCON_S *pre;
bool ok = true;

if (pre_nid)	// There is a previous node.
	{
	pre = qkbm_->Con(pre_nid);
	ok = (qkbm_->phrase_insert_after(pre, node) ? true : false);
	kbfree(pre);
	}
else				// Re-add node as first in the phrase.
	{
	CON_ID owner_cid = cgcon_->node_owner(node);
	XCON_S *owner = qkbm_->Con(owner_cid);
	ok = qkbm_->phrase_add_start(owner, node) ? true : false;
	kbfree(owner);
	}

kbfree(node);
return ok;
}


// "Bubble" node right.
bool CG::moveNright(CONCEPT *nid)
{
if (!nid)
	return false;

XCON_S *node = qkbm_->Con((CON_ID)nid);
if (!node)
	return false;

// Get the node after.
CON_ID after_nid = node->next;
if (!after_nid)	// cant' move node to the right.
	return false;

if (!qkbm_->phrase_detach_node(node))
	return false;

XCON_S *after = qkbm_->Con(after_nid);
bool ok = (qkbm_->phrase_insert_after(after, node) ? true : false);

kbfree(after);
kbfree(node);
return ok;
}


// Delete a node.
bool CG::rmNode(PHRASE *&phr,  _TCHAR *name)
{
if (!phr)
	return 0;

XCON_S *phrc = qkbm_->Con((CON_ID)phr);
if (!phrc)
	return 0;

XCON_S *con;
if (!(con = cgcon_->node_find(phrc, name)))
	{
	kbfree(phrc);
	return false;
	}

// To keep this function behavior as-is, return the concept's phrase.
CON_ID ownerid = cgcon_->node_owner(con);
XCON_S *owner = qkbm_->Con(ownerid);

if (!qkbm_->phrase_rm_node(con))		// Remove node from phrase and delete.
	{
	kbfree(phrc);
	kbfree(con);
	return false;
	}

phr = (PHRASE *) qkbm_->con_phrase(owner);	// UP. concept's phrase.
kbfree(owner);

dirty_ = true;																	// 05/12/00 AM.
kbfree(phrc);
kbfree(con);
return true;
}

bool CG::rmNode(PHRASE *&phr,  int    pos)
{
phr = 0;

CON_ID cid;
if (!(cid = (CON_ID) findNode((CONCEPT *) phr, pos)))
	return false;
XCON_S *con = qkbm_->Con(cid);

// To keep this function behavior as-is, return the concept's phrase.
CON_ID ownerid = cgcon_->node_owner(con);
XCON_S *owner = qkbm_->Con(ownerid);

if (!qkbm_->phrase_rm_node(con))		// Remove node from phrase and delete.
	{
	kbfree(owner);
	kbfree(con);
	return false;
	}

phr = (PHRASE *) qkbm_->con_phrase(owner);	// UP. concept's phrase.
kbfree(owner);

dirty_ = true;																	// 05/12/00 AM.
kbfree(owner);
kbfree(con);
return true;
}

// Remove node from its phrase.
bool CG::rmNode_cid(PHRASE *&phr, CONCEPT *node)
{
phr = 0;

if (!node)
	return false;
// Given phr can help speed up, but ignoring for now.	// 02/13/07 AM.
XCON_S *nodec = qkbm_->Con((CON_ID)node);

// To keep this function behavior as-is, return the concept's phrase.
CON_ID ownerid = cgcon_->node_owner(nodec);
XCON_S *owner = qkbm_->Con(ownerid);

bool ok = qkbm_->phrase_rm_node(nodec);

phr = (PHRASE *) qkbm_->con_phrase(owner);	// UP. concept's phrase.
kbfree(owner);

if (ok)
	dirty_ = true;
kbfree(nodec);
return ok;
}

// Remove node from its phrase.
bool CG::rmNode(CONCEPT *node)
{
XCON_S *nodec = qkbm_->Con((CON_ID)node);
bool ok = qkbm_->phrase_rm_node(nodec);
kbfree(nodec);
if (ok)
	dirty_ = true;																	// 05/12/00 AM.
return ok;
}


// Delete phrase.
bool CG::rmPhrase(PHRASE *&phr)
{

XCON_S *node = qkbm_->Con((CON_ID)phr);
if (!node)
	return false;
CON_ID owner_id = cgcon_->node_owner(node);
if (!owner_id)
	return false;
kbfree(node);
XCON_S *owner = qkbm_->Con(owner_id);
if (!owner)
	return false;
bool ok = qkbm_->con_rm_phrases(owner);				// 03/11/07 AM.
kbfree(owner);

dirty_ = true;																	// 05/12/00 AM.
phr = 0;				// Zero out caller's phrase pointer.
return true;
}

bool CG::rmCphrase(CONCEPT *owner)
{
if (!owner)			// 03/11/07 AM.
	return false;	// Error. No concept given.	// 03/11/07 AM.
XCON_S *con = qkbm_->Con((CON_ID)owner);			// 03/11/07 AM.
if (!con)			// 03/11/07 AM.
	return false;	// 03/11/07 AM.
bool ok = qkbm_->con_rm_phrases(con);				// 03/11/07 AM.
kbfree(con);		// 03/11/07 AM.

dirty_ = true;																	// 05/12/00 AM.
return ok;
}

// Rename node.
bool CG::renameNode(PHRASE *phr, _TCHAR *name, _TCHAR *rename)
{
XCON_S *phrc;
if (!(phrc = qkbm_->Con((CON_ID)phr)))
	return 0;
bool ok = (cgcon_->phrase_rename_node(phrc, name, rename) ? true : false);
kbfree(phrc);
if (ok)
	dirty_ = true;
return ok;

}

bool CG::renameNode(PHRASE *phr, int    pos, _TCHAR *rename)
{
XCON_S *phrc;
if (!(phrc = qkbm_->Con((CON_ID)phr)))
	return 0;

bool ok = (cgcon_->phrase_rename_node(phrc, pos, rename) ? true : false);
kbfree(phrc);
if (ok)
	dirty_ = true;
return ok;
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
*cgerr << _T("[Starting modular kb dump.]") << endl;

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
_TCHAR path[FNAMESIZ];
_stprintf(path, _T("%s%c%s"), getAppdir(), DIR_CH, dir);

// If tmp directory absent, create it.
make_dir(path);

// Prep and open up the output files.
_TCHAR o_hier[FNAMESIZ];
_TCHAR o_word[FNAMESIZ];
_TCHAR o_attr[FNAMESIZ];
_TCHAR o_phr[FNAMESIZ];

_TCHAR *suff;
suff = _T("kb");		// Kb file suffix.

_stprintf(o_hier, _T("%s%chier.%s"), path, DIR_CH, suff);
_stprintf(o_word, _T("%s%cword.%s"), path, DIR_CH, suff);
_stprintf(o_attr, _T("%s%cattr.%s"), path, DIR_CH, suff);
_stprintf(o_phr,  _T("%s%cphr.%s"),  path, DIR_CH, suff);

// Open the files for output.
_t_ofstream f_hier(TCHAR2A(o_hier));
_t_ofstream f_word(TCHAR2A(o_word));
_t_ofstream f_phr(TCHAR2A(o_phr));

_t_ofstream *f_attr = new _t_ofstream(TCHAR2A(o_attr));	// 07/01/03 AM.

// GET PATH TO "root" CONCEPT.
CON_ID parent = (CON_ID) Up(root);
_TCHAR spath[MAXPATH];
if (!parent)			// Root of hierarchy
	{
	spath[0] ='\0';
	f_hier << _T("add empty") << endl;
	f_hier << _T("add root") << endl;
	}
else
	{
	XCON_S *xcon = qkbm_->Con(parent);	// 02/19/07 AM.
	cgcon_->con_to_path(xcon, /*UP*/ spath);
	kbfree(xcon);								// 02/19/07 AM.
	}

// Traverse the subtree.  Write out the goodies.
long n_attr=0;												// 07/01/03 AM.
long c_attr=0;												// 07/01/03 AM.
XCON_S *con = qkbm_->Con((CON_ID) root);			// 03/08/07 AM.
if (!writeTree(con, true, f_hier,f_word,f_phr, spath,
			f_attr,n_attr,c_attr,0,0))					// 07/01/03 AM.
	{
	kbfree(con);											// 03/08/07 AM.
	delete f_attr;											// 08/07/03 AM.
	return false;
	}
kbfree(con);												// 03/08/07 AM.
delete f_attr;												// 08/07/03 AM.

// Terminate the last command file.
f_phr  << endl << _T("quit") << endl << endl;

// TODO: CATENATE THE COMMAND FILES TO THE OUTPUT FILE.
#ifndef LINUX
// Do the catenate in C++.
_TCHAR buf[MAXLN];
_t_ofstream *f_dump = new _t_ofstream(TCHAR2A(file));	// Final output file.
_t_ifstream *f_in = new _t_ifstream(TCHAR2A(o_hier));
while (f_in->getline(buf, MAXLN))		// Copy first file.
	*f_dump << buf << endl;
delete f_in;

*f_dump << endl;								// Separate commands.
f_in = new _t_ifstream(TCHAR2A(o_word));
while (f_in->getline(buf, MAXLN))		// Copy first file.
	*f_dump << buf << endl;
delete f_in;

*f_dump << endl;								// Separate commands.
f_in = new _t_ifstream(TCHAR2A(o_attr));
while (f_in->getline(buf, MAXLN))		// Copy first file.
	*f_dump << buf << endl;
delete f_in;

*f_dump << endl;								// Separate commands.
f_in = new _t_ifstream(TCHAR2A(o_phr));
while (f_in->getline(buf, MAXLN))		// Copy first file.
	*f_dump << buf << endl;
delete f_in;

delete f_dump;

#else
_TCHAR cmd[2048];
_TCHAR *cp = _T("cat");
_stprintf(cmd, _T("%s %s %s %s %s %s"),
	cp, o_hier, o_word, o_attr, o_phr, file);
_tsystem(cmd);
#endif

*cgerr << _T("[Done with modular kb dump.]") << endl;
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
	XCON_S *con,			// 03/08/07 AM.
	bool flag,				// true if root of subtree.
	_t_ofstream &hier,
	_t_ofstream &word,
	_t_ofstream &phr,
	_TCHAR *path,				// Hierarchy path down to parent.
	_t_ofstream *&attr,		// Current attr output stream.			// 07/01/03 AM.
	long &n_attr,			// # of attr files.						// 07/01/03 AM.
	long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
	_TCHAR *o_attr,			// Buff for building attr filenames.	// 07/01/03 AM.
	_TCHAR *fpath,			// Path for attr file.						// 07/01/03 AM.
	_TCHAR *suff				// Filename suffix.							// 07/01/03 AM.
	)
{
if (!con)
	return true;

//char name[128];
_TCHAR name[MAXSTR];	// 12/17/03 AM.
_TCHAR *kbstr = qkbm_->getXstr(con->v.vname);
_tcscpy(name, kbstr);
kbfree(kbstr);

if (!name || !*name)
	{
	*cgerr << _T("[writeTree: No concept name.]") << endl;
	return false;
	}

_TCHAR buf[2048];
prep_str(buf, name);

// Print information for the current concept.

switch (con->kind)
	{
	case cXBASIC:
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
			hier << endl;
			}
//		else
//			{
			// Axiomatics.
//			hier << "add empty" << endl;
//			hier << "add root" << endl;
//			}
		break;
	case cXWORD:
		// Need to handle some chars specially.
		{
#ifndef UNICODE
		word << _T("add word \"") << buf << _T("\"") << endl;
#else
		char *lpstr8;
		word << _T("add word \"");
		u_to_mbcs((LPCWSTR)buf, CP_UTF8, (LPCTSTR*&)lpstr8);
		word << lpstr8;
		u_delete((LPCTSTR*&)lpstr8);
		word << _T("\"");
		word << endl;
#endif
		}
		break;
	default:
		*cgerr << _T("[writeTree: Not word or basic concept.]") << endl;
		return false;
	}

// Make path to concept itself.
_TCHAR conpath[MAXPATH];
if (path && *path)
	_stprintf(conpath, _T("%s \"%s\""), path, buf);
else if (*buf && !_tcscmp(buf,_T("concept")))				// CHECK.	// 12/28/01 AM.
	_stprintf(conpath, _T("\"%s\""), buf);
else																				// 12/28/01 AM.
	{
	*cgerr << _T("[writeTree:  Bad path.]") << endl;						// 12/28/01 AM.
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
		  << endl;
	return false;
	}

// TODO: If concept has a phrase, traverse that separately...
if (!writePhrase(con, hier,word,attr,phr,conpath,
						n_attr,c_attr,o_attr,fpath,suff))				// 07/01/03 AM.
	{
	*cgerr << _T("[Error calling writePhrase]") << endl;
	return false;
	}

// Traverse children of concept.
XCON_S *dcon = qkbm_->Con(con->dn);	// 03/08/07 AM.
if (!writeTree(dcon, false, hier,word,phr,conpath,
			attr,n_attr,c_attr,o_attr,fpath,suff))						// 07/01/03 AM.
	{
	kbfree(dcon);
	*cgerr << _T("[Error calling writeTree]") << endl;
	return false;
	}
kbfree(dcon);	// 03/08/07 AM.

// If concept is first in a list, traverse its siblings.
if (!flag && !con->prev)
	{
	// Don't free the given con.		// 03/08/07 AM.
	CON_ID sib = con->next;
	XCON_S *sibc = qkbm_->Con(sib);	// 02/28/07 AM.
	bool ok;
	while (sibc)	// 03/08/07 AM.
		{
		// Handle sibling as root of a subtree.
		ok = writeTree(sibc, true, hier,word,phr,path,
			attr,n_attr,c_attr,o_attr,fpath,suff);						// 07/01/03 AM.
		sib = sibc->next;			// 03/08/07 AM.
		kbfree(sibc);				// 03/08/07 AM.
		if (!ok)
			{
			*cgerr << _T("[Error in sibling call of writeTree]") << endl;
			return false;
			}
		sibc = qkbm_->Con(sib);	// 02/28/07 AM.
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
	XCON_S *con,			// Concept (or node).	// 03/08/07 AM.
	int pos,					// 0 for concept, positive int for each
								// node of concept's phrase.
	_t_ofstream *&o_attr,		// File to write attributes to.	// 07/01/03 AM.
	_TCHAR *conpath,			// Hierarchy path down to (node's) concept.
	long &n_attr,			// # of attr files.						// 07/01/03 AM.
	long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
	_TCHAR *b_attr,															// 07/01/03 AM.
	_TCHAR *fpath,															// 07/01/03 AM.
	_TCHAR *suff																// 07/01/03 AM.
	)
{

if (!con)
	{
	*cgerr << _T("[writeAttrs: Given null concept.]") << endl;
	return false;
	}

PTR_ID attrs = 0;
if (!(attrs = qkbm_->con_get_attrs(con->id)))
	return true;				// No attrs. That's OK.

PTR_ID p_slot, p_vals;
XPTR_S *vals, *slot;
CON_ID c_slot;
//PVAL pv;
_TCHAR *str, *s_slot;
_TCHAR *kbstr;	// 02/13/07 AM.
_TCHAR *s_kind;
_TCHAR buf[MAXPATH];					// 04/29/99 AM.
_TCHAR sbuf[MAXPATH];					// 04/29/99 AM.
_TCHAR strbuf[2048];				// 04/30/99 AM.
int val_pos = 0;	// If concept value is a node.	// 10/09/05 AM.

while (attrs)
   {
   p_slot = qkbm_->ptr_get_ptr(attrs);	// Get slot ptr id.
	qkbm_->getXptr(p_slot, /*DU*/ slot);	// Get slot obj.
	p_vals = slot->next;
	c_slot = slot->v.xcon;
	kbfree(slot);
	XCON_S *xx = qkbm_->Con(c_slot);		// 02/19/07 AM.
	cgcon_->con_to_path(xx, /*UP*/ sbuf);
	kbfree(xx);									// 02/19/07 AM.
   s_slot = sbuf;								// 04/29/99 AM.
   while (p_vals)
      {
      // Unpack current val.
      qkbm_->getXptr(p_vals, /*DU*/ vals);
      
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
         case pXCON:
            {
				// Handle case where value is a KB NODE.					// 10/09/05 AM.
				// val_pos = node number...									// 10/09/05 AM.
				XCON_S *qcon = qkbm_->Con(vals->v.xcon);
				XCON_S *hcon = qkbm_->Con(cgcon_->node_owner(qcon));		// 10/09/05 AM.
				if (hcon)		// Original concept is a node.			// 10/09/05 AM.
					{
					// Get position of the node.								// 10/09/05 AM.
					val_pos = cgcon_->con_ord(qcon);					// 10/09/05 AM.
					cgcon_->con_to_path(hcon, /*UP*/ buf);			// 10/09/05 AM.
					}
				else																	// 10/09/05 AM.
					{
					cgcon_->con_to_path(qcon, /*UP*/ buf);
					}
				kbfree(qcon);
				kbfree(hcon);
				//str = qkbm_->con_str(vals->v.vcon);				// 04/29/99 AM.
				str = buf;															// 04/29/99 AM.
				s_kind = _T("pcon");
				}
				break;
/*
         case pSYM:
				str = vals->v.vsym->str;
				prep_str(buf, str);											// 04/30/99 AM.
				_stprintf(strbuf, _T("\"%s\""), buf);							// 04/30/99 AM.
				str = strbuf;													// 04/30/99 AM.
				s_kind = _T("psym");
				break;
*/
         case pXSTR:
				kbstr = qkbm_->getXstr(vals->v.xstr);
//				str = vals->v.xstr;
				prep_str(buf, kbstr);											// 04/30/99 AM.
				_stprintf(strbuf, _T("\"%s\""), buf);							// 04/30/99 AM.
				str = strbuf;													// 04/30/99 AM.
				s_kind = _T("pst");
				kbfree(kbstr);
				break;
         case pNUM:
				_stprintf(buf, _T("%ld"), vals->v.vnum);
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
         case pXPTR:
         default:
			// 04/20/99 AM.
        //fprintf(stderr, "[attrs_pret: Bad value kind=%d]\n", vals->kind);
		  *cgerr << _T("[attrs_pret: Bad value kind=") << vals->kind << endl;
            exit(1);
         }

		// For backward compatibility, avoid ind nattr.				// 10/09/05 AM.
		if (!val_pos)															// 10/09/05 AM.
			*o_attr << _T("ind attr") << endl;
		else																		// 10/09/05 AM.
			*o_attr << _T("ind nattr") << endl;							// 10/09/05 AM.
		*o_attr << conpath << endl;
		*o_attr << pos << endl;
		*o_attr << s_slot << endl;
		*o_attr << s_kind << endl;
		*o_attr << str << endl;												// 04/29/99 AM.
		if (val_pos)															// 10/09/05 AM.
			*o_attr << val_pos << endl;									// 10/09/05 AM.
		*o_attr << _T("end ind") << endl << endl;

		// Manage current attr kb file.									// 07/01/03 AM.
		if (n_attr	// If zero, keep the old way.						// 07/01/03 AM.
		 && ++c_attr >= ATTRS_PER_FILE )									// 07/01/03 AM.
			{
			// Close up the attr file and open a new one.			// 07/01/03 AM.
			++n_attr;	// Increment file count.						// 07/01/03 AM.
			c_attr = 0;	// Reset count of attributes.					// 07/01/03 AM.
			*o_attr << endl << _T("quit") << endl << endl;				// 07/01/03 AM.
			delete o_attr;	// Maxed out in the old file.				// 07/01/03 AM.
			_stprintf(b_attr, _T("%s%cattr%ld.%s"),fpath,DIR_CH,n_attr,suff);
			o_attr = new _t_ofstream(TCHAR2A(b_attr));								// 07/01/03 AM.
			}

		p_vals = vals->next;
		kbfree((char *) vals);
      }
   //fprintf(out, "\n");													// 04/20/99 AM.
	*o_attr << endl;															// 04/20/99 AM.
   attrs = qkbm_->ptr_get_next(attrs);
   }

return true;
}


/********************************************
* FN:		WRITEPHRASE
* SUBJ:	Traverse and write out concept's phrase.
* CR:		04/30/99 AM.
********************************************/

bool CG::writePhrase(			// 04/30/99 AM.
	XCON_S *con,					// 03/08/07 AM.
	_t_ofstream &o_hier,
	_t_ofstream &o_word,
	_t_ofstream *&o_attr,
	_t_ofstream &o_phr,
	_TCHAR *path,				// Hierarchy path down to parent.
	long &n_attr,			// # of attr files.						// 07/01/03 AM.
	long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
	_TCHAR *b_attr,															// 07/01/03 AM.
	_TCHAR *fpath,															// 07/01/03 AM.
	_TCHAR *suff																// 07/01/03 AM.
	)
{
if (!con)
	{
	*cgerr << _T("[writePhrase: no concept]") << endl;
	return false;
	}

CON_ID phr;
if (!(phr = qkbm_->con_phrase(con)))
	return true;			// No phrase.  That's OK.

XCON_S *bas;				// Node's basic concept.
_TCHAR buf[MAXPATH];
CON_ID ptr = phr;				// For traversing.
XCON_S *ptr_con;		// 02/19/07 AM.

// Traverse phrase and write it out.
o_phr << _T("ind phrase") << endl;
o_phr << path << endl;
while (ptr)		// 02/19/07 AM.
	{
	ptr_con = qkbm_->Con(ptr);	// 02/19/07 AM.
	ptr = ptr_con->next;			// 02/19/07 AM.
	kbfree(ptr_con);				// 03/08/07 AM.
	bas = cgcon_->nc(ptr_con);	// 02/19/07 AM.
	cgcon_->con_to_path(bas, /*UP*/ buf);
	o_phr << buf << endl;
	kbfree(bas);					// 02/19/07 AM.
	}
o_phr << _T("end ind") << endl << endl;

// Retraverse phrase to write out attributes.
// (Opt: not optimal, but simple and modular.)
if (!(phr = qkbm_->con_phrase(con)))		// 03/08/07 AM.
	return true;			// No phrase.  That's OK.
ptr = phr;	// 03/08/07 AM.
int pos = 0;
bool ok;	// 03/08/07 AM.
while (ptr)	// 03/08/07 AM.
	{
	ptr_con = qkbm_->Con(ptr);
	ok = writeAttrs(ptr_con, ++pos, o_attr, path,
							n_attr,c_attr,b_attr,fpath,suff);		// 07/01/03 AM.
	ptr = ptr_con->next;	// 03/08/07 AM.
	kbfree(ptr_con);		// 03/08/07 AM.
	if (!ok)
		{
		*cgerr << _T("[writeTree: Couldn't write attrs for con=") << path
			  << _T(", node=") << pos
			  << _T("]") << endl;
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
//cout << "infile=" << infile << endl;
//ifstream fin(infile, ios::in | ios::nocreate);	// Upgrade. // 01/24/01 AM.
_t_ifstream fin(TCHAR2A(infile), ios::in);							// Upgrade.	// 01/24/01 AM.
#ifdef UNICODE
if (!u_readbom(&fin))
	{
	*cgerr << _T("[readFile: Unhandled Unicode format=") << infile << _T("]") << endl;
	return false;
	}
#endif

if (!fin)
	{
	*cgerr << _T("[readFile: File not found=") << infile << _T("]") << endl;
	return false;
	}

// "Take" the input command file.
//if (!cmd_top(&fin, &cout, "> ", false))
//if (!cmd_top(&fin, &cout, "> ", false,true))						// 03/27/00 AM.
if (!cmd_top(&fin, cgerr, _T("> "), false,this,true))					// 02/19/02 AM.
	{
	*cgerr << _T("[readFile: Error in reading file=") << infile << _T("]") << endl;
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
if (!(word = (CONCEPT *) cgdict_->dict_find_word(str)) )
        return 0;
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
CON_ID word;
bool dirt;	// 06/29/03 AM.
XCON_S *con;	// 03/09/07 AM.
if (!(con = cgdict_->dict_get_word(str,dirt)) )
        return 0;
word = con->id;	// 03/09/07 AM.
kbfree(con);	// OPT: Inefficient...	// 03/09/07 AM.
return (CONCEPT *) word;
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
XCON_S *con = cgdict_->dict_first();	// 03/20/07 AM.
CON_ID cid = 0;
if (con)
	{
	cid = con->id;	// 03/20/07 AM.
	kbfree(con);	// 03/20/07 AM.
	}
return (CONCEPT *) cid;	// 03/20/07 AM.
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
XCON_S *cc = qkbm_->Con((CON_ID)con);
if (!cc)
	return 0;	// 03/08/07 AM.
XCON_S *nxt = cgdict_->dict_next(cc);
CON_ID nxt_id = nxt->id;
kbfree(nxt);
kbfree(cc);
return (CONCEPT *) nxt_id;
}

/**********************     END OF FILE         ***************************/
