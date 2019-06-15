/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									PN.H
*
* FILE:	conan.¹/pn.h
* SUBJ:	Declarations for parse-tree node manager.
* NOTE:	
* CR:	10/28/95 AM.
*
*******************************************************************************/

#define PN_SEGS_MAX	100

/* Not limited to Mac seg size. */
#define PN_SEG_SIZE ((long) 2000)

#define PNNULL	((PN *) NULL)

extern PN		*pn_build();
extern void		 pn_clean();
extern int		 pn_cmp();
extern void		 pn_ini();
extern PN		*pn_make();

extern PN		*pn_last(PN *pn);

extern long		 pn_len();
extern int		 pn_ncmp();
extern void		 pn_free();

extern void		 pn_free_list(PN *pns);

extern void		 pn_pret_bounds();
extern bool		 pn_same_phrase();
extern void		 pn_tree_pret();