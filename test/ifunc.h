/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	IFUNC.H
* FILE:	lite\ifunc.h
* CR:		12/19/01 AM.
* SUBJ:	Declares for the Ifunc function-declaration class.
* NOTE:	Functions will be declared in the @DECL region.
*
*******************************************************************************/

#ifndef IFUNC_H_
#define IFUNC_H_
#include <assert.h>
#include "func_defs.h"

//#include "lite/dlist.h"

// Forward references
class Gen;
class Iarg;
class RFASem;
class Nlppp;

/********************************************
* CLASS:	IFUNC
* CR:		12/19/01 AM.
* SUBJ:	Function-declaration object.
********************************************/

class Ifunc
{
	friend _t_ostream &operator<<(_t_ostream &, Ifunc &);

public:
	Ifunc(
		_TCHAR *			= 0,		// Action name.
		Dlist<Iarg> *	= 0,		// List of parameters.
		RFASem *			= 0,		// Function body (block).
		long				= 0,		// Line number in pass file.
		long				= 0		// Pass number.
		);								// Default constructor.
	~Ifunc();						// Destructor.

	Ifunc(Ifunc &orig);		// Copy constructor.

	void clear();
	void zero();
	void copy(const Ifunc *orig);

	// Assignment.
	const Ifunc &operator=(const Ifunc &);

	// Access
	_TCHAR *getName();
	Dlist<Iarg> *getParams();
	RFASem *getBody();
	long getLine();
	long getPass();				// 12/21/01 AM.
	enum funcDef getId();		// 12/20/01 AM.

	// Modify
	void setName(_TCHAR *);
	void setParams(Dlist<Iarg> *);
	void setBody(RFASem*);
	void setLine(long);
	void setPass(long);			// 12/21/01 AM.
	void setId(enum funcDef);	// 12/20/01 AM.

	// Class-wide member functions.
	static bool genDecls(Dlist<Iarg>*,Gen *);

	static void *createHtab(long);										// 12/20/01 AM.
	static void *makeHtab(_TCHAR**);										// 12/20/01 AM.
	static bool cleanHtab(void*);											// 12/20/01 AM.
	static bool deleteHtab(void*);										// 12/20/01 AM.
	static bool loadHtab(void*,Dlist<Iarg>*,void*,long);			// 08/23/02 AM.
	static enum funcDef funcID(_TCHAR*,void*);							// 12/21/01 AM.
	static Ifunc *htLookup(_TCHAR*,void*);								// 12/21/01 AM.

	// General.
	bool gen(_t_ostream*,Gen*);
	bool eval(Delt<Iarg>*,Nlppp*,RFASem*&);							// 12/21/01 AM.
	Dlist<Ipair> *bindParams(Delt<Iarg>*,Delt<Iarg>*,Nlppp*);	// 03/10/02 AM.

protected:
	_TCHAR *name_;							// The function name.

	Dlist<Iarg> *params_;				// The function's parameter list.

	RFASem *body_;

	long line_;		// Track line number in pass file.

	// Track the pass that the function is declared/defined in.	// 12/21/01 AM.
	long pass_;																	// 12/21/01 AM.

	// For built-in NLP++ functions only.
	enum funcDef id_;

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
