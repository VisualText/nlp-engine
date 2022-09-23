/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IRECURSE.CPP
* FILE:	lite\irecurse.cpp
* CR:		11/24/98 AM.
* SUBJ:	Class for a named set of rule regions in a rules file.
* NOTE:	For recursive mini-passes invoked by rule elements.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"		// 05/19/99 AM.
#include "io.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"			// 05/30/00 AM.
#include "irule.h"			// For interning.
//#include "gen.h"			// 05/30/00 AM.
#include "irecurse.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Slist<Irecurse>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Selt<Irecurse>::count_  = 0;
#else
int Slist<Irecurse>::count_ = 0;
int Selt<Irecurse>::count_  = 0;
#endif
int Irecurse::count_	= 0;

/********************************************
* FN:		Special functions for the class
********************************************/

Irecurse::Irecurse(			// Default constructor.
	_TCHAR *name,
	Slist<Iregion> *regions
	)
{
name_ = name;
regions_ = regions;
rules_ = 0;
recid_ = 0;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

Irecurse::~Irecurse()
{
if (name_)
	name_ = 0;			// Don't delete the string.  Owned elsewhere.
							// eg, in the string table.
if (regions_)
	delete (Slist<Iregion> *) regions_;

// 11/24/98 AM.  Recurse deletes its own rules.
if (rules_)
	{
	Dlist<Irule>::DeleteDlistAndData(rules_);
	rules_ = 0;
	}

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Irecurse &irecurse)
{
output << _T("@recurse") << str(irecurse.name_) << std::endl;
if (irecurse.regions_)
	output << *(irecurse.regions_);
output << _T("@@recurse") << str(irecurse.name_) << std::endl;
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Selt<Irecurse> &selt)
{
Irecurse *dat;
dat = selt.getData();
output << *dat;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Slist<Irecurse> &list)
{
Selt<Irecurse> *selt;
Irecurse *dat;
selt = list.getFirst();
dat   = selt->getData();
output << *dat;
while ((selt = selt->Right()))
	{
	dat = selt->getData();
	output << *dat;
	}

return output;
}
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

_TCHAR				*Irecurse::getName()		{return name_;}
Slist<Iregion>	*Irecurse::getRegions()	{return regions_;}
Dlist<Irule>   *Irecurse::getRules()	{return rules_;  }
int				 Irecurse::getRecid()	{return recid_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Irecurse::setName(_TCHAR					*x)	{name_	 = x;}
void	Irecurse::setRegions(Slist<Iregion>	*x)	{regions_ = x;}
void  Irecurse::setRules(Dlist<Irule>     *x)	{rules_	 = x;}
void	Irecurse::setRecid(int x)						{recid_	 = x;}


/********************************************
* FN:		GETCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Irecurse::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Irecurse::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Irecurse count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Irecurse count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
********************************************/

Dlist<Seqn> *Irecurse::intern(Slist<Irecurse> *recs,
	Parse *parse,																// 08/23/02 AM.
	bool fhash)
{
Dlist<Seqn> *seqs = 0;
Seqn *seqn = 0;
Selt<Irecurse> *selt = 0;
Irecurse *rec = 0;
int count = 0;																	// 05/31/00 AM.

seqs = new Dlist<Seqn>();
for (selt = recs->getFirst(); selt; selt = selt->Right())
	{
	++count;																		// 05/31/00 AM.
	rec = selt->getData();
	seqn = Irecurse::intern(rec,parse, fhash);	// Convert a recurse to a pass.
	rec->recid_ = count;														// 05/31/00 AM.
	seqn->setRecid(count);													// 05/31/00 AM.
	// 07/02/99 AM. No longer copies.
	seqs->rpush(seqn);
	//seqn->setAlgo(0);
	//seqn->setRulesfilename(0);
	//seqn->setRules(0);
	//delete seqn;
	}

return seqs;
}

/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
********************************************/

Seqn *Irecurse::intern(Irecurse *rec,
	Parse *parse,																// 08/23/02 AM.
	bool fhash)
{
Seqn *seqn = 0;
Dlist<Irule> *rules = 0;

if (rec->getRegions())
	rules = Iregion::intern(rec->getRegions(), 0, parse, fhash);
seqn = new Seqn(0,0,rules, rec->getName());

// Assign rules to the recurse object, so they can be deleted there later on.
rec->setRules(rules);

return seqn;
}


/********************************************
* FN:		GEN
* CR:		04/20/00 AM.
* NOTE:	Generate code for a list of recurse regions.
*			05/30/00 AM. Filling this in finally.
********************************************/

bool Irecurse::gen(Slist<Irecurse> *list, Gen *gen)
{
// Do much the same as for an Ifile.  Naming of arrays will have to be
// tweaked to handle recurse minipasses.
// Will add yet another counter to the gen object, for recurse regions.
Selt<Irecurse> *selt;
gen->recid_ = 0;
gen->ruleid_ = 0;
for (selt = list->getFirst(); selt; selt = selt->Right())
	{
	gen->recid_ += 1;		// Unique number for this recurse region in this pass.
	if (!selt->getData()->gen(gen))
		{
		gen->recid_ = 0;	// Recovery action.
		return false;		// Error in generating code.
		}
	}
gen->recid_ = 0;			// Reset this so it won't screw anyone else up.
gen->ruleid_ = 0;			// Reset the rule numbers also.
return true;
}


/********************************************
* FN:		GEN
* CR:		05/30/00 AM.
* NOTE:	Generate code for a recurse region.
*			05/30/00 AM. Filling this in finally.
********************************************/

bool Irecurse::gen(Gen *gen)
{
if (!name_ || !rules_)
	return false;

_TCHAR rulebuf[MAXSTR];
_stprintf(rulebuf, _T("recurse%d_%d(int ruleno,bool &done,Nlppp *nlppp)"),
	gen->id_, gen->recid_);
*(gen->fhead_) << _T("bool ") << rulebuf << _T(";") << std::endl;

gen->region_ = REGPRE;
Irule::genPres(rules_, gen);
gen->region_ = REGRULE;
Irule::genRule(rules_,rulebuf, gen);
return true;
}
