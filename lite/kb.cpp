/*******************************************************************************
Copyright (c) 2007-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	KB.CPP
* FILE:	lite\kb.cpp
* CR:		02/12/07 AM.
* SUBJ:	Class for interfacing to KBMS.
* NOTE:	Implementing variant KBMS based on file storage, using QDBM freeware.
*			Want single version of LITE library to handle both kbms variants, as
*			far as possible.  Using QDBM_ define only as necessary, with goal of
*			not using that at all.  Strings (interning, copying, and freeing) are
*			the biggest issue of the moment.
*			Want to assume that all kbms variants may handle in-memory as well as
*			on-disk storage of kb objects, and interface to kbms should deal with
*			"handles" for objects rather than explicit pointers, ultimately.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/global.h"
#include "htab.h"

#include "consh/libconsh.h"
#include "consh/cg.h"

#include "kb.h"


/********************************************
* FN:		Special functions for the class
********************************************/

KB::KB()					// Default constructor.
{
}

/*******************************************/

KB::~KB()
{
}


/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

/********************************************
* FN:		Modify Functions
********************************************/


/********************************************
* FN:		Class-Wide Functions
********************************************/


/********************************************
* FN:		strVal
* CR:		02/12/07 AM.
* SUBJ:	Get string-valued attr from kbms.
* NOTE:	This variant interns string in a given hash table.
********************************************/

_TCHAR *KB::strVal(CONCEPT *con,_TCHAR *attr,CG *cg,Htab *ht)
{
_TCHAR *val = cg->strVal(con, attr);
if (!val || !*val)
	return 0;

#ifdef QDBM_
if (!ht)
	return 0;	// No hash table for interning.
_TCHAR *ival = ht->getStr(val);
cg->kbfree(val);	// Free string that came from kbms.
return ival;
#else
return val;
#endif
}


/*************************      END OF FILE      ******************************/
