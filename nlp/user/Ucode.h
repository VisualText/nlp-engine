/*******************************************************************************
Copyright (c) 1998-2003 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	UCODE.H
* FILE:	c:\dev\nlplite\ucode.h
* CR:		12/08/98 AM.
* SUBJ:	User-defined code actions for the Pat pass.
* NOTE:	This class is a convenience for registering user-defined code
*			actions. The user
*			class has to be registered with NLP Lite.
*
*******************************************************************************/

#ifndef UCODE_H_
#define UCODE_H_
//#include "user.h"				// 01/29/99 AM.
#include "lite/dlist.h"
#include "lite/code.h"


// Forward references
class Parse;
class Auser;																	// 02/13/01 AM.
class Nlppp;																	// 02/14/01 AM.

/********************************************
* CLASS:	USER
* CR:		12/08/98 AM.
* SUBJ:	Convenience class for code actions.
********************************************/

class USER_API user
{
	//friend ostream &operator<<(ostream &, user &);

public:
	user();										// Default constructor.
	~user();

	// Access

	// Modify

	// General.
//	virtual bool execute(			// 01/29/99 AM.
	static bool execute(				// 01/29/99 AM.
		_TCHAR *func,
		DELTS *args,
		Auser *auser);															// 02/13/01 AM.

	// Class-wide
	static bool userTestfn(DELTS*,Auser*);
	static bool userAnalyzefile(DELTS*,Auser*);
	static bool userAnalyzebuf(DELTS*,Auser*);
	static bool userLoadanalyzer(DELTS*,Auser*);
	static bool userUnloadanalyzer(DELTS*,Auser*);
	static bool userPostagger(DELTS*,Auser*);                   // 12/03/03 AM.

	// RUNTIME VARIANTS GO HERE.
	static _TCHAR *testfn(Nlppp *);

	// Run another analyzer on a file.
	// Grab outputs by the analyzer to the cbuf text buffer.
	// Args: analyzer_str, inpfile_str, datum_str, ostream.
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,RFASem*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,RFASem*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,_TCHAR*,RFASem*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,RFASem*,RFASem*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,_TCHAR*,RFASem*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,RFASem*,RFASem*,_t_ostream*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,RFASem*,_TCHAR*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,RFASem*,_TCHAR*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,_TCHAR*,RFASem*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,_TCHAR*,RFASem*,RFASem*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,_TCHAR*,RFASem*,RFASem*);
	static _TCHAR *analyzefile(Nlppp*,RFASem*,RFASem*,RFASem*,RFASem*);

	// Run another analyzer on a text string.
	// Grab outputs by the analyzer to the cbuf text buffer.
	// Args: analyzer_str, input_buf, datum_str, ostream.
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,RFASem*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,RFASem*,_TCHAR*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,_TCHAR*,RFASem*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,RFASem*,RFASem*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,_TCHAR*,RFASem*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,RFASem*,RFASem*,_t_ostream*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,RFASem*,_TCHAR*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,RFASem*,_TCHAR*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,_TCHAR*,RFASem*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,_TCHAR*,RFASem*,RFASem*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,_TCHAR*,RFASem*,RFASem*);
	static _TCHAR *analyzebuf(Nlppp*,RFASem*,RFASem*,RFASem*,RFASem*);

	// Load an analyzer into memory.
	static _TCHAR *loadanalyzer(Nlppp*,_TCHAR*);
	static _TCHAR *loadanalyzer(Nlppp*,RFASem*);

	// Unload an analyzer from memory.
	static bool unloadanalyzer(Nlppp*,_TCHAR*);
	static bool unloadanalyzer(Nlppp*,RFASem*);

   // Invoke a 3rd-party POS tagger.
   static bool postagger(Nlppp*);                // 12/03/03 AM.

};



#endif
/*************************      END OF FILE      ******************************/
