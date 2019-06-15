/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XPTR.H
* FILE:	libqkbm/xptr.h
* CR:		02/07/07 AM.
* SUBJ:	Database for pointer data structure.
* NOTE:	index -> (kind value next)
*			Mirrors the Conceptual Grammar pointer data structure, but in QDBM.
*			Will use index and next-index as ways to traverse these, keeping the
*			old structure.  One change is that phrases will have their own field
*			in the concept data structure.
*			Note that we're using unique identifiers rather than pointers, in this
*			implementation.
*
*******************************************************************************/

#ifndef XPTR_H_
#define XPTR_H_



// Forward reference.
//class XXX;

/********************************************
* CLASS:	XPTR
* CR:		02/07/07 AM.
* SUBJ:	API Class for small database with stats and counters.
********************************************/

class LIBQKBM_API XPTR
{
//	friend class ACON;

public:
	XPTR(		// Default constructor.
		);

	~XPTR();	// Destructor.

	// Access. (from memory)

	// Update. (in memory)

	// Class Members.

	// General Functions.
	bool opendb(_TCHAR *dbname);
	bool closedb();

	PTR_ID addXptr(char *idx, int val, XPKIND kind, PTR_ID next);
	PTR_ID addXptr(char *idx, float val, PTR_ID next);

	bool updateXptr(char *id_s, XPTR_S *ptr);

	bool getXptr(char *idx_s, /*DU*/ XPTR_S *&ptr);
	bool rmXptr(char *idx_s);
	bool rmXptr(PTR_ID pid);
	CON_ID get_ptr_con(char *idx_s);
	PTR_ID get_ptr_ptr(char *idx_s);
	PTR_ID get_ptr_next(char *idx_s);
	STR_ID get_ptr_str(char *idx_s);
	bool get_ptr_num(char *idx_s, long &num);
	bool get_ptr_flt(char *idx_s, float &flt);

private:
	CURIA *db_;

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
/*************************      END OF FILE      ******************************/
