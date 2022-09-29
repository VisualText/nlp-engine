/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	TREE.H
* FILE:	include/API/lite/tree.h
* CR:		10/07/98 AM.
* SUBJ:	Declares for a tree node.
* NOTE:	A doubly-doubly-linked structure, good for parse trees.
*			First child of list of siblings is special, in that it has no
*			left sibling and is attached to a parent.  Root of the tree is
*			special (e.g., cannot get parent or sibling; only children).
*			10/26/98 AM. Change Tree to allow lists of nodes at the top level.
*			ie, "lists of roots".  Also, add a field to point to the last node
*			in such a list, or the "last root".  This will make lists of nodes
*			easier to manage.  Also, push, rpush, pop, rpop for the list of roots.
*
*******************************************************************************/

#ifndef TREE_H
#define TREE_H
//#include <iostream.h>
#include <assert.h>
#ifdef LINUX
#include <sstream>
#else
#include <strstream>															// 05/25/01 AM.
#endif

//#include "node.h"

// Forward reference.
class Pat;											// OPT.					// 05/16/00 AM.
class Rec;											// OPT.					// 06/01/00 AM.

/********************************************
* CLASS:	TREE
* CR:		10/07/98 AM.
********************************************/

template<class NODETYPE>
class Tree
{
	friend class Pat;										// OPT.					// 05/16/00 AM.
	friend class Rec;										// OPT.					// 06/01/00 AM.
public:
	Tree(Node<NODETYPE> *root = 0);			// Default constructor.

	// Access
	Node<NODETYPE> *getRoot();
	Node<NODETYPE> *getLast();

	// Modify
	void setRoot(Node<NODETYPE> *);
	void setLast(Node<NODETYPE> *);
	void resetTree();								// OPT // 10/06/99 AM.

	void insertUp( Node<NODETYPE> &node, Node<NODETYPE> &pos);
	void insertDown( Node<NODETYPE> &node, Node<NODETYPE> &pos);
	void insertLeft( Node<NODETYPE> &node, Node<NODETYPE> &pos);
	void insertRight( Node<NODETYPE> &node, Node<NODETYPE> &pos);

	// 10/28/98 AM.
	void replaceSubs(
		Node<NODETYPE> *x_start,	// Being removed.
		Node<NODETYPE> *x_end,
		Node<NODETYPE> *y_start,	// Being added.
		Node<NODETYPE> *y_end
		);
	// 11/04/98 AM.
	void exciseSubs(
		Node<NODETYPE> *start,	// Being removed.
		Node<NODETYPE> *end
		);
	void spliceSubs(				// 07/30/99 AM
		Node<NODETYPE> *start,
		Node<NODETYPE> *end,
		Node<NODETYPE> * &newstart,										// 07/19/02 AM.
		Node<NODETYPE> * &newend											// 07/19/02 AM.
		);

	// For list of nodes ("roots").
	Node<NODETYPE> *push(Node<NODETYPE> *);
	Node<NODETYPE> *push(NODETYPE *);
	Node<NODETYPE> *rpush(Node<NODETYPE> *);
	Node<NODETYPE> *rpush(NODETYPE *);
	Node<NODETYPE> *pop();
	Node<NODETYPE> *rpop();

	void Traverse(	const Node<NODETYPE> *,
						std::_t_ostream &,						// 12/01/98 AM.
						bool root = true,
						int indent = 0) const;
	void TraverseSetFiredFlag(	const Node<NODETYPE> *,				// 05/21/01 Dd.
								bool value = false) const;
	void Dump() const;			// Traverse the entire tree.
	//void postTraverse() const;

	void firstNode(Node<NODETYPE> &node);
	void deleteSubsNC(Node<NODETYPE> *);
	void deleteSubs(Node<NODETYPE> *start);

	static void DeleteTreeAndData(Tree<NODETYPE> *);
	static bool rightmostNode(Node<NODETYPE> *);		// 11/25/98 AM.
	static Node<NODETYPE> *leafNode(Node<NODETYPE> *node);	// 01/10/00 AM.

private:
	Node<NODETYPE> *pRoot;
	Node<NODETYPE> *pLast;	// The node in a list of "roots".
	enum {INDENT_LEN = 3};	// For Traversing and pretty printing tree.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_TCHAR * = _T(""),
							std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif
};


#endif
//////////////////////// END OF FILE ////////////////////////////////
