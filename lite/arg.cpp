/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ARG.CPP
* FILE:	lite\arg.cpp
* CR:		11/27/98 AM.
* SUBJ:	Arg actions for Pat pass.
* NOTE:	User can employ arg actions to accept or reject the rule match
*			prior to modifying the parse tree.
*
*******************************************************************************/

#include "StdAfx.h"
#ifdef LINUX
#ifdef _ODBC
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#endif
#endif
#include "machine.h"	// 10/25/06 AM.
#include "lite/global.h"
#include "u_out.h"		// 01/19/06 AM.
#include "dlist.h"						// 02/26/01 AM.
#include "lite/Auser.h"					// 07/07/03 AM.
#include "iarg.h"							// 02/26/01 AM.
#include "str.h"
#include "io.h"
#include "string.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"							// 02/26/01 AM.
#include "pn.h"							// 02/26/01 AM.
#include "rfasem.h"						// 05/26/02 AM.
#include "ivar.h"							// 05/26/02 AM.
#include "nlppp.h"						// 11/14/02 AM.
#include "parse.h"						// 11/14/02 AM.

#include "arg.h"

/********************************************
* FN:		Special functions for the class
********************************************/

Arg::Arg()
{
*gout << _T("[DON'T CREATE OBJECTS OF Arg CLASS.]") << std::endl;
std::_t_strstream gerrStr;							// 02/19/02 AM.
gerrStr << _T("[DON'T CREATE OBJECTS OF Arg CLASS.]") << std::ends;		// 02/19/02 AM.
errOut(&gerrStr,false);																	// 02/19/02 AM.
}

/*******************************************/

/*******************************************/

Arg::~Arg()
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
* FN:		DONE
* CR:		11/29/98 AM.
* SUBJ:	Check that args have been used up.
* RET:	True if no args left, else false.
********************************************/

bool Arg::done(
	DELTS *args,			// Remaining arguments in list.
	_TCHAR *errstr,				// String for reporting errors.
	Parse *parse
	)
{
if (!args)
	return true;

_stprintf(Errbuf,_T("[%s: Too many args.]"), errstr);						// 05/18/01 AM.
if (parse)																		// 06/12/03 AM.
	return parse->errOut(false); // UNFIXED 											// 06/12/03 AM.
return errOut(false); // UNFIXED 														// 05/18/01 AM.
}


/********************************************
* FN:		ELT1
* CR:		11/29/98 AM.
* SUBJ:	Get nodes that represent one rule element.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::elt1(
	TREE *collect,		// Tracking for current rule match.
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	NODE* &coll,			// Collect for rule element, if any.
   NODE* &start,			// First node for rule element, if any.
	NODE* &end
	)
{
coll = 0;
start = end = 0;

if (!args || !collect)
	{
	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);		// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

long long num;
switch(arg->getType())													// 11/15/99 AM.
	{
	case IANUM:
		num = arg->getNum();													// 11/15/99 AM.
		break;														// FIX.	// 12/20/99 AM.
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg->getStr(), num))
			{
			_stprintf(Errbuf,_T("[%s: Arg not numeric.]"), errstr);		// 05/18/01 AM.
			return errOut(false); // UNFIXED 											// 05/18/01 AM.
			}
		break;
	default:
		_stprintf(Errbuf,_T("[%s: Arg must be string.]"), errstr);		// 05/18/01 AM.
		return errOut(false); // UNFIXED 												// 05/18/01 AM.
	}

// Check self-consistency.
if (!num)
	return true;		// Accepting zero numeric value.

long long len;
Node<Pn> *colls;
colls = ((Tree<Pn> *)collect)->getRoot();
len = colls->length();
if (num > len)
	{
	_stprintf(Errbuf,_T("[%s: Numeric arg larger than rule phrase.]"),// 05/18/01 AM.
																	errstr);		// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

coll = colls->nth(num);
assert(coll);

// Get needed stuff from each collect.
start = (NODE*)((Node<Pn>*)coll)->Down();
end   = (NODE*)((Node<Pn>*)coll)->eDown();

return true;
}


/********************************************
* FN:		NUM1
* CR:		11/29/98 AM.
* SUBJ:	Get a numeric arg.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::num1(
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	long long &num,					// Converted number.
	bool opt						// If arg is optional.	// 12/25/99 AM.
	)
{
num = 0;

if (!args)
	{
	if (!opt)											// 12/25/99 AM.
		{
		_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);	// 05/18/01 AM.
		return errOut(false); // UNFIXED 												// 05/18/01 AM.
		}
	return false;
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

switch (arg->getType())					// 11/15/99 AM.
	{
	case IANUM:								// 11/15/99 AM.
		num = arg->getNum();				// 11/15/99 AM.
		break;								// 11/15/99 AM.
	case IASTR:								// 11/15/99 AM.
		// Convert string to number.
		if (!str_to_long(arg->getStr(), num))
			{
			_stprintf(Errbuf,_T("[%s: Arg not numeric.]"), errstr);		// 05/18/01 AM.
			return errOut(false); // UNFIXED 											// 05/18/01 AM.
			}
		break;
	default:
		_stprintf(Errbuf,_T("[%s: Arg must be integer.]"), errstr);		// 05/18/01 AM.
		return errOut(false); // UNFIXED 												// 05/18/01 AM.
	}

return true;
}



/********************************************
* FN:		FLOAT1
* CR:		04/29/04 AM.
* SUBJ:	Get a floating point arg.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::float1(
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	float &num,					// Converted number.
	bool opt						// If arg is optional.
	)
{
num = 0.0;

if (!args)
	{
	if (!opt)
		{
		_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);
		return errOut(false); // UNFIXED 
		}
	return false;
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.
long long numx = 0;
switch (arg->getType())
	{
	case IAFLOAT:
		num = arg->getFloat();
		break;
	case IANUM:
		num = (float)(arg->getNum());
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg->getStr(), numx))
			{
			_stprintf(Errbuf,_T("[%s: Arg not numeric.]"), errstr);
			return errOut(false); // UNFIXED 
			}
		num = (float) numx;
		break;
	default:
		_stprintf(Errbuf,_T("[%s: Arg must be string.]"), errstr);
		return errOut(false); // UNFIXED 
	}

return true;
}


/********************************************
* FN:		STR1
* CR:		12/07/98 AM.
* SUBJ:	Get a string arg.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::str1(
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	_TCHAR* &str					// Fetched string.
	)
{
str = 0;

if (!args)
	{
	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);		// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

bool ok = true;																// 10/30/00 AM.
switch (arg->getType())														// 10/30/00 AM.
	{
	case IASTR:					// Good one.								// 10/30/00 AM.
		str = arg->getStr();													// 10/30/00 AM.
		break;
	case IANUM:
		// allow num==0 to represent empty arg.						// 10/30/00 AM.
		if (arg->getNum())													// 10/30/00 AM.
			ok = false;															// 10/30/00 AM.
		break;
	default:
		ok = false;				// All other types are bad.			// 10/30/00 AM.
		break;
	}

if (!ok)																			// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[%s: Arg must be string.]"), errstr);			// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

return true;
}


/********************************************
* FN:		SEM1
* CR:		02/22/00 AM.
* SUBJ:	Get a sem arg.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
*			10/30/00 AM. Now returning sem==0 for empty arg.
********************************************/

bool Arg::sem1(
	_TCHAR *errstr,				// String for reporting errors.
	Nlppp *nlppp,
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	RFASem* &sem,				// Fetched sem.
	bool opt						// If optional arg.						// 05/04/03 AM.
	)
{
sem = 0;

if (!args)
	{
	if (!opt)																	// 05/04/03 AM.
		{
	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);		// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
		}
	return false;																// 05/04/03 AM.
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

bool ok = true;																// 10/30/00 AM.
switch (arg->getType())														// 10/30/00 AM.
	{
	case IASEM:					// Good one.								// 10/30/00 AM.
		sem = arg->getSem();													// 10/30/00 AM.
		break;
	case IANUM:
		// allow num==0 to represent empty arg.						// 10/30/00 AM.
		if (arg->getNum())													// 10/30/00 AM.
			ok = false;															// 10/30/00 AM.
		break;
	default:
		ok = false;				// All other types are bad.			// 10/30/00 AM.
		break;
	}
		
if (!ok)																			// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[%s: Arg must be sem object.]"), errstr);		// 05/18/01 AM.
	return nlppp->getParse()->errOut(false); // UNFIXED 							// 11/14/02 AM.
	}

return true;
}


/********************************************
* FN:		REF1
* CR:		05/26/02 AM.
* SUBJ:	Get a call-by-reference sem arg.
* RET:	True if got arg ok, else false.
********************************************/

bool Arg::ref1(
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	RFASem* &sem				// Fetched sem.
	)
{
sem = 0;

if (!args)
	{
	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);
	return errOut(false); // UNFIXED 
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

bool ok = true;
switch (arg->getType())
	{
	case IAREF:
		if ((sem = arg->getSem()))
			{
			Ivar *var = 0;
			if (sem->getType() != RSVAR)	// Must be a variable.
				ok = false;
			else if (!(var = sem->getVar()))
				ok = false;
			else if (*(var->getName()) == '$')	// Special var name.
				ok = false;
			}
		break;
	case IANUM:
		// allow num==0 to represent empty arg.
		if (arg->getNum())
			ok = false;
		break;
	default:
		ok = false;				// All other types are bad.
		break;
	}

if (!ok)
	{
	_stprintf(Errbuf,_T("[%s: Error in call-by-reference variable.]"), errstr);
	return errOut(false); // UNFIXED 
	}

return true;
}


/********************************************
* FN:		STR_OR_NUM1
* CR:		02/23/00 AM.
* SUBJ:	Get a string or numeric arg.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::str_or_num1(
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	_TCHAR* &str,					// Fetched string.
	long long &num					// Fetched num.
	)
{
str = 0;
num = -1;

if (!args)
	{
	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);		// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

switch (arg->getType())
	{
	case IASTR:
		// Fetch string.
		str = arg->getStr();
		break;
	case IANUM:
		num = arg->getNum();
		break;
	default:
		_stprintf(Errbuf,_T("[%s: Arg must be string or num.]"),			// 05/18/01 AM.
																	errstr);		// 05/18/01 AM.
		return errOut(false); // UNFIXED 												// 05/18/01 AM.
	}


return true;
}


/********************************************
* FN:		ANY1
* CR:		08/12/00 AM.
* SUBJ:	Get a string, numeric, or sem arg.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::any1(
	_TCHAR *errstr,				// String for reporting errors.
	Parse *parse,		// For error reporting.	// 12/26/06 AM.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	_TCHAR* &str,					// Fetched string.
	long long &num,					// Fetched num.
	float &fnum, // 12/15/14 AM.
	RFASem* &sem				// Fetched sem.
	)
{
str = 0;
num = -1;
fnum = -1.0; // 12/15/14 AM.
sem = 0;

if (!args)
	{
//	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);// 05/18/01 AM.
//	return errOut(false); // UNFIXED 									// 05/18/01 AM.
	std::_t_strstream gerrStr;													// 12/26/06 AM.
	gerrStr << _T("[") << errstr <<										// 12/26/06 AM.
			_T(": Couldn't get argument.]")								// 03/20/02 AM.
			<< std::ends;
	return parse->errOut(&gerrStr, false);								// 12/26/06 AM.
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

switch (arg->getType())
	{
	case IASTR:
		// Fetch string.
		str = arg->getStr();
		break;
	case IANUM:
		num = arg->getNum();
		break;
	case IAFLOAT:              // 12/15/14 AM.
		fnum = arg->getFloat(); // 12/15/14 AM.
		break;
	case IASEM:
		sem = arg->getSem();
		break;														// FIX.	// 09/07/00 AM.
	default:
		{
		// TREATING THIS AS AN EMPTY VALUE, RETURNING 0.			// 03/20/02 AM.
		num = 0;																	// 03/20/02 AM.
		std::_t_strstream gerrStr;												// 12/26/06 AM.
		gerrStr << _T("[") << errstr <<									// 12/26/06 AM.
			_T(": Warning: Arg is null.]")								// 03/20/02 AM.
			<< std::ends;
		parse->errOut(&gerrStr, false);									// 12/26/06 AM.
		}
	}


return true;
}


/********************************************
* FN:		ANY1
* CR:		05/29/02 AM.
* SUBJ:	Get a string, numeric, or sem arg.
* RET:	True if got arg ok, else false.
* NOTE:	VARIANT.  Overhauling to get float type, tell what type is returned,
*			get float, int, and str out of RFASem types.
*			Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::any1(
	_TCHAR *errstr,				// String for reporting errors.
	Parse *parse,		// For error reporting.	// 12/26/06 AM.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	enum Iargtype &typ,		// Type of data returned.
	_TCHAR* &str,					// Fetched string.
	long long &num,					// Fetched num.
	float &flt,					// Fetched float.
	RFASem* &sem				// Fetched sem.
	)
{
str = 0;
num = -1;
flt = -1.0;
sem = 0;
typ = IANULL;

if (!args)
	{
	std::_t_strstream gerrStr;													// 12/26/06 AM.
	gerrStr << _T("[") << errstr <<										// 12/26/06 AM.
			_T(": Couldn't get argument.]")								// 12/26/06 AM.
			<< std::ends;
	return parse->errOut(&gerrStr, false);								// 12/26/06 AM.
	}

Iarg *arg;
arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

switch (arg->getType())
	{
	case IASTR:
		// Fetch string.
		str = arg->getStr();
		typ = IASTR;
		break;
	case IANUM:
		num = arg->getNum();
		typ = IANUM;
		break;
	case IAFLOAT:
		flt = arg->getFloat();
		typ = IAFLOAT;
		break;
	case IASEM:
		sem = arg->getSem();
		if (!sem)
			{
			typ = IASEM;
			return true;
			}
		switch (sem->getType())
			{
			case RSSTR:
			case RSNAME:
				typ = IASTR;
				str = sem->getName();
				sem = 0;
				break;
			case RSLONG:
				typ = IANUM;
				num = sem->getLong();
				sem = 0;
				break;
			case RSFLOAT:
				typ = IAFLOAT;
				flt = sem->getFloat();
				sem = 0;
				break;
			default:
				typ = IASEM;
				break;
			}
		break;
	default:
		{
		// TREATING THIS AS AN EMPTY VALUE, RETURNING 0.
		num = 0;
		std::_t_strstream gerrStr;												// 12/26/06 AM.
		gerrStr << _T("[") << errstr <<													// 12/26/06 AM.
			_T(": Warning: Arg is null.]")							// 03/20/02 AM.
			<< std::ends;
		parse->errOut(&gerrStr, false);									// 12/26/06 AM.
		}
	}

return true;
}


/********************************************
* FN:		ARG1
* CR:		12/07/98 AM.
* SUBJ:	Get the arg itself.
* RET:	True if got arg ok, else false.
* NOTE:	Setting this up for repeated calls, each to fetch one arg.
********************************************/

bool Arg::arg1(
	_TCHAR *errstr,				// String for reporting errors.
	/*DU*/
	DELTS* &args,		// Remaining arguments in list.
	Iarg* &arg					// Fetched arg.
	)
{
arg = 0;

if (!args)
	{
	_stprintf(Errbuf,_T("[%s: Couldn't get argument.]"), errstr);		// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

arg = ((Delt<Iarg>*)args)->getData();
args = ((Delt<Iarg>*)args)->Right();		// Set up for next fetch of argument.

return true;
}

/*************************      END OF FILE      ******************************/
