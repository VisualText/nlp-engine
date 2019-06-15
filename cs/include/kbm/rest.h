/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									REST.H
*
* FILE:	consh.¹/rest.h
* SUBJ:	Declarations for rule-action manager.
* CR:		7/7/96 AM.
*
*******************************************************************************/

/* Maximum number of actions for a rule. (For some optimizations.) */
#define MAX_RESTS	((long) 128)

extern LIBKBM_API bool
rest_add(
	CON *con,
   CON *typ,
   CON *ds,
   CON *slot,
	CON *val,
	long from,
	long to
	);
extern LIBKBM_API long		 rest_inc_count(CON *con);
extern LIBKBM_API PTR *
rest_make_vals(
   long cnt,	/* Action attribute number. */
   CON *typ,
   CON *ds,
   CON *slot,
   CON *val,
   long from,
   long to);
LIBKBM_API void
rest_get_rests(CON *pass, PTR **acts);
extern LIBKBM_API long
rest_get_count(CON *con);
LIBKBM_API void
rest_get_rest(
	PTR *ptr,
	/*UP*/
	CON **typ,
	CON **ds,
	CON **slot,
	CON **val,
	long *from,
	long *to
	);
extern LIBKBM_API void
rest_get_actions(CON *rule, PTR **acts);
extern LIBKBM_API void
rest_get_action(
	PTR *ptr,
	/*UP*/
	CON **typ,
	CON **ds,
	CON **slot,
	CON **val,
	long *from,
	long *to
	);

extern LIBKBM_API void
rest_get_rest_s(
	PTR *ptr,
	/*UP*/
	_TCHAR **s_typ,
	_TCHAR **s_ds,
	_TCHAR **s_slot,
	_TCHAR **s_val,
	long *from,
	long *to
	);
