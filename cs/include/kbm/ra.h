/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									RA.H
*
* FILE:	consh.¹/ra.h
* SUBJ:	Declarations for rule-action manager.
* CR:	12/20/95 AM.
*
*******************************************************************************/

/* Maximum number of actions for a rule. (For some optimizations.) */
#define MAX_ACTIONS	((long) 128)

extern LIBKBM_API bool
ra_add(
	CON *con,
   CON *typ,
   CON *ds,
   CON *slot,
	CON *val,
	long from,
	long to
	);
extern LIBKBM_API long		 ra_inc_count(CON *con);
extern LIBKBM_API PTR *
ra_make_vals(
   long cnt,	/* Action attribute number. */
   CON *typ,
   CON *ds,
   CON *slot,
   CON *val,
   long from,
   long to);

extern LIBKBM_API long
ra_get_count(CON *con);
extern LIBKBM_API void
ra_get_actions(CON *rule, PTR **acts);
extern LIBKBM_API void
ra_get_action(
	PTR *ptr,
	/*UP*/
	CON **typ,
	CON **ds,
	CON **slot,
	CON **val,
	long *from,
	long *to
	);
