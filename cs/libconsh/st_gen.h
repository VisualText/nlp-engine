/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									ST_GEN.H
*
* FILE:	conan.¹/st.h
* SUBJ:	Declarations for string table manager.
* NOTE:	
* CR:	5/10/95 AM.
*		9/25/95 AM. Moved from st.h .
*
*******************************************************************************/

// 04/26/99 AM. Moving to libconsh includes.
//#define consh_ST_SEG_SIZE ((long) SEG_SIZE)
//#define consh_ST_BASE	"St"

extern void st_gen(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail,		/* Tail for naming generated files.		*/
	CG *cg
	);
extern void st_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void st_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
