/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AVAR.H
* FILE:	include/API/lite/Avar.h
* CR:		12/09/98 AM.
* SUBJ:	API for accessing user-created variables.
*
*******************************************************************************/

#ifndef AVAR_H_
#define AVAR_H_
#include <assert.h>
#include "lite.h"


/********************************************
* CLASS:	AVAR
* CR:		12/09/98 AM.
* SUBJ:	API Class for user-defined code variables.
********************************************/

class Avar
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Avar();									// Default constructor.
	~Avar();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	LITE_API static void *getVars(void *hparse);
	LITE_API static void *getVar(_TCHAR *name, void *hvars);
	LITE_API static bool getLong(void *hvar, /*DU*/ long long &val);
	LITE_API static bool getLong(_TCHAR *name, void *hvars, /*DU*/ long long &val);

	LITE_API static void *getFirst(void *hvars);
	LITE_API static void *getNext(void *pvar);
	LITE_API static void *getVar(void *pvar);
	LITE_API static _TCHAR *getName(void *hvar);

	// Things that may not belong here.
	LITE_API static _TCHAR *Inputfile(void *hparse);


protected:
};



#endif
/*************************      END OF FILE      ******************************/
