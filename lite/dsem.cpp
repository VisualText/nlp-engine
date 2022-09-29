/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DSEM.CPP
* FILE:	lite/dsem.cpp
* CR:		08/03/99 AM.
* SUBJ:	Dynamic semantics.
* NOTE:	Dynamic semantics refers to data inside parse tree nodes.  The
*			user can update this data via code actions and rule actions.
*			Copied part of var.cpp and modified.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "lite/Auser.h"				// 07/07/03 AM.
#include	"lite/iarg.h"					// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "string.h"
#include "tree.h"							// 02/26/01 AM.
#include "pn.h"							// 02/26/01 AM.
#include "arg.h"
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "parse.h"
#include "iaction.h"
#include "ifile.h"
#include "seqn.h"

#include "var.h"
#include "dsem.h"

/********************************************
* FN:		Special functions for the class
********************************************/

Dsem::Dsem()
{
*gout << _T("[DON'T CREATE OBJECTS OF Dsem CLASS.]") << std::endl;
}

/*******************************************/

/*******************************************/

Dsem::~Dsem()
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
* FN:		PARSE NODE-SPECIFIC FUNCTIONS
********************************************/

/********************************************
* FN:		CREATE
* CR:		08/04/99 AM.
* SUBJ:	Create a new dynamic variable.
********************************************/

bool Dsem::create(_TCHAR *name, Iarg *val, Pn *pn, Parse *parse)
{
Ipair *pair = 0;	// 11/17/99 AM.

// Not requiring a value.
if (!name || !pn || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();

bool ok = true;
ok = Var::create(name, val, /*DU*/ dlist,pair);
pn->setDsem(dlist);
//cout << "Dsems:" << std::endl;
//cout << *(pn->getDsem()) << std::endl;
return ok;
}

/********************************************
* FN:		CREATESTRS
* CR:		08/04/99 AM.
* SUBJ:	Create a new dynamic variable with list of strings as value.
********************************************/

bool Dsem::createstrs(_TCHAR *name, Pn *pn, Parse *parse)
{
if (!name || !pn || !parse)
	return false;

// If variable exists already, error.
// Traverse the list to verify.
// Need a find-in-list function for Ipair objects.
// Here should be a separate Get function.
Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();

bool ok = true;
ok = Var::createstrs(name, /*DU*/ dlist);
pn->setDsem(dlist);
//cout << "Dsems:" << std::endl;
//cout << *(parse->getVars()) << std::endl;
return ok;
}


/********************************************
* FN:		INC
* CR:		08/04/99 AM.
* SUBJ:	Increment a variable's value.
********************************************/

bool Dsem::inc(_TCHAR *name, Pn *pn, Parse *parse)
{
if (!name || !pn || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();
bool ok = true;
ok = Var::inc(name, parse, /*DU*/ dlist);
pn->setDsem(dlist);
return true;
}


/********************************************
* FN:		ADDSTRS
* CR:		08/04/99 AM.
* SUBJ:	Add a string to a string-list variable.
********************************************/

bool Dsem::addStrs(
	_TCHAR *name,			// Variable name.
	_TCHAR *str,			// String value to add.
	Pn *pn,
	Parse *parse
	)
{
if (!name || !str || !*str || !pn || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();
bool ok = true;
ok = Var::addStrs(name, str, /*DU*/ dlist);
pn->setDsem(dlist);
return ok;
}


/********************************************
* FN:		SETVAL
* CR:		08/04/99 AM.
* SUBJ:	Set variable's value to an ostream pointer.
********************************************/

bool Dsem::setVal(
	_TCHAR *name,
	Pn *pn,
	Parse *parse,
	std::_t_ostream *ostr)
{
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();
bool ok = true;
ok = Var::setVal(name, ostr, /*DU*/ dlist);
pn->setDsem(dlist);
return ok;
}

/********************************************
* FN:		FIND
* CR:		08/04/99 AM.
* SUBJ:	Find dynamic variable in list.
* RET:	True if ok, else false.
* NOTE:	Value as arg lets us handle string, num, etc.
********************************************/

bool Dsem::find(
	_TCHAR *name,					// Variable name to find.
	Pn *pn,
	Parse *parse,
	/*DU*/
	Ipair* &pairx				// Pair found or null.
	)
{
pairx = 0;
return (Var::find(name, pn->getDsem(), pairx));
}


/********************************************
* FN:		VAL
* CR:		08/04/99 AM.
* SUBJ:	Fetch a variable's value.
********************************************/

bool Dsem::val(_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ _TCHAR* &str)
{
str = 0;
if (!name || !pn || !parse)
	return false;


Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();
bool ok = true;
// 11/03/99 AM. Must intern string here.
_TCHAR buf[MAXSTR];												// 11/03/99 AM.
ok = Var::val(name, dlist, /*DU*/ buf);
parse->internStr(buf, /*DU*/ str);						// 11/03/99 AM.
return ok;
}


/********************************************
* FN:		VAL
* CR:		08/04/99 AM.
* SUBJ:	Fetch a variable's ostream pointer value.
********************************************/

bool Dsem::val(_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ std::_t_ostream* &ostr)
{
ostr = 0;
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();
bool ok = true;
ok = Var::val(name, dlist, /*DU*/ ostr);
return ok;
}


/********************************************
* FN:		VALS
* CR:		08/04/99 AM.
* SUBJ:	Fetch handle for a variable's values.
********************************************/

bool Dsem::vals(
	_TCHAR *name,
	Pn *pn,
	Parse *parse,
	/*DU*/
	Delt<Iarg>* &values
	)
{
values = 0;
Dlist<Ipair> *dlist;			// List of variables.
dlist = pn->getDsem();
bool ok = true;
ok = Var::vals(name, dlist, /*DU*/ values);
return ok;
}

/********************************************
* FN:		VALS
* CR:		08/04/99 AM.
* SUBJ:	Fetch handle for a variable's values.
********************************************/

bool Dsem::vals(_TCHAR *name, Pn *pn, Parse *parse, /*DU*/ Dlist<Iarg>* &list)
{
list = 0;
if (!name || !parse)
	return false;

Dlist<Ipair> *dlist = 0;				// Global list of vars.
dlist = pn->getDsem();
bool ok = true;
ok = Var::vals(name, dlist, /*DU*/ list);
return ok;
}


/********************************************
* FN:		SORTVALS
* CR:		08/04/99 AM.
* SUBJ:	Sort values of a variable alphabetically.
* NOTE:	Redundant values retained.
*			Another function could delete those if desired.
********************************************/


bool Dsem::sortVals(_TCHAR *name, Pn *pn, Parse *parse)
{
if (!name || !parse)
	return false;
Dlist<Ipair> *dlist = 0;
dlist = pn->getDsem();
bool ok = true;
ok = Var::sortVals(name, dlist);
return ok;
}



/*************************      END OF FILE      ******************************/
