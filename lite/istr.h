/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ISTR.H
* FILE:	lite\istr.h
* CR:		10/24/98 AM.
* SUBJ:	Class representing a generic rule-action's string argument.
* NOTE:	Derived from Iarg.
*
*******************************************************************************/

#ifndef ISTR_H_
#define ISTR_H_

#include "iarg.h"

/********************************************
* CLASS:	ISTR
* CR:		10/24/98 AM.
* SUBJ:	Class for rule-action's string arguments.
********************************************/

class Istr : public Iarg
{
public:
	Istr(_TCHAR * = 0);						// Default constructor.
	virtual ~Istr();						// Destructor.

	// Access.
	_TCHAR *getData();						// Return STRING data.

	// Modify
	void setData(_TCHAR *);				// Set string arg.

protected:					// Accessible by derived classes.
	_TCHAR *str;								// String arg data.
};


#endif
