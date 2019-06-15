/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ACONSH.H
* FILE:	include/API/consh/Aconsh.h
* CR:		04/26/99 AM.
* SUBJ:	Consh library API includes.
*
*******************************************************************************/

#ifndef ACONSH_
#define ACONSH_

class CG;

#define consh_ST_SEG_SIZE ((long) SEG_SIZE)
#define consh_ST_BASE	_T("St")

#define consh_SYM_SEG_SIZE ((long) SEG_SIZE / (long) sizeof(SYM))
#define consh_SYM_BASE _T("Sym")

#define consh_PTR_SEG_SIZE ((long) SEG_SIZE / (long) sizeof(PTR))
#define consh_PTR_BASE _T("Ptr")

LIBCONSH_API bool																// 05/05/01 AM.
consh_ini(void * = 0);														// 06/11/02 AM.

LIBCONSH_API void
consh_clean(CG*);

#endif
