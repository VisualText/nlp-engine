/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									FIND.H
*
* FILE:	consh.¹/find.h
* SUBJ:	Declares for finding stuff in the knowledge base.
* CR:	12/10/95 AM.
*
*******************************************************************************/

#ifdef OLD_020815_
extern LIBKBM_API LIST *
find_name(
	_TCHAR *name,
	AKBM *akbm,																// 08/15/02 AM.
	/*UP*/
	LIST **end
	);
extern LIBKBM_API void
find_name_rec(
	_TCHAR *name,
	CON *root,
	ALIST *alist,																// 08/14/02 AM.
	/*UP*/
	LIST **list,
	LIST **end
	);
#endif
