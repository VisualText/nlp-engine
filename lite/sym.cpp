/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SYM.CPP
* FILE:	lite\sym.cpp
* CR:		11/18/98 AM.
* SUBJ:	Class for symbols.
* NOTE:	Using Htab class to make a hashed symbol table.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "io.h"
#include "slist.h"
#include "selt.h"
#include "stab.h"				// 11/19/98 AM.
#include "sym.h"

#ifdef LINUX
template<> int Slist<Sym>::count_ = 0;
template<> int Selt<Sym>::count_ = 0;
#else
int Slist<Sym>::count_ = 0;
int Selt<Sym>::count_ = 0;
#endif

int Sym::count_ = 0;

// BIT FLAGS.				// 01/25/99 AM.
// WARN! Numbers are octal.
static const int bitLOOKED = 01;		// If word has been looked up in dict.
static const int bitKNOWN  = 02;		// If it's on ANY list of known words,names.
static const int bitWORD	= 04;		// If sym represents an English word.
// 02, 04, 010, 020, 040, 0100...

/********************************************
* FN:		Special functions for the class
********************************************/

Sym::Sym(_TCHAR *str, Stab *stab)			// Default constructor.
{
// 11/19/98 AM.  Need to provide a symbol table for interning the string.
if (stab)
	{
	if (!(str_ = stab->addStr(str)))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Sym: Couldn't create string.]") << std::ends;
		errOut(&gerrStr,false);
		return;
		}
	}
else if (!empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Sym: Can't intern string without a string table.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
//str_ = str;

flags_ = 0;				// 01/25/99 AM.
lcsym_ = 0;				// 01/26/99 AM.
use_	 = 0;				// 04/11/06 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
Sym::Sym(_TCHAR *str, long len, Stab *stab)			// Constructor.
{
// 11/19/98 AM.  Need to provide a symbol table for interning the string.
if (stab)
	{
	if (!(str_ = stab->addStr(str, len)))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Sym: Couldn't create string.]") << std::ends;
		errOut(&gerrStr,false);
		return;
		}
	}
else if (!empty(str) || (len > 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Sym: Can't intern string without a string table.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
//str_ = str;
lcsym_ = 0;
flags_ = 0;	// FIX.	// 06/12/06 AM.
use_	 = 0;	// FIX.	// 06/12/06 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Sym::~Sym()
{
// We don't free up the string in the string table.

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Sym &sym)
{
#ifndef UNICODE
output << str(sym.str_);
#else
char *lpstr8;																	// 01/28/06 AM.
u_to_mbcs((LPCWSTR)str(sym.str_), CP_UTF8, (LPCTSTR*&)lpstr8);	// 01/28/06 AM.
output << lpstr8;																// 01/28/06 AM.
u_delete((LPCTSTR*&)lpstr8);												// 01/28/06 AM.
#endif
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

_TCHAR *Sym::getStr()		{return str_;  }
int   Sym::getFlags()	{return flags_;}		// 01/25/99 AM.
Sym  *Sym::getLcsym()	{return lcsym_;}		// 01/26/99 AM.
int	Sym::getUse()		{return use_;}			// 04/11/06 AM.

/********************************************
* FN:		Access Bitflags
********************************************/

bool Sym::isLooked() const			// 01/25/99 AM.
{
if (flags_ & bitLOOKED)
	return true;
return false;
}

bool Sym::isKnown() const			// 01/25/99 AM.
{
if (flags_ & bitKNOWN)
	return true;
return false;
}

bool Sym::isWord() const			// 01/25/99 AM.
{
if (flags_ & bitWORD)
	return true;
return false;
}

/********************************************
* FN:		Modify Functions
********************************************/

void	Sym::setStr(_TCHAR *x)	{str_	  = x;}
void	Sym::setFlags(int x)	{flags_ = x;}	// 01/25/99 AM.
void	Sym::setLcsym(Sym *x){lcsym_ = x;}	// 01/26/99 AM.
void	Sym::setUse(int x)	{use_   = x;}	// 04/11/06 AM.

/********************************************
* FN:		Modify Bitflags
********************************************/

void Sym::setLooked(bool x)					// 01/25/99 AM.
{
if (x)
	flags_ |= bitLOOKED;
else
	flags_ &= (~bitLOOKED);
}

void Sym::setKnown(bool x)					// 01/25/99 AM.
{
if (x)
	flags_ |= bitKNOWN;
else
	flags_ &= (~bitKNOWN);
}

void Sym::setWord(bool x)					// 01/25/99 AM.
{
if (x)
	flags_ |= bitWORD;
else
	flags_ &= (~bitWORD);
}


/********************************************
* FN:		GETCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Sym::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Sym::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Sym count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Sym count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

