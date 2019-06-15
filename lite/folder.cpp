/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	FOLDER.CPP
* FILE:	lite/folder.cpp
* CR:		11/12/00 AM.
* SUBJ:	Folder pass.
*******************************************************************************/

#include "StdAfx.h"
//#include <iostream> // 07/18/03 AM.
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"

//#include "parse.h"
#include "folder.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("folder");

/********************************************
* FN:		Special Functions for Folder class.
********************************************/

Folderx::Folderx()			// Default constructor
	: Algo(algo_name /*, 0 */)
{

}


Folderx::Folderx(const Folderx &orig)			// Copy constructor
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
* CR:		11/12/00 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = NAME of the STUB REGION.
********************************************/

void Folderx::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		11/12/00 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Folderx::dup()
{
Folderx *ptr;
ptr = new Folderx(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}


/********************************************
* FN:		EXECUTE
* CR:		11/12/00 AM.
* SUBJ:	Perform actions for folder pass.
********************************************/

bool Folderx::Execute(Parse *parse, Seqn *seqn)
{
// Folder pass is only executed when called by an NLP++
// function specific for executing a folder.
// Not executed as a "normal" pass in the analyzer sequence.
return true;
}
