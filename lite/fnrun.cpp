/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	RUN.CPP
* FILE:	lite/fnrun.cpp
* CR:		06/02/00 AM.
* SUBJ:	Compiled runtime variants of NLP++ functions.
* NOTE:	Separate file for modularity, but putting these into the Arun class
*			so they'll be visible to the compiled code.
*
*******************************************************************************/


#include "StdAfx.h"
#ifdef LINUX
#ifdef _ODBC
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>
#endif
#else
#include <direct.h>	// For _mkdir										// 06/11/03 AM.

#endif
#include <limits.h>
#include <math.h>		// For log10, etc.								// 04/29/04 AM.
#include "machine.h"				// 01/09/01 AM.
#include "lite/lite.h"		// MOVE UP	// 03/23/19 AM.
#include "lite/global.h"	// MOVE UP	// 03/23/19 AM.
//#include "NativeInterface.h"	// 05/19/14 DDH.
#include "u_out.h"		// 01/19/06 AM.
#include "consh/libconsh.h"	// 02/14/01 AM.
#include "consh/cg.h"			// 02/14/01 AM.

#include "htab.h"					// 02/12/07 AM.
#include "kb.h"					// 02/12/07 AM.

#include "prim/libprim.h"	// 09/15/08 AM.
#ifndef DWORD	// 09/16/20 AM.
#define DWORD double
#endif
LIBPRIM_API DWORD run_silent(_TCHAR* strCMD);	// 09/15/08 AM.

//#include "lite/lite.h"				// 07/07/03 AM.
//#include "lite/global.h"
#include "lite/mach.h"			// 02/08/01 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "lite/Auser.h"			// 07/07/03 AM.
#include "lite/iarg.h"			// 05/14/03 AM.
#include "tree.h"			// 02/26/01 AM.
#include "inline.h"
#include "str.h"
#include "io.h"
#include "string.h"
#include "words/words.h"		// 01/09/01 AM.
#include "words/wordarrs.h"	// 01/09/01 AM.

#include "pn.h"					// 02/26/01 AM.
#include "ana.h"
#include "parse.h"
#include "lite/nlp.h"			// 05/06/02 AM.
#include "lite/nlppp.h"
#include "rfasem.h"
#include "arg.h"
#include "ivar.h"					// 01/08/01 AM.

#include "chars.h"				// 01/06/01 AM.
#include "fn.h"			// For unpackdirs	// 01/09/01 AM.
#include "var.h"			// For pncopyvars	// 05/16/01 AM.
#include "ipair.h"		// For deleting locals_	// 03/11/02 AM.
#include "xml.h"					// 05/10/03 AM.
#ifndef LINUX
#include "LexTagger.h"			// 12/14/03 AM.


//#include <vector>	// 05/06/14 DDH.
//#include <wininet.h>	// 05/06/14 DDH.
//#include <process.h>	// 05/19/14 DDH.
//#include <atlstr.h>	// 05/19/14 DDH.
//#include "web.h"	// 05/06/14 DDH.
//using namespace openutils;	// 05/06/14 DDH.

#endif

#include "lite/vtrun.h"			// 05/29/03 AM.
#include "lite/Arun.h"


/********************************************
* FN:		FACTORIAL
* CR:		06/02/00 AM.
* SUBJ:	Factorial function.
* NOTE:	VARIANT. Compiled runtime.
*********************************************/

long Arun::factorial(
	Nlppp *nlppp,
	long num1
	)
{
long fact = -1;

if (num1 < 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[fnFactorial: Given bad value.]") << ends;
	errOut(&gerrStr,false);
	return 0;														// FIX.	// 08/11/02 AM.
	}
else
	{
	fact = 1;	// Value is good for the 0 and 1 cases.
	while (num1 > 1)
		{
		if ((double)fact > ((double)LONG_MAX / (double)num1))
			{
			// Overflow.
			_t_strstream gerrStr;
			gerrStr << _T("[factorial: Overflow.]") << ends;
			errOut(&gerrStr,false);
			return 0;												// FIX.	// 08/11/02 AM.
			}
		fact *= num1--;
		}
	}
return fact;
}

long Arun::factorial(
	Nlppp *nlppp,
	RFASem *num1_sem
	)
{
if (!num1_sem)
	return 0;														// FIX.	// 08/11/02 AM.
bool ok = false;
long num1 = num1_sem->sem_to_long(ok);
delete num1_sem;
if (!ok)
	return 0;														// FIX.	// 08/11/02 AM.
return factorial(nlppp,num1);
}


/********************************************
* FN:		FINDROOT
* CR:		06/02/00 AM.
* SUBJ:	Get root concept of KB tree.
* RET:	True if ok, else false.
*			UP - sem for root concept.
* FORM:	findroot()
********************************************/

RFASem *Arun::findroot(
	Nlppp *nlppp
	)
{
// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

CONCEPT *root = cg->findRoot();

return new RFASem(root, RS_KBCONCEPT);
}


/********************************************
* FN:		FINDCONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Find concept under given parent.
* RET:	True if ok, else false.
*			UP - child concept.
* FORMS:	findconcept(parent, namestr)
*			findconcept(parent, num)
* NOTE:	Handling TWO VARIANTS here.
********************************************/

RFASem *Arun::findconcept(
	Nlppp *nlppp,
	RFASem *sem,
	_TCHAR *name
	)
{
if (!name || !*name || !sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();
CONCEPT *child = cg->findConcept(conc, name);

delete sem;													// MEM LEAK.	// 06/27/00 AM.
return new RFASem(child, RS_KBCONCEPT);
}

// VARIANT.
RFASem *Arun::findconcept(
	Nlppp *nlppp,
	RFASem *sem,
	long num
	)
{
if (num <= 0 || !sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = sem->getKBconcept();
CONCEPT *child = cg->findConcept(conc1, num);

delete sem;													// MEM LEAK.	// 06/27/00 AM.
return new RFASem(child, RS_KBCONCEPT);
}

// VARIANT (AMBIGUOUS)
RFASem *Arun::findconcept(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *meth_sem	// Method for finding concept.
	)
{
if (!meth_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}

long num = 0;
_TCHAR *name = 0;
switch (meth_sem->getType())
	{
	case RSLONG:
		num = meth_sem->getLong();
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		name = meth_sem->getName();
		break;
	default:
		delete meth_sem;
		if (sem)
			delete sem;
		return 0;
		break;
	}
delete meth_sem;
if (name)
	return findconcept(nlppp,sem,name);
else
	return findconcept(nlppp,sem,num);
}


/********************************************
* FN:		FINDATTR
* CR:		06/02/00 AM.
* SUBJ:	Find concept's attribute.
* RET:	True if ok, else false.
*			UP - attr.
* FORMS:	findattr(conc, namestr)
********************************************/

RFASem *Arun::findattr(
	Nlppp *nlppp,
	RFASem *sem,
	_TCHAR *name
	)
{
if (!name || !*name || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findattr: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();
ATTR *attr = 0;

attr = cg->findAttr(conc, name);

delete sem;													// MEM LEAK.	// 06/27/00 AM.
return new RFASem(attr, RS_KBATTR);
}

RFASem *Arun::findattr(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return findattr(nlppp,sem,name);
}


/********************************************
* FN:		FINDATTRS
* CR:		06/02/00 AM.
* SUBJ:	Find concept's attributes.
* RET:	True if ok, else false.
*			UP - attrs.
* FORMS:	findattrs(conc)
********************************************/

RFASem *Arun::findattrs(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findattr: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();
ATTR *attr = 0;

attr = cg->findAttrs(conc);

delete sem;													// MEM LEAK.	// 06/27/00 AM.
return new RFASem(attr, RS_KBATTR);
}


/********************************************
* FN:		ATTRNAME
* CR:		06/02/00 AM.
* SUBJ:	Get attribute's name.
* RET:	True if ok, else false.
*			UP - name
* FORMS:	attrname(attr)
* NOTE:	Not totally analogous to cg.cpp function.
********************************************/

_TCHAR *Arun::attrname(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBATTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[attrname: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
ATTR *attr = sem->getKBattr();

// Get attr name from kb.
_TCHAR buf[NAMESIZ];
cg->attrName(attr, buf, NAMESIZ);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	{
	delete sem;														// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.
	}

// Internalize in analyzer's string table.
_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as string type.
delete sem;													// MEM LEAK.	// 06/27/00 AM.
return str;
}


/********************************************
* FN:		ATTRVALS
* CR:		06/02/00 AM.
* SUBJ:	Get attribute's values.
* RET:	True if ok, else false.
*			UP - values of attribute
* FORMS:	attrvals(attr)
********************************************/

RFASem *Arun::attrvals(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBATTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[attrname: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
ATTR *attr = sem->getKBattr();

// Get attr name from kb.
VAL *vals = cg->attrVals(attr);

// Return as val type.
delete sem;													// MEM LEAK.	// 06/28/00 AM.
return new RFASem(vals, RS_KBVAL);
}


/********************************************
* FN:		FINDVALS
* CR:		06/02/00 AM.
* SUBJ:	Fetch attribute values.
* RET:	True if ok, else false.
*			UP attrs - list of attribute values.
* FORM:	findvals(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

RFASem *Arun::findvals(
	Nlppp *nlppp,
	RFASem *sem,		// Concept.
	_TCHAR *name			// Attr name.
	)
{
if (!sem || !name || !*name || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findvals: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

VAL *vals = cg->findVals(conc, name);

// Return as val type.
delete sem;													// MEM LEAK.	// 06/28/00 AM.
return new RFASem(vals, RS_KBVAL);
}

RFASem *Arun::findvals(
	Nlppp *nlppp,
	RFASem *sem,				// Concept.
	RFASem *name_sem			// Attr name.
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return findvals(nlppp,sem,name);
}


/********************************************
* FN:		NUMVAL
* CR:		06/02/00 AM.
* SUBJ:	Fetch numeric attribute value.
* RET:	True if ok, else false.
*			UP num - numeric value of attribute.
* FORM:	numval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

long Arun::numval(
	Nlppp *nlppp,
	RFASem *sem,		// Concept.
	_TCHAR *name			// Attribute name.
	)
{
if (!sem || !name || !*name || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;														// FIX.	// 08/11/02 AM.
	}

if (sem->getType() == RSLONG && sem->getLong() == 0)	// 05/15/08 AM.
	{
	_t_strstream gerrStr;	// 05/15/08 AM.
	gerrStr << _T("[numval: Given no concept.]") << ends;	// 05/15/08 AM.
	errOut(&gerrStr,false);	// 05/15/08 AM.
	delete sem;	// 05/15/08 AM.
	// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
	nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
	throw ex_EXITPASS;	// 04/15/10 AM.
	return 0;	// 05/15/08 AM.
	}

if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[numval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;														// FIX.	// 08/11/02 AM.
	}
CONCEPT *conc1 = sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

long val = -1;
if (!cg->findVal(conc1, name, /*UP*/ val))
	{
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;			// This is ok, actually.			// FIX.	// 08/11/02 AM.
	}

delete sem;													// MEM LEAK.	// 06/28/00 AM.
return val;
}

// VARIANT.
long Arun::numval(															// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (sem)															// FIX.	// 05/06/01 AM.
		delete sem;													// FIX.	// 05/06/01 AM.
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return numval(nlppp,sem,name);
}



/********************************************
* FN:		FLTVAL
* CR:		12/27/06 AM.
* SUBJ:	Fetch float attribute value.
* RET:	True if ok, else false.
*			UP flt - float value of attribute.
* FORM:	fltval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

float Arun::fltval(
	Nlppp *nlppp,
	RFASem *sem,			// Concept.
	_TCHAR *name			// Attribute name.
	)
{
if (!sem || !name || !*name || !nlppp)
	{
	if (sem)
		delete sem;
	return 0;
	}

if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[fltval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}
CONCEPT *conc1 = sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

float val = 0.0;
if (!cg->findVal(conc1, name, /*UP*/ val))
	{
	delete sem;
	return 0;
	}

delete sem;
return val;
}

// VARIANT.
float Arun::fltval(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return fltval(nlppp,sem,name);
}


/********************************************
* FN:		STRVAL
* CR:		06/02/00 AM.
* SUBJ:	Fetch numeric attribute value.
* RET:	True if ok, else false.
*			UP str - string value of attribute.
* FORM:	strval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

_TCHAR *Arun::strval(
	Nlppp *nlppp,
	RFASem *sem,		// Concept.
	_TCHAR *name			// Attr name
	)
{
if (!sem || !name || !*name || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();
Htab *ht = parse->getHtab();	// 02/12/07 AM.


//char buf[MAXSTR];															// 11/12/00 AM.
//if (!cg->findVal(conc, name, /*UP*/ buf))				// OPT.	// 11/12/00 AM.
//str = cg->strVal(conc,name);								// OPT.	// 11/12/00 AM.
_TCHAR *str = KB::strVal(conc,name,cg,ht);							// 02/12/07 AM.
if (!str || !*str)												// FIX.	// 02/09/01 AM.
	{
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

//parse->internStr(buf, /*UP*/ str);						// OPT.	// 11/12/00 AM.

delete sem;													// MEM LEAK.	// 06/28/00 AM.
return str;
}

_TCHAR *Arun::strval(
	Nlppp *nlppp,
	RFASem *sem,		// Concept.
	RFASem *name_sem			// Attr name
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strval(nlppp,sem,name);
}


/********************************************
* FN:		CONVAL
* CR:		08/12/00 AM.
* SUBJ:	Fetch concept attribute value.
* RET:	True if ok, else false.
*			UP con - concept value of attribute.
* FORM:	conval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

RFASem *Arun::conval(
	Nlppp *nlppp,
	RFASem *sem,		// Concept.
	_TCHAR *name			// Attribute name.
	)
{
if (!sem || !name || !*name || !nlppp)
	{
	if (sem)
		delete sem;
	return 0;
	}

if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[conval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

CONCEPT *val = 0;
if (!cg->findVal(conc1, name, /*UP*/ val))
	{
	delete sem;
	return 0;
	}

delete sem;
return new RFASem(val, RS_KBCONCEPT);
}

RFASem *Arun::conval(
	Nlppp *nlppp,
	RFASem *sem,				// Concept.
	RFASem *name_sem			// Attr name
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return conval(nlppp,sem,name);
}


/********************************************
* FN:		INHERITVAL
* CR:		06/02/00 AM.
* SUBJ:	Fetch str attribute value up the hierarchy.
* RET:	True if ok, else false.
*			UP str - string value of attribute.
* FORM:	inheritval(concept, attributename, hierconcept)
	hier is the top of the concept hierarchy to be searched.
	hier = 0 means search up to root of the entire tree.
********************************************/

_TCHAR *Arun::inheritval(
	Nlppp *nlppp,
	RFASem *csem,		// Concept.
	_TCHAR *name,			// Attr name
	RFASem *hsem		// Hier concept.
	)
{
if (!csem || !name || !*name || !hsem || !nlppp)
	{
	if (csem)
		delete csem;
	if (hsem)
		delete hsem;
	return 0;
	}

if (csem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[inheritval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete csem;											// MEM LEAK.	// 06/27/00 AM.
	delete hsem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = csem->getKBconcept();

if (hsem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[inheritval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete csem;											// MEM LEAK.	// 06/27/00 AM.
	delete hsem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *hier = hsem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

_TCHAR buf[MAXSTR];
if (!cg->inheritVal(conc, name, hier, /*UP*/ buf))
	{
	delete csem;											// MEM LEAK.	// 06/27/00 AM.
	delete hsem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
if (!buf[0])														// FIX.	// 02/09/01 AM.
	{
	delete csem;													// FIX.	// 02/09/01 AM.
	delete hsem;													// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.
	}

_TCHAR *str = 0;
parse->internStr(buf, /*UP*/ str);

delete csem;												// MEM LEAK.	// 06/27/00 AM.
delete hsem;												// MEM LEAK.	// 06/27/00 AM.
return str;
}

_TCHAR *Arun::inheritval(
	Nlppp *nlppp,
	RFASem *csem,		// Concept.
	RFASem *name_sem,			// Attr name
	RFASem *hsem		// Hier concept.
	)
{
if (!name_sem)
	{
	if (csem)
		delete csem;
	if (hsem)
		delete hsem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return inheritval(nlppp,csem,name,hsem);
}

/********************************************
* FN:		CONCEPTNAME
* CR:		06/02/00 AM.
* SUBJ:	Get name of given concept.
* RET:	True if ok, else false.
*			UP - name of concept.
* FORM:	conceptname(concept)
********************************************/

_TCHAR *Arun::conceptname(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[conceptname: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();

_TCHAR buf[MAXSTR];
_TCHAR *str;
cg->conceptName(conc, /*UP*/ buf);

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

delete sem;													// MEM LEAK.	// 06/28/00 AM.
return str;
}


/********************************************
* FN:		CONCEPTPATH
* CR:		06/02/00 AM.
* SUBJ:	Get path of given concept.
* RET:	True if ok, else false.
*			UP str - path of concept.
* FORM:	conceptpath(concept)
********************************************/

_TCHAR *Arun::conceptpath(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem || !nlppp)
	{
	if (sem)
		delete sem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[conceptpath: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;												// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();

_TCHAR buf[PATHSIZ];
_TCHAR *str;
cg->conceptPath(conc, /*UP*/ buf);

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

delete sem;													// MEM LEAK.	// 06/28/00 AM.
return str;
}


/********************************************
* FN:		WORDPATH
* CR:		06/02/00 AM.
* SUBJ:	Get word-concept path of given string.
* RET:	True if ok, else false.
*			UP str - path of concept.
* FORM:	wordpath(str)
********************************************/

_TCHAR *Arun::wordpath(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1 || !nlppp)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();


_TCHAR buf[PATHSIZ];
_TCHAR *str;
cg->wordPath(str1, /*UP*/ buf);

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

return str;
}

_TCHAR *Arun::wordpath(
	Nlppp *nlppp,
	RFASem *str1_sem
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return wordpath(nlppp,str1);
}


/********************************************
* FN:		FINDWORDPATH
* CR:		06/02/00 AM.
* SUBJ:	Find word-concept path of given string.
* RET:	True if ok, else false.
*			UP str - path of concept.
* FORM:	findwordpath(str)
* NOTE:	Unlike wordpath, doesn't add word if not already present.
********************************************/

_TCHAR *Arun::findwordpath(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1 || !nlppp)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();


_TCHAR buf[PATHSIZ];
_TCHAR *str;
cg->findWordpath(str1, /*UP*/ buf);
if (!*buf)															// FIX.	// 09/07/00 AM.
	return 0;														// FIX.	// 09/07/00 AM.

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

return str;
}

_TCHAR *Arun::findwordpath(
	Nlppp *nlppp,
	RFASem *str1_sem
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return findwordpath(nlppp,str1);
}

/********************************************
* FN:		WORDINDEX
* CR:		06/02/00 AM.
* SUBJ:	Get index concept for given string.
* RET:	True if ok, else false.
*			UP index - index concept for placing word.
* FORM:	wordindex(str)
********************************************/

RFASem *Arun::wordindex(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1 || !nlppp)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();


CONCEPT *conc = cg->wordIndex(str1);

return new RFASem(conc, RS_KBCONCEPT);
}


RFASem *Arun::wordindex(
	Nlppp *nlppp,
	RFASem *str1_sem
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return wordindex(nlppp,str1);
}


/********************************************
* FN:		PATHCONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Get concept for given path.
* RET:	True if ok, else false.
*			UP con - concept
* FORM:	pathconcept(path)
********************************************/

RFASem *Arun::pathconcept(
	Nlppp *nlppp,
	_TCHAR *path
	)
{
if (!path || !*path || !nlppp)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *con = cg->pathConcept(path);

return new RFASem(con, RS_KBCONCEPT);
}


RFASem *Arun::pathconcept(
	Nlppp *nlppp,
	RFASem *path_sem
	)
{
if (!path_sem)
	return 0;
_TCHAR *path = path_sem->sem_to_str();
delete path_sem;
return pathconcept(nlppp,path);
}


/********************************************
* FN:		FINDHIERCONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Find name in a subhierarchy.
* RET:	True if ok, else false.
*			UP conc - concept found.
* FORM:	findhierconcept(name, hierconcept)
	Find hierarchy concept.  Given a name, look through the given subtree
	for it.  Also look through node names.  If a node, return its owning
	concept.  If hierconcept == 0, start at root of concept tree.
********************************************/

RFASem *Arun::findhierconcept(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *hsem
	)
{
if (!name || !*name || !hsem || !nlppp)
	{
	if (hsem)
		delete hsem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (hsem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findhierconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hsem;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = hsem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->findHierConcept(name, conc1);
//	return false;	// FIX. // 06/02/00 AM.

// Return appropriate value.
delete hsem;												// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


// VARIANT.
RFASem *Arun::findhierconcept(											// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *hsem
	)
{
if (!name_sem)
	{
	if (hsem)														// FIX.	// 05/06/01 AM.
		delete hsem;												// FIX.	// 05/06/01 AM.
	return 0;
	}
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
return findhierconcept(nlppp,name_str,hsem);	// WRONG ARG.	// FIX.	// 07/02/09 AM.
}

/********************************************
* FN:		ATTREXISTS
* CR:		06/02/00 AM.
* SUBJ:	Search for given attr and value in given subtree.
* RET:	True if ok, else false.
*			UP bool - 1 if found, 0 if not.
* FORM:	attrexists(hier_conc, attr_str, val_str)
	Find hierarchy concept.  Given a name, look through the given subtree
	for it.  Also look through node names.  If a node, return its owning
	concept.  If hierconcept == 0, start at root of concept tree.
********************************************/

bool Arun::attrexists(
	Nlppp *nlppp,
	RFASem *hsem,
	_TCHAR *name,
	_TCHAR *val_str
	)
{
if (!hsem || !name || !*name || !val_str || !*val_str || !nlppp)
	{
	if (hsem)
		delete hsem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (hsem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[attrexists: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hsem;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *hier1 = hsem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

delete hsem;												// MEM LEAK.	// 06/27/00 AM.
return cg->attrExists(hier1, name, val_str);
}


bool Arun::attrexists(
	Nlppp *nlppp,
	RFASem *hsem,
	RFASem *name_sem,
	_TCHAR *val_str
	)
{
if (!name_sem)
	{
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return attrexists(nlppp,hsem,name,val_str);
}


bool Arun::attrexists(
	Nlppp *nlppp,
	RFASem *hsem,
	_TCHAR *name,
	RFASem *val_str_sem
	)
{
if (!val_str_sem)
	{
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *val_str = val_str_sem->sem_to_str();
delete val_str_sem;
return attrexists(nlppp,hsem,name,val_str);
}


bool Arun::attrexists(
	Nlppp *nlppp,
	RFASem *hsem,
	RFASem *name_sem,
	RFASem *val_str_sem
	)
{
if (!val_str_sem)
	{
	if (hsem)
		delete hsem;
	if (name_sem)
		delete name_sem;
	return false;
	}
if (!name_sem)
	{
	delete val_str_sem;
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
_TCHAR *val_str = val_str_sem->sem_to_str();
delete name_sem;
delete val_str_sem;
return attrexists(nlppp,hsem,name,val_str);
}


/********************************************
* FN:		ATTRWITHVAL
* CR:		06/02/00 AM.
* SUBJ:	See if concept's attr has given val.
* RET:	True if ok.
*			UP bool - 1 if found, 0 if not.
* FORM:	attrwithval(conc, attr_str, val_str)
********************************************/

bool Arun::attrwithval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	_TCHAR *val_str
	)
{
if (!attr_str || !*attr_str || !val_str || !*val_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[attrwithval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return cg->attrWithval(conc1, attr_str, val_str);
}


bool Arun::attrwithval(
	Nlppp *nlppp,
	RFASem *csem,
	RFASem *name_sem,
	_TCHAR *val_str
	)
{
if (!name_sem)
	{
	if (csem)
		delete csem;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return attrwithval(nlppp,csem,name,val_str);
}


bool Arun::attrwithval(
	Nlppp *nlppp,
	RFASem *csem,
	_TCHAR *name,
	RFASem *val_str_sem
	)
{
if (!val_str_sem)
	{
	if (csem)
		delete csem;
	return false;
	}
_TCHAR *val_str = val_str_sem->sem_to_str();
delete val_str_sem;
return attrwithval(nlppp,csem,name,val_str);
}


bool Arun::attrwithval(
	Nlppp *nlppp,
	RFASem *csem,
	RFASem *name_sem,
	RFASem *val_str_sem
	)
{
if (!val_str_sem)
	{
	if (csem)
		delete csem;
	if (name_sem)
		delete name_sem;
	return false;
	}
if (!name_sem)
	{
	delete val_str_sem;
	if (csem)
		delete csem;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
_TCHAR *val_str = val_str_sem->sem_to_str();
delete name_sem;
delete val_str_sem;
return attrwithval(nlppp,csem,name,val_str);
}


/********************************************
* FN:		ATTRCHANGE
* CR:		06/02/00 AM.
* SUBJ:	Change all values of attr to the new value, in a subtree.
* RET:	True if ok, else false.
*			UP
* FORM:	attrchange(hier_conc, attr_str, val_str, nval_str)
	hier_conc - subtree to examine.
	attr_str, val_str - existing attribute and value sought.
	nval_str - new value to substitute.
********************************************/

bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	_TCHAR *attr_str,
	_TCHAR *val_str,
	_TCHAR *nval_str
	)
{
if (!hsem || !attr_str || !*attr_str
	 || !val_str || !*val_str
	 || !nlppp)
	{
	if (hsem)
		delete hsem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (hsem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[attrchange: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hsem;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *hier1 = hsem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

delete hsem;												// MEM LEAK.	// 06/27/00 AM.
cg->attrChange(hier1, attr_str, val_str, nval_str);
return true;
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	RFASem *attr_str_sem,
	_TCHAR *val_str,
	_TCHAR *nval_str
	)
{
if (!attr_str_sem)
	{
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *attr_str = attr_str_sem->sem_to_str();
delete attr_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	_TCHAR *attr_str,
	RFASem *val_str_sem,
	_TCHAR *nval_str
	)
{
if (!val_str_sem)
	{
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *val_str = val_str_sem->sem_to_str();
delete val_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	_TCHAR *attr_str,
	_TCHAR *val_str,
	RFASem *nval_str_sem
	)
{
if (!nval_str_sem)
	{
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *nval_str = nval_str_sem->sem_to_str();
delete nval_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	RFASem *attr_str_sem,
	RFASem *val_str_sem,
	_TCHAR *nval_str
	)
{
if (!attr_str_sem)
	{
	if (hsem)
		delete hsem;
	if (val_str_sem)
		delete val_str_sem;
	return false;
	}
if (!val_str_sem)
	{
	delete attr_str_sem;
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *attr_str = attr_str_sem->sem_to_str();
_TCHAR *val_str  = val_str_sem->sem_to_str();
delete attr_str_sem;
delete val_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	RFASem *attr_str_sem,
	_TCHAR *val_str,
	RFASem *nval_str_sem
	)
{
if (!attr_str_sem)
	{
	if (hsem)
		delete hsem;
	if (nval_str_sem)
		delete nval_str_sem;
	return false;
	}
if (!nval_str_sem)
	{
	delete attr_str_sem;
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *attr_str = attr_str_sem->sem_to_str();
_TCHAR *nval_str = nval_str_sem->sem_to_str();
delete attr_str_sem;
delete nval_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	_TCHAR *attr_str,
	RFASem *val_str_sem,
	RFASem *nval_str_sem
	)
{
if (!val_str_sem)
	{
	if (hsem)
		delete hsem;
	if (nval_str_sem)
		delete nval_str_sem;
	return false;
	}
if (!nval_str_sem)
	{
	delete val_str_sem;
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *val_str = val_str_sem->sem_to_str();
_TCHAR *nval_str = nval_str_sem->sem_to_str();
delete val_str_sem;
delete nval_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


bool Arun::attrchange(
	Nlppp *nlppp,
	RFASem *hsem,
	RFASem *attr_str_sem,
	RFASem *val_str_sem,
	RFASem *nval_str_sem
	)
{
if (!attr_str_sem)
	{
	if (hsem)
		delete hsem;
	if (val_str_sem)
		delete val_str_sem;
	if (nval_str_sem)
		delete nval_str_sem;
	return false;
	}
if (!val_str_sem)
	{
	delete attr_str_sem;
	if (hsem)
		delete hsem;
	if (nval_str_sem)
		delete nval_str_sem;
	return false;
	}
if (!nval_str_sem)
	{
	delete attr_str_sem;
	delete val_str_sem;
	if (hsem)
		delete hsem;
	return false;
	}
_TCHAR *attr_str = attr_str_sem->sem_to_str();
_TCHAR *val_str = val_str_sem->sem_to_str();
_TCHAR *nval_str = nval_str_sem->sem_to_str();
delete attr_str_sem;
delete val_str_sem;
delete nval_str_sem;
return attrchange(nlppp,hsem,attr_str,val_str,nval_str);
}


/********************************************
* FN:		DOWN
* CR:		06/02/00 AM.
* SUBJ:	Follow down link of concept.
* RET:	True if ok, else false.
*			UP conc - child concept in hierarchy.
* FORM:	down(hierconcept)
********************************************/

RFASem *Arun::down(
	Nlppp *nlppp,
	RFASem *hier_sem
	)
{
if (!hier_sem || !nlppp)
	{
	if (hier_sem)
		delete hier_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[down: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hier_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Down(hier1);
//	return false;		// 06/02/00 AM.

// Return appropriate value.
delete hier_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


/********************************************
* FN:		UP
* CR:		06/02/00 AM.
* SUBJ:	Follow up link of concept.
* RET:	True if ok, else false.
*			UP conc - parent concept in hierarchy.
* FORM:	up(hierconcept)
********************************************/

RFASem *Arun::up(
	Nlppp *nlppp,
	RFASem *hier_sem
	)
{
if (!hier_sem || !nlppp)
	{
	if (hier_sem)
		delete hier_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[up: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hier_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Up(hier1);

// Return appropriate value.
delete hier_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


/********************************************
* FN:		NEXT
* CR:		06/02/00 AM.
* SUBJ:	Follow next link of concept.
* RET:	True if ok, else false.
*			UP conc - next concept in hierarchy.
* FORM:	next(hierconcept)
********************************************/

RFASem *Arun::next(
	Nlppp *nlppp,
	RFASem *hier_sem
	)
{
if (!hier_sem || !nlppp)
	{
	if (hier_sem)
		delete hier_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[next: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hier_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Next(hier1);
//	return false;				// 06/02/00 AM.

// Return appropriate value.
delete hier_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


/********************************************
* FN:		PREV
* CR:		06/02/00 AM.
* SUBJ:	Follow prev link of concept.
* RET:	True if ok, else false.
*			UP conc - prev concept in hierarchy.
* FORM:	prev(hierconcept)
********************************************/

RFASem *Arun::prev(
	Nlppp *nlppp,
	RFASem *hier_sem
	)
{
if (!hier_sem || !nlppp)
	{
	if (hier_sem)
		delete hier_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[prev: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete hier_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Prev(hier1);

// Return appropriate value.
delete hier_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


/********************************************
* FN:		NEXTATTR
* CR:		06/02/00 AM.
* SUBJ:	Follow next link in list of attributes.
* RET:	True if ok, else false.
*			UP attr - next in list.
* FORM:	nextattr(attr)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
********************************************/

RFASem *Arun::nextattr(
	Nlppp *nlppp,
	RFASem *attr_sem
	)
{
if (!attr_sem || !nlppp)
	{
	if (attr_sem)
		delete attr_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (attr_sem->getType() != RS_KBATTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[nextattr: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete attr_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
ATTR *attr = attr_sem->getKBattr();
if (!attr)
	{
	delete attr_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

cg->popAttr(/*DU*/ attr);	// Move to next attr in list.

// Return appropriate value.
delete attr_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(attr, RS_KBATTR);
}


/********************************************
* FN:		NEXTVAL
* CR:		06/02/00 AM.
* SUBJ:	Follow next link in list of attr values.
* RET:	True if ok, else false.
*			UP val - next value in list.
* FORM:	nextval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
********************************************/

RFASem *Arun::nextval(
	Nlppp *nlppp,
	RFASem *val_sem
	)
{
if (!val_sem || !nlppp)
	{
	if (val_sem)
		delete val_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[nextval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

val = cg->nextVal(val);	// Move to next val in list.

// Return appropriate value.
delete val_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(val, RS_KBVAL);
}


/********************************************
* FN:		GETSVAL
* CR:		06/02/00 AM.
* SUBJ:	Fetch or convert value to string.
* RET:	True if ok, else false.
*			UP str - Value converted to string.
* FORM:	getsval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popSval.
********************************************/

_TCHAR *Arun::getsval(
	Nlppp *nlppp,
	RFASem *val_sem
	)
{
if (!val_sem || !nlppp)
	{
	if (val_sem)
		delete val_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[getsval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

_TCHAR buf[MAXSTR];
cg->popSval(val, /*UP*/ buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	{
	delete val_sem;												// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.
	}

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return appropriate value.
delete val_sem;											// MEM LEAK.	// 06/27/00 AM.
return str;
}


/********************************************
* FN:		GETSTRVAL
* CR:		06/02/00 AM.
* SUBJ:	Fetch string value.
* RET:	True if ok, else false.
*			UP str - String obtained from value.
* FORM:	getstrval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popVal.  Differs from getsval in that no
*			conversion is done. Value must be string-valued.
********************************************/

_TCHAR *Arun::getstrval(
	Nlppp *nlppp,
	RFASem *val_sem
	)
{
if (!val_sem || !nlppp)
	{
	if (val_sem)
		delete val_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[getstrval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

_TCHAR buf[MAXSTR];
cg->popVal(val, /*UP*/ buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	{
	delete val_sem;												// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.
	}

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

delete val_sem;											// MEM LEAK.	// 06/27/00 AM.
return str;
}


/********************************************
* FN:		GETNUMVAL
* CR:		06/02/00 AM.
* SUBJ:	Fetch numeric value.
* RET:	True if ok, else false.
*			UP num - Long obtained from value.
* FORM:	getnumval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popVal.
********************************************/

long Arun::getnumval(
	Nlppp *nlppp,
	RFASem *val_sem
	)
{
if (!val_sem || !nlppp)
	{
	if (val_sem)
		delete val_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[getnumval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	delete val_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

long num;
cg->popVal(val, /*UP*/ num);

delete val_sem;											// MEM LEAK.	// 06/27/00 AM.
return num;
}


/********************************************
* FN:		GETCONVAL
* CR:		08/12/00 AM.
* SUBJ:	Fetch concept value.
* RET:	True if ok, else false.
*			UP con - Concept obtained from value.
* FORM:	getconval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popVal.
********************************************/

RFASem *Arun::getconval(
	Nlppp *nlppp,
	RFASem *val_sem
	)
{
if (!val_sem || !nlppp)
	{
	if (val_sem)
		delete val_sem;
	return 0;
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[getconval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete val_sem;
	return 0;
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	delete val_sem;
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc;
cg->popVal(val, /*UP*/ conc);

delete val_sem;
return new RFASem(conc, RS_KBCONCEPT);
}


/********************************************
* FN:		MAKECONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Make concept under parent.
* RET:	True if ok, else false.
*			UP conc - the new concept.
* FORM:	makeconcept(parent_conc, name_str [, pos_num])
*			parent_conc - the parent concept.
*			name_str - name of new concept.
*			pos_num - position in childs.  If absent or 0, placed at end of list.
********************************************/

RFASem *Arun::makeconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *name_str,
	long pos_num	/*OPTIONAL*/
	)
{
if (!con_sem || !name_str || !*name_str || pos_num < 0  || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[makeconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->makeConcept(conc1, name_str, pos_num);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


RFASem *Arun::makeconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *name_sem,
	long pos_num	/*OPTIONAL*/
	)
{
if (!name_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return makeconcept(nlppp,con_sem,name,pos_num);
}


RFASem *Arun::makeconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *name,
	RFASem *num_sem
	)
{
if (!num_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
return makeconcept(nlppp,con_sem,name,num);
}


RFASem *Arun::makeconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *name_sem,
	RFASem *num_sem
	)
{
if (!name_sem)
	{
	if (con_sem)
		delete con_sem;
	if (num_sem)
		delete num_sem;
	return 0;
	}
if (!num_sem)
	{
	delete name_sem;
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
return makeconcept(nlppp,con_sem,name,num);
}


/********************************************
* FN:		ADDCONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Add concept under parent.
* RET:	True if ok, else false.
*			UP conc - the new concept.
* FORM:	addconcept(parent_conc, child_conc [, pos_num])
*			parent_conc - the parent concept.
*			child_con - DETACHED concept to add.
*			pos_num - position in childs.  If absent or 0, placed at end of list.
* WARN:	DON'T USE THIS FOR A CONCEPT ALREADY IN THE HIERARCHY.
********************************************/

#ifdef DISABLED_
RFASem *Arun::addconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *child_sem,
	long pos_num		/*OPTIONAL*/
	)
{
if (!con_sem || !child_sem || pos_num < 0 || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	if (child_sem)
		delete child_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	delete child_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *parent = con_sem->getKBconcept();
if (child_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	delete child_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *child = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->addConcept(parent, child, pos_num);


delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
delete child_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}
#endif


/********************************************
* FN:		ADDATTR
* CR:		06/02/00 AM.
* SUBJ:	Add an empty attribute to concept.
* RET:	True if ok, else false.
*			UP attr - new attribute object with no values.
* FORM:	addattr(concept, attrstr)
* NOTE:	06/02/00 AM. Redoing this to conform to cg.h .
********************************************/

RFASem *Arun::addattr(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *name_str
	)
{
if (!con_sem || !name_str || !*name_str || !nlppp)
	{
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addattr: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

ATTR *attr = cg->addAttr(conc1, name_str);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(attr, RS_KBATTR);
}


RFASem *Arun::addattr(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return addattr(nlppp,con_sem,name);
}


/********************************************
* FN:		ADDSVAL
* CR:		06/02/00 AM.
* SUBJ:	Add num value as a STRING to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addsval(concept, attrname, num)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Arun::addsval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	long num
	)
{
if (!con_sem || !attr_str || !*attr_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addsval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

cg->addSval(conc1, attr_str, num);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return true;
}


bool Arun::addsval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	long num
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return addsval(nlppp,con_sem,attr_str,num);
}


bool Arun::addsval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *num_sem
	)
{
if (!num_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
return addsval(nlppp,con_sem,attr_str,num);
}


bool Arun::addsval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *num_sem
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	if (num_sem)
		delete num_sem;
	return false;
	}
if (!num_sem)
	{
	delete attr_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
return addsval(nlppp,con_sem,attr_str,num);
}

/********************************************
* FN:		ADDSTRVAL
* CR:		06/02/00 AM.
* SUBJ:	Add string to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addstrval(concept, attrname, str)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Arun::addstrval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	_TCHAR *str
	)
{
if (!con_sem || !attr_str || !*attr_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
// Allowing str val to be null.

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addstrval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

cg->addVal(conc1, attr_str, str);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return true;
}

// VARIANT.
bool Arun::addstrval(														// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *val_sem
	)
{
if (!val_sem)
	{
	if (con_sem)													// FIX.	// 05/06/01 AM.
		delete con_sem;											// FIX.	// 05/06/01 AM.
	return false;																// 05/06/01 AM.
	}
_TCHAR *val = val_sem->sem_to_str();
delete val_sem;
return addstrval(nlppp,con_sem,attr_str,val);
}


bool Arun::addstrval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	_TCHAR *str
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return addstrval(nlppp,con_sem,attr_str,str);
}


bool Arun::addstrval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *str_sem
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	if (str_sem)
		delete str_sem;
	return false;
	}
if (!str_sem)
	{
	delete attr_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return addstrval(nlppp,con_sem,attr_str,str);
}


/********************************************
* FN:		ADDNUMVAL
* CR:		06/02/00 AM.
* SUBJ:	Add num value to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addnumval(concept, attrname, num)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Arun::addnumval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	long num
	)
{
if (!con_sem || !attr_str || !*attr_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addnumval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

cg->addVal(conc1, attr_str, num);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return true;
}


bool Arun::addnumval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	long num
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return addnumval(nlppp,con_sem,attr_str,num);
}


bool Arun::addnumval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *num_sem
	)
{
if (!num_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
return addnumval(nlppp,con_sem,attr_str,num);
}


bool Arun::addnumval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *num_sem
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	if (num_sem)
		delete num_sem;
	return false;
	}
if (!num_sem)
	{
	delete attr_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
return addnumval(nlppp,con_sem,attr_str,num);
}


/********************************************
* FN:		ADDCONVAL
* CR:		08/11/00 AM.
* SUBJ:	Add concept value to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addconval(concept, attrname, val_con)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Arun::addconval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *val_sem
	)
{
if (!con_sem || !attr_str || !*attr_str || !nlppp
	|| !val_sem)																// 05/06/01 AM.
	{
	if (con_sem)													// FIX.	// 05/06/01 AM.
		delete con_sem;											// FIX.	// 05/06/01 AM.
	if (val_sem)													// FIX.	// 05/06/01 AM.
		delete val_sem;											// FIX.	// 05/06/01 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	delete con_sem;												// FIX.	// 05/06/01 AM.
	if (val_sem)													// FIX.	// 05/06/01 AM.
		delete val_sem;											// FIX.	// 05/06/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[addconval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

if (val_sem->getType() == RSLONG											// 08/08/02 AM.
 && val_sem->getLong() == 0)												// 08/08/02 AM.
	{
	delete con_sem;															// 08/08/02 AM.
	delete val_sem;															// 08/08/02 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[addconval: Given no concept to add.]") << ends;	// 08/08/02 AM.
	errOut(&gerrStr,false);																// 08/08/02 AM.
	return false;																// 08/08/02 AM.
	}

if (val_sem->getType() != RS_KBCONCEPT)
	{
	delete con_sem;												// FIX.	// 05/06/01 AM.
	delete val_sem;												// FIX.	// 05/06/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[addconval: Bad semantic arg(2).]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
CONCEPT *val1 = val_sem->getKBconcept();

delete con_sem;													// FIX.	// 05/06/01 AM.
delete val_sem;													// FIX.	// 05/06/01 AM.

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

cg->addVal(conc1, attr_str, val1);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

return true;
}


bool Arun::addconval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *val_sem
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	if (val_sem)
		delete val_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return addconval(nlppp,con_sem,attr_str,val_sem);
}


/********************************************
* FN:		GETCONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Find or make concept under parent.
* RET:	True if ok, else false.
*			UP conc - the new concept.
* FORM:	getconcept(parent_conc, name_str)
*			parent_conc - the parent concept.
*			name_str - name of new concept.
********************************************/

RFASem *Arun::getconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *name_str
	)
{
if (!con_sem || !name_str || !*name_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[getconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->getConcept(conc1, name_str);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}

// VARIANT
RFASem *Arun::getconcept(													// 05/04/01 AM.
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
return getconcept(nlppp,con_sem,name_str);
}

/********************************************
* FN:		RMCONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Delete given concept.
* RET:	True if ok, else false.
* FORM:	rmconcept(conc)
********************************************/

bool Arun::rmconcept(
	Nlppp *nlppp,
	RFASem *con_sem
	)
{
if (!con_sem || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

cg->rmConcept(conc1);

delete con_sem;											// MEM LEAK.	// 06/27/00 AM.
return true;
}


/********************************************
* FN:		RMCHILD
* CR:		06/02/00 AM.
* SUBJ:	Remove concept under given parent.
* RET:	True if ok, else false.
* FORMS:	rmchild(parent, namestr)
*			rmchild(parent, num)
* NOTE:	Handling TWO VARIANTS here.
*			Differs from cg.h, which uses rmConcept for three variants.
********************************************/

bool Arun::rmchild(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *name1
	)
{
if (!sem1 || !name1 || !*name1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmchild: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();
CONCEPT *child = 0;

delete sem1;												// MEM LEAK.	// 06/27/00 AM.
return cg->rmConcept(conc1, name1);
}

bool Arun::rmchild(
	Nlppp *nlppp,
	RFASem *sem1,
	long num1
	)
{
if (!sem1 || num1 <= 0 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmchild: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();
CONCEPT *child = 0;

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmConcept(conc1, num1);
}


// VARIANT (AMBIG)
bool Arun::rmchild(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *meth_sem	// Method (name or num)
	)
{
if (!meth_sem)
	{
	if (sem)
		delete sem;
	return false;
	}

long num = 0;
_TCHAR *name = 0;
switch (meth_sem->getType())
	{
	case RSLONG:
		num = meth_sem->getLong();
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		name = meth_sem->getName();
		break;
	default:
		delete meth_sem;
		if (sem)
			delete sem;
		return false;
		break;
	}
delete meth_sem;
if (name)
	return rmchild(nlppp,sem,name);
else
	return rmchild(nlppp,sem,num);
}


/********************************************
* FN:		RMVALS
* CR:		06/02/00 AM.
* SUBJ:	Delete values of concept's attribute.
* RET:	True if ok, else false.
* FORMS:	rmvals(conc, namestr)
* NOTE:	RENAMED FROM RMVAL.  06/02/00 AM.
********************************************/

bool Arun::rmvals(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *name1
	)
{
if (!sem1 || !name1 || !*name1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmvals: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmVals(conc1, name1);
}


bool Arun::rmvals(
	Nlppp *nlppp,
	RFASem *sem1,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return rmvals(nlppp,sem1,name);
}

/********************************************
* FN:		RMVAL
* CR:		06/02/00 AM.
* SUBJ:	Delete value of concept's attribute.
* RET:	True if ok, else false.
* FORMS:	rmval(attr, val)
********************************************/

bool Arun::rmval(
	Nlppp *nlppp,
	RFASem *sem1,
	RFASem *sem2
	)
{
if (!sem1 || !sem2 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	if (sem2)
		delete sem2;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get attr from sem.
if (sem1->getType() != RS_KBATTR)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	delete sem2;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
ATTR *attr = sem1->getKBattr();

// Get val from sem.
if (sem2->getType() != RS_KBVAL)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	delete sem2;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
VAL *val = sem2->getKBval();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
delete sem2;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmVal(attr, val);
}


/********************************************
* FN:		RMATTR
* CR:		06/02/00 AM.
* SUBJ:	Delete concept's attribute.
* RET:	True if ok, else false.
* FORMS:	rmattr(conc, namestr)
* NOTE:	Not handling the form rmattr(conc, attr) yet.
********************************************/

bool Arun::rmattr(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *name1
	)
{
if (!sem1 || !name1 || !*name1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmattr: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmAttr(conc1, name1);
}


bool Arun::rmattr(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return false;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return rmattr(nlppp,sem,name);
}


/********************************************
* FN:		RMATTRVAL
* CR:		06/02/00 AM.
* SUBJ:	Delete concept's attribute value.
* RET:	True if ok, else false.
* FORMS:	rmattr(conc, attr_s, val_s)
********************************************/

bool Arun::rmattrval(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *attr_s,
	_TCHAR *val_s
	)
{
if (!sem1 || !attr_s || !*attr_s || !val_s || !*val_s)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmattrval: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmAttrval(conc, attr_s, val_s);
}


bool Arun::rmattrval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *val_sem
	)
{
if (!val_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *val = val_sem->sem_to_str();
delete val_sem;
return rmattrval(nlppp,con_sem,attr_str,val);
}


bool Arun::rmattrval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	_TCHAR *str
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return rmattrval(nlppp,con_sem,attr_str,str);
}


bool Arun::rmattrval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *str_sem
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	if (str_sem)
		delete str_sem;
	return false;
	}
if (!str_sem)
	{
	delete attr_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return rmattrval(nlppp,con_sem,attr_str,str);
}


/********************************************
* FN:		RMCHILDREN
* CR:		06/02/00 AM.
* SUBJ:	Delete given concept's children AND PHRASE.
* RET:	True if ok, else false.
* FORM:	rmchildren(conc)
********************************************/

bool Arun::rmchildren(
	Nlppp *nlppp,
	RFASem *con_sem
	)
{
if (!con_sem || !nlppp)
	return false;

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmchildren: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

cg->rmChildren(conc1);

delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return true;
}


/********************************************
* FN:		PRUNEPHRASES
* CR:		06/02/00 AM.
* SUBJ:	Remove all phrases from given subtree.
* RET:	True if ok, else false.
* FORM:	prunephrases(conc)
********************************************/

bool Arun::prunephrases(
	Nlppp *nlppp,
	RFASem *con_sem
	)
{
if (!con_sem || !nlppp)
	return false;

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[prunephrases: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

cg->prunePhrases(conc1);

delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return true;
}


/********************************************
* FN:		REPLACEVAL
* CR:		06/02/00 AM.
* SUBJ:	Replace attribute value with given value.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	replaceval(concept, attrname, str)
*			replaceval(concept, attrname, num)
********************************************/

bool Arun::replaceval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	_TCHAR *str
	)
{
if (!con_sem || !attr_str || !*attr_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

switch (con_sem->getType())												// 08/08/02 AM.
	{
	case RS_KBCONCEPT:														// 08/08/02 AM.
		{
		CONCEPT *conc1 = con_sem->getKBconcept();

		// Need to get the current KB.
		CG *cg = nlppp->getParse()->getAna()->getCG();

		delete con_sem;									// MEM LEAK.	// 06/28/00 AM.
		return cg->replaceVal(conc1, attr_str, str);
		}
	case RSLONG:																// 08/08/02 AM.
		if (con_sem->getLong() == 0)										// 08/08/02 AM.
			{
			_t_strstream gerrStr;
			gerrStr << _T("[replaceval: Given no concept.]") << ends;
			errOut(&gerrStr,false);
			delete con_sem;
			// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
			nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
			throw ex_EXITPASS;	// 04/15/10 AM.
			return false;
			}
		break;
	default:
		break;
	}

_t_strstream gerrStr;
gerrStr << _T("[replaceval: Bad concept.]") << ends;
errOut(&gerrStr,false);
delete con_sem;								// MEM LEAK.	// 06/27/00 AM.
	// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
	nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
	throw ex_EXITPASS;	// 04/15/10 AM.
return false;
}


// VARIANT.
bool Arun::replaceval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	long num
	)
{
if (!con_sem || !attr_str || !*attr_str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Long value == 0 is ok.	// FIX.	// 05/15/08 AM.
if (con_sem->getType() == RSLONG && con_sem->getLong() == 0)	// 05/15/08 AM.
	{
	_t_strstream gerrStr;	// 05/15/08 AM.
	gerrStr << _T("[replaceval: Given no concept.]") << ends;	// 05/15/08 AM.
	errOut(&gerrStr,false);	// 05/15/08 AM.
	delete con_sem;	// 05/15/08 AM.
	// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
	nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
	throw ex_EXITPASS;	// 04/15/10 AM.
	return false;	// 05/15/08 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[replaceval: Bad concept.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
	nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
	throw ex_EXITPASS;	// 04/15/10 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return cg->replaceVal(conc1, attr_str, num);
}


// VARIANT.																		// 06/27/00 AM.
// Note: Using value sem directly as attribute value.
bool Arun::replaceval(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *val_sem
	)
{
if (!con_sem || !attr_str || !*attr_str || !val_sem || !nlppp)
	{
	if (con_sem)
		delete con_sem;
	if (val_sem)
		delete val_sem;
	return false;
	}

// Long value == 0 is ok.	// FIX.	// 05/15/08 AM.
if (con_sem->getType() == RSLONG && con_sem->getLong() == 0)	// 05/15/08 AM.
	{
	_t_strstream gerrStr;	// 05/15/08 AM.
	gerrStr << _T("[replaceval: Given no concept.]") << ends;	// 05/15/08 AM.
	errOut(&gerrStr,false);	// 05/15/08 AM.
	delete con_sem;	// 05/15/08 AM.
	delete val_sem;	// 05/15/08 AM.
	// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
	nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
	throw ex_EXITPASS;	// 04/15/10 AM.
	return false;	// 05/15/08 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[replaceval: Bad concept.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;
	delete val_sem;
	// RECOVER. GET OUT OF THIS PASS.  // Else infinite loop possible.	// 04/15/10 AM.
	nlppp->exitpass_ = true;	// RECOVER.	// 04/15/10 AM.
	throw ex_EXITPASS;	// 04/15/10 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

_TCHAR *str;
long num;
float fnum;																		// 08/16/01 AM.
CONCEPT *val_conc;
Node<Pn> *val_node;															// 10/30/00 AM.
switch (val_sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:		// "numeric" string. old stuff.
		str = val_sem->getName();
		delete con_sem;
		delete val_sem;
		return cg->replaceVal(conc1, attr_str, str);
	case RSLONG:
		num = val_sem->getLong();
		delete con_sem;
		delete val_sem;
		return cg->replaceVal(conc1, attr_str, num);
	case RSFLOAT:																// 08/16/01 AM.
		fnum = val_sem->getFloat();										// 08/16/01 AM.
		delete con_sem;														// 08/16/01 AM.
		delete val_sem;														// 08/16/01 AM.
		return cg->replaceVal(conc1, attr_str, fnum);				// 08/16/01 AM.
	case RS_KBCONCEPT:														// 08/12/00 AM.
		val_conc = val_sem->getKBconcept();								// 08/12/00 AM.
		delete con_sem;														// 08/12/00 AM.
		delete val_sem;														// 08/12/00 AM.
		return cg->replaceVal(conc1, attr_str, val_conc);			// 08/12/00 AM.

	//case RSOSTREAM:
	//case RS_KBPHRASE:
	//case RS_KBATTR:
	//case RS_KBVAL:
	case RSNODE:																// 10/30/00 AM.
		val_node = val_sem->getNode();									// 10/30/00 AM.
		delete con_sem;														// 10/30/00 AM.
		delete val_sem;														// 10/30/00 AM.
		return cg->replaceVal(conc1, attr_str, val_node);			// 10/30/00 AM.
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[replaceval: Bad concept value.]") << ends;
		errOut(&gerrStr,false);
		}

		break;
	}
delete con_sem;
delete val_sem;
return false;
}

// VARIANT.
bool Arun::replaceval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	long num
	)
{
if (!attr_sem)
	return 0;
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return replaceval(nlppp,con_sem,attr_str,num);
}

// VARIANT
bool Arun::replaceval(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *val_sem
	)
{
if (!attr_sem)
	return false;
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return replaceval(nlppp,con_sem,attr_str,val_sem);
}

/********************************************
* FN:		RENAMECONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Rename given concept.
* RET:	True if ok, else false.
* FORM:	renameconcept(concept, name)
********************************************/

bool Arun::renameconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *str
	)
{
if (!con_sem || !str || !*str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[renameconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return cg->renameConcept(conc1, str);
}


bool Arun::renameconcept(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *str_sem
	)
{
if (!str_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return renameconcept(nlppp,con_sem,str);
}

/********************************************
* FN:		RENAMECHILD
* CR:		06/02/00 AM.
* SUBJ:	Rename nth child of given concept.
* RET:	True if ok, else false.
* FORM:	renamechild(parent, ord, name)
* NOTE:	In cg.h, this is a variant of renameConcept.
********************************************/

bool Arun::renamechild(
	Nlppp *nlppp,
	RFASem *con_sem,
	long num,
	_TCHAR *str
	)
{
if (!con_sem || num <= 0 || !str || !*str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[renamechild: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return cg->renameConcept(conc1, num, str);
}


bool Arun::renamechild(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *num_sem,
	_TCHAR *str
	)
{
if (!num_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
return renamechild(nlppp,con_sem,num,str);
}


bool Arun::renamechild(
	Nlppp *nlppp,
	RFASem *con_sem,
	long num,
	RFASem *str_sem
	)
{
if (!str_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return renamechild(nlppp,con_sem,num,str);
}


bool Arun::renamechild(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *num_sem,
	RFASem *str_sem
	)
{
if (!num_sem)
	{
	if (con_sem)
		delete con_sem;
	if (str_sem)
		delete str_sem;
	return false;
	}
if (!str_sem)
	{
	delete num_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	delete str_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return renamechild(nlppp,con_sem,num,str);
}


/********************************************
* FN:		RENAMEATTR
* CR:		06/02/00 AM.
* SUBJ:	Rename an attribute.
* RET:	True if ok, else false.
* FORM:	renameattr(concept, attrname, newname)
* NOTE:	Not doing the form that takes an attr object.
********************************************/

bool Arun::renameattr(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	_TCHAR *str
	)
{
if (!con_sem || !attr_str || !*attr_str || !str || !*str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[renameattr: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return cg->renameAttr(conc1, attr_str, str);
}


bool Arun::renameattr(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *attr_str,
	RFASem *val_sem
	)
{
if (!val_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *val = val_sem->sem_to_str();
delete val_sem;
return renameattr(nlppp,con_sem,attr_str,val);
}


bool Arun::renameattr(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	_TCHAR *str
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
return renameattr(nlppp,con_sem,attr_str,str);
}


bool Arun::renameattr(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *attr_sem,
	RFASem *str_sem
	)
{
if (!attr_sem)
	{
	if (con_sem)
		delete con_sem;
	if (str_sem)
		delete str_sem;
	return false;
	}
if (!str_sem)
	{
	delete attr_sem;
	if (con_sem)
		delete con_sem;
	return false;
	}
_TCHAR *attr_str = attr_sem->sem_to_str();
delete attr_sem;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return renameattr(nlppp,con_sem,attr_str,str);
}


/********************************************
* FN:		MOVECLEFT
* CR:		06/02/00 AM.
* SUBJ:	Bubble concept left in list.
* RET:	True if ok, else false.
* FORM:	movecleft(concept)
********************************************/

bool Arun::movecleft(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[movecleft: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->moveCleft(conc1);
}


/********************************************
* FN:		MOVECRIGHT
* CR:		06/02/00 AM.
* SUBJ:	Bubble concept right in list.
* RET:	True if ok, else false.
* FORM:	movecright(concept)
********************************************/

bool Arun::movecright(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[movecright: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->moveCright(conc1);
}


/********************************************
* FN:		FINDPHRASE
* CR:		06/02/00 AM.
* SUBJ:	Find concept's phrase.
* RET:	True if ok, else false.
*			UP phrase - phrase found.
* FORM:	findphrase(concept)
********************************************/

RFASem *Arun::findphrase(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findphrase: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

PHRASE *phr = cg->findPhrase(conc1);


delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(phr, RS_KBPHRASE);
}


/********************************************
* FN:		SORTPHRASE
* CR:		06/02/00 AM.
* SUBJ:	Find concept's phrase.
* RET:	True if ok, else false.
* FORM:	sortphrase(concept)
********************************************/

bool Arun::sortphrase(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[sortphrase: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->sortPhrase(conc1);
}


/********************************************
* FN:		SORTCHILDS
* CR:		02/20/09 AM.
* SUBJ:	Sort concept's immediate children.
* RET:	True if ok, else false.
* FORM:	sortchilds(concept)
********************************************/

bool Arun::sortchilds(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[sortchilds: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;
return cg->sortChilds(conc1);
}



/********************************************
* FN:		SORTHIER
* CR:		02/20/09 AM.
* SUBJ:	Sort concept's subhierarchy.
* RET:	True if ok, else false.
* FORM:	sorthier(concept)
********************************************/

bool Arun::sorthier(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[sorthier: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;
return cg->sortHier(conc1);
}


/********************************************
* FN:		PHRASELENGTH
* CR:		06/02/00 AM.
* SUBJ:	Get length of phrase.
* RET:	True if ok, else false.
*			UP len - length of phrase.
* FORM:	phraselength(concept)
********************************************/

long Arun::phraselength(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[phraselength: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->phraseLength(conc1);
}


/********************************************
* FN:		NODECONCEPT
* CR:		06/02/00 AM.
* SUBJ:	Get node's owning concept.
* RET:	True if ok, else false.
*			UP conc - owning concept.
* FORM:	nodeconcept(node_con)
********************************************/

RFASem *Arun::nodeconcept(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[nodeconcept: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *node = sem1->getKBconcept();

CONCEPT *conc = cg->nodeConcept(node);

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(conc, RS_KBCONCEPT);
}


/********************************************
* FN:		NODEOWNER
* CR:		12/03/02 AM.
* SUBJ:	Get node's owning concept.
* RET:	True if ok, else false.
*			UP conc - owning concept.
* FORM:	nodeowner(node_con)
********************************************/

RFASem *Arun::nodeowner(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;
	return NULL;	// 09/26/19 AM.
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[nodeowner: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;
	return NULL;	// 09/26/19 AM.
	}
CONCEPT *node = sem1->getKBconcept();

CONCEPT *conc = cg->nodeOwner(node);

delete sem1;
return new RFASem(conc, RS_KBCONCEPT);
}

/********************************************
* FN:		FINDNODE
* CR:		06/02/00 AM.
* SUBJ:	Find node in phrase.
* RET:	True if ok, else false.
*			UP node - node if found.
* FORMS:	findnode(phrase, namestr) # Returns first match of name.
*			findnode(phrase, ord)   # Nth node.  If 0, then last node.
* NOTE:	Handling TWO VARIANTS here.
********************************************/

RFASem *Arun::findnode(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *name1
	)
{
if (!sem1 || !name1 || !*name1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}
PHRASE *phr1 = sem1->getKBphrase();
CONCEPT *node = 0;

node = cg->findNode(phr1, name1);

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(node, RS_KBCONCEPT);
}


// VARIANT.
RFASem *Arun::findnode(
	Nlppp *nlppp,
	RFASem *sem1,
	long num1
	)
{
if (!sem1 || num1 <= 0 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[findnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}
PHRASE *phr1 = sem1->getKBphrase();
CONCEPT *node = 0;

node = cg->findNode(phr1, num1);

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(node, RS_KBCONCEPT);
}



// VARIANT (AMBIG)
RFASem *Arun::findnode(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *meth_sem	// Method (name or num)
	)
{
if (!meth_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}

long num = 0;
_TCHAR *name = 0;
switch (meth_sem->getType())
	{
	case RSLONG:
		num = meth_sem->getLong();
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		name = meth_sem->getName();
		break;
	default:
		delete meth_sem;
		if (sem)
			delete sem;
		return 0;
		break;
	}
delete meth_sem;
if (name)
	return findnode(nlppp,sem,name);
else
	return findnode(nlppp,sem,num);
}

/********************************************
* FN:		LISTNODE
* CR:		06/02/00 AM.
* SUBJ:	Get first node in list of given node.
* RET:	True if ok, else false.
*			UP conc - first node in list.
* FORM:	listnode(node_con)
********************************************/

RFASem *Arun::listnode(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[listnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *node = sem1->getKBconcept();

CONCEPT *first = cg->listNode(node);

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(first, RS_KBCONCEPT);
}


/********************************************
* FN:		FIRSTNODE
* CR:		06/02/00 AM.
* SUBJ:	Get first node of phrase.
* RET:	True if ok, else false.
*			UP node - first node of phrase.
* FORMS:	firstnode(phrase)
********************************************/

RFASem *Arun::firstnode(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[firstnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
PHRASE *phr1 = sem1->getKBphrase();

CONCEPT *node = cg->firstNode(phr1);

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(node, RS_KBCONCEPT);
}


/********************************************
* FN:		LASTNODE
* CR:		06/02/00 AM.
* SUBJ:	Get last node of phrase.
* RET:	True if ok, else false.
*			UP node - last node of phrase.
* FORMS:	lastnode(phrase)
********************************************/

RFASem *Arun::lastnode(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[lastnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
PHRASE *phr1 = sem1->getKBphrase();

CONCEPT *node = cg->lastNode(phr1);

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return new RFASem(node, RS_KBCONCEPT);
}


/********************************************
* FN:		MAKEPHRASE
* CR:		06/02/00 AM.
* SUBJ:	Create phrase for concept by adding first node.
* RET:	True if ok, else false.
*			UP phr - phrase.
* FORM:	makephrase(concept, name)
********************************************/

RFASem *Arun::makephrase(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *str
	)
{
if (!con_sem || !str || !*str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[makephrase: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

PHRASE *phr = cg->makePhrase(conc1, str);
delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return new RFASem(phr, RS_KBPHRASE);
}


RFASem *Arun::makephrase(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *str_sem
	)
{
if (!str_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return makephrase(nlppp,con_sem,str);
}


/********************************************
* FN:		ADDCNODE
* CR:		06/02/00 AM.
* SUBJ:	Add node to end of concept's phrase.
* RET:	True if ok, else false.
*			UP node - created node-concept.
* FORM:	addcnode(concept, name)
********************************************/

RFASem *Arun::addcnode(
	Nlppp *nlppp,
	RFASem *con_sem,
	_TCHAR *str
	)
{
if (!con_sem || !str || !*str || !nlppp)
	{
	if (con_sem)
		delete con_sem;									// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addcnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete con_sem;										// MEM LEAK.	// 06/27/00 AM.
	return 0;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

CONCEPT *node = cg->addCnode(conc1, str);
delete con_sem;											// MEM LEAK.	// 06/28/00 AM.
return new RFASem(node, RS_KBCONCEPT);
}


// Modifying to handle concept as well as str.						// 11/27/02 AM.
RFASem *Arun::addcnode(
	Nlppp *nlppp,
	RFASem *con_sem,
	RFASem *n_sem
	)
{
if (!n_sem)
	{
	if (con_sem)
		delete con_sem;
	return 0;
	}
if (!con_sem)																		// 11/27/02 AM.
	{
	delete n_sem;																	// 11/27/02 AM.
	return 0;																		// 11/27/02 AM.
	}
switch (n_sem->getType())
	{
	case RSSTR:																		// 11/27/02 AM.
		{
		_TCHAR *str = n_sem->sem_to_str();
		delete n_sem;
		return addcnode(nlppp,con_sem,str);	// DELETES con_sem.
		}
		break;
	case RS_KBCONCEPT:															// 11/27/02 AM.
		{
		CONCEPT *conc1 = con_sem->getKBconcept();							// 11/27/02 AM.
		CG *cg = nlppp->getParse()->getAna()->getCG();					// 11/27/02 AM.
		CONCEPT *node = cg->addCnode(conc1,n_sem->getKBconcept());	// 11/27/02 AM.
		delete con_sem;															// 11/27/02 AM.
		delete n_sem;																// 11/27/02 AM.
		return new RFASem(node, RS_KBCONCEPT);
		}
		break;
	default:
		return 0;
	}
}


/********************************************
* FN:		ADDNODE
* CR:		06/02/00 AM.
* SUBJ:	Add node to nth position of given phrase.
* RET:	True if ok, else false.
*			UP node - created node-concept.
* FORM:	addnode(phrase, name, pos)
* NOTE:	Ignoring the form with a detached node.
********************************************/

RFASem *Arun::addnode(
	Nlppp *nlppp,
	RFASem *phr_sem,
	_TCHAR *str,
	long num
	)
{
if (!phr_sem || !str || !*str || num <= 0 || !nlppp)
	{
	if (phr_sem)
		delete phr_sem;									// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}

if (phr_sem->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[addnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete phr_sem;										// MEM LEAK.	// 06/27/00 AM.
	return NULL;	// 09/26/19 AM.
	}
PHRASE *phr = phr_sem->getKBphrase();

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

CONCEPT *node = cg->addNode(phr, str, num);
delete phr_sem;											// MEM LEAK.	// 06/27/00 AM.
return new RFASem(node, RS_KBCONCEPT);
}


RFASem *Arun::addnode(
	Nlppp *nlppp,
	RFASem *phr_sem,
	RFASem *str_sem,
	long num
	)
{
if (!str_sem)
	{
	if (phr_sem)
		delete phr_sem;
	return 0;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return addnode(nlppp,phr_sem,str,num);
}


RFASem *Arun::addnode(
	Nlppp *nlppp,
	RFASem *phr_sem,
	_TCHAR *str,
	RFASem *num_sem
	)
{
if (!num_sem)
	{
	if (phr_sem)
		delete phr_sem;
	return 0;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (phr_sem)
		delete phr_sem;
	return 0;
	}
return addnode(nlppp,phr_sem,str,num);
}


RFASem *Arun::addnode(
	Nlppp *nlppp,
	RFASem *phr_sem,
	RFASem *str_sem,
	RFASem *num_sem
	)
{
if (!str_sem)
	{
	if (phr_sem)
		delete phr_sem;
	if (num_sem)
		delete num_sem;
	return 0;
	}
if (!num_sem)
	{
	delete str_sem;
	if (phr_sem)
		delete phr_sem;
	return 0;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	{
	if (phr_sem)
		delete phr_sem;
	return 0;
	}
return addnode(nlppp,phr_sem,str,num);
}


// Skipping some NODE MOVEMENT functions.  Not useful for now.


/********************************************
* FN:		RMNODE
* CR:		06/02/00 AM.
* SUBJ:	Remove node from its phrase.
* RET:	True if ok, else false.
* FORM:	rmnode(node_con)
* NOTE:	Ignoring all kinds of variants for now.
********************************************/

bool Arun::rmnode(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmnode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/28/00 AM.
	return false;
	}
CONCEPT *node = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmNode(node);
}


/********************************************
* FN:		RMPHRASE
* CR:		06/02/00 AM.
* SUBJ:	Remove the given phrase.
* RET:	True if ok, else false.
* FORM:	rmphrase(phrase)
* WARN:	Caller should set phrase to zero.
********************************************/

bool Arun::rmphrase(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmphrase: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/28/00 AM.
	return false;
	}
PHRASE *phr = sem1->getKBphrase();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmPhrase(phr);
}


/********************************************
* FN:		RMCPHRASE
* CR:		06/02/00 AM.
* SUBJ:	Remove given concept's phrase.
* RET:	True if ok, else false.
* FORM:	rmcphrase(conc)
********************************************/

bool Arun::rmcphrase(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1 || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmcphrase: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/28/00 AM.
	return false;
	}
CONCEPT *conc = sem1->getKBconcept();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->rmCphrase(conc);
}


/********************************************
* FN:		RENAMENODE
* CR:		06/02/00 AM.
* SUBJ:	Rename node.
* RET:	True if ok, else false.
* FORMS:	renamenode(phrase, namestr, rename)
*			renamenode(phrase, ord, rename)
* NOTE:	Handling TWO VARIANTS here.
********************************************/

bool Arun::renamenode(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *name1,
	_TCHAR *rename
	)
{
if (!sem1 || !name1 || !*name1 || !rename || !*rename || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get object from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[renamenode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/28/00 AM.
	return false;
	}
PHRASE *phr1 = sem1->getKBphrase();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->renameNode(phr1, name1, rename);
}


// VARIANT.
bool Arun::renamenode(
	Nlppp *nlppp,
	RFASem *sem1,
	long num1,
	_TCHAR *rename
	)
{
if (!sem1 || num1 <= 0 || !rename || !*rename || !nlppp)
	{
	if (sem1)
		delete sem1;										// MEM LEAK.	// 06/27/00 AM.
	return false;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get object from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[renamenode: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;											// MEM LEAK.	// 06/28/00 AM.
	return false;
	}
PHRASE *phr1 = sem1->getKBphrase();

delete sem1;												// MEM LEAK.	// 06/28/00 AM.
return cg->renameNode(phr1, num1, rename);
}

bool Arun::renamenode(
	Nlppp *nlppp,
	RFASem *sem1,
	_TCHAR *name1,
	RFASem *rename_sem
	)
{
if (!rename_sem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
_TCHAR *rename = rename_sem->sem_to_str();
delete rename_sem;
return renamenode(nlppp,sem1,name1,rename);
}


bool Arun::renamenode(
	Nlppp *nlppp,
	RFASem *sem1,
	long num1,
	RFASem *rename_sem
	)
{
if (!rename_sem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
_TCHAR *rename = rename_sem->sem_to_str();
delete rename_sem;
return renamenode(nlppp,sem1,num1,rename);
}


// VARIANT (AMBIG)
bool Arun::renamenode(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *meth_sem,
	_TCHAR *rename
	)
{
if (!meth_sem)
	{
	if (sem)
		delete sem;
	return false;
	}

long num = 0;
_TCHAR *name = 0;
switch (meth_sem->getType())
	{
	case RSLONG:
		num = meth_sem->getLong();
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		name = meth_sem->getName();
		break;
	default:
		delete meth_sem;
		if (sem)
			delete sem;
		return 0;
		break;
	}
delete meth_sem;
if (name)
	return renamenode(nlppp,sem,name,rename);
else
	return renamenode(nlppp,sem,num,rename);
}


// VARIANT (AMBIG)
bool Arun::renamenode(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *meth_sem,
	RFASem *rename_sem
	)
{
if (!meth_sem)
	{
	if (sem)
		delete sem;
	if (rename_sem)
		delete rename_sem;
	return false;
	}

long num = 0;
_TCHAR *name = 0;
switch (meth_sem->getType())
	{
	case RSLONG:
		num = meth_sem->getLong();
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		name = meth_sem->getName();
		break;
	default:
		delete meth_sem;
		if (sem)
			delete sem;
		if (rename_sem)
			delete rename_sem;
		return 0;
		break;
	}
delete meth_sem;
_TCHAR *rename = rename_sem->sem_to_str();
delete rename_sem;
if (name)
	return renamenode(nlppp,sem,name,rename);
else
	return renamenode(nlppp,sem,num,rename);
}


/********************************************
* FN:	   STRCONTAINS
* CR:	   06/02/00 Dd.
* SUBJ: Is first string contained in the second?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strcontains(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Arun::strcontains(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if (!name1 || !*name1 || !name2 || !*name2)
	return false;

bool result = false;

while (*name2)
	{
	if (!_tcsncmp(name1,name2,_tcsclen(name1)))
		{
		result = true;
		break;
		}
	++name2;															// OPT.	// 11/29/00 AM.
	}

return result;
}


bool Arun::strcontains(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strcontains(nlppp,name1,name2);
}


bool Arun::strcontains(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strcontains(nlppp,name1,name2);
}


bool Arun::strcontains(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strcontains(nlppp,name1,name2);
}

bool Arun::strcontains(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
return false;
}


bool Arun::strcontains(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	long num
	)
{
return false;
}


bool Arun::strcontains(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name1
	)
{
return false;
}


bool Arun::strcontains(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *sem1,
	long num
	)
{
if (sem1)
	delete sem1;
return false;
}


bool Arun::strcontains(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *sem1
	)
{
if (sem1)
	delete sem1;
return false;
}


/********************************************
* FN:	   STRCONTAINSNOCASE
* CR:	   06/02/00 Dd.
* SUBJ: Is first string contained in the second without case?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strcontainsnocase(str1,str2)
* NOTE: This is not case sensitive
********************************************/

bool Arun::strcontainsnocase(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if (!name1 || !*name1 || !name2 || !*name2)
	return false;

bool result = false;

while (*name2)
	{
	if (!strcmp_ni(name1,name2,_tcsclen(name1)))
		{
		result = true;
		break;
		}
	++name2;															// OPT.	// 11/29/00 AM.
	}

return result;
}



bool Arun::strcontainsnocase(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strcontainsnocase(nlppp,name1,name2);
}


bool Arun::strcontainsnocase(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strcontainsnocase(nlppp,name1,name2);
}


bool Arun::strcontainsnocase(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strcontainsnocase(nlppp,name1,name2);
}

bool Arun::strcontainsnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
return false;
}


bool Arun::strcontainsnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	long num
	)
{
return false;
}


bool Arun::strcontainsnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name1
	)
{
return false;
}


bool Arun::strcontainsnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *sem1,
	long num
	)
{
if (sem1)
	delete sem1;
return false;
}


bool Arun::strcontainsnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *sem1
	)
{
if (sem1)
	delete sem1;
return false;
}


/********************************************
* FN:	   STREQUAL
* CR:	   06/02/00 Dd.
* SUBJ: Strings Equal?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strequal(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Arun::strequal(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if ((!name1 || !*name1) && (!name2 || !*name2))	// FIX.	// 07/11/03 AM.
	return true;
if (!name1 || !*name1 || !name2 || !*name2)
	return false;
return !_tcscmp(name1,name2) ? true : false;
}


bool Arun::strequal(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strequal(nlppp,name1,name2);
}


bool Arun::strequal(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strequal(nlppp,name1,name2);
}


bool Arun::strequal(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strequal(nlppp,name1,name2);
}


bool Arun::strequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (name1 && *name1)
	return false;
return true;
}


bool Arun::strequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (name1 && *name1)
	return false;
return true;
}



bool Arun::strequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return true;
return false;
}


bool Arun::strequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return true;
return false;
}

bool Arun::strequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
return true;
}


/********************************************
* FN:	   STRNOTEQUAL
* CR:	   06/02/00 Dd.
* SUBJ: Strings Not Equal?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strnotequal(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Arun::strnotequal(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if ((!name1 || !*name1) && (!name2 || !*name2))	// FIX.	// 07/11/03 AM.
	return false;
if (!name1 || !*name1 || !name2 || !*name2)
	return true;
return _tcscmp(name1,name2) ? true : false;
}


bool Arun::strnotequal(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strnotequal(nlppp,name1,name2);
}


bool Arun::strnotequal(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strnotequal(nlppp,name1,name2);
}


bool Arun::strnotequal(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strnotequal(nlppp,name1,name2);
}


bool Arun::strnotequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return true;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
if (name1 && *name1)
	return true;
return false;
}


bool Arun::strnotequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return true;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
if (name1 && *name1)
	return true;
return false;
}



bool Arun::strnotequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return true;
}


bool Arun::strnotequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return true;
}

bool Arun::strnotequal(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequal: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
return false;
}


/********************************************
* FN:	   STREQUALNOCASE
* CR:	   06/02/00 Dd.
* SUBJ: Strings Equal ignoring case?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strequalnocase(str1,str2)
* NOTE: This is not case sensitive
********************************************/

bool Arun::strequalnocase(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if ((!name1 || !*name1) && (!name2 || !*name2))	// FIX.	// 07/11/03 AM.
	return true;
if (!name1 || !*name1 || !name2 || !*name2)
	return false;

return !strcmp_i(name1,name2) ? true : false;
}


bool Arun::strequalnocase(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strequalnocase(nlppp,name1,name2);
}


bool Arun::strequalnocase(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strequalnocase(nlppp,name1,name2);
}


bool Arun::strequalnocase(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strequalnocase(nlppp,name1,name2);
}

bool Arun::strequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (name1 && *name1)
	return false;
return true;
}


bool Arun::strequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (name1 && *name1)
	return false;
return true;
}



bool Arun::strequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return true;
return false;
}


bool Arun::strequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return true;
return false;
}

bool Arun::strequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
return true;
}


/********************************************
* FN:	   STRNOTEQUALNOCASE
* CR:	   06/02/00 Dd.
* SUBJ: Strings Not Equal ignoring case?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strnotequalnocase(str1,str2)
* NOTE: This is not case sensitive
********************************************/

bool Arun::strnotequalnocase(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if ((!name1 || !*name1) && (!name2 || !*name2))	// FIX.	// 07/11/03 AM.
	return false;
if (!name1 || !*name1 || !name2 || !*name2)
	return true;
return strcmp_i(name1,name2) ? true : false;
}


bool Arun::strnotequalnocase(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strnotequalnocase(nlppp,name1,name2);
}


bool Arun::strnotequalnocase(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strnotequalnocase(nlppp,name1,name2);
}


bool Arun::strnotequalnocase(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strnotequalnocase(nlppp,name1,name2);
}

bool Arun::strnotequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return true;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
if (name1 && *name1)
	return true;
return false;
}


bool Arun::strnotequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return true;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
if (name1 && *name1)
	return true;
return false;
}



bool Arun::strnotequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return true;
}


bool Arun::strnotequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return true;
}

bool Arun::strnotequalnocase(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strnotequalnocase: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
return false;
}


/********************************************
* FN:	   STRGREATERTHAN
* CR:	   06/02/00 Dd.
* SUBJ: First string is greater than the second?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strgreaterthan(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Arun::strgreaterthan(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if ((!name1 || !*name1) && (!name2 || !*name2))	// FIX.	// 07/11/03 AM.
	return false;
if (!name1 || !*name1)
	return false;
if (!name2 || !*name2)
	return true;
return (_tcscmp(name1,name2) > 0) ? true : false;
}


bool Arun::strgreaterthan(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strgreaterthan(nlppp,name1,name2);
}


bool Arun::strgreaterthan(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strgreaterthan(nlppp,name1,name2);
}


bool Arun::strgreaterthan(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strgreaterthan(nlppp,name1,name2);
}

bool Arun::strgreaterthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strgreaterthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
if (name1 && *name1)
	return true;
return false;
}


bool Arun::strgreaterthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strgreaterthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
if (name1 && *name1)
	return false;
return false;
}



bool Arun::strgreaterthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strgreaterthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return false;
}


bool Arun::strgreaterthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strgreaterthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return true;
}

bool Arun::strgreaterthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strgreaterthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
return false;
}

/********************************************
* FN:	   STRLESSTHAN
* CR:	   06/02/00 Dd.
* SUBJ: First string is less than the second?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strlessthan(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Arun::strlessthan(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
if ((!name1 || !*name1) && (!name2 || !*name2))	// FIX.	// 07/11/03 AM.
	return false;
if (!name1 || !*name1)
	return true;
if (!name2 || !*name2)
	return false;
return (_tcscmp(name1,name2) < 0) ? true : false;
}


bool Arun::strlessthan(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return strlessthan(nlppp,name1,name2);
}


bool Arun::strlessthan(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return false;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strlessthan(nlppp,name1,name2);
}


bool Arun::strlessthan(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return false;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return false;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return strlessthan(nlppp,name1,name2);
}

bool Arun::strlessthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strlessthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
if (name1 && *name1)
	return false;
return false;
}


bool Arun::strlessthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return false;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strlessthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}
if (name1 && *name1)
	return true;
return false;
}



bool Arun::strlessthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strlessthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return true;
}


bool Arun::strlessthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strlessthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

if (!name2 || !*name2)	// FIX.	// 07/11/03 AM.
	return false;
return false;
}

bool Arun::strlessthan(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strlessthan: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}
return false;
}

/********************************************
* FN:	   DICTFINDWORD
* CR:	   06/02/00 Dd.
* SUBJ: Find a dictionary word.
* RET:  True if ok, else false.
*		       UP concept.
* FORM: dictfindword(str)
********************************************/

RFASem *Arun::dictfindword(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1 || !nlppp)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *wordConcept = cg->findWordConcept(str1);				// 06/29/03 AM.

return new RFASem(wordConcept, RS_KBCONCEPT);
}

// VARIANT.
RFASem *Arun::dictfindword(											// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *str1_sem
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return dictfindword(nlppp,str1);
}

/********************************************
* FN:	   DICTGETWORD
* CR:		06/29/03 AM.
* SUBJ:	Find or make a dictionary word.
* RET:  True if ok, else false.
*		       UP concept.
* FORM: dictgetword(str)
********************************************/

RFASem *Arun::dictgetword(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1 || !nlppp)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

CONCEPT *wordConcept = cg->getWordConcept(str1);

return new RFASem(wordConcept, RS_KBCONCEPT);
}

// VARIANT.
RFASem *Arun::dictgetword(
	Nlppp *nlppp,
	RFASem *str1_sem
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return dictgetword(nlppp,str1);
}

/********************************************
* FN:	   STRTOLOWER
* CR:	   06/02/00 Dd.
* SUBJ: Makes a string lower case.
* RET:  True if ok, else false.
*	 UP - returns the lower case version of the string
* FORMS:	strtolower(str1)
* NOTE:
********************************************/

_TCHAR *Arun::strtolower(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!name1 || !*name1 || !nlppp)
	return 0;

_TCHAR *buf = Chars::create(_tcsclen(name1)+1);						// 07/16/03 AM.
str_to_lower(name1, buf);			// 06/03/00 AM.

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);														// 07/16/03 AM.

return str;
}

// VARIANT.
_TCHAR *Arun::strtolower(													// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
return strtolower(nlppp,name_str);
}

// VARIANT.
_TCHAR *Arun::strtolower(													// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:	   STRTOUPPER
* CR:	   06/02/00 Dd.
* SUBJ: Makes a string upper case.
* RET:  True if ok, else false.
*	 UP - returns the upper case version of the string
* FORMS:	strtoupper(str1)
* NOTE:
********************************************/

_TCHAR *Arun::strtoupper(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!name1 || !*name1 || !nlppp)
	return 0;

_TCHAR *buf = Chars::create(_tcsclen(name1)+1);					// 07/16/03 AM.
str_to_upper(name1, buf);			// 06/03/00 AM.

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);													// 07/16/03 AM.

return str;
}

_TCHAR *Arun::strtoupper(
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
return strtoupper(nlppp,name_str);
}

// VARIANT.
_TCHAR *Arun::strtoupper(													// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}

/********************************************
* FN:	   ADDWORD
* CR:		06/02/00 AM.
* SUBJ:	Add word to KB dictionary.
* RET:	True if ok, else false.
*			UP - concept built for word.
* FORMS:	addword(str)
* NOTE:
********************************************/

RFASem *Arun::addword(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!name1 || !*name1 || !nlppp)
	return 0;

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(name1, /*UP*/ str);

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();


CONCEPT *conc = cg->addWord(str);

return new RFASem(conc, RS_KBCONCEPT);
}

// VARIANT.
RFASem *Arun::addword(														// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
return addword(nlppp,name_str);
}

/********************************************
* FN:	   STRLENGTH
* CR:	   06/02/00 Dd.
* SUBJ: Finds the length of the string
* RET:  True if ok, else false.
*	 UP - returns the length of the string
* FORMS:	strlength(str1)
* NOTE:
********************************************/

long Arun::strlength(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!name1 || !*name1)
	return 0;
return _tcsclen(name1);
}

// VARIANT.																// 01/23/01/00 AM.
long Arun::strlength(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem)
	return 0;

_TCHAR *st = 0;
switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		st = sem->getName();
		break;
	default:
		break;
	}
delete sem;
if (!st || !*st)
	return 0;
return _tcsclen(st);
}

long Arun::strlength(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:	   STRPIECE
* CR:	   06/02/00 Dd.
* SUBJ: Returns a substring of a string
* RET:  True if ok, else false.
*	 UP - returns a substring of a string
* FORMS:	strlength(str1,start,end)
* NOTE: start and end are zero-numbered
********************************************/

_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	_TCHAR *str1,
	long start,
	long end
	)
{
if (!str1 || !*str1 || !nlppp)
	return 0;

//char buf[MAXSTR];
long len = _tcsclen(str1);
if (end > len)
	end = len - 1;

#ifdef JUNK_
_TCHAR *c;
long i = 0;

for (c = str1; i < len && i < MAXSTR; ++c)
	{
	if (i >= start && i <= end)
		buf[i++] = *c;
	}
buf[i] = '\0';
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.
#endif

long xlen = end - start + 1;												// 07/16/03 AM.
_TCHAR *buf = Chars::create(xlen+1);										// 07/16/03 AM.

if (!str_piece(str1,start,end,xlen+1,/*UP*/buf))					// 11/18/02 AM.
	{
	Chars::destroy(buf);														// 07/16/03 AM.
	return 0;																	// 11/18/02 AM.
	}

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);															// 07/16/03 AM.

return str;
}


_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	RFASem *str_sem,
	long start,
	long end
	)
{
if (!str_sem)
	return 0;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return strpiece(nlppp,str,start,end);
}


_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	_TCHAR *str,
	RFASem *start_sem,
	long end
	)
{
if (!start_sem)
	return 0;
bool ok = false;
long start = start_sem->sem_to_long(ok);
delete start_sem;
if (!ok)
	return 0;
return strpiece(nlppp,str,start,end);
}


_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	_TCHAR *str,
	long start,
	RFASem *end_sem
	)
{
if (!end_sem)
	return 0;
bool ok = false;
long end = end_sem->sem_to_long(ok);
delete end_sem;
if (!ok)
	return 0;
return strpiece(nlppp,str,start,end);
}


_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	RFASem *str_sem,
	RFASem *start_sem,
	long end
	)
{
if (!str_sem)
	{
	if (start_sem)
		delete start_sem;
	return 0;
	}
if (!start_sem)
	{
	delete str_sem;
	return 0;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
bool ok = false;
long start = start_sem->sem_to_long(ok);
delete start_sem;
if (!ok)
	return 0;
return strpiece(nlppp,str,start,end);
}


_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	RFASem *str_sem,
	long start,
	RFASem *end_sem
	)
{
if (!str_sem)
	{
	if (end_sem)
		delete end_sem;
	return 0;
	}
if (!end_sem)
	{
	delete str_sem;
	return 0;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
bool ok = false;
long end = end_sem->sem_to_long(ok);
delete end_sem;
if (!ok)
	return 0;
return strpiece(nlppp,str,start,end);
}


_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	RFASem *str_sem,
	RFASem *start_sem,
	RFASem *end_sem
	)
{
if (!str_sem)
	{
	if (start_sem)
		delete start_sem;
	if (end_sem)
		delete end_sem;
	return 0;
	}
if (!start_sem)
	{
	delete str_sem;
	if (end_sem)
		delete end_sem;
	return 0;
	}
if (!end_sem)
	{
	delete str_sem;
	delete start_sem;
	return 0;
	}
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
bool ok = false;
long start = start_sem->sem_to_long(ok);
delete start_sem;
if (!ok)
	{
	delete str_sem;
	delete end_sem;
	return 0;
	}
bool ok1 = false;
long end = end_sem->sem_to_long(ok1);
delete end_sem;
if (!ok1)
	{
	delete str_sem;
	delete start_sem;
	return 0;
	}
return strpiece(nlppp,str,start,end);
}

// A forgotten variant.	// 04/20/05 AM.
_TCHAR *Arun::strpiece(
	Nlppp *nlppp,
	_TCHAR *str,
	RFASem *start_sem,
	RFASem *end_sem
	)
{
if (!str && !*str)
	{
	if (start_sem)
		delete start_sem;
	if (end_sem)
		delete end_sem;
	return 0;
	}
if (!start_sem)
	{
	if (end_sem)
		delete end_sem;
	return 0;
	}
if (!end_sem)
	{
	delete start_sem;
	return 0;
	}
bool ok = false;
long start = start_sem->sem_to_long(ok);
delete start_sem;
if (!ok)
	{
	delete end_sem;
	return 0;
	}
bool ok1 = false;
long end = end_sem->sem_to_long(ok1);
delete end_sem;
if (!ok1)
	{
	delete start_sem;
	return 0;
	}
return strpiece(nlppp,str,start,end);
}

// NULL VARIANTS.	// 07/11/03 AM.
_TCHAR *Arun::strpiece(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *start_sem,
	RFASem *end_sem
	)
{
if (start_sem)
	delete start_sem;
if (end_sem)
	delete end_sem;
return 0;
}

_TCHAR *Arun::strpiece(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	long num1,
	RFASem *end_sem
	)
{
if (end_sem)
	delete end_sem;
return 0;
}

_TCHAR *Arun::strpiece(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *start_sem,
	long num1
	)
{
if (start_sem)
	delete start_sem;
return 0;
}

_TCHAR *Arun::strpiece(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	long num1,
	long num2
	)
{
return 0;
}


/********************************************
* FN:	   STRCHAR
* CR:	   06/02/00 Dd.
* SUBJ: Finds a character in a string
* RET:  True if ok, else false.
*	 UP - returns a character
* FORMS:	strchar(name1,charnum)
* NOTE:
********************************************/

_TCHAR *Arun::strchar(
	Nlppp *nlppp,
	_TCHAR *name1,
	long charnum
	)
{
if (!name1 || !*name1
// || !charnum	// FIX.	// 02/24/14 AM.
	|| !nlppp)
	return 0;

_TCHAR buf[2];
buf[0] = name1[charnum];
buf[1] = '\0';
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
return str;
}


_TCHAR *Arun::strchar(
	Nlppp *nlppp,
	RFASem *name_sem,
	long charnum
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strchar(nlppp,name,charnum);
}


_TCHAR *Arun::strchar(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *charnum_sem
	)
{
if (!charnum_sem)
	return 0;
bool ok = false;
long charnum = charnum_sem->sem_to_long(ok);
delete charnum_sem;
if (!ok)
	return 0;
return strchar(nlppp,name,charnum);
}


_TCHAR *Arun::strchar(	// MISSING VARIANT.	// 07/25/11 AM.
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *charnum_sem
	)
{
if (!name_sem && !charnum_sem)
	return 0;
if (!name_sem)
	{
	delete charnum_sem;
	return 0;
	}

_TCHAR *name = name_sem->sem_to_str();
delete name_sem;

bool ok = false;
long charnum = 0;
if (charnum_sem)
	{
	charnum = charnum_sem->sem_to_long(ok);
	delete charnum_sem;
	}

return strchar(nlppp,name,charnum);
}


// NULL VARIANTS.	// 07/11/03 AM.
_TCHAR *Arun::strchar(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	RFASem *charnum_sem
	)
{
if (charnum_sem)
	delete charnum_sem;
return 0;
}

_TCHAR *Arun::strchar(		// 07/11/03 AM.
	Nlppp *nlppp,
	long num,
	long num1
	)
{
return 0;
}


/********************************************
* FN:	   STRISDIGIT
* CR:	   06/02/00 Dd.
* SUBJ: Checks to see if the entire string is digits
* RET:  True if ok, else false.
*	 UP - returns 1 if all digits, 0 if not
* FORMS:	strisdigit(str1)
* NOTE:
********************************************/

bool Arun::strisdigit(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return false;

for (_TCHAR *c = str1; *c; ++c)
	{
	if (!_istdigit((_TUCHAR)*c))
		return false;
	}
return true;
}


bool Arun::strisdigit(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return false;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return strisdigit(nlppp,str);
}


bool Arun::strisdigit(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}


/********************************************
* FN:	   STRISALPHA
* CR:	   06/02/00 Dd.
* SUBJ: Checks to see if the entire string is alpha
* RET:  True if ok, else false.
*	 UP - returns 1 for true, 0 for false
* FORMS:	strisalpha(str1)
* NOTE:
********************************************/

bool Arun::strisalpha(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return false;

for (_TCHAR *c = str1; *c; ++c)
	{
	if (!alphabetic(*c))
		return false;
	}
return true;
}


bool Arun::strisalpha(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return false;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return strisalpha(nlppp,str);
}


bool Arun::strisalpha(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}


/********************************************
* FN:		TAKE
* CR:		06/02/00 AM.
* SUBJ:	Take a file of commands!
* RET:	True if ok, else false.
* NOTE:	That is, read and execute a file of knowledge addition commands.
* FORMS:	take(filename_s)
********************************************/

bool Arun::take(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!name1 || !*name1 || !nlppp)
	return false;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

return cg->readFile(name1);
}


bool Arun::take(
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return false;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return take(nlppp,name);
}



/********************************************
* FN:	   RMWORD
* CR:		07/26/00 AM.
* SUBJ:	Remove word from KB dictionary.
* RET:	True if ok, else false.
* FORMS:	rmword(str)
********************************************/

bool Arun::rmword(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!name1 || !*name1 || !nlppp)
	return 0;

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(name1, /*UP*/ str);

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

return cg->rmWord(name1);
}


bool Arun::rmword(
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return false;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return rmword(nlppp,name);
}


/********************************************
* FN:	   SYSTEM
* CR:		07/26/00 AM.
* SUBJ:	Execute given string as an operating system command.
* RET:	True if ok, else false.
* FORMS:	system(str)
********************************************/

bool Arun::vtsystem(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
if (!str || !*str || !nlppp)
	return 0;

run_silent(str);			// system() w/o DOC CMD PROMPT POPUP.	// 09/28/08 AM.
return true;	// 09/28/08 AM.

// Returns -1 if problems, I believe.									// 05/07/01 AM.
#ifdef _OLDWAY_
#ifndef UNICODE
if (system(str) < 0)											// FIX.	// 05/07/01 AM.
#else
if (_tsystem(str) < 0)		// 11/05/06 AM.
#endif
	return false;																// 05/07/01 AM.
return true;																	// 05/07/01 AM.
#endif
}


bool Arun::vtsystem(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return false;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return vtsystem(nlppp,str);
}


/********************************************
* FN:		STR
* CR:		07/26/00 AM.
* SUBJ:	Convert argument to string.
* RET:	True if ok.
* FORMS:	str(num);
*			str(str);		# no-op.
********************************************/

_TCHAR *Arun::str(
	Nlppp *nlppp,
	RFASem *sem
	)
{
_TCHAR *st = 0;
if (!sem)
	{
	// For consistency, treat this as a (long)0.	// FIX.	// 05/17/08 AM.
	Parse *parse = nlppp->getParse();				// FIX.	// 05/17/08 AM.
	parse->internStr(_T("0"), /*UP*/ st);			// FIX.	// 05/17/08 AM.
	return st;
	}

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		st = sem->getName();
		break;
	case RSLONG:
		st = str(nlppp, sem->getLong());
		break;
	case RSFLOAT:																// 08/16/01 AM.
		st = str(nlppp, sem->getFloat());								// 08/16/01 AM.
		break;																	// 08/16/01 AM.
	default:
		break;
	}
delete sem;
return st;
}

_TCHAR *Arun::str(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
// If str == 0, retaining the idea that we may want an implementation
// of a NULL string or "".	So not converting this to the string "0".	// 05/17/08 AM.
return str;
}

_TCHAR *Arun::str(
	Nlppp *nlppp,
	long num
	)
{
_TCHAR *st=0;

// Convert num, then intern string.
_TCHAR buf[MAXSTR+1];
_stprintf(buf, _T("%ld"), num);
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ st);
return st;
}

_TCHAR *Arun::str(																// 08/16/01 AM.
	Nlppp *nlppp,
	float fnum
	)
{
_TCHAR *st=0;

// Convert num, then intern string.
_TCHAR buf[MAXSTR+1];
_stprintf(buf, _T("%f"), fnum);
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ st);
return st;
}


/********************************************
* FN:		NUM
* CR:		07/26/00 AM.
* SUBJ:	Convert argument to number.
* RET:	True if ok.
* FORMS:	num(str);
*			num(num);		# no-op.
********************************************/

long Arun::num(
	Nlppp *nlppp,
	RFASem *sem
	)
{
long nm = 0;
if (!sem)
	return nm;

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		nm = num(nlppp, sem->getName());
		break;
	case RSLONG:
		nm = sem->getLong();
		break;
	case RSFLOAT:																// 08/16/01 AM.
		// NOTE: Will truncate the float.								// 08/16/01 AM.
		nm = (long) sem->getFloat();		// .NET COMPLAINT.		// 06/11/03 AM.
//		nm = (float) sem->getFloat();										// 09/27/01 AM.
		break;																	// 08/16/01 AM.
	default:
		break;
	}
delete sem;
return nm;
}

long Arun::num(
	Nlppp *nlppp,
	long num
	)
{
return num;
}

// Trying to get compiled/interpreted behavior the same.	// 08/10/08 AM.
// Also, this will eliminate a float -> long conversion error in C++ compile
// of the RUN.DLL project.	// 08/10/08 AM.
long Arun::num(
	Nlppp *nlppp,
	float num
	)
{
long x = (long) num;
return x;
}

long Arun::num(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
long num = 0;

if (str && *str)			// String variant.
	str_to_long(str, /*UP*/ num);
return num;
}


/********************************************
* FN:		STRTOTITLE
* CR:		08/22/00 AM.
* SUBJ:	Makes a string title case.
* RET:	True if ok, else false.
*			UP - returns the title case version of the string
* FORMS:	strtotitle(str1)
* NOTE:
********************************************/

_TCHAR *Arun::strtotitle(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!nlppp || !name1 || !*name1)
	return 0;

_TCHAR *buf = Chars::create(_tcsclen(name1)+1);	// 07/16/03 AM.
str_to_title(name1, buf);

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);									// 07/16/03 AM.

return str;
}


// VARIANT.
_TCHAR *Arun::strtotitle(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!nlppp || !sem)
	return 0;

_TCHAR *name1=0;
switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		name1 = sem->getName();
		break;
	default:
		delete sem;
		return 0;
	}
delete sem;

if (!name1 || !*name1)
	return 0;

_TCHAR *buf = Chars::create(_tcsclen(name1)+1);	// 07/16/03 AM.
str_to_title(name1, buf);

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);									// 07/16/03 AM.

return str;
}


/********************************************
* FN:		ARRAYLENGTH
* CR:		01/04/01 AM.
* SUBJ:	Get length of an array.
* RET:	True if ok, else false.
* FORMS:	long arraylength(var)
* NOTE:	Return 1 for simple types.
********************************************/

long Arun::arraylength(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
return 1;
}

// VARIANT.
long Arun::arraylength(
	Nlppp *nlppp,
	long num
	)
{
return 1;
}

// VARIANT
long Arun::arraylength(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1)
	return 1;

switch (sem1->getType())
	{
	case RSARGS:
	   {
		// Array.  Compute length.
		long len = sem1->getArgs()->getLength();						// 08/07/02 AM.
		delete sem1;										// MEM LEAK.	// 08/07/02 AM.
		return len;																// 08/07/02 AM.
		}
	default:
		break;
	}
delete sem1;												// MEM LEAK.	// 08/07/02 AM.
return 1;
}


/********************************************
* FN:		LASTELTNODE
* CR:		01/04/01 AM.
* SUBJ:	Get last node matching a rule element.
* RET:	Sem object for node, if any.
* FORMS:	lasteltnode(elt_num)
* NOTE:	04/28/01 AM. Changing return to RFASem, to simplify.
********************************************/

RFASem *Arun::lasteltnode(
	Nlppp *nlppp,
	long num1
	)
{
if (!nlppp || num1 < 0)
	return 0;

// Removed check for region here.							// FIX.	// 04/29/01 AM.

if (!nlppp->coll_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[lasteltnode: Warning. No collected nodes.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

Node<Pn> *node = (Node<Pn> *)nlppp->coll_[num1].end;				// 04/28/01 AM.
if (!node)																		// 04/28/01 AM.
	return 0;																	// 04/28/01 AM.
RFASem *sem = new RFASem(node);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.
}

// VARIANT.
RFASem *Arun::lasteltnode(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem)			// MEMLEAK.	// 05/15/08 AM.
	return 0;		// MEMLEAK.	// 05/15/08 AM.

if (!nlppp)
	{
	delete sem;		// MEMLEAK.	// 05/15/08 AM.
	return 0;
	}

// Removed check for region here.							// FIX.	// 04/29/01 AM.

if (sem->getType() != RSLONG)
	{
	delete sem;		// MEMLEAK.	// 05/15/08 AM.
	return 0;
	}
long num1 = sem->getLong();
delete sem;		// MEMLEAK.	// 05/15/08 AM.

if (num1 < 0)
	return 0;

if (!nlppp->coll_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[lasteltnode: Warning. No collected nodes.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

Node<Pn> *node = (Node<Pn> *)nlppp->coll_[num1].end;				// 04/28/01 AM.
if (!node)																		// 04/28/01 AM.
	return 0;																	// 04/28/01 AM.
RFASem *sem1 = new RFASem(node);											// 04/28/01 AM.
return sem1;																	// 04/28/01 AM.
}



/********************************************
* FN:		PHRASETEXT
* CR:		01/06/01 AM.
* SUBJ:	Get text for right-hand-side phrase matching current rule.
* FORMS:	phrasetext().
********************************************/

_TCHAR *Arun::phrasetext(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return 0;

// Removed check for region here.							// FIX.	// 04/29/01 AM.

long len  = 0;
_TCHAR *text=0;
_TCHAR *str=0;
Node<Pn> *nstart= nlppp->getFirst();	// First node matching rule.
Node<Pn> *nend = nlppp->getLast();		// Last node matching rule.
// Not sure if these will always be what we want.

if (!nstart || !nend)
	return 0;
Pn *pn = nstart->getData();
if (nstart != nend)
	{
	Pn *pn2 = nend->getData();
	len = pn2->getEnd() - pn->getStart() + 1;
	}
else
	len = pn->getEnd() - pn->getStart() + 1;

	// Get text for multi-node region.
	// Intern text.
_TCHAR *buf;
text = make_str(pn->getText(), len);
if (!text || !*text)
	return 0;

buf = Chars::create(len+1);
clean_str(text,buf);
nlppp->getParse()->internStr(buf, /*UP*/ str);
//sem = new RFASem(str, RSSTR);
Chars::destroy(buf);			// Free up buffer.
Chars::destroy(text);
return str;
}


/********************************************
* FN:		PHRASERAW
* CR:		01/06/01 AM.
* SUBJ:	Get raw text for right-hand-side phrase matching current rule.
* FORMS:	phraseraw().
********************************************/

_TCHAR *Arun::phraseraw(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return 0;

// Removed check for region here.							// FIX.	// 04/29/01 AM.

long len  = 0;
_TCHAR *text=0;
_TCHAR *str=0;
Node<Pn> *nstart= nlppp->getFirst();	// First node matching rule.
Node<Pn> *nend = nlppp->getLast();		// Last node matching rule.
// Not sure if these will always be what we want.

if (!nstart || !nend)
	return 0;
Pn *pn = nstart->getData();
if (nstart != nend)
	{
	Pn *pn2 = nend->getData();
	len = pn2->getEnd() - pn->getStart() + 1;
	}
else
	len = pn->getEnd() - pn->getStart() + 1;

	// Get text for multi-node region.
	// Intern text.
text = make_str(pn->getText(), len);
if (!text || !*text)
	return 0;

nlppp->getParse()->internStr(text, /*UP*/ str);
//sem = new RFASem(str, RSSTR);
Chars::destroy(text);
return str;
}

//*************************************************
// NLP++ PARSE TREE FUNCTIONS
//*************************************************

/********************************************
* FN:		PNVAR
* CR:		01/06/01 AM.
* SUBJ:	Get PNODE's variable.
* RET:	True if ok, else false.
* FORMS:	pnvar(pnode, var_str)
* NOTE:	First NLP++ function for PNODE type!
********************************************/

RFASem *Arun::pnvar(
	Nlppp *nlppp,
	NODE *node,
	_TCHAR *name1
	)
{
RFASem *sem = 0;

if (!nlppp || !node || !name1 || !*name1)
	return 0;

bool ok = true;

if (*name1 == '$')												// Get special var.
	ok = Ivar::nodeVarspecial((Node<Pn> *)node,name1+1,nlppp,/*UP*/sem);
else																	// Get normal var.
	ok = Ivar::nodeVar((Node<Pn> *)node,name1,nlppp,/*UP*/sem);

if (!ok)
	{
	if (sem)															// 10/29/04 AM.
		delete sem;
	return 0;
	}
return sem;
}


RFASem *Arun::pnvar(
	Nlppp *nlppp,
	RFASem *node_sem,
	_TCHAR *name1
	)
{
if (!node_sem)
	return 0;
NODE *node = node_sem->sem_to_node();
delete node_sem;
return pnvar(nlppp,node,name1);
}


RFASem *Arun::pnvar(
	Nlppp *nlppp,
	NODE *node,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return pnvar(nlppp,node,name);
}


RFASem *Arun::pnvar(
	Nlppp *nlppp,
	RFASem *node_sem,
	RFASem *name_sem
	)
{
if (!node_sem)
	{
	if (name_sem)
		delete name_sem;
	return 0;
	}
if (!name_sem)
	{
	delete node_sem;
	return 0;
	}
NODE *node = node_sem->sem_to_node();
delete node_sem;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return pnvar(nlppp,node,name);
}


/********************************************
* FN:		PNSINGLETDOWN
* CR:		01/08/01 AM.
* SUBJ:	Get PNODE's child, if a singlet.
* RET:	True if ok, else false.
* FORMS:	pnsingletdown(pnode)
* NOTE:	Observe rules about branching and BASE attribute.
********************************************/

RFASem *Arun::pnsingletdown(
	Nlppp *nlppp,
	NODE *pnode
	)
{
if (!nlppp || !pnode)
	return 0;

Node<Pn> *node = (Node<Pn> *)pnode;
Pn *pn = node->getData();
if (pn->getBase())			// Current node is a bottommost singlet.
	return 0;
if (!(node = node->Down()))	// Leaf.
	return 0;					// Return nil node.
if (node->Right())				// Branches out.
	return 0;					// Return nil node.
// Got good singlet child!
RFASem *sem = new RFASem(node);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.
}

RFASem *Arun::pnsingletdown(
	Nlppp *nlppp,
	RFASem *pnode_sem
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;
return pnsingletdown(nlppp,pnode);
}


/********************************************
* FN:		PNNAME
* CR:		01/08/01 AM.
* SUBJ:	Get PNODE's name.
* RET:	True if ok, else false.
* FORMS:	str pnname(pnode)
* NOTE:	Observe rules about branching and BASE attribute.
********************************************/

_TCHAR *Arun::pnname(
	Nlppp *nlppp,
	NODE *node
	)
{
if (!nlppp || !node)
	return 0;

Pn *pn = ((Node<Pn> *)node)->getData();
_TCHAR *str = pn->getName();
// Todo: Name should be interned, but may want to check to be sure.
return str;
}

// VARIANT
_TCHAR *Arun::pnname(															// 04/29/01 AM.
	Nlppp *nlppp,
	RFASem *node_sem
	)
{
if (!nlppp || !node_sem)
	return 0;
NODE *node = node_sem->sem_to_node();
delete node_sem;
return pnname(nlppp,node);
}


/********************************************
* FN:		PNROOT
* CR:		01/08/01 AM.
* SUBJ:	Get root of parse tree.
* RET:	True if ok, else false.
* FORMS:	pnode pnroot()
* NOTE:
********************************************/

RFASem *Arun::pnroot(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return 0;

Parse *parse = nlppp->getParse();

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
if (!tree)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[pnroot: Couldn't fetch parse tree.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

Node<Pn> *root = tree->getRoot();
if (!root)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[pnroot: Couldn't fetch root of parse tree.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

RFASem *sem = new RFASem(root);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.

}


/********************************************
* FN:		PNDOWN
* CR:		01/08/01 AM.
* SUBJ:	Get PNODE's child.
* RET:	True if ok, else false.
* FORMS:	pnode pndown(pnode)
********************************************/

RFASem *Arun::pndown(
	Nlppp *nlppp,
	NODE *pnode
	)
{
if (!nlppp || !pnode)
	return 0;

Node<Pn> *node = ((Node<Pn> *)pnode)->Down();						// 04/28/01 AM.
if (!node)																		// 04/28/01 AM.
	return 0;																	// 04/28/01 AM.
RFASem *sem = new RFASem(node);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.
}


// VARIANT
RFASem *Arun::pndown(														// 04/29/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;
return pndown(nlppp,pnode);
}

/********************************************
* FN:		PNUP
* CR:		01/08/01 AM.
* SUBJ:	Get PNODE's parent.
* RET:	True if ok, else false.
* FORMS:	pnode pnup(pnode)
********************************************/

RFASem *Arun::pnup(
	Nlppp *nlppp,
	NODE *pnode
	)
{
if (!nlppp || !pnode)
	return 0;

Node<Pn> *node = ((Node<Pn> *)pnode)->Up();							// 04/28/01 AM.
if (!node)																		// 04/28/01 AM.
	return 0;																	// 04/28/01 AM.
RFASem *sem = new RFASem(node);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.
}


// VARIANT
RFASem *Arun::pnup(															// 04/29/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;
return pnup(nlppp,pnode);
}


/********************************************
* FN:		PNNEXT
* CR:		01/08/01 AM.
* SUBJ:	Get PNODE's right sibling.
* RET:	True if ok, else false.
* FORMS:	pnode pnnext(pnode)
********************************************/

RFASem *Arun::pnnext(
	Nlppp *nlppp,
	NODE *pnode
	)
{
if (!nlppp || !pnode)
	return 0;

Node<Pn> *node = ((Node<Pn> *)pnode)->Right();						// 04/28/01 AM.
if (!node)																		// 04/28/01 AM.
	return 0;																	// 04/28/01 AM.
RFASem *sem = new RFASem(node);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.
}


// VARIANT
RFASem *Arun::pnnext(														// 04/29/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;
return pnnext(nlppp,pnode);
}


/********************************************
* FN:		PNPREV
* CR:		01/08/01 AM.
* SUBJ:	Get PNODE's left sibling.
* RET:	True if ok, else false.
* FORMS:	pnode pnprev(pnode)
********************************************/

RFASem *Arun::pnprev(
	Nlppp *nlppp,
	NODE *pnode
	)
{
if (!nlppp || !pnode)
	return 0;

Node<Pn> *node = ((Node<Pn> *)pnode)->Left();						// 04/28/01 AM.
if (!node)																		// 04/28/01 AM.
	return 0;																	// 04/28/01 AM.
RFASem *sem = new RFASem(node);											// 04/28/01 AM.
return sem;																		// 04/28/01 AM.
}


// VARIANT
RFASem *Arun::pnprev(														// 04/29/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;
return pnprev(nlppp,pnode);
}


/********************************************
* FN:		PNRENAME
* CR:		01/08/01 AM.
* SUBJ:	Rename PNODE.
* RET:	True if ok, else false.
* FORMS:	str pnrename(pnode, str)
********************************************/

_TCHAR *Arun::pnrename(
	Nlppp *nlppp,
	NODE *pnode,
	_TCHAR *str1
	)
{
if (!nlppp || !pnode)
	return 0;

if (!str1 || !*str1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[pnrename: Warning. Given no rename string.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR *str=0;
nlppp->getParse()->internStr(str1, /*UP*/str);	// Intern str.

Pn *pn = ((Node<Pn> *)pnode)->getData();
pn->setName(str);

// Silently tell where this node rename came from.					// 09/16/08 AM.
long ruleline = (nlppp->sugg_ ? nlppp->sugg_->ruleline : 0);	// 09/16/08 AM.
pn->setPassnum(nlppp->parse_->currpass_);								// 09/16/08 AM.
pn->setRuleline(ruleline);													// 09/16/08 AM.

return str;
}

// VARIANTS.																	// 04/28/01 AM.
_TCHAR *Arun::pnrename(														// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem,
	_TCHAR *str1
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;
return pnrename(nlppp,pnode,str1);
}


_TCHAR *Arun::pnrename(														// 04/28/01 AM.
	Nlppp *nlppp,
	NODE *pnode,
	RFASem *str1_sem
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return pnrename(nlppp,pnode,str1);
}


_TCHAR *Arun::pnrename(														// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem,
	RFASem *str1_sem
	)
{
if (!pnode_sem && !str1_sem)
	return 0;
if (!pnode_sem)
	{
	delete str1_sem;
	return 0;
	}
if (!str1_sem)
	{
	delete pnode_sem;
	return 0;
	}
Node<Pn> *pnode = pnode_sem->sem_to_node();
_TCHAR *str1 = str1_sem->sem_to_str();
delete pnode_sem;
delete str1_sem;
return pnrename(nlppp,pnode,str1);
}



/********************************************
* FN:		PNDELETECHILDS
* CR:		01/08/01 AM.
* SUBJ:	Delete PNODE's children.
* RET:	True if ok, else false.
* FORMS:	pndeletechilds(pnode)
********************************************/

bool Arun::pndeletechilds(
	Nlppp *nlppp,
	NODE *pnode
	)
{
if (!nlppp || !pnode)
	return false;

Node<Pn> *dn = ((Node<Pn> *)pnode)->Down();
if (dn)
	((Tree<Pn> *)nlppp->getParse()->getTree())->deleteSubs(dn);	// Delete the subtree.
return true;
}

// VARIANT
bool Arun::pndeletechilds(													// 04/29/01 AM.
	Nlppp *nlppp,
	RFASem *pnode_sem
	)
{
if (!pnode_sem)
	return 0;
NODE *pnode = pnode_sem->sem_to_node();
delete pnode_sem;							// FIXES MEM LEAK.			// 04/29/01 AM.
return pndeletechilds(nlppp,pnode);
}


/********************************************
* FN:		VARINLIST
* CR:		01/08/01 AM.
* SUBJ:	Get any variable in list of pnodes that matched rule element.
* RET:	True if ok, else false.
* FORMS:	varinlist(var_str, elt_num)
* NOTE:	To handle multi-node matches of a rule element.
*			Not handling special variables.
********************************************/

RFASem *Arun::varinlist(
	Nlppp *nlppp,
	_TCHAR *name1,
	long num1
	)
{
RFASem *sem = 0;

if (!nlppp || num1 < 1)											// FIX.	// 04/29/01 AM.
//if (!nlppp || num < 1)	// VC++ "FEATURE"--THIS COMPILES!	// 04/29/01 AM.
	return NULL;	// 09/26/19 AM.

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[varinlist: Warning. Given no var name.]") << ends;
	errOut(&gerrStr,false);
	return NULL;	// 09/26/19 AM.
	}

// This type of logic appears in post-actions.  Fetch the node(s) that
// matched the given rule element number.
// COMPILED STYLE.
COLL *coll = &(nlppp->coll_[num1]);

Node<Pn> *node1, *node2;

node1 = (Node<Pn> *) coll->start;	// Try to get start of range.
node2 = (Node<Pn> *) coll->end;	// Try to get end of range.

if (!node1 || !node2)
	return NULL;	// 09/26/19 AM.

Node<Pn> *end = node2->Right();

// Traverse nodes looking for variable.
Node<Pn> *node;
for (node = node1; node != end; node = node->Right())
	{
	Ivar::nodeVar(node,name1,nlppp,/*UP*/sem);
	if (sem)		// FOUND VARIABLE.
		return sem;
	}
return sem;
}


RFASem *Arun::varinlist(
	Nlppp *nlppp,
	RFASem *name_sem,
	long num
	)
{
if (!name_sem)
	return NULL;	// 09/26/19 AM.
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return varinlist(nlppp,name,num);
}


RFASem *Arun::varinlist(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *num_sem
	)
{
if (!num_sem)
	return NULL;	// 09/26/19 AM.
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	return NULL;	// 09/26/19 AM.
return varinlist(nlppp,name,num);
}


RFASem *Arun::varinlist(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *num_sem
	)
{
if (!name_sem)
	{
	if (num_sem)
		delete num_sem;
	return NULL;	// 09/26/19 AM.
	}
if (!num_sem)
	{
	delete name_sem;
	return NULL;	// 09/26/19 AM.
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	return NULL;	// 09/26/19 AM.
return varinlist(nlppp,name,num);
}


/********************************************
* FN:		BATCHSTART
* CR:		01/08/01 AM.
* SUBJ:	Check if current file is first in a batch.
* RET:	True if ok, else false.
* FORMS:	bool batchstart()
* NOTE:	Could have been G("$batchstart") or somethin.
********************************************/

bool Arun::batchstart(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return false;

Parse *parse = nlppp->getParse();
if (!parse)
	return false;
return parse->getFbatchstart();

}

/********************************************
* FN:		INTERACTIVE
* CR:		05/06/02 AM.
* SUBJ:	Check if analyzer is in an interactive environment.
* RET:	True if ok, else false.
* FORMS:	bool interactive()
* NOTE:	Could have been G("$interactive") or somethin.
********************************************/

bool Arun::interactive(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return false;

Parse *parse = nlppp->getParse();
if (!parse)
	return false;
return parse->getAna()->getNLP()->getFinteractive();
}


/********************************************
* FN:		TOPDIR
* CR:		01/08/01 AM.
* SUBJ:	Get topmost directory in a file path.
* FORMS:	dir_str topdir(filepath_str)
*			dir_str - topmost folder or directory.
********************************************/

_TCHAR *Arun::topdir(
	Nlppp *nlppp,
	_TCHAR *filepath
	)
{
if (!nlppp)
	return 0;

if (!filepath || !*filepath)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[topdir: Empty filepath.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

// INCOMPLETE, UNATTACHED..... 10/20/00 AM.
_t_strstream gerrStr;
gerrStr << _T("[topdir: Unimplemented function.]") << ends;
errOut(&gerrStr,false);

return 0;
}


_TCHAR *Arun::topdir(
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return topdir(nlppp,name);
}


/********************************************
* FN:		UNPACKDIRS
* CR:		01/08/01 AM.
* SUBJ:	Unpack directory names from a full file path.
* FORMS:	dirs_list unpackdir(filepath_str)
*			dir_list - a MULTI-VALUE LIST of directory names.
********************************************/

RFASem *Arun::unpackdirs(
	Nlppp *nlppp,
	_TCHAR *filepath
	)
{
RFASem *sem=0;

if (!nlppp)
	return 0;

if (!filepath || !*filepath)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[unpackdirs: Empty filepath.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

Dlist<Iarg> *dirs = Fn::unpackdirs(filepath, nlppp->getParse());

if (dirs)
	sem = new RFASem(dirs);
return sem;
}

// VARIANT.
RFASem *Arun::unpackdirs(											// 05/04/01 AM.
	Nlppp *nlppp,
	RFASem *filepath_sem
	)
{
_TCHAR *filepath = filepath_sem->sem_to_str();
delete filepath_sem;
return unpackdirs(nlppp, filepath);
}

RFASem *Arun::unpackdirs(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:		RMATTRS
* CR:		01/08/01 AM.
* SUBJ:	Remove concept's attributes.
* RET:	True if ok, else false.
* FORMS:	rmattrs(conc)
********************************************/

bool Arun::rmattrs(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!nlppp)
	return false;

if (!sem1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmattrs: Warning. Given no concept.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[rmattrs: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem1;	// MEM LEAK.	// 05/14/08 AM.
	return false;
	}
CONCEPT *conc1 = sem1->getKBconcept();
delete sem1;	// MEM LEAK.	// 05/14/08 AM.

cg->rmAttrs(conc1);
return true;
}


/********************************************
* FN:		STRCLEAN
* CR:		01/08/01 AM.
* SUBJ:	Cleans the string of white space.
* RET:	True if ok, else false.
*			UP - returns the cleaned version of the string
* FORMS:	strclean(str1)
* NOTE:
********************************************/

_TCHAR *Arun::strclean(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!nlppp)
	return 0;

//char *name1=0;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strclean: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR buf[100000];
clean_str(name1, buf);

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);

// Return as str type.
return str;
}

_TCHAR *Arun::strclean(
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strclean(nlppp,name);
}

_TCHAR *Arun::strclean(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:		STRTRIM
* CR:		01/08/01 AM.
* SUBJ:	Trims the start and end white space from a string
* RET:	True if ok, else false.
*			UP - returns the trimmed version of the string
* FORMS:	strtrim(str1)
* NOTE:
********************************************/

_TCHAR *Arun::strtrim(
	Nlppp *nlppp,
	_TCHAR *name1
	)
{
if (!nlppp)
	return 0;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strtrim: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR buf[100000];
str_trim(name1, buf);

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);

return str;
}

_TCHAR *Arun::strtrim(
	Nlppp *nlppp,
	RFASem *name_sem
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strtrim(nlppp,name);
}

_TCHAR *Arun::strtrim(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:		STRSTARTSWITH
* CR:		08/05/21 Dd.
* SUBJ:		Checks to see if the string start with the given string.
* RET:		True if ok, else false.
*			UP - returns true if string has starting string, else false.
* FORMS:	strstartswith(str,end_str)
* NOTE:
********************************************/

bool Arun::strstartswith(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *startstr
	)
{
if (!nlppp || !startstr || !*startstr)
	return false;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strstartswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;																// 08/05/21 Dd
	}

return str_starts_with(name1, startstr);
}


// VARIANTS.																	// 08/05/21 Dd
bool Arun::strstartswith(														// 08/05/21 Dd.
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *startstr
	)
{
if (!name1_sem)
	return false;
if (!nlppp || !startstr || !*startstr)
	{
	delete name1_sem;
	return false;
	}

_TCHAR *name1 = sem_to_str(name1_sem);
delete name1_sem;
if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strstartswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

return str_starts_with(name1, startstr);
}

bool Arun::strstartswith(														// 08/05/21 Dd.
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *start_sem
	)
{
if (!start_sem)
	return false;
if (!nlppp || !name1 || !*name1)
	{
	delete start_sem;
	return false;
	}

_TCHAR *startstr = sem_to_str(start_sem);
delete start_sem;
if (!startstr || !*startstr)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strstartswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

return str_starts_with(name1, startstr);
}

bool Arun::strstartswith(														// 08/05/21 Dd.
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *start_sem
	)
{
if (!start_sem && !name1_sem)
	return false;
if (!start_sem)
	{
	delete name1_sem;
	return false;
	}
if (!name1_sem)
	{
	delete start_sem;
	return false;
	}
if (!nlppp)
	{
	delete name1_sem;
	delete start_sem;
	return false;
	}

_TCHAR *name1  = sem_to_str(name1_sem);
_TCHAR *startstr = sem_to_str(start_sem);
delete name1_sem;
delete start_sem;
if (!startstr || !*startstr || !name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strstartswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

return str_starts_with(name1, startstr);
}
////////////////////////////////
////////////////////////////////

/********************************************
* FN:		STRENDSWITH
* CR:		01/08/01 AM.
* SUBJ:	Checks to see if the string ends with the given ending.
* RET:	True if ok, else false.
*			UP - returns true if string has ending, else false.
* FORMS:	strendswith(str,end_str)
* NOTE:
********************************************/

bool Arun::strendswith(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *ending
	)
{
if (!nlppp || !ending || !*ending)
	return false;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strendswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;																// 04/28/01 AM.
	}

return str_ends_with(name1, ending);
}

// VARIANTS.																	// 04/28/01 AM.
bool Arun::strendswith(														// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *ending
	)
{
if (!name1_sem)
	return false;
if (!nlppp || !ending || !*ending)
	{
	delete name1_sem;
	return false;
	}

_TCHAR *name1 = sem_to_str(name1_sem);
delete name1_sem;
if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strendswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

return str_ends_with(name1, ending);
}

bool Arun::strendswith(														// 04/28/01 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *ending_sem
	)
{
if (!ending_sem)
	return false;
if (!nlppp || !name1 || !*name1)
	{
	delete ending_sem;
	return false;
	}

_TCHAR *ending = sem_to_str(ending_sem);
delete ending_sem;
if (!ending || !*ending)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strendswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

return str_ends_with(name1, ending);
}

bool Arun::strendswith(														// 04/28/01 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *ending_sem
	)
{
if (!ending_sem && !name1_sem)
	return false;
if (!ending_sem)
	{
	delete name1_sem;
	return false;
	}
if (!name1_sem)
	{
	delete ending_sem;
	return false;
	}
if (!nlppp)
	{
	delete name1_sem;
	delete ending_sem;
	return false;
	}

_TCHAR *name1  = sem_to_str(name1_sem);
_TCHAR *ending = sem_to_str(ending_sem);
delete name1_sem;
delete ending_sem;
if (!ending || !*ending || !name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strendswith: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

return str_ends_with(name1, ending);
}
////////////////////////////////
////////////////////////////////


/********************************************
* FN:		STRCHR
* CR:	   01/09/01 AM.
* SUBJ: Find first occurrence of char in string
* RET:  True if ok, else false.
*	 UP - returns string headed by found char.
* FORMS:	strchr(str, ch_str)
* NOTE:
********************************************/

//_TCHAR *Arun::_tcschr(
_TCHAR *Arun::strchr(	// FIXED mangled name.	// 05/14/08 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *ch_str
	)
{
if (!nlppp)
	return 0;

if (!name1 || !*name1 || !ch_str || !*ch_str)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strchr: Warning: Given no str or char.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (ch_str[1])
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strchr: Warning: 2nd arg must be string of length 1.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}


_TCHAR *str = ::_tcschr(name1, ch_str[0]);
if (!str || !*str)
	return 0;

// Don't know if I need to or should intern the partial string.
//char *instr=0;																// 04/28/01 AM.
//Parse *parse = nlppp->getParse();										// 04/28/01 AM.
//parse->internStr(str, /*UP*/ instr);									// 04/28/01 AM.

//return instr;																// 04/28/01 AM.
return str;																		// 04/28/01 AM.
}

// VARIANTS.																	// 04/28/01 AM.
_TCHAR *Arun::strchr(// 04/28/01 AM.	// FIXED	// 05/14/08 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *ch_str
	)
{
if (!name1_sem)
	return 0;
_TCHAR *name1 = sem_to_str(name1_sem);
delete name1_sem;
return strchr(nlppp,name1,ch_str);			// FIXED.	// 05/14/08 AM.
}


_TCHAR *Arun::strchr(	// 04/28/01 AM.	// FIXED	// 05/14/08 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *ch_str_sem
	)
{
if (!ch_str_sem)
	return 0;
_TCHAR *ch_str = sem_to_str(ch_str_sem);
delete ch_str_sem;
return strchr(nlppp,name1,ch_str);			// FIXED.	// 05/14/08 AM.
}


_TCHAR *Arun::strchr(	// 04/28/01 AM.	// FIXED.	// 05/14/08 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *ch_str_sem
	)
{
if (!name1_sem && !ch_str_sem)
	return 0;
if (!name1_sem)
	{
	delete ch_str_sem;
	return 0;
	}
if (!ch_str_sem)
	{
	delete name1_sem;
	return 0;
	}
_TCHAR *name1 = sem_to_str(name1_sem);
_TCHAR *ch_str = sem_to_str(ch_str_sem);
delete name1_sem;
delete ch_str_sem;
return strchr(nlppp,name1,ch_str);	// FIXED.	// 05/14/08 AM.
}


/********************************************
* FN:		STRCHRCOUNT
* CR:	   12/15/02 AM.
* SUBJ: Count occurrences of char in string
* RET:  True if ok, else false.
*	 UP - count.
* FORMS:	strchrcount(str, ch_str)
* NOTE:
********************************************/

long Arun::strchrcount(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *ch_str
	)
{
if (!nlppp)
	return 0;

if (!name1 || !*name1 || !ch_str || !*ch_str)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strchrcount: Warning: Given no str or char.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (ch_str[1])
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strchrcount: Warning: 2nd arg must be string of length 1.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}


long count = 0;
for (; *name1; ++name1)
	if (*name1 == *ch_str)
		++count;

return count;
}

// VARIANTS.																	// 12/15/02 AM.
long Arun::strchrcount(														// 12/15/02 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *ch_str
	)
{
if (!name1_sem)
	return 0;
_TCHAR *name1 = sem_to_str(name1_sem);
delete name1_sem;
return strchrcount(nlppp,name1,ch_str);
}


long Arun::strchrcount(														// 12/15/02 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *ch_str_sem
	)
{
if (!ch_str_sem)
	return 0;
_TCHAR *ch_str = sem_to_str(ch_str_sem);
delete ch_str_sem;
return strchrcount(nlppp,name1,ch_str);
}


long Arun::strchrcount(														// 12/15/02 AM.
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *ch_str_sem
	)
{
if (!name1_sem && !ch_str_sem)
	return 0;
if (!name1_sem)
	{
	delete ch_str_sem;
	return 0;
	}
if (!ch_str_sem)
	{
	delete name1_sem;
	return 0;
	}
_TCHAR *name1 = sem_to_str(name1_sem);
_TCHAR *ch_str = sem_to_str(ch_str_sem);
delete name1_sem;
delete ch_str_sem;
return strchrcount(nlppp,name1,ch_str);
}


/********************************************
* FN:		STRRCHR
* CR:	   01/09/01 AM.
* SUBJ: Find last occurrence of char in string
* RET:  True if ok, else false.
*	 UP - returns string headed by last found char.
* FORMS:	strrchr(str, ch_str)
* NOTE:	FIXED. strrchr was mangeld to _tcsrchr by a replace edit.  05/14/08 AM.
********************************************/

_TCHAR *Arun::strrchr(	// FIXED.	// 05/14/08 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *ch_str
	)
{
if (!nlppp)
	return 0;

if (!name1 || !*name1 || !ch_str || !*ch_str)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strrchr: Warning: Given no str or char.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}
if (ch_str[1])
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strrchr: Warning: 2nd arg must be string of length 1.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}


_TCHAR *str = ::_tcsrchr(name1, ch_str[0]);
if (!str || !*str)
	return 0;

// Don't know if I need to or should intern the partial string.
_TCHAR *instr=0;
Parse *parse = nlppp->getParse();
parse->internStr(str, /*UP*/ instr);

return instr;
}


_TCHAR *Arun::strrchr(	// FIXED.	// 05/14/08 AM.
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *ch_str
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strrchr(nlppp,name,ch_str);	// FIXED.	// 05/14/08 AM.
}


_TCHAR *Arun::strrchr(	// FIXED.	// 05/14/08 AM.
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *ch_sem
	)
{
if (!ch_sem)
	return 0;
_TCHAR *ch_str = ch_sem->sem_to_str();
delete ch_sem;
return strrchr(nlppp,name1,ch_str);	// FIXED.	// 05/14/08 AM.
}


_TCHAR *Arun::strrchr(	// FIXED.	// 05/14/08 AM.
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *ch_sem
	)
{
if (!name_sem)
	{
	if (ch_sem)
		delete ch_sem;
	return 0;
	}
if (!ch_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *ch_str = ch_sem->sem_to_str();
delete ch_sem;
return strrchr(nlppp,name,ch_str);
}

/********************************************
* FN:		STRWRAP
* CR:	   01/09/01 Dd.
* SUBJ: Takes a long string and wraps it.
* RET:  True if ok, else false.
*		       UP - returns the wrapped version of the string
* FORMS:	strwrap(str1,int)
* NOTE:
********************************************/

_TCHAR *Arun::strwrap(
	Nlppp *nlppp,
	_TCHAR *name1,
	long wrapSize
	)
{
if (!nlppp)
	return 0;

if (wrapSize < 4)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strwrap: Warning: Wrap size too small.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strwrap: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR buf[100000];
str_wrap(name1, wrapSize, buf);

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);

return str;
}


_TCHAR *Arun::strwrap(
	Nlppp *nlppp,
	RFASem *name_str,
	long wrap
	)
{
if (!name_str)
	return 0;
_TCHAR *name = name_str->sem_to_str();
delete name_str;
return strwrap(nlppp,name,wrap);
}


_TCHAR *Arun::strwrap(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *wrap_sem
	)
{
if (!wrap_sem)
	return 0;
bool ok = false;
long wrap = wrap_sem->sem_to_long(ok);
delete wrap_sem;
if (!ok)
	return 0;
return strwrap(nlppp,name,wrap);
}


_TCHAR *Arun::strwrap(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *wrap_sem
	)
{
if (!name_sem)
	{
	if (wrap_sem)
		delete wrap_sem;
	return 0;
	}
if (!wrap_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
bool ok = false;
long wrap = wrap_sem->sem_to_long(ok);
delete wrap_sem;
if (!ok)
	return 0;
return strwrap(nlppp,name,wrap);
}


/********************************************
* FN:		STRSUBST
* CR:	   01/09/01 AM.
* SUBJ: Replaces a substring of another string.
* RET:  True if ok, else false.
*		       UP - returns the substituted version of the string
* FORMS:	strsubst(str, oldstr, newstr)
* NOTE:
********************************************/

_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *oldStr,
	_TCHAR *newStr
	)
{
if (!nlppp || !oldStr || !*oldStr
// || !newStr || !*newStr	// OK to replace with empty str.	// FIX.	// 07/12/08 AM.
	)
	return 0;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strsubst: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR buf[100000];
str_subst(name1, oldStr, newStr, buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);

return str;
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *sold,
	_TCHAR *snew
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *sold_sem,
	_TCHAR *snew
	)
{
if (!sold_sem)
	return 0;
_TCHAR *sold = sold_sem->sem_to_str();
delete sold_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	_TCHAR *name,
	_TCHAR *sold,
	RFASem *snew_sem
	)
{
if (!snew_sem)
	return 0;
_TCHAR *snew = snew_sem->sem_to_str();
delete snew_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *sold_sem,
	_TCHAR *snew
	)
{
if (!name_sem)
	{
	if (sold_sem)
		delete sold_sem;
	return 0;
	}
if (!sold_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *sold = sold_sem->sem_to_str();
delete sold_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *sold,
	RFASem *snew_sem
	)
{
if (!name_sem)
	{
	if (snew_sem)
		delete snew_sem;
	return 0;
	}
if (!snew_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *snew = snew_sem->sem_to_str();
delete snew_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *sold_sem,
	RFASem *snew_sem
	)
{
if (!sold_sem)
	{
	if (snew_sem)
		delete snew_sem;
	return 0;
	}
if (!snew_sem)
	{
	delete sold_sem;
	return 0;
	}
_TCHAR *sold = sold_sem->sem_to_str();
delete sold_sem;
_TCHAR *snew = snew_sem->sem_to_str();
delete snew_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *sold_sem,
	RFASem *snew_sem
	)
{
if (!name_sem)
	{
	if (sold_sem)
		delete sold_sem;
	if (snew_sem)
		delete snew_sem;
	return 0;
	}
if (!sold_sem)
	{
	delete name_sem;
	if (snew_sem)
		delete snew_sem;
	return 0;
	}
if (!snew_sem)
	{
	delete name_sem;
	delete sold_sem;
	return 0;
	}

_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *sold = sold_sem->sem_to_str();
delete sold_sem;
_TCHAR *snew = snew_sem->sem_to_str();
delete snew_sem;
return strsubst(nlppp,name,sold,snew);
}


_TCHAR *Arun::strsubst(	// 07/12/08 AM.
	Nlppp *nlppp,
	_TCHAR *name,
	_TCHAR *sold,
	long lnew
	)
{
if (lnew != 0)	// Error.
	return 0;

return strsubst(nlppp,name,sold,(_TCHAR *)0);
}
_TCHAR *Arun::strsubst(	// 07/12/08 AM.
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *sold,
	long lnew
	)
{
if (!name_sem)
	return 0;

_TCHAR *name = name_sem->sem_to_str();
delete name_sem;

if (lnew != 0)
	return 0;
return strsubst(nlppp,name,sold,(_TCHAR*)0);
}


_TCHAR *Arun::strsubst(	// 07/12/08 AM.
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *sold_sem,
	long lnew
	)
{
if (!sold_sem)
	return 0;
_TCHAR *sold = sold_sem->sem_to_str();
delete sold_sem;
if (lnew != 0)
	return 0;
return strsubst(nlppp,name,sold,(_TCHAR*)0);
}

_TCHAR *Arun::strsubst(	// 07/12/08 AM.
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *sold_sem,
	long lnew
	)
{
if (!name_sem)
	{
	if (sold_sem)
		delete sold_sem;
	return 0;
	}
if (!sold_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *sold = sold_sem->sem_to_str();
delete sold_sem;
if (lnew != 0)
	return 0;
return strsubst(nlppp,name,sold,(_TCHAR*)0);
}

/********************************************
* FN:		OPENFILE
* CR:		01/09/01 AM.
* SUBJ:	Open an output file.
* RET:	True if ok, else false.
*			UP sem - semantic object of type OSTREAM.
* FORM:	openfile(path_str [, a_str [, b_str [, c_str]]]);
*			path_str - file name.
*			*_str - in random order: (up to three flags handled, for now).
*				USING C++ MODES.
*			"app"			= append.
*			"ate"			= weird append.
*			"nocreate"	= if file doesn't exist, fail.
*			"noreplace" = if file exists, fail.
*			"binary"		= open in binary mode (text is default).
* NOTE:	Unlike fileout, this doesn't create a "file variable".
* WARN:	Default mode is WRITE, not APPEND.  Unlike fileout in this also.
********************************************/

RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *a,
	_TCHAR *b,
	_TCHAR *c
	)
{
RFASem *sem = 0;
//char *fname = 0;
_TCHAR *flag[3];	// Array of flag strings.
flag[0] = flag[1] = flag[2] = 0;

if (!nlppp || !fname || !*fname)
	return 0;

if (a && *a)
	{
	flag[0] = a;
	if (b && *b)
		{
		flag[1] = b;
		if (c && *c)
			flag[2] = c;
		}
	}

// Now figure out what all them flags are.
// Track redundancy, consistency.
int napp = 0;
int nate = 0;
int nnocreate = 0;
int nnoreplace = 0;
int nbinary = 0;

int ii;
_TCHAR *tmp;
for (ii = 0; ii < 2; ++ii)  // Check out each of the three flags.
	{
	tmp = flag[ii];
	if (!tmp)
		;							// Skip.
	else if (!strcmp_i(tmp, _T("app")))
		{
		if (napp)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Redundant 'app' mode.]") << ends;
			errOut(&gerrStr,false);
			}

		if (nate)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Incompatible modes 'ate' and 'app'.]") << ends;
			errOut(&gerrStr,false);
			return 0;
			}
		++napp;
		}
	else if (!strcmp_i(tmp, _T("ate")))
		{
		if (nate)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Redundant 'ate' mode.]") << ends;
			errOut(&gerrStr,false);
			}

		if (nate)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Incompatible modes 'app' and 'ate'.]") << ends;
			errOut(&gerrStr,false);
			return 0;
			}
		++nate;
		}
	else if (!strcmp_i(tmp, _T("nocreate")))
		{
		if (nnocreate)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Redundant 'nocreate' mode.]") << ends;
			errOut(&gerrStr,false);
			}

		++nnocreate;
		}
	else if (!strcmp_i(tmp, _T("noreplace")))
		{
		if (nnoreplace)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Redundant 'noreplace' mode.]") << ends;
			errOut(&gerrStr,false);
			}

		++nnoreplace;
		}
	else if (!strcmp_i(tmp, _T("binary")))
		{
		if (nbinary)
			{
			_t_strstream gerrStr;
			gerrStr << _T("[openfile: Redundant 'binary' mode.]") << ends;
			errOut(&gerrStr,false);
			}

		++nbinary;
		}
	}

// Check logical consistency of flags.
// eg, write, but don't create and don't overwrite is a bad combo!
if (!napp && !nate && nnoreplace && nnocreate)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[openfile: Modes disallow writing out a file.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Is filename relative or absolute?
// Todo: Need better filename parsing.
bool absolute = ( (*(fname+1) == ':') ? true : false);

// Get user specified filename.  Taking name as is.
// If relative path, $appdir\output is base directory.
// If absolute, use it as is.

if (!fname || !*fname)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[openfile: Error. Given null filename.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Build up a bit-flag vector for the modes.
#ifdef __linux__
std::_Ios_Openmode modes;
#else
int modes = 0;
#endif
if (napp)
	modes |= ios::app;
if (nate)
	modes |= ios::ate;
//if (nnocreate)
//	modes |= ios::nocreate;
//if (nnoreplace)
//	modes |= ios::noreplace;
if (nbinary)
	modes |= ios::binary;

Parse *parse = nlppp->getParse();
// Set up proper path for file.
// Should check if it's relative or absolute.
_TCHAR buf[MAXSTR];
_TCHAR *str = 0;
if (!absolute)		// Relative path.
	{
	_stprintf(buf, _T("%s%c%s"), parse->getOutdir(),DIR_CH, fname);
	parse->internStr(buf, /*UP*/ str);
	}
else					// Absolute path.
	parse->internStr(fname, /*UP*/ str);

// Open the output file.
_t_ostream *ostr = new _t_ofstream(TCHAR2CA(str), modes);
parse->newostr(ostr);			// Add to list of ostrs.			// 05/23/01 AM.

if (!ostr)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[openfile: Couldn't create output stream.]") << ends;
	errOut(&gerrStr,false);
	gerrStr << _T("[file=") << str << _T("]") << ends;
	errOut(&gerrStr,false);
	return NULL;	// 09/26/19 AM.
	}

return new RFASem(ostr);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	RFASem *a_sem,
	_TCHAR *b,
	_TCHAR *c
	)
{
if (!a_sem)
	return 0;
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *a,
	RFASem *b_sem,
	_TCHAR *c
	)
{
if (!b_sem)
	return 0;
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *a,
	_TCHAR *b,
	RFASem *c_sem
	)
{
if (!c_sem)
	return 0;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	RFASem *a_sem,
	RFASem *b_sem,
	_TCHAR *c
	)
{
if (!a_sem)
	{
	if (b_sem)
		delete b_sem;
	return 0;
	}
if (!b_sem)
	{
	delete a_sem;
	return 0;
	}
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	_TCHAR *a,
	RFASem *b_sem,
	RFASem *c_sem
	)
{
if (!b_sem)
	{
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	if (b_sem)
		delete b_sem;
	return 0;
	}
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	RFASem *a_sem,
	_TCHAR *b,
	RFASem *c_sem
	)
{
if (!a_sem)
	{
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	delete a_sem;
	return 0;
	}
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	_TCHAR *fname,
	RFASem *a_sem,
	RFASem *b_sem,
	RFASem *c_sem
	)
{
if (!a_sem)
	{
	if (b_sem)
		delete b_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!b_sem)
	{
	delete a_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	delete a_sem;
	delete b_sem;
	return 0;
	}
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


// VARIANT.
RFASem *Arun::openfile(															// 05/05/01 AM.
	Nlppp *nlppp,
	RFASem *fname_sem,
	_TCHAR *a,
	_TCHAR *b,
	_TCHAR *c
	)
{
if (!fname_sem)
	return 0;
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	RFASem *a_sem,
	_TCHAR *b,
	_TCHAR *c
	)
{
if (!fname_sem)
	{
	if (a_sem)
		delete a_sem;
	return 0;
	}
if (!a_sem)
	{
	delete fname_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	_TCHAR *a,
	RFASem *b_sem,
	_TCHAR *c
	)
{
if (!fname_sem)
	{
	if (b_sem)
		delete b_sem;
	return 0;
	}
if (!b_sem)
	{
	delete fname_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	_TCHAR *a,
	_TCHAR *b,
	RFASem *c_sem
	)
{
if (!fname_sem)
	{
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	delete fname_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	RFASem *a_sem,
	RFASem *b_sem,
	_TCHAR *c
	)
{
if (!fname_sem)
	{
	if (a_sem)
		delete a_sem;
	if (b_sem)
		delete b_sem;
	return 0;
	}
if (!a_sem)
	{
	delete fname_sem;
	if (b_sem)
		delete b_sem;
	return 0;
	}
if (!b_sem)
	{
	delete fname_sem;
	delete a_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	_TCHAR *a,
	RFASem *b_sem,
	RFASem *c_sem
	)
{
if (!fname_sem)
	{
	if (b_sem)
		delete b_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!b_sem)
	{
	delete fname_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	delete fname_sem;
	if (b_sem)
		delete b_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	RFASem *a_sem,
	_TCHAR *b,
	RFASem *c_sem
	)
{
if (!fname_sem)
	{
	if (a_sem)
		delete a_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!a_sem)
	{
	delete fname_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	delete fname_sem;
	delete a_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


RFASem *Arun::openfile(
	Nlppp *nlppp,
	RFASem *fname_sem,
	RFASem *a_sem,
	RFASem *b_sem,
	RFASem *c_sem
	)
{
if (!fname_sem)
	{
	if (a_sem)
		delete a_sem;
	if (b_sem)
		delete b_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!a_sem)
	{
	delete fname_sem;
	if (b_sem)
		delete b_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!b_sem)
	{
	delete fname_sem;
	delete a_sem;
	if (c_sem)
		delete c_sem;
	return 0;
	}
if (!c_sem)
	{
	delete fname_sem;
	delete a_sem;
	delete b_sem;
	return 0;
	}
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
_TCHAR *a = a_sem->sem_to_str();
delete a_sem;
_TCHAR *b = b_sem->sem_to_str();
delete b_sem;
_TCHAR *c = c_sem->sem_to_str();
delete c_sem;
return openfile(nlppp,fname,a,b,c);
}


// VARIANT.
RFASem *Arun::openfile( // 07/26/11 AM.
	Nlppp *nlppp,
	RFASem *fname_sem,
	long a,
	long b,
	long c
	)
{
if (!fname_sem)
	return 0;
_TCHAR *fname = fname_sem->sem_to_str();
delete fname_sem;
// Todo: If these nums aren't zero, flag error.
return openfile(nlppp,fname,(_TCHAR*)0,(_TCHAR*)0,(_TCHAR*)0);
}


// VARIANT.
RFASem *Arun::openfile( // 07/26/11 AM.
	Nlppp *nlppp,
	_TCHAR *fname,
	long a,
	long b,
	long c
	)
{
// Todo: If these nums aren't zero, flag error.
return openfile(nlppp,fname,(_TCHAR*)0,(_TCHAR*)0,(_TCHAR*)0);
}

/********************************************
* FN:		CLOSEFILE
* CR:		05/21/01 AM.
* SUBJ:	Close file stream.
* RET:	True if ok, else false.
* NOTE:	Should work for input streams, if we ever build 'em.
********************************************/

bool Arun::closefile(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem)
	return false;
if (!nlppp)
	{
	delete sem;
	return false;
	}

if (sem->getType() != RSOSTREAM)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[closefile: Error. Given bad arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return false;
	}
_t_ostream *ostr = sem->getOstream();
delete sem;
//delete ostr;	// DELETE THE OSTREAM.
nlppp->parse_->deleteostr(ostr);	// Rm from list and delete.	// 05/23/01 AM.
return true;
}

bool Arun::closefile(
	Nlppp *nlppp,
	_t_ostream *ostr
	)
{
if (!ostr)
	return false;
if (!nlppp)
	{
//	delete ostr;																// 05/23/01 AM.
	_t_strstream gerrStr;
	gerrStr << _T("[Internal error. Can't delete ostream.]") << ends;
	errOut(&gerrStr,false);
	// 05/23/01 AM.
	return false;
	}
//delete ostr;																	// 05/23/01 AM.
nlppp->parse_->deleteostr(ostr);	// Rm from list and delete.	// 05/23/01 AM.
return true;
}


/********************************************
* FN:		STRSPELLCANDIDATE
* CR:		01/09/01 AM.
* SUBJ:	Choose the best candidate from a string list
* RET:	True if ok, else false.
*			UP - returns the best candidate.
* FORMS:	strspellcandidate(str1,strList)
* ASS:	strList is already all-lowercase.
********************************************/

_TCHAR *Arun::strspellcandidate(
	Nlppp *nlppp,
	_TCHAR *str1,
	_TCHAR *str2
	)
{
if (!nlppp || !str1 || !*str1 || !str2 || !*str2)
	return 0;

// Remember the case of word, if all caps or capitalized.		// 09/28/00 AM.
bool allcap = all_uppercase(str1);										// 09/28/00 AM.
bool cap = (is_upper((_TUCHAR)*str1) ? true : false);		// 12/16/01 AM.
// Forget about mix cap!

// Convert to lowercase.													// 09/28/00 AM.
_TCHAR *lcstr1 = Chars::create(_tcsclen(str1) + 1);						// 09/28/00 AM.
str_to_lower(str1, lcstr1);												// 09/28/00 AM.

_TCHAR buf[SPELL_BUF_LEN+1];
buf[0] = '\0';
str_spell_candidate(lcstr1, str2, buf);

Chars::destroy(lcstr1);														// 09/28/00 AM.

// _TCHAR *str = '\0';
_TCHAR *str = "";	// 09/26/19 AM.
if (buf[0] == '\0')															// 09/28/00 AM.
	return 0;

Parse *parse = nlppp->getParse();

// Convert best candidate to the letter case of given word.		// 09/28/00 AM.
if (allcap)	// Orig word was all caps.									// 09/28/00 AM.
	{
	_TCHAR *ucstr = Chars::create(_tcsclen(buf) + 1);					// 09/28/00 AM.
	str_to_upper(buf, ucstr);												// 09/28/00 AM.
	parse->internStr(ucstr, /*UP*/ str);								// 09/28/00 AM.
	Chars::destroy(ucstr);													// 09/28/00 AM.
	}
else if (cap)			// Orig word was capitalized.					// 09/28/00 AM.
	{
	buf[0] = to_upper(buf[0]);												// 12/16/01 AM.
	parse->internStr(buf, /*UP*/ str);
	}
else	// lower case. (mix cap falls in here also.)					// 09/28/00 AM.
	parse->internStr(buf, /*UP*/ str);

return str;
}


_TCHAR *Arun::strspellcandidate(
	Nlppp *nlppp,
	RFASem *str1_sem,
	_TCHAR *str2
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return strspellcandidate(nlppp,str1,str2);
}


_TCHAR *Arun::strspellcandidate(
	Nlppp *nlppp,
	_TCHAR *str1,
	RFASem *str2_sem
	)
{
if (!str2_sem)
	return 0;
_TCHAR *str2 = str2_sem->sem_to_str();
delete str2_sem;
return strspellcandidate(nlppp,str1,str2);
}


_TCHAR *Arun::strspellcandidate(
	Nlppp *nlppp,
	RFASem *str1_sem,
	RFASem *str2_sem
	)
{
if (!str1_sem)
	{
	if (str2_sem)
		delete str2_sem;
	return 0;
	}
if (!str2_sem)
	{
	delete str1_sem;
	return 0;
	}
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
_TCHAR *str2 = str2_sem->sem_to_str();
delete str2_sem;
return strspellcandidate(nlppp,str1,str2);
}

/********************************************
* FN:		STRSPELLCOMPARE
* CR:		01/09/01 AM.
* SUBJ:	Compares two strings and returns their spell "distance".
* RET:	True if ok, else false.
*			UP - returns the wrapped version of the string
* FORMS:	strspellcompare(str1,str2)
* NOTE:
********************************************/

long Arun::strspellcompare(
	Nlppp *nlppp,
	_TCHAR *str1,
	_TCHAR *str2
	)
{
if (!nlppp || !str1 || !*str1 || !str2 || !*str2)
	return 0;														// FIX.	// 08/11/02 AM.

// Convert both strings to lowercase.									// 09/28/00 AM.
_TCHAR *lcstr1 = Chars::create(_tcsclen(str1) + 1);						// 09/28/00 AM.
_TCHAR *lcstr2 = Chars::create(_tcsclen(str2) + 1);						// 09/28/00 AM.
str_to_lower(str1, lcstr1);												// 09/28/00 AM.
str_to_lower(str2, lcstr2);												// 09/28/00 AM.

int weight = str_spell_compare(lcstr1, lcstr2);

Chars::destroy(lcstr1);														// 09/28/00 AM.
Chars::destroy(lcstr2);														// 09/28/00 AM.

return weight;
}


long Arun::strspellcompare(
	Nlppp *nlppp,
	RFASem *str1_sem,
	_TCHAR *str2
	)
{
if (!str1_sem)
	return 0;
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
return strspellcompare(nlppp,str1,str2);
}


long Arun::strspellcompare(
	Nlppp *nlppp,
	_TCHAR *str1,
	RFASem *str2_sem
	)
{
if (!str2_sem)
	return 0;
_TCHAR *str2 = str2_sem->sem_to_str();
delete str2_sem;
return strspellcompare(nlppp,str1,str2);
}


long Arun::strspellcompare(
	Nlppp *nlppp,
	RFASem *str1_sem,
	RFASem *str2_sem
	)
{
if (!str1_sem)
	{
	if (str2_sem)
		delete str2_sem;
	return 0;
	}
if (!str2_sem)
	{
	delete str1_sem;
	return 0;
	}
_TCHAR *str1 = str1_sem->sem_to_str();
delete str1_sem;
_TCHAR *str2 = str2_sem->sem_to_str();
delete str2_sem;
return strspellcompare(nlppp,str1,str2);
}

/********************************************
* FN:		SPELLCANDIDATES
* CR:		01/09/01 AM.
* SUBJ:	Find candidates for given string.
* RET:	True if ok.
*			UP sem - contains string-list of candidates.
* FORMS:	spellcandidates(badword_str)
* NOTE:	Returns all lowercase candidates.
********************************************/

_TCHAR *Arun::spellcandidates(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
if (!nlppp || !str || !*str)
	return 0;

// Convert to lowercase.													// 09/28/00 AM.
_TCHAR *lcstr = Chars::create(_tcsclen(str) + 1);						// 09/28/00 AM.
str_to_lower(str, lcstr);													// 09/28/00 AM.

_TCHAR buf[SPELL_BUF_LEN];
buf[0] = '\0';																	// 09/28/00 AM.
spell_candidates(lcstr, buf, SPELL_BUF_LEN);							// 10/16/00 AM.
Chars::destroy(lcstr);														// 09/28/00 AM.

if (buf[0] == '\0')															// 09/28/00 AM.
	return 0;

Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
return str;
}


_TCHAR *Arun::spellcandidates(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return 0;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return spellcandidates(nlppp,str);
}

_TCHAR *Arun::spellcandidates(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:		SPELLWORD
* CR:		01/09/01 AM.
* SUBJ:	See if given word is in the spell word list.
* RET:	True if ok.
*			UP sem - 1 if found, 0 if not found.
* FORMS:	spellword(word_str)
********************************************/

bool Arun::spellword(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
bool flag = false;

//if (!nlppp)			// DON'T CARE.	// 08/22/08 AM.
//	return false;	// DON'T CARE.	// 08/22/08 AM.

if (!str || !*str)
	return false;

// Convert to lowercase.
long len = _tcsclen(str);
if (len > MAXWORDLENGTH)										// FIX	// 11/13/01 AM.
	return false;													// FIX	// 11/13/01 AM.

_TCHAR *lcstr = Chars::create(len + 1);
str_to_lower(str, lcstr);

long pos;
//int ans;
flag = (binary_spell(lcstr, word_arrays[len],word_lengths[len], /*UP*/pos)
	? true : false);
Chars::destroy(lcstr);

return flag;
}

// VARIANT.																		// 01/23/01 AM.
bool Arun::spellword(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem)
	return false;

_TCHAR *st = 0;
switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		st = sem->getName();
		break;
	default:
		break;
	}
delete sem;
if (!st || !*st)
	return false;
return spellword(nlppp,st);
}

bool Arun::spellword(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}


/********************************************
* FN:		SPELLCORRECT
* CR:		01/09/01 AM.
* SUBJ:	If not on spell word list, then correct word.  Else return as is.
* RET:	True if ok.
*			UP sem - corrected word.
* FORMS:	spellcorrect(word_str)
********************************************/

_TCHAR *Arun::spellcorrect(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
if (!nlppp || !str || !*str)
	return 0;

// Remember the case of word, if all caps or capitalized.		// 09/28/00 AM.
bool allcap = all_uppercase(str);										// 09/28/00 AM.
bool cap = (is_upper((_TUCHAR)*str) ? true : false);		// 12/16/01 AM.
// Forget about mix cap!

// Convert to lowercase.													// 09/28/00 AM.
long len = _tcsclen(str);														// 10/16/00 AM.
if (len > MAXWORDLENGTH)			// Forget it.			// FIX	// 11/13/01 AM.
	return 0;														// FIX	// 11/13/01 AM.

_TCHAR *lcstr = Chars::create(len + 1);									// 09/28/00 AM.
str_to_lower(str, lcstr);													// 09/28/00 AM.

Parse *parse = nlppp->getParse();
_TCHAR *istr = 0;

_TCHAR buf2[1001];
long pos;
if (!binary_spell(lcstr, word_arrays[len],word_lengths[len], /*UP*/pos))
	{
	// Get corrected spell here.
	_TCHAR buf[SPELL_BUF_LEN];
	buf[0] = '\0';																// 09/28/00 AM.
	spell_candidates(lcstr, buf, SPELL_BUF_LEN);
	if (buf[0] == '\0')		// No candidates.							// 09/28/00 AM.
		{
		Chars::destroy(lcstr);												// 09/28/00 AM.
		return str;
		}

	buf2[0] = '\0';
	str_spell_candidate(lcstr, buf, buf2);
	Chars::destroy(lcstr);													// 09/28/00 AM.
	str = buf2;
	}
else			// Word is known.												// 09/28/00 AM.
	{
	Chars::destroy(lcstr);													// 09/28/00 AM.
	parse->internStr(str, /*UP*/ istr);
	return istr;
	}

if (!str || !*str)
	return 0;

// Convert best candidate to the letter case of given word.		// 09/28/00 AM.
if (allcap)	// Orig word was all caps.									// 09/28/00 AM.
	{
	_TCHAR *ucstr = Chars::create(_tcsclen(str) + 1);					// 09/28/00 AM.
	str_to_upper(str, ucstr);												// 09/28/00 AM.
	parse->internStr(ucstr, /*UP*/ istr);								// 09/28/00 AM.
	Chars::destroy(ucstr);													// 09/28/00 AM.
	}
else if (cap)			// Orig word was capitalized.					// 09/28/00 AM.
	{
	*str = to_upper(*str);													// 12/16/01 AM.
	parse->internStr(str, /*UP*/ istr);
	}
else	// lower case. (mix cap falls in here also.)					// 09/28/00 AM.
	parse->internStr(str, /*UP*/ istr);

return istr;
}


_TCHAR *Arun::spellcorrect(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
if (!str_sem)
	return 0;
_TCHAR *str = str_sem->sem_to_str();
delete str_sem;
return spellcorrect(nlppp,str);
}

_TCHAR *Arun::spellcorrect(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}



/********************************************
* FN:		LJ
* CR:		01/09/01 AM.
* SUBJ:	Left justify STRING in given field size.
* RET:	str - of given field size, with number right justified.
*				If number is larger than size, then wider string is returned.
* FORMS:	LJ(str, fieldsize_num)
* TODO:	Write one function for for all the types (str, num).
********************************************/

_TCHAR *Arun::lj(
	Nlppp *nlppp,
	_TCHAR *sptr,
	long fieldsize
	)
{
if (!nlppp || fieldsize < 1)
	return 0;

if (!sptr || !*sptr)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[LJ: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR *buf=0;
if (!(buf = leftjustify(sptr,fieldsize)))
	return 0;
if (!*buf)															// FIX!	// 05/01/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str=0;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

return str;
}


// VARIANT.																		// 01/23/01 AM.
_TCHAR *Arun::lj(
	Nlppp *nlppp,
	RFASem *sem,
	long fieldsize
	)
{
_TCHAR *st = 0;
if (!sem)
	return 0;

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		st = sem->getName();
		break;
	default:
		break;
	}
delete sem;
return lj(nlppp,st,fieldsize);
}


_TCHAR *Arun::lj(
	Nlppp *nlppp,
	_TCHAR *sptr,
	RFASem *field_sem
	)
{
if (!field_sem)
	return 0;
bool ok = false;
long field = field_sem->sem_to_long(ok);
delete field_sem;
if (!ok)
	return 0;
return lj(nlppp,sptr,field);
}


_TCHAR *Arun::lj(
	Nlppp *nlppp,
	RFASem *sptr_sem,
	RFASem *field_sem
	)
{
if (!sptr_sem)
	{
	if (field_sem)
		delete field_sem;
	return 0;
	}
if (!field_sem)
	{
	delete sptr_sem;
	return 0;
	}
_TCHAR *sptr = sptr_sem->sem_to_str();
delete sptr_sem;
bool ok = false;
long field = field_sem->sem_to_long(ok);
delete field_sem;
if (!ok)
	return 0;
return lj(nlppp,sptr,field);
}

// VARIANT (spelling)
_TCHAR *Arun::LJ(
	Nlppp *nlppp,
	_TCHAR *sptr,
	long fieldsize
	)
{
return lj(nlppp,sptr,fieldsize);
}


// VARIANT.																		// 01/23/01 AM.
_TCHAR *Arun::LJ(
	Nlppp *nlppp,
	RFASem *sem,
	long fieldsize
	)
{
return lj(nlppp,sem,fieldsize);
}

// VARIANT.																		// 01/23/01 AM.
_TCHAR *Arun::LJ(
	Nlppp *nlppp,
	_TCHAR *sptr,
	RFASem *sem
	)
{
long num = 0;
_TCHAR *st = 0;
if (!sem)
	return 0;

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		st = sem->getName();
		if (!str_to_long(st, /*UP*/ num))
			{
			delete sem;
			return 0;
			}
		break;
	case RSLONG:
		num = sem->getLong();
		break;
	default:
		break;
	}
delete sem;
return LJ(nlppp,sptr,num);
}

_TCHAR *Arun::LJ(
	Nlppp *nlppp,
	RFASem *sptr_sem,
	RFASem *field_sem
	)
{
return lj(nlppp,sptr_sem,field_sem);
}


/********************************************
* FN:		RIGHTJUSTIFYNUM
* CR:		01/09/01 AM.
* SUBJ:	Right justify number in given field size.
* RET:	str - of given field size, with number right justified.
*				If number is larger than size, then wider string is returned.
* FORMS:	rightjustifynum(num, fieldsize_num)
********************************************/

_TCHAR *Arun::rightjustifynum(
	Nlppp *nlppp,
	long num,
	long fieldsize
	)
{
if (!nlppp || fieldsize < 1)
	return 0;

_TCHAR *buf=0;
if (!(buf = rightjustify(num,fieldsize)))
	return 0;
if (!*buf)															// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str=0;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

return str;
}

// VARIANT.																		// 01/23/01 AM.
_TCHAR *Arun::rightjustifynum(
	Nlppp *nlppp,
	RFASem *sem,
	long fieldsize
	)
{
long num = 0;
_TCHAR *st = 0;
if (!sem)
//	return 0;																	// 11/19/02 AM.
	return rightjustifynum(nlppp,(long)0,fieldsize);				// 11/19/02 AM.

switch (sem->getType())
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		st = sem->getName();
		if (!str_to_long(st, /*UP*/ num))
			{
			delete sem;
			return 0;
			}
		break;
	case RSLONG:
		num = sem->getLong();
		break;
	default:
		break;
	}
delete sem;
return rightjustifynum(nlppp,num,fieldsize);
}

_TCHAR *Arun::rightjustifynum(
	Nlppp *nlppp,
	long num,
	RFASem *field_sem
	)
{
if (!field_sem)
	return 0;
bool ok = false;
long field = field_sem->sem_to_long(ok);
delete field_sem;
if (!ok)
	return 0;
return rightjustifynum(nlppp,num,field);
}


_TCHAR *Arun::rightjustifynum(
	Nlppp *nlppp,
	RFASem *num_sem,
	RFASem *field_sem
	)
{
if (!num_sem)
	{
	if (field_sem)
		delete field_sem;
	return 0;
	}
if (!field_sem)
	{
	delete num_sem;
	return 0;
	}
bool ok1 = false;
long num = num_sem->sem_to_long(ok1);
delete num_sem;
if (!ok1)
	{
	if (field_sem)
		delete field_sem;
	return 0;
	}
bool ok = false;
long field = field_sem->sem_to_long(ok);
delete field_sem;
if (!ok)
	return 0;
return rightjustifynum(nlppp,num,field);
}


/********************************************
* FN:		PERCENTSTR
* CR:		01/09/01 AM.
* SUBJ:	Calculate and format a percentage.
* RET:	str - Right-justified percentage string.
*			  If percent is > 100, no justify.
* FORMS:	percentstr(numerator_num, denominator_num)
********************************************/

_TCHAR *percentstr(
	Nlppp *nlppp,
	long numerator,
	long denominator
	)
{
int fieldsize = 3;
int pct=0;

if (!nlppp)
	return 0;

if (numerator < 0 || denominator < 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[percentstr: Not handling negative nums.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

if (denominator == 0)
	{
	// Divide-by-zero.
	pct = 0;
	}
else
	pct = ((100 * numerator) / denominator);

_TCHAR *buf=0;
if (!(buf = rightjustify(pct,fieldsize)))
	return 0;
if (!*buf)															// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str=0;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

return str;
}


_TCHAR *percentstr(
	Nlppp *nlppp,
	RFASem *num_sem,
	long den
	)
{
if (!num_sem)
	return 0;
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	return 0;
return percentstr(nlppp,num,den);
}


_TCHAR *percentstr(
	Nlppp *nlppp,
	long num,
	RFASem *den_sem
	)
{
if (!den_sem)
	return 0;
bool ok = false;
long den = den_sem->sem_to_long(ok);
delete den_sem;
if (!ok)
	return 0;
return percentstr(nlppp,num,den);
}


_TCHAR *percentstr(
	Nlppp *nlppp,
	RFASem *num_sem,
	RFASem *den_sem
	)
{
if (!num_sem)
	{
	if (den_sem)
		delete den_sem;
	return 0;
	}
if (!den_sem)
	{
	delete num_sem;
	return 0;
	}
bool ok = false;
long num = num_sem->sem_to_long(ok);
delete num_sem;
if (!ok)
	return 0;
ok = false;
long den = den_sem->sem_to_long(ok);
delete den_sem;
if (!ok)
	return 0;
return percentstr(nlppp,num,den);
}


/********************************************
* FN:		HITCONF
* CR:		01/09/01 AM.
* SUBJ:	Confidence function for hits.
* RET:	num - between 0 and 100.
* FORMS:	hitconf(hits_num, total_num, factor_num)
*			factor_num - A fudge factor.  Typical values range from 3 to 13.
********************************************/

long Arun::hitconf(
	Nlppp *nlppp,
	long hits,
	long tot,
	long factor
	)
{
int pct=0;

if (!nlppp)
	return 0;														// FIX.	// 08/11/02 AM.

if (hits < 0 || tot < 0 || factor < 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[hitconf: Not handling negative nums.]") << ends;
	errOut(&gerrStr,false);
	return 0;														// FIX.	// 08/11/02 AM.
	}

pct = webcoConfidence(hits,tot,factor);
return pct;
}


long Arun::hitconf(
	Nlppp *nlppp,
	RFASem *hits_sem,
	long tot,
	long factor
	)
{
if (!hits_sem)
	return 0;
bool ok = false;
long hits = hits_sem->sem_to_long(ok);
delete hits_sem;
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}

long Arun::hitconf(
	Nlppp *nlppp,
	long hits,
	RFASem *tot_sem,
	long factor
	)
{
if (!tot_sem)
	return 0;
bool ok = false;
long tot = tot_sem->sem_to_long(ok);
delete tot_sem;
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}


long Arun::hitconf(
	Nlppp *nlppp,
	long hits,
	long tot,
	RFASem *factor_sem
	)
{
if (!factor_sem)
	return 0;
bool ok = false;
long factor = factor_sem->sem_to_long(ok);
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}


long Arun::hitconf(
	Nlppp *nlppp,
	RFASem *hits_sem,
	RFASem *tot_sem,
	long factor
	)
{
if (!hits_sem)
	{
	if (tot_sem)
		delete tot_sem;
	return 0;
	}
if (!tot_sem)
	{
	delete hits_sem;
	return 0;
	}
bool ok = false;
long hits = hits_sem->sem_to_long(ok);
delete hits_sem;
if (!ok)
	{
	if (tot_sem)
		delete tot_sem;
	return 0;
	}
ok = false;
long tot = tot_sem->sem_to_long(ok);
delete tot_sem;
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}


long Arun::hitconf(
	Nlppp *nlppp,
	long hits,
	RFASem *tot_sem,
	RFASem *factor_sem
	)
{
if (!tot_sem)
	{
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
if (!factor_sem)
	{
	delete tot_sem;
	return 0;
	}
bool ok = false;
long tot = tot_sem->sem_to_long(ok);
delete tot_sem;
if (!ok)
	{
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
ok = false;
long factor = factor_sem->sem_to_long(ok);
delete factor_sem;
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}


long Arun::hitconf(
	Nlppp *nlppp,
	RFASem *hits_sem,
	long tot,
	RFASem *factor_sem
	)
{
if (!hits_sem)
	{
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
if (!factor_sem)
	{
	delete hits_sem;
	return 0;
	}
bool ok = false;
long hits = hits_sem->sem_to_long(ok);
delete hits_sem;
if (!ok)
	{
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
ok = false;
long factor = factor_sem->sem_to_long(ok);
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}


long Arun::hitconf(
	Nlppp *nlppp,
	RFASem *hits_sem,
	RFASem *tot_sem,
	RFASem *factor_sem
	)
{
if (!hits_sem)
	{
	if (tot_sem)
		delete tot_sem;
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
if (!tot_sem)
	{
	delete hits_sem;
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
if (!factor_sem)
	{
	delete hits_sem;
	delete tot_sem;
	return 0;
	}
bool ok = false;
long hits = hits_sem->sem_to_long(ok);
delete hits_sem;
if (!ok)
	{
	if (tot_sem)
		delete tot_sem;
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
ok = false;
long tot = tot_sem->sem_to_long(ok);
delete tot_sem;
if (!ok)
	{
	if (factor_sem)
		delete factor_sem;
	return 0;
	}
ok = false;
long factor = factor_sem->sem_to_long(ok);
delete factor_sem;
if (!ok)
	return 0;
return hitconf(nlppp,hits,tot,factor);
}


/********************************************
* FN:		EXITPASS
* CR:		02/06/01 AM.
* SUBJ:	Force immediate exit of current pass.
* RET:	True if exiting pass, else false.
* FORMS:	exitpass()
********************************************/

bool Arun::exitpass(
	Nlppp *nlppp
	)
{
if (!nlppp)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[exitpass: Error.]") << ends;
	errOut(&gerrStr,false);
	return true;
	}

// Removed check for region here.							// FIX.	// 04/29/01 AM.

if (nlppp)
	{
	nlppp->succeed_ = true;			// Done with actions.
	nlppp->exitpass_ = true;		// Done with pass.
	}

// Not allowing "exceptions" to be inside a user function.		// 08/18/08 AM.
#ifdef _TOO_STRICT_
Parse *parse = nlppp->getParse();										// 08/18/08 AM.
if (nlppp->insidefn_)														// 08/18/08 AM.
	{
	_stprintf(Errbuf,_T("[exitpass: Ignoring inside a function.]"));	// 08/18/08 AM.
	return parse->errOut(true);										// 08/18/08 AM.
	}
#endif

return true;
}


/********************************************
* FN:		TODAY
* CR:		02/08/01 AM.
* SUBJ:	Current date and time as a string.
* RET:	str - today string.
* FORMS:	today()
********************************************/

_TCHAR *Arun::today(
	Nlppp *nlppp
	)
{
if (!nlppp)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[today: Error.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

// Intern the today string.
_TCHAR *buf = ::today();
if (!buf || !*buf)
	return 0;
_TCHAR *str=0;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
return str;
}


/********************************************
* FN:		FNPNCOPYVARS
* CR:		05/16/01 AM.
* SUBJ:	Copy variables and values to suggested node.
* RET:	True if ok, else false.
* FORMS:	pncopyvars(pnode)
*			pncopyvars(num)
*			pncopyvars(0) or pncopyvars() -- last elt's node.
********************************************/

bool Arun::pncopyvars(
	Nlppp *nlppp
	)
{
return pncopyvars(nlppp,(long)0);
}

// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	long num1
	)
{
if (!nlppp || num1 < 0)
	return false;

if (!nlppp->coll_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[pncopyvars: Warning. No collected nodes.]") << ends;
	errOut(&gerrStr,false);
	return false;
	}

if (!num1)	// 0 means last element of rule.
	num1 = nlppp->len_;

COLL *coll = &(nlppp->coll_[num1]);

Node<Pn> *node1, *node2;

node1 = (Node<Pn> *) coll->start;	// Try to get start of range.
node2 = (Node<Pn> *) coll->end;	// Try to get end of range.

if (!node1 || !node2)
	return false;
if (node1 != node2)
	return false;
Node<Pn> *node = node1;
return pncopyvars(nlppp,node);
}

// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	RFASem *sem1
	)
{
if (!sem1)
	return false;
if (!nlppp)
	{
	delete sem1;
	return false;
	}

Node<Pn> *node = 0;
if (sem1->getType() == RSNODE)
	{
	node = sem1->sem_to_node();
	delete sem1;
	return pncopyvars(nlppp, node);
	}

bool ok = false;
long num1 = sem1->sem_to_long(ok);
delete sem1;
if (!ok)
	return false;
return pncopyvars(nlppp,num1);
}


// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	NODE *node
	)
{
if (!nlppp || !node)
	return false;

Pn *pn = ((Node<Pn>*)node)->getData();
Dlist<Ipair> *fmlist = pn->getDsem();
Dlist<Ipair> *tolist = nlppp->getDsem();
Var::copy_vars(fmlist,tolist);
// Restore var list in parser state.
nlppp->setDsem(tolist);														// 08/04/02 AM.
return true;
}

// TWO-ARG VARIANTS.

// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	NODE *node1,
	NODE *node2
	)
{
if (!nlppp || !node1 || !node2)
	return false;

Pn *pn1 = ((Node<Pn>*)node1)->getData();
Pn *pn2 = ((Node<Pn>*)node2)->getData();
Dlist<Ipair> *fmlist = pn1->getDsem();
Dlist<Ipair> *tolist = pn2->getDsem();
Var::copy_vars(fmlist,tolist);
pn2->setDsem(tolist);			// Restore to destination.			// 08/04/02 AM.
return true;
}

// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	RFASem *sem1,
	NODE *node2
	)
{
if (!sem1)
	return false;
if (!node2)
	{
	delete sem1;
	return false;
	}

Node<Pn> *node1 = sem1->sem_to_node();
delete sem1;
return pncopyvars(nlppp,node1,node2);
}

// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	NODE *node1,
	RFASem *sem2
	)
{
if (!sem2)
	return false;
if (!node1)
	{
	delete sem2;
	return false;
	}

Node<Pn> *node2 = sem2->sem_to_node();
delete sem2;
return pncopyvars(nlppp,node1,node2);
}

// VARIANT.
bool Arun::pncopyvars(
	Nlppp *nlppp,
	RFASem *sem1,
	RFASem *sem2
	)
{
if (!sem1 || !sem2)
	{
	if (sem1)
		delete sem1;
	if (sem2)
		delete sem2;
	return false;
	}

Node<Pn> *node1 = sem1->sem_to_node();
delete sem1;
if (!node1)
	{
	delete sem2;
	return false;
	}

Node<Pn> *node2 = sem2->sem_to_node();
delete sem2;
return pncopyvars(nlppp,node1,node2);
}


/********************************************
* FN:		STRESCAPE
* CR:	   07/24/01 DD.
* SUBJ: Places an escape character in front of the specified
*       characters.
* RET:  True if ok, else false.
*		       UP - returns the substituted version of the string
* FORMS:	strescape(str, charsToEscapeStr, escapeChar)
* NOTE:
********************************************/

_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *charsToEscapeStr,
	_TCHAR *escapeChar
	)
{
if (!nlppp)
	return 0;
if (!charsToEscapeStr || !*charsToEscapeStr)
	return 0;
if (!escapeChar || !*escapeChar)
	return 0;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strescape: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR buf[100000];
str_escape(name1, charsToEscapeStr, escapeChar[0], buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);

return str;
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *charsToEscape,
	_TCHAR *escapeChar
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *charsToEscape_sem,
	_TCHAR *escapeChar
	)
{
if (!charsToEscape_sem)
	return 0;
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	_TCHAR *name,
	_TCHAR *charsToEscape,
	RFASem *escapeChar_sem
	)
{
if (!escapeChar_sem)
	return 0;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *charsToEscape_sem,
	_TCHAR *escapeChar
	)
{
if (!name_sem)
	{
	if (charsToEscape_sem)
		delete charsToEscape_sem;
	return 0;
	}
if (!charsToEscape_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *charsToEscape,
	RFASem *escapeChar_sem
	)
{
if (!name_sem)
	{
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!escapeChar_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *charsToEscape_sem,
	RFASem *escapeChar_sem
	)
{
if (!charsToEscape_sem)
	{
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!escapeChar_sem)
	{
	delete charsToEscape_sem;
	return 0;
	}
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *charsToEscape_sem,
	RFASem *escapeChar_sem
	)
{
if (!name_sem)
	{
	if (charsToEscape_sem)
		delete charsToEscape_sem;
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!charsToEscape_sem)
	{
	delete name_sem;
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!escapeChar_sem)
	{
	delete name_sem;
	delete charsToEscape_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strescape(nlppp,name,charsToEscape,escapeChar);
}


/********************************************
* FN:		STRUNESCAPE
* CR:	   07/24/01 DD.
* SUBJ: Places an escape character in front of the specified
*       characters.
* RET:  True if ok, else false.
*		       UP - returns the substituted version of the string
* FORMS:	strunescape(str, charsToEscapeStr, escapeChar)
* NOTE:
********************************************/

_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *charsToEscapeStr,
	_TCHAR *escapeChar
	)
{
if (!nlppp)
	return 0;
if (!charsToEscapeStr || !*charsToEscapeStr)
	return 0;
if (!escapeChar || !*escapeChar)
	return 0;

if (!name1 || !*name1)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[strunescape: Warning. Given no str.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

_TCHAR buf[100000];
str_unescape(name1, charsToEscapeStr, escapeChar[0], buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return 0;														// FIX.	// 02/09/01 AM.

_TCHAR *str;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);

return str;
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *charsToEscape,
	_TCHAR *escapeChar
	)
{
if (!name_sem)
	return 0;
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *charsToEscape_sem,
	_TCHAR *escapeChar
	)
{
if (!charsToEscape_sem)
	return 0;
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	_TCHAR *name,
	_TCHAR *charsToEscape,
	RFASem *escapeChar_sem
	)
{
if (!escapeChar_sem)
	return 0;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *charsToEscape_sem,
	_TCHAR *escapeChar
	)
{
if (!name_sem)
	{
	if (charsToEscape_sem)
		delete charsToEscape_sem;
	return 0;
	}
if (!charsToEscape_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	_TCHAR *charsToEscape,
	RFASem *escapeChar_sem
	)
{
if (!name_sem)
	{
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!escapeChar_sem)
	{
	delete name_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	_TCHAR *name,
	RFASem *charsToEscape_sem,
	RFASem *escapeChar_sem
	)
{
if (!charsToEscape_sem)
	{
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!escapeChar_sem)
	{
	delete charsToEscape_sem;
	return 0;
	}
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}


_TCHAR *Arun::strunescape(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *charsToEscape_sem,
	RFASem *escapeChar_sem
	)
{
if (!name_sem)
	{
	if (charsToEscape_sem)
		delete charsToEscape_sem;
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!charsToEscape_sem)
	{
	delete name_sem;
	if (escapeChar_sem)
		delete escapeChar_sem;
	return 0;
	}
if (!escapeChar_sem)
	{
	delete name_sem;
	delete charsToEscape_sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
_TCHAR *charsToEscape = charsToEscape_sem->sem_to_str();
delete charsToEscape_sem;
_TCHAR *escapeChar = escapeChar_sem->sem_to_str();
delete escapeChar_sem;
return strunescape(nlppp,name,charsToEscape,escapeChar);
}



/********************************************
* FN:		KBDUMPTREE
* CR:		08/06/01 AM.
* SUBJ:	Modular dump of kb subtree.
* RET:	True if ok, else false.
* FORMS:	kbdumptree(conc, namestr)
********************************************/

RFASem *Arun::kbdumptree(
	Nlppp *nlppp,
	RFASem *sem,
	_TCHAR *name
	)
{
if (!name || !*name || !nlppp)
	{
	if (sem)
		delete sem;
	return 0;
	}

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem->getType() != RS_KBCONCEPT)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[kbdumptree: Bad semantic arg.]") << ends;
	errOut(&gerrStr,false);
	delete sem;
	return 0;
	}
CONCEPT *conc = sem->getKBconcept();

bool ok = cg->dumpTree(conc, name);

delete sem;
return new RFASem(ok ? (long)1 : (long)0);
}

RFASem *Arun::kbdumptree(
	Nlppp *nlppp,
	RFASem *sem,
	RFASem *name_sem
	)
{
if (!name_sem)
	{
	if (sem)
		delete sem;
	return 0;
	}
_TCHAR *name = name_sem->sem_to_str();
delete name_sem;
return kbdumptree(nlppp,sem,name);
}



/********************************************
* FN:		FLT
* CR:		08/23/01 AM.
* SUBJ:	Conversions to float.
* RET:	True if ok, else false.
* FORMS:	flt(int)
*			flt(str)
*			flt(float)  -- no op.
********************************************/

float Arun::flt(
	Nlppp *nlppp,
	long num
	)
{
return (float)num;
}

float Arun::flt(
	Nlppp *nlppp,
	float fnum
	)
{
return fnum;	// no op.
}

float Arun::flt(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
if (!str || !*str)
	return (float)0.0;
float fnum = 0.0;
if (!s_to_f(str, /*UP*/ &fnum))
	return (float)0.0;
return fnum;
}

float Arun::flt(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem)
	return (float)0.0;
switch (sem->getType())
	{
	case RSLONG:
		{
		float fnum = (float) sem->getLong();
		delete sem;
		return fnum;
		}
	case RSFLOAT:
		{
		float fnum = sem->getFloat();
		delete sem;
		return fnum;
		}
	case RSSTR:
	case RSNUM:
	case RSNAME:
		{
		_TCHAR *str = sem->getName();
		delete sem;
		float fnum = 0.0;
		if (!str || !*str)
			return (float)0.0;
		if (!s_to_f(str, /*UP*/ &fnum))
			return (float)0.0;
		return fnum;
		}
	default:
		delete sem;
		return (float)0.0;
	}
return (float)0.0;
}

/********************************************
* FN:		SEM
* CR:		03/07/02 AM.
* SUBJ:	Conversions to sem object.
* RET:	Sem object.
* FORMS:	sem(long)
*			sem(str)
*			sem(float)
*			sem(sem) -- noop.
* NOTE:	Used to enable dynamic return of any type from a user-defined
*			NLP++ function.
********************************************/

RFASem *Arun::sem(
	RFASem *sem
	)
{
return sem;
}

RFASem *Arun::sem(
	long num
	)
{
return new RFASem(num);
}

RFASem *Arun::sem(
	float fnum
	)
{
return new RFASem(fnum);
}

RFASem *Arun::sem(
	_TCHAR *str
	)
{
return new RFASem(str, RSSTR);
}

/********************************************
* FN:		STRISUPPER
* CR:		11/20/01 AM.
* SUBJ:	Check string's first letter.
* RET:	True if uppercase 1st letter.
* FORMS:	strisupper(str)
********************************************/

bool Arun::strisupper(Nlppp *nlppp, _TCHAR *str)
{
if (str && *str && is_upper(*str))										// 12/16/01 AM.
	return true;
return false;
}

bool Arun::strisupper(Nlppp *nlppp, RFASem *sem)
{
if (!sem)
	return false;

_TCHAR *str=0;
switch (sem->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		str = sem->getName();
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[strisupper: Bad semantic arg.]") << ends;
		errOut(&gerrStr,false);
		str = 0;
		}
		break;
	}
delete sem;
if (str && *str && is_upper(*str))										// 12/16/01 AM.
	return true;
return false;
}

bool Arun::strisupper(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}

/********************************************
* FN:		STRISLOWER
* CR:		11/20/01 AM.
* SUBJ:	Check string's first letter.
* RET:	True if lowercase 1st letter.
* FORMS:	strislower(str)
********************************************/

bool Arun::strislower(Nlppp *nlppp, _TCHAR *str)
{
if (str && *str && is_lower(*str))										// 12/16/01 AM.
	return true;
return false;
}

bool Arun::strislower(Nlppp *nlppp, RFASem *sem)
{
if (!sem)
	return false;

_TCHAR *str=0;
switch (sem->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		str = sem->getName();
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[strislower: Bad semantic arg.]") << ends;
		errOut(&gerrStr,false);
		str = 0;
		}
		break;
	}
delete sem;
if (str && *str && is_lower(*str))										// 12/16/01 AM.
	return true;
return false;
}

bool Arun::strislower(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}

/********************************************
* FN:	   SUFFIX
* CR:	   11/23/01 AM.
* SUBJ:	See if reasonable suffix of given word.
* RET:	True if good suffix.
* FORMS:	suffix(str, suffix_str)
* NOTE:	Using words array to study word.  Morphological grounds only.
********************************************/

bool Arun::suffix(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *suff
	)
{
if (!name1 || !*name1 || !suff || !*suff)
	{
	_stprintf(Errbuf,_T("[suffix: Warning: Given no str or suffix.]"));
	return errOut(false); // UNFIXED
	}

return strsuffix(name1,suff);
}


// VARIANT.
bool Arun::suffix(Nlppp *nlppp, _TCHAR *name1, RFASem *sem)
{
if (!sem)
	return false;
if (!name1 || !*name1)
	{
	delete sem;
	_stprintf(Errbuf,_T("[suffix: Warning: Given no str.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *str=0;
switch (sem->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		str = sem->getName();
		break;
	default:
		{
		delete sem;
		_t_strstream gerrStr;
		gerrStr << _T("[suffix: Bad semantic arg.]") << ends;
		return errOut(&gerrStr,false);
		}
		break;
	}
delete sem;
return strsuffix(name1,str);
}

// VARIANT.
bool Arun::suffix(Nlppp *nlppp, RFASem *sem1, _TCHAR *suff)
{
if (!sem1)
	return false;
if (!suff || !*suff)
	{
	delete sem1;
	_stprintf(Errbuf,_T("[suffix: Warning: Given no suffix.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *str=0;
switch (sem1->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		str = sem1->getName();
		break;
	default:
		{
		delete sem1;
		_t_strstream gerrStr;
		gerrStr << _T("[suffix: Bad semantic arg.]") << ends;
		return errOut(&gerrStr,false);
		}
		break;
	}
delete sem1;
return strsuffix(str,suff);
}


// VARIANT.
bool Arun::suffix(Nlppp *nlppp, RFASem *sem1, RFASem *sem2)
{
if (!sem1 && !sem2)
	return false;
if (!sem1)
	{
	delete sem2;
	return false;
	}
if (!sem2)
	{
	delete sem1;
	return false;
	}

_TCHAR *name1=0;
switch (sem1->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		name1 = sem1->getName();
		break;
	default:
		{
		delete sem1;
		delete sem2;
		_t_strstream gerrStr;
		gerrStr << _T("[suffix: Bad semantic arg.]") << ends;
		return errOut(&gerrStr,false);
		}
		break;
	}
delete sem1;
_TCHAR *suff=0;
switch (sem2->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		suff = sem2->getName();
		break;
	default:
		{
		delete sem2;
		_t_strstream gerrStr;
		gerrStr << _T("[suffix: Bad semantic arg.]") << ends;
		return errOut(&gerrStr,false);
		}
		break;
	}
delete sem2;
return strsuffix(name1,suff);
}

/********************************************
* FN:		FNSTART
* CR:		03/11/02 AM.
* SUBJ:	Special start for user-defined functions.
* RET:	loc = Local vars from the calling context.
* NOTE:	To "push" the current stack frame.
********************************************/

void *Arun::fnstart(
	Nlppp *nlppp,
	void *&savelocals,	// 01/08/07 AM.
	void *&savelocalstrs	// 01/08/07 AM.
	)
{
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
savelocalstrs = (void *) nlppp->localstrs_;					// 01/08/07 AM.
savelocals = (void *) nlppp->locals_;			// MOD				// 01/08/07 AM.
nlppp->locals_ = 0;	// Start current frame with no local vars.
nlppp->localstrs_ = 0;	// Start with no local strings.			// 01/08/07 AM.
//return savelocals;	// 01/08/07 AM.
return 0;	// 01/08/07 AM.
}


/********************************************
* FN:		RET
* CR:		03/11/02 AM.
* SUBJ:	Special return for user-defined functions.
* RET:	Sem object.
* FORMS:	ret(nlppp,loc,locstrs,long)
*			ret(nlppp,loc,locstrs,str)
*			ret(nlppp,loc,locstrs,float)
*			ret(nlppp,loc,locstrs,sem)
* NOTE:	Used to enable dynamic return of any type from a user-defined
*			NLP++ function.
********************************************/

RFASem *Arun::ret(
	Nlppp *nlppp,
	void *loc,	// List of L() local vars from calling frame.
	void *locstrs,
	RFASem *sem
	)
{
if (nlppp->locals_)
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);
nlppp->locals_ = (Dlist<Ipair> *) loc;		// "Pop" the stack.
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.				// 01/08/07 AM.
nlppp->freeLocals((Slist<_TCHAR> *)locstrs);					// 01/08/07 AM.
return sem;
}

RFASem *Arun::ret(
	Nlppp *nlppp,
	void *loc,	// List of L() local vars from calling frame.
	void *locstrs,
	long num
	)
{
if (nlppp->locals_)
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);
nlppp->locals_ = (Dlist<Ipair> *) loc;		// "Pop" the stack.
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.				// 01/08/07 AM.
nlppp->freeLocals((Slist<_TCHAR> *)locstrs);					// 01/08/07 AM.
return new RFASem(num);
}

RFASem *Arun::ret(
	Nlppp *nlppp,
	void *loc,	// List of L() local vars from calling frame.
	void *locstrs,
	float fnum
	)
{
if (nlppp->locals_)
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);
nlppp->locals_ = (Dlist<Ipair> *) loc;		// "Pop" the stack.
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
nlppp->freeLocals((Slist<_TCHAR> *)locstrs);					// 01/08/07 AM.
return new RFASem(fnum);
}

RFASem *Arun::ret(
	Nlppp *nlppp,
	void *loc,	// List of L() local vars from calling frame.
	void *locstrs,
	_TCHAR *str
	)
{
if (nlppp->locals_)
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);
nlppp->locals_ = (Dlist<Ipair> *) loc;		// "Pop" the stack.
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
nlppp->freeLocals((Slist<_TCHAR> *)locstrs);					// 01/08/07 AM.
return new RFASem(str, RSSTR);
}


/********************************************
* FN:		EXCEPTH
* CR:		03/09/14 AM.
* SUBJ:	Cleanup when catching an exception.
* RET:	0
* FORMS:	excepth(nlppp,loc,locstrs,long)
* NOTE:	Don't create a sem, as ret() does.
********************************************/
long Arun::excepth(
	Nlppp *nlppp,
	void *loc,	// List of L() local vars from calling frame.
	void *locstrs,
	int e	// THE EXCEPTION NUMBER.
	)
{
long ruleline = 0;
long passnum = 0;
// For debugging.	// 03/10/14 AM.
#ifdef _DEBUG_STUFFNJUNK_
if (nlppp)
	{
	ruleline = (nlppp->sugg_ ? nlppp->sugg_->ruleline : 0);
	passnum  = (nlppp->parse_ ? nlppp->parse_->currpass_ : 0);
	}

_t_strstream gerrStr;
gerrStr << _T("[") << passnum
	<< _T(",") << ruleline << _T("] EXCEPTION: ")
	<< e << ends;
errOut(&gerrStr,false);
#endif

if (nlppp->locals_)
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);
nlppp->locals_ = (Dlist<Ipair> *) loc;		// "Pop" the stack.
// LOCAL STRING, TEMPORARY STRING MANAGEMENT.
nlppp->freeLocals((Slist<_TCHAR> *)locstrs);
return 0;
}


/********************************************
* FN:		COUTRESET
* CR:		05/12/02 AM.
* SUBJ:	Rebind COUT to the specified filename.
* RET:	ostream - new output-stream value.
* FORMS:	coutreset(filename) - cout goes to specified file.
*			coutreset() - [unimplemented] could reset to user-supplied stream.
* NOTE:	Could distinguish between user-supplied stream, standard out, and
*			redirect to file.
* OPT:	Would be more efficient to return ostream.
********************************************/

RFASem *Arun::coutreset(
	Nlppp *nlppp,
	_TCHAR *fname		// Filename to rebind to.
	)
{
RFASem *sem = 0;

if (!fname || !*fname)
	return 0;

// Check for good filename.
// Set up output stream.
sem = Arun::openfile(nlppp,fname, _T("app"));
if (!sem)
	{
	_stprintf(Errbuf,_T("[coutreset: Couldn't set up file=%s.]"), fname);
	errOut(true); // UNFIXED
	return 0;
	}

// REBIND COUT.
// .NET FIX.  NOT GOOD TO MANGLE THE REAL COUT.	// 06/11/03 AM.
if (sem->getOstream())
//	cout = *(sem->getOstream());
	nlppp->parse_->setCout(sem->getOstream());	// 06/11/03 AM.

//delete sem;
//return &cout;
return sem;
}

// VARIANT.
RFASem *Arun::coutreset(
	Nlppp *nlppp,
	RFASem *fsem
	)
{
if (!fsem)
	return 0;
_TCHAR *fname = 0;
switch (fsem->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		fname = fsem->getName();
		break;
	default:
		{
		_t_strstream gerrStr;
		gerrStr << _T("[coutreset: Bad semantic type.]") << ends;
		errOut(&gerrStr,false);
		delete fsem;
		return 0;
		}
	}
return coutreset(nlppp,fname);
}


/********************************************
* FN:		CBUF
* CR:		05/12/02 AM.
* SUBJ:	Return the CBUF output stream.
* RET:	sem ostrstream - output-stream value.
* FORMS:	cbuf() - Output buffer stream.
* NOTE:	By calling nlp->analyze() with an output buffer, analyzer can
*			output to cbuf().
* OPT:	Would be more efficient to return ostream or ostrstream.
********************************************/

RFASem *Arun::cbuf(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return 0;
if (!nlppp->parse_)
	return 0;
return new RFASem(nlppp->parse_->getCbuf());
}


/********************************************
* FN:		PNVARNAMES
* CR:		05/13/02 AM.
* SUBJ:	Get PNODE's variable names.
* RET:	True if ok, else false.
* FORMS:	pnvarnames(pnode)
********************************************/

RFASem *Arun::pnvarnames(
	Nlppp *nlppp,
	NODE *pnode
	)
{
RFASem *sem = 0;

if (!nlppp || !pnode)
	return 0;

Node<Pn> *node = (Node<Pn> *)pnode;
Pn *pn = node->getData();

Dlist<Iarg> *vars = Ivar::nodeVarnames(pn);
if (vars)
   sem = new RFASem(vars);
return sem;
}


RFASem *Arun::pnvarnames(
	Nlppp *nlppp,
	RFASem *node_sem
	)
{
if (!node_sem)
	return 0;
NODE *pnode = node_sem->sem_to_node();
delete node_sem;
return pnvarnames(nlppp,pnode);
}

#ifdef _ODBC

/********************************************
* FN:		DBOPEN
* CR:		06/15/02 AM.
* SUBJ:	Open ODBC database connection.
* RET:	True if ok, else false.
*			UP success - "boolean" true if success, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			Assume only ONE OPEN DB CONNECTION PER PARSE, FOR NOW.
* FORMS:	dbopen(dbname_str, username_str, password_str)
* EX:		dbopen("myodbc3-test", "root", 0);
********************************************/

bool Arun::dbopen(
	Nlppp *nlppp,
	_TCHAR *dbname,
	_TCHAR *usrname,
	_TCHAR *password
	)
{

if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;

if (!dbname || !*dbname)
	{
	_stprintf(Errbuf,_T("[dbopen: Warning. Given no dbname.]"));
	return errOut(false); // UNFIXED
	}


SQLHENV    henv = 0;
SQLHDBC    hdbc = 0;
SQLRETURN  retcode = SQL_ERROR;

//Allocate environment handle.
retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	return false;

parse->setHenv(henv);

// Set the ODBC version environment attribute to version 3.
retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,
											(SQLPOINTER)SQL_OV_ODBC3, 0);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	parse->dbclose();
	return false;
	}

// Allocate connection handle.
retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	parse->dbclose();
	return false;
	}

parse->setHdbc(hdbc);

// Connect to data source myodbc3.
// retcode = SQLConnect(hdbc, (SQLCHAR*) "myodbc3", SQL_NTS,
retcode = SQLConnect(hdbc, (SQLTCHAR*) dbname, SQL_NTS,
							(SQLTCHAR*) usrname, SQL_NTS,
							(SQLTCHAR*) password, SQL_NTS);

if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	parse->dbclose();
	return false;
	}

// Set auto commit to ON.
// retcode = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTO_COMMIT,
//										SQL_AUTOCOMMIT_ON,0);
retcode = SQLSetConnectAttr(hdbc, SQL_AUTOCOMMIT,
							(void *)SQL_AUTOCOMMIT_ON,0);
// printf("\n autocommit returned :%d", retcode);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	parse->dbclose();
	return false;
	}

return true;
}

// VARIANTS
bool Arun::dbopen(
	Nlppp *nlppp,
	RFASem *dbsem,
	_TCHAR *usrname,
	_TCHAR *password
	)
{
if (!dbsem)
	return false;
_TCHAR *dbname = dbsem->sem_to_str();
delete dbsem;
return dbopen(nlppp,dbname,usrname,password);
}


bool Arun::dbopen(
	Nlppp *nlppp,
	_TCHAR *dbname,
	RFASem *usrsem,
	_TCHAR *password
	)
{
_TCHAR *usrname = 0;
if (usrsem)
	{
	usrname = usrsem->sem_to_str();
	delete usrsem;
	}
return dbopen(nlppp,dbname,usrname,password);
}


bool Arun::dbopen(
	Nlppp *nlppp,
	_TCHAR *dbname,
	_TCHAR *usrname,
	RFASem *passsem
	)
{
_TCHAR *password = 0;
if (passsem)
	{
	password = passsem->sem_to_str();
	delete passsem;
	}
return dbopen(nlppp,dbname,usrname,password);
}


bool Arun::dbopen(
	Nlppp *nlppp,
	RFASem *dbsem,
	RFASem *usrsem,
	_TCHAR *password
	)
{
if (!dbsem)
	{
	if (usrsem)
		delete usrsem;
	return false;
	}
_TCHAR *dbname = dbsem->sem_to_str();
delete dbsem;
_TCHAR *usrname = 0;
if (usrsem)
	{
	usrname = usrsem->sem_to_str();
	delete usrsem;
	}
return dbopen(nlppp,dbname,usrname,password);
}

bool Arun::dbopen(
	Nlppp *nlppp,
	RFASem *dbsem,
	_TCHAR *usrname,
	RFASem *passsem
	)
{
if (!dbsem)
	{
	if (passsem)
		delete passsem;
	return false;
	}
_TCHAR *dbname = dbsem->sem_to_str();
delete dbsem;
_TCHAR *password = 0;
if (passsem)
	{
	password = passsem->sem_to_str();
	delete passsem;
	}
return dbopen(nlppp,dbname,usrname,password);
}


bool Arun::dbopen(
	Nlppp *nlppp,
	_TCHAR *dbname,
	RFASem *usrsem,
	RFASem *passsem
	)
{
_TCHAR *usrname = 0;
if (usrsem)
	{
	usrname = usrsem->sem_to_str();
	delete usrsem;
	}
_TCHAR *password = 0;
if (passsem)
	{
	password = passsem->sem_to_str();
	delete passsem;
	}
return dbopen(nlppp,dbname,usrname,password);
}


bool Arun::dbopen(
	Nlppp *nlppp,
	RFASem *dbsem,
	RFASem *usrsem,
	RFASem *passsem
	)
{
if (!dbsem)
	{
	if (usrsem)
		delete usrsem;
	if (passsem)
		delete passsem;
	return false;
	}
_TCHAR *dbname = dbsem->sem_to_str();
delete dbsem;
_TCHAR *usrname = 0;
if (usrsem)
	{
	usrname = usrsem->sem_to_str();
	delete usrsem;
	}
_TCHAR *password = 0;
if (passsem)
	{
	password = passsem->sem_to_str();
	delete passsem;
	}
return dbopen(nlppp,dbname,usrname,password);
}

/********************************************
* FN:		DBCLOSE
* CR:		06/15/02 AM.
* SUBJ:	Close ODBC database connection.
* RET:	True if ok, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			PLAYING AROUND HERE, for the moment.
*			Assume only ONE OPEN DB CONNECTION PER PARSE, FOR NOW.
* FORMS:	dbclose()
********************************************/

bool Arun::dbclose(
	Nlppp *nlppp
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;

parse->dbclose();
return true;
}


/********************************************
* FN:		DBEXEC
* CR:		06/15/02 AM.
* SUBJ:	Execute ODBC statement.
* RET:	True if ok, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			Assume only ONE OPEN DB CONNECTION PER PARSE, FOR NOW.
* OPT:	May want to store statement handle for reuse.
* FORMS:	dbexec(str)
********************************************/

bool Arun::dbexec(
	Nlppp *nlppp,
	_TCHAR *stmt
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;


SQLTCHAR *statement = (_TUCHAR *)stmt;
if (!statement || !*statement)
	{
	_stprintf(Errbuf,_T("[dbexec: Warning. Given no statement.]"));
	return errOut(false); // UNFIXED
	}


SQLHDBC    hdbc = parse->getHdbc();
SQLHSTMT   hstmt;
SQLRETURN  retcode;
// Allocate statement handle.
retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	_stprintf(Errbuf,_T("[dbexec: Couldn't allocate handle for statement.]"));
	return errOut(false); // UNFIXED
	}

// Execute the statement.
SQLExecDirect (hstmt, statement, SQL_NTS);


// Free statement handle.
SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

return true;
}

// VARIANT.
bool Arun::dbexec(
	Nlppp *nlppp,
	RFASem *stmtsem
	)
{
if (!stmtsem)
	return true;
_TCHAR *stmt = stmtsem->sem_to_str();
delete stmtsem;
return dbexec(nlppp,stmt);
}

/********************************************
* FN:		DBALLOCSTMT
* CR:		06/15/02 AM.
* SUBJ:	Allocate db statement handle.
* RET:	True if ok, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			Can implement NLP++ data types for hstmt, hdbc, henv...
* ASS:	Assume one statement handle open at a time, for now.
* FORMS:	dballocstmt()
********************************************/

bool Arun::dballocstmt(
	Nlppp *nlppp
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;

// ALLOCATE STMT HANDLE. (Todo: make it an NLP++ data type.)
if (!parse->allocDbstmt())
	return false;
return true;
}


/********************************************
* FN:		DBEXECSTMT
* CR:		06/15/02 AM.
* SUBJ:	Execute using the currently active statement.
* RET:	True if ok, else false.
* NOTE:	Uses db stmt handle saved in parse object.
* FORMS:	dbexecstmt(str)
********************************************/

bool Arun::dbexecstmt(
	Nlppp *nlppp,
	_TCHAR *stmt
	)
{
if (!nlppp)
	return false;
Parse *parse = nlppp->getParse();
if (!parse)
	return false;

SQLTCHAR *statement = (_TUCHAR *)stmt;
if (!statement || !*statement)
	{
	_stprintf(Errbuf,_T("[dbexecstmt: Warning. Given no statement.]"));
	return errOut(true); // UNFIXED
	}

SQLHSTMT   hstmt = parse->getHstmt();
if (!hstmt)
	return false;

// Execute the statement.
SQLExecDirect (hstmt, statement, SQL_NTS);

return true;
}


// VARIANT.
bool Arun::dbexecstmt(
	Nlppp *nlppp,
	RFASem *stmtsem
	)
{
if (!stmtsem)
	return true;
_TCHAR *stmt = stmtsem->sem_to_str();
delete stmtsem;
return dbexecstmt(nlppp,stmt);
}

/********************************************
* FN:		DBFREESTMT
* CR:		06/15/02 AM.
* SUBJ:	Free ODBC statement.
* RET:	True if ok, else false.
* NOTE:	Free the db stmt handle in the parse object.
* FORMS:	dbfreestmt()
********************************************/

bool Arun::dbfreestmt(
	Nlppp *nlppp
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;

SQLHSTMT   hstmt = parse->getHstmt();
if (!hstmt)
	return false;

// Free statement handle.
SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

parse->setHstmt(0);	// CLEAR OUT statement handle.

return true;
}


/********************************************
* FN:		DBFETCH
* CR:		06/15/02 AM.
* SUBJ:	Fetch result using the currently active statement.
* RET:	True if ok, else false.
* NOTE:	Uses db stmt handle saved in parse object.
* FORMS:	dbfetch()
********************************************/

bool Arun::dbfetch(
	Nlppp *nlppp
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;

SQLHSTMT   hstmt = parse->getHstmt();
if (!hstmt)
	return false;

// Execute the statement.
SQLRETURN retcode = SQLFetch (hstmt);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	// When there's no more result to fetch, this return is OK.	// 05/29/02 DD.
//	sprintf_s(Errbuf,"[dbfetch: Error. Couldn't fetch result.]");	// 05/29/02 DD.
//	return errOut(true);														// 05/29/02 DD.
	return false;
	}

return true;
}

#endif // _ODBC

/********************************************
* FN:		EXITTOPOPUP
* CR:		06/15/02 AM.
* SUBJ:	Exit current pass, preparing for GUI popup.
* RET:	True if ok, else false.
* NOTE:
* FORMS:	exittopopup(message_str, type_str)
*				The meaning of these arguments depends on the GUI implementation
*				of the popup window.
********************************************/

bool Arun::exittopopup(
	Nlppp *nlppp,
	_TCHAR *msg_str,
	_TCHAR *typ_str
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->getParse();
if (!parse)
  return false;
Ana *ana = parse->getAna();
NLP *nlp = ana->getNLP();

// These will signal popup time to the GUI.
nlp->setPopupmsg(msg_str);
nlp->setPopuptyp(typ_str);

return true;
}


// VARIANT.
bool Arun::exittopopup(
	Nlppp *nlppp,
	RFASem *msgsem,
	_TCHAR *typ_str
	)
{
if (!msgsem)
	return false;
_TCHAR *msg_str = msgsem->sem_to_str();
delete msgsem;
return exittopopup(nlppp,msg_str,typ_str);
}

// VARIANT.
bool Arun::exittopopup(
	Nlppp *nlppp,
	_TCHAR *msg_str,
	RFASem *typsem
	)
{
if (!typsem)
	return false;
_TCHAR *typ_str = typsem->sem_to_str();
delete typsem;
return exittopopup(nlppp,msg_str,typ_str);
}

// VARIANT.
bool Arun::exittopopup(
	Nlppp *nlppp,
	RFASem *msgsem,
	RFASem *typsem
	)
{
if (!msgsem)
	{
	if (typsem)
		delete typsem;
	return false;
	}
_TCHAR *msg_str = msgsem->sem_to_str();
delete msgsem;
if (!typsem)
	return false;
_TCHAR *typ_str = typsem->sem_to_str();
delete typsem;
return exittopopup(nlppp,msg_str,typ_str);
}

/********************************************
* FN:		GETPOPUPDATA
* CR:		06/15/02 AM.
* SUBJ:	Fetch data from GUI popup.
* RET:	True if ok, else false.
* NOTE:	ALSO CLEARS THE POPUP DATA.
*			Gets data that user typed into the popup, as a string.
* FORMS:	str getpopupdata()
********************************************/

_TCHAR *Arun::getpopupdata(
	Nlppp *nlppp
	)
{
if (!nlppp)
  return 0;
Parse *parse = nlppp->getParse();
if (!parse)
  return 0;
Ana *ana = parse->getAna();
NLP *nlp = ana->getNLP();

// Get data from GUI popup.
_TCHAR *str = nlp->getPopupdat();

// CLEAR OUT THE POPUP DATA.
nlp->setPopupdat(0);

// Return as string type.
return str;
}

#ifdef _ODBC

/********************************************
* FN:		DBBINDCOL
* CR:		06/15/02 AM.
* SUBJ:	Bind to column of db result set.
* RET:	True if ok, else false.
* NOTE:	Uses db stmt handle saved in parse object.
*			FIRST USE OF CALL BY REFERENCE.
* FORMS:	dbbindcol(column_num, type_str, bsiz, &var, &flagvar)
********************************************/


bool Arun::dbbindcol(
	Nlppp *nlppp,
	long column_num,
	_TCHAR *typ,
	long bsiz,
	RFASem *var,
	RFASem *flagv
	)
{
bool ok = dbbindcol_int(nlppp,column_num,typ,bsiz,var,flagv);
if (var)
	delete var;												// MEM LEAK.
if (flagv)
	delete flagv;											// MEM LEAK.
return ok;
}

/********************************************
* FN:		DBBINDCOL_INT
* CR:		03/10/03 AM.
* SUBJ:	Bind to column of db result set.
* RET:	True if ok, else false.
* NOTE:	Interpreted variant of dbbindcol.
*			dbbindcol(column_num, type_str, bsiz, &var, &flagvar)
********************************************/

bool Arun::dbbindcol_int(
	Nlppp *nlppp,
	long column_num,
	_TCHAR *typ,
	long bsiz,
	RFASem *var,
	RFASem *flagv
	)
{

// Need to validate column number also.

if (!Fn::SeqlValidType(typ))
	{
	_stprintf(Errbuf,_T("[dbbindcol: Warning. type not defined in ODBC.]"));
	return errOut(false); // UNFIXED
	}

// Check some types for sizing.
if (!_tcscmp(typ, _T("tinyblob")))												// 06/14/02 AM.
  bsiz = 255 + 2;																// 06/14/02 AM.
else if (!_tcscmp(typ,_T("blob")))												// 02/27/03 AM.
	bsiz = 65536 + 2;															// 02/27/03 AM.
else if (!_tcscmp(typ,_T("mediumblob")))										// 02/27/03 AM.
	bsiz = 16777215 + 2;														// 02/27/03 AM.
else if (!_tcscmp(typ,_T("longblob")))										// 02/27/03 AM.
	bsiz = 429496295 + 2;													// 02/27/03 AM.
//else if (
	// !strcmp(typ,"blob")												// 06/14/02 AM.
//		||
//		!strcmp(typ,"mediumblob")										// 06/14/02 AM.
//		||
//		!strcmp(typ,"longblob"))										// 06/14/02 AM.
//	{
//	sprintf_s(Errbuf,"[dbbindcol: Only handling tinyblob size.]");// 06/14/02 AM.
//	return errOut(false);													// 06/14/02 AM.
//	}

SQLSMALLINT sqltyp = Fn::SeqlTypeFromString(typ);					// 06/07/02 AM.
bool success = false;														// 06/08/02 AM.
switch (sqltyp)																// 06/07/02 AM.
	{
	case SQL_C_CHAR:															// 06/07/02 AM.
		success = Fn::dbbind2str(nlppp,var,sqltyp,column_num,bsiz,	// 06/08/02 AM.
							flagv);												// 06/12/02 AM.
		break;
	case SQL_C_BINARY:														// 06/07/02 AM.
		// SQL_C_CHAR  should be 2*len+1
		success = Fn::dbbind2str(nlppp,var,SQL_C_BINARY,
									column_num,bsiz,flagv);					// 06/12/02 AM.
		break;
	case SQL_C_STINYINT:														// 06/07/02 AM.
	case SQL_C_BIT:															// 06/07/02 AM.
	case SQL_C_SBIGINT:														// 06/07/02 AM.
	case SQL_C_SLONG:															// 06/07/02 AM.
	case SQL_C_SSHORT:														// 06/07/02 AM.
		// Ignoring user-supplied length.
		success = Fn::dbbind2long(nlppp,var,SQL_C_LONG,column_num,	// 06/08/02 AM.
							flagv);												// 06/12/02 AM.
		break;
	case SQL_C_DOUBLE:														// 06/07/02 AM.
	case SQL_C_FLOAT:															// 06/07/02 AM.
		// Ignoring user-supplied length.
		success = Fn::dbbind2float(nlppp,var,SQL_C_FLOAT,column_num,	// 06/12/02 AM.
							flagv);												// 06/12/02 AM.
		break;
	case SQL_C_DATE:															// 06/07/02 AM.
		// Ignoring user-supplied length.
		success = Fn::dbbind2str(nlppp,var,SQL_C_CHAR,column_num,11,	// 06/12/02 AM.
							flagv);												// 06/12/02 AM.
//		success = dbbind2str(nlppp,var,sqltyp,column_num,11);		// 06/08/02 AM.
		break;
	case SQL_C_TIME:															// 06/07/02 AM.
		// Ignoring user-supplied length.
		success = Fn::dbbind2str(nlppp,var,SQL_C_CHAR,column_num,6,	// 06/12/02 AM.
							flagv);												// 06/12/02 AM.
		break;
	case SQL_C_TIMESTAMP:													// 06/07/02 AM.
		// Ignoring user-supplied length.
		success = Fn::dbbind2str(nlppp,var,SQL_C_CHAR,column_num,23,	// 06/12/02 AM.
							flagv);												// 06/12/02 AM.
		break;
	default:
		break;
	}

return success;
}

// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	RFASem *columnsem,
	_TCHAR *typ,
	long bsiz,
	RFASem *var,
	RFASem *flagv
	)
{
if (!columnsem)
	return false;
bool ok = false;
long column_num = columnsem->sem_to_long(ok);
delete columnsem;
if (!ok)
	return false;
return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}


// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	long column_num,
	RFASem *typsem,
	long bsiz,
	RFASem *var,
	RFASem *flagv
	)
{
if (!typsem)
	return false;
_TCHAR *typ = typsem->sem_to_str();
delete typsem;
return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}


// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	long column_num,
	_TCHAR *typ,
	RFASem *bsizsem,
	RFASem *var,
	RFASem *flagv
	)
{
if (!bsizsem)
	return false;
bool ok = false;
long bsiz = bsizsem->sem_to_long(ok);
delete bsizsem;
if (!ok)
	return false;
return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}


// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	RFASem *columnsem,
	RFASem *typsem,
	long bsiz,
	RFASem *var,
	RFASem *flagv
	)
{
if (!columnsem && !typsem)
	return false;
if (!columnsem)
	{
	delete typsem;
	return false;
	}
else if (!typsem)
	{
	delete columnsem;
	return false;
	}

bool ok = false;
_TCHAR *typ = typsem->sem_to_str();
long column_num = columnsem->sem_to_long(ok);
delete columnsem;
delete typsem;
if (!ok)
	return false;

return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}


// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	RFASem *columnsem,
	_TCHAR *typ,
	RFASem *bsizsem,
	RFASem *var,
	RFASem *flagv
	)
{
if (!columnsem && !bsizsem)
	return false;
if (!columnsem)
	{
	delete bsizsem;
	return false;
	}
else if (!bsizsem)
	{
	delete columnsem;
	return false;
	}

bool ok1 = false;
bool ok2 = false;
long column_num = columnsem->sem_to_long(ok1);
long bsiz = bsizsem->sem_to_long(ok2);
delete columnsem;
delete bsizsem;
if (!ok1 || !ok2)
	return false;
return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}


// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	long column_num,
	RFASem *typsem,
	RFASem *bsizsem,
	RFASem *var,
	RFASem *flagv
	)
{
if (!typsem && !bsizsem)
	return false;
if (!typsem)
	{
	delete bsizsem;
	return false;
	}
else if (!bsizsem)
	{
	delete typsem;
	return false;
	}

bool ok = false;
long bsiz = bsizsem->sem_to_long(ok);
_TCHAR *typ = typsem->sem_to_str();
delete bsizsem;
delete typsem;
if (!ok)
	return false;
return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}

// VARIANT.
bool Arun::dbbindcol(
	Nlppp *nlppp,
	RFASem *columnsem,
	RFASem *typsem,
	RFASem *bsizsem,
	RFASem *var,
	RFASem *flagv
	)
{
if (!columnsem || !typsem || !bsizsem)
	{
	if (columnsem)
		delete columnsem;
	if (typsem)
		delete typsem;
	if (bsizsem)
		delete bsizsem;
	return false;
	}

bool ok1 = false;
bool ok2 = false;
long column_num = columnsem->sem_to_long(ok1);
_TCHAR *typ = typsem->sem_to_str();
long bsiz = bsizsem->sem_to_long(ok2);
delete columnsem;
delete typsem;
delete bsizsem;
if (!ok1 || !ok2)
	return false;
return dbbindcol(nlppp,column_num,typ,bsiz,var,flagv);
}

#endif // _ODBC

/********************************************
* FN:		PNMAKEVAR
* CR:		07/03/02 AM.
* SUBJ:	Make a PNODE variable.
* RET:	True if ok, else false.
* FORMS:	pnmakevar(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Arun::pnmakevar(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnmakevar: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

// If variable exists, error.
if (Ivar::nodeVarhasval(node, name1))
	{
	_stprintf(Errbuf,_T("[pnmakevar: Node variable already exists with a value.]"));
	return errOut(false); // UNFIXED
	}

Pn *pn = node->getData();

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

_TCHAR *x;											// Intern string.	// 01/09/07 AM.
nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
Ivar::nodeReplaceval(pn, name2, x);

return true;
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	long numval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnmakevar: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

// If variable exists, error.
if (Ivar::nodeVarhasval(node, name1))
	{
	_stprintf(Errbuf,_T("[pnmakevar: Node variable already exists with a value.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodeReplaceval(pn, name2, numval);

return true;
}

//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nd || !name1 || !*name1)
	{
	if (sem1)
		delete sem1;
	return false;
	}

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	if (sem1)
		delete sem1;
	_stprintf(Errbuf,_T("[pnmakevar: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

// If variable exists, error.
if (Ivar::nodeVarhasval(node, name1))
	{
	if (sem1)
		delete sem1;
	_stprintf(Errbuf,_T("[pnmakevar: Node variable already exists with a value.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

// Not copying sem.  Using it as node var's value rather than deleting it.
// NO. Copying and interning strings as needed.					// 01/09/07 AM.
if (sem1 && sem1->getType() == RSARGS)					// FIX.	// 08/07/03 AM.
	{
	Dlist<Iarg> *xargs = Iarg::copy_args(sem1->getArgs(),		// 01/09/07 AM.
												nlppp->parse_,true);		// 01/09/07 AM.
	Ivar::nodeReplaceval(pn,name2,xargs);							// 01/09/07 AM.
//	Ivar::nodeReplaceval(pn,name1,sem1->getArgs());	// FIX.	// 08/07/03 AM.
//	sem1->setArgs(0);	// Clear so won't be deleted.	// FIX.	// 08/07/03 AM.
	delete sem1;												// FIX.	// 08/07/03 AM.
	}
else																// FIX.	// 08/07/03 AM.
	{
	// Copying and interning.											// 01/09/07 AM.
	RFASem *cpsem = sem1->copy_rfasem(nlppp->parse_,true);	// 01/09/07 AM.
	Ivar::nodeReplaceval(pn, name2, cpsem);
	delete sem1;															// 01/09/07 AM.
	}

return true;
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnmakevar(nlppp,pnode,name1,strval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	long numval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnmakevar(nlppp,pnode,name1,numval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nodesem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnmakevar(nlppp,pnode,name1,sem1);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnmakevar(nlppp,nd,name,strval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	long numval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnmakevar(nlppp,nd,name,numval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem)
	{
	if (semval)
		delete semval;
	return false;
	}
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnmakevar(nlppp,nd,name,semval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnmakevar(nlppp,pnode,name,strval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	long numval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnmakevar(nlppp,pnode,name,numval);
}


//////// VARIANT ////////
bool Arun::pnmakevar(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem && !nodesem)
	{
	if (semval)
		delete semval;
	return false;
	}
if (!namesem)
	{
	if (semval)
		delete semval;
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	if (semval)
		delete semval;
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnmakevar(nlppp,pnode,name,semval);
}


/********************************************
* FN:		PNREPLACEVAL
* CR:		07/03/02 AM.
* SUBJ:	Replace value of a PNODE variable.
* RET:	True if ok, else false.
* FORMS:	pnreplaceval(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

if (strval && *strval)										// FIX.	// 07/06/06 AM.
	{
	_TCHAR *x;											// Intern string.	// 01/09/07 AM.
	nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
	Ivar::nodeReplaceval(pn, name2, x);
	}
else																// FIX.	// 07/06/06 AM.
	Ivar::nodeReplaceval(pn,name2,(long)0);			// FIX.	// 07/06/06 AM.

return true;
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	long numval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodeReplaceval(pn, name2, numval);

return true;
}

//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	float numval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodeReplaceval(pn, name2, numval);

return true;
}

//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nd || !name1 || !*name1)
	{
	if (sem1)
		delete sem1;
	return false;
	}

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	if (sem1)
		delete sem1;
	_stprintf(Errbuf,_T("[pnreplaceval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

// Not copying sem.  Using it as node var's value rather than deleting it.
// NO. Copying and interning strings as needed.					// 01/09/07 AM.
if (sem1 && sem1->getType() == RSARGS)					// FIX.	// 11/14/02 AM.
	{
	Dlist<Iarg> *xargs = Iarg::copy_args(sem1->getArgs(),		// 01/09/07 AM.
												nlppp->parse_,true);		// 01/09/07 AM.
	Ivar::nodeReplaceval(pn,name2,xargs);							// 01/09/07 AM.
//	Ivar::nodeReplaceval(pn,name1,sem1->getArgs());	// FIX.	// 11/14/02 AM.
//	sem1->setArgs(0);	// Clear so won't be deleted.	// FIX.	// 11/14/02 AM.
	delete sem1;												// FIX.	// 11/14/02 AM.
	}
else if (sem1)													// FIX.	// 11/14/02 AM.
	{
	// Copying and interning.											// 01/09/07 AM.
	RFASem *cpsem = sem1->copy_rfasem(nlppp->parse_,true);	// 01/09/07 AM.
	Ivar::nodeReplaceval(pn, name2, cpsem);
	delete sem1;															// 01/09/07 AM.
	}
else																// FIX.	// 07/06/06 AM.
	Ivar::nodeReplaceval(pn,name2,(long)0);			// FIX.	// 07/06/06 AM.

return true;
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnreplaceval(nlppp,pnode,name1,strval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	long numval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnreplaceval(nlppp,pnode,name1,numval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	float numval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnreplaceval(nlppp,pnode,name1,numval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nodesem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnreplaceval(nlppp,pnode,name1,sem1);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,nd,name,strval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	long numval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,nd,name,numval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	float numval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,nd,name,numval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem)
	{
	if (semval)
		delete semval;
	return false;
	}
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,nd,name,semval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,pnode,name,strval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	long numval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,pnode,name,numval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	float numval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,pnode,name,numval);
}


//////// VARIANT ////////
bool Arun::pnreplaceval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem && !nodesem)
	{
	if (semval)
		delete semval;
	return false;
	}
if (!namesem)
	{
	if (semval)
		delete semval;
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	if (semval)
		delete semval;
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnreplaceval(nlppp,pnode,name,semval);
}

/********************************************
* FN:		PNPUSHVAL
* CR:		12/12/14 AM.
* SUBJ:	Push value onto a PNODE variable's field.
* RET:	True if ok, else false.
* FORMS:	pnpushval(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

if (strval && *strval)										// FIX.	// 07/06/06 AM.
	{
	_TCHAR *x;											// Intern string.	// 01/09/07 AM.
	nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
//	Ivar::nodeReplaceval(pn, name2, x,false,true);
	Ivar::nodePushval(pn, name2, x,false,true); // 12/14/14 AM.
	}
else																// FIX.	// 07/06/06 AM.
	Ivar::nodeReplaceval(pn,name2,(long)0,false,true);			// FIX.	// 07/06/06 AM.

return true;
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	long numval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodePushval(pn, name2, numval,false,true); // 12/14/14 AM.

return true;
}

//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	float fnum
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodePushval(pn, name2, fnum,false,true); // 12/14/14 AM.

return true;
}

//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nd || !name1 || !*name1)
	{
	if (sem1)
		delete sem1;
	return false;
	}

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	if (sem1)
		delete sem1;
	_stprintf(Errbuf,_T("[pnpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

// Not copying sem.  Using it as node var's value rather than deleting it.
// NO. Copying and interning strings as needed.					// 01/09/07 AM.
// NOTE: PUSHING LIST OF ARGS NOT HANDLED. MERGE....			// 12/13/14 AM.

if (!sem1) // 12/14/14 AM.
	{
	Ivar::nodeReplaceval(pn,name2,(long)0,false,true);			// FIX.	// 07/06/06 AM.
	return true; // 12/14/14 AM.
	}

switch (sem1->getType()) // 12/14/14 AM.
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		{
		// TRY NO INTERN // 12/14/14 AM.
		_TCHAR *x = sem1->getName(); // 12/14/14 AM.
		Ivar::nodePushval(pn, name2, x,false,true); // 12/14/14 AM.
		}
		break;
	case RSARGS:
		{
		// Unhandled case. // 12/14/14 AM.
		delete sem1;       // 12/14/14 AM.
		return false;      // 12/14/14 AM.

		Dlist<Iarg> *xargs = Iarg::copy_args(sem1->getArgs(),		// 01/09/07 AM.
													nlppp->parse_,true);		// 01/09/07 AM.
		Ivar::nodeReplaceval(pn,name2,xargs);							// 01/09/07 AM.
	//	Ivar::nodeReplaceval(pn,name1,sem1->getArgs());	// FIX.	// 11/14/02 AM.
	//	sem1->setArgs(0);	// Clear so won't be deleted.	// FIX.	// 11/14/02 AM.

		}
		break;
	case RSLONG: // 12/14/14 AM.
		{
		long numval = sem1->getLong(); // 12/14/14 AM.
		Ivar::nodeReplaceval(pn, name2, numval,false,true); // 12/14/14 AM.
		}
		break;
	// INCOMPLETE******	// 12/14/14 AM.
	case RSFLOAT: // 12/14/14 AM.
		{
		float fnum = sem1->getFloat(); // 12/14/14 AM.
		Ivar::nodeReplaceval(pn, name2, fnum,false,true); // 12/14/14 AM.
		}
		break;
	default:
		Ivar::nodeReplaceval(pn,name2,(long)0,false,true);			// FIX.	// 07/06/06 AM.
		break;
	}

delete sem1;												// FIX.	// 11/14/02 AM.

return true;
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnpushval(nlppp,pnode,name1,strval);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	long numval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnpushval(nlppp,pnode,name1,numval);
}

//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	float fnum
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnpushval(nlppp,pnode,name1,fnum);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nodesem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnpushval(nlppp,pnode,name1,sem1);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,nd,name,strval);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	long numval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,nd,name,numval);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	float fnum
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,nd,name,fnum);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem)
	{
	if (semval)
		delete semval;
	return false;
	}
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,nd,name,semval);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,pnode,name,strval);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	long numval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,pnode,name,numval);
}

//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	float fnum
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,pnode,name,fnum);
}


//////// VARIANT ////////
bool Arun::pnpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem && !nodesem)
	{
	if (semval)
		delete semval;
	return false;
	}
if (!namesem)
	{
	if (semval)
		delete semval;
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	if (semval)
		delete semval;
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnpushval(nlppp,pnode,name,semval);
}


/********************************************
* FN:		PNRPUSHVAL
* CR:		12/12/14 AM.
* SUBJ:	Push value onto a PNODE variable's field.
* RET:	True if ok, else false.
* FORMS:	pnpushval(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnrpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

if (strval && *strval)										// FIX.	// 07/06/06 AM.
	{
	_TCHAR *x;											// Intern string.	// 01/09/07 AM.
	nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
//	Ivar::nodeReplaceval(pn, name2, x,false,true);
	Ivar::nodePushval(pn, name2, x,false,false); // 12/14/14 AM.
	}
else																// FIX.	// 07/06/06 AM.
	Ivar::nodeReplaceval(pn,name2,(long)0,false,false);			// FIX.	// 07/06/06 AM.

return true;
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	long numval
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnrpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodePushval(pn, name2, numval,false,false); // 12/14/14 AM.

return true;
}

//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	float fnum
	)
{
if (!nd || !name1 || !*name1)
	return false;

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnrpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

Ivar::nodePushval(pn, name2, fnum,false,false); // 12/14/14 AM.

return true;
}

//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nd || !name1 || !*name1)
	{
	if (sem1)
		delete sem1;
	return false;
	}

Node<Pn> *node = (Node<Pn> *) nd;

if (*name1 == '$')												// Get special var.
	{
	if (sem1)
		delete sem1;
	_stprintf(Errbuf,_T("[pnrpushval: Variable name can't start with '$'.]"));
	return errOut(false); // UNFIXED
	}

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

Pn *pn = node->getData();

// Not copying sem.  Using it as node var's value rather than deleting it.
// NO. Copying and interning strings as needed.					// 01/09/07 AM.
// NOTE: PUSHING LIST OF ARGS NOT HANDLED. MERGE....			// 12/13/14 AM.

if (!sem1) // 12/14/14 AM.
	{
	Ivar::nodeReplaceval(pn,name2,(long)0,false,false);			// FIX.	// 07/06/06 AM.
	return true; // 12/14/14 AM.
	}

switch (sem1->getType()) // 12/14/14 AM.
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		{
		// TRY NO INTERN // 12/14/14 AM.
		_TCHAR *x = sem1->getName(); // 12/14/14 AM.
		Ivar::nodePushval(pn, name2, x,false,false); // 12/14/14 AM.
		}
		break;
	case RSARGS:
		{
		// Unhandled case. // 12/14/14 AM.
		delete sem1;       // 12/14/14 AM.
		return false;      // 12/14/14 AM.

		Dlist<Iarg> *xargs = Iarg::copy_args(sem1->getArgs(),		// 01/09/07 AM.
													nlppp->parse_,true);		// 01/09/07 AM.
		Ivar::nodeReplaceval(pn,name2,xargs);							// 01/09/07 AM.
	//	Ivar::nodeReplaceval(pn,name1,sem1->getArgs());	// FIX.	// 11/14/02 AM.
	//	sem1->setArgs(0);	// Clear so won't be deleted.	// FIX.	// 11/14/02 AM.

		}
		break;
	case RSLONG: // 12/14/14 AM.
		{
		long numval = sem1->getLong(); // 12/14/14 AM.
		Ivar::nodeReplaceval(pn, name2, numval,false,false); // 12/14/14 AM.
		}
		break;
	// INCOMPLETE******	// 12/14/14 AM.
	case RSFLOAT: // 12/14/14 AM.
		{
		float fnum = sem1->getFloat(); // 12/14/14 AM.
		Ivar::nodeReplaceval(pn, name2, fnum,false,false); // 12/14/14 AM.
		}
		break;
	default:
		Ivar::nodeReplaceval(pn,name2,(long)0,false,false);			// FIX.	// 07/06/06 AM.
		break;
	}

delete sem1;												// FIX.	// 11/14/02 AM.

return true;
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	_TCHAR *strval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnrpushval(nlppp,pnode,name1,strval);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	long numval
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnrpushval(nlppp,pnode,name1,numval);
}

//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	float fnum
	)
{
if (!nodesem)
	return false;
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnrpushval(nlppp,pnode,name1,fnum);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	_TCHAR *name1,
	RFASem *sem1
	)
{
if (!nodesem)
	{
	if (sem1)
		delete sem1;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
return pnrpushval(nlppp,pnode,name1,sem1);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,nd,name,strval);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	long numval
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,nd,name,numval);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	float fnum
	)
{
if (!namesem)
	return false;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,nd,name,fnum);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	NODE *nd,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem)
	{
	if (semval)
		delete semval;
	return false;
	}
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,nd,name,semval);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	_TCHAR *strval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,pnode,name,strval);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	long numval
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,pnode,name,numval);
}

//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	float fnum
	)
{
if (!namesem && !nodesem)
	return false;
if (!namesem)
	{
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,pnode,name,fnum);
}


//////// VARIANT ////////
bool Arun::pnrpushval(
	Nlppp *nlppp,
	RFASem *nodesem,
	RFASem *namesem,
	RFASem *semval
	)
{
if (!namesem && !nodesem)
	{
	if (semval)
		delete semval;
	return false;
	}
if (!namesem)
	{
	if (semval)
		delete semval;
	delete nodesem;
	return false;
	}
if (!nodesem)
	{
	if (semval)
		delete semval;
	delete namesem;
	return false;
	}
NODE *pnode = nodesem->sem_to_node();
delete nodesem;
_TCHAR *name = namesem->sem_to_str();
delete namesem;
return pnrpushval(nlppp,pnode,name,semval);
}



/********************************************
* FN:		PNINSERT
* CR:		07/14/08 AM.
* SUBJ:	Insert a new node into parse tree.
* RET:	node - the newly created and inserted node.
* FORMS:	pninsert(name_str, pos_pnode, after_num)
********************************************/

RFASem *Arun::pninsert(
	Nlppp *nlppp,
	_TCHAR *name_str,
	NODE *pos_nd,
	long after_num
	)
{
RFASem *sem = 0;
Parse *parse = nlppp->getParse();
Node<Pn> *pos_node = (Node<Pn> *) pos_nd;

if (!pos_node || !name_str || !*name_str)
	return sem;
if (after_num != 0 && after_num != 1)
	return sem;

switch (nlppp->region_)
	{
	case REGCODE:
		break;
	default:
		_stprintf(Errbuf,
		_T("[pninsert: Error. Must appear in @CODE region.]"));
		errOut(true);	// UNFIXED
		return sem;
	}

if (*name_str != '_')
	{
	_stprintf(Errbuf,_T("[pninsert: Only nonliteral node name allowed.]"));
	errOut(false); // UNFIXED
	return sem;
	}

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();

// DUMMY UP A NODE.
long o_start = -1;
long o_end = -1;
_TCHAR *text = 0;
Sym *sym = parse->getSym(name_str);
_TCHAR *str = sym->getStr();
long len = _tcslen(str);
enum Pntype typ = PNNULL;

// Todo: These type checks are hooks for future enhancement of pninsert.
if (*name_str == '_')
	typ = PNNODE;
else if (alphabetic(*name_str))
	{
	typ = PNALPHA;
	}
else if (_istdigit(*name_str))
	{
	typ = PNNUM;
	}
else if (_istpunct(*name_str))
	{
	typ = PNPUNCT;
	}
else if (_istspace(*name_str))
	{
	typ = PNWHITE;
	}

Node<Pn> *new_node = Pn::makeNode(
	o_start,	// o_start
	o_end,	// o_end
	typ,		// Node type
	text,		// Pointer to text input buffer
	str,		// Suggested concept str
	sym,		// Suggested concept sym
	0,			// Assoc line number
	0,			// Assoc code pass
	0			// Assoc rule line
	);

if (after_num)
	tree->insertRight(*new_node, *pos_node);
else
	tree->insertLeft(*new_node, *pos_node);

sem = new RFASem(new_node);	// Return the node.
return sem;
}

// VARIANT
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	RFASem *name_sem,
	NODE *pos_nd,
	long after_num
	)
{
if (!name_sem)
	return 0;
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
return pninsert(nlppp,name_str,pos_nd,after_num);
}


// VARIANT
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	_TCHAR *name_str,
	NODE *pos_nd,
	RFASem *after_sem
	)
{
bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
return pninsert(nlppp,name_str,pos_nd,after_num);
}

// VARIANT
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	RFASem *name_sem,
	NODE *pos_nd,
	RFASem *after_sem
	)
{
if (!name_sem)
	{
	if (after_sem)
		delete after_sem;
	return 0;
	}

_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;

bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
return pninsert(nlppp,name_str,pos_nd,after_num);
}

// VARIANT.
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	_TCHAR *name_str,
	RFASem *pos_sem,
	long after_num
	)
{
if (!pos_sem)
	return 0;

NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;
return pninsert(nlppp,name_str,pos_nd,after_num);
}




// VARIANT
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *pos_sem,
	long after_num
	)
{
if (!name_sem && !pos_sem)
	return 0;
if (!name_sem)
	{
	if (pos_sem)
		delete pos_sem;
	return 0;
	}
if (!pos_sem)
	{
	if (name_sem)
		delete name_sem;
	return 0;
	}
_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;
return pninsert(nlppp,name_str,pos_nd,after_num);
}


// VARIANT
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	_TCHAR *name_str,
	RFASem *pos_sem,
	RFASem *after_sem
	)
{
if (!pos_sem)
	{
	if (after_sem)
		delete after_sem;
	return 0;
	}

bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;
return pninsert(nlppp,name_str,pos_nd,after_num);
}

// VARIANT
RFASem *Arun::pninsert(
	Nlppp *nlppp,
	RFASem *name_sem,
	RFASem *pos_sem,
	RFASem *after_sem
	)
{
if (!name_sem && !pos_sem)
	{
	if (after_sem)
		delete after_sem;
	return 0;
	}
if (!name_sem)
	{
	if (pos_sem)
		delete pos_sem;
	if (after_sem)
		delete after_sem;
	return 0;
	}
if (!pos_sem)
	{
	if (name_sem)
		delete name_sem;
	if (after_sem)
		delete after_sem;
	return 0;
	}

_TCHAR *name_str = name_sem->sem_to_str();
delete name_sem;
NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;

bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
return pninsert(nlppp,name_str,pos_nd,after_num);
}


/********************************************
* FN:		PNMOVE
* CR:		10/06/13 AM.
* SUBJ:	Move node to new position in parse tree.
* RET:	node - given node, now in new position, else null.
* FORMS:	pnmove(name_str, pos_pnode, after_num)
********************************************/

RFASem *Arun::pnmove(
	Nlppp *nlppp,
	NODE *pn_nd,	// Node to excise and move. (FROM)
	NODE *pos_nd,	// Node at destination.  (TO)
	long after_num
	)
{
RFASem *sem = 0;
Parse *parse = nlppp->getParse();
Node<Pn> *pn_node = (Node<Pn> *) pn_nd;
Node<Pn> *pos_node = (Node<Pn> *) pos_nd;

if (!pn_node || !pos_node)
	return sem;
if (after_num != 0 && after_num != 1)
	return sem;

switch (nlppp->region_)
	{
	case REGCODE:
		break;
	default:
		_stprintf(Errbuf,
		_T("[pnmove: Error. Must appear in @CODE region.]"));
		errOut(true);	// UNFIXED
		return sem;
	}

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();

// DETACH NODE HERE.
tree->exciseSubs(pn_node, pn_node);

// ATTACH NODE HERE.
if (after_num)
	tree->insertRight(*pn_node, *pos_node);
else
	tree->insertLeft(*pn_node, *pos_node);

sem = new RFASem(pn_node);	// Return the node.
return sem;
}

// VARIANT
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	RFASem *pn_sem,
	NODE *pos_nd,
	long after_num
	)
{
if (!pn_sem)
	return 0;
NODE *pn_nd = pn_sem->sem_to_node();
delete pn_sem;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}


// VARIANT
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	NODE *pn_nd,
	NODE *pos_nd,
	RFASem *after_sem
	)
{
bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}

// VARIANT
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	RFASem *pn_sem,
	NODE *pos_nd,
	RFASem *after_sem
	)
{
if (!pn_sem)
	{
	if (after_sem)
		delete after_sem;
	return 0;
	}

NODE *pn_nd = pn_sem->sem_to_node();
delete pn_sem;

bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}

// VARIANT.
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	NODE *pn_nd,
	RFASem *pos_sem,
	long after_num
	)
{
if (!pos_sem)
	return 0;

NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}


// VARIANT
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	RFASem *pn_sem,
	RFASem *pos_sem,
	long after_num
	)
{
if (!pn_sem && !pos_sem)
	return 0;

if (!pn_sem)
	{
	if (pos_sem)
		delete pos_sem;
	return 0;
	}

if (!pos_sem)
	{
	if (pn_sem)
		delete pn_sem;
	return 0;
	}

NODE *pn_nd = pn_sem->sem_to_node();
delete pn_sem;
NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}


// VARIANT
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	NODE *pn_nd,
	RFASem *pos_sem,
	RFASem *after_sem
	)
{
if (!pos_sem)
	{
	if (after_sem)
		delete after_sem;
	return 0;
	}

bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}

// VARIANT
RFASem *Arun::pnmove(
	Nlppp *nlppp,
	RFASem *pn_sem,
	RFASem *pos_sem,
	RFASem *after_sem
	)
{
if (!pn_sem && !pos_sem)
	{
	if (after_sem)
		delete after_sem;
	return 0;
	}
if (!pn_sem)
	{
	if (pos_sem)
		delete pos_sem;
	if (after_sem)
		delete after_sem;
	return 0;
	}
if (!pos_sem)
	{
	if (pn_sem)
		delete pn_sem;
	if (after_sem)
		delete after_sem;
	return 0;
	}

NODE *pn_nd = pn_sem->sem_to_node();
delete pn_sem;
NODE *pos_nd = pos_sem->sem_to_node();
delete pos_sem;

bool ok;
long after_num = 0;
if (after_sem)
	{
	after_num = after_sem->sem_to_long(ok);
	delete after_sem;
	}
if (!ok)
	return 0;
return pnmove(nlppp,pn_nd,pos_nd,after_num);
}


/********************************************
* FN:		STEM
* CR:		08/04/02 AM.
* SUBJ:	Stem nouns and verbs.
* RET:	str - stem of noun or verb, else return what's given.
* FORMS:	stem(word_str)
********************************************/

_TCHAR *Arun::stem(
	Nlppp *nlppp,
	_TCHAR *word
	)
{
if (!word || !*word)
	return 0;

_TCHAR *buf=0;
if (!(buf = strstem(word)))
	return word;

_TCHAR *str=0;
Parse *parse = nlppp->getParse();
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);
return str;
}

// VARIANT

_TCHAR *Arun::stem(
	Nlppp *nlppp,
	RFASem *wordsem
	)
{
if (!wordsem)
	return 0;
_TCHAR *word = wordsem->sem_to_str();
delete wordsem;
return stem(nlppp,word);
}

_TCHAR *Arun::stem(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}

/********************************************
* FN:		FNSORTCONSBYATTR
* CR:		12/14/02 AM.
* SUBJ:	Sort an array of kb concepts.
* RET:	Sorted array (COPIED from given array.)
* FORMS:	sortconsbyattr(
				array - multi-valued array of kb concepts.
				attr = str.  Name of attribute to sort on.
				numeric = 0 (default) if str, 1 if numeric.
				descending = 0 (default) if ascending, 1 if descending.
* NOTE:	Return true for simple types.
********************************************/

RFASem *Arun::sortconsbyattr(
	Nlppp *nlppp,
	RFASem *array_sem,
	_TCHAR *attr,
	long numeric,
	long descending
	)
{
if (!array_sem)
	return 0;

// Need to get the current KB.
Parse *parse = nlppp->getParse();
CG *cg = parse->getAna()->getCG();

Dlist<Iarg> *arr=0;
switch (array_sem->getType())
	{
	case RSARGS:
		arr = array_sem->getArgs();
		break;
	case RS_KBCONCEPT:
		// Reuse the sem, rather than deleting and creating.
		return array_sem;
	default:
		{
		_stprintf(Errbuf,_T("[sortconsbyattr: Given non-array variable.]"));
		parse->errOut(false); // UNFIXED
		delete array_sem;
		return 0;
		}
		break;
	}

if (!attr || !*attr)
	{
	_stprintf(Errbuf,_T("[sortconsbyattr: Given no concept attribute.]"));
	parse->errOut(false); // UNFIXED
	delete array_sem;
	return 0;
	}

Dlist<Iarg> *sorted = (Dlist<Iarg> *) sort_arr_kbcon_by_attr(
	arr,									// Array of kbconcepts.
	attr,									// Name of concept attribute.
	(numeric ? true : false),		// True if numeric, false if string.
	(descending ? true : false),	// True if descending, false if ascending.
	cg
	);

delete array_sem;
return new RFASem(sorted);
}


// VARIANT.
RFASem *Arun::sortconsbyattr(											// 12/14/02 AM.
	Nlppp *nlppp,
	RFASem *array_sem,
	RFASem *attr_sem,
	long numeric,
	long descending
	)
{
if (!attr_sem)
	{
	if (array_sem)
		delete array_sem;
	return 0;
	}

_TCHAR *attr = 0;
switch (attr_sem->getType())
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		attr = attr_sem->getName();
		break;
	default:
		{
		_stprintf(Errbuf,_T("[sortconsbyattr: Given non-array variable.]"));
		nlppp->getParse()->errOut(false); // UNFIXED
		delete array_sem;
		return 0;
		}
	}
// Deletes the sem as needed.
return sortconsbyattr(nlppp,array_sem,attr,numeric,descending);
}


/********************************************
* FN:		INPUTRANGE
* CR:		12/15/02 AM.
* SUBJ:	Fetch a substring of the input text.
* RET:	str - Substring specified by start and end offsets.
* FORMS:	inputrange(start_num,end_num)
********************************************/

_TCHAR *Arun::inputrange(
	Nlppp *nlppp,
	long start,
	long end
	)
{
Parse *parse = nlppp->getParse();
_TCHAR *text = parse->getText();	// Entire input text.
long length = parse->getLength();	// Length of input text.

if (start >= length || end >= length
 || start < 0 || end < 0
 || start > end)
	{
	_stprintf(Errbuf,_T("[inputrange: Error. Bad offsets.]"));
	parse->errOut(false); // UNFIXED
	return 0;
	}

long len = end - start + 1;

_TCHAR *buf = Chars::create(len+1);
_tcsnccpy(buf,&(text[start]),len);
buf[len] = '\0';	// Terminate string.
_TCHAR *str;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);			// Free up buffer.
return str;
}


// VARIANT.
_TCHAR *Arun::inputrange(
	Nlppp *nlppp,
	RFASem *start_sem,
	long end
	)
{
if (!start_sem)
	return 0;
bool ok;
long start = start_sem->sem_to_long(ok);
delete start_sem;
if (!ok)
	return 0;
return inputrange(nlppp,start,end);
}


// VARIANT.
_TCHAR *Arun::inputrange(
	Nlppp *nlppp,
	long start,
	RFASem *end_sem
	)
{
if (!end_sem)
	return 0;
bool ok;
long end = end_sem->sem_to_long(ok);
delete end_sem;
if (!ok)
	return 0;
return inputrange(nlppp,start,end);
}


// VARIANT.
_TCHAR *Arun::inputrange(
	Nlppp *nlppp,
	RFASem *start_sem,
	RFASem *end_sem
	)
{
if (!start_sem)
	{
	if (end_sem)
		delete end_sem;
	return 0;
	}
if (!end_sem)
	{
	delete start_sem;
	return 0;
	}

bool ok1;
long start = start_sem->sem_to_long(ok1);
delete start_sem;

bool ok2;
long end = end_sem->sem_to_long(ok2);
delete end_sem;

if (!ok1 || !ok2)
	return 0;

return inputrange(nlppp,start,end);
}


/********************************************
* FN:		XMLSTR
* CR:		05/10/03 AM.
* SUBJ:	Convert string to XML string.
* RET:	str - string with XML char formats.
* FORMS:	char *xmlstr(str)
********************************************/

_TCHAR *Arun::xmlstr(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::text2xml(str1, len, nlppp->htab());
return xstr;
}

// VARIANT
_TCHAR *Arun::xmlstr(
	Nlppp *nlppp,
	RFASem *strsem
	)
{
if (!strsem)
	return 0;
_TCHAR *str1 = strsem->sem_to_str();
delete strsem;
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::text2xml(str1, len, nlppp->htab());
return xstr;
}

_TCHAR *Arun::xmlstr(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}

/********************************************
* FN:		SQLSTR
* CR:		07/22/03 AM.
* SUBJ:	Convert string to SQL string.
* RET:	str - string with SQL char formats.
* FORMS:	char *sqlstr(str)
********************************************/

_TCHAR *Arun::sqlstr(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::text2sql(str1, len, nlppp->htab());
return xstr;
}

// VARIANT
_TCHAR *Arun::sqlstr(
	Nlppp *nlppp,
	RFASem *strsem
	)
{
if (!strsem)
	return 0;
_TCHAR *str1 = strsem->sem_to_str();
delete strsem;
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::text2sql(str1, len, nlppp->htab());
return xstr;
}

_TCHAR *Arun::sqlstr(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:   DICTFIRST
* CR:   05/29/03 AM.
* SUBJ: Get first alphabetic word in kb dict.
* RET:  concept - First dictionary concept found.
* FORM: dictfirst()
* NOTE: In conjunction with dictnext, for easy traverse of dictionary.
********************************************/

RFASem *Arun::dictfirst(
	Nlppp *nlppp
	)
{
Parse *parse = nlppp->parse_;

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *wordConcept = cg->dictFirst();

if (!wordConcept)
	return 0;

return new RFASem(wordConcept, RS_KBCONCEPT);
}



/********************************************
* FN:   FNDICTNEXT
* CR:   05/29/03 AM.
* SUBJ: Get next alphabetic word in kb dict.
* RET:  concept.
* FORM: dictnext(dict_con)
* NOTE: For easy traverse of dictionary.
********************************************/

RFASem *Arun::dictnext(
	Nlppp *nlppp,
	RFASem* kbsem
	)
{
Parse *parse = nlppp->parse_;

if (!kbsem)
	{
	_stprintf(Errbuf,_T("[dictnext: Warning. Given no concept.]"));
	parse->errOut(false); // UNFIXED
	return 0;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (kbsem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[dictnext: Bad semantic arg.]"));
	parse->errOut(false); // UNFIXED
	delete kbsem;
	return 0;
	}
CONCEPT *conc1 = kbsem->getKBconcept();
delete kbsem;

CONCEPT *wordConcept = cg->dictNext(conc1);

if (!wordConcept)
	return 0;
return new RFASem(wordConcept, RS_KBCONCEPT);
}


/********************************************
* FN:		FNFINDANA
* CR:		02/03/03 AM.
* SUBJ:	See if an analyzer is loaded in memory.
* RET:	Bool (1 or 0)
********************************************/

bool Arun::findana(
	Nlppp *nlppp,
	_TCHAR *appname
	)
{
Parse *parse = nlppp->parse_;
NLP *nlp = parse->getNLP();		// [DEGLOB]	// 10/15/20 AM.
VTRun *vtrun = nlp->getVTRun();	// [DEGLOB]	// 10/15/20 AM.

if (!appname || !*appname)
	return false;

//NLP *nlp1 = VTRun_Ptr->findAna(appname);	// [DEGLOB]	// 10/15/20 AM.
NLP *nlp1 = vtrun->findAna(appname);		// [DEGLOB]	// 10/15/20 AM.
bool retval = (nlp1 ? true : false);
return retval;
}

// VARIANT.
bool Arun::findana(
	Nlppp *nlppp,
	RFASem *appname_sem
	)
{
if (!appname_sem)
	return false;
_TCHAR *appname = sem_to_str(appname_sem);
delete appname_sem;
return findana(nlppp,appname);
}

bool Arun::findana(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}


/********************************************
* FN:		INPUTRANGETOFILE
* CR:		05/29/03 AM.
* SUBJ:	Output a segment of the input buffer to a file.
* RET:	True if ok, else false.
* FORMS:	inputrangetofile(start_num,end_num,file_ostr)
*			start_num,end_num - range of offsets for input buffer.
* NOTE:	Optimization, doesn't create strings, etc.
********************************************/

bool Arun::inputrangetofile(
	Nlppp *nlppp,
	long num1,
	long num2,
	_t_ostream *ostr
	)
{
Parse *parse = nlppp->parse_;

if (!ostr)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Couldn't fetch file stream.]"));
	return parse->errOut(false); // UNFIXED
	}

if (num1 < 0 || num1 > num2)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Bad range.]"));
	return parse->errOut(false); // UNFIXED
	}
if (num2 >= parse->getLength())	// Larger than file length.
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Offset exceeds input file length.]"));
	return parse->errOut(false); // UNFIXED
	}

// Get buffer range and output it.
_TCHAR *text = parse->getText();
if (!text)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: No input text buffer.]"));
	return parse->errOut(false); // UNFIXED
	}

_TCHAR *ptr = &(text[num1]) - 1;	// Minus 1 for convenient looping.
for (; num1 <= num2; ++num1)
	*ostr << *++ptr;
*ostr << flush;

return true;
}


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	long num1,
	long num2,
	RFASem *ostr_sem
	)
{
if (!ostr_sem)
	return false;
_t_ostream *ostr = sem_to_ostream(ostr_sem);
delete ostr_sem;
return inputrangetofile(nlppp,num1,num2,ostr);
}


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	long num1,
	RFASem *num2_sem,
	_t_ostream *ostr
	)
{
if (!num2_sem)
	return false;
bool ok = false;
long num2 = num2_sem->sem_to_long(ok);
delete num2_sem;
if (!ok)
	return false;
return inputrangetofile(nlppp,num1,num2,ostr);
}


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	RFASem *num1_sem,
	long num2,
	_t_ostream *ostr
	)
{
if (!num1_sem)
	return false;
bool ok = false;
long num1 = num1_sem->sem_to_long(ok);
delete num1_sem;
if (!ok)
	return false;
return inputrangetofile(nlppp,num1,num2,ostr);
}

///////////////////


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	long num1,
	RFASem *num2_sem,
	RFASem *ostr_sem
	)
{
if (!ostr_sem && !num2_sem)
	return false;
if (!ostr_sem)
	{
	delete num2_sem;
	return false;
	}
if (!num2_sem)
	{
	delete ostr_sem;
	return false;
	}
bool ok = false;
long num2 = num2_sem->sem_to_long(ok);
delete num2_sem;
_t_ostream *ostr = sem_to_ostream(ostr_sem);
delete ostr_sem;
if (!ok)
	return false;
return inputrangetofile(nlppp,num1,num2,ostr);
}


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	RFASem *num1_sem,
	RFASem *num2_sem,
	_t_ostream *ostr
	)
{
if (!num1_sem && !num2_sem)
	return false;
if (!num1_sem)
	{
	delete num2_sem;
	return false;
	}
if (!num2_sem)
	{
	delete num1_sem;
	return false;
	}

bool ok1 = false;
long num1 = num1_sem->sem_to_long(ok1);
delete num1_sem;
bool ok2 = false;
long num2 = num2_sem->sem_to_long(ok2);
delete num2_sem;
if (!ok1 || !ok2)
	return false;
return inputrangetofile(nlppp,num1,num2,ostr);
}


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	RFASem *num1_sem,
	long num2,
	RFASem *ostr_sem
	)
{
if (!num1_sem && !ostr_sem)
	return false;
if (!num1_sem)
	{
	delete ostr_sem;
	return false;
	}
if (!ostr_sem)
	{
	delete num1_sem;
	return false;
	}

bool ok1 = false;
long num1 = num1_sem->sem_to_long(ok1);
delete num1_sem;
_t_ostream *ostr = sem_to_ostream(ostr_sem);
delete ostr_sem;
if (!ok1)
	return false;
return inputrangetofile(nlppp,num1,num2,ostr);
}


// VARIANT
bool Arun::inputrangetofile(
	Nlppp *nlppp,
	RFASem *num1_sem,
	RFASem *num2_sem,
	RFASem *ostr_sem
	)
{
bool ok1 = false;
long num1 = 0;
if (num1_sem)
	{
	num1 = num1_sem->sem_to_long(ok1);
	delete num1_sem;
	}

bool ok2 = false;
long num2 = 0;
if (num2_sem)
	{
	num2 = num2_sem->sem_to_long(ok2);
	delete num2_sem;
	}

bool ok3 = false;
_t_ostream *ostr = 0;
if (ostr_sem)
	{
	ostr = sem_to_ostream(ostr_sem);
	delete ostr_sem;
	ok3 = (ostr ? true : false);
	}

if (!ok1 || !ok2 || !ok3)
	return false;

return inputrangetofile(nlppp,num1,num2,ostr);
}



/********************************************
* FN:   RESOLVEURL
* CR:   05/29/03 AM.
* SUBJ: Resolve embedded URL with base URL.
* RET:  str - Resolved URL string.
* FORM: resolveurl(base_url_str,embedded_url_str)
********************************************/

_TCHAR *Arun::resolveurl(
	Nlppp *nlppp,
	_TCHAR *burl,
	_TCHAR *eurl
	)
{
Parse *parse = nlppp->parse_;

_TCHAR buf[1024];
if (!resolve_url(burl,eurl,parse,/*DU*/buf))
	{
	_stprintf(Errbuf,_T("[resolveurl: Could not resolve url.]"));
	parse->errOut(false); // UNFIXED
	return 0;
	}

_TCHAR *xstr;
parse->internStr(buf,xstr);
return xstr;
}


// VARIANT.
_TCHAR *Arun::resolveurl(
	Nlppp *nlppp,
	RFASem *burl_sem,
	_TCHAR *eurl
	)
{
if (!burl_sem)
	return 0;
_TCHAR *burl = burl_sem->sem_to_str();
delete burl_sem;
return resolveurl(nlppp,burl,eurl);
}


// VARIANT.
_TCHAR *Arun::resolveurl(
	Nlppp *nlppp,
	_TCHAR *burl,
	RFASem *eurl_sem
	)
{
if (!eurl_sem)
	return 0;
_TCHAR *eurl = eurl_sem->sem_to_str();
delete eurl_sem;
return resolveurl(nlppp,burl,eurl);
}


// VARIANT.
_TCHAR *Arun::resolveurl(
	Nlppp *nlppp,
	RFASem *burl_sem,
	RFASem *eurl_sem
	)
{
if (!burl_sem && !eurl_sem)
	return 0;
if (!burl_sem)
	{
	delete eurl_sem;
	return 0;
	}
if (!eurl_sem)
	{
	delete burl_sem;
	return 0;
	}

_TCHAR *eurl = eurl_sem->sem_to_str();
delete eurl_sem;
_TCHAR *burl = burl_sem->sem_to_str();
delete burl_sem;
return resolveurl(nlppp,burl,eurl);
}



/********************************************
* FN:		FNMKDIR
* CR:		12/06/01 AM.
* SUBJ:	Make directory.
* RET:	True if ok.
* FORMS:	mkdir(str);
********************************************/

bool Arun::mkdir(
	Nlppp *nlppp,
	_TCHAR *str
	)
{
Parse *parse = nlppp->parse_;

if (!str|| !*str)
	{
	_stprintf(Errbuf,_T("[mkdir: Warning. Given no directory.]"));
	return parse->errOut(true); // UNFIXED
	}

bool ok = true;
#ifdef _OLDWAY_
#ifndef LINUX
ok = (_tmkdir(str) ? false : true);
#else
_TCHAR cmd[2048];
_stprintf(cmd,_T("mkdir %s"), str);
ok = (_tsystem(cmd) ? false : true);
#endif
#endif

_TCHAR cmd[2048];	// 09/28/08 AM.
_stprintf(cmd,_T("mkdir %s"), str);	// 09/28/08 AN.
run_silent(cmd);			// system() w/o DOC CMD PROMPT POPUP.	// 09/28/08 AM.
ok = true;	// 09/28/08 AM.

if (!ok)
	{
	_stprintf(Errbuf,_T("[mkdir: Warning. Couldn't create directory=%s.]"),str);
	return parse->errOut(true); // UNFIXED
	}

return true;
}


// VARIANT.
bool Arun::mkdir(
	Nlppp *nlppp,
	RFASem *str_sem
	)
{
_TCHAR *str = 0;
if (str_sem)
	{
	str = str_sem->sem_to_str();
	delete str_sem;
	}
return mkdir(nlppp,str);
}

bool Arun::mkdir(	// 07/11/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return false;
}



/********************************************
* FN:		FNSPLIT
* CR:		08/14/03 AM.
* SUBJ:	Split a string according to delimiter chars.
* FORMS:	split(str, charstr)
********************************************/

RFASem *Arun::split(
	Nlppp *nlppp,
	_TCHAR *buf,
	_TCHAR *chrs
	)
{
RFASem *sem = 0;
Parse *parse = nlppp->parse_;


if (!buf || !*buf)
	return 0;	// Given empty buffer is ok.
if (!chrs || !*chrs)
	{
	_stprintf(Errbuf,_T("[split: Empty delimiter string.]"));
	parse->errOut(true); // UNFIXED
	return 0;
	}

Dlist<Iarg> *parts = Fn::split_util(buf, chrs, nlppp->parse_);

if (parts)
	sem = new RFASem(parts);
return sem;
}


// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	RFASem *buf_sem,
	_TCHAR *chrs
	)
{
if (!buf_sem)
	return 0;
_TCHAR *buf = buf_sem->sem_to_str();
delete buf_sem;
return split(nlppp,buf,chrs);
}


// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	_TCHAR *buf,
	RFASem *chrs_sem
	)
{
if (!chrs_sem)
	return 0;
_TCHAR *chrs = chrs_sem->sem_to_str();
delete chrs_sem;
return split(nlppp,buf,chrs);
}


// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	RFASem *buf_sem,
	RFASem *chrs_sem
	)
{
if (!buf_sem && !chrs_sem)
	return 0;
if (!buf_sem)
	{
	delete chrs_sem;
	return 0;
	}
if (!chrs_sem)
	{
	delete buf_sem;
	return 0;
	}

_TCHAR *chrs = chrs_sem->sem_to_str();
delete chrs_sem;
_TCHAR *buf = buf_sem->sem_to_str();
delete buf_sem;
return split(nlppp,buf,chrs);
}


// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	long buf_num,
	long chrs_num
	)
{
return 0;
}

// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	long buf_num,
	_TCHAR *chrs
	)
{
return 0;
}

// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	long buf_num,
	RFASem *chrs_sem
	)
{
if (chrs_sem)
	delete chrs_sem;
return 0;
}

// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	_TCHAR *buf,
	long chrs_num
	)
{
return 0;
}

// VARIANT.
RFASem *Arun::split(
	Nlppp *nlppp,
	RFASem *buf_sem,
	long chrs_num
	)
{
if (buf_sem)
	delete buf_sem;
return 0;
}


/********************************************
* FN:		DEACCENT
* CR:		09/07/03 AM.
* SUBJ:	Convert string to replace accented chars.
* RET:	str - converted string.
* FORMS:	char *deaccent(str)
********************************************/

_TCHAR *Arun::deaccent(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::de_accent(str1, len, nlppp->htab());
return xstr;
}

// VARIANT
_TCHAR *Arun::deaccent(	// 09/07/03 AM.
	Nlppp *nlppp,
	RFASem *strsem
	)
{
if (!strsem)
	return 0;
_TCHAR *str1 = strsem->sem_to_str();
delete strsem;
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::de_accent(str1, len, nlppp->htab());
return xstr;
}

_TCHAR *Arun::deaccent(	// 09/07/03 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}


/********************************************
* FN:		DEJUNK
* CR:		09/09/11 AM.
* SUBJ:	Convert string to replace non-ASCII chars.
* RET:	str - converted string.
* FORMS:	char *dejunk(str)
********************************************/

_TCHAR *Arun::dejunk(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::de_junk(str1, len, nlppp->htab());
return xstr;
}

// VARIANT
_TCHAR *Arun::dejunk(	// 09/09/11 AM.
	Nlppp *nlppp,
	RFASem *strsem
	)
{
if (!strsem)
	return 0;
_TCHAR *str1 = strsem->sem_to_str();
delete strsem;
if (!str1 || !*str1)
	return 0;
long len = _tcsclen(str1);
_TCHAR *xstr = Xml::de_junk(str1, len, nlppp->htab());
return xstr;
}

_TCHAR *Arun::dejunk(	// 09/09/11 AM.
	Nlppp *nlppp,
	long num
	)
{
return 0;
}



/********************************************
* FN:   LEXTAGGER
* CR:   12/14/03 AM.
* SUBJ: Embed the LexTagger interface in NLP++.
* FORM: lextagger()
********************************************/

bool Arun::lextagger(
	Nlppp *nlppp
	)
{
//char *str1=0;
Parse *parse = nlppp->parse_;

#ifdef LINUX
	{
	_stprintf(Errbuf,_T("[lextagger: Unimplemented in LINUX.]"));
	parse->errOut(true); // UNFIXED
	return 0;
	}
#else
LexTagger::postagger(nlppp);
#endif

return true;
}


/********************************************
* FN:		SETLOOKAHEAD
* CR:		12/14/03 AM.
* SUBJ:	Dynamically set lookahead element.
* RET:	True if ok, else false.
* FORMS:	setlookahead(elt_num)
*			elt_num - Element num to set as lookahead locus.
* NOTE:	An ACTION, assumes a current rule match context.
********************************************/

bool Arun::setlookahead(
	Nlppp *nlppp,
	RFASem *num1_sem
	)
{
if (!num1_sem)
	return 0;
bool ok = false;
long num1 = num1_sem->sem_to_long(ok);
delete num1_sem;
if (!ok)
	return 0;
return setlookahead(nlppp,num1);
}


// RUNTIME VARIANT.
bool Arun::setlookahead(
	Nlppp *nlppp,
	long num1
	)
{
if (!nlppp)
	return false;

Parse *parse = nlppp->parse_;

#ifdef REGION_CK_COMPILED_
switch (nlppp->region_)
	{
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,
			_T("[setlookahead: Error. Must appear in @POST region.]"));
		return parse->errOut(true); // UNFIXED
	}
#endif

if (num1 <= 0)
	{
	_stprintf(Errbuf,_T("[setlookahead: Error. Given bad num.]"));
	return parse->errOut(true); // UNFIXED
	}

if (nlppp->red_)
	{
	_stprintf(Errbuf,_T("[setlookahead: Error. Rule already reduced.]"));
	return parse->errOut(true); // UNFIXED
	}


////////////////////////////////////////////////
// Find the collect elt for lookahead.
////////////////////////////////////////////////
// These numbers represent a range of rule elts in the collect.
// Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.

Node<Pn> *node1 = (Node<Pn> *)nlppp->coll_[num1].start;

if (node1)
	{
	nlppp->after_ = node1;
	nlppp->nlookahead_ = num1;		// FIX	// 09/06/08 AM.
	return true;
	}

// Look to the right.
while (++num1 <= nlppp->len_)
	{
	if ((node1 = (Node<Pn>*)nlppp->coll_[num1].start))
		{
		nlppp->after_ = node1;
		nlppp->nlookahead_ = num1;		// FIX	// 09/06/08 AM.
		return true;
		}
	}
return false;
}



/********************************************
* FN:		PERMUTEN
* CR:		03/10/04 AM.
* SUBJ:	Cleanly permute integers 0 to N-1.
* RET:	True if ok, else false.
*			UP - arr = array of ints 0 to N-1 in permuted order.
* FORMS:	sem = permuten(num)
*			Number of ints to permute.
********************************************/


RFASem *Arun::permuten(
	Nlppp *nlppp,
	RFASem *num1_sem
	)
{
if (!num1_sem)
	return 0;
bool ok = false;
long num1 = num1_sem->sem_to_long(ok);
delete num1_sem;
if (!ok)
	return 0;
return permuten(nlppp,num1);
}


// VARIANT.
RFASem *Arun::permuten(
	Nlppp *nlppp,
	long num1
	)
{
if (num1 <= 0)
	return 0;

if (num1 == 1)	// Trivial case.
	return new RFASem((long)0);

// Create buf of length n-1.
long *buf = new long[num1];

// Order elements 0, 1, .... n-1.
long ii = 0;
for (; ii < num1; ++ii)
	buf[ii] = ii;


// Traverse each element.
// Permute only those with arr[i] == i.
long rr = 0;
long tmp = 0;
for (ii = 0; ii < num1; ++ii)
	{
	if (buf[ii] == ii)
		{
		// Make a switch.
		rr = rand() % num1;
		if (rr == ii)	// Can't switch with current loc.
			{
			if (ii == 0)
				rr = 1;
			else
				--rr;
			}
		tmp = buf[ii];
		buf[ii] = buf[rr];
		buf[rr] = tmp;
		}
	}

// Convert to array type.
Dlist<Iarg> *list = new Dlist<Iarg>();		// Make empty list of args.
Iarg *arg = 0;

for (ii = 0; ii < num1; ++ii)
	{
	arg = new Iarg(buf[ii]);
	list->rpush(arg);
	}

// Delete allocated buf.
delete [] (long *) buf;

// Return an array of ints...
return new RFASem(list);
}

/********************************************
* FN:		LOGTEN
* CR:		04/29/04 AM.
* SUBJ:	Get base 10 logarithm of number.
* RET:	True if ok, else false.
*			UP - arr = array of ints 0 to N-1 in permuted order.
* FORMS:	sem = permuten(num)
*			Number of ints to permute.
********************************************/

float Arun::logten(
	Nlppp *nlppp,
	RFASem *num1_sem
	)
{
if (!num1_sem)
	return 0;
bool ok = false;
float num1 = num1_sem->sem_to_float(ok);
delete num1_sem;
if (!ok)
	return 0;
return logten(nlppp,num1);
}

// VARIANT.
float Arun::logten(
	Nlppp *nlppp,
	float num1
	)
{
return log10(num1);
}


/********************************************
* FN:		ELTNODE
* CR:		09/12/06 AM.
* SUBJ:	Get first node matching a rule element.
* RET:	Sem object for node, if any.
* FORMS:	lasteltnode(elt_num)
********************************************/

RFASem *Arun::eltnode(
	Nlppp *nlppp,
	long num1
	)
{
if (!nlppp || num1 < 0)
	return 0;

// Removed check for region here.

if (!nlppp->coll_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[eltnode: Warning. No collected nodes.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

Node<Pn> *node = (Node<Pn> *)nlppp->coll_[num1].start;
if (!node)
	return 0;
RFASem *sem = new RFASem(node);
return sem;
}

// VARIANT.
RFASem *Arun::eltnode(
	Nlppp *nlppp,
	RFASem *sem
	)
{
if (!sem)		// MEMLEAK.	// 05/15/08 AM.
	return 0;	// MEMLEAK.	// 05/15/08 AM.
if (!nlppp)
	{
	delete sem;		// MEMLEAK.	// 05/15/08 AM.
	return 0;
	}

// Removed check for region here.

if (sem->getType() != RSLONG)
	{
	delete sem;		// MEMLEAK.	// 05/15/08 AM.
	return 0;
	}
long num1 = sem->getLong();
if (num1 < 0)
	{
	delete sem;		// MEMLEAK.	// 05/15/08 AM.
	return 0;
	}

delete sem;		// MEMLEAK.	// 05/15/08 AM.

if (!nlppp->coll_)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[eltnode: Warning. No collected nodes.]") << ends;
	errOut(&gerrStr,false);
	return 0;
	}

Node<Pn> *node = (Node<Pn> *)nlppp->coll_[num1].start;
if (!node)
	return 0;
RFASem *sem1 = new RFASem(node);
return sem1;
}


/********************************************
* FN:	   LEVENSHTEIN
* CR:	   03/06/07 AM.
* SUBJ: String edit-distance metric.
* RET:  int - distance.
* FORMS:	levenshtein(str1,str2)
********************************************/

long Arun::levenshtein(
	Nlppp *nlppp,
	_TCHAR *name1,
	_TCHAR *name2
	)
{
return levenshtein_int(name1,name2);
}


long Arun::levenshtein(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return -1;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return levenshtein(nlppp,name1,name2);
}


long Arun::levenshtein(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return -1;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return levenshtein(nlppp,name1,name2);
}


long Arun::levenshtein(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return -1;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return -1;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return levenshtein(nlppp,name1,name2);
}

long Arun::levenshtein(
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return -1;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[levenshtein: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return -1;
	}
return -1;
}


long Arun::levenshtein(
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return -1;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[levenshtein: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return -1;
	}
return -1;
}



long Arun::levenshtein(
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[levenshtein: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return -1;
	}

return -1;
}


long Arun::levenshtein(
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
if (num != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[levenshtein: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return -1;
	}

return -1;
}

long Arun::levenshtein(
	Nlppp *nlppp,
	long num1,
	long num2
	)
{
if (num1 != 0 || num2 != 0)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[levenshtein: Error. Nonstring type.]") << ends;
	errOut(&gerrStr,false);
	return -1;
	}
return -1;
}

/********************************************
* FN:	   VTCOUT
* CR:		05/06/09 AM.
* SUBJ:	Return current binding to cout output stream, if any.
* RET:	True if ok, else false.
* FORMS:	vtcout()
* NOTE:	Creates a sem for the cout stream.
********************************************/

RFASem *Arun::vtcout(
	Nlppp *nlppp
	)
{
if (!nlppp)
	return 0;
Parse *parse = nlppp->parse_;
if (!parse)
	{
	_t_strstream gerrStr;	// 06/25/09 AM.
	gerrStr << _T("[vtcout: Error. No parse structure.]") << ends;	// 06/25/09 AM.
	errOut(&gerrStr,false);	// 06/25/09 AM.
	return 0;	// 06/25/09 AM.
	}

_t_ostream *x = parse->getCout();
if (!x)
	{
	_t_strstream gerrStr;	// 06/25/09 AM.
	gerrStr << _T("[vtcout: Error. No cout stream.]") << ends;	// 06/25/09 AM.
	errOut(&gerrStr,false);	// 06/25/09 AM.
	return 0;	// 06/25/09 AM.
	}
return new RFASem(x);
}





/********************************************
* FN:	   URLTOFILE
* CR:	   05/14/14 AM.
* SUBJ: Fetch given URL and place in given file.
* RET:  True if fetched page, else false.
* FORMS:	urltofile(url_str,file_str)
* NOTE:
********************************************/

_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	_TCHAR *url_s,
	_TCHAR *outfile_s
	)
{
if (!url_s || !*url_s || !outfile_s || !*outfile_s)
	return NULL;	// 09/26/19 AM.


bool ret = false;	// FAIL.	// 05/13/14 AM.
Parse *parse = nlppp->parse_;
#ifdef LINUX
	{
	_stprintf(Errbuf,_T("[urltofile: LINUX version unimplemented]"));
	parse->errOut(false); // UNFIXED
	return NULL;	// 09/26/19 AM.
	}
#else

	//FetchHtmlSource(url_s, outfile_s);

#ifdef WEBF_
	WebForm wf;	// 05/06/14 DDH.
	string location;
	wf.setHost((const char *)url_s);	// 05/06/14 DDH.
	location = wf.sendRequest();	// 05/06/14 DDH.
	if (location == "") {
 		location = url_s;
	}
	if (wf.getFile((char *)outfile_s))	// 05/06/14 DDH.
		ret = true;	// 05/06/14 DDH.	// SUCCEED.	// 05/13/14 AM.
	else							// 05/19/14 DDH.
//		location = "DONE";	// 05/19/14 DDH.
		return 0; // FAIL.				// 05/20/14 AM.
#endif


	//if (tear_url(url_s,outfile_s))	// 03/12/03 AM.
	//	ret = 0;								// 03/12/03 AM.
//#endif

#ifdef WEBF_
	vector<TCHAR> buffer(location.begin(), location.end());
	buffer.push_back(_T('\0'));
	TCHAR* p = &buffer[0];

	_TCHAR *ilocation = 0;
	parse->internStr(p, /*UP*/ ilocation);

	return ilocation;	// 05/20/14 AM.
#endif

//if (tear_url(url_s,outfile_s))	// 03/12/03 AM.
//	ret = 0;								// 03/12/03 AM.
#endif

//return ret;
return 0;	// 05/20/14 AM.
}


_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	RFASem *name1_sem,
	_TCHAR *name2
	)
{
if (!name1_sem)
	return NULL;	// 09/26/19 AM.
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return urltofile(nlppp,name1,name2);
}


_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	_TCHAR *name1,
	RFASem *name2_sem
	)
{
if (!name2_sem)
	return NULL;	// 09/26/19 AM.
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return urltofile(nlppp,name1,name2);
}


_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	RFASem *name1_sem,
	RFASem *name2_sem
	)
{
if (!name1_sem)
	{
	if (name2_sem)
		delete name2_sem;
	return 0;
	}
if (!name2_sem)
	{
	delete name1_sem;
	return 0;
	}
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
_TCHAR *name2 = name2_sem->sem_to_str();
delete name2_sem;
return urltofile(nlppp,name1,name2);
}


_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	RFASem *name1_sem,
	long num
	)
{
if (!name1_sem)
	return 0;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return 0;
}


_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	long num,
	RFASem *name1_sem
	)
{
if (!name1_sem)
	return 0;
_TCHAR *name1 = name1_sem->sem_to_str();
delete name1_sem;
return 0;
}



_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	long num,
	_TCHAR *name2
	)
{
return 0;
}


_TCHAR *Arun::urltofile(
	Nlppp *nlppp,
	_TCHAR *name2,
	long num
	)
{
return 0;
}



/********************************************
* FN:		URLBASE
* CR:		05/20/14 AM.
* SUBJ:	Clean given URL.
* RET:	str - reworked url.
* FORMS:	char *urlbase(str)
* NOTE:	Old, superseded by resolveurl, here for
*			completeness.
********************************************/

_TCHAR *Arun::urlbase(
	Nlppp *nlppp,
	_TCHAR *str1
	)
{
if (!str1 || !*str1)
	return 0;

Parse *parse = nlppp->parse_;

_TCHAR *str = Chars::create(_tcsclen(str1) + 1);
if (!url_base(str1, /*UP*/ str))
	{
	Chars::destroy(str);
	return 0;
	}

_TCHAR *istr = 0;
parse->internStr(str, /*UP*/ istr);
Chars::destroy(str);

return istr;

}

// VARIANT
_TCHAR *Arun::urlbase(
	Nlppp *nlppp,
	RFASem *strsem
	)
{
if (!strsem)
	return 0;
_TCHAR *str1 = strsem->sem_to_str();
delete strsem;
if (!str1 || !*str1)
	return 0;
return urlbase(nlppp,str1);
}

_TCHAR *Arun::urlbase(
	Nlppp *nlppp,
	long num
	)
{
return 0;
}



/********************************************
* FN:		DICTPHRASELOOKUP
* CR:		12/16/14 AM.
* SUBJ:	Dictionary-based phrase lookup.
* RET:	ok - true if ok.
* FORMS:	bool DICTphraselookup(node,key_str,match_str,list_str,skippunct_num);
* NOTE:
********************************************/


bool Arun::DICTphraselookup(
	Nlppp*nlppp,
	NODE*pn_node,     // n
	_TCHAR*key_str,   // k
	_TCHAR*match_str, // m
	_TCHAR*list_str,  // a
	long punct_num    // p
	)
{
if (!nlppp || !pn_node || !key_str || !match_str || !list_str)
	return false;

Parse *parse = nlppp->getParse();


// Get pnode text
_TCHAR buf[512];
long siz=512;
buf[0] = '\0';
_TCHAR *ptr = buf;
Fn::prosify((Node<Pn> *)pn_node,true,buf,ptr,siz);

// strtolower // Trying IN-PLACE conversion.
str_to_lower(buf);

// OMITTING INTERN (if possible)
_TCHAR *str;
//parse->internStr(buf, /*UP*/ str);
str = buf;


// dictfindword
CG *cg = parse->getAna()->getCG();	// Get current KB.

CONCEPT *wc = cg->findWordConcept(str);
if (!wc) return true;	// No matches; done.

// conval
// key_str
CONCEPT *cc = 0;
if (!cg->findVal(wc,key_str,/*UP*/ cc))
	return true;	// No phrase start concept; done.

// Now we are at first word's concept in phrase hierarchy.
long len = 1;	// Length of matched phrase.


Htab *ht = parse->getHtab();	// Current hash table for strings.

// TODO: Record info if first word is a full match.
Fn::DICTphraselookup_match((Node<Pn> *)pn_node,(Node<Pn> *)pn_node,cc,match_str,list_str,cg,ht);

// Move to next pnode.
Node<Pn> *x  = ((Node<Pn> *)pn_node)->Right();

// CRASH. // 01/14/15 AM.
if (!x) return true;

// Tandem traverse current parse tree nodes and down the kb hierarchy path.
// Record nodes that match.
CONCEPT *cx = 0;
bool done = false;
while (!done)
	{
	// Try to match.
	ptr = buf;
	Fn::prosify(x,true,buf,ptr,siz);
	str_to_upper(buf);	// Phrase hierarchy assumed all uppercase.
	if (_istspace(*buf))	// Whitepace.
		;	// Move to next.
	else if (_istpunct(*buf) && punct_num)
		; // Skip punctuation too.
	else if ((cx = cg->findConcept(cc, buf)))	// MATCHED CHILD
		{
		cc = cx;
		// TODO: RECORD MATCH.
		// If end of concept path, done.
		Fn::DICTphraselookup_match((Node<Pn> *)pn_node,x,cc,match_str,list_str,cg,ht);

		if (!cg->Down(cc)) done = true;
		}
	else
		done = true;

	if (!(x = x->Right()) ) done = true;
	}


return true;
}

/////////////////

bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,_TCHAR*m,_TCHAR*a,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,_TCHAR*m,_TCHAR*a,long p)
{
_TCHAR *k = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,RFASem*ms,_TCHAR*a,long p)
{
_TCHAR *m = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,_TCHAR*m,RFASem*as,long p)
{
_TCHAR *a = 0;

if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,_TCHAR*m,_TCHAR*a,RFASem*ps)
{
bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}


// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,_TCHAR*m,_TCHAR*a,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,RFASem*ms,_TCHAR*a,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *m = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,_TCHAR*m,RFASem*as,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *a = 0;

if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,_TCHAR*m,_TCHAR*a,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,RFASem*ms,_TCHAR*a,long p)
{
_TCHAR *k = 0;
_TCHAR *m = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,_TCHAR*m,RFASem*as,long p)
{
_TCHAR *k = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,_TCHAR*m,_TCHAR*a,RFASem*ps)
{
_TCHAR *k = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,RFASem*ms,RFASem*as,long p)
{
_TCHAR *m = 0;
_TCHAR *a = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,RFASem*ms,_TCHAR*a,RFASem*ps)
{
_TCHAR *m = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,_TCHAR*m,RFASem*as,RFASem*ps)
{
_TCHAR *a = 0;

if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}


// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,RFASem*ms,_TCHAR*a,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;
_TCHAR *m = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,_TCHAR*m,RFASem*as,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,_TCHAR*m,_TCHAR*a,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,RFASem*ms,RFASem*as,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *m = 0;
_TCHAR *a = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,RFASem*ms,_TCHAR*a,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *m = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,_TCHAR*m,RFASem*as,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *a = 0;

if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,RFASem*ms,RFASem*as,long p)
{
_TCHAR *k = 0;
_TCHAR *m = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,RFASem*ms,_TCHAR*a,RFASem*ps)
{
_TCHAR *k = 0;
_TCHAR *m = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,_TCHAR*m,RFASem*as,RFASem*ps)
{
_TCHAR *k = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,_TCHAR*k,RFASem*ms,RFASem*as,RFASem*ps)
{
_TCHAR *m = 0;
_TCHAR *a = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}


// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,NODE*n,RFASem*ks,RFASem*ms,RFASem*as,RFASem*ps)
{
_TCHAR *k = 0;
_TCHAR *m = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,_TCHAR*k,RFASem*ms,RFASem*as,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *m = 0;
_TCHAR *a = 0;

if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,_TCHAR*m,RFASem*as,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,RFASem*ms,_TCHAR*a,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;
_TCHAR *m = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,RFASem*ms,RFASem*as,long p)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;
_TCHAR *m = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

// VARIANT
bool Arun::DICTphraselookup(Nlppp*nlppp,RFASem*ns,RFASem*ks,RFASem*ms,RFASem*as,RFASem*ps)
{
NODE *n = 0;
if (ns) { n = ns->sem_to_node(); delete ns; }

_TCHAR *k = 0;
_TCHAR *m = 0;
_TCHAR *a = 0;

if (ks) { k = ks->sem_to_str(); delete ks; }
if (ms) { m = ms->sem_to_str(); delete ms; }
if (as) { a = as->sem_to_str(); delete as; }

bool ok = false;
long p = 0;
if (ps) { p = ps->sem_to_long(ok); delete ps; }

return DICTphraselookup(nlppp,n,k,m,a,p);
}

//////////////////////////
