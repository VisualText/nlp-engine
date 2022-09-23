/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STRING.CPP
* FILE:	lite\string.cpp
* CR:		10/28/98 AM.
* SUBJ:	A simple string class.
* NOTE:	Taken from Dietel&Dietel pp 454-458.
*			Want to eventually use a large string table (one shot allocation).
*			UNUSED. Don't believe this class is ever used.	01/08/07 AM.
*
*******************************************************************************/

#include "StdAfx.h"
//#include <string.h>
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "chars.h"
#include "string.h"

int String::count_ = 0;

// Conversion constructor: Convert char * to String.
String::String(const _TCHAR *s)
{
*gout << _T("Conversion constructor: ") << s << std::endl;
length_ = _tcsclen(s);					// Compute length.
//sptr_	= new char[length_ + 1];	// Allocate storage.
sptr_ = Chars::create(length_ + 1);
assert(sptr_ != 0);					// Ensure memory allocated.
_tcscpy(sptr_, s);						// Copy literal to object.

#ifndef STABLE_
++count_;
#endif
}

// 10/28/98 AM. Conversion constructor alt: Convert char * with length.
String::String(_TCHAR *s, long len)
{
length_ = len;								// Get length of substring.
//sptr_ = new char[length_ + 1];		// Allocate storage.
sptr_ = Chars::create(length_ + 1);
assert(sptr_ != 0);						// Ensure memory allocated.
_tcsnccpy(sptr_, s, length_);			// Copy substring.
sptr_[length_] = '\0';					// Terminate string.

#ifndef STABLE_
++count_;
#endif
}

// Copy constructor
String::String(const String &copy)
{
*gout << _T("Copy constructor: ") << copy.sptr_ << std::endl;
length_ = copy.length_;				// Copy length.
//sptr_ = new char[length_ + 1];	// Allocate storage.
sptr_ = Chars::create(length_ + 1);
assert(sptr_ != 0);					// Ensure memory allocated.
_tcscpy(sptr_, copy.sptr_);			// Copy string.

#ifndef STABLE_
++count_;
#endif
}


// Destructor
String::~String()
{
*gout << _T("Destructor: ") << sptr_ << std::endl;
//delete [] sptr_;						// Reclaim string.
Chars::destroy(sptr_);

#ifndef STABLE_
--count_;
#endif
}

/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int String::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void String::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active String count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active String count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


// Overloaded '=' operator.  Avoids self-assignment.
const String &String::operator=(const String &right)
{
*gout << _T("Operator= called.") << std::endl;
if (&right != this)					// Avoid self-assignment.
	{
	//delete [] sptr_;					// Reclaim string.
	Chars::destroy(sptr_);
	//sptr_ = new char[length_ + 1];// Allocate memory.
	sptr_ = Chars::create(right.length_ + 1);	// FIX. 07/01/99 AM.
	assert(sptr_ != 0);				// Ensure memory allocated.
	_tcscpy(sptr_, right.sptr_);	// Copy string.
	}
else
	*gout << _T("Attempted assignment of string to itself.") << std::endl;
return *this;				// Enables concatenated strings.
}

// Concatenate right operand to this object and store in this object.
String &String::operator+=(const String &right)
{
_TCHAR *tempPtr = sptr_;					// For deleting.
length_ += right.length_;				// New string length.
//sptr_ = new char[length_ + 1];		// Create space for new string.
sptr_ = Chars::create(length_ + 1);
assert(sptr_ != 0);						// Ensure memory allocated.

_tcscpy(sptr_, tempPtr);					// Left part of new string.
_tcscat(sptr_, right.sptr_);			// Right part of new string.
//delete [] tempPtr;						// Reclaim old space.
Chars::destroy(tempPtr);

return *this;							// Enables concatenated calls.
}

// If string empty.
int String::operator !() const { return length_ == 0; }

// If this string equals right string.
int String::operator==(const String &right) const
{
return _tcscmp(sptr_, right.sptr_) == 0;
}

// If this string not equal right string.
int String::operator !=(const String &right) const
{
return _tcscmp(sptr_, right.sptr_) != 0;
}

// If this string less than right string.
int String::operator <(const String &right) const
{
return _tcscmp(sptr_, right.sptr_) < 0;
}


// If this string greater than right string.
int String::operator >(const String &right) const
{
return _tcscmp(sptr_, right.sptr_) > 0;
}


// If this string greater-equal than right string.
int String::operator >=(const String &right) const
{
return _tcscmp(sptr_, right.sptr_) >= 0;
}


// If this string less-equal than right string.
int String::operator <=(const String &right) const
{
return _tcscmp(sptr_, right.sptr_) <= 0;
}

// Return a reference to a char in a string.
_TCHAR &String::operator [](int subscript)
{
// First test for subscript out of range.
assert(subscript >= 0 && subscript < length_);
return sptr_[subscript];			// Creates lvalue.
}


// Return substring beginning at index, with length subLength
// as a reference to a String object.
String String::operator()(int index, int subLength)
{
// Ensure index is in range and substring length >= 0.
assert(index >= 0 && index < length_ && subLength >= 0);

String sub;								// Empty string.

// Determine length of substring.
if ((subLength == 0) || (index + subLength > length_))
	sub.length_ = length_ - index + 1;
else
	sub.length_ = subLength + 1;

// Allocate memory for substring.
delete sub.sptr_;						// Delete char from object.
								// Because default constructor creates a one-char
								// buffer holding '\0'.
//sub.sptr_ = new char[sub.length_];
sub.sptr_ = Chars::create(sub.length_);
assert(sub.sptr_ != 0);				// Ensure space allocated.

// Copy substring into new string.
_tcsnccpy(sub.sptr_, &sptr_[index], sub.length_);
sub.sptr_[sub.length_] = '\0';		// Terminate new string.

return sub;							// Return copy of sub string.
}


// Return string length.
int String::getLength() const { return length_; }

// Overloaded output operator.
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, const String &s)
{
#ifndef UNICODE
output << s.sptr_;
#else
char *lpstr8;																	// 01/28/06 AM.
u_to_mbcs((LPCWSTR)s.sptr_, CP_UTF8, (LPCTSTR*&)lpstr8);			// 01/28/06 AM.
output << lpstr8;																// 01/28/06 AM.
u_delete((LPCTSTR*&)lpstr8);												// 01/28/06 AM.
#endif
return output;			// Enables concatenation.
}

// Overloaded input operator.
std::_t_istream &operator>>(std::_t_istream &input, String &s)
{
_TCHAR temp[1024];				// Buffer to store input.

//input >> setw(1024) >> temp;							// Upgrade.	// 01/24/01 AM.
input >> temp;													// Upgrade.	// 01/24/01 AM.
s = temp;			// Use String class assignment operator.
return input;			// Enables concatenation.
}




/******************************************************************************/
