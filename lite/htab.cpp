/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	HTAB.CPP
* FILE:	lite\htab.cpp
* CR:		11/18/98 AM.
* SUBJ:	Class for string tables.
* NOTE:	Could have one for permanent strings and one for the current text.
* ASS:	Individual strings should be much smaller than a segment.
*******************************************************************************/

#include "StdAfx.h"
//#include <iostream> // 07/18/03 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "io.h"
#include "lite/iarg.h"	// 05/14/03 AM.
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"			// 06/14/00 AM.
#include "htab.h"


int Htab::count_ = 0;
Stab *Htab::gstab_ = 0;				// 12/12/98 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

Htab::Htab(							// Default constructor.
	Stab *stab,
	long size						// Size of hash table array.	// 12/12/98 AM.
	)
{
// 12/12/98 AM. Allocate new hash table array.
parr_ = new Slist<Sym> [size];
size_ = size;								// 12/12/98 AM.

stab_ = stab;								// 11/19/98 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Htab::~Htab()
{
// 11/19/98 AM. Let the string table fend for itself.

clean();

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

std::_t_ostream &operator<<(std::_t_ostream &output, Htab &htab)
{
output << _T("<Hash table")
		 << _T(">") << std::endl;
return output;
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

Stab *Htab::getStab()			{return stab_;}
Stab *Htab::getGstab()			{return gstab_;}
long  Htab::getSize()			{return size_;}

/********************************************
* FN:		Modify Functions
********************************************/

void Htab::setStab(Stab  *x)	{stab_  = x;}
void Htab::setGstab(Stab *x)	{gstab_ = x;}
void Htab::setSize(long   x)	{size_  = x;}

/********************************************
* FN:		GETCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Htab::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Htab::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Htab count=") << count_ << std::endl;
	*gout << _T("Active Htab count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Htab count=") << count_ << std::ends;
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
* NOTE:	Since ch can randomly be even or odd, keeping warp * warp
*	always odd, so that value will have 50/50 chance of being even/odd.
*
********************************************/

long Htab::hash(_TCHAR *str)
{
if (empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hash: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;										// FIX.	// 11/21/00 AM.
unsigned long val=0;									// FIX.	// 11/21/00 AM.
unsigned long warp=113;								// FIX.	// 11/21/00 AM.
unsigned long ii=1;												// 06/24/03 AM.

while ((ch = *str++) != '\0')
	{
	warp += 22;																	// 06/24/03 AM.
	val += ((unsigned long) ch * warp						// FIX.	// 11/21/00 AM.
						* warp);													// 06/24/03 AM.
	if (val > (unsigned long) size_)										// 11/21/00 AM.
		val = val % size_;										// FIX.	// 09/27/00 AM.
	if (++ii > HASH_MAX)														// 06/24/03 AM.
		return val;																// 06/24/03 AM.
	}
return val % size_;
}

/*******************************************/

long Htab::hash(_TCHAR *str, long len)
{
if (empty(str) || (len <= 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hash: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;
unsigned long val=0;
unsigned long warp=113;											// 06/24/03 AM.
unsigned long ii=1;												// 06/24/03 AM.
unsigned long max=HASH_MAX;									// 06/24/03 AM.

if (len < HASH_MAX)															// 06/24/03 AM.
	max = len;

while ((ch = *str++) != '\0'  && (len-- > 0))
	{
	warp += 22;																	// 06/24/03 AM.
	val += ((unsigned long) ch * warp									// 06/24/03 AM.
						* warp);													// 06/24/03 AM.
	if (val > (unsigned long) size_)							// FIX.	// 09/27/00 AM.
		val = val % size_;										// FIX.	// 09/27/00 AM.
	if (++ii > max)															// 06/24/03 AM.
		return val;																// 06/24/03 AM.
	}
return val % size_;
}

/********************************************
* FN:		HASHFN
* CR:		06/15/00 AM.
* SUBJ:	Compute a hash table offset for given string.
* NOTE:	VARIANT. Static. For compiled runtime support.
*			Needed to change name, or it conflicts with above.
*
********************************************/

long Htab::hashfn(_TCHAR *str, long hsize)
{
if (empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hash: Given empty string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;										// FIX.	// 06/21/02 AM.
unsigned long val=0;									// FIX.	// 06/21/02 AM.
unsigned long warp=113;								// FIX.	// 06/21/02 AM.
unsigned long ii=1;												// 06/24/03 AM.

while ((ch = *str++) != '\0')
	{
	warp += 22;																	// 06/24/03 AM.
	val += ((unsigned long) ch * warp						// FIX.	// 06/21/02 AM.
						* warp);													// 06/24/03 AM.
	if (val > (unsigned long) hsize)							// FIX.	// 06/21/02 AM.
		val = val % hsize;										// FIX.	// 09/27/00 AM.
	if (++ii > HASH_MAX)														// 06/24/03 AM.
		return val;																// 06/24/03 AM.
	}
return val % hsize;
}


/********************************************
* FN:		MAKESIZE
* CR:		06/15/00 AM.
* SUBJ:	Make reasonable table size for given number of entries.
* NOTE:	Should check against maxlong and so on.
*			Trying 3 times length instead of 2.
*
********************************************/

long Htab::makesize(long len)
{
// Trying to make it always an odd number.
if (len > 0)
	return (3 * len) + (len % 2) + 1;
//2 * len + 1;
return 0;
}


/********************************************
* FN:		HADD
* CR:		11/18/98 AM.
* SUBJ:	Add string to hash table.
* NOTE:	Forced add.  Doesn't check if present.
*
********************************************/

Selt<Sym> *Htab::hadd(_TCHAR *str)
{
if (empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hadd: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str);
Slist<Sym> *list;
list = &(parr_[val]);			// Hash array loc for string.		// 12/12/98 AM
//list = &(arr_[val]);			// Hash array loc for string.		// 12/12/98 AM

Sym *sym;
sym = new Sym(str, stab_);	// Create sym for string.
if (empty(sym->getStr()))							// CRASH GUARD.	// 12/07/01 AM.
	{
	delete sym;																	// 12/07/01 AM.
	return 0;																	// 12/07/01 AM.
	}
Selt<Sym> *selt;
selt = new Selt<Sym>(sym);	// Create hash table entry for string.

list->push(selt);			// Place at FRONT of hash chain.
return selt;
}

/*******************************************/

Selt<Sym> *Htab::hadd(_TCHAR *str, long len)
{
if (empty(str) || (len <= 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hadd: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str, len);
Slist<Sym> *list;
list = &(parr_[val]);			// Hash array loc for string.		// 12/12/98 AM
//list = &(arr_[val]);			// Hash array loc for string.		// 12/12/98 AM

Sym *sym;
sym = new Sym(str, len, stab_);	// Create sym for string.
Selt<Sym> *selt;
selt = new Selt<Sym>(sym);	// Create hash table entry for string.
if (empty(sym->getStr()))							// CRASH GUARD.	// 12/07/01 AM.
	{
	delete sym;																	// 12/07/01 AM.
	return 0;																	// 12/07/01 AM.
	}
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

Selt<Sym> *Htab::hfind(_TCHAR *str)
{
if (empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str);
Slist<Sym> *list;
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM
Selt<Sym> *ptr;
Sym *sym;
for (ptr = list->getFirst(); ptr; ptr = ptr->Right())
	{
	sym = ptr->getData();
	if (!_tcscmp(str, sym->getStr()))
		return ptr;				// Found it in chain.
	}
return 0;						// Not found.
}

/*******************************************/

Selt<Sym> *Htab::hfind(_TCHAR *str, long len)
{
if (empty(str) || (len <= 0))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
long val;
val = hash(str, len);
Slist<Sym> *list;
list = &(parr_[val]);			// Hash array location for string.	// 12/12/98 AM
//list = &(arr_[val]);			// Hash array location for string.	// 12/12/98 AM
Selt<Sym> *ptr;
Sym *sym;
_TCHAR *hstr;		// Hash table string.										// 12/06/99 AM.
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
static _TCHAR lcbuf[MAXSTR];
Selt<Sym> *Htab::hfind_lc(_TCHAR *str)
{
if (empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[hfind_lc: Given null string.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Convert to lower case.
str_to_lower(str, lcbuf);
return hfind(lcbuf);
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

Selt<Sym> *Htab::hpre(_TCHAR *str, /*UP*/ Slist<Sym>* &list)
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
Selt<Sym> *ptr;
Selt<Sym> *elt;
Sym *sym;

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

Selt<Sym> *Htab::hget(_TCHAR *str)
{
// The calls check for empty string.
Selt<Sym> *ptr;
if ((ptr = hfind(str)))
	return ptr;
return hadd(str);
}


/********************************************
* FN:		GETSTR
* CR:		01/18/99 AM.
* SUBJ:	Find string in hash table, else add it.
* NOTE:	For users who don't care about syms, only strings.
*
********************************************/

_TCHAR *Htab::getStr(_TCHAR *str)
{
// The calls check for empty string.
Selt<Sym> *ptr;
if (!(ptr = hget(str)))
	return 0;
Sym *sym;
sym = ptr->getData();
assert(sym);
return sym->getStr();
}

/*******************************************/

Selt<Sym> *Htab::hget(_TCHAR *str, long len)
{
Selt<Sym> *ptr;
if ((ptr = hfind(str, len)))
	return ptr;
return hadd(str, len);
}


/********************************************
* FN:		HSYM
* CR:		11/18/98 AM.
* SUBJ:	Find string in hash table, else add it.
*
********************************************/

Sym *Htab::hsym(_TCHAR *str)
{
Selt<Sym> *selt;
if ((selt = hget(str)))														// 12/07/01 AM.
	return selt->getData();
return 0;																		// 12/07/01 AM.
}


Sym *Htab::hsym(_TCHAR *str, long len)
{
Selt<Sym> *selt;
if ((selt = hget(str, len)))													// 12/07/01 AM.
	return selt->getData();
return 0;																		// 12/07/01 AM.
}



/********************************************
* FN:		HSYM_KB
* CR:		01/26/99 AM.
* SUBJ:	Find/add string and its lowercase variant.
*
********************************************/

Sym *Htab::hsym_kb(_TCHAR *str)
{
Sym *sym, *lcsym;
_TCHAR buf[MAXSTR];
Selt<Sym> *selt;
if (!(selt = hget(str)))													// 12/07/01 AM.
	return 0;																	// 12/07/01 AM.
sym = selt->getData();

if (sym->getLcsym())			// Already has a lowercase version.
	return sym;

// Install lowercase variant.
str_to_lower(str, buf);
if (!_tcscmp(str, buf))		// String is already lowercase.
	{
	sym->setLcsym(sym);		// Make it point to itself!
	return sym;
	}

lcsym = hsym(buf);			// Get or make lowercase sym.
lcsym->setLcsym(lcsym);		// Lc points to itself (whether set or not).
sym->setLcsym(lcsym);		// Non-lc points to its lc variant.
return sym;
}


/********************************************
* FN:		HSYM_KB
* CR:		01/26/99 AM.
* SUBJ:	Find/add string and its lowercase variant.
*
********************************************/

Sym *Htab::hsym_kb(_TCHAR *str, long len,
	/*UP*/ _TCHAR* &lcstr)
{
lcstr = 0;
Sym *sym, *lcsym;
_TCHAR buf[MAXSTR];
Selt<Sym> *selt;
if (!(selt = hget(str, len)))												// 12/07/01 AM.
	return 0;																	// 12/07/01 AM.
sym = selt->getData();

if ((lcsym = sym->getLcsym()))			// Already has a lowercase version.
	{
	lcstr = lcsym->getStr();	// 08/01/11 AM.
	return sym;
	}

str = sym->getStr();			// Get terminated string.

// Install lowercase variant.
str_to_lower(str, buf);
if (!_tcscmp(str, buf))		// String is already lowercase.
	{
	sym->setLcsym(sym);		// Make it point to itself!
	lcstr = str;	// 08/01/11 AM.
	return sym;
	}

lcsym = hsym(buf);			// Get or make lowercase sym.
lcsym->setLcsym(lcsym);		// Lc points to itself (whether set or not).
sym->setLcsym(lcsym);		// Non-lc points to its lc variant.
lcstr = lcsym->getStr();	// 08/01/11 AM.
return sym;
}

/********************************************
* FN:		HDEL
* CR:		11/18/98 AM.
* SUBJ:	Remove string from hash table.
* NOTE:	Delete the singly-linked list element and sym also.
*			Does not touch the string table.
*
********************************************/

bool Htab::hdel(_TCHAR *str)
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
Slist<Sym> *list;
Selt<Sym> *pre;		// Prior element in list.
Selt<Sym> *elt = nullptr;		// The element itself.
list = 0;
if ((pre = hpre(str, /*UP*/ list)))
	{
	elt = list->rmNext(pre);	// Delete from list.
	}
else if (list)			// It's at head of list.
	{
	elt = list->pop();
	}

// Delete element and sym.
delete (Selt<Sym> *) elt;

return true;
}

/********************************************
* FN:		HINC
* CR:		04/11/06 AM.
* SUBJ:	Increment string use in hash table.
* NOTE:	Implementing some GARBAGE COLLECTION.
*
********************************************/

void Htab::hinc(_TCHAR *str)
{
if (!str || !*str)
	{
//	std::_t_strstream gerrStr;
//	gerrStr << _T("[hinc: Given null string.]") << std::ends;
//	errOut(&gerrStr,false);
	return;
	}

Selt<Sym> *elt = hfind(str);
if (!elt)
	return;	// String not in table, so ignore it.

Sym *sym = elt->getData();
int use = sym->getUse();
sym->setUse(++use);	// Increment use count.
}

/********************************************
* FN:		HDEC
* CR:		04/11/06 AM.
* SUBJ:	Decrement string use in hash table. Delete if not in use.
* NOTE:	Implementing some GARBAGE COLLECTION.
*			The string table space is not reclaimed.
*			Presumably, strings can come from other places than the hashed
*			symbol table.  Those are ignored.
*
********************************************/

void Htab::hdec(_TCHAR *str)
{
if (!str || !*str)
	{
//	std::_t_strstream gerrStr;
//	gerrStr << _T("[hdec: Given null string.]") << std::ends;
//	errOut(&gerrStr,false);
	return;
	}

Selt<Sym> *elt = hfind(str);
if (!elt)
	return;	// String not in table, so ignore it.

Sym *sym = elt->getData();
int use = sym->getUse();
if (use == 1)	// Not in use.	// 06/12/06 AM.
//	hdel(sym);
	hdel(str);	// Remove from hashed symbol table.
else if (use < 1)						// 06/12/06 AM.
	// Todo: Flag an error here.	// 06/12/06 AM.
	sym->setUse(0);					// 06/12/06 AM.
else
	sym->setUse(--use);	// Decrement use count.
}


/********************************************
* FN:		CLEAN
* CR:		11/18/98 AM.
* SUBJ:	Clean out the hash table.
*
********************************************/

void Htab::clean()
{
long ii;
Slist<Sym> *list;
Selt<Sym> *ptr;
//Selt<Sym> *tmp;
for (ii = 0; ii < size_; ++ii)
	{
	list = &(parr_[ii]);	// 12/12/98 AM
	//list = &(arr_[ii]);	// 12/12/98 AM
	if (!list)		// RECOVER	// 02/27/20 AM.
		continue;	// RECOVER	// 02/27/20 AM.
	if (!(ptr = list->getFirst()))
		continue;			// Empty hash chain.
	Slist<Sym>::DelSlistAndData(ptr);		// 11/24/98 AM.
	list->setFirst(0);
	list->setLast(0);
	}

// 12/12/98 AM. Must free the allocated array.
delete [] (Slist<Sym> *) parr_;
parr_ = 0;	// 08/20/03 AM.
}


/********************************************
* FN:		PRETTY
* CR:		11/18/98 AM.
* SUBJ:	Pretty-print the hash table.
*
********************************************/

void Htab::pretty(
	std::_t_ostream *gout	// 06/24/03 AM.
	)
{
long ii;
Slist<Sym> *list;
Selt<Sym> *ptr;
Sym *sym;
*gout << _T("Hash table:\n")
	  << _T("-----------") << std::endl;
for (ii = 0; ii < size_; ++ii)
	{
	list = &(parr_[ii]);		// 12/12/98 AM.
	//list = &(arr_[ii]);	// 12/12/98 AM.
	if (!(ptr = list->getFirst()))
		continue;			// Empty hash chain.
	*gout << ii << _T(": ");
//	if (!ptr) *gout << "<EMPTY>";	// DEBUGGING!!!!	// 09/28/20 AM.
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

void Htab::test()
{
Stab stab;				// String table.		// 11/19/98 AM.
Htab table(&stab);

table.pretty(gout);
table.hadd(_T("hello"));
table.pretty(gout);
table.hadd(_T("goodbye"));
table.pretty(gout);
table.hadd(_T("goodbye"));
table.pretty(gout);
if (table.hfind(_T("goodbye")))
	*gout << _T("Found goodbye") << std::endl;
if (table.hfind(_T("hello")))
	*gout << _T("Found hello") << std::endl;
if (!table.hfind(_T("grunk")))
	*gout << _T("Didn't find grunk") << std::endl;
table.hget(_T("hello"));
table.pretty(gout);
table.hget(_T("grunk"));
table.pretty(gout);
table.hget(_T("gntmf"));
table.pretty(gout);
table.hdel(_T("hello"));
table.pretty(gout);
table.hdel(_T("goodbye"));
table.pretty(gout);
table.hdel(_T("goodbye"));
table.pretty(gout);
table.clean();
table.pretty(gout);
}


/********************************************
* FN:		GEN
* CR:		06/14/00 AM.
* SUBJ:	Gen code for hash table (for match lists in rule elements).
* NOTE:	Traverse the entire hash table.  When a non-empty loc is
*			found, generate an array of strings for it.
*			Each string array will be null terminated.
* OPT:	A null terminator is put at the end of the hash table, just
*			to simplify the code below.  Could optimize away.
*			A simpler numbering scheme (eg, a global counter) would shorten
*			string array names substantially.
*
********************************************/

bool Htab::gen(
	_TCHAR *stem,			// Base name for building array name.
	Gen *gen				// Object controlling code generation.
	)
{
std::_t_ofstream *ehash = gen->ehash_;	// File for hash table.
std::_t_ofstream *edata = gen->edata_;
								// File for arrays of strings, one for each hash loc.
std::_t_ofstream *ehead	= gen->ehead_;		// Header file.

long ii;
Slist<Sym> *list;
Selt<Sym> *ptr;
Sym *sym;

_TCHAR table[128];
_stprintf(table, _T("%s%d_%d_%d_%d"),
				stem,
				gen->id_,
				gen->recid_,
				gen->ruleid_,
				gen->eltid_
				);

// A table of tables of strings.
*ehead << _T("extern const _TCHAR **")
		 << table
		 << _T("[];");
Gen::eol(ehead);	// 04/04/03 AM.

*ehash << _T("const _TCHAR **")
		 << table
		 << _T("[]={");
Gen::eol(ehash);	// 04/04/03 AM.

int hmax = 20;		// Number of entries to put on one line of hash table.
int smax = 20;		// Number of entries to put on one line of string array.
long jj = 0;
_TCHAR buf[MAXSTR+1];
_TCHAR buf2[MAXSTR+1];
for (ii = 0; ii < size_; ++ii)
	{
#ifdef GENPRETTY_
	if (!(ii % hmax))
		*ehash << std::endl << _T("\t");			// Line break.
#endif
	list = &(parr_[ii]);
	if (!(ptr = list->getFirst()))
		{
		*ehash << _T("0,");
		continue;			// Empty hash chain.
		}

	// Non empty locus.

	// Give hash table reference to array.
	*ehash << table << _T("_") << ii
			 << _T(",");

	// Output the array.
	*edata << _T("const _TCHAR *")
			 << table << _T("_") << ii
			 << _T("[]={");
	jj = 0;
	for (; ptr; ptr = ptr->pRight)
		{
		// Pretty print the entry.
#ifdef GENPRETTY_
		if (!(++jj % smax))
			*edata << std::endl << _T("\t");
#endif
		sym = ptr->getData();
		// OPT: These can be condensed to one string traversal...
		str_to_lower(sym->getStr(), buf);
		*edata << _T("_T(\"") << c_str(buf,buf2,MAXSTR) << _T("\"),");
		}
	*edata << _T("0};");
	Gen::eol(edata);	// 04/04/03 AM.
	}

*ehash << _T("0};");
Gen::eol(ehash);		// 04/04/03 AM.
return true;
}

/*************************      END OF FILE      ******************************/
