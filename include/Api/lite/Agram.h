/*******************************************************************************
Copyright © 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AGRAM.H
* FILE:	include/API/lite/Agram.h
* CR:		08/26/99 AM.
* SUBJ:	API for managing the GRAM TAB HIERARCHY, part of CG knowledge base.
* NOTE:	Provides convenient API for the VisualText GUI.
*			Knows about rule concepts, samples, locked and dirty concepts,
*			inherited concepts, and so on.
*			Modularity class.  Don't create instances.
*
*******************************************************************************/

#ifndef AGRAM_H_
#define AGRAM_H_
#include <assert.h>
#include "lite.h"
//#include "mode.h"			// 04/04/00 AM.

// Readability definitions.
typedef void APARSE;			// Handle for the current parse.
typedef void ARULES;			// List of rules.
typedef void ASELECTS;		// List of select node names.

// Forward reference.
//class NLP;
class Mode;						// 04/04/00 AM.

/********************************************
* CLASS:	AGRAM
* CR:		08/26/99 AM.
* SUBJ:	API class for managing the Gram hierarchy.
********************************************/

class LITE_API Agram
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Agram();										// Default constructor.
	~Agram();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide

	// Flag a concept (actually a subtree) as dirty for rule generation.
	static bool makeDirty(CONCEPT *conc, CG *cg);

	// If a label concept, just do its parent.
	static bool smartDirty(CONCEPT *conc, CG *cg);

	// Add a sample to a Gram hierarchy concept.
	static CONCEPT *addSample(CONCEPT *con, _TCHAR *name, CG *cg);

	// Remove a sample node.
	static bool rmSample(CONCEPT *node, CG *cg);

	// Get concept's pass number, if any.
	static int smartPassnum(CONCEPT *conc, CG *cg);		// 09/25/99 AM.

	// Rename concept.
	static bool renameConcept(CONCEPT *conc, _TCHAR *name, CG *cg);

	static bool rmAttr(CONCEPT *conc, _TCHAR *name, CG *cg);
	static bool addVal(CONCEPT *conc, _TCHAR *name, _TCHAR *val, CG *cg);
	static bool addSval(CONCEPT *conc, _TCHAR *name, _TCHAR *val, CG *cg);
	static bool replaceVal(CONCEPT *conc, _TCHAR *name, _TCHAR *val, CG *cg);

protected:
};



#endif
/*************************      END OF FILE      ******************************/
