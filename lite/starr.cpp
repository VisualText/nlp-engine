/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STARR.CPP
* FILE:	lite\starr.cpp
* CR:		05/05/00 AM.
* SUBJ:	A class for arrays of strings.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "chars.h"
#include "starr.h"

int Starr::count_ = 0;

// Default constructor.
Starr::Starr(int size)
{
if (size <= 0)		// Bad size.
	length_ = 1;		// Minimum allowed size.
else
	length_ = size;

// Create the array of strings.
arr_ = new _TCHAR * [length_];
reset();

#ifndef STABLE_
++count_;
#endif
}

// Convert string to a starr object.
Starr::Starr(_TCHAR *str)
{
length_ = 1;
arr_ = new _TCHAR * [1];
assert (arr_ != 0);

#ifndef STABLE_
++count_;
#endif
}

// Copy constructor.
// WARN: DOES NOT COPY THE STRINGS!
// (Assumes they are interned or permanent).
Starr::Starr(const Starr &orig)
{
copy(&orig);
#ifndef STABLE_
++count_;
#endif
}

// Destructor.
Starr::~Starr()
{
clear();
#ifndef STABLE_
--count_;
#endif
}



// Primitives
void Starr::clear()
{
delete [] arr_;
}

void Starr::zero()
{
length_ = 0;
arr_ = 0;
}


void Starr::copy(const Starr *orig)
{
length_ = orig->length_;
arr_ = new _TCHAR * [length_];
int ii = length_ + 1;						// one LARGER than array.
_TCHAR **ptr1 = arr_ - 1;						// one BEFORE array.
_TCHAR **ptr2 = orig->arr_ - 1;				// one BEFORE array.
while (--ii)
	*++ptr1 = *++ptr2;
}

// Set strings to zero.
void Starr::reset()
{
int ii = length_ + 1;	// one LARGER than array.
_TCHAR **ptr = arr_ - 1;	// one BEFORE array.
while (--ii)
	*++ptr = 0;
}



/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Starr::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Starr::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Starr count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Starr count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


// Overloaded '=' operator.  Avoids self-assignment.
const Starr &Starr::operator=(const Starr &right)
{
if (&right != this)					// Avoid self-assignment.
	{
	clear();								// Wipe out left side.
	copy(&right);						// Copy from right side.
	}
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Attempted assignment of starr to itself.") << std::ends;
	errOut(&gerrStr,false);
	}

return *this;				// Enables concatenated strings.
}


/////////////////
// ACCESS
/////////////////

_TCHAR **Starr::getArr()	{return arr_;   }
int Starr::getLength()	{return length_;}

/////////////////
// MODIFY
/////////////////
void	Starr::setArr(_TCHAR **x)	{arr_    = x;}
void	Starr::setLength(int x)	{length_	= x;}


// Overloaded output operator.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, const Starr &s)
{
int ii = s.length_;
_TCHAR **ptr = s.arr_;
output << *ptr;
while (--ii)
	output << _T(", ") << *++ptr;
return output;			// Enables concatenation.
}

////////////
// GENERAL
////////////


/********************************************
* FN:		NTH
* CR:		05/05/00 AM.
* SUBJ:	Get nth string of starr.
* RET:	string.
********************************************/

_TCHAR *Starr::nth(int num)
{
if (num <= 0 || num >= length_)
	return 0;
return arr_[num];
}


/********************************************
* FN:		SETNTH
* CR:		05/05/00 AM.
* SUBJ:	Set nth string of starr.
* RET:	true if ok.
********************************************/

bool Starr::setnth(int num, _TCHAR *str)
{
if (num <= 0 || num >= length_)
	return false;
arr_[num] = str;
return true;
}


/********************************************
* FN:		SAME
* CR:		05/05/00 AM.
* SUBJ:	Set nth string of starr.
* RET:	true if ok.
********************************************/

bool Starr::same(Starr *s1, Starr *s2)
{
if (!s1 && !s2)
	return true;
if (!s1 || !s2)
	return false;
int l1 = s1->length_;
int l2 = s2->length_;
if (l1 != l2)
	return false;
_TCHAR **a1 = s1->arr_ - 1;
_TCHAR **a2 = s2->arr_ - 1;
int len = l1 + 1;
while (--len)
	{
	if (*++a1 == *++a2)		// Same pointer.
		;
	else if (_tcscmp(*a1, *a2))
		return false;
	}
return true;
}

/******************************************************************************/
