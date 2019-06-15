/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DICT.CPP
* FILE:	cs/libqconsh/cgattr.cpp
* CR:		02/18/07 AM.
* SUBJ:	Class for managing CG KBMS dictionary subsystem.
* NOTE:	Overhauls for QDBM.
*			Moving many of the dict.cpp functions from libkbm to here.
*			libkbm/dict.cpp no longer present.
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include "machine-min.h"
#include "prim/libprim.h"
#include "prim/str.h"

#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/xcon_s.h"
#include "qkbm/xptr_s.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "cg_global.h"
#include "qconsh/cg.h"

#include "attr.h"
#include "cgcon.h"

#include "cgdict.h"


int CGDICT::count_ = 0;



/********************************************
* FN:		Special functions for the class
********************************************/

CGDICT::CGDICT(
	CG *cg	// Want the kbms available to attr manager.
	)
{
cg_ = cg;

c_one = c_two = c_word = 0;	// 03/06/07 AM.
c_one_next = c_two_next = 0;	// 03/06/07 AM.
s_one_next = s_two_next = 0;	// 03/06/07 AM.

#ifndef STABLE_
++count_;
#endif
}


CGDICT::~CGDICT()
{
dict_load_fin();

#ifndef STABLE_
--count_;
#endif
}

XCON_S *CGDICT::getC_one()
{
return c_one;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/**************************************************
*						DICT_ADD_PUNCT
* FUN:	dict_add_punct
* SUBJ:	Specialized add of punctuation chars to kb.
* CR:		11/1/95 AM.
* RET:	True if successful.
*
**************************************************/

bool CGDICT::dict_add_punct()
{
XCON_S *hier;

hier = cg_->qkbm_->Con(cg_->nlp_PUNCT);	/* Get top of punct hierarchy. */

cg_->cgcon_->con_add_word_first(_T(";"), hier);
cg_->cgcon_->con_add_word_first(_T("."), hier);
cg_->cgcon_->con_add_word_first(_T(","), hier);
cg_->cgcon_->con_add_word_first(_T(":"), hier);
cg_->cgcon_->con_add_word_first(_T("'"), hier);
cg_->cgcon_->con_add_word_first(_T("\""), hier);
cg_->cgcon_->con_add_word_first(_T("!"), hier);
cg_->cgcon_->con_add_word_first(_T("?"), hier);
cg_->cgcon_->con_add_word_first(_T("~"), hier);
cg_->cgcon_->con_add_word_first(_T("`"), hier);
cg_->cgcon_->con_add_word_first(_T("@"), hier);
cg_->cgcon_->con_add_word_first(_T("#"), hier);
cg_->cgcon_->con_add_word_first(_T("$"), hier);
cg_->cgcon_->con_add_word_first(_T("%"), hier);
cg_->cgcon_->con_add_word_first(_T("^"), hier);
cg_->cgcon_->con_add_word_first(_T("&"), hier);
cg_->cgcon_->con_add_word_first(_T("*"), hier);
cg_->cgcon_->con_add_word_first(_T("("), hier);
cg_->cgcon_->con_add_word_first(_T(")"), hier);
cg_->cgcon_->con_add_word_first(_T("-"), hier);
cg_->cgcon_->con_add_word_first(_T("_"), hier);
cg_->cgcon_->con_add_word_first(_T("="), hier);
cg_->cgcon_->con_add_word_first(_T("+"), hier);
cg_->cgcon_->con_add_word_first(_T("{"), hier);
cg_->cgcon_->con_add_word_first(_T("}"), hier);
cg_->cgcon_->con_add_word_first(_T("["), hier);
cg_->cgcon_->con_add_word_first(_T("]"), hier);
cg_->cgcon_->con_add_word_first(_T("|"), hier);
cg_->cgcon_->con_add_word_first(_T("\\"), hier);
cg_->cgcon_->con_add_word_first(_T("/"), hier);
cg_->cgcon_->con_add_word_first(_T("<"), hier);
cg_->cgcon_->con_add_word_first(_T(">"), hier);
cg_->kbfree(hier);	// 03/05/07 AM.
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

#ifdef _OBSOLET_070219
CON_ID CGDICT::dict_add_unknown(
	_TCHAR *name
	)
{
return(cg_->cgcon_->con_add_word_first(name, cg_->c_dict_UNKNOWN));
}
#endif


/**************************************************
*						DICT_ADD_WHITE
* FUN:	dict_add_white
* SUBJ:	Specialized add of whitespace chars to kb.
* CR:	11/1/95 AM.
* RET:	True if successful.
*
**************************************************/

bool CGDICT::dict_add_white()
{
XCON_S *wht;

wht = cg_->qkbm_->Con(cg_->nlp_WHT);;
cg_->cgcon_->con_add_word_first(_T(" "), wht);
cg_->cgcon_->con_add_word_first(_T("	"), wht);
cg_->cgcon_->con_add_word_first(_T("\n"), wht);
cg_->kbfree(wht);	// 03/05/07 AM.
return(true);
}


/**************************************************
*						DICT_ADD_WORD
* FUN:	dict_add_word
* SUBJ:	Add a word to kb.
* CR:		10/22/95 AM.
* RET:	con - word concept, if successful.
* NOTE:	02/20/07 AM. REIMPLEMENT.
*			Relaxing call to succeed if word already in dictionary.
*
**************************************************/

CON_ID CGDICT::dict_add_word(
	_TCHAR *name			/* Word to add. */
	)
{
bool dirty;									// 06/29/03 AM.

if (str_empty(name))	// 02/20/07 AM.
	return 0;			// 02/20/07 AM.

STR_ID sid = cg_->qkbm_->sym_get(name, /*UP*/dirty);
if (!sid)
	return 0;


return dict_add_word_con(sid,name,dirty);
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

inline CON_ID CGDICT::dict_add_word_con(
	STR_ID sid,
	_TCHAR *name,
	bool &dirty		// 06/29/03 AM.
	)
{
dirty = 0;

XCON_S *hier, *index;
CON_ID hier_cid;
XCON_S *root;	// 03/05/07 AM.
CON_ID cid;	// 03/05/07 AM.

if (!sid)
   return 0;

// If string already has a dict concept, can't add it.
CON_ID sym_cid;
if (sym_cid = cg_->qkbm_->getXsym(sid))
	return sym_cid;

if (str_empty(name))	// If not given name, fetch it.				// 02/20/07 AM.
	name = cg_->qkbm_->getXstr(sid);										// 02/20/07 AM.

dirty = true;																	// 06/29/03 AM.

// 05/02/99 AM. Allowing nonalphabetic starts.
_TUCHAR ch = *name;	// Handle extended ANSI.	// FIX.	// 10/08/99 AM.
if (cg_alphabetic(ch))											// FIX.	// 10/08/99 AM.
	hier_cid  = cg_->dict_ALPHA;	// Root of index tree.			// 06/18/03 AM.
else if (_istspace((_TUCHAR)ch))									// 05/02/99 AM.
	{
//	return cg_->cgcon_->con_add_word_first(name, cg_->qkbm_->Con(cg_->nlp_WHT));	// 05/02/99 AM.
	root = cg_->qkbm_->Con(cg_->nlp_WHT);					// 03/05/07 AM.
	cid = cg_->cgcon_->con_add_word_first(name, root);	// 03/05/07 AM.
	cg_->kbfree(root);	// 03/05/07 AM.
	return cid;				// 03/05/07 AM.
	}
else if (cg_punct(ch))		// Handle ANSI.		// 10/08/99 AM.
	{
//	return cg_->cgcon_->con_add_word_first(name, cg_->qkbm_->Con(cg_->nlp_PUNCT));	// 05/02/99 AM.
	root = cg_->qkbm_->Con(cg_->nlp_PUNCT);				// 03/05/07 AM.
	cid = cg_->cgcon_->con_add_word_first(name, root);	// 03/05/07 AM.
	cg_->kbfree(root);	// 03/05/07 AM.
	return cid;				// 03/05/07 AM.
	}
else if (_istdigit((_TUCHAR)ch))									// 05/13/99 AM.
	{
	// Hacking in a place for phrases that start with digit.
	// 05/13/99 AM.
	root = cg_->qkbm_->Con(cg_->dict_ALPHA);	// FIX.	// 03/21/07 AM.
	hier_cid = cg_->cgcon_->con_parent(root);// Get parent of dictionary.
	XCON_S *hr = cg_->qkbm_->Con(hier_cid);
	hier_cid = cg_->cgcon_->con_find_child(hr, _T("0"));		// Numeric root.
	cg_->kbfree(root);	// FIX.	// 03/21/07 AM.
	cg_->kbfree(hr);
//	return cg_->cgcon_->con_add_word_first(name, hier);		// FIX.	// 06/18/03 AM.
	}
else	// Catchall																// 06/19/03 AM.
	{
   _t_cerr << _T("[dict_add_word: Unhandled word=") << name << _T("]") << endl;
	hier_cid = cg_->dict_ALPHA;											// 06/19/03 AM.
	}

/* Traverse indexes to find index for word. */
hier = cg_->qkbm_->Con(hier_cid);
index = dict_find_index(hier, name);
cg_->kbfree(hier);
if (!index)
   {
   _t_cerr << _T("[dict_add_word: Failed on name=") << name << _T("]") << endl;
   return 0;
   }

/* Find location under index. */
/* Add word at location. */
/* Bind word to its sym. */
CON_ID wid = cg_->cgcon_->con_add_alpha(index, name);	// 03/19/07 AM.
if (index)	// 03/19/07 AM.
	cg_->kbfree(index);	// FIX.	// 03/19/07 AM.
return wid;	// 03/19/07 AM.
}


/**************************************************
*						DICT_LOAD_INI
* FUN:	dict_load_ini
* SUBJ:	Set up for loading words into kb's dictionary.
* CR:		03/06/07 AM.
* RET:	bool - ok if everything ok.
* NOTE:	A fast O(n) algorithm that tracks the previous word.
*			Assumes words are being added alphabetically.
*
**************************************************/

bool CGDICT::dict_load_ini()
{
// Set up first dictionary indexes.
XCON_S *hier = cg_->qkbm_->Con(cg_->dict_ALPHA);
if (!hier)
	return false;
CON_ID hier_id = hier->dn;	// FIX.	// 03/22/07 AM.
cg_->kbfree(hier);			// FIX.	// 03/22/07 AM.
if (!hier_id)
	return false;
if (!(c_one = cg_->qkbm_->Con(hier_id)))	// FIX.	// 03/22/07 AM.
	return false;
if (!(c_two = cg_->qkbm_->Con(c_one->dn)))
	return false;
c_word = 0;

c_one_next = cg_->qkbm_->Con(c_one->next);
c_two_next = cg_->qkbm_->Con(c_two->next);

s_one_next = cg_->qkbm_->con_str(c_one_next);
s_two_next = cg_->qkbm_->con_str(c_two_next);
return true;
}

/**************************************************
*						DICT_LOAD_FIN
* FUN:	dict_load_fin
* SUBJ:	Clean up after loading words into kb's dictionary.
* CR:		03/06/07 AM.
* RET:	bool - ok if everything ok.
*
**************************************************/

bool CGDICT::dict_load_fin()
{
if (cg_)							// 03/06/07 AM.
	{
	if (c_one)					// 03/06/07 AM.
		{
		cg_->kbfree(c_one);	// 03/06/07 AM.
		c_one = 0;
		}
	if (c_two)					// 03/06/07 AM.
		{
		cg_->kbfree(c_two);	// 03/06/07 AM.
		c_two = 0;
		}
	if (c_word)					// 03/06/07 AM.
		{
		cg_->kbfree(c_word);	// 03/06/07 AM.
		c_word = 0;
		}
	if (c_one_next)					// 03/06/07 AM.
		{
		cg_->kbfree(c_one_next);	// 03/06/07 AM.
		c_one_next = 0;
		}
	if (c_two_next)					// 03/06/07 AM.
		{
		cg_->kbfree(c_two_next);	// 03/06/07 AM.
		c_two_next = 0;
		}
	if (s_one_next)					// 03/06/07 AM.
		{
		cg_->kbfree(s_one_next);	// 03/06/07 AM.
		s_one_next = 0;
		}
	if (s_two_next)					// 03/06/07 AM.
		{
		cg_->kbfree(s_two_next);	// 03/06/07 AM.
		s_two_next = 0;
		}
	}

return true;
}


/**************************************************
*						DICT_LOAD_WORD
* FUN:	dict_load_word
* SUBJ:	Add a word into kb's dictionary.
* CR:		03/06/07 AM.
* RET:	con - word concept, if successful.
* NOTE:	A fast O(n) algorithm that tracks the previous word.
*			Assumes words are being added alphabetically.
*
**************************************************/

CON_ID CGDICT::dict_load_word(
	_TCHAR *name			/* Word to add. */
	)
{
bool dirty;

if (str_empty(name))
	return 0;

STR_ID sid = cg_->qkbm_->sym_get(name, /*UP*/dirty);
if (!sid)
	return 0;

// If word-concept already there, then redundant.
CON_ID cid = cg_->qkbm_->getXsym(sid);
if (cid)	// Already there.  Error in loading kb. Not allowing redundancy for now.
	return 0;

return dict_load_word_con(sid,name,dirty);
}


/**************************************************
*						DICT_LOAD_WORD_CON
* FUN:	dict_load_word_con
* SUBJ:	Add a word-concept to kb.  FAST METHOD.
* CR:		03/06/07 AM.
* RET:	con - word concept, if successful.
* NOTE:	Taken from dict_add_word.
*			Separating making of sym from making of concept.
*			Keeping old comments.
*			Assumes dict indexes are initialized properly, if no words added yet.
**************************************************/

CON_ID CGDICT::dict_load_word_con(
	STR_ID sid,
	_TCHAR *name,
	bool &dirty	
	)
{
dirty = false;

if (str_empty(name))
	return 0;

if (!sid)
   return 0;

_TUCHAR ch = *name;

// Handle non-alphabetics in the usual way.
if (!cg_alphabetic(ch))
	return dict_add_word_con(sid, name, dirty);

//hier_cid  = cg_->dict_ALPHA;	// Root of index tree.

// CHECK INDEX.
// If word still at 2nd index, add it there.
if (s_two_next)
	{
	if (_tcscmp(name,s_two_next) < 0)	// Still precedes next 2nd index.
		{
		dirty = true;
		return dict_load_add(sid);		// Add to current 2nd index.
		}
	// Else move forward.

	// Clear word.
	if (c_word)
		{
		cg_->kbfree(c_word);
		c_word = 0;
		}

	// Check if still in same 1st index.
	if (!s_one_next						// At end of 1st index.
	 || _tcscmp(name, s_one_next) < 0)	// or before next 1st index.
		{
		// Move on 2nd index.
		for (;;)
			{
			cg_->kbfree(c_two);
			cg_->kbfree(s_two_next);
			c_two = c_two_next;
			if (!c_two->next)
				{
				c_two_next = 0;
				s_two_next = 0;
				break;
				}
			c_two_next = cg_->qkbm_->Con(c_two->next);
			s_two_next = cg_->qkbm_->con_str(c_two_next);
			if (_tcscmp(name,s_two_next) < 0)	// Precedes next 2nd index.
				break;
			}
		dirty = true;
		return dict_load_add(sid);
		}
	// Else, move ahead on 1st index.
	}
else if (s_one_next)
	{
	if (_tcscmp(name,s_one_next) < 0)	// Still precede next 1st 
		{
		dirty = true;
		return dict_load_add(sid);			// Add to current 2nd index.
		}
	// Move ahead on 1st index.
	}

// MOVE FORWARD ON 1st INDEX HERE.

// Reset the previous word tracking.
if (c_word)
	{
	cg_->kbfree(c_word);
	c_word = 0;
	}

// Reset 2nd index.
if (c_two)
	{
	cg_->kbfree(c_two);
	c_two = 0;
	}
if (c_two_next)
	{
	cg_->kbfree(c_two_next);
	c_two_next = 0;
	}
if (s_two_next)
	{
	cg_->kbfree(s_two_next);
	s_two_next = 0;
	}

// Move on 1st index.
for (;;)
	{
	cg_->kbfree(c_one);
	cg_->kbfree(s_one_next);
	c_one = c_one_next;
	if (!c_one->next)
		{
		// We're at the last 1st index of dictionary.
		c_one_next = 0;
		s_one_next = 0;
		break;
		}
	c_one_next = cg_->qkbm_->Con(c_one->next);
	s_one_next = cg_->qkbm_->con_str(c_one_next);
	if (_tcscmp(name,s_one_next) < 0)	// Precedes next 1nd index.
		break;
	}

// Now we're at good 1st index. Set up 2nd index.
// Todo: sanity check that word is not before the first 2nd index.
if (!c_one->dn)
	return 0;	// Sanity check.
c_two = cg_->qkbm_->Con(c_one->dn);
if (c_two->next)
	{
	c_two_next = cg_->qkbm_->Con(c_two->next);
	s_two_next = cg_->qkbm_->con_str(c_two_next);
	}

// Check on first 2nd index.
if (s_two_next && _tcscmp(name, s_two_next) < 0)
	{
	dirty = true;
	return dict_load_add(sid);
	}

// Move on 2nd index.
for (;;)
	{
	cg_->kbfree(c_two);
	cg_->kbfree(s_two_next);
	c_two = c_two_next;
	if (!c_two->next)
		{
		c_two_next = 0;
		s_two_next = 0;
		break;
		}
	c_two_next = cg_->qkbm_->Con(c_two->next);
	s_two_next = cg_->qkbm_->con_str(c_two_next);
	if (_tcscmp(name,s_two_next) < 0)	// Precedes next 2nd index.
		break;
	}

// Add word.
dirty = true;
return dict_load_add(sid);
}


/**************************************************
*						DICT_LOAD_ADD
* FUN:	dict_load_add
* SUBJ:	Do the add of a word concept to dict.  FAST METHOD.
* CR:		03/06/07 AM.
* RET:	con - word concept, if successful.
* NOTE:	Don't know where name came from, so getting copy from db.
**************************************************/

inline CON_ID CGDICT::dict_load_add(
	STR_ID sid
	)
{
if (!c_two)
	return 0;	// Sanity check.

CON_ID prev = (c_word ? c_word->id : 0);

// Add the word-concept.
CON_ID cid = cg_->qkbm_->addXcon(
				sid,			// concept val.
				cXWORD,		// kind
				0,				// flags
				0,				// attrs
				c_two->id,	// up
				0,				// dn
				prev,			// prev
				0,				// next
				0,				// atom
				0				// phrases
				);

// Bind string to word-concept.
cg_->qkbm_->addXsym(sid, cid);	// Relate str and con.

if (!c_word)	// No word under 2nd index yet.
	{
	// Add word as first under this index.
	c_two->dn = cid;
	cg_->qkbm_->con_update(c_two->id, c_two);

	// Update tracking.
	c_word = cg_->qkbm_->Con(cid);
	}
else
	{
	// Add after previous word.
	c_word->next = cid;
	cg_->qkbm_->con_update(c_word->id, c_word);

	// Update tracking.
	cg_->kbfree(c_word);
	c_word = cg_->qkbm_->Con(cid);
	}
return cid;
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

bool CGDICT::dict_rm_word(
	_TCHAR *name
	)
{
if (str_empty(name))								// 02/20/07 AM.
	return false;									// 02/20/07 AM.

STR_ID sid = cg_->qkbm_->getStrx(name);	// 02/20/07 AM.
if (!sid)											// 02/20/07 AM.
	return false;									// 02/20/07 AM.

CON_ID sym_cid;									// 02/20/07 AM.
if (!(sym_cid = cg_->qkbm_->getXsym(sid)))// 02/20/07 AM.
	return false;

// Remove reference to dictionary concept.
cg_->qkbm_->sym_rm(sid);						// 02/20/07 AM.

return cg_->qkbm_->con_rm(sym_cid);
}


/**************************************************
*						DICT_FIND_INDEX
* FUN:	dict_find_index
* SUBJ:	Find index concept for given word.
* CR:		10/23/95 AM.
* RET:	index - Concept under which to place name.
* NOTE:	NEED REIMPLEMENT WATCHING OUT FOR GIVEN CON.
*			EASIER TO RETURN con_id, EVEN IF NOT AS EFFICIENT.
*
**************************************************/

XCON_S *CGDICT::dict_find_index(
	XCON_S *root,
	_TCHAR *name
	)
{
CON_ID subs_cid;
XCON_S *subs, *index;
int result;
XCON_S *con;	// 03/05/07 AM.

if (!root || !str_full(name))
   return 0;

subs_cid = root->dn;
if (!(subs = cg_->qkbm_->Con(subs_cid)))
	return root;

if (subs->kind == cXWORD)
	{
	cg_->kbfree(subs);
   return(root);		/* Root is the index concept. */
   }
_TCHAR *str = cg_->qkbm_->con_str(subs);
result = _tcscmp(name, str);
if (result < 0)
   {
   _t_cerr << _T("[dict_find_index: name=")
		  << name << _T(" precedes first index=")
		  << str << _T("]") << endl;
	cg_->kbfree(str);	// 03/05/07 AM.
	cg_->kbfree(subs);
   return 0;
   }
else if (result == 0)
	{
	cg_->kbfree(str);	// 03/05/07 AM.
   con = dict_find_index(subs, name);
   if (con->id != subs->id)	// Don't clobber what you're returning. // 03/05/07 AM.
		cg_->kbfree(subs);	// 03/05/07 AM.
	return con;				// 03/05/07 AM.
	}
cg_->kbfree(str);	// 03/05/07 AM.

index = subs;
while (subs_cid = subs->next)
   {
   if (!(subs = cg_->qkbm_->Con(subs_cid)))
		return 0;	// Error in db.
   str = cg_->qkbm_->con_str(subs);	// 03/05/07 AM.
   result = _tcscmp(name, str);
   cg_->kbfree(str);	// 03/05/07 AM.
   if (result < 0)
		{
      con = dict_find_index(index, name);
		if (con && con->id != index->id)	// Don't clobber what you're returning. // 03/05/07 AM.
			cg_->kbfree(index);	// 03/05/07 AM.
		cg_->kbfree(subs);		// 03/05/07 AM.
      return con;					// 03/05/07 AM.
      }
   else if (result == 0)
		{
      con = dict_find_index(subs, name);
      if (con && con->id != subs->id)	// Don't clobber what you're returning.	// 03/05/07 AM.
			cg_->kbfree(subs);	// 03/05/07 AM.
		cg_->kbfree(index);		// 03/05/07 AM.
      return con;					// 03/05/07 AM.
      }
   if (index)	// 03/05/07 AM.
		cg_->kbfree(index);	// 03/05/07 AM.
   index = subs;
   }

if (!index)	// Sanity check.	// 03/05/07 AM.
	return 0;						// 03/05/07 AM.

con = dict_find_index(index, name);
if (con && con->id != index->id)	// Don't clobber what you're returning.	// 03/05/07 AM.
	cg_->kbfree(index);		// 03/05/07 AM.
return con;						// 03/05/07 AM.
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

XCON_S *CGDICT::dict_find_index(
	_TCHAR *name
	)
{
XCON_S *con = cg_->qkbm_->Con(cg_->dict_ALPHA);	// FIX.	// 03/21/07 AM.
XCON_S *cret = dict_find_index(con, name);		// FIX.	// 03/21/07 AM.
cg_->kbfree(con);											// FIX.	// 03/21/07 AM.
return cret;												// FIX.	// 03/21/07 AM.
}


/**************************************************
*						DICT_FIND_WORD
* FUN:	dict_find_word
* SUBJ:	Find word if present in system.
* CR:		10/26/95 AM.
* RET:	word - Word concept, if present.
*
**************************************************/

CON_ID 
CGDICT::dict_find_word(
	_TCHAR *name
	)
{
if (str_empty(name))								// 02/20/07 AM.
	return 0;										// 02/20/07 AM.

STR_ID sid = cg_->qkbm_->getStrx(name);	// 02/20/07 AM.
if (!sid)											// 02/20/07 AM.
	return 0;										// 02/20/07 AM.

return cg_->qkbm_->getXsym(sid);				// 02/20/07 AM.
}

/**************************************************
*						DICT_GET_WORD
* FUN:	dict_get_word
* SUBJ:	Find word if present, else make it.
* CR:		06/29/03 AM.
* RET:	word - Word concept, if present.
*
**************************************************/

XCON_S * 
CGDICT::dict_get_word(
	_TCHAR *name,	// Word string to look up.
	bool &dirty	// If KB has changed.
	)
{
if (str_empty(name))	// 02/20/07 AM.
	return 0;	// 02/20/07 AM>

STR_ID sid = cg_->qkbm_->sym_get(name,dirty);	// 02/28/07 AM.
if (!sid)											// 02/20/07 AM.
	return 0;										// 02/20/07 AM.

// If string already has a dict concept, can't add it.
CON_ID sym_cid;									// 02/20/07 AM.
if (sym_cid = cg_->qkbm_->getXsym(sid))	// 02/20/07 AM.
	return cg_->qkbm_->Con(sym_cid);			// 02/20/07 AM.

CON_ID wid = dict_add_word_con(sid,name,dirty);	// 06/29/03 AM.
return cg_->qkbm_->Con(wid);					// 02/20/07 AM.
}

/**************************************************
*						DICT_GET_WORD_ID
* FUN:	dict_get_word_id
* SUBJ:	Find word if present, else make it. [VARIANT]
* CR:		02/20/07 AM.
* RET:	word - Word concept, if present.
*
**************************************************/

CON_ID 
CGDICT::dict_get_word_id(
	_TCHAR *name,	// Word string to look up.
	bool &dirty	// If KB has changed.
	)
{
if (str_empty(name))	// 02/20/07 AM.
	return 0;	// 02/20/07 AM>

STR_ID sid = cg_->qkbm_->sym_get(name,dirty);	// 02/28/07 AM.
if (!sid)											// 02/20/07 AM.
	return 0;										// 02/20/07 AM.

// If string already has a dict concept, can't add it.
CON_ID sym_cid;									// 02/20/07 AM.
if (sym_cid = cg_->qkbm_->getXsym(sid))	// 02/20/07 AM.
	return sym_cid;								// 02/20/07 AM.

return dict_add_word_con(sid,name,dirty);	// 06/29/03 AM.
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

CON_ID 
CGDICT::dict_find_word_lc(
	_TCHAR *name
	)
{
if (!name || !*name)
	return 0;
s_to_lower(name);

if (str_empty(name))	// 02/20/07 AM.
	return 0;	// 02/20/07 AM>

STR_ID sid = cg_->qkbm_->getStrx(name);	// 02/20/07 AM.
if (!sid)											// 02/20/07 AM.
	return 0;										// 02/20/07 AM.

// See if string has a dict concept.
return cg_->qkbm_->getXsym(sid);	// 02/20/07 AM.
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

XCON_S *CGDICT::dict_first()
{
XCON_S *root  = cg_->qkbm_->Con(cg_->dict_ALPHA);			/* Root of index tree. */
XCON_S *subs = 0;
if (!root)
	return 0;	// Error. No dict hierarchy.
subs = dict_subtree(root, true);
cg_->kbfree(root);
return subs;
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

XCON_S *CGDICT::dict_next(XCON_S *wordcon)
{
if (!wordcon)
	return 0;
if (wordcon->kind != cXWORD)
	{
	// Assume it's an index.
	if (wordcon->id == cg_->dict_ALPHA)	// FIX.	// 03/21/07 AM.
		return dict_subtree(wordcon,true);
	else
		return dict_next(wordcon,false);
	}
if (wordcon->next)
	return cg_->qkbm_->Con(wordcon->next);
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

XCON_S *CGDICT::dict_next(XCON_S *con, bool rootflag)
{
if (!con)
	return 0;

XCON_S *tmp, *found;
CON_ID cid;
XCON_S *nxt;	// 03/05/07 AM.

// If I'm a word concept, then go to next.
if (con->kind == cXWORD)
	{
	if (con->next)
		return cg_->qkbm_->Con(con->next);
	}

// I'm an index concept
else
	{
	// Todo: Don't free the given con, but free the other copies as built.// 02/20/07 AM.
	// Traverse me and rest of my list.
	nxt = con;	// 03/05/07 AM.
	while (tmp = nxt)
		{
		// Look in subtree for first wordcon.
		if (found = dict_subtree(tmp,true))
			{
			if (tmp->id != con->id)	// 03/05/07 AM.
				cg_->kbfree(tmp);
			return found;
			}
		nxt = cg_->qkbm_->Con(tmp->next);
		if (tmp->id != con->id)	// 03/05/07 AM.
			cg_->kbfree(tmp);
		}
	}

// Didn't find anything down and right, so try going up.
tmp = con;	// 03/05/07 AM.
for (;;)
	{
	cid = tmp->up;
	if (tmp->id != con->id)	// Don't clobber given concept.	// 03/05/07 AM.
		cg_->kbfree(tmp);	// 03/05/07 AM.
	if (!cid)
		return 0;	// Hit root of kb.  Error.
	if (cid == cg_->dict_ALPHA)	// 03/05/07 AM.
		return 0;	// Couldn't go up and right, hit top of dict.

	tmp = cg_->qkbm_->Con(cid);	// 03/05/07 AM.
	if (tmp->next)
		{
		nxt = cg_->qkbm_->Con(tmp->next);	// 03/05/07 AM.
		cg_->kbfree(tmp);							// 03/05/07 AM.
		found = dict_next(nxt,false);
		if (found && found->id != nxt->id)	// 03/05/07 AM.
			cg_->kbfree(nxt);						// 03/05/07 AM.
		return found;	// 03/05/07 AM.
		}
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

XCON_S *CGDICT::dict_subtree(XCON_S *con, bool rootflag)
{
if (!con)
	return 0;

XCON_S *tmp, *found, *nxt;
CON_ID cid;

// If I'm a word concept, then done.
if (con->kind == cXWORD)
	return con;

// I'm an index concept
// Look down.
if (cid = con->dn)
	{
	tmp = cg_->qkbm_->Con(cid);
	if (tmp->kind == cXWORD)
		return tmp;
	if (found = dict_subtree(tmp,false))
		{
		if (tmp->id != found->id)	// 03/05/07 AM.
			cg_->kbfree(tmp);			// 03/05/07 AM.
		return found;
		}
	cg_->kbfree(tmp);					// 03/05/07 AM.
	}

// Look right, if head of list and not "root".
if (!rootflag && !con->prev)
	{
	nxt = cg_->qkbm_->Con(con->next);
	while (tmp = nxt)	// 03/05/07 AM.
		{
		if (found = dict_subtree(tmp,true))
			{
			if (found->id != tmp->id)	// 03/05/07 AM.
				cg_->kbfree(tmp);			// 03/05/07 AM.
			return found;
			}
		nxt = cg_->qkbm_->Con(tmp->next);	// 03/05/07 AM.
		cg_->kbfree(tmp);							// 03/05/07 AM.
		}
	}

return 0;
}

/**********************     END OF FILE         ***************************/
