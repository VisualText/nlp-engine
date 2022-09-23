/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ARUG.H
* FILE:	lite/Arug.h
* CR:		05/20/99 AM.
* SUBJ:	API for rule generation.
* NOTE:	For use by LIBRUG.DLL.
*			Knows about parse instance, parse tree, and rule internals.
*
*******************************************************************************/

#ifndef ARUG_H_
#define ARUG_H_
//#include <iostream>											// Upgrade.	// 01/24/01 AM.
#include <assert.h>
#include "lite/lite.h"
//#include "mode.h"				// 12/10/99 AM.

// Readability definitions.
typedef void APARSE;			// Handle for the current parse.
typedef void ARULES;			// List of rules.
typedef void ASELECTS;		// List of select node names.
//typedef void APASS;

// Forward reference.
//class NLP;
class Mode;						// 04/04/00 AM.

/********************************************
* CLASS:	ARUG
* CR:		05/20/99 AM.
* SUBJ:	API Class for rule generation.
********************************************/

class LITE_API Arug
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Arug();									// Default constructor.
	~Arug();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide

	// Make empty rules list.
	static ARULES *makeRules();

	// Make empty selects list.
	static ASELECTS *makeSelects();

	// Delete selects list.	// 05/21/09 AM.
	static bool deleteSelects(ASELECTS *selects);

	APASS *makeSeq(
		bool active,
		_TCHAR *algo_name,
		_TCHAR *rules_file_name,
		_TCHAR *comment
		);

	// Delete rules list.
	static void rmRules(ARULES* &rules);

	// Pretty print the rules list.
	static void prettyRules(ARULES *rules);

	// Generate a rule for a concept's sample.
	static bool genSample(		// 05/20/99 AM.
		long start,				// Start offset of sample in input text.
		long end,				// End offset of sample in input text.
		APARSE *parse,			// Current text analysis instance.
		int num,					// Current pass number.
		_TCHAR *conname,			// Concept name.
		/*DU*/
		APASS *seq,				// Down: current pass. UP: new pass.
									// New pass contains rule for sample.
		ARULES *rules,			// Rules for current concept.
		ASELECTS *selects		// List of select node names (context).
		);

	// Generate a rule for a concept's sample, 
	static bool genxSample(		// 06/08/99 AM.
		_TCHAR *fname,			// Sample's text file name.
		long start,				// Start offset of sample in input text.
		long end,				// End offset of sample in input text.
		_TCHAR *text,				// Sample text.  // 06/16/99 AM.
		CONCEPT *conc,			// Current concept in Gram hierarchy.
		APARSE *parse,			// Current text analysis instance.
		_TCHAR *conname,			// Concept name.
		Mode *mode,				// 08/09/99 AM.
		CG *cg,					// Current KB.
		/*DU*/
		ARULES *rules,			// Rules for current concept.
		ASELECTS *selects		// List of select node names (context).
		);

	// Attach rules, selects to pass.
	static bool fillPass(APASS *rugpass, ARULES *rules, ASELECTS *selects);

	// Generate pass file for a rug pass.
	static bool genRulesfile(APASS *pass, _TCHAR *fname);

	// Remove duplicate rules from a list.		// 06/07/99 AM.
	static ARULES *dedup(ARULES *rules);

	// Generalize and merge rules.				// 06/28/99 AM.
	static ARULES *generalize(ARULES *rules, Mode *mode);

protected:
};



#endif
/*************************      END OF FILE      ******************************/
