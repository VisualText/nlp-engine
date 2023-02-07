/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PRE.H
* FILE:	include/API/lite/pre.h
* CR:		11/27/98 AM.
* SUBJ:	Pre actions for Pat pass.
*
*******************************************************************************/

#ifndef PRE_H_
#define PRE_H_
#include <assert.h>
#include "lite.h"
#include "dlist.h"			// 12/04/98 AM.


// Forward references
class Pn;			// 12/04/98 AM.
class Iarg;			// 12/11/98 AM.

/********************************************
* CLASS:	PRE
* CR:		11/30/98 AM.
* SUBJ:	Convenience class for pre actions.
********************************************/

class LITE_API Pre
{
	//friend ostream &operator<<(ostream &, Pre &);

public:
	Pre();										// Default constructor.
	virtual ~Pre();									// Destructor.

	// Access

	// Modify

	// General.
	virtual bool execute(	// Let pre subclass handle action.
		_TCHAR *func,
		Delt<Iarg> *args,
		Pn *pn) = 0;

	// Class-wide
	static bool userPre(_TCHAR *func, Delt<Iarg> *args, Pn *pn);
	static bool regPre(Pre *);
	static void clean();

	static bool preUppercase(
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preLowercase(
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preCap(	// 01/26/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preLength(	// 01/31/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preLengthr(	// 03/22/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preNumrange(	// 08/10/99 AM.
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preUnknown(
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool preDebug(
		Delt<Iarg> *args,			// Action's arguments.
		Pn *pn						// Node to match.
		);
	static bool preVar(   // 06/14/05 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preVarz(   // 06/16/05 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preVareq(   // 06/16/05 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preVargt(   // 06/16/05 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preVarlt(   // 06/16/05 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preVarne(   // 06/16/05 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preRegexp(   // 03/23/09 AM.
		Delt<Iarg> *args,
		Pn *pn
		);
	static bool preRegexpi(   // 03/26/09 AM.
		Delt<Iarg> *args,
		Pn *pn
		);

protected:

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
