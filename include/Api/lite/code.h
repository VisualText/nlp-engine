/*******************************************************************************
Copyright � 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CODE.H
* FILE:	include/API/lite/code.h
* CR:		12/07/98 AM.
* SUBJ:	Interpretive Code Region execution.  Part of semantic processing.
* NOTE:	Just provides functions.  Should not create objects of this class.
*
*******************************************************************************/

#ifndef CODE_H_
#define CODE_H_
#include <assert.h>
#include "lite.h"			// 12/08/98 AM.
#include "dlist.h"		// 12/08/98 AM.


// Forward references
class Parse;
class Seqn;
class Iaction;
class Iarg;

/********************************************
* CLASS:	CODE
* CR:		12/07/98 AM.
* SUBJ:	Class for interpretive code execution.
********************************************/

class LITE_API Code
{
	//friend ostream &operator<<(ostream &, Code &);

public:
	Code();									// Default constructor.
	virtual ~Code();						// Destructor.

	// Access

	// Modify

	// General.
	virtual bool execute(	// Let code subclass handle action.
		_TCHAR *funct,
		Delt<Iarg> *args,
		Parse *parse) = 0;

	// Class-wide
//	static bool userCode(char *func, Delt<Iarg> *args, Parse *parse);
	static bool regCode(Code *);
	static void clean();

	static void execCode(Parse *parse, Seqn *seqn,
										bool &exitpass);						// 02/06/01 AM.
	static bool codeAction(Iaction *action, Parse *parse,
		/*UP*/ bool &badname);												// 01/14/00 AM.

	static bool codeVar(     Delt<Iarg> *args, Parse *parse);
	static bool codeVarstrs( Delt<Iarg> *args, Parse *parse);
	static bool codePrint(   Delt<Iarg> *args, Parse *parse);
	static bool codePrintvar(Delt<Iarg> *args, Parse *parse);
	static bool codeFprintvar(Delt<Iarg> *args, Parse *parse);
	static bool codeStartout(Delt<Iarg> *args, Parse *parse);
	static bool codeFileout(Delt<Iarg> *args,Parse *parse);		// 02/09/99 AM.
	static bool codePrlit(Delt<Iarg> *args,Parse *parse);			// 02/09/99 AM.
	static bool codeStopout( Delt<Iarg> *args, Parse *parse);
	static bool codeSortvals(Delt<Iarg> *args, Parse *parse);
	static bool codeLookup(  Delt<Iarg> *args, Parse *parse);

	static bool codeGdump(													// 12/13/99 AM.
		Delt<Iarg> *args,
		Parse *parse
		);
	static bool codeGtolower(												// 12/13/99 AM.
		Delt<Iarg> *args,
		Parse *parse
		);
	static bool codeGuniq(													// 12/13/99 AM.
		Delt<Iarg> *args,
		Parse *parse
		);

	// So user actions can get hold of some data.
	static std::_t_ostream *getOut(Parse *parse);

	// UTILITIES
	static bool codeLookup(													// 06/04/00 AM.
	_TCHAR *var,
	_TCHAR *fname,
	_TCHAR *flag,
	Parse *parse
	);

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
