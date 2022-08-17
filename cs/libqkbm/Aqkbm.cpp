/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AQKBM.CPP
* FILE:	cs/libqkbm/Aqkbm.cpp
* CR:		02/05/07 AM.
* SUBJ:	API class for the KB Manager module based on QDBM.
* NOTE:	There is a test_libqkbm program in the dev\projects area.
* DES:	Many of the attr functions have been looking for a slot by name.
*			But Conceptual Grammar KBMS requires attr slot to be a CONCEPT.
*			So reworking some of the attr stuff.
*			02/16/07 AM. Allowing attribute slot to be EITHER concept or string.
*			KBs that mix and match these are allowed, though we recommend one way
*			or t'other for consistency.
*
*******************************************************************************/

#include "StdAfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "machine-min.h"

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/list_s.h"	// 08/15/02 AM.
#include "prim/list.h"		// 08/15/02 AM.
#include "prim/str.h"		// 02/14/07 AM.

/*
#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
*/

#include "depot.h"
#include "curia.h"

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xptr_s.h"
#include "qkbm/xcon_s.h"
#include "mgr.h"
#include "strx.h"
#include "xstr.h"
#include "xptr.h"
#include "xcon.h"
#include "xsym.h"	// 02/19/07 AM.
#include "util.h"
#include "qkbm/Aqkbm.h"

int AQKBM::count_ = 0;

/********************************************
* FN:		Special functions for the class
********************************************/

AQKBM::AQKBM(
	)
{

mgr_  = 0;
strx_ = 0;
xstr_ = 0;
xptr_ = 0;
xcon_ = 0;
xsym_ = 0;	// 02/19/07 AM.
balance_ = 0;	// 03/13/07 AM.

#ifndef STABLE_
++count_;
#endif
}

/*
st_	= 0;
con_	= 0;
ptr_	= 0;

// Create knowledge base tables.
ast_  = new AST();
asym_ = new ASYM();
aptr_ = new APTR();
acon_ = new ACON();

// Set up all the backpointers among tables.
aptr_->acon_ = acon_;
aptr_->asym_ = asym_;

asym_->ast_ = ast_;

acon_->akbm_ = this;
acon_->asym_ = asym_;
*/

/*******************************************/
/*******************************************/

AQKBM::~AQKBM()
{

closekbms();	// In case kbms hasn't been explicitly closed.

#ifndef STABLE_
--count_;
#endif
}
/*
if (st_)
	delete [] (_TCHAR *) st_;
if (con_)
	delete [] (CON *) con_;
if (ptr_)
	delete [] (PTR *) ptr_;

// Todo: Each table should take care of its cleanup,
// then remove this call.
kbm_clean();

if (acon_)
	delete acon_;
if (asym_)
	delete asym_;
if (aptr_)
	delete aptr_;
if (ast_)
	delete ast_;
*/

/*******************************************/
/*******************************************/
// void *AQKBM::getSt()	{return st_;}
//void *AQKBM::getCon()	{return con_;}
//void *AQKBM::getPtr()	{return ptr_;}


//void	AQKBM::setSt(void	*x)	{st_	= x;}
//void	AQKBM::setCon(void *x)	{con_	= x;}
//void	AQKBM::setPtr(void *x)	{ptr_	= x;}

///////////

/********************************************
* FN:		ISKBMS
* CR:		03/04/07 AM.
* SUBJ:	See if given path is a kbms.
* RET:	True if found a kbms, else fals.
* NOTE:	Assume that having a manager is necessary and sufficient.
********************************************/

bool AQKBM::iskbms(_TCHAR *path)
{
char mgrname[1024];
sprintf(mgrname,"%s\\mgr",path);

MGR *mgr = new MGR();
bool exists = mgr->isdb(mgrname);
delete mgr;
return exists;
}


/********************************************
* FN:		OPENKBMS
* CR:		02/06/07 AM.
* SUBJ:	Open the current kbms.
* RET:	True if ok, else false.
********************************************/

bool AQKBM::openkbms(_TCHAR *path)
{
if (!(mgr_ = new MGR()))
	return false;

char mgrname[1024];
sprintf(mgrname,"%s\\mgr",path);
if (!mgr_->opendb(mgrname))
	return false;

if (!(strx_ = new STRX()))
	return false;
char strxname[1024];
sprintf(strxname,"%s\\strx",path);
if (!strx_->opendb(strxname))
	return false;

if (!(xstr_ = new XSTR()))
	return false;
char xstrname[1024];
sprintf(xstrname,"%s\\xstr",path);
if (!xstr_->opendb(xstrname))
	return false;

if (!(xptr_ = new XPTR()))
	return false;
char xptrname[1024];
sprintf(xptrname,"%s\\xptr",path);
if (!xptr_->opendb(xptrname))
	return false;

if (!(xcon_ = new XCON()))
	return false;
char xconname[1024];
sprintf(xconname,"%s\\xcon",path);
if (!xcon_->opendb(xconname))
	return false;

if (!(xsym_ = new XSYM()))				// 02/19/07 AM.
	return false;							// 02/19/07 AM.
char xsymname[1024];						// 02/19/07 AM.
sprintf(xsymname,"%s\\xsym",path);	// 02/19/07 AM.
if (!xsym_->opendb(xsymname))			// 02/19/07 AM.
	return false;							// 02/19/07 AM.

return true;
}

/********************************************
* FN:		CLOSEKBMS
* CR:		02/06/07 AM.
* SUBJ:	Close the current kbms.
* RET:	True if ok, else false.
********************************************/

bool AQKBM::closekbms()
{
if (mgr_)
	{
	mgr_->closedb();
	delete mgr_;	// 03/04/07 AM.
	mgr_ = 0;
	}
if (strx_)
	{
	strx_->closedb();
	delete strx_;	// 03/04/07 AM.
	strx_ = 0;
	}
if (xstr_)
	{
	xstr_->closedb();
	delete xstr_;	// 03/04/07 AM.
	xstr_ = 0;
	}
if (xptr_)
	{
	xptr_->closedb();
	delete xptr_;	// 03/04/07 AM.
	xptr_ = 0;
	}
if (xcon_)
	{
	xcon_->closedb();
	delete xcon_;	// 03/04/07 AM.
	xcon_ = 0;
	}
if (xsym_)				// 02/19/07 AM.
	{
	xsym_->closedb();	// 02/19/07 AM.
	delete xsym_;		// 03/04/07 AM.
	xsym_ = 0;			// 02/19/07 AM.
	}

if (balance_ != 0)	// 03/13/07 AM.
	{
	// REPORT FREE/ALLOC MISMATCH.	// 03/13/07 AM.
	}

return true;
}


/********************************************
* FN:		INITKBMS
* CR:		03/04/07 AM.
* SUBJ:	Initialize an EXISTING kbms.
* RET:	True if ok, else false.
********************************************/

bool AQKBM::initkbms()
{
return mgr_->initdb();
}


/********************************************
* FN:		RESETKBMS
* CR:		02/06/07 AM.
* SUBJ:	Reset the current kbms.
* RET:	True if ok, else false.
* NOTE:	Doesn't really empty out the database.
********************************************/

bool AQKBM::resetkbms()
{
return mgr_->resetdb();
}

/********************************************
* FN:		KBFREE
* CR:		02/08/07 AM.
* SUBJ:	Free up string object from qdbm.
* RET:	
* NOTE:	Change to inline.	03/13/07 AM.
********************************************/

inline void AQKBM::kbfree(void *str)
{
--balance_;	// 03/13/07 AM.
dpfree(str);
}

/********************************************
* FN:		GETSTRINGS
* CR:		02/06/07 AM.
* SUBJ:	Get strings counter from kbms.
* RET:	int - number of strings in kbms.
********************************************/

STR_ID AQKBM::getStrings()
{
return mgr_->getStrings();
}

/********************************************
* FN:		ADDSTRING
* CR:		02/06/07 AM.
* SUBJ:	Add string to kbms.
* RET:	int - Unique identifier for string in strx table.
********************************************/

STR_ID AQKBM::addString(_TCHAR *str)
{
// Increment counter.
STR_ID idx = mgr_->incStrings();

// Add string to STRX, with value = string counter.
strx_->addString(str, idx);

// Add string to XSTR, with value = index.
xstr_->addXstr(idx, str);

// Return counter.
return idx;
}


/********************************************
* FN:		GETSTRX
* CR:		02/06/07 AM.
* SUBJ:	Get string's index from kbms.
* RET:	int - Unique identifier for string in strx table.
********************************************/

STR_ID AQKBM::getStrx(_TCHAR *str)
{

// Add string to STRX, with value = string counter.
STR_ID idx = strx_->getStrx(str);

// Return counter.
return idx;
}

/********************************************
* FN:		GETXSTR
* CR:		02/07/07 AM.
* SUBJ:	Get string from kbms, given index.
* RET:	str - string from kbms.
* WARN:	USER MUST FREE STRING FROM KBMS.
********************************************/

_TCHAR *AQKBM::getXstr(STR_ID index)
{
_TCHAR *str = xstr_->getXstr(index);
if (str && *str)	// 03/24/07 AM.
	++balance_;	// 03/13/07 AM.
else
	return 0;	// 03/24/07 AM.
return str;
}


/********************************************
* FN:		ADDXSYM
* CR:		02/19/07 AM.
* SUBJ:	Bind a string to a concept.
* RET:	bool - if added/overwrote ok.
********************************************/

bool AQKBM::addXsym(STR_ID sid, CON_ID cid)
{
return xsym_->addXsym(sid, cid);
}

/********************************************
* FN:		GETXSYM
* CR:		02/07/07 AM.
* SUBJ:	Get string from kbms, given index.
* RET:	str - string from kbms.
********************************************/

CON_ID AQKBM::getXsym(STR_ID sid)
{
return xsym_->getXsym(sid);
}


/**************************************************
*						SYM_RM
* FUN:	sym_rm
* SUBJ:	Remove string's reference to concept.
* CR:		02/20/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::sym_rm(STR_ID sid)
{
if (!sid)
	return true;	// Allow call with empty.

int arr[2];	// Nasty.
arr[0] = sid;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *sid_s = (char *) arr;

return xsym_->rmXsym(sid_s);
}

/********************************************
* FN:		ADDXPTR
* CR:		02/07/07 AM.
* SUBJ:	Add pointer to kbms.
* RET:	int - Unique identifier for ptr in xptr table.
********************************************/

PTR_ID AQKBM::addXptr(int val, XPKIND kind, PTR_ID next)
{
// Increment counter.
PTR_ID idx = mgr_->incPointers();

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

// Add string to XSTR, with value = index.
xptr_->addXptr(idx_str, val, kind, next);

// Return counter.
return idx;
}

/********************************************
* FN:		ADDXPTR
* CR:		02/15/07 AM.
* SUBJ:	Add pointer to kbms.
* RET:	ptr - Pointer struct.
*			pid - Unique identifier for ptr in xptr table.
* WARN:	USER MUST FREE POINTER STRUCT.
********************************************/

XPTR_S *AQKBM::addXptr(int val,XPKIND kind, PTR_ID next,/*UP*/ PTR_ID &pid)
{
pid = 0;

// Increment counter.
PTR_ID idx = mgr_->incPointers();

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

// Add string to XSTR, with value = index.
xptr_->addXptr(idx_str, val, kind, next);

// OPT:	Not efficient to build, update, then fetch pointer.
// (DES: Problem is that the ones from db are a "string".
// So would need to distinguish those and ones created as structs.
// I suppose one could copy the structs to a string or vice versa. Yucky.)
XPTR_S *ptr = 0;
if (!xptr_->getXptr(idx_str, /*DU*/ ptr))
	return 0;
++balance_;	// 03/13/07 AM.

pid = idx;	// Everything ok, so return id here.
return ptr;
}

/********************************************
* FN:		ADDXPTR
* CR:		02/15/07 AM.
* SUBJ:	Add pointer to kbms.
* RET:	ptr - Pointer struct.
*			pid - Unique identifier for ptr in xptr table.
* WARN:	USER MUST FREE POINTER STRUCT.
********************************************/

XPTR_S *AQKBM::addXptr(XPTR_S *ptr, /*UP*/ PTR_ID &pid)
{
pid = 0;

// Increment counter.
PTR_ID idx = mgr_->incPointers();

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

if (!xptr_->updateXptr(idx_str, ptr))
	return 0;

pid = idx;	// Everything ok, so return id here.

// MUST RE-FETCH PTR, FOR CONSISTENCY.		// FIX.	// 02/28/07 AM.
XPTR_S *newptr = 0;								// FIX.	// 02/28/07 AM.
if (!xptr_->getXptr(idx_str, /*DU*/ newptr))	// FIX.	// 02/28/07 AM.
	return 0;	// 03/24/07 AM.
++balance_;	// 03/13/07 AM.

return newptr;
}

/********************************************
* FN:		ADDXPTR
* CR:		02/08/07 AM.
* SUBJ:	Add float pointer to kbms. [VARIANT]
* RET:	int - Unique identifier for ptr in xptr table.
********************************************/

PTR_ID AQKBM::addXptr(float val, PTR_ID next)
{
// Increment counter.
PTR_ID idx = mgr_->incPointers();

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

// Add string to XSTR, with value = index.
xptr_->addXptr(idx_str, val, next);

// Return counter.
return idx;
}


/********************************************
* FN:		PTR_UPDATE
* CR:		02/15/07 AM.
* SUBJ:	Update ptr object in db.
* RET:	bool - true if ok.
********************************************/

bool AQKBM::ptr_update(PTR_ID pid, XPTR_S *ptr)
{
if (!pid)
	return 0;	// 02/28/07 AM.

int arr[2];	// Nasty.
arr[0] = pid;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *id_s = (char *) arr;

return xptr_->updateXptr(id_s,ptr);
}

/********************************************
* FN:		GETXPTR
* CR:		02/07/07 AM.
* SUBJ:	Get ptr from kbms, given index.
* RET:	DU ptr - ptr struct filled from kbms.
*			bool - true if ok.
* WARN:	User manages the ptr struct, creating and freeing it.
********************************************/

bool AQKBM::getXptr(PTR_ID idx, /*DU*/ XPTR_S *&ptr )
{
ptr = 0;
if (!idx)
	{
	ptr = 0;	// 02/28/07 AM.
	return false;	// 02/28/07 AM.
	}
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

if (!xptr_->getXptr(idx_str, /*DU*/ ptr))
	return false;	// 03/24/07 AM.
++balance_;	// 03/13/07 AM.
return true;
}

/********************************************
* FN:		PTR_GET_CON
* CR:		02/09/07 AM.
* SUBJ:	Get con from kbms, given index.
* RET:	DU ptr - ptr struct filled from kbms.
*			bool - true if ok.
* WARN:	User manages the ptr struct, creating and freeing it.
********************************************/

CON_ID AQKBM::ptr_get_con(PTR_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xptr_->get_ptr_con(idx_str);
}

/********************************************
* FN:		PTR_GET_PTR
* CR:		02/09/07 AM.
* SUBJ:	Get ptr value from ptr.
* RET:	ptrid - ptr value of ptr.
********************************************/

PTR_ID AQKBM::ptr_get_ptr(PTR_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xptr_->get_ptr_ptr(idx_str);
}

/********************************************
* FN:		PTR_GET_NEXT
* CR:		02/12/07 AM.
* SUBJ:	Get next value from ptr.
* RET:	ptrid - next value of ptr.
********************************************/

PTR_ID AQKBM::ptr_get_next(PTR_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xptr_->get_ptr_next(idx_str);
}

/********************************************
* FN:		PTR_GET_STR
* CR:		02/12/07 AM.
* SUBJ:	Get str value from ptr.
* RET:	strid.
********************************************/

STR_ID AQKBM::ptr_get_str(PTR_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xptr_->get_ptr_str(idx_str);
}

/********************************************
* FN:		PTR_GET_STR_DB
* CR:		02/14/07 AM.
* SUBJ:	Get string value from ptr.
* RET:	str - Actual db string.
* WARN:	USER MUST FREE THE STRING.
********************************************/

_TCHAR *AQKBM::ptr_get_str_db(PTR_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

STR_ID sidx = xptr_->get_ptr_str(idx_str);

// Get string from string table.
_TCHAR *s = getXstr(sidx);
return s;
}


/********************************************
* FN:		PTR_GET_STR
* CR:		02/11/07 AM.
* SUBJ:	Get string value from ptr.
* RET:	ptrid - ptr value of ptr.
********************************************/

bool AQKBM::ptr_get_str(PTR_ID idx, /*UP*/ _TCHAR *str)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

STR_ID sidx = xptr_->get_ptr_str(idx_str);

// Get string from string table.
_TCHAR *s = getXstr(sidx);
if (!s)
	return true;	// 02/28/07 AM.

// Copy string.
_tcscpy(str, s);
kbfree(s);
return true;
}


/********************************************
* FN:		PTR_GET_NUM
* CR:		02/11/07 AM.
* SUBJ:	Get num value from ptr.
* RET:	num - numeric value of ptr.
* NOTE:	Float truncated to long.
********************************************/

bool AQKBM::ptr_get_num(PTR_ID idx, /*UP*/ long &num)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xptr_->get_ptr_num(idx_str, /*UP*/ num);
}

/********************************************
* FN:		PTR_GET_FLT
* CR:		02/11/07 AM.
* SUBJ:	Get float value from ptr.
* RET:	flt - float value of ptr.
* NOTE:	Long converted to float.
********************************************/

bool AQKBM::ptr_get_flt(PTR_ID idx, /*UP*/ float &flt)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xptr_->get_ptr_flt(idx_str, /*UP*/ flt);
}


/********************************************
* FN:		ADDXCON
* CR:		02/07/07 AM.
* SUBJ:	Add concept to kbms.
* RET:	int - Unique identifier for con in xcon table.
********************************************/

CON_ID AQKBM::addXcon(int val, XCKIND kind,
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
// Increment counter.
CON_ID idx = mgr_->incConcepts();

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

xcon_->addXcon(idx_str, idx, val, kind, flags,attrs,up,dn,prev,next,atom, phrases);

// Return counter.
return idx;
}

/********************************************
* FN:		ADDXCON
* CR:		02/14/07 AM.
* SUBJ:	Add concept to kbms.
* RET:	cid - Unique identifier for con in xcon table.
********************************************/

CON_ID AQKBM::addXcon(XCON_S *con)
{
if (!con)		// 03/08/07 AM.
	return 0;	// 03/08/07 AM.

// Increment counter.
CON_ID idx = mgr_->incConcepts();

// Place id into con structure.	// FIX.	// 03/08/07 AM.
con->id = idx;							// FIX.	// 03/08/07 AM.

con_update(idx, con);

// Return counter.
return idx;
}

/********************************************
* FN:		CON_UPDATE
* CR:		02/13/07 AM.
* SUBJ:	Update con object in db.
* RET:	bool - true if ok.
********************************************/

bool AQKBM::con_update(CON_ID cid, XCON_S *con)
{
if (!cid)
	return 0;	// 02/28/07 AM.

int arr[2];	// Nasty.
arr[0] = cid;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *id_s = (char *) arr;

return xcon_->updateXcon(id_s,con);
}

/********************************************
* FN:		CON
* CR:		02/14/07 AM.
* SUBJ:	Get con from kbms, given index.
* RET:	con - con struct filled from kbms.
* WARN:	USER MUST FREE THE RETURNED CONCEPT STRUCT WITH dpfree or cg->kbfree().
********************************************/

XCON_S *AQKBM::Con(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

XCON_S *con;
if (!xcon_->getXcon(idx_str, /*DU*/ con))
	return 0;	// 03/24/07 AM.
++balance_;	// 03/13/07 AM.
return con;
}

/********************************************
* FN:		GETXCON
* CR:		02/07/07 AM.
* SUBJ:	Get con from kbms, given index.
* RET:	DU con - con struct filled from kbms.
*			bool - true if ok.
* WARN:	User manages the con struct, creating and freeing it.
********************************************/

bool AQKBM::getXcon(CON_ID idx, /*DU*/ XCON_S *&con)
{
con = 0;
if (!idx)
	{
	con = 0;	// 02/28/07 AM.
	return false;	// 02/28/07 AM.
	}
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

if (!xcon_->getXcon(idx_str, /*DU*/ con))
	return false;
++balance_;	// 03/13/07 AM.
return true;
}

/********************************************
* FN:		GETXCON_PHRASES
* CR:		02/09/07 AM.
* SUBJ:	Get con from kbms, given index.
* RET:	ptrid.
********************************************/

PTR_ID AQKBM::getXcon_phrases(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->getXcon_phrases(idx_str);
}

/********************************************
* FN:		CON_GET_NAME
* CR:		02/12/07 AM.
* SUBJ:	Get concept's name.
* RET:	strid.
* NOTE:	IDENTICAL TO con_sid.
********************************************/

STR_ID AQKBM::con_get_name(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_name(idx_str);
}

/********************************************
* FN:		CON_STR
* CR:		02/14/07 AM.
* SUBJ:	Get concept's name.
* RET:	str from kb.
* WARN:	USER MUST FREE STRING.
********************************************/

_TCHAR *AQKBM::con_str(CON_ID idx)
{
if (!idx)
	return 0;

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

STR_ID sid = xcon_->get_name(idx_str);
_TCHAR *str = getXstr(sid);	// Actual db str returned.	// FIX.	// 02/27/07 AM.
if (str && *str)	// 03/24/07 AM.
	++balance_;	// 03/13/07 AM.
else
	return 0;
return str;
}

/********************************************
* FN:		CON_STR
* CR:		02/14/07 AM.
* SUBJ:	Get concept's name.
* RET:	str from kb.
* WARN:	USER MUST FREE STRING.
* NOTE:	Copied from xcon/get_name
********************************************/

_TCHAR *AQKBM::con_str(XCON_S *con)
{
if (!con)
	return 0;

STR_ID sid = 0;
CON_ID owner;
XCON_S *oc;

switch (con->kind)
	{
	case cXBASIC:
		sid = con->v.vname;
		break;
	case cXWORD:				// Probably obsolete.
		sid = con->v.vsym;	// Probably obsolete.
		break;
	case cXPROXY:
		// Could fetch node's owning concept here...
		owner = con->v.vcon;
		getXcon(owner, /*DU*/ oc);
		sid = oc->v.vname;
		kbfree(oc);
		break;
	default:
		break;
	}
return getXstr(sid);	// Actual db str returned.	// FIX.	// 02/27/07 AM.
}


/********************************************
* FN:		CON_SID
* CR:		02/15/07 AM.
* SUBJ:	Get concept's name.
* RET:	sid - id for concept's name.
* NOTE:	Consistent naming...
*			IDENTICAL TO con_get_name.
********************************************/

STR_ID AQKBM::con_sid(CON_ID idx)
{
if (!idx)
	return 0;

int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_name(idx_str);
}

/********************************************
* FN:		CON_SID
* CR:		02/15/07 AM.
* SUBJ:	Get concept's name.
* RET:	sid - id for concept's name.
********************************************/

STR_ID AQKBM::con_sid(XCON_S *con)
{
if (!con)
	return 0;

STR_ID sid;

switch (con->kind)
	{
	case cXBASIC:
		sid = con->v.vname;
		break;
	case cXWORD:				// Probably obsolete.
		sid = con->v.vsym;	// Probably obsolete.
		break;
	case cXPROXY:
		// Could fetch node's owning concept here...
		return node_sid(con);
		break;
	default:
		sid = 0;
		break;
	}
return sid;
}


/********************************************
* FN:		NODE_SID
* CR:		02/20/07 AM.
* SUBJ:	Get node's name.
* RET:	sid - id for concept's name.
********************************************/

STR_ID AQKBM::node_sid(XCON_S *con)
{
if (!con)
	return 0;

STR_ID sid;

if (con->kind != cXPROXY)
	return 0;

// Fetch node's target concept.
CON_ID tcid = con->v.vcon;
XCON_S *tcon;
getXcon(tcid, /*UP*/ tcon);

switch (tcon->kind)
	{
	case cXBASIC:
		sid = tcon->v.vname;	// FIX.	// 03/08/07 AM.
		break;
	case cXWORD:				
		sid = tcon->v.vsym;	// FIX.	// 03/08/07 AM.
		break;
	case cXPROXY:
	default:
		sid = 0;	// No recursive node-to-node allowed.
		break;
	}
kbfree(tcon);
return sid;
}

/********************************************
* FN:		CON_GET_ATTRS
* CR:		02/09/07 AM.
* SUBJ:	Get concept's attrs.
* RET:	ptrid.
********************************************/

PTR_ID AQKBM::con_get_attrs(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_attrs(idx_str);
}

/********************************************
* FN:		GETXCON_ATOM
* CR:		02/09/07 AM.
* SUBJ:	Get phrase concept's (node's) atomic concept..
* RET:	conid.
********************************************/

CON_ID AQKBM::getXcon_atom(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->getXcon_atom(idx_str);
}

/********************************************
* FN:		GET_UP
* CR:		02/09/07 AM.
* SUBJ:	Get concept's up concept..
* RET:	conid.
********************************************/

CON_ID AQKBM::get_up(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_up(idx_str);
}

/********************************************
* FN:		GET_DN
* CR:		02/09/07 AM.
* SUBJ:	Get concept's dn concept..
* RET:	conid.
********************************************/

CON_ID AQKBM::get_dn(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_dn(idx_str);
}

/********************************************
* FN:		GET_PREV
* CR:		02/09/07 AM.
* SUBJ:	Get concept's prev concept..
* RET:	conid.
********************************************/

CON_ID AQKBM::get_prev(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_prev(idx_str);
}

/********************************************
* FN:		GET_NEXT
* CR:		02/09/07 AM.
* SUBJ:	Get concept's next concept..
* RET:	conid.
********************************************/

CON_ID AQKBM::get_next(CON_ID idx)
{
if (!idx)
	return 0;	// 02/28/07 AM.
int arr[2];	// Nasty.
arr[0] = idx;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *idx_str = (char *) arr;

return xcon_->get_next(idx_str);
}


/**************************************************
*						CON_PHRASE
* FUN:	con_phrase
* SUBJ:	Get the phrase associated with the given concept.
* RET:	Id for the first concept of the first phrase.
* NOTE:	Moved around from libkbm.
*
**************************************************/

CON_ID AQKBM::con_phrase(
	CON_ID idx
	)
{
if (!idx)
	return 0;	// 02/28/07 AM.
// Get concept's phrases.
PTR_ID ptrid = getXcon_phrases(idx);
if (!ptrid)
	return 0;

// Get node-value of first phrase.
CON_ID conid = ptr_get_con(ptrid);

return conid;
}

/**************************************************
*						CON_PHRASE
* FUN:	con_phrase
* SUBJ:	Get the phrase associated with the given concept. [VARIANT]
* RET:	Id for the first concept of the first phrase.
* NOTE:	Moved around from libkbm.
*
**************************************************/

CON_ID AQKBM::con_phrase(
	XCON_S *con
	)
{
if (!con)
	return 0;

// Get node-value of first phrase.
return ptr_get_con(con->phrases);
}

/**************************************************
*						PHRASE_CON
* FUN:	phrase_con
* SUBJ:	Get the atomic concept associated with the given phrase.
* NOTE:	Moved around from libkbm.
*			Assumes every node in phrase points back to atomic concept.
*	(Old scheme used to be, get the first node in the phrase,
*	and it points back to atomic concept.)
*
**************************************************/

CON_ID AQKBM::phrase_con(
	CON_ID phr
	)
{
if (!phr)
	return 0;	// 02/28/07 AM.
// Fetch phrase node (ie concept) atomic concept.
CON_ID conid = getXcon_atom(phr);
return conid;
}

//////////////////////////////////////

/**************************************************
*						SYM_GET
* FUN:	sym_get
* SUBJ:	Find string in kb or add it.
* CR:		02/09/07 AM.
* RET:	strid - id for found or created string.
*			UP dirty - if kb has changed.
* NOTE:	Find or intern a string in the kb.  Keeping the old name for now.
*
**************************************************/


STR_ID AQKBM::sym_get(
	_TCHAR *str,
	bool &dirty		// If KB has changed.
	)
{
dirty = false;	// Default.

if (!str || !*str)
	return 0;

// Try to find the string in kb.
STR_ID id = getStrx(str);		// Get string's index.
if (id > 0)
	return id;

// Not found, so add it.
if (id = addString(str))
	dirty = true;	// Successful add to kb.
return id;
}

/**************************************************
*						CON_SET_NAME
* FUN:	con_set_name
* SUBJ:	Change name of basic concept.
* CR:		02/09/07 AM.
* RET:	bool - True if everything ok.
*			UP dirty - if kb has changed.
*
**************************************************/


bool AQKBM::con_set_name(CON_ID con, STR_ID name, bool &dirty)
{
if (!con)
	return 0;	// 02/28/07 AM.
// Get concept.
// Check that kind is basic.
// Check that name is different.
// Change name.
return xcon_->set_name(con,name, /*UP*/ dirty);
}


/**************************************************
*						NODE_SET_TARGET
* FUN:	node_set_target
* SUBJ:	Change the concept that given node represents.
* CR:		02/21/07 AM.
* RET:	bool - True if everything ok.
*			UP dirty - if kb has changed.
*
**************************************************/


bool AQKBM::node_set_target(XCON_S *node, CON_ID cid, bool &dirty)
{
dirty = false;

if (!node || !cid)
	return false;

if (node->kind != cXPROXY)
	return false;

if (node->v.vcon == cid)
	{
	// Node already points to this concept. No change.
	return false;
	}

// Update the node.
bool ok = con_update(node->id, node);
if (ok)
	dirty = true;	// UP. kb has changed.
return ok;
}


/**************************************************
*						ATTR_FIND_STR
* FUN:	attr_find_str
* SUBJ:	Get string-value of named attr of concept.
* CR:		02/09/07 AM.
* RET:	bool - True if everything ok.
*			UP str - copy of the string from kb..
*
**************************************************/

bool AQKBM::attr_find_str(CON_ID con, _TCHAR *name, /*UP*/ _TCHAR *str)
{
*str = '\0';

if (con <= 0 || !name || !*name)
	return false;

// Get concept attrs.
PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return true;

// TODO: TRAVERSE DOWN ATTRS LOOKING FOR NAME.
// Get first attr key.
PTR_ID val = attr_find_val_by_name(attrs,name);

// Get string value from ptr.
// Check that ptr kind is string.
// Copy into given buffer.
return ptr_get_str(val, str);
}

/**************************************************
*						ATTR_FIND_STR
* FUN:	attr_find_str
* SUBJ:	Get string-value of named attr of concept. [VARIANT]
* CR:		02/14/07 AM.
* RET:	bool - True if everything ok.
*			UP strid - the string from kb.
*
**************************************************/

STR_ID AQKBM::attr_find_str(CON_ID con, _TCHAR *name)
{
if (con <= 0 || !name || !*name)
	return false;

// Get concept attrs.
PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return true;

// TODO: TRAVERSE DOWN ATTRS LOOKING FOR NAME.
// Get first attr key.
PTR_ID val = attr_find_val_by_name(attrs,name);

// Get string value from ptr.
// Check that ptr kind is string.
return ptr_get_str(val);
}

/**************************************************
*						ATTR_FIND_STR_DB
* FUN:	attr_find_str_db
* SUBJ:	Get string-value of named attr of concept.
* CR:		02/09/07 AM.
* RET:	bool - True if everything ok.
*			UP str - the string from kb.
* WARN:	USER MUST FREE KB STRING.
*
**************************************************/

bool AQKBM::attr_find_str_db(CON_ID con, _TCHAR *name, /*UP*/ _TCHAR *&str)
{
str = 0;	// FIX.	03/12/07 AM.

if (con <= 0 || !name || !*name)
	return false;

// Get concept attrs.
PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return true;

// TODO: TRAVERSE DOWN ATTRS LOOKING FOR NAME.
// Get first attr key.
PTR_ID val = attr_find_val_by_name(attrs,name);

// Get string value from ptr.
// Check that ptr kind is string.
// Copy into given buffer.
str = ptr_get_str_db(val);
return true;
}


/**************************************************
*						ATTR_FIND_NUM
* FUN:	attr_find_num
* SUBJ:	Get num-value of named attr of concept.
* CR:		02/11/07 AM.
* RET:	bool - True if found num-valued attr.
*			UP num - long value.
* NOTE:	For backward compatibility, truncating float value if found.
*
**************************************************/

bool AQKBM::attr_find_num(CON_ID con, _TCHAR *name, /*UP*/ long &num)
{
num = 0;

if (con <= 0 || !name || !*name)
	return false;

// Get concept attrs.
PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return false;

// TODO: TRAVERSE DOWN ATTRS LOOKING FOR NAME.
// Get first attr key.
PTR_ID val = attr_find_val_by_name(attrs,name);

// Get string value from ptr.
// Check that ptr kind is string.
// Copy into given buffer.
return ptr_get_num(val, num);
}

/**************************************************
*						ATTR_FIND_FLT
* FUN:	attr_find_flt
* SUBJ:	Get float-value of named attr of concept.
* CR:		02/11/07 AM.
* RET:	bool - True if found num-valued attr.
*			UP num - long value.
* NOTE:	Converting long value if found.
*
**************************************************/

bool AQKBM::attr_find_flt(CON_ID con, _TCHAR *name, /*UP*/ float &flt)
{
flt = 0.0;

if (con <= 0 || !name || !*name)
	return false;

// Get concept attrs.
PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return false;

// TODO: TRAVERSE DOWN ATTRS LOOKING FOR NAME.
// Get first attr key.
PTR_ID val = attr_find_val_by_name(attrs,name);

// Get string value from ptr.
// Check that ptr kind is string.
// Copy into given buffer.
return ptr_get_flt(val, flt);
}

/**************************************************
*						ATTR_FIND_CON
* FUN:	attr_find_con
* SUBJ:	Get con-value of named attr of concept.
* CR:		02/11/07 AM.
* RET:	bool - True if found con-valued attr.
*			UP con - concept value.
*
**************************************************/

bool AQKBM::attr_find_con(CON_ID con, _TCHAR *name, /*UP*/ CON_ID &cid)
{
cid = 0;

if (con <= 0 || !name || !*name)
	return false;

// Get concept attrs.
PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return false;

// TODO: TRAVERSE DOWN ATTRS LOOKING FOR NAME.
// Get first attr key.
PTR_ID val = attr_find_val_by_name(attrs,name);

// Get con value from ptr.
// Check that ptr kind is con.
cid = ptr_get_con(val);
return cid ? true : false;
}

/**************************************************
*						CON_ATTR_FIND
* FUN:	con_attr_find
* SUBJ:	Find specified attr of given concept.
* CR:		02/10/07 AM.
* RET:	ptrid - first value of attr, if found.
* NOTE:	Reimplementation with qdbm.
*			Return first argument that matches.
*
**************************************************/

PTR_ID AQKBM::con_attr_find(CON_ID con, _TCHAR *name)
{
return true;
}

/**************************************************
*						ATTR_FIND_VAL_BY_NAME
* FUN:	attr_find_val_by_name
* SUBJ:	Find specified attr of given concept.
* CR:		07/12/99 AM. 02/10/07 AM.
* RET:	ptrid - first value of attr, if found.
* NOTE:	Reimplementation with qdbm.
*			Return first argument that matches.
*			'h' is short for handle.
*			IN CG, THE SLOT IS A CONCEPT, NOT A STRING!!!	02/15/07 AM.
*			So, getting first attribute whose slot-concept has the sought name.
*			02/16/07 Renaming from attr_find_h, to not overwrite attr functions.
*			02/16/07 AM. ALLOWING SLOT TO BE CONCEPT OR STRING!
*
**************************************************/

PTR_ID AQKBM::attr_find_val_by_name(PTR_ID attrs, _TCHAR *name)
{
if (!attrs || !name || !*name)
	return 0;

// Get id for string comparison.
STR_ID name_sid = getStrx(name);
if (!name_sid)
	return 0;

XPTR_S *hptr, *slotptr;
PTR_ID next, slot, val;
STR_ID slot_sid;
while (attrs)
	{
	if (!getXptr(attrs, /*DU*/ hptr))
		return 0;	// 03/08/07 AM.
	slot = hptr->v.xptr;
	next = hptr->next;
	kbfree(hptr);

	// SLOT IS EITHER A CONCEPT OR A STRING. // 02/16/07 AM.
	getXptr(slot, /*DU*/ slotptr);
	val = slotptr->next;	// In case this is the right one.
	slot_sid = slot_sid(slotptr);		// 02/16/07 AM.
	kbfree(slotptr);

	if (name_sid == slot_sid)	// Found matching slot-concept name.
		return val;

	attrs = next;
	}
val = 0;
return 0;	// Named attr not found.
}


/**************************************************
*						FIND_ATTR_VAL
* FUN:	find_attr_val
* SUBJ:	Find attribute with particular value.
* CR:		02/12/07 AM.
* RET:	bool - if value found.
*
**************************************************/

bool AQKBM::find_attr_val(CON_ID con, _TCHAR *attr, _TCHAR *val)
{
if (!con || !attr || !*attr || !val || !*val)
	return false;

PTR_ID attrs = con_get_attrs(con);
if (!attrs)
	return false;

// Find string in db.
STR_ID sid = getStrx(val);
if (!sid)
	return false;	// String is not in db yet, so can't be a value of an attribute.


PTR_ID vv = attr_find_val_by_name(attrs, attr);
STR_ID ss;

// Now find the particular string. Traverse values.
for (; vv; vv = ptr_get_next(vv))
	{
	ss = ptr_get_str(vv);
	if (ss && ss == sid)	// Found match.
		return true;
	}
return false;
}

/**************************************************
*						PTR_VAL_AS_STR
* FUN:	ptr_val_to_str
* SUBJ:	Fetch pointer value as a string.
* CR:		02/12/07 AM.
* RET:	bool - if value converted ok.
*
**************************************************/

bool AQKBM::ptr_val_as_str(PTR_ID ptr, _TCHAR *buf)
{
if (!ptr || !buf)
	return false;
*buf = '\0';

XPTR_S *sptr;
if (!getXptr(ptr, /*DU*/ sptr))
	return false;

float fnum;
long num;
bool ok = true;
_TCHAR *str;
switch (sptr->kind)
	{
	case pXSTR:
		str = getXstr(sptr->v.xstr);	// Fetch string.
		if (!str)	// 02/28/07 AM.
			break;	// 02/28/07 AM.
		_tcscpy(buf, str);
		kbfree(str);
		break;
	case pNUM:
		num = sptr->v.vnum;
		_stprintf(buf, _T("%ld"), num);	// Convert to string.
		break;
	case pFLOAT:
		fnum = sptr->v.vfloat;
		_stprintf(buf, _T("%f"), fnum);
		break;
	default:
		ok = false;
		break;
	}
kbfree(sptr);
return ok;
}

/**************************************************
*						CON_RM
* FUN:	con_rm
* SUBJ:	Remove concept and all data associated with it.
* CR:		02/12/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::con_rm(CON_ID cid)
{
if (!cid)
	return true;	// Allow call with empty.

int arr[2];	// Nasty.
arr[0] = cid;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *cid_s = (char *) arr;


XCON_S *con;
bool ok = getXcon(cid, /*DU*/ con);
if (!ok)
	return false;

ok = con_rm(con, cid_s);
kbfree(con);
return ok;
}


/**************************************************
*						CON_RM
* FUN:	con_rm
* SUBJ:	Remove concept and all data associated with it.
* CR:		02/12/07 AM.
* RET:	bool - if removed ok.
* NOTE:	DOES NOT DETACH.
*			DOES NOT REMOVE CHILDREN.
*			Hierarchic part of this is in libqconsh; see con_rm_hier.
*
**************************************************/

bool AQKBM::con_rm(XCON_S *con, char *cid_s)
{
if (!con)
	return true;	// Empty is ok.
if (!cid_s)	// NULL FIRST CHAR OK.	// FIX.	// 03/01/07 AM.
	return false;	// Need the name of the concept in db.

// Check if concept is a node within a phrase.
// To assure user knows what he's doing, return false in this case.
if (con->kind != cXBASIC && con->kind != cXWORD)
	return false;

// Remove attributes.
con_rm_attrs(con);

// Remove phrases.
con_rm_phrases(con);

// REMOVE CONCEPT ITSELF FROM KB HERE.
xcon_->rmXcon(cid_s);

return true;
}



/**************************************************
*						CON_RM
* FUN:	con_rm
* SUBJ:	Remove concept and all data associated with it.
* CR:		02/12/07 AM.
* RET:	bool - if removed ok.
* NOTE:	DOES NOT DETACH.
*			DOES NOT REMOVE CHILDREN.
*			Hierarchic part of this is in libqconsh; see con_rm_hier.
*
**************************************************/

bool AQKBM::con_rm(XCON_S *con)
{
if (!con)
	return true;	// Empty is ok.

CON_ID cid = con->id;
if (!cid)
	return true;	// Allow call with empty.

int arr[2];	// Nasty.
arr[0] = cid;
char *x = (char *) &(arr[1]);
*x = '\0';	// Nasty.
char *cid_s = (char *) arr;

return con_rm(con, cid_s);
}


/**************************************************
*						CON_RM_PHRASES
* FUN:	con_rm_phrases
* SUBJ:	Remove phrases of a concept.
* CR:		02/12/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::con_rm_phrases(XCON_S *con)
{
if (!con)
	return false;

int arr[2];	// Nasty.
char *x;
char *id_s;

PTR_ID phrases = con->phrases;
XPTR_S *ptr;
CON_ID nid;
XCON_S *node;

// Traverse list of pointers.
while (phrases)
	{
	getXptr(phrases, /*DU*/ ptr);
	xptr_->rmXptr(phrases);	// Delete ptr from kb.
	nid = ptr->v.xcon;
	phrases = ptr->next;
	kbfree(ptr);	// Free fetched copy of ptr.

	// Traverse list of nodes, ie a phrase.
	while (nid)
		{
		arr[0] = nid; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;
		xcon_->getXcon(id_s, /*DU*/ node);
		if (node)
			{
			++balance_;	// FIX COUNT.	// 03/21/07 AM.
			nid = node->next;
			node->dn = 0;	// Zero out in case of db inconsistency.
			con_rm(node, id_s);	// Delete node from kb.
			kbfree(node);			//	Free the fetched copy of node.
			}
		else
			nid = 0;
		}
	}


con->phrases = 0;	// Zero out phrases.
con_update(con->id,con);	// FIX.	// 03/12/07 AM.

return true;
}


/**************************************************
*						CON_RM_ATTRS
* FUN:	con_rm_attrs
* SUBJ:	Remove attributes of a concept.
* CR:		02/12/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::con_rm_attrs(XCON_S *con)
{
return true;
}


/**************************************************
*						CON_MOVE_LEFT
* FUN:	con_move_left
* SUBJ:	Move concept one to the left in its list.
* CR:		02/13/07 AM.
* RET:	bool - if moved ok.
* NOTE:	Concept may be in a hierarchy list or a list of nodes.
*			Concepts are:  (left) lsib con rsib
*			After move they are (left) con lsib rsib
*
**************************************************/

bool AQKBM::con_move_left(CON_ID cid)
{
if (!cid)
	return false;

bool ok = true;
int arr[2]; char *x; char *id_s;	// Nasty.
arr[0] = cid; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;

XCON_S *con;
if (!xcon_->getXcon(id_s, /*DU*/ con))
	return false;
++balance_;	// FIX COUNT.	// 03/21/07 AM.

if (con->kind == cXPROXY)
	{
	// Move node to the left.
	ok = node_move_left(id_s);
	kbfree(con);
	return ok;
	}

CON_ID lsib_id = con->prev;

if (!lsib_id)	// Concept already first in its list.
	{
	kbfree(con);
	return false;	// Cannot move further left.
	}

XCON_S *lsib = 0;
getXcon(lsib_id, /*DU*/ lsib);
if (!lsib)
	{
	kbfree(con);
	return false;
	}

XCON_S *rsib = 0;
CON_ID rsib_id = con->next;
getXcon(rsib_id, /*UP*/ rsib);

XCON_S *left = 0;	// Leftmost node of interest.
CON_ID left_id = lsib->prev;
getXcon(left_id, /*UP*/ left);

XCON_S *up = 0;	// Parent, if needed.
CON_ID up_id = 0;

// Interchange con and lsib.
//left->next = cid;	// 03/08/07 AM.
con->prev = left_id;
con->next = lsib_id;
lsib->prev = cid;
lsib->next = rsib_id;
if (rsib)
	rsib->prev = lsib_id;
if (left)
	left->next = cid;
else
	{
	// Con becomes new head of list.
	up_id = lsib->up;
	getXcon(up_id, /*UP*/ up);
	con->up = up_id;
	up->dn = cid;
	}

// If present, update and then free!
if (lsib)
	{
	con_update(lsib_id, lsib);
	kbfree(lsib);
	}
if (rsib)
	{
	con_update(rsib_id, rsib);
	kbfree(rsib);
	}
if (left)
	{
	con_update(left_id, left);
	kbfree(left);
	}
if (up)
	{
	con_update(up_id, up);
	kbfree(up);
	}
con_update(cid, con);
kbfree(con);
return true;
}

/**************************************************
*						NODE_MOVE_LEFT
* FUN:	node_move_left
* SUBJ:	Move node one to the left in its list.
* CR:		02/13/07 AM.
* RET:	bool - if moved ok.
* NOTE:	Similar to above, but need to fix up atom and phrases.
*			(If moving to first node of phrase, need to seek phrase among phrases....tedious)
* OPT:	Placeholder for optimized treatment.
*
**************************************************/

bool AQKBM::node_move_left(char *id_s)
{
return true;
}



/**************************************************
*						CON_MOVE_RIGHT
* FUN:	con_move_right
* SUBJ:	Move concept one to the right in its list.
* CR:		02/13/07 AM.
* RET:	bool - if moved ok.
* NOTE:	Distinguishing concept list and node list.
*			Concepts are:  (lsib) con rsib (right)
*			After move they are (lsib) rsib con (right)
*
**************************************************/

bool AQKBM::con_move_right(CON_ID cid)
{
if (!cid)
	return false;

bool ok = true;
int arr[2]; char *x; char *id_s;	// Nasty.
arr[0] = cid; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;

XCON_S *con;
if (!xcon_->getXcon(id_s, /*DU*/ con))
	return false;
++balance_;	// FIX COUNT.	// 03/21/07 AM.

if (con->kind == cXPROXY)
	{
	// Move node to the left.
	ok = node_move_right(id_s);
	kbfree(con);
	return ok;
	}

XCON_S *rsib = 0;
CON_ID rsib_id = con->next;
if (!rsib_id)	// Can't move to the right.
	{
	kbfree(con);
	return false;
	}
getXcon(rsib_id, /*UP*/ rsib);

CON_ID lsib_id = con->prev;
XCON_S *lsib = 0;
if (lsib_id)
	getXcon(lsib_id, /*DU*/ lsib);


XCON_S *right = 0;	// Rightmost node of interest.
CON_ID right_id = 0;
if (rsib)
	{
	right_id = rsib->next;
	getXcon(right_id, /*UP*/ right);
	}

XCON_S *up = 0;	// Parent, if needed.
CON_ID up_id = 0;

if (!lsib)	// Moving con from head of list.
	{
	up_id = con->up;
	getXcon(up_id, /*UP*/ up);

	// Fix up new head of list.
	rsib->up = up_id;
	up->dn = rsib_id;
	}
else
	{
	// Fix up left part of list.
	lsib->next = rsib_id;
	rsib->prev = lsib_id;
	}

// Interchange con and rsib.
rsib->next = cid;
con->prev = rsib_id;
con->next = right_id;
if (right)
	right->prev = cid;

// If present, update and then free!
if (lsib)
	{
	con_update(lsib_id, lsib);
	kbfree(lsib);
	}
if (rsib)
	{
	con_update(rsib_id, rsib);
	kbfree(rsib);
	}
if (right)
	{
	con_update(right_id, right);
	kbfree(right);
	}
if (up)
	{
	con_update(up_id, up);
	kbfree(up);
	}
con_update(cid, con);
kbfree(con);
return true;
}

/**************************************************
*						NODE_MOVE_RIGHT
* FUN:	node_move_right
* SUBJ:	Move node one to the right in its list.
* CR:		02/13/07 AM.
* RET:	bool - if moved ok.
* NOTE:	Similar to above, but need to fix up atom and phrases.
*			(If moving from first node of phrase, need to seek phrase among phrases....tedious)
* OPT:	Placeholder for optimized treatment.
*
**************************************************/

bool AQKBM::node_move_right(char *id_s)
{
return true;
}




/**************************************************
*						S_TO_PKIND
* FUN:	s_to_pkind
* SUBJ:	Convert string to PKIND.
* CR:	10/12/95 AM.
* RET:	ok - true if a valid PKIND given.
*		(up) pk - PKIND corresponding to given string.
* NOTE:	Used in user interface, eg.
*
**************************************************/

bool AQKBM::s_to_pkind(
	_TCHAR *str,
	/*UP*/
	XPKIND *pk
	)
{
if (!str || !*str)
   return(false);
else if (!_tcscmp(str, _T("no_ptr")))
   *pk = pNO_PTR;
else if (!_tcscmp(str, _T("pcon")))
   *pk = pXCON;
else if (!_tcscmp(str, _T("pptr")))
   *pk = pXPTR;
else if (!_tcscmp(str, _T("psym")))
   *pk = pXSTR;
else if (!_tcscmp(str, _T("pst")))
   *pk = pXSTR;
else if (!_tcscmp(str, _T("pnum")))
   *pk = pNUM;
else if (!_tcscmp(str, _T("pchar")))
   *pk = pCHAR;
else if (!_tcscmp(str, _T("pfloat")))											// 08/16/01 AM.
	*pk = pFLOAT;																// 08/16/01 AM.
else
   {
   _ftprintf(stderr, _T("[s_to_pkind: Bad str=%s]\n"), str);
   return(false);
   }
return(true);
}


/**************************************************
*						S_TO_PVAL
* FUN:	s_to_pval
* SUBJ:	Convert string to a PVAL.
* CR:	10/12/95 AM.
* RET:	ok - true if a valid PVAL given.
*		(up) pv - PVAL corresponding to given string.
* NOTE:	Used in user interface, eg.
*		This stuff is tedious, since we're allowing
*		lots of types, and have to do type checking.
*
**************************************************/

bool AQKBM::s_to_pval(
	_TCHAR *str,
	XPKIND pkind,
	/*UP*/
	XPVAL *pv
	)
{
long num;
bool ok;
float fnum;				// 08/16/01 AM.
STR_ID sid;

if (!str || !*str)
   return(false);

switch (pkind)
   {
   case pXCON:
      ok = s_to_l(str, &num);
      if (!ok)
         return(false);
      pv->xcon = num;
      break;
   case pXPTR:
      _ftprintf(stderr, _T("[s_to_pval: PTR value unimplemented.]\n"));
      return(false);
   case pXSTR:
		sid = getStrx(str);		// Get string's index.
		if (!sid)	// If absent, add string.
			sid = addString(str);
      if (!sid)
         return(false);
      pv->xstr = sid;
      break;
   case pNUM:
      ok = s_to_l(str, &num);
      if (!ok)
         return(false);
      pv->vnum = num;
      break;
   case pFLOAT:
      ok = s_to_f(str, &fnum);											// 08/16/01 AM.
      if (!ok)																	// 08/16/01 AM.
         return(false);														// 08/16/01 AM.
      pv->vfloat = fnum;													// 08/16/01 AM.
      break;
   case pCHAR:
      if (!str || !*str || *(str+1))
         return(false);
      pv->vchar = *str;
      break;
   case pNO_PTR:      
   default:
      _ftprintf(stderr, _T("[s_to_pval: Bad pkind=%d]\n"), pkind);
      return(false);
   }
return(true);
}


/**************************************************
*						MAKE_PHRASE
* FUN:	make_phrase
* SUBJ:	Make first node of phrase.
* CR:		02/14/07 AM.
* RET:	nid - Created phrase node's id.
* OPT:	Creating the node structure, then returning just the id.  Not as efficient as possible.
* NOTE:	Assuming just one phrase per concept, though hooks are in for more.
* DES:	If no phrases, requiring ptr pointing to first node to be removed as well.
**************************************************/

CON_ID AQKBM::make_phrase(
	XCON_S *c_owner,	// Owning concept for phrase.
	CON_ID i_target	// Node created will be a proxy or representative of this concept.
	)
{
if (!c_owner || !i_target)
	return 0;

CON_ID i_owner = c_owner->id;

// Consistency check. If concept already has a phrase, error.
// (Disallowing dangling ptr that points to nothing.)
if (c_owner->phrases)
	return 0;

// Make concept to be the first node.
XCON_S node_s;
XCON_S *node = &node_s;
node->kind = cXPROXY;
node->v.vcon = i_target;	// Node points to its "target".
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = 0;
node->next = 0;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

// Add node to db and get its id.
CON_ID nid = addXcon(node);

// Make a ptr to point to the first node.
PTR_ID pid = addXptr(nid, pXCON, 0);

// Owner points to ptr for phrase.
c_owner->phrases = pid;
con_update(i_owner, c_owner);	// Update changed owner to db.

return nid;
}


/**************************************************
*						MAKE_PHRASE
* FUN:	make_phrase
* SUBJ:	Install first node of phrase. (VARIANT)
* CR:		02/20/07 AM.
* RET:	nid - Given phrase node's id.
* NOTE:	Assumes node struct is properly detached from its former location, if any.
**************************************************/

CON_ID AQKBM::make_phrase(
	XCON_S *c_owner,	// Owning concept for phrase.
	XCON_S *node		// Node to install as first node of phrase.
	)
{
if (!c_owner || !node)
	return 0;

if (node->kind != cXPROXY)
	return 0;	// Must be a node, not any old concept.

CON_ID i_owner = c_owner->id;

// Consistency check. If concept already has a phrase, error.
// (Disallowing dangling ptr that points to nothing.)
if (c_owner->phrases)
	return 0;

CON_ID nid = node->id;

// Set up node as first phrase node.
// Make concept to be the first node.
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = 0;
node->next = 0;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

con_update(nid, node);	// Update to database.

// Make a ptr to point to the first node.
PTR_ID pid = addXptr(nid, pXCON, 0);

// Owner points to ptr for phrase.
c_owner->phrases = pid;
con_update(i_owner, c_owner);	// Update changed owner to db.

return nid;
}


/**************************************************
*						EXTEND_PHRASE
* FUN:	extend_phrase
* SUBJ:	Make and add node to last node of phrase.
* CR:		02/14/07 AM.
* RET:	nid - Created phrase node's id.
* OPT:	Creating the node structure, then returning just the id.  Not as efficient as possible.
* NOTE:	Assuming just one phrase per concept, though hooks are in for more.
* DES:	If no phrases, requiring ptr pointing to first node to be removed as well.
**************************************************/

CON_ID AQKBM::extend_phrase(
	XCON_S *n_last,	// Current last node of phrase.
	CON_ID i_target	// Node created will be a proxy or representative of this concept.
	)
{
if (!n_last || !i_target)
	return 0;

CON_ID i_last = n_last->id;
CON_ID i_owner = n_last->atom;

// Consistency check. If node has a next, then error.
if (n_last->next)
	return 0;

// Make concept to be the next node.
XCON_S node_s;
XCON_S *node = &node_s;
node->kind = cXPROXY;
node->v.vcon = i_target;	// Node points to its "target".
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = i_last;
node->next = 0;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

// Add node to db and get its id.
CON_ID nid = addXcon(node);

// Fix last node to point to new last node.
n_last->next = nid;
con_update(i_last, n_last);

return nid;
}


/**************************************************
*						EXTEND_PHRASE
* FUN:	extend_phrase
* SUBJ:	Add given node as last node of phrase. (VARIANT)
* CR:		02/20/07 AM.
* RET:	nid - Given phrase node's id.
**************************************************/

CON_ID AQKBM::extend_phrase(
	XCON_S *n_last,	// Current last node of phrase.
	XCON_S *node		// Node to install as new last node.
	)
{
if (!n_last || !node)
	return 0;
if (node->kind != cXPROXY)
	return 0;

CON_ID i_last = n_last->id;
CON_ID i_owner = n_last->atom;

// Consistency check. If node has a next, then error.
if (n_last->next)
	return 0;

CON_ID nid = node->id;

// Set up node as last phrase node.
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = i_last;
node->next = 0;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

con_update(nid, node);	// Update to database.

// Fix last node to point to new last node.
n_last->next = nid;
con_update(i_last, n_last);

return nid;
}


/**************************************************
*						PHRASE_ADD_START
* FUN:	phrase_add_start
* SUBJ:	Add node to the beginning of the phrase.
* CR:		02/19/07 AM.
* RET:	nid - Created phrase node's id.
* OPT:	Creating the node structure, then returning just the id.  Not as efficient as possible.
* NOTE:	Assuming just one phrase per concept, though hooks are in for more.
**************************************************/

CON_ID AQKBM::phrase_add_start(
	XCON_S *c_owner,	// Owning concept for phrase.
	CON_ID i_target	// Node created will be a proxy or representative of this concept.
	)
{
if (!c_owner || !i_target)
	return 0;

CON_ID i_owner = c_owner->id;

PTR_ID pid = c_owner->phrases;

// If adding first node, just make the phrase from scratch.
if (!pid)
	return make_phrase(c_owner, i_target);

XPTR_S *ptr = 0;
if (pid)
	getXptr(pid, /*DU*/ ptr);
if (ptr->kind != pXCON)
	{
	kbfree(ptr);
	return 0;	// Error.
	}
CON_ID head_cid = ptr->v.xcon;
if (!head_cid)
	{
	kbfree(ptr);
	return 0;	// Error.
	}
XCON_S *head_con = Con(head_cid);
if (!head_con)
	{
	kbfree(ptr);
	return 0;
	}
// Ok, got the ptr and the first node.

// Make concept to be the first node.
XCON_S node_s;
XCON_S *node = &node_s;
node->kind = cXPROXY;
node->v.vcon = i_target;	// Node points to its "target".
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = 0;
node->next = head_cid;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

// Add node to db and get its id.
CON_ID nid = addXcon(node);

// Update pointer to point to new first node.
ptr->v.xcon = nid;
ptr_update(pid, ptr);

// Update the old head to be 2nd node.
head_con->prev = nid;
con_update(head_cid, head_con);

kbfree(ptr);
kbfree(head_con);

return nid;
}


/**************************************************
*						PHRASE_ADD_START
* FUN:	phrase_add_start
* SUBJ:	Add node to the beginning of the phrase. (VARIANT)
* CR:		02/20/07 AM.
* RET:	nid - Given phrase node's id.
**************************************************/

CON_ID AQKBM::phrase_add_start(
	XCON_S *c_owner,	// Owning concept for phrase.
	XCON_S *node		// Node to install as first.
	)
{
if (!c_owner || !node)
	return 0;

if (node->kind != cXPROXY)
	return 0;

CON_ID i_owner = c_owner->id;

PTR_ID pid = c_owner->phrases;

// If adding first node, just make the phrase from scratch.
if (!pid)
	return make_phrase(c_owner, node);

XPTR_S *ptr = 0;
if (pid)
	getXptr(pid, /*DU*/ ptr);
if (ptr->kind != pXCON)
	{
	kbfree(ptr);
	return 0;	// Error.
	}
CON_ID head_cid = ptr->v.xcon;
if (!head_cid)
	{
	kbfree(ptr);
	return 0;	// Error.
	}
XCON_S *head_con = Con(head_cid);
if (!head_con)
	{
	kbfree(ptr);
	return 0;
	}
// Ok, got the ptr and the first node.

CON_ID nid = node->id;

// Fix up given node to be first.
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = 0;
node->next = head_cid;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

con_update(nid, node);

// Update pointer to point to new first node.
ptr->v.xcon = nid;
ptr_update(pid, ptr);

// Update the old head to be 2nd node.
head_con->prev = nid;
con_update(head_cid, head_con);

kbfree(ptr);
kbfree(head_con);

return nid;
}


/**************************************************
*						PHRASE_INSERT_AFTER
* FUN:	phrase_insert_after
* SUBJ:	Make and add node after given node.
* CR:		02/19/07 AM.
* RET:	nid - Created phrase node's id.
* NOTE:	See comments in extend_phrase.
**************************************************/

CON_ID AQKBM::phrase_insert_after(
	XCON_S *n_pre,		// Node before the insertion.
	CON_ID i_target	// Node created will be a proxy or representative of this concept.
	)
{
if (!n_pre || !i_target)
	return 0;

CON_ID i_pre = n_pre->id;
CON_ID i_owner = n_pre->atom;

// Fetch the node AFTER the insert.
CON_ID i_post = n_pre->next;
XCON_S *n_post = 0;
if (i_post)
	n_post = Con(i_post);

// Make concept to be the next node.
XCON_S node_s;
XCON_S *node = &node_s;
node->kind = cXPROXY;
node->v.vcon = i_target;	// Node points to its "target".
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = i_pre;
node->next = i_post;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

// Add node to db and get its id.
CON_ID nid = addXcon(node);

// Fix up the after node.
if (n_post)
	{
	n_post->prev = nid;
	con_update(i_post, n_post);
	}

// Fix up the before node.
n_pre->next = nid;
con_update(i_pre, n_pre);

kbfree(n_post);

return nid;
}


/**************************************************
*						PHRASE_INSERT_AFTER
* FUN:	phrase_insert_after
* SUBJ:	Add node after given phrase node. (VARIANT)
* CR:		02/20/07 AM.
* RET:	nid - Added phrase node's id.
**************************************************/

CON_ID AQKBM::phrase_insert_after(
	XCON_S *n_pre,		// Node before the insertion.
	XCON_S *node		// Node to add after the pre node.
	)
{
if (!n_pre || !node)
	return 0;

if (node->kind != cXPROXY)
	return 0;

CON_ID i_pre = n_pre->id;
CON_ID i_owner = n_pre->atom;

// Fetch the node AFTER the insert.
CON_ID i_post = n_pre->next;
XCON_S *n_post = 0;
if (i_post)
	n_post = Con(i_post);

CON_ID nid = node->id;

// Fix up given node to be the next node.
node->flags = 0;
node->attrs = 0;
node->up = 0;
node->dn = 0;
node->prev = i_pre;
node->next = i_post;
node->atom = i_owner;	// Node points back to concept that owns it.
node->phrases = 0;		// Node can't have its own phrases.

con_update(nid, node);

// Fix up the after node.
if (n_post)
	{
	n_post->prev = nid;
	con_update(i_post, n_post);
	}

// Fix up the before node.
n_pre->next = nid;
con_update(i_pre, n_pre);

kbfree(n_post);

return nid;
}


/**************************************************
*						PHRASE_DETACH_NODE
* FUN:	phrase_detach_node
* SUBJ:	Detach given node from its phrase.
* CR:		02/21/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::phrase_detach_node(XCON_S *node)
{
if (!node)
	return true;	// Empty is ok.

// Require a node type.
if (node->kind != cXPROXY)
	return false;

XCON_S *prev = Con(node->prev);	// Get prev node.
XCON_S *next = Con(node->next);	// Get next node.

// Fix up connections.
if (next)
	{
	next->prev = node->prev;
	con_update(next->id, next);
	}
if (prev)
	{
	prev->next = node->next;
	con_update(prev->id, prev);
	}

node->prev = node->next = 0;	// Detach.
con_update(node->id, node);

if (!prev)	// First node. Special case.
	{
	// Get owner and fix him up.
	XCON_S *owner = Con(node->atom);
	XPTR_S *ptr;
	getXptr(owner->phrases, /*UP*/ ptr);
	ptr->v.xcon = next->id;
	ptr_update(owner->phrases, ptr);
	kbfree(owner);
	kbfree(ptr);
	}

if (prev)
	kbfree(prev);
if (next)
	kbfree(next);
return true;
}


/**************************************************
*						PHRASE_RM_NODE
* FUN:	phrase_rm_node
* SUBJ:	Remove node from its phrase and delete it.
* CR:		02/21/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::phrase_rm_node(XCON_S *node)
{
// Detach node from phrase.
if (!phrase_detach_node(node))
	return false;

// Remove the node itself from kb.
node_rm(node);

return true;
}


/**************************************************
*						NODE_RM
* FUN:	node_rm
* SUBJ:	Delete node, with no attention to its phrase.
* CR:		02/21/07 AM.
* RET:	bool - if removed ok.
*
**************************************************/

bool AQKBM::node_rm(XCON_S *node)
{
if (!node)
	return true;	// Empty is ok.
CON_ID nid = node->id;

if (node->kind != cXPROXY)
	return false;

// Node has no children.
// Node has no phrases.

// Remove attributes.
con_rm_attrs(node);

// REMOVE CONCEPT ITSELF FROM KB HERE.
int arr[2]; char *x; char *id_s;	// Nasty.
arr[0] = nid; x = (char *) &(arr[1]); *x = '\0'; id_s = (char *) arr;
xcon_->rmXcon(id_s);

return true;
}

/**************************************************
*						ATTR_GET_LAST_PTR
* FUN:	attr_get_last_ptr
* SUBJ:	Find/make the last ptr for an attribute. Internal use.
* CR:		02/15/07 AM.
* RET:	ptr - Fetch of ptr, whether created or not.
* OPT:	If absent, make an empty attr slot.
*			If present, find last value for quick append.
* WARN:	USER FREES THE RETURNED PTR STRUCT.
*			May create attr slot with empty values.
**************************************************/

XPTR_S *AQKBM::attr_get_last_ptr(
	XCON_S *con,
	XCON_S *slot,
	/*UP*/
	PTR_ID &pid
	)
{
pid = 0;

if (!con || !slot)
	return 0;

PTR_ID attrs = con->attrs;
CON_ID slot_cid = slot->id;

// No attrs at all.  So build attrs framework and return slot ptr.
if (!attrs)
	{
	// OPT: Build backwards!
	// Build slot pointer.
	PTR_ID slot_pid;
	XPTR_S *slot_ptr = addXptr(slot_cid,pXCON,0,/*UP*/slot_pid);
	// Build attr pointer.
	PTR_ID attr_pid = addXptr(slot_pid,pXPTR,0);
	// Point from concept to attr.
	con->attrs = attr_pid;
	con_update(con->id, con);

	pid = slot_pid;	// UP. Return id also.
	return slot_ptr;
	}

// Chain down attrs looking for match.
// OPT:	Looking for slot by its id, efficiently.
// If found, get its last ptr.
// Else, build new attr at end of chain.
XPTR_S *hptr, *slotptr;
PTR_ID next, slot_pid, val;
XPTR_S *hlast = 0;
PTR_ID hlast_pid = 0;
CON_ID cid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;
	if (!next)
		{
		hlast = hptr;	// Save in case we need it.
		hlast_pid = attrs;
		}
	else
		kbfree(hptr);

	// SLOT IS A CONCEPT, NOT A STRING. // 02/15/07 AM.
	getXptr(slot_pid, /*DU*/ slotptr);
	cid = slotptr->v.xcon;
	val = slotptr->next;	// In case this is the right one.

	if (cid == slot_cid)	// Found matching slot-concept id.
		{
		if (!val)	// If slot with no values.
			{
			if (hlast)
				kbfree(hlast);
			pid = slot_pid;	// UP. Return id also.
			return slotptr;	// Return slot pointer as last item in this attr.
			}

		// Chain down this attr's pointers to the end.
		XPTR_S *val_ptr = 0;
		while (val)
			{
			pid = val;	// UP. Track the id of last ptr.
			if (getXptr(val, /*DU*/ val_ptr))	// Ok ptr fetch.
				val = val_ptr->next;
			else	// Error.
				val = 0;
			if (!val)	// Found last one.
				break;
			kbfree(val_ptr);	// Not the last one.
			val_ptr = 0;
			}
		if (hlast)
			kbfree(hlast);
		return val_ptr;
		}

	kbfree(slotptr);
	attrs = next;
	}

if (!hlast)	// Coding error.
	return 0;

// Didn't find attr, so build it at the end of attrs list.

// OPT: Build backwards!
// Build slot pointer.
slot_pid = 0;
XPTR_S *slot_ptr = addXptr(slot_cid,pXCON,0,/*UP*/slot_pid);
// Build attr pointer.
PTR_ID attr_pid = addXptr(slot_pid,pXPTR,0);
// Point from old last attr to new last attr.
hlast->next = attr_pid;
ptr_update(hlast_pid, hlast);

kbfree(hlast);
pid = slot_pid;	// UP. Id of last ptr.
return slot_ptr;
}


/**************************************************
*						ATTR_GET_LAST_PTR
* FUN:	attr_get_last_ptr
* SUBJ:	Find/make the last ptr for an attribute. Internal use. [VARIANT]
* CR:		02/16/07 AM.
* RET:	ptr - Fetch of ptr, whether created or not.
* OPT:	If absent, make an empty attr slot.
*			If present, find last value for quick append.
* WARN:	USER FREES THE RETURNED PTR STRUCT.
*			May create attr slot with empty values.
*			Variant uses STRING as name of attribute slot.
**************************************************/

XPTR_S *AQKBM::attr_get_last_ptr(
	XCON_S *con,
	STR_ID slot_sid,
	/*UP*/
	PTR_ID &pid
	)
{
pid = 0;

if (!con || !slot_sid)
	return 0;

PTR_ID attrs = con->attrs;

// No attrs at all.  So build attrs framework and return slot ptr.
if (!attrs)
	{
	// OPT: Build backwards!
	// Build slot pointer.
	PTR_ID slot_pid;
	XPTR_S *slot_ptr = addXptr(slot_sid,pXSTR,0,/*UP*/slot_pid);
	// Build attr pointer.
	PTR_ID attr_pid = addXptr(slot_pid,pXPTR,0);
	// Point from concept to attr.
	con->attrs = attr_pid;
	con_update(con->id, con);

	pid = slot_pid;	// UP. Return id also.
	return slot_ptr;
	}

// Chain down attrs looking for match.
// OPT:	Looking for slot by its id, efficiently.
// If found, get its last ptr.
// Else, build new attr at end of chain.
XPTR_S *hptr, *slotptr;
PTR_ID next, slot_pid, val;
XPTR_S *hlast = 0;
PTR_ID hlast_pid = 0;
STR_ID sid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	if (!hptr)	// Sanity.
		return false;	// 03/11/07 AM.
	slot_pid = hptr->v.xptr;
	next = hptr->next;
	if (!next)
		{
		hlast = hptr;	// Save in case we need it.
		hlast_pid = attrs;
		}
	else
		kbfree(hptr);

	// LOOKING FOR SLOT BY STRING. // 02/16/07 AM.
	getXptr(slot_pid, /*DU*/ slotptr);
	sid = slot_sid(slotptr);		// 02/16/07 AM.
	val = slotptr->next;	// In case this is the right one.

	if (sid == slot_sid)	// Found matching slot-name id.
		{
		if (!val)	// If slot with no values.
			{
			if (hlast)
				kbfree(hlast);
			pid = slot_pid;	// UP. Return id also.
			return slotptr;	// Return slot pointer as last item in this attr.
			}
		kbfree(slotptr);	// FIX.	// 03/19/07 AM.

		// Chain down this attr's pointers to the end.
		XPTR_S *val_ptr = 0;
		while (val)
			{
			pid = val;	// UP. Track the id of last ptr.
			if (getXptr(val, /*DU*/ val_ptr))	// Ok ptr fetch.
				val = val_ptr->next;
			else	// Error.
				val = 0;
			if (!val)	// Found last one.
				break;
			kbfree(val_ptr);	// Not the last one.
			val_ptr = 0;
			}
		if (hlast)
			kbfree(hlast);
		return val_ptr;
		}

	kbfree(slotptr);
	attrs = next;
	}

if (!hlast)	// Coding error.
	return 0;

// Didn't find attr, so build it at the end of attrs list.

// OPT: Build backwards!
// Build slot pointer.
slot_pid = 0;
XPTR_S *slot_ptr = addXptr(slot_sid,pXSTR,0,/*UP*/slot_pid);
// Build attr pointer.
PTR_ID attr_pid = addXptr(slot_pid,pXPTR,0);
// Point from old last attr to new last attr.
hlast->next = attr_pid;
ptr_update(hlast_pid, hlast);

kbfree(hlast);
pid = slot_pid;	// UP. Id of last ptr.
return slot_ptr;
}


/**************************************************
*						ATTR_ADD_VAL
* FUN:	attr_add_val
* SUBJ:	Extend pointer's values with new value. Internal use.
* CR:		02/15/07 AM.
* RET:	ptr - Fetch of ptr for new value.
* WARN:	USER FREES THE RETURNED PTR STRUCT.
**************************************************/

XPTR_S *AQKBM::attr_add_val(
	XPTR_S *last,		// Ptr to last value of this attribute (or slot-ptr if no vals yet).
	PTR_ID last_pid,	// Unique id of last ptr.
	XPVAL &val,			// Unioned value.
	XPKIND kind,		// Kind of ptr value.
	/*UP*/
	PTR_ID &val_pid			// Return id of new ptr for val.
	)
{
val_pid = 0;
if (!last || !kind)
	return 0;

// Make ptr for new value.
XPTR_S ptr_s;	// New ptr for update.
ptr_s.v = val;
ptr_s.kind = kind;
ptr_s.next = 0;
// Add and re-fetch ptr.
XPTR_S *val_ptr = addXptr(&ptr_s, /*UP*/ val_pid);
if (!val_pid)
	return 0;

// Point to it.
last->next = val_pid;			// Point to new last ptr.
ptr_update(last_pid, last);	// Update the old last ptr.

return val_ptr;
}



/**************************************************
*						ATTR_PREFIND_H
* FUN:	attr_prefind_h
* SUBJ:	Find specified attr of given concept.
* CR:		07/12/99 AM.
* RET:	Pointer to attribute handle of PRIOR attribute.
*			UP first - if attr is first in list.
*			UP pid - pointer id for the previous ptr.
* NOTE:	Return first argument that matches.
*			'h' is short for handle.
* WARN:	USER FREES THE RETURNED PTR STRUCT.
*
**************************************************/

XPTR_S *AQKBM::attr_prefind_h(
	XCON_S *con,
	_TCHAR *name,
	/*UP*/
	bool &first,
	PTR_ID &pre_pid,	// Pre handle id.
	PTR_ID &h_pid,		// Handle id.
	PTR_ID &slot_pid	// Slot id.
	)
{
first = false;
pre_pid = h_pid = slot_pid = 0;
if (!con || !name || !*name)
	return 0;

PTR_ID attrs_pid;

if (!(attrs_pid = con->attrs))
	return 0;

STR_ID slot_sid = getStrx(name);
if (!slot_sid)
	return 0;

XPTR_S *attrs;
if (!getXptr(attrs_pid, /*DU*/ attrs))
	return 0;

// First is special.  See if slot matches.

PTR_ID spid = attrs->v.xptr;
XPTR_S *slot;
if (!getXptr(spid, /*DU*/ slot))
	{
	kbfree(attrs);
	return 0;
	}

if (slot_sid(slot) == slot_sid)	// First slot is the one sought.
	{
	first = true;
	kbfree(attrs);
	kbfree(slot);
	h_pid = attrs_pid;	// UP. handle id.
	slot_pid = spid;		// UP. slot id.
	return 0;	// Attr must be removed via concept's pointer to it.
	}
kbfree(slot);

// Traverse looking for attr and keeping previous attr's pointer.
XPTR_S *pre = attrs;	// Keep track of previous attr (handle).
PTR_ID p_pid = attrs_pid;
while (attrs_pid = pre->next)
	{
	// See if current attr is the one sought.
	if (!getXptr(attrs_pid, /*DU*/ attrs))
		{
		kbfree(pre);
		return 0;
		}
	spid = attrs->v.xptr;
	if (!getXptr(spid, /*DU*/ slot))
		{
		kbfree(pre);
		kbfree(attrs);
		return 0;
		}
	if (slot_sid(slot) == slot_sid)	// Found the slot.
		{
		kbfree(attrs);
		kbfree(slot);	// FIX.	// 03/21/07 AM.
		pre_pid = p_pid;		// UP. pre handle id.
		h_pid = attrs_pid;	// UP. handle id.
		slot_pid = spid;		// UP. slot id.
		return pre;
		}
	kbfree(slot);

	// Current isn't it, so move forward.
	kbfree(pre);
	pre = attrs;
	p_pid = attrs_pid;
	}

// Attribute not found.
if (pre)
	kbfree(pre);
return 0;
}


/**************************************************
*						ATTR_PREFIND_H
* FUN:	attr_prefind_h
* SUBJ:	Find specified attribute for deleting.  VARIANT.
* CR:		07/12/99 AM.
* RET:	Pointer to attribute handle of PRIOR attribute.
*			UP first - if attr is first in list.
*			UP pid - pointer id for the previous ptr.
* NOTE:	'h' is short for handle.
* WARN:	USER FREES THE RETURNED PTR STRUCT.
*
**************************************************/

XPTR_S *AQKBM::attr_prefind_h(
	XCON_S *con,
	PTR_ID pid,	// ID OF THE SOUGHT ATTRIBUTE HANDLE.
	/*UP*/
	bool &first,
	PTR_ID &pre_pid
	)
{
first = false;
pre_pid = 0;
if (!con || !pid)
	return 0;

PTR_ID attrs_pid;

if (!(attrs_pid = con->attrs))
	return 0;

// First is special.  See if slot matches.
if (pid == attrs_pid)	// First slot is the one sought.
	{
	first = true;
	return 0;	// Attr must be removed via concept's pointer to it.
	}

XPTR_S *attrs;
if (!getXptr(attrs_pid, /*DU*/ attrs))
	return 0;

// Traverse looking for attr and keeping previous attr's pointer.
XPTR_S *pre = attrs;	// Keep track of previous attr.
PTR_ID p_pid = attrs_pid;
while (attrs_pid = pre->next)
	{
	// See if current attr is the one sought.
	if (pid == attrs_pid)	// Found the slot.
		{
		pre_pid = p_pid;	// Return the id also.
		return pre;
		}

	if (!getXptr(attrs_pid, /*DU*/ attrs))
		{
		kbfree(pre);
		return 0;
		}

	// Current isn't it, so move forward.
	kbfree(pre);
	pre = attrs;
	p_pid = attrs_pid;
	}

// Attribute not found.
if (pre)
	kbfree(pre);
return 0;
}



/**************************************************
*						ATTRS_RM
* FUN:	attrs_rm
* SUBJ:	Remove all attributes of a concept.
* CR:		10/31/00 AM.
* NOTE:	QDBM reimplementation.  No more worries about sys attributes.
*
**************************************************/

bool AQKBM::attrs_rm(
	CON_ID con_id,
	/*UP*/
	bool &dirty		// If kb has changed.
   )
{
dirty = false;

if (!con_id)
	return false;

XCON_S *con = Con(con_id);
if (!con)
	return false;

PTR_ID attrs_pid = con->attrs;
if (!attrs_pid)
	{
	kbfree(con);	// Free copy of concept.
	return true;
	}

// Remove concept's pointer to attributes.
dirty = true;
con->attrs = 0;
if (!con_update(con->id, con))
	{
	kbfree(con);
	return false;
	}
kbfree(con);	// FIX.	// 03/21/07 AM.

XPTR_S *attrs;
if (!getXptr(attrs_pid, /*UP*/ attrs))
	return false;

// Traverse, freeing ptrs and their values.
PTR_ID next;
XPTR_S *ptr;
PTR_ID pid, p_next;
while (attrs)
	{
	next = attrs->next;
	pid = attrs->v.xptr;			// Slot id.
	kbfree(attrs);					// Free copy of handle.
	xptr_->rmXptr(attrs_pid);	// Free handle from db.

	// Free slot and values.
	while (pid)
		{
		// Get struct.
		if (!getXptr(pid, /*UP*/ ptr))
			return false;
		p_next = ptr->next;
		kbfree(ptr);			// Free copy of ptr.
		xptr_->rmXptr(pid);	// Free ptr from db.
		pid = p_next;			// Move to next guy.
		}


	// Go onto the next attr.
	if (!next)
		return true;
	if (!getXptr(next, /*UP*/ attrs))
		return false;
	}
return true;
}

/**************************************************
*						PTR_LIST_RM
* FUN:	ptr_list_rm
* SUBJ:	Remove list of pointers on next field.
* CR:		02/16/07 AM.
*
**************************************************/

bool AQKBM::ptr_list_rm(
	PTR_ID pid,
	XPTR_S *ptr	// CAN BE NULL.  If present, no need to fetch.
	)
{
if (!pid)
	return false;

PTR_ID next_pid;

// Treat the first one specially.
if (!ptr)	// If need to fetch.
	{
	if (!getXptr(pid, /*UP*/ ptr))
		return false;
	next_pid = ptr->next;
	kbfree(ptr);
	}
else	// Caller's ptr.
	next_pid = ptr->next;
xptr_->rmXptr(pid);

while (pid = next_pid)
	{
	if (!getXptr(pid, /*UP*/ ptr))
		return false;
	next_pid = ptr->next;

	kbfree(ptr);			// Free copy of ptr.
	xptr_->rmXptr(pid);	// Free ptr from db.
	}

return true;
}



/**************************************************
*						ATTR_FIND
* FUN:	attr_find
* SUBJ:	Find value(s) for given slot of given concept.
* CR:		08/12/99 AM.
* RET:	Pointer to first value.
*
**************************************************/

PTR_ID AQKBM::attr_find(CON_ID cid, _TCHAR *s_slot)
{
if (!cid || !s_slot || !*s_slot)
	return 0;

STR_ID slot_sid = getStrx(s_slot);
if (!slot_sid)
	return 0;

XCON_S *con = Con(cid);
if (!con)
	return 0;

PTR_ID attrs = con->attrs;
kbfree(con);	// Free copy of con.
if (!attrs)
	return 0;

// Chain down attrs looking for match.
// OPT:	Looking for slot by its id, efficiently.
// If found, get its first value.
XPTR_S *hptr, *slotptr;
PTR_ID next, slot_pid, val;
STR_ID sid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;
	kbfree(hptr);

	// LOOKING FOR SLOT BY STRING. // 02/16/07 AM.
	getXptr(slot_pid, /*DU*/ slotptr);
	sid = slot_sid(slotptr);		// 02/16/07 AM.
	val = slotptr->next;	// In case this is the right one.
	kbfree(slotptr);

	if (sid == slot_sid)	// Found matching slot-name id.
		return val;	// Return ptr to first val, if any.
	attrs = next;
	}
return 0;	// Didn't find attribute.
}


/**************************************************
*						ATTR_FIND_SLOT
* FUN:	attr_find_slot
* SUBJ:	Find given slot of given concept, if present.
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
* NOTE:	02/17/07 AM. REIMPLEMENT.
* WARN:	CALLER MUST FREE RETURNED PTR.
*
**************************************************/

XPTR_S *AQKBM::attr_find_slot(XCON_S *con, CON_ID slot_cid, /*UP*/ PTR_ID &pid)
{
pid = 0;

if (!con || !slot_cid)
	return 0;

PTR_ID attrs = con->attrs;
if (!attrs)
	return 0;

// Chain down attrs looking for match.
XPTR_S *hptr, *slotptr;
PTR_ID next, slot_pid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;
	kbfree(hptr);

	// LOOKING FOR SLOT BY CONCEPT.
	getXptr(slot_pid, /*DU*/ slotptr);
	if (slotptr->kind == pXCON && slot_cid == slotptr->v.xcon)	// Found match.
		{
		pid = slot_pid;	// UP id of slot ptr.
		return slotptr;
		}
	kbfree(slotptr);

	attrs = next;
	}
return 0;	// Didn't find attribute.
}


/**************************************************
*						ATTR_FIND_SLOT
* FUN:	attr_find_slot
* SUBJ:	Find given slot of given concept, if present. [VARIANT]
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
* WARN:	CALLER MUST FREE RETURNED PTR.
*
**************************************************/

XPTR_S *AQKBM::attr_find_slot_s(XCON_S *con, STR_ID slot_sid, /*UP*/ PTR_ID &pid)
{
pid = 0;

if (!con || !slot_sid)
	return 0;

PTR_ID attrs = con->attrs;
if (!attrs)
	return 0;

// Chain down attrs looking for match.
XPTR_S *hptr, *slotptr;
PTR_ID next, slot_pid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;
	kbfree(hptr);

	// LOOKING FOR SLOT BY STRING ID.
	getXptr(slot_pid, /*DU*/ slotptr);
	if (slot_sid == slot_sid(slotptr))	// Found match.
		{
		pid = slot_pid;	// UP id of slot ptr.
		return slotptr;
		}
	kbfree(slotptr);

	attrs = next;
	}
return 0;	// Didn't find attribute.
}


/**************************************************
*						ATTR_FIND_H
* FUN:	attr_find_h
* SUBJ:	Find specified attr of given concept.
* CR:		07/12/99 AM.
* RET:	Pointer to attribute handle.
* NOTE:	Return first argument that matches.
*			'h' is short for handle.
* WARN:	CALLER MUST FREE RETURNED OBJECT.
*
**************************************************/

XPTR_S *AQKBM::attr_find_h(XCON_S *con, _TCHAR *name,
	/*UP*/
	PTR_ID &pid	// Id of attribute handle.	// 03/08/07 AM.
	)
{
pid = 0;	// 03/08/07 AM.

if (!con || !name || !*name)
	return 0;

STR_ID slot_sid = getStrx(name);
if (!slot_sid)
	return 0;

PTR_ID attrs = con->attrs;
if (!attrs)
	return 0;

// Chain down attrs looking for match.
// OPT:	Looking for slot by its id, efficiently.
// If found, get its first value.
XPTR_S *hptr, *slotptr;
PTR_ID next, slot_pid;
STR_ID sid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;

	// LOOKING FOR SLOT BY STRING. // 02/16/07 AM.
	getXptr(slot_pid, /*DU*/ slotptr);
	sid = slot_sid(slotptr);		// 02/16/07 AM.
	kbfree(slotptr);

	if (sid == slot_sid)	// Found matching slot-name id.
		{
		pid = attrs;	// UP - return id of attribute handle.	// 03/08/07 AM.
		return hptr;	// Return handle ptr.
		}
	kbfree(hptr);		// Else trash this copy of handle.
	attrs = next;
	}
return 0;	// Didn't find attribute.
}


/**************************************************
*						ATTR_GET_H
* FUN:	attr_get_h
* SUBJ:	Find or make specified attr of given concept.
* CR:		05/03/99 AM.
* RET:	Pointer to first value.  02/16/07 AM. NO! Returns handle.
* ARG:	Argument is a string, not a concept.
* NOTE:	Return first argument that matches.
*			'h' is short for handle.
*			REIMPLEMENT.	02/17/07 AM.
* WARN:	USER FREES PTR FOR HANDLE.
*
**************************************************/

XPTR_S *AQKBM::attr_get_h(XCON_S *con, _TCHAR *name,
	/*UP*/ PTR_ID &pid	// 03/19/07 AM.
	)
{
pid = 0;	// 03/19/07 AM.

if (!con || !name || !*name)
	return 0;

STR_ID slot_sid = getStrx(name);
if (!slot_sid)
	return 0;

PTR_ID attrs = con->attrs;
// No attrs is ok, we'll add a first one.

// Chain down attrs looking for match.
// OPT:	Looking for slot by its id, efficiently.
// If found, get its first value.
XPTR_S *hptr, *slotptr;
XPTR_S *hlast = 0;
PTR_ID next, slot_pid, hlast_pid;
STR_ID sid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;

	// LOOKING FOR SLOT BY STRING. // 02/16/07 AM.
	getXptr(slot_pid, /*DU*/ slotptr);
	sid = slot_sid(slotptr);		// 02/16/07 AM.
	kbfree(slotptr);

	if (sid == slot_sid)	// Found matching slot-name id.
		{
		pid = attrs;	// UP. ptr id.	// 03/19/07 AM.
		return hptr;	// Return handle ptr.
		}

	// Else, save the new last handle.
	if (hlast)
		kbfree(hlast);	// Remove the prior.
	hlast = hptr;		// Save the current as last handle.
	hlast_pid = attrs;	// Save pid also.	// 02/19/07 AM.
	attrs = next;
	}

// Didn't find attribute. So make it at the end of attrs.
// Return the handle.
PTR_ID hpid;
return attr_add_slot_h(con, hlast, hlast_pid, slot_sid, /*UP*/ hpid);
}



/**************************************************
*						ATTR_GET_SLOT
* FUN:	attr_get_slot
* SUBJ:	Find or add given slot of given concept.
* CR:		10/9/95 AM.
* RET:	Pointer to slot.
* NOTE:	REIMPLEMENT.	02/17/07 AM.
* WARN:	USER FREES PTR FOR SLOT.
*
**************************************************/

XPTR_S *AQKBM::attr_get_slot(XCON_S *con, CON_ID slot_cid)
{
if (!con || !slot_cid)
	return 0;

PTR_ID attrs = con->attrs;
// No attrs is ok, we'll add a first one.

// Chain down attrs looking for match.
// OPT:	Looking for slot by its id, efficiently.
// If found, get its first value.
XPTR_S *hptr, *slotptr;
XPTR_S *hlast = 0;
PTR_ID next, slot_pid, hlast_pid;
while (attrs)
	{
	getXptr(attrs, /*DU*/ hptr);
	slot_pid = hptr->v.xptr;
	next = hptr->next;

	// LOOKING FOR SLOT BY CONCEPT.
	getXptr(slot_pid, /*DU*/ slotptr);
	if (!slotptr)
		return 0;	// Error.
	if (slotptr->kind == pXCON && slot_cid == slotptr->v.xcon)	// Match.
		{
		if (hlast)
			kbfree(hlast);
		kbfree(hptr);
		return slotptr;
		}
	// Didn't match.
	kbfree(slotptr);

	// Save the new last handle.
	if (hlast)
		kbfree(hlast);	// Remove the prior.
	hlast = hptr;		// Save the current as last handle.
	hlast_pid = attrs;	// 02/19/07 AM.
	attrs = next;
	}

// Didn't find attribute. So make it at the end of attrs.
// Return the slot.
PTR_ID tmpid;
XPTR_S *slot_ptr = attr_add_slot(con, hlast, hlast_pid, slot_cid, /*UP*/ tmpid);
kbfree(hlast);
return slot_ptr;
}

/**************************************************
*						ATTR_ADD_SLOT_H
* FUN:	attr_add_slot_h
* SUBJ:	Make a new attr slot for concept.
* CR:		02/17/07 AM.
* RET:	hptr - Handle for new attr.
*			UP hpid = id for handle.
* NOTE:	Assumes slot doesn't already exist.
*			Assumes that given handle is the last of concept's attrs, if present.
* WARN:	USER FREES PTR FOR HANDLE.
*
**************************************************/

XPTR_S *AQKBM::attr_add_slot_h(
	XCON_S *con,	// Concept to add attr to.
	XPTR_S *hlast,	// Last handle. If 0, add slot at beginning of attrs.
	PTR_ID hlast_pid,	// Id of last handle.	// 02/19/07 AM.
	STR_ID slot_sid,	// String id for slot.
	/*UP*/
	PTR_ID &hpid	// UP. Id of handle.
	)
{
hpid = 0;

if (!con || !slot_sid)
	return 0;

XPTR_S *ptr, *slot_ptr;
PTR_ID pid, slot_pid;

// Build in reverse order, so forward pointers are available...

// Build slot ptr.
slot_ptr = addXptr(slot_sid, pXSTR, 0, /*UP*/ slot_pid);
if (!slot_ptr)
	return 0;
kbfree(slot_ptr);	// Don't need it, returning handle.

// Build handle ptr.
// Point to existing attrs if present.
ptr = addXptr(slot_pid, pXPTR, con->attrs, /*UP*/ pid);
if (!ptr)
	return 0;

if (!hlast || !con->attrs)
	{
	// Add as first attr to concept.
	con->attrs = pid;
	con_update(con->id, con);

	}
else
	{
	// Given the last handle.  Add after that.

	// Fix up the old last handle.
	hlast->next = pid;
	ptr_update(hlast_pid, hlast);
	}

// Everything ok, so return.
hpid = pid;	// UP. Id for handle.
return ptr;
}

/**************************************************
*						ATTR_ADD_SLOT
* FUN:	attr_add_slot
* SUBJ:	Make a new attr slot for concept.
* CR:		02/17/07 AM.
* RET:	slot_ptr - Slot ptr for new attr.
*			UP slot_pid = id for slot.
* NOTE:	Assumes slot doesn't already exist.
*			Assumes that given handle is the last of concept's attrs, if present.
* WARN:	USER FREES PTR FOR HANDLE.
*
**************************************************/

XPTR_S *AQKBM::attr_add_slot(
	XCON_S *con,	// Concept to add attr to.
	XPTR_S *hlast,	// Last handle. If 0, add slot at beginning of attrs.
	PTR_ID hlast_pid,	// Id of last handle.
	CON_ID slot_cid,	// CONCEPT id for slot.
	/*UP*/
	PTR_ID &slot_pid	// UP. Id of new slot.
	)
{
slot_pid = 0;

if (!con || !slot_cid)
	return 0;

// Build in reverse order, so forward pointers are available...
XPTR_S *hptr, *sptr;
PTR_ID hpid, spid;

// Build slot ptr.
sptr = addXptr(slot_cid, pXCON, 0, /*UP*/ spid);
if (!sptr)
	return 0;

// Build handle ptr.
// Point to existing attrs if present.
hptr = addXptr(spid, pXPTR, con->attrs, /*UP*/ hpid);
if (!hptr)
	return 0;

if (!hlast || !con->attrs)
	{
	// Add as first attr to concept.
	con->attrs = hpid;
	con_update(con->id, con);
	}
else
	{
	// Given the last handle.  Add after that.

	// Fix up the old last handle.
	hlast->next = hpid;
	ptr_update(hlast_pid, hlast);
	}

kbfree(hptr);

// Everything ok, so return.
slot_pid = spid;	// UP. Id for slot.
return sptr;
}



/********************************************
* FN:		ATTACH_PHRASE
* CR:		05/08/99 AM.
* SUBJ:	Attach phrase to hierarchy concept.
* NOTE:	02/20/07 AM. REIMPLEMENT.
*			Convenience for tearing apart and rebuilding phrase.
*			Eg, detach_phrase and attach_phrase are used by libqconsh/sort_phrase.
********************************************/

bool AQKBM::attach_phrase(PTR_ID pid, XPTR_S *ptr, CON_ID nid)
{
if (!pid || !ptr || !nid)
	return false;

// Attach and update.
ptr->next = nid;
return ptr_update(pid, ptr);
}


/********************************************
* FN:		DETACH_PHRASE
* CR:		05/08/99 AM.
* SUBJ:	Detach phrase from hierarchy concept.
* RET:	node - First node of phrase.
*			UP ptr object for phrase.
*			(User can get ptr id from owner).
* NOTE:	02/20/07 AM. REIMPLEMENT.
*			For editing function, allowing detach to keep a
*			dangling pointer in the owning concept.
* WARN:	USER MUST FREE NODE AND PTR OBJECT COPIES.
********************************************/

XCON_S *AQKBM::detach_phrase(XCON_S *owner, /*UP*/ XPTR_S *&ptr)
{
ptr = 0;
if (!owner)
	return 0;

// Get phrase ptr.
XPTR_S *tmp;
if (!getXptr(owner->phrases, /*DU*/ tmp))
	return 0;

// Get first node of phrase.
CON_ID nid = tmp->v.xcon;
XCON_S *node;
if (!(node = Con(nid)))
	{
	kbfree(tmp);
	return 0;
	}

// Detach node from ptr.
// (Don't really need to do this if sort_phrase is working right.)
// If kb is munged, doing all this isn't going to help.
ptr->next = 0;	// Detach first node from ptr.
ptr_update(owner->phrases, ptr);

// Return all these things.
ptr = tmp;	// UP. ptr object.

return node;
}
