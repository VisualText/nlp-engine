/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CHECK.CPP
* FILE:	lite\check.cpp
* CR:		11/27/98 AM.
* SUBJ:	Check actions for Pat pass.
* NOTE:	User can employ check actions to accept or reject the rule match
*			prior to modifying the parse tree.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/24/06 AM.
#include "lite/Auser.h"		// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "string.h"
#include "lite/nlppp.h"		// 11/19/99 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "parse.h"
//#include "lite/nlppp.h"		// 11/19/99 AM.
#include "pat.h"
#include "pn.h"

#include "arg.h"
#include "var.h"			// 08/06/99 AM.
#include "rfasem.h"		// 11/22/99 AM.
#include "check.h"

int Check::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

Check::Check()
{
#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

Check::~Check()
{

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

/********************************************
* FN:		Modify Functions
********************************************/

/********************************************
* FN:		GETCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Check::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Check::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Check count=") << count_ << std::endl;
	*gout << _T("Active Check count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Check count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		CHECKACTIONS
* CR:		11/27/98 AM.
* SUBJ:	Execute check actions for matched rule.
* NOTE:	If rejecting the rule, sem structures built should be removed.
********************************************/
bool Pat::checkActions(		// 11/19/99 AM.
	Nlppp *nlppp
	)
{
if (nlppp->sem_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Check Actions: given non null semantic object.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

assert(nlppp->rule_);
Dlist<Iaction> *checks;		// Check-actions.
if (!(checks = nlppp->rule_->getChecks()))
	return true;			// No checks have led to failure of the rule match.

nlppp->region_ = REGCHECK;	// Exec NLP++ in check region.	// 01/13/00 AM.

// Handle L() local vars in @CODE region!							// 11/24/02 AM.
void *dummy = 0;															// 01/08/07 AM.
Arun::fnstart(nlppp,dummy,dummy);			// 11/24/02 AM.	// 01/08/07 AM.

Delt<Iaction> *dcheck;
Iaction *check;
bool accept = true;		// DEFAULT: accept current rule match.
bool ok = true;															// 11/22/99 AM.
RFASem *rfasem, *val = 0;												// 11/22/99 AM.
bool badname = false;													// 01/13/00 AM.
for (dcheck = checks->getFirst(); dcheck; dcheck = dcheck->Right())
	{
	// Perform the current check action.
	check	= dcheck->getData();

	if ((rfasem = check->getSem()))											// 11/22/99 AM.
		{
		//*gerr << "[Check: Statements to execute.]" << std::endl;		// 11/22/99 AM.
		if (!(ok = rfasem->eval(nlppp, /*UP*/ val)))					// 11/22/99 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Check: Error in NLP++.]") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}
				// 11/22/99 AM.
		if (val)
			{
			// Check value here.
			//accept = Check::checkTrue(val);							// 06/10/00 AM.
			delete val;															// 11/22/99 AM.
			//if (!accept)														// 06/10/00 AM.
			//	break;															// 06/10/00 AM.
			}
		//continue;																// 06/10/00 AM.
		}
	else if (!checkAction(check, nlppp, /*UP*/ badname))			// 01/13/00 AM.
		{
		if (badname)															// 01/13/00 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Execute check action: Unknown action=")
							<< check->getName() << _T("].") << std::ends;
			nlppp->parse_->errOut(&gerrStr,false);
			}

		accept = false;			// Reject the current rule match.
		break;
		}

	if (nlppp->fail_													// 06/10/00 AM.
	 || nlppp->exitpass_)								// FIX.	// 06/26/09 AM.
		{
		accept = false;														// 06/10/00 AM.
		break;																	// 06/10/00 AM.
		}
	else if (nlppp->succeed_)														// 06/10/00 AM.
		break;																	// 06/10/00 AM.
	}

return checkFinal(accept, nlppp);
}


/********************************************
* FN:		CHECKACTION
* CR:		11/05/98 AM.
* SUBJ:	Execute one check action for matched rule.
* RET:	True if action executed ok, false if error.
********************************************/
bool Pat::checkAction(
	Iaction *check,			// 01/13/00 AM.
	Nlppp *nlppp,				// 11/19/99 AM.
	/*UP*/
	bool &badname				// 01/13/00 AM.
	)
{
//if (Debug())
//	*gout << "  [Execute check action...]" << std::endl;

// Moved this stuff here.												// 01/13/00 AM.
_TCHAR *func;
Dlist<Iarg> *dargs;
Delt<Iarg> *args;
func	= check->getName();
dargs	= check->getArgs();
if (dargs)
	args	= dargs->getFirst();
else
	args = 0;

// Just a simple test of check actions.
if (!_tcscmp(func, _T("or")))
	return Check::checkOr(args,nlppp);

else if (!_tcscmp(func, _T("Xlt")))			// Renamed.
	return Check::checkXlt(args,nlppp);
else if (!_tcscmp(func, _T("Xgt")))			// 09/14/99 AM.
	return Check::checkXgt(args,nlppp);
else if (!_tcscmp(func, _T("Xeq")))			// 09/14/99 AM.
	return Check::checkXeq(args,nlppp);
else if (!_tcscmp(func, _T("Xne")))			// 09/14/99 AM.
	return Check::checkXne(args,nlppp);
else if (!_tcscmp(func, _T("Xle")))			// 09/14/99 AM.
	return Check::checkXle(args,nlppp);
else if (!_tcscmp(func, _T("Xge")))			// 09/14/99 AM.
	return Check::checkXge(args,nlppp);
else if (!_tcscmp(func, _T("Nlt")))			// Renamed.
	return Check::checkNlt(args,nlppp);
else if (!_tcscmp(func, _T("Ngt")))			// 09/14/99 AM.
	return Check::checkNgt(args,nlppp);
else if (!_tcscmp(func, _T("Neq")))			// 09/14/99 AM.
	return Check::checkNeq(args,nlppp);
else if (!_tcscmp(func, _T("Nne")))			// 09/14/99 AM.
	return Check::checkNne(args,nlppp);
else if (!_tcscmp(func, _T("Nle")))			// 09/14/99 AM.
	return Check::checkNle(args,nlppp);
else if (!_tcscmp(func, _T("Nge")))			// 09/14/99 AM.
	return Check::checkNge(args,nlppp);
else if (!_tcscmp(func, _T("Glt")))			// 09/19/99 AM.
	return Check::checkGlt(args,nlppp);
else if (!_tcscmp(func, _T("Ggt")))			// 09/19/99 AM.
	return Check::checkGgt(args,nlppp);
else if (!_tcscmp(func, _T("Geq")))			// 09/19/99 AM.
	return Check::checkGeq(args,nlppp);
else if (!_tcscmp(func, _T("Gne")))			// 09/19/99 AM.
	return Check::checkGne(args,nlppp);
else if (!_tcscmp(func, _T("Gle")))			// 09/19/99 AM.
	return Check::checkGle(args,nlppp);
else if (!_tcscmp(func, _T("Gge")))			// 09/19/99 AM.
	return Check::checkGge(args,nlppp);

else if (!_tcscmp(func, _T("debug")))			// 11/22/99 AM.
	return Check::checkDebug(args,nlppp);

badname = true;											// FIX!			// 06/10/00 AM.
return false;
}


/********************************************
* FN:		CHECKFINAL
* CR:		11/27/98 AM.
* SUBJ:	Clean up after check actions.
* RET:	True if accepting rule, else false.
********************************************/

bool Pat::checkFinal(
	bool accept,				// If accepting rule match.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
// Handle local variables.													// 11/24/02 AM.
if (nlppp->locals_)															// 11/24/02 AM.
	{
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);				// 11/24/02 AM.
	nlppp->locals_ = 0;														// 11/24/02 AM.
	}
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
nlppp->freeLocals(0);														// 01/08/07 AM.

if (accept)
	return true;
else
	{
	if (nlppp->sem_)
		{
		delete nlppp->sem_;
		nlppp->sem_ = 0;
		}
	if (nlppp->dsem_)												// 11/17/99 AM.
		{
		Dlist<Ipair>::DeleteDlistAndData(nlppp->dsem_);	// 11/17/99 AM.
		nlppp->dsem_ = 0;											// 11/17/99 AM.
		}
	return false;
	}
}




/********************************************
* FN:		CHECKEND
* CR:		05/20/08 AM.
* SUBJ:	Clean up after successful check actions.
* RET:	True if ok.
* NOTE:	Need to clear out local vars after check actions are done,
*			in compiled version of analyzers.
********************************************/

bool Pat::checkEnd(
	Nlppp *nlppp
	)
{
// Handle local variables.
if (nlppp->locals_)
	{
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);
	nlppp->locals_ = 0;
	}
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.
nlppp->freeLocals(0);

return true;
}

/********************************************
* FN:		CHECK ACTIONS.
********************************************/


/********************************************
* FN:		CHECKOR
* CR:		11/27/98 AM.
* SUBJ:	Check if one of the range of rule elements has nodes for it.
* RET:	True if check succeeded.
* NOTE:	Testing the check action machinery with this first check.
* WARN:	This does NOT check for presence of one out of two elements.
********************************************/

bool Check::checkOr(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{

Node<Pn> *nstart1, *nend1, *coll1;
Node<Pn> *nstart2, *nend2, *coll2;

if (!Arg::elt1(nlppp->collect_, _T("checkOr"),/*DU*/ ((DELTS*&)args), (NODE*&)coll1, (NODE*&)nstart1, (NODE*&)nend1))
	return false;
if (!Arg::elt1(nlppp->collect_, _T("checkOr"),/*DU*/ ((DELTS*&)args), (NODE*&)coll2, (NODE*&)nstart2, (NODE*&)nend2))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkOr"),nlppp->parse_))
	return false;

// Check the range of collects from coll1 to coll2, inclusive.
Node<Pn> *bound;
Node<Pn> *ptr;
bound = (coll2 ? coll2->Right() : 0);

for (ptr = coll1; ptr != bound; ptr = ptr->Right())
	{
	if (coll1->Down())
		return true;			// One rule element has been matched.
	}
return false;
}


/********************************************
* FN:		CHECKNLT
* CR:		08/07/99 AM.
* SUBJ:	Check if rule node's var is less than a number.
* RET:	True if check succeeded.
* NOTE:	09/14/99 AM. Renamed from checkNless, nless.
* FORM:	Nlt(ord,var,num)
********************************************/

bool Check::checkNlt(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend, *coll;
_TCHAR *var = 0;
long long num = 0;

if (!Arg::elt1(nlppp->collect_, _T("checkNlt"),/*DU*/ ((DELTS*&)args), (NODE*&)coll, (NODE*&)nstart, (NODE*&)nend))
	return false;
if (!Arg::str1(_T("checkNlt"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkNlt"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkNlt"),nlppp->parse_))
	return false;

// Get nth node.  If more than one covered, not handled.
if (!nstart)
	return false;
if (nstart != nend)
	return false;

// Get given value of variable from nth node.

Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nstart->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum < num)
	return true;
return false;
}


/********************************************
* FN:		CHECKNGT
* CR:		09/14/99 AM.
* SUBJ:	Check if rule node's var is less than a number.
* RET:	True if check succeeded.
* FORM:	Ngt(ord,var,num)
********************************************/

bool Check::checkNgt(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend, *coll;
_TCHAR *var = 0;
long long num = 0;

if (!Arg::elt1(nlppp->collect_, _T("checkNgt"),/*DU*/ ((DELTS*&)args), (NODE*&)coll, (NODE*&)nstart, (NODE*&)nend))
	return false;
if (!Arg::str1(_T("checkNgt"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkNgt"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkNgt"),nlppp->parse_))
	return false;

// Get nth node.  If more than one covered, not handled.
if (!nstart)
	return false;
if (nstart != nend)
	return false;

// Get given value of variable from nth node.

Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nstart->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum > num)
	return true;
return false;
}


/********************************************
* FN:		CHECKNEQ
* CR:		09/14/99 AM.
* SUBJ:	Check if rule node's var equals a number.
* RET:	True if check succeeded.
* FORM:	Neq(ord,var,num)
********************************************/

bool Check::checkNeq(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend, *coll;
_TCHAR *var = 0;
long long num = 0;

if (!Arg::elt1(nlppp->collect_, _T("checkNeq"),/*DU*/ ((DELTS*&)args), (NODE*&)coll, (NODE*&)nstart, (NODE*&)nend))
	return false;
if (!Arg::str1(_T("checkNeq"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkNeq"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkNeq"),nlppp->parse_))
	return false;

// Get nth node.  If more than one covered, not handled.
if (!nstart)
	return false;
if (nstart != nend)
	return false;

// Get given value of variable from nth node.

Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nstart->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum == num)
	return true;
return false;
}


/********************************************
* FN:		CHECKNNE
* CR:		09/14/99 AM.
* SUBJ:	Check if rule node's var not equal a number.
* RET:	True if check succeeded.
* FORM:	Nne(ord,var,num)
********************************************/

bool Check::checkNne(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend, *coll;
_TCHAR *var = 0;
long long num = 0;

if (!Arg::elt1(nlppp->collect_, _T("checkNne"),/*DU*/ ((DELTS*&)args), (NODE*&)coll, (NODE*&)nstart, (NODE*&)nend))
	return false;
if (!Arg::str1(_T("checkNne"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkNne"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*&)args), _T("checkNne"),nlppp->parse_))
	return false;

// Get nth node.  If more than one covered, not handled.
if (!nstart)
	return false;
if (nstart != nend)
	return false;

// Get given value of variable from nth node.

Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nstart->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum != num)
	return true;
return false;
}


/********************************************
* FN:		CHECKNGE
* CR:		09/14/99 AM.
* SUBJ:	Check if rule node's var greater or equal to a number.
* RET:	True if check succeeded.
* FORM:	Nge(ord,var,num)
********************************************/

bool Check::checkNge(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend, *coll;
_TCHAR *var = 0;
long long num = 0;

if (!Arg::elt1(nlppp->collect_, _T("checkNge"),/*DU*/ ((DELTS*&)args), (NODE*&)coll, (NODE*&)nstart, (NODE*&)nend))
	return false;
if (!Arg::str1(_T("checkNge"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkNge"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkNge"),nlppp->parse_))
	return false;

// Get nth node.  If more than one covered, not handled.
if (!nstart)
	return false;
if (nstart != nend)
	return false;

// Get given value of variable from nth node.

Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nstart->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum >= num)
	return true;
return false;
}


/********************************************
* FN:		CHECKNLE
* CR:		09/14/99 AM.
* SUBJ:	Check if rule node's var lessequal a number.
* RET:	True if check succeeded.
* FORM:	Nle(ord,var,num)
********************************************/

bool Check::checkNle(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend, *coll;
_TCHAR *var = 0;
long long num = 0;

if (!Arg::elt1(nlppp->collect_, _T("checkNle"),/*DU*/ ((DELTS*&)args), (NODE*&)coll, (NODE*&)nstart, (NODE*&)nend))
	return false;
if (!Arg::str1(_T("checkNle"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkNle"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkNle"),nlppp->parse_))
	return false;

// Get nth node.  If more than one covered, not handled.
if (!nstart)
	return false;
if (nstart != nend)
	return false;

// Get given value of variable from nth node.

Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nstart->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum <= num)
	return true;
return false;
}


/********************************************
* FN:		CHECKXLT
* CR:		08/06/99 AM.
* SUBJ:	Check if context node's var is less than a number.
* RET:	True if check succeeded.
* NOTE:	09/14/99 AM. Renamed from checkXless, xless.
* FORM:	Xlt(var,num)
********************************************/

bool Check::checkXlt(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkXlt"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkXlt"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkXlt"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum < num)
	return true;
return false;
}


/********************************************
* FN:		CHECKXGT
* CR:		09/14/99 AM.
* SUBJ:	Check if context node's var is less than a number.
* RET:	True if check succeeded.
* FORM:	Xgt(var,num)
********************************************/

bool Check::checkXgt(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkXgt"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkXgt"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkXgt"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum > num)
	return true;
return false;
}


/********************************************
* FN:		CHECKXEQ
* CR:		09/14/99 AM.
* SUBJ:	Check if context node's var equals a number.
* RET:	True if check succeeded.
* FORM:	Xeq(var,num)
********************************************/

bool Check::checkXeq(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkXeq"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkXeq"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkXeq"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum == num)
	return true;
return false;
}


/********************************************
* FN:		CHECKXNE
* CR:		09/14/99 AM.
* SUBJ:	Check if context node's var not equal a number.
* RET:	True if check succeeded.
* FORM:	Xne(var,num)
********************************************/

bool Check::checkXne(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkXne"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkXne"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkXne"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum != num)
	return true;
return false;
}


/********************************************
* FN:		CHECKXGE
* CR:		09/14/99 AM.
* SUBJ:	Check if context node's var greater or equal to a number.
* RET:	True if check succeeded.
* FORM:	Xge(var,num)
********************************************/

bool Check::checkXge(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkXge"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkXge"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkXge"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum >= num)
	return true;
return false;
}


/********************************************
* FN:		CHECKXLE
* CR:		09/14/99 AM.
* SUBJ:	Check if context node's var lessequal a number.
* RET:	True if check succeeded.
* FORM:	Xle(var,num)
********************************************/

bool Check::checkXle(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkXle"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkXle"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkXle"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

long long vnum = 0;
Var::val(var, dlist, /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum <= num)
	return true;
return false;
}


/********************************************
* FN:		CHECKGLT
* CR:		09/20/99 AM.
* SUBJ:	Check if context node's var is less than a number.
* RET:	True if check succeeded.
* FORM:	Glt(var,num)
********************************************/

bool Check::checkGlt(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkGlt"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkGlt"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkGlt"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;
long long vnum = 0;
Var::val(var, nlppp->parse_->getVars(), /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum < num)
	return true;
return false;
}


/********************************************
* FN:		CHECKGGT
* CR:		09/20/99 AM.
* SUBJ:	Check if context node's var is less than a number.
* RET:	True if check succeeded.
* FORM:	Ggt(var,num)
********************************************/

bool Check::checkGgt(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkGgt"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkGgt"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkGgt"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;

long long vnum = 0;
Var::val(var, nlppp->parse_->getVars(), /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum > num)
	return true;
return false;
}


/********************************************
* FN:		CHECKGEQ
* CR:		09/20/99 AM.
* SUBJ:	Check if context node's var equals a number.
* RET:	True if check succeeded.
* FORM:	Geq(var,num)
********************************************/

bool Check::checkGeq(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkGeq"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkGeq"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkGeq"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;

long long vnum = 0;
Var::val(var, nlppp->parse_->getVars(), /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum == num)
	return true;
return false;
}


/********************************************
* FN:		CHECKGNE
* CR:		09/20/99 AM.
* SUBJ:	Check if context node's var not equal a number.
* RET:	True if check succeeded.
* FORM:	Gne(var,num)
********************************************/

bool Check::checkGne(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkGne"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkGne"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkGne"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;

long long vnum = 0;
Var::val(var, nlppp->parse_->getVars(), /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum != num)
	return true;
return false;
}


/********************************************
* FN:		CHECKGGE
* CR:		09/20/99 AM.
* SUBJ:	Check if context node's var greater or equal to a number.
* RET:	True if check succeeded.
* FORM:	Gge(var,num)
********************************************/

bool Check::checkGge(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkGge"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkGge"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkGge"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;

long long vnum = 0;
Var::val(var, nlppp->parse_->getVars(), /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum >= num)
	return true;
return false;
}


/********************************************
* FN:		CHECKGLE
* CR:		09/20/99 AM.
* SUBJ:	Check if context node's var lessequal a number.
* RET:	True if check succeeded.
* FORM:	Gle(var,num)
********************************************/

bool Check::checkGle(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;
long long num = 0;

if (!Arg::str1(_T("checkGle"), /*DU*/ ((DELTS*&)args), var))
	return false;
if (!Arg::num1(_T("checkGle"), /*DU*/ ((DELTS*&)args), num))
	return false;
if (!Arg::done(((DELTS*)args), _T("checkGle"),nlppp->parse_))
	return false;

// Get given value of variable from select node.

if (!nlppp->select_)
	return false;

long long vnum = 0;
Var::val(var, nlppp->parse_->getVars(), /*DU*/ vnum);

// Compare variable's value to given value.
if (vnum <= num)
	return true;
return false;
}


/********************************************
* FN:		CHECKDEBUG
* CR:		11/22/99 AM.
* SUBJ:	For developer breakpoints.
* RET:	Always returns true.
* FORM:	debug()
********************************************/

bool Check::checkDebug(
	Delt<Iarg> *args,			// Action's arguments.
	Nlppp *nlppp
	)
{
return true;
}


/********************************************
* FN:		CHECKTRUE
* CR:		11//99 AM.
* SUBJ:	Check if context node's var lessequal a number.
* RET:	True if check succeeded.
* FORM:	Gle(var,num)
* NOTE:	06/10/00 AM. OBSOLETE. USING SUCCEED/FAIL FUNCTIONS.
********************************************/

bool Check::checkTrue(RFASem *val)
{
if (!val)
	return false;
_TCHAR *str;
long long num;
switch (val->getType())
	{
	case RSLONG:	return val->getLong() ? true : false;
	case RSNAME:
	case RSSTR:		// True if a string is not empty.
		if ((str = val->getName()) && *str)
			return true;
		else
			return false;
	case RSNUM:			// "Numeric string".
		// Convert to num.  See if nonzero.
		if (!str_to_long(val->getNum(), /*UP*/ num))
			return false;
		return num ? true : false;
	default:					// Statement type, etc.
		return true;		// Succeed by default.
	}
}
