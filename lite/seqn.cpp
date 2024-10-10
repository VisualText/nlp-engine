/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SEQN.CPP
* FILE:	lite\seqn.cpp
* CR:		10/13/98 AM.
* SUBJ:	List of passes, implemented reusing tree and node classes.
*
*******************************************************************************/
#include "StdAfx.h"
#include "machine.h"			// 03/08/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/mach.h"			// today()	// 11/04/99 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"						// 05/19/99 AM.
#include "io.h"
#include "node.h"				// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "irule.h"
//#include "lite/delt.h"
#include "gen.h"							// 05/10/00 AM.
#include "ifile.h"						// 11/25/98 AM.
#include "seqn.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Seqn>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Seqn>::count_ = 0;
#else
template<>	// Strange non-LINUX.
int Dlist<Seqn>::count_ = 0;
int Delt<Seqn>::count_ = 0;
#endif
//int Seqn::count_ = -1;			// 11/17/98 AM. Because of one static.
int Seqn::count_ = 0;				// 07/02/99 AM.

/********************************************
* FN:		Special Functions for Seqn Class
* CR:		10/13/98 AM.
********************************************/

Seqn::Seqn(Algo *aa, _TCHAR *rfile, Dlist<Irule> *rules, _TCHAR *name)
			// Default constructor.
{
algo = aa;
if (rfile)
	_tcscpy(rules_file_name_,rfile);
else
	rules_file_name_[0] = '\0';
rules_ = rules;
name_ = name;				// 11/24/98 AM.
rulesfile_ = 0;			// 11/25/98 AM.
displaytree_ = false;
active_ = true;			// 01/08/99 AM.
comment_[0] = '\0';		// 01/08/99 AM.
passnum_ = 0;				// 09/23/99 AM.
recid_ = 0;

// BUG IN INITIALIZING LITE DUE TO THE FUNCTION CALL HERE.
// Because a static Seqn object is created later in this file, and that
// one is given a NULL algo, the library couldn't properly initialize.
//algo_name_ = 0;					// 10/21/99 AM.
algo_name_[0] = '\0';	// 05/21/09 AM.
if (aa)								// FIX.				// 01/13/99 AM.
	{
	//algo_name_ = aa->getName();	// 01/08/99 AM.	// Prob change this.	// 05/21/09 AM.
	_tcscpy(algo_name_,aa->getName());	// 05/21/09 AM.
	}

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/

// New constructor for uninterned pass.
// 01/13/99 AM.
Seqn::Seqn(
	bool active,
	_TCHAR *algo_name,
	_TCHAR *rules_file_name,
	_TCHAR *comment
	)
{
active_ = active;
//algo_name_ = algo_name;	// 05/21/09 AM.
algo_name_[0] = '\0';	// 05/21/09 AM.
if (algo_name && *algo_name)	// 05/21/09 AM.
	_tcscpy(algo_name_,algo_name);	// 05/21/09 AM.
if (rules_file_name)
	_tcscpy(rules_file_name_, rules_file_name);
else
	rules_file_name_[0] = '\0';
if (comment && *comment)			// 02/12/99 AM.
	_tcscpy(comment_, comment);
algo = 0;
rules_ = 0;
rulesfile_ = 0;
displaytree_ = false;
passnum_ = 0;							// 09/23/99 AM.
recid_ = 0;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Seqn::~Seqn()
{
unintern();				// 01/15/99 AM.

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Seqn &seqn)
{
// Need some way to print each pass.
Algo *aa;
_TCHAR *name = _T("");
_TCHAR *rfile;
aa = seqn.algo;
rfile = seqn.rules_file_name_;
if (aa)								// 01/14/99 AM.
	name = aa->getName();
if (empty(rfile))
	output << name;
else
	output << _T("(") << name << _T(",") << rfile << _T(")");
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/13/98 AM.
********************************************/

Algo			 *Seqn::getAlgo()				{return algo;			}
_TCHAR			 *Seqn::getRulesfilename()	{return rules_file_name_; }
Dlist<Irule> *Seqn::getRules()			{return rules_;		}
_TCHAR			 *Seqn::getName()				{return name_;			}
Ifile			 *Seqn::getRulesfile()		{return rulesfile_;	}
bool			  Seqn::getActive()			{return active_;		}
bool			  Seqn::getDisplaytree()	{return displaytree_;	}
_TCHAR			 *Seqn::getComment()			{return comment_;		}
_TCHAR			 *Seqn::getAlgoname()		{return algo_name_;	}
int			  Seqn::getPassnum()			{return passnum_;		} // 09/23/99 AM.
int			  Seqn::getRecid()			{return recid_;		}

Dlist<Iarg>	*Seqn::getSelects()			// 11/30/98 AM.
{
if (!rulesfile_)
	return 0;
return rulesfile_->getSelects();
}


/********************************************
* FN:		Modify Functions
* CR:		10/13/98 AM.
********************************************/

void Seqn::setAlgo(Algo *x)				{algo			= x;}
void Seqn::setRulesfilename(_TCHAR *x)
{
if (x)
	_tcscpy(rules_file_name_, x);
else
	rules_file_name_[0] = '\0';
}
void Seqn::setRules(Dlist<Irule> *x)	{rules_		= x;}
void Seqn::setName(_TCHAR *x)				{name_		= x;}
void Seqn::setRulesfile(Ifile		*x)	{rulesfile_	= x;}
void Seqn::setActive(bool			 x)	{active_		= x;}
void Seqn::setDisplaytree(bool x)		{displaytree_ = x;}
void Seqn::setPassnum(int x)				{passnum_	= x;}	// 09/23/99 AM.
void Seqn::setRecid(int x)					{recid_		= x;}
void Seqn::setComment(_TCHAR			*x)
{
if (x && *x)																	// 11/12/00 AM.
	_tcscpy(comment_, x);														// 11/12/00 AM.
else
	comment_[0] = '\0';														// 11/14/00 AM.
}
void Seqn::setAlgoname(_TCHAR		*x)
	{
	//	algo_name_	 = x;	// 05/21/09 AM.
	algo_name_[0] = '\0';	// 05/21/09 AM.
	if (x && *x)	// 05/21/09 AM.
		_tcscpy(algo_name_,x);	// 05/21/09 AM.
	}


void Seqn::setSelects(Dlist<Iarg> *x)		// 11/30/98 AM.
{
if (rulesfile_)
	rulesfile_->setSelects(x);
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Seqn::setSelects -- no pass file.]") << std::ends;
	errOut(&gerrStr,false);
	}

}


/********************************************
* FN:		GETCOUNT
* CR:		10/13/98 AM.
********************************************/
#ifndef STABLE_
int Seqn::getCount() { return count_; }
#endif


/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Seqn::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Seqn count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Seqn count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// GENERAL FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		genSeqn
* CR:		11/04/99 AM.
* SUBJ:	Generate rules file for one pass in analyzer sequence.
* NOTE:	Need to handle selects, recurse regions, code, etc.
********************************************/

bool Seqn::genSeqn(
	_TCHAR *dir			// Directory for writing rules file.
	)
{
if (!rules_)
	return false;
if (_tcscmp(algo_name_, _T("nlp")))
	return true;		// Rules file only for PAT algorithm.
if (!rules_file_name_ || !*rules_file_name_)
	return false;		// PAT should have rules file.

// Set up sequence filename.
_TCHAR fname[MAXSTR];
//ostrstream *outStr = 0;													// 03/08/00 AM.
//ofstream *fout = 0;														// 03/08/00 AM.
//outStr = new ostrstream(fname, MAXSTR, std::ios::out);				// 03/08/00 AM.
//*outStr << dir << "\\" << rules_file_name_ << ".nlp" << std::ends; // 03/08/00 AM.
//fout = new ofstream(fname, std::ios::out);								// 03/08/00 AM.

#ifdef UNICODE
wsprintf(fname, _T("%s%c%s%s"), dir, DIR_CH, rules_file_name_, _T(".nlp"));
#elif LINUX
sprintf(fname, _T("%s%c%s%s"), dir, DIR_CH, rules_file_name_, _T(".nlp"));
#else
sprintf_s(fname, _T("%s%c%s%s"), dir, DIR_CH, rules_file_name_, _T(".nlp"));
//_t_ostrstream outStr(fname, MAXSTR, std::ios::out);							// 03/08/00 AM.
//outStr << dir << DIR_CH << rules_file_name_ << _T(".nlp") << std::ends; // 03/08/00 AM.
#endif
std::_t_ofstream fout(TCHAR2CA(fname), std::ios::out);											// 03/08/00 AM.


_TCHAR fhead[1024];
_TCHAR upp[MAXSTR];
str_to_upper(rules_file_name_, upp);
_stprintf(fhead, _T("%s.NLP (pass %d)"), upp, passnum_);

// Write out file header.
genFileheader(
	fhead,					// Filename.
	_T("Automatically generated RFA file."),	// Subject.
	_T("VisualText"),			// Author.
#ifndef LINUX
	today1(),					// Date.	// WORKAROUND	// 02/25/00 AM.
#else
	today(),					// Date.	// WORKAROUND	// 02/25/00 AM.
#endif
	_T(""),						// Modified.
	_T(""),						// Note.
//	*fout);																		// 03/08/00 AM.
	fout);																		// 03/08/00 AM.


// Write out code region. (None in RFA).

// Write out select region. (None in RFA).

// Write out recurse regions, based on interned rules.
//Irule::genRulerecurses(rules_, " ", *fout);						// 03/08/00 AM.
Irule::genRulerecurses(rules_, _T(" "), fout);							// 03/08/00 AM.

// Write out rule regions for file.
//Irule::genRuleblocks(rules_, " ", *fout);							// 03/08/00 AM.
Irule::genRuleblocks(rules_, _T(" "), fout);								// 03/08/00 AM.

//*fout << std::endl;				// Final cr just in case.				// 03/08/00 AM.
fout << std::endl;				// Final cr just in case.					// 03/08/00 AM.

//delete fout;
//delete outStr;	// FIX MEM LEAK.										// 02/25/00 AM.
return true;
}


/********************************************
* FN:		genFileheader
* CR:		11/04/99 AM.
* SUBJ:	Generate rules file header.
********************************************/

void Seqn::genFileheader(
	_TCHAR *fname,
	_TCHAR *subject,
	_TCHAR *author,
	_TCHAR *date,
	_TCHAR *modified,
	_TCHAR *note,
	std::_t_ostream &ofile
	)
{

ofile << _T("###############################################\n")
		<< _T("# FILE:     ") << fname    << _T("\n")
		<< _T("# SUBJ:     ") << subject  << _T("\n")
		<< _T("# AUTH:     ") << author   << _T("\n")
		<< _T("# CREATED:  ") << date     << _T("\n")
		<< _T("# MODIFIED: ") << modified << _T("\n")
		<< _T("# NOTE:     ") << note	   << _T("\n")
		<< _T("###############################################")
		<< std::endl;
}

///////////////////////////////////////////////////////////////////
// CLASS-WIDE FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		makeDelt
* CR:		10/13/98 AM.
* SUBJ:	Create a list element with appropriate data.
* NOTE:	Must be a more elegant way to initialize the elt.
*			10/17/98 Renamed, using Delt instead of Node.
********************************************/
//static Seqn ss;					// 07/02/99 AM.

Delt<Seqn> *Seqn::makeDelt(Algo *aa, _TCHAR *rfile, Dlist<Irule> *rules,
	Ifile *rulesfile)
{
Delt<Seqn> *delt;
Seqn *ss;				// 07/02/99 AM.

ss = new Seqn();		// 07/02/99 AM.

ss->algo = aa;
if (!empty(rfile))
	_tcscpy(ss->rules_file_name_, rfile);
else
	ss->rules_file_name_[0] = '\0';
ss->rules_ = rules;
ss->rulesfile_ = rulesfile;		// 11/25/98 AM.
delt = new Delt<Seqn>(ss);

// 07/02/99 AM. OLD WAY.
// Bug fix. 11/16/98 AM.
//ss.algo = 0;
//ss.rules_ = 0;
//ss.name_ = 0;
//ss.rulesfile_ = 0;				// 11/25/98 AM.
//ss.active_ = true;				// 01/08/99 AM.
//ss.comment_[0] = '\0';			// 01/08/99 AM.
//ss.algo_name_ = 0;				// 01/08/99 AM.

return delt;
}


/********************************************
* FN:		makeDlist
* CR:		10/13/98 AM.
* SUBJ:	Create a list with appropriate data.
* NOTE:	Class member function.
*			(Can be called without an instance existing).
*			10/17/98 AM. Renamed. Using Dlist, not Tree.
********************************************/

Dlist<Seqn> *Seqn::makeDlist(
	Algo *aa,
	_TCHAR *rr,				// rules file name.
	Dlist<Irule> *rules,
	Ifile *rulesfile			// 11/25/98 AM.
	)
{
Delt<Seqn> *delt;
delt = makeDelt(aa, rr, rules, rulesfile);
return new Dlist<Seqn>(delt);
}


/********************************************
* FN:		UNINTERN
* CR:		01/15/99 AM.
* SUBJ:	Unintern one pass in the analyzer sequence.
* NOTE:	To help with analyzer sequence editing.
*			Moved stuff here from the ~Seqn destructor.
********************************************/

void Seqn::unintern()
{
if (algo)
	delete algo;

// 11/25/98 AM. Pass (Seqn) no longer owns its rules.  The rulesfile object
// now owns them.
if (rulesfile_)
	{
	delete rulesfile_;									// 11/25/98 AM.
	rulesfile_ = 0;										// 11/25/98 AM.
	}

//if (rules_)												// 11/24/98 AM.
//	Dlist<Irule>::DeleteDlistAndData(rules_);		// 11/24/98 AM.
algo = 0;
rules_ = 0;

}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	If lists of passes are equivalent.
********************************************/

bool Seqn::same(Dlist<Seqn> *seqn1, Dlist<Seqn> *seqn2)
{
if (!seqn1 && !seqn2)
	return true;
if (!seqn1 || !seqn2)
	return false;

// Compare pass-by-pass.
Delt<Seqn> *d1, *d2;
for (d1 = seqn1->getFirst(), d2 = seqn2->getFirst();
	  d1, d2;
	  d1 = d1->Right(), d2 = d2->Right())
	{
	if (!same(d1->getData(), d2->getData()))
		return false;
	}

if (d1 || d2)			// Unequal length.
	return false;

return true;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	If passes are equivalent.
* NOTE:	Looking only at rules list, not rulefile.  Assumes the rules
*			have been internalized (though not necessarily hashed).
*			Don't care if pass is active or not.
*			Not comparing file names and other irrelevant features.
*			Looking at equivalent functionality only.
********************************************/

bool Seqn::same(Seqn *seqn1, Seqn *seqn2)
{
if (!seqn1 && !seqn2)
	return true;
if (!seqn1 || !seqn2)
	return false;

if (!str_equal(seqn1->getAlgoname(), seqn2->getAlgoname()))
	return false;

if (!Irule::same(seqn1->getRules(), seqn2->getRules()))
	return false;
return true;
}


/********************************************
* FN:		GENRECURSES
* CR:		11/04/99 AM.
* SUBJ:	Generate list of recursive pass names.
********************************************/

bool Seqn::genRecurses(
	Dlist<Seqn> *seqs,
	_TCHAR *sep,
	std::_t_ostream &ofile
	)
{
if (!seqs)
	return true;
Delt<Seqn> *dseq;
Seqn *seq;
if (!(dseq = seqs->getFirst()))
	return true;
seq = dseq->getData();
ofile << seq->getName();
for (dseq = dseq->Right(); dseq; dseq = dseq->Right())
	{
	seq = dseq->getData();
	ofile << sep << seq->getName();
	}
return true;
}


/********************************************
* FN:		GEN
* CR:		05/10/00 AM.
* SUBJ:	Gen code for analyzer sequence.
********************************************/

bool Seqn::gen(
	Dlist<Seqn> *seqs,
	Gen *gen
	)
{
if (!seqs || !gen)
	return false;
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
if (!fcode || !fhead)
	return false;

// SET LENGTH OF SPECIALARR.												// 06/09/00 AM.
Arun::set_specialarr_len();												// 06/09/00 AM.


// Output prototype.
*fhead << _T("extern \"C\" bool run_analyzer(Parse *);");
Gen::nl(fhead);																// 04/04/03 AM.

// Output function head.
*fcode << _T("bool run_analyzer(Parse *parse)");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("if (!Arun::init_analyzer(parse)) return false;");
Gen::nl(fcode);																// 04/04/03 AM.

int count = 0;
Delt<Seqn> *dseq;
Seqn *seq;
_TCHAR *name;
bool active = false;															// 08/04/02 AM.
for (dseq = seqs->getFirst(); dseq; dseq = dseq->Right())
	{
	// Should have enum type for algorithm type.
	seq = dseq->getData();
	name = seq->getAlgoname();												// 06/04/00 AM.
	active = seq->getActive();												// 08/04/02 AM.
	++count;
	if (!_tcscmp(_T("nlp"), name))
		{
		if (active)																// 08/04 02 AM.
			*fcode << _T("patExecute") << count << _T("(parse);");
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	else if (!_tcscmp(_T("rec"), name))										// 05/31/00 AM.
		{
		if (active)																// 08/04/02 AM.
		 *fcode << _T("recExecute") << count << _T("(parse);");				// 05/31/00 AM.
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	else if (!_tcscmp(_T("stub"),name) || !_tcscmp(_T("end"),name)			// 06/04/00 AM.
		|| !_tcscmp(_T("folder"),name))									// 08/24/08 AM.
		;			// Ignore these.											// 06/04/00 AM.
	else
		{
		*fcode << name << count << _T("(parse);");
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	}

*fcode << _T("return true;");
		Gen::nl(fcode);														// 04/04/03 AM.
*fcode << _T("}");
		Gen::eol(fcode);														// 04/04/03 AM.
return true;
}


/********************************************
* FN:		GENRECURSE
* CR:		05/31/00 AM.
* SUBJ:	Gen code for recurse minipass calls.
********************************************/

bool Seqn::genRecurse(
	Dlist<Seqn> *seqs,
	Gen *gen
	)
{
if (!seqs || !gen)
	return false;
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
_TCHAR *indent = gen->getIndent();
int id = gen->getId();
int eltid = gen->getEltid();
if (!fcode || !fhead)
	return false;

Delt<Seqn> *dseq;
Seqn *seq;
for (dseq = seqs->getFirst(); dseq; dseq = dseq->Right())
	{
	seq = dseq->getData();
	*fcode << indent
			 << _T("Arun::recurse(")
			 << eltid << _T(",")
			 << _T("recurse")
			 << id << _T("_")
			 << seq->getRecid()		// Number of the recurse minipass.
			 << _T(",nlppp);");
	Gen::nl(fcode);															// 04/04/03 AM.
	}

return true;
}
