/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SELT.H
* FILE:	include/API/lite/selt.h
* CR:		11/18/98 AM.
* SUBJ:	Declares for a singly-linked list.
* NOTE:	Copied and modified delt.h.
* WARN:	Has a POINTER to its data, rather than having data inside the object.
*
*******************************************************************************/

#ifndef SELT_H
#define SELT_H
#ifdef LINUX
#include <sstream>
#else
#include <strstream>															// 05/25/01 AM.
#endif


/********************************************
* CLASS:	SELT
* CR:		11/18/98 AM.
********************************************/

template<class SELTTYPE>
class Selt
{
// Can't get the right syntax here. Apparently a Visual C++ bug.
//	friend class Tree<SELTTYPE>;

public:
	Selt(SELTTYPE *, Selt * = 0);	// Constructor
	~Selt();									// Destructor
	SELTTYPE *getData();					// Selt's data.

	Selt *Right();

	void setRight(Selt<SELTTYPE> *);	// 07/16/99 AM. !!

	static void DeleteSeltAndData(Selt<SELTTYPE> *);
	static void DeleteSelt(Selt<SELTTYPE> *);	// Not the data. // 12/16/98 AM

// 11/18/98 AM. WORKAROUND. Making these data elements public till Microsoft
// enables a template class to be a friend of another class.
public:
//private:
	SELTTYPE *data;						// Data associated with node.

	Selt *pRight;							// Selt's right sibling.

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
* FN:		SELT (constructor)
* CR:		11/18/98 AM.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE>::Selt(SELTTYPE *dd, Selt *rr)
{
data = dd;
pRight = rr;
#ifndef STABLE_
++count_;
#endif
}


/********************************************
* FN:		~SELT (destructor)
* CR:		11/18/98 AM.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE>::~Selt()
{
if (data)						// 12/16/98 AM.
	delete data;				// 11/18/98 AM.

#ifndef STABLE_
--count_;
#endif
}


/********************************************
* FN:		Access functions.
********************************************/

template<class SELTTYPE>
Selt<SELTTYPE> *Selt<SELTTYPE>::Right() { return pRight; }


// 07/16/99 AM. !!
template<class SELTTYPE>
void Selt<SELTTYPE>::setRight(Selt<SELTTYPE> *x) {pRight = x;}

/********************************************
* FN:		GETDATA
* CR:		11/18/98 AM.
********************************************/

template<class SELTTYPE>
SELTTYPE *Selt<SELTTYPE>::getData()
{
return data;
}


/********************************************
* FN:		GETCOUNT
* CR:		11/18/98 AM.
********************************************/
#ifndef STABLE_
template<class SELTTYPE>
int Selt<SELTTYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/18/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class SELTTYPE>
void Selt<SELTTYPE>::prettyCount(_TCHAR *str,
	std::_t_ofstream *ofstr)
{
if (count_)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Selt<") << str << _T("> count=")
		  << count_ << std::ends;
	errOut(&gerrStr,false);
	}

	*gout << _T("Active Selt<") << str << _T("> count=")
		  << count_ << std::endl;

	if (ofstr)
		*ofstr << _T("Active Selt<") << str << _T("> count=")
		  << count_ << std::endl;
	}
}
#endif


/********************************************
* FN:		DELETESELTANDDATA
* CR:		11/18/98 AM.
********************************************/
template<class SELTTYPE>
void Selt<SELTTYPE>::DeleteSeltAndData(Selt<SELTTYPE> *ss)
{
if (!ss)
	return;
delete ss;
}


/********************************************
* FN:		DELETESELT
* SUBJ:	Delete singly-linked list elt, but not its data.
* CR:		12/18/98 AM.
********************************************/
template<class SELTTYPE>
void Selt<SELTTYPE>::DeleteSelt(Selt<SELTTYPE> *ss)
{
if (!ss)
	return;
ss->data = 0;	// Clear it so it won't be deleted.
delete ss;
}

//////////////////////// END OF FILE ////////////////////////////////
#endif
