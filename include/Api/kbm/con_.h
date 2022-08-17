/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									CON_.H
*
* FILE:	include/API/kbm/con_.h
* SUBJ:	Declarations for concept table manager.
* CR:		9/10/95 AM.
* ASS:	Always have at least one predefined con table segment.
* NOTE:	MS Windows objects to a file named "con", so renamed to "con_".
*
*******************************************************************************/

#ifndef ACON_H_
#define ACON_H_

/* 10/19/95 AM. */
/* Maximum number of phrase elements allowed. */
// 08/09/99 AM. 25 NOT BIG ENOUGH!!
#define MAX_PHRASE_ELTS 10000

#define CNULL ((CON *) NULL)

#ifdef CON_OLD
#define CON_SEG_SIZE (SEG_SIZE / (long) sizeof(CON))
#endif

/* Maximum number of segments in con table. */
/* Want to check total segment sizes in program also.... */
// Upped max by factor of 10.												// 04/20/01 AM.
// 5000	// 07/15/03 AM.
//#define CON_SEGS_MAX	50000
// SIZES.	// 03/24/13 AM.
//#define CON_SEGS_MAX	500000

// 05/04/13 AM.
#define CON_SEGS_MAX	75000

// Forward reference.
class AKBM;
class ASYM;


/********************************************
* CLASS:	ACON
* CR:		08/13/02 AM.
* SUBJ:	API Class for managing the KBM CONCEPT TABLE.
********************************************/

class LIBKBM_API ACON
{
	friend class AKBM;

public:
	ACON(		// Default constructor.
		);

	~ACON();	// Destructor.

	// Access.
//	void *getCon();

	// Update.
//	void setCon(void *);

	// Members
	CON *Con(long id);
	CON *con_alloc();
	void con_clean();
	bool con_hard_ini(														// 05/05/01 AM.
		long seg_size,
		int segs_tot,
		int seg_curr,
		CON *seg_p,
		ID count
		);
	void con_off(
		CON *con,
		/*UP*/
		int *seg,
		long *off
		);
	ID con_off_to_id(
		int seg,
		long off
		);
	int con_seg_curr();
	CON *con_seg_p();
	long con_seg_size();
	int con_segs_tot();
	CON **con_seg_table();
	void con_tab_pretty(
		//FILE *out			// 04/20/99 AM.
		std::_t_ostream *out		// 04/20/99 AM.
		);
	ID con_count();
	long con_curr_off();

	CON *con_add_alpha(
		CON *hier,
		_TCHAR *name
		);
	CON *con_add_basic(
		_TCHAR *name,				/* Descriptive name for new concept.	*/
		CON *up					/* Hierarchical parent of new concept.	*/
		);
	bool con_add_first(CON *con, CON *hier);		// 05/08/99 AM.
	CON *con_add_nh(
		_TCHAR *name				/* Descriptive name for new concept.	*/
		);
	CON *con_add_nth(
		CON *hier,
		_TCHAR *name,
		int pos			// position to insert into.  if = 0, at end of list.
		);
	CON *con_add_nth(			// 05/13/99 AM.
		CON *hier,
		CON *con,
		int pos			// position to insert into.  if = 0, at end of list.
		);
	CON *con_add_proxy(
		CON *gen				/* The concept that proxy represents. */
		);
	CON *con_add_word_first(
		_TCHAR *name,				/* Descriptive name for new concept.	*/
		CON *hier				/* Hierarchical parent of new concept.	*/
		);
	CON *con_child_alpha(
		CON *hier,
		_TCHAR *name,
		/*UP*/
		CON **pre
		);
	ID id_get_child(ID,_TCHAR*);
	CON *path_to_con(_TCHAR*);
	CON *get_component(CON*,_TCHAR*);

	bool sysAttr(CON*);
	CON *c_cg_concept();
	CON *con_phrase(CON*);
	CON *phrase_con(CON*);
	CON *con_nth_proxy(CON*,long);
	CON *node_owner(CON*);                               // 10/09/05 AM.


	// General Functions

	static CON *con_get_child(CON *hier,_TCHAR *name);
	static _TCHAR *con_kind_str(CON *con);
	static _TCHAR *con_str(CON *con);
	static _TCHAR *con_str_pret(CON*);
	static CON *nc(CON *proxy);
	static long phrase_len(CON*);
	static CON *con_nth(CON*,long);
	static long con_ord(CON*);                                  // 10/09/05 AM.
	static CON *con_last(CON*);
	static _TCHAR *con_to_path(CON*,/*UP*/_TCHAR*);
	static void con_to_path_r(CON*,/*UP*/_TCHAR*&);
	static bool con_remove(CON*);
	static CON *con_parent(CON*);

	// Public Data.
	// MOVING THESE VARIABLES HERE FROM kbm_def.h					// 08/15/02 AM.
	CON																			// 08/15/02 AM.
		*c_cg_CONCEPT,
		*c_cg_PHRASE,				// 04/16/99 AM.
		*c_cg_ATOM,					// 05/04/99 AM.
		*c_dict_ALPHA,
		*c_dict_UNKNOWN,
		*c_nlp_PUNCT,
		*c_nlp_WHT;


private:
	// Backpointers.
	AKBM *akbm_;
	ASYM *asym_;

	/*
	The segment table for the concept table segments (arrays of CON).
	Each entry points to a pre-built or dynamically allocated array.
	0 -> k-1	- for the k pre-built con table segments.
	k -> CON_SEGS_MAX-1	- for dynamically allocated con table segments.
	Con_segs_tot counts the number of allocated con table segments.
	*/
	// For freeing up tables, need to distinguish hard-wired from dynamic.


	// EFFECTIVE CONSTANTS.
	// Set once at sys init using generated con table, and never modify.

	long		 CON_SEG_SIZE;		// Size of con segment.
	int		 CON_SEGS_TOT;		// Number of hard con segments.
	int		 CON_SEG_CURR;		// Number of last hard con seg.
	CON		*CON_SEG_P;			// Last full loc in last hard con seg.

	ID		 CON_COUNT;			// Total concept count.

	// GLOBAL VARIABLES FOR DYNAMIC CON TABLE MGR
   
	CON		*Con_segs[CON_SEGS_MAX];	// Seg table for con mgr.
	int		 Con_segs_tot;			// Number of con segments.
	int		 Con_seg_curr;			// Current seg in con.
	CON		*Con_seg_p;				// Last full loc in con.

	ID		 Con_count;				// Total concept count.


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};

/********************************************
* FN:    SYSATTR
* CR:    08/15/02 AM.
* SUBJ:  If given concept is internal system attribute.
********************************************/

inline bool ACON::sysAttr(CON *con)
{
return (con == c_cg_PHRASE || con == c_cg_ATOM);
}

//LIBKBM_API void
//phrase_pret(
//	CON *phr,
//	//FILE *out		// 04/20/99 AM.
//	ostream *out	// 04/20/99 AM.
//	);

//LIBKBM_API bool
//con_bind_root();

#endif
/*************************      END OF FILE      ******************************/
