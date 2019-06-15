/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	KBM_ALLOC.CPP
* FILE:	libkbm/kbm_alloc.cpp
* CR:		08/10/99 AM.
* SUBJ:	Storage allocation for knowledge base management library (libkbm).
* NOTE:	08/10/99 AM. Doesn't look like allocating and freeing can occur in
*			separate libraries.  Each will have to take care of its own space.
*
*******************************************************************************/

#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include "kbm/libkbm.h"
#include "kbm/kbm_alloc.h"

/**************************************************
*						KBM_ALLOC
* FUN:	kbm_alloc
* SUBJ:	Allocate given block of space.
* CR:		8/10/99 AM.
* NOTE:	Modularity fun.
*
**************************************************/

LIBKBM_API void *kbm_alloc(
	long num,
	long size
	)
{
void *tmp;

tmp = new _TCHAR[num * size];

if (!tmp)
   {
   _ftprintf(stderr, _T("[kbm_alloc: Couldn't alloc %ld units of %ld size.]\n"),
   																			num, size);
   exit(1);
   }

//fprintf(stderr, "[kbm_alloc: Allocd %ld units of %ld size.]\n", num, size);
return(tmp);
}
