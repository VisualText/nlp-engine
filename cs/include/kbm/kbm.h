/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									KBM.H
*
* FILE:	ext.¹/kbm.h
* SUBJ:	Declarations for kb manager.
* CR:		9/26/95 AM.
*
*******************************************************************************/


/* Kbm library initialization file for Conan applications and Consh. */
#ifdef OLD_020815_
extern LIBKBM_API void		 kbm_clean();
extern LIBKBM_API bool		 kbm_ini();									// 05/05/01 AM.

LIBKBM_API void
kbm_vars(
	CON *cg_concept,
	CON *cg_phrase,			// 04/16/99 AM.
	CON *cg_atom,				// 05/04/99 AM.
	CON *dict_alpha,
	CON *dict_unknown,
//	CON *id_action,
//	CON *id_rest,
	CON *nlp_punct,
	CON *nlp_wht
	);
#endif
