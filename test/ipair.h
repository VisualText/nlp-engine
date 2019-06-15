/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	IPAIR.H
* FILE:	lite\ipair.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Ipair class for rule element's key=val pair.
* NOTE:	This class is for the interpreted analyzer.
*
*******************************************************************************/

#ifndef IPAIR_H_
#define IPAIR_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/dlist.h"
#include "iarg.h"

// Forward references.
class Iarg;		// Argument class.

/********************************************
* CLASS:	IPAIR
* CR:		10/24/98 AM.
* SUBJ:	Key-value pair.
* NOTE:	Will be placed into dlist (doubly-linked lists).
********************************************/

class Ipair
{
	friend _t_ostream &operator<<(_t_ostream &, Ipair &);
	friend _t_ostream &operator<<(_t_ostream &, Delt<Ipair> &);		// 11/22/98 AM.
	friend _t_ostream &operator<<(_t_ostream &, Dlist<Ipair> &);	// 11/22/98 AM.

public:
	Ipair(								// Default constructor.
		_TCHAR *			= 0,			// Pair's key name.
		Dlist<Iarg> *	= 0			// Pair's values.		// 11/07/98 AM.
	);
	~Ipair();							// Destructor.

	Ipair(Ipair &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
	void copy(const Ipair *orig);

	// Assignment.	// 07/02/99 AM.
	const Ipair &operator=(const Ipair &);

	// Access
	_TCHAR *getKey();
	Dlist<Iarg> *getVals();			// 11/07/98 AM.

	// Modify
	void setKey(_TCHAR *);
	void setVals(Dlist<Iarg> *);	// 11/07/98 AM.

	// Class-wide member functions.
	static Dlist<Ipair> *makeDlist(
		_TCHAR *,							// Key name.
		Dlist<Iarg> *					// Values		11/07/98 AM.
		);
	static Delt<Ipair> *makeDelt(
		_TCHAR *,							// Key name.
		Dlist<Iarg> *					// Values		11/07/98 AM.
		);
	static Delt<Ipair> *addDelt(
		Dlist<Ipair> *,				// List to add element to.
		_TCHAR *,							// Key name.
		Dlist<Iarg> *					// Values		11/07/98 AM.
		);


private:
	_TCHAR *key;							// Key name.
	Dlist<Iarg> *vals_;				// Values.  11/07/98 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
