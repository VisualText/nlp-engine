/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPRE.H
* FILE:	lite\ipre.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Ipre pre-action class.
* NOTE:	Ipre (pre-actions) and Ipost post-actions) inherit from Iaction.
*			This class checks for valid pre-actions, but mainly reuses Iaction.
*
*******************************************************************************/

#ifndef IPRE_H_
#define IPRE_H_
//#include <iostream.h>
#include <assert.h>

#include "iaction.h"

// Forward references

/********************************************
* CLASS:	IPRE
* CR:		10/24/98 AM.
* SUBJ:	Interpreted pre-action object.
********************************************/

class Ipre : public Iaction
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Ipre &pre);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Ipre> &);

public:
	Ipre(
		_TCHAR *				= 0,				// Action name.
		Dlist<Iarg> *		= 0,				// Action args.
		int					= 0,				// Start elt covered.
		int					= 0				// End elt covered.
		);								// Default constructor.
	~Ipre();							// Destructor.

	Ipre(Ipre &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
	void copy(const Ipre *orig);

	const Ipre &operator=(const Ipre &);	// Assignment.

	// Access
	int getStart();
	int getEnd();

	// Modify
	void setStart(int);
	void setEnd(int);

	// Class-wide member functions.
	static bool same(Dlist<Ipre> *list1, Dlist<Ipre> *list2); //06/07/99 AM.
	static bool same(Ipre *pre1, Ipre *pre2);			// 06/07/99 AM.

	static void genPres(Dlist<Ipre> *pres, std::_t_ostream &ofile);// 07/06/99AM.
	static void genPre(Ipre *pre, std::_t_ostream &ofile);			 // 07/06/99 AM.

private:
	int start_;				// First rule elt covered by action.	// 11/29/98 AM.
	int end_;				// Last rule elt covered by action.		// 11/29/98 AM.
};



#endif
/*************************      END OF FILE      ******************************/
