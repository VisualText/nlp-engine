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

#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"
#include "qconsh/arg.h"

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
	_t_ostream *out
	)
{
*out << _T("CMD   ARGS         DESCR") << endl;
*out << _T("-------------------------------------------------") << endl;
*out << _T("add   attr CON SLOT VAL KIND  Add attribute to concept.") << endl;
*out << _T("add   con NAME CON    Add concept NAME in hierarchy under CON.") << endl;
*out << _T("add   empty ......    Add empty concept.") << endl;
*out << _T("add   hier name1 [..] Add path to hierarchy.") << endl;
*out << _T("add   punct ........  Add the punctuation chars to kb.") << endl;
*out << _T("add   root .........  Add root of concept hierarchy.") << endl;
*out << _T("add   white ........  Add the whitespace char to kb.") << endl;
*out << _T("add   word WORD ....  Add WORD to kb.") << endl;
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
	_t_ostream *out
	)
{
*out << _T("CMD   ARGS         DESCR") << endl;
*out << _T("-------------------------------------------------") << endl;
*out << _T("ind   action ........ Add a rule-action to concept.") << endl;
*out << _T("ind   rest .......... Add a restriction to analyzer pass.") << endl;
*out << _T("ind   attr .......... Add attribute to concept.") << endl;
*out << _T("ind   wattr ......... Add attribute to word-concept.") << endl;
*out << _T("ind   childs ........ Add children to hierarchy concept.") << endl;
*out << _T("ind   named_phrase .. Add named phrase to concept.") << endl;
*out << _T("ind   phrase ........ Add phrase to concept.") << endl;
*out << _T("ind   proxy ......... OBSOLETE. Use ind attr.") << endl;
return true;
}
