/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	INTERN.CPP
* FILE:	lite/intern.cpp
* CR:		11/05/98 AM.
* SUBJ:	RFA tree-traversal and internalizing pass.
* ASS:	RFA has completed the parse tree for current rules file.
* NOTE:	May also use this pass to attach actions to rules.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"		// 02/26/01 AM.
#include "lite/global.h"		// 05/19/99 AM.
#include "dlist.h"					// 07/07/03 AM.
//#include "lite/global.h"		// 05/19/99 AM.
#include "inline.h"		// 05/19/99 AM.
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
#include "intern.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("intern");

/********************************************
* FN:		Special Functions for class.
* CR:		11/05/98 AM.
********************************************/

Intern::Intern()			// Default constructor
	: Algo(algo_name /*, 0*/)
{

}


Intern::Intern(const Intern &orig)			// Copy constructor	// 12/03/98 AM.
{
name = orig.name;
//parse = orig.parse;		// 12/04/98 AM.
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
* CR:		12/04/98 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Intern::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		12/03/98 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Intern::dup()
{
Intern *ptr;
ptr = new Intern(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}

/********************************************
* FN:		EXECUTE
* CR:		11/05/98 AM.
* SUBJ:	Perform the traversal and interning.
* NOTE:	11/11/98 AM. Now, just picking the rules right off the tree!
*			11/22/98 AM. Use this pass to internalize the contents of the
*			pairs lists.
********************************************/

bool Intern::Execute(Parse *parse, Seqn *seqn)
{
if (parse->Verbose())
	*gout << _T("[Intern:]") << std::endl;

Tree<Pn> *tree = (Tree<Pn> *) parse->getTree();

if (!tree)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: No parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

Node<Pn> *root;
root = tree->getRoot();

if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: Empty parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// 11/24/98 AM.
// Get the rules file node, if any.
Node<Pn> *node;
if (!(node = root->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: Rules file '")
	  // << str(seqn->getRulesfilename())
		  << str(parse->getInput())					// 03/05/99 AM.
		  << _T("' not parsed.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

if (node->Right())
	{
	if (parse->getIntern())													// 05/26/01 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Pass file has some unhandled text.]") << std::ends;
		errOut(&gerrStr,false,parse->getInputpass(),0);
		}
	}



Pn *pn;
pn = node->getData();
assert(pn);
RFASem *sem;
sem = (RFASem *) pn->getSem();
if (!sem)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: No semantics for parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// Could search for a _RULESFILE node at the top level here.
if (sem->getType() != RSFILE)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: Couldn't parse rules file.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

Ifile *rulesfile;
if (!(rulesfile = sem->getRulesfile()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: No rules found in file.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// 11/23/98 AM.
// Internalize the Ifile rules file object.
// Build a sequence list for the recurse mini-passes.
// Build a single rules list for the file.
// Attach pre and post actions to each rule.
// Build recurse passes and bind them to the rule elements that call them.
Dlist<Irule> *rules;
rules = rulesfile->intern(parse); // Let rules file object internalize itself.

//if (!(rules = sem->getRules()))			// 12/17/98 AM. Bug!
if (!rules)											// 12/17/98 AM. Fix.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Intern: No rules.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// 12/17/98 AM.
// HASH ALL THE RULES OF THE FILE.
rulesfile->rhash(
	parse);											// 04/03/03 AM.

// CLEAN THE MATCH/FAIL LISTS OF RULES.
//if (parse->getInputpass() != 3)		// DEBUGGING!! // 02/27/00 AM.
Irule::pruneLists(rules);

// Traverse the rules list, internalizing the pairs information.
// Delete the pairs that are internalized.
// OBSOLETED BY THE ABOVE INTERN CALL.
//internRules(rules);

return true;
}


/******************************************************************************/
