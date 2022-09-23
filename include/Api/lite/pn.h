/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PN.H
* FILE:	include/API/lite/pn.h
* CR:		10/07/98 AM.
* SUBJ:	Declares for parse tree node's data.
* NOTE:	This class will know about nodes and trees.
*
*******************************************************************************/

#ifndef PN_H_
#define PN_H_
//#include <iostream.h>
#include <assert.h>

//#include "lite/tree.h"
//#include "lite/dlist.h"
//#include "ielt.h"				// 02/26/01 AM.
//#include "sem.h"
//#include "sym.h"

// Glomming node types and token types together.
// WARNING: Change pn.cpp/Pntype_s when changing Pntype.
enum Pntype
	{
	PNNULL,				// No type set.
	PNNODE,				// Nonleaf.
	PNALPHA,				// Alphabetic token.
	PNNUM,				// Numeric token.
	PNPUNCT,				// Punctuation token.
	PNWHITE,				// Whitespace token.
	PNCTRL,				// Ignorable token.
	PNEMOJI
	};

// Forward references.
class Sem;
class Arun;			// OPT			// 05/16/00 AM.
class Ivar;																		// 06/04/00 AM.
class Ielt;																		// 02/26/01 AM.
class Ipair;																	// 02/26/01 AM.
class Sym;																		// 02/26/01 AM.
class Pat;			// OPT													// 05/17/01 AM.

/********************************************
* CLASS:	PN
* CR:		10/07/98 AM.
* SUBJ:	Parse tree node's data.
********************************************/

// Exporting for LIBRUG.		// 05/18/99 AM.
class LITE_API Pn
{
	friend class Arun;													// OPT	// 05/16/00 AM.
	friend class Ivar;													// OPT	// 06/04/00 AM.
	friend class Pat;														// OPT	// 05/17/01 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, const Pn &);
	friend std::_t_ostream &operator<<(std::_t_ostream &, Node<Pn> &);

public:

public:
	Pn();
	Pn(long, long, enum Pntype, _TCHAR *, _TCHAR *,
		long=0,						// For line num.						// 05/17/01 AM.
		long=0,						// For code pass num.				// 01/04/02 AM.
		long=0						// For code line num.				// 01/04/02 AM.
		);
	~Pn();
	Pn(Pn &);						// Copy constructor. 10/26/98 AM.

	// Access
	long getStart();
	long getEnd();
	long getUstart();	// [UNICODE]	// 06/15/22 AM.
	long getUend();		// [UNICODE]	// 06/15/22 AM.
	enum Pntype getType();
	_TCHAR *getText();					// START OF NODE'S TEXT IN INPUT BUFFER.
	_TCHAR *getName();					// GET NAME OF NODE.
	int getFlags();					// 11/03/98 AM.
	Delt<Ielt> *getRulelt();
	Sem *getSem();						// 11/05/98 AM.
	Dlist<Ipair> *getDsem();		// 08/01/99 AM.
	Sym *getSym();						// 11/19/98 AM.
	Sym *getlcSym();					// 01/25/99 AM.
	long getLine();					// 05/17/01 AM.
	long getPassnum();				// 01/04/02 AM.
	long getRuleline();				// 01/04/02 AM.
	Node<Pn> *getRestart();       // 07/17/06 AM.

	bool getBase() const;			// 11/03/98 AM.
	bool getUnsealed() const;		// 10/09/99 AM.
	bool getFired() const;			// 05/21/01 Dd.
	bool getBuilt() const;			// 05/25/01 AM.

	void getTextStartEnd(_TCHAR* &, long &, long &);			// 12/29/99 AM.
	void getTextUstartUend(_TCHAR* &, long &, long &);	// [UNICODE]	// 06/15/22 AM.

	// Modify
	void setStart(long);
	void setEnd(long);
	void setStartEnd(long, long);
	void setUstart(long);	// [UNICODE]	// 06/15/22 AM.
	void setUend(long);	// [UNICODE]	// 06/15/22 AM.
	void setUstartUend(long, long);	// [UNICODE]	// 06/15/22 AM.
	void setType(enum Pntype);
	void setText(_TCHAR *);
	void setName(_TCHAR *);
	void setFlags(int);			// 11/03/98 AM.
	void setRulelt(Delt<Ielt> *);
	void setSem(Sem *);				// 11/05/98 AM.
	void setDsem(Dlist<Ipair> *);	// 08/01/99 AM.
	void setSym(Sym *);				// 11/19/98 AM.
	void setLine(long);				// 05/17/01 AM.
	void setPassnum(long);			// 01/04/02 AM.
	void setRuleline(long);			// 01/04/02 AM.
	void setRestart(Node<Pn> *);  // 07/17/06 AM.

	void setBase(bool);				// 11/03/98 AM.
	void setUnsealed(bool);			// 10/09/99 AM.
	void setFired(bool);				// 05/21/01 Dd.
	void setBuilt(bool);				// 05/25/01 AM.

	// GET THE TEXT FOR THE CURRENT NODE.
	_TCHAR *pnStr();																// 11/06/98 AM.

	// GET THE TEXT AND TEXT LENGTH FOR THE CURRENT NODE.
	_TCHAR *pnStr(long &);														// 07/23/00 AM.

	// Class member functions.
	static Tree<Pn> *makeTree(
		long ostart,
		long oend,
		long ustart,	// [UNICODE]	// 06/15/22 AM.
		long uend,	// [UNICODE]	// 06/15/22 AM.
		enum Pntype typ,
		_TCHAR *txt,
		_TCHAR *name,
		Sym *sym = 0
		);

	
	static Node<Pn> *makeNode(
		long start, long end,
		long ustart, long uend,	// [UNICODE]	// 06/15/22 AM.
		enum Pntype, _TCHAR *, _TCHAR *, Sym * = 0,
		long line=0,			// Line number for debug.				// 05/17/01 AM.
		long=0,					// Pass number for code.				// 01/04/02 AM.
		long=0					// Code lineno for debug.				// 01/04/02 AM.
			);
	static Node<Pn> *makeTnode(			// 10/09/99 AM.
		long start, long end,
		long ustart, long uend,	// [UNICODE]	// 06/15/22 AM.
		enum Pntype, _TCHAR *, _TCHAR *, Sym * = 0,
		long line=0,			// Line number for debug.				// 05/17/01 AM.
		long=0,					// Pass number for code.				// 01/04/02 AM.
		long=0					// Code lineno for debug.				// 01/04/02 AM.
			);
	static void prettyNodes(Node<Pn> *);		// 11/05/98 AM.
	static Node<Pn> *findTreenode(
		_TCHAR *name,
		Node<Pn> *node,
		bool root				// If this is the top-level node of subtree.
		);
	static bool findOffsets(			// 05/18/99 AM.
		Node<Pn> *tree,
		long o_start,			// Start offset sought.
		long o_end,				// End offset sought.
		/*UP*/
		Node<Pn>* &n_start,	// Start node that matched range.
		Node<Pn>* &n_end,		// End node that matched range.
		Node<Pn>* &parent		// Parent containing the found phrase.
		);
	static bool findOffsetsRec(		// 05/18/99 AM.
		Node<Pn> *node,
		long o_start,			// Start offset sought.
		long o_end,				// End offset sought.
		/*UP*/
		Node<Pn>* &n_start,	// Start node that matched range.
		Node<Pn>* &n_end,		// End node that matched range.
		Node<Pn>* &parent		// Parent containing the found phrase.
		);
	static bool findUoffsets(			// [UNICODE]	// 06/15/22 AM.
		Node<Pn> *tree,
		long ustart,			// Start offset sought.
		long uend,				// End offset sought.
		/*UP*/
		Node<Pn>* &n_start,	// Start node that matched range.
		Node<Pn>* &n_end,		// End node that matched range.
		Node<Pn>* &parent		// Parent containing the found phrase.
		);
	static bool findUoffsetsRec(		// [UNICODE]	// 06/15/22 AM.
		Node<Pn> *node,
		long ustart,			// Start offset sought.
		long uend,				// End offset sought.
		/*UP*/
		Node<Pn>* &n_start,	// Start node that matched range.
		Node<Pn>* &n_end,		// End node that matched range.
		Node<Pn>* &parent		// Parent containing the found phrase.
		);

	static void TraverseSetFlags(									// 05/25/01 AM.
		const Node<Pn> *node,
		bool fired,
		bool built
		);

private:
	long Start;				// Start offset of node in text.
	long End;				// End offset of node in text.
	long Ustart;	// [UNICODE]	// 06/15/22 AM.
	long Uend;	// [UNICODE]	// 06/15/22 AM.
	_TCHAR *Text;				// Ptr to start of node's text in parse buffer.
								// (May want this for nonterminal nodes also.)

	enum Pntype Type;		// Node's type.

	// Associated "concept" or literal value.
	// May hash and lookup if alphabetic.

	_TCHAR *name_;			// Name or concept associated with node.
								// For tokens, the literal string.  For nonliterals,
								// a concept like "_noun".
	Sym *sym_;				// Symbol table entry for name.  // 11/19/98 AM.
								// Keep name_ for convenience, even though redundant.

	int flags_;				// For bitflags, as we develop them. 11/3/98 AM.
								// REACTIVATED. 11/23/98 AM.

	//bool base_;			// If current node is a bottom or base attribute.
								// 11/23/98 AM. Using bit flags.

	////// Rule-tracking information /////
	// May go into a separate structure someday.
	// Could call it collect,track,mirror....
	Delt<Ielt> *rulelt_;	// Pointer to node's corresponding rule elt.

	////// Semantic information /////
	Sem *sem_;				// Task, domain, or general semantic information.

	// DYNAMIC SEMANTICS. Analogous to the global pairs of var-values used
	// in code actions.  (The above sem_ is used only by the RFA.)
	Dlist<Ipair> *dsem_;

	// For debugging. record line number this node is on.			// 05/17/01 AM.
	long line_;				// Line number corresponding to node.	// 05/17/01 AM.

	// For backtracing from parse tree to pass file in VT GUI.	// 01/04/02 AM.
	long passnum_;			// Pass num of code that created node.	// 01/04/02 AM.
	long ruleline_;		// Lineno of rule that created node.	// 01/04/02 AM.
	
	// RECOPT2. Point back to restart place for rec matching.   // 07/17/06 AM.
	Node<Pn> *restart_;  // Restart locus for rec matching.     // 07/17/06 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

#endif
