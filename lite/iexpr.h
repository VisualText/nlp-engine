/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IEXPR.H
* FILE:	lite/iexpr.h
* CR:		11/09/99 AM.
* SUBJ:	Declares for NLP++ expression semantics.
* EXAMPLE:	Using C-like syntax.
* NOTE:	Using RFASem is an overgeneralization, but good for a quick and
*			dirty cut.  (These I classes should all inherit from some Iobject
*			class, etc.)
*******************************************************************************/

#ifndef IEXPR_H_
#define IEXPR_H_
//#include <iostream.h>
#include <assert.h>

// Forward reference.
class Gen;			// 05/08/00 AM.
class RFASem;
class Iop;
class Nlppp;
class Ivar;
class Ipair;		// 12/27/99 AM.
class Iarg;			// 12/27/99 AM.

// WARN:	Iexprtype and s_Vartype must be kept in sync.
enum Iexprtype
	{
	NULLEXPR,
	BINARYEXPR,
	ASSIGNEXPR,
	UNARYEXPR,
	POSTUNARYEXPR,

	DUMMYTERMINATOREXPR
	};

/********************************************
* CLASS:	IEXPR
* CR:		11/09/99 AM.
* SUBJ:	Interpreted expression semantic object.
********************************************/

class Iexpr
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Iexpr &);

public:
	Iexpr(											// Default constructor.
		enum Iexprtype		= NULLEXPR,			// Expr type.
		RFASem *				= 0,					// LHS expression.
		Iop *					= 0,					// Operator. (binary or unary).
		RFASem *				= 0,					// RHS expression. (or single).
		Iop *					= 0,					// Post-unary operator.
		long					= 0					// Line number in pass file.
		);
	~Iexpr();										// Destructor.

	Iexpr(Iexpr &orig);							// Copy constructor.

	void clear();
	void zero();
#ifdef LINUX
	void copy(const Iexpr *orig);
#else
	void Iexpr::copy(const Iexpr *orig);
#endif

	const Iexpr &operator=(const Iexpr &);	// Assignment.

	// Access
	enum		 Iexprtype getType();
	RFASem	*getLhs();
	Iop		*getOp();
	RFASem	*getRhs();
	Iop		*getPostop();
	long		 getLine();														// 05/18/01 AM.

	// Modify
	void setType(enum Iexprtype);
	void setLhs(RFASem *);
	void setOp(Iop *);
	void setRhs(RFASem *);
	void setPostop(Iop *);
	void setLine(long);														// 05/18/01 AM.

	// General.
	void genExpr(std::_t_ostream &ofile);
	bool eval(Nlppp *, /*UP*/ RFASem* &);

	bool genEval(Gen *,bool = false);									// 05/07/00 AM.

	// Class-wide member functions.

	static bool same(Iexpr *expr1, Iexpr *expr2);
	static _TCHAR *Exprtype(enum Iexprtype);
	static enum Iexprtype Exprtype(_TCHAR *);
	static																		// 06/01/00 AM.
	bool unpackVar(						// 11/15/99 AM.
		RFASem *semvar,
		Nlppp *,
		/*UP*/
		Ivar* &var,
		Ipair* &pair,
		Dlist<Iarg>* &vals,
		long &index																// 10/14/00 AM.
		);
	static bool inc(RFASem *, Nlppp *, /*UP*/ long long&);				// 06/01/00 AM.
	static bool dec(RFASem *, Nlppp *, /*UP*/ long long&);				// 06/01/00 AM.
	static bool vtnot(RFASem *, Nlppp *, /*UP*/ long long&);				// 06/01/00 AM.
	static inline bool inc_interp(RFASem*,Nlppp*,/*UP*/long long&);	// 06/13/00 AM.
	static inline bool dec_interp(RFASem*,Nlppp*,/*UP*/long long&);	// 06/13/00 AM.
	inline bool eval_lhs_only(RFASem*,/*UP*/RFASem*&,bool&);
																					// 12/24/02 AM.

	static void ruleError(long,Nlppp*);									// 12/21/01 AM.
	static Iarg *getVarindex(Dlist<Iarg> *,long);					// 03/11/02 AM.

protected:
	enum Iexprtype type_;				// Type of expression.
	RFASem *lhs_;							// LHS expression.
	Iop *op_;								// Binary (or unary) operator.
	RFASem *rhs_;							// RHS expression.
	Iop *postop_;							// Post-unary operator.

	long line_;	// Line number in pass file.							// 05/18/01 AM.

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
