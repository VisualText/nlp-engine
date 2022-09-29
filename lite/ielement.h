/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IELEMENT.H
* FILE:	lite\ielement.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Ielement rule-element class.
* NOTE:	Isugg (suggested concept) and Ielt (rule-phrase's element) inherit.
* EXAMPLE:	In the rules file, elements will look like
*						NAME [key1=val1 key2=val2 ...]
*				eg,	_noun [flags="*<"   rename=head   max=3]
*				or just _noun
*				key=val will is an Ipair and will reuse Iarg for val.
* REF:	Need to document the element language as we go.
*
*******************************************************************************/

#ifndef IELEMENT_H_
#define IELEMENT_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/dlist.h"

// Forward references
class Ipair;
class Iarg;

/********************************************
* CLASS:	IELEMENT
* CR:		10/24/98 AM.
* SUBJ:	Interpreted rule-element object.
* NOTE:	Will be placed into dlist (doubly-linked lists).
********************************************/

class Ielement
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Ielement &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Ielement> &);

public:
	Ielement(									// Default constructor.
		_TCHAR *				= 0,				// Element name.
		Dlist<Ipair> *		= 0,				// List of pairs.
		Dlist<Iarg> *   = 0,					// List of attrs.
		long = 0									// Line in pass file.	// 08/08/01 AM.
		);
	virtual ~Ielement();						// Destructor.
													// VIRTUAL 02/07/00 AM.

	Ielement(Ielement &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
#ifdef LINUX
	void copy(const Ielement *orig);
#else
	void Ielement::copy(const Ielement *orig);
#endif

	// Assignment.	// 07/02/99 AM.
	const Ielement &operator=(const Ielement &);

	// Access
	_TCHAR *getName();
	Dlist<Ipair> *getPairs();
	Dlist<Iarg> *getAttrs();
	_TCHAR *lastAttr();							// 12/27/99 AM.
	long getLine();							// 08/08/01 AM.
	// Todo: Probably need a getVal(Key) type of call.

	// Modify
	void setName(_TCHAR *);
	void setPairs(Dlist<Ipair> *);
	void setAttrs(Dlist<Iarg> *);
	void setLine(long);						// 08/08/01 AM.

	// General
	virtual void intern(bool &info);			// 11/22/98 AM.

protected:
	_TCHAR *name_;						// The element name or "concept".
	Dlist<Ipair> *pairs_;			// The element's list of match conditions.
	Dlist<Iarg> *attrs_;				// Multiple reduce or layering names.
											// (Reuse Iarg with IASTR type.)

	long line_;		// Track line number in pass file.				// 08/08/01 AM.

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
