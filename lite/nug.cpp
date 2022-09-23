/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NUG.CPP
* FILE:	lite/nug.cpp
* CR:		12/27/99 AM.
* SUBJ:	NLPPP Generation utilities.
* NOTE:	"NUG" data type equates to an RFASem object.
*
*******************************************************************************/


#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"
#include "node.h"	// 07/07/03 AM.
#include "lite/Auser.h"		// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "string.h"
#include "tree.h"		// 02/26/01 AM.
#include "pn.h"					// 02/26/01 AM.
#include "iexpr.h"
//#include "lite/node.h"			// 10/18/00 AM.
#include "ivar.h"
#include "iop.h"
#include "iexprstmt.h"
#include "iifstmt.h"

#include "rfasem.h"
#include "arg.h"
#include "lite/nlppp.h"

#include "nug.h"


/********************************************
* FN:		MAKEVAR
* CR:		12/27/99 AM.
* SUBJ:	Build a variable data structure.
*********************************************/

NUG *Nug::makeVar(
	_TCHAR *s_typ,			// "G", "S", "N", "X".
	_TCHAR *name,			// Variable name.  eg, "$text".
	int ord
	)
{
if (!s_typ || !name)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[makeVar: Null inputs.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}


enum Ivartype typ;
typ = Ivar::Vartype(*s_typ);		// Use first letter of typestring.
if (typ == NULLVAR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[makeVar: Bad variable type=") << s_typ << _T("]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

RFASem *rfasem;

Ivar *var;				// Object to build.
var = new Ivar(typ, name, ord);
rfasem = new RFASem(var);
return (NUG *) rfasem;
}


/********************************************
* FN:		MAKEASSIGN
* CR:		12/27/99 AM.
* SUBJ:	Build an assignment expression.
*********************************************/

NUG *Nug::makeAssign(
	NUG *lhs,
	NUG *rhs
	)
{
if (!lhs || !rhs)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[makeAssign: Null inputs.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

RFASem *rfasem;
Iexpr *expr;				// Object to build.
Iop *iop = new Iop(ASSIGNOP);
expr = new Iexpr(ASSIGNEXPR, (RFASem *)lhs, iop, (RFASem *)rhs);
rfasem = new RFASem(expr);
return (NUG *) rfasem;
}

/********************************************
* FN:		MAKEEXPRSTMT
* CR:		12/27/99 AM.
* SUBJ:	Build an expression statement.
* NOTE:	Basically an expression followed by a semicolon.
*********************************************/

NUG *Nug::makeExprstmt(
	NUG *expr
	)
{
if (!expr)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[makeExprstmt: Null input.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

RFASem *rfasem;
Iexprstmt *stmt;				// Object to build.
stmt = new Iexprstmt((RFASem *)expr);
rfasem = new RFASem(stmt);
return (NUG *) rfasem;
}



/********************************************
* FN:		MAKEINC
* CR:		11/27/00 AM.
* SUBJ:	Build an increment operation.
*********************************************/

NUG *Nug::makeInc(
	NUG *var
	)
{
if (!var)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[makeInc: Null inputs.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

RFASem *rfasem;
Iexpr *expr;				// Object to build.
Iop *iop = new Iop(INCOP);
expr = new Iexpr(UNARYEXPR, 0, iop, (RFASem *)var);
rfasem = new RFASem(expr);
return (NUG *) rfasem;
}
