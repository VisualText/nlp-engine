/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NUG.H
* FILE:	lite/nug.h
* CR:		12/27/99 AM.
* SUBJ:	NLPPP Generation utilities.
*
*******************************************************************************/

#ifndef NUG_H_
#define NUG_H_
//#include <iostream.h>
#include <assert.h>
#include "lite/lite.h"

// Readability definitions.
typedef void NUG;

// Forward reference.

/********************************************
* CLASS:	NUG
* CR:		12/27/99 AM.
* SUBJ:	Modularity class for NLP++ generation.
********************************************/

class LITE_API Nug
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Nug();									// Default constructor.
	~Nug();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide
	static NUG *makeVar(													// 12/27/99 AM.
		_TCHAR *s_typ,			// "G", "S", "N", "X".
		_TCHAR *name,			// Variable name.  eg, "$text".
		int ord
		);
	static NUG *makeAssign(												// 12/27/99 AM.
		NUG *lhs,
		NUG *rhs
		);
	static NUG *makeExprstmt(											// 12/27/99 AM.
		NUG *expr
		);
	static NUG *makeInc(													// 11/27/00 AM.
		NUG *var
		);

protected:
};



#endif
/*************************      END OF FILE      ******************************/
