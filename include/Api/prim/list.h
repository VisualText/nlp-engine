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
*									LIST.H
*
* FILE:	conch./list.h
* SUBJ:	Declarations for the list subsystem.
* CR:	8/26/95 AM.
*
*******************************************************************************/

#ifndef ALIST_H_
#define ALIST_H_

#define LNULL	((LIST *) NULL)

// Multiplied table size and max segs by 10.							// 04/20/01 AM.
// Multiplied max by another 10.											// 05/05/01 AM.
//#define LIST_SEG_SIZE	((long) 200000)
//#define LIST_SEGS_MAX	10000
// Increasing sizes.	// 03/24/13 AM.
//#define LIST_SEG_SIZE	((long) 1000000)
//#define LIST_SEGS_MAX	50000

// 05/04/13 AM.
#define LIST_SEG_SIZE	((long) 200000)
#define LIST_SEGS_MAX	10000

/********************************************
* CLASS:	ALIST
* CR:		06/11/02 AM.
* SUBJ:	API Class for managing the VisualText KBM Module.
********************************************/

class LIBPRIM_API ALIST
{

public:
	ALIST(		// Default constructor.
		);

	~ALIST();	// Destructor.

	// Access.
//	void *getSt();

	// Update.
//	void setSt(void *);

	// General
	void	list_clean();
	void	list_free(
		LIST *list,
		LIST *end	/* LNULL, or end of given list. */
		);
	void list_free_elt(LIST *list);
	bool list_ini();															// 05/05/01 AM.
	LIST *list_make(
		long val
		);
	LIST *list_add(
		LIST *list,		/* Possibly empty list.				*/
		long val,		/* Val to add to end of list.		*/
		/*DU*/
		LIST **end		/* NULL, or end of given list.		*/
		);
	LIST *list_push(
		long val,
		LIST *list
		);
	void list_add_buf(
		_TCHAR* buf
	);

	// GENERAL FUNCTIONS.
	static LIST *list_add_elt(
		LIST *list,		/* Possibly empty list.				*/
		LIST *elt,		/* Elt to add to end of list.		*/
		/*DU*/
		LIST **end		/* NULL, or end of given list.		*/
		);
	static LIST *list_build(
		LIST *array,
		long num				/* Number of list elements.	*/
		);
	static int list_cmp(
		LIST *lista,
		LIST *listb
		);
	static long list_len(
		LIST *list
		);
	static int list_ncmp(
		LIST *list,
		long num
		);
	static long list_pop(
		LIST **list
		);
	static _TCHAR* list_pop_buf(
		LIST** list,
		_TCHAR* buf
	);
	static _TCHAR* list_str(
		LIST** list,
		_TCHAR* buf
	);
	static LIST *list_pop_elt(
		LIST **list
		);
	static void list_pp_strs(
		LIST *args,
		//FILE *out				// 04/20/99 AM.
		std::_t_ostream *out,		// 04/20/99 AM.
		_TCHAR *buf
		);
	static LIST *list_push_elt(
		LIST *elt,
		LIST *list
		);


private:
	LIST *List_segs[LIST_SEGS_MAX];	// Seg table for list mgr.
	int   List_segs_tot;					// Number of list segments.

	LIST *List_free;						// The free list.
	LIST *List_end;						// End of free list.


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
	_TCHAR* List_buffer;
private:
	static int count_;						// Count objects currently allocated.
#endif
};


#endif
