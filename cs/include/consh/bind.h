/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									BIND.H
*
* FILE:	Conan API/Ext/bind.h
* SUBJ:	Bindings of C variables to knowledge base concepts.
* NOTE:	
* CR:	9/18/95 AM.
*
*******************************************************************************/

/* Name of root of concept hierarchy. */
#define CON_ROOT_NAME	_T("concept")

/* Name of zeroth (empty) concept. */
#define CON_EMPTY_NAME	_T("NIL")

extern bool
bind_all(CG*);
extern LIBCONSH_API bool
bind_app();
extern LIBCONSH_API bool
bind_sys(CG*);
extern LIBCONSH_API bool
unbind_all(CG*);
