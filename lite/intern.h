/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	INTERN.H
* FILE:	lite\intern.h
* CR:		11/05/98 AM.
* SUBJ:	Class for internalizing the RFA parse tree.
* NOTE:	Convert the parse tree to executable list of rules.
*			This is a final pass of the RFA.
*
*******************************************************************************/

#ifndef INTERN_H_
#define INTERN_H_

//#include "tree.h"
//#include "lite/dlist.h"
#include "irule.h"
#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	INTERN
* CR:		11/05/98 AM.
* SUBJ:	Class for RFA tree-traversal algorithm.
********************************************/

class Intern : public Algo
{
public:
	Intern();						// Default constructor.
	Intern(const Intern &orig);	// Copy constructor.   // 12/03/98 AM.

	virtual Algo &dup();					// 12/03/98 AM.
	virtual void setup(_TCHAR *s_data);					// 12/04/98 AM.
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
