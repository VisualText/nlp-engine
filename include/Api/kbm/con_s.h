/*******************************************************************************
Copyright © 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
*									CON_S.H
*
* FILE:	include/API/kbm/con_s.h
* SUBJ:	Structure definitions for the concept data structure.
* CR:		9/7/95 AM.
*
*******************************************************************************/


/**************************************************
*						CKIND
* SUBJ:	Types of concepts.
* CR:		9/15/95 AM.
* NOTE:	If we have rule type, will need to move the concept name field out of
*		the union, because rule nodes should have names.
*		Should we implement parse trees in the CG in Consh, we'll need to have
*		parent and child fields in the concept data structure as well, and
*		perhaps a parse-tree node concept type. (Of course we could implement
*		nodes via attributes, or even as separate data structures, but the latter
*		won't save, unless we make yet another table for them.  And really, we'd
*		want them to be "conceptual" as well.)
*
**************************************************/

typedef enum ckind
   {
   cNO_CON,
   cPROXY,			/* A concept representing another concept.  Resides in
   						phrases and rules, but not in the concept hierarchy. */
   cBASIC,			/* A concept that resides in the concept hierarchy.	*/
   cWORD,				/* A concept representing an "atomic" piece of text.	*/
   /* cPHRASE, */	/* An idea, for efficient handling of literal phrases.*/
   /* cRULE, */		/* An idea, for concepts that represent grammar rules.*/
   cNUM,				/* An idea, for special handling of integers.			*/
   cCHAR				/* An idea, for special handling of chars.				*/
   } CKIND;


/**************************************************
*						CON_S
* SUBJ:	Concept data structure.
* CR:	9/7/95 AM.
* NOTE:	For Consh, reducing space here to an absolute minimum.
*		For Conan, many things here will become specialized structures.
*		Eg, a dictionary management system, a parse tree management system.
*		PROXY concepts are used to build phrases and grammar rules.
*		Proxies do not directly reside in the hierarchy.  But ATOM attribute
*		of theirs will point to a basic concept that represents them, called
*		the atomic concept. (ATOM-PHRASE attribute pair.)  By convention,
*		only the first proxy of a phrase will have the atom attr, to save
*		space.
*		Others are "basic" concepts that fit in the concept hierarchy.
*		9/15/95 AM. up,down,prev,next don't necessarily cost space, because
*		the down hierarchy requires a list of concepts, which shall be
*		hierarchical siblings.  In this way, the concept structure shall
*		serve as a parse tree node as well, at least in Consh.  Just don't
*		have a use for down and up in the proxy concepts, at present.  Could
*		use them to point to rule nodes, for example, but the reverse would
*		be an attr, so it's not aesthetic.
*
**************************************************/

typedef struct con
   {
   ID id;					/* Unique number identifying concept.				*/
   							/* 12/2/95 AM. Using this for gend code.			*/
   CKIND kind;				/* Type of concept: (See union val below.)		*/
   union val
      {
      struct sym *vname;/* kind=BASIC. Name describing basic concept.	*/
      struct sym *vsym;	/* kind=WORD.  Word that concept represents.		*/
      long vnum;			/* kind=NUM.   Int that concept represents.		*/
      _TCHAR vchar;			/* kind=CHAR.  Char that concept represents.		*/

      struct con *vcon;	/* kind=PROXY. Concept that proxy represents.	*/
      }v;

   int flags;				/* Bit-flags for concept.								*/
   struct ptr *attrs;	/* Attribute list for concept.						*/
   							/* Use for: skips, starts, ends, rules, etc.		*/
   struct con *up;		/* if kind != PROXY, up hierarchy.					*/
   							/* if kind == PROXY, unused for now.				*/
   struct con *dn;		/* if kind != PROXY, down hierarchy.				*/
   							/* if kind == PROXY, unused for now.				*/
   struct con *prev;		/* if kind != PROXY, prev hierarchy sibling.		*/
								/* if kind == PROXY, previous elt of phrase.		*/	
   struct con *next;		/* if kind != PROXY, next hierarchy sibling.		*/
								/* if kind == PROXY, then next elt of phrase.	*/
   } CON;
