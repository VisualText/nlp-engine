/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									PN_S.H
*
* FILE:	conan.¹/pn_s.h
* SUBJ:	Structure definitions for parse-tree nodes.
* CR:		4/23/95 AM.
* NOTE:	10/28/95 AM. These may change.  Looks more like token info.
*
*******************************************************************************/


typedef enum
   {
   NO_PN,
   ALPH_PN,
   NUM_PN,
   WHT_PN,
   PNCT_PN,
   CTRL_PN,
   NODE_PN				/* Non-literal node.	*/
   } PN_TYP;


/**************************************************
*						PN_S
* SUBJ:	Parse-tree node data structure.
* CR:	4/23/95 AM.
* NOTE:	Optimized for both space and speed,
*		for a hard-wired, highly compiled analyzer.
*		left, right - siblings in the current phrase.
*		down - first node in subtree phrase
*		9/23/95 AM. Looks very similar to concept struct,
*		for now.  But we anticipate more attrs, each of
*		which will be added to the struct.
*		10/26/95 AM. Will need semantic, syntactic concepts, etc.
*		(Maybe a level - ie, word, lit, component, ...)
*		Also, rule leading to it, as well as if this is a segment
*		or a "completely characterized" subtree.
*		Attachment of meaning structures, discourse structures...
*
**************************************************/

typedef struct pn
   {
   PN_TYP pn_typ;		/* Type of node.					*/
   CON *con;			/* CG concept for this node.	*/
   long start, end;	/* Text offsets for node		*/
   struct pn *prev;	/* Left or previous node.		*/
   struct pn *next;	/* Right or next node.			*/
   struct pn *dn;		/* Down or child node.			*/
   struct pn *up;		/* Up or parent node.			*/
   						/* Only 1st node in a phrase
   							points to the parent node. */
   CON *mean;			/* Dynamic info for node.		*/
   } PN;
