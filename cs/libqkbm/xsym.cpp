/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XSYM.CPP
* FILE:	cs/libqkbm/xsym.cpp
* CR:		02/18/07 AM.
* SUBJ:	Database for mapping string index to a concept.
* NOTE:	This table can serve as an analog to the old sym table.
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

#include "xsym.h"

int XSYM::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

XSYM::XSYM(
	)
{

db_ = 0;

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

XSYM::~XSYM()
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
* CR:		02/18/07 AM.
* SUBJ:	Open xstr database.
* RET:	True if ok, else false.
********************************************/

bool XSYM::opendb(_TCHAR *dbname)
{
if(db_ = cropen(dbname, CR_OWRITER | CR_OCREAT, -1, -1))
	return true;

//    fprintf(stderr, "cropen: %s\n", crerrmsg(crecode));
return false;
}

/********************************************
* FN:		CLOSEDB
* CR:		02/18/07 AM.
* SUBJ:	Close manager database.
* RET:	True if ok, else false.
********************************************/

bool XSYM::closedb()
{
if (crclose(db_))
	return true;

// fprintf(stderr, "dpclose: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		ADDXSYM
* CR:		02/18/07 AM.
* SUBJ:	Add string to xstr db.
* RET:	True if ok, else false.
********************************************/

bool XSYM::addXsym(STR_ID sid, CON_ID cid)
{
if (sid <= 0 || cid <=0)
	return false;

int arr[2]; char *x; char *id_s;	// Nasty.
arr[0] = sid; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;

int carr[2]; char *cx; char *id_c;	// Nasty.
carr[0] = cid;		// FIX.	// 03/07/07 AM.
cx = (char *) &(carr[1]); *cx = '\0'; id_c = (char *) carr;

// Adding size of key and val.	// FIX.	// 03/01/07 AM.
if (crput(db_, id_s, sizeof(int), id_c, sizeof(int), CR_DOVER))
	return true;
return false;
}

/********************************************
* FN:		GETXSYM
* CR:		02/18/07 AM.
* SUBJ:	Get index's concept, if present.
* RET:	str - String value of index.
********************************************/

CON_ID XSYM::getXsym(STR_ID index)
{
if (index < 0)
	return 0;

int arr[2]; char *x; char *id_s;	// Nasty.
arr[0] = index; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;

/*
int *x;	// Pretend number is a string.
_TCHAR sidx[5];	// Buffer for converted number.

x = (int *) sidx;	// Nasty.
*x = index;			// Nasty.
sidx[4] = '\0';	// Terminate the "string".
*/

int *xx;	// FIX.	// 03/08/07 AM.
char *val;
int ival;	// FIX.	// 03/08/07 AM.
int siz;
if (val = crget(db_, id_s, sizeof(int), 0, sizeof(int), &siz))
	{
	xx = (int *) val;	// Nasty.							// FIX.	// 03/08/07 AM.
	ival = *xx;			// Nasty un-serialize code.	// FIX.	// 03/08/07 AM.
	dpfree(val);	// FIX.	// 03/08/07 AM.
	return ival;	// FIX.	// 03/08/07 AM.
	}
return 0;
}





/********************************************
* FN:		RMXSYM
* CR:		02/20/07 AM.
* SUBJ:	Remove concept from db.
* RET:	strid - id into string db.
********************************************/

bool XSYM::rmXsym(char *idx_s)
{
//int siz = sizeof(int);		// FIX.	// 03/07/07 AM.
return crout(db_, idx_s, sizeof(int)) ? true : false;
}
