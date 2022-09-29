/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	HTAB.H
* FILE:	lite\htab.h
* CR:		11/18/98 AM.
* SUBJ:	Declares for the hash table class.
*
*******************************************************************************/

#ifndef HTAB_H_
#define HTAB_H_
//#include <iostream.h>
#include <assert.h>
#include "slist.h"
#include "selt.h"
#include "sym.h"

//// Global definitions for string tables.

// Size of hash table
// UPPING table sizes.														// 08/27/02 AM.
//#define HTAB_SIZE	((long)  10001)
//#define HTAB_SIZE	((long)  49999)									// 06/24/03 AM.
//#define HTAB_SIZE	((long) 100003)
#define HTAB_SIZE	   ((long) 250007)

// A maximum char length to hash on.									// 06/24/03 AM.
#define HASH_MAX 25

// Forward references
class Gen;

/********************************************
* CLASS:	HTAB
* CR:		11/18/98 AM.
* SUBJ:	A hash table manager.
********************************************/

class Htab
{
public:
	Htab(							// Default constructor.
		Stab * = Htab::gstab_,
		long = HTAB_SIZE		// Default size.  big!
		);
	~Htab();						// Destructor.

	// Access
	Stab *getStab();
	long getSize();			// 12/12/98 AM.

	// Modify
	void setStab(Stab *);
	void setSize(long);		// 12/12/98 AM.

	// General
	long hash(_TCHAR *);			// Get hash location in table.
	long hash(_TCHAR *, long);	// str, length.
	static long hashfn(_TCHAR *,long hsize);		// 06/15/00 AM.
	static long makesize(long len);										// 06/15/00 AM.
	Selt<Sym> *hfind(_TCHAR *);				// Find sym in table.
	Selt<Sym> *hfind(_TCHAR *, long);
	Selt<Sym> *hfind_lc(_TCHAR *);			// Find lowercase.
	Selt<Sym> *hadd(_TCHAR *);	// Add sym to table.
	Selt<Sym> *hadd(_TCHAR *, long);
	Selt<Sym> *hget(_TCHAR *);				// Find or add sym to table.
	Sym       *hsym(_TCHAR *);				// Find or add sym to table.
	Sym		 *hsym_kb(_TCHAR *);	// and fix up lowercase sym.	// 01/26/99 AM
	Sym		 *hsym_kb(_TCHAR *, long,		// 01/26/99 AM.
                     /*UP*/ _TCHAR* &lcstr);	// 08/01/11 AM.
	Selt<Sym> *hget(_TCHAR *, long);
	Sym       *hsym(_TCHAR *, long);
	Selt<Sym> *hpre(_TCHAR *, /*UP*/ Slist<Sym>* &);
												// Find sym for deleting.
	bool hdel(_TCHAR *);						// Remove sym from table.
	bool hdel(Selt<Sym> *);					// Remove sym from table.
	void pretty(std::_t_ostream*);						// Pretty-print the hash table.
	void clean();						// Wipe out the hash table.

	// GARBAGE COLLECTION.	// 04/11/06 AM.
	void hinc(_TCHAR *);		// 04/11/06 AM.
	void hdec(_TCHAR *);		// 04/11/06 AM.

	_TCHAR		*getStr(_TCHAR *);		// Get a hash table version of string.

	bool gen(																	// 06/14/00 AM.
		_TCHAR *,				// For naming table.
		Gen *gen				// Object controlling code generation.
		);

	// Class-wide
	static void test();							// Testing of hash table.
	static Stab *getGstab();					// 12/12/98 AM.
	static void setGstab(Stab *);				// 12/12/98 AM.

	
	// 12/12/98 AM. A global pointer to the current or default string table.
	// 12/29/04 AM. Moved out of private decls.  VC++ 7.0 2003 (.NET) complain.
	static Stab *gstab_;					// 12/12/98 AM.

private:
	// 12/12/98 AM. This is the hard-wired array.
	//Slist<Sym> arr_[HTAB_SIZE];				// The hash table array.

	// 12/12/98 AM. This is the dynamic pointer to array.
	Slist<Sym> *parr_;				// Hash table array ptr.	// 12/12/98 AM.
	long size_;							// Hash table size.			// 12/12/98 AM.

	Stab *stab_;						// String table.		// 11/19/98 AM.

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
