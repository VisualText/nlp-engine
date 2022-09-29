/*******************************************************************************
Copyright (c) 2000-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IPNODE.H
* FILE:	lite/ipnode.h
* CR:		10/18/00 AM.
* SUBJ:	Declares for Ipnode semantics.
* NOTE:	Syntax same as Ivar, minus the variable name.
*				X([num])		Numth context (i.e., dominant) node. (last if no num).
*				S()			Suggested-node.
*				N([num])		Numth rule element's node (last if no num).
*			name=variable's name as string. num=numeric.
*******************************************************************************/

#ifndef IPNODE_H_
#define IPNODE_H_
//#include <iostream.h>
#include <assert.h>


// WARN:	Ipnodetype and s_Ipnodetype must be kept in sync.
enum Ipnodetype
	{
	NULLPNODE,
	CONTEXTPNODE,
	SUGGESTEDPNODE,
	NODEPNODE,
	RUNNODEPNODE,			// For compiled runtime pnodes.
	DUMMYPNODE
	};

// Forward reference.
class Nlppp;
class Gen;
class RFASem;
class Ipair;
class Parse;

/********************************************
* CLASS:	IPNODE
* CR:		10/18/00 AM.
* SUBJ:	Interpreted parse-tree node semantic object.
********************************************/

class Ipnode
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, Ipnode &);

public:
	Ipnode(											// Default constructor.
		enum Ipnodetype	= NULLPNODE,		// Pnode type.
		long					= 0					// Associated number.
		);
	~Ipnode();										// Destructor.

	Ipnode(Ipnode &orig);							// Copy constructor.

	void clear();
	void zero();
#ifdef LINUX
	void copy(const Ipnode *orig);
#else
	void Ipnode::copy(const Ipnode *orig);
#endif

	const Ipnode &operator=(const Ipnode &);	// Assignment.

	// Access
	enum Ipnodetype getType();
	long getNum();

	// Modify
	void setType(enum Ipnodetype);
	void setNum(long);

	// General.
	void genPnode(std::_t_ostream &ofile);

	bool eval(
		Nlppp *nlppp,
		/*UP*/
		RFASem* &sem
		);

	static bool eval(
		enum Ipnodetype typ,
		int num,
		Nlppp *nlppp,
		/*UP*/
		Node<Pn>* &pnode
		);

	bool genEval(Gen *);


	// Class-wide member functions.

	static bool same(Ipnode *var1, Ipnode *var2);
	static _TCHAR Ipnodetype(enum Ipnodetype);

	static enum Ipnodetype Ipnodetype(_TCHAR);


protected:
	enum Ipnodetype type_;			// Type of variable.
	long num_;							// Associated positional number, if any.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;					// Count nodes currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
