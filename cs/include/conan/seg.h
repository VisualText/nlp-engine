/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									SEG.H
*
* FILE:	conan.¹/seg.c
* SUBJ:	Declaration for segmentation algorithms.
* CR:	11/7/95 AM.
*
*******************************************************************************/
extern PN *
seg_multi(
	PN *root,			/* Full parse tree with tokens at leafs.				*/
	long rule_s,
	long rule_e,		/* Start and end rule numbers for current pass.		*/
	PTR *untrig			/* List of untriggered rule numbers for curr pass.	*/
	);

extern void
seg_multi_eoseg(
	PN *root,			/* Full parse tree with tokens at leafs.				*/
	long rule_s,
	long rule_e,		/* Start and end rule numbers for current pass.		*/
	PTR *untrig,		/* List of untriggered rule numbers for curr pass.	*/
	PN *last,			/* Last node in segment.	*/
	PN *seg_s,			/* Earliest node to match backward to.		*/
	PN *seg_e			/* One past end boundary.	*/
	);

extern PN *
segs_multi(
	PN *root,			/* Given subtree of a parse tree.						*/
	PN *seg_s,			/* Start of parse tree phrase to be matched.			*/
	PN *seg_e,			/* "One past" end of parse phrase to be matched.	*/
	long rule_s,
	long rule_e,		/* Start and end rule numbers for current pass.		*/
	PTR *untrig			/* List of untriggered rule numbers for curr pass.	*/
	);
