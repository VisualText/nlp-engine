/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									PTR.C
*
* FILE:	consh.�/ptr.c
* SUBJ:	Ptr table manager for Consh.
* NOTE:	Analogous to concept table.
*		'ptr' may be a bad choice of words.  In text, 'ptr' will mean a regular
*		pointer, while 'PTR' will refer to the data structure.
* CR:	9/11/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/kbm_alloc.h"		// 08/10/99 AM.

//#define DEBUG

int APTR::count_ = 0;		// Counter for APTR class.				// 08/14/02 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

APTR::APTR(
	)
{
//st_	= 0;
//con_	= 0;
//ptr_	= 0;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
/*******************************************/

APTR::~APTR()
{
//if (st_)
//	delete [] (char *) st_;
//if (con_)
//	delete [] (CON *) con_;
//if (ptr_)
//	delete [] (PTR *) ptr_;

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/
//void *AST::getCon()	{return con_;}


//void	APTR::setCon(void *x)	{con_	= x;}

///////////


/**************************************************
*						PTR_ADD_E
* FUN:	ptr_add_e
* SUBJ:	Add a ptr struct to end of given ptrs.
* CR:	12/20/95 AM.
* RET:	Added ptr structure, if successful.
* NOTE:	Need address of list, in case given list is empty.
*		Seems a more natural fn than ptr_add_end.
*
**************************************************/

PTR *APTR::
ptr_add_e(
	PTR *ptr,				/* Ptr to add to end.	*/
	/*DU*/
	PTR **ptrs,
	PTR **end		/* Ptrs list to modify. */
	)
{
PTR *list;

if (!*ptrs)				/* Empty list.				*/
   {
   *ptrs = *end = ptr;
   return(ptr);
   }
if (*end)				/* If end of list is known.*/
   {
   (*end)->next = ptr;
   return(*end = ptr);
   }

list = *ptrs;
while (list->next)		/* Get last ptr in list. */
   list = list->next;
*end = list->next = ptr;
return(ptr);
}


/**************************************************
*						PTR_ADD_END
* FUN:	ptr_add_end
* SUBJ:	Make a ptr struct at end of given ptrs.
* CR:	10/9/95 AM.
* RET:	Added ptr structure, if successful.
* NOTE:	Need address of list, in case given list is empty.
*
**************************************************/

PTR *APTR::
ptr_add_end(
	PTR **ptrs		/* Ptrs list to modify. */
	)
{
PTR *list, *elt;

list = *ptrs;
elt  = ptr_alloc();

if (!list)
   return(*ptrs = elt);

while (list->next)		/* Get last ptr in list. */
   list = list->next;
return(list->next = elt);
}


/**************************************************
*						PTR_ALLOC
* FUN:	ptr_alloc
* SUBJ:	Allocate ptr data structure.
* CR:	09/11/95 AM.
* RET:	ptr to ptr structure, if successful.
* NOTE:	Fetch next available space in ptr table.
*		If at end of segment, allocate new segment.
*
**************************************************/

PTR *APTR::
ptr_alloc()
{
long off;		/* Offset in segment. */

off = (long) (1 + Ptr_seg_p - Ptr_segs[Ptr_seg_curr]);

if (off < PTR_SEG_SIZE)
	{
	++Ptr_seg_p;
	sm_zero((_TCHAR *)Ptr_seg_p, sizeof(PTR));	// 06/09/99 AM.
   return(Ptr_seg_p);		/* Use next location. */
	}

if (Ptr_seg_curr < (Ptr_segs_tot - 1))
   {
   /* An empty segment is already available. */
   /* This happens if multiple empty segs are generated, eg. */
      
   /* Zero out rest of segment, if needed.	*/
      
   ++Ptr_seg_curr;
	sm_zero((_TCHAR *)Ptr_seg_p, sizeof(PTR));	// 06/09/99 AM.
   Ptr_seg_p = Ptr_segs[Ptr_seg_curr];
   return(Ptr_seg_p);
   }

/* Need a more global check on space, also. */
if (Ptr_segs_tot >= PTR_SEGS_MAX)
   {
   _ftprintf(stderr, _T("[ptr_alloc: Ptr segment table overflow.]\n"));
   exit(1);
   }

/* Allocate a new segment. */
// FIX // 08/10/99 AM.  Not using sm_alloc.
Ptr_segs[Ptr_segs_tot] = (PTR *) kbm_alloc(PTR_SEG_SIZE, sizeof(PTR));

++Ptr_segs_tot;
++Ptr_seg_curr;
Ptr_seg_p = Ptr_segs[Ptr_seg_curr];
sm_zero((_TCHAR *)Ptr_seg_p, sizeof(PTR));	// 06/09/99 AM.
return(Ptr_seg_p);
}


/**************************************************
*						PTR_CLEAN
* FUN:	ptr_clean
* SUBJ:	Clean up ptr table management prior to exit.
* CR:	09/11/95 AM.
* NOTE:	Hard-wired segs go to (HW_ptr_segs - 1).
*
**************************************************/

void APTR::
ptr_clean()
{
int ii;
/* Free all allocated segments. */
for (ii = PTR_SEGS_TOT; ii < Ptr_segs_tot; ii++)
	{
   //free(Ptr_segs[ii]);		// 04/16/99 AM.
	delete [] (Ptr_segs[ii]);	// 04/16/99 AM.
	Ptr_segs[ii] = PNULL;		// 04/16/99 AM.
	}
}


/**************************************************
*						PTR_CURR_OFF
* FUN:	ptr_curr_off
* SUBJ:	Return offset of last full loc in ptr table.
* CR:	10/4/95 AM.
* RET:	Offset from base of current segment.
* NOTE:	Modularity fn.
*
**************************************************/

long APTR::
ptr_curr_off()
{
return((long) (Ptr_seg_p - Ptr_segs[Ptr_seg_curr]));
}


/**************************************************
*						PTR_HARD_INI
* FUN:	ptr_hard_ini
* SUBJ:	Initialize ptr table manager with hard-coded data.
* CR:	10/4/95 AM.
* ASS:	Code is compiled with automatically generated ptr table
*		code (Ptr_ini.c Ptr_ini.h Ptr0.c ...).
* NOTE:	Handshake between the ptr table mgr and the generated ptr table.
*
**************************************************/

bool																				// 05/05/01 AM.
APTR::ptr_hard_ini(
	long seg_size,
	int segs_tot,
	int seg_curr,
	PTR *seg_p
	)
{
if (seg_size <= 0)															// 05/05/01 AM.
	return false;																// 05/05/01 AM.
PTR_SEG_SIZE = seg_size;

Ptr_segs_tot = PTR_SEGS_TOT = segs_tot;
Ptr_seg_curr = PTR_SEG_CURR = seg_curr;
Ptr_seg_p    = PTR_SEG_P    = seg_p;
return true;																	// 05/05/01 AM.
}


#ifdef PTR_OLD
/**************************************************
*						PTR_INI
* FUN:	ptr_ini
* SUBJ:	Initialize ptr table management.
* CR:	9/11/95 AM.
* NOTE:	Call an automatically generated init function for the
*		compiled portion of the ptr table.
*
**************************************************/

void
ptr_ini()
{
/*
Automatically generated init function will fill ptr segment
array slots with addresses of prebuilt segments, and will
set global variables indicating the number of prebuilt segments,
as well as ptrs to first open locus in these segments.
*/
cc_ptr_ini(Ptr_segs, &Ptr_segs_tot, &Ptr_size, &Ptr_ptr, &Ptr_seg);
HW_ptr_segs = Ptr_segs_tot;
}
#endif


/**************************************************
*						PTR_MAKE_*
* FUN:	ptr_make_c
* SUBJ:	Make ptr with specific kind of value.
* CR:	12/20/95 AM.
*
**************************************************/

PTR *
APTR::ptr_make_c(CON *con)
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pCON;
ptr->v.vcon = con;
return(ptr);
}

PTR *
APTR::ptr_make_p(PTR *pv)
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pPTR;
ptr->v.vptr = pv;
return(ptr);
}

PTR *
APTR::ptr_make_sym(SYM *sym)
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pSYM;
ptr->v.vsym = sym;
return(ptr);
}

PTR *
APTR::ptr_make_st(_TCHAR *st)
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pST;
ptr->v.vst = st;
return(ptr);
}

PTR *
APTR::ptr_make_n(long num)
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pNUM;
ptr->v.vnum = num;
return(ptr);
}

PTR *
APTR::ptr_make_n(float fnum)		// 08/16/01 AM.
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pFLOAT;
ptr->v.vfloat = fnum;
return(ptr);
}

PTR *
APTR::ptr_make_ch(_TCHAR ch)
{
PTR *ptr;

ptr = ptr_alloc();
ptr->kind = pCHAR;
ptr->v.vchar = ch;
return(ptr);
}


/**************************************************
*						PTR_OFF
* FUN:	ptr_off
* SUBJ:	Compute offset of ptr table pointer.
* CR:	10/4/95 AM.
* RET:	(up) seg - segment number in table (or neg).
*		(up) off - offset within segment.
* ALG:	Traverse segment table. If pointer falls within seg bounds,
*		compute its offset in seg.
*
**************************************************/

void APTR::ptr_off(
	PTR *ptr,
	/*UP*/
	int *seg,
	long *off
	)
{
int ii;

if (ptr)
   {
   for (ii = 0; ii < Ptr_segs_tot; ii++)
      {
      if (ptr >= Ptr_segs[ii] && ptr < (Ptr_segs[ii] + PTR_SEG_SIZE))
         {
         *seg = ii;
         *off = ptr - Ptr_segs[ii];
         return;
         }
      }
   }
*seg = -1;
*off = (long) -1;
}


/**************************************************
*						PTR_SEG_CURR
* FUN:	ptr_seg_curr
* SUBJ:	Return current seg num for ptr table.
* CR:	10/4/95 AM.
* RET:	Ptr_seg_curr.
* NOTE:	Modularity fn.
*
**************************************************/

int
APTR::ptr_seg_curr()
{
return(Ptr_seg_curr);
}


/**************************************************
*						PTR_SEG_P
* FUN:	ptr_seg_p
* SUBJ:	Get first empty loc of ptr table.
* CR:	10/4/95 AM.
* RET:	Return ptr to string for given offset.
* NOTE:	Modularity fn.
*
**************************************************/

PTR *
APTR::ptr_seg_p()
{
return(Ptr_seg_p);
}


/**************************************************
*						PTR_SEG_SIZE
* FUN:	ptr_seg_size
* SUBJ:	Return size of a ptr table segment.
* CR:	10/4/95 AM.
* RET:	PTR_SEG_SIZE
* NOTE:	Modularity fn.
*
**************************************************/

long
APTR::ptr_seg_size()
{
return(PTR_SEG_SIZE);
}


/**************************************************
*						PTR_SEGS_TOT
* FUN:	ptr_segs_tot
* SUBJ:	Return total num of segments in ptr table.
* CR:	10/4/95 AM.
* RET:	Ptr_segs_tot
* NOTE:	Modularity fn.
*
**************************************************/

int
APTR::ptr_segs_tot()
{
return(Ptr_segs_tot);
}

/**************************************************
*						PTR_SEG_TABLE
* FUN:	ptr_seg_table
* SUBJ:	Return the global segment table for ptr table mgmt.
* CR:	10/1/95 AM.
* NOTE:	Modularity fn.
*
**************************************************/

PTR **
APTR::ptr_seg_table()
{
return(Ptr_segs);
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

bool
APTR::s_to_pkind(
	_TCHAR *str,
	/*UP*/
	PKIND *pk
	)
{
if (!str || !*str)
   return(false);
else if (!_tcscmp(str, _T("no_ptr")))
   *pk = pNO_PTR;
else if (!_tcscmp(str, _T("pcon")))
   *pk = pCON;
else if (!_tcscmp(str, _T("pptr")))
   *pk = pPTR;
else if (!_tcscmp(str, _T("psym")))
   *pk = pSYM;
else if (!_tcscmp(str, _T("pst")))
   *pk = pST;
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

bool APTR::s_to_pval(
	_TCHAR *str,
	PKIND pkind,
	/*UP*/
	PVAL *pv
	)
{
long num;
bool ok;
CON *con;
SYM *sym;
float fnum;				// 08/16/01 AM.
bool dirty;									// 06/29/03 AM.

if (!str || !*str)
   return(false);

switch (pkind)
   {
   case pCON:
      ok = s_to_l(str, &num);
      if (!ok)
         return(false);
      con = acon_->Con(num);
      if (!con)
         return(false);
      pv->vcon = con;
      break;
   case pPTR:
      _ftprintf(stderr, _T("[s_to_pval: PTR value unimplemented.]\n"));
      return(false);
   case pSYM:
      sym = asym_->sym_get(str,dirty);
      if (!sym)
         return(false);
      pv->vsym = sym;
      break;
   case pST:
      sym = asym_->sym_get(str,dirty);
      if (!sym)
         return(false);
      pv->vst = sym->str;
      break;
   case pNUM:
      ok = s_to_l(str, &num);
      if (!ok)
         return(false);
      pv->vnum = num;
      break;
   case pFLOAT:																// 08/16/01 AM.
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

