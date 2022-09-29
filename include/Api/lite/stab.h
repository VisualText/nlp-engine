/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STAB.H
* FILE:	lite\stab.h
* CR:		11/18/98 AM.
* SUBJ:	Declares for the string table class.
* NOTE:	This will enable use of strings without worrying about allocation
*			and freeing.  In language processing
* MACH:	Because the string tables can get huge, the sizes should be set here
*			based on the machine that the program is running on.
*
*******************************************************************************/

#ifndef STAB_H_
#define STAB_H_
//#include <iostream.h>
#include <assert.h>

//// Global definitions for string tables.

// Size of one string table segment.
//#define STAB_SEG_SIZE	((long) 50000)
// Enlarging segment sizes, to handle huge strings.			// 05/22/03 AM.
#define STAB_SEG_SIZE	(((long) 8192 * (long) 64) + (long) 1)

// Maximum number of segments in string table.
// Upping the limit.														// 08/27/02 AM.
//#define STAB_SEG_MAX		800
#define STAB_SEG_MAX		8000

// The number of segments to allocate when the program starts.
#define STAB_INIT_SEGS	1

// Forward references

/********************************************
* CLASS:	STAB
* CR:		11/18/98 AM.
* SUBJ:	A string table manager.
********************************************/

class Stab
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Stab &);

public:
	Stab();								// Default constructor.
	~Stab();								// Destructor.

	// Access
	int    getCurr();
	int    getLast();
	_TCHAR  *getPtr();
	int    getPerm();

	// Modify
	void setCurr(int);
	void setLast(int);
	void setPtr(_TCHAR *);
	void setPerm(int);

	// General.
	_TCHAR *addStr(_TCHAR *);			// Add a string to the string table.
	_TCHAR *addStr(_TCHAR *, long);
	_TCHAR *addStr(long);														// 05/26/02 AM.
	_TCHAR *addStrNC(_TCHAR *, long);	// Nonchecking version.
	_TCHAR *addStrNC(long);													// 05/26/02 AM.

	bool initStab();					// Initialize string table.
	bool cleanStab();					// Wipe out string table.
	void resetStab();					// Reset the string table (keeping perm).
	void prettyStab();				// Pretty-print the string table.
	long currSize();					// Remaining space in current segment.
	bool nextSeg();					// Get a new string table segment.
	bool allocSeg();					// Allocate a new segment.

	// Class-wide member functions. (static)
	static void test();

protected:
	_TCHAR *seg_[STAB_SEG_MAX];			// The array of segments.
	int curr_;								// The segment currently in use.
	int last_;								// The last segment allocated.
	_TCHAR *ptr_;								// First empty location in current segment.
	int perm_;								// The last permanent segment.
			// eg, each new text analyzed starts using string space at perm_+1.

// When code is well debugged and stable, can remove the stuff below.
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
