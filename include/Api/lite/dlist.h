/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
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
*			07/07/03 AM. Function definitions out of here.
*
*******************************************************************************/

#ifndef DLIST_H
#define DLIST_H
#include <assert.h>
#ifdef LINUX
#include <sstream>
#else
#include <strstream>															// 05/25/01 AM.
#endif

#include "lite.h"
#include "delt.h"


/********************************************
* CLASS:	DLIST
* CR:		10/17/98 AM.
********************************************/

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
		std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
//////////////////////// END OF FILE ////////////////////////////////
