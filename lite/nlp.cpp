/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NLP.CPP
* FILE:	lite\nlp.cpp
* CR:		10/19/98 AM.
* SUBJ:	This class manages an instance of a text analyzer and its operational
*			environment.
* NOTE:	08/27/02 AM. Now subordinate to the new VTRUN class, which oversees
*			the runtime system, including multiple analyzers.
*
*******************************************************************************/

#ifdef _WINDOWS
#include "StdAfx.h"
#endif
#include <iostream>	// 09/27/19 AM.
#include <strstream>	// 09/27/19 AM.
#include <time.h>
#include "machine.h"				// 03/08/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "prim/libprim.h"	// 09/15/08 AM.
#ifndef DWORD
#define DWORD double
#endif
LIBPRIM_API DWORD run_silent(_TCHAR* strCMD);	// 09/15/08 AM.

#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "lite/mach.h"			// 03/10/99 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"				// 11/12/00 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
//#include "thtab.h"				// 02/26/01 AM.
#include "io.h"
#include "str.h"					// 07/24/00 AM.
#include "words/words.h"		// 10/16/00 AM.
#include "words/wordarrs.h"	// 10/16/00 AM.
#include "std.h"
#include "chars.h"
#include "slist.h"				// 11/24/98 AM.
#include "stab.h"
#include "htab.h"
#include "tree.h"
#include "pn.h"
#include "gen.h"					// 05/10/00 AM.
#include "thtab.h"				// 02/26/01 AM.

#include "ana.h"
#include "parse.h"
#include "tok.h"
#include "line.h"
#include "pat.h"					// 12/03/98 AM.
#include "intern.h"				// 12/03/98 AM.
#include "rec.h"					// 11/08/99 AM.

#include "seqn.h"
#include "rfa.h"

#include "irule.h"
#include "iregion.h"				// 11/24/98 AM.
#include "irecurse.h"			// 11/24/98 AM.
#include "ifile.h"				// 11/24/98 AM.
#include "iexpr.h"				// 11/10/99 AM.
#include "ivar.h"					// 11/09/99 AM.
#include "iop.h"					// 11/10/99 AM.
#include "istmt.h"				// 02/14/00 AM.
#include "iexprstmt.h"			// 02/14/00 AM.
#include "iifstmt.h"				// 02/14/00 AM.
#include "iwhilestmt.h"			// 08/31/00 AM.
#include "ireturn.h"				// 03/07/02 AM.
#include "mode.h"					// 04/04/00 AM.
#include "sem.h"					// 02/26/01 AM.

#include "check.h"				// 11/27/98 AM.
#include "lite/code.h"					// 02/14/00 AM.
#include "postrfa.h"
#include "string.h"
#include "dyn.h"					// 01/29/99 AM.
#include "lite/Aseq.h"					// 10/07/99 AM.
#include "Eana.h"					// 10/13/99 AM.
#include "func_defs.h"			// 12/20/01 AM.
#include "ifunc.h"				// 12/20/01 AM.
#include "lite/vtrun.h"			// 08/28/02 AM.
#include "consh/libconsh.h"	// 07/18/03 AM.
#include "consh/cg.h"			// 07/18/03 AM.

#ifdef RUNEMBED_
//#include "analyzer.h"	// 09/29/19 AM.
extern "C" bool run_analyzer(Parse *);	// 09/29/19 AM.
#endif

#include "lite/nlp.h"

void nlp_clean(Ana *rfa);

int NLP::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

NLP::NLP(
	_TCHAR *appdir,		// Application base dir
	bool develop,		// If development mode.		// 12/25/98 AM.
	bool silent,		// Silent run mode.			// 06/16/02 AM.
	bool compiled,		// If compiled analyzer.	// 07/03/00 AM.
	_TCHAR *outdir,		// Intermed files.			// 03/10/99 AM.
//	char *datadir		// Data area for analyzer.	// 12/08/99 AM.
	_TCHAR *name,			// Unique name for analyzer.	// 01/06/03 AM.
	_TCHAR *logfile,	// VTLOG	// 05/06/13 AM.
	VTRun *vtrun		// VT Runtime Environment	// [DEGLOB]	// 10/15/20 AM.
	)
{
vtrun_ = vtrun;	// [DEGLOB]	// 10/15/20 AM.
//rfa_  = 0;
//rfb_  = 0;													// 11/05/99 AM.
htab_ = 0;
stab_ = 0;
//htfunc_ = 0;												// 12/20/01 AM.

ana_  = 0;
fbatchstart_ = false;									// 10/19/00 AM.
finteractive_ = false;									// 05/06/02 AM.
isDirRun_ = false;
isLastFile_ = false;
isFirstFile_ = false;
#ifndef LINUX
hdll_ = 0;													// 01/29/99 AM.
hrundll_ = 0;												// 05/14/00 AM.
#endif
cg_   = 0;													// 02/15/00 AM.
gui	= 0;							// Dave snuck this var in.			// 02/21/02 AM.
dbgout_	= 0;																	// 08/26/02 AM.

logfile_ = logfile;	// VTLOG	// 05/06/13 AM.

//if (datadir)
//	strcpy(datadir_, datadir);							// 12/08/99 AM.
//else
//	datadir_[0] = '\0';									// 12/08/99 AM.

popupmsg_ = popuptyp_ = popupdat_ = 0;				// 05/24/02 AM.

if (name && *name)															// 01/06/03 AM.
	_tcscpy(name_,name);														// 01/06/03 AM.
else																				// 01/06/03 AM.
	name_[0] = '\0';															// 01/06/03 AM.

if (vtrun_)		// 01/06/03 AM.	// [DEGLOB]	// 10/15/20 AM.
	vtrun_->addAna(this);	// Register this ana instance.	// 01/06/03 AM. // [DEGLOB]	// 10/15/20 AM.

init(appdir, develop,silent,compiled,outdir); // Set these objects up properly.

#ifndef STABLE_
++count_;
#endif
}

// A DUMMY CONSTRUCTOR FOR RFA, RFB.				// 06/30/03 AM.
NLP::NLP(
	bool dummy
	)
{
vtrun_ = 0;	// [DEGLOB]	// 10/15/20 AM.
htab_ = 0;
stab_ = 0;
//htfunc_ = 0;

ana_  = 0;
fbatchstart_ = false;
finteractive_ = false;
isLastFile_ = false;
isFirstFile_ = false;
#ifndef LINUX
hdll_ = 0;
hrundll_ = 0;
#endif
cg_   = 0;
gui	= 0;
dbgout_	= 0;

popupmsg_ = popuptyp_ = popupdat_ = 0;

name_[0] = '\0';

logfile_ = 0;	// VTLOG	// 05/06/13 AM.

// Seed random number generator.											// 03/10/04 AM.
srand((unsigned)time(NULL));												// 03/10/04 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

NLP::~NLP()
{
// User-defined cleanups in USER Project.								// 01/23/02 AM.
user_fin();																		// 01/23/02 AM.

if (vtrun_)	// 01/06/03 AM.	// [DEGLOB]	// 10/15/20 AM.
	vtrun_->rmAna(this);	// Unregister this ana instance.	// 01/06/03 AM. // [DEGLOB]	// 10/15/20 AM.

clean();

//if (rfa_)
//	{
//	RFA::rfa_clean(*rfa_);		// Special cleanups for RFA.
//	delete rfa_;
//	rfa_ = 0;						// 11/05/99 AM.
//	}
//if (rfb_)							// 11/05/99 AM.
//	{
	// Clean this up like a regular analyzer.
//	delete rfb_;					// 11/05/99 AM.
//	rfb_ = 0;						// 11/05/99 AM.
//	}

// Now hash table and string table are LOCAL.	// 06/25/03 AM.
if (htab_)
	{
	delete htab_;											// 06/25/03 AM.
	htab_ = 0;
	}
if (stab_)
	{
	delete stab_;											// 06/25/03 AM.
	stab_ = 0;
	}
//if (htfunc_)							// 12/20/01 AM.
//	Ifunc::deleteHtab(htfunc_);	// 12/20/01 AM.
//	htfunc_ = 0;
if (ana_)
	delete ana_;

// FOR NOW, NLP OBJECT "OWNS" THE KB.									// 07/18/03 AM.
if (cg_)																			// 07/18/03 AM.
	{
//	delete cg_;																	// 07/18/03 AM.
	CG::deleteCG(cg_);														// 07/28/03 AM.
	cg_ = 0;																		// 07/18/03 AM.
	}

#ifndef LINUX
if (hdll_)
	{
	unload_dll(hdll_);	// 01/29/99 AM.
	hdll_ = 0;				// 01/29/99 AM.
	}
if (hrundll_)
	{
	unload_dll(hdll_);	// 05/14/00 AM.
	hrundll_ = 0;			// 05/14/00 AM.
	}
#endif

if (dbgout_)																	// 08/26/02 AM.
	{
	delete dbgout_;															// 08/26/02 AM.
	dbgout_ = 0;																// 08/26/02 AM.
	}

logfile_ = 0;	// VTLOG	// 05/06/13 AM.

#ifndef STABLE_
--count_;
#endif
}

/********************************************
* FN:		Access Functions
********************************************/
void *NLP::getAna()			{return ana_;}								// 01/13/99 AM.
//void *NLP::getRFA()			{return rfa_;}								// 01/13/99 AM.
//void *NLP::getRFB()			{return rfb_;}								// 11/05/99 AM.
void *NLP::getHtab()			{return htab_;}							// 01/18/99 AM.
void *NLP::getStab()			{return stab_;}							// 05/26/02 AM.
//void *NLP::getHtfunc()		{return htfunc_;}							// 12/20/01 AM.
VTRun *NLP::getVTRun()			{return vtrun_;}	// [DEGLOB]	// 10/15/20 AM.
#ifndef LINUX
HINSTANCE NLP::getHdll()	{return hdll_;}							// 01/29/99 AM.
HINSTANCE NLP::getHrundll(){return hrundll_;}						// 05/14/00 AM.
#endif
//char *NLP::getDatadir()		{return datadir_;}					// 12/08/99 AM.
CG	  *NLP::getCG()			{return cg_;}								// 02/15/00 AM.
bool	NLP::getFbatchstart(){return fbatchstart_;}					// 10/19/00 AM.
bool	NLP::getFinteractive(){return finteractive_;}				// 05/06/02 AM.

_TCHAR *NLP::getPopupmsg()	{return popupmsg_;}						// 05/24/02 AM.
_TCHAR *NLP::getPopuptyp()	{return popuptyp_;}						// 05/24/02 AM.
_TCHAR *NLP::getPopupdat()	{return popupdat_;}						// 05/24/02 AM.
_TCHAR *NLP::getName()			{return name_;		}						// 01/06/03 AM.
//bool	NLP::getDevelop()		{return develop_;	}						// 07/18/03 AM.
//bool	NLP::getSilent()		{return silent_;	}						// 07/18/03 AM.
//bool	NLP::getCompiled()	{return compiled_;}						// 07/18/03 AM.
bool	NLP::getIsDirRun()	{return isDirRun_;}
bool	NLP::getIsLastFile()	{return isLastFile_;}
bool	NLP::getIsFirstFile()	{return isFirstFile_;}

/********************************************
* FN:		Modify Functions
********************************************/
//void	NLP::setDatadir(char *x)	{if (x) strcpy(datadir_,x);}// 12/08/99 AM.
void	NLP::setCG(CG *x)				{cg_ = x;}							// 02/15/00 AM.
void	NLP::setHtab(Htab *x)		{htab_ = x;}						// 07/03/03 AM.
void	NLP::setStab(Stab *x)		{stab_ = x;}						// 07/03/03 AM.
void	NLP::setFbatchstart(bool x){fbatchstart_ = x;}				// 10/19/00 AM.
void	NLP::setFinteractive(bool x){finteractive_ = x;}			// 05/06/02 AM.
//void	NLP::setHtfunc(void *x)		{htfunc_			= x;}				// 12/20/01 AM.
void	NLP::setVTRun(VTRun *x)		{vtrun_ = x;}	// [DEGLOB]	// 10/15/20 AM.

void	NLP::setPopupmsg(_TCHAR *x)	{popupmsg_ = x;}					// 05/24/02 AM.
void	NLP::setPopuptyp(_TCHAR *x)	{popuptyp_ = x;}					// 05/24/02 AM.
void	NLP::setPopupdat(_TCHAR *x)	{popupdat_ = x;}					// 05/24/02 AM.
//void	NLP::setDevelop(bool x)		{develop_  = x;}					// 07/18/03 AM.
//void	NLP::setSilent(bool x)		{silent_   = x;}					// 07/18/03 AM.
//void	NLP::setCompiled(bool x)	{compiled_ = x;}					// 07/18/03 AM.
void	NLP::setIsDirRun(bool val)	{isDirRun_ = val;}
void	NLP::setIsLastFile(bool val)	{isLastFile_ = val;}
void	NLP::setIsFirstFile(bool val)	{isFirstFile_ = val;}
void	NLP::setName(_TCHAR *x)												// 01/06/03 AM.
{
if (x && *x)
	_tcscpy(name_,x);
else
	name_[0] = '\0';
}

/********************************************
* FN:		Class-wide Functions
********************************************/

/********************************************
* FN:		GETCOUNT
* CR:		12/14/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int NLP::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		12/14/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void NLP::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active NLP count=") << count_ << std::endl;
	*gout << _T("Active NLP count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active NLP count=") << count_ << std::ends;
	errOut(&gerrStr,false,0,0);
	}
}
#endif


/********************************************
* FN:		INIT
* CR:		12/14/98 AM.
* SUBJ:	Initializations for Lite.
* NOTE:	Get appdir, build RFA here.
*			Should probably hide the rfa, htab, stab in an NLP object.
*			Then it can clean itself and call this init function when
*			created.
* ALLOC:	User will need to call a function to clean up the RFA, etc.
********************************************/

void NLP::init(
	_TCHAR *appdir,			// Application base directory.  (spec files, debug out)
	bool develop,			// Development mode.							// 12/25/98 AM.
	bool silent,	// Silent mode.										// 06/16/02 AM.
	bool compiled,			// Loading compiled analyzer.				// 07/03/00 AM.
	_TCHAR *outdir			// Intermed outputs.							// 03/10/99 AM.
	)
{

#ifdef GEODUCK_
gui_.Init();
gui = &gui_;
#endif

bool verbose;
bool ok = true;																// 07/19/00 AM.
//ostream *sout;
//ofstream *fout;
clock_t   s_time, e_time;
_TCHAR buf[MAXSTR];																// 01/30/99 AM.

#ifdef GEODUCK_
gui_.SetStatus(GUI_STATUS_INITIALIZING);								// 11/29/99 DD.
#endif
verbose = false;

// Without this, can't find the error output on the console.
//cerr = cout;	// HACK.	// 10/18/98 AM.	// NEED XP FIX.		// 01/12/03 AM.

if (!appdir || !*appdir || !dir_exists(appdir))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Lite init: Given bad appdir.]") << std::ends;			// 09/16/99 AM.
	errOut(&gerrStr,false,0,0);

	nlp_clean(0);																// 12/14/98 AM.
	return;
	}

_TCHAR tmp[MAXSTR];
if (!outdir || !*outdir)													// 03/10/99 AM.
	{
	// Make the default here.
	_stprintf(tmp, _T("%s%coutput"), appdir, DIR_CH);
	outdir = tmp;
	}

if (!dir_exists(outdir))
	make_dir(outdir);

// Moved error reporting stream up here.								// 01/17/02 AM.
std::_t_ostream *serr;																	// 03/22/99 AM.
std::_t_ofstream *ferr;																// 03/22/99 AM.
std::_t_ostream *sdbg=0;																// 02/21/02 AM.
_TCHAR errout[MAXMSG];															// 03/22/99 AM.
//if (!silent)																	// 06/16/02 AM.
	{
	_stprintf(errout, _T("%s%cinit.log"), outdir,							// 03/22/99 AM.
				DIR_CH);															// 03/08/00 AM.
	fileErr(errout, /*DU*/ ferr, serr);
	fileDbg(outdir,sdbg);													// 02/21/02 AM.
	}

// Making a logs folder for various reporting.						// 05/19/01 AM.
// Just using default area.												// 05/19/01 AM.
_TCHAR *logdirname = _T("logs");													// 05/19/01 AM.
_TCHAR logdir[MAXSTR];															// 05/19/01 AM.
_stprintf(logdir, _T("%s%c%s"), appdir,DIR_CH,logdirname);				// 05/19/01 AM.
if (!dir_exists(logdir))													// 05/19/01 AM.
	make_dir(logdir);															// 05/19/01 AM.


// SOME GLOBAL INITS.														// 09/27/00 AM.
// Get length of array used by spell correction.					// 09/27/00 AM.
// Using words.dll now.														// 10/16/00 AM.
//spell_words_len = set_arr_len(spell_words);						// 10/16/00 AM.

#ifdef OLD_020828_
// GETTING RFB DEFINITION AND DICTIONARY DATA, ETC. FROM A GLOBAL
// AREA FOR TAI DATA, if not defined by the user.
_TCHAR *taidir = 0;
if (!datadir_ || !*datadir_)												// 12/08/99 AM.
	{
	taidir = _tgetenv(_T("TAI"));													// 12/08/99 AM.
	if (!taidir)																// 12/08/99 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Lite: No 'TAI' environment var.]") << std::ends;	// 12/08/99 AM.
		errOut(&gerrStr,false);
		resetErr(ferr, serr);												// 01/17/02 AM.
		resetDbg(sdbg);														// 02/21/02 AM.
		nlp_clean(0);															// 12/08/99 AM.
		return;
		}
	_stprintf(datadir_, _T("%s%cdata"), taidir, DIR_CH);					// 12/08/99 AM.
	}

if (!datadir_)																	// 12/08/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Lite: No 'TAI' environment var.]") << std::ends;		// 12/08/99 AM.
	errOut(&gerrStr,false);
	resetErr(ferr, serr);													// 01/17/02 AM.
	resetDbg(sdbg);															// 02/21/02 AM.
	nlp_clean(0);																// 12/08/99 AM.
	return;
	}
if (!dir_exists(datadir_))													// 12/08/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Lite: No TAI environment directory.]") << std::ends;	// 12/08/99 AM.
	errOut(&gerrStr,false);
	resetErr(ferr, serr);													// 01/17/02 AM.
	resetDbg(sdbg);															// 02/21/02 AM.
	nlp_clean(0);																// 12/08/99 AM.
	return;
	}

_TCHAR rfbspecdir[MAXSTR];													// 12/08/99 AM.
_stprintf(rfbspecdir, _T("%s%crfb%c%s"), datadir_,							// 12/08/99 AM.
			DIR_CH, DIR_CH,													// 03/08/00 AM.
			SPECDIRNAME);														// 05/14/00 AM.
#endif

// LOAD APP-SPECIFIC USER EXTENSIONS TO THE LITE LIBRARY.		// 01/29/99 AM.
// QUICK FIX TO LIBRARY SELF-CONSISTENCY.  LOAD DEBUG AND		// 04/07/01 AM.
// RELEASE CONFIGS OF USER.DLL AS APPROPRIATE.						// 04/07/01 AM.

//_stprintf(buf, _T("%s%cuser%cdebug%cuser.dll"), appdir,		// 01/30/99 AM.
//			DIR_CH,DIR_CH,DIR_CH);											// 03/08/00 AM.

#ifdef _DEBUG

#ifdef UNICODE
_TCHAR *fname = _T("userdu");							// userdu.dll	// 01/21/06 AM.
#else
_TCHAR *fname = _T("userd");							// userd.dll	// 01/21/06 AM.
#endif

#else	// NOT DEBUG

#ifdef UNICODE
_TCHAR *fname = _T("useru");							// useru.dll	// 01/21/06 AM.
#else
_TCHAR *fname = _T("user");							// user.dll		// 01/21/06 AM.
#endif

#endif

_stprintf(buf, _T("%s%cbin%c%s.dll"),									// 01/21/06 AM.
											appdir,DIR_CH,DIR_CH,fname);	// 01/21/06 AM.
std::_t_strstream gerrStr;
gerrStr << _T("[Loading user project: ") << buf << _T("]") << std::ends;		// 01/17/02 AM.
errOut(&gerrStr,false);
//*gout << "[Loading user project: " << buf << "]" << std::endl;		// 02/18/02 AM.

#ifndef LINUX
hdll_ = load_dll(buf);
#endif


// MOVED LOAD OF RUN.DLL TO make_analyzer.							// 07/05/00 AM.

// Register algorithms!														// 12/03/98 AM.
// Just to test.
Algo *algo;
algo = new Tok();		Algo::regAlgo(*algo);
algo = new Line();	Algo::regAlgo(*algo);
algo = new Pat();		Algo::regAlgo(*algo);
algo = new Intern();	Algo::regAlgo(*algo);
algo = new Rec();		Algo::regAlgo(*algo);							// 11/08/99 AM.

// REGISTER PRE, CHECK, AND POST ACTIONS!
// Export the register functions so user can register new objects also!
// What about registering semantic objects......

////////////////
// INITIALIZE
////////////////

s_time = clock();																// 08/28/02 AM.

// Use global data structures, by default.							// 08/28/02 AM.
// Note: User can install private versions, if he dares.			// 08/28/02 AM.
if (!vtrun_)	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
	{
	std::_t_strstream gerrStr;						// 08/28/02 AM.
	gerrStr << _T("[NLP: Error. VTRun not initialized.]") << std::ends;	// 08/28/02 AM.
	errOut(&gerrStr,false);																// 08/28/02 AM.
	resetErr(ferr, serr);													// 08/28/02 AM.
	resetDbg(sdbg);															// 08/28/02 AM.
	nlp_clean(0);																// 08/28/02 AM.
	return;																		// 08/28/02 AM.
	}

#ifdef OLD_030625_
htab_   = (Htab *)vtrun_->htab_;	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
stab_   = (Stab *)vtrun_->stab_;	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
#endif

// Now current analyzer instance gets a LOCAL hash and string table.
stab_ = new Stab();															// 06/25/03 AM.
htab_ = new Htab(stab_);													// 06/25/03 AM.


// rfa, rfb, htfunc for builtins belong to VTRun.					// 08/28/02 AM.

#ifdef OLD_020828_
// Create string table and hashed symbol table to be used globally.
stab_ = new Stab();															// 12/14/98 AM.
htab_ = new Htab(stab_);													// 12/14/98 AM.

Htab::setGstab(stab_);			// Set the default string table.

// Build a hash table for NLP++ builtin functions.					// 12/20/01 AM.
htfunc_ = Ifunc::makeHtab(nlpFuncs);									// 12/20/01 AM.

////// DEFINE RFA.
// Define the rules-file analyzer!
// This will be an internal representation constructed by hand,
// that performs the parsing and internalization of the rules
// files themselves.  Call it the RF analyzer, or RFA.
// Note: Loading RFA,RFB even in compiled analyzer mode.			// 07/03/00 AM.
// (Just in case they're needed anywhere.)
s_time = clock();																// 12/28/98 AM.

rfa_ = new Ana();
rfa_->setNLP(this);															// 12/21/01 AM.

rfa_->setVerbose(false);													// 02/21/02 AM.
//rfa_->setVerbose(true);	// PRINT RULES...							// 02/21/02 AM.

_TCHAR rfaout[MAXSTR];
_stprintf(rfaout, _T("%s%crfa.log"), outdir, DIR_CH);						// 03/10/99 AM.
fileOut(rfaout, /*DU*/ fout, sout);

// Moving error stream up from here.									// 01/17/02 AM.

RFA::make_rfa(*rfa_);			// Build the rules file analyzer.

//if (develop)
resetOut(fout, sout);

rfa_->setHtab(htab_);			// Use the global hash table.		// 11/19/98 AM.

e_time = clock();																// 12/28/98 AM.
std::_t_strstream gerrStr1;
gerrStr1 << _T("[Build RFA time=")
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC
	  << _T(" sec]") << std::ends;
errOut(&gerrStr1,false);
s_time = e_time;

// 11/04/99 AM. TESTING RFA GENERATION TO FILES.
// Commenting this out.											// OPT	// 02/21/02 AM.
#ifdef GENRFA_
_stprintf(tmp, _T("%s%crfa%c%s"), appdir,DIR_CH,DIR_CH,					// 03/08/00 AM.
			SPECDIRNAME);														// 05/14/00 AM.
if (!dir_exists(tmp))
	make_dir(tmp);
rfa_->genAna(tmp);
#endif
#endif

#ifdef OLD_020828_
// MAKE THE RFB ANALYZER.													// 11/05/99 AM.
// RFA is used to make RFB, which then replaces RFA!
rfb_ = (Ana *)make_rfb(appdir,											// 08/27/02 AM.
			rfbspecdir,															// 12/08/99 AM.
			develop,
			silent,
			(void *)rfa_,														// 08/27/02 AM.
			(void *)htab_														// 08/27/02 AM.
			);
if (!rfb_)																		// 08/27/02 AM.
	ok = false;																	// 08/27/02 AM.
#endif

e_time = clock();																// 11/05/99 AM.

#ifdef GEODUCK_
if (!ok && gui_.IsStatus(GUI_STATUS_ANALYZING))						// 07/19/00 AM.
	{
	_TCHAR displayBuf[1001];													// 07/19/00 AM.
	_stprintf(displayBuf, _T("[ERROR IN ANALYZER INIT.]"));				// 07/19/00 AM.
	gui_.SendMessage(_T("Analyzer init:"),displayBuf);					// 07/19/00 AM.
	}
#else
if (!ok)																			// 07/06/01 AM.
	{
	std::_t_strstream gerrStr3;						// 07/06/01 AM.
	gerrStr3 << _T("[ERROR IN ANALYZER INIT.]") << std::ends;				// 07/06/01 AM.
	errOut(&gerrStr3,false);																// 07/06/01 AM.
	}
#endif

#ifdef DIAGNOSTIC_
std::_t_strstream gerrStr2;
gerrStr2 << _T("[Build RFB time=")											// 11/05/99 AM.
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC
	  << _T(" sec]") << std::ends;
errOut(&gerrStr2,false);
#endif
s_time = e_time;																// 11/05/99 AM.

resetErr(ferr, serr);														// 03/22/99 AM.
resetDbg(sdbg);																// 02/21/02 AM.
}


/********************************************
* FN:		MAKE_RFB
* CR:		11/05/99 AM.
* SUBJ:	Create RFB analyzer using RFA analyzer.
* NOTE:	RFB is built by using an analyzer sequence and rules files that
*			define a more complete rules language than represented by the
*			hard-wired RFA.
*			12/08/99 AM. RFB spec dir and RFB build dir can differ.
********************************************/

void *NLP::make_rfb(
	_TCHAR *appdir,
	_TCHAR *rfbspecdir,			// 12/08/99 AM.
	bool develop,				// If running in development mode.
	bool silent,																// 06/16/02 AM.
	void *rfa,																	// 08/27/02 AM.
	void *htab																	// 08/27/02 AM.
	)
{
_TCHAR sequence[MAXSTR];
_TCHAR outdir[MAXSTR];		// Intermed files.

std::_t_ostream *sout;
std::_t_ofstream *fout;
bool verbose = false;

if (!appdir || !*appdir)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[make_rfb: Given no appdir.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (!rfbspecdir || !*rfbspecdir)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[make_rfb: Given no RFB spec dir.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

_stprintf(sequence, _T("%s%canalyzer.seq"), rfbspecdir,					// 12/08/99 AM.
						DIR_CH);	// 03/08/00 AM.

// IF THERE IS NO SEQUENCE FILE, DON'T BUILD RFB.  RELY ON THE INTERNAL
// RFA.  Note: The analyzer that builds the RFB itself is the main one
// that doesn't use the RFB!												// 11/05/99 AM.
if (!file_exists(sequence))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[File not found=") << sequence << _T("]")				// 03/08/00 AM.
			  << _T("\n")
			  << _T("[No RFB.  Using the RFA for parsing rules.]")
			  << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

_stprintf(outdir, _T("%s%crfb%coutput"), appdir, DIR_CH,DIR_CH);		// 03/08/00 AM.
if (!dir_exists(outdir))
   {
#ifdef DIAGNOSTIC_
	std::_t_strstream gerrStr;
	gerrStr << _T("[No RFB output dir=") << outdir << _T("]") << std::ends;	// 03/08/00 AM.
	errOut(&gerrStr,false);
#endif
	make_dir(outdir);
	}

std::_t_ostream *serr;
std::_t_ofstream *ferr;
std::_t_ostream *sdbg=0;																// 02/21/02 AM.
_TCHAR errout[1024];
//if (!silent)																	// 06/16/02 AM.
	{
	_stprintf(errout, _T("%s%cmake_rfb.log"), outdir, DIR_CH);			// 03/08/00 AM.
	fileErr(errout, /*DU*/ ferr, serr);
//	fileDbg(outdir,sdbg);													// 02/21/02 AM.
	}


////// SET UP RFB ANALYZER.
Ana *rfb = new Ana();														// 08/27/02 AM.
//rfb->setNLP(this);															// 12/21/01 AM.

rfb->setSeqfile(sequence);	// Analyzer specification.
rfb->setHtab((Htab*)htab);	// Use the global hash table.			// 08/27/02 AM.

rfb->setSpecdir(rfbspecdir);	// Where the RFB definition resides.

// At this point, appdir is dummied up as c:\temp, yuck.	// 04/03/09 AM.

/////////////////////////////////////////////////
// GET ANALYZER DEFINITION ("Specs" or specifications)
/////////////////////////////////////////////////
// Read and parse analyzer sequence file.
// Use the analyzer sequence to read and parse pass files.
// Could condense into an ana.parseSeqfile(sequence) call.

_TCHAR defout[1024];
_stprintf(defout, _T("%s%cdef_rfb.log"), outdir, DIR_CH);
fileOut(defout, /*DU*/ fout, sout);

if (!(rfb->readSeqfile()))		// Read sequence file into buffer.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Couldn't read RFB sequence file.]")
			  << _T("\n")
			  << _T("[file=") << sequence << _T("]")
			  << std::ends;
	errOut(&gerrStr,false);
	resetOut(fout, sout);										// FIX	// 03/09/00 AM.
	resetErr(ferr, serr);										// FIX	// 03/09/00 AM.
//	resetDbg(sdbg);															// 02/21/02 AM.
	delete rfb;
	return 0;
	}

if (develop)
	{
	*gout << _T("Sequence file is:\n");
	*gout << rfb->getSeqbuf() << std::endl;
	}

// Setup flags (verboseness, timing, etc) for parsing the rules files.
enum Eanaconf conf = conf_ZILCH;
if (develop)
	conf = conf_RFA;

Eana eana(conf);
eana.setFtimepass(false);													// 02/21/02 AM.
eana.setFtimesum(false);													// 02/21/02 AM.
eana.setFintern(true);														// 12/20/01 AM.
eana.setFreadrfb(true);														// 12/20/01 AM.
//eana.setFlogfiles(true);

// Parse the sequence buffer.
rfb->parseSeq(false);

// Parse and internalize the pass files.
// USE THE RFA HERE.
if (!rfb->internSeq(*((Ana*)rfa), &eana,	// Asynchrony.			// 12/11/99 AM.
										0))  // TRYING WITH NO NLP			// 08/27/02 AM.
//										this))									// 12/20/01 AM.
	{
	resetOut(fout, sout);
	resetErr(ferr, serr);													// 03/22/99 AM.
//	resetDbg(sdbg);															// 02/21/02 AM.
	delete rfb;
	return 0;
	}

// Number the passes in the sequence.
// (Opt: Could do this while building the passes.)

// Need a proper way to number sequence of RFB, any analyzer....
//Aseq::numberSeq(this);

// Pretty-print the RFB analyzer sequence.
if (develop)
	rfb->prettySeq();

resetOut(fout, sout);
resetErr(ferr, serr);
//resetDbg(sdbg);																// 02/21/02 AM.

return (void *)rfb;
}


/********************************************
* FN:		MAKE_ANALYZER
* CR:		12/14/98 AM.
* SUBJ:	Create analyzer from sequence and rules files.
* NOTE:	11/11/00 AM. If the sequence is in the kb and some flag is set,
*			then read the sequence from the kb.  If main, execute only that...
********************************************/

bool NLP::make_analyzer(
	_TCHAR *sequence,
	_TCHAR *appdir,
	bool develop,																// 12/25/98 AM.
	bool silent,			// Silent run mode.							// 06/16/02 AM.
	_TCHAR *outdir,			// Intermed files.							// 03/10/99 AM.
	bool compile,			// Compile while loading.					// 05/10/00 AM.
	bool compiled,			// If running compiled analyzer.			// 07/03/00 AM.
	bool fromkb				// Get sequence from kb.					// 11/11/00 AM.
	)
{
clock_t   s_time, e_time;
std::_t_ostream *sout;
std::_t_ofstream *fout;
bool verbose = false;
bool ok;

s_time = clock();																// 12/28/98 AM.

if (!appdir || !*appdir)													// 03/10/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[make_analyzer: Given no appdir.]") << std::ends;
	errOut(&gerrStr,false);
	return false;																// 06/15/99 AM.
	}

_TCHAR tmp[MAXSTR];
if (!outdir)																	// 03/10/99 AM.
	{
	_stprintf(tmp, _T("%s%coutput"), appdir, DIR_CH);
	outdir = tmp;
	}

if (!dir_exists(outdir))
	make_dir(outdir);

std::_t_ostream *serr=0;					// 03/22/99 AM.						// 06/16/02 AM.
std::_t_ofstream *ferr=0;					// 03/22/99 AM.						// 06/16/02 AM.
std::_t_ostream *sdbg=0;																// 02/21/02 AM.
_TCHAR outd[MAXSTR];															// 02/21/02 AM.
_stprintf(outd,_T("%s%c%s"), appdir,DIR_CH,_T("logs"));						// 02/21/02 AM.
_TCHAR errout[MAXSTR];															// 03/22/99 AM.
//if (!silent)																	// 06/16/02 AM.
	{
	_stprintf(errout, _T("%s%c%s%cmake_ana.log"),							// 05/19/01 AM.
			appdir,DIR_CH, _T("logs"),DIR_CH);								// 05/19/01 AM.
	//sprintf_s(errout, "%s%cmake_ana.log", outdir,					// 05/19/01 AM.
	//				DIR_CH);														// 05/19/01 AM.
	fileErr(errout, /*DU*/ ferr, serr);
	fileDbg(outd,sdbg);														// 02/21/02 AM.
	}

std::_t_strstream gerrStr;														// 02/25/05 AM.
gerrStr << _T("[Date: ") << today() << _T("]") << std::ends;			// 05/26/01 AM.
errOut(&gerrStr,false,0,0); 												// 02/25/05 AM.

////// SET UP TEXT ANALYZER.
ana_ = new Ana();																// 12/14/98 AM.
ana_->setNLP(this);															// 12/21/01 AM.

ana_->setSeqfile(sequence);	// Analyzer sequence file = analyzer specification.

// For now, getting global hash table from env.						// 06/25/03 AM.
// Todo: Each analyzer def gets its own hash table.				// 06/25/03 AM.
//ana_->setHtab(htab_);			// Use the global hash table.		// 11/19/98 AM.
ana_->setHtab((Htab *)vtrun_->htab_);	// 06/25/03 AM.	// [DEGLOB]	// 10/15/20 AM.

// 12/03/98 AM. Using the appdir for finding the analyzer specification also.
_TCHAR specdir[MAXSTR];
_stprintf(specdir, _T("%s%c%s"), appdir,DIR_CH, SPECDIRNAME);

ana_->setSpecdir(specdir);	// Where the app definition resides	// 12/03/98 AM.
//ana_->setDatadir(datadir_);	// Data directory.					// 12/08/99 AM.
ana_->setCG(cg_);					// Convenience ptr to KB.			// 02/15/00 AM.
ana_->setAppdir(appdir);

/////////////////////////////////////////////////
// GET ANALYZER DEFINITION ("Specs" or specifications)
/////////////////////////////////////////////////
// Read and parse analyzer sequence file.
// Use the analyzer sequence to read and parse pass files.
// Could condense into an ana.parseSeqfile(sequence) call.

//if (develop)																	// 12/26/98 AM.
	{
	_TCHAR defout[MAXMSG];
	_stprintf(defout, _T("%s%cdef.log"), outdir,								// 03/10/99 AM.
						DIR_CH);													// 03/08/00 AM.
	fileOut(defout, /*DU*/ fout, sout);
	}

if (compiled &&	// Running compiled ana and						// 07/03/00 AM.
	 !compile)		// not generating code for ana.					// 07/03/00 AM.
	{
	resetOut(fout, sout);
	ok = load_compiled(appdir);								// FIX.	// 09/13/00 AM.


	// Perform user-defined initializations in USER Project.		// 07/28/03 AM.
	user_ini();																	// 07/28/03 AM.

	std::_t_strstream gerrStr;
	gerrStr << _T("[Build analyzer time= (running compiled)]") << std::ends;
	errOut(&gerrStr,false,0,0);
	resetErr(ferr, serr);
	resetDbg(sdbg);															// 02/21/02 AM.
	return ok;																	// 09/13/00 AM.
	}

// GET ANALYZER SEQUENCE FROM KB IF PRESENT. ELSE FROM FILE.
bool gotana = false;															// 11/11/00 AM.
if (fromkb)																		// 11/11/00 AM.
	{
	if (!(gotana = ana_->kbgetSeq()))									// 11/11/00 AM.
		;		// Message is given in kbgetSeq.							// 08/01 01 AM.
	else
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Read analyzer sequence from kb.]") << std::ends;	// 11/14/00 AM.
		errOut(&gerrStr,false,0,0);
		}
	}

if (!gotana)	// Haven't got ana sequence yet.						// 11/11/00 AM.
	{
	if (!(ana_->readSeqfile()))		// Read sequence file into buffer.
		return false;															// 06/15/99 AM.

	// Parse the sequence buffer.
	ana_->parseSeq(false);													// 11/11/00 AM.
	}

if (develop)														// OPT	// 10/12/99 AM.
	{
	*gout << _T("Sequence file is:\n");
	*gout << str(ana_->getSeqbuf()) << std::endl;							// 11/12/00 AM.
	}

// Setup flags (verboseness, timing, etc) for parsing the rules files.
enum Eanaconf conf = conf_ZILCH;											// 10/13/99 AM.
if (develop)																	// 10/13/99 AM.
	conf = conf_INTERN;														// 12/20/01 AM.

Eana eana(conf);																// 10/13/99 AM.
if (!silent)																	// 06/16/02 AM.
	{
	eana.setFtimepass(true);												// 10/21/99 AM.
	}
eana.setFintern(true);														// 12/20/01 AM.
eana.setFcompile(compile);													// 05/10/00 AM.

Gen *gen = 0;																	// 05/10/00 AM.
_TCHAR buf[MAXSTR];
if (compile)																	// 05/10/00 AM.
	{
	// Build a gen object to control code gen and compilation.

	// Set up an output file for code generation.
	_stprintf(buf, _T("%s%c%s%canalyzer.cpp"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *fcode = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%canalyzer.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *fhead = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%cdata.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *fdata = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%cauxiliary.cpp"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *faux = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);

	// 06/14/00 AM.
	_stprintf(buf, _T("%s%c%s%cehash.cpp"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *ehash = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%cehead.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *ehead = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%cedata.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *edata = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);

	// 06/17/00 AM.
	_stprintf(buf, _T("%s%c%s%crhash.cpp"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *rhash = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%crhead.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *rhead = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%crchain.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *rchain = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);
	_stprintf(buf, _T("%s%c%s%crdata.h"),
				ana_->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH);
	std::_t_ofstream *rdata = new std::_t_ofstream(TCHAR2CA(buf), std::ios::out);

	gen = new Gen(_T(""), fcode,fhead,fdata,								// 05/10/00 AM.
							faux);												// 06/05/00 AM.
	gen->setNLP(this);														// 12/22/01 AM.
	gen->setEhash(ehash);													// 06/14/00 AM.
	gen->setEhead(ehead);													// 06/14/00 AM.
	gen->setEdata(edata);													// 06/14/00 AM.

	gen->setRhash(rhash);													// 06/17/00 AM.
	gen->setRhead(rhead);													// 06/17/00 AM.
	gen->setRchain(rchain);													// 06/17/00 AM.
	gen->setRdata(rdata);													// 06/17/00 AM.

	// Generate some file headers.
	gen->writeHeaders();
	}

// Moving this line of code higher up.									// 11/11/00 AM.
// Parse the sequence buffer.
// ana_->parseSeq(false);													// 11/11/00 AM.

// MEMLEAK to here.	// 12/17/03 AM.

// GENERATE CODE FOR ANALYZER DURING LOAD.
if (compile)																	// 05/10/00 AM.
	Seqn::gen(ana_->getSeqlist(), gen);	// Gen code.				// 05/10/00 AM.


// Parse and internalize the pass files.
// USE THE RFB OR RFA HERE.
ok = true;		// Asynchrony.												// 12/11/99 AM.
//if (rfb_)																		// 11/05/99 AM.
if (vtrun_->rfb_)	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
	{
	// RESET APPDIR TO CURRENT ANALYZER'S.	// 04/03/09 AM.
	// Compiling analyzer needs appdir.		// 04/03/09 AM.
	((Ana *)vtrun_->rfb_)->setAppdir(ana_->getAppdir());	// 04/03/09 AM.	// [DEGLOB]	// 10/15/20 AM.
	((Ana *)vtrun_->rfb_)->setGen(gen);	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
//	rfb_->setGen(gen);		// Tell RFB how to compile.			// 05/10/00 AM.
	ok = ana_->internSeq(
//								*rfb_,
								*((Ana *)vtrun_->rfb_),	// 08/28/02 AM. // [DEGLOB]	// 10/15/20 AM.
										&eana,									// 11/05/99 AM.
								vtrun_->htfunc_); // 08/28/02 AM. // [DEGLOB]	// 10/15/20 AM.
//										this);									// 12/20/01 AM.
	}
else if (vtrun_->rfa_)	// 01/10/06 AM.	// [DEGLOB]	// 10/15/20 AM.
	ok = ana_->internSeq(
//								*rfa_,
								*((Ana *)vtrun_->rfa_),	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
										&eana,									// 01/13/99 AM.
								vtrun_->htfunc_);	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
//										this);									// 12/20/01 AM.
else																				// 01/10/06 AM.
	ok = false;																	// 01/10/06 AM.

// Finish out code generation.											// 05/16/00 AM.
// Close files, reset counters, etc.									// 05/16/00 AM.
if (compile && gen)												// FIX.	// 06/07/00 AM.
	gen->reset();																// 05/16/00 AM.

// IF GEN'ING CODE, THEN ALSO COMPILE IT.								// 07/05/00 AM.
if (compile																		// 07/05/00 AM.
    && ok)																		// 05/26/01 AM.
	{
	_TCHAR cmd[MAXSTR];
	cmd[0] = '\0';
#ifndef LINUX
// Compile debug or release version appropriately.		// FIX	// 02/14/02 AM.
#ifdef _DEBUG
	_stprintf(cmd,																// 07/05/00 AM.
		_T("msdev %s%crun%crun.dsp /MAKE \"run - Win32 Debug\" /REBUILD"),
		appdir, DIR_CH,DIR_CH);
//	_tsystem(cmd);																// 07/05/00 AM.
	run_silent(cmd);			// system() w/o DOC CMD PROMPT POPUP.	// 09/15/08 AM.
#else
	_stprintf(cmd,																// 02/14/02 AM.
		_T("msdev %s%crun%crun.dsp /MAKE \"run - Win32 Release\" /REBUILD"),
		appdir, DIR_CH,DIR_CH);
//	_tsystem(cmd);																// 02/14/02 AM.
	run_silent(cmd);			// system() w/o DOC CMD PROMPT POPUP.	// 09/15/08 AM.
#endif
#endif
	}

// MOVED FROM init() to here.												// 07/05/00 AM.
// LOAD COMPILED RUNTIME VERSION OF ANALYZER.						// 05/14/00 AM.
if (compiled)		// User wants compiled ana.						// 07/02/00 AM.
	load_compiled(appdir);													// 09/13/00 AM.

if (!ok)		// Asynchrony.													// 12/11/99 AM.
	{
// Commented out for Dave.													// 08/15/00 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Errors in loading analyzer.]") << std::ends;
	errOut(&gerrStr,false,0,0);

	resetOut(fout, sout);													// 12/11/99 AM.
	resetErr(ferr, serr);													// 12/11/99 AM.
	resetDbg(sdbg);															// 02/21/02 AM.
	return false;
	}

// Number the passes in the sequence.									// 10/07/99 AM.
// (Opt: Could do this while building the passes.)
Aseq::numberSeq(this);														// 10/07/99 AM.

// Pretty-print the analyzer sequence.
if (develop)														// OPT	// 10/12/99 AM.
	ana_->prettySeq();											// OPT	// 10/12/99 AM.

resetOut(fout, sout);

// Perform user-defined initializations in USER Project.			// 01/23/02 AM.
user_ini();																		// 01/23/02 AM.

e_time = clock();																// 12/28/98 AM.
std::_t_strstream gerrStr1;
gerrStr1 << _T("[Build analyzer time=")
	  << (double)(e_time - s_time)/CLOCKS_PER_SEC
	  << _T(" sec]") << std::ends;
errOut(&gerrStr1,false,0,0);
s_time = e_time;

resetErr(ferr, serr);														// 03/22/99 AM.
resetDbg(sdbg);																// 02/21/02 AM.
return true;																	// 06/15/99 AM.
}


/********************************************
* FN:		LOAD_COMPILED
* CR:		09/13/00 AM.
* SUBJ:	Load compiled analyzer.
* NOTE:	Modularity fn.
********************************************/

bool NLP::load_compiled(
	_TCHAR *appdir
	)
{
#ifdef RUNEMBED_
return true;
#endif

_TCHAR buf[MAXSTR];

// LOAD COMPILED RUNTIME VERSION OF ANALYZER.						// 05/14/00 AM.
#ifndef LINUX
hrundll_ = 0;

#ifdef _DEBUG

// Moving and renaming all RUN.DLL (run.dll) libraries.			// 01/23/06 AM.
#ifdef UNICODE
_TCHAR *fname = _T("rundu");							// rundu.dll	// 01/23/06 AM.
#else
_TCHAR *fname = _T("rund");							// rund.dll		// 01/23/06 AM.
#endif

#else	// NOT DEBUG

#ifdef UNICODE
_TCHAR *fname = _T("runu");							// runu.dll		// 01/23/06 AM.
#else
_TCHAR *fname = _T("run");								// run.dll		// 01/23/06 AM.
#endif

#endif

_stprintf(buf, _T("%s%cbin%c%s.dll"),									// 01/23/06 AM.
						appdir,DIR_CH,DIR_CH,fname);						// 01/23/06 AM.
std::_t_strstream gerrStr;
gerrStr << _T("[Loading compiled analyzer ")							// 01/23/06 AM.
												<< buf << _T("]") << std::ends;	// 01/23/06 AM.
errOut(&gerrStr,false);

hrundll_ = load_dll(buf);
if (!hrundll_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Error: Couldn't load compiled analyzer.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
#endif
return true;
}


/********************************************
* FN:		INTERNPASS
* CR:		09/01/99 AM.
* SUBJ:	Intern a single pass of analyzer.
* NOTE:	Used by RUG rule generation stuff.
********************************************/

bool NLP::internPass(
	APASS *seqn,
	Eana  *erfa				// 10/13/99 AM.
	)
{
#ifdef GEODUCK_
if (!seqn || gui_.IsMessage(GUI_MESSAGE_ABORT))						// 11/29/99 DD.
	return false;
#else
if (!seqn)																		// 07/06/01 AM.
	return false;																// 07/06/01 AM.
#endif

Seqn *pass;
pass = ((Delt<Seqn> *) seqn)->getData();

#ifdef GEODUCK_
if (gui_.IsStatus(GUI_STATUS_GENERATING))								// 11/18/99 DD.
	{
	_TCHAR buf[1001];															// 11/18/99 DD.
	//sprintf_s(buf,"(%d) %s",pass->getPassnum(),						// 11/18/99 DD.
	//							pass->getRulesfilename());					// 11/18/99 DD.
	_stprintf(buf,_T("%3d %-10s %s"),											// 11/18/99 DD.
			pass->getPassnum(),
			pass->getAlgoname(),
			pass->getRulesfilename()
			);
	gui_.SendMessage(_T("Generating rules:"),buf);												// 11/18/99 DD.
	}
#endif

// Handle asynchrony by checking return.								// 12/11/99 AM.
if (vtrun_->rfb_)	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
//if (rfb_)																		// 11/05/99 AM.
	return ana_->internPass(pass,
									*((Ana *)vtrun_->rfb_),	// 08/28/02 AM. // [DEGLOB]	// 10/15/20 AM.
//									*rfb_,
									erfa);										// 11/05/99 AM.
else
	return ana_->internPass(pass,
									*((Ana *)vtrun_->rfa_),	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
//								   *rfa_,
									erfa);
//return true;																	// 12/11/99 AM.
}


/********************************************
* FN:		INITANALYZE
* CR:		05/13/99 AM.
* SUBJ:	Initialize the analysis of a single text.
* RET:	Return handle to current parse.
* NOTE:	Splitting up analysis of a single text to be able to step
*			through it one pass at a time.
*			02/06/00 AM. User can supply a buffer to parse instead of an
*			input file.  If len > 0, specifies length to be parsed.
********************************************/

Parse *NLP::initAnalyze(
	_TCHAR *input,
	_TCHAR *output,
	_TCHAR *appdir,
	Eana *eana,						// 10/13/99 AM.
	_TCHAR *outdir,
	_TCHAR *inbuf,		// Parse input buffer, if nonzero.			// 02/06/00 AM.
	long len,			// Length of input buf, if partial.			// 02/06/00 AM.
	std::_t_ostream *os,	// Rebinding of output stream.				// 05/11/02 AM.
	void *cbufv,		// Output buffer stream, if outlen > 0.	// 10/10/03 AM.
	long outlen,		// Output buffer max length.					// 05/11/02 AM.
	bool retain			// If retaining hash,sym tables.				// 05/21/09 AM.
	)
{
// BUG: cbufv is already an std::ostringstream at this point.	// 06/30/05 AM.
// WINDOWS:

#ifndef LINUX
std::_t_ostrstream *cbuf = (std::_t_ostrstream *) cbufv;			// 06/30/05 AM.
#else
// BUFFER/ARRAY STREAM ALREADY CREATED!!!	// 09/28/19 AM.
std::ostringstream *cbuf = (std::ostringstream *) cbufv;			// 06/30/05 AM.	// 09/28/19 AM.

// cerr << "initAnalyze in: " << std::ends;
// cerr << "cbufv: *" << cbuf->str() << "*" << std::endl;
#endif

// ANCIENT STUFF HERE...	// 09/28/19 AM.
#ifdef UNICODE
//std::_t_ostrstream *cbuf = new std::_t_ostrstream((char*)cbufv,std::ios::out);			// 03/29/05 AM.
#else
//std::_t_ostrstream *cbuf = new std::_t_ostrstream((char*)cbufv,outlen,std::ios::out);	// 03/29/05 AM.
#endif
//std::_t_ostrstream *cbuf = (std::_t_ostrstream *) cbufv;								// 10/10/03 AM.

std::_t_ostream* sout = 0;
std::_t_ofstream* fout = 0;
std::_t_ostream *sdbg=0;																// 02/21/02 AM.

if (!appdir || !*appdir)													// 03/10/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[initAnalyze: Given no appdir.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR tmp[MAXSTR];
if (!outdir || !*outdir)													// 03/10/99 AM.
	{
	_stprintf(tmp, _T("%s%coutput"), appdir, DIR_CH);
	outdir = tmp;
	}

if (!dir_exists(outdir))
	make_dir(outdir);

// Setting up ERROR OUTPUT FILE.											// 03/22/99 AM.
_TCHAR errout[MAXMSG];															// 03/22/99 AM.

//if (!eana->getFsilent())													// 06/16/02 AM.
	{
	_stprintf(errout, _T("%s%cerr.log"), outdir,								// 03/22/99 AM.
				DIR_CH);															// 03/08/00 AM.
	fileErr(errout, /*DU*/ fout, sout);

	// Set up a debug,verbose,timing,etc. log file.					// 02/21/02 AM.
	fileDbg(outdir,sdbg);													// 02/21/02 AM.
	}


// Now current analyzer instance gets a LOCAL hash and string table.
// Caller may also fill in with global, or decide not to delete, etc.
if (!stab_)																		// 06/25/03 AM.
	stab_ = new Stab();														// 06/25/03 AM.
if (!htab_)																		// 06/25/03 AM.
	htab_ = new Htab(stab_);												// 06/25/03 AM.


Parse *parse;					// The current text analysis instance.
parse = new Parse();															// 05/13/99 AM.
parse->nlp_ = this;															// 06/25/03 AM.
parse->htab_ = this->htab_;												// 06/25/03 AM.

parse->setEana(eana);														// 10/13/99 AM.

parse->setFout(fout);														// 05/13/99 AM.
parse->setSout(sout);														// 05/13/99 AM.
parse->setAna(ana_);			// Analyzer that the parse will use.
parse->setAppdir(appdir);	// Where the app runs from.			// 12/03/98 AM.
parse->setAnasdir(appdir);
parse->setKBdir(appdir);
parse->setOutdir(outdir);	// Intermed output files.				// 03/10/99 AM.
#ifndef LINUX
parse->setHdll(hdll_);		// Handle to user.dll.					// 01/29/99 AM.
#endif

// Set up output to buffer.												// 05/11/02 AM.
parse->setCbuf(cbuf);														// 05/11/02 AM.
parse->setCbufmax(outlen);													// 05/11/02 AM.

// 12/14/98 AM. Moving these here, to more cleanly separate initialization
// and execution of an analyzer.
if (input)	// 09/25/20 AM.
	parse->setInput(input);		// Input file for the parse.
if (output)	// 09/25/20 AM.
	parse->setOutput(output);	// Output file for the parse.

//parse->setVerbose(false);												// 10/13/99 AM.
parse->setVerbose(eana->getFverbose());								// 10/13/99 AM.

// Get user or default setting for batch start.		// FIX		// 05/19/08 AM.
parse->setFbatchstart(fbatchstart_);					// FIX		// 05/19/08 AM.

if (!inbuf)		// If no supplied buffer.								// 02/06/00 AM.
	{
	if (!parse->readFile())				// READ INPUT FILE TO BUFFER.
		{
		// Abort analysis of empty text.						// FIX.	// 09/23/07 AM.
		cleanAnalyze(parse,retain);							// FIX.	// 09/23/07 AM.
		return 0;													// FIX.	// 09/23/07 AM.
		}
	}
											// (TODO: Check into file mapping.)
else				// Set up to parse given buffer.						// 02/06/00 AM.
	{
	// Copy buffer into parse object.
	parse->copyBuffer(inbuf, len);										// 02/06/00 AM.
	}

// New manager for cout ostream.											// 05/04/03 AM.
if (os)																			// 05/05/03 AM.
	parse->setCout(os);														// 05/04/03 AM.

parse->setLogfile(this->logfile_);	// VTLOG	// 05/06/13 AM.

//parse->setVerbose(verbose);												// 10/13/99 AM.
return parse;
}



/********************************************
* FN:		INITANALYZE (VARIANT)
* CR:		11/17/20 AM.
* SUBJ:	Initialize the analysis of a single text (with strstream io).
* RET:	Return handle to current parse.
* NOTE:	
********************************************/

Parse *NLP::initAnalyze(
	std::istringstream *iss,
	std::ostringstream *oss,
	_TCHAR *appdir,
	Eana *eana,
	_TCHAR *outdir,
	std::_t_ostream *os,	// Rebinding of output stream.
	bool retain		// If retaining hash,sym tables.
	)
{
// BUG: cbufv is already an std::ostringstream at this point.	// 06/30/05 AM.
// WINDOWS:

#ifndef LINUX
//std::_t_ostrstream *cbuf = (std::_t_ostrstream *) cbufv;
#else
// BUFFER/ARRAY STREAM ALREADY CREATED!!!	// 09/28/19 AM.
//std::ostringstream *cbuf = (std::ostringstream *) cbufv;			// 06/30/05 AM.	// 09/28/19 AM.

// cerr << "initAnalyze in: " << std::ends;
// cerr << "cbufv: *" << cbuf->str() << "*" << std::endl;
#endif

// ANCIENT STUFF HERE...	// 09/28/19 AM.
#ifdef UNICODE
//std::_t_ostrstream *cbuf = new std::_t_ostrstream((char*)cbufv,std::ios::out);			// 03/29/05 AM.
#else
//std::_t_ostrstream *cbuf = new std::_t_ostrstream((char*)cbufv,outlen,std::ios::out);	// 03/29/05 AM.
#endif
//std::_t_ostrstream *cbuf = (std::_t_ostrstream *) cbufv;								// 10/10/03 AM.

std::_t_ostream* sout = 0;
std::_t_ofstream* fout = 0;
std::_t_ostream *sdbg=0;																// 02/21/02 AM.

if (!appdir || !*appdir)													// 03/10/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[initAnalyze: Given no appdir.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR tmp[MAXSTR];
if (!outdir || !*outdir)													// 03/10/99 AM.
	{
	_stprintf(tmp, _T("%s%coutput"), appdir, DIR_CH);
	outdir = tmp;
	}

if (!dir_exists(outdir))
	make_dir(outdir);

// Setting up ERROR OUTPUT FILE.											// 03/22/99 AM.
_TCHAR errout[MAXMSG];															// 03/22/99 AM.

//if (!eana->getFsilent())													// 06/16/02 AM.
	{
	_stprintf(errout, _T("%s%cerr.log"), outdir,								// 03/22/99 AM.
				DIR_CH);															// 03/08/00 AM.
	fileErr(errout, /*DU*/ fout, sout);

	// Set up a debug,verbose,timing,etc. log file.					// 02/21/02 AM.
	fileDbg(outdir,sdbg);													// 02/21/02 AM.
	}


// Now current analyzer instance gets a LOCAL hash and string table.
// Caller may also fill in with global, or decide not to delete, etc.
if (!stab_)																		// 06/25/03 AM.
	stab_ = new Stab();														// 06/25/03 AM.
if (!htab_)																		// 06/25/03 AM.
	htab_ = new Htab(stab_);												// 06/25/03 AM.


Parse *parse;					// The current text analysis instance.
parse = new Parse();															// 05/13/99 AM.
parse->nlp_ = this;															// 06/25/03 AM.
parse->htab_ = this->htab_;												// 06/25/03 AM.

parse->setEana(eana);														// 10/13/99 AM.

parse->setFout(fout);														// 05/13/99 AM.
parse->setSout(sout);														// 05/13/99 AM.
parse->setAna(ana_);			// Analyzer that the parse will use.
parse->setAppdir(appdir);	// Where the app runs from.			// 12/03/98 AM.
parse->setOutdir(outdir);	// Intermed output files.				// 03/10/99 AM.
parse->setAnasdir(appdir);
parse->setKBdir(appdir);
#ifndef LINUX
parse->setHdll(hdll_);		// Handle to user.dll.					// 01/29/99 AM.
#endif

// Set up output to buffer.
#ifdef LINUX
parse->setCbuf((std::ostringstream*)oss);			// Try this.	// 11/17/20 AM.
#else
parse->setCbuf((std::_t_ostrstream*)oss);			// Try this.	// 11/17/20 AM.
#endif
//parse->setCbufmax(INFINITY);	// Try this.	// 11/17/20 AM.

//parse->setVerbose(false);												// 10/13/99 AM.
parse->setVerbose(eana->getFverbose());								// 10/13/99 AM.

// Get user or default setting for batch start.		// FIX		// 05/19/08 AM.
parse->setFbatchstart(fbatchstart_);					// FIX		// 05/19/08 AM.

// (TODO: Check into file mapping.)
// Set up to parse given buffer.						// 02/06/00 AM.

// Copy buffer into parse object.
std::string s = iss->str();
_TCHAR *param = new _TCHAR[s.size()+1];
param[s.size()] = 0;
std::copy(s.begin(),s.end(),param);
parse->copyBuffer(param, 0);	// Try this.	// 11/17/20 AM.

// New manager for cout ostream.											// 05/04/03 AM.
if (os)																			// 05/05/03 AM.
	parse->setCout(os);														// 05/04/03 AM.

parse->setLogfile(this->logfile_);	// VTLOG	// 05/06/13 AM.

//parse->setVerbose(verbose);												// 10/13/99 AM.
return parse;
}

/********************************************
* FN:		CLEANANALYZE
* CR:		05/13/99 AM.
* SUBJ:	Clean up the analysis of a single text.
* RET:	True if successful cleanup.
* NOTE:	Splitting up analysis of a single text to be able to step
*			through it one pass at a time.
********************************************/

bool NLP::cleanAnalyze(
	Parse *parse,
	bool retain		// If retaining hash and sym tables.			// 07/03/03 AM.
	)
{
std::_t_ostream *sout;
std::_t_ofstream *fout;
std::_t_ostream *sdbg=0;																// 02/21/02 AM.
if (!parse)
	return false;

sout = parse->getSout();
fout = parse->getFout();

#ifdef PERFORM_
//*fout << "SYSTEM HASH" << std::endl;
//Htab *htab = (Htab *)vtrun_->htab_;	// 06/24/03 AM.	// [DEGLOB]	// 10/15/20 AM.
//htab->pretty(fout);															// 06/24/03 AM.
//*fout << "\n\nLOCAL HASH" << std::endl;
//htab_->pretty(fout);
#endif

clock_t s_time = clock();								// 06/26/03 AM.

// Now hash table and string table are LOCAL.	// 06/25/03 AM.
// TRYING DELETE AFTER EVERY PARSE!!				// 06/25/03 AM.
if (!retain)												// 07/03/03 AM.
	{
	if (htab_)													// 06/25/03 AM.
		{
		delete htab_;											// 06/25/03 AM.
		htab_ = 0;												// 06/25/03 AM.
		}
	if (stab_)													// 06/25/03 AM.
		{
		delete stab_;											// 06/25/03 AM.
		stab_ = 0;												// 06/25/03 AM.
		}
	}

clock_t e_time = clock();														// 06/26/03 AM.
bool ftimepass = parse->eana_->getFtimepass();							// 06/26/03 AM.
double tot = (double)(e_time - s_time)/CLOCKS_PER_SEC;				// 06/26/03 AM.
if (ftimepass)																		// 06/26/03 AM.
	{
	std::_t_strstream gerrStr;														// 02/25/05 AM.
	gerrStr	<< _T("[Clean time: ") << tot << _T(" sec]") << std::ends;			// 06/26/03 AM.
	logOut(&gerrStr,false);														// 02/25/05 AM.
	}

if (sout && fout)
	{
	resetErr(fout, sout);
	}
if (dbgout_)																	// 02/21/02 AM.
	resetDbg(sdbg);															// 02/21/02 AM.
else
	return false;
if (parse)
	delete parse;
else
	return false;
return true;
}

/********************************************
* FN:		STEPEXECUTE
* CR:		05/17/99 AM.
* SUBJ:	Step through one pass of analyzer.
* NOTE:	Splitting up analysis of a single text to be able to step
*			through it one pass at a time.
********************************************/

bool NLP::stepExecute(							// 05/17/99 AM.
	Parse *parse,				// Current parse instance.
	APASS *seq,					// Current step in analyzer sequence.
	int num						// Pass number.
	)
{
return parse->stepExecute((Delt<Seqn> *) seq, num);
}


/********************************************
* FN:		PARTEXECUTE
* CR:		05/18/99 AM.
* SUBJ:	Analyzer passes from given pass to given pass.
* ASS:	Parse has been initialized.  Caller must clean up
*			afterward.  Assumes both passes are in the current
*			analyzer.
* NOTE:	Enables looking at parse trees in midstream by the rule
*			generator (rug).
********************************************/

bool NLP::partExecute(
	Parse *parse,				// Current parse instance.
	APASS *seq_s,				// Start pass.
	APASS *seq_e,				// End pass.
	/*DU*/
	int &num						// Start pass number.
	)
{
APASS *pass;

// Execute up to the end pass.
for (pass = seq_s; pass && pass != seq_e; pass = Aseq::nextPass(pass))
	{
	parse->stepExecute((Delt<Seqn> *) pass, num++);
	}

// Execute the end pass.
parse->stepExecute((Delt<Seqn> *) seq_e, num++);
return true;
}


/********************************************
* FN:		ANALYZE
* CR:		12/14/98 AM.
* SUBJ:	Analyze a single text.
* NOTE:	05/07/02 AM. Whereas the input comes from file or buffer,
*			the output can go to all three: file, buffer, and stream.
*			In NLP++, specify which output goes to which output conduit.
********************************************/

void NLP::analyze(
	_TCHAR *input,
	_TCHAR *output,
	_TCHAR *appdir,
	bool flogfiles,	// Changing the meaning of this.				// 10/13/99 AM.
	bool silent,		// Silent run mode.								// 06/16/02 AM.
	_TCHAR *outdir,		// Intermed files.								// 03/10/99 AM.
	_TCHAR *inbuf,		// If != 0, analyzing BUFFER					// 02/11/00 AM.
	long len,			// If != 0, then buffer length				// 02/11/00 AM.
	bool compiled,		// If running compiled analyzer.				// 07/05/00 AM.
	std::_t_ostream *os,		// Rebinding of output stream.				// 05/05/02 AM.
	_TCHAR *outbuf,		// Output buffer, if outlen > 0.				// 05/07/02 AM.
	long outlen,			// Output buffer max length.				// 05/07/02 AM.
	_TCHAR *datum			// Pass info to G("$datum").					// 03/13/03 AM.
	)
{
// NOTE: NLP++ function cout() now allows output to stream.		// 05/05/02 AM.
// NEED XP FIX.	// 01/12/03 AM.
//if (os)					// If supplied output stream,					// 05/05/02 AM.
//	cout = *os;			// Rebind standard output.						// 05/05/02 AM.

// To avoid any output string stream bugs, keeping this			// 05/11/02 AM.
// object here. (G++ bug reported way back.)							// 05/11/02 AM.
outbuf[0] = L'\0';
#ifdef LINUX
// How to return to outbuf.	// 10/26/06 AM.
//std::_t_ostrstream ocbuf;
//std::_t_ostrstream *pbuf = 0;
//pbuf = &ocbuf;
//std::ostringstream ocbuf(outbuf, outlen, std::ios::out);							// 05/11/02 AM.	// 09/28/19 AM.
std::ostringstream ocbuf(outbuf, std::ios_base::app);							// 09/28/19 AM.
std::ostringstream *pbuf = 0;														// 05/13/02 AM.	// 09/28/19 AM.
if (outbuf && outlen > 1)													// 05/13/02 AM.	// 09/28/19 AM.
	pbuf = &ocbuf;																// 05/13/02 AM.	// 09/28/19 AM.
#else
//std::_t_ostrstream ocbuf(outbuf, outlen, std::ios::out);							// 05/11/02 AM.
std::_t_ostrstream ocbuf(outbuf, std::ios::app);							// 09/28/19 AM.
std::_t_ostrstream *pbuf = 0;														// 05/13/02 AM.
if (outbuf && outlen > 1)													// 05/13/02 AM.
	pbuf = &ocbuf;																// 05/13/02 AM.
#endif

if (compiled)																	// 07/05/00 AM.
	{
	runAnalyzer(input,output,appdir,flogfiles,						// 07/05/00 AM.
														silent,					// 07/03/02 AM.
														outdir,
														inbuf,len,
														os, pbuf, outlen,		// 05/13/02 AM.
														datum						// 03/13/03 AM.
														);

#ifdef UNICODE
	// No Unicode analog to std::ostringstream, so copy buffer.				// 04/15/06 AM.
	long siz = ocbuf.str().size();											// 04/15/06 AM.
	if (siz >= outlen)	// Overflow.										// 04/15/06 AM.
		siz = outlen - 1;															// 04/15/06 AM.
	ocbuf.str().copy(outbuf,siz); // Hacked this together.			// 04/15/06 AM.
	outbuf[siz] = '\0';															// 04/15/06 AM.
#endif

	return;
	}

clock_t s_time = 0;
clock_t e_time;

if (flogfiles)																	// 02/21/02 AM.
	s_time = clock();															// 12/28/98 AM.

// Set up runtime config for current parse.							// 10/13/99 AM.
enum Eanaconf conf = conf_ZILCH;											// 06/16/02 AM.
if (!silent)														// FIX	// 07/02/02 AM.
	{
	if (flogfiles)																// 07/02/02 AM.
		conf = conf_DEV;														// 07/02/02 AM.
	else
		conf = conf_REG;														// 07/02/02 AM.
	}
Eana *eana = new Eana(conf);												// 06/16/02 AM.
eana->setFsilent(silent);													// 06/16/02 AM.

Parse *parse = 0;																// 05/13/99 AM.

if (!(parse = initAnalyze(input,output,appdir,eana,outdir,		// 10/13/99 AM.
									inbuf, len, 								// 02/11/00 AM.
									os, pbuf, outlen							// 05/11/02 AM.
									)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Analyze: Couldn't create parse instance.]") << std::ends;
	errOut(&gerrStr,false);
	delete eana;																// 07/23/01 AM.
	return;
	}

// Pass the batch start flag to analyzer (from VisualText).		// 10/19/00 AM.
// Used for running folders or other multi-file runs, as a
// handshake to say when the first file is being processed.
parse->setFbatchstart(fbatchstart_);									// 10/19/00 AM.



// Pass arbitrary information to the analyzer.						// 03/13/03 AM.
parse->setDatum(datum);														// 03/13/03 AM.

/////////////////////////////////////////////////
// EXECUTE THE ANALYZER
/////////////////////////////////////////////////
// Execute steps as described in analyzer sequence.
// Each pass traverses parse tree, applying rules and actions.


if (parse->getText())
	{
	parse->execute();					// PERFORM TEXT ANALYSIS.
	}

cleanAnalyze(parse);
delete eana;			// MOVED AFTER CLEANANALYZE.					// 07/19/03 AM.

// Set batch start to false after analysis of any file.			// 05/19/08 AM.
fbatchstart_ = false;										// FIX		// 05/19/08 AM.

if (pbuf)																		// 05/13/02 AM.
	*pbuf << std::ends;		// Terminate buffer.								// 05/11/02 AM.

#ifdef UNICODE
// No Unicode analog to std::ostringstream, so copy buffer.				// 04/14/06 AM.
long siz = ocbuf.str().size();											// 04/15/06 AM.
if (siz >= outlen)	// Overflow.										// 04/15/06 AM.
	siz = outlen - 1;															// 04/15/06 AM.
ocbuf.str().copy(outbuf,siz); // Hacked this together.			// 04/14/06 AM.
outbuf[siz] = '\0';															// 04/15/06 AM.
#endif

if (flogfiles)																	// 02/21/02 AM.
	{
	e_time = clock();
	std::_t_strstream gerrStr;
	gerrStr << _T("[Exec analyzer time=")
		  << (double) (e_time - s_time)/CLOCKS_PER_SEC
		  << _T(" sec]") << std::ends;
	errOut(&gerrStr,false);
	s_time = e_time;
	}

#ifdef PERFORM_
//*gout << "SYSTEM HASH" << std::endl;
//Htab *htab = (Htab *)vtrun_->htab_;	// 06/24/03 AM.	// [DEGLOB]	// 10/15/20 AM.
//htab->pretty(gout);															// 06/24/03 AM.
//*gout << "\n\nLOCAL HASH" << std::endl;
//htab_->pretty(gout);
#endif
}


/********************************************
* FN:		ANALYZE
* CR:		11/17/20 AM.
* SUBJ:	Analyze a single text. STRSTREAM VARIANT.
* NOTE:	Attempt to use cout() for strstream output within NLP++ analyzer.
********************************************/

void NLP::analyze(
	std::istringstream *iss,	// Input strstream.
	std::ostringstream *oss,	// Output strstream.
	_TCHAR *appdir,		// Directory holding analyzer.
	bool flogfiles,		// Changing the meaning of this.
	bool silent,		// Silent run mode.
	_TCHAR *outdir,		// Intermed files.
	bool compiled,		// If running compiled analyzer.
	std::_t_ostream *os,		// Rebinding of output stream.				// 05/05/02 AM.
	_TCHAR *datum		// Pass info to G("$datum").
	)
{
if (!iss || !oss)
	return;



if (compiled)																	// 07/05/00 AM.
	{
	runAnalyzer(iss,oss,appdir,flogfiles,silent,outdir,os,datum);	// 11/17/20 AM.
	return;
	}
#ifdef UNICODE
#endif

clock_t   s_time, e_time;

if (flogfiles)																	// 02/21/02 AM.
	s_time = clock();															// 12/28/98 AM.

// Set up runtime config for current parse.							// 10/13/99 AM.
enum Eanaconf conf = conf_ZILCH;											// 06/16/02 AM.
if (!silent)														// FIX	// 07/02/02 AM.
	{
	if (flogfiles)																// 07/02/02 AM.
		conf = conf_DEV;														// 07/02/02 AM.
	else
		conf = conf_REG;														// 07/02/02 AM.
	}
Eana *eana = new Eana(conf);												// 06/16/02 AM.
eana->setFsilent(silent);													// 06/16/02 AM.

Parse *parse = 0;																// 05/13/99 AM.

if (!(parse = initAnalyze(iss,oss,appdir,eana,outdir)))	// 11/17/20 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Analyze: Couldn't create parse instance.]") << std::ends;
	errOut(&gerrStr,false);
	delete eana;																// 07/23/01 AM.
	return;
	}

// Pass the batch start flag to analyzer (from VisualText).		// 10/19/00 AM.
// Used for running folders or other multi-file runs, as a
// handshake to say when the first file is being processed.
parse->setFbatchstart(fbatchstart_);									// 10/19/00 AM.



// Pass arbitrary information to the analyzer.						// 03/13/03 AM.
parse->setDatum(datum);														// 03/13/03 AM.

/////////////////////////////////////////////////
// EXECUTE THE ANALYZER
/////////////////////////////////////////////////
// Execute steps as described in analyzer sequence.
// Each pass traverses parse tree, applying rules and actions.


if (parse->getText())
	{
	parse->execute();					// PERFORM TEXT ANALYSIS.
	}

cleanAnalyze(parse);
delete eana;			// MOVED AFTER CLEANANALYZE.					// 07/19/03 AM.

// Set batch start to false after analysis of any file.			// 05/19/08 AM.
fbatchstart_ = false;										// FIX		// 05/19/08 AM.

// Terminate buffer.	// 11/17/20 AM.

#ifdef UNICODE
// No Unicode analog to std::ostringstream, so copy buffer.				// 04/14/06 AM.
long siz = ocbuf.str().size();											// 04/15/06 AM.
if (siz >= outlen)	// Overflow.										// 04/15/06 AM.
	siz = outlen - 1;															// 04/15/06 AM.
ocbuf.str().copy(outbuf,siz); // Hacked this together.			// 04/14/06 AM.
outbuf[siz] = '\0';															// 04/15/06 AM.
#endif

if (flogfiles)																	// 02/21/02 AM.
	{
	e_time = clock();
	std::_t_strstream gerrStr;
	gerrStr << _T("[Exec analyzer time=")
		  << (double) (e_time - s_time)/CLOCKS_PER_SEC
		  << _T(" sec]") << std::ends;
	errOut(&gerrStr,false);
	s_time = e_time;
	}

#ifdef PERFORM_
//*gout << "SYSTEM HASH" << std::endl;
//Htab *htab = (Htab *)vtrun_->htab_;	// 06/24/03 AM.	// [DEGLOB]	// 10/15/20 AM.
//htab->pretty(gout);															// 06/24/03 AM.
//*gout << "\n\nLOCAL HASH" << std::endl;
//htab_->pretty(gout);
#endif
}


/********************************************
* FN:		APIINITANALYZE
* CR:		07/23/01 AM.
* RET:	parse - Handle.  Internally = Parse object.
* SUBJ:	API call to initialize analysis of a text.
* NOTE:	To enable stepping through analyzer by caller.
*			Interpreted analyzer only.
********************************************/

void *NLP::apiInitAnalyze(
	_TCHAR *input,
	_TCHAR *output,
	_TCHAR *appdir,
	bool flogfiles,	// Changing the meaning of this.				// 10/13/99 AM.
	_TCHAR *outdir,		// Interned files.								// 03/10/99 AM.
	_TCHAR *inbuf,		// If != 0, analyzing BUFFER					// 02/11/00 AM.
	long len,				// If != 0, then buffer length			// 02/11/00 AM.
	std::_t_ostream *os,		// Rebinding of output stream.				// 05/13/02 AM.
	_TCHAR *outbuf,		// Output buffer, if outlen > 0.				// 05/13/02 AM.
	long outlen			// Output buffer max length.					// 05/13/02 AM.
	)
{
// NOTE: NLP++ function cout() now allows output to stream.		// 05/13/02 AM.
// NEED XP FIX.	// 01/12/03 AM.
//if (os)					// If supplied output stream,					// 05/13/02 AM.
//	cout = *os;			// Rebind standard output.						// 05/13/02 AM.

// Set up runtime config for current parse.							// 10/13/99 AM.
enum Eanaconf conf = conf_REG;								// FIX.	// 07/02/02 AM.
if (flogfiles)																	// 06/16/02 AM.
	conf = conf_DEV;															// 06/16/02 AM.
Eana *eana = new Eana(conf);												// 06/16/02 AM.
eana->setFlogfiles(flogfiles);											// 07/23/01 AM.

// To avoid any output string stream bugs, keeping this			// 05/11/02 AM.
// object here. (G++ bug reported way back.)							// 05/11/02 AM.
//std::_t_ostrstream ocbuf(outbuf, outlen, std::ios::out);							// 05/11/02 AM.
std::_t_ostrstream *pbuf = 0;														// 05/13/02 AM.
if (outbuf && outlen > 1)													// 05/13/02 AM.
//	pbuf = &ocbuf;																// 05/13/02 AM.
#ifdef UNICODE
	pbuf = new std::_t_ostrstream(outbuf,std::ios::out);						// 02/20/05 AM.
#elif LINUX
	pbuf = new std::_t_ostrstream(outbuf,std::ios_base::out);
#else
	pbuf = new std::_t_ostrstream(outbuf,std::ios::out);				// 02/20/05 AM.
#endif

Parse *parse = 0;																// 05/13/99 AM.

if (!(parse = initAnalyze(input,output,appdir,eana,outdir,		// 10/13/99 AM.
									inbuf, len,									// 02/11/00 AM.
									os, pbuf, outlen							// 05/13/02 AM.
									)))
	{
	delete eana;																// 07/23/01 AM.
	return 0;
	}

// Pass the batch start flag to analyzer (from VisualText).		// 10/19/00 AM.
// Used for running folders or other multi-file runs, as a
// handshake to say when the first file is being processed.
parse->setFbatchstart(fbatchstart_);									// 10/19/00 AM.

if (!parse->getText())	// No text to analyze.
	{
	delete eana;
//	cleanAnalyze(parse);														// 06/19/03 AM.
	return 0;
	}

// Set up for traversing sequence.
parse->setSeq(0);
parse->setCurrpass(0);

return (void *) parse;
}


/********************************************
* FN:		APISTEPANALYZE
* CR:		07/24/01 AM.
* RET:	ok - true if successfully analyzed next step.
*			False if done analyzing or if failed on pass.
* SUBJ:	API call to step to next pass in analysis of a text.
* NOTE:	To enable stepping through analyzer by caller.
*			Interpreted analyzer only.
********************************************/

bool NLP::apiStepAnalyze(
	void *parse		// Handle for current parse.
	)
{
Parse *prs = (Parse *) parse;
long num = prs->getCurrpass();
Delt<Seqn> *seq = 0;
if (num > 0)
	{
	if (!(seq = prs->getSeq()))
		{
		prs->setCurrpass(0);	// Reset.
		return false;		// Finished analyzing.
		}
	seq = seq->Right();	// Next pass.
	++num;					// Next pass.
	}
else	// No passes executed yet.
	{
	seq = prs->getAna()->getSeq();
	num = 1;					// At first pass.
	}

if (!seq)
	{
	prs->setCurrpass(0);	// Reset.
	return false;		// Finished analyzing.
	}

prs->setCurrpass(num);		// Install in parse object.
prs->setSeq(seq);		// Install in parse object.

//currpass = num;	// Now redundant.										// 08/22/02 AM.
if (!prs->stepExecute(seq, num))
	{
	prs->setCurrpass(0);
	prs->setSeq(0);
	return false;
	}
return true;
}


/********************************************
* FN:		APICLEANANALYZE
* CR:		07/23/01 AM.
* RET:	parse - Handle.  Internally = Parse object.
* SUBJ:	API call to clean up after analysis of a text.
* NOTE:	To enable stepping through analyzer by caller.
*			Interpreted analyzer only.
********************************************/

bool NLP::apiCleanAnalyze(
	void *parse		// Handle for current parse.
	)
{
if (!parse)
	return false;
Parse *prs = (Parse *) parse;

Eana *eana = prs->getEana();

#ifdef LINUX
std::ostringstream* cbuf = prs->getCbuf();											// 09/28/19 AM.
#else
std::_t_ostream *cbuf = prs->getCbuf();											// 05/13/02 AM.
#endif

if (cbuf)																		// 05/13/02 AM.
	*cbuf << std::ends;		// Terminate buffer.								// 05/13/02 AM.

// PRINT OUT THE FINAL PARSE TREE.										// 08/05/01 AM.
if (eana->getFlogfinal())													// 08/05/01 AM.
	prs->finalTree();															// 08/05/01 AM.

cleanAnalyze(prs);
if (eana)
	delete eana;	// MOVED AFTER CLEANANALYZE.						// 07/19/03 AM.

return true;
}

/********************************************
* FN:		APISETPOPUPDATA
* CR:		05/24/02 AM.
* SUBJ:	Transfer string from GUI popup to analyzer.
* ALG:	When data is fetched from the popup in NLP++, the popupdat_ variable
*			is automatically cleared as well. (getpopupdata).
********************************************/

bool NLP::apiSetpopupdata(_TCHAR *x)
{
if (popupdat_)	// Not cleared from prior invocation of popup.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning: Data from GUI popup was never used.]") << std::ends;
	errOut(&gerrStr,false);
	}

if (!x)
  popupdat_ = 0;
else
  internStr(x, popupdat_);
return true;
}


/********************************************
* FN:		RUNANALYZER
* CR:		05/15/00 AM.
* SUBJ:	Analyze a single text with compiled runtime analyzer.
********************************************/

void NLP::runAnalyzer(
	_TCHAR *input,
	_TCHAR *output,
	_TCHAR *appdir,
	bool flogfiles,	// Changing the meaning of this.
	bool silent,		// Silent run mode.								// 06/16/02 AM.
	_TCHAR *outdir,		// Interned files.
	_TCHAR *inbuf,		// If != 0, analyzing BUFFER
	long len,				// If != 0, then buffer length
	std::_t_ostream *os,		// Rebinding of output stream.				// 05/13/02 AM.
	void *cbufv,		// Output buffer stream, if outlen > 0.	// 10/10/03 AM.
	long outlen,			// Output buffer max length.				// 05/13/02 AM.
	_TCHAR *datum			// Pass info to G("$datum").					// 03/13/03 AM.
	)
{

std::_t_ostrstream *cbuf = (std::_t_ostrstream *) cbufv;					// 10/10/03 AM.

clock_t   s_time, e_time;

//if (flogfiles)							// 02/21/02 AM.	// 03/29/05 AM.
	s_time = clock();

// Set up runtime config for current parse.
enum Eanaconf conf = conf_ZILCH;											// 06/16/02 AM.
if (!silent)														// FIX	// 07/03/02 AM.
	{
	if (flogfiles)																// 07/03/02 AM.
		conf = conf_DEV;														// 07/03/02 AM.
	else
		conf = conf_REG;														// 07/03/02 AM.
	}
Eana eana(conf);																// 06/16/02 AM.
eana.setFlogfiles(flogfiles);

Parse *parse = 0;

if (!(parse = initAnalyze(input,output,appdir,&eana,outdir,
									inbuf, len,
									os, cbuf, outlen							// 05/13/02 AM.
									)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Analyze: Couldn't create parse instance.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

// Pass arbitrary information to the analyzer.						// 03/13/03 AM.
parse->setDatum(datum);														// 03/13/03 AM.

// Compiled analyzer gets batchstart flag.							// 05/16/08 AM.
parse->setFbatchstart(fbatchstart_);									// 05/16/08 AM.

/////////////////////////////////////////////////
// EXECUTE THE ANALYZER
/////////////////////////////////////////////////
// Execute steps as described in analyzer sequence.
// Each pass traverses parse tree, applying rules and actions.


if (parse->getText())
	{
#ifdef RUNEMBED_
  cout << "runAnalyzer: RUNEMBED_ calling run_analyzer(parse)" << std::endl;
  run_analyzer(parse);
#else
#ifndef LINUX
	if (!hrundll_)
		parse->execute();
	else
		call_runAnalyzer(hrundll_,parse);
#else
	parse->execute();
#endif
#endif
	}

#ifdef LINUX
std::ostringstream *cbf = parse->getCbuf();										// 09/28/19 AM.
#else
std::_t_ostream *cbf = parse->getCbuf();										// 04/25/05 AM.
#endif
if (cbf)																			// 05/13/02 AM.
	*cbf << std::ends;		// Terminate buffer.								// 05/13/02 AM.

e_time = clock();

parse->finExecute(ana_->getNpasses(),s_time,e_time);				// 06/13/00 AM.

cleanAnalyze(parse);

// Compiled analyzer turns off batchstart by default after processing
// each file.  Caller can restart a new batch at any point, as desired.
fbatchstart_ = false;														// 05/16/08 AM.

if (flogfiles)																	// 02/21/02 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Exec analyzer time=")
		  << (double) (e_time - s_time)/CLOCKS_PER_SEC
		  << _T(" sec]") << std::ends;
	errOut(&gerrStr,false);
	s_time = e_time;
	}
}




/********************************************
* FN:		RUNANALYZER (VARIANT)
* CR:		11/17/20 AM.
* SUBJ:	Analyze a single text with compiled runtime analyzer.
********************************************/

void NLP::runAnalyzer(
	std::istringstream *iss,
	std::ostringstream *oss,
	_TCHAR *appdir,
	bool flogfiles,	// Changing the meaning of this.
	bool silent,		// Silent run mode.
	_TCHAR *outdir,		// Interned files.
	std::_t_ostream *os,		// Rebinding of output stream.
	_TCHAR *datum			// Pass info to G("$datum").
	)
{

std::_t_ostrstream *cbuf = (std::_t_ostrstream *) oss;	// 11/17/20 AM.

clock_t   s_time, e_time;

//if (flogfiles)							// 02/21/02 AM.	// 03/29/05 AM.
	s_time = clock();

// Set up runtime config for current parse.
enum Eanaconf conf = conf_ZILCH;											// 06/16/02 AM.
if (!silent)														// FIX	// 07/03/02 AM.
	{
	if (flogfiles)																// 07/03/02 AM.
		conf = conf_DEV;														// 07/03/02 AM.
	else
		conf = conf_REG;														// 07/03/02 AM.
	}
Eana eana(conf);																// 06/16/02 AM.
eana.setFlogfiles(flogfiles);

Parse *parse = 0;

if (!(parse = initAnalyze(iss,oss,appdir,&eana,outdir,os)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Analyze: Couldn't create parse instance.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

// Pass arbitrary information to the analyzer.						// 03/13/03 AM.
parse->setDatum(datum);														// 03/13/03 AM.

// Compiled analyzer gets batchstart flag.							// 05/16/08 AM.
parse->setFbatchstart(fbatchstart_);									// 05/16/08 AM.

/////////////////////////////////////////////////
// EXECUTE THE ANALYZER
/////////////////////////////////////////////////
// Execute steps as described in analyzer sequence.
// Each pass traverses parse tree, applying rules and actions.


if (parse->getText())
	{
#ifdef RUNEMBED_
  cout << "runAnalyzer: RUNEMBED_ calling run_analyzer(parse)" << std::endl;
  run_analyzer(parse);
#else
#ifndef LINUX
	if (!hrundll_)
		parse->execute();
	else
		call_runAnalyzer(hrundll_,parse);
#else
	parse->execute();
#endif
#endif
	}
#ifdef LINUX
std::ostringstream *cbf = parse->getCbuf();										// 09/28/19 AM.
#else
std::_t_ostream *cbf = parse->getCbuf();										// 04/25/05 AM.
#endif
if (cbf)																			// 05/13/02 AM.
	*cbf << std::ends;		// Terminate buffer.								// 05/13/02 AM.

e_time = clock();

parse->finExecute(ana_->getNpasses(),s_time,e_time);				// 06/13/00 AM.

cleanAnalyze(parse);

// Compiled analyzer turns off batchstart by default after processing
// each file.  Caller can restart a new batch at any point, as desired.
fbatchstart_ = false;														// 05/16/08 AM.

if (flogfiles)																	// 02/21/02 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Exec analyzer time=")
		  << (double) (e_time - s_time)/CLOCKS_PER_SEC
		  << _T(" sec]") << std::ends;
	errOut(&gerrStr,false);
	s_time = e_time;
	}
}


/********************************************
* FN:		USER_INI
* CR:		01/23/02 AM.
* SUBJ:	Enable user-defined initializations in USER project.
********************************************/

bool NLP::user_ini()
{
#ifndef LINUX

if (hdll_ && call_ucodeIni(hdll_,this))
	return true;

#endif
return false;
}

/********************************************
* FN:		USER_FIN
* CR:		01/23/02 AM.
* SUBJ:	Enable user-defined cleanups in USER project.
********************************************/

bool NLP::user_fin()
{
#ifndef LINUX

if (hdll_ && call_ucodeFin(hdll_,this))
	return true;

#endif
return false;
}


/********************************************
* FN:		CLEAN
* CR:		12/14/98 AM.
* SUBJ:	Clean up objects created by Lite.
* NOTE:	Most things clean themselves up.
********************************************/

void NLP::clean()
{
Algo::clean();
}


/********************************************
* FN:		NLP_CLEAN
* CR:		12/14/98 AM.
* SUBJ:	Clean up objects created by Lite.
* NOTE:	Most things clean themselves up.
********************************************/

void nlp_clean(Ana *rfa)
{
//if (rfa)
//	RFA::rfa_clean(*rfa);
Algo::clean();
}

/********************************************
* FN:		INTERNSTR
* CR:		12/10/99 AM.
* SUBJ:	Get a hash table version of a string.
********************************************/

bool NLP::internStr(_TCHAR *str, /*UP*/ _TCHAR* &hstr)
{
if (!str || !*str)
	return false;
if (!htab_)			// Make sure there's a hash table around!
	return false;
hstr = htab_->getStr(str);
return true;
}


/********************************************
* FN:		READCONVERT
* CR:		07/24/00 AM.
* SUBJ:	Read and convert an input file.
* NOTE:	Could do unicode conversions here, so that VisualText GUI
*			doesn't mess with that stuff. 03/03/05 AM.
********************************************/

bool NLP::readConvert(
	_TCHAR *fname,
	_TCHAR* &buf,		/*UP*/
	long &len,		/*UP*/
	long maxline	// default==MAXSTR
	)
{
return ::readConvert(fname,buf,len,maxline);	// From str.cpp
}


/********************************************
* FN:		DELETECHARS
* CR:		08/02/00 AM.
* SUBJ:	Delete an array allocated in lite.dll.
* NOTE:	Direct delete from VisualText causes problems, at least
*			in release version.  Trying this.
********************************************/

bool NLP::deleteBuf(_TCHAR *buf)
{
if (buf)
	Chars::destroy(buf);
return true;
}

// isBuiltin -- moved to vtrun.cpp.	// 08/28/02 AM.

/********************************************
* FN:		FILEDBG
* CR:		02/21/02 AM.
* SUBJ:	Set global debug output to a file.
* NOTE:	For verbose, timing, debug outputs for current analysis.
*			Not a rebind, like the others.
*			08/26/02 AM. Moved here.  Part of removing globals.
********************************************/

void NLP::fileDbg(_TCHAR *dirname, std::_t_ostream* &sdbg)
{
sdbg = dbgout_;			// Save the current debug stream.

_TCHAR fname[MAXSTR];
_stprintf(fname, _T("%s%c%s"), dirname, DIR_CH, _T("dbg.log"));
dbgout_ = new std::_t_ofstream(TCHAR2CA(fname), std::ios::out);
}


/********************************************
* FN:		RESETDBG
* CR:		02/21/02 AM.
* SUBJ:	Reset global debug output.
* NOTE:	08/26/02 AM. Moved here.  Part of removing globals.
********************************************/

void NLP::resetDbg(std::_t_ostream* &sdbg)
{
#ifdef OLD_
if (!dbgout_)
  {
  *gerr << _T("0 0 [Error: Debug output can't be reset.]") << std::endl;
  return;
  }
#endif
if (dbgout_)					// 06/16/02 AM.
	delete dbgout_;			// Done with debug stream.
dbgout_ = sdbg;				// Restore the save debug stream.
sdbg = 0;
}


/********************************************
* FN:		LOGOUT
* CR:		05/26/01 AM.
* SUBJ:	Print out a message and return true/false.
* RET:	Return given boolean value.
* NOTE:	Utility function for printing log/verbose message.
*			08/26/02 AM. Moved here.  Part of removing globals.
********************************************/

bool NLP::logOut(
	bool retval				// Value to return.
	)
{
if (dbgout_)																	// 06/30/03 AM.
*dbgout_																			// 02/21/02 AM.
//*gout																			// 02/21/02 AM.
		<< Errbuf		// Should rename Errbuf everywhere.
		<< std::endl;
return retval;

}

// VARIANT.	For Unicode, and to remove global buffer.				// 02/25/05 AM.
bool NLP::logOut(
	std::_t_strstream *st,		// Error message to print.
	bool retval				// Value to return.
	)
{
if (dbgout_)
*dbgout_
		<< st->str()
		<< std::endl;
#ifndef UNICODE
#ifndef LINUX
st->freeze(false);	// 07/01/05 AM.
st->clear();	// 07/01/05 AM.
#endif
#endif

return retval;
}


/********************************************
* FN:		OBJECT_COUNTS
* CR:		10/19/98 AM.
* SUBJ:	Count number of objects allocated for various classes.
* NOTE:	02/14/00 AM. Some reorganizing of this fn.
********************************************/

LITE_API void object_counts(std::_t_ofstream *ofstr)
{
#ifdef OLDY_
NLP::prettyCount(ofstr);			// 12/14/98 AM.
Algo::prettyCount(ofstr);
Ana::prettyCount(ofstr);
Iaction::prettyCount(ofstr);
Iarg::prettyCount(ofstr);
Ielement::prettyCount(ofstr);
Ipair::prettyCount(ofstr);
Irule::prettyCount(ofstr);
Iregion::prettyCount(ofstr);				// 11/24/98 AM.
Irecurse::prettyCount(ofstr);			// 11/24/98 AM.
Ifile::prettyCount(ofstr);				// 11/24/98 AM.

Iexpr::prettyCount(ofstr);				// 11/09/99 AM.
Ivar::prettyCount(ofstr);					// 11/09/99 AM.
Iop::prettyCount(ofstr);					// 11/09/99 AM.

Ln::prettyCount(ofstr);
Parse::prettyCount(ofstr);
Pn::prettyCount(ofstr);
PostRFA::prettyCount(ofstr);
Check::prettyCount(ofstr);				// 11/27/98 AM.
RFA::prettyCount(ofstr);
Sem::prettyCount(ofstr);
Seqn::prettyCount(ofstr);
String::prettyCount(ofstr);


Stab::prettyCount(ofstr);
Htab::prettyCount(ofstr);
Sym::prettyCount(ofstr);
Chars::prettyCount(ofstr);
// Delt
// Dlist
// Node
// Tree
#endif
Algo::prettyCount(ofstr);
Ana::prettyCount(ofstr);
Check::prettyCount(ofstr);
Code::prettyCount(ofstr);
Eana::prettyCount(ofstr);
Htab::prettyCount(ofstr);
Iaction::prettyCount(ofstr);
Iarg::prettyCount(ofstr);
Ielement::prettyCount(ofstr);
Iexpr::prettyCount(ofstr);
Iexprstmt::prettyCount(ofstr);
Ifile::prettyCount(ofstr);
Iifstmt::prettyCount(ofstr);
Iwhilestmt::prettyCount(ofstr);
Ireturn::prettyCount(ofstr);
Iop::prettyCount(ofstr);
Ipair::prettyCount(ofstr);
Irecurse::prettyCount(ofstr);
Iregion::prettyCount(ofstr);
Irule::prettyCount(ofstr);
Istmt::prettyCount(ofstr);
Ivar::prettyCount(ofstr);
Ln::prettyCount(ofstr);
Mode::prettyCount(ofstr);
NLP::prettyCount(ofstr);
Nlppp::prettyCount(ofstr);
Parse::prettyCount(ofstr);
Pn::prettyCount(ofstr);
RFA::prettyCount(ofstr);
Sem::prettyCount(ofstr);
Seqn::prettyCount(ofstr);
Stab::prettyCount(ofstr);
String::prettyCount(ofstr);
Sym::prettyCount(ofstr);

/*
D:\lite\delt.h(152):void Delt<DELTTYPE>::prettyCount(char *str)
D:\lite\dlist.h(239):void Dlist<DELTTYPE>::prettyCount(char *str)
D:\lite\node.h(188):void Node<NODETYPE>::prettyCount(char *str)
D:\lite\selt.h(135):void Selt<SELTTYPE>::prettyCount(char *str)
D:\lite\slist.h(245):void Slist<SELTTYPE>::prettyCount(char *str)
D:\lite\thtab.h(192):void tHtab<TYPE>::prettyCount(ofstr)
D:\lite\tree.h(182):void Tree<NODETYPE>::prettyCount(char *str)
D:\lite\tsym.h(182):void tSym<TYPE>::prettyCount(ofstr)
*/

Delt<Iaction>::prettyCount(_T("iaction"),ofstr);
Delt<Iarg>::prettyCount(_T("iarg"),ofstr);
Delt<Ielt>::prettyCount(_T("ielt"),ofstr);
Delt<Ipair>::prettyCount(_T("ipair"),ofstr);
//Delt<Ipost>::prettyCount("ipost",ofstr);
//Delt<Ipre>::prettyCount("ipre",ofstr);
Delt<Irule>::prettyCount(_T("irule"),ofstr);
Delt<Pn>::prettyCount(_T("pn"),ofstr);
Delt<Seqn>::prettyCount(_T("seqn"),ofstr);

Dlist<Iaction>::prettyCount(_T("iaction"),ofstr);
Dlist<Iarg>::prettyCount(_T("iarg"),ofstr);
Dlist<Ielt>::prettyCount(_T("ielt"),ofstr);
Dlist<Ipair>::prettyCount(_T("ipair"),ofstr);
//Dlist<Ipost>::prettyCount("ipost",ofstr);
//Dlist<Ipre>::prettyCount("ipre",ofstr);
Dlist<Irule>::prettyCount(_T("irule"),ofstr);
Dlist<Pn>::prettyCount(_T("pn"),ofstr);
Dlist<Seqn>::prettyCount(_T("seqn"),ofstr);

Node<Ln>::prettyCount(_T("ln"),ofstr);
Node<Pn>::prettyCount(_T("pn"),ofstr);

Selt<Algo>::prettyCount(_T("algo"),ofstr);
Selt<Iarg>::prettyCount(_T("iarg"),ofstr);
Selt<Ifile>::prettyCount(_T("ifile"),ofstr);			// 11/24/98 AM.
Selt<Irecurse>::prettyCount(_T("irecurse"),ofstr);	// 11/24/98 AM.
Selt<Iregion>::prettyCount(_T("iregion"),ofstr);		// 11/24/98 AM.
Selt<Irule>::prettyCount(_T("irule"),ofstr);			// 12/16/98 AM.
Selt<Istmt>::prettyCount(_T("istmt"),ofstr);
Selt<Sym>::prettyCount(_T("sym"),ofstr);
Selt<NLP>::prettyCount(_T("nlp"),ofstr);		// 07/19/03 AM.

Slist<Algo>::prettyCount(_T("algo"),ofstr);
Slist<Iarg>::prettyCount(_T("iarg"),ofstr);
Slist<Ifile>::prettyCount(_T("ifile"),ofstr);			// 11/24/98 AM.
Slist<Irecurse>::prettyCount(_T("irecurse"),ofstr);	// 11/24/98 AM.
Slist<Iregion>::prettyCount(_T("iregion"),ofstr);	// 11/24/98 AM.
Slist<Irule>::prettyCount(_T("irule"),ofstr);			// 12/16/98 AM.
Slist<Istmt>::prettyCount(_T("istmt"),ofstr);
Slist<Sym>::prettyCount(_T("sym"),ofstr);

Tree<Ln>::prettyCount(_T("ln"),ofstr);
Tree<Pn>::prettyCount(_T("pn"),ofstr);

tSym<Slist<Irule> >::prettyCount(ofstr);
tHtab<Slist<Irule> >::prettyCount(ofstr);
}



