/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ISTR.CPP
* FILE:	lite\istr.cpp
* CR:		10/24/98 AM.
* SUBJ:	Abstract class for rule-action's string arguments.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"

#include "istr.h"

/********************************************
* FN:		Special Functions for class.
* CR:		10/24/98 AM.
********************************************/

Istr::Istr(			// Constructor.
	_TCHAR *ss			// String data.
	)
	: Iarg(IASTR)	// Init with string type.
{
str = ss;
}

Istr::~Istr()
{
}


/********************************************
* FN:		Access functions
* CR:		10/24/98 AM.
********************************************/

_TCHAR *Istr::getData()	{ return str; }


/********************************************
* FN:		Modify functions
* CR:		10/24/98 AM.
********************************************/

void Istr::setData(_TCHAR *x)	{ str = x; }

