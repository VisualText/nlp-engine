/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPOST.CPP
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
#include "node.h"	// 07/07/03 AM.
#include "lite/Auser.h"				// 07/07/03 AM.
#include "inline.h"															// 09/26/01 AM.
#include "io.h"
#include "tree.h"			// 02/26/01 AM.
#include "iarg.h"			// 02/26/01 AM.
#include "arg.h"			// 12/28/99 AM.
#include "ipost.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Ipost>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Ipost>::count_ = 0;
#else
int Dlist<Ipost>::count_ = 0;
int Delt<Ipost>::count_ = 0;
#endif


/********************************************
* FN:		Special Functions for Ipost Class
* CR:		10/24/98 AM.
********************************************/

Ipost::Ipost(						// Default constructor.
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa				// Action arguments.
	)
	: Iaction(nn, aa)				// Init base class.
{
}

/*******************************************/

Ipost::~Ipost()
{
}


/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Ipost::Ipost(Ipost &orig)
: Iaction(orig)			// Call parent class' copy constructor.
{
zero();
Ipost::copy(&orig);
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Ipost &Ipost::operator=(const Ipost &fm)
{
Ipost *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ipost object to itself.]") << std::ends;
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
void Ipost::clear()
{
// 07/02/99 AM.
// WARN: DOESN'T CLEAR FOR THE PARENT CLASS.
}

void Ipost::zero()
{
// WARN: DOESN'T ZERO FOR THE PARENT CLASS.
}

void Ipost::copy(const Ipost *orig)
{
// WARN: DOESN'T COPY THE PARENT CLASS.
Ipost *dest;
dest = this;
}

/*******************************************/

/*******************************************/

#ifdef IPOST_OVERRIDE_
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iaction &action)
{
#ifndef UNICODE
output << str(action.name) << _T("(") << action.args << _T(")") << std::endl;
#else
char *lpstr8;																	// 01/28/06 AM.
u_to_mbcs((LPCWSTR)str(action.name_), CP_UTF8, (LPCTSTR*&)lpstr8);// 01/28/06 AM.
output << lpstr8;																// 01/28/06 AM.
u_delete((LPCTSTR*&)lpstr8);												// 01/28/06 AM.
output << _T("(") << action.args << _T(")") << std::endl;				// 01/28/06 AM.
#endif

return output;
}
#endif

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Ipost> &list)
{
Delt<Ipost> *delt;
Ipost *elt;
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
* CR:		10/24/98 AM.
********************************************/

/********************************************
* FN:		Modify Functions
* CR:		10/24/98 AM.
********************************************/


///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		makeDelt
* CR:		11/04/98 AM.
* SUBJ:	Create a list element with appropriate data.
* NOTE:	Must be a more elegant way to initialize the elt.
********************************************/

Delt<Ipost> *Ipost::makeDelt(
	_TCHAR *nn,				// Action name.
	Dlist<Iarg> *aa		// Action arguments.
	)
{
Delt<Ipost> *delt;
//static Ipost obj;		// 07/02/99 AM.

Ipost *obj;					// 07/02/99 AM.
obj = new Ipost(nn,aa);

// Hack. Keep refilling same static Iaction, because it will be copied.
//obj.name	= nn;
//obj.args	= aa;

delt = new Delt<Ipost>(obj);

//obj.args = 0;

return delt;
}


/********************************************
* FN:		makeDlist
* CR:		11/04/98 AM.
* SUBJ:	Create a list with appropriate data.
* NOTE:	Class member function.
*			(Can be called without an instance existing).
********************************************/

Dlist<Ipost> *Ipost::makeDlist(
	_TCHAR *nn,				// Action name.
	Dlist<Iarg> *aa		// Action arguments.
	)
{
Delt<Ipost> *delt;
delt = makeDelt(nn, aa);
return new Dlist<Ipost>(delt);
}


/********************************************
* FN:		addDelt
* CR:		11/04/98 AM.
* SUBJ:	Add new element to given list.
* NOTE:	Must be a more elegant way to initialize the elt.
*			Class member function. Need not be called with object.
********************************************/

Delt<Ipost> *Ipost::addDelt(
	Dlist<Ipost> *list,		// List to add element to.
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa				// Action arguments.
	)
{
Delt<Ipost> *delt;
//static Ipost obj;			// 07/02/99 AM.

Ipost *obj;						// 07/02/99 AM.
obj = new Ipost(nn,aa);		// 07/02/99 AM.

// Hack. Keep refilling same static Ipost, because it will be copied.
//obj.name	= nn;
//obj.args	= aa;

delt = new Delt<Ipost>(obj);

//obj.args = 0;		// 11/17/98 AM.

list->rpush(delt);
return delt;			// In case anyone wants it.
}

/********************************************
* FN:		unpackGroup
* CR:		12/28/99 AM.
* SUBJ:	Unpack the GROUP post action.
* RET:	True if ok, else false.
* NOTE:	Someday, build a variant with less error checking! ;-)
********************************************/

bool Ipost::unpackGroup(
	Ipost *act,
	/*UP*/
	int &first,
	int &last,
	_TCHAR* &label
	)
{
first = last = -1;
label = 0;
if (!act)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Given null action.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
if (strcmp_i(act->getName(), _T("group")))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Action is '") << act->getName()
			<< _T("', not GROUP.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Dlist<Iarg> *args = act->getArgs();
if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: No args in GROUP action.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Delt<Iarg> *darg_first = args->getFirst();
if (!darg_first)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Empty args list in GROUP action.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Delt<Iarg> *darg_last = darg_first->Right();
if (!darg_last)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Missing args in GROUP action.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Delt<Iarg> *darg_label = darg_last->Right();
if (!darg_label)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Missing arg in GROUP action.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
if (darg_label->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Too many args in GROUP action.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Got all the args ok, now check on them.
// Not converting types here.  Want this done at rules-file parse time.
Iarg *arg_first = darg_first->getData();
if (arg_first->getType() != IANUM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Start arg not numeric.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Iarg *arg_last = darg_last->getData();
if (arg_last->getType() != IANUM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: End arg not numeric.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Iarg *arg_label = darg_label->getData();
if (arg_label->getType() != IASTR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Label arg not string type.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Types are ok, now get values.
_TCHAR *str = arg_label->getStr();
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Label string is empty.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
int num1 = arg_first->getNum();
int num2 = arg_last->getNum();
if (num1 <= 0 || num2 <= 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[unpackGroup: Range must be positive integers.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Everything A-OK!
label = str;
first = num1;
last = num2;
return true;
}

/*************************      END OF FILE      ******************************/
