/*******************************************************************************
Copyright (c) 2002-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IRETURN.H
* FILE:	lite/ireturn.h
* CR:		03/07/02 AM.
* SUBJ:	Declares for NLP++ return-statement semantics.
*******************************************************************************/

#ifndef IRETURN_H_
#define IRETURN_H_
//#include <iostream.h>
#include <assert.h>
#include "istmt.h"

// Forward reference.
class Gen;
class RFASem;

/********************************************
* CLASS:	IRETURN
* CR:		03/07/02 AM.
* SUBJ:	Interpreted return-statement semantic object.
********************************************/

class Ireturn : public Istmt
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Ireturn &);

public:
	Ireturn(											// Default constructor.
		RFASem * = 0,									// expr
		long = 0			// Line number in pass file.					// 03/12/02 AM.
		);
	~Ireturn();										// Destructor.

	Ireturn(Ireturn &orig);					// Copy constructor.

	virtual void clear();
	virtual void zero();
	virtual void copy(const Ireturn *orig);

	virtual void print(std::_t_ostream &);

	const Ireturn &operator=(const Ireturn &);	// Assignment.

	// Access
	RFASem *getExpr();

	// Modify
	void setExpr(RFASem *);

	// General.
	virtual void genReturnstmt(std::_t_ostream &ofile);
	virtual bool same(Istmt *stmt1, Istmt *stmt2);
	virtual bool eval(Nlppp *, /*UP*/ RFASem* &);

	virtual bool genEval(Gen *);

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
