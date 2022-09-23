/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STRING.H
* FILE:	lite\string.h
* CR:		10/28/98 AM.
* SUBJ:	A simple string class.
* NOTE:	Taken from Dietel&Dietel pp 454-458.
*			Want to eventually use a large string table (one shot allocation).
*
*******************************************************************************/

#ifndef STRING_H_
#define STRING_H_

//#include <iostream.h>

class String
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, const String &);
	friend std::_t_istream &operator>>(std::_t_istream &, String &);
public:
	String(const _TCHAR * = _T(""));		// Conversion constructor.
											// Also, default constructor.

	String(_TCHAR *, long);			// 10/28/98 AM. Convert chars to string.

	String(const String &);			// Copy constructor.
	~String();							// Destructor.
	const String &operator=(const String &);	// Assignment.
	String &operator+=(const String &);			// Concatenation.
	int operator!() const;							// If string empty.
	int operator==(const String &) const;		// If s1 == s2.
	int operator!=(const String &) const;		// If s1 != s2.
	int operator<( const String &) const;		// If s1 <  s2.
	int operator>( const String &) const;		// If s1 >  s2.
	int operator>=(const String &) const;		// If s1 >= s2.
	int operator<=(const String &) const;		// If s1 <= s2.
	_TCHAR &operator[](int);				// Get char reference.
	String operator()(int, int);		// Get substring.
	int getLength() const;				// Get string length.
private:
	_TCHAR *sptr_;							// Ptr to start of string.
	int length_;							// String length.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};

#endif

