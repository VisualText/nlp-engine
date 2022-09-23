/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	POSTRFA.H
* FILE:	lite\postrfa.h
* CR:		11/05/98 AM.
* SUBJ:	Post actions specialized for the RFA.
* NOTE:	Part of the Pat pass.  Perhaps should have an RFA-Pat class.
*			Organizing this as a class to manage things like debug flags.
*			Should probably have a generic Post class and Action class....
*			(These are algorithms, unlike the Ipost and Iaction classes.)
*
*******************************************************************************/

#ifndef POSTRFA_H_
#define POSTRFA_H_

class PostRFA
{
public:
	PostRFA(bool debug = false);	// Default constructor.
	~PostRFA();							// Destructor.

	// Access
	static bool Debug();
	static bool Verbose();

	// Modify
	static void setDebug(bool);
	static void setVerbose(bool);

	static bool postRFAname(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAnum(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAfloat(Delt<Iarg>*,Nlppp*);					// 08/19/01 AM.
	static bool postRFAnonlit(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAstr(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAarg(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAargtolist(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAlist(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAaction(									// 11/23/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAlittopair(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFApair(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAlittoaction(							// 11/23/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAactions(								// 11/23/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAregion(									// 11/24/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAregions(								// 11/24/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArecurse(								// 11/25/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArecurses(								// 11/25/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArulesfile(								// 11/24/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArange(									// 11/29/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFApres(									// 11/29/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAnodes(									// 11/30/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAselect(									// 11/30/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAcode(									// 12/07/98 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFApairs(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAlitelt(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAnonlitelt(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAelement(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAelt(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArulelts(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAsugg(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArule(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFArules(
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	///////////////// NLP++ SEMANTICS
	static bool postRFAmovesem(								// 11/09/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAmakestmt(								// 05/11/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp,															// 11/19/99 AM.
		bool semicolon = true	// Semicolon at end.					// 05/29/01 AM.
		);
	static bool postRFAmakestmts(								// 05/29/01 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFAaddstmt(								// 11/12/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAifstmt(									// 11/12/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAifelse(									// 11/12/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAexpr(									// 11/10/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAunary(									// 11/11/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFApostunary(								// 11/11/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAvar(										// 11/09/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFAop(										// 11/09/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp															// 11/19/99 AM.
		);
	static bool postRFBdecl(Delt<Iarg>*,Nlppp*);						// 12/19/01 AM.
	static bool postRFBdecls(Delt<Iarg>*,Nlppp*);					// 12/19/01 AM.

	static bool postRFBarg(													// 12/18/99 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFBerror(												// 05/29/01 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFBscopefncallstart(								// 02/13/01 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFBfncallstart(										// 02/16/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFAaddarg(												// 02/16/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFAvarfn(												// 02/16/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFApnode(												// 10/18/00 AM.
		_TCHAR *typestr,					// Var type= G,X,N,S.
		long ord,						// Ordinality of node (for N or X).
		Delt<Iarg> *darg,				// Check on extra args.
		bool numflag,					// If numeric arg was present.
		Nlppp *nlppp
		);
	static bool postRFAvarfnarray(										// 10/13/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFApreaction(											// 02/17/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFArulemark(											// 08/31/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFAwhilestmt(											// 08/31/00 AM.
		Delt<Iarg> *args,
		Nlppp *nlppp
		);
	static bool postRFAreturnstmt(Delt<Iarg>*,Nlppp*);				// 03/07/02 AM.

	static bool postRFBunmatchedpunct(Delt<Iarg>*,Nlppp*);		// 05/19/01 AM.

	/////////////////
	/////////////////
	static bool args_1or2(
		_TCHAR *action,				// Name of current postRFA action.
		Delt<Iarg> *args,			// Argument list for the action.
		Tree<Pn> *collect,		// Tracking for current rule match.
		Sem *sem,					// Semantic object built by actions, if any.
		/*DU*/
		Node<Pn>* &node1,			// Parse tree node for first arg.
		Node<Pn>* &node2			// Parse tree node for second arg, if any.
		);
	static bool args_1(
		_TCHAR *action,				// Name of current postRFA action.
		Delt<Iarg> *args,			// Argument list for the action.
		Tree<Pn> *collect,		// Tracking for current rule match.
		Sem *sem,					// Semantic object built by actions, if any.
		/*DU*/
	 Node<Pn>* &nstart,			// Parse tree node for arg.
	 Node<Pn>* &nend				// End node, as needed.
		);
	static bool args_0to2(
		_TCHAR *action,				// Name of current postRFA action.
		Delt<Iarg> *args,			// Argument list for the action.
		Tree<Pn> *collect,		// Tracking for current rule match.
		Sem *sem,					// Semantic object built by actions, if any.
		/*DU*/
		Node<Pn>* &node1,			// Parse tree node for first arg, if any.
		Node<Pn>* &node2			// Parse tree node for second arg, if any.
		);
	static bool args_range(										// 01/17/99 AM.
		_TCHAR *action,				// Name of current postRFA action.
		Delt<Iarg> *args,			// Argument list for the action.
		Tree<Pn> *collect,		// Tracking for current rule match.
		Sem *sem,					// Semantic object built by actions, if any.
		Nlppp *nlppp,	// 09/21/13 AM.
		/*DU*/
		Node<Pn>* &node1,			// Parse tree node for first arg, if any.
		Node<Pn>* &node2			// Parse tree node for second arg, if any.
		);
	static bool args_0to3(
		_TCHAR *action,				// Name of current postRFA action.
		Delt<Iarg> *args,			// Argument list for the action.
		Tree<Pn> *collect,		// Tracking for current rule match.
		Sem *sem,					// Semantic object built by actions, if any.
		/*DU*/
		Node<Pn>* &node1,			// Parse tree node for first arg, if any.
		Node<Pn>* &node2,			// Parse tree node for second arg, if any.
		Node<Pn>* &node3			// Parse tree node for third arg, if any.
		);
	static bool args_0to4(										// 11/27/98 AM.
		_TCHAR *action,				// Name of current postRFA action.
		Delt<Iarg> *args,			// Argument list for the action.
		Tree<Pn> *collect,		// Tracking for current rule match.
		Sem *sem,					// Semantic object built by actions, if any.
		/*DU*/
		Node<Pn>* &node1,			// Parse tree node for first arg, if any.
		Node<Pn>* &node2,			// Parse tree node for second arg, if any.
		Node<Pn>* &node3,			// Parse tree node for third arg, if any.
		Node<Pn>* &node4			// Parse tree node for 4th arg, if any.
		);

	static bool args_0to5(_TCHAR*,Delt<Iarg>*,Tree<Pn>*,Sem*,		// 12/19/01 AM.
		Node<Pn>*&,Node<Pn>*&,Node<Pn>*&,Node<Pn>*&,Node<Pn>*&);

	static bool rfbParams(Dlist<Iarg>*,Nlppp*,Dlist<Iarg>*&);	// 03/09/02 AM.

	/////////////////
	//static void fillDefaults(Ielt *elt);

private:
	static bool debug_;					// If debugging output.
	static bool verbose_;				// If verbose output.

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
