/*******************************************************************************
Copyright (c) 2000-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DYN.H
* FILE:	libconsh/dyn.h
* CR:		06/29/00 AM.
* SUBJ:	Module dealing with dynamic load of kb.dll library.
*
*******************************************************************************/

#ifdef LINUX
typedef double HINSTANCE;	// 02/19/19 AM.
extern "C" bool kb_setup(void*);	// 02/19/19 AM.
#endif

#ifndef LIBCONSH_DYN_
#define LIBCONSH_DYN_

bool call_kb_setup(
	HINSTANCE hLibrary,
	void *cg
	);

#endif
