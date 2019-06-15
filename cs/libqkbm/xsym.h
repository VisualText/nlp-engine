/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XSYM.H
* FILE:	libqkbm/xsym.h
* CR:		02/18/07 AM.
* SUBJ:	Database for mapping index to its string.
*
*******************************************************************************/

#ifndef XSYM_H_
#define XSYM_H_



// Forward reference.
//class XXX;

/********************************************
* CLASS:	XSYM
* CR:		02/18/07 AM.
* SUBJ:	API Class for symbol table.
********************************************/

class LIBQKBM_API XSYM
{
//	friend class ACON;

public:
	XSYM(		// Default constructor.
		);

	~XSYM();	// Destructor.

	// Access. (from memory)

	// Update. (in memory)

	// Class Members.

	// General Functions.
	bool opendb(_TCHAR *dbname);
	bool closedb();

	bool addXsym(STR_ID val, CON_ID cid);
	CON_ID getXsym(STR_ID index);
	bool rmXsym(char *idx_s);

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
