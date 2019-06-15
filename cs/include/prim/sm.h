/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									SM.H
*
* FILE:	conch.¹/sm.h
* SUBJ:	Declarations for space manager.
* NOTE:	
* CR:	5/11/95 AM.
*
*******************************************************************************/

extern LIBPRIM_API void *sm_alloc(
	long num,
	long size
	);
extern LIBPRIM_API void sm_zero(		// 06/09/99 AM.
	_TCHAR *ptr,	// Ptr to struct.
	long size	// Size in bytes.
	);
