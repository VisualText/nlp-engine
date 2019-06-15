/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DELT.H
* FILE:	include/API/lite/delt.h
* CR:		10/17/98 AM.
* SUBJ:	Declares for a doubly-linked list.
* NOTE:	Copied and modified node.h.
*			07/07/03 AM. Function definitions out of here.
* WARN:	This (and tree.h,node.h) won't work on simple types.
*			As needed, build separate template classes for those.
*
*******************************************************************************/

#ifndef DELT_H
#define DELT_H

#include "lite.h"

/********************************************
* CLASS:	DELT
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
class LITE_API Delt
{
// Visual C++ bug.
//	friend class Tree<DELTTYPE>;

public:
	// 07/02/99 AM. NEW WAY.
	//Delt(const DELTTYPE &, Delt * = 0, Delt * = 0);	// Constructor
	Delt(DELTTYPE *, Delt * = 0, Delt * = 0);	// Constructor
	~Delt();									// Destructor
	DELTTYPE *getData();					// Delt's data.
	void setData(DELTTYPE *);			// Set data.			// 02/17/00 AM.

	Delt *Left();
	Delt *Right();

	void setLeft(Delt<DELTTYPE> *);	// 01/25/99 AM. !!
	void setRight(Delt<DELTTYPE> *);	// 01/25/99 AM. !!

	static void DeleteDeltAndData(Delt<DELTTYPE> *);

// 10/17/98 AM. WORKAROUND. Making these data elements public till Visual C++
// enables a template class to be a friend of another class.
public:
//private:
	//DELTTYPE data;							// Data associated with node.
	DELTTYPE *data;							// 07/02/99 AM.

	Delt *pLeft;							// Delt's left  sibling.
	Delt *pRight;							// Delt's right sibling.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_TCHAR * = _T(""),_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


//////////////////////// END OF FILE ////////////////////////////////
#endif
