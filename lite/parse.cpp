/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PARSE.CPP
* FILE:	lite\parse.cpp
* CR:		10/08/98 AM.
* SUBJ:	Class responsible for analysis of a single text.
*
*******************************************************************************/

#include "StdAfx.h"

#ifdef LINUX
#include <iostream>	// 09/27/19 AM.
#include <strstream>	// 09/27/19 AM.
#ifdef _ODBC
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#endif
#endif

#include "machine.h"										// 03/08/00 AM.

#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"														// 11/12/00 AM.
#include "io.h"
#include "std.h"
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"				// 02/14/01 AM.
#include "lite/mach.h"									// 10/11/99 AM.
#include "chars.h"
//#include "lite/dlist.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"		// 02/26/01 AM.
#include "htab.h"				// 02/26/01 AM.
#include "ipair.h"
#include "pn.h"				// 02/14/01 AM.
#include "ln.h"				// 02/14/01 AM.
#include "ana.h"
#include "seqn.h"				// 02/26/01 AM.
#include "Eana.h"				// 02/26/01 AM.
//#include "iarg.h"				// 05/23/01 AM.
#include "parse.h"
#include "lite/nlp.h"		// FOR DEBUGGING!!!	// 10/10/99 AM.
#include "var.h"											// 09/26/00 AM.
#include "lite/nlppp.h"										// 05/17/00 AM.

#include <filesystem>

int Parse::count_ = 0;

// Keep cout safe, in case it is rebound.
// NEED XP FIX.	// 01/12/03 AM.
//static ostream safecout = cout;											// 05/06/02 AM.

/********************************************
* FN:		Special Functions for Parse Class
* CR:		10/08/98 AM.
********************************************/

Parse::Parse()
{
text = 0;
input[0] =  '\0';
output[0] =  '\0';
appdir_[0] =  '\0';
anasdir_[0] = '\0';
kbdir_[0] =  '\0';
outdir_[0] = '\0';
tree = 0;
lines = 0;
ana = 0;
nlp_ = 0;				// 06/25/03 AM.
htab_ = 0;				// 06/25/03 AM.
eana_ = 0;				// 10/13/99 AM.
vars_ = 0;				// 12/07/98 AM.
verbose = false;		// Default.  // 12/26/98 AM.
intern_ = true;																// 05/26/01 AM.
currpass_ = 0;			// 07/24/01 AM.
rulepass_ = 0;			// 02/03/05 AM.
inputpass_ = 0;		// 08/23/02 AM.
seq_ = 0;				// 07/24/01 AM.
line_	= 0;				// 08/24/02 AM.
inputline_ = 0;		// 08/24/02 AM.
length = 0;

// Make an empty list when creating the Parse object.				// 05/23/01 AM.
// (That way, never have to check if it exists or not.			// 05/23/01 AM.
ostrs_ = new Dlist<Iarg>();	// Make empty list.					// 05/23/01 AM.

// The output file is not setup, by default.	// 01/07/99 AM.
out_ = 0;
sout_ = 0;					// 05/13/99 AM.
fout_ = 0;					// 05/13/99 AM.

cbuf_ = 0;					// 05/11/02 AM.
cbufmax_ = 0;				// 05/11/02 AM.
cbuftot_ = 0;				// 05/11/02 AM.
cbufover_ = false;		// 05/11/02 AM.

// Default will be standard output stream.
cout_ = &std::_t_cout;				// 05/04/03 AM.

#ifdef _ODBC
henv_ = 0;					// 05/23/02 AM.
hdbc_ = 0;					// 05/23/02 AM.
hstmt_ = 0;					// 05/25/02 AM.
cursor_ = 0;				// 05/26/02 AM.
#endif

blobs_ = new Dlist<Iarg>();	// Make empty list.						// 02/27/03 AM.
datum_ = 0;					// 03/13/03 AM.

#ifndef LINUX
hdll_ = 0;					// 01/29/99 AM.
#endif

collect_ = 0;				// 05/17/00 AM.
nlppp_	= 0;				// 05/17/00 AM.
algo_		= 0;				// 05/31/00 AM.

fbatchstart_ = false;	// 10/19/00 AM.

logfile_ = 0;	// VTLOG	// 05/06/13 AM.

#ifndef STABLE_
++count_;
#endif
}

Parse::~Parse()
{
clean();			// 11/17/98 AM.

#ifndef STABLE_
--count_;
#endif
}

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/08/98 AM.
********************************************/

Ana  *Parse::getAna()		{return ana;	 }
NLP  *Parse::getNLP()		{return nlp_;	 }							// 06/25/03 AM.
Htab *Parse::getHtab()		{return htab_;	 }							// 06/25/03 AM.
Eana *Parse::getEana()		{return eana_;	 }							// 10/13/99 AM.
_TCHAR *Parse::getInput()    {return input;  }
_TCHAR *Parse::getOutput()   {return output; }
_TCHAR *Parse::getAppdir()	{return appdir_;}
_TCHAR *Parse::getAnasdir()	{return anasdir_;}
_TCHAR *Parse::getKBdir()	{return kbdir_;}
_TCHAR *Parse::getText()     {return text;   }
long  Parse::getLength()	{return length; }
long Parse::getUlength()	{return ulength;}	// [UNICODE]
TREE *Parse::getTree() {return tree;   }
LTREE *Parse::getLines(){return lines;  }
Dlist<Ipair> *Parse::getVars()	{return vars_;}
Dlist<Iarg>	 *Parse::getOstrs()	{return ostrs_;}					// 05/23/01 AM.
std::_t_ostream		 *Parse::getOut()		{return out_;}
std::_t_ostream		 *Parse::getCout()	{return cout_;}					// 05/04/03 AM.
std::_t_ostream		 *Parse::getSout()	{return sout_;}					// 05/13/99 AM.
std::_t_ofstream		 *Parse::getFout()	{return fout_;}					// 05/13/99 AM.
#ifndef LINUX
HINSTANCE	  Parse::getHdll()	{return hdll_;}					// 01/29/99 AM.
#endif
_TCHAR *Parse::getOutdir()	{return outdir_;}							// 03/10/99 AM.
COLL *Parse::getCollect()	{return collect_;}						// 05/17/00 AM.
Nlppp *Parse::getNlppp()	{return nlppp_;}							// 05/17/00 AM.
_TCHAR *Parse::getAlgo()		{return algo_;}							// 05/31/00 AM.
bool	Parse::getFbatchstart() {return fbatchstart_;}				// 10/19/00 AM.
bool	Parse::getIntern()	{return intern_;}							// 05/26/01 AM.
long	Parse::getCurrpass()		{return currpass_;}		// REN.	// 08/22/02 AM.
long	Parse::getRulepass()		{return rulepass_;}					// 02/03/05 AM.
long	Parse::getInputpass()	{return inputpass_;}					// 08/23/02 AM.
Delt<Seqn> *Parse::getSeq()		{return seq_;}						// 07/24/01 AM.

#ifdef LINUX
std::ostringstream		*Parse::getCbuf()	{return cbuf_;}					// 05/11/02 AM. // 09/27/19 AM.
#else
std::_t_ostrstream		*Parse::getCbuf()	{return cbuf_;}					// 05/11/02 AM.
#endif
long	Parse::getCbufmax()			{return cbufmax_;}				// 05/11/02 AM.
long	Parse::getCbuftot()			{return cbuftot_;}				// 05/11/02 AM.
bool	Parse::getCbufover()		{return cbufover_;}					// 05/11/02 AM.

#ifdef _ODBC
SQLHENV Parse::getHenv()	{return henv_;}							// 05/23/02 AM.
SQLHDBC Parse::getHdbc()	{return hdbc_;}							// 05/23/02 AM.
SQLHDBC Parse::getHstmt()	{return hstmt_;}							// 05/25/02 AM.
SDWORD  Parse::getCursor()	{return cursor_;}							// 05/26/02 AM.
SDWORD *Parse::getPcursor()	{return &cursor_;}					// 05/26/02 AM.
#endif

Dlist<Iarg>	*Parse::getBlobs()		{return blobs_;}				// 02/27/03 AM.
_TCHAR	 *Parse::getDatum()	{return datum_;}							// 03/13/03 AM.

long	Parse::getLine()			{return line_;	}						// 08/24/02 AM.
long	Parse::getInputline()	{return inputline_;}					// 08/24/02 AM.

bool Parse::Verbose()		{return verbose;}

_TCHAR	 *Parse::getLogfile()	{return logfile_;}	// VTLOG	// 05/06/13 AM.


/********************************************
* FN:		Modify Functions
* CR:		10/08/98 AM.
********************************************/

void Parse::setAna(Ana *x)			{ana		= x;}
void Parse::setNLP(NLP *x)			{nlp_		= x;}						// 06/25/03 AM.
void Parse::setHtab(Htab *x)		{htab_	= x;}						// 06/25/03 AM.
void Parse::setEana(Eana *x)		{eana_	= x;}						// 10/13/99 AM.
void Parse::setInput(_TCHAR  *x)   {_tcscpy(input,   x);}			// 11/19/98 AM.
void Parse::setOutput(_TCHAR *x)
{
if (x && *x)																	// 11/20/02 AM.
	_tcscpy(output,  x);
else
	output[0] = '\0';															// 11/20/02 AM.
}
void Parse::setAppdir(_TCHAR *x)	{_tcscpy(appdir_, x);}			// 12/03/98 AM.
void Parse::setText(_TCHAR *x)     {text   = x;}
void Parse::setLength(long x)    {length = x;}
void Parse::setUlength(long x)	{ulength = x;}	// [UNICODE]
void Parse::setTree(TREE *x) {tree   = x;}
void Parse::setLines(LTREE *x){lines  = x;}
void Parse::setVerbose(bool x)   {verbose= x;}
void Parse::setVars(Dlist<Ipair> *x)	{vars_ = x;}				// 12/07/98 AM.
void Parse::setOstrs(Dlist<Iarg> *x)	{ostrs_ = x;}				// 05/23/01 AM.
void Parse::setOut(std::_t_ostream			*x)	{out_	 = x;}
void Parse::setCout(std::_t_ostream		*x)	{cout_ = x;}				// 05/04/03 AM.
void Parse::setSout(std::_t_ostream		*x)	{sout_ = x;}				// 05/13/99 AM.
void Parse::setFout(std::_t_ofstream		*x)	{fout_ = x;}				// 05/13/99 AM.
#ifndef LINUX
void Parse::setHdll(HINSTANCE x)			{hdll_ = x;}				// 01/29/99 AM.
#endif
void Parse::setOutdir(_TCHAR *x)	{_tcscpy(outdir_, x);}			// 03/10/99 AM.
void Parse::setCollect(COLL *x)	{collect_	= x;}					// 05/17/00 AM.
void Parse::setNlppp(Nlppp *x)	{nlppp_		= x;}					// 05/17/00 AM.
void Parse::setAlgo(_TCHAR *x)		{algo_		= x;}
void Parse::setFbatchstart(bool x) {fbatchstart_ = x;}			// 10/19/00 AM.
void Parse::setIntern(bool x)				{intern_ = x;}				// 05/26/01 AM.
void Parse::setCurrpass(long x)		{currpass_		= x;}// REN	// 08/22/02 AM.
void Parse::setRulepass(long x)		{rulepass_		=	x;}		// 02/03/05 AM.
void Parse::setInputpass(long x)		{inputpass_	= x;}				// 08/23/02 AM.
void Parse::setSeq(Delt<Seqn> *x)		{seq_		= x;}				// 07/24/01 AM.
#ifdef LINUX
void Parse::setCbuf(std::ostringstream* x) { cbuf_ = x; }				// 05/11/02 AM. // 09/27/19 AM.
#else
void Parse::setCbuf(std::_t_ostrstream* x) { cbuf_ = x; }				// 05/11/02 AM.
#endif

void Parse::setCbufmax(long x)			{cbufmax_ = x;}			// 05/11/02 AM.
void Parse::setCbuftot(long x)			{cbuftot_ = x;}			// 05/11/02 AM.
void Parse::setCbufover(bool x)			{cbufover_ = x;}			// 05/11/02 AM.

#ifdef _ODBC
void Parse::setHenv(SQLHENV x)			{henv_	= x;}				// 05/23/02 AM.
void Parse::setHdbc(SQLHDBC x)			{hdbc_	= x;}				// 05/23/02 AM.
void Parse::setHstmt(SQLHSTMT x)			{hstmt_	= x;}				// 05/25/02 AM.
void Parse::setCursor(SDWORD x)			{cursor_	= x;}				// 05/26/02 AM.
#endif

void Parse::setBlobs(Dlist<Iarg> *x)	{blobs_	= x;}				// 02/27/03 AM.

void Parse::setLine(long x)				{line_		= x;}			// 08/24/02 AM.
void Parse::setInputline(long x)			{inputline_	= x;}			// 08/24/02 AM.

void Parse::setDatum(_TCHAR *x)												// 03/13/03 AM.
{
datum_ = 0;
if (x && *x)
	internStr(x, /*DU*/ datum_);
}

void Parse::setKBdir(_TCHAR *appdir)	{
	_TCHAR kbDir[MAXSTR];
	_stprintf(kbDir, _T("%s%ckb%cuser%c"), appdir,DIR_CH,DIR_CH,DIR_CH);
	_tcscpy(kbdir_, kbDir);
}

void Parse::setAnasdir(_TCHAR *appdir)	{
	std::filesystem::path p;
	p = appdir;
	std::string anas = p.parent_path().string();
	_tcscpy(anasdir_, anas.c_str());
	int i = 0;
}

void Parse::setLogfile(_TCHAR *x)	{logfile_ = x;}	// VTLOG	// 05/06/13 AM.

/********************************************
* FN:		GETCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Parse::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		11/13/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Parse::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Parse count=") << count_ << std::endl;
	*gout << _T("Active Parse count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Parse count=") << count_ << std::ends;
	::errOut(&gerrStr,false,0,0);
	}
}
#endif


/********************************************
* FN:		READFILE
* CR:		10/13/98 AM.
* SUBJ:	Read the input file into a buffer.
* ALLOC:	Creates buffer that must be freed.
********************************************/

bool Parse::readFile()
{
// Get file length.
long len;
long ulen;	// [UNICODE]
_TCHAR *buf;

// DOING DIRTY FILE CONVERSIONS!											// 07/18/00 AM.
//read_file(input, /*UP*/ len, buf);									// 07/18/00 AM.
//readConvert(input, buf, len, MAXSTR);								// 07/26/01 AM.
readConvertU(input,buf,len,MAXLINE);	// BIG LINE.					// 07/26/01 AM.
ulen = u_strlen(buf);	// [UNICODE]
setUlength(ulen);	// [UNICODE]
setLength(len);
setUlength(ulen);	// [UNICODE]
setText(buf);

if (len <= 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[File=") << input << _T(" not found.]") << std::ends;
	::errOut(&gerrStr,false,0,0);
	return false;
	}

if (Verbose())
	{
	*gout << _T("Length of input file is ") << len << std::endl;
	*gout << _T("\nInput text is:\n") << buf << std::endl;
	}

return true;
}


/********************************************
* FN:		COPYBUFFER
* CR:		02/06/00 AM.
* SUBJ:	Copy user-supplied input buffer to parse object.
* ALLOC:	Creates buffer that must be freed.
* NOTE:	Want optional length arg, in case input buffer is part of a
*			larger buffer.
********************************************/

bool Parse::copyBuffer(
	_TCHAR *inbuf,			// Input buffer to parse.
	long len					// If len > 0, length of input buffer to parse.
								// If len==0, Parse entire buffer.
	)
{
_TCHAR *buf = 0;
long ulen;	// [UNICODE]

if (!inbuf || !*inbuf)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Input buffer is empty.]") << std::ends;
	::errOut(&gerrStr,false,0,0);
	return false;
	}

// Get file length.
if (!len)						// If parsing entire input buffer,
	len = _tcsclen(inbuf);
if (len <= 0)
	return false;

// Allocate a buffer to hold all the chars of input buffer.
//buf = new char[len+1];
buf = Chars::create(len+1);

// Copy input buffer to parse buffer.
_tcsnccpy(buf, inbuf, len);
buf[len] = '\0';

// Get Unicode byte length. [UNICODE]
ulen = u_strlen(buf);	// [UNICODE]

// Install buffer in parse object.
setLength(len);
setUlength(ulen);	// [UNICODE]
setText(buf);

if (Verbose())
	{
	*gout << _T("Length of input buffer is ") << len << std::endl;
	*gout << _T("\nInput buffer text is:\n")  << buf << std::endl;
	}

return true;
}


/********************************************
* FN:		EXECUTE
* CR:		10/19/98 AM.
* SUBJ:	Perform the analysis of the current text.
********************************************/

void Parse::execute()
{
Delt<Seqn> *seq;
clock_t s_time, e_time;

s_time = clock();

#ifndef LINUX
if (!eana_->getFintern())													// 05/26/01 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Date: ") << today1() << _T("]") << std::ends;
	::errOut(&gerrStr,false,0,0);
	}

	// WORKAROUND.	// 02/25/00 AM.
#else
if (!eana_->getFintern())													// 05/26/01 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Date: ") << today() << _T("]") << std::ends;
	::errOut(&gerrStr,false,0,0);
	}

#endif

currpass_ = 0;																	// 08/22/02 AM.
rulepass_ = 0;																	// 02/03/05 AM.

#ifdef GEODUCK_
_TCHAR displayBuf[1001];
Seqn *s;																			// 11/18/99 DD.
#endif

Ana *ana = getAna();	// 05/05/09 AM.
if (!ana)	// 05/05/09 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Error: No analyzer built.]") << std::ends;
	::errOut(&gerrStr,false,0);
	return;	// 05/05/09 AM.
	}

// Cosmetic rewrite as for loop.											// 11/10/99 AM.
for (seq = ana->getSeq(); seq; seq = seq->Right())
	{
	++rulepass_;																// 02/03/05 AM.
	if (!stepExecute(seq, ++currpass_))		// 05/15/99 AM.		// 08/22/02 AM.
		return;																	// 05/15/99 AM.

#ifdef GEODUCK_
	if (gui_.IsStatus(GUI_STATUS_ANALYZING)) {						// 11/18/99 DD.
		s = seq->getData();													// 11/18/99 DD.
		_stprintf(displayBuf,_T("%3d %-10s %s"),								// 11/18/99 DD.
			currpass_,
			s->getAlgoname(),
			s->getRulesfilename()
			);
		gui_.SendMessage(_T("Processing pass:"),displayBuf);			// 11/18/99 DD.
	}

	// Allow other processes.												// 11/10/99 DD.
	PeekHere();																	// 11/10/99 AM.
	if (gui_.IsMessage(GUI_MESSAGE_ABORT))								// 11/10/99 AM.
		break;																	// 11/10/99 AM.
#endif
	}

e_time = clock();																// 10/06/99 AM.

// PRINT OUT THE FINAL PARSE TREE.										// 10/11/99 AM.
// NOTE: NOT INCLUDING IN TIMING.										// 10/11/99 AM.
finExecute(currpass_,s_time,e_time);									// 06/13/00 AM.

#ifdef GEODUCK_
if (gui_.IsMessage(GUI_MESSAGE_ABORT))									// 11/10/99 DD.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Aborted by user]") << std::ends;
	::errOut(&gerrStr,false,0);
	}
#endif
}

/********************************************
* FN:		FINEXECUTE
* CR:		06/13/99 AM.
* SUBJ:	Final printouts for parsing a text.
* NOTE:	Modularity function.
********************************************/

void Parse::finExecute(
	int num,						// Current pass number.
	clock_t s_time,
	clock_t e_time
	)
{
//NLP *nlp = ana->getNLP();													// 08/26/02 AM.

// PRINT OUT THE FINAL PARSE TREE.						// 10/11/99 AM.
// NOTE: NOT INCLUDING IN TIMING.						// 10/11/99 AM.
if (eana_->getFlogfinal())									// 10/13/99 AM.
	finalTree();												// 10/11/99 AM.

if (eana_->getFtimesum())									// 10/13/99 AM.
	{
	double tot;
	double len = getLength();								// 10/06/99 AM.
	double tot1000 = 0;
//	double pass1000 = 0;							// PURIFY COMPLAINT!	// 04/09/01 AM.
	double pass1000 = 0.0;										// FIX.	// 04/09/01 AM.
	_TCHAR *infile = getInput();

	if (!len || !num)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[No text or passes.]") << std::ends;
		::errOut(&gerrStr,false,0,0);
		// 10/11/99 AM.
		return;
		}
	tot = (double)(e_time - s_time)/CLOCKS_PER_SEC;
	tot1000 = tot * (double) 1000.0 / len;
	pass1000 = tot1000 / (double) num;

	if (infile)
		{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Input file: ") << infile << _T("]") << std::ends;
	nlp_->logOut(&gerrStr,false);
	}

	else
		infile = _T("");
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Total passes: ") << num  << _T("]") << std::ends;
	nlp_->logOut(&gerrStr,false);
	}

	{
	std::_t_strstream gerrStr;
	gerrStr	<< _T("[Total time: ") << tot << _T(" sec]") << std::ends;
	nlp_->logOut(&gerrStr,false);
	}

   {
	std::_t_strstream gerrStr;
	gerrStr << _T("[Time per 1000 chars (") << tot << _T("/") << len << _T("): ")
	      << tot1000 << _T(" sec]") << std::ends;
	nlp_->logOut(&gerrStr,false);
	}

	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Time per pass per 1000 chars: ") << pass1000 << _T("]") << std::ends;
	nlp_->logOut(&gerrStr,false);
	}


	// A line for putting into a timing study.	// 10/11/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("infile  passes tottime length tot/1000 tot/pass/1000") << std::ends;
	nlp_->logOut(&gerrStr,false);
	}

	{
	std::_t_strstream gerrStr;
	gerrStr << infile	<< _T("\t")
			<< num		<< _T("\t")
			<< tot		<< _T("\t")
			<< len		<< _T("\t")
			<< tot1000	<< _T("\t")
			<< pass1000	<< std::ends;
	nlp_->logOut(&gerrStr,false);
	}


	}

}

/********************************************
* FN:		STEPEXECUTE
* CR:		05/14/99 AM.
* SUBJ:	Execute one step (pass) in the text analyzer sequence.
* RET:	True if successful, else false.
* NOTE:	The caller must traverse the steps of the analyzer.  This fn
*			merely executes the given step.
********************************************/

bool Parse::stepExecute(
	Delt<Seqn> *seq,			// Current step in sequence.
	int num						// Current pass number.
	)
{
std::_t_ofstream *fout;				// File pass output.						// 12/01/98 AM.
std::_t_ostream *sout;					// For restoring output stream.		// 12/01/98 AM.
clock_t s_time;
_TCHAR *pretname;
Seqn *pass = seq->getData();
this->seq_ = seq;
Algo *algo = pass->getAlgo();
_TCHAR *sfile = pass->getRulesfilename();
_TCHAR *salgo = pass->getAlgoname();
_TCHAR *prefix = _T("ana");		// Prefix for naming files.
bool flogfiles = eana_->getFlogfiles();								// 10/13/99 AM.
bool ftimepass = eana_->getFtimepass();								// 10/13/99 AM.

iniPass(num,prefix,flogfiles,ftimepass,pass->getActive(),sfile,salgo,				// 05/20/00 AM.
			/*DU*/fout,sout,s_time,pretname);

if (pass->getActive() && algo)	// Execute active pass.			// 01/08/99 AM.
	{
	if (!algo->Execute(this, pass)) // Algorithm, do your thing.// 01/26/02 AM.
		return false;															// 01/26/02 AM.
	}
else if (!strcmp_i(str(salgo),_T("folder")))							// 02/03/05 DD.
	;																				// 02/03/05 DD.
else if (!algo && pass->getActive())									// 01/15/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Error. Analyzer pass '")
			<< str(salgo)														// 11/12/00 AM.
			<< _T("' is uninterned.]") << std::ends;
	return ::errOut(&gerrStr,false);
	}

if (pass->getActive())
	finPass(num,flogfiles,fout,sout,pretname,ftimepass,s_time);		// 05/20/00 AM.
return true;
}


/********************************************
* FN:		INIPASS
* CR:		05/20/00 AM.
* SUBJ:	Initial work for one pass of a parse.
********************************************/

#ifndef LINUX
inline
#endif
 void Parse::iniPass(
	int num,					// Current pass number.
	_TCHAR *prefix,			// Prefix for naming dump files.
	bool flogfiles,
	bool ftimepass,
	bool active,
	_TCHAR *sfile,			// Name of rulesfile.
	_TCHAR *salgo,			// Name of algorithm.
	/*DU*/
	std::_t_ofstream* &fout,		// File for outputting pass data.
	std::_t_ostream* &sout,		// For restoring output stream.
	clock_t &s_time,
	_TCHAR* &pretname
	)
{
_TCHAR	fname[MAXSTR*2];

s_time = clock();																// 12/28/98 AM.

currpass_ = num;													// FIX.	// 07/08/03 AM.
rulepass_ = num;																// 02/03/05 AM.

// FETCH PRETTY NAME FOR PASS.
// Should do if timing passes or verbose.								// 10/12/99 AM.
pretname = _T("");																	// 10/09/99 AM.
// If we need a pretty name for pass, get one.						// 10/13/99 AM.
if (flogfiles || ftimepass)												// 10/13/99 AM.
	{
	if (!sfile || !*sfile)
			sfile = _T("NIL");
	if (salgo && *salgo
		 && strcmp_i(salgo, _T("nlp"))
		 && strcmp_i(salgo, _T("rec")))									// 06/13/00 AM.
		pretname = salgo;
	else
		pretname = sfile;
	}

if (flogfiles && active)																	// 10/13/99 AM.
	{
	bool save;																	// 10/13/99 AM.
	save = eana_->getFverbose();											// 10/13/99 AM.
	setVerbose(true);															// 10/13/99 AM.
	eana_->setFverbose(true);												// 10/13/99 AM.

#ifdef UNICODE
wsprintf(fname, _T("%s%c%s%s%d%s"),
		 getOutdir(),
		 DIR_CH,
		 prefix,
		 ((num < 10) ? _T("00") : (num < 100) ? _T("0") : _T("")),
		 num,
		_T(".tree")
		);
#else
#ifdef LINUX
sprintf(fname, _T("%s%c%s%s%d%s"),
#else
sprintf_s(fname, _T("%s%c%s%s%d%s"),
#endif
		 getOutdir(),
		 DIR_CH,
		 prefix,
		 ((num < 10) ? _T("00") : (num < 100) ? _T("0") : _T("")),
		 num,
		_T(".tree")
		);
#endif
	fileOut(fname, /*DU*/ fout, sout);
	prettyPassnum(num, pretname);

	setVerbose(save);															// 10/13/99 AM.
	eana_->setFverbose(save);												// 10/13/99 AM.
	}
}


/********************************************
* FN:		FINPASS
* CR:		05/20/00 AM.
* SUBJ:	Tasks to perform at end of a pass.
* NOTE:	Modularity fn.
********************************************/

 void Parse::finPass(
	int num,							// Pass number.
	bool flogfiles,
	std::_t_ofstream *fout,
	std::_t_ostream *sout,
	_TCHAR *pretname,
	bool ftimepass,
	clock_t &s_time)
{
if (flogfiles)																	// 03/08/00 AM.
	resetOut(fout, sout);
if (ftimepass)																	// 10/13/99 AM.
	{
	clock_t e_time = clock();												// 12/28/98 AM.

// DEBUGGING memleak // 06/19/05 AM.

	std::_t_strstream gerrStr;
	gerrStr	<< _T("[Pass ") << num
			<< _T(" time: ")
			<< (double)(e_time - s_time)/CLOCKS_PER_SEC
			<< _T(" sec     ")
			<< pretname
			<< _T("]") << std::ends;
#ifndef UNICODE
#ifndef LINUX
	gerrStr.freeze(false);	// 07/01/05 AM.
	gerrStr.clear();	// 07/01/05 AM.
#endif
#endif

/*
	std::_t_strstream gerrStr;
	gerrStr	<< _T("[Pass ") << num;
	gerrStr << _T(" time: ");
	gerrStr << (double)(e_time - s_time)/CLOCKS_PER_SEC;
	gerrStr << _T(" sec     ");
	gerrStr << pretname;
	gerrStr << _T("]");
	gerrStr << std::ends;
*/

//	ana->getNLP()->logOut(&gerrStr,false);
	if (nlp_)																	// 06/26/03 AM.
		nlp_->logOut(&gerrStr,false);
	}

}


/********************************************
* FN:		FINALTREE
* CR:		10/11/99 AM.
* SUBJ:	Pretty print final parse tree to a file.
********************************************/

void Parse::finalTree()
{
std::_t_ofstream *fout;				// File for outputting pass data.
std::_t_ostream *sout;					// For restoring output stream.
_TCHAR fname[MAXSTR*2];

// G++ BUGS.	// 03/08/00 AM.
//std::ostringstream *outStr = 0;													// 03/08/00 AM.
//outStr = new std::ostringstream(fname, MAXSTR, std::ios::out);				// 03/08/00 AM.
//*outStr << getOutdir() << "\\final.tree" << std::ends;					// 03/08/00 AM.

#ifdef UNICODE
wsprintf(fname, _T("%s%c%s"), getOutdir(), DIR_CH, _T("final.tree"));
#else
#ifdef LINUX
sprintf(fname, _T("%s%c%s"), getOutdir(), DIR_CH, _T("final.tree"));
#else
sprintf_s(fname, _T("%s%c%s"), getOutdir(), DIR_CH, _T("final.tree"));
#endif
#endif
fileOut(fname, /*DU*/ fout, sout);										// 03/08/00 AM.

// TRYING SOMETHING NEW HERE... // 02/26/05 AM.
// std::_t_strstream outStr;
// outStr << getOutdir() << DIR_CH << _T("final.tree") << std::ends;
// fileOut(&outStr, /*DU*/ fout, sout);

//prettyPassnum(num, pretname);

*gout << _T("\nFINAL OUTPUT TREE:\n") << std::endl;

// Get the parse tree's root node.
Tree<Pn> *tree = (Tree<Pn> *)getTree();		// 03/09/00 AM.
if (!tree)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass: No parse tree.]") << std::ends;
	::errOut(&gerrStr,false,0,0);
	resetOut(/*DU*/ fout, sout);											// 02/21/02 AM.
	return;
	}

Node<Pn> *root;
root = tree->getRoot();
if (!root)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pat pass: Tree has no root.]") << std::ends;
	::errOut(&gerrStr,false,0,0);
	resetOut(/*DU*/ fout, sout);											// 02/21/02 AM.
	return;
	}

tree->Traverse(root, *gout);

resetOut(/*DU*/ fout, sout);
//delete outStr;						// 03/08/00 AM.
//outStr = 0;						// 03/08/00 AM.
}


/********************************************
* FN:		CLEAN
* CR:		10/13/98 AM.
* SUBJ:	Perform cleanups for current parse.
********************************************/

void Parse::clean()
{
if (text)
	{
	//delete [] text;
	Chars::destroy(text);
	text = 0;
	}
if (tree)
	{
	Tree<Pn>::DeleteTreeAndData((Tree<Pn> *)tree);
	tree = 0;
	}
if (lines)
	{
	Tree<Ln>::DeleteTreeAndData((Tree<Ln> *)lines);
	lines = 0;
	}
if (vars_)										// 12/07/98 AM.
	{
//	Var::clean_vars(vars_);	// CLEAN OUT OSTREAMS.					// 05/23/01 AM.
	Dlist<Ipair>::DeleteDlistAndData(vars_);
	vars_ = 0;
	}

// In case cout was rebound, reset it prior to deleting.			// 05/06/02 AM.
// NEED XP FIX.	// 01/12/03 AM.
//if (&cout != &safecout)														// 05/06/02 AM.
//	cout = safecout;		// RESET COUT.									// 05/06/02 AM.

deleteostrs();		// Delete Parse's ostream list.					// 05/24/01 AM.

/// Now cbuf is deletable.													// 02/21/05 AM.
// Commented out. When called from prog/main.cpp, not deletable. // 06/25/05 AM.
//if (cbuf_)																		// 02/21/05 AM.
//	delete cbuf_;																// 02/21/05 AM.
#ifdef _ODBC
if (hstmt_)																		// 05/25/02 AM.
  freeDbstmt();																// 05/25/02 AM.

dbclose();			// Close ODBC handlers.								// 05/23/02 AM.
#endif

deleteblobs();		// Delete allocated blobs.							// 02/27/03 AM.

if (collect_)									// 05/17/00 AM.
	delete [] collect_;
if (nlppp_)
	delete nlppp_;								// 05/17/00 AM.

datum_ = 0;																		// 03/13/03 AM.

logfile_ = 0;	// VTLOG	// 05/06/13 AM.

}


/********************************************
* FN:		PRETTYSEQ
* CR:		10/19/98 AM.
* SUBJ:	Pretty print the analyzer sequence.
********************************************/

void Parse::prettySeq()
{
if (Verbose())
	getAna()->prettySeq();
}


/********************************************
* FN:		PRETTYPASSNUM
* CR:		10/19/98 AM.
* SUBJ:	Pretty print the pass number.
********************************************/

inline void Parse::prettyPassnum(
	int num,
//	Seqn *pass,		// 05/20/00 AM.
	_TCHAR *str		// 10/09/99 AM.
	)
{
if (Verbose())
	{
	*gout << _T("\n")
		  << _T("***************\n")
		  << _T("    PASS ")
		  << num << _T(" (") << str << _T(")")
		  << _T("\n")
		  << _T("***************\n")
		  ;
	}
}

/********************************************
* FN:		PRETTYPARSEDONE
* CR:		10/19/98 AM.
* SUBJ:	Pretty print end of analysis.
********************************************/

inline void Parse::prettyParsedone()
{
if (Verbose())
	{
	*gout << _T("\n")
		  << _T("***************\n")
		  << _T("     DONE\n")
		  << _T("***************\n")
		  ;
	}
}

/********************************************
* FN:		GETSYM
* CR:		11/19/98 AM.
* SUBJ:	Internalize given string in the hash table.
********************************************/

Sym *Parse::getSym(_TCHAR *str)
{
#ifdef OLD_030625_
Htab *htab;
Ana *ana;
ana = getAna();
assert(ana);
htab = ana->getHtab();
assert(htab);
return htab->hsym(str);
#endif

return htab_->hsym(str);	// 06/25/03 AM.
}

// Variant. 03/05/99 AM.
Sym *Parse::getSym(_TCHAR *str, long len)
{
#ifdef OLD_030625_
Htab *htab;
Ana *ana;
ana = getAna();
assert(ana);
htab = ana->getHtab();
assert(htab);
return htab->hsym(str, len);
#endif

//return htab_->hsym(str);	// 06/25/03 AM.	// BUG.	// 02/01/08 AM.
return htab_->hsym(str,len);						// FIX.	// 02/01/08 AM.
}

/********************************************
* FN:		INTERNSTR
* CR:		12/08/98 AM.
* SUBJ:	Internalize given string in the hash table.
* RET:	String in the string table.
********************************************/

_TCHAR *Parse::internStr(_TCHAR *str, /*DU*/ _TCHAR* &tstr)
{
tstr = 0;
Sym *sym = 0;
if ((sym = getSym(str)))
	return (tstr = sym->getStr());
return 0;
}

// Variant. 03/05/99 AM.
_TCHAR *Parse::internStr(_TCHAR *str, long len, /*DU*/ _TCHAR* &tstr)
{
tstr = 0;
Sym *sym = 0;
if ((sym = getSym(str, len)))
	return (tstr = sym->getStr());
return 0;
}

/********************************************
* FN:		INPUT*
* CR:		12/24/99 AM.
* SUBJ:	Get interned pieces of input filename.
* RET:	String in the string table.
********************************************/

_TCHAR *Parse::inputName()
{
_TCHAR *ptr;
// Don't need to copy, for this one.
file_name(input, /*UP*/ ptr);
return ptr;
}

_TCHAR *Parse::inputPath()
{
_TCHAR *ptr, *str;
_TCHAR buf[FNAME_SIZE];
_tcscpy(buf, input);			// Copy input filename for modifying.
file_path(buf, /*UP*/ ptr);
internStr(ptr, /*UP*/ str);	// Install in string table.
return str;
}

_TCHAR *Parse::inputParent()
{
_TCHAR *ptr, *str;
_TCHAR buf[FNAME_SIZE];
_tcscpy(buf, input);			// Copy input filename for modifying.
file_parent(buf, /*UP*/ ptr);
internStr(ptr, /*UP*/ str);	// Install in string table.
return str;
}

_TCHAR *Parse::inputHead()
{
_TCHAR *ptr, *str;
_TCHAR buf[FNAME_SIZE];
_tcscpy(buf, input);			// Copy input filename for modifying.
file_head(buf, /*UP*/ ptr);
internStr(ptr, /*UP*/ str);	// Install in string table.
return str;
}

_TCHAR *Parse::inputTail()
{
_TCHAR *ptr;
// Don't need to copy, for this one.
file_tail(input, /*UP*/ ptr);
return ptr;
}


/********************************************
* FN:		STROUT
* CR:		02/17/01 AM.
* SUBJ:	Output string to user-specified output stream.
* RET:	True if ok, else false.
* NOTE:	To give the user project (user.dll) access to
*			the current parse output stream.
********************************************/

bool Parse::strOut(_TCHAR *str)
{
if (str && *str && out_)
	{
	*out_ << str;
	return true;
	}
return false;
}


/********************************************
* FN:		FLUSHOUT
* CR:		02/17/01 AM.
* SUBJ:	Flush the user-specified output stream.
* RET:	True if ok, else false.
* NOTE:	To give the user project (user.dll) access to
*			the current parse output stream.
********************************************/

bool Parse::flushOut()
{
if (out_)
	{
	*out_ << std::endl;
	return true;
	}
return false;
}


/********************************************
* FN:		NEWOSTR
* CR:		05/23/01 AM.
* SUBJ:	Create a new NLP++ OSTREAM and add to global list.
* RET:	New Delt<Iarg> in list.
* NOTE:	New management of ostreams, within Parse object.
*			Caller should make sure ostream isn't in list already.
*			May want a wrapper for ostream that includes filename, eg,
*			to avoid collisions. (Forget it for the moment.)
********************************************/

Delt<Iarg> *Parse::newostr(std::_t_ostream *ostr)
{
if (!ostrs_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[newostr: Error. Empty list.]") << std::ends;
	::errOut(&gerrStr,false);
	return 0;
	}

Iarg *arg = new Iarg(ostr);
ostrs_->rpush(arg);
return ostrs_->getLast();
}


/********************************************
* FN:		DELETEOSTR
* CR:		05/23/01 AM.
* SUBJ:	Delete an NLP++ ostream from list.
* RET:	true if ok.
* WARN:	Caller should zero out the ostream value.
********************************************/

bool Parse::deleteostr(std::_t_ostream *ostr)
{
if (!ostrs_)
	return false;
if (!ostr)
	return true;

Delt<Iarg> *delt;
Iarg *arg;
for (delt = ostrs_->getFirst(); delt; delt = delt->Right())
	{
	arg = delt->getData();
	if (ostr == arg->getOstream())
		{
		// Delete current delt.
		ostrs_->exciseElt(delt);	// Excise from list.

		// DELETE THE OSTREAM.												// 05/24/01 AM.
		delete ostr;															// 05/24/01 AM.

		// Note: the delete below does not delete the ostream.	// 05/24/01 AM.
		Delt<Iarg>::DeleteDeltAndData(delt);
		break;
		}
	}
return true;
}


/********************************************
* FN:		FINDOSTR
* CR:		05/23/01 AM.
* SUBJ:	Find an NLP++ ostream in global list.
* RET:	Delt if found.
* NOTE:	Need a better way to find ostreams for same file.
********************************************/

Delt<Iarg> *Parse::findostr(std::_t_ostream *ostr)
{
if (!ostrs_)
	return 0;
if (!ostr)
	return 0;

Delt<Iarg> *delt;
Iarg *arg;
for (delt = ostrs_->getFirst(); delt; delt = delt->Right())
	{
	arg = delt->getData();
	if (ostr == arg->getOstream())
		return delt;				// Found it.
	}
return 0;
}


/********************************************
* FN:		DELETEOSTRS
* CR:		05/24/01 AM.
* SUBJ:	Delete the NLP++ ostream list from Parse object.
* RET:	true if ok.
* OPT:	Doing this in a primitive way for efficiency.
********************************************/

bool Parse::deleteostrs()
{
if (!ostrs_)
	return false;

Delt<Iarg> *delt = ostrs_->getFirst();
Iarg *arg;
Delt<Iarg> *dnext;
std::_t_ostream *ostr;
while (delt)
	{
	dnext = delt->Right();	// Save the next guy.

	// Delete the current guy.
	arg = delt->getData();
	ostr = arg->getOstream();
	delete ostr;					// DELETE THE OSTREAM.
	Delt<Iarg>::DeleteDeltAndData(delt);

	delt = dnext;				// Get the next guy.
	}

// Clear out and delete the list.
ostrs_->setFirst(0);
ostrs_->setLast(0);
delete ostrs_;
ostrs_ = 0;
return true;
}

#ifdef _ODBC
/********************************************
* FN:		DBCLOSE
* CR:		05/23/02 AM.
* SUBJ:	Delete the ODBC handles from Parse object.
* NOTE:	Should move all this to an odbc class.
* RET:	true if ok.
********************************************/

bool Parse::dbclose()
{

if (hdbc_)
	{
	// If exists, assume connection is open.

	// Disconnect from the server.
	SQLDisconnect(hdbc_);

	// Close the connection handle.
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
	hdbc_ = 0;
	}

if (henv_)
	{
	// Close the environment handle.
	SQLFreeHandle(SQL_HANDLE_ENV, henv_);
	henv_ = 0;
	}
return true;
}


/********************************************
* FN:		ALLOCDBSTMT
* CR:		05/25/02 AM.
* SUBJ:	Allocate an active ODBC statement handle.
* NOTE:	Should move all this to an db class.
* RET:	true if ok.
********************************************/

bool Parse::allocDbstmt()
{
if (hstmt_)
	{
	// Warn user to free the old one in NLP++.
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning: Need to free old db stmt with dbfreestmt.]") << std::ends;
	::errOut(&gerrStr,false,0,0);

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
	hstmt_ = 0;
	}

SQLRETURN  retcode;
// Allocate statement handle.
retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	_stprintf(Errbuf,_T("[Error: Couldn't allocate handle for db stmt.]"));
	return ::errOut(false); // UNFIXED
	}
return true;
}


/********************************************
* FN:		FREEDBSTMT
* CR:		05/25/02 AM.
* SUBJ:	Delete the active ODBC statement handle.
* NOTE:	Should move all this to an db class.
* RET:	true if ok.
********************************************/

bool Parse::freeDbstmt()
{
if (hstmt_)
	{
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
	hstmt_ = 0;
	}
return true;
}
#endif

/********************************************
* FN:		ERROUT
* CR:		08/24/02 AM.
* SUBJ:	Print out an error message and return true/false.
* RET:	Return given boolean value.
* NOTE:	Utility function for printing error message.
*			Parser-specific printout of pass and line number.
*			Still keeping a global variant in global.cpp.
********************************************/

bool Parse::errOut(
	bool retval,
	bool inputerr	// E,g., if error is in file being interned.
	)
{

if (!inputerr
 || !inputpass_)	// 09/21/13 AM.
	return ::errOut(retval,currpass_,line_);
else
	return ::errOut(retval,inputpass_,inputline_);
}


// VARIANT.	For Unicode, and to remove global buffer.				// 02/25/05 AM.
bool Parse::errOut(
	std::_t_strstream *st,			// Error message to print.
	bool retval,
	bool inputerr	// E,g., if error is in file being interned.
	)
{
if (logfile_)	// VTLOG	// 05/06/13 AM.
	{
	// Todo: get input file name, text near error.
	std::_t_ofstream logf(TCHAR2CA(logfile_), std::ios::app);	// VTLOG	// 05/06/13 AM.
	logf << _T("[") << currpass_ << _T(",") << line_ << _T("] ")
		<< st->str() << std::endl;	// VTLOG	// 05/06/13 AM.
	logf << _T("  TEXT: ") << getText() << std::endl;	// VTLOG	// 05/06/13 AM.
	}

if (!inputerr
 || !inputpass_)	// 09/21/13 AM.
	return ::errOut(st,retval,currpass_,line_);
else
	return ::errOut(st,retval,inputpass_,inputline_);
}

/********************************************
* FN:		NEWBLOB
* CR:		02/27/03 AM.
* SUBJ:	Create a new blob and add to global list.
* RET:	New Delt<Iarg> in list.
********************************************/

Delt<Iarg> *Parse::newblob(long siz)
{
if (!blobs_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[newblob: Error. Empty list.]") << std::ends;
	::errOut(&gerrStr,false);
	return 0;
	}

_TCHAR * buf = Chars::create(siz);
#ifdef LINUX
memset(buf,0,siz);	// 10/26/06 AM.
#else
ZeroMemory(buf,siz);	// Zero out.	// 03/12/03 AM.
#endif
Iarg *arg = new Iarg(buf);
blobs_->rpush(arg);
return blobs_->getLast();
}


/********************************************
* FN:		DELETEBLOBS
* CR:		02/27/03 AM.
* SUBJ:	Delete the NLP++ blob list from Parse object.
* RET:	true if ok.
* OPT:	Doing this in a primitive way for efficiency.
********************************************/

bool Parse::deleteblobs()
{
if (!blobs_)
	return false;

Delt<Iarg> *delt = blobs_->getFirst();
Iarg *arg;
Delt<Iarg> *dnext;
_TCHAR *buf;
while (delt)
	{
	dnext = delt->Right();	// Save the next guy.

	// Delete the current guy.
	arg = delt->getData();
	buf = arg->getStr();
	Chars::destroy(buf);
	Delt<Iarg>::DeleteDeltAndData(delt);

	delt = dnext;				// Get the next guy.
	}

// Clear out and delete the list.
blobs_->setFirst(0);
blobs_->setLast(0);
delete blobs_;
blobs_ = 0;
return true;
}

/*************************      END OF FILE      ******************************/
