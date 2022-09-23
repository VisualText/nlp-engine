/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IOP.CPP
* FILE:	lit/iop.cpp
* CR:		11/09/99 AM.
* SUBJ:	Interpreted/internal operator semantic object. NLP++ semantics.
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
#include "tree.h"			// 02/26/01 AM.
#include "pn.h"			// 10/18/00 AM.
#include "rfasem.h"
#include "iop.h"

// WARN:	Ioptype and s_Optype must be kept in sync.
_TCHAR *s_Ioptype[] =
	{
	_T("<NULL>"),
	_T("+"),
	_T("-"),
	_T("*"),
	_T("/"),
	_T("%%"),					// Confidence operator!						// 12/17/99 AM.
	_T("++"),
	_T("--"),

	_T("="),

	// Relational ops
	_T("=="),
	_T("!="),
	_T("<"),
	_T("<="),
	_T(">"),
	_T(">="),

	// Bool
	_T("&&"),
	_T("||"),
	_T("!"),

	_T("dummy")
	};

int Iop::count_ = 0;

/********************************************
* FN:		Special Functions for Iop Class
********************************************/

Iop::Iop(						// Default constructor.
	enum Ioptype typ				// Operator type.
	)
{
type_ = typ;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Iop::~Iop()
{

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Iop::Iop(Iop &orig)
{
zero();
Iop::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Iop &Iop::operator=(const Iop &fm)
{
Iop *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iop object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iop::clear()
{
}

void Iop::zero()
{
type_ = NULLOP;
}

void Iop::copy(const Iop *orig)
{
Iop *dest;
dest = this;

dest->type_ = orig->type_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iop &op)
{
_TCHAR *str;
if ((str = Iop::Optype(op.type_)) && *str)
	output << str;
else
	output << _T("<BAD OP>");
return output;
}

/*******************************************/

/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

enum Ioptype	 Iop::getType()	{return type_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Iop::setType(enum Ioptype x)	{type_ = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iop::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iop::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iop count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iop count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENOP
* CR:		11/09/99 AM.
* SUBJ:	Generate op to a rules file.
*********************************************/
void Iop::genOp(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		11/09/99 AM.
* SUBJ:	See if two ops are identical in form.
*********************************************/

bool Iop::same(Iop *op1, Iop *op2)
{
if (!op1 && !op2)
	return true;
if (!op1 || !op2)
	return false;

if (op1->type_ != op2->type_)
	return false;

return true;
}


/********************************************
* FN:		OPTYPE
* CR:		11/09/99 AM.
* SUBJ:	Convert enum to str and vice versa.
*********************************************/

enum Ioptype Iop::Optype(_TCHAR *str)
{
if (!str || !*str)
	return NULLOP;
if (!*(str+1))				// Single-char op.
	switch (*str)
		{
		case '+':	return PLUSOP;
		case '-':	return MINUSOP;
		case '*':	return TIMESOP;
		case '/':	return DIVIDEOP;
		case '%':	return REMOP;		// Remainder.				// 12/31/99 AM.

		case '=':	return ASSIGNOP;

		case '<':	return LTOP;
		case '>':	return GTOP;

		case '!':	return NOTOP;

		default:		return NULLOP;
		}

if (*(str+2))
	return NULLOP;		// More than two chars.

// 2-char ops.
switch (*str)
	{
	case '+':
		if (*(str+1) == '+')
			return INCOP;
		return NULLOP;
	case '-':
		if (*(str+1) == '-')
			return DECOP;
		return NULLOP;
	case '=':
		if (*(str+1) == '=')
			return EQOP;
		return NULLOP;
	case '!':									// FIX.					// 12/13/99 AM.
		if (*(str+1) == '=')					// FIX.					// 12/13/99 AM.
			return NEOP;						// FIX.					// 12/13/99 AM.
		return NULLOP;							// FIX.					// 12/13/99 AM.
	case '<':
		if (*(str+1) == '=')
			return LEOP;
		if (*(str+1) == '<')												// 12/31/99 AM.
			return OUTOP;													// 12/31/99 AM.
		return NULLOP;
	case '>':
		if (*(str+1) == '=')
			return GEOP;
		return NULLOP;
	case '&':
		if (*(str+1) == '&')
			return ANDOP;
		return NULLOP;
	case '|':
		if (*(str+1) == '|')
			return OROP;
		return NULLOP;
	case '%':
		if (*(str+1) == '%')												// 12/17/99 AM.
			return CONFOP;													// 12/17/99 AM.
		return NULLOP;														// 12/17/99 AM.
	default:
		return NULLOP;
	}
}

_TCHAR *Iop::Optype(enum Ioptype typ)
{
switch (typ)
	{
	case PLUSOP:	return _T("+");
	case MINUSOP:	return _T("-");
	case TIMESOP:	return _T("*");
	case DIVIDEOP:	return _T("/");
	case REMOP:		return _T("%");		// Remainder op.				// 12/31/99 AM.
	case CONFOP:	return _T("%%");
	case INCOP:		return _T("++");
	case DECOP:		return _T("--");

	case ASSIGNOP:	return _T("=");
	case OUTOP:		return _T("<<");	// Output operator.			// 12/31/99 AM.

	case EQOP:		return _T("==");
	case NEOP:		return _T("!=");
	case LTOP:		return _T("<");
	case LEOP:		return _T("<=");
	case GTOP:		return _T(">");
	case GEOP:		return _T(">=");

	case ANDOP:		return _T("&&");
	case OROP:		return _T("||");
	case NOTOP:		return _T("!");

	case NULLOP:
	default:
		return _T("");
	}
}

/********************************************
* FN:		EVAL
* CR:		11/15/99 AM.
* SUBJ:	Evaluate operator in expression.
* OPT:	Kind of useless.  Shouldn't be called.
*********************************************/


bool Iop::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
// Copy and return.
Iop *tmp = new Iop(*this);	// Copy constructor.
val = new RFASem(tmp);
return true;
}


/********************************************
* FN:		EVAL
* CR:		11/15/99 AM.
* SUBJ:	Evaluate operator in expression.
*********************************************/


bool Iop::eval(Nlppp *nlppp, /*UP*/ enum Ioptype &optype)
{
// OPT: Kind of trivial.  Probably shouldn't be called.
optype = type_;
return true;
}

/*************************      END OF FILE      ******************************/
