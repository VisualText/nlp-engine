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
*									SYM.C
*
* FILE:	consh.�/sym.c
* SUBJ:	Symbol table manager for Conan.
* NOTE:	For the compiled analyzer (i.e., Conan), implementing highly optimized
*		(hopefully) (for Macintosh) primitive table managers from scratch.
*		As always (eg VOX), our symbol table is a simple hash table.
*		To optimize, will store each table to fit in multiple 32K segments,
*		with a segment manager to tell which segment to lookup in.
*		Segment # = (hash * SYM_SIZ) / 32K.  Segment numbering is zero-based.
*		Hash function will be modulo (TOT_SYM_SEGS * 32K) / SYM_SIZ, so that
*		every lookup will automatically fall within range.
*		A separate set of segments will manage the conflict set, i.e., when
*		more
*		than one string hashes to the same location.  Conflict chains will be
*		extensible, to allow dynamic addition of symbols during text analysis.
*		9/6/95 AM. Conflict stuff should point to last used entry, rather than
*		first unused slot.  This way, a needless system call will be avoided.
*		(Rarely! But we lose nothing in handling it this way.)
* OPT:	Next ptr will be the unique offset of the next sym in conflict chain.
*		(Would like it to be a ptr, but not sure if this is possible.)
*		08/30/95 AM. Will build hard-wired pointers.  Generating code using
*		these is slightly tricky (must compute offsets based on addresses).
*		
* CR:	5/02/95 AM.
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
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/kbm_alloc.h"				// 08/10/99 AM.

//#define DEBUG



int ASYM::count_ = 0;			// Counter for ASYM class.				// 08/14/02 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

ASYM::ASYM(
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

ASYM::~ASYM()
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


//void	ASYM::setCon(void *x)	{con_	= x;}

///////////


/**************************************************
*						SYM_ADD
* FUN:	sym_add
* SUBJ:	Add string to symbol and string tables.
* CR:	09/06/95 AM.
* RET:	ptr to symbol table entry for string.
* NOTE:	Primitive.  Doesn't check if string is already in
*		symbol and string tables.
*		Retains case (upper/lower) of given string.
*
**************************************************/

SYM *
ASYM::sym_add(
	_TCHAR *str				/* String to add.	*/
	)
{
_TCHAR *ptr;
long off;
//long str_off;
long hash;
int segno;
SYM *sym, *snew;

if (!Str_full(str))
   return(SNULL);

/* Add string to string table. */
//if (!st_add(str, &ptr, &str_off))				// 04/16/99 AM.
if (!ast_->st_add(str, &ptr))
   return(SNULL);

/* Get hash location. */
hash = sym_hash(str);		/* Get hash number.	*/

/* Get segment to lookup in, and offset within segment. */
segno = (int) (hash / SYM_SEG_SIZE);	// DIV.
off   = hash % SYM_SEG_SIZE;				// DIV.

/* Get entry in sym table. */
sym = Sym_segs[segno] + off;

/* If loc is empty, use it. */
if (str_empty(sym->str))
   {
   sym->str = ptr;
   return(sym);
   }

/* Else, go to end of hashed location's chain. */
while (sym->chain)
   sym = sym->chain;

/* Add to end of chain. */
snew = sym_alloc();
if (!snew)
   return(SNULL);
sym->chain = snew;

snew->str = ptr;
snew->chain = SNULL;			/* Possibly optional. */
return(snew);
}


/**************************************************
*						SYM_ALLOC
* FUN:	sym_alloc
* SUBJ:	Allocate sym data structure.
* CR:	09/06/95 AM.
* RET:	ptr to sym structure, if successful.
* NOTE:	Fetch next available space in Conflict table.
*		If at end of segment, allocate new segment.
*
**************************************************/

SYM *
ASYM::sym_alloc()
{
//SYM *sym;
long off;		/* Offset in segment. */

off = (long) (1 + Conf_seg_p - Sym_segs[Conf_seg_curr]);

if (off < SYM_SEG_SIZE)
   {
   /* Use next loc. */
   ++Conf_seg_p;
	// Line below may not be needed. // 06/09/99 AM.
	sm_zero((_TCHAR *)Conf_seg_p, sizeof(SYM));	// 06/09/99 AM.
   return(Conf_seg_p);
   }

/* Need a more global check on space, also. */
if (Sym_segs_tot >= SYM_SEGS_MAX)
   {
   _ftprintf(stderr, _T("[sym_alloc: Sym segment table overflow.]\n"));
   exit(1);
   }

/* Allocate a new segment. */
// FIX // 08/10/99 AM. Not using sm_alloc.
Sym_segs[Sym_segs_tot] = (SYM *) kbm_alloc(SYM_SEG_SIZE, sizeof(SYM));
Conf_seg_curr = Sym_segs_tot;
++Sym_segs_tot;
/* Sym_size += SYM_SEG_SIZE; */
Conf_seg_p = Sym_segs[Conf_seg_curr];
sym_seg_zero(Conf_seg_p);	// 06/09/99 AM.
return(Conf_seg_p);		/* Return 1st loc in segment. */
}

/**************************************************
*						SYM_SEG_ZERO
* FUN:	sym_seg_zero
* SUBJ:	Zero out entire symbol table segment.
* CR:		06/09/99 AM.
*
**************************************************/

bool ASYM::sym_seg_zero(SYM *seg)
{
sm_zero((_TCHAR *)seg, sizeof(SYM) * SYM_SEG_SIZE);	// 06/09/99 AM.
return true;
}


/**************************************************
*						SYM_CLEAN
* FUN:	sym_clean
* SUBJ:	Clean up symbol table management prior to exit.
* CR:	09/10/95 AM.
* NOTE:	Hard-wired segs go to (HW_sym_segs - 1).
*
**************************************************/

void
ASYM::sym_clean()
{
int ii;
/* Free all allocated segments. */
for (ii = SYM_SEGS_TOT; ii < Sym_segs_tot; ii++)
	{
   //free(Sym_segs[ii]);		// 04/16/99 AM.
	delete [] (Sym_segs[ii]);	// 04/16/99 AM.
	Sym_segs[ii] = SNULL;		// 04/16/99 AM.
	}
}


/**************************************************
*						SYM_CURR_OFF
* FUN:	sym_curr_off
* SUBJ:	Return offset of last full loc in sym table.
* CR:	10/4/95 AM.
* RET:	Offset from base of current segment.
* NOTE:	Modularity fn.
*
**************************************************/

long
ASYM::sym_curr_off()
{
return((long) (Conf_seg_p - Sym_segs[Conf_seg_curr]));
}


/**************************************************
*						SYM_FIND
* FUN:	sym_find
* SUBJ:	See if given string is in symbol table.
* CR:	5/02/95 AM.
* RET:	sym - If found, return the sym. Else NULL.
*
**************************************************/

SYM *
ASYM::sym_find(
	_TCHAR *str
	)
{
if (!Str_full(str))
   return(SNULL);

long hash = sym_hash(str);		/* Get hash number.	*/
/* Get segment to lookup in, and offset within segment. */
int segno = (int) (hash / SYM_SEG_SIZE);	// DIV.
long off   = hash % SYM_SEG_SIZE;				// DIV.

/* 9/10/95 AM. Updating this code. */
SYM *sym = Sym_segs[segno] + off;

if (str_empty(sym->str))	/* Empty hash location. */
   return(SNULL);

/* Chain down the conflict set. */
while (sym && _tcscmp(str, sym->str))
   sym = sym->chain;
return(sym);
}


/**************************************************
*						SYM_GET
* FUN:	sym_get
* SUBJ:	Find else make a symbol for given string.
* CR:	9/10/95 AM.
* RET:	sym - Found or created symbol for string.
* OPT:	More efficient to copy and modify sym_find code, eventually.
*		(But for development, this is the nicer form.)
*		06/29/03 AM.	Better late than never. Took code from sym_find,sym_add.
*
**************************************************/

#ifdef OLD_030629_
SYM *ASYM::sym_get(
	_TCHAR *str
	)
{
SYM *sym;

sym = sym_find(str);
if (sym)
   return(sym);
return(sym_add(str));
}
#endif

SYM *ASYM::sym_get(
	_TCHAR *str,
	bool &dirty		// If KB has changed.								// 06/29/03 AM.
	)
{
if (!Str_full(str))															// 06/29/03 AM.
   return(SNULL);																// 06/29/03 AM.

long hash = sym_hash(str);		// Get hash number.					// 06/29/03 AM.
// Get segment to lookup in, and offset within segment.
int segno = (int) (hash / SYM_SEG_SIZE);		// DIV.				// 06/29/03 AM.
long off   = hash % SYM_SEG_SIZE;				// DIV.				// 06/29/03 AM.

SYM *sym = Sym_segs[segno] + off;										// 06/29/03 AM.

if (sym->str && *sym->str)	// Non-empty hash loc.					// 06/29/03 AM.
	{
	if (!_tcscmp(str, sym->str))	// Found in 1st spot.			// 06/29/03 AM.
		return sym;																// 06/29/03 AM.

	// Chain down the conflict set.
	// Get last sym in chain, for adding purposes.					// 06/29/03 AM.
	while (sym->chain)
		{
		sym = sym->chain;														// 06/29/03 AM.
		if (!_tcscmp(str, sym->str))										// 06/29/03 AM.
			return sym;															// 06/29/03 AM.
		}
	// At last sym in chain with no match.								// 06/29/03 AM.
	SYM *snew = sym_alloc();												// 06/29/03 AM.
	if (!snew)																	// 06/29/03 AM.
		return(SNULL);															// 06/29/03 AM.
	sym->chain = snew;														// 06/29/03 AM.
	sym = snew;																	// 06/29/03 AM.
	}

dirty = true;																	// 06/29/03 AM.

/* Add string to string table. */
_TCHAR *ptr;																		// 06/29/03 AM.
if (!ast_->st_add(str, &ptr))												// 06/29/03 AM.
   return(SNULL);																// 06/29/03 AM.

sym->str = ptr;																// 06/29/03 AM.
sym->chain = SNULL;	// Possibly optional.							// 06/29/03 AM.
return(sym);																	// 06/29/03 AM.
}


/**************************************************
*						SYM_HARD_INI
* FUN:	sym_hard_ini
* SUBJ:	Initialize sym table manager with hard-coded data.
* CR:	10/1/95 AM.
* ASS:	Code is compiled with automatically generated sym+conf table
*		code (Sym_ini.c Sym_ini.h Sym0.c Sym1.c ...).
* NOTE:	Handshake between the sym mgr and the generated sym table.
*
**************************************************/

bool																// 05/05/01 AM.
ASYM::sym_hard_ini(
	long seg_size,
	long hash_size,
	int hash_tot,
	int segs_hard_tot,		// Number of hard-coded segs.			// 02/16/01 AM.
	int segs_tot,				// Total number of initial segs.		// 02/16/01 AM.
	int seg_curr,
	SYM *seg_p
	)
{
// CHECKING FOR VALID INITIALIZATION.									// 05/05/01 AM.
if ((seg_size <= 0) || (hash_size <= 0))								// 05/05/01 AM.
	return false;

SYM_SEG_SIZE  = seg_size;
SYM_HASH_SIZE = hash_size;
SYM_HASH_TOT  = hash_tot;

SYM_SEGS_TOT  = segs_hard_tot;		// Hard-coded segs.			// 02/16/01 AM.
Sym_segs_tot  = segs_tot;				// Total number of segs.	// 02/16/01 AM.
Conf_seg_curr = CONF_SEG_CURR = seg_curr;
Conf_seg_p    = CONF_SEG_P    = seg_p;
return true;																	// 05/05/01 AM.
}


/**************************************************
*						SYM_HASH
* FUN:	sym_hash
* SUBJ:	Build a sym table offset for given string.
* CR:	5/02/95 AM.
* RET:	hash - hash number for given string.
* NOTE:	
*
**************************************************/

#ifdef OLD_030629_
long
ASYM::sym_hash(
	_TCHAR *str			// 10/05/99 AM.
	)
{
_TUCHAR ch;					// 10/05/99 AM.
long hash;
long warp;

warp = 1;
hash = 0;
while((ch = *str++) != '\0')
   {
   hash += ((long)ch * warp++);
	if (hash > SYM_HASH_SIZE)									// FIX	// 01/08/02 AM.
		hash = hash % SYM_HASH_SIZE;							// FIX	// 01/08/02 AM.
	}

return(hash % SYM_HASH_SIZE);
}
#endif

// Overhauling to conform to hashing in lite.dll					// 06/29/03 AM.
long ASYM::sym_hash(_TCHAR *str)								// 06/29/03 AM.
{
if (!str || !*str)															// 06/29/03 AM.
	{
	// Print error here.
	return 0;			// Reserving zero for bad strings, empty string.
	}

_TUCHAR ch;													// 10/05/99 AM.
unsigned long val=0;												// 06/29/03 AM.
unsigned long warp=113;											// 06/29/03 AM.
unsigned long ii=1;												// 06/29/03 AM.

while ((ch = *str++) != '\0')
	{
	warp += 22;																	// 06/29/03 AM.
	val += ((unsigned long) ch * warp									// 06/29/03 AM.
						* warp);													// 06/29/03 AM.
	if (val > (unsigned long) SYM_HASH_SIZE)							// 06/29/03 AM.
		val = val % SYM_HASH_SIZE;											// 06/29/03 AM.
	if (++ii > HASH_MAX)														// 06/29/03 AM.
		return val;																// 06/29/03 AM.
	}
return val % SYM_HASH_SIZE;												// 06/29/03 AM.
}


/**************************************************
*						SYM_HASH_SIZE
* FUN:	sym_hash_size
* SUBJ:	Return size of hashed sym table.
* CR:	10/5/95 AM.
* RET:	SYM_HASH_SIZE.
* NOTE:	Modularity fn.
*
**************************************************/

long
ASYM::sym_hash_size()
{
return(SYM_HASH_SIZE);
}


/**************************************************
*						SYM_HASH_TOT
* FUN:	sym_hash_tot
* SUBJ:	Return number of hashed sym table segments.
* CR:	10/5/95 AM.
* RET:	SYM_HASH_TOT.
* NOTE:	Modularity fn.
*
**************************************************/

int
ASYM::sym_hash_tot()
{
return(SYM_HASH_TOT);
}


/**************************************************
*						SYM_INI
* FUN:	sym_ini
* SUBJ:	Initialize symbol table management.
* CR:	08/30/95 AM.
* NOTE:	Call an automatically generated init function for the
*		compiled portion of the symbol table (if any).  In all
*		these tables, the sole difference between static and dynamic
*		is that the static isn't freed up, while the dynamic is.
*		So, we track the place where static segments end and dynamics
*		begin.
*
**************************************************/

#ifdef SYM_OLD
void
ASYM::sym_ini()
{
/*
Automatically generated init function will fill sym segment
array slots with addresses of prebuilt segments, and will
set global variables indicating the number of prebuilt segments,
as well as ptrs to first open locus in these segments.
*/
cc_sym_ini(Sym_segs, &Sym_segs_tot, &Sym_size, &Conf_ptr, &Conf_seg);
HW_sym_size = Sym_size;
HW_sym_segs = Sym_segs_tot;
}
#endif


/**************************************************
*						SYM_OFF
* FUN:	sym_off
* SUBJ:	Compute offset of sym table pointer.
* CR:	10/4/95 AM.
* RET:	(up) seg - segment number in table (or neg).
*		(up) off - offset within segment.
* ALG:	Traverse segment table. If pointer falls within seg bounds,
*		compute its offset in seg.
*
**************************************************/

void ASYM::sym_off(
	SYM *sym,
	/*UP*/
	int *seg,
	long *off
	)
{
int ii;

if (sym)
   {
   for (ii = 0; ii < Sym_segs_tot; ii++)
      {
      if (sym >= Sym_segs[ii] && sym < (Sym_segs[ii] + SYM_SEG_SIZE))
         {
         *seg = ii;
         *off = sym - Sym_segs[ii];
         return;
         }
      }
   }
*seg = -1;
*off = (long) -1;
}


/**************************************************
*						SYM_PP
* FUN:	sym_pp
* SUBJ:	Pretty-print the symbol table.
* CR:	9/2/95 AM.
* NOTE:	
*
**************************************************/

void
ASYM::sym_pp(
	//FILE *out		// 04/20/99 AM.
	std::_t_ostream *out	// 04/20/99 AM.
	)
{
int ii;
long jj;
long count;
SYM *sym, *ptr;

//fprintf(out, "Symbol Table\n");
//fprintf(out, "------------\n");
*out << _T("Symbol Table") << std::endl;
*out << _T("------------") << std::endl;

/* Traverse the symbol hash table, looking for nonempty locs. */
count = 0;
for (ii = 0; ii < SYM_HASH_TOT; ii++)
   {
   sym = Sym_segs[ii];
   
   for (jj = 0; jj < SYM_SEG_SIZE; jj++)
      {
      if (str_full(sym->str))
         {
         //fprintf(out, "%ld: %s", count, sym->str);
			*out << count << _T(": ") << sym->str;
         
         /* Traverse the chain. */
         ptr = sym;
         while ((ptr = ptr->chain) != (SYM *) NULL)
            //fprintf(out, " %s", ptr->str);
				*out << _T(" ") << ptr->str;
         //fprintf(out, "\n");
			*out << std::endl;
         }
      ++sym;
      if (count++ >= SYM_HASH_SIZE - 1)
         return;
      }
   }
}


/**************************************************
*						SYM_SEG_CURR
* FUN:	sym_seg_curr
* SUBJ:	Return current seg num for sym table.
* CR:	10/4/95 AM.
* RET:	Sym_seg_curr.
* NOTE:	Modularity fn.
*
**************************************************/

int
ASYM::sym_seg_curr()
{
return(Conf_seg_curr);
}


/**************************************************
*						SYM_SEG_P
* FUN:	sym_seg_p
* SUBJ:	Get first empty loc of sym table.
* CR:	10/4/95 AM.
* RET:	Return ptr to string for given offset.
* NOTE:	Modularity fn.
*
**************************************************/

SYM *
ASYM::sym_seg_p()
{
return(Conf_seg_p);
}


/**************************************************
*						SYM_SEG_SIZE
* FUN:	sym_seg_size
* SUBJ:	Return size of a sym table segment.
* CR:	10/4/95 AM.
* RET:	SYM_SEG_SIZE
* NOTE:	Modularity fn.
*
**************************************************/

long
ASYM::sym_seg_size()
{
return(SYM_SEG_SIZE);
}


/**************************************************
*						SYM_SEGS_TOT
* FUN:	sym_segs_tot
* SUBJ:	Return total num of segments in sym table.
* CR:	10/4/95 AM.
* RET:	Sym_segs_tot
* NOTE:	Modularity fn.
*
**************************************************/

int
ASYM::sym_segs_tot()
{
return(Sym_segs_tot);
}

/**************************************************
*						SYM_SEG_TABLE
* FUN:	sym_seg_table
* SUBJ:	Return the global segment table for sym table mgmt.
* CR:	10/1/95 AM.
* NOTE:	Modularity fn.
*
**************************************************/

SYM **
ASYM::sym_seg_table()
{
return(Sym_segs);
}

/**************************************************
*						SYM_STAT
* FUN:	sym_stat
* SUBJ:	Display data about symbol table.
* CR:	9/2/95 AM.
* NOTE:	
*
**************************************************/

void
ASYM::sym_stat(
	//FILE *out		// 04/20/99 AM.
	std::_t_ostream *out	// 04/20/99 AM.
	)
{
#ifdef _OBSOLETE_
_ftprintf(out, _T("Sym structure size  = %d\n"), (int) sizeof(SYM));
_ftprintf(out, _T("Sym segment size    = %ld\n"), SYM_SEG_SIZE);
_ftprintf(out, _T("Hash table size     = %ld\n"), SYM_HASH_SIZE);
_ftprintf(out, _T("# of hard hash segs = %d\n"), SYM_HASH_TOT);
_ftprintf(out, _T("# of hard sym segs  = %d\n"), SYM_SEGS_TOT);
_ftprintf(out, _T("# hard filled segs  = %d\n"), CONF_SEG_CURR+1);
_ftprintf(out, _T("Hard filled size    = %ld\n"),
	((long)CONF_SEG_CURR * SYM_SEG_SIZE) +
	(long) (CONF_SEG_P - Sym_segs[CONF_SEG_CURR] + (long) 1) );
_ftprintf(out, _T("Number of segments  = %d\n"),  Sym_segs_tot);
_ftprintf(out, _T("Current conf seg    = %d\n"), Conf_seg_curr);
_ftprintf(out, _T("Conflict offset     = %ld\n"),
               (long) (Conf_seg_p - Sym_segs[Conf_seg_curr]));
_ftprintf(out, _T("Total filled size   = %ld\n"),
	((long)Conf_seg_curr * SYM_SEG_SIZE) +
	(long) (Conf_seg_p - Sym_segs[Conf_seg_curr] + (long) 1) );
#endif

*out << _T("Sym structure size  = ") << (int) sizeof(SYM) << std::endl;
*out << _T("Sym segment size    = ") << SYM_SEG_SIZE << std::endl;
*out << _T("Hash table size     = ") << SYM_HASH_SIZE << std::endl;
*out << _T("# of hard hash segs = ") << SYM_HASH_TOT << std::endl;
*out << _T("# of hard sym segs  = ") << SYM_SEGS_TOT << std::endl;
*out << _T("# hard filled segs  = ") << CONF_SEG_CURR+1 << std::endl;
*out << _T("Hard filled size    = ") <<
	((long)CONF_SEG_CURR * SYM_SEG_SIZE) +
	(long) (CONF_SEG_P - Sym_segs[CONF_SEG_CURR] + (long) 1) << std::endl;
*out << _T("Number of segments  = ") << Sym_segs_tot << std::endl;
*out << _T("Current conf seg    = ") << Conf_seg_curr << std::endl;
*out << _T("Conflict offset     = ") <<
               (long) (Conf_seg_p - Sym_segs[Conf_seg_curr]) << std::endl;
*out << _T("Total filled size   = ") <<
	(((long)Conf_seg_curr * SYM_SEG_SIZE) +
	(long) (Conf_seg_p - Sym_segs[Conf_seg_curr] + (long) 1) ) << std::endl;

}


/**************************************************
*						SYM_STR
* FUN:	sym_str
* SUBJ:	Get string for given sym.
* CR:	9/20/95 AM.
* NOTE:	
*
**************************************************/

_TCHAR *
ASYM::sym_str(SYM *sym)
{
if (sym)
   return(sym->str);
return(_T(""));
}


/**************************************************
*						SYM_RESET
* FUN:	sym_reset
* SUBJ:	Reset and zero out the hashed sym table.
* CR:		06/14/99 AM.
* NOTE:	To enable deletion and creation of KBs.
*
**************************************************/

void
ASYM::sym_reset()
{
long off;

// RESET CONFLICT TABLE CONSTANTS AND VARIABLES.
// Start at end of hashed sym table.
CONF_SEG_CURR = (int) (SYM_HASH_SIZE / SYM_SEG_SIZE);	// DIV.
off   = SYM_HASH_SIZE % SYM_SEG_SIZE;						// DIV.
CONF_SEG_P = Sym_segs[CONF_SEG_CURR] + off;

Conf_seg_curr = CONF_SEG_CURR;
Conf_seg_p    = CONF_SEG_P;

// ZERO OUT THE HASH TABLE.
sym_zero();
}


/**************************************************
*						SYM_ZERO
* FUN:	sym_zero
* SUBJ:	Zero out the hashed sym table.
* CR:		06/14/99 AM.
* NOTE:	To enable deletion and creation of KBs.
*			02/15/01 AM. May want to parameterize this fn.
* OPT:	02/15/01 AM. Should use memset() to zero out each seg.
*
**************************************************/

void
ASYM::sym_zero()
{
long ii;
long jj;
long count;
SYM *sym;


/* Traverse the symbol hash table. */
count = 0;
for (ii = 0; ii < SYM_HASH_TOT; ii++)
   {
   sym = Sym_segs[ii];
   
   for (jj = 0; jj < SYM_SEG_SIZE; jj++)
      {
		// DEPENDS ON SYM STRUCTURE.
		sym->str = 0;
		sym->chain = 0;
		sym->con = 0;
      ++sym;
      if (count++ >= SYM_HASH_SIZE - 1)
         return;
      }
   }
}
