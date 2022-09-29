/*******************************************************************************
Copyright (c) 2009-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	REGEXP.H
* FILE:	lite\regexp.h
* CR:		03/24/09 AM.
* SUBJ:	Regular expression functions.
*
*******************************************************************************/

#ifndef REGEXP_H_
#define REGEXP_H_

// Element consists of 0 or more question marks,
// 0 or more wildcards,
// and 1 alphabetic.
// Single-element pattern with no alphas is possible degenerate case.
typedef struct rx
	{
	_TCHAR *alpha;	// Point to alpha string.
	int stars;	// Count wildcards.
	int qms;	// Count question marks.
	} RX;

// Maximum pattern elements.
#define MAX_RX 100

// Forward references

/********************************************
* CLASS:	REGEXP
* CR:		03/24/09 AM.
* SUBJ:	Regular expression manager.
********************************************/

class Regexp
{
public:
	Regexp();				// Default constructor.
	~Regexp();			// Destructor.

	// Access

	// Modify

	// General
	bool init(_TCHAR *pat_str);  // Initialize pattern string.
	bool match(int,_TCHAR*);
	bool matchi(int,_TCHAR*);

	// Class-wide
//	static _TCHAR *create(long);			// Allocate char array
	static bool regexp_match(
		_TCHAR *str,
		_TCHAR *xstr	// Regular expression string.
		);
	static bool regexpi_match(
		_TCHAR *str,
		_TCHAR *xstr	// Regular expression string.
		);

private:
	struct rx elt_[MAX_RX];	// Array for pattern elements.
	int elts_;	// Count of pattern elements.
	
	_TCHAR *xbuf_;				// Copy of pattern string for modifying.
	int xlen_;


#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


#endif
