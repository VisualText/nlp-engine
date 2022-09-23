/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	LN.H
* FILE:	lite\ln.h
* CR:		10/11/98 AM.
* SUBJ:	Declares for line tree node's data.
* NOTE:	This class will know about nodes and trees.
*
*******************************************************************************/

#ifndef LN_H_
#define LN_H_
//#include <iostream.h>
#include <assert.h>

#include "tree.h"

/********************************************
* CLASS:	LN
* CR:		10/11/98 AM.
* SUBJ:	Line tree node's data.
********************************************/

class Ln
{
	friend std::_t_ostream &operator<<(std::_t_ostream &, const Ln &);

public:
	Ln();
	Ln(long, long, _TCHAR *);
	~Ln();
	long getStart();
	long getEnd();
	_TCHAR *getText();

	void setStart(long);
	void setEnd(long);
	void setStartEnd(long, long);
	void setText(_TCHAR *);

	static Tree<Ln> *makeTree(
	long ostart,
	long oend,
	_TCHAR *
	);

	
	static Node<Ln> *makeNode(long start, long end, _TCHAR *);

private:
	long Start;				// Start offset of node in text.
	long End;				// End offset of node in text.
	_TCHAR *Text;				// Ptr to start of node's text in parse buffer.

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

