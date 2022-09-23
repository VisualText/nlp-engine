/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NLPPP.CPP
* FILE:	lite/nlppp.cpp
* CR:		11/14/99 AM.
* SUBJ:	Control state of the pattern matcher.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>	// 09/27/19 AM.
#include <strstream>	// 09/27/19 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"
#include "io.h"
#include "iarg.h"
#include "str.h"
#include "chars.h"	// 01/08/07 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "parse.h"		// 12/29/99 AM.
#include "nlp.h"			// 06/25/03 AM.
#include "lite/nlppp.h"

#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "ana.h"			// 12/29/99 AM.
#include "irule.h"		// 12/21/01 AM.

#ifdef LINUX
template<>
int Selt<_TCHAR>::count_	= 0;	// 01/08/07 AM.
template<>
int Slist<_TCHAR>::count_	= 0;	// 01/08/07 AM.
#else
int Selt<_TCHAR>::count_	= 0;	// 01/08/07 AM.
int Slist<_TCHAR>::count_	= 0;	// 01/08/07 AM.
#endif

int Nlppp::count_ = 0;

/********************************************
* FN:		Special Functions for Nlppp Class
********************************************/

Nlppp::Nlppp(						// Default constructor.
	Irule *rule,
	Tree<Pn> *collect,
	Parse *parse,
	Node<Pn> *select,
	Node<Pn> *first,
	Node<Pn> *last,
	Node<Pn> *node,
	Node<Pn> *start,
	Node<Pn> *end,
	Sem *sem,
	Dlist<Ipair> *dsem			// 11/17/99 AM.
	)
{
rule_		= rule;				// FIX.		// 11/20/99 AM.
collect_ = collect;
parse_	= parse;
select_	= select;
first_	= first;
last_		= last;
node_		= node;
start_	= start;
end_		= end;
sem_		= sem;		// Wasn't done.	// 11/17/99 AM.
dsem_		= dsem;								// 11/17/99 AM.
lookahead_ = 0;								// 11/22/99 AM.
noop_		= false;								// 12/15/99 AM.

used_		= false;								// 01/13/00 AM.
red_		= 0;									// 01/13/00 AM.
region_	= REGNULL;							// 01/13/00 AM.

after_	= 0;									// 05/19/00 AM.
elts_		= 0;									// 05/17/00 AM.
sugg_		= 0;									// 05/19/00 AM.
coll_		= 0;									// 05/17/00 AM.
ord_		= 0;									// 05/18/00 AM.
wildord_	= 0;									// 05/18/00 AM.
wildmin_ = -1;		// Undef value.		// 05/18/00 AM.
wildmax_ = -1;		// Undef value.		// 05/18/00 AM.
wildmark_ = 0;									// 05/18/00 AM.
wildstart_ = 0;								// 06/01/00 AM.
wildtot_	= -1;		// Undef value.		// 05/18/00 AM.
len_		= 0;									// 05/18/00 AM.
prefn_	= 0;									// 05/24/00 AM.
succeed_	= false;
fail_		= false;
nlookahead_ = 0;
exitpass_ = false;	// FIX.				// 04/28/01 AM.
return_ = false;								// 03/07/02 AM.
insidefn_ = false;							// 03/12/02 AM.
depth_ = 0;										// 03/12/02 AM.
excised_ = false;
locals_ = 0;									// 03/09/02 AM.
localstrs_ = 0;								// 01/08/07 AM.
span_ = 0;										// 02/04/05 AM.
maxspan_ = 0;									// 02/04/05 AM.
rightmost_ = 0;								// 07/17/06 AM.
rmost_ = 0;										// 07/23/06 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Nlppp::~Nlppp()
{
// if (locals_) do nothing. Just a reference.						// 03/09/02 AM.

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Nlppp::Nlppp(Nlppp &orig)
{
zero();
Nlppp::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Nlppp &Nlppp::operator=(const Nlppp &fm)
{
Nlppp *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Nlppp object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Nlppp::clear()
{
}

void Nlppp::zero()
{
rule_		= 0;				// FIX.		// 11/20/99 AM.
collect_ = 0;
parse_	= 0;
select_	= 0;
first_	= 0;
last_		= 0;
node_		= 0;
start_	= 0;
end_		= 0;
sem_		= 0;
dsem_		= 0;
lookahead_ = 0;								// 11/22/99 AM.
noop_		= false;								// 12/15/99 AM.
used_		= false;								// 01/13/00 AM.
red_		= 0;									// 01/13/00 AM.
region_	= REGNULL;							// 01/13/00 AM.
locals_ = 0;									// 03/09/02 AM.
localstrs_ = 0;								// 01/08/07 AM.

after_	= 0;									// 05/19/00 AM.
elts_		= 0;									// 05/17/00 AM.
sugg_		= 0;									// 05/19/00 AM.
coll_		= 0;									// 05/17/00 AM.
ord_		= 0;									// 05/18/00 AM.
wildord_	= 0;									// 05/18/00 AM.
wildmin_ = -1;		// Undef value.		// 05/18/00 AM.
wildmax_ = -1;		// Undef value.		// 05/18/00 AM.
wildmark_ = 0;									// 05/18/00 AM.
wildstart_ = 0;								// 06/01/00 AM.
wildtot_	= 0;		// Undef value.		// 05/18/00 AM.
len_		= 0;									// 05/18/00 AM.
prefn_	= 0;									// 05/24/00 AM.
succeed_	= false;
fail_		= false;
nlookahead_ = 0;
exitpass_ = false;
return_ = false;
insidefn_ = false;
depth_ = 0;
excised_ = false;
span_ = 0;
maxspan_ = 0;
rightmost_ = 0;
rmost_ = 0;
}

void Nlppp::copy(const Nlppp *orig)
{
Nlppp *dest;
dest = this;
dest->rule_		= orig->rule_;			// FIX.			// 11/20/99 AM.
dest->collect_ = orig->collect_;
dest->parse_	= orig->parse_;
dest->select_	= orig->select_;
dest->first_	= orig->first_;
dest->last_		= orig->last_;
dest->node_		= orig->node_;
dest->start_	= orig->start_;
dest->end_		= orig->end_;
dest->sem_		= orig->sem_;
dest->dsem_		= orig->dsem_;
dest->lookahead_ = orig->lookahead_;					// 11/22/99 AM.
dest->noop_		= orig->noop_;								// 12/15/99 AM.
dest->used_		= orig->used_;								// 01/13/00 AM.
dest->red_		= orig->red_;								// 01/13/00 AM.
dest->region_	= orig->region_;							// 01/13/00 AM.
dest->locals_ = orig->locals_;							// 03/09/02 AM.
dest->localstrs_ = orig->localstrs_;					// 01/08/07 AM.

dest->after_	= orig->after_;							// 05/19/00 AM.
dest->elts_		= orig->elts_;								// 05/17/00 AM.
dest->sugg_		= orig->sugg_;								// 05/19/00 AM.
dest->coll_		= orig->coll_;								// 05/17/00 AM.
dest->ord_		= orig->ord_;								// 05/18/00 AM.
dest->wildord_	= orig->wildord_;							// 05/18/00 AM.
dest->wildmin_	= orig->wildmin_;							// 05/18/00 AM.
dest->wildmax_	= orig->wildmax_;							// 05/18/00 AM.
dest->wildmark_	= orig->wildmark_;					// 05/18/00 AM.
dest->wildstart_ = orig->wildstart_;					// 06/01/00 AM.
dest->wildtot_	= orig->wildtot_;							// 05/18/00 AM.
dest->len_		= orig->len_;								// 05/18/00 AM.
dest->prefn_	= orig->prefn_;							// 05/24/00 AM.
dest->succeed_	= orig->succeed_;
dest->fail_		= orig->fail_;
dest->nlookahead_ = orig->nlookahead_;
dest->exitpass_ = orig->exitpass_;
dest->return_ = orig->return_;
dest->insidefn_ = orig->insidefn_;
dest->depth_ = orig->depth_;
dest->excised_ = orig->excised_;
dest->span_ = orig->span_;
dest->maxspan_ = orig->maxspan_;
dest->rightmost_ = orig->rightmost_;
dest->rmost_ = orig->rmost_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Nlppp &nlppp)
{
output << _T("[NLPPP object]") << std::endl;

return output;
}

/*******************************************/
/*******************************************/

/*******************************************/

/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

Irule		*Nlppp::getRule()	{return rule_;}
Tree<Pn> *Nlppp::getCollect()	{return collect_;}
Parse		*Nlppp::getParse()	{return parse_;  }
Node<Pn>	*Nlppp::getSelect()	{return select_; }
Node<Pn>	*Nlppp::getFirst()	{return first_;  }
Node<Pn>	*Nlppp::getLast()		{return last_;		}
Node<Pn>	*Nlppp::getStart()	{return start_;	}
Node<Pn> *Nlppp::getEnd()		{return end_;		}
Sem		*Nlppp::getSem()		{return sem_;		}
Dlist<Ipair>	*Nlppp::getDsem()	{return dsem_;	}
Node<Pn>	*Nlppp::getLookahead()	{return lookahead_;}
bool		 Nlppp::getNoop()			{return noop_;}					// 12/15/99 AM.
bool		 Nlppp::getUsed()			{return used_;}					// 01/13/00 AM.
Node<Pn>	*Nlppp::getRed()			{return red_;}						// 01/13/00 AM.
enum NlpppRegion Nlppp::getRegion() {return region_;}				// 01/13/00 AM.
Dlist<Ipair>	*Nlppp::getLocals()	{return locals_;	}			// 03/09/02 AM.
Slist<_TCHAR> *Nlppp::getLocalstrs() {return localstrs_;}		// 01/08/07 AM.


Node<Pn>		*Nlppp::getAfter()	{return after_;}					// 05/19/00 AM.
const ELT	*Nlppp::getElts()			{return elts_;}				// 05/17/00 AM.
const SUGG	*Nlppp::getSugg()		{return sugg_;}					// 05/19/00 AM.
COLL		*Nlppp::getColl()			{return coll_;}					// 05/17/00 AM.
int		 Nlppp::getOrd()			{return ord_;}						// 05/18/00 AM.
int		 Nlppp::getWildord()		{return wildord_;}				// 05/18/00 AM.
int		 Nlppp::getWildmin()		{return wildmin_;}				// 05/18/00 AM.
int		 Nlppp::getWildmax()		{return wildmax_;}				// 05/18/00 AM.
int		 Nlppp::getWildtot()		{return wildtot_;}				// 05/18/00 AM.
Node<Pn> *Nlppp::getWildmark()	{return wildmark_;}				// 05/18/00 AM.
Node<Pn> *Nlppp::getWildstart()	{return wildstart_;}				// 06/01/00 AM.
int		 Nlppp::getLen()			{return len_;}						// 05/18/00 AM.
PREFN		 Nlppp::getPres()			{return prefn_;}					// 05/24/00 AM.
bool		 Nlppp::getSucceed()		{return succeed_;}
bool		 Nlppp::getFail()			{return fail_;}
int		 Nlppp::getNlookahead()	{return nlookahead_;}
bool		 Nlppp::getExitpass()	{return exitpass_;}
bool		 Nlppp::getReturn()		{return return_;}
bool		 Nlppp::getInsidefn()	{return insidefn_;}
long		 Nlppp::getDepth()		{return depth_;}
bool		 Nlppp::getExcised()		{return excised_;}
long		 Nlppp::getSpan()			{return span_;		}
long		 Nlppp::getMaxspan()		{return maxspan_;	}
Node<Pn>	*Nlppp::getRightmost()	{return rightmost_;}
Node<Pn>	*Nlppp::getRmost()		{return rmost_;}


/********************************************
* FN:		Modify Functions
********************************************/

void	Nlppp::setRule(Irule *x)	{rule_	= x;}
void	Nlppp::setCollect(Tree<Pn> *x)	{collect_	= x;}
void	Nlppp::setParse(Parse		*x)	{parse_		= x;}
void	Nlppp::setSelect(Node<Pn>	*x)	{select_		= x;}
void	Nlppp::setFirst(Node<Pn>	*x)	{first_		= x;}
void	Nlppp::setLast(Node<Pn>		*x)	{last_		= x;}
void	Nlppp::setStart(Node<Pn>	*x)	{start_		= x;}
void	Nlppp::setEnd(Node<Pn>		*x)	{end_			= x;}
void	Nlppp::setSem(Sem				*x)	{sem_			= x;}
void	Nlppp::setDsem(Dlist<Ipair>	*x)	{dsem_	= x;}
void	Nlppp::setLookahead(Node<Pn>	*x)	{lookahead_ = x;}
void	Nlppp::setNoop(bool x)					{noop_	= x;}			// 12/15/99 AM.
void	Nlppp::setUsed(bool x)					{used_	= x;}			// 01/13/00 AM.
void	Nlppp::setRed(Node<Pn>		*x)		{red_		= x;}			// 01/13/00 AM.
void	Nlppp::setRegion(enum NlpppRegion x) {region_ = x;}		// 01/13/00 AM.
void	Nlppp::setLocals(Dlist<Ipair>	*x)	{locals_	= x;}			// 03/09/02 AM.
void	Nlppp::setLocalstrs(Slist<_TCHAR> *x) {localstrs_ = x;}  // 01/08/07 AM.

void	Nlppp::setAfter(Node<Pn> *x)			{after_		= x;}		// 05/19/00 AM.
void	Nlppp::setElts(const ELT *x)			{elts_		= x;}		// 05/17/00 AM.
void	Nlppp::setSugg(const SUGG *x)			{sugg_		= x;}		// 05/19/00 AM.
void	Nlppp::setColl(COLL *x)					{coll_		= x;}		// 05/17/00 AM.
void	Nlppp::setOrd(int x)						{ord_			= x;}		// 05/18/00 AM.
void	Nlppp::setWildord(int x)				{wildord_	= x;}		// 05/18/00 AM.
void	Nlppp::setWildmin(int x)				{wildmin_	= x;}		// 05/18/00 AM.
void	Nlppp::setWildmax(int x)				{wildmax_	= x;}		// 05/18/00 AM.
void	Nlppp::setWildtot(int x)				{wildtot_	= x;}		// 05/18/00 AM.
void	Nlppp::setWildmark(Node<Pn> *x)		{wildmark_	= x;}		// 05/18/00 AM.
void	Nlppp::setWildstart(Node<Pn> *x)		{wildstart_ = x;}		// 06/01/00 AM.
void	Nlppp::setLen(int x)						{len_			= x;}		// 05/18/00 AM.
void	Nlppp::setPres(PREFN x)					{prefn_		= x;}		// 05/24/00 AM.
void	Nlppp::setSucceed(bool x)				{succeed_	= x;}
void	Nlppp::setFail(bool x)					{fail_		= x;}
void	Nlppp::setNlookahead(int x)			{nlookahead_ = x;}
void	Nlppp::setExitpass(bool x)				{exitpass_	= x;}
void	Nlppp::setReturn(bool x)				{return_ = x;}
void	Nlppp::setInsidefn(bool x)				{insidefn_ = x;}
void	Nlppp::setDepth(long x)					{depth_ = x;}
void	Nlppp::setExcised(bool x)				{excised_	= x;}
void	Nlppp::setSpan(long x)					{span_ = x;}
void	Nlppp::setMaxspan(long x)				{maxspan_ = x;}
void	Nlppp::setRightmost(Node<Pn> *x)		{rightmost_ = x;}
void	Nlppp::setRmost(Node<Pn> *x)		{rmost_ = x;}

/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Nlppp::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Nlppp::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Nlppp count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Nlppp count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		HTAB
* CR:		12/29/99 AM.
* SUBJ:	Fetch a hash table, to use for interning strings, etc.
********************************************/

Htab *Nlppp::htab()
{
//return parse_->getAna()->getHtab();	// 06/25/03 AM.
return parse_->htab_;						// 06/25/03 AM.
}


/********************************************
* FN:		RULELINE
* CR:		12/21/01 AM.
* SUBJ:	Get current rule's line number.
********************************************/

long Nlppp::ruleLine()
{
if (rule_)
	return rule_->getLine();
return 0;
}

/********************************************
* FN:		FREELOCALS
* CR:		01/08/07 AM.
* SUBJ:	Free up local stuff.
********************************************/

void Nlppp::freeLocals(
	Slist<_TCHAR> *restorelocalstrs
	)
{

// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
if (localstrs_)
	{
	// Traverse and delete temporary strings here.
	// Opt: Shouldn't need to traverse twice...
	_TCHAR *tmp;
	Selt<_TCHAR> *ptr = localstrs_->getFirst();
	for (; ptr; ptr = ptr->Right())
		{
		tmp = ptr->getData();
		Chars::destroy(tmp);
		}
	Slist<_TCHAR>::DeleteSlist(localstrs_);
	}
localstrs_ = restorelocalstrs;	// RESTORE CALLER LOCALS.
}

/*************************      END OF FILE      ******************************/
