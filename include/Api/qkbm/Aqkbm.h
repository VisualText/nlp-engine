/*******************************************************************************
Copyright (c) 2007-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AQKBM.H
* FILE:	include/API/qkbm/Aqkbm.h
* CR:		02/05/07 AM.
* SUBJ:	API for VisualText KB Manager based on QDBM database system.
*
*******************************************************************************/

#ifndef AQKBM_H_
#define AQKBM_H_

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "qkbm/defs.h"
#include "qkbm/xptr_s.h"
#include "qkbm/xcon_s.h"

#define MAX_PHRASE_ELTS 10000


// Fetch string for slot pointer.
// OPT: Getting concepts by name needs this extra fetch.
//	Can avoid it by searching for slot as a concept id.
// 02/16/07 AM. Slot can be CONCEPT OR STRING.
#define slot_sid(slot) (((slot)->kind == pXSTR) ? (slot)->v.xstr : con_sid((slot)->v.xstr))

// Forward reference.
class MGR;
class STRX;
class XSTR;
class XPTR;
class XCON;
class XSYM;

/********************************************
* CLASS:	AQKBM
* CR:		02/05/07 AM.
* SUBJ:	API Class for managing the VisualText QKBM Module.
********************************************/

class LIBQKBM_API AQKBM
{
//	friend class ACON;

public:
	AQKBM(		// Default constructor.
		);

	~AQKBM();	// Destructor.

	// Access.

	// Update.

	// General functions.

		// Need create database.
		// Need temp and safe copies of the database.

		// See if a kbms exists at the given path.   // 03/04/07 AM.
		bool iskbms(_TCHAR *path);                   // 03/04/07 AM.

		bool openkbms(_TCHAR *path);	// Open the databases of the kbms.
		bool closekbms();	// Close the kbms.

		bool initkbms();	// Initialize existing kbms.	// 03/04/07 AM.
		bool resetkbms();	// Reset counters/tracking for kbms. If newly created or emptied.
		
		STR_ID getStrings();	// Get strings counter.
		
		void kbfree(void *obj);	// FREE UP A QDBM OBJECT.

		// STRING DBS.
		STR_ID addString(_TCHAR *str);	// Add string to kbms.
		STR_ID getStrx(_TCHAR *str);		// Get string's index.
		_TCHAR *getXstr(STR_ID index);	// Get index's string.
		
		// SYM DB. For indexing between string and concept.(eg dict)	// 02/19/07 AM.
		bool addXsym(STR_ID sid, CON_ID cid);	// Relate str and con.	// 02/19/07 AM.
		CON_ID getXsym(STR_ID sid);	// Get concept for str.				// 02/19/07 AM.
		bool sym_rm(STR_ID sid);		// Remove reference to concept.	// 02/20/07 AM.
		
		// POINTER DB.
		PTR_ID addXptr(int val, XPKIND kind, PTR_ID next);
		XPTR_S *addXptr(int val,XPKIND kind, PTR_ID next, /*UP*/ PTR_ID &pid);
		PTR_ID addXptr(float val, PTR_ID next);
		XPTR_S *addXptr(XPTR_S *ptr, /*UP*/ PTR_ID &pid);	// 02/15/07 AM.
		bool ptr_update(PTR_ID pid, XPTR_S *ptr);
		bool ptr_list_rm(PTR_ID pid, XPTR_S *ptr = 0);			// 02/16/07 AM.
		bool getXptr(PTR_ID index, /*DU*/ XPTR_S *&ptr);
		CON_ID ptr_get_con(PTR_ID index);
		PTR_ID ptr_get_ptr(PTR_ID index);
		PTR_ID ptr_get_next(PTR_ID index);
		STR_ID ptr_get_str(PTR_ID index);	// Copy of db str returned.	// 02/14/07 AM.
		_TCHAR *ptr_get_str_db(PTR_ID index);	// Actual db str returned.	// 02/14/07 AM.
		bool ptr_get_str(PTR_ID index, /*UP*/ _TCHAR *str);	// User supplied buffer filled.
		bool ptr_get_num(PTR_ID idx, /*UP*/ long &num);
		bool ptr_get_flt(PTR_ID idx, /*UP*/ float &flt);

		bool ptr_val_as_str(PTR_ID ptr, _TCHAR *buf);
		bool s_to_pkind(_TCHAR *str, /*UP*/ XPKIND *pk);
		bool s_to_pval(_TCHAR *str,XPKIND pkind,/*UP*/ XPVAL *pv);

		// CONCEPT DB.
		CON_ID addXcon(int val, XCKIND kind,
				int flags,
				PTR_ID attrs,
				CON_ID up,
				CON_ID dn,
				CON_ID prev,
				CON_ID next,
				CON_ID atom,
				PTR_ID phrases
				);
		CON_ID addXcon(XCON_S *con);	// 02/14/07 AM.
		bool con_update(CON_ID cid, XCON_S *con);
		bool getXcon(CON_ID index, /*DU*/ XCON_S *&con);
		XCON_S *Con(CON_ID index);	// The good old fashioned way.	// 02/14/07 AM.
		PTR_ID getXcon_phrases(CON_ID index);
		PTR_ID con_get_attrs(CON_ID index);
		STR_ID con_get_name(CON_ID index);	// Same as con_str.
		CON_ID getXcon_atom(CON_ID index);
		CON_ID get_up(CON_ID index);
		CON_ID get_dn(CON_ID index);
		CON_ID get_prev(CON_ID index);
		CON_ID get_next(CON_ID index);
		bool con_move_left(CON_ID cid);
		bool node_move_left(CON_ID nid);
		bool node_move_left(char *id_s);
		bool con_move_right(CON_ID cid);
		bool node_move_right(CON_ID nid);
		bool node_move_right(char *id_s);
		

		// MACHINERY FOR PHRASES.
		
		// Make and return first node of phrase.
		CON_ID make_phrase(XCON_S *c_owner, CON_ID i_target);	// 02/14/07 AM.
		CON_ID make_phrase(XCON_S *c_owner, XCON_S *node);		// 02/20/07 AM.

		// Add a node to last node of phrase.
		CON_ID extend_phrase(XCON_S *n_last, CON_ID i_target);	// 02/14/07 AM.
		CON_ID extend_phrase(XCON_S *n_last, XCON_S *node);		// 02/20/07 AM.

		// Add new node at start of phrase.
		CON_ID phrase_add_start(XCON_S *c_owner, CON_ID i_target);	// 02/14/07 AM.
		CON_ID phrase_add_start(XCON_S *c_owner, XCON_S *node);		// 02/20/07 AM.

		// Insert node into phrase after given node.
		// (Subsumes some of the above.)
		CON_ID phrase_insert_after(XCON_S *pre, CON_ID i_target);	// 02/19/07 AM.
		CON_ID phrase_insert_after(XCON_S *pre, XCON_S *node);		// 02/20/07 AM.

		bool phrase_detach_node(XCON_S *node);	// Detach node from phrase.	// 02/21/07 AM.
		bool phrase_rm_node(XCON_S *node);	// Rm a single node from its phrase.  Fix up phrase.
		bool node_rm(XCON_S *node);	// Remove node object from kb, Ignore phrase.	// 02/21/07 AM.

		// Primitives for internal use, eg in editing.
		bool attach_phrase(PTR_ID pid, XPTR_S *ptr, CON_ID nid);
		XCON_S *detach_phrase(XCON_S *con, /*UP*/ XPTR_S *&ptr);

		// Todo: implement these.
		CON_ID con_find_in_list(CON_ID cid, _TCHAR *name);	// Find first.
		CON_ID con_first_in_list(CON_ID cid);	// Get first con in list.
		CON_ID phr_add_nth(CON_ID owner, _TCHAR *name, int pos);	//
		CON_ID phr_add_nth(CON_ID phr, CON_ID con, int pos);	// 1st node of phrase.


		// MIX AND MATCH
		CON_ID con_phrase(CON_ID cid);
		CON_ID con_phrase(XCON_S *con);
		CON_ID phrase_con(CON_ID phr);
		
		STR_ID sym_get(_TCHAR *str, bool &dirty);
		bool con_set_name(CON_ID con, STR_ID name, bool &dirty);

		bool find_attr_val(CON_ID con, _TCHAR *attr, _TCHAR *val);
		PTR_ID attr_find_val_by_name(PTR_ID attrs, _TCHAR *name);
		bool attr_find_str(CON_ID con, _TCHAR *name, /*UP*/ _TCHAR *str);	// 02/09/07 AM.
		bool attr_find_str_db(CON_ID con, _TCHAR *name, /*UP*/ _TCHAR *&str);	// 02/14/07 AM.
		STR_ID attr_find_str(CON_ID con, _TCHAR *name);	// 02/14/07 AM.
		bool attr_find_num(CON_ID con, _TCHAR *name, /*UP*/ long &num);
		bool attr_find_flt(CON_ID con, _TCHAR *name, /*UP*/ float &flt);
		bool attr_find_con(CON_ID con, _TCHAR *name, /*UP*/ CON_ID &cid);

		// OPT:	If absent, make an empty attr slot.					// 02/15/07 AM.
		//			If present, find last value for quick append.	// 02/15/07 AM.
		XPTR_S *attr_get_last_ptr(XCON_S *con, XCON_S *slot,/*UP*/PTR_ID &pid);	// 02/15/07 AM.
		XPTR_S *attr_get_last_ptr(XCON_S *con, STR_ID slot_sid,/*UP*/PTR_ID &pid);	// 02/16/07 AM.


	// Attach new value to given ptr representing the end of concept's attr.
		XPTR_S *attr_add_val(	// 02/15/07 AM.
			XPTR_S *last,
			PTR_ID last_pid,
			XPVAL &val,
			XPKIND kind,
			/*UP*/
			PTR_ID &val_pid
			);
		XPTR_S *attr_add_slot(XCON_S *con,XPTR_S *hlast,PTR_ID hlast_pid, CON_ID slot_cid,/*UP*/ PTR_ID &slot_pid);
		XPTR_S *attr_add_slot_h(XCON_S *con,XPTR_S *hlast, PTR_ID hlast_pid, STR_ID slot_sid,/*UP*/ PTR_ID &hpid);

		XPTR_S *attr_prefind_h(XCON_S *con, _TCHAR *name,
			/*UP*/ bool &first, PTR_ID &pre_pid, PTR_ID &hpid, PTR_ID &spid);
		XPTR_S *attr_prefind_h(XCON_S *con, PTR_ID ptr, /*UP*/ bool &first, PTR_ID &pre_pid);
		bool attrs_rm(CON_ID,/*UP*/bool&);

		PTR_ID attr_find(CON_ID con, _TCHAR *s_slot);
		XPTR_S *attr_find_slot(XCON_S *con, CON_ID slot_cid, /*UP*/ PTR_ID &pid);
		XPTR_S *attr_find_slot_s(XCON_S *con, STR_ID slot_sid, /*UP*/ PTR_ID &pid); // 02/17/07 AM.
		XPTR_S *attr_find_h(XCON_S *con, _TCHAR *name,/*UP*/ PTR_ID &pid);
		XPTR_S *attr_get_h(XCON_S *con,_TCHAR *name, /*UP*/ PTR_ID &pid);
		XPTR_S *attr_get_slot(XCON_S *con, CON_ID slot_cid);

		PTR_ID con_attr_find(CON_ID con, _TCHAR *name);

		bool con_rm(XCON_S *con, char *cid_s);
		bool con_rm(CON_ID cid);
		bool con_rm(XCON_S *con);	// 03/04/07 AM.
		bool con_rm_attrs(XCON_S *con);
		bool con_rm_phrases(XCON_S *con);

		// Use con_set_name.
//		bool con_rename(CON_ID cid, _TCHAR *newname);

		// Change the target concept that given node represents.
		// Terms: node is the proxy, target is the concept that it represents.
		bool node_set_target(XCON_S *node, CON_ID cid, bool &dirty);

		///////////////////////////////////////////////////////
		// FROM AKBM ....
	void kbm_vars(
		CON_ID cg_concept,
		CON_ID cg_phrase,		// 04/16/99 AM.
		CON_ID cg_atom,			// 05/04/99 AM.
		CON_ID dict_alpha,
		CON_ID dict_unknown,
		CON_ID id_action,
		CON_ID id_rest,
		CON_ID nlp_punct,
		CON_ID nlp_wht
		);
	void kbm_clean();

	// Class Members.
	LIST *find_name(
		_TCHAR *name,
		/*UP*/
		LIST **end
		);
	void find_name_rec(
		_TCHAR *name,
		CON_ID root,
		/*UP*/
		LIST **list,
		LIST **end
		);



		///////////////////////////////////////////////////////

	void con_tab_pretty(
		//FILE *out			// 04/20/99 AM.
		_t_ostream *out		// 04/20/99 AM.
		);
	int con_count();


	// General Functions
	_TCHAR *con_str(XCON_S *con);	// REIMP	// 02/14/07 AM.
	_TCHAR *con_str(CON_ID cid);	// REIMP	// 02/14/07 AM.
	STR_ID con_sid(XCON_S *con);	// 02/15/07 AM.
	STR_ID con_sid(CON_ID cid);	// Same as con_get_name // 02/15/07 AM.
	STR_ID node_sid(XCON_S *con);	// 02/20/07 AM.

		///////////////////////////////////////////////////////

	// Class functions.

private:
	MGR *mgr_;		// Manager database.
	STRX *strx_;	// StringX database.	 (string -> index)
	XSTR *xstr_;	// XString database.  (index  -> string)
	XPTR *xptr_;	// Pointer database.
	XCON *xcon_;	// Concept database.
	XSYM *xsym_;	// Symbol-table database.	// 02/19/07 AM.

	// Count objects returned vs freed.
	long balance_;	// 03/13/07 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};

	// Access.
//	void *getSt();
//	void *getCon();
//	void *getPtr();

/*
	void AKBM::kbm_vars(
		CON_ID cg_concept,
		CON_ID cg_phrase,		// 04/16/99 AM.
		CON_ID cg_atom,			// 05/04/99 AM.
		CON_ID dict_alpha,
		CON_ID dict_unknown,
		CON_ID id_action,
		CON_ID id_rest,
		CON_ID nlp_punct,
		CON_ID nlp_wht
		);
	void kbm_clean();


	ACON_ID getACON();	// Get concept table manager.
	AST *getAST();		// Get string table manager.
	ASTR_ID getASYM();	// Get symbol table manager.
	APTR_ID getAPTR();	// Get pointer table manager.

	ALIST *getALIST();	// Get list manager.
*/


	// Update.
//	void setSt(void *);
/*
	void setCon(void *);
	void setPtr(void *);

	void setACON(ACON_ID);
	void setAST(AST*);
	void setASYM(ASYM*);
	void setAPTR(APTR_ID);

	void setALIST(ALIST*);
*/

	// Class Members.


	// General Functions.


//	ACON_ID acon_;
//	AST *ast_;
//	ASTR_ID asym_;
//	APTR_ID aptr_;

//	ALIST *alist_;





#endif
/*************************      END OF FILE      ******************************/
