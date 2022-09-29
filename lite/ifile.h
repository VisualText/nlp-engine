/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IFILE.H
* FILE:	lite\ifile.h
* CR:		11/24/98 AM.
* SUBJ:	Declares for the Ifile class.
* NOTE:	Internal representation of a rules file.
*
*******************************************************************************/

#ifndef IFILE_H_
#define IFILE_H_
//#include <iostream.h>
#include <assert.h>

#include "irule.h"		// For internalizing to a rules list.
//#include "lite/dlist.h"		// For internalizing to a rules list.
#include "slist.h"		// Moving to singly-linked lists.

// Forward references
class Irecurse;
class Iregion;
class Seqn;
class Iaction;				// 12/07/98 AM.
class Htab;					// 12/16/98 AM.
class Gen;					// 05/08/00 AM.
class Parse;				// 08/23/02 AM.

/********************************************
* CLASS:	IFILE
* CR:		11/24/98 AM.
* SUBJ:	Internal representation of a named set of rules file regions.
********************************************/

class Ifile
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Ifile &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Selt<Ifile> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Slist<Ifile> &);

public:
	Ifile(					// Default constructor.
		Dlist<Iarg>		 * = 0,					// 12/19/01 AM. Decls.
		Dlist<Iaction>  * = 0,					// 12/07/98 AM. Codes.
		Dlist<Iarg>		 * = 0,					// 11/30/98 AM. Selects.
		Slist<Irecurse> * = 0,
		Slist<Iregion>  * = 0,
		bool = false,								// 03/25/99 AM. Fmulti.
		bool = false,								// 10/19/99 AM. Fpath.
		bool = false								// 10/25/99 AM. Fpremulti.
		);
	~Ifile();									// Destructor.

	// Access
	Slist<Irecurse>	*getRecurses();
	Slist<Iregion> 	*getRegions();
	Dlist<Irule>		*getRules();
	Dlist<Seqn>			*getSeqs();				// 11/25/98 AM.
	Dlist<Iarg>			*getSelects();			// 11/30/98 AM.
	Dlist<Iarg>			*getDecls();			// 12/19/01 AM.
	Dlist<Iaction>		*getCodes();			// 12/07/98 AM.
	tHtab<Slist<Irule> > *getHtab();			// 12/16/98 AM.
	Slist<Irule>		*getMusts();			// 12/16/98 AM.
	bool					 getFmulti();			// 03/25/99 AM.
	bool					 getFpath();			// 10/19/99 AM.
	bool					 getFpremulti();		// 10/25/99 AM.
	bool					 getEmpty();			// 08/31/00 AM.
	bool					 getOwning();			// 12/24/00 AM.

	// Modify
	void setRecurses(Slist<Irecurse> *);
	void setRegions(Slist<Iregion> *);
	void setRules(Dlist<Irule> *);
	void setSeqs(Dlist<Seqn> *);				// 11/25/98 AM.
	void setSelects(Dlist<Iarg> *);			// 11/30/98 AM.
	void setDecls(Dlist<Iarg>*);				// 12/19/01 AM.
	void setCodes(Dlist<Iaction> *);			// 12/07/98 AM.
	void setHtab(tHtab<Slist<Irule> > *);	// 12/16/98 AM.
	void setMusts(Slist<Irule> *);			// 12/16/98 AM.
	void setFmulti(bool);						// 03/25/99 AM.
	void setFpath(bool);							// 10/19/99 AM.
	void setFpremulti(bool);					// 10/25/99 AM.
	void setEmpty(bool);							// 08/31/00 AM.
	void setOwning(bool);						// 12/24/00 AM.

	// General.
	Dlist<Irule> *intern(Parse*,bool fhash = true);
	void rhash(							// 12/16/98 AM.
					Parse*);														// 04/03/03 AM.

	bool gen(Parse *);														// 04/19/00 AM.
	bool genExecute(Parse *);												// 06/20/00 AM.
	bool genHash(Parse *);													// 06/13/00 AM.
	bool genRhash(Gen *);													// 06/16/00 AM.
	//bool genSelect(char *fun, Gen *gen);								// 04/27/00 AM.
	//bool genNodes(char *fun, Gen *gen);								// 04/28/00 AM.
	//bool genRules(char *fun, Gen *gen);								// 04/28/00 AM.

protected:
	// Will have global header information for a rules file here.

	Dlist<Iarg>		 *decls_;		// Declarations.					// 12/19/01 AM.
	Dlist<Iaction>	 *codes_;		// Code actions.					// 12/07/98 AM.
	Dlist<Iarg>		 *selects_;		// Select nodes.					// 11/30/98 AM.
	Slist<Irecurse> *recurses_;			// Named recursively-called mini-passes.
	Slist<Iregion>  *regions_;				// Normal rules regions of a rules file.

	// Internalized information below.
	// Ifile will own and delete its ruleset.
	Dlist<Irule> *rules_;													// 11/24/98 AM.

	// Ifile will own and delete the sequence list built for the RECURSE
	// regions.
	Dlist<Seqn>  *seqs_;						// 11/25/98 AM. (Pronounced "sex" ;-)

	// Hash table for speedy lookup of rules to be matched.
	// Attached list of rules that must always be tried.
	tHtab<Slist<Irule> > *htab_;				// 12/16/98 AM.
	Slist<Irule> *musts_;					// Rules that must always be tried.

	// 03/25/99 AM. May want to set up a flags variable in here.
	bool fmulti_;		// If SELECT region specified MULTI.		// 03/25/99 AM.
	bool fpath_;		// If SELECT region specified PATH.			// 10/19/99 AM.
	bool fpremulti_;	// If SELECT region specified PREMULTI.	// 10/25/99 AM.

	// When rulefile is likely to be legitimately empty of rules, set this.
	bool empty_;																// 08/31/00 AM.

	// Flag whether rules own their actions.  If RUG, then yes.	// 12/24/00 AM.
	bool owning_;																// 12/24/00 AM.

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
