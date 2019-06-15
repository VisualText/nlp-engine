/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									TOK.H
*
* FILE:	conan.¹/tok.h
* SUBJ:	Declarations for tokenizer.
* CR:	10/28/95 AM.
*
*******************************************************************************/

/* This should probably be in an analyzer sizes .h file */
#define TOK_SIZE  ((long)128)

extern void		 tok();
extern int
tok_get(
	_TCHAR *token,		/* Buffer to place token.				*/
	_TCHAR *mixed,		/* Buffer for mixed-case, if any.	*/
	long tok_siz,		/* Size of token buffer.				*/
	FILE *fp,			/* Output stream.							*/
	/*UP*/
	_TCHAR **curr,		/* Curr char in buf.						*/
	long *off,			/* Curr position in buf.				*/
	long *start,
	long *end,			/* Token offsets in buf.				*/
	PN_TYP *typ,		/* Alphanum, white, punct, ctrl 		*/
	bool *cap,			/* If token is capitalized.			*/
	bool *allcap		/* If token is all-caps.				*/
	);
