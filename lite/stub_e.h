/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STUB_E.H
* FILE:	lite/stub_e.h
* CR:		06/23/99 AM.
* SUBJ:	Class for beginning a stub region of passes.
* NOTE:	Used in rule generation.
*
*******************************************************************************/

#ifndef STUB_E_H_
#define STUB_E_H_

#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	STUB_E
* CR:		06/23/99 AM.
* SUBJ:	Analyzer pass for starting a stub region.
********************************************/

class Stub_e : public Algo
{
public:
	Stub_e();							// Default constructor.
	Stub_e(const Stub_e &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
