/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ISTMT.CPP
* FILE:	lit/istmt.cpp
* CR:		11/09/99 AM.
* SUBJ:	Interpreted/internal statement semantic object. NLP++ semantics.
* NOTE:	This base class will also manage lists of statements.
*
*******************************************************************************/

#include "StdAfx.h"
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
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "lite/nlppp.h"
#include "rfasem.h"
#include "gen.h"																// 05/27/00 AM.
#include "parse.h"															// 08/24/02 AM.
#include "istmt.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Slist<Istmt>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Selt<Istmt>::count_  = 0;
#else
int Slist<Istmt>::count_ = 0;
int Selt<Istmt>::count_  = 0;
#endif
int Istmt::count_ = 0;

/********************************************
* FN:		Special Functions for Istmt Class
********************************************/

Istmt::Istmt(						// Default constructor.
	_TCHAR *typ,
	long line		// Line number in pass file.						// 03/12/02 AM.
	)
{
type_[0] = '\0';
if (typ && *typ)
	_tcscpy(type_, typ);
line_ = line;																	// 03/12/02 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Istmt::~Istmt()
{

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Istmt::Istmt(Istmt &orig)
{
zero();
Istmt::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Istmt &Istmt::operator=(const Istmt &fm)
{
Istmt *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Istmt object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Istmt::clear()
{
}

void Istmt::zero()
{
}

void Istmt::copy(const Istmt *orig)
{
Istmt *dest;
dest = this;

}

/*******************************************/

/*******************************************/

// 12/27/99 AM. The derived types should print themselves.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Istmt &stmt)
{
// Workaround to print derived classes.				// 12/27/99 AM.
stmt.print(output);											// 12/27/99 AM.
return output;
}

void Istmt::print(std::_t_ostream &output)
{
output << _T("[Base Istmt class]") << std::endl;
}

/*******************************************/
/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Selt<Istmt> &selt)
{
Istmt *dat;
dat = selt.getData();
output << *dat;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Slist<Istmt> &list)
{
Selt<Istmt> *selt;
Istmt *dat;
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

/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

_TCHAR *Istmt::getType()	{return type_;}
long	Istmt::getLine()	{return line_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Istmt::setType(_TCHAR *x)	{if (x && *x) _tcscpy(type_, x);}
void	Istmt::setLine(long x)	{line_ = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Istmt::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Istmt::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Istmt count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Istmt count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENSTMT
* CR:		11/11/99 AM.
* SUBJ:	Generate stmt to a rules file.
*********************************************/
void Istmt::genStmt(
	std::_t_ostream &ofile
	)
{
//ofile << this << std::flush;
}

/********************************************
* FN:		COMPARE
* CR:		11/11/99 AM.
* SUBJ:	See if two stmt lists are identical in form.
* NOTE:	Sometimes a list has no elements.
*********************************************/

bool Istmt::compare(Slist<Istmt> *stmts1, Slist<Istmt> *stmts2)
{
if (!stmts1 && !stmts2)
	return true;
Selt<Istmt> *dstmt1 = 0, *dstmt2 = 0;
if (stmts1)
	dstmt1 = stmts1->getFirst();
if (stmts2)
	dstmt2 = stmts2->getFirst();
if (!dstmt1 && !dstmt2)
	return true;
if (!dstmt1 || !dstmt2)
	return false;
for (; dstmt1 && dstmt2;
		dstmt1 = dstmt1->Right(), dstmt2 = dstmt2->Right())
	{
	if (!compare(dstmt1->getData(), dstmt2->getData()))
		return false;
	}
if (dstmt1 || dstmt2)
	return false;
return true;
}


/********************************************
* FN:		COMPARE
* CR:		11/11/99 AM.
* SUBJ:	See if two stmts are identical in form.
*********************************************/

bool Istmt::compare(Istmt *stmt1, Istmt *stmt2)
{
if (!stmt1 && !stmt2)
	return true;
if (!stmt1 || !stmt2)
	return false;

if (_tcscmp(stmt1->type_, stmt2->type_))
	return false;		// Not the same statement subtype.

// Compare using derived class comparator.
return stmt1->same(stmt1, stmt2);
}


/********************************************
* FN:		EVAL
* CR:		11/14/99 AM.
* SUBJ:	Evaluate a list of statements.
*********************************************/

bool Istmt::eval(Slist<Istmt> *stmts, Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;
if (!stmts)
	return true;

bool ok = true;
Istmt *stmt;
Selt<Istmt> *selt;
RFASem *sval;

// Save current programmatic context.									// 03/08/02 AM.
enum NlpppRegion savereg = nlppp->region_;							// 03/08/02 AM.

for (selt = stmts->getFirst(); selt; selt = selt->Right())
	{
	stmt = selt->getData();
	nlppp->parse_->line_ = stmt->line_;									// 03/13/03 AM.
	if (!stmt->eval(nlppp, /*UP*/ sval))	// USE DERIVED CLASS.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Error in stmts.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		ok = false;
		}
	nlppp->region_ = savereg;					// Restore.				// 03/08/02 AM.
	if (nlppp->return_)														// 03/07/02 AM.
		{
		if (!nlppp->insidefn_)												// 03/12/02 AM.
			{
//			nlppp->parse_->line_ = stmt->getLine();					// 08/24/02 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Return statement not in a function.]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);
			ok = false;
			}
		val = sval;		// THE RETURN VALUE.								// 03/08/02 AM.
		return ok;		// RETURN IMMEDIATELY.							// 03/08/02 AM.
		}
	if (sval)
		{
		if (sval->getType() == RSOSTREAM)					// FIX.	// 10/26/00 AM.
			sval->setOstream(0);	// So won't be del'ed.	// FIX.	// 10/26/00 AM.
		delete sval;
		}
	if (!ok)																		// 06/10/00 AM.
		return false;															// 06/10/00 AM.
	if (nlppp->exitpass_)													// 11/19/02 AM.
		return true;															// 11/19/02 AM.
	if (nlppp->succeed_)														// 06/10/00 AM.
		{
		if (nlppp->region_ == REGCHECK									// 06/10/00 AM.
			|| nlppp->region_ == REGCODE									// 10/23/00 AM.
//			|| nlppp->region_ == REGDECL									// 03/12/02 AM.
			)
			return true;														// 06/10/00 AM.
		}
	else if (nlppp->fail_ && nlppp->region_ != REGCODE)			// 06/10/00 AM.
//		&& nlppp->region_ != REGDECL)										// 03/12/02 AM.
		return true;															// 06/10/00 AM.
	}
return ok;
}


/********************************************
* FN:		GENEVAL
* CR:		05/06/00 AM.
* SUBJ:	Gen code to eval a list of statements.
* OPT:	If genEval recursion can determine when an RFASem type is
*			the result of the statement, then we could know when to generate
*			the "stmt" function to delete the RFASem.
*********************************************/

bool Istmt::genEval(Slist<Istmt> *stmts, Gen *gen)
{
if (!stmts)
	return true;

bool ok = true;
Istmt *stmt;
Selt<Istmt> *selt;
for (selt = stmts->getFirst(); selt; selt = selt->Right())
	{
	stmt = selt->getData();
	if (!stmt->genEval(gen))	// USE DERIVED CLASS.
		ok = false;
	}
return ok;
}

/*************************      END OF FILE      ******************************/
