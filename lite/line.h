/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	LINE.H
* FILE:	lite\line.h
* CR:		10/11/98 AM.
* SUBJ:	Class for line-gathering pass.
* NOTE:	Separating from tokenization to keep everything simple as can be.
*			This pass may gather character stats for each line.
*
*******************************************************************************/

#ifndef LINE_H_
#define LINE_H_

#include "ln.h"
//#include "tree.h"
#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	LINE
* CR:		10/11/98 AM.
* SUBJ:	Line-gathering class.
********************************************/

class Line : public Algo
{
public:
	Line();			// Default constructor.
	//Line(Parse * = 0);			// Default constructor.
	Line(const Line &orig);	// Copy constructor.   // 12/03/98 AM.

	virtual Algo &dup();					// 12/03/98 AM.
	virtual void setup(_TCHAR *s_data);					// 12/04/98 AM.
	virtual bool Execute(Parse *, Seqn *);

	void Gather(Parse *);	// Perform the line-gathering.
	void FirstLine(Tree<Ln> *, /*DU*/ Node<Pn>* &,
										long &start, Node<Ln>* &last);

	void NextLine(
			Tree<Ln>*,
			/*DU*/
			Node<Pn>* &,
			long &start,
			Node<Ln>* &last
			);

private:
};


#endif
