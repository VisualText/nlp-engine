/*******************************************************************************
Copyright (c) 2000-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	MACHINE.H
* FILE:	include/API/machine.h
* CR:		03/08/00 AM.
* SUBJ:	Machine-specific defines.
* NOTE:	Part of port to Linux.  This file DOES NOT BELONG TO ANY PROJECT
*	OR PROGRAM.  It's meant to be global to all TextAI code.
*
*******************************************************************************/

#ifndef MACHINE_
#define MACHINE_

#include "machine-min.h"	// 11/01/06 AM.

#ifdef _ODBC
#include <sql.h>

#ifdef LINUX
#include <sqlext.h>
#include <sqlucode.h>
// Try without odbc for now... 03/03/16 AM.
//#include <iodbcext.h>
#endif
#endif

#endif

