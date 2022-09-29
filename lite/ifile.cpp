/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IFILE.CPP
* FILE:	lite\ifile.cpp
* CR:		11/24/98 AM.
* SUBJ:	Class for a named set of rule regions in a rules file.
* NOTE:	For recursive mini-passes invoked by rule elements.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "lite/mach.h"	// today()	// 04/03/09 AM.
#include "lite/inline.h"	// Linux.	// 04/26/07 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "io.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
//#include "nlppp.h"	// 07/07/03 AM.
#include "iaction.h"															// 12/07/98 AM.
#include "irecurse.h"
#include "iregion.h"
#include "htab.h"																// 12/16/98 AM.
#include "thtab.h"															// 12/17/98 AM.
#include "gen.h"																// 05/08/00 AM.
#include "parse.h"															// 05/31/00 AM.
#include "ana.h"																// 05/31/00 AM.
#include "ifunc.h"															// 12/19/01 AM.
#include "ifile.h"

// Ad hoc cleanup of rules list in each hash table sym.
void cleanRules(tHtab<Slist<Irule> > *htab);

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Slist<Ifile>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Selt<Ifile>::count_  = 0;
#else
int Slist<Ifile>::count_ = 0;
int Selt<Ifile>::count_  = 0;
#endif
int Ifile::count_	= 0;


/********************************************
* FN:		Special functions for the class
********************************************/

Ifile::Ifile(			// Default constructor.
	Dlist<Iarg>		 *decls,				// 12/19/01 AM.
	Dlist<Iaction>	 *codes,				// 12/07/98 AM.
	Dlist<Iarg>		 *selects,			// 11/30/98 AM.
	Slist<Irecurse> *recurses,
	Slist<Iregion>  *regions,
	bool fmulti,							// 03/25/99 AM.
	bool fpath,								// 10/19/99 AM.
	bool fpremulti							// 10/25/99 AM.
	)
{
decls_		= decls;			// 12/19/01 AM.
codes_		= codes;			// 12/07/98 AM.
selects_		= selects;		// 11/30/98 AM.
recurses_	= recurses;
regions_		= regions;
fmulti_		= fmulti;		// 03/25/99 AM.
fpath_	   = fpath;			// 10/19/99 AM.
fpremulti_	= fpremulti;		// 10/25/99 AM.
rules_ = 0;
seqs_  = 0;						// 11/25/98 AM.
htab_  = 0;						// 12/16/98 AM.
musts_ = 0;						// 12/16/98 AM.
empty_ = false;				// 08/31/00 AM.
owning_ = false;				// 12/24/00 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

Ifile::~Ifile()
{
if (decls_)																		// 12/19/01 AM.
	Dlist<Iarg>::DeleteDlistAndData(decls_);							// 12/19/01 AM.

if (codes_)																		// 12/07/98 AM.
	Dlist<Iaction>::DeleteDlistAndData(codes_);

if (selects_)
	Dlist<Iarg>::DeleteDlistAndData(selects_);						// 11/30/98 AM.

if (recurses_)
	delete (Slist<Irecurse> *) recurses_;

if (regions_)
	delete (Slist<Iregion> *) regions_;

// Ifile owns and deletes the rules removed from its regions
// upon internalizing.
if (rules_)
	{
	// Rules created by RUG own their actions.						// 12/24/00 AM.
	if (owning_)																// 12/24/00 AM.
		{
		Irule::deleteOwning(rules_);	// Delete actions.			// 12/24/00 AM.
		owning_ = false;					// RESET THIS.					// 12/24/00 AM.
		}
	Dlist<Irule>::DeleteDlistAndData(rules_);
	rules_ = 0;
	}
if (seqs_)						// 11/25/98 AM.
	{
	Dlist<Seqn>::DeleteDlistAndData(seqs_);
	seqs_ = 0;
	}

if (musts_)						// 12/16/98 AM.
	{
	// Make sure you don't delete the rules here!
	// Need a delete that DOES NOT delete the data pointer.
	Slist<Irule>::DeleteSlist(musts_);
	musts_ =0;
	}
if (htab_)						// 12/16/98 AM.
	{
	cleanRules(htab_);		// Ad hoc cleanup of rules lists.
	delete htab_;
	htab_ = 0;
	}

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ifile &ifile)
{
output << _T("@file") << std::endl;
if (ifile.codes_)							// 12/19/01 AM.
	{
	output << _T("@decl") << std::endl;
	output << *(ifile.decls_);
	output << _T("@@decl") << std::endl;
	}
if (ifile.codes_)							// 12/07/98 AM.
	{
	output << _T("@code") << std::endl;
	output << *(ifile.codes_);
	output << _T("@@code") << std::endl;
	}
if (ifile.selects_)
	{
	output << _T("@select") << std::endl;
	output << _T("@nodes")
			 << *(ifile.selects_) << std::endl
			 << _T("@@nodes") << std::endl;
	output << _T("@@select") << std::endl;
	}
if (ifile.recurses_)
	output << *(ifile.recurses_);
if (ifile.regions_)
	output << *(ifile.regions_);
output << _T("@@file") << std::endl;
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Selt<Ifile> &selt)
{
Ifile *dat;
dat = selt.getData();
output << *dat;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Slist<Ifile> &list)
{
Selt<Ifile> *selt;
Ifile *dat;
selt = list.getFirst();
dat   = selt->getData();
output << *dat << std::endl;
while ((selt = selt->Right()))
	{
	dat = selt->getData();
	output << *dat << std::endl;
	}

return output;
}
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

Slist<Irecurse> *Ifile::getRecurses()	{return recurses_;}
Slist<Iregion>	 *Ifile::getRegions()	{return regions_; }
Dlist<Irule>	 *Ifile::getRules()		{return rules_;	}
Dlist<Seqn>		 *Ifile::getSeqs()		{return seqs_;		}
Dlist<Iarg>		 *Ifile::getSelects()	{return selects_;	}
Dlist<Iarg>		 *Ifile::getDecls()		{return decls_;	}		// 12/19/01 AM.
Dlist<Iaction>	 *Ifile::getCodes()		{return codes_;	}		// 12/07/98 AM
Slist<Irule>	 *Ifile::getMusts()		{return musts_;	}		// 12/16/98 AM
tHtab<Slist<Irule> >	 *Ifile::getHtab()	{return htab_;	}		// 12/16/98 AM
bool				  Ifile::getFmulti()		{return fmulti_;	}		// 03/25/99 AM
bool				  Ifile::getFpath()		{return fpath_;	}		// 10/19/99 AM.
bool				  Ifile::getFpremulti()	{return fpremulti_;}		// 10/25/99 AM.
bool				  Ifile::getEmpty()		{return empty_;}			// 08/31/00 AM.
bool				  Ifile::getOwning()		{return owning_;}			// 12/24/00 AM.

/********************************************
* FN:		Modify Functions
********************************************/

void	Ifile::setRecurses(Slist<Irecurse> *x)	{recurses_ = x;}
void	Ifile::setRegions(Slist<Iregion>	  *x)	{regions_  = x;}
void	Ifile::setRules(Dlist<Irule>		  *x)	{rules_	  = x;}
void	Ifile::setSeqs(Dlist<Seqn>			  *x)	{seqs_	  = x;}
void	Ifile::setSelects(Dlist<Iarg>		  *x)	{selects_  = x;}
void	Ifile::setDecls(Dlist<Iarg>		  *x)	{decls_	  = x;}	// 12/19/01 AM.
void	Ifile::setCodes(Dlist<Iaction>	  *x)	{codes_	  = x;}	// 12/07/98 AM
void	Ifile::setMusts(Slist<Irule>		  *x)	{musts_	  = x;}	// 12/16/98 AM

// WARN! DON'T REMOVE THE SPACE BETWEEN ANGLE BRACKETS! C++ FEATURE.
void	Ifile::setHtab(tHtab<Slist<Irule> > *x){htab_	  = x;}	// 12/16/98 AM
void	Ifile::setFmulti(bool x)					{fmulti_	  = x;}	// 03/25/99 AM
void	Ifile::setFpath(bool x)						{fpath_	  = x;}	// 10/19/99 AM.
void	Ifile::setFpremulti(bool x)				{fpremulti_ = x;}	// 10/25/99 AM.
void	Ifile::setEmpty(bool x)						{empty_		= x;}
void	Ifile::setOwning(bool x)					{owning_		= x;}	// 12/24/00 AM.

/********************************************
* FN:		GETCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ifile::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ifile::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ifile count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ifile count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
*			12/07/98 AM. codes_ actions will be retained in their original
*			form, for now.  (Only a variable declaration that will be parsed
*			at the appropriate time.)
********************************************/

Dlist<Irule> *Ifile::intern(
	Parse *parse,																// 08/23/02 AM.
	bool fhash		// If hashing the elts & rules.					// 05/02/00 AM.
	)
{
Dlist<Irule> *rules = 0;
Dlist<Seqn>  *seqs  = 0;

// Build a sequence list for the recurse mini-passes.
if (recurses_)
	seqs = Irecurse::intern(recurses_,parse, fhash);

// Build a single rules list for the regions.
// Bind recursive mini-passes to the rule elements that call them.
if (regions_)
	rules = Iregion::intern(regions_, seqs, parse, fhash);

// Ifile will own the rules and delete them when analyzer is deleted.
rules_ = rules;

// IF RULES NOT HASHED, THEN NUMBER THEM HERE.					// 05/04/00 AM.
if (!fhash)																	// 05/04/00 AM.
	Irule::renumber(rules);												// 05/04/00 AM.

// Rules file owns the sequence list built.			// FIX.	// 11/25/98 AM.
seqs_  = seqs;

return rules;
}


/********************************************
* FN:		RHASH
* CR:		12/16/98 AM.
* SUBJ:	Install the internalized rules into a hash table.
* NOTE:	For optimizing analyzer speed.  Hash table sym will have a list
*			of pointers to rules.
*********************************************/

void Ifile::rhash(Parse *parse)
{
Dlist<Irule> *rules = 0;
int fudge = 30;

if (!(rules = rules_))
	return;

// CREATE HASH TABLE HERE.
// Creating a pretty big hash table because each rule might have a long
// list of matches/fails, or rules may have some optional elements, etc.

tHtab<Slist<Irule> > *htab;
long len;				// Size of hash table.
len = rules->getLength() * fudge + 1;	// Some breathing room for hashing.
htab = new tHtab<Slist<Irule> > (Htab::getGstab(), len);
htab_ = htab;
musts_ = new Slist<Irule>();		// Empty list.

// For each rule, either place a pointer to it in the hash table or in
// the musts_ list.
// Number the rules also.   // 12/18/98 AM.
Delt<Irule> *drule = 0;
Irule *rule = 0;
long num = 0;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();
	rule->setNum(++num);				// 12/18/98 AM.

	if (!rule->rhash(htab,			// Place into hash table if possible.
			parse))						// 04/03/03 AM.
		{
		// Informational output for the user.
		*gout << _T("NOHASH rule: ") << *rule << std::endl;
		// Couldn't hash. Add to musts_ list.
		musts_->rpush(rule);
		}
	else
		{
		//*gerr << "Hashed rule: " << *rule << std::endl;
		}
	}
// Shutting this down for now.  // 12/25/98 AM.
//htab->pretty();				// 12/25/98 AM.
}




/********************************************
* FN:		CLEANRULES
* CR:		12/17/98 AM.
* SUBJ:	Special-case cleanup for hash table with rule lists.
* NOTE:	Zero out the pointers to rules so they won't be deleted.
*********************************************/

void cleanRules(tHtab<Slist<Irule> > *htab)
{
// For each hash table location, delete the list of rules.
// DON'T delete the pointers to rules, just the list elts and list object.
long ii;
Slist<tSym<Slist<Irule> > > *list;
Selt<tSym<Slist<Irule> > > *ptr;
tSym<Slist<Irule> >  *sym;
Slist<tSym<Slist<Irule> > > *parr;
parr = htab->getParr();
Slist<Irule> *rules;

for (ii = 0; ii < htab->getSize(); ++ii)
	{
	list = &(parr[ii]);		// 12/12/98 AM.
	if (!(ptr = list->getFirst()))
		continue;			// Empty hash chain.
	for (; ptr; ptr = ptr->pRight)
		{
		sym = ptr->getData();
		rules = sym->getPtr();			// Get the rules list.
		Slist<Irule>::DeleteSlist(rules);	// Don't delete data.
		sym->setPtr(0);					// Zero out the rules list.
		}
	}
}


/********************************************
* FN:		GEN
* CR:		04/19/00 AM.
* NOTE:	Generate code for a rules file.
********************************************/

bool Ifile::gen(Parse *parse)
{
Gen *gen = parse->getAna()->getGen();									// 05/31/00 AM.
if (!gen)
	return false;					// Couldn't gen code and compile.

int id = gen->id_;

// Name of the input file (a pass file).
_TCHAR *sfile = parse->getInput();											// 05/31/00 AM.

// std::_t_ofstream *fcode = gen->fcode_;	// 04/03/09 AM.
std::_t_ofstream *fhead = gen->fhead_;
//std::_t_ofstream *fdata = gen->fdata_;		// 04/04/09 AM.

// Set up file for current pass.	// 04/03/09 AM.
_TCHAR buff[MAXSTR];					// 04/03/09 AM.
_stprintf(buff, _T("%s%c%s%cpass%d.cpp"),	// 04/03/09 AM.
				parse->getAna()->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH, id);
std::_t_ofstream *passc = new std::_t_ofstream(TCHAR2CA(buff), std::ios::out);
gen->setPassc(passc);	// 04/03/09 AM.

// Todo: Replace fcode with passc in this function.	// 04/03/09 AM.
std::_t_ofstream *fcode = passc;	// Just locally for current pass.	// 04/03/09 AM.

// And would need to reference this file in the .cpp file.

// Set up header file for current pass.	// 04/03/09 AM.
// Used for FDATA information private to the current pass.	// 04/03/09 AM.
_stprintf(buff, _T("%s%c%s%cpass%d.h"),	// 04/03/09 AM.
				parse->getAna()->getAppdir(), DIR_CH, DEFAULT_RUNDIR, DIR_CH, id);
std::_t_ofstream *passh = new std::_t_ofstream(TCHAR2CA(buff), std::ios::out);
gen->setPassh(passh);	// 04/03/09 AM.

// Replace fdata with passh in this function.	// 04/04/09 AM.
std::_t_ofstream *fdata = passh;							// 04/04/09 AM.


_TCHAR *algo = parse->getAlgo();											// 05/31/00 AM.

*passh << _T("// Automatically generated: ") << today() << std::endl;	// 04/04/09 AM.


*fcode << _T("// Automatically generated: ") << today() << std::endl;	// 04/03/09 AM.

*fcode << _T("#include \"analyzer.h\"") << std::endl;			// 04/03/09 AM.
*fcode << _T("#include \"ehead.h\"") << std::endl;				// 04/03/09 AM.
*fcode << _T("#include \"rhead.h\"") << std::endl;				// 04/03/09 AM.
*fcode << _T("#include \"data.h\"") << std::endl;					// 04/03/09 AM.
*fcode << _T("#include \"..\\user\\user.h\"") << std::endl;	// 04/03/09 AM.
*fcode << _T("#include \"..\\user\\Ucode.h\"") << std::endl;	// 04/03/09 AM.

// Point to the corresponding header file.					// 04/04/09 AM.
*fcode << _T("#include \"pass") << id << ".h\"" << std::endl;	// 04/04/09 AM.

*fcode << _T("// CODE GENERATION FOR PASS ") << id << _T(".") << std::endl << std::endl;

_TCHAR codebuf[MAXSTR];
_TCHAR rulebuf[MAXSTR];

// NEED TO GEN CODE FOR DECLARATIONS.									// 12/19/01 AM.
// For now, put them in the same pass file.							// 12/19/01 AM.
if (decls_)
	Ifunc::genDecls(decls_, gen);											// 12/19/01 AM.
// Should have a DECLS module.											// 12/19/01 AM.


// Prototypes.
if (codes_)
	{
	_stprintf(codebuf, _T("code%d(Nlppp *nlppp)"), id);
	*fhead << _T("bool ") << codebuf << _T(";");	// Prototype.
	Gen::nl(fhead);															// 04/04/03 AM.
	}
else
	{
	// Install a zero.
	codebuf[0] = '0';
	codebuf[1] = '\0';
	}

if (rules_)				// Handling pass with no rules.			// 01/23/01 AM.
	{
	_stprintf(rulebuf, _T("matchRule%d(int ruleno,bool &done,Nlppp *nlppp)"), id);
	*fhead << _T("bool ") << rulebuf << _T(";");
	Gen::nl(fhead);															// 04/04/03 AM.
	}

// Global data.
// SELECTS FOR THIS PASS.
Delt<Iarg> *dselect = 0;
Iarg *select;
_TCHAR buf[MAXSTR+1];
if (selects_)
	{
	*fdata << _T("static const _TCHAR *select") << id << _T("[] = {");	// select0
	for (dselect = selects_->getFirst(); dselect; dselect = dselect->Right())
		{
		select = dselect->getData();
		*fdata << _T("_T(\"")
				 << c_str(select->getStr(), buf, MAXSTR)
				 << _T("\"),");
		}
	*fdata << _T("0};");	// 04/04/03 AM.
Gen::eol(fdata);																// 04/04/03 AM.

	// Declare it also.
	//*fhead << "extern const _TCHAR *select" << id << "[];" << std::endl;
	}

////////////

if (recurses_)
	Irecurse::gen(recurses_, gen);

if (codes_)
	{
	// Generate code for the @CODE region.
	*fcode << _T("bool ") << codebuf;
	Gen::nl(fcode);															// 04/04/03 AM.
	*fcode << _T("{");
	Gen::nl(fcode);															// 04/04/03 AM.

	// Handle L() local vars in @CODE region!							// 11/19/02 AM.
	*fcode << _T("void *dummy = 0;");									// 01/08/07 AM.
	Gen::nl(fcode);															// 01/08/07 AM.
	*fcode << _T("Arun::fnstart(nlppp,dummy,dummy);");	// 11/19/02 AM.	// 01/08/07 AM.
	Gen::nl(fcode);															// 04/04/03 AM.

	gen->region_ = REGCODE;
	Irule::genCodes(codes_, gen);										// 06/05/00 AM.
	Iaction::gen(codes_, gen);

	// Handle L() local vars in @CODE region!							// 11/19/02 AM.
	*fcode << _T("Arun::ret(nlppp,0,0,(RFASem*)0);");					// 11/19/02 AM.
	Gen::nl(fcode);															// 04/04/03 AM.

	*fcode << _T("return true;");
	Gen::nl(fcode);															// 04/04/03 AM.
	*fcode << _T("}");
	Gen::nl(fcode);															// 04/04/03 AM.
	Gen::eol(fcode);															// 04/04/03 AM.
	}


// GENERATE PAT or REC EXECUTE FUNCTION HERE.						// 06/20/00 AM.
// Note:	Moving this to after intern pass.							// 06/20/00 AM.
// Note: Moving this back here!											// 04/04/09 AM.
genExecute(parse);															// 04/04/09 AM.


// USING THE INTERNED RULES.												// 05/01/00 AM.
if (rules_)																		// 05/01/00 AM.
	{
	// Gen'ing preactions before rule matching for each pass.	// 05/23/00 AM.
	gen->region_ = REGPRE;													// 05/24/00 AM.
	Irule::genPres(rules_, gen);											// 05/30/00 AM.
	gen->region_ = REGRULE;													// 05/24/00 AM.
	Irule::genRule(rules_, rulebuf, gen);								// 05/01/00 AM.
	}

// For each rule file, a new compiled file.
// Note: Maybe these don't need to be placed into the gen class object.
if (passc)
	{
	passc->close();	// 04/03/09 AM.
	delete passc;		// 04/03/09 AM.
	gen->passc_ = 0;	// 04/04/09 AM.
	}
if (passh)
	{
	passh->close();	// 04/03/09 AM.
	delete passh;		// 04/03/09 AM.
	gen->passh_ = 0;	// 04/04/09 AM.
	}

return true;
}


/********************************************
* FN:		GENEXECUTE
* CR:		06/20/00 AM.
* NOTE:	Generate top-level call for a rules file.
*			Possibly place this after interning, so that hash and must
*			values can be set appropriately.
*			06/21/00 AM. Generating PASS struct now.
********************************************/

bool Ifile::genExecute(Parse *parse)
{
Gen *gen = parse->getAna()->getGen();									// 05/31/00 AM.
if (!gen)
	return false;					// Couldn't gen code and compile.

// Name of the input file (a pass file).
_TCHAR *sfile = parse->getInput();											// 05/31/00 AM.

std::_t_ofstream *fcode = gen->passc_; // was fcode_						// 04/04/09 AM.
std::_t_ofstream *fhead = gen->fhead_;
std::_t_ofstream *fdata = gen->passc_; // was fdata_						// 04/04/09 AM.
int id = gen->id_;
_TCHAR *algo = parse->getAlgo();											// 05/31/00 AM.

/////////////////////////
// PASS DATA STRUCT HERE
/////////////////////////

_TCHAR *passname= _T("pass");														// 06/21/00 AM.
_TCHAR passbuf[MAXSTR];														// 06/21/00 AM.
_stprintf(passbuf,_T("%s%d_%d"),passname,id,gen->recid_);				// 06/21/00 AM.

*fdata << _T("const PASS ") << passbuf << _T("={");							// 06/21/00 AM.

if (codes_)
	*fdata << _T("code") << id << _T(",");
else
	*fdata << _T("0,");

if (selects_)																	// 06/05/00 AM.
	*fdata << _T("select") << id << _T(",");
else
	*fdata << _T("0,");															// 06/05/00 AM.

// Put traversal mode here.												// 06/08/00 AM.
if (!selects_)																	// 06/08/00 AM.
	*fdata << _T("SELECTROOT");													// 06/08/00 AM.
else if (fmulti_)
	*fdata << _T("SELECTMULTI");
else if (fpremulti_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Unimplemented: pre multi traversal.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
else if (fpath_)
	*fdata << _T("SELECTPATH");
else
	*fdata << _T("SELECTNODES");
*fdata << _T(",");

// Function name only if pass has rules.								// 01/23/01 AM.
if (rules_)
	*fdata << _T("matchRule") << id << _T(",");
else																				// 01/23/01 AM.
	*fdata << _T("0,");															// 01/23/01 AM.

_TCHAR buff[MAXSTR];
if (!sfile || !*sfile														// 05/20/00 AM.
	 || !strcmp_i(sfile, _T("NIL")))											// 05/20/00 AM.
	sfile = _T("0");																// 05/20/00 AM.
else
	{
	_tcscpy(buff, sfile);
	file_head(buff, /*UP*/ sfile);										// 05/20/00 AM.
	}
*fdata << id << _T(",");				// Current pass number.				// 05/20/00 AM.
*fdata << _T("_T(\"") << sfile << _T("\"),");										// 05/20/00 AM.

// RULE HASH TABLE STUFF ADDED HERE.									// 06/19/00 AM.
if (!Slist<Irule>::empty(musts_))
	*fdata << _T("&must") << id << _T("_") << gen->recid_ << _T(",");			// 06/19/00 AM.
else
	*fdata << _T("0,");															// 06/19/00 AM.

if (htab_)																		// 06/19/00 AM.
	*fdata << _T("rhash") << id << _T("_") << gen->recid_ << _T(",");			// 06/19/00 AM.
else
	*fdata << _T("0") << _T(",");													// 06/19/00 AM.

if (htab_)
	*fdata << htab_->getSize();
else
	*fdata << _T("0");

*fdata << _T("};");
Gen::eol(fdata);																// 04/04/03 AM.

/////////////////////////

/////////////////////////
// EXEC FUNCTION HERE
/////////////////////////

_TCHAR execbuf[MAXSTR];

_TCHAR *execname;
if (!_tcscmp(algo, _T("nlp")))													// 05/31/00 AM.
	execname = _T("patExecute");
else if (!_tcscmp(algo, _T("rec")))
	execname = _T("recExecute");
else
	return false;																// 05/31/00 AM.
_stprintf(execbuf, _T("%s%d(Parse *parse)"), execname, id);				// 05/31/00 AM.
*fhead << _T("bool ") << execbuf << _T(";");
Gen::nl(fhead);																// 04/04/03 AM.

////// patExecute function.
*fcode << _T("bool ") << execbuf;												// 05/31/00 AM.
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("return Arun::")
		 << execname
		 << _T("(parse,");

*fcode << _T("&") << passbuf << _T(",");		// Pass data struct.			// 06/21/00 AM.
*fcode << id << _T(",");				// Current pass number.				// 05/20/00 AM.
*fcode << _T("_T(\"") << sfile << _T("\")");											// 05/20/00 AM.

*fcode << _T(");");
Gen::nl(fcode);																// 04/04/03 AM.

*fcode << _T("}");
Gen::nl(fcode);																// 04/04/03 AM.
Gen::eol(fcode);																// 04/04/03 AM.
////// end of patExecute function.

return true;
}



/********************************************
* FN:		GENEXECUTE
* CR:		06/20/00 AM.
* NOTE:	Generate top-level call for a rules file.
*			Possibly place this after interning, so that hash and must
*			values can be set appropriately.
********************************************/
#ifdef OLD_
bool Ifile::genExecute(Parse *parse)
{
Gen *gen = parse->getAna()->getGen();									// 05/31/00 AM.
if (!gen)
	return false;					// Couldn't gen code and compile.

// Name of the input file (a pass file).
_TCHAR *sfile = parse->getInput();											// 05/31/00 AM.

std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
std::_t_ofstream *fhead = gen->fhead_;
int id = gen->id_;
_TCHAR *algo = parse->getAlgo();											// 05/31/00 AM.

_TCHAR execbuf[MAXSTR];

_TCHAR *execname;
if (!_tcscmp(algo, _T("nlp")))													// 05/31/00 AM.
	execname = _T("patExecute");
else if (!_tcscmp(algo, _T("rec")))
	execname = _T("recExecute");
else
	return false;																// 05/31/00 AM.
_stprintf(execbuf, _T("%s%d(Parse *parse)"), execname, id);				// 05/31/00 AM.
*fhead << _T("bool ") << execbuf << _T(";");
Gen::nl(fhead);																// 04/04/03 AM.

////// patExecute function.
*fcode << _T("bool ") << execbuf;												// 05/31/00 AM.
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("return Arun::")
		 << execname
		 << _T("(parse,");
if (codes_)
	*fcode << _T("code") << id << _T(",");
else
	*fcode << _T("0,");

if (selects_)																	// 06/05/00 AM.
	*fcode << _T("select") << id << _T(",");
else
	*fcode << _T("0,");															// 06/05/00 AM.

// Put traversal mode here.												// 06/08/00 AM.
if (!selects_)																	// 06/08/00 AM.
	*fcode << _T("SELECTROOT");													// 06/08/00 AM.
else if (fmulti_)
	*fcode << _T("SELECTMULTI");
else if (fpremulti_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Unimplemented: pre multi traversal.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
else if (fpath_)
	*fcode << _T("SELECTPATH");
else
	*fcode << _T("SELECTNODES");
*fcode << _T(",");

*fcode << _T("matchRule") << id << _T(",");
*fcode << id << _T(",");				// Current pass number.				// 05/20/00 AM.
_TCHAR buff[MAXSTR];
if (!sfile || !*sfile														// 05/20/00 AM.
	 || !strcmp_i(sfile, _T("NIL")))											// 05/20/00 AM.
	sfile = _T("0");																// 05/20/00 AM.
else
	{
	_tcscpy(buff, sfile);
	file_head(buff, /*UP*/ sfile);										// 05/20/00 AM.
	}

*fcode << _T("_T(\"") << sfile << _T("\"),");										// 05/20/00 AM.

// RULE HASH TABLE STUFF ADDED HERE.									// 06/19/00 AM.
if (!Slist<Irule>::empty(musts_))
	*fcode << _T("&must") << id << _T("_") << gen->recid_ << _T(",");			// 06/19/00 AM.
else
	*fcode << _T("0,");															// 06/19/00 AM.

if (htab_)																		// 06/19/00 AM.
	*fcode << _T("rhash") << id << _T("_") << gen->recid_;					// 06/19/00 AM.
else
	*fcode << _T("0");																// 06/19/00 AM.

*fcode << _T(");");
Gen::nl(fcode);																// 04/04/03 AM.

*fcode << _T("}");
Gen::nl(fcode);																// 04/04/03 AM.
Gen::eol(fcode);																// 04/04/03 AM.
////// end of patExecute function.

return true;
}
#endif

/********************************************
* FN:		GENSELECT
* CR:		04/27/00 AM.
* NOTE:	Generate code for traversing and matching parse tree.
*			Modularity function.
********************************************/

#ifdef OLD_
bool Ifile::genSelect(_TCHAR *travbuf, Gen *gen)
{
std::_t_ostream *fcode = gen->passc_;	// 04/03/09 AM.
int id = gen->id_;

Delt<Iarg> *dselect = 0;
Iarg *select;
if (selects_)
	dselect = selects_->getFirst();

*fcode << _T("void ") << travbuf << std::endl;
*fcode << _T("{") << std::endl;
if (!dselect)			// Use root of parse tree.
	{
	*fcode << _T("matchNodes") << id << _T("(node->Down(),0,parse,node);") << std::endl;
	*fcode << _T("}") << std::endl;
	return true;
	}

*fcode << _T("if (!node) return;") << std::endl;
*fcode << _T("Pn *pn = node->getData();") << std::endl;
*fcode << _T("_TCHAR *name = pn->getName();") << std::endl;

*fcode << _T("if (");

// Traverse selects_ and get their names here.
select = dselect->getData();
*fcode << _T("!strcmp_i(name, _T(\"") << select->getStr() << _T("\"))");
for (dselect = dselect->Right(); dselect; dselect = dselect->Right())
	{
	select = dselect->getData();
	*fcode << std::endl << _T("\t|| !strcmp_i(name, _T(\"") << select->getStr() << _T("\"))");
	}

*fcode << _T(")") << std::endl;
*fcode << _T("\tmatchNodes") << id << _T("(node->Down(),0,parse,node);") << std::endl;
*fcode << _T("else if (pn->getUnsealed())") << std::endl;
*fcode << _T("\ttraverseSelect") << id << _T("(parse, node->Down());") << std::endl;
*fcode << _T("if (!(node->Left()))") << std::endl;
*fcode << _T("\t{") << std::endl;
*fcode << _T("\tfor (node = node->Right(); node; node = node->Right())") << std::endl;
*fcode << _T("\t\ttraverseSelect") << id << _T("(parse,node);") << std::endl;
*fcode << _T("\t}") << std::endl;

*fcode << _T("}") << std::endl;
return true;
}
#endif

/********************************************
* FN:		GENNODES
* CR:		04/28/00 AM.
* NOTE:	Generate code for matching on parse tree nodes.
*			Modularity function.
********************************************/

#ifdef OLD_
bool Ifile::genNodes(_TCHAR *nodesbuf, Gen *gen)
{
std::_t_ostream *fcode = gen->passc_;	// 04/03/09 AM.
int id = gen->id_;

*fcode << _T("void ") << nodesbuf << std::endl;
*fcode << _T("{") << std::endl;

*fcode << _T("if (!start || !parse) return;") << std::endl;
*fcode << _T("Delt<Irule> *prule;") << std::endl;
*fcode << _T("NODE *node = start;") << std::endl;
*fcode << _T("Nlppp nlppp(0,new Tree<Pn>(),parse,select,0,0,node,start,end,0,0);") << std::endl;

*fcode << _T("while (node)") << std::endl;
*fcode << _T("\t{") << std::endl;

*fcode << _T("\tif (!matchRules") << id << _T("(&nlppp, /*DU*/ prule))") << std::endl;
*fcode << _T("\t\tnode = nlppp.node_ = nodeRight(node,end);") << std::endl;
*fcode << _T("\telse") << std::endl;
	*fcode << _T("\t\t{") << std::endl;
	*fcode << _T("\t\tnlppp.rule_ = prule->getData();") << std::endl;
	*fcode << _T("\t\texecActions(&nlppp);") << std::endl;	// Need runtime versions.
	*fcode << _T("\t\tnode = nlppp.node_;") << std::endl;
	*fcode << _T("\t\tstart = nlppp.start_;") << std::endl;
	*fcode << _T("\t\tend = nlppp.end_;") << std::endl;
	*fcode << _T("\t\tnlppp.sem_ = 0;") << std::endl;
	*fcode << _T("\t\tnlppp.dsem_ = 0;") << std::endl;
	*fcode << _T("\t\t}") << std::endl;

*fcode << _T("\t}") << std::endl;

*fcode << _T("delete (Tree<Pn> *) nlppp.collect_;") << std::endl;
*fcode << _T("}") << std::endl << std::endl;
return true;
}
#endif


/********************************************
* FN:		GENRULES
* CR:		04/28/00 AM.
* NOTE:	Generate code for matching rules against parse tree nodes.
*			Modularity function.
********************************************/

#ifdef OLD_
bool Ifile::genRules(_TCHAR *rulesbuf, Gen *gen)
{
std::_t_ostream *fcode = gen->passc_;	// 04/03/09 AM.
int id = gen->id_;

*fcode << _T("void ") << rulesbuf << std::endl;
*fcode << _T("{") << std::endl;

*fcode << _T("NODE *node = nlppp->node_;") << std::endl;

// Need to get the total rule count for this pass.
// (Or can use a sentinel in the default case of the rule switch).
*fcode << _T("long ruleno=0;") << std::endl;
*fcode << _T("for (;;)") << std::endl;
	*fcode << _T("\t{") << std::endl;
	*fcode << _T("\t++ruleno;") << std::endl;
	*fcode << _T("\tnlppp->node_ = node;") << std::endl;
	//*fcode << "\tnlppp->rule_ = rule->getData();" << std::endl;
	*fcode << _T("\tnlppp->first_ = nlppp->last_ = 0;") << std::endl;
	*fcode << _T("\tif (matchRule") << id << _T("(ruleno,bool &,nlppp)") << std::endl;
		*fcode << _T("\t\t && nlppp->first_") << std::endl;
		*fcode << _T("\t\t && checkActions") << id << _T("(ruleno,nlppp))") << std::endl;
		*fcode << _T("\t\t{") << std::endl;
		*fcode << _T("\t\tnlppp->node_ = node;") << std::endl;
		*fcode << _T("\t\treturn true;") << std::endl;
		*fcode << _T("\t\t}") << std::endl;
	*fcode << _T("\telse") << std::endl;
		*fcode << _T("\t\tmatchCleanup(nlppp->collect_);") << std::endl;

	*fcode << _T("\t}") << std::endl;


*fcode << _T("nlppp->node_ = node;") << std::endl;
*fcode << _T("return false;") << std::endl;
*fcode << _T("}") << std::endl << std::endl;
return true;
}
#endif


/********************************************
* FN:		GENHASH
* CR:		06/13/00 AM.
* NOTE:	Generate code for hash tables.
********************************************/

bool Ifile::genHash(Parse *parse)
{
Gen *gen = parse->getAna()->getGen();
if (!gen)
	return false;					// Couldn't gen code and compile.


//if (recurses_)
//	Irecurse::genHash(recurses_, gen);


// GEN CODE FOR HASHING RULE ELEMENTS.
// NOTE: TRAVERSE HASH TABLES AND DUMP THEM.
if (rules_)
	{
	Irule::genHash(rules_, gen);

	// GEN CODE FOR HASHING THE RULES THEMSELVES.
	genRhash(gen);
	}

// Generate pass even if no rules.							// FIX.	// 01/09/01 AM.
// GEN TOP LEVEL CALL AFTER INTERNING PASS.							// 06/20/00 AM.
// Moving this back to gen pass of analyzer compilation.			// 04/04/09 AM.
//genExecute(parse);															// 04/04/09 AM.


return true;
}


/********************************************
* FN:		GENRHASH
* CR:		06/16/00 AM.
* NOTE:	Generate code for rule-triggering hash tables.
********************************************/

bool Ifile::genRhash(Gen *gen)
{
Dlist<Irule> *rules = rules_;
if (!rules)
	return false;

long len = rules->getLength();

// Gen code for the musts list of rules.
// ie, the rules that every node must be tried against; rules that
// couldn't be hashed.
Irule::genMusts(musts_, gen);

// Gen code for the rule hash table.
htab_->gen(gen);

return true;
}
