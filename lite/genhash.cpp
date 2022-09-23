/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	GENHASH.CPP
* FILE:	lite/genhash.cpp
* CR:		06/13/00 Genhash AM.
* SUBJ:	Pass to generate code for hash tables.
* ASS:	A pass like NINTERN or intern has been run before this.
*
*******************************************************************************/

#include "StdAfx.h"
//#include <iostream> // 07/18/03 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
//#include "dlist.h"					// 07/07/03 AM.
#include "lite/global.h"
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"	// 07/07/03 AM.
#include "parse.h"
#include "seqn.h"
#include "ifile.h"			// 02/27/01 AM.
#include "rfasem.h"
#include "pn.h"				// 02/14/01 AM.
#include "ana.h"				// 04/28/01 AM.
#include "genhash.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("genhash");

/********************************************
* FN:		Special Functions for class.
* CR:		06/13/00 AM.
********************************************/

Genhash::Genhash()			// Default constructor
	: Algo(algo_name /*, 0*/)
{

}


Genhash::Genhash(const Genhash &orig)			// Copy constructor
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
* CR:		06/13/00 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Genhash::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		06/13/00 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Genhash::dup()
{
Genhash *ptr;
ptr = new Genhash(*this);					// Copy constructor.
//ptr = new Pat();
//ptr->setName(this->name);
//ptr->setParse(this->parse);
return (Algo &) *ptr;
}

/********************************************
* FN:		EXECUTE
* CR:		06/13/00 AM.
* SUBJ:	Perform the traversal and interning.
* NOTE:	Now, just picking the rules right off the tree!
*			Use this pass to internalize the contents of the pairs lists.
********************************************/

bool Genhash::Execute(Parse *parse, Seqn *seqn)
{
if (!parse)																		// 04/28/01 AM.
	return false;																// 01/26/02 AM.

Gen *gen = parse->getAna()->getGen();									// 04/28/01 AM.
if (!gen)																		// 04/28/01 AM.
	return false;			// Couldn't gen code and compile.		// 01/26/02 AM.
std::_t_ofstream *fcode = gen->getFcode();										// 04/28/01 AM.
int id = gen->getId();														// 04/28/01 AM.

if (parse->Verbose())
	*gout << _T("[Genhash:]") << std::endl;

Tree<Pn> *tree;

tree = (Tree<Pn> *)parse->getTree();

if (!tree)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Genhash: No parse tree.]") << std::ends;
	errOut(&gerrStr,false,parse->getInputpass(),0);

	*fcode << std::endl
				<< _T("// ERROR in pass ") << id << _T(".") << std::endl;			// 04/28/01 AM.
	return false;
	}

Node<Pn> *root;
root = tree->getRoot();

if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Genhash: Empty parse tree.]") << std::ends;
	errOut(&gerrStr,false,parse->getInputpass(),0);

	*fcode << std::endl
				<< _T("// ERROR in pass ") << id << _T(".") << std::endl;			// 04/28/01 AM.
	return false;
	}

// Get the pass file node, if any.
Node<Pn> *node;
if (!(node = root->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Genhash: Pass file not parsed.]") << std::ends;
	errOut(&gerrStr,false,parse->getInputpass(),0);

	*fcode << std::endl
				<< _T("// ERROR in pass ") << id << _T(".") << std::endl;			// 04/28/01 AM.
	return false;
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
	gerrStr << _T("[Genhash: No semantics for parse tree.]") << std::ends;
	errOut(&gerrStr,false,parse->getInputpass(),0);

	*fcode << std::endl
				<< _T("// ERROR in pass ") << id << _T(".") << std::endl;			// 04/28/01 AM.
	return false;
	}

// Could search for a _RULESFILE node at the top level here.
if (sem->getType() != RSFILE)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Genhash: Couldn't parse pass file.]") << std::ends;
	errOut(&gerrStr,false,parse->getInputpass(),0);

	*fcode << std::endl
				<< _T("// ERROR in pass ") << id << _T(".") << std::endl;			// 04/28/01 AM.
	return false;
	}

Ifile *rulesfile;
if (!(rulesfile = sem->getRulesfile()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Genhash: No rules found in file.]") << std::ends;
	errOut(&gerrStr,false,parse->getInputpass(),0);

	*fcode << std::endl
				<< _T("// ERROR in pass ") << id << _T(".") << std::endl;			// 04/28/01 AM.
	return false;
	}

Dlist<Irule> *rules = rulesfile->getRules();

// GENERATE PASS EVEN IF ZERO RULES.									// 01/09/01 AM.
//if (!rules)
//	{
//	*gerr << "[Genhash: No rules.]" << std::endl;
//	return;
//	}


// GENERATE CODE FOR ELEMENT HASH TABLES.
// GENERATE CODE FOR RULE HASH TABLES.
// Traverse all the rules in recurse and main regions of pass file.
rulesfile->genHash(parse);


return true;
}


/******************************************************************************/
