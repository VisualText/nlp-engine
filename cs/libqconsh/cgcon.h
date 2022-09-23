/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CGCON.H
* FILE:	include/API/cgcon.h.
* CR:		02/19/07 AM.
* SUBJ:	Class for CG KBMS concept management.
*
*******************************************************************************/

#ifndef CGCON_H_
#define CGCON_H_


#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <stdlib.h>
#include <iostream>
#include <fstream>
#endif

// Maximum depth of hierarchy.	// 03/08/07 AM.
#define HIER_MAX_DEPTH 8192

// Forward reference.
class CG;

/********************************************
* CLASS:	CGCON
* CR:		02/19/07 AM.
* SUBJ:	API Class for managing the VisualText KBMS attributes.
********************************************/

class LIBQCONSH_API CGCON
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	CGCON(										// Default constructor.
		CG *cg
		);
	~CGCON();									// Destructor.

	// Access
//	bool  getXyz;

	// Modify
//	void setXyz(_TCHAR *);

	// General functions


	CON_ID con_add_alpha(
		XCON_S *hier,
		_TCHAR *name
		);
	CON_ID con_add_basic(
		_TCHAR *name,				/* Descriptive name for new concept.	*/
		XCON_S *up					/* Hierarchical parent of new concept.	*/
		);
	bool con_add_first(XCON_S *con, XCON_S *hier);		// 05/08/99 AM.

	CON_ID con_add_nh(
		_TCHAR *name				/* Descriptive name for new concept.	*/
		);

	CON_ID con_add_nth(
		XCON_S *hier,
		_TCHAR *name,
		int pos			// position to insert into.  if = 0, at end of list.
		);
	CON_ID con_add_nth(			// 05/13/99 AM.
		XCON_S *hier,
		XCON_S *con,
		int pos			// position to insert into.  if = 0, at end of list.
		);
	CON_ID con_add_proxy(
		CON_ID gen_cid				/* The concept that proxy represents. */
		);
	CON_ID con_add_word_first(
		_TCHAR *name,				/* Descriptive name for new concept.	*/
		XCON_S *hier				/* Hierarchical parent of new concept.	*/
		);
	CON_ID con_child_alpha(
		XCON_S *hier,
		_TCHAR *name,
		/*UP*/
		XCON_S **pre
		);

	// Find concept in a list of concepts.
	XCON_S *con_find(	// 02/20/07 AM.
		XCON_S *con,
		_TCHAR *name
		);

	// Find named node in a list of nodes.
	XCON_S *node_find(	// 02/20/07 AM.
		XCON_S *node,
		_TCHAR *name
		);

	CON_ID id_get_child(CON_ID,_TCHAR*);
	XCON_S *path_to_con(_TCHAR*);
	XCON_S *get_component(XCON_S*,_TCHAR*);

	//XCON_S *c_cg_concept();
	CON_ID con_nth_proxy(XCON_S *con,long ord);	// Todo: REIMP.	// 02/14/07 AM.
	CON_ID node_owner(XCON_S *);                               // 10/09/05 AM.

	// CHANGING TO NONSTATIC.	// 02/19/07 AM.
	CON_ID con_find_child(XCON_S *hier,_TCHAR *name);			// 02/19/07 AM.
//	CON_ID con_get_child(XCON_S *hier,_TCHAR *name);	// Todo: REIMP.
	_TCHAR *con_kind_str(XCON_S *con);

	_TCHAR *con_str_pret(XCON_S *);
	XCON_S *nc(XCON_S *proxy);
	long phrase_len(XCON_S *);
	CON_ID con_nth(XCON_S *,long);
	long con_ord(XCON_S *);                                  // 10/09/05 AM.
	CON_ID con_last(XCON_S *);
	_TCHAR *con_to_path(XCON_S *,/*UP*/_TCHAR*);
	void con_to_path_r(XCON_S *,/*UP*/_TCHAR*&);
	bool con_remove(CON_ID);
	CON_ID con_parent(XCON_S *);

	
	// Make phrase. Name 1st node with dictionary concept.
	CON_ID make_phrase_using_dict(XCON_S *owner, _TCHAR *name);			// 02/20/07 AM.

	// Rename node using (and making if need be) a dictionary concept.
	CON_ID phrase_rename_node(XCON_S *phr, _TCHAR *oldname, _TCHAR *newname);	// 02/21/07 AM.
	CON_ID phrase_rename_node(XCON_S *phr, int pos, _TCHAR *newname);				// 02/21/07 AM.

	//////////////////////////////////////////////////////
	////// MOVED HERE FROM CONSH_KB.CPP/H	// 02/19/07 AM.
	//////////////////////////////////////////////////////

	void con_add_root();

	bool con_stationary(CON_ID con);

	CON_ID phr_add_nth(			// 05/04/99 AM.
		XCON_S *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
		_TCHAR *name,
		int pos			// position to insert into.  if = 0, at end of list.
		);

	CON_ID phr_add_nth(	// 05/05/99 AM.
		XCON_S *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
		XCON_S *node,		// Node to attach.
		int pos			// position to insert into.  if = 0, at end of list.
		);

	CON_ID insert_node(	// 05/05/99 AM.
		XCON_S *owner,		// Concept that OWNS the phrase. NOT 1st phrase node.
		XCON_S *node,		// Node to attach.
		XCON_S *n_pos		// position to insert before.  if = 0, at end of list.
		);

	CON_ID CGCON::con_add_internal(	// 02/26/07 AM.
		_TCHAR *name,
		PTR_ID attrs,
		CON_ID up,
		CON_ID dn,
		CON_ID prev,
		CON_ID next,
		CON_ID phrases
		);

	bool rm_proxy(	// 05/04/99 AM.
		XCON_S *proxy
		);

//	int compare_nodes(const void *arg1, const void *arg2);

	bool sort_phrase(
		XCON_S *con			// The concept owning the phrase.
		);

	bool sort_childs(
		XCON_S *con			// Parent concept.
		);


	CON_ID con_reduce_sublist(_TCHAR *name, XCON_S *start, XCON_S *end);	// 02/26/07 AM.

	bool con_detach(XCON_S *con);			// 03/04/07 AM.
	bool con_rm_hier(XCON_S *con);		// 03/04/07 AM.
	bool con_rm_children(CON_ID cid);	// 03/04/07 AM.
	
	// Clear tracked path from depth to lpath.				// 03/08/07 AM.
	bool clear_path(int depth, XCON_S **path, _TCHAR **spath, int &lpath);	// 03/08/07 AM.

	//////////////////////////////////////////////////////

	// Class functions



private:
	CG *cg_;

public:
	// OPT: TRACK THE LAST CONCEPT PATH FETCHED.		// 03/08/07 AM.
	// Note that root always kept in mem this way.	// 03/08/07 AM.
	// NOTE: ZERO-BASED COUNTING.							// 03/08/07 AM.
	XCON_S *path_[HIER_MAX_DEPTH];	// Concept.		// 03/08/07 AM.
	_TCHAR *spath_[HIER_MAX_DEPTH];	// String.		// 03/08/07 AM.
	int lpath_;								// Depth.		// 03/08/07 AM.

	// OPT: Track the last used attr slot concept.	// 03/08/07 AM.
	// Note: Can use to track any other path.			// 03/08/07 AM.
	XCON_S *path2_[HIER_MAX_DEPTH];	// Concept.		// 03/08/07 AM.
	_TCHAR *spath2_[HIER_MAX_DEPTH];	// String.		// 03/08/07 AM.
	int lpath2_;							// Depth.		// 03/08/07 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
private:
	static int count_;						// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
