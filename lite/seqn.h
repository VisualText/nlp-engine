/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SEQN.H
* FILE:	lite\seqn.h
* CR:		10/13/98 AM.
* SUBJ:	Declares for sequence dlist's data.
* NOTE:	Data for list representing analyzer sequence.  Part of analyzer
*			DEFINITION, not a particular parse instance.  (see Parse.h)
*
*******************************************************************************/

#ifndef SEQN_H_
#define SEQN_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/dlist.h"
#include "lite/algo.h"

// Forward reference.
class Irule;
class Ifile;				// 11/25/98 AM.
class Iarg;					// 12/04/98 AM.
class Gen;					// 05/10/00 AM.
class Pat;		// OPT.	// 05/16/00 AM.

#define MAXRFN 64
// Annoying htab issues.	// 05/21/09 AM.
#define MAXALGONM 32

/********************************************
* CLASS:	SEQN
* CR:		10/13/98 AM.
* SUBJ:	Sequence dlist's data.
********************************************/

class Seqn
{
	friend class Pat;			// OPT												// 05/16/00 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Seqn &);

public:
	Seqn(		// Default constructor.
		Algo * = 0,
		_TCHAR * rfile = 0,
		Dlist<Irule> * = 0,
		_TCHAR *name = 0
		);

	// New constructor for pass via GUI.		// 01/13/99 AM.
	Seqn(
		bool active,				// = true
		_TCHAR *algo_name	= 0,
		_TCHAR *rules_file_name = 0,
		_TCHAR *comment = 0
		);

	~Seqn();

	void unintern();								// 01/15/99 AM.

	// Access
	Algo *getAlgo();
	_TCHAR *getRulesfilename();
	Dlist<Irule> *getRules();
	_TCHAR *getName();
	Ifile *getRulesfile();
	Dlist<Iarg> *getSelects();			// 11/30/98 AM.
	bool getActive();						// 01/08/99 AM.
	bool getDisplaytree();
	_TCHAR *getAlgoname();					// 01/08/99 AM.
	_TCHAR *getComment();					// 01/08/99 AM.
	int   getPassnum();					// 09/23/99 AM.
	int	getRecid();						// 05/31/00 AM.

	// Modify
	void setAlgo(Algo *);
	void setRulesfilename(_TCHAR *);
	void setRules(Dlist<Irule> *);
	void setName(_TCHAR *);
	void setRulesfile(Ifile *);
	void setSelects(Dlist<Iarg> *);
	void setActive(bool);				// 01/08/99 AM.
	void setDisplaytree(bool);
	void setAlgoname(_TCHAR *);			// 01/08/99 AM.
	void setComment(_TCHAR *);			// 01/08/99 AM.
	void setPassnum(int);				// 09/23/99 AM.
	void setRecid(int);					// 05/31/00 AM.

	// General
	bool genSeqn(_TCHAR *dir);			// 11/04/99 AM.
	void genFileheader(					// 11/04/99 AM.
		_TCHAR *fname,
		_TCHAR *subject,
		_TCHAR *author,
		_TCHAR *date,
		_TCHAR *modified,
		_TCHAR *note,
		std::_t_ostream &ofile
		);

	// Class-wide
	static Dlist<Seqn> *makeDlist(
			Algo *,
			_TCHAR *			= 0,
			Dlist<Irule> *	= 0,
			Ifile *			= 0				// 11/25/98 AM.
			);
	static Delt<Seqn> *makeDelt(
			Algo *,
			_TCHAR *			= 0,
			Dlist<Irule> *	= 0,
			Ifile *			= 0				// 11/25/98 AM.
			);

	static bool same(Dlist<Seqn> *seqn1, Dlist<Seqn> *seqn2);	// 06/07/99 AM.
	static bool same(Seqn *seqn1, Seqn *seqn2);						// 06/07/99 AM.

	static bool genRecurses(												// 11/04/99 AM.
		Dlist<Seqn> *seqs,
		_TCHAR *sep,
		std::_t_ostream &ofile
		);

	static bool gen(Dlist<Seqn> *, Gen *);								// 05/10/00 AM.
	static bool genRecurse(													// 05/31/00 AM.
		Dlist<Seqn> *seqs,
		Gen *gen
		);

private:
	Algo *algo;				// Current analysis pass. (ALGORITHM)

	// Gonna use this for the folder name also.						// 11/12/00 AM.
	// Should probably be called "data".								// 11/12/00 AM.
	_TCHAR rules_file_name_[MAXRFN];	// File containing rules for a pat pass.
								// (ALGORITHM).
	Dlist<Irule> *rules_;	// Rules list for (pat) algorithm.

	_TCHAR *name_;			// Eg, recurse passes will have a name.

	Ifile *rulesfile_;	// Internal representation of rules file.// 11/25/98 AM.

	bool displaytree_;			// Force tree to be displayed

	// New data to enable user-editing of the analyzer sequence.// 01/08/99 AM.
	bool active_;				// If this pass will be executed.	// 01/08/99 AM.
	_TCHAR comment_[MAXSTR];	// User comment for pass.				// 01/08/99 AM.
//	_TCHAR *algo_name_;		// Formal name of algorithm (eg, pat).	// 01/08/99 AM.
	_TCHAR algo_name_[MAXALGONM];		// Formal name of algorithm (eg, pat).	// 01/08/99 AM.

	int passnum_;			// Ordinal number of pass in sequence.	// 09/23/99 AM.

	int recid_;				// Number of recursive minipass.			// 05/31/00 AM.


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
