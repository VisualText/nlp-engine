/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IEXPRSTMT.H
* FILE:	lite/iexprstmt.h
* CR:		11/11/99 AM.
* SUBJ:	Declares for NLP++ expr-statement semantics.
*******************************************************************************/

#ifndef IEXPRSTMT_H_
#define IEXPRSTMT_H_
//#include <iostream.h>
#include <assert.h>
#include "istmt.h"

// Forward reference.
class Gen;					// 05/08/00 AM.
class Iexpr;
class RFASem;				// 11/12/99 AM.

/********************************************
* CLASS:	IEXPRSTMT
* CR:		11/11/99 AM.
* SUBJ:	Interpreted expr-statement semantic object.
********************************************/

class Iexprstmt : public Istmt
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iexprstmt &);

public:
	Iexprstmt(											// Default constructor.
		RFASem * = 0,
		long = 0																	// 03/12/02 AM.
		);
	~Iexprstmt();										// Destructor.

	Iexprstmt(Iexprstmt &orig);					// Copy constructor.

	virtual void clear();
	virtual void zero();
#ifdef LINUX
	virtual void copy(const Iexprstmt *orig);
#else
	virtual void Iexprstmt::copy(const Iexprstmt *orig);
#endif

	virtual void print(std::_t_ostream &);						// 12/27/99 AM.

	const Iexprstmt &operator=(const Iexprstmt &);	// Assignment.

	// Access
	RFASem *getExpr();

	// Modify
	void setExpr(RFASem *);

	// General.
	virtual void genExprstmt(std::_t_ostream &ofile);
	virtual bool same(Istmt *stmt1, Istmt *stmt2);

	virtual bool eval(Nlppp *, /*UP*/ RFASem* &);

	virtual bool genEval(Gen *);					// 05/06/00 AM.

	// Class-wide member functions.

protected:
	RFASem *expr_;

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;					// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
