/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IELEMENT.CPP
* FILE:	lite\ielement.cpp
* CR:		10/24/98 AM.
* SUBJ:	Interpreted/internal rule element class.
* NOTE:	11/11/98 AM. If making a list of elements, use Ielt.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"		// 05/19/99 AM.
#include "io.h"
#include "ipair.h"
#include "ielement.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Delt<Ielement>::count_ = 0;
#else
int Delt<Ielement>::count_ = 0;
#endif

// 07/02/99 AM. Removing those statics.
//int Ielement::count_ = -1;			// 11/17/98 AM. Because of static ielt.
int Ielement::count_ = 0;				// 07/02/99 AM.

// Forward references.
class Irule;			// 11/22/98 AM.

/********************************************
* FN:		Special Functions for Class
* CR:		10/24/98 AM.
********************************************/

Ielement::Ielement(				// Default constructor.
	_TCHAR *nn,						// Element name.
	Dlist<Ipair> *prs,			// Pairs.
	Dlist<Iarg> *attrs,			// Attrs.								// 11/07/98 AM.
	long num							// Line num in pass file.			// 08/08/01 AM.
	)
{
name_  = nn;
pairs_ = prs;
attrs_ = attrs;					// 11/07/98 AM.
line_	 = num;						// 08/08/01 AM.

//*gout << "[Ielement: " << (long) this << "]" << std::endl;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ielement::~Ielement()
{
//*gout << "[~Ielement: " << (long) this << "]" << std::endl;

Ielement::clear();			// 07/02/99 AM.

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Ielement::Ielement(Ielement &orig)
{
zero();
Ielement::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Ielement &Ielement::operator=(const Ielement &fm)
{
Ielement *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ielement object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}

/*******************************************/
/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ielement &element)
{
#ifndef UNICODE
output << str(element.name_) << _T(" [");
#else
char *lpstr8;																	// 01/27/06 AM.
u_to_mbcs((LPCWSTR)str(element.name_), CP_UTF8, (LPCTSTR*&)lpstr8);	// 01/27/06 AM.
output << lpstr8;																// 01/27/06 AM.
u_delete((LPCTSTR*&)lpstr8);												// 01/27/06 AM.
output << _T(" [");															// 01/27/06 AM.
#endif

if (element.pairs_)
	output << element.pairs_;
output << _T("]");
return output;
}

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Ielement> &list)
{
Delt<Ielement> *delt;
Ielement *elt;
delt = list.getFirst();
elt  = delt->getData();
output << *elt;
while ((delt = delt->Right()))
	{
	elt = delt->getData();
	output << _T(" ") << *elt;
	}
return output;
}

/*******************************************/
void Ielement::clear()
{
if (pairs_)
	Dlist<Ipair>::DeleteDlistAndData(pairs_);
if (attrs_)
	Dlist<Iarg>::DeleteDlistAndData(attrs_);
}

void Ielement::zero()
{
name_ = 0;
pairs_ = 0;
attrs_ = 0;
line_	 = 0;
}

void Ielement::copy(const Ielement *orig)
{
Ielement *dest;
dest = this;

dest->name_ = orig->name_;

if (orig->pairs_)
	dest->pairs_ = new Dlist<Ipair>(*(orig->pairs_));
if (orig->attrs_)
	dest->attrs_ = new Dlist<Iarg>(*(orig->attrs_));
dest->line_ = orig->line_;
}

/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/24/98 AM.
********************************************/

_TCHAR				*Ielement::getName()		{return name_; }
Dlist<Ipair>	*Ielement::getPairs()	{return pairs_;}
Dlist<Iarg>		*Ielement::getAttrs()	{return attrs_;}	// 11/07/98 AM.
long				 Ielement::getLine()		{return line_;}

/********************************************
* FN:		Modify Functions
* CR:		10/24/98 AM.
********************************************/

void Ielement::setName(_TCHAR *x)				{name_   = x;}
void Ielement::setPairs(Dlist<Ipair> *x)	{pairs_  = x;}
void Ielement::setAttrs(Dlist<Iarg>  *x)	{attrs_ = x; }	// 11/07/98 AM.
void Ielement::setLine(long x)				{line_	= x;}


/********************************************
* FN:		GETCOUNT
* CR:		10/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ielement::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		10/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ielement::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ielement count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ielement count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		General Functions
********************************************/

void Ielement::intern(bool &info)
{
// Derived classes should override this.
}


/********************************************
* FN:		LASTATTR
* CR:		12/27/99 AM. AM.
* SUBJ:	Get the last attr in list, if any.
********************************************/

_TCHAR *Ielement::lastAttr()
{
if (!attrs_)
	return 0;
Delt<Iarg> *delt;
if (!(delt = attrs_->getLast()))
	return 0;
Iarg *arg = delt->getData();
return arg->getStr();
}

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/*************************      END OF FILE      ******************************/
