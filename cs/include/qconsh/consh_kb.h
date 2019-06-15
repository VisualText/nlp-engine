/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CONSH_KB.H
*
* FILE:	consh.¹/consh_kb.h
* SUBJ:	Declarations for Consh-specific knowledge base handlers.
* CR:	11/28/95 AM.
*
*******************************************************************************/

#ifdef _MOVING_OUT_OF_HERE_070219_

extern LIBQCONSH_API void
con_add_root(CG*);
//extern CON_ID 
//con_phrase(
//	CON_ID con
//	);
extern LIBQCONSH_API bool con_stationary(CON_ID con);
/*
LIBQCONSH_API CON_ID 	// 05/04/99 AM.
phr_add_nth(
	CON_ID owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	_TCHAR *name,
	int pos,			// position to insert into.  if = 0, at end of list.
	CG *cg
	);
LIBQCONSH_API CON_ID 	// 05/05/99 AM.
phr_add_nth(
	CON_ID owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	CON_ID node,		// Node to attach.
	int pos,			// position to insert into.  if = 0, at end of list.
	CG *cg
	);
*/
LIBQCONSH_API CON_ID 	// 05/05/99 AM.
insert_node(
	CON_ID owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	CON_ID node,		// Node to attach.
	CON_ID n_pos,		// position to insert before.  if = 0, at end of list.
	CG *cg
	);
LIBQCONSH_API bool	// 05/04/99 AM.
rm_proxy(
	CON_ID proxy,
	CG *cg
	);
int compare_nodes(const void *arg1, const void *arg2);
bool sort_phrase(
	CON_ID con,			// The concept owning the phrase.
	CG *cg
	);
LIBQCONSH_API bool
attach_phrase(CON_ID con, CON_ID node, CG *cg);
LIBQCONSH_API CON_ID 
detach_phrase(CON_ID con, CG *cg);

#endif
