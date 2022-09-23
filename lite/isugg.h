/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ISUGG.H
* FILE:	lite\isugg.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Isugg suggested rule element class.
* NOTE:	Inherit from Ielement.  Check on element's name, key=val pairs.
*
*******************************************************************************/

#ifndef ISUGG_H_
#define ISUGG_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/dlist.h"
#include "ipair.h"
#include "ielement.h"

// Forward references
class Ipair;
class Starr;

/********************************************
* CLASS:	ISUGG
* CR:		10/24/98 AM.
* SUBJ:	Interpreted suggested-element object.
* NOTE:	Will be placed into dlist (doubly-linked lists).
********************************************/

class Isugg : public Ielement
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Isugg &);					// 11/23/98 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &output, Delt<Isugg> &);	// 11/23/98 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Isugg> &);		// 11/23/98 AM.

public:
	Isugg(								// Default constructor.
		_TCHAR *				= 0,		// Element name.
		Dlist<Ipair> *		= 0,		// List of pairs.
		long					= 0		// Line num in pass file.		// 08/08/01 AM.
		);
	~Isugg();								// Destructor.

	Isugg(Isugg &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
	void copy(const Isugg *orig);	// Linux?
	//void Isugg::copy(const Isugg *orig);

	// Assignment.	// 07/02/99 AM.
	const Isugg &operator=(const Isugg &);

	// Access
	bool getBase();
	bool getUnsealed();				// 10/09/99 AM.
	Starr *getLayers();				// 05/06/00 AM.

	// Modify
	void setBase(bool);
	void setUnsealed(bool);			// 10/09/99 AM.
	void setLayers(Starr *);		// 05/06/00 AM.


	// General functions
	virtual void intern(
		bool &info			// Not used currently.
		);
	void fillDefaults();

	// Class-wide member functions.
	static void genSugg(Isugg *sugg, std::_t_ostream &ofile);
	static bool same(Isugg *sugg1, Isugg *sugg2);					// 06/07/99 AM.

	bool gen(_TCHAR *, Gen *,int,long);									// 08/09/02 AM.

private:
	bool base_;							// If matched node will be a base or bottom
											// attribute.
	bool unsealed_;	// If node will be unsealed to traversal by pattern
							// matcher.				// 10/09/99 AM.

	Starr *layers_;	// Interned form of attrs_ list.		// 05/06/00 AM.
};



#endif
/*************************      END OF FILE      ******************************/
