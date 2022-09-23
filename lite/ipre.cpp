/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPRE.CPP
* FILE:	lite\ipre.cpp
* CR:		10/24/98 AM.
* SUBJ:	Interpreted/internal pre-action class.
* NOTE:	Reuse Iaction class.  Use Ipre and Ipost to check for valid actions
*			within these subclasses.  Use them to execute these actions properly
*			also.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "io.h"
#include "iarg.h"
#include "ipre.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Delt<Ipre>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Ipre>::count_ = 0;
#else
int Delt<Ipre>::count_ = 0;
int Dlist<Ipre>::count_ = 0;
#endif

/********************************************
* FN:		Special Functions for Ipre Class
* CR:		10/24/98 AM.
********************************************/

Ipre::Ipre(							// Default constructor.
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa,				// Action arguments.
	int start,						// First rule elt covered.
	int end							// Last rule elt covered.
	)
	: Iaction(nn, aa)				// Init base class.
{
start_ = start;
end_	 = end;
}

/*******************************************/

Ipre::~Ipre()
{
}

/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Ipre::Ipre(Ipre &orig)
: Iaction(orig)			// Call parent class' copy constructor.
{
zero();
Ipre::copy(&orig);
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Ipre &Ipre::operator=(const Ipre &fm)
{
Ipre *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ipre object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

// Let the parent do its assignments.
to->Iaction::operator =(fm);

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}

/*******************************************/
/*******************************************/
void Ipre::clear()
{
// 07/02/99 AM.
// WARN: DOESN'T CLEAR FOR THE PARENT CLASS.
}

void Ipre::zero()
{
// WARN: DOESN'T ZERO FOR THE PARENT CLASS.

start_ = 0;
end_ = 0;
}

void Ipre::copy(const Ipre *orig)
{
// WARN: DOESN'T COPY THE PARENT CLASS.
Ipre *dest;
dest = this;

dest->start_ = orig->start_;
dest->end_ = orig->end_;
}

/*******************************************/

#ifdef OLD_
const Ipre &Ipre::operator=(const Ipre &pre)	// Assignment.	// 11/30/98 AM.
{
Ipre *to;
to = this;
if (&pre == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ipre object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}
start_ = pre.start_;
end_   = pre.end_;
_TCHAR *nm;
nm = pre.name;
to->setName(nm);

// Copy the args.  Create a new args object.
Dlist<Iarg> *aa;
aa = new Dlist<Iarg>(*(pre.args));		// Copy constructor.
to->setArgs(aa);

return *this;
}
#endif

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ipre &pre)
{
output << _T("<") << pre.start_ << _T(",") << pre.end_ << _T("> ");
// ADDING SEMICOLON.												// 02/17/00 AM.
output << str(pre.name_) << _T("(") << pre.args_ << _T(");") << std::endl;
return output;
}


std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Ipre> &list)
{
Delt<Ipre> *delt;
Ipre *elt;
delt = list.getFirst();
elt  = delt->getData();
output << *elt << std::endl;
while ((delt = delt->Right()))
	{
	elt = delt->getData();
	output << *elt << std::endl;
	}
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

int Ipre::getStart()	{return start_;}
int Ipre::getEnd()	{return end_;}

/********************************************
* FN:		Modify Functions
********************************************/

void Ipre::setStart(int x)	{start_	= x;}
void Ipre::setEnd(int x)	{end_		= x;}


/********************************************
* FN:		GENPRES
* CR:		07/06/99 AM.
* SUBJ:	Generate pre actions to a rules file.
*********************************************/

void Ipre::genPres(
	Dlist<Ipre> *pres,
	std::_t_ostream &ofile
	)
{
if (!pres)
	return;

Delt<Ipre> *dpre;

for (dpre = pres->getFirst(); dpre; dpre = dpre->Right())
	{
	genPre(dpre->getData(), ofile);
	ofile << std::endl;
	}
}


/********************************************
* FN:		GENPRE
* CR:		07/06/99 AM.
* SUBJ:	Generate pre to a rules file.
*********************************************/
void Ipre::genPre(
	Ipre *pre,
	std::_t_ostream &ofile
	)
{
if (!pre)
	return;

ofile	<< _T("<")
		<< pre->getStart()
		<< _T(",")
		<< pre->getEnd()
		<< _T(">\t")
		<< std::flush;

// Rest is same as Iaction::genAction().
ofile << pre->getName() << _T("(") << std::flush;
if (pre->getArgs())
	Iarg::genArgs(pre->getArgs(), _T(", "), ofile);

// ADDING SEMICOLON AFTER PRE ACTION.	// 02/17/00 AM.
ofile << _T(");") << std::flush;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	See if two pre lists are identical in form.
*********************************************/

bool Ipre::same(Dlist<Ipre> *list1, Dlist<Ipre> *list2)
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

// Compare pre-by-pre.
Delt<Ipre> *d1 = list1->getFirst();
Delt<Ipre> *d2 = list2->getFirst();
if (!d1 && !d2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Ipre::same: Empty lists.]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}
if (!d1 || !d2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Ipre::same: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
for ( ; d1 && d2; d1 = d1->Right(), d2 = d2->Right())
	{
	if (!same(d1->getData(), d2->getData()))
		return false;
	}

if (d1 || d2)			// Unequal length.
	return false;

return true;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	See if two pres are identical in form.
*********************************************/

bool Ipre::same(Ipre *pre1, Ipre *pre2)
{
// Try super's function first.
if (!Iaction::same(pre1, pre2))
	return false;

if (!pre1 && !pre2)
	return true;
if (!pre1 || !pre2)
	return false;

if (pre1->getStart() != pre2->getStart())
	return false;
if (pre1->getEnd() != pre2->getEnd())
	return false;

return Iaction::same(pre1, pre2);
}


/*************************      END OF FILE      ******************************/
