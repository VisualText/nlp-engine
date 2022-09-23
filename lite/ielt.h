/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IELT.H
* FILE:	lite\ielt.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Ielt phrase-element class.
* NOTE:	Inherit from Ielement.  Check on element's name, key=val pairs.
*
*******************************************************************************/

#ifndef IELT_H_
#define IELT_H_
//#include <iostream.h>
#include <assert.h>

#include "inline.h"															// 09/26/01 AM.
//#include "lite/dlist.h"
#include "ipair.h"
#include "ielement.h"
#include "thtab.h"		// 12/17/98 AM.

// Best numeric rating possible for a trigger element.	// 10/07/99 AM.
#define BESTTRIG 6

// Forward references
class Gen;																		// 05/08/00 AM.
class Ipair;
class Irule;
class Seqn;
class Iarg;
class Ipre;
class Htab;
class Starr;																	// 05/06/00 AM.
class Pat;																		// 05/16/00 AM.
class Parse;																	// 08/23/02 AM.

/********************************************
* CLASS:	IELT
* CR:		10/24/98 AM.
* SUBJ:	Interpreted phrase-element object.
* NOTE:	Will be placed into dlist (doubly-linked lists).
********************************************/

class Ielt : public Ielement
{
	friend class Pat;						// OPT									// 05/16/00 AM.

	friend std::_t_ostream &operator<<(std::_t_ostream &, Ielt &);
	friend std::_t_ostream &operator<<(std::_t_ostream &output, Delt<Ielt> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Ielt> &);

public:
	Ielt(									// Default constructor.
		_TCHAR *				= 0,		// Element name.
		Dlist<Ipair> *		= 0,		// List of pairs.
		long					= 0		// Line num in pass file.		// 08/08/01 AM.
		);
	~Ielt();								// Destructor.

	Ielt(Ielt &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
#ifdef LINUX
	void copy(const Ielt *orig);
#else
	void Ielt::copy(const Ielt *orig);
#endif

	// Assignment.	// 07/02/99 AM.
	const Ielt &operator=(const Ielt &);

	// Access
	int getMin();
	int getMax();
	_TCHAR *getRename();
	_TCHAR *getGroup();						// 09/23/00 AM.
	bool getSinglet();
	bool getTrigger();
	bool getTree();						// 03/17/99 AM.
	bool getXsinglet();					// 03/24/99 AM.
	bool getXtree();						// 03/24/99 AM.
	bool getLookahead();					// 11/19/99 AM.
	bool getClosed();						// RUG	// 07/14/99 AM.
	bool getDeaccent();					// 01/28/05 AM.
	bool getDejunk();                // 09/09/11 AM.
	//Dlist<Irule> *getRuleset();
	Dlist<Seqn> *getPasses();
	Dlist<Iarg> *getMatches();			// 11/09/98 AM.
	Dlist<Iarg> *getFails();			// 11/09/98 AM.
	Dlist<Iarg> *getExcepts();			// 10/13/99 AM.
	int getFlags();						// 11/23/98 AM.
	Dlist<Ipre> *getPres();				// 11/29/98 AM.
	Htab *getHmatches();					// 12/12/98 AM.
	Htab *getHfails();					// 12/12/98 AM.
	Htab *getHexcepts();					// 10/13/99 AM.
	Starr *getLayers();					// 05/06/00 AM.

	// Modify
	void setMin(int);
	void setMax(int);
	void setRename(_TCHAR *);
	void setGroup(_TCHAR *);				// 09/23/00 AM.
	void setSinglet(bool);
	void setTrigger(bool);
	void setTree(bool);					// 03/17/99 AM.
	void setXsinglet(bool);				// 03/24/99 AM.
	void setXtree(bool);					// 03/24/99 AM.
	void setLookahead(bool);			// 11/19/99 AM.
	void setClosed(bool);				// RUG	// 07/14/99 AM.
	void setDeaccent(bool);				// 01/28/05 AM.
	void setDejunk(bool);				// 09/09/11 AM.
	//void setRuleset(Dlist<Irule> *);
	void setPasses(Dlist<Seqn> *);
	void setMatches(Dlist<Iarg> *);	// 11/09/98 AM.
	void setFails(Dlist<Iarg> *);		// 11/09/98 AM.
	void setExcepts(Dlist<Iarg> *);	// 10/13/99 AM.
	void setFlags(int);					// 11/23/98 AM.
	void setPres(Dlist<Ipre> *);		// 11/29/98 AM.
	void setHmatches(Htab *);			// 12/12/98 AM.
	void setHfails(Htab *);				// 12/12/98 AM.
	void setHexcepts(Htab *);			// 10/13/99 AM.
	void setLayers(Starr *);			// 05/06/00 AM.

	// General functions.
	virtual void intern(Dlist<Seqn> *, Dlist<Ipre> *, int,
							Parse*,												// 08/23/02 AM.
							/*UP*/ bool &info, bool &f_look,
							bool fhash = true);
	void internPres(						// 07/13/99 AM.
		Dlist<Ipre> *pres,		// For binding pre actions.
		int ord						// Current element number in rule's phrase.
		);
	void internRecurse(Dlist<Iarg> *, Dlist<Seqn> *);
	void fillDefaults();
	//bool wild();
	bool nameMatches(_TCHAR *str);		// 07/05/99 AM.
	_TCHAR *litName();						// 07/05/99 AM.
	bool getMinmax(				// 10/06/99 AM.
		/*UP*/
		int &min,
		int &max
		);

	// Class-wide member functions.
	static Dlist<Ielt> *makeDlist(
		_TCHAR *			= 0,			// Name.
		Dlist<Ipair> * = 0,			// List of pairs.
		int				= 1,			// Default min.
		int			   = 1,			// Default max.
		_TCHAR *			= 0			// Rename.
		);
	static Delt<Ielt> *makeDelt(
		_TCHAR *			= 0,			// Name.
		Dlist<Ipair> *	= 0,			// List of pairs.
		int				= 1,			// Default min.
		int			   = 1,			// Default max.
		_TCHAR *			= 0			// Rename.
		);
	static Delt<Ielt> *addDelt(
		Dlist<Ielt> *,					// List to add element to.
		_TCHAR *			= 0,			// Name.
		Dlist<Ipair> *	= 0,			// List of pairs.
		int				= 1,			// Default min.
		int			   = 1,			// Default max.
		_TCHAR *			= 0			// Rename.
		);

	static bool getDeaccent(int flags);                      // 03/28/05 AM.
	static bool getDejunk(int flags);                        // 09/09/11 AM.


	// Build hash tables as needed.
	void internList(												// 12/12/98 AM.
		/*DU*/
		Dlist<Iarg>* &list,
		Htab* &htab
		);

	int trigRating(												// 10/07/99 AM.
		/*UP*/
		bool &bestrating
		);

	static bool phraseActions(Dlist<Ielt> *phrase);				// 05/08/00 AM.

	static bool rhash(Ielt *,											// 12/16/98 AM.
		tHtab<Slist<Irule> > *, Irule *,
		Parse*,																// 04/03/03 AM.
		bool &);
	static bool rhash(Dlist<Ielt> *,									// 12/16/98 AM.
		tHtab<Slist<Irule> > *, Irule *, Parse*);					// 04/03/03 AM.
	static bool hashable(Dlist<Ielt> *,								// 12/16/98 AM.
								Parse*);										// 04/03/03 AM.
	static bool hashable(Ielt *,										// 12/17/98 AM.
								Parse*,										// 04/03/03 AM.
								bool &);
	static bool hashables(Dlist<Iarg> *dlist);					// 12/17/98 AM.
	static bool rhashes(Dlist<Iarg> *,
		tHtab<Slist<Irule> > *, Irule *);							// 12/17/98 AM.
	static bool hashRule(_TCHAR *,
		tHtab<Slist<Irule> > *, Irule *);							// 12/17/98 AM.

	static void pruneLists(Ielt *);									// 12/17/98 AM.
	static Dlist<Iarg> *pruneList(Dlist<Iarg> *vals);			// 12/17/98 AM.

	void ehash();																// 05/02/00 AM.

	// Code generation.
	static bool genFirst(Delt<Ielt> *, Gen *);						// 05/02/00 AM.
	static bool genTrigger(Delt<Ielt> *, Gen *);						// 05/02/00 AM.
	static bool genLeft(Delt<Ielt> *, Gen *);							// 05/02/00 AM.
	static bool genRight(Delt<Ielt> *, Gen *);						// 05/02/00 AM.
	static bool genDirection(												// 05/30/00 AM.
		Delt<Ielt> *,
		bool,					// If going left of trigger.
		Gen *
		);
	static bool genMatchelt(Delt<Ielt> *,_TCHAR *,_TCHAR *,Gen *);	// 05/04/00 AM.
	static bool genSpecialelt(												// 05/25/00 AM.
		Delt<Ielt> *delt,
		_TCHAR *fnname,
		_TCHAR *matchfn,
		_TCHAR *microfn,
		Gen *gen
		);
	static bool genEltarrays(Dlist<Ielt> *, Gen *);					// 05/22/00 AM.
	_TCHAR *matchFn();															// 05/23/00 AM.
	_TCHAR *matchSpecialfn();													// 05/25/00 AM.
	static void lookList(													// 06/09/00 AM.
		Dlist<Iarg> *,
//		bool &reg,																// 04/29/01 AM.
//		bool &spec,																// 04/29/01 AM.
		bool &hashed,
		long &rlen,			// # of regular toks.						// 04/29/01 AM.
		long &slen			// # of special toks.						// 04/29/01 AM.
		);

	// Dump or prettyprint, not code generation.
	static void genPhrase(													// 05/30/99 AM.
		Dlist<Ielt> *phr,
		_TCHAR *sep,																// 11/04/99 AM.
		std::_t_ostream &ofile,
		bool trunc
		);
	static void genElt(														// 05/30/99 AM.
		Ielt *elt,
		_TCHAR *sep,																// 11/04/99 AM.
		std::_t_ostream &ofile,
		bool trunc
		);

	static bool genPhraserecurses(										// 11/04/99 AM.
		Dlist<Ielt> *phrase,
		_TCHAR *sep,			// Element separator.
		std::_t_ostream &ofile
		);
	bool genEltrecurses(												// 11/04/99 AM.
		_TCHAR *sep,			// Element separator.
		std::_t_ostream &ofile
		);
	static void genEltlist(Dlist<Iarg> *,_TCHAR *,Gen *);			// 05/04/00 AM.
	static void genEltspecial(												// 06/09/00 AM.
		Dlist<Iarg> *list,
		_TCHAR *buf,
		Gen *gen);
	static void genEltstarr(Starr *, _TCHAR *, Gen *);				// 05/06/00 AM.
	static void genElts(Dlist<Ielt> *, _TCHAR *, Gen *);				// 05/17/00 AM.

	static bool genHash(Dlist<Ielt> *, Gen *);						// 06/14/00 AM.
	bool genHash(Gen *);														// 06/14/00 AM.

	static bool same(Dlist<Ielt> *phr1, Dlist<Ielt> *phr2);		// 06/07/99 AM.
	static bool same(Ielt *elt1, Ielt *elt2);							// 06/07/99 AM.

	// Find best candidate to be the trigger element.
	static Delt<Ielt> *trigPhrase(Dlist<Ielt> *);					// 10/07/99 AM.
	static int trigRatename(										// 10/07/99 AM.
		_TCHAR *name
		);

	/********************************************
	* FN:		WILD
	* CR:		12/17/98 AM.
	* SUBJ:	Check if given rule elt is a wildcard.
	* NOTE:	May be other ways in the future, like
	*				_xFEAT [mode=wild]
	*			11/09/98 AM. Treating wildcards with MATCH and FAIL lists
	*			as NON wildcards.
	*			12/17/98 AM. Took this from Pat::wildElt.
	********************************************/

#ifdef LINUX
	inline bool wild()
#else
	inline bool Ielt::wild()
#endif
	{
	_TCHAR *name;

	return ((name = this->getName())
		  && (!strcmp_i(name, _T("_xWILD")))
		  && (!this->getMatches())				// 11/09/98 AM.
		  && (!this->getFails())
		  && (!this->getHmatches())			// 11/09/99 AM. 1 yr later!
		  && (!this->getHfails())				// 11/09/99 AM.
		  // EVEN PRE CONDITIONS REMOVE THE WILDCARD ASPECT	// 08/16/11 AM.
		  && !this->pres_	// FIX	// 08/16/11 AM.
		  );
	}

private:
	// Min-max specify optional, repetition, and wildcard ranges.
	int min_;								// Minimum nodes to match.
											// -1 = undefined. 0 = optional.>0 lower bound.
	int max_;								// Maximum nodes to match.
											// -1 = undefined 0 = infinity. >0 upper bound.
	_TCHAR *rename_;						// For renaming nodes that match this elt.

	// For wildcard or repeating elements, a way to group at the element
	// level. An alternate to group(n,n,label) post-action.		// 09/23/00 AM.
	_TCHAR *group_;																// 09/23/00 AM.

	// Though Irule has a trigger_ variable, Ielt should probably have
	// trigger also, at least as a bit flag.

	// Hard-wired attributes associated with rule elements.  11/3/98
	// Opt: These will be more efficient than traversing the pairs list.
	//bool singlet_;						// Search down singlet subtree, ie,
											// down till a base node (if any).
											// 11/23/98 AM. Implementing as bit-flags.

	// 11/08/98 AM. ruleset_, if any, is a way to recursively invoke a set
	// of rules on the nodes matched by the current rule elt.  Allows fixing
	// up the matched nodes.
	//Dlist<Irule> *ruleset_;			// Rules to invoke recursively.

	// 11/09/98 AM. Replacing ruleset_ with a list of rulesets.  Actually,
	// putting in the hooks for a set of passes here.
	Dlist<Seqn> *passes_;

	// 11/09/98 AM. Implementing restriction (MATCH and FAIL) lists on
	// wildcards.
	Dlist<Iarg> *matches_;
	Dlist<Iarg> *fails_;
	Dlist<Iarg> *excepts_;					// 10/13/99 AM.

	// 12/12/98 AM. Hash tables for long restriction lists.
	Htab *hmatches_;
	Htab *hfails_;
	Htab *hexcepts_;							// 10/13/99 AM.

	// 11/23/98 AM.	Implementing bit-flags.
	int flags_;
	// singlet and trigger will be flags for a rule elt.

	// Rule element gets its own copy of PRE actions to execute.	// 11/29/98 AM.
	Dlist<Ipre> *pres_;

	Starr *layers_;	// Interned form of attrs_ list.				// 05/06/00 AM.
};



#endif
/*************************      END OF FILE      ******************************/
