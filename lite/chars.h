/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CHARS.H
* FILE:	lite\chars.h
* CR:		11/19/98 AM.
* SUBJ:	Declares for the Chars class.
* NOTE:	This class is strictly to count the character arrays allocated and
*			freed.
*
*******************************************************************************/

#ifndef CHARS_H_
#define CHARS_H_
#include <assert.h>

// Forward references

/********************************************
* CLASS:	CHARS
* CR:		11/19/98 AM.
* SUBJ:	A hash table manager.
********************************************/

class Chars
{
public:
	Chars();				// Default constructor.
	~Chars();			// Destructor.

	// Access

	// Modify

	// General

	// Class-wide
	static _TCHAR *create(long);			// Allocate char array
	static void  destroy(_TCHAR *);		// Delete char array.

private:


#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


#endif
/*************************      END OF FILE      ******************************/
