/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									IND.H
*
* FILE:	consh.¹/ind.h
* SUBJ:	Declarations for IND knowledge addition commands.
* CR:	10/15/95 AM.
*
*******************************************************************************/

#ifdef OLD_020815_
extern bool ind_action(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.
	);
#endif
extern bool ind_attr(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.

	);
extern bool ind_wattr(LIST*,_t_istream*,_t_ostream*,bool,bool,		// 05/30/02 AM.
											CG*);								// 08/14/02 AM.
extern bool ind_nattr(LIST*,_t_istream*,_t_ostream*,     // 10/09/05 AM.
                                         bool,bool,CG*);

extern bool ind_childs(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.
	);
extern bool ind_named_phrase(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.
	);
extern bool ind_phrase(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.
	);
#ifdef OBSOLETE
extern bool ind_proxy(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag
	);
#endif
extern bool ind_rest(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																// 08/14/02 AM.

	);
