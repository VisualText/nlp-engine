/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	GENHASH.H
* FILE:	lite\genhash.h
* CR:		06/13/00 AM.
* SUBJ:	Class for pass to generate code for hash tables.
* NOTE:	RFB pass to dump out hash tables to C++ files.
*
*******************************************************************************/

#ifndef GENHASH_H_
#define GENHASH_H_

//#include "tree.h"
//#include "lite/dlist.h"
#include "irule.h"
#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	GENHASH
* CR:		06/13/00 AM.
* SUBJ:	Class for RFA tree-traversal algorithm.
********************************************/

class Genhash : public Algo
{
public:
	Genhash();						// Default constructor.
	Genhash(const Genhash &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
