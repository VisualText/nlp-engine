/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IREGION.H
* FILE:	lite\iregion.h
* CR:		11/24/98 AM.
* SUBJ:	Declares for the Iregion class.
* NOTE:	Represents a rule-action region in a rules file.
*
*******************************************************************************/

#ifndef IREGION_H_
#define IREGION_H_
//#include <iostream.h>
#include <assert.h>

#include "seqn.h"			// For interning.
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "slist.h"		// Moving to singly-linked lists of regions.

// Forward references
class Iaction;
class Ipre;
class Irule;
class Parse;																// 08/23/02 AM.

/********************************************
* CLASS:	IREGION
* CR:		11/24/98 AM.
* SUBJ:	Internal representation of a rules file region.
********************************************/

class Iregion
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iregion &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Selt<Iregion> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Slist<Iregion> &);

public:
	Iregion(Dlist<Ipre> *	 = 0,			// 11/29/98 AM.
			  Dlist<Iaction> * = 0,			// 11/27/98 AM.
			  Dlist<Iaction> * = 0,
			  Dlist<Irule>   * = 0);		// Default constructor.
	~Iregion();									// Destructor.

	// Access
	Dlist<Ipre>		*getPres();
	Dlist<Iaction> *getChecks();				// 11/27/98 AM.
	Dlist<Iaction> *getPosts();
	Dlist<Irule>   *getRules();

	// Modify
	void setPres(Dlist<Ipre> *);
	void setChecks(Dlist<Iaction> *);			// 11/27/98 AM.
	void setPosts(Dlist<Iaction> *);
	void setRules(Dlist<Irule> *);

	// General.
	static Dlist<Irule> *intern(Slist<Iregion> *, Dlist<Seqn> *, Parse*,
		bool fhash = true);
	static Dlist<Irule> *intern(Iregion *, Dlist<Seqn> *, Parse*,
		bool fhash = true);
	static bool gen(Slist<Iregion> *list, Gen *gen);

protected:
	Dlist<Ipre> *pres_;						// Pre-actions.
	Dlist<Iaction> *checks_;				// Check_actions.		// 11/27/98 AM.
	Dlist<Iaction> *posts_;					// Post actions.
	Dlist<Irule>   *rules_;					// Rules using the above actions.

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
