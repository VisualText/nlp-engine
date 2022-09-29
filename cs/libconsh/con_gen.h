/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CON_GEN.H
*
* FILE:	consh.�/con_gen.h
* SUBJ:	Declarations for con table code generation.
* NOTE:	
* CR:	10/6/95 AM.
*
*******************************************************************************/

#define consh_CON_SEG_SIZE ((long) SEG_SIZE / (long) sizeof(CON))
#define consh_CON_BASE _T("Con")

extern void con_gen(
	_TCHAR *dir,		/* Output directory for gend code.		*/
	_TCHAR *tail,		/* Tail for naming generated files.		*/
	CG *cg
	);
extern void con_gen_hdr(
	_TCHAR *dir,			/* Directory to generate code in.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void con_gen_ini(
	_TCHAR *dir,			/* Output directory for gend code.		*/
	_TCHAR *tail,			/* Tail for naming generated files.		*/
	CG *cg
	);
extern void
con_gen_struct(
	CON *pseg,
	std::_t_ofstream *fp,
	CG *cg
	);

extern bool
con_gen_addr(CON *con, CG *cg, /*UP*/ _TCHAR *buf);

extern bool
con_gen_entry(CON *con,CG *cg, /*UP*/ _TCHAR *buf);
