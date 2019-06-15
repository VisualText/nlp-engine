/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	FOLDER.H
* FILE:	lite/folder.h
* CR:		11/12/00 AM.
* SUBJ:	Class for folder pass.
*
*******************************************************************************/

#ifndef FOLDER_H_
#define FOLDER_H_

#include "lite/algo.h"

class Seqn;	// Forward reference.

/********************************************
* CLASS:	FOLDER
* CR:		11/12/00 AM.
* SUBJ:	Analyzer pass for folder pass.
********************************************/

class Folderx : public Algo
{
public:
	Folderx();							// Default constructor.
	Folderx(const Folderx &orig);	// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
};


#endif
