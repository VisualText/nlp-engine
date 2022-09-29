/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CHECK.H
* FILE:	lite\check.h
* CR:		11/27/98 AM.
* SUBJ:	Check actions for Pat pass.
*
*******************************************************************************/

#ifndef CHECK_H_
#define CHECK_H_
#include <assert.h>


// Forward references
class RFASem;				// 11/22/99 AM.

/********************************************
* CLASS:	CHECK
* CR:		11/27/98 AM.
* SUBJ:	Convenience class for check actions.
********************************************/

class Check
{
	//friend ostream &operator<<(ostream &, Check &);

public:
	Check();										// Default constructor.
	~Check();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static bool checkOr(
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkXlt(	// 08/06/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkXgt(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkXeq(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkXne(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkXge(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkXle(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkNlt(	// 08/07/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkNgt(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkNeq(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkNne(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkNge(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkNle(	// 09/14/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkGlt(	// 09/20/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkGgt(	// 09/20/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkGeq(	// 09/20/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkGne(	// 09/20/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkGge(	// 09/20/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkGle(	// 09/20/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp				// 11/19/99 AM.
		);

	static bool checkDebug(							// 11/22/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Nlppp *nlppp
		);

	static bool checkTrue(RFASem *val);			// 11/22/99 AM.

protected:

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);				// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
