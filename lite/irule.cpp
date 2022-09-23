/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IRULE.CPP
* FILE:	lite\irule.cpp
* CR:		10/23/98 AM.
* SUBJ:	Interpreted/internal rule class.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"															// 09/26/01 AM.
#include "io.h"
#include "lite/iarg.h"		// 05/14/03 AM.
#include "slist.h"			// 12/16/98 AM.
#include "htab.h"				// 12/16/98 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"				// 05/08/00 AM.
#include "thtab.h"			// 12/17/98 AM.
#include "nug.h"				// 12/27/99 AM.
//#include "gen.h"				// 05/08/00 AM.
#include "iregion.h"			// 02/27/01 AM.
#include "rfasem.h"	// Don't really want this here.		// 05/07/00 AM.
#include "parse.h"															// 08/23/02 AM.
#include "irule.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Irule>::count_	= 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Irule>::count_		= 0;
#else
int Dlist<Irule>::count_	= 0;
int Delt<Irule>::count_		= 0;
#endif
//int Irule::count_				= -2;	// 11/17/98 AM. Because of two statics.
int Irule::count_ = 0;							// 07/02/99 AM.

// Used in the musts_ list of Ifile.  // 12/16/98 AM.
#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Slist<Irule>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Selt<Irule>::count_  = 0;
#else
int Slist<Irule>::count_ = 0;
int Selt<Irule>::count_  = 0;
#endif

/********************************************
* FN:		Special Functions for Irule Class
* CR:		10/23/98 AM.
********************************************/

Irule::Irule(				// Default constructor.
	Dlist<Ielt> *phr,				// Phrase of elements.
	Isugg *elt,						// Suggested concept.
	Dlist<Ipre> *pr,				// Pre-actions.
	Dlist<Iaction> *checks,		// Check-actions.						// 11/27/98 AM.
	Dlist<Iaction> *pst,			// Post-actions.
	Delt<Ielt> *trig,				// Trigger elt.
	long line						// Line number.						// 05/26/01 AM.
	)
{
phrase_	= phr;
sugg_		= elt;
pre_		= pr;
checks_	= checks;															// 11/27/98 AM.
post_		= pst;
trigger_ = trig;
lookahead_ = 0;			// No lookahead elt.							// 11/22/99 AM.

sample_	= 0;																	// 06/16/99 AM.
down_		= 0;																	// 06/28/99 AM.
nlabels_	= 0;																	// 07/16/99 AM.
line_		= line;																// 05/26/01 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Irule::~Irule()
{

// 11/24/98 AM. Rules built by RFA will not own their actions.
// The region object will own them and delete them.
clear(false);

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
// Copy constructor.
// ASSUMES ACTIONS NOT OWNED BY RULE.
Irule::Irule(		// 07/01/99 AM.
	Irule &orig
	)
{
zero();
Irule::copy(&orig, false);

#ifndef STABLE_
++count_;
#endif
}

// 07/03/99 AM.
// Alternative copy for rules that do own their actions.
// (As in RUG rules.)
// ALLOC: CREATES NEW IRULE.
// Copies the rule used to call this with.
Irule *Irule::copy_owning()
{
Irule *tmp;

tmp = new Irule();			// Empty rule.
*tmp = *this;	// ASSIGNMENT ASSUMES ACTIONS ARE OWNED BY RULE.
return tmp;
}


// Copy an entire list of rules that owns its actions.	// 10/29/99 AM.
Dlist<Irule> *Irule::copy_owning_list(Dlist<Irule> *list)
{
if (!list)
	return 0;

Dlist<Irule> *nlist = new Dlist<Irule>();	// New empty list.
Delt<Irule> *delt;
Irule *rule;

for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	rule = delt->getData();
	nlist->rpush(rule->copy_owning());	// Copy rule into new list.
	}
return nlist;
}


/*******************************************/
const Irule &Irule::operator=(const Irule &rule)	// Assignment.
{
Irule *to;
to = this;
if (&rule == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Irule object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

// Clear out the TO object.
to->clear(true);

// Zero out fields of the TO object.
to->zero();

// Copy the given rule into TO object.
// WARN:	ASSUMES NEW RULE OWNS ITS ACTIONS.
to->copy(&rule, true);

return *this;
}


/*******************************************/
void Irule::clear(bool owning)				// 07/01/99 AM.
{
if (phrase_)
	Dlist<Ielt>::DeleteDlistAndData(phrase_);
if (sugg_)
	delete (Isugg *) sugg_;

// 11/24/98 AM. Rules built by RFA will not own their actions.
// The region object will own them and delete them.
if (owning)
	{
	if (pre_)
		Dlist<Ipre>::DeleteDlistAndData(pre_);
	if (checks_)
		Dlist<Iaction>::DeleteDlistAndData(checks_);
	if (post_)
		Dlist<Iaction>::DeleteDlistAndData(post_);
	}

// 06/30/99 AM. Deleting the DOWN list.  The Dlist and Delt elements
// are owned by the current rule, but the actual rules pointed to
// are NOT owned by the current rule.  They should just be pointers to
// those damned rules.  (Make it an Slist!)
if (down_)
	{
	Slist<Irule>::DeleteSlist(down_);
	down_ = 0;
	}
}

/*******************************************/
// Primitive function.  Does no checking.
void Irule::zero()			// 07/01/99 AM.
{
phrase_ = 0;
sugg_ = 0;
pre_ = 0;
checks_ = 0;
post_ = 0;
trigger_ = 0;
lookahead_ = 0;		// 11/22/99 AM.
num_ = 0;

sample_ = 0;
down_ = 0;
nlabels_ = 0;			// 07/16/99 AM.
line_		= 0;			// 05/26/01 AM.
}
/*******************************************/
// Copy one Irule into another.
// Primitive.  Does no clearing or checking.
void Irule::copy(		// 07/02/99 AM.
	const Irule *orig,		// The source.
	bool owning			// Whether new rule will own its actions
							// or point to those of orig.
	)
{
Irule *dest;			// Destination of copy.
dest = this;

if (orig->phrase_)
	dest->setPhrase(new Dlist<Ielt>(*(orig->phrase_)));
if (orig->sugg_)
	dest->setSugg(new Isugg(*(orig->sugg_)));

if (owning)			// Rule will get its own copy of actions.
	{
	if (orig->pre_)
		dest->setPre(new Dlist<Ipre>(*(orig->pre_)));
	if (orig->checks_)
		dest->setChecks(new Dlist<Iaction>(*(orig->checks_)));
	if (orig->post_)
		dest->setPost(new Dlist<Iaction>(*(orig->post_)));
	}
else		// 07/03/99 AM.
	{
	dest->pre_ = orig->pre_;
	dest->checks_ = orig->checks_;
	dest->post_ = orig->post_;
	}

// Transfer the trigger.
// Which elt is the trigger in original rule.
Delt<Ielt> *trig = 0;
long ord = 0;
if ((trig = orig->getTrigger()))
	{
	ord = orig->getPhrase()->getOrd(trig);
	assert(ord);
	trig = dest->getPhrase()->getNth(ord);		// Get new trigger.
	assert(trig);
	dest->setTrigger(trig);
	}

dest->lookahead_	= orig->lookahead_;			// 11/22/99 AM.
dest->sample_		= orig->sample_;

// Should copy the down_ list also, I suppose.
if (orig->down_)
	dest->setDown(new Slist<Irule>(*(orig->down_)));
dest->line_ = orig->line_;							// 05/26/01 AM.
}



/*******************************************/

// Whoever wants to can print the actions out separately.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Irule &rule)
{
if (!rule.sugg_ || !rule.phrase_)
	{
	output << _T("**empty rule**");
	return output;
	}

//if (rule.pre_)
//	output << "pre: " << *(rule.pre_) << std::endl;
//if (rule.post_)
//	output << "post_: " << *(rule.post_) << std::endl;

// Note: Can't get trigger info to the phrase elt! 11/22/98 AM.
long ord;
ord = 0;
if (rule.trigger_)
	ord = rule.phrase_->getOrd(rule.trigger_);
output << *(rule.sugg_) << _T(" <-");
if (ord)
	output << ord << _T("-");
output << _T(" ") << *(rule.phrase_) << _T(" @@");
return output;
}

// 10/26/98 AM.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Delt<Irule> &delt)
{
Irule *rr;
rr = delt.getData();
output << *rr;
return output;
}

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Irule> &list)
{
Delt<Irule> *delt;
Irule *rr;
if (!(delt = list.getFirst()))
	return output;
rr   = delt->getData();
output << *rr << std::endl;
while ((delt = delt->Right()))
	{
	rr = delt->getData();
	output << *rr << std::endl;
	}

return output;
}

/*******************************************/

// 12/19/98 AM.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Selt<Irule> &selt)
{
Irule *rr;
rr = selt.getData();
output << *rr;
return output;
}

// 12/19/98 AM.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Slist<Irule> &list)
{
Selt<Irule> *selt;
Irule *rr;
if (!(selt = list.getFirst()))
	return output;
rr   = selt->getData();
output << *rr << std::endl;
while ((selt = selt->Right()))
	{
	rr = selt->getData();
	output << *rr << std::endl;
	}

return output;
}
/*******************************************/

/*******************************************/

/********************************************
* FN:		Access Functions
* CR:		10/23/98 AM.
********************************************/

Dlist<Ielt>		*Irule::getPhrase() const	{return phrase_;  }
Isugg          *Irule::getSugg()		{return sugg_;    }
Dlist<Ipre>		*Irule::getPre()		{return pre_;     }
Dlist<Iaction> *Irule::getChecks()	{return checks_; }			// 11/27/98 AM.
Dlist<Iaction>	*Irule::getPost()		{return post_;    }
Delt<Ielt>	*Irule::getTrigger() const	{return trigger_;}
int			Irule::getLookahead() {return lookahead_;}			//	11/22/99 AM.
long				 Irule::getNum()		{return num_;	  }			// 12/18/98 AM.
_TCHAR				*Irule::getSample()	{return sample_; }			// 06/16/99 AM.
Slist<Irule>	*Irule::getDown()		{return down_;	  }			// 06/28/99 AM.
int				 Irule::getNlabels()	{return nlabels_;}			// 07/16/99 AM.
long				 Irule::getLine()		{return line_;}				// 05/26/01 AM.


/********************************************
* FN:		Modify Functions
* CR:		10/23/98 AM.
********************************************/

void Irule::setPhrase(Dlist<Ielt>	 *x)	{phrase_		= x;}
void Irule::setSugg(Isugg				 *x)	{sugg_			= x;}
void Irule::setPre(Dlist<Ipre>		 *x)	{pre_			= x;}
void Irule::setChecks(Dlist<Iaction> *x)	{checks_		= x;}
void Irule::setPost(Dlist<Iaction>	 *x)	{post_			= x;}
void Irule::setTrigger(Delt<Ielt>	 *x)	{trigger_	= x;}
void Irule::setLookahead(int			  x)	{lookahead_ = x;}		// 11/22/99 AM.
void Irule::setNum(long x)						{num_			= x;}		// 12/18/98 AM.
void Irule::setSample(_TCHAR				 *x)	{sample_		= x;}		// 06/16/99 AM.
void Irule::setDown(Slist<Irule>		 *x)	{down_		= x;}		// 06/28/99 AM.
void Irule::setNlabels(int x)					{nlabels_	= x;}		// 07/16/99 AM.
void Irule::setLine(long x)					{line_		= x;}		// 05/26/01 AM.


/********************************************
* FN:		GETCOUNT
* CR:		10/23/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Irule::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		10/23/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Irule::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Irule count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Irule count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		unpack
* CR:		10/06/99 AM.
* SUBJ:	Unpack some rule data for pattern matcher.
* RET:	True if ok fetch of data.
* OPT:	One function call rather than three, in critical part of
*			pattern match.
*			OK, but shouldn't be used in Irule!
********************************************/

bool Irule::unpack(
	/*UP*/
	Dlist<Ielt>* &phr,
	Isugg* &sgg,
	Delt<Ielt>* &trig
	)
{
// Transfer values up.
trig = trigger_;
if ((phr = phrase_) && (sgg = sugg_))
	return true;

std::_t_strstream gerrStr;
gerrStr << _T("[Irule::unpack: Empty data.]") << std::ends;
errOut(&gerrStr,false);

return false;
}


/********************************************
* FN:		makeDelt
* CR:		10/23/98 AM.
* SUBJ:	Create a list element with appropriate data.
* NOTE:	Must be a more elegant way to initialize the elt.
********************************************/
//static Irule rr;					// 07/02/99 AM.

Delt<Irule> *Irule::makeDelt(
	Dlist<Ielt> *phr,					// Phrase of elements.
	Isugg *elt,							// Suggested concept.
	Dlist<Ipre> *pr,					// Pre-actions.
	Dlist<Iaction> *checks,			// Check-actions.					// 11/27/98 AM.
	Dlist<Iaction> *pst,				// Post-actions.
	Delt<Ielt>		*trig,			// Trigger.
	long				 line				// Line number.					// 05/26/01 AM.
	)
{
Delt<Irule> *delt;

// NEW WAY.																		 // 07/02/99 AM.
Irule *rr;																		// 07/02/99 AM.
rr = new Irule(phr,elt,pr,checks,pst,trig,line);					// 05/26/01 AM.
delt = new Delt<Irule>(rr);
return delt;
}


/********************************************
* FN:		makeDlist
* CR:		10/23/98 AM.
* SUBJ:	Create a list with appropriate data.
* NOTE:	Class member function.
*			(Can be called without an instance existing).
********************************************/

Dlist<Irule> *Irule::makeDlist(
	Dlist<Ielt> *phr,					// Phrase of elements.
	Isugg *elt,							// Suggested concept.
	Dlist<Ipre> *pr,					// Pre-actions.
	Dlist<Iaction> *checks,			// Check-actions.					// 11/27/98 AM.
	Dlist<Iaction> *pst,				// Post-actions.
	Delt<Ielt>		*trig,			// Trigger.
	long				 line				// Line number.					// 05/26/01 AM.
	)
{
Delt<Irule> *delt;
delt = makeDelt(phr, elt, pr, checks, pst, trig, line);			// 05/26/01 AM.
return new Dlist<Irule>(delt);
}

// 10/25/98 AM. 2nd variant.  Makes empty list.
Dlist<Irule> *Irule::makeDlist()
{
return new Dlist<Irule>();
}


/********************************************
* FN:		addDelt
* CR:		10/23/98 AM.
* SUBJ:	Add new element to given list.
* NOTE:	Must be a more elegant way to initialize the elt.
*			Class member function. Need not be called with object.
********************************************/
//static Irule ir;			// 07/02/99 AM.

Delt<Irule> *Irule::addDelt(
	Dlist<Irule> *list,				// List to add element to.
	Dlist<Ielt> *phr,					// Phrase of elements.
	Isugg *elt,							// Suggested concept.
	Dlist<Ipre> *pr,					// Pre-actions.
	Dlist<Iaction> *checks,				// Check-actions.				// 11/27/98 AM.
	Dlist<Iaction> *pst,				// Post-actions.
	Delt<Ielt>		*trig,				// Trigger.
	long				 line				// Line number.					// 05/26/01 AM.
	)
{
Delt<Irule> *delt;

Irule *ir;																		// 07/02/99 AM.

ir = new Irule(phr,elt,pr,checks,pst,trig, line);					// 05/26/01 AM.
delt = new Delt<Irule>(ir);
list->rpush(delt);
return delt;			// In case anyone wants it.
}


/********************************************
* FN:		deleteOwning
* CR:		11/24/98 AM.
* SUBJ:	Delete actions of a rule list that owns its actions.
* NOTE:	Because the RFA doesn't build regions, recurses, and rulesfile
*			objects.  If we change it to do all that, we can remove this
*			ad hoc function.
********************************************/

void Irule::deleteOwning(Dlist<Irule> *list)
{
Delt<Irule> *rules;
Irule *rule;
if (!list)
	return;
for (rules = list->getFirst(); rules; rules = rules->Right())
	{
	rule = rules->getData();
	Irule::deleteOwning(rule);				// 07/03/99 AM.
	}
}


/********************************************
* FN:		deleteOwning
* CR:		07/03/99 AM.
* SUBJ:	Delete actions of a rule that owns its actions.
* NOTE:	All this arises because RFA rules were the first ones built,
*			and they were optimized to use actions owned by region objects
*			from parsing a rules file.
********************************************/

void Irule::deleteOwning(Irule *rule)
{
if (!rule)
	return;
Dlist<Ipre>::DeleteDlistAndData(rule->getPre());
Dlist<Iaction>::DeleteDlistAndData(rule->getChecks());
Dlist<Iaction>::DeleteDlistAndData(rule->getPost());
rule->setPre(0);
rule->setChecks(0);
rule->setPost(0);
}


/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
********************************************/

Dlist<Irule> *Irule::intern(
	Dlist<Irule> *rules,
	Dlist<Ipre> *pres,
	Dlist<Iaction> *checks,													// 11/27/98 AM.
	Dlist<Iaction> *posts,
	Dlist<Seqn> *seqs,
	Parse *parse,																// 08/23/02 AM.
	bool fhash
	)
{
Delt<Irule> *ptr = 0;

if (!rules)
	return rules;

for (ptr = rules->getFirst(); ptr; ptr = ptr->Right())
	intern(ptr->getData(), pres, checks, posts, seqs,parse, fhash);
return rules;
}


/********************************************
* FN:		INTERN
* CR:		11/24/98 AM.
* NOTE:	Part of RFA machinery for internalizing a rules file.
*			Redoing the work of Intern::internRule here.
********************************************/

Irule *Irule::intern(
	Irule *rule,
	Dlist<Ipre> *pres,
	Dlist<Iaction> *checks,													// 11/27/98 AM.
	Dlist<Iaction> *posts,
	Dlist<Seqn> *seqs,
	Parse *parse,																// 08/23/02 AM.
	bool fhash
	)
{
if (!rule)
	return rule;

// Interning check,post actions consists of binding them to each rule.
// Rule doesn't use pre-actions.  Individual elements do.  11/29/98 AM.
//rule->setPre(pres);
rule->setChecks(checks);					// 11/27/98 AM.
rule->setPost(posts);

// Intern pairs for the suggested elt.
bool inf;						// No particular info for suggested element.
inf = false;
Isugg *sugg;
sugg = rule->getSugg();
assert(sugg);
sugg->intern(inf);			// Internalize suggested element.

// Intern pairs for each phrase elt.
bool f_trigger;
f_trigger = false;
bool f_trig = false;					// To gather trigger info.

bool f_lookahead = false;	// First elt with lookahead.	// 11/22/99 AM.
bool f_look = false;			// If elt is lookahead.			// 11/22/99 AM.
Dlist<Ielt> *phrase;
phrase = rule->getPhrase();
assert(phrase);
Delt<Ielt> *delt;
// For each rule phrase element.
int ord = 0;					// Count which element we're at.	// 11/29/98 AM.
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	++ord;						// 11/29/98 AM.
	Ielt *elt;					// Base pointer.
	elt = delt->getData();
	assert(elt);

	// Internalize phrase element.
	// Bind recursive passes to rule elements that use them.
	// Interning pre-actions is done per rule element.				// 11/29/98 AM.
	elt->intern(seqs, pres, ord,											// 11/29/98 AM.
					parse,														// 08/23/02 AM.
					/*UP*/ f_trig, f_look, fhash);						// 11/22/99 AM.

	if (f_look)																	// 11/22/99 AM.
		{
		// Only zero or one elt should have lookahead.
		if (!f_lookahead)														// 11/22/99 AM.
			{
			f_lookahead = true;			// First lookahead.			// 11/22/99 AM.
			rule->setLookahead(ord);	// Track elt number.			// 11/22/99 AM.
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Rule can't have multiple lookaheads.]") << std::ends;
//			parse->errOut(false,true);	// 05/27/07 AM.
			errOut(&gerrStr,false, parse->getInputpass(),// FIX.	// 11/12/01 AM.
						parse->getInputline());	// 05/27/07 AM.

			// Recover: Keeping first lookahead.
			}
		}
	if (f_trig)			// Current rule elt is trigger.
		{
		// Set up current elt as rule's trigger elt.
		if (!f_trigger)
			{
			f_trigger = true;
			rule->setTrigger(delt);			// Set rule's trigger elt.
			}
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Rule can't have multiple triggers.]") << std::ends;
//			parse->errOut(false,true);	// 05/27/07 AM.
			errOut(&gerrStr,false, parse->getInputpass(),// FIX.	// 11/12/01 AM.
						parse->getInputline());	// 05/27/07 AM.

			// Recover: Keeping first trigger.
			}
		}
	}

return rule;
}


/********************************************
* FN:		INTERNPRES
* CR:		07/08/99 AM.
* NOTE:	Used by rule generation algorithms.
********************************************/

void Irule::internPres(
	Dlist<Ipre> *pres,
	Dlist<Ielt> *phrase
	)
{
if (!pres || !phrase)
	return;

bool info;					// To gather trigger info.
info = false;
Delt<Ielt> *delt;
// For each rule phrase element.
int ord = 0;					// Count which element we're at.	// 11/29/98 AM.
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	++ord;
	Ielt *elt;					// Base pointer.
	elt = delt->getData();
	assert(elt);

	// Interning pre-actions is done per rule element.
	elt->internPres(pres, ord);				// 07/13/99 AM.
	}
}


/********************************************
* FN:		PRUNELISTS
* CR:		12/17/98 AM.
* SUBJ:	Remove match/fail list items that have been placed in hash table.
* NOTE:	In conjunction with interning, especially internList.  Don't remove
*			them at that time, because these lists are also used in hashing the
*			rules themselves.
*********************************************/

void Irule::pruneLists(Dlist<Irule> *rules)
{
Delt<Irule> *drule;
if (!rules)
	return;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	pruneLists(drule->getData());
	}
}
/********************************************/
void Irule::pruneLists(Irule *rule)
{
if (!rule)
	return;

// For each element, clean its lists.
Dlist<Ielt> *dlist;
if (!(dlist = rule->getPhrase()))
	return;
Delt<Ielt> *delt;
for (delt = dlist->getFirst(); delt; delt = delt->Right())
	{
	Ielt::pruneLists(delt->getData());
	}
}


/********************************************
* FN:		RHASH
* CR:		12/16/98 AM.
* SUBJ:	Install the internalized rule into a hash table, if possible.
* RET:	True if could hash the rule, else false.
* NOTE:	For optimizing analyzer speed.  Hash table sym will have a list
*			of pointers to rules.
*********************************************/

bool Irule::rhash(tHtab<Slist<Irule> > *htab,
	Parse *parse	// 04/03/03 AM.
	)
{
Irule *rule;
rule = this;

// If rule is triggered, try to hash on the trigger.
// (Assumes trigger is non-optional.)
Delt<Ielt> *dtrig;
bool done = false;
Ielt *trig;
if ((dtrig = rule->getTrigger())
	 && (trig = dtrig->getData()) )
	{
   if (Ielt::hashable(trig,parse, /*UP*/ done))
		// Trigger is hashable, so hash it.
		return Ielt::rhash(trig, htab, rule, parse, /*UP*/ done);	// 04/03/03 AM.
	else
		return false;
	}


// Else, look at first element onward, till non-optional is found.
// Or till nonhashable elt is found.
// (If wildcard along the way, fail to hash.)
// If an element has match/fail list, hash to all those.
Dlist<Ielt> *dlist;
dlist = rule->getPhrase();
if (Ielt::hashable(dlist,parse))			// If the phrase is hashable.
	{
	return Ielt::rhash(dlist, htab, rule, parse);		// then hash it.
	}

return false;
}


/********************************************
* FN:		MERGERULES
* CR:		12/18/98 AM.
* SUBJ:	Merge a copy of the second list into the first list.
* RET:	First list is synthesized, second is untouched.
* NOTE:	Merging based on rule numbers.  Used by hashed rule analyzer
*			mechanism.
* ASS:	Assumes NO redundancy in the rule lists as a whole.
*			01/23/05 AM. NO!! Because of [s] chain, lists of rules may have redundancies.
*			(Rarely occurs, so this wasn't caught before...)
*********************************************/

void Irule::mergeRules(
	Slist<Irule>* &list1,
	Slist<Irule> *list2
	)
{
if (!list2 || !(list2->getFirst()))
	return;

if (!list1)
	{
	// Copy list2.
	list1 = new Slist<Irule>(*list2);		// Copy constructor.
	return;
	}
else if (!list1->getFirst())	// List of zero elements.
	{
	delete list1;		// Delete the old one. (yawn).
	list1 = new Slist<Irule>(*list2);
	return;
	}
// Merge based on rule numbers.

// Print out the lists here. // 01/23/05 AM.

Selt<Irule> *selt1, *selt2;
Irule *rule1, *rule2;
long num1, num2, max, min;
Selt<Irule> *next1;
selt1 = list1->getFirst();
selt2 = list2->getFirst();
rule1 = selt1->getData();
rule2 = selt2->getData();
num1  = rule1->getNum();
num2  = rule2->getNum();
min = num1;

// First, add a rule that precedes the first one in list1, if any
if (num2 < num1)
	{
	list1->push(rule2);
	next1 = selt1;
	max = num1;

	selt1 = list1->getFirst();
	min = num2;

	if (!(selt2 = selt2->Right()))
		return;			// 2nd list exhausted.
	}
else if (num2 > num1)	// 09/20/06 AM.
	{
	if ((next1 = selt1->Right()))
		{
		rule1 = next1->getData();
		max = rule1->getNum();
		}
	}
else			// EQUAL.	// 09/20/06 AM.
	{
	if ((next1 = selt1->Right()))	// 09/20/06 AM.
		{
		rule1 = next1->getData();	// 09/20/06 AM.
		max = rule1->getNum();		// 09/20/06 AM.
		}
	selt2 = selt2->Right();	// Skip redundant rule.		// 09/20/06 AM.
	}

// Now list1's first rule is the lowest numbered rule of both lists.
while (next1)
	{
	// Add from list2 between selt1 and next1.
	while (selt2
			 && (rule2 = selt2->getData())
			 && (num2 = rule2->getNum())
			 && (num2 < max)
			 )
		{
		// Add rule2 onto list.
		list1->insertRight(rule2, selt1);
		selt1 = selt1->Right();									// FIX!	// 11/12/01 AM.
		selt2 = selt2->Right();
		}

	if (!selt2)
		return;							// Exhausted the 2nd list.

	// CAN HAVE REDUNDANY.										// FIX!	// 01/23/05 AM.
	if (num2 == max)												// FIX	// 01/23/05 AM.
		selt2 = selt2->Right();												// 01/23/05 AM.

	// Move ahead one rule in list1.
	selt1 = next1;						// Update the min rule.
//	min = max;												// UNNEEDED.	// 11/12/01 AM.
	if ((next1 = next1->Right()))
		{
		rule1 = next1->getData();
		max = rule1->getNum();
		}
	}

// Else, keep adding list2's rules to the end of list1.
for (; selt2; selt2 = selt2->Right())
	{
	rule2 = selt2->getData();
	list1->rpush(rule2);
	}
}


/********************************************
* FN:		GENRULEBLOCKS
* CR:		11/04/99 AM.
* SUBJ:	Generate rule regions.
* NOTE:	Not handling recurse regions here.
*			If pre,check,post actions the same, print in same @RULES block.
*********************************************/

bool Irule::genRuleblocks(
	Dlist<Irule> *rules,
	_TCHAR *sep,			// Element separator.
	std::_t_ostream &ofile)
{
if (!rules)
	return false;
Delt<Irule> *drule;
Irule *rule;
Dlist<Ipre> *pres;
Dlist<Iaction> *checks, *posts;
//for (drule = rules->getFirst(); drule; drule = drule->Right())

if (!(drule = rules->getFirst()))
	return false;
if (!(rule = drule->getData()))
	return false;
pres = rule->getPre();
checks = rule->getChecks();
posts  = rule->getPost();
for (;;)							// For each region of rules.
	{
	// At a new rule region, so print out actions.
	ofile << std::endl;
	if (pres  && pres->getFirst())
		{
		ofile << _T("@PRE") << std::endl;
		Ipre::genPres(pres, ofile);
		}
	if (checks && checks->getFirst())
		{
		ofile << _T("@CHECK") << std::endl;
		Iaction::genActions(checks, ofile);
		}
	if (posts && posts->getFirst())
		{
		ofile << _T("@POST") << std::endl;
		Iaction::genActions(posts, ofile);
		}

	ofile << _T("@RULES") << std::endl;


	// Print out the first rule of new region.
	rule->genRule(sep, ofile);
	ofile << std::endl;

	// Now compare actions of successive rules.  As long as they're the
	// same, print them in the same region.
	Irule *test;
	Dlist<Ipre> *tpres;
	Dlist<Iaction> *tchecks, *tposts;
	for (;;)			// While subsequent rules are in the same region...
		{
		if (!(drule = drule->Right()))
			return true;						// DONE WITH RULES.
		test = drule->getData();
		tpres = test->getPre();
		tchecks = test->getChecks();
		tposts = test->getPost();
		if (Iaction::same(posts,tposts)
			 && Ipre::same(pres,tpres)
			 && Iaction::same(checks,tchecks)
			 )
			{
			// Actions same.  Just print out the current rule.
			test->genRule(sep, ofile);
			ofile << std::endl;
			}
		else
			{
			// Actions differ.  Set up for next region.
			rule = test;
			pres = tpres;
			checks = tchecks;
			posts = tposts;
			break;
			}
		}
	}
return false;		// Shouldn't get down here.
}


/********************************************
* FN:		GENRULERECURSES
* CR:		11/04/99 AM.
* SUBJ:	Generate recurse rule regions based on interned rules.
* NOTE:	Generating from internal RFA is the only use so far.
*********************************************/

bool Irule::genRulerecurses(
	Dlist<Irule> *rules,
	_TCHAR *sep,			// Element separator.
	std::_t_ostream &ofile)
{
// Traverse rules to get their recurse regions.
Delt<Irule> *drule;
Irule *rule;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();

	// For each elt, generate its recurses.
	Ielt::genPhraserecurses(rule->getPhrase(), sep, ofile);
	}
return true;
}


/********************************************
* FN:		GENRULES
* CR:		05/30/99 AM.
* SUBJ:	Generate part of a rules file for a list of rules.
* ASS:	File header has been generated already.
*			Actions associated with each rule in list (ie, not concerned
*			with separate regions. Each rule is treated as a new region,
*			if it has actions.)
* NOTE:	Not handling recurse regions yet.
*********************************************/

void Irule::genRules(
	Dlist<Irule> *rules,
	_TCHAR *sep,			// Element separator.	// 11/04/99 AM.
	std::_t_ostream &ofile)
{
if (!rules)
	return;
Delt<Irule> *drule;
Irule *rule;
bool start = true;			// Start a new region.
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();
	ofile << std::endl;															// 11/04/99 AM.

	// TODO: If actions, print them here.
	if (rule->getPre()  && rule->getPre()->getFirst())	// 09/20/99 AM.
		{
		ofile << _T("@PRE") << std::endl;
		Ipre::genPres(rule->getPre(), ofile);
		start = true;
		}
	if (rule->getChecks() && rule->getChecks()->getFirst())	// 09/20/99 AM.
		{
		ofile << _T("@CHECK") << std::endl;										// 11/04/99 AM.
		Iaction::genActions(rule->getChecks(), ofile);			// 11/04/99 AM.
		start = true;
		}
	if (rule->getPost() && rule->getPost()->getFirst())		// 09/20/99 AM.
		{
		ofile << _T("@POST") << std::endl;
		Iaction::genActions(rule->getPost(), ofile);
		start = true;
		}

	ofile << _T("@RULES") << std::endl;


	// Print out the rule here.
	rule->genRule(sep, ofile);				// 11/04/99 AM.
	ofile << std::endl;
	start = false;
	}
}


/********************************************
* FN:		GENRULE
* CR:		05/30/99 AM.
* SUBJ:	Generate one rule.
* ASS:	Rule region and actions are already handled.
* NOTE:	Only used by automatically created rules (RUG) right now.
*********************************************/

void Irule::genRule(
	_TCHAR *sep,			// Elt separator (" ", "\n\t")				// 11/04/99 AM.
	std::_t_ostream &ofile,
	bool trunc																	// 06/05/00 AM.
	)
{
Irule *rule;
rule = this;																	// 06/22/99 AM.

// Print out rule here.

if (rule->sample_)				// 06/16/99 AM.
	{
	// 09/04/99 AM. BUG. If newlines in sample, screws up rules file,
	// display, whatever.  So prettifying the sample.
	_TCHAR buf[1024];
	pretty_str(rule->sample_,buf, 1024);
	ofile << _T("\n# Ex:\t")						// 11/04/99 AM.
			<< buf << std::endl;					// 09/04/99 AM.
	//ofile << rule->sample_ << std::endl;	// 09/04/99 AM.
	}

Isugg *sugg;
sugg = rule->getSugg();
Isugg::genSugg(sugg, ofile);

ofile << _T(" <-");		// Generate rule ARROW.

// Generate phrase.
Ielt::genPhrase(rule->getPhrase(), sep, ofile, trunc);

ofile << sep << _T("@@") << std::flush;		// Generate rule termination.
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	See if two rule lists are identical in form.
*********************************************/

bool Irule::same(Dlist<Irule> *list1, Dlist<Irule> *list2)
{
if (!list1 && !list2)
	return true;
if (!list1 || !list2)
	{
	if (list1 && list1->getFirst())
		return false;
	if (list2 && list2->getFirst())
		return false;
	return true;			// Empty lists even so.
	}


// Compare rule-by-rule.
Delt<Irule> *d1 = list1->getFirst();
Delt<Irule> *d2 = list2->getFirst();
if (!d1 && !d2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Irule::same: Empty lists.]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}
if (!d1 || !d2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Irule::same: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
for ( ; d1 && d2; d1 = d1->Right(), d2 = d2->Right())
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
* SUBJ:	See if two rules are identical in form.
* ASS:	Rule region and actions are already handled.
*********************************************/

bool Irule::same(Irule *rule1, Irule *rule2)
{
if (!rule1 && !rule2)
	return true;			// The same empty rule.
if (!rule1 || !rule2)
	return false;

if (!Ipre::same(rule1->getPre(), rule2->getPre()))
	return false;
if (!Iaction::same(rule1->getChecks(), rule2->getChecks()))
	return false;
if (!Iaction::same(rule1->getPost(), rule2->getPost()))
	return false;

if (!Isugg::same(rule1->getSugg(), rule2->getSugg()))
	return false;
if (!Ielt::same(rule1->getPhrase(), rule2->getPhrase()))
	return false;

// If phrase checks out, trigger elt automatically the same.
//rule1->getTrigger();

return true;
}


/********************************************
* FN:		DEDUP
* CR:		06/07/99 AM.
* SUBJ:	Remove redundant rules from a list.
* NOTE:	Used by RUG automated rule generation.
*			N-squared "bubble" algorithm.
* WARN:	MODIFIES GIVEN RULE LIST.
*			ASSUMES RULES OWN THEIR ACTIONS.
*********************************************/

Dlist<Irule> *Irule::dedup(Dlist<Irule> *list)
{
if (!list)
	return 0;

Irule *rule;
Delt<Irule> *drule, *test, *dnext;
for (drule = list->getFirst(); drule; drule = drule->Right())
	{
	// Traverse all subsequent rules, looking for redundancy with
	// current rule.
	test = drule->Right();
	while (test)
		{
		dnext = test->Right();	// Get next guy ahead of time.
		rule = test->getData();
		if (same(drule->getData(), rule))
			{
			// Remove redundant rule.
			//*gerr << "[FOUND REDUNDANT!]" << std::endl;
			list->Dlist<Irule>::exciseElt(test);
			Irule::deleteOwning(rule);					// 07/03/99 AM.
			Delt<Irule>::DeleteDeltAndData(test);	// 07/03/99 AM.
			//delete test;									// 07/03/99 AM.
			}
		test = dnext;
		}
	}

return list;
}


/********************************************
* FN:		TRIGRULES
* CR:		10/07/99 AM.
* SUBJ:	Build triggers for a list of rules.
* ASS:	Used for automatically generated rules (RUG).
*********************************************/

void Irule::trigRules(Dlist<Irule> *rules)
{
if (!rules)
	return;
Delt<Irule> *drule;
Irule *rule;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();

	// Build trigger for one rule.
	rule->trigRule();
	}
}


/********************************************
* FN:		TRIGRULE
* CR:		10/07/99 AM.
* SUBJ:	Build a trigger for a rule.
* NOTE:	Only used by automatically created rules (RUG) right now.
*********************************************/

void Irule::trigRule()
{
Irule *rule;
rule = this;

if (rule->getTrigger())
	return;						// Rule already has a trigger.

// Traverse rule elements looking for the first good candidate.
// (If first elt is a good candidate, don't need triggering.)
Delt<Ielt> *dtrig = Ielt::trigPhrase(rule->getPhrase());
if (!dtrig)
	return;
Ielt *trig = dtrig->getData();

// If found good candidate, make it the trigger.
// (And tell the elt that it is the trigger elt.)
rule->setTrigger(dtrig);
trig->setTrigger(true);
}


/********************************************
* FN:		QUICKSEM
* CR:		12/27/99 AM.
* SUBJ:	Automatically transfer labels to semantic variables.
* NOTE:	Only used by automatically created rules (RUG) right now.
* VARIANT:	This variant takes a list of rules.
*********************************************/

void Irule::quickSem(Dlist<Irule> *list)
{
Delt<Irule> *delt;
if (!list)
	return;
for (delt = list->getFirst(); delt; delt = delt->Right())
	delt->getData()->quickSem();
}


/********************************************
* FN:		QUICKSEM
* CR:		12/27/99 AM.
* SUBJ:	Automatically transfer labels to semantic variables.
* NOTE:	Only used by automatically created rules (RUG) right now.
* VARIANT:	This variant takes a single rule.
*********************************************/

void Irule::quickSem()
{
Irule *rule = this;
Dlist<Iaction> *posts = rule->getPost();
Dlist<Ielt> *phrase = rule->getPhrase();

if (!phrase)
	return;

bool built = false;		// Haven't built new post actions yet.

Delt<Iaction> *dfirst = 0, *dnew = 0;
if (posts)
	dfirst = posts->getFirst();

// For every GROUP post action, add an NLP++ action immediately following.
Delt<Iaction> *tmp, *daction;											// 12/28/99 AM.
Iaction *act = 0, *newact = 0;
int first = 0, last = 0;
_TCHAR *label;
daction = dfirst;
while (daction)																	// 12/28/99 AM.
	{
	act = daction->getData();
	if (!strcmp_i(_T("group"), act->getName()))	// GROUP action.	// 12/28/99 AM.
		{
		// Not setting built, because post actions already set up.
		// Unpack. Group args are first,last,label.
		if (!Ipost::unpackGroup((Ipost *)act, /*UP*/ first,last,label))
			return;	// Recover from bad group action.
		// Build: S("label") = N("$text",first);
		newact = actionQuicksem(label, first);

		// Insert immediately after the group action.
		tmp = daction->Right();
		dnew = new Delt<Iaction>(newact);
		posts->insertRight(dnew,daction);
		daction = tmp;						// Continue traversing actions.
		}
	else
		daction = daction->Right();
	}

// For every layered elt, add an NLP++ action.
Dlist<Iaction> *acts = new Dlist<Iaction>();				// Make empty list.
Delt<Ielt> *delt;
Ielt *elt;
_TCHAR *layer;
int count = 0;			// Count rule elts.
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	++count;
	elt = delt->getData();
	if ((layer = elt->lastAttr()))		// Get last layer from elt.
		{
		built = true;
		// Build and attach action.
		// Build S("layer") = N("$text", ord);
		// Build 2 vars, glom into an assignment expression, convert to
		// an NLP++ statement, install in a post action, add to list.
		act = actionQuicksem(layer, count);
		acts->rpush(act);					// Attach to action list.
		}
	}

if (!built)			// No NLP++ actions were built.
	return;

if (!dfirst)	// Rule had no post actions.
	{
	// Build a single() action at end of post actions.
	act = new Iaction();
	act->setName(_T("single"));		// Single() action with no args.
	acts->rpush(act);
	}

// Do the right thing if rule had no post actions, an empty, or full list.
// Including deletion of whichever unused list structure.
rule->setPost(Dlist<Iaction>::merge(acts,posts));
}


/********************************************
* FN:		ACTIONQUICKSEM
* CR:		12/28/99 AM.
* SUBJ:	Build NLP++ to copy node text to sugg variable.
* RET:	Action built, or 0 if error.
* FORM:	S("name") = N("$text", ord);
*********************************************/

inline Iaction *Irule::actionQuicksem(
	_TCHAR *label,		// Name of labeled rule element.
	int ord				// Position of labeled rule element.
	)
{
if (!label)
	return 0;

// Slough the initial underscore.
if (*label == '_')
	++label;
// Build S("layer") = N("$text", ord);
// Build 2 vars, glom into an assignment expression, convert to
// an NLP++ statement, install in a post action, add to list.
NUG *nug = Nug::makeAssign(
				Nug::makeVar(_T("S"), label, 0),
				Nug::makeVar(_T("N"), _T("$text"), ord)
				);
nug = Nug::makeExprstmt(nug);
Iaction *act = new Iaction();
act->setSem((RFASem *)nug);
return act;
}


/********************************************
* FN:		RUGCOUNTER
* CR:		11/27/00 AM.
* SUBJ:	Generate code to count number of times a rule has matched.
* NOTE:	Only used by automatically created rules (RUG) right now.
* VARIANT:	This variant takes a list of rules.
*********************************************/

void Irule::rugCounter(Dlist<Irule> *list)
{
Delt<Irule> *delt;
if (!list)
	return;
for (delt = list->getFirst(); delt; delt = delt->Right())
	delt->getData()->rugCounter();
}



/********************************************
* FN:		RUGCOUNTER
* CR:		11/27/00 AM.
* SUBJ:	Automatically count times a rule has matched.
* NOTE:	Only used by automatically created rules (RUG) right now.
*			Can place action as first action.
*			Assume it's always a global G variable, for now.
* VARIANT:	This variant takes a single rule.
*********************************************/

void Irule::rugCounter()
{
Irule *rule = this;
Dlist<Iaction> *posts = rule->getPost();
if (!posts)		// Make an empty list.
	{
	posts = new Dlist<Iaction>();
	rule->setPost(posts);	// Attach to rule.
	}

bool firstaction = (posts->getFirst() ? false : true);

// VARIABLE NAME == NAME OF SUGGESTED CONCEPT.
Isugg *sugg = rule->getSugg();
_TCHAR *var_str = sugg->getName();
if (var_str && (*var_str == '_'))
	++var_str;			// Slough initial underscore.

// Add a count statement at beginning of post actions.
Iaction *newact = actionRugcounter(var_str);
posts->push(newact);

// If there were no posts, add a single action also.
Iaction *act = 0;
if (firstaction)
	{
	// Build a single() action at end of post actions.
	act = new Iaction();
	act->setName(_T("single"));		// Single() action with no args.
	posts->rpush(act);
	}
}


/********************************************
* FN:		ACTIONRUGCOUNTER
* CR:		11/27/00 AM.
* SUBJ:	Build NLP++ to increment rule-match counter.
* RET:	Action built, or 0 if error.
* FORM:	++G("suggname");
*********************************************/

inline Iaction *Irule::actionRugcounter(
	_TCHAR *var_str	// Name of global variable to increment.
	)
{
if (!var_str || !*var_str)
	return 0;

// Build ++G("var_str");
NUG *nug = Nug::makeInc(
				Nug::makeVar(_T("G"), var_str, 0)
				);
nug = Nug::makeExprstmt(nug);
Iaction *act = new Iaction();
act->setSem((RFASem *)nug);
return act;
}


/********************************************
* FN:		RENUMBER
* CR:		05/04/00 AM.
* SUBJ:	Assign numbers to rule list.
*********************************************/

void Irule::renumber(Dlist<Irule> *rules)
{
if (!rules)
	return;
long num = 0;
Delt<Irule> *drule;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	drule->getData()->setNum(++num);
}


/********************************************
* FN:		EHASH
* CR:		05/02/00 AM.
* SUBJ:	Hash elts for a list of interned rules.
*********************************************/

void Irule::ehash(Dlist<Irule> *rules)
{
Delt<Irule> *drule;
if (!rules)
	return;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	drule->getData()->ehash();
	}
}
/********************************************/
void Irule::ehash()
{
Irule *rule = this;

// For each element, hash its lists.
Dlist<Ielt> *dlist;
if (!(dlist = rule->getPhrase()))
	return;
Delt<Ielt> *delt;
for (delt = dlist->getFirst(); delt; delt = delt->Right())
	{
	delt->getData()->ehash();
	}
}


/********************************************
* FN:		GEN
* CR:		05/01/00 AM.
* SUBJ:	Generate code for an interned rule.
* ASS:	We're in a function where rules are gen'd as cases
*			in a switch statement
* NOTE:	Based on pat/matchRule().
********************************************/

bool Irule::gen(Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
gen->region_ = REGRULE;		// Says that we're in a rule.
_TCHAR indent[64];
_TCHAR *saveindent = gen->indent_;
_stprintf(indent, _T("%s\t"), saveindent);

// Gather needed data.
//Isugg *sugg;				// Suggested elt.
//Delt<Ielt> *trig;			// Rule's trigger elt.
Delt<Ielt> *ptr;
//if (!unpack(/*UP*/ rlist,sugg,trig))
//	return false;
if (!phrase_ || !sugg_)														// 06/14/00 AM.
	return false;																// 06/14/00 AM.
Delt<Ielt> *rphrase = phrase_->getFirst();			// Rule phrase.
if (!rphrase)
	return false;

//////////////////////////////////
// Generate data for suggested elt.										// 05/19/00 AM.
_TCHAR asugg[256];
_stprintf(asugg, _T("sugg%d_%d_%d"),											// 05/30/00 AM.
				gen->id_,gen->recid_,gen->ruleid_);						// 05/30/00 AM.
sugg_->gen(asugg, gen,														// 05/19/00 AM.
				gen->id_,line_													// 08/09/02 AM.
				);
//////////////////////////////////

_TCHAR eltarr[64];
bool haswild = hasWild();	// If rule has any wildcards.			// 05/17/00 AM.
int rulelen = phrase_->getLength();										// 05/18/00 AM.
_stprintf(eltarr, _T("elts%d_%d_%d"),											// 05/30/00 AM.
				gen->id_,gen->recid_,gen->ruleid_);						// 05/30/00 AM.
Ielt::genElts(phrase_,eltarr,gen);	// Array for rule elts.		// 05/17/00 AM.

// Initialize rule data.
*fcode << indent
		 << _T("Arun::init_rule(")
		 << rulelen << _T(",")													// 05/18/00 AM.
		 << asugg << _T(",")														// 05/19/00 AM.
		 << eltarr << _T(",");

// If rule has pre actions, include the fn gen'ed by genPres()	// 05/24/00 AM.
if (hasPres())																	// 05/24/00 AM.
	{
	*fcode << _T("pres") << gen->id_ << _T("_")									// 05/24/00 AM.
			 << gen->recid_ << _T("_")											// 05/30/00 AM.
			 << gen->ruleid_ << _T(",");										// 05/24/00 AM.
	}
else
	*fcode << _T("0") << _T(",");													// 05/24/00 AM.

// If rule has a lookahead elt, record its ord.						// 06/10/00 AM.
*fcode << lookahead_ << _T(",");												// 06/10/00 AM.
											
*fcode << _T("nlppp);");
//		 << std::endl;																// 05/17/00 AM.
Gen::nl(fcode);																// 04/04/03 AM.

long id = num_;

// Gen: match trigger or first elt of rule.
gen->indent_ = indent;
int eltno=0;		// Keep running count of elts in the order we visit them.
if (!trigger_
	 || !trigger_->pLeft)		// If trigger is first elt.	// OPT	// 06/07/00 AM.
	{
	gen->setEltid(++eltno);
	Ielt::genFirst(rphrase, gen);
	while ((rphrase = rphrase->Right()))
		{
		gen->setEltid(++eltno);
		Ielt::genRight(rphrase, gen);
		}
	}
else
	{
	int ord = phrase_->getOrd(trigger_);					// Trig num		// 06/07/00 AM.
	eltno = ord;													// FIX	// 06/07/00 AM.
	gen->setEltid(eltno);										// FIX	// 06/07/00 AM.
	Ielt::genTrigger(trigger_, gen);
	bool left = false;		// If elts to left of trigger.		// 05/30/00 AM.
	for (ptr = trigger_->Left(); ptr; ptr = ptr->Left())
		{
		if (!left)																// 06/01/00 AM.
			*fcode << indent
					 << _T("Arun::switch_left(nlppp);");						// 06/01/00 AM.
		Gen::nl(fcode);														// 04/04/03 AM.
		left = true;															// 05/30/00 AM.
		gen->setEltid(--eltno);									// FIX	// 06/07/00 AM.
		Ielt::genLeft(ptr, gen);
		}

	if (left)																	// 05/30/00 AM.
		*fcode << indent
				 << _T("Arun::switch_right(nlppp);");						// 05/30/00 AM.
		Gen::nl(fcode);														// 04/04/03 AM.

	eltno = ord;				// Set to trigger num.		// FIX	// 06/07/00 AM.
	for (ptr = trigger_->Right(); ptr; ptr = ptr->Right())
		{
		gen->setEltid(++eltno);
		Ielt::genRight(ptr, gen);
		}
	}

// Gen: If success, perform check and post actions.
gen->region_ = REGCHECK;													// 05/30/00 AM.
bool found = genChecks(checks_, gen);									// 05/20/08 AM.

if (gen->label_)	// @CHECK region had a SUCCEED action.			// 06/06/00 AM.
	{
	_TCHAR label[64];
	_stprintf(label, _T("L%d_%d_%d"), gen->id_,gen->recid_,gen->ruleid_);
	*fcode << std::endl;			// Just in case.							// 04/04/03 AM.
	*fcode << label << _T(":") << std::endl;
	gen->label_ = false;		// RESET.									// 06/06/00 AM.
	}

// If there were check actions, then generate a cleanup of local vars, etc. // 05/20/08 AM.
if (found)																		// 05/20/08 AM.
  *fcode << indent << _T("Arun::checkend(nlppp);") << std::endl;		// 05/20/08 AM.

gen->region_ = REGPOST;
genPost(post_, gen);

gen->indent_ = saveindent;
return true;
}


/********************************************
* FN:		GENPOST
* CR:		05/04/00 AM.
* SUBJ:	Generate code for post actions.
* RET:	True if ok, else false.
********************************************/

bool Irule::genPost(Dlist<Iaction> *posts, Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.

long id = num_;

// Need to generate code for all the post action tree.
//*fcode << "\t\t" << "exec_actions(nlppp);" << std::endl;
_TCHAR *indent = _T("\t\t");

// Modeled on postActions.

// Gen'ing fired flags to log files, if post actions present.	// 08/09/02 AM.
// Allowing matched_rule to reject the match, eg no nodes matched.	// 05/19/08 AM.
if (!posts)																		// 08/09/02 AM.
	*fcode << indent
			 << _T("if (!Arun::matched_rule(nlppp,false)) return false;");// 05/19/08 AM.
else																				// 08/09/02 AM.
	*fcode << indent															// 08/09/02 AM.
			 << _T("if (!Arun::matched_rule(nlppp,true)) return false;") << std::endl;// 05/19/08 AM.
Gen::nl(fcode);																// 04/04/03 AM.

// Walk the rule phrase elements and gen their actions here.
// Moved this before default post action gen.					// 05/23/00 AM.
genExecphrase(gen);


if (!posts)
	{
	*fcode << indent << _T("return Arun::default_post_action(nlppp);");
	Gen::nl(fcode);															// 04/04/03 AM.
	return true;
	}


/////////////////////////////////////////////
// Traverse the post actions themselves here.
/////////////////////////////////////////////

if (posts)
	{
	// PUTTING A TRY BLOCK FOR SUCCEED,FAIL,EXITPASS.			// 09/15/08 AM.
	*fcode << indent;														// 09/15/08 AM.
	*fcode << _T("try {");												// 09/15/08 AM.
	Gen::nl(fcode);														// 09/15/08 AM.
	}

//*fcode << indent << "// Traverse post actions." << std::endl;
Delt<Iaction> *dpost;
Iaction *post;
RFASem *rfasem, *val = 0;
bool badname = false;
for (dpost = posts->getFirst(); dpost; dpost = dpost->Right())
	{
	post = dpost->getData();
	if ((rfasem = post->getSem()))
		{
		rfasem->genEval(gen);	// GENERATE CODE FOR NLP++.
		//*fcode << indent << "if (val) delete val;" << std::endl;
		continue;
		}
	// Well, we do reach here when actions look like "fn()".
	// Old-style and used by RFA.
	*fcode << indent;
	post->genEval(fcode, gen,true);
	*fcode << _T(";");
	Gen::nl(fcode);															// 04/04/03 AM.
	}

if (posts)																	// 09/15/08 AM.
	{
	*fcode << indent;														// 09/15/08 AM.
//	*fcode << _T("} catch (...) {}");						// 09/15/08 AM.
	*fcode << _T("} catch (int e) {e=e;}");						// 09/16/08 AM.
	Gen::nl(fcode);														// 09/15/08 AM.
	}

*fcode << indent
		 << _T("if (Arun::post_final(nlppp)) return true;");			// 05/25/00 AM.
Gen::nl(fcode);																// 04/04/03 AM.
return true;
}

/********************************************
* FN:		GENCHECKS
* CR:		05/30/00 AM.
* SUBJ:	Generate code for check actions.
* RET:	True if generated one or more actions, else false.	05/20/08 AM.
********************************************/

bool Irule::genChecks(Dlist<Iaction> *checks, Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.

//long id = num_;

_TCHAR *indent = _T("\t\t");

if (!checks)
	return false;	// 05/20/08 AM.

// PUTTING A TRY BLOCK FOR SUCCEED,FAIL,EXITPASS.			// 09/15/08 AM.
*fcode << indent;														// 09/15/08 AM.
*fcode << _T("try {");												// 09/15/08 AM.
Gen::nl(fcode);														// 09/15/08 AM.

/////////////////////////////////////////////
// Traverse the actions themselves here.
/////////////////////////////////////////////
//*fcode << indent << "// Traverse post actions." << std::endl;
Delt<Iaction> *dcheck;
Iaction *check;
RFASem *rfasem, *val = 0;
bool badname = false;
bool found = false;	// 05/20/08 AM.
for (dcheck = checks->getFirst(); dcheck; dcheck = dcheck->Right())
	{
	check = dcheck->getData();
	if ((rfasem = check->getSem()))
		{
		found = true;	// 05/20/08 AM.
		rfasem->genEval(gen);	// GENERATE CODE FOR NLP++.
		//*fcode << indent << "if (val) delete val;" << std::endl;
		continue;
		}
	// Well, we do reach here when actions look like "fn()".
	// Old-style and used by RFA.
	*fcode << indent;
	check->genEval(fcode, gen,true);
	*fcode << _T(";");
	Gen::nl(fcode);															// 04/04/03 AM.
	}

*fcode << indent;														// 09/15/08 AM.
*fcode << _T("} catch (int e) {if (e) return Arun::checkfail(nlppp,e);}");// 09/15/08 AM.
Gen::nl(fcode);														// 09/15/08 AM.

return found;	// 05/20/08 AM.
}


/********************************************
* FN:		GENCODES
* CR:		06/06/00 AM.
* SUBJ:	Generate code for actions in the @CODE region.
* RET:	True if ok, else false.
********************************************/

bool Irule::genCodes(Dlist<Iaction> *codes, Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.

//long id = num_;

_TCHAR *indent = _T("\t\t");



if (!codes)
	return true;

// PUTTING A TRY BLOCK FOR SUCCEED,FAIL,EXITPASS.			// 09/15/08 AM.
*fcode << _T("try {");												// 09/15/08 AM.
Gen::nl(fcode);														// 09/15/08 AM.

/////////////////////////////////////////////
// Traverse the actions themselves here.
/////////////////////////////////////////////
//*fcode << indent << "// Traverse post actions." << std::endl;
Delt<Iaction> *dcode;
Iaction *code;
RFASem *rfasem, *val = 0;
bool badname = false;
for (dcode = codes->getFirst(); dcode; dcode = dcode->Right())
	{
	code = dcode->getData();
	if ((rfasem = code->getSem()))
		{
		rfasem->genEval(gen);	// GENERATE CODE FOR NLP++.
		//*fcode << indent << "if (val) delete val;" << std::endl;
		continue;
		}
	// Well, we do reach here when actions look like "fn()".
	// Old-style and used by RFA.
	*fcode << indent;
	code->genEval(fcode, gen,true);
	*fcode << _T(";");
	Gen::nl(fcode);															// 04/04/03 AM.
	}

//*fcode << _T("} catch (...) {}");								// 09/15/08 AM.
*fcode << _T("} catch (int e) {e=e;}");						// 09/16/08 AM.
Gen::nl(fcode);														// 09/15/08 AM.

return true;
}

/********************************************
* FN:		GENEXECPHRASE
* CR:		05/05/00 AM.
* SUBJ:	Generate code for pairs of each rule element.
* RET:	True if ok, else false.
* NOTE:	Modelled after Pat::execPhrase.
********************************************/

bool Irule::genExecphrase(Gen *gen)
{
Dlist<Ielt> *phrase = getPhrase();
if (!phrase)
	return false;
if (!Ielt::phraseActions(phrase))
	return true;

std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
_TCHAR *indent = gen->indent_;

//*fcode << indent << "// Exec Phrase" << std::endl;

// Using array indexing rather than traversing the collect.		// 05/23/00 AM.
int count = 0;																	// 05/23/00 AM.
//*fcode << indent << "pcoll = Arun::get_colls(nlppp);" << std::endl;// 05/23/00 AM.

// Walk the rule elements, generating their "pair" actions.
//*fcode << indent << "Pn *data;" << std::endl;


Delt<Ielt> *delt;
Ielt *elt;
Dlist<Seqn> *passes;
_TCHAR *ren;
_TCHAR *gp;																		// 09/23/00 AM.
// Should see if any element has a pair action.  If not, need not
// traverse them at all!  (Could have a flag for the interpreted mode,
// even!)
_TCHAR buf[MAXSTR+1];
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	gen->setEltid(++count);													// 05/31/00 AM.
	elt = delt->getData();
	if ((passes = elt->getPasses()))
		Seqn::genRecurse(passes, gen);									// 05/31/00 AM.
	if ((gp = elt->getGroup()))												// 09/23/00 AM.
		{
		*fcode << indent << _T("Arun::group_elt(")							// 09/23/00 AM.
				 << count  << _T(",")												// 09/23/00 AM.
				 << _T("_T(\"")
				 << c_str(gp, buf, MAXSTR)
				 << _T("\")")
				 << _T(",") << _T("nlppp")
				 << _T(");  /* Group. */");
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	if ((ren = elt->getRename()))
		{
//		*fcode << indent << "// For each node matched by rule elt...." << std::endl;
//		*fcode << indent << "// Could collapse nodes then rename...." << std::endl;
//		*fcode << indent << "Arun::set_node_name(pcoll,"			// 05/23/00 AM.
		*fcode << indent << _T("Arun::set_node_name(")					// 05/23/00 AM.
				 << count  << _T(",")												// 05/23/00 AM.
				 << _T("_T(\"")
				 << c_str(ren, buf, MAXSTR)
				 << _T("\")")
				 << _T(",") << _T("nlppp")											// 05/23/00 AM.
				 << _T(");  /* Rename. */");
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	Starr *starr;
	if ((starr = elt->getLayers()))											// 05/06/00 AM.
		{
		_TCHAR a_attrs[MAXSTR];
		_stprintf(a_attrs, _T("attr%d_%d_%d_%d"),
				gen->id_,gen->recid_,gen->ruleid_,gen->eltid_);		// 05/30/00 AM.
			// getNum());

		// This is already done in generating the array for all elts.
		// Ielt::genEltstarr(starr, a_attrs, gen);					// 05/31/00 AM.

//		*fcode << indent << "//Layering that takes array of names." << std::endl;
		*fcode << indent
//			<< "Arun::layer_elt(pcoll,"									// 05/23/00 AM.
			<< _T("Arun::layer_elt(")											// 05/23/00 AM.
			<< count << _T(",")													// 05/23/00 AM.
			<< a_attrs
			<< _T(",nlppp);");
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	//if (delt->Right())														// 05/23/00 AM.
	//	*fcode << indent														// 05/23/00 AM.
	//			 << "pcoll = Arun::next_node(pcoll);" << std::endl;		// 05/23/00 AM.
	}

return true;
}


/********************************************
* FN:		HASWILD
* CR:		05/17/00 AM.
* SUBJ:	See if rule has any unrestricted wildcards.
* RET:	True if has a wildcard, else false.
********************************************/

bool Irule::hasWild()
{
Delt<Ielt> *delt = phrase_->getFirst();
for (; delt; delt = delt->Right())
	{
	if (delt->getData()->wild())
		return true;
	}
return false;
}


/********************************************
* FN:		HASPRES
* CR:		05/24/00 AM.
* SUBJ:	See if any rule element has pre actions.
* RET:	True if has a pre action, else false.
********************************************/

bool Irule::hasPres()
{
Delt <Ielt> *delt = phrase_->getFirst();
for (; delt; delt = delt->Right())
	{
	if (delt->getData()->getPres())
		return true;
	}
return false;
}



/********************************************
* FN:		GENPRES
* CR:		05/23/00 AM.
* NOTE:	Generate code for preactions for rules of a pass.
*			05/30/00 AM. Moved here from ifile.
********************************************/

bool Irule::genPres(Dlist<Irule> *rules, Gen *gen)
{
std::_t_ofstream *faux = gen->faux_;
std::_t_ofstream *fhead = gen->fhead_;
int id = gen->id_;
_TCHAR *indent;

_TCHAR prebuf[64];

Delt<Irule> *drule;
Irule *rule;
int ruleno=0;	// Rule has its count, but this is faster.
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();
	gen->setRuleid(++ruleno);
	if (!rule->hasPres())		// No elt of rule has preactions.
		continue;					// Go to next rule.

	// Rule has preactions, so generate a function.

	_stprintf(prebuf, _T("bool pres%d_%d_%d(int ord, Nlppp *nlppp)"),
				id, gen->recid_, ruleno);									// 05/30/00 AM.
	*fhead << prebuf << _T(";");
	Gen::nl(fhead);															// 04/04/03 AM.

	*faux << prebuf;
	Gen::nl(faux);																// 04/04/03 AM.
	indent = _T("\t");
	*faux << _T("{");
	Gen::nl(faux);																// 04/04/03 AM.
	*faux << _T("switch (ord)");
	Gen::nl(faux);																// 04/04/03 AM.
	*faux << indent << _T("{");
	Gen::nl(faux);																// 04/04/03 AM.

	///////////////////////////////////////////
	// Generate code for rule's preactions here.
	// For each rule element, gen code for its preactions.
	Dlist<Ielt> *phrase = rule->getPhrase();
	Delt<Ielt> *delt;
	Ielt *ielt;
	Dlist<Ipre> *pres;
	Delt<Ipre> *dpre;
	Ipre *ipre;
	int count = 0;
	for (delt = phrase->getFirst(); delt; delt = delt->Right())
		{
		ielt = delt->getData();
		++count;
		if (!(pres = ielt->getPres()))
			continue;
		if (!(dpre = pres->getFirst()))
			continue;
		*faux << indent << _T("case ") << count << _T(":");
		Gen::nl(faux);															// 04/04/03 AM.
		// For each preaction of current elt.
		for (; dpre; dpre = dpre->Right())
			{
			ipre = dpre->getData();
			*faux << indent << _T("\t") << _T("if (!(");
			ipre->genEval(faux, gen,true);
			*faux << _T(")) return false;");
			Gen::nl(faux);														// 04/04/03 AM.
			}
		*faux << indent << _T("\t") << _T("return true;");
		Gen::nl(faux);															// 04/04/03 AM.
		}
	///////////////////////////////////////////

	*faux << indent << _T("default:");
	Gen::nl(faux);																// 04/04/03 AM.
	*faux << indent << _T("\t") << _T("return true;");
	Gen::nl(faux);																// 04/04/03 AM.
	*faux << indent << _T("}");
	Gen::nl(faux);																// 04/04/03 AM.
	*faux << _T("}");
	Gen::eol(faux);															// 04/04/03 AM.
	}
return true;
}


/********************************************
* FN:		GENRULE
* CR:		05/02/00 AM.
* NOTE:	Generate code for matching each rule against parse tree nodes.
*			Modularity function.
*			START WITH UNHASHED VERSION!!
********************************************/

bool Irule::genRule(Dlist<Irule> *rules, _TCHAR *rulebuf, Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.

*fcode << _T("bool ") << rulebuf;
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.

*fcode << _T("NODE *pcoll=0;");
Gen::nl(fcode);																// 04/04/03 AM.

*fcode << _T("switch (ruleno)");
Gen::nl(fcode);																// 04/04/03 AM.

_TCHAR *saveindent = gen->indent_;
_TCHAR indent[64];
_stprintf(indent, _T("%s\t"), saveindent);
gen->indent_ = indent;

*fcode << indent << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.

Delt<Irule> *drule;
Irule *rule;
int ruleno=0;	// Rule has its count, but this is faster.
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();
	gen->setRuleid(++ruleno);

	// Gen code for rule.
	*fcode << indent << _T("case ")
			 //<< rule->getNum()	// Bug. Not set in recurse rule. // 05/31/00 AM.
			 << ruleno					// Fix.								// 05/31/00 AM.
			 << _T(":");
	Gen::nl(fcode);															// 04/04/03 AM.

	// Pretty-printing the rule also.									// 05/19/00 AM.
	*fcode << indent << _T("\t") << _T("/* ");									// 04/04/03 AM.
	rule->genRule(_T(" "), *fcode,												// 05/19/00 AM.
						true);	// TRUNCATE LONG LISTS.					// 06/05/00 AM.
	*fcode << _T(" */");															// 04/04/03 AM.
	Gen::eol(fcode);															// 04/04/03 AM.

	// Match trigger.
	// Move left.
	// Move right.
	// If succeeded, perform check and post actions.
	rule->gen(gen);
	*fcode << indent << _T("\t") << _T("break;");
	Gen::eol(fcode);															// 04/04/03 AM.
	}


*fcode << indent << _T("default:");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << gen->indent_ << _T("done = true;");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << gen->indent_ << _T("return false;");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << indent << _T("}");
Gen::nl(fcode);																// 04/04/03 AM.

//*fcode << "nlppp->node_ = node;" << std::endl;
*fcode << _T("return false;");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("}");
Gen::nl(fcode);																// 04/04/03 AM.
Gen::eol(fcode);																// 04/04/03 AM.
gen->indent_ = saveindent;
return true;
}


/********************************************
* FN:		GENHASH
* CR:		06/14/00 AM.
* SUBJ:	Generate code for hashing rules and their elements.
* NOTE:	Generate the hash tables themselves.
********************************************/

bool Irule::genHash(Dlist<Irule> *rules, Gen *gen)
{
Delt<Irule> *drule;
Irule *rule;
gen->ruleid_ = 0;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	++(gen->ruleid_);															// 06/14/00 AM.
	rule = drule->getData();
	rule->genHash(gen);
	}
return true;
}


/********************************************
* FN:		GENHASH
* CR:		06/14/00 AM.
* SUBJ:	Generate code for hashing an interned rule and its elements.
********************************************/

bool Irule::genHash(Gen *gen)
{
if (!phrase_ || !sugg_)
	return false;

// Generate hash table for each rule element.
Ielt::genHash(phrase_, gen);

// Generate hash table for the entire rule, based on trigger, else first
// set of optional elements, till a nonoptional is seen.

return true;
}


/********************************************
* FN:		GENMUSTS
* CR:		06/16/00 AM.
* SUBJ:	Generate code for pass' musts list of rules.
********************************************/

bool Irule::genMusts(Slist<Irule> *musts, Gen *gen)
{
if (!musts)
	return true;

long len = musts->getLength();											// 06/19/00 AM.
if (len <= 0)																	// 06/19/00 AM.
	return true;			// Ok, but no code.							// 06/19/00 AM.

std::_t_ofstream *rdata = gen->rdata_;
std::_t_ofstream *rhead = gen->rhead_;

_TCHAR *mustname = _T("must");		// For single organizing struct.	// 06/19/00 AM.
_TCHAR *mustsname = _T("musts");			// For array of rule numbers.		// 06/19/00 AM.
int count=0;						// Count rules per line.
int maxline = 20;					// Max rule numbers per line.

_TCHAR mustbuf[MAXSTR];
_TCHAR mustsbuf[MAXSTR];
_stprintf(mustsbuf, _T("%s%d_%d"), mustsname, gen->id_,gen->recid_);
_stprintf(mustbuf, _T("%s%d_%d"), mustname, gen->id_, gen->recid_);

//*rhead << "extern const int " << mustsbuf << "[];" << std::endl;

*rdata << _T("const int ")
		 << mustsbuf
		 << _T("[] ={");

Selt<Irule> *selt;
Irule *rule;
for (selt = musts->getFirst(); selt; selt = selt->Right())
	{
#ifdef GENPRETTY_
	if (++count == maxline)
		{
		*rdata << std::endl << _T("\t");
		count = 0;
		}
#endif
	rule = selt->getData();
	*rdata << rule->num_
			 << _T(",");
	}

*rdata << _T("0};");		// Null termination of array.
Gen::eol(rdata);	// 04/04/03 AM.

// Must is now a struct.													// 06/19/00	AM.
*rhead << _T("extern const RHASH ") << mustbuf << _T(";");
Gen::eol(rhead);	// 04/04/03 AM.

*rdata << _T("const RHASH ") << mustbuf << _T("={")
		 << _T("0,")
		 << len << _T(",")
		 << mustsbuf
		 << _T("};");
Gen::eol(rdata);	// 04/04/03 AM.
return true;
}


/********************************************
* FN:		GENCONFLICTS
* CR:		06/17/00 AM.
* SUBJ:	Generate code for hash conflict chain.
* RET:	Length of conflict chain.
* NOTE:	Generate rule number for each rule in chain.
********************************************/

long Irule::genConflicts(Slist<Irule> *list, std::_t_ostream *ostr)
{
if (!list || !ostr)
	return false;
 

Selt<Irule> *selt;
Irule *rule;
long count = 0;
for (selt = list->getFirst(); selt; selt = selt->Right())
	{
	++count;																		// 06/19/00 AM.
	rule = selt->getData();
	*ostr << rule->num_ << _T(",");
	}
*ostr << _T("0");		// NULL TERMINATION!
return count;																	// 06/19/00 AM.
}

/*************************      END OF FILE      ******************************/
