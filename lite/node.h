/*******************************************************************************
Copyright (c) 1998-2015 by Text Analysis International, Inc.
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

#ifndef NODE_HI
#define NODE_HI
#ifdef LINUX
#include <sstream>
#else
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif

#include "lite/node.h"


/********************************************
* FN:		NODE (constructor)
* CR:		10/07/98 AM.
********************************************/

template<class NODETYPE>
Node<NODETYPE>::Node(const NODETYPE &d)
{
//*gerr << "[Node: " << (long) this << "]" << std::endl;

data = d;
pUp = pDown = pLeft = pRight = peDown = 0;
#ifndef STABLE_
++count_;
#endif
}


/********************************************
* FN:		~NODE (destructor)
* CR:		10/07/98 AM.
********************************************/

template<class NODETYPE>
Node<NODETYPE>::~Node()
{
//*gerr << "[Node: " << (long) this << "~]" << std::endl;

#ifndef STABLE_
--count_;
#endif
}


/********************************************
* FN:		NODE (copy constructor)
* CR:		11/17/98 AM.
* Note:	Usually a copy constructor will copy everything.
*			But not here.
********************************************/

template<class NODETYPE>
Node<NODETYPE>::Node(Node<NODETYPE> &nn)
{
data = nn.data;
pUp = pRight = pLeft = pDown = 0;		// NONSTANDARD!!! // 11/17/98 AM.

#ifndef STABLE_
++count_;
#endif
}

/********************************************
* FN:		Access functions.
* CR:		10/12/98 AM.
********************************************/
template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::Up()	 { return pUp;    }

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::Down()	 { return pDown;  }

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::Left()	 { return pLeft;  }

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::Right() { return pRight; }

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::eDown() { return peDown; }

/********************************************
* FN:		GETDATA
* CR:		10/07/98 AM.
********************************************/

template<class NODETYPE>
NODETYPE *Node<NODETYPE>::getData()
{
return &data;
}


/********************************************
* FN:		GETCOUNT
* CR:		10/11/98 AM.
********************************************/
#ifndef STABLE_
template<class NODETYPE>
int Node<NODETYPE>::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
template<class NODETYPE>
void Node<NODETYPE>::prettyCount(_TCHAR *str,
	std::_t_ofstream *ofstr)
{
if (count_)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Node<") << str << _T("> count=")
		  << Node<NODETYPE>::count_ << std::ends;
	errOut(&gerrStr,false);
	}

	*gout << _T("Active Node<") << str << _T("> count=")
		  << Node<NODETYPE>::count_ << std::endl;

	if (ofstr)
		*ofstr << _T("Active Node<") << str << _T("> count=")
		  << Node<NODETYPE>::count_ << std::endl;
	}
}
#endif


/********************************************
* FN:		Modify functions.
* CR:		10/28/98 AM.
* WARN:	IF NODES ARE IN A TREE, RECOMMENDED TO USE FUNCTIONS
*			IN Tree.h .  ELSE TREE MAY GET MUNGED.
********************************************/

template<class NODETYPE>
void Node<NODETYPE>::setUp(Node<NODETYPE> *x)
{
pUp = x;
}

template<class NODETYPE>
void Node<NODETYPE>::setDown(Node<NODETYPE> *x)
{
pDown = x;
}

template<class NODETYPE>
void Node<NODETYPE>::setLeft(Node<NODETYPE> *x)
{
pLeft = x;
}

template<class NODETYPE>
void Node<NODETYPE>::setRight(Node<NODETYPE> *x)
{
pRight = x;
}

template<class NODETYPE>
void Node<NODETYPE>::seteDown(Node<NODETYPE> *x)
{
peDown = x;
}

/********************************************
* FN:		DELETENODEANDDATA
* CR:		10/11/98 AM.
********************************************/
template<class NODETYPE>
void Node<NODETYPE>::DeleteNodeAndData(Node<NODETYPE> *nn)
{
if (!nn)
	return;
//*gerr << "DelNode: " << (long) nn << " " << *(nn->getData()) << std::endl;
delete nn;
}

/********************************************
* FN:		DELETENODES
* CR:		11/15/98 AM.
* SUBJ:	Delete a list of nodes.
********************************************/
template<class NODETYPE>
void Node<NODETYPE>::deleteNodes(Node<NODETYPE> *nn)
{
Node<NODETYPE> *tmp;
while (nn)
	{
	tmp = nn;
	nn = nn->Right();
	DeleteNodeAndData(tmp);
	}
}


/********************************************
* FN:		LINKNODES
* CR:		ll/06/98 AM.
* SUBJ:	Attach nodes as parent and child.
********************************************/
template<class NODETYPE>
void Node<NODETYPE>::linkNodes(
	Node<NODETYPE> *parent,
	Node<NODETYPE> *child
	)
{
if (!parent || !child)
	return;
if (parent->pDown || child->pUp)
	{
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[linkNodes: Nodes can't be attached.]") << std::ends;
	errOut(&gerrStr,false);
	}

	return;
	}
parent->pDown = child;
child->pUp = parent;
}

/********************************************
* FN:		LENGTHR
* CR:		12/30/03 AM.
* SUBJ:	Get length of a list of nodes.
********************************************/
template<class NODETYPE>
long Node<NODETYPE>::lengthr(
	Node<NODETYPE> *first,
	Node<NODETYPE> *last
	)
{
long num = 0;
for (; first && (first != last); first = first->Right())
	++num;
if (first != last)	// ERROR, nodes not in same list.
	return -1;
return num;
}


/********************************************
* FN:		LENGTH
* CR:		ll/24/98 AM.
* SUBJ:	Get length of a list of nodes.
********************************************/
template<class NODETYPE>
long Node<NODETYPE>::length()
{
Node<NODETYPE> *list;
long num = 0;
list = this;
for (; list; list = list->Right())
	++num;
return num;
}


/********************************************
* FN:		NTH
* CR:		ll/24/98 AM.
* SUBJ:	Get nth node of a list of nodes.
* NOTE:	Counting starts at 1.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::nth(
	long num
	)
{
Node<NODETYPE> *list;
list = this;
if (num <= 0 || !list)
	return 0;

while (--num)
	{
	if (!(list = list->Right()))
		return 0;
	}
return list;
}


/********************************************
* FN:		LAST
* CR:		05/18/99 AM.
* SUBJ:	Get last node of a list of nodes.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::last()
{
Node<NODETYPE> *list;
list = this;

while (list->Right())
	list = list->Right();
return list;
}


/********************************************
* FN:		PATHNTH
* CR:		11/24/99 AM.
* SUBJ:	Get nth node in the given path.
* RET:	nth node in path, or NULL if error.
* NOTE:	Treats the given node as a "path" down the tree.
*			Gets nth node down that path, with root == 1st.
* ALG:	Recursively traverse to root, going to first node in list,
*			at each level.  Then count down when returning.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::pathNth(int num)
{
int level;
// If I am not the root, then keep going up to root.
// On the return, see if my level corresponds to given num.
return this->pathNth1(num, /*DU*/ level);
}


/********************************************
* FN:		PATHNTH1
* CR:		11/24/99 AM.
* SUBJ:	Get nth node in the given path.
* RET:	nth node in path, or NULL if error.
* NOTE:	Recursive part of pathNth.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::pathNth1(int num, /*DU*/ int &level)
{
// If I am not the root, then keep going up to root.
Node<NODETYPE> *par = this->parent();
Node<NODETYPE> *node;
if (par)
	{
	// Go up rest of path.
	node = par->pathNth1(num, /*DU*/ level);
	if (node)			// Found the appropriate guy.
		return node;	// Return it.
	++level;				// Else record my level.
	}
else							// If I am the root.
	level = 1;				// Root is 1st component of path.
if (level == num)		// If I am the right guy.
	return this;		// Return me.
return 0;				// Haven't gotten it yet.
}


/********************************************
* FN:		PARENT
* CR:		11/24/99 AM.
* SUBJ:	Get parent of given node.
* NOTE:	Hard to believe this hasn't been needed till now.
********************************************/

template<class NODETYPE>
Node<NODETYPE> *Node<NODETYPE>::parent()
{
Node<NODETYPE> *tmp, *node;

// Get head, or first node in list.
node = this;
while ((tmp = node->pLeft))
	node = tmp;

return node->pUp;
}

//////////////////////// END OF FILE ////////////////////////////////
#endif
