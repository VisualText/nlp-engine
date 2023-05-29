/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									DICT.C
*
* FILE:	ext.¹/dict.c
* SUBJ:	Dictionary in kb.
* CR:	10/22/95 AM.
* NOTE:	
*
*******************************************************************************/

#include "StdAfx.h"
#include <ctype.h>		// 03/07/00 AM.
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <filesystem>
#include "prim/libprim.h"
#include "kbm/libkbm.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prim/prim.h"
#include "prim/str.h"
//#include "prim/mach.h"
#include "prim/list_s.h"
#include "prim/list.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
/* #include "Con_ini.h" */
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/dict.h"
#include "kbm/Akbm.h"
/* #include "cc_var.h" */

#include "kbm_priv.h"
#include "prim/unicu.h"


/**************************************************
*						DICT_ADD_PUNCT
* FUN:	dict_add_punct
* SUBJ:	Specialized add of punctuation chars to kb.
* CR:		11/1/95 AM.
* RET:	True if successful.
*
**************************************************/

bool AKBM::dict_add_punct()
{
CON *hier;

hier = acon_->c_nlp_PUNCT;	/* Get top of punct hierarchy. */

acon_->con_add_word_first(_T(";"), hier);
acon_->con_add_word_first(_T("."), hier);
acon_->con_add_word_first(_T(","), hier);
acon_->con_add_word_first(_T(":"), hier);
acon_->con_add_word_first(_T("'"), hier);
acon_->con_add_word_first(_T("\""), hier);
acon_->con_add_word_first(_T("!"), hier);
acon_->con_add_word_first(_T("?"), hier);
acon_->con_add_word_first(_T("~"), hier);
acon_->con_add_word_first(_T("`"), hier);
acon_->con_add_word_first(_T("@"), hier);
acon_->con_add_word_first(_T("#"), hier);
acon_->con_add_word_first(_T("$"), hier);
acon_->con_add_word_first(_T("%"), hier);
acon_->con_add_word_first(_T("^"), hier);
acon_->con_add_word_first(_T("&"), hier);
acon_->con_add_word_first(_T("*"), hier);
acon_->con_add_word_first(_T("("), hier);
acon_->con_add_word_first(_T(")"), hier);
acon_->con_add_word_first(_T("-"), hier);
acon_->con_add_word_first(_T("_"), hier);
acon_->con_add_word_first(_T("="), hier);
acon_->con_add_word_first(_T("+"), hier);
acon_->con_add_word_first(_T("{"), hier);
acon_->con_add_word_first(_T("}"), hier);
acon_->con_add_word_first(_T("["), hier);
acon_->con_add_word_first(_T("]"), hier);
acon_->con_add_word_first(_T("|"), hier);
acon_->con_add_word_first(_T("\\"), hier);
acon_->con_add_word_first(_T("/"), hier);
acon_->con_add_word_first(_T("<"), hier);
acon_->con_add_word_first(_T(">"), hier);
return(true);
}


/**************************************************
*						DICT_ADD_UNKNOWN
* FUN:	dict_add_unknown
* SUBJ:	Quickly add new word to kb.
* CR:		10/25/95 AM.
* RET:	con - word concept, if successful.
*
**************************************************/

CON *AKBM::dict_add_unknown(
	_TCHAR *name
	)
{
return(acon_->con_add_word_first(name, acon_->c_dict_UNKNOWN));
}


/**************************************************
*						DICT_ADD_WHITE
* FUN:	dict_add_white
* SUBJ:	Specialized add of whitespace chars to kb.
* CR:	11/1/95 AM.
* RET:	True if successful.
*
**************************************************/

bool AKBM::dict_add_white()
{
CON *wht;

wht = acon_->c_nlp_WHT;
acon_->con_add_word_first(_T(" "), wht);
acon_->con_add_word_first(_T("	"), wht);
acon_->con_add_word_first(_T("\n"), wht);
return(true);
}


/**************************************************
*						DICT_ADD_WORD
* FUN:	dict_add_word
* SUBJ:	Add a word to kb.
* CR:		10/22/95 AM.
* RET:	con - word concept, if successful.
*
**************************************************/

CON *AKBM::dict_add_word(
	_TCHAR *name			/* Word to add. */
	)
{
SYM *sym;
bool dirty;									// 06/29/03 AM.

if (!(sym = asym_->sym_get(name,dirty))) /* Find or make sym */
   return(CNULL);

if (sym->con)
   return(CNULL);					// Exists already.

DICT_CALL caller;
return dict_add_word_con((ASYM*)sym,dirty,caller);
}

CON *AKBM::dict_add_word_dict(
	_TCHAR *name,
	DICT_CALL caller
	)
{
SYM *sym;
bool dirty;									// 06/29/03 AM.

if (!(sym = asym_->sym_get(name,dirty))) /* Find or make sym */
   return(CNULL);

if (sym->con)
   return(CNULL);					// Exists already.

return dict_add_word_con((ASYM*)sym,dirty,caller);
}



/**************************************************
*						DICT_ADD_WORD_CON
* FUN:	dict_add_word_con
* SUBJ:	Add a word-concept to kb.
* CR:		06/29/03 AM.
* RET:	con - word concept, if successful.
* NOTE:	Taken from dict_add_word.
*			Separating making of sym from making of concept.
*			Keeping old comments.
**************************************************/

inline CON *AKBM::dict_add_word_con(
	ASYM *sym,
	bool &dirty,		// 06/29/03 AM.
	DICT_CALL caller
	)
{
CON *con, *hier, *index;

if (!sym)
   return(CNULL);

if (((SYM *)sym)->con)
   return ((SYM*)sym)->con;			// Exists already.
_TCHAR *name = ((SYM*)sym)->str;											// 06/29/03 AM.

dirty = true;																	// 06/29/03 AM.

// 05/02/99 AM. Allowing nonalphabetic starts.
//UChar32 ch = *name;	// Handle extended ANSI.	// FIX.	// 10/08/99 AM.

UChar32 ch;
int32_t end = 0;
int32_t length = 0;
U8_NEXT(name, end, length, ch);

if (unicu::isAlphabetic(ch))											// FIX.	// 10/08/99 AM.
	hier  = acon_->c_dict_ALPHA;	// Root of index tree.			// 06/18/03 AM.
else if (unicu::isWhiteSpace(ch))									// 05/02/99 AM.
	return acon_->con_add_word_first(name, acon_->c_nlp_WHT);	// 05/02/99 AM.
else if (unicu::isPunct(ch))		// Handle ANSI.		// 10/08/99 AM.
	return acon_->con_add_word_first(name, acon_->c_nlp_PUNCT);	// 05/02/99 AM.
else if (unicu::isEmoji(ch))
	{
	hier = ACON::con_parent(acon_->c_dict_ALPHA);
	hier = ACON::con_get_child(hier, _T("emoji"));
	}
else if (unicu::isDigit(ch))									// 05/13/99 AM.
	{
	// Hacking in a place for phrases that start with digit.
	// 05/13/99 AM.
	hier = ACON::con_parent(acon_->c_dict_ALPHA);// Get parent of dictionary.
	hier = ACON::con_get_child(hier, _T("0"));		// Numeric root.
//	return acon_->con_add_word_first(name, hier);		// FIX.	// 06/18/03 AM.
	}
else	// Catchall																// 06/19/03 AM.
	{
	if (caller.type == DICT_CALL_FILE) {
		std::string s = std::filesystem::path(caller.file).filename().string();
    	std::_t_cerr << _T("0 ") << caller.line << _T(" [Unhandled \"") << name << "\" " << s << _T("]") << std::endl;
	} else {
    	std::_t_cerr << _T("[dict_add_word: Unhandled word=") << name << _T("]") << std::endl;
	}
	hier = acon_->c_dict_ALPHA;											// 06/19/03 AM.
	}

/* Traverse indexes to find index for word. */
index = dict_find_index(hier, name);
if (!index)
   {
   std::_t_cerr << _T("[dict_add_word: Failed on name=") << name << _T("]") << std::endl;
   return(CNULL);
   }

/* Find location under index. */
/* Add word at location. */
/* Bind word to its sym. */
con = acon_->con_add_alpha(index, name);
if (!con)
   return(CNULL);

return(con);
}


/**************************************************
*						DICT_RM_WORD
* FUN:	dict_rm_word
* SUBJ:	Remove a word from kb.
* CR:		07/17/00 AM.
* RET:	bool - true if found and successful.
* NOTE:	If doing lowercase manipulation, need to find lowercase word
*			and concept also? (If so, need to have counter or something in
*			the lowercase word that tells it others are pointing to it.)
*
**************************************************/

bool AKBM::dict_rm_word(
	_TCHAR *str
	)
{
CON *con;
SYM *sym;

if (!(sym = asym_->sym_find(str)))
   return false;

if (!(con = sym->con))
   return false;

sym->con = CNULL;		// Zap pointer from sym to its word concept.
return ACON::con_remove(con);
}


/**************************************************
*						DICT_FIND_INDEX
* FUN:	dict_find_index
* SUBJ:	Find index concept for given word.
* CR:		10/23/95 AM.
* RET:	index - Concept under which to place name.
*
**************************************************/

CON *AKBM::dict_find_index(
	CON *root,
	_TCHAR *name
	)
{
CON *subs, *index;
int result;

if (!root || !str_full(name))
   return(CNULL);

subs = root->dn;
if (!subs || subs->kind == cWORD)
   return(root);		/* Root is the index concept. */
result = _tcscmp(name, ACON::con_str(subs));
if (result < 0)
   {
   std::_t_cerr << _T("[dict_find_index: name=")
		  << name << _T(" precedes first index=")
		  << ACON::con_str(subs) << _T("]") << std::endl;
   return(CNULL);
   }
else if (result == 0)
   return(dict_find_index(subs, name));

index = subs;
while ((subs = subs->next))
   {
   result = _tcscmp(name, ACON::con_str(subs));
   if (result < 0)
      return(dict_find_index(index, name));
   else if (result == 0)
      return(dict_find_index(subs, name));
   index = subs;
   }
return(dict_find_index(index, name));
}


/**************************************************
*						DICT_FIND_INDEX
* FUN:	dict_find_index
* SUBJ:	Find index concept for given word.
* CR:		05/31/00 AM.
* RET:	index - Concept under which to place name.
* NOTE:	Variant.
*
**************************************************/

CON *AKBM::dict_find_index(
	_TCHAR *name
	)
{
return dict_find_index(acon_->c_dict_ALPHA, name);
}


/**************************************************
*						DICT_FIND_WORD
* FUN:	dict_find_word
* SUBJ:	Find word if present in system.
* CR:		10/26/95 AM.
* RET:	word - Word concept, if present.
*
**************************************************/

CON *
AKBM::dict_find_word(
	_TCHAR *name
	)
{
SYM *sym;

if ((sym = asym_->sym_find(name)))
   return(sym->con);
return(CNULL);
}

/**************************************************
*						DICT_GET_WORD
* FUN:	dict_get_word
* SUBJ:	Find word if present, else make it.
* CR:		06/29/03 AM.
* RET:	word - Word concept, if present.
*
**************************************************/

CON *
AKBM::dict_get_word(
	_TCHAR *name,	// Word string to look up.
	bool &dirty	// If KB has changed.
	)
{
SYM *sym;

sym = asym_->sym_get(name,dirty);				// 06/29/03 AM.
if (sym->con)											// 06/29/03 AM.
   return(sym->con);									// 06/29/03 AM.
DICT_CALL caller;
return dict_add_word_con((ASYM*)sym,dirty,caller);	// 06/29/03 AM.
}

CON *
AKBM::dict_get_word(
	_TCHAR *name,	// Word string to look up.
	bool &dirty,	// If KB has changed.
	DICT_CALL caller
	)
{
SYM *sym;

sym = asym_->sym_get(name,dirty);				// 06/29/03 AM.
if (sym->con)											// 06/29/03 AM.
   return(sym->con);									// 06/29/03 AM.
return dict_add_word_con((ASYM*)sym,dirty,caller);	// 06/29/03 AM.
}


/**************************************************
*						DICT_FIND_WORD_LC
* FUN:	dict_find_word_lc
* SUBJ:	Find word lowercased if present in system.
* CR:		7/19/96 AM.
* RET:	word - Word concept, if present.
* NOTE:	To document when we need to lowercase.
* WARN:	MODIFIES STRING IN PLACE.
*			CALLER MUST COPY STRING BEFORE CALL, IF NEED BE.
*
**************************************************/

CON *
AKBM::dict_find_word_lc(
	_TCHAR *name
	)
{
SYM *sym;

if (!name || !*name)
	return CNULL;
s_to_lower(name);

if ((sym = asym_->sym_find(name)))
   return(sym->con);
return(CNULL);
}


/**************************************************
*						DICT_FIRST
* FUN:	dict_first
* SUBJ:	Get first alphabetic word-concept in dict.
* CR:		03/21/03 AM.
* RET:	con - first alphabetic word-concept in dict.
* NOTE:	For easy dict traversal.
*			Traverse indices looking for first word-concept.
*
**************************************************/

CON *AKBM::dict_first()
{
CON *root  = acon_->c_dict_ALPHA;			/* Root of index tree. */
CON *subs = 0;
if (!root)
	return 0;	// Error. No dict hierarchy.
return dict_subtree(root, true);
}



/**************************************************
*						DICT_NEXT
* FUN:	dict_next
* SUBJ:	Get next alphabetic word-concept in dict.
* CR:		03/21/03 AM.
* RET:	con - next alphabetic word-concept in dict.
* NOTE:	For easy dict traversal.
*
**************************************************/

CON *AKBM::dict_next(CON *wordcon)
{
if (!wordcon)
	return 0;
if (wordcon->kind != cWORD)
	{
	// Assume it's an index.
	if (wordcon == acon_->c_dict_ALPHA)
		return dict_subtree(wordcon,true);
	else
		return dict_next(wordcon,false);
	}
if (wordcon->next)
	return wordcon->next;
return dict_next(wordcon,false);
}


/**************************************************
*						DICT_NEXT
* FUN:	dict_next
* SUBJ:	Get next alphabetic word-concept in dict, given an index con.
* CR:		03/21/03 AM.
* RET:	con - next alphabetic word-concept in dict.
* NOTE:	Variant. Recursive part. For easy dict traversal.
*			Given an index concept, find 
*
**************************************************/

CON *AKBM::dict_next(CON *con, bool rootflag)
{
if (!con)
	return 0;

CON *tmp, *found;

// If I'm a word concept, then go to next.
if (con->kind == cWORD)
	{
	if (con->next)
		return con->next;
	}

// I'm an index concept
else
	{
	// Traverse me and rest of my list.
	for (tmp = con; tmp; tmp = tmp->next)
		{
		// Look in subtree for first wordcon.
		if ((found = dict_subtree(tmp,true)))
			return found;
		}
	}

// Didn't find anything down and right, so try going up.
for (;;)
	{
	if (!(con = ACON::con_parent(con)))
		return 0;	// Hit root of kb.  Error.
	if (con == acon_->c_dict_ALPHA)
		return 0;	// Couldn't go up and right, hit top of dict.
	if (con->next)
		return dict_next(con->next,false);
	// Couldn't go right, so keep going up.
	}
return 0;
}


/**************************************************
*						DICT_SUBTREE
* FUN:	dict_subtree
* SUBJ:	Get first alphabetic word-concept in dict subtree.
* CR:		03/21/03 AM.
* RET:	con - first alphabetic word-concept in subtree.
* NOTE:	Recursive. For easy dict traversal.
*			Given an index concept, find 
*
**************************************************/

CON *AKBM::dict_subtree(CON *con, bool rootflag)
{
if (!con)
	return 0;

CON *tmp, *found;

// If I'm a word concept, then done.
if (con->kind == cWORD)
	return con;

// I'm an index concept
// Look down.
if ((tmp = con->dn))
	{
	if (tmp->kind == cWORD)
		return tmp;
	if ((found = dict_subtree(tmp,false)))
		return found;
	}

// Look right, if head of list and not "root".
if (!rootflag && !con->prev)
	{
	for (tmp = con->next; tmp; tmp = tmp->next)
		{
		if ((found = dict_subtree(tmp,true)))
			return found;
		}
	}

return 0;
}
