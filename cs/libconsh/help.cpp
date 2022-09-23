/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									HELP.C
*
* FILE:	consh.�/help.c
* SUBJ:	Help commands.
* CR:	11/11/95 AM.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
#include "prim/prim.h"
#include "prim/str.h"
//#include "prim/mach.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "consh/libconsh.h"
#include "consh/arg.h"
#include "consh/cg.h"
#include "cmd.h"
#include "help.h"


/**************************************************
*						HELP_ADD
* FUN:	help_add
* SUBJ:	Print help for ADD command.
* CR:	11/11/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

bool
help_add(
	std::_t_ostream *out
	)
{
*out << _T("CMD   ARGS         DESCR") << std::endl;
*out << _T("-------------------------------------------------") << std::endl;
*out << _T("add   attr CON SLOT VAL KIND  Add attribute to concept.") << std::endl;
*out << _T("add   con NAME CON    Add concept NAME in hierarchy under CON.") << std::endl;
*out << _T("add   empty ......    Add empty concept.") << std::endl;
*out << _T("add   hier name1 [..] Add path to hierarchy.") << std::endl;
*out << _T("add   punct ........  Add the punctuation chars to kb.") << std::endl;
*out << _T("add   root .........  Add root of concept hierarchy.") << std::endl;
*out << _T("add   white ........  Add the whitespace char to kb.") << std::endl;
*out << _T("add   word WORD ....  Add WORD to kb.") << std::endl;
return true;
}


/**************************************************
*						HELP_IND
* FUN:	help_ind
* SUBJ:	Print help for IND command.
* CR:	11/14/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

bool
help_ind(
	std::_t_ostream *out
	)
{
*out << _T("CMD   ARGS         DESCR") << std::endl;
*out << _T("-------------------------------------------------") << std::endl;
*out << _T("ind   action ........ Add a rule-action to concept.") << std::endl;
*out << _T("ind   rest .......... Add a restriction to analyzer pass.") << std::endl;
*out << _T("ind   attr .......... Add attribute to concept.") << std::endl;
*out << _T("ind   wattr ......... Add attribute to word-concept.") << std::endl;
*out << _T("ind   childs ........ Add children to hierarchy concept.") << std::endl;
*out << _T("ind   named_phrase .. Add named phrase to concept.") << std::endl;
*out << _T("ind   phrase ........ Add phrase to concept.") << std::endl;
*out << _T("ind   proxy ......... OBSOLETE. Use ind attr.") << std::endl;
return true;
}
