/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									GEN.H
*
* FILE:	conan.�/gen.h
* SUBJ:	Declarations for code generation.
* NOTE:	
* CR:	5/06/95 AM.
*
*******************************************************************************/

extern void gen_array_decl(
	_TCHAR *typ,
	_TCHAR *nam,
	std::_t_ofstream *fp
	);
extern void gen_array_def(
	_TCHAR *typ,
	_TCHAR *nam,
	_TCHAR *siz,
	std::_t_ofstream *fp
	);
extern void gen_array_hd(
	_TCHAR *typ,
	_TCHAR *nam,
	_TCHAR *siz,
	std::_t_ofstream *fp
	);
extern void gen_array_tl(
	std::_t_ofstream *fp
	);
extern void gen_file_head(
	std::_t_ofstream *fp
	);
extern void
gen_st_empty(
	_TCHAR *s_tab,		/* Name of string table array. */
	_TCHAR *s_nam			/* Name of file for string table array.	*/
	);
extern void
words_to_st_file(
	_TCHAR *words_f,		/* Name of words file. 						*/
	_TCHAR *s_base		/* Basic name for string table files.	*/
	);

