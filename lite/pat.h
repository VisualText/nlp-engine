/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PAT.H
* FILE:	lite\pat.h
* CR:		10/18/98 AM.
* SUBJ:	Class for the main pattern-based (or rule-based) pass.
* NOTE:	Should include the general pattern-matching engine and action
*			execution machinery.  Should not know about particular rules files.
*
*******************************************************************************/

#ifndef PAT_H_
#define PAT_H_

#include "ln.h"
#include "pn.h"
#include "tree.h"
#include "lite/algo.h"
#include "seqn.h"
#include "lite/Arun.h"				// 05/26/00 AM.
#include "thtab.h"					// 02/27/01 AM.

// Direction we're moving in to do pattern matching.
// WARNING: Change pat.cpp/Dir_s when changing Dir.
enum Dir
	{
	DIRNULL,				// No direction set.
	LEFT,					// Match backward from trigger.
	RIGHT,				// Match forward from trigger.
	DONE					// Successfully matched rule in current direction.
	};

class Seqn;			// Forward reference.
class Ielt;
//class Isugg;			// 01/13/00 AM.
class Nlppp;			// 11/19/99 AM.
class Parse;			// 08/22/02 AM.
class Iaction;			// 01/13/00 AM.
class Ipre;				// 02/26/01 AM.
class Htab;				// 02/26/01 AM.

/********************************************
* CLASS:	PAT
* CR:		10/18/98 AM.
* SUBJ:	Class for the main pattern-based pass type.
********************************************/

class Pat : public Algo
{
public:
	Pat();			// Default constructor.
	Pat(const Pat &orig);	// Copy constructor.   // 12/03/98 AM.

	// Access
	//Delt<Irule> *getRule();

	// Modify
	//void setRule(Delt<Irule> *);

	// Tells how to execute the current pattern-based pass.
	// Call it with a parse tree built from a rules file also.
	virtual bool Execute(Parse *, Seqn *);
	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);					// 12/04/98 AM.

	static bool matchSelect(
		Node<Pn> *select,
		Delt<Irule> *rules,
		Parse *parse
		);
	static bool matchSelect(		// 12/18/98 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse				// Current analysis instance.
		);
	static bool traverseSelect(	// 11/30/98 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Delt<Irule> *rules,		// Rule set to match against.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);
	static bool traverseSelect(	// 12/18/98 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Selt<Irule> *musts,		// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);

	static bool multiSelect(		// 03/25/99 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);
	static bool multimatchSelect(
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		bool froot					// If current node is the root of the subtree.
		);
	static bool premultiSelect(		// 10/25/99 AM.
		Dlist<Iarg> *selects,	// Names of select nodes.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *node				// Current node of parse tree.
		);
	static bool premultimatchSelect(	// 10/25/99 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		bool froot					// If current node is the root of the subtree.
		);

	static bool pathSelect(		// 10/19/99 AM.
		Delt<Iarg> *selects,				// Names of select path nodes.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,						// Current analysis instance.
		Node<Pn> *node						// Current node of parse tree.
		);
	static bool pathmatchSelect(	// 10/19/99 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		bool froot					// If current node is the root of the subtree.
		);

	static bool matchNodes(
		Node<Pn> *start,			// Start node of range.
		Node<Pn> *end,				// End node of range, inclusive.
										// Null means move from start till end of list.
		Delt<Irule> *rules,		// Rule set to match against.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *select			// 08/05/99 AM.
		);

	static bool matchNodes(		// 12/18/98 AM.
		Node<Pn> *start,			// Start node of range.
		Node<Pn> *end,				// End node of range, inclusive.
										// Null means move from start till end of list.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab,	// Hashed rule table.
		Parse *parse,				// Current analysis instance.
		Node<Pn> *select			// 08/05/99 AM.
		);

	static Selt<Irule> *resetRules(		// 12/18/98 AM.
		Node<Pn> *node,					// Current node in parse tree.
		Selt<Irule> *musts,				// Rules that must be tried.
		tHtab<Slist<Irule> > *htab, 	// Hashed rule table.
		/*DU*/
		Slist<Irule>* &rules				// Rules list to build.
		);
	static bool matchRules(						// 12/19/98 AM.
		Nlppp *nlppp,				// Parse state.		// 11/19/99 AM.
		/*DU*/
		Delt<Irule>* &rules		// DOWN. Rules to match on.
										// UP. Pointer to matched rule in list.
		);
	static bool matchRules(		// 11/19/99 AM.
		Nlppp *nlppp,				// Parse state.
		/*DU*/
		Selt<Irule>* &rules		// DOWN. Rules to match on.
										// UP. Pointer to matched rule in list.
		);

	//static bool matchRule(Irule *, Node<Pn> *, Node<Pn>	*, Node<Pn>	*,
	//					/*DU*/ Tree<Pn>* &, Node<Pn>* &, Node<Pn>* &);
	static bool matchRule(		// 11/19/99 AM.
		Nlppp *nlppp
		);
	static bool matchTrigger(	// 11/19/99 AM.
		Delt<Ielt> *trig,		// Trigger rule element.
		Nlppp *nlppp,			// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,		// Rule elt to continue matching against.
		enum Dir  &dir			// Direction to continue matching in.
		);
	static bool matchFirst(		// 11/19/99 AM.
		Delt<Ielt> *elt1,		// First rule element.
		Nlppp *nlppp,			// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,		// Rule elt to continue matching against.
		enum Dir  &dir			// Direction to continue matching in.
		);
	static bool matchLeft(		// 11/19/99 AM.
		Delt<Ielt> *trig,		// Trigger rule element.
		Nlppp *nlppp,			// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,		// Rule elt to continue matching against.
		enum Dir  &dir,		// Direction to continue matching in.
		Node<Pn>* &wildone,	// First unfilled wildcard.
		int &wildmin,			// Min nodes that wildcards must cover.
		int &wildmax			// Total possible nodes that wildcards can cover.
		);
	static bool matchRight(	// 11/19/99 AM.
		Delt<Ielt> *trig,		// Trigger rule element. (NOT USED HERE).
		Nlppp *nlppp,			// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,		// Rule elt to continue matching against.
		enum Dir  &dir,		// Direction to continue matching in.
		Node<Pn>* &wildone,	// First unfilled wildcard.
		int &wildmin,			// Min nodes that wildcards must cover.
		int &wildmax			// Total possible nodes that wildcards can cover.
		);
	static bool wildLeft(	// 11/19/99 AM.
		Delt<Ielt> *trig,		// Trigger rule element.
		Nlppp *nlppp,			// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,		// Rule elt to continue matching against.
		enum Dir  &dir,		// Direction to continue matching in.
		Node<Pn>* &wildone,	// First unfilled wildcard.
		int &wildmin,			// Min nodes that wildcard must cover.
		int &wildmax			// Total possible nodes that wildcards can cover.
		);
	static bool wildRight(	// 11/19/99 AM.
		Delt<Ielt> *trig,		// Trigger rule element.
		Nlppp *nlppp,			// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,		// Rule elt to continue matching against.
		enum Dir  &dir,		// Direction to continue matching in.
		Node<Pn>* &wildone,	// First unfilled wildcard.
		int &wildmin,			// Min nodes that wildcard must cover.
		int &wildmax			// Total possible nodes that wildcards can cover.
		);

	static bool matchRealLeft(
		Ielt *ielt,				// Rule elt to continue matching against.
		Node<Pn> *start,		// Left bound of tree phrase (inclusive).
									// If null, no left bound.
		Node<Pn> *wildone,	// First unfilled wildcard.
		int wildmin,
		int wildmax,			// Total possible nodes that wildcards can cover.
		/*DU*/
		Node<Pn>* &real,		// First real node matched (moving left).
									// (Allows for repetition.)
		Node<Pn>* &node		// DN: Current node.  UP: Last real node matched.
		);

	static bool trueMatchLeft(
		Ielt *ielt,					// OPT // 10/06/99 AM.
		Node<Pn> *start,
		/*DU*/
		Node<Pn>* &node,			// DN: Current node.  UP: Last real matched node.
		Node<Pn>* &real			// First real matched node, moving left.
		);

	static bool wildFillLeft(
		Node<Pn> *s_coll,			// Collect for first pending wildcard.
		Node<Pn> *e_coll,			// One past collect for last pending wildcard.
		Node<Pn> *s_node,			// First unmatched node to assign to wildcards.
		Node<Pn> *e_node			// One past last unmatched node.
		);

	static bool matchRealRight(
		Ielt *ielt,			// Rule elt to continue matching against.
		Node<Pn> *end,		// Right bound of tree phrase (inclusive).
									// If null, no right bound.
		Node<Pn> *wildone,	// First unfilled wildcard.
		int wildmin,
		int wildmax,			// Total possible nodes that wildcards can cover.
		Nlppp *nlppp,        // 04/27/06 AM.
		/*DU*/
		Node<Pn>* &real,		// First real node matched (moving left).
									// (Allows for repetition.)
		Node<Pn>* &node,		// DN: Current node.  UP: Last real node matched.
		long & tot				// 02/04/05 AM.
		);

	static bool trueMatchRight(
		Ielt *ielt,					// OPT // 10/06/99 AM.
		Node<Pn> *end,
		Nlppp *nlppp,        // 04/27/06 AM.
		/*DU*/
		Node<Pn>* &node,			// DN: Current node.  UP: Last real matched node.
		Node<Pn>* &real,			// First real matched node, moving left.
		int &tot						// Count of nodes matched.
		);

	static bool wildFillRight(
		Node<Pn> *s_coll,			// Collect for first pending wildcard.
		Node<Pn> *e_coll,			// One past collect for last pending wildcard.
		Node<Pn> *s_node,			// First unmatched node to assign to wildcards.
		Node<Pn> *e_node			// One past last unmatched node.
		);

	static bool Match(
		Ielt *ielt,				// OPT	// 10/05/99 AM.
		Node<Pn> *node
		);

	static bool modeMatch(
		Ielt *ielt,				// OPT	// 10/05/99 AM.
		Pn *pn,					// OPT	// 10/05/99 AM.
		/*UP*/
		bool &failmode				// 03/23/99 AM.
		);
	static bool modeMatch1(		// 12/04/98 AM.
		_TCHAR *nname,				// Node name.
		enum Pntype ntype,		// Node type.
		_TCHAR *ename,					// One of rule elt names from a match/fail list.
		bool deaccent,						// 01/28/05 AM.
		bool dejunk                               // 09/09/11 AM.
		);
	static bool modeHash(			// 12/12/98 AM.
		_TCHAR *nname,				// Node name.
		Htab *htab,					// List's hash table.
		bool deaccent,                          // 03/27/05 AM.
		bool dejunk                               // 09/09/11 AM.
		);
	static bool modeMatches(					// 10/16/99 AM.
		Dlist<Iarg> *list,					// EXCEPTS list.
		Htab *htab,										// 11/09/99 AM.
		_TCHAR *nname,							// Node name.
		enum Pntype typ,						// Node type.
		bool deaccent,										// 01/28/05 AM.
		bool dejunk                               // 09/09/11 AM.
		);
	static bool modeFails(						// 10/16/99 AM.
		Dlist<Iarg> *list,					// EXCEPTS list.
		Htab *htab,										// 11/09/99 AM.
		_TCHAR *nname,							// Node name.
		enum Pntype typ,						// Node type.
		bool deaccent,										// 01/28/05 AM.
		bool dejunk                               // 09/09/11 AM.
		);
	static bool deaccentMatch(	// 01/28/05 AM.
		_TCHAR *ename,
		_TCHAR *nname
		);
	static bool dejunkMatch(	// 09/09/11 AM.
		_TCHAR *ename,
		_TCHAR *nname
		);
	static bool literalMatch(
		_TCHAR *ename,
		_TCHAR *nname
		);
	static bool singletMatch(
		Ielt *ielt,				// OPT	// 10/05/99 AM.
		Node<Pn> *node,
		/*UP*/
		bool &failmode						// 10/18/99 AM.
		);
	static bool treeMatch(					// 03/17/99 AM.
		Ielt *ielt,				// OPT	// 10/05/99 AM.
		Node<Pn> *node,
		bool root,			// If this is the top-level node of subtree.
		/*UP*/
		bool &failmode						// 10/18/99 AM.
		);
	static bool preMatch(			// 11/30/98 AM.
//		Dlist<Ipre> *pres,
//		Pn *pn
//		);
		Ielt *ielt,							// 06/16/05 AM.
		Node<Pn> *node						// 06/16/05 AM.
		);
//	static bool leafMatch(			// 01/31/99 AM.	// 06/16/05 AM.
//		Ielt *ielt,					// 10/05/99 AM.				// 06/16/05 AM.
//		Node<Pn> *node													// 06/16/05 AM.
//		);
	static bool singletExcept(	// 10/18/99 AM.
		Ielt *ielt,
		Node<Pn> *node,
		bool failmode
		);
	static bool treeExcept(		// 10/18/99 AM.
		Dlist<Iarg> *elist,
		Htab *htab,										// 11/09/99 AM.
		Node<Pn> *node,
		bool deaccent,									// 01/28/05 AM.
		bool dejunk,                           // 09/09/11 AM.
		bool root			// If this is the top-level node of subtree.
		);


	static bool preAction(			// 11/30/98 AM.
		_TCHAR *func,					// Name of pre action.
		Delt<Iarg> *args,			// Action's arguments.
//		Pn *pn						// Current node being matched.	// 06/16/05 AM.
		Node<Pn> *node				// 06/16/05 AM.
		);
	static bool matchEOF(			// 11/25/98 AM.
		Ielt *ielt,			// Current rule element.	// 10/05/99 AM.
		Node<Pn> *last				// Last node matched by rule.
		);
	static bool matchSTART(		// 01/30/99 AM.
		Ielt *ielt,			// Current rule element.	// 10/05/99 AM.
		Node<Pn> *last				// Last node matched by rule.
		);
	static bool matchEND(			// 01/21/99 AM.
		Ielt *ielt,			// Current rule element.	// 10/05/99 AM.
		Node<Pn> *last				// Last node matched by rule.
		);

	static bool eltMatches(		// 11/25/98 AM.
		Ielt *ielt,			// Current rule element.	// 10/05/99 AM.
		_TCHAR *name
		);

	static bool switchRight(		// 11/19/99 AM.
		Delt<Ielt> *trig,
		Nlppp *nlppp,				// 11/19/99 AM.
		/*DU*/
		Delt<Ielt>* &elt,
		enum Dir &dir
		);
	static bool execActions(			// 11/25/98 AM.
		Nlppp *nlppp							// 11/19/99 AM.
		);

	static bool checkActions(		// 11/27/98 AM.
		Nlppp *nlppp		// 11/19/99 AM.
		);
	static bool checkAction(			// 11/27/98 AM.
		Iaction *check,						// 01/13/00 AM.
		Nlppp *nlppp,							// 11/19/99 AM.
		/*UP*/
		bool &badname							// 01/13/00 AM.
		);
	static bool checkFinal(			// 11/27/98 AM.
		bool accept,				// If accepting rule match.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool checkEnd(          // 05/20/08 AM.
		Nlppp *nlppp
		);

	static bool postActions(			// 11/19/99 AM.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool postAction(
		Iaction *post,					// 01/13/00 AM.
		Nlppp *nlppp,					// 11/19/99 AM.
		bool &badname					// 01/13/00 AM.
		);
	static bool postFinal(
		Nlppp *nlppp				// 11/19/99 AM.
		/*DU*/
//		Node<Pn>* &after // Saved node for continued matching.	// 12/14/03 AM.
		);
	static bool initLookahead(Nlppp *nlppp);							// 11/22/99 AM.
	static bool updateLookahead(long,Nlppp*);							// 12/14/03 AM.

	static bool redSingle(
		Delt<Iarg> *args,														// 05/26/01 AM.
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static Node<Pn> *redsingle(											// 12/04/03 AM.
						Node<Pn>*,Node<Pn>*,_TCHAR*,bool,bool,Nlppp*);

	static bool redSingler(											// 01/17/99 AM.
		Delt<Iarg> *args,		// Argument list for subphrase.
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool redSinglex(											// 01/17/99 AM.
		Delt<Iarg> *args,		// Argument list for subphrase.
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool redMerger(													// 12/02/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool redMerge(													// 12/02/99 AM.
		Nlppp *nlppp
		);
	static bool redSinglezap(												// 11/21/00 AM.
		Nlppp *nlppp
		);
	static bool postListadd(												// 11/16/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool listAddleft(												// 12/05/99 AM.
		int num1,			// The ITEM.
		int num2,			// The LIST.
		bool keep,																// 12/14/99 AM.
		Nlppp *nlppp
		);
	static bool postGroup(													// 06/15/99 AM.
		Delt<Iarg> *args,		// Argument list for the post action.
		Nlppp *nlppp															// 11/19/99 AM.
		);

	static Node<Pn> *makeCollect(
		Tree<Pn> *collect,			// The current tracking list.
		Delt<Ielt> *rulelt,			// Current rule elt.
		Node<Pn> *left,				// Leftmost node covered by rule elt.
		Node<Pn> *right,				// Rightmost node covered by rule elt.
		enum Dir dir					// Collecting LEFT or RIGHT.
		);
	static bool collectNth(											// 01/24/99 AM.
		Tree<Pn> *collect,
		long ord,
		/*UP*/
		Node<Pn>* &nstart,
		Node<Pn>* &nend
		);
	static bool collectNthnew(										// 12/03/99 AM.
		Tree<Pn> *collect,
		long ord,
		/*UP*/
		Node<Pn>* &nstart,
		Node<Pn>* &nend
		);

	static inline bool wildElt(Ielt *ielt);						// 10/05/99 AM.
	static inline bool optionalElt(Ielt *ielt);				// 10/05/99 AM.
	static inline bool warnWild(Irule *,Nlppp*);
	static inline Node<Pn> *nodeRight(
		Node<Pn> *node,			// Current node.
		Node<Pn> *end				// Right boundary, inclusive.
		)
		{
		return (node && node != end) ? node->pRight : 0;
		}
	static inline Node<Pn> *nodeLeft(
		Node<Pn> *node,			// Current node.
		Node<Pn> *start			// Left boundary, inclusive.
		)
		{
		return (node && node != start) ? node->pLeft : 0;
		}
	static bool matchCleanup(Tree<Pn> *collect);


	static bool postExcise(
		Delt<Iarg> *args,			// Argument list for the excise action.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool postSplice(				// 07/30/99 AM.
		Delt<Iarg> *args,			// Argument list for the excise action.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool postSetbase(					// 12/12/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postSetunsealed(				// 01/26/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postInc(							// 12/08/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postAddstrs(					// 01/24/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXaddlen(					// 08/05/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXinc(						// 08/06/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXaddNvar(					// 09/23/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXset(						// 10/31/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXXsetifzero(				// 10/31/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXsetX(						// 11/03/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postNinc(						// 10/02/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postXrename(					// 12/25/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);

	static bool postPrint(						// 01/30/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPrintr(						// 01/30/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPrlit(						// 01/30/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPrrange(					// 01/30/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPrchild(					// 03/17/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPrtree(						// 03/22/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPrxtree(					// 09/25/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postPranchor(					// 03/05/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postFprintnvar(				// 08/05/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp								// 11/19/99 AM.
		);
	static bool postFprintxvar(				// 11/26/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postFprintgvar(				// 12/03/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postNdump(						// 12/13/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postXdump(						// 12/13/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postGdump(						// 12/13/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postSdump(													// 12/13/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postAddattr(												// 02/15/00 AM.
		Delt<Iarg> *args,			// Argument list for the action.
		Nlppp *nlppp
		);
	static bool printr(												// 02/09/99 AM.
		long ord1,
		long ord2,
		Tree<Pn> *collect,
		Parse *parse,
		std::_t_ostream *ostr
		);
	static bool printr(														// 06/02/00 AM.
		Node<Pn> *node1,
		Node<Pn> *node2,
		std::_t_ostream *ostr,
		Nlppp *nlppp
		);
	static bool getr(													// 03/09/99 AM.
		long ord1,
		long ord2,
		Tree<Pn> *collect,
		Parse *parse,
		/*DU*/
		_TCHAR* &str,
		long &len
		);

	static Node<Pn> *Layering(
		Node<Pn> *node,
		Dlist<Iarg> *attrs,
		Parse*																	// 08/22/02 AM.
		);
	static void Layering(												// 03/24/99 AM.
		Node<Pn> *node,			// Current node, for reference.
		Dlist<Iarg> *attrs,
		Ielt *elt,																// 07/24/03 AM.
		Parse*,																	// 08/22/02 AM.
		/*UP*/
		Node<Pn>* &top,
		Node<Pn>* &bottom
		);
	static bool Recurse(
		Node<Pn> *pcoll,			// Collect covering nodes to pattern match on.
		Dlist<Irule> *ruleset,	// Rules to use in pattern matching.
		Parse    *parse,			// Current parse instance.
		/*DU*/
		Node<Pn>* &first,			// First node matched by rule.
		Node<Pn>* &last			// Last node matched by rule.
		);
	static bool Recurse(														// 05/31/00 AM.
		COLL *pcoll,			// Collect covering nodes to pattern match on.
		bool (rules)(int,bool&,Nlppp*),	// Rule fn to use in pattern matching.
		Parse    *parse,			// Current parse instance.
		/*DU*/
		Node<Pn>* &first,			// First node matched by rule.
		Node<Pn>* &last			// Last node matched by rule.
		);
	static bool execPhrase(	// 11/20/99 AM.
		Nlppp *nlppp				// 11/19/99 AM.
		);
	static bool layerElt(			// 03/24/99 AM.
		Node<Pn> *pcoll,			// Collect covering nodes of current elt.
		Dlist<Iarg> *attrs,		// Attributes to layer on nodes of current elt.
		Ielt *elt,					// Element data.						// 07/24/03 AM.
		Parse    *parse,			// Current parse instance.
		/*DU*/
		Node<Pn>* &first,			// First node matched by rule.
		Node<Pn>* &last			// Last node matched by rule.
		);
	static Node<Pn> *groupElt(												// 09/23/00 AM.
		Node<Pn> *pcoll,
		_TCHAR *gp,
		Nlppp *nlppp
		);
	static Node<Pn> *groupElts(											// 09/23/00 AM.
		Node<Pn> *coll1,
		Node<Pn> *coll2,
		Node<Pn> *node1,
		Node<Pn> *node2,
		_TCHAR *name,					// Name for group of nodes.
		Nlppp *nlppp
		);
	static bool spliceElts(long,long,Nlppp*);							// 07/19/02 AM.
#ifdef _OBSOLETE_022403
	static bool updateParent(									// 12/19/99 AM.
	Node<Pn> *node1,			// First node to be excised.
	Node<Pn> *node2			// Last node to be excised.
	);
#endif
	static bool matchRightspecial(									// 12/19/99 AM.
		_TCHAR *ename,			// Elt name.
		Nlppp *nlppp,
		/*DU*/
		Delt<Ielt>* &elt,
		enum Dir  &dir,
		Node<Pn>* &wildone,
		int &wildmin,
		int &wildmax
		);

	static Node<Pn> *baseNode(Node<Pn> *node);					// 01/24/99 AM.
	static _TCHAR *baseStr(Node<Pn> *node);						// 01/30/99 AM.
	static Node<Pn> *leafNode(Node<Pn> *node);					// 01/30/99 AM.
	static _TCHAR *rangeText(Node<Pn> *nstart, Node<Pn> *nend, Parse *parse);
					// 03/05/99 AM.
	static bool treeText(													// 09/26/00 AM.
		Node<Pn> *node,
		bool root,				// If this is the top-level node of subtree.
		bool spaces,			// If to include spaces between leaf node text
		bool &first,				// If this is the first node in the tree.
		_TCHAR *buf,				// Buffer to fill.
		_TCHAR* &ptr,				// First empty spot in buffer.
		long &siz				// Empty space left in buffer.		// 10/29/04 AM.
		);

	static Node<Pn> *group(											// 05/25/00 AM.
		Node<Pn> *node1,	// from
		Node<Pn> *node2,	// to
		_TCHAR *name,			// Name for group node.
		Nlppp *nlppp
		);
	static void prettyPass(Node<Pn>*,Tree<Pn>*, Parse *);			// 06/08/00 AM.
	static bool postErrout(bool,Nlppp*);								// 11/20/01 AM.

	// COMPILED VARIANTS.
	static bool Execute(														// 05/13/00 AM.
		Parse * parse,
		const PASS *pass					// Static pass data.					// 06/21/00 AM.
#ifdef OLD_
		bool (code)(Nlppp *),
		const _TCHAR **arr_select,
		enum selectType,														// 06/08/00 AM.
		bool (rules)(int,bool&,Nlppp *)
#endif
		);
	static bool traverseSelect(										// 05/13/00 AM.
#ifdef OLD_
		const _TCHAR *arr_select[],
		bool (rules)(int,bool&,Nlppp *),
#endif
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node
		);
	static bool matchNodes(													// 05/13/00 AM.
		Node<Pn> *start,
		Node<Pn> *end,
#ifdef OLD_
		bool (rules)(int,bool&,Nlppp *),
#endif
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *select
		);
	static bool matchNodes(													// 05/13/00 AM.
		Node<Pn> *start,
		Node<Pn> *end,
		bool (rules)(int,bool&,Nlppp *),
		Parse *parse,
		Node<Pn> *select
		);
	static bool matchRules(													// 05/13/00 AM.
		Nlppp *nlppp,
		/*DU*/
		bool (rules)(int,bool&,Nlppp *)
		);

	// HASHED VARIANTS.
	static bool Hexecute(													// 06/20/00 AM.
		Parse * parse,
		const PASS *pass					// Static pass data.					// 06/21/00 AM.
#ifdef OLD_
		bool (code)(Nlppp *),
		const _TCHAR **arr_select,
		enum selectType,
		const RHASH *must,
		const RHASH **htab,
		bool (rules)(int,bool&,Nlppp *)
#endif
		);
	static bool HtraverseSelect(									// 06/20/00 AM.
#ifdef OLD_
		const _TCHAR *arr_select[],
		const RHASH *must,
		const RHASH **htab,
		bool (rules)(int,bool&,Nlppp *),
#endif
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node
		);
	static bool HmatchNodes(												// 06/20/00 AM.
		Node<Pn> *start,
		Node<Pn> *end,
#ifdef OLD_
		const RHASH *must,
		const RHASH **htab,
		bool (rules)(int,bool&,Nlppp *),
#endif
		Parse *parse,
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *select
		);
	static bool matchRules(													// 06/20/00 AM.
		Nlppp *nlppp,
		/*DU*/
		int *arr,
		bool (rules)(int,bool&,Nlppp *)
		);
	static bool HpathSelect(													// 06/20/00 AM.
		const _TCHAR *arr_select[],		// Names of select nodes.
#ifdef OLD_
		const RHASH *must,
		const RHASH **htab,
		bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
#endif
		Parse *parse,						// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node						// Current node of parse tree.
		);
	static bool HmultiSelect(												// 06/20/00 AM.
#ifdef OLD_
		const _TCHAR *arr_select[],		// Names of select nodes.
		const RHASH *must,
		const RHASH **htab,
		bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
#endif
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.

		Node<Pn> *node				// Current node of parse tree.
		);
	static bool HmultimatchSelect(										// 06/20/00 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
#ifdef OLD_
		const RHASH *must,
		const RHASH **htab,
		bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
#endif
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.

		bool froot					// If current node is the root of the subtree.
		);


	static inline void resetRule(Node<Pn> *, Nlppp *);     // 05/18/00 AM.
//	static inline Node<Pn> *updateRestart(Nlppp*,Node<Pn> *);// 07/17/06 AM.
//	static inline void endRestart(Nlppp*);                 // 07/17/06 AM.

	static bool wild_backfill_right(Nlppp *);							// 05/18/00 AM.
	static bool wild_backfill_left(Nlppp *);							// 05/30/00 AM.

	static bool layerElt(													// 05/25/00 AM.
		COLL *pcoll,				// Collect covering nodes of current elt.
		const _TCHAR **attrs,		// Attributes to layer on nodes of current elt.
		Parse    *parse,			// Current parse instance.
		/*DU*/
		Node<Pn>* &first,			// First node matched by rule.
		Node<Pn>* &last			// Last node matched by rule.
		);

	// Compiled variant post actions. ///////////
	static bool single(Nlppp *);											// 05/19/00 AM.
	static void Layering(													// 05/25/00 AM.
		Node<Pn> *node,			// Current node, for reference.
		const _TCHAR **attrs,
		Parse*,																	// 08/22/02 AM.
		/*UP*/
		Node<Pn>* &top,
		Node<Pn>* &bottom
		);
	static Node<Pn> *Layering(Node<Pn> *,const _TCHAR **);			// 05/19/00 AM.
	static bool args_range(													// 06/02/00 AM.
		int from,
		int to,
		COLL *coll,
		int siz,					// Size of collect.
		Nlppp *nlppp,	// 09/21/13 AM.
		/*UP*/
		Node<Pn>* &start,
		Node<Pn>* &end
		);
	static bool merge(														// 06/05/00 AM.
		Nlppp *nlppp
		);
	static bool pathSelect(													// 06/08/00 AM.
		const _TCHAR *arr_select[],		// Names of select nodes.
#ifdef OLD_
		bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
#endif
		Parse *parse,						// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node						// Current node of parse tree.
		);
	static bool multiSelect(												// 06/08/00 AM.
//		const char *arr_select[],		// Names of select nodes.
//		bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		Node<Pn> *node				// Current node of parse tree.
		);
	static bool multimatchSelect(											// 06/08/00 AM.
		Node<Pn> *select,			// Select, or chosen tree node.
//		bool (rules)(int,bool&, Nlppp *),		// Fn of rules to match against.
		Parse *parse,				// Current analysis instance.
		const PASS *pass,					// Static pass data.					// 06/21/00 AM.
		bool froot					// If current node is the root of the subtree.
		);
	static const int *resetRules(											// 06/20/00 AM.
		Node<Pn> *node,			// Current node in parse tree.
		const PASS *pass					// Static pass data.					// 06/21/00 AM.
#ifdef OLD_
		const RHASH *must,
		const RHASH **htab
#endif
		);


/********************************************
* FN:	UPDATERESTART
* CR:	07/17/06 AM.
* SUBJ:	Update Recursive optimization status in rule matching. RECOPT2
* RET:	The previous rightmost node, if changed by this call.
* ALG:	Track rightmost node reached by any rule in recursive pass.
*			Each node points the earliest node that covered it.
*			On rule match in recursive pass, restart at backup pointed to by first
*			matched node.
* NOTE:	
********************************************/

static inline Node<Pn> *updateRestart(	// RECOPT2
	Nlppp *nlppp,
	Node<Pn> *node	// Current node being matched within a rule match.
	)
{
// Don't want to clobber the previous rightmost settings.	// 08/12/08 AM.
if (node && node != nlppp->rmost_)	// 08/12/08 AM.
	{
	if (nlppp->rmost_) // Finding a new rightmost.
		{
		Pn *pnx = node->getData();
		// For safety, goto RESTART of first node.	// FIXREC201306.	// 06/03/13 AM.
		if (nlppp->first_)
		  {
		  Pn *pnf = nlppp->first_->getData();
		  Node<Pn> *rst = pnf->restart_;
		  if (rst)
			pnx->restart_ = rst;
		  else
			pnx->restart_ = nlppp->first_;  // Restart at first node of current rule match.
		  }
		nlppp->rmost_ = node;	// Update.	// 07/23/06 AM.
		}
	else if (nlppp->rightmost_ == node)	// Found rightmost in current rule.
		{
		Node<Pn> *rmost = nlppp->rightmost_;
		nlppp->rightmost_ = 0;
		nlppp->rmost_ = node;	// Track rightmost in current rule match.	// 07/23/06 AM.
		return rmost;
		}
	else if (!nlppp->rightmost_)	// 07/23/06 AM.
		{
		nlppp->rmost_ = node;		// 07/23/06 AM.
		node->getData()->restart_ = 0;	// 07/23/06 AM.
		}

	// else, haven't reached rightmost_ yet.
	}
return 0;
}


/********************************************
* FN:	ENDRESTART
* CR:	07/17/06 AM.
* SUBJ:	Update Recursive optimization status in rule matching at end of failed rule. RECOPT2
* RET:	Node at which to restart matching in current phrase.
* ALG:	Track rightmost node reached by any rule in recursive pass.
*			Each node points the earliest node that covered it.
*			On rule match in recursive pass, restart at backup pointed to by first
*			Convention: rightmost == 0 means it's at the current node.
*			matched node.
* NOTE:	
********************************************/

static inline void endRestart(	// RECOPT2
	Nlppp *nlppp
	)
{
if (nlppp->rmost_ // Exceeded rightmost in current rule.
// && nlppp->last_
 )
//	nlppp->rightmost_ = nlppp->last_->Right();	// Failure point.
	nlppp->rightmost_ = nlppp->rmost_;	// Update tracking.	// 07/23/06 AM.
		// NOTE: This may change when doing the group and reduce actions, etc.
}

private:
	//Delt<Irule> *rule_;		// The current rule being processed.
};


#endif
