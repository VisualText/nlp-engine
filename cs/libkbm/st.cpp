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
*									ST.C
*
* FILE:	conan.�/st.c
* SUBJ:	String table manager for Conch.
* CR:	5/07/95 AM.
* NOTE:	String table split into segments (Macintosh).
*		Assume that some part of the table exists already, and allocate
*		dynamically as needed.
*		If segmented, tables are called St0, St1, etc.
*		If not, entire table is called St0.
*		Whether automatically generated or not, need a pointer to next empty
*		spot in current segment of string table.
*		One complication. Dynamic and prebuilt segment sizes may differ, esp
*		on systems like the Sun.
*		When the string table is generated automatically, need to provide
*		size data as well.
*		5/9/95 AM. Since we're treating hard and dynamic str segs uniformly,
*		may not need to track hard-wired totals (assuming we dump the whole
*		symbol table, when requested.)  But as an optimization, could dump
*		only the dynamically created segments (assuming no deletions in hard).
*		For now, assuming all segments have equal size. (For UNIX, etc., may
*		want a more efficient single-segment string table, though this works
*		ok, to save space. Bigger segment size than 32K are advisable.)
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <stdlib.h>
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include "kbm/st.h"
#include "kbm/kbm_alloc.h"			// 08/10/99 AM.
//#define ST_DEBUG


int AST::count_ = 0;			// Counter for AST class.				// 08/14/02 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

AST::AST(
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

AST::~AST()
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


//void	AST::setCon(void *x)	{con_	= x;}

///////////



/**************************************************
*						ST_ADD
* FUN:	st_add
* SUBJ:	Add string to string table.
* CR:	5/10/95 AM.
* RET:	True if successful.
*		(up) ptr - Pointer to string in str table.
* NOTE:	Primitive.  Doesn't check str table or sym table.
* OPT:	Counting by pointer arithmetic.
*
**************************************************/

bool
AST::st_add(
	_TCHAR *str,			/* Given string. */
	/*UP*/
	_TCHAR **ptr			/* Position in str table.	*/
	)
{
long avail;			/* Space left in segment.	*/
long off;

/*
If string fits in current segment, great.
Else, allocate a new segment.  Zero out end of the old seg.
Copy in string.
Update globals.
*/

avail = st_avail() - (long) 1;			/* minus one space for '\0' */
if (strlen_cmp(str, avail) > 0)			/* Not enough space left.	*/
   st_alloc();

/* Install string */
*ptr = St_seg_p+1;						/* First empty loc in seg.	*/
while ((*++St_seg_p = *str++) != '\0')
   ;

/* If it was an exact fit (or error) */
off = St_seg_p - St_segs[St_seg_curr] - ST_SEG_SIZE + 1;
if (off == 0)
   _ftprintf(stderr, _T("[st_add: Exact fit in segment.]\n"));
else if (off > 0)
   {
   _ftprintf(stderr, _T("[st_add: String tab error.]\n"));
   exit(1);
   }

return(true);
}


/**************************************************
*						ST_ALLOC
* FUN:	st_alloc
* SUBJ:	Allocate string table segment.
* CR:	9/30/95 AM.
* RET:	Nothing.
* EFF:	Modifies st mgr globals.
* NOTE:	For new empty segment, st_seg_p points to an illegal loc one
*		element before the 0 element.
*
**************************************************/

void
AST::st_alloc()
{
if (St_seg_curr < (St_segs_tot - 1))
   {
   /* An empty segment is already available. */
   /* This happens if multiple empty segs are generated, eg. */
      
   /* Zero out rest of segment, if needed.	*/
      
   ++St_seg_curr;
   St_seg_p = St_segs[St_seg_curr] - 1;	/* Note: One BEFORE array! */
   return;
   }
   
if (St_segs_tot >= ST_SEGS_MAX)
   {
   _ftprintf(stderr, _T("[st_alloc: String segment table overflow.]\n"));
   exit(1);
   }
   
/* Zero out rest of segment, if needed. */

/* Allocate a new segment. */
// FIX. // 08/10/99 AM. Not using sm_alloc.
St_segs[St_segs_tot] = (_TCHAR *) kbm_alloc(ST_SEG_SIZE, sizeof(_TCHAR));

/* Update globals. */
++St_segs_tot;
++St_seg_curr;
St_seg_p = St_segs[St_seg_curr] - 1;	/* Note: One BEFORE array! */
}


/**************************************************
*						ST_AVAIL
* FUN:	st_avail
* SUBJ:	Compute space left in current st seg.
* CR:	9/30/95 AM.
* RET:	Num of chars till curr st seg is filled.
*
**************************************************/

long
AST::st_avail()
{
return((long) (St_segs[St_seg_curr] + ST_SEG_SIZE - St_seg_p -
	2));	// FIX!	// 05/08/00 AM.
}


/**************************************************
*						ST_CLEAN
* FUN:	st_clean
* SUBJ:	String table cleanup prior to exit.
* CR:	9/10/95 AM.
* NOTE:	Seg numbers could be tracked by global vars.
*
**************************************************/

void
AST::st_clean()
{
int first, ii;

/* Get first allocated segment number. */
//first = 1 + ST_SEGS_TOT;									// MEM LEAK	// 01/05/01 AM.
first = ST_SEGS_TOT;											// FIX		// 01/05/01 AM.

/* Free up all allocated segments. */
for (ii = first; ii < St_segs_tot; ii++)
   {
   //printf("here\n");
   //free(St_segs[ii]);			// 04/16/99 AM.
	delete [] (St_segs[ii]);	// 04/16/99 AM.
	St_segs[ii] = NULL;			// 04/16/99 AM.
   }
}


/**************************************************
*						ST_CURR_OFF
* FUN:	st_curr_off
* SUBJ:	Return offset of last full loc in st.
* CR:	9/30/95 AM.
* RET:	Offset from base of current segment.
* NOTE:	Modularity fn.
*
**************************************************/

long
AST::st_curr_off()
{
return((long) (St_seg_p - St_segs[St_seg_curr]));
}


/**************************************************
*						ST_FIND
* FUN:	st_find
* SUBJ:	String table lookup.
* CR:	5/07/95 AM.
* RET:	Return ptr to string for given offset.
* NOTE:	Presumably it would be an error to reference other
*		than the start of any string in the table.  Some
*		perverse users may want to do this, so we won't check
*		against it.
*
**************************************************/

#ifdef ST_OLD
_TCHAR *st_find(long off)
{
int nseg;		/* Segment number.					*/
long oseg;		/* Offset in appropriate segment.	*/
_TCHAR *seg;		/* The referenced st segment.		*/

#ifdef ST_DEBUG
if (off >= St_tot || off < 0)
   {
   _ftprintf(stderr, _T("[st_find: Given bad offset=%ld.]\n"), off);
   return((_TCHAR *) NULL);
   }
#endif

oseg = (off % ST_SEG_SIZE);	// Offset in segment.	// DIV.
nseg = (off / ST_SEG_SIZE);	// DIV.

#ifdef ST_DEBUG
if (nseg > St_segs_tot)
   {
   _ftprintf(stderr, _T("[st_find: Error(1). seg=%d > tot=%d.]\n"), seg, St_segs_tot);
   exit(1);
   }
if (oseg && seg[oseg-1] != '\0')
   {
   _ftprintf(stderr, _T("[st_find: Error(2). seg=%d > tot=%d.]\n"), seg, St_segs_tot);
   exit(1);
   }
#endif

seg = St_segs[nseg];

#ifdef ST_DEBUG
if (!seg)
   {
   _ftprintf(stderr, _T("[st_find: Bad seg ptr in St_segs[%d]. Abort.]\n"), nseg);
   exit(1);
   }
#endif

return(seg + oseg);
}
#endif


/**************************************************
*						ST_HARD_INI
* FUN:	st_hard_ini
* SUBJ:	Initialize string table manager with hard-coded data.
* CR:	9/30/95 AM.
* ASS:	Code is compiled with automatically generated string table
*		code (St_ini.c St_ini.h St0.c St1.c ...).
* NOTE:	Handshake between the st mgr and the generated st.
*
**************************************************/

bool																// 05/05/01 AM.
AST::st_hard_ini(
	long seg_size,
	int segs_tot,
	int seg_curr,
	_TCHAR *seg_p
	)
{
if (seg_size <= 0)															// 05/05/01 AM.
	return false;																// 05/05/01 AM.

ST_SEG_SIZE = seg_size;

St_segs_tot = ST_SEGS_TOT = segs_tot;
St_seg_curr = ST_SEG_CURR = seg_curr;
St_seg_p    = ST_SEG_P    = seg_p;
return true;																	// 05/05/01 AM.
}


/**************************************************
*						ST_INI
* FUN:	st_ini
* SUBJ:	String table init.
* CR:	5/07/95 AM.
* NOTE:	
*
**************************************************/

#ifdef ST_OLD
void
AST::st_ini()
{
long total,			/* Size of hard-wired string table.		*/
     filled;		/* Filled size of hard-wired string table. */
_TCHAR *st_ptr;

/* Automatically gen'd function for hard-wired string table.	*/

cc_st_ini(St_segs, &St_segs_tot, &Hard_st_tot, &Hard_st_fill, &St_ptr);

St_tot	= Hard_st_tot;		/* String table size so far.		*/
St_fill = Hard_st_fill;

if (St_segs_tot <= 0)
   _ftprintf(stderr, _T("Warning: Conch has no string table.\n"));
}
#endif


/**************************************************
*						ST_OFF
* FUN:	st_off
* SUBJ:	Compute offset of string table pointer.
* CR:	10/4/95 AM.
* RET:	(up) seg - segment number in string table (or neg).
*		(up) off - offset within segment.
* ALG:	Traverse segment table. If str falls within seg bounds,
*		compute its offset in seg.
*
**************************************************/

void AST::st_off(
	_TCHAR *str,
	/*UP*/
	int *seg,
	long *off
	)
{
int ii;

if (str)
   {
   for (ii = 0; ii < St_segs_tot; ii++)
      {
      if (str >= St_segs[ii] && str < (St_segs[ii] + ST_SEG_SIZE))
         {
         *seg = ii;
         *off = str - St_segs[ii];
         return;
         }
      }
   }
*seg = -1;
*off = (long) -1;
}


/**************************************************
*						ST_PP
* FUN:	st_pp
* SUBJ:	Pretty-print the filled string table.
* CR:	8/27/95 AM.
* ALG:	Traverse all the filled segments.
* ASS:	Empty end of each segment is populated with nulls.
*
**************************************************/

bool
AST::st_pp(
	//FILE *fp			// 04/20/99 AM.
	std::_t_ostream *fp			// 04/20/99 AM.
	)
{
int ii;
long count;
_TCHAR *ptr;
int flag;

//fprintf(fp, "String Table:\n-------------\n");
*fp << _T("String Table:\n-------------") << std::endl;
for (ii = 0; ii < St_segs_tot; ii++)
   {
   ptr = St_segs[ii];
   count = 0;
   flag = 0;
   while ((count++ < ST_SEG_SIZE))
      {
      if (!(count % STD_LINE))
         //fprintf(fp, "\n");
			*fp << std::endl;
      if (ptr == St_seg_p + 1)	/* End of filled table. */
         {
         //fprintf(fp, "\n");
			*fp << std::endl;
         return true;
         }
      if (*ptr)
         {
         //fprintf(fp, "%c", *ptr);
			*fp << *ptr;
         flag = 1;
         }
      else if (flag)
         {
         //fprintf(fp, " ");	/* String separator */
			*fp << _T(" ");				// String separator.
         flag = 0;
         }
      ++ptr;
      }
   }
return true;
}


/**************************************************
*						ST_PP_ALL
* FUN:	st_pp_all
* SUBJ:	Pretty-print the entire string table.
* CR:	5/15/95 AM.
* ALG:	Traverse all the segments, dumping each in full.
* ASS:	Empty end of each segment is populated with nulls.
* NOTE:	Traverses even empty parts of the table.
*		8/27/95 AM. Renamed from st_pp.
*
**************************************************/

void AST::st_pp_all(
	//FILE *fp			// 04/20/99 AM.
	std::_t_ostream *fp			// 04/20/99 AM.
	)
{
int ii;
long count;
_TCHAR *ptr;
int flag;

for (ii = 0; ii < St_segs_tot; ii++)
   {
   ptr = St_segs[ii];
   count = 0;
   flag = 0;
   while (count++ < ST_SEG_SIZE)
      {
      if (!(count % STD_LINE))
         //fprintf(fp, "\n");
			*fp << std::endl;
      if (!(count % 1000))
         {
         //printf("%ld\n", count);
			*fp << count << std::endl;
         }
      if (*ptr)
         {
         //fprintf(fp, "%c", *ptr);
			*fp << *ptr;
         flag = 1;
         }
      else if (flag)
         {
         //fprintf(fp, " ");	/* String separator */
			*fp << _T(" ");				// String separator.
         flag = 0;
         }
      ++ptr;
      }
   }
}


/**************************************************
*						ST_SEG_CURR
* FUN:	st_seg_curr
* SUBJ:	Return current seg num for string table.
* CR:	9/30/95 AM.
* RET:	St_seg_curr.
* NOTE:	Modularity fn.
*
**************************************************/

int
AST::st_seg_curr()
{
return(St_seg_curr);
}


/**************************************************
*						ST_SEG_P
* FUN:	st_seg_p
* SUBJ:	Get first empty loc of string table.
* CR:	9/30/95 AM.
* RET:	Return ptr to string for given offset.
* NOTE:	Modularity fn.
*
**************************************************/

_TCHAR *
AST::st_seg_p()
{
return(St_seg_p);
}


/**************************************************
*						ST_SEG_SIZE
* FUN:	st_seg_size
* SUBJ:	Return size of a string table segment.
* CR:	9/30/95 AM.
* RET:	ST_SEG_SIZE
* NOTE:	Modularity fn.
*
**************************************************/

long
AST::st_seg_size()
{
return(ST_SEG_SIZE);
}


/**************************************************
*						ST_SEGS_TOT
* FUN:	st_segs_tot
* SUBJ:	Return total num of segments in string table.
* CR:	9/30/95 AM.
* RET:	St_segs_tot
* NOTE:	Modularity fn.
*
**************************************************/

int
AST::st_segs_tot()
{
return(St_segs_tot);
}


/**************************************************
*						ST_SEG_TABLE
* FUN:	st_seg_table
* SUBJ:	Return the global segment table for string table mgmt.
* CR:	9/30/95 AM.
* NOTE:	Modularity fn.
*
**************************************************/

_TCHAR **
AST::st_seg_table()
{
return(St_segs);
}
