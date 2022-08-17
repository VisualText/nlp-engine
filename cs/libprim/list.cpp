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
*									LIST.C
*
* FILE:	consh.�/list.c
* SUBJ:	Generic list data structures.
* NOTE:	This is akin to a LISP subsystem, which substantial programs require.
*		List element will have a generic value field, and pointer to next list
*		element.  Can reuse for any purpose.
*		Need global manager for free list of these list elements.
* CR:	8/26/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include "consh.h" */
//#include "prim/mach.h"
#include "prim/sm.h"
#include "prim/list_s.h"
#include "prim/list.h"

/* GLOBAL VARIABLES FOR DYNAMIC LIST MGR */
#ifdef OLD_
static LIST		*List_segs[LIST_SEGS_MAX];	/* Seg table for list mgr.		*/
static int		 List_segs_tot;				/* Number of list segments.		*/

static LIST 	*List_free;					/* The free list.				*/
static LIST 	*List_end;					/* End of free list.			*/
#endif

int ALIST::count_ = 0;														// 08/14/02 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

ALIST::ALIST(																	// 08/14/02 AM.
	)
{
List_segs_tot = 0;
List_free = 0;
List_end = 0;
List_buffer = NULL;

// May want to clear out the segment table also.
int ii;
for (ii = 0; ii < LIST_SEGS_MAX; ++ii)
	List_segs[ii] = LNULL;

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
/*******************************************/

ALIST::~ALIST()
{
//if (st_)
//	delete [] (char *) st_;

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/
//void *ALIST::getSt()	{return st_;}


//void	ALIST::setSt(void	*x)	{st_	= x;}

///////////


/**************************************************
*						LIST_ADD
* FUN:	list_add
* SUBJ:	Add a value to end of given list.
* CR:	8/26/95 AM.
* RET:	list - Start of list.
*		UP end - End of list.
* NOTE:	Should be a macro.
*
**************************************************/

LIST *ALIST::list_add(
	LIST *list,		/* Possibly empty list.				*/
	long val,		/* Val to add to end of list.		*/
	/*DU*/
	LIST **end		/* NULL, or end of given list.		*/
	)
{
return(list_add_elt(list, list_make(val), end));
}

/**************************************************
*						LIST_CLEAN
* FUN:	list_clean
* SUBJ:	Clean up list management.
* CR:	8/26/95 AM.
*
**************************************************/

void ALIST::list_clean()
{
int ii;

//fprintf(stderr, "[list_clean: Freeing space for lists.]\n");
for (ii = 0; ii < List_segs_tot; ++ii)						// OPT.	// 08/14/02 AM.
	{
	// 04/15/99 AM. Converting C-style allocation and free to C++.
   //free(List_segs[ii]);
	delete [] (List_segs[ii]);		// 04/15/99 AM.
	List_segs[ii] = LNULL;			// 04/15/99 AM.
	}
}



/**************************************************
*						LIST_FREE
* FUN:	list_free
* SUBJ:	Free given list.
* CR:	8/26/95 AM.
*
**************************************************/

void ALIST::list_free(
	LIST *list,
   LIST *end	/* LNULL, or end of given list. */
   )
{
if (!list) return;

if (!end)
   {
   end = list;
   while (end->next)
      end = end->next;
   }

end->next = List_free;
List_free = list;
if (!List_end)
   List_end = end;
}


/**************************************************
*						LIST_FREE_ELT
* FUN:	list_free_elt
* SUBJ:	Free given list element.
* CR:	8/26/95 AM.
* NOTE:	May want to return rest of list.
*
**************************************************/

void ALIST::list_free_elt(
	LIST *list
	)
{
if (!list) return;
list->next = List_free;
List_free = list;
if (!List_end)
   List_end = list;
}


/**************************************************
*						LIST_INI
* FUN:	list_ini
* SUBJ:	Initialize list management.
* CR:	8/26/95 AM.
* NOTE:	Allocate initial space, and construct an
*		initial free list.
*
**************************************************/

bool ALIST::list_ini()
{
//fprintf(stderr, "[list_ini: Allocating space for lists.]\n");
if (LIST_SEG_SIZE <= 0)														// 05/05/01 AM.
	{
	_ftprintf(stderr, _T("[list_ini: Bad initialization.]\n"));			// 05/05/01 AM.
	return false;																// 05/05/01 AM.
	}
List_segs_tot = 0;
List_free = (LIST *) sm_alloc(LIST_SEG_SIZE, (long) sizeof(LIST));
List_segs[List_segs_tot] = List_free;
List_end = list_build(List_segs[List_segs_tot], LIST_SEG_SIZE);
++List_segs_tot;													// OPT.	// 08/14/02 AM.
return true;																	// 05/05/01 AM.
}


/**************************************************
*						LIST_MAKE
* FUN:	list_make
* SUBJ:	Create a list element.
* CR:	8/26/95 AM.
*
**************************************************/

LIST *ALIST::list_make(
	long val
	)
{
LIST *list;

if (!List_free)
   {
   if (List_segs_tot + 1 == LIST_SEGS_MAX)
      {
      _ftprintf(stderr, _T("[list_make: List table overflow.\n"));
      exit(1);
      }
   List_free = (LIST *) sm_alloc(LIST_SEG_SIZE, (long) sizeof(LIST));
   List_segs[List_segs_tot] = List_free;
   List_end = list_build(List_segs[List_segs_tot], LIST_SEG_SIZE);
   ++List_segs_tot;												// OPT.	// 08/14/02 AM.
   }

list = List_free;
List_free = List_free->next;
if (!List_free)
   List_end = LNULL;

list->next = LNULL;
list->val  = val;
return(list);
}


/**************************************************
*						LIST_PUSH
* FUN:	list_push
* SUBJ:	Add val to start of given list.
* CR:	8/26/95 AM.
* RET:	list - Start of list.
* NOTE:	Should be a macro.
*
**************************************************/

LIST *ALIST::list_push(
	long val,
	LIST *list
	)
{
LIST *elt;

elt = list_make(val);
return(list_push_elt(elt, list));
}

void ALIST::list_add_buf(
	_TCHAR *buf
)
{
	List_buffer = buf;
}

////////////////////////////////////////////////////////////////////////
// GENERAL LIST FUNCTIONS
////////////////////////////////////////////////////////////////////////

/**************************************************
*						LIST_ADD_ELT
* FUN:	list_add_elt
* SUBJ:	Add an element to end of given list.
* CR:	8/26/95 AM.
* RET:	list - Start of list.
*		UP end - End of list.
*
**************************************************/

LIST *ALIST::list_add_elt(
	LIST *list,		/* Possibly empty list.				*/
	LIST *elt,		/* Elt to add to end of list.		*/
	/*DU*/
	LIST **end		/* NULL, or end of given list.		*/
	)
{
LIST *en;

if (!list)
   return(*end = elt);

en = *end;
if (!en)
   {
   en = list;
   while (en->next)
      en = en->next;
   }

en->next = *end = elt;
return(list);
}

/**************************************************
*						LIST_BUILD
* FUN:	list_build
* SUBJ:	Convert a block of space to a list.
* CR:	8/26/95 AM.
* RET:	end - Last element of list.
*
**************************************************/

LIST *ALIST::list_build(
	LIST *array,
	long num				/* Number of list elements.	*/
	)
{
LIST *curr;

curr = array;

/* Traverse and fix up array.	*/
while (--num)
   {
   curr->next = 1 + curr;
   ++curr;															// OPT.	// 08/14/02 AM.
   }
curr->next = LNULL;
return(curr);
}



/**************************************************
*						LIST_CMP
* FUN:	list_cmp
* SUBJ:	Compare length of two lists.
* CR:	8/26/95 AM.
* NOTE:	Optimized.
*
**************************************************/

int ALIST::list_cmp(
	LIST *lista,
	LIST *listb
	)
{
while (lista && listb)
   {
   lista = lista->next;
   listb = listb->next;
   }
if (!lista)
   return(listb ? -1 : 0);
if (!listb)
   return(1);
_ftprintf(stderr, _T("[list_cmp: Error.]\n"));
return 0;
}


/**************************************************
*						LIST_LEN
* FUN:	list_len
* SUBJ:	Get length of given list.
* CR:	8/26/95 AM.
*
**************************************************/

long ALIST::list_len(
	LIST *list
	)
{
long len;

len = 0;
while (list)
   {
   ++len;															// OPT.	// 08/14/02 AM.
   list = list->next;
   }
return(len);
}

/**************************************************
*						LIST_NCMP
* FUN:	list_ncmp
* SUBJ:	Compare list length to a number.
* CR:	8/26/95 AM.
* NOTE:	Optimized.
*
**************************************************/

int ALIST::list_ncmp(
	LIST *list,
	long num
	)
{
while (list && num)
   {
   list = list->next;
   num--;
   }
if (!num)
   return(list ? 1 : 0);
if (!list)
   return(-1);
_ftprintf(stderr, _T("[list_ncmp: Error.]\n"));
return 0;
}


/**************************************************
*						LIST_POP
* FUN:	list_pop
* SUBJ:	Pop first elt off list.
* CR:	8/27/95 AM.
* WARN:	Not setting ->next of popped elt to NULL.
* RET:	list - First elt of list.
* NOTE:	Should be a macro.
*
**************************************************/

long ALIST::list_pop(
	LIST **list
	)
{
LIST *tmp;

if (!list || !*list)
   return((long)0);
tmp = *list;
*list = tmp->next;
return(tmp->val);
}

_TCHAR* ALIST::list_pop_buf(
	LIST** list,
	_TCHAR* buf
)
{
	LIST* tmp;

	if (!list || !*list)
		return(nullptr);
	tmp = *list;
	*list = tmp->next;
	return(&buf[tmp->val]);
}

_TCHAR* ALIST::list_str(
	LIST** list,
	_TCHAR* buf
)
{
	LIST* tmp;
	tmp = *list;
	return(&buf[tmp->val]);
}



/**************************************************
*						LIST_POP_ELT
* FUN:	list_pop_elt
* SUBJ:	Pop first elt off list.
* CR:	8/27/95 AM.
* WARN:	Not setting ->next of popped elt to NULL.
* RET:	First elt of list.
* NOTE:	Should be a macro.
*
**************************************************/

LIST *ALIST::list_pop_elt(
	LIST **list
	)
{
LIST *tmp;

if (!list || !*list)
   return(LNULL);
tmp = *list;
*list = tmp->next;
return(tmp);
}


/**************************************************
*						LIST_PP_STRS
* FUN:	list_pp_strs
* SUBJ:	Pretty-print a list of strings.
* CR:	8/27/95 AM.
*
***************************************flush_t_***********/

void ALIST::list_pp_strs(
	LIST *args,
//	FILE *out			// 04/20/99 AM.
	std::_t_ostream *out,	// 04/20/99 AM.
	_TCHAR *buf
	)
{
while (args)
   {
   //fprintf(out, "\"%s\" ", (char *) args->val);				// 04/20/99 AM.
	*out << _T("\"") << (ALIST::list_str(&args,buf)) << _T("\" ") << std::flush;	// 04/20/99 AM.
   args = args->next;
   }
//fprintf(out, "\n");		// 04/20/99 AM.
*out << std::endl;					// 04/20/99 AM.
}


/**************************************************
*						LIST_PUSH_ELT
* FUN:	list_push_elt
* SUBJ:	Add elt to start of given list.
* CR:	8/26/95 AM.
* RET:	list - Start of list.
*
**************************************************/

LIST *ALIST::list_push_elt(
	LIST *elt,
	LIST *list
	)
{
if (!elt)
   return(list);
elt->next = list;
return(elt);
}

