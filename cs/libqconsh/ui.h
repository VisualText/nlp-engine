/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									UI.H
*
* FILE:	conch.¹/ui.h
* SUBJ:	Declarations for ui primitives.
* CR:	10/15/95 AM.
*
*******************************************************************************/

extern bool
path_to_con(
	LIST *args,
	CG *cg,
	/*UP*/
	XCON_S **upcon
	);

extern bool
path_to_con(															// 03/08/07 AM.
	LIST *args,
	CG *cg,
	XCON_S **path,
	_TCHAR **spath,
	int &lpath
	);

extern bool
ui_read_con(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	XCON_S **con
	);

extern bool
ui_read_con(															// 03/08/07 AM.
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,
	XCON_S **path,
	_TCHAR **spath,
	int &lpath
	);

extern bool
ui_read_end(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.
	);
extern bool
ui_read_num(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	long *num
	);
extern bool
ui_read_pkind(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	XPKIND *pkind
	);
extern bool
ui_read_sym(
	_TCHAR *prompt,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	STR_ID *sym
	);
extern bool ui_read_word(													// 05/30/02 AM.
	_TCHAR*,_t_istream*,_t_ostream*,bool,bool,CG*,/*UP*/XCON_S**);		// 05/30/02 AM.

