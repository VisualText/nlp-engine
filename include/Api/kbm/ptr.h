/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									PTR.H
*
* FILE:	include/API/kbm/ptr.h
* SUBJ:	Declarations for ptr table manager.
* CR:		9/10/95 AM.
* ASS:	Always have at least one predefined ptr table segment.
*
*******************************************************************************/

#ifndef APTR_H_
#define APTR_H_

#define PNULL ((PTR *) NULL)

#ifdef PTR_OLD
#define PTR_SEG_SIZE (SEG_SIZE / (long) sizeof(PTR))
#endif

/* Maximum number of segments in ptr table. */
/* Want to check total segment sizes in program also.... */
// Upped max by factor of 10.												// 04/20/01 AM.
// 5000	// 07/15/03 AM.
//#define PTR_SEGS_MAX	50000
// SIZES.	// 03/24/13 AM.
//#define PTR_SEGS_MAX	500000

// 05/04/13 AM.
#define PTR_SEGS_MAX	75000

// Forward reference.
class AKBM;
class ACON;

/********************************************
* CLASS:	APTR
* CR:		08/14/02 AM.
* SUBJ:	API Class for managing the KBM POINTER TABLE.
********************************************/

class LIBKBM_API APTR
{
	friend class AKBM;

public:
	APTR(		// Default constructor.
		);

	~APTR();	// Destructor.

	// Access.
//	void *getCon();

	// Update.
//	void setCon(void *);

	// Members.

	PTR *
	ptr_add_e(
		PTR *ptr,				/* Ptr to add to end.	*/
		/*DU*/
		PTR **ptrs,
		PTR **end		/* Ptrs list to modify. */
		);

	PTR *
	ptr_add_end(
		PTR **ptrs		/* Ptrs list to modify. */
		);
	PTR *
	ptr_alloc();
	void
	ptr_clean();
	long
	ptr_curr_off();
	bool																			// 05/05/01 AM.
	ptr_hard_ini(
		long seg_size,
		int segs_tot,
		int seg_curr,
		PTR *seg_p
		);
	void ptr_ini();
	PTR *ptr_make_c(CON *con);
	PTR *ptr_make_p(PTR *pv);
	PTR *ptr_make_sym(SYM *sym);
	PTR *ptr_make_st(_TCHAR *st);
	PTR *ptr_make_n(long num);
	PTR *ptr_make_n(float num);											// 08/15/02 AM.
	PTR *ptr_make_float(float);											// 08/16/01 AM.
	PTR *ptr_make_ch(_TCHAR ch);
	void ptr_off(
		PTR *ptr,
		/*UP*/
		int *seg,
		long *off
		);
	int
	ptr_seg_curr();
	PTR *
	ptr_seg_p();
	long
	ptr_seg_size();
	int
	ptr_segs_tot();
	PTR **
	ptr_seg_table();
	bool
	s_to_pkind(
		_TCHAR *str,
		/*UP*/
		PKIND *pk
		);
	bool s_to_pval(
		_TCHAR *str,
		PKIND pkind,
		/*UP*/
		PVAL *pv
		);

private:
	// Backpointers.
	ACON *acon_;
	ASYM *asym_;

	/*
	The segment table for the ptr table segments (arrays of PTR).
	Each entry points to a pre-built or dynamically allocated array.
	0 -> k-1	- for the k pre-built table segments.
	k -> PTR_SEGS_MAX-1	- for dynamically allocated table segments.
	Ptr_segs_tot counts the number of allocated table segments.
	*/
	/* For freeing up tables, need to distinguish hard-wired from dynamic. 		*/


	/* EFFECTIVE CONSTANTS. */
	/* Set once at sys init using generated ptr table, and never modify. */

	long		 PTR_SEG_SIZE;		/* Size of ptr segment.					*/
	int		 PTR_SEGS_TOT;		/* Number of hard ptr segments.			*/
	int		 PTR_SEG_CURR;		/* Number of last hard ptr seg.			*/
	PTR		*PTR_SEG_P;			/* Last full loc in last hard ptr seg.	*/


	/* GLOBAL VARIABLES FOR DYNAMIC PTR TABLE MGR */
   
	PTR		*Ptr_segs[PTR_SEGS_MAX];	/* Seg table for ptr mgr.		*/
	int		 Ptr_segs_tot;			/* Number of ptr segments.			*/
	int		 Ptr_seg_curr;			/* Current seg in ptr.				*/
	PTR		*Ptr_seg_p;				/* Last full loc in ptr.			*/


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};



// MACROS
// WORKAROUND TO VISUAL C++ PROBLEM Q39910
// PRB: Initializing Non-Primary Union Element Produces Errors
// Need to store floats as longs in the PTR table.					// 08/16/01 AM.
#define float_encode(x) { *((long  *) &(x)) }
#define float_decode(x) { *((float *) &(x)) }

#endif
