/*******************************************************************************
Copyright (c) 2009-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	REGEXP.CPP
* FILE:	lite\regexp.cpp
* CR:		03/24/09 AM.
* SUBJ:	Simple regular expression matching.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/lite.h"
#include "lite/global.h"
#include "dlist.h"
#include "lite/iarg.h"
#include "std.h"
#include "inline.h"
#include "chars.h"
#include "node.h"

#include "regexp.h"


int Regexp::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

Regexp::Regexp()					// Default constructor.
{
}


/*******************************************/

Regexp::~Regexp()
{
Chars::destroy(xbuf_);								// Destroy temporary buffer.
xbuf_ = 0;
}


/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

/********************************************
* FN:		Modify Functions
********************************************/


/********************************************
* FN:		INIT
* CR:		03/24/09 AM.
* NOTE:	Initialize pattern array.
********************************************/

bool Regexp::init(_TCHAR *xstr)
{
if (!xstr || !*xstr)
	return false;

xlen_ = _tcsclen(xstr);
xbuf_ = Chars::create(xlen_+1);
_tcscpy(xbuf_, xstr);					// Copy to buffer.

// Init first element of pattern array to zeros.
elts_ = 0;
// elt_[0].alpha = '\0';
elt_[0].alpha = "";	// 09/26/19 AM.
elt_[0].qms = 0;
elt_[0].stars = 0;

// Traverse the pattern string copy.
// Collect wild and qm counts, followed by a single alpha.
// Final element may omit an alpha.
// Point to each alpha string.  Terminate each alpha string with 0.
_TCHAR *x = xbuf_;
while (*x)
	{
	switch (*x)
		{
		case '*':
			if (elt_[elts_].alpha)
				{
				// New elt.
				++elts_;
				// elt_[elts_].alpha = '\0';
				elt_[elts_].alpha = "";	// 09/26/19 AM.
				elt_[elts_].qms = 0;
				elt_[elts_].stars = 1;
				}
			else
			  ++(elt_[elts_].stars);
			*x = '\0';	// To terminate any prior alpha.
			break;
		case '?':
			if (elt_[elts_].alpha)
				{
				// New elt.
				++elts_;
				// elt_[elts_].alpha = '\0';
				elt_[elts_].alpha = "";	// 09/26/19 AM.
				elt_[elts_].qms = 1;
				elt_[elts_].stars = 0;
				}
			else
			  ++(elt_[elts_].qms);
			*x = '\0';	// To terminate any prior alpha.
			break;
		default:	// Regular character.
			if (!(elt_[elts_].alpha))
				elt_[elts_].alpha = x;	// Point to start of alpha.
			break;
		}
	++x;	// Next char.
	}

return true;
}

/********************************************
* FN:		MATCH
* CR:		03/24/09 AM.
* NOTE:	Recursive match of pattern.
********************************************/

bool Regexp::match(
	int count,	// Zero based counter for where we are in pattern.
	_TCHAR *str	// Remaining text to be matched.
	)
{
if (count < 0)
	return false;	// Bad data check.
if (count > elts_)
	{
	if (!str || !*str)
		return true;	// Presumably reached end of pattern.
	return false;
	}

// Match the current element.

// No string left to match.
if (!str || !*str)
	{
	// String gone, so current element should be wild or nil.
	if (elt_[count].alpha)
		return false;
	if (elt_[count].qms)
		return false;
	return true;	// If wild or empty, it's ok.
	}

// If there are question marks, skip same number of chars.
int ii = 0;
while (ii < elt_[count].qms)
	{
	++ii;
	if (!*++str)
		{
		// Ran out of string.
		if (ii < elt_[count].qms || elt_[count].alpha)
			return false;
		return true;
		}
	}

// Only the last element may omit an alpha.
if (!(elt_[count].alpha))
	{
	if (elt_[count].stars)
		return true;
	if (!str || !*str)
		return true;
	return false;
	}

bool wild = (elt_[count].stars > 0);

// Now we're at the alpha.
int slen = _tcslen(str);
int xlen = _tcslen(elt_[count].alpha);

// If not wild, must match the alpha now.
if (!wild)
	{
	if (slen < xlen)
		return false;	// String not long enough.
	if (_tcsncmp(str,elt_[count].alpha,xlen))	// Nonmatch.
		return false;
	// ALPHA MATCH.
	// Set up to match the rest of the elements.
	return match(1+count,str+xlen);
	}

// Wild.
// Try to match it.  If succeeeded, match next element.
bool ok = false;
if (slen < xlen)
	return false;	// String not long enough.
if (!_tcsncmp(str,elt_[count].alpha,xlen))	// Match.
	{
	ok = match(1+count,str+xlen);
	if (ok)
		return true;
	// Since we are a wildcard, we get to move right...
	}

// If failed, move right iteratively, repeat match.
while (*++str)
	{
	--slen;	// String is shorter.
	if (slen < xlen)
		return false;
	if (!_tcsncmp(str,elt_[count].alpha,xlen))	// Match.
		{
		ok = match(1+count,str+xlen);
		if (ok)
			return true;
		// Since we are a wildcard, we get to move right...
		}
	}

return false;
}


/********************************************
* FN:		MATCHI
* CR:		03/26/09 AM.
* NOTE:	Recursive match of pattern. (Case insensitive)
********************************************/

bool Regexp::matchi(
	int count,	// Zero based counter for where we are in pattern.
	_TCHAR *str	// Remaining text to be matched.
	)
{
if (count < 0)
	return false;	// Bad data check.
if (count > elts_)
	{
	if (!str || !*str)
		return true;	// Presumably reached end of pattern.
	return false;
	}

// Match the current element.

// No string left to match.
if (!str || !*str)
	{
	// String gone, so current element should be wild or nil.
	if (elt_[count].alpha)
		return false;
	if (elt_[count].qms)
		return false;
	return true;	// If wild or empty, it's ok.
	}

// If there are question marks, skip same number of chars.
int ii = 0;
while (ii < elt_[count].qms)
	{
	++ii;
	if (!*++str)
		{
		// Ran out of string.
		if (ii < elt_[count].qms || elt_[count].alpha)
			return false;
		return true;
		}
	}

// Only the last element may omit an alpha.
if (!(elt_[count].alpha))
	{
	if (elt_[count].stars)
		return true;
	if (!str || !*str)
		return true;
	return false;
	}

bool wild = (elt_[count].stars > 0);

// Now we're at the alpha.
int slen = _tcslen(str);
int xlen = _tcslen(elt_[count].alpha);

// If not wild, must match the alpha now.
if (!wild)
	{
	if (slen < xlen)
		return false;	// String not long enough.
	if (strcmp_ni(str,elt_[count].alpha,xlen))	// Nonmatch.
		return false;
	// ALPHA MATCH.
	// Set up to match the rest of the elements.
	return match(1+count,str+xlen);
	}

// Wild.
// Try to match it.  If succeeeded, match next element.
bool ok = false;
if (slen < xlen)
	return false;	// String not long enough.
if (!strcmp_ni(str,elt_[count].alpha,xlen))	// Match.
	{
	ok = match(1+count,str+xlen);
	if (ok)
		return true;
	// Since we are a wildcard, we get to move right...
	}

// If failed, move right iteratively, repeat match.
while (*++str)
	{
	--slen;	// String is shorter.
	if (slen < xlen)
		return false;
	if (!strcmp_ni(str,elt_[count].alpha,xlen))	// Match.
		{
		ok = match(1+count,str+xlen);
		if (ok)
			return true;
		// Since we are a wildcard, we get to move right...
		}
	}

return false;
}
/********************************************
* FN:		Class-Wide Functions
********************************************/

/**************************************************
*						REGEXP_MATCH
* FUN:	regexp_match
* SUBJ:	See if str matches regexp str.
* CR:		03/23/09 AM.
* RET:	1 = match. 0 = not match.
* NOTE:	Regular expression can include *, ? for now.
*	Case sensitive matching.
*
**************************************************/

bool Regexp::regexp_match(
	_TCHAR *str,
	_TCHAR *xstr	// Regular expression string.
	)
{
if (!str || !*str || !xstr || !*xstr)
	return false;

// Internalize pattern.
Regexp pattern;
bool ok = pattern.init(xstr);
if (!ok)
	return false;

// Recursively match each element of pattern to string.
ok = pattern.match(0,str);

return ok;
}

/**************************************************
*						REGEXPI_MATCH
* FUN:	regexpi_match
* SUBJ:	See if str matches regexp str. (Case insensitive)
* CR:		03/26/09 AM.
* RET:	1 = match. 0 = not match.
* NOTE:	Regular expression can include *, ? for now.
*	Case sensitive matching.
*
**************************************************/

bool Regexp::regexpi_match(
	_TCHAR *str,
	_TCHAR *xstr	// Regular expression string.
	)
{
if (!str || !*str || !xstr || !*xstr)
	return false;

// Internalize pattern.
Regexp pattern;
bool ok = pattern.init(xstr);
if (!ok)
	return false;

// Recursively match each element of pattern to string.
ok = pattern.matchi(0,str);

return ok;
}




/********************************************
* FN:		GETCOUNT
* CR:		11/19/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Regexp::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		03/24/09 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Regexp::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Regexp count=") << count_ << std::endl;
	*gout << _T("Active Regexp count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Regexp count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

/*************************      END OF FILE      ******************************/
