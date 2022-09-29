/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IREGION.CPP
* FILE:	lite\iregion.cpp
* CR:		11/24/98 AM.
* SUBJ:	Class for a region of rules in a rules file.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "io.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"
#include "iaction.h"
#include "irule.h"
#include "iregion.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Slist<Iregion>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Selt<Iregion>::count_  = 0;
#else
int Slist<Iregion>::count_ = 0;
int Selt<Iregion>::count_  = 0;
#endif
int Iregion::count_        = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

Iregion::Iregion(			// Default constructor.
	Dlist<Ipre> *pres,
	Dlist<Iaction> *checks,			// 11/27/98 AM.
	Dlist<Iaction> *posts,
	Dlist<Irule>   *rules
	)
{
pres_  = pres;
checks_ = checks;						// 11/27/98 AM.
posts_ = posts;
rules_ = rules;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

Iregion::~Iregion()
{
if (pres_)
	Dlist<Ipre>::DeleteDlistAndData(pres_);
if (checks_)
	Dlist<Iaction>::DeleteDlistAndData(checks_);			// 11/27/98 AM.
if (posts_)
	Dlist<Iaction>::DeleteDlistAndData(posts_);
if (rules_)
	Dlist<Irule>::DeleteDlistAndData(rules_);

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iregion &iregion)
{
if (iregion.pres_)
	{
	output << _T("@pre") << std::endl;
	output << *(iregion.pres_);
	}
if (iregion.checks_)
	{
	output << _T("@check") << std::endl;
	output << *(iregion.checks_);
	}
if (iregion.posts_)
	{
	output << _T("@post") << std::endl;
	output << *(iregion.posts_);
	}
if (iregion.rules_)
	{
	output << _T("@rules") << std::endl;
	output << *(iregion.rules_);
	}
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Selt<Iregion> &selt)
{
Iregion *dat;
dat = selt.getData();
output << *dat;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Slist<Iregion> &list)
{
Selt<Iregion> *selt;
Iregion *dat;
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

Dlist<Ipre> *Iregion::getPres()	{return pres_; }
Dlist <Iaction> *Iregion::getChecks() {return checks_;}			// 11/27/98 AM.
Dlist<Iaction> *Iregion::getPosts()	{return posts_;}
Dlist<Irule>   *Iregion::getRules()	{return rules_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Iregion::setPres(Dlist<Ipre>		 *x)	{pres_	= x;}
void	Iregion::setChecks(Dlist<Iaction> *x)	{checks_	= x;}		// 11/27/98 AM.
void	Iregion::setPosts(Dlist<Iaction>  *x)	{posts_	= x;}
void	Iregion::setRules(Dlist<Irule>    *x)	{rules_	= x;}


/********************************************
* FN:		GETCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iregion::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iregion::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iregion count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iregion count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
********************************************/

Dlist<Irule> *Iregion::intern(Slist<Iregion> *regions, Dlist<Seqn> *seqs,
	Parse *parse,																// 08/23/02 AM.
	bool fhash)
{
Dlist<Irule> *rules = 0;	// Linearized set of all rules for region.
Dlist<Irule> *list;			// Partial rules for region.
Irule *rule = 0;
Selt<Iregion> *selt = 0;
Iregion *region = 0;

rules = new Dlist<Irule>();
for (selt = regions->getFirst(); selt; selt = selt->Right())
	{
	region = selt->getData();
	list = Iregion::intern(region, seqs, parse, fhash);
	rules = Dlist<Irule>::merge(rules, list);
	}
return rules;
}


/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
********************************************/

Dlist<Irule> *Iregion::intern(Iregion *region, Dlist<Seqn> *seqs,
	Parse *parse,																// 08/23/02 AM.
	bool fhash)
{
Dlist<Irule> *rules = 0;
Dlist<Ipre> *pres = 0;
Dlist<Iaction> *checks = 0;				// 11/27/98 AM.
Dlist<Iaction> *posts = 0;

if (!region)
	return rules;
pres = region->getPres();
checks = region->getChecks();				// 11/27/98 AM.
posts = region->getPosts();
rules = region->getRules();

// Intern the actions.  (Nothing done there yet.)

// Intern the list of rules.
rules = Irule::intern(rules, pres, checks, posts, seqs,parse, fhash);

// Since we're taking the region's rules, zero them out.
region->setRules(0);

return rules;
}


/********************************************
* FN:		GEN
* CR:		04/20/00 AM.
* NOTE:	Generate code for a list of regions.
********************************************/

bool Iregion::gen(Slist<Iregion> *list, Gen *gen)
{
return true;
}

