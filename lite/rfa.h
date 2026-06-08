/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	RFA.H
* FILE:	lite\rfa.h
* CR:		10/21/98 AM.
* SUBJ:	Declares for the rule-file analyzer (RFA) module.
* NOTE:	
*
*******************************************************************************/

#ifndef RFA_H_
#define RFA_H_

//#include "lite/dlist.h"

// The classes used in defining a rule set.
//#include "Istr.h"
#include "iarg.h"
#include "ipair.h"
#include "ielement.h"
#include "iaction.h"
#include "ielt.h"
#include "isugg.h"
#include "ipre.h"
#include "ipost.h"
#include "ifile.h"
#include "irule.h"

class RFA
{
public:
	RFA(bool debug = false);	// Default constructor.
	~RFA();							// Destructor.

	// Access
	static bool Debug();

	// Modify
	static void setDebug(bool);

	static void make_rfa(Ana &rfa);
	static bool execute_rfa(_TCHAR *file, Ana &rfa, _TCHAR *specdir,
										Eana *erfa,								// 10/13/99 AM.
										_TCHAR *algo,								// 05/31/00 AM.
										long passnum,							// 08/23/02 AM.
										/*UP*/
										Ifile* &rulesfile,
										bool &intern							// 05/26/01 AM.
										);
	static _TCHAR *rfa_fix_file_name(
		_TCHAR *specdir,				// 12/03/98 AM.
		_TCHAR *file,
		_TCHAR *suff
		)
		;
	static Ifile *rfa_output(Parse &parse);

	static void rfa_test(Ana &rfa);

	// Build the passes.
	static void rfa_retok(Ana &rfa);
	static void rfa_bigtok(Ana &rfa);
	static void rfa_components(Ana &rfa);
	static void rfa_list(Ana &rfa);
	static void rfa_action(Ana &rfa);
	static void rfa_pair(Ana &rfa);
	//static void rfa_x_white(Ana &rfa);
	static void rfa_pairs(Ana &rfa);
	static void rfa_element(Ana &rfa);
	static void rfa_rule(Ana &rfa);
	static void rfa_rules(Ana &rfa);
	static void rfa_actions(Ana &rfa);
	static void rfa_region(Ana &rfa);
	static void rfa_code(Ana &rfa);					// 12/07/98 AM.
	static void rfa_nodes(Ana &rfa);					// 11/30/98 AM.
	static void rfa_multi(Ana &rfa);					// 03/25/99 AM.
	static void rfa_path(Ana &rfa);					// 10/19/99 AM.
	static void rfa_select(Ana &rfa);				// 11/30/98 AM.
	static void rfa_regions(Ana &rfa);
	static void rfa_recurse(Ana &rfa);				// 11/25/98 AM.
	static void rfa_recurses(Ana &rfa);				// 11/25/98 AM.
	static void rfa_rulesfile(Ana &rfa);

	static void rfa_excise(
		_TCHAR *name,			// The node name to zap. eg "_xWHITE".
		_TCHAR *passname,	// eg, "x_white".
		Ana &rfa
		);

	// Recursively called rule sets.
	//Dlist<Irule> *rfa_rulelt();
	static Seqn *rfa_elt(Ana &rfa);
	static Seqn *rfa_rulelt(Ana &rfa);
	static Seqn *rfa_rulelts(Ana &rfa);
	static Seqn *rfa_sugg(Ana &rfa);

	static Seqn *rfa_listarg(Ana &rfa);			// 11/21/98 AM.
	static Seqn *rfa_argtolist(Ana &rfa);		// 11/21/98 AM.
	static Seqn *rfa_littopair(Ana &rfa);		// 11/22/98 AM.
	static Seqn *rfa_littoaction(Ana &rfa);	// 11/23/98 AM.
	static Seqn *rfa_littopre(Ana &rfa);		// 11/29/98 AM.

	// Single-rule convenience functions.
	static void rfa_rule_bs_lit(
		_TCHAR *chstr,						// The char as a string. (eg, ">").
		_TCHAR *suggstr,						// The suggested name (eg, "_cRANGLE").
		Dlist<Irule> *rules,				// Current rules list being built.
		_TCHAR *rename = 0					// Rename 2nd elt, if alphabetic.
	);
	static void rfa_rule_key(
		_TCHAR *keystr,						// The keyword.
		_TCHAR *suggstr,						// The suggested name (eg, "_POST").
		Dlist<Irule> *rules				// Current rules list being built.
		);
	static void rfa_rule_endkey(
		_TCHAR *keystr,						// The keyword.
		_TCHAR *suggstr,						// The suggested name (eg, "_POST").
		Dlist<Irule> *rules				// Current rules list being built.
		);

	static void rfa_clean(Ana &rfa);			// 11/24/98 AM.
	static void rfa_clean_seqns(Dlist<Seqn> *);	// 11/24/98 AM.

private:
	static bool debug_;					// If debugging output.

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

