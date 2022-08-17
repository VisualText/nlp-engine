/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XCON.CPP
* FILE:	cs/libqkbm/xcon.cpp
* CR:		02/07/07 AM.
* SUBJ:	Database for concept data structure.
* NOTE:	
*
*******************************************************************************/

#include "StdAfx.h"
#include "depot.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "depot.h"
#include "curia.h"

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xcon_s.h"

#include "mgr.h"
#include "util.h"

#include "xcon.h"

int XCON::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

XCON::XCON(
	)
{

db_ = 0;

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

XCON::~XCON()
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
* SUBJ:	Open xcon database.
* RET:	True if ok, else false.
********************************************/

bool XCON::opendb(_TCHAR *dbname)
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

bool XCON::closedb()
{
if (crclose(db_))
	return true;

// fprintf(stderr, "dpclose: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		ADDXCON
* CR:		02/07/07 AM.
* SUBJ:	Add xcon struct to  db.
* RET:	Index of added xcon in db.
* Nasty:	Concatenate values into a char array.
* NOTE:	Variants of this will add different kinds of values.
* WARN:	Need to specify SIZE of string, else null chars lead to premature
*	truncation of the string.
********************************************/

CON_ID XCON::addXcon(
	char *idx,
	CON_ID id,	// 02/14/07 AM.
	int val,
	XCKIND kind,
	int flags,
	PTR_ID attrs,
	CON_ID up,
	CON_ID dn,
	CON_ID prev,
	CON_ID next,
	CON_ID atom,
	PTR_ID phrases
	)
{
//cout << "addXcon conid=" << id << endl;

// Nasty: Make an array of two objects.  Fill the first.
// Then cast it as char array and set one char past the first object to string null.
XCON_S arr[2];
arr[0].id = id;	// 02/14/07 AM.
arr[0].kind = kind;
arr[0].flags = flags;
arr[0].attrs = attrs;
arr[0].up = up;
arr[0].dn = dn;
arr[0].prev = prev;
arr[0].next = next;
arr[0].atom = atom;
arr[0].phrases = phrases;

//cout << "arr0id=" << arr[0].id << endl;

switch (kind)
	{
	case cXBASIC:
		arr[0].v.vname = val;
		break;
	case cXWORD:
		arr[0].v.vsym = val;
		break;
	case cXPROXY:
		arr[0].v.vcon = val;
		break;
	default:
		return 0;	// Error: Given wrong kind.
	}


char *tmp = (char *) arr;	// Nasty: treat array as chars.
int siz = sizeof(XCON_S);

/*
cout << "con siz=" << sizeof(XCON_S) << endl;

cout << "con arr="
	<< tmp[0] << ","
	<< tmp[1] << ","
	<< tmp[2] << ","
	<< tmp[3] << ","
	<< tmp[4] << ","
	<< tmp[5] << ","
	<< tmp[6] << ","
	<< tmp[7] << ","
	<< tmp[8] << ","
	<< tmp[9] << ","
	<< tmp[10] << ","
	<< tmp[11] << ","
	<< tmp[12]
	<< endl;
*/

// Nasty: Zero out first byte of second struct.
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty: Zero out first byte of second struct.

/*
cout << "con arr="
	<< tmp[0] << ","
	<< tmp[1] << ","
	<< tmp[2] << ","
	<< tmp[3] << ","
	<< tmp[4] << ","
	<< tmp[5] << ","
	<< tmp[6] << ","
	<< tmp[7] << ","
	<< tmp[8] << ","
	<< tmp[9] << ","
	<< tmp[10] << ","
	<< tmp[11] << ","
	<< tmp[12]
	<< endl;
*/

// Adding size of key.	// FIX.	// 03/01/07 AM.
if (crput(db_, idx, sizeof(int), tmp, siz, CR_DOVER))
	return true;
return false;
}


/********************************************
* FN:		UPDATEXCON
* CR:		02/13/07 AM.
* SUBJ:	Update/replace xcon struct to  db.
* RET:	Index of added xcon in db.
* Nasty:	Concatenate values into a char array.
* NOTE:	Variants of this will add different kinds of values.
* WARN:	Need to specify SIZE of string, else null chars lead to premature
*	truncation of the string.
********************************************/

bool XCON::updateXcon(char *id_s, XCON_S *con)
{
// WARN: FIRST CHAR CAN BE 0 or NULL!!!	// FIX.	// 03/01/07 AM.
if (!id_s || !con)
	return false;

int siz = sizeof(XCON_S);

XCON_S arr[2];

// Copy into a char array, terminate and update.  Nasty code.
// 02/27/07 AM. strncpy doesn't seem to work right.
// So copy fields one at a time.
// (Maybe an unsigned char issue.)

arr[0].id = con->id;
arr[0].kind = con->kind;
arr[0].flags = con->flags;
arr[0].attrs = con->attrs;
arr[0].up = con->up;
arr[0].dn = con->dn;
arr[0].prev = con->prev;
arr[0].next = con->next;
arr[0].atom = con->atom;
arr[0].phrases = con->phrases;
arr[0].v = con->v;

char *tmp = (char *) arr;	// Nasty: treat array as chars.
//strncpy(tmp,tmpy,siz);
tmp[siz] = '\0';	// Terminate string.

cout << "con arr after copy="
	<< tmp[0] << ","
	<< tmp[1] << ","
	<< tmp[2] << ","
	<< tmp[3] << ","
	<< tmp[4] << ","
	<< tmp[5] << ","
	<< tmp[6] << ","
	<< tmp[7] << ","
	<< tmp[8] << ","
	<< tmp[9] << ","
	<< tmp[10] << ","
	<< tmp[11] << ","
	<< tmp[12]
	<< endl;

// Adding size of key.	// FIX.	// 03/01/07 AM.
return crput(db_, id_s, sizeof(int), tmp, siz, CR_DOVER) ? true : false;
}

/********************************************
* FN:		GETXCON
* CR:		02/07/07 AM.
* SUBJ:	Get ptr from kbms, given index.
* RET:	DU ptr - ptr struct filled from kbms.
*			bool - true if ok.
* WARN:	USER MUST FREE THE RETURNED CON OBJECT.
********************************************/

bool XCON::getXcon(char *idx_s, /*DU*/ XCON_S *&con)
{
con = 0;
//XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

int siz = 0;
char *val = 0;
// Adding size of key.	// FIX.	// 03/01/07 AM.
if (val = crget(db_, idx_s, sizeof(int), 0,
										sizeof(XCON_S), &siz))	// 03/23/07 AM.
	{
	// RETURN STRUCT FROM KB.	// 02/26/07 AM.
	con = (XCON_S *) val;
	return true;
	}

return false;
}

/********************************************
* FN:		RMXCON
* CR:		02/12/07 AM.
* SUBJ:	Remove concept from db.
* RET:	strid - id into string db.
********************************************/

bool XCON::rmXcon(char *idx_s)
{
//int siz = sizeof(XCON_S);
return crout(db_, idx_s, sizeof(int)) ? true : false;
}

/********************************************
* FN:		GET_NAME
* CR:		02/12/07 AM.
* SUBJ:	Get concept's name.
* RET:	strid - id into string db.
********************************************/

STR_ID XCON::get_name(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.
STR_ID sid = 0;

//int siz = 0;
//char *val = 0;
CON_ID owner;
XCON_S *oc;
char vbuf[sizeof(XCON_S)];	// 03/23/07 AM.

// Adding size of key.	// FIX.	// 03/01/07 AM.
//if (val = crget(db_, idx_s, sizeof(int), 0, -1, &siz))	// 03/23/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0, sizeof(XCON_S), vbuf))	// 03/23/07 AM.
	{
//	tmp = (XCON_S *) val;	// 03/23/07 AM.
	tmp = (XCON_S *) vbuf;	// 03/23/07 AM.
	
	switch (tmp->kind)
		{
		case cXBASIC:
			sid = tmp->v.vname;
			break;
		case cXWORD:		// Probably used in dictionary within kb.
			sid = tmp->v.vsym;
			break;
		case cXPROXY:
			// Could fetch node's owning concept here...
			owner = tmp->v.vcon;
			getXcon(owner, /*DU*/ oc);
			sid = oc->v.vname;
			dpfree(oc);
			break;
		default:
			break;
		}
//	dpfree(val);	// 03/23/07 AM.
	}
return sid;
}

/********************************************
* FN:		GETXCON
* CR:		02/00/07 AM.
* SUBJ:	Get ptr from kbms, given index. [VARIANT]
* RET:	DU ptr - ptr struct filled from kbms.
*			bool - true if ok.
* WARN:	User manages the ptr struct, creating and freeing it.
********************************************/

bool XCON::getXcon(CON_ID idx, /*DU*/ XCON_S *&con)
{
con = 0;
if (idx <= 0)
	return false;

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_s = (char *) arr;

return getXcon(idx_s, con);
}


/********************************************
* FN:		GETXCON_PHRASES
* CR:		02/09/07 AM.
* SUBJ:	Get concept's phrases ptr.
* RET:	ptr_id
********************************************/

PTR_ID XCON::getXcon_phrases(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

int siz = 0;
char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.

// Adding size of key.	// FIX.	// 03/01/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0, sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	PTR_ID id = tmp->phrases;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_ATTRS
* CR:		02/09/07 AM.
* SUBJ:	Get concept's attrs.
* RET:	ptrid - id of first attr in attrs.
********************************************/

PTR_ID XCON::get_attrs(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.

// Adding size of key.	// FIX.	// 03/01/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	PTR_ID id = tmp->attrs;
//	dpfree(val);
	return id;
	}
return 0;
}
/********************************************
* FN:		GETXCON_ATOM
* CR:		02/09/07 AM.
* SUBJ:	Get phrase concept's atomic concept.
* RET:	conid
********************************************/

CON_ID XCON::getXcon_atom(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	CON_ID id = tmp->atom;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_UP
* CR:		02/09/07 AM.
* SUBJ:	Get phrase concept's up concept.
* RET:	conid
********************************************/

CON_ID XCON::get_up(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	CON_ID id = tmp->up;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_DN
* CR:		02/09/07 AM.
* SUBJ:	Get phrase concept's down concept.
* RET:	conid
********************************************/

CON_ID XCON::get_dn(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	CON_ID id = tmp->dn;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_PREV
* CR:		02/09/07 AM.
* SUBJ:	Get phrase concept's prev concept.
* RET:	conid
********************************************/

CON_ID XCON::get_prev(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	CON_ID id = tmp->prev;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_NEXT
* CR:		02/09/07 AM.
* SUBJ:	Get phrase concept's next concept.
* RET:	conid
********************************************/

CON_ID XCON::get_next(char *idx_s)
{
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XCON_S *) vbuf;
	CON_ID id = tmp->next;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GETXCON
* CR:		02/00/07 AM.
* SUBJ:	Get ptr from kbms, given index. [VARIANT]
* RET:	DU ptr - ptr struct filled from kbms.
*			bool - true if ok.
* WARN:	User manages the ptr struct, creating and freeing it.
********************************************/

bool XCON::set_name(CON_ID conid, STR_ID strid, /*DU*/ bool &dirty)
{
dirty = false;

if (conid <= 0 || strid <= 0)
	return false;

int arr[2];	// Nasty.
arr[0] = conid;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *conid_s = (char *) arr;

// Get concept.
XCON_S *tmp;	// Nasty: Pretend ptr struct is a string.
//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XCON_S)];	// 03/24/07 AM.
if (0 >= crgetwb(db_, conid_s, sizeof(int), 0,sizeof(XCON_S), vbuf))	// 03/24/07 AM.
	return false;

tmp = (XCON_S *) vbuf;	// Convert string to concept struct.

// Check that kind is basic.
if (tmp->kind != cXBASIC)
	{
//	dpfree(val);
	return false;
	}

STR_ID nm = tmp->v.vname;
if (strid == nm)	// Already has the name being changed to.
	{
//	dpfree(val);
	return true;	// Minor error, so let it through.
	}

// Name is different, so change name and overwrite concept.
// Adding size of key.	// FIX.	// 03/01/07 AM.
tmp->v.vname = strid;
//int ok = crput(db_, conid_s, sizeof(int), vbuf, siz, CR_DOVER);
int ok = crput(db_, conid_s, sizeof(int), vbuf, sizeof(XCON_S), CR_DOVER);
dirty = true;
//dpfree(val);
return ok ? true : false;
}

