/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MGR.CPP
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
#include "qkbm/defs.h"
#include "depot.h"
#include "curia.h"
#include "util.h"

#include "mgr.h"

int MGR::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

MGR::MGR(
	)
{

db_ = 0;

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

MGR::~MGR()
{

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/

STR_ID MGR::getStrings()	{return strings_;}
CON_ID MGR::getConcepts()	{return concepts_;}
PTR_ID MGR::getPointers()	{return pointers_;}


void MGR::setStrings(STR_ID x)	{strings_ = x;}
void MGR::setConcepts(CON_ID x)	{concepts_ = x;}
void MGR::setPointers(PTR_ID x)	{pointers_ = x;}


///////////

/********************************************
* FN:		ISDB
* CR:		03/04/07 AM.
* SUBJ:	Check if manager db exists.
* RET:	True if exists, else false.
********************************************/

bool MGR::isdb(_TCHAR *dbname)
{
db_ = dpopen(dbname, DP_OREADER, -1);
bool exists = (db_ ? true : false);
if (exists)
	dpclose(db_);
else
	{
	fprintf(stderr, "dpopen: %s\n", dperrmsg(dpecode));
	}
return exists;
}

/********************************************
* FN:		OPENDB
* CR:		02/05/07 AM.
* SUBJ:	Open manager database.
* RET:	True if ok, else false.
********************************************/

bool MGR::opendb(_TCHAR *dbname)
{
if(db_ = dpopen(dbname, DP_OWRITER | DP_OCREAT, -1))
	return true;

//    fprintf(stderr, "dpopen: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		CLOSEDB
* CR:		02/05/07 AM.
* SUBJ:	Close manager database.
* RET:	True if ok, else false.
********************************************/

bool MGR::closedb()
{
if (dpclose(db_))
	return true;

// fprintf(stderr, "dpclose: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		SELECTSTRINGS
* CR:		02/05/07 AM.
* SUBJ:	Get strings counter from db.
********************************************/

STR_ID MGR::selectStrings()
{
return UTIL::selectInt(db_,_T("strings"));
}

/********************************************
* FN:		SELECTCONCEPTS
* CR:		02/05/07 AM.
* SUBJ:	Get concepts counter from db.
********************************************/

CON_ID MGR::selectConcepts()
{
return UTIL::selectInt(db_,_T("concepts"));
}

/********************************************
* FN:		SELECTPOINTERS
* CR:		02/05/07 AM.
* SUBJ:	Get pointers counter from db.
********************************************/

PTR_ID MGR::selectPointers()
{
return UTIL::selectInt(db_,_T("pointers"));
}


/********************************************
* FN:		UPDATESTRINGS
* CR:		02/06/07 AM.
* SUBJ:	Update strings counter to db.
********************************************/

PTR_ID MGR::updateStrings()
{
UTIL::updateInt(db_,_T("strings"), strings_);
return strings_;
}

/********************************************
* FN:		UPDATECONCEPTS
* CR:		02/06/07 AM.
* SUBJ:	Update concepts counter to db.
********************************************/

CON_ID MGR::updateConcepts()
{
UTIL::updateInt(db_,_T("concepts"),concepts_);
return concepts_;
}

/********************************************
* FN:		UPDATEPOINTERS
* CR:		02/06/07 AM.
* SUBJ:	Update pointers counter from db.
********************************************/

PTR_ID MGR::updatePointers()
{
UTIL::updateInt(db_,_T("pointers"),pointers_);
return pointers_;
}



/********************************************
* FN:		INCSTRINGS
* CR:		02/06/07 AM.
* SUBJ:	Increment strings counter to db.
********************************************/

STR_ID MGR::incStrings()
{
UTIL::updateInt(db_,_T("strings"), ++strings_);
return strings_;
}

/********************************************
* FN:		INCCONCEPTS
* CR:		02/06/07 AM.
* SUBJ:	Increment concepts counter to db.
********************************************/

CON_ID MGR::incConcepts()
{
UTIL::updateInt(db_,_T("concepts"),++concepts_);
return concepts_;
}

/********************************************
* FN:		INCPOINTERS
* CR:		02/06/07 AM.
* SUBJ:	Increment pointers counter from db.
********************************************/

PTR_ID MGR::incPointers()
{
UTIL::updateInt(db_,_T("pointers"),++pointers_);
return pointers_;
}


/********************************************
* FN:		RESETDB
* CR:		02/06/07 AM.
* SUBJ:	Reset the database counters.
* RET:	True if ok, else false.
* NOTE:	This is a quick way to "empty" a database,
*	given that new values will override the old ones.
*	One could check that fetches yield ids smaller than the tracking
*	numbers.
********************************************/

bool MGR::resetdb()
{
setStrings(0);
setConcepts(0);
setPointers(0);
updateStrings();
updateConcepts();
updatePointers();
return true;
}


/********************************************
* FN:		INITDB
* CR:		03/04/07 AM.
* SUBJ:	Initialize an existing db that was just loaded.
* RET:	True if ok, else false.
********************************************/

bool MGR::initdb()
{
strings_	 = selectStrings();
concepts_ = selectConcepts();
pointers_ = selectPointers();
return true;
}

//{
//  DEPOT *depot;
//  char *val;

  /* store the record */
//  if(!dpput(depot, NAME, -1, NUMBER, -1, DP_DOVER)){
//    fprintf(stderr, "dpput: %s\n", dperrmsg(dpecode));
//  }

  /* retrieve the record */
//  if(!(val = dpget(depot, NAME, -1, 0, -1, NULL))){
//    fprintf(stderr, "dpget: %s\n", dperrmsg(dpecode));
//  } else {
//    printf("Name: %s\n", NAME);
//    printf("Number: %s\n", val);
//    dpfree(val);
//  }


//  return 0;
//}
