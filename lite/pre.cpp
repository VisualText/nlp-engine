/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PRE.CPP
* FILE:	lite\pre.cpp
* CR:		11/30/98 AM.
* SUBJ:	Pre actions for Pat pass.
* NOTE:	User can employ pre actions to accept or reject the rule match
*			prior to modifying the parse tree.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"															// 09/26/01 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/24/06 AM.
#include "lite/Auser.h"		// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "chars.h"
#include "string.h"
#include "lite/nlppp.h"	// 07/24/06 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "parse.h"
//#include "lite/nlppp.h"	// 07/24/06 AM.
#include "pat.h"
#include "pn.h"
#include "ivar.h"			// 06/14/05 AM.
#include "regexp.h"		// 03/24/09 AM.

#include "arg.h"
#include "lite/pre.h"

int Pre::count_        = 0;

// For users to be able to register a Pre action class containing
// their pre actions.
static Pre *userpre_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

Pre::Pre()
{
#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

Pre::~Pre()
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
* CR:		11/30/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Pre::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/30/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Pre::prettyCount()
{
if (count_)
	{
	*gout << _T("Active Pre count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Pre count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		PREMATCH
* CR:		11/30/98 AM.
* SUBJ:	See if node conforms to pre-actions.
* RET:	True if successful match, else false.
* NOTE:	06/16/05 AM. Moving leafMatch functionality down here.
*			See obsolete leafMatch for its documentation.
********************************************/

//bool Pat::preMatch(
//	Dlist<Ipre> *pres,
//	Pn *pn
//	)
bool Pat::preMatch(
	Ielt *ielt,			// 06/16/05 AM.
	Node<Pn> *node		// 06/16/05 AM.
	)
{
if (!node || !ielt)	// 06/16/05 AM.
	return false;		// 06/16/05 AM.

Dlist<Ipre> *pres = ielt->getPres();	// 06/16/05 AM.
if (!pres)
	return true;

Delt<Ipre> *dpre;
_TCHAR *func;
Iaction *pre;
Dlist<Iarg> *dargs;
Delt<Iarg> *args;
bool accept = true;			// If accepting current rule match.
for (dpre = pres->getFirst(); dpre; dpre = dpre->Right())
	{
	// Perform the current pre action.
	pre	= dpre->getData();
	func	= pre->getName();
	dargs	= pre->getArgs();
	if (dargs)
		args	= dargs->getFirst();
	else
		args = 0;

	if (!preAction(func, args,
							node ))	// 06/16/05 AM.
		return false;
	}

return true;			// Completed obstacle course.
}


/********************************************
* FN:		PREACTION
* CR:		11/30/98 AM.
* SUBJ:	Execute one pre action on current node.
* RET:	True if successful match, else false.
********************************************/

bool Pat::preAction(
	_TCHAR *func,					// Name of pre action.
	Delt<Iarg> *args,			// Action's arguments.
//	Pn *pn						// Current node being matched.	// 06/16/05 AM.
	Node<Pn> *node				// TOPLEVEL node being matched.	// 06/16/05 AM.
	)
{
if (!node)	// 06/16/05 AM.
	return false;	// 06/16/05 AM.

//if (Debug())
//	*gout << "  [Execute pre action...]" << std::endl;

////////////////////////////////////////
// PRE ACTIONS THAT MATCH TOPLEVEL NODE.							// 06/16/05 AM.
Pn *pn = node->getData();												// 06/16/05 AM.

if (!strcmp_i(func, _T("var")))
	return Pre::preVar(args, pn);						// 06/14/05 AM.
if (!strcmp_i(func, _T("varz")))
	return Pre::preVarz(args, pn);					// 06/16/05 AM.
if (!strcmp_i(func, _T("vareq")))
	return Pre::preVareq(args, pn);					// 06/16/05 AM.
if (!strcmp_i(func, _T("vargt")))
	return Pre::preVargt(args, pn);
if (!strcmp_i(func, _T("varlt")))
	return Pre::preVarlt(args, pn);
if (!strcmp_i(func, _T("varne")))
	return Pre::preVarne(args, pn);					// 06/16/05 AM.
if (!strcmp_i(func, _T("regexp")))
	return Pre::preRegexp(args, pn);					// 03/23/09 AM.
if (!strcmp_i(func, _T("regexpi")))
	return Pre::preRegexpi(args, pn);				// 03/26/09 AM.

////////////////////////////////////////
// PRE ACTIONS THAT MATCH LEAF NODE.							// 06/16/05 AM.

Node<Pn> *leaf;														// 06/16/05 AM.
if (!(leaf = leafNode(node)))										// 06/16/05 AM.
	return false;														// 06/16/05 AM.
pn = leaf->getData();												// 06/16/05 AM.

// Just a simple test of pre actions.
if (!strcmp_i(func, _T("uppercase")))
	return Pre::preUppercase(args, pn);
else if (!strcmp_i(func, _T("lowercase")))								// 01/03/00 AM.
	return Pre::preLowercase(args, pn);								// 01/03/00 AM.
else if (!strcmp_i(func, _T("cap")))
	return Pre::preCap(args, pn);
else if (!strcmp_i(func, _T("length")))
	return Pre::preLength(args, pn);
else if (!strcmp_i(func, _T("lengthr")))
	return Pre::preLengthr(args, pn);
else if (!strcmp_i(func, _T("numrange")))				// 08/10/99 AM.
	return Pre::preNumrange(args, pn);
else if (!strcmp_i(func, _T("unknown")))
	return Pre::preUnknown(args, pn);
else if (!strcmp_i(func, _T("debug")))
	return Pre::preDebug(args, pn);					// 09/17/99 AM.

if (!Pre::userPre(func, args, pn))
	return false;	// Unknown or botched user-defined pre action.
return true;
}


/********************************************
* FN:		USERPRE
* CR:		12/04/98 AM.
* SUBJ:	Execute user-defined preactions on current node, if any.
* RET:	True if successful match, else false.
********************************************/

bool Pre::userPre(_TCHAR *func, Delt<Iarg> *args, Pn *pn)
{
if (userpre_)
	return userpre_->execute(func, args, pn);	// Let pre subclass do it.

std::_t_strstream gerrStr;
gerrStr << _T("[Execute pre action: Unknown action=") << func << _T("].") << std::ends;
errOut(&gerrStr,false);

return false;
}


/********************************************
* FN:		REGPRE
* CR:		12/04/98 AM.
* SUBJ:	Register the user-defined pre-action class.
* NOTE:	Class function.
* ALLOC:	User creates the algorithm instance to add.
*			Expected to be a derived class of Pre.
********************************************/

bool Pre::regPre(
	Pre *pre				// Algorithm class to register.
	)
{
if (userpre_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[regPre: Can only register one pre action class.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
userpre_ = pre;
return true;
}

/********************************************
* FN:		CLEAN
* CR:		12/04/98 AM.
* SUBJ:	Cleanups for pre class.
* NOTE:	Class function.
* ALLOC:	Deletes the user-registered pre action class.
********************************************/

void Pre::clean()
{
if (userpre_)
	{
	delete userpre_;
	userpre_ = 0;
	}
}



/********************************************
* FN:		PRE ACTIONS.
********************************************/


/********************************************
* FN:		PREUPPERCASE
* CR:		11/30/98 AM.
* SUBJ:	Succeed if all of the range of rule elements is all-uppercase.
* RET:	True if pre succeeded.
* NOTE:	Testing the pre action machinery with this first pre.
********************************************/

bool Pre::preUppercase(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
static bool warned = false;
bool ok = true;
if (args && !warned)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Uppercase pre action: Ignoring arguments.]") << std::ends;
	errOut(&gerrStr,false);
	warned = true;			// Don't repeat this message.
	}

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
_TCHAR *buf;
buf = pn->pnStr();			// Create buffer for node's text.
ok = all_uppercase(buf);
Chars::destroy(buf);			// Free up buffer.

return ok;
}


/********************************************
* FN:		PRELOWERCASE
* CR:		01/03/00 AM.
* SUBJ:	Succeed if all of the range of rule elements is all-lowercase.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preLowercase(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
static bool warned = false;
bool ok = true;
if (args && !warned)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Lowercase pre action: Ignoring arguments.]") << std::ends;
	errOut(&gerrStr,false);
	warned = true;			// Don't repeat this message.
	}

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
_TCHAR *buf;
buf = pn->pnStr();			// Create buffer for node's text.
ok = all_lowercase(buf);
Chars::destroy(buf);			// Free up buffer.

return ok;
}


/********************************************
* FN:		PRECAP
* CR:		01/26/99 AM.
* SUBJ:	See if covered nodes are capitalized.
* RET:	True if pre succeeded.
* NOTE:	Succeed if capitalized alphabetic.
*			01/31/99 AM. Fail if not alphabetic at all.  We might be
*			traversing down a singlet, so that if the top node is nonliteral,
*			we'll always succeed, no matter what's underlying it.
********************************************/

bool Pre::preCap(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
static bool warned = false;
bool ok = true;
if (args && !warned)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Cap pre action: Ignoring arguments.]") << std::ends;
	errOut(&gerrStr,false);
	warned = true;			// Don't repeat this message.
	}

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
_TCHAR *buf;
_TCHAR ch;

buf = pn->getName();
if (!(ch = *buf))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Cap pre action: Node with no text.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
if (!alphabetic(ch))			// 09/22/99 AM.
	return false;				// NON ALPHABETIC FAILS.	// 01/31/99 AM.
if (is_upper((_TUCHAR)ch))											// 12/16/01 AM.
	return ok;
return false;
}

/********************************************
* FN:		PREUNKNOWN
* CR:		01/25/99 AM.
* SUBJ:	Check for unknown word.
* RET:	True if pre succeeded.
* NOTE:	Assumes this is applied to alphabetics only.
*			If word hasn't been looked up, not looking it up here.
********************************************/

bool Pre::preUnknown(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
static bool warned = false;
bool ok = true;
if (args && !warned)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("['Unknown' pre action: Ignoring arguments.]") << std::ends;
	errOut(&gerrStr,false);
	warned = true;			// Don't repeat this message.
	}

// Get the node's text.  If alphabetic chars are all uppercase, then
// success.
Sym *sym;
sym = pn->getlcSym();	// Get lowercased sym.
if (sym &&															// 06/05/00 AM.
	sym->isLooked() && !(sym->isKnown()))
	return ok;
return false;
}


/********************************************
* FN:		PRELENGTH
* CR:		01/31/99 AM.
* SUBJ:	Check if length of token is a match.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preLength(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
long long len = 0;

if (!Arg::num1(_T("preLength"), (DELTS*&)args, len))
	return false;
if (!Arg::done(args, _T("preLength")))
	return false;

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(pn->getName()));
const UChar *strBuf = ustr.getTerminatedBuffer();
long uniLen = unicu::strLen(strBuf);

return ((unsigned int) len == uniLen);
}


/********************************************
* FN:		PRELENGTHR
* CR:		03/22/99 AM.
* SUBJ:	Check if length of token is in given number range.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preLengthr(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
long long len1 = 0, len2=0;
long long len = 0;

if (!Arg::num1(_T("preLength"), (DELTS*&)args, len1))
	return false;
if (!Arg::num1(_T("preLength"), (DELTS*&)args, len2))
	return false;
if (!Arg::done(args, _T("preLength")))
	return false;

if (len1 < 0 || len2 < 0 || (len1 > len2))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[LENGTHR pre action: Bad range (") << len1 << _T(",") << len2
		  << _T(")]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(pn->getName()));
const UChar *strBuf = ustr.getTerminatedBuffer();
len = unicu::strLen(strBuf);

if ((len >= len1) && (len <= len2))
	return true;
return false;
}


/********************************************
* FN:		PRENUMRANGE
* CR:		08/10/99 AM.
* SUBJ:	See if numeric token is in given range.
* RET:	True if pre succeeded.
* FORM:	numrange(LOWNUM, HIGHNUM)
*				These are INCLUSIVE numbers.
********************************************/

bool Pre::preNumrange(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
long long len1 = 0, len2=0;

if (!Arg::num1(_T("preNumrange"), (DELTS*&)args, len1))
	return false;
if (!Arg::num1(_T("preNumrange"), (DELTS*&)args, len2))
	return false;
if (!Arg::done(args, _T("preNumrange")))
	return false;

if (len1 < 0 || len2 < 0 || (len1 > len2))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[NUMRANGE pre action: Bad range (") << len1 << _T(",") << len2
		  << _T(")]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

long long num = 0;
_TCHAR *str;
str = pn->getName();
if (!str_to_long(str, /*UP*/ num))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[NUMRANGE pre action: Bad num=") << str << _T("]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

if (num >= len1 && num <= len2)
	return true;			// Number is within range.
return false;
}


/********************************************
* FN:		PREDEBUG
* CR:		09/17/99 AM.
* SUBJ:	Always succeed!  A way for programmer to place a breakpoint.
* RET:	True.
* NOTE:	Programmer must set a breakpoint manually. (Would be nice to
*			do what VC++ does when user sets a breakpoint.)
********************************************/

bool Pre::preDebug(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
if (!Arg::done(args, _T("preDebug")))
	return false;

// SET A BREAKPOINT HERE!
return true;
}


/********************************************
* FN:		PREVAR
* CR:		06/14/05 AM.
* SUBJ:	Check if node's var has non-empty value.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preVar(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;

if (!Arg::str1(_T("preVar"), (DELTS*&)args, str))
	return false;
if (!Arg::done(args, _T("preVar")))
	return false;

return Ivar::nodeVarNZ(pn,str);
}


/********************************************
* FN:		PREVARZ
* CR:		06/16/05 AM.
* SUBJ:	Check if node's var has empty value.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preVarz(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;

if (!Arg::str1(_T("preVarz"), (DELTS*&)args, str))
	return false;
if (!Arg::done(args, _T("preVarz")))
	return false;

return !Ivar::nodeVarNZ(pn,str);
}


/********************************************
* FN:		PREVAREQ
* CR:		06/16/05 AM.
* SUBJ:	Check if node's var equals value.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preVareq(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;
_TCHAR *sval;
long long nval;

if (!Arg::str1(_T("preVareq"), (DELTS*&)args, str))
	return false;
if (!Arg::str_or_num1(_T("preVareq"), (DELTS*&)args, sval,nval))
	return false;
if (!Arg::done(args, _T("preVareq")))
	return false;

if (sval && *sval)
  return Ivar::nodeVarEQ(pn,str,sval);
else
  return Ivar::nodeVarEQ(pn,str,nval);
}



/********************************************
* FN:		PREVARGT
* CR:		02/03/23 Dd.
* SUBJ:	Check if node's var greater than a number.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preVargt(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;
_TCHAR *sval;
long long nval;

if (!Arg::str1(_T("preVargt"), (DELTS*&)args, str))
	return false;
if (!Arg::str_or_num1(_T("preVargt"), (DELTS*&)args, sval,nval))
	return false;
if (!Arg::done(args, _T("preVargt")))
	return false;

if (sval && *sval)
  return false;
else
  return Ivar::nodeVarGTLT(pn,str,nval,false);
}


/********************************************
* FN:		PREVARGT
* CR:		02/07/23 Dd.
* SUBJ:	Check if node's var less than a number.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preVarlt(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;
_TCHAR *sval;
long long nval;

if (!Arg::str1(_T("preVarlt"), (DELTS*&)args, str))
	return false;
if (!Arg::str_or_num1(_T("preVarlt"), (DELTS*&)args, sval,nval))
	return false;
if (!Arg::done(args, _T("preVarlt")))
	return false;

if (sval && *sval)
  return false;
else
  return Ivar::nodeVarGTLT(pn,str,nval,true);
}




/********************************************
* FN:		PREVARNE
* CR:		06/16/05 AM.
* SUBJ:	Check if node's var does not equal value.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preVarne(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;
_TCHAR *sval;
long long nval;

if (!Arg::str1(_T("preVareq"), (DELTS*&)args, str))
	return false;
if (!Arg::str_or_num1(_T("preVareq"), (DELTS*&)args, sval,nval))
	return false;
if (!Arg::done(args, _T("preVareq")))
	return false;

if (sval && *sval)
  return !Ivar::nodeVarEQ(pn,str,sval);
else
  return !Ivar::nodeVarEQ(pn,str,nval);
}



/********************************************
* FN:		PREREGEXP
* CR:		03/23/09 AM.
* SUBJ:	Match node to regular expression.
* RET:	True if pre succeeded.
********************************************/

bool Pre::preRegexp(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;

if (!Arg::str1(_T("preRegexp"), (DELTS*&)args, str))
	return false;
if (!Arg::done(args, _T("preRegexp")))
	return false;

// Get node's text.
if (!pn)
	return false;
_TCHAR *nstr = pn->pnStr();
bool ok = Regexp::regexp_match(nstr,str);
Chars::destroy(nstr);			// Free up buffer.
return ok;
}
/********************************************
* FN:		PREREGEXPI
* CR:		03/26/09 AM.
* SUBJ:	Match node to regular expression. (Case insensitive)
* RET:	True if pre succeeded.
********************************************/

bool Pre::preRegexpi(
	Delt<Iarg> *args,			// Action's arguments.
	Pn *pn						// Node to match.
	)
{
_TCHAR *str;

if (!Arg::str1(_T("preRegexpi"), (DELTS*&)args, str))
	return false;
if (!Arg::done(args, _T("preRegexpi")))
	return false;

// Get node's text.
if (!pn)
	return false;
_TCHAR *nstr = pn->pnStr();
bool ok = Regexp::regexpi_match(nstr,str);
Chars::destroy(nstr);			// Free up buffer.
return ok;
}

