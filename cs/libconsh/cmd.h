/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CMD.H
*
* FILE:	conch.�/cmd.h
* SUBJ:	Declarations for the command-line interface.
* CR:	8/20/95 AM.
*
*******************************************************************************/

/* Size of the command buffer. */
//#define CMD_SIZE	((long) 512)
// SIZES	// 03/25/13 AM.
#define CMD_SIZE	((long) 8192)

extern LIBCONSH_API bool
cmd_add(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool cmd_add_attr(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_con(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_empty(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_hier(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_punct(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_root(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_white(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_add_word(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_bind(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_bind_app(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_bind_sys(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_con(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_gen(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_gen_all(
	_TCHAR *dir,		// Dir to gen code to.								// 07/01/00 AM.
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_help(
	LIST *args,
	std::_t_ostream *out,
	_TCHAR *buf
	);
extern LIBCONSH_API bool
cmd_ind(
	LIST *args,
	std::_t_istream *in,
	std::_t_ostream *out,
	bool i_flag,
	bool silent,																// 02/21/00 AM.
	CG *cg																// 08/14/02 AM.
	);
extern LIBCONSH_API bool
cmd_proxy(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_show(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_show_name(
	LIST *args,
	std::_t_ostream *out,
	CG *cg																// 08/14/02 AM.
	);
extern LIBCONSH_API bool
cmd_st(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_st_add(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_st_gen(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_sym(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_sym_add(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_sym_gen(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_sym_stat(
	LIST *args,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_take(
	LIST *args,
	std::_t_istream *in,
	std::_t_ostream *out,
	CG *cg
	);
extern LIBCONSH_API bool
cmd_top(
	std::_t_istream *in,
	std::_t_ostream *out,
	_TCHAR *prompt,			/* Prompt to print, if any.	*/
	bool i_flag,			/* If interactive session.		*/
	CG *cg,																		// 08/15/02 AM.
	bool silent = true	// If silent mode.							// 02/15/00 AM.
	);

bool gen_makefile(_TCHAR*,CG*);												// 07/27/01 AM.
bool gen_kbbat(_TCHAR *);														// 07/29/01 AM.
