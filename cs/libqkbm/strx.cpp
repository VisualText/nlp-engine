/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STRX.CPP
* FILE:	cs/libqkbm/strx.cpp
* CR:		02/06/07 AM.
* SUBJ:	Database for strings and info about them.
* NOTE:	Can't see a way around having two databases, at double the space:
*				string	index   (this one is STRX)
*	and
*				index    string  (this one is XSTR)
*
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

#include "strx.h"

int STRX::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

STRX::STRX(
	)
{

db_ = 0;

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

STRX::~STRX()
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
* CR:		02/06/07 AM.
* SUBJ:	Open strx database.
* RET:	True if ok, else false.
********************************************/

bool STRX::opendb(_TCHAR *dbname)
{
if(db_ = cropen(dbname, CR_OWRITER | CR_OCREAT, -1, -1))
	return true;

//    fprintf(stderr, "cropen: %s\n", crerrmsg(crecode));
return false;
}

/********************************************
* FN:		CLOSEDB
* CR:		02/06/07 AM.
* SUBJ:	Close manager database.
* RET:	True if ok, else false.
********************************************/

bool STRX::closedb()
{
if (crclose(db_))
	return true;

// fprintf(stderr, "dpclose: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		ADDSTRING
* CR:		02/06/07 AM.
* SUBJ:	Add string to strx db.
* RET:	True if ok, else false.
********************************************/

bool STRX::addString(_TCHAR *str, STR_ID val)
{
int *x;	// Pretend number is a string.
_TCHAR sval[5];	// Buffer for converted number.
x = (int *) sval;	// Nasty.
*x = val;			// Nasty.
sval[4] = '\0';	// Terminate the "string".

// FIX.	02/27/07 AM. Seem to need the size, to assure alignment etc.
int siz = sizeof(int);

if (crput(db_, str, -1, sval, siz, CR_DOVER))
	return true;
return false;
}

/********************************************
* FN:		GETSTRX
* CR:		02/06/07 AM.
* SUBJ:	Get string's index.
* RET:	int - Index value.
********************************************/

STR_ID STRX::getStrx(_TCHAR *str)
{
if (!str)		// NULL FIRST CHAR OK.	// FIX.	// 03/01/07 AM.
	return 0;

int *x;
int ival;

#ifdef OLD_032307_
char *val;
int siz = 0;
if (val = crget(db_, str, -1, 0, sizeof(int), &siz))
	{
	x = (int *) val;	// Nasty.
	ival = *x;	// Nasty un-serialize code.
	dpfree(val);
	return ival;
	}
#endif

char vbuf[sizeof(int)];	// 03/23/07 AM.
if (0 < crgetwb(db_, str, -1, 0, sizeof(int), vbuf))	//03/23/07 AM.
	{
	x = (int *) vbuf;	// Nasty.	// 03/23/07 AM.
	ival = *x;			// Nasty.	// 03/23/07 AM.
	return ival;						// 03/23/07 AM.
	}

//    fprintf(stderr, "dpget: %s\n", dperrmsg(dpecode));
return 0;
}
