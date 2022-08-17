/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	UTIL.CPP
* FILE:	cs/libqkbm/mgr.cpp
* CR:		02/05/07 AM.
* SUBJ:	A tiny database used for keeping counts and stats for the other databases.
*			An overall "manager" database.  Could be implemented as a flat file.
*
*******************************************************************************/

#include "StdAfx.h"
#include "depot.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "qkbm/libqkbm.h"
#include "depot.h"
#include "curia.h"
#include "util.h"

int UTIL::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

UTIL::UTIL(
	)
{

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

UTIL::~UTIL()
{

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/



///////////

/********************************************
* FN:		SELECTINT
* CR:		02/05/07 AM.
* SUBJ:	Get int-value from database.
* NOTE:	"un-serializing" trick/nasty code, not using _ttoi.
********************************************/

int UTIL::selectInt(
	DEPOT *db,
	_TCHAR *field
	)
{
int *x;
char *val = 0;
int ival = 0;
if (val = dpget(db, field, -1, 0, sizeof(int), NULL))
	{
	x = (int *) val;	// Nasty.
	ival = *x;	// Nasty un-serialize code.
	dpfree(val);
	return ival;
	}

//    fprintf(stderr, "dpget: %s\n", dperrmsg(dpecode));
return 0;
}



/********************************************
* FN:		UPDATEINT
* CR:		02/05/07 AM.
* SUBJ:	Store int-value in database.
* NOTE:	Storing number as a set of bytes. "serializing" trick/nasty code.
*			Not bothering to convert ascii to int. with _itot.
********************************************/

bool UTIL::updateInt(
	DEPOT *db,
	_TCHAR *field,
	int val
	)
{
int *x;	// Pretend number is a string.
_TCHAR sval[5];	// Buffer for converted number.

x = (int *) sval;	// Nasty.
*x = val;			// Nasty.
sval[4] = '\0';	// Terminate the "string".
//cout << sizeof(int) << endl;
//_itot(val, sval, 10);	// Convert, base 10.

if (dpput(db, field, -1, sval, sizeof(int), DP_DOVER))
	return true;

//    fprintf(stderr, "dpget: %s\n", dperrmsg(dpecode));
return false;
}

