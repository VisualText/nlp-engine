/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CONSH_KB.H
*
* FILE:	consh./consh_kb.h
* SUBJ:	Declarations for Consh-specific knowledge base handlers.
* CR:	11/28/95 AM.
*
*******************************************************************************/

extern LIBCONSH_API void
con_add_root(CG*);
//extern CON *
//con_phrase(
//	CON *con
//	);
extern LIBCONSH_API bool con_stationary(CON *con);
LIBCONSH_API CON *	// 05/04/99 AM.
phr_add_nth(
	CON *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	_TCHAR *name,
	int pos,			// position to insert into.  if = 0, at end of list.
	CG *cg
	);
LIBCONSH_API CON *	// 05/05/99 AM.
phr_add_nth(
	CON *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	CON *node,		// Node to attach.
	int pos,			// position to insert into.  if = 0, at end of list.
	CG *cg
	);
LIBCONSH_API CON *	// 05/05/99 AM.
insert_node(
	CON *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
	CON *node,		// Node to attach.
	CON *n_pos,		// position to insert before.  if = 0, at end of list.
	CG *cg
	);
LIBCONSH_API bool	// 05/04/99 AM.
rm_proxy(
	CON *proxy,
	CG *cg
	);
int compare_nodes(const void *arg1, const void *arg2);
bool sort_phrase(
	CON *con,			// The concept owning the phrase.
	CG *cg
	);
bool sort_childs(    // 02/20/09 AM.
	CON *con,         // The parent concept.
	CG *cg
	);
LIBCONSH_API bool
attach_phrase(CON *con, CON *node, CG *cg);
LIBCONSH_API CON *
detach_phrase(CON *con, CG *cg);

