/*******************************************************************************
Copyright � 2003-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	LEXTAGGER.CPP
* FILE:	lite/lextagger.h
* CR:		12/11/03 AM.
*
* SUBJ:	Class to manage UMIST LexTagger, a POS tagger.
*
* NOTE:	Support for multiple and embedded analyzers.
*
*******************************************************************************/

#include "StdAfx.h"

// Comment out this file for now.
#ifndef LINUX

#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.

#include "prim/libprim.h"	// 09/15/08 AM.
#ifndef DWORD
#define DWORD double
#endif
LIBPRIM_API DWORD run_silent(_TCHAR* strCMD);	// 09/15/08 AM.

#include "lite/global.h"
#include "std.h"
#include "inline.h"
#include "dlist.h"
#include "lite/iarg.h"
#include "str.h"
#include "node.h"
#include "tree.h"
#include "lite/pn.h"
#include "lite/parse.h"
#include "lite/nlppp.h"
#include "pat.h"

#include "LexTagger.h"


/********************************************
* FN:    POSTAGGER
* CR:    12/11/03 AM.
* SUBJ:  Function to invoke a 3rd-party POS tagger called LexTagger.
* RET:   True if ok.
* FORM:  str = user::unloadanalyzer(handle);
* EX:    user::postagger();
* NOTE:  Assumes parse tree has been tokenized already.
********************************************/

bool LexTagger::postagger(
	Nlppp *nlppp
   )
{
if (!nlppp)
   return false;

// Access the current parse object.
Parse *parse = nlppp->getParse();

// Assume tagger is in VisualText\apps folder.
_TCHAR *path=0;
if (!(path = _tgetenv(_T("VISUALTEXT"))))
   return 0;

// Tagger program.  (LexTagger sample is Java code.)
_TCHAR *tagger = _T("LexTagger-1.0");
_TCHAR taggerpath[MAXSTR];
_stprintf(taggerpath,_T("%s\\apps\\%s"), path, tagger);	// $visualtext\apps\taggerfolder.

_TCHAR cmd[MAXSTR];

// Specify input and output.
// May need to copy input file, create a config file, etc.,
// depending on the 3rd party tagger.

// Get input file path.
_TCHAR inf[MAXSTR];
_stprintf(inf,_T("%s\\test\\input.txt"), taggerpath);

// Get name of output file.
_TCHAR outf[MAXSTR];
_stprintf(outf,_T("%s\\test\\result.txt"), taggerpath);

// Copy current input file to tagger work area.
_stprintf(cmd,_T("copy %s %s"), parse->getInput(), inf);
//_tsystem(cmd);	// 09/28/08 AM.
run_silent(cmd);			// system() w/o DOC CMD PROMPT POPUP.	// 09/28/08 AM.

// Invoke POS Tagger.
_stprintf(cmd,_T("%s\\run.bat"), taggerpath);
//_tsystem(cmd);	// 09/28/08 AM.
run_silent(cmd);			// system() w/o DOC CMD PROMPT POPUP.	// 09/28/08 AM.

// Poll here, waiting for output file to be ready.

// Parse the tagged output file and update
// the parse tree.
posParse(outf,nlppp);

return true;
}



/********************************************
* FN:		POSPARSE
* CR:		12/03/03 AM.
* RET:	True if successful parse.
* SUBJ:	Read POS tagger output.
* NOTE:	Assume max of two tokens per line of output.
*			Eg, L.  NNP
********************************************/

bool LexTagger::posParse(
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
Parse *parse = nlppp->getParse();

Tree<Pn> *tree = (Tree<Pn> *) parse->getTree();
if (!tree)
   {
   std::_t_cout << _T("[posParse: Couldn't fetch parse tree.]") << std::endl;
   return false;
   }

Node<Pn> *root = tree->getRoot();
if (!root)
   {
   std::_t_cout << _T("[posParse: Couldn't fetch root of parse tree.]") << std::endl;
   return false;
   }

// Example: Get name of parse tree root.
//Pn *pn = root->getData();
Pn *pn = root->getData();
_TCHAR *str = pn->getName();

// Get list of tokens under the root.
Node<Pn> *node = root->Down();

/////////////////////////////////////////////////



_TCHAR tok1[MAXSTR+1];
_TCHAR tok2[MAXSTR+1];
_TCHAR pos[MAXSTR+1];
tok1[0] = tok2[0] = pos[0] = '\0';	// Initialize.

#ifdef LINUX
std::_t_ifstream inFile(fname, std::ios::in | std::ios::nocreate);
#else
std::_t_ifstream inFile(TCHAR2CA(fname), std::ios::in | std::ios_base::binary);
#endif
if (!inFile)
	{
//	*gerr << "[Could not open file '" << fname << "']." << std::endl;
	return false;
	}

_TCHAR ch;
_TCHAR *ptr=tok1;
int linelen = 0;
bool flag = false;
ch = inFile.get();	// Lookahead char.
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
		cout << std::endl;
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
	else if (_istdigit(ch))
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
		while (_istdigit(ch))
			{
			*++ptr = ch;
			ch = inFile.get();
			}
		}
	else if (_istpunct(ch))
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
		// Control char?
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

bool LexTagger::posUpdate(
	_TCHAR *tok1,
	_TCHAR *tok2,
	_TCHAR *pos,
	Nlppp *nlppp,
	/*DU*/
	Node<Pn> *&node
	)
{
if (!node || !*tok1 || !*pos)
	return false;

if (!alphabetic(*pos))	// Not an alphabetic POS
	{
	node = node->Right();
	return true;			// Ok and no op, eg, "(  (".
	}

// Assume parse tree node is in proper position in the parse tree.
// Let's handle the single token case first.
Pn *pn = node->getData();
_TCHAR *str = pn->getName();

// Look for match of token in parse tree.
while (_tcscmp(tok1,str))
	{
	node = node->Right();
	if (!node)
		return false;
	pn = node->getData();
	str = pn->getName();
	}

if (_tcscmp(tok1,str))	// Just confirm.
	{
	node = node->Right();
	return false;			// Error,
	}

// Matched first token at this point.

//	cout << "[MATCHED]";
//	cout << "[" << tok1 << "," << pos << "]" << std::endl;
Node<Pn> *start = node;
Node<Pn> *end = 0;
if (empty(tok2))	// No 2nd token from tagger on this line.
	end = start;
else
	{
	// FIND SECOND TOKEN.
	// Skip whitespace if any.
	node = node->Right();
	pn = node->getData();
	str = pn->getName();
	while (_istspace(*str))
		{
		node = node->Right();
		if (!node)
			return false;
		pn = node->getData();
		str = pn->getName();
		}

	// Now must match 2nd token.
	if (_tcscmp(tok2,str))
		return false;	// Error.
	end = node;
	}

node = node->Right();	// Return next position.

// Do the parse tree reduction, and done.
_TCHAR buf[MAXSTR+1];
_stprintf(buf,_T("_%s"),pos);
Pat::redsingle(	// Manual reduce in parse tree.
	start,	// First node of range.
	end,		// Last node of range.
	buf,		// Suggested node's name.
	false,	// Set BASE attr.
	false,	// Set UNSEALED attr.
	nlppp
	);
return true;
}

#endif  // LINUX

/*************************      END OF FILE      ******************************/
