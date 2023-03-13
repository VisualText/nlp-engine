/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AVAR.CPP
* FILE:	lite\var.cpp
* CR:		12/09/98 AM.
* SUBJ:	API functions for accessing user-created variables.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"	// 07/18/00 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "parse.h"
#include "var.h"
#include "ipair.h"			// 02/14/01 AM.
#include "lite/Avar.h"

/********************************************
* FN:		Special functions for the class
********************************************/

Avar::Avar()
{
std::_t_strstream gerrStr;
gerrStr << _T("[DON'T CREATE OBJECTS OF Avar CLASS.]") << std::ends;
errOut(&gerrStr,false);
}

/*******************************************/

/*******************************************/

Avar::~Avar()
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
* FN:		GETVARS
* CR:		12/09/98 AM.
* SUBJ:	Return handle for the list of variables.
********************************************/

LITE_API void *Avar::getVars(void *hparse)
{
Parse *parse = 0;
parse = (Parse *) hparse;
if (parse)
	return (void *) (parse->getVars());
return 0;
}


/********************************************
* FN:		GETVAR
* CR:		12/09/98 AM.
* SUBJ:	Return handle for one variable.
********************************************/

LITE_API void *Avar::getVar(_TCHAR *name, void *hvars)
{
Dlist<Ipair> *vars = 0;
Ipair *pair = 0;

vars = (Dlist<Ipair> *) hvars;
if (!vars)
	return 0;

if (!Var::find(name, vars, /*DU*/ pair))
	return 0;

return (void *) pair;		// Variable is represented as a pair.
}


/********************************************
* FN:		GETVAR
* CR:		12/09/98 AM.
* SUBJ:	Get var from pvar.
********************************************/

LITE_API void *Avar::getVar(void *pvar)
{
if (!pvar)
	return 0;
Delt<Ipair> *var;

var = (Delt<Ipair> *) pvar;
return (void *) (var->getData());
}


/********************************************
* FN:		GETNAME
* CR:		12/09/98 AM.
* SUBJ:	Get variable name.
********************************************/

LITE_API _TCHAR *Avar::getName(void *hvar)
{
if (!hvar)
	return 0;
Ipair *var;

var = (Ipair *) hvar;
return var->getKey();
}


/********************************************
* FN:		GETLONG
* CR:		12/09/98 AM.
* SUBJ:	Get a long value for a variable.
********************************************/

LITE_API bool Avar::getLong(void *hvar, /*DU*/ long long &val)
{
// val = -1;

//Ipair *var = 0;

//var  = (Ipair *) hvar;

//if (!var)
//	return false;

//char *str = 0;										// 11/03/99 AM.
//if (!Var::val(var, /*DU*/ str))				// 11/03/99 AM.
//	return false;										// 11/03/99 AM.

// Convert to long
//if (!str_to_long(str, val))						// 11/03/99 AM.
//	return false;										// 11/03/99 AM.
// return true;										// 11/03/99 AM.

return Var::val((Ipair *) hvar,/*DU*/ val);		// 11/03/99 AM.
}


/********************************************
* FN:		GETLONG
* CR:		12/09/98 AM.
* SUBJ:	Get a long value for a variable.
********************************************/

LITE_API bool Avar::getLong(_TCHAR *name, void *hvars, /*DU*/ long long &val)
{
val = -1;

if (!name || !hvars)
	return false;

Dlist<Ipair> *vars;
Ipair *pair;

vars = (Dlist<Ipair> *) hvars;

if (!Var::find(name, vars, /*DU*/ pair))
	return 0;

return Avar::getLong(pair, /*DU*/ val);
}


/********************************************
* FN:		GETFIRST
* CR:		12/09/98 AM.
* SUBJ:	Get first pvar in list.
********************************************/

LITE_API void *Avar::getFirst(void *hvars)
{
if (!hvars)
	return 0;
Dlist<Ipair> *vars;

vars = (Dlist<Ipair> *) hvars;
return (void *) (vars->getFirst());
}


/********************************************
* FN:		GETNEXT
* CR:		12/09/98 AM.
* SUBJ:	Get next pvar in list.
********************************************/

LITE_API void *Avar::getNext(void *pvar)
{
if (!pvar)
	return 0;
Delt<Ipair> *var;

var = (Delt<Ipair> *) pvar;
return (void *) (var->Right());
}

/********************************************
* FN:		INPUTFILE
* CR:		12/09/98 AM.
* SUBJ:	Get name of input file.
********************************************/

LITE_API _TCHAR *Avar::Inputfile(void *hparse)
{
Parse *parse;
if (!hparse)
	return 0;

parse = (Parse *) hparse;
return parse->getInput();
}
