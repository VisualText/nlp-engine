/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	THTAB.H
* FILE:	lite\htab.h
* CR:		11/18/98 AM.
* SUBJ:	Declares for the hash table class.
*
*******************************************************************************/

#ifndef THTAB_H_
#define THTAB_H_
//#include <iostream.h>
#include <assert.h>
#ifdef LINUX
#include <sstream>
#else
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
//#include "inline.h"		// 05/19/99 AM.
#include "str.h"
#include "slist.h"
#include "selt.h"
#ifdef LINUX
#include "gen.h"				// 06/16/00 AM.
#endif
#include "htab.h"			// Just for the global string table ptr.
#include "tsym.h"

#ifdef LINUX
class Gen;	// Try this forward ref.	// 04/26/07 AM.
#endif

//// Global definitions for string tables.

// Size of hash table
//#define THTAB_SIZE	((long) 10001)
// Increase. // 10/29/05 AM.
#define THTAB_SIZE	((long) 100003)

// Forward references

/********************************************
* CLASS:	THTAB
* CR:		11/18/98 AM.
* SUBJ:	A hash table manager.
********************************************/

template<class TYPE>
class tHtab
{
public:
	tHtab(							// Default constructor.
		Stab * = Htab::gstab_,
		long = THTAB_SIZE		// Default size.  big!
		);
	~tHtab();						// Destructor.

	// Access
	Stab *getStab();
	long getSize();			// 12/12/98 AM.
	Slist<tSym<TYPE> > *getParr();
	// Modify
	void setStab(Stab *);
	void setSize(long);		// 12/12/98 AM.

	// General
	long hash(_TCHAR *);			// Get hash location in table.
	long hash(_TCHAR *, long);	// str, length.
	static long hashfn(_TCHAR *,long hsize);		// 07/08/03 AM.
	long makesize(long);													// 06/16/00 AM.
	Selt<tSym<TYPE> > *hfind(_TCHAR *);				// Find sym in table.
	Selt<tSym<TYPE> > *hfind(_TCHAR *, long);
	Selt<tSym<TYPE> > *hfind_lc(_TCHAR *);			// Find lowercase.
	Selt<tSym<TYPE> > *hfind_lc(_TCHAR *, TYPE* &);	// 12/18/98 AM.
	Selt<tSym<TYPE> > *hadd(_TCHAR *);	// Add sym to table.
	Selt<tSym<TYPE> > *hadd_lc(_TCHAR *);	// Add lc sym to table.
	Selt<tSym<TYPE> > *hadd(_TCHAR *, long);
	Selt<tSym<TYPE> > *hget(_TCHAR *);				// Find or add sym to table.
	Selt<tSym<TYPE> > *hget_lc(_TCHAR *);			// Find or add lc sym.
	tSym<TYPE>        *hsym(_TCHAR *);				// Find or add sym to table.
	Selt<tSym<TYPE> > *hget(_TCHAR *, long);
	tSym<TYPE>        *hsym(_TCHAR *, long);
	Selt<tSym<TYPE> > *hpre(_TCHAR *, /*UP*/ Slist<tSym<TYPE> >* &);
												// Find sym for deleting.
	bool hdel(_TCHAR *);						// Remove sym from table.
	bool hdel(Selt<tSym<TYPE> > *);					// Remove sym from table.
	void pretty();						// Pretty-print the hash table.
	void clean();						// Wipe out the hash table.

	bool gen(																	// 06/16/00 AM.
		Gen *gen				// Object controlling code generation.
		);

	// Class-wide
	static void test();							// Testing of hash table.
	static Stab *getGstab();					// 12/12/98 AM.
	static void setGstab(Stab *);				// 12/12/98 AM.

private:
	// 12/12/98 AM. This is the hard-wired array.
	//Slist<tSym<TYPE> > arr_[THTAB_SIZE];				// The hash table array.

	// 12/12/98 AM. This is the dynamic pointer to array.
	Slist<tSym<TYPE> > *parr_;				// Hash table array ptr.	// 12/12/98 AM.
	long size_;							// Hash table size.			// 12/12/98 AM.

	Stab *stab_;						// String table.		// 11/19/98 AM.

	// 12/12/98 AM. A global pointer to the current or default string table.
	static Stab *gstab_;					// 12/12/98 AM.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


/********************************************
* FN:		Special functions for the class
********************************************/

template<class TYPE>
tHtab<TYPE>::tHtab(							// Default constructor.
	Stab *stab,
	long size						// Size of hash table array.	// 12/12/98 AM.
	)
{
// 12/12/98 AM. Allocate new hash table array.
parr_ = new Slist<tSym<TYPE> > [size];
size_ = size;								// 12/12/98 AM.

stab_ = stab;								// 11/19/98 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

template<class TYPE>
tHtab<TYPE>::~tHtab()
{
// 11/19/98 AM. Let the string table fend for itself.

clean();

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/


/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

template<class TYPE>
Stab *tHtab<TYPE>::getStab()			{return stab_;}
template<class TYPE>
Stab *tHtab<TYPE>::getGstab()			{return gstab_;}
template<class TYPE>
long  tHtab<TYPE>::getSize()			{return size_;}
template<class TYPE>
Slist<tSym<TYPE> > *tHtab<TYPE>::getParr()	{return parr_;}


/********************************************
* FN:		Modify Functions
********************************************/

template<class TYPE>
void tHtab<TYPE>::setStab(Stab  *x)	{stab_  = x;}
template<class TYPE>
void tHtab<TYPE>::setGstab(Stab *x)	{gstab_ = x;}
template<class TYPE>
void tHtab<TYPE>::setSize(long   x)	{size_  = x;}

/********************************************
* FN:		GETCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class TYPE>
int tHtab<TYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class TYPE>
void tHtab<TYPE>::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active tHtab count=") << count_ << std::endl;
	*gout << _T("Active tHtab count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active tHtab count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif



/********************************************
* FN:		General Functions
********************************************/

/********************************************
* FN:		HASH
* CR:		11/18/98 AM.
* SUBJ:	Compute a hash table offset for given string.
*
********************************************/

template<class TYPE>
long tHtab<TYPE>::hash(_TCHAR *str)
{
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hash: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;
unsigned long val;
unsigned long warp;

warp = 1;
val = 0;
while ((ch = *str++) != '\0')
	{
	val += ((unsigned long) ch * warp++);								// 11/21/00 AM.
	if (val > (unsigned long) size_)										// 11/21/00 AM.
		val = val % size_;													// 11/21/00 AM.
	}

return val % size_;
}

/*******************************************/

template<class TYPE>
long tHtab<TYPE>::hash(_TCHAR *str, long len)
{
if (!str || !*str || (len <= 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hash: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;													// 11/21/00 AM.
unsigned long val;												// 11/21/00 AM.
unsigned long warp;												// 11/21/00 AM.

warp = 1;
val = 0;
while ((ch = *str++) != '\0'  && (len-- > 0))
	{
	val += ((unsigned long) ch * warp++);								// 11/21/00 AM.
	if (val > (unsigned long) size_)										// 11/21/00 AM.
		val = val % size_;													// 11/21/00 AM.
	}
return val % size_;
}

/********************************************
* FN:		HASHFN
* CR:		07/08/03 AM.
* SUBJ:	Compute a hash table offset for given string.
* NOTE:	VARIANT. Static for use in runtime.
*			Was using wrong type, but the hashfns may not
*			always be in sync.
*
********************************************/

template<class TYPE>
long tHtab<TYPE>::hashfn(_TCHAR *str, long siz)
{
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hashfn: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;
unsigned long val;
unsigned long warp;

warp = 1;
val = 0;
while ((ch = *str++) != '\0')
	{
	val += ((unsigned long) ch * warp++);
	if (val > (unsigned long) siz)
		val = val % siz;
	}

return val % siz;
}


/********************************************
* FN:		MAKESIZE
* CR:		06/16/00 AM.
* SUBJ:	Make reasonable table size for given number of rules.
* NOTE:	Should check against maxlong and so on.
*			30 is the old fudge factor used in interp version.
*			Not sure how helpful.  Better hash function will allow it
*			to be a much smaller fudge factor.
*
********************************************/

template<class TYPE>
long tHtab<TYPE>::makesize(long len)
{
// Trying to make it always an odd number.
if (len > 0)
	return (30 * len) + 1;
//	return (3 * len) + (len % 2) + 1;
return 0;
}


/********************************************
* FN:		HADD
* CR:		11/18/98 AM.
* SUBJ:	Add string to hash table.
* NOTE:	Forced add.  Doesn't check if present.
*
********************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hadd(_TCHAR *str)
{
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hadd: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str);
Slist<tSym<TYPE> > *list;
list = &(parr_[val]);			// Hash array loc for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array loc for string.	// 12/12/98 AM

tSym<TYPE>  *sym;
sym = new tSym<TYPE> (str, stab_);	// Create sym for string.
if (empty(sym->getStr()))							// CRASH GUARD.	// 12/07/01 AM.
	{
	delete sym;																	// 12/07/01 AM.
	return 0;																	// 12/07/01 AM.
	}
Selt<tSym<TYPE> > *selt;
selt = new Selt<tSym<TYPE> >(sym);	// Create hash table entry for string.

list->push(selt);			// Place at FRONT of hash chain.
return selt;
}

/*******************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hadd(_TCHAR *str, long len)
{
if (!str || !*str || (len <= 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hadd: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str, len);
Slist<tSym<TYPE> > *list;
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM

tSym<TYPE>  *sym;
sym = new tSym<TYPE> (str, len, stab_);	// Create sym for string.
if (empty(sym->getStr()))							// CRASH GUARD.	// 12/07/01 AM.
	{
	delete sym;																	// 12/07/01 AM.
	return 0;																	// 12/07/01 AM.
	}
Selt<tSym<TYPE> > *selt;
selt = new Selt<tSym<TYPE> >(sym);	// Create hash table entry for string.

list->push(selt);			// Place at FRONT of hash chain.
return selt;
}

/********************************************
* FN:		HFIND
* CR:		11/18/98 AM.
* SUBJ:	Find string in hash table.
* NOTE:	Does not add to table.
*			Stops at first instance found in chain.
*
********************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hfind(_TCHAR *str)
{
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str);
Slist<tSym<TYPE> > *list;
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM
Selt<tSym<TYPE> > *ptr;
tSym<TYPE>  *sym;
for (ptr = list->getFirst(); ptr; ptr = ptr->Right())
	{
	sym = ptr->getData();
	if (!_tcscmp(str, sym->getStr()))
		return ptr;				// Found it in chain.
	}
return 0;						// Not found.
}

/*******************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hfind(_TCHAR *str, long len)
{
if (!str || !*str || (len <= 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str, len);
Slist<tSym<TYPE> > *list;
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM
Selt<tSym<TYPE> > *ptr;
tSym<TYPE>  *sym;
_TCHAR *hstr;																			// 12/06/99 AM.
for (ptr = list->getFirst(); ptr; ptr = ptr->Right())
	{
	sym = ptr->getData();
	if (!_tcsncmp(str, (hstr = sym->getStr()), len)						// 12/06/99 AM.
		 && !hstr[len]		// FIX! Hashed str must be fully matched.	// 12/06/99 AM.
		 )
		return ptr;				// Found it in chain.
	}
return 0;						// Not found.
}

/********************************************
* FN:		HFIND_LC
* CR:		12/12/98 AM.
* SUBJ:	Find lowercased string in hash table.
* NOTE:	Does not add to table.
*			Stops at first instance found in chain.
*
********************************************/
static _TCHAR lc_buf[MAXSTR];
template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hfind_lc(_TCHAR *str)
{
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind_lc: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Convert to lower case.
str_to_lower(str, lc_buf);
return hfind(lc_buf);
}

/********************************************
* FN:		HFIND_LC
* CR:		12/18/98 AM.
* SUBJ:	Find lowercased string in hash table.
* NOTE:	Does not add to table.
*			Stops at first instance found in chain.
*
********************************************/

// 12/18/98 AM.
template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hfind_lc(
	_TCHAR *str,
	/*UP*/
	TYPE* &dat
	)
{
dat = 0;

if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind_lc: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Convert to lower case.
str_to_lower(str, lc_buf);
Selt<tSym<TYPE> > *selt;
selt = hfind(lc_buf);
if (!selt)
	return 0;
dat = selt->getData()->getPtr();
return selt;
}

/********************************************
* FN:		HADD_LC
* CR:		12/17/98 AM.
* SUBJ:	Add lowercased string to hash table.
********************************************/
template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hadd_lc(_TCHAR *str)
{
if (!str || !*str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hadd: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;

// Convert to lower case.
str_to_lower(str, lc_buf);

val = hash(lc_buf);
Slist<tSym<TYPE> > *list;
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM

tSym<TYPE>  *sym;
sym = new tSym<TYPE> (lc_buf, stab_);	// Create sym for string.
Selt<tSym<TYPE> > *selt;
selt = new Selt<tSym<TYPE> >(sym);	// Create hash table entry for string.

list->push(selt);			// Place at FRONT of hash chain.
return selt;
}


/********************************************
* FN:		HGET_LC
* CR:		12/17/98 AM.
* SUBJ:	Find string in hash table, else add it.
* OPT:	Unoptimized.  The two calls do redundant work.
*
********************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hget_lc(_TCHAR *str)
{
// Convert to lower case.
str_to_lower(str, lc_buf);

// The calls check for empty string.
Selt<tSym<TYPE> > *ptr;
if ((ptr = hfind(lc_buf)))
	return ptr;
return hadd(lc_buf);
}

/********************************************
* FN:		HPRE
* CR:		11/18/98 AM.
* SUBJ:	Find string in hash table. Special returns.
* RET:	If not at head of list, return element.
*			If at head of list, return element = 0 but list not 0.
*			If not found, element = 0 and list = 0.
* NOTE:	Used for removing the element found.
*
********************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hpre(_TCHAR *str, /*UP*/ Slist<tSym<TYPE> >* &list)
{
if (!str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hpre: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	list = 0;
	return 0;
	}
long val;
val = hash(str);
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM
Selt<tSym<TYPE> > *ptr;
Selt<tSym<TYPE> > *elt;
tSym<TYPE>  *sym;

ptr = list->getFirst();
sym = ptr->getData();
if (!_tcscmp(str, sym->getStr()))
	return 0;					// It's at head of the list.

while (ptr && (elt = ptr->Right()))
	{
	sym = elt->getData();
	if (!_tcscmp(str, sym->getStr()))
		return ptr;				// Found it in chain.
	ptr = elt;
	}
list = 0;						// Not found at head of list.
return 0;						// Not found in list.
}


/********************************************
* FN:		HGET
* CR:		11/18/98 AM.
* SUBJ:	Find string in hash table, else add it.
* OPT:	Unoptimized.  The two calls do redundant work.
*
********************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hget(_TCHAR *str)
{
// The calls check for empty string.
Selt<tSym<TYPE> > *ptr;
if ((ptr = hfind(str)) != NULL)
	return ptr;
return hadd(str);
}

/*******************************************/

template<class TYPE>
Selt<tSym<TYPE> > *tHtab<TYPE>::hget(_TCHAR *str, long len)
{
Selt<tSym<TYPE> > *ptr;
if ((ptr = hfind(str, len)) != NULL)
	return ptr;
return hadd(str, len);
}


/********************************************
* FN:		HSYM
* CR:		11/18/98 AM.
* SUBJ:	Find string in hash table, else add it.
*
********************************************/

template<class TYPE>
tSym<TYPE>  *tHtab<TYPE>::hsym(_TCHAR *str)
{
Selt<tSym<TYPE> > *selt;
if ((selt = hget(str)) != NULL)														// 12/07/01 AM.
	return selt->getData();
return 0;																		// 12/07/01 AM.
}


template<class TYPE>
tSym<TYPE>  *tHtab<TYPE>::hsym(_TCHAR *str, long len)
{
Selt<tSym<TYPE> > *selt;
if ((selt = hget(str, len)) != NULL)													// 12/07/01 AM.
	return selt->getData();
return 0;																		// 12/07/01 AM.
}

/********************************************
* FN:		HDEL
* CR:		11/18/98 AM.
* SUBJ:	Remove string from hash table.
* NOTE:	Delete the singly-linked list element and sym also.
*			Does not touch the string table.
*
********************************************/

template<class TYPE>
bool tHtab<TYPE>::hdel(_TCHAR *str)
{
if (!str)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hdel: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
// Need a special find that gets the element previous to the one being
// deleted.  If it's first in the list, list will be nonnull.
Slist<tSym<TYPE> > *list;
Selt<tSym<TYPE> > *pre;		// Prior element in list.
Selt<tSym<TYPE> > *elt;		// The element itself.
list = 0;
if ((pre = hpre(str, /*UP*/ list)) != NULL)
	{
	elt = list->rmNext(pre);	// Delete from list.
	}
else if (list)			// It's at head of list.
	{
	elt = list->pop();
	}

// Delete element and sym.
delete (Selt<tSym<TYPE> > *) elt;

return true;
}


/********************************************
* FN:		CLEAN
* CR:		11/18/98 AM.
* SUBJ:	Clean out the hash table.
*
********************************************/

template<class TYPE>
void tHtab<TYPE>::clean()
{
long ii;
Slist<tSym<TYPE> > *list;
Selt<tSym<TYPE> > *ptr;
//Selt<tSym<TYPE> > *tmp;
for (ii = 0; ii < size_; ++ii)
	{
	list = &(parr_[ii]);	// 12/12/98 AM
	//list = &(arr_[ii]);	// 12/12/98 AM
	if (!(ptr = list->getFirst()))
		continue;			// Empty hash chain.
	Slist<tSym<TYPE> >::DelSlistAndData(ptr);		// 11/24/98 AM.
	list->setFirst(0);
	list->setLast(0);
	}

// 12/12/98 AM. Must free the allocated array.
delete [] (Slist<tSym<TYPE> > *) parr_;
parr_ = 0;	// 08/20/03 AM.
}


/********************************************
* FN:		PRETTY
* CR:		11/18/98 AM.
* SUBJ:	Pretty-print the hash table.
*
********************************************/

template<class TYPE>
void tHtab<TYPE>::pretty()
{
long ii;
Slist<tSym<TYPE> > *list;
Selt<tSym<TYPE> > *ptr;
tSym<TYPE>  *sym;
*gout << _T("Hash table:\n")
	  << _T("-----------") << std::endl;
for (ii = 0; ii < size_; ++ii)
	{
	list = &(parr_[ii]);		// 12/12/98 AM.
	//list = &(arr_[ii]);	// 12/12/98 AM.
	if (!(ptr = list->getFirst()))
		continue;			// Empty hash chain.
	*gout << ii << _T(": ");
	for (; ptr; ptr = ptr->pRight)
		{
		// Pretty print the entry.
		sym = ptr->getData();
		*gout << sym->getStr() << _T(" ");
		}
	*gout << std::endl;
	}
}


/********************************************
* FN:		TEST
* CR:		11/18/98 AM.
* SUBJ:	Test out the hash table code.
*
********************************************/

template<class TYPE>
void tHtab<TYPE>::test()
{
Stab stab;				// String table.		// 11/19/98 AM.
tHtab table(&stab);

table.pretty();
table.hadd(_T("hello"));
table.pretty();
table.hadd(_T("goodbye"));
table.pretty();
table.hadd(_T("goodbye"));
table.pretty();
if (table.hfind(_T("goodbye")))
	*gout << _T("Found goodbye") << std::endl;
if (table.hfind(_T("hello")))
	*gout << _T("Found hello") << std::endl;
if (!table.hfind(_T("grunk")))
	*gout << _T("Didn't find grunk") << std::endl;
table.hget(_T("hello"));
table.pretty();
table.hget(_T("grunk"));
table.pretty();
table.hget(_T("gntmf"));
table.pretty();
table.hdel(_T("hello"));
table.pretty();
table.hdel(_T("goodbye"));
table.pretty();
table.hdel(_T("goodbye"));
table.pretty();
table.clean();
table.pretty();
}


/********************************************
* FN:		GEN
* CR:		06/16/00 AM.
* SUBJ:	Gen code for pass' hash table.
*
********************************************/

template<class TYPE>
bool tHtab<TYPE>::gen(Gen *gen)
{

std::_t_ofstream *rhash = gen->getRhash();
std::_t_ofstream *rhead = gen->getRhead();
std::_t_ofstream *rchain = gen->getRchain();
std::_t_ofstream *rdata = gen->getRdata();
int id = gen->getId();
int recid = gen->getRecid();

_TCHAR *rname = _T("rhash");		// For naming hash tables.
_TCHAR *cname = _T("chain");		// For naming conflict chain.
_TCHAR buf[MAXSTR];
_TCHAR arr[1024];
_TCHAR chain[MAXSTR];
_TCHAR buf1[MAXSTR+1];
_TCHAR buf2[MAXSTR+1];
int linemax=20;		// Limit of elts per line.
_stprintf(buf, _T("%s%d_%d"), rname,id,recid);


*rhead << _T("extern const RHASH *") << buf << _T("[];");
Gen::eol(rhead);	// 04/04/03 AM.
*rhash << _T("const RHASH *") << buf << _T("[]={");
Gen::eol(rhash);	// 04/04/03 AM.

long ii, jj;
Slist<tSym<TYPE> > *list;
Selt<tSym<TYPE> > *ptr;
tSym<TYPE>  *sym;
long len;
for (ii = 0; ii < size_; ++ii)
	{
#ifdef GENPRETTY_
	if (!(ii % linemax))
		*rhash << std::endl;
#endif
	list = &(parr_[ii]);		// 12/12/98 AM.
	if (!(ptr = list->getFirst()))
		{
		*rhash << _T("0,");
		continue;			// Empty hash chain.
		}
	// _stprintf(arr, _T("%s_%d"), buf, ii);	// 09/26/19 AM.
	_stprintf(arr, _T("%s_%ld"), buf, ii);
	*rhash << arr << _T(",");
	*rchain << _T("const RHASH ") << arr << _T("[]={");
	jj = 0;
	for (; ptr; ptr = ptr->pRight)
		{
		++jj;			// 04/04/03 AM.
#ifdef GENPRETTY_
		if (!(jj % linemax))	// 04/04/03 AM.
			*rchain << std::endl;
#endif

		sym = ptr->getData();

		// _stprintf(chain,_T("%s%d_%d_%d_%d"),cname,id,recid,ii,jj);
		_stprintf(chain,_T("%s%d_%d_%ld_%ld"),cname,id,recid,ii,jj);	// 09/26/19 AM.
		*rdata << _T("const int ") << chain << _T("[]={");
		// Get length of conflict array.									// 06/19/00 AM.
		len = sym->getPtr()->genConflicts(rdata);						// 06/17/00 AM.
		*rdata << _T("};");
		Gen::eol(rdata);	// 04/04/03 AM.

		// Three-member struct (key, array, len, rulenum array).
		str_to_lower(sym->getStr(), buf1);
		*rchain << _T("{") << _T("_T(\"") << c_str(buf1,buf2,MAXSTR) << _T("\"),")
				  << len << _T(",");												// 06/19/00 AM.
		*rchain << chain << _T("},");
		}
	*rchain << _T("0};");	// Null termination.
	Gen::eol(rchain);	// 04/04/03 AM.
	}

*rhash << _T("0};");		// Null termination.
Gen::eol(rhash);	// 04/04/03 AM.
return true;
}

/*************************      END OF FILE      ******************************/


#endif
/*************************      END OF FILE      ******************************/
