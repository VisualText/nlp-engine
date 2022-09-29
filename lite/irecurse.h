/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IRECURSE.H
* FILE:	lite\irecurse.h
* CR:		11/24/98 AM.
* SUBJ:	Declares for the Irecurse class.
* NOTE:	Represents a named set of rule-action regions in a rules file.
*
*******************************************************************************/

#ifndef IRECURSE_H_
#define IRECURSE_H_
//#include <iostream.h>
#include <assert.h>

#include "seqn.h"			// For internalizing.
#include "slist.h"		// Moving to singly-linked lists.
#include "iregion.h"

// Forward references
class Gen;					// 05/08/00 AM.

/********************************************
* CLASS:	IRECURSE
* CR:		11/24/98 AM.
* SUBJ:	Internal representation of a named set of rules file regions.
********************************************/

class Irecurse
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Irecurse &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Selt<Irecurse> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Slist<Irecurse> &);

public:
	Irecurse(					// Default constructor.
		_TCHAR * = 0,
		Slist<Iregion> * = 0
		);
	~Irecurse();									// Destructor.

	// Access
	_TCHAR				*getName();
	Slist<Iregion>	*getRegions();
	Dlist<Irule>   *getRules();
	int				 getRecid();											// 05/31/00 AM.

	// Modify
	void setName(_TCHAR *);
	void setRegions(Slist<Iregion> *);
	void setRules(Dlist<Irule> *);
	void setRecid(int);

	// General.
	static Dlist<Seqn> *intern(Slist<Irecurse> *recs,
			Parse*,																// 08/23/02 AM.
			bool fhash = true);
	static Seqn *intern(Irecurse *rec,
			Parse*,																// 08/23/02 AM.
			bool fhash = true);
	static bool gen(Slist<Irecurse> *list, Gen *);
	bool gen(Gen *);															// 05/30/00 AM.


protected:
	// Name is referred to by rule elements that want to execute mini-passes
	// recursively.
	_TCHAR *name_;								// Required name for set of regions.
	Slist<Iregion> *regions_;				// Set of rule regions to execute.

	// Interned version of a recurse region will have the data below.
	Dlist<Irule> *rules_;					// Recurse owns and deletes these rules.

	int recid_;		// Number of recursive pass.						// 05/31/00 AM.

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
