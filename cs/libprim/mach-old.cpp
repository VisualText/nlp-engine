/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									MACH.C
*
* FILE:	librim.¹/mach.c
* SUBJ:	Machine-dependent utilities.
* CR:	10/7/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include "prim\libprim.h"

#include <stdio.h>
#include <time.h>
#include "prim\mach.h"


/**************************************************
*						DATE_STR
* FUN:	date_str
* SUBJ:	Get current date-time, nicely formatted.
* CR:	10/7/95 AM.
* RET:	str - formatted date-time.
*
**************************************************/

LIBPRIM_API void
date_str(
	_TCHAR str[],		/* Buffer to fill with date-time. */
	int siz		/* Max size of buffer. */
	)
{
time_t now;
struct tm *date;

/* Get the current calendar time. */
time(&now);

/* Break down time. */
date = localtime(&now);

/* Convert to string. */
_tcsftime(str, siz, _T("%x %H:%M"), date);
}
