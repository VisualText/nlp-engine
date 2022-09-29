/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	UALGO.CPP
* FILE:	lite/ualgo.cpp
* CR:		02/27/01 AM.
* SUBJ:	Pass for invoking user-defined pass algorithm.
* NOTE:	Handshake to the user project for current analyzer.  Assumes
*			that pass names not in the core will be defined in the user project.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.

// May need an API for user-algorithm.
// #include "lite/Aalgo.h"

#include "inline.h"
#include "node.h"	// 07/07/03 AM.
#include "parse.h"
#include "seqn.h"
#include "dyn.h"
#include "uAlgo.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("ualgo");

/********************************************
* FN:		Special Functions for class.
* CR:		02/27/01 AM.
********************************************/

Ualgo::Ualgo()			// Default constructor
	: Algo(algo_name /*, 0*/)
{

}


Ualgo::Ualgo(const Ualgo &orig)			// Copy constructor
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
* CR:		02/27/01 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Ualgo::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		02/27/01 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Ualgo::dup()
{
Ualgo *ptr;
ptr = new Ualgo(*this);					// Copy constructor.
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}

/********************************************
* FN:		EXECUTE
* CR:		02/27/01 AM.
* SUBJ:	Invoke pass algorithm from user project.
* NOTE:
********************************************/

bool Ualgo::Execute(Parse *parse, Seqn *seqn)
{
if (parse->Verbose())
	*gout << _T("[Ualgo:]") << std::endl;

////////////////////////////
// SET UP CALL TO USER PASS.
////////////////////////////

// NEED TO XFER THE NAME OF THE ALGO.
_TCHAR *name = seqn->getAlgoname();
#ifndef LINUX
HINSTANCE hdll = parse->getHdll();
if (hdll && call_ucodeAlgo(hdll, parse, name
		// ,&glob_Auser
	))
	return true;
#endif
return true;	// 01/28/02 AM.
}


/******************************************************************************/
