/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XCON.H
* FILE:	libqkbm/xcon.h
* CR:		02/07/07 AM.
* SUBJ:	Database for concept data structure.
* NOTE:	index -> (kind value flags attrs up dn prev next atom phrases)
*			Mirrors the Conceptual Grammar concept data structure, but in QDBM.
*			Will use index numbers to traverse these, keeping the
*			old structure.  One change is that atom and phrases will have their own field
*			in the concept data structure.
*			Note that we're using unique identifiers rather than pointers, in this
*			implementation.
*
*******************************************************************************/

#ifndef XCON_H_
#define XCON_H_



// Forward reference.
//class XXX;

/********************************************
* CLASS:	XCON
* CR:		02/07/07 AM.
* SUBJ:	API Class for small database with stats and counters.
********************************************/

class LIBQKBM_API XCON
{
//	friend class ACON;

public:
	XCON(		// Default constructor.
		);

	~XCON();	// Destructor.

	// Access. (from memory)

	// Update. (in memory)

	// Class Members.

	// General Functions.
	bool opendb(_TCHAR *dbname);
	bool closedb();

	CON_ID addXcon(
		char *idx,
		CON_ID id,	// Concept's id. // 02/14/07 AM.
		int val,
		XCKIND kind,
		int flags,
		PTR_ID attrs,
		CON_ID up,
		CON_ID dn,
		CON_ID prev,
		CON_ID next,
		CON_ID atom,
		PTR_ID phrases
		);
	bool updateXcon(char *id_s, XCON_S *con);

	bool getXcon(char *idx_s, /*DU*/ XCON_S *&con);
	bool getXcon(CON_ID idx, /*DU*/ XCON_S *&con);

	PTR_ID getXcon_phrases(char *idx_s);
	STR_ID get_name(char *idx_s);
	CON_ID getXcon_atom(char *idx_s);
	CON_ID get_dn(char *idx_s);
	CON_ID get_up(char *idx_s);
	CON_ID get_prev(char *idx_s);
	CON_ID get_next(char *idx_s);
	PTR_ID get_attrs(char *idx_s);
	bool rmXcon(char *idx_s);

	bool set_name(CON_ID conid, STR_ID strid, /*DU*/ bool &dirty);

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
