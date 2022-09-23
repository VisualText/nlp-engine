/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

////////////////////////////////
/// 04/23/99 AM.
// Putting this here, from libprim/io.cpp
// because of some weird dll access violation problem.
////////////////////////////////
#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.

#include "prim/libprim.h"
//#include "prim/mach.h"
#include "prim/prim.h"
#include "prim/sm.h"
#include "prim/str.h"
#include "prim/list_s.h"
#include "prim/list.h"

#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/ra.h"
#include "kbm/pret.h"
#include "kbm/Akbm.h"

#include "ext/kb.h"
#include "kbm/kbm_kb.h"

#include "consh/libconsh.h"
#include "consh/cg.h"

#include "io.h"		// 04/23/99 AM.

void
//lctab(int num, FILE *out)			// 04/20/99 AM.
lctab(int num, std::_t_ostream *out)		// 04/20/99 AM.
{
while (num-- > 0)
   //fprintf(out, "\t");		// 04/20/99 AM.
	*out << _T("\t") << std::flush;		// 04/20/99 AM.
}


/**************************************************
*						RULE_PRET
* FUN:	rule_pret
* SUBJ:	Short pretty-print of grammar rule.
* CR:		6/29/96 AM.
*
**************************************************/

void
rule_pret(
	CON *atom,			/* Atomic concept for phrase.	*/
	//FILE *out			// 04/20/99 AM.
	std::_t_ostream *out,		// 04/20/99 AM.
	CG *cg
	)
{
CON *phr;

if (!atom)
	return;
phr = cg->acon_->con_phrase(atom);
//fprintf(out, "%s <- ", con_str(atom));
*out << ACON::con_str(atom)
	  << _T(" <- ");
phrase_pret(phr, out,cg);
}



/**************************************************
*						PHRASE_PRET
* FUN:	phrase_pret
* SUBJ:	Pretty-print the given phrase.
* CR:		10/20/95 AM.
*
**************************************************/

//LIBKBM_API
void
phrase_pret(
	CON *phr,
	//FILE *out		// 04/20/99 AM.
	std::_t_ostream *out,	// 04/20/99 AM.
	CG *cg
	)
{
while (phr)
   {
   //fprintf(out, "%s ", con_str_pret(phr));	// 04/20/99 AM.
	*out << cg->acon_->con_str_pret(phr) << _T(" ");		// 04/20/99 AM.
   phr = phr->next;
   }
*out << std::flush;	// 04/20/99 AM.
}


/**************************************************
*						CON_PRET
* FUN:	con_pret
* SUBJ:	Pretty-print one concept.
* CR:		9/20/95 AM.
* NOTE:	Need to watch for deleted concepts.
*
**************************************************/

void
con_pret(
	CON *con,
	//FILE *out		// 04/20/99 AM.
	std::_t_ostream *out,	// 04/20/99 AM.
	CG *cg
	)
{
CON *pc, *phr;

if (!con)
   {
   //fprintf(out, "[No concept given.]\n");
	*out << _T("[No concept given.]") << std::endl;
   return;
   }

//fprintf(out, "(%s) %s \n", con_kind_str(con), con_str(con));	// 04/20/99AM
*out << _T("(")
	  << cg->acon_->con_kind_str(con)
	  << _T(") ")
	  << ACON::con_str(con)
	  << std::endl;				// 04/20/99 AM.
switch(con->kind)
   {
   case cPROXY:
      break;
   case cBASIC:
   case cWORD:
      if (con->up)
			//fprintf(out, "    up: %s\n", con_str(con->up));
			*out << _T("    up: ") << ACON::con_str(con->up) << std::endl;	// 04/20/99 AM.
      //fprintf(out, "    down: ");
		*out << _T("    down: ");
      pc = con->dn;
      while (pc)
         {
         //fprintf(out, "%s ", con_str(pc));
			*out << ACON::con_str(pc) << _T(" ");
         pc = pc->next;
         }
      //fprintf(out, "\n");
		*out << std::endl;
      
      /* Get phrase attr, if any.  Print out phrase. */
      phr = cg->acon_->con_phrase(con);
      if (phr)
         {
         //fprintf(out, "phr: ");
			*out << phr << _T(": ");
         phrase_pret(phr, out,cg);
         //fprintf(out, "\n");
			*out << std::endl;
         }
		
		// 08/31/99 AM. Print path for concept!
		_TCHAR buf[1024];
		cg->acon_->con_to_path(con, /*DU*/ buf);
		*out << _T("path=") << buf << std::endl;

      break;
   default:
      break;
   }
//fprintf(out, "    Attrs:\n");
*out << _T("    Attrs:") << std::endl;
cg->kbm_->attrs_pret(con->attrs, out, _T("     "));
}


/**************************************************
*						RULE_PRET
* FUN:	rule_pret
* SUBJ:	Short pretty-print of grammar rule.
* CR:		6/29/96 AM.
*
**************************************************/
#ifdef _DUP_
void
rule_pret(
	CON *atom,			/* Atomic concept for phrase.	*/
	//FILE *out			// 04/20/99 AM.
	std::_t_ostream *out		// 04/20/99 AM.
	)
{
CON *phr;

if (!atom)
	return;
phr = con_phrase(atom);
//fprintf(out, "%s <- ", con_str(atom));
*out << con_str(atom)
	  << _T(" <- ");
phrase_pret(phr, out,cg);
}
#endif


/**************************************************
*						F_EXISTS
* FUN:	f_exists
* SUBJ:	Yet another file-exists function.
* CR:		05/02/99 AM.  (Moved from cmd.cpp)
*
**************************************************/

bool
f_exists(_TCHAR *fil)
{
//ifstream x(fil, std::ios::in | std::ios::nocreate);
std::_t_ifstream x(TCHAR2A(fil), std::ios::in);									// Upgrade.	// 01/24/01 AM.
if (x)
	return true;
else
	return false;
}
