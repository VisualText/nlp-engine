/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	NINTERN.H
* FILE:	lite\nintern.h
* CR:		05/01/00 AM.
* SUBJ:	Class for interning pass file's parse tree.  W/O HASHING elts&rules.
* NOTE:	Convert the parse tree to executable list of rules.
*
*******************************************************************************/

#ifndef NINTERN_H_
#define NINTERN_H_

//#include "tree.h"
//#include "lite/dlist.h"
#include "irule.h"
#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	NINTERN
* CR:		05/01/00 AM.
* SUBJ:	Class for RFA tree-traversal algorithm.
********************************************/

class Nintern : public Algo
{
public:
	Nintern();						// Default constructor.
	Nintern(const Nintern &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
