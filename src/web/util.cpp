/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	UTIL.CPP
* FILE:	web\util.cpp
* CR:		03/11/99 AM.
* SUBJ:	Primitives for this library.
* NOTE:	Should probably have a UTIL.DLL for sharing.
*			Functions here are redundant with stuff in lite.dll, etc.
*
*******************************************************************************/

#include "stdafx.h"
#include "web.h"

#include <iostream>											// Upgrade.	// 01/24/01 AM.
#include <fstream>											// Upgrade.	// 01/24/01 AM.
#include <stdlib.h>

#include "util.h"


/********************************************
*
* FN:		FILE_EXISTS
* CR:		12/14/98 AM.
* SUBJ:	Check if file exists.
********************************************/

bool file_exists(const _TCHAR *iname)
{

//ifstream inFile(iname, ios::in | ios::nocreate);
_t_ifstream inFile(CTCHAR2CA(iname), ios::in);							// Upgrade.	// 01/24/01 AM.
return (inFile ? true : false);
}
