/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									ST.H
*
* FILE:	include/API/kbm/st.h
* SUBJ:	Declarations for string table manager.
* CR:		5/10/95 AM.
*
*******************************************************************************/

#ifndef AST_H_
#define AST_H_

/* Maximum number of segments in string table. */
// 500 // 07/15/03 AM.
//#define ST_SEGS_MAX	5000
// SIZES.	// 03/24/13 AM.
//#define ST_SEGS_MAX	50000

// 05/04/13 AM.
#define ST_SEGS_MAX	7500

/********************************************
* CLASS:	AST
* CR:		08/14/02 AM.
* SUBJ:	API Class for managing the KBM STRING TABLE.
********************************************/

class LIBKBM_API AST
{

public:
	AST(		// Default constructor.
		);

	~AST();	// Destructor.

	// Access.
//	void *getCon();

	// Update.
//	void setCon(void *);

	// Members.
	bool
	st_add(
		_TCHAR *str,			/* Given string. */
		/*UP*/
		_TCHAR **ptr			/* Position in str table.	*/
		);
	void		 st_alloc();
	long		 st_avail();
	void		 st_clean();
	long		 st_curr_off();
	/* char		*st_find(long off); */
	bool														// 05/05/01 AM.
	st_hard_ini(
		long seg_size,
		int segs_tot,
		int seg_curr,
		_TCHAR *seg_p
		);
	void st_off(
		_TCHAR *str,
		/*UP*/
		int *seg,
		long *off
		);
	bool		 st_pp(
		//FILE *fp			// 04/20/99 AM.
		std::_t_ostream *fp			// 04/20/99 AM.
		);
	void		 st_pp_all(
		//FILE *fp			// 04/20/99 AM.
		std::_t_ostream *fp			// 04/20/99 AM.
		);
	int		 st_seg_curr();
	_TCHAR		*st_seg_p();
	long		 st_seg_size();
	_TCHAR	  **st_seg_table();
	int		 st_segs_tot();

private:

	/*
	The segment table for the string table segments (arrays of char).
	Each entry points to a pre-built or dynamically allocated array.
	0 -> k	- for the (k + 1) pre-built string table segments.
	k+1 -> ST_SEGS_MAX-1	- for dynamically allocated string table segments.
	St_segs_tot counts the number of allocated string table segments.
	*/
	/* 8/27/95 AM. For freeing up tables, need to distinguish hard-wired
		from dynamic. */

	/* 9/30/95 AM. Overhaul of st mgr. */
	/* Keeping both St_segs_tot and St_seg_curr because may want to preallocate
	empty segments (so these could differ by more than 1). */

	/* EFFECTIVE CONSTANTS. */
	/* Set once at sys init using generated st, and never modify. */

	long		 ST_SEG_SIZE;		/* Size of st segment.					*/
	int		 ST_SEGS_TOT;		/* Number of hard st segments.			*/
	int		 ST_SEG_CURR;		/* Number of last hard st seg.			*/
	_TCHAR		*ST_SEG_P;			/* Last full loc in last hard st seg.	*/


	/* GLOBAL VARIABLES FOR DYNAMIC ST MGR */
   
	_TCHAR		*St_segs[ST_SEGS_MAX];	/* Seg table for st mgr.			*/
	int		 St_segs_tot;			/* Number of st segments.			*/
	int		 St_seg_curr;			/* Current seg in st.				*/
	_TCHAR		*St_seg_p;				/* Last full loc in st.				*/


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};





#ifdef ST_OLD
void		 st_ini();

_TCHAR		*St_segs[];
int		 St_segs_tot;

_TCHAR		*St_ptr;
long		 St_tot;
long		 St_fill;
long		 Hard_st_tot;
long		 Hard_st_fill;
#endif

#endif

