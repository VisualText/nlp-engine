/****************************************
Copyright � 1995 by Conceptual Systems.
Copyright (c) 1995 by Conceptual Systems.
All rights reserved.
*****************************************/ 
/*******************************************************************************
*
*									CMD.C
*
* FILE:	consh.�/cmd.c
* SUBJ:	Command-line interface.
* CR:	8/20/95 AM.
* NOTE:	To support all the needed commands for Consh.
*		Idea is to support even micro-level commands, in preparation for a
*		graphic user interface, and to enforce modularity of the latter.
*
*******************************************************************************/

#include "StdAfx.h"			// 04/29/99 AM.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#include <string.h>


#include "prim/libprim.h"
#include "cg_global.h"		// 10/05/99 AM. Debugging QUANTIFY.

#include "prim/unicode.h"			// 01/14/06 AM.

#include "prim/prim.h"
#include "prim/str.h"
//#include "prim/mach.h"
#include "prim/list_s.h"
#include "prim/list.h"
#include "prim/io.h"	// 07/01/03 AM.

/*
#include "kbm/libkbm.h"
#include "kbm/st.h"
#include "kbm/sym_s.h"
#include "kbm/sym.h"
#include "kbm/con_s.h"
#include "kbm/con_.h"
#include "kbm/ptr_s.h"
#include "kbm/ptr.h"
#include "kbm/attr.h"
#include "kbm/dict.h"
#include "kbm/find.h"
#include "kbm/ra.h"
#include "kbm/rest.h"
#include "kbm/pret.h"
#include "kbm/kbm_kb.h"		// 04/23/99 AM.
#include "kbm/Akbm.h"
*/
#include "qkbm/libqkbm.h"
#include "qkbm/Aqkbm.h"

#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "cgcon.h"
#include "cgdict.h"
#include "qconsh/cg.h"
#include "qconsh/bind.h"
//#include "qconsh/consh_kb.h"		// OBSOLETE.	02/19/07 AM.
#include "qconsh/arg.h"

//#include "consh/cc_var.h"

#include "cmd.h"

#include "io.h"
//#include "Con_ini.h"

//#include "cc_gen.h"
//#include "st_gen.h"
//#include "sym_gen.h"
//#include "con_gen.h"
//#include "ptr_gen.h"
//#include "var_gen.h"
//#include "ana_gen.h"
#include "ind.h"
#include "help.h"

/**************************************************
*						CMD_TOP
* FUN:	cmd_top
* SUBJ:	Top-level of the command-line interface.
* CR:	8/20/95 AM.
* NOTE:	For now, a simple conditional statement.
*		Ultimately, should implement as a self-contained hash table.
*		Command line args should be a list of strings.
*		(Keeping this mechanism away from string table, in case of problems
*		with the latter.)
*		As in C, backslash escapes next char, double quotes enclose strings,
*		whitespace delimits arguments.  Double quotes surrounding string
*		are not part of string. (Nice to allow backslash <cr> as well, to
*		escape newline).
*		If non-interactive read, may want to quit if commands return !ok.
*
**************************************************/

LIBQCONSH_API bool
cmd_top(
	_t_istream *in,
	_t_ostream *out,
	_TCHAR *prompt,			/* Prompt to print, if any.	*/
	bool i_flag,			/* If interactive session.		*/
	CG *cg,																		// 08/15/02 AM.
	bool silent				// If silent mode.							// 02/15/00 AM.
	)
{
_TCHAR buf[CMD_SIZE];
_TCHAR *str;
LIST *args, *list;
bool ok;
ALIST *alist = cg->alist_;			// List manager.					// 08/15/02 AM.


ok = true;
for (;;)		/* Execute commands till quit or abort. */
   {
   /* Read command line arguments. */
	if (!silent)																// 02/21/00 AM.
		*out << prompt << flush;
   ok = args_read(in,out,silent,alist,buf,CMD_SIZE,&args);		// 07/22/04 AM.
   if (!ok)
      {
      if (!args)
         {
         *out << _T("Unexpected EOF.") << endl;
         return(false);
         }
      *out << _T("Bad command=");
      args_pp(args, out, buf);
      alist->list_free(args, LNULL);
      if (!i_flag) return(false);
      continue;
      }
   list = args;
   str = ALIST::list_pop_buf(&args,buf);		/* Get first arg. */
//	if (str && *str)							// 10/05/99 AM.
//		{
//		*cgerr << prompt << flush;
//		*cgerr << str << "\n" << flush;	// 10/05/99 AM. DEBUGGING QUANTIFY!!!
//		}

   /* COMMANDS GO HERE */
   
	if (!str || !*str)
		;										// 04/21/99 AM.
   else if (!_tcscmp(_T("add"), str))
      ok = cmd_add(args, out,cg);
   else if (!_tcscmp(_T("bind"), str))
      ok = cmd_bind(args, out,cg);
   else if (!_tcscmp(_T("ind"), str))
      ok = cmd_ind(args, in, out, i_flag, silent,cg);
   else if (!_tcscmp(_T("con"), str))
      ok = cmd_con(args, out,cg);
   else if (!_tcscmp(_T("gen"), str))
      ok = cmd_gen(args, out,cg);		/* Kb code generation. */
   else if (!_tcscmp(_T("st"), str))
      ok = cmd_st(args, out,cg);		/* String table primitives. */
   else if (!_tcscmp(_T("help"), str)
         || !_tcscmp(_T("h"), str))
      ok = cmd_help(args, out, buf);
   else if (!_tcscmp(_T("proxy"), str))
      ok = cmd_proxy(args, out,cg);
   else if (!_tcscmp(_T("show"), str))
      ok = cmd_show(args, out,cg);
   else if (!_tcscmp(_T("sym"), str))
      ok = cmd_sym(args, out,cg);
   else if (!_tcscmp(_T("take"), str))
      ok = cmd_take(args, in, out,cg);
   else if (';' == str[0])			/* Comment line. */
      ok = true;
   else if (!_tcscmp(_T("quit"), str)
       || !_tcscmp(_T("q"), str))
      {
		if (!silent)														// 02/21/00 AM.
			*out << _T("Quitting.") << endl;
      alist->list_free(list, LNULL);
      break;
      }
   else if (Str_full(str))
      {
      *out << _T("Unknown command=");
      args_pp(list, out, buf);
      ok = false;
      }
   /* else empty command. */
   
   if (!ok)
      *out << _T("[Command failed=") << str << _T(".]") << endl;			// 09/27/01 AM.
   
   alist->list_free(list, LNULL);
   
   if (!ok && !i_flag)
      return(ok);
   }
return(ok);
}


/**************************************************
*						CMD_HELP
* FUN:	cmd_help
* SUBJ:	Print command help for user.
* CR:	8/27/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_help(
	LIST *args,
	_t_ostream *out,
   _TCHAR *buf
	)
{
_TCHAR *str;
bool ok;

ok = true;
if (args)
   {
   str = ALIST::list_pop_buf(&args,buf);		/* Get first arg. */
   if (args)
      {
      *out << _T("Too many args in 'help' command.") << endl;
      return(false);
      }

   /* COMMANDS GO HERE */
   if (!_tcscmp(_T("add"), str))
      return(help_add(out));
   else if (!_tcscmp(_T("ind"), str))
      return(help_ind(out));
   else if (Str_full(str))
      {
      *out << _T("Unknown command=");
      args_pp(args, out, buf);
      return(false);
      }
   /* else empty command. */
   }

*out << _T("") << endl;
*out << _T("CMD   ARGS         DESCR") << endl;
*out << _T("-------------------------------------------------") << endl;
*out << _T("add   ............  Set of add commands. (type 'help add')") << endl;
*out << _T("bind  app ........  (kb bootstrap) Bind concepts to code variables.") << endl;
*out << _T("bind  sys ........  (kb bootstrap) Bind concepts to code variables.") << endl;
*out << _T("con   [pp] .......  Pretty-print the concept table.") << endl;
*out << _T("con   ID .........  Pretty-print concept with given ID number.") << endl;
*out << _T("gen   all ........  Generate all code for Resume app.") << endl;
*out << _T("gen   st .........  Generate code for string table.") << endl;
*out << _T("help  ............  Print command help.") << endl;
*out << _T("ind   ............  Higher-level add commands.") << endl;
*out << _T("proxy ID NTH .....  Pretty-print NTH element of ID's phrase.") << endl;
*out << _T("show  name NAME ..  Find instances of NAME in the kb.") << endl;
*out << _T("st    add STRING    Add STRING to string table.") << endl;
*out << _T("st    gen  .......  Generate code for string table.") << endl;
*out << _T("st    [pp] .......  Pretty-print the string table.") << endl;
*out << _T("sym   ............  Symbol table commands.") << endl;
*out << _T("sym   add STRING .. Add STRING to symbol table.") << endl;
*out << _T("sym   gen ......... Generate code for symbol table.") << endl;
*out << _T("sym   [pp] .......  Pretty-print the symbol table.") << endl;
*out << _T("sym   stat ........ Display statistics for symbol table.") << endl;
*out << _T("take  DIR FILE .... Execute commands from dir:file.") << endl;
*out << _T("quit  ............  Quit the program.") << endl;
return(ok);
}


/**************************************************
*						CMD_ADD
* FUN:	cmd_add
* SUBJ:	Primitive knowledge addition commands.
* CR:	9/18/95 AM.
* RET:	ok - If command successful.
* NOTE:	Operates one level of abstraction higher than commands for the
*		individual tables.  Should be available to knowledge engineer.
*
**************************************************/

LIBQCONSH_API bool
cmd_add(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Too few args in ADD command.") << endl;
   return(false);
   }

str = ALIST::list_pop_buf(&args,buf);

if (!_tcscmp(str, _T("root")))				/* Add root kb concept.			*/
   return(cmd_add_root(args, out,cg));
else if (!_tcscmp(str, _T("attr")))			/* Add attr to concept.			*/
   return(cmd_add_attr(args, out,cg));
else if (!_tcscmp(str, _T("con")))			/* Add concept to hierarchy.	*/
   return(cmd_add_con(args, out, cg));
else if (!_tcscmp(str, _T("empty")))
   return(cmd_add_empty(args, out,cg));	/* Add empty concept to hier.	*/
else if (!_tcscmp(str, _T("hier")))			/* Add concept to hierarchy.	*/
   return(cmd_add_hier(args, out,cg));
else if (!_tcscmp(str, _T("punct")))			/* Add punct chars to kb.	*/
   return(cmd_add_punct(args, out,cg));
else if (!_tcscmp(str, _T("white")))			/* Add white chars to kb.	*/
   return(cmd_add_white(args, out,cg));
else if (!_tcscmp(str, _T("word")))			/* Add word to kb.	*/
   return(cmd_add_word(args, out,cg));
else
   {
   *out << _T("Unknown command= add ") << str << _T(".") << endl;
   return(false);
   }
}


/**************************************************
*						CMD_ADD_ATTR
* FUN:	cmd_add_attr
* SUBJ:	Add attribute to concept.
* CR:	10/11/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool cmd_add_attr(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
return false;	// OBSOLETE FUN, I THINK.	// 02/15/07 AM.
#ifdef OLD_070215_

XCON_S *con, *slot;
_TCHAR *s_con, *s_slot, *s_val, *s_kind;
long i_con, i_slot;
XPVAL p_val;
XPKIND p_kind;
bool ok;

if (!args)
   {
   *out << _T("Too few args in ADD ATTR command.") << endl;
   return(false);
   }
s_con = ALIST::list_pop_buf(&args,buf);
if (!args)
   {
   *out << _T("Too few args in ADD ATTR command.") << endl;
   return(false);
   }
s_slot = ALIST::list_pop_buf(&args,buf);
if (!args)
   {
   *out << _T("Too few args in ADD ATTR command.") << endl;
   return(false);
   }
s_val = ALIST::list_pop_buf(&args,buf);
if (!args)
   {
   *out << _T("Too few args in ADD ATTR command.") << endl;
   return(false);
   }
s_kind = ALIST::list_pop_buf(&args,buf);

/* VERIFY INPUTS. */
/* Check appropriate values everywhere. */
/* Check that con and slot are in con table, etc. */
ok = s_to_l(s_con, &i_con);		/* Convert string to id. */
if (!ok)
   {
   *out << _T("Bad concept id=") << s_con << endl;
   return(false);
   }
ok = s_to_l(s_slot, &i_slot);	/* Convert.	*/
if (!ok)
   {
   *out << _T("Bad slot concept id=") << s_slot << endl;
   return(false);
   }
ok = cg->qkbm_->s_to_pkind(s_kind, &p_kind);
if (!ok)
   {
   *out << _T("Bad ptr kind=") << s_kind << endl;
   return(false);
   }
ok = (cg->qkbm_->s_to_pval(s_val, p_kind, &p_val) ? true : false);
if (!ok)
   {
   *out << _T("Bad slot value=") << s_val << endl;
   return(false);
   }

/* RESOLVE VALUES */
con = cg->qkbm_->Con(i_con);
if (!con)
   {
   *out << _T("Bad concept=") << s_con << _T("]") << endl;
   return(false);
   }
slot = cg->qkbm_->Con(i_slot);
if (!slot)
   {
   *out << _T("Bad concept=") << s_slot << _T("]") << endl;
   return(false);
   }

/* ADD THE ATTRIBUTE! */
cg->qkbm_->attr_add_val(con, slot, p_val, p_kind);
return(true);
#endif
}


/**************************************************
*						CMD_ADD_CON
* FUN:	cmd_add_con
* SUBJ:	Add a concept to the hierarchy.
* CR:	9/18/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_add_con(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
return false; // 02/15/07 AM.
/*
_TCHAR *name,
     *idstr;		// Hierarchy to put new con under.
long id;
//SYM *sym;
XCON_S *con;

if (!args)
   {
   *out << _T("Too few args in ADD CON command.") << endl;
   return(false);
   }

name = ALIST::list_pop_buf(&args,buf);

if (!args)
   {
   *out << _T("Too few args in ADD CON command.") << endl;
   return(false);
   }

idstr = ALIST::list_pop_buf(&args,buf);

if (args)
   {
   *out << _T("Too many args in ADD CON command.") << endl;
   return(false);
   }
*/

/*
Get sym for name.
Convert id to number.
Find id concept in con table.
Add concept under id-concept.
*/
/*
bool dirt;	// 06/29/03 AM.
sym = cg->qkbm_->sym_get(name,dirt);
id = _ttoi(idstr);	// Need a better fn.
con = cg->qkbm_->Con(id);		// Hierarchic parent.
cg->qkbm_->con_add_basic(name, con);

return(true);
*/
}


/**************************************************
*						CMD_ADD_EMPTY
* FUN:	cmd_add_empty
* SUBJ:	Add empty concept of hierarchy.
* CR:	9/20/95 AM.
* NOTE:	Must verify that this is the 1st concept.
* RET:	ok - If command successful.
*	REIMPLEMENT.	// 02/26/07 AM.
*
**************************************************/

LIBQCONSH_API bool
cmd_add_empty(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
// NOT ADDING A "NULL" CONCEPT TO QDBM.	// 02/26/07 AM.
return true;	// 02/26/07 AM.
//char *str;

if (args)
   {
   *out << _T("Too many args in ADD EMPTY command.") << endl;
   return(false);
   }

XCON_S con_x;	// Build a struct, automatically freed on scope exit.
XCON_S *con = &con_x;

//SYM *sym;
//bool dirt;	// 06/29/03 AM.


// Todo:	Check that db is reset and counter is at 0.

int empty_cid = 0;
STR_ID sid = cg->qkbm_->addString(CON_EMPTY_NAME);

con->kind = cXBASIC;
con->v.vname = sid;
con->flags = 0;
con->attrs = 0;
con->up = 0;
con->dn = 0;
con->prev = 0;
con->next = 0;
con->atom = 0;
con->phrases = 0;

cg->qkbm_->con_update(empty_cid, con);	// ADD TO DB.

//cg->cgcon_->con_add_nh(CON_EMPTY_NAME);
return(true);
}


/**************************************************
*						CMD_ADD_HIER
* FUN:	cmd_add_hier
* SUBJ:	Add a concept to the hierarchy by its path.
* CR:	10/15/95 AM.
* ALG:	Traverse down path given by args, starting with "concept", the
*		root of the concept tree.  Start building from first nonexistent
*		name in hierarchy path.
* RET:	ok - If command successful.
* DES:	Of course there's a problem if two or more siblings share a name.
*			(KBMS should disallow it.)
*
**************************************************/

LIBQCONSH_API bool
cmd_add_hier(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
_TCHAR *name;		// Latest name in hierarchy path.
XCON_S *con,		// Hierarchy path built so far.
    *sub,		// Next concept in path.
    *child;		// Hierarchic children.
CON_ID child_id;	// 02/15/07 AM.
CON_ID cid;
bool dirty = false;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Too few args in ADD HIER command.") << endl;
   return(false);
   }

// Set up root of hierarchy.
name = ALIST::list_pop_buf(&args,buf);
if (_tcscmp(name, CON_ROOT_NAME))
   {
   *out << _T("add hier: Path must begin with '")
		  << CON_ROOT_NAME << _T("'.")
        << endl;
   return(false);
   }
con = cg->qkbm_->Con(cg->cg_CONCEPT);

if (!con)
   {
   *out << _T("add hier: Root concept absent.") << endl;
   return(false);
   }

// Find first path component not yet built.
STR_ID sid = 0;	// 02/15/07 AM.
STR_ID c_sid = 0;	// 02/15/07 AM.
while (name = ALIST::list_pop_buf(&args,buf))
   {
   sid = cg->qkbm_->sym_get(name, /*UP*/dirty);	// 02/27/07 AM.
   child_id = con->dn;
   sub = 0;
   while (child_id)
      {
      child = cg->qkbm_->Con(child_id);	// 02/15/07 AM.
      c_sid = cg->qkbm_->con_sid(child);	// 02/15/07 AM.
      if (child->kind == cXBASIC && sid == c_sid)
         {
         sub = child;
         break;
         }
      child_id = child->next;
      cg->kbfree(child);	// 02/15/07 AM.
      }
   if (sub)
		{
		cg->kbfree(con);	// 02/15/07 AM.
      con = sub;
      }
   else
      break;
   }

if (!name)
   {
	// Allowing this redundancy.											// 10/27/00 AM.
   *out << _T("[add hier: Hierarchy already exists.]") << endl;
   //return(false);															// 10/27/00 AM.
	cg->kbfree(con);	// 02/15/07 AM.
	return true;																// 10/27/00 AM.
   }

// Now build.
XCON_S *newcon;	// 02/15/07 AM.
while (name)
   {
   cid = cg->cgcon_->con_add_basic(name, con);
   newcon = cg->qkbm_->Con(cid);
   cg->kbfree(con);	// 02/15/07 AM.
   con = newcon;		// 02/15/07 AM.
   name = ALIST::list_pop_buf(&args,buf);
   }

cg->kbfree(con);	// Could return this ...	// 02/15/07 AM.
return(true);
}


/**************************************************
*						CMD_ADD_PUNCT
* FUN:	cmd_add_punct
* SUBJ:	Add the punctuation chars to the kb.
* CR:	11/1/95 AM.
* RET:	ok - If command successful.
* NOTE:	Specialized, hardwired command.
*		Consh - to be used for dictionary building.
* FORM:	add white
*
**************************************************/

LIBQCONSH_API bool
cmd_add_punct(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
XCON_S *con;

/*** PROCESS COMMANDS ***/
if (args)
   {
   *out << _T("Too many args in ADD PUNCT command.") << endl;
   return(false);
   }

/*** CHECK ***/

if (!cg->nlp_PUNCT)
   {
   *out << _T("Add punct: No punct hierarchy root.") << endl;
   return(false);
   }
con = cg->qkbm_->Con(cg->nlp_PUNCT);
CON_ID down = con->dn;	// 03/05/07 AM.
cg->kbfree(con);			// 03/05/07 AM.
if (down)
   {
   *out << _T("Add punct: Hierarchy already present.") << endl;
   return(false);
   }

/*** EXECUTE ***/

return(cg->cgdict_->dict_add_punct());
}


/**************************************************
*						CMD_ADD_ROOT
* FUN:	cmd_add_root
* SUBJ:	Add root concept of hierarchy.
* CR:	9/18/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_add_root(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)

{
//char *str;

if (args)
   {
   *out << _T("Too many args in ADD ROOT command.") << endl;
   return(false);
   }

cg->cgcon_->con_add_root();
return(true);
}


/**************************************************
*						CMD_ADD_WHITE
* FUN:	cmd_add_white
* SUBJ:	Add the whitespace chars to the kb.
* CR:	11/1/95 AM.
* RET:	ok - If command successful.
* NOTE:	Specialized, hardwired command.
*		Consh - to be used for dictionary building.
* FORM:	add white
*
**************************************************/

LIBQCONSH_API bool
cmd_add_white(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
//char *name;
//SYM *sym;
//XCON_S *con;
XCON_S *tmp;

/*** PROCESS COMMANDS ***/
if (args)
   {
   *out << _T("Too many args in ADD WHITE command.") << endl;
   return(false);
   }

/*** CHECK ***/

if (!cg->nlp_WHT)
   {
   *out << _T("Add white: No white hierarchy root.") << endl;
   return(false);
   }
tmp = cg->qkbm_->Con(cg->nlp_WHT);
CON_ID down = tmp->dn;	// 03/05/07 AM.
cg->kbfree(tmp);			// 03/05/07 AM.
if (down)
   {
   *out << _T("Add white: Hierarchy already present.") << endl;
   return(false);
   }

/*** EXECUTE ***/

return(cg->cgdict_->dict_add_white());
}


/**************************************************
*						CMD_ADD_WORD
* FUN:	cmd_add_word
* SUBJ:	Add a word to the kb.
* CR:	10/22/95 AM.
* RET:	ok - If command successful.
* NOTE:	"word" means an English word, proper name, etc.
*		Something that would appear in an English dictionary, eg.
*		We make it known as a word in the system, with no syntax class
*		nor semantics here.
*		Consh - to be used for dictionary building.
*		Conan - probably will use cmd_add_unknown instead, for speed.
* ALG:	Get sym.  If it has no concept, add one to the alpha
*		hierarchy.
* FORM:	add word NAME
*
**************************************************/

LIBQCONSH_API bool
cmd_add_word(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
_TCHAR *name;
_TCHAR *buf = cg->alist_->List_buffer;
//SYM *sym;
//XCON_S *con;

/*** PROCESS AND CHECK COMMANDS ***/
if (!args)
   {
   *out << _T("Too few args in ADD WORD command.") << endl;
   return(false);
   }

name = ALIST::list_pop_buf(&args,buf);

if (args)
   {
   *out << _T("Too many args in ADD WORD command.") << endl;
   return(false);
   }

/*** CHECK ***/

if (!name || !*name)
   return(false);

// 05/02/99 AM. Allowing nonalphabetics through here.
//if (!isalpha((unsigned char)*name))	/* Must be alphabetic. */
//   {
//   *out << "add word: Nonalphabetic=" << name << endl;
//   }

/*** EXECUTE ***/


//if (cg->cgdict_->dict_add_word(name))				// 04/22/03 AM.
// NEED A FAST METHOD FOR LOADING THE DICTIONARY.	// 03/06/07 AM.

// Not an elegant way to init....
if (!cg->cgdict_->getC_one())	// 03/06/07 AM.
	cg->cgdict_->dict_load_ini();	// 03/06/07 AM.

if (cg->cgdict_->dict_load_word(name))					// 03/06/07 AM.
  return true;								// 04/22/03 AM.
*out << _T("[add word: Failed to add '") << name << _T("']") << endl;	// 04/22/03 AM.
return false;								// 04/22/03 AM.
}


/**************************************************
*						CMD_BIND
* FUN:	cmd_bind
* SUBJ:	Commands to bind knowledge base with code.
* CR:	10/19/95 AM.
* RET:	ok - If command successful.
* NOTE:	Part of bootstrapping the knowledge base.
*
**************************************************/

LIBQCONSH_API bool
cmd_bind(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Too few args in BIND command.") << endl;
   return(false);
   }

str = ALIST::list_pop_buf(&args,buf);

if (!_tcscmp(str, _T("app")))				/* Bind application concepts.	*/
   return(cmd_bind_app(args, out,cg));
else if (!_tcscmp(str, _T("sys")))			/* Bind system concepts.		*/
   return(cmd_bind_sys(args, out,cg));
else
   {
   *out << _T("Unknown command= bind ") << str << _T(".") << endl;
   return(false);
   }
}


/**************************************************
*						CMD_BIND_APP
* FUN:	cmd_bind_app
* SUBJ:	Bind application-specific concepts to code.
* CR:	12/5/95 AM.
* RET:	ok - If command successful.
* NOTE:	Part of bootstrapping the knowledge base.
*
**************************************************/

LIBQCONSH_API bool
cmd_bind_app(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
//char *str;
//XCON_S *sys, *attrs, *proxy, *pproxy;

if (args)
   {
   *out << _T("Too many args in BIND APP command.") << endl;
   return(false);
   }

return(bind_app());
}


/**************************************************
*						CMD_BIND_SYS
* FUN:	cmd_bind_sys
* SUBJ:	Bind system internal concepts with code.
* CR:	10/19/95 AM.
* RET:	ok - If command successful.
* NOTE:	Part of bootstrapping the knowledge base.
*
**************************************************/

LIBQCONSH_API bool
cmd_bind_sys(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
//char *str;
//XCON_S *sys, *attrs, *proxy, *pproxy;

if (args)
   {
   *out << _T("Too many args in BIND SYS command.") << endl;
   return(false);
   }

if (!cg->cg_CONCEPT)
   {
   *out << _T("Bind sys: Root concept not defined.") << endl;
   return(false);
   }

/*
if (cg->cg_PHRASE
    || cg->cg_ATOM)
   {
   *out << _T("Bind sys: Some concepts defined already.") << endl;
   return(false);
   }
*/
return(bind_sys(cg));
}


/**************************************************
*						CMD_CON
* FUN:	cmd_con
* SUBJ:	Concept table management primitives.
* CR:	9/20/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_con(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
#ifdef OLD_070215_
_TCHAR *str;
long id;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   cg->qkbm_->con_tab_pretty(out);
   return(true);
   }

str = ALIST::list_pop_buf(&args,buf);

if (!_tcscmp(str, _T("pp")))
   {
   cg->qkbm_->con_tab_pretty(out);
   return(true);
   }
else if (s_to_l(str, /*UP*/ &id))
   {
   con_pret(cg->qkbm_->Con(id), out, cg);
   return(true);
   }
else
   {
   *out << _T("Unknown command= con ") << str << _T(".") << endl;
   return(false);
   }
#endif
return(true);
}


/**************************************************
*						CMD_GEN
* FUN:	cmd_gen
* SUBJ:	Code generation commands.
* CR:	10/1/95 AM.
* RET:	ok - If command successful.
* EFF:	Creates generated code files in Resume Project.
* NOTE:	Need to parameterize where code files go.
*
**************************************************/

LIBQCONSH_API bool
cmd_gen(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {\
   *out << _T("Too few args in \"GEN\" command.") << endl;
   return(false);
   }

str = ALIST::list_pop_buf(&args,buf);

*cgerr << _T("[cmd gen: Command not updated. 07/01/00 AM.]") << endl;
return false;

#ifdef OLD_
if (!_tcscmp(str, _T("all")))				/* Generate all code for app.		*/
   return(cmd_gen_all(args, out));
else if (!_tcscmp(str, _T("st")))			/* Generate code for string table.	*/
   return(cmd_st_gen(args, out));
else
   {
   *out << _T("Unknown command= gen ") << str << _T(".") << endl;
   return(false);
   }
#endif

return(true);
}


/**************************************************
*						CMD_GEN_ALL
* FUN:	cmd_gen_all
* SUBJ:	Command to generate code for kb mgr.
* CR:	10/1/95 AM.
* RET:	ok - If command successful.
* EFF:	Overwrites generated code files in app project.
*
**************************************************/

LIBQCONSH_API bool
cmd_gen_all(
	_TCHAR *dir,		// Directory to gen code to.						// 07/01/00 AM.
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
// NOT USED IN QDBM version of Conceptual Grammar KBMS.	// 02/15/07 AM.
return false;	// 02/15/07 AM.
#ifdef OLD_070215_

_TCHAR *tail;

if (args)
   {
   *out << _T("Too many args in \"gen all\" command.") << endl;
   return(false);
   }

/* tail = ".cc"; */
tail = _T("");			/* 10/1/95 AM */

_t_cerr << _T("Generating overall code.") << endl;
cc_gen_hdr(dir, tail);
cc_gen_ini(dir, tail);

/* Generate analyzer code first, because it modifies the kb.
   Creates trigger attributes, for example. */
_t_cerr << _T("Generating analyzer code.") << endl;
_t_cerr << _T("[04/23/99 AM. Turned off analyzer code gen.") << endl;
//ana_gen(dir, tail);

_t_cerr << _T("Generating string table code.") << endl;
st_gen(dir, tail,cg);
st_gen_hdr(dir, tail,cg);
st_gen_ini(dir, tail,cg);

_t_cerr << _T("Generating symbol table code.") << endl;
sym_gen(dir, tail,cg);
sym_gen_hdr(dir, tail,cg);
sym_gen_ini(dir, tail,cg);

_t_cerr << _T("Generating concept table code.") << endl;
con_gen(dir, tail,cg);
con_gen_hdr(dir, tail,cg);
con_gen_ini(dir, tail,cg);

_t_cerr << _T("Generating pointer table code.") << endl;
ptr_gen(dir, tail,cg);
ptr_gen_hdr(dir, tail,cg);
ptr_gen_ini(dir, tail,cg);

// NO.  Using libconsh definitions and dynamic binding.			// 06/28/00 AM.
//cerr << "Generating defines code." << endl;
//var_gen(dir, tail);

// GENERATE A MAKEFILE														// 07/27/01 AM.
// Need to get file counts from above calls.							// 07/27/01 AM.
gen_makefile(dir,cg);														// 07/27/01 AM.

//cerr << "Done generating code for Resume Project." << endl;	// 06/28/00 AM.
_t_cerr << _T("[Done generating code for knowledge base.]") << endl;	// 06/28/00 AM.
return(true);
#endif
}


/**************************************************
*						CMD_IND
* FUN:	cmd_ind
* SUBJ:	Higher-level knowledge addition commands.
* CR:	10/15/95 AM.
* RET:	ok - If command successful.
* NOTE:	Supports multi-line, interactive addition cmds.
*
**************************************************/

LIBQCONSH_API bool
cmd_ind(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	bool i_flag,
	bool silent,
	CG *cg																		// 08/15/02 AM.
	)
{
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Too few args in IND command.") << endl;
   return(false);
   }

str = ALIST::list_pop_buf(&args,buf);

//if (!strcmp(str, "action"))
//   return(ind_action(args, in, out, i_flag, silent,cg));
//else
if (!_tcscmp(str, _T("attr")))
   return(ind_attr(args, in, out, i_flag, silent,cg));
else if (!_tcscmp(str, _T("wattr")))	// Add attr to a word-concept	// 05/30/02 AM.
   return(ind_wattr(args, in, out, i_flag, silent,cg));					// 05/30/02 AM.
else if (!_tcscmp(str, _T("nattr")))	// NEW IND ATTR!					// 10/08/05 AM.
   return(ind_nattr(args, in, out, i_flag, silent,cg));					// 10/08/05 AM.
else if (!_tcscmp(str, _T("childs")))
   return(ind_childs(args, in, out, i_flag, silent,cg));
else if (!_tcscmp(str, _T("named_phrase")))
   return(ind_named_phrase(args, in, out, i_flag, silent,cg));
else if (!_tcscmp(str, _T("phrase")))
   return(ind_phrase(args, in, out, i_flag, silent,cg));
//else if (!strcmp(str, "rest"))
//   return(ind_rest(args, in, out, i_flag, silent,cg));
#ifdef OBSOLETE
else if (!_tcscmp(str, _T("proxy")))
   return(ind_proxy(args, in, out, i_flag));
#endif
else
   {
   *out << _T("Unknown command= ind ") << str << _T(".") << endl;
   return(false);
   }
}


/**************************************************
*						CMD_PROXY
* FUN:	cmd_proxy
* SUBJ:	Commands related to proxies.
* CR:	12/12/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_proxy(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
#ifdef OLD_070215_
_TCHAR *s_id, *s_ord, *usage;
long id;
long ord;
XCON_S *con, *phr, *prox;
CON_ID phr_nid;	// 03/05/07 AM.
_TCHAR *buf = cg->alist_->List_buffer;

usage = _T("Usage: proxy ID NTH");

if (!args)
   {
   *out << usage << endl;
   return(false);
   }

s_id = ALIST::list_pop_buf(&args,buf);

if (!args)
   {
   *out << usage << endl;
   return(false);
   }

s_ord = ALIST::list_pop_buf(&args,buf);

if (!s_to_l(s_id, /*UP*/ &id) || !s_to_l(s_ord, /*UP*/ &ord))
   {
   *out << usage << endl;
   return(false);
   }

if (!(con = cg->qkbm_->Con(id)) || !(phr_nid = cg->qkbm_->con_phrase(con)))
   {
   *out << usage << endl;
   return(false);
   }

if (!(prox = cg->qkbm_->con_nth(phr, ord)))
   {
   *out << usage << endl;
   return(false);
   }

con_pret(prox, out, cg);
#endif
return(true);
}


/**************************************************
*						CMD_SHOW
* FUN:	cmd_show
* SUBJ:	Commands to display information for user.
* CR:	12/10/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_show(
	LIST *args,
	_t_ostream *out,
	CG *cg																		// 08/15/02 AM.
	)
{
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   return(false);
   }

str = ALIST::list_pop_buf(&args,buf);

if (!_tcscmp(str, _T("name")))				/* Show instances of name in kb.*/
   return(cmd_show_name(args, out,cg));
else
   {
   *out << _T("Unknown command= sym ") << str << _T(".") << endl;
   return(false);
   }
return(true);
}


/**************************************************
*						CMD_SHOW_NAME
* FUN:	cmd_st_add
* SUBJ:	Display instances of name in kb.
* CR:	12/10/95 AM.
* RET:	ok - If command successful.
*
**************************************************/

LIBQCONSH_API bool
cmd_show_name(
	LIST *args,
	_t_ostream *out,
	CG *cg																		// 08/15/02 AM.
	)
{
#ifdef OLD_070215_
_TCHAR *str;
//char *ptr;
LIST *list=0, *end=0, *elt;
XCON_S *con;
ALIST *alist = cg->alist_;	// List manager.							// 08/14/02 AM.
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Usage: show name NAME") << endl;
   return(false);
   }
str = ALIST::list_pop_buf(&args,buf);
if (args)
   {
   *out << _T("Too many args in \"show name\" command.") << endl;
   return(false);
   }

// This is ancient code that's hosed.									// 08/15/02 AM.
/* Find name in kb. */
//list = cg->qkbm_->find_name(str /*UP*/ &end);

/* Display data nicely. */
_TCHAR *kstr, *qstr;	// 03/05/07 AM.
for (elt = list; elt; elt = elt->next)
   {
   if (con = (XCON_S *) elt->val)
		{
		kstr = cg->qkbm_->con_kind_str(con);	// 03/05/07 AM.
		qstr = cg->qkbm_->con_str(con);			// 03/05/07 AM.
      *out << con->id << _T(": (")
			  << kstr
			  << _T(") ")
			  << qstr
			  << _T(" ") << endl;
		cg->kbfree(kstr);	// 03/05/07 AM.
		cg->kbfree(qstr);	// 03/05/07 AM.
		}
   }

/* Clean up */
alist->list_free(list, end);
#endif
return(true);
}


/**************************************************
*						CMD_ST
* FUN:	cmd_st
* SUBJ:	String table management primitives.
* CR:	8/27/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_st(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
/*
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   cg->ast_->st_pp(out);
   return(true);
   }
str = ALIST::list_pop_buf(&args,buf);

if (!_tcscmp(str, _T("add")))				// Add string to string table.
   return(cmd_st_add(args, out,cg));
else if (!_tcscmp(str, _T("gen")))			// Generate code for string table.
   return(cmd_st_gen(args, out,cg));
else
   {
   *out << _T("Unknown command= st ") << str << _T(".") << endl;
   return(false);
   }
*/
return(true);
}


/**************************************************
*						CMD_ST_ADD
* FUN:	cmd_st_add
* SUBJ:	String table primitive add command.
* CR:	8/27/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_st_add(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
/*
_TCHAR *str;
_TCHAR *ptr;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Usage: st add \"STRING\"") << endl;
   return(false);
   }
str = ALIST::list_pop_buf(&args,buf);
if (args)
   {
   *out << _T("Too many args in \"st add\" command.") << endl;
   return(false);
   }
   
return(cg->ast_->st_add(str, &ptr));
*/
return true;	// 02/14/07 AM.
}


/**************************************************
*						CMD_ST_GEN
* FUN:	cmd_st_gen
* SUBJ:	Command to generate string table code.
* CR:	8/27/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_st_gen(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
return false;	// 02/15/07 AM.
#ifdef OLD_070215_

_TCHAR *dir, *s_base, *tail;

if (args)
   {
   *out << _T("Too many args in \"st gen\" command.") << endl;
   return(false);
   }

dir = _T("::Resume Project");
s_base = _T("St");
/* tail = ".cc"; */
tail = _T("");			/* 10/1/95 AM */

st_gen(dir, tail,cg);
st_gen_hdr(dir, tail,cg);
st_gen_ini(dir, tail,cg);

return(true);
#endif
}


/**************************************************
*						CMD_SYM
* FUN:	cmd_sym
* SUBJ:	Symbol table management primitives.
* CR:	9/2/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_sym(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
return false;	// 02/15/07 AM.
/*
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   cg->qkbm_->sym_pp(out);
   return(true);
   }

str = ALIST::list_pop_buf(&args,buf);

if (!_tcscmp(str, _T("add")))				// Add string to sym table.
   return(cmd_sym_add(args, out,cg));
else if (!_tcscmp(str, _T("gen")))			// Generate code for sym table.
   return(cmd_sym_gen(args, out,cg));
else if (!_tcscmp(str, _T("pp")))
   {
   cg->qkbm_->sym_pp(out);
   return(true);
   }
else if (!_tcscmp(str, _T("stat")))
   return(cmd_sym_stat(args, out,cg));
else
   {
   *out << _T("Unknown command= sym ") << str << _T(".") << endl;
   return(false);
   }
return(true);
*/
}


/**************************************************
*						CMD_SYM_ADD
* FUN:	cmd_sym_add
* SUBJ:	Command to add a string to symbol table.
* CR:	9/3/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_sym_add(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
return false;	// 02/15/07 AM.
/*
_TCHAR *str;
_TCHAR *buf = cg->alist_->List_buffer;

if (!args)
   {
   *out << _T("Usage: sym add \"STRING\"") << endl;
   return(false);
   }
str = ALIST::list_pop_buf(&args,buf);
if (args)
   {
   *out << _T("Too many args in \"sym add\" command.") << endl;
   return(false);
   }
   
(void) cg->qkbm_->sym_add(str);
return(true);
*/
}


/**************************************************
*						CMD_SYM_GEN
* FUN:	cmd_sym_gen
* SUBJ:	Command to generate sym table code.
* CR:	9/3/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_sym_gen(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{

if (args)
   {
   *out << _T("Too many args in \"st add\" command.") << endl;
   return(false);
   }

/* Generate sym table code here. */
*out << _T("[Unimplemented]") << endl;

return(true);
}


/**************************************************
*						CMD_SYM_STAT
* FUN:	cmd_sym_stat
* SUBJ:	Command to show sym table statistics.
* CR:	9/3/95 AM.
* RET:	ok - If command successful.
* NOTE:	For system developer only.
* WARN:	Can introduce inconsistencies into kb.
*
**************************************************/

LIBQCONSH_API bool
cmd_sym_stat(
	LIST *args,
	_t_ostream *out,
	CG *cg
	)
{
return false;	// 02/15/07 AM.
/*
if (args)
   {
   *out << _T("Too many args in \"sym stat\" command.") << endl;
   return(false);
   }

cg->qkbm_->sym_stat(out);

return(true);
*/
}


/**************************************************
*						CMD_TAKE
* FUN:	cmd_take
* SUBJ:	Execute commands from a file.
* CR:	9/22/95 AM.
* RET:	ok - If command successful.
* NOTE:	Must handle nested calls to TAKE from within files.
*
**************************************************/

LIBQCONSH_API bool
cmd_take(
	LIST *args,
	_t_istream *in,
	_t_ostream *out,
	CG *cg																		// 08/15/02 AM.
	)
{
_TCHAR *fil;
//ifstream *fin;						// 03/08/00 AM.
bool ok;
_TCHAR *buf = cg->alist_->List_buffer;

#ifdef OLD_030701_
_TCHAR *dir;
if (!args)
   {
   *out << _T("Too few args in TAKE command.") << endl;
   return(false);
   }

dir = ALIST::list_pop_buf(&args,buf);
#endif

if (!args)
   {
   *out << _T("Too few args in TAKE command.") << endl;
   return(false);
   }

fil = ALIST::list_pop_buf(&args,buf);

if (args)
   {
   *out << _T("Too many args in TAKE command.") << endl;
   return(false);
   }

// Handling relative filename.											// 07/01/03 AM.
_TCHAR buf[FNAME_SIZE+1];														// 07/01/03 AM.
if (!resolve_file(fil,cg->getAppdir(),/*DU*/buf))					// 07/01/03 AM.
	{
	_t_cerr << _T("[Bad filename=") << fil << _T("]") << endl;
	return false;
	}

/* Create full path to knowledge command file. */
/* Rebind input path. */
//if (!file_open(fil, "r", &fin))
//   return(false);

if (!f_exists(buf))
	{
	_t_cerr << _T("[File=") << buf << _T(" doesn't exist.]") << endl;
	return false;
	}

// G++ BUGS.																	// 03/08/00 AM.
//fin = new ifstream(fil, ios::in | ios::nocreate);				// 04/30/99 AM.
//ifstream fin(fil, ios::in | ios::nocreate);						// 03/08/00 AM.
_t_ifstream fin(TCHAR2A(buf), ios::in);								// Upgrade.	// 01/24/01 AM.
if (!fin)		// 04/30/99 AM.
	{
	_t_cerr << _T("[Couldn't open file ") << buf << _T("]") << endl;
	return false;
	}
#ifdef UNICODE
if (!u_readbom(&fin))
	{
	*cgerr << _T("[take: Unhandled Unicode format=") << buf << _T("]") << endl;
	return false;
	}
#endif

/* Start up command loop for reading file. */
// SILENT READ OR NOT.														// 03/27/00 AM.
//ok = cmd_top(fin, out, "> ", false);									// 03/08/00 AM.
ok = cmd_top(&fin, out, _T("> "),true,cg);								// 03/27/00 AM.

/* Restore input path. */
//file_close(fil, fin);
//delete fin;																	// 03/09/00 AM.

return(ok);
}


/********************************************
* FN:		GEN_MAKEFILE
* SUBJ:	Generate makefile for the KB.
* CR:		07/27/01 AM.
********************************************/

bool gen_makefile(_TCHAR *dir, CG *cg)
{
return false;	// 02/15/07 AM.
/*
if (!dir || !*dir)
	return false;

// Simplest to run the makefile from the kb directory.			// 07/29/01 AM.
gen_kbbat(dir);																// 07/29/01 AM.

int st_segs  = cg->ast_->st_segs_tot();
int sym_segs = cg->qkbm_->sym_segs_tot();
int ptr_segs = cg->qkbm_->ptr_segs_tot();
int con_segs = cg->qkbm_->con_segs_tot();
int ii;	// Counter.

_TCHAR s_nam[PATH];
_stprintf(s_nam, _T("%s%skb.mak"), dir, DIR_SEP);
_t_ofstream *fp = new _t_ofstream(TCHAR2A(s_nam));

*fp << _T("# VISUALTEXT Generated NMAKE File, Based on kb.dsp\n");
*fp << _T("!IF \"$(CFG)\" == \"\" \n");
*fp << _T("CFG=kb - Win32 Debug\n");
*fp << _T("!MESSAGE No configuration specified. Defaulting to kb - Win32 Debug.\n");
*fp << _T("!ENDIF \n");
*fp << _T("\n");
*fp << _T("!IF \"$(CFG)\" != \"kb - Win32 Release\" && \"$(CFG)\" != \"kb - Win32 Debug\"\n");
*fp << _T("!MESSAGE Invalid configuration \"$(CFG)\" specified.\n");
*fp << _T("!MESSAGE You can specify a configuration when running NMAKE\n");
*fp << _T("!MESSAGE by defining the macro CFG on the command line. For example:\n");
*fp << _T("!MESSAGE \n");
*fp << _T("!MESSAGE NMAKE /f \"kb.mak\" CFG=\"kb - Win32 Debug\"\n");
*fp << _T("!MESSAGE \n");
*fp << _T("!MESSAGE Possible choices for configuration are:\n");
*fp << _T("!MESSAGE \n");
*fp << _T("!MESSAGE \"kb - Win32 Release\" (based on \"Win32 (x86) Dynamic-Link Library\")\n");
*fp << _T("!MESSAGE \"kb - Win32 Debug\" (based on \"Win32 (x86) Dynamic-Link Library\")\n");
*fp << _T("!MESSAGE \n");
*fp << _T("!ERROR An invalid configuration is specified.\n");
*fp << _T("!ENDIF \n");
*fp << _T("\n");
*fp << _T("!IF \"$(OS)\" == \"Windows_NT\"\n");
*fp << _T("NULL=\n");
*fp << _T("!ELSE \n");
*fp << _T("NULL=nul\n");
*fp << _T("!ENDIF \n");
*fp << _T("\n");

*fp << _T("\n");
*fp << _T("!IF  \"$(CFG)\" == \"kb - Win32 Release\"\n");
*fp << _T("\n");
*fp << _T("OUTDIR=.\\Release\n");
*fp << _T("INTDIR=.\\Release\n");
*fp << _T("# Begin Custom Macros\n");
*fp << _T("OutDir=.\\Release\n");
*fp << _T("# End Custom Macros\n");
*fp << _T("\n");
*fp << _T("ALL : \"$(OUTDIR)\\kb.dll\"\n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("CLEAN :\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.obj\"\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.pch\"\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.idb\"\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.pdb\"\n");
*fp << _T("	-@erase \"$(OUTDIR)\\*.dll\"\n");
*fp << _T("	-@erase \"$(OUTDIR)\\*.exp\"\n");
*fp << _T("	-@erase \"$(OUTDIR)\\*.lib\"\n");
*fp << _T("\n");
*fp << _T("\"$(OUTDIR)\" :\n");
*fp << _T("    if not exist \"$(OUTDIR)/$(NULL)\" mkdir \"$(OUTDIR)\"\n");
*fp << _T("\n");
*fp << _T("CPP=cl.exe\n");
*fp << _T("CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /I \"$(TAI)\\include\\Api\" /D \"WIN32\" /D \"NDEBUG\" /D \"_WINDOWS\" /D \"_MBCS\" /D \"_USRDLL\" /D \"KB_EXPORTS\" /Fp\"$(INTDIR)\\kb.pch\" /Yu\"StdAfx.h\" /Fo\"$(INTDIR)\\\\\" /Fd\"$(INTDIR)\\\\\" /FD /c \n");
*fp << _T("\n");
*fp << _T(".c{$(INTDIR)}.obj::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cpp{$(INTDIR)}.obj::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cxx{$(INTDIR)}.obj::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".c{$(INTDIR)}.sbr::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cpp{$(INTDIR)}.sbr::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cxx{$(INTDIR)}.sbr::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T("MTL=midl.exe\n");
*fp << _T("MTL_PROJ=/nologo /D \"NDEBUG\" /mktyplib203 /win32 \n");
*fp << _T("RSC=rc.exe\n");
*fp << _T("BSC32=bscmake.exe\n");
*fp << _T("BSC32_FLAGS=/nologo /o\"$(OUTDIR)\\kb.bsc\" \n");
*fp << _T("BSC32_SBRS= \\\n");
*fp << _T("	\n");
_TCHAR *env = _tgetenv(_T("VCBIN"));	// In case machine has LINK.EXE conflict.	// 11/22/02 AM.
if (env && *env)																			// 11/22/02 AM.
	*fp << _T("LINK32=\"$(VCBIN)\\link.exe\"\n");										// 11/22/02 AM.
else																							// 11/22/02 AM.
	*fp << _T("LINK32=link.exe\n");
*fp << _T("LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libkbm.lib /nologo /dll /incremental:no /pdb:\"$(OUTDIR)\\kb.pdb\" /debug /machine:I386 /def:\"kb.def\" /out:\"$(OUTDIR)\\kb.dll\" /implib:\"$(OUTDIR)\\kb.lib\" /libpath:\"$(TAI)\\lib\" /fixed:no \n");
*fp << _T("\n");

*fp << _T("LINK32_OBJS= \\\n");
for (ii = 0; ii < con_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\Con") << ii << _T(".obj\" \\\n");
for (ii = 0; ii < ptr_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\Ptr") << ii << _T(".obj\" \\\n");
for (ii = 0; ii < st_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\St") << ii << _T(".obj\" \\\n");
for (ii = 0; ii < sym_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\Sym") << ii << _T(".obj\" \\\n");

*fp << _T("	\"$(INTDIR)\\StdAfx.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\kb.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\kb_setup.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Cc_code.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Con_ini.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Ptr_ini.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\St_ini.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Sym_ini.obj\"\n");
*fp << _T("\n");

*fp << _T("\"$(OUTDIR)\\kb.dll\" : \"$(OUTDIR)\" $(DEF_FILE) $(LINK32_OBJS)\n");
*fp << _T("    $(LINK32) @<<\n");
*fp << _T("  $(LINK32_FLAGS) $(LINK32_OBJS)\n");
*fp << _T("<<\n");
*fp << _T("\n");

*fp << _T("!ELSEIF  \"$(CFG)\" == \"kb - Win32 Debug\"\n");
*fp << _T("\n");
*fp << _T("OUTDIR=.\\Debug\n");
*fp << _T("INTDIR=.\\Debug\n");
*fp << _T("# Begin Custom Macros\n");
*fp << _T("OutDir=.\\Debug\n");
*fp << _T("# End Custom Macros\n");
*fp << _T("\n");
*fp << _T("ALL : \"$(OUTDIR)\\kb.dll\"\n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("CLEAN :\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.obj\"\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.pch\"\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.idb\"\n");
*fp << _T("	-@erase \"$(INTDIR)\\*.pdb\"\n");
*fp << _T("	-@erase \"$(OUTDIR)\\*.dll\"\n");
*fp << _T("	-@erase \"$(OUTDIR)\\*.exp\"\n");
*fp << _T("	-@erase \"$(OUTDIR)\\*.lib\"\n");
*fp << _T("\n");
*fp << _T("\"$(OUTDIR)\" :\n");
*fp << _T("    if not exist \"$(OUTDIR)/$(NULL)\" mkdir \"$(OUTDIR)\"\n");
*fp << _T("\n");
*fp << _T("CPP=cl.exe\n");
*fp << _T("CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I \"$(TAI)\\include\\Api\" /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"_MBCS\" /D \"_USRDLL\" /D \"KB_EXPORTS\" /Fp\"$(INTDIR)\\kb.pch\" /Yu\"StdAfx.h\" /Fo\"$(INTDIR)\\\\\" /Fd\"$(INTDIR)\\\\\" /FD /GZ /c \n");
*fp << _T("\n");
*fp << _T(".c{$(INTDIR)}.obj::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cpp{$(INTDIR)}.obj::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cxx{$(INTDIR)}.obj::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".c{$(INTDIR)}.sbr::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cpp{$(INTDIR)}.sbr::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T(".cxx{$(INTDIR)}.sbr::\n");
*fp << _T("   $(CPP) @<<\n");
*fp << _T("   $(CPP_PROJ) $< \n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T("MTL=midl.exe\n");
*fp << _T("MTL_PROJ=/nologo /D \"_DEBUG\" /mktyplib203 /win32 \n");
*fp << _T("RSC=rc.exe\n");
*fp << _T("BSC32=bscmake.exe\n");
*fp << _T("BSC32_FLAGS=/nologo /o\"$(OUTDIR)\\kb.bsc\" \n");
*fp << _T("BSC32_SBRS= \\\n");
*fp << _T("	\n");
if (env && *env)																			// 11/22/02 AM.
	*fp << _T("LINK32=\"$(VCBIN)\\link.exe\"\n");										// 11/22/02 AM.
else																							// 11/22/02 AM.
	*fp << _T("LINK32=link.exe\n");
*fp << _T("LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libkbmd.lib /nologo /dll /incremental:no /pdb:\"$(OUTDIR)\\kb.pdb\" /debug /machine:I386 /def:\"kb.def\" /out:\"$(OUTDIR)\\kb.dll\" /implib:\"$(OUTDIR)\\kb.lib\" /libpath:\"$(TAI)\\lib\" /fixed:no \n");
*fp << _T("\n");

*fp << _T("LINK32_OBJS= \\\n");
for (ii = 0; ii < con_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\Con") << ii << _T(".obj\" \\\n");
for (ii = 0; ii < ptr_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\Ptr") << ii << _T(".obj\" \\\n");
for (ii = 0; ii < st_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\St") << ii << _T(".obj\" \\\n");
for (ii = 0; ii < sym_segs; ++ii)
	*fp << _T("	\"$(INTDIR)\\Sym") << ii << _T(".obj\" \\\n");

*fp << _T("	\"$(INTDIR)\\StdAfx.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\kb.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\kb_setup.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Cc_code.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Con_ini.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Ptr_ini.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\St_ini.obj\" \\\n");
*fp << _T("	\"$(INTDIR)\\Sym_ini.obj\"\n");
*fp << _T("\n");

*fp << _T("\"$(OUTDIR)\\kb.dll\" : \"$(OUTDIR)\" $(DEF_FILE) $(LINK32_OBJS)\n");
*fp << _T("    $(LINK32) @<<\n");
*fp << _T("  $(LINK32_FLAGS) $(LINK32_OBJS)\n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T("!ENDIF \n");

*fp << _T("\n");
*fp << _T("!IF \"$(NO_EXTERNAL_DEPS)\" != \"1\"\n");
*fp << _T("!IF EXISTS(\"kb.dep\")\n");
*fp << _T("!INCLUDE \"kb.dep\"\n");
*fp << _T("!ELSE \n");
*fp << _T("!MESSAGE Warning: cannot find \"kb.dep\"\n");
*fp << _T("!ENDIF \n");
*fp << _T("!ENDIF \n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("!IF \"$(CFG)\" == \"kb - Win32 Release\" || \"$(CFG)\" == \"kb - Win32 Debug\"\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\StdAfx.cpp\n");
*fp << _T("\n");
*fp << _T("!IF  \"$(CFG)\" == \"kb - Win32 Release\"\n");
*fp << _T("\n");
*fp << _T("CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I \"$(TAI)\\include\\Api\" /D \"WIN32\" /D \"NDEBUG\" /D \"_WINDOWS\" /D \"_MBCS\" /D \"_USRDLL\" /D \"KB_EXPORTS\" /Fp\"$(INTDIR)\\kb.pch\" /Yc\"StdAfx.h\" /Fo\"$(INTDIR)\\\\\" /Fd\"$(INTDIR)\\\\\" /FD /c \n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\StdAfx.obj\"	\"$(INTDIR)\\kb.pch\" : $(SOURCE) \"$(INTDIR)\"\n");
*fp << _T("	$(CPP) @<<\n");
*fp << _T("  $(CPP_SWITCHES) $(SOURCE)\n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("!ELSEIF  \"$(CFG)\" == \"kb - Win32 Debug\"\n");
*fp << _T("\n");
*fp << _T("CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I \"$(TAI)\\include\\Api\" /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"_MBCS\" /D \"_USRDLL\" /D \"KB_EXPORTS\" /Fp\"$(INTDIR)\\kb.pch\" /Yc\"StdAfx.h\" /Fo\"$(INTDIR)\\\\\" /Fd\"$(INTDIR)\\\\\" /FD /GZ /c \n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\StdAfx.obj\"	\"$(INTDIR)\\kb.pch\" : $(SOURCE) \"$(INTDIR)\"\n");
*fp << _T("	$(CPP) @<<\n");
*fp << _T("  $(CPP_SWITCHES) $(SOURCE)\n");
*fp << _T("<<\n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("!ENDIF \n");

*fp << _T("\n");
*fp << _T("SOURCE=.\\Cc_code.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\Cc_code.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\Con_ini.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\Con_ini.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\kb.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\kb.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\kb_setup.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\kb_setup.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\Ptr_ini.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\Ptr_ini.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\St_ini.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\St_ini.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");
*fp << _T("SOURCE=.\\Sym_ini.cpp\n");
*fp << _T("\n");
*fp << _T("\"$(INTDIR)\\Sym_ini.obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
*fp << _T("\n");

for (ii = 0; ii < con_segs; ++ii)
	{
	*fp << _T("SOURCE=.\\Con") << ii << _T(".cpp\n");
	*fp << _T("\n");
	*fp << _T("\"$(INTDIR)\\Con") << ii
	    << _T(".obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
	*fp << _T("\n");
	}
for (ii = 0; ii < ptr_segs; ++ii)
	{
	*fp << _T("SOURCE=.\\Ptr") << ii << _T(".cpp\n");
	*fp << _T("\n");
	*fp << _T("\"$(INTDIR)\\Ptr") << ii
	    << _T(".obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
	*fp << _T("\n");
	}
for (ii = 0; ii < st_segs; ++ii)
	{
	*fp << _T("SOURCE=.\\St") << ii << _T(".cpp\n");
	*fp << _T("\n");
	*fp << _T("\"$(INTDIR)\\St") << ii
	    << _T(".obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
	*fp << _T("\n");
	}
for (ii = 0; ii < sym_segs; ++ii)
	{
	*fp << _T("SOURCE=.\\Sym") << ii << _T(".cpp\n");
	*fp << _T("\n");
	*fp << _T("\"$(INTDIR)\\Sym") << ii
	    << _T(".obj\" : $(SOURCE) \"$(INTDIR)\" \"$(INTDIR)\\kb.pch\"\n");
	*fp << _T("\n");
	}

*fp << _T("\n");
*fp << _T("!ENDIF \n");
*fp << _T("\n");
*fp << endl;

delete fp;
return true;
*/
}

/********************************************
* FN:		GEN_KBBAT
* SUBJ:	Generate batch files to run KB makefile.
* CR:		07/29/01 AM.
********************************************/

bool gen_kbbat(_TCHAR *dir)
{
if (!dir || !*dir)
	return false;

_TCHAR s_nam[PATH];

// RELEASE VERSION
_stprintf(s_nam, _T("%s%skb_release.bat"), dir, DIR_SEP);
_t_ofstream *fp = new _t_ofstream(TCHAR2A(s_nam));

*fp << _T("rem VISUALTEXT Generated KB batch file.\n");
*fp << _T("pushd ") << dir << endl;
*fp << _T("del /q kb.dep") << endl;
*fp << _T("nmake /f kb.mak CFG=\"kb - Win32 Release\"") << endl;
*fp << _T("popd") << endl;
delete fp;

// DEBUG VERSION
_stprintf(s_nam, _T("%s%skb_debug.bat"), dir, DIR_SEP);
fp = new _t_ofstream(TCHAR2A(s_nam));

*fp << _T("rem VISUALTEXT Generated KB batch file.\n");
*fp << _T("pushd ") << dir << endl;
*fp << _T("del /q kb.dep") << endl;
*fp << _T("nmake /f kb.mak CFG=\"kb - Win32 Debug\"") << endl;
*fp << _T("popd") << endl;
delete fp;

return true;
}
