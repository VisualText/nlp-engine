/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IIFSTMT.H
* FILE:	lite/iifstmt.h
* CR:		11/12/99 AM.
* SUBJ:	Declares for NLP++ if-statement semantics.
*******************************************************************************/

#ifndef IIFSTMT_H_
#define IIFSTMT_H_
//#include <iostream.h>
#include <assert.h>
#include "istmt.h"

// Forward reference.
class Gen;			// 05/08/00 AM.
class RFASem;

/********************************************
* CLASS:	IIFSTMT
* CR:		11/11/99 AM.
* SUBJ:	Interpreted if-statement semantic object.
********************************************/

class Iifstmt : public Istmt
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iifstmt &);

public:
	Iifstmt(											// Default constructor.
		RFASem * = 0,								// cond
		RFASem * = 0,								// if part
		RFASem * = 0,								// else part
		long = 0					// Line number in pass file.			// 03/12/02 AM.
		);
	~Iifstmt();										// Destructor.

	Iifstmt(Iifstmt &orig);					// Copy constructor.

	virtual void clear();
	virtual void zero();
#ifdef LINUX
	virtual void copy(const Iifstmt *orig);
#else
	virtual void Iifstmt::copy(const Iifstmt *orig);
#endif

	virtual void print(std::_t_ostream &);					// 12/27/99 AM.

	const Iifstmt &operator=(const Iifstmt &);	// Assignment.

	// Access
	RFASem *getCond();
	RFASem *getIf();
	RFASem *getElse();

	// Modify
	void setCond(RFASem *);
	void setIf(RFASem *);
	void setElse(RFASem *);

	// General.
	virtual void genIfstmt(std::_t_ostream &ofile);
	virtual bool same(Istmt *stmt1, Istmt *stmt2);
	virtual bool eval(Nlppp *, /*UP*/ RFASem* &);

	virtual bool genEval(Gen *);								// 05/06/00 AM.

	// Class-wide member functions.

protected:
	RFASem *cond_;
	RFASem *if_;
	RFASem *else_;

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
