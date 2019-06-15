/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	UTIL.H
* FILE:	include/API/qkbm/Aqkbm.h
* CR:		02/05/07 AM.
* SUBJ:	A small database for stats and counters.
*
*******************************************************************************/

#ifndef UTIL_H_
#define UTIL_H_


// Forward reference.
//class XXX;

/********************************************
* CLASS:	UTIL
* CR:		02/05/07 AM.
* SUBJ:	API Class for small database with stats and counters.
********************************************/

class LIBQKBM_API UTIL
{
//	friend class ACON;

public:
	UTIL(		// Default constructor.
		);

	~UTIL();	// Destructor.

	// Access.

	// Update.

	// Class Members.
	static int selectInt(DEPOT *db, _TCHAR *field);
	static bool updateInt(DEPOT *db, _TCHAR *field, int val);

	// General Functions.

private:

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
