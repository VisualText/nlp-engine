/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SYM.H
* FILE:	lite\sym.h
* CR:		11/18/98 AM.
* SUBJ:	Declares for the symbol class.
* NOTE:	A symbol is the data in a hash table entry.
*			01/25/99 AM. At least for now, this class will be used for the
*			primary hash table for words.  The template class tSym is used
*			for hashing rules and other applications of hash tables.  Sym
*			can thus be dedicated to the "major mode".  An alternative design
*			would trash Sym and have everyone use tSym, which may yet happen.
*
*******************************************************************************/

#ifndef SYM_H_
#define SYM_H_
//#include <iostream.h>
#include <assert.h>
#include "stab.h"

// Forward references

/********************************************
* CLASS:	SYM
* CR:		11/18/98 AM.
* SUBJ:	A symbol class, representing a string.
********************************************/

class Sym
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Sym &);

public:
	Sym(_TCHAR * = 0, Stab * = 0);		// Default constructor.
	Sym(_TCHAR *, long, Stab *);			// String with length.
	~Sym();									// Destructor.

	// Access
	_TCHAR *getStr();
	int getFlags();					// 01/25/99 AM.
	Sym *getLcsym();					// 01/26/99 AM.
	int getUse();						// 04/11/06 AM.

	bool isLooked() const;			// 01/25/99 AM.
	bool isKnown() const;			// 01/25/99 AM.
	bool isWord() const;				// 01/25/99 AM.

	// Modify
	void setStr(_TCHAR *);
	void setFlags(int);				// 01/25/99 AM.
	void setLcsym(Sym *);			// 01/26/99 AM.
	void setUse(int);					// 04/11/06 AM.

	void setLooked(bool);			// 01/25/99 AM.
	void setKnown(bool);				// 01/25/99 AM.
	void setWord(bool);				// 01/25/99 AM.

	// General.

protected:
	_TCHAR *str_;							// POINTER TO STRING TABLE.
	// Can associate dictionary lookup, lexical information
	// with the sym.  Even concepts!

	// 01/25/99 AM. Placing lexical information directly in the sym,
	// for now.
	int flags_;				// For bitflags.		// 01/25/99 AM.

	// 01/26/99 AM. Some lexical knowledge here also.
	Sym *lcsym_;			// Lowercase sym, if any.	// 01/26/99 AM.

	// GARBAGE COLLECTION. If not referenced, remove sym from hash table.	// 04/11/06 AM.
	int use_;					// Use count.												// 04/11/06 AM.

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
