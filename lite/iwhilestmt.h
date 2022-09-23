/*******************************************************************************
Copyright (c) 2000-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IWHILESTMT.H
* FILE:	lite/iwhilestmt.h
* CR:		08/31/00 AM.
* SUBJ:	Declares for NLP++ while-statement semantics.
*******************************************************************************/

#ifndef IWHILESTMT_H_
#define IWHILESTMT_H_
//#include <iostream.h>
#include <assert.h>
#include "istmt.h"

// Forward reference.
class Gen;
class RFASem;

/********************************************
* CLASS:	IWHILESTMT
* CR:		08/31/00 AM.
* SUBJ:	Interpreted while-statement semantic object.
********************************************/

class Iwhilestmt : public Istmt
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iwhilestmt &);

public:
	Iwhilestmt(											// Default constructor.
		RFASem * = 0,									// cond
		RFASem * = 0,									// block
		long = 0			// Line number in pass file.					// 03/12/02 AM.
		);
	~Iwhilestmt();										// Destructor.

	Iwhilestmt(Iwhilestmt &orig);					// Copy constructor.

	virtual void clear();
	virtual void zero();
	virtual void copy(const Iwhilestmt *orig);	// Linux?
	//virtual void Iwhilestmt::copy(const Iwhilestmt *orig);

	virtual void print(std::_t_ostream &);

	const Iwhilestmt &operator=(const Iwhilestmt &);	// Assignment.

	// Access
	RFASem *getCond();
	RFASem *getBlock();

	// Modify
	void setCond(RFASem *);
	void setBlock(RFASem *);

	// General.
	virtual void genWhilestmt(std::_t_ostream &ofile);
	virtual bool same(Istmt *stmt1, Istmt *stmt2);
	virtual bool eval(Nlppp *, /*UP*/ RFASem* &);

	virtual bool genEval(Gen *);

	// Class-wide member functions.

protected:
	RFASem *cond_;
	RFASem *block_;

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
