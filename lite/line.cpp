/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	LINE.CPP
* FILE:	lite\line.cpp
* CR:		10/11/98 AM.
* SUBJ:	Line-gathering pass.
* ASS:	Tree has just been tokenized, with no internal nodes.
* NOTE:	Will check and warn user about this.  More general would be to
*			find the lines in any arbitrary parse tree (e.g., by traversing
*			the text buffer itself, since white tokens could be absent from tree.)
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
//#include "dlist.h"					// 07/07/03 AM.
#include "lite/global.h"
#include "lite/inline.h"	// Linux.	// 04/26/07 AM.
#include "io.h"	// Linux.	// 04/26/07 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"		// 02/26/01 AM.
#include "pn.h"				// 02/14/01 AM.

#include "Eana.h"				// 02/01/00 AM.
#include "parse.h"
#include "lite/nlppp.h"			// 07/24/06 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "pat.h"			// For calling prettyPass().				// 04/04/01 AM.
#include "line.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("lines");

/********************************************
* FN:		Special Functions for Line class.
* CR:		10/11/98 AM.
********************************************/

Line::Line()			// Default constructor
	: Algo(algo_name /*, 0 */)
{

}


Line::Line(const Line &orig)			// Copy constructor	// 12/03/98 AM.
{
name = orig.name;
//parse = orig.parse;			// 12/04/98 AM.
debug_ = orig.debug_;
}

/********************************************
* FN:		Access Functions
* CR:		10/11/98 AM.
********************************************/



/********************************************
* FN:		Modify Functions
* CR:		10/11/98 AM.
********************************************/



/********************************************
* FN:		SETUP
* CR:		12/04/98 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Line::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		12/03/98 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Line::dup()
{
Line *ptr;
ptr = new Line(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}


/********************************************
* FN:		EXECUTE
* CR:		10/12/98 AM.
* SUBJ:	Perform the line-gathering.
* ASS:	Tree is a list of tokens.
********************************************/

bool Line::Execute(Parse *parse, Seqn *seqn)
{
Gather(parse);
return true;
}


/********************************************
* FN:		GATHER
* CR:		10/11/98 AM.
* SUBJ:	Perform the line-gathering.
* ASS:	Tree is a list of tokens.
********************************************/

void Line::Gather(Parse *parse)
{
if (parse->Verbose())
	*gout << _T("[Line Execute:]") << std::endl;

_TCHAR *text;
Tree<Pn> *tree;
long len;

text = parse->getText();
tree = (Tree<Pn> *)parse->getTree();
len  = parse->getLength();

if (!tree)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Line Execute: No parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

Node<Pn> *root;
root = tree->getRoot();

if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Line Execute: Empty parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

/* Create tree for lines of text. */
Tree<Ln> *ltree;
ltree = Ln::makeTree(0, len-1, text);		// Create lines tree.
parse->setLines(ltree);							// Update global data.

//////////////////////////////////////////////////////////
// Traverse tokens, creating a line node for every newline.
//////////////////////////////////////////////////////////
// May want to gather statistical data as we go.

long start;				// Count offset of current char.
Node<Ln> *last;		// Current line.
Node<Pn> *node;		// Current parse tree node.

last  = 0;
start = 0;
node  = root->Down();	// First node of token list.

// Build line object for first line of text.
FirstLine(ltree, /*DU*/ node, start, last);

// Continue getting lines and gathering data about them.
while (node)
	NextLine(ltree, /*DU*/ node, start, last);

#ifdef ANCIENT_
// This ancient output could go to a special dump file.			// 04/04/01 AM.
//if (parse->Verbose())											// FIX	// 02/01/00 AM.
if (parse->getEana()->getFlogfiles())						// FIX	// 02/01/00 AM.
	{
	*gout << _T("[Line Execute: before dump.]") << std::endl;
	ltree->Traverse(ltree->getRoot(), *gout);
	}
#endif

// ALL PASSES SHOULD DUMP PARSE TREE AT THE END.					// 04/04/01 AM.
Pat::prettyPass(root,tree,parse);										// 04/04/01 AM.
}


/********************************************
* FN:		FIRSTLINE
* CR:		10/12/98 AM.
* SUBJ:	Set up first line-node.
* NOTE:	We may or may not have the end of first line yet.
********************************************/

void Line::FirstLine(
	Tree<Ln> *ltree,
	/*DU*/
	Node<Pn>* &node,
	long &start,
	Node<Ln>* &last
	)
{
if (!node)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[FirstLine: Empty parse tree.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}

/*
Process the current parse tree node.
If it's a newline, fine.  We have a blank first line.
Else, gather data about that token.
(add to alpha chars, num, white, etc.)
*/
Pn *pn;
enum Pntype typ;
long ostart, oend;
_TCHAR *text;
pn = node->getData();
typ = pn->getType();
ostart = pn->getStart();
oend = pn->getEnd();
text = pn->getText();

/* Attach first line-node. */
last = Ln::makeNode(start, oend, text);
ltree->firstNode(*last);	// Attach first node to tree.

node = node->Right();							// Next node in parse tree.
start = oend + 1;									// Next text offset.

if (!node)
	return;											// Done with pass.

if (typ == PNWHITE && *text == '\n')		// Found newline.
	{
	// First char of buffer was a newline.
	// Create line-node for the next line.
	pn = node->getData();
	text = pn->getText();
	Node<Ln> *lnode;
	lnode = Ln::makeNode(start, start, text);	// Node for next line.
	ltree->insertRight(*lnode, *last);			// Attach to list.
	last = lnode;										// Processing next line now.
	}
}


/********************************************
* FN:		NEXTLINE
* CR:		10/12/98 AM.
* SUBJ:	Gather current line, get start of next line.
********************************************/

void Line::NextLine(
	Tree<Ln>* ltree,
	/*DU*/
	Node<Pn>* &node,
	long &start,
	Node<Ln>* &last
	)
{
/*
Process the current parse tree node.
If it's a newline, fine.  We have a blank first line.
Else, gather data about that token.
(add to alpha chars, num, white, etc.)
*/
Pn *pn;
enum Pntype typ;
long ostart, oend;
_TCHAR *text;
pn = node->getData();
typ = pn->getType();
ostart = pn->getStart();
oend = pn->getEnd();
text = pn->getText();

// Update the current line's end.
Ln *ln;
ln = last->getData();
ln->setEnd(oend);

start = oend + 1;			// Next text offset.
node  = node->Right();	// Next node in parse tree.

if (!node)
	return;					// Done with pass.

if (typ == PNWHITE && *text == '\n')		// Found newline.
	{
	// First char of buffer was a newline.
	// Create line-node for the next line.
	pn = node->getData();
	text = pn->getText();
	Node<Ln> *lnode;
	lnode = Ln::makeNode(start, start, text);	// Node for next line.
	ltree->insertRight(*lnode, *last);	// Attach to list.
	last = lnode;								// Processing next line now.
	}
}

