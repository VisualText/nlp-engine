/*******************************************************************************
Copyright (c) 1999-2007 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CGDICT.H
* FILE:	include/API/cgdict.h.
* CR:		02/19/07 AM.
* SUBJ:	Class for VisualText CG KBMS dictionary functions.
*
*******************************************************************************/

#ifndef CGDICT_H_
#define CGDICT_H_


#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <stdlib.h>
#include <iostream>
#include <fstream>
#endif

// Forward reference.
class CG;

/********************************************
* CLASS:	CGDICT
* CR:		02/18/07 AM.
* SUBJ:	API Class for managing the VisualText KBMS attributes.
********************************************/

class LIBQCONSH_API CGDICT
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	CGDICT(										// Default constructor.
		CG *cg
		);
	~CGDICT();									// Destructor.

	// Access
//	bool  getXyz;
	XCON_S *getC_one();	// 03/06/07 AM.

	// Modify
//	void setXyz(_TCHAR *);

	// Class functions


	// General functions

	bool dict_add_punct();
//	CON_ID dict_add_unknown(_TCHAR*);	// OBSOLETE.	// 02/19/07 AM.
	bool dict_add_white();
	CON_ID dict_add_word(_TCHAR*);
	CON_ID dict_add_word_con(STR_ID,_TCHAR*,bool&,DICT_CALL);	// 06/29/03 AM.	// Todo: Reimplement. // 02/14/07 AM.
	XCON_S *dict_find_index(_TCHAR*);
	XCON_S *dict_find_index(XCON_S*,_TCHAR*);
	bool dict_rm_word(_TCHAR *str);				// 07/17/00 AM.
	CON_ID dict_find_word(_TCHAR *name);
	XCON_S *dict_get_word(_TCHAR*,bool&);			// 06/29/03 AM.
	CON_ID dict_get_word_id(_TCHAR*,bool&);		// 02/20/07 AM.
	CON_ID dict_find_word_lc(_TCHAR *name);
	XCON_S *dict_first();							// 03/21/03 AM.
	XCON_S *dict_next(XCON_S*);						// 03/21/03 AM.
	XCON_S *dict_next(XCON_S*,bool);					// 03/21/03 AM.
	XCON_S *dict_subtree(XCON_S*,bool);				// 03/21/03 AM.

	// A FASTER LOAD OF DICTIONARY FROM FILES.				// 03/06/07 AM.
	bool dict_load_ini();											// 03/06/07 AM.
	bool dict_load_fin();											// 03/06/07 AM.
	CON_ID dict_load_word(_TCHAR*);								// 03/06/07 AM.
	CON_ID dict_load_word_con(STR_ID,_TCHAR*,bool&);		// 03/06/07 AM.
	CON_ID dict_load_add(											// 03/06/07 AM.
		STR_ID sid);													// 03/06/07 AM.

private:
	CG *cg_;
	
	// FASTER DICTIONARY LOAD. TRACK POSITION SO FAR.		// 03/06/07 AM.
	XCON_S *c_one;			// Top dict index layer.			// 03/06/07 AM.
	XCON_S *c_one_next;	// Next top index.					// 03/06/07 AM.
	XCON_S *c_two;			// 2nd dict index layer.			// 03/06/07 AM.
	XCON_S *c_two_next;	// Next 2nd index.					// 03/06/07 AM.
	XCON_S *c_word;		// Previous word added.				// 03/06/07 AM.

	_TCHAR *s_one_next;	// str. Next top index.				// 03/06/07 AM.
	_TCHAR *s_two_next;	// str. Next 2nd index.				// 03/06/07 AM.

// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
private:
	static int count_;						// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
