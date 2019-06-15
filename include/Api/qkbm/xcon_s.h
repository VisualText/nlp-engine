/*******************************************************************************
Copyright (c) 2007-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	XCON_S.H
* FILE:	include/cs/qkbm/xcon_s.h
* CR:		02/07/07 AM.
* SUBJ:	XCON_S concept structure definition.
* NOTE:	QDBM implementation of Conceptual Grammar KBMS.
*			Using unique numbers to identify objects, rather than pointers.
*
*******************************************************************************/

#ifndef XCON_S_H_
#define XCON_S_H_


/**************************************************
*						CKIND
* SUBJ:	Types of concepts.
**************************************************/

typedef enum xckind
   {
   cNO_CON,
   cXPROXY,			// A con representing another con. Not in the hierarchy.
   cXBASIC,			// A concept that resides in the concept hierarchy.
   cXWORD,			// A concept representing an "atomic" piece of text.
   cNUM,				// An idea, for special handling of integers. [UNUSED]
   cCHAR				// An idea, for special handling of chars. [UNUSED]
   } XCKIND;


/**************************************************
*						CON_S
* SUBJ:	Concept data structure.
* NOTE:	Keeping ID field, keeps code simpler.
*
**************************************************/

typedef struct xcon_s
   {
   int id;				// CON_ID. Unique num for concept.	REINSTATE 02/14/07 AM.
   XCKIND kind;		// Type of concept: (See union val below.)
   union val
      {
      int vname;	// CON_ID. kind=BASIC. Name describing basic concept.
      int vsym;	// CON_ID. kind=WORD.  Word that concept represents.
      long vnum;		// kind=NUM.   Int that concept represents.
      _TCHAR vchar;	// kind=CHAR.  Char that concept represents.

      int vcon;	// CON_ID. kind=PROXY. Concept that proxy represents.
      }v;

   int flags;			// Bit-flags for concept.
   int attrs;		// PTR_ID. Attribute list for concept.
   						// Use for: skips, starts, ends, rules, etc.
   int up;			// CON_ID. if kind != PROXY, up hierarchy.
   						// if kind == PROXY, unused for now.
   int dn;			// CON_ID. if kind != PROXY, down hierarchy.
   						// if kind == PROXY, unused for now.
   int prev;		// CON_ID. if kind != PROXY, prev hierarchy sibling.
							// if kind == PROXY, previous elt of phrase.
   int next;		// CON_ID. if kind != PROXY, next hierarchy sibling.
							// if kind == PROXY, then next elt of phrase.
	int atom;		// CON_ID. if kind == PROXY, then con that owns phrase/node.
	int phrases;	// PTR_ID. Setting up for concept to own multiple phrases.
   } XCON_S;



#endif
/*************************      END OF FILE      ******************************/
