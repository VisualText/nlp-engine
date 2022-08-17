/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									FIND.C
*
* FILE:	consh.�/find.c
* SUBJ:	Find stuff in the knowledge base.
* CR:	12/10/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prim/prim.h"
#include "prim/str.h"
#include "prim/list_s.h"
#include "prim/list.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/find.h"

#include "kbm_priv.h"
#include "kbm/Akbm.h"	// 08/15/02 AM.

/**************************************************
*						FIND_NAME
* FUN:	find_name
* SUBJ:	Find instances of name in kb.
* CR:	12/10/95 AM.
* RET:	list - list of pointers to concepts.
*		UP end - end of list (optimization for freeing.)
* EFF:	Caller must free the returned list.
* NOTE:	Not searching for nodes, but only hierarchy concepts.
*
**************************************************/

LIST *AKBM::find_name(
	_TCHAR *name,
	/*UP*/
	LIST **end
	)
{
LIST *list;

/* Traverse entire kb looking for concepts that match name. */
list = *end = LNULL;
find_name_rec(name, acon_->c_cg_CONCEPT, /*DU*/ &list, end);
return(list);
}


/**************************************************
*						FIND_NAME_REC
* FUN:	find_name
* SUBJ:	Find instances of name in kb. Recursive.
* CR:	12/10/95 AM.
* RET:	(UP) list - list of pointers to concepts.
*
**************************************************/

void AKBM::find_name_rec(
	_TCHAR *name,
	CON *root,
	/*UP*/
	LIST **list,
	LIST **end
	)
{
CON *childs;

if (!root)
   return;
if (!_tcscmp(name, ACON::con_str(root)))	/* Found match */
   {
   _ftprintf(stderr, _T("Found match!\n"));
   (*list) = alist_->list_add(*list, (long) root, /*DU*/ end);	// 08/14/02 AM.
   }

/* Check out hierarchy. */
for (childs = root->dn; childs; childs = childs->next)
   find_name_rec(name, childs, /*DU*/ list, end);			// 08/14/02 AM.
}
