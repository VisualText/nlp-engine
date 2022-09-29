/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									PTR_GEN.H
*
* FILE:	consh.�/ptr_gen.h
* SUBJ:	Declarations for ptr table code generation.
* NOTE:	
* CR:	10/5/95 AM.
*
*******************************************************************************/

// 04/26/99 AM. Moving to libconsh includes.
//#define consh_PTR_SEG_SIZE ((long) SEG_SIZE / (long) sizeof(PTR))
//#define consh_PTR_BASE "Ptr"

extern void ptr_gen(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail,		/* Tail for naming generated files.		*/
	CG *cg
	);
extern void ptr_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void ptr_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void
ptr_gen_struct(
	PTR *pseg,
	std::_t_ofstream *fp,
	CG *cg
	);

extern bool
ptr_gen_addr(PTR *ptr,CG *cg, /*UP*/ _TCHAR *buf);
extern bool
ptr_gen_entry(PTR *ptr, CG *cg, /*UP*/ _TCHAR *buf);
