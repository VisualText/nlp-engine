/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPOST.H
* FILE:	lite\ipre.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Ipost pre-action class.
* NOTE:	Ipre (pre-actions) and Ipost post-actions) inherit from Iaction.
*			This class checks for valid post-actions, but mainly reuses Iaction.
*
*******************************************************************************/

#ifndef IPOST_H_
#define IPOST_H_
//#include <iostream.h>
#include <assert.h>

#include "iaction.h"

// Forward references

/********************************************
* CLASS:	IPOST
* CR:		10/24/98 AM.
* SUBJ:	Interpreted post-action object.
********************************************/

class Ipost : public Iaction
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Ipost> &);

public:
	Ipost(
		_TCHAR *				= 0,				// Action name.
		Dlist<Iarg> *		= 0				// List of actions.
		);								// Default constructor.
	~Ipost();							// Destructor.

	Ipost(Ipost &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
#ifdef LINUX
	void copy(const Ipost *orig);
#else
	void Ipost::copy(const Ipost *orig);
#endif

	const Ipost &operator=(const Ipost &);	// Assignment.


	// Access

	// Modify

	// Class-wide member functions.
	static Dlist<Ipost> *makeDlist(
		_TCHAR *,							// Action name.
		Dlist<Iarg> *					// List of arguments.
		);
	static Delt<Ipost> *makeDelt(
		_TCHAR *,							// Action name.
		Dlist<Iarg> *					// List of arguments.
		);
	static Delt<Ipost> *addDelt(
		Dlist<Ipost> *,				// List to add element to.
		_TCHAR *,							// Action name.
		Dlist<Iarg> *					// List of arguments.
		);
	static bool unpackGroup(											// 12/28/99 AM.
		Ipost *act,
		/*UP*/
		int &first,
		int &last,
		_TCHAR* &label
		);

private:

};



#endif
/*************************      END OF FILE      ******************************/
