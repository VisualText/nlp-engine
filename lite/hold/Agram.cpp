/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	AGRAM.CPP
* FILE:	lite/Agram.cpp
* CR:		05/20/99 AM.
* SUBJ:	API functions for rule generation.
*
*******************************************************************************/

#include "StdAfx.h"

#include "machine.h"	// 10/25/06 AM.
#include "consh/libconsh.h"		// 02/14/01 AM.
#include "consh/cg.h"				// 02/14/01 AM.
#include "lite/global.h"	// 07/18/00 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "lite/iarg.h"				// 05/14/03 AM.
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/24/06 AM.
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "parse.h"
#include "seqn.h"
#include "ana.h"
//#include "htab.h"	// Cleanups, removing dependencies.	// 06/22/03 AM.
#include "lite/nlp.h"
#include "irule.h"
#include "ifile.h"
//#include "lite/nlppp.h"			// 07/24/06 AM.
#include "pat.h"		// 05/26/99 AM.
#include "lite/Aseq.h"
#include "lite/Agram.h"


/********************************************
* FN:		Special functions for the class
********************************************/

Agram::Agram()
{
_t_strstream gerrStr;
gerrStr << _T("[DON'T CREATE OBJECTS OF Agram CLASS.]") << ends;
errOut(&gerrStr,false,0,0);
}

/*******************************************/
/*******************************************/

Agram::~Agram()
{
}


/*******************************************/
/*******************************************/


/********************************************
* FN:		Class-wide Functions
********************************************/


/********************************************
* FN:		MAKEDIRTY
* CR:		08/27/99 AM.
* SUBJ:	Flag a concept as dirty for rule generation.
********************************************/

bool Agram::makeDirty(CONCEPT *conc, CG *cg)
{
if (!conc)
	return false;

// Set the DIRTY flag for concept.
return cg->replaceVal(conc, _T("dirty"), _T("true"));
}


/********************************************
* FN:		SMARTDIRTY
* CR:		09/01/99 AM.
* SUBJ:	Flag a concept and possibly its parent as dirty.
* NOTE:	If current concept is a label, just set its parent to dirty.
********************************************/

bool Agram::smartDirty(CONCEPT *conc, CG *cg)
{
if (!conc)
	return false;

// If the at a LABEL concept, dirty the parent concept instead!
CONCEPT *par;
if ((par = cg->Up(conc))
	 && cg->findPhrase(par))			// Parent is a rule concept.
	makeDirty(par, cg);			// Only dirty the parent.
else
	makeDirty(conc, cg);			// Set the DIRTY flag for concept.

return true;
}


/********************************************
* FN:		ADDSAMPLE
* CR:		08/26/99 AM.
* SUBJ:	Add a sample to a Gram hierarchy concept.
* MAP:	cg->addCnode(con, name);  => Agram::addSample(con, name, cg);
********************************************/

CONCEPT *Agram::addSample(CONCEPT *con, _TCHAR *name, CG *cg)
{
CONCEPT *sample;
if (!(sample = cg->addCnode(con, name)))
	return 0;

smartDirty(con, cg); // Set DIRTY flag for concept -- or parent, if label.
return sample;
}


/********************************************
* FN:		RMSAMPLE
* CR:		08/27/99 AM.
* SUBJ:	Add a sample to a Gram hierarchy concept.
* MAP:	cg->rmNode(con);  => Agram::rmSample(con, cg);
********************************************/

bool Agram::rmSample(CONCEPT *node, CG *cg)
{
CONCEPT *conc;

// Get sample's concept.
if (!(conc = cg->nodeOwner(node)))
	return false;

// Remove the sample node.
if (!cg->rmNode(node))
	return false;

smartDirty(conc, cg); // Set DIRTY flag for concept -- or parent, if label.
return true;
}


/********************************************
* FN:		SMARTPASSNUM
* CR:		09/25/99 AM.
* SUBJ:	Get concept's pass number, if any.
* NOTE:	If current concept is a label, get its parent's pass number.
********************************************/

int Agram::smartPassnum(CONCEPT *conc, CG *cg)
{
if (!conc || !cg)
	return false;

// If the at a LABEL concept, get passnum from parent.
long val = 0;
bool ok = false;			// If got the passnum.
CONCEPT *par;
if (!(cg->Down(conc))
	 && (par = cg->Up(conc))
	 && cg->findPhrase(par))			// Parent is a rule concept.
	ok = cg->findVal(par, _T("passnum"), /*UP*/ val);
else
	ok = cg->findVal(conc, _T("passnum"), /*UP*/ val);

if (ok)
	return val;
return 0;			// Signifies passnum is absent.
}


/********************************************
* FN:		RENAMECONCEPT
* CR:		08/27/96 AM.
* SUBJ:	Rename Gram hierarchy concept.
* MAP:	cg->renameConcept(con);  => Agram::renameConcept(con, cg);
********************************************/

bool Agram::renameConcept(CONCEPT *conc, _TCHAR *name, CG *cg)
{
if (!cg->renameConcept(conc, name))
	return false;

// If concept has no samples, don't set dirty flag.
if (!cg->findPhrase(conc))
	return true;

smartDirty(conc, cg); // Set DIRTY flag for concept -- or parent, if label.
return true;
}


/********************************************
* FN:		RMATTR
* CR:		08/27/96 AM.
* SUBJ:	Remove attribute.
* MAP:	cg->rmAttr(conc,name);  => Agram::rmAttr(conc,name, cg);
********************************************/

bool Agram::rmAttr(CONCEPT *conc, _TCHAR *name, CG *cg)
{
if (!conc || !name || !*name || !cg)
	return false;

cg->rmAttr(conc, name);														// 05/28/01 AM.

return makeDirty(conc, cg);	// Set the DIRTY flag for concept.
}


/********************************************
* FN:		ADDVAL
* CR:		08/27/96 AM.
* SUBJ:	Add value to attribute.
* MAP:	cg->addVal(conc,attr,val);  => Agram::addVal(conc,attr,val,cg);
********************************************/

bool Agram::addVal(CONCEPT *conc, _TCHAR *name, _TCHAR *val, CG *cg)
{
if (!conc || !name || !*name || !val || !*val || !cg)
	return false;

if (!cg->addVal(conc, name, val))
	return false;

return makeDirty(conc, cg);	// Set the DIRTY flag for concept.
}


/********************************************
* FN:		ADDSVAL
* CR:		08/27/96 AM.
* SUBJ:	Add value to attribute.
* MAP:	cg->addSval(conc,attr,val);  => Agram::addSval(conc,attr,val,cg);
********************************************/

bool Agram::addSval(CONCEPT *conc, _TCHAR *name, _TCHAR *val, CG *cg)
{
return addVal(conc,name,val,cg);
}


/********************************************
* FN:		REPLACEVAL
* CR:		08/27/96 AM.
* SUBJ:	Replace value of attribute.
* MAP:	cg->replaceVal(conc,attr,val);  =>
*			Agram::replaceVal(conc,attr,val,cg);
********************************************/

bool Agram::replaceVal(CONCEPT *conc, _TCHAR *name, _TCHAR *val, CG *cg)
{
if (!cg->replaceVal(conc,name,val))
	return false;
return makeDirty(conc, cg);
}
