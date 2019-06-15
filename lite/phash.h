/*******************************************************************************
Copyright (c) 2000-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PHASH.H
* FILE:	lite\phash.h
* CR:		05/02/00 AM.
* SUBJ:	Class for interning pass file's parse tree.  W/O HASHING elts&rules.
* NOTE:	Convert the parse tree to executable list of rules.
*
*******************************************************************************/

#ifndef PHASH_H_
#define PHASH_H_

//#include "tree.h"
//#include "lite/dlist.h"
#include "irule.h"
#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	PHASH
* CR:		05/02/00 AM.
* SUBJ:	Class for RFA tree-traversal algorithm.
********************************************/

class Phash : public Algo
{
public:
	Phash();						// Default constructor.
	Phash(const Phash &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
