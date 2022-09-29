/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IOP.H
* FILE:	lite/iop.h
* CR:		11/09/99 AM.
* SUBJ:	Declares for NLP++ operator semantics.
* EXAMPLE:	Using C-like operators.  ++, --, !=, ==, etc.
*******************************************************************************/

#ifndef IOP_H_
#define IOP_H_
//#include <iostream.h>
#include <assert.h>

// WARN:	Ioptype and s_Ioptype must be kept in sync.
enum Ioptype
	{
	NULLOP,
	PLUSOP,
	MINUSOP,
	TIMESOP,
	DIVIDEOP,
	REMOP,		// %			// Remainder op.						// 12/31/99 AM.
	CONFOP,		// %%			// NEW OPERATOR FOR CONFIDENCE!	// 12/17/99 AM.
	INCOP,		// ++
	DECOP,		// --

	ASSIGNOP,	// =
	OUTOP,		// <<			// New operator for output!		// 12/31/99 AM.
	
	// Relational ops
	EQOP,
	NEOP,
	LTOP,
	LEOP,
	GTOP,
	GEOP,

	// Bool
	ANDOP,
	OROP,
	NOTOP,

	DUMMYTERMINATOROP
	};

// Forward reference.
class Nlppp;
class RFASem;

/********************************************
* CLASS:	IOP
* CR:		11/09/99 AM.
* SUBJ:	Interpreted operator semantic object.
********************************************/

class Iop
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iop &);

public:
	Iop(											// Default constructor.
		enum Ioptype		= NULLOP			// Op type.
		);
	~Iop();										// Destructor.

	Iop(Iop &orig);							// Copy constructor.

	void clear();
	void zero();
#ifdef LINUX
	void copy(const Iop *orig);
#else
	void Iop::copy(const Iop *orig);
#endif

	const Iop &operator=(const Iop &);	// Assignment.

	// Access
	enum Ioptype getType();

	// Modify
	void setType(enum Ioptype);

	// General.
	void genOp(std::_t_ostream &ofile);
	bool eval(Nlppp *, /*UP*/ enum Ioptype &);
	bool eval(Nlppp *, /*UP*/ RFASem* &);

	// Class-wide member functions.

	static bool same(Iop *op1, Iop *op2);
	static _TCHAR *Optype(enum Ioptype);
	static enum Ioptype Optype(_TCHAR *);

protected:
	enum Ioptype type_;				// Type of operator.

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
