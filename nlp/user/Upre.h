/*******************************************************************************
Copyright (c) 1998,1999 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	UPRE.H
* FILE:	c:\dev\nlplite\upre.h
* CR:		12/04/98 AM.
* SUBJ:	User-defined pre actions for the Pat pass.
* NOTE:	This class is a convenience for registering user-defined pre
*			actions (actually, preconditions) on rule matching. The Upre
*			class has to be registered with Lite library.
*			1/29/02 AM. UNUSED.
*
*******************************************************************************/

#ifndef UPRE_H_
#define UPRE_H_
#include "lite/dlist.h"
#include "lite/Auser.h"
#include "lite/pre.h"


// Forward references
class Pn;

/********************************************
* CLASS:	UPRE
* CR:		11/30/98 AM.
* SUBJ:	Convenience class for pre actions.
********************************************/

class Upre : public Pre
{
	//friend ostream &operator<<(ostream &, Upre &);

public:
	Upre();										// Default constructor.
	~Upre();

	// Access

	// Modify

	// General.
	virtual bool execute(
		_TCHAR *func,
		DELTS *args,
		Pn *pn);

	// Class-wide

	static bool upreLowercase(
		DELTS *args,			// Action's arguments.
		Pn *pn						// Current parse tree node.
		);
	static bool upreHello(
		DELTS *args,
		Pn *pn
		);
};



#endif
/*************************      END OF FILE      ******************************/
