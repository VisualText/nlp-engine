/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ANGEN.CPP
* FILE:	lite/angen.cpp
* CR:		04/17/00 AM.
* SUBJ:	Code generation pass for a rule file analyzer.
* ASS:	Works off parse tree built by analyzer prior to interning.
* NOTE:	This may just be a sandbox for building and running the compilation
*			of rules and passes.
*
*******************************************************************************/

#include "StdAfx.h"
#ifdef LINUX
//#include <sql.h>
//#include <sqlext.h>
//#include <sqlucode.h>
//#include <iodbcext.h>
#endif
#include "machine.h"						// 04/27/00 AM.
#include "lite/global.h"
#include "u_out.h"		// 01/19/06 AM.
#include "inline.h"
#include "dlist.h"						// 02/26/01 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"							// 02/26/01 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "pn.h"
#ifdef LINUX						// 02/14/01 AM.
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#endif
#include "ana.h"							// 05/10/00 AM.
#include "parse.h"
#include "seqn.h"
#include "ifile.h"						// 02/27/01 AM.
#include "rfasem.h"
#include "gen.h"							// 05/08/00 AM.
#include "angen.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("angen");

/********************************************
* FN:		Special Functions for class.
********************************************/

Angen::Angen()			// Default constructor
	: Algo(algo_name /*, 0*/)
{

}


Angen::Angen(const Angen &orig)			// Copy constructor
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
* CR:		04/17/00 AM.
* SUBJ:	Set up Algo as an analyzer pass.
* ARG:	s_data = This is an argument from the analyzer sequence file,
*		   if any, for the current pass.
********************************************/

void Angen::setup(_TCHAR *s_data)
{
// No arguments to this pass in sequence file.
}

/********************************************
* FN:		DUP
* CR:		04/17/00 AM.
* SUBJ:	Duplicate the given Algo object.
* NOTE:	Don't know a better way to have a base pointer duplicate the
*			object that it points to!
********************************************/

Algo &Angen::dup()
{
Angen *ptr;
ptr = new Angen(*this);					// Copy constructor.
return (Algo &) *ptr;
}

/********************************************
* FN:		EXECUTE
* CR:		04/17/00 AM.
* SUBJ:	Perform the traversal and code generation.
********************************************/

bool Angen::Execute(Parse *parse, Seqn *seqn)
{
if (parse->Verbose())
	*gout << _T("[Angen:]") << std::endl;

Tree<Pn> *tree;

tree = (Tree<Pn> *)parse->getTree();

if (!tree)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Angen: No parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

Node<Pn> *root;
root = tree->getRoot();

if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Angen: Empty parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}


// Get the pass file node, if any.
Node<Pn> *node;
if (!(node = root->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Angen: Pass file '")
	  // << str(seqn->getRulesfilename())
		  << str(parse->getInput())
		  << _T("' not parsed.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

if (node->Right())
	{
	if (parse->getIntern())												// 05/26/01 AM.
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
	gerrStr << _T("[Angen: No semantics for parse tree.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

// Could search for a _RULESFILE node at the top level here.
if (sem->getType() != RSFILE)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Angen: Couldn't parse pass file.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

Ifile *rulesfile;
if (!(rulesfile = sem->getRulesfile()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Angen: No rules found in file.]") << std::ends;
	return errOut(&gerrStr,false,parse->getInputpass(),0);
	}

#ifdef OLD_
// Set up an output file for code generation.
_TCHAR codefile[MAXSTR];														// 04/27/00 AM.
// Note: This is not the correct pass number.  Need to get the num of the
// pass file being analyzed.
int id = seqn->getPassnum();												// 04/27/00 AM.
_stprintf(codefile, _T("%s%cpass%d.cpp"),										// 04/27/00 AM.
			parse->getOutdir(), DIR_CH, id);
_t_ofstream fcode(codefile, std::ios::out);										// 04/27/00 AM.

// Create object to manage code generation.							// 05/08/00 AM.
Gen gen(_T(""), &fcode, id, REGNULL);										// 05/08/00 AM.
#endif

// Code generate from the Ifile pass file object.
// Build a sequence list for the recurse mini-passes.
// Build a single rules list for the file.
// Attach pre and post actions to each rule.
// Build recurse passes and bind them to the rule elements that call them.
 // Let pass file object generate for itself.
//rulesfile->gen(parse->getAna()->getGen(),
//						parse->getInput());									// 05/20/00 AM.
rulesfile->gen(parse);														// 05/31/00 AM.
return true;
}


/******************************************************************************/
