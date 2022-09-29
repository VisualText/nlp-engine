/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NODE.H
* FILE:	include/API/lite/node.h
* CR:		10/07/98 AM.
* SUBJ:	Declares for a tree node.
* NOTE:	A doubly-doubly-linked structure, good for parse trees.
*
*******************************************************************************/

#ifndef NODE_H
#define NODE_H
#ifdef LINUX
#include <sstream>
#else
#include <strstream>															// 05/25/01 AM.
#endif

class Pn;

/********************************************
* CLASS:	NODE
* CR:		10/07/98 AM.
********************************************/

template<class NODETYPE>
class Node
{
// Can't get the right syntax here. Apparently a Visual C++ 5.0 bug.
//	friend class Tree<NODETYPE>;

public:
	Node(const NODETYPE &);				// Constructor
	~Node();									// Destructor
	Node(Node &);							// Copy constructor	// 11/17/98 AM.

	// Access
	NODETYPE *getData();					// Node's data.

	Node *Up();
	Node *Down();
	Node *Left();
	Node *Right();
	Node *eDown();							// 10/28/98 AM.

	// Modify.
	// WARNING! If nodes are within a tree, use functions in tree.h .
	void setUp(Node<NODETYPE> *);
	void setDown(Node<NODETYPE> *);
	void setLeft(Node<NODETYPE> *);
	void setRight(Node<NODETYPE> *);
	void seteDown(Node<NODETYPE> *);

	static void DeleteNodeAndData(Node<NODETYPE> *);
	static void deleteNodes(Node<NODETYPE> *);						// 11/15/98 AM.
	static void linkNodes(Node<NODETYPE> *, Node<NODETYPE> *); // 11/06/98 AM.
	static long lengthr(Node<NODETYPE> *,Node<NODETYPE> *);    // 12/30/03 AM.

	long length();								// 11/24/98 AM.
	Node<NODETYPE> *nth(long);				// 11/24/98 AM.
	Node<NODETYPE> *last();					// 05/18/99 AM.
	Node<NODETYPE> *pathNth(int);			// 11/24/99 AM. !
	Node<NODETYPE> *pathNth1(int, /*DU*/ int &);		// 11/24/99 AM.
	Node<NODETYPE> *parent();								// 11/24/99 AM.

// 10/07/98 AM. WORKAROUND. Making these data elements public till Microsoft
// gets off its butt and enables a template class to be a friend of another
// class.
public:
//private:
	NODETYPE data;							// Data associated with node.

	Node *pUp;								// Node's parent.
	Node *pDown;							// Node's children.
	Node *pLeft;							// Node's left  sibling.
	Node *pRight;							// Node's right sibling.

	////// RULE-TRACKING INFORMATION BELOW HERE /////
	Node *peDown;							// Last subnode covered by current node.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(_TCHAR * = _T(""),std::_t_ofstream* =0);
private:
	static int count_;						// Count nodes currently allocated.
#endif
};



//////////////////////// END OF FILE ////////////////////////////////
#endif
