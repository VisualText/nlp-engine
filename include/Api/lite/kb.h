/*******************************************************************************
Copyright (c) 2007-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	KB.H
* FILE:	lite\kb.h
* CR:		02/12/07 AM.
* SUBJ:	Class for interfacing to KBMS.
* NOTE:	Don't create objects of this class.  A function repository only.
*
*******************************************************************************/

#ifndef KB_H_
#define KB_H_
#include <assert.h>

// Forward references

/********************************************
* CLASS:	KB
********************************************/

class KB
{
public:
	KB();				// Default constructor.
	~KB();			// Destructor.

	// Access

	// Modify

	// General

	// Class-wide
//	static _TCHAR *create(long);			// Allocate char array
	static _TCHAR *strVal(CONCEPT*,_TCHAR*,CG*,Htab*);	// 02/12/07 AM.

private:

};


#endif
/*************************      END OF FILE      ******************************/
