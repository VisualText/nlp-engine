/*******************************************************************************
Copyright (c) 2002-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IRETURN.CPP
* FILE:	lite/ireturn.cpp
* CR:		03/07/02 AM.
* SUBJ:	Interpreted/internal return statement semantic object. NLP++ semantics.
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
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"
#include "pn.h"				// (g++ complained.)
#include "rfasem.h"
#include "ivar.h"
#include "lite/nlppp.h"
#include "parse.h"	// For interning string.	// 01/16/07 AM.
#include "ireturn.h"

#ifdef LINUX
//template<>	// Linux.	// 04/26/07 AM.
int Ireturn::count_ = 0;
#else
int Ireturn::count_ = 0;
#endif

/********************************************
* FN:		Special Functions for Ireturn Class
********************************************/

Ireturn::Ireturn(						// Default constructor.
	RFASem *expr,
	long line			// Line number in pass file.					// 03/12/02 AM.
	)
	: Istmt(_T("returnstmt"),line)												// 03/12 02 AM.
{
expr_ = expr;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ireturn::~Ireturn()
{
// Owns the expr.
clear();

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Ireturn::Ireturn(Ireturn &orig)
{
zero();
Ireturn::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Ireturn &Ireturn::operator=(const Ireturn &fm)
{
Ireturn *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ireturn object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Ireturn::clear()
{
if (expr_)
	delete expr_;
}

void Ireturn::zero()
{
expr_		= 0;
}

void Ireturn::copy(const Ireturn *orig)
{
Ireturn *dest;
dest = this;

dest->expr_ = orig->expr_;	// Assignment operator.
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ireturn &returnstmt)
{
output << _T("return ");
if (returnstmt.expr_)
	output << *returnstmt.expr_;
output << _T(";\n");
return output;
}

// Workaround to calling overloaded << in derived classes.
void Ireturn::print(std::_t_ostream &output)
{
output << *this;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

RFASem *Ireturn::getExpr()		{return expr_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Ireturn::setExpr(RFASem *x)		{expr_	 = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ireturn::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ireturn::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ireturn count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ireturn count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENRETURNSTMT
* CR:		03/07/02 AM.
* SUBJ:	Generate returnstmt to a rules file.
*********************************************/
void Ireturn::genReturnstmt(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		03/07/02 AM.
* SUBJ:	See if two returnstmts are identical in form.
*********************************************/

bool Ireturn::same(Istmt *stmt1, Istmt *stmt2)
{
Ireturn *returnstmt1 = (Ireturn *) stmt1;
Ireturn *returnstmt2 = (Ireturn *) stmt2;
if (!returnstmt1 && !returnstmt2)
	return true;
if (!returnstmt1 || !returnstmt2)
	return false;

if (!RFASem::same(returnstmt1->expr_, returnstmt2->expr_))
	return false;
return true;
}


/********************************************
* FN:		EVAL
* CR:		03/07/02 AM.
* SUBJ:	Evaluate return-statement.
*********************************************/

bool Ireturn::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;
if (!expr_)
	{
	val = new RFASem(0LL);	// Return null value.
	return true;
	}

// Need to check if within a function.
// (Really should be an NLP++ parse time check).

bool ok = expr_->eval(nlppp, /*UP*/ val);

nlppp->return_ = true;			// Flag returning from function.

// CONVERT RSNUM == "0" to 0 numeric.									// 07/29/02 AM.
if (val)
	{
	_TCHAR *str;
	switch (val->getType())													// 07/29/02 AM.
		{
		case RSNUM:																// 07/29/02 AM.
			str = val->getName();											// 07/29/02 AM.
			if (*str == '0' && !*(str+1))									// 07/29/02 AM.
				{
				// CONVERT.
				val->setType(RSLONG);										// 07/29/02 AM.
				val->setLong(0);												// 07/29/02 AM.
				}
			break;
		// LOCAL STRING, TEMPORARY STRING MANAGEMENT.				// 01/08/07 AM.
		case RSNAME:															// 01/16/07 AM.
		case RSSTR:																// 01/16/07 AM.
			{
			// For now, intern any string(s) being returned.		// 01/16/07 AM.
			str = val->getName();											// 01/16/07 AM.
			_TCHAR *x;															// 01/16/07 AM.
			if (str && *str)													// 01/16/07 AM.
				{
				nlppp->parse_->internStr(val->getName(),/*UP*/x);	// 01/16/07 AM.
				val->setName(x);	// Install the interned string.	// 01/16/07 AM.
				}
			}
			break;
		case RSARGS:												// FIX	// 07/23/07 AM.
			// Traverse array, intern strings.				// FIX	// 07/23/07 AM.
			Iarg::intern_strings(val->getArgs(),nlppp->parse_);	// FIX	// 07/23/07 AM.
			break;
		}
	}															

return ok;
}


/********************************************
* FN:		GENEVAL
* CR:		03/07/02 AM.
* SUBJ:	Gen code to evaluate return-statement.
* NOTE:	Because type of return varies dynamically, we'll have all user-defined functions
*			return an RFASem data type, which encompasses everything.
*********************************************/

bool Ireturn::genEval(Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
_TCHAR *indent = gen->indent_;

if (gen->region_ != REGDECL)												// 03/12/02 AM.
	{
	std::_t_strstream gerrStr;						// 03/12/02 AM.
	gerrStr << _T("[Gen: Return not in function.]") << std::ends;			// 03/12/02 AM.
	errOut(&gerrStr,false);																// 03/12/02 AM.
	*fcode << std::endl																// 03/12/02 AM.
			 << _T("// ERROR: Return not in user-defined NLP++ fn.")	// 03/12/02 AM.
			 << std::endl;															// 03/12/02 AM.
	*fcode << _T("bad_code_gen_on_return;") << std::endl;	// Break code.	// 03/12 02 AM.
	return false;																// 03/12/02 AM.
	}

if (!expr_)
	{
	*fcode << indent << _T("return 0;");
	Gen::nl(fcode);															// 04/04/03 AM.
	return true;
	}
RFASem *tmp = 0;
enum RFASemtype typ = RSNULL;
*fcode << indent << _T("return Arun::ret(nlppp,loc,locstrs,");					// 03/11/02 AM.
if (!expr_->genEval(gen))
	{
	*fcode << _T("// ERROR in returnstmt.") << std::endl;
	return false;
	}
*fcode << _T(");");
Gen::eol(fcode);																// 04/04/03 AM.
return true;
}

/*************************      END OF FILE      ******************************/
