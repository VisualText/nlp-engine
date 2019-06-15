/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	UALGO.H
* FILE:	lite\ualgo.h
* CR:		02/27/01 AM.
* SUBJ:	Class for interning pass file's parse tree.  W/O HASHING elts&rules.
* NOTE:	Convert the parse tree to executable list of rules.
*
*******************************************************************************/

#ifndef UALGO_H_
#define UALGO_H_

#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	UALGO
* CR:		02/27/01 AM.
* SUBJ:	Class for RFA tree-traversal algorithm.
********************************************/

class Ualgo : public Algo
{
public:
	Ualgo();						// Default constructor.
	Ualgo(const Ualgo &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
