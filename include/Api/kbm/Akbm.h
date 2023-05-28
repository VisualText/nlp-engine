/*******************************************************************************
Copyright (c) 2002-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AKBM.H
* FILE:	include/API/kbm/Akbm.h
* CR:		06/11/02 AM.
* SUBJ:	API for VisualText KB Manager module.
* NOTE:	Manage the formerly global variables bound to KB concepts.
*
*******************************************************************************/

#ifndef AKBM_H_
#define AKBM_H_

#include "kbm/st.h"	// 10/30/06 AM.

typedef enum {
   DICT_CALL_DEFAULT,
   DICT_CALL_FILE
} DICTCALL_TYPE;

class DICT_CALL {
  public:
    DICT_CALL() {
		type = DICT_CALL_DEFAULT;
		file = "";
		line = 0;
    }
    DICTCALL_TYPE type;
    std::string file;
	int line;
};

// Forward reference.
class ACON;
class AST;
class ASYM;
class APTR;
class ALIST;
class CG;

/********************************************
* CLASS:	AKBM
* CR:		06/11/02 AM.
* SUBJ:	API Class for managing the VisualText KBM Module.
********************************************/

class LIBKBM_API AKBM
{
	friend class ACON;
	friend class AST;
	friend class ASYM;
	friend class APTR;
	friend class ALIST;
	friend class CG;

public:
	AKBM(		// Default constructor.
		);

	~AKBM();	// Destructor.

	// Access.
	void *getSt();
	void *getCon();
	void *getPtr();


	void kbm_vars(
		CON *cg_concept,
		CON *cg_phrase,		// 04/16/99 AM.
		CON *cg_atom,			// 05/04/99 AM.
		CON *dict_alpha,
		CON *dict_unknown,
		CON *id_action,
		CON *id_rest,
		CON *nlp_punct,
		CON *nlp_wht
		);
	void kbm_clean();


	ACON *getACON();	// Get concept table manager.
	AST *getAST();		// Get string table manager.
	ASYM *getASYM();	// Get symbol table manager.
	APTR *getAPTR();	// Get pointer table manager.

	ALIST *getALIST();	// Get list manager.


	// Update.
	void setSt(void *);
	void setCon(void *);
	void setPtr(void *);

	void setACON(ACON*);
	void setAST(AST*);
	void setASYM(ASYM*);
	void setAPTR(APTR*);

	void setALIST(ALIST*);

	// Class Members.
	LIST *find_name(
		_TCHAR *name,
		/*UP*/
		LIST **end
		);
	void find_name_rec(
		_TCHAR *name,
		CON *root,
		/*UP*/
		LIST **list,
		LIST **end
		);

	PTR *attr_get_h(CON*,_TCHAR*);
	bool attr_is_sys(PTR*);
	bool attr_rename(PTR*,_TCHAR*);
	bool attrs_rm(CON*,/*UP*/bool&);
	PTR *attr_add_by_name(CON*,_TCHAR*,_TCHAR*);				// 05/03/99 AM.
	PTR *attr_add_by_name(CON*,_TCHAR*,long long);					// 05/03/99 AM.
	PTR *attr_add_by_name(CON*,_TCHAR*,float);				// 08/16/01 AM.
	PTR *attr_add_by_name(CON*,_TCHAR*,CON*);					// 08/11/00 AM.
	bool attr_rm_by_name(CON*, _TCHAR*);
	bool attr_has_n_val(CON*,_TCHAR*,long long);
	bool
	attr_rm(							// 07/12/99 AM.
		CON *con,
		PTR *ptr
		);
	PTR *
	attr_add_new(
		CON *con,
		CON *slot,
		PTR *vals);	/* May be NULL */
	PTR *
	attr_add_n(
		CON *con,
		CON *slot_con,
		long num
		);
	PTR *
	attr_add_c(
		CON *con,
		CON *slot_con,
		CON *val
		);

	PTR *
	attr_add_val(
		CON *con,
		CON *slot_con,
		PVAL val,
		PKIND kind
		);

	PTR *
	attr_find(CON *con, _TCHAR *s_slot);				// 08/12/99 AM.
	PTR *
	attr_find_h(CON *con, _TCHAR *name);				// 07/12/99 AM.
	PTR *
	attr_prefind_h(CON *con, _TCHAR *name, /*UP*/ bool &first);	// 07/12/99 AM.
	PTR *
	attr_prefind_h(CON *con, PTR *ptr, /*UP*/ bool &first);		// 07/12/99 AM.
	PTR *
	attr_find_slot(CON *con, CON *slot_con);
	PTR *
	attr_find_slot_0(CON *con, CON *slot_con);
	PTR *
	attr_get(CON *con, CON *slot);
	PTR *
	attr_get(CON *con, _TCHAR *s_slot);				// 05/03/99 AM.
	CON *
	attr_get_c(CON *con, CON *slot);
	long
	attr_get_n(CON *con, CON *slot);
	bool
	attr_get_ns(CON *con, CON *slot, int len, /*UP*/ long *arr);

	SYM *
	attr_get_s(CON *con, CON *slot);
	PTR *
	attr_get_slot(CON *con, CON *slot_con);
	void
	attrs_pret(
		PTR *attrs,
		//FILE *out,		// 04/20/99 AM.
		std::_t_ostream *out,		// 04/20/99 AM.
		_TCHAR *indent		/* String for indenting. */
		);
	CON *attr_get_c();

	/******/

	PTR *
	attr_a_add_end(CON *con);
	PTR *
	attr_as_add_end(
		CON *con,		/* Concept to add attribute to. */
		CON *slot_con	/* Slot concept.				*/
		);
	PTR *
	attr_asv_add_end(
		CON *con,		/* Concept to add attribute to. */
		CON *slot_con	/* Slot concept.				*/
		);
	PTR *
	attr_s_add(
		PTR *attr,
		CON *slot_con		/* Concept representing slot, if any.	*/
		);
	PTR *
	attr_v_add_end(PTR *slot);

	_TCHAR *		// 07/05/99 AM.
	attrs_name(
		PTR *attrs
		);
	PTR *		// 07/05/99 AM.
	attrs_vals(
		PTR *attrs
		);

		bool															// 05/11/00 AM.
	rm_one_val(
		PTR *attr,
		PTR *val
		);
	bool																// 05/17/00 AM.
	attr_rm_val_by_name(
		CON *con,
		_TCHAR *attr_s,
		_TCHAR *val_s
		);
	bool
	attr_rm_val_by_name(															// 05/19/00 AM.
		CON *con,
		_TCHAR *attr_s,
		long long val
		);
	bool
	attr_rm_val_by_name(															// 06/11/03 AM.
		CON *con,
		_TCHAR *attr_s,
		float val
		);
	bool
	attr_rm_val_by_concept(															// 06/03/02 Dd.
		CON *con,
		_TCHAR *attr_s,
		CON *val
		);

	bool dict_add_punct();
	CON *dict_add_unknown(_TCHAR*);
	bool dict_add_white();
	CON *dict_add_word(_TCHAR*);
	CON *dict_add_word_dict(_TCHAR*,DICT_CALL);
	CON *dict_add_word_con(ASYM*,bool&,DICT_CALL);	// 06/29/03 AM.
	CON *dict_find_index(_TCHAR*);
	CON *dict_find_index(CON*,_TCHAR*);
	bool dict_rm_word(_TCHAR *str);				// 07/17/00 AM.
	CON *dict_find_word(_TCHAR *name);
	CON *dict_get_word(_TCHAR*,bool&);			// 06/29/03 AM.
	CON *dict_get_word(_TCHAR*,bool&,DICT_CALL);
	CON *dict_find_word_lc(_TCHAR *name);
	CON *dict_first();							// 03/21/03 AM.
	CON *dict_next(CON*);						// 03/21/03 AM.
	CON *dict_next(CON*,bool);					// 03/21/03 AM.
	CON *dict_subtree(CON*,bool);				// 03/21/03 AM.


	// General Functions.


	ACON *acon_;
	AST *ast_;
	ASYM *asym_;
	APTR *aptr_;

	ALIST *alist_;
private:
	// For now, storing "hard-wired" initial table segments here.
	void *st_;	// Store string table segment.
	void *con_;	// Store concept table segment.
	void *ptr_;	// Store ptr table segment.


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
