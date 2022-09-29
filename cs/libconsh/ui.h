/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									UI.H
*
* FILE:	conch.�/ui.h
* SUBJ:	Declarations for ui primitives.
* CR:	10/15/95 AM.
*
*******************************************************************************/

extern bool
path_to_con(
	LIST *args,
	CG *cg,
	/*UP*/
	CON **upcon
	);
extern bool
ui_read_con(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	CON **con
	);
extern bool
ui_read_end(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.
	);
extern bool
ui_read_num(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	long *num
	);
extern bool
ui_read_pkind(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	PKIND *pkind
	);
extern bool
ui_read_sym(
	_TCHAR *prompt,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg,																// 08/14/02 AM.
	/*UP*/
	SYM **sym
	);
extern bool ui_read_word(													// 05/30/02 AM.
	_TCHAR*,std::_t_istream*,std::_t_ostream*,bool,bool,CG*,/*UP*/CON**);		// 05/30/02 AM.

