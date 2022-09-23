/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	NINTERN.CPP
* FILE:	lite/nintern.cpp
* CR:		05/01/00 AM.
* SUBJ:	RFA tree-traversal and internalizing pass.
* ASS:	RFA has completed the parse tree for current rules file.
* NOTE:	May also use this pass to attach actions to rules.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"
#include "inline.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"		// 02/26/01 AM.
#include "pn.h"				// 02/14/01 AM.
#include "io.h"	// Linux.	// 04/26/07 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "parse.h"
#include "seqn.h"
#include "ifile.h"						// 02/27/01 AM.
#include "rfasem.h"
#include "nintern.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("nintern");

/********************************************
* FN:		Special Functions for class.
* CR:		05/01/00 AM.
********************************************/

Nintern::Nintern()			// Default constructor
	: Algo(algo_name /*, 0*/)
{

}


Nintern::Nintern(const Nintern &orig)			// Copy constructor
{
name = orig.name;
debug_ = orig.debug_;
}

/********************************************
* FN:		Access Functions
********************************************/



/********************************************
* FN:		Modify Functions
********************************************/




/********************************************
* FN:		SETUP
* CR:		05/01/00 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Nintern::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		05/01/00 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Nintern::dup()
{
Nintern *ptr;
ptr = new Nintern(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}

/********************************************
* FN:		EXECUTE
* CR:		05/01/00 AM.
* SUBJ:	Perform the traversal and interning.
* NOTE:	Now, just picking the rules right off the tree!
*			Use this pass to internalize the contents of the pairs lists.
********************************************/

bool Nintern::Execute(Parse *parse, Seqn *seqn)
{
if (parse->Verbose())
	*gout << _T("[Nintern:]") << std::endl;

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();

if (!tree)
	{
	parse->setIntern(false);	// Syntax error.						// 05/26/01 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Nintern: No parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

Node<Pn> *root;
root = tree->getRoot();

if (!root)
	{
	parse->setIntern(false);	// Syntax error.						// 05/26/01 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Nintern: Empty parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// Get the rules file node, if any.
Node<Pn> *node;
if (!(node = root->Down()))
	{
	parse->setIntern(false);	// Syntax error.						// 05/26/01 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Nintern: Rules file '")
	  // << str(seqn->getRulesfilename())
		  << str(parse->getInput())
		  << _T("' not parsed.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

if (node->Right())
	{
	if (parse->getIntern())													// 05/26/01 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Pass file has some unhandled text. Ignoring.]")
				  << std::ends;
		errOut(&gerrStr,false,parse->getInputpass(),0);
		parse->setIntern(false);	// Syntax error.					// 05/26/01 AM.
		}
											// 07/19/00 AM.
// This is hanging.															// 07/19/00 AM.
//	if (gui_.IsStatus(GUI_STATUS_INITIALIZING))		// TRYING		// 03/22/01 AM.
//		gui_.SendMessage("Loading rules:",displayBuf);				// 03/22/01 AM.

//	*gerr << "[Pass file has some unhandled text.]" << std::endl;
	}


Pn *pn;
pn = node->getData();
assert(pn);
RFASem *sem;
sem = (RFASem *) pn->getSem();
if (!sem)
	{
	parse->setIntern(false);	// Syntax error.						// 05/26/01 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Nintern: No semantics for parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// Could search for a _RULESFILE node at the top level here.
if (sem->getType() != RSFILE)
	{
	parse->setIntern(false);	// Syntax error.						// 05/26/01 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Nintern: Couldn't parse rules file.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

Ifile *rulesfile;
if (!(rulesfile = sem->getRulesfile()))
	{
	parse->setIntern(false);	// Syntax error.						// 05/26/01 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Nintern: No rules found in file.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// Internalize the Ifile rules file object.
// Build a sequence list for the recurse mini-passes.
// Build a single rules list for the file.
// Attach pre and post actions to each rule.
// Build recurse passes and bind them to the rule elements that call them.
Dlist<Irule> *rules;
rules = rulesfile->intern(parse,false);		// Let rules file object internalize itself.
		// FALSE MEANS DON'T HASH THE ELTS AND RULES.				// 05/01/00 AM.

if (!rules)
	{
//	strstream gerrStr(Errbuf,MAXSTR,ios::out);
//	gerrStr << "[Nintern: No rules.]" << std::ends;
//	errOut(false,parse->getInputpass(),0);

	// LOOKS LIKE THIS INTRODUCED A BUG.								// 03/07/02 AM.
//	return false;				// COMMENTED OUT.	// FIX.				// 03/07/02 AM.
	}

return true;
}


/******************************************************************************/
