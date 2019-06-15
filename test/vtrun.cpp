/*******************************************************************************
Copyright © 2002 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	VTRUN.CPP
* FILE:	lite/vtrun.h
* CR:		08/27/02 AM.
*
* SUBJ:	CLASS TO MANAGE VISUALTEXT RUNTIME
*
* NOTE:	Support for multiple and embedded analyzers.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include "machine.h"
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include	"lite/slist.h"		// 01/06/03 AM.
#include "node.h"				// 07/07/03 AM.
#include "tree.h"	// Linux.	// 04/26/07 AM.
#include "io.h"	// Linux.	// 04/26/07 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "lite/ana.h"
#include "lite/nlp.h"
#include "prim/libprim.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "htab.h"
#include "rfa.h"
#include "func_defs.h"
#include "ifunc.h"

#include "consh/libconsh.h"	// 07/18/03 AM.
#include	"consh/cg.h"			// 07/18/03 AM.

#include "lite/vtrun.h"

// Keep track of the first instance of this class.
VTRun *VTRun_Ptr = 0;

int VTRun::count_ = 0;
template<> int Slist<NLP>::count_	= 0;	// 01/06/03 AM.
template<> int Selt<NLP>::count_	= 0;	// 01/06/03 AM.


/********************************************
* FN:		Special functions for the class
********************************************/

VTRun::VTRun(
	_TCHAR *logfile,
	_TCHAR *rfbspecdir,
	bool silent
	)
{
zero();
if (!VTRun_Ptr)	// Creating first instance.
	{
	init(logfile,rfbspecdir,silent);
	VTRun_Ptr = this;
	}

#ifndef STABLE_
++count_;
#endif

}


VTRun::~VTRun()
{
VTRun_Ptr = 0;		// Clear out this global.
clear();
zero();

#ifndef STABLE_
--count_;
#endif
}

//*******************************************
void *VTRun::getHtab()
{
return htab_;
}


//*******************************************

/********************************************
* FN:		CLEAR
* CR:		08/27/02 AM.
* SUBJ:	Delete data structures of VisualText runtime system.
********************************************/

void VTRun::clear()
{
if (rfa_)
	{
	RFA::rfa_clean(*((Ana *)rfa_));		// Special cleanups for RFA.
	delete (Ana *) rfa_;
	}
if (rfb_)
	delete (Ana *) rfb_;
if (rfc_)
	delete (Ana *) rfc_;

if (nlpa_)
	delete (NLP *) nlpa_;
if (nlpb_)
	delete (NLP *) nlpb_;
if (nlpc_)
	delete (NLP *) nlpc_;

if (htab_)
	delete (Htab *) htab_;
if (stab_)
	delete (Stab *) stab_;
if (htfunc_)
	Ifunc::deleteHtab(htfunc_);
if (alist_)
	{
	((ALIST *)alist_)->list_clean();
	delete (ALIST *) alist_;
	}

if (anas_)																			// 01/06/03 AM.
//	Slist<NLP>::DeleteSlist((Slist<NLP>*)anas_);							// 07/18/03 AM.
	Slist<NLP>::DeleteSlistAndData((Slist<NLP>*)anas_);	// FIX.	// 07/18/03 AM.
}


/********************************************
* FN:		ZERO
* CR:		08/27/02 AM.
* SUBJ:	Zero out VisualText runtime system.
********************************************/

void VTRun::zero()
{
rfa_    = 0;
rfb_    = 0;
rfc_    = 0;
htab_   = 0;
stab_   = 0;
htfunc_ = 0;
alist_  = 0;
logfile_ = 0;
anas_   = 0;	// 01/06/03 AM.
nlpa_ = nlpb_ = nlpc_ = 0;	// 06/30/03 AM.
}


/********************************************
* FN:		INIT
* CR:		08/27/02 AM.
* SUBJ:	Initialize VisualText runtime system.
********************************************/

void VTRun::init(_TCHAR *logfile, _TCHAR *rfbspecdir,bool silent)
{
logfile_ = logfile;	// 10/29/02 AM.
stab_ = new Stab();

htab_ = (void *) new Htab((Stab *)stab_);
Htab::setGstab((Stab *)stab_);	// OBSOLETE: Get rid of this.

// Build a hash table for NLP++ builtin functions.
htfunc_ = Ifunc::makeHtab(nlpFuncs);

//#ifndef UNICODE
// TESTING UNICODE.	// 01/10/06 AM.

nlpa_ = new NLP(true);	// Dummy environment.						// 06/30/03 AM.
rfa_ = create_rfa(htab_,logfile,nlpa_);

nlpb_ = new NLP(true);	// Dummy environment.						// 06/30/03 AM.
rfb_ = create_rfb(logfile,rfbspecdir,false,silent,rfa_,htab_,nlpb_);
//#endif

alist_ = new ALIST();
((ALIST *)alist_)->list_ini();	// Initialize List Manager.

anas_ = new Slist<NLP>();	// Make empty list.	// 01/06/03 AM.
}


/********************************************
* FN:		CREATE_RFA
* CR:		08/27/02 AM.
* SUBJ:	Create the RFA analyzer.
* NOTE:	Convenience function.  Moved all this ancient code from NLP::init.
********************************************/

void *VTRun::create_rfa(
	void *htab,
	_TCHAR *logfile,
	NLP *nlp
	)
{
clock_t   s_time, e_time;
_t_ostream *sout;
_t_ofstream *fout;

////// DEFINE RFA.
// Define the rules-file analyzer!
// This will be an internal representation constructed by hand,
// that performs the parsing and internalization of the rules
// files themselves.  Call it the RF analyzer, or RFA.
// Note: Loading RFA,RFB even in compiled analyzer mode.			// 07/03/00 AM.
// (Just in case they're needed anywhere.)
s_time = clock();																// 12/28/98 AM.

Ana *rfa = new Ana();
//rfa_->setNLP(this);	// TRY WITHOUT AN NLP OBJECT.				// 08/27/02 AM.
rfa->setNLP(nlp);																// 06/30/03 AM.

rfa->setVerbose(false);														// 02/21/02 AM.

fileOut(logfile, /*DU*/ fout, sout);

// Moving error stream up from here.									// 01/17/02 AM.

RFA::make_rfa(*rfa);			// Build the rules file analyzer.

//if (develop)
resetOut(fout, sout);

rfa->setHtab((Htab *)htab);	// Use the global hash table.		// 08/27/02 AM.

e_time = clock();																// 12/28/98 AM.
#ifdef DIAGNOSTIC_
_t_strstream gerrStr1;
gerrStr1 << _T("[Build RFA time=")
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC
	  << _T(" sec]") << ends;
errOut(&gerrStr1,false);
#endif
s_time = e_time;

// 11/04/99 AM. TESTING RFA GENERATION TO FILES.
// Commenting this out.											// OPT	// 02/21/02 AM.
#ifdef GENRFA_
_stprintf(tmp, _T("%s%crfa%c%s"), appdir,DIR_CH,DIR_CH,					// 03/08/00 AM.
			SPECDIRNAME);														// 05/14/00 AM.
if (!dir_exists(tmp))
	make_dir(tmp);
rfa->genAna(tmp);
#endif

return (void *) rfa;
}


/********************************************
* FN:		CREATE_RFB
* CR:		08/27/02 AM.
* SUBJ:	Create the RFB analyzer.
* NOTE:	Convenience function.  Moved all this ancient code from NLP::init.
********************************************/

void *VTRun::create_rfb(
	_TCHAR *logfile,
	_TCHAR *rfbspecdir,
	bool develop,
	bool silent,
	void *rfa,
	void *htab,
	NLP *nlp
	)
{
clock_t   s_time, e_time;

_TCHAR *appdir = _T("c:\\temp");	// Temp directory for now.				// 08/27/02 AM.

s_time = clock();

// MAKE THE RFB ANALYZER.													// 11/05/99 AM.
// RFA is used to make RFB, which then replaces RFA!
Ana *rfb = (Ana *)NLP::make_rfb(appdir,								// 11/05/99 AM.
			rfbspecdir,															// 12/08/99 AM.
			develop,
			silent,
			rfa,
			htab);
rfb->setNLP(nlp);																// 06/30/03 AM.

e_time = clock();																// 11/05/99 AM.

if (!rfb)																		// 07/06/01 AM.
	{
	_t_strstream gerrStr3;						// 07/06/01 AM.
	gerrStr3 << _T("[ERROR IN ANALYZER INIT.]") << ends;				// 07/06/01 AM.
	errOut(&gerrStr3,false);																// 07/06/01 AM.
	}

#ifdef DIAGNOSTIC_
_t_strstream gerrStr2;
gerrStr2 << _T("[Build RFB time=")											// 11/05/99 AM.
	  << (double) (e_time - s_time)/CLOCKS_PER_SEC
	  << _T(" sec]") << ends;
errOut(&gerrStr2,false);
#endif
s_time = e_time;																// 11/05/99 AM.

return (void *) rfb;
}

/********************************************
* FN:		ISBUILTIN
* CR:		12/24/01 AM.
* SUBJ:	Is given name a builtin NLP++ function name.
* NOTE:	08/28/02 AM. Moved here from nlp.cpp.
********************************************/

bool VTRun::isBuiltin(_TCHAR *name)
{
if (!name)
	return false;
return ((Ifunc::funcID(name,htfunc_) != FNNULL) ? true : false);
}

/********************************************
* FN:		ADDANA
* CR:		01/06/03 AM.
* SUBJ:	Register analyzer instance.
********************************************/

bool VTRun::addAna(NLP *nlp)
{
if (!nlp)
	return false;
if (!anas_)
	return false;
_TCHAR *name = nlp->getName();
if (!name || !*name)
	return false;	// Only register if it has a name.
((Slist<NLP>*)anas_)->push(nlp);
return true;
}


/********************************************
* FN:		RMANA
* CR:		01/06/03 AM.
* SUBJ:	Unregister analyzer instance.
********************************************/

bool VTRun::rmAna(NLP *nlp)
{
if (!nlp)
	return false;
if (!anas_)
	return false;
_TCHAR *name = nlp->getName();
if (!name || !*name)
	return false;	// Only unregister if it has a name.

Selt<NLP> *selt = ((Slist<NLP>*)anas_)->getFirst();
if (!selt)
	return false;
if (selt->getData() == nlp)	// At head of list.
	{
	((Slist<NLP>*)anas_)->pop();
	Selt<NLP>::DeleteSelt(selt);	// 07/18/03 AM.
	return true;
	}

// Find the list element for this analyzer.
while (selt->Right())
	{
	if (selt->Right()->getData() == nlp)
		break;
	selt = selt->Right();
	}

// Delete the list element.
if (selt && selt->Right())
	{
	Selt<NLP> *tmp =							// FIX.	// 07/19/03 AM.
		((Slist<NLP>*)anas_)->rmNext(selt);
	Selt<NLP>::DeleteSelt(tmp);	// MEM LEAK.	// 07/19/03 AM.
	}
return true;
}


/********************************************
* FN:		FINDANA
* CR:		01/06/03 AM.
* SUBJ:	Find analyzer instance by name.
********************************************/

NLP *VTRun::findAna(_TCHAR *name)
{
if (!name || !*name || !anas_)
	return 0;

Selt<NLP> *selt = ((Slist<NLP>*)anas_)->getFirst();
NLP *nlp;
while (selt)
	{
	nlp = selt->getData();
	if (!strcmp_i(nlp->getName(),name))
		return nlp;
	selt = selt->Right();
	}
return 0;
}


/********************************************
* FN:		MAKECG
* CR:		07/18/03 AM.
* SUBJ:	Make knowledge base instance.
* NOTE:	Localizing creation, deletion of CG to lite dll.
********************************************/

CG *VTRun::makeCG(
	_TCHAR *appdir,	// Directory housing kb folder.
	bool compiled,	// If loading compiled kb.
	NLP *nlp			// Associated analyzer if any.
	)
{
if (!appdir || !*appdir)
   {
   _t_cerr << _T("[makeCG: Given no kb directory.]") << endl;
   return 0;
   }

//CG *cg = new CG(
//         appdir,
//         compiled,      // LOAD COMPILED KB IF POSSIBLE.
//         (ALIST*)alist_);    // Give KB the handle for List Manager.
CG *cg = CG::makeCG((ALIST*)alist_,appdir,compiled);	// 07/28/03 AM.

if (!cg)
   {
   _t_cerr << _T("[makeCG: Couldn't make knowledge base.]") << endl;
   return 0;
   }

//return cg;	///// TESTING!!!!!!!!!!!!  // 07/28/03 AM.

if (!cg->readKB())
   {
   _t_cerr << _T("[Couldn't read knowledge base.]") << endl;
//   if (cg) delete cg;	// 07/18/03 AM.
	CG::deleteCG(cg);
   return 0;
   }

if (nlp)
	nlp->setCG(cg);      // Give analyzer the CG REFERENCE.

return cg;
}


/********************************************
* FN:		DELETECG
* CR:		07/18/03 AM.
* SUBJ:	Delete knowledge base instance.
* NOTE:	Localizing creation, deletion of CG to lite dll.
* TODO:	Should see if any registered analyzers use this cg,
*			and dissociate them.
********************************************/

bool VTRun::deleteCG(
	CG *cg
	)
{
if (!cg)
   {
   _t_cerr << _T("[deleteCG: Given no KB object.]") << endl;
   return false;
   }

// Traverse the registered NLP objects.
// Remove references to the current CG object.
Selt<NLP> *selt = ((Slist<NLP>*)anas_)->getFirst();
NLP *nlp;
while (selt)
	{
	nlp = selt->getData();
	if (nlp->getCG() == cg)
		nlp->setCG(0);	// Clear reference to this kb.
	selt = selt->Right();
	}

//delete cg;
CG::deleteCG(cg);		// 07/28/03 AM.
return true;			// 07/28/03 AM.
}


/********************************************
* FN:		MAKENLP
* CR:		07/18/03 AM.
* SUBJ:	Make an analyzer.
* NOTE:	Localizing creation, deletion of NLP objects to lite dll.
********************************************/

NLP *VTRun::makeNLP(
	_TCHAR *appdir,	// Directory housing kb folder.
	_TCHAR *name,
	bool develop,
	bool silent,
	bool compiled
	)
{
if (!appdir || !*appdir)
   {
   _t_cerr << _T("[makeNLP: Given no analyzer directory.]") << endl;
   return 0;
   }
if (!name || !*name)
   {
   _t_cerr << _T("[makeNLP: Given no analyzer name.]") << endl;
   return 0;
   }

NLP *nlp = new NLP(
	appdir,
	develop,
	silent,
	compiled,
	0,
	name
	);
return nlp;
}


/********************************************
* FN:		DELETENLP
* CR:		07/18/03 AM.
* SUBJ:	Delete analyzer instance.
* NOTE:	Localizing creation, deletion of NLP objects to lite dll.
********************************************/

bool VTRun::deleteNLP(
	NLP *nlp
	)
{
if (!nlp)
   {
   _t_cerr << _T("[deleteNLP: Given no NLP object.]") << endl;
   return false;
   }

delete nlp;
return true;
}

/********************************************
* FN:		MAKEVTRUN
* CR:		07/19/03 AM.
* SUBJ:	Create runtime environment.
* NOTE:	Localizing creation, deletion of objects to lite dll.
********************************************/

VTRun *VTRun::makeVTRun(
	_TCHAR *logfile,
	_TCHAR *rfbspecdir,
	bool silent
	)
{
return new VTRun(
	logfile,
	rfbspecdir,
	silent
	);
}


/********************************************
* FN:		DELETEVTRUN
* CR:		07/19/03 AM.
* SUBJ:	Delete runtime environment.
* NOTE:	Localizing creation, deletion of objects to lite dll.
********************************************/

bool VTRun::deleteVTRun(VTRun *vtrun)
{
if (vtrun)
	{
	delete vtrun;
	return true;
	}
return false;
}

/********************************************
* FN:		GETKBROOT
* CR:		12/19/03 AM.
* SUBJ:	Get KB root.
* NOTE:	Testing memory leak issues.
********************************************/

CONCEPT *VTRun::getKBroot(CG *cg)
{
if (cg)
  return cg->findRoot();
return 0;
}

/*************************      END OF FILE      ******************************/
