/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									SYM_GEN.H
*
* FILE:	consh.�/sym_gen.h
* SUBJ:	Declarations for sym table code generation.
* NOTE:	
* CR:	10/5/95 AM.
*
*******************************************************************************/

// 04/26/99 AM. Moving these to a libconsh include.
//#define consh_SYM_SEG_SIZE ((long) SEG_SIZE / (long) sizeof(SYM))
//#define consh_SYM_BASE "Sym"

extern void sym_gen(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail,		/* Tail for naming generated files.		*/
	CG *cg
	);
extern void sym_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void sym_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void
sym_gen_struct(
	SYM *pseg,
	std::_t_ofstream *fp,
	CG *cg
	);
