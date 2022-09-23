/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IRULE.H
* FILE:	lite\irule.h
* CR:		10/23/98 AM.
* SUBJ:	Declares for the Irule analyzer rule class.
* NOTE:	This class is for the interpreted analyzer.
*
*******************************************************************************/

#ifndef IRULE_H_
#define IRULE_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/dlist.h"
#include "slist.h"			// 12/18/98 AM.


//#include "dlist.h"
//#include "algo.h"

#include "ipre.h"
#include "ipost.h"
#include "ielt.h"
#include "isugg.h"

// Forward references.
class Gen;									// 05/8/00 AM.
class Ielt;		// Phrase elements.
class Isugg;	// Suggested concept.
class Iaction;	// Actions.
class Ipre;
class Htab;		// Hash table.			// 12/16/98 AM.
class Parse;								// 08/23/02 AM.

/********************************************
* CLASS:	IRULE
* CR:		10/23/98 AM.
* SUBJ:	Interpreted rule object.
* NOTE:	Will be placed into dlist (doubly-linked lists), for now.
********************************************/

class Irule
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Irule &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Delt<Irule> &);	// 10/26/98 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Irule> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Selt<Irule> &);		// 12/19/98 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Slist<Irule> &);	// 12/19/98 AM.

public:
	Irule(									// Default constructor.
		Dlist<Ielt> *		= 0,			// Phrase of elements.
		Isugg *				= 0,			// Suggested concept.
		Dlist<Ipre> *		= 0,			// Pre-actions.
		Dlist<Iaction> *	= 0,			// Check-actions.				// 11/27/98 AM.
		Dlist<Iaction> *	= 0,			// Post-actions.
		Delt<Ielt> *		= 0,			// Trigger.
		long					= 0			// Line num in pass file.	// 05/26/01 AM.
		);
	~Irule();								// Destructor.

	// Copy constructor.
	// ASSUMES ACTIONS ARE NOT OWNED BY RULE.
	Irule(		// 07/01/99 AM.
		Irule &orig
		);

	// ASSUMES ACTIONS OWNED BY NEW RULE.
	Irule *copy_owning();	// 07/03/99 AM.

	const Irule &operator=(const Irule &);	// Assignment.	// 07/01/99 AM.

	void clear(bool owning = false);		// Empty out rule. // 07/01/99 AM.
	void zero();									// Set fields to zero. // 07/01/99 AM.
	void copy(		// 07/02/99 AM.
		const Irule *orig,		// The source.
		bool owning			// If destination rule will own its actions.
		);


	// Access
	Dlist<Ielt> *getPhrase() const;
	Isugg *getSugg();
	Dlist<Ipre> *getPre();
	Dlist<Iaction> *getChecks();				// 11/27/98 AM.
	Dlist<Iaction> *getPost();
	Delt<Ielt> *getTrigger() const;
	int getLookahead();							// 11/22/99 AM.
	long getNum();									// 12/18/98 AM.
	_TCHAR *getSample();							// 06/16/99 AM.
	Slist<Irule> *getDown();					// 06/28/99 AM.
	int getNlabels();								// 07/16/99 AM.
	long getLine();								// 05/26/01 AM.

	// Modify
	void setPhrase(Dlist<Ielt> *);
	void setSugg(Isugg *);
	void setPre(Dlist<Ipre> *);
	void setChecks(Dlist<Iaction> *);		// 11/27/98 AM.
	void setPost(Dlist<Iaction> *);
	void setTrigger(Delt<Ielt> *);
	void setLookahead(int);						// 11/22/99 AM.
	void setNum(long);							// 12/18/98 AM.
	void setSample(_TCHAR *);						// 06/16/99 AM.
	void setDown(Slist<Irule> *);				// 06/28/99 AM.
	void setNlabels(int);						// 07/16/99 AM.
	void setLine(long);							// 05/26/01 AM.

	// General
	bool rhash(tHtab<Slist<Irule> > *htab,					// 12/16/98 AM.
								Parse*);											// 04/03/03 AM.
	bool unpack(														// 10/06/99 AM.
		/*UP*/
		Dlist<Ielt>* &phr,
		Isugg* &sgg,
		Delt<Ielt>* &trig
		);

	bool hasWild();															// 05/17/00 AM.

	// Class-wide member functions.
	static Dlist<Irule> *makeDlist();	// Make empty list.
	static Dlist<Irule> *makeDlist(
		Dlist<Ielt> *,					// Phrase of elements.
		Isugg *,							// Suggested concept.
		Dlist<Ipre> *		= 0,		// Pre-actions.
		Dlist<Iaction> *	= 0,		// Check-actions.					// 11/27/98 AM.
		Dlist<Iaction> *	= 0,		// Post-actions.
		Delt<Ielt> *		= 0,		// Trigger.
		long					= 0		// Line number.					// 05/26/01 AM.
		);
	static Delt<Irule> *makeDelt(
		Dlist<Ielt> *,					// Phrase of elements.
		Isugg *,							// Suggested concept.
		Dlist<Ipre> *		= 0,		// Pre-actions.
		Dlist<Iaction> *	= 0,		// Check-actions.					// 11/27/98 AM.
		Dlist<Iaction> *	= 0,		// Post-actions.
		Delt<Ielt> *		= 0,		// Trigger.
		long					= 0		// Line number.					// 05/26/01 AM.
		);
	static Delt<Irule> *addDelt(
		Dlist<Irule> *,				// List to add element to.
		Dlist<Ielt> *,					// Phrase of elements.
		Isugg *,							// Suggested concept.
		Dlist<Ipre> *		= 0,		// Pre-actions.
		Dlist<Iaction> *	= 0,		// Check-actions.					// 11/27/98 AM.
		Dlist<Iaction> *	= 0,		// Post-actions.
		Delt<Ielt> *		= 0,		// Trigger.
		long					= 0		// Line number						// 05/26/01 AM.
		);

	// Copy a list of rules that owns its actions.					// 10/29/99 AM.
	static Dlist<Irule> *copy_owning_list(Dlist<Irule> *);

	static void deleteOwning(Dlist<Irule> *);							// 11/24/98 AM.
	static void deleteOwning(Irule *rule);					// 07/03/99 AM.
	static Dlist<Irule> *intern(
		Dlist<Irule> *rules,
		Dlist<Ipre> *pres,
		Dlist<Iaction> *checks,												// 11/27/98 AM.
		Dlist<Iaction> *posts,
		Dlist<Seqn> *seqs,
		Parse*,																	// 08/23/02 AM.
		bool fhash = true														// 05/02/00 AM.
		);
	static Irule *intern(
		Irule *rule,
		Dlist<Ipre> *pres,
		Dlist<Iaction> *checks,												// 11/27/98 AM.
		Dlist<Iaction> *posts,
		Dlist<Seqn> *seqs,
		Parse*,																	// 08/23/02 AM.
		bool fhash = true														// 05/02/00 AM.
		);
	static void internPres(													// 07/08/99 AM.
		Dlist<Ipre> *pres,
		Dlist<Ielt> *phrase
		);

	static void pruneLists(Dlist<Irule> *rules);						// 12/17/98 AM.
	static void pruneLists(Irule *rule);								// 12/17/98 AM.

	static void ehash(Dlist<Irule> *rules);							// 05/02/00 AM.
	void ehash();																// 05/02/00 AM.

	static void mergeRules(Slist<Irule>* &, Slist<Irule> *);		// 12/18/98 AM
	static void renumber(Dlist<Irule> *rules);						// 05/04/00 AM.

	static bool genRuleblocks(									// 11/04/99 AM.
		Dlist<Irule> *rules,
		_TCHAR *sep,			// Element separator.
		std::_t_ostream &ofile
		);
	static bool genRulerecurses(									// 11/04/99 AM.
		Dlist<Irule> *rules,
		_TCHAR *sep,			// Element separator.
		std::_t_ostream &ofile
		);

	static void genRules(													// 05/30/99 AM.
		Dlist<Irule> *rules,
		_TCHAR *sep,																// 11/04/99 AM.
		std::_t_ostream &ofile
		);
	virtual void genRule(													// 06/22/99 AM.
		_TCHAR *sep,																// 11/04/99 AM.
		std::_t_ostream &ofile,
		bool trunc=false														// 06/05/00 AM.
		);

	static bool same(Dlist<Irule> *list1, Dlist<Irule> *list2);	// 06/07/99 AM.
	static bool same(Irule *rule1, Irule *rule2);					// 06/07/99 AM.
	static Dlist<Irule> *dedup(Dlist<Irule> *list);					// 06/07/99 AM.
	static void trigRules(Dlist<Irule> *list);						// 10/07/99 AM.
	void trigRule();															// 10/07/99 AM.

	static void quickSem(Dlist<Irule> *);								// 12/27/99 AM.
	void quickSem();															// 12/27/99 AM.
	inline Iaction *actionQuicksem(										// 12/28/99 AM.
		_TCHAR *label,		// Name of labeled rule element.
		int ord				// Position of labeled rule element.
		);
	static void rugCounter(Dlist<Irule> *);							// 11/27/00 AM.
	void rugCounter();														// 11/27/00 AM.
	inline Iaction *actionRugcounter(									// 11/27/00 AM.
		_TCHAR *var_str
		);
	bool gen(Gen *);															// 05/01/00 AM.
	static bool genCodes(Dlist<Iaction> *, Gen *);					// 06/06/00 AM.
	static bool genChecks(Dlist<Iaction> *, Gen *);					// 05/30/00 AM.
	bool genPost(Dlist<Iaction> *, Gen *);								// 05/04/00 AM.
	bool genExecphrase(Gen *);												// 05/05/00 AM.
	bool hasPres();															// 05/24/00 AM.
	static bool genPres(Dlist<Irule> *, Gen *);						// 05/23/00 AM.
				// Moved from ifile.											// 05/30/00 AM.
	static bool genRule(Dlist<Irule> *, _TCHAR *, Gen *);			// 05/02/00 AM.
			// 05/30/00 AM. Moved here from ifile. Confusing overloading of
			// names for genRule()....
	static bool genHash(Dlist<Irule> *, Gen *);						// 06/14/00 AM.
	bool genHash(Gen *);														// 06/14/00 AM.
	static bool genMusts(Slist<Irule>*,Gen*);							// 06/16/00 AM.
	static long genConflicts(Slist<Irule>*,std::_t_ostream*);				// 06/17/00 AM.
private:
	Dlist<Ielt> *phrase_;				// Rule's phrase of elements.
	Isugg *sugg_;						// Rule's suggested concept.
	Dlist<Ipre> *pre_;					// Rule's pre-actions.
	Dlist<Iaction> *checks_;			// Rule's check-actions.	// 11/27/98 AM.
	Dlist<Iaction> *post_;				// Rule's post-actions.

	// Convenience stuff.
	Delt<Ielt> *trigger_;			// Rule's trigger element.
	int lookahead_;					// Ord of lookahead elt.		// 11/22/99 AM.

	long num_;				// Number of rule in current pass.		// 12/18/98 AM

	long line_;		// Track line number in pass file.				// 05/17/01 AM.

	// RUG: Automatically generated rules can point to a sample text.
	_TCHAR *sample_;							// 06/16/99 AM.

	// RUG: A way for a rule to point to more specialized variants.
	// To be used as a way for the most generic rule to point to the
	// list of literal rules that coalesced under it.				// 06/28/99 AM.
	Slist<Irule> *down_;

	// RUG: A count of the number of labels in rule.				// 07/16/99 AM.
	int nlabels_;																// 07/16/99 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;				// Count nodes currently allocated.
#endif
};



#endif
