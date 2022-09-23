/*******************************************************************************
Copyright � 1998-2003 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DLIST.H
* FILE:	include/API/lite/dlist.h
* CR:		10/17/98 AM.
* SUBJ:	Declares for a doubly-linked list.
* NOTE:	Copied and modified tree.h.
*			Derived classes could track more data, like length of list.
*			(And put last elt tracking there also.)
*
*******************************************************************************/

#ifndef DLIST_HI
#define DLIST_HI
#include <assert.h>
#ifdef LINUX
//#include <std::_t_strstream.h>
#include <sstream>
#else
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif

#include "lite.h"
#include "delt.h"


/********************************************
* CLASS:	DLIST
* CR:		10/17/98 AM.
********************************************/

#include "lite/dlist.h"

#ifdef OLD_030707_
template<class DELTTYPE>
class LITE_API Dlist
{
public:
	Dlist();							// Default constructor. Makes empty list.
	Dlist(DELTTYPE *);			// Constructor. Takes elt value.
	Dlist(Delt<DELTTYPE> *);	// Constructor. Takes an element.
	~Dlist();						// Destructor.
	Dlist(const Dlist<DELTTYPE> &);		// Copy constructor.	// 11/30/98 AM.

	// Special copy and delete.										// 02/18/00 AM.
	// Just the list and delts are copied and deleted, NOT THE DATA.
	// For the data, only the pointer is copied.
	// Caller must manage the data manually.
	Dlist<DELTTYPE> *copy_list();										// 02/18/00 AM.
	static void delete_list(Dlist<DELTTYPE> *);					// 02/18/00 AM.

	// Access
	Delt<DELTTYPE> *getFirst();
	Delt<DELTTYPE> *getLast();

	// Modify.
	void setFirst(Delt<DELTTYPE> *);
	void setLast(Delt<DELTTYPE> *);

	void push(DELTTYPE *);
	void push(Delt<DELTTYPE> *);
	void rpush(DELTTYPE *);
	void rpush(Delt<DELTTYPE> *);
	Delt<DELTTYPE> *pop();
	Delt<DELTTYPE> *rpop();		// Pop from end.

	Delt<DELTTYPE> *getNth(long);
	long getLength();
	int CmpNumDlist(long);		// Compare number to length. 1,0,-1.
	long getOrd(Delt<DELTTYPE> *);	// 11/22/98 AM.
	
	// Insert, given an element of the list.
	void insertLeft( Delt<DELTTYPE> *delt, Delt<DELTTYPE> *pos);
	void insertRight( Delt<DELTTYPE> *delt, Delt<DELTTYPE> *pos);

	// Insert at given ordinality position.
	Delt<DELTTYPE> *insertAfter(DELTTYPE *dat, long ord);			 // 01/14/99 AM.
	Delt<DELTTYPE> *insertAfter(Delt<DELTTYPE> *delt, long ord); // 01/14/99 AM.

	// Excise from list.
	Delt<DELTTYPE> *exciseNth(long ord);							// 01/14/99 AM.
	Delt<DELTTYPE> *exciseElt(Delt<DELTTYPE> *delt);			// 06/01/99 AM.

	// Detach and splice a range of elts.							// 10/27/99 AM.
	bool excise(Delt<DELTTYPE> *start, Delt<DELTTYPE> *end); // 10/27/99 AM.
	bool spliceAfter(Delt<DELTTYPE> *start, Delt<DELTTYPE> *end,
								long ord);									// 10/27/99 AM.
	bool spliceAfter(Delt<DELTTYPE> *start, Delt<DELTTYPE> *end,
								Delt<DELTTYPE> *pos);					// 10/27/99 AM.

	void Traverse(const Delt<DELTTYPE> *) const;
	void Dump() const;			// Traverse the entire list.

	static void DeleteDlistAndData(Dlist<DELTTYPE> *);
	static void DelDlistAndData(Delt<DELTTYPE> *);
	static Dlist<DELTTYPE> *merge(
		Dlist<DELTTYPE> *list1,
		Dlist<DELTTYPE> *list2
		);

private:
	Delt<DELTTYPE> *pFirst;	// Point to first element.
	Delt<DELTTYPE> *pLast;	// Point to last element.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_TCHAR * = _T(""),
							std::_t_ofstream* = 0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};
#endif

/********************************************
* FN:		DLIST (constructor)
* CR:		10/17/98 AM.
* NOTE:	Not checking that element is a loner.
********************************************/
template<class DELTTYPE>
Dlist<DELTTYPE>::Dlist()
{
pFirst = pLast = 0;

#ifndef STABLE_
++count_;
#endif
}

template<class DELTTYPE>
Dlist<DELTTYPE>::Dlist(DELTTYPE *dd)
{
Delt<DELTTYPE> *delt;

//delt = new Delt<DELTTYPE>(*dd);			// 07/02/99 AM.
delt = new Delt<DELTTYPE>(dd);				// 07/02/99 AM.
pFirst = pLast = delt;

#ifndef STABLE_
++count_;
#endif
}

// WARNING: element passed to this function should have
// these values set.  Caller must assure this.
//delt->pLeft = delt->pRight = 0;
template<class DELTTYPE>
Dlist<DELTTYPE>::Dlist(Delt<DELTTYPE> *delt)
{
pFirst = pLast = delt;

// While code being built, do this check.
#ifndef STABLE_
if (delt->pLeft || delt->pRight)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Dlist: Given element is part of another list.]") << std::ends;
	errOut(&gerrStr,false);

	delt->pLeft = delt->pRight = 0;
	}
#endif

#ifndef STABLE_
++count_;
#endif
}

template<class DELTTYPE>
Dlist<DELTTYPE>::~Dlist()
{
#ifndef STABLE_
--count_;
#endif
}

//Dlist<DELTTYPE>::Dlist<DELTTYPE>(const Dlist<DELTTYPE> &orig)

// Copy constructor
template<class DELTTYPE>
Dlist<DELTTYPE>::Dlist(const Dlist<DELTTYPE> &orig)	// 08/28/12 AM.
// (The above is same form as LINUX version.)	// 03/20/19 AM.
{
//*gerr << "[Dlist copy constructor]" << std::endl;
pFirst = pLast = 0;
// Copy the list.
Delt<DELTTYPE> *ptr;
ptr = orig.pFirst;
while (ptr)
	{
	if (ptr->data)			// 07/02/99 AM.
		{
		DELTTYPE *tmp;				// 07/02/99 AM.
		tmp = new DELTTYPE(*(ptr->data));	// COPY OBJECT.	// 07/02/99 AM.
	//rpush(&(ptr->data));		// 07/02/99 AM.
	    rpush(tmp);				// 07/02/99 AM.
		}
	ptr = ptr->pRight;
	}

#ifndef STABLE_
++count_;
#endif
}

/********************************************
* FN:		Access functions.
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::getFirst()
{
return pFirst;
}

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::getLast()
{
return pLast;
}


/********************************************
* FN:		Modify functions.
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::setFirst(Delt<DELTTYPE> *delt)
{
pFirst = delt;
}

template<class DELTTYPE>
void Dlist<DELTTYPE>::setLast(Delt<DELTTYPE> *delt)
{
pLast = delt;
}

/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class DELTTYPE>
int Dlist<DELTTYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class DELTTYPE>
void Dlist<DELTTYPE>::prettyCount(_TCHAR *str,
	std::_t_ofstream *ofstr)
{
if (count_)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Dlist<") << str << _T("> count=")
		  << count_ << std::ends;
	errOut(&gerrStr,false);
	}

	*gout << _T("Active Dlist<") << str << _T("> count=")
		  << count_ << std::endl;

	if (ofstr)
		*ofstr << _T("Active Dlist<") << str << _T("> count=")
		  << count_ << std::endl;
	}
}
#endif

/********************************************
* FN:		PUSH
* CR:		10/17/98 AM.
* SUBJ:	Push element on front of list.
* NOTE:	Two variants: push data or an actual elt.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::push(DELTTYPE *dd)
{
Delt<DELTTYPE> *delt;
// 07/02/99 AM. Caller responsible for copying as needed.
delt = new Delt<DELTTYPE>(dd, 0, pFirst);	// Make elt for data.
//delt = new Delt<DELTTYPE>(*dd, 0, pFirst);	// Make elt for data.
if (pFirst)					// List not empty
	pFirst->pLeft = delt;
else							// List empty.
	pLast = delt;
pFirst = delt;
}


// WARNING: Caller must assure that delt is not attached to a list.
template<class DELTTYPE>
void Dlist<DELTTYPE>::push(Delt<DELTTYPE> *delt)
{
#ifndef STABLE_
if (delt->pLeft || delt->pRight)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[push: Given element is part of another list.]") << std::ends;
	errOut(&gerrStr,false);

	delt->pLeft = 0;
	}
#endif

if (pFirst)					// List not empty.
	{
	pFirst->pLeft = delt;
	delt->pRight = pFirst;
	}
else							// Empty list.
	pLast = delt;

pFirst = delt;
}

/********************************************
* FN:		RPUSH
* CR:		10/17/98 AM.
* SUBJ:	Push element onto back of list.
* NOTE:	Two variants: push data or an actual elt.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::rpush(DELTTYPE *dd)
{
Delt<DELTTYPE> *delt;
// 07/02/99 AM. Caller responsible for copying as needed.
//delt = new Delt<DELTTYPE>(*dd);	// Make elt for data.
delt = new Delt<DELTTYPE>(dd);	// Make elt for data.
if (pLast)			// List not empty.
	{
	pLast->pRight = delt;
	delt->pLeft = pLast;
	pLast = delt;
	}
else					// If list is empty.
	{
	pLast = pFirst = delt;
	}
}

// WARNING: Caller must assure that delt is not attached to a list.
// OPT: An efficient caller could leave pLeft, so not checking it.
// NOTE:	We have a choice here.  Could allow the element to be the
//			first of a list and let rpush glom them.  But not allowing
//			this, so that a merge function could do it in principled fashion.
template<class DELTTYPE>
void Dlist<DELTTYPE>::rpush(Delt<DELTTYPE> *delt)
{
if (delt->pLeft || delt->pRight)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[rpush: Given element is part of a list.]") << std::ends;
	errOut(&gerrStr,false);

	delt->pLeft = delt->pRight = 0;
	}
if (pLast)			// List not empty.
	{
	pLast->pRight = delt;
	delt->pLeft = pLast;
	pLast = delt;
	}
else					// If list is empty.
	{
	pLast = pFirst = delt;
	}
}


/********************************************
* FN:		POP
* CR:		10/17/98 AM.
* SUBJ:	Pop element from front of list.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::pop()
{
if (!pFirst)
	return 0;

Delt<DELTTYPE> *tmp;
tmp = pFirst;
pFirst = pFirst->pRight;
if (pFirst)
	{
	tmp->pRight = 0;
	pFirst->pLeft = 0;
	}
else
	pLast = 0;
return tmp;
}


/********************************************
* FN:		RPOP
* CR:		10/17/98 AM.
* SUBJ:	Pop element from back of list.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::rpop()
{
if (!pLast)
	return 0;

Delt<DELTTYPE> *tmp;
tmp = pLast;
pLast = pLast->pLeft;
if (pLast)
	{
	tmp->pLeft = 0;
	pLast->pRight = 0;
	}
else
	pFirst = 0;
return tmp;
}


/********************************************
* FN:		GETNTH
* CR:		10/17/98 AM.
* SUBJ:	Get nth element of list.
* NOTE:	Count of elements is one-based.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::getNth(long ord)
{
Delt<DELTTYPE> *ptr;
for (ptr = pFirst,--ord; ptr, ord > 0; ptr = ptr->pRight, --ord)
	;
return ptr;
}


/********************************************
* FN:		GETLENGTH
* CR:		10/17/98 AM.
* SUBJ:	Get length of list.
* NOTE:	The loop is slightly devious.  Count is always one behind,
*			since a NULL pointer will eventually be reached.
********************************************/

template<class DELTTYPE>
long Dlist<DELTTYPE>::getLength()
{
Delt<DELTTYPE> *ptr;
long len;
for (ptr = pFirst, len = 0; ptr; ptr = ptr->pRight, ++len)
	;
return len;
}


/********************************************
* FN:		GETORD
* CR:		11/22/98 AM.
* SUBJ:	Find element's ordinal place in list.
* RET:	Ordinal place, or 0 if not found.
********************************************/

template<class DELTTYPE>
long Dlist<DELTTYPE>::getOrd(Delt<DELTTYPE> *delt)
{
long ord;
ord = 1;
Delt<DELTTYPE> *ptr;
if (!delt)
	return 0;
ptr = getFirst();
while (ptr && ptr != delt)
	{
	++ord;
	ptr = ptr->Right();
	}
if (ptr == delt)
	return ord;
return 0;
}


/********************************************
* FN:		CMPNUMDLIST
* CR:		10/17/98 AM.
* SUBJ:	Compare number to length of list.
* RET:	1 if number is greater, 0 if equal, -1 if less..

********************************************/

template<class DELTTYPE>
int Dlist<DELTTYPE>::CmpNumDlist(long num)
{
Delt<DELTTYPE> *ptr;
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
* CR:		10/17/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::insertLeft(
	Delt<DELTTYPE> *delt,
	Delt<DELTTYPE> *pos
	)
{
if (!delt || !pos)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: Given NULL ptr]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}
if (delt->pLeft || delt->pRight)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: Given elt is in another list.]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}

if (pos->pLeft)
	{
	delt->pLeft = pos->pLeft;
	pos->pLeft->pRight = delt;
	}
else		// Pos is first in list.
	pFirst = delt;

pos->pLeft = delt;
delt->pRight = pos;
}


/********************************************
* FN:		INSERTRIGHT
* CR:		10/17/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::insertRight(
	Delt<DELTTYPE> *delt,
	Delt<DELTTYPE> *pos
	)
{
if (!delt || !pos)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[insertRight: Given NULL element.]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}
if (delt->pLeft || delt->pRight)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[insertRight: Element in another list.]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}

if (pos->pRight)
	{
	delt->pRight = pos->pRight;
	pos->pRight->pLeft = delt;
	}
else
	pLast = delt;

pos->pRight = delt;
delt->pLeft = pos;
}


/********************************************
* FN:		INSERTAFTER
* CR:		01/14/99 AM.
* NOTE:	ord == 0 means insert as first element of list.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::insertAfter(
	DELTTYPE *dat,
	long ord
	)
{
if (!dat || ord < 0)
	return 0;

Delt<DELTTYPE> *delt;

// Looks like this wasn't updated.  // FIX.	// 08/04/99 AM.
//delt = new Delt<DELTTYPE>(*dat);	// FIX.	// 08/04/99 AM.
delt = new Delt<DELTTYPE>(dat);		// FIX.	// 08/04/99 AM.
if (!ord)			// Making this the first element.
	push(delt);
else
	{
	// Find the position to insert after.
	// Make sure length of list is ok!
	Delt<DELTTYPE> *list;
	list = pFirst;
	while (--ord)
		{
		if (!list || !(list = list->pRight))
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[insertAfter: Position longer than list.]") << std::ends;
			errOut(&gerrStr,false);

			Delt<DELTTYPE>::DeleteDeltAndData(delt);
			return 0;
			}
		}
	insertRight(delt, list);
	}
return delt;
}


/********************************************
* FN:		INSERTAFTER
* CR:		01/14/99 AM.
* NOTE:	ord == 0 means insert as first element of list.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::insertAfter(
	Delt<DELTTYPE> *delt,
	long ord
	)
{
if (!delt || ord < 0)
	return 0;

if (!ord)			// Making this the first element.
	push(delt);
else
	{
	// Find the position to insert after.
	// Make sure length of list is ok!
	Delt<DELTTYPE> *list;
	list = pFirst;
	while (--ord)
		{
		if (!list || !(list = list->pRight))
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[insertAfter: Position longer than list.]") << std::ends;
			errOut(&gerrStr,false);

			return 0;
			}
		}
	insertRight(delt, list);
	}
return delt;
}


/********************************************
* FN:		EXCISENTH
* CR:		01/14/99 AM.
* SUBJ:	Remove the nth element from the list.
* NOTE:	Caller must dispose of this element.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::exciseNth(
	long ord
	)
{
if (ord <= 0)
	return 0;

Delt<DELTTYPE> *delt;
if (!(delt = getNth(ord)))
	return 0;

return exciseElt(delt);
}


#ifdef OLDY_
template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::exciseNth(
	long ord
	)
{
if (ord <= 0)
	return 0;
else if (ord == 1)			// First elt of list.
	return pop();

Delt<DELTTYPE> *delt;
if (!(delt = getNth(ord)))
	return 0;

if (delt->pLeft)
	delt->pLeft->pRight = delt->pRight;
if (delt->pRight)
	delt->pRight->pLeft = delt->pLeft;
delt->pLeft  = 0;
delt->pRight = 0;

if (delt != pLast)			// Embedded elt.
	return delt;

// It's the last elt.
return rpop();
}
#endif


/********************************************
* FN:		EXCISEELT
* CR:		06/01/99 AM.
* SUBJ:	Remove the element from the list.
* NOTE:	Variant.
* ASS:	Assumes element is attached to current list.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Dlist<DELTTYPE>::exciseElt(
	Delt<DELTTYPE> *delt
	)
{
if (!delt)
	return 0;
if (!(delt->pLeft))
	return pop();			// If first elt of list.
if (!(delt->pRight))		// If last elt of list
	return rpop();

delt->pLeft->pRight = delt->pRight;
delt->pRight->pLeft = delt->pLeft;
delt->pLeft  = 0;
delt->pRight = 0;

return delt;
}


/********************************************
* FN:		EXCISE
* CR:		10/27/99 AM.
* SUBJ:	Remove a range of elts from the list.
* ASS:	Assumes elts attached to current list.
********************************************/

template<class DELTTYPE>
bool Dlist<DELTTYPE>::excise(
	Delt<DELTTYPE> *start,
	Delt<DELTTYPE> *end
	)
{
if (!start || !end)
	return false;

if (!(start->pLeft))		// If first elt of list.
	{
	pFirst = end->pRight;
	if (!pFirst)
		pLast = 0;			// Emptied out the list.
	else
		pFirst->pLeft = 0;
	end->pRight = 0;
	return true;
	}

if (!(end->pRight))		// If last elt of list.
	{
	pLast = start->pLeft;
	// start shouldn't be first elt of list.
	assert(pLast);
	pLast->pRight = 0;
	start->pLeft = 0;
	return true;
	}

// Else, embedded in list.
start->pLeft->pRight = end->pRight;
end->pRight->pLeft = start->pLeft;
start->pLeft = 0;
end->pRight = 0;
return true;
}


/********************************************
* FN:		SPLICEAFTER
* CR:		10/27/99 AM.
* SUBJ:	Splice a range of elts into a list.
********************************************/

template<class DELTTYPE>
bool Dlist<DELTTYPE>::spliceAfter(
	Delt<DELTTYPE> *start,
	Delt<DELTTYPE> *end,
	long ord
	)
{
if (!start || !end || ord < 0)
	return false;
if (start->pLeft || end->pRight)	// Just makin' sure.
	return false;

if (!ord)			// Put elts at front of list.
	{
	end->pRight = pFirst;
	if (pFirst)
		pFirst->pLeft = end;
	else				// Splicing to empty list.
		pLast = end;
	pFirst = start;
	return true;
	}

// Ord > 0.
// Find the position to insert after.
// Make sure length of list is ok!
Delt<DELTTYPE> *list;
list = pFirst;
while (--ord)
	{
	if (!list || !(list = list->pRight))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[spliceAfter: Position longer than list.]") << std::ends;
		errOut(&gerrStr,false);

		return 0;
		}
	}

// Insert after list elt.
end->pRight = list->pRight;
if (list->pRight)
	list->pRight->pLeft = end;
else
	pLast = end;
start->pLeft = list;
list->pRight = start;
return true;
}


/********************************************
* FN:		SPLICEAFTER
* CR:		10/27/99 AM.
* SUBJ:	Splice a range of elts into a list.
* NOTE:	Allowing null pos, I guess.
********************************************/

template<class DELTTYPE>
bool Dlist<DELTTYPE>::spliceAfter(
	Delt<DELTTYPE> *start,
	Delt<DELTTYPE> *end,
	Delt<DELTTYPE> *pos)
{
if (!start || !end)
	return false;
if (start->pLeft || end->pRight)	// Just makin' sure.
	return false;

if (!pos)			// List is empty.
	{
	pFirst = start;
	pLast = end;
	return true;
	}


// Insert after pos.
end->pRight = pos->pRight;
if (pos->pRight)
	pos->pRight->pLeft = end;
else
	pLast = end;
start->pLeft = pos;
pos->pRight = start;
return true;
}


/********************************************
* FN:		TRAVERSE
* CR:		10/17/98 AM.
* NOTE:	Need indentation.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::Traverse(
	const Delt<DELTTYPE> *delt) const
{
for (; delt; delt = delt->pRight)
// This was a fix from the LINUX code.	// 03/20/19 AM.
//	std::_t_cout << pn->data < _T(" ");
	std::_t_cout << delt->data < _T(" ");
}


/********************************************
* FN:		DUMP
* CR:		10/17/98 AM.
* NOTE:	Traverse the entire tree.  OMITS THE ROOT.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::Dump() const
{
Traverse(pFirst);
}


/********************************************
* FN:		MERGE
* CR:		11/24/98 AM.
* SUBJ:	Merge two lists.
* NOTE:	07/16/99 AM. Fill list1 whenever it exists.
* WARN:	DOES NOT COPY LISTS.  FREES OR USES THE SECOND LIST OBJECT.
********************************************/

template<class DELTTYPE>
Dlist<DELTTYPE> *Dlist<DELTTYPE>::merge(
	Dlist<DELTTYPE> *list1,
	Dlist<DELTTYPE> *list2
	)
{
if (!list2)
	return list1;
if (!list1)
	return list2;

if (list1->pFirst && list2->pFirst)			// Both not empty.
	{
	list1->pLast->pRight = list2->pFirst;
	list2->pFirst->pLeft = list1->pLast;
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
	// 07/16/99 AM. Transfer to list1.
	list1->pFirst = list2->pFirst;
	list1->pLast = list2->pLast;
	list2->pFirst = list2->pLast = 0;
	delete list2;
	return list1;
	}

delete list2;
return list1;
}


/********************************************
* FN:		DELETEDLISTANDDATA
* CR:		10/17/98 AM.
* SUBJ:	Delete the elements and the list object.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::DeleteDlistAndData(Dlist<DELTTYPE> *dlist)
{
if (!dlist) return;
DelDlistAndData(dlist->pFirst);
delete dlist;
}


/********************************************
* FN:		DELDLISTANDDATA
* CR:		10/17/98 AM.
* SUBJ:	Delete the elements of the list.
* WARN:	Given sublist had better be detached from previous elt,
*			or we'll bomb.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::DelDlistAndData(Delt<DELTTYPE> *delt)
{
if (!delt)
	return;
if (delt->pLeft)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[DelDlistAndData: Given sublist is not detached.]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}

Delt<DELTTYPE> *tmp;
while ((tmp = delt))
	{
	delt = delt->pRight;
	Delt<DELTTYPE>::DeleteDeltAndData(tmp);
	}
}


/********************************************
* FN:		COPY_LIST
* CR:		02/18/00 AM.
* SUBJ:	Copy the list object, elements, and data POINTERS only.
* WARN:	Caller must manage the data manually.
********************************************/

template<class DELTTYPE>
Dlist<DELTTYPE> *Dlist<DELTTYPE>::copy_list()
{
Dlist<DELTTYPE> *nlist;
Delt<DELTTYPE> *delt, *ndelt;
nlist = new Dlist<DELTTYPE>();	// Empty list.
for (delt = this->getFirst(); delt; delt = delt->Right())
	{
	ndelt = new Delt<DELTTYPE>(delt->data);
	nlist->rpush(ndelt);
	}
return nlist;
}


/********************************************
* FN:		DELETE_LIST
* CR:		02/18/00 AM.
* SUBJ:	Delete the list object and elements, but NOT THE DATA.
* WARN:	Caller must manage the data manually.
********************************************/

template<class DELTTYPE>
void Dlist<DELTTYPE>::delete_list(Dlist<DELTTYPE> *list)
{
if (!list)
	return;
Delt<DELTTYPE> *delt, *tmp;
delt = list->pFirst;
while (delt)
	{
	tmp = delt->pRight;
	delt->data = 0;
	delete delt;
	delt = tmp;
	}
list->pFirst = 0;
list->pLast = 0;
delete list;
}

#endif
//////////////////////// END OF FILE ////////////////////////////////
