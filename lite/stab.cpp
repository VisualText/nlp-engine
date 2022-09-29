/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STAB.CPP
* FILE:	lite\stab.cpp
* CR:		11/18/98 AM.
* SUBJ:	Class for string tables.
* NOTE:	Could have one for permanent strings and one for the current text.
* ASS:	Individual strings should be much smaller than a segment.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "chars.h"
#include "io.h"
#include "stab.h"


int Stab::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

Stab::Stab(					// Default constructor.
	)
{
initStab();

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Stab::~Stab()
{
cleanStab();

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Stab &stab)
{
output << _T("<String table: curr=") << stab.curr_
		 << _T(",last=") << stab.last_
		 << _T(",perm=") << stab.perm_
		 << _T(">") << std::endl;
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

int   Stab::getCurr()	{return curr_;}
int   Stab::getLast()	{return last_;}
_TCHAR *Stab::getPtr()		{return ptr_; }
int	Stab::getPerm()	{return perm_;}

/********************************************
* FN:		Modify Functions
********************************************/

void	Stab::setCurr(int x)		{curr_ = x;}
void	Stab::setLast(int x)		{last_ = x;}
void	Stab::setPtr(_TCHAR *x)	{ptr_  = x;}
void	Stab::setPerm(int x)		{perm_ = x;}


/********************************************
* FN:		GETCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Stab::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Stab::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Stab count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Stab count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		General Functions
********************************************/

/********************************************
* FN:		ADDSTR
* CR:		11/18/98 AM.
* SUBJ:	Add a string to the string table.
* NOTE:	Update string table management appropriately.
********************************************/

_TCHAR *Stab::addStr(_TCHAR *str)
{
long len;

if (empty(str))
	return 0;

len = _tcsclen(str);

return addStrNC(str, len);
}

/*******************************************/

_TCHAR *Stab::addStr(_TCHAR *str, long len)
{
if (empty(str))
	return 0;

if (len <= 0)
	return 0;

return addStrNC(str, len);
}

// VARIANT. Get a buffer of given length from string table.		// 05/26/02 AM.
_TCHAR *Stab::addStr(long len)												// 05/26/02 AM.
{
return addStrNC(len);
}


/********************************************
* FN:		ADDSTRNC
* CR:		11/18/98 AM.
* SUBJ:	Add a string to the string table.
* NOTE:	Update string table management appropriately.
*			NONCHECKING VERSION.  Assumes str, len are ok values.
*			String may not be terminated, so use length arg.
********************************************/

_TCHAR *Stab::addStrNC(_TCHAR *str, long len)
{
if (len >= STAB_SEG_SIZE)		// Whopper string.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Add string: String length ") << len
		  << _T(" is too large for table.") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// If string too large for current segment, create a new one.
if (len >= currSize())
	{
	if (!nextSeg())	// Create new segment.
		return 0;
	}

// String will fit in current segment.
_TCHAR *save;
save = ptr_;					// Save starting point of string.

// Copy the string to the current string table buffer.
long ii;
for (ii = 1; ii <= len; ++ii)
	*ptr_++ = *str++;
*ptr_++ = '\0';				// Terminate string.

// If string exactly filled current segment, flag this.
if (len + 1 == currSize())
	ptr_ = 0;					// Notify that this segment is full.

return save;
}


// VARIANT.  Allocate buffer in string table.						// 05/26/02 AM.
_TCHAR *Stab::addStrNC(long len)
{
if (len >= STAB_SEG_SIZE)		// Whopper string.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Add string: String length ") << len
		  << _T(" is too large for table.") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// If string too large for current segment, create a new one.
if (len >= currSize())
	{
	if (!nextSeg())	// Create new segment.
		return 0;
	}

// String will fit in current segment.
_TCHAR *save;
save = ptr_;					// Save starting point of string.

*ptr_ = '\0';	// Zero out, in case some fool tries to use it.

// Set the string table pointer appropriately.
ptr_ += len;

// If string exactly filled current segment, flag this.
if (len + 1 == currSize())
	ptr_ = 0;					// Notify that this segment is full.

return save;
}


/********************************************
* FN:		CURRSIZE
* CR:		11/18/98 AM.
* SUBJ:	Get remaining unused size of current segment.
********************************************/

long Stab::currSize()
{
if (!ptr_)
	return 0;			// Current segment is full.

_TCHAR *start;
start = seg_[curr_];	// Start address of current segment.
return STAB_SEG_SIZE - ((long) ptr_ - (long) start);
}


/********************************************
* FN:		NEXTSEG
* CR:		11/18/98 AM.
* SUBJ:	Get remaining unused size of current segment.
********************************************/

bool Stab::nextSeg()
{
if (curr_ < last_)
	{
	// Reuse a segment already allocated.
	++curr_;
	ptr_ = seg_[curr_];	// First empty space in new segment.
	}
else			// Allocate a new segment.
	return allocSeg();
return true;
}


/********************************************
* FN:		ALLOCSEG
* CR:		11/18/98 AM.
* SUBJ:	Create a new segment.
********************************************/

bool Stab::allocSeg()
{
if (curr_ != last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[allocSeg: Called incorrectly.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

if (curr_ + 1 >= STAB_SEG_MAX)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[String table is full.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

_TCHAR *seg;
//seg = new char[STAB_SEG_SIZE];
seg = Chars::create(STAB_SEG_SIZE);		// 11/19/98 AM.
if (!seg)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[String table: Can't allocate more space.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

++curr_;
++last_;
seg_[curr_] = ptr_ = seg;
return true;
}


/********************************************
* FN:		INITSTAB
* CR:		11/18/98 AM.
* SUBJ:	Create a string table.
********************************************/

bool Stab::initStab()
{
perm_ = curr_ = last_ = -1;
ptr_ = 0;

int ii;
for (ii = 1; ii <= STAB_INIT_SEGS; ++ii)
	if (!allocSeg())
		return false;
return true;
}


/********************************************
* FN:		CLEANSTAB
* CR:		11/18/98 AM.
* SUBJ:	Deallocate the entire string table.
********************************************/

bool Stab::cleanStab()
{
int ii;

for (ii = 0; ii <= last_; ++ii)
	// delete seg_[ii];					// 11/19/98 AM.
	Chars::destroy(seg_[ii]);			// 11/19/98 AM.
perm_ = curr_ = last_ = -1;
ptr_ = 0;
return true;
}


/********************************************
* FN:		RESETSTAB
* CR:		11/18/98 AM.
* SUBJ:	Reset the string table, keeping the permanent part.
* ASS:	If a permanent part has been assigned, then there
*			exists at least one nonpermanent segment.
********************************************/

void Stab::resetStab()
{
if (perm_ == -1)
	{
	// No permanent part assigned.
	curr_ = 0;
	ptr_ = seg_[curr_];
	}
else		// There is a permanent part.
	{
	if (perm_ >= last_)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[resetStab: Internal error.]") << std::ends;
		errOut(&gerrStr,false);
		return;
		}
	curr_ = perm_ + 1;
	ptr_ = seg_[curr_];
	}
}


/********************************************
* FN:		PRETTYSTAB
* CR:		11/18/98 AM.
* SUBJ:	Pretty print the string table.
********************************************/

void Stab::prettyStab()
{
int ii;
_TCHAR *tmp, *end;

*gout << _T("String table:\n")
	  << _T("-------------")
	  << std::endl;

// For all but the last segment.
for (ii = 0; ii < curr_; ++ii)
	{
	end = seg_[ii] + STAB_SEG_SIZE;
	for (tmp = seg_[ii]; tmp != end; ++tmp)
		*gout << pretty_char(*tmp);
	}

// For the last segment.
for (tmp = seg_[curr_]; tmp != ptr_; ++tmp)
	*gout << pretty_char(*tmp);
*gout << std::endl;
}


/********************************************
* FN:		TEST
* CR:		11/18/98 AM.
* SUBJ:	Minimal test of string table.
* NOTE:	Could parameterize the sizes of things,
*			rather than having global defines.
********************************************/

void Stab::test()
{
*gout << _T("[TESTING STRING TABLE:]") << std::endl;

Stab table;

table.prettyStab();
table.addStr(_T("hello"));
table.prettyStab();
table.addStr(_T("goodbye"));
table.prettyStab();
}


/*************************      END OF FILE      ******************************/
