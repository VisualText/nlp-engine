/*******************************************************************************
Copyright (c) 2003 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:   MISC.CPP
* FILE:   User\misc.cpp
* CR:     12/03/03 AM.
* SUBJ:   Miscellaneous support functions.
*
*******************************************************************************/

#ifdef LINUX
#include <locale.h>
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>
#else
#include <windows.h>
#endif

#include "machine.h"
#include "prim/libprim.h"
#include "prim/list_s.h"
#include "prim/list.h"
#include "lite/lite.h"
#include "lite/global.h"
#include "lite/delt.h"
#include "lite/dlist.h"
#include "lite/dir.h"
#include "lite/iarg.h"
#include "lite/node.h"
#include "lite/tree.h"
#include "lite/pn.h"
#include "lite/Avar.h"
//#include "lite/nlppp.h"
#include "lite/Auser.h"
#include "lite/parse.h"
#include "lite/ana.h"
//#include "lite/pn.h"
#include "lite/rfasem.h"
#include "lite/nlp.h"
#include "lite/vtrun.h"
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "lite/arg.h"
#include "user.h"
#include "lite/std.h"
LITE_API bool alphabetic(_TCHAR);
#include "lite/inline.h"

#include "Umisc.h"


/********************************************
* FN:		POSPARSE
* CR:		12/03/03 AM.
* RET:	True if successful parse.
* SUBJ:	Read POS tagger output.
* NOTE:	Assume max of two tokens per line of output.
*			Eg, L.  NNP
********************************************/

bool posParse(
	_TCHAR *fname,
	Nlppp *nlppp
	)
{
if (!fname || !*fname)
	return false;

long flen = file_size(fname);		// Get file size from system.
if (flen <= 0)
	return false;

// Get parse tree also...
///////////////////////
///  PARSE TREE TESTING
///////////////////////

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
if (!tree)
   {
   cout << "[posParse: Couldn't fetch parse tree.]" << endl;
   return false;
   }

Node<Pn> *root = (Node<Pn> *)Auser::getRoot(tree);
if (!root)
   {
   cout << "[posParse: Couldn't fetch root of parse tree.]" << endl;
   return false;
   }

// Example: Get name of parse tree root.
//Pn *pn = root->getData();
Pn *pn = Auser::getNodedata(root);
_TCHAR *str = pn->getName();

// Get list of tokens under the root.
NODE *node = Auser::getNodedown(root);

/////////////////////////////////////////////////



_TCHAR tok1[MAXSTR+1];
_TCHAR tok2[MAXSTR+1];
_TCHAR pos[MAXSTR+1];
tok1[0] = tok2[0] = pos[0] = '\0';	// Initialize.

#ifdef LINUX
//ifstream inFile(fname, ios::in | ios::nocreate);
ifstream inFile(fname, ios::in);
#else
_t_ifstream inFile(TCHAR2CA(fname), ios::in | ios_base::binary);
#endif
if (!inFile)
	{
//	*gerr << "[Could not open file '" << fname << "']." << endl;
	return false;
	}

_TCHAR ch;
_TCHAR *ptr=tok1;
int linelen = 0;
bool flag = false;
ch = inFile.get();	// Lookahead _TCHAR.
for (;;)
	{
	if (inFile.eof())
		return true;

	if (ch == '\r' || ch == '\n')
		{
		*++ptr = '\0';	// Terminate.
		ch = inFile.get();
		while (ch == '\r' || ch == '\n')
			ch = inFile.get();

		// Pretty print.
/*
		if (*tok1)
			cout << tok1;
		if (*tok2)
			cout << tok2;
		if (flag)
			cout << " ";
		if (*pos)
			cout << pos;
		cout << endl;
*/

		// Find token(s) in the parse tree.
		// Perform reduction for current POS.
		// If replacement identical to token, do nothing.
		if (!node)
			return false;
		posUpdate(tok1, tok2, pos, nlppp, /*DU*/ node);

		// Reset parser.
		tok1[0] = tok2[0] = pos[0] = '\0';	// Initialize.
		flag = false;
		ptr = tok1;
		}
	else if (alphabetic(ch))
		{
		if (flag)
		  ptr = pos;
		else if (*tok1)	// First token already filled.
			{
			*++ptr = '\0';	// Terminate.
			ptr = tok2;
			}
		*ptr = ch;
		ch = inFile.get();
		while (alphabetic(ch))
			{
			*++ptr = ch;
			ch = inFile.get();
			}
		}
	else if (isdigit(ch))
		{
		if (flag)
		  ptr = pos;
		else if (*tok1)	// First token already filled.
			{
			*++ptr = '\0';	// Terminate.
			ptr = tok2;
			}
		*ptr = ch;
		ch = inFile.get();
		while (isdigit(ch))
			{
			*++ptr = ch;
			ch = inFile.get();
			}
		}
	else if (ispunct(ch))
		{
		if (flag)
		  ptr = pos;
		else if (*tok1)	// First token already filled.
			{
			*++ptr = '\0';	// Terminate.
			ptr = tok2;
			}
		*ptr = ch;
		ch = inFile.get();
		}
	else if (ch == '\t' || ch == ' ')	// Whitespace.
		{
		// Separates token from POS assignment.
		*++ptr = '\0';	// Terminate current token.
		flag = true;	// Flag whitespace seen on line.
		ch = inFile.get();
		}
	else
		{
		// Control _TCHAR?
		return false;
		}
	}
return true;
}

/********************************************
* FN:		POSUPDATE
* CR:		12/04/03 AM.
* RET:	True if successful parse.
* SUBJ:	Update parse tree for current POS tagger line.
* NOTE:	Assume max of two tokens per line of output.
*			Eg, L.  NNP
********************************************/

bool posUpdate(
	_TCHAR *tok1,
	_TCHAR *tok2,
	_TCHAR *pos,
	Nlppp *nlppp,
	/*DU*/
	NODE *&node
	)
{
if (!node || !*tok1 || !*pos)
	return false;

if (!alphabetic(*pos))	// Not an alphabetic POS
	{
	node = Auser::getNoderight(node);
	return true;			// Ok and no op, eg, "(  (".
	}

// Assume parse tree node is in proper position in the parse tree.
// Let's handle the single token case first.
Pn *pn = Auser::getNodedata(node);
_TCHAR *str = pn->getName();

// Look for match of token in parse tree.
while (_tcscmp(tok1,str))
	{
	node = Auser::getNoderight(node);
	if (!node)
		return false;
	pn = Auser::getNodedata(node);
	str = pn->getName();
	}

if (_tcscmp(tok1,str))	// Just confirm.
	{
	node = Auser::getNoderight(node);
	return false;			// Error,
	}

// Matched first token at this point.

//	cout << "[MATCHED]";
//	cout << "[" << tok1 << "," << pos << "]" << endl;
NODE *start = node;
NODE *end = 0;
if (empty(tok2))	// No 2nd token from tagger on this line.
	end = start;
else
	{
	// FIND SECOND TOKEN.
	// Skip whitespace if any.
	node = Auser::getNoderight(node);
	pn = Auser::getNodedata(node);
	str = pn->getName();
	while (isspace(*str))
		{
		node = Auser::getNoderight(node);
		if (!node)
			return false;
		pn = Auser::getNodedata(node);
		str = pn->getName();
		}

	// Now must match 2nd token.
	if (_tcscmp(tok2,str))
		return false;	// Error.
	end = node;
	}

node = Auser::getNoderight(node);	// Return next position.

// Do the parse tree reduction, and done.
_TCHAR buf[MAXSTR+1];
_stprintf(buf,_T("_%s"),pos);	// TESTING UNICODE. // 01/10/06 AM.
Auser::redsingle(	// Manual reduce in parse tree.
	start,	// First node of range.
	end,		// Last node of range.
	buf,		// Suggested node's name.
	false,	// Set BASE attr.
	false,	// Set UNSEALED attr.
	nlppp
	);

return true;
}
