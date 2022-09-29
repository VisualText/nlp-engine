/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SLIST.H
* FILE:	include/API/lite/slist.h
* CR:		11/18/98 AM.
* SUBJ:	Declares for a singly-linked list.
* NOTE:	Copied and modified dlist.h.
*
*******************************************************************************/

#ifndef SLIST_H
#define SLIST_H
//#include <iostream.h>
#include <assert.h>
#ifdef LINUX
#include <sstream>
#else
#include <strstream>															// 05/25/01 AM.
#endif

#include "selt.h"

// Forward references.
#ifdef LINUX
//class Gen;					// 06/17/00 AM.
#else
class Gen;					// 06/17/00 AM.
#endif

/********************************************
* CLASS:	SLIST
* CR:		11/18/98 AM.
********************************************/

template<class SELTTYPE>
class Slist
{
public:
	Slist();							// Default constructor. Makes empty list.
	Slist(SELTTYPE *);			// Constructor. Takes elt value.
	Slist(Selt<SELTTYPE> *);	// Constructor. Takes an element.
	~Slist();						// Destructor.
	Slist(const Slist<SELTTYPE> &);	// Copy constructor.	// 12/18/98 AM.
	Slist(Selt<SELTTYPE> *, bool);	// Copy constructor(2). // 12/19/98 AM.

	// Access
	Selt<SELTTYPE> *getFirst();
	Selt<SELTTYPE> *getLast();

	// Modify.
	void setFirst(Selt<SELTTYPE> *);
	void setLast(Selt<SELTTYPE> *);

	void push(SELTTYPE *);
	void push(Selt<SELTTYPE> *);
	void rpush(SELTTYPE *);
	void rpush(Selt<SELTTYPE> *);
	Selt<SELTTYPE> *pop();
	Selt<SELTTYPE> *rpop();		// Pop from end.
	Selt<SELTTYPE> *rmNext(Selt<SELTTYPE> *);		// Rm next guy from list.

	Selt<SELTTYPE> *getNth(long);
	long getLength();
	int CmpNumSlist(long);		// Compare number to length. 1,0,-1.
	
	// Insert, given an element of the list.
	void insertLeft( Selt<SELTTYPE> *selt, Selt<SELTTYPE> *pos);
	void insertRight( Selt<SELTTYPE> *selt, Selt<SELTTYPE> *pos);
	void insertRight(SELTTYPE *, Selt<SELTTYPE> *pos);

	void Traverse(const Selt<SELTTYPE> *) const;
	void Dump() const;			// Traverse the entire list.

	static bool empty(Slist<SELTTYPE>*);								// 06/20/00 AM.

	static void DeleteSlistAndData(Slist<SELTTYPE> *);
	static void DelSlistAndData(Selt<SELTTYPE> *);

	static void DeleteSlist(Slist<SELTTYPE> *);	// Don't delete data 12/16/98 AM
	static void DelSlist(Selt<SELTTYPE> *);	// Don't delete data 12/16/98 AM
	static Slist<SELTTYPE> *merge(				// 06/30/99 AM.
		Slist<SELTTYPE> *list1,
		Slist<SELTTYPE> *list2
		);
	long genConflicts(std::_t_ostream *);											// 06/17/00 AM.

private:
	Selt<SELTTYPE> *pFirst;	// Point to first element.
	Selt<SELTTYPE> *pLast;	// Point to last element.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_TCHAR * = _T(""),
								std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


/********************************************
* FN:		SLIST (constructor)
* CR:		11/18/98 AM.
* NOTE:	Not checking that element is a loner.
********************************************/
template<class SELTTYPE>
Slist<SELTTYPE>::Slist()
{
pFirst = pLast = 0;

#ifndef STABLE_
++count_;
#endif
}

template<class SELTTYPE>
Slist<SELTTYPE>::Slist(SELTTYPE *dd)
{
Selt<SELTTYPE> *selt;

selt = new Selt<SELTTYPE>(dd);
pFirst = pLast = selt;

#ifndef STABLE_
++count_;
#endif
}

// Copy constructor.			// 12/18/98 AM.
// LINUX vs WINDOWS. Both use the same form now. // 03/20/19 AM.
//Slist<SELTTYPE>::Slist<SELTTYPE>(const Slist<SELTTYPE> &orig)
template<class SELTTYPE>
Slist<SELTTYPE>::Slist(const Slist<SELTTYPE> &orig)	// 11/19/07 AM.
{
//*gerr << "[Slist copy constructor]" << std::endl;
pFirst = pLast = 0;
// Copy the list.
Selt<SELTTYPE> *ptr;
ptr = orig.pFirst;
while (ptr)
	{
	rpush(ptr->data);
	ptr = ptr->pRight;
	}

#ifndef STABLE_
++count_;
#endif
}


// Trying out a variant copy constructor.
// Copy constructor.			// 12/19/98 AM.
// LINUX vs WINDOWS. Both use the same form now. // 03/20/19 AM.
// Slist<SELTTYPE>::Slist<SELTTYPE>(Selt<SELTTYPE> *orig, bool dummy)
template<class SELTTYPE>
Slist<SELTTYPE>::Slist(Selt<SELTTYPE> *orig, bool dummy)	// 11/19/07 AM.
{
//*gerr << "[Slist copy constructor(2)]" << std::endl;
pFirst = pLast = 0;
// Copy the list.
Selt<SELTTYPE> *ptr;
ptr = orig;
while (ptr)
	{
	rpush(ptr->data);
	ptr = ptr->pRight;
	}

#ifndef STABLE_
++count_;
#endif
}


// WARNING: element passed to this function should have
// these values set.  Caller must assure this.
// selt->pRight = 0;
template<class SELTTYPE>
Slist<SELTTYPE>::Slist(Selt<SELTTYPE> *selt)
{
pFirst = pLast = selt;

// While code being built, do this check.
#ifndef STABLE_
if (selt->pRight)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Slist: Given element is part of another list.]") << std::ends;
	errOut(&gerrStr,false);
	}

	selt->pRight = 0;
	}
#endif

#ifndef STABLE_
++count_;
#endif
}

template<class SELTTYPE>
Slist<SELTTYPE>::~Slist()
{
Slist<SELTTYPE>::DelSlistAndData(pFirst);					// 11/24/98 AM.
pFirst = pLast = 0;			// Squeaky clean removal.	// 11/24/98 AM.

#ifndef STABLE_
--count_;
#endif
}


/********************************************
* FN:		Access functions.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE> *Slist<SELTTYPE>::getFirst()
{
return pFirst;
}

template<class SELTTYPE>
Selt<SELTTYPE> *Slist<SELTTYPE>::getLast()
{
return pLast;
}


/********************************************
* FN:		Modify functions.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::setFirst(Selt<SELTTYPE> *selt)
{
pFirst = selt;
}

template<class SELTTYPE>
void Slist<SELTTYPE>::setLast(Selt<SELTTYPE> *selt)
{
pLast = selt;
}

/********************************************
* FN:		GETCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class SELTTYPE>
int Slist<SELTTYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class SELTTYPE>
void Slist<SELTTYPE>::prettyCount(_TCHAR *str,
	std::_t_ofstream *ofstr)
{
if (count_)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Slist<") << str << _T("> count=")
		  << count_ << std::ends;
	errOut(&gerrStr,false);
	}

	*gout << _T("Active Slist<") << str << _T("> count=")
		  << count_ << std::endl;

	if (ofstr)
		*ofstr << _T("Active Slist<") << str << _T("> count=")
		  << count_ << std::endl;
	}
}
#endif

/********************************************
* FN:		PUSH
* CR:		11/18/98 AM.
* SUBJ:	Push element on front of list.
* NOTE:	Two variants: push data or an actual elt.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::push(SELTTYPE *dd)
{
Selt<SELTTYPE> *selt;
// This sets the pRight pointer:
selt = new Selt<SELTTYPE>(dd, pFirst);	// Make elt for data.
if (!pFirst)							// List empty.
	pLast = selt;
pFirst = selt;
}


// WARNING: Caller must assure that selt is not attached to a list.
template<class SELTTYPE>
void Slist<SELTTYPE>::push(Selt<SELTTYPE> *selt)
{
#ifndef STABLE_
if (selt->pRight)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[push: Given element is part of another list.]") << std::ends;
	errOut(&gerrStr,false);
	}

#endif

if (pFirst)					// List not empty.
	selt->pRight = pFirst;
else							// Empty list.
	pLast = selt;

pFirst = selt;
}

/********************************************
* FN:		RPUSH
* CR:		11/18/98 AM.
* SUBJ:	Push element onto back of list.
* NOTE:	Two variants: push data or an actual elt.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::rpush(SELTTYPE *dd)
{
Selt<SELTTYPE> *selt;
selt = new Selt<SELTTYPE>(dd);	// Make elt for data.
if (pLast)			// List not empty.
	{
	pLast->pRight = selt;
	pLast = selt;
	}
else					// If list is empty.
	{
	pLast = pFirst = selt;
	}
}

// WARNING: Caller must assure that selt is not attached to a list.
// OPT: An efficient caller could leave pLeft, so not checking it.
// NOTE:	We have a choice here.  Could allow the element to be the
//			first of a list and let rpush glom them.  But not allowing
//			this, so that a merge function could do it in principled fashion.
template<class SELTTYPE>
void Slist<SELTTYPE>::rpush(Selt<SELTTYPE> *selt)
{
if (selt->pRight)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rpush: Given element is part of a list.]") << std::ends;
	errOut(&gerrStr,false);
	}

	selt->pRight = 0;
	}
if (pLast)			// List not empty.
	{
	pLast->pRight = selt;
	pLast = selt;
	}
else					// If list is empty.
	{
	pLast = pFirst = selt;
	}
}


/********************************************
* FN:		POP
* CR:		11/18/98 AM.
* SUBJ:	Pop element from front of list.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE> *Slist<SELTTYPE>::pop()
{
if (!pFirst)
	return 0;

Selt<SELTTYPE> *tmp;
tmp = pFirst;
pFirst = pFirst->pRight;
if (pFirst)
	tmp->pRight = 0;
else
	pLast = 0;
return tmp;
}


/********************************************
* FN:		RPOP
* CR:		11/18/98 AM.
* SUBJ:	Pop element from back of list.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE> *Slist<SELTTYPE>::rpop()
{
if (!pFirst)
	return 0;

Selt<SELTTYPE> *tmp;

if (!pFirst->pRight)		// List of one element.	// 11/18/98 AM.
	{
	tmp = pFirst;
	pFirst = pLast = 0;
	return tmp;
	}

// Traverse to next-to-last element.			// 11/18/98 AM.
for (tmp = pFirst; (tmp->pRight && tmp->pRight->pRight); tmp = tmp->pRight)
	;

Selt<SELTTYPE> *save;
save = tmp->pRight;

tmp->pRight = 0;
pLast = tmp;
return save;
}


/********************************************
* FN:		GETNTH
* CR:		11/18/98 AM.
* SUBJ:	Get nth element of list.
* NOTE:	Count of elements is one-based.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE> *Slist<SELTTYPE>::getNth(long ord)
{
Selt<SELTTYPE> *ptr;
for (ptr = pFirst,--ord; ptr, ord > 0; ptr = ptr->pRight, --ord)
	;
return ptr;
}


/********************************************
* FN:		RMNEXT
* CR:		11/18/98 AM.
* SUBJ:	Remove the next element from list.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE> *Slist<SELTTYPE>::rmNext(Selt<SELTTYPE> *pre)
{
if (!pre)
	return 0;
Selt<SELTTYPE> *elt;
if (!(elt = pre->pRight))
	return 0;
pre->pRight = elt->pRight;
elt->pRight = 0;
return elt;
}


/********************************************
* FN:		GETLENGTH
* CR:		11/18/98 AM.
* SUBJ:	Get length of list.
* NOTE:	The loop is slightly devious.  Count is always one behind,
*			since a NULL pointer will eventually be reached.
********************************************/

template<class SELTTYPE>
long Slist<SELTTYPE>::getLength()
{
Selt<SELTTYPE> *ptr;
long len;
for (ptr = pFirst, len = 0; ptr; ptr = ptr->pRight, ++len)
	;
return len;
}


/********************************************
* FN:		CMPNUMSLIST
* CR:		11/18/98 AM.
* SUBJ:	Compare number to length of list.
* RET:	1 if number is greater, 0 if equal, -1 if less..

********************************************/

template<class SELTTYPE>
int Slist<SELTTYPE>::CmpNumSlist(long num)
{
Selt<SELTTYPE> *ptr;
if (num < 0)
	return -1;
for (ptr = pFirst; ptr, num > 0; ptr = ptr->pRight, --num)
	;
if (!ptr && !num)
	return 0;
if (!ptr)
	return 1;
return -1;	// List is longer.
}


/********************************************
* FN:		INSERTLEFT
* CR:		11/18/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::insertLeft(
	Selt<SELTTYPE> *selt,
	Selt<SELTTYPE> *pos
	)
{
if (!selt || !pos)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: Given NULL ptr]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}
if (selt->pRight)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: Given elt is in another list.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

if (pFirst == pos)			// Insert at front of list.
	{
	push(selt);
	return;
	}

// Traverse to one before given pos.
Selt<SELTTYPE> *tmp;
for (tmp = pFirst; tmp && tmp->pRight != pos; tmp = tmp->pRight)
	;

if (!tmp)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: given position not in list.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

selt->pRight = pos;
tmp->pRight = selt;
}


/********************************************
* FN:		INSERTRIGHT
* CR:		11/18/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::insertRight(
	Selt<SELTTYPE> *selt,
	Selt<SELTTYPE> *pos
	)
{
if (!selt || !pos)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[insertRight: Given NULL element.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}
if (selt->pRight)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[insertRight: Element in another list.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

if (pos->pRight)
	selt->pRight = pos->pRight;
else
	pLast = selt;

pos->pRight = selt;
}


/********************************************
* FN:		INSERTRIGHT
* CR:		12/18/98 AM.
* SUBJ:	Insert DATA to the right of given element.
* NOTE:	Not checking that pos,node in current list.
*			ALLOWING NULL DATA.
* ALLOC:	Creates new list element.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::insertRight(
	SELTTYPE *dat,
	Selt<SELTTYPE> *pos
	)
{
if (!pos)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[insertRight: Given NULL element.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

Selt<SELTTYPE> *tmp;
tmp = new Selt<SELTTYPE>(dat);

if (pos->pRight)
	tmp->pRight = pos->pRight;
else
	pLast = tmp;

pos->pRight = tmp;
}


/********************************************
* FN:		TRAVERSE
* CR:		11/18/98 AM.
* NOTE:	Need indentation.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::Traverse(
	const Selt<SELTTYPE> *selt) const
{
for (; selt; selt = selt->pRight)
// FIX FROM THE LINUX CODE.	// 03/20/19 AM.
//	std::_t_cout << pn->data < _T(" ");
	std::_t_cout << selt->data < _T(" "); // FIX.	// 04/26/07 AM.
}


/********************************************
* FN:		DUMP
* CR:		11/18/98 AM.
* NOTE:	Traverse the entire tree.  OMITS THE ROOT.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::Dump() const
{
Traverse(pFirst);
}


/********************************************
* FN:		DELETESLISTANDDATA
* CR:		11/18/98 AM.
* SUBJ:	Delete the elements and the list object.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::DeleteSlistAndData(Slist<SELTTYPE> *slist)
{
if (!slist) return;
//DelSlistAndData(slist->pFirst);			// 11/24/98 AM. Fix.
//slist->pFirst = slist->pLast = 0;			// 11/24/98 AM. Fix.
delete slist;
}


/********************************************
* FN:		DELSLISTANDDATA
* CR:		11/18/98 AM.
* SUBJ:	Delete the elements of the list.
* WARN:	Given sublist had better be detached from previous elt,
*			or we'll bomb.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::DelSlistAndData(Selt<SELTTYPE> *selt)
{
if (!selt)
	return;

Selt<SELTTYPE> *tmp;
while ((tmp = selt))
	{
	selt = selt->pRight;
	Selt<SELTTYPE>::DeleteSeltAndData(tmp);
	}
}


/********************************************
* FN:		DELETESLIST
* CR:		12/16/98 AM.
* SUBJ:	Delete the elements and the list object, but NOT the data.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::DeleteSlist(Slist<SELTTYPE> *slist)
{
if (!slist) return;
DelSlist(slist->pFirst);
slist->pFirst = slist->pLast = 0;
delete slist;
}


/********************************************
* FN:		DELSLIST
* CR:		12/16/98 AM.
* SUBJ:	Delete the elements of the list, but NOT the data.
* WARN:	Given sublist had better be detached from previous elt,
*			or we'll bomb.
********************************************/

template<class SELTTYPE>
void Slist<SELTTYPE>::DelSlist(Selt<SELTTYPE> *selt)
{
if (!selt)
	return;

Selt<SELTTYPE> *tmp;
while ((tmp = selt))
	{
	selt = selt->pRight;
	Selt<SELTTYPE>::DeleteSelt(tmp);
	}
}


/********************************************
* FN:		EMPTY
* CR:		06/20/00 AM.
* SUBJ:	See if given list is empty.
********************************************/

template<class SELTTYPE>
bool Slist<SELTTYPE>::empty(Slist<SELTTYPE> *slist)
{
if (!slist || !slist->pFirst)
	return true;
return false;
}


/********************************************
* FN:		MERGE
* CR:		06/30/99 AM.
* SUBJ:	Merge two lists.
* WARN:	DOES NOT COPY LISTS.  FREES OR USES THE SECOND LIST OBJECT.
*			CALLER MUST RESET POINTERS TO LISTS.
*			CALLER SHOULD USE THE RETURN OF THIS FUNCTION.
* NOTE:	Doesn't work quite the same way as the dlist version.
*			Assumes first list is getting the result.
********************************************/

template<class SELTTYPE>
Slist<SELTTYPE> *Slist<SELTTYPE>::merge(
	Slist<SELTTYPE> *list1,
	Slist<SELTTYPE> *list2
	)
{
if (!list2)
	return list1;
if (!list1)
	{
	return list2;
	}

if (list1->pFirst && list2->pFirst)			// Both not empty.
	{
	list1->pLast->pRight = list2->pFirst;
	list1->pLast = list2->pLast;

	list2->pFirst = list2->pLast = 0;
	delete list2;
	return list1;
	}
else if (list1->pFirst)				// 1st not empty.
	{
	delete list2;
	return list1;
	}
else if (list2->pFirst)				// 2nd not empty.
	{
	delete list1;
	return list2;
	}

// Both empty.
delete list2;
return list1;
}


/********************************************
* FN:		GENCONFLICTS
* CR:		06/17/00 AM.
* SUBJ:	Gen code for a conflict set of a hash table.
* NOTE:	Just a way to xfer control to the specific data type.
********************************************/

template<class SELTTYPE>
long Slist<SELTTYPE>::genConflicts(std::_t_ostream *ostr)
{
return SELTTYPE::genConflicts(this, ostr);
}

#endif
//////////////////////// END OF FILE ////////////////////////////////
