/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	STUB_S.CPP
* FILE:	lite/stub_s.cpp
* CR:		06/23/99 AM.
* SUBJ:	Stub start pass, beginning a region of generated passes.
* ASS:	
* NOTE:	
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.

#include "node.h"				// 07/07/03 AM.
#include "parse.h"
#include "stub_s.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("stub");

/********************************************
* FN:		Special Functions for Stub_s class.
********************************************/

Stub_s::Stub_s()			// Default constructor
	: Algo(algo_name /*, 0 */)
{

}


Stub_s::Stub_s(const Stub_s &orig)			// Copy constructor
{
name = orig.name;
debug_ = orig.debug_;
}

/********************************************
* FN:		Access Functions
********************************************/



/********************************************
* FN:		Modify Functions
********************************************/



/********************************************
* FN:		SETUP
* CR:		06/23/99 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = NAME of the STUB REGION.
********************************************/

void Stub_s::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		06/23/99 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Stub_s::dup()
{
Stub_s *ptr;
ptr = new Stub_s(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}


/********************************************
* FN:		EXECUTE
* CR:		06/23/99 AM.
* SUBJ:	Perform actions for stub region start.
********************************************/

bool Stub_s::Execute(Parse *parse, Seqn *seqn)
{
// This pass is a placeholder in the analyzer sequence.
// No analysis action is performed, though some kind of flag
// could be set, so errors that occur in the generated
// region can be reported more intelligently.
return true;
}
