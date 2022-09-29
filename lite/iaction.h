/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IACTION.H
* FILE:	lite\iaction.h
* CR:		10/24/98 AM.
* SUBJ:	Declares for the Iaction rule-action class.
* NOTE:	Ipre (pre-actions) and Ipost post-actions) will inherit from here.
* EXAMPLE:	In the rules file, actions will look like function calls
*						function(arg1, arg2, arg3...)
*			where arg1, etc. can be numeric, string...
* REF:	Need to document the action language as we go.  It looks like
*			functions for maximum flexibility.  Someday, may want to generate
*			code directly from this type of specification.
*
*******************************************************************************/

#ifndef IACTION_H_
#define IACTION_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/dlist.h"
//#include "iarg.h"					// 11/24/98 AM.
//#include "algo.h"					// 11/24/98 AM.

// Forward references
class Gen;								// 05/08/00 AM.
class Iarg;
class RFASem;							// 11/13/99 AM.
class Nlppp;							// 01/13/00 AM.
class Ifunc;							// 12/26/01 AM.

/********************************************
* CLASS:	IACTION
* CR:		10/24/98 AM.
* SUBJ:	Interpreted rule-action object.
* NOTE:	Will be placed into dlist (doubly-linked lists), for now.
********************************************/

class Iaction
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iaction &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Delt<Iaction> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Dlist<Iaction> &);

public:
	Iaction(
		_TCHAR *			= 0,		// Action name.
		Dlist<Iarg> *	= 0,		// List of arguments.
		long				= 0		// Line number in pass file.		// 05/17/01 AM.
		);								// Default constructor.
	~Iaction();						// Destructor.

	Iaction(Iaction &orig);		// Copy constructor.	// 07/02/99 AM.

	void clear();			// 07/02/99 AM.
	void zero();			// 07/02/99 AM.
#ifdef LINUX
	void copy(const Iaction *orig);
#else
	void Iaction::copy(const Iaction *orig);
#endif

	// Assignment.	// 07/02/99 AM.
	const Iaction &operator=(const Iaction &);

	// Access
	_TCHAR *getName();
	Dlist<Iarg> *getArgs();
	Dlist<Iarg> *getOrigargs();		// 03/13/02 AM.
	RFASem *getSem();						// 11/13/99 AM.
	_TCHAR *getScope();						// 02/13/01 AM.
	long getLine();						// 05/17/01 AM.
	Ifunc *getFunc();						// 12/26/01 AM.

	// Modify
	void setName(_TCHAR *);
	void setArgs(Dlist<Iarg> *);
	void setOrigargs(Dlist<Iarg> *);	// 03/13/02 AM.
	void setSem(RFASem *);				// 11/13/99 AM.
	void setScope(_TCHAR *);				// 02/13/01 AM.
	void setLine(long);					// 05/17/01 AM.
	void setFunc(Ifunc*);				// 12/26/01 AM.

	// Class-wide member functions.
	static Dlist<Iaction> *makeDlist(
		_TCHAR *,							// Action name.
		Dlist<Iarg> *,					// List of arguments.
		long = 0							// Line number.					// 05/17/01 AM.
		);
	static Delt<Iaction> *makeDelt(
		_TCHAR *,							// Action name.
		Dlist<Iarg> *,					// List of arguments.
		long = 0							// Line number.					// 05/17/01 AM.
		);
	static Delt<Iaction> *addDelt(
		Dlist<Iaction> *,				// List to add element to.
		_TCHAR *,							// Action name.
		Dlist<Iarg> *,					// List of arguments.
		long = 0							// Line number.					// 05/17/01 AM.
		);

	static void genActions(Dlist<Iaction> *actions,
													std::_t_ostream &ofile);			// 06/15/99	AM.
	static void genAction(Iaction *action, std::_t_ostream &ofile);		// 06/15/99 AM.

	static bool same(Dlist<Iaction> *list1,
											Dlist<Iaction> *list2);			// 06/07/99 AM.
	static bool same(Iaction *pre1, Iaction *pre2);					// 06/07/99 AM.

	bool eval(Nlppp *nlppp, /*UP*/ RFASem* &sem);

	static bool gen(Dlist<Iaction> *list, Gen *);

	bool genEval(std::_t_ofstream *,Gen *,bool);								// 05/07/00 AM.

protected:
	_TCHAR *name_;							// The "function" or action name.

	// SCOPE NAME.  eg, user::fnname().									// 02/13/01 AM.
	_TCHAR *scope_;																// 02/13/01 AM.

	// Todo: Map name to an actual function call for making action-object.
	Dlist<Iarg> *args_;				// The function's argument list.

	// To support recursive user-defined NLP++ function calls, need to
	// keep the original arg list.										// 03/13/02 AM.
	// Just a reference, don't delete.									// 03/13/02 AM.
	Dlist<Iarg> *origargs_;													// 03/13/02 AM.

	// Rather than overhauling action semantics, adding this to handle
	// the new NLP++ semantics.											// 11/13/99 AM.
	RFASem *sem_;																// 11/13/99 AM.

	long line_;		// Track line number in pass file.				// 05/17/01 AM.

	// OPTIMIZATION. Store the definition information here.		// 12/26/01 AM.
	Ifunc *func_;	// Reference to definition info.					// 12/26/01 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
