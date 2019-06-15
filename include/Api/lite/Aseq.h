/*******************************************************************************
Copyright © 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ASEQ.H
* FILE:	include/API/lite/Aseq.h
* CR:		01/07/99 AM.
* SUBJ:	API for accessing the analyzer sequence.
* NOTE:	Supports editing of the sequence by the VisualText GUI.
*			Shouldn't create an object of this class, just use its functions.
*
*******************************************************************************/

#ifndef ASEQ_H_
#define ASEQ_H_
#include <assert.h>
#include "lite.h"

// Readability definitions.
typedef void APASS;

// Forward reference.
class NLP;

/********************************************
* CLASS:	ASEQ
* CR:		01/07/99 AM.
* SUBJ:	API Class for user-editing of the analyzer sequence.
********************************************/

class Aseq
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Aseq();									// Default constructor.
	~Aseq();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide

	// Create analyzer sequence.  Allow a header comment.
	LITE_API static bool makeSeq(NLP *nlp, _TCHAR *comment);

	// Delete analyzer sequence.
	LITE_API static bool deleteSeq(NLP *nlp);

	// Get sequence.  Internal convenience function.
	LITE_API static void *getSeq(NLP *nlp);

	// Read sequence from file.
	LITE_API static bool readFile(NLP *nlp);

	// Write sequence to file.
	LITE_API static bool writeFile(NLP *nlp);

	// Create a new pass.
	LITE_API static APASS *makePass(NLP *nlp, _TCHAR *algo, _TCHAR *data,
										_TCHAR *comment,	bool active);

	// Insert pass after given pos.  pos == 0 means insert as 1st pass.
	LITE_API static APASS *insertPass(NLP *nlp, void *pass, int pos);

	// Insert pass after given pass.
	LITE_API static APASS *insertAfter(NLP *nlp, APASS *loc, APASS *pass);

	// Variant.																	// 2/23/99 AM.
	LITE_API static APASS *insertAfter(NLP *nlp, APASS *loc,
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment,	bool active);


	// Excise pass at given position. NOTE: Caller must delete pass.
	LITE_API static APASS *excisePass(NLP *nlp, int pos);

	// Excise given pass.  NOTE: Caller must delete or reattach pass.
	LITE_API static APASS *excisePass(NLP *nlp, APASS *pass);

	// Delete pass in the sequence.
	LITE_API static void deletePass(NLP *nlp, int pos);

	// Delete pass standalone.  NOTE: Pass must not be within a sequence.
	LITE_API static void deletePass(APASS *pass);

	// Get nth pass from sequence.
	LITE_API static APASS *nthPass(NLP *nlp, int pos);

	// For traversing sequence.
	LITE_API static APASS *firstPass(NLP *nlp);
	LITE_API static APASS *lastPass(NLP *nlp);
	LITE_API static APASS *nextPass(APASS *pass);
	LITE_API static APASS *prevPass(APASS *pass);

	// Get pass info.
	LITE_API static bool unpackPass(NLP *nlp, int pos, /*UP*/
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool &active);
	LITE_API static bool unpackPass(APASS *pass, /*UP*/
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool &active);
	LITE_API static _TCHAR *getAlgo(NLP *nlp, int pos);
	LITE_API static _TCHAR *getAlgo(APASS *pass);
	LITE_API static _TCHAR *getData(NLP *nlp, int pos);
	LITE_API static _TCHAR *getData(APASS *pass);
	LITE_API static _TCHAR *getComment(NLP *nlp, int pos);
	LITE_API static _TCHAR *getComment(APASS *pass);
	LITE_API static bool getActive(NLP *nlp, int pos);
	LITE_API static bool getActive(APASS *pass);

	LITE_API static int  getPassnum(APASS *pass);					// 09/23/99 AM.

	// Set pass info.
	LITE_API static void packPass(NLP *nlp, int pos,
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool active);
	LITE_API static void packPass(NLP *nlp, APASS *pass,
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool active);
	LITE_API static void setAlgo(NLP *nlp, int pos, _TCHAR *algo);
	LITE_API static void setAlgo(NLP *nlp, APASS *pass, _TCHAR *algo);
	LITE_API static void setData(NLP *nlp, int pos, _TCHAR *data);
	LITE_API static void setData(NLP *nlp, APASS *pass, _TCHAR *data);
	LITE_API static void setComment(NLP *nlp, int pos, _TCHAR *comment);
	LITE_API static void setComment(NLP *nlp, APASS *pass, _TCHAR *comment);
	LITE_API static void setActive(NLP *nlp, int pos, bool active);
	LITE_API static void setActive(NLP *nlp, APASS *pass, bool active);

	// Set the ordinal number for the given pass.					// 09/23/99 AM.
	LITE_API static void setPassnum(APASS *pass, int num);

	// 02/24/99 AM. To notify when a pass has been modified, eg, when a
	// rules file has been modified and saved.
	LITE_API static void setDirty(NLP *nlp, APASS *pass);

	// Internalize the analyzer sequence so that it can be executed.
	// Note that the sequence can be internalized and executed without being
	// saved to a file, thereby enabling testing before committing.
	LITE_API static bool internSeq(NLP *nlp);

	// Renumber the passes of the analyzer sequence.
	LITE_API static void numberSeq(NLP *nlp);

	// 10/13/99 AM.
	// Move a pass up or down.  "Smart" because it's aware of stub regions.
	// Renumbers parse tree also.
	// Returns true if successful (eg, not moving first pass up!).
	// WARN: In general such motion requires RUG GENERATE ALL.
	LITE_API static bool smartUp(NLP *nlp, APASS *pass);
	LITE_API static bool smartUp(NLP *nlp, int pos);
	LITE_API static bool smartDown(NLP *nlp, APASS *pass);
	LITE_API static bool smartDown(NLP *nlp, int pos);

	// MOVING TO DUMB VERSIONS OF THESE.								// 12/13/99 AM.
	// Because future VisualText version will have folders of passes.
	// Renumbers parse tree also.
	// Returns true if successful (eg, not moving first pass up!).
	// WARN: In general such motion requires RUG GENERATE ALL.
	LITE_API static bool moveUp(NLP *nlp, APASS *pass);
	LITE_API static bool moveUp(NLP *nlp, int pos);
	LITE_API static bool moveDown(NLP *nlp, APASS *pass);
	LITE_API static bool moveDown(NLP *nlp, int pos);

	// Find start stub going backward.									// 10/13/99 AM.
	// If find end stub first, that's an error.
	LITE_API static APASS *rfindStartstub(NLP *nlp, APASS *pass);

	// Find end stub going forward.										// 10/13/99 AM.
	LITE_API static APASS *findEndstub(NLP *nlp, APASS *pass);

	// If given pass is a start or end stub.							// 10/13/99 AM.
	LITE_API static bool startStub(APASS *pass);
	LITE_API static bool endStub(APASS *pass);

protected:
};



#endif
/*************************      END OF FILE      ******************************/
