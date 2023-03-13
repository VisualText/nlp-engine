/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	RUG.CPP
* FILE:	librug/rug.cpp
* CR:		05/11/99 AM.
* SUBJ:	Top level of analyzer generator library for VisualText.
* NOTE:	12/11/99 AM. Folding into LITE LIBRARY!  It's about time.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"		// 03/09/00 AM.
#include "u_out.h"		// 01/19/06 AM.

#include "consh/libconsh.h"		// 02/14/01 AM.
#include "consh/cg.h"				// 02/14/01 AM.
#include "lite/lite.h"
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"															// 09/26/01 AM.
#include "std.h"	// For file_size.	// 05/21/09 AM.
#include "lite/mach.h"		// 08/29/99 AM. For remove_path.
#include "node.h"	// 07/07/03 AM.
#include "lite/nlp.h"
#include "vtrun.h"	// [DEGLOB]	// 10/15/20 AM.
#include "iarg.h"		// 02/14/01 AM.
#include "parse.h"	// 05/19/99 AM.
#include "Eana.h"		// 10/13/99 AM.
#include "lite/Aseq.h"		// 05/20/99 AM.
#include "Arug.h"		// 05/20/99 AM.
#include "mode.h"		// 04/04/00 AM.
#include "vtrun.h"	// 07/03/03 AM.

#include "lite/rug.h"											// 12/11/99 AM.

int RUG::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/
RUG::RUG(										// Default constructor.
		CONCEPT *conc,				// Top-level GRAM concept.
		NLP     *nlp,				// Current analyzer.
		_TCHAR    *appdir,			// Base directory for analyzer app.
		CG *cg						// Current KB.		// 05/17/99 AM.
		)
{
gram_ = conc;
nlp_		= nlp;
appdir_[0] = '\0';
if (appdir && *appdir)
	_tcscpy(appdir_, appdir);
cg_ = cg;							// 05/17/99 AM.

// Runtime config for RFA to intern passes.					// 10/13/99 AM.
erfa_ = new Eana(conf_INTERN);									// 12/20/01 AM.


// Defaults for output of internal analysis of samples.	// 05/18/99 AM.
if (appdir_[0])
	{
	_stprintf(outdir_, _T("%s%coutput"), appdir_, DIR_CH);
	_stprintf(output_, _T("%s%coutput.txt"), outdir_, DIR_CH);
	}
else
	{
	outdir_[0] = '\0';
	_tcscpy(output_, _T("output.txt"));
	}

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

RUG::~RUG()
{
// Delete runtime config for RFA.				// 10/13/99 AM.
if (erfa_)												// 10/13/99 AM.
	{
	delete erfa_;										// 10/13/99 AM.
	erfa_ = 0;											// 10/13/99 AM.
	}

#ifndef STABLE_
--count_;
#endif
}

/********************************************
* FN:		Access Functions
********************************************/
CONCEPT	*RUG::getGram()	{return gram_;}
NLP		*RUG::getNLP()		{return nlp_;}
_TCHAR		*RUG::getAppdir()	{return appdir_;}
CG			*RUG::getCG()		{return cg_;}				// 05/17/99 AM.
_TCHAR		*RUG::getOutdir()	{return outdir_;}			// 05/18/99 AM.
_TCHAR		*RUG::getOutput()	{return output_;}			// 05/18/99 AM.
Eana		*RUG::getErfa()	{return erfa_;	 }			// 10/13/99 AM.

/********************************************
* FN:		Modify Functions
********************************************/
void	RUG::setGram(CONCEPT *x)	{gram_		  = x ;}
void	RUG::setNLP(NLP *x)			{nlp_			  = x ;}
void	RUG::setAppdir(_TCHAR *x)		{_tcscpy(appdir_,x);}
void	RUG::setCG(CG *x)				{cg_			  = x ;}		// 05/17/99 AM.
void	RUG::setOutdir(_TCHAR *x)		{_tcscpy(outdir_,x);}		// 05/18/99 AM.
void	RUG::setOutput(_TCHAR *x)		{_tcscpy(output_,x);}		// 05/18/99 AM.
void	RUG::setErfa(Eana *x)		{erfa_		  = x ;}		// 10/13/99 AM.

/********************************************
* FN:		Class-wide Functions
********************************************/

/********************************************
* FN:		GETCOUNT
* CR:		12/14/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int RUG::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		12/14/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void RUG::prettyCount()
{
if (count_)
	{
	std::_t_cout << _T("Active RUG count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active RUG count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

/********************************************
* FN:		RUG_COUNTS
* SUBJ:	Count number of objects allocated for various classes.
********************************************/

//LIBRUG_API void rug_counts()										// 12/11/99 AM.
//{
//RUG::prettyCount();													// 12/11/99 AM.
//}



/********************************************
* FN:		RUGINIT
* CR:		08/28/99 AM.
* SUBJ:	Initializations for rule generation.
* NOTE:	CLASS-WIDE FUNCTION.
*			Called merely to add attributes to Gram concept.
*			Copied here from rug constructor.
********************************************/

void
RUG::rugInit(CONCEPT *gram, CG *cg)
{
// Set default attributes in Gram concept.			// 08/25/99 AM.
// 08/28/99 AM. Shouldn't be here, but in initializing the KB.
_TCHAR val[MAXSTR];
val[0] = '\0';
cg->findVal(gram, _T("xwhite"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("xwhite"), _T("true"));				// 08/25/99 AM.
cg->findVal(gram, _T("constrain"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("constrain"), _T("true"));			// 08/25/99 AM.

cg->findVal(gram, _T("closed"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("closed"), _T("false"));				// 08/25/99 AM.
cg->findVal(gram, _T("lit"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("lit"), _T("false"));					// 08/25/99 AM.
cg->findVal(gram, _T("split"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("split"), _T("false"));				// 08/25/99 AM.
cg->findVal(gram, _T("gen"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("gen"), _T("false"));					// 08/25/99 AM.
cg->findVal(gram, _T("locked"), /*UP*/ val);
if (!val || !*val)
	cg->addVal(gram, _T("locked"), _T("false"));				// 08/25/99 AM.
cg->findVal(gram, _T("quicksem"), /*UP*/ val);			// 12/27/99 AM.
if (!val || !*val)											// 12/27/99 AM.
	cg->addVal(gram, _T("quicksem"), _T("false"));				// 12/27/99 AM.
cg->findVal(gram, _T("counter"), /*UP*/ val);				// 11/27/00 AM.
if (!val || !*val)											// 11/27/00 AM.
	cg->addVal(gram, _T("counter"), _T("false"));				// 11/27/00 AM.
if (!val || !*val)											// 12/08/00 AM.
	cg->addVal(gram, _T("notrig"), _T("false"));				// 12/08/00 AM.
}


/********************************************
* FN:		RUGGENERATE
* CR:		05/11/99 AM.
* SUBJ:	Generate analyzer from given examples.
********************************************/

bool
RUG::rugGenerate(
	bool justDirty,		// If only doing dirty concepts.		// 08/25/99 AM.
	bool skipLocks			// If skipping locked concepts.		// 08/25/99 AM
	)
{
CONCEPT *gram;				// Top-level GRAM concept.
NLP     *nlp;				// Current analyzer.
_TCHAR    *appdir;			// Base directory for analyzer app.
CG		  *cg;				// Current KB.				// 05/17/99 AM.

gram = getGram();
nlp  = getNLP();
appdir = getAppdir();
cg	  = getCG();
bool ok = true;	// 05/20/09 AM.

if (!gram || !nlp || !appdir || !*appdir)
	{
	std::_t_cerr << _T("[rugGenerate: Empty hierarchy,analyzer, or base dir]") << std::endl;
	return false;
	}


// Set up to use persistent global hash,sym tables.		// 07/03/03 AM.
Htab *savehtab = (Htab*) nlp_->getHtab();						// 07/03/03 AM.
Stab *savestab = (Stab*) nlp_->getStab();						// 07/03/03 AM.
VTRun *vtrun = nlp_->getVTRun();	// [DEGLOB]	// 10/15/20 AM.
//nlp_->setHtab((Htab*)VTRun_Ptr->htab_);	// 07/03/03 AM.	// [DEGLOB]	// 10/15/20 AM.
//nlp_->setStab((Stab*)VTRun_Ptr->stab_);	// 07/03/03 AM.	// [DEGLOB]	// 10/15/20 AM.
nlp_->setHtab((Htab*)vtrun->htab_);	// [DEGLOB]	// 10/15/20 AM.
nlp_->setStab((Stab*)vtrun->stab_);	// [DEGLOB]	// 10/15/20 AM.

// Remove generated passes, if any, from analyzer.		// 06/01/99 AM.
rmGenpasses();

// Fabricate an input document from from the samples in the Gram hierarchy.
// HARDWIRE the fabricated filename as RUG.TXT.
_TCHAR intext[MAXPATH*2];
_stprintf(intext, _T("%s%crug%crug.txt"), appdir, DIR_CH,DIR_CH);		// HARDWIRED.
// 05/16/99 AM.  Actually, going to take text directly from samples into
// the input buffer.  NO FILES!  Even if fabricating, will place dummy
// separators into the buffer to separate samples for a concept.

// Start up a parse and get a handle for it.
//Parse *parse;					// The current analysis instance.
//parse = nlp->initAnalyze(intext,output_, appdir, false, outdir_);
//if (!parse)
//	return false;

// Traverse analyzer sequence till stub pass.
APASS *seq;						// Analyzer sequence.
int num = 0;					// Current pass number.
_TCHAR *passname;				// Name of current pat pass file.

num = 0;
for (seq = Aseq::firstPass(nlp); seq; seq = Aseq::nextPass(seq))
	{
	// 09/23/99 AM. Renumber each pass in the analyzer sequence.
	// (Will also add a PASSNUM attribute to each rule concept in the
	// GRAM hierarchy.)	// 09/23/99 AM.
	Aseq::setPassnum(seq, ++num);		// 09/23/99 AM.

	// Get the pat pass name.
	// If it's a stub pass, look for it in the top level concepts under the
	// GRAM hierarchy.
	passname = Aseq::getData(seq);
	if (endStub(seq))
		std::_t_cerr << _T("[Found end stub.]") << std::endl;		// Skip end of stub passes region.
	else if (startStub(seq))
		{
		std::_t_cerr << _T("[Found stub pass=") << passname << _T("]") << std::endl;

		// Find Gram hierarchy top-level concept that matches stub.
		_TCHAR *stubname;
		stubname = stubName(seq);
//		stubname = passname + 5;		// Skip past "stub_".
		CONCEPT *tree;
		if ((tree = findStubconcept(stubname, gram)))
			{
			std::_t_cerr << _T("[Found stub in hierachy]") << std::endl;

			// Traverse subtree of concepts.  Generate a pass for each
			// "rule" concept. RETURN LAST NEW PASS GENERATED.
			ok = genTree(tree, justDirty, skipLocks, /*DU*/ seq);	// 06/23/99 AM.
			//genxConcepts(cg->Down(tree), /*DU*/ seq);		// 06/23/99 AM.
			num = Aseq::getPassnum(seq);							// 09/23/99 AM.
			}
		if (!ok)	// 05/20/09 AM.
			break;	// 05/20/09 AM.
		}
	if (!ok)	// 05/20/09 AM.
		break;	// 05/20/09 AM.
	}

// Don't execute yet.
//	else if (!nlp->stepExecute(parse, seq, ++num))	// Process normal pass.
//		return false;

// See if stub matches a top-level node in the Gram hierarchy.
// If so, process the matching subtree.
//		Generate and execute one pass per concept that has samples.
//		Postorder.  Generate for children, then for concept.

//		Gen algo: Find nodes within the offset of sample, in parse tree.
//		Build rule based on these nodes.
//		Also based on subconcepts in HIERARCHY.  Match them up.
// Repeat till done.

// Restore hash,sym table to analyzer.							// 07/03/03 AM.
nlp_->setHtab(savehtab);											// 07/03/03 AM.
nlp_->setStab(savestab);											// 07/03/03 AM.

if (!ok)	// 05/20/09 AM.
	{
	return ok;	// 05/20/09 AM.
	}
return true;
}


/********************************************
* FN:		RMGENPASSES
* CR:		05/31/99 AM.
* SUBJ:	Remove the generated passes from analyzer sequence.
* ALG:	Traverse the analyzer sequence, looking for passes named
*			"stub_X", not "stub_end".  For each such, delete all passes
*			till "stub_end" is reached.
********************************************/

bool
RUG::rmGenpasses()
{
APASS *seq;						// Analyzer sequence.
APASS *snext;					// Pointer to next.
bool in_stub = false;		// If in generated pass region.

seq = Aseq::firstPass(nlp_);
for (;;)
	{
	if (!seq)
		return true;
	snext = Aseq::nextPass(seq);	// Save the next, in case current is rmed.

	if (startStub(seq))
		{
		std::_t_cout << _T("[Found stub pass.]") << std::endl;
		in_stub = true;
		}
	else if (endStub(seq))
		{
		std::_t_cout << _T("[Found end stub pass.]") << std::endl;
		in_stub = false;
		}
	else if (in_stub)			// At a generated pass.
		{
		std::_t_cout << _T("[Removing gend pass.]") << std::endl;
		Aseq::excisePass(nlp_, seq);
		Aseq::deletePass(seq);
		}
	seq = snext;
	}
return true;
}


/********************************************
* FN:		RMGENPASSES
* CR:		08/04/99 AM.
* SUBJ:	Remove generated passes for the given stub.
* RET:	True if found named stub pass.
* ALG:	Delete passes from the named stub pass to its end pass,
*			non-inclusive.
********************************************/

bool
RUG::rmGenpasses(_TCHAR *stubname)
{
APASS *seq;						// Analyzer sequence.
APASS *snext;					// Pointer to next.
bool in_stub = false;		// If in generated pass region.

seq = Aseq::firstPass(nlp_);
for (;;)
	{
	if (!seq)
		return false;
	snext = Aseq::nextPass(seq);	// Save the next, in case current is rmed.

	if (startStub(seq) && !_tcscmp(stubname, Aseq::getData(seq)))
		{
		std::_t_cout << _T("[Found stub pass.]") << std::endl;
		in_stub = true;
		}
	else if (endStub(seq))
		{
		if (in_stub)			// If done removing stub's passes.
			return true;
		}
	else if (in_stub)			// At a generated pass.
		{
		std::_t_cout << _T("[Removing gend pass.]") << std::endl;
		Aseq::excisePass(nlp_, seq);
		Aseq::deletePass(seq);
		}
	seq = snext;
	}
return false;			// Didn't find named stub.
}


/********************************************
* FN:		STARTSTUB
* CR:		05/31/99 AM.
* SUBJ:	If given pass is a stub pass that begins a region of gend passes.
********************************************/

bool
RUG::startStub(APASS *pass)
{
if (!pass)
	return false;

if (!_tcscmp(_T("stub"), Aseq::getAlgo(pass)))	// 06/23/99 AM.
	return true;

#ifdef OLD_
_TCHAR *passname;
passname = Aseq::getData(pass);
if (!passname || !*passname)
	return false;

if (!_tcsicmp(passname, _T("stub_end")))
	return false;
else if (!_tcsncmp(passname, _T("stub_"), 5))
	return true;
#endif

return false;
}


/********************************************
* FN:		ENDSTUB
* CR:		05/31/99 AM.
* SUBJ:	If given pass is a stub pass that std::ends a region of gend passes.
********************************************/

bool
RUG::endStub(APASS *pass)
{
if (!pass)
	return false;

if (!_tcscmp(_T("end"), Aseq::getAlgo(pass)))	// 06/23/99 AM.
	return true;

#ifdef OLD_
_TCHAR *passname;
passname = Aseq::getData(pass);
if (!passname || !*passname)
	return false;

if (!_tcsicmp(passname, _T("stub_end")))
	return true;
#endif

return false;
}


/********************************************
* FN:		STUBNAME
* CR:		06/01/99 AM.
* SUBJ:	Get name for given stub pass.
********************************************/

_TCHAR *
RUG::stubName(APASS *pass)
{
if (!pass)
	return 0;
_TCHAR *passname;
passname = Aseq::getData(pass);
if (!passname || !*passname)
	return 0;

if (_tcscmp(_T("stub"), Aseq::getAlgo(pass)))		// 06/23/99 AM.
	return 0;

return passname;										// 06/23/99 AM.

#ifdef OLD_
if (_tcsncmp(passname, _T("stub_"), 5))
	return 0;

return passname+5;	// Skip past "stub_".
#endif

}


/********************************************
* FN:		findStubconcept
* CR:		05/17/99 AM.
* SUBJ:	Find top-level hierarchy concept matching analyzer stub pass.
********************************************/

CONCEPT *
RUG::findStubconcept(
	_TCHAR *name,			// Name of stub, from analyzer sequence.
	CONCEPT *gram		// Top level of Gram hierarchy.
	)
{
if (!gram || !name || !*name)
	return 0;
return cg_->findConcept(gram, name);
}


/********************************************
* FN:		genTree
* CR:		05/17/99 AM.
* SUBJ:	Generate passes for a stub subtree of gram hierarchy.
* NOTE:	Reinvokes analyzer multiple times for samples under each concept.
* WARN:	MODIFIES THE ANALYZER SEQUENCE AS IT PROGRESSES!
********************************************/

bool
RUG::genTree(
	CONCEPT *tree,		// Concept immediately under Gram.  Matches analyzer
							// STUB pass.
	bool justDirty,		// If only doing dirty concepts.		// 08/25/99 AM.
	bool skipLocks,		// If skipping locked concepts.		// 08/25/99 AM
	/*DU*/
	APASS* &seq			// The last pass generated by genTree.
	)
{
if (!tree || !seq)
	return false;

// Todo: Any handling for stub concept should go here.

// NOTE:	SHOULD GET THESE FROM GRAM CONCEPT!  // 08/25/99 AM.
// Create info structure to carry down the tree.
Mode mode;				// 07/19/99 AM.
mode.setConstrain(true);		// DEFAULT: RETAIN CONSTRAINED RULES ONLY.

// DEFAULT IS TO GENERALIZE WHITESPACE.	// 08/09/99 AM.
mode.setXwhite(true);

updateMode(&mode, tree);

// Traverse recursively.
return genTreeRec(cg_->Down(tree), false, &mode, justDirty,skipLocks,
															/*DU*/ seq);
}


/********************************************
* FN:		genTreeRec
* CR:		05/17/99 AM.
* SUBJ:	Generate passes for a stub subtree of gram hierarchy.
* NOTE:	06/23/99 AM. Restoring this and genTree.
*			RECURSIVE.
* ALG:	Traverse the Gram hierarchy under current Stub Concept.  Concepts
*			with no samples under them are ignored and assumed to be "grouper"
*			or "organizer" concepts. When a concept with samples is found,
*			one of the following structures is assumed in the hierarchy.
*				CONCEPT (with samples)
*				CONCEPT (with samples) LABEL (with samples)
*			System will ignore any concepts underneath a LABEL concept, for
*			now.
* TODO:	Could use the level under LABEL for a REF, or concept that refers
*			to a system concept or something already built by user.  Then
*				CONCEPT - LABEL - REF
*			would be another alternative.
* WARN:	MODIFIES THE ANALYZER SEQUENCE AS IT PROGRESSES!
********************************************/

bool
RUG::genTreeRec(
	CONCEPT *tree,		// Current concept in hierarchy.
	bool flag,			// True if root of subtree.
	Mode *mode,			// Information object.		// 07/19/99 AM.
	bool justDirty,	// If only doing dirty concepts.		// 08/25/99 AM.
	bool skipLocks,	// If skipping locked concepts.		// 08/25/99 AM
	/*DU*/
	APASS* &seq			// The last pass generated by genTree.
	)
{
if (!tree)
	return true;

bool ok = true;

// Make new info structure.
Mode nmode(*mode);
updateMode(&nmode, tree);

if (cg_->findPhrase(tree))
	{
	// If current concept is a "Rule Concept", ie something with samples
	// under it, then treat this as a CONCEPT-LABEL subhierarchy.
//	ok = genxConcept(tree, /*DU*/ seq);				// 06/26/99 AM.
	ok = genoConcept(tree, &nmode,justDirty,skipLocks, /*DU*/ seq);
	}
else
	{
	// Then current concept is a GROUPER (or ORGANIZER) concept.
	// Todo: Any handling of Grouper attributes, etc. here.

	// Zap rules file, in case this concept lost its samples. // 08/29/99 AM.
	zapFile(tree);				// 08/29/99 AM.
	//cg_->rmVal(tree, "passnum");	// Zero out fm concept.		// 09/24/99 AM.
	cg_->rmAttr(tree,_T("passnum"));	// FIX.								// 06/04/00 AM.

	// Traverse children.
	ok = genTreeRec(cg_->Down(tree), false, &nmode, justDirty,skipLocks,
										 /*DU*/ seq);

	// Generate for current concept.
	//genConcept(tree, /*DU*/ seq);	// Old way.		// 06/23/99 AM.
	}

// Could free up mode structure here.

if (!ok)	// 05/20/09 AM.
	return ok;	// 05/20/09 AM.

// If at start of list, traverse rest of list.
if (!flag && !cg_->Prev(tree))
	{
	CONCEPT *conc;
	for (conc = cg_->Next(tree); conc; conc = cg_->Next(conc))
		{
		ok = genTreeRec(conc, false, mode, justDirty,skipLocks, /*DU*/ seq);
		if (!ok)	// 05/20/09 AM.
			return ok;	// 05/20/09 AM.
		}
	}
return ok;
}


/********************************************
* FN:		updateMode
* CR:		07/19/99 AM.
* SUBJ:	Update mode data for current Gram hierarchy concept.
* NOTE:	If an attribute has "true" or "false" value, update.
*			If attribute has no value, ignore it.
* ASS:	This gets a copy of mode object to modify at will.
********************************************/

bool
RUG::updateMode(
	Mode *mode,
	CONCEPT *conc
	)
{
if (!mode || !conc)
	return false;

_TCHAR val[MAXSTR];
val[0] = '\0';
cg_->findVal(conc, _T("lit"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setLit(true);
else if (!_tcscmp(val, _T("false")))
	mode->setLit(false);
val[0] = '\0';
cg_->findVal(conc, _T("constrain"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setConstrain(true);
else if (!_tcscmp(val, _T("false")))
	mode->setConstrain(false);
val[0] = '\0';
cg_->findVal(conc, _T("split"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setSplit(true);
else if (!_tcscmp(val, _T("false")))
	mode->setSplit(false);
val[0] = '\0';
cg_->findVal(conc, _T("gen"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setGen(true);
else if (!_tcscmp(val, _T("false")))
	mode->setGen(false);
val[0] = '\0';
cg_->findVal(conc, _T("opt"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setOpt(true);
else if (!_tcscmp(val, _T("false")))
	mode->setOpt(false);
val[0] = '\0';
cg_->findVal(conc, _T("closed"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setClosed(true);
else if (!_tcscmp(val, _T("false")))
	mode->setClosed(false);
val[0] = '\0';											// 08/09/99 AM.
cg_->findVal(conc, _T("xwhite"), /*UP*/ val);		// 08/09/99 AM.
if (!_tcscmp(val, _T("true")))							// 08/09/99 AM.
	mode->setXwhite(true);							// 08/09/99 AM.
else if (!_tcscmp(val, _T("false")))					// 08/09/99 AM.
	mode->setXwhite(false);							// 08/09/99 AM.
// LOCKED attribute.									// 08/25/99 AM.
val[0] = '\0';
cg_->findVal(conc, _T("locked"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	mode->setLocked(true);
else if (!_tcscmp(val, _T("false")))
	mode->setLocked(false);
val[0] = '\0';											// 12/27/99 AM.
cg_->findVal(conc, _T("quicksem"), /*UP*/ val);	// 12/27/99 AM.
if (!_tcscmp(val, _T("true")))							// 12/27/99 AM.
	mode->setQuicksem(true);						// 12/27/99 AM.
else if (!_tcscmp(val, _T("false")))					// 12/27/99 AM.
	mode->setQuicksem(false);						// 12/27/99 AM.
val[0] = '\0';											// 11/27/00 AM.
cg_->findVal(conc, _T("counter"), /*UP*/ val);	// 11/27/00 AM.
if (!_tcscmp(val, _T("true")))							// 11/27/00 AM.
	mode->setCounter(true);							// 11/27/00 AM.
else if (!_tcscmp(val, _T("false")))					// 11/27/00 AM.
	mode->setCounter(false);						// 11/27/00 AM.
val[0] = '\0';											// 11/27/00 AM.
cg_->findVal(conc, _T("notrig"), /*UP*/ val);		// 11/27/00 AM.
if (!_tcscmp(val, _T("true")))							// 12/08/00 AM.
	mode->setNotrig(true);							// 12/08/00 AM.
else if (!_tcscmp(val, _T("false")))					// 12/08/00 AM.
	mode->setNotrig(false);							// 12/08/00 AM.

// DIRTY attribute.									// 08/27/99 AM.
// NOTE:	Gets special handling!  It's a one-way on flag.
// Can't turn it off down the hierarchy.  Also, as we traverse down
// the concept tree, we'll turn it off wherever we find it on.
val[0] = '\0';
cg_->findVal(conc, _T("dirty"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	{
	mode->setDirty(true);
	// GET RID OF DIRTY FLAG IN CURRENT NODE!
	cg_->rmVals(conc, _T("dirty"));			// FIX	// 06/04/00 AM.
	}
else if (val[0])						// If it has any other value.
	cg_->rmVals(conc, _T("dirty"));		// Remove it.	// FIX	// 06/04/00 AM.

// LAYERED NAMES.														// 12/10/99 AM.
// Handling only one value, for now, though mode can take a whole list.
// (I suppose with the GUI we could handle a string of multiple vals.)
val[0] = '\0';															// 12/10/99 AM.
Dlist<Iarg> *list;													// 12/10/99 AM.
Delt<Iarg> *delt;														// 12/10/99 AM.
Iarg *arg;																// 12/10/99 AM.
_TCHAR *str;																// 12/10/99 AM.
cg_->findVal(conc,_T("layer"), /*UP*/ val);						// 12/10/99 AM.
if (val[0])		// User specified a value.						// 12/10/99 AM.
	{
	if ((list = mode->getLayer())									// 12/10/99 AM.
		 && (delt = list->getFirst())
		 && (arg = delt->getData())
		 && (arg->getType() == IASTR)
		 && (str = arg->getStr())
		 && _tcscmp(val, str)
		 )
		{
		// User has modified value, so update.
		// Need to get an interned string somehow.
		nlp_->internStr(val, /*UP*/ str);						// 12/10/99 AM.
		arg->setStr(str);
		}
	else		// First layering.									// 12/11/99 AM.
		{
		nlp_->internStr(val, /*UP*/ str);						// 12/11/99 AM.
		arg = new Iarg(str);											// 12/11/99 AM.
		list = new Dlist<Iarg>(arg);								// 12/11/99 AM.
		mode->setLayer(list);										// 12/11/99 AM.
		}
	}

return true;
}


/********************************************
* FN:		genoConcept
* CR:		06/26/99 AM.
* SUBJ:	Generate passes for the current concept in Gram hierarchy.
* NOTE:	Invoke analyzer for each sample.  One pass per sample.
*			SUPERSEDES genxConcept, which superseded genConcept...
*			08/28/99 AM. Note that the CALLER resets the dirty flag
*			(in updateMode).
* ASS:	Files from prior generation are zapped or can be overwritten.
* OPT:	Checks for multiple samples from a single document.
* WARN:	MODIFIES THE ANALYZER SEQUENCE.  ADDS NEW PASSES.
* ALG:	For each sample, get its parse.  Traverse subsequent passes
*			looking for those with the same input document.
* HOOK:	Hooks for textless samples.
*			Hooks for generalizing rules.
*			Hooks for a control structure that says how to build rules and
*			how to generalize rules.
********************************************/

bool
RUG::genoConcept(
	CONCEPT *conc,		// Current concept in Gram hierarchy.
	Mode *mode,
	bool justDirty,		// If only doing dirty concepts.		// 08/25/99 AM.
	bool skipLocks,		// If skipping locked concepts.		// 08/25/99 AM
	/*DU*/
	APASS* &seq			// The last pass generated so far.
	)
{
if (!conc)
	return true;
PHRASE *samples;		// Phrase of samples under concept.
CONCEPT *sample;
_TCHAR conname[1024];
_TCHAR buf[1024];

cg_->conceptName(conc, conname);
std::_t_cerr << _T("[genoConcept: Concept=") << conname << _T("]") << std::endl;

// Get concept's samples.
if (!(samples = cg_->findPhrase(conc)))
	{
	zapFile(conname);								// 08/29/99 AM.
	cg_->rmVals(conc, _T("passnum"));				// 09/24/99 AM.	// FIX	// 06/04/00 AM.
	std::_t_cerr << _T("[Concept has no samples.]") << std::endl;
	return true;
	}

APASS *rugpass = 0;				// Newly generated pass.

// Get pass number of pass that we are given.	// 09/24/99 AM.
int pnum = Aseq::getPassnum(seq);					// 09/24/99 AM.
++pnum;		// Pass number for current pass.		// 09/24/99 AM.

// If only processing dirty concepts and this isn't dirty.
// set up to read existing pass file.		// 08/25/99 AM.
if (justDirty)										// 08/25/99 AM.
	{
	bool dirty = mode->getDirty();

	if (!dirty)
		{
		// Set up to read rules file here.
		// Attach pass to analyzer sequence.
		rugpass = Aseq::insertAfter(nlp_, seq,
							_T("nlp"),			// Algorithm name.
							conname,			// Concept name == pass file name.
							_T("RUG file!"),	// Comment.
							true);			// An active pass.
		seq = rugpass;						// Move forward one pass.
		Aseq::setPassnum(seq, pnum);										// 09/24/99 AM.
		cg_->replaceVal(conc, _T("passnum"),(long long)pnum);					// 03/19/03 AM.
		// Intern this pass and rules file.								// 09/01/99 AM.
		return nlp_->internPass(seq, erfa_);							// 05/26/01 AM.
		}
	else
		{
		// Fall through.
		}
	}

// If skipping locked concepts and this is locked,
// set up to read existing pass file.		// 08/25/99 AM.
else if (skipLocks)								// 08/25/99 AM.
	{
	bool locked = mode->getLocked();

	if (locked)
		{
		// Set up to read rules file here.
		// Attach pass to analyzer sequence.
		rugpass = Aseq::insertAfter(nlp_, seq,
							_T("nlp"),			// Algorithm name.
							conname,			// Concept name == pass file name.
							_T("RUG file!"),	// Comment.
							true);			// An active pass.
		seq = rugpass;						// Move forward one pass.
		Aseq::setPassnum(seq, pnum);										// 09/24/99 AM.
		cg_->replaceVal(conc, _T("passnum"),(long long)pnum);					// 03/19/03 AM.
		// Intern this pass and rules file.								// 09/01/99 AM.
		return nlp_->internPass(seq, erfa_);							// 05/26/01 AM.
		}
	// else fall through.
	}

//cg_->rmVal(conc, "dirty");		// Reset dirty attribute.
// 08/28/99 AM. Note that caller resets dirty flag in updateMode.

_TCHAR input[MAXSTR];				// Input file to analyze for sample.
input[0] = '\0';

Parse *parse = 0;						// The current analysis instance.
int num = 0;							// Track pass number.

ARULES *rules;							// List of rules for current concept.
rules = Arug::makeRules();			// Make empty list of rules.

ASELECTS *selects;					// List of context node names. @NODES.
selects = Arug::makeSelects();	// Make empty list of selects.

CONCEPT *mark = 0;				// Mark next sample with different doc.
long long val = 0;						// Samples with doc flagged as redundant.

// Closed-set attribute from up the Gram hierarchy.
//bool closed = false;				// 08/09/99 AM.
//closed = mode->getClosed();		// 08/09/99 AM.

// Traverse list of samples.
// Build and collect rules and a list of the phrase parents (for use in
// @NODES).
// Every sample with an analyzed text is flagged with a "geno" attr.
// Next sample with an unanalyzed text is pointed to by mark variable.

for (sample = cg_->firstNode(samples); sample; sample = mark)
//sample = cg_->Next(sample))
	{
	cg_->conceptName(sample, buf);
	std::_t_cerr << _T("  Sample=") << buf << std::endl;

	// If sample is flagged as having an input document that has been
	// processed, turn off the flag and continue.
	if (cg_->findVal(sample, _T("geno"), /*UP*/ val))
		{
		cg_->rmVals(sample, _T("geno"));		// Remove the value.	// FIX	// 06/04/00 AM.
		mark = cg_->Next(sample);			// Get next sample.
		continue;
		}

	// Now we're at a sample with unanalyzed doc.

	// Reset mark to make sure loop terminates, if there's an error
	// further down.
	mark = 0;

	//////////////////////////////
	// PARTIALLY PARSE CURRENT DOC
	//////////////////////////////

	if (!sampleFile(sample, /*UP*/ input))	// GET INPUT FILENAME.
		{
		std::_t_cerr << _T("[Sample has no filename]") << std::endl;
		// Treating no document as a normal case now.
		}
	// See if file is empty.	// 05/21/09 AM.
	else if (file_size(input) <= 3)	// 05/21/09 AM.
		{
		// Fall through, ignore this file.	// 05/21/09 AM.
		}
	// Partially parse the document.
	else
		{
		// Analyze the sample's entire document UP TO THE CURRENT PASS.

		// Start up a parse and get a handle for it.
		// Use eana config.			// 10/13/99 AM.
		parse = nlp_->initAnalyze(input,output_, appdir_, erfa_, outdir_,
			0,0,0,0,0,true);	// Retain hash,sym tables.	// 05/21/09 AM.
		if (!parse)
			{
			std::_t_cerr << _T("[genoConcept: Couldn't initialize analyzer.]") << std::endl;
			Arug::rmRules(rules);
			Arug::deleteSelects(selects);	// MEM LEAK	// 05/21/09 AM.
			return false;
			}

		num = 1;	// Reset pass number counter.
		nlp_->partExecute(parse, Aseq::firstPass(nlp_), seq, /*DU*/ num);
		}

	long long start, end;

	/////////////////////////
	// Handle current sample.
	/////////////////////////
	cg_->findVal(sample, _T("start"), /*UP*/ start);
	cg_->findVal(sample, _T("end"), /*UP*/ end);
	Arug::genxSample(input, start, end,
							buf, // Sample text.			// 06/16/99 AM.
							conc,
							(APARSE *) parse, conname, mode, cg_,
														/*DU*/ rules, selects);

	/////////////////////////////////////
	// FIND ALL SAMPLES WITH SAME DOC
	/////////////////////////////////////
	// Traverse ALL samples from current one on, looking for same-doc.
	// Mark the first sample that has a text different from current
	// and that has not been flagged as processed (with "geno" attr).
	// From that point on, flag the same-doc samples with "geno" attr.

	/////////////////////////////////////
	bool first;					// Flag first difference pending.
	first = true;
	CONCEPT *samp;
	for (samp = cg_->Next(sample); samp; samp = cg_->Next(samp))
		{
		if (cg_->findVal(samp, _T("geno"), /*UP*/ val))	// SAMPLE WAS PROCESSED.
			{
			if (first)		// Still in completely analyzed sub-sequence.
				{
				// Turn off the processed flag, because we will not visit
				// this sample again.
				cg_->rmVals(samp, _T("geno"));	// FIX	// 06/04/00 AM.
				}
			}
		else if (sameDoc(input, samp))		// SAMPLE FROM SAME DOCUMENT.
			{
			if (first)		// Still in completely analyzed sub-sequence.
				{
				// Turn off the processed flag, because we will not visit
				// this sample again.
				cg_->rmVals(samp, _T("geno"));	// FIX	// 06/04/00 AM.
				}

			// Set up data for current sample.
			cg_->conceptName(samp, buf);
			std::_t_cerr << _T("  Samp=") << buf << std::endl;

			// Generate rule for current sample.
			cg_->findVal(samp, _T("start"), /*UP*/ start);
			cg_->findVal(samp, _T("end"), /*UP*/ end);
			Arug::genxSample(input, start, end,
									buf, // Sample text.			// 06/16/99 AM.
									conc,
									(APARSE *) parse, conname, mode, cg_,
																/*DU*/ rules, selects);
			}
		else if (first)	// Now at a first sample with an unanalyzed doc.
			{
			mark = samp;	// Continue main loop from here.
			first = false;
			}
		}

	// Remove the data for doc that was analyzed, if any.
	if (parse)
		{
		// Clean up the text analysis instance.
		nlp_->cleanAnalyze(parse,
									true);	// RETAIN HASH,SYM TABLE.		// 07/03/03 AM.
		parse = 0;
		}

	// Return pass.
	}					// END OF TRAVERSE SAMPLES LOOP.


// Get the rules built for all samples.
// Get list of parent concepts (for @NODES).
// Merge rules here. (May need the whole parse tree phrases, in the
// general case.  Not sure....)

if (!rules)
	return true;			// Don't generate pass if no rules for concept.

// Remove duplicates.		// 06/07/99 AM.
Arug::dedup(rules);

// Generalize and merge rules!			// 06/28/99 AM.
Arug::generalize(rules, mode);

// Remove duplicates AGAIN.				// 07/03/99 AM.
Arug::dedup(rules);							// 07/03/99 AM.


Arug::prettyRules(rules);

// Make sure pass file name doesn't conflict with existing filenames.
// (If regeneration, WILL conflict.  So need to wipe out all generated
// filenames when regenerating.)  Safer to put generated files in a
// separate folder.

// Build pass, attach rules, context, attach to analyzer,
// generate file for pass, etc.

// Build the pass.

// Attach pass to analyzer sequence.
rugpass = Aseq::insertAfter(nlp_, seq,
					_T("nlp"),			// Algorithm name.
					conname,			// Concept name == pass file name.
					_T("RUG file!"),	// Comment.
					true);			// An active pass.
// Attach rules, selects to pass.
Arug::fillPass(rugpass, rules, selects);

// Intern pass.

// Build file path for pass.
_TCHAR buff[MAXPATH*2];
_stprintf(buff, _T("%s%c%s%c%s.pat"), appdir_,DIR_CH,
			SPECDIRNAME,														// 05/14/00 AM.
			DIR_CH, conname);

// Generate rules file for pass.
Arug::genRulesfile(rugpass, buff);

seq = rugpass;			// Return the new pass as current stopping
							// point in analyzer.
Aseq::setPassnum(seq, pnum);				// 09/24/99 AM.
cg_->replaceVal(conc, _T("passnum"),(long long)pnum);	// 03/19/03 AM.

return true;
}


/********************************************
* FN:		zapFile
* CR:		08/29/99 AM.
* SUBJ:	Remove concept's file if present.
* RET:	True if ok.
* NOTE:	For example, if pruned all the samples under a concept, need to
*			get rid of the old rules file.
********************************************/

bool RUG::zapFile(_TCHAR *conname)
{
if (!conname || !*conname)
	return false;

// Build file path for pass.
_TCHAR buff[MAXPATH*2];
_stprintf(buff, _T("%s%c%s%c%s.pat"), appdir_,DIR_CH,
			SPECDIRNAME,														// 05/14/00 AM.
			DIR_CH, conname);

// Remove file if present.
remove_path(buff);

return true;
}


/********************************************
* FN:		zapFile
* CR:		08/29/99 AM.
* SUBJ:	Remove concept's file if present.
* RET:	True if ok.
* NOTE:	Variant if conname isn't around.
********************************************/

bool RUG::zapFile(CONCEPT *conc)
{
if (!conc)
	return false;
_TCHAR conname[1024];
cg_->conceptName(conc, conname);

return zapFile(conname);
}

/********************************************
* FN:		sampleFile
* CR:		06/26/99 AM.
* SUBJ:	Get input file for given sample concept.
* RET:	True if sample has a file.
* NOTE:	Not having a file is ok ("textless samples") but returns false.
* TODO:	Change filenames from absolute path to relative path.
********************************************/

bool
RUG::sampleFile(
	CONCEPT *sample,
	/*UP*/
	_TCHAR *input				// User-supplied buffer.
	)
{
*input = '\0';				// Reset buffer.

if (!sample)
	return false;

// GET THE INPUT FILE.  FOR NOW, TAKE AS IS.
// THIS HANDSHAKE WITH GUI MAY CHANGE.  Putting in the hook....
//char fname[MAXSTR];				// Buffer for sample's input file.
//fname[0] =  '\0';
//cg_->findVal(sample, "text", /*UP*/ fname);	// TEXT attribute.
_TCHAR *fname = cg_->strVal(sample, _T("text"));				// OPT.	// 11/12/00 AM.
//if (!fname[0])
if (!fname || !*fname)														// 11/12/00 AM.
	{
	std::_t_cerr << _T("[Sample has no filename]") << std::endl;
	// Treating no document as a normal case now.
	return false;
	}

// Cool!  Moving to relative files finally!  // 99/09/09 AM.
// Old. For absolute file names.
//strcpy(input, fname);		// 99/09/09 AM.

// Handshake for relative file names.
// DAVE'S RELATIVE PATH INCLUDES THE APP SUBDIRECTORY.		// 99/09/09 AM.
//sprintf(input, "%s\\input\\%s", cg_->getAppdir(), fname); // 99/09/09 AM.
//sprintf_s(input, "%s\\input\\%s", cg_->getAppdir(), fname); // 99/09/09 AM.
_stprintf(input, _T("%s%c%s"), cg_->getAppdir(),DIR_CH, fname); // 99/09/09 AM.

#ifdef QDBM_
cg_->kbfree(fname);	// 02/14/07 AM.
#endif

return true;
}


/********************************************
* FN:		sameDoc
* CR:		06/26/99 AM.
* SUBJ:	See if current doc is same as sample's doc.
* RET:	True if the same document (or both textless).
********************************************/

bool
RUG::sameDoc(
	_TCHAR *fname,			// Current input document.
	CONCEPT *sample		// Sample whose document is being compared.
	)
{
if (!sample)
	return false;			// Erroneous.

_TCHAR buf[MAXSTR];
buf[0] = '\0';

if (sampleFile(sample, /*UP*/ buf))		// Sample has file.
	{
	if (!strcmp_i(fname, buf))				// Case-insensitive.
		return true;
	}
else												// Sample is text-less.
	{
	if (!fname || !*fname)
		return true;							// No text here either.
	}
return false;
}


/*************************      END OF FILE      ******************************/
