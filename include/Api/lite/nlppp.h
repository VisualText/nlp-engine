/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NLPPP.H
* FILE:	include/API/lite/inlppp.h
* CR:		11/14/99 AM.
* SUBJ:	Declares for NLP++ execution object.
* NOTE:	Data for executing in POST,CHECK region of a match (in PAT, REC).
*******************************************************************************/

#ifndef NLPPP_H_
#define NLPPP_H_
//#include <iostream.h>
#include <assert.h>
//#include "node.h"
//#include "tree.h"
//#include "lite/dlist.h"			// 02/22/00 AM.
#include "slist.h"	// For local string mgmt. // 01/08/07 AM.
#include "lite/Arun.h"			// For runtime data structures.	// 05/17/00 AM.

typedef bool (*PREFN)(int,Nlppp *);										// 05/24/00 AM.

enum NlpppRegion
	{
	REGNULL,
	REGCODE,
	REGCODEFIN,		// Anticipates @FIN following @CODE.
	REGCOND,			// Anticipates @COND.
	REGPRE,
	REGCHECK,
	REGPOST,
	REGRULE,			// Possible.  No nlp++ inside a rule phrase yet.
	REGDECL,																		// 12/24/01 AM.
	REGSENTINEL		// Don't worry about comma at the end of this list.
	};

// Forward reference.
class Irule;
class Parse;
class Sem;
class Pn;
class Ipair;
class Pat;			// 11/19/99 AM.
class Check;		// 11/19/99 AM.
class PostRFA;		// 11/20/99 AM.
class Rec;			// 11/20/99 AM.
class Htab;			// 12/29/99 AM.
class Code;			// 01/14/00 AM.
class Arun;			// 05/16/00 AM.
class Ivar;			// 05/26/00 AM.
class Fn;			// 06/10/00 AM.
class Istmt;		// 06/10/00 AM.
class Ipnode;		// 10/18/00 AM.
class	Iaction;		// 11/20/01 AM.
class Iexpr;		// 11/20/01 AM.
class Iwhilestmt;	// 02/13/02 AM.
class Ireturn;		// 03/07/02 AM.
class Ifunc;		// 03/07/02 AM.

/********************************************
* CLASS:	NLPPP
* CR:		11/14/99 AM.
* SUBJ:	Execution context object.
********************************************/

class LITE_API Nlppp
{
	friend class Pat;								// OPT			// 11/19/99 AM.
	friend class Check;							// OPT			// 11/19/99 AM.
	friend class PostRFA;						// OPT			// 11/20/99 AM.
	friend class Rec;								// OPT			// 11/20/99 AM.
	friend class Code;							// OPT			// 01/14/00 AM.
	friend class Arun;							// OPT			// 05/16/00 AM.
	friend class Ivar;							// OPT			// 05/26/00 AM.
	friend class Fn;								// OPT			// 06/10/00 AM.
	friend class Istmt;							// OPT			// 06/10/00 AM.
	friend class Ipnode;							// OPT			// 10/18/00 AM.
	friend class Iaction;						// OPT			// 11/20/01 AM.
	friend class Iexpr;							// OPT			// 11/02/01 AM.
	friend class Iwhilestmt;					// OPT			// 02/13/02 AM.
	friend class Ireturn;						// OPT			// 03/07/02 AM.
	friend class Ifunc;							// OPT			// 03/07/02 AM.
	friend std::_t_ostream &operator<<(std::_t_ostream &, Nlppp &);

public:
	Nlppp(											// Default constructor.
		Irule *rule = 0,
		Tree<Pn> *collect = 0,
		Parse *parse = 0,
		Node<Pn> *select = 0,
		Node<Pn> *first = 0,
		Node<Pn> *last = 0,
		Node<Pn> *node = 0,
		Node<Pn> *start = 0,
		Node<Pn> *end = 0,
		Sem *sem = 0,
		Dlist<Ipair> *dsem = 0					// 11/17/99 AM.
		);
	~Nlppp();										// Destructor.

	Nlppp(Nlppp &orig);							// Copy constructor.

	virtual void clear();
	virtual void zero();
	virtual void copy(const Nlppp *orig);

	const Nlppp &operator=(const Nlppp &);	// Assignment.

	// Access
	Irule *getRule();
	Tree<Pn> *getCollect();
	Parse *getParse();
	Node<Pn> *getSelect();
	Node<Pn> *getFirst();
	Node<Pn> *getLast();
	Node<Pn> *getNode();
	Node<Pn> *getStart();
	Node<Pn> *getEnd();
	Sem *getSem();
	Dlist<Ipair> *getDsem();				// 11/17/99 AM.
	Node<Pn>	*getLookahead();				// 11/22/99 AM.
	bool getNoop();							// 12/15/99 AM.
	Node<Pn> *getRed();						// 01/13/00 AM.
	bool getUsed();							// 01/13/00 AM.
	enum NlpppRegion getRegion();			// 01/13/00 AM.
	Dlist<Ipair> *getLocals();				// 03/09/02 AM.
	Slist<_TCHAR> *getLocalstrs();      // 01/08/07 AM.

	Node<Pn> *getAfter();					// 05/19/00 AM.
	const ELT *getElts();					// 05/17/00 AM.
	const SUGG *getSugg();					// 05/19/00 AM.
	COLL *getColl();							// 05/17/00 AM.
	int getOrd();								// 05/18/00 AM.
	Node<Pn> *getWildmark();				// 05/18/00 AM.
	Node<Pn> *getWildstart();				// 06/01/00 AM.
	int getWildord();							// 05/18/00 AM.
	int getWildmin();							// 05/18/00 AM.
	int getWildmax();							// 05/18/00 AM.
	int getLen();								// 05/18/00 AM.
	int getWildtot();							// 05/18/00 AM.
//	void *getPres();							// 05/24/00 AM.
//	bool (*getPres)(int,Nlppp *);			// 05/24/00 AM.
	PREFN getPres();							// 05/24/00 AM.
	bool getSucceed();						// 06/10/00 AM.
	bool getFail();							// 06/10/00 AM.
	int getNlookahead();						// 06/10/00 AM.
	bool getExitpass();						// 02/06/01 AM.
	bool getReturn();							// 03/07/02 AM.
	bool getInsidefn();						// 03/12/02 AM.
	long getDepth();							// 03/12/02 AM.
	bool getExcised();						// 04/28/01 AM.
	long getSpan();                     // 02/04/05 AM.
	long getMaxspan();                  // 02/04/05 AM.
	Node<Pn> *getRightmost();           // 07/17/06 AM.
	Node<Pn> *getRmost();               // 07/23/06 AM.

	Htab *htab();								// 12/29/99 AM.

	// Modify
	void setRule(Irule *);
	void setCollect(Tree<Pn> *);
	void setParse(Parse *);
	void setSelect(Node<Pn> *);
	void setFirst(Node<Pn> *);
	void setLast(Node<Pn> *);
	void setNode(Node<Pn> *);
	void setStart(Node<Pn> *);
	void setEnd(Node<Pn> *);
	void setSem(Sem *);
	void setDsem(Dlist<Ipair> *);			// 11/17/99 AM.
	void setLookahead(Node<Pn> *);		// 11/22/99 AM.
	void setNoop(bool);						// 12/15/99 AM.
	void setRed(Node<Pn> *);				// 01/13/00 AM.
	void setUsed(bool);						// 01/13/00 AM.
	void setRegion(NlpppRegion);			// 01/13/00 AM.
	void setLocals(Dlist<Ipair> *);		// 03/09/02 AM.
	void setLocalstrs(Slist<_TCHAR> *); // 01/08/07 AM.

	void setAfter(Node<Pn> *);				// 05/19/00 AM.
	void setElts(const ELT *);				// 05/17/00 AM.
	void setSugg(const SUGG *);			// 05/19/00 AM.
	void setColl(COLL *);					// 05/17/00 AM.
	void setOrd(int);							// 05/18/00 AM.
	void setWildord(int);					// 05/18/00 AM.
	void setWildmin(int);					// 05/18/00 AM.
	void setWildmax(int);					// 05/18/00 AM.
	void setWildmark(Node<Pn> *);			// 05/18/00 AM.
	void setWildstart(Node<Pn> *);		// 06/01/00 AM.
	void setWildtot(int);					// 05/18/00 AM.
	void setLen(int);							// 05/18/00 AM.
	void setPres(bool (*)(int,Nlppp *));	// 05/24/00 AM.
	void setSucceed(bool);					// 06/10/00 AM.
	void setFail(bool);						// 06/10/00 AM.
	void setNlookahead(int);				// 06/10/00 AM.
	void setExitpass(bool);					// 02/06/01 AM.
	void setReturn(bool);					// 03/07/02 AM.
	void setInsidefn(bool);					// 03/12/02 AM.
	void setDepth(long);						// 03/12/02 AM.
	void setExcised(bool);					// 04/28/01 AM.
	void setSpan(long);                 // 02/04/05 AM.
	void setMaxspan(long);              // 02/04/05 AM.
	void setRightmost(Node<Pn> *);      // 07/17/06 AM.
	void setRmost(Node<Pn> *);          // 07/23/06 AM.

	// General.
	long ruleLine();							// 12/21/01 AM.
	void freeLocals(Slist<_TCHAR> *);   // 01/08/07 AM.

	// Class-wide member functions.

protected:
	// None of this stuff belongs to the NLPPP object.
	Irule *rule_;				// Current rule.
	Tree<Pn> *collect_;	// List collecting matched nodes.
	Parse *parse_;
	Node<Pn> *select_;
	/*DU*/
	Node<Pn> *first_;		// First node matched by rule.
	Node<Pn> *last_;		// Last node matched by rule.
	Node<Pn> *node_;		// UP. Next node to match on.
	Node<Pn> *start_;		// Left bound of tree phrase (inclusive).
								// If null, no left bound.
	Node<Pn> *end_;		// Right bound of tree phrase (inclusive).
								// If null, no right bound.
	Node<Pn> *lookahead_;	// Lookahead, if any.				// 11/22/99 AM.
	Sem *sem_;				// Semantic info for rule.
	Dlist<Ipair> *dsem_;	// Dynamic semantics for sugg.		// 11/17/99 AM.

	// Flag noop reductions for recursive algorithms.			// 12/15/99 AM.
	bool noop_;																// 12/15/99 AM.

	// Data used by POST actions, after a rule has matched.	// 01/13/00 AM.
	Node<Pn> *red_;		// Suggested node built by actions.	// 01/13/00 AM.
	bool used_;				// If collect has been consumed.		// 01/13/00 AM.

	// Tell execution code what REGION (eg, post vs check)
	// the code is executing in.  Affects execution....		// 01/13/00 AM.
	enum NlpppRegion region_;											// 01/13/00 AM.

	// STORE CURRENT USER-DEFINED FUNCTION'S LOCAL VARS.			// 03/09/02 AM.
	Dlist<Ipair> *locals_;	// Fn's local var list.					// 03/09/02 AM.
	
	// LIST OF TEMPORARY STRINGS FOR CURRENT SCOPE.             // 01/08/07 AM.
	// GC, garbage collection, string optimization.             // 01/08/07 AM.
	Slist<_TCHAR> *localstrs_;                                  // 01/08/07 AM.
	
	// OPTIMIZATIONS FOR RECURSIVE PASS.                        // 02/04/05 AM.
	// Tracking counts of nodes in FAILED rule matches.         // 02/04/05 AM.
	// Using max span to move backward in successful matches.   // 02/04/05 AM.
	long maxspan_;	// Max nodes to the right matched by a rule. // 02/04/05 AM.
	long span_; // Count for the current rule match.            // 02/04/05 AM.

   // RECOPT2                                                  // 07/17/06 AM.
   // Rightmost node covered in current phrase.                // 07/17/06 AM.
   Node<Pn> *rightmost_;                                       // 07/17/06 AM.
   
   // Rightmost node established in current rule match.        // 07/23/06 AM.
   Node<Pn> *rmost_;                                           // 07/23/06 AM.

	////////////////////////////////////
	// COMPILED RUNTIME DATA STRUCTURES.								// 05/17/00 AM.
	////////////////////////////////////
	// May want to have a runtime version of Nlppp object.		// 05/17/00 AM.

	// The node to go to after a rule has matched.  Useful for
	// handling lookahead. (Could be used by interpreted
	// analyzer also.)														// 05/19/00 AM.
	// Using this in interpreted system now also.					// 12/14/03 AM.
	Node<Pn> *after_;															// 05/19/00 AM.

	// USE THIS FOR INTERPRETED ANALYZER ALSO.                  // 09/06/08 AM.
	int nlookahead_;		// Lookahead ord.								// 06/10/00 AM.

	// Note: The array is in gen'd code.  Don't delete.			// 05/17/00 AM.
	const ELT *elts_;	// Array of rule element data.				// 05/17/00 AM.
	int len_;			// Rule phrase length.  (static data.)		// 05/18/00 AM.
	const SUGG *sugg_;	// Static data for suggested concept.	// 05/19/00 AM.	

	// Compiled version of collect.  A one-time allocated array	// 05/17/00 AM.
	COLL *coll_;																// 05/17/00 AM.
	int ord_;		// Rule elt number, indexes compiled arrays.	// 05/18/00 AM.

	// End of bloc of nodes reserved for wildcards, corresponding to their
	// minimum constraint.													// 05/18/00 AM.
	Node<Pn>* wildmark_;														// 05/18/00 AM.

	// Starting element number for backfilling wildcards.			// 05/18/00 AM.
	int wildord_;																// 05/18/00 AM.

	int wildmin_;	// Total minimum constraint for wildcards.	// 05/18/00 AM.
	int wildmax_;	// Total maximum constraint for wildcards.	// 05/18/00 AM.
	int wildtot_;	// Total nodes to assign to wildcards.			// 05/18/00 AM.

	// Record the start node position for rule when no nodes have
	// matched yet and wildcards are "operating".					// 06/01/00 AM.
	Node<Pn> *wildstart_;													// 06/01/00 AM.

	// Gen'd function for pre actions.									// 05/24/00 AM.
	PREFN prefn_;																// 05/24/00 AM.

	// CONTROL OF FLOW.														// 02/06/01 AM.

	// SUCCEED/FAIL LOGIC FOR @CHECK REGION.							// 06/10/00 AM.
	bool succeed_;																// 06/10/00 AM.
	bool fail_;																	// 06/10/00 AM.

	// For interpreted NLP++, need to know when exiting a fn.	// 03/07/02 AM.
	bool return_;		// If returning from function.				// 03/07/02 AM.
	bool insidefn_;	// Tracks when we're inside a function.	// 03/12/02 AM.
	long depth_;		// Level of nesting of function calls.		// 03/12/02 AM.

	bool exitpass_;	// If exiting current pass.					// 02/06/01 AM.

	// To flag that some matched nodes were excised.				// 04/28/01 AM.
	// So rule matcher won't think that match failed to
	// consume nodes.  Not strictly for compiled, necessarily.
	bool excised_;																// 04/28/01 AM.

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
