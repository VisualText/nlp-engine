/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XSTR.CPP
* FILE:	cs/libqkbm/xstr.cpp
* CR:		02/07/07 AM.
* SUBJ:	Database for mapping index to its string.
* NOTE:	Can't see a way around having two databases, at double the space:
*				string	index   (this one is STRX)
*	and
*				index    string  (this one is XSTR)
*
*******************************************************************************/

#include "StdAfx.h"
#include "depot.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "depot.h"
#include "curia.h"
#include "mgr.h"
#include "util.h"

#include "xstr.h"

int XSTR::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

XSTR::XSTR(
	)
{

db_ = 0;

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

XSTR::~XSTR()
{

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/



///////////
/********************************************
* FN:		OPENDB
* CR:		02/07/07 AM.
* SUBJ:	Open xstr database.
* RET:	True if ok, else false.
********************************************/

bool XSTR::opendb(_TCHAR *dbname)
{
if(db_ = cropen(dbname, CR_OWRITER | CR_OCREAT, -1, -1))
	return true;

//    fprintf(stderr, "cropen: %s\n", crerrmsg(crecode));
return false;
}

/********************************************
* FN:		CLOSEDB
* CR:		02/07/07 AM.
* SUBJ:	Close manager database.
* RET:	True if ok, else false.
********************************************/

bool XSTR::closedb()
{
if (crclose(db_))
	return true;

// fprintf(stderr, "dpclose: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		ADDXstr
* CR:		02/07/07 AM.
* SUBJ:	Add string to xstr db.
* RET:	True if ok, else false.
* Nasty:	PRETEND UNICODE STRING IS JUST CHAR *.
********************************************/

bool XSTR::addXstr(STR_ID val, _TCHAR *str)
{
int *x;	// Pretend number is a string.
_TCHAR sval[5];	// Buffer for converted number.
x = (int *) sval;	// Nasty.
*x = val;			// Nasty.
sval[4] = '\0';	// Terminate the "string".

// Adding size of key.	// FIX.	// 03/01/07 AM.
if (crput(db_, sval, sizeof(int), str, -1, CR_DOVER))
	return true;
return false;
}

/********************************************
* FN:		GETXSTR
* CR:		02/07/07 AM.
* SUBJ:	Get index's string.
* RET:	str - String value of index.
* WARN:	User must FREE the string when done with it.
*			For UNICODE, may need to convert char strings coming from the db.
********************************************/

_TCHAR *XSTR::getXstr(STR_ID index)
{
if (index <= 0)
	return 0;

int *x;	// Pretend number is a string.
_TCHAR sidx[5];	// Buffer for converted number.

x = (int *) sidx;	// Nasty.
*x = index;			// Nasty.
sidx[4] = '\0';	// Terminate the "string".

char *val = 0;
int siz = 0;
if (val = crget(db_, sidx, sizeof(int), 0, -1, &siz))
	{
//	dpfree(val);
	return (_TCHAR *) val;	// Nasty: CAST CHAR * AS UNICODE STRING.
	}
return 0;
}
