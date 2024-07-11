/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IVAR.H
* FILE:	lite/ivar.h
* CR:		11/09/99 AM.
* SUBJ:	Declares for Ivar semantics.
* EXAMPLE:	In the rules file, vars look like function calls
*					G(name)				Global variable.
*					X(name [,num])		Context (i.e., dominant) variable.
*					S(name)				Suggested-node variable
*					N(name [,num])		Nth node's variable
*			name=variable's name as string. num=numeric.
*******************************************************************************/

#ifndef IVAR_H_
#define IVAR_H_
//#include <iostream.h>
#include <assert.h>

// This will be the index value representing "NO INDEXING".		// 10/14/00 AM.
// The old style is to fetch the value if only one, but fetch the list of
// values if there is more than one.  (Seems somewhat inconsistent.)
#define FULLARRAY -1

// Now want to allow array assignment expressions as follows:	// 10/20/00 AM.
// G("arr")[] = multistuff();
// This syntax will emphasize that multiple values are being assigned
// simultaneously.  Now
// G("arr") = stuff();  could simply assign a single value, replacing an
// array of values.  Thus, consistent treatment of multi and single values
// is possible.  single can be treated as literal or as one elt array.
#define MULTIARRAY -2

// WARN:	Ivartype and s_Vartype must be kept in sync.
enum Ivartype
	{
	NULLVAR,
	GLOBALVAR,
	CONTEXTVAR,
	SUGGESTEDVAR,
	NODEVAR,
	LOCALVAR,			// FOR USER-DEFINED FNS.						// 03/09/02 AM.
	RUNNODEVAR,			// For compiled runtime node variables.	// 05/26/00 AM.
	DUMMYVAR
	};

// Forward reference.
class Nlppp;
class Gen;			// 05/08/00 AM.
class RFASem;
class Ipair;
class Parse;
class Pn;			// 10/18/00 AM.

/********************************************
* CLASS:	IVAR
* CR:		11/09/99 AM.
* SUBJ:	Interpreted variable semantic object.
********************************************/

class Ivar
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Ivar &);

public:
	Ivar(											// Default constructor.
		enum Ivartype		= NULLVAR,		// Var type.
		_TCHAR *				= 0,				// Var name.
		long					= 0,				// Associated number.
		RFASem *				= 0				// Array index.			// 10/13/00 AM.
		);
	~Ivar();										// Destructor.

	Ivar(Ivar &orig);							// Copy constructor.

	void clear();
	void zero();
	void copy(const Ivar *orig);	// Linux?
	//void Ivar::copy(const Ivar *orig);

	const Ivar &operator=(const Ivar &);	// Assignment.

	// Access
	enum Ivartype getType();
	_TCHAR *getName();
	long getNum();
	RFASem *getIndex();														// 10/13/00 AM.

	// Modify
	void setType(enum Ivartype);
	void setName(_TCHAR *);
	void setNum(long);
	void setIndex(RFASem *);												// 10/13/00 AM.

	// General.
	void genVar(std::_t_ostream &ofile);

	// This one returns the value(s) of the variable.
	bool eval(Nlppp *, /*UP*/ RFASem* &);

	bool evalSpecial(Nlppp *nlppp, /*UP*/ RFASem* &sem);			// 11/17/99 AM.

	static bool eval(															// 05/26/00 AM.
		Ivartype vtype,
		_TCHAR *vname,
		long vnum,
		RFASem *index,															// 10/13/00 AM.
		bool ref,		// Call-by-reference.							//06/16/02 AM.
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool eval(			// Runtime variant						// 10/13/00 AM.
		Ivartype vtype,
		_TCHAR *vname,
		long vnum,
		long index,																// 10/13/00 AM.
		bool ref,		// Call-by-reference.							//06/16/02 AM.
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool evalSpecial(												// 05/26/00 AM.
		Ivartype vtype,
		_TCHAR *vname,
		long vnum,
		long index,																// 10/13/00 AM.
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool nodeVar(														// 10/18/00 AM.
		Node<Pn> *node,
		_TCHAR *name,			// Name of var.
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool nodeVarspecial(											// 10/18/00 AM.
		Node<Pn> *nstart,
		Node<Pn> *nend,
		_TCHAR *name,		// Variable name.
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);
	static bool nodeVarspecial(											// 10/18/00 AM.
		Node<Pn> *node,
		_TCHAR *name,		// Variable name.
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool varVal(Iarg *arg, /*UP*/ RFASem* &sem);			// 10/13/00 AM.

	bool genEval(Gen *,														// 05/07/00 AM.
				bool = false);		// Flag call-by-reference.			// 06/16/02 AM.

	// This one returns the variable itself, if any.
	bool find(Nlppp *, /*UP*/ Ipair* &);

	// This one finds, otherwise makes, the variable.
	bool get(Nlppp *, /*UP*/ Ipair* &);

	static bool findVar(														// 05/25/00 AM.
		Ivartype typ,
		_TCHAR *name,
		int num,
		Nlppp *nlppp,
		/*UP*/
		Ipair* &pair
		);
	static bool getVar(														// 05/25/00 AM.
		Ivartype typ,
		_TCHAR *name,
		int num,
		Nlppp *nlppp,
		/*UP*/
		Ipair* &pair
		);
	static bool makeVar(														// 05/25/00 AM.
		Ivartype typ,
		_TCHAR *name,
		int num,
		Nlppp *nlppp,
		/*UP*/
		Ipair* &pair
		);

	// This one makes the variable.
	bool make(Nlppp *, /*UP*/ Ipair* &);

	// Class-wide member functions.

	static bool same(Ivar *var1, Ivar *var2);
	static _TCHAR Vartype(enum Ivartype);
	static enum Ivartype Vartype(_TCHAR);

	static bool nodeVarhasval(Node<Pn>*, _TCHAR*);						// 06/26/01 AM.
    static bool nodeVarhasval(Pn*,_TCHAR*);                        // 06/14/05 AM.
    static bool nodeVarNZ(Pn*,_TCHAR*);                            // 06/16/05 AM.
    static bool nodeVarEQ(Pn*,_TCHAR*,_TCHAR*);                    // 06/16/05 AM.
    static bool nodeVarEQ(Pn*,_TCHAR*,long long);                       // 06/16/05 AM.
    static bool nodeVarGTLT(Pn*,_TCHAR*,long long,bool=false);
	static bool nodeRemoveval(Pn*,_TCHAR*);
	static bool nodeReplaceval(Pn*,_TCHAR*,RFASem*,bool=true,bool=false);					// 06/26/01 AM.
	static bool nodeReplaceval(Pn*,_TCHAR*,CONCEPT*,bool=true,bool=false);
	static bool nodeReplaceval(Pn*,_TCHAR*,Dlist<Iarg>*);			// 11/14/02 AM.
	static bool nodeReplaceval(Pn*,_TCHAR*,_TCHAR*,bool=true,bool=false);						// 06/26/01 AM.
	static bool nodeReplaceval(Pn*,_TCHAR*,long long,bool=true,bool=false);						// 06/26/01 AM.
	static bool nodeReplaceval(Pn*,_TCHAR*,float,bool=true,bool=false); // 12/15/14 AM.

	static bool nodePushval(Pn*,_TCHAR*,_TCHAR*,bool=false,bool=false); // [OPT] // 12/14/14 AM.
	static bool nodePushval(Pn*,_TCHAR*,RFASem*,bool=false,bool=false); // [OPT] // 12/15/14 AM.
	static bool nodePushval(Pn*,_TCHAR*,long long,bool=false,bool=false);    // [OPT] // 12/15/14 AM.
	static bool nodePushval(Pn*,_TCHAR*,float,bool=false,bool=false);   // [OPT] // 12/15/14 AM.

	static Dlist<Iarg> *nodeVarnames(Pn*);								// 05/13/02 AM.

protected:
	enum Ivartype type_;				// Type of variable.
	_TCHAR *name_;						// Variable name.
	long num_;							// Associated positional number, if any.
	RFASem *index_;					// Index into multi-valued var (ie array).

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
