/*******************************************************************************
Copyright (c) 1999 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	KBM_ALLOC.KBM
* FILE:	include/kbm/kbm_alloc.h
* CR:		08/10/99 AM.
* SUBJ:	Storage allocation for knowledge base management library (libkbm).
* NOTE:	08/10/99 AM. Doesn't look like allocating and freeing can occur in
*			separate libraries.  Each will have to take care of its own space.
*
*******************************************************************************/

extern LIBKBM_API void *kbm_alloc(
	long num,
	long size
	);
