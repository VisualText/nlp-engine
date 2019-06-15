/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STRX.H
* FILE:	libqkbm/strx.h
* CR:		02/06/07 AM.
* SUBJ:	Database for strings and info about them.
*
*******************************************************************************/

#ifndef STRX_H_
#define STRX_H_



// Forward reference.
//class XXX;

/********************************************
* CLASS:	STRX
* CR:		02/06/07 AM.
* SUBJ:	API Class for small database with stats and counters.
********************************************/

class LIBQKBM_API STRX
{
//	friend class ACON;

public:
	STRX(		// Default constructor.
		);

	~STRX();	// Destructor.

	// Access. (from memory)

	// Update. (in memory)

	// Class Members.

	// General Functions.
	bool opendb(_TCHAR *dbname);
	bool closedb();

	bool addString(_TCHAR *str, STR_ID val);
	STR_ID getStrx(_TCHAR *str);

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
