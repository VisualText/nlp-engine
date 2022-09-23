/*******************************************************************************
Copyright � 1998-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DELT.H
* FILE:	include/API/lite/delt.h
* CR:		10/17/98 AM.
* SUBJ:	Declares for a doubly-linked list.
* NOTE:	Copied and modified node.h.
* WARN:	This (and tree.h,node.h) won't work on simple types.
*			As needed, build separate template classes for those.
*
*******************************************************************************/

#ifndef DELT_H
#define DELT_H
#ifdef LINUX
#include <sstream>
#else
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif
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
	static void prettyCount(_TCHAR * = _T(""),
								std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


/********************************************
* FN:		DELT (constructor)
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE>::Delt(DELTTYPE *dd, Delt *ll, Delt *rr)	// 07/02/99AM
//Delt<DELTTYPE>::Delt(const DELTTYPE &dd, Delt *ll, Delt *rr) // 07/02/99AM
{
// HERE'S A BIG SOURCE OF TROUBLE! // 07/02/99 AM.
//data = dd;								// 07/02/99 AM.
data = dd;		// Just taking a POINTER. NOT COPYING!!!	// 07/02/99 AM.
pLeft = ll;
pRight = rr;
#ifndef STABLE_
++count_;
#endif
}


/********************************************
* FN:		~DELT (destructor)
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE>::~Delt()
{
#ifndef STABLE_
--count_;
#endif
}


/********************************************
* FN:		Access functions.
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
Delt<DELTTYPE> *Delt<DELTTYPE>::Left()	 { return pLeft;  }

template<class DELTTYPE>
Delt<DELTTYPE> *Delt<DELTTYPE>::Right() { return pRight; }

/********************************************
* FN:		Access functions.
* CR:		01/25/99 AM. !!
********************************************/
template<class DELTTYPE>
void Delt<DELTTYPE>::setLeft(Delt<DELTTYPE> *x) {pLeft = x;}
template<class DELTTYPE>
void Delt<DELTTYPE>::setRight(Delt<DELTTYPE> *x) {pRight = x;}

/********************************************
* FN:		GETDATA
* CR:		10/17/98 AM.
********************************************/

template<class DELTTYPE>
DELTTYPE *Delt<DELTTYPE>::getData()
{
//return &data;			// 07/02/99 AM.
return data;				// 07/02/99 AM.
}

/********************************************
* FN:		SETDATA
* CR:		02/17/00 AM.
********************************************/

template<class DELTTYPE>
void Delt<DELTTYPE>::setData(DELTTYPE *x)
{
data = x;
}


/********************************************
* FN:		GETCOUNT
* CR:		10/17/98 AM.
********************************************/
#ifndef STABLE_
template<class DELTTYPE>
int Delt<DELTTYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class DELTTYPE>
void Delt<DELTTYPE>::prettyCount(_TCHAR *str,std::_t_ofstream *ofstr)
{
if (count_)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Delt<") << str << _T("> count=")
			<< count_ << std::ends;
	errOut(&gerrStr,false);
	}

	*gout << _T("Active Delt<") << str << _T("> count=")
			<< count_ << std::endl;

	if (ofstr)
		*ofstr << _T("Active Delt<") << str << _T("> count=")
			<< count_ << std::endl;
	}
}
#endif


/********************************************
* FN:		DELETEDELTANDDATA
* CR:		10/11/98 AM.
********************************************/
template<class DELTTYPE>
void Delt<DELTTYPE>::DeleteDeltAndData(Delt<DELTTYPE> *dd)
{
if (!dd)
	return;
if (dd->data)
	delete dd->data;
delete dd;
}

//////////////////////// END OF FILE ////////////////////////////////
#endif
