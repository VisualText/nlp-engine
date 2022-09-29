/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ASEQ.CPP
* FILE:	lite\var.cpp
* CR:		01/07/99 AM.
* SUBJ:	API functions for analyzer sequence.
* NOTE:	The API calls the external object a "seq", for analyzer sequence.
*			Internally, this will be mapped to the current Ana object!
* MAP:	void *pass == Delt<Seqn> *		An element of the sequence list.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"		// To get DIR_CH.		// 05/19/01 AM.
#include "lite/global.h"	// 07/18/00 AM.
#include "lite/mach.h"		// 05/26/01 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "dlist.h"			// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "parse.h"
#include "seqn.h"
#include "tree.h"		// Linux.	// 04/26/07 AM.
#include "io.h"		// Linux.	// 04/26/07 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "ana.h"
#include "htab.h"
#include "lite/nlp.h"
#include "Eana.h"				// 10/13/99 AM.
#include "func_defs.h"
#include "ifunc.h"			// 12/20/01 AM.
#include "ifile.h"			// 12/20/01 AM.
#include "lite/vtrun.h"		// 08/28/02 AM.
#include "lite/Aseq.h"

/********************************************
* FN:		Special functions for the class
********************************************/

Aseq::Aseq()
{
std::_t_strstream gerrStr;
gerrStr << _T("[DON'T CREATE OBJECTS OF Aseq CLASS.]") << std::ends;
errOut(&gerrStr,false);
}

/*******************************************/

/*******************************************/

Aseq::~Aseq()
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
* CR:		01/15/99 AM.
* SUBJ:	xxx
********************************************/

//LITE_API void *Aseq::xxx(void *xxx)
//{
//return 0;
//}

/********************************************
* FN:		MAKESEQ
* CR:		01/13/99 AM.
* SUBJ:	Create sequence for analyzer, allowing a header comment.
* ASS:	User must first delete the existing sequence for this analyzer.
********************************************/

// Create sequence for analyzer.  Allow a header comment.
// RET:	Sequence list.
LITE_API bool Aseq::makeSeq(NLP *nlp, _TCHAR *comment)
{
if (!nlp)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Given null NLP object.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Ana *ana;
ana = (Ana *) (nlp->getAna());
if (!ana)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[NLP object has no analyzer.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Dlist<Seqn> *seqlist = 0;
seqlist = ana->getSeqlist();
if (seqlist)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Current analyzer already has a sequence.]") << std::ends;
	errOut(&gerrStr,false);
	gerrStr << _T("[Delete it first.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Passed hurdles.  Create a sequence list for this analyzer.

// 02/12/99 AM. No need for this, since ana makes its own copy.
// Copy the comment string into string table.  // 01/17/99 AM.
//if (comment && *comment)
//	comment = ((Htab *)nlp->getHtab())->getStr(comment);
//else
//   comment = 0;

ana->makeSeq();
ana->setComment(comment);
comment = ana->getComment();		// Get copied comment.	// 02/12/99 AM.
zap_final_white(comment);			// Zap trailing whitespace.	// 02/12/99 AM.
return true;
}

	// Delete analyzer's sequence.
LITE_API bool Aseq::deleteSeq(NLP *nlp)
{
if (!nlp)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Given null NLP object.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Ana *ana;
ana = (Ana *) (nlp->getAna());
if (!ana)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[NLP object has no analyzer.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Dlist<Seqn> *seqlist = 0;
seqlist = ana->getSeqlist();
if (!seqlist)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Current analyzer sequence already empty.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// DELETE THE SEQUENCE.
Dlist<Seqn>::DeleteDlistAndData(seqlist);
ana->setSeqlist(0);
ana->setComment(0);
return true;
}


LITE_API void *Aseq::getSeq(NLP *nlp)
{
if (!nlp)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Given null NLP object.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
Ana *ana;
ana = (Ana *) (nlp->getAna());
if (!ana)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[NLP object has no analyzer.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

return ana->getSeqlist();
}

// Read sequence from file.
// NOTE:	Reads from the default analyzer.seq file.
LITE_API bool Aseq::readFile(NLP *nlp)
{
if (!nlp)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Given null NLP object.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
Ana *ana;
ana = (Ana *) (nlp->getAna());
if (!ana)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[NLP object has no analyzer.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

Dlist<Seqn> *seqlist = 0;
seqlist = ana->getSeqlist();
if (seqlist)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Current analyzer already has a sequence.]") << std::ends;
	errOut(&gerrStr,false);
	gerrStr << _T("[Delete it first.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// READ THE SEQUENCE FILE.
if (!(ana->readSeqfile()))			// 06/15/99 AM.
	return false;						// 06/15/99 AM.
ana->parseSeq(false);				// 11/05/99 AM.
Aseq::numberSeq(nlp);				// 10/07/99 AM.
return true;
}


// Write sequence to file.
// NOTE:	Write to the default analyzer.seq file.
LITE_API bool Aseq::writeFile(NLP *nlp)
{
((Ana *) nlp->getAna())->writeSeqfile();
return true;
}

// Create a new pass.
// RET:	Pass.
LITE_API APASS *Aseq::makePass(NLP *nlp, _TCHAR *algo, _TCHAR *data, _TCHAR *comment,
													bool active)
{
Seqn *pass = 0;

// STATUS.	//06/25/09 AM.
// The old way may work best for RUG, but the "new" way seems to work best for regular
// text analysis.  Something wrong with the sequencing (at least on Vista) by reverting
// to the old way.  So back to the "new way" for now. Heck with RUG.	// 06/25/09 AM.

// To account for RUG creation of many analyzers, reverting back to old way.	// 05/21/09 AM.
// Copy strings into hash/string table.  // 01/18/99 AM.
#ifdef OLD_030625_
if (!nlp)
	return 0;	// 05/21/09 AM.
Htab *htab = (Htab *)nlp->getHtab();	// 05/21/09 AM.
if (!htab)
	return 0;	// 05/21/09 AM.
algo	= htab->getStr(algo);
data	= htab->getStr(data);
// end oldstuff.
#endif

// #ifdef CAUSES_RUG_ISSUES_090521
// Use analyzer definition hash table.				// 06/25/03 AM.
Ana *ana = (Ana *)nlp->getAna();						// 06/25/03 AM.
Htab *htab = (Htab *)ana->getHtab();				// 06/25/03 AM.
algo = htab->getStr(algo);								// 06/25/03 AM.
data = htab->getStr(data);								// 06/25/03 AM.
// #endif

// 02/12/99 AM.  The Seqn object copies the comment, so no need to do it
// here.
//if (comment && *comment)
//	comment = ((Htab *)nlp->getHtab())->getStr(comment);
//else
//	comment = 0;

pass = new Seqn(active,algo,data,comment);
comment = pass->getComment();			// GET THE COPIED COMMENT.	// 02/12/99 AM.
zap_final_white(comment);				// Rm trailing white. // 02/12/99 AM.
Delt<Seqn> *delt;
//delt = new Delt<Seqn>(*pass);		// 07/02/99 AM.
delt = new Delt<Seqn>(pass);		// 07/02/99 AM.
// Clear out and delete the pass, because it's been copied.
//pass->setRules(0);					// 07/02/99 AM.
//pass->setAlgo(0);					// 07/02/99 AM.
//pass->setRulesfile(0);			// 07/02/99 AM.
//delete pass;	// Because making the delt copies the pass.	// 07/02/99 AM.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);				// 10/07/99 AM.

return delt;
}


// Insert pass after given pos.  pos == 0 means insert as 1st pass.
// ALLOC:	The given pass object is used as is.  "belongs" to the list.
LITE_API APASS *Aseq::insertPass(NLP *nlp, void *pass, int pos)
{
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't insert pass into null list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

seqlist->insertAfter((Delt<Seqn> *)pass, pos);

Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);				// 10/07/99 AM.

return 0;
}

// Insert pass after given pass (loc).
LITE_API APASS *Aseq::insertAfter(NLP *nlp, APASS *loc, APASS *pass)
{
if (!loc || !pass || !nlp)
	return 0;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't insert pass into null list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
seqlist->insertRight((Delt<Seqn> *)pass, (Delt<Seqn> *)loc);

Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);				// 10/07/99 AM.

return pass;
}


// 2/23/99 AM.
LITE_API APASS *Aseq::insertAfter(NLP *nlp, APASS *loc,
						_TCHAR *algo, _TCHAR *data, _TCHAR *comment,	bool active)
{
APASS *pass;
pass = Aseq::makePass(nlp,algo,data,comment,active);

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
// Aseq::numberSeq(nlp);			// LOOKS REDUNDANT.				// 12/20/01 AM.

return Aseq::insertAfter(nlp,loc,pass);
}

// Excise pass at given position. NOTE: Caller must delete pass.
LITE_API APASS *Aseq::excisePass(NLP *nlp, int pos)
{
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't excise from empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);				// 10/07/99 AM.

return seqlist->exciseNth(pos);
}

// Excise given pass.  NOTE: Caller must delete or reattach pass.
LITE_API APASS *Aseq::excisePass(NLP *nlp, APASS *pass)
{
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't excise from empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);				// 10/07/99 AM.

return seqlist->exciseElt((Delt<Seqn> *) pass);
}	

// Delete pass in the sequence.
// WARN:	DELETES OBJECTS.
LITE_API void Aseq::deletePass(NLP *nlp, int pos)
{
Delt<Seqn> *delt = 0;
if ((delt = (Delt<Seqn> *)Aseq::excisePass(nlp, pos)))
	Delt<Seqn>::DeleteDeltAndData(delt);
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Failed to delete pass.]") << std::ends;
	errOut(&gerrStr,false);
	}

Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);				// 10/07/99 AM.

}

// Delete pass standalone.
// NOTE: Pass must not be within a sequence.
// WARN:	Deletes objects.
LITE_API void Aseq::deletePass(APASS *pass)
{
if (pass)
	Delt<Seqn>::DeleteDeltAndData((Delt<Seqn> *)pass);
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Failed to delete pass.]") << std::ends;
	errOut(&gerrStr,false);
	}

}

	// Get nth pass from sequence.
LITE_API APASS *Aseq::nthPass(NLP *nlp, int pos)
{
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[nthPass: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
return seqlist->getNth(pos);
}


	// For traversing sequence.
LITE_API APASS *Aseq::firstPass(NLP *nlp)
{
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[firstPass: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
return seqlist->getFirst();
}

LITE_API APASS *Aseq::lastPass(NLP *nlp)
{
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[lastPass: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}
return seqlist->getLast();
}

LITE_API APASS *Aseq::nextPass(void *pass)
{
if (!pass)
	return 0;
return ((Delt<Seqn> *)pass)->Right();
}

LITE_API APASS *Aseq::prevPass(void *pass)
{
if (!pass)
	return 0;
return ((Delt<Seqn> *)pass)->Left();
}


	// Get pass info.
LITE_API bool Aseq::unpackPass(NLP *nlp, int pos, /*UP*/
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool &active)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
return Aseq::unpackPass(pass, /*UP*/ algo,data,comment,active);
}

LITE_API bool Aseq::unpackPass(APASS *pass, /*UP*/
							_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool &active)
{
*algo = *data = *comment = '\0';
active = true;
if (!pass)
	return false;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return false;
_tcscpy(algo, seqn->getAlgoname());
_tcscpy(data, seqn->getRulesfilename());
_tcscpy(comment, seqn->getComment());
active  = seqn->getActive();
return true;
}

LITE_API _TCHAR *Aseq::getAlgo(NLP *nlp, int pos)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
return Aseq::getAlgo(pass);
}

LITE_API _TCHAR *Aseq::getAlgo(void *pass)
{
if (!pass)
	return 0;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return 0;
return seqn->getAlgoname();
}

LITE_API _TCHAR *Aseq::getData(NLP *nlp, int pos)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
return Aseq::getData(pass);
}

LITE_API _TCHAR *Aseq::getData(void *pass)
{
if (!pass)
	return 0;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return 0;
return seqn->getRulesfilename();
}

LITE_API _TCHAR *Aseq::getComment(NLP *nlp, int pos)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
return Aseq::getComment(pass);
}

LITE_API _TCHAR *Aseq::getComment(void *pass)
{
if (!pass)
	return 0;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return 0;
return seqn->getComment();
}

LITE_API bool Aseq::getActive(NLP *nlp, int pos)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
return Aseq::getActive(pass);
}

LITE_API bool Aseq::getActive(void *pass)
{
if (!pass)
	return 0;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return 0;
return seqn->getActive();
}

LITE_API int Aseq::getPassnum(APASS *pass)		// 09/23/99 AM.
{
if (!pass)
	return 0;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return 0;
return seqn->getPassnum();
}


	// Set pass info.
LITE_API void Aseq::packPass(NLP *nlp, int pos,
						_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool active)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
packPass(nlp, pass, algo, data, comment, active);
}

LITE_API void Aseq::packPass(NLP *nlp, APASS *pass,
						_TCHAR *algo, _TCHAR *data, _TCHAR *comment, bool active)
{
Aseq::setAlgo(nlp,pass,algo);
Aseq::setData(nlp,pass,data);
Aseq::setComment(nlp,pass,comment);
Aseq::setActive(nlp,pass,active);
}

LITE_API void Aseq::setAlgo(NLP *nlp, int pos, _TCHAR *algo)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
Aseq::setAlgo(nlp, pass, algo);
}

LITE_API void Aseq::setAlgo(NLP *nlp, void *pass, _TCHAR *algo)
{
if (!pass)
	return;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return;
// Copy strings into hash/string table.  // 01/18/99 AM.
//algo	= ((Htab *)nlp->getHtab())->getStr(algo);
// Use analyzer definition hash table.				// 06/25/03 AM.
Ana *ana = (Ana *)nlp->getAna();						// 06/25/03 AM.
Htab *htab = (Htab *)ana->getHtab();				// 06/25/03 AM.
algo = htab->getStr(algo);								// 06/25/03 AM.
seqn->setAlgoname(algo);
seqn->unintern();				// Remove internal objects if any.
}

LITE_API void Aseq::setData(NLP *nlp, int pos, _TCHAR *data)
{
void *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
Aseq::setData(nlp, pass, data);
}

LITE_API void Aseq::setData(NLP *nlp, void *pass, _TCHAR *data)
{
if (!pass)
	return;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return;
// Copy strings into hash/string table.  // 01/18/99 AM.
//data	= ((Htab *)nlp->getHtab())->getStr(data);
// Use analyzer definition hash table.				// 06/25/03 AM.
Ana *ana = (Ana *)nlp->getAna();						// 06/25/03 AM.
Htab *htab = (Htab *)ana->getHtab();				// 06/25/03 AM.
data = htab->getStr(data);								// 06/25/03 AM.
seqn->setRulesfilename(data);
seqn->unintern();				// Remove internal objects if any.
}

LITE_API void Aseq::setComment(NLP *nlp, int pos, _TCHAR *comment)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
Aseq::setComment(nlp, pass, comment);
}

LITE_API void Aseq::setComment(NLP *nlp, void *pass, _TCHAR *comment)
{
if (!pass)
	return;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return;

// 02/12/99 AM. No need for this copy.
// Copy strings into hash/string table.  // 01/18/99 AM.
//comment	= ((Htab *)nlp->getHtab())->getStr(comment);
seqn->setComment(comment);
comment = seqn->getComment();		// Get copied comment.	// 02/12/99 AM.
zap_final_white(comment);			// Zap trailing whitespace.	// 02/12/99 AM.
}

LITE_API void Aseq::setActive(NLP *nlp, int pos, bool active)
{
Delt<Seqn> *pass;
pass = (Delt<Seqn> *) nthPass(nlp, pos);
Aseq::setActive(nlp, pass, active);
Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.
}

LITE_API void Aseq::setActive(NLP *nlp, void *pass, bool active)
{
if (!pass)
	return;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return;
seqn->setActive(active);
Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.
}

// Set the ordinal number for the given pass.	// 09/23/99 AM.
LITE_API void Aseq::setPassnum(APASS *pass, int num)
{
if (!pass)
	return;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return;
seqn->setPassnum(num);
}

// 02/24/99 AM. To notify when a pass has been modified, eg, when its
// rule file has been edited and saved.
LITE_API void Aseq::setDirty(NLP *nlp, void *pass)
{
if (!pass)
	return;
Seqn *seqn = 0;
if (!(seqn = ((Delt<Seqn> *) pass)->getData()))
	return;
seqn->unintern();				// Remove internal objects if any.
Ana *ana = (Ana *) nlp->getAna();										// 12/20/01 AM.
ana->setDirty(true);															// 12/20/01 AM.
}

	
// Internalize the analyzer sequence so that it can be executed.
// Note that the sequence can be internalized and executed without being
// saved to a file, thereby enabling testing before committing.
LITE_API bool Aseq::internSeq(NLP *nlp)
{
Ana *ana = (Ana *) nlp->getAna();	// Moved this up.				// 05/19/10 AM.

// SETTING UP ERROR LOG FILE FOR RE-INTERNING SEQUENCE.			// 05/19/01 AM.
std::_t_ostream *serr;																	// 05/19/01 AM.
std::_t_ofstream *ferr;																// 05/19/01 AM.
std::_t_ostream *sdbg=0;																// 02/21/02 AM.
_TCHAR outd[MAXSTR];															// 02/21/02 AM.
_TCHAR errout[1024];															// 05/19/01 AM.
_stprintf(outd,_T("%s%c%s"), ana->getAppdir(),DIR_CH,_T("logs"));			// 02/21/02 AM.
_stprintf(errout, _T("%s%cmake_ana.log"),outd,DIR_CH);					// 02/21/02 AM.
fileErr(errout, /*DU*/ ferr, serr);										// 05/19/01 AM.
nlp->fileDbg(outd,sdbg);													// 08/26/02 AM.

std::_t_strstream gerrStr;							// 05/26/01 AM.
gerrStr << _T("[Date: ") << today() << _T("]") << std::ends;						// 05/26/01 AM.
errOut(&gerrStr,false,0,0);															// 05/26/01 AM.

//*gerr << "Please wait...." << endl;	// Leave to the GUI.		// 05/19/01 AM.

// Give the RFA a runtime configuration.								// 10/13/99 AM.
Eana erfa(conf_INTERN);														// 12/20/01 AM.

// ana->getDirty();															// 12/20/01 AM.
// Assuming that if this internSeq fn is called, then ana is
// dirty, or that we're forcing a re-intern.							// 12/20/01 AM.

// Reset the hash table for user functions.							// 12/20/01 AM.
Ifunc::deleteHtab(ana->getHtfunc());									// 12/20/01 AM.
void *htfunc = Ifunc::createHtab(HTFUNC_SIZE);						// 12/20/01 AM.
ana->setHtfunc(htfunc);														// 12/20/01 AM.

// Traverse the sequence list.
// If not internalized, then intern each pass in turn.
Delt<Seqn> *selt;
Seqn *seqn;
Ana *rfa;
VTRun *vtrun;	// [DEGLOB]	// 10/15/20 AM.
// IF RFB IS AROUND, USE IT.									// FIX	// 11/16/99 AM.
//if (!(rfa = (Ana *) nlp->getRFB()))						// FIX	// 11/16/99 AM.
//	rfa = (Ana *) nlp->getRFA();
//if (!(rfa = ((Ana *)VTRun_Ptr->rfb_)))									// 08/28/02 AM.
//	rfa = (Ana *)VTRun_Ptr->rfa_;											// 08/28/02 AM.
vtrun = (VTRun*)nlp->getVTRun();
if (!(rfa = ((Ana *)(vtrun->rfb_))))	// [DEGLOB]	// 10/15/20 AM.
	rfa = (Ana *)vtrun->rfa_;			// [DEGLOB]	// 10/15/20 AM.
long currseqpass = 0;	// Count input file passes.				// 08/23/02 AM.
bool okparse = true;		// If interning went ok.					// 05/26/01 AM.
Ifile *rf;																		// 12/20/01 AM.
for (selt = (Delt<Seqn> *)Aseq::firstPass(nlp); selt; selt = selt->Right())
	{
	seqn = selt->getData();
	seqn->setPassnum(++currseqpass);	// In case...					// 05/20/01 AM.
	if (seqn->getActive())			// Pass that will be executed.
		{
		if (!(seqn->getAlgo())												// 12/20/01 AM.
		 && !(seqn->getRules())
		 && !(seqn->getRulesfile()) )
			{
			if (!ana->internPass(seqn, *rfa, &erfa))					// 10/13/99 AM.
				{
				okparse = false;
//				resetErr(ferr, serr);										// 05/26/01 AM.
//				return false;		// Asynchrony.							// 12/26/99 AM.
				}
			}
		else	// Unmodified passes may need some processing.		// 12/20/01 AM.
			{
			}
		// Reload the hash table for functions here, etc.			// 12/20/01 AM.
		if ((rf = seqn->getRulesfile()))								// 12/20/01 AM.
			if (!Ifunc::loadHtab(htfunc,									// 12/20/01 AM.
							rf->getDecls(),									// 12/20/01 AM.
							vtrun->htfunc_,	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
//							nlp->getHtfunc(),		// Builtin hash tab.	// 12/20/01 AM.
							currseqpass ))										// 08/23/02 AM.
				okparse = false;												// 12/21/01 AM.
		}
	}
ana->setDirty(false);	// Analyzer now re-interned.				// 12/20/01 AM.
resetErr(ferr, serr);														// 05/19/01 AM.
nlp->resetDbg(sdbg);															// 08/26/02 AM.
return okparse;	// Status of interning analyzer.					// 05/26/01 AM.
}

/********************************************
* FN:		NUMBERSEQ
* CR:		10/07/99 AM.
* SUBJ:	Number the passes of the analyzer sequence.
* OPT:	Should do this with internal functions, not this API.
********************************************/

LITE_API void Aseq::numberSeq(NLP *nlp)
{
APASS *seq;						// Analyzer sequence.
int num = 0;					// Current pass number.

num = 0;
for (seq = Aseq::firstPass(nlp); seq; seq = Aseq::nextPass(seq))
	Aseq::setPassnum(seq, ++num);
}


/********************************************
* FN:		SMARTUP, SMARTDOWN
* CR:		10/13/99 AM.
* SUBJ:	Move a pass up or down in the sequence.
********************************************/

// 10/13/99 AM.
// Move a pass up or down.  "Smart" because it's aware of stub regions.
// Renumbers parse tree also.
// Returns true if successful (eg, not moving first pass up!).
// WARN: In general such motion requires RUG GENERATE ALL.
// WARN:	Not checking if current pass is in a stub region.
LITE_API bool Aseq::smartUp(NLP *nlp, APASS *pass)
{
if (!pass || !nlp)
	return false;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't get move pass up in sequence.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Delt<Seqn> *curr = (Delt<Seqn> *)pass;
Delt<Seqn> *prev, *prev2;

// Get the previous pass.
if (!(prev = curr->Left()))
	return false;

if (Aseq::endStub(prev))				// Previous is a stub end.
	{
	// Find the stub start or fail.
	prev = (Delt<Seqn> *) rfindStartstub(nlp, (APASS *) (prev->Left()));
	if (!prev)
		return false;
	}

// 10/27/99 AM. If current pass is a stub, handle specially.
if (Aseq::startStub(curr))
	{
	// Find end stub.
	Delt<Seqn> *end;
	if (!(end = (Delt<Seqn> *) findEndstub(nlp, (APASS *) (curr->Right()))))
		return false;
	seqlist->excise(curr,end);		// Detach the entire stub region.
	prev2 = prev->Left();	// If there's one 2 passes earlier, get it.
	seqlist->spliceAfter(curr,end,prev2);	// Splice after it (or at start).
	}
else
	{
	seqlist->exciseElt(curr);				// Remove fm current position.
	if ((prev2 = prev->Left()))				// If there's one 2 passes earlier.
		seqlist->insertRight(curr, prev2);		// ... insert after it.
	else
		seqlist->insertAfter(curr, 0);	// Insert as 1st (before prev).
									// FIX. 10/27/99 AM. ch prev to curr.
	}

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);

return true;
}


LITE_API bool Aseq::smartUp(NLP *nlp, int pos)
{
if (!nlp || !pos)
	return false;

APASS *pass;
pass = nthPass(nlp, pos);
return smartUp(nlp, pass);
}


LITE_API bool Aseq::smartDown(NLP *nlp, APASS *pass)
{
if (!pass || !nlp)
	return false;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't get move pass down in sequence.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Delt<Seqn> *curr = (Delt<Seqn> *)pass;
Delt<Seqn> *next;

// 10/27/99 AM. If current pass is a stub, handle specially.
if (Aseq::startStub(curr))
	{
	// Find end stub.
	Delt<Seqn> *end;
	if (!(end = (Delt<Seqn> *) findEndstub(nlp, (APASS *) (curr->Right()))))
		return false;

	if (!(next = end->Right()))	// Get next pass.
		return false;

	if (Aseq::startStub(next))				// Next is a stub start.
		{
		// Find the stub end or fail.
		next = (Delt<Seqn> *) findEndstub(nlp, (APASS *) (next->Right()));
		if (!next)
			return false;
		}

	seqlist->excise(curr,end);		// Detach the entire stub region.
	seqlist->spliceAfter(curr,end,next);	// Splice after next.
	}
else
	{
	// Get the next pass.
	if (!(next = curr->Right()))
		return false;

	if (Aseq::startStub(next))				// Next is a stub start.
		{
		// Find the stub end or fail.
		next = (Delt<Seqn> *) findEndstub(nlp, (APASS *) (next->Right()));
		if (!next)
			return false;
		}

	seqlist->exciseElt(curr);				// Remove fm current position.
	seqlist->insertRight(curr, next);	// Insert after next.
	}

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);

return true;
}


LITE_API bool Aseq::smartDown(NLP *nlp, int pos)
{
if (!nlp || !pos)
	return false;

APASS *pass;
pass = nthPass(nlp, pos);
return smartDown(nlp, pass);

}


LITE_API bool Aseq::startStub(APASS *pass)
{
if (!pass)
	return false;
if (!_tcscmp(_T("stub"), Aseq::getAlgo(pass)))
	return true;
return false;
}

LITE_API bool Aseq::endStub(APASS *pass)
{
if (!pass)
	return false;
if (!_tcscmp(_T("end"), Aseq::getAlgo(pass)))
	return true;
return false;
}

// Find start stub going backward.
// If find end stub first, that's an error.
LITE_API APASS *Aseq::rfindStartstub(NLP *nlp, APASS *pass)
{
if (!pass || !nlp)
	return 0;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[No analyzer sequence.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

while (pass)
	{
	_TCHAR *algo = Aseq::getAlgo(pass);
	if (!_tcscmp(_T("stub"), algo))	// startStub()
		return pass;
	if (!_tcscmp(_T("end"), algo))		// endStub()
		return 0;
	pass = ((Delt<Seqn> *)pass)->Left();
	}
return 0;
}


// Find start stub going backward.
// If find end stub first, that's an error.
LITE_API APASS *Aseq::findEndstub(NLP *nlp, APASS *pass)
{
if (!pass || !nlp)
	return 0;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[No analyzer sequence.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

while (pass)
	{
	_TCHAR *algo = Aseq::getAlgo(pass);
	if (!_tcscmp(_T("end"), algo))		// endStub()
		return pass;
	if (!_tcscmp(_T("stub"), algo))	// startStub()
		return 0;
	pass = ((Delt<Seqn> *)pass)->Right();
	}
return 0;
}







/********************************************
* FN:		MOVEUP,MOVEDOWN
* CR:		12/13/99 AM.
* SUBJ:	Move a pass up or down in the sequence.
********************************************/

// Renumbers parse tree also.
// Returns true if successful (eg, not moving first pass up!).
// WARN: In general such motion requires RUG GENERATE ALL.
// WARN:	Not checking if current pass is in a stub region.
LITE_API bool Aseq::moveUp(NLP *nlp, APASS *pass)
{
if (!pass || !nlp)
	return false;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't get move pass up in sequence.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Delt<Seqn> *curr = (Delt<Seqn> *)pass;
Delt<Seqn> *prev, *prev2;

// Get the previous pass.
if (!(prev = curr->Left()))
	return false;

seqlist->exciseElt(curr);				// Remove fm current position.
if ((prev2 = prev->Left()))				// If there's one 2 passes earlier.
	seqlist->insertRight(curr, prev2);		// ... insert after it.
else
	seqlist->insertAfter(curr, 0);	// Insert as 1st (before prev).

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);

return true;
}


LITE_API bool Aseq::moveUp(NLP *nlp, int pos)
{
if (!nlp || !pos)
	return false;

APASS *pass;
pass = nthPass(nlp, pos);
return moveUp(nlp, pass);
}


LITE_API bool Aseq::moveDown(NLP *nlp, APASS *pass)
{
if (!pass || !nlp)
	return false;
Dlist<Seqn> *seqlist = 0;
if (!(seqlist = (Dlist<Seqn> *)getSeq(nlp)))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't get move pass down in sequence.]") << std::ends;
	errOut(&gerrStr,false);
	return 0;
	}

Delt<Seqn> *curr = (Delt<Seqn> *)pass;
Delt<Seqn> *next;

// Get the next pass.
if (!(next = curr->Right()))
	return false;

seqlist->exciseElt(curr);				// Remove fm current position.
seqlist->insertRight(curr, next);	// Insert after next.

// Renumber the analyzer sequence.
// (Opt: Could renumber from the current pass onward.)
Aseq::numberSeq(nlp);

return true;
}


LITE_API bool Aseq::moveDown(NLP *nlp, int pos)
{
if (!nlp || !pos)
	return false;

APASS *pass;
pass = nthPass(nlp, pos);
return moveDown(nlp, pass);

}
