/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ANGEN.H
* FILE:	lite/angen.h
* CR:		04/17/00 AM.
* SUBJ:	Class for generating code from parse tree.
*
*******************************************************************************/

#ifndef ANGEN_H_
#define ANGEN_H_

//#include "tree.h"
//#include "lite/dlist.h"
#include "irule.h"
#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	ANGEN
* CR:		04/17/00 AM.
* SUBJ:	Class for code generation algorithm.
********************************************/

class Angen : public Algo
{
public:
	Angen();						// Default constructor.
	Angen(const Angen &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
