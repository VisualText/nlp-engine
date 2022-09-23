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
*									CON.C
*
* FILE:	consh.�/con.c
* SUBJ:	Concept table manager for Consh.
* NOTE:	Like sym handler, but simpler.  No hashing and no conflict table.
*		Just as in the string table, we add concepts to the end, allocate
*		when we need more space.  Offset into the concept table serves as
*		unique concept id.  (Concept table segments required to be same size,
*		to simplify management.)
* CR:	9/10/95 AM.
*		08/13/02 AM. Converting this to a class. for concept table.
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
#include "kbm/kbm_kb.h"			// 05/04/99 AM.
#include "kbm/kbm_alloc.h"		// 08/10/99 AM.

#include "kbm/con_.h"		// May want to rename Acon.h.			// 08/13/02 AM.

#include "ext/kb.h"

#include "kbm_priv.h"			// 04/25/00 AM.

void
con_to_path_r(
	CON *con,
	/*UP*/
	_TCHAR* &end			// First empty loc in buffer.
	);

//#define DEBUG
#define MAXSTR 256


int ACON::count_ = 0;			// Counter for ACON class.			// 08/13/02 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

ACON::ACON(
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

ACON::~ACON()
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
  CON *ACON::c_cg_concept()	{return c_cg_CONCEPT;}


//void	ACON::setCon(void *x)	{con_	= x;}


/*******************************************/


///////////
///////////

/**************************************************
*						Con
* FUN:	Con
* SUBJ:	Find concept, given its ID.
* CR:	9/20/95 AM.
* NOTE:	12/24/95 AM. 0 is an ok concept number == the NIL concept.
*
**************************************************/

CON *ACON::Con(long id)
{
//CON *con;
long seg, off;

if (id > (long) 0)
   ;
else if (id == (long) 0)
   {
   /* 12/24/95 AM. Handling empty concept. */
   return(Con_segs[(long) 0]);		/* NIL concept. */
   }
else
   {
   _ftprintf(stderr, _T("[Con: Given no id]\n"));
   return(CNULL);
   }

seg = id / CON_SEG_SIZE;	// DIV
off = id % CON_SEG_SIZE;	// DIV

if (seg >= Con_segs_tot)
   {
   _ftprintf(stderr, _T("[Con: seg %ld > tot segs %d]\n"), seg, Con_segs_tot);
   return(CNULL);
   }


// SINCE STUFF ALLOCATED DYNAMICALLY, THIS MAY NOT BE AN ERROR. // 06/29/00 AM.
//con = Con_segs[seg] + off;												// 06/29/00 AM.
//if (con > Con_seg_p)														// 06/29/00 AM.
//   {	
//   fprintf(stderr, "[Con: Error.]\n");								// 06/29/00 AM.
//   return(CNULL);															// 06/29/00 AM.
//   }

return(Con_segs[seg] + off);
}


/**************************************************
*						CON_ALLOC
* FUN:	con_alloc
* SUBJ:	Allocate con data structure.
* CR:	09/10/95 AM.
* RET:	ptr to con structure, if successful.
* NOTE:	Fetch next available space in concept table.
*		If at end of segment, allocate new segment.
*
**************************************************/

CON *ACON::con_alloc()
{
long off;		/* Offset in segment. */

off = (long) (1 + Con_seg_p - Con_segs[Con_seg_curr]);

if (off < CON_SEG_SIZE)
   {
	++Con_seg_p;
	sm_zero((_TCHAR *)Con_seg_p, sizeof(CON));	// 06/09/99 AM.
   Con_seg_p->id = Con_count++;
   return(Con_seg_p);		/* Use next location. */
   }

if (Con_seg_curr < (Con_segs_tot - 1))
   {
   /* An empty segment is already available. */
   /* This happens if multiple empty segs are generated, eg. */
      
   /* Zero out rest of segment, if needed.	*/
      
   ++Con_seg_curr;
   Con_seg_p = Con_segs[Con_seg_curr];
	sm_zero((_TCHAR *)Con_seg_p, sizeof(CON));	// 06/09/99 AM.
   Con_seg_p->id = Con_count++;
   return(Con_seg_p);
   }

/* Need a more global check on space, also. */
if (Con_segs_tot >= CON_SEGS_MAX)
   {
   _ftprintf(stderr, _T("[con_alloc: Con segment table overflow.]\n"));
   exit(1);
   }

/* Allocate a new segment. */
// FIX. 08/10/99 AM. Not using sm_alloc.
Con_segs[Con_segs_tot] = (CON *) kbm_alloc(CON_SEG_SIZE, sizeof(CON));

/* Update globals. */
++Con_segs_tot;
++Con_seg_curr;
Con_seg_p = Con_segs[Con_seg_curr];
sm_zero((_TCHAR *)Con_seg_p, sizeof(CON));	// 06/09/99 AM.
Con_seg_p->id = Con_count++;

return(Con_seg_p);
}


/**************************************************
*						CON_CLEAN
* FUN:	con_clean
* SUBJ:	Clean up concept table management prior to exit.
* CR:	09/10/95 AM.
* NOTE:	Hard-wired segs go to (HW_con_segs - 1).
*
**************************************************/

void ACON::con_clean()
{
int ii;
_TCHAR *tmp;			// 08/10/99 AM.
/* Free all allocated segments. */
for (ii = CON_SEGS_TOT; ii < Con_segs_tot; ii++)
	{
   // free(Con_segs[ii]);				// 04/16/99 AM.
//	delete [] (Con_segs[ii]);			// 04/16/99 AM.
	// FIX. Was allocated as CHAR, so delete same way. // 08/10/99 AM.
	tmp = (_TCHAR *) (Con_segs[ii]);							// 08/10/99 AM.
	delete [] tmp;						// 08/10/99 AM.
	Con_segs[ii] = CNULL;				// 04/16/99 AM.
	}
}


/**************************************************
*						CON_HARD_INI
* FUN:	con_hard_ini
* SUBJ:	Initialize con table manager with hard-coded data.
* CR:	10/4/95 AM.
* ASS:	Code is compiled with automatically generated con table
*		code (Con_ini.c Con_ini.h Con0.c ...).
* NOTE:	Handshake between the con table mgr and the generated con table.
*
**************************************************/

bool ACON::con_hard_ini(
	long seg_size,
	int segs_tot,
	int seg_curr,
	CON *seg_p,
	ID count
	)
{
if (seg_size <= 0)															// 05/05/01 AM.
	{
	std::_t_cerr << _T("[con_hard_ini: Error. Zero seg size.]") << std::endl;		// 05/07/01 AM.
	return false;																// 05/05/01 AM.
	}

CON_SEG_SIZE = seg_size;

Con_segs_tot = CON_SEGS_TOT = segs_tot;
Con_seg_curr = CON_SEG_CURR = seg_curr;
Con_seg_p    = CON_SEG_P    = seg_p;
Con_count    = CON_COUNT    = count;
return true;
}


/**************************************************
*						CON_OFF
* FUN:	con_off
* SUBJ:	Compute offset of con table pointer.
* CR:	10/4/95 AM.
* RET:	(up) seg - segment number in table (or neg).
*		(up) off - offset within segment.
* ALG:	Traverse segment table. If pointer falls within seg bounds,
*		compute its offset in seg.
*
**************************************************/

void ACON::con_off(
	CON *con,
	/*UP*/
	int *seg,
	long *off
	)
{
int ii;

if (con)
   {
   for (ii = 0; ii < Con_segs_tot; ii++)
      {
      if (con >= Con_segs[ii] && con < (Con_segs[ii] + CON_SEG_SIZE))
         {
         *seg = ii;
         *off = con - Con_segs[ii];
         return;
         }
      }
   }
*seg = -1;
*off = (long) -1;
}


/**************************************************
*						CON_OFF_TO_ID
* FUN:	con_off_to_id
* SUBJ:	Convert seg,off pair to concept id.
* CR:	12/2/95 AM.
* RET:	id - Concept id, whether concept present or not.
*
**************************************************/

ID ACON::con_off_to_id(
	int seg,
	long off
	)
{
if (seg < 0)
   return((ID) 0);
return((CON_SEG_SIZE * (long) seg) + off);
}


/**************************************************
*						CON_SEG_CURR
* FUN:	con_seg_curr
* SUBJ:	Return current seg num for con table.
* CR:	10/4/95 AM.
* RET:	Con_seg_curr.
* NOTE:	Modularity fn.
*
**************************************************/

int ACON::con_seg_curr()
{
return(Con_seg_curr);
}


/**************************************************
*						CON_SEG_P
* FUN:	con_seg_p
* SUBJ:	Get first empty loc of con table.
* CR:	10/4/95 AM.
* RET:	Return ptr to string for given offset.
* NOTE:	Modularity fn.
*
**************************************************/

CON *ACON::con_seg_p()
{
return(Con_seg_p);
}


/**************************************************
*						CON_SEG_SIZE
* FUN:	con_seg_size
* SUBJ:	Return size of a con table segment.
* CR:	10/4/95 AM.
* RET:	CON_SEG_SIZE
* NOTE:	Modularity fn.
*
**************************************************/

long ACON::con_seg_size()
{
return(CON_SEG_SIZE);
}


/**************************************************
*						CON_SEGS_TOT
* FUN:	con_segs_tot
* SUBJ:	Return total num of segments in con table.
* CR:	10/4/95 AM.
* RET:	Con_segs_tot
* NOTE:	Modularity fn.
*
**************************************************/

int ACON::con_segs_tot()
{
return(Con_segs_tot);
}

/**************************************************
*						CON_SEG_TABLE
* FUN:	con_seg_table
* SUBJ:	Return the global segment table for con table mgmt.
* CR:	10/1/95 AM.
* NOTE:	Modularity fn.
*
**************************************************/

CON **ACON::con_seg_table()
{
return(Con_segs);
}


/**************************************************
*						CON_TAB_PRETTY
* FUN:	con_tab_pretty
* SUBJ:	Pretty-print the concept table.
* CR:	9/20/95 AM.
* NOTE:	Need to watch for deleted concepts.
*
**************************************************/

void ACON::con_tab_pretty(
	//FILE *out			// 04/20/99 AM.
	std::_t_ostream *out		// 04/20/99 AM.
	)
{
int ii;
long jj, tot;
CON *con;

if (!out)	// 02/20/19 AM.
  out = &(std::_t_cout);	// 02/20/19 AM.

//fprintf(out, "Concept Table\n");	// 04/20/99 AM.
//fprintf(out, "-------------\n");	// 04/20/99 AM.
*out << _T("Concept Table") << std::endl;		// 04/20/99 AM.
*out << _T("-------------") << std::endl;		// 04/20/99 AM.
tot = 0;
for (ii = 0; ii <= Con_seg_curr; ii++)	/* For every segment. */
   {
   con = Con_segs[ii];
   for (jj = 0; jj < CON_SEG_SIZE; jj++)
      {
      if (con == Con_seg_p + 1)
         return;
		// 04/20/99 AM.
   //fprintf(out, "%ld: (%s) %s \n", tot, con_kind_str(con), con_str(con));
		*out << tot
			  << _T(": (")
		     << con_kind_str(con)
			  << _T(") ")
			  << con_str(con) << std::endl;			// 04/20/99 AM.
      ++tot;
      ++con;
      }
   }
}


/**************************************************
*						CON_COUNT
* FUN:	con_count
* SUBJ:	Return count of concepts in concept table.
* CR:	12/2/95 AM.
* RET:	Total count.
* NOTE:	Modularity fn.
*
**************************************************/

ID ACON::con_count()
{
return(Con_count);
}


/**************************************************
*						CON_CURR_OFF
* FUN:	con_curr_off
* SUBJ:	Return offset of last full loc in con table.
* CR:	10/4/95 AM.
* RET:	Offset from base of current segment.
* NOTE:	Modularity fn.
*
**************************************************/

long ACON::con_curr_off()
{
return((long) (Con_seg_p - Con_segs[Con_seg_curr]));
}


/**************************************************
*						CON_ADD_ALPHA
* FUN:	con_add_alpha
* SUBJ:	Insert given name into alphabetically ordered hierarchy phrase.
* CR:	10/24/95 AM.
* ASS:	Assumes alphabetic phrase.
*		Assumes redundant names not allowed.
* RET:	The new concept inserted into hierarchy phrase.
* NOTE:	May be the first concept in phrase.
*		Adding as a WORD concept.
*
**************************************************/

CON *ACON::con_add_alpha(
	CON *hier,
	_TCHAR *name
	)
{
CON *con, *pre;
//CON *phr;
SYM *sym;
//int res;

if (!hier || str_empty(name))
   return(CNULL);

con = con_child_alpha(hier, name, /*UP*/ &pre);
if (con)
   {
   _ftprintf(stderr, _T("[con_add_alpha: hier=%s already has name=%s]\n"),
   		con_str(hier), name);
//   return(CNULL);	// 07/21/03 AM.
	return con;	// RECOVER.	// 07/21/03 AM.
   }

bool dirty;									// 06/29/03 AM.
sym = asym_->sym_get(name,dirty);		/* Fetch sym for given name. 	*/

if (!sym || sym->con)
   return(CNULL);

con = con_alloc();			/* Get empty concept struct.	*/

/* Add sym as name of concept. */
con->kind = cWORD;
con->v.vsym = sym;
sym->con = con;


CON *fst;																		// 11/02/00 AM.
if (!pre)		/* Concept goes first. */
   {
   fst = con->next = hier->dn;											// 11/02/00 AM.
   if (fst)																		// 11/02/00 AM.
		{
      fst->prev = con;														// 11/02/00 AM.
		fst->up = CNULL;											// FIX.	// 11/02/00 AM.
		}
   hier->dn = con;
	/* Point to hierarchic parent. */									// 05/13/99 AM.
	con->up = hier;															// 05/13/99 AM.
   }
else	/* Insert concept alphabetically. */
   {
   con->next = pre->next;
   con->prev = pre;
   if (pre->next)
      pre->next->prev = con;
   pre->next = con;
	con->up = CNULL;							// 05/13/99 AM.
   }
return(con);
}


/**************************************************
*						CON_ADD_BASIC
* FUN:	con_add_basic
* SUBJ:	Add a basic concept to the con table.
* CR:	09/15/95 AM.
* RET:	con structure, if successful.
* ASS:	Not allowing multiple concept hierarchies.  Root of
*		hierarchy must be added in a separate place, for
*		efficiency (maybe a con_add_root fun).
* NOTE:	Adds concept to END of hierarchic children.
*
**************************************************/

CON *ACON::con_add_basic(
	_TCHAR *name,				/* Descriptive name for new concept.	*/
	CON *up					/* Hierarchical parent of new concept.	*/
	)
{
CON *con, *cptr;
SYM *sym;
bool dirty;									// 06/29/03 AM.

if (!up)
   {
   _ftprintf(stderr, _T("[con_add_basic: No hierachical parent.]\n"));
   return(CNULL);
   }

con = con_alloc();			/* Get empty concept struct.	*/
sym = asym_->sym_get(name,dirty);		/* Fetch sym for given name. 	*/

/* Add sym as name of concept. */
con->kind = cBASIC;
con->v.vname = sym;

/* Add new concept to its hierarchic parent's hierarchy. */
if (!up->dn)
	{
   up->dn = con;
	con->up = up;																// 11/02/00 AM.
	}
else
   {
   /* Traverse to end of down list. */
   cptr = up->dn;
   while (cptr->next)
      cptr = cptr->next;
   cptr->next = con;
   con->prev = cptr;
	con->up = CNULL;		// Assurance.									// 11/02/00 AM.
   }

// Opt. Moved these into the code above.								// 11/02/00 AM.
/* Point to hierarchic parent. */
//if (!(con->prev))			// Fix.			// 05/13/99 AM.
//	con->up = up;
//else
//	con->up = CNULL;		// Assurance.	// 05/13/99 AM.

return(con);
}

/**************************************************
*						CON_ADD_FIRST
* FUN:	con_add_first
* SUBJ:	Add a basic concept as first child.
* CR:		05/08/99 AM.
* WARN:	con should be detached from hierarchy.  Caller must ensure this.
*
**************************************************/

bool ACON::con_add_first(CON *con, CON *hier)
{
if (!con || !hier)
	return false;

// Point to hierarchic parent.
con->up = hier;

/* Add new concept to its hierarchic parent's hierarchy. */
CON *fst;																		// 11/02/00 AM.
fst = con->next = hier->dn;												// 11/02/00 AM.
if (fst)																			// 11/02/00 AM.
	{
   fst->prev = con;															// 11/02/00 AM.
	fst->up = CNULL;												// FIX.	// 11/02/00 AM.
	}
hier->dn = con;
return true;
}

/**************************************************
*						CON_ADD_NH
* FUN:	con_add_nh
* SUBJ:	Add a basic concept to the con table, no hierarchy attachment.
* CR:	09/15/95 AM.
* RET:	con structure, if successful.
* ASS:	Not allowing multiple concept hierarchies.  Root of
*		hierarchy must be added in a separate place, for
*		efficiency (maybe a con_add_root fun).
*
**************************************************/

CON *ACON::con_add_nh(
	_TCHAR *name				/* Descriptive name for new concept.	*/
	)
{
CON *con;
//CON *cptr;
SYM *sym;
bool dirty;									// 06/29/03 AM.

con = con_alloc();			/* Get empty concept struct.	*/
sym = asym_->sym_get(name,dirty);		/* Fetch sym for given name. 	*/

/* Add sym as name of concept. */
con->kind = cBASIC;
con->v.vname = sym;
return(con);
}


/**************************************************
*						CON_ADD_NTH
* FUN:	con_add_nth
* SUBJ:	Add concept to nth position among children.
* CR:		05/03/99 AM.
*	RET:	The new concept inserted into hierarchy phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	Not checking for redundant concept name, etc.
*
**************************************************/

CON *ACON::con_add_nth(
	CON *hier,
	_TCHAR *name,
	int pos			// position to insert into.  if = 0, at end of list.
	)
{
CON *con;
//CON *phr;
SYM *sym;
//int res;
bool dirty;									// 06/29/03 AM.

if (!hier || str_empty(name))
   return(CNULL);

if (pos == 0)			// Add to end of the list.
	return con_add_basic(name, hier);

if (!(sym = asym_->sym_get(name,dirty)))		/* Fetch sym for given name. 	*/
   return(CNULL);

/* Traverse to position of down list. */
bool first = false;
if (pos == 1)
	first = true;			// First position is special.

CON *cptr;
if (!(cptr = hier->dn))
	{
	if (pos != 1 &&															// 07/01/99 AM.
		 pos != 0)													// FIX.	// 11/02/00 AM.
		return CNULL;															// 07/01/99 AM.
	// Add it as a child of hier.
	con = con_alloc();														// 07/01/99 AM.
	con->kind = cBASIC;														// 07/01/99 AM.
	con->v.vsym = sym;														// 07/01/99 AM.
	con->up = hier;															// 07/01/99 AM.
	hier->dn = con;															// 07/01/99 AM.
	return con;																	// 07/01/99 AM.
	}

while (--pos > 1)
	{
	if (!cptr)
		return CNULL;
	cptr = cptr->next;	// Prior position.
	}

con = con_alloc();			/* Get empty concept struct.	*/

/* Add sym as name of concept. */
con->kind = cBASIC;
con->v.vsym = sym;

// Attach to siblings, etc.
CON *ptr;
if (first)
	{
	/* Point to hierarchic parent. */
	con->up = hier;
	ptr = hier->dn;
	hier->dn = con;
	con->next = ptr;
	if (ptr)						// Fix.	// 05/04/99 AM.
		{
		ptr->prev = con;
		ptr->up = NULL;		// FIX	// 12/13/99 AM.
		}
	}
else
	{
	// Insert.
	con->next = cptr->next;
	con->prev = cptr;
	if (cptr->next)
		cptr->next->prev = con;
	cptr->next = con;
	}
return(con);
}


/**************************************************
*						CON_ADD_NTH
* FUN:	con_add_nth
* SUBJ:	Add concept to nth position among children.
* CR:		05/13/99 AM.
* RET:	The new concept inserted into hierarchy phrase.
* ARG:	pos = 0 means add at the end of the list.
* NOTE:	VARIANT.  Takes an existing detached concept.
*
**************************************************/

CON *ACON::con_add_nth(
	CON *hier,
	CON *con,
	int pos			// position to insert into.  if = 0, at end of list.
	)
{

if (!hier || !con)
   return(CNULL);
if (con->up || con->prev)
	return CNULL;

CON *cptr;
if (!(cptr = hier->dn))
	{
	if (pos != 0 && pos != 1)		// 07/01/99 AM.
		return CNULL;					// 07/01/99 AM.
	// Add it as a child of hier.
	con->up = hier;					// 07/01/99 AM.
	hier->dn = con;					// 07/01/99 AM.
	return con;							// 07/01/99 AM.
	}

/* Traverse to position of down list. */
bool first = false;

if (pos == 0)			// Add to end of the list.
	{
	// Get last concept of list.
	for (; cptr->next; cptr = cptr->next)
		;
	}
else if (pos == 1)
	first = true;			// First position is special.

while (--pos > 1)
	{
	if (!cptr)
		return CNULL;
	cptr = cptr->next;	// Prior position.
	}

// Attach to siblings, etc.
CON *ptr;
if (first)
	{
	/* Point to hierarchic parent. */
	con->up = hier;
	ptr = hier->dn;
	hier->dn = con;
	con->next = ptr;
	if (ptr)						// Fix.	// 05/04/99 AM.
		{
		ptr->prev = con;
		ptr->up = NULL;		// FIX.	// 12/13/99 AM.
		}
	}
else
	{
	// Insert.
	con->next = cptr->next;
	con->prev = cptr;
	if (cptr->next)
		cptr->next->prev = con;
	cptr->next = con;
	}
return(con);
}


/**************************************************
*						CON_ADD_PROXY
* FUN:	con_add_proxy
* SUBJ:	Add a proxy concept to the con table, no hierarchy attachment.
* CR:	10/19/95 AM.
* RET:	con structure, if successful.
*
**************************************************/

CON *ACON::con_add_proxy(
	CON *gen				/* The concept that proxy represents. */
	)
{
CON *con;
//CON *cptr;
//SYM *sym;

if (!gen)
   return(CNULL);

con = con_alloc();			/* Get empty concept struct.	*/
con->kind = cPROXY;
con->v.vcon = gen;
return(con);
}


/**************************************************
*						CON_ADD_WORD_FIRST
* FUN:	con_add_word_first
* SUBJ:	Add a word concept to start of hierarchic children.
* CR:	10/25/95 AM.
* RET:	con structure, if successful.
* ASS:	Conan - For quick addition of words.
*
**************************************************/

CON *ACON::con_add_word_first(
	_TCHAR *name,				/* Descriptive name for new concept.	*/
	CON *hier				/* Hierarchical parent of new concept.	*/
	)
{
CON *con;
SYM *sym;
bool dirty;									// 06/29/03 AM.

if (!hier)
   {
   _ftprintf(stderr, _T("[con_add_word_first: No hierachical parent.]\n"));
   return(CNULL);
   }

sym = asym_->sym_get(name,dirty);		/* Fetch sym for given name. 	*/
if (!sym || sym->con)
   return(CNULL);

con = con_alloc();			/* Get empty concept struct.	*/

/* Add sym as name of concept. */
con->kind = cWORD;
con->v.vsym = sym;
sym->con = con;

/* Add new concept to its hierarchic parent's hierarchy. */
con_add_first(con, hier);			// 05/08/99 AM.
/*
// Point to hierarchic parent.
con->up = hier;
con->next = hier->dn;
if (hier->dn)
   hier->dn->prev = con;
hier->dn = con;
*/

return(con);
}


/**************************************************
*						CON_CHILD_ALPHA
* FUN:	con_child_alpha
* SUBJ:	Find name in concept's hierarchic children (alphabetically ordered).
* CR:	10/24/95 AM.
* RET:	child - Named child, if found.
*		(up) pre - Location where named concept should be added.
*			(In case someone wants to add it if absent.)
*
**************************************************/

CON *ACON::con_child_alpha(
	CON *hier,
	_TCHAR *name,
	/*UP*/
	CON **pre
	)
{
CON *phr;
int res;

*pre = CNULL;
if (!hier || str_empty(name))
   return(CNULL);
phr = hier->dn;
if (!phr)
   return(CNULL);

while (phr)
   {
   res = _tcscmp(name, con_str(phr));	/* Compare lexically. */
   if (res < 0)
      return(CNULL);	/* Name should precede phr. */
   else if (res == 0)
      return(phr);		/* Found it. */
   *pre = phr;			/* phr precedes name lexically. */
   phr = phr->next;
   }
return(CNULL);
}


/**************************************************
*						ID_GET_CHILD
* FUN:	id_get_child
* SUBJ:	Get immediate named child of given hierarchy concept.
* CR:	12/3/95 AM.
* RET:	id - child of given hier concept.
*
**************************************************/

ID ACON::id_get_child(
	ID id,
	_TCHAR *name
	)
{
CON *con;
con = con_get_child(Con(id), name);
if (con)
   return(con->id);
return(INULL);
}


/**************************************************
*						GET_COMPONENT
* FUN:	get_component
* SUBJ:	Get componet of a concept's path.
* CR:		04/25/00 AM.
* RET:	concept, if possible, else null.
* NOTE:	Helper function for path_to_con.
**************************************************/

CON *ACON::get_component(
	CON *root,		// Parent concept to look under, if any.
	_TCHAR *compo		// Next component of path.
	)
{
if (!compo || !*compo)
	return root;		// Keep the current path as is.
if (root)			// Get compo under root.
	return con_get_child(root, compo);

// Root is absent.  So compo should equal "concept".
if (!_tcscmp(compo, _T("concept")))
	return c_cg_CONCEPT;			// Return root of hierarchy.

// We can be lenient and allow it to be a direct child of "concept", ie, root of hierarchy.
std::_t_cerr << _T("[Path should start with \"concept\"]") << std::endl;
return con_get_child(c_cg_CONCEPT, compo);
}


/**************************************************
*						PATH_TO_CON
* FUN:	path_to_con
* SUBJ:	Convert hierarchy path string to concept.
* CR:		04/25/00 AM.
* RET:	concept, if possible, else null.
* NOTE:	Needed for NLP++.
*			Does NOT create concept if it doesn't already exist.
* Opt:	Would like to munge string, but because of backslash escaping, need to copy
*			each component of path, in general.
*
**************************************************/

CON *ACON::path_to_con(
	_TCHAR *path
	)
{
if (!path || !*path)
	{
	std::_t_cerr << _T("[path_to_con: Warn: Given null path.]") << std::endl;
	return 0;
	}


CON *con = 0;
_TCHAR *ptr = path;				// Lookahead char.
_TCHAR *start = 0;				// First position of current component of path.
									// If not set, then at end of double quote region.
bool esc = false;				// If prev char was an escaping backslash.
bool inside = false;			// If inside double quotes.
_TCHAR buf[MAXSTR];				// Store current component of path.
buf[0] = '\0';
_TCHAR *bptr = buf;				// Ptr to component.
//con = c_cg_CONCEPT;		// Start at root.
// Traverse each piece of path.  Simultaneously traverse concept hierarchy.
for (;;)
	{
	switch (*ptr)				// Look at current char.
		{
		case '\0':
			*bptr = '\0';
			con = get_component(con, buf);	// Get final component of path, if any.
			if (!con)
				std::_t_cerr << _T("[path_to_con: Warn: Couldn't find con for path=") << path << _T("]") << std::endl;
			return con;			// Chars exhausted, return concept found.
		case ' ':
		case '\t':
		case '\n':
		case '\r':				// Whitespace.
			if (esc				// I suppose we'll allow escape of whitespace.
				 || inside)		// Or we're inside double quotes.
				{
				// Treat as ordinary char.
				*bptr++ = *ptr++;
				}
			else					// Else, it's a separator.
				{
				*bptr = '\0';		// Terminate component.
				if (!(con = get_component(con, buf)))	// Get latest part of path.
					return 0;		// Failed.
				buf[0] = '\0';		// Reset.
				bptr = buf;			// Reset.
				++ptr;
				}
			break;
		case '\"':				// Start or end of component.
			if (esc)
				*bptr++ = *ptr++;		// Ordinary char.
			else if (inside)	// End of component.
				{
				*bptr = '\0';
				if (!(con = get_component(con, buf)))	// Get latest part of path.
					return 0;		// Failed.
				++ptr;
				buf[0] = '\0';
				bptr = buf;			// Reset.
				inside = false;
				}
			else if (buf[0])	// In the middle of a component.
				*bptr++ = *ptr++;		// Grukky.  eg, "ab\"c".
			else							// Start of component.
				{
				inside = true;
				++ptr;
				}
			break;
		case '\\':				// Backslash.
			if (esc)				// Prev was an escaping backslash.
				{
				*bptr++ = *ptr++;	// Convert current to a simple backslash.
				}
			else
				{
				esc = true;
				++ptr;
				}
			break;
		default:					// Accumulate char.
			// Should probably have punct marks be separators, but who cares for now.
			*bptr++ = *ptr++;
			break;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//                        GENERAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////


/**************************************************
*						CON_GET_CHILD
* FUN:	con_get_child
* SUBJ:	Get immediate named child of given hierarchy concept.
* CR:	10/19/95 AM.
* RET:	con - child of given hier concept.
*
**************************************************/

CON *ACON::con_get_child(
	CON *hier,
	_TCHAR *name
	)
{
CON *childs;

if (!hier || !name || !*name)
   {
   _ftprintf(stderr, _T("[con_get_child: Bad input.]\n"));
   return(CNULL);
   }
childs = hier->dn;
while (childs)
   {
   if (!_tcscmp(name, childs->v.vname->str))
      return(childs);
   childs = childs->next;
   }
return(CNULL);
}


/**************************************************
*						CON_KIND_STR
* FUN:	con_kind_str
* SUBJ:	Generate a kind string for the given concept.
* CR:	9/20/95 AM.
*
**************************************************/

_TCHAR *ACON::con_kind_str(
	CON *con
	)
{
_TCHAR *emp;
//CON *bas;

emp = _T("");

if (!con)
   return(emp);
switch (con->kind)
   {
   case cNO_CON:	return(_T("nul"));
   case cPROXY:		return(_T("prx"));
   case cBASIC:		return(_T("bas"));
   case cWORD:		return(_T("wrd"));
   /*
   case cPHRASE:
   case cRULE:
   */
   case cNUM:
   case cCHAR:
   default:
      _ftprintf(stderr, _T("[con_str: Unimplemented kind.]\n"));
      return(emp);
   }
}


/**************************************************
*						CON_STR
* FUN:	con_str
* SUBJ:	Generate a string for the given concept.
* CR:		9/20/95 AM.
*
**************************************************/

_TCHAR *ACON::con_str(
	CON *con
	)
{
_TCHAR *emp;
CON *bas;

emp = _T("");

if (!con)
   return(emp);
switch (con->kind)
   {
   case cNO_CON:
      return(emp);
   case cPROXY:
      {
      /* Get the referenced concept. */
      bas = con->v.vcon;
      if (!bas)
         return(emp);
      if (bas->kind == cPROXY)
         {
         _ftprintf(stderr, _T("[con_str: Error]\n"));
         return(emp);
         }
      return(ASYM::sym_str(bas->v.vname));
      }
   case cBASIC:
      return(ASYM::sym_str(con->v.vname));
   case cWORD:
      return(ASYM::sym_str(con->v.vsym));
   /*
   case PHRASE:
   case RULE:
   */
   case cNUM:
   case cCHAR:
   default:
      _ftprintf(stderr, _T("[con_str: Unimplemented kind.]\n"));
      return(emp);
   }
   
}


/**************************************************
*						CON_STR_PRET
* FUN:	con_str_pret
* SUBJ:	Generate a pretty string for the given concept.
* CR:		6/23/96 AM.
*
**************************************************/

_TCHAR *ACON::con_str_pret(CON *con)
{
_TCHAR *str;

str = con_str(con);
if (!str || !*str)
   str = _T("EMPTY STR");
else if (!_tcscmp(str, _T("\n")))
   str = _T("\\n");
else if (!_tcscmp(str, _T("\t")))
	str = _T("\\t");
else if (!_tcscmp(str, _T(" ")))
	str = _T("\\s");
return str;
}
 

/**************************************************
*						NC
* FUN:	nc
* SUBJ:	Get concept referenced by a proxy.
* CR:		11/9/95 AM.
* NOTE:	"NC" stands for "node's concept".
*
**************************************************/

CON *ACON::nc(
	CON *proxy
	)
{
CON *con;

if (!proxy)
   return(CNULL);
if (proxy->kind != cPROXY)
   {
   _ftprintf(stderr, _T("[nc: Not given proxy.]\n"));
   return(CNULL);
   }
con = proxy->v.vcon;
if (!con)
   {
   _ftprintf(stderr, _T("[nc: proxy has no concept.]\n"));
   return(CNULL);
   }
return(con);
}
 

/**************************************************
*						PHRASE_LEN
* FUN:	phrase_len
* SUBJ:	Get length of the given phrase.
* CR:		7/8/96 AM.
*
**************************************************/

long ACON::phrase_len(CON *phr)
{
long len;

len = (long)0;
while (phr)
	{
	++len;
	phr = phr->next;
	}
return len;
}

 
/**************************************************
*						CON_NTH
* FUN:	con_nth
* SUBJ:	Get the nth concept in a phrase.
* CR:	11/15/95 AM.
* RET:	elt - nth element, if present.
* NOTE:	05/03/99 AM. Using nth==0 to mean the end of phrase.
*
**************************************************/

CON *ACON::con_nth(
	CON *phr,
	long nth
	)
{
if (!phr || nth < (long) 0)		// 05/03/99 AM.
   return(CNULL);
if (nth == 0)							// 05/03/99 AM.
	return con_last(phr);			// 05/03/99 AM.
while (phr)
   {
   if (--nth == (long) 0)
      return(phr);
   phr = phr->next;
   }
return(CNULL);
}

/**************************************************
*						CON_ORD
* FUN:	con_ord
* SUBJ:	Get the position of node in its phrase.
* CR:		10/09/05 AM.
* RET:	num - position in phrase, 1 based.  0 if not a node.
*
**************************************************/

long ACON::con_ord(
	CON *con
	)
{
if (!con)
   return 0;
if (con->kind != cPROXY)
	return 0;

long ord = 1;
while ((con = con->prev))
   ++ord;
return ord;
}

/**************************************************
*						NODE_OWNER
* FUN:	node_owner
* SUBJ:	Get concept owning current node and phrase.
* CR:		10/09/05 AM.
* RET:	con - owning concept, if given a node.
*
**************************************************/

CON *ACON::node_owner(
	CON *node
	)
{
if (!node)
   return 0;
if (node->kind != cPROXY)
	return 0;

// Get first node in phrase.
while (node->prev)
   node = node->prev;
// Get first node's owning concept.
return phrase_con(node);
}


/**************************************************
*						CON_LAST
* FUN:	con_last
* SUBJ:	Get the last concept in a phrase.
* CR:		05/03/99 AM.
* RET:	last element, if present.
*
**************************************************/

CON *ACON::con_last(
	CON *phr
	)
{
if (!phr)
	return CNULL;
while (phr->next)
	phr = phr->next;
return phr;
}


/**************************************************
*						CON_TO_PATH
* FUN:	con_to_path
* SUBJ:	Build hierarchy path string for concept.
* CR:		04/29/99 AM.
* NOTE:	04/30/99 AM. Building quoted string for every concept.
* OPT:	Using non-optimal but simple recursion.
*			Should quote only what needs it.
*
**************************************************/

_TCHAR *ACON::con_to_path(		// 04/29/99 AM.
	CON *con,
	/*UP*/
	_TCHAR *buf			// Buffer to fill with path.
	)
{
*buf = '\0';
if (!con)
	{
	std::_t_cerr << _T("[con_to_path: Given null concept.]") << std::endl;
	return (_TCHAR *) NULL;
	}

// Glom my ancestors, then glom me.
_TCHAR *end;
end = buf;
con_to_path_r(con, /*UP*/ end);	// Recursive.
*end = '\0';							// Terminate path string.
return buf;
}


/**************************************************
*						CON_TO_PATH_R
* FUN:	con_to_path_r
* SUBJ:	Build hierarchy path string for concept. Recursive.
* CR:		04/29/99 AM.
* OPT:	Using non-optimal but simple recursion.
*
**************************************************/

void ACON::con_to_path_r(
	CON *con,
	/*UP*/
	_TCHAR* &end			// First empty loc in buffer.
	)
{
CON *parent;
if ((parent = con_parent(con)))
	{
	// Glom my ancestors.
	con_to_path_r(parent, end);

	// I had ancestors, so separate me from them.
	*end++ = ' ';					// Add separation space.
	}

// Glom me.
_TCHAR *str;
_TCHAR buf[2048];
str = con_str(con);
prep_str(buf, str);
str = buf;
*end++ = '"';
while ((*end++ = *str++))	// Concept's string.
	;							// Copy string into buffer.
//end--;						// Back to first empty space.
*(end-1) = '"';
}


/**************************************************
*						CON_REMOVE
* FUN:	con_remove
* SUBJ:	Remove concept from hierarchy.
* CR:		05/03/99 AM.
* NOTE:	Not deleting nor wiping out data structures.
*			(Can't delete, but could zero it out in the concept table.)
* RET:	True if successful.
*
**************************************************/

bool ACON::con_remove(CON *con)
{
if (!con)
	return false;
if (!(con->prev) && !(con->up))
	return false;		// Detached already, or root of tree.

// Detach from parent and siblings.
if (!(con->prev))			// First in list.
	{
	CON *parent, *ptr;
	parent = con->up;
	ptr = con->next;
	con->next = CNULL;
	if (ptr)
		{
		ptr->prev = CNULL;
		ptr->up = parent;											// FIX.	// 11/02/00 DD.
		}
 	parent->dn = ptr;
	con->up = CNULL;												// FIX.	// 05/13/99 AM.
	}
else
	{
	con->prev->next = con->next;
	if (con->next)
		con->next->prev = con->prev;
	con->next = CNULL;
	con->prev = CNULL;
	}
return true;
}


/**************************************************
*						CON_PARENT
* FUN:	con_parent
* SUBJ:	Get concept's parent in hierarchy.
* CR:		05/13/99 AM.
* NOTE:	A long-overdue function.  Apparently, all concepts
*			in a list were getting pointer to the hierarchy parent.
*			Yet this wasn't being updated when moving concepts
*			around.
*
**************************************************/

CON *ACON::con_parent(CON *con)
{
if (!con)
	return CNULL;
if (con->kind == cPROXY)
	return CNULL;
for (; con->prev; con = con->prev)
	;				// Find first in list.
return con->up;
}


/////////////////////////////////////////////////////////////////////////////

#ifdef CONAN_OLD
/**************************************************
*						CON_BIND_ROOT
* FUN:	con_bind_root
* SUBJ:	Bind root concept of hierarchy to its variable.
* CR:	10/21/95 AM.
* ASS:	For Conan applications only. Consh uses con_add_root.
*		Assumes root concept is concept number 1 in segment 0.
* NOTE:	This is the way to tell Conan what the root of the hierarchy is.
*		This should be a function generated by Consh, so that it won't be
*		a hard-wired thing.
*		11/28/95 AM. This will now disappear.
*
**************************************************/

bool
con_bind_root()
{
CON *con;

if (cg_CONCEPT)
   {
   _ftprintf(stderr, _T("[con_bind_root: Root global already defined.]\n"));
   return(false);
   }

con = Con_segs[0] + 1;	/* HARD-WIRED.  Root is always concept number 1. */

if (con->kind != BASIC)
   {
   _ftprintf(stderr, _T("[con_bind_root: Root concept not defined yet.]\n"));
   return(false);
   }

cg_CONCEPT = con->id;
return(true);
}
#endif

#ifdef CON_OLD
/**************************************************
*						CON_INI
* FUN:	con_ini
* SUBJ:	Initialize concept table management.
* CR:	9/10/95 AM.
* NOTE:	Call an automatically generated init function for the
*		compiled portion of the concept table.
*
**************************************************/

void
con_ini()
{
/*
Automatically generated init function will fill con segment
array slots with addresses of prebuilt segments, and will
set global variables indicating the number of prebuilt segments,
as well as ptrs to first open locus in these segments.
*/
cc_con_ini(Con_segs, &Con_segs_tot, &Con_size, &Con_ptr, &Con_seg);
HW_con_segs = Con_segs_tot;
}
#endif
