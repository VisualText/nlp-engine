/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IWHILESTMT.CPP
* FILE:	lite/iwhilestmt.cpp
* CR:		08/31/00 AM.
* SUBJ:	Interpreted/internal whilestmt semantic object. NLP++ semantics.
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
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"
#include "iarg.h"
#include "pn.h"				// 06/27/01 AM. (g++ complained.)
#include "rfasem.h"
#include "ivar.h"
#include "lite/parse.h"														// 08/24/02 AM.
#include "iwhilestmt.h"

int Iwhilestmt::count_ = 0;

/********************************************
* FN:		Special Functions for Iwhilestmt Class
********************************************/

Iwhilestmt::Iwhilestmt(						// Default constructor.
	RFASem *cond,
	RFASem *block,
	long line																	// 03/12/02 AM.
	)
	: Istmt(_T("whilestmt"),line)												// 03/12 02 AM.
{
cond_ = cond;
block_   = block;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Iwhilestmt::~Iwhilestmt()
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
Iwhilestmt::Iwhilestmt(Iwhilestmt &orig)
{
zero();
Iwhilestmt::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Iwhilestmt &Iwhilestmt::operator=(const Iwhilestmt &fm)
{
Iwhilestmt *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iwhilestmt object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iwhilestmt::clear()
{
if (cond_)
	delete cond_;
if (block_)
	delete block_;
}

void Iwhilestmt::zero()
{
cond_		= 0;
block_	= 0;
}

void Iwhilestmt::copy(const Iwhilestmt *orig)
{
Iwhilestmt *dest;
dest = this;

dest->cond_ = orig->cond_;	// Assignment operator.
dest->block_	= orig->block_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iwhilestmt &whilestmt)
{
output << _T("while (");
if (whilestmt.cond_)
	output << *whilestmt.cond_;
output << _T(")\n\t{");
if (whilestmt.block_)
	output << *whilestmt.block_;
output << _T("}");
output << _T("\n");
return output;
}

// Workaround to calling overloaded << in derived classes.
void Iwhilestmt::print(std::_t_ostream &output)
{
output << *this;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

RFASem *Iwhilestmt::getCond()		{return cond_;}
RFASem *Iwhilestmt::getBlock()	{return block_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Iwhilestmt::setCond(RFASem *x)		{cond_	 = x;}
void	Iwhilestmt::setBlock(RFASem	*x)	{block_	 = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iwhilestmt::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iwhilestmt::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iwhilestmt count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iwhilestmt count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENWHILESTMT
* CR:		08/31/00 AM.
* SUBJ:	Generate whilestmt to a rules file.
*********************************************/
void Iwhilestmt::genWhilestmt(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		08/31/00 AM.
* SUBJ:	See if two whilestmts are identical in form.
*********************************************/

bool Iwhilestmt::same(Istmt *stmt1, Istmt *stmt2)
{
Iwhilestmt *whilestmt1 = (Iwhilestmt *) stmt1;
Iwhilestmt *whilestmt2 = (Iwhilestmt *) stmt2;
if (!whilestmt1 && !whilestmt2)
	return true;
if (!whilestmt1 || !whilestmt2)
	return false;

if (!RFASem::same(whilestmt1->cond_, whilestmt2->cond_))
	return false;
if (!RFASem::same(whilestmt1->block_, whilestmt2->block_))
	return false;
return true;
}


/********************************************
* FN:		EVAL
* CR:		08/31/00 AM.
* SUBJ:	Evaluate while-statement.
*********************************************/

bool Iwhilestmt::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;
if (!cond_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[whilestmt eval: Error. Not allowing infinite loop.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

RFASem *c_val = 0, *b_val = 0;
RFASem *tmp = 0;
enum RFASemtype typ = RSNULL;

bool ok = true;
_TCHAR *str;

for (;;)				// EXECUTE THE WHILE-LOOP.
	{
	ok = cond_->eval(nlppp, /*UP*/ c_val);
	if (!ok || !c_val)
		{
		if (!ok)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[whilestmt: Error(1).]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}
				// 10/18/00 AM.

		// SHOULD BE OK FOR A FUNCTION TO RETURN NO SEM, AS A WAY TO QUIT A WHILE LOOP.
							// FIX.	// 06/15/02 AM.
		// ok = false;	// FIX.	// 06/15/02 AM.
		goto done;
		}

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
				gerrStr << _T("[whilestmt: Error(2).]") << std::ends;
				nlppp->parse_->errOut(&gerrStr,false);
				}
			// 10/18/00 AM.
			ok = false;
			goto done;
			}
		typ = tmp->getType();
		c_val = tmp;
		}

	switch (typ)
		{
		case RSNAME:
		case RSNUM:
		case RSSTR:
			if (!(str = c_val->getName()) || !*str)
				goto done;
			break;
		case RSLONG:
			if (!c_val->getLong())
				goto done;
			break;
		case RS_KBCONCEPT:
			if (!c_val->getKBconcept())
				goto done;
			break;
		case RS_KBPHRASE:
			if (!c_val->getKBphrase())
				goto done;
			break;
		case RS_KBATTR:
			if (!c_val->getKBattr())
				goto done;
			break;
		case RS_KBVAL:
			if (!c_val->getKBval())
				goto done;
			break;
		case RSNODE:												// FIX.	// 10/30/00 AM.
			if (!c_val->getNode())								// FIX.	// 10/30/00 AM.
				goto done;											// FIX.	// 10/30/00 AM.
			break;																// 10/30/00 AM.
		case RSARGS:															// 12/07/00 AM.
			if (!c_val->getArgs())				// Allowing.			// 12/04/01 AM.
				goto done;														// 12/04/01 AM.
			break;
		case RSOSTREAM:						// Preemptive.				// 05/23/01 AM.
			if (!c_val->getOstream())										// 05/23/01 AM.
				goto done;														// 05/23/01 AM.
			break;																// 05/23/01 AM.
		default:
			ok = false;
			goto done;
		}

	if (c_val)														// FIX.	// 10/10/00 AM.
		{
		delete c_val;												// FIX.	// 10/10/00 AM.
		c_val = 0;													// FIX.	// 10/15/00 AM.
		}
	if (tmp)															// FIX.	// 10/10/00 AM.
		{
		delete tmp;													// FIX.	// 10/10/00 AM.
		tmp = 0;														// FIX.	// 10/15/00 AM.
		}

	// Eval block.
	if (block_)
		{
		ok = block_->eval(nlppp, val);									// 10/10/00 AM.
		if (nlppp->return_)													// 03/09/02 AM.
			{
			if (!nlppp->insidefn_)											// 03/12/02 AM.
				{
				nlppp->parse_->line_ = line_;								// 08/24/02 AM.
				std::_t_strstream gerrStr;			// 03/09/02 AM.
				gerrStr << _T("[Return statement not in a function.]") << std::ends;
				nlppp->parse_->errOut(&gerrStr,false);								// 03/09/02 AM.
				ok = false;														// 03/09/02 AM.
				}
			// val is the RETURN VALUE.  Don't delete it.			// 03/09/02 AM.
			goto done;															// 03/09/02 AM.
			}
		if (val)														// FIX.	// 10/10/00 AM.
			{
			delete val;												// FIX.	// 10/10/00 AM.
			val = 0;													// FIX.	// 10/15/00 AM.
			}
		if (!ok)																	// 10/10/00 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[whilestmt: Error(3).]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);

				// 10/18/00 AM.
			goto done;
			}
		if (nlppp->exitpass_)												// 11/19/02 AM.
			goto done;															// 11/19/02 AM.

		if (nlppp->succeed_)													// 02/13/02 AM.
			{
			switch (nlppp->region_)											// 02/13/02 AM.
				{
				case REGCHECK:													// 02/13/02 AM.
				case REGCODE:													// 02/13/02 AM.
//				case REGDECL:													// 03/12/02 AM.
					goto done;													// 02/13/02 AM.
				default:															// 02/13/02 AM.
					break;														// 02/13/02 AM.
				}
			}
		else if (nlppp->fail_)												// 02/13/02 AM.
			{
			switch (nlppp->region_)											// 02/13/02 AM.
				{
				case REGCHECK:													// 02/13/02 AM.
				case REGPOST:													// 02/13/02 AM.
					goto done;													// 02/13/02 AM.
				default:															// 02/13/02 AM.
					{
					std::_t_strstream gerrStr;		// 02/13/02 AM.
					gerrStr << _T("[Error: fail() in bad region.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 02/13/02 AM.
					}
					break;														// 02/13/02 AM.
				}
			}
		}

#ifdef GEODUCK_
	PeekHere();																	// 12/08/00 DD.
	if (gui_.IsMessage(GUI_MESSAGE_ABORT))								// 12/08/00 DD.
		break;																	// 12/08/00 DD.
#endif
	}											// END OF LOOP.
	
done:

if (c_val)
	delete c_val;
if (tmp)
	delete tmp;

return ok;
}


/********************************************
* FN:		GENEVAL
* CR:		08/31/00 AM.
* SUBJ:	Gen code to evaluate while-statement.
*********************************************/

bool Iwhilestmt::genEval(Gen *gen)
{
//std::_t_ofstream *fcode = gen->fcode_;
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
// Yet another alt:  generate while (truth(....)) -- then overload that
// function with all possible return types!  Elegant!  Can be an inline.
// This can be a general way....(but not totally optimized.)
// If var, evals it. (see Iwhilestmt::eval).
*fcode << indent << _T("while (Arun::truth(");				// FIX!	// 04/29/01 AM.
if (!cond_->genEval(gen))
	{
	*fcode << _T("\n// ERROR in whilestmt condition.") << std::endl;
	return false;
	}
*fcode << _T("))");
Gen::nl(fcode);																// 04/04/03 AM.

// Gen statements in the while-stmt block.
// (Could see if only one, then omit the curly braces.)
_TCHAR newindent[64];
_stprintf(newindent, _T("%s\t"), indent);
gen->setIndent(newindent);
*fcode << newindent << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
if (!block_->genEval(gen))
	{
	*fcode << _T("\n// ERROR in whilestmt block.") << std::endl;
	gen->setIndent(indent);			// Restore.
	return false;
	}
*fcode << newindent << _T("}");
Gen::eol(fcode);																// 04/04/03 AM.

gen->setIndent(indent);			// Restore.
return true;
}

/*************************      END OF FILE      ******************************/
