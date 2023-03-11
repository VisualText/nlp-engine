/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IEXPR.CPP
* FILE:	lit/iexpr.cpp
* CR:		11/09/99 AM.
* SUBJ:	Interpreted/internal expr semantic object. NLP++ semantics.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"		// 02/26/01 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"
#include "chars.h"											// 07/16/03 AM.
#include "io.h"
#include "iarg.h"
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"				// 02/26/01 AM.
#include "nlppp.h"			// To get at parse.		// 12/15/99 AM.
#include "htab.h"		// 04/11/06 AM.
#include "pn.h"				// 02/14/01 AM.
#include "var.h"
#include "gen.h"												// 05/08/00 AM.
#include "ipair.h"
#include "ivar.h"
#include "parse.h"			// To intern strings.	// 12/15/99 AM.
#include "lite/vtrun.h"	// 04/11/06 AM.
#include "iexpr.h"
#include "rfasem.h"
#include "iop.h"

// WARN:	Iexprtype and s_Exprtype must be kept in sync.
_TCHAR *s_Iexprtype[] =
	{
	0,
	_T("binary"),
	_T("assign"),
	_T("unary"),
	_T("postunary"),

	_T("dummy")
	};

int Iexpr::count_ = 0;

/********************************************
* FN:		Special Functions for Iexpr Class
********************************************/

Iexpr::Iexpr(						// Default constructor.
	enum Iexprtype typ,				// Variable type.
	RFASem *lhs,
	Iop *op,
	RFASem *rhs,
	Iop *postop,
	long line																	// 05/18/01 AM.
	)
{
type_		= typ;
lhs_		= lhs;
op_		= op;
rhs_		= rhs;
postop_	= postop;
line_		= line;		// 05/18/01 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Iexpr::~Iexpr()
{
clear();

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Iexpr::Iexpr(Iexpr &orig)
{
zero();
Iexpr::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Iexpr &Iexpr::operator=(const Iexpr &fm)
{
Iexpr *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iexpr object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iexpr::clear()
{
if (lhs_)
	delete lhs_;
if (op_)
	delete op_;
if (rhs_)
	delete rhs_;
if (postop_)
	delete postop_;
}

void Iexpr::zero()
{
type_ = NULLEXPR;
lhs_		= 0;
op_		= 0;
rhs_		= 0;
postop_	= 0;
line_		= 0;		// 05/18/01 AM.
}

void Iexpr::copy(const Iexpr *orig)
{
Iexpr *dest;
dest = this;

dest->type_		= orig->type_;
dest->lhs_		= orig->lhs_;
dest->op_		= orig->op_;
dest->rhs_		= orig->rhs_;
dest->postop_	= orig->postop_;
dest->line_		= orig->line_;
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iexpr &expr)
{
switch (expr.type_)
	{
	// 12/27/99 AM. Fixing up parentheses.
	case BINARYEXPR:
	case ASSIGNEXPR:
		output << _T("(") << *expr.lhs_
				 << _T(" ") << *expr.op_ << _T(" ")
				 << *expr.rhs_ << _T(")");
		break;
	case UNARYEXPR:
		output << *expr.op_ << _T("(") << *expr.rhs_ << _T(")");
		break;
	case POSTUNARYEXPR:
		output << _T("(") << *expr.rhs_ << _T(")") << *expr.postop_;
		break;
	default:
		output << _T("<BAD EXPR>");
		break;
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

enum Iexprtype	 Iexpr::getType()		{return type_;  }
RFASem			*Iexpr::getLhs()		{return lhs_;   }
Iop				*Iexpr::getOp()		{return op_;    }
RFASem			*Iexpr::getRhs()		{return rhs_;   }
Iop				*Iexpr::getPostop()	{return postop_;}
long				 Iexpr::getLine()		{return line_;	 }

/********************************************
* FN:		Modify Functions
********************************************/

void	Iexpr::setType(enum Iexprtype  x)	{type_	= x;}
void	Iexpr::setLhs(RFASem				*x)	{lhs_		= x;}
void	Iexpr::setOp(Iop					*x)	{op_		= x;}
void	Iexpr::setRhs(RFASem				*x)	{rhs_		= x;}
void	Iexpr::setPostop(Iop				*x)	{postop_ = x;}
void	Iexpr::setLine(long				 x)	{line_	= x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iexpr::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iexpr::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iexpr count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iexpr count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENEXPR
* CR:		11/09/99 AM.
* SUBJ:	Generate expr to a rules file.
*********************************************/
void Iexpr::genExpr(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		11/09/99 AM.
* SUBJ:	See if two exprs are identical in form.
*********************************************/

bool Iexpr::same(Iexpr *expr1, Iexpr *expr2)
{
if (!expr1 && !expr2)
	return true;
if (!expr1 || !expr2)
	return false;

if (expr1->type_ != expr2->type_)
	return false;
if (!RFASem::same(expr1->lhs_, expr2->lhs_))
	return false;
if (!Iop::same(expr1->op_, expr2->op_))
	return false;
if (!RFASem::same(expr1->rhs_, expr2->rhs_))
	return false;
if (!Iop::same(expr1->postop_, expr2->postop_))
	return false;

return true;
}


/********************************************
* FN:		EXPRTYPE
* CR:		11/09/99 AM.
* SUBJ:	Convert enum to str and vice versa.
*********************************************/

enum Iexprtype Iexpr::Exprtype(_TCHAR *str)
{
if (!str || !*str)
	return NULLEXPR;
if (!_tcscmp(str, _T("binary")))
	return BINARYEXPR;
if (!_tcscmp(str, _T("assign")))
	return ASSIGNEXPR;
if (!_tcscmp(str, _T("unary")))
	return UNARYEXPR;
if (!_tcscmp(str, _T("postunary")))
	return POSTUNARYEXPR;
return NULLEXPR;
}

_TCHAR *Iexpr::Exprtype(enum Iexprtype typ)
{
switch (typ)
	{
	case BINARYEXPR:		return _T("binary");
	case ASSIGNEXPR:		return _T("assign");
	case UNARYEXPR:		return _T("unary");
	case POSTUNARYEXPR:	return _T("postunary");
	default:					return _T("");
	}
}


/********************************************
* FN:		EVAL
* CR:		11/15/99 AM.
* SUBJ:	Evaluate an NLP++ expression.
*********************************************/

bool Iexpr::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;
bool ok = true;
RFASem *rval = 0;	// All types use this one.
RFASem *lval = 0;

Ivar *var;
Ipair *pair;
Dlist<Iarg> *vals;
Delt<Iarg> *darg = 0;
long index=FULLARRAY;														// 10/14/00 AM.
Iarg *arg = NULL;																		// 10/14/00 AM.
//bool deleting = false;														// 10/14/00 AM.
if (!rhs_)
	{
	ruleError(line_,nlppp);	// Id current rule line.				// 12/21/01 AM.
	nlppp->parse_->line_ = line_;											// 08/24/02 AM.
	_stprintf(Errbuf,_T("[No rhs in expression.]"));						// 05/18/01 AM.
	return nlppp->parse_->errOut(false); // UNFIXED 									// 05/18/01 AM.
	}
// BUG: Eval'ing rhs first is wrong order.							// 01/02/00 AM.

switch (type_)
	{
	////////////////////////////////
	//		BINARY EXPRESSION
	////////////////////////////////

	case BINARYEXPR:
		{
		// Get left, right, op, and put them together.
		if (!lhs_ || !op_)
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			_stprintf(Errbuf,_T("[Binary expression with no lhs.]"));	// 05/18/01 AM.
			nlppp->parse_->errOut(false); // UNFIXED 									// 05/18/01 AM.
			ok = false;
			goto done;
			}
		if (!(ok = lhs_->eval(nlppp, /*UP*/ lval)))					// 11/21/00 AM.
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			_stprintf(Errbuf,_T("[Bad lhs expression.]"));					// 05/18/01 AM.
			nlppp->parse_->errOut(false); // UNFIXED 									// 05/18/01 AM.
//			*gerr << "[Bad lhs expression: " << *lhs_					// 05/18/01 AM.
//					<< "]" << std::endl;											// 05/18/01 AM.
			goto done;
			}

		// C/C++ STYLE EVALUATION OF ANDOP and OROP HERE.			// 12/24/02 AM.
		if (eval_lhs_only(lval,/*UP*/val,ok))							// 12/24/02 AM.
			// If !ok, could report an error here.						//	12/24/02 AM.
			goto done;															// 12/24/02 AM.

		if (!(ok = rhs_->eval(nlppp, /*UP*/ rval)))		// FIX.	// 01/02/00 AM.
//				|| !rval)														// 09/30/00 AM.
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			_stprintf(Errbuf,_T("[Bad rhs expression.]"));					// 05/18/01 AM.
			nlppp->parse_->errOut(false); // UNFIXED 									// 05/18/01 AM.
//			*gerr << "[Bad rhs expression: " << *rhs_					// 05/18/01 AM.
//					<< "]" << std::endl;											// 05/18/01 AM.
			goto done;
			}
		// Allowing empty rhs in all expr types now.					// 11/22/00 AM.
#ifdef OBSOLETE_
		if (!rval)																// 09/30/00 AM.
			{
			// Allowing NULL expr in an output statement.			// 09/30/00 AM.
			if (op_->getType() != OUTOP)									// 09/30/00 AM.
				{
				ruleError(line_,nlppp);										// 12/21/01 AM.
				nlppp->parse_->line_ = line_;								// 08/24/02 AM.
				_stprintf(Errbuf,_T("[Empty rhs expression.]"));			// 05/18/01 AM.
				nlppp->parse_->errOut(false); // UNFIXED 								// 05/18/01 AM.
//				*gerr << "[Empty rhs expression: " << *rhs_			// 05/18/01 AM.
//						<< "]" << std::endl;										// 05/18/01 AM.
				goto done;
				}
			}
#endif
		switch(op_->getType())
			{
			case PLUSOP:
				if (!lval && !rval)											// 11/21/00 AM.
					val = new RFASem(0LL);								// 11/21/00 AM.
				else if (!lval)												// 11/21/00 AM.
					{
					switch (rval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
							val = new RFASem(rval->getLong());			// 11/21/00 AM.
							break;
						case RSFLOAT:											// 08/19/01 AM.
							val = new RFASem(rval->getFloat());			// 08/19/01 AM.
							break;
						case RSSTR:												// 11/21/00 AM.
							val = new RFASem(rval->getName(), RSSTR);	// 11/21/00 AM.
							break;
						case RSNUM:	// Bastard str-num.					// 03/12/02 AM.
							{
							long long num=0;											// 12/24/02 AM.
							if (!str_to_long(rval->getName(),num))		// 03/12/02 AM.
								{
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
								}
							val = new RFASem(num);							// 03/12/02 AM.	
							}					
							break;												// 03/12/02 AM.

						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
									_T("[Binary plus: Error. Bad rhs.]"));	// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary plus: Error. Bad rhs:"	// 05/18/01 AM.
//								<< *rhs_ << "]" << std::endl;					// 05/18/01 AM.
							ok = false;
							goto done;
						}
					}
				else if (!rval)												// 11/21/00 AM.
					{
					switch (lval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
							val = new RFASem(lval->getLong());			// 11/21/00 AM.
							break;
						case RSFLOAT:											// 08/19/01 AM.
							val = new RFASem(lval->getFloat());			// 08/19/01 AM.
							break;
						case RSSTR:												// 11/21/00 AM.
							val = new RFASem(lval->getName(), RSSTR);	// 11/21/00 AM.
							break;
						case RSNUM:	// Bastard str-num.					// 03/12/02 AM.
							{
							long long num=0;											// 12/24/02 AM.
							if (!str_to_long(rval->getName(),num))		// 03/12/02 AM.
								{
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
								}
							val = new RFASem(num);							// 03/12/02 AM.
							}						
							break;												// 03/12/02 AM.
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
									_T("[Binary plus: Error. Bad lhs.]"));	// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary plus: Error. Bad lhs:"	// 05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
							ok = false;
							goto done;
						}
					}
				else																// 11/21/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						switch(rval->getType())
							{
							case RSLONG:
								{
								// Two numbers.
								long long num=0;										// 12/24/02 AM.
								num = lval->getLong() + rval->getLong();
								val = new RFASem(num);
								}
								break;
							case RSFLOAT:										// 08/19/01 AM.
								{
								float fnum =									// 12/24/02 AM.
											(float) lval->getLong()			// 08/19/01 AM.
										 + rval->getFloat();					// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSNUM:	// Bastard str-num.				// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num += lval->getLong();						// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;											// 03/12/02 AM.
							case RSSTR:											// 08/17/02 AM.
								if (lval->getLong() == 0)					// 08/17/02 AM.
									{
							val = new RFASem(rval->getName(), RSSTR);	// 08/17/02 AM.
									break;										// 08/17/02 AM.
									}
								// Fall through!								// 08/17/02 AM.
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 05/18/01 AM.
							_T("[Binary plus: Error. Rhs nonnumeric.]"));	// 05/18/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 05/18/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSFLOAT:												// 08/19/01 AM.
						switch(rval->getType())								// 08/19/01 AM.
							{
							case RSLONG:										// 08/19/01 AM.
								{
								// Two numbers.
								float fnum =									// 12/24/02 AM.
										lval->getFloat()						// 08/19/01 AM.
										 + (float) rval->getLong();		// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSFLOAT:										// 08/19/01 AM.
								{
								float fnum =									// 12/24/02 AM.
										lval->getFloat()						// 08/19/01 AM.
										 + rval->getFloat();					// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 08/19/01 AM.
							_T("[Binary plus: Error. Rhs nonnumeric.]"));	// 08/19/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 08/19/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSSTR:													// 12/15/99 AM.
						// Handling string catenation.
						switch (rval->getType())							// 12/15/99 AM.
							{
							case RSSTR:											// 12/15/99 AM.
								{
								// Build a new string.
								_TCHAR *str1 = lval->getName();				// 12/24/02 AM.
								_TCHAR *str2 = rval->getName();				// 12/24/02 AM.
								if (empty(str1) && empty(str2))			// 10/09/03 AM.
									{
									ruleError(line_,nlppp);					// 10/09/03 AM.
									nlppp->parse_->line_ = line_;			// 10/09/03 AM.
									_stprintf(Errbuf,							// 10/09/03 AM.
										_T("[Adding zeroed strings.]"));		// 10/09/03 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 10/09/03 AM.
									val = new RFASem(0LL);				// 10/09/03 AM.
									ok = true;
									goto done;
									}
								else if (empty(str1))
									{
									val = new RFASem(str2,RSSTR);
									ok = true;
									goto done;
									}
								else if (empty(str2))
									{
									val = new RFASem(str1,RSSTR);
									ok = true;
									goto done;
									}
								_TCHAR *buf = Chars::create(					// 07/16/03 AM.
									_tcsclen(str1)+_tcsclen(str2)+1);
								_stprintf(buf, _T("%s%s"), str1,str2);			// 12/15/99 AM.
								// LOCAL STRING, TEMPORARY STRING MANAGEMENT.	// 01/08/07 AM.
								// NOT INTERNING STRING.					// 01/08/07 AM.
								//_TCHAR *str=0;									// 12/24/02 AM.
								//nlppp->parse_->internStr(buf, /*UP*/ str);
								//Chars::destroy(buf);							// 07/16/03 AM.
								//val = new RFASem(str, RSSTR);				// 12/15/99 AM.
								val = new RFASem(buf, RSSTR);	// ***	// 01/08/07 AM.
								// ADD STRING TO LOCAL STRINGS.			// 01/08/07 AM.
								if (!nlppp->localstrs_)						// 01/08/07 AM.
									nlppp->localstrs_ = new Slist<_TCHAR>(buf); // 01/08/07 AM.
								else												// 01/08/07 AM.
									nlppp->localstrs_->push(buf);			// 01/08/07 AM.
								ok = true;										// 12/15/99 AM.
								goto done;										// 12/15/99 AM.
								}
							case RSLONG:										// 08/17/02 AM.
								if (rval->getLong() == 0)					// 08/17/02 AM.
									{
							val = new RFASem(lval->getName(), RSSTR);	// 08/17/02 AM.
									break;										// 08/17/02 AM.
									}
								// Fall through.								// 08/17/02 AM.
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 05/18/01 AM.
									_T("[Incompatible terms in expr.]"));	// 05/18/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 05/18/01 AM.
//				*gerr << "[Incompatible terms in expr.]" << std::endl;	// 05/18/01 AM.
								ok = false;										// 12/15/99 AM.
								goto done;										// 12/15/99 AM.
							}
						break;
					case RSNUM:													// 03/12/02 AM.
						{
						long long num1=0;											// 12/24/02 AM.

						if (!str_to_long(lval->getName(),num1))		// 03/12/02 AM.
							{
							ok = false;											// 03/12/02 AM.
							goto done;											// 03/12/02 AM.
							}
						switch(rval->getType())								// 03/12/02 AM.
							{
							case RSLONG:										// 03/12/02 AM.
								{
								// Two numbers.								// 03/12/02 AM.
								long long num=0;										// 12/24/02 AM.
								num = num1 + rval->getLong();				// 03 12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							case RSFLOAT:										// 03 12/02 AM.
								{
								float fnum =									// 12/24/02 AM.
										(float) num1							// 03 12/02 AM.
										 + rval->getFloat();					// 03 12/02 AM.
								val = new RFASem(fnum);						// 03 12/02 AM.
								}
								break;
							case RSNUM:	// Bastard str-num.				// 03 12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num += num1;									// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}						
								break;											// 03/12/02 AM.
							default:
								ruleError(line_,nlppp);						// 03 12/02 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 03 12/02 AM.
							_T("[Binary plus: Error. Rhs nonnumeric.]"));	// 03 12/02 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 03 12/02 AM.
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
							}
						}
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
							_T("[Binary plus. Error: Lhs bad type.]"));	// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
//						*gerr << "[Binary plus: Lhs bad type: "		// 05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
						ok = false;
						goto done;
					}
				break;
			case MINUSOP:
				if (!lval && !rval)											// 11/21/00 AM.
					val = new RFASem(0LL);								// 11/21/00 AM.
				else if (!lval)												// 11/21/00 AM.
					{
					switch (rval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
							{
							long long num=0;											// 12/24/02 AM.
							num = - (rval->getLong());						// 11/21/00 AM.
							val = new RFASem(num);							// 11/21/00 AM.
							}
							break;
						case RSFLOAT:											// 08/19/01 AM.
							{
							float fnum =										// 12/24/02 AM.
									- (rval->getFloat());					// 08/19/01 AM.
							val = new RFASem(fnum);							// 08/19/01 AM.
							}
							break;
						case RSNUM:												// 03/12/02 AM.
							{
							long long num=0;											// 12/24/02 AM.
							if (!str_to_long(rval->getName(),num))		// 03/12/02 AM.
								{
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
								}
							val = new RFASem(-num);							// 03/12/02 AM.
							}
							break;
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Binary minus: Error. Bad rhs.]"));		// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary minus: Error. Bad rhs:"	// 05/18/01 AM.
//								<< *rhs_ << "]" << std::endl;					// 05/18/01 AM.
							ok = false;
							goto done;
						}
					}
				else if (!rval)												// 11/21/00 AM.
					{
					switch (lval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
							{
							long long num=0;											// 12/24/02 AM.
							num = lval->getLong();							// 11/21/00 AM.
							val = new RFASem(num);							// 11/21/00 AM.
							}
							break;
						case RSFLOAT:											// 08/19/01 AM.
							{
							float fnum = lval->getFloat();						// 08/19/01 AM.
							val = new RFASem(fnum);							// 08/19/01 AM.
							}
							break;
						case RSNUM:												// 03/12/02 AM.
							{
							long long num=0;											// 12/24/02 AM.
							if (!str_to_long(lval->getName(),num))		// 03/12/02 AM.
								{
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
								}
							val = new RFASem(num);							// 03/12/02 AM.
							}
							break;
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Binary minus: Error. Bad lhs.]"));		// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary minus: Error. Bad lhs:"	// 11/21/00 AM.
//								<< *lhs_ << "]" << std::endl;
							ok = false;
							goto done;
						}
					}
				else																// 11/21/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						switch(rval->getType())
							{
							case RSLONG:
								{
								// Two numbers.
								long long num=0;										// 12/24/02 AM.
								num = lval->getLong() - rval->getLong();
								val = new RFASem(num);
								}
								break;
							case RSFLOAT:										// 08/19/01 AM.
								{
								float fnum = (float) lval->getLong()			// 08/19/01 AM.
										 - rval->getFloat();					// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSNUM:											// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num = lval->getLong() - num;				// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 05/18/01 AM.
						_T("[Binary minus: Error. Rhs nonnumeric.]"));	// 05/18/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 05/18/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSFLOAT:												// 08/19/01 AM.
						switch(rval->getType())								// 08/19/01 AM.
							{
							case RSLONG:
								{
								// Two numbers.
								float fnum = lval->getFloat()						// 08/19/01 AM.
										 - (float) rval->getLong();		// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSFLOAT:										// 08/19/01 AM.
								{
								float fnum = lval->getFloat()						// 08/19/01 AM.
										 - rval->getFloat();					// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSNUM:											// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								float fnum = lval->getFloat()						// 03/12/02 AM.
										 - (float) num;						// 03/12/02 AM.
								val = new RFASem(fnum);						// 03/12/02 AM.
								}
								break;
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 08/19/01 AM.
						_T("[Binary minus: Error. Rhs nonnumeric.]"));	// 08/19/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 08/19/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSNUM:													// 03/12/02 AM.
						{
						long long num1=0;											// 12/24/02 AM.

						if (!str_to_long(lval->getName(),num1))		// 03/12/02 AM.
							{
							ok = false;											// 03/12/02 AM.
							goto done;											// 03/12/02 AM.
							}
						switch(rval->getType())
							{
							case RSLONG:										// 03/12/02 AM.
								{
								// Two numbers.								// 03/12/02 AM.
								long long num=0;										// 12/24/02 AM.
								num = num1 - rval->getLong();				// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							case RSFLOAT:										// 03/12/02 AM.
								{
								float fnum = (float) num1							// 03/12/02 AM.
										 - rval->getFloat();					// 03/12/02 AM.
								val = new RFASem(fnum);						// 03/12/02 AM.
								}
								break;
							case RSNUM:											// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num = num1 - num;								// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							default:
								ruleError(line_,nlppp);						// 03/12/02 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 03/12/02 AM.
						_T("[Binary minus: Error. Rhs nonnumeric.]"));	// 03/12/02 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 03/12/02 AM.
								ok = false;
								goto done;
							}
						}
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
						_T("[Binary minus: Error. Lhs nonnumeric.]"));	// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
//						*gerr << "[Binary minus: Lhs nonnumeric: "	// 05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
						ok = false;
						goto done;
					}
				break;
			case TIMESOP:
				if (!lval && !rval)											// 11/21/00 AM.
					val = new RFASem(0LL);								// 11/21/00 AM.
				else if (!lval)												// 11/21/00 AM.
					{
					switch (rval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
						case RSNUM:							// PREEMPTIVE.	// 03/12/02 AM.
							val = new RFASem(0LL);						// 11/21/00 AM.
							break;
						case RSFLOAT:											// 08/19/01 AM.
							val = new RFASem((float)0.0);					// 08/19/01 AM.
							break;
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Binary times: Error. Bad rhs.]"));		// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary times: Error. Bad rhs:"	// 05/18/01 AM.
//								<< *rhs_ << "]" << std::endl;					// 05/18/10 AM.
							ok = false;
							goto done;
						}
					}
				else if (!rval)												// 11/21/00 AM.
					{
					switch (lval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
						case RSNUM:												// 03/12/02 AM.
							val = new RFASem(0LL);						// 11/21/00 AM.
							break;
						case RSFLOAT:											// 08/19/01 AM.
							val = new RFASem((float)0.0);					// 08/19/01 AM.
							break;
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Binary times: Error. Bad lhs.]"));		// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary times: Error. Bad lhs:"	// 05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
							ok = false;
							goto done;
						}
					}
				else																// 11/21/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						switch(rval->getType())
							{
							case RSLONG:
								{
								// Two numbers.
								long long num=0;										// 12/24/02 AM.
								num = lval->getLong() * rval->getLong();
								val = new RFASem(num);
								}
								break;
							case RSFLOAT:										// 08/19/01 AM.
								{
								float fnum = (float) lval->getLong()			// 08/19/01 AM.
										 * rval->getFloat();					// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSNUM:	// Bastard str-num.				// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num *= lval->getLong();						// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;											// 03/12/02 AM.
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 05/18/01 AM.
									_T("[Binary times: Rhs nonnumeric.]"));	// 05/18/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 05/18/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSFLOAT:												// 08/19/01 AM.
						switch(rval->getType())								// 08/19/01 AM.
							{
							case RSLONG:										// 08/19/01 AM.
								{
								// Two numbers.
								float fnum =						// FIX.	// 12/24/02 AM.
										(float) (lval->getFloat()
										* (float) rval->getLong());		// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSFLOAT:										// 08/19/01 AM.
								{
								float fnum = lval->getFloat()				// 12/24/02 AM.
										 * rval->getFloat();					// 08/19/01 AM.
								val = new RFASem(fnum);						// 08/19/01 AM.
								}
								break;
							case RSNUM:	// Bastard str-num.				// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(lval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								float fnum =
										(float) num * lval->getFloat();	// 12/24/02 AM.
								val = new RFASem(fnum);						// 03/12/02 AM.
								}
								break;											// 03/12/02 AM.
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 08/19/01 AM.
									_T("[Binary times: Rhs nonnumeric.]"));	// 08/19/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 08/19/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSNUM:													// 03/12/02 AM.
						{
						long long num1=0;											// 12/24/02 AM.

						if (!str_to_long(lval->getName(),num1))		// 03/12/02 AM.
							{
							ok = false;											// 03/12/02 AM.
							goto done;											// 03/12/02 AM.
							}
						switch(rval->getType())								// 03/12/02 AM.
							{
							case RSLONG:										// 03/12/02 AM.
								{
								// Two numbers.								// 03/12/02 AM.
								long long num=0;										// 12/24/02 AM.
								num = num1 * rval->getLong();				// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							case RSFLOAT:										// 03/12/02 AM.
								{
								float fnum = (float) num1					// 12/24/02 AM.
										 * rval->getFloat();					// 03/12/02 AM.
								val = new RFASem(fnum);						// 03/12/02 AM.
								}
								break;
							case RSNUM:	// Bastard str-num.				// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num))	// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num *= num1;									// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}						
								break;											// 03/12/02 AM.
							default:
								ruleError(line_,nlppp);						// 03/12/02 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 03/12/02 AM.
									_T("[Binary times: Rhs nonnumeric.]"));	// 03/12/02 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 03/12/02 AM.
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
							}
						}
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
							_T("[Binary times: Error. Lhs nonnumeric]"));	// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
//						*gerr << "[Binary times: Lhs nonnumeric: "	// 05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
						ok = false;
						goto done;
					}
				break;
			case DIVIDEOP:
				if (!lval && !rval)											// 11/21/00 AM.
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Undefined division.]"));					// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Undefined division.]" << std::endl;			// 05/18/01 AM.
					ok = false;
					goto done;
					}
				else if (!lval)												// 11/21/00 AM.
					{
					switch (rval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
							{
							long long num=0;											// 12/24/02 AM.
							if (!(num = rval->getLong()))					// 11/21/00 AM.
								{
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 05/18/01 AM.
									_T("[Error. Division by zero.]"));		// 05/18/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 05/18/01 AM.
								ok = false;
								goto done;
								}
							val = new RFASem(0LL);						// 11/21/00 AM.
							}
							break;
						case RSFLOAT:											// 08/20/01 AM.
							if (!(rval->getFloat()))						// 12/24/02 AM.
								{
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 08/20/01 AM.
									_T("[Error. Division by zero.]"));		// 08/20/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 08/20/01 AM.
								ok = false;										// 08/20/01 AM.
								goto done;										// 08/20/01 AM.
								}
							val = new RFASem((float)0.0);					// 08/20/01 AM.
							break;
						case RSNUM:												// 03/12/02 AM.
							{
							long long num=0;											// 12/24/02 AM.
							if (!str_to_long(rval->getName(),num))		// 03/12/02 AM.
								{
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
								}
							if (!num)											// 03/12/02 AM.
								{
								ruleError(line_,nlppp);						// 03/12/02 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 03/12/02 AM.
									_T("[Error. Division by zero.]"));		// 03/12/02 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 03/12/02 AM.
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
								}
							val = new RFASem(0LL);						// 03/12/02 AM.
							}
							break;
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Binary divide: Error. Bad rhs.]"));	// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary divide: Error. Bad rhs:" // 05/18/01 AM.
//								<< *rhs_ << "]" << std::endl;					// 05/18/01 AM.
							ok = false;
							goto done;
						}
					}
				else if (!rval)												// 11/21/00 AM.
					{
					switch (lval->getType())								// 11/21/00 AM.
						{
						case RSLONG:											// 11/21/00 AM.
						case RSNUM:												// 03/12/02 AM.
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Error. Division by zero.]"));			// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
							ok = false;
							goto done;
						case RSFLOAT:											// 08/20/01 AM.
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 08/20/01 AM.
								_T("[Error. Division by zero.]"));			// 08/20/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 08/20/01 AM.
							ok = false;											// 08/20/01 AM.
							goto done;											// 08/20/01 AM.
						default:
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							_stprintf(Errbuf,									// 05/18/01 AM.
								_T("[Binary divid: Error. Bad lhs.]"));		// 05/18/01 AM.
							nlppp->parse_->errOut(false); // UNFIXED 					// 05/18/01 AM.
//							*gerr << "[Binary divide: Error. Bad lhs:" //05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
							ok = false;
							goto done;
						}
					}
				else																// 11/21/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						switch(rval->getType())
							{
							case RSLONG:
								{
								// Two numbers.
								long long num=0;										// 12/24/02 AM.
								long long num2=0;									// 12/24/02 AM.
								if (!(num2 = rval->getLong()))			// 12/16/99 AM.
									{
									ruleError(line_,nlppp);					// 12/21/01 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 05/18/01 AM.
											_T("[Error. Division by zero]"));	// 05/18/01 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 05/18/01 AM.
									ok = false;
									goto done;
									}
								num = lval->getLong() / num2;
								val = new RFASem(num);
								}
								break;
							case RSFLOAT:
								{
								// Two numbers.
								float ffnum2=0.0;								// 12/24/02 AM.
								if (!(ffnum2 = rval->getFloat()))		// 08/20/01 AM.
									{
									ruleError(line_,nlppp);					// 12/21/01 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 08/20/01 AM.
											_T("[Error. Division by zero]"));	// 08/20/01 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 08/20/01 AM.
									ok = false;									// 08/20/01 AM.
									goto done;									// 08/20/01 AM.
									}
								float fnum =										// 12/24/02 AM.
										(float)lval->getLong() / ffnum2;	// 08/20/01 AM.
								val = new RFASem(fnum);						// 08/20/01 AM.
								}
								break;
							case RSNUM:											// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								long long num2=0;									// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num2))// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								if (!num2)										// 03/12/02 AM.
									{
									ruleError(line_,nlppp);					// 03/12/02 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 03/12/02 AM.
											_T("[Error. Division by zero]"));	// 03/12/02 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 03/12/02 AM.
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num = lval->getLong() / num2;				// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							default:
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 05/18/01 AM.
								_T("[Binary divide: Rhs nonnumeric.]"));	// 05/18/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 05/18/01 AM.
								ok = false;
								goto done;
							}
						break;
					case RSFLOAT:												// 08/20/01 AM.
						switch(rval->getType())								// 08/20/01 AM.
							{
							case RSLONG:										// 08/20/01 AM.
								{
								// Two numbers.
								long long num2=0;									// 12/24/02 AM.
								if (!(num2 = rval->getLong()))			// 08/20/01 AM.
									{
									ruleError(line_,nlppp);					// 12/21/01 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 08/20/01 AM.
											_T("[Error. Division by zero]"));	// 08/20/01 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 08/20/01 AM.
									ok = false;									// 08/20/01 AM.
									goto done;									// 08/20/01 AM.
									}
								float fnum = lval->getFloat() / (float)num2;	// 08/20/01 AM.
								val = new RFASem(fnum);						// 08/20/01 AM.
								}
								break;
							case RSFLOAT:										// 08/20/01 AM.
								{
								// Two numbers.
								float ffnum2=0.0;								// 12/24/02 AM.
								if (!(ffnum2 = rval->getFloat()))		// 08/20/01 AM.
									{
									ruleError(line_,nlppp);					// 12/21/01 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 08/20/01 AM.
											_T("[Error. Division by zero]"));	// 08/20/01 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 08/20/01 AM.
									ok = false;									// 08/20/01 AM.
									goto done;									// 08/20/01 AM.
									}
								float fnum = lval->getFloat() / ffnum2;	// 12/24/02 AM.
								val = new RFASem(fnum);						// 08/20/01 AM.
								}
								break;
							case RSNUM:											// 03/12/02 AM.
								{
								long long num2=0;									// 12/24/02 AM.
								if (!str_to_long(rval->getName(),num2))// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								if (!num2)										// 03/12/02 AM.
									{
									ruleError(line_,nlppp);					// 03/12/02 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 03/12/02 AM.
											_T("[Error. Division by zero]"));	// 03/12/02 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 03/12/02 AM.
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								float fnum =										// 12/24/02 AM.
										lval->getFloat() / (float)num2;	// 03/12/02 AM.
								val = new RFASem(fnum);						// 03/12/02 AM.
								}
								break;
							default:												// 08/20/01 AM.
								ruleError(line_,nlppp);						// 12/21/01 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 08/20/01 AM.
								_T("[Binary divide: Rhs nonnumeric.]"));	// 08/20/01 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 08/20/01 AM.
								ok = false;										// 08/20/01 AM.
								goto done;										// 08/20/01 AM.
							}
						break;
					case RSNUM:													// 03/12/02 AM.
						{
						long long num1=0;											// 12/24/02 AM.

						if (!str_to_long(lval->getName(),num1))		// 03/12/02 AM.
							{
							ok = false;											// 03/12/02 AM.
							goto done;											// 03/12/02 AM.
							}
						switch(rval->getType())								// 03/12/02 AM.
							{
							case RSLONG:										// 03/12/02 AM.
								{
								// Two numbers.
								long long num=0;										// 12/24/02 AM.
								long long num2=0;									// 12/24/02 AM.

								if (!(num2 = rval->getLong()))			// 03/12/02 AM.
									{
									ruleError(line_,nlppp);					// 03/12/02 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 03/12/02 AM.
											_T("[Error. Division by zero]"));	// 03/12/02 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 03/12/02 AM.
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num = num1 / num2;							// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							case RSFLOAT:										// 03/12/02 AM.
								{
								// Two numbers.								// 03/12/02 AM.
								float ffnum2=0.0;								// 12/24/02 AM.
								if (!(ffnum2 = rval->getFloat()))		// 03/12/02 AM.
									{
									ruleError(line_,nlppp);					// 03/12/02 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 03/12/02 AM.
											_T("[Error. Division by zero]"));	// 03/12/02 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 03/12/02 AM.
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								float fnum = (float)num1 / ffnum2;		// 12/24/02 AM.
								val = new RFASem(fnum);						// 03/12/02 AM.
								}
								break;
							case RSNUM:											// 03/12/02 AM.
								{
								long long num=0;										// 12/24/02 AM.
								long long num2=0;									// 12/24/02 AM.

								if (!str_to_long(rval->getName(),num2))// 03/12/02 AM.
									{
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								if (!num2)										// 03/12/02 AM.
									{
									ruleError(line_,nlppp);					// 03/12/02 AM.
									nlppp->parse_->line_ = line_;			// 08/24/02 AM.
									_stprintf(Errbuf,							// 03/12/02 AM.
											_T("[Error. Division by zero]"));	// 03/12/02 AM.
									nlppp->parse_->errOut(false); // UNFIXED 			// 03/12/02 AM.
									ok = false;									// 03/12/02 AM.
									goto done;									// 03/12/02 AM.
									}
								num = num1 / num2;							// 03/12/02 AM.
								val = new RFASem(num);						// 03/12/02 AM.
								}
								break;
							default:												// 03/12/02 AM.
								ruleError(line_,nlppp);						// 03/12/02 AM.
								nlppp->parse_->line_ = line_;				// 08/24/02 AM.
								_stprintf(Errbuf,								// 03/12/02 AM.
								_T("[Binary divide: Rhs nonnumeric.]"));	// 03/12/02 AM.
								nlppp->parse_->errOut(false); // UNFIXED 				// 03/12/02 AM.
								ok = false;										// 03/12/02 AM.
								goto done;										// 03/12/02 AM.
							}
						}
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
							_T("[Binary divide: Lhs nonnumeric.]"));		// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
//						*gerr << "[Binary divide: Lhs nonnumeric: "	// 05/18/01 AM.
//								<< *lhs_ << "]" << std::endl;					// 05/18/01 AM.
						ok = false;
						goto done;
					}
				break;
			case REMOP:															// 12/31/99 AM.
				if (!lval && !rval)											// 11/21/00 AM.
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Remainder by zero.]"));					// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Remainder by zero!]" << std::endl;			// 05/18/01 AM.
					ok = false;
					goto done;
					}
				if (lval && lval->getType() != RSLONG)					// 11/21/00 AM.
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Remainder: Bad lhs.]"));					// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Remainder: Bad lhs: "						// 05/18/01 AM.
//							<< *lhs_ << "]" << std::endl;						// 05/18/01 AM.
					ok = false;
					goto done;
					}
				if (rval && rval->getType() != RSLONG)					// 11/21/00 AM.
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Remainder: bad rhs.]"));					// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Remainder: Bad rhs: "						// 05/18/01 AM.
//							<< *rhs_ << "]" << std::endl;						// 05/18/01 AM.
					ok = false;
					goto done;
					}
				if (!rval)
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Remainder by zero.]"));					// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Remainder by zero!]" << std::endl;			// 05/18/01 AM.
					ok = false;
					goto done;
					}
				if (!(rval->getLong()))
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Remainder by zero.]"));					// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Remainder by zero!]" << std::endl;			// 05/18/01 AM.
					ok = false;
					goto done;
					}

				if (!lval)
					val = new RFASem(0LL);
				else		// Both values present.							// 11/21/00 AM.
					val = new RFASem(lval->getLong() % rval->getLong());
				break;
			case CONFOP:						// %% operator				// 12/17/99 AM.
				{
				long long num=0;														// 12/24/02 AM.
				long long num1=0;													// 12/24/02 AM.
				long long num2=0;													// 12/24/02 AM.
				if (!lval && !rval)											// 11/21/00 AM.
					{
					val = new RFASem(0LL);
					goto done;
					}
				if (lval && lval->getType() != RSLONG)
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Conf: Bad lhs.]"));						// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//			*gerr << "[conf: Bad lhs: " << *lhs_ << "]" << std::endl;	// 05/18/01 AM.
					ok = false;
					goto done;
					}
				if (rval && rval->getType() != RSLONG)
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Error. Conf: Bad rhs.]"));						// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//			*gerr << "[conf: Bad rhs: " << *rhs_ << "]" << std::endl;	// 05/18/01 AM.
					ok = false;
					goto done;
					}
				if (lval)
					num1 = lval->getLong();
				else
					num1 = 0;
				if (rval)
					num2 = rval->getLong();
				else
					num2 = 0;
				if (num1 < -100 || num1 > 100)
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Warning: Confidence not -100 to 100.]"));	// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Warning: Confidence '" << num1			// 05/18/01 AM.
//						<< "' not between -100 and 100.]" << std::endl;	// 05/18/01 AM.
					}
				if (num2 < -100 || num2 > 100)
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					_stprintf(Errbuf,											// 05/18/01 AM.
						_T("[Warning: Confidence not -100 to 100.]"));	// 05/18/01 AM.
					nlppp->parse_->errOut(false); // UNFIXED 							// 05/18/01 AM.
//					*gerr << "[Warning: Confidence '" << num2			// 05/18/01 AM.
//						<< "' not between -100 and 100.]" << std::endl;	// 05/18/01 AM.
					}
				num = confidence(num1,num2);
				val = new RFASem(num);
				}
				break;
			case EQOP:
			case NEOP:
				{
				long long num=0;														// 12/24/02 AM.
				float ffnum1=0.0;												// 12/24/02 AM.
				float ffnum2=0.0;												// 12/24/02 AM.
				long long num1=0;													// 12/24/02 AM.
				long long num2=0;													// 12/24/02 AM.
				bool fflt1 = false, fflt2 = false;						// 12/24/02 AM.
				_TCHAR *str1=0;													// 12/24/02 AM.
				_TCHAR *str2=0;													// 12/24/02 AM.
				bool fnum1=false;												// 12/24/02 AM.
				bool fnum2=false;												// 12/24/02 AM.
				bool empty1=false;											// 12/24/02 AM.
				bool empty2=false;											// 12/24/02 AM.
				int kb1=0;														// 12/24/02 AM.
				int kb2=0;														// 12/24/02 AM.

				if (!lval && !rval)											// 11/22/00 AM.
					{
					switch (op_->getType())									// 11/22/00 AM.
						{
						case EQOP:
							val = new RFASem(1LL);
							goto done;
						case NEOP:
							val = new RFASem(0LL);
							goto done;
						}
					}
				// 03/15/00 AM.  EQ and NEQ are different from the other
				// relational operators, in that they can admit many more
				// types.
				if (lval)														// 11/22/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						//str1 = 0;	// Shouldn't be needed.				// 11/22/00 AM.
						num1 = lval->getLong();
						fnum1 = true;
						if (num1 == 0)											// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						break;
					case RSFLOAT:												// 08/20/01 AM.
						ffnum1 = lval->getFloat();							// 08/20/01 AM.
						fflt1 = true;											// 08/20/01 AM.
						break;
					case RSSTR:
					case RSNAME:
					case RSNUM:
						{
						//num1 = str_to_long(str1, num1);				// 11/22/00 AM.
						str1 = lval->getNum();
						if (!str1 || !*str1)									// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						}
						break;
					case RS_KBCONCEPT:										// 03/15/00 AM.
						num1 = (long) (lval->getKBconcept());
						kb1 = 1;
						fnum1 = true;
						if (!num1)												// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						break;
					case RS_KBPHRASE:											// 03/15/00 AM.
						num1 = (long) (lval->getKBphrase());
						kb1 = 2;
						fnum1 = true;
						if (!num1)												// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						break;
					case RS_KBATTR:											// 03/15/00 AM.
						num1 = (long) (lval->getKBattr());
						kb1 = 3;
						fnum1 = true;
						if (!num1)												// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						break;
					case RS_KBVAL:												// 03/15/00 AM.
						num1 = (long) (lval->getKBval());
						kb1 = 4;
						fnum1 = true;
						if (!num1)												// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						break;
					case RSNODE:												// 10/30/00 AM.
						num1 = (long) (lval->getNode());					// 10/30/00 AM.
						kb1 = 5;													// 10/30/00 AM.
						fnum1 = true;											// 10/30/00 AM.
						if (!num1)												// 08/11/02 AM.
							empty1 = true;										// 08/11/02 AM.
						break;
					case RSARGS:												// 12/04/01 AM.
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 12/04/01 AM.
							_T("[Relop: Not handling arrays.]"));			// 12/04/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 12/04/01 AM.
						ok = false;
						goto done;
						break;													// 12/04/01 AM.
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
							_T("[Binary relop: Error. Bad lhs.]"));			// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
						ok = false;
						goto done;
					}
				if (rval)														// 11/22/00 AM.
				switch(rval->getType())
					{
					case RSLONG:
						//str2 = 0;	// Shouldn't be needed.				// 11/22/00 AM.
						num2 = rval->getLong();
						fnum2 = true;
						if (!num2)												// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						break;
					case RSFLOAT:												// 08/20/01 AM.
						ffnum2 = rval->getFloat();							// 08/20/01 AM.
						fflt2 = true;											// 08/20/01 AM.
						break;
					case RSSTR:
					case RSNUM:
					case RSNAME:
						{
						str2 = rval->getNum();
						//fnum2 = str_to_long(str2, num2);	// FIX.	// 11/22/00 AM.
						if (!str2 || !*str2)									// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						}
						break;
					case RS_KBCONCEPT:										// 03/15/00 AM.
						num2 = (long) (rval->getKBconcept()); // FIX	// 10/30/00 AM.
						kb2 = 1;
						fnum2 = true;
						if (!num2)												// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						break;
					case RS_KBPHRASE:											// 03/15/00 AM.
						num2 = (long) (rval->getKBphrase()); // FIX	// 10/30/00 AM.
						kb2 = 2;
						fnum2 = true;
						if (!num2)												// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						break;
					case RS_KBATTR:											// 03/15/00 AM.
						num2 = (long) (rval->getKBattr());	// FIX	// 10/30/00 AM.
						kb2 = 3;
						fnum2 = true;
						if (!num2)												// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						break;
					case RS_KBVAL:												// 03/15/00 AM.
						num2 = (long) (rval->getKBval());	// FIX	// 10/30/00 AM.
						kb2 = 4;
						fnum2 = true;
						if (!num2)												// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						break;
					case RSNODE:												// 10/30/00 AM.
						num2 = (long) (rval->getNode());					// 10/30/00 AM.
						kb2 = 5;													// 10/30/00 AM.
						fnum2 = true;											// 10/30/00 AM.
						if (!num2)												// 08/11/02 AM.
							empty2 = true;										// 08/11/02 AM.
						break;													// 10/30/00 AM.
					case RSARGS:												// 12/04/01 AM.
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 12/04/01 AM.
							_T("[Relop: Not handling arrays.]"));			// 12/04/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 12/04/01 AM.
						ok = false;
						goto done;
						break;													// 12/04/01 AM.
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
							_T("[Binary relop: Error. Bad rhs.]"));			// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
//						*gerr << "[Binary relop: Bad rhs: "				// 05/18/01 AM.
//								<< *rhs_ << "]" << std::endl;					// 05/18/01 AM.
						ok = false;
						goto done;
					}
				switch (op_->getType())		// AGAIN!!
					{
					case EQOP:
						{
						if (!lval)												// 11/22/00 AM.
							{
							if (fnum2)
								num = (num2 ? 0 : 1);
							else if (fflt2)									// 08/20/01 AM.
								num = (ffnum2 ? 0 : 1);						// 08/20/01 AM.
							else
								num = ((str2 && *str2) ? 0 : 1);
							}
						else if (!rval)										// 11/22/00 AM.
							{
							if (fnum1)
								num = (num1 ? 0 : 1);
							else if (fflt1)									// 08/20/01 AM.
								num = (ffnum1 ? 0 : 1);						// 08/20/01 AM.
							else
								num = ((str1 && *str1) ? 0 : 1);
							}
						else if (empty1 && empty2)							// 08/11/02 AM.
							num = 1;												// 08/11/02 AM.
						else if (empty1 || empty2)							// 08/11/02 AM.
							num = 0;												// 08/11/02 AM.
						else if (fnum1 && fnum2)		// Both are numeric.
							{
							num = ((num1 == num2)
									&& (kb1 == kb2));							// 03/15/00 AM.
							if (kb1 != kb2)						// FIX.	// 07/06/06 AM.
								{
								ruleError(line_,nlppp);						// 07/06/06 AM.
								nlppp->parse_->line_ = line_;				// 07/06/06 AM.
								_stprintf(Errbuf,								// 07/06/06 AM.
							_T("[Error: Bad types in '==' operation.]"));	// 07/06/06 AM.
								nlppp->parse_->errOut(false);				// 07/06/06 AM.
								}
							}
						else if (fflt1 && fflt2)							// 08/20/01 AM.
							num = (ffnum1 == ffnum2);						// 08/20/01 AM.
						else if (!kb1 && fflt2)		// Mixed num		// 08/20/01 AM.
							num = ((float)num1 == ffnum2);				// 08/20/01 AM.
						else if (fflt1 && !kb2)		// Mixed num		// 08/20/01 AM.
							num = (ffnum1 == (float)num2);				// 08/20/01 AM.
						else if (str1 && str2)	// Both stringable.
							(_tcscmp(str1,str2) ? (num = 0) : (num = 1));
						else
							num = 0;					// Can't be equal.
						}
						break;
					case NEOP:
						{
						if (!lval)												// 11/22/00 AM.
							{
							if (fnum2)
								num = (num2 ? 1 : 0);
							else if (fflt2)									// 08/20/01 AM.
								num = (ffnum2 ? 1 : 0);						// 08/20/01 AM.
							else
								num = ((str2 && *str2) ? 1 : 0);
							}
						else if (!rval)										// 11/22/00 AM.
							{
							if (fnum1)
								num = (num1 ? 1 : 0);
							else if (fflt1)									// 08/20/01 AM.
								num = (ffnum1 ? 1 : 0);						// 08/20/01 AM.
							else
								num = ((str1 && *str1) ? 1 : 0);
							}
						else if (empty1 && empty2)							// 08/11/02 AM.
							num = 0;												// 08/11/02 AM.
						else if (empty1 || empty2)							// 08/11/02 AM.
							num = 1;												// 08/11/02 AM.
						else if (fnum1 && fnum2)		// Both are numeric.
							{
							num = ((num1 != num2)
									|| (kb1 != kb2));							// 03/15/00 AM.
							if (kb1 != kb2)						// FIX.	// 07/06/06 AM.
								{
								ruleError(line_,nlppp);						// 07/06/06 AM.
								nlppp->parse_->line_ = line_;				// 07/06/06 AM.
								_stprintf(Errbuf,								// 07/06/06 AM.
							_T("[Error: Bad types in '!=' operation.]"));	// 07/06/06 AM.
								nlppp->parse_->errOut(false);				// 07/06/06 AM.
								}
							}
						else if (fflt1 && fflt2)							// 08/20/01 AM.
							num = (ffnum1 != ffnum2);						// 08/20/01 AM.
						else if (!kb1 && fflt2)								// 08/20/01 AM.
							num = ((float)num1 != ffnum2);				// 08/20/01 AM.
						else if (fflt1 && !kb2)								// 08/20/01 AM.
							num = (ffnum1 != (float)num2);				// 08/20/01 AM.
						else if (str1 && str2)	// Both stringable.
							(_tcscmp(str1,str2) ? (num = 1) : (num = 0));
						else
							num = 1;					// Can't be equal.
						}
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						_stprintf(Errbuf,										// 05/18/01 AM.
							_T("[Relop: Error.]"));								// 05/18/01 AM.
						nlppp->parse_->errOut(false); // UNFIXED 						// 05/18/01 AM.
//						*gerr << "[Relop error.]" << std::endl;				// 05/18/01 AM.
						ok = false;
						goto done;
					}
				val = new RFASem(num);
				}
				break;
			case GTOP:
			case LTOP:
			case GEOP:
			case LEOP:
				{
				long long num=0;														// 12/24/02 AM.
				float ffnum1=0.0;												// 12/24/02 AM.
				float ffnum2=0.0;												// 12/24/02 AM.
				long long num1=0;													// 12/24/02 AM.
				long long num2=0;													// 12/24/02 AM.
				bool fflt1 = false, fflt2 = false;						// 12/24/02 AM.
				_TCHAR *str1=0;													// 12/24/02 AM.
				_TCHAR *str2=0;													// 12/24/02 AM.
				bool fnum1=false;												// 12/24/02 AM.
				bool fnum2=false;												// 12/24/02 AM.
				int kb1=0;														// 12/24/02 AM.
				int kb2=0;														// 12/24/02 AM.

				if (!lval && !rval)											// 11/22/00 AM.
					{
					val = new RFASem(0LL);
					goto done;
					}
				if (lval)														// 11/22/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						str1 = 0;
						num1 = lval->getLong();
						fnum1 = true;
						break;
					case RSFLOAT:												// 08/20/01 AM.
						str1 = 0;												// 08/20/01 AM.
						ffnum1 = lval->getFloat();							// 08/20/01 AM.
						fflt1 = true;											// 08/20/01 AM.
						break;
					case RSSTR:
					case RSNUM:
					case RSNAME:
						str1 = lval->getNum();
						//fnum1 = str_to_long(str1, num1); // FIX.	// 11/22/00 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Binary relop: Bad lhs: ")
								  << *lhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
				if (rval)														// 11/22/00 AM.
				switch(rval->getType())
					{
					case RSLONG:
						str2 = 0;
						num2 = rval->getLong();
						fnum2 = true;
						break;
					case RSFLOAT:												// 08/20/01 AM.
						str2 = 0;												// 08/20/01 AM.
						ffnum2 = rval->getFloat();							// 08/20/01 AM.
						fflt2 = true;											// 08/20/01 AM.
						break;
					case RSSTR:
					case RSNUM:
					case RSNAME:
						str2 = rval->getNum();
						//fnum2 = str_to_long(str2, num2);				// 11/22/00 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Binary relop: Bad rhs: ")
								  << *rhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
				switch (op_->getType())		// AGAIN!!
					{
					case GTOP:
						if (!lval)												// 11/22/00 AM.
							{
							if (fnum2)
								num = ((0 > num2) ? 1 : 0);
							else if (fflt2)									// 08/20/01 AM.
								num = (((float)0.0 > ffnum2) ? 1 : 0);	// 08/20/01 AM.
							else
								num = 0;
							}
						else if (!rval)										// 11/22/00 AM.
							{
							if (fnum1)
								num = ((num1 > 0) ? 1 : 0);
							else if (fflt1)									// 08/20/01 AM.
								num = ((ffnum1 > (float)0.0) ? 1 : 0);	// 08/20/01 AM.
							else
								num = ((str1 && *str1) ? 1 : 0);
							}
						else if (fnum1 && fnum2)		// Both are numeric.
							num = (num1 > num2);
						else if (fflt1 && fflt2)							// 08/21/01 AM.
							num = (ffnum1 > ffnum2);						// 08/21/01 AM.
						else if (!kb1 && fflt2)								// 08/21/01 AM.
							num = ((float)num1 > ffnum2);					// 08/21/01 AM.
						else if (fflt1 && !kb2)								// 08/21/01 AM.
							num = (ffnum1 > (float)num2);					// 08/21/01 AM.
						else if (str1 && str2)	// Both stringable.
							((_tcscmp(str1,str2)>0) ? (num = 1) : (num = 0));
						else							// Gibberish.
							{
							ok = false;
							goto done;
							}
						break;
					case LTOP:
						if (!lval)												// 11/22/00 AM.
							{
							if (fnum2)
								num = ((0 < num2) ? 1 : 0);
							else if (fflt2)									// 08/22/01 AM.
								num = (((float)0.0 < ffnum2) ? 1 : 0);	// 08/22/01 AM.
							else
								num = ((str2 && *str2) ? 1 : 0);
							}
						else if (!rval)										// 11/22/00 AM.
							{
							if (fnum1)
								num = ((num1 < 0) ? 1 : 0);
							else if (fflt1)									// 08/22/01 AM.
								num = ((ffnum1 < (float)0.0) ? 1 : 0);	// 08/22/01 AM.
							else
								num = 0;
							}
						else if (fnum1 && fnum2)		// Both are numeric.
							num = (num1 < num2);
						else if (fflt1 && fflt2)							// 08/22/01 AM.
							num = (ffnum1 < ffnum2);						// 08/22/01 AM.
						else if (!kb1 && fflt2)								// 08/22/01 AM.
							num = ((float)num1 < ffnum2);					// 08/22/01 AM.
						else if (fflt1 && !kb2)								// 08/22/01 AM.
							num = (ffnum1 < (float)num2);					// 08/22/01 AM.
						else if (str1 && str2)	// Both stringable.
							((_tcscmp(str1,str2)<0) ? (num = 1) : (num = 0));
						else							// Gibberish.
							{
							ok = false;
							goto done;
							}
						break;
					case GEOP:
						if (!lval)												// 11/22/00 AM.
							{
							if (fnum2)
								num = ((0 >= num2) ? 1 : 0);
							else if (fflt2)									// 08/22/01 AM.
								num = (((float)0.0 >= ffnum2) ? 1 : 0);// 08/22/01 AM.
							else
								num = ((str2 && *str2) ? 0 : 1);
							}
						else if (!rval)										// 11/22/00 AM.
							{
							if (fnum1)
								num = ((num1 >= 0) ? 1 : 0);
							else if (fflt1)									// 08/22/01 AM.
								num = ((ffnum1 >= (float)0.0) ? 1 : 0);// 08/22/01 AM.
							else
								num = 1;
							}
						else if (fnum1 && fnum2)		// Both are numeric.
							num = (num1 >= num2);
						else if (fflt1 && fflt2)							// 08/22/01 AM.
							num = (ffnum1 >= ffnum2);						// 08/22/01 AM.
						else if (!kb1 && fflt2)								// 08/22/01 AM.
							num = ((float)num1 >= ffnum2);				// 08/22/01 AM.
						else if (fflt1 && !kb2)								// 08/22/01 AM.
							num = (ffnum1 >= (float)num2);				// 08/22/01 AM.
						else if (str1 && str2)	// Both stringable.
							((_tcscmp(str1,str2)>=0) ? (num = 1) : (num = 0));
						else							// Gibberish.
							{
							ok = false;
							goto done;
							}
						break;
					case LEOP:
						if (!lval)												// 11/22/00 AM.
							{
							if (fnum2)
								num = ((0 <= num2) ? 1 : 0);
							else if (fflt2)									// 08/22/01 AM.
								num = (((float)0.0 <= ffnum2) ? 1 : 0);// 08/22/01 AM.
							else
								num = 1;
							}
						else if (!rval)										// 11/22/00 AM.
							{
							if (fnum1)
								num = ((num1 <= 0) ? 1 : 0);
							else if (fflt1)									// 08/22/01 AM.
								num = ((ffnum1 <= (float)0.0) ? 1 : 0);// 08/22/01 AM.
							else
								num = ((str1 && *str1) ? 0 : 1);
							}
						else if (fnum1 && fnum2)		// Both are numeric.
							num = (num1 <= num2);
						else if (fflt1 && fflt2)							// 08/22/01 AM.
							num = (ffnum1 <= ffnum2);						// 08/22/01 AM.
						else if (!kb1 && fflt2)								// 08/22/01 AM.
							num = ((float)num1 <= ffnum2);				// 08/22/01 AM.
						else if (fflt1 && !kb2)								// 08/22/01 AM.
							num = (ffnum1 <= (float)num2);				// 08/22/01 AM.
						else if (str1 && str2)	// Both stringable.
							((_tcscmp(str1,str2)<=0) ? (num = 1) : (num = 0));
						else							// Gibberish.
							{
							ok = false;
							goto done;
							}
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Relop error.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);

						ok = false;
						goto done;
					}
				val = new RFASem(num);
				}
				break;
			case ANDOP:
				{
				long long num=0;														// 12/24/02 AM.
				long long num1=0;													// 12/24/02 AM.
				long long num2=0;													// 12/24/02 AM.

				// Now handled by eval_lhs_only							// 12/24/02 AM.
				if (
#ifdef OLD_
					!lval || 
#endif
					!rval)														// 11/22/00 AM.
					{
					val = new RFASem(0LL);
					goto done;
					}
#ifdef OLD_
				switch (lval->getType())
					{
					case RSLONG:
						num1 = lval->getLong();
						break;
					case RSFLOAT:												// 08/22/01 AM.
						num1 = (lval->getFloat() ? 1 : 0);				// 08/22/01 AM.
						break;
					case RSNUM:
						if (!str_to_long(lval->getNum(), num1))
							{
							ok = false;
							goto done;
							}
						break;
					case RSSTR:										// FIX.	// 12/15/99 AM.
					case RSNAME:									// FIX.	// 12/15/99 AM.
						{
						_TCHAR *str = lval->getName();						// 12/24/02 AM.
						if (str && *str)										// 12/15/99 AM.
							num1 = 1;	// "true".							// 12/15/99 AM.
						else
							num1 = 0;	// "false"							// 12/15/99 AM.
						}
						break;													// 12/15/99 AM.
					case RS_KBCONCEPT:										// 03/15/00 AM.
						num1 = (lval->getKBconcept() ? 1 : 0);
						break;
					case RS_KBPHRASE:											// 03/15/00 AM.
						num1 = (lval->getKBphrase() ? 1 : 0);
						break;
					case RS_KBATTR:											// 03/15/00 AM.
						num1 = (lval->getKBattr() ? 1 : 0);
						break;
					case RS_KBVAL:												// 03/15/00 AM.
						num1 = (lval->getKBval() ? 1 : 0);
						break;
					case RSNODE:												// 10/30/00 AM.
						num1 = (lval->getNode() ? 1 : 0);				// 10/30/00 AM.
						break;													// 10/30/00 AM.
					case RSOSTREAM:				// Preemptive.			// 05/23/01 AM.
						num1 = (lval->getOstream() ? 1 : 0);			// 05/23/01 AM.
						break;													// 05/23/01 AM.
					case RSARGS:	// Array.								// 12/04/01 AM.
						num1 = 1;												// 12/04/01 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Binary and: Bad lhs: ")
								  << *lhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
#endif
				switch(rval->getType())
					{
					case RSLONG:
						num2 = rval->getLong();
						break;
					case RSFLOAT:												// 08/22/01 AM.
						num2 = (rval->getFloat() ? 1 : 0);				// 08/22/01 AM.
						break;
					case RSNUM:
						if (!str_to_long(rval->getNum(), num2))
							{
							ok = false;
							goto done;
							}
						break;
					case RSSTR:										// FIX.	// 12/15/99 AM.
					case RSNAME:									// FIX.	// 12/15/99 AM.
						{
						_TCHAR *str = rval->getName();						// 12/24/02 AM.
						if (str && *str)										// 12/15/99 AM.
							num2 = 1;	// "true".							// 12/15/99 AM.
						else
							num2 = 0;	// "false"							// 12/15/99 AM.
						}
						break;													// 12/15/99 AM.
					case RS_KBCONCEPT:										// 03/15/00 AM.
						num2 = (rval->getKBconcept() ? 1 : 0); //FIX	// 10/30/00 AM.
						break;
					case RS_KBPHRASE:											// 03/15/00 AM.
						num2 = (rval->getKBphrase() ? 1 : 0); // FIX	// 10/30/00 AM.
						break;
					case RS_KBATTR:											// 03/15/00 AM.
						num2 = (rval->getKBattr() ? 1 : 0);	// FIX	// 10/30/00 AM.
						break;
					case RS_KBVAL:												// 03/15/00 AM.
						num2 = (rval->getKBval() ? 1 : 0);	// FIX	// 10/30/00 AM.
						break;
					case RSNODE:												// 10/30/00 AM.
						num2 = (rval->getNode() ? 1 : 0);				// 10/30/00 AM.
						break;													// 10/30/00 AM.
					case RSOSTREAM:				// Preemptive.			// 05/23/01 AM.
						num2 = (rval->getOstream() ? 1 : 0);			// 05/23/01 AM.
						break;													// 05/23/01 AM.
					case RSARGS:	// Array.								// 12/04/01 AM.
						num2 = 1;												// 12/04/01 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Binary and: Bad rhs: ")
								  << *rhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
				// Two numbers.
//				num = (num1 && num2);
//				val = new RFASem(num);
				val = new RFASem(num2 ? 1LL : 0LL);			// 12/24/02 AM.
				}
				break;
			case OROP:
				{
				long long num=0;														// 12/24/02 AM.
				long long num1=0;													// 12/24/02 AM.
				long long num2=0;													// 12/24/02 AM.

				if (
#ifdef OLD_
					!lval &&
#endif
					!rval)														// 11/22/00 AM.
					{
					val = new RFASem(0LL);
					goto done;
					}
#ifdef OLD_
				if (lval)														// 11/22/00 AM.
				switch (lval->getType())
					{
					case RSLONG:
						num1 = lval->getLong();
						break;
					case RSFLOAT:												// 08/22/01 AM.
						num1 = (lval->getFloat() ? 1 : 0);				// 08/22.01 AM.
						break;
					case RSNUM:
						if (!str_to_long(lval->getNum(), num1))
							{
							ok = false;
							goto done;
							}
						break;
					case RSSTR:										// FIX.	// 12/15/99 AM.
					case RSNAME:									// FIX.	// 12/15/99 AM.
						{
						_TCHAR *str = lval->getName();						// 12/24/02 AM.
						if (str && *str)										// 12/15/99 AM.
							num1 = 1;	// "true".							// 12/15/99 AM.
						else
							num1 = 0;	// "false"							// 12/15/99 AM.
						}
						break;													// 12/15/99 AM.
					case RS_KBCONCEPT:										// 03/15/00 AM.
						num1 = (lval->getKBconcept() ? 1 : 0);
						break;
					case RS_KBPHRASE:											// 03/15/00 AM.
						num1 = (lval->getKBphrase() ? 1 : 0);
						break;
					case RS_KBATTR:											// 03/15/00 AM.
						num1 = (lval->getKBattr() ? 1 : 0);
						break;
					case RS_KBVAL:												// 03/15/00 AM.
						num1 = (lval->getKBval() ? 1 : 0);
						break;
					case RSNODE:												// 10/30/00 AM.
						num1 = (lval->getNode() ? 1 : 0);				// 10/30/00 AM.
						break;													// 10/30/00 AM.
					case RSOSTREAM:				// Preemptive.			// 05/23/01 AM.
						num1 = (lval->getOstream() ? 1 : 0);			// 05/23/01 AM.
						break;													// 05/23/01 AM.
					case RSARGS:	// Array.								// 12/04/01 AM.
						num1 = 1;												// 12/04/01 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Binary or: Bad lhs: ")
								  << *lhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
#endif
				if (rval)														// 11/22/00 AM.
				switch(rval->getType())
					{
					case RSLONG:
						num2 = rval->getLong();
						break;
					case RSFLOAT:												// 08/22/01 AM.
						num2 = (rval->getFloat() ? 1 : 0);				// 09/27/01 AM.
						break;
					case RSNUM:
						if (!str_to_long(rval->getNum(), num2))
							{
							ok = false;
							goto done;
							}
						break;
					case RSSTR:										// FIX.	// 12/15/99 AM.
					case RSNAME:									// FIX.	// 12/15/99 AM.
						{
						_TCHAR *str = rval->getName();						// 12/24/02 AM.
						if (str && *str)										// 12/15/99 AM.
							num2 = 1;	// "true".							// 12/15/99 AM.
						else
							num2 = 0;	// "false"							// 12/15/99 AM.
						}
						break;													// 12/15/99 AM.
					case RS_KBCONCEPT:										// 03/15/00 AM.
						num2 = (rval->getKBconcept() ? 1 : 0);// FIX	// 10/30/00 AM.
						break;
					case RS_KBPHRASE:											// 03/15/00 AM.
						num2 = (rval->getKBphrase() ? 1 : 0); // FIX	// 10/30/00 AM.
						break;
					case RS_KBATTR:											// 03/15/00 AM.
						num2 = (rval->getKBattr() ? 1 : 0);	// FIX	// 10/30/00 AM.
						break;
					case RS_KBVAL:												// 03/15/00 AM.
						num2 = (rval->getKBval() ? 1 : 0);	// FIX	// 10/30/00 AM.
						break;
					case RSNODE:												// 10/30/00 AM.
						num2 = (rval->getNode() ? 1 : 0);				// 10/30/00 AM.
						break;													// 10/30/00 AM.
					case RSOSTREAM:				// Preemptive.			// 05/23/01 AM.
						num2 = (rval->getOstream() ? 1 : 0);			// 05/23/01 AM.
						break;													// 05/23/01 AM.
					case RSARGS:	// Array.								// 12/04/01 AM.
						num1 = 1;												// 12/04/01 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Binary or: Bad rhs: ")
								  << *rhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
				// Two numbers.
//				num = (num1 || num2);
//				val = new RFASem(num);
				val = new RFASem(num2 ? 1LL : 0LL);			// 12/24/02 AM.
				}
				break;
			case OUTOP:		// C++ -like << output operator!			// 12/31/99 AM.
				{
				std::_t_ostream *ostr=0;												// 12/24/02 AM.

				if (!lval)														// 11/22/00 AM.
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					std::_t_strstream gerrStr;
					gerrStr << _T("[Output op: Error. Empty lhs.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);

					ok = false;
					goto done;
					}
				switch (lval->getType())
					{
					case RSOSTREAM:	// New type for convenience.
						// Strung together output ops return this.
						ostr = lval->getOstream();
					// cout << _T("[LINUX DEBUG cbuf(): got iexpr/OUTOP/lval/ostr]") << std::endl;	// 09/29/19 AM.
						break;
					case RSVAR:
						// If it's an ostream val, use that.
						// If it's a string val, see if global ostream exists.
						// If not, warn user to use fileout, then make the var!
						// Other vals are error.
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						{
						std::_t_strstream gerrStr;
						gerrStr << _T("[unimplemented RSVAR in output op.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
						}

						ok = false;
						goto done;
						break;
					case RSSTR:
						{
						_TCHAR *str = lval->getName();						// 12/24/02 AM.
						if (!str || !*str)
							{
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							std::_t_strstream gerrStr;
							gerrStr << _T("[Error. Can't output to empty file.]")
									  << std::ends;
							nlppp->parse_->errOut(&gerrStr,false);

							ok = false;
							goto done;
							}
						Ipair *pair;
						bool exists,open,inuse,badname;
						if (!Var::filevar(str,nlppp->getParse(),
									/*UP*/ pair,ostr,exists,open,inuse,badname))
							{
							ruleError(line_,nlppp);							// 12/21/01 AM.
							nlppp->parse_->line_ = line_;					// 08/24/02 AM.
							std::_t_strstream gerrStr;
							gerrStr << _T("[Output op: Error. Couldn't make var=")
									  << str << _T("]") << std::ends;
							nlppp->parse_->errOut(&gerrStr,false);

							ok = false;
							goto done;
							}
						}
						break;
					case RSNAME:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						{
						std::_t_strstream gerrStr;
						gerrStr << _T("[Output operator: Local vars unimplemented.]")
								  << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
						}
						ok = false;
						goto done;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						{
						std::_t_strstream gerrStr;
						gerrStr << _T("[Output operator: Bad lhs: ")
								  << *lhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
						}
					// 08/20/00 AM.
						ok = false;
						goto done;
					}
				if (!rval)		// RHS is empty.							// 09/30/00 AM.
					{
					// Print nothing.  Just pass on ostream.			// 09/30/00 AM.
					if (ostr)													// 09/30/00 AM
						val = new RFASem(ostr);								// 09/30/00 AM.
					goto done;													// 09/30/00 AM.
					}


				if (!ostr)	// Empty ostr.									// 05/13/02 AM.
					{
					std::_t_strstream gerrStr;		// 05/13/02 AM.
					gerrStr << _T("[Error: Empty output stream.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 05/13/02 AM.
					ok = false;													// 05/13/02 AM.
					goto done;													// 05/13/02 AM.
					}

				// Right-hand side can be anything printable.
				switch(rval->getType())
					{
					case RSLONG:
						try														// 10/27/04 AM.
							{		
						*ostr << rval->getLong();		// Print it!	// 12/24/02 AM.
							}
						catch(...)												// 10/27/04 AM.
							{
					ruleError(line_,nlppp);									// 10/27/04 AM.
					nlppp->parse_->line_ = line_;							// 10/27/04 AM.
					std::_t_strstream gerrStr;		// 10/27/04 AM.
					gerrStr << _T("[Error: Can't output.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 10/27/04 AM.
					ok = false;													// 10/27/04 AM.
					goto done;													// 10/27/04 AM.
							}
						break;
					case RSFLOAT:												// 08/22/01 AM.
						try														// 10/27/04 AM.
							{		
						*ostr << rval->getFloat();							// 08/22/01 AM.
							}
						catch(...)												// 10/27/04 AM.
							{
					ruleError(line_,nlppp);									// 10/27/04 AM.
					nlppp->parse_->line_ = line_;							// 10/27/04 AM.
					std::_t_strstream gerrStr;		// 10/27/04 AM.
					gerrStr << _T("[Error: Can't output.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 10/27/04 AM.
					ok = false;													// 10/27/04 AM.
					goto done;													// 10/27/04 AM.
							}
						break;
					case RSNUM:
						{
						long long num2=0;											// 12/24/02 AM.
						if (!str_to_long(rval->getNum(), num2))
							{
							ok = false;
							goto done;
							}
						try														// 10/27/04 AM.
							{		
							*ostr << num2;					// Print it!
							}
						catch(...)												// 10/27/04 AM.
							{
					ruleError(line_,nlppp);									// 10/27/04 AM.
					nlppp->parse_->line_ = line_;							// 10/27/04 AM.
					std::_t_strstream gerrStr;		// 10/27/04 AM.
					gerrStr << _T("[Error: Can't output.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 10/27/04 AM.
					ok = false;													// 10/27/04 AM.
					goto done;													// 10/27/04 AM.
							}
						}
						break;
					case RSSTR:
					case RSNAME:
						{
					// cout << _T("[LINUX DEBUG cbuf(): at iexpr/OUTOP/rval/RSSTR]") << std::endl;	// 09/29/19 AM.
						// Name should become identifiers, local vars, whatever.
						_TCHAR *str = rval->getName();						// 12/24/02 AM.
						if (str && *str)
							try													// 10/27/04 AM.
								{
								// Amazing that this works ok when LHS is cbuf() !!	// 09/28/19 AM.
								// (Else, cbuf() can get its own expr type, rather than OUTOP. Or a conditional.)
								*ostr << str;				// Print it!
								// Try unicode output overload.	// 01/06/06 AM.
								//char *lpstr8;
								//u_to_mbcs((LPCWSTR)str, CP_UTF8, (LPCTSTR*&)lpstr8);
								//*ostr << lpstr8;
								//u_delete((LPCTSTR*&)lpstr8);
								}
							catch(...)											// 10/27/04 AM.
								{
						ruleError(line_,nlppp);								// 10/27/04 AM.
						nlppp->parse_->line_ = line_;						// 10/27/04 AM.
						std::_t_strstream gerrStr;	// 10/27/04 AM.
						gerrStr << _T("[Error: Can't output.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);						// 10/27/04 AM.
						ok = false;												// 10/27/04 AM.
						goto done;												// 10/27/04 AM.
								}

						}
						break;
					case RS_KBCONCEPT:										// 07/26/00 AM.
					case RS_KBPHRASE:											// 07/26/00 AM.
					case RS_KBATTR:											// 07/26/00 AM.
					case RS_KBVAL:												// 07/26/00 AM.
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						{
						std::_t_strstream gerrStr;
						gerrStr << _T("[Error: Can't print kb object.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
						}
						ok = true;					// Tolerate.			// 10/15/00 AM.
						goto done;												// 07/26/00 AM.
					case RSNODE:												// 10/30/00 AM.
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						{
						std::_t_strstream gerrStr;
						gerrStr << _T("[Error: Can't print node.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
						}
																					// 10/30/00 AM.
						break;													// 10/30/00 AM.
					case RSARGS:	// ARRAY VALUES.						// 10/15/00 AM.
						try														// 10/27/04 AM.
							{		
							*ostr << *(rval->getArgs());					// 10/15/00 AM.
							}
						catch(...)												// 10/27/04 AM.
							{
					ruleError(line_,nlppp);									// 10/27/04 AM.
					nlppp->parse_->line_ = line_;							// 10/27/04 AM.
					std::_t_strstream gerrStr;		// 10/27/04 AM.
					gerrStr << _T("[Error: Can't output.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 10/27/04 AM.
					ok = false;													// 10/27/04 AM.
					goto done;													// 10/27/04 AM.
							}
						ok = true;
						// MEM LEAK. Now copying always, so zap.		// 12/07/00 AM.
						//rval->setArgs(0);	// SO VALS WON'T BE ZAPPED. // 10/15/00 AM.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[Output operator: Bad rhs: ")
								  << *rhs_ << _T("]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
					// 08/20/00 AM.
						ok = true;					// Tolerate.			// 10/15/00 AM.
						goto done;
					}
				// Use the ostream operator (from lhs) as returned val.
				if (ostr)
					{
						try														// 10/27/04 AM.
							{		
					*ostr << std::flush;	// Make sure output gets there.
							}
						catch(...)												// 10/27/04 AM.
							{
					ruleError(line_,nlppp);									// 10/27/04 AM.
					nlppp->parse_->line_ = line_;							// 10/27/04 AM.
					std::_t_strstream gerrStr;		// 10/27/04 AM.
					gerrStr << _T("[Error: Can't output.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);							// 10/27/04 AM.
					ok = false;													// 10/27/04 AM.
					goto done;													// 10/27/04 AM.
							}
					val = new RFASem(ostr);
					}
				}
				break;
			default:
				ruleError(line_,nlppp);										// 12/21/01 AM.
				nlppp->parse_->line_ = line_;								// 08/24/02 AM.
				std::_t_strstream gerrStr;
				gerrStr << _T("[Binary expr: bad operator.]") << std::ends;
				nlppp->parse_->errOut(&gerrStr,false);

				ok = false;
				goto done;
			}
		}
		break;								////// END OF BINARYEXPR

	////////////////////////////////
	//		ASSIGNMENT EXPRESSION
	////////////////////////////////

	case ASSIGNEXPR:
		{
		// Assume op_ is assignment).
		if (!(ok = rhs_->eval(nlppp, /*UP*/ rval)))		// FIX.	// 01/02/00 AM.
//				|| !rval)														// 09/30/00 AM.
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Bad rhs expression: ") << *rhs_				// 08/20/00 AM.
					  << _T("]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);

			goto done;
			}

		// ORDER OF EVALUATION.	MOVED THIS AFTER rhs_ EVAL.		// 08/10/02 AM.
		index=FULLARRAY;														// 10/14/00 AM.
		if (!unpackVar(lhs_,nlppp,/*UP*/var,pair,vals,index))		// 10/14/00 AM.
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Assignment: Couldn't get variable.]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);

			ok = false;
			goto done;
			}

		bool fintern = false;												// 01/09/07 AM.
		if (var->getType() != LOCALVAR)									// 01/09/07 AM.
			fintern = true;													// 01/09/07 AM.

		// Got variable.  Now match variable type against expression value.
//		last=0;

		// CLEAR OUT LHS VALS IN EVERY CASE.				// FIX.	// 09/25/00 AM.
		// To make things simple, delete the appropriate lhs		// 10/14/00 AM.
		// stuff.  Then set up the particular arg to be
		// assigned to!
		// Todo: Need better tracking of ostreams,etc.				// 10/14/00 AM.
		// (Shouldn't just delete them willy nilly here.)			// 10/14/00 AM.
		bool deleting = false;												// 10/14/00 AM.
		if (index >= 0)			// INDEXED ARRAY VALUE.				// 10/14/00 AM.
			{
			if (vals)															// 10/14/00 AM.
				darg = vals->getFirst();									// 10/14/00 AM.
			else
				{
				ruleError(line_,nlppp);										// 12/21/01 AM.
				nlppp->parse_->line_ = line_;								// 08/24/02 AM.
				std::_t_strstream gerrStr;
				gerrStr << _T("[eval: No values for variable.]") << std::ends;
				nlppp->parse_->errOut(&gerrStr,false);

				return false;
				}
			if (!darg)															// 10/14/00 AM.
				{
				arg = new Iarg(0LL);									// 10/14/00 AM.
				vals->rpush(arg);												// 10/14/00 AM.
				darg = vals->getFirst();									// 10/14/00 AM.
				}

			// See if indexed value exists.
			long ii;																// 10/14/00 AM.
			for (ii = 0; darg && ii < index;								// 10/14/00 AM.
					++ii, darg = darg->Right())							// 10/14/00 AM.
				{
//				if (darg) last = darg;										// 03/11/02 AM.
				}

			// If so, clear it out.
			if (darg)		// Indexed value exists.					// 10/14/00 AM.
				{
				arg = darg->getData();										// 10/14/00 AM.
				if (arg->getType() == IASEM)								// 10/14/00 AM.
					{
					RFASem *sema = arg->getSem();							// 10/14/00 AM.
#ifdef OLD_
					if (sema->getType() == RSOSTREAM)					// 10/14/00 AM.
						{
					// Zap the ostream here. (Not the best....)		// 10/14/00 AM.
						std::_t_ostream *ostr = sema->getOstream();				// 10/14/00 AM.
						if (ostr)												// 10/14/00 AM.
							delete ostr;										// 10/14/00 AM.
						sema->setOstream(0);									// 10/14/00 AM.
						}
#endif
					// ALL sems must be deleted.				// FIX.	// 12/22/00 AM.
					if (sema)													// 03/11/02 AM.
						delete sema;											// 12/22/00 AM.
					arg->setSem(0);											// 12/22/00 AM.
					arg->setType(IANUM);	// Convert to "nil".			// 12/22/00 AM.
					arg->setNum(0);											// 12/22/00 AM.

					}
				else			// Simply delete the value.				// 10/14/00 AM.
					{
					arg->clear();
					arg->setType(IANUM);
					arg->setNum(0);
					}
				}
			else	// Indexed value doesn't exist.						// 10/14/00 AM.
				{
				// PAD ZEROS TO INDEX.  Because we're going to
				// assign to that position down below.
				for (; ii <= index; ++ii)									// 10/14/00 AM.
					{
					arg = new Iarg(0LL);								// 10/14/00 AM.
					vals->rpush(arg);											// 10/14/00 AM.
					}
				darg = vals->getLast();										// 10/14/00 AM.
				}
			// NOTE: arg is set up to be assigned to!					// 10/14/00 AM.
			}
		else if (rval &&
					rval->getType() == RSARGS)	// Multi vals			// 10/20/00 AM.
			{
			// Delete all lhs values.
			// (Should watch ostreams, etc....).
			Dlist<Iarg>::DeleteDlistAndData(vals);						// 10/20/00 AM.
			// vals = 0;														// 12/07/00 AM.
			// pair->setVals(0);			// Documentation.				// 12/07/00 AM.

			// Too many copies.  MEMORY LEAK.							// 10/25/00 AM.
			// Copy whatever's on the rhs, whether single or multi values.
			Dlist<Iarg> *copyargs;											// 10/25/00 AM.
//			copyargs = new Dlist<Iarg>(*(rval->getArgs()));			// 10/26/00 AM.
			copyargs = Iarg::copy_args(rval->getArgs(),	// FIX.	// 12/05/00 AM.
												nlppp->parse_,fintern);		// 01/09/07 AM.
			pair->setVals(copyargs);	// Assign vals to var.		// 10/26/00 AM.
			val = rval;					// REUSE SEM.	// MEM LEAK.	// 10/26/00 AM.

			// DON'T GOTO done, or will delete rhs value.
			if (lval)
				delete lval;
			return ok;
			}
		else if (vals && (darg = vals->getFirst()))					// 03/15/00 AM.
			{
			deleting = true;													// 10/14/00 AM.
			// Idiom to delete ostream-valued arg.
			// ostream value has to be treated specially. // FIX	// 09/25/00 AM.
			arg = darg->getData();											// 09/25/00 AM.
#ifdef OLD_
			if (arg->getType() == IASEM)									// 09/25/00 AM.
				{
				RFASem *sema = arg->getSem();								// 09/25/00 AM.
				if (sema->getType() == RSOSTREAM)						// 09/25/00 AM.
					{
					std::_t_ostream *ostr = sema->getOstream();					// 09/25/00 AM.
					if (ostr)													// 09/25/00 AM.
						delete ostr;											// 09/25/00 AM.
					sema->setOstream(0);										// 09/25/00 AM.
					}
				}
#endif
			}
		else // no vals or darg.											// 10/14/00 AM.
			{
			if (!vals)
				{
				ruleError(line_,nlppp);										// 12/21/01 AM.
				nlppp->parse_->line_ = line_;								// 08/24/02 AM.
				std::_t_strstream gerrStr;
				gerrStr << _T("[eval(1): No values for variable.]") << std::ends;
				nlppp->parse_->errOut(&gerrStr,false);
				return false;
				}
			arg = new Iarg(0LL);										// 10/14/00 AM.
			vals->rpush(arg);													// 10/14/00 AM.
			darg = vals->getFirst();										// 10/14/00 AM.
			}

		// UNIFORMLY SET UP ARG TO BE ASSIGNED TO.					// 10/14/00 AM.
		// Delete all values beside the first.
		Delt<Iarg> *tmp;														// 10/14/00 AM.
		darg = darg->Right();												// 10/14/00 AM.
		if (deleting)															// 10/14/00 AM.
			{
			if (darg)															// 10/14/00 AM.
				{
				darg->Left()->setRight(0);		// Detach.				// 10/14/00 AM.
				darg->setLeft(0);					// Detach.				// 10/15/00 AM.
				vals->setLast(vals->getFirst());	// Fix list.		// 10/15/00 AM.
				}
			while ((tmp = darg))												// 10/14/00 AM.
				{
				darg = darg->Right();										// 10/14/00 AM.
				Delt<Iarg>::DeleteDeltAndData(tmp);						// 10/14/00 AM.
				}

			// MEM LEAK. IF ARG VAL IS SEM, DELETE IT, etc.			// 12/07/00 AM.
			if (arg->getType() == IASEM)									// 12/07/00 AM.
				{
				RFASem *sem;													// 12/07/00 AM.
				if ((sem = arg->getSem()))									// 12/07/00 AM.
					delete sem;													// 12/07/00 AM.
				}
			else if (arg->getType() == IASTR)	// GC.				// 06/12/06 AM.
				{
				// GC: Testing out garbage collection.					// 06/12/06 AM.
				 _TCHAR *x = arg->getStr();								// 06/12/06 AM.
				Htab *htab = nlppp->parse_->htab_;			// FIX.	// 06/16/06 AM.
	//			Htab *htab=(Htab *)VTRun_Ptr->htab_;					// 06/12/06 AM.
				htab->hdec(x);	// GC: Increment use.					// 06/12/06 AM.
				}

			// NOTE: arg is set up to assign to.
			arg->setType(IANUM);												// 10/14/00 AM.
			arg->setNum(0);													// 10/14/00 AM.
			darg = vals->getFirst(); // Only retained 1st val		// 10/15/00 AM.
			}

		if (!rval)				// Empty RHS in assignment.			// 09/30/00 AM.
			{
			val = 0;				// Nothing to pass on.					// 09/30/00 AM.
			if (lval)															// 09/30/00 AM.
				delete lval;													// 09/30/00 AM.
			return ok;															// 09/30/00 AM.
			}

		// REWORKING THE ASSIGNMENT.  ASSUME THERE'S AN ARG		// 10/14/00 AM.
		// THAT'S READY TO BE ASSIGNED TO.								// 10/14/00 AM.
		if (arg)
			{
			ok = true;															// 10/14/00 AM.
			// Variable has no value.  Just assign it a value from expr.
			switch (rval->getType())
				{
				case RSLONG:
					arg->setType(IANUM);										// 08/21/01 AM.
					arg->setNum(rval->getLong());							// 10/14/00 AM.
					break;
				case RSFLOAT:													// 08/19/01 AM.
					arg->setType(IAFLOAT);									// 08/21/01 AM.
					arg->setFloat(rval->getFloat());						// 08/19/01 AM.
					break;
				case RSNUM:	// Bastard num (was converted to str).	// 10/14/00 AM.
					{
					// Convert to num and assign.
					long long num=0;													// 12/24/02 AM.
					arg->setType(IANUM);										// 10/14/00 AM.
					if (!str_to_long(rval->getName(), num))			// 10/14/00 AM.
						{
						ok = false;												// 10/14/00 AM.
						goto done;												// 10/14/00 AM.
						}
					arg->setNum(num);											// 10/14/00 AM.
					}
					break;											// FIX.	// 11/22/00 AM.
				case RSNAME:													// 11/17/99 AM.
				case RSSTR:
					{
				   _TCHAR *x = rval->getName();							// 01/09/07 AM.
					// LOCAL STRING, TEMPORARY STRING MANAGEMENT.	// 01/08/07 AM.
					// If var not local (SXGN) then intern string.	// 01/09/07 AM.
				   if (var->getType() != LOCALVAR						// 01/09/07 AM.
						&& x && *x)												// 01/16/07 AM.
						nlppp->parse_->internStr(x,						// 01/09/07 AM.
																 /*UP*/ x);		// 01/09/07 AM.

					arg->setType(IASTR);										// 10/14/00 AM.
					arg->setStr(x);											// 10/14/00 AM.
					Htab *htab = nlppp->parse_->htab_;		// FIX.	// 06/16/06 AM.
	//				Htab *htab=(Htab *)VTRun_Ptr->htab_;				// 06/12/06 AM.
					htab->hinc(x);	// GC: Increment use.				// 04/11/06 AM.
					}
					break;
				case RS_KBCONCEPT:											// 02/22/00 AM.
					arg->setType(IASEM);										// 10/14/00 AM.
					CONCEPT *conc;												// 11/15/00 AM.
					if ((conc = rval->getKBconcept()))						// 11/15/00 AM.
						val = new RFASem(rval->getKBconcept(),RS_KBCONCEPT,NULL);
					else
						val = 0;													// 11/15/00 AM.
					arg->setSem(val);											// 10/14/00 AM.
					break;
				case RS_KBPHRASE:												// 02/22/00 AM.
					arg->setType(IASEM);										// 10/14/00 AM.
					val = new RFASem(rval->getKBphrase(), RS_KBPHRASE);
					arg->setSem(val);											// 10/14/00 AM.
					break;
				case RS_KBATTR:												// 02/22/00 AM.
					arg->setType(IASEM);										// 10/14/00 AM.
					val = new RFASem(rval->getKBattr(), RS_KBATTR);
					arg->setSem(val);											// 10/14/00 AM.
					break;
				case RS_KBVAL:													// 02/22/00 AM.
					arg->setType(IASEM);										// 10/14/00 AM.
					val = new RFASem(rval->getKBval(), RS_KBVAL);
					arg->setSem(val);											// 10/14/00 AM.
					break;
				case RSOSTREAM:												// 09/23/00 AM.
					arg->setType(IASEM);										// 10/14/00 AM.
					val = new RFASem(rval->getOstream());				// 09/23/00 AM.
					arg->setSem(val);											// 10/14/00 AM.
					break;														// 09/23/00 AM.
				case RSARGS:	// ARRAY VALUES (uncopied).			// 10/15/00 AM.
					// Should not be here.									// 12/07/00 AM.
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					{
					std::_t_strstream gerrStr;
					gerrStr << _T("[Copying array -- Error.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);
					}
					//rval->setArgs(0);	// So won't be deleted.		// 10/15/00 AM.
					break;
				case RSNODE:													// 10/18/00 AM.
					arg->setType(IASEM);										// 10/18/00 AM.
					val = new RFASem(rval->getNode());					// 10/18/00 AM.
					arg->setSem(val);											// 10/18/00 AM.
					break;
				default:
					ruleError(line_,nlppp);	// Id current rule line.// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					std::_t_strstream gerrStr;
					gerrStr << _T("[Bad rhs type in assignment.]")		// 09/23/00 AM.
							  << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);
					ok = false;
					break;
				}
			// Assignment expression needs to return a value also.
			val = rval;		// Value of assignment is the right hand side.
			// DON'T GOTO done, or will delete rhs value.
			if (lval)
				delete lval;
			return ok;
			}

		// Assignment expression needs to return a value also.
		val = rval;		// Value of assignment is the right hand side.
		// DON'T GOTO done, or will delete rhs value.
		if (lval)
			delete lval;
		return false;		// FAILURE.
		}

	////////////////////////////////
	//		UNARY EXPRESSION
	////////////////////////////////

	case UNARYEXPR:
		// Note: rval need not be computed for INCOP, etc.		// 05/07/00 AM.
		if (!(ok = rhs_->eval(nlppp, /*UP*/ rval)))			 // FIX 08/21/00 AM.
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Empty rhs expression: ") << *rhs_			// 08/20/00 AM.
					<< _T("]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);

			goto done;
			}
		else if (!rval)														// 08/21/00 AM.
			{
			if (op_->getType() == NOTOP)					// BUG FIX.	// 11/10/01 AM.
				val = new RFASem(1LL);					// BUG FIX.	// 11/10/01 AM.
			else													// BUG FIX.	// 11/10/01 AM.
				val = new RFASem(0LL);								// 08/21/00 AM.
			goto done;															// 08/21/00 AM.
			}
		switch (op_->getType())
			{
			case INCOP:
				{
				long long num=0;														// 12/24/02 AM.
				if (!inc_interp(rhs_, nlppp, /*UP*/ num))				// 06/13/00 AM.
					{
					ok = false;
					goto done;
					}
				val = new RFASem(num);			// Return incremented value.
				goto done;
				}
			case DECOP:
				{
				long long num=0;														// 12/24/02 AM.
				// Return decremented num.  Create variable with init
				// value zero, if not found.								// 06/01/00 AM.
				if (!dec_interp(rhs_, nlppp, /*UP*/ num))				// 06/13/00 AM.
					{
					ok = false;
					goto done;
					}
				val = new RFASem(num);			// Return decremented value.
				goto done;
				}
			case NOTOP:
				{
				long long num=0;														// 12/24/02 AM.
				// Get rhs value.
				// Attempt to do a "not".
				// Return the not.
				switch (rval->getType())
					{
					case RSLONG:
						num = rval->getLong();
						break;
					case RSFLOAT:												// 08/22/01 AM.
						num = (rval->getFloat() ? 1 : 0);				// 08/22/01 AM.
						break;
					case RSNUM:
						if (!str_to_long(rval->getName(), num))
							{
							ok = false;
							goto done;
							}
						break;
					case RSSTR:
					case RSNAME:				// 11/17/99 AM.
						{
						// Treat empty string as false, all else as true.
						_TCHAR *str = rval->getName();						// 12/24/02 AM.
						if (!str || !*str)
							num = 0;		// Boolean false.
						else
							num = 1;		// Boolean true.
						}
						break;
					case RS_KBCONCEPT:	// FIX	// 03/15/00 AM.
						num = ((rval->getKBconcept()) ? 1 : 0);		// 10/30/00 AM.
						break;
					case RS_KBPHRASE:		// FIX	// 03/15/00 AM.
						num = ((rval->getKBphrase()) ? 1 : 0);			// 10/30/00 AM.
						break;
					case RS_KBATTR:		// FIX	// 03/15/00 AM.
						num = ((rval->getKBattr()) ? 1 : 0);			// 10/30/00 AM.
						break;
					case RS_KBVAL:			// FIX	// 03/15/00 AM.
						num = ((rval->getKBval()) ? 1 : 0);				// 10/30/00 AM.
						break;
					case RSNODE:												// 10/30/00 AM.
						num = ((rval->getNode()) ? 1 : 0);				// 10/30/00 AM.
						break;													// 10/30/00 AM.
					case RSARGS:	// Array.								// 12/04/01 AM.
						num = 1;													// 12/04/01 AM.
						break;
					case RSOSTREAM:				// Preemptive.			// 05/23/01 AM.
																		// FIX.	// 04/11/03 AM.
						num = ((rval && rval->getOstream()) ? 1 : 0);// 04/11/03 AM.
						break;													// 05/23/01 AM.

					default:
						ok = false;
						goto done;
					}
				num = !num;
				val = new RFASem(num);			// New value to return.
				goto done;
				}
			case PLUSOP:
				// This is a noop.  (Could do some conversions to numeric.)
				val = rval;
				return ok;
			case MINUSOP:
				{
				long long num=0;														// 12/24/02 AM.
				// Get rhs value.
				// Attempt to negate.
				// Return the negation.
				switch (rval->getType())
					{
					case RSLONG:
						num = rval->getLong();
						break;
					case RSFLOAT:												// 08/22/01 AM.
						val = new RFASem(-(rval->getFloat()));			// 08/22/01 AM.
						goto done;												// 08/22/01 AM.
					case RSSTR:
					case RSNUM:
					case RSNAME:			// 11/17/99 AM.
						if (!str_to_long(rval->getName(), num))
							{
							ok = false;
							goto done;
							}
						break;
					default:
						ok = false;
						goto done;
					}
				num = -num;							// NEGATE.
				val = new RFASem(num);			// New value to return.
				goto done;
				}
			default:
				ok = false;
				goto done;
			}
		break;

	////////////////////////////////
	//		POSTUNARY EXPRESSION
	////////////////////////////////

	case POSTUNARYEXPR:
		if (!(ok = rhs_->eval(nlppp, /*UP*/ rval)) || !rval) // FIX 01/02/00 AM.
			{
			ruleError(line_,nlppp);											// 12/21/01 AM.
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			std::_t_strstream gerrStr;
			gerrStr << _T("[Empty rhs expression: ") << *rhs_			// 08/20/00 AM.
					  << _T("]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);

			goto done;
			}
		switch (postop_->getType())
			{
			case INCOP:
				{
				long long num=0;														// 12/24/02 AM.
				// Rhs must be a variable.
				// Get the variable and its value.
				if (!unpackVar(rhs_, nlppp, /*UP*/ var, pair, vals,index))
					{
					ok = false;
					goto done;
					}
				Delt<Iarg> *darg;
				Iarg *arg;
				if (!vals || !(darg = vals->getFirst()))
					{
					// Variable has no value.  Set to 1 and done.
					val = new RFASem(0LL);		// RETURN UNINCREMENTED VALUE.
					Var::setVal(pair, 1LL);			// Install 1 in variable.
					goto done;
					}
				if (darg->Right())
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					std::_t_strstream gerrStr;
					gerrStr << _T("[Can't increment multi-valued variable.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);

					ok = false;
					goto done;
					}
				// Increment and store value.
				arg = darg->getData();
				// Depending on the argument type....
				switch (arg->getType())
					{
					case IANUM:
						num = arg->getNum();		// FETCH VALUE.
						arg->setNum(num+1);		// STORE INCREMENTED VALUE.
						break;
					case IASTR:
						if (!str_to_long(arg->getStr(), num))	// Convert, fetch.
							{
							ok = false;
							goto done;
							}
						arg->setType(IANUM);					// Convert value type.
						arg->setNum(num+1);		// Store incremented number.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[inc: Error. Bad type.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
																					// 11/22/00 AM.
						ok = false;
						goto done;
					}
				val = new RFASem(num);			// Return incremented value.
				goto done;
				}
			case DECOP:
				{
				long long num=0;														// 12/24/02 AM.
				// Rhs must be a variable.
				// Get the variable and its value.
				if (!unpackVar(rhs_, nlppp, /*UP*/ var, pair, vals,index))
					{
					ok = false;
					goto done;
					}
				if (!vals || !(darg = vals->getFirst()))
					{
					// Variable has no value.  Set to -1 and done.
					val = new RFASem(0LL);		// RETURN UNDECREMENTED VALUE.
					Var::setVal(pair, -1LL);			// Install -1 in variable.
					goto done;
					}
				if (darg->Right())
					{
					ruleError(line_,nlppp);									// 12/21/01 AM.
					nlppp->parse_->line_ = line_;							// 08/24/02 AM.
					std::_t_strstream gerrStr;
					gerrStr << _T("[Can't increment multi-valued variable.]") << std::ends;
					nlppp->parse_->errOut(&gerrStr,false);

					ok = false;
					goto done;
					}
				// Increment and store value.
				arg = darg->getData();
				// Depending on the argument type....
				switch (arg->getType())
					{
					case IANUM:
						num = arg->getNum();	// FETCH VALUE.
						arg->setNum(num-1);		// STORE DECREMENTED VALUE.
						break;
					case IASTR:
						if (!str_to_long(arg->getStr(), num))	// Convert, fetch.
							{
							ok = false;
							goto done;
							}
						arg->setType(IANUM);					// Convert value type.
						arg->setNum(num-1);		// Store decremented number.
						break;
					default:
						ruleError(line_,nlppp);								// 12/21/01 AM.
						nlppp->parse_->line_ = line_;						// 08/24/02 AM.
						std::_t_strstream gerrStr;
						gerrStr << _T("[dec: Error. Bad type.]") << std::ends;
						nlppp->parse_->errOut(&gerrStr,false);
																					// 11/22/00 AM.
						ok = false;
						goto done;
					}
				val = new RFASem(num);			// Return decremented value.
				goto done;
				}
			default:
				ok = false;
				goto done;
			}		break;
	default:
		ruleError(line_,nlppp);												// 12/21/01 AM.
		nlppp->parse_->line_ = line_;										// 08/24/02 AM.
		std::_t_strstream gerrStr;
		gerrStr << _T("[Iexpr::eval: Bad expression type.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
	}			//////////// END OF THE BIG SWITCH.

done:
	if (!ok || !val)
		{
		ruleError(line_,nlppp);												// 12/21/01 AM.
		nlppp->parse_->line_ = line_;										// 08/24/02 AM.
		std::_t_strstream gerrStr;
		gerrStr << _T("[Expression evaluation error.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

	if (rval)
		delete rval;
	if (lval)
		delete lval;
return ok;
}


/********************************************
* FN:		EVAL_LHS_ONLY
* CR:		12/24/02 AM.
* SUBJ:	Handle evaluation of lhs only.
* RET:	True if lhs-only evaluation case, else false.
*			UP - val, ok.
* NOTE:	To support C/C++ style evaluation of things like
*			(X && Y) (X || Y)
*			If X is 0 with && case, Y won't be evaled.
*			If X nonzero and ||, Y similarly won't be evaled.
* OPT:	Making this INLINE.
*********************************************/

inline bool Iexpr::eval_lhs_only(
	RFASem *lval,
	/*UP*/
	RFASem* &val,
	bool &ok)
{
//ok = true;
switch (op_->getType())
	{
	case ANDOP:
		if (!lval)
			{
			val = new RFASem(0LL);
			return true;
			}
		break;
	case OROP:
		if (!lval)
			return false;	// Must eval rhs.
		break;
	default:		// Not ANDOP, OROP.  Must eval rhs.
		return false;
	}

// Compute whether lval is zero or nonzero.
// Can use this in any case for calcs on rhs.
bool nonzero=false;
switch (lval->getType())
	{
	case RSLONG:
		nonzero = (lval->getLong() ? true : false);
		break;
	case RSFLOAT:
		nonzero = (lval->getFloat() ? true : false);
		break;
	case RSNUM:
		{
		long long num1=0;
		if (!str_to_long(lval->getNum(), num1))
			{
			ok = false;
			return false;
			}
		nonzero = (num1 ? true : false);
		}
		break;
	case RSSTR:
	case RSNAME:
		{
		_TCHAR *str = lval->getName();
		nonzero = (str && *str);
		}
		break;
	case RS_KBCONCEPT:
		nonzero = (lval->getKBconcept() ? true : false);
		break;
	case RS_KBPHRASE:
		nonzero = (lval->getKBphrase() ? true : false);
		break;
	case RS_KBATTR:
		nonzero = (lval->getKBattr() ? true : false);
		break;
	case RS_KBVAL:
		nonzero = (lval->getKBval() ? true : false);
		break;
	case RSNODE:
		nonzero = (lval->getNode() ? true : false);
		break;
	case RSOSTREAM:
		nonzero = (lval->getOstream() ? true : false);
		break;
	case RSARGS:
		// Recurse??
		nonzero = true;
		break;
	default:
		ok = false;
		return false;
	}

switch (op_->getType())
	{
	case ANDOP:
		{
		if (nonzero)
			return false;
		else
			{
			val = new RFASem(0LL);
			return true;
			}
		}
	case OROP:
		if (nonzero)
			{
			val = new RFASem(1LL);
			return true;
			}
		else
			return false;
	default:
		ok = false;
		return false;
	}
}


/********************************************
* FN:		UNPACKVAR
* CR:		11/15/99 AM.
* SUBJ:	Get data for a variable.  If doesn't exist, make it.
* RET:	True if ok, else false.
* NOTE:	Maybe should go in RFASem or Ivar.
*********************************************/

bool Iexpr::unpackVar(
	RFASem *semvar,
	Nlppp *nlppp,
	/*UP*/
	Ivar* &var,
	Ipair* &pairx,
	Dlist<Iarg>* &vals,
	long &index																	// 10/14/00 AM.
	)
{
var = 0;
pairx = 0;
vals = 0;
index=FULLARRAY;																// 10/14/00 AM.
if (!semvar)
	return false;
if (semvar->getType() != RSVAR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var needed.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
if (!(var = semvar->getVar()))
	return false;

// -1==FULLARRAY, the unindexed variable (old-style).				// 10/14/00 AM.
// 0-infinity == INDEXED VARIABLE == NLP++ ARRAY!					// 10/14/00 AM.
RFASem *sem = var->getIndex();											// 10/14/00 AM.

// Evaluate the index dynamically!										// 10/14/00 AM.
_TCHAR *str=0;																	// 12/07/00 AM.
if (sem)																			// 10/14/00 AM.
	{
	RFASem *semval;															// 10/14/00 AM.
	sem->eval(nlppp,/*UP*/semval);										// 10/14/00 AM.
	if (!semval)																// 12/07/00 AM.
		return false;															// 12/07/00 AM.
	switch (semval->getType())												// 10/14/00 AM.
		{
		case RSLONG:															// 10/14/00 AM.
			index = semval->getLong();										// 10/14/00 AM.
			delete semval; semval = 0;					// MEM LEAK.	// 12/07/00 AM.
			break;
		case RSNUM:
			str = semval->getName();										// 12/07/00 AM.
			delete semval; semval = 0;					// MEM LEAK.	// 12/07/00 AM.
			if (!str_to_long(str, index))				// 10/14/00 AM.
				return false;
			break;
		default:
			delete semval; semval = 0;					// MEM LEAK.	// 12/07/00 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[unpackVar: Error in index.]") << std::ends;
			errOut(&gerrStr,false);
			}
		// 10/1400 AM.
			return false;
		}
	}

if (!var->get(nlppp, /*UP*/ pairx))
	return false;
if (!pairx)													// 12/03/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Couldn't fetch var.]") << std::ends;
	errOut(&gerrStr,false);

		// 12/03/99 AM.
	return false;											// 12/03/99 AM.
	}
vals = pairx->getVals();
return true;
}


/********************************************
* FN:		GENEVAL
* CR:		05/07/00 AM.
* SUBJ:	Gen evaluation of an NLP++ expression.
*********************************************/

bool Iexpr::genEval(Gen *gen,
	bool top						// If expr is at top or stmt level.	// 06/06/00 AM.
	)
{
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
if (top) *fcode << gen->indent_ << _T("Arun::stmt(");


if (!rhs_)
	return false;	// Every expr type has a right-hand-side.

Ivar *var;
enum Ivartype typ;
RFASem *index;																	// 01/02/01 AM.
switch (type_)		// Type of expression.
	{
	case BINARYEXPR:
		if (!lhs_ || !op_)
			return false;
		// Generating as an inline function.  Let C++ take care of types
		// via overloaded function.
		*fcode << _T("Arun::");
		switch (op_->getType())
			{
			case PLUSOP:	*fcode << _T("plus");		break;
			case MINUSOP:	*fcode << _T("minus");	break;
			case TIMESOP:	*fcode << _T("times");	break;
			case DIVIDEOP:	*fcode << _T("divide");	break;
			case REMOP:		*fcode << _T("rem");		break;
			case CONFOP:	*fcode << _T("conf");		break;
			case EQOP:		*fcode << _T("eq");		break;
			case NEOP:		*fcode << _T("ne");		break;
			case GTOP:		*fcode << _T("gt");		break;
			case LTOP:		*fcode << _T("lt");		break;
			case GEOP:		*fcode << _T("ge");		break;
			case LEOP:		*fcode << _T("le");		break;
			case ANDOP:		*fcode << _T("vtand");		break;	// LINUX compatibility.	// 11/26/06 AM.
			case OROP:		*fcode << _T("vtor");		break;	// LINUX compatibility.	// 11/26/06 AM.
			case OUTOP:		*fcode << _T("out");
				break;
			default:	
				*fcode << _T("\nERROR();") << std::endl;							// 05/04/01 AM.
				*fcode << _T("// [Iexpr: Bad operator]") << std::endl;		// 05/04/01 AM.
				return false;
			}
		*fcode << _T("(");
		if (!lhs_->genEval(gen))
			return false;
		*fcode << _T(", ");
		if (!rhs_->genEval(gen))
			return false;

		// For interning strings & for output.							// 05/26/00 AM.
		if (op_->getType() == PLUSOP ||									// 05/26/00 AM.
			 op_->getType() == OUTOP)										// 05/26/00 AM.
			*fcode << _T(",nlppp");												// 05/26/00 AM.
		*fcode << _T(")");
		break;
	case ASSIGNEXPR:
		if (!lhs_ || !op_)
			return false;

		// Unpack the variable.
		if (lhs_->getType() != RSVAR)
			return false;
		if (!(var = lhs_->getVar()))
			return false;

		typ = var->getType();												// 05/26/00 AM.
		index = var->getIndex();											// 01/02/01 AM.
		if (typ == NODEVAR)													// 05/26/00 AM.
			typ = RUNNODEVAR;													// 05/26/00 AM.
		*fcode << _T("Arun::assign(")
				 << typ << _T(", ")
				 << _T("_T(\"") << var->getName() << _T("\"), ")
				 << var->getNum() << _T(", ");
		if (index)													// FIX.	// 05/04/01 AM.
			index->genEval(gen);									// FIX.	// 05/04/01 AM.
		else															// FIX.	// 05/04/01 AM.
			*fcode << FULLARRAY;									// FIX.	// 05/04/01 AM.
		*fcode														// FIX.	// 05/04/01 AM.
				 << _T(", nlppp, ")
				 << _T("(");
		if (!rhs_->genEval(gen))
			return false;
		*fcode << _T("))");
		break;
	case UNARYEXPR:
		if (!op_)
			return false;
		switch (op_->getType())
			{
			case INCOP:
				// Unpack the variable.
				if (rhs_->getType() != RSVAR)
					return false;
				if (!(var = rhs_->getVar()))
					return false;

				typ = var->getType();					// FIX.			// 06/08/00 AM.
				index = var->getIndex();									// 01/09/01 AM.
				if (typ == NODEVAR)						// FIX.			// 06/08/00 AM.
					typ = RUNNODEVAR;						// FIX.			// 06/08/00 AM.
				*fcode << _T("Arun::inc(")
						 << typ << _T(", ")
						 << _T("_T(\"") << var->getName() << _T("\"), ")
						 << var->getNum() << _T(", ");
				if (index)											// FIX.	// 05/04/01 AM.
					index->genEval(gen);							// FIX.	// 05/04/01 AM.
				else													// FIX.	// 05/04/01 AM.
					*fcode << FULLARRAY;							// FIX.	// 05/04/01 AM.
				*fcode												// FIX.	// 05/04/01 AM.
				 << _T(", ")															// 01/09/01 AM.
						 << _T("nlppp")
						 << _T(")");
				break;
			case DECOP:
				// Unpack the variable.
				if (rhs_->getType() != RSVAR)
					return false;
				if (!(var = rhs_->getVar()))
					return false;

				typ = var->getType();					// FIX.			// 06/08/00 AM.
				index = var->getIndex();									// 01/09/01 AM.
				if (typ == NODEVAR)						// FIX.			// 06/08/00 AM.
					typ = RUNNODEVAR;						// FIX.			// 06/08/00 AM.
				*fcode << _T("Arun::dec(")
						 << typ << _T(", ")
						 << _T("_T(\"") << var->getName() << _T("\"), ")
						 << var->getNum() << _T(", ");
				if (index)											// FIX.	// 05/04/01 AM.
					index->genEval(gen);							// FIX.	// 05/04/01 AM.
				else													// FIX.	// 05/04/01 AM.
					*fcode << FULLARRAY;							// FIX.	// 05/04/01 AM.
				*fcode												// FIX.	// 05/04/01 AM.
				 << _T(", ")															// 01/09/01 AM.
						 << _T("nlppp")
						 << _T(")");
				break;
			case NOTOP:
				*fcode << _T("Arun::vtnot")	// LINUX compatibility.	// 11/26/06 AM.
						 << _T("(");
				if (!rhs_->genEval(gen))
					return false;
				*fcode << _T(")");
				break;
			case PLUSOP:
				*fcode << _T("Arun::plus")
						 << _T("(");
				if (!rhs_->genEval(gen))
					return false;
				*fcode << _T(")");
				break;
			case MINUSOP:
				*fcode << _T("Arun::minus")
						 << _T("(");
				if (!rhs_->genEval(gen))
					return false;
				*fcode << _T(")");
				break;
			default:
				return false;
			}
		break;
	case POSTUNARYEXPR:
		if (!postop_)
			return false;
		*fcode << _T("Arun::");
		switch (postop_->getType())
			{
			case INCOP:
				// Unpack the variable.
				if (rhs_->getType() != RSVAR)
					return false;
				if (!(var = rhs_->getVar()))
					return false;

				typ = var->getType();					// FIX.			// 06/08/00 AM.
				index = var->getIndex();									// 01/09/01 AM.
				if (typ == NODEVAR)						// FIX.			// 06/08/00 AM.
					typ = RUNNODEVAR;						// FIX.			// 06/08/00 AM.
				*fcode << _T("postinc(")
						 << typ << _T(", ")
						 << _T("_T(\"") << var->getName() << _T("\"), ")
						 << var->getNum() << _T(", ");
				if (index)											// FIX.	// 05/04/01 AM.
					index->genEval(gen);							// FIX.	// 05/04/01 AM.
				else													// FIX.	// 05/04/01 AM.
					*fcode << FULLARRAY;							// FIX.	// 05/04/01 AM.
				*fcode												// FIX.	// 05/04/01 AM.
				 << _T(", ")															// 01/09/01 AM.
						 << _T("nlppp")
						 << _T(")");
				break;
			case DECOP:
				// Unpack the variable.
				if (rhs_->getType() != RSVAR)
					return false;
				if (!(var = rhs_->getVar()))
					return false;

				typ = var->getType();					// FIX.			// 06/08/00 AM.
				index = var->getIndex();									// 01/09/01 AM.
				if (typ == NODEVAR)						// FIX.			// 06/08/00 AM.
					typ = RUNNODEVAR;						// FIX.			// 06/08/00 AM.
				*fcode << _T("postdec(")
						 << typ << _T(", ")
						 << _T("_T(\"") << var->getName() << _T("\"), ")
						 << var->getNum() << _T(", ");
				if (index)											// FIX.	// 05/04/01 AM.
					index->genEval(gen);							// FIX.	// 05/04/01 AM.
				else													// FIX.	// 05/04/01 AM.
					*fcode << FULLARRAY;							// FIX.	// 05/04/01 AM.
				*fcode												// FIX.	// 05/04/01 AM.
				 << _T(", ")															// 01/09/01 AM.
						 << _T("nlppp")
						 << _T(")");
				break;
			default:
				*fcode << _T("ERROR();") << std::endl;								// 05/04/01 AM.
				*fcode << _T("// [postunaryexpr: Bad op.]") << std::endl;	// 05/04/01 AM.
				return false;
			}
		break;
	default:
		*fcode << _T("// ERROR: Bad expression type.") << std::endl;
		return false;
	}
if (top) *fcode << _T(")");			// Close out stmt().
return true;
}


/********************************************
* FN:		INC
* CR:		06/01/00 AM.
* SUBJ:	Increment variable.
* RET:	True if ok, else false.
*			UP - the incremented value of variable.
* NOTE:	Supporting compiled runtime also.
*			If var doesn't exist, make it with init value zero.
*********************************************/

bool Iexpr::inc(RFASem *sem, Nlppp *nlppp, /*UP*/ long long &num)
{
bool ok = inc_interp(sem,nlppp,/*UP*/ num);
delete sem;
return ok;
}


/********************************************
* FN:		INC_INTERP
* CR:		06/13/00 AM.
* SUBJ:	Increment variable.
* RET:	True if ok, else false.
*			UP - the incremented value of variable.
* NOTE:	Supporting compiled runtime also.
*			If var doesn't exist, make it with init value zero.
*			Interpreted version doesn't free given SEM. Compiled does.
*********************************************/

inline bool Iexpr::inc_interp(RFASem *sem, Nlppp *nlppp, /*UP*/ long long &num)
{
num = -1;
// Sem must be a variable.
// Get the variable and its value.
Ivar *var;
Ipair *pair;
Dlist<Iarg> *vals;
long index=FULLARRAY;
if (!unpackVar(sem, nlppp, /*UP*/ var, pair, vals,index))
	{
//	delete sem;												// MEM LEAK.	// 06/13/00 AM.
	return false;
	}

Delt<Iarg> *darg=0;															// 07/15/03 AM.
Iarg *arg;
if (vals)																		// 07/15/03 AM.
	darg = vals->getFirst();												// 07/15/03 AM.

if (!darg && index <= 0)										// FIX.	// 07/15/03 AM.
	{
	// Variable has no value.  Set to 1 and done.
	num = 1;
	Var::setVal(pair, 1LL);	// Install 1 in variable.
//	delete sem;												// MEM LEAK.	// 06/13/00 AM.
	return true;
	}

if (index >= 0)	// INDEXED ARRAY VALUE.								// 10/15/00 AM.
	{
	// See if indexed value exists.
	long ii;																		// 10/15/00 AM.
	for (ii = 0; darg && ii < index;										// 10/15/00 AM.
			++ii, darg = darg->Right())									// 10/15/00 AM.
		;

	if (!darg)	// Indexed value doesn't exist.						// 10/15/00 AM.
		{
		// PAD ZEROS TO INDEX.  Because we're going to
		// assign to that position down below.
		for (; ii <= index; ++ii)											// 10/15/00 AM.
			{
			arg = new Iarg(0LL);										// 10/15/00 AM.
			vals->rpush(arg);													// 10/15/00 AM.
			}
		darg = vals->getLast();												// 10/15/00 AM.
		}
	}
else if (darg->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't increment multi-valued variable.]") << std::ends;
	errOut(&gerrStr,false);
//	delete sem;												// MEM LEAK.	// 06/13/00 AM.
	return false;
	}

// Increment and store value.
arg = darg->getData();
// Depending on the argument type....
switch (arg->getType())
	{
	case IANUM:
		num = arg->getNum();	// FETCH VALUE.
		++num;					// INCREMENT VALUE.
		arg->setNum(num);		// STORE VALUE.
		break;
	case IASTR:
		if (!str_to_long(arg->getStr(), num))	// Convert, fetch.
			{
//			delete sem;										// MEM LEAK.	// 06/13/00 AM.
			return false;
			}
		++num;									// Increment.
		arg->setType(IANUM);					// Convert value type.
		arg->setNum(num);						// Store number.
		break;
	default:
//		delete sem;											// MEM LEAK.	// 06/13/00 AM.
		return false;
	}
//delete sem;													// MEM LEAK.	// 06/13/00 AM.
return true;
}


/********************************************
* FN:		DEC
* CR:		06/01/00 AM.
* SUBJ:	Decrement variable.
* RET:	True if ok, else false.
*			UP - the decremented value of variable.
* NOTE:	Supporting compiled runtime also.
*			If var doesn't exist, make it with init value zero.
*********************************************/

bool Iexpr::dec(RFASem *sem, Nlppp *nlppp, /*UP*/ long long &num)
{
bool ok = dec_interp(sem,nlppp,/*UP*/num);
delete sem;
return ok;
}

/********************************************
* FN:		DEC_INTERP
* CR:		06/13/00 AM.
* SUBJ:	Decrement variable.
* RET:	True if ok, else false.
*			UP - the decremented value of variable.
* NOTE:	Supporting compiled runtime also.
*			If var doesn't exist, make it with init value zero.
*			Interpreted version doesn't free given SEM. Compiled does.
*********************************************/

inline bool Iexpr::dec_interp(RFASem *sem, Nlppp *nlppp, /*UP*/ long long &num)
{
num = -1;
Ivar *var;
Ipair *pair;
Dlist<Iarg> *vals;
long index=FULLARRAY;
if (!unpackVar(sem, nlppp, /*UP*/ var, pair, vals,index))
	{
//	delete sem;												// MEM LEAK.	// 06/13/00 AM.
	return false;
	}

Delt<Iarg> *darg=0;															// 07/15/03 AM.
Iarg *arg;
if (vals)																		// 07/15/03 AM.
	darg = vals->getFirst();												// 07/15/03 AM.1LL

if (!darg && index <= 0)										// FIX.	// 07/15/03 AM.
	{
	// Variable has no value.  Set to -1 and done.
	num = -1;
	Var::setVal(pair, -1LL);	// Install -1 in variable.
//	delete sem;												// MEM LEAK.	// 06/13/00 AM.
	return true;
	}

if (index >= 0)	// INDEXED ARRAY VALUE.								// 07/15/03 AM.
	{
	// See if indexed value exists.
	long ii;																		// 07/15/03 AM.
	for (ii = 0; darg && ii < index;										// 07/15/03 AM.
			++ii, darg = darg->Right())									// 07/15/03 AM.
		;

	if (!darg)	// Indexed value doesn't exist.						// 07/15/03 AM.
		{
		// PAD ZEROS TO INDEX.  Because we're going to
		// assign to that position down below.
		for (; ii <= index; ++ii)											// 07/15/03 AM.
			{
			arg = new Iarg(0LL);										// 07/15/03 AM.
			vals->rpush(arg);													// 07/15/03 AM.
			}
		darg = vals->getLast();												// 07/15/03 AM.
		}
	}

else if (darg->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't increment multi-valued variable.]") << std::ends;
	errOut(&gerrStr,false);
//	delete sem;												// MEM LEAK.	// 06/13/00 AM.
	return false;
	}
// Increment and store value.
arg = darg->getData();
// Depending on the argument type....
switch (arg->getType())
	{
	case IANUM:
		num = arg->getNum();	// FETCH VALUE.
		--num;					// DECREMENT VALUE.
		arg->setNum(num);		// STORE VALUE.
		break;
	case IASTR:
		if (!str_to_long(arg->getStr(), num))	// Convert, fetch.
			return false;
		--num;									// Decrement.
		arg->setType(IANUM);					// Convert value type.
		arg->setNum(num);						// Store number.
		break;
	default:
//		delete sem;											// MEM LEAK.	// 06/13/00 AM.
		return false;
	}
//delete sem;													// MEM LEAK.	// 06/13/00 AM.
return true;
}


/********************************************
* FN:		NOT
* CR:		06/01/00 AM.
* SUBJ:	NOT operator.
* RET:	True if ok, else false.
*			UP - the notted value of expr.
* NOTE:	Supporting compiled runtime also.
*********************************************/

bool Iexpr::vtnot(RFASem *sem, Nlppp *nlppp, /*UP*/ long long &num)
{
num = 1;
_TCHAR *str;

if (!sem)															// FIX.	// 06/23/02 AM.
	return true;													// FIX.	// 06/23/02 AM.

// Get rhs value.
// Attempt to do a "not".
// Return the not.
switch (sem->getType())
	{
	case RSLONG:
		num = sem->getLong();
		break;
	case RSFLOAT:																// 08/19/01 AM.
		num = (sem->getFloat() ? 1 : 0);									// 08/19/01 AM.
		break;
	case RSNUM:
		if (!str_to_long(sem->getName(), num))
			{
			delete sem;										// MEM LEAK.	// 06/13/00 AM.
			return false;
			}
		break;
	case RSSTR:
	case RSNAME:				// 11/17/99 AM.
		// Treat empty string as false, all else as true.
		str = sem->getName();
		if (!str || !*str)
			num = 0;		// Boolean false.
		else
			num = 1;		// Boolean true.
		break;
	case RS_KBCONCEPT:											// FIX	// 03/15/00 AM.
		num = ((sem->getKBconcept()) ? 1 : 0);							// 10/30/00 AM.
		break;
	case RS_KBPHRASE:		// FIX	// 03/15/00 AM.
		num = ((sem->getKBphrase()) ? 1 : 0);							// 10/30/00 AM.
		break;
	case RS_KBATTR:		// FIX	// 03/15/00 AM.
		num = ((sem->getKBattr()) ? 1 : 0);								// 10/30/00 AM.
		break;
	case RS_KBVAL:			// FIX	// 03/15/00 AM.
		num = ((sem->getKBval()) ? 1 : 0);								// 10/30/00 AM.
		break;
	case RSNODE:																// 10/30/00 AM.
		num = ((sem->getNode()) ? 1 : 0);								// 10/30/00 AM.
		break;																	// 10/30/00 AM.
	case RSOSTREAM:								// Preemptive.			// 05/23/01 AM.
		num = (sem->getOstream() ? 1 : 0);								// 05/23/01 AM.
		break;																	// 05/23/01 AM.
	case RSARGS:
		num = (sem->getArgs() ? 1 : 0);									// 12/04/01 AM.
		break;
	default:
		return false;
	}
num = !num;
delete sem;													// MEM LEAK.	// 06/13/00 AM.
return true;
}


/********************************************
* FN:		RULEERROR
* CR:		08/19/01 AM.
* SUBJ:	Report pass and line for rule where an error occurred.
* NOTE:	More general than just iexpr.
*********************************************/

void Iexpr::ruleError(long line, Nlppp *nlppp)
{
switch (nlppp->region_)
	{
	case REGCOND:
	case REGPRE:
	case REGCHECK:
	case REGPOST:
//	case REGDECL:																// 03/12/02 AM.
		break;
	default:
		return;	// Not in a rule context.
	}
//if (Linenum > line)	// At rule's line. (Not a great test.)	// 12/21/01 AM.

std::_t_strstream gerrStr;
gerrStr << _T("[Error executing code for current rule.]")
		  << std::ends;
errOut(&gerrStr,false,
		nlppp->parse_->currpass_,											// 08/24/02 AM.
		nlppp->ruleLine());
}


/********************************************
* FN:		GETVARINDEX
* CR:		03/11/02 AM.
* SUBJ:	Get var's index position.
* NOTE:	Utility function.
*			Could plug this into Iexpr::eval etc.
********************************************/

Iarg *Iexpr::getVarindex(
	Dlist<Iarg> *vals,
	long index
	)
{
Iarg *arg = NULL;
Delt<Iarg> *darg=vals->getFirst();
if (!darg)
	{
	arg = new Iarg(0LL);
	vals->rpush(arg);
	darg = vals->getFirst();
	}

// See if indexed value exists.
long ii;
for (ii = 0; darg && ii < index; ++ii, darg = darg->Right())
	;

if (darg)	// If index value exists, clear it out.
	{
	arg = darg->getData();
	if (arg->getType() == IASEM)
		{
		RFASem *sema = arg->getSem();

		// ALL sems must be deleted.
		if (sema)
			delete sema;
		arg->setSem(0);
		}
	else		// If not sem, simply delete the value.
		arg->clear();
	arg->setType(IANUM);
	arg->setNum(0);
	}
else	// Indexed value doesn't exist.
	{
	// PAD ZEROS TO INDEX.  Because we're going to
	// assign to that position down below.
	for (; ii <= index; ++ii)
		{
		arg = new Iarg(0LL);
		vals->rpush(arg);
		}
	darg = vals->getLast();
	}
// NOTE: arg is set up to be assigned to!
return arg;
}

/*************************      END OF FILE      ******************************/
