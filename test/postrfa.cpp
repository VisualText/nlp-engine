/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	POSTRFA.CPP
* FILE:	lite\postrfa.cpp
* CR:		11/05/98 AM.
* SUBJ:	Post actions for the RFA.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "lite/Auser.h"	// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "chars.h"		// 02/11/00 AM.
#include "io.h"
#include "string.h"
#include "lite/nlppp.h"		// 11/20/99 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "iexpr.h"		// 11/10/99 AM.
#include "ivar.h"			// 11/09/99 AM.
#include "iop.h"			// 11/09/99 AM.
#include "iexprstmt.h"	// 11/12/99 AM.
#include "iifstmt.h"		// 11/12/99 AM.
#include "iwhilestmt.h"	// 08/31/00 AM.
#include "ireturn.h"		// 03/07/02 AM.
#include "parse.h"
#include "post.h"
#include "rfasem.h"
#include "arg.h"
#include "ipnode.h"		// 10/18/00 AM.
#include "ifunc.h"		// 12/19/01 AM.
#include "pat.h"
#include "ifile.h"						// 02/27/01 AM.
#include "irecurse.h"					// 02/27/01 AM.
#include "var.h"			// 08/31/00 AM.
#include "postrfa.h"

bool PostRFA::debug_   = false;
bool PostRFA::verbose_ = false;

#ifndef STABLE_
int PostRFA::count_ = 0;
#endif

/********************************************
* FN:		Special Functions for Class
********************************************/

PostRFA::PostRFA(bool debug)
{
debug_ = debug;
verbose_ = false;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

PostRFA::~PostRFA()
{
#ifndef STABLE_
--count_;
#endif
}


/*******************************************/


/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

bool PostRFA::Debug()	{return debug_;}
bool PostRFA::Verbose()	{return verbose_;}


/********************************************
* FN:		Modify Functions
********************************************/

void PostRFA::setDebug(bool x)	{debug_ = x;}
void PostRFA::setVerbose(bool x)	{debug_ = x;}

/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int PostRFA::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void PostRFA::prettyCount(_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active PostRFA count=") << count_ << endl;
	_t_strstream gerrStr;
	gerrStr << _T("Active PostRFA count=") << count_ << ends;
	errOut(&gerrStr,false);
	}
}
#endif


/********************************************
* FN:		POSTRFANAME
* CR:		11/10/98 AM.
* SUBJ:	Create a semantic object to hold node's proper name.
* RET:	True if ok, else false.
* RULE:	_LIT <- _xALPHA @@
* FORM:	postRFAname("1") -- arg is for the rhs token.
********************************************/

bool PostRFA::postRFAname(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA name action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA name action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA name action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA name action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (!num1)
	return false;

// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelt;
nelt = coll1->Down();

if (Verbose())
	*gout << _T("   [Executing RFA name action.]") << endl;

if (Debug())
	{
	*gout << _T("token=") << *nelt << endl;
	}

////////////////////////
// BUILD SEM FOR NAME
////////////////////////

// GET DATA.
Pn *pn;
_TCHAR *name;
pn = nelt->getData();
//name = pn->pnStr();		// 11/19/98 AM.
// Matched token node should have name equal its text.
name = pn->getName();		// Should be token node's name.

RFASem *rfasem;
rfasem = new RFASem(name);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFANUM
* CR:		11/21/98 AM.
* SUBJ:	Create a semantic object for numeric token.
* RET:	True if ok, else false.
* RULE:	_NUM <- _xNUM @@
* FORM:	postRFAnum("1") -- arg is for the rhs token.
********************************************/

bool PostRFA::postRFAnum(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA num action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA num action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA num action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA num action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelt;
nelt = coll1->Down();

if (Verbose())
	*gout << _T("   [Executing RFA num action.]") << endl;

if (Debug())
	{
	*gout << _T("token=") << *nelt << endl;
	}

////////////////////////
// BUILD SEM FOR NAME
////////////////////////

// GET DATA.
Pn *pn;
_TCHAR *name;
pn = nelt->getData();
//name = pn->pnStr();		// 11/19/98 AM.
// Matched token node should have name equal its text.
name = pn->getName();		// Should be token node's name.

RFASem *rfasem;
// Converting to numeric.						// 11/15/99 AM.
//long num;											// 11/15/99 AM.
//if (!str_to_long(name, /*UP*/ num))			// 11/15/99 AM.
//	{
//	*gout << "[Couldn't convert to num=" << name << "]" << endl;
//	return false;
//	}
//rfasem = new RFASem(num);						// 11/15/99 AM.

// NO! KEEPING NUMS AS STRINGS TILL FIND OUT OTHERWISE.	// 11/15/99 AM.
rfasem = new RFASem(name, RSNUM);

nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAFLOAT
* CR:		08/19/01 AM.
* SUBJ:	Create a semantic object for floating point number.
* RET:	True if ok, else false.
* RULE:	_FLOAT <- _xNUM \. _xNUM @@
* FORM:	postRFAfloat("1","3")
********************************************/

bool PostRFA::postRFAfloat(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA float action: Two args required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA float action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1 = args->getData();

if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA float action: Missing arg.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

Iarg *arg2 = args->getData();

if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA float action: Too many args.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())
	{
	case IANUM:
		num1 = arg1->getNum();
		break;
	case IASTR:
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[RFA float action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
	}

long num2;
switch (arg2->getType())
	{
	case IANUM:
		num2 = arg2->getNum();
		break;
	case IASTR:
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		_t_strstream gerrStr;
		gerrStr << _T("[RFA float action: Arg must be string.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
	}

// Check self-consistency.
if (!num1 || !num2 || (num1 >= num2))
	return false;

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1, *coll2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;
while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		break;
	}
coll2 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelt1, *nelt2;
nelt1 = coll1->Down();
nelt2 = coll2->Down();

if (Verbose())
	*gout << _T("   [Executing RFA float action.]") << endl;

if (Debug())
	{
	*gout << _T("token=") << *nelt1 << _T(".") << *nelt2 << endl;
	}

////////////////////////
// BUILD SEM OBJECT
////////////////////////

// GET DATA.
Pn *pn;
_TCHAR *part1, *part2;
pn = nelt1->getData();
//name = pn->pnStr();
// Matched token node should have name equal its text.
part1 = pn->getName();		// Should be token node's name.

pn = nelt2->getData();
part2 = pn->getName();

float num = 0.0;
if (!convert_to_float(part1,part2, /*UP*/ num))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA float action: Couldn't build float number.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

RFASem *rfasem = new RFASem(num);

nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFANONLIT
* CR:		11/10/98 AM.
* SUBJ:	Create a semantic object to hold nonlit node's proper name.
* RET:	True if ok, else false.
* RULE:	_NONLIT <- \_ _xALPHA @@
* FORM:	postRFAnonlit("2") -- arg is for the rhs alphabetic token.
* NOTE:	Could have a generic glom text action, but keeping this for
*			RFA-specific semantic work, if any.
********************************************/

bool PostRFA::postRFAnonlit(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nonlit action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nonlit action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nonlit action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA nonlit action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (!num1)
	return false;

// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelt;
nelt = coll1->Down();

if (Verbose())
	*gout << _T("   [Executing RFA nonlit action.]") << endl;

if (Debug())
	{
	*gout << _T("token=") << *nelt << endl;
	}

////////////////////////
// BUILD SEM FOR NONLIT
////////////////////////

// GET DATA.
Pn *pn;
_TCHAR *text;
long len;
pn = nelt->getData();
text = pn->getText();
len = pn->getEnd() - pn->getStart() + 1;

// Glom underscore back onto node's text.
_TCHAR buf[MAXSTR];					// Build string in here.
_TCHAR *ptr;
long count;
count = MAXSTR;					// Track space left in buffer.
buf[0] = '\0';
ptr = &(buf[0]);					// Track first empty loc in buffer.
strcat_e(ptr, _T("_"), count);
if (!strncat_e(ptr, text, len, count))
	return false;

_TCHAR *name;
//name = make_str(buf);		// 11/19/98 AM.
Sym *sym;
sym = nlppp->parse_->getSym(buf);
assert(sym);
name = sym->getStr();		// 11/19/98 AM.

RFASem *rfasem;
rfasem = new RFASem(name);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFASTR
* CR:		11/19/98 AM.
* SUBJ:	Create a string semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_STR <- \" _xWILD [+] \" @@
* FORM:	postRFAstr(2) -- arg is for the contents of the string.
********************************************/

bool PostRFA::postRFAstr(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA str action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA str action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA str action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA str action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelts, *nend;
nelts = coll1->Down();
nend  = coll1->eDown();

// 11/26/98 AM. Should probably handle empty string here.
// Also, case where nend == 0.
if (!nelts || !nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA str action: Unhandled null case.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}


if (Verbose())
	*gout << _T("   [Executing RFA str action.]") << endl;

if (Debug())
	{
	*gout << _T("elements=");
	Pn::prettyNodes(nelts);
	*gout << endl;
	}

////////////////////////
// BUILD STRING
////////////////////////
// Traverse collect elements, gathering a string
// Be careful with backslashed chars in a string.
// Take what is in their semantic name field.
// Then add backslashes as appropriate for C++.
// For all others, just get their covered text.
// Throw the built string into the string table.

Node<Pn> *bound;
if (nend && nend->Right())
	bound = nend->Right();
else
	bound = 0;
// abcdefghijklmnopqrstuvwxyz  -- Sammy 11/19/98
RFASem *semelt;
Pn *pn;

_TCHAR buf[MAXSTR];					// Build string in here.
_TCHAR *ptr;
long count;
count = MAXSTR;					// Track space left in buffer.
buf[0] = '\0';
ptr = &(buf[0]);					// Track first empty loc in buffer.
_TCHAR *tmpstr;																// 09/23/00 AM.

for (; nelts != bound; nelts = nelts->Right())
	{
	// Get semantics of element if present, else use coverage.
	pn = nelts->getData();
	assert(pn);
	semelt = (RFASem *) pn->getSem();
	if (semelt && (semelt->getType() == RSNAME))
		{
		// Workaround: Since strings get "reglommed", if a \ is seen,
		// undo it back to a \\.								// FIX	// 09/23/00 AM.
		tmpstr = semelt->getName();										// 09/23/00 AM.
		if (tmpstr && *tmpstr == '\\' && !*(tmpstr+1))				// 09/23/00 AM.
			tmpstr = _T("\\\\");			// UNDO GLOMMING.		// FIX.	// 09/23/00 AM.

		// WARN: ptr is modified to be at end of new string.
		if (!strcat_e(ptr, tmpstr, count))								// 09/23/00 AM.
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA str action: Failed to build string.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;
			}
		}
	else
		{
		// Get node's covered text here.
		_TCHAR *text;
		long len;
		text = pn->getText();
		len = pn->getEnd() - pn->getStart() + 1;
		if (!strncat_e(ptr, text, len, count))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA str action: Failed to build string.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;
			}
		}

	if (semelt)
		{
		// DELETE THE SEMANTIC OBJECT.
		delete (RFASem *) semelt;
		pn->setSem(0);
		}
	}

// RFB grammar now collects strings, comments, and backslashed chars
// in the same pass (retok.pat).  Therefore, need to restore this
// C string conversion.														// 08/19/00 AM.
// Since RFA and RFB grammars have rules to collect backslashed
// chars, this conversion is INVALID!									// 08/10/00 AM.
// BUG FIX. Convert string with backslashes to C string.			// 12/15/99 AM.
_TCHAR cbuf[MAXSTR];	// For converting to C string.				// 08/19/00 AM.
cbuf[0] = '\0';																// 08/19/00 AM.
str_to_cstr(buf, /*UP*/ cbuf);											// 08/19/00 AM.


// Internalize the string in hash and string table. (Don't really need hash.)
//*gout << "[postRFAstr: str=\"" << buf << "\"]" << endl;
Sym *sym = 0;
_TCHAR *str = 0;				// 12/04/98 AM.
if (!empty(cbuf))																// 08/19/00 AM.
//if (!empty(buf))															// 08/19/00 AM.
	{
	sym = nlppp->parse_->getSym(cbuf);									// 08/19/00 AM.
//	sym = nlppp->parse_->getSym(buf);									// 08/19/00 AM.
	str = sym->getStr();
	}
else
	return false;

// Create semantic object for the STR.
RFASem *rfasem;
rfasem = new RFASem(str, RSSTR);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAARG
* CR:		11/21/98 AM.
* SUBJ:	Create an arg semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_ARG <- _NONLIT @@
*			_ARG <- _LIT @@
*			_ARG <- _STR @@
*			_ARG <- _NUM @@
* FORM:	postRFAarg("1")
* NOTE:	All these guys being treated as strings.
********************************************/

bool PostRFA::postRFAarg(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA arg action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA arg action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}


Iarg *arg1;
arg1 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA arg action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA arg action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (num1 <= 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Bad num]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// num1 is collect for element.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Bad collect.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}
coll1 = colls;

// Get needed stuff from each collect.
Node<Pn> *nname;
nname = coll1->Down();


////////////////////////
// BUILD AND ATTACH ARG
////////////////////////

// Get semantics from the kiddies.
RFASem *semname;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA arg action: No semantic object for name.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// 11/15/99 AM. HANDLING NUMERIC ARGUMENT TYPE.
Iarg *iarg;				// Sem to build.  Treat all types as string.
_TCHAR *name;
switch (semname->getType())											// 11/15/99 AM.
	{
	case RSLONG:								// 11/15/99 AM. Pure numeric.
		iarg = new Iarg(semname->getLong());						// 11/15/99 AM.
		break;
	case RSFLOAT:
		iarg = new Iarg(semname->getFloat());							// 08/19/01 AM.
		break;
	case RSNUM:						// "Bastard". Numeric as string.
		// NOTE: In the RFA, everything will continue to be parsed as
		// STRING!  But will have a different variant of this function
		// in the RFB.														// 12/18/99 AM.
#ifdef RFB_
		// OVERHAUL. TRYING TO TREAT THIS AS NUMBER!				// 12/18/99 AM.
		str = semname->getNum();										// 12/18/99 AM.
		if (!str_to_long(str, /*UP*/ num))							// 12/18/99 AM.
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA arg: Couldn't convert to num.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;													// 12/18/99 AM.
			}
		iarg = new Iarg(num);											// 12/18/99 AM.
		break;
#endif
	case RSNAME:
	case RSSTR:
		if (!(name = semname->getName()))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Semname problem.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;
			}
		iarg = new Iarg(name);
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA arg action: Bad semantic type.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
delete (RFASem *) semname;
nname->getData()->setSem(0);

RFASem *rfasem;
rfasem = new RFASem(iarg);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAARGTOLIST
* CR:		11/21/98 AM.
* SUBJ:	Convert an arg to a list for RFA pair.
* RET:	True if ok, else false.
* RULE:	_LIST <- _ARG @@
* FORM:	postRFAargtolist("1")
********************************************/

bool PostRFA::postRFAargtolist(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA argtolist action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA argtolist action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}


Iarg *arg1;
arg1 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA argtolist action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA argtolist action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (num1 <= 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Bad num]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// num1 is collect for element.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Bad collect.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}
coll1 = colls;

// Get needed stuff from each collect.
Node<Pn> *niarg;
niarg = coll1->Down();


////////////////////////
// BUILD AND ATTACH LIST
////////////////////////

// Get semantics from the kiddies.
RFASem *semiarg;
if (!(semiarg = (RFASem *) niarg->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA argtolist action: No semantic object for arg.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Dlist<Iarg> *iargs;				// Sem to build.  Treat all types as string.
Iarg *iarg;

if (!(iarg = semiarg->getArg()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Sem arg problem.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
semiarg->setArg(0);
delete semiarg;
niarg->getData()->setSem(0);

iargs = new Dlist<Iarg>(iarg);
//delete iarg;			// 07/02/99 AM.
RFASem *rfasem;
rfasem = new RFASem(iargs);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFARANGE
* CR:		11/29/98 AM.
* SUBJ:	Recognize a range of elements.
* RET:	True if ok, else false.
* RULE:	_PREPAIR <- \< _NUM _NUM \> @@
* FORM:	rfarange(2,3)
********************************************/

bool PostRFA::postRFArange(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_0to2(_T("range"), args, nlppp->collect_, nlppp->sem_,
										/*DU*/ nstart, nend))
	return false;

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semstart, *semend;
semstart = semend = 0;

semstart = (RFASem *) nstart->getData()->getSem();
semend   = (RFASem *) nend->getData()->getSem();

// Converting to numeric type.							// 11/15/99 AM.
//if (semstart && semstart->getType() != RSNUM)		// 11/15/99 AM.
long lend = 0, lstart = 0;				// 11/15/99 AM.
if (semstart)													// 11/15/99 AM.
	{
	switch (semstart->getType())							// 11/15/99 AM.
		{
		case RSNUM:
			if (!str_to_long(semstart->getNum(), lstart))
				{
				nlppp->parse_->setIntern(false);	// Syntax error.	// 05/26/01 AM.
				_t_strstream gerrStr;
				gerrStr << _T("[RFA range action: Nonnumeric start.]") << ends;
				return nlppp->parse_->errOut(&gerrStr,false,true);
				}
			break;
		case RSLONG:											// 11/15/99 AM.
			lstart = semstart->getLong();					// 11/15/99 AM.
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA range action: Bad start sem object.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	delete semstart;
	nstart->getData()->setSem(0);
	}
else
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA range action: No start sem object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semend)													// 11/15/99 AM.
	{
	switch (semend->getType())							// 11/15/99 AM.
		{
		case RSNUM:
			if (!str_to_long(semend->getNum(), lend))
				{
				nlppp->parse_->setIntern(false);	// Syntax error.	// 05/26/01 AM.
				_t_strstream gerrStr;
				gerrStr << _T("[RFA range action: Nonnumeric end.]") << ends;
				return nlppp->parse_->errOut(&gerrStr,false,true);
				}
			break;
		case RSLONG:											// 11/15/99 AM.
			lend = semend->getLong();					// 11/15/99 AM.
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA range action: Bad end sem object.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;
			}
		}
	delete semend;
	nend->getData()->setSem(0);
	}
else
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA range action: No end sem object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Build an args semantic object with one arg for start, one for end.
Dlist<Iarg> *iargs;
iargs = new Dlist<Iarg>();			// Empty list.
// 07/02/99 AM. No longer copies.
//Iarg iarg(IASTR);								// Temporary guy.
Iarg *iarg;			// 07/02/99 AM.
iarg = new Iarg(IANUM);		// 11/15/99 AM.
iarg->setNum(lstart);		// 11/15/99 AM.
//iarg->setStr(sstart);
iargs->rpush(iarg);
iarg = new Iarg(IANUM);		// 2nd arg. // 11/15/99 AM.
iarg->setNum(lend);							// 11/15/99 AM.
//iarg->setStr(send);
iargs->rpush(iarg);

//delete X;									// Keep for consistency.	
RFASem *rfasem;
rfasem = new RFASem(iargs);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFALIST
* CR:		11/20/98 AM.
* SUBJ:	Create an arg-list semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_LIST <- \( _xWILD \) @@
* FORM:	postRFAlist("2") -- arg is for the args.
* ASS:	Because of recursive rule invocation, the wildcard should be all
*			set up with a list of args!
********************************************/

bool PostRFA::postRFAlist(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA list action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA list action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA list action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA list action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for args.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nargs, *nend;
nargs = coll1->Down();
nend  = coll1->eDown();

if (!nargs && !nend)			// Handling zero args.	// 06/16/99 AM.
	return true;

if (!nargs || !nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA list action: Unhandled null case.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

if (Verbose())
	*gout << _T("   [Executing RFA list action.]") << endl;

if (Debug())
	{
	*gout << _T("args=");
	Pn::prettyNodes(nargs);
	*gout << endl;
	}

////////////////////////
// BUILD LIST
////////////////////////
// Traverse collect elements, gathering a list.
Dlist<Iarg> *iargs;
iargs = new Dlist<Iarg>();			// Empty list.
Node<Pn> *bound;
if (nend && nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semarg;
Pn *pn;
Iarg *tmp;
for (; nargs != bound; nargs = nargs->Right())
	{
	// Get semantics of arg, copy, and glom onto list.
	pn = nargs->getData();
	assert(pn);
	semarg = (RFASem *) pn->getSem();
	if (!semarg || (semarg->getType() != RSARG))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA list action: Non-argument in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		//delete iargs;					// 11/21/98 AM.
		Dlist<Iarg>::DeleteDlistAndData(iargs);	// 11/21/98 AM.
		return false;
		}
	tmp = semarg->getArg();
	assert(tmp);

	semarg->setArg(0);				// Wipe out the original ptr.
	delete semarg;
	pn->setSem(0);

	iargs->rpush(tmp);					// Create new elt.
	//tmp->setX(0);						// Wipe it out.
	// 07/02/99 AM. No longer copies.
	//delete tmp;			// 07/02/99 AM.

	}

RFASem *rfasem;
rfasem = new RFASem(iargs);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAACTION
* CR:		11/23/98 AM.
* SUBJ:	Create an action semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_ACTION <- _PREPAIR [opt] _LIT _LIST @@
*			_ACTION <- _LIT \( \) @@	 # Basically, accept empty list.
* FORM:	postRFAaction("1", "2")
*			postRFAaction("1")			 # Accept one arg.
* NOTE:	11/29/98 AM. Adding the "prepair" to rule and action.
********************************************/

bool PostRFA::postRFAaction(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *npre, *nname, *nlist;
npre = nname = nlist = 0;

if (!args_0to3(_T("action"),args,nlppp->collect_,nlppp->sem_, /*DU*/ npre,nname,nlist))
	return false;

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *sempre, *semname, *semlist;
if (npre)
	sempre = (RFASem *) npre->getData()->getSem();
else
	sempre = 0;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	return false;
if (nlist)
	semlist = (RFASem *) nlist->getData()->getSem();
else
	semlist = 0;

// Some more checking.  May also want to check on node names.
// (ie, are they _LIT and _LIST.  If not, fail.)
if (sempre && sempre->getType() != RSARGS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA action action: Bad semantic type.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semname->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA action action: Bad semantic type.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semlist && semlist->getType() != RSARGS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA action action: Bad semantic type.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Dlist<Iarg> *prepair;
_TCHAR *name;
Dlist<Iarg> *iargs;
prepair = iargs = 0;

if (sempre && !(prepair = sempre->getArgs()))
	return false;
if (!(name = semname->getName()))
	return false;
if (semlist && !(iargs = semlist->getArgs()))
	return false;

// 11/29/98 AM. If the prepair is present, get start and end.
long start, end;
if (prepair)
	{
	Delt<Iarg> *ptr;
	ptr = prepair->getFirst();
	// Unpack the start and end args.
	if (!Arg::num1(_T("RFA Action"), (DELTS*&)ptr, start))
		return false;
	if (!Arg::num1(_T("RFA Action"), (DELTS*&)ptr, end))
		return false;
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
if (sempre)
	{
	sempre->setArgs(0);
	delete sempre;
	npre->getData()->setSem(0);
	}

long line = nname->getData()->getLine();	// 07/29/03 AM.

delete semname;
nname->getData()->setSem(0);

if (semlist)
	{
	semlist->setArgs(0);
	delete semlist;
	nlist->getData()->setSem(0);
	}

RFASem *rfasem;

// 11/29/98 AM. If the prepair is present, get start and end.
if (prepair)
	{
	Ipre *pre;
	pre = new Ipre(name, iargs, start, end);
	rfasem = new RFASem(pre);
	//delete prepair;				// Just using its values.
	Dlist<Iarg>::DeleteDlistAndData(prepair);	// Just using its values.
	}
else
	{
	Iaction *action;				// Object to build.
	action = new Iaction(name, iargs,
		line);						// 07/29/03 AM.
	rfasem = new RFASem(action);
	}

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFALITTOPAIR
* CR:		11/22/98 AM.
* SUBJ:	Convert a literal to a pair.
* RET:	True if ok, else false.
* RULE:	_PAIR <- _LIT @@
* FORM:	postRFAlittopair("1")
********************************************/

bool PostRFA::postRFAlittopair(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA littopair action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA littopair action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}


Iarg *arg1;
arg1 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA littopair action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA littopair action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (num1 <= 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Bad num]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// num1 is collect for element.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Bad collect.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}
coll1 = colls;

// Get needed stuff from each collect.
Node<Pn> *nlit;
nlit = coll1->Down();


////////////////////////
// BUILD AND ATTACH PAIR
////////////////////////

// Get semantics from the kiddies.
RFASem *semlit;
if (!(semlit = (RFASem *) nlit->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA littopair action: No semantic object for lit.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Ipair *pair;				// Sem to build.
_TCHAR *name;

if (!(name = semlit->getName()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Sem lit problem.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
//semlit->setName(0);			// Not needed.  Keep for consistency.
delete semlit;
nlit->getData()->setSem(0);

pair = new Ipair(name, 0);			// Build pair with empty val.
//delete X;								// Keep for consistency.	
RFASem *rfasem;
rfasem = new RFASem(pair);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAPAIR
* CR:		11/21/98 AM.
* SUBJ:	Create a pair semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_PAIR <- _LIT \= _xWILD [1,1][matches...] @@
* FORM:	postRFApair("1", "3")
* NOTE:	Could trap things like [trigger=] where someone forgot to put
*			in a value.  But that's in the noise for now.
********************************************/

bool PostRFA::postRFApair(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pair action: Two args required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pair action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}


Iarg *arg1;
Iarg *arg2;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pair action: Missing second argument.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
arg2 = args->getData();

if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pair action: More than two args given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA pair action: Arg1 must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA pair action: Arg2 must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;
num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// num1 is collect for key name (_nonlit or _lit guy).
// num2 is distance to val.
Node<Pn> *colls;
Node<Pn> *coll1, *coll2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;
while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		break;
	}
coll2 = colls;

// Get needed stuff from each collect.
Node<Pn> *nkey, *nval;
if (!(nkey = coll1->Down()))
	return false;


nval = coll2->Down();

////////////////////////
// BUILD AND ATTACH PAIR
////////////////////////

// Get semantics from the kiddies.
RFASem *semkey, *semval;
if (!(semkey = (RFASem *) nkey->getData()->getSem()))
	return false;
if (!(semval = (RFASem *) nval->getData()->getSem()))
	return false;

// Some more checking.  May also want to check on node names also.
// (ie, are they _KEY and _VAL.  If not, fail.)
if (semkey->getType() != RSNAME || semval->getType() != RSARGS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pair action: Bad semantic type.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Ipair *pair;
_TCHAR *name;
Dlist<Iarg> *iargs;

if (!(name = semkey->getName()))
	return false;
if (!(iargs = semval->getArgs()))
	return false;

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
delete semkey;
nkey->getData()->setSem(0);

semval->setArgs(0);
delete semval;
nval->getData()->setSem(0);

pair = new Ipair(name, iargs);

RFASem *rfasem;
rfasem = new RFASem(pair);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAPAIRS
* CR:		11/06/98 AM.
* SUBJ:	Create a pairs semantic object for RFA.
* NOTE:	11/22/98 AM. Finally implementing this properly!
* RET:	True if ok, else false.
* RULE:	_PAIRS <- [ _xWILD ] @@
* FORM:	postRFApairs("2") -- arg is for the wildcard elt.
********************************************/

bool PostRFA::postRFApairs(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pairs action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pairs action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA pairs action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA pairs action: Arg must be string.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for pairs.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *npair, *nend;
npair = coll1->Down();
nend  = coll1->eDown();

if (!npair || !nend)
	{
//	*gerr << "[RFA pairs action: Unhandled null case.]" << endl;
//	return false;
	// Empty pairs.  // 06/04/99 AM.
	nlppp->sem_ = 0;				// Try this.
	return true;		// Try this.
	}

if (Verbose())
	*gout << _T("   [Executing RFA pairs action.]") << endl;

if (Debug())
	{
	*gout << _T("pairs=");
	Pn::prettyNodes(npair);
	*gout << endl;
	}


////////////////////////
// BUILD PAIRS
////////////////////////
// Traverse collect elements, gathering a list of pairs.
Dlist<Ipair> *ipairs;
ipairs = new Dlist<Ipair>();			// Empty list.
Node<Pn> *bound;
if (nend && nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *sempair;
Pn *pn;
Ipair *tmp;
for (; npair != bound; npair = npair->Right())
	{
	// Get semantics of pair, copy, and glom onto list.
	pn = npair->getData();
	assert(pn);
	sempair = (RFASem *) pn->getSem();
	if (!sempair || (sempair->getType() != RSPAIR))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA pair action: Non-pair in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		//delete ipairs;
		Dlist<Ipair>::DeleteDlistAndData(ipairs);
		return false;
		}
	tmp = sempair->getPair();
	assert(tmp);

	sempair->setPair(0);				// Wipe out the original ptr.
	delete sempair;
	pn->setSem(0);

	// 07/02/99 AM. No longer copies.
	ipairs->rpush(tmp);					// Create new elt.
	//tmp->setVals(0);					// Zero it out.
	//delete tmp;							// Delete the copied elt.
	}

RFASem *rfasem;
rfasem = new RFASem(ipairs);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFALITELT
* CR:		11/08/98 AM.
* SUBJ:	Create an element semantic object for RFA.
* NOTE:	Placeholder, for now.
* RET:	True if ok, else false.
* RULE:	_ELEMENT <- _LIT _PAIRS [optional] @@
* FORM:	postRFAlitelt("1", "2") -- if pairs.
*			postRFAlitelt("1")		-- if no pairs.
********************************************/

bool PostRFA::postRFAlitelt(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nname, *npairs;
nname = npairs = 0;

if (!args_1or2(_T("litelt"), args, nlppp->collect_, nlppp->sem_,
												/*DU*/ nname, npairs))
	return false;


////////////////////////
// BUILD AND ATTACH PAIRS
////////////////////////

// Get semantics from the kiddies.
RFASem *semname, *sempairs;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	return false;
if (npairs)
	sempairs = (RFASem *) npairs->getData()->getSem();
else
	sempairs = 0;					// 11/11/98 AM. Bug fix.

// Note: should more properly build Ielement here, but this is ok.
Ielt *elt;						// Internal data structure to build.
// Just do Ielement semantics here.
// Don't get Ielement's attrs here. Defer till you decide if this
// is an ELT or a SUGG.
// Get name, pairs.
Dlist<Ipair> *pairs;
_TCHAR *name;

if (sempairs && !(pairs = sempairs->getPairs()))
	return false;
else
	pairs = 0;
if (!(name = semname->getName()))
	return false;

// Delete the old semantic objects.		// 11/17/98 AM.
// Clear semantic object data so it won't be deleted.
delete semname;								// 11/17/98 AM.
nname->getData()->setSem(0);
if (sempairs)									// 11/17/98 AM.
	{
	sempairs->setPairs(0);
	delete sempairs;
	npairs->getData()->setSem(0);
	}

// Get pass line number from pairs.										// 08/08/01 AM.
Pn *pn = 0;																		// 08/08/01 AM.
if (npairs)																		// 08/08/01 AM.
	pn = npairs->getData();													// 08/08/01 AM.
else																				// 08/08/01 AM.
	pn = nname->getData();													// 08/08/01 AM.

elt = new Ielt(name, pairs,
	pn->getLine());				// Get line num in pass.			// 08/08/01 AM.
assert(elt);
elt->fillDefaults();		// Set defaults for an elt.				// 11/11/98 AM.

RFASem *rfasem;
rfasem = new RFASem(elt);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFANONLITELT
* CR:		11/08/98 AM.
* SUBJ:	Create an element semantic object for RFA.
* NOTE:	Placeholder, for now.
* RET:	True if ok, else false.
* RULE:	_ELEMENT <- _NONLIT _PAIRS [optional] @@
* FORM:	postRFAnonlitelt("1", "2") -- if pairs.
*			postRFAnonlitelt("1")		-- if no pairs.
********************************************/

bool PostRFA::postRFAnonlitelt(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nname, *npairs;
nname = npairs = 0;

if (!args_1or2(_T("nonlitelt"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nname, npairs))
	return false;

////////////////////////
// BUILD AND ATTACH PAIRS
////////////////////////

// Get semantics from the kiddies.
RFASem *semname, *sempairs;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nonlit elt action: No semantic object for name.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (npairs)
	{
	*gout << _T("npairs=") << *npairs << endl;
	*gout << _T("data=") << *(npairs->getData()) << endl;
	sempairs = (RFASem *) npairs->getData()->getSem();
	}
else
	sempairs = 0;			// 11/11/98 AM. Bug fix.

// Note: should more properly build Ielement here, but this is ok.
Ielt *elt;						// Internal data structure to build.
// Just do Ielement semantics here.
// Don't get Ielement's attrs here. Defer till you decide if this
// is an ELT or a SUGG.
// Get name, pairs.
Dlist<Ipair> *pairs;
_TCHAR *name;

if (sempairs && !(pairs = sempairs->getPairs()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Sempairs problem.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
else
	pairs = 0;
if (!(name = semname->getName()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Semname problem.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Delete the old semantic objects.		// 11/17/98 AM.
// Clear semantic object data so it won't be deleted.
delete semname;								// 11/17/98 AM.
nname->getData()->setSem(0);
if (sempairs)									// 11/17/98 AM.
	{
	sempairs->setPairs(0);
	delete sempairs;
	npairs->getData()->setSem(0);
	}

// Get pass line number.													// 08/08/01 AM.
Pn *pn = 0;																		// 08/08/01 AM.
if (npairs)																		// 08/08/01 AM.
	pn = npairs->getData();													// 08/08/01 AM.
else																				// 08/08/01 AM.
	pn = nname->getData();													// 08/08/01 AM.

elt = new Ielt(name, pairs,
	pn->getLine());				// Get line num in pass.			// 08/08/01 AM.
assert(elt);
elt->fillDefaults();

RFASem *rfasem;
rfasem = new RFASem(elt);
nlppp->sem_ = rfasem;

return true;
}

/********************************************
* FN:		POSTRFAELEMENT
* CR:		11/06/98 AM.
* SUBJ:	Create an element semantic object for RFA.
* NOTE:	Placeholder, for now.
* RET:	True if ok, else false.
* RULE:	_ELEMENT <- _NONLIT _PAIRS [optional] @@
*			_ELEMENT <- _LIT    _PAIRS [optional] @@
* FORM:	postRFAelement("1", "2")
********************************************/

bool PostRFA::postRFAelement(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA element action: Two args required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA element action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}


Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA element action: Two args required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA element action: More than two args given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA element action: Arg1 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA element action: Arg2 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// num1 is collect for element name (_nonlit or _lit guy).
// num2 is distance to pairs, if any.
Node<Pn> *colls;
Node<Pn> *coll1, *coll2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;
while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		break;
	}
coll2 = colls;			// Can be null.


// Get needed stuff from each collect.
Node<Pn> *nname, *npairs;
nname = coll1->Down();
if (coll2)
	npairs = coll2->Down();
else
	npairs = 0;


////////////////////////
// BUILD AND ATTACH PAIRS
////////////////////////


// Get semantics from the kiddies.
RFASem *semname, *sempairs;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	return false;
if (npairs)
	sempairs = (RFASem *) npairs->getData()->getSem();

// Note: should more properly build Ielement here, but this is ok.
Ielt *elt;						// Internal data structure to build.
// Just do Ielement semantics here.
// Don't get Ielement's attrs here. Defer till you decide if this
// is an ELT or a SUGG.
// Get name, pairs.
Dlist<Ipair> *pairs;
_TCHAR *name;
pairs = 0;

if (sempairs)
	{
	if (!(pairs = sempairs->getPairs()))
		{
		*gout << _T("[RFA pairs action: No pairs.]") << endl;
		}
	}
if (!(name = semname->getName()))
	return false;

// Delete the old semantic objects.		// 11/17/98 AM.
// Clear semantic object data so it won't be deleted.
delete semname;								// 11/17/98 AM.
nname->getData()->setSem(0);
if (sempairs)									// 11/17/98 AM.
	{
	sempairs->setPairs(0);
	delete sempairs;
	npairs->getData()->setSem(0);
	}

// Get pass line number.													// 08/08/01 AM.
Pn *pn = 0;																		// 08/08/01 AM.
if (npairs)																		// 08/08/01 AM.
	pn = npairs->getData();													// 08/08/01 AM.
else																				// 08/08/01 AM.
	pn = nname->getData();													// 08/08/01 AM.

elt = new Ielt(name, pairs,
	pn->getLine());				// Get line num in pass.			// 08/08/01 AM.
assert(elt);
elt->fillDefaults();					// 11/11/98 AM.

RFASem *rfasem;
rfasem = new RFASem(elt);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAELT
* CR:		11/10/98 AM.
* SUBJ:	Create a phrase-element semantic object for RFA.
* NOTE:	Placeholder, for now.
* RET:	True if ok, else false.
* RULE:	_ELT <- _ELEMENT @@
* FORM:	postRFAelt("1") -- arg is for the element.
********************************************/

bool PostRFA::postRFAelt(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA elt action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA elt action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA elt action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA elt action: Arg must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1)
	return false;

// num1 is collect for element.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelt;
nelt = coll1->Down();

if (Verbose())
	*gout << _T("   [Executing RFA elt action.]") << endl;

if (Debug())
	{
	*gout << _T("element=") << *nelt;
//	Pn::prettyNodes(nelt);
	*gout << endl;
	}

////////////////////////
// BUILD ELEMENT LIST
////////////////////////

// Need to traverse the pairs, if any, and fill the Ielt and Ielement
// structures appropriately.
Pn *pn;
pn = nelt->getData();
assert(pn);
RFASem *semelt;
semelt = (RFASem *) pn->getSem();
assert(semelt);
Ielt *elt;
elt = semelt->getRulelt();

// Delete the old semantic objects.			// 11/17/98 AM.
semelt->setRulelt(0);
delete semelt;
pn->setSem(0);

RFASem *rfasem;
rfasem = new RFASem(elt);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFARULELTS
* CR:		11/09/98 AM.
* SUBJ:	Create an element-list semantic object for RFA.
* NOTE:	Placeholder, for now.
* RET:	True if ok, else false.
* RULE:	_PHRASE <- _ELEMENT [+] @@
* FORM:	postRFArulelts("1") -- arg is for the elts.
********************************************/

bool PostRFA::postRFArulelts(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulelts action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulelts action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulelts action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA rulelts action: Arg1 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelts, *nend;
nelts = coll1->Down();
nend  = coll1->eDown();

if (!nelts || !nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulelts action: Unhandled null case.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

if (Verbose())
	*gout << _T("   [Executing RFA rulelts action.]") << endl;

if (Debug())
	{
	*gout << _T("elements=");
	Pn::prettyNodes(nelts);
	*gout << endl;
	}

////////////////////////
// BUILD ELEMENT LIST
////////////////////////
// Traverse collect elements, gathering a list.
Dlist<Ielt> *elts;
elts = new Dlist<Ielt>();			// Empty list.
Node<Pn> *bound;
if (nend && nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semelt;
Pn *pn;
Ielt *tmp;
for (; nelts != bound; nelts = nelts->Right())
	{
	// Get semantics of element, copy, and glom onto list.
	pn = nelts->getData();
	assert(pn);
	semelt = (RFASem *) pn->getSem();
	assert(semelt);
	tmp = semelt->getRulelt();
	assert(tmp);

	semelt->setRulelt(0);				// Wipe out the original ptr.
	delete semelt;							// 11/17/98 AM.
	pn->setSem(0);							// 11/17/98 AM.

	// 07/02/99 AM. No longer copies.
	elts->rpush(tmp);					// Create new elt.
	// Zero out the elt.
	//tmp->setPairs(0);
	//tmp->setAttrs(0);
	//tmp->setFails(0);
	//tmp->setMatches(0);
	//tmp->setPasses(0);
	//delete tmp;						// Delete the copied elt.	// 11/17/98 AM.

	}

RFASem *rfasem;
rfasem = new RFASem(elts);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFASUGG
* CR:		11/09/98 AM.
* SUBJ:	Create a suggested-element semantic object for RFA.
* NOTE:	Placeholder, for now.
* RET:	True if ok, else false.
* RULE:	_SUGG <- _ELEMENT @@
* FORM:	postRFAsugg("1") -- arg is for the elt.
********************************************/

bool PostRFA::postRFAsugg(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA sugg action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA sugg action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA sugg action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA sugg action: Arg1 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nelt;
nelt = coll1->Down();

if (Verbose())
	*gout << _T("   [Executing RFA sugg action.]") << endl;

if (Debug())
	{
	*gout << _T("element=");
	Pn::prettyNodes(nelt);
	*gout << endl;
	}

////////////////////////
// BUILD SUGG
////////////////////////

RFASem *semelt;
Pn *pn;
pn = nelt->getData();
assert(pn);
semelt = (RFASem *) pn->getSem();
assert(semelt);
Ielt *elt;
elt = semelt->getRulelt();
assert(elt);
Isugg *sugg;
// Just grab the elt's stuff rather than copying.
sugg = new Isugg(elt->getName(), elt->getPairs());
elt->setName(0);
elt->setPairs(0);			// Clear the old stuff.
delete semelt;				// 11/17/98 AM.
pn->setSem(0);				// 11/17/98 AM.

RFASem *rfasem;
rfasem = new RFASem(sugg);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFARULE
* CR:		11/05/98 AM.
* SUBJ:	Create a rule semantic object for RFA rule that matches a rule!
* NOTE:	Is that clear, or what?  Now have to decide on ordering of actions.
*			Does the redSingle go first, or postRFArule.  And how does the
*			semantics end up in the suggested _RULE node.  A convention could
*			keep semantic ptr around for the reduce to pick up.
*			If redSingle happens first and the postActions function keeps the
*			collect around, then can do it that way. (Either keep the sugg
*			around or the sem...)
*			Could name this semRFArule() to denote a semantic action.
* ASS:	Because of recursive rule matching, the stuff in the wildcard is
*			all set up as a phrase of elts!
* RET:	True if ok, else false.
* RULE:	_RULE <- _NONLIT _ARROW _xWILD _ENDRULE @@
* FORM:	postRFArule("1", "3") -- first arg is sugg, 2nd is phrase.
********************************************/

bool PostRFA::postRFArule(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rule action: Two args required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rule action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rule action: Two args required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rule action: More than two args given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA rule action: Arg1 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA rule action: Arg2 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// num1 is collect for suggested elt.
// num2 is distance to rule's phrase (under the _xWILD wildcard elt).
Node<Pn> *colls;
Node<Pn> *coll1, *coll2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;
while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll2 = colls;


// Get needed stuff from each collect.
Node<Pn> *nsugg, *nphrase;
nsugg = coll1->Down();
nphrase = coll2->Down();

if (Verbose())
	*gout << _T("   [Executing RFA rule action.]") << endl;

if (Debug())
	{
	*gout << _T("sugg=") << *nsugg << endl;
	*gout << _T("phrase=");
	Pn::prettyNodes(nphrase);
	*gout << endl;
	}

////////////////////////
// BUILD RULE COMPONENTS
////////////////////////
if (!nphrase)																	// 02/14/02 AM.
	{
	if (nlppp->node_)															// 02/14/02 AM.
		{
		nlppp->parse_->inputline_											// 08/24/02 AM.
				= nlppp->node_->getData()->getLine();					// 02/14/02 AM.
		}
	_t_strstream gerrStr;						// 02/14/02 AM.
	gerrStr << _T("[Error: Could not read rule.]") << ends;			// 02/14/02 AM.
	return nlppp->parse_->errOut(&gerrStr,false,true);			// 02/14/02 AM.
	}

Pn *pn;

pn = nphrase->getData();
assert(pn);
RFASem *semphrase;
semphrase = (RFASem *) pn->getSem();
if (!semphrase)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rule action: No phrase.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (semphrase->getType() != RSPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Wrong type for semantic phrase object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	gerrStr << _T("[type=") << ((int) semphrase->getType()) << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
Dlist<Ielt> *phrase;
phrase = semphrase->getPhrase();
semphrase->setPhrase(0);			// Clear the old.
delete semphrase;						// 11/17/98 AM.
pn->setSem(0);							// 11/17/98 AM.


pn = nsugg->getData();
assert(pn);
RFASem *semsugg;
semsugg = (RFASem *) pn->getSem();
assert(semsugg);
Isugg *sugg;
sugg = semsugg->getSugg();
semsugg->setSugg(0);				// Clear the old.
delete semsugg;					// 11/17/98 AM.
pn->setSem(0);					// 11/17/98 AM.

// 11/17/98 AM. COMMENT OUT THIS CRAP.  CAUSED A MEMORY LEAK TOO!
//Dlist<Ipair> *pairs;
//pairs = 0;			// No pairs for now.
//char *name;
//name = pn->pnStr();
//sugg = new Isugg(name, pairs);

Dlist<Ipre> *pres;
pres = 0;

Dlist<Iaction> *post;
post = 0;

Delt<Ielt> *trigger;
trigger = 0;

////////////////////////
// BUILD AND ATTACH RULE
////////////////////////
Irule *rule;
rule = new Irule(phrase, sugg, pres, 0, post, trigger,
				pn->getLine());												// 05/26/01 AM.

if (Debug())
	{
	*gout << _T("RFA RULE ACTION: RULE = ");
	*gout << *rule;
	*gout << endl;
	}

RFASem *rfasem;
rfasem = new RFASem(rule);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFARULES
* CR:		11/11/98 AM.
* SUBJ:	Create an rule-list semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_RULES <- _RULE [+] @@
* FORM:	postRFArules("1") -- arg is for _rule.
********************************************/

bool PostRFA::postRFArules(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rules action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rules action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rules action: More than one arg given.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA rulesaction action: Arg must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

		return false;
	}

// Check self-consistency.
if (!num1)
	return false;


// num1 is collect for elements.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nrule, *nend;
nrule = coll1->Down();
nend  = coll1->eDown();

if (!nrule || !nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rules action: Unhandled null case.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

if (Verbose())
	*gout << _T("   [Executing RFA rules action.]") << endl;

if (Debug())
	{
	*gout << _T("rules=");
	Pn::prettyNodes(nrule);
	*gout << endl;
	}

////////////////////////
// BUILD RULE LIST
////////////////////////
// Traverse collect elements, gathering a list.
Dlist<Irule> *rules;
rules = new Dlist<Irule>();			// Empty list.
Node<Pn> *bound;
if (nend && nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semrule;
Pn *pn;
Irule *rule, *tmp;
for (; nrule != bound; nrule = nrule->Right())
	{
	// Get semantics of element, copy, and glom onto list.
	pn = nrule->getData();
	assert(pn);
	semrule = (RFASem *) pn->getSem();
	assert(semrule);
	rule = semrule->getRule();
	assert(rule);
	// Use the rule's semantic object.
	tmp = rule;

	// DELETE THE RULE'S SEM OBJECT HERE.
	semrule->setRule(0);				// Clear the rule so it won't be deleted.
	delete semrule;					// 11/17/98 AM.
	pn->setSem(0);						// 11/17/98 AM.

	// 07/02/99 AM. No longer copies.
	rules->rpush(tmp);

	// 11/17/98 AM. Wipe out the rule that was copied.
	//tmp->setPhrase(0);
	//tmp->setPost(0);
	//tmp->setPre(0);
	//tmp->setSugg(0);
	//delete tmp;
	}

if (Debug())
	*gout << _T("RFA RULES ACTION: RULES = ") << *rules << endl;

RFASem *rfasem;
rfasem = new RFASem(rules);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFALITTOACTION
* CR:		11/23/98 AM.
* SUBJ:	Convert a literal to an action.
* RET:	True if ok, else false.
* RULE:	_ACTION <- _LIT @@
* FORM:	postRFAlittoaction("1")
********************************************/

bool PostRFA::postRFAlittoaction(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nlit, *nend;
nlit = nend = 0;

if (!args_1(_T("actions"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nlit, nend))
	return false;

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semlit;
if (!(semlit = (RFASem *) nlit->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA littoaction action: No semantic object for lit.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iaction *action;				// Sem to build.
_TCHAR *name;

if (!(name = semlit->getName()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Sem lit problem.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
//semlit->setName(0);			// Not needed.  Keep for consistency.
delete semlit;
nlit->getData()->setSem(0);

action = new Iaction(name, 0);		// Build action with empty args.
//delete X;									// Keep for consistency.	
RFASem *rfasem;
rfasem = new RFASem(action);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAACTIONS
* CR:		11/23/98 AM.
* SUBJ:	Create an actions semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_ACTIONS <- _soPOST _xWILD [match=(_LIST _LIT)] _eoPOST [opt] @@
* FORM:	postRFAactions("2") -- arg is for the wildcard elt.
* NOTE:	11/13/99 AM. Handling NLP++ semantic types.
********************************************/

bool PostRFA::postRFAactions(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("actions"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

if (!nstart)				// 11/27/98 AM. No actions is ok.
	return true;

////////////////////////
// BUILD ACTIONS LIST
////////////////////////
// Traverse collect elements, gathering a list of actions.
Dlist<Iaction> *iactions;
iactions = new Dlist<Iaction>();			// Empty list.
Node<Pn> *bound;
if (nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semaction;
Pn *pn;
Iaction *tmp;
for (; nstart != bound; nstart = nstart->Right())
	{
	// Get semantics of action, copy, and glom onto list.
	pn = nstart->getData();
	assert(pn);
	if (!(semaction = (RFASem *) pn->getSem()))			// 11/08/99 AM.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA actions action: Null action in list.]")
				  << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		Dlist<Iaction>::DeleteDlistAndData(iactions);
		return false;
		}
	switch (semaction->getType())								// 11/08/99 AM.
		{
		case RSACTION:
			tmp = semaction->getAction();
			assert(tmp);
			semaction->setAction(0);				// Wipe out the original ptr.
			delete semaction;
			break;
		case RSSTMTS:												// NLP++	// 11/08/99 AM.
		case RSSTMT:												// FIX	// 11/17/99 AM.
		case RSEXPR:												// FIX.	// 11/17/99 AM.
		case RSVAR:													// FIX.	// 11/30/99 AM.
		case RSPNODE:															// 11/15/01 AM.
			// Create an action. Throw stmts object into it.
			// ie, DON'T DELETE THE SEM OBJECT.
			tmp = new Iaction();			// Build new action object.
			tmp->setSem(semaction);		// Add stmts to new action object.
			break;
		case RSPRE:
			{
			_t_strstream gerrStr;
			gerrStr <<
				_T("[RFA actions action: Pre-action not ok in check,post actions.]")
					<< ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			}

			Dlist<Iaction>::DeleteDlistAndData(iactions);
			return false;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA actions action: Non-action in list.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			}

			Dlist<Iaction>::DeleteDlistAndData(iactions);
			return false;
			break;
		}
	iactions->rpush(tmp);	// Push action onto list of actions.
	pn->setSem(0);				// Clear node of ptr to old sem object.
	}

RFASem *rfasem;
rfasem = new RFASem(iactions);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAPRES
* CR:		11/29/98 AM.
* SUBJ:	Create a pre-actions semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_ACTIONS <- _soPRE _xWILD [match=(_ACTION _LIT)] _eoPRE [opt] @@
* FORM:	postRFAactions("2") -- arg is for the wildcard elt.
********************************************/

bool PostRFA::postRFApres(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("pres"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

if (!nstart)				// No actions is ok.
	return true;

////////////////////////
// BUILD ACTIONS LIST
////////////////////////
// Traverse collect elements, gathering a list of actions.
Dlist<Ipre> *iactions;
iactions = new Dlist<Ipre>();			// Empty list.
Node<Pn> *bound;
if (nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semaction;
Pn *pn;
Ipre *tmp;
for (; nstart != bound; nstart = nstart->Right())
	{
	// Get semantics of action, copy, and glom onto list.
	pn = nstart->getData();
	assert(pn);
	semaction = (RFASem *) pn->getSem();
	if (!semaction || (semaction->getType() != RSPRE))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA pres action: Non pre action in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		Dlist<Ipre>::DeleteDlistAndData(iactions);
		return false;
		}
	tmp = semaction->getPre();
	assert(tmp);

	semaction->setPre(0);				// Wipe out the original ptr.
	delete semaction;
	pn->setSem(0);

	// 07/02/99 AM. No longer copies.
	iactions->rpush(tmp);					// Create new elt.
	//Dlist<Iarg>::DeleteDlistAndData(tmp->getArgs());
	//tmp->setArgs(0);							// Zero it out.
	//tmp->setName(0);							// Zero it out.
	//delete tmp;									// Delete the copied elt.
	}

RFASem *rfasem;
rfasem = new RFASem(iactions);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFACODE
* CR:		12/07/98 AM.
* SUBJ:	Create a code-region object for RFA.
* RET:	True if ok, else false.
* RULE:	_CODE <- _soCODE _xWILD [match=(_LIST _LIT)] _eoCODE [opt] @@
* FORM:	postRFAcode("2") -- arg is for the wildcard elt.
********************************************/

bool PostRFA::postRFAcode(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("code"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

if (!nstart)				// 11/27/98 AM. No actions is ok.
	return true;

////////////////////////
// BUILD ACTIONS LIST
////////////////////////
// Traverse collect elements, gathering a list of actions.
Dlist<Iaction> *iactions;
iactions = new Dlist<Iaction>();			// Empty list.
Node<Pn> *bound;
if (nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semaction;
Pn *pn;
Iaction *tmp;
for (; nstart != bound; nstart = nstart->Right())
	{
	// Get semantics of action, copy, and glom onto list.
	pn = nstart->getData();
	assert(pn);
	semaction = (RFASem *) pn->getSem();
	if (semaction && (semaction->getType() == RSPRE))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA code action: Pre-action not ok here.]")
				  << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		Dlist<Iaction>::DeleteDlistAndData(iactions);
		return false;
		}
	if (!semaction || (semaction->getType() != RSACTION))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA code action: Non-action in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		//delete iactions;
		Dlist<Iaction>::DeleteDlistAndData(iactions);
		return false;
		}
	tmp = semaction->getAction();
	assert(tmp);

	semaction->setAction(0);				// Wipe out the original ptr.
	delete semaction;
	pn->setSem(0);

	// 07/02/99 AM. No longer copies.
	iactions->rpush(tmp);					// Create new elt.
	//tmp->setArgs(0);							// Zero it out.
	//tmp->setName(0);							// Zero it out.
	//delete tmp;									// Delete the copied elt.
	}

RFASem *rfasem;
rfasem = new RFASem(iactions);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFANODES
* CR:		11/30/98 AM.
* SUBJ:	Create an arg-list semantic object for RFA nodes.
* RET:	True if ok, else false.
* RULE:	_NODES <- _soNODES _NONLIT [*] _eoNODES [opt] @@
* FORM:	postRFAnodes("2", "TYP") -- arg1 is for the args.
*			arg2 is for the type = "nodes", "multi", or "path". (10/19/99 AM)
*			10/25/99 AM. PUT PREMULTI HERE WHEN READY.
********************************************/

bool PostRFA::postRFAnodes(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nodes action: One arg required.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nodes action: Semantic object already built.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))			// 03/25/99 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nodes action: Missing nodes/multi arg.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
Iarg *arg2;
arg2 = args->getData();

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA nodes action: Arg1 must be numeric.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	}

if (arg2->getType() != IASTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nodes action: Arg must be string.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Check self-consistency.
if (!num1)
	return false;

// Get type of nodes list.  // 03/25/99 AM.
_TCHAR *ltype;
if (!(ltype = arg2->getStr()) || !*ltype)
	return false;
if (_tcscmp(ltype, _T("nodes"))
	 && _tcscmp(ltype, _T("multi"))
	 && _tcscmp(ltype, _T("path"))				// 10/19/99 AM.
	 )
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rfanodes: 2nd arg must be 'nodes', 'multi', or 'path'.]")
			<< ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// num1 is collect for args.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
Node<Pn> *nnls, *nend;
nnls = coll1->Down();
nend  = coll1->eDown();
if (!nnls || !nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA nodes action: Unhandled null case.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

if (Verbose())
	*gout << _T("   [Executing RFA nodes action.]") << endl;

if (Debug())
	{
	*gout << _T("args=");
	Pn::prettyNodes(nnls);
	*gout << endl;
	}

////////////////////////
// BUILD LIST
////////////////////////
// Traverse collect elements, gathering a list.
Dlist<Iarg> *iargs;
iargs = new Dlist<Iarg>();			// Empty list.
Node<Pn> *bound;
if (nend && nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semnl;
Pn *pn;
Iarg *tmp;
for (; nnls != bound; nnls = nnls->Right())
	{
	// Get semantics of nonlit, copy, and glom onto list.
	pn = nnls->getData();
	assert(pn);
	semnl = (RFASem *) pn->getSem();
	if (!semnl || (semnl->getType() != RSNAME))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA nodes action: Non-name in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		//delete iargs;					// 11/21/98 AM.
		Dlist<Iarg>::DeleteDlistAndData(iargs);	// 11/21/98 AM.
		return false;
		}
	tmp = new Iarg(semnl->getName());

	//semnl->setName(0);				// Wipe out the original ptr.
	delete semnl;
	pn->setSem(0);

	// 07/02/99 AM. No longer copies.
	iargs->rpush(tmp);					// Create new elt.
	//tmp->setX(0);						// Wipe it out.
	//delete tmp;						// Delete the copied elt.	// 11/17/98 AM.

	}

RFASem *rfasem;

// Build appropriately typed semantic object.	// 03/25/99 AM.
//rfasem = new RFASem(iargs);							// 03/25/99 AM.
if (!_tcscmp(ltype, _T("nodes")))							// 03/25/99 AM.
	rfasem = new RFASem(iargs, RSNODES);			// 03/25/99 AM.
else if (!_tcscmp(ltype, _T("multi")))					// 03/25/99 AM.
	rfasem = new RFASem(iargs, RSMULTI);			// 03/25/99 AM.
else if (!_tcscmp(ltype, _T("path")))						// 10/19/99 AM.
	rfasem = new RFASem(iargs, RSPATH);				// 10/19/99 AM.
else
	return false;

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFASELECT
* CR:		11/30/98 AM.
* SUBJ:	Build a SELECT region.
* RET:	True if ok, else false.
* RULE:	_SELECT <- _soSELECT [opt] _NODES _eoSELECT [opt] @@
* FORM:	postRFAselect("2")
* NOTE:	For now, just copying up the list of node names.
*			In the future, may want a full SELECT semantic object, with
*			various ways of specifying the parse tree nodes to traverse.
********************************************/

bool PostRFA::postRFAselect(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nnodes, *nend;
nnodes = nend = 0;

if (!args_1(_T("select"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nnodes, nend))
	return false;

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semnodes;
if (!(semnodes = (RFASem *) nnodes->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA select action: No semantic object for nodes.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// FOR NOW, JUST MOVE UP THE SEMANTIC OBJECT FROM NODES.
nnodes->getData()->setSem(0);

nlppp->sem_ = semnodes;

return true;
}


/********************************************
* FN:		POSTRFAREGION
* CR:		11/24/98 AM.
* SUBJ:	Recognize a pre-post-rules region in rules file.
* RET:	True if ok, else false.
* RULE:	_REGION <- _PRES [opt] _POST [opt] _RULES @@
* FORM:	postRFArecurse("1","2","3")
* NOTE:	Handle optional rules also!
*			11/27/98 AM.	Added check action handling.
********************************************/

bool PostRFA::postRFAregion(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *npres, *nchecks, *nposts, *nrules;
npres = nchecks = nposts = nrules = 0;

if (!args_0to4(_T("region"), args, nlppp->collect_, nlppp->sem_,
										/*DU*/ npres, nchecks, nposts, nrules))
	return false;

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *sempres, *semchecks, *semposts, *semrules;
sempres = semchecks = semposts = semrules = 0;

if (npres)
	sempres = (RFASem *) npres->getData()->getSem();
if (nchecks)
	semchecks = (RFASem *) nchecks->getData()->getSem();
if (nposts)
	semposts = (RFASem *) nposts->getData()->getSem();
if (nrules)
	semrules = (RFASem *) nrules->getData()->getSem();
//if (!sempres || !semposts || !semrules)
//	*gerr << "[RFA region action: Missing semantic object. Ignored.]" << endl;

if (sempres && sempres->getType() != RSPRES)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA region action: Bad pre semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semchecks && semchecks->getType() != RSACTIONS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA region action: Bad check semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semposts && semposts->getType() != RSACTIONS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA region action: Bad post semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semrules && semrules->getType() != RSRULES)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA region action: Bad rules semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Get stuff from the semantic objects, then remove them.
Dlist<Ipre> *pres;
Dlist<Iaction> *checks;
Dlist<Iaction> *posts;
Dlist<Irule> *rules;
pres = 0;
checks = 0;
posts = 0;
rules = 0;

if (sempres)
	{
	pres = sempres->getPres();
	sempres->setPres(0);			// Zero them out.
	delete sempres;
	npres->getData()->setSem(0);
	}
if (semchecks)
	{
	checks = semchecks->getActions();
	semchecks->setActions(0);			// Zero them out.
	delete semchecks;
	nchecks->getData()->setSem(0);
	}
if (semposts)
	{
	posts = semposts->getActions();
	semposts->setActions(0);		// Zero them out.
	delete semposts;
	nposts->getData()->setSem(0);
	}
if (semrules)
	{
	rules = semrules->getRules();
	semrules->setRules(0);			// Zero them out.
	delete semrules;
	nrules->getData()->setSem(0);
	}

Iregion *region;				// Sem to build.
region = new Iregion(pres, checks, posts, rules);
//delete X;									// Keep for consistency.	
RFASem *rfasem;
rfasem = new RFASem(region);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFAREGIONS
* CR:		11/24/98 AM.
* SUBJ:	Create a regions semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_REGIONS <- _REGION [+] @@
* FORM:	postRFAregions("1")
********************************************/

bool PostRFA::postRFAregions(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("actions"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

////////////////////////
// BUILD REGIONS LIST
////////////////////////
// Traverse collect elements, gathering a list of regions.
Slist<Iregion> *iregions;
iregions = new Slist<Iregion>();			// Empty list.
Node<Pn> *bound;
if (nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semregion;
Pn *pn;
Iregion *tmp;
for (; nstart != bound; nstart = nstart->Right())
	{
	// Get semantics of region, copy, and glom onto list.
	pn = nstart->getData();
	assert(pn);
	semregion = (RFASem *) pn->getSem();
	if (!semregion || (semregion->getType() != RSREGION))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA regions action: Non-region in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		delete (Slist<Iregion> *) iregions;
		//Slist<Iregion>::DeleteSlistAndData(iregions);
		return false;
		}
	tmp = semregion->getRegion();
	assert(tmp);

	semregion->setRegion(0);				// Wipe out the original ptr.
	delete semregion;
	pn->setSem(0);

	// Note: delete not needed for Slist,Selt.
	iregions->rpush(tmp);					// Create new elt.
	//tmp->setArgs(0);							// Zero it out.
	//tmp->setName(0);							// Zero it out.
	//delete tmp;									// Delete the copied elt.
	}

RFASem *rfasem;
rfasem = new RFASem(iregions);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFARECURSE
* CR:		11/25/98 AM.
* SUBJ:	Recognize a recursive mini-pass in rules file.
* RET:	True if ok, else false.
* RULE:	_RECURSE <- _soRECURSE _LIT
*							_REGIONS [opt]
*							_eoRECURSE _LIT [opt]
* FORM:	postRFArecurse("2","3","5")
********************************************/

bool PostRFA::postRFArecurse(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nname, *nregs, *ncheck;
nname = nregs = ncheck = 0;

if (!args_0to3(_T("recurse"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nname,nregs,ncheck))
	return false;

if (nname <= 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: Argument for region name is required.]")
		  << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semname, *semregs, *semcheck;
semname = semregs = semcheck = 0;


//if (nname)
	semname = (RFASem *) nname->getData()->getSem();
if (nregs)
	semregs = (RFASem *) nregs->getData()->getSem();
if (ncheck)
	semcheck = (RFASem *) ncheck->getData()->getSem();

if (!semname)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: Missing semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

if (/*semname &&*/ semname->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: Bad name semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semregs && semregs->getType() != RSREGIONS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: Bad regions semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (semcheck && semcheck->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: Bad check semantic object.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Get stuff from the semantic objects, then remove them.
_TCHAR *name;
Slist<Iregion> *regs;
_TCHAR *check;
name = 0;
regs = 0;
check = 0;

//if (semname)
	{
	name = semname->getName();
	assert(name);
	semname->setName(0);			// Zero them out.
	delete semname;
	nname->getData()->setSem(0);
	}
if (semregs)
	{
	regs = semregs->getRegions();
	assert(regs);
	semregs->setRegions(0);		// Zero them out.
	delete semregs;
	nregs->getData()->setSem(0);
	}
if (semcheck)
	{
	check = semcheck->getName();
	assert(check);
	semcheck->setName(0);			// Zero them out.
	delete semcheck;
	ncheck->getData()->setSem(0);
	}

// Some semantic checking.
if (check && strcmp_i(name, check))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: Nonmatching names in region=(")
		  << name << _T(",") << check << _T(").  Please fix.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (!regs)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA recurse action: No rules found in recurse region.]")
		  << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	}

Irecurse *recurse;				// Sem to build.
recurse = new Irecurse(name, regs);
//delete X;									// Keep for consistency.	
RFASem *rfasem;
rfasem = new RFASem(recurse);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFARECURSES
* CR:		11/25/98 AM.
* SUBJ:	Create a recurses semantic object for RFA.
* RET:	True if ok, else false.
* RULE:	_RECURSES <- _RECURSE [+] @@
* FORM:	rfarecurses(1).
********************************************/

bool PostRFA::postRFArecurses(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("recurses"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

////////////////////////
// BUILD RECURSES LIST
////////////////////////
// Traverse collect elements, gathering a list of recurses.
Slist<Irecurse> *irecurses;
irecurses = new Slist<Irecurse>();			// Empty list.
Node<Pn> *bound;
if (nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semrecurse;
Pn *pn;
Irecurse *tmp;
for (; nstart != bound; nstart = nstart->Right())
	{
	// Get semantics of recurse, copy, and glom onto list.
	pn = nstart->getData();
	assert(pn);
	semrecurse = (RFASem *) pn->getSem();
	if (!semrecurse || (semrecurse->getType() != RSRECURSE))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA recurses action: Non-recurse in list.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		delete (Slist<Irecurse> *) irecurses;
		//Slist<Irecurse>::DeleteSlistAndData(irecurses);
		return false;
		}
	tmp = semrecurse->getRecurse();
	assert(tmp);

	semrecurse->setRecurse(0);				// Wipe out the original ptr.
	delete semrecurse;
	pn->setSem(0);

	// Note: delete not needed for Slist,Selt.
	irecurses->rpush(tmp);					// Create new elt.
	//tmp->setArgs(0);							// Zero it out.
	//tmp->setName(0);							// Zero it out.
	//delete tmp;									// Delete the copied elt.
	}

RFASem *rfasem;
rfasem = new RFASem(irecurses);
nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFARULESFILE
* CR:		11/24/98 AM.
* SUBJ:	Recognize a complete rules file.
* RET:	True if ok, else false.
* RULE:	_RULESFILE <- _DECL _CODE _SELECT [opt] _RECURSES [opt] _REGIONS @@
* FORM:	rfarulesfile(1,2,3,4,5)
* NOTE:	11/30/98 AM. Adding optional SELECT elt to rule.
*			12/19/01 AM. Adding @DECL region for function definitions.
********************************************/

bool PostRFA::postRFArulesfile(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *ndecl=0, *ncode=0, *nselect=0, *nrecs=0, *nregions=0;// 12/19/01 AM.

if (!args_0to5(_T("rulesfile"), args, nlppp->collect_, nlppp->sem_,
										/*DU*/ ndecl,ncode, nselect, nrecs, nregions))
	return false;

// ALLOWING EMPTY RULES IF THERE IS CODE.								// 08/31/00 AM.
if (!nregions
	 && !ncode																	// 08/31/00 AM.
	 && !ndecl )																// 12/19/01 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulesfile action: Requiring rules or code in file.]")
			  << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semdecl, *semcode, *semselect, *semrecs, *semregions;
semdecl = semcode = semselect = semrecs = semregions = 0;

if (ndecl)
	semdecl = (RFASem *) ndecl->getData()->getSem();
if (ncode)
	semcode = (RFASem *) ncode->getData()->getSem();
if (nselect)
	semselect = (RFASem *) nselect->getData()->getSem();
if (nrecs)
	semrecs = (RFASem *) nrecs->getData()->getSem();
if (nregions)
	semregions = (RFASem *) nregions->getData()->getSem();

if (semdecl && semdecl->getType() != RSARGS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulesfile action: Bad @decl semantic object.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (semcode && semcode->getType() != RSACTIONS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulesfile action: Bad code semantic object.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
//if (semselect && semselect->getType() != RSARGS)		// 03/25/99 AM.
if (semselect
    && semselect->getType() != RSNODES						// 03/25/99 AM.
    && semselect->getType() != RSMULTI						// 03/25/99 AM.
    && semselect->getType() != RSPATH						// 10/19/99 AM.
	 )
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulesfile action: Bad select semantic object.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (semrecs && semrecs->getType() != RSRECURSES)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulesfile action: Bad recurses semantic object.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (semregions && semregions->getType() != RSREGIONS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA rulesfile action: Bad regions semantic object.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

// Get stuff from the semantic objects, then remove them.
Dlist<Iarg> *decls=0;														// 12/19/01 AM.
Dlist<Iaction> *code=0;														// 12/07/98 AM.
Dlist<Iarg> *sels=0;															// 11/30/98 AM.
Slist<Irecurse> *recs=0;
Slist<Iregion> *regions=0;

if (semdecl)																	// 12/19/01 AM.
	{
	decls = semdecl->getArgs();											// 12/19/01 AM.
	semdecl->setArgs(0);			// Zero them out.						// 12/19/01 AM.
	delete semdecl;															// 12/19/01 AM.
	ndecl->getData()->setSem(0);											// 12/19/01 AM.
	}

if (semcode)
	{
	code = semcode->getActions();
	semcode->setActions(0);			// Zero them out.
	delete semcode;
	ncode->getData()->setSem(0);
	}

bool fmulti = false;					// Flag if MULTI select.
bool fpath = false;						// Flag if PATH select.
if (semselect)
	{
	enum RFASemtype typ = semselect->getType();	// 10/19/99 AM.
	if (typ == RSMULTI)									// 03/25/99 AM.
		fmulti = true;										// 03/25/99 AM.
	else if (typ == RSPATH)								// 10/19/99 AM.
		fpath = true;										// 10/19/99 AM.
	sels = semselect->getArgs();
	semselect->setArgs(0);			// Zero them out.
	delete semselect;
	nselect->getData()->setSem(0);
	semselect = 0;											// 10/19/99 AM.
	}

if (semrecs)
	{
	recs = semrecs->getRecurses();
	semrecs->setRecurses(0);			// Zero them out.
	delete semrecs;
	nrecs->getData()->setSem(0);
	}
if (semregions)
	{
	regions = semregions->getRegions();
	semregions->setRegions(0);		// Zero them out.
	delete semregions;
	nregions->getData()->setSem(0);
	}

Ifile *ifile;					// Sem to build.
ifile = new Ifile(decls, code, sels, recs, regions,				// 12/19/01 AM.
	fmulti, fpath);															// 10/19/99 AM.
//delete X;									// Keep for consistency.

// Register if rulefile is legitimately empty of rules.			// 08/31/00 AM.
long num =0;																	// 08/31/00 AM.
Var::val(_T("RFA rulemark"), nlppp->getParse(), num);					// 08/31/00 AM.
ifile->setEmpty(num ? false : true);									// 08/31/00 AM.

RFASem *rfasem;
rfasem = new RFASem(ifile);
nlppp->sem_ = rfasem;

// Pretty-print the rules file.
// Shutting this down for now.		// 12/25/98 AM.
// Should be put in a more generic place.
/*
*gout << endl
		<< "*********************\n"
		<< " INTERNAL RULES FILE\n"
		<< "*********************"
		<< endl;
*gout << *ifile << endl;
*/
return true;
}

/**************************************************************************
//                 NLP++ SEMANTICS
/**************************************************************************


/********************************************
* FN:		POSTRFAMOVESEM
* CR:		11/09/99 AM.
* SUBJ:	Move semantic object up.
* RET:	True if ok, else false.
* RULE:	X <- A B C ... @@
* FORM:	postRFAmovesem(3)			[node number
* NOTE:	This is a much more general semantic operation than for the
*			RFA semantics.  Should be renamed and moved to a better place.
********************************************/

bool PostRFA::postRFAmovesem(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("movesem"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

if (!nstart)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA movesem action: Requires one argument.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
if (nstart != nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA movesem action: Can't handle multiple nodes.]")
			<< ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}

// Get semantics from the child.
Pn *pnchild = nstart->getData();
RFASem *semchild;
if (!(semchild = (RFASem *) pnchild->getSem()))
	{
	//*gerr << "[RFA movesem action: Node has no semantics to move.]"
	//		<< endl;
	//return false;
	return true;		// Treating this as ok.
	}

// Clear semantic object from child so it won't be deleted.
pnchild->setSem(0);

// Transfer up.
nlppp->sem_ = semchild;
return true;
}


/********************************************
* FN:		POSTRFAMAKESTMT
* CR:		05/11/00 AM.
* SUBJ:	Convert expr, etc. to stmt.
* RET:	True if ok, else false.
* RULE:	@POST
*				makestmt(1)
*				single()
*			@RULES
*			_STMT <- _xWILD [s one match=( _EXPR _NUM _STR )] \; [s] @@
*			_STMTS [base] <- _xWILD [s one match=(_STMT _EXPR _BLOCK)] @@
*
* FORM:	postRFAmakestmt(1) -- 1 - expr,stmt,str, etc.
********************************************/

bool PostRFA::postRFAmakestmt(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	bool semicolon	// If semicolon present.	// 05/29/01 AM.
	)
{
Node<Pn> *nstart=0, *nend=0;
if (!args_1(_T("makestmt"), args, nlppp->collect_, nlppp->sem_, 
																	/*DU*/ nstart, nend))
	return false;
if (!nstart)
	return false;

// Get semantics from the kiddies.
Pn *pn = nstart->getData();
RFASem *sem = 0;
if (!(sem = (RFASem *) pn->getSem()))
	{
	// FIX. Allow empty statements.										// 08/01/00 AM.
	// "Statement" could be a _NUM or _STR.
	// But not handling these useless cases for now.
//	*gerr << "[RFB makestmt: No semantics in expr. Unimplemented.]" << endl;
//	return false;				// 08/01/00 AM.
	}

else																	// FIX.	// 08/01/00 AM.
switch (sem->getType())
	{
	case RSSTMT:
	case RSSTMTS:
		// Transfer up.
		pn->setSem(0);			// Clear out of node.
		nlppp->sem_ = sem;	// Transfer it to suggested concept.
		return true;
	case RSEXPR:
	case RSACTION:
	case RSNAME:
	case RSSTR:
	case RSLONG:
	case RSFLOAT:																// 08/19/01 AM.
	case RSVAR:
	case RSPNODE:																// 11/15/01 AM.
	  if (!semicolon)		// No terminating semicolon.				// 05/29/01 AM.
		{
		nlppp->parse_->inputline_											// 08/24/02 AM.
					= pn->getLine();// Line num in INPUT pass file.	// 05/29/01 AM.
		_t_strstream gerrStr;					// 05/29/01 AM.
		gerrStr << _T("[Warning: Missing ';']") << ends;					// 05/29/01 AM.
		nlppp->parse_->errOut(&gerrStr,true,true);
										// Pass num of INPUT pass file.	// 05/29/01 AM.
		// Convert to expr statement type.
		}
		break;
	default:
		if (nlppp->parse_->getIntern())									// 05/26/01 AM.
			{
			nlppp->parse_->inputline_										// 08/24/02 AM.
						= pn->getLine();	// Pass file line number.	// 05/20/01 AM.
			_stprintf(Errbuf, _T("[Error: Bad statement.]"));				// 05/20/01 AM.
			nlppp->parse_->errOut(false,true); // UNFIXED 
												// Pass file num.				// 05/20/01 AM.
			}
		return false;															// 05/26/01 AM.
	}

pn->setSem(0);		// Clear out of node.

// Build expr statement object.
Iexprstmt *exprstmt = new Iexprstmt(sem,
												pn->getLine());				// 03/12/02 AM.
RFASem *rfasem;
rfasem = new RFASem(exprstmt);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAMAKESTMTS
* CR:		05/29/01 AM.
* SUBJ:	Convert expr, etc. to stmts list.
* RET:	True if ok, else false.
* RULE:	@POST
*				makestmts(1)
*				single()
*			@RULES
*			_STMTS [base] <- _xWILD [s one match=(_STMT _EXPR _BLOCK)] @@
*
* FORM:	postRFAmakestmts(1,false) -- 1 - expr,stmt,str, etc.
*				false == NO SEMICOLON AT END.
********************************************/

bool PostRFA::postRFAmakestmts(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
return postRFAmakestmt(args,nlppp,false);
}

/********************************************
* FN:		POSTRFAADDSTMT
* CR:		11/12/99 AM.
* SUBJ:	Create a BINARY expr semantic object.
* RET:	True if ok, else false.
* RULE:	@POST
*				addstmt(1, 2)
*				single()
*			@RULES
*			_STMTS [base] <- _STMTS _xWILD [s one match=(_STMT _EXPR)] @@
* FORM:	postRFAaddstmt(1,2) -- 1 - stmts,stmt, or expr. 2- stmt,expr.
* NOTE:	01/13/00 AM. FNCALL is now an expression.
*			02/12/00 AM. FIX.  If statement list RFASem object exists, use it.
*			Else, create a new one.
********************************************/

bool PostRFA::postRFAaddstmt(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nstmts = 0, *nstmt = 0;

if (!args_0to2(_T("addstmt"),args,nlppp->collect_,nlppp->sem_, /*DU*/ nstmts,nstmt))
	return false;

// Get semantics from the kiddies.
Pn *pnstmts = nstmts->getData();
Pn *pnstmt = nstmt->getData();
RFASem *semstmts = 0, *semstmt = 0, *semstmt1 = 0;
RFASem *semexpr = 0;
Slist<Istmt> *stmts = 0;
Slist<Istmt> *block = 0;			// 12/15/99 AM.
Istmt *stmt = 0, *stmt1 = 0;
if (!(semstmts = (RFASem *) pnstmts->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA addstmt action: No semantics in stmts.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
else
	{
	switch (semstmts->getType())
		{
		// case RSNUM:
		// case RSSTR:
		// case RSLONG:
		case RSEXPR:
		case RSACTION:														// 01/13/00 AM.
			// Build stmt semantic object.
			semexpr = semstmts;	// Call a spade a spade.
			stmt1 = new Iexprstmt(semexpr,
										 pnstmt->getLine());					// 03/12/02 AM.

			// Build stmts semantic object.
			stmts = new Slist<Istmt>(stmt1);
			semstmts = new RFASem(stmts);
			break;
		case RSSTMT:
			// Get stmt semantic object.
			semstmt1 = semstmts;				// Call a spade a spade.
			stmt1 = semstmt1->getStmt();
			semstmt1->setStmt(0);
			delete semstmt1;

			// Build stmts semantic object.
			stmts = new Slist<Istmt>(stmt1);
			semstmts = new RFASem(stmts);
			break;
		case RSSTMTS:
			stmts = semstmts->getStmts();
			break;
		default:
			{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA addstmt action: Bad stmts semantic object.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		}

			return false;
		}
	pnstmts->setSem(0);		// Clear out of node.
	}
// At this point, there's a semstmts and a stmts.				// 02/12/00 AM.
if (!(semstmt = (RFASem *) pnstmt->getSem()))
	{
	// FIX. Handling empty statements.									// 08/01/00 AM.
//	*gerr << "[RFA addstmt action: No semantics in stmt.]" << endl;
//	return false;											// FIX.			// 08/01/00 AM.
	}
else
	{
	switch (semstmt->getType())
		{
		case RSEXPR:
		case RSACTION:														// 01/13/00 AM.
			{
			nlppp->parse_->inputline_										// 08/24/02 AM.
					= pnstmt->getLine();// Line num in INPUT file.	// 05/29/01 AM.
			_t_strstream gerrStr;				// 05/29/01 AM.
			gerrStr << _T("[Warning: Missing ';']") << ends;				// 05/29/01 AM.
			nlppp->parse_->errOut(&gerrStr,true,true);// Pass num of INPUT file.	// 05/29/01 AM.
			}
			// Build stmt semantic object.
			semexpr = semstmt;	// Call a spade a spade.
			stmt = new Iexprstmt(semexpr);
			stmts->rpush(stmt);			// Glom the new stmt.	// 12/15/99 AM.
			break;
		case RSSTMT:
			stmt = semstmt->getStmt();
			semstmt->setStmt(0);
			delete semstmt;
			stmts->rpush(stmt);			// Glom the new stmt.	// 12/15/99 AM.
			break;
		case RSSTMTS:					// From a BLOCK of stmts.	// 12/15/99 AM.
			block = semstmt->getStmts();								// 12/15/99 AM.
			// Glom the block statements.								// 12/15/99 AM.
			stmts = Slist<Istmt>::merge(stmts,block);				// 12/15/99 AM.
			block = 0;														// 12/15/99 AM.
			semstmt->setStmts(0);	// FIX.							// 02/14/00 AM.
			delete semstmt;			// FIX.							// 02/14/00 AM.
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA addstmt action: Bad stmt semantic object.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			}

			return false;
		}
	pnstmt->setSem(0);					// Clear out of node.
	}

//stmts->rpush(stmt);					// Glom the new stmt.	// 12/15/99 AM.

// Build sem object from stmts list.
RFASem *rfasem;
// rfasem = new RFASem(stmts);		// FIX.						// 02/12/00 AM.
rfasem = semstmts;						// FIX.						// 02/12/00 AM.

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAIFSTMT
* CR:		11/12/99 AM.
* SUBJ:	Create an IF STMT semantic object.
* RET:	True if ok, else false.
* RULE:	@POST
*				ifstmt(1, 2)
*				single()
*			@RULES
*			_IFSTMT <- _IFPART _xWILD [s one match=(_BLOCK _STMT _EXPR)] @@
* FORM:	postRFAifstmt(1,2) -- 1 - expr,num. 2- stmts, stmt,expr.
********************************************/

bool PostRFA::postRFAifstmt(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *ncond = 0, *nif = 0;

if (!args_0to2(_T("ifstmt"),args,nlppp->collect_,nlppp->sem_, /*DU*/ ncond,nif))
	return false;

// Get semantics from the kiddies.
Pn *pncond = ncond->getData();
Pn *pnif = nif->getData();
RFASem *semcond = 0, *semif = 0;
Iexprstmt *estmt = 0;														// 06/05/00 AM.
if (!(semcond = (RFASem *) pncond->getSem()))
	{
	// Condition could be a _NUM or _STR.
	// But not handling these useless cases for now.
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ifstmt action: No semantics in cond. Unimplemented.]")
			  << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);

	return false;
	}
else
	{
	switch (semcond->getType())
		{
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSVAR:  case RSEXPR:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:												// FIX.	// 11/10/01 AM.
			// Done.
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA ifstmt action: Bad cond semantic object.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	pncond->setSem(0);		// Clear out of node.
	}
if (!(semif = (RFASem *) pnif->getSem()))
	{
	// FIX. ALLOWING EMPTY IF STATEMENTS.							// 08/01/00 AM.
	// Could be _NUM, _STR. Unimplemented.
//	*gerr << "[RFA ifstmt action: No semantics in if-part. Unimplemented.]" << endl;
//	return false;									// FIX.				// 08/01/00 AM.
	}
else
	{
	switch (semif->getType())
		{
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSVAR: case RSEXPR:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:															// 01/15/01 AM.
			{
			nlppp->parse_->inputline_										// 08/24/02 AM.
						= pnif->getLine();// Line num in INPUT file.	// 05/29/01 AM.
			_t_strstream gerrStr;				// 05/29/01 AM.
			gerrStr << _T("[Warning: Missing ';']") << ends;				// 05/29/01 AM.
			nlppp->parse_->errOut(&gerrStr,true,true);// Pass num of INPUT file.	// 05/29/01 AM.
			}
			// Converting to stmt type, so that code gen
			// will handle them as C++ statements.						// 06/05/00 AM.
			estmt = new Iexprstmt(semif,									// 06/05/00 AM.
										 pncond->getLine());					// 03/12/02 AM.
			semif = new RFASem(estmt);
			break;
		case RSSTMT: case RSSTMTS:
			break;		// These are ok.  Nothing to do.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA ifstmt action: Bad if-part semantic object.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			}

			return false;
		}
	pnif->setSem(0);					// Clear out of node.
	}



// Build if statement object.
Iifstmt *ifstmt = new Iifstmt(semcond, semif,
										0, pnif->getLine());					// 03/12/02 AM.
RFASem *rfasem;
rfasem = new RFASem(ifstmt);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAIFELSE
* CR:		11/12/99 AM.
* SUBJ:	Create an IF STMT semantic object.
* RET:	True if ok, else false.
* RULE:	@POST
*				ifelse(1, 2)
*				single()
*			@RULES
*			_STMT <- _IFSTMT _ELSEPART @@
* FORM:	postRFAifelse(1,2) -- 1 - ifstmt. 2- stmts, stmt,expr, etc.
********************************************/

bool PostRFA::postRFAifelse(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nifstmt = 0, *nelse = 0;

if (!args_0to2(_T("ifelse"),args,nlppp->collect_,nlppp->sem_, /*DU*/ nifstmt,nelse))
	return false;

// Get semantics from the kiddies.
Pn *pnifstmt = nifstmt->getData();
Pn *pnelse = nelse->getData();
RFASem *semifstmt = 0, *semelse = 0;
Iifstmt *stmt = 0;
RFASem *elsepart = 0;
if (!(semifstmt = (RFASem *) pnifstmt->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ifelse action: No semantics in if-statement.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
else
	{
	if (semifstmt->getType() != RSSTMT)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ifelse action: Bad if-stmt semantic object.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	if (!(stmt = (Iifstmt *)(semifstmt->getStmt())))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ifelse action: No if-statement object.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	if (_tcscmp(_T("ifstmt"), stmt->getType()))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ifelse action: Non if-statement.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);
		return false;
		}
	pnifstmt->setSem(0);		// Clear out of node.
	}
if (!(semelse = (RFASem *) pnelse->getSem()))
	{
	// Could be _NUM, _STR. Unimplemented.
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ifelse action: No semantics in else-part. Unimplemented.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
else
	{
	switch (semelse->getType())
		{
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSVAR: case RSEXPR: case RSSTMT: case RSSTMTS:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:															// 11/15/01 AM.
			break;		// These are ok.  Nothing to do.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA ifelse action: Bad else-part sem object.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	pnelse->setSem(0);					// Clear out of node.
	}

// Add else-part to if-stmt.
stmt->setElse(semelse);

// Reuse if statement object.
RFASem *rfasem;
rfasem = semifstmt;

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAWHILESTMT
* CR:		08/31/00 AM.
* SUBJ:	Create a WHILE STMT semantic object.
* RET:	True if ok, else false.
* RULE:	@POST
*				whilestmt(2, 3)
*				single()
*			@RULES
*			_WHILESTMT <- WHILE _EXPR _xWILD [s one match=(_BLOCK _STMT)] @@
* FORM:	whilestmt(2,3) -- 2 - expr,num. 3- stmts, stmt.
********************************************/

bool PostRFA::postRFAwhilestmt(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *ncond = 0, *nblock = 0;

if (!args_0to2(_T("whilestmt"),args,nlppp->collect_,nlppp->sem_,
																		/*DU*/ ncond,nblock))
	return false;

// Get semantics from the kiddies.
Pn *pncond = ncond->getData();
Pn *pnblock = nblock->getData();
RFASem *semcond = 0, *semblock = 0;
Iexprstmt *estmt = 0;
if (!(semcond = (RFASem *) pncond->getSem()))
	{
	// Condition could be a _NUM or _STR.
	// But not handling these useless cases for now.
	_t_strstream gerrStr;
	gerrStr << _T("[RFA whilestmt action: No semantics in cond. Unimplemented.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
else
	{
	switch (semcond->getType())
		{
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSVAR:  case RSEXPR:
		case RSACTION:
		case RSPNODE:								// PREEMPTIVE FIX.	// 11/11/01 AM.
			// Done.
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA whilestmt action: Bad cond semantic object.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;
			}
		}
	pncond->setSem(0);		// Clear out of node.
	}
if (!(semblock = (RFASem *) pnblock->getSem()))
	{
	// ALLOWING EMPTY WHILE STATEMENTS.
	// Could be _NUM, _STR. Unimplemented.
	}
else
	{
	switch (semblock->getType())
		{
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSVAR: case RSEXPR:
		case RSACTION:
		case RSPNODE:															// 11/15/01 AM.
			{
			nlppp->parse_->inputline_										// 08/24/02 AM.
					= pnblock->getLine();// Line num in INPUT file.	// 05/29/01 AM.
			_t_strstream gerrStr;				// 05/29/01 AM.
			gerrStr << _T("[Warning: Missing ';']") << ends;				// 05/29/01 AM.
			nlppp->parse_->errOut(&gerrStr,true,true);// Pass num of INPUT file.	// 05/29/01 AM.
			}
			// Converting to stmt type, so that code gen
			// will handle them as C++ statements.
			estmt = new Iexprstmt(semblock,
										 pnblock->getLine());				// 03/12/02 AM.
			semblock = new RFASem(estmt);
			break;
		case RSSTMT: case RSSTMTS:
			break;		// These are ok.  Nothing to do.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA whilestmt action: Bad semantic object.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			}

			return false;
		}
	pnblock->setSem(0);					// Clear out of node.
	}


// Build if statement object.
Iwhilestmt *whilestmt = new Iwhilestmt(semcond, semblock,
													pncond->getLine());		// 03/12/02 AM.
RFASem *rfasem;
rfasem = new RFASem(whilestmt);

nlppp->sem_ = rfasem;
return true;
}




/********************************************
* FN:		POSTRFARETURNSTMT
* CR:		03/07/02 AM.
* SUBJ:	Create a RETURN STMT semantic object.
* RET:	True if ok, else false.
* RULE:	@POST
*			returnstmt(2)
*			single()
*			@RULES
*			_STMT <-	
*				_RETURN
*				_xWILD [s opt match=(_EXPR _NUM _FLOAT _STR)]
*				\; [s]
*				@@
* FORM:	returnstmt(1,2) -- 1 - return keyword, 2 - expr, num.
********************************************/

bool PostRFA::postRFAreturnstmt(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *nret=0,*nexpr=0;													// 03/12/02 AM.
if (!args_1or2(_T("returnstmt"),args,nlppp->collect_,nlppp->sem_,
																		/*DU*/ nret,nexpr))
	return false;

Pn *pnret = nret->getData();												// 03/12/02 AM.

if (!nexpr)	// No expression to evaluate.
	{
	RFASem *semexpr = new RFASem((long)0);	// Value for expr of return stmt.
	Ireturn *returnstmt = new Ireturn(semexpr,
												 pnret->getLine());			// 03/12/02 AM.
	RFASem *rfasem = new RFASem(returnstmt);
	nlppp->sem_ = rfasem;
	return true;
	}

// Get semantics from the kiddies.
Pn *pnexpr = nexpr->getData();
RFASem *semexpr = 0;
Iexprstmt *estmt = 0;
if (!(semexpr = (RFASem *) pnexpr->getSem()))
	{
	// Expr could be a _NUM or _STR.
	// But not handling yet.
	_t_strstream gerrStr;
	gerrStr << _T("[RFA returnstmt action: Num,str return. Unimplemented.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);
	return false;
	}
else
	{
	switch (semexpr->getType())
		{
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSFLOAT: case RSVAR:  case RSEXPR: case RSACTION: case RSPNODE:
			// Done.
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA returnstmt action: Bad expr semantic object.]")
					  << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			return false;
			}
		}
	pnexpr->setSem(0);		// Clear out of node.
	}

// Build if statement object.
Ireturn *returnstmt = new Ireturn(semexpr,
											 pnret->getLine());				// 03/12/02 AM.
RFASem *rfasem;
rfasem = new RFASem(returnstmt);

nlppp->sem_ = rfasem;
return true;
}

/********************************************
* FN:		POSTRFAEXPR
* CR:		11/10/99 AM.
* SUBJ:	Create a BINARY expr semantic object.
* RET:	True if ok, else false.
* RULE:	_EXPR <- _EXPR _OP _EXPR @@   [for example]
* FORM:	postRFAexpr(1,2,3) -- lhs,op,rhs.
* NOTE:	This will be somewhat complex, because it must handle
*			num, var, expr in lhs and rhs.  Also, the op can be a semantic
*			object or a plain "<" character, eg.
*			Actually, will glom sem object if present, else will build one,
*			for each thingy.  Not too complex.
********************************************/

bool PostRFA::postRFAexpr(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nlhs = 0, *nop = 0, *nrhs = 0;

if (!args_0to3(_T("expr"),args,nlppp->collect_,nlppp->sem_, /*DU*/ nlhs,nop,nrhs))
	return false;

// Get semantics from the kiddies.
Pn *pnlhs = nlhs->getData();
Pn *pnop  = nop->getData();
Pn *pnrhs = nrhs->getData();
RFASem *semlhs = 0, *semop = 0, *semrhs = 0;
Iop *iop = 0;				// Operator semantic object.
enum Iexprtype typ = BINARYEXPR;
_TCHAR *str, *name, *tstr;
long num;
if (!(semlhs = (RFASem *) pnlhs->getSem()))
	{
	// If it's an atomic thing like a number or string, build a sem for it.
	tstr = pnlhs->pnStr();
	nlppp->parse_->internStr(tstr,/*UP*/ str);	// FIX		// 02/11/00 AM.
	Chars::destroy(tstr);								// FIX.		// 02/11/00 AM.
	name = pnlhs->getName();
	if (!_tcscmp(name, _T("_STR")))
		{
		// Build string sem object.
		semlhs = new RFASem(str, RSSTR);
		}
	else if (!_tcscmp(name, _T("_NUM")))
		{
		// Build numeric sem object.
		if (!str_to_long(str, /*UP*/ num))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: Bad num in lhs.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		semlhs = new RFASem(num);
		}
	else
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA expr action: Bad lhs.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	}
else
	{
	// Check for appropriate sem object.
	switch (semlhs->getType())
		{
		case RSNUM:
			// Converting to a pure numeric type.		// 11/15/99 AM.
			if (!str_to_long(semlhs->getName(), /*UP*/ num))
				{
				_t_strstream gerrStr;
				gerrStr << _T("[RFA expr action: Bad numeric in lhs.]") << ends;
				return nlppp->parse_->errOut(&gerrStr,false,true);
				}
			semlhs->setType(RSLONG);
			semlhs->setLong(num);
			break;
		case RSNAME: case RSSTR: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSEXPR: case RSVAR:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:												// FIX.	// 11/15/01 AM.
			break;			// These are ok.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: Bad lhs expr.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	pnlhs->setSem(0);		// Clear out of node.
	}
if (!(semop = (RFASem *) pnop->getSem()))
	{
	// If it's a single-char operator, build a sem for it.
	tstr = pnop->pnStr();
	nlppp->parse_->internStr(tstr,/*UP*/ str);	// FIX		// 02/11/00 AM.
	Chars::destroy(tstr);								// FIX.		// 02/11/00 AM.
	if (*(str+1))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA expr action: Bad multi-char operator.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	enum Ioptype optype = NULLOP;
	switch (*str)
		{
		case '<':	optype = LTOP;			break;
		case '>':	optype = GTOP;			break;
		case '+':	optype = PLUSOP;		break;
		case '-':	optype = MINUSOP;		break;
		case '/':	optype = DIVIDEOP;	break;
		case '*':	optype = TIMESOP;		break;
		case '%':	optype = REMOP;		break;	// Remainder. // 12/31/99 AM.
		case '=':
			optype = ASSIGNOP;
			typ = ASSIGNEXPR;			// Gets its own expr type.
			break;
		case '!':
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: '!' can't be binary op.]") << ends;
			nlppp->parse_->errOut(&gerrStr,false,true);
			}

			return false;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: Bad single-char operator.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	// Now build sem.
	iop = new Iop(optype);
	}
else
	{
	// Check for appropriate sem object.
	if (semop->getType() != RSOP)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA expr action: Bad sem object for operator.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	// For the object, take its particular structure.
	iop = semop->getOp();			// Keep this guy.
	switch (iop->getType())
		{
		case PLUSOP: case MINUSOP: case TIMESOP: case DIVIDEOP:
		case EQOP:   case NEOP: case LTOP: case LEOP: case GTOP: case GEOP:
		case ANDOP:  case OROP:
		case CONFOP:																// 12/17/99 AM.
		case REMOP:																	// 12/31/99 AM.
		case OUTOP:																	// 12/31/99 AM.
			break;	// These are ok.
		case DECOP: case INCOP: case NOTOP:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: '") << Iop::Optype(iop->getType())
					  << _T("' can't be a binary operator.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		case NULLOP:
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: Bad operator.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	semop->setOp(0);					// Clear, so iop won't be deleted.
	delete semop;						// Delete the containing sem object.
	pnop->setSem(0);					// Clear out of node.
	}
if (!(semrhs = (RFASem *) pnrhs->getSem()))
	{
	// If it's an atomic thing like a number or string, build a sem for it.
	tstr = pnrhs->pnStr();
	nlppp->parse_->internStr(tstr,/*UP*/ str);	// FIX		// 02/11/00 AM.
	Chars::destroy(tstr);								// FIX.		// 02/11/00 AM.
	name = pnrhs->getName();
	if (!_tcscmp(name, _T("_STR")))
		{
		// Build string sem object.
		semrhs = new RFASem(str, RSSTR);
		}
	else if (!_tcscmp(name, _T("_NUM")))
		{
		// Build numeric sem object.
		if (!str_to_long(str, /*UP*/ num))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: Bad num in rhs.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		semrhs = new RFASem(num);
		}
	else
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA expr action: Bad rhs.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	}
else
	{
	// Check for appropriate sem object.
	switch (semrhs->getType())
		{
		case RSNUM:
			// Converting to a pure numeric type.		// 11/15/99 AM.
			if (!str_to_long(semrhs->getName(), /*UP*/ num))
				{
				_t_strstream gerrStr;
				gerrStr << _T("[RFA expr action: Bad numeric in rhs.]") << ends;
				return nlppp->parse_->errOut(&gerrStr,false,true);
				}
			semrhs->setType(RSLONG);
			semrhs->setLong(num);
			break;
		case RSNAME: case RSSTR: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSEXPR: case RSVAR:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:															// 10/18/00 AM.
			break;			// These are ok.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA expr action: Bad rhs expr.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	pnrhs->setSem(0);					// Clear out of node.
	}

long line = pnlhs->getLine();	// Line number in pass file.		// 05/18/01 AM.

// For the exprs, keep them as RFASem objects.
// Build sem object for the binary expr.
RFASem *rfasem;
Iexpr *expr;				// Object to build.
expr = new Iexpr(typ, semlhs, iop, semrhs,
											0, line);							// 05/18/01 AM.
rfasem = new RFASem(expr);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAUNARY
* CR:		11/11/99 AM.
* SUBJ:	Create a UNARY expr semantic object.
* RET:	True if ok, else false.
* RULE:	_EXPR <- _OP _EXPR @@   [for example]
* FORM:	postRFAunary(1,2) -- op,rhs.
* NOTE:	In Iexpr, using rhs for unary expression.
*			If no sem, then build one, eg, for number.
*			For now, ignoring strings that could be converted to nums.
********************************************/

bool PostRFA::postRFAunary(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nop = 0, *nrhs = 0;

if (!args_0to2(_T("unary"),args,nlppp->collect_,nlppp->sem_, /*DU*/ nop,nrhs))
	return false;

// Get semantics from the kiddies.
Pn *pnop  = nop->getData();
Pn *pnrhs = nrhs->getData();
RFASem *semop = 0, *semrhs = 0;
Iop *iop = 0;				// Operator semantic object.
_TCHAR *str, *name, *tstr;
long num;

if (!(semop = (RFASem *) pnop->getSem()))
	{
	// If it's a single-char operator, build a sem for it.
	tstr = pnop->pnStr();
	nlppp->parse_->internStr(tstr,/*UP*/ str);	// FIX		// 02/11/00 AM.
	Chars::destroy(tstr);								// FIX.		// 02/11/00 AM.
	if (*(str+1))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA unary action: Bad multi-char operator.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	enum Ioptype optype = NULLOP;
	switch (*str)
		{
		case '!':	optype = NOTOP;		break;
		case '+':	optype = PLUSOP;		break;
		case '-':	optype = MINUSOP;		break;
		case '<': case '>': case '/': case '*':
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA unary action: '")
							<< *str
							<< _T("' can't be a unary op.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA unary action: Bad single-char operator.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	// Now build sem.
	iop = new Iop(optype);
	}
else
	{
	// Check for appropriate sem object.
	if (semop->getType() != RSOP)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA unary action: Bad sem object for operator.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}

	// For the object, take its particular structure.
	iop = semop->getOp();			// Keep this guy.
	switch (iop->getType())
		{
		case INCOP:
		case DECOP:
		case NOTOP:
		case PLUSOP:
		case MINUSOP:
			break;			// These are ok.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA postunary action: '") << Iop::Optype(iop->getType())
							<< _T("' can't be a post operator.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	semop->setOp(0);					// Clear, so iop won't be deleted.
	delete semop;						// Delete the containing sem object.
	pnop->setSem(0);					// Clear out of node.
	}
if (!(semrhs = (RFASem *) pnrhs->getSem()))
	{
	// If it's an atomic thing like a number or string, build a sem for it.
	tstr = pnrhs->pnStr();
	nlppp->parse_->internStr(tstr,/*UP*/ str);	// FIX		// 02/11/00 AM.
	Chars::destroy(tstr);								// FIX.		// 02/11/00 AM.
	name = pnrhs->getName();
	if (!_tcscmp(name, _T("_STR")))
		{
		// Build string sem object.
		semrhs = new RFASem(str, RSSTR);
		}
	else if (!_tcscmp(name, _T("_NUM")))
		{
		// Build numeric sem object.
		if (!str_to_long(str, /*UP*/ num))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA unary action: Bad num in expr.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		semrhs = new RFASem(num);
		}
	else
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA unary action: Bad expr.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	}
else
	{
	// Check for appropriate sem object.
	switch (semrhs->getType())
		{
		case RSNUM:																// 06/05/00 AM.
			// Convert to LONG.												// 06/05/00 AM.
			if (!str_to_long(semrhs->getNum(), num))					// 06/05/00 AM.
				{
				_t_strstream gerrStr;
				gerrStr << _T("[Postunary: Couldn't convert to number.]") << ends;
				return nlppp->parse_->errOut(&gerrStr,false,true);
				}
			delete semrhs;		// Delete the old one.					// 06/05/00 AM.
			semrhs = new RFASem(num);										// 06/05/00 AM.
			break;

		case RSNAME: case RSSTR: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSEXPR: case RSVAR:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:											// BUG FIX.	// 11/11/01 AM.
			break;			// These are ok.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA unary action: Bad expr.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	pnrhs->setSem(0);					// Clear out of node.
	}

long line = pnop->getLine();	// Line number in pass file.		// 05/18/01 AM.

// For the exprs, keep them as RFASem objects.
// Build sem object for the binary expr.
RFASem *rfasem;
Iexpr *expr;				// Object to build.
expr = new Iexpr(UNARYEXPR, 0, iop, semrhs,
											0, line);							// 05/18/01 AM.
rfasem = new RFASem(expr);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAPOSTUNARY
* CR:		11/11/99 AM.
* SUBJ:	Create a POSTUNARY expr semantic object.
* RET:	True if ok, else false.
* RULE:	_EXPR <- _EXPR _OP @@   [for example]
* FORM:	postRFApostunary(1,2) -- rhs,op.
* NOTE:	In Iexpr, using rhs for unary expressions.
*			If no sem, then build one, eg, for number.
*			ONLY Accepting VAR as expression.
*			For now, ignoring strings that could be converted to nums.
********************************************/

bool PostRFA::postRFApostunary(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nop = 0, *nrhs = 0;

if (!args_0to2(_T("postunary"),args,nlppp->collect_,nlppp->sem_, /*DU*/ nrhs,nop))
	return false;

// Get semantics from the kiddies.
Pn *pnrhs = nrhs->getData();
Pn *pnop  = nop->getData();
RFASem *semrhs = 0, *semop = 0;
Iop *iop = 0;				// Operator semantic object.
_TCHAR *str, *tstr;

if (!(semrhs = (RFASem *) pnrhs->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA postunary action: Bad expr.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
else
	{
	// Check for appropriate sem object.
	switch (semrhs->getType())
		{
		case RSVAR:
			break;			// This is the only ok one.
		case RSNAME: case RSSTR: case RSNUM: case RSLONG:
		case RSFLOAT:															// 08/19/01 AM.
		case RSEXPR:
		case RSACTION:															// 01/13/00 AM.
		case RSPNODE:															// 11/15/01 AM.
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA postunary action: Expr must be a variable.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA postunary action: Bad expr.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	pnrhs->setSem(0);					// Clear out of node.
	}
if (!(semop = (RFASem *) pnop->getSem()))
	{
	tstr = pnop->pnStr();
	nlppp->parse_->internStr(tstr,/*UP*/ str);	// FIX		// 02/11/00 AM.
	Chars::destroy(tstr);								// FIX.		// 02/11/00 AM.
	if (*(str+1))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA postunary action: Bad multi-char operator.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	else	// Single-char ops are bad also.
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA postunary action: Operator must be ++ or --]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	}
else
	{
	// Check for appropriate sem object.
	if (semop->getType() != RSOP)
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA postunary action: Bad sem object for operator.]") << ends;
		return nlppp->parse_->errOut(&gerrStr,false,true);
		}
	// For the object, take its particular structure.
	iop = semop->getOp();			// Keep this guy.
	switch (iop->getType())
		{
		case INCOP:
		case DECOP:
			break;			// These are ok.
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA postunary action: '") << Iop::Optype(iop->getType())
							<< _T("' can't be a post operator.]") << ends;
			return nlppp->parse_->errOut(&gerrStr,false,true);
			}
		}
	semop->setOp(0);					// Clear, so iop won't be deleted.
	delete semop;						// Delete the containing sem object.
	pnop->setSem(0);					// Clear out of node.
	}

long line = pnrhs->getLine();	// Line number in pass file.		// 05/18/01 AM.

// For the exprs, keep them as RFASem objects.
// Build sem object for the binary expr.
RFASem *rfasem;
Iexpr *expr;				// Object to build.
expr = new Iexpr(POSTUNARYEXPR, 0, 0, semrhs, iop,
															line);				// 05/18/01 AM.
rfasem = new RFASem(expr);

nlppp->sem_ = rfasem;
return true;
}

/********************************************
* FN:		POSTRFAVAR
* CR:		11/09/99 AM.
* SUBJ:	Create a variable semantic object.
* RET:	True if ok, else false.
* RULE:	_VAR [layer=(_EXPR)] <- _xWILD [s one match=( s G N X P )]
*						_LIST @@
* FORM:	postRFAvar(1,2)
* NOTE:
********************************************/

bool PostRFA::postRFAvar(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *nname, *nlist;
nname = nlist = 0;

if (!args_0to2(_T("var"),args,nlppp->collect_,nlppp->sem_, /*DU*/ nname,nlist))
	return false;

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semname, *semlist;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	return false;
if (nlist)
	semlist = (RFASem *) nlist->getData()->getSem();
else
	semlist = 0;

// Some more checking.  May also want to check on node names.
// (ie, are they _LIT and _LIST.  If not, fail.)
if (semname->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA var action: Bad semantic type.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (semlist && semlist->getType() != RSARGS)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA var action: Bad semantic type.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

_TCHAR *name;
_TCHAR typestr[256];
Dlist<Iarg> *iargs = 0;

_tcscpy(typestr, semname->getName());
if (!typestr[0])
	return false;
if (semlist && !(iargs = semlist->getArgs()))
	return false;

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.

delete semname;
nname->getData()->setSem(0);

if (semlist)
	{
	semlist->setArgs(0);
	delete semlist;
	nlist->getData()->setSem(0);
	}
// Now we own the iargs.												// 02/13/00 AM.

// Get the variable name and the position number (if any) from args.
Iarg *namearg = 0;
Iarg *numarg = 0;
long num = 0;		// Important init.	// FIX.		// 11/24/99 AM.
Delt<Iarg> *darg;
if (!(darg = iargs->getFirst()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA var action: No variable name.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);

	Dlist<Iarg>::DeleteDlistAndData(iargs);	// FIX.			// 02/13/00 AM.
	return false;
	}
namearg = darg->getData();
if (namearg->getType() != IASTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA var action: No variable name.]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);

	Dlist<Iarg>::DeleteDlistAndData(iargs);	// FIX.			// 02/13/00 AM.
	return false;
	}
name = namearg->getStr();

if (darg = darg->Right())	// If there's a second arg.
	{
	if (darg->Right())
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA var action: Too many arguments.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		Dlist<Iarg>::DeleteDlistAndData(iargs);	// FIX.		// 02/13/00 AM.
		return false;
		}
	numarg = darg->getData();
	if (!numarg->fetchNum(/*UP*/ num))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA var action: Bad number arg.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false,true);

		Dlist<Iarg>::DeleteDlistAndData(iargs);	// FIX.		// 02/13/00 AM.
		return false;
		}
	}

enum Ivartype typ;
typ = Ivar::Vartype(typestr[0]);		// Use first letter of typestring.
if (typ == NULLVAR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA var action: Bad variable name=") << typestr << _T("]") << ends;
	nlppp->parse_->errOut(&gerrStr,false,true);

	Dlist<Iarg>::DeleteDlistAndData(iargs);	// FIX.			// 02/13/00 AM.
	return false;
	}

// Should check if variable type is compatible with numeric 2nd arg....

Dlist<Iarg>::DeleteDlistAndData(iargs);	// FIX.			// 02/13/00 AM.

RFASem *rfasem;

Ivar *var;				// Object to build.
var = new Ivar(typ, name, num);
rfasem = new RFASem(var);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAOP
* CR:		11/09/99 AM.
* SUBJ:	Create an operator semantic object.
* RET:	True if ok, else false.
* RULE:	_opINC <- \+ \+ @@    [for example]
* FORM:	postRFAop(1,2)
* NOTE:
********************************************/

bool PostRFA::postRFAop(
	Delt<Iarg> *args,
	Nlppp *nlppp								// 11/19/99 AM.
	)
{
Node<Pn> *n1 = 0, *n2 = 0;

if (!args_0to2(_T("op"),args,nlppp->collect_,nlppp->sem_, /*DU*/ n1,n2))
	return false;

if (!n1 || !n2)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA op action: Only handles two-char operators.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

////////////////////////
// BUILD AND ATTACH SEM
////////////////////////

// Get operator name from the two nodes.
// (Assuming two-char operators.)

Pn *pn1 = n1->getData();
Pn *pn2 = n2->getData();
_TCHAR *txt1 = pn1->getName();
_TCHAR *txt2 = pn2->getName();

if (!txt1 || !*txt1 || !txt2 || !*txt2)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA op action: Operator must be two single chars.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}
if (*(txt1+1) || *(txt2+1))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA op action: Two single chars need for operator.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}

_TCHAR opstr[3];
_stprintf(opstr, _T("%c%c"), *txt1, *txt2);	// Glom the chars together.

// Figure out the operator type and build semantic object.
enum Ioptype typ = Iop::Optype(opstr);
if (typ == NULLOP)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA op action: Bad operator type.]") << ends;
	return nlppp->parse_->errOut(&gerrStr,false,true);
	}


RFASem *rfasem;

Iop *op;				// Object to build.
op = new Iop(typ);
rfasem = new RFASem(op);

nlppp->sem_ = rfasem;
return true;
}



/**************************************************************************
//                 UTILITIES
/**************************************************************************

/********************************************
* FN:		ARGS_1
* CR:		11/23/98 AM.
* SUBJ:	Helper function for getting one arg for an action.
* RET:	True if ok, else false.
********************************************/

bool PostRFA::args_1(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &nstart,		// Parse tree node for arg.
	Node<Pn>* &nend			// End node, as needed.
	)
{
if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: One arg required.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
	     << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}

Iarg *arg1;
arg1 = args->getData();
if (args = args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than one arg given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Check self-consistency.
if (!num1)
	return false;

// num1 is collect for nodes.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = collect->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;


// Get needed stuff from each collect.
nstart = coll1->Down();
nend   = coll1->eDown();

if (!nstart && !nend)
	return true;				// Empty arg is ok.
if (!nstart || !nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Unhandled null case.]") << ends;
	return errOut(&gerrStr,false);
	}

if (Verbose())
	{
	_t_strstream gerrStr;
	gerrStr << _T("   [Executing RFA ")
		  << action
		  << _T(" action.]") << ends;
	errOut(&gerrStr,false);
	}


if (Debug())
	{
	*gout << _T("nodes=");
	Pn::prettyNodes(nstart);
	*gout << endl;
	}
return true;
}


/********************************************
* FN:		ARGS_1OR2
* CR:		11/23/98 AM.
* SUBJ:	Helper function for getting one or two args for an action.
* RET:	True if ok, else false.
********************************************/

bool PostRFA::args_1or2(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &node1,			// Parse tree node for first arg.
	Node<Pn>* &node2			// Parse tree node for second arg, if any.
	)
{
if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: One or two args required.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}


Iarg *arg1;
Iarg *arg2;
arg1 = args->getData();
if (args->Right())
	{
	args = args->Right();
	arg2 = args->getData();
	}
else
	arg2 = 0;		// Pairs argument is optional.


if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than two args given.]") << ends;
	return errOut(&gerrStr,false);
	}
long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg1 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num2;
if (arg2)
	{
	switch (arg2->getType())				// 11/15/99 AM.
		{
		case IANUM:								// 11/15/99 AM.
			num2 = arg2->getNum();			// 11/15/99 AM.
			break;
		case IASTR:								// 11/15/99 AM.
			// Convert strings to numbers.
			if (!str_to_long(arg2->getStr(), num2))
				return false;
			break;
		default:
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFA ")
						  << action
						  << _T(" action: Arg2 must be numeric.]") << ends;
			return errOut(&gerrStr,false);
			}
		}
	}

// Check self-consistency.
if (arg2)
	{
	if (!num1 || !num2 || num1 > num2)
		return false;
	num2 -= num1;			// Now num2 is the number of nodes beyond num1.
	}
else
	num2 = 0;

// num1 is collect for first elt.
// num2 is distance to second elt, if any.
Node<Pn> *colls;
Node<Pn> *coll1, *coll2;
colls = collect->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;
while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		break;
	}
if (arg2)
	coll2 = colls;			// Can be null.
else
	coll2 = 0;

// Get needed stuff from each collect.
if (!(node1 = coll1->Down()))
	return false;

if (coll2)
	node2 = coll2->Down();
else
	node2 = 0;
return true;
}


/********************************************
* FN:		ARGS_0TO2
* CR:		11/24/98 AM.
* SUBJ:	Helper function for getting zero to two args for an action.
* RET:	True if ok, else false.
* NOTE:	Need more generic place to put these arg-fetching functions.
********************************************/

bool PostRFA::args_0to2(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &node1,			// Parse tree node for first arg, if any.
	Node<Pn>* &node2			// Parse tree node for second arg, if any.
	)
{
node1 = node2 = 0;

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Two args required, not zero.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}

Iarg *arg1;
Iarg *arg2;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Two args required, not one.]") << ends;
	return errOut(&gerrStr,false);
	}
arg2 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than three args given.]") << ends;
	return errOut(&gerrStr,false);
	}
long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg1 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg2 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Check self-consistency.
if (!num1 && !num2)
	return true;				// Accepting all null arguments.
// 11/24/98 AM. Accepting any numeric values, as long as in the
// collect.
long len;
Node<Pn> *colls;
colls = collect->getRoot();
len = colls->length();
if (num1 > len || num2 > len)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Numerical arg too large.]") << ends;
	return errOut(&gerrStr,false);
	}


Node<Pn> *coll1, *coll2;
coll1 = coll2 = 0;

coll1 = colls->nth(num1);
coll2 = colls->nth(num2);

// Get needed stuff from each collect.
if (coll1)
	node1 = coll1->Down();
if (coll2)
	node2 = coll2->Down();

return true;
}


/********************************************
* FN:		ARGS_RANGE
* CR:		01/17/99 AM.
* SUBJ:	Helper function for getting a range from two args.
* RET:	True if ok, else false.
* NOTE:	Need more generic place to put these arg-fetching functions.
********************************************/

bool PostRFA::args_range(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &node1,			// Parse tree node for first arg, if any.
	Node<Pn>* &node2			// Parse tree node for second arg, if any.
	)
{
node1 = node2 = 0;

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Two args required, not zero.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}

Iarg *arg1;
Iarg *arg2;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Two args required, not one.]") << ends;
	return errOut(&gerrStr,false);
	}
arg2 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than three args given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg1 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg2 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Check self-consistency.
if (!num1 && !num2)
	return true;				// Accepting all null arguments.
// 11/24/98 AM. Accepting any numeric values, as long as in the
// collect.
long len;
Node<Pn> *colls;
colls = collect->getRoot();
len = colls->length();
if (num1 > len || num2 > len)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Numerical arg too large.]") << ends;
	return errOut(&gerrStr,false);
	}
if (num1 > num2)					// Preemptive.	// 09/26/99 AM.
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ") << action
		   << _T(" action: Bad range.]") << ends;
	return errOut(&gerrStr,false);
	}

Node<Pn> *coll1, *coll2;
coll1 = coll2 = 0;

coll1 = colls->nth(num1);
coll2 = colls->nth(num2);

// Get needed stuff from each collect.
if (coll1)
	node1 = coll1->Down();
if (coll2)
	node2 = coll2->eDown();	// 01/17/99 AM.  This is the difference.

// If end of range is NULL, back up to find a node.			// 09/26/99 AM.
// Assume that if there's a start, there's an end.
if (node1 && node2)				// FIX!								// 01/11/00 AM.
	return true;															// 09/26/99 AM.

if (!node2)							// FIX								// 01/11/00 AM.
	{
	while (--num2 >= num1)												// 09/26/99 AM.
		{
		if (coll2 = colls->nth(num2))									// 09/26/99 AM.
			{
			if (node2 = coll2->eDown())								// 09/26/99 AM.
//				return true;												// 01/11/00 AM.
				break;														// 01/11/00 AM.
			}
		}
	}

// NODE1 can be optional, or a placeholder for _xSTART.		// 01/11/00 AM.
if (!node1)																	// 01/11/00 AM.
	{
	while (++num1 <= num2)												// 01/11/00 AM.
		{
		if (coll1 = colls->nth(num1))									// 01/11/00 AM.
			{
//			if (node1 = coll1->eDown())						// BUG	// 02/13/02 AM.
			if (node1 = coll1->Down())							// FIX	// 02/13/02 AM.
				break;
			}
		}
	}
if (node1 && node2)														// 01/11/00 AM.
	return true;
return false;																// 01/11/00 AM.
}


/********************************************
* FN:		ARGS_0TO3
* CR:		11/24/98 AM.
* SUBJ:	Helper function for getting zero to three args for an action.
* RET:	True if ok, else false.
********************************************/

bool PostRFA::args_0to3(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &node1,			// Parse tree node for first arg, if any.
	Node<Pn>* &node2,			// Parse tree node for second arg, if any.
	Node<Pn>* &node3			// Parse tree node for third arg, if any.
	)
{
node1 = node2 = node3 = 0;

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Three args required, not zero.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}

Iarg *arg1;
Iarg *arg2;
Iarg *arg3;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Three args required, not one.]") << ends;
	return errOut(&gerrStr,false);
	}
arg2 = args->getData();
if (!(args=args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Three args required, not two.]") << ends;
	return errOut(&gerrStr,false);
	}
arg3 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than three args given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg1 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg2 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num3;
switch (arg3->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num3 = arg3->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg3->getStr(), num3))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg3 must be numeric.]") << ends;
		errOut(&gerrStr,false);
		}
	}



// Check self-consistency.
if (!num1 && !num2 && !num3)
	return true;				// Accepting all null arguments.
// 11/24/98 AM. Accepting any numeric values, as long as in the
// collect.
long len;
Node<Pn> *colls;
colls = collect->getRoot();
len = colls->length();
if (num1 > len || num2 > len || num3 > len)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Numerical arg too large.]") << ends;
	return errOut(&gerrStr,false);
	}


Node<Pn> *coll1, *coll2, *coll3;
coll1 = coll2 = coll3 = 0;

coll1 = colls->nth(num1);
coll2 = colls->nth(num2);
coll3 = colls->nth(num3);

// Get needed stuff from each collect.
if (coll1)
	node1 = coll1->Down();
if (coll2)
	node2 = coll2->Down();
if (coll3)
	node3 = coll3->Down();

return true;
}


/********************************************
* FN:		ARGS_0TO5
* CR:		12/19/01 AM.
* SUBJ:	Helper function for getting zero to five args for an action.
* RET:	True if ok, else false.
********************************************/

bool PostRFA::args_0to5(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &node1,			// Parse tree node for first arg, if any.
	Node<Pn>* &node2,			// Parse tree node for second arg, if any.
	Node<Pn>* &node3,			// Parse tree node for third arg, if any.
	Node<Pn>* &node4,			// Parse tree node for 4th arg, if any.
	Node<Pn>* &node5			// Parse tree node for 5th arg, if any.
	)
{
node1 = node2 = node3 = node4 = node5 = 0;

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Five args required, not zero.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}

Iarg *arg1,*arg2,*arg3,*arg4,*arg5;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Five args required, not one.]") << ends;
	return errOut(&gerrStr,false);
	}
arg2 = args->getData();
if (!(args=args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Five args required, not two.]") << ends;
	return errOut(&gerrStr,false);
	}
arg3 = args->getData();
if (!(args=args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Five args required, not three.]") << ends;
	return errOut(&gerrStr,false);
	}
arg4 = args->getData();
if (!(args=args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Five args required, not four.]") << ends;
	return errOut(&gerrStr,false);
	}
arg5 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than five args given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg1 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg2 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num3;
switch (arg3->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num3 = arg3->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg3->getStr(), num3))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg3 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num4;
switch (arg4->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num4 = arg4->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg4->getStr(), num4))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg4 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num5;
switch (arg5->getType())
	{
	case IANUM:
		num5 = arg5->getNum();
		break;
	case IASTR:
		// Convert strings to numbers.
		if (!str_to_long(arg5->getStr(), num5))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg5 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Check self-consistency.
if (!num1 && !num2 && !num3 && !num4 && !num5)
	return true;				// Accepting all null arguments.
// Accepting any numeric values, as long as in the collect.
long len;
Node<Pn> *colls;
colls = collect->getRoot();
len = colls->length();
if (num1 > len || num2 > len || num3 > len || num4 > len || num5 > len)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Numerical arg too large.]") << ends;
	return errOut(&gerrStr,false);
	}


Node<Pn> *coll1=0, *coll2=0, *coll3=0, *coll4=0,*coll5=0;

coll1 = colls->nth(num1);
coll2 = colls->nth(num2);
coll3 = colls->nth(num3);
coll4 = colls->nth(num4);
coll5 = colls->nth(num5);

// Get needed stuff from each collect.
if (coll1)
	node1 = coll1->Down();
if (coll2)
	node2 = coll2->Down();
if (coll3)
	node3 = coll3->Down();
if (coll4)
	node4 = coll4->Down();
if (coll5)
	node5 = coll5->Down();

return true;
}


/********************************************
* FN:		ARGS_0TO4
* CR:		11/27/98 AM.
* SUBJ:	Helper function for getting zero to four args for an action.
* RET:	True if ok, else false.
********************************************/

bool PostRFA::args_0to4(
	_TCHAR *action,				// Name of current postRFA action.
	Delt<Iarg> *args,			// Argument list for the action.
	Tree<Pn> *collect,		// Tracking for current rule match.
	Sem *sem,					// Semantic object built by actions, if any.
	/*DU*/
   Node<Pn>* &node1,			// Parse tree node for first arg, if any.
	Node<Pn>* &node2,			// Parse tree node for second arg, if any.
	Node<Pn>* &node3,			// Parse tree node for third arg, if any.
	Node<Pn>* &node4			// Parse tree node for 4th arg, if any.
	)
{
node1 = node2 = node3 = node4 = 0;

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Four args required, not zero.]") << ends;
	return errOut(&gerrStr,false);
	}
if (sem)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}

Iarg *arg1;
Iarg *arg2;
Iarg *arg3;
Iarg *arg4;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Four args required, not one.]") << ends;
	return errOut(&gerrStr,false);
	}
arg2 = args->getData();
if (!(args=args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Four args required, not two.]") << ends;
	return errOut(&gerrStr,false);
	}
arg3 = args->getData();
if (!(args=args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Four args required, not three.]") << ends;
	return errOut(&gerrStr,false);
	}
arg4 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: More than four args given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg1 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num2;
switch (arg2->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num2 = arg2->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg2 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num3;
switch (arg3->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num3 = arg3->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg3->getStr(), num3))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg3 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
long num4;
switch (arg4->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num4 = arg4->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg4->getStr(), num4))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFA ")
				  << action
				  << _T(" action: Arg4 must be numeric.]") << ends;
		return errOut(&gerrStr,false);
		}
	}


// Check self-consistency.
if (!num1 && !num2 && !num3 && !num4)
	return true;				// Accepting all null arguments.
// Accepting any numeric values, as long as in the collect.
long len;
Node<Pn> *colls;
colls = collect->getRoot();
len = colls->length();
if (num1 > len || num2 > len || num3 > len || num4 > len)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA ")
		  << action
		  << _T(" action: Numerical arg too large.]") << ends;
	return errOut(&gerrStr,false);
	}


Node<Pn> *coll1, *coll2, *coll3, *coll4;
coll1 = coll2 = coll3 = 0;

coll1 = colls->nth(num1);
coll2 = colls->nth(num2);
coll3 = colls->nth(num3);
coll4 = colls->nth(num4);

// Get needed stuff from each collect.
if (coll1)
	node1 = coll1->Down();
if (coll2)
	node2 = coll2->Down();
if (coll3)
	node3 = coll3->Down();
if (coll4)
	node4 = coll4->Down();

return true;
}


#ifdef UNMOVED_
/********************************************
* FN:		FILLDEFAULTS
* CR:		11/11/98 AM.
* SUBJ:	Fill some default values for an elt.
********************************************/

void PostRFA::fillDefaults(Ielt *elt)
{
_TCHAR *name;
if (!elt)
	return;
if (!(name = elt->getName()))
	return;
if (!strcmp_i(name, _T("_xWILD")))
	{
	elt->setMin(0);
	elt->setMax(0);
	}
else
	{
	elt->setMin(1);
	elt->setMax(1);
	}
}

#endif

/******************************************************************************/
// RULES-FILE BLASTER (RFB) FUNCTIONS
/******************************************************************************/


/********************************************
* FN:		POSTRFBARG
* CR:		12/18/99 AM.
* SUBJ:	Create an arg semantic object for RFB.
* RET:	True if ok, else false.
* RULE:	_ARG <- _NONLIT @@
*			_ARG <- _LIT @@
*			_ARG <- _STR @@
*			_ARG <- _NUM @@
* FORM:	postRFBarg(1)
* NOTE:	The RFA will continue to parse "everything" as strings. So the RFB
*			must be WRITTEN WITH THAT IN MIND.  However, the real analyzer rule
*			files, such as the resume analyzer, will now be parsed so that
*			IDENTIFIERS, STRINGS, AND NUMBERS are DISTINGUISHABLE.
*			Maybe someday the RFA will be written so as to do the same.
*			Copied and modified postRFAarg.
*			Anyway, rfaarg collects everything as string.
*			rfbarg distinguishes different argument types.
********************************************/

bool PostRFA::postRFBarg(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{

if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFB arg action: One arg required.]") << ends;
	return errOut(&gerrStr,false);
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFB arg action: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}


Iarg *arg1;
arg1 = args->getData();
if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFB arg action: More than one arg given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())				// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num1 = arg1->getNum();			// 11/15/99 AM.
		break;
	case IASTR:								// 11/15/99 AM.
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFB arg action: Arg must be string.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Check self-consistency.
if (num1 <= 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Bad num]") << ends;
	return errOut(&gerrStr,false);
	}

// num1 is collect for element.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[Bad collect.]") << ends;
		return errOut(&gerrStr,false);
		}
	}
coll1 = colls;

// Get needed stuff from each collect.
Node<Pn> *nname;
nname = coll1->Down();


////////////////////////
// BUILD AND ATTACH ARG
////////////////////////

// Get semantics from the kiddies.
RFASem *semname;
if (!(semname = (RFASem *) nname->getData()->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFB arg action: No semantic object for name.]") << ends;
	return errOut(&gerrStr,false);
	}

// 11/15/99 AM. HANDLING NUMERIC ARGUMENT TYPE.
Iarg *iarg;				// Sem to build.  Treat all types as string.
_TCHAR *name;
_TCHAR *str;																		// 12/18/99 AM.
long num;																		// 12/18/99 AM.
switch (semname->getType())												// 11/15/99 AM.
	{
	case RSLONG:								// 11/15/99 AM. Pure numeric.
		iarg = new Iarg(semname->getLong());							// 11/15/99 AM.
		break;
	case RSFLOAT:																// 08/19/01 AM.
		iarg = new Iarg(semname->getFloat());							// 08/19/01 AM.
		break;																	// 08/19/01 AM.
	case RSNUM:
		// NOTE: In the RFA, everything will continue to be parsed as
		// STRING!  But will have a different variant of this function
		// in the RFB.															// 12/18/99 AM.
		// OVERHAUL. TRYING TO TREAT THIS AS NUMBER!					// 12/18/99 AM.
		str = semname->getNum();											// 12/18/99 AM.
		if (!str_to_long(str, /*UP*/ num))								// 12/18/99 AM.
			{
			_t_strstream gerrStr;
			gerrStr << _T("[RFB arg: Couldn't convert to num.]") << ends;
			return errOut(&gerrStr,false);
			}
		iarg = new Iarg(num);											// 12/18/99 AM.
		break;
	case RSNAME:
		// IN THE RFB, IDENTIFIERS WILL BE DISTINGUISHED FROM STRINGS.
	case RSSTR:
		if (!(name = semname->getName()))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[Semname problem.]") << ends;
			return errOut(&gerrStr,false);
			}
		iarg = new Iarg(name);
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFB arg action: Bad semantic type.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
delete (RFASem *) semname;
nname->getData()->setSem(0);

RFASem *rfasem;
rfasem = new RFASem(iarg);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		POSTRFBSCOPEFNCALLSTART
* CR:		02/16/00 AM.
* SUBJ:	Create a function call (action) semantic object.
* RET:	True if ok, else false.
* RULE:	@POST
				scopefncallstart(1,4)
				single()
			@RULES
			_FNCALLLIST [base] <- _LIT \: \: _LIT [layer=(_FNNAME)] \( @@

* FORM:	scopefncallstart(scope_num, fnname_num)
* NOTE:	Part of overhaul to get NLP++ exprs as function arguments.
* OPT:	Excise the children also.
********************************************/

bool PostRFA::postRFBscopefncallstart(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *n1 = 0, *n2 = 0;

if (!args_0to2(_T("scopefncallstart"),args,nlppp->collect_,nlppp->sem_,
																				/*DU*/ n1,n2))
	return false;

if (!n1 || !n2)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[scopefncallstart: Two args required.]") << ends;
	return errOut(&gerrStr,false);
	}


////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////
// Get semantics from the kiddies.
RFASem *semscope;
if (!(semscope = (RFASem *) n1->getData()->getSem()))
	return false;

// Some more checking.  May also want to check on node names.
// (ie, are they _LIT and _LIST.  If not, fail.)
if (semscope->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[scopefncallstart: Bad semantic type.]") << ends;
	return errOut(&gerrStr,false);
	}

_TCHAR *scope;
Dlist<Iarg> *iargs = 0;
if (!(scope = semscope->getName()))
	return false;

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
delete semscope;
n1->getData()->setSem(0);

RFASem *semname;
if (!(semname = (RFASem *) n2->getData()->getSem()))
	return false;

// Some more checking.  May also want to check on node names.
// (ie, are they _LIT and _LIST.  If not, fail.)
if (semname->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[scopefncallstart: Bad semantic type.]") << ends;
	return errOut(&gerrStr,false);
	}

_TCHAR *name;
if (!(name = semname->getName()))
	return false;

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
delete semname;
Pn *pn = n2->getData();														// 05/17/01 AM.
pn->setSem(0);
long line = pn->getLine();	// Funcall's line number.				// 05/17/01 AM.

RFASem *rfasem;
Iaction *action;				// Object to build.
action = new Iaction(name, iargs,line);								// 05/17/01 AM.
action->setScope(scope);	// ADD SCOPE NAME TO FN CALL OBJECT.
rfasem = new RFASem(action);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFBFNCALLSTART
* CR:		02/16/00 AM.
* SUBJ:	Create a function call (action) semantic object.
* RET:	True if ok, else false.
* RULE:	_FNCALLLIST [base] <- _LIT [layer=(_FNNAME)] \( @@
* FORM:	fncallstart()
* NOTE:	Part of overhaul to get NLP++ exprs as function arguments.
* OPT:	Not processing args, as an optimization.
*			Excise the children also.
********************************************/

bool PostRFA::postRFBfncallstart(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
if (args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[fncallstart: Takes zero arguments.]") << ends;
	return errOut(&gerrStr,false);
	}

Node<Pn> *coll1 = nlppp->collect_->getRoot();
Node<Pn> *nname = coll1->Down();
Pn *pn = nname->getData();													// 05/17/01 AM.
long line = pn->getLine();	// Funcall's line number.				// 05/17/01 AM.

////////////////////////
// BUILD AND ATTACH ACTION
////////////////////////

// Get semantics from the kiddies.
RFASem *semname;
if (!(semname = (RFASem *) pn->getSem()))								// 05/17/01 AM.
	return false;

// Some more checking.  May also want to check on node names.
// (ie, are they _LIT and _LIST.  If not, fail.)
if (semname->getType() != RSNAME)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[fncallstart: Bad semantic type.]") << ends;
	return errOut(&gerrStr,false);
	}

_TCHAR *name;
Dlist<Iarg> *iargs = 0;
if (!(name = semname->getName()))
	return false;

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
delete semname;
pn->setSem(0);																	// 05/17/01 AM.

RFASem *rfasem;
Iaction *action;				// Object to build.
action = new Iaction(name, iargs, line);								// 05/17/01 AM.
rfasem = new RFASem(action);

nlppp->sem_ = rfasem;
return true;
}


/********************************************
* FN:		POSTRFAADDARG
* CR:		02/16/00 AM.
* SUBJ:	Add arg to a semantic object for function call.
* RET:	True if ok, else false.
* RULE:	@POST
*				addarg(1,3,4)
*				single()
*			@RULES
*			_xNIL <- _FNCALLLIST \,  & [opt] _EXPR @@
* FORM:	postRFAaddarg(1,3, 4) -- 1 - fncall+list. 3-ampersand, 4- expr.
* NOTE:	The fncall may or may not have an arg list.  Need to check.
*			05/26/02 AM. Adding &, call by reference.
* ASS:	REQUIRES LISTADD reduction mode in subsequent action.
********************************************/

bool PostRFA::postRFAaddarg(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *nfn = 0, *nexpr = 0;
Node<Pn> *namp = 0;															// 05/26/02 AM.

//if (!args_0to2("addarg",args,nlppp->collect_,nlppp->sem_, /*DU*/ nfn,nexpr))
if (!args_0to3(_T("addarg"),args,nlppp->collect_,nlppp->sem_,		// 05/26/02 AM.
					/*DU*/ nfn, namp, nexpr))								// 05/26/02 AM.
	return false;

// Get semantics from the kiddies.
Pn *pnfn = nfn->getData();
Pn *pnexpr = nexpr->getData();
RFASem *semfn = 0, *semexpr = 0;
Slist<Iexpr> *fn = 0;
Iexpr *expr = 0;
if (!(semfn = (RFASem *) pnfn->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA addarg action: No semantics in fn.]") << ends;
	return errOut(&gerrStr,false);
	}
if (!(semexpr = (RFASem *) pnexpr->getSem()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA addarg action: No semantics in expr.]") << ends;
	return errOut(&gerrStr,false);
	}

if (semfn->getType() != RSACTION)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[RFA addarg action: Bad sem for fn.]") << ends;
	return errOut(&gerrStr,false);
	}

Iaction *action = semfn->getAction();
Dlist<Iarg> *iargs = action->getArgs();

// If it's a simple type, get rid of the semantic wrapper.
_TCHAR *str;
long num;
float fnum;																		// 08/19/01 AM.
Iarg *iarg = 0;
switch (semexpr->getType())
	{
	case RSEXPR:
	case RSACTION:
	case RSVAR:
	case RSPNODE:						// Parse tree node type!		// 10/18/00 AM.
		if (namp)						// If call by reference.		// 05/26/02 AM.
			iarg = new Iarg(IAREF);	// Special type.					// 05/26/02 AM.
		else																		// 05/26/02 AM.
			iarg = new Iarg(IASEM);
		iarg->setSem(semexpr);
		break;
	case RSSTR:
		str = semexpr->getName();
		iarg = new Iarg(str);
		delete semexpr;
		semexpr = 0;
		break;
	case RSLONG:
		num = semexpr->getLong();
		iarg = new Iarg(num);
		delete semexpr;
		semexpr = 0;
		break;
	case RSFLOAT:																// 08/19/01 AM.
		fnum = semexpr->getFloat();										// 08/19/01 AM.
		iarg = new Iarg(fnum);												// 08/19/01 AM.
		delete semexpr;														// 08/19/01 AM.
		semexpr = 0;															// 08/19/01 AM.
		break;																	// 08/19/01 AM.
	case RSNUM:																	// 02/17/00 AM.
		str = semexpr->getNum();
		if (!str_to_long(str, /*UP*/ num))
			{
			_t_strstream gerrStr;
			gerrStr << _T("[addarg: Couldn't convert to num.]") << ends;
			return errOut(&gerrStr,false);
			}
		iarg = new Iarg(num);
		delete semexpr;
		semexpr = 0;
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[addarg: Unhandled arg type.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// DON'T ZERO THIS OUT.
// Just keep collecting args in this object.
//pnfn->setSem(0);

pnexpr->setSem(0);			// Zero out the source.

if (!iargs)						// First arg for this fn/action.
	{
	// Create and attach empty list.
	iargs = new Dlist<Iarg>();
	action->setArgs(iargs);
	action->setOrigargs(iargs);											// 03/13/02 AM.
	}

// Add current arg to args list.
iargs->rpush(iarg);

// Don't need this, since not creating a suggested node.
//nlppp->sem_ = semfn;
nlppp->sem_ = 0;
return true;
}


/********************************************
* FN:		POSTRFAVARFN
* CR:		02/16/00 AM.
* SUBJ:	Convert function call to variable.
* RET:	True if fn executed fine, false if internal error.
* RULE:	_VAR [layer=(_EXPR)] <- _VARLIST \) @@
* FORM:	postRFAvarfn()
* NOTE:	
********************************************/

bool PostRFA::postRFAvarfn(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
if (args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfn: Takes zero args.]") << ends;
	return errOut(&gerrStr,false);
	}

bool ok = true;
Node<Pn> *coll1 = nlppp->collect_->getRoot();
Node<Pn> *node = coll1->Down();

RFASem *semfn;
Pn *pn = node->getData();
if (!(semfn = (RFASem *) pn->getSem()))
	return false;

if (semfn->getType() != RSACTION)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfn: Bad semantic type.]") << ends;
	return errOut(&gerrStr,false);
	}

// Unpack and check on the function and args.
long ord = 0;
Iaction *action = semfn->getAction();
_TCHAR *typestr = action->getName();			// Var type=G,X,N,S...
Dlist<Iarg> *iargs = action->getArgs();
if (!iargs)																		// 02/21/00 AM.
	{
	if (ok = postRFApnode(typestr,ord,0,false,nlppp))				// 10/18/00 AM.
		{
		pn->setSem(0);			// Clear old sem.							// 10/18/00 AM.
		delete semfn;			// Delete old sem.						// 10/18/00 AM.
		}
	return ok;																	// 10/18/00 AM.
	}
Delt<Iarg> *darg = iargs->getFirst();
Iarg *iarg;
if (!darg)
	{
	if (ok = postRFApnode(typestr,ord,0,false,nlppp))				// 10/18/00 AM.
		{
		pn->setSem(0);			// Clear old sem.							// 10/18/00 AM.
		delete semfn;			// Delete old sem.						// 10/18/00 AM.
		}
	return ok;																	// 10/18/00 AM.
	}

// IMPLEMENTING PNODE.  eg, n(1) returns a node.					// 10/18/00 AM.
// First arg is var name.
_TCHAR *name=0;																	// 05/26/01 AM.
iarg = darg->getData();
switch (iarg->getType())													// 10/18/00 AM.
	{
	case IANUM:					// The PNODE!								// 10/18/00 AM.
		// HANDLE THE PNODE HERE.											// 10/18/00 AM.
		ord = iarg->getNum();												// 10/18/00 AM.
		darg = darg->Right();	// Check if next arg.				// 10/18/00 AM.
		if (ok = postRFApnode(typestr,ord,darg,true,nlppp))		// 10/18/00 AM.
			{
			pn->setSem(0);			// Clear old sem.						// 10/18/00 AM.
			delete semfn;			// Delete old sem.					// 10/18/00 AM.
			}
		return ok;																// 10/18/00 AM.
	case IASTR:					// The good old stuff.					// 10/18/00 AM.
		name = iarg->getStr();	// Moved here.							// 05/26/01 AM.
		break;
	default:																		// 10/18/00 AM.
		{
		nlppp->parse_->inputline_											// 08/24/02 AM.
					= pn->getLine();											// 05/26/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[Error: 1st arg of var must be num or str.]")	// 05/26/01 AM.
				  << ends;														// 05/26/01 AM.
		errOut(&gerrStr,false,nlppp->parse_->getInputpass());					// 05/26/01 AM.
		// REPAIR ERROR, SO PARSE CAN CONTINUE!						// 05/26/01 AM.
		// For the purpose of finding further errors.)				// 05/26/01 AM.
		nlppp->parse_->setIntern(false);	// Syntax error found.	// 05/26/01 AM.
		name = _T("$ERR");			// REPAIR. DUMMY NAME.					// 05/26/01 AM.
		}
		break;																	// 05/26/01 AM.
	}

darg = darg->Right();
// Second arg is ord, if any.
if (darg)
	{
	iarg = darg->getData();
	if (iarg->getType() != IANUM)
		{
		nlppp->parse_->inputline_											// 08/24/02 AM.
					= pn->getLine();											// 05/26/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[varfn: Second arg must be a number.]") << ends;
		nlppp->parse_->setIntern(false);	// Syntax error found.	// 05/26/01 AM.
//		return errOut(false,nlppp->parse_->getInputpass());		// 05/26/01 AM.
		return 
			nlppp->parse_->errOut(&gerrStr,false,true);							// 11/18/02 AM.
		}
	ord = iarg->getNum();
	}

// No third arg allowed.
if (darg && darg->Right())
	{
	{
	nlppp->parse_->inputline_												// 08/24/02 AM.
				= pn->getLine();												// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfn: Only 2 args allowed in var.]") << ends;
	nlppp->parse_->setIntern(false);	// Syntax error found.		// 05/26/01 AM.
	return errOut(&gerrStr,false,nlppp->parse_->getInputpass());									// 05/26/01 AM.
	}

	return false;
	}

enum Ivartype typ;
typ = Ivar::Vartype(*typestr);		// Use first letter of typestring.
if (typ == NULLVAR)
	{
	nlppp->parse_->setIntern(false);	// Syntax error found.	// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfn: Bad variable name=") << typestr << _T("]") << ends;
	return errOut(&gerrStr,false);
	}
// Should check if variable type is compatible with numeric 2nd arg....

pn->setSem(0);			// Clear old sem.
delete semfn;			// Delete old sem.

Ivar *var;				// Object to build.
var = new Ivar(typ, name, ord);
RFASem *semvar = new RFASem(var);
nlppp->sem_ = semvar;
return true;
}


/********************************************
* FN:		POSTRFAPNODE
* CR:		10/18/00 AM.
* SUBJ:	Handle reference to a parse tree node.
* RET:	True if ok, else false.
********************************************/

bool PostRFA::postRFApnode(
	_TCHAR *typestr,					// Var type= G,X,N,S.
	long ord,						// Ordinality of node (for N or X).
	Delt<Iarg> *darg,				// Check on extra args.
	bool numflag,					// If numeric arg was present.
	Nlppp *nlppp
	)
{
if (!typestr || ord < 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[pnode: Error.]") << ends;
	return errOut(&gerrStr,false);
	}
if (*typestr == 'G')
	{
	nlppp->parse_->setIntern(false);	// Syntax error found.	// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[pnode: G() doesn't eval to pnode.]") << ends;
	return errOut(&gerrStr,false);
	}
if (darg)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[pnode: Too many args.]") << ends;
	return errOut(&gerrStr,false);
	}

enum Ipnodetype typ;
typ = Ipnode::Ipnodetype(*typestr);		// Use first letter of typestring.
switch (typ)
	{
	case SUGGESTEDPNODE:
		if (numflag && !ord)
			{
			nlppp->parse_->setIntern(false);	// Syntax error.	// 05/26/01 AM.
			_t_strstream gerrStr;
			gerrStr << _T("[pnode: Ignoring 0 in S(0)]") << ends;
			errOut(&gerrStr,false);
			}

		else if (numflag)
			{
			nlppp->parse_->setIntern(false);	// Syntax error.	// 05/26/01 AM.
			_t_strstream gerrStr;
			gerrStr << _T("[pnode: S() takes no arg.]") << ends;
			return errOut(&gerrStr,false);
			}
		break;
	case CONTEXTPNODE:
	case NODEPNODE:
	case RUNNODEPNODE:
		// Allow N() and X() as equivalent to N(0) and X(0).
		break;
	case NULLPNODE:
	default:
		{
		nlppp->parse_->setIntern(false);	// Syntax error.			// 05/26/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[pnode: Bad var name=") << typestr << _T("]") << ends;
		return errOut(&gerrStr,false);
		}
	}

Ipnode *pnode;				// Object to build.
pnode = new Ipnode(typ, ord);
RFASem *sempnode = new RFASem(pnode);
nlppp->sem_ = sempnode;
return true;
}


/********************************************
* FN:		POSTRFAVARFNARRAY
* CR:		10/13/00 AM.
* SUBJ:	Convert function call to variable with array index.
* RET:	True if ok, else false.
* RULE:	_VAR [layer=(_EXPR)] <- _VARLIST \) \[ _EXPR \] @@
* FORM:	varfnarray(var_str, expr_sem)
* NOTE:	
********************************************/

bool PostRFA::postRFAvarfnarray(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
if (!args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Two args required.]") << ends;
	return errOut(&gerrStr,false);
	}
if (nlppp->sem_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Semantic object already built.]") << ends;
	return errOut(&gerrStr,false);
	}


Iarg *arg1;
Iarg *arg2;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Missing second argument.]") << ends;
	return errOut(&gerrStr,false);
	}
arg2 = args->getData();

if (args->Right())
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: More than two args given.]") << ends;
	return errOut(&gerrStr,false);
	}

long num1;
switch (arg1->getType())
	{
	case IANUM:
		num1 = arg1->getNum();
		break;
	case IASTR:
		// Convert strings to numbers.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		nlppp->parse_->setIntern(false);	// Syntax error.			// 05/26/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[varfnarray: Arg1 must be num.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

long num2;
switch (arg2->getType())
	{
	case IANUM:
		num2 = arg2->getNum();
		break;
	case IASTR:
		// Convert strings to numbers.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		nlppp->parse_->setIntern(false);	// Syntax error.			// 05/26/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[varfnarray: Arg2 must be num.]") << ends;
		return errOut(&gerrStr,false);
		}
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;
num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// num1 is collect for key name (_nonlit or _lit guy).
// num2 is distance to val.
Node<Pn> *colls;
Node<Pn> *coll1, *coll2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;
while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		break;
	}
coll2 = colls;

// Get needed stuff from each collect.
Node<Pn> *nfn, *nindex;
if (!(nfn = coll1->Down()))
	return false;

nindex = coll2->Down();
bool multiarray=false;														// 10/20/00 AM.
if (!nindex)																	// 10/20/00 AM.
	multiarray = true;														// 10/20/00 AM.

RFASem *semfn;
Pn *pn = nfn->getData();
if (!(semfn = (RFASem *) pn->getSem()))
	return false;

if (semfn->getType() != RSACTION)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Bad semantic type.]") << ends;
	return errOut(&gerrStr,false);
	}

// GET INDEX.
RFASem *semindex;
if (nindex)																		// 10/20/00 AM.
	{
	Pn *pnindex = nindex->getData();
	if (!(semindex = (RFASem *) pnindex->getSem()))
		return false;
	pnindex->setSem(0);	// Grab this sem to carry on.

	// If index is string type try to convert to long.				// 09/09/01 AM.
	// This way things like assign compile right.					// 09/09/01 AM.
	long numb = 0;																// 09/09/01 AM.
	switch (semindex->getType())											// 09/09/01 AM.
		{
		case RSSTR:																// 09/09/01 AM.
		case RSNAME:															// 09/09/01 AM.
		case RSNUM:		// old style str-as-num.						// 09/09/01 AM.
			if (!str_to_long(semindex->getName(), /*UP*/numb))		// 09/09/01 AM.
				return false;
			// Change type of sem. Assumes str was interned.		// 09/09/01 AM.
			semindex->setType(RSLONG);										// 09/09/01 AM.
			semindex->setLong(numb);										// 09/09/01 AM.
			break;
		default:																	// 09/09/01 AM.
			break;
		}
	}
else																				// 10/20/00 AM.
	semindex = new RFASem((long) MULTIARRAY);							// 10/20/00 AM.

// Unpack and check on the function and args.
Iaction *action = semfn->getAction();
_TCHAR *typestr = action->getName();			// Var type=G,X,N,S...
Dlist<Iarg> *iargs = action->getArgs();
if (!iargs)
	{
	nlppp->parse_->setIntern(false);	// Syntax error.				// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Var has no args.]") << ends;
	return errOut(&gerrStr,false);
	}

Delt<Iarg> *darg = iargs->getFirst();
Iarg *iarg;
if (!darg)
	{
	nlppp->parse_->setIntern(false);	// Syntax error.				// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Var has no args.]") << ends;
	return errOut(&gerrStr,false);
	}

// First arg is var name.
iarg = darg->getData();
if (iarg->getType() != IASTR)
	{
	nlppp->parse_->setIntern(false);	// Syntax error.				// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: First arg of var must be a string.]") << ends;
	return errOut(&gerrStr,false);
	}

_TCHAR *name = iarg->getStr();

darg = darg->Right();
// Second arg is ord, if any.
long ord = 0;
if (darg)
	{
	iarg = darg->getData();
	if (iarg->getType() != IANUM)
		{
		nlppp->parse_->setIntern(false);	// Syntax error.			// 05/26/01 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[varfnarray: Second arg must be a number.]") << ends;
		return errOut(&gerrStr,false);
		}

	ord = iarg->getNum();
	}

// No third arg allowed.
if (darg && darg->Right())
	{
	nlppp->parse_->setIntern(false);	// Syntax error.				// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Only 2 args allowed in var.]") << ends;
	return errOut(&gerrStr,false);
	}

enum Ivartype typ;
typ = Ivar::Vartype(*typestr);		// Use first letter of typestring.
if (typ == NULLVAR)
	{
	nlppp->parse_->setIntern(false);	// Syntax error.				// 05/26/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[varfnarray: Bad variable name=") << typestr << _T("]") << ends;
	return errOut(&gerrStr,false);
	}

// Should check if variable type is compatible with numeric 2nd arg....

pn->setSem(0);			// Clear old sem.
delete semfn;			// Delete old sem.

Ivar *var;				// Object to build.
var = new Ivar(typ, name, ord,
	semindex);			// NOW IVAR GETS AN INDEX!!					// 10/13/00 AM.
RFASem *semvar = new RFASem(var);
nlppp->sem_ = semvar;
return true;
}



/********************************************
* FN:		POSTRFAPREACTION
* CR:		02/17/00 AM.
* SUBJ:	Create a pre action semantic object for RFB.
* RET:	True if ok, else false.
* RULE:	_ACTION [base] <- _PREPAIR _FNCALL [s] \; [s opt] @@
* FORM:	postRFApreaction()
* OPT:	Not parsing arguments, to save time.  Assuming
*			prepair == 1, fncall == 2.
********************************************/

bool PostRFA::postRFApreaction(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
// Get pre node == first.
Node<Pn> *coll = nlppp->collect_->getRoot();
Node<Pn> *npre = coll->Down();

// Get fncall node == second.
coll = coll->Right();
Node<Pn> *nfn = coll->Down();

// Get semantics from the kiddies.
RFASem *sempre = (RFASem *) npre->getData()->getSem();
RFASem *semfn  = (RFASem *) nfn->getData()->getSem();

// Get contents of semantic wrappers.
Dlist<Iarg> *prepair = sempre->getArgs();
Iaction *action = semfn->getAction();

// Get start and end range.
Dlist<Iarg> *iargs = 0;
long start, end;
Delt<Iarg> *ptr;
ptr = prepair->getFirst();
// Unpack the start and end args.
if (!Arg::num1(_T("preaction"), (DELTS*&)ptr, start))
	return false;
if (!Arg::num1(_T("preaction"), (DELTS*&)ptr, end))
	return false;

// Get action name and args.
_TCHAR *name = action->getName();
iargs = action->getArgs();			// REUSING iargs VARIABLE.

// Clear and delete the old semantic objects.
action->setArgs(0);		// Clear so it won't be deleted.  Reusing fn args.
action->setOrigargs(0);	// Clear.										// 03/13/02 AM.
delete sempre;
delete semfn;
npre->getData()->setSem(0);	// Clear from parse tree.
nfn->getData()->setSem(0);		// Clear from parse tree.

RFASem *rfasem;
Ipre *pre;
pre = new Ipre(name, iargs, start, end);
rfasem = new RFASem(pre);

nlppp->sem_ = rfasem;
return true;
}




/********************************************
* FN:		POSTRFARULEMARK
* CR:		08/31/00 AM.
* SUBJ:	Record if @RULES seen in file.
* RET:	True if ok, else false.
* RULE:	_soRULES <- \@ RULES @@
* FORM:	postRFArulemark()
* NOTE:	In order to not warn the user about "no rules in pat file"
*			when there isn't even a @RULES marker in the file.
********************************************/

bool PostRFA::postRFArulemark(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
if (args)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rfarulemark: Error. Requires zero args.]") << ends;
	return errOut(&gerrStr,false);
	}
if (!nlppp)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rfarulemark: Error. No global parse structure.]") << ends;
	return errOut(&gerrStr,false);
	}

// Add or increment a global var that tracks if @RULES has been seen.
return Var::inc(_T("RFA rulemark"), nlppp->getParse());
}




/********************************************
* FN:		POSTRFBUNMATCHEDPUNCT
* CR:		05/19/01 AM.
* SUBJ:	Report on unmatched punctuation char.
* RET:	True if ok, else false.
* RULE:	_xNIL <- _xWILD [one match=( \( \) \[ \] .....)] @@
* FORM:	rfbunmatchedpunct(num)
*				num = ord of unmatched punctuation.
* NOTE:	One of the first syntax-error catching functions!!
********************************************/

bool PostRFA::postRFBunmatchedpunct(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("rfbunmatchedpunct"), args, nlppp->collect_, nlppp->sem_,
																	/*DU*/ nstart, nend))
	return false;

if (nstart != nend)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rfbunmatchedpunct: Internal error. Multinode match.]")
			<< ends;
	return errOut(&gerrStr,false);
	}

Pn *pn = nstart->getData();
if (!pn)
	return false;

if (pn->getSem())
	return false;

_TCHAR *name = pn->getName();
if (!name || !*name)			// No token name.
	return false;

if (*(name+1))					// Need single-char match.
	return false;

_TCHAR *cname = _T("");
switch (*name)
	{
	case '(':	cname = _T("left paren");				break;
	case ')':	cname = _T("right paren");				break;
	case '[':	cname = _T("left square bracket");	break;
	case ']':	cname = _T("right square bracket");	break;
	case '<':	cname = _T("left angle bracket");		break;
	case '>':	cname = _T("right angle bracket");	break;
	case '{':	cname = _T("left brace");				break;
	case '}':	cname = _T("right brace");				break;
	default:
		return false;	// Bad char match in RFB rule.
	}

// REPORT SYNTAX ERROR IN INPUT PASS FILE.
nlppp->parse_->setIntern(false);	// Syntax error.					// 05/26/01 AM.
nlppp->parse_->inputline_													// 08/24/02 AM.
	= pn->getLine();			// Line number in INPUT pass file.
//sprintf(Errbuf, "[Error: Unmatched %s.]", cname);				// 05/29/01 AM.
_t_strstream gerrStr;							// 05/29/01 AM.
gerrStr << _T("[Syntax error at '") << *name << _T("'.]") << ends;		// 05/29/01 AM.
return errOut(&gerrStr,true,nlppp->parse_->getInputpass());
														// Pass num of INPUT pass file.
}


/********************************************
* FN:		POSTRFBERROR
* CR:		05/29/01 AM.
* SUBJ:	Report on extra crud in pass file.
* RET:	True if ok, else false.
* RULE:	eg,
*			_xNIL <- _RULESFILE _xANY @@
* FORM:	rfberror(num)
*				num = ord of extra crud.
* NOTE:	A syntax-error catching function.
********************************************/

bool PostRFA::postRFBerror(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;
Parse *parse = nlppp->parse_;

if (!args_1(_T("rfberror"), args, nlppp->collect_, nlppp->sem_,
																	/*DU*/ nstart, nend))
	return false;

if (!nstart)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rfbunmatchedpunct: Internal error. Must match a node.]")
			<< ends;
	return errOut(&gerrStr,false);
	}

Pn *pn = nstart->getData();
if (!pn)
	return false;

// REPORT SYNTAX ERROR IN INPUT PASS FILE.
parse->setIntern(false);	// Syntax error.
parse->inputline_													// 08/24/02 AM.
	= pn->getLine();			// Line number in INPUT pass file.
_t_strstream gerrStr;
gerrStr << _T("[Syntax error.]") << ends;
return parse->errOut(&gerrStr,true,true);
											// Pass num of INPUT pass file.
}


/********************************************
* FN:		POSTRFBDECL
* CR:		12/19/01 AM.
* SUBJ:	Handle a function declaration.
* NOTE:	
* RET:	True if ok, else false.
* RULE:	_FUNDEF <- _FNCALL _BLOCK @@
* FORM:	postRFBdecl("1", "2");
********************************************/

bool PostRFA::postRFBdecl(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *nfn=0, *nblock=0;

if (!args_1or2(_T("rfbdecl"), args, nlppp->collect_, nlppp->sem_,
												/*DU*/ nfn, nblock))
	return false;

if (!nfn || !nblock)
	return false;

////////////////////////
// BUILD AND ATTACH
////////////////////////

// Get semantics from the kiddies.
RFASem *semfn, *semblock;
if (!(semfn = (RFASem *) nfn->getData()->getSem()))
	return false;
if (!(semblock = (RFASem *) nblock->getData()->getSem()))
	return false;

// Unpack the function call and block.
Iaction *fncall=0;

if (semfn->getType() != RSACTION)
	return false;
if (!(fncall = semfn->getAction()))
	return false;

_TCHAR *name = fncall->getName();
if (!name || !*name)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rfbdecl: Error. No function name.]") << ends;
	return errOut(&gerrStr,false);
	}

Dlist<Iarg> *params = 0;	// Placeholder for parameters.
Dlist<Iarg> *pargs=0;														// 03/09/02 AM.
if (pargs = fncall->getArgs())
	{
	// CHECK ON AND CONDENSE PARAMS PROGRAMMATICALLY.				// 03/09/02 AM.
	if (!rfbParams(pargs,nlppp,/*UP*/params))							// 03/09/02 AM.
		{
		_t_strstream gerrStr;					// 03/09/02 AM.
		gerrStr << _T("[rfbdecl: Error in function parameters.]")		// 03/09/02 AM.
				<< ends;															// 03/09/02 AM.
		return errOut(&gerrStr,false);												// 03/09/02 AM.
		}
	}

// Delete the old semantic objects.
// Clear semantic object data so it won't be deleted.
//semfn->setAction(0);	// All we needed was function name.
delete semfn;
nfn->getData()->setSem(0);

// Keeping the sem for the function body.
//delete semblock;
nblock->getData()->setSem(0);

// Get pass line number from block.
Pn *pn = nfn->getData();
long linenum = 0;
linenum = nlppp->parse_->inputline_ = pn->getLine();

long passnum = nlppp->parse_->getInputpass();						// 12/21/01 AM.

Ifunc *ifunc = new Ifunc(name, params, semblock, linenum, passnum);
RFASem *rfasem = new RFASem(ifunc);
nlppp->sem_ = rfasem;

return true;
}


/********************************************
* FN:		RFBPARAMS
* CR:		03/09/02 AM.
* SUBJ:	Programmatically "parse" parameters for user-defined function.
* NOTE:	Internal function for postRFBdecl.
* RET:	True if no errors, else false.
*			UP params = reworked arglist of params.  Looks like list of
*			string args, each one representing an L() variable.
********************************************/

bool PostRFA::rfbParams(
	Dlist<Iarg> *args,	// Raw expressions that should be L() vars.
	Nlppp *nlppp,
	/*UP*/
	Dlist<Iarg>* &params	// Arg list of L() var names.
	)
{
params = 0;
if (!args)
	return true;

params = new Dlist<Iarg>();			// Make empty list.

Delt<Iarg> *darg = args->getFirst();
Iarg *arg;
RFASem *sem;
Ivar *var;
_TCHAR *varname;
Iarg *tmp;

for (; darg; darg = darg->Right())	// Process each arg.
	{
	arg = darg->getData();
	if (arg->getType() != IASEM)
		{
		Dlist<Iarg>::DeleteDlistAndData(params);
		return false;
		}
	sem = arg->getSem();
	if (sem->getType() != RSVAR)
		{
		Dlist<Iarg>::DeleteDlistAndData(params);
		return false;
		}
	var = sem->getVar();
	if (var->getType() != LOCALVAR)
		{
		Dlist<Iarg>::DeleteDlistAndData(params);
		return false;
		}
	// Finally, assured of having L("varname").
	varname = var->getName();
	tmp = new Iarg(varname);
	params->rpush(tmp);	// Add varname to new parameter list.
	}

return true;
}

/********************************************
* FN:		POSTRFBDECLS
* CR:		12/19/01 AM.
* SUBJ:	Handle function declaration list.
* NOTE:	
* RET:	True if ok, else false.
* RULE:	_DECL <- _FNDEF [plus] @@
* FORM:	postRFBdecls("1");
********************************************/

bool PostRFA::postRFBdecls(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *nstart, *nend;
nstart = nend = 0;

if (!args_1(_T("rfbdecls"), args, nlppp->collect_, nlppp->sem_, /*DU*/ nstart, nend))
	return false;

if (!nstart)
	return true;

////////////////////////
// BUILD DECLS LIST
////////////////////////
// Traverse collect elements, gathering a list of decls.
Dlist<Iarg> *decls = new Dlist<Iarg>();			// Empty list.
Node<Pn> *bound;
if (nend->Right())
	bound = nend->Right();
else
	bound = 0;
RFASem *semdecl;
Pn *pn;
Iarg *tmp;
for (; nstart != bound; nstart = nstart->Right())
	{
	// Get semantics of action, copy, and glom onto list.
	pn = nstart->getData();
	assert(pn);
	semdecl = (RFASem *) pn->getSem();
	if (!semdecl || (semdecl->getType() != RSFUNC))
		{
		_t_strstream gerrStr;
		gerrStr << _T("[RFB decls action: Non-function in @DECL.]") << ends;
		errOut(&gerrStr,false);

		//delete iactions;
		Dlist<Iarg>::DeleteDlistAndData(decls);
		return false;
		}
	pn->setSem(0);
	tmp = new Iarg(semdecl);
	decls->rpush(tmp);					// Create new elt.
	}

nlppp->sem_ = new RFASem(decls);
return true;
}

