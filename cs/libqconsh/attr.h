/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CGATTR.H
* FILE:	include/API/cg.h.
* CR:		04/27/99 AM.
* SUBJ:	API for VisualText KBMS.
*			KBMS = knowledge base management system. A hierarchical data base.
*			CGATTR = Conceptual Grammar. A knowledge representation scheme.
*
*******************************************************************************/

#ifndef CGATTR_H_
#define CGATTR_H_


#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <stdlib.h>
#include <iostream>
#include <fstream>
#endif

// Forward reference.
class CG;

/********************************************
* CLASS:	CGATTR
* CR:		02/16/07 AM.
* SUBJ:	API Class for managing the VisualText KBMS attributes.
********************************************/

class LIBQCONSH_API CGATTR
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	CGATTR(										// Default constructor.
		CG *cg
		);
	~CGATTR();									// Destructor.

	// Access
//	bool  getXyz;

	// Modify
//	void setXyz(_TCHAR *);

	// Class functions


	bool attr_is_sys(XPTR_S*);
	bool attr_rename(PTR_ID,_TCHAR*);
	PTR_ID attr_add_by_name(CON_ID ,_TCHAR*,_TCHAR*);
	PTR_ID attr_add_by_name_s(CON_ID,_TCHAR*,STR_ID);	// 02/16/07 AM.
	PTR_ID attr_add_by_name(CON_ID ,_TCHAR*,long long);
	PTR_ID attr_add_by_name(CON_ID ,_TCHAR*,float);
	PTR_ID attr_add_by_name(CON_ID ,_TCHAR*,CON_ID );
	PTR_ID attr_add_by_name(XCON_S*,_TCHAR*,CON_ID);	// 02/16/07 AM.
	bool attr_rm_by_name(CON_ID, _TCHAR*, /*UP*/ bool &dirty);
	bool
	attr_rm(
		CON_ID con,
		PTR_ID ptr
		);
	bool attr_rm(	// 02/17/07 AM.
		XCON_S *con,
		XPTR_S *pre,
		PTR_ID pre_pid,
		bool first
		);
	XPTR_S *
	attr_add_new(
		XCON_S *con,
		XCON_S *slot,
		XPTR_S *vals);	// May be NULL
	XPTR_S *
	attr_add_n(
		XCON_S *con,
		XCON_S *slot_con,
		long long num
		);
	XPTR_S *
	attr_add_c(
		XCON_S *con,
		XCON_S *slot_con,
		XCON_S *val
		);

	XPTR_S *
	attr_add_val(
		XCON_S *con,
		XCON_S *slot_con,
		XPVAL val,
		XPKIND kind
		);

	XPTR_S *
	attr_find_slot_0(XCON_S *con, XCON_S *slot_con);
	XPTR_S *
	attr_get(XCON_S *con, XCON_S *slot, /*UP*/ PTR_ID &val_pid);
	XPTR_S *
	attr_get(XCON_S *con, _TCHAR *s_slot, /*UP*/ PTR_ID &val_pid);
	CON_ID
	attr_get_c(XCON_S *con, XCON_S *slot);
	long long
	attr_get_n(XCON_S *con, XCON_S *slot);
	bool
	attr_get_ns(XCON_S *con, XCON_S *slot, int len, /*UP*/ long *arr);

	STR_ID
	attr_get_s(XCON_S *con, XCON_S *slot);
	void
	attrs_pret(
		XPTR_S *attrs,
		_t_ostream *out,
		_TCHAR *indent		// String for indenting.
		);
	XCON_S *attr_get_c();

	/////////////

	_TCHAR *
	attrs_name(
		PTR_ID attrs
		);
	PTR_ID 
	attrs_vals(
		PTR_ID attrs
		);

		bool
	rm_one_val(
		PTR_ID attr,
		PTR_ID val
		);
	bool
	attr_rm_val_by_name(
		CON_ID con,
		_TCHAR *attr_s,
		_TCHAR *val_s
		);
	bool
	attr_rm_val_by_name(
		CON_ID con,
		_TCHAR *attr_s,
		long long val
		);
	bool
	attr_rm_val_by_name(
		CON_ID con,
		_TCHAR *attr_s,
		float val
		);
	bool
	attr_rm_val_by_concept(															// 06/03/02 Dd.
		CON_ID con,
		_TCHAR *attr_s,
		CON_ID val
		);
	// General.
//   static void xyz(xyz*);

private:
	CG *cg_;

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
private:
	static int count_;						// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
