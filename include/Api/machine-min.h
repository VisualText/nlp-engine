/*******************************************************************************
Copyright (c) 2006-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MACHINE-MIN.H
* FILE:	include/API/machine-min.h
* CR:		11/01/06 AM.
* SUBJ:	Machine-specific defines.
* NOTE:	Part of port to Linux.  This file DOES NOT BELONG TO ANY PROJECT
*	OR PROGRAM.  It's meant to be global to all TextAI code.
*
*******************************************************************************/

#ifndef MACHINE_MIN_
#define MACHINE_MIN_

#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <ctype.h>
#include <string.h>
#include <assert.h>
#ifdef LINUX
#include <sstream>
#else
#include <io.h>	// 11/01/06 AM.
#ifdef UNICODE
#include <sstream>		// 01/24/2005 SRP
#else
#include <strstream>		// 05/25/01 AM.
#endif
#endif

#endif

#include "my_tchar.h"
#include "streamClasses.h"

// CONFIGURATION STUFF.  Could well be moved to a configuration
// file, if desired.		// 05/14/00 AM.
#define SPECDIRNAME	_T("spec")

#ifdef LINUX
#define DIR_CH '/'
#define DIR_STR _T("/")
#else
// WINDOWS
#define DIR_CH '\\'
#define DIR_STR _T("\\")
#endif

// Tolerance for comparing float numeric equality.	// 02/18/07 AM.
#define FLT_DELTA 0.0001
#endif

