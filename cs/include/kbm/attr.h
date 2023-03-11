/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									ATTR.H
*
* FILE:	libkbm includes/attr.h
* SUBJ:	Declarations for attr manager.
* CR:	10/9/95 AM.
*
*******************************************************************************/
#ifdef OLD_020815_
LIBKBM_API PTR *attr_add_by_name(CON*,_TCHAR*,_TCHAR*);				// 05/03/99 AM.
LIBKBM_API PTR *attr_add_by_name(CON*,_TCHAR*,long long);					// 05/03/99 AM.
LIBKBM_API PTR *attr_add_by_name(CON*,_TCHAR*,float);				// 08/16/01 AM.
LIBKBM_API PTR *attr_add_by_name(CON*,_TCHAR*,CON*);					// 08/11/00 AM.
LIBKBM_API bool attr_rm_by_name(CON*, _TCHAR*);
LIBKBM_API bool
attr_rm(							// 07/12/99 AM.
	CON *con,
   PTR *ptr
   );
LIBKBM_API PTR *
attr_add_new(
   CON *con,
   CON *slot,
   PTR *vals);	/* May be NULL */
LIBKBM_API PTR *
attr_add_n(
	CON *con,
	CON *slot_con,
	long num
	);
LIBKBM_API PTR *
attr_add_c(
	CON *con,
	CON *slot_con,
	CON *val
	);

LIBKBM_API PTR *
attr_add_val(
	CON *con,
	CON *slot_con,
	PVAL val,
	PKIND kind
	);

LIBKBM_API PTR *
attr_find(CON *con, _TCHAR *s_slot);				// 08/12/99 AM.
LIBKBM_API PTR *
attr_find_h(CON *con, _TCHAR *name);				// 07/12/99 AM.
LIBKBM_API PTR *
attr_prefind_h(CON *con, _TCHAR *name, /*UP*/ bool &first);	// 07/12/99 AM.
LIBKBM_API PTR *
attr_prefind_h(CON *con, PTR *ptr, /*UP*/ bool &first);		// 07/12/99 AM.
LIBKBM_API PTR *
attr_find_slot(CON *con, CON *slot_con);
LIBKBM_API PTR *
attr_find_slot_0(CON *con, CON *slot_con);
LIBKBM_API PTR *
attr_get(CON *con, CON *slot);
LIBKBM_API PTR *
attr_get(CON *con, _TCHAR *s_slot);				// 05/03/99 AM.
LIBKBM_API CON *
attr_get_c(CON *con, CON *slot);
LIBKBM_API long
attr_get_n(CON *con, CON *slot);
LIBKBM_API bool
attr_get_ns(CON *con, CON *slot, int len, /*UP*/ long *arr);

LIBKBM_API SYM *
attr_get_s(CON *con, CON *slot);
LIBKBM_API PTR *
attr_get_slot(CON *con, CON *slot_con);
LIBKBM_API void
attrs_pret(
	PTR *attrs,
	//FILE *out,		// 04/20/99 AM.
	_t_ostream *out,		// 04/20/99 AM.
	_TCHAR *indent		/* String for indenting. */
	);

/******/

LIBKBM_API PTR *
attr_a_add_end(CON *con);
LIBKBM_API PTR *
attr_as_add_end(
	CON *con,		/* Concept to add attribute to. */
   CON *slot_con	/* Slot concept.				*/
   );
LIBKBM_API PTR *
attr_asv_add_end(
	CON *con,		/* Concept to add attribute to. */
   CON *slot_con	/* Slot concept.				*/
   );
LIBKBM_API PTR *
attr_s_add(
	PTR *attr,
	CON *slot_con		/* Concept representing slot, if any.	*/
	);
LIBKBM_API PTR *
attr_v_add_end(PTR *slot);

LIBKBM_API _TCHAR *		// 07/05/99 AM.
attrs_name(
	PTR *attrs
	);
LIBKBM_API PTR *		// 07/05/99 AM.
attrs_vals(
	PTR *attrs
	);

	LIBKBM_API bool															// 05/11/00 AM.
rm_one_val(
	PTR *attr,
   PTR *val
   );
LIBKBM_API bool																// 05/17/00 AM.
attr_rm_val_by_name(
   CON *con,
   _TCHAR *attr_s,
	_TCHAR *val_s
   );
LIBKBM_API bool
attr_rm_val_by_name(															// 05/19/00 AM.
   CON *con,
   _TCHAR *attr_s,
	long val
   );
LIBKBM_API bool
attr_rm_val_by_concept(															// 06/03/02 Dd.
   CON *con,
   _TCHAR *attr_s,
	CON *val
   );

LIBKBM_API bool																// 10/31/00 AM.
attr_is_sys(
	PTR *ptr
   );
LIBKBM_API bool																// 10/31/00 AM.
attrs_rm(
	CON *con,
	/*UP*/
	bool &dirty		// If kb has changed.
   );
LIBKBM_API bool
attr_rename(					// 07/12/99 AM.
   PTR *ptr,			// Handle for attribute.
	_TCHAR *rename
   );
LIBKBM_API PTR *
attr_get_h(CON *con, _TCHAR *name);				// 05/03/99 AM.
#endif
