/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	ARUN.CPP
* FILE:	lite/Arun.cpp
* CR:		05/11/00 AM.
* SUBJ:	API for compiled runtime system.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include <limits.h>					// 05/17/00 AM.
#include <float.h>					// 08/17/01 AM.
//#include <tchar.h>					// 09/26/01 AM.
#include <locale.h>					// 01/06/03 AM.
#include "consh/libconsh.h"		// 02/14/01 AM.
#include "consh/cg.h"				// 02/14/01 AM.
#include "lite/global.h"			// 01/24/01 AM.
#include "u_out.h"	// TRYING.	// 01/19/06 AM.
#include "inline.h"					// 09/26/01 AM.
#include "dlist.h"					// 02/26/01 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"						// 02/26/01 AM.
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "thtab.h"					// 02/26/01 AM.
#include "tok.h"
#include "line.h"

#include "irule.h"					// 01/06/02 AM.
#include "ielt.h"						// 03/28/05 AM.

#include "lite/lite.h"
#include "lite/Arun.h"
#include "lite/code.h"				// 06/04/00 AM.
#include "inline.h"					// 06/06/00 AM.
#include "lite/nlppp.h"
#include "chars.h"					// 06/02/00 AM.
#include "xml.h"						// 03/28/05 AM.
#include "pat.h"
#include "rec.h"						// 06/01/00 AM.
#include "Eana.h"						// 05/20/00 AM.
#include "parse.h"					// 05/17/00 AM.
#include "ana.h"						// 06/02/00 AM.
#include "ivar.h"						// 05/25/00 AM.
#include "var.h"						// 05/25/00 AM.
#include "iexpr.h"					// 06/01/00 AM.
#include "ipair.h"					// 02/27/01 AM.
#include "ipnode.h"					// 04/28/01 AM.
#include "rfasem.h"					// 05/26/00 AM.
extern int compare_numattrs( const void *arg1, const void *arg2 );
extern int compare_strattrs( const void *arg1, const void *arg2 );

// Array of names to keep in sync with enum specialType			// 06/09/00 AM.
// Set up for binary search, if you like.								// 06/09/00 AM.
// Could determine size of array dynamically by traversing to
// null terminator (once only at start of program).
_TCHAR *specialArr[] =															// 06/09/00 AM.
	{
	0,			// Null first element.
	_T("_xALPHA"),
	_T("_xANY"),
	_T("_xBLANK"),
	_T("_xCAP"),
	_T("_xCTRL"),																	// 07/24/00 AM.
	_T("_xEND"),
	_T("_xEOF"),
	_T("_xFEAT"),
	_T("_xNIL"),
	_T("_xNUM"),
	_T("_xPUNCT"),
	_T("_xSTART"),
	_T("_xWHITE"),
	_T("_xWILD"),
	0	// Dummy terminator.
	};

// Length of above array to be determined by a one-time call to
// set_specialarr_len.														// 06/09/00 AM.
long SPECIALARR_LEN = 0;	// Global to keep length of array.	// 06/09/00 AM.

/******************************************************************************/
// PASSES
/******************************************************************************/

/********************************************
* FN:		TOKENIZE
* CR:		05/11/00 AM.
* SUBJ:	Runtime variant of tokenize pass.
* NOTE:	An expedient static function to call the regular tokenize
*			pass, for now.
********************************************/

bool Arun::tokenize(
	Parse *parse,
	int num									// Pass number.				// 05/21/00 AM.
	)
{
_t_ofstream *fout;				// File pass output.
_t_ostream *sout;					// For restoring output stream.
clock_t s_time;
_TCHAR *pretname;
_TCHAR *salgo = _T("tokenize");
_TCHAR *prefix = _T("ana");		// Prefix for naming files.
bool flogfiles = parse->eana_->getFlogfiles();
bool ftimepass = parse->eana_->getFtimepass();

parse->iniPass(num,prefix,flogfiles,ftimepass,0,salgo,			// 05/21/00 AM.
			/*DU*/fout,sout,s_time,pretname);

Tok tok;			// Create object.
if (!tok.Tokenize(parse))													// 01/26/02 AM.
	return false;																// 01/26/02 AM.

parse->finPass(num,flogfiles,fout,sout,
										pretname,ftimepass,s_time);		// 05/21/00 AM.
return true;
}


/********************************************
* FN:		LINES
* CR:		05/11/00 AM.
* SUBJ:	Runtime variant of lines pass.
* NOTE:	An expedient static function to call the regular lines
*			pass, for now.
********************************************/

bool Arun::lines(
	Parse *parse,
	int num									// Pass number.				// 05/21/00 AM.
	)
{
_t_ofstream *fout;				// File pass output.
_t_ostream *sout;					// For restoring output stream.
clock_t s_time;
_TCHAR *pretname;
_TCHAR *salgo = _T("lines");
_TCHAR *prefix = _T("ana");		// Prefix for naming files.
bool flogfiles = parse->eana_->getFlogfiles();
bool ftimepass = parse->eana_->getFtimepass();

parse->iniPass(num,prefix,flogfiles,ftimepass,0,salgo,			// 05/21/00 AM.
			/*DU*/fout,sout,s_time,pretname);

Line ln;			// Create object.
ln.Gather(parse);

parse->finPass(num,flogfiles,fout,sout,
										pretname,ftimepass,s_time);		// 05/21/00 AM.
return true;
}


/********************************************
* FN:		PATEXECUTE
* CR:		05/11/00 AM.
* SUBJ:	Runtime variant of pat pass.
********************************************/

bool Arun::patExecute(
	Parse *parse,					// Current parse.
	const PASS *pass,				// Static data for current pass.	// 06/20/00 AM.
	int num,									// Pass number.				// 05/20/00 AM.
	_TCHAR *sfile								// Pass file name.			// 05/20/00 AM.
#ifdef OLD_
	bool (code)(Nlppp *),				// Gen'd fn for code region of this pass.
	const _TCHAR **arr_select,			// Array of select node names.
	enum selectType seltype,			// Traversal mode.			// 06/08/00 AM.
	bool (rules)(int,bool&,Nlppp *),	// Gen'd fn for rules of this pass.
	const RHASH *must,					// Must-try rule data.		// 06/19/00 AM.
	const RHASH **htab					// Rule hash table.			// 06/19/00 AM.
#endif
	)
{
_t_ofstream *fout;				// File pass output.
_t_ostream *sout;					// For restoring output stream.
clock_t s_time;
_TCHAR *pretname;
_TCHAR *salgo = _T("pat");
_TCHAR *prefix = _T("ana");		// Prefix for naming files.
bool flogfiles = parse->eana_->getFlogfiles();
bool ftimepass = parse->eana_->getFtimepass();

parse->iniPass(num,prefix,flogfiles,ftimepass,sfile,salgo,		// 05/20/00 AM.
			/*DU*/fout,sout,s_time,pretname);

// if (pass->getActive())	// Need to get active flag.
if (pass->htab)																// 06/20/00 AM.
//	Pat::Execute(parse,code,arr_select,seltype,must,htab,rules);// 06/20/00 AM.
	Pat::Hexecute(parse, pass);										// 06/21/00 AM.
else
	{
//	Pat::Execute(parse,code,arr_select,seltype,rules);
	if (!																			// 01/26/02 AM.
	Pat::Execute(parse, pass))												// 06/21/00 AM.
		return false;															// 01/26/02 AM.
	}

parse->finPass(num,flogfiles,fout,sout,
										pretname,ftimepass,s_time);		// 05/20/00 AM.
return true;
}


/********************************************
* FN:		RECEXECUTE
* CR:		06/01/00 AM.
* SUBJ:	Runtime variant of rec pass.
********************************************/

bool Arun::recExecute(
	Parse *parse,							// Current parse.
	const PASS *pass,
	int num,									// Pass number.				// 05/20/00 AM.
	_TCHAR *sfile							// Pass file name.			// 05/20/00 AM.
#ifdef OLD_
	bool (code)(Nlppp *),				// Gen'd fn for code region of this pass.
	const _TCHAR **arr_select,			// Array of select node names.
	enum selectType seltype,			// Select traversal mode.	// 06/08/00 AM.
	bool (rules)(int,bool&,Nlppp *),	// Gen'd fn for rules of this pass.
	const RHASH *must,					// Must-try rule data.		// 06/19/00 AM.
	const RHASH **htab					// Rule hash table.			// 06/19/00 AM.
#endif
	)
{
_t_ofstream *fout;				// File pass output.
_t_ostream *sout;					// For restoring output stream.
clock_t s_time;
_TCHAR *pretname;
_TCHAR *salgo = _T("rec");															// 06/13/00 AM.
_TCHAR *prefix = _T("ana");		// Prefix for naming files.
bool flogfiles = parse->eana_->getFlogfiles();
bool ftimepass = parse->eana_->getFtimepass();

parse->iniPass(num,prefix,flogfiles,ftimepass,sfile,salgo,		// 05/20/00 AM.
			/*DU*/fout,sout,s_time,pretname);

// if (pass->getActive())	// Need to get active flag.
//	Rec::Execute(parse, code, arr_select, seltype, rules);
if (!																				// 01/26/02 AM.
	Rec::Execute(parse,pass))
	return false;																// 01/26/02 AM.

parse->finPass(num,flogfiles,fout,sout,
										pretname,ftimepass,s_time);		// 05/20/00 AM.
return true;
}

/********************************************
* FN:		CHECKFINAL
* CR:		08/07/02 AM.
* SUBJ:	Runtime variant of checkFinal.
* NOTE:	Operates only when fail in check region.
********************************************/

bool Arun::checkfinal(bool ret, Nlppp *nlppp)
{
return Pat::checkFinal(ret,nlppp);
}

/********************************************
* FN:		EXITPASS
* CR:		08/07/02 AM.
* SUBJ:	Runtime function for exitpass.
* NOTE:	
********************************************/

bool Arun::exitpass(bool ret, Nlppp *nlppp)
{
nlppp->exitpass_ = true;
return ret;
}


/******************************************************************************/
// PATTERN MATCHER
/******************************************************************************/


/********************************************
* FN:		MATCH_FIRST
* CR:		05/11/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_first(
	int ord,						// 05/18/00 AM.
	bool (match)(NODE *, const ELT *,Nlppp *),
	Nlppp *nlppp
	)
{
if (!match_right(ord,match,nlppp))										// 05/23/00 AM.
	return false;
return true;
}


/********************************************
* FN:		MATCH_LEFT
* CR:		05/11/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	05/30/00 AM. Copying from match_right.
********************************************/

bool Arun::match_left(
	int ord,						// 05/18/00 AM.
	bool (match)(NODE *, const ELT *,Nlppp *),
	Nlppp *nlppp
	)
{
nlppp->ord_ = ord;
Node<Pn> *save = nlppp->node_;
COLL *coll;
const ELT *elt = &(nlppp->elts_[ord]);

if (!nlppp->node_)	// No more nodes in current context.
	{
	if (!elt->min											// Optional
	 || find_special_tok(XSTART,elt->match.spec)// _xSTART match // 08/10/02 AM.
	 )
		{
		// Update status.
		coll = &(nlppp->coll_[ord]);
		coll->start = coll->end = 0;
		coll->totnodes = 0;
		return true;
		}
	return false;
	}

if (nlppp->wildord_)				// If pending wildcards.
	{
	// Assume we're one past the minimum constraint.
	int count = nlppp->wildtot_;
	int limit = nlppp->wildmax_;
	bool matched = false;
	for (;;)
		{
		if (matched = match_real_left(match,nlppp))
			break;
		++count;
		if (limit && count > limit)
			break;
		if (!(nlppp->node_ = Pat::nodeLeft(nlppp->node_, nlppp->start_)))
			break;
		}

	if (matched)						// Matched
		{
		nlppp->wildtot_ = count;			// New number of nodes in wild range.
		// Backfill wildcards.
		if (nlppp->wildord_)
			if (!Pat::wild_backfill_left(nlppp))
				return false;

		// Update state of rule match.
		coll = &(nlppp->coll_[ord]);
//		nlppp->node_ = nlppp->start_ = (Node<Pn> *) coll->start; // BUG.
		nlppp->node_ = nlppp->first_							// FIX.	// 08/09/02 AM.
								= (Node<Pn> *) coll->start;	// FIX.	// 08/09/02 AM.
		if (!nlppp->last_)	// No nodes matched yet.
			nlppp->last_ = (Node<Pn> *) coll->end;

		// Try to move left one node.
		nlppp->node_ = Pat::nodeLeft(nlppp->first_, nlppp->start_);
		return true;
		}
	else if (!elt->min)					// Optional
		{
		nlppp->node_ = save;		// Restore position.
		return true;
		}
	return false;	// Failure.
	}

// Match a real node in parse tree.
if (!match_real_left(match,nlppp))
	{
	if (!elt->min)		// Optional
		{
		nlppp->node_ = save;		// Restore.
		return true;
		}
	return false;
	}

// Match succeeded.  Get data from collect.
coll = &(nlppp->coll_[ord]);

// Update status.
nlppp->node_ = nlppp->first_ = (Node<Pn> *) coll->start;

if (!nlppp->last_)
	nlppp->last_ = (Node<Pn> *) coll->end;

// Try to move left one node.
nlppp->node_ = Pat::nodeLeft(nlppp->first_, nlppp->start_);
return true;
}


/********************************************
* FN:		MATCH_RIGHT
* CR:		05/11/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/

bool Arun::match_right(
	int ord,						// 05/18/00 AM.
	bool (match)(NODE *, const ELT *,Nlppp *),
	Nlppp *nlppp
	)
{
nlppp->ord_ = ord;
Node<Pn> *save = nlppp->node_;
COLL *coll;
const ELT *elt = &(nlppp->elts_[ord]);

if (!nlppp->node_)	// No more nodes in current context.
	{
	if (!elt->min										// Optional
	 || find_special_tok(XEND,elt->match.spec)// _xEND matches.	// 08/10/02 AM.
	 || match_list_eof(nlppp->last_,elt)		// _xEOF matches.	// 08/10/02 AM.
	 )
		{
		// Update status.
		coll = &(nlppp->coll_[ord]);
		coll->start = coll->end = 0;
		coll->totnodes = 0;
		return true;
		}
	return false;
	}

if (nlppp->wildord_)				// If pending wildcards.
	{
	// Assume we're one past the minimum constraint.
	int count = nlppp->wildtot_;
	int limit = nlppp->wildmax_;
	bool matched = false;
	for (;;)
		{
		if (matched = match_real_right(match,nlppp))					// 05/23/00 AM.
			break;
		++count;
		if (limit && count > limit)
			break;
		if (!(nlppp->node_ = Pat::nodeRight(nlppp->node_, nlppp->end_)))
			break;
		Pat::updateRestart(nlppp, nlppp->node_);			// RECOPT2.	// 07/25/06 AM.
		}

	if (matched)						// Matched
		{
		nlppp->wildtot_ = count;			// New number of nodes in wild range.
		// Backfill wildcards.
		if (nlppp->wildord_)
			if (!Pat::wild_backfill_right(nlppp))
				return false;

		// Update state of rule match.
		coll = &(nlppp->coll_[ord]);
		nlppp->node_ = nlppp->last_ = (Node<Pn> *) coll->end;
		if (!nlppp->first_)	// No nodes matched yet.				// 05/25/00 AM.
			nlppp->first_ = (Node<Pn> *) coll->start;					// 05/25/00 AM.

		// Try to move right one node.
		nlppp->node_ = Pat::nodeRight(nlppp->last_, nlppp->end_);
		Pat::updateRestart(nlppp, nlppp->node_);			// RECOPT2.	// 07/25/06 AM.
		return true;
		}
	else if (!elt->min)					// Optional
		{
		nlppp->node_ = save;		// Restore position.
		return true;
		}
	return false;	// Failure.
	}

// Match a real node in parse tree.
if (!match_real_right(match,nlppp))										// 05/23/00 AM.
	{
	if (!elt->min)		// Optional
		{
		nlppp->node_ = save;		// Restore.
		return true;
		}
	return false;
	}

// Match succeeded.  Get data from collect.
coll = &(nlppp->coll_[ord]);

// Update status.
nlppp->node_ = nlppp->last_ = (Node<Pn> *) coll->end;

if (!nlppp->first_)															// 05/23/00 AM.
	nlppp->first_ = (Node<Pn> *) coll->start;							// 05/23/00 AM.

// Try to move right one node.
nlppp->node_ = Pat::nodeRight(nlppp->last_, nlppp->end_);
Pat::updateRestart(nlppp,((Node<Pn> *)nlppp->node_));	// RECOPT2.	// 07/24/06 AM.
return true;
}


/********************************************
* FN:		MATCH_LEFT_WILD
* CR:		05/17/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	05/30/00 AM. Copying from match_right_wild.
********************************************/

bool Arun::match_left_wild(
	int ord,						// 05/18/00 AM.
	Nlppp *nlppp
	)
{
COLL *coll;
nlppp->ord_ = ord;
const ELT *elt = &(nlppp->elts_[ord]);

if (!elt->min)									// Wildcard has no min constraint.
	{
	//if (ord == nlppp->len_)
	// Allowing wildcard at end of rule now.

	// Update status.
	coll = &(nlppp->coll_[ord]);
	coll->start = coll->end = 0;
	coll->totnodes = 0;
	if (!nlppp->wildord_)				// No pending wildcards...
		{
		nlppp->wildord_ = ord;
		nlppp->wildmark_ = 0;			// No nodes reserved for wildcards.
		nlppp->wildstart_ = nlppp->node_;								// 06/01/00 AM.
		nlppp->wildmin_ = elt->min;
		nlppp->wildmax_ = elt->max;
		nlppp->wildtot_ = 0;
		}
	else										// There are pending wildcards.
		{
		//nlppp->wildmin_ += elt->min;
		if (!nlppp->wildmax_ || !elt->max)	// One of these is unbounded.
			nlppp->wildmax_ = 0;			// Unbounded upper limit.
		else
			nlppp->wildmax_ += elt->max;		// Else, accrue max possible.
		//nlppp->wildtot_ += elt->min;
		}
	return true;							// Wildcard "match" succeeded.
	}


if (!nlppp->node_)		// No nodes remain to match.
	{
	// But there's a min requirement, so fail.
	return false;
	}

if (!nlppp->wildord_)	// No wildcards pending.					// 06/01/00 AM.
	nlppp->wildstart_ = nlppp->node_;									// 06/01/00 AM.

// Wildcard has min constraint, make sure there are enough nodes
// around to satisfy it.  Traverse to the appropriate node and set
// a mark there.
// Also, if first not set, set it here. (Ie, allowing wildcard at
// start and end of rule now.)
Node<Pn> *fm = nlppp->node_;
Node<Pn> *to = fm;															// 06/01/00 AM.
int ii;
for (ii = 1; ii < elt->min; ++ii)
	{
	if (!(to = Pat::nodeLeft(fm, nlppp->start_)))
		return false;				// Not enough nodes.
	}

// Ok, there were enough nodes.
// Update status.
coll = &(nlppp->coll_[ord]);
coll->start = coll->end = 0;		// Don't assign nodes yet.
coll->totnodes = elt->min;				// Wildcard "owns" min nodes so far.
if (!nlppp->wildord_)				// No pending wildcards...
	{
	nlppp->wildord_ = ord;
	nlppp->wildmark_ = to;			// Min nodes reserved for wildcards.
	nlppp->wildmin_ = elt->min;
	nlppp->wildmax_ = elt->max;
	nlppp->wildtot_ = elt->min;
	}
else										// There are pending wildcards.
	{
	nlppp->wildmark_ = to;			// New end of nodes reserved for wildcards.
	nlppp->wildmin_ += elt->min;
	if (!nlppp->wildmax_ || !elt->max)	// One of these is unbounded.
		nlppp->wildmax_ = 0;			// Unbounded upper limit.
	else
		nlppp->wildmax_ += elt->max;		// Else, accrue max possible.
	nlppp->wildtot_ += elt->min;
	}
// Try to move right one node.
nlppp->node_ = to;	// NEW POSITION.									// 06/01/00 AM.
nlppp->node_ = Pat::nodeLeft(nlppp->node_, nlppp->start_);		// 06/01/00 AM.
return true;							// Wildcard "match" succeeded.
}


/********************************************
* FN:		MATCH_RIGHT_WILD
* CR:		05/17/00 AM.
* SUBJ:	Runtime match of wild elt.
* NOTE:	Allowing wildcard at start and end of rule in compiled version.
********************************************/

bool Arun::match_right_wild(
	int ord,																		// 05/18/00 AM.
	Nlppp *nlppp
	)
{
COLL *coll;
nlppp->ord_ = ord;
const ELT *elt = &(nlppp->elts_[ord]);

if (!elt->min)									// Wildcard has no min constraint.
	{
	//if (ord == nlppp->len_)
	// Allowing wildcard at end of rule now.

	// Update status.
	coll = &(nlppp->coll_[ord]);
	coll->start = coll->end = 0;
	coll->totnodes = 0;
	if (!nlppp->wildord_)				// No pending wildcards...
		{
		nlppp->wildord_ = ord;
		nlppp->wildmark_ = 0;			// No nodes reserved for wildcards.
		nlppp->wildstart_ = nlppp->node_;								// 06/01/00 AM.
		nlppp->wildmin_ = elt->min;
		nlppp->wildmax_ = elt->max;
		nlppp->wildtot_ = 0;
		}
	else										// There are pending wildcards.
		{
		//nlppp->wildmin_ += elt->min;
		if (!nlppp->wildmax_ || !elt->max)	// One of these is unbounded.
			nlppp->wildmax_ = 0;			// Unbounded upper limit.
		else
			nlppp->wildmax_ += elt->max;		// Else, accrue max possible.
		//nlppp->wildtot_ += elt->min;
		}
	return true;							// Wildcard "match" succeeded.
	}


if (!nlppp->node_)		// No nodes remain to match.
	{
	// But there's a min requirement, so fail.
	return false;
	}

if (!nlppp->wildord_)	// No wildcards pending.					// 06/01/00 AM.
	nlppp->wildstart_ = nlppp->node_;									// 06/01/00 AM.

// Wildcard has min constraint, make sure there are enough nodes
// around to satisfy it.  Traverse to the appropriate node and set
// a mark there.
// Also, if first not set, set it here. (Ie, allowing wildcard at
// start and end of rule now.)
Node<Pn> *fm = nlppp->node_;
Node<Pn> *to = fm;															// 06/01/00 AM.
int ii;
for (ii = 1; ii < elt->min; ++ii)
	{
	if (!(to = Pat::nodeRight(fm, nlppp->end_)))
		return false;				// Not enough nodes.
	}

// Ok, there were enough nodes.
// Update status.
coll = &(nlppp->coll_[ord]);
coll->start = coll->end = 0;		// Don't assign nodes yet.
coll->totnodes = elt->min;				// Wildcard "owns" min nodes so far.
if (!nlppp->wildord_)				// No pending wildcards...
	{
	nlppp->wildord_ = ord;
	nlppp->wildmark_ = to;			// Min nodes reserved for wildcards.
	nlppp->wildmin_ = elt->min;
	nlppp->wildmax_ = elt->max;
	nlppp->wildtot_ = elt->min;
	}
else										// There are pending wildcards.
	{
	nlppp->wildmark_ = to;			// New end of nodes reserved for wildcards.
	nlppp->wildmin_ += elt->min;
	if (!nlppp->wildmax_ || !elt->max)	// One of these is unbounded.
		nlppp->wildmax_ = 0;			// Unbounded upper limit.
	else
		nlppp->wildmax_ += elt->max;		// Else, accrue max possible.
	nlppp->wildtot_ += elt->min;
	}
// Try to move right one node.
nlppp->node_ = to;	// NEW POSITION.									// 06/01/00 AM.
nlppp->node_ = Pat::nodeRight(nlppp->node_, nlppp->end_);		// 06/01/00 AM.
Pat::updateRestart(nlppp, nlppp->node_);			// RECOPT2.	// 07/25/06 AM.
return true;							// Wildcard "match" succeeded.
}


/********************************************
* FN:		MATCHED_RULE
* CR:		05/13/00 AM.
* SUBJ:	Bookkeeping when a rule has matched.
********************************************/

bool Arun::matched_rule(Nlppp *nlppp,
	bool flagactions	// True if @post actions are present.		// 08/09/02 AM.
	)
{
if (nlppp->nlookahead_)														// 06/10/00 AM.
	nlppp->after_ = (Node<Pn> *)init_lookahead(nlppp);				// 06/10/00 AM.

if (!nlppp->after_)
	nlppp->after_ = Pat::nodeRight(nlppp->last_, nlppp->end_);

if (!flagactions)																// 08/09/02 AM.
	return true;																// 08/09/02 AM.

// COPIED FROM postActions.												// 08/09/02 AM.
// Mark all matched nodes as FIRED.										// 08/09/02 AM.
// Do this only if saving log files.									// 08/09/02 AM.
// (They may be excised later.  Don't care.)							// 08/09/02 AM.
Parse *parse = nlppp->parse_;				// Moved up.				// 08/09/02 AM.
Eana *eana = parse->getEana();			// Moved up.				// 08/09/02 AM.
if (eana->getFlogfiles())					// If saving logs.		// 08/09/02 AM.
	{
	// Traverse all nodes matched by the current rule.				// 08/09/02 AM.
	Node<Pn> *end = nlppp->last_;											// 08/09/02 AM.
	if (end)																		// 08/09/02 AM.
		end = end->Right();		// Sentinel to end loop.			// 08/09/02 AM.
	Node<Pn> *node = nlppp->first_;										// 08/09/02 AM.
	for (node = nlppp->first_; node != end; node = node->Right())// 08/09/02 AM.
		{
		Pn *pn = node->getData();											// 08/09/02 AM.
		pn->setFired(true);													// 08/09/02 AM.
		}
	}

return true;
}


/********************************************
* FN:		MATCH_REAL_LEFT
* CR:		05/30/00 AM.
* SUBJ:	Runtime match of elt to a real node.
* NOTE:	05/30/00 AM. Copying match_real_right
********************************************/

bool Arun::match_real_left(
	bool (match)(NODE *, const ELT *, Nlppp *),
	Nlppp *nlppp
	)
{
Node<Pn> *node = nlppp->node_;
Pn *pn = &(node->data);
const ELT *elt = &(nlppp->elts_[nlppp->ord_]);
_TCHAR *name = elt->name;
int min = elt->min;
int max = elt->max;

if (!(match)(node, elt, nlppp))			// The MATCH FN.
	return false;
if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))
	return false;

int count = 1;									// Count number of nodes matched.
Node<Pn> *from = node;
Node<Pn> *to = node;

// Repetition.  Match as greedily as possible.
if (!max)					// Indefinite repetition.
	max = INT_MAX;

Node<Pn> *save = node;		// Save current position.
while (count < max)
	{
	if (!(nlppp->node_ = Pat::nodeLeft(nlppp->node_, nlppp->start_)))
		break;
	pn = &(nlppp->node_->data);
	if (!(match)(nlppp->node_, elt, nlppp))	// The MATCH FN.
		break;				// Failed to match.
	if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))
		break;				// Failed on pre actions.
	// Matched.
	++count;
	to = nlppp->node_;
	}

if (count < min)
	{
	nlppp->node_ = save;			// Restore node position.
	return false;
	}

// Update data structures.
COLL *curr_coll = &(nlppp->coll_[nlppp->ord_]);
curr_coll->start = to;
curr_coll->end   = from;
curr_coll->totnodes = count;

return true;
}


/********************************************
* FN:		MATCH_REAL_RIGHT
* CR:		05/18/00 AM.
* SUBJ:	Runtime match of elt to a real node.
* NOTE:	05/23/00 AM. Changing the actual matching to a funarg.
********************************************/

bool Arun::match_real_right(
	bool (match)(NODE *, const ELT *, Nlppp *),
	Nlppp *nlppp
	)
{
Node<Pn> *node = nlppp->node_;
Pn *pn = &(node->data);
const ELT *elt = &(nlppp->elts_[nlppp->ord_]);
_TCHAR *name = elt->name;
int min = elt->min;
int max = elt->max;

if (!(match)(node, elt, nlppp))			// The MATCH FN.			// 05/23/00 AM.
	return false;
if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))		// 05/24/00 AM.
	return false;

int count = 1;									// Count number of nodes matched.
Node<Pn> *from = node;
Node<Pn> *to = node;

if (!nlppp->first_)											// RECOPT2.	// 07/25/06 AM.
	nlppp->first_ = node;									// RECOPT2.	// 07/25/06 AM.

// Repetition.  Match as greedily as possible.
if (!max)					// Indefinite repetition.
	max = INT_MAX;

Node<Pn> *save = node;		// Save current position.
Node<Pn> *saverightmost = 0;	// RECOPT2.	// 07/25/06 AM.
while (count < max)
	{
	if (!(nlppp->node_ = Pat::nodeRight(nlppp->node_, nlppp->end_)))
		break;
	pn = &(nlppp->node_->data);
	Pat::updateRestart(nlppp,nlppp->node_);			// RECOPT2.	// 07/25/06 AM.
	if (nlppp->rmost_)	// Curr rule updating rm.	// RECOPT2.	// 07/25/06 AM.
		saverightmost = nlppp->node_;						// RECOPT2.	// 07/25/06 AM.

	if (!(match)(nlppp->node_, elt, nlppp))	// The MATCH FN.	// 05/23/00 AM.
		break;				// Failed to match.
	if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))	// 05/24/00 AM.
		break;				// Failed on pre actions.					// 05/24/00 AM.
	// Matched.
	++count;
	to = nlppp->node_;
	}

if (count < min)
	{
	nlppp->node_ = save;			// Restore node position.			// 05/24/00 AM.
	// Reset rightmost info.	// RECOPT2.	// 07/25/06 AM.
	if (saverightmost)										// RECOPT2.	// 07/25/06 AM.
		{
		nlppp->rightmost_ = saverightmost;				// RECOPT2.	// 07/25/06 AM.
		nlppp->rmost_ = 0;									// RECOPT2.	// 07/25/06 AM.
		}
	return false;
	}

if (to != nlppp->node_	// Something didn't match.	// RECOPT2.	// 07/25/06 AM.
 && saverightmost)											// RECOPT2.	// 07/25/06 AM.
	{
	nlppp->rightmost_ = saverightmost;					// RECOPT2.	// 07/25/06 AM.
	nlppp->rmost_ = 0;										// RECOPT2.	// 07/25/06 AM.
	}

// Update data structures.
COLL *curr_coll = &(nlppp->coll_[nlppp->ord_]);
curr_coll->start = from;
curr_coll->end   = to;
curr_coll->totnodes = count;

return true;
}


/********************************************
* FN:		MATCH_SIMPLE
* CR:		05/16/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial, nonsinglet/nontree.
*			Match at least one real node or fail.
*			Should probably be in pat.cpp.
********************************************/

bool Arun::match_simple(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;								// 01/28/05 AM.

if (strcmp_i(elt->name,nname))
	return false;
if (elt->except.reg && find_str_nocase(nname, elt->except.reg))
	return false;
bool attop = true;	// 07/08/03 AM.
if (elt->except.spec && match_special_tok(node,elt->except.spec,nlppp,
						attop))	// 07/08/03 AM.
	return false;

return true;
}
/********************************************
* FN:		MATCH_SIMPLE_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial, nonsinglet/nontree.
*			Match at least one real node or fail.
*			For deaccented element.
********************************************/

bool Arun::match_simple_deaccent(NODE *node, const ELT *elt, Nlppp *nlppp)
{

Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);

bool ok = true;
bool attop = true;
if (strcmp_i(elt->name,dbuf))
	ok = false;
else if (elt->except.reg && find_str_nocase(dbuf, elt->except.reg))
	ok = false;
else if (elt->except.spec && match_special_tok(node,elt->except.spec,nlppp,
						attop))
	ok = false;

Chars::destroy(dbuf);
return ok;
}


/********************************************
* FN:		MATCH_SINGLET
* CR:		05/16/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	There are several ways to apply the match and the except
*			list.  This fn doesn't deal with match list.
********************************************/

bool Arun::match_singlet(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn;
bool success = false;
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (success = !strcmp_i(pn->name_, elt->name))
		break;
	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return false;
	}
if (!success)			// Didn't match.
	return false;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.reg && !elt->except.spec)						// OPT	// 06/07/00 AM.
	return true;													// OPT	// 06/07/00 AM.
bool attop = true;	// 07/08/03 AM.
nd = (Node<Pn> *) node;											// FIX	// 07/05/06 AM.
//for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
while (nd)															// FIX	// 07/05/06 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (elt->except.reg && find_str_nocase(pn->name_, elt->except.reg))
		return false;
	if (elt->except.spec && match_special_tok(nd,elt->except.spec,nlppp,
					attop))	// 07/08/03 AM.
		return false;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return true;
	nd = nd->pDown;			// Keep going down.			// FIX	// 07/05/06 AM.
	if (nd && nd->pRight)										// FIX	// 07/05/06 AM.
		return true;
	}
return true;
}


/********************************************
* FN:		MATCH_SINGLET_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	There are several ways to apply the match and the except
*			list.  This fn doesn't deal with match list.
*			Handling deaccented elt.
********************************************/

bool Arun::match_singlet_deaccent(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn;
bool success = false;
long len;
_TCHAR *dbuf;
_TCHAR *nname;
Node<Pn> *nd = (Node<Pn> *) node;
while (nd)
	{
	pn = &(((Node<Pn> *)nd)->data);
	nname = pn->name_;
	len = _tcsclen(nname);
	dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);

	if (success = !strcmp_i(dbuf, elt->name))
		{
		Chars::destroy(dbuf);
		break;
		}
	if (pn->getBase())
		{
		Chars::destroy(dbuf);
		return false;
		}
	Chars::destroy(dbuf);
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return false;
	}
if (!success)			// Didn't match.
	return false;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.reg && !elt->except.spec)						// OPT	// 06/07/00 AM.
	return true;													// OPT	// 06/07/00 AM.
bool attop = true;	// 07/08/03 AM.
nd = (Node<Pn> *) node;											// FIX	// 07/05/06 AM.
//for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
while (nd)															// FIX	// 07/05/06 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	nname = pn->name_;
	len = _tcsclen(nname);
	dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);
	if (elt->except.reg && find_str_nocase(dbuf, elt->except.reg))
		{
		Chars::destroy(dbuf);
		return false;
		}
	Chars::destroy(dbuf);
	if (elt->except.spec && match_special_tok(nd,elt->except.spec,nlppp,
					attop))	// 07/08/03 AM.
		return false;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return true;
	nd = nd->pDown;			// Keep going down.			// FIX	// 07/05/06 AM.
	if (nd && nd->pRight)										// FIX	// 07/05/06 AM.
		return true;
	}
return true;
}

/********************************************
* FN:		MATCH_TREE
* CR:		05/16/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/

bool Arun::match_tree(NODE *node, const ELT *elt, Nlppp *nlppp)
{
if (!match_tree(node, elt->name, true, nlppp))
	return false;
if (!elt->except.reg && !elt->except.spec)
	return true;
return !match_tree_list(node,elt->except,true, nlppp);
	return false; 
}

// Recursive workhorse.
bool Arun::match_tree(NODE *node, const _TCHAR *name, bool root, Nlppp *nlppp)
{
if (!node)
	return false;

Pn *pn = &(((Node<Pn> *)node)->data);
if (!strcmp_i(pn->name_, name))			// Match.	// FIX		// 06/07/00 AM.
	return true;

if (match_tree(((Node<Pn> *)node)->pDown, name, false, nlppp))	// Recurse down.
	return true;

// Manage list of nodes, if I'm the first in the list. (Minimizes recursion).
if (!root && !(((Node<Pn> *)node)->pLeft))
	{
	for (node = ((Node<Pn> *)node)->Right();
		  node;
		  node = ((Node<Pn> *)node)->Right())
		{
		if (match_tree(node,name,false,nlppp))
			return true;
		}
	}
return false;
}


// Recursive workhorse.
bool Arun::match_tree_list(
	NODE *node,
	ELIST elist,																// 06/15/00 AM.
#ifdef OLD_
	const _TCHAR ***hlist,														// 06/14/00 AM.
	const _TCHAR **list,											
	const int *spec,															// 06/09/00 AM.
#endif
	bool root,
	Nlppp *nlppp
	)
{
if (!node)
	return false;

Pn *pn = &(((Node<Pn> *)node)->data);
if (elist.htab)
	{
	if (hash_str_nocase(pn->name_, elist))
		return true;
	}
else if (elist.reg)
	{
	if (find_str_nocase(pn->name_, elist.reg))			// Match.
		return true;
	}
bool attop = true;	// 07/08/03 AM.
if (elist.spec && match_special_tok(node, elist.spec, nlppp,					// 06/09/00 AM.
				attop))	// 07/08/03 AM.
	return true;																// 06/09/00 AM.

if (match_tree_list(
		((Node<Pn> *)node)->pDown,elist,false,nlppp))	// Recurse down.
	return true;

// Manage list of nodes, if I'm the first in the list. (Minimizes recursion).
if (!root && !(((Node<Pn> *)node)->pLeft))
	{
	for (node = ((Node<Pn> *)node)->Right();
		  node;
		  node = ((Node<Pn> *)node)->Right())
		{
		if (match_tree_list(node,elist,false,nlppp))
			return true;
		}
	}
return false;
}


/********************************************
* FN:		MATCH_TREE_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
********************************************/

bool Arun::match_tree_deaccent(NODE *node, const ELT *elt, Nlppp *nlppp)
{
if (!match_tree_deaccent(node, elt->name, true, nlppp))
	return false;
if (!elt->except.reg && !elt->except.spec)
	return true;
return !match_tree_list_deaccent(node,elt->except,true, nlppp);
	return false; 
}

// Recursive workhorse.
bool Arun::match_tree_deaccent(NODE *node, const _TCHAR *name, bool root, Nlppp *nlppp)
{
if (!node)
	return false;

Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);

if (!strcmp_i(dbuf, name))			// Match.
	{
	Chars::destroy(dbuf);
	return true;
	}
Chars::destroy(dbuf);

if (match_tree_deaccent(((Node<Pn> *)node)->pDown, name, false, nlppp))	// Recurse down.
	return true;

// Manage list of nodes, if I'm the first in the list. (Minimizes recursion).
if (!root && !(((Node<Pn> *)node)->pLeft))
	{
	for (node = ((Node<Pn> *)node)->Right();
		  node;
		  node = ((Node<Pn> *)node)->Right())
		{
		if (match_tree_deaccent(node,name,false,nlppp))
			return true;
		}
	}
return false;
}


// Recursive workhorse.
bool Arun::match_tree_list_deaccent(
	NODE *node,
	ELIST elist,
	bool root,
	Nlppp *nlppp
	)
{
if (!node)
	return false;

Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);

if (elist.htab)
	{
	if (hash_str_nocase(dbuf, elist))
		{
		Chars::destroy(dbuf);
		return true;
		}
	}
else if (elist.reg)
	{
	if (find_str_nocase(dbuf, elist.reg))			// Match.
		{
		Chars::destroy(dbuf);
		return true;
		}
	}
Chars::destroy(dbuf);

bool attop = true;	// 07/08/03 AM.
if (elist.spec && match_special_tok(node, elist.spec, nlppp,					// 06/09/00 AM.
				attop))	// 07/08/03 AM.
	return true;																// 06/09/00 AM.

if (match_tree_list_deaccent(
		((Node<Pn> *)node)->pDown,elist,false,nlppp))	// Recurse down.
	return true;

// Manage list of nodes, if I'm the first in the list. (Minimizes recursion).
if (!root && !(((Node<Pn> *)node)->pLeft))
	{
	for (node = ((Node<Pn> *)node)->Right();
		  node;
		  node = ((Node<Pn> *)node)->Right())
		{
		if (match_tree_list_deaccent(node,elist,false,nlppp))
			return true;
		}
	}
return false;
}


/********************************************
* FN:		MATCH_LIST
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_list(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *name = pn->name_;
bool attop = true;	// 07/08/03 AM.

// Note: shouldn't keep both hashed and unhashed in same rule elt.
if (elt->match.htab)	// USE HASHING IF AVAILABLE.					// 06/14/00 AM.
	{
	if (hash_str_nocase(name, elt->match))
		goto matched;
	}
else if (elt->match.reg)
	{
	if (find_str_nocase(name, elt->match.reg))
		goto matched;
	}

if (!elt->match.spec
			|| !match_special_tok(node, elt->match.spec, nlppp,		// 06/14/00 AM.
								attop)) // 07/08/03 AM.
	return false;

matched:

if (elt->except.htab)
	{
	if (hash_str_nocase(name, elt->except))
		return false;
	}
else if (elt->except.reg)
	{
	if (find_str_nocase(name, elt->except.reg))
		return false;
	}

if (elt->except.spec && match_special_tok(node, elt->except.spec, nlppp,attop))
	return false;
return true;
}



/********************************************
* FN:		MATCH_LIST_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_list_deaccent(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);

bool attop = true;	// 07/08/03 AM.

// Note: shouldn't keep both hashed and unhashed in same rule elt.
if (elt->match.htab)	// USE HASHING IF AVAILABLE.					// 06/14/00 AM.
	{
	if (hash_str_nocase(dbuf, elt->match))
		goto matched;
	}
else if (elt->match.reg)
	{
	if (find_str_nocase(dbuf, elt->match.reg))
		goto matched;
	}

if (!elt->match.spec
			|| !match_special_tok(node, elt->match.spec, nlppp,		// 06/14/00 AM.
								attop)) // 07/08/03 AM.
	{
	Chars::destroy(dbuf);
	return false;
	}

matched:

if (elt->except.htab)
	{
	if (hash_str_nocase(dbuf, elt->except))
		{
		Chars::destroy(dbuf);
		return false;
		}
	}
else if (elt->except.reg)
	{
	if (find_str_nocase(dbuf, elt->except.reg))
		{
		Chars::destroy(dbuf);
		return false;
		}
	}

Chars::destroy(dbuf);

if (elt->except.spec && match_special_tok(node, elt->except.spec, nlppp,attop))
	return false;
return true;
}


/********************************************
* FN:		MATCH_FAILS
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt with FAILS list.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_fails(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *name = pn->name_;
bool attop = true;	// 07/08/03 AM.

if (elt->fail.htab)
	{
	if (hash_str_nocase(name, elt->fail))
		goto failed;
	}
else if (elt->fail.reg)
	{
	if (find_str_nocase(name, elt->fail.reg))
		goto failed;
	}

if (!elt->fail.spec
			|| !match_special_tok(node, elt->fail.spec, nlppp,		// 06/14/00 AM.
											attop))	// 07/08/03 AM.
	return true;

failed:

if (elt->except.htab)
	{
	if (hash_str_nocase(name, elt->except))
		return true;
	}
else if (elt->except.reg)
	{
	if (find_str_nocase(name, elt->except.reg))
		return true;
	}

if (elt->except.spec && match_special_tok(node, elt->except.spec, nlppp,
										attop))	// 07/08/03 AM.
	return true;
return false;
}


/********************************************
* FN:		MATCH_FAILS_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt with FAILS list.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_fails_deaccent(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);

bool attop = true;	// 07/08/03 AM.

if (elt->fail.htab)
	{
	if (hash_str_nocase(dbuf, elt->fail))
		goto failed;
	}
else if (elt->fail.reg)
	{
	if (find_str_nocase(dbuf, elt->fail.reg))
		goto failed;
	}

if (!elt->fail.spec
			|| !match_special_tok(node, elt->fail.spec, nlppp,		// 06/14/00 AM.
											attop))	// 07/08/03 AM.
	{
	Chars::destroy(dbuf);
	return true;
	}

failed:

if (elt->except.htab)
	{
	if (hash_str_nocase(dbuf, elt->except))
		{
		Chars::destroy(dbuf);
		return true;
		}
	}
else if (elt->except.reg)
	{
	if (find_str_nocase(dbuf, elt->except.reg))
		{
		Chars::destroy(dbuf);
		return true;
		}
	}

Chars::destroy(dbuf);

if (elt->except.spec && match_special_tok(node, elt->except.spec, nlppp,
										attop))	// 07/08/03 AM.
	return true;
return false;
}


/********************************************
* FN:		MATCH_LIST_TREE
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_list_tree(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
if (!match_tree_list(node,elt->match,true,nlppp))
	return false;

if (!elt->except.htab && !elt->except.reg && !elt->except.spec)
	return true;
return !match_tree_list(node,elt->except,true,nlppp);
}



/********************************************
* FN:		MATCH_LIST_TREE_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_list_tree_deaccent(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
if (!match_tree_list_deaccent(node,elt->match,true,nlppp))
	return false;

if (!elt->except.htab && !elt->except.reg && !elt->except.spec)
	return true;
return !match_tree_list_deaccent(node,elt->except,true,nlppp);
}

/********************************************
* FN:		MATCH_FAILS_TREE
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt with FAILS list.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_fails_tree(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
if (!match_tree_list(node,elt->fail,true,nlppp))
	return true;
if (!elt->except.htab && !elt->except.reg && !elt->except.spec)
	return false;
if (match_tree_list(node,elt->except,true,nlppp))
	return true;
return false;

}


/********************************************
* FN:		MATCH_FAILS_TREE_DEACCENT
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt with FAILS list.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_fails_tree_deaccent(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
if (!match_tree_list_deaccent(node,elt->fail,true,nlppp))
	return true;
if (!elt->except.htab && !elt->except.reg && !elt->except.spec)
	return false;
if (match_tree_list_deaccent(node,elt->except,true,nlppp))
	return true;
return false;

}


/********************************************
* FN:		MATCH_LIST_SINGLET
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_list_singlet(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn;
bool success = false;
bool attop = true;	// for _xSTART,_xEND,_xEOF		// FIX.	// 07/08/03 AM.
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (elt->match.htab)
		{
		if (success = hash_str_nocase(pn->name_, elt->match))
			break;
		}
	else if (elt->match.reg)
		{
		if (success = find_str_nocase(pn->name_, elt->match.reg))
			break;
		}

	if (elt->match.spec &&
				(success = match_special_tok(nd, elt->match.spec, nlppp,
										/*UP*/attop)))		// FIX	// 07/08/03 AM.
																		// FIX.	// 08/09/02 AM.
		break;
	attop = false;		// 07/08/03 AM.

	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return false;
	}

if (!success)			// Didn't match.
	return false;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.htab && !elt->except.reg && !elt->except.spec)// OPT	// 06/07/00 AM.
	return true;													// OPT	// 06/07/00 AM.
attop = true;	// 07/08/03 AM.
nd = (Node<Pn> *) node;											// FIX	// 07/05/06 AM.
//for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
while (nd)															// FIX	// 07/05/06 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (elt->except.htab)
		{
		if (hash_str_nocase(pn->name_, elt->except))
			return false;
		}
	else if (elt->except.reg)
		{
		if (find_str_nocase(pn->name_, elt->except.reg))
			return false;
		}
	if (elt->except.spec && match_special_tok(nd, elt->except.spec, nlppp,
										attop))	// 07/08/03 AM.
		return false;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return true;
	nd = nd->pDown;			// Keep going down.			// FIX	// 07/05/06 AM.
	if (nd && nd->pRight)										// FIX	// 07/05/06 AM.
		return true;															// 07/05/06 AM.
	}
return true;
}


/********************************************
* FN:		MATCH_LIST_SINGLET_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_list_singlet_deaccent(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn;
bool success = false;
long len;
_TCHAR *dbuf;
_TCHAR *nname;
bool attop = true;	// for _xSTART,_xEND,_xEOF		// FIX.	// 07/08/03 AM.
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	nname = pn->name_;
	len = _tcsclen(nname);
	dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);

	if (elt->match.htab)
		{
		if (success = hash_str_nocase(dbuf, elt->match))
			{
			Chars::destroy(dbuf);
			break;
			}
		}
	else if (elt->match.reg)
		{
		if (success = find_str_nocase(dbuf, elt->match.reg))
			{
			Chars::destroy(dbuf);
			break;
			}
		}
	Chars::destroy(dbuf);

	if (elt->match.spec &&
				(success = match_special_tok(nd, elt->match.spec, nlppp,
										/*UP*/attop)))		// FIX	// 07/08/03 AM.
																		// FIX.	// 08/09/02 AM.
		break;
	attop = false;		// 07/08/03 AM.

	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return false;
	}

if (!success)			// Didn't match.
	return false;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.htab && !elt->except.reg && !elt->except.spec)// OPT	// 06/07/00 AM.
	return true;													// OPT	// 06/07/00 AM.
attop = true;	// 07/08/03 AM.
nd = (Node<Pn> *) node;											// FIX	// 07/05/06 AM.
//for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
while (nd)															// FIX	// 07/05/06 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	nname = pn->name_;
	len = _tcsclen(nname);
	dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);

	if (elt->except.htab)
		{
		if (hash_str_nocase(dbuf, elt->except))
			{
			Chars::destroy(dbuf);
			return false;
			}
		}
	else if (elt->except.reg)
		{
		if (find_str_nocase(dbuf, elt->except.reg))
			{
			Chars::destroy(dbuf);
			return false;
			}
		}
	Chars::destroy(dbuf);
	if (elt->except.spec && match_special_tok(nd, elt->except.spec, nlppp,
										attop))	// 07/08/03 AM.
		return false;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return true;
	nd = nd->pDown;			// Keep going down.			// FIX	// 07/05/06 AM.
	if (nd && nd->pRight)										// FIX	// 07/05/06 AM.
		return true;															// 07/05/06 AM.
	}
return true;
}


/********************************************
* FN:		MATCH_FAILS_SINGLET
* CR:		05/23/00 AM.
* SUBJ:	Runtime match of elt with FAILS list.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_fails_singlet(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn;
bool success = false;
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
bool attop = true;	// 07/08/03 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (elt->fail.htab)
		{
		if (success = hash_str_nocase(pn->name_, elt->fail))
			break;
		}
	else if (elt->fail.reg)
		{
		if (success = find_str_nocase(pn->name_, elt->fail.reg))
			break;
		}
	if (elt->fail.spec &&										// FIX.	// 06/14/00 AM.
				(success = match_special_tok(nd, elt->fail.spec, nlppp,
									attop)))	// 07/08/03 AM.
																		// FIX.	// 08/09/02 AM.
		break;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return true;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return true;
	}
if (!success)			// Didn't match.
	return true;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.htab && !elt->except.reg && !elt->except.spec)// OPT	// 06/07/00 AM.
	return false;													// OPT	// 06/07/00 AM.
attop = true;	// 07/08/03 AM.
nd = (Node<Pn> *) node;											// FIX	// 07/05/06 AM.
//for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
while (nd)															// FIX	// 07/05/06 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (elt->except.htab)
		{
		if (hash_str_nocase(pn->name_, elt->except))
			return true;
		}
	else if (elt->except.reg)
		{
		if (find_str_nocase(pn->name_, elt->except.reg))
			return true;
		}
	if (elt->except.spec && match_special_tok(nd, elt->except.spec, nlppp,
								attop))	// 07/08/03 AM.
		return true;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 07/05/06 AM.
	if (nd && nd->pRight)										// FIX	// 07/05/06 AM.
		return false;															// 07/05/06 AM.
	}
return false;
}


/********************************************
* FN:		MATCH_FAILS_SINGLET_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt with FAILS list.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_fails_singlet_deaccent(NODE *node, const ELT *elt, 
	Nlppp *nlppp
	)
{
Pn *pn;
bool success = false;
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
long len;
_TCHAR *dbuf;
_TCHAR *nname;

bool attop = true;	// 07/08/03 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	nname = pn->name_;
	len = _tcsclen(nname);
	dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);

	if (elt->fail.htab)
		{
		if (success = hash_str_nocase(dbuf, elt->fail))
			{
			Chars::destroy(dbuf);
			break;
			}
		}
	else if (elt->fail.reg)
		{
		if (success = find_str_nocase(dbuf, elt->fail.reg))
			{
			Chars::destroy(dbuf);
			break;
			}
		}
	Chars::destroy(dbuf);
	if (elt->fail.spec &&										// FIX.	// 06/14/00 AM.
				(success = match_special_tok(nd, elt->fail.spec, nlppp,
									attop)))	// 07/08/03 AM.
																		// FIX.	// 08/09/02 AM.
		break;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return true;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return true;
	}
if (!success)			// Didn't match.
	return true;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.htab && !elt->except.reg && !elt->except.spec)// OPT	// 06/07/00 AM.
	return false;													// OPT	// 06/07/00 AM.
attop = true;	// 07/08/03 AM.
nd = (Node<Pn> *) node;											// FIX	// 07/05/06 AM.
//for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
while (nd)															// FIX	// 07/05/06 AM.
	{
	pn = &(((Node<Pn> *)nd)->data);
	nname = pn->name_;
	len = _tcsclen(nname);
	dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);

	if (elt->except.htab)
		{
		if (hash_str_nocase(dbuf, elt->except))
			{
			Chars::destroy(dbuf);
			return true;
			}
		}
	else if (elt->except.reg)
		{
		if (find_str_nocase(dbuf, elt->except.reg))
			{
			Chars::destroy(dbuf);
			return true;
			}
		}
	Chars::destroy(dbuf);
	if (elt->except.spec && match_special_tok(nd, elt->except.spec, nlppp,
								attop))	// 07/08/03 AM.
		return true;
	attop = false;	// 07/08/03 AM.
	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 07/05/06 AM.
	if (nd && nd->pRight)										// FIX	// 07/05/06 AM.
		return false;															// 07/05/06 AM.
	}
return false;
}


/********************************************
* FN:		FIND_SPECIAL_TOK
* CR:		08/10/02 AM.
* SUBJ:	See if given special tok is present.
********************************************/

bool Arun::find_special_tok(
   enum specialType tok,
	const int *arr
	)
{
if (!arr)
	return false;
const int *ptr;
for (ptr = arr; *ptr; ++ptr)
	{
	if (tok == (enum specialType) *ptr)
		return true;
	}
return false;
}


/********************************************
* FN:		MATCH_SPECIAL_TOK
* CR:		06/09/00 AM.
* SUBJ:	Match against array of special tokens.
* NOTE:	Nonwild, nonlist, nonspecial.
********************************************/

bool Arun::match_special_tok(
	NODE *node,
	const int *arr, 
	Nlppp *nlppp,
	bool attop	// 07/08/03 AM.
	)
{
const int *ptr;
for (ptr = arr; *ptr; ++ptr)
	{
	switch ((enum specialType) *ptr)
		{
		case XNULL:
			break;
		case XALPHA:	// _xALPHA
			if (micro_alpha(node, 0, nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XANY:		// _xANY
			if (node)													// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XBLANK:	// _xBLANK
			if (micro_blank(node,0,nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XCAP:		// _xCAP
			if (micro_cap(node, 0, nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XEND:		// _xEND
			if (!attop)													// FIX.	// 07/08/03 AM.
				return false;											// FIX.	// 07/08/03 AM.
			if (micro_end(node, nlppp))							// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XEOF:		// _xEOF
			if (!attop)													// FIX.	// 07/08/03 AM.
				return false;											// FIX.	// 07/08/03 AM.
			if (micro_eof(node, nlppp))							// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XFEAT:	// _xFEAT [unimplemented]
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: _xFEAT unimplemented, Ignoring.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}
			break;
		case XNIL:		// _xNIL
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: _xNIL in a match list. Ignoring.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}
			break;
		case XNUM:		// _xNUM
			if (micro_num(node, 0, nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XPUNCT:	// _xPUNCT
			if (micro_punct(node, 0, nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XCTRL:	// _xCTRL													// 07/24/00 AM.
			if (micro_ctrl(node, 0, nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XSTART:	// _xSTART
#ifdef OLD_031231
// Problem is we're not flagging "don't advance to next node".	// 12/31/03 AM.
// _xSTART in list is taken care of by special code generation.	// 12/31/03 AM.
			if (!attop)													// FIX.	// 07/08/03 AM.
				return false;											// FIX.	// 07/08/03 AM.
			if (micro_start(node, nlppp))							// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
#endif
			break;
		case XWHITE:	// _xWHITE
			if (micro_white(node, 0, nlppp))						// FIX.	// 08/09/02 AM.
				return true;											// FIX.	// 08/09/02 AM.
			break;
		case XWILD:	// _xWILD
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: _xWILD in a match list.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}

			return false;
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad special token in match_special_tok.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}

			return false;
		}
	}
return false;
}

/******************************************************************************/
// SPECIAL MODE FUNCTIONS FOR PATTERN MATCHER
/******************************************************************************/


/********************************************
* FN:		SPECIAL_LEFT
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	05/30/00 AM. Copying special_right.
********************************************/

bool Arun::special_left(
	int ord,
	bool (match)(NODE *, const ELT *,MICROFN,Nlppp *),
	MICROFN micro,
	Nlppp *nlppp
	)
{
nlppp->ord_ = ord;
Node<Pn> *save = nlppp->node_;
COLL *coll;
const ELT *elt = &(nlppp->elts_[ord]);

if (!nlppp->node_)	// No more nodes in current context.
	{
	if (!elt->min)			// Optional
		{
		// Update status.
		coll = &(nlppp->coll_[ord]);
		coll->start = coll->end = 0;
		coll->totnodes = 0;
		return true;
		}
	return false;
	}

if (nlppp->wildord_)				// If pending wildcards.
	{
	// Assume we're one past the minimum constraint.
	int count = nlppp->wildtot_;
	int limit = nlppp->wildmax_;
	bool matched = false;
	for (;;)
		{
		if (matched = special_real_left(match,micro,nlppp))
			break;
		++count;
		if (limit && count > limit)
			break;
		if (!(nlppp->node_ = Pat::nodeLeft(nlppp->node_, nlppp->start_)))
			break;
		}

	if (matched)						// Matched
		{
		nlppp->wildtot_ = count;			// New number of nodes in wild range.
		// Backfill wildcards.
		if (nlppp->wildord_)
			if (!Pat::wild_backfill_left(nlppp))
				return false;

		// Update state of rule match.
		coll = &(nlppp->coll_[ord]);
		nlppp->node_ = nlppp->first_ = (Node<Pn> *) coll->start;

		// Try to move left one node.
		nlppp->node_ = Pat::nodeLeft(nlppp->first_, nlppp->start_);
		return true;
		}
	else if (!elt->min)					// Optional
		{
		nlppp->node_ = save;		// Restore position.
		return true;
		}
	return false;	// Failure.
	}

// Match a real node in parse tree.
if (!special_real_left(match,micro,nlppp))
	{
	if (!elt->min)		// Optional
		{
		nlppp->node_ = save;		// Restore.
		return true;
		}
	return false;
	}

// Match succeeded.  Get data from collect.
coll = &(nlppp->coll_[ord]);

// Update status.
nlppp->node_ = nlppp->first_ = (Node<Pn> *) coll->start;

if (!nlppp->last_)
	nlppp->last_ = (Node<Pn> *) coll->end;

// Try to move left one node.
nlppp->node_ = Pat::nodeLeft(nlppp->first_, nlppp->start_);
return true;
}


/********************************************
* FN:		SPECIAL_RIGHT
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/

bool Arun::special_right(
	int ord,
	bool (match)(NODE *, const ELT *,MICROFN,Nlppp *),
	MICROFN micro,
	Nlppp *nlppp
	)
{
nlppp->ord_ = ord;
Node<Pn> *save = nlppp->node_;
COLL *coll;
const ELT *elt = &(nlppp->elts_[ord]);
Node<Pn> *saverightmost = 0;			// RECOPT2.	// 07/25/06 AM.

if (!nlppp->node_)	// No more nodes in current context.
	{
	if (!elt->min)			// Optional
		{
		// Update status.
		coll = &(nlppp->coll_[ord]);
		coll->start = coll->end = 0;
		coll->totnodes = 0;
		return true;
		}
	return false;
	}

if (nlppp->wildord_)				// If pending wildcards.
	{
	// Assume we're one past the minimum constraint.
	int count = nlppp->wildtot_;
	int limit = nlppp->wildmax_;
	bool matched = false;

	for (;;)
		{
		if (matched = special_real_right(match,micro,nlppp))
			break;
		++count;
		if (limit && count > limit)
			break;
		if (!(nlppp->node_ = Pat::nodeRight(nlppp->node_, nlppp->end_)))
			break;
		Pat::updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/25/06 AM.
		// Need to back up if failed repeat match.// RECOPT2.	// 07/25/06 AM.
		if (nlppp->rmost_)	// Curr rule.			// RECOPT2.	// 07/25/06 AM.
			saverightmost = nlppp->rmost_;			// RECOPT2.	// 07/25/06 AM.
		}

	if (matched)						// Matched
		{
		nlppp->wildtot_ = count;			// New number of nodes in wild range.
		// Backfill wildcards.
		if (nlppp->wildord_)
			if (!Pat::wild_backfill_right(nlppp))
				{
				if (saverightmost)								// RECOPT2.	// 07/25/06 AM.
					{
					nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/25/06 AM.
					nlppp->rmost_ = 0;							// RECOPT2.	// 07/25/06 AM.
					}
				return false;
				}

		// Update state of rule match.
		coll = &(nlppp->coll_[ord]);
		nlppp->node_ = nlppp->last_ = (Node<Pn> *) coll->end;

		// Try to move right one node.
		nlppp->node_ = Pat::nodeRight(nlppp->last_, nlppp->end_);
		Pat::updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/25/06 AM.
		return true;
		}

	if (saverightmost)								// RECOPT2.	// 07/25/06 AM.
		{
		nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/25/06 AM.
		nlppp->rmost_ = 0;							// RECOPT2.	// 07/25/06 AM.
		}

	if (!elt->min)					// Optional
		{
		nlppp->node_ = save;		// Restore position.
		return true;
		}
	return false;	// Failure.
	}

// Match a real node in parse tree.
if (!special_real_right(match,micro,nlppp))
	{
	if (!elt->min)		// Optional
		{
		nlppp->node_ = save;		// Restore.
		return true;
		}
	return false;
	}

// Match succeeded.  Get data from collect.
coll = &(nlppp->coll_[ord]);

// Update status.
nlppp->node_ = nlppp->last_ = (Node<Pn> *) coll->end;

if (!nlppp->first_)
	nlppp->first_ = (Node<Pn> *) coll->start;

// Try to move right one node.
nlppp->node_ = Pat::nodeRight(nlppp->last_, nlppp->end_);
Pat::updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/25/06 AM.
return true;
}


/********************************************
* FN:		SPECIAL_REAL_LEFT
* CR:		05/30/00 AM.
* SUBJ:	Runtime match of elt to a real node.
********************************************/

bool Arun::special_real_left(
	bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
	MICROFN micro,
	Nlppp *nlppp
	)
{
Node<Pn> *node = nlppp->node_;
Pn *pn = &(node->data);
const ELT *elt = &(nlppp->elts_[nlppp->ord_]);
_TCHAR *name = elt->name;
int min = elt->min;
int max = elt->max;

if (!(match)(node, elt,micro, nlppp))			// The MATCH FN.
	return false;
if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))
	return false;

int count = 1;									// Count number of nodes matched.
Node<Pn> *from = node;
Node<Pn> *to = node;

// Repetition.  Match as greedily as possible.
if (!max)					// Indefinite repetition.
	max = INT_MAX;

Node<Pn> *save = node;		// Save current position.
while (count < max)
	{
	if (!(nlppp->node_ = Pat::nodeLeft(nlppp->node_, nlppp->start_)))
		break;
	pn = &(nlppp->node_->data);
	if (!(match)(nlppp->node_, elt,micro, nlppp))	// The MATCH FN.
		break;				// Failed to match.
	if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))
		break;				// Failed on pre actions.
	// Matched.
	++count;
	to = nlppp->node_;
	}

if (count < min)
	{
	nlppp->node_ = save;			// Restore node position.
	return false;
	}

// Update data structures.
COLL *curr_coll = &(nlppp->coll_[nlppp->ord_]);
curr_coll->start = to;
curr_coll->end   = from;
curr_coll->totnodes = count;

return true;
}


/********************************************
* FN:		SPECIAL_REAL_RIGHT
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt to a real node.
********************************************/

bool Arun::special_real_right(
	bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
	MICROFN micro,
	Nlppp *nlppp
	)
{
Node<Pn> *node = nlppp->node_;
Pn *pn = &(node->data);
const ELT *elt = &(nlppp->elts_[nlppp->ord_]);
_TCHAR *name = elt->name;
int min = elt->min;
int max = elt->max;
Node<Pn> *saverightmost = 0;							// RECOPT2.	// 07/25/06 AM.

if (!(match)(node, elt,micro, nlppp))			// The MATCH FN.
	return false;
if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))
	return false;

int count = 1;									// Count number of nodes matched.
Node<Pn> *from = node;
Node<Pn> *to = node;

if (!nlppp->first_)											// RECOPT2.	// 07/25/06 AM.
	nlppp->first_ = node;									// RECOPT2.	// 07/25/06 AM.

// Repetition.  Match as greedily as possible.
if (!max)					// Indefinite repetition.
	max = INT_MAX;

Node<Pn> *save = node;		// Save current position.
while (count < max)
	{
	if (!(nlppp->node_ = Pat::nodeRight(nlppp->node_, nlppp->end_)))
		break;
	pn = &(nlppp->node_->data);
	Pat::updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/25/06 AM.
	// Need to back up if failed repeat match.// RECOPT2.	// 07/25/06 AM.
	if (nlppp->rmost_)	// Curr rule.			// RECOPT2.	// 07/25/06 AM.
		saverightmost = nlppp->rmost_;			// RECOPT2.	// 07/25/06 AM.

	if (!(match)(nlppp->node_, elt,micro, nlppp))	// The MATCH FN.
		break;				// Failed to match.
	if (nlppp->prefn_ && !(nlppp->prefn_)(nlppp->ord_,nlppp))
		break;				// Failed on pre actions.
	// Matched.

	// Need this for tracking in RECOPT2.						// 07/25/06 AM.
	if (!nlppp->first_)												// 07/25/06 AM.
		nlppp->first_ = nlppp->last_ = nlppp->node_;			// 07/25/06 AM.

	++count;
	to = nlppp->node_;
	}

if (count < min)
	{
	nlppp->node_ = save;			// Restore node position.
	if (saverightmost)								// RECOPT2.	// 07/25/06 AM.
		{
		nlppp->rightmost_ = saverightmost;		// RECOPT2.	// 07/25/06 AM.
		nlppp->rmost_ = 0;							// RECOPT2.	// 07/25/06 AM.
		}
	return false;
	}

// Update data structures.
COLL *curr_coll = &(nlppp->coll_[nlppp->ord_]);
curr_coll->start = from;
curr_coll->end   = to;
curr_coll->totnodes = count;

return true;
}

/********************************************
* FN:		SPECIAL_SIMPLE
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial, nonsinglet/nontree.
*			Match at least one real node or fail.
*			Should probably be in pat.cpp.
********************************************/

bool Arun::special_simple(
	NODE *node,
	const ELT *elt,
	MICROFN match,
	Nlppp *nlppp
	)
{
if (!(match)(node,elt,nlppp))
	return false;

// Matched, so check exception list, if any.
Pn *pn = &(((Node<Pn> *)node)->data);

if (elt->except.reg && find_str_nocase(pn->name_, elt->except.reg))
	return false;

bool attop = true;	// 07/08/03 AM.
if (elt->except.spec && match_special_tok(node, elt->except.spec, nlppp,
								attop))
	return false;
return true;
}


/********************************************
* FN:		SPECIAL_SIMPLE_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	Nonwild, nonlist, nonspecial, nonsinglet/nontree.
*			Match at least one real node or fail.
*			Should probably be in pat.cpp.
********************************************/

bool Arun::special_simple_deaccent(
	NODE *node,
	const ELT *elt,
	MICROFN match,
	Nlppp *nlppp
	)
{
if (!(match)(node,elt,nlppp))
	return false;

// Matched, so check exception list, if any.
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR *nname = pn->name_;
long len = _tcsclen(nname);
_TCHAR *dbuf = Chars::create(len + 2);
Xml::de_accent(nname,/*DU*/dbuf);

if (elt->except.reg && find_str_nocase(dbuf, elt->except.reg))
	{
	Chars::destroy(dbuf);
	return false;
	}
Chars::destroy(dbuf);

bool attop = true;	// 07/08/03 AM.
if (elt->except.spec && match_special_tok(node, elt->except.spec, nlppp,
								attop))
	return false;
return true;
}

/********************************************
* FN:		SPECIAL_SINGLET
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	There are several ways to apply the match and the except
*			list.  This fn doesn't deal with match list.
********************************************/

bool Arun::special_singlet(
	NODE *node,
	const ELT *elt,
	MICROFN match,
	Nlppp *nlppp
	)
{
Pn *pn;
bool success = false;
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	if (success = (match)(nd,elt,nlppp))	// MATCH.
		break;
	pn = &(((Node<Pn> *)nd)->data);
	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return false;
	}
if (!success)			// Didn't match.
	return false;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.reg && !elt->except.spec)						// OPT	// 06/07/00 AM.
	return true;													// OPT	// 06/07/00 AM.
bool attop = true;	// 07/08/03 AM.
for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
	{
	pn = &(((Node<Pn> *)nd)->data);
	if (elt->except.reg && find_str_nocase(pn->name_, elt->except.reg))
		return false;
	if (elt->except.spec && match_special_tok(nd,elt->except.spec,nlppp,
							attop))	// 07/08/03 AM.
		return false;
	if (pn->getBase())
		return true;
	}
return true;
}


/********************************************
* FN:		SPECIAL_SINGLET_DEACCENT
* CR:		03/29/05 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	There are several ways to apply the match and the except
*			list.  This fn doesn't deal with match list.
********************************************/

bool Arun::special_singlet_deaccent(
	NODE *node,
	const ELT *elt,
	MICROFN match,
	Nlppp *nlppp
	)
{
Pn *pn;
bool success = false;
Node<Pn> *nd = (Node<Pn> *) node;							// FIX	// 06/07/00 AM.
while (nd)															// FIX	// 06/07/00 AM.
	{
	if (success = (match)(nd,elt,nlppp))	// MATCH.
		break;
	pn = &(((Node<Pn> *)nd)->data);
	if (pn->getBase())
		return false;
	nd = nd->pDown;			// Keep going down.			// FIX	// 06/07/00 AM.
	if (nd && nd->pRight)										// FIX	// 06/07/00 AM.
		return false;
	}
if (!success)			// Didn't match.
	return false;

// Matched.  Now see if any of the nodes are on the exception list.
// Opt: Loop again now, rather than putting except test in prior loop.
// Assumption is that matching usually fails, so the loop below won't
// be tried too often.  Also, singlet chains assumed to be short, on
// average.
if (!elt->except.reg && !elt->except.spec)						// OPT	// 06/07/00 AM.
	return true;													// OPT	// 06/07/00 AM.
bool attop = true;	// 07/08/03 AM.
for (nd = (Node<Pn> *)node; nd; nd = ((Node<Pn> *)nd)->Down())
	{
	pn = &(((Node<Pn> *)nd)->data);
	_TCHAR *nname = pn->name_;
	long len = _tcsclen(nname);
	_TCHAR *dbuf = Chars::create(len + 2);
	Xml::de_accent(nname,/*DU*/dbuf);
	if (elt->except.reg && find_str_nocase(dbuf, elt->except.reg))
		{
		Chars::destroy(dbuf);
		return false;
		}
	Chars::destroy(dbuf);
	if (elt->except.spec && match_special_tok(nd,elt->except.spec,nlppp,
							attop))	// 07/08/03 AM.
		return false;
	if (pn->getBase())
		return true;
	}
return true;
}

/********************************************
* FN:		SPECIAL_TREE
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/

bool Arun::special_tree(
	NODE *node,
	const ELT *elt,
	MICROFN match,
	Nlppp *nlppp
	)
{
if (!special_tree(node, elt, true, match, nlppp))
	return false;
if (elt->except.reg
	 && match_tree_list(node,elt->except,true,nlppp))
	return false;
bool attop = true;	// 07/08/03 AM.
if (elt->except.spec && match_special_tok(node, elt->except.spec,nlppp,
							attop))		// 07/08/03 AM.
	return false;
return true;
}

// Recursive workhorse.
bool Arun::special_tree(
	NODE *node,
	const ELT *elt,
	bool root,
	MICROFN match,
	Nlppp *nlppp
	)
{
if (!node)
	return false;

if ((match)(node,elt,nlppp))
	return true;

if (special_tree(
	((Node<Pn> *)node)->pDown, elt, false, match, nlppp))	// Recurse down.
	return true;

// Manage list of nodes, if I'm the first in the list. (Minimizes recursion).
if (!root && !(((Node<Pn> *)node)->pLeft))
	{
	for (node = ((Node<Pn> *)node)->Right();
		  node;
		  node = ((Node<Pn> *)node)->Right())
		{
		if (special_tree(node,elt,false,match,nlppp))
			return true;
		}
	}
return false;
}


/********************************************
* FN:		MATCH_START
* CR:		05/11/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/


bool Arun::match_start(int ord, Nlppp *nlppp)
{
if (ord != 1)
	return false;		// Compile time error, really.

// If triggered and fell off left edge, sounds ok.		// FIX.	// 06/07/00 AM.
if (!nlppp->node_)												// FIX.	// 06/07/00 AM.
	return true;													// FIX.	// 06/07/00 AM.

if (nlppp->node_->pLeft)
	return false;
return true;
}


/********************************************
* FN:		MATCH_END
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/


bool Arun::match_end(int ord, Nlppp *nlppp)
{
if (ord != nlppp->len_)
	return false;		// Compile time error, really.
if (!nlppp->node_)	// FIX.												// 06/08/00 AM.
	return true;		// If fell of right side of phrase, then met condition.

// BUG: If we're at a real node, the _xEND is FALSE.				// 08/09/02 AM.
return false;														// FIX.	// 08/09/02 AM.

//if (nlppp->node_->pRight)									// BUG.	// 08/09/02 AM.
//	return false;													// BUG.	// 08/09/02 AM.
//return true;														// BUG.	// 08/09/02 AM.
}


/********************************************
* FN:		MATCH_EOF
* CR:		05/25/00 AM.
* SUBJ:	Runtime match of elt.
********************************************/


bool Arun::match_eof(int ord, Nlppp *nlppp)
{
if (ord != nlppp->len_)
	return false;		// Compile time error, really.

if (!nlppp->last_)															// 04/28/01 AM.
	return false;																// 04/28/01 AM.

// Should use the last node matched, not the next node.			// 04/28/01 AM.
//if (!Tree<Pn>::rightmostNode(nlppp->node_))			// BUG.	// 04/28/01 AM.
if (!Tree<Pn>::rightmostNode(nlppp->last_))				// FIX.	// 04/28/01 AM.
	return false;

return true;
}

/********************************************
* FN:		MATCH_LIST_EOF
* CR:		08/10/02 AM.
* SUBJ:	See if EOF condition is matched.
* NOTE:	Assumes we're at the end of a segment/phrase,
*			and checking for _xEOF in special match list.
********************************************/

bool Arun::match_list_eof(NODE *last, const ELT *elt)
{
if (!find_special_tok(XEOF,elt->match.spec))
	return false;

if (!last)			// If we fell of the tree.
	return true;	// Assume developer set this up right.

// Now, make sure we're at end of file.
if (!Tree<Pn>::rightmostNode((Node<Pn> *)last))
	{
	// If not the rightmost node in the parse tree, we're not at end
	// of file.
	return false;
	}

return true;	// At end of file.
}


/********************************************
* FN:		MICRO_ANY
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_any(NODE *node, const ELT *elt, Nlppp *nlppp)
{
return node ? true : false;
}


/********************************************
* FN:		MICRO_ALPHA
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_alpha(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
//return ((pn->getType() == PNALPHA) ? true : false);	// 07/20/04 AM.
return (alphabetic(*pn->getName()) ? true : false);	// 07/20/04 AM.
}


/********************************************
* FN:		MICRO_BLANK
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_blank(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
_TCHAR ch;
if ((ch = *(pn->getName())) == ' ' || ch == '\t')
	return true;
return false;
}


/********************************************
* FN:		MICRO_CAP
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_cap(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
return ((pn->getType() == PNALPHA)
	&& is_upper((_TUCHAR)*(pn->getName())) );					// 12/16/01 AM.
}


/********************************************
* FN:		MICRO_NUM
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_num(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
return ((pn->getType() == PNNUM) ? true : false);
}


/********************************************
* FN:		MICRO_PUNCT
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_punct(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
return ((pn->getType() == PNPUNCT) ? true : false);
}


/********************************************
* FN:		MICRO_CTRL
* CR:		07/24/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_ctrl(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
return ((pn->getType() == PNCTRL) ? true : false);
}


/********************************************
* FN:		MICRO_WHITE
* CR:		05/25/00 AM.
* SUBJ:	Micro match of node to rule elt.
********************************************/


bool Arun::micro_white(NODE *node, const ELT *elt, Nlppp *nlppp)
{
Pn *pn = &(((Node<Pn> *)node)->data);
return ((pn->getType() == PNWHITE) ? true : false);
}




/********************************************
* FN:		MICRO_START
* CR:		06/09/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	For when _xSTART is in a match list.
*			Will yield silly results if not used right.
********************************************/


bool Arun::micro_start(NODE *node, Nlppp *nlppp)
{
if (nlppp->ord_ != 1)
	return false;

// If triggered and fell off left edge, sounds ok.
if (!node)
	return true;

if (((Node<Pn>*)node)->pLeft)
	return false;
return true;
}


/********************************************
* FN:		MICRO_END
* CR:		06/09/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	When _xEND is in a match list.
********************************************/


bool Arun::micro_end(NODE *node, Nlppp *nlppp)
{
if (nlppp->ord_ != nlppp->len_)
	return false;		// Compile time error, really.
if (!node)
	return true;		// If fell of right side of phrase, then met condition.
//if (((Node<Pn> *)node)->pRight)							// BUG.	// 08/09/02 AM.
//	return false;													// BUG.	// 08/09/02 AM.
//return true;														// BUG.	// 08/09/02 AM.
return false;														// FIX.	// 08/09/02 AM.
}


/********************************************
* FN:		MICRO_EOF
* CR:		06/09/00 AM.
* SUBJ:	Runtime match of elt.
* NOTE:	When _xEOF is in a match list.
********************************************/

bool Arun::micro_eof(NODE *node, Nlppp *nlppp)
{
if (nlppp->ord_ != nlppp->len_)
	return false;		// Compile time error, really.

if (!Tree<Pn>::rightmostNode((Node<Pn> *)node))
	return false;

return true;
}

/******************************************************************************/
// UTILITIES FOR PATTERN MATCHER
/******************************************************************************/


/********************************************
* FN:		INIT_ANALYZER
* CR:		05/17/00 AM.
* SUBJ:	Initializations for current text analysis instance.
********************************************/

bool Arun::init_analyzer(
	Parse *parse
	)
{
if (!parse)
	return false;

// Set up some data structures.
// Should determine the max elements per rule.
parse->nlppp_ = new Nlppp();	// Empty pattern match control object.
parse->collect_ = new COLL [MAX_RULE_LENGTH];
parse->nlppp_->parse_ = parse;			// Backpointer.
parse->nlppp_->coll_ = parse->collect_;								// 06/01/00 AM.
return true;
}


/********************************************
* FN:		INIT_RULE
* CR:		05/17/00 AM.
* SUBJ:	Initializations for current rule.
* NOTE:	As needed.  For now, just setting up
*			for rules that have wildcards.
*			Really, the static rule information should be set up in a
*			hardwired compiled array. (eg, rule length, etc.)
********************************************/

void Arun::init_rule(
	int rulelen,				// Length of current rule phrase.
	const SUGG &sugg,			// Data for suggested concept.
	const ELT *elts,			// Array of elt data for rule.
	PREFN prefn,				// Gen'd fn for pre actions.			// 05/24/00 AM.
	int lookahead,				// Ord for lookahead, if any.			// 06/10/00 AM.
	Nlppp *nlppp
	)
{
nlppp->len_	 = rulelen;
nlppp->elts_ = elts;
nlppp->sugg_ = &sugg;														// 05/19/00 AM.
nlppp->prefn_ = prefn;														// 05/24/00 AM.
nlppp->nlookahead_ = lookahead;											// 06/10/00 AM.

// Clean out the collect.
reset_collect(nlppp->coll_, rulelen);

// RESET WILDCARD TRACKING.	// FIX.									// 05/25/00 AM.
if (nlppp->wildord_)
	{
	nlppp->wildord_	= 0;
	nlppp->wildmin_	= -1;
	nlppp->wildmax_	= -1;
	nlppp->wildmark_	= 0;
	}

}


/********************************************
* FN:		INIT_LOOKAHEAD
* CR:		06/10/00 AM.
* SUBJ:	Set node that matched lookahead rule element, if any.
* RET:	Lookahead node, if found.
* NOTE:	Compiled runtime variant of Pat::initLookahead.
*			Not using nlppp->lookahead_.
********************************************/

NODE *Arun::init_lookahead(Nlppp *nlppp)
{

int ord = nlppp->nlookahead_;
if (ord <= 0)
	return 0;
// Traverse the collect to position of lookahead rule element.
// Pick up first node found there, if any.
// If not found, traverse ahead to next available node in collect.
// (Latter case possible if lookahead elt is optional.)
return get_left_node(
	nlppp->coll_,
	ord,
	nlppp->len_
	);
}

/********************************************
* FN:		SWITCH_LEFT
* CR:		05/30/00 AM.
* SUBJ:	Switch left after matching the trigger.
********************************************/

void Arun::switch_left(
	Nlppp *nlppp
	)
{
Node<Pn> *node;
if (node = nlppp->first_)		// There is a first matched node.
	{
	nlppp->node_ = Pat::nodeLeft(node, nlppp->start_);
	return;
	}

// Else, stay at current node.
}

/********************************************
* FN:		SWITCH_RIGHT
* CR:		05/30/00 AM.
* SUBJ:	Switch right after matching to left of trigger.
********************************************/

void Arun::switch_right(
	Nlppp *nlppp
	)
{
Node<Pn> *node;
if (node = nlppp->last_)		// There is a last matched node.
	{
	nlppp->node_ = Pat::nodeRight(node, nlppp->end_);
	Pat::updateRestart(nlppp,nlppp->node_);	// RECOPT2	// 07/25/06 AM.
	return;
	}

// Else, stay at current node.
//nlppp->node_ = Pat::nodeRight(nlppp->start_, nlppp->end_);
}

/********************************************
* FN:		SET_SPECIALARR_LEN
* CR:		06/09/00 AM.
* SUBJ:	Set length of special token array.
* NOTE:	Should be done in some global place, like program startup, one time.
********************************************/

enum specialType Arun::set_specialarr_len()
{
int ii = 0;
_TCHAR **ptr = specialArr;
// Note: slough 0th element of array.
while (*++ptr)
	++ii;
return (enum specialType) (SPECIALARR_LEN = ii);
}


/********************************************
* FN:		FIND_SPECIAL
* CR:		06/09/00 AM.
* SUBJ:	Find special token in list.
* RET:	Return its internal token in array of special tokens.
********************************************/

enum specialType Arun::find_special(
	_TCHAR *str
	)
{
if (!str || !*str)
	return XNULL;
return (enum specialType)
	binary_search(str, specialArr, 1,
		SPECIALARR_LEN	// Global var determined once at program start....
		);
}


/********************************************
* FN:		GET_COLLS
* CR:		05/13/00 AM.
* SUBJ:	Fetch the collect list.
* NOTE:	OBSOLETE!
********************************************/

NODE *Arun::get_colls(
	Nlppp *nlppp
	)
{
//return (NODE *) (nlppp->coll_->getRoot());
return 0;
}


/********************************************
* FN:		NEXT_NODE
* CR:		05/13/00 AM.
* SUBJ:	Fetch the next node in a list.
********************************************/

NODE *Arun::next_node(
	NODE *node
	)
{
return (NODE *) (((Node<Pn> *) node)->Right());
}


/********************************************
* FN:		RESET_COLLECT
* CR:		05/18/00 AM.
* SUBJ:	Reset the collect array.
* NOTE:	Using 1-BASED numbering, so watch out.
*			For simplicity, IGNORE 0th element of collect array!
********************************************/
void Arun::reset_collect(
	COLL *coll,
	int len			// Length to clear out.
	)
{
++coll;				// ZEROTH ELT IS IGNORED.
while (--len >= 0)
	{
	coll->start = coll->end = 0;
	coll->totnodes = 0;
	++coll;
	}
}


/********************************************
* FN:		GET_LEFT_NODE
* CR:		05/25/00 AM.
* SUBJ:	Find the leftmost real node in a range.
* NOTE:	Needed because of optional rule elts that matched no nodes.
********************************************/

NODE *Arun::get_left_node(
	COLL *collect,
	int from,
	int to
	)
{
int ii;
for (ii = from, collect = &(collect[from]); ii <= to; ++ii, ++collect)
	{
	if (collect->start)
		return collect->start;
	}
return 0;
}


/********************************************
* FN:		GET_RIGHT_NODE
* CR:		05/25/00 AM.
* SUBJ:	Find the rightmost real node in a range.
* NOTE:	Needed because of optional rule elts that matched no nodes.
********************************************/

NODE *Arun::get_right_node(
	COLL *collect,
	int from,
	int to
	)
{
int ii;
for (ii = to, collect = &(collect[to]); ii >= from; --ii, --collect)
	{
	if (collect->end)
		return collect->end;
	}
return 0;
}

/******************************************************************************/
// PRE ACTIONS
/******************************************************************************/


/******************************************************************************/
// ACTIONS
/******************************************************************************/


/********************************************
* FN:		UPPERCASE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::uppercase(
	Nlppp *nlppp
	)
{
bool ok = true;

Pn *pn = Pat::leafNode(nlppp->node_)->getData();

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
_TCHAR *buf;
buf = pn->pnStr();			// Create buffer for node's text.
ok = all_uppercase(buf);
Chars::destroy(buf);			// Free up buffer.

return ok;
}


/********************************************
* FN:		LOWERCASE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::lowercase(
	Nlppp *nlppp
	)
{
bool ok = true;

Pn *pn = Pat::leafNode(nlppp->node_)->getData();

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
_TCHAR *buf;
buf = pn->pnStr();			// Create buffer for node's text.
ok = all_lowercase(buf);
Chars::destroy(buf);			// Free up buffer.

return ok;
}


/********************************************
* FN:		CAP
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::cap(
	Nlppp *nlppp
	)
{
bool ok = true;

Pn *pn = Pat::leafNode(nlppp->node_)->getData();

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
_TCHAR *buf;
_TCHAR ch;

buf = pn->getName();
if (!(ch = *buf))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Cap pre action: Node with no text.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!alphabetic(ch))
	return false;				// NON ALPHABETIC FAILS.
if (is_upper((_TUCHAR)ch))											// 12/16/01 AM.
	return ok;
return false;
}


/********************************************
* FN:		UNKNOWN
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::unknown(
	Nlppp *nlppp
	)
{
bool ok = true;

Node<Pn> *leaf = Pat::leafNode(nlppp->node_);						// 06/05/00 AM.
if (leaf->pDown)				// Not a token node.						// 06/05/00 AM.
	return false;

Pn *pn = leaf->getData();

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
Sym *sym;
sym = pn->getlcSym();	// Get lowercased sym.

if (sym && sym->isLooked() && !(sym->isKnown()))
	return ok;
return false;
}


/********************************************
* FN:		LENGTH
* CR:		05/24/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::length(
	Nlppp *nlppp,
	int len
	)
{
// Get current node.
// Get its leaf.
// See if leaf has matching length.
return strlen_eq(Pat::leafNode(nlppp->node_)->getData()->getName(), len);
}


/********************************************
* FN:		LENGTHR
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::lengthr(
	Nlppp *nlppp,
	long len1,
	long len2
	)
{
if (len1 < 0 || len2 < 0 || (len1 > len2))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[LENGTHR pre action: Bad range (") << len1 << _T(",") << len2
		  << _T(")]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Pn *pn = Pat::leafNode(nlppp->node_)->getData();

long len = _tcsclen(pn->getName());
if ((len >= len1) && (len <= len2))
	return true;
return false;
}



/********************************************
* FN:		NUMRANGE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of pre action.
* OPT:	Could fetch the leaf node once for all its preactions.
********************************************/

bool Arun::numrange(
	Nlppp *nlppp,
	long len1,
	long len2
	)
{
if (len1 < 0 || len2 < 0 || (len1 > len2))				// FIX.	// 06/24/02 AM.
//							|| len2 > nlppp->len_				// FIX.	// 06/24/02 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[NUMRANGE pre action: Bad range (") << len1 << _T(",") << len2
		  << _T(")]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Pn *pn = Pat::leafNode(nlppp->node_)->getData();

long num = 0;
_TCHAR *str;
str = pn->getName();
if (!str_to_long(str, /*UP*/ num))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[NUMRANGE pre action: Bad num=") << str << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

if (num >= len1 && num <= len2)
	return true;			// Number is within range.
return false;
}

/********************************************
* FN:		VAR
* CR:		10/01/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::var(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
return Ivar::nodeVarNZ(nlppp->node_->getData(),str);
}
/********************************************
* FN:		VARZ
* CR:		10/01/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::varz(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
return !Ivar::nodeVarNZ(nlppp->node_->getData(),str);
}
/********************************************
* FN:		VAREQ
* CR:		10/01/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::vareq(
	Nlppp *nlppp,
	_TCHAR *str,
	_TCHAR *sval
	)
{
 return Ivar::nodeVarEQ(nlppp->node_->getData(),str,sval);
}
/********************************************
* FN:		VAREQ
* CR:		10/01/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::vareq(
	Nlppp *nlppp,
	_TCHAR *str,
	long nval
	)
{
 return Ivar::nodeVarEQ(nlppp->node_->getData(),str,nval);
}
/********************************************
* FN:		VARNE
* CR:		10/01/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::varne(
	Nlppp *nlppp,
	_TCHAR *str,
	_TCHAR *sval
	)
{
 return !Ivar::nodeVarEQ(nlppp->node_->getData(),str,sval);
}
/********************************************
* FN:		VARNE
* CR:		10/01/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::varne(
	Nlppp *nlppp,
	_TCHAR *str,
	long nval
	)
{
 return !Ivar::nodeVarEQ(nlppp->node_->getData(),str,nval);
}



/********************************************
* FN:		VAR
* CR:		10/04/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::var(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return false;
_TCHAR *str = sem_to_str(str_sem);
delete str_sem;
if (!str || !*str)
	return false;

return Ivar::nodeVarNZ(nlppp->node_->getData(),str);
}
/********************************************
* FN:		VARZ
* CR:		10/04/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::varz(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return false;
_TCHAR *str = sem_to_str(str_sem);
delete str_sem;
if (!str || !*str)
	return false;

return !Ivar::nodeVarNZ(nlppp->node_->getData(),str);
}
/********************************************
* FN:		VAREQ
* CR:		10/04/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::vareq(
	Nlppp *nlppp,
	RFASem *str_sem,
	RFASem *val_sem
	)
{
if (!str_sem && !val_sem)
	return false;

if (!str_sem)
	return false;
_TCHAR *str = sem_to_str(str_sem);
delete str_sem;

if (!val_sem)
	return false;

_TCHAR *sval = 0;
long nval = 0;
bool ok = true;
switch (val_sem->getType())
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		sval = sem_to_str(val_sem);
		delete val_sem;
		return Ivar::nodeVarEQ(nlppp->node_->getData(),str,sval);
		break;
	case RSLONG:
		nval = val_sem->sem_to_long(ok);
		delete val_sem;
		if (!ok)
			return false;
		return Ivar::nodeVarEQ(nlppp->node_->getData(),str,nval);
		break;
	default:
		delete val_sem;
		return false;
	}
return false;
}


/********************************************
* FN:		VARNE
* CR:		10/04/05 AM.
* SUBJ:	Runtime variant of pre action.
********************************************/

bool Arun::varne(
	Nlppp *nlppp,
	RFASem *str_sem,
	RFASem *val_sem
	)
{
if (!str_sem && !val_sem)
	return false;

if (!str_sem)
	return false;
_TCHAR *str = sem_to_str(str_sem);
delete str_sem;

if (!val_sem)
	return false;

_TCHAR *sval = 0;
long nval = 0;
bool ok = true;
switch (val_sem->getType())
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		sval = sem_to_str(val_sem);
		delete val_sem;
		return !Ivar::nodeVarEQ(nlppp->node_->getData(),str,sval);
		break;
	case RSLONG:
		nval = val_sem->sem_to_long(ok);
		delete val_sem;
		if (!ok)
			return false;
		return !Ivar::nodeVarEQ(nlppp->node_->getData(),str,nval);
		break;
	default:
		delete val_sem;
		return false;
	}
return false;
}



/********************************************
* FN:		SINGLE
* CR:		05/13/00 AM.
* SUBJ:	Runtime variant of single post action.
********************************************/

bool Arun::single(
	Nlppp *nlppp
	)
{
return Pat::single(nlppp);
}

/********************************************
* FN:		NOOP
* CR:		05/13/00 AM.
* SUBJ:	Runtime variant of noop post action.
********************************************/

bool Arun::noop(
	Nlppp *nlppp
	)
{
nlppp->noop_ = true;				// FIX.									// 06/05/00 AM.
return true;
}


/********************************************
* FN:		SINGLER
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::singler(
	Nlppp *nlppp,
	long from,
	long to
	)
{
//Node<Pn> *savefirst = nlppp->first_;
//Node<Pn> *savelast  = nlppp->last_;

Node<Pn> *nfirst = 0, *nlast = 0;										// 07/28/03 AM.

if (!Pat::args_range(from,to,nlppp->coll_,nlppp->len_,
								/*UP*/ nfirst, nlast))						// 07/28/03 AM.
	return false;

Node<Pn> *savefirst = 0, *savelast = 0;								// 07/28/03 AM.
bool chfirst=false,chlast=false;											// 07/28/03 AM.
if (nlppp->first_ != nfirst)												// 07/28/03 AM.
	{
	chfirst = true;															// 07/28/03 AM.
	savefirst = nlppp->first_;												// 07/28/03 AM.
	nlppp->first_ = nfirst;													// 07/28/03 AM.
	}
if (nlppp->last_ != nlast)													// 07/28/03 AM.
	{
	chlast = true;																// 07/28/03 AM.
	savelast = nlppp->last_;												// 07/28/03 AM.
	nlppp->last_ = nlast;													// 07/28/03 AM.
	}

bool ok = Pat::single(nlppp);

// This doesn't feel right, since single() action can change first and/or
// last.
//nlppp->first_ = savefirst;
//nlppp->last_ = savelast;

Node<Pn> *node = nlppp->red_;	// Newly created node.				// 07/28/03 AM.

if (chfirst)						// No overlap on first,				// 07/28/03 AM.
	nlppp->first_ = savefirst;	// so restore first.					// 07/28/03 AM.
else
	nlppp->first_ = node;													// 07/28/03 AM.
if (chlast)							// No overlap on last,				// 07/28/03 AM.
	nlppp->last_ = savelast;	// so restore last.					// 07/28/03 AM.
else
	nlppp->last_ = node;														// 07/28/03 AM.

return ok;
}


/********************************************
* FN:		SINGLEX
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Taken from Pat::singlex.
********************************************/

bool Arun::singlex(
	Nlppp *nlppp,
	long from,
	long to
	)
{
Node<Pn> *nfirst = 0, *nlast = 0;

if (!Pat::args_range(from,to,nlppp->coll_,nlppp->len_,/*UP*/ nfirst,nlast))
	return false;

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();

// Excise from first to nfirst (exclusive).

Node<Pn> *node1, *node2;
if (nlppp->first_ && nlppp->first_ != nfirst)
	{
	node1 = nlppp->first_;
	node2 = nfirst->Left();
//	if (!node1->Left())
//		Pat::updateParent(node1,node2);
	tree->exciseSubs(node1, node2);
	// DELETE THE EXTRACTED SUBTREES.
	tree->deleteSubs(node1);
	nlppp->excised_ = true;													// 04/28/01 AM.
	}

// If there's a lookahead node, it is the rightmost bound.
if (nlppp->lookahead_)
	{
	node1 = nlast->Right();
	node2 = nlppp->lookahead_->Left();
	if (nlast != node2)
		{
		tree->exciseSubs(node1, node2);
		// DELETE THE EXTRACTED SUBTREES.
		tree->deleteSubs(node1);
		}
	}
// Excise from nlast (exclusive) to last.
else if (nlppp->last_ && nlppp->last_ != nlast)
	{
	node1 = nlast->Right();
	node2 = nlppp->last_;
	tree->exciseSubs(node1, node2);
	// DELETE THE EXTRACTED SUBTREES.
	tree->deleteSubs(node1);
	// Since last_ node was just deleted, should reset it, I would think.
	nlppp->last_ = nlast;	// Preemptive fix.
	nlppp->excised_ = true;													// 04/28/01 AM.
	}

// Seems to me this code didn't account for possible overlap of first
// and last being reduced with those of entire matched rule.
// So doing a preemptive fix.
Node<Pn> *savefirst = 0, *savelast = 0;
if (nlppp->first_ != nfirst)
	{
	savefirst = nlppp->first_;
	nlppp->first_ = nfirst;
	}
if (nlppp->last_ != nlast)
	{
	savelast = nlppp->last_;
	nlppp->last_ = nlast;
	}

// Reduce the range.
bool ok = Pat::single(nlppp);

// The point is that when there's overlap, we want to KEEP the new
// first_ or last_ that redSingle has set, ie, new reduce node.
if (savefirst)						// No overlap on first,
	nlppp->first_ = savefirst;	// so restore first.
if (savelast)						// No overlap on last,
	nlppp->last_ = savelast;	// so restore last.


return ok;
}


/********************************************
* FN:		SINGLEZAP
* CR:		01/04/01 AM.
* SUBJ:	Reduce, excising all nodes in phrase.
* RET:	True if ok, else false.
* NOTE:	RUNTIME VARIANT.
********************************************/

bool Arun::singlezap(
	Nlppp *nlppp
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[singlezap: Null first or last node in range.]")
		  << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[singlezap: Rule already reduced.]")
		  << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[singlezap: Collect already used.]")
		  << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

if (!nlppp->coll_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[singlezap: No collected nodes.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

//Isugg *sugg = nlppp->rule_->getSugg();	// Suggested elt.

// Create a new node for the reduced rule.
long o_start, o_end;
Pn *tmp;
_TCHAR *text;
tmp = nlppp->first_->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= nlppp->last_->getData()->getEnd();

Node<Pn> *sub;				// The new subtree.
Sym *sym;
_TCHAR *str;
//sym = nlppp->parse_->getSym(sugg->getName());				// Intern the name.
sym = nlppp->parse_->getSym(nlppp->sugg_->name);			// Intern the name.
assert(sym);
str = sym->getStr();
sub = Pn::makeNode(o_start, o_end, PNNODE, text, str, sym,
							line,													// 05/17/01 AM.
							nlppp->sugg_->passnum,							// 08/09/02 AM.
							nlppp->sugg_->ruleline							// 08/09/02 AM.
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.
//tmp->setBase(sugg->getBase());
//tmp->setUnsealed(sugg->getUnsealed());
tmp->setBase(nlppp->sugg_->base);
tmp->setUnsealed(nlppp->sugg_->unsealed);
tmp->setFired(true);															// 05/21/01 AM.

// NO ELT ACTIONS, BECAUSE EVERYTHING'S GONNA GET ZAPPED.
// Perform layering, if any.
//Node<Pn> *bottom;
//bottom = sub;
//sub = Layering(bottom, sugg->getAttrs());

Parse *parse = nlppp->parse_;												// 08/22/02 AM.
tmp->passnum_  = parse->currpass_;	// Pass number for code.	// 08/22/02 AM.
//tmp->ruleline_ = nlppp->line_;	// Associated code line.		// 01/04/02 AM.
tmp->ruleline_ = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 01/06/02 AM.

Tree<Pn> *tree = (Tree<Pn> *) parse->getTree();

// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
tree->replaceSubs(nlppp->first_, nlppp->last_, sub, sub);

////////////////////////////////////
// DELETE ALL THE NODES.
////////////////////////////////////
tree->deleteSubsNC(nlppp->first_);

nlppp->excised_ = true;														// 04/28/01 AM.
nlppp->first_ = nlppp->last_ = 0;
nlppp->red_ = sub;
return true;
}


/********************************************
* FN:		MERGE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::merge(
	Nlppp *nlppp
	)
{
return Pat::merge(nlppp);
}


/********************************************
* FN:		MERGER
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::merger(
	Nlppp *nlppp,
	long from,
	long to
	)
{
//Node<Pn> *savefirst = nlppp->first_;
//Node<Pn> *savelast  = nlppp->last_;

Node<Pn> *nfirst=0, *nlast=0;												// 07/28/03 AM.
if (!Pat::args_range(from,to,nlppp->coll_,nlppp->len_,
								/*UP*/ nfirst,nlast))						// 07/28/03 AM.
	return false;

Node<Pn> *savefirst = 0, *savelast = 0;								// 07/28/03 AM.
bool chfirst=false,chlast=false;											// 07/28/03 AM.
if (nlppp->first_ != nfirst)												// 07/28/03 AM.
	{
	chfirst = true;															// 07/28/03 AM.
	savefirst = nlppp->first_;												// 07/28/03 AM.
	nlppp->first_ = nfirst;													// 07/28/03 AM.
	}
if (nlppp->last_ != nlast)													// 07/28/03 AM.
	{
	chlast = true;																// 07/28/03 AM.
	savelast = nlppp->last_;												// 07/28/03 AM.
	nlppp->last_ = nlast;													// 07/28/03 AM.
	}

// This fixes up first_ and last_.
bool ok = Pat::merge(nlppp);

// This doesn't feel right, since single() action can change first and/or
// last.
//nlppp->first_ = savefirst;
//nlppp->last_ = savelast;


if (chfirst)						// No overlap on first,				// 07/28/03 AM.
	nlppp->first_ = savefirst;	// so restore first.					// 07/28/03 AM.
if (chlast)							// No overlap on last,				// 07/28/03 AM.
	nlppp->last_ = savelast;	// so restore last.					// 07/28/03 AM.

return ok;
}


/********************************************
* FN:		LISTADD
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::listadd(
	Nlppp *nlppp,
	long num1,		// LIST
	long num2,		// ITEM
	_TCHAR *skeep		// (bool) flag."true" or "false".
	)
{
if (nlppp->red_)
	return false;
if (!num1 || !num2)
	return false;

long keep;
if (!_tcscmp(_T("true"),skeep))
	keep = true;
else if (!_tcscmp(_T("false"), skeep))
	keep = false;
else
	return false;

if (num1 > num2)		// Building list to the left.
	return listaddleft(nlppp,num2,num1,keep);

COLL *coll1 = &(nlppp->coll_[num1]);
COLL *coll2 = &(nlppp->coll_[num2]);
Node<Pn> *node1 = (Node<Pn> *)coll1->start;
Node<Pn> *node2 = (Node<Pn> *)coll2->start;
if (!node1 || !node2)
	return false;
if (coll1->start != coll1->end)
	return false;
if (coll2->start != coll2->end)
	return false;

Node<Pn> *childs = node1->Down();
if (!childs)
	return false;

//////////////////////////////////
// Do the listing actions.
//////////////////////////////////
// Remove all nodes between node1 and node2.
// Place node2 as the last child of node1.
// Fix things up accordingly.

// Fix up the first,last nodes for subsequent actions.
// First node is ok as is, since the list node is first or later.
if (nlppp->last_ == node2)
	nlppp->last_ = node1;		// node1 is the "reduced" node.

// Excise from node1's right through node2.
Node<Pn> *nodes = node1->Right();
if (node2->Right())
	node2->Right()->setLeft(node1);
node1->setRight(node2->Right());
nodes->setLeft(0);					// Detach.
node2->setRight(0);					// Detach.

if (!keep)	// Only keeping node2
	{
	// Separate node2 from the rest.
	if (node2->Left())
		node2->Left()->setRight(0);	// Separate.
	node2->setLeft(0);					// Separate.

	if (nodes && nodes != node2)		// If intervening nodes.
		{
//		Node<Pn>::deleteNodes(nodes);	// Delete them.	// BUG.	// 08/07/02 AM.
	// DELETE THE EXTRACTED SUBTREES.						// FIX.	// 08/07/02 AM.
		Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();		// FIX.	// 08/07/02 AM.
		tree->deleteSubs(nodes);								// FIX.	// 08/07/02 AM.
		}

		// Assumed to be whitespace, commas, or other chaff.
	nodes = node2; // Item is the 'list' to be glommed.
	}


// Place nodes under node1.
// List goes from nodes to node2.
if (!nodes)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[listadd: Error..]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
nlppp->excised_ = true;														// 04/28/01 AM.
Node<Pn> *end = childs->last();
end->setRight(nodes);
nodes->setLeft(end);

// Update information for the list node.
Pn *pn1 = node1->getData();
Pn *pn2 = node2->getData();
pn1->setEnd(pn2->getEnd());	// Update the end offset.

//////////
// FIXUPS
//////////
// Rejigger the collect to reflect the list "reduction".
coll1->end = node2;

// Zero collect elts from (coll1+1) to coll2.
COLL *pcoll = &(nlppp->coll_[num1+1]);
int ii;
for (ii = num1+1; ii <= num2; ++ii, ++pcoll)
	{
	pcoll->start = pcoll->end = 0;
	pcoll->totnodes = 0;
	}

// LIST NODE IS THE "NEW" REDUCE NODE!
nlppp->red_ = node1;
return true;
}


/********************************************
* FN:		LISTADDLEFT
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::listaddleft(
	Nlppp *nlppp,
	long num1,		// ITEM (to the left of list).
	long num2,		// LIST
	long keep		// (bool) flag.
	)
{
if (nlppp->red_)
	return false;
if (!num1 || !num2)
	return false;
if (num1 > num2)		// Building list to the left.
	return listaddleft(nlppp,num2,num1,keep);

COLL *coll1 = &(nlppp->coll_[num1]);
COLL *coll2 = &(nlppp->coll_[num2]);
Node<Pn> *node1 = (Node<Pn> *)coll1->start;
Node<Pn> *node2 = (Node<Pn> *)coll2->start;
if (!node1 || !node2)
	return false;
if (coll1->start != coll1->end)
	return false;
if (coll2->start != coll2->end)
	return false;

Node<Pn> *childs = node2->Down();
if (!childs)
	return false;

//////////////////////////////////
// Do the listing actions.
//////////////////////////////////
// Remove all nodes between node1 and node2.
// Place node2 as the last child of node1.
// Fix things up accordingly.

// Fix up the first,last nodes for subsequent actions.
// Last node is ok as is, since the list node is last or earlier.
if (nlppp->first_ == node1)
	nlppp->first_ = node2;		// node2 is the "reduced" node.

// Excise from node1 up to but not including node2.
Node<Pn> *ends = node2->Left();
if (node1->Left())
	node1->Left()->setRight(node2);
else	// NODE IS FIRST IN ITS LIST.
	{
	// DETACH ITEM FROM TREE, ATTACH LIST TO TREE.
	if (node1->Up())
		node1->Up()->setDown(node2);
	node2->setUp(node1->Up());
	node1->setUp(0);
	}
node2->setLeft(node1->Left());
node1->setLeft(0);					// Detach.
ends->setRight(0);					// Detach.

if (!keep)			// Not keeping intervening nodes.		// 12/14/99 AM.
	{
	// Separate node1 from the rest.
	Node<Pn> *nodes = node1->Right();
	if (nodes)
		nodes->setLeft(0);				// Separate.
	node1->setRight(0);					// Separate.

	if (nodes && nodes != node2)		// If intervening nodes.
//		Node<Pn>::deleteNodes(nodes);	// Delete them.	// BUG.	// 08/07/02 AM.
		{
		// DELETE THE EXTRACTED SUBTREES.					// FIX.	// 08/07/02 AM.
		Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();		// FIX.	// 08/07/02 AM.
		tree->deleteSubs(nodes);								// FIX.	// 08/07/02 AM.
		// Assumed to be whitespace, commas, or other chaff.
		}
	ends = node1;	// New end of 'list'.						// 12/14/99 AM.
	}

// Nodes go from node1 to end, whether kept or not.		// 12/14/99 AM.

// Place node1 under node2.
if (!node1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: Error..]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
nlppp->excised_ = true;														// 04/28/01 AM.

// Detach children of list node.
if (childs)
	childs->setUp(0);
node2->setDown(0);

// Push item chain onto list of children.
ends->setRight(childs);
if (childs)
	childs->setLeft(ends);
childs = node1;			// Item is new start of list.

// Reattach children.
childs->setUp(node2);
node2->setDown(childs);


// Update information for the list node.
Pn *pn1 = node1->getData();
Pn *pn2 = node2->getData();
pn2->setStart(pn1->getStart());	// Update the start offset.
pn2->setText(pn1->getText());		// New start of node's text.

//////////
// FIXUPS
//////////
// Rejigger the collect to reflect the list "reduction".
coll2->start = node1;

// Zero collect elts from (coll1) to (coll2-1).
COLL *pcoll = &(nlppp->coll_[num1]);
int ii;
for (ii = num1; ii < num2; ++ii, ++pcoll)
	{
	pcoll->start = pcoll->end = 0;
	pcoll->totnodes = 0;
	}

// LIST NODE IS THE "NEW" REDUCE NODE!
nlppp->red_ = node2;
return true;
}


/********************************************
* FN:		SPLICE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	12/30/03 AM. Retaining the collect structuring.
********************************************/

bool Arun::splice(
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (nlppp->red_)
	return false;
if (!nlppp->coll_)
	return false;

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();

// New first and last nodes that were spliced.
Node<Pn> *first=0, *last=0;											// 12/30/03 AM.
bool ffirst=false,flast=false;	// Overlap first & last.	// 12/30/03 AM.
Node<Pn> *sub1=0, *sub2=0;												// 12/30/03 AM.
Node<Pn> *child1=0, *child2=0;	// First and last child for current elt.

// For each elt in range, splice its nodes.						// 12/30/03 AM.
COLL *pcoll = &(nlppp->coll_[num1]);								// 12/12/02 AM.
int ii;																		// 12/12/02 AM.
for (ii = num1; ii <= num2; ++ii, ++pcoll)						// 12/12/02 AM.
	{
	if (!(sub1 = (Node<Pn>*)pcoll->start))							// 12/30/03 AM.
		continue;															// 12/30/03 AM.
	sub2 = (Node<Pn>*)pcoll->end;										// 12/30/03 AM.

	// Check overlap with first,last matched nodes of rule.	// 12/30/03 AM.
	if (sub1 == nlppp->first_)											// 12/30/03 AM.
		ffirst = true;														// 12/30/03 AM.
	if (sub2 == nlppp->last_)											// 12/30/03 AM.
		flast = true;														// 12/30/03 AM.

	// Splice this subrange of nodes.
	tree->spliceSubs(sub1,sub2,/*UP*/child1,child2);			// 12/30/03 AM.
	if (!child1)	// Node has no children							// 12/30/03 AM.
		{
		pcoll->start = pcoll->end = 0;								// 12/30/03 AM.
		pcoll->totnodes = 0;												// 12/30/03 AM.
		continue;															// 12/30/03 AM.
		}

	if (!first)																// 12/30/03 AM.
		first = child1;													// 12/30/03 AM.
	last = child2;															// 12/30/03 AM.

	// Fixup collect element.
	pcoll->start = (Node<Pn>*)child1;								// 12/30/03 AM.
	pcoll->end = (Node<Pn>*)child2;									// 12/30/03 AM.
	pcoll->totnodes = Node<Pn>::lengthr(child1,child2);		// 12/30/03 AM.
	}

// Fix up matcher data structures.
// Todo: what if nlppp->node_ is impacted?  Do we care here.
// Note that splice may remove all matched nodes...
if (ffirst)																	// 12/30/03 AM.
	nlppp->first_ = first;												// 12/30/03 AM.
if (flast)																	// 12/30/03 AM.
	nlppp->last_ = last;													// 12/30/03 AM.

return true;
}

#ifdef SPLICE_OLD
bool Arun::splice(
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (nlppp->red_)
	return false;

Node<Pn> *node1, *node2;
if (!Pat::args_range(num1,num2,nlppp->coll_,nlppp->len_,
								/*UP*/ node1,node2))
	return false;

if (node1)
	{
	Node<Pn> *newstart, *newend;											// 07/19/02 AM.
	Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();
	tree->spliceSubs(node1, node2,
							newstart, newend);								// 07/19/02 AM.

	// QUICK-AND-DIRTY.											// FIX.	// 12/12/02 AM.
	// Just zero out the collect range.									// 12/12/02 AM.
	// (Really, each element												// 12/12/02 AM.
	// should get its spliced subphrase, I suppose.)				// 12/12/02 AM.
	COLL *pcoll = &(nlppp->coll_[num1]);								// 12/12/02 AM.
	int ii;																		// 12/12/02 AM.
	for (ii = num1; ii <= num2; ++ii, ++pcoll)						// 12/12/02 AM.
		{
		pcoll->start = pcoll->end = 0;									// 12/12/02 AM.
		pcoll->totnodes = 0;													// 12/12/02 AM.
		}
	}


return true;
}
#endif


/********************************************
* FN:		SETBASE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::setbase(
	Nlppp *nlppp,
	long num,
	_TCHAR *str
	)
{
if (num <= 0 || num > nlppp->len_ || !str || !*str)
	return false;
if (nlppp->red_)
	return false;

bool flag;
if (!strcmp_i(_T("true"), str))
	flag = true;
else if (!strcmp_i(_T("false"), str))
	flag = false;
else
	return false;

COLL *pcoll = &(nlppp->coll_[num]);
if (!pcoll->start)
	return true;		// Must have been an optional elt.
if (pcoll->start != pcoll->end)
	return false;		// Requiring a one-node match.

Pn *pn = ((Node<Pn> *)pcoll->start)->getData();
pn->setBase(flag);
return true;
}


/********************************************
* FN:		SETUNSEALED
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
********************************************/

bool Arun::setunsealed(
	Nlppp *nlppp,
	long num,
	_TCHAR *str
	)
{
if (num <= 0 || num > nlppp->len_ || !str || !*str)
	return false;
if (nlppp->red_)
	return false;

bool flag;
if (!strcmp_i(_T("true"), str))
	flag = true;
else if (!strcmp_i(_T("false"), str))
	flag = false;
else
	return false;

COLL *pcoll = &(nlppp->coll_[num]);
if (!pcoll->start)
	return true;		// Must have been an optional elt.
if (pcoll->start != pcoll->end)
	return false;		// Requiring a one-node match.

Pn *pn = ((Node<Pn> *)pcoll->start)->getData();
pn->setUnsealed(flag);
return true;
}


/********************************************
* FN:		DEFAULT_POST_ACTION
* CR:		05/11/00 AM.
* SUBJ:	Runtime default post action.
********************************************/

bool Arun::default_post_action(
	Nlppp *nlppp
	)
{
if (!single(nlppp))
	return false;
return post_final(nlppp);
}

/********************************************
* FN:		GROUP
* CR:		05/13/00 AM.
* SUBJ:	Runtime variant of group post action.
********************************************/

bool Arun::group(
	Nlppp *nlppp,
	int from,			// Start of range.
	int to,				// End of range.
	_TCHAR *name			// Name for grouped node.
	)
{
Node<Pn> *node1 = (Node<Pn> *) get_left_node(nlppp->coll_, from, to);
Node<Pn> *node2 = (Node<Pn> *) get_right_node(nlppp->coll_, from, to);
if (!node1 && !node2)
	return true;
if (!node1 || !node2)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: group post action.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;		// Error to be able to fetch one but not the other.
	}

// Build the group node.
Node<Pn> *node = Pat::group(node1,node2,name,nlppp);

// FIX UP THE COLLECT.
COLL *coll = &(nlppp->coll_[from]);
coll->start = coll->end = node;
// coll->totnodes = ?	// Probably don't need this, else can compute it.
		// Get child and compute length of that phrase.
int ii;

// NOTE:	ALIGNING COMPILED VERSION WITH INTERP.						// 08/06/03 AM.
long newlen = nlppp->len_ - (to - from);								// 08/06/03 AM.
COLL *ptr = &(nlppp->coll_[to]);											// 08/06/03 AM.
for (ii = (to+1); ii <= newlen; ++ii)									// 08/06/03 AM.
	{
	++coll;																		// 08/06/03 AM.
	++ptr;																		// 08/06/03 AM.
	// Copy back from ptr to coll.
	coll->start = ptr->start;												// 08/06/03 AM.
	coll->end   = ptr->end;													// 08/06/03 AM.
	coll->totnodes = ptr->totnodes;										// 08/06/03 AM.
	}

// CLEAR OUT BEYOND NEW COLLECT LENGTH.								// 08/06/03 AM.
for (ii = newlen+1; ii <= nlppp->len_; ++ii)							// 08/06/03 AM.
	{
	++coll;																		// 08/06/03 AM.
	++ptr;																		// 08/06/03 AM.
	// Copy back from ptr to coll.
	coll->start = ptr->start;												// 08/06/03 AM.
	coll->end   = ptr->end;													// 08/06/03 AM.
	coll->totnodes = ptr->totnodes;										// 08/06/03 AM.
	// Clear out.
	ptr->start = ptr->end = 0;												// 08/06/03 AM.
	ptr->totnodes = 0;														// 08/06/03 AM.
	}

// RESET COLLECT'S CURRENT LENGTH.										// 08/06/03 AM.
nlppp->len_ = newlen;														// 08/06/03 AM.

#ifdef OLD_030806_
// NOTE:  DIFFERS FROM THE INTERPRETED VERSION!!!!
// Keep the collect as is, except that the group node will be placed into
// the "from" collect element, and the rest of the collect elements of the
// group will be zeroed out.  This result is distinct from that currently
// done for the interpreted analyzer.  (The latter should be changed to
// look like the compiled version.)
for (ii = ++from; ii <= to; ++ii)
	{
	++coll;		// Traverse collect.
	coll->start = coll->end = 0;
	coll->totnodes = 0;
	}
#endif

return true;
}

//////////////////
// GROUP VARIANTS																// 04/28/01 AM.
//////////////////

bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *from_sem,	// Start of range.
	int to,				// End of range.
	_TCHAR *name			// Name for grouped node.
	)
{
if (!from_sem)
	return false;
// Convert sem to num and trash sem.
bool ok = true;
int from = from_sem->sem_to_long(ok);
delete from_sem;
if (!ok)
	return false;
return Arun::group(nlppp,from,to,name);
}


bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	int from,			// Start of range.
	RFASem *to_sem,	// End of range.
	_TCHAR *name			// Name for grouped node.
	)
{
if (!to_sem)
	return false;
// Convert sem to num and trash sem.
bool ok = true;
int to = to_sem->sem_to_long(ok);
delete to_sem;
if (!ok)
	return false;
return Arun::group(nlppp,from,to,name);
}


bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	int from,			// Start of range.
	int to,				// End of range.
	RFASem *name_sem	// Name for grouped node.
	)
{
if (!name_sem)
	return false;
_TCHAR *name = sem_to_str(name_sem);
delete name_sem;
if (!name || !*name)
	return false;
return Arun::group(nlppp,from,to,name);
}

bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *from_sem,	// Start of range.
	RFASem *to_sem,	// End of range.
	_TCHAR *name			// Name for grouped node.
	)
{
if (!from_sem || !to_sem)
	return false;
// Convert sem to num and trash sem.
bool ok = true;
int from = from_sem->sem_to_long(ok);
delete from_sem;
if (!ok)
	{
	delete to_sem;
	return false;
	}
int to = to_sem->sem_to_long(ok);
delete to_sem;
if (!ok)
	return false;
return Arun::group(nlppp,from,to,name);
}

bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *from_sem,	// Start of range.
	int to,				// End of range.
	RFASem *name_sem	// Name for grouped node.
	)
{
if (!from_sem || !name_sem)
	return false;

bool ok = true;
int from = from_sem->sem_to_long(ok);
delete from_sem;
if (!ok)
	{
	delete name_sem;
	return false;
	}

_TCHAR *name = sem_to_str(name_sem);
delete name_sem;
if (!name || !*name)
	return false;
return Arun::group(nlppp,from,to,name);
}


bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	int from,			// Start of range.
	RFASem *to_sem,	// End of range.
	RFASem *name_sem	// Name for grouped node.
	)
{
if (!to_sem || !name_sem)
	return false;

bool ok = true;
int to = to_sem->sem_to_long(ok);
delete to_sem;
if (!ok)
	{
	delete name_sem;
	return false;
	}

_TCHAR *name = sem_to_str(name_sem);
delete name_sem;
if (!name || !*name)
	return false;
return Arun::group(nlppp,from,to,name);
}


bool Arun::group(																// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *from_sem,	// Start of range.
	RFASem *to_sem,	// End of range.
	RFASem *name_sem	// Name for grouped node.
	)
{
if (!from_sem || !to_sem || !name_sem)
	return false;
// Convert sem to num and trash sem.
bool ok = true;
int from = from_sem->sem_to_long(ok);
delete from_sem;
if (!ok)
	{
	delete to_sem;
	delete name_sem;
	return false;
	}
int to = to_sem->sem_to_long(ok);
delete to_sem;
if (!ok)
	{
	delete name_sem;
	return false;
	}

_TCHAR *name = sem_to_str(name_sem);
delete name_sem;
if (!name || !*name)
	return false;

return Arun::group(nlppp,from,to,name);
}


/********************************************
* FN:		EXCISE
* CR:		05/11/00 AM.
* SUBJ:	Runtime variant of excise post action.
********************************************/

bool Arun::excise(
	Nlppp *nlppp,
	int from,			// Start of range.
	int to				// End of range.
	)
{
// Assume from-to numbers have been checked at compile time.
Node<Pn> *node1 = (Node<Pn> *) get_left_node(nlppp->coll_, from, to);
Node<Pn> *node2 = (Node<Pn> *) get_right_node(nlppp->coll_,from,to);
if (!node1 && !node2)
	return true;
if (!node1 || !node2)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: excise post action.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;		// Error to be able to fetch one but not the other.
	}

//Pat::updateParent(node1,node2);

// FIX UP STATE OF THE PARSE.								// FIX.		// 03/31/01 AM.
// This will enable actions to follow excise in @POST.			// 03/31/01 AM.
// Fix up the first,last nodes for subsequent actions.			// 03/31/01 AM.
Node<Pn> *nbefore = node1->Left();										// 03/31/01 AM.
Node<Pn> *nafter = node2->Right();										// 03/31/01 AM.
if (nlppp->first_ == node1 && nlppp->last_ == node2)				// 03/31/01 AM.
	{
	// Excising everything that matched rule.							// 03/31/01 AM.
	nlppp->first_ = nlppp->last_ = 0;									// 03/31/01 AM.
	}
else if (nlppp->first_ == node1)											// 03/31/01 AM.
	nlppp->first_ = nafter;													// 03/31/01 AM.
else if (nlppp->last_ == node2)											// 03/31/01 AM.
	nlppp->last_ = nbefore;													// 03/31/01 AM.

COLL *coll = &(nlppp->coll_[from-1]);									// 08/06/03 AM.
int ii;																			// 08/06/03 AM.

// NOTE:	ALIGNING COMPILED VERSION WITH INTERP.						// 08/06/03 AM.
long newlen = nlppp->len_ - (to - from + 1);							// 08/06/03 AM.
COLL *ptr = &(nlppp->coll_[to]);											// 08/06/03 AM.
for (ii = (to+1); ii <= newlen; ++ii)									// 08/06/03 AM.
	{
	++coll;																		// 08/06/03 AM.
	++ptr;																		// 08/06/03 AM.
	// Copy back from ptr to coll.
	coll->start = ptr->start;												// 08/06/03 AM.
	coll->end   = ptr->end;													// 08/06/03 AM.
	coll->totnodes = ptr->totnodes;										// 08/06/03 AM.
	}

// CLEAR OUT BEYOND NEW COLLECT LENGTH.								// 08/06/03 AM.
for (ii = newlen+1; ii <= nlppp->len_; ++ii)							// 08/06/03 AM.
	{
	++coll;																		// 08/06/03 AM.
	++ptr;																		// 08/06/03 AM.
	// Copy back from ptr to coll.
	coll->start = ptr->start;												// 08/06/03 AM.
	coll->end   = ptr->end;													// 08/06/03 AM.
	coll->totnodes = ptr->totnodes;										// 08/06/03 AM.
	// Clear out.
	ptr->start = ptr->end = 0;												// 08/06/03 AM.
	ptr->totnodes = 0;														// 08/06/03 AM.
	}

// RESET COLLECT'S CURRENT LENGTH.										// 08/06/03 AM.
nlppp->len_ = newlen;														// 08/06/03 AM.

#ifdef OLD_030806_
// Fix up the collect data structure.					// FIX.		// 03/31/01 AM.
// NOTE:  DIFFERS FROM THE INTERPRETED VERSION!!!!					// 03/31/01 AM.
// Keep the collect as is, except that the excised collect elements will
// be zeroed out.  This result is distinct from that currently
// done for the interpreted analyzer.  (The latter should be changed to
// look like the compiled version.)
COLL *coll = &(nlppp->coll_[from]);										// 03/31/01 AM.
int ii;																			// 03/31/01 AM.
for (ii = from; ii <= to; ++ii, ++coll)								// 03/31/01 AM.
	{
	coll->start = coll->end = 0;											// 03/31/01 AM.
	coll->totnodes = 0;														// 03/31/01 AM.
	}
#endif

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();
tree->exciseSubs(node1,node2);
tree->deleteSubs(node1);
nlppp->excised_ = true;														// 04/28/01 AM.

return true;
}


/********************************************
* FN:		ADDSTRS
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Old stuff, but used in lookup.  Var name refers to a global var.
********************************************/

bool Arun::addstrs(
	Nlppp *nlppp,
	_TCHAR *var,
	long num
	)
{
if (!var || !*var)
	return false;
if (num <= 0 || num > nlppp->len_)
	return false;

COLL *pcoll = &(nlppp->coll_[num]);
if (!pcoll->start)
	return true;		// Must be an empty optional elt.

Node<Pn> *nstart = (Node<Pn> *)pcoll->start;
Node<Pn> *nend   = (Node<Pn> *)pcoll->end;

// Fetch node's string.
Node<Pn> *base;
Pn *pn;
_TCHAR *str;

if (nstart == nend)
	{
	base = Pat::baseNode(nstart);	// Get bottommost node.
	pn = base->getData();
	str = pn->getName();
	if (!str || !*str || (*str == '_'))
		return true;		// Not a literal. Don't add it.
	}
else	// Build and internalize string for range of nodes.
	str = Pat::rangeText(nstart, nend, nlppp->parse_);

// Add string value to the variable.
Var::addStrs(var, str, nlppp->parse_);

return true;
}


/********************************************
* FN:		PRINT
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::print(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
if (!str || !*str)
	return false;
_t_cout << str << flush;
return true;
}


/********************************************
* FN:		PRINTR
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::printr(
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
Node<Pn> *node1, *node2;

if (!Pat::args_range(num1,num2,nlppp->coll_,nlppp->len_,
								/*UP*/ node1,node2))
	return false;

return Pat::printr(node1,node2,&_t_cout,nlppp);
}

/********************************************
* FN:		PRLIT
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::prlit(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *str
	)
{
if (!str || !*str)
	return true;			// Allowing "null" print to file.

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Prlit: Couldn't output to file.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Prlit post action: file=") << fname << _T(" is closed.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Output the string to the file.
if (str && *str)
	*ostr << str << flush;

return true;
}


/********************************************
* FN:		PRRANGE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::prrange(
	Nlppp *nlppp,
	_TCHAR *fname,
	long num1,
	long num2
	)
{
if (!fname || !*fname)
	return false;

Node<Pn> *node1, *node2;
if (!Pat::args_range(num1,num2,nlppp->coll_,nlppp->len_,/*UP*/ node1,node2))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Prrange post action: file=") << fname << _T(" is closed.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

return Pat::printr(node1,node2,ostr,nlppp);		// FIX.			// 06/11/00 AM.
}


/********************************************
* FN:		PRCHILD
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::prchild(
	Nlppp *nlppp,
	_TCHAR *fname,
	long num,
	_TCHAR *name
	)
{
if (!fname || !*fname || !name || !*name)
	return false;
if (num <= 0 || num > nlppp->len_)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Prrange post action: file=") << fname << _T(" is closed.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get nth node.
COLL *pcoll = &(nlppp->coll_[num]);
if (!pcoll->start)			// Optional node.
	return true;			// That's ok.
if (pcoll->start != pcoll->end)				// Multi-node match of elt.
	return false;			// Not handling.

// Get subphrase.
Node<Pn> *phrase = ((Node<Pn> *) pcoll->start)->Down();
if (!phrase)
	return true;			// No children, that's ok.

// Look for name in subphrase.
Pn *pn;
for (; phrase; phrase = phrase->Right())
	{
	pn = phrase->getData();
	if (!strcmp_i(name, pn->getName()))	// FIX?	// 06/07/00 AM.
		break;
	}
if (!phrase)
	return true;		// Absent is ok.

// If found, get its phrase.
_TCHAR *str;
long len;
long n1, n2;
n1 = pn->getStart();
n2 = pn->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Get ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;

// Print the thing.
while (len--
		 && *str)																// 04/30/01 AM.
	*ostr << *str++;
*ostr << flush;

return true;
}


/********************************************
* FN:		PRTREE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::prtree(
	Nlppp *nlppp,
	_TCHAR *fname,
	long num,
	_TCHAR *name
	)
{
if (!fname || !*fname || !name || !*name)
	return false;
if (num <= 0 || num > nlppp->len_)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Prrange post action: file=") << fname << _T(" is closed.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get nth node.
COLL *pcoll = &(nlppp->coll_[num]);
if (!pcoll->start)			// Optional node.
	return true;			// That's ok.
if (pcoll->start != pcoll->end)				// Multi-node match of elt.
	return false;			// Not handling.
Node <Pn> *node = (Node<Pn> *)pcoll->start;

// Traverse the nstart subree, looking for name.
Node<Pn> *tmp;
Pn *pn;
if (!(tmp = Pn::findTreenode(name, node, true)))
	return true;			// Not found is ok.
pn = tmp->getData();

// If found, get its phrase.
_TCHAR *str;
long len;
long n1, n2;
n1 = pn->getStart();
n2 = pn->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Get ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;

// Print the thing.
while (len--
		 && *str)																// 04/30/01 AM.
	*ostr << *str++;
*ostr << flush;

return true;
}


/********************************************
* FN:		PRXTREE
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::prxtree(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *prestr,
	long ord,
	_TCHAR *name,
	_TCHAR *poststr
	)
{
if (!fname || !*fname || !name || !*name)
	return false;
if (ord <= 0 || ord > nlppp->len_)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Prxtree post action: file=") << fname << _T(" is closed.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get nth node.
COLL *pcoll = &(nlppp->coll_[ord]);
if (!pcoll->start)			// Optional node.
	return true;			// That's ok.
if (pcoll->start != pcoll->end)				// Multi-node match of elt.
	return false;			// Not handling.
Node <Pn> *node = (Node<Pn> *)pcoll->start;

// Traverse the node subree, looking for name.
Node<Pn> *tmp;
Pn *pn;
if (!(tmp = Pn::findTreenode(name, node, true)))
	return true;			// Not found is ok.
pn = tmp->getData();

// If found, get its phrase.
_TCHAR *str;
long len;
long n1, n2;
n1 = pn->getStart();
n2 = pn->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Get ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;

if (!len)
	return true;			// If nothing to print.

// Print the thing.
if (prestr && *prestr)														// 04/30/01 AM.
	*ostr << prestr;
while (len--
		 && *str)																// 04/30/01 AM.
	*ostr << *str++;
if (poststr && *poststr)													// 04/30/01 AM.
	*ostr << poststr;
*ostr << flush;

return true;
}


/********************************************
* FN:		NDUMP
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::ndump(
	Nlppp *nlppp,
	_TCHAR *fname,
	long ord
	)
{
if (!fname || !*fname)
	return false;
if (ord <= 0 || ord > nlppp->len_)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Ndump: file=") << fname << _T(" is closed.]")
			<< ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get nth node.
COLL *pcoll = &(nlppp->coll_[ord]);
if (!pcoll->start)			// Optional node.
	return true;			// That's ok.
if (pcoll->start != pcoll->end)				// Multi-node match of elt.
	return false;			// Not handling.
Node <Pn> *node = (Node<Pn> *)pcoll->start;


// GET EACH VARIABLE IN NODE AND PRINT IT!
//Dlist<Ipair> *vars = nlppp->parse_->getVars();	// These are globals!
Pn *pn = node->getData();

// DUMP THE NODE TEXT ALSO!
_TCHAR *buf = pn->pnStr();
*ostr << _T("$text=")
		<< ::str(buf)																// 04/30/01 AM.
		<< endl;
Chars::destroy(buf);


Dlist<Ipair> *vars = pn->getDsem();
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}


/********************************************
* FN:		XDUMP
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::xdump(
	Nlppp *nlppp,
	_TCHAR *fname,
	long ord
	)
{
if (!fname || !*fname)
	return false;
if (ord < 0)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Xdump: file=") << fname << _T(" is closed.]")
			<< ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET NTH CONTEXT NODE.
// This is a new style, absent in earlier X actions.
// Taken from IVAR.CPP functions.
Node<Pn> *select = 0;
Node<Pn> *tmp;
Pn *pn = 0;
if (!(select = nlppp->getSelect()))
	return false;
if (!ord)						// LAST component of path.
	pn = select->getData();
else								// Get component.
	{
	// Recurse to the root, then count down.
	tmp = select->pathNth(ord);
	if (!tmp)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Xdump: Couldn't find node X(") << ord
					<< _T(").]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	pn  = tmp->getData();
	}

// GET EACH VARIABLE IN NODE AND PRINT IT!
//Dlist<Ipair> *vars = nlppp->parse_->getVars();	// These are globals!
Dlist<Ipair> *vars = pn->getDsem();
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}


/********************************************
* FN:		GDUMP
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::gdump(
	Nlppp *nlppp,
	_TCHAR *fname
	)
{
if (!fname || !*fname)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Gdump: file=") << fname << _T(" is closed.]")
			<< ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET EACH VARIABLE IN NODE AND PRINT IT!
Dlist<Ipair> *vars = nlppp->parse_->getVars();	// These are globals!
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}



/********************************************
* FN:		SDUMP
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::sdump(
	Nlppp *nlppp,
	_TCHAR *fname
	)
{
if (!fname || !*fname)
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Sdump: file=") << fname << _T(" is closed.]")
			<< ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET EACH VARIABLE IN SUGGESTED NODE AND PRINT IT!
// (This variable list is being built for suggested node.)
Dlist<Ipair> *vars = nlppp->getDsem();		// Get vars for sugg.
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}

/********************************************
* FN:		ADDATTR
* CR:		06/02/00 AM.
* SUBJ:	Runtime variant of post action.
* NOTE:	Backward compatibility.
*			Belongs in FN
********************************************/

bool Arun::addattr(
	Nlppp *nlppp,
	_TCHAR *s_conc,
	_TCHAR *attr,
	_TCHAR *val
	)
{
if (!s_conc || !*s_conc || !attr || !*attr || !val || !*val)
	return false;

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

// Then get ticket concept.
CONCEPT *root = cg->findRoot();
CONCEPT *conc = cg->findConcept(root, _T("ticket"));

// Then just add the attribute.
cg->addVal(conc, attr, val);

return true;

}


/********************************************
* FN:		SET_NODE_NAME
* CR:		05/13/00 AM.
* SUBJ:	Rename the given node.
* NOTE:	An element action.
********************************************/

bool Arun::set_node_name(
	NODE *node,
	_TCHAR *name
	)
{
Pn *pn = ((Node<Pn> *) node)->getData();
pn->setName(name);
return true;
}


/********************************************
* FN:		SET_NODE_NAME
* CR:		05/23/00 AM.
* SUBJ:	Rename the given node.
* NOTE:	An element action.
********************************************/

bool Arun::set_node_name(
	int ord,						// Elt number.
	_TCHAR *name,
	Nlppp *nlppp
	)
{
COLL *coll = &(nlppp->coll_[ord]);
Node<Pn> *node;
Pn *pn;

Node<Pn> *end = 0;
if (end = (Node<Pn> *) coll->end)
	end = end->Right();					// One past last node.
for (node = (Node<Pn> *)coll->start;
				node && node != end;
				node = node->Right())
	{
	pn = node->getData();
	pn->setName(name);
	}

return true;
}



/********************************************
* FN:		GROUP_ELT
* CR:		09/23/00 AM.
* SUBJ:	Element's group action.
* NOTE:	An element action.
********************************************/

bool Arun::group_elt(
	int ord,						// Elt number.
	_TCHAR *name,
	Nlppp *nlppp
	)
{
if (!ord || !name || !*name || !nlppp)
	return false;
return group(nlppp,ord,ord,name);
}


/********************************************
* FN:		LAYER_ELT
* CR:		05/13/00 AM.
* SUBJ:	Runtime variant of layer_elt element action.
********************************************/

bool Arun::layer_elt(
	NODE *node,
	const _TCHAR **arr_attrs,
	Nlppp *nlppp)
{
Node<Pn> *nd = (Node<Pn> *) node;
return false;
}

/********************************************
* FN:		LAYER_ELT
* CR:		05/23/00 AM.
* SUBJ:	Runtime variant of layer_elt element action.
********************************************/

bool Arun::layer_elt(
	int ord,								// Rule elt number.
	const _TCHAR **arr_attrs,
	Nlppp *nlppp)
{
return Pat::layerElt(
	&(nlppp->coll_[ord]),
	arr_attrs,
	nlppp->parse_,
	/*DU*/
	nlppp->first_,
	nlppp->last_
	);
}

/********************************************
* FN:		POST_FINAL
* CR:		05/13/00 AM.
* SUBJ:	Runtime variant of final post action.
* NOTE:	Modelled on Pat::postFinal.
********************************************/

bool Arun::post_final(
	Nlppp *nlppp
	)
{
nlppp->start_ = nlppp->node_ = nlppp->after_;	// Set up for next rule.

// Handle local variables.													// 11/24/02 AM.
if (nlppp->locals_)															// 11/24/02 AM.
	{
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);				// 11/24/02 AM.
	nlppp->locals_ = 0;														// 11/24/02 AM.
	}

// nlppp->used_	// This is no longer an issue.
if (nlppp->red_)
	{
	if (!nlppp->sem_ && !nlppp->dsem_)
		return true;		// No semantics to attach.
	// Attach semantics to the reduce node.
	Pn *pn;
	pn = nlppp->red_->getData();
	pn->setSem(nlppp->sem_);
	pn->setDsem(nlppp->dsem_);
	}
else		// Implicit NOOP, excise, or other reduces.
	{
	if (nlppp->sem_ || nlppp->dsem_)
		{
	//	strstream gerrStr(Errbuf,MAXSTR,ios::out);
	//	gerrStr << "[No rule reduce. Can't attach semantic data.]" << ends;
	//	nlppp->parse_->errOut(false);

		if (nlppp->sem_)
			{
			delete nlppp->sem_;
			nlppp->sem_ = 0;
			}
		if (nlppp->dsem_)
			{
			Dlist<Ipair>::DeleteDlistAndData(nlppp->dsem_);
			nlppp->dsem_ = 0;
			}
		// THIS IS OK, NOT WRONG, SO SHOULDN'T AFFECT PROCESSING	// 08/09/02 AM.
//		return false;												// BUG.	// 08/09/02 AM.
		}
	}

return true;
}


/********************************************
* FN:		RECURSE
* CR:		05/31/00 AM.
* SUBJ:	Runtime variant of recurse mini pass execution.
********************************************/

bool Arun::recurse(
	int ord,
	bool (rules)(int,bool&,Nlppp *),	// Gen'd fn for rules of recurse pass.
//	const PASS *pass,
	Nlppp *nlppp
	)
{
COLL *coll = &(nlppp->coll_[ord]);
if (!coll)
	return false;
if (!coll->start)
	return true;		// Elt must have been optional.

return Pat::Recurse(coll, rules, nlppp->parse_, 
				/*UP*/ nlppp->first_, nlppp->last_);
//return Pat::Recurse(coll, nlppp->parse_, pass,
//				/*UP*/ nlppp->first_, nlppp->last_);

}

/******************************************************************************/
// NLP++ COMPILED VERSION, RUNTIME FUNCTIONS
/******************************************************************************/

/********************************************
* FN:		N
* CR:		05/13/00 AM.
* SUBJ:	NLP++ variable, compiled runtime.
* NOTE:	User should free the sem that is built here.
********************************************/

RFASem *Arun::n(
	_TCHAR *varname,
	int nelt,
	long index,					// 10/13/00 AM.
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
RFASem *sem = 0;
Ivar::eval(RUNNODEVAR,varname,nelt,index,ref,nlppp,/*UP*/sem);
return sem;
}

// VARIANT
RFASem *Arun::n(																// 05/04/01 AM.
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return n(varname,nelt,index,ref,nlppp);
}

/********************************************
* FN:		X
* CR:		05/13/00 AM.
* SUBJ:	NLP++ variable, compiled runtime.
********************************************/

RFASem *Arun::x(
	_TCHAR *varname,
	int nelt,
	long index,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
RFASem *sem = 0;
Ivar::eval(CONTEXTVAR,varname,nelt,index,ref,nlppp,/*UP*/sem);
return sem;
}

// VARIANT.
RFASem *Arun::x(															// 05/04/01 AM.
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return x(varname,nelt,index,ref,nlppp);
}

/********************************************
* FN:		S
* CR:		05/13/00 AM.
* SUBJ:	NLP++ variable, compiled runtime.
********************************************/

RFASem *Arun::s(
	_TCHAR *varname,
	long index,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
RFASem *sem = 0;
Ivar::eval(SUGGESTEDVAR,varname,0,index,ref,nlppp,/*UP*/sem);
return sem;
}

// VARIANT.
RFASem *Arun::s(																// 05/04/01 AM.
	_TCHAR *varname,
	RFASem *index_sem,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return s(varname,index,ref,nlppp);
}

/********************************************
* FN:		G
* CR:		05/13/00 AM.
* SUBJ:	NLP++ variable, compiled runtime.
********************************************/

RFASem *Arun::g(
	_TCHAR *varname,
	long index,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
RFASem *sem = 0;
Ivar::eval(GLOBALVAR,varname,0,index,ref,nlppp,/*UP*/sem);
return sem;
}

// VARIANT
RFASem *Arun::g(																// 05/04/01 AM.
	_TCHAR *varname,
	RFASem *index_sem,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return g(varname,index,ref,nlppp);
}


/********************************************
* FN:		L
* CR:		03/09/02 AM.
* SUBJ:	NLP++ variable, compiled runtime.
********************************************/

RFASem *Arun::l(
	_TCHAR *varname,
	long index,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
RFASem *sem = 0;
Ivar::eval(LOCALVAR,varname,0,index,ref,nlppp,/*UP*/sem);
return sem;
}

// VARIANT
RFASem *Arun::l(																// 03/09/02 AM.
	_TCHAR *varname,
	RFASem *index_sem,
	bool ref,																	// 06/16/02 AM.
	Nlppp *nlppp
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return l(varname,index,ref,nlppp);
}


// NEW SYNTAX.																	// 04/28/01 AM.

RFASem *Arun::n(
	int num,
	Nlppp *nlppp
	)
{
Node<Pn> *pnode = 0;
if (!Ipnode::eval(RUNNODEPNODE,num,nlppp,/*UP*/pnode)) // FIX	// 04/29/01 AM.
	{
//	strstream gerrStr(Errbuf,MAXSTR,ios::out);
//	gerrStr << "[n(): Couldn't fetch node.]" << ends;
//	nlppp->parse_->errOut(false);
	return 0;
	}
if (!pnode)
	return 0;
RFASem *sem = new RFASem(pnode);
return sem;
}


RFASem *Arun::x(
	int num,
	Nlppp *nlppp
	)
{
Node<Pn> *pnode = 0;
if (!Ipnode::eval(CONTEXTPNODE, num, nlppp, /*UP*/ pnode))
	{
//	strstream gerrStr(Errbuf,MAXSTR,ios::out);
//	gerrStr << "[x(): Couldn't fetch node.]" << ends;
//	nlppp->parse_->errOut(false);
	return 0;
	}
if (!pnode)
	return 0;
RFASem *sem = new RFASem(pnode);
return sem;
}


RFASem *Arun::s(
	Nlppp *nlppp
	)
{
Node<Pn> *pnode = 0;
if (!Ipnode::eval(SUGGESTEDPNODE, 0, nlppp, /*UP*/ pnode))
	{
//	strstream gerrStr(Errbuf,MAXSTR,ios::out);
//	gerrStr << "[s(): Couldn't fetch node.]" << ends;
//	nlppp->parse_->errOut(false);
	return 0;
	}
if (!pnode)
	return 0;
RFASem *sem = new RFASem(pnode);
return sem;
}


/********************************************
* FN:		ASSIGN
* CR:		05/13/00 AM.
* SUBJ:	NLP++ assignment statement, compiled runtime.
* NOTE:	setVal trashes the old values.
*			Should have an RFASem copy constructor...
*			03/11/02 AM. Old value is cleared by setVal.
*			03/11/02 AM. Handling indexed assignment.
********************************************/

RFASem *Arun::assign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	RFASem *val
	)
{
if (index >= 0)																// 03/11/02 AM.
	return iassign(typ,varname,nelt,index,nlppp,val);				// 03/11/02 AM.

Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.				// 04/29/01 AM.
	return val;																	// 06/05/00 AM.

// Case of NULL val.												// FIX.	// 04/28/01 AM.
if (!val)															// FIX.	// 04/28/01 AM.
	{
	Var::setVal(pair, (long) 0);								// FIX.	// 04/28/01 AM.
	return 0;														// FIX.	// 04/28/01 AM.
	}

// Need to copy sem object for some types.							// 05/26/00 AM.
// Code is taken from iexpr::eval()
RFASem *nval = 0;
switch(val->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:		// "numeric" string. old stuff.
		Var::setVal(pair, val->getName());
		break;
	case RSLONG:
		Var::setVal(pair, val->getLong());
		break;
	case RSFLOAT:																// 08/16/01 AM.
		Var::setVal(pair, val->getFloat());								// 08/16/01 AM.
		break;																	// 08/16/01 AM.
	case RSOSTREAM:
		Var::setVal(pair, val->getOstream());
		break;
	case RS_KBCONCEPT:
		CONCEPT *conc;															// 11/15/00 AM.
		if (conc = val->getKBconcept())						// FIX.	// 11/15/00 AM.
			nval = new RFASem(conc,RS_KBCONCEPT);
		Var::setVal(pair, nval);
		break;
	case RS_KBPHRASE:
		nval = new RFASem(val->getKBphrase(), RS_KBPHRASE);
		Var::setVal(pair, nval);
		break;
	case RS_KBATTR:
		nval = new RFASem(val->getKBattr(), RS_KBATTR);
		Var::setVal(pair, nval);
		break;
	case RS_KBVAL:
		nval = new RFASem(val->getKBval(), RS_KBVAL);
		Var::setVal(pair, nval);
		break;
	case RSNODE:																// 10/30/00 AM.
		nval = new RFASem(val->getNode());								// 10/30/00 AM.
		Var::setVal(pair,nval);												// 10/30/00 AM.
		break;																	// 10/30/00 AM.
	case RSARGS:																// 12/04/01 AM.
		{
		// COPYING ARG LIST.										// FIX.	// 03/11/02 AM.
		Dlist<Iarg> *vls = Iarg::copy_args(val->getArgs());// FIX// 08/10/02 AM.
		// Installing args in variable.						// FIX.	// 08/10/02 AM.
		Var::setVal(pair,vls);									// FIX.	// 08/10/02 AM.
		}
		break;
	default:
		break;
	}

return val;
}

long Arun::assign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	long val)
{
if (index >= 0)																// 03/11/02 AM.
	return iassign(typ,varname,nelt,index,nlppp,val);				// 03/11/02 AM.

Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return val;																	// 06/05/00 AM.

// Note: this also trashes the old values.
Var::setVal(pair, val);
return val;
}

float Arun::assign(															// 08/18/01 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	float val)
{
if (index >= 0)																// 03/11/02 AM.
	return iassign(typ,varname,nelt,index,nlppp,val);				// 03/11/02 AM.

Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return val;

// Note: this also trashes the old values.
Var::setVal(pair, val);
return val;
}


_TCHAR *Arun::assign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	_TCHAR *val)
{
if (index >= 0)																// 03/11/02 AM.
	return iassign(typ,varname,nelt,index,nlppp,val);				// 03/11/02 AM.

Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return val;																	// 06/05/00 AM.

// Note: this also trashes the old values.
Var::setVal(pair, val);
return val;
}

_t_ostream *Arun::assign(														// 11/20/02 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	_t_ostream *val)
{
if (index >= 0)
	return iassign(typ,varname,nelt,index,nlppp,val);

Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return val;

// Note: this also trashes the old values.
Var::setVal(pair, val);
return val;
}

bool Arun::assign(															// 12/10/02 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	bool val)
{
if (index >= 0)
	return iassign(typ,varname,nelt,index,nlppp,val);

Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return val;

// Note: this also trashes the old values.
Var::setVal(pair, (val ? (long)1 : (long)0));	// No boolean vars yet.
return val;
}

// VARIANTS OF ASSIGN.

RFASem *Arun::assign(														// 05/04/01 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp,
	RFASem *val
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return assign(typ,varname,nelt,index,nlppp,val);
}

long Arun::assign(															// 05/04/01 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp,
	long val
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return assign(typ,varname,nelt,index,nlppp,val);
}


float Arun::assign(															// 08/18/01 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp,
	float val
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return assign(typ,varname,nelt,index,nlppp,val);
}


_TCHAR *Arun::assign(															// 05/04/01 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp,
	_TCHAR *val
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return assign(typ,varname,nelt,index,nlppp,val);
}

_t_ostream *Arun::assign(														// 11/20/02 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp,
	_t_ostream *val
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return assign(typ,varname,nelt,index,nlppp,val);
}

bool Arun::assign(															// 12/10/02 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp,
	bool val
	)
{
if (!index_sem)
	return 0;
bool flag = false;
long index = index_sem->sem_to_long(flag);
delete index_sem;
if (!flag)
	return 0;
return assign(typ,varname,nelt,index,nlppp,val);
}

/********************************************
* FN:		IASSIGN
* CR:		03/11/02 AM.
* SUBJ:	INDEXED NLP++ assignment statement, compiled runtime.
* NOTE:	
********************************************/

RFASem *Arun::iassign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	RFASem *val
	)
{
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.
	return val;

Delt<Iarg> *last=0;
Dlist<Iarg> *vals = pair->getVals();

if (!vals)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: No values for var='") << varname << _T("']") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return val;
	}
Iarg *arg = Iexpr::getVarindex(vals,index);

if (!val)
	return 0;	// Assign-to place is already nulled, so done.

RFASem *nval = 0;
long numx;
switch(val->getType())
	{
	case RSLONG:
		arg->setType(IANUM);
		arg->setNum(val->getLong());
		break;
	case RSFLOAT:
		arg->setType(IAFLOAT);
		arg->setFloat(val->getFloat());
		break;
	case RSNUM:	// Bastard num (was converted to str).
		// Convert to num and assign.
		arg->setType(IANUM);
		if (!str_to_long(val->getName(), numx))
			return val;
		arg->setNum(numx);
		break;
	case RSSTR:
	case RSNAME:
		arg->setType(IASTR);
		arg->setStr(val->getName());
		break;
	case RS_KBCONCEPT:
		arg->setType(IASEM);
		CONCEPT *conc;
		if (conc = val->getKBconcept())
			nval = new RFASem(conc,RS_KBCONCEPT);
		arg->setSem(nval);
		break;
	case RS_KBPHRASE:
		arg->setType(IASEM);
		nval = new RFASem(val->getKBphrase(), RS_KBPHRASE);
		arg->setSem(nval);
		break;
	case RS_KBATTR:
		arg->setType(IASEM);
		nval = new RFASem(val->getKBattr(), RS_KBATTR);
		arg->setSem(nval);
		break;
	case RS_KBVAL:
		arg->setType(IASEM);
		nval = new RFASem(val->getKBval(), RS_KBVAL);
		arg->setSem(nval);
		break;
	case RSOSTREAM:
		arg->setType(IASEM);
		nval = new RFASem(val->getOstream());
		arg->setSem(nval);
		break;
	case RSARGS:	// ARRAY VALUES (uncopied).
		// Should not be here.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Copying array -- Error.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		break;
	case RSNODE:
		arg->setType(IASEM);
		nval = new RFASem(val->getNode());
		arg->setSem(nval);
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Bad rhs type in assignment.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		break;
	}

return val;
}

long Arun::iassign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	long val)
{
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.
	return val;

Delt<Iarg> *last=0;
Dlist<Iarg> *vals = pair->getVals();

if (!vals)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: No values for var='") << varname << _T("']") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return val;
	}
Iarg *arg = Iexpr::getVarindex(vals,index);

arg->setType(IANUM);
arg->setNum(val);

return val;
}


float Arun::iassign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	float val)
{
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.
	return val;

Delt<Iarg> *last=0;
Dlist<Iarg> *vals = pair->getVals();

if (!vals)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: No values for var='") << varname << _T("']") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return val;
	}
Iarg *arg = Iexpr::getVarindex(vals,index);

arg->setType(IAFLOAT);
arg->setFloat(val);

return val;
}


_TCHAR *Arun::iassign(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	_TCHAR *val)
{
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.
	return val;

Delt<Iarg> *last=0;
Dlist<Iarg> *vals = pair->getVals();

if (!vals)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: No values for var='") << varname << _T("']") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return val;
	}
Iarg *arg = Iexpr::getVarindex(vals,index);

arg->setType(IASTR);
arg->setStr(val);

return val;
}

_t_ostream *Arun::iassign(										// 11/20/02 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	_t_ostream *val)
{
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.
	return val;

Delt<Iarg> *last=0;
Dlist<Iarg> *vals = pair->getVals();

if (!vals)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: No values for var='") << varname << _T("']") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return val;
	}
Iarg *arg = Iexpr::getVarindex(vals,index);

arg->setType(IAOSTREAM);
arg->setOstream(val);

return val;
}

bool Arun::iassign(															// 12/10/02 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,
	Nlppp *nlppp,
	bool val)
{
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	// Todo: Looks like error should be flagged here.
	return val;

Delt<Iarg> *last=0;
Dlist<Iarg> *vals = pair->getVals();

if (!vals)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: No values for var='") << varname << _T("']") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return val;
	}
Iarg *arg = Iexpr::getVarindex(vals,index);

arg->setType(IANUM);
arg->setNum((val ? 1 : 0));

return val;
}


/********************************************
* FN:		TRUTH
* CR:		05/13/00 AM.
* SUBJ:	NLP++ exprs, compiled runtime.
* NOTE:	Truth of a condition, such as in an if-else statement.
*			05/27/00 AM. Delete sem structure here.
********************************************/

bool Arun::truth(long val)
{
return (val ? true : false);
}

bool Arun::truth(float val)
{
return (val ? true : false);
}

bool Arun::truth(_TCHAR *val)
{
return ((val && *val) ? true : false);
}

bool Arun::truth(bool flg)													// 09/03/01 AM.
{
return flg;																		// 09/03/01 AM.
}

bool Arun::truth(RFASem *sem)
{
if (!sem)
	return false;																// 05/27/00 AM.
bool flag = sem->truth();
delete sem;		// No one using this sem beyond this point.		// 05/27/00 AM.
return flag;
}


/********************************************
* FN:		PLUS
* CR:		05/13/00 AM.
* SUBJ:	NLP++ '+' operator, compiled runtime.
* NOTE:	08/17/01 AM. Handling float also.
********************************************/

RFASem *Arun::plus(RFASem *sem1, long num, Nlppp *nlppp)
{
RFASem *sem=0;																	// 08/17/01 AM.

if (sem1)																		// 03/13/02 AM.
switch (sem1->getType())													// 08/17/01 AM.
	{
	case RSLONG:																// 08/17/01 AM.
		{
		long res = sem1->getLong() + num;
		delete sem1;				// Memory leak.						// 05/27/00 AM.
		sem = new RFASem(res);												// 08/17/01 AM.
		}
		break;																	// 08/17/01 AM.
	case RSFLOAT:																// 08/17/01 AM.
		{
		float res = sem1->getFloat() + (float)num;					// 08/17/01 AM.
		delete sem1;															// 08/17/01 AM.
		sem = new RFASem(res);												// 08/17/01 AM.
		}
		break;																	// 08/17/01 AM.
	case RSSTR:														// FIX.	// 07/11/03 AM.
		if (num == 0)												// FIX.	// 07/11/03 AM.
			{
			_TCHAR *nm = sem1->getName();						// FIX.	// 07/11/03 AM.
			delete sem1;											// FIX.	// 07/11/03 AM.
			if (nm)													// FIX.	// 07/11/03 AM.
				return new RFASem(nm,RSSTR);					// FIX.	// 07/11/03 AM.
			else
				return new RFASem((long)0);			// NULL STR.	// 07/11/03 AM.
			}
		// FALL THROUGH!!														// 07/11/03 AM.
	default:																		// 08/17/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in plus(sem,long)]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		delete sem1;				// Memory leak.						// 05/27/00 AM.
		return 0;
	}
else if (num == 0)	// NULL-NULL case.					// FIX.	// 07/11/03 AM.
	return new RFASem((long)0);								// FIX.	// 07/11/03 AM.
else																				// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Empty value in plus(sem,long)]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);																// 03/13/02 AM.
	sem = new RFASem(num);													// 03/13/02 AM.
	}
return sem;																		// 08/17/01 AM.
}

// VARIANT.																		// 08/17/01 AM.
float Arun::plus(RFASem *sem1, float fnum, Nlppp *nlppp)			// 08/17/01 AM.
{
float res = 0;
if (sem1)																		// 03/13/02 AM.
	{
	switch (sem1->getType())
		{
		case RSLONG:
			res = ((float) sem1->getLong()) + fnum;
			break;
		case RSFLOAT:
			res = sem1->getFloat() + fnum;
			break;
		default:
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad sem type in plus(sem,float)]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete sem1;
			return 0;
		}
	delete sem1;
	}
else																				// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Empty value in plus(sem,flt)]") << ends;	// 03/13/02 AM.
	nlppp->parse_->errOut(&gerrStr,false);																// 03/13/02 AM.
	return fnum;																// 03/13/02 AM.
	}
return res;
}


_TCHAR *Arun::plus(RFASem *sem, _TCHAR *str, Nlppp *nlppp)
{
if (!sem)																		// 05/21/01 AM.
	return str;																	// 05/21/01 AM.
if (!nlppp)																		// 05/21/01 AM.
	return 0;																	// 05/21/01 AM.

_TCHAR *semstr=0;
// Zero is acceptable as "empty string".
if (sem->getType() == RSLONG && sem->getLong() == 0)
	semstr = _T("");
else if (sem->getType() != RSSTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in plus(sem,str)]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	delete sem;				// Memory leak.								// 05/27/00 AM.
	return 0;
	}
else
	semstr = sem->getName();
if (!str || !*str)												// FIX.	// 02/15/04 AM.
	str = _T("");														// FIX.	// 02/15/04 AM.
_TCHAR *newstr;
_TCHAR *buf = Chars::create(_tcsclen(semstr)+_tcsclen(str)	// FIX.	// 07/16/03 AM.
														+1);			// FIX.	// 07/16/03 AM.
_stprintf(buf,_T("%s%s"),semstr,str);
nlppp->getParse()->internStr(buf,/*UP*/newstr);
Chars::destroy(buf);												// FIX.	// 07/16/03 AM.
delete sem;				// Memory leak.									// 05/27/00 AM.
return newstr;
}


RFASem *Arun::plus(RFASem *sem1, RFASem *sem2, Nlppp *nlppp)
{
RFASem *sem=0;				// New sem.

if (!sem1 || !sem2)															// 05/21/01 AM.
	{
	if (sem1)																	// 05/21/01 AM.
		delete sem1;															// 05/21/01 AM.
	if (sem2)																	// 05/21/01 AM.
		delete sem2;															// 05/21/01 AM.
	return 0;																	// 05/21/01 AM.
	}

bool ok = true;
long num;													// FIX			// 08/17/01 AM.
float fnum;																		// 08/17/01 AM.
_TCHAR *str;
//char buf[MAXSTR];
switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/17/01 AM.
			{
			case RSLONG:														// 08/17/01 AM.
				num = sem1->getLong() + sem2->getLong();
				sem = new RFASem(num);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			case RSFLOAT:														// 08/17/01 AM.
				fnum = ((float) sem1->getLong())							// 08/17/01 AM.
						 + sem2->getFloat();									// 08/17/01 AM.
				sem = new RFASem(fnum);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			case RSSTR:												// FIX.	// 07/11/03 AM.
				if (sem1->getLong() == 0)						// FIX.	// 07/11/03 AM.
					sem = new RFASem(sem2->getName());		// FIX.	// 07/11/03 AM.
				else													// FIX.	// 07/11/03 AM.
					ok = false;										// FIX.	// 07/11/03 AM.
				break;															// 07/11/03 AM.
			default:																// 08/17/01 AM.
				ok = false;
				break;															// 08/17/01 AM.
			}
		break;
	case RSFLOAT:																// 08/17/01 AM.
		switch (sem2->getType())											// 08/17/01 AM.
			{
			case RSLONG:														// 08/17/01 AM.
				fnum = sem1->getFloat()										// 08/17/01 AM.
						+ ((float) sem2->getLong());						// 08/17/01 AM.
				sem = new RFASem(fnum);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			case RSFLOAT:														// 08/17/01 AM.
				fnum = sem1->getFloat() + sem2->getFloat();			// 08/17/01 AM.
				sem = new RFASem(fnum);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			default:																// 08/17/01 AM.
				ok = false;														// 08/17/01 AM.
				break;															// 08/17/01 AM.
			}
		break;
	case RSSTR:
		switch (sem2->getType())											// 07/11/03 AM.
			{
			case RSSTR:															// 07/11/03 AM.
				{
				_TCHAR *s1 = sem1->getName();								// 07/16/03 AM.
				_TCHAR *s2 = sem2->getName();								// 07/16/03 AM.
				if (!s1 || !*s1)									// FIX.	// 02/15/04 AM.
					s1 = _T("");											// FIX.	// 02/15/04 AM.
				if (!s2 || !*s2)									// FIX.	// 02/15/04 AM.
					s2 = _T("");											// FIX.	// 02/15/04 AM.
				_TCHAR *buf = Chars::create(_tcsclen(s1)+_tcsclen(s2)		// 07/16/03 AM.
																		+ 1);		// 07/16/03 AM.
				_stprintf(buf,_T("%s%s"),s1, s2);								// 07/16/03 AM.
				nlppp->getParse()->internStr(buf,/*UP*/str);
				Chars::destroy(buf);											// 07/16/03 AM.
				sem = new RFASem(str, RSSTR);
				}
				break;
			case RSLONG:														// 07/11/03 AM.
				if (sem2->getLong() == 0)						// FIX.	// 07/11/03 AM.
					sem = new RFASem(sem1->getName());		// FIX.	// 07/11/03 AM.
				else
					ok = false;										// FIX.	// 07/11/03 AM.
				break;
			default:																// 07/11/03 AM.
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '+' operation.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	}


delete sem1;				// Memory leak.								// 05/27/00 AM.
delete sem2;				// Memory leak.								// 05/27/00 AM.
return sem;
}

_TCHAR *Arun::plus(_TCHAR *str1, _TCHAR *str2, Nlppp *nlppp)
{
_TCHAR *newstr;
if (!str1 || !*str1)												// FIX.	// 02/15/04 AM.
	str1 = _T("");														// FIX.	// 02/15/04 AM.
if (!str2 || !*str2)												// FIX.	// 02/15/04 AM.
	str2 = _T("");														// FIX.	// 02/15/04 AM.
_TCHAR *buf = Chars::create(_tcsclen(str1)+_tcsclen(str2)+1);			// 07/16/03 AM.
_stprintf(buf,_T("%s%s"),str1,str2);
nlppp->getParse()->internStr(buf,/*UP*/newstr);
Chars::destroy(buf);															// 07/16/03 AM.
return newstr;
}

_TCHAR *Arun::plus(_TCHAR*str1, long num, Nlppp *nlppp)				// 07/11/03 AM.
{
// Assume string is interned.
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad type in plus(str,num)]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return 0;
	}
return str1;
}

_TCHAR *Arun::plus(long num, _TCHAR *str1, Nlppp *nlppp)				// 07/11/03 AM.
{
// Assume string is interned.
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad type in plus(str,num)]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return 0;
	}
return str1;
}

_TCHAR *Arun::plus(_TCHAR *str, RFASem *sem, Nlppp *nlppp)
{
_TCHAR *semstr=0;
// Zero is acceptable as "empty string".
if (!sem)																		// 03/13/02 AM.
	semstr = _T("");																// 03/13/02 AM.
else if (sem->getType() == RSLONG && sem->getLong() == 0)
	semstr = _T("");
else if (sem->getType() != RSSTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in plus(str,sem)]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	delete sem;				// Memory leak.								// 05/27/00 AM.
	return 0;
	}
else
	semstr = sem->getName();
_TCHAR *newstr;
if (!str || !*str)												// FIX.	// 02/15/04 AM.
	str = _T("");														// FIX.	// 02/15/04 AM.
if (!semstr || !*semstr)										// FIX.	// 02/15/04 AM.
	semstr = _T("");													// FIX.	// 02/15/04 AM.
_TCHAR *buf = Chars::create(_tcsclen(str)+_tcsclen(semstr)+1);			// 07/16/03 AM.
_stprintf(buf,_T("%s%s"),str,semstr);
nlppp->getParse()->internStr(buf,/*UP*/newstr);
Chars::destroy(buf);															// 07/16/03 AM.
delete sem;				// Memory leak.									// 05/27/00 AM.
return newstr;
}

long Arun::plus(long num1, long num2, Nlppp *nlppp)
{
return num1 + num2;
}

RFASem *Arun::plus(long num, RFASem *sem2, Nlppp *nlppp)
{
RFASem *sem = 0;																// 08/17/01 AM.

if (sem2)																		// 03/13/02 AM.
	{
	switch (sem2->getType())												// 08/17/01 AM.
		{
		case RSLONG:															// 08/17/01 AM.
			{
			long res = sem2->getLong() + num;
			sem = new RFASem(res);											// 08/17/01 AM.
			}
			break;																// 08/17/01 AM.
		case RSFLOAT:															// 08/17/01 AM.
			{
			float res = sem2->getFloat() + (float)num;				// 08/17/01 AM.
			sem = new RFASem(res);											// 08/17/01 AM.
			}
			break;																// 08/17/01 AM.
		case RSSTR:													// FIX.	// 07/11/03 AM.
			if (num == 0)											// FIX.	// 07/11/03 AM.
				{
				_TCHAR *st = sem2->getName();					// FIX.	// 07/11/03 AM.
				delete sem2;										// FIX.	// 07/11/03 AM.
				if (st && *st)										// FIX.	// 07/11/03 AM.
					return new RFASem(st,RSSTR);				// FIX.	// 07/11/03 AM.
				else
					return new RFASem((long)0);				// FIX.	// 07/11/03 AM.
				}
			// FALL THROUGH.													// 07/11/03 AM.
		default:																	// 08/17/01 AM.
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad sem type in plus(long,sem)]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete sem2;				// Memory leak.					// 05/27/00 AM.
			return 0;
		}
	delete sem2;				// Memory leak.							// 05/27/00 AM.
	}
else
	{
	sem = new RFASem(num);													// 03/13/02 AM.
	}
return sem;																		// 08/17/01 AM.
}


float Arun::plus(float fnum, RFASem *sem2, Nlppp *nlppp)			// 08/17/01 AM.
{
RFASem *sem = 0;

float res;
if (sem2)																		// 03/13/02 AM.
	{
	switch (sem2->getType())
		{
		case RSLONG:
			res = ((float) sem2->getLong()) + fnum;
			break;
		case RSFLOAT:
			res = sem2->getFloat() + fnum;
			break;
		default:
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad sem type in plus(float,sem)]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete sem2;
			return 0;
		}
	delete sem2;
	}
else																				// 03/13/02 AM.
	{
	return fnum;																// 03/13/02 AM.
	}
return res;
}

float Arun::plus(float num1, long num2, Nlppp *nlppp)				// 08/17/01 AM.
{
return num1 + (float)num2;
}

float Arun::plus(long num1, float num2, Nlppp *nlppp)				// 08/17/01 AM.
{
return (float)num1 + num2;
}

float Arun::plus(float num1, float num2, Nlppp *nlppp)			// 08/17/01 AM.
{
return num1 + num2;
}


/********************************************
* FN:		PLUS
* CR:		06/02/00 AM.
* SUBJ:	NLP++ '+' operator, compiled runtime.
* NOTE:	UNARY.
********************************************/

long Arun::plus(long num, Nlppp *nlppp)
{
return num;	// no-op.
}

float Arun::plus(float fnum, Nlppp *nlppp)							// 08/18/01 AM.
{
return fnum; // no-op;
}

RFASem *Arun::plus(RFASem *sem1, Nlppp *nlppp)
{
bool ok = false;																// 04/28/01 AM.
RFASem *sem=0;
if (sem1)																		// 03/13/02 AM.
	{
	switch (sem1->getType())												// 08/18/01 AM.
		{
		case RSLONG:
			{
			long num = sem1->sem_to_long(ok);							// 04/28/01 AM.
			sem = new RFASem(num);
			}
			break;
		case RSFLOAT:
			{
			float fnum = sem1->sem_to_float(ok);
			sem = new RFASem(fnum);
			}
			break;
		default:
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad sem type in plus(sem)]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete sem1;
			return 0;
		}
	delete sem1;											// MEM LEAK.	// 06/12/00 AM.
	}
else																				// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Empty val in plus(sem)]") << ends;			// 03/13/02 AM.
	nlppp->parse_->errOut(&gerrStr,false);																// 03/13/02 AM.
	}
return sem;	// "no-op"
}


/********************************************
* FN:		OUT
* CR:		05/13/00 AM.
* SUBJ:	NLP++ output operator, compiled runtime.
********************************************/

_t_ostream *Arun::out(_TCHAR *fname, RFASem *sem, Nlppp *nlppp)
{
Ipair *pair;
_t_ostream *ostr = 0;
bool exists,open,inuse,badname;
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
if (!Var::filevar(fname,nlppp->getParse(),
			/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[out: Error. Couldn't make var=")
			<< fname << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/12/00 AM.
	return 0;
	}
if (ostr)																		// 08/04/02 AM.
	sem->out(ostr);
delete sem;				// No one using sem past this point.		// 05/27/00 AM.
return ostr;
}

_t_ostream *Arun::out(_TCHAR *fname, _TCHAR *str, Nlppp *nlppp)
{
Ipair *pair;
_t_ostream *ostr = 0;
bool exists,open,inuse,badname;
if (!Var::filevar(fname,nlppp->getParse(),
			/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[out: Error. Couldn't make var=")
			<< fname << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return 0;
	}
if (str && *str																// 04/30/01 AM.
	&& ostr)																		// 08/04/02 AM.
	*ostr << str;
return ostr;
}

_t_ostream *Arun::out(_TCHAR *fname, long num, Nlppp *nlppp)
{
Ipair *pair;
_t_ostream *ostr = 0;
bool exists,open,inuse,badname;
if (!Var::filevar(fname,nlppp->getParse(),
			/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[out: Error. Couldn't make var=")
			<< fname << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return 0;
	}
*ostr << num;
return ostr;
}


_t_ostream *Arun::out(_TCHAR *fname, bool flag, Nlppp *nlppp)			// 07/11/03 AM.
{
Ipair *pair;
_t_ostream *ostr = 0;
bool exists,open,inuse,badname;
if (!Var::filevar(fname,nlppp->getParse(),
			/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[out: Error. Couldn't make var=")
			<< fname << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return 0;
	}
*ostr << (flag ? 1 : 0);
return ostr;
}



_t_ostream *Arun::out(_TCHAR *fname, float num, Nlppp *nlppp)			// 08/19/01 AM.
{
Ipair *pair;
_t_ostream *ostr = 0;
bool exists,open,inuse,badname;
if (!Var::filevar(fname,nlppp->getParse(),
			/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[out: Error. Couldn't make var=")
			<< fname << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return 0;
	}
if (ostr)																		// 08/04/02 AM.
	*ostr << num;
return ostr;
}

_t_ostream *Arun::out(RFASem *ostrsem, RFASem *sem, Nlppp *nlppp)
{
if (!ostrsem)
	{
	if (sem)																		// 08/19/01 AM.
		delete sem;
	return 0;
	}

// PRINT ACCORDING TO ITS TYPE.
_t_ostream *ostr=0;
_TCHAR *str;
switch(ostrsem->getType())													// 08/07/02 AM.
	{
	case RSOSTREAM:															// 08/07/02 AM.
		ostr = ostrsem->getOstream();										// 08/19/01 AM.
		break;
	case RSVAR:																	// 08/07/02 AM.
		// If it's an ostream val, use that.
		// If it's a string val, see if global ostream exists.
		// If not, warn user to use fileout, then make the var!
		// Other vals are error.
//		ruleError(line_,nlppp);
//		nlppp->line_ = Linenum = line_;
		{
		_t_strstream gerrStr;
		gerrStr << _T("[unimplemented RSVAR in output op.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		if (sem)
			delete sem;
		delete ostrsem;
		return 0;
	case RSSTR:																	// 08/07/02 AM.
		str = ostrsem->getName();
		if (!str || !*str)
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Error. Can't output to empty file.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			if (sem)
				delete sem;
			delete ostrsem;
			return 0;

			}
		Ipair *pair;
		bool exists,open,inuse,badname;
		if (!Var::filevar(str,nlppp->getParse(),
					/*UP*/ pair,ostr,exists,open,inuse,badname))
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Output op: Error. Couldn't make var=")
					  << str << _T("]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			if (sem)
				delete sem;
			delete ostrsem;
			return 0;
			}
		break;
	case RSNAME:
	default:																		// 08/07/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Out: bad output stream.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		if (sem)
			delete sem;
		delete ostrsem;
		return 0;
		}
	}

if (!sem)																		// 08/19/01 AM.
	{
	delete ostrsem;															// 08/19/01 AM.
	return ostr;		// RECOVER.											// 08/19/01 AM.
	}

if (ostr)																		// 08/04/02 AM.
	sem->out(ostr);	// PRINT IT.										// 08/19/01 AM. 

delete ostrsem;																// 08/19/01 AM.
delete sem;				// No one using sem past this point.		// 05/27/00 AM.
return ostr;																	// 08/19/01 AM.
}

_t_ostream *Arun::out(RFASem *ostrsem, _TCHAR *str, Nlppp *nlppp)
{
if (!ostrsem)
	return 0;

// PRINT ACCORDING TO ITS TYPE.
_t_ostream *ostr=0;
_TCHAR *fstr;
switch(ostrsem->getType())													// 08/07/02 AM.
	{
	case RSOSTREAM:															// 08/07/02 AM.
		ostr = ostrsem->getOstream();										// 08/19/01 AM.
		break;
	case RSVAR:																	// 08/07/02 AM.
		// If it's an ostream val, use that.
		// If it's a string val, see if global ostream exists.
		// If not, warn user to use fileout, then make the var!
		// Other vals are error.
//		ruleError(line_,nlppp);
//		nlppp->line_ = Linenum = line_;
		{
		_t_strstream gerrStr;
		gerrStr << _T("[unimplemented RSVAR in output op.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		delete ostrsem;
		return 0;
	case RSSTR:																	// 08/07/02 AM.
		fstr = ostrsem->getName();
		if (!fstr || !*fstr)
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Error. Can't output to empty file.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;

			}
		Ipair *pair;
		bool exists,open,inuse,badname;
		if (!Var::filevar(fstr,nlppp->getParse(),
					/*UP*/ pair,ostr,exists,open,inuse,badname))
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Output op: Error. Couldn't make var=")
					  << fstr << _T("]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;
			}
		break;
	case RSNAME:
	default:																		// 08/07/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Out: bad output stream.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		delete ostrsem;
		return 0;
		}
	}

if (!str || !*str)															// 08/19/01 AM.
	{
	delete ostrsem;															// 08/19/01 AM.
	return ostr;		// RECOVER.											// 08/19/01 AM.
	}

if (ostr)																		// 08/04/02 AM.
	*ostr << str;	// PRINT IT.											// 08/19/01 AM. 

delete ostrsem;																// 08/19/01 AM.
return ostr;																	// 08/19/01 AM.
}

_t_ostream *Arun::out(RFASem *ostrsem, long num, Nlppp *nlppp)
{
if (!ostrsem)
	return 0;

// PRINT ACCORDING TO ITS TYPE.
_t_ostream *ostr=0;
_TCHAR *str;
switch(ostrsem->getType())													// 08/07/02 AM.
	{
	case RSOSTREAM:															// 08/07/02 AM.
		ostr = ostrsem->getOstream();										// 08/19/01 AM.
		break;
	case RSVAR:																	// 08/07/02 AM.
		// If it's an ostream val, use that.
		// If it's a string val, see if global ostream exists.
		// If not, warn user to use fileout, then make the var!
		// Other vals are error.
//		ruleError(line_,nlppp);
//		nlppp->line_ = Linenum = line_;
		{
		_t_strstream gerrStr;
		gerrStr << _T("[unimplemented RSVAR in output op.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		delete ostrsem;
		return 0;
	case RSSTR:																	// 08/07/02 AM.
		str = ostrsem->getName();
		if (!str || !*str)
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Error. Can't output to empty file.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;

			}
		Ipair *pair;
		bool exists,open,inuse,badname;
		if (!Var::filevar(str,nlppp->getParse(),
					/*UP*/ pair,ostr,exists,open,inuse,badname))
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Output op: Error. Couldn't make var=")
					  << str << _T("]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;
			}
		break;
	case RSNAME:
	default:																		// 08/07/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Out: bad output stream.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		delete ostrsem;
		return 0;
		}
	}

if (ostr)																		// 08/04/02 AM.
	*ostr << num;	// PRINT IT.											// 08/19/01 AM. 

delete ostrsem;																// 08/19/01 AM.
return ostr;																	// 08/19/01 AM.
}


// VARIANT.																		// 07/11/03 AM.
_t_ostream *Arun::out(RFASem *ostrsem, bool flag, Nlppp *nlppp)	// 07/11/03 AM.
{
if (!ostrsem)
	return 0;

// PRINT ACCORDING TO ITS TYPE.
_t_ostream *ostr=0;
_TCHAR *str;
switch(ostrsem->getType())
	{
	case RSOSTREAM:
		ostr = ostrsem->getOstream();
		break;
	case RSVAR:
		// If it's an ostream val, use that,
		// If it's a string val, see if global ostream exists.
		// If not, warn user to use fileout, then make the var!
		// Other vals are error.
//		ruleError(line_,nlppp);
//		nlppp->line_ = Linenum = line_;
		{
		_t_strstream gerrStr;
		gerrStr << _T("[unimplemented RSVAR in output op.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		delete ostrsem;
		return 0;
	case RSSTR:
		str = ostrsem->getName();
		if (!str || !*str)
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Error. Can't output to empty file.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;

			}
		Ipair *pair;
		bool exists,open,inuse,badname;
		if (!Var::filevar(str,nlppp->getParse(),
					/*UP*/ pair,ostr,exists,open,inuse,badname))
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Output op: Error. Couldn't make var=")
					  << str << _T("]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;
			}
		break;
	case RSNAME:
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Out: bad output stream.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		delete ostrsem;
		return 0;
		}
	}

if (ostr)
	*ostr << (flag ? 1 : 0);	// PRINT IT.

delete ostrsem;
return ostr;
}


_t_ostream *Arun::out(RFASem *ostrsem, float num, Nlppp *nlppp)	// 08/19/01 AM.
{
if (!ostrsem)
	return 0;

// PRINT ACCORDING TO ITS TYPE.
_t_ostream *ostr=0;
_TCHAR *str;
switch(ostrsem->getType())													// 08/07/02 AM.
	{
	case RSOSTREAM:															// 08/07/02 AM.
		ostr = ostrsem->getOstream();										// 08/19/01 AM.
		break;
	case RSVAR:																	// 08/07/02 AM.
		// If it's an ostream val, use that.
		// If it's a string val, see if global ostream exists.
		// If not, warn user to use fileout, then make the var!
		// Other vals are error.
//		ruleError(line_,nlppp);
//		nlppp->line_ = Linenum = line_;
		{
		_t_strstream gerrStr;
		gerrStr << _T("[unimplemented RSVAR in output op.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
		delete ostrsem;
		return 0;
	case RSSTR:																	// 08/07/02 AM.
		str = ostrsem->getName();
		if (!str || !*str)
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Error. Can't output to empty file.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;

			}
		Ipair *pair;
		bool exists,open,inuse,badname;
		if (!Var::filevar(str,nlppp->getParse(),
					/*UP*/ pair,ostr,exists,open,inuse,badname))
			{
//			ruleError(line_,nlppp);
//			nlppp->line_ = Linenum = line_;
			_t_strstream gerrStr;
			gerrStr << _T("[Output op: Error. Couldn't make var=")
					  << str << _T("]") << ends;
			nlppp->parse_->errOut(&gerrStr,false);
			delete ostrsem;
			return 0;
			}
		break;
	case RSNAME:
	default:																		// 08/07/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Out: bad output stream.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		delete ostrsem;
		return 0;
		}
	}

if (ostr)																		// 08/04/02 AM.
	*ostr << num;	// PRINT IT.											// 08/19/01 AM. 

delete ostrsem;																// 08/19/01 AM.
return ostr;																	// 08/19/01 AM.
}


_t_ostream *Arun::out(_t_ostream *ostr, RFASem *sem, Nlppp *nlppp)	// 08/19/01 AM.
{
if (!sem)
	return ostr;

if (ostr)																		// 08/04/02 AM.
	sem->out(ostr);	// PRINT IT.
delete sem;
return ostr;
}

_t_ostream *Arun::out(_t_ostream *ostr, _TCHAR *str, Nlppp *nlppp)
{
if (str && *str																// 04/30/01 AM.
		  && ostr)																// 08/04/02 AM.
	*ostr << str;
return ostr;
}

_t_ostream *Arun::out(_t_ostream *ostr, long num, Nlppp *nlppp)
{
if (ostr)																		// 08/04/02 AM.
	*ostr << num;
return ostr;
}

_t_ostream *Arun::out(_t_ostream *ostr, bool flag, Nlppp *nlppp)		// 07/11/03 AM.
{
if (ostr)
	*ostr << (flag ? 1 : 0);
return ostr;
}

_t_ostream *Arun::out(_t_ostream *ostr, float num, Nlppp *nlppp)		// 08/19/01 AM.
{
if (ostr)																		// 08/04/02 AM.
	*ostr << num;
return ostr;
}


/********************************************
* FN:		STMT
* CR:		05/27/00 AM.
* SUBJ:	Cleanups at statement level.
* NOTE:	06/06/00 AM.	For CHECK actions, having stmt return boolean.
*			08/18/01 AM. Looks obsolete to do this computation, since check
*			region now uses succeed and fail special functions.
********************************************/

bool Arun::stmt(RFASem *sem)
{
if (!sem)
	return false;
bool res = truth(sem);	// THIS DELETES SEM!.			// FIX.	// 06/07/00 AM.
//if (sem)
//	delete sem;
return res;
}

bool Arun::stmt(_t_ostream *ostr)
{
return true;
}


bool Arun::stmt(long num)
{
return num ? true : false;
}

bool Arun::stmt(float num)													// 08/19/01 AM.
{
return num ? true : false;
}


bool Arun::stmt(_TCHAR *str)
{
return (str && *str);
}

bool Arun::stmt(bool flg)													// 09/03/01 AM.
{
return flg;																		// 09/03/01 AM.
}


/********************************************
* FN:		MINUS
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '-' operator, compiled runtime.
********************************************/

RFASem *Arun::minus(RFASem *sem1, long num)
{
RFASem *sem=0;																	// 08/17/01 AM.
if (!sem1)																		// 03/13/02 AM.
	{
	sem = new RFASem(- num);												// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}
switch (sem1->getType())													// 08/17/01 AM.
	{
	case RSLONG:																// 08/17/01 AM.
		{
		long res = sem1->getLong() - num;
		sem = new RFASem(res);												// 08/17/01 AM.
		}
		break;
	case RSFLOAT:																// 08/17/01 AM.
		{
		float res = sem1->getFloat() - num;								// 08/17/01 AM.
		sem = new RFASem(res);												// 08/17/01 AM.
		}
		break;
	default:																		// 08/17/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in minus(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
delete sem1;
return sem;																		// 08/17/01 AM.
}

float Arun::minus(RFASem *sem1, float fnum)							// 08/17/01 AM.
{
float res=0.0;
if (!sem1)																		// 03/13/02 AM.
	{
	return - fnum;																// 03/13/02 AM.
	}
switch (sem1->getType())
	{
	case RSLONG:
		res = ((float)sem1->getLong()) - fnum;
		break;
	case RSFLOAT:
		res = sem1->getFloat() - fnum;
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in minus(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
delete sem1;
return res;
}


RFASem *Arun::minus(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;															// FIX	// 08/17/01 AM.
float fnum;																		// 08/17/01 AM.
RFASem *sem=0;																	// 08/17/01 AM.
if (!sem1 && !sem2)															// 03/13/02 AM.
	{
	sem = new RFASem((float)0.0);											// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}
else if (!sem1)																// 03/13/02 AM.
	{
	switch (sem2->getType())												// 03/13/02 AM.
		{
		case RSLONG:															// 03/13/02 AM.
			num = - sem2->getLong();										// 03/13/02 AM.
			sem = new RFASem(num);											// 03/13/02 AM.
			break;
		case RSFLOAT:															// 03/13/02 AM.
			fnum = - (float) sem2->getFloat();							// 03/13/02 AM.
			sem = new RFASem(fnum);											// 03/13/02 AM.
			break;
		case RSNUM:																// 03/13/02 AM.
			if (!str_to_long(sem2->getName(), num))					// 03/13/02 AM.
				{
				num = 0;															// 03/13/02 AM.
				ok = false;														// 03/13/02 AM.
				}
			sem = new RFASem(num);											// 03/13/02 AM.
			break;
		default:																	// 03/13/02 AM.
			break;
		}
	delete sem2;																// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}
else if (!sem2)																// 03/13/02 AM.
	{
	switch (sem1->getType())												// 03/13/02 AM.
		{
		case RSLONG:															// 03/13/02 AM.
			num = - sem1->getLong();										// 03/13/02 AM.
			sem = new RFASem(num);											// 03/13/02 AM.
			break;
		case RSFLOAT:															// 03/13/02 AM.
			fnum = - (float) sem1->getFloat();							// 03/13/02 AM.
			sem = new RFASem(fnum);											// 03/13/02 AM.
			break;
		case RSNUM:																// 03/13/02 AM.
			if (!str_to_long(sem1->getName(), num))					// 03/13/02 AM.
				{
				num = 0;															// 03/13/02 AM.
				ok = false;														// 03/13/02 AM.
				}
			sem = new RFASem(num);											// 03/13/02 AM.
			break;
		default:																	// 03/13/02 AM.
			break;
		}
	delete sem1;																// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/17/01 AM.
			{
			case RSLONG:														// 08/17/01 AM.
				num = sem1->getLong() - sem2->getLong();
				sem = new RFASem(num);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			case RSFLOAT:														// 08/17/01 AM.
				fnum = (float)sem1->getLong() - sem2->getFloat();	// 08/17/01 AM.
				sem = new RFASem(fnum);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			default:																// 08/17/01 AM.
				ok = false;														// 08/17/01 AM.
				break;
			}
		break;
	case RSFLOAT:																// 08/17/01 AM.
		switch (sem2->getType())											// 08/17/01 AM.
			{
			case RSLONG:														// 08/17/01 AM.
				fnum = sem1->getFloat() - (float)sem2->getLong();	// 08/17/01 AM.
				sem = new RFASem(fnum);										// 08/17/01 AM.
				break;															// 08/17/01 AM.
			case RSFLOAT:														// 08/17/01 AM.
				fnum = sem1->getFloat() - sem2->getFloat();			// 08/17/01 AM.
				break;															// 08/17/01 AM.
			default:																// 08/17/01 AM.
				ok = false;														// 08/17/01 AM.
				break;															// 08/17/01 AM.
			}
		break;																	// 08/17/01 AM.
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '-' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return sem;																		// 08/17/01 AM.
}


long Arun::minus(long num1, long num2)
{
return num1 - num2;
}

RFASem *Arun::minus(long num, RFASem *sem2)
{
RFASem *sem=0;																	// 08/17/01 AM.
switch (sem2->getType())													// 08/17/01 AM.
	{
	case RSLONG:																// 08/17/01 AM.
		{
		long res = num - sem2->getLong();
		sem = new RFASem(res);												// 08/17/01 AM.
		}
		break;
	case RSFLOAT:																// 08/17/01 AM.
		{
		float res = (float)num - sem2->getFloat();					// 08/17/01 AM.
		sem = new RFASem(res);												// 08/17/01 AM.
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in minus(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem2;
		return 0;
	}
delete sem2;
return sem;
}

float Arun::minus(float fnum, RFASem *sem2)							// 08/17/01 AM.
{
float res = 0.0;
if (!sem2)																		// 03/13/02 AM.
	{
	return fnum;																// 03/13/02 AM.
	}
switch (sem2->getType())
	{
	case RSLONG:
		res = fnum - (float) sem2->getLong();
		break;
	case RSFLOAT:
		res = fnum - sem2->getFloat();
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in minus(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem2;
		return 0;
	}
delete sem2;
return res;
}

float Arun::minus(float fnum1, long num2)
{
return fnum1 - (float) num2;
}

float Arun::minus(long num1, float fnum2)
{
return (float) num1 - fnum2;
}

float Arun::minus(float fnum1, float fnum2)
{
return fnum1 - fnum2;
}

/********************************************
* FN:		MINUS
* CR:		06/02/00 AM.
* SUBJ:	NLP++ '-' operator, compiled runtime.
* NOTE:	Unary.
********************************************/

long Arun::minus(long num)
{
return -num;
}

float Arun::minus(float fnum)												// 08/18/01 AM.
{
return -fnum;
}

RFASem *Arun::minus(RFASem *sem1)
{
RFASem *sem=0;																	// 08/18/01 AM.
bool ok = true;																// 04/28/01 AM.
if (!sem1)																		// 03/13/02 AM.
	{
	sem = new RFASem((long)0);												// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}

switch (sem1->getType())													// 08/18/01 AM.
	{
	case RSLONG:
		{
		long num = sem1->sem_to_long(ok);								// 04/28/01 AM.
		sem = new RFASem(-num);
		}
		break;
	case RSFLOAT:
		{
		float fnum = sem1->sem_to_float(ok);
		sem = new RFASem(-fnum);
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in minus(sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
delete sem1;												// MEM LEAK.	// 06/12/00 AM.
return sem;
}


/********************************************
* FN:		TIMES
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '*' operator, compiled runtime.
********************************************/

RFASem *Arun::times(RFASem *sem1, long num)							// 08/17/01 AM.
{
RFASem *sem=0;
if (!sem1)																		// 03/13/02 AM.
	{
	sem = new RFASem((long)0);												// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}
switch (sem1->getType())
	{
	case RSLONG:
		{
		long res = sem1->getLong() * num;
		sem = new RFASem(res);
		}
		break;
	case RSFLOAT:
		{
		float res = sem1->getFloat() * (float) num;
		sem = new RFASem(res);
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in times(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
delete sem1;
return sem;
}

float Arun::times(RFASem *sem1, float fnum)							// 08/17/01 AM.
{
float res=0.0;
if (!sem1)																		// 03/13/02 AM.
	return (float)0.0;														// 03/13/02 AM.

switch (sem1->getType())
	{
	case RSLONG:
		res = (float)sem1->getLong() * fnum;
		break;
	case RSFLOAT:
		res = sem1->getFloat() * fnum;
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in times(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
delete sem1;
return res;
}


RFASem *Arun::times(RFASem *sem1, RFASem *sem2)
{
RFASem *sem=0;
bool ok = true;

if (sem1 && sem2)																// 03/13/02 AM.
	{
 switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())
			{
			case RSLONG:
				{
				long num = sem1->getLong() * sem2->getLong();
				sem = new RFASem(num);
				}
				break;
			case RSFLOAT:
				{
				float fnum = (float)sem1->getLong() * sem2->getFloat();
				sem = new RFASem(fnum);
				}
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:
		switch (sem2->getType())
			{
			case RSLONG:
				{
				float fnum = sem1->getFloat() * (float)sem2->getLong();
				sem = new RFASem(fnum);
				}
				break;
			case RSFLOAT:
				{
				float fnum = sem1->getFloat() * sem2->getFloat();
				sem = new RFASem(fnum);
				}
				break;
			default:
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}
	delete sem1;																// 03/13/02 AM.
	delete sem2;																// 03/13/02 AM.
	}
else if (sem1)																	// 03/13/02 AM.
	{
	delete sem1;																// 03/13/02 AM.
	ok = false;																	// 03/13/02 AM.
	}
else if (sem2)																	// 03/13/02 AM.
	{
	delete sem2;																// 03/13/02 AM.
	ok = false;																	// 03/13/02 AM.
	}
else
	ok = false;																	// 03/13/02 AM.

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '*' operation.]") << ends;
	errOut(&gerrStr,false);
	sem = new RFASem((long) 0);	// Recover.							// 03/13/02 AM.
	}

return sem;
}


long Arun::times(long num1, long num2)
{
return num1 * num2;
}

RFASem *Arun::times(long num, RFASem *sem2)
{
RFASem *sem =0;
if (!sem2)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem2->getType())
	{
	case RSLONG:
		{
		long res = num * sem2->getLong();
		sem = new RFASem(res);
		}
		break;
	case RSFLOAT:
		{
		float res = (float)num * sem2->getFloat();
		sem = new RFASem(res);
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in times(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem2;
		return 0;
	}
delete sem2;
return sem;
}

float Arun::times(float fnum, RFASem *sem2)
{
float res=0.0;
if (!sem2)																		// 03/13/02 AM.
	return (float)0.0;														// 03/13/02 AM.

switch (sem2->getType())
	{
	case RSLONG:
		res = fnum * (float) sem2->getLong();
		break;
	case RSFLOAT:
		res = fnum * sem2->getFloat();
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in times(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem2;
		return 0;
	}
delete sem2;
return res;
}

float Arun::times(float fnum1, long num2)
{
return fnum1 * (float) num2;
}

float Arun::times(long num1, float fnum2)
{
return (float) num1 * fnum2;
}

float Arun::times(float fnum1, float fnum2)
{
return fnum1 * fnum2;
}

/********************************************
* FN:		DIVIDE
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '/' operator, compiled runtime.
********************************************/

RFASem *Arun::divide(RFASem *sem1, long num)
{
RFASem *sem=0;
if (!num)																		// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Divide by zero in divide(sem,long)]") << ends;
	errOut(&gerrStr,false);																// 03/13/02 AM.
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}
if (!sem1)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem1->getType())
	{
	case RSLONG:
		{
		long res = sem1->getLong() / num;
		sem = new RFASem(res);
		}
		break;
	case RSFLOAT:
		{
		float res = sem1->getFloat() / (float) num;
		sem = new RFASem(res);
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in divide(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
delete sem1;
return sem;
}

float Arun::divide(RFASem *sem1, float fnum)
{
// NEED A FLOAT DIVIDE BY ZERO TEST.									// 03/13/02 AM.
// NEED OVERFLOW TESTS FOR MATH FNS...									// 03/13/02 AM.
//if (abs(fnum - 0.0000003) <= 0.0)										// 03/13/02 AM.
#ifdef LINUX
if (fnum <= 0.0000003 && fnum  >= -0.0000003) // 04/26/07 AM.
#else
if ((abs(fnum) - 0.0000003) <= 0.0)										// 06/11/03 AM.
#endif
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Divide by zero in divide(sem,flt)]") << ends;
	errOut(&gerrStr,false);																// 03/13/02 AM.
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	return 0.0;																	// 03/13/02 AM.
	}
if (!sem1)																		// 03/13/02 AM.
	return 0.0;																	// 03/13/02 AM.

switch (sem1->getType())
	{
	case RSLONG:
		{
		float res = (float)sem1->getLong() / fnum;
		delete sem1;
		return res;
		}
		break;
	case RSFLOAT:
		{
		float res = sem1->getFloat() / fnum;
		delete sem1;
		return res;
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in divide(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem1;
		return 0;
	}
}

RFASem *Arun::divide(RFASem *sem1, RFASem *sem2)
{
RFASem *sem=0;
bool ok = true;
// Need better checking here...											// 03/13/02 AM.
if (!sem2)																		// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Divide by zero in divide(sem,sem)]") << ends;
	errOut(&gerrStr,false);																// 03/13/02 AM.
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	sem = new RFASem((long)0);												// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}
if (!sem1)																		// 03/13/02 AM.
	{
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	sem = new RFASem((long)0);												// 03/13/02 AM.
	return sem;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())
			{
			case RSLONG:
				{
				long num = sem1->getLong() / sem2->getLong();
				sem = new RFASem(num);
				}
				break;
			case RSFLOAT:
				{
				float fnum = (float)sem1->getLong() / sem2->getFloat();
				sem = new RFASem(fnum);
				}
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:
		switch (sem2->getType())
			{
			case RSLONG:
				{
				float fnum = sem1->getFloat() / (float) sem2->getLong();
				sem = new RFASem(fnum);
				}
				break;
			case RSFLOAT:
				{
				float fnum = sem1->getFloat() / sem2->getFloat();
				sem = new RFASem(fnum);
				}
				break;
			default:
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '/' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return sem;
}


long Arun::divide(long num1, long num2)
{
return num1 / num2;
}

RFASem *Arun::divide(long num, RFASem *sem2)
{
RFASem *sem=0;
if (!sem2)																		// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Divide by zero in divide(num,sem)]") << ends;
	errOut(&gerrStr,false);																// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem2->getType())
	{
	case RSLONG:
		{
		long res = num / sem2->getLong();
		sem = new RFASem(res);
		}
		break;
	case RSFLOAT:
		{
		float res = (float)num / sem2->getFloat();
		sem = new RFASem(res);
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in divide(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem2;
		return 0;
	}
delete sem2;
return sem;
}

float Arun::divide(float fnum, RFASem *sem2)
{
if (!sem2)																		// 03/13/02 AM.
	{
	_t_strstream gerrStr;						// 03/13/02 AM.
	gerrStr << _T("[Error: Divide by zero in divide(fnum,sem)]") << ends;
	errOut(&gerrStr,false);																// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}
switch (sem2->getType())
	{
	case RSLONG:
		{
		float res = fnum / (float) sem2->getLong();
		delete sem2;
		return res;
		}
		break;
	case RSFLOAT:
		{
		float res = fnum / sem2->getFloat();
		delete sem2;
		return res;
		}
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in divide(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem2;
		return 0;
	}
delete sem2;
return 0.0;
}

float Arun::divide(float fnum1, long num2)
{
return fnum1 / (float) num2;
}

float Arun::divide(long num1, float fnum2)
{
return (float) num1 / fnum2;
}

float Arun::divide(float fnum1, float fnum2)
{
return fnum1 / fnum2;
}

/********************************************
* FN:		REM
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '%' operator, compiled runtime.
********************************************/

long Arun::rem(RFASem *sem, long num)
{
if (!sem)																		// 03/13/02 AM.
	{
	return 0;																	// 03/13/02 AM.
	}

if (sem->getType() != RSLONG)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in rem(sem,long)]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}
long res = sem->getLong() % num;
delete sem;
return res;
}


long Arun::rem(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;															// FIX	// 08/17/01 AM.
if (!sem2 || !sem1)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		if (sem2->getType() != RSLONG)
			{
			ok = false;
			break;
			}
		num = sem1->getLong() % sem2->getLong();
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '%' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::rem(long num1, long num2)
{
return num1 % num2;
}

long Arun::rem(long num, RFASem *sem)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
if (sem->getType() != RSLONG)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in rem(long,sem)]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}
long res = num % sem->getLong();
delete sem;
return res;
}


/********************************************
* FN:		CONF
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '%%' operator, compiled runtime.
********************************************/

long Arun::conf(RFASem *sem, long num)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

if (sem->getType() != RSLONG)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in conf(sem,long)]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}
long res = confidence(sem->getLong(), num);
delete sem;
return res;
}


long Arun::conf(RFASem *sem1, RFASem *sem2)
{
RFASem *sem=0;				// New sem.
bool ok = true;
long num;															// FIX	// 08/17/01 AM.
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		if (sem2->getType() != RSLONG)
			{
			ok = false;
			break;
			}
		num = confidence(sem1->getLong(), sem2->getLong());
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '%%' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::conf(long num1, long num2)
{
return confidence(num1, num2);
}

long Arun::conf(long num, RFASem *sem)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

if (sem->getType() != RSLONG)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in conf(long,sem)]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}
long res = confidence(num, sem->getLong());
delete sem;
return res;
}


/********************************************
* FN:		EQ
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '==' operator, compiled runtime.
********************************************/

long Arun::eq(RFASem *sem, long num)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return (num ? 0 : 1);										// FIX.	// 12/12/02 AM.
//	return 0;																	// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (sem->getLong() == num);
		delete sem;																// 12/12/02 AM.
		return res;																// 12/12/02 AM.
		break;
	case RSFLOAT:
		res = (sem->getFloat() == num);									// 08/18/01 AM.
		delete sem;																// 12/12/02 AM.
		return res;																// 12/12/02 AM.
		break;
	default:
		break;
	}

if (num)	// If not 0.														// 12/12/02 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in eq(sem,long)]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}


switch (sem->getType())														// 12/12/02 AM.
	{
	case RSSTR:																	// 12/12/02 AM.
	case RSNAME:																// 12/12/02 AM.
	case RSNUM:																	// 12/12/02 AM.
		{
		_TCHAR *str = sem->getName();										// 12/12/02 AM.
		res = (!str || !*str);												// 12/12/02 AM.
		}
		break;																	// 12/12/02 AM.
	case RS_KBCONCEPT:														// 12/12/02 AM.
		res = (sem->getKBconcept() ? 0 : 1);							// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RS_KBPHRASE:															// 12/12/02 AM.
		res = (sem->getKBphrase() ? 0 : 1);								// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RS_KBATTR:															// 12/12/02 AM.
		res = (sem->getKBattr() ? 0 : 1);								// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RS_KBVAL:																// 12/12/02 AM.
		res = (sem->getKBval() ? 0 : 1);									// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RSNODE:																// 12/12/02 AM.
		res = (sem->getNode() ? 0 : 1);									// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RSARGS:																// 12/12/02 AM.
		res = (sem->getArgs() ? 0 : 1);									// 12/12/02 AM.
		// Recurse...?															// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	default:																		// 12/12/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in eq(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
		}
	}

delete sem;
return res;
}

long Arun::eq(RFASem *sem, float num)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (sem->getLong() == num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() == num);
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in eq(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


long Arun::eq(RFASem *sem, _TCHAR *str)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
		break;
	case RSLONG:	// Zeroed out long is ok.							// 06/06/00 AM.
		if (sem->getLong())
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad sem type in eq(sem,str)]") << ends;
			errOut(&gerrStr,false);
			delete sem;
			return 0;
			}
		delete sem;
		if (!str || !*str)
			return 1;
		return 0;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in eq(sem,str)]") << ends;
		errOut(&gerrStr,false);
		}

		delete sem;
		return 0;
	}
long res = eq(sem->getName(), str);
delete sem;
return res;
}

long Arun::eq(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;															// FIX	// 08/17/01 AM.
if (!sem1 && !sem2)												// FIX.	// 07/04/06 AM.
	return 1;																	// 07/04/06 AM.
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		return eq(sem1,(long)0);	// Deletes sem.		// FIX.	// 07/04/06 AM.
//		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		return eq(sem2,(long)0);	// Deletes sem.		// FIX.	// 07/04/06 AM.
//		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/18/01 AM.
			{
			case RSLONG:
				num = (sem1->getLong() == sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getLong() == sem2->getFloat());			// 08/18/01 AM.
				break;
			case RSSTR:															// 08/08/02 AM.
			case RSNAME:														// 08/08/02 AM.
			case RSNUM:		// ??												// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getName() ? 0 : 1);						// 08/08/02 AM.
				break;
			case RS_KBCONCEPT:												// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBconcept() ? 0 : 1);				// 08/08/02 AM.
				break;
			case RS_KBPHRASE:													// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBphrase() ? 0 : 1);				// 08/08/02 AM.
				break;
			case RS_KBATTR:													// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBattr() ? 0 : 1);					// 08/08/02 AM.
				break;
			case RS_KBVAL:														// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBval() ? 0 : 1);					// 08/08/02 AM.
				break;
			case RSNODE:														// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getNode() ? 0 : 1);						// 08/08/02 AM.
				break;
			case RSARGS:														// 08/08/02 AM.
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:																// 08/18/01 AM.
		switch (sem2->getType())
			{
			case RSLONG:
				num = (sem1->getFloat() == sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getFloat() == sem2->getFloat());
				break;
			default:
				ok = false;
			}
	case RSSTR:
	case RSNAME:
	case RSNUM:		// Should be number...?								// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RSSTR:															// 08/08/02 AM.
			case RSNAME:														// 08/08/02 AM.
			case RSNUM:		// ??												// 08/08/02 AM.
				num = eq(sem1->getName(),sem2->getName());
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else
					num = (sem1->getName() ? 0 :1);						// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;															// 08/08/02 AM.
			}
		break;
	case RS_KBCONCEPT:														// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBCONCEPT:												// 08/08/02 AM.
				num = (sem1->getKBconcept() ==							// 08/08/02 AM.
						  sem2->getKBconcept() );							// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else
					num = (sem1->getKBconcept() ? 0 :1);				// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBPHRASE:															// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBPHRASE:													// 08/08/02 AM.
				num = (sem1->getKBphrase() ==								// 08/08/02 AM.
						  sem2->getKBphrase() );							// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else
					num = (sem1->getKBphrase() ? 0 :1);					// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBATTR:															// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBATTR:													// 08/08/02 AM.
				num = (sem1->getKBattr() ==								// 08/08/02 AM.
						  sem2->getKBattr() );								// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else
					num = (sem1->getKBattr() ? 0 :1);					// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBVAL:																// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBVAL:														// 08/08/02 AM.
				num = (sem1->getKBval() ==									// 08/08/02 AM.
						  sem2->getKBval() );								// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else
					num = (sem1->getKBval() ? 0 :1);						// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RSNODE:																// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RSNODE:														// 08/08/02 AM.
				num = (sem1->getNode() ==									// 08/08/02 AM.
						  sem2->getNode() );									// 08/08/02 AM.
				break;												// FIX.	// 08/12/02 AM.
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else
					num = (sem1->getNode() ? 0 :1);						// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RSARGS:																// 08/08/02 AM.
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '==' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::eq(long num1, long num2)
{
return num1 == num2;
}


long Arun::eq(long num, RFASem *sem)
{
return eq(sem,num);					// Deletes sem.					// 12/12/02 AM.
}


long Arun::eq(long num, _TCHAR *str1)										// 07/11/03 AM.
{
if (num != 0)
	return 0;
return empty(str1) ? 1 : 0;
}


long Arun::eq(_TCHAR *str1, long num)										// 07/11/03 AM.
{
if (num != 0)
	return 0;
return empty(str1) ? 1 : 0;
}


long Arun::eq(float num, RFASem *sem)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (num == sem->getLong());
		break;
	case RSFLOAT:
		res = (num == sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in eq(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::eq(_TCHAR *str1, _TCHAR *str2)
{
if (str1 && *str1 && str2 && *str2)
	return !_tcscmp(str1,str2) ? true : false;
if ((str1 && *str1) || (str2 && *str2))
	return false;			// Exactly one is empty.
return true;				// Both are empty.
}


long Arun::eq(_TCHAR *str, RFASem *sem)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
		break;
	case RSLONG:	// Zeroed out long is ok.							// 06/06/00 AM.
		if (sem->getLong())
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Num and str type in '==']") << ends;
			errOut(&gerrStr,false);
			delete sem;
			return 0;
			}
		delete sem;
		if (!str || !*str)
			return 1;
		return 0;
	case RSARGS:																// 08/08/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Multivalued variable in '==']") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
		}	
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Incompatible type with str in '==']") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
		}
	}
long res = eq(sem->getName(), str);
delete sem;
return res;
}


long Arun::eq(long num1, float num2)
{
return num1 == num2;
}

long Arun::eq(float num1, long num2)
{
return num1 == num2;
}

long Arun::eq(float num1, float num2)
{
return num1 == num2;
}


/********************************************
* FN:		NE
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '!=' operator, compiled runtime.
* NOTE:	Should return bool...	12/12/02 AM.
********************************************/

long Arun::ne(RFASem *sem, long num)
{
long res;
if (!sem)																		// 03/13/02 AM.
	{
	return (num ? 1 : 0);										// FIX.	// 12/12/02 AM.
//	return 1;	// Dummy recovery.										// 03/13/02 AM.
	}

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (sem->getLong() != num);
		delete sem;																// 12/12/02 AM.
		return res;																// 12/12/02 AM.
		break;
	case RSFLOAT:
		res = (sem->getFloat() != num);									// 08/18/01 AM.
		delete sem;																// 12/12/02 AM.
		return res;																// 12/12/02 AM.
		break;
	default:
		break;
	}

if (num)	// If not 0.														// 12/12/02 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad sem type in ne(sem,long)]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}

switch (sem->getType())														// 12/12/02 AM.
	{
	case RSSTR:																	// 12/12/02 AM.
	case RSNAME:																// 12/12/02 AM.
	case RSNUM:																	// 12/12/02 AM.
		{
		_TCHAR *str = sem->getName();										// 12/12/02 AM.
		res = (str && *str);													// 12/12/02 AM.
		}
		break;																	// 12/12/02 AM.
	case RS_KBCONCEPT:														// 12/12/02 AM.
		res = (sem->getKBconcept() ? 1 : 0);							// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RS_KBPHRASE:															// 12/12/02 AM.
		res = (sem->getKBphrase() ? 1 : 0);								// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RS_KBATTR:															// 12/12/02 AM.
		res = (sem->getKBattr() ? 1 : 0);								// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RS_KBVAL:																// 12/12/02 AM.
		res = (sem->getKBval() ? 1 : 0);									// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RSNODE:																// 12/12/02 AM.
		res = (sem->getNode() ? 1 : 0);									// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	case RSARGS:																// 12/12/02 AM.
		res = (sem->getArgs() ? 1 : 0);									// 12/12/02 AM.
		// Recurse...?															// 12/12/02 AM.
		break;																	// 12/12/02 AM.
	default:																		// 12/12/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ne(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
		}
	}

delete sem;
return res;
}

long Arun::ne(RFASem *sem, float num)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (sem->getLong() != num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() != num);
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ne(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


long Arun::ne(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;															// FIX	// 08/17/01 AM.
if (!sem1 && !sem2)												// FIX.	// 07/04/06 AM.
	return 0;																	// 07/04/06 AM.
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		return ne(sem1,(long)0);	// Deletes sem.		// FIX.	// 07/04/06 AM.
//		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		return ne(sem2,(long)0);	// Deletes sem.		// FIX.	// 07/04/06 AM.
//		delete sem2;															// 03/13/02 AM.
	return 1;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/18/01 AM.
			{
			case RSLONG:
				num = (sem1->getLong() != sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getLong() != sem2->getFloat());
				break;
			case RSSTR:															// 08/08/02 AM.
			case RSNAME:														// 08/08/02 AM.
			case RSNUM:		// ??												// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getName() ? 1 : 0);						// 08/08/02 AM.
				break;
			case RS_KBCONCEPT:												// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBconcept() ? 1 : 0);				// 08/08/02 AM.
				break;
			case RS_KBPHRASE:													// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBphrase() ? 1 : 0);				// 08/08/02 AM.
				break;
			case RS_KBATTR:													// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBattr() ? 1 : 0);					// 08/08/02 AM.
				break;
			case RS_KBVAL:														// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getKBval() ? 1 : 0);					// 08/08/02 AM.
				break;
			case RSNODE:														// 08/08/02 AM.
				if (sem1->getLong() != 0)									// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem2->getNode() ? 1 : 0);						// 08/08/02 AM.
				break;
			case RSARGS:														// 08/08/02 AM.
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:																// 08/18/01 AM.
		switch (sem2->getType())
			{
			case RSLONG:
				num = (sem1->getFloat() != sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getFloat() != sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSSTR:																	// 08/08/02 AM.
	case RSNAME:																// 08/08/02 AM.
	case RSNUM:				// Should be a number...??					// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RSSTR:															// 08/08/02 AM.
			case RSNAME:														// 08/08/02 AM.
			case RSNUM:		// ??												// 08/08/02 AM.
				num = ne(sem1->getName(),sem2->getName());			// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem1->getName() ? 1 : 0);						// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBCONCEPT:														// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBCONCEPT:												// 08/08/02 AM.
				num = (sem1->getKBconcept() !=							// 08/08/02 AM.
						  sem2->getKBconcept() );							// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem1->getKBconcept() ? 1 : 0);				// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBPHRASE:															// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBPHRASE:													// 08/08/02 AM.
				num = (sem1->getKBphrase() !=								// 08/08/02 AM.
						  sem2->getKBphrase() );							// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem1->getKBphrase() ? 1 : 0);				// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBATTR:															// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBATTR:													// 08/08/02 AM.
				num = (sem1->getKBattr() !=								// 08/08/02 AM.
						  sem2->getKBattr() );								// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem1->getKBattr() ? 1 : 0);					// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RS_KBVAL:																// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RS_KBVAL:														// 08/08/02 AM.
				num = (sem1->getKBval() !=									// 08/08/02 AM.
						  sem2->getKBval() );								// 08/08/02 AM.
				break;
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem1->getKBval() ? 1 : 0);					// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RSNODE:																// 08/08/02 AM.
		switch (sem2->getType())											// 08/08/02 AM.
			{
			case RSNODE:														// 08/08/02 AM.
				num = (sem1->getNode() !=									// 08/08/02 AM.
						  sem2->getNode() );									// 08/08/02 AM.
				break;												// FIX.	// 08/12/02 AM.
			case RSLONG:														// 08/08/02 AM.
				if (sem2->getLong())											// 08/08/02 AM.
					ok = false;													// 08/08/02 AM.
				else																// 08/08/02 AM.
					num = (sem1->getNode() ? 1 : 0);						// 08/08/02 AM.
				break;
			default:																// 08/08/02 AM.
				ok = false;														// 08/08/02 AM.
				break;
			}
		break;
	case RSARGS:																// 08/08/02 AM.
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '!=' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
if (num)																			// 08/08/02 AM.
	return 1;																	// 08/08/02 AM.
return 0;																		// 08/08/02 AM.
}


long Arun::ne(long num1, long num2)
{
return num1 != num2;
}

long Arun::ne(long num, RFASem *sem)
{
return ne(sem,num);			// Deletes sem.							// 12/12/02 AM.
}

long Arun::ne(float num, RFASem *sem)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (num != sem->getLong());
		break;
	case RSFLOAT:
		res = (num != sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ne(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


bool Arun::ne(_TCHAR *str1, _TCHAR *str2)
{
if (str1 && *str1 && str2 && *str2)
	return _tcscmp(str1,str2) ? true : false;		// FIX			// 06/08/00 AM.
if (str1 && *str1)
	return true;											// FIX.			// 06/08/00 AM.
if (str2 && *str2)
	return true;											// FIX.			// 06/08/00 AM.
return false;		// Both empty.						// FIX.			// 06/08/00 AM.
}


bool Arun::ne(RFASem *sem, _TCHAR *str)
{
return ne(str,sem);
}

bool Arun::ne(_TCHAR *str, RFASem *sem)
{
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
		break;
	case RSLONG:	// Zeroed out long is ok.
		if (sem->getLong())
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Error: Bad sem type in ne(sem,str)]") << ends;
			errOut(&gerrStr,false);
			delete sem;
			return 0;
			}
		delete sem;
		if (!str || !*str)
			return false;	// Both empty.
		return true;		// One empty, one not.
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ne(str,sem)]") << ends;
		errOut(&gerrStr,false);
		}

		delete sem;
		return 0;
	}
bool res = ne(sem->getName(), str);
delete sem;
return res;
}


long Arun::ne(float num1, long num2)
{
return num1 != num2;
}

long Arun::ne(long num1, float num2)
{
return num1 != num2;
}

long Arun::ne(float num1, float num2)
{
return num1 != num2;
}


long Arun::ne(long num, _TCHAR *str1)										// 07/11/03 AM.
{
if (num != 0)
	return 1;
return empty(str1) ? 0 : 1;
}


long Arun::ne(_TCHAR *str1, long num)										// 07/11/03 AM.
{
if (num != 0)
	return 1;
return empty(str1) ? 0 : 1;
}


/********************************************
* FN:		GT
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '>' operator, compiled runtime.
********************************************/

long Arun::gt(RFASem *sem, long num)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (sem->getLong() > num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() > num);									// 08/18/01 AM.
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in gt(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::gt(RFASem *sem, float num)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (sem->getLong() > num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() > num);
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in gt(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


long Arun::gt(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13 02 AM.
	return 1;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/18/01 AM.
			{
			case RSLONG:
				num = (sem1->getLong() > sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getLong() > sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:																// 08/18/01 AM.
		switch (sem2->getType())
			{
			case RSLONG:
				num = (sem1->getFloat() > sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getFloat() > sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '>' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::gt(long num1, long num2)
{
return num1 > num2;
}

long Arun::gt(long num, RFASem *sem)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (num > sem->getLong());
		break;
	case RSFLOAT:
		res = (num > sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in gt(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::gt(float num, RFASem *sem)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (num > sem->getLong());
		break;
	case RSFLOAT:
		res = (num > sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in gt(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::gt(float num1, long num2)
{
return num1 > num2;
}

long Arun::gt(long num1, float num2)
{
return num1 > num2;
}

long Arun::gt(float num1, float num2)
{
return num1 > num2;
}


/********************************************
* FN:		LT
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '<' operator, compiled runtime.
********************************************/

long Arun::lt(RFASem *sem, long num)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (sem->getLong() < num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() < num);									// 08/18/01 AM.
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in lt(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::lt(RFASem *sem, float num)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (sem->getLong() < num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() < num);
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in lt(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


long Arun::lt(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/18/01 AM.
			{
			case RSLONG:
				num = (sem1->getLong() < sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getLong() < sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:																// 08/18/01 AM.
		switch (sem2->getType())
			{
			case RSLONG:
				num = (sem1->getFloat() < sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getFloat() < sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '<' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::lt(long num1, long num2)
{
return num1 < num2;
}

long Arun::lt(long num, RFASem *sem)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (num < sem->getLong());
		break;
	case RSFLOAT:
		res = (num < sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in lt(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::lt(float num, RFASem *sem)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (num < sem->getLong());
		break;
	case RSFLOAT:
		res = (num < sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in lt(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::lt(float num1, long num2)
{
return num1 < num2;
}

long Arun::lt(long num1, float num2)
{
return num1 < num2;
}

long Arun::lt(float num1, float num2)
{
return num1 < num2;
}



/********************************************
* FN:		GE
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '>=' operator, compiled runtime.
********************************************/

long Arun::ge(RFASem *sem, long num)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (sem->getLong() >= num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() >= num);									// 08/18/01 AM.
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ge(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::ge(RFASem *sem, float num)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (sem->getLong() >= num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() >= num);
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ge(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


long Arun::ge(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/18/01 AM.
			{
			case RSLONG:
				num = (sem1->getLong() >= sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getLong() >= sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:																// 08/18/01 AM.
		switch (sem2->getType())
			{
			case RSLONG:
				num = (sem1->getFloat() >= sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getFloat() >= sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '>=' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::ge(long num1, long num2)
{
return num1 >= num2;
}

long Arun::ge(long num, RFASem *sem)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (num >= sem->getLong());
		break;
	case RSFLOAT:
		res = (num >= sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ge(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::ge(float num, RFASem *sem)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (num >= sem->getLong());
		break;
	case RSFLOAT:
		res = (num >= sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in ge(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::ge(float num1, long num2)
{
return num1 >= num2;
}

long Arun::ge(long num1, float num2)
{
return num1 >= num2;
}

long Arun::ge(float num1, float num2)
{
return num1 >= num2;
}



/********************************************
* FN:		LE
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '<=' operator, compiled runtime.
********************************************/

long Arun::le(RFASem *sem, long num)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (sem->getLong() <= num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() <= num);									// 08/18/01 AM.
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in le(sem,long)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::le(RFASem *sem, float num)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (sem->getLong() <= num);
		break;
	case RSFLOAT:
		res = (sem->getFloat() <= num);
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in le(sem,float)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}


long Arun::le(RFASem *sem1, RFASem *sem2)
{
bool ok = true;
long num;
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

switch (sem1->getType())
	{
	case RSLONG:
		switch (sem2->getType())											// 08/18/01 AM.
			{
			case RSLONG:
				num = (sem1->getLong() <= sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getLong() <= sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	case RSFLOAT:																// 08/18/01 AM.
		switch (sem2->getType())
			{
			case RSLONG:
				num = (sem1->getFloat() <= sem2->getLong());
				break;
			case RSFLOAT:
				num = (sem1->getFloat() <= sem2->getFloat());
				break;
			default:
				ok = false;
				break;
			}
		break;
	default:
		ok = false;
		break;
	}

if (!ok)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: Bad types in '<=' operation.]") << ends;
	errOut(&gerrStr,false);
	}

delete sem1;
delete sem2;
return num;
}


long Arun::le(long num1, long num2)
{
return num1 <= num2;
}

long Arun::le(long num, RFASem *sem)
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())														// 08/18/01 AM.
	{
	case RSLONG:
		res = (num <= sem->getLong());
		break;
	case RSFLOAT:
		res = (num <= sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in le(long,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::le(float num, RFASem *sem)									// 08/18/01 AM.
{
long res;
if (!sem)																		// 03/13/02 AM.
	return 1;	// Dummy recovery.										// 03/13/02 AM.

switch (sem->getType())
	{
	case RSLONG:
		res = (num <= sem->getLong());
		break;
	case RSFLOAT:
		res = (num <= sem->getFloat());
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in le(float,sem)]") << ends;
		errOut(&gerrStr,false);
		delete sem;
		return 0;
	}
delete sem;
return res;
}

long Arun::le(float num1, long num2)
{
return num1 <= num2;
}

long Arun::le(long num1, float num2)
{
return num1 <= num2;
}

long Arun::le(float num1, float num2)
{
return num1 <= num2;
}


/********************************************
* FN:		AND
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '&&' operator, compiled runtime.
* NOTE:	06/11/00 AM. Should take any "atomic" value types.
********************************************/

long Arun::vtand(RFASem *sem, long num)
{
if (!sem)																		// 03/13/02 AM.
	return 0;	// Dummy recovery.										// 03/13/02 AM.

if (!num)
	{
	delete sem;
	return 0;				// Quick fail.
	}

long res;
_TCHAR *str;
switch (sem->getType())													// 06/11/00 AM.
	{
	case RSLONG:
		res = sem->getLong();
		break;
	case RSFLOAT:																// 08/18/01 AM.
		res = (sem->getFloat() ? 1 : 0);									// 09/27/01 AM.
		break;																	// 08/18/01 AM.
	case RSSTR:
	case RSNAME:
		str = sem->getName();
		res = ((str && *str) ? 1 : 0);
		break;
	case RS_KBCONCEPT:
		res = (sem->getKBconcept() ? 1 : 0);
		break;
	case RS_KBPHRASE:
		res = (sem->getKBphrase() ? 1 : 0);
		break;
	case RS_KBATTR:
		res = (sem->getKBattr() ? 1 : 0);
		break;
	case RS_KBVAL:
		res = (sem->getKBval() ? 1 : 0);
		break;
	case RSNUM:				// Not allowing this.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Num as string in and(sem,long)]")	<< ends;
		errOut(&gerrStr,false);
		}

		res = 0;
		break;
	case RSNODE:																// 10/30/00 AM.
		res = (sem->getNode() ? 1 : 0);									// 10/30/00 AM.
		break;																	// 10/30/00 AM.
	case RSARGS:																// 12/04/01 AM.
		res = (sem->getArgs() ? 1 : 0);									// 12/04/01 AM.
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in and(sem,long)]") << ends;
		errOut(&gerrStr,false);
		}

		res = 0;
		break;
	}

delete sem;
return res;
}

long Arun::vtand(RFASem *sem1, float num2)								// 08/18/01 AM.
{
if (!sem1)																		// 03/13/02 AM.
	return 0;	// Dummy recovery.										// 03/13/02 AM.

if (!vtand(sem1, (long)1))		// Note: DELETES sem1.
	return 0;
if (!num2)
	return 0;
return 1;
}

// 06/11/00 AM. Overhaul.
long Arun::vtand(RFASem *sem1, RFASem *sem2)
{
if (!sem1 || !sem2)															// 03/13/02 AM.
	{
	if (sem1)																	// 03/13/02 AM.
		delete sem1;															// 03/13/02 AM.
	if (sem2)																	// 03/13/02 AM.
		delete sem2;															// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.
	}

if (!vtand(sem1, (long)1))		// Note: DELETES sem1.
	{
	delete sem2;
	return 0;
	}

return vtand(sem2, (long)1);		// Note: DELETES sem2.
}

long Arun::vtand(RFASem *sem, _TCHAR *str)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

// Better order of evaluation.											// 08/18/01 AM.
if (!vtand(sem, (long)1))	// Note: DELETES sem.
	return 0;
if (!str || !*str)
	return 0;
return 1;																		// 08/18/01 AM.
}

bool Arun::vtand(RFASem *sem, bool flg)									// 09/03/01 AM.
{
if (!sem)																		// 03/13/02 AM.
	return false;																// 03/13/02 AM.

if (!flg)
	{
	delete sem;
	return false;
	}
return vtand(sem, (long)1) ? true : false;		// Note: DELETES sem1.
}


long Arun::vtand(long num1, long num2)
{
return num1 && num2;
}

// OPT:	Could optimize by inlining.
long Arun::vtand(long num, RFASem *sem)
{
return vtand(sem, num);														// 06/11/00 AM.
}

long Arun::vtand(float num, RFASem *sem)									// 08/18/01 AM.
{
return vtand(sem, num);
}

long Arun::vtand(long num, _TCHAR *str)										// 08/18/01 AM.
{
return (num && str && *str);
}

bool Arun::vtand(long num, bool flg)										// 09/03/01 AM.
{
return num && flg;
}

long Arun::vtand(float num, _TCHAR *str)									// 08/18/01 AM.
{
return (num && str && *str);
}

long Arun::vtand(_TCHAR *str, RFASem *sem)
{
return vtand(sem, str);
}

long Arun::vtand(_TCHAR *str, long num)
{
return (str && *str && num);
}

long Arun::vtand(_TCHAR *str, float num)
{
return (str && *str && num);
}

long Arun::vtand(_TCHAR *str1, _TCHAR *str2)
{
return (str1 && *str1 && str2 && *str2);
}

long Arun::vtand(float num1, long num2)									// 08/18/01 AM.
{
return (num1 && num2);
}

bool Arun::vtand(float fnum, bool flg)									// 09/03/01 AM.
{
return fnum && flg;
}

long Arun::vtand(long num1, float num2)									// 08/18/01 AM.
{
return (num1 && num2);
}

long Arun::vtand(float num1, float num2)									// 08/18/01 AM.
{
return (num1 && num2);
}

bool Arun::vtand(_TCHAR *str, bool flg)										// 09/03/01 AM.
{
return (str && *str) && flg;
}

bool Arun::vtand(bool flg, RFASem *sem)									// 09/03/01 AM.
{
if (!flg)
	{
	delete sem;
	return false;
	}
return vtand(sem, (long)1) ? true : false;
}

bool Arun::vtand(bool flg, long num)										// 09/03/01 AM.
{
return flg && num;
}

bool Arun::vtand(bool flg, float fnum)									// 09/03/01 AM.
{
return flg && fnum;
}

bool Arun::vtand(bool flg, _TCHAR *str)										// 09/03/01 AM.
{
return flg && str && *str;
}

bool Arun::vtand(bool flg1, bool flg2)									// 09/03/01 AM.
{
return flg1 && flg2;
}


/********************************************
* FN:		OR
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '||' operator, compiled runtime.
********************************************/

// 06/11/00 AM. Overhaul.
long Arun::vtor(RFASem *sem, long num)
{
if (!sem)															// FIX.	// 04/28/01 AM.
	{
	// Assuming optional match possible here.							// 04/28/01 AM.
	return (num ? 1 : 0);													// 04/28/01 AM.
	}
if (num)
	{
	delete sem;
	return 1;				// Quick success.
	}

long res;
_TCHAR *str;
switch (sem->getType())														// 06/11/00 AM.
	{
	case RSLONG:
		res = sem->getLong();
		break;
	case RSFLOAT:																// 08/19/01 AM.
		res = (sem->getFloat() ? 1 : 0);									// 08/19/01 AM.
		break;																	// 08/19/01 AM.
	case RSSTR:
	case RSNAME:
		str = sem->getName();
		res = ((str && *str) ? 1 : 0);
		break;
	case RS_KBCONCEPT:
		res = (sem->getKBconcept() ? 1 : 0);
		break;
	case RS_KBPHRASE:
		res = (sem->getKBphrase() ? 1 : 0);
		break;
	case RS_KBATTR:
		res = (sem->getKBattr() ? 1 : 0);
		break;
	case RS_KBVAL:
		res = (sem->getKBval() ? 1 : 0);
		break;
	case RSNODE:																// 10/30/00 AM.
		res = (sem->getNode() ? 1 : 0);									// 10/30/00 AM.
		break;																	// 10/30/00 AM.
	case RSARGS:																// 12/04/01 AM.
		res = (sem->getArgs() ? 1 : 0);									// 12/04/01 AM.
		break;
	case RSNUM:				// Not allowing this.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Num as string in or(sem,long)]")	<< ends;
		errOut(&gerrStr,false);
		}

		res = 0;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Bad sem type in or(sem,long)]") << ends;
		errOut(&gerrStr,false);
		}

		res = 0;
		break;
	}

delete sem;
return res;
}

long Arun::vtor(RFASem *sem1, float num2)								// 08/19/01 AM.
{
if (vtor(sem1,(long)0))		// Note: DELETES sem1.
	return 1;
return (num2 ? 1 : 0);
}


long Arun::vtor(RFASem *sem1, RFASem *sem2)
{
if (vtor(sem1,(long)0))		// Note: DELETES sem1.
	{
	delete sem2;
	return 1;
	}

return vtor(sem2,(long)0);	// Note: DELETES sem2.
}

long Arun::vtor(RFASem *sem, _TCHAR *str)
{
if (str && *str)
	{
	delete sem;
	return 1;
	}

return vtor(sem,(long)0);	// Note: DELETES sem.
}

bool Arun::vtor(RFASem *sem, bool flg)									// 09/03/01 AM.
{
if (flg)
	{
	delete sem;
	return true;
	}
return vtor(sem, (long)0) ? true : false;	// Note: DELETES sem.
}


long Arun::vtor(long num1, long num2)
{
return num1 || num2;
}

long Arun::vtor(long num, RFASem *sem)
{
return vtor(sem,num);
}

long Arun::vtor(float num, RFASem *sem)									// 08/19/01 AM.
{
return vtor(sem,num);
}

long Arun::vtor(long num, _TCHAR *str)										// 08/19/01 AM.
{
return (str && *str) || num;
}

bool Arun::vtor(long num, bool flg)										// 09/03/01 AM.
{
return num || flg;
}

long Arun::vtor(float num, _TCHAR *str)										// 08/19/01 AM.
{
return (str && *str) || num;
}

long Arun::vtor(_TCHAR *str, RFASem *sem)
{
if (str && *str)
	{
	delete sem;
	return 1;
	}
return vtor(sem,(long)0);
}

long Arun::vtor(_TCHAR *str, long num)
{
return (num || (str && *str));
}

long Arun::vtor(_TCHAR *str, float num)										// 08/19/01 AM.
{
return (num || (str && *str));
}

long Arun::vtor(_TCHAR *str1, _TCHAR *str2)
{
return ((str1 && *str1) || (str2 && *str2));
}

bool Arun::vtor(_TCHAR *str, bool flg)										// 09/03/01 AM.
{
return ((str && *str) || flg);
}

long Arun::vtor(float num1, long num2)									// 08/19/01 AM.
{
return num1 || num2;
}

bool Arun::vtor(float fnum, bool flg)										// 09/03/01 AM.
{
return fnum || flg;
}

long Arun::vtor(long num1, float num2)									// 08/19/01 AM.
{
return num1 || num2;
}

long Arun::vtor(float num1, float num2)									// 08/19/01 AM.
{
return num1 || num2;
}

bool Arun::vtor(bool flg, RFASem *sem)									// 09/03/01 AM.
{
if (flg)
	{
	delete sem;
	return true;
	}
return vtor(sem, (long)0) ? true : false;	// Note: DELETES sem.
}

bool Arun::vtor(bool flg, long num)										// 09/03/01 AM.
{
return flg || num;
}

bool Arun::vtor(bool flg, float fnum)										// 09/03/01 AM.
{
return flg || fnum;
}

bool Arun::vtor(bool flg, _TCHAR *str)										// 09/03/01 AM.
{
return (flg || (str && *str));
}

bool Arun::vtor(bool flg1, bool flg2)										// 09/03/01 AM.
{
return (flg1 || flg2);
}

/********************************************
* FN:		INC
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '++' operator, compiled runtime.
* NOTE:	Not doing this like assign(), which may be too restrictive.
*			Want an expression to be able to eval to a variable.
*			If no var fetched, create it with initial value zero.
*			06/02/00 AM.NOT USED YET.  WHEN NLP++ EXPRS (eg functions)
*			can return a VARIABLE, can use something like this...
********************************************/
long Arun::inc(
	RFASem *sem,
	Nlppp *nlppp
	)
{
long num;
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

// Creates variable with init value of zero, if not found.
if (!Iexpr::inc(sem, nlppp, /*UP*/ num))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: increment operator.]") << ends;
	errOut(&gerrStr,false);
	}

return num;
}


// VARIANT TAKES EXPLICIT VAR.											// 06/02/00 AM.
long Arun::inc(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,																	// 10/13/00 AM.
	Nlppp *nlppp
	)
{
long num= 0;
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return 0;																	// 06/05/00 AM.
Dlist<Iarg> *vals = pair->getVals();

Delt<Iarg> *darg=0;															// 07/15/03 AM.
Iarg *arg;
if (vals)																		// 07/15/03 AM.
	darg = vals->getFirst();												// 07/15/03 AM.

if (!darg && index <= 0)										// FIX.	// 07/15/03 AM.
	{
	// Variable has no value.  Set to 1 and done.
	num = 1;
	Var::setVal(pair, (long)1);	// Install 1 in variable.
	return num;
	}

if (index >= 0)	// INDEXED ARRAY VALUE.								// 07/15/03 AM.
	{
	// See if indexed value exists.
	long ii;																		// 07/15/03 AM.
	for (ii = 0; darg && ii < index;										// 07/15/03 AM.
			++ii, darg = darg->Right())									// 07/15/03 AM.
		;

	if (!darg)	// Indexed value doesn't exist.						// 07/15/03 AM.
		{
		// PAD ZEROS TO INDEX.  Because we're going to
		// assign to that position down below.
		for (; ii <= index; ++ii)											// 07/15/03 AM.
			{
			arg = new Iarg((long)0);										// 07/15/03 AM.
			vals->rpush(arg);													// 07/15/03 AM.
			}
		darg = vals->getLast();												// 07/15/03 AM.
		}
	}

else if (darg->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Can't increment multi-valued variable.]") << ends;
	errOut(&gerrStr,false);
	return num;
	}
// Increment and store value.
arg = darg->getData();
// Depending on the argument type....
switch (arg->getType())
	{
	case IANUM:
		num = arg->getNum();	// FETCH VALUE.
		++num;					// INCREMENT VALUE.
		arg->setNum(num);		// STORE VALUE.
		break;
	case IASTR:
		if (!str_to_long(arg->getStr(), num))	// Convert, fetch.
			return 0;
		++num;									// Increment.
		arg->setType(IANUM);					// Convert value type.
		arg->setNum(num);						// Store number.
		break;
	case IASEM:														// FIX.	// 07/05/06 AM.
		{
		RFASem *semarg = arg->getSem();									// 07/05/06 AM.
		if (!semarg)															// 07/05/06 AM.
			return 0;															// 07/05/06 AM.
		bool ok = false;														// 07/05/06 AM.
		num = semarg->sem_to_long(ok);	// FETCH.					// 07/05/06 AM.
		if (!ok)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Can't increment non-numeric variable.]") << ends;
			errOut(&gerrStr,false);
			return 0;															// 07/05/06 AM.
			}
		++num;	// INCREMENT.												// 07/05/06 AM.
		semarg->sem_set_long(num,ok);	// STORE.						// 07/05/06 AM.
		return num;																// 07/05/06 AM.
		}
		break;
	default:
		return num;
	}
return num;
}


// VARIANT.			// 07/15/03 AM.
long Arun::inc(	// 07/15/03 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp
	)
{
bool ok = false;
long index = index_sem->sem_to_long(ok);
delete index_sem;
if (!ok)
	index = FULLARRAY;

return inc(typ,varname,nelt,index,nlppp);
}


/********************************************
* FN:		DEC
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '--' operator, compiled runtime.
* NOTE:	Not doing this like assign(), which may be too restrictive.
*			Want an expression to be able to eval to a variable.
*			If no var fetched, create it with initial value zero.
********************************************/
long Arun::dec(
	RFASem *sem,
	Nlppp *nlppp
	)
{
long num;
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

// Creates variable with init value of zero, if not found.
if (!Iexpr::dec(sem, nlppp, /*UP*/ num))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: decrement operator.]") << ends;
	errOut(&gerrStr,false);
	}

return num;
}

// VARIANT TAKES EXPLICIT VAR.											// 06/02/00 AM.
long Arun::dec(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,																	// 10/13/00 AM.
	Nlppp *nlppp
	)
{
long num= 0;
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return num;																	// 06/05/00 AM.
Dlist<Iarg> *vals = pair->getVals();

Delt<Iarg> *darg=0;															// 07/15/03 AM.
Iarg *arg;
if (vals)																		// 07/15/03 AM.
	darg = vals->getFirst();												// 07/15/03 AM.

if (!darg && index <= 0)										// FIX.	// 07/15/03 AM.
	{
	// Variable has no value.  Set to -1 and done.
	num = -1;
	Var::setVal(pair, num);	// Install -1 in variable.
	return num;
	}

if (index >= 0)	// INDEXED ARRAY VALUE.								// 07/15/03 AM.
	{
	// See if indexed value exists.
	long ii;																		// 07/15/03 AM.
	for (ii = 0; darg && ii < index;										// 07/15/03 AM.
			++ii, darg = darg->Right())									// 07/15/03 AM.
		;

	if (!darg)	// Indexed value doesn't exist.						// 07/15/03 AM.
		{
		// PAD ZEROS TO INDEX.  Because we're going to
		// assign to that position down below.
		for (; ii <= index; ++ii)											// 07/15/03 AM.
			{
			arg = new Iarg((long)0);										// 07/15/03 AM.
			vals->rpush(arg);													// 07/15/03 AM.
			}
		darg = vals->getLast();												// 07/15/03 AM.
		}
	}

else if (darg->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Can't decrement multi-valued variable.]") << ends;
	errOut(&gerrStr,false);
	return num;
	}
// Decrement and store value.
arg = darg->getData();
// Depending on the argument type....
switch (arg->getType())
	{
	case IANUM:
		num = arg->getNum();	// FETCH VALUE.
		--num;					// DECREMENT VALUE.
		arg->setNum(num);		// STORE VALUE.
		break;
	case IASTR:
		if (!str_to_long(arg->getStr(), num))	// Convert, fetch.
			return 0;
		--num;									// Decrement.
		arg->setType(IANUM);					// Convert value type.
		arg->setNum(num);						// Store number.
		break;
	case IASEM:														// FIX.	// 07/05/06 AM.
		{
		RFASem *semarg = arg->getSem();									// 07/05/06 AM.
		if (!semarg)															// 07/05/06 AM.
			return 0;															// 07/05/06 AM.
		bool ok = false;														// 07/05/06 AM.
		num = semarg->sem_to_long(ok);	// FETCH.					// 07/05/06 AM.
		if (!ok)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Can't decrement non-numeric variable.]") << ends;
			errOut(&gerrStr,false);
			return 0;															// 07/05/06 AM.
			}
		--num;	// DECREMENT.												// 07/05/06 AM.
		semarg->sem_set_long(num,ok);	// STORE.						// 07/05/06 AM.
		return num;																// 07/05/06 AM.
		}
		break;
	default:
		return num;
	}
return num;
}

// VARIANT.			// 07/15/03 AM.
long Arun::dec(	// 07/15/03 AM.
	int typ,
	_TCHAR *varname,
	int nelt,
	RFASem *index_sem,
	Nlppp *nlppp
	)
{
bool ok = false;
long index = index_sem->sem_to_long(ok);
delete index_sem;
if (!ok)
	index = FULLARRAY;

return dec(typ,varname,nelt,index,nlppp);
}

/********************************************
* FN:		NOT
* CR:		06/01/00 AM.
* SUBJ:	NLP++ '!' operator, compiled runtime.
********************************************/
long Arun::vtnot(
	RFASem *sem
	)
{
long num;

// Creates variable with init value of zero, if not found.
if (!Iexpr::vtnot(sem, 0, /*UP*/ num))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Error: unary 'not' operator.]") << ends;
	errOut(&gerrStr,false);
	}

return num;
}

long Arun::vtnot(
	long num
	)
{
return (num ? 0 : 1);											// FIX!	// 04/29/01 AM.
}

long Arun::vtnot(float num)													// 08/19/01 AM.
{
return (num ? 0 : 1);
}

long Arun::vtnot(
	_TCHAR *str
	)
{
return (str && *str) ? 0 : 1;									// FIX!	// 04/29/01 AM.
}

bool Arun::vtnot(bool flg)													// 09/03/01 AM.
{
return !flg;
}


/********************************************
* FN:		POSTINC
* CR:		06/02/00 AM.
* SUBJ:	NLP++ '++' operator, compiled runtime.
********************************************/

// VARIANT TAKES EXPLICIT VAR.											// 06/02/00 AM.
long Arun::postinc(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,																	// 10/13/00 AM.
	Nlppp *nlppp
	)
{
long ret = 0;				// Value to return.
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return ret;																	// 06/05/00 AM.
Dlist<Iarg> *vals = pair->getVals();

Delt<Iarg> *darg;
Iarg *arg;
if (!vals || !(darg = vals->getFirst()))
	{
	// Variable has no value.  Set to +1 and done.;
	Var::setVal(pair, (long)(ret+1));	// Install +1 in variable.
	return ret;
	}
if (darg->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Can't increment multi-valued variable.]") << ends;
	errOut(&gerrStr,false);
	return ret;
	}
// Increment and store value.
arg = darg->getData();
// Depending on the argument type....
switch (arg->getType())
	{
	case IANUM:
		ret = arg->getNum();		// FETCH VALUE.
		arg->setNum(ret+1);		// STORE INCREMENTED VALUE.
		break;
	case IASTR:
		if (!str_to_long(arg->getStr(), ret))	// Convert, fetch.
			return 0;
		arg->setType(IANUM);					// Convert value type.
		arg->setNum(ret+1);						// Store incremented number.
		break;
	default:
		return ret;
	}
return ret;
}


/********************************************
* FN:		POSTDEC
* CR:		06/02/00 AM.
* SUBJ:	NLP++ '--' operator, compiled runtime.
********************************************/

// VARIANT TAKES EXPLICIT VAR.											// 06/02/00 AM.
long Arun::postdec(
	int typ,
	_TCHAR *varname,
	int nelt,
	long index,																	// 10/13/00 AM.
	Nlppp *nlppp
	)
{
long ret = 0;				// Value to return.
Ipair *pair;
Ivar::getVar((enum Ivartype)typ,varname,nelt,nlppp,/*UP*/pair);
if (!pair)
	return ret;																	// 06/05/00 AM.
Dlist<Iarg> *vals = pair->getVals();

Delt<Iarg> *darg;
Iarg *arg;
if (!vals || !(darg = vals->getFirst()))
	{
	// Variable has no value.  Set to -1 and done.;
	Var::setVal(pair, (long)(ret-1));	// Install -1 in variable.
	return ret;
	}
if (darg->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Can't decrement multi-valued variable.]") << ends;
	errOut(&gerrStr,false);
	return ret;
	}
// Increment and store value.
arg = darg->getData();
// Depending on the argument type....
switch (arg->getType())
	{
	case IANUM:
		ret = arg->getNum();		// FETCH VALUE.
		arg->setNum(ret-1);		// STORE DECREMENTED VALUE.
		break;
	case IASTR:
		if (!str_to_long(arg->getStr(), ret))	// Convert, fetch.
			return 0;
		arg->setType(IANUM);					// Convert value type.
		arg->setNum(ret-1);						// Store decremented number.
		break;
	default:
		return ret;
	}
return ret;
}

/******************************************************************************/
// CODE ACTIONS
/******************************************************************************/


/********************************************
* FN:		SORTVALS
* CR:		06/04/00 AM.
* SUBJ:	Sort a variable's values.
* RET:	True if ok, else false.
********************************************/

bool Arun::sortvals(
	Nlppp *nlppp,
	_TCHAR *var
	)
{
if (!var || !*var || !nlppp)
	return false;

return Var::sortVals(var, nlppp->parse_);
}



/********************************************
* FN:		GTOLOWER
* CR:		06/04/00 AM.
* SUBJ:	Convert variable values to lowercase.
* RET:	True if ok, else false.
* NOTE:	To support case-insensitive lookup.
* FORM:	Gtolower(var_str)
*				var_str - global var has a list of strings as values.
********************************************/

bool Arun::gtolower(
	Nlppp *nlppp,
	_TCHAR *var
	)
{
if (!var || !*var || !nlppp)
	return false;

return Var::tolowerVals(var, nlppp->parse_);
}

/********************************************
* FN:		GUNIQ
* CR:		06/04/00 AM.
* SUBJ:	Remove redundant strings in sorted list of values.
* RET:	True if ok, else false.
* NOTE:	To support case-insensitive lookup.
*			Backward compatibility.
* FORM:	Guniq(VAR)
*				VAR - global var has SORTED list of strings as values.
********************************************/

bool Arun::guniq(
	Nlppp *nlppp,
	_TCHAR *var
	)
{
if (!var || !*var || !nlppp)
	return false;
return Var::uniqVals(var, nlppp->parse_);
}



/********************************************
* FN:		CODELOOKUP
* CR:		06/04/00 AM.
* SUBJ:	Lookup variable's strings in the given file. Update sym's flag.
* RET:	True if ok, else false.
* NOTE:	06/04/00 AM. Pulled internals to separate variant.
* FORM:	lookup(VAR, FILE, FLAG)
*			eg, lookup("Words", "doom.words", "word")
*			VAR - has a list of strings as values.
*			FILE - a file of strings, one per line, in appdir/data/FILE
*			FLAG - eg, "word", tells which bit-flag of hashed sym to update.
********************************************/

bool Arun::lookup(
	Nlppp *nlppp,
	_TCHAR *var,
	_TCHAR *fname,
	_TCHAR *flag
	)
{
if (!nlppp)
	return false;

return Code::codeLookup(var,fname,flag,nlppp->parse_);
}


/********************************************
* FN:		STARTOUT
* CR:		06/04/00 AM.
* SUBJ:	Start the output to the user-supplied output file.
* RET:	True if ok, else false.
* NOTE:	Backward compatibility. 
********************************************/

bool Arun::startout(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return false;

_TCHAR *str = 0;
Parse *parse = nlppp->parse_;

if (parse->getOut())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Startout code action: Output file already open.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

// Open the user-supplied output file.
// If none, then use standard output.
_TCHAR *name = 0;
if (name = parse->getOutput())
	{
	// Open the output file for APPENDING.
	_t_ofstream *fout = 0;
	fout = new _t_ofstream(TCHAR2CA(name), ios::app);
	parse->setOut(fout);
	parse->newostr(fout);			// Add to list of ostrs.		// 05/23/01 AM.
	}
else	// No output file.  Use standard out by default.
	{
	parse->setOut(&_t_cout);
	}

return true;
}


/********************************************
* FN:		STOPOUT
* CR:		06/04/00 AM.
* SUBJ:	Stop the output to the user-supplied output file.
* RET:	True if ok, else false.
* NOTE:	Backward compatibility.
********************************************/

bool Arun::stopout(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return false;

_TCHAR *str = 0;
Parse *parse = nlppp->parse_;

//if (!Arg::done(args, "codeStopout"))
	{
	// Let it go.
	//return false;
	}


// Close the output file.
_t_ostream *tmp = parse->getOut();											// 05/23/01 AM.
if (!tmp)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Stopout code action: No output file is open.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
else if (tmp == &_t_cout)		// Program's "real" output is to standard output.
	;
else
	{
	// Close the output file.
//	delete tmp;																	// 05/23/01 AM.
	parse->deleteostr(tmp);		// Rm from list and delete.		// 05/23/01 AM.
	}

parse->setOut(0);				// Reset the output handle.

return true;
}



/********************************************
* FN:		FILEOUT
* CR:		06/04/00 AM.
* SUBJ:	Start the output to an output file specified in this action.
* RET:	True if ok, else false.
* NOTE:	Create a variable whose name is the file name and value is
*			the output stream.  A quick solution.
*			If file is closed, the ostream ptr will be NIL.
* OPT:	Some redundancy in fetching the variable here.
********************************************/

bool Arun::fileout(
	Nlppp *nlppp,
	_TCHAR *fname
	)
{
if (!nlppp)
	return false;
Parse *parse = nlppp->parse_;

// Get user specified filename.  Taking name as is.
// Name shouldn't have a path in it, for now.
// eg, expecting something like "companies.txt", to list
// company names in $appdir\output\companies.txt.

if (!fname || !*fname)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[fileout: Error. Given null filename.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

Ipair *pair;
_t_ostream *ostr;
bool exists,open,inuse,badname;
if (!Var::filevar(fname,parse,										// 12/31/99 AM.
									/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	if (inuse)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[fileout: Error. var=") << fname << _T(" already in use.]")
					<< ends;
		errOut(&gerrStr,false);
		}

	else if (badname)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[fileout: Error. var=") << fname << _T(" is bad name.]")
					<< ends;
		errOut(&gerrStr,false);
		}

	else
		{
		_t_strstream gerrStr;
		gerrStr << _T("[fileout: Error creating output var=")
					<< fname << _T("]") << ends;
		errOut(&gerrStr,false);
		}

	return false;
	}

if (exists)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[fileout: Warning. output var=")
			<< fname << _T(" exists.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
return true;
}


/********************************************
* FN:		XRENAME
* CR:		06/05/00 AM.
* SUBJ:	Rename a context node.
* RET:	True if ok, else false.
* FORM:	Xrename(NAME, [NUM])
*				where
*					NAME - new name for node.
*					[NUM] - numth context node. If zero or absent, last node.
********************************************/

bool Arun::xrename(
	Nlppp *nlppp,
	_TCHAR *name,
	long ord
	)
{
if (!name || !*name || ord < 0 || !nlppp)
	return false;

_TCHAR *xvar = 0;					// Context node var.

// Get the ordth context node.
// ord == 0 means get from last component of context, ie,
// the immediately dominant node.
Node<Pn> *select, *tmp;
Pn *pn;
if (!(select = nlppp->select_))
	return false;
if (!ord)						// LAST component of path.
	pn = select->getData();
else								// Get component.
	{
	// Recurse to the root, then count down.
	tmp = select->pathNth(ord);
	if (!tmp)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Xrename: Couldn't find node X(") << ord
					<< _T(").]") << ends;
		errOut(&gerrStr,false);
		return false;
		}
	pn  = tmp->getData();
	}

// Rename it.
pn->setName(name);

return true;
}


bool Arun::xrename(
	Nlppp *nlppp,
	RFASem *name_sem,
	long ord
	)
{
if (!name_sem)
	return false;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return xrename(nlppp,name,ord);
}


bool Arun::xrename(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *ord_sem
	)
{
if (!ord_sem)
	return false;
bool ok = false;
long ord = ord_sem->sem_to_long(ok);
delete ord_sem;
if (!ok)
	return false;
return xrename(nlppp,name,ord);
}


bool Arun::xrename(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *ord_sem
	)
{
if (!name_sem)
	{
	if (ord_sem)
		delete ord_sem;
	return false;
	}
if (!ord_sem)
	{
	delete name_sem;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
bool ok = false;
long ord = ord_sem->sem_to_long(ok);
delete ord_sem;
if (!ok)
	return false;
return xrename(nlppp,name,ord);
}


/********************************************
* FN:		DEBUG
* CR:		06/05/00 AM.
* SUBJ:	Breakpoint function for debugging rules.
* RET:	True if ok, else false.
* FORM:	debug();
********************************************/

bool Arun::debug(
	Nlppp *nlppp
	)
{
return true;
}


/********************************************
* FN:		FPRINTVAR
* CR:		06/06/00 AM.
* SUBJ:	Print a variable's value to file.
* RET:	True if ok, else false.
* NOTE:	Compiled runtime variant of codeFprintvar.
*			For dumping multivalued string variable.
********************************************/

bool Arun::fprintvar(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *var,
	_TCHAR *sep
	)
{
if (!fname || !*fname || !var || !*var || !nlppp)
	return false;

if (!sep || !*sep)
	sep = _T("\n");				// A default separator.
// INCOMPLETE!  NOT HANDLING SEPARATOR YET.

Parse *parse = nlppp->parse_;

_TCHAR *st = 0;

// 01/25/99 AM.
// Fetch a handle for the variable values.
Delt<Iarg> *values = 0;
if (!Var::vals(var, parse, /*UP*/ values))
	return false;

_t_ostream *ostr = 0;
if (!Var::val(fname, parse, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Fprintvar code action: file=") << fname
			  << _T(" is closed.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

// Print out all values.
// (Handling multiple values of variable).
bool first = true;
Iarg *arg;
while (values)
	{
	// Get string and print it.
	//st = Var::getStr(values);
	if (!first)
		{
		*ostr << _T(" ");
//		*ostr << sep;
		}
	else
		first = false;
	arg = values->getData();
	switch (arg->getType())
		{
		case IASTR:
			st = arg->getStr();
			*ostr << ::str(st) << flush;
			break;
		case IANUM:
			*ostr << arg->getNum() << flush;
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[codeFprintvar: Bad value]") << ends;
			errOut(&gerrStr,false);
			}

			return false;
		}
	values = values->Right();
	}
return true;
}

/******************************************************************************/
// FN.CPP FUNCTIONS
/******************************************************************************/


// NOTE: THESE ARE IN FNRUN.CPP		// 06/27/00 AM.

/******************************************************************************/
// MISC
/******************************************************************************/

/********************************************
* FN:		HASH_STR_NOCASE
* CR:		06/14/00 AM.
* SUBJ:	Dummy for the moment.
* RET:	True if ok, else false.
********************************************/

bool Arun::hash_str_nocase(
	_TCHAR *name,
	const ELIST elist
	)
{
// Hash to location.
_TCHAR buf[MAXSTR+1];
str_to_lower(name,buf);										// 06/15/00 AM.
long index = Htab::hashfn(buf, elist.hsize);
const _TCHAR **arr = elist.htab[index];

// Then use
return find_str_nocase(name, arr);
}


/********************************************
* FN:		HFIND_LC
* CR:		06/20/00 AM.
* SUBJ:	Find name in rule hash table.
* RET:	Associated array of rule numbers, if any.
********************************************/

const int *Arun::hfind_lc(
	_TCHAR *name,
	const RHASH **htab,
	long hsize,
	/*UP*/
	long &len				// Length of rule array.
	)
{
len = 0;
if (!name || !*name || !htab || hsize <= 0)
	return 0;

// Hash to table index.
// (Make sure string is lowercase.)
_TCHAR buf[MAXSTR];
str_to_lower(name, buf);
//long index = Htab::hashfn(buf,hsize);	// WRONG HASH CLASS, but same fn.
long index = tHtab<Slist<Irule> >::hashfn(buf,hsize);	// FIX.	// 07/08/03 AM.

// Traverse down conflict chain looking for a match.
const RHASH *chain;
for (chain = htab[index]; chain && (chain->length > 0); ++chain)
	{
	// Return array of rule nums if match found.
	if (!_tcscmp(buf,chain->name))
		{
		len = chain->length;
		return chain->rules;
		}
	}
return 0;			// Did not find in hash table.
}


/********************************************
* FN:		MERGERULES
* CR:		06/21/00 AM.
* SUBJ:	Find name in rule hash table.
* RET:	Associated array of rule numbers, if any.
* ASS:	Neither array is empty.
*			Both arrays are sorted numbers.
*			(Should have no redundancy.)
* NOTE:	Otherwise, get into issues of which array can be deleted,
*			needs to be copied and so on.
********************************************/

int *Arun::mergeRules(
	int *arr1,
	long size1,			// Size of array.
	int *arr2,
	long size2
	)
{
if (!arr1 || !arr2 || !size1 || !size2)
	return 0;				// Treat this as an error.

int *newarr = new int [size1+size2+1];		// 1 for null termination.
int *ptr = newarr;
for (;;)			// Traverse both arrays simultaneously.
	{
	if (!*arr1)
		{
		// Copy out rest of arr2 and done.
		while (*ptr++ = *arr2++);
		return newarr;
		}
	else if (!*arr2)
		{
		// Copy out rest of arr1 and done.
		while (*ptr++ = *arr1++);
		return newarr;
		}
	else			// Both arrays still full.
		{
		if (*arr1 < *arr2)
			*ptr++ = *arr1++;
		else if (*arr2 > *arr1)	// 09/20/06 AM.
			*ptr++ = *arr2++;
		else	// EQUAL rule numbers.  Just in case.	// 09/20/06 AM.
			{
			*ptr++ = *arr2++;		// 09/20/06 AM.
			++arr1;					// 09/20/06 AM.
			}
		}
	}
return newarr;		// Shouldn't reach here.
}


/********************************************
* FN:		SEM_TO_STR
* CR:		04/28/01 AM.
* SUBJ:	Convert sem to string.
* RET:	INTERNED string, or 0 if not ok.
*			If "empty string", using "" -- not deletable also.
* NOTE:	Convenience function.
* ASS:	Assume sem has an interned string.
********************************************/

_TCHAR *Arun::sem_to_str(
	RFASem *sem
	)
{
if (!sem)																		// 03/13/02 AM.
	return 0;																	// 03/13/02 AM.

switch (sem->getType())														// 08/08/02 AM.
	{
	case RSSTR:																	// 08/08/02 AM.
	case RSNAME:																// 08/08/02 AM.
	case RSNUM:
		return sem->getName();												// 08/08/02 AM.
	case RSLONG:																// 08/08/02 AM.
		// Zero is acceptable as "empty string".
		if (sem->getLong() == 0)											// 08/08/02 AM.
			return 0;															// 08/08/02 AM.
		break;
	case RSARGS:																// 08/08/02 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Can't get string from multivalued var.]") << ends;
		errOut(&gerrStr,false);
		return 0;
		}
	default:																		// 08/08/02 AM.
		break;
	}

_t_strstream gerrStr;
gerrStr << _T("[Error: Bad sem type in sem_to_str]") << ends;
errOut(&gerrStr,false);
return 0;
}


/********************************************
* FN:		SEM_TO_OSTREAM
* CR:		05/04/03 AM.
* SUBJ:	Convert sem to ostream.
* RET:	ostream.
* NOTE:	Convenience function.
********************************************/

_t_ostream *Arun::sem_to_ostream(
	RFASem *sem
	)
{
if (!sem)
	return 0;

switch (sem->getType())
	{
	case RSOSTREAM:
		return sem->getOstream();
	case RSARGS:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Error: Can't get ostream from multivalued var.]") << ends;
		errOut(&gerrStr,false);
		return 0;
		}
	default:
		break;
	}

_t_strstream gerrStr;
gerrStr << _T("[Error: Bad sem type in sem_to_ostream]") << ends;
errOut(&gerrStr,false);
return 0;
}


/********************************************
* FN:		SORT_ARR_KBCON_BY_ATTR
* CR:		12/02/02 AM.
* SUBJ:	Sort array of KB concepts by attribute value.
* WARN:	Creates new array.  Caller must delete.
********************************************/
static _TCHAR *global_attr = 0;
static CG *global_cg = 0;
void *Arun::sort_arr_kbcon_by_attr(
	void *args,			// Array of kbconcepts.
	_TCHAR *attr,			// Name of concept attribute.
	bool numeric,		// True if numeric, false if string.
	bool descending,	// True if descending, false if ascending.
	CG *cg
	)
{
if (!args || !attr || !*attr || !cg)
	return 0;
Delt<Iarg> *darg = ((Dlist<Iarg> *)args)->getFirst();
if (!darg)
  return 0;

// Create an array.
long len = 0;
CONCEPT **arr;
len = ((Dlist<Iarg>*)args)->getLength();
arr = new CONCEPT * [len];

// Make array elts point to list elts.
CONCEPT **parr = 0;
CONCEPT *pnode = 0;
Iarg *arg = 0;
RFASem *sem = 0;
for (parr = arr;
	  darg;
	  darg = darg->Right(), ++parr
	  )
	{
	arg = darg->getData();
	sem = arg->getSem();
	*parr = sem->getKBconcept();
	}

// Sort array.
global_attr = attr;
global_cg = cg;
if (numeric)
	qsort((void *)arr, (size_t)len, sizeof(CONCEPT *), compare_numattrs);
else
	qsort((void *)arr, (size_t)len, sizeof(CONCEPT *), compare_strattrs);

// Copy array back to new arg list.
Dlist<Iarg> *dargs = new Dlist<Iarg>();
parr = arr;

if (!descending)
	while (--len >= 0)
		{
		sem = new RFASem(*parr,RS_KBCONCEPT);
		arg = new Iarg(sem);
		dargs->rpush(arg);
		++parr;
		}
else
	while (--len >= 0)
		{
		sem = new RFASem(*parr,RS_KBCONCEPT);
		arg = new Iarg(sem);
		dargs->push(arg);
		++parr;
		}

// Trash the array.
delete [] arr;
return dargs;
}

/********************************************
* FN:		COMPARE_NUMATTRS
* CR:		12/02/02 AM.
* SUBJ:	Compare numeric concept attributes.
* NOTE:	Used by qsort.
********************************************/

int compare_numattrs( const void *arg1, const void *arg2 )
{
CONCEPT **a1, **a2;

a1 = (CONCEPT **) arg1;
a2 = (CONCEPT **) arg2;

// Get attrs according to global_attr.
long val1 = 0;
long val2 = 0;
global_cg->findVal(*a1, global_attr, /*UP*/ val1);
global_cg->findVal(*a2, global_attr, /*UP*/ val2);

return (val1 - val2);
}

/********************************************
* FN:		COMPARE_STRATTRS
* CR:		12/02/02 AM.
* SUBJ:	Compare string concept attributes.
* NOTE:	Used by qsort.
********************************************/

int compare_strattrs( const void *arg1, const void *arg2 )
{
CONCEPT **a1, **a2;

a1 = (CONCEPT **) arg1;
a2 = (CONCEPT **) arg2;

// Get attrs according to global_attr.
_TCHAR *str1 = global_cg->strVal(*a1, global_attr);
_TCHAR *str2 = global_cg->strVal(*a2, global_attr);
if (!str1 || !str2)
  return 0;
#ifndef LINUX
return strcmp_i(str1, str2);
#else
return strcasecmp(str1, str2);
#endif
}

