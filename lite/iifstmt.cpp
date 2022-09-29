/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IIFSTMT.CPP
* FILE:	lit/iifstmt.cpp
* CR:		11/12/99 AM.
* SUBJ:	Interpreted/internal if-stmt semantic object. NLP++ semantics.
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
#include "gen.h"					// 05/08/00 AM.
#include "pn.h"					// 06/27/01 AM. (g++ complained.)
#include "rfasem.h"
#include "ivar.h"
#include "lite/parse.h"			// 03/13/03 AM.
#include "iifstmt.h"

#ifdef LINUX
//template<>	// Linux.	// 04/26/07 AM.
int Iifstmt::count_ = 0; // 03/03/16 AM.
#else
int Iifstmt::count_ = 0;
#endif

/********************************************
* FN:		Special Functions for Iifstmt Class
********************************************/

Iifstmt::Iifstmt(						// Default constructor.
	RFASem *cond,
	RFASem *ifpart,
	RFASem *elsepart,
	long line																	// 03/12/02 AM.
	)
	: Istmt(_T("ifstmt"),line)													// 03/12/02 AM.
{
cond_ = cond;
if_   = ifpart;
else_ = elsepart;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Iifstmt::~Iifstmt()
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
Iifstmt::Iifstmt(Iifstmt &orig)
{
zero();
Iifstmt::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Iifstmt &Iifstmt::operator=(const Iifstmt &fm)
{
Iifstmt *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iifstmt object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iifstmt::clear()
{
if (cond_)
	delete cond_;
if (if_)
	delete if_;
if (else_)
	delete else_;
}

void Iifstmt::zero()
{
cond_ = 0;
if_	= 0;
else_ = 0;
}

void Iifstmt::copy(const Iifstmt *orig)
{
Iifstmt *dest;
dest = this;

dest->cond_ = orig->cond_;	// Assignment operator.
dest->if_	= orig->if_;
dest->else_ = orig->else_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iifstmt &ifstmt)
{
output << _T("if (");
if (ifstmt.cond_)
	output << *ifstmt.cond_;
output << _T(")\n\t{");
if (ifstmt.if_)
	output << *ifstmt.if_;
output << _T("}");
if (ifstmt.else_)
	{
	output << _T("else\n\t{");
	output << *ifstmt.else_;
	output << _T("}");
	}
output << _T("\n");
return output;
}

// Workaround to calling overloaded << in derived classes.	// 12/27/99 AM.
void Iifstmt::print(std::_t_ostream &output)
{
output << *this;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

RFASem *Iifstmt::getCond()	{return cond_;}
RFASem *Iifstmt::getIf()	{return if_;}
RFASem *Iifstmt::getElse()	{return else_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Iifstmt::setCond(RFASem *x)	{cond_ = x;}
void	Iifstmt::setIf(RFASem	*x)	{if_	 = x;}
void	Iifstmt::setElse(RFASem	*x)	{else_ = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iifstmt::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iifstmt::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iifstmt count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iifstmt count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENIFSTMT
* CR:		11/11/99 AM.
* SUBJ:	Generate ifstmt to a rules file.
*********************************************/
void Iifstmt::genIfstmt(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		11/11/99 AM.
* SUBJ:	See if two ifstmts are identical in form.
*********************************************/

bool Iifstmt::same(Istmt *stmt1, Istmt *stmt2)
{
Iifstmt *ifstmt1 = (Iifstmt *) stmt1;
Iifstmt *ifstmt2 = (Iifstmt *) stmt2;
if (!ifstmt1 && !ifstmt2)
	return true;
if (!ifstmt1 || !ifstmt2)
	return false;

if (!RFASem::same(ifstmt1->cond_, ifstmt2->cond_))
	return false;
if (!RFASem::same(ifstmt1->if_, ifstmt2->if_))
	return false;
if (!RFASem::same(ifstmt1->else_, ifstmt2->else_))
	return false;
return true;
}


/********************************************
* FN:		EVAL
* CR:		11/14/99 AM.
* SUBJ:	Evaluate if-else-statement.
*********************************************/

bool Iifstmt::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;
if (!cond_)
	return false;
RFASem *c_val = 0, *i_val = 0, *e_val = 0;
RFASem *tmp = 0;
enum RFASemtype typ = RSNULL;
bool ifflag = true;			// True if evaluating the IF part, else ELSE.

bool ok = cond_->eval(nlppp, /*UP*/ c_val);
if (!ok)																			// 11/21/00 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[ifstmt: Error(1).]") << std::ends;
	nlppp->getParse()->errOut(&gerrStr,false);											// 03/13/03 AM.
							// 10/18/00 AM.
	goto done;
	}
if (!c_val)		// Accepting this as ok.								// 11/21/00 AM.
	{
//	goto done;																	// 11/21/00 AM.
	ifflag = false;												// FIX.	// 02/06/01 AM.
	goto doit;														// FIX.	// 02/06/01 AM.
	}

_TCHAR *str;
typ = c_val->getType();

if (typ == RSVAR)
	{
	// Evaluate variable's value (as opposed to assigning to it).
	ok = c_val->getVar()->eval(nlppp, /*UP*/ tmp);
	if (!ok || !tmp)
		{
		if (!ok)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[ifstmt: Error(2).]") << std::ends;
			nlppp->getParse()->errOut(&gerrStr,false);									// 03/13/03 AM.
			}
					// 10/18/00 AM.
		ok = false;
		goto done;
		}
	typ = tmp->getType();
	c_val = tmp;
	}

Dlist<Iarg> *dlist;															// 05/23/01 AM.
switch (typ)
	{
	case RSNAME:
	case RSNUM:
	case RSSTR:
		ifflag = ((str = c_val->getName()) && *str);
		break;
	case RSLONG:
		ifflag = (c_val->getLong() != 0);
		break;
	case RSFLOAT:									// FIX.	// 06/26/06 AM.
		ifflag = (c_val->getFloat() != 0);	// FIX.	// 06/26/06 AM.
		break;										// FIX.	// 06/26/06 AM.
	case RS_KBCONCEPT:						// 03/15/00 AM.
		ifflag = (c_val->getKBconcept() ? true : false);
		break;
	case RS_KBPHRASE:							// 03/15/00 AM.
		ifflag = (c_val->getKBphrase() ? true : false);

		break;
	case RS_KBATTR:							// 03/15/00 AM.
		ifflag = (c_val->getKBattr() ? true : false);
		break;
	case RS_KBVAL:								// 03/15/00 AM.
		ifflag = (c_val->getKBval() ? true : false);
		break;
	case RSNODE:																// 10/18/00 AM.
		ifflag = (c_val->getNode() ? true : false);					// 10/18/00 AM.
		break;
	case RSARGS:																// 12/07/00 AM.
//		*gerr << "[ifstmt: Multiarg unhandled.]" << std::endl;			// 05/23/01 AM.
//		ok = false;																// 05/23/01 AM.
//		goto done;																// 05/23/01 AM.
		if (!(dlist = c_val->getArgs()))									// 05/23/01 AM.
			ifflag = false;													// 05/23/01 AM.
		else																		// 05/23/01 AM.
			ifflag = (dlist->getFirst() ? true : false);				// 05/23/01 AM.
		break;																	// 05/23/01 AM.
	case RSOSTREAM:												// FIX.	// 05/23/01 AM.
		ifflag = (c_val->getOstream() ? true : false);	// FIX.	// 05/23/01 AM.
		break;																	// 05/23/01 AM.
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[ifstmt: Error(3).]") << std::ends;
		nlppp->getParse()->errOut(&gerrStr,false);										// 03/13/03 AM.
		}
						// 10/18/00 AM.
		ok = false;
		goto done;
	}

doit:																	// FIX.	// 02/06/01 AM.

if (ifflag)				// Evaluate the if-statements.
	{
	if (if_)
		{
		ok = if_->eval(nlppp, val);
		// Looks like RETURN takes care of itself here.				// 03/09/02 AM.
		if (!ok)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[ifstmt: Error(4).]") << std::ends;
			nlppp->getParse()->errOut(&gerrStr,false);									// 03/13/03 AM.
			}
		// 10/18/00 AM.
		goto done;
		}
	ok = true;				// Empty block is ok!			// FIX.	// 08/31/00 AM.
	goto done;
	}
else if (else_)		// Evaluate the else-statements, if any.
	{
	ok = else_->eval(nlppp, val);
	// Looks like RETURN takes care of itself here.					// 03/09/02 AM.
	if (!ok)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[ifstmt: Error(5).]") << std::ends;
		nlppp->getParse()->errOut(&gerrStr,false);										// 03/13/03 AM.
		}
						// 10/18/00 AM.
	}
else
	ok = true;		// No else-part is OK.
	
done:

if (c_val)
	delete c_val;
if (tmp)
	delete tmp;

return ok;
}


/********************************************
* FN:		GENEVAL
* CR:		05/06/00 AM.
* SUBJ:	Gen code to evaluate if-else-statement.
*********************************************/

bool Iifstmt::genEval(Gen *gen)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
_TCHAR *indent = gen->indent_;

if (!cond_)
	return false;
RFASem *tmp = 0;
enum RFASemtype typ = RSNULL;

// Hmmm, need to know the type before generating code.
// Maybe need a prepass through....
// When I parse/intern NLP++ expressions, that's when I should determine
// their types. (eg RSEXPR of type STRING...)
// An alternative is to gen code with a target type, eg BOOL in the present
// case.
// Yet another alt:  generate if (truth(....)) -- then overload that
// function with all possible return types!  Elegant!  Can be an inline.
// This can be a general way....(but not totally optimized.)
// If var, evals it. (see Iifstmt::eval).
*fcode << indent << _T("if (Arun::truth(");
if (!cond_->genEval(gen))
	{
	*fcode << std::endl << _T("// ERROR in if condition.") << std::endl;
	return false;
	}
*fcode << _T("))");
Gen::nl(fcode);																// 04/04/03 AM.


// Gen the if-statements.
// (Could see if only one, then omit the curly braces.)
_TCHAR newindent[64];
_stprintf(newindent, _T("%s\t"), indent);
gen->setIndent(newindent);
*fcode << newindent << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
if (!if_ ||															// FIX.	// 04/28/01 AM.
	 !if_->genEval(gen))
	{
	// Accepting this as an EMPTY if-part.					// FIX.	// 04/28/01 AM.
//	*fcode << "// ERROR in if-part." << std::endl;				// FIX.	// 04/28/01 AM.
//	gen->setIndent(indent);			// Restore.				// FIX.	// 04/28/01 AM.
//	return false;													// FIX.	// 04/28/01 AM.
	}
*fcode << newindent << _T("}");
Gen::nl(fcode);																// 04/04/03 AM.

if (else_)		// Gen the else-statements, if any.
	{
	*fcode << indent << _T("else");
	Gen::nl(fcode);															// 04/04/03 AM.
	*fcode << newindent << _T("{");
	Gen::nl(fcode);															// 04/04/03 AM.
	if (!else_->genEval(gen))
		{
		// Accepting this as an EMPTY else-part.			// FIX.	// 04/28/01 AM.
//		*fcode << "// ERROR in else-part" << std::endl;		// FIX.	// 04/28/01 AM.
//		gen->setIndent(indent);			// Restore.			// FIX.	// 04/28/01 AM.
//		return false;												// FIX.	// 04/28/01 AM.
		}
	*fcode << newindent << _T("}");
	Gen::nl(fcode);															// 04/04/03 AM.
	}
gen->setIndent(indent);			// Restore.
return true;
}

/*************************      END OF FILE      ******************************/
