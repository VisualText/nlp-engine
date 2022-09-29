/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IEXPRSTMT.CPP
* FILE:	lit/iexprstmt.cpp
* CR:		11/09/99 AM.
* SUBJ:	Interpreted/internal expr-stmt semantic object. NLP++ semantics.
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
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"				// 05/08/00 AM.
#include "pn.h"				// 06/27/01 AM. (g++ complained.)
#include "iop.h"				// 12/27/99 AM.
#include "iexpr.h"			// 12/27/99 AM.
#include "rfasem.h"			// 11/12/99 AM.
#include "iexprstmt.h"

int Iexprstmt::count_ = 0;

/********************************************
* FN:		Special Functions for Iexprstmt Class
********************************************/

Iexprstmt::Iexprstmt(						// Default constructor.
	RFASem *expr,
	long line																	// 03/12/02 AM.
	)
	: Istmt(_T("exprstmt"),line)												// 03/12/02 AM.
{
expr_ = expr;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Iexprstmt::~Iexprstmt()
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
Iexprstmt::Iexprstmt(Iexprstmt &orig)
{
zero();
Iexprstmt::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Iexprstmt &Iexprstmt::operator=(const Iexprstmt &fm)
{
Iexprstmt *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iexprstmt object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iexprstmt::clear()
{
if (expr_)
	delete expr_;
}

void Iexprstmt::zero()
{
expr_ = 0;
}

void Iexprstmt::copy(const Iexprstmt *orig)
{
Iexprstmt *dest;
dest = this;

dest->expr_ = orig->expr_;	// Assignment operator.
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iexprstmt &exprstmt)
{
// For more elegant output, checking on the top-level types of the
// expr statement.														// 12/27/99 AM.
RFASem *rfasem = exprstmt.expr_;										// 12/27/99 AM.
if (rfasem->getType() != RSEXPR)										// 12/27/99 AM.
	{
	output << *rfasem << _T(";") ;
	return output;
	}
Iexpr *expr = rfasem->getExpr();										// 12/27/99 AM.
switch (expr->getType())												// 12/27/99 AM.
	{
	case BINARYEXPR:
	case ASSIGNEXPR:
		if (expr->getLhs()->getType() == RSEXPR)
			output << _T("(") << *expr->getLhs() << _T(") ");
		else
			output << *expr->getLhs() << _T(" ");
		output << *expr->getOp() << _T(" ");
		if (expr->getRhs()->getType() == RSEXPR)
			output << _T("(") << *expr->getRhs() << _T(")");
		else
			output << *expr->getRhs();
		break;
	case UNARYEXPR:
		output << *expr->getOp();
		if (expr->getRhs()->getType() == RSEXPR)
			output << _T("(") << *expr->getRhs() << _T(")");
		else
			output << *expr->getRhs();
		break;
	case POSTUNARYEXPR:
		if (expr->getRhs()->getType() == RSEXPR)
			output << _T("(") << *expr->getRhs() << _T(")");
		else
			output << *expr->getRhs();
		output << *expr->getPostop();
		break;
	default:
		output << _T("<BAD EXPR>");
		break;
	}

output << _T(";") ;

return output;
}

// Workaround to calling overloaded << in derived classes.	// 12/27/99 AM.
void Iexprstmt::print(std::_t_ostream &output)
{
output << *this;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

RFASem *Iexprstmt::getExpr()	{return expr_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Iexprstmt::setExpr(RFASem *x)	{expr_ = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iexprstmt::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iexprstmt::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iexprstmt count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iexprstmt count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENEXPRSTMT
* CR:		11/11/99 AM.
* SUBJ:	Generate exprstmt to a rules file.
*********************************************/
void Iexprstmt::genExprstmt(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		11/11/99 AM.
* SUBJ:	See if two exprstmts are identical in form.
*********************************************/

bool Iexprstmt::same(Istmt *stmt1, Istmt *stmt2)
{
Iexprstmt *exprstmt1 = (Iexprstmt *) stmt1;
Iexprstmt *exprstmt2 = (Iexprstmt *) stmt2;
if (!exprstmt1 && !exprstmt2)
	return true;
if (!exprstmt1 || !exprstmt2)
	return false;

return RFASem::same(exprstmt1->expr_, exprstmt2->expr_);
}


/********************************************
* FN:		EVAL
* CR:		11/14/99 AM.
* SUBJ:	Evaluate expr-statement.
*********************************************/

bool Iexprstmt::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;
if (!expr_)
	return true;
bool ok = expr_->eval(nlppp, /*UP*/ val);
if (val)
	{
	// 06/04/00 AM. CHECK ACTIONS DO USE THIS VALUE, SO DON'T DELETE.
	// Not using value at statement level.
	//delete val;																// 06/04/00 AM.
	//val = 0;																	// 06/04/00 AM.
	}
return ok;
}



/********************************************
* FN:		GENEVAL
* CR:		05/06/00 AM.
* SUBJ:	Gen code to eval expr-statement.
* NOTE:	05/27/00 AM. stmt() action will delete sem object, if any.
* OPT:	At compile time, could determine when to generate the stmt()
*			function.  Only needed when stmt evaluates to an RFASem type.
*********************************************/

bool Iexprstmt::genEval(Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
_TCHAR *indent = gen->indent_;

if (!expr_)
	return true;
// Let the internals decide whether to generate an Arun::stmt(); wrapper.
// 06/06/00 AM.
//*fcode << indent
//		 << "Arun::stmt(";													// 05/27/00 AM.
if (!expr_->genEval(gen, true))
	return false;
//*fcode << ")";																	// 05/27/00 AM.
*fcode << _T(";");
Gen::eol(fcode);																// 04/04/03 AM.
return true;
}

/*************************      END OF FILE      ******************************/
