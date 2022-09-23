/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPAIR.CPP
* FILE:	lite\ipair.cpp
* CR:		10/24/98 AM.
* SUBJ:	Class for rule element's key=val pairs.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "io.h"
#include "ipair.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Ipair>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Ipair>::count_ = 0;
#else
int Dlist<Ipair>::count_ = 0;
int Delt<Ipair>::count_ = 0;
#endif
//int Ipair::count_ = -2;				// Two statics in file.  // 12/07/98 AM.
int Ipair::count_ = 0;					// 07/02/99 AM.

/********************************************
* FN:		Special Functions for Class
********************************************/

Ipair::Ipair(						// Default constructor.
	_TCHAR *kk,						// Key name.
	Dlist<Iarg> *vv				// Values.	11/07/98 AM.
	)
{
key   = kk;
vals_ = vv;							// 11/07/98 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ipair::~Ipair()
{
clear();		// 07/07/99 AM.

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Ipair::Ipair(Ipair &orig)
{
zero();
Ipair::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Ipair &Ipair::operator=(const Ipair &fm)
{
Ipair *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ipair object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Ipair::clear()
{
if (vals_)
	Dlist<Iarg>::DeleteDlistAndData(vals_);
}

void Ipair::zero()
{
key = 0;
vals_ = 0;
}

void Ipair::copy(const Ipair *orig)
{
Ipair *dest;
dest = this;

dest->key = orig->key;

if (orig->vals_)
	dest->vals_ = new Dlist<Iarg>(*(orig->vals_));
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ipair &pairx)
{
output << pairx.key;
if (pairx.vals_)
	output << _T("=") << *(pairx.vals_);		// 11/07/98 AM.
return output;
}

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Delt<Ipair> &delt)
{
Ipair *data;
data = delt.getData();
output << *data;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Ipair> &list)
{
Delt<Ipair> *delt;
Ipair *rr;
delt = list.getFirst();
rr   = delt->getData();
output << *rr;
while ((delt = delt->Right()))
	{
	rr = delt->getData();
	output << _T(" ") << *rr;
	}

return output;
}


/********************************************
* FN:		Access Functions
********************************************/

_TCHAR *Ipair::getKey()	{ return key;   }
Dlist<Iarg> *Ipair::getVals()	{ return vals_; }		// 11/07/98 AM.


/********************************************
* FN:		Modify Functions
********************************************/

void Ipair::setKey(_TCHAR *x)			{ key   = x; }
void Ipair::setVals(Dlist<Iarg> *x)	{ vals_ = x; }		// 11/07/98 AM.


/********************************************
* FN:		GETCOUNT
* CR:		10/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ipair::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		10/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ipair::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ipair count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ipair count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		makeDelt
* CR:		10/24/98 AM.
* SUBJ:	Create a list element with appropriate data.
* NOTE:	Must be a more elegant way to initialize the elt.
********************************************/
//static Ipair pr;			// 07/02/99 AM.

Delt<Ipair> *Ipair::makeDelt(
	_TCHAR *kk,					// Key name.
	Dlist<Iarg> *vv			// Values list.		// 11/07/98 AM.
	)
{
Delt<Ipair> *delt;

Ipair *pr;			// 07/02/99 AM.
pr = new Ipair(kk,vv);

// Hack. Keep refilling same static Ipair, because it will be copied.
//pr.key   = kk;
//pr.vals_ = vv;

delt = new Delt<Ipair>(pr);

//pr.vals_ = 0;

return delt;
}


/********************************************
* FN:		makeDlist
* CR:		10/24/98 AM.
* SUBJ:	Create a list with appropriate data.
* NOTE:	Class member function.
*			(Can be called without an instance existing).
********************************************/

Dlist<Ipair> *Ipair::makeDlist(
	_TCHAR *kk,					// Key name.
	Dlist<Iarg> *vv			// Values list.		// 11/07/98 AM.
	)
{
Delt<Ipair> *delt;
delt = makeDelt(kk, vv);
return new Dlist<Ipair>(delt);
}


/********************************************
* FN:		addDelt
* CR:		10/24/98 AM.
* SUBJ:	Add new element to given list.
* NOTE:	Must be a more elegant way to initialize the elt.
*			Class member function. Need not be called with object.
********************************************/
//static Ipair obj;

Delt<Ipair> *Ipair::addDelt(
	Dlist<Ipair> *list,		// List to add element to.
	_TCHAR *kk,					// Key name.
	Dlist<Iarg> *vv			// Values list.		// 11/07/98 AM.
	)
{
Delt<Ipair> *delt;

Ipair *obj;							// 07/02/99 AM.
obj = new Ipair(kk,vv);			// 07/02/99 AM.

// Hack. Keep refilling same static Ipair, because it will be copied.
//obj.key	 = kk;
//obj.vals_ = vv;					// 11/07/98 AM.

delt = new Delt<Ipair>(obj);

//obj.vals_ = 0;						// 11/17/98 AM.

list->rpush(delt);
return delt;			// In case anyone wants it.
}

/*************************      END OF FILE      ******************************/
