/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IVAR.CPP
* FILE:	lite/ivar.cpp
* CR:		11/09/99 AM.
* SUBJ:	Interpreted/internal variable semantic object. NLP++ semantics.
* NOTE:	10/14/00 AM. Implementing G("var")[0] as an NLP++ "ARRAY"!
*			For the old G("var"), index will be -1, so that it can
*			be treated distinctly.
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
#include "lite/mach.h"				// 02/04/00 AM.
#include "io.h"
#include "iarg.h"
#include "str.h"
#include "chars.h"
#include "lite/slist.h"				// 02/26/01 AM.
#include "node.h"	// 07/07/03 AM.
//#include "thtab.h"					// 02/26/01 AM.
#include "tree.h"				// 01/10/00 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "pn.h"
#include "gen.h"				// 05/08/00 AM.
#include "thtab.h"					// 02/26/01 AM.
#include "var.h"
#include "pat.h"
//#include "gen.h"				// 05/08/00 AM.
#include "rfasem.h"
#include "lite/nlppp.h"
#include "parse.h"			// 12/03/99 AM.
#include "xml.h"				// 12/29/99 AM.
#include "lite/Arun.h"	// To access runtime collect.				// 05/26/00 AM.
#include "ipair.h"			// 02/27/01 AM.
#include "lite/parse.h"		// 03/13/03 AM.
#include "irule.h"			// 09/12/06 AM.
#include "ivar.h"
#include "nlp.h"

// WARN:	Ivartype and s_Vartype must be kept in sync.
_TCHAR *s_Ivartype[] =
	{
	0,
	_T("G"),
	_T("X"),
	_T("S"),
	_T("N"),
	_T("L"),	// LOCAL VAR FOR USER-DEFINED FN.							// 03/09/02 AM.
	_T("runtime"),
	_T("dummy")
	};

int Ivar::count_ = 0;

/********************************************
* FN:		Special Functions for Ivar Class
********************************************/

Ivar::Ivar(						// Default constructor.
	enum Ivartype typ,		// Variable type.
	_TCHAR *name,					// Variable name.
	long num,					// Variable num.
	RFASem *index				// Index to nth elt!						// 10/13/00 AM.
	)
{
type_ = typ;
name_ = name;
num_  = num;
index_ = index;				// To be eval'ed at runtime.			// 10/13/00 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ivar::~Ivar()
{
if (index_)																		// 10/13/00 AM.
	{
	delete index_;																// 10/13/00 AM.
	index_ = 0;																	// 10/13/00 AM.
	}

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Ivar::Ivar(Ivar &orig)
{
zero();
Ivar::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Ivar &Ivar::operator=(const Ivar &fm)
{
Ivar *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ivar object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Ivar::clear()
{
}

void Ivar::zero()
{
type_ = NULLVAR;
name_ = 0;
num_  = 0;
index_ = 0;	// 10/13/00 AM.
}

void Ivar::copy(const Ivar *orig)
{
Ivar *dest;
dest = this;

dest->type_ = orig->type_;
dest->name_ = orig->name_;
dest->num_  = orig->num_;
dest->index_ = orig->index_;		// Copy constructor??			// 10/13/00 AM.
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ivar &var)
{
#ifndef UNICODE
_TCHAR *nm = var.name_;													// 01/28/06 AM.
#else
char *nm;																	// 01/28/06 AM.
u_to_mbcs((LPCWSTR)var.name_, CP_UTF8, (LPCTSTR*&)nm);		// 01/28/06 AM.
#endif

switch (var.type_)
	{
	case GLOBALVAR:
		output << _T("G(\"") << nm << _T("\")");
		break;
	case CONTEXTVAR:
		if (!var.num_)			// Var of last component in path.	// 11/24/99 AM.
			output << _T("X(\"") << nm << _T("\")");
		else																		// 11/24/99 AM.
			output << _T("X(\"") << nm << _T("\", ") << var.num_ << _T(")");
		break;
	case NODEVAR:
		output << _T("N(\"") << nm << _T("\", ") << var.num_ << _T(")");
		break;
	case SUGGESTEDVAR:														// 11/17/99 AM.
		output << _T("S(\"") << nm << _T("\")");
		break;
	case LOCALVAR:																// 03/09/02 AM.
		output << _T("L(\"") << nm << _T("\")");						// 03/09/02 AM.
		break;																	// 03/09/02 AM.
	case NULLVAR:
	default:
		output << _T("<BAD VAR>");
		break;
	}
if (var.index_)																// 10/13/00 AM.
	output << _T("[") << var.index_ << _T("]");									// 10/13/00 AM.

#ifdef UNICODE
if (nm && *nm)																// 01/28/06 AM.
	u_delete((LPCTSTR*&)nm);											// 01/28/06 AM.
#endif

return output;
}

/*******************************************/

/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

enum Ivartype	 Ivar::getType()	{return type_;}
_TCHAR				*Ivar::getName()	{return name_;}
long				 Ivar::getNum()	{return num_; }
RFASem			*Ivar::getIndex()	{return index_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Ivar::setType(enum Ivartype x)	{type_  = x;}
void	Ivar::setName(_TCHAR *x)				{name_  = x;}
void	Ivar::setNum(long x)					{num_   = x;}
void	Ivar::setIndex(RFASem *x)			{index_ = x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ivar::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ivar::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ivar count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ivar count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////


/********************************************
* FN:		GENVAR
* CR:		11/09/99 AM.
* SUBJ:	Generate var to a rules file.
*********************************************/
void Ivar::genVar(
	std::_t_ostream &ofile
	)
{
ofile << this << std::flush;
}


/********************************************
* FN:		SAME
* CR:		11/09/99 AM.
* SUBJ:	See if two vars are identical in form.
*********************************************/

bool Ivar::same(Ivar *var1, Ivar *var2)
{
if (!var1 && !var2)
	return true;
if (!var1 || !var2)
	return false;

if (var1->type_ != var2->type_)
	return false;
if (!str_equal(var1->name_, var2->name_))
	return false;
if (var1->index_ != var2 ->index_)										// 10/13/00 AM.
	return false;																// 10/13/00 AM.

// WARN: Requiring types that don't care about num to have num==0.
// Zero in NODE or CONTEXT means last.									// 11/24/99 AM.
if (var1->num_ != var2->num_)
	return false;

return true;
}


/********************************************
* FN:		VARTYPE
* CR:		11/09/99 AM.
* SUBJ:	Convert enum to str and vice versa.
*********************************************/

enum Ivartype Ivar::Vartype(_TCHAR ch)
{
switch (ch)
	{
	case 'G':	return GLOBALVAR;
	case 'X':	return CONTEXTVAR;
	case 'S':	return SUGGESTEDVAR;
	case 'N':	return NODEVAR;
	case 'L':	return LOCALVAR;											// 03/09/02 AM.
	default:		return NULLVAR;
	}
}

_TCHAR Ivar::Vartype(enum Ivartype typ)
{
switch (typ)
	{
	case GLOBALVAR:		return 'G';
	case CONTEXTVAR:		return 'X';
	case SUGGESTEDVAR:	return 'S';
	case NODEVAR:			return 'N';
	case LOCALVAR:			return 'L';										// 03/09/02 AM.
	case NULLVAR:			return '\0';
	default:					return '\0';
	}
}


/********************************************
* FN:		EVAL
* CR:		11/15/99 AM.
* SUBJ:	Return value of variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* WARN:	11/15/99 AM. Always returns a sem object.  If var doesn't exist,
*			returns a numeric 0.
*********************************************/

bool Ivar::eval(Nlppp *nlppp, /*UP*/ RFASem* &sem)
{
sem = 0;
return eval(type_,name_,num_,
										index_,									// 10/13/00 AM.
										false,	// call-by-reference.	// 06/16/02 AM.
										nlppp,/*UP*/sem);						// 05/26/00 AM.
}


/********************************************
* FN:		EVALSPECIAL
* CR:		11/17/99 AM.
* SUBJ:	Return value of special variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	eg, X("$text") -- The text of a context node.
*********************************************/

bool Ivar::evalSpecial(Nlppp *nlppp, /*UP*/ RFASem* &sem)
{
sem = 0;
long index = FULLARRAY;

// Evaluate the index.
if (index_)																		// 10/13/00 AM.
	{
	RFASem *semval = 0;
	index_->eval(nlppp, /*UP*/ semval);
	if (semval)
		{
		switch (semval->getType())
			{
			case RSLONG:
				index = semval->getLong();
				break;
			case RSNUM:
				if (!str_to_long(semval->getName(), index))
					return false;
				break;
			default:
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[ivar::eval: Bad type in index.]") << std::ends;
				errOut(&gerrStr,false);
				}

				return false;
			}
		}
	}

return evalSpecial(type_,name_,num_,index,nlppp,/*UP*/sem);
}


/********************************************
* FN:		EVAL
* CR:		10/13/00 AM.
* SUBJ:	Return value of variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	Variant for supporting compiled analyzer.
* WARN:	Always returns a sem object.  If var doesn't exist,
*			returns a numeric 0.
*			10/13/00 AM. Indexing into variable values (i.e., NLP++ arrays!)
*********************************************/

bool Ivar::eval(
	Ivartype vtype,
	_TCHAR *vname,
	long vnum,
	RFASem *vindex,
	bool ref,		// Call-by-reference.								// 06/16/02 AM.
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
long index=FULLARRAY;			// Default.

// Evaluate the index.
if (vindex)
	{
	RFASem *semval = 0;
	vindex->eval(nlppp, /*UP*/ semval);
	if (semval)
		{
		switch (semval->getType())
			{
			case RSLONG:
				index = semval->getLong();
				break;
			case RSNUM:
				if (!str_to_long(semval->getName(), index))
					{
					std::_t_strstream gerrStr;
					gerrStr << _T("[ivar: eval: Bad num.]") << std::ends;
					errOut(&gerrStr,false);
					delete semval;							// MEM LEAK.	// 10/26/00 AM.
					return false;
					}
				break;
			default:
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[ivar::eval: Bad type in index.]") << std::ends;
				errOut(&gerrStr,false);
				}

				delete semval;								// MEM LEAK.	// 10/26/00 AM.
				return false;
			}
		delete semval;										// MEM LEAK.	// 10/26/00 AM.
		}
	}

// Then fetch the value.
return eval(vtype,vname,vnum,index,ref,nlppp,/*UP*/sem);
}


/********************************************
* FN:		EVAL
* CR:		05/26/00 AM.
* SUBJ:	Return value of variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	Variant for supporting compiled analyzer.
*			06/16/02 AM. Handling call by reference.
* WARN:	Always returns a sem object.  If var doesn't exist,
*			returns a numeric 0.
*********************************************/

bool Ivar::eval(
	Ivartype vtype,
	_TCHAR *vname,
	long vnum,
	long vindex,																// 10/13/00 AM.
	bool ref,		// Call-by-reference.								// 06/16/02 AM.
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

// HANDLE SPECIAL VARIABLES.						// 11/17/99 AM.
// With names like X("$text")						// 11/17/99 AM.
// (May want to skip leading whitesp!			// 11/17/99 AM.
if (*vname == '$')									// 11/17/99 AM.
	{
	// This should really be flagged at compile time.				// 06/16/02 AM.
	if (ref)																		// 06/16/02 AM.
		{
		std::_t_strstream gerrStr;					// 06/16/02 AM.
		gerrStr << _T("[Error. $-variable can't take &.]") << std::ends;	// 06/16/02 AM.
		errOut(&gerrStr,false);															// 06/16/02 AM.
		return 0;																// 06/16/02 AM.
		}
	return evalSpecial(vtype,vname,vnum,vindex,
													nlppp, /*UP*/ sem);		// 05/26/00 AM.
	}

// Handle call by reference.  Return a var sem.						// 06/16/02 AM.
if (ref)																			// 06/16/02 AM.
	{
	// Going backwards a bit here.										// 06/16/02 AM.
	RFASem *semindex = new RFASem((long long)vindex);								// 06/16/02 AM.
	Ivar *ivar = new Ivar(vtype,vname,vnum,semindex);				// 06/16/02 AM.
	sem = new RFASem(ivar);													// 06/16/02 AM.
	return true;																// 06/16/02 AM.
	}

Ipair *pairx = 0;
findVar(vtype,vname,vnum,nlppp, /*UP*/ pairx);						// 05/25/00 AM.
if (!pairx)
	{
	// BUILD A DEFAULT VALUE.												// 11/15/99 AM.
	sem = new RFASem(0LL);											// 11/15/99 AM.
	return true;
	}

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	// BUILD A DEFAULT VALUE.									// FIX!	// 01/02/00 AM.
	sem = new RFASem(0LL);								// FIX!	// 01/02/00 AM.
	return true;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	// BUILD A DEFAULT VALUE.									// FIX!	// 01/02/00 AM.
	sem = new RFASem(0LL);								// FIX!	// 01/02/00 AM.
	return true;		// Empty values list.
	}

if (vindex >= 0)		// Index into variable's values (ARRAY!)	// 10/13/00 AM.
	{
	while (darg && vindex-- > 0)											// 10/13/00 AM.
		darg = darg->Right();												// 10/13/00 AM.
	if (!darg)																	// 10/13/00 AM.
		{
		sem = new RFASem(0LL);										// 10/13/00 AM.
		return true;															// 10/13/00 AM.
		}
	return varVal(darg->getData(), sem);
	}

// Old-style treatment.														// 10/14/00 AM.
if (darg->Right())	// Multiple values.
	{
	// Build a sem object.  Return values as args list.
	// COPY LIST HERE AND NOW.									// FIX.	// 12/05/00 AM.
	Dlist<Iarg> *lst = Iarg::copy_args(pairx->getVals());			// 12/05/00 AM.
	sem = new RFASem(lst);													// 12/05/00 AM.
	//sem = new RFASem(pairx->getVals());								// 12/05/00 AM.
	return true;
	}

// Only one value. Build convenient sem object for value.
return varVal(darg->getData(), sem);
}


/********************************************
* FN:		VARVAL
* CR:		10/13/00 AM.
* SUBJ:	Return single value from variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	Pulled this out of eval().
*********************************************/

bool Ivar::varVal(Iarg *arg, /*UP*/ RFASem* &sem)
{
sem = 0;

RFASem *obj;
switch (arg->getType())
	{
	case IANUM:
		sem = new RFASem(arg->getNum());	// Convert to a sem object.
		return true;
	case IAFLOAT:																// 08/17/01 AM.
		sem = new RFASem(arg->getFloat());								// 08/17/01 AM.
		return true;															// 08/17/01 AM.
	case IASTR:
		sem = new RFASem(arg->getStr(), RSSTR);	// Convert to a sem object.
		return true;
	case IASEM:																	// 02/22/00 AM.
		obj = arg->getSem();
		if (!obj)																// 06/12/02 AM.
		  return false;														// 06/12/02 AM.
		switch (obj->getType())
			{
			case RS_KBCONCEPT:												// 02/22/00 AM.
				sem = new RFASem(obj->getKBconcept(),RS_KBCONCEPT);
				break;
			case RS_KBPHRASE:													// 02/22/00 AM.
				sem = new RFASem(obj->getKBphrase(), RS_KBPHRASE);
				break;
			case RS_KBATTR:													// 02/22/00 AM.
				sem = new RFASem(obj->getKBattr(), RS_KBATTR);
				break;
			case RS_KBVAL:														// 02/22/00 AM.
				sem = new RFASem(obj->getKBval(), RS_KBVAL);
				break;
			case RSOSTREAM:													// 09/23/00 AM.
				sem = new RFASem(obj->getOstream());					// 09/23/00 AM.
				break;
			case RSNODE:														// 10/18/00 AM.
				sem = new RFASem(obj->getNode());						// 10/18/00 AM.
				break;
			case RSARGS:														// 03/11/02 AM.
				// Multi-value list.											// 03/11/02 AM.
				sem = new RFASem(Iarg::copy_args(obj->getArgs()));	// 03/11/02 AM.
				break;															// 03/11/02 AM.
			case RSLONG:														// 08/07/02 AM.
				sem = new RFASem(obj->getLong());						// 08/07/02 AM.
				break;
			case RSFLOAT:														// 08/07/02 AM.
				sem = new RFASem(obj->getFloat());						// 08/07/02 AM.
				break;
			case RSSTR:															// 08/07/02 AM.
			case RSNAME:														// 08/07/02 AM.
				sem = new RFASem(obj->getName());						// 08/07/02 AM.
				break;
			case RSNUM:															// 08/07/02 AM.
				{
				long long num = 0;													// 08/07/02 AM.
				if (!str_to_long(obj->getName(),num))					// 08/07/02 AM.
					{
					std::_t_strstream gerrStr;
					gerrStr << _T("[Bad sem type in variable.]") << std::ends;
					return errOut(&gerrStr,false);
					}
				sem = new RFASem(num);										// 08/07/02 AM.
				break;
				}
			default:
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Bad sem type in variable.]") << std::ends;
				return errOut(&gerrStr,false);
				}
			}
		return true;
	case IAOSTREAM:
		// Need ostream variable in output expression, at least.	// 09/23/00 AM.
		//*gerr << "[Can't use file variable in expression.]" << std::endl;
		//return false;														// 09/23/00 AM.

		sem = new RFASem(arg->getOstream());							// 09/23/00 AM.
		return true;															// 09/23/00 AM.
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Ivar::eval: Bad variable value type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}
return false;
}


/********************************************
* FN:		EVALSPECIAL
* CR:		05/26/00 AM.
* SUBJ:	Return value of special variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	Variant to support compiled runtime analyzer.
*********************************************/

bool Ivar::evalSpecial(
	Ivartype vtype,
	_TCHAR *vname,
	long vnum,
	long vindex,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (vindex != FULLARRAY)													// 10/13/00 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[evalSpecial: No indexing on special vars.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

_TCHAR *name = 0;
long num = 0;
if (*vname != '$')
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[evalSpecial: Var name must start with $=") << vname << _T("]")
			<< std::ends;
	errOut(&gerrStr,false);
	return false;
	}
if (!*(name = (vname + 1)))		// Slough the dollar sign.	// 11/18/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[evalSpecial: $ is not a valid variable name.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Get the node.  Not handling range of nodes yet.
Node<Pn> *select=0, *tmp=0;
Node<Pn> *node = 0;			// The current node.		// 11/18/99 AM.
_TCHAR *text = 0;
_TCHAR *str = 0;
Node<Pn> *nstart=0, *nend=0;												// 05/17/01 AM.
COLL *coll=0;
switch (vtype)
	{
	case CONTEXTVAR:
		if (!(select = nlppp->getSelect()))
			return false;
		if (!vnum)						// LAST component of path.	// 11/24/99 AM.
			{
			node = select;						// FIX.					// 08/01/00 AM.
			}
		else								// Get component.				// 11/24/99 AM.
			{
			// Recurse to the root, then count down.				// 11/24/99 AM.
			tmp = select->pathNth(vnum);								// 11/24/99 AM.
			if (!tmp)														// 11/24/99 AM.
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Couldn't find node X(") << vnum
									<< _T(").]") << std::ends;
				errOut(&gerrStr,false);
				return false;
				}
			node = tmp;							// FIX.					// 07/31/00 AM.
			}
		break;
	case NODEVAR:
		// Get node from collect.
		nstart = nend = 0;
		if (!Pat::collectNthnew(											// 12/03/99 AM.
						nlppp->getCollect(), vnum, /*UP*/ nstart, nend)
			 || !nstart || !nend)
			{
			// ASSUME IT'S AN OPTIONAL NODE.								// 01/05/00 AM.
			sem = new RFASem(0LL);						// FIX		// 01/05/00 AM.
			return true;										// FIX		// 01/05/00 AM.
			// return false;													// 01/05/00 AM.
			}
		// 06/05/00 AM.	ALLOWING MULTI-NODE MATCHES.				// 06/05/00 AM.
		//if (nstart != nend)
		//	{
		//	*gerr << "[Special variable " << vname
		//			<< " covers multiple nodes.]" << std::endl;
		//	return false;	// Elt matched multiple nodes.
		//	}
		node = nstart;					// 11/18/99 AM.
		// Get node variable.
		break;
	case RUNNODEVAR:	// COMPILED RUNTIME NODEVAR.					// 05/26/00 AM.
		// Get node from collect.
		if (!vnum)
			vnum = nlppp->len_;	// If 0, get the last coll.		// 05/26/00 AM.
		coll = &(nlppp->coll_[vnum]);
		nstart = (Node<Pn> *) coll->start;
		nend = (Node<Pn> *) coll->end;
		if (!nstart || !nend)
			{
			// ASSUME IT'S AN OPTIONAL NODE.
			sem = new RFASem(0LL);
			return true;
			}
		// 06/05/00 AM.	ALLOWING MULTI-NODE MATCHES.				// 06/05/00 AM.
		//if (nstart != nend)
		//	{
		//	*gerr << "[Special variable " << vname
		//			<< " covers multiple nodes.]" << std::endl;
		//	return false;	// Elt matched multiple nodes.
		//	}
		node = nstart;
		break;
	case GLOBALVAR:
		// Handling some globally useful data for user.			// 12/24/99 AM.
		if (!strcmp_i(name, _T("datum")))	//	$DATUM $datum.				// 03/13/03 AM.
			{
			Parse *parse = nlppp->getParse();							// 03/13/03 AM.
			str = parse->getDatum();										// 03/13/03 AM.
			if (str && *str)													// 03/13/03 AM.
				sem = new RFASem(str, RSSTR);								// 03/13/03 AM.
			else																	// 03/13/03 AM.
				sem = new RFASem(0LL);									// 03/13/03 AM.
			return true;														// 03/13/03 AM.
			}

		if (!strcmp_i(name, _T("input")))	// $INPUT,$input			// 12/24/99 AM.
			{
			// The full pathname, eg, "D:\apps\resume\input\x.txt".
			text = nlppp->getParse()->getInput();					// 12/24/99 AM.
			sem = new RFASem(text, RSSTR);							// 12/24/99 AM.
			return true;													// 12/24/99 AM.
			}
		if (!strcmp_i(name, _T("inputname")))	// $INPUTNAME			// 12/24/99 AM.
			{
			// The file name, eg, "x.txt".
			text = nlppp->getParse()->inputName();					// 12/24/99 AM.
			sem = new RFASem(text, RSSTR);							// 12/24/99 AM.
			return true;													// 12/24/99 AM.
			}
		if (!strcmp_i(name, _T("inputpath")))	// $INPUTPATH			// 12/24/99 AM.
			{
			// The path, eg, "D:\apps\resume\input".
			text = nlppp->getParse()->inputPath();					// 12/24/99 AM.
			sem = new RFASem(text, RSSTR);							// 12/24/99 AM.
			return true;													// 12/24/99 AM.
			}
		if (!strcmp_i(name, _T("inputparent")))	// $INPUTPARENT
			{
			// The path, eg, "Dev" if inputpath is "D:\apps\resume\input\Dev".
			text = nlppp->getParse()->inputParent();
			sem = new RFASem(text, RSSTR);
			return true;
			}
		if (!strcmp_i(name, _T("inputhead")))	// $INPUTHEAD			// 12/24/99 AM.
			{
			// The path, eg, "dave" in "dave.txt".
			text = nlppp->getParse()->inputHead();					// 12/24/99 AM.
			sem = new RFASem(text, RSSTR);							// 12/24/99 AM.
			return true;													// 12/24/99 AM.
			}
		if (!strcmp_i(name, _T("inputtail")))	// $INPUTTAIL			// 12/24/99 AM.
			{
			// The path, eg, "txt" in "dave.txt".
			text = nlppp->getParse()->inputTail();					// 12/24/99 AM.
			sem = new RFASem(text, RSSTR);							// 12/24/99 AM.
			return true;													// 12/24/99 AM.
			}
		if (!strcmp_i(name, _T("today")))		// $TODAY				// 02/04/00 AM.
			{
			text = today1();	// WORKAROUND.							// 02/25/00 AM.
			nlppp->getParse()->internStr(text, /*UP*/ str);		// 02/04/00 AM.
			sem = new RFASem(str, RSSTR);								// 02/04/00 AM.
			return true;
			}
		if (!strcmp_i(name, _T("apppath")))	// $APPPATH				// 11/09/00 AM.
			{
			// The application base directory path.
			text = nlppp->getParse()->getAppdir();					// 11/09/00 AM.
			sem = new RFASem(text, RSSTR);							// 11/09/00 AM.
			return true;													// 11/09/00 AM.
			}
		if (!strcmp_i(name, _T("anaspath")))	// $ANALYZERS				// 11/09/00 AM.
			{
			// The application base directory path.
			text = nlppp->getParse()->getAnasdir();					// 11/09/00 AM.
			sem = new RFASem(text, RSSTR);							// 11/09/00 AM.
			return true;													// 11/09/00 AM.
			}
		if (!strcmp_i(name, _T("kbpath")))	// $KBPATH
			{
			// The application base directory path.
			text = nlppp->getParse()->getKBdir();
			sem = new RFASem(text, RSSTR);
			return true;
			}
		if (!strcmp_i(name, _T("isdirrun")))
			{
			NLP *nlp = nlppp->getParse()->getNLP();
			long processingDir = nlp->getIsDirRun();
			sem = new RFASem((long long)processingDir);
			return true;
			}
		if (!strcmp_i(name, _T("islastfile")))
			{
			NLP *nlp = nlppp->getParse()->getNLP();
			long processingDir = nlp->getIsLastFile();
			sem = new RFASem((long long)processingDir);
			return true;
			}
		if (!strcmp_i(name, _T("isfirstfile")))
			{
			NLP *nlp = nlppp->getParse()->getNLP();
			long processingDir = nlp->getIsFirstFile();
			sem = new RFASem((long long)processingDir);
			return true;
			}
		if (!strcmp_i(name, _T("passnum")))		// $PASSNUM		// 10/11/06 AM.
			{
			// Pass that current rule is in, if in a rule.
			// Need a "top level pass" vs "code scope pass" when not
			// in a rule.
			Parse *parse = nlppp->getParse();						// 10/11/06 AM.
			long xx = parse->getRulepass();							// 09/12/06 AM.
			if (!xx)															// 10/11/06 AM.
				xx = parse->getCurrpass();								// 10/11/06 AM.
			sem = new RFASem((long long)xx);										// 09/12/06 AM.
			return true;													// 09/12/06 AM.
			}
		if (!strcmp_i(name, _T("rulenum")))		// $RULENUM		// 09/12/06 AM.
			{
			// Number of current rule in current pass.
			Irule *rule = nlppp->getRule();
			long xx = 0;													// 10/10/06 AM.
			if (rule)														// 10/10/06 AM.
				xx = rule->getNum();										// 09/12/06 AM.
			// else return zero.						// FIX.			// 10/10/06 AM.

			sem = new RFASem((long long)xx);										// 09/12/06 AM.
			return true;													// 09/12/06 AM.
			}
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Bad special var=") << vname << _T("]") << std::ends;
		Parse *parse = nlppp->getParse();							// 03/13/03 AM.
		errOut(&gerrStr,false, parse->currpass_,parse->line_);			// 03/13/03 AM.
		return false;
		}
	case SUGGESTEDVAR:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[evalSpecial: Suggested var can't be special.]") << std::ends;
		errOut(&gerrStr,false);
		}
		return false;
	case LOCALVAR:																// 03/09/02 AM.
		{
		std::_t_strstream gerrStr;					// 03/09/02 AM.
		gerrStr << _T("[evalSpecial: Local var can't be special.]") << std::ends;
		errOut(&gerrStr,false);															// 03/09/02 AM.
		}
		return false;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[evalSpecial: No variable type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}

if (nstart == nend)
	return nodeVarspecial(node,name,nlppp,/*UP*/sem);
else			// MULTI-NODE CASE.
	return nodeVarspecial(nstart,nend,name,nlppp,/*UP*/sem);
}


/********************************************
* FN:		NODEVAR
* CR:		10/18/00 AM.
* SUBJ:	Return value of variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	Normal var.  Variants below are for special $vars.
* WARN:	Always returns a sem object.  If var doesn't exist,
*			returns a numeric 0.
*			11/21/00 AM. DON'T CREATE EMPTY SEM OBJECTS.
*********************************************/

bool Ivar::nodeVar(
	Node<Pn> *node,
	_TCHAR *name,			// Name of var.
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!node)
	return false;
Pn *pn = node->getData();
Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return true;

Ipair *pairx = 0;
Var::find(name,dlist,/*UP*/pairx);
if (!pairx)
	{
	// BUILD A DEFAULT VALUE.												// 11/21/00 AM.
	//sem = new RFASem(0LL);											// 11/15/99 AM.
	return true;
	}

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	// BUILD A DEFAULT VALUE.												// 11/21/00 AM.
	//sem = new RFASem(0LL);								// FIX!	// 01/02/00 AM.
	return true;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	// BUILD A DEFAULT VALUE.												// 11/21/00 AM.
	//sem = new RFASem(0LL);								// FIX!	// 01/02/00 AM.
	return true;		// Empty values list.
	}

// Old-style treatment.														// 10/14/00 AM.
if (darg->Right())	// Multiple values.
	{
	// Build a sem object.  Return values as args list.
	// Copying multiple args.									// FIX	// 08/15/01 AM.
	// sem = new RFASem(pairx->getVals()); // Not copying.		// 08/15/01 AM.
	sem = new RFASem(Iarg::copy_args(pairx->getVals()));			// 08/15/01 AM.
	return true;
	}

// Only one value. Build convenient sem object for value.
return varVal(darg->getData(), sem);
}


/********************************************
* FN:		NODEVARSPECIAL
* CR:		10/18/00 AM.
* SUBJ:	Return value of special node variable ON MULTIPLE NODES.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	SPLIT UP EVALSPECIAL.
*********************************************/

bool Ivar::nodeVarspecial(
	Node<Pn> *nstart,
	Node<Pn> *nend,
	_TCHAR *name,		// Variable name.
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
if (!nstart || !nend)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[nodevarspecial: No node given.]") << std::ends;
	errOut(&gerrStr,false);
			// 05/17/01 AM.
	return false;
	}

Pn *pn = nstart->getData();

_TCHAR *text = 0;
_TCHAR *str = 0;

if (!strcmp_i(name, _T("length")))	//	$LENGTH $length.	// Get node text length.
	{
	long len;
	if (nstart != nend)		// Multi-node match.						// 06/05/00 AM.
		{
		Pn *pn2 = nend->getData();											// 06/05/00 AM.
		len = pn2->End - pn->Start + 1;									// 06/05/00 AM.
		}
	else
		len = pn->End - pn->Start + 1;									// 06/04/00 AM.
	sem = new RFASem((long long)len);													// 06/04/00 AM.
	return true;
	}
if (!strcmp_i(name, _T("text")))	// $TEXT $text							// 11/10/00 AM.
	{
	long len  = 0;
	if (nstart != nend)
		{
		Pn *pn2 = nend->getData();
		len = pn2->End - pn->Start + 1;
		}
	else
		len = pn->End - pn->Start + 1;

		// Get text for multi-node region.
		// Intern text.
	_TCHAR *buf;
	//text = pn->pnStr(len);			// Create buffer for node's text.
	text = make_str(pn->getText(), len);					// FIX.	// 11/20/00 AM.
	if (!text)																
		{
		sem = new RFASem(0LL);					// "NULL"			
		return true;
		}
	buf = Chars::create(len+1);							
	clean_str(text,buf);
	nlppp->getParse()->internStr(buf, /*UP*/ str);
	sem = new RFASem(str, RSSTR);							
	Chars::destroy(buf);			// Free up buffer.
	Chars::destroy(text);
	return true;
	}

std::_t_strstream gerrStr;
gerrStr << _T("[nodevar: Multi-node case not handled.]") << std::ends;
errOut(&gerrStr,false);
return false;
}


/********************************************
* FN:		NODEVARSPECIAL
* CR:		10/18/00 AM.
* SUBJ:	Return value of special node variable.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for variable values.
* NOTE:	SPLIT UP EVALSPECIAL.
*********************************************/

bool Ivar::nodeVarspecial(
	Node<Pn> *node,
	_TCHAR *name,		// Variable name.
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
if (!node)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[nodevarspecial: No node given.]") << std::ends;
	errOut(&gerrStr,false);
			// 05/17/01 AM.
	return false;
	}

Pn *pn = node->getData();

_TCHAR *text = 0;
_TCHAR *str = 0;
long num;

// Get the data from the node(s).
if (!strcmp_i(name, _T("text")))	//	$TEXT $text.	// Get node text.
	{
	_TCHAR *buf;
	long len  = 0;																// 07/23/00 AM.
	text = pn->pnStr(len);			// Create buffer for node's text.
	if (!text)																	// 07/24/00 AM.
		{
		sem = new RFASem(0LL);					// "NULL"			// 07/24/00 AM.
		return true;
		}
	buf = Chars::create(len+1);								// FIX	// 07/24/00 AM.
	clean_str(text,buf);
	nlppp->getParse()->internStr(buf, /*UP*/ str);
	sem = new RFASem(str, RSSTR);									// 01/25/00 AM.
	Chars::destroy(buf);			// Free up buffer.
	Chars::destroy(text);
	return true;
	}

if (!strcmp_i(name, _T("length")))	//	$LENGTH $length.	// Get node text length.
	{
	long len;
	len = pn->End - pn->Start + 1;										// 06/04/00 AM.
	sem = new RFASem((long long)len);													// 06/04/00 AM.
	return true;
	}

if (!strcmp_i(name, _T("exists")))	//	$EXISTS $exists.	// TRUE if node exists.
	{
	// We got here, so there's a node.  Only meaningful for rule elements,
	// but will return true for X("$exists") also.
	sem = new RFASem (1LL);
	return true;
	}

// Get the data from the node(s).
if (!strcmp_i(name, _T("raw")))	//	$RAW $raw.	// Get node's raw text.
	{
	str = pn->pnStr();													// 01/25/00 AM.
	nlppp->getParse()->internStr(str, /*UP*/ text);
	sem = new RFASem(text, RSSTR);									// 01/25/99 AM.
	Chars::destroy(str);													// 01/25/00 AM.
	return true;
	}

if (!strcmp_i(name, _T("xmltext")))	//	$XMLTEXT $xmltext.		// 12/29/99 AM.
	{
	// Convert to form suitable for XML output.
	// Convert '&' to "&amp;", etc.
	long start, end;
	_TCHAR *xml;
	pn->getTextStartEnd(text, start, end);
	xml = Xml::text2xml(text,end-start+1,nlppp->htab());
	// xml is an INTERNED string.										// 01/25/00 AM.
	sem = new RFASem(xml, RSSTR);
	return true;
	}

	// TEXT BASED ON TREE, NOT JUST TEXT BUFFER.						// 09/26/00 AM.
if (!strcmp_i(name, _T("treeraw"))	//	$TREERAW $treeraw.			// 09/26/00 AM.
 || !strcmp_i(name, _T("treetext"))
 || !strcmp_i(name, _T("treespace"))
) // $TREETEXT $treetext			// 03/10/03 AM.
	{
	bool spaces = !strcmp_i(name, _T("treespace"));

	// Tree text will be <= buffer text length.
	long len = pn->getEnd() - pn->getStart() + 1;
	long siz = len+1;															// 10/29/04 AM.
	if (siz < 100)	// Heuristic for cushion.							// 10/29/04 AM.
		siz *= 5;																// 10/29/04 AM.
	else
		siz *= 2;																// 10/29/04 AM.
	text = Chars::create(siz);												// 11/01/04 AM.
	_TCHAR *ptr = text;					// First empty loc in text.
	long tmp = siz;	
	bool first = true;											// FIX.	// 08/01/06 AM.
	bool ok = Pat::treeText(node, true, spaces, first, text, ptr, tmp);
	if (!ok)																		// 10/29/04 AM.
		{
		Chars::destroy(text);												// 10/29/04 AM.
		return false;															// 10/29/04 AM.
		}
	if (!strcmp_i(name, _T("treetext")))										// 03/10/03 AM.
	  {
//		_TCHAR *buf = Chars::create(len+1);								// 03/10/03 AM.
		_TCHAR *buf = Chars::create(siz);	// More!	// FIX.		// 08/01/06 AM.
		clean_str(text,buf);													// 03/10/03 AM.
		nlppp->getParse()->internStr(buf, /*UP*/ str);				// 03/10/03 AM.
		sem = new RFASem(str, RSSTR);										// 03/10/03 AM.
		Chars::destroy(buf);			// Free up buffer.				// 03/10/03 AM.
		Chars::destroy(text);												// 03/10/03 AM.
		return true;															// 03/10/03 AM.
		}
	nlppp->getParse()->internStr(text, /*UP*/ str);
	sem = new RFASem(str, RSSTR);
	Chars::destroy(text);					// Free up buffer.
	return true;
	}

// This one checks to see if the node is last one in its phrase.
if (!strcmp_i(name, _T("end")))		// $END $end	//		// 11/18/99 AM.
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Use $end only in node,context vars.  eg, N($end,1)]")
				  << std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $end)]") << std::ends;
		errOut(&gerrStr,false);

		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	num = (node->Right() ? 0 : 1);	// True if end, false if not.
	sem = new RFASem((long long)num);
	return true;
	}

// This one checks to see if the node is first one in its phrase.
if (!strcmp_i(name, _T("start")))		// $START $start			// 12/02/99 AM.
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Use $start only in node,context vars.  eg, N($start,1)]")
					<< std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $start)]") << std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	num = (node->Left() ? 0 : 1);	// True if start, false if not.
	sem = new RFASem((long long)num);
	return true;
	}

// This one retrieves start offset of node.
if (!strcmp_i(name, _T("ostart")))		// $OSTART $ostart		// 12/02/99 AM.
	{
	if (!pn)							// FIX.	// 12/02/99 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $ostart)]") << std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a zero offset.
		sem = new RFASem(0LL);
		return true;
		}
	num = pn->getStart();
	sem = new RFASem((long long)num);
	return true;
	}

// This one retrieves end offset of node.
if (!strcmp_i(name, _T("oend")))		// $OEND $oend.			// 12/02/99 AM.
	{
	if (!pn)							// FIX.	// 12/02/99 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $oend)]") << std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a zero offset.
		sem = new RFASem(0LL);
		return true;
		}
	num = pn->getEnd();
	sem = new RFASem((long long)num);
	return true;
	}

// Check if the node's token is all-caps.
if (!strcmp_i(name, _T("allcaps"))			// $ALLCAPS $allcaps		// 01/10/00 AM.
	 || !strcmp_i(name, _T("uppercase")))	// $UPPERCASE $uppercase.
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Use $allcaps only in node,context vars. eg, N($allcaps,1)]")
					<< std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $allcaps)]") << std::ends;
		errOut(&gerrStr,false);

		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	// GET THE LEAF NODE.
	// (NOT HANDLING MULTIPLE NODES, FOR NOW.)
	if (!(node = Tree<Pn>::leafNode(node)))
		return false;
	pn = node->getData();
	// If allcaps, return 1, else 0.
	// Get the node's text.  If alphabetic chars are all uppercase, then
	// success.
	_TCHAR *buf;
	bool ok;
	buf = pn->pnStr();			// Create buffer for node's text.
	ok = all_uppercase(buf);
	Chars::destroy(buf);			// Free up buffer.
	num = (ok ? 1 : 0);
	sem = new RFASem((long long)num);
	return true;
	}

// Check if the node's token is all-lowercase.
if (!strcmp_i(name, _T("lowercase")))	// $LOWERCASE $lowercase.	// 01/11/00 AM.
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr <<
			_T("[Use $lowercase only in node,context vars.  eg, N($lowercase,1)]")
					<< std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $lowercase]")
					<< std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	// GET THE LEAF NODE.
	// (NOT HANDLING MULTIPLE NODES, FOR NOW.)
	if (!(node = Tree<Pn>::leafNode(node)))
		return false;
	pn = node->getData();
	// If allcaps, return 1, else 0.
	// Get the node's text.  If alphabetic chars are all uppercase, then
	// success.
	_TCHAR *buf;
	bool ok;
	buf = pn->pnStr();			// Create buffer for node's text.
	ok = all_lowercase(buf);
	Chars::destroy(buf);			// Free up buffer.
	num = (ok ? 1 : 0);
	sem = new RFASem((long long)num);
	return true;
	}

// Check if the node's token is capitalized.
if (!strcmp_i(name, _T("cap")))	// $CAP $cap.	// 01/11/00 AM.
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr <<
			_T("[Use $cap only in node,context vars.  eg, N($cap,1)]")
					<< std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $cap.]")
					<< std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	// GET THE LEAF NODE.
	// (NOT HANDLING MULTIPLE NODES, FOR NOW.)
	if (!(node = Tree<Pn>::leafNode(node)))
		return false;
	pn = node->getData();
	// If allcaps, return 1, else 0.
	// Get the node's text.  If alphabetic chars are all uppercase, then
	// success.
	_TCHAR *buf;
	_TCHAR ch;
	buf = pn->getName();
	if ((ch = *buf) && alphabetic(ch)
		&& is_upper((_TUCHAR)ch))									// 12/16/01 AM.
		num = 1;
	else
		num = 0;
	sem = new RFASem((long long)num);
	return true;
	}

// Check if the node's token is MIXCAP.
if (!strcmp_i(name, _T("mixcap")))			// $MIXCAP $mixcap		// 01/15/00 AM.
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Use $mixcap only in node,context vars.  eg, N($mixcap,1)]")
					<< std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $mixcap.]") << std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	// GET THE LEAF NODE.
	// (NOT HANDLING MULTIPLE NODES, FOR NOW.)
	if (!(node = Tree<Pn>::leafNode(node)))
		return false;
	pn = node->getData();
	// If allcaps, return 1, else 0.
	// Get the node's text.  If alphabetic chars are all uppercase, then
	// success.
	_TCHAR *buf;
	bool ok;
	buf = pn->pnStr();			// Create buffer for node's text.
	ok = mixcap(buf);
	Chars::destroy(buf);			// Free up buffer.
	num = (ok ? 1 : 0);
	sem = new RFASem((long long)num);
	return true;
	}

// Check if the node's token is unknown.
if (!strcmp_i(name, _T("unknown"))	// $UNKNOWN $unknown.	// 01/11/00 AM.
	 || !strcmp_i(name, _T("unk")))	// $UNK $unk
	{
#ifdef OLD_
	if (vtype != NODEVAR && vtype != CONTEXTVAR
		 && vtype != RUNNODEVAR)											// 06/06/00 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr <<
			_T("[Use $unknown only in node,context vars.  eg, N($unknown,1)]")
					<< std::ends;
		errOut(&gerrStr,false);
		return false;
		}
#endif
	if (!node)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find node for $unknown.]")
					<< std::ends;
		errOut(&gerrStr,false);
		//return false;
		// Recovery. Return a "false" value.
		sem = new RFASem(0LL);
		return true;
		}
	// GET THE LEAF NODE.
	// (NOT HANDLING MULTIPLE NODES, FOR NOW.)
	if (!(node = Tree<Pn>::leafNode(node)))
		return false;
	pn = node->getData();
	// If allcaps, return 1, else 0.
	// Get the node's text.  If alphabetic chars are all uppercase, then
	// success.
	Sym *sym;
	sym = pn->getlcSym();	// Get lowercased sym.
	if (sym->isLooked() && !(sym->isKnown()))
		num = 1;
	else
		num = 0;
	sem = new RFASem((long long)num);
	return true;
	}

std::_t_strstream gerrStr;
gerrStr << _T("[Bad special var=") << name << _T("]") << std::ends;
Parse *parse = nlppp->getParse();										// 03/13/03 AM.
errOut(&gerrStr,false, parse->currpass_,parse->line_);						// 03/13/03 AM.

return false;
}


/********************************************
* FN:		FIND
* CR:		11/15/99 AM.
* SUBJ:	Return internal pointer to variable itself, if found.
* RET:	True if OK, NOT IF FOUND.
*			UP pair - filled if found.
*********************************************/

bool Ivar::find(Nlppp *nlppp, /*UP*/ Ipair* &pairx)
{
return findVar(type_,name_,num_,nlppp, /*UP*/ pairx);				// 05/25/00 AM.
}


/********************************************
* FN:		MAKE
* CR:		11/15/99 AM.
* SUBJ:	Make and return internal pointer to a variable.
* RET:	True if ok.
*********************************************/

bool Ivar::make(Nlppp *nlppp, /*UP*/ Ipair* &pairx)
{
return makeVar(type_,name_,num_,nlppp,/*UP*/ pairx);				// 05/25/00 AM.
}


/********************************************
* FN:		GET
* CR:		11/15/99 AM.
* SUBJ:	Find or make internal variable.
* RET:	True if found or made ok.
* OPT:	The find and the making can be combined.
*********************************************/

bool Ivar::get(Nlppp *nlppp, /*UP*/ Ipair* &pairx)
{
// NOTE: this find returns ok if everything ok, NOT if it found something!
if (!findVar(type_,name_,num_,nlppp, /*UP*/ pairx))					// 05/25/00 AM.
	return false;
if (pairx)
	return true;		// IF FOUND IT, RETURN TRUE.

// Didn't find variable, so make it.
return makeVar(type_,name_,num_,nlppp, /*UP*/ pairx);				// 05/25/00 AM.
}


/********************************************
* FN:		FINDVAR
* CR:		05/25/00 AM.
* SUBJ:	Return internal pointer to variable itself, if found.
* RET:	True if OK, NOT IF FOUND.
*			UP pair - filled if found.
*********************************************/

bool Ivar::findVar(
	Ivartype typ,
	_TCHAR *name,
	int num,
	Nlppp *nlppp,
	/*UP*/
	Ipair* &pairx
	)
{
pairx = 0;
Node<Pn> *select, *tmp;
Dlist<Ipair> *dlist;
Pn *pn;
Node<Pn> *nstart, *nend;
COLL *coll;
if (!name || !*name)															// 04/04/02 AM.
	{
	std::_t_strstream gerrStr;						// 04/04/02 AM.
	gerrStr << _T("[Ivar::find: Given no name.]") << std::ends;				// 04/04/02 AM.
	errOut(&gerrStr,false);																// 04/04/02 AM.
//	return true;	// RECOVER.												// 04/04/02 AM.
	}

switch (typ)
	{
	case GLOBALVAR:
		if (!Var::find(name, nlppp->getParse(), /*DU*/ pairx)) // 11/17/99 AM.
			return false;													// 11/17/99 AM.
		return true;													// 11/17/99 AM.
	case CONTEXTVAR:
		if (!(select = nlppp->getSelect()))
			return false;
		if (!num)						// LAST component of path.	// 11/24/99 AM.
			pn = select->getData();
		else								// Get component.				// 11/24/99 AM.
			{
			// Recurse to the root, then count down.				// 11/24/99 AM.
			tmp = select->pathNth(num);								// 11/24/99 AM.
			if (!tmp)														// 11/24/99 AM.
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Couldn't find node X(") << num
									<< _T(").]") << std::ends;
				errOut(&gerrStr,false);
				return false;
				}
			pn  = tmp->getData();										// 11/24/99 AM.
			}
		dlist = pn->getDsem();
		break;
	case SUGGESTEDVAR:
		if (!Var::find(name, nlppp->getDsem(), /*DU*/ pairx))	// 11/17/99 AM.
			return false;													// 11/17/99 AM.
		return true;														// 11/17/99 AM.
	case NODEVAR:
		// Get node from collect.
		nstart = nend = 0;
		if (!Pat::collectNthnew(										// 12/03/99 AM.
								nlppp->getCollect(), num, /*UP*/ nstart, nend)
			 || !nstart || !nend)
			return false;
		if (nstart != nend)
			return false;	// Elt matched multiple nodes.

		// Get node variable and value.
		pn = nstart->getData();
		dlist = pn->getDsem();
		break;
	case LOCALVAR:																// 03/09/02 AM.
		// nlppp will have a local var list for the current user-defined
		// function.  Fetch value from there.
#ifdef DEBUGGING_
		{													// DEBUGGING		// 04/08/02 AM.
		std::_t_strstream gerrStr;
		gerrStr << _T("[Ivar::find: localvar.]")  << name << _T("|");
		if (nlppp->getLocals())
			gerrStr << *(nlppp->getLocals());
		gerrStr << std::ends;
		errOut(&gerrStr,false);
		}
#endif
		if (!Var::find(name, nlppp->getLocals(), /*DU*/ pairx))	// 03/09/02 AM.
			return false;														// 03/09/02 AM.
		return true;															// 03/09/02 AM.
	case RUNNODEVAR:				// COMPILED RUNTIME NODEVAR.		// 05/26/00 AM.
		// Get node from collect.
		if (!num)
			num = nlppp->len_;	// If 0, get the last coll.		// 05/26/00 AM.
		coll = &(nlppp->coll_[num]);
		nstart = (Node<Pn> *) coll->start;
		nend = (Node<Pn> *) coll->end;
		if (!nstart || !nend)
			return false;
		if (nstart != nend)
			return false;	// Elt matched multiple nodes.

		// Get node variable and value.
		pn = nstart->getData();
		dlist = pn->getDsem();
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Ivar::find: No variable type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}

// Find variable in list.
return Var::find(name, dlist, /*DU*/ pairx);
}


/********************************************
* FN:		GETVAR
* CR:		05/25/00 AM.
* SUBJ:	Find or make internal variable.
* RET:	True if found or made ok.
* OPT:	The find and the making can be combined.
*********************************************/

bool Ivar::getVar(
	Ivartype typ,
	_TCHAR *name,
	int num,
	Nlppp *nlppp,
	/*UP*/
	Ipair* &pairx
	)
{
// NOTE: this find returns ok if everything ok, NOT if it found something!
if (!findVar(typ, name, num, nlppp, /*UP*/ pairx))
	return false;
if (pairx)
	return true;		// IF FOUND IT, RETURN TRUE.

// Didn't find variable, so make it.
bool ok = makeVar(typ,name,num,nlppp, /*UP*/ pairx);
if (ok && pairx)
	return true;

std::_t_strstream gerrStr;
gerrStr << _T("[getVar: Couldn't make variable.]") << std::ends;
errOut(&gerrStr,false);
return false;
}


/********************************************
* FN:		MAKEVAR
* CR:		05/25/00 AM.
* SUBJ:	Make and return internal ptr to variable.
* RET:	True if ok.
*			UP pair - internal ptr to variable.
* NOTE:	Used by compiled runtime system.
*			Taken from Ivar::make().
*********************************************/

bool Ivar::makeVar(
	Ivartype typ,
	_TCHAR *name,
	int num,
	Nlppp *nlppp,
	/*UP*/
	Ipair* &pairx
	)
{
pairx = 0;
Node<Pn> *select, *tmp;
Dlist<Ipair> *dlist = 0;
Pn *pn;
bool ok;			// 12/03/99 AM.
Parse *parse;	// 12/03/99 AM.
Node<Pn> *nstart, *nend;
COLL *coll;
switch (typ)
	{
	case GLOBALVAR:
		// OPT: Calls find to see if var is there!  Need variant that doesn't.
		// BUG: Doesn't return pair.									// 12/03/99 AM.
		//return Var::create(name_, (Iarg *)0, nlppp->getParse(), 0);

		// FIX: This one does return the pair.						// 12/03/99 AM.
		parse = nlppp->getParse();										// 12/03/99 AM.
		dlist = parse->getVars();										// 12/03/99 AM.
		ok = Var::create(name,(Iarg *)0,/*DU*/dlist,pairx);		// 12/03/99 AM.
		parse->setVars(dlist);											// 12/03/99 AM.
		return ok;															// 12/03/99 AM.
	case CONTEXTVAR:
		if (!(select = nlppp->getSelect()))
			return false;
		if (!num)						// LAST component of path.	// 11/24/99 AM.
			pn = select->getData();
		else								// Get component.				// 11/24/99 AM.
			{
			// Recurse to the root, then count down.				// 11/24/99 AM.
			tmp = select->pathNth(num);								// 11/24/99 AM.
			if (!tmp)														// 11/24/99 AM.
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[Couldn't find node X(") << num
									<< _T(").]") << std::ends;
				errOut(&gerrStr,false);
				return false;
				}
			pn  = tmp->getData();										// 11/24/99 AM.
			}
		dlist = pn->getDsem();
		break;
	case SUGGESTEDVAR:
		dlist = nlppp->getDsem();										// 11/17/99 AM.
		Var::create(name, (Iarg *)0, /*DU*/ dlist,pairx);		// 11/17/99 AM.
		nlppp->setDsem(dlist);											// 11/17/99 AM.
		return true;
	case NODEVAR:
		// Get node from collect.
		nstart = nend = 0;
		if (!Pat::collectNthnew(										// 12/03/99 AM.
									nlppp->getCollect(), num, /*UP*/ nstart, nend)
			 || !nstart || !nend)
			return false;
		if (nstart != nend)
			return false;	// Elt matched multiple nodes.

		// Get node variable and value.
		pn = nstart->getData();
		dlist = pn->getDsem();
		break;
	case LOCALVAR:																// 03/09/02 AM.
		dlist = nlppp->getLocals();										// 03/09/02 AM.
		Var::create(name, (Iarg *)0, /*DU*/ dlist,pairx);			// 03/09/02 AM.
		nlppp->setLocals(dlist);											// 03/09/02 AM.
		return true;
	case RUNNODEVAR:			// COMPILED RUNTIME NODEVAR.			// 05/26/00 AM.
		// Get node from collect.
		if (!num)
			num = nlppp->len_;	// If 0, get the last coll.		// 05/26/00 AM.
		coll = &(nlppp->coll_[num]);
		nstart = (Node<Pn> *) coll->start;
		nend = (Node<Pn> *) coll->end;
		if (!nstart || !nend)
			return false;
		if (nstart != nend)
			return false;	// Elt matched multiple nodes.

		// Get node variable and value.
		pn = nstart->getData();
		dlist = pn->getDsem();
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Ivar::find: No variable type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}

// Create variable and add to appropriate node.
ok = Var::create(name, (Iarg *)0, /*DU*/ dlist,pairx);
pn->setDsem(dlist);
return ok;
}



/********************************************
* FN:		GENEVAL
* CR:		05/07/00 AM.
* SUBJ:	Gen evaluation of an NLP++ var.
*********************************************/

bool Ivar::genEval(Gen *gen,
	bool ref			// Flag call-by-reference.							// 06/16/02 AM.
	)
{
//std::_t_ofstream *fcode = gen->fcode_;
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
// This one will have to return an RFASem, because type can only
// be known dynamically at runtime.
// SHOULD HANDLE THE SPECIAL VARS HERE.
_TCHAR *name = getName();
long long num = getNum();
_TCHAR s_num[64];																// 05/04/01 AM.
if (num)																			// 05/04/01 AM.
	_stprintf(s_num, _T("%llu"), num);											// 05/04/01 AM.
else																				// 05/04/01 AM.
	_stprintf(s_num, _T("0LL"));												// 05/04/01 AM.
*fcode << _T("Arun::");
switch (getType())
	{
	case GLOBALVAR:
		*fcode << _T("g(_T(\"") << name											// 06/05/00 AM.
			<< _T("\"),");															// 05/04/01 AM.
		break;
	case CONTEXTVAR:
		*fcode << _T("x(_T(\"") << name << _T("\"),") << s_num					// 05/04/01 AM.
			<< _T(",");																// 05/04/01 AM.
		break;
	case SUGGESTEDVAR:
		*fcode << _T("s(_T(\"") << name											// 06/05/00 AM.
			<< _T("\"),");															// 05/04/01 AM.
		break;
	case LOCALVAR:																// 03/09/02 AM.
		*fcode << _T("l(_T(\"") << name											// 03/09/02 AM.
			<< _T("\"),");															// 03/09/02 AM.
		break;																	// 03/09/02 AM.
	case NODEVAR:
	case RUNNODEVAR:							// TRY						// 05/04/01 AM.
		*fcode << _T("n(_T(\"") << name << _T("\"),") << s_num					// 05/04/01 AM.
			<< _T(",");																// 05/04/01 AM.
		break;
	default:
		*fcode << _T("\nERROR();") << std::endl;									// 05/04/01 AM.
		*fcode << _T("// Bad variable type.") << std::endl;					// 05/04/01 AM.
		return false;
	}

if (index_)															// FIX.	// 05/04/01 AM.
	index_->genEval(gen);											// FIX.	// 05/04/01 AM.
else																	// FIX.	// 05/04/01 AM.
	*fcode << FULLARRAY;											// FIX.	// 05/04/01 AM.

// CALL-BY-REFERENCE.														// 06/16/02 AM.
if (ref)																			// 06/16/02 AM.
	*fcode << _T(",true");														// 06/16/02 AM.
else																				// 06/16/02 AM.
	*fcode << _T(",false");														// 06/16/02 AM.

*fcode << _T(",nlppp)");												// FIX.	// 05/04/01 AM.

return true;
}



/********************************************
* FN:		NODEVARHASVAL
* CR:		06/26/01 AM.
* SUBJ:	True if named node variable has a value.
* RET:	True if named node variable has a value.
*********************************************/

bool Ivar::nodeVarhasval(
	Node<Pn> *node,
	_TCHAR *name			// Name of var.
	)
{
if (!node)
	return false;
Pn *pn = node->getData();
return nodeVarhasval(pn,name);
}

// VARIANT.	// 06/14/05 AM.
bool Ivar::nodeVarhasval(
	Pn *pn,
	_TCHAR *name			// Name of var.
	)
{
if (!pn)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return false;

Ipair *pairx = 0;
Var::find(name,dlist,/*UP*/pairx);
if (!pairx)
	return false;		// No pair.

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	return false;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	return false;		// Empty values list.
	}

return true;			// Has at least one value.
}

/********************************************
* FN:		NODEVARNZ
* CR:		06/16/05 AM.
* SUBJ:	True if named node variable has nonzero value.
* RET:	True if named node variable has nonzero value.
*********************************************/

bool Ivar::nodeVarNZ(
	Pn *pn,
	_TCHAR *name			// Name of var.
	)
{
if (!pn)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return false;

Ipair *pairx = 0;
Var::find(name,dlist,/*UP*/pairx);
if (!pairx)
	return false;		// No pair.

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	return false;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	return false;		// Empty values list.
	}

// If more than one value, then it's an array.
if (darg->Right())
	return true;

RFASem *sm = 0;	// 03/7/14 AM.

// One value.  See if it's zero or null.
Iarg *iarg = darg->getData();
switch (iarg->getType())
	{
	case IASEM:
	   sm = iarg->getSem();		// 03/07/14 AM.
	   if (!sm) return false;	// 03/07/14 AM.
		return sm->truth();	// FIX.	// 03/07/14 AM.
		break;
	case IASTR:
		if (iarg->getStr())
			return true;  // What about an empty string? ...
		return false;
		break;
	case IANUM:
		if (iarg->getNum())
			return true;
		return false;
		break;
	case IAFLOAT:
		if (iarg->getFloat())	// Need a tolerance comparison.
			return true;
		return false;
		break;
	case IAREF:
	default:
		return false;
		break;
	}

return true;			// Has at least one value.
}


/********************************************
* FN:		NODEVAREQ
* CR:		06/16/05 AM.
* SUBJ:	True if named node variable equals str value.
* RET:	True if equal.
*********************************************/

bool Ivar::nodeVarEQ(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	_TCHAR *sval
	)
{
if (!pn)
	return false;

// If string empty, see if var is empty.
if (!sval || !*sval)
	return !nodeVarNZ(pn,name);

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return false;

Ipair *pairx = 0;
Var::find(name,dlist,/*UP*/pairx);
if (!pairx)
	return false;		// No pair.

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	return false;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	return false;		// Empty values list.
	}

// If more than one value, then it's an array.
if (darg->Right())
	return false;	// (Fail even if first value of array is same.)

// One value.  See if it matches.
Iarg *iarg = darg->getData();
_TCHAR *vstr;
RFASem *sem;	// FIX. // 07/12/11 AM.
switch (iarg->getType())
	{
	case IASTR:
		vstr = iarg->getStr();
		return _tcscmp(vstr,sval) ? false : true;
		break;
	case IASEM:
		sem = iarg->getSem();	// FIX.	// 07/12/11 AM.
		switch (sem->getType())	// FIX.	// 07/12/11 AM.
			{
			case RSNAME:	// FIX.	// 07/12/11 AM.
			case RSSTR:	// FIX.	// 07/12/11 AM.
			case RSNUM:	// FIX.	// 07/12/11 AM.
				vstr = sem->sem_to_str();	// FIX.	// 07/12/11 AM.
				return _tcscmp(vstr,sval) ? false : true;	// FIX.	// 07/12/11 AM.
				break;
			default: break;
			}
		break;
	case IANUM:
	case IAFLOAT:
	case IAREF:
	default:
		return false;
		break;
	}

return false;
}


/********************************************
* FN:		NODEVAREQ
* CR:		06/16/05 AM.
* SUBJ:	True if named node variable equals num value.
* RET:	True if equal.
*********************************************/

bool Ivar::nodeVarEQ(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	long long nval
	)
{
if (!pn)
	return false;

// If num is zero, see if var is zero or empty.
if (!nval)
	return !nodeVarNZ(pn,name);

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return false;

Ipair *pairx = 0;
Var::find(name,dlist,/*UP*/pairx);
if (!pairx)
	return false;		// No pair.

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	return false;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	return false;		// Empty values list.
	}

// If more than one value, then it's an array.
if (darg->Right())
	return false;	// (Fail even if first value of array is same.)

// One value.  See if it matches.
Iarg *iarg = darg->getData();
RFASem *sem;	// FIX. // 07/12/11 AM.
bool ok = false;	// FIX.	// 07/12/11 AM.
long long aval = 0;	// FIX.	// 07/12/11 AM.
switch (iarg->getType())
	{
	case IANUM:
		return nval == iarg->getNum();
		break;
	case IASEM:
		sem = iarg->getSem();	// FIX.	// 07/12/11 AM.
		switch (sem->getType())	// FIX.	// 07/12/11 AM.
			{
			case RSLONG:	// FIX.	// 07/12/11 AM.
				aval = sem->sem_to_long(ok); //FIX // 07/12/11 AM.
				return nval == aval;	// FIX.	// 07/12/11 AM.
				break;
			default: break;
			}
		break;
	case IASTR:
	case IAFLOAT:
	case IAREF:
	default:
		return false;
		break;
	}

return false;
}


/********************************************
* FN:		NODEVARGT
* CR:		02/03/23
* SUBJ:	True if named node variable great than a number value.
* RET:	True if greater than.
*********************************************/

bool Ivar::nodeVarGTLT(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	long long nval,
	bool lessThan
	)
{
if (!pn)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return false;

Ipair *pairx = 0;
Var::find(name,dlist,/*UP*/pairx);
if (!pairx)
	return false;		// No pair.

Dlist<Iarg> *vals = pairx->getVals();
if (!vals)
	{
	return false;		// Pair with no values list.
	}
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	{
	return false;		// Empty values list.
	}

// If more than one value, then it's an array.
if (darg->Right())
	return false;	// (Fail even if first value of array is same.)

// One value.  See if it matches.
Iarg *iarg = darg->getData();
RFASem *sem;	// FIX. // 07/12/11 AM.
bool ok = false;	// FIX.	// 07/12/11 AM.
long long aval = 0;	// FIX.	// 07/12/11 AM.

switch (iarg->getType())
	{
	case IANUM:
		return lessThan? iarg->getNum() < nval : iarg->getNum() > nval;
		break;
	case IASEM:
		sem = iarg->getSem();	// FIX.	// 07/12/11 AM.
		switch (sem->getType())	// FIX.	// 07/12/11 AM.
			{
			case RSLONG:	// FIX.	// 07/12/11 AM.
				aval = sem->sem_to_long(ok); //FIX // 07/12/11 AM.
				return lessThan ? aval < nval : aval > nval;	// FIX.	// 07/12/11 AM.
				break;
			default: break;
			}
		break;
	case IASTR:
		aval = std::stoll(iarg->getStr());
		return lessThan? aval < nval : aval > nval;
		break;
	case IAFLOAT:
	case IAREF:
	default:
		return false;
		break;
	}

return false;
}


bool Ivar::nodeRemoveval(
	Pn *pn,
	_TCHAR *name
	)
{
if (!pn || !name || !*name)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

int argCount = 0;
if (!Var::rmVal(name, dlist, argCount))
	return false;

pn->setDsem(dlist);
return true;
}


bool Ivar::nodeReplaceval(
	Pn *pn,
	_TCHAR *name,
	CONCEPT *con,
	bool bRM,
	bool bPUSH
	)
{
if (!pn || !name || !*name || !con)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

RFASem *semval = new RFASem(con,RS_KBCONCEPT);

if (!Var::setVal(name, semval,bRM,bPUSH,dlist))
	return false;

pn->setDsem(dlist);
return true;
}


/********************************************
* FN:		NODEREPLACEVAL
* CR:		06/26/01 AM.
* SUBJ:	Replace node variable value.
* RET:	True if ok.
*********************************************/

bool Ivar::nodeReplaceval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	RFASem *semval,		// Value to replace.
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH	// If pushing value front	// 12/12/14 AM.
	)
{
if (!pn || !name || !*name || !semval)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::setVal(name, semval,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}


/********************************************
* FN:		NODEREPLACEVAL
* CR:		11/14/02 AM.
* SUBJ:	Replace node variable value with list of args.
* RET:	True if ok.
* NOTE:	Sem with type==RSARGS needs special handling.
*********************************************/

bool Ivar::nodeReplaceval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	Dlist<Iarg> *args		// Value to replace.
	)
{
if (!pn || !name || !*name || !args)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::setVals(name, args, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEREPLACEVAL
* CR:		06/26/01 AM.
* SUBJ:	Replace node variable value.
* RET:	True if ok.
*********************************************/

bool Ivar::nodeReplaceval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	_TCHAR *strval,		// Value to replace.
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH	// If pushing value front	// 12/12/14 AM.
	)
{
if (!pn || !name || !*name || !strval || !*strval)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::setVal(name, strval,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEREPLACEVAL
* CR:		06/26/01 AM.
* SUBJ:	Replace node variable value.
* RET:	True if ok.
*********************************************/

bool Ivar::nodeReplaceval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	long long numval,		// Value to replace.
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH	// If pushing value front	// 12/12/14 AM.
	)
{
if (!pn || !name || !*name)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::setVal(name, numval,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEREPLACEVAL
* CR:		12/15/14 AM.
* SUBJ:	Replace node variable value.
* RET:	True if ok.
*********************************************/

bool Ivar::nodeReplaceval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	float fnum,		// Value to replace.
	bool bRM,	// If rm existing values
	bool bPUSH	// If pushing value front
	)
{
if (!pn || !name || !*name)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::setVal(name, fnum,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEPUSHVAL
* CR:		12/14/14 AM.
* SUBJ:	Replace node variable value.
* RET:	True if ok.
*********************************************/

bool Ivar::nodePushval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	_TCHAR *strval,		// Value to push.
	bool bRM,	// If rm existing values
	bool bPUSH	// If pushing value front
	)
{
if (!pn || !name || !*name || !strval || !*strval)
	return false;


Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	{
	dlist = new Dlist<Ipair>();	// Empty list.
	}

if (!Var::pushVal(name, strval,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}


/********************************************
* FN:		NODEPUSHVAL
* CR:		12/15/14 AM.
* SUBJ:	Push sem value onto node variable.
* RET:	True if ok.
*********************************************/

bool Ivar::nodePushval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	RFASem *semval,		// Value to replace.
	bool bRM,	// If rm existing values (not needed)
	bool bPUSH	// If pushing value front
	)
{
if (!pn || !name || !*name || !semval)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::pushVal(name, semval,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEPUSHVAL
* CR:		12/15/14 AM.
* SUBJ:	Push value onto node variable.
* RET:	True if ok.
*********************************************/

bool Ivar::nodePushval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	long long numval,		// Value to replace.
	bool bRM,	// If rm existing values
	bool bPUSH	// If pushing value front
	)
{
if (!pn || !name || !*name)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::pushVal(name, numval,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEPUSHVAL
* CR:		12/15/14 AM.
* SUBJ:	Push value onto node variable.
* RET:	True if ok.
*********************************************/

bool Ivar::nodePushval(
	Pn *pn,
	_TCHAR *name,			// Name of var.
	float fnum,		// Value to replace.
	bool bRM,	// If rm existing values
	bool bPUSH	// If pushing value front
	)
{
if (!pn || !name || !*name)
	return false;

Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	dlist = new Dlist<Ipair>();	// Empty list.

if (!Var::pushVal(name, fnum,bRM,bPUSH, /*DU*/ dlist))
	return false;

pn->setDsem(dlist);
return true;
}

/********************************************
* FN:		NODEVARNAMES
* CR:		05/13/02 AM.
* SUBJ:	Get names of all node variables.
* RET:	list - List of variable names.
*********************************************/

Dlist<Iarg> *Ivar::nodeVarnames(
	Pn *pn
	)
{
if (!pn)
	return 0;
Dlist<Ipair> *dlist = pn->getDsem();
if (!dlist)
	return 0;
Delt<Ipair> *dpair = dlist->getFirst();
if (!dpair)
	return 0;

Dlist<Iarg> *vars = new Dlist<Iarg>();
Iarg *arg;
Ipair *pair;
for (; dpair; dpair = dpair->Right())
	{
	pair = dpair->getData();
	arg = new Iarg(pair->getKey());
	vars->rpush(arg);
	}
return vars;
}

/*************************      END OF FILE      ******************************/
