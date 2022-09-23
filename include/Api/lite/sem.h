/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	SEM.H
* FILE:	include/API/lite/sem.h
* CR:		11/05/98 AM.
* SUBJ:	Class for node's semantic data.
* NOTE:	May be a virtual class.
*
*******************************************************************************/

#ifndef SEM_H_
#define SEM_H_



/********************************************
* CLASS:	SEM
* CR:		11/05/98 AM.
* SUBJ:	Class for node's semantic information.
* NOTE:	The idea is to have derived classes for each domain or application.
********************************************/

class LITE_API Sem
{
public:
	Sem();	// Default constructor.
	virtual ~Sem();

	// Access

	// Modify

	// Virtual function. Defines Sem as an ABSTRACT CLASS.
	// Use this to fill Sem object from parse tree and rule match data.
	virtual void Fill() = 0;

protected:					// Accessible by derived classes.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);			// Pretty-print the count.
private:
	static int count_;						// Count nodes currently allocated.
#endif

};


#endif
