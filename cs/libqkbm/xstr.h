/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XSTR.H
* FILE:	libqkbm/xstr.h
* CR:		02/07/07 AM.
* SUBJ:	Database for mapping index to its string.
*
*******************************************************************************/

#ifndef XSTR_H_
#define XSTR_H_



// Forward reference.
//class XXX;

/********************************************
* CLASS:	XSTR
* CR:		02/07/07 AM.
* SUBJ:	API Class for small database with stats and counters.
********************************************/

class LIBQKBM_API XSTR
{
//	friend class ACON;

public:
	XSTR(		// Default constructor.
		);

	~XSTR();	// Destructor.

	// Access. (from memory)

	// Update. (in memory)

	// Class Members.

	// General Functions.
	bool opendb(_TCHAR *dbname);
	bool closedb();

	bool addXstr(STR_ID val, _TCHAR *str);
	_TCHAR *getXstr(STR_ID index);

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
