/*******************************************************************************
Copyright (c) 2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MGR.H
* FILE:	libqkbm/mgr.h
* CR:		02/05/07 AM.
* SUBJ:	A small database for stats and counters.
*
*******************************************************************************/

#ifndef MGR_H_
#define MGR_H_


// Forward reference.
//class XXX;

/********************************************
* CLASS:	MGR
* CR:		02/05/07 AM.
* SUBJ:	API Class for small database with stats and counters.
********************************************/

class LIBQKBM_API MGR
{
//	friend class ACON;

public:
	MGR(		// Default constructor.
		);

	~MGR();	// Destructor.

	// Access. (from memory)
	STR_ID getStrings();
	CON_ID getConcepts();
	PTR_ID getPointers();

	// Update. (in memory)
	void setStrings(STR_ID);
	void setConcepts(CON_ID);
	void setPointers(PTR_ID);

	// Class Members.

	// General Functions.
	bool isdb(_TCHAR *dbname);	// 03/04/07 AM.
	bool opendb(_TCHAR *dbname);
	bool closedb();
	
	bool initdb();		// Initialize existing db after loading.	// 03/04/07 AM.
	bool resetdb();	// Zero out manager's counters. For a newly created or emptied db.

	// Fetch counters from database.
	STR_ID selectStrings();
	CON_ID selectConcepts();
	PTR_ID selectPointers();

	// Update counters to database.
	STR_ID updateStrings();
	CON_ID updateConcepts();
	PTR_ID updatePointers();

	// Increment counters in memory only (not in database).
	STR_ID incStrings();
	CON_ID incConcepts();
	PTR_ID incPointers();

private:
	DEPOT *db_;
	
	STR_ID strings_;	// Count strings in databases.
	CON_ID concepts_;	// Count concepts in databases.
	PTR_ID pointers_;	// Count pointers in databases.

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
