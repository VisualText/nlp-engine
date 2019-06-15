/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	REC.H
* FILE:	lite\rec.h
* CR:		11/07/99 AM.
* SUBJ:	Recursive algorithm for analyzer pass.
*
*******************************************************************************/

#ifndef REC_H_
#define REC_H_

#include "ln.h"
#include "pn.h"
//#include "tree.h"
#include "lite/algo.h"
#include "seqn.h"
#include "pat.h"

// Forward reference.

/********************************************
* CLASS:	REC
* CR:		11/07/99 AM.
* SUBJ:	Class for recursive analyzer algorithm.
********************************************/

class Rec : public Algo
{
public:
	Rec();						// Default constructor.
	Rec(const Rec &orig);	// Copy constructor.

	// Access

	// Modify

	// General

	// Tells how to execute the current pattern-based pass.
	// Call it with a parse tree built from a rules file also.
	virtual bool Execute(Parse *, Seqn *);
	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);




	bool RecmatchSelect(	// 11/08/99 AM.
		Node<Pn> *select,
		Delt<Irule> *rules,
		Parse *parse
		);
	bool RecmatchSelect(	// 11/08/99 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse				// Current analysis instance.
		);
	bool RectraverseSelect(	// 11/08/99 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Delt<Irule> *rules,		// Rule set to match against.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);
	bool RectraverseSelect(	// 11/08/99 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Selt<Irule> *musts,		// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);
	bool RecmultiSelect(	// 11/08/99 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);
	bool RecmultimatchSelect(	// 11/08/99 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		bool froot					// If current node is the root of the subtree.
		);
	bool RecpathSelect(	// 11/08/99 AM.
		Delt<Iarg> *selects,				// Names of select path nodes.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,						// Current analysis instance.
		Node<Pn> *node						// Current node of parse tree.
		);
	bool RecpathmatchSelect(	// 11/08/99 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		bool froot					// If current node is the root of the subtree.
		);
	bool RecmatchNodes(	// 11/08/99 AM.
		Node<Pn> *start,			// Start node of range.
		Node<Pn> *end,				// End node of range, inclusive.
										// Null means move from start till end of list.
		Delt<Irule> *rules,		// Rule set to match against.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *select
		);
	bool RecmatchNodes(	// 11/08/99 AM.
		Node<Pn> *start,			// Start node of range.
		Node<Pn> *end,				// End node of range, inclusive.
										// Null means move from start till end of list.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *select
		);

	static	// RECINFLOOP	// 05/10/13 AM.
	inline bool RecinfLoop(		// 06/09/05 AM.
		Nlppp *nlppp,
		Pn *pn,
		Parse *parse,				// Current analysis instance.
		/*DU*/
		long &state_start,
		long &state_end,
		long &state_count,
		long &state_line,

		bool &state_matched,	// If matched any state.
		long &state_toggle,	// Point to 1 or 2 as next fill.
		long &state_start2,
		long &state_end2,
		long &state_count2,
		long &state_line2
		);

	static inline Node<Pn> *matchRestart(Nlppp*);          // RECOPT1  // 04/19/06 AM.
	static Node<Pn> *recMoveback(Nlppp*);                             // 02/04/05 AM.

	// COMPILED VARIANTS
	static bool Execute(														// 06/01/00 AM.
		Parse * parse,
		const PASS *pass					// Static pass data.					// 06/21/00 AM.
		);
	static bool traverseSelect(											// 06/01/00 AM.
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node
		);
	static bool matchNodes(													// 06/01/00 AM.
		Node<Pn> *start,
		Node<Pn> *end,
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *select
		);

	// COMPILED RUNTIME VARIANTS.
	static bool RecpathSelect(												// 06/08/00 AM.
		const _TCHAR *arr_select[],		// Names of select nodes.
		Parse *parse,								// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node								// Current node of parse tree.
		);
	static bool RecmultiSelect(											// 06/08/00 AM.
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node				// Current node of parse tree.
		);
	static bool RecmultimatchSelect(										// 06/08/00 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		bool froot					// If current node is the root of the subtree.
		);


	// COMPILED HASHED VARIANTS
	static bool Hexecute(													// 06/20/00 AM.
		Parse * parse,
		const PASS *pass					// Static pass data.					// 06/21/00 AM.
		);
	static bool HtraverseSelect(											// 06/20/00 AM.
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node
		);
	static bool HmatchNodes(													// 06/20/00 AM.
		Node<Pn> *start,
		Node<Pn> *end,
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *select
		);

	static bool HRecpathSelect(												// 06/20/00 AM.
		const _TCHAR *arr_select[],		// Names of select nodes.		
		Parse *parse,								// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node								// Current node of parse tree.
		);
	static bool HRecmultiSelect(											// 06/20/00 AM.
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node				// Current node of parse tree.
		);
	static bool HRecmultimatchSelect(										// 06/20/00 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		bool froot					// If current node is the root of the subtree.
		);

	// Class-wide

private:

};


#endif

