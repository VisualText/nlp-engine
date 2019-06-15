/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STUB_S.H
* FILE:	lite/stub_s.h
* CR:		06/23/99 AM.
* SUBJ:	Class for beginning a stub region of passes.
* NOTE:	Used in rule generation.
*
*******************************************************************************/

#ifndef STUB_S_H_
#define STUB_S_H_

#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	STUB_S
* CR:		06/23/99 AM.
* SUBJ:	Analyzer pass for starting a stub region.
********************************************/

class Stub_s : public Algo
{
public:
	Stub_s();							// Default constructor.
	Stub_s(const Stub_s &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
