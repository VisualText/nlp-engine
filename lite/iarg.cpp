/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IARG.CPP
* FILE:	lite\iarg.cpp
* CR:		10/24/98 AM.
* SUBJ:	Abstract class for rule-action's arguments.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"		// 05/21/01 AM.

#include "slist.h"		// 12/10/99 AM.
#include "node.h"			// 07/07/03 AM.
#include "rfasem.h"		// 12/16/00 AM.
#include "starr.h"		// 05/06/00 AM.
#include "node.h"			// 05/21/01 AM.
#include "tree.h"			// 05/21/01 AM.
#include "pn.h"			// 05/21/01 AM.
#include "parse.h"		// 07/23/07 AM.
#include "iarg.h"
#include "str.h"			// 06/07/99 AM.

// WARNING: Must match iarg.h/enum Iargtype definition.
static _TCHAR *Iargtype_s[] =
	{
	_T("null"),
	_T("str"),
	_T("num"),
	_T("ostream"),				// 02/09/99 AM.
	_T("sem"),					// 02/16/00 AM.
	_T("float")					// 08/17/01 AM.
	};

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Iarg>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Iarg>::count_  = 0;
template<>	// Linux.	// 04/26/07 AM.
int Slist<Iarg>::count_ = 0;			// 12/10/99 AM.
template<>	// Linux.	// 04/26/07 AM.
int Selt<Iarg>::count_  = 0;			// 12/10/99 AM.
#else
int Dlist<Iarg>::count_ = 0;
int Delt<Iarg>::count_  = 0;
int Slist<Iarg>::count_ = 0;			// 12/10/99 AM.
int Selt<Iarg>::count_  = 0;			// 12/10/99 AM.
#endif
int Iarg::count_ = 0;

/********************************************
* FN:		Special Functions for class.
* CR:		10/24/98 AM.
********************************************/

Iarg::Iarg(			// Constructor.
	enum Iargtype typ
	)
{
type = typ;
switch (typ)
	{
	case IASTR:
		val_.str_ = 0;
		break;
	case IANUM:
		val_.num_ = 0;
		break;
	case IAOSTREAM:
		val_.out_ = 0;
		break;				// FIX	// 08/17/01 AM.
	case IASEM:							// 02/16/00 AM.
	case IAREF:							// 05/26/02 AM.
		val_.sem_ = 0;					// 02/16/00 AM.
		break;				// FIX	// 08/17/01 AM.
	case IAFLOAT:						// 08/17/01 AM.
		val_.float_ = 0;				// 08/17/01 AM.
	case IANULL:
	default:
		val_.str_ = 0;
		break;			// FIX	// 08/17/01 AM.
	}

#ifndef STABLE_
++count_;
#endif
}

Iarg::Iarg(_TCHAR *str)
{
type = IASTR;
val_.str_ = str;

#ifndef STABLE_
++count_;
//*gerr << (long)this << "+" << count_ << std::endl;
#endif
}

Iarg::Iarg(long long num)
{
type = IANUM;
val_.num_ = num;

#ifndef STABLE_
++count_;
#endif
}

Iarg::Iarg(float fnum)			// 08/17/01 AM.
{
type = IAFLOAT;
val_.float_ = fnum;

#ifndef STABLE_
++count_;
#endif
}

Iarg::Iarg(std::_t_ostream *outptr)
{
type = IAOSTREAM;
val_.out_ = outptr;

#ifndef STABLE_
++count_;
#endif
}

Iarg::Iarg(RFASem *sem)
{
type = IASEM;
val_.sem_ = sem;

#ifndef STABLE_
++count_;
#endif
}

Iarg::Iarg(						// New constructor.						// 05/26/02 AM.
	RFASem *sem,
	enum Iargtype typ
	)
{
type = typ;
val_.sem_ = sem;

#ifndef STABLE_
++count_;
#endif
}

Iarg::~Iarg()
{
clear();			// 07/02/99 AM.

#ifndef STABLE_
--count_;
//*gerr << (long)this << "-" << count_ << std::endl;
#endif
}


/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Iarg::Iarg(Iarg &orig)
{
//zero();									// FIX PURIFY COMPLAINT.	// 04/08/01 AM.
Iarg::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Iarg &Iarg::operator=(const Iarg &fm)
{
Iarg *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iarg object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iarg::clear()
{
// Open output streams should be removed. // 02/09/99 AM.
if (type == IAOSTREAM)
	{
//	delete val_.out_;	// ostreams now managed in Parse object.	// 05/23/01 AM.
	val_.out_ = 0;
	}
else if (type == IASEM
	|| type == IAREF)															// 05/26/02 AM.
	{
	delete val_.sem_;
	val_.sem_ = 0;
	}
}

void Iarg::zero()
{
switch (this->type)		// To remove PURIFY warning.	// 08/12/99 AM.
	{
	case IASTR:
		val_.str_ = 0;
		break;
	case IANUM:
		val_.num_ = 0;
		break;
	case IAOSTREAM:
		val_.out_ = 0;
		break;			// FIX	// 08/17/01 AM.
	case IASEM:
	case IAREF:						// 05/26/02 AM.
		val_.sem_ = 0;
		break;			// FIX	// 08/17/01 AM.
	case IAFLOAT:					// 08/17/01 AM.
		val_.float_ = 0;			// 08/17/01 AM.
		break;						// 08/17/01 AM.
	case IANULL:
	default:
		val_.str_ = 0;
		break;			// FIX	// 08/17/01 AM.
	}

}

void Iarg::copy(const Iarg *orig)
{
Iarg *dest;
dest = this;

dest->type = orig->type;

switch (orig->type)
	{
	case IASTR:
		dest->val_.str_ = orig->val_.str_;
		break;
	case IANUM:
		dest->val_.num_ = orig->val_.num_;
		break;
	case IAOSTREAM:
		dest->val_.out_ = orig->val_.out_;
		break;				// PREEMPTIVE FIX!							// 05/23/01 AM.
	case IASEM:
	case IAREF:																	// 05/26/02 AM.
		// NOTE: JUST POINTER ASSIGNMENT.  IF CHANGING TO A FULL COPY,
		// WATCH OUT FOR FIREWORKS!
		dest->val_.sem_ = orig->val_.sem_;
		break;				// PREEMPTIVE FIX!							// 05/23/01 AM.
	case IAFLOAT:																// 08/17/01 AM.
		dest->val_.float_ = orig->val_.float_;							// 08/17/01 AM.
		break;																	// 08/17/01 AM.
	case IANULL:
	default:
		dest->val_.str_ = orig->val_.str_;
	}

}

/*******************************************/

/*******************************************/


std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iarg &arg)
{
switch(arg.type)
	{
	case IANUM:
		output << arg.val_.num_;
		break;
	case IASTR:
		if (arg.val_.str_)													// 03/14/03 AM.
			{
#ifndef UNICODE
			output << arg.val_.str_;
#else
			char *lpstr8;														// 01/27/06 AM.
			u_to_mbcs((LPCWSTR)arg.val_.str_, CP_UTF8, (LPCTSTR*&)lpstr8);
			output << lpstr8;													// 01/27/06 AM.
			u_delete((LPCTSTR*&)lpstr8);									// 01/27/06 AM.
#endif
			}
		break;
	case IAOSTREAM:
		output << (long)(arg.val_.out_);
		break;
	case IASEM:
	case IAREF:																	// 05/26/02 AM.
		output << *(arg.val_.sem_);										// 08/20/00 AM.
			// << "<rfasem>";			// 02/16/00 AM.					// 08/20/00 AM.
		break;
	case IAFLOAT:																// 08/17/01 AM.
		output << arg.val_.float_;											// 08/17/01 AM.
		break;																	// 08/17/01 AM.
	case IANULL:
	default:
		output << _T("<NOARG>");
		break;
	}
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Delt<Iarg> &delt)
{
Iarg *arg;
arg  = delt.getData();
output << *arg;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Iarg> &list)
{
Delt<Iarg> *delt;
delt = list.getFirst();
if (!delt)
	return output;
output << *delt;
while ((delt = delt->Right()))
	{
	output << _T(" ") << *delt;
	}
return output;
}
/*******************************************/


/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iarg::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iarg::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iarg count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iarg count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		COPY_TOP
* CR:		02/18/00 AM.
* SUBJ:	Special copy.  Only list,delts,iargs, but not pointed to data.
* NOTE:	Class function.
*			12/05/00 AM. To work with delete_top, must assume that no SEM
*			values are present in the given list.  (May want to check...)
********************************************/
Dlist<Iarg> *Iarg::copy_top(Dlist<Iarg> *list)
{
if (!list)
	return 0;
Dlist<Iarg> *nlist = new Dlist<Iarg>();	// Make empty list.
Delt<Iarg> *delt;
Iarg *iarg, *narg;
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	iarg = delt->getData();
	narg = new Iarg(*iarg);
	nlist->rpush(narg);
	}
return nlist;
}


/********************************************
* FN:		DELETE_TOP
* CR:		02/18/00 AM.
* SUBJ:	Special delete.  Only list,delts,iargs, but not pointed to data.
* NOTE:	Class function.
*			09/23/02 AM. Inconsistency here, especially regarding create and
*			delete of RSARGS, perhaps other sems.  Need to make consistent with
*			copy_top and then let every user fend for himself.
********************************************/
void Iarg::delete_top(Dlist<Iarg> *list)
{
if (!list)
	return;

#ifdef DEBUGGING_
	{										// DEBUGGING!!!	// 04/08/02 AM>
	std::_t_strstream gerrStr;
	gerrStr << _T("[delete_top:]") << list << std::ends;
	errOut(&gerrStr,false);
	}
#endif

Delt<Iarg> *delt, *tmp;
delt = list->getFirst();
Iarg *iarg;
RFASem *sem;
while (delt)
	{
	tmp = delt->Right();
	iarg = delt->getData();
	if (iarg->type == IASEM													// 05/26/02 AM.
		|| iarg->type == IAREF)												// 05/26/02 AM.
		{
		// Used only by arg evaluation for fn calls.					// 03/29/00 AM.
		// Delete sem objects that represent evaluated exprs.
		sem = iarg->getSem();												// 03/29/00 AM.
		switch (sem->getType())												// 03/29/00 AM.
			{
			case RS_KBCONCEPT:
			case RS_KBPHRASE:
			case RS_KBATTR:
			case RS_KBVAL:
			case RSSTR:
			case RSNUM:
			case RSLONG:
			case RSNODE:									// MEM LEAK.	// 10/25/00 AM.
			case RSOSTREAM:						// Preemptive.			// 05/23/01 AM.
				delete sem;														// 03/29/00 AM.
				break;
			case RSARGS:									// MEM LEAK.	// 10/25/00 AM.
				// Now we are copying args always, so delete them.	// 12/07/00 AM.
//				sem->setArgs(0);		// Zero out args so won't del	// 10/25/00 AM.
				delete sem;
				break;
			default:
				break;															// 10/25/00 AM.
			}
		iarg->setSem(0);			// DON'T WIPE OUT SEMANTIC TREE.
		}
	Delt<Iarg>::DeleteDeltAndData(delt);	// Now safe to wipe out element.
	delt = tmp;
	}
list->setFirst(0);
list->setLast(0);
delete list;
}


/********************************************
* FN:		COPY_ARGS
* CR:		12/05/00 AM.
* SUBJ:	Like copy_top, but also copies sem object.
* OPT:	Adding an argument for interning strings during copy.
*			If moving data from local to global variable, then need to
*			make sure strings are interned. 01/09/07 AM.
********************************************/

Dlist<Iarg> *Iarg::copy_args(Dlist<Iarg> *list,
	Parse *parse,	// For interning.			// 01/09/07 AM.
	bool fintern	// Flag if interning.	// 01/09/07 AM.
	)
{
if (!list)
	return 0;
Dlist<Iarg> *nlist = new Dlist<Iarg>();	// Make empty list.
Delt<Iarg> *delt;
Iarg *iarg, *narg;
RFASem *nsem, *sem;				// Copied sem, if any.
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	iarg = delt->getData();
	narg = new Iarg(*iarg);
	nlist->rpush(narg);

	if (iarg->type == IASEM													// 05/26/02 AM.
	 || iarg->type == IAREF)												// 05/26/02 AM.
		{
		sem = iarg->getSem();
		switch (sem->getType())
			{
			case RS_KBCONCEPT:
			case RS_KBPHRASE:
			case RS_KBATTR:
			case RS_KBVAL:
			case RSSTR:
			case RSNUM:
			case RSLONG:
			case RSFLOAT:														// 04/09/02 AM.
			case RSNODE:
			case RSOSTREAM:										// FIX.	// 05/21/01 AM.
			case RSNAME:											// FIX.	// 05/21/01 AM.
//				nsem = new RFASem(*sem);						// BUG?	// 04/09/02 AM.
				nsem = sem->copy_rfasem(						// FIX	// 04/09/02 AM.
										parse,fintern);						// 01/09/07 AM.
				narg->setSem(nsem);
				break;
			case RSARGS:
			   {
				// NEED TO COPY ARGS LIST.						// FIX.	// 05/05/02 AM.
				// A recursive call here.
				Dlist<Iarg> *nnlist =							// FIX.	// 05/05/02 AM.
						copy_args(sem->getArgs(),				// FIX.	// 05/05/02 AM.
							parse,fintern);									// 01/09/07 AM.
				nsem = new RFASem(nnlist);						// FIX.	// 05/05/02 AM.
				narg->setSem(nsem);								// FIX.	// 05/05/02 AM.
				}
				break;												// FIX.	// 05/05/02 AM.
			default:
				narg->setSem(0);
				break;
			}
		}
	else if (iarg->type == IASTR)								// FIX.	// 10/28/08 AM.
		{
		if (fintern)												// FIX.	// 10/28/08 AM.
			{
			// For now, intern any string(s).
			_TCHAR *str = iarg->getStr();						// FIX.	// 10/28/08 AM.
			_TCHAR *x = 0;														// 10/28/08 AM.
			if (str && *str)													// 10/28/08 AM.
				{
				parse->internStr(str,/*UP*/x);							// 10/28/08 AM.
				narg->setStr(x);	// Install the interned string.	// 10/28/08 AM.
				}
			}
		}
	}
return nlist;
}



/********************************************
* FN:		COPY_ARG
* CR:		04/08/02 AM>
* SUBJ:	Full copy of a single arg.
* NOTE:	Assume this is used for local string copies only,
*			else modify as in copy_args.	01/09/07 AM.
********************************************/

Iarg *Iarg::copy_arg(Iarg *iarg)
{
if (!iarg)
	return 0;
Iarg *narg;
RFASem *nsem, *sem;				// Copied sem, if any.
narg = new Iarg(*iarg);

if (iarg->type == IASEM														// 05/26/02 AM.
 || iarg->type == IAREF )													// 05/26/02 AM.
	{
	sem = iarg->getSem();
	switch (sem->getType())
		{
		case RS_KBCONCEPT:
		case RS_KBPHRASE:
		case RS_KBATTR:
		case RS_KBVAL:
		case RSSTR:
		case RSNUM:
		case RSLONG:
		case RSNODE:
		case RSFLOAT:															// 04/09/02 AM.
		case RSOSTREAM:
		case RSNAME:
//			nsem = new RFASem(*sem);							// BUG?	// 04/09/02 AM.
			nsem = sem->copy_rfasem();							// FIX.	// 04/09/02 AM.
			narg->setSem(nsem);
			break;
		case RSARGS:
			   {
				// NEED TO COPY ARGS LIST.						// FIX.	// 05/05/02 AM.
				// A recursive call here.
				Dlist<Iarg> *nnlist =							// FIX.	// 05/05/02 AM.
						copy_args(sem->getArgs());				// FIX.	// 05/05/02 AM.
				nsem = new RFASem(nnlist);						// FIX.	// 05/05/02 AM.
				narg->setSem(nsem);								// FIX.	// 05/05/02 AM.
				}
				break;												// FIX.	// 05/05/02 AM.
		default:
			narg->setSem(0);
			break;
		}
	}
return narg;
}

/********************************************
* FN:		Access functions
* CR:		10/24/98 AM.
********************************************/

enum Iargtype Iarg::getType()	{ return type; }
_TCHAR *Iarg::getStr()
{
if (type != IASTR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[getStr: Arg is not a string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
return val_.str_;
}

long long Iarg::getNum()
{
if (type != IANUM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[getNum: Arg is not a num.]") << std::ends;
	errOut(&gerrStr,false);
	return 0LL;
	}
return val_.num_;
}

float Iarg::getFloat()														// 08/17/01 AM.
{
if (type != IAFLOAT)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[getFloat: Arg is not a float.]") << std::ends;
	errOut(&gerrStr,false);
	return 0.0;
	}
return val_.float_;
}

std::_t_ostream *Iarg::getOstream()
{
if (type != IAOSTREAM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[getOstream: Arg is not an ostream ptr.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
return val_.out_;
}

RFASem *Iarg::getSem()
{
switch (type)
	{
	case IASEM:
	case IAREF:																	// 05/26/02 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[getSem: Arg is not a sem.]") << std::ends;
		errOut(&gerrStr,false);
		return 0;
		}
	}
return val_.sem_;
}

// FOR ODBC BINDING.															// 06/08/02 AM.
long long *Iarg::getNumaddr()													// 06/08/02 AM.
{
return &(val_.num_);
}

float *Iarg::getFloataddr()												// 06/08/02 AM.
{
return &(val_.float_);
}

/********************************************
* FN:		Modify functions
* CR:		10/24/98 AM.
********************************************/

void Iarg::setType(enum Iargtype x)	{ type = x; }
void Iarg::setNum(long long x)
{
if (type != IANUM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[setNum: Arg is not a num.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

val_.num_ = x;
}

void Iarg::setFloat(float x)												 // 08/17/01 AM.
{
if (type != IAFLOAT)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[setFloat: Arg is not a float.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

val_.float_ = x;
}

void Iarg::setStr(_TCHAR *x)
{
if (type != IASTR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[setStr: Arg is not a string.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
val_.str_ = x;
}

void Iarg::setOstream(std::_t_ostream *x)
{
if (type != IAOSTREAM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[setOstream: Arg is not an ostream ptr.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
val_.out_ = x;
}

void Iarg::setSem(RFASem *x)
{
switch (type)																	// 05/26/02 AM.
	{
	case IASEM:
	case IAREF:																	// 05/26/02 AM>
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[setSem: Arg is not a sem.]") << std::ends;
		errOut(&gerrStr,false);
		return;
		}
	}
val_.sem_ = x;
}

/********************************************
* FN:		FETCHNUM
* CR:		11/09/99 AM.
* SUBJ:	Get numeric arg value.
* NOTE:	If string try to convert.
* OPT:	Would like semantics to stop storing numeric args as strings.
********************************************/

bool Iarg::fetchNum(/*UP*/ long long &num)
{
num = -1;
switch (type)
	{
	case IANUM:
		num = val_.num_;
		return true;
	case IASTR:
		// Try to convert.
		if (!str_to_long(val_.str_, num))
			return false;
		return true;
	default:
		return false;
	}
}

/********************************************
* FN:		FINDARG
* CR:		05/21/99 AM.
* SUBJ:	Find arg by its value.
* NOTE:	Doing case-sensitive matching.
********************************************/

Delt<Iarg> *Iarg::findArg(Dlist<Iarg> *dlist, _TCHAR *key)
{
if (!dlist || !key)
	return 0;
Delt<Iarg> *delt;
Iarg *arg;
for (delt = dlist->getFirst(); delt; delt = delt->pRight)
	{
	arg = delt->getData();
	if (arg->getType() == IASTR
		 && !_tcscmp(arg->getStr(), key))
		return delt;				// FOUND THE STRING.
	// If not string type or matching string, keep looking through list.
	}
return 0;			// Arg not found.
}

/********************************************
* FN:		FINDARGCASE
* CR:		08/10/03 AM.
* SUBJ:	Find arg by its value, case-insensitive.
* NOTE:	Doing case-sensitive matching.
********************************************/

Delt<Iarg> *Iarg::findArgcase(Dlist<Iarg> *dlist, _TCHAR *key)
{
if (!dlist || !key)
	return 0;
Delt<Iarg> *darg;
for (darg = dlist->getFirst(); darg; darg = darg->Right())
	{
	if (!strcmp_i(key, darg->getData()->getStr()))
		return darg;
	}
return 0;
}

/********************************************
* FN:		WHITEARG
* CR:		08/10/99 AM.
* SUBJ:	Find whitespace arg if any.
********************************************/

Delt<Iarg> *Iarg::whiteArg(Dlist<Iarg> *dlist)
{
if (!dlist)
	return 0;
Delt<Iarg> *delt;
if ((delt = findArg(dlist, _T(" "))))
	return delt;
if ((delt = findArg(dlist, _T("\t"))))
	return delt;
if ((delt = findArg(dlist, _T("\n"))))
	return delt;
if ((delt = findArg(dlist, _T("\r"))))
	return delt;
return 0;
}

/********************************************
* FN:		GENARGS
* CR:		06/15/99 AM.
* SUBJ:	Generate args to a rules file.
********************************************/

void Iarg::genArgs(
	Dlist<Iarg> *args,
	_TCHAR *sep,				// Separator string.  eg, ", "
	std::_t_ostream &ofile,
	bool trunc				// eg, if in a comment and want to truncate list.
	)
{
if (!args || !sep)
	return;
Delt<Iarg> *darg;
if ((darg = args->getFirst()))	// First specially.
	genArg(darg->getData(), ofile);

int count = 1;		// To avoid long lines.								// 06/05/00 AM.
for (darg = darg->Right(); darg; darg = darg->Right())
	{
	if (++count >= 10)														// 06/05/00 AM.
		{
		if (trunc)																// 06/05/00 AM.
			{
			ofile << _T("...") << std::flush;
			return;
			}
		ofile << std::endl;
		count = 0;
		}
	ofile << sep << std::flush;
	genArg(darg->getData(), ofile);
	}
}


/********************************************
* FN:		GENARG
* CR:		06/07/99 AM.
* SUBJ:	Generate arg to a rules file.
********************************************/

void Iarg::genArg(
	Iarg *arg,
	std::_t_ostream &ofile,
	bool cast	// If true, display "(long)" etc.					// 10/01/01 AM.
	)
{
if (!arg)
	return;
RFASem *sem = 0;																// 05/21/01 AM.
Node<Pn> *node;																// 05/21/01 AM.
Pn *pn = 0;																		// 05/21/01 AM.
CONCEPT *conc = 0;															// 05/21/01 AM.
CG *cg = 0;
Dlist<Iarg> *iargs=0;														// 08/08/02 AM.

// FIX. Strings may have newlines and other junk.					// 05/10/07 AM.
_TCHAR cbuf[MAXLINE];	// For converting to C string. // FIX.	// 02/19/08 AM.

switch (arg->getType())
	{
	case IASTR:
//		ofile << arg->getStr() << std::flush;		// 07/11/99 AM.
		// This may not be sufficient....
		cbuf[0] = '\0';																// 05/10/07 AM.
		clean_str(arg->getStr(), /*UP*/ cbuf);								// 05/10/07 AM.
		ofile << _T("\"");								// 02/17/00 AM.
		Iarg::genName(cbuf, ofile);
		ofile << _T("\"") << std::flush;					// 02/17/00 AM.
		break;
	case IANUM:
		if (cast)																// 10/01/01 AM.
			ofile <<_T("((long)")													// 10/01/01 AM.
			<< arg->getNum()
			<< _T(")")																// 09/09/01 AM.
			<< std::flush;
		else																		// 10/01/01 AM.
		ofile
			<< arg->getNum()
			<< std::flush;
		break;
	case IAFLOAT:																// 08/17/01 AM.
		if (cast)																// 10/01/01 AM.
			ofile <<_T("((float)")												// 10/01/01 AM.
				<< arg->getFloat()											// 08/17/01 AM.
				<< _T(")")															// 09/09/01 AM.
				<< std::flush;
		else																		// 10/01/01 AM.
		ofile
				<< arg->getFloat()											// 08/17/01 AM.
				<< std::flush;
		break;																	// 08/17/01 AM.
	case IAOSTREAM:
		ofile << _T("<ostream>") << std::flush;
		break;
	case IASEM:
	case IAREF:																	// 05/26/02 AM.
//		ofile << "<sem>" << std::flush;											// 05/21/01 AM.
		if (!(sem = arg->getSem()))										// 05/21/01 AM.
			return;																// 05/21/01 AM.
		switch (sem->getType())												// 05/21/01 AM.
			{
			case RSNAME:														// 05/21/01 AM.
			case RSSTR:															// 05/21/01 AM.
			case RSNUM:															// 05/21/01 AM.
				cbuf[0] = '\0';												// 05/10/07 AM.
				clean_str(sem->getName(), /*UP*/ cbuf);				// 05/10/07 AM.
				ofile << _T("\"");													// 05/21/01 AM.
				Iarg::genName(cbuf, ofile);								// 05/21/01 AM.
				ofile << _T("\"") << std::flush;										// 05/21/01 AM.
				break;															// 05/21/01 AM.
			case RSLONG:														// 05/21/01 AM.
				if (cast)														// 10/01/01 AM.
					ofile <<_T("((long)")											// 10/01/01 AM.
						<< sem->getLong()										// 05/21/01 AM.
						<< _T(")")													// 09/09/01 AM.
						<< std::flush;
				else																// 10/01/01 AM.
				ofile
						<< sem->getLong()										// 05/21/01 AM.
						<< std::flush;
				break;															// 05/21/01 AM.
			case RSFLOAT:														// 09/09/01 AM.
				if (cast)														// 10/01/01 AM.
					ofile <<_T("((float)")										// 10/01/01 AM.
						<< sem->getFloat()									// 09/09/01 AM.
						<< _T(")") << std::flush;										// 09/09/01 AM.
				else																// 10/01/01 AM.
				ofile 
						<< sem->getFloat()									// 09/09/01 AM.
						<< std::flush;												// 09/09/01 AM.
			case RSOSTREAM:													// 05/21/01 AM.
				ofile << _T("<ostream>") << std::flush;							// 05/21/01 AM.
				break;															// 05/21/01 AM.
			case RSNODE:														// 05/21/01 AM.
				if (!(node = sem->getNode()))								// 05/21/01 AM.
					return;														// 05/21/01 AM.
				if (!(pn = node->getData()))								// 05/21/01 AM.
					return;														// 05/21/01 AM.
				ofile << _T("pnode:\"")											// 05/21/01 AM.
						<< str(pn->getName())								// 05/21/01 AM.
						<< _T("\"")													// 05/21/01 AM.
						<< std::flush;												// 05/21/01 AM.
				break;															// 05/21/01 AM.
			case RS_KBCONCEPT:												// 05/21/01 AM.
				// TODO: NEED CG OBJECT TO GET KB CONCEPTS.			// 05/21/01 AM.
				if (!(conc = sem->getKBconcept()))						// 05/21/01 AM.
					return;														// 05/21/01 AM.
				_TCHAR buf[MAXSTR];
				cg = sem->getCG();
				cg->conceptName(conc, buf);
				ofile << _T("concept:\"")										// 05/21/01 AM.
						<< buf
						<< _T("\"")													// 05/21/01 AM.
						<< std::flush;												// 05/21/01 AM.
				break;															// 05/21/01 AM.
			case RS_KBPHRASE:													// 05/21/01 AM.
			case RS_KBATTR:													// 05/21/01 AM.
			case RS_KBVAL:														// 05/21/01 AM.
				ofile << _T("kbobject:\"")										// 05/21/01 AM.
						<< _T("<name>")										// 05/21/01 AM.
						<< _T("\"")													// 05/21/01 AM.
						<< std::flush;												// 05/21/01 AM.
				break;															// 05/21/01 AM.
			case RSARGS:														// 08/08/02 AM.
				break;
				{
				// Choosing to flag this as an error -- no
				// recursive lists, etc.									// 08/10/02 AM.
				std::_t_strstream gerrStr;
				gerrStr << _T("[genArg: Error. Multi-value in list.]") << std::ends;
				errOut(&gerrStr,false);
				}
			default:																// 05/21/01 AM.
				{
				std::_t_strstream gerrStr;
				gerrStr << _T("[genArg: Error. Bad sem arg.]") << std::ends;
				errOut(&gerrStr,false);
				}
				// 05/21/01 AM.
				break;															// 05/21/01 AM.
			}
		break;
	case IANULL:
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[genArg: Bad arg.]") << std::ends;
		errOut(&gerrStr,false);
		}

		break;
	}
}


/********************************************
* FN:		GENNAME
* CR:		05/30/99 AM.
* SUBJ:	Generate name for rule element.
* NOTE:	Class function.
*			07/11/99 AM. Moved here from Ielement.
********************************************/

void Iarg::genName(_TCHAR *name, std::_t_ostream &ofile)
{
if (!name || !*name)
	return;
if (*(name+1))			// Length greater than one.
	{
	ofile << name << std::flush;
	return;
	}

// Handle special single char names.
  if (alphabetic(*name))		// 09/22/99 AM.
	{
	ofile << name << std::flush;
	return;
	}
else if (_istpunct((_TUCHAR)*name))
	{
	ofile << _T("\\") << name;
	return;
	}

switch(*name)
	{
	case '\a': ofile << _T("\\a");		break;
	case '\b': ofile << _T("\\b");		break;
	case '\n': ofile << _T("\\n");		break;
	case '\r': ofile << _T("\\r");		break;
	case '\f': ofile << _T("\\f");		break;
	case '\t': ofile << _T("\\t");		break;
	case '\v': ofile << _T("\\v");		break;
	case '\\': ofile << _T("\\\\");	break;
	case '\0': ofile << _T("\\0");		break;
	case ' ':  ofile << _T("\\ ");		break;
	default:	  ofile << name;		break;
	}
ofile << std::flush;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	If lists of args are of the same form.
********************************************/

bool Iarg::same(Dlist<Iarg> *list1, Dlist<Iarg> *list2)
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


// Compare arg-by-arg.
Delt<Iarg> *darg1 = list1->getFirst();
Delt<Iarg> *darg2 = list2->getFirst();
if (!darg1 && !darg2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Iarg::same: Empty lists.]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}
if (!darg1 || !darg2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Iarg::same: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
for ( ; darg1 && darg2; darg1 = darg1->Right(), darg2 = darg2->Right())
	{
	if (!same(darg1->getData(), darg2->getData()))
		return false;
	}
if (darg1 || darg2)		// Unequal length.
	return false;
return true;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	If args are of the same form.
********************************************/

bool Iarg::same(Iarg *arg1, Iarg *arg2)
{
if (!arg1 && !arg2)
	return true;
if (!arg1 || !arg2)
	return false;

enum Iargtype typ1, typ2;
typ1 = arg1->getType();
typ2 = arg2->getType();
if (typ1 != typ2)
	return false;
switch (typ1)
	{
	case IANULL:
		break;
	case IASTR:
		if (!str_equal(arg1->getStr(), arg2->getStr()))
			return false;
		break;
	case IANUM:
		if (arg1->getNum() != arg2->getNum())
			return false;
		break;
	case IAFLOAT:																// 08/17/01 AM.
		if (arg1->getFloat() != arg2->getFloat())						// 08/17/01 AM.
			return false;														// 08/17/01 AM.
		break;																	// 08/17/01 AM.
	case IAOSTREAM:
		if (arg1->getOstream() != arg2->getOstream())
			return false;
		break;
	case IASEM:
	case IAREF:																	// 05/26/02 AM.
		return RFASem::same(arg1->getSem(), arg2->getSem());		// 02/16/00 AM.
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Iarg::same: Unhandled arg type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}
return true;
}


/********************************************
* FN:		STRINGS_TO_STARR
* CR:		05/06/00 AM.
* SUBJ:	Convert string args to an array.
* ASS:	arg list is string types only.
********************************************/

Starr *Iarg::strings_to_starr(Dlist<Iarg> *list)
{
if (!list)
	return 0;
Delt<Iarg> *delt = list->getFirst();
if (!delt)
	return 0;
int len = list->getLength();
Starr *starr = new Starr(len);
_TCHAR **arr = starr->getArr() - 1;	// One BEFORE array of strings.
Iarg *arg;
for (; delt; delt = delt->Right())
	{
	arg = delt->getData();
	*++arr = arg->getStr();				// Install string ptr in array.
	}
return starr;
}


/********************************************
* FN:		INTERN_STRINGS
* CR:		07/23/07 AM.
* SUBJ:	Intern strings in a list of args.
* NOTE:	For example when returning an array from an NLP++ function.
********************************************/

bool Iarg::intern_strings
	(
	Dlist<Iarg> *args,
	Parse *parse
	)
{
if (!args)
	return true;
Delt<Iarg> *delt;
Iarg *iarg;
RFASem *sem;
_TCHAR *str;
_TCHAR *x;
for (delt = args->getFirst(); delt; delt = delt->Right())
	{
	iarg = delt->getData();
	switch (iarg->getType())
		{
		case IASTR:
			// For now, intern any string(s).
			str = iarg->getStr();
			if (str && *str)
				{
				parse->internStr(str,/*UP*/x);
				iarg->setStr(x);	// Install the interned string.
				}
			break;
		case IASEM:
			sem = iarg->getSem();
			if (sem->getType() != RSSTR && sem->getType())
				break;
			// For now, intern any string(s).
			str = sem->getName();
			if (str && *str)
				{
				parse->internStr(str,/*UP*/x);
				sem->setName(x);	// Install the interned string.
				}
			break;
		}
	}
return true;
}