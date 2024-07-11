/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	VAR.CPP
* FILE:	lite\var.cpp
* CR:		12/07/98 AM.
* SUBJ:	Var management for parse instance.
* NOTE:	For now, just a list of Ipair objects.  As needed, may convert
*			to a hashed scheme.  For the SIC code domain, will probably need
*			the hashing eventually.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"		// 03/09/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "lite/Auser.h"		// 07/07/03 AM.
#include "inline.h"															// 09/26/01 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "string.h"
#include "tree.h"	// 02/26/01 AM.
#ifdef LINUX
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#endif

#include "arg.h"
#include "parse.h"
#include "iaction.h"
#include "ifile.h"
#include "rfasem.h"															// 09/26/00 AM.

#include "var.h"
#include "kbm/con_s.h"

extern char BOM_UTF8[];

/********************************************
* FN:		Special functions for the class
********************************************/

Var::Var()
{
*gout << _T("[DON'T CREATE OBJECTS OF Var CLASS.]") << std::endl;
}

/*******************************************/

/*******************************************/

Var::~Var()
{
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
* FN:		General Functions
********************************************/

/********************************************
* FN:		Class-wide Functions
********************************************/

/********************************************
* FN:		PARSE-SPECIFIC FUNCTIONS
********************************************/

/********************************************
* FN:		CREATE
* CR:		08/03/99 AM.
* SUBJ:	Create a new dynamic variable.
* NOTE:	Separating parse-specific from generic stuff.
********************************************/

bool Var::create(_TCHAR *name, Iarg *val, Parse *parse
	//, Seqn *seqn															// 01/14/00 AM.
	)
{
Ipair *pairx = 0;			// 11/17/99 AM.

// 02/09/99 AM. Not requiring a value.
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();

bool ok = true;
ok = create(name, val, /*DU*/ dlist,pairx);
parse->setVars(dlist);
//cout << "Vars:" << std::endl;
//cout << *(parse->getVars()) << std::endl;
return ok;
}

/********************************************
* FN:		CREATESTRS
* CR:		01/24/98 AM.
* SUBJ:	Create a new dynamic variable with list of strings as value.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::createstrs(_TCHAR *name, Parse *parse
	//, Seqn *seqn															// 01/14/00 AM.
	)
{
if (!name || !parse)
	return false;

// If variable exists already, error.
// Traverse the list to verify.
// Need a find-in-list function for Ipair objects.
// Here should be a separate Get function.
Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();

bool ok = true;
ok = createstrs(name, /*DU*/ dlist);
parse->setVars(dlist);
//cout << "Vars:" << std::endl;
//cout << *(parse->getVars()) << std::endl;
return ok;
}


/********************************************
* FN:		INC
* CR:		12/08/98 AM.
* SUBJ:	Increment a variable's value.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::inc(_TCHAR *name, Parse *parse)
{
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
ok = inc(name, parse, /*DU*/ dlist);
parse->setVars(dlist);
return true;
}


/********************************************
* FN:		ADDSTRS
* CR:		01/24/99 AM.
* SUBJ:	Add a string to a string-list variable.
* NOTE:	Used for building a list of all words, to do one-shot
*			lexical lookup.
*			08/03/99 AM. Overhaul.
********************************************/

bool Var::addStrs(
	_TCHAR *name,			// Variable name.
	_TCHAR *str,			// String value to add.
	Parse *parse
	)
{
if (!name || !str || !*str || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
ok = addStrs(name, str, /*DU*/ dlist);
parse->setVars(dlist);
return ok;
}


/********************************************
* FN:		SETVAL
* CR:		02/09/99 AM.
* SUBJ:	Set variable's value to an ostream pointer.
********************************************/

bool Var::setVal(
	_TCHAR *name,
	Parse *parse,
	std::_t_ostream *ostr)
{
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
ok = setVal(name, ostr, /*DU*/ dlist);
parse->setVars(dlist);
return ok;
}

/********************************************
* FN:		FIND
* CR:		02/09/99 AM.
* SUBJ:	Find dynamic variable in list.
* RET:	True if ok, else false.
* NOTE:	Value as arg lets us handle string, num, etc.
********************************************/

bool Var::find(
	_TCHAR *name,					// Variable name to find.
	Parse *parse,
	/*DU*/
	Ipair* &pairx				// Pair found or null.
	)
{
pairx = 0;
return (Var::find(name, parse->getVars(), pairx));
}


/********************************************
* FN:		VAL
* CR:		12/08/98 AM.
* SUBJ:	Fetch a variable's value.
********************************************/

bool Var::val(_TCHAR *name, Parse *parse, /*DU*/ _TCHAR *str)
{
str = 0;
if (!name || !parse)
	return false;


Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
_TCHAR buf[MAXSTR];							// 11/03/99 AM.
ok = val(name, dlist, /*DU*/ buf);	// 11/03/99 AM.
parse->internStr(buf, /*UP*/ str);	// 11/03/99 AM.
return ok;
}


/********************************************
* FN:		VAL
* CR:		02/09/99 AM.
* SUBJ:	Fetch a variable's ostream pointer value.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::val(_TCHAR *name, Parse *parse, /*DU*/ std::_t_ostream* &ostr)
{
ostr = 0;
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
ok = val(name, dlist, /*DU*/ ostr);
return ok;
}


/********************************************
* FN:		VAL
* CR:		08/31/00 AM.
* SUBJ:	Fetch a variable's num value.
********************************************/

bool Var::val(_TCHAR *name, Parse *parse, /*DU*/ long long &num)
{
num = 0;
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
return val(name, dlist, /*DU*/ num);
}


/********************************************
* FN:		VALS
* CR:		01/25/98 AM.
* SUBJ:	Fetch handle for a variable's values.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::vals(_TCHAR *name, Parse *parse, /*DU*/ Delt<Iarg>* &values)
{
values = 0;
Dlist<Ipair> *dlist;			// List of variables.
dlist = parse->getVars();
bool ok = true;
ok = vals(name, dlist, /*DU*/ values);
return ok;
}

/********************************************
* FN:		VALS
* CR:		01/25/98 AM.
* SUBJ:	Fetch handle for a variable's values.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::vals(_TCHAR *name, Parse *parse, /*DU*/ Dlist<Iarg>* &list)
{
list = 0;
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = parse->getVars();
bool ok = true;
ok = vals(name, dlist, /*DU*/ list);
return ok;
}


/********************************************
* FN:		SORTVALS
* CR:		01/25/98 AM.
* SUBJ:	Sort values of a variable alphabetically.
* NOTE:	Redundant values retained.
*			Another function could delete those if desired.
*			08/03/99 AM. Overhaul.
********************************************/


bool Var::sortVals(_TCHAR *name, Parse *parse)
{
if (!name || !parse)
	return false;
Dlist<Ipair> *dlist = 0;
dlist = parse->getVars();
bool ok = true;
ok = sortVals(name, dlist);
return ok;
}





/********************************************
* FN:		SEMANTIC VARIABLE FUNCTIONS
********************************************/

/********************************************
* FN:		CREATE
* CR:		12/07/98 AM.
* SUBJ:	Create a new dynamic variable.
* NOTE:	Value as arg lets us handle string, num, etc.
*			08/03/99 AM. Now independent of Parse object.
********************************************/

bool Var::create(_TCHAR *name, Iarg *val,
	/*DU*/
	Dlist<Ipair>* &dlist,
	Ipair* &pairx							// 11/17/99 AM.
	)
{
pairx = 0;			// 11/17/99 AM.

// 02/09/99 AM. Not requiring a value.
if (!name)
	return false;

// If variable exists already, error.
// Traverse the list to verify.
// Need a find-in-list function for Ipair objects.
// Here should be a separate Get function.
Delt<Ipair> *dpair = 0;						// 11/17/99 AM.
Var::find(name, dlist, /*DU*/ pairx);
if (pairx)			// In list already.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::create: Variable=") << name << _T("already declared.]")
		  << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Dlist<Iarg> *args;
if (val)							// 02/09/99 AM.
	{
	Iarg *nval;					// FIX.	// 07/27/99 AM.
	nval = new Iarg(*val);	// FIX.	// 07/27/99 AM.
	args = new Dlist<Iarg>(nval);
	}
else					// Make an empty list.	//02/09/99 AM.
	args = new Dlist<Iarg>();

// Add variable to front of list.
// Create new pair, copying the Iarg.
if (!dlist)
	{
	// First variable to be placed in list.
	dlist = Ipair::makeDlist(name, args);
	dpair = dlist->getFirst();
	}
else
	{
	// Add to list.
	dpair = Ipair::addDelt(dlist, name, args);
	}

pairx = dpair->getData();	// Return pair.			// 11/17/99 AM.
return true;
}

/********************************************
* FN:		CREATE
* CR:		11/03/99 AM.
* SUBJ:	Create a new dynamic variable.
* NOTE:	VARIANT. Takes a list of values.
********************************************/

bool Var::create(_TCHAR *name, Dlist<Iarg> *vals, /*DU*/ Dlist<Ipair>* &dlist)
{

// 02/09/99 AM. Not requiring a value.
if (!name)
	return false;

// If variable exists already, error.
// Traverse the list to verify.
// Need a find-in-list function for Ipair objects.
// Here should be a separate Get function.
Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (pairx)			// In list already.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::create: Variable=") << name << _T("already declared.]")
		  << std::ends;
	errOut(&gerrStr,false);
	return false;
	}


// Add variable to front of list.
// Create new pair, copying the Iarg.
if (!dlist)
	{
	// First variable to be placed in list.
	dlist = Ipair::makeDlist(name, vals);
	}
else
	{
	// Add to list.
	Ipair::addDelt(dlist, name, vals);
	}

return true;
}

/********************************************
* FN:		CREATESTRS
* CR:		01/24/98 AM.
* SUBJ:	Create a new dynamic variable with list of strings as value.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::createstrs(_TCHAR *name, /*DU*/ Dlist<Ipair>* &dlist)
{
if (!name)
	return false;

// If variable exists already, error.
// Traverse the list to verify.
// Need a find-in-list function for Ipair objects.
// Here should be a separate Get function.
Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (pairx)			// In list already.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::createstrs: Variable=") << name << _T("already declared.]")
		  << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Dlist<Iarg> *args;
args = new Dlist<Iarg>();			// Make empty list of values.

// Add variable to front of list.
// Create new pair, copying the Iarg.
if (!dlist)
	{
	// First variable to be placed in list.
	dlist = Ipair::makeDlist(name, args);
	}
else
	{
	// Add to list.
	Ipair::addDelt(dlist, name, args);
	}

//cout << "Vars:" << std::endl;
//cout << *(parse->getVars()) << std::endl;
return true;
}


/********************************************
* FN:		INC
* CR:		12/08/98 AM.
* SUBJ:	Increment a variable's value.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::inc(_TCHAR *name, Parse *parse, /*DU*/ Dlist<Ipair>* &dlist)
{
if (!name || !parse)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)			// Var doesn't exist yet.
	{
	// *gerr << "[Var::inc: Variable=" << name << " not found.]" << std::endl;
	// If var not found, create it.  If no list, create it.		// 08/31/00 AM.
	Iarg *val;																	// 08/31/00 AM.
	val = new Iarg(1LL);		// Start with +1.					// 08/31/00 AM.
	Var::create(name, val, /*DU*/ dlist,pairx);						// 08/31/00 AM.
	delete val;																	// 08/31/00 AM.
	return true;																// 08/31/00 AM.
	//return false;															// 08/31/00 AM.
		}

Dlist<Iarg> *args;
Delt<Iarg> *darg;
Iarg *arg;
args = pairx->getVals();
assert(args);
darg = args->getFirst();
assert(darg);
arg = darg->getData();
assert(arg);


// Convert to number as needed.								// 11/03/99 AM.
long long num;
switch (arg->getType())											// 11/03/99 AM.
	{
	case IASTR:
		if (!str_to_long(arg->getStr(), num))
			return false;
		arg->setType(IANUM);		// Recast type.			// 11/03/99 AM.
		break;
	case IANUM:
		num = arg->getNum();										// 11/03/99 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Var::inc: Bad value type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}

++num;	// Increment.

// Convert back to string.
//char buf[128];										// 11/03/99 AM
//sprintf(buf, "%d", num);							// 11/03/99 AM.
//sprintf_s(buf, "%d", num);							// 11/03/99 AM.
//char *str = 0;										// 11/03/99 AM.
//str = parse->internStr(buf, /*DU*/ str);	// String table copy.

// Install.
//arg->setStr(str);									// 11/03/99 AM.
arg->setNum(num);										// 11/03/99 AM.

//cout << "str=" << str << std::endl;

return true;
}


/********************************************
* FN:		ADDNUM
* CR:		08/05/99 AM.
* SUBJ:	Add number to variable's value.
* NOTE:	Because these variables are created in a node, there's no
			convenient way to initialize them, so do it here.
			Don't know why increment functions are using IASTR.  Using
			IANUM here.
********************************************/

bool Var::addNum(
	_TCHAR *name,		// Variable name.
	long long numb,		// Number to add to its value.
	Parse *parse,
	/*DU*/
	Dlist<Ipair>* &dlist		// May be empty going down.
	)
{
if (!name || !parse)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)			// Absent from list.
	{
	// *gerr << "[Var::addNum: Creating var=" << name << ".]"
	//	  << std::endl;
	// Give it the initial value of numb.
	Iarg *val;
	val = new Iarg(numb);
	Var::create(name, val, /*DU*/ dlist,pairx);
	delete val;				// FIX.	// 10/10/99 AM.
	return true;
	}

Dlist<Iarg> *args;
Delt<Iarg> *darg;
Iarg *arg;
args = pairx->getVals();
assert(args);
darg = args->getFirst();
assert(darg);
arg = darg->getData();
assert(arg);


// Convert to number.
//if (arg->getType() != IANUM)								// 11/03/99 AM.
//	{
//	*gerr << "[Var::inc: Bad value type.]" << std::endl;
//	return false;
//	}
//long num;
//num = arg->getNum();

// Convert to number as needed.								// 11/03/99 AM.
long long num;
switch (arg->getType())											// 11/03/99 AM.
	{
	case IASTR:
		if (!str_to_long(arg->getStr(), num))
			return false;
		arg->setType(IANUM);		// Recast type.			// 11/03/99 AM.
		break;
	case IANUM:
		num = arg->getNum();										// 11/03/99 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Var::inc: Bad value type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}


num += numb;		// ADD NUMBERS.


// Install.
arg->setNum(num);

return true;
}


/********************************************
* FN:		ADDSTRS
* CR:		01/24/99 AM.
* SUBJ:	Add a string to a string-list variable.
* NOTE:	Used for building a list of all words, to do one-shot
*			lexical lookup.
********************************************/

bool Var::addStrs(
	_TCHAR *name,			// Variable name.
	_TCHAR *str,			// String value to add.
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name || !str || !*str)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)			// In list already.
	{
	// If not present, adding it. OPT: not optimized.				// 06/06/00 AM.
	Var::createstrs(name, /*DU*/ dlist);								// 06/06/00 AM.
	Var::find(name,dlist,/*DU*/ pairx);									// 06/06/00 AM.
	//*gerr << "[Var::addStrs: Variable=" << name << " not found.]"
	//	  << std::endl;
	//return false;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

// Build an arg for the given string.
arg = new Iarg(str);

// Add it to the vals list.
args->rpush(arg);

// Null and delete, since it was copied.
//delete arg;		// 07/02/99 AM.

return true;
}


/********************************************
* FN:		SETVALS
* CR:		11/03/99 AM.
* SUBJ:	Set variable's value to given values.
* NOTE:	Uses the given values list as is.
*			CALLER RESPONSIBLE FOR COPYING!
*			ZAP AND REPLACE previous values, if any.
*			Want to store even numeric values as string, so that I can
*			have LISP-like freedom from strict type casting.
*			IF NO VALS GIVEN, WIPE OUT DESTINATION.
********************************************/

bool Var::setVals(
	_TCHAR *name,
	Dlist<Iarg> *vals,
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	// MAKE VARIABLE
	return create(name, vals, /*DU*/ dlist);
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (args)
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DeleteDlistAndData(args);
	}

pairx->setVals(vals);

return true;
}


/********************************************
* FN:		SETVAL
* CR:		11/15/99 AM.
* SUBJ:	Set variable's value to a string.
* NOTE:	Internal version.
*			REPLACE previous values, if any.
********************************************/

bool Var::setVal(
	Ipair *pairx,
	_TCHAR *str
	)
{
// BUG: EMPTY STRING CLEARS VALUE	// FIX.	// 08/29/14 AM.
//if (!pairx || !str)	// FIX.	// 08/29/14 AM.
if (!pairx)					// FIX.	// 08/29/14 AM.
	return false;

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();

// IF ALREADY EMPTY, DONE.	// FIX.	// 08/29/14 AM.
if (!args && !(str && *str))	// FIX.	// 08/29/14 AM.
  return true;	// FIX. // 08/29/14 AM.

if (str && *str)	// FIX.	// 08/29/14 AM.
  arg = new Iarg(str);		// Build an arg for the given str.

if (!args
	&& (str && *str))	// FIX.	// 08/29/14 AM.
	{
	// Create a list, put value on, and return.
	args = new Dlist<Iarg>(arg);
	pairx->setVals(args);
	return true;
	}

if (args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

if (arg)	// FIX.	// 08/29/14 AM.
  args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		11/15/99 AM.
* SUBJ:	Set variable's value to a num.
* NOTE:	Internal version.
*			REPLACE previous values, if any.
********************************************/

bool Var::setVal(
	Ipair *pairx,
	long long num
	)
{
if (!pairx)
	return false;

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();

arg = new Iarg(num);		// Build an arg for the given num.

if (!args)
	{
	// Create a list, put value on, and return.
	args = new Dlist<Iarg>(arg);
	pairx->setVals(args);
	return true;
	}

if (args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		08/18/01 AM.
* SUBJ:	Set variable's value to a float.
* NOTE:	Internal version.
*			REPLACE previous values, if any.
********************************************/

bool Var::setVal(
	Ipair *pairx,
	float fnum
	)
{
if (!pairx)
	return false;

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();

arg = new Iarg(fnum);		// Build an arg for the given fnum.

if (!args)
	{
	// Create a list, put value on, and return.
	args = new Dlist<Iarg>(arg);
	pairx->setVals(args);
	return true;
	}

if (args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		12/31/99 AM.
* SUBJ:	Set variable's value to an ostream.
* NOTE:	Internal version.
*			REPLACE previous values, if any.
********************************************/

bool Var::setVal(
	Ipair *pairx,
	std::_t_ostream *ostr
	)
{
if (!pairx)
	return false;

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();

arg = new Iarg(ostr);		// Build an arg for the given num.

if (!args)
	{
	// Create a list, put value on, and return.
	args = new Dlist<Iarg>(arg);
	pairx->setVals(args);
	return true;
	}

if (args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		02/22/00 AM.
* SUBJ:	Set variable's value to a sem object.
* NOTE:	Internal version.
*			REPLACE previous values, if any.
*			Used for KB OBJECTS, currently.
********************************************/

bool Var::setVal(
	Ipair *pairx,
	RFASem *sem
	)
{
if (!pairx)
	return false;

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();

// CRASH: Inconsistent handling of sem=0, interp vs compiled runtime. // 09/30/19 AM.
if (sem)	// HACK TO STOP CRASH.	// 09/30/19 AM.
	arg = new Iarg(sem);		// Build an arg for the given sem.
else
	arg = new Iarg(0LL);	// HACK TO STOP CRASH.	// 09/30/19 AM.

if (!args)
	{
	// Create a list, put value on, and return.
	args = new Dlist<Iarg>(arg);
	pairx->setVals(args);
	return true;
	}

if (args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

// if (sem)	// (THIS STOPPED THE CRASH)	// DEBUGGING!!!! // 09/30/19 AM.
args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		08/10/02 AM.
* SUBJ:	Set variable's value to multi-value list.
* NOTE:	To handle multi-valued lists.
*			User must provide a copied, free and clear list.
*			Internal version.
*			REPLACE previous values, if any.
*			Used for KB OBJECTS, currently.
********************************************/

bool Var::setVal(
	Ipair *pairx,
	Dlist<Iarg> *vals	// Assume free and clear list, if any.
	)
{
if (!pairx)
	return false;

Dlist<Iarg> *args = pairx->getVals();

// Remove the old.
if (args)
	Dlist<Iarg>::DeleteDlistAndData(args);

// Replace with the new
pairx->setVals(vals);
return true;
}


/********************************************
* FN:		SETVAL
* CR:		10/31/99 AM.
* SUBJ:	Set variable's value to a string.
* NOTE:	Hard to believe I didn't have this already!
*			REPLACE previous values, if any.
*			Want to store even numeric values as string, so that I can
*			have LISP-like freedom from strict type casting.
********************************************/

bool Var::setVal(
	_TCHAR *name,
	_TCHAR *str,
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH,	// If pushing value front	// 12/12/14 AM.
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
// EMPTY STRING CLEARS VALUES.	// 08/29/14 AM.
// if (!name || !str)	// FIX.	// 08/29/14 AM.
if (!name)	// FIX.	// 08/29/14 AM.
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);

// ALREADY EMPTY AND STR EMPTY.	// FIX. // 08/29/14 AM.
if (!pairx && !(str && *str))	// FIX.	// 08/29/14 AM.
  return true;	// FIX.	// 08/29/14 AM.

if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(str);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	delete aval;													// FIX.	// 06/26/01 AM.
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

if (str && *str)	// FIX.	// 08/29/14 AM.
	{
	arg = new Iarg(str);		// Build an arg for the given str.
	if (bPUSH)
		args->push(arg);		// 12/12/14 AM.
	else
		args->rpush(arg);			// Add it to the vals list.
	}

return true;
}


/********************************************
* FN:		SETVAL
* CR:		02/09/99 AM.
* SUBJ:	Set variable's value to an ostream pointer.
* NOTE:	08/03/99 AM. Overhaul.
*			12/12/14 INCONSISTENT WITH OTHER TYPES.
********************************************/

bool Var::setVal(
	_TCHAR *name,
	std::_t_ostream *ostr,
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name || !ostr)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::setVal: Variable=") << name << _T(" not found.]")
		  << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (args->getFirst())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Variable=") << name << _T(" is already set.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Build an arg for the given ostream ptr.
arg = new Iarg(ostr);

// Add it to the vals list.
args->rpush(arg);

// Null and delete, since it was copied.
//arg->setOstream(0);		// 07/02/99 AM.
//delete arg;					// 07/02/99 AM.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		02/22/00 AM.
* SUBJ:	Set variable's value to a sem object.
* NOTE:	REPLACE previous values, if any.
*			Want to store even numeric values as string, so that I can
*			have LISP-like freedom from strict type casting.
********************************************/

bool Var::setVal(
	_TCHAR *name,
	RFASem *sem,
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH,	// If pushing value front	// 12/12/14 AM.
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
//if (!name || !sem)	// FIX.	// 08/29/14 AM.
if (!name)	// FIX.	// 08/29/14 AM.
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);

if (!pairx && !sem)	// FIX.	// 08/29/14 AM.
	return true;	// FIX.	// 08/29/14 AM.

if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(sem);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	aval->setSem(0);	// Clear so won't be deleted.		// FIX.	// 06/26/01 AM.
	delete aval;													// FIX.	// 06/26/01 AM.
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

if (sem)	// FIX.	// 08/29/14 AM.
	{
	arg = new Iarg(sem);		// Build an arg for the given sem.
	if (bPUSH)
		args->push(arg);		// 12/12/14 AM.
	else
		args->rpush(arg);			// Add it to the vals list.
	}

return true;
}


/********************************************
* FN:		SETVAL
* CR:		06/26/01 AM.
* SUBJ:	Set variable's value to a num.
* NOTE:	REPLACE previous values, if any.
*			Not sure why this variant wasn't implemented till now.
********************************************/

bool Var::setVal(
	_TCHAR *name,
	long long numval,
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH,	// If pushing value front	// 12/12/14 AM.
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(numval);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	delete aval;													// FIX.	// 06/26/01 AM.
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

arg = new Iarg(numval);		// Build an arg for the given num.

if (bPUSH)
	args->push(arg);		// 12/12/14 AM.
else
	args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		SETVAL
* CR:		08/18/01 AM.
* SUBJ:	Set variable's value to a float
* NOTE:	REPLACE previous values, if any.
********************************************/

bool Var::setVal(
	_TCHAR *name,
	float fval,
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH,	// If pushing value front	// 12/12/14 AM.
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(fval);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	delete aval;
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

arg = new Iarg(fval);	// Build an arg for the given num.

if (bPUSH)
	args->push(arg);		// 12/12/14 AM.
else
	args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		RMVAL
* CR:		07/09/24 Dd.
* SUBJ:	Remove a value
* NOTE:	
********************************************/

bool Var::rmVal(
	_TCHAR *name,
	Dlist<Ipair>* &dlist,
	int &argCount
	)
{
if (!name)
	return false;

Ipair *pairx = 0;
Var::remove(name, dlist);

return true;
}

//////

/********************************************
* FN:		PUSHVAL
* CR:		12/14/14 AM.
* SUBJ:	Push string value onto variable.
* NOTE:	Hard to believe I didn't have this already!
*			REPLACE previous values, if any.
*			Want to store even numeric values as string, so that I can
*			have LISP-like freedom from strict type casting.
********************************************/

bool Var::pushVal(
	_TCHAR *name,
	_TCHAR *str,
	bool bRM,	// If rm existing values	// 12/12/14 AM.
	bool bPUSH,	// If pushing value front	// 12/12/14 AM.
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
// EMPTY STRING CLEARS VALUES.	// 08/29/14 AM.
// if (!name || !str)	// FIX.	// 08/29/14 AM.
if (!name)	// FIX.	// 08/29/14 AM.
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);

// ALREADY EMPTY AND STR EMPTY.	// FIX. // 08/29/14 AM.
if (!pairx && !(str && *str))	// FIX.	// 08/29/14 AM.
  return true;	// FIX.	// 08/29/14 AM.

if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(str);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	delete aval;													// FIX.	// 06/26/01 AM.
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}

if (str && *str)	// FIX.	// 08/29/14 AM.
	{
	arg = new Iarg(str);		// Build an arg for the given str.
	if (bPUSH)
		args->push(arg);		// 12/12/14 AM.
	else
		args->rpush(arg);			// Add it to the vals list.
	}

return true;
}


/********************************************
* FN:		PUSHVAL
* CR:		12/15/14 AM.
* SUBJ:	Push semval on pnode variable.
* WARN:	OPT: NOT COPYING EXISTING VALUES
* WARN:	CALLER MUST DELETE sem AS NEEDED.
* NOTE:	A copy flag would tell what to do if pushing
*			a list of args.
********************************************/

bool Var::pushVal(
	_TCHAR *name,
	RFASem *sem,
	bool bRM,	// If rm existing values (unused, could be a COPY flag)
	bool bPUSH,	// If pushing value front
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
//if (!name || !sem)	// FIX.	// 08/29/14 AM.
if (!name)	// FIX.	// 08/29/14 AM.
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);

if (!pairx && !sem)	// FIX.	// 08/29/14 AM.
	return true;	// FIX.	// 08/29/14 AM.

if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(sem);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	aval->setSem(0);	// Clear so won't be deleted.		// FIX.	// 06/26/01 AM.
	delete aval;													// FIX.	// 06/26/01 AM.
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

#ifdef _OBSO_
if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}
#endif

// TODO: SWITCH STATEMENT HERE.

// NOTE: PUSHING LIST OF ARGS NOT HANDLED. MERGE....			// 12/13/14 AM.

switch (sem->getType()) // 12/14/14 AM.
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		{
		// TRY NO INTERN // 12/14/14 AM.
		_TCHAR *x = sem->getName(); // 12/14/14 AM.
		arg = new Iarg(x);
		}
		break;
	case RSARGS:
		{
		// Unhandled case. // 12/14/14 AM.
		arg = 0;
		return false;
		}
		break;
	case RSLONG: // 12/14/14 AM.
		{
		long long numval = sem->getLong(); // 12/14/14 AM.
		arg = new Iarg(numval);
		}
		break;
	// INCOMPLETE******	// 12/14/14 AM.
	case RSFLOAT: // 12/14/14 AM.
		{
		float fnum = sem->getFloat(); // 12/14/14 AM.
		arg = new Iarg(fnum);
		}
		break;
	default:
		break;
	}

if (arg)
	{
	if (bPUSH)
		args->push(arg);		// 12/12/14 AM.
	else
		args->rpush(arg);			// Add it to the vals list.
	}

return true;
}


/********************************************
* FN:		PUSHVAL
* CR:		12/15/14 AM.
* SUBJ:	Push num onto pnode variable values.
* NOTE:
********************************************/

bool Var::pushVal(
	_TCHAR *name,
	long long numval,
	bool bRM,	// If rm existing values (unused)
	bool bPUSH,	// If pushing value front
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(numval);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	delete aval;													// FIX.	// 06/26/01 AM.
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

#ifdef _OBSO_
if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}
#endif

arg = new Iarg(numval);		// Build an arg for the given num.

if (bPUSH)
	args->push(arg);		// 12/12/14 AM.
else
	args->rpush(arg);			// Add it to the vals list.

return true;
}


/********************************************
* FN:		PUSHVAL
* CR:		12/15/14 AM.
* SUBJ:	Push float onto pnode variable values.
* NOTE:
********************************************/

bool Var::pushVal(
	_TCHAR *name,
	float fval,
	bool bRM,	// If rm existing values	// (unused)
	bool bPUSH,	// If pushing value front
	/*DU*/
	Dlist<Ipair>* &dlist
	)
{
if (!name)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	// MAKE VARIABLE
	Iarg *aval;
	aval = new Iarg(fval);
	bool ok = true;
	ok = create(name, aval, /*DU*/ dlist,pairx);
	delete aval;
	return ok;
	}

Dlist<Iarg> *args;
Iarg *arg = 0;
args = pairx->getVals();
assert(args);

#ifdef _OBSO_
if (bRM && args->getFirst())
	{
	// REMOVE THE PREVIOUS VALUES.
	Dlist<Iarg>::DelDlistAndData(args->getFirst());
	args->setFirst(0);
	args->setLast(0);
	}
#endif

arg = new Iarg(fval);	// Build an arg for the given num.

if (bPUSH)
	args->push(arg);		// 12/12/14 AM.
else
	args->rpush(arg);			// Add it to the vals list.

return true;
}


/////

/********************************************
* FN:		FIND
* CR:		12/08/98 AM.
* SUBJ:	Find dynamic variable in list.
* RET:	True if ok, else false.
* NOTE:	Value as arg lets us handle string, num, etc.
********************************************/

bool Var::find(
	_TCHAR *name,					// Variable name to find.
	Dlist<Ipair> *dlist,		// List of variable-value pairs.
	/*DU*/
	Ipair* &pairx				// Pair found or null.
	)
{
pairx = 0;

if (!name)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::find: given null name.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Delt<Ipair> *delt = 0;
Ipair *pr = 0;
if (dlist)
	{
	for (delt = dlist->getFirst(); delt; delt = delt->Right())
		{
		pr = delt->getData();
		if (!_tcscmp(name, pr->getKey()))
			{
			pairx = pr;
			return true;							// Found it.
			}
		}
	}
return true;				// Didn't find it.  That's ok too.
}


/********************************************
* FN:		remove
* CR:		07/10/24 Dd.
* SUBJ:	Rmove variable from a list.
* RET:	True if ok, else false.
* NOTE:	
********************************************/

bool Var::remove(
	_TCHAR *name,					// Variable name to find.
	Dlist<Ipair> *dlist		// List of variable-value pairs.
	)
{
if (!name)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::find: given null name.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Delt<Ipair> *delt = 0;
Ipair *pr = 0;
if (dlist)
	{
	for (delt = dlist->getFirst(); delt; delt = delt->Right())
		{
		pr = delt->getData();
		if (!_tcscmp(name, pr->getKey()))
			{
			Delt<Ipair> *left = delt->Left();
			Delt<Ipair> *right = delt->Right();
			left->setRight(delt->Right());
			right->setLeft(delt->Left());
			delete delt;
			return true;	
			}
		}
	}

return true;   // Didn't find it.  That's ok too.
}


/********************************************
* FN:		VAL
* CR:		12/08/98 AM.
* SUBJ:	Fetch a variable's value.
* NOTE:	08/03/99 AM. Overhaul.
*			11/03/99 AM. str must be called with a buffer.
********************************************/

bool Var::val(_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ _TCHAR *buf)
{
//str = 0;					// 11/03/99 AM.
buf[0] = '\0';				// 11/03/99 AM.
if (!name || !dlist)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	//*gerr << "[Var::val: Variable=" << name << " not found.]" << std::endl;
	return false;
	}

return Var::val(pairx, /*DU*/ buf);
}


/********************************************
* FN:		VAL
* CR:		08/06/99 AM.
* SUBJ:	Fetch a variable's numeric value.
********************************************/

bool Var::val(_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ long long &num)
{
num = 0;
if (!name || !dlist)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	//*gerr << "[Var::val: Variable=" << name << " not found.]" << std::endl;
	return false;
	}

return Var::val(pairx, /*DU*/ num);
}


/********************************************
* FN:		VAL
* CR:		12/09/98 AM.
* SUBJ:	Fetch a variable's value.
* NOTE:	11/03/99 AM. Changing str to a buffer, so won't have to intern
*			it in a string table.
********************************************/

bool Var::val(Ipair *pairx, /*DU*/ _TCHAR *buf)						// 11/03/99 AM.
{
//str = 0;				// 11/03/99 AM.
buf[0] = '\0';			// 11/03/99 AM.

if (!pairx)
	return false;

Dlist<Iarg> *args;
Delt<Iarg> *darg;
Iarg *arg;
args = pairx->getVals();
assert(args);
darg = args->getFirst();
if (!darg)
	return false;			// 03/10/99 AM.
arg = darg->getData();
assert(arg);


//if (arg->getType() != IASTR)											// 11/03/99 AM.
//	{
//	*gerr << "[Var::val: Bad value type.]" << std::endl;
//	return false;
//	}


// Convert to string as needed.											// 11/03/99 AM.
long long num;
switch (arg->getType())														// 11/03/99 AM.
	{
	case IASTR:
		buf = arg->getStr();			// FETCH VALUE.
		break;
	case IANUM:
		num = arg->getNum();													// 11/03/99 AM.
		// Convert to string.
		// _stprintf(buf, _T("%d"), num);	// 11/03/99 AM.
		_stprintf(buf, _T("%llu"), num);	// 11/03/99 AM.	// 09/26/19 AM.
		break;
	case IAFLOAT:																// 08/17/01 AM.
		_stprintf(buf, _T("%f"), arg->getFloat());							// 08/17/01 AM.
		break;																	// 08/17/01 AM.
	default:
		std::_t_strstream gerrStr;
		gerrStr << _T("[Var::inc: Bad value type.]") << std::ends;
		errOut(&gerrStr,false);
		return false;
	}

return true;
}


/********************************************
* FN:		VAL
* CR:		08/06/99 AM.
* SUBJ:	Fetch a variable's numeric value.
* TODO:	Could convert string-valued attributes with all digits also.
********************************************/

bool Var::val(Ipair *pairx, /*DU*/ long long &num)
{
num = 0;

if (!pairx)
	return false;

Dlist<Iarg> *args;
Delt<Iarg> *darg;
Iarg *arg;
args = pairx->getVals();
assert(args);
darg = args->getFirst();
if (!darg)
	return false;			// 03/10/99 AM.
arg = darg->getData();
assert(arg);


//if (arg->getType() == IANUM)							// 09/20/99 AM.
//	{
//	num = arg->getNum();			// FETCH VALUE.
//	return true;

//	}

// Convert to number.
//if (arg->getType() != IASTR)								// 11/03/99 AM.
//	{
//	*gerr << "[Var::val: Bad value type.]" << std::endl;	// 09/20/99 AM.
//	return false;
//	}

// 09/20/99 AM. Getting number from string variable, if all numeric.
//return str_to_long(arg->getStr(), /*UP*/ num);		// 09/20/99 AM.

// Convert to number as needed.								// 11/03/99 AM.
switch (arg->getType())											// 11/03/99 AM.
	{
	case IASTR:
		if (!str_to_long(arg->getStr(), num))
			return false;
		break;
	case IANUM:
		num = arg->getNum();										// 11/03/99 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Var::inc: Bad value type.]") << std::ends;
		errOut(&gerrStr,false);
		}

		return false;
	}
return true;
}


/********************************************
* FN:		VAL
* CR:		08/17/01 AM.
* SUBJ:	Fetch a variable's float value.
* TODO:	Could convert string-valued attributes with all digits also.
********************************************/

bool Var::val(Ipair *pairx, /*DU*/ float &fnum)
{
fnum = 0;

if (!pairx)
	return false;

Dlist<Iarg> *args;
Delt<Iarg> *darg;
Iarg *arg;
args = pairx->getVals();
assert(args);
darg = args->getFirst();
if (!darg)
	return false;
arg = darg->getData();
assert(arg);

// Convert to number as needed.
switch (arg->getType())
	{
//	case IASTR:
//		if (!str_to_float(arg->getStr(), num))
//			return false;
//		break;
	case IAFLOAT:
		fnum = arg->getFloat();
		break;
	default:
		std::_t_strstream gerrStr;
		gerrStr << _T("[Var::inc: Bad value type.]") << std::ends;
		return errOut(&gerrStr,false);
	}
return true;
}


/********************************************
* FN:		VAL
* CR:		02/09/99 AM.
* SUBJ:	Fetch a variable's ostream pointer value.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::val(_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ std::_t_ostream* &ostr)
{
ostr = 0;
if (!name || !dlist)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)
	{
	//*gerr << "[Var::val: Variable=" << name << " not found.]" << std::endl;
	return false;
	}

return Var::val(pairx, /*DU*/ ostr);
}


/********************************************
* FN:		VAL
* CR:		02/09/99 AM.
* SUBJ:	Fetch a variable's value.
********************************************/

bool Var::val(Ipair *pairx, /*DU*/ std::_t_ostream* &ostr)
{
ostr = 0;

if (!pairx)
	return false;

Dlist<Iarg> *args;
Delt<Iarg> *darg;
Iarg *arg;
args = pairx->getVals();
assert(args);
darg = args->getFirst();
if (!darg)			// Empty list. // 02/09/99 AM.
	return true;	// That's ok.
arg = darg->getData();
assert(arg);


// Convert to ostream ptr.
if (arg->getType() != IAOSTREAM)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::val: Bad value type.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

ostr = arg->getOstream();			// FETCH VALUE.
return true;
}


/********************************************
* FN:		VALS
* CR:		01/25/98 AM.
* SUBJ:	Fetch handle for a variable's values.
* NOTE:	08/03/99 AM. Overhaul.
********************************************/

bool Var::vals(_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ Delt<Iarg>* &values)
{
values = 0;
Dlist<Iarg> *list = 0;
if (!Var::vals(name, dlist, /*DU*/ list))
	return false;
if (!list)
	return true;

values = list->getFirst();
return true;
}

/********************************************
* FN:		VALS
* CR:		01/25/98 AM.
* SUBJ:	Fetch handle for a variable's values.
********************************************/

bool Var::vals(_TCHAR *name, Dlist<Ipair> *dlist, /*DU*/ Dlist<Iarg>* &list)
{
list = 0;
if (!name || !dlist)
	return false;

Ipair *pairx = 0;
Var::find(name, dlist, /*DU*/ pairx);
if (!pairx)			// In list already.
	{
	//*gerr << "Var::vals: Variable=" << name << " not found.]"
	//	  << std::endl;
	return false;
	}

list = pairx->getVals();
return true;
}


/********************************************
* FN:		GETSTR
* CR:		01/25/98 AM.
* SUBJ:	Get string for a value.
********************************************/

_TCHAR *Var::getStr(Delt<Iarg> *value)
{
if (!value)
	return 0;

Iarg *arg;
arg = value->getData();
assert(arg);

if (arg->getType() != IASTR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Var::getStr: Bad value type.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

return arg->getStr();			// FETCH VALUE.
}


/********************************************
* FN:		SORTVALS
* CR:		01/25/98 AM.
* SUBJ:	Sort values of a variable alphabetically.
* NOTE:	Redundant values retained.
*			Another function could delete those if desired.
*			08/03/99 AM. Overhaul.
********************************************/

bool Var::sortVals(_TCHAR *name, Dlist<Ipair> *dlist)
{
Dlist<Iarg> *list = 0;
if (!Var::vals(name, dlist, /*DU*/ list))
	return false;
if (!list)
	return true;
Delt<Iarg> *vals;
if (!(vals = list->getFirst()))
	return true;

// Create an array.
long len = 0;
Delt<Iarg> **arr;
len = list->getLength();
arr = new Delt<Iarg> * [len];

// Make array elts point to list elts.
Delt<Iarg> **parr = 0;
Delt<Iarg> *pvals = 0;
for (pvals = vals, parr = arr;
	  pvals;
	  pvals = pvals->Right(), ++parr
	  )
	{
	*parr = pvals;
	}

// Sort array.
qsort((void *)arr, (size_t)len, sizeof(Delt<Iarg> *), Var::compare);

// Relink list.
parr = arr;
(*parr)->setLeft(0);		// Fix first element's left ptr.
while (--len > 0)			// For all but last elt.
	{
	(*parr)->setRight(*(parr+1));
	(*(parr+1))->setLeft(*parr);
	++parr;
	}
// Fix up the last element.
(*parr)->setRight(0);

// Reset list object.
list->setFirst(*arr);
list->setLast(*parr);

// Trash the array.
delete [] arr;
return true;
}


/********************************************
* FN:		TOLOWERVALS
* CR:		12/15/99 AM.
* SUBJ:	Convert global var values to lowercase.
* NOTE:	Redundant values retained.
********************************************/

bool Var::tolowerVals(_TCHAR *name, Parse *parse)
{
if (!name || !*name || !parse)
	return false;
return tolowerVals(name, parse->getVars(), parse);
}


/********************************************
* FN:		TOLOWERVALS
* CR:		12/15/99 AM.
* SUBJ:	Convert variable values to lowercase.
* NOTE:	Redundant values retained.
********************************************/

bool Var::tolowerVals(_TCHAR *name, Dlist<Ipair> *dlist, Parse *parse)
{
Dlist<Iarg> *list = 0;
if (!Var::vals(name, dlist, /*DU*/ list))
	return false;
if (!list)
	return true;

Delt<Iarg> *dval;
Iarg *arg;
_TCHAR *upstr, *str;
_TCHAR buf[MAXSTR];
for (dval = list->getFirst(); dval; dval = dval->Right())
	{
	arg = dval->getData();
	if (arg->getType() == IASTR)		// Just do string values.
		{
		upstr = arg->getStr();
		// Convert to lowercase, intern, and set.
		if (upstr && *upstr)
			{
			str_to_lower(upstr, /*UP*/ buf);
			parse->internStr(buf, /*UP*/ str);
			arg->setStr(str);
			}
		}
	}

return true;
}


/********************************************
* FN:		UNIQVALS
* CR:		12/15/99 AM.
* SUBJ:	Uniquefy sorted list of strings in global var.
********************************************/

bool Var::uniqVals(_TCHAR *name, Parse *parse)
{
if (!name || !*name || !parse)
	return false;
return uniqVals(name, parse->getVars(), parse);
}


/********************************************
* FN:		UNIQVALS
* CR:		12/15/99 AM.
* SUBJ:	Uniquefy sorted list of strings in var.
* ASS:	All values are strings.
********************************************/

bool Var::uniqVals(_TCHAR *name, Dlist<Ipair> *dlist, Parse *parse)
{
Dlist<Iarg> *list = 0;
if (!Var::vals(name, dlist, /*DU*/ list))
	return false;
if (!list)
	return true;

Delt<Iarg> *dval = list->getFirst();
if (!dval)
	return true;	// Empty list is ok.
Iarg *arg = dval->getData();
_TCHAR *first = arg->getStr();
_TCHAR *str;
Delt<Iarg> *tmp;
dval = dval->Right();	// Get next value.
for (;;)				// While traversing list of values.
	{
	if (!dval)
		return true;			// Done with list.
	arg = dval->getData();
	str = arg->getStr();
	if (!_tcscmp(first, str))		// If redundant string.
		{
		// Zap it.
		tmp = dval;
		dval = dval->Right();
		list->exciseElt(tmp);
		Delt<Iarg>::DeleteDeltAndData(tmp);
		}
	else									// Not redundant.
		{
		first = str;					// New val to compare against.
		dval = dval->Right();
		}
	}
return true;
}


/********************************************
* FN:		COMPARE
* CR:		01/25/98 AM.
* SUBJ:	Sort values of a variable alphabetically.
********************************************/

int Var::compare( const void *arg1, const void *arg2 )
{
Delt<Iarg> **a1, **a2;

a1 = (Delt<Iarg> **) arg1;
a2 = (Delt<Iarg> **) arg2;

Iarg *ar1, *ar2;
ar1 = (*a1)->getData();
ar2 = (*a2)->getData();

return strcmp_i(ar1->getStr(), ar2->getStr());
}



/********************************************
* FN:		DUMP
* CR:		12/13/99 AM.
* SUBJ:	Dump variable list to stream.
********************************************/

bool Var::dump(Dlist<Ipair> *vars, std::_t_ostream *ostr)
{
if (!ostr)
	return false;
if (!vars)
	return true;	// No vars is ok!

Delt<Ipair> *dpair;
Ipair *pairx;
_TCHAR *name;
Dlist<Iarg> *list;
Delt<Iarg> *darg;
Iarg *arg;
_TCHAR *sep = _T("||");		// Separator between values.
for (dpair = vars->getFirst(); dpair; dpair = dpair->Right())
	{
	pairx = dpair->getData();
	name = pairx->getKey();
	list = pairx->getVals();

	// Print out variable and its values.
	*ostr << name << _T(" = ");
	if (!list)
		{
		*ostr << std::endl;
		continue;
		}
	for (darg = list->getFirst(); darg; darg = darg->Right())
		{
		arg = darg->getData();
		switch (arg->getType())
			{
			case IASTR:
				*ostr << arg->getStr();
				break;
			case IANUM:
				*ostr << arg->getNum();
				break;
			case IAOSTREAM:
				*ostr << _T("<ostream>");
			default:
				break;
			}
		}
	*ostr << std::endl;
	}
return true;
}


/********************************************
* FN:		FILEVAR
* CR:		12/31/99 AM.
* SUBJ:	Start the output to given filename.
* RET:	True if ok.
*			UP var  - global ostream var.
*			UP ostr - the ostream itself.
*			UP some flags.
* NOTE:	Internals of building an ostream var.
*			Taken from codeFileout.
* TODO:	Should also do some checks on the filename, eg, relative
*			vs absolute.
********************************************/

bool Var::filevar(
   _TCHAR *fname,				// Filename to open or use.
	Parse *parse,
	/*UP*/
	Ipair* &pairx,				// The variable-value pair.
	std::_t_ostream* &ostr,			// Var's ostream value.
	bool &exists,				// True if ostream var was already present.
	bool &open,					// True if ostream already open.
	bool &inuse,				// True if var in use for a non-ostream value.
	bool &badname				// If bad filename.
	)
{
if (!parse)	// 05/20/09 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[filevar: No parse structure.'") << _T("']") << std::ends;
	errOut(&gerrStr,false);
	return false;	// 05/20/09 AM.
	}
pairx = 0;
ostr = 0;
exists = open = inuse = badname = false;
if (!fname)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't open empty filename.]") << std::ends;
	errOut(&gerrStr,false);
	badname = true;
	return false;
	}

// Get the variable matching the filename, if any.
if (!Var::find(fname, parse, pairx))
	return false;

if (pairx)							// Var exists already.
	{
	exists = true;					// Var already present.

	if (!Var::val(pairx, /*DU*/ ostr))
	//if (!Var::val(fname, parse, /*DU*/ ostr))
		{
		inuse = true;
		return false;
		}

	// Need to check if file is already open.
	// Need some kind of list for output files associated with the
	// current text being analyzed.
	if (ostr)
		{
		open = true;
		return true;
		}

	// Else, var exists but has no value.  In effect, doesn't exist.
	exists = false;
	}
else
	{
	// If var not present, create a new var.
	Dlist<Ipair> *dlist = parse->getVars();
	if (!Var::create(fname, 0, /*DU*/ dlist, pairx))
		return false;
	parse->setVars(dlist);
	}

// Set up proper path for file.
// Should check if it's relative or absolute.
_TCHAR buf[MAXSTR];
_TCHAR *str = 0;
//sprintf(buf, "%s\\output\\%s", parse->getAppdir(), fname); // 03/10/99 AM.
//sprintf_s(buf, "%s\\output\\%s", parse->getAppdir(), fname); // 03/10/99 AM.
_stprintf(buf, _T("%s%c%s"), parse->getOutdir(),DIR_CH, fname);	 // 03/10/99 AM.
parse->internStr(buf, /*UP*/ str);

// Open the output file for APPENDING.
ostr = new std::_t_ofstream(TCHAR2CA(str), std::ios::app);
#ifdef UNICODE
*ostr << BOM_UTF8;
#endif
parse->newostr(ostr);			// Add to list of ostrs.			// 05/23/01 AM.

// Set the variable's value to the output stream pointer.
if (!Var::setVal(pairx, ostr))
	return false;

return true;
}


/********************************************
* FN:		CLEAN_VARS
* CR:		09/26/00 AM.
* SUBJ:	Clean out ostream-valued vars.
* NOTE:	Used prior to deleting the current parse.  Need to close files,
*			delete ostreams for the parse.
* ASS:	Assume only one value per ostream variable.
********************************************/

bool Var::clean_vars(Dlist<Ipair> *dlist)
{
#ifdef OLD_

if (!dlist)
	return true;
Delt<Ipair> *delt;
Ipair *pairx;
Dlist<Iarg> *dargs;
Delt<Iarg> *darg;
Iarg *arg;
std::_t_ostream *ostr;
RFASem *sem;
for (delt = dlist->getFirst(); delt; delt = delt->Right())
	{
	pairx = delt->getData();
	if (!(dargs = pairx->getVals()))
		continue;
	if (!(darg = dargs->getFirst()))
		continue;
	arg = darg->getData();
	if (arg->getType() == IAOSTREAM)
		{
		ostr = arg->getOstream();
//		delete ostr;
		arg->setOstream(0);
		}
	else if (arg->getType() == IASEM)
		{
		sem = arg->getSem();
		if (sem->getType() == RSOSTREAM)
			{
			ostr = sem->getOstream();
//			delete ostr;
			sem->setOstream(0);
			}
		}
	}
#endif
return true;
}


/********************************************
* FN:		COPY_VARS
* CR:		05/16/01 AM.
* SUBJ:	Copy vars and values from one list to another.
* NOTE:	Supports "copying up semantics".
*			CREATES destination list if empty.
*			If destination has redundant var name, aborts the copy.
*			(for now).
********************************************/

bool Var::copy_vars(
	Dlist<Ipair> *fmlist,
	/*DU*/
	Dlist<Ipair>* &tolist
	)
{
if (!fmlist)
	return true;
Delt<Ipair> *dpair = fmlist->getFirst();
if (!dpair)								// Empty list.
	return true;

if (!tolist)
	tolist = new Dlist<Ipair>();	// Create empty list.

Ipair *fmpair, *topair;
_TCHAR *name;
Dlist<Iarg> *vals = 0;
// For each var in list, copy it to destination.
for (dpair = fmlist->getFirst(); dpair; dpair = dpair->Right())
	{
	fmpair = dpair->getData();
	name = fmpair->getKey();

	// If destination has variable name, abort the copy.
	if (!Var::find(name, tolist, /*DU*/ topair))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[copy_vars: Error in copying '") << name << _T("']") << std::ends;
		errOut(&gerrStr,false);
		return false;
		}
	// OK if pair already exists.								// FIX.	// 11/27/01 AM.
	// Don't even bother issuing a warning.				// FIX.	// 11/27/01 AM.
//	if (topair)
//		{
//		strstream gerrStr(Errbuf,MAXSTR,std::ios::out);
//		gerrStr << "[copy_vars: Variable '" << name << "' already assigned.]"
//				<< std::ends;
//		return errOut(false);
//		}

	// Copy the values to destination var.
// Copy constructor for Dlist<Iarg> doesn't copy sem.				// 05/18/01 AM.
// FIX: Use a different copy function.									// 05/18/01 AM.
//	vals = new Dlist<Iarg>(*(fmpair->getVals())); // Doesn't copy sem.
	vals = Iarg::copy_args(fmpair->getVals());			// FIX.	// 05/18/01 AM.
	if (!Var::setVals(name, vals, /*DU*/ tolist))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[copy_vars: Error during copy.]") << std::ends;
		errOut(&gerrStr,false);
		return false;
		}
	}
return true;
}


/********************************************
* FN:		COPY_VARS
* CR:		01/19/23  Dd.
* SUBJ:	Copy vars and values from one list to a concept.
* NOTE:	Supports "copying up semantics".
*			CREATES destination list if empty.
*			If destination has redundant var name, aborts the copy.
*			(for now).
********************************************/

bool Var::copy_vars(
	Dlist<Ipair> *fmlist,
	/*DU*/
	RFASem *sem1,
	CG *cg
	)
{
if (!fmlist)
	return true;
Delt<Ipair> *dpair = fmlist->getFirst();
if (!dpair)								// Empty list.
	return true;

// if (!con)
// 	tolist = new Dlist<Ipair>();	// Create empty list.

Ipair *fmpair;
_TCHAR *name;
Dlist<Iarg> *vals = 0;
Delt<Iarg> *val = 0;
Iarg *arg = 0;
RFASem *s;
RFASemtype semType;
CONCEPT *conc1 = sem1->getKBconcept();
CON *c1 = (CON *)conc1;

// For each var in list, copy it to the concept.
for (dpair = fmlist->getFirst(); dpair; dpair = dpair->Right())
	{
	fmpair = dpair->getData();
	name = fmpair->getKey();
	vals = fmpair->getVals();
	val = vals->getFirst();

	arg = val->getData();
	enum Iargtype argType = arg->getType();
	if (argType == IASEM)
		{
		s = arg->getSem();
		semType = s->getType();

		if (semType == RS_KBCONCEPT) {
			CONCEPT *con = s->getKBconcept();
			CON *c = (CON *)con;
			cg->addVal(conc1,name,con);
			}
		else if (semType == RS_KBVAL) {
			int what = 1;
			}
		else
			{
			int there = 1;
			}	
		} 
	else if (argType == IANUM)
		{
		long long num = arg->getNum();
		cg->addVal(conc1,name,num);
		}
	else if (argType == IASTR)
		{
		_TCHAR *str = arg->getStr();
		cg->addSval(conc1,name,str);
		}
	else if (argType == IAFLOAT)
		{
		float num = arg->getFloat();
		cg->addVal(conc1,name,num);
		}
	else
		{
		int where = 1;
		}
}

return true;
}


/*************************      END OF FILE      ******************************/
