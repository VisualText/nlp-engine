/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	TSYM.H
* FILE:	lite\tsym.h
* CR:		12/17/98 AM.
* SUBJ:	Template version of Sym class.
*
*******************************************************************************/

#ifndef TSYM_H_
#define TSYM_H_
//#include <iostream.h>
#include <assert.h>
#include <string.h>
#ifdef LINUX
#include <sstream>
#else
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif
#include "u_out.h"		// 01/19/06 AM.
#include "stab.h"

// Forward references

/********************************************
* CLASS:	TSYM
* CR:		12/17/98 AM.
* SUBJ:	A symbol class, representing a string.
********************************************/

template<class TYPE>
class tSym
{
	//friend ostream &operator<<(ostream &, tSym &);

public:
	tSym(_TCHAR * = 0, Stab * = 0);		// Default constructor.
	tSym(_TCHAR *, long, Stab *);			// String with length.
	~tSym();									// Destructor.

	// Access
	_TCHAR *getStr();
	TYPE *getPtr();

	// Modify
	void setStr(_TCHAR *);
	void setPtr(TYPE *);

	// General.

protected:
	_TCHAR *str_;							// POINTER TO STRING TABLE.
	// Can associate dictionary lookup, lexical information
	// with the sym.  Even concepts!

	TYPE *ptr_;							// POINTER TO DATA.

// When code is well debugged and htable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};

/********************************************
* FN:		Special functions for the class
********************************************/

template<class TYPE>
tSym<TYPE>::tSym(_TCHAR *str, Stab *stab)			// Default constructor.
{
// Need to provide a symbol table for interning the string.
if (stab)
	{
	if (!(str_ = stab->addStr(str)))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[tSym: Couldn't create string.]") << std::ends;
		errOut(&gerrStr,false);
		return;
		}
	}
else if (!empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[tSym: Can't intern string without a string table.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
//str_ = str;

ptr_ = 0;			// Default is no data.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

template<class TYPE>
tSym<TYPE>::tSym(_TCHAR *str, long len, Stab *stab)			// Constructor.
{
// Need to provide a symbol table for interning the string.
if (stab)
	{
	if (!(str_ = stab->addStr(str, len)))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[tSym: Couldn't create string.]") << std::ends;
		errOut(&gerrStr,false);
		return;
		}
	}
else if (!empty(str) || (len > 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[tSym: Can't intern string without a string table.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
//str_ = str;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

template<class TYPE>
tSym<TYPE>::~tSym()
{
// We don't free up the string in the string table.

// Be careful here!  User must free up any data not "owned" by the sym.
delete ptr_;

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

template<class TYPE>
_TCHAR *tSym<TYPE>::getStr()	{return str_;}

template<class TYPE>
TYPE *tSym<TYPE>::getPtr() {return ptr_;}

/********************************************
* FN:		Modify Functions
********************************************/

template<class TYPE>
void	tSym<TYPE>::setStr(_TCHAR *x)	{str_ = x;}

template<class TYPE>
void	tSym<TYPE>::setPtr(TYPE *x)	{ptr_ = x;}


/********************************************
* FN:		GETCOUNT
* CR:		12/17/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class TYPE>
int tSym<TYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		12/17/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class TYPE>
void tSym<TYPE>::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active tSym count=") << count_ << std::endl;
	*gout << _T("Active tSym count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active tSym count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif




#endif
/*************************      END OF FILE      ******************************/
