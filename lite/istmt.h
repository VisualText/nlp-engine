/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ISTMT.H
* FILE:	lite/istmt.h
* CR:		11/11/99 AM.
* SUBJ:	Declares for NLP++ statement semantics.
* NOTE:	This will be parent class for various statement types.
*			PURE VIRTUAL, ABSTRACT CLASS.
*******************************************************************************/

#ifndef ISTMT_H_
#define ISTMT_H_
//#include <iostream.h>
#include <assert.h>
#include "slist.h"

// Forward reference.
class Gen;					// 05/08/00 AM.
class Nlppp;
class RFASem;

/********************************************
* CLASS:	ISTMT
* CR:		11/11/99 AM.
* SUBJ:	Interpreted statement semantic object.
********************************************/

class Istmt
{
	// Derived types should print themselves.						// 12/27/99 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Istmt &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Selt<Istmt> &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Slist<Istmt> &);

public:
	Istmt(											// Default constructor.
		_TCHAR *typ = 0,
		long line = 0		// Line number in pass file.			// 03/12/02 AM.
		);
	virtual ~Istmt();								// Destructor.
														// VIRTUAL. 02/07/00 AM.

	Istmt(Istmt &orig);							// Copy constructor.

	virtual void clear();
	virtual void zero();
//	virtual void copy(const Istmt *orig);	# Linux?
	virtual void copy(const Istmt *orig);

	// A semi-principled workaround to the operator overload problem.
	virtual void print(std::_t_ostream &);

	const Istmt &operator=(const Istmt &);	// Assignment.

	// Access
	_TCHAR *getType();
	long getLine();															// 03/12/02 AM.

	// Modify
	void setType(_TCHAR *);
	void setLine(long);														// 03/12/02 AM.

	// General.
	virtual void genStmt(std::_t_ostream &ofile);
	virtual bool same(Istmt *stmt1, Istmt *stmt2) = 0;

	virtual bool eval(Nlppp *, /*UP*/ RFASem* &) = 0;

	// Class-wide member functions.

	static bool compare(Slist<Istmt> *stmts1, Slist<Istmt> *stmts2);
	static bool compare(Istmt *stmt1, Istmt *stmt2);

	static bool eval(Slist<Istmt> *, Nlppp *, /*UP*/ RFASem* &);

	virtual bool genEval(Gen *) = 0;						// 05/06/00 AM.
	static bool genEval(Slist<Istmt> *, Gen *);		// 05/06/00 AM.

protected:
	_TCHAR type_[64];							// Type of stmt object.
	long line_;		// Line number in pass file.						// 03/12/02 AM.

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
