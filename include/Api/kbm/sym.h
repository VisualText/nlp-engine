/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									SYM.H
*
* FILE:	include/API/kbm/sym.h
* SUBJ:	Declarations for symbol table manager.
* NOTE:	Must automatically have one predefined sym tab segment.
* CR:		5/2/95 AM.
*
*******************************************************************************/

#ifndef ASYM_H_
#define ASYM_H_

#define SNULL ((SYM *) NULL)

/* Programmer-defined size of hashable portion of sym table. */
/* Note that the conflict syms immediately follow the hashed part
of the table.  May want a sentinel to immediately follow the last
hash table locus, for convenience. */

#ifdef SYM_OLD
/* 9/2/95 TESTING WITH SMALL VALUE */
/* Size is in terms of sym elements, NOT bytes. */
#define SYMHASH_SIZE	((long) 3)
/* define SYMHASH_SIZE ((long) 4001) */

#define SYM_SEG_SIZE (SEG_SIZE / (long) sizeof(SYM))
#endif

/* Maximum number of segments in sym table. */
/* Want to check total segment sizes in program also.... */
// Upped max by factor of 10.												// 04/20/01 AM.
// 5000	// 07/15/03 AM.
//#define SYM_SEGS_MAX	50000
// SIZES.	// 03/24/13 AM.
//#define SYM_SEGS_MAX	500000
// 05/04/13 AM.
#define SYM_SEGS_MAX	75000

// Maximum length of chars to hash with.								// 06/29/03 AM.
// Ignore string beyond this length.									// 06/29/03 AM.
#define HASH_MAX 25

// Forward reference.
class AKBM;
class AST;

/********************************************
* CLASS:	ASYM
* CR:		08/14/02 AM.
* SUBJ:	API Class for managing the KBM SYMBOL TABLE.
********************************************/

class LIBKBM_API ASYM
{
	friend class AKBM;

public:
	ASYM(		// Default constructor.
		);

	~ASYM();	// Destructor.

	// Access.
//	void *getCon();

	// Update.
//	void setCon(void *);

	// Members.
	SYM		*sym_add(_TCHAR *str);
	SYM		*sym_alloc();
	void		 sym_clean();
	SYM		*sym_find(_TCHAR *str);
	SYM		*sym_get(_TCHAR *str,
							bool &);						// 06/29/03 AM.
	bool													// 05/05/01 AM.
	sym_hard_ini(
		long seg_size,
		long hash_size,
		int hash_tot,
		int segs_hard_tot,														// 02/16/01 AM.
		int segs_tot,
		int seg_curr,
		SYM *seg_p
		);
	long	 sym_hash(_TCHAR *str);
	long	 sym_hash_size();
	int		 sym_hash_tot();
// void	 sym_ini();
	void sym_off(
		SYM *sym,
		/*UP*/
		int *seg,
		long *off
		);
	void	 sym_pp(
		// FILE *out
		std::_t_ostream *out				// 04/20/99 AM.
		);
	SYM		**sym_seg_table();
	void	 sym_stat(
		//FILE *out
		std::_t_ostream *out				// 04/20/99 AM.
		);


	long		 sym_curr_off();
	int		 sym_seg_curr();
	SYM		*sym_seg_p();
	long		 sym_seg_size();
	int		 sym_segs_tot();
	void sym_reset();
	void sym_zero();

	bool sym_seg_zero(SYM *seg);			// 02/16/01 AM.

	// General functions.
	static _TCHAR	*sym_str(SYM *sym);


private:
	// Backpointers.
	AST *ast_;


	/**** EFFECTIVE CONSTANTS ****/
	/* Set once and then frozen.  Handshake from generated sym+conf table. */

	long		 SYM_SEG_SIZE;		/* Size of sym table seg (in SYMs).	*/
	long		 SYM_HASH_SIZE;		/* Size of hash table (in SYMs).	*/
	int		 SYM_HASH_TOT;		/* # of segs used by hash table.	*/
	int		 SYM_SEGS_TOT;		/* # of gend sym+conf segments.		*/

	int		 CONF_SEG_CURR;		/* Last seg of gend conf table.		*/
	SYM		*CONF_SEG_P;		/* Last full loc of gend conf table	*/

	/*** DYNAMIC GLOBAL VARIABLES ****/

	/*
	The segment table for the sym table segments (arrays of SYM).
	Each entry points to a pre-built or dynamically allocated array.
	0 -> k	- for the (k + 1) pre-built sym table segments.
	k+1 -> SYM_SEGS_MAX-1	- for dynamically allocated sym table segments.
	Sym_segs_tot counts the number of allocated sym table segments.
	*/
	/* 8/27/95 AM. For freeing up tables, need to distinguish hard-wired
		from dynamic. */
	/* Note sym table is always prebuilt.  Conflict table allocd as needed.
	Conflict table starts where sym table ends. */
	SYM		*Sym_segs[SYM_SEGS_MAX];
	int		 Sym_segs_tot;		// DYNAMIC COUNT OF SYM TABLE SEGS.

	int		 Conf_seg_curr;		/* Last seg of conf table.			*/
	SYM		*Conf_seg_p;		/* Last full loc of conf table.		*/


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};



#endif
