/*******************************************************************************
Copyright (c) 1998-2003 by Text Analysis International, Inc.
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

#ifndef TREE_HI
#define TREE_HI
//#include <iostream.h>
#include <assert.h>
#ifdef LINUX
#include <sstream>
#else
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif

//#include "node.h"

// Forward reference.
class Pat;											// OPT.					// 05/16/00 AM.
class Rec;											// OPT.					// 06/01/00 AM.

#include "lite/tree.h"

/********************************************
* FN:		TREE (constructor)
* CR:		10/07/98 AM.
* NOTE:	Not checking that node is a loner.
********************************************/

template<class NODETYPE>
Tree<NODETYPE>::Tree(Node<NODETYPE> *nn)
{
pRoot = pLast = nn;
}


/********************************************
* FN:		Access functions.
* CR:		10/11/98 AM.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::getRoot()
{
return pRoot;
}

// 10/26/98 AM.
template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::getLast()
{
return pLast;
}

/********************************************
* FN:		Modify functions.
* CR:		10/11/98 AM.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::setRoot(Node<NODETYPE> *rr)
{
pRoot = rr;
}

template<class NODETYPE>
void Tree<NODETYPE>::setLast(Node<NODETYPE> *rr)
{
pLast = rr;
}

// OPT // 10/06/99 AM.
template<class NODETYPE>
void Tree<NODETYPE>::resetTree()
{
pRoot = pLast = 0;
}

/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class NODETYPE>
int Tree<NODETYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class NODETYPE>
void Tree<NODETYPE>::prettyCount(_TCHAR *str,
	std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Tree<") << str << _T("> count=")
		  << count_ << std::endl;
	*gout << _T("Active Tree<") << str << _T("> count=")
		  << count_ << std::endl;
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Tree<") << str << _T("> count=")
		  << count_ << std::ends;
	errOut(&gerrStr,false);
	}

	}
}
#endif



/********************************************
* FN:		FIRSTNODE
* CR:		10/09/98 AM.
* SUBJ:	Attach first node (beyond root) to tree.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::firstNode(
	Node<NODETYPE> &node)
{
insertDown(node, *pRoot);
}


/********************************************
* FN:		INSERTUP
* CR:		10/07/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::insertUp(
	Node<NODETYPE> &node,
	Node<NODETYPE> &pos
	)
{
if (node.pUp || node.pLeft)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertUp: Given node already in another tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

if (&pos == pRoot)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertUp: Can't displace root of tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

node.pUp = pos.pUp;
if (pos.pUp)
	pos.pUp->pDown = &node;

pos.pUp = &node;
node.pDown = &pos;
}


/********************************************
* FN:		INSERTDOWN
* CR:		10/07/98 AM.
* NOTE:	Not checking that pos, node in current tree.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::insertDown(
	Node<NODETYPE> &node,
	Node<NODETYPE> &pos
	)
{
if (node.pUp || node.pLeft)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertDown: Given node already in another tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

node.pDown = pos.pDown;

if (pos.pDown)
	pos.pDown->pUp = &node;

pos.pDown = &node;
node.pUp = &pos;
}


/********************************************
* FN:		INSERTLEFT
* CR:		10/07/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::insertLeft(
	Node<NODETYPE> &node,
	Node<NODETYPE> &pos
	)
{
if (node.pUp || node.pLeft)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: Given node already in another tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

if (&pos == pRoot)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertLeft: Can't displace root of tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

if (!pos.pLeft)		// First of a list of siblings.
	{
	node.pUp = pos.pUp;
	if (pos.pUp)
		pos.pUp->pDown = &node;
	pos.pUp = 0;
	}

node.pLeft = pos.pLeft;

if (pos.pLeft)
	pos.pLeft->pRight = &node;

pos.pLeft = &node;
node.pRight = &pos;
}


/********************************************
* FN:		INSERTRIGHT
* CR:		10/07/98 AM.
* NOTE:	Not checking that pos,node in current tree.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::insertRight(
	Node<NODETYPE> &node,
	Node<NODETYPE> &pos
	)
{
if (node.pUp || node.pLeft)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertRight: Given node is in another tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

if (&pos == pRoot)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[InsertRight: Root can't have siblings.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

node.pRight = pos.pRight;

if (pos.pRight)
	pos.pRight->pLeft = &node;

pos.pRight = &node;
node.pLeft = &pos;
}


/********************************************
* FN:		TRAVERSE
* CR:		10/07/98 AM.
* NOTE:	Need indentation.
*			12/09/98 AM. Removing some needless recursion here.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::Traverse(
	const Node<NODETYPE> *node,
	std::_t_ostream &out,						// 12/01/98 AM.
	bool root,
	int indent
	) const
{
if (node == 0)
	return;

for (int ii = indent * INDENT_LEN; ii > 0; ii--)
	out << _T(" ");

out << node->data << std::endl;

Traverse(node->pDown, out, false, indent + 1);

// 12/09/98 AM. Removing recursion here.
//if (!root)
//	Traverse(node->pRight, out, false, indent);
if (!root && !(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		Traverse(node, out, false, indent);
		}
	}
}

#ifdef DAVE_
/********************************************
* FN:		TRAVERSESETFIREDFLAG
* CR:		05/21/01 Dd.
* NOTE:	Sets the fired flag in the entire tree
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::TraverseSetFiredFlag(
	const Node<NODETYPE> *node,
	bool value
	) const
{
if (node == 0)
	return;

TraverseSetFiredFlag(node->pDown, value);
Pn *pn = ((Node<Pn> *)node)->getData();
pn->setFired(value);

if (!(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		TraverseSetFiredFlag(node, value);
		}
	}
}
#endif


/********************************************
* FN:		DUMP
* CR:		10/08/98 AM.
* NOTE:	Traverse the entire tree.  OMITS THE ROOT.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::Dump() const
{
if (!pRoot)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Dump: No root for given tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

Traverse(pRoot->pDown, *gout, false, 0);
}


/********************************************
* FN:		DELETETREEANDDATA
* CR:		10/11/98 AM.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::DeleteTreeAndData(Tree<NODETYPE> *tt)
{
if (!tt) return;
tt->deleteSubsNC(tt->pRoot);
tt->pRoot = 0;
delete tt;
}


/********************************************
* FN:		DELETESUBSNC
* CR:		10/11/98 AM.
* NOTE:	Recursive.  Nonchecking.
*			11/19/98 AM. Need to check if given node is the root or the first in
*			a list.  Renamed from DelTreeAndData.
*			12/09/98 AM. Removing some needless recursion, to handle huge inputs.
* ASS:	Caller has detached the given list of subtrees or is wiping out
*			the whole tree.  (Else, user should call deleteSubs.)
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::deleteSubsNC(Node<NODETYPE> *nn)
{
if (!nn)
	return;
//deleteSubsNC(nn->pRight);	// 12/09/98 AM.
if (!(nn->pLeft))			// If first in list, handle the list. // 12/09/98 AM.
	{
	// Be careful.  Must not use the one being deleted.
	Node<NODETYPE> *tmp, *next;
	tmp = nn->pRight;
	while (tmp)
		{
		next = tmp->pRight;		// Save one past the node to delete.
		deleteSubsNC(tmp);
		tmp = next;
		}
	}
deleteSubsNC(nn->pDown);
Node<NODETYPE>::DeleteNodeAndData(nn);
}


/********************************************
* FN:		DELETESUBS
* CR:		11/19/98 AM.
* NOTE:	Delete a list of subtrees.
*			Doing extra checking because the subtrees may or may not be
*			detached from their tree.
*			deletenodes deletechilds rmnodes rmchilds  (possible lookup names).
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::deleteSubs(Node<NODETYPE> *start)
{
if (!start)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[deleteSubs: Given null list of nodes.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

// Check on the start node.
if (start == pRoot)			// Start is root.
	pRoot = 0;					// Wiping out the whole tree.
else if (start->pUp)	// Start is first in a tree list.
	start->pUp->pDown = 0;			// Detach from tree.
else if (start->pLeft)				// Start has node to the left.
	start->pLeft->pRight = 0;
start->pLeft = 0;					// Detach from tree, if attached.
start->pUp = 0;

deleteSubsNC(start);			// Now traverse and delete it all.
}


/********************************************
* FN:		EXCISESUBS
* CR:		11/04/98 AM.
* SUBJ:	Remove a range of subtrees from tree.
* ALLOC:	Caller must take care of the excised nodes.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::exciseSubs(
	Node<NODETYPE> *start,	// Being removed.
	Node<NODETYPE> *end
	)
{
Tree<NODETYPE> *tree;
tree = this;						// Just for clarity.

if (!start)							// 11/26/98 AM. Allow null end of range.
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[exciseSubs: Given null node.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}

Node<NODETYPE> *tmp;

// 11/26/98 AM. If null end of range, handle it separately.
if (!end)
	{
	if (start == tree->pRoot)	// Chuck the whole tree.
		tree->setRoot(0);
	else if (!start->pLeft)		// Replacing entire list.
		{
		start->pUp->pDown = 0;
		start->pUp = 0;
		}
	else							// Normal tail segment of a list.
		{
		start->pLeft->pRight = 0;
		start->pLeft = 0;
		}
	return;
	}

if (start == tree->pRoot)		// Replacing nodes starting at root.
	{
	tree->setRoot(end->pRight);			// Reset tree's root.
	if (end->pRight)
		end->pRight->pLeft = 0;
	end->pRight = 0;
	}
else if (!start->pLeft)			// Replacing first node in phrase.
	{
	if (!(start->pUp))
		{
		{
	std::_t_strstream gerrStr;
	gerrStr << _T("[exciseSubs: Node not in a tree.]") << std::ends;
	errOut(&gerrStr,false);
	}

		return;
		}
	if ((tmp = end->pRight))
		{
		end->pRight = 0;
		tmp->pLeft = 0;
		tmp->pUp = start->pUp;
		}
	start->pUp->pDown = tmp;		// Whether tmp is null or not.
	start->pUp = 0;
	}
else										// Normal node.
	{
	start->pLeft->pRight = end->pRight;
	if (end->pRight)
		end->pRight->pLeft = start->pLeft;
	start->pLeft = 0;
	end->pRight = 0;
	}
}


/********************************************
* FN:		SPLICESUBS
* CR:		07/30/99 AM.
* SUBJ:	Zap a range of nodes from tree and splice in their children.
* NOTE:	A way to undo rule reductions.  This fn deletes the top level
*			range of nodes.
* ALG:	Fix up left edge conditions.  Traverse the list. Fix up right
*			edge conditions. MUST HANDLE CASES WHERE NODES HAVE NO CHILDREN.
*			If a node has no children, its a "simple" excise.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::spliceSubs(
	Node<NODETYPE> *start,	// Being removed.
	Node<NODETYPE> *end,
	Node<NODETYPE> * &newstart,	// First node in spliced list	// 07/19/02 AM.
	Node<NODETYPE> * &newend		// Last node in spliced list	// 07/19/02 AM.
	)
{
newstart = newend = 0;														// 07/19/02 AM.
Tree<NODETYPE> *tree;
tree = this;						// Just for clarity.

if (!start)							// 11/26/98 AM. Allow null end of range.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[spliceSubs: Given null node.]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}

bool first = true;				// If need to reattach childs.

Node<NODETYPE> *childs = 0;

childs = start->pDown;

// Get last node of children.  Then attach it to the 
// next set of children, and so on.
Node<NODETYPE> *echilds = 0;		// Last node of chilren.
if (childs)
	newend = echilds = childs->last();

Node<NODETYPE> *next;			// Next node in list.
next = start->pRight;

// DETACH START NODE FROM TREE.
// IF CHILDS AROUND, REATTACH THEM.
// Handle first node specially.  Maybe root, first in list, etc.

if (start == tree->pRoot)	// Chuck the root of the tree.
	{
	// Displace root with its children (if any so far)!
	// Note that there can be a LIST of "root" nodes.
	tree->setRoot(childs);
	if (childs)
		{
		childs->pUp = 0;			// Becomes new root.
		newstart = childs;													// 07/19/02 AM.
		first = false;
		}
	}
else if (!start->pLeft)		// First in list.
	{
	// Remove first specially.
	start->pUp->pDown = childs;	// Splice in start's children.
	if (childs)
		{
		childs->pUp = start->pUp;
		newstart = childs;													// 07/19/02 AM.
		first = false;
		}
	}
else						// Normal tail segment of a list.
	{
	start->pLeft->pRight = childs;
	if (childs)
		{
		childs->pLeft = start->pLeft;
		childs->pUp = 0;	// No longer has a parent (directly speaking).
		newstart = childs;													// 07/19/02 AM.
		first = false;
		}
	else
		echilds = start->pLeft;		// PLACE TO ATTACH NODES TO.
	}

// Set a convenient stopping point (one past end node, else null).
Node<NODETYPE> *stop = 0;
if (end)
	stop = end->pRight;

// Remove the rest of the list normally.
// If reattach pending, do that also.
Node<NODETYPE> *node;
for (;;)
	{
	if (next == stop)
		break;
	if (!(node = next))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[spliceSubs: Error]") << std::ends;
		errOut(&gerrStr,false);
		break;
		}
	next = node->pRight;
	childs = node->pDown;
	Node<NODETYPE>::DeleteNodeAndData(node);
	if (childs)		// Attach these to prior nodes.
		{
		if (first)	// REATTACH TO TREE.
			{
			if (start == tree->pRoot)		// Make new root.
				{
				tree->setRoot(childs);
				childs->pUp = 0;
				}
			else if (!start->pLeft)			// First in list.
				{
				start->pUp->pDown = childs;
				childs->pUp = start->pUp;
				}
			else
				{
				childs->pUp = 0;
				childs->pLeft = echilds;
				echilds->pRight = childs;
				}
			newstart = childs;												// 07/19/02 AM.
			first = false;		// Finally reattached to tree.
			}
		else						// Extending the spliced list.
			{
			childs->pUp = 0;
			// Link the two child lists.
			childs->pLeft = echilds;
			if (echilds)
				echilds->pRight = childs;
			}

		newend = echilds = childs->last();
		}
	}

// IF STILL TRYING TO REATTACH!
if (first && stop)
	{
	// newstart = newend = 0;												// 07/19/02 AM.
	if (start == tree->pRoot)		// Attach new root
		{
		tree->setRoot(stop);
		stop->pUp = stop->pLeft = 0;
		}
	else if (!start->pLeft)			// Attach first in list.
		{
		stop->pLeft = 0;
		stop->pUp = start->pUp;
		start->pUp->pDown = stop;
		}
	else									// Normal list attachment.
		{
		stop->pLeft = echilds;
		if (echilds)
			echilds->pRight = stop;
		}
	}
else if (stop)		// There's a node past the spliced nodes.
	{
	// Link it up with tail end of range.
	stop->pLeft = echilds;
	if (echilds)
		echilds->pRight = stop;
	}

// One of the top-level nodes to zap.
Node<NODETYPE>::DeleteNodeAndData(start);

}


/********************************************
* FN:		REPLACESUBS
* CR:		10/28/98 AM.
* SUBJ:	Replace a range of subtrees with a range of subtrees.
* NOTE:	Can replace a single subtree, or with a single.
*			May want to allow endpoints to be null, for convenience.
*			(Not implemented.)
*			Each node keeps its children.
* ALLOC:	Caller must take care of the excised nodes.
********************************************/

template<class NODETYPE>
void Tree<NODETYPE>::replaceSubs(
	Node<NODETYPE> *x_start,	// Being removed.
	Node<NODETYPE> *x_end,
	Node<NODETYPE> *y_start,	// Being added.
	Node<NODETYPE> *y_end
	)
{
Tree<NODETYPE> *tree;
tree = this;						// Just for clarity.

if (!x_start || !y_start || !x_end || !y_end)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[replaceSubs: Given null node.]") << std::ends;
	errOut(&gerrStr,false);

	return;
	}

// Replace right side.
y_end->pRight = x_end->pRight;
if (x_end->pRight)
	x_end->pRight->pLeft = y_end;
x_end->pRight = 0;

if (x_start == tree->pRoot)		// Replacing nodes starting at root.
	tree->setRoot(y_start);			// Reset tree's root.
else if (!x_start->pLeft)			// Replacing first node in phrase.
	{
	// Replace attachment to parent.
	y_start->pUp = x_start->pUp;
	x_start->pUp->pDown = y_start;
	x_start->pUp = 0;
	}
else										// Normal node.
	{
	// Replace left side.
	y_start->pLeft = x_start->pLeft;
	if (x_start->pLeft)
		x_start->pLeft->pRight = y_start;
	x_start->pLeft = 0;
	}
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// FOR TREATING A TREE AS A LIST OF NODES (ROOTS).
// 10/26/98 AM.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/********************************************
* FN:		PUSH
* CR:		10/26/98 AM.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::push(Node<NODETYPE> *nn)
{
if (!nn) return nn;
if (!pRoot)		// If tree is empty.
	{
	pRoot = pLast = nn;
	}
else
	{
	nn->pRight = pRoot;
	pRoot->pLeft = nn;
	pRoot = nn;				// New root
	}
return nn;
}

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::push(NODETYPE *data)
{
if (!data) return 0;
Node<NODETYPE> *nn;
nn = new Node<NODETYPE>(*data);
return push(nn);
}

/********************************************
* FN:		RPUSH
* CR:		10/26/98 AM.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::rpush(Node<NODETYPE> *nn)
{
if (pLast)	// List not empty.
	{
	pLast->pRight = nn;
	nn->pLeft = pLast;
	pLast = nn;
	}
else			// List empty.
	{
	pRoot = pLast = nn;
	}
return nn;
}

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::rpush(NODETYPE *data)
{
if (!data)
	return 0;
Node<NODETYPE> *nn;
nn = new Node<NODETYPE>(*data);
return rpush(nn);
}

/********************************************
* FN:		POP
* CR:		10/26/98 AM.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::pop()
{
if (!pRoot)
	return 0;

Node<NODETYPE> *nn;
nn = pRoot;
pRoot = pRoot->pRight;
if (!pRoot)
	{
	pLast = 0;
	}
else
	{
	nn->pRight = 0;
	pRoot->pLeft = 0;
	}
return nn;
}

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::rpop()
{
if (!pLast)
	return 0;
Node<NODETYPE> *nn;
nn = pLast;
pLast = pLast->pLeft;
if (!pLast)
	pRoot = 0;
else
	{
	nn->pLeft = 0;
	pLast->pRight = 0;
	}
return nn;
}


/********************************************
* FN:		RIGHTMOSTNODE
* CR:		11/25/98 AM.
* NOTE:	01/31/99 AM. Traverse to leftmost node to get parent. (BUG FIX).
********************************************/

template<class NODETYPE>
bool Tree<NODETYPE>::rightmostNode(Node<NODETYPE> *node)
{
if (!node)
	return false;

// Traversing to leftmost node to get parent.
Node<NODETYPE> *tmp;
for (;;)	// While looking up the tree.
	{
	if (node->pRight)
		return false;

	// Go to first node in phrase. (leftmost node in phrase)
	while ((tmp = node->pLeft))
		node = tmp;

	if (!(node = node->pUp))
		return true;
	}
return true;
}


/********************************************
* FN:		LEAFNODE
* CR:		01/30/99 AM.
* SUBJ:	Get the leafiest node below given node.
* RET:	Leafiest node before branching.
* NOTE:	Taken from Pat::leafNode.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Tree<NODETYPE>::leafNode(Node<NODETYPE> *node)
{
if (!node)
	return 0;
Node<NODETYPE> *leaf;									// FIX.			// 03/06/00 AM.
leaf = node;
while ((node = node->Down()) && !(node->Right()))
	leaf = node;
return leaf;
}

#endif
//////////////////////// END OF FILE ////////////////////////////////
