/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright © 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									LIST_S.H
*
* FILE:	conan.¹/list_s.h
* SUBJ:	Structure definitions for list subsystem.
* NOTE:	
* CR:	8/26/95 AM.
*
*******************************************************************************/



/**************************************************
*						LIST
* SUBJ:	List data structure.
* CR:	8/26/95 AM.
* NOTE:	val can be used for any value, e.g., a pointer.
*
**************************************************/

#ifndef LIST_S_H_
#define LIST_S_H_

typedef struct list
   {
   long val;				/* Generic value.		*/
   struct list *next;		/* Next list element.	*/
   } LIST;

#endif

