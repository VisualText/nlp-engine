/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XPTR.CPP
* FILE:	cs/libqkbm/xptr.cpp
* CR:		02/07/07 AM.
* SUBJ:	Database for mapping index to its string.
* NOTE:	Can't see a way around having two databases, at double the space:
*				string	index   (this one is STRX)
*	and
*				index    string  (this one is XPTR)
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

#include "qkbm/xptr_s.h"
#include "xptr.h"

int XPTR::count_ = 0;


/********************************************
* FN:		Special functions for the class
********************************************/

XPTR::XPTR(
	)
{

db_ = 0;

#ifndef STABLE_
++count_;
#endif
}


/*******************************************/
/*******************************************/

XPTR::~XPTR()
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
* SUBJ:	Open xptr database.
* RET:	True if ok, else false.
********************************************/

bool XPTR::opendb(_TCHAR *dbname)
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

bool XPTR::closedb()
{
if (crclose(db_))
	return true;

// fprintf(stderr, "dpclose: %s\n", dperrmsg(dpecode));
return false;
}

/********************************************
* FN:		ADDXPTR
* CR:		02/07/07 AM.
* SUBJ:	Add xptr struct to  db.
* RET:	Index of added xptr in db.
* Nasty:	Concatenate values into a char array.
* NOTE:	Variants of this will add different types.
* WARN:	Need to specify SIZE of string, else null chars lead to premature
*	truncation of the string.
********************************************/

PTR_ID XPTR::addXptr(char *idx, int val, XPKIND kind, PTR_ID next)
{
// Nasty: Make an array of two objects.  Fill the first.
// Then cast it as char array and set one char past the first object to string null.
XPTR_S arr[2];
arr[0].kind = kind;
arr[0].next = next;

switch (kind)
	{
	case pXCON:
		arr[0].v.xcon = val;
		break;
	case pXPTR:
		arr[0].v.xptr = val;
		break;
	case pXSTR:
		arr[0].v.xstr = val;
		break;
	default:
		return 0;	// Error: Given wrong kind.
	}


char *tmp = (char *) arr;	// Nasty: treat array as chars.
int siz = sizeof(XPTR_S);
/*
cout << "ptr siz=" << sizeof(XPTR_S) << endl;
cout << "ptr arr="
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
cout << "ptr arr="
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
* FN:		ADDXPTR
* CR:		02/08/07 AM.
* SUBJ:	Add xptr struct to db, with float value.
* RET:	Index of added xptr in db.
* Nasty:	Concatenate values into a char array.
* NOTE:	VARIANT.
* WARN:	Need to specify SIZE of string, else null chars lead to premature
*	truncation of the string.
********************************************/

PTR_ID XPTR::addXptr(char *idx, float val, PTR_ID next)
{
XPKIND kind = pFLOAT;

// Nasty: Make an array of two objects.  Fill the first.
// Then cast it as char array and set one char past the first object to string null.
XPTR_S arr[2];
arr[0].kind = kind;
arr[0].next = next;

arr[0].v.vfloat = val;

char *tmp = (char *) arr;	// Nasty: treat array as chars.
int siz = sizeof(XPTR_S);
/*
cout << "ptr siz=" << sizeof(XPTR_S) << endl;
cout << "ptr arr="
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
cout << "ptr arr="
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
* FN:		UPDATEXPTR
* CR:		02/15/07 AM.
* SUBJ:	Update/replace pointer struct to  db.
* RET:	Index of added ptr in db.
* Nasty:	Concatenate values into a char array.
* WARN:	Need to specify SIZE of string, else null chars lead to premature
*	truncation of the string.
********************************************/

bool XPTR::updateXptr(char *id_s, XPTR_S *ptr)
{
if (!id_s || !ptr)		// NULL FIRST CHAR OK.	// FIX.	// 03/01/07 AM.
	return false;

int siz = sizeof(XPTR_S);

XPTR_S arr[2];

// Copy into a char array, terminate and update.  Nasty code.
// 02/27/07 AM. String copy doesn't seem to work right.
arr[0].kind = ptr->kind;
arr[0].next = ptr->next;
arr[0].v = ptr->v;

char *tmp = (char *) arr;	// Nasty: treat array as chars.
//strncpy(tmp,(char *)ptr,siz);
tmp[siz] = '\0';	// Terminate string.


// Adding size of key.	// FIX.	// 03/01/07 AM.
return crput(db_, id_s, sizeof(int), tmp, siz, CR_DOVER) ? true : false;
}

/********************************************
* FN:		GETXPTR
* CR:		02/07/07 AM.
* SUBJ:	Get ptr from kbms, given index.
* RET:	DU ptr - ptr struct filled from kbms.
*			bool - true if ok.
* WARN:	USER MUST FREE THE RETURNED PTR OBJECT.
********************************************/

bool XPTR::getXptr(char *idx_s, /*DU*/ XPTR_S *&ptr)
{
ptr = 0;
//XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

int siz = 0;
char *val = 0;
if (val = crget(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), &siz))	// 03/24/07 AM.
	{
/*
cout << "ptr str=" << val << endl;
cout << "ptr arr="
	<< val[0] << ","
	<< val[1] << ","
	<< val[2] << ","
	<< val[3] << ","
	<< val[4] << ","
	<< val[5] << ","
	<< val[6] << ","
	<< val[7] << ","
	<< val[8] << ","
	<< val[9] << ","
	<< val[10] << ","
	<< val[11] << ","
	<< val[12]
	<< endl;
*/

	ptr = (XPTR_S *) val;
/*
	tmp = (XPTR_S *) val;
	// Copy struct from kbms.
	ptr->kind = tmp->kind;
	ptr->next = tmp->next;
	ptr->v = tmp->v;
	dpfree(val);
*/
	return true;
	}
return false;
}

/********************************************
* FN:		RMXPTR
* CR:		02/12/07 AM.
* SUBJ:	Remove pointer from db.
* RET:	strid - id into string db.
********************************************/

bool XPTR::rmXptr(char *idx_s)
{
//int siz = sizeof(XPTR_S);
return crout(db_, idx_s, sizeof(int)) ? true : false;
}

/********************************************
* FN:		RMXPTR
* CR:		02/13/07 AM.
* SUBJ:	Remove pointer from db.
********************************************/

bool XPTR::rmXptr(PTR_ID pid)
{
int arr[2];	// Nasty.
char *x;
char *id_s;
arr[0] = pid; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;

//int siz = sizeof(XPTR_S);
return crout(db_, id_s, sizeof(int)) ? true : false;
}

/********************************************
* FN:		GET_PTR_CON
* CR:		02/09/07 AM.
* SUBJ:	Get con-valued ptr from kbms, given index.
* RET:	con_id
********************************************/

CON_ID XPTR::get_ptr_con(char *idx_s)
{
XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XPTR_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XPTR_S *) vbuf;
	if (tmp->kind != pXCON)
		{
		// Error.
		return 0;
		}
	CON_ID id = tmp->v.xcon;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_PTR_PTR
* CR:		02/09/07 AM.
* SUBJ:	Get ptr-value from ptr.
* RET:	ptr_id
********************************************/

PTR_ID XPTR::get_ptr_ptr(char *idx_s)
{
XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XPTR_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), vbuf))	// 03/23/07 AM.
	{
	tmp = (XPTR_S *) vbuf;
	if (tmp->kind != pXPTR)
		{
		// Error.
		return 0;
		}
	PTR_ID id = tmp->v.xptr;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_PTR_NEXT
* CR:		02/12/07 AM.
* SUBJ:	Get next-value from ptr.
* RET:	ptr_id
********************************************/

PTR_ID XPTR::get_ptr_next(char *idx_s)
{
XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XPTR_S)];	// 03/24/07 AM.
if (0 <crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XPTR_S *) vbuf;
	PTR_ID id = tmp->next;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_PTR_STR
* CR:		02/11/07 AM.
* SUBJ:	Get str-value from ptr.
* RET:	strid - id of string in string table.
********************************************/

STR_ID XPTR::get_ptr_str(char *idx_s)
{
XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XPTR_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XPTR_S *) vbuf;
	if (tmp->kind != pXSTR)
		{
		// Error.
		return 0;
		}
	STR_ID id = tmp->v.xstr;
//	dpfree(val);
	return id;
	}
return 0;
}

/********************************************
* FN:		GET_PTR_NUM
* CR:		02/11/07 AM.
* SUBJ:	Get num-value from ptr.
* RET:	bool - true if ok.
*			UP num - numeric value of attr.
********************************************/

bool XPTR::get_ptr_num(char *idx_s, long &num)
{
num = 0;
bool ok = true;
XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XPTR_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XPTR_S *) vbuf;
	switch (tmp->kind)
		{
		case pNUM:
			num = tmp->v.vnum;
			break;
		case pFLOAT:
			num = (long) tmp->v.vfloat;	// Convert/truncate float.
			break;
		default:
			ok = false;
			break;
		}
//	dpfree(val);
	return ok;
	}
return false;
}

/********************************************
* FN:		GET_PTR_FLT
* CR:		02/11/07 AM.
* SUBJ:	Get float-value from ptr.
* RET:	bool - true if ok.
*			UP flt - float value of attr.
********************************************/

bool XPTR::get_ptr_flt(char *idx_s, float &flt)
{
flt = 0.0;
bool ok = true;
XPTR_S *tmp;	// Nasty: Pretend ptr struct is a string.

//int siz = 0;
//char *val = 0;
char vbuf[sizeof(XPTR_S)];	// 03/24/07 AM.
if (0 < crgetwb(db_, idx_s, sizeof(int), 0,sizeof(XPTR_S), vbuf))	// 03/24/07 AM.
	{
	tmp = (XPTR_S *) vbuf;
	switch (tmp->kind)
		{
		case pNUM:
			flt = (float) tmp->v.vnum;
			break;
		case pFLOAT:
			flt = tmp->v.vfloat;	// Convert/truncate float.
			break;
		default:
			ok = false;
			break;
		}
//	dpfree(val);
	return ok;
	}
return false;
}

