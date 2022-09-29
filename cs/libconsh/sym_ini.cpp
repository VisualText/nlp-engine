/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/* This will be automatically generated.  But setting it up by
hand for now.  9/2/95 AM */
/* Consh will generate these automatically for Conan.  But for
Consh itself, hand-editing will be ok for a very long time.
Mainly, this will say how much predefined empty space we're
providing for Consh.  Auto knowledge load will happen every
time Consh is invoked. */
/* Need to know the exact initial configuration of tables. */ 
/*** AUTOMATICALLY GENERATED! EDITS WILL BE LOST. ***/
#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"			// 04/21/99 AM.
#include "prim/sm.h"				// 04/27/01 AM.


#include "kbm/libkbm.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/kbm_alloc.h"		// 02/15/01 AM.

#include "consh/libconsh.h"
#include "consh/cg.h"

#include "cg_global.h"			// 02/15/01 AM.
#include "sym_ini.h"

/********************************************
* FN:		CC_SYM_INI
* CR:		1995 AM.
* SUBJ:	Initialize the hashed symbol table.
* NOTE:	02/15/01 AM. libconsh had hardwired arrays in Sym0.cpp and Sym1.cpp.
*			Getting rid of these hard-wired sym table segments.
*			Based on the total size specified, the appropriate number of sym
*			table segments will be allocated and initialized dynamically.
********************************************/

// Distinguish library from auto-generated kb load.	// 03/23/19 AM.
#ifdef LINUX
bool ccx_sym_ini(void *xcg)				// 03/23/19 AM.
#else
bool cc_sym_ini(void *xcg)				// 06/11/02 AM.
#endif
{
CG *cg = (CG *) xcg;
AKBM *kbm = cg->kbm_;
// Global seg table for sym mgr.
SYM **segs = cg->asym_->sym_seg_table();		// Get the sym segment table from sym mgr.

// Size of a sym seg, in terms of entries.							// 02/15/01 AM.				
long sym_seg_size = (long) (SEG_SIZE / (long) sizeof(SYM));		// 02/15/01 AM.

// Total segments hard-wired into the code.							// 02/15/01 AM.
int tot_hard_segs = 0;														// 02/15/01 AM.

// Fill the hard-coded sym segments.
// Note: In a compiled kb, some of these may be prebuilt.
//segs[0] = &(Sym0[0]);
//segs[1] = &(Sym1[0]);

// Total size (in elements) of the hash table.
long tot_hash_size =															// 02/16/01 AM.
//					(long) 2663;												// 02/15/01 AM.
//					(long) 5323;	// 2 segs.								// 02/16/01 AM.
//					(long) 24989;	// scale up.							// 02/16/01 AM.
//					(long) 104729;	// scale up.							// 04/21/01 AM.
					(long) 250007;	// scale up.							// 06/29/03 AM.
if (tot_hash_size < 100 || tot_hash_size > (long)10000000)		// 02/16/01 AM.
	{
	*cgerr << _T("[Error: Sym table size out of bounds.]") << std::endl;	// 02/16/01 AM.
	// Need way to flag error.												// 02/16/01 AM.
	return false;																// 05/05/01 AM.
	}

// Calculate total segs for the hash table.							// 02/15/01 AM.
int tot_hash_segs = 1 + (int)(tot_hash_size / sym_seg_size);	// 02/15/01 AM.

int tot_conf_segs = 2;		// Initial segs for conflicts.		// 02/15/01 AM.

// If sym table larger than allowed by seg table, fail.			// 02/15/01 AM.
if ((tot_hash_segs + tot_conf_segs) > SYM_SEGS_MAX)				// 02/15/01 AM.
	{
	*cgerr << _T("[Sym table too large to initialize.]") << std::endl;	// 02/15/01 AM.
	// Need a way to crash and burn, or return failure.			// 02/15/01 AM.
	return false;																// 05/05/01 AM.
	}

// PRE-ALLOCATE SEGMENTS FOR HASH TABLE.								// 02/15/01 AM.
int tot_alloc = tot_hash_segs + tot_conf_segs - tot_hard_segs;	// 02/15/01 AM.
int ii;																			// 02/15/01 AM.
for (ii = 0; ii < tot_alloc; ++ii)										// 02/15/01 AM.
	{
	segs[ii] = (SYM *) kbm_alloc(sym_seg_size, sizeof(SYM));		// 02/15/01 AM.
//	sym_seg_zero(segs[ii]);										// BUG.	// 04/27/01 AM.
	sm_zero((_TCHAR*)segs[ii],sizeof(SYM)*sym_seg_size);	// FIX	// 04/27/01 AM.
	}

// Get offset of final element of hash table.						// 02/15/01 AM.
// (Note: start conf set there, to use all available seg space.)
SYM *last_loc = 0;															// 02/16/01 AM.
long off = tot_hash_size % sym_seg_size;								// 02/15/01 AM.
if (off < (sym_seg_size - 1))												// 02/16/01 AM.
	last_loc = (segs[tot_hash_segs-1] + off);							// 02/15/01 AM.
else
	{
	// If hash table exactly consumes its highest segment,		// 02/16/01 AM.
	// conflict set is at start of next segment.						// 02/16/01 AM.
	last_loc = segs[tot_hash_segs];										// 02/16/01 AM.
	}

// Provide hard-coded values to the sym mgr.
bool ok = cg->asym_->sym_hard_ini(										// 05/05/01 AM.
   sym_seg_size,				// Size of a sym seg.
   (long) tot_hash_size,	// Size of hash table.
   tot_hash_segs,				// # of segs in hash table.
   tot_hard_segs,				// # of hard segs in sym+conf
	tot_hash_segs + tot_conf_segs,	// Total init segs.			// 02/16/01 AM.
   (int) 0,						// Current hard seg of conf table.
   last_loc						// Last full loc of conf table.
   );
return ok;																		// 05/05/01 AM.
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// 02/15/01 AM.
#ifdef OLD_

/* 10/1/95 AM. Redoing the handshake between the sym management
library and the automatically generated code.
The library should know NOTHING direct about the generated code.
But the generated code has access to the library.
cc_sym_ini() will be called directly by the application (eg Consh),
to initialize st management.
 */

void cc_sym_ini()
{
SYM **segs;			/* Global seg table for sym mgr.		*/
long dummy;

segs = sym_seg_table();	/* Get the table from sym mgr.		*/

/* Fill the hard-coded sym segments.			*/
segs[0] = &(Sym0[0]);
segs[1] = &(Sym1[0]);

/* Provide hard-coded values to the sym mgr.	*/
//dummy = (long) 1081; /* TESTING! */									// 02/15/01 AM.
// Other primes: 10007  20011.
// NOTE: CAN'T INCREASE SIZE ARBITRARILY, TILL INIT CODE UPGRADED
// TO CREATE THE RIGHT NUMBER OF SEGMENTS.							// 02/15/01 AM.
// Seg size is 2666 right now, so 2663 should be ok.				// 02/15/01 AM.
dummy = (long) 2663;		// Trying something bigger.				// 02/15/01 AM.
sym_hard_ini(

   (long) (SEG_SIZE / (long) sizeof(SYM)),	/* Size of a sym seg.	*/
   (long) dummy,				/* Size of hash table.			*/
   (int) 1,						/* # of segs in hash table.		*/
   (int) 2,						/* # of hard segs in sym+conf	*/
	(int) 2,		// Total # of allocd segs in sym+conf				// 02/16/01 AM.
   (int) 0,						/* Current hard seg of conf table.	*/
   (SYM *) &(Sym0[dummy-1])		/* Last full loc of conf table.	*/
   );
}

#endif
// 02/15/01 AM.
