/*******************************************************************************
Copyright (c) 2000-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STARR.H
* FILE:	lite\starr.h
* CR:		05/05/00 AM.
* SUBJ:	A class for arrays of strings.
* NOTE:	To dynamically create and delete arrays of strings.
*			Storing length. NOT using sentinel, so that empty strings can be
*			valid values.
*			User is responsible for managing the strings themselves.  Not owned
*			by Starr object.
*******************************************************************************/

#ifndef STARR_H_
#define STARR_H_

//#include <iostream.h>

class Starr
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, const Starr &);
public:
	Starr(int size = 1);							// Default constructor.
	Starr(_TCHAR *);									// Convert one string to a starr.
	Starr(const Starr &);						// Copy constructor.
	~Starr();										// Destructor.

	const Starr &operator=(const Starr &);	// Assignment.

	// Primitives
	void clear();
	void zero();
	void copy(const Starr *);
	void reset();									// Set strings to 0.

	// Access
	_TCHAR **getArr();
	int getLength();

	// Modify
	void setArr(_TCHAR **);
	void setLength(int);

	// General
	_TCHAR *nth(int);
	_TCHAR **push(_TCHAR *);		// Add string to array.		[UNIMPLEMENTED]
	_TCHAR **rpush(_TCHAR *);	// Add string to end of array.[UNIMPLEMENTED]
	_TCHAR *pop();				// [UNIMPLEMENTED]
	_TCHAR *rpop();				// [UNIMPLEMENTED]
	bool setnth(int, _TCHAR *);

	// Class-wide
	static bool same(Starr *, Starr *);		// Identical content.


private:
	_TCHAR **arr_;									// Array of strings.
	int length_;									// Number of strings.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;							// Count objects currently allocated.
#endif
};

#endif

