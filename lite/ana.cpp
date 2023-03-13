/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ANA.CPP
* FILE:	lite\ana.cpp
* CR:		10/13/98 AM.
* SUBJ:	Analyzer definition class.
*
*******************************************************************************/

#include "StdAfx.h"
#ifdef LINUX
//#include <sql.h>
//#include <sqlext.h>
//#include <sqlucode.h>
//#include <iodbcext.h>
#endif

#include "machine.h"		// 03/08/00 AM.
#include "lite/global.h"
#include "u_out.h"		// 01/19/06 AM.
#include "inline.h"		// 05/19/99 AM.
#include "std.h"
#include "chars.h"
#include "io.h"
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"			// 05/10/00 AM.
#include "parse.h"

// NOTE:	Add new pass types here.
#include "tok.h"
#include "cmltok.h"		// 08/18/08 AM.
#include "dicttok.h"		// 07/29/11 AM.
#include "line.h"
#include "thtab.h"		// 02/26/01 AM.
#include "pat.h"
#include "rec.h"			// 11/08/99 AM.
#include "stub_s.h"		// 06/23/99 AM.
#include "stub_e.h"		// 06/23/99 AM.
#include "angen.h"		// 04/27/00 AM.
#include "intern.h"		// 05/02/00 AM.
#include "nintern.h"		// 05/02/00 AM.
#include "phash.h"		// 05/02/00 AM.
#include "genhash.h"		// 06/14/00 AM.
#include "ifunc.h"		// 12/20/01 AM.

#include "consh/libconsh.h"	// 11/11/00 AM.
#include "consh/cg.h"			// 11/11/00 AM.

#include "kb.h"			// 02/12/07 AM.

#include "rfa.h"
#include "seqn.h"
#include "Eana.h"			// 12/20/01 AM.
#include "nlp.h"			// 12/20/01 AM.
#include "ana.h"

int Ana::count_ = 0;

/********************************************
* FN:		Special functions for class
* CR:		10/13/98 AM.
********************************************/

Ana::Ana(_TCHAR *appdir)
{
seqlist_		= 0;
seqbuf_		= 0;
seqbuflen_	= 0;
htab_			= 0;
htfunc_		= 0;																// 12/20/01 AM.
nlp_			= 0;																// 12/21/01 AM.
verbose_		= false;

if (appdir && !appdir)														// 05/10/00 AM.
	{
	_tcscpy(appdir_, appdir);												// 05/10/00 AM.
	}
else
	{
	appdir_[0] = '\0';														// 05/10/00 AM.
	}

specdir_[0] = '\0';															// 12/03/98 AM.
comment_[0] = '\0';															// 01/13/99 AM.
seqfile_[0] = '\0';															// 01/20/99 AM.
datadir_[0] = '\0';															// 12/08/99 AM.
cg_ = 0;																			// 02/15/00 AM.
gen_ = 0;																		// 05/10/00 AM.
npasses_ = 0;																	// 06/13/00 AM.
dirty_	= false;																// 12/20/01 AM.

#ifndef STABLE_
++count_;
#endif
}

Ana::~Ana()
{
if (gen_)																		// 05/10/00 AM.
	delete gen_;																// 05/10/00 AM.

if (seqlist_)
	Dlist<Seqn>::DeleteDlistAndData(seqlist_);
seqlist_ = 0;

if (seqbuf_)			// 11/19/98 AM.
	{
	//delete [] seqbuf_;
	Chars::destroy(seqbuf_);
	seqbuf_ = 0;
	}

if (htfunc_)																	// 12/20/01 AM.
	Ifunc::deleteHtab(htfunc_);											// 12/20/01 AM.

#ifndef STABLE_
--count_;
#endif
}

/********************************************
* FN:		Access functions
* CR:		10/13/98 AM.
********************************************/

Dlist<Seqn> *Ana::getSeqlist()		{ return seqlist_; }
_TCHAR		  *Ana::getSeqfile()		{ return seqfile_; }
_TCHAR		  *Ana::getSeqbuf()		{ return seqbuf_;  }
long			Ana::getSeqbuflen()	{ return seqbuflen_; }
Htab		  *Ana::getHtab()			{ return htab_;	}
bool			Ana::Verbose()			{ return verbose_; }
_TCHAR		  *Ana::getSpecdir()		{return specdir_;}
_TCHAR		  *Ana::getComment()		{return comment_;}				// 01/13/99 AM.
_TCHAR		  *Ana::getDatadir()		{return datadir_;}				// 12/08/99 AM.
_TCHAR		  *Ana::getAppdir()		{return appdir_;}					// 05/10/00 AM.
CG			  *Ana::getCG()			{return cg_;}						// 02/15/00 AM.
Gen		  *Ana::getGen()			{return gen_;}						// 05/10/00 AM.
long			Ana::getNpasses()		{return npasses_;}				// 06/13/00 AM.
bool			Ana::getDirty()		{return dirty_;}					// 12/20/01 AM.
void		  *Ana::getHtfunc()		{return htfunc_;}					// 12/20/01 AM.
NLP		  *Ana::getNLP()			{return nlp_;}						// 12/21/01 AM.

Delt<Seqn> *Ana::getSeq()
{
if (seqlist_)
	return seqlist_->getFirst();
return 0;
}

/********************************************
* FN:		Modify functions
* CR:		10/13/98 AM.
********************************************/

void Ana::setSeqlist(Dlist<Seqn> *x) { seqlist_		= x;}
void Ana::setSeqbuf(_TCHAR *x)			{ seqbuf_		= x;}
void Ana::setSeqbuflen(long x)		{ seqbuflen_	= x;}
void Ana::setHtab(Htab *x)				{ htab_			= x;}
void Ana::setVerbose(bool x)			{ verbose_		= x;}
void Ana::setSpecdir(_TCHAR *x)			{_tcscpy(specdir_, x);}		// 12/03/98 AM.
void Ana::setCG(CG *x)					{cg_				= x;}			// 02/15/00 AM.
void Ana::setGen(Gen *x)				{gen_				= x;}			// 05/10/00 AM.
void Ana::setNpasses(long x)			{npasses_		= x;}			// 06/13/00 AM.
void Ana::setDirty(bool x)				{dirty_			= x;}			// 12/20/01 AM.
void Ana::setHtfunc(void *x)			{htfunc_			= x;}			// 12/20/01 AM.
void Ana::setNLP(NLP *x)				{nlp_				= x;}			// 12/21/01 AM.

void Ana::setAppdir(_TCHAR *x)												// 05/10/00 AM.
{
if (x && *x)
	_tcscpy(appdir_, x);
else
	appdir_[0] = '\0';
}

void Ana::setComment(_TCHAR *x)												// 01/13/99 AM.
{
if (x && *x)
	_tcscpy(comment_, x);
else
	comment_[0] = '\0';														// 05/10/00 AM.
}

void Ana::setSeqfile(_TCHAR *x)
{
if (x && *x)
	_tcscpy(seqfile_, x);
else
	seqfile_[0] = '\0';														// 05/10/00 AM.
}
void Ana::setDatadir(_TCHAR *x)
{
if (x && *x)
	_tcscpy(datadir_, x);
else
	datadir_[0] = '\0';														// 05/10/00 AM.
}


/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ana::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ana::prettyCount(std::_t_ofstream *ostr)
{
if (count_)
	{
	if (ostr)
		*ostr << _T("Active Ana count=") << count_ << std::endl;
	*gout << _T("Active Ana count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ana count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

/********************************************
* FN:		READSEQFILE
* CR:		10/13/98 AM.
* SUBJ:	Read the analyzer sequence file into a buffer.
* ALLOC:	Creates buffer that must be freed.
********************************************/

bool Ana::readSeqfile()
{
// Get file length.
long len;
_TCHAR *buf;

read_file(seqfile_, /*UP*/ len, buf);
if (len <= 0)
	{
	// Should throw an exception!
	std::_t_strstream gerrStr;
	gerrStr << _T("[No analyzer sequence file.]") << std::ends;
	errOut(&gerrStr,false);
	gerrStr << _T("[file=") << seqfile_ << _T("]") << std::ends;
	errOut(&gerrStr,false);

	//exit(1);			// 06/15/99 AM.
	return false;		// 06/15/99 AM.
	}

setSeqbuflen(len);
setSeqbuf(buf);

if (Verbose())
	{
	*gout << _T("Length of input file is ") << len << std::endl;
	*gout << _T("\nInput text is:\n") << buf << std::endl;
	}
return true;
}


/********************************************
* FN:		PARSESEQ
* CR:		10/13/98 AM.
* SUBJ:	Parse the sequence buffer and convert to node list.
* FORM:	Analyzer sequence file has lines with the format:
*			[num] algo  [data]...
*			...
*			The only algorithms so far are: tokenize & lines.
*			Recognize a pat algo, etc.  Any other is user defined
*			and will be given subsequent arg as a string.
*			File may contain # for comments.  Rest of line ignored
*			after #. [num] is a pass number for readability.
*			(A pretty-printer could fix numbers up for user.)
*			01/07/99 AM. Initial slash will signify an "inactive" line,
*			to support GUI editing of the file.
*			01/13/99 AM. To support editing, this function only reads in
*			the strings and creates a sequence list.  Will not internalize
*			the algorithm and the rule file.  (See internSeq).
********************************************/

void Ana::parseSeq(bool verbose)
{
if (!seqbuf_)
	{
	if (verbose_)																// 02/19/02 AM.
		*gout << _T("[parseSeq: No analyzer sequence buffer.]") << std::endl;
	std::_t_strstream gerrStr;						// 02/19/02 AM.
	gerrStr << _T("[parseSeq: No analyzer sequence buffer.]") << std::ends;
	errOut(&gerrStr,false);																// 02/19/02 AM.
	//exit(1);			// 06/15/99 AM.
	return;				// 06/15/99 AM.
	}

// Create the internal analyzer sequence object.
makeSeq();

/* Get lookahead token */
_TCHAR *token;
_TCHAR *algo;
_TCHAR *data;
_TCHAR *buf;
bool eol;
bool active = true;			// Default.									// 01/13/99 AM.
eol = false;
buf = seqbuf_;
algo = 0;
data = 0;
_TCHAR comm[MAXSTR];
comm[0] = '\0';
bool first = true;		// First pass still pending.				// 01/13/99 AM.
npasses_ = 0;				// COUNTING PASSES.							// 06/13/00 AM.

while (*buf)	// For each line of file.
	{
	// Slash signifies an inactive pass.								// 01/07/99 AM.
	if (*buf == '/')
		{
		++buf;			// Skip the slash.					// OPT.	// 11/29/00 AM.
		active = false;
		}

	token = next_token(/*DU*/ buf, eol, comm);
	if (!token) goto line;

	/* If it's numeric, ignore. */
	if (!strcmp_i(_T("folder"),token) || !strcmp_i(_T("stub"),token) || !strcmp_i(_T("end"),token)) {
		goto skip;
	}
	if (!strcmp_i(_T("pat"),token)) {
		token = _T("nlp");
	}

	if (_istdigit((_TUCHAR)*token))
		{
		if (eol) goto line;		// Lone number on line.
		token = next_token(/*DU*/ buf, eol, comm);
		if (!token) goto line;
		}

	if (!alphabetic(*token))		// 09/22/99 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Bad algorithm in sequence file=")
				  << token << _T("]") << std::ends;
		errOut(&gerrStr,false);

		//exit(1);				// 06/15/99 AM.
		return;					// 06/15/99 AM.
		}

	algo = token;
	if (eol) goto line;
	data = next_token(/*DU*/ buf, eol, comm);
	if (eol)
		goto line;
skip:
	while (next_token(/*DU*/ buf, eol, comm) && !eol)
		{
		// Ignore tokens at end of line.
		}

line:
	if (algo)
		{
		++npasses_;																// 06/13/00 AM.
		Delt<Seqn> *delt;
		Seqn *seqn;
		delt = Seqn::makeDelt(0, data, 0, 0);
		seqn = delt->getData();
		seqn->setActive(active);
		seqn->setAlgoname(algo);
		seqn->setComment(comm);
		// Add data as pass file for current pass.
		addSeq(delt);

		// Process current line's pass.
//		if (verbose)															// 02/19/02 AM.
//			{
//			*gout << "algo=" << algo << std::endl;
//			*gout << "data=" << str(data) << std::endl;
//			}

		// ADD TO INTERNAL ANALYZER SEQUENCE.
		// PARSE PAT (RULES) FILES HERE.
		//addPass(algo, data, rfa);										// 01/13/99 AM.
		}
	else if (first)			// Get header comment if any.			// 01/13/99 AM.
		{
		if (comm[0])			// Comment is present.
			{
			setComment(comm);	// Set header comment for the file.
			}
		first = false;
		}
	algo = 0;
	data = 0;
	eol = false;
	comm[0] = '\0';			// 01/13/99 AM.
	active = true;				// 01/13/99 AM.
	}

}


/********************************************
* FN:		MAKESEQ
* CR:		10/17/98 AM.
* SUBJ:	Create the internal analyzer sequence.
* ALLOC:	Creates sequence list object.
********************************************/

void Ana::makeSeq()
{
Dlist<Seqn> *slist;
slist = new Dlist<Seqn>();
setSeqlist(slist);
}


/********************************************
* FN:		ADDSEQ
* CR:		10/18/98 AM.
* SUBJ:	Add element to the internal analyzer sequence.
********************************************/

void Ana::addSeq(Delt<Seqn> *delt)
{
Dlist<Seqn> *slist;
slist = getSeqlist();
slist->rpush(delt);
}


/********************************************
* FN:		INTERNSEQ
* CR:		01/13/99 AM.
* SUBJ:	Internalize the analyzer sequence list.
* NOTE:	Build algorithm object, parse the pass file and internalize,
*			if any.  To allow separate editing and execution of the analyzer
*			specification files.
********************************************/

bool Ana::internSeq(
	Ana &rfa,
	Eana *erfa,			// Configuration for the RFA.					// 10/13/99 AM.
//	NLP *nlp																		// 08/27/02 AM.
	void *htfunc		// Built-in hash table.							// 08/27/02 AM.
	)
{
Dlist<Seqn> *slist;
slist = getSeqlist();
if (!slist)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[No steps in analyzer sequence.]") << std::ends;
	return errOut(&gerrStr,false);
	}

// If not interning RFB, reset hash table for user functions.	// 12/20/01 AM.
bool notrfb = !(erfa->getFreadrfb());									// 12/20/01 AM.
if (notrfb)																		// 12/20/01 AM.
	{
	Ifunc::deleteHtab(htfunc_);											// 12/20/01 AM.
	htfunc_ = Ifunc::createHtab(HTFUNC_SIZE);							// 12/20/01 AM.
	}

// Traverse the sequence list, internalizing algorithms.
// If pat pass, parse and internalize pass file.
Delt<Seqn> *selt;
Seqn *seqn;
long currseqpass = 0;														// 08/23/02 AM.
bool okparse = true;															// 05/26/01 AM.
Ifile *rf = 0;																	// 12/20/01 AM.
for (selt = slist->getFirst(); selt; selt = selt->Right())
	{
	seqn = selt->getData();
	seqn->setPassnum(++currseqpass);										// 05/10/00 AM.
	if (seqn->getActive())
		{
		if (!internPass(seqn, rfa, erfa))	// Asynchrony.			// 12/11/99 AM.
			okparse = false;													// 05/26/01 AM.
		// Load the hash table for user-defined functions.			// 12/20/01 AM.
		if (notrfb																// 12/20/01 AM.
		 && (rf = seqn->getRulesfile()))									// 12/20/01 AM.
			if (!Ifunc::loadHtab(htfunc_,									// 12/20/01 AM.
							rf->getDecls(),									// 12/20/01 AM.
//							nlp->getHtfunc(),		// Builtin hash tab.	// 08/27/02 AM.
							htfunc,												// 08/27/02 AM.
							currseqpass ))										// 08/23/02 AM.
				okparse = false;												// 12/21/01 AM.
		}
	}
dirty_ = false;			// Analyzer sequence is clean.			// 12/20/01 AM.
return okparse;																// 05/26/01 AM.
}


/********************************************
* FN:		INTERNPASS
* CR:		01/13/99 AM.
* SUBJ:	Internalize one pass in the analyzer sequence.
* NOTE:	Build algorithm object, parse the pass file and internalize,
*			if any.  To allow separate editing and execution of the analyzer
*			specification files.
*			Copied from addPass.
********************************************/

bool Ana::internPass(Seqn *pass, Ana &rfa,
	Eana *erfa											// 10/13/99 AM.
	)
{
if (!pass)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[internPass: empty pass.]") << std::ends;
	errOut(&gerrStr,false);
	return false;															// 12/11/99 AM.
	}
if (pass->getAlgo() || pass->getRules() || pass->getRulesfile())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[internPass: Internalized objects present in pass.]") << std::ends;
	errOut(&gerrStr,false);
	return false; 
	}

_TCHAR *s_algo = 0;
_TCHAR *s_data = 0;

s_algo = pass->getAlgoname();
s_data = pass->getRulesfilename();
//cout << "[Internpass: pass file name=" << s_data << "]" << std::endl;
//cout << "[Pass id=" << (long) pass << "]" << std::endl;

Algo *algo;
Dlist<Irule> *rules;
Ifile *rulesfile;				// 11/25/98 AM.
rules = 0;
rulesfile = 0;					// 11/25/98 AM.
Gen *gen = rfa.getGen();													// 05/10/00 AM.
if (gen)		// Gen'ing code for analyzer.								// 05/10/00 AM.
	gen->setId(pass->getPassnum());										// 05/10/00 AM.
	// Note:	The Angen pass of RFB takes care of code gen for the
	// PAT pass.

if (empty(s_algo))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[internPass: Given empty algorithm.]") << std::ends;
	errOut(&gerrStr,false);
	return false;																// 12/11/99 AM.
	}
else if (	!strcmp_i(s_algo, _T("folder")))							// 02/03/05 DD
	return true;																// 02/03/05 DD.
else if (	!strcmp_i(s_algo, _T("tokenize"))
			|| !strcmp_i(s_algo, _T("tok"))
			|| !strcmp_i(s_algo, _T("token"))
		  )
	{
	algo = new Tok();
	if (gen)		// Gen'ing code for analyzer.							// 05/10/00 AM.
		genTok(pass, gen);													// 05/10/00 AM.
	}
else if (	!strcmp_i(s_algo, _T("cmltokenize"))					// 08/18/08 AM.
			|| !strcmp_i(s_algo, _T("cmltok"))							// 08/18/08 AM.
		  )
	{
	algo = new CMLTok();														// 08/18/08 AM.
	if (gen)		// Gen'ing code for analyzer.							// 08/18/08 AM.
		genCMLTok(pass, gen);												// 08/18/08 AM.
	}
else if ( !strcmp_i(s_algo, _T("dicttok"))							// 07/29/11 AM.
		  )
	{
	algo = new DICTTok();														// 07/29/11 AM.
	if (gen)		// Gen'ing code for analyzer.							// 07/29/11 AM.
		genDICTTok(pass, gen);												// 07/29/11 AM.
	}
else if ( !strcmp_i(s_algo, _T("dicttokz"))							// 08/16/11 AM.
		  )
	{
//	algo = new DICTTok();													// 08/16/11 AM.
	DICTTok *dt = new DICTTok();											// 08/16/11 AM.
	dt->setZapwhite(true);													// 08/16/11 AM.
	algo = dt;																	// 08/16/11 AM.
	if (gen)		// Gen'ing code for analyzer.							// 08/16/11 AM.
		genDICTTokz(pass, gen);												// 08/16/11 AM.
	}
else if ( !strcmp_i(s_algo, _T("chartok"))							// 07/29/11 AM.
		  )
	{
	Tok *tk = new Tok();
	tk->setSeparateChars(true);
	algo = tk;		
	if (gen)		// Gen'ing code for analyzer.							// 05/10/00 AM.
		genTok(pass, gen);													// 05/10/00 AM.
	}
else if (	!strcmp_i(s_algo, _T("lines"))
			|| !strcmp_i(s_algo, _T("line"))
		  )
	{
	algo = new Line();
	if (gen)		// Gen'ing code for analyzer.							// 05/10/00 AM.
		genLines(pass, gen);													// 05/10/00 AM.
	}
else if (!strcmp_i(s_algo, _T("nlp")))
	{
	// 12/02/98 AM. Pat algorithm should have all the code in this case.
	// Current function should merely say: Pat, instantiate yourself.
	// Special constructor like Pat(s_data, rfa, /*DU*/ rulesfile);
	// (Or, maybe Seqn).

	if (empty(s_data))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[In sequence file: Pat pass missing pass file.]")
				  << std::ends;
		errOut(&gerrStr,false);
		return false;															// 12/11/99 AM.
		}

	// PARSE THE PASS FILE HERE.
	// (Would like this function a little higher in the call stack.)
	// Some rules tree type should get return of this.
	if (!parseRulesfile(s_data, rfa, erfa, s_algo,
								pass->getPassnum(),							// 08/23/02 AM.
								/*UP*/ rulesfile))
		{
		// Couldn't parse pass file for this pass.
//		strstream gerrStr(Errbuf,MAXSTR,std::ios::out);
//		if (s_data && *s_data)												// 05/26/01 AM.
//			gerrStr << "[Bad pass=" << s_data							// 05/26/01 AM.
//					  << ". Skipping.]" << std::ends;							// 05/26/01 AM.
//		else																		// 05/26/01 AM.
//			gerrStr << "[Bad pass=" << s_algo << ". Skipping.]" << std::ends;
//		return errOut(false,currseqpass,0);
		return false;															// 05/26/01 AM.
		}

	if (rulesfile)									// 11/25/98 AM.
		rules = rulesfile->getRules();		// 11/25/98 AM.

	// Pass file shouldn't go into Pat pass, but into
	// analyzer sequence element.
	algo = new Pat();
	}
else if (!strcmp_i(s_algo, _T("rec")))				// 11/08/99 AM.
	{
	if (empty(s_data))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[In sequence file: Rec pass missing pass file.]")
				  << std::ends;
		errOut(&gerrStr,false);
		return false;														// 12/11/99 AM.
		}

	// PARSE THE PASS FILE HERE.
	if (!parseRulesfile(s_data, rfa, erfa, s_algo,
								pass->getPassnum(),							// 08/23/02 AM.
								/*UP*/ rulesfile))							// 11/08/99 AM.
		{
		// Couldn't parse pass file for this pass.
//		strstream gerrStr(Errbuf,MAXSTR,std::ios::out);
//		if (s_data && *s_data)												// 05/26/01 AM.
//			gerrStr << "[Bad pass=" << s_data							// 05/26/01 AM.
//					  << ". Skipping.]" << std::ends;							// 05/26/01 AM.
//		else																		// 05/26/01 AM.
//			gerrStr << "[Bad pass=" << s_algo << ". Skipping.]" << std::ends;
//		return errOut(false,currseqpass,0);
		return false;															// 05/26/01 AM.
		}

	if (rulesfile)
		rules = rulesfile->getRules();

	algo = new Rec();									// 11/08/99 AM.
	}
else if (	!strcmp_i(s_algo, _T("stub"))			// 06/23/99 AM.
		  )
	{
	algo = new Stub_s();
	}
else if (	!strcmp_i(s_algo, _T("end"))				// 06/23/99 AM.
		  )
	{
	algo = new Stub_e();
	}
else if (	!strcmp_i(s_algo, _T("gen"))				// 04/27/00 AM.
		  )
	algo = new Angen();								// 04/27/00 AM.
else if (	!strcmp_i(s_algo, _T("intern"))			// 05/02/00 AM.
		  )
	algo = new Intern();								// 05/02/00 AM.
else if (	!strcmp_i(s_algo, _T("nintern"))		// 05/02/00 AM.
		  )
	algo = new Nintern();							// 05/02/00 AM.
else if (	!strcmp_i(s_algo, _T("hash"))			// 05/02/00 AM.
		  )
	algo = new Phash();								// 05/02/00 AM.
else if (	!strcmp_i(s_algo, _T("genhash"))		// 06/14/00 AM.
		  )
	algo = new Genhash();							// 06/14/00 AM.


// USER-DEFINED PASS TYPES GO HERE.
else if (!Algo::userAlgo(s_algo, s_data, /*DU*/ algo))	// 12/02/98 AM.
	return false;														// 12/11/99 AM.
						// Botched user pass or unknown pass.	// 12/02/98 AM.

// Got a good algorithm for current pass.
// Add internalized objects to pass.
#ifdef junk_
algo->setDebug(false);					// 11/11/98 AM.
Delt<Seqn> *delt;
delt = Seqn::makeDelt(algo, s_data, rules, rulesfile);
// Add data as pass file for current pass.
addSeq(delt);
#endif

pass->setAlgo(algo);
pass->setRules(rules);
pass->setRulesfile(rulesfile);
return true;															// 12/11/99 AM.
}


/********************************************
* FN:		UNINTERNPASS
* CR:		01/15/99 AM.
* SUBJ:	Unintern one pass in the analyzer sequence.
* NOTE:	To help with analyzer sequence editing.
********************************************/

void Ana::uninternPass(Seqn *pass)
{
if (!pass)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[uninternPass: empty pass.]") << std::ends;
	errOut(&gerrStr,false);
	return;
	}
pass->unintern();
}


/********************************************
* FN:		ADDPASS
* CR:		10/18/98 AM.
* OBSOLETE:	10/13/99 AM.  Moving addPass. to old.txt.
********************************************/



/********************************************
* FN:		PARSERULESFILE
* CR:		10/19/98 AM.
* SUBJ:	Convert the pass file to an internal tree representation.
* NOTE:	Create a tree of objects that are ready to execute,
*			or at least ready to submit to the rule-matcher.
*			Build a mini-parser that uses tokenizer already built!
********************************************/

bool Ana::parseRulesfile(
	_TCHAR *file,
	Ana &rfa,			// Meta-analyzer for parsing pass file.
	Eana *erfa,			// Runtime config for RFA						// 10/13/99 AM.
	_TCHAR *algo,			// Algo of the input file!						// 05/31/00 AM.
	long passnum,		// Pass number of input rule file.			// 08/23/02 AM.
	/*UP*/
	Ifile* &rulesfile	// Internal rep built for pass file.		// 11/25/98 AM.
	)
{
if (empty(file))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[parseRulesfile: Given NULL pass file name.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Give the RFA an NLP object to write with....stopgap measure.// 08/29/02 AM.
rfa.setNLP(nlp_);																// 08/29/02 AM.

// Invoke the rule-file analyzer on the current pass file!
// Read pass file into buffer.
// Analyze.
bool intern = true;	// If pass file interned ok.					// 05/26/01 AM.
if (!RFA::execute_rfa(file, rfa, getSpecdir(), erfa, algo,
													passnum,						// 08/23/02 AM.
													/*UP*/
													rulesfile,					// 12/03/98 AM.
													intern						// 05/26/01 AM.
													))
	return false;

return intern;			// Return true if interned ok.				// 05/26/01 AM.
}


/********************************************
* FN:		PRETTYSEQ
* CR:		10/19/98 AM.
* SUBJ:	Pretty-print the analyzer sequence.
********************************************/

void Ana::prettySeq()
{
Delt<Seqn> *seq;
Delt<Seqn> *ptr;
seq = getSeq();
ptr = seq;						// For traversing analyzer sequence.

Seqn *pass;						// Current pass.

*gout << _T("\nPasses: ");
while (ptr)
	{
	pass = ptr->getData();
	*gout << *pass << _T(" ");
	ptr = ptr->Right();
	}
*gout << std::endl << std::endl;
}


/********************************************
* FN:		WRITESEQFILE
* CR:		01/15/99 AM.
* SUBJ:	Write the analyzer sequence to the default file.
* NOTE:	To support user editing of the sequence file.
********************************************/

void Ana::writeSeqfile()
{
// Open temporary output file.
_TCHAR fname[FILENAME_MAX*2];

// G++ BUGS.																	// 03/08/00 AM.
//ostrstream *outStr = 0;													// 03/08/00 AM.
//ofstream *fout = 0;														// 03/08/00 AM.
//outStr = new ostrstream(fname, MAXSTR, std::ios::out);				// 03/08/00 AM.
//*outStr << this->getSpecdir() << "\\tmp.seq" << std::ends;			// 03/08/00 AM.
//fout = new ofstream(fname, std::ios::out);								// 03/08/00 AM.

#ifdef UNICODE
wsprintf(fname, _T("%s%c%s"), this->getSpecdir(), DIR_CH, _T("tmp.seq"));
#elif LINUX
sprintf(fname, _T("%s%c%s"), this->getSpecdir(), DIR_CH, _T("tmp.seq"));
#else
sprintf_s(fname, _T("%s%c%s"), this->getSpecdir(), DIR_CH, _T("tmp.seq"));
#endif
std::_t_ofstream fout(TCHAR2CA(fname), std::ios::out);											// 03/08/00 AM.

// Write out the sequence file.
//genSeqfile(*fout);							// 11/04/99 AM.			// 03/08/00 AM.
genSeqfile(fout);																// 03/08/00 AM.

//delete fout;																	// 03/08/00 AM.
// should probably have had: delete outStr;							// 03/08/00 AM.

copy_file(fname, this->getSeqfile());	// Copy to sequence file.
}


/********************************************
* FN:		GENSEQFILE
* CR:		11/04/99 AM.
* SUBJ:	Write out the analyzer sequence.
* NOTE:	Pulling out the guts of writeSeqfile.
********************************************/

void Ana::genSeqfile(std::_t_ostream &ofile)
{
// Print the file's header comment.
if (this->getComment())
	ofile << _T("# ") << this->getComment() << std::endl;

// Traverse the sequence. Write to a temp file.  When successful,
// copy to the default analyzer.seq file.
Delt<Seqn> *delt = 0;
Seqn *seqn = 0;
for (delt = this->getSeq(); delt; delt = delt->Right())
	{
	seqn = delt->getData();
	if (!(seqn->getActive()))
		ofile << _T("/");
	ofile << seqn->getAlgoname() << _T("\t")
			<< seqn->getRulesfilename();
	if (seqn->getComment())
		{
		ofile << _T("\t# ")
				<< seqn->getComment()
				<< std::endl;
		}
	}
}


/********************************************
* FN:		GENANA
* CR:		11/04/99 AM.
* SUBJ:	Write the entire analyzer to files.
* NOTE:	Using internalized structures, not rulefile.
********************************************/

void Ana::genAna(
	_TCHAR *dir				// Directory to write files to.
	)
{
if (!dir)
	dir = _T(".");				// Default to current directory.

////////////////////////////////////
// Write the analyzer sequence file.
////////////////////////////////////

// Set up sequence filename.
_TCHAR fname[MAXSTR];
// WORKAROUND FOR G++ BUG.													// 03/08/00 AM.
//ostrstream *outStr = 0;													// 03/08/00 AM.
//ofstream *fout = 0;														// 03/08/00 AM.
//outStr = new ostrstream(fname, MAXSTR, std::ios::out);				// 03/08/00 AM.
//*outStr << dir << "\\" << DEFAULT_SEQFILE << std::ends;				// 03/08/00 AM.
//fout = new ofstream(fname, std::ios::out);								// 03/08/00 AM.

#ifdef UNICODE
wsprintf(fname, _T("%s%c%s"), dir, DIR_CH, DEFAULT_SEQFILE);
#elif LINUX
sprintf(fname, _T("%s%c%s"), dir, DIR_CH, DEFAULT_SEQFILE);
#else
sprintf_s(fname, _T("%s%c%s"), dir, DIR_CH, DEFAULT_SEQFILE);
#endif
std::_t_ofstream fout(TCHAR2CA(fname), std::ios::out);											// 03/08/00 AM.

//genSeqfile(*fout);	// Write out the sequence file.				// 03/08/00 AM.
genSeqfile(fout);		// Write out the sequence file.				// 03/08/00 AM.

//delete fout;
//delete outStr;		// FIX MEM LEAKS.								// 02/25/00 AM.


//////////////////////////////////////////////////
// Write out a file for each pass of the analyzer.
//////////////////////////////////////////////////
Delt<Seqn> *delt = 0;
Seqn *seqn = 0;
for (delt = seqlist_->getFirst(); delt; delt = delt->Right())
	{
	seqn = delt->getData();
	seqn->genSeqn(dir);			// Generate rules file!
	}
}


/********************************************
* FN:		GENTOK
* CR:		05/10/00 AM.
* SUBJ:	Gen code for tokenize pass.
********************************************/

bool Ana::genTok(Seqn *pass, Gen *gen)
{
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
long id = pass->getPassnum();

// Write out prototype.
*fhead << _T("bool tokenize") << id << _T("(Parse *);");	// 04/04/03 AM.
Gen::nl(fhead);																// 04/04/03 AM.


// Write out function.
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("bool tokenize") << id << _T("(Parse *parse)");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
// Return false if fatal error on tokenization.						// 01/26/02 AM.
*fcode << _T("if (!Arun::tokenize(parse,") << id << _T(")) return false;");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("return true;");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("}");
Gen::nl(fcode);																// 04/04/03 AM.
return true;
}

/********************************************
* FN:		GENCMLTOK
* CR:		08/18/08 AM.
* SUBJ:	Gen code for optimized CML tokenize pass.
********************************************/

bool Ana::genCMLTok(Seqn *pass, Gen *gen)
{
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
long id = pass->getPassnum();

// Write out prototype.
*fhead << _T("bool cmltokenize") << id << _T("(Parse *);");
Gen::nl(fhead);


// Write out function.
Gen::nl(fcode);
*fcode << _T("bool cmltokenize") << id << _T("(Parse *parse)");
Gen::nl(fcode);
*fcode << _T("{");
Gen::nl(fcode);
// Return false if fatal error on tokenization.
*fcode << _T("if (!Arun::cmltokenize(parse,") << id << _T(")) return false;");
Gen::nl(fcode);
*fcode << _T("return true;");
Gen::nl(fcode);
*fcode << _T("}");
Gen::nl(fcode);
return true;
}


/********************************************
* FN:		GENDICTTOK
* CR:		07/29/11 AM.
* SUBJ:	Gen code for optimized DICT tokenize pass.
********************************************/

bool Ana::genDICTTok(Seqn *pass, Gen *gen)
{
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
long id = pass->getPassnum();

// Write out prototype.
*fhead << _T("bool dicttok") << id << _T("(Parse *);");
Gen::nl(fhead);


// Write out function.
Gen::nl(fcode);
*fcode << _T("bool dicttok") << id << _T("(Parse *parse)");
Gen::nl(fcode);
*fcode << _T("{");
Gen::nl(fcode);
// Return false if fatal error on tokenization.
*fcode << _T("if (!Arun::dicttok(parse,") << id << _T(")) return false;");
Gen::nl(fcode);
*fcode << _T("return true;");
Gen::nl(fcode);
*fcode << _T("}");
Gen::nl(fcode);
return true;
}


/********************************************
* FN:		GENDICTTOKZ
* CR:		08/16/11 AM.
* SUBJ:	Gen code for optimized DICT tokenize pass (w ZAP WHITESPACE).
********************************************/

bool Ana::genDICTTokz(Seqn *pass, Gen *gen)
{
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
long id = pass->getPassnum();

// Write out prototype.
*fhead << _T("bool dicttokz") << id << _T("(Parse *);");
Gen::nl(fhead);


// Write out function.
Gen::nl(fcode);
*fcode << _T("bool dicttokz") << id << _T("(Parse *parse)");
Gen::nl(fcode);
*fcode << _T("{");
Gen::nl(fcode);
// Return false if fatal error on tokenization.
*fcode << _T("if (!Arun::dicttokz(parse,") << id << _T(")) return false;");
Gen::nl(fcode);
*fcode << _T("return true;");
Gen::nl(fcode);
*fcode << _T("}");
Gen::nl(fcode);
return true;
}

/********************************************
* FN:		GENLINES
* CR:		05/10/00 AM.
* SUBJ:	Gen code for line pass.
********************************************/

bool Ana::genLines(Seqn *pass, Gen *gen)
{
std::_t_ofstream *fcode = gen->getFcode();
std::_t_ofstream *fhead = gen->getFhead();
long id = pass->getPassnum();

// Write out prototype.
*fhead << _T("bool lines") << id << _T("(Parse *);");
Gen::nl(fhead);																// 04/04/03 AM.

// Write out function.
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("bool lines") << id << _T("(Parse *parse)");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("{");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("Arun::lines(parse,") << id << _T(");");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("return true;");
Gen::nl(fcode);																// 04/04/03 AM.
*fcode << _T("}");
Gen::eol(fcode);																// 04/04/03 AM.
return true;
}


/********************************************
* FN:		KBGETSEQ
* CR:		11/11/00 AM.
* SUBJ:	Get analyzer sequence from kb.
* NOTE:	Part of "analyzer in kb" implementation in VisualText.
********************************************/

bool Ana::kbgetSeq()
{
if (seqlist_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[kbgetSeq: Warning: Analyzer already has sequence.]") << std::ends;
	return errOut(&gerrStr,true);	// Recovery: use the existing sequence.	
	}

if (!cg_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[kbgetSeq: No kb loaded.]") << std::ends;
	return errOut(&gerrStr,false);
	}
CONCEPT *con = 0;
if (!(con = cg_->findRoot()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[kbgetSeq: Kb error(1).]") << std::ends;
	return errOut(&gerrStr,false);
	}
if (!(con = cg_->findConcept(con,_T("sys"))))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[kbgetSeq: Kb error(2).]") << std::ends;
	return errOut(&gerrStr,false);
	}
bool flag = false;			// 06/29/03 AM.
if (!(con = cg_->findConcept(con,_T("gui"))))
	{
//	strstream gerrStr(Errbuf,MAXSTR,std::ios::out);
//	gerrStr << "[Note: No analyzer sequence in the kb.]" << std::ends;
//	return errOut(false);
	}
else	// 06/29/03 AM.
	flag = true;	// 06/29/03 AM.
if (!(con = cg_->findConcept(con,_T("spec"))))
	{
//	strstream gerrStr(Errbuf,MAXSTR,std::ios::out);
//	gerrStr << "[Note: No analyzer sequence in the kb.]" << std::ends;
//	return errOut(false);
	}
else	// 06/29/03 AM.
	flag = true;	// 06/29/03 AM.

if (flag)	// 06/29/03 AM.
	{
	std::_t_strstream gerrStr;	// 06/29/03 AM.
	gerrStr << _T("[Ignoring analyzer sequence in the kb.]") << std::ends;
	errOut(&gerrStr,false);		 // 06/29/03 AM.
	}

// NEVER GETTING SEQUENCE FROM KB.	// 06/29/03 AM.
return false;								// 06/29/03 AM.

if (!(con = cg_->Down(con)))
	return true;	// Ok to have no passes in analyzer.

// Create the internal analyzer sequence object.
makeSeq();

_TCHAR *algo = 0;
_TCHAR *data = 0;
bool active = true;			// Default.
_TCHAR *comm = 0;
bool first = true;		// First pass still pending.
npasses_ = 0;				// COUNTING PASSES.
Delt<Seqn> *delt;
Seqn *seqn;
long long num;

for (; con; con = cg_->Next(con))	// While traversing passes.
	{
	algo = KB::strVal(con,_T("algo"),cg_,htab_);	// 02/12/07 AM.
	data = KB::strVal(con,_T("data"),cg_,htab_);	// 02/12/07 AM.
	comm = KB::strVal(con,_T("comm"),cg_,htab_);	// 02/12/07 AM.
	cg_->findVal(con, _T("active"), num);
	active = (num ? true : false);

	++npasses_;
	delt = Seqn::makeDelt(0, data, 0, 0);
	seqn = delt->getData();
	seqn->setActive(active);
	seqn->setAlgoname(algo);
	seqn->setComment(comm);
	// Add data as pass file for current pass.
	addSeq(delt);
	}
return true;
}
