/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ARUG.CPP
* FILE:	lite/Arug.cpp
* CR:		05/20/99 AM.
* SUBJ:	API functions for rule generation.
*
*******************************************************************************/

#include "StdAfx.h"

#include "machine.h"
#include "lite/global.h"	// 07/18/00 AM.
#include "lite/mach.h"		// 08/31/99 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "dlist.h"			// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/24/06 AM.
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "parse.h"
#include "seqn.h"
#include "ana.h"
//#include "htab.h"
#include "lite/nlp.h"
#include "irule.h"
#include "ifile.h"
//#include "lite/nlppp.h"			// 07/24/06 AM.
#include "pat.h"		// 05/26/99 AM.
#include "lite/Aseq.h"
#include "Arug.h"

/********************************************
* FN:		Special functions for the class
********************************************/

Arug::Arug()
{
std::_t_strstream gerrStr;
gerrStr << _T("[DON'T CREATE OBJECTS OF Arug CLASS.]") << std::ends;
errOut(&gerrStr,false);
}

/*******************************************/

/*******************************************/

Arug::~Arug()
{
}


/*******************************************/

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

/********************************************
* FN:		Modify Functions
********************************************/

/********************************************
* FN:		General Functions
********************************************/

/********************************************
* FN:		Class-wide Functions
********************************************/

/********************************************
* FN:		XXX
* CR:		05/20/99 AM.
* SUBJ:	xxx
********************************************/

//void *Arug::xxx(void *xxx)
//{
//return;
//}

// Make empty rules list.
ARULES *Arug::makeRules()
{
return (ARULES *) new Dlist<Irule>();
}

// Make empty selects list.
ASELECTS *Arug::makeSelects()
{
return (ASELECTS *) new Dlist<Iarg>();
}

// Delete selects list.	// 05/21/09 AM.
bool Arug::deleteSelects(	// 05/21/09 AM.
	ASELECTS *selects
	)
{
if (!selects)
	return true;
Dlist<Iarg>::DeleteDlistAndData((Dlist<Iarg>*)selects);
return true;
}

// Make a pass.
APASS *Arug::makeSeq(
	bool active,
	_TCHAR *algo_name,
	_TCHAR *rules_file_name,
	_TCHAR *comment
	)
{
Seqn *seq;
seq = new Seqn(active, algo_name,
				rules_file_name, comment);

// Assume it's a pat algo.
Algo *alg;
alg = new Pat();
seq->setAlgo(alg);
return (APASS *) seq;
}

// Delete rules list.
void Arug::rmRules(ARULES* &rules)
{
if (rules)
	{
	delete (Dlist<Irule> *) rules;
	rules = 0;
	}
}

// Pretty print the rules list.
void Arug::prettyRules(ARULES *rules)
{
Dlist<Irule> *tmp;
tmp = (Dlist<Irule> *) rules;
std::_t_cout << *tmp;
}


/********************************************
* FN:		GENSAMPLE
* CR:		05/20/99 AM.
* SUBJ:	Generate a rule for a single sample.
********************************************/

bool Arug::genSample(
	long start,				// Start offset of sample in input text.
	long end,				// End offset of sample in input text.
	APARSE *parse,			// Current text analysis instance.
	int num,					// Current pass number.
	_TCHAR *conname,			// Concept name.
	/*DU*/
	APASS *seq,				// Down: current pass. UP: new pass.
								// New pass contains rule for sample.
	ARULES *rules,			// Rules for current concept.
	ASELECTS *selects		// List of select node names (context).
	)
{
return Parse::genSample(start, end, (Parse *)parse, num, conname,
									/*DU*/
									(Delt<Seqn> *) seq,
									(Dlist<Irule> *) rules,
									(Dlist<Iarg> *) selects);
}

/********************************************
* FN:		GENXSAMPLE
* CR:		06/08/99 AM.
* SUBJ:	Generate a rule for a single sample, with LABELS.
********************************************/

bool Arug::genxSample(
	_TCHAR *fname,			// Sample's text file name.
	long start,				// Start offset of sample in input text.
	long end,				// End offset of sample in input text.
	_TCHAR *text,				// Sample text.			// 06/16/99 AM.
	CONCEPT *conc,			// Current concept in Gram hierarchy.
	APARSE *parse,			// Current text analysis instance.
	_TCHAR *conname,			// Concept name.
	Mode *mode,				// 08/09/99 AM.
	CG *cg,					// Current KB.
	/*DU*/
	ARULES *rules,			// Rules for current concept.
	ASELECTS *selects		// List of select node names (context).
	)
{
return Parse::genxSample(fname, start, end, text,
									 conc,		// Using CG in LITE!
									 (Parse *)parse, conname,
									 mode,			// 08/09/99 AM.
									 cg,
									/*DU*/
									(Dlist<Irule> *) rules,
									(Dlist<Iarg> *) selects);
}


/********************************************
* FN:		FILLPASS
* CR:		05/26/99 AM.
* SUBJ:	Fill pass with rules, select nodes.
* NOTE:	Simulate the way RFA builds a pass.
********************************************/

bool Arug::fillPass(APASS *pass, ARULES *rules, ASELECTS *selects)
{
Delt<Seqn> *dseq;
Dlist<Irule> *ruls;
Dlist<Iarg> *sels;

if (!pass || !rules || !selects)
	return false;

dseq = (Delt<Seqn> *) pass;
ruls = (Dlist<Irule> *) rules;
sels = (Dlist<Iarg> *) selects;


Seqn *seq;
seq = dseq->getData();

// Not sure why rules stored in pass and file.  Only one owns them.
// As I recall, it's because they started out in one and moved to another
// object.
seq->setRules(ruls);

// Assume it's a pat algo.
Algo *alg;
alg = new Pat();
seq->setAlgo(alg);

// Create a rulesfile.
Ifile *rf;
rf = new Ifile();			// Empty rulesfile object.
rf->setRules(ruls);
rf->setOwning(true);		// Rules own their actions.				// 12/24/00 AM.
rf->setSelects(sels);

// Attach rulesfile to pass.
seq->setRulesfile(rf);
return true;
}


/********************************************
* FN:		GENRULESFILE
* CR:		05/29/99 AM.
* SUBJ:	Generate pass file for rug pass.
* NOTE:	Assumes only rules and selects are filled, for now.
*			Will need to handle the generic case, eventually.
*			(Recurse, regions, actions, etc.)
********************************************/

bool Arug::genRulesfile(APASS *pass, _TCHAR *fname)
{
Delt<Seqn> *dseq;
Seqn *seq;
if (!(dseq = (Delt<Seqn> *) pass))
	return false;
if (!(seq = dseq->getData()))
	return false;

// Get pass name.
//*gout << "name=" << fname << std::endl;

Dlist<Irule> *rules;
if (!(rules = seq->getRules()))
	return false;

// Open file for writing.
std::_t_ofstream ofile(TCHAR2CA(fname), std::ios::out);

// File header.
ofile << _T("\n# AUTOMATICALLY GENERATED! EDITS WILL BE LOST!") << std::endl;
ofile << _T("# FILE: ") << fname << std::endl;
ofile << _T("# TIME: ") << today1() << std::endl;	// 08/31/99 AM.
	// WORKAROUND.	// 02/25/00 AM.

// Traverse selects.
ofile << _T("@NODES") << std::flush;

Dlist<Iarg> *sels;
Delt<Iarg> *dsel;
Iarg *sel;
sels = seq->getSelects();
for (dsel = sels->getFirst(); dsel; dsel = dsel->Right())
	{
	sel = dsel->getData();
	ofile << _T(" ") << sel->getStr();
	}
ofile << std::endl;

// Traverse rules list.
// (Each rule owns its actions, etc. in this variant.)
//Irule::genRules(rules, " ", ofile);							// 11/04/99 AM.
Irule::genRuleblocks(rules, _T(" "), ofile);						// 11/04/99 AM.

// File footer.
ofile << std::endl;


return true;
}


/********************************************
* FN:		DEDUP
* CR:		06/07/99 AM.
* SUBJ:	Remove duplicates from a list of rules.
********************************************/

ARULES *Arug::dedup(ARULES *rules)
{
return (ARULES *) Irule::dedup((Dlist<Irule> *) rules);
}


/********************************************
* FN:		GENERALIZE
* CR:		06/28/99 AM.
* SUBJ:	Generalize and merge the list of rules.
********************************************/

ARULES *Arug::generalize(ARULES *rules, Mode *mode)
{
return (ARULES *) Parse::generalize((Dlist<Irule> *) rules, mode);
}
