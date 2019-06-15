/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									SM.C
*
* FILE:	conan.¹/sm.c
* SUBJ:	Space management module.
* NOTE:	08/10/99 AM. Doesn't look like allocating and freeing can occur in
*			separate libraries.  Each will have to take care of its own space.
* CR:	5/11/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include "prim/libprim.h"

#include <stdio.h>
#include <stdlib.h>
#include "prim/sm.h"

/**************************************************
*						SM_ALLOC
* FUN:	sm_alloc
* SUBJ:	Allocate given block of space.
* CR:	5/11/95 AM.
* NOTE:	Modularity fun.
*
**************************************************/

LIBPRIM_API void *sm_alloc(
	long num,
	long size
	)
{
void *tmp;

// 04/15/99 AM. Converting C-style allocation to C++.
//tmp = calloc(num, size);
tmp = new _TCHAR[num * size];		// 04/15/99 AM.

if (!tmp)
   {
   _ftprintf(stderr, _T("[sm_alloc: Couldn't alloc %ld units of %ld size.]\n"),
   																			num, size);
   exit(1);
   }

//fprintf(stderr, "[sm_alloc: Allocd %ld units of %ld size.]\n", num, size);
return(tmp);
}


/**************************************************
*						SM_ZERO
* FUN:	sm_zero
* SUBJ:	Zero out fields of a structure.
* CR:		06/09/99 AM.
* NOTE:	To enable freeing and recreation of knowledge bases!
* OPT:	02/15/01 AM. Using memset fn to optimize.
*
**************************************************/

LIBPRIM_API void sm_zero(
	_TCHAR *ptr,	// Ptr to struct.
	long size	// Size in bytes.
	)
{
if (!ptr || size <= 0)														// 02/15/01 AM.
	return;																		// 02/15/01 AM.

#ifndef LINUX
memset(ptr, '\0', size);													// 02/15/01 AM.

#else
while (--size >= 0)															// 02/15/01 AM.
 	*ptr++ = '\0';																// 02/15/01 AM.
#endif
}
