/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CODE.CPP
* FILE:	lite\code.cpp
* CR:		12/07/98 AM.
* SUBJ:	Code actions for Pat pass.
* NOTE:	This is an embryonic Lite programming language that the user
*			can write as part of text analysis.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"		// 03/09/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "lite/Auser.h"	// 07/07/03 AM.
#include "lite/iarg.h"	// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "string.h"
#include "tree.h"							// 02/26/01 AM.
#include "lite/nlppp.h"		// 01/14/00 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "arg.h"
#include "parse.h"
#include "iaction.h"
#include "ifile.h"
#include "seqn.h"
#include "var.h"
#include "dyn.h"			// 01/29/99 AM.
#include "ana.h"			// 12/08/99 AM.
//#include "lite/nlppp.h"		// 01/14/00 AM.
#include "rfasem.h"		// 01/14/00 AM.

#include "lite/code.h"

int Code::count_ = 0;

// For users to be able to register a Code action class containing
// their code actions.
//static Code *usercode_ = 0;												// 02/09/01 AM.

/********************************************
* FN:		Special functions for the class
********************************************/

Code::Code()
{
#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

/*******************************************/

Code::~Code()
{

#ifndef STABLE_
--count_;
#endif
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
* FN:		GETCOUNT
* CR:		12/08/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Code::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		12/08/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Code::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Code count=") << count_ << std::endl;
	*gout << _T("Active Code count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Code count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

/********************************************
* FN:		EXECCODE
* CR:		12/07/98 AM.
* SUBJ:	Execute code actions for Pat pass.
********************************************/

void Code::execCode(Parse *parse, Seqn *seqn, bool &exitpass)
{
bool ok = true;																// 01/14/00 AM.
exitpass = false;																// 02/06/01 AM.

if (!parse || !seqn)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[execCode: given no parser or pass information.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return;
	}

// Get the rulesfile object.
Ifile *rulesfile = 0;
if (!(rulesfile = seqn->getRulesfile()))
	{
	// Ok for the RFA passes not to have a pass file.
	//*gerr << "[execCode: no pass file.]" << std::endl;
	return;
	}

// Get the code actions list.
Dlist<Iaction> *codes = 0;
if (!(codes = rulesfile->getCodes()))
	{
	//cout << "[No code actions.]" << std::endl;
	return;
	}

// Set up for executing NLP++ code.									// 01/14/00 AM.
// (Opt: Could move the nlppp object to an outer loop.)
// Dummy up a "parse state" object even though no parse here.
Nlppp nlppp;					// Empty Nlppp object.				// 01/14/00 AM.
nlppp.parse_ = parse;													// 01/14/00 AM.
nlppp.region_ = REGCODE;	// Execute NLP++ in code region.	// 01/14/00 AM.

// Handle L() local vars in @CODE region!							// 11/19/02 AM.
void *dummy = 0;															// 01/08/07 AM.
Arun::fnstart(&nlppp,dummy,dummy);	// 11/19/02 AM.			// 01/08/07 AM.

// Traverse the list, performing actions in turn.
Delt<Iaction> *daction = 0;
Iaction *action = 0;
RFASem *rfasem, *val = 0;
bool badname = false;			// If unknown action name.		// 01/14/00 AM.
for (daction = codes->getFirst(); daction; daction = daction->Right())
	{
	action = daction->getData();

	// Execute NLP++ code.												// 01/14/00 AM.
	if ((rfasem = action->getSem()))									// 01/14/00 AM.
		{
		//*gerr << "[Code: Statements to execute.]" << std::endl;

		if (!(ok = rfasem->eval(&nlppp, /*UP*/ val)))
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Code actions: Error in NLP++.]") << std::ends;
			parse->errOut(&gerrStr,false);
			}

		if (val)
			delete val;			// Not using value of semantics at top level.
		if (!ok)
			{
			exitpass = nlppp.exitpass_;									// 02/06/01 AM.
			Arun::ret(&nlppp,0,0,(RFASem*)0);// Handle local vars!	// 11/19/02 AM.
			return;
			}
		//continue;															// 10/24/00 AM.
		}

	else if (!Code::codeAction(action, parse, /*UP*/ badname))
		{
		if (badname)														// 01/14/00 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Code actions: Error. Unknown action=")
							<< action->getName() << _T("].")
							<< std::ends;
			parse->errOut(&gerrStr,false);
			}

		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Post actions: Error in action=")
							<< action->getName()
							<< _T("].") << std::ends;
			parse->errOut(&gerrStr,false);
			}

		exitpass = nlppp.exitpass_;										// 02/06/01 AM.
		Arun::ret(&nlppp,0,0,(RFASem*)0);	// Handle local vars!	// 11/19/02 AM.
		return;	// false.												// 01/14/00 AM.
		}

	if (nlppp.succeed_)														// 10/23/00 AM.
		break;																	// 10/23/00 AM.
	}

Arun::ret(&nlppp,0,0,(RFASem*)0);	// Handle local vars!			// 11/19/02 AM.
exitpass = nlppp.exitpass_;												// 02/06/01 AM.
}


/********************************************
* FN:		CODEACTION
* CR:		12/07/98 AM.
* SUBJ:	Execute code action for Pat pass.
* NOTE:	Variable names are case-sensitive.
* OPT:	Internalizing should convert action strings into a table-
*			driven scheme.
********************************************/

bool Code::codeAction(Iaction *action, Parse *parse,
	/*UP*/
	bool &badname															// 01/14/00 AM.
	)
{
badname = false;
if (!action || !parse)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[codeAction: given null action or parse object.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}

// Get the action name.  If it's known, do it with the args.
_TCHAR *str = 0;
Dlist<Iarg> *list = 0;
Delt<Iarg> *args = 0;
str = action->getName();
list = action->getArgs();
if (list)									// FIX.						// 02/22/00 AM.
	args = list->getFirst();
if (empty(str))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[codeAction: no action name.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

//cout << "str=" << str << std::endl;
if (!_tcscmp(str, _T("var")))			// Variable creation action.
	return codeVar(args, parse);
else if (!_tcscmp(str, _T("varstrs")))									// 01/24/99 AM.
	return codeVarstrs(args, parse);
else if (!_tcscmp(str, _T("sortvals")))									// 01/25/99 AM.
	return codeSortvals(args, parse);
else if (!_tcscmp(str, _T("lookup")))										// 01/25/99 AM.
	return codeLookup(args, parse);
else if (!_tcscmp(str, _T("print")))
	return codePrint(args, parse);
else if (!_tcscmp(str, _T("printvar")))
	return codePrintvar(args, parse);
else if (!_tcscmp(str, _T("fprintvar")))
	return codeFprintvar(args, parse);	// 03/23/99 AM.
else if (!_tcscmp(str, _T("startout")))	// To the user-supplied file name.
	return codeStartout(args, parse);
else if (!_tcscmp(str, _T("fileout")))	// To a file specified in the action.
	return codeFileout(args, parse);
else if (!_tcscmp(str, _T("prlit")))	// To a file specified in the action.
	return codePrlit(args, parse);
else if (!_tcscmp(str, _T("stopout")))
	return codeStopout(args, parse);

else if (!strcmp_i(str, _T("gdump")))										// 12/13/99 AM.
	return codeGdump(args, parse);									// 12/13/99 AM.
else if (!strcmp_i(str, _T("gtolower")))									// 12/15/99 AM.
	return codeGtolower(args, parse);								// 12/15/99 AM.
else if (!strcmp_i(str, _T("guniq")))										// 12/15/99 AM.
	return codeGuniq(args, parse);									// 12/15/99 AM.

//else if (!userCode(str, args, parse))								// 02/09/01 AM.
	{
	// ASSUME BAD NAME, FOR NOW.										// 01/14/00 AM.
	badname = true;														// 01/14/00 AM.
	return false;		// Unknown or botched user code action.
	}

return true;	// Linux merge.  Should keep as is.				// 03/24/00 AM.
}

#ifdef OLD_
/********************************************
* FN:		USERCODE
* CR:		12/08/98 AM.
* SUBJ:	Execute user-defined code actions, if any.
* RET:	True if successful match, else false.
********************************************/

bool Code::userCode(_TCHAR *func, Delt<Iarg> *args, Auser *auser)
{
#ifndef LINUX

// Do the ucodeAction(func, args, parse) thing here.	// 01/29/99 AM.
HINSTANCE hdll = parse->getHdll();

//if (hdll && call_ucodeAction(hdll, func,args,parse))			// 01/29/99 AM.
if (hdll && call_ucodeAction(hdll, func,args,auser))				// 08/28/02 AM.
	return true;

//*gerr << "[Execute code action: Unknown action=" << func << "]." << std::endl;
#endif
return false;
}
#endif


/********************************************
* FN:		REGCODE
* CR:		12/08/98 AM.
* SUBJ:	Register the user-defined code-action class.
* NOTE:	Class function.
* ALLOC:	User creates the code-action class to add.
*			Expected to be a derived class of Code.
********************************************/

bool Code::regCode(
	Code *code				// Algorithm class to register.
	)
{
//if (usercode_)																// 02/09/01 AM.
//	{
//	*gerr << "[regCode: Can only register one code action class.]" << std::endl;
//	return false;
//	}
//usercode_ = code;															// 02/09/01 AM.
return true;
}

/********************************************
* FN:		CLEAN
* CR:		12/08/98 AM.
* SUBJ:	Cleanups for code class.
* NOTE:	Class function.
* ALLOC:	Deletes the user-registered code action class.
********************************************/

void Code::clean()
{
//if (usercode_)																// 02/09/01 AM.
//	{
//	delete usercode_;
//	usercode_ = 0;
//	}
}


/********************************************
* FN:		CODEVAR
* CR:		12/07/98 AM.
* SUBJ:	Declare and set the value of a variable.
********************************************/

bool Code::codeVar(Delt<Iarg> *args, Parse *parse)
{
_TCHAR *name;			// Variable name.
Iarg *arg;			// Variable value.

// Should have two args: variable name and value to set.
if (!Arg::str1(_T("codeVar"), /*DU*/ ((DELTS*&)args), name))
	return false;
if (!Arg::arg1(_T("codeVar"), /*DU*/ ((DELTS*&)args), arg))
	return false;
if (!Arg::done((DELTS*)args, _T("codeVar"),parse))
	return false;

// Install the variable in a list within the parse object.
// The variable is known globally within the current parse instance.
// REUSE Ipair!  Still need a var class to manage all this.
return Var::create(name, arg, parse);						// 01/14/00 AM.
}


/********************************************
* FN:		CODEVARSTRS
* CR:		01/24/98 AM.
* SUBJ:	Declare and set the value of a strings variable.
* NOTE:	Value of this variable is a list of strings.
********************************************/

bool Code::codeVarstrs(Delt<Iarg> *args, Parse *parse)
{
_TCHAR *name;			// Variable name.

// Should have two args: variable name and value to set.
if (!Arg::str1(_T("codeVarstrs"), /*DU*/ ((DELTS*&)args), name))
	return false;
//if (!Arg::done((DELTS*)args, "codeVarstrs"))
	//return false;

// Install the variable in a list within the parse object.
// The variable is known globally within the current parse instance.
// REUSE Ipair!  Still need a var class to manage all this.
return Var::createstrs(name, parse);							// 01/14/00 AM.
}


/********************************************
* FN:		CODESORTVALS
* CR:		01/25/99 AM.
* SUBJ:	Sort a variable's values.
* RET:	True if ok, else false.
* NOTE:	Prepping for word lookup.
********************************************/

bool Code::codeSortvals(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *var = 0;

if (!Arg::str1(_T("codeSortvals"), ((DELTS*&)args), var))
	return false;
if (!Arg::done((DELTS*)args, _T("codeSortvals"),parse))
	return false;

return Var::sortVals(var, parse);
}


/********************************************
* FN:		CODEGTOLOWER
* CR:		12/15/99 AM.
* SUBJ:	Convert variable values to lowercase.
* RET:	True if ok, else false.
* NOTE:	To support case-insensitive lookup.
* FORM:	Gtolower(VAR)
*				VAR - global var has a list of strings as values.
********************************************/

bool Code::codeGtolower(
	Delt<Iarg> *args,
	Parse *parse
	)
{
_TCHAR *var = 0;

if (!Arg::str1(_T("Gtolower"), ((DELTS*&)args), var))
	return false;
if (!Arg::done((DELTS*)args, _T("Gtolower"),parse))
	return false;

return Var::tolowerVals(var, parse);
}


/********************************************
* FN:		CODEGUNIQ
* CR:		12/15/99 AM.
* SUBJ:	Remove redundant strings in sorted list of values.
* RET:	True if ok, else false.
* NOTE:	To support case-insensitive lookup.
* FORM:	Guniq(VAR)
*				VAR - global var has SORTED list of strings as values.
********************************************/

bool Code::codeGuniq(
	Delt<Iarg> *args,
	Parse *parse
	)
{
_TCHAR *var = 0;

if (!Arg::str1(_T("Guniq"), ((DELTS*&)args), var))
	return false;
if (!Arg::done((DELTS*)args, _T("Guniq"),parse))
	return false;

return Var::uniqVals(var, parse);
}


/********************************************
* FN:		CODELOOKUP
* CR:		01/25/99 AM.
* SUBJ:	Lookup variable's strings in the given file. Update sym's flag.
* RET:	True if ok, else false.
* NOTE:	06/04/00 AM. Pulled internals to separate variant.
* FORM:	lookup(VAR, FILE, FLAG)
*			eg, lookup("Words", "doom.words", "word")
*			VAR - has a list of strings as values.
*			FILE - a file of strings, one per line, in appdir/data/FILE
*			FLAG - eg, "word", tells which bit-flag of hashed sym to update.
********************************************/

bool Code::codeLookup(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *var = 0;
_TCHAR *fname = 0;
_TCHAR *flag = 0;

if (!Arg::str1(_T("codeLookup"), ((DELTS*&)args), var))
	return false;
if (!Arg::str1(_T("codeLookup"), ((DELTS*&)args), fname))
	return false;
if (!Arg::str1(_T("codeLookup"), ((DELTS*&)args), flag))
	return false;
if (!Arg::done((DELTS*)args, _T("codeLookup"),parse))
	return false;

return codeLookup(var,fname,flag,parse);
}


/********************************************
* FN:		CODELOOKUP
* CR:		06/04/00 AM.
* SUBJ:	Lookup variable's strings in the given file. Update sym's flag.
* RET:	True if ok, else false.
* NOTE:	Will do case-insensitive string comparison.
*			Redundancy is checked automatically.
*			Assume file is in sorted order and has unique LOWERCASE lines.
*			Hook - may want a delimiter char on the file line.
*			06/04/00 AM. Supporting compiled runtime.  Pulled internals into
*			here.
* FORM:	lookup(VAR, FILE, FLAG)
*			eg, lookup("Words", "doom.words", "word")
*			VAR - has a list of strings as values.
*			FILE - a file of strings, one per line, in appdir/data/FILE
*			FLAG - eg, "word", tells which bit-flag of hashed sym to update.
********************************************/

bool Code::codeLookup(
	_TCHAR *var,
	_TCHAR *fname,
	_TCHAR *flag,
	Parse *parse
	)
{
if (!var || !*var || !fname || !*fname || !flag || !*flag || !parse)
	return false;

_TCHAR *str = 0;

// Get the variable's values.
Delt<Iarg> *values = 0;
if (!Var::vals(var, parse, /*UP*/ values))
	return false;
if (!values)
	return true;	// No values is ok!	// FIX.						// 12/11/99 AM.

// Open the file for input.
// NOTE: LINUX vs WINDOWS FILE PATHS.	// 03/20/19 AM.
// GETTING FROM ANALYZER'S DATA DIRECTORY, POSSIBLY THE SHARED TAI
// DATA AREA DEFINED BY %TAI% environment variable.				// 12/08/99 AM.
_TCHAR buf[MAXSTR];
_stprintf(buf, _T("%s%c%s"), parse->getAna()->getDatadir(),		// 12/08/99 AM.
													DIR_CH,						// 03/09/00 AM.
													fname);						// 12/08/99 AM.
//ifstream inFile(buf, std::ios::in | std::ios::nocreate);
std::_t_ifstream inFile(TCHAR2CA(buf), std::ios::in);							// Upgrade.	// 01/24/01 AM.
if (!inFile)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("Could not open input file '") << buf << _T("'.") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

// Traverse list and file.
// If match, lookup the associated sym and update it.
_TCHAR line[MAXSTR];
//char lcbuf[MAXSTR];
if (!inFile.eof())
	inFile.getline(line, MAXSTR);
else
	return true;
str = Var::getStr(values);
int cmp;
Sym *sym = 0;
for (;;)					// Traverse file and list.
	{
	if ((cmp = strcmp_i(str,line)) < 0)	// List is alphabetically less.
		{
		// Word not in file.  Set looked up flag.
		sym = parse->getSym(str);
		if ((sym = sym->getLcsym()))		// Get lowercased sym.
			sym->setLooked(true);		// It's been looked up.
		else
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[String='") << str << _T("' has no lowercase sym.]") << std::ends;
			parse->errOut(&gerrStr,false);
			}

		if (!(values = values->Right()))
			return true;
		str = Var::getStr(values);
		}
	else if (cmp > 0)							// File is alphabetically less.
		{
		// Get next file entry.
		if (!inFile.eof())
			inFile.getline(line, MAXSTR);
		else
			return true;
		}
	else											// Match
		{
		//cout << "[Found match=" << str << "]" << std::endl;
		// Get and update sym.
		// 01/26/99 AM. Use line, since it is lowercase.
		//str_to_lower(str, lcbuf);

		sym = parse->getSym(line);	// Sym for lowercased word.
		sym->setLooked(true);		// It's been looked up.
		sym->setKnown(true);			// It's a known thingy.
		if (!strcmp_i(_T("word"), flag))
			sym->setWord(true);			// It's an English word.

		// Advance both.
		if (!(values = values->Right()))
			return true;
		str = Var::getStr(values);
		if (!inFile.eof())
			inFile.getline(line, MAXSTR);
		else
			return true;
		}
	}
return true;
}


/********************************************
* FN:		CODEPRINT
* CR:		12/08/98 AM.
* SUBJ:	Print a literal string.
* RET:	True if ok, else false.
* NOTE:	Should direct output to output file, if any.
*			For synchronization of all outputs with dlls, threads, etc., do an
*			std::endl or std::flush after every print out.
********************************************/

bool Code::codePrint(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *str = 0;

if (!Arg::str1(_T("codePrint"), ((DELTS*&)args), str))
	return false;
if (!Arg::done((DELTS*)args, _T("codePrint"),parse))
	return false;

// Use main output file, if open, else standard output.
std::_t_ostream *fout = 0;			// "Real" output stream.
if (!(fout = Code::getOut(parse)))		// 03/23/99 AM
	fout = &std::_t_cout;								// 03/23/99 AM
	// Want to use gout here, but doesn't work.  check it out.

// Print the string
*fout << str;		// 03/23/99 AM.
*fout << std::flush;	// 03/23/99 AM.

return true;
}


/********************************************
* FN:		CODEPRINTVAR
* CR:		12/08/98 AM.
* SUBJ:	Print a variable's value.
* RET:	True if ok, else false.
* NOTE:	Should direct output to output file, if any.
********************************************/

bool Code::codePrintvar(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *var = 0;

if (!Arg::str1(_T("codePrintvar"), ((DELTS*&)args), var))
	return false;
if (!Arg::done((DELTS*)args, _T("codePrintvar"),parse))
	return false;

_TCHAR *st = 0;

#ifdef OLD_
// Fetch the variable's value.
if (!Var::val(var, parse, /*DU*/ st))
	return false;

// Print the string
*gout << str(st);		// 03/23/99 AM.
*gout << std::flush;		// 03/23/99 AM.
#endif

// 01/25/99 AM.
// Fetch a handle for the variable values.
Delt<Iarg> *values = 0;
if (!Var::vals(var, parse, /*UP*/ values))
	return false;

// Use main output file, if open, else standard output.
std::_t_ostream *fout = 0;			// "Real" output stream.
if (!(fout = Code::getOut(parse)))		// 03/23/99 AM
	fout = &std::_t_cout;								// 03/23/99 AM
// Not sure why gout isn't working here.

// Print out all values.
// (Handling multiple values of variable).
bool first = true;
Iarg *arg;																	// 12/03/99 AM.
while (values)
	{
	// Get string and print it.
	//st = Var::getStr(values);										// 12/03/99 AM.
	if (!first)
		{
		*fout << _T(" ");														// 03/23/99 AM.
		}
	else
		first = false;
	arg = values->getData();											// 12/03/99 AM.
	switch (arg->getType())												// 12/03/99 AM.
		{
		case IASTR:															// 12/03/99 AM.
			st = arg->getStr();											// 12/03/99 AM.
			*fout << str(st) << std::flush;									// 03/23/99 AM.
			break;
		case IANUM:															// 12/03/99 AM.
			*fout << arg->getNum() << std::flush;							// 12/03/99 AM.
			break;
		default:
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[codePrintvar: Bad value]") << std::ends;
			parse->errOut(&gerrStr,false);
			}
		// 12/03/99 AM.
			return false;
		}
	values = values->Right();
	}
return true;
}


/********************************************
* FN:		CODEFPRINTVAR
* CR:		03/23/99 AM.
* SUBJ:	Print a variable's value to file.
* RET:	True if ok, else false.
* NOTE:	Adding a separator string.
********************************************/

bool Code::codeFprintvar(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *var = 0;
_TCHAR *fname = 0;
_TCHAR *sep = 0;

// Get user specified filename.  Taking name as is.
// Name shouldn't have a path in it, for now.
// eg, expecting something like "companies.txt", to list
// company names in $appdir\output\companies.txt.
if (!Arg::str1(_T("codeFprintvar"), /*DU*/ ((DELTS*&)args), fname))
	return false;
if (!Arg::str1(_T("codeFprintvar"), /*DU*/ ((DELTS*&)args), var))
	return false;

// Allowing this one to be optional.								// 03/25/99 AM.
if (args)																	// 12/03/99 AM.
	Arg::str1(_T("codeFprintvar"), ((DELTS*&)args), sep);							// 03/25/99 AM.

if (!sep || !*sep)		// 03/25/99 AM.
	sep = _T("\n");				// A default separator.		// 03/25/99 AM.
// 03/25/99 AM.	INCOMPLETE!  NOT HANDLING SEPARATOR YET.

if (!Arg::done((DELTS*)args, _T("codeFprintvar"),parse))
	return false;

_TCHAR *st = 0;

// 01/25/99 AM.
// Fetch a handle for the variable values.
Delt<Iarg> *values = 0;
if (!Var::vals(var, parse, /*UP*/ values))
	return false;

std::_t_ostream *ostr = 0;
if (!Var::val(fname, parse, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Fprintvar code action: file=") << fname << _T(" is closed.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

// Print out all values.
// (Handling multiple values of variable).
bool first = true;
Iarg *arg;																	// 12/03/99 AM.
while (values)
	{
	// Get string and print it.
	//st = Var::getStr(values);										// 12/03/99 AM.
	if (!first)
		{
		*ostr << _T(" ");														// 04/09/99 AM.
//		*ostr << sep;														// 04/09/99 AM.
		}
	else
		first = false;
	arg = values->getData();											// 12/03/99 AM.
	switch (arg->getType())												// 12/03/99 AM.
		{
		case IASTR:															// 12/03/99 AM.
			st = arg->getStr();											// 12/03/99 AM.
			*ostr << str(st) << std::flush;
			break;
		case IANUM:															// 12/03/99 AM.
			*ostr << arg->getNum() << std::flush;							// 12/03/99 AM.
			break;
		default:
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[codeFprintvar: Bad value]") << std::ends;
			parse->errOut(&gerrStr,false);
			}
		// 12/03/99 AM.
			return false;
		}
	values = values->Right();
	}
return true;
}


/********************************************
* FN:		CODEGDUMP
* CR:		12/13/99 AM.
* SUBJ:	Dump all global variables to file.
* RET:	True if ok, else false.
* FORM:	gdump(filename).
* NOTE:	File must be opened with FILEOUT action.
********************************************/

bool Code::codeGdump(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *fname = 0;

// Get user specified filename.  Taking name as is.
// Name shouldn't have a path in it, for now.
// eg, expecting something like "companies.txt", to list
// company names in $appdir\output\companies.txt.
if (!Arg::str1(_T("Gdump"), /*DU*/ ((DELTS*&)args), fname))
	return false;

if (!Arg::done((DELTS*)args, _T("Gdump"),parse))
	return false;

_TCHAR *st = 0;

// Fetch a handle for the variable values.
Dlist<Ipair> *vars = parse->getVars();
if (!vars)
	return true;			// No variables is ok!

std::_t_ostream *ostr = 0;
if (!Var::val(fname, parse, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Gdump code action: file=") << fname << _T(" is closed.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

return Var::dump(vars, ostr);
}

/********************************************
* FN:		CODESTARTOUT
* CR:		01/07/99 AM.
* SUBJ:	Start the output to the user-supplied output file.
* RET:	True if ok, else false.
********************************************/

bool Code::codeStartout(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *str = 0;

//if (!Arg::done((DELTS*)args, "codeStartout"))
	{
	// Let it go.
	//return false;
	}

if (parse->getOut())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Startout code action: Output file already open.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

// Open the user-supplied output file.
// If none, then use standard output.
_TCHAR *name = 0;
if ((name = parse->getOutput()))
	{
	// Open the output file for APPENDING.
	std::_t_ofstream *fout = 0;
	fout = new std::_t_ofstream(TCHAR2CA(name), std::ios::app);
	parse->setOut(fout);
	parse->newostr(fout);			// Add to list of ostrs.		// 05/23/01 AM.
	}
else	// No output file.  Use standard out by default.
	{
	parse->setOut(&std::_t_cout);
	}

return true;
}

/********************************************
* FN:		CODESTOPOUT
* CR:		01/07/99 AM.
* SUBJ:	Stop the output to the user-supplied output file.
* RET:	True if ok, else false.
* NOTE:	For now, each 
********************************************/

bool Code::codeStopout(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *str = 0;

//if (!Arg::done((DELTS*)args, "codeStopout"))
	{
	// Let it go.
	//return false;
	}


// Close the output file.
std::_t_ostream *tmp = parse->getOut();											// 05/23/01 AM.
if (!tmp)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Stopout code action: No output file is open.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}
else if (tmp == &std::_t_cout)		// Program's "real" output is to standard output.
	;
else
	{
	// Close the output file.
//	delete tmp;																	// 05/23/01 AM.
	parse->deleteostr(tmp);		// Rm from list and delete.		// 05/23/01 AM.
	}

parse->setOut(0);				// Reset the output handle.

return true;
}


/********************************************
* FN:		GETOUT
* CR:		01/07/99 AM.
* SUBJ:	Get the handle for user-supplied output file.
* RET:	True if ok, else false.
* NOTE:	Function for hiding internals from user.
********************************************/

std::_t_ostream *Code::getOut(
	Parse *parse
	)
{
return parse->getOut();
}


/********************************************
* FN:		CODEFILEOUT
* CR:		02/08/99 AM.
* SUBJ:	Start the output to an output file specified in this action.
* RET:	True if ok, else false.
* NOTE:	Create a variable whose name is the file name and value is
*			the output stream.  A quick solution.
*			If file is closed, the ostream ptr will be NIL.
* OPT:	Some redundancy in fetching the variable here.
********************************************/

bool Code::codeFileout(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *fname = 0;

// Get user specified filename.  Taking name as is.
// Name shouldn't have a path in it, for now.
// eg, expecting something like "companies.txt", to list
// company names in $appdir\output\companies.txt.
if (!Arg::str1(_T("codeFileout"), ((DELTS*&)args), fname))
	return false;
if (!Arg::done((DELTS*)args, _T("codeFileout"),parse))
	return false;

if (!fname)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[fileout: Error. Given null filename.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

Ipair *pair;
std::_t_ostream *ostr;
bool exists,open,inuse,badname;
if (!Var::filevar(fname,parse,										// 12/31/99 AM.
									/*UP*/ pair,ostr,exists,open,inuse,badname))
	{
	if (inuse)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[fileout: Error. var=") << fname << _T(" already in use.]")
					<< std::ends;
		parse->errOut(&gerrStr,false);
		}

	else if (badname)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[fileout: Error. var=") << fname << _T(" is bad name.]")
					<< std::ends;
		parse->errOut(&gerrStr,false);
		}

	else
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[fileout: Error creating output var=")
					<< fname << _T("]") << std::ends;
		parse->errOut(&gerrStr,false);
		}

	return false;
	}

if (exists)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[fileout: Warning. output var=")
			<< fname << _T(" exists.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return true;
	}
return true;
}



/********************************************
* FN:		CODEPRLIT
* CR:		02/09/99 AM.
* SUBJ:	Print literal string to file.
* RET:	True if ok, else false.
* FORM:	prlit("filename", "literal string")
* NOTE:	File name doubles as variable name.
********************************************/

bool Code::codePrlit(
	Delt<Iarg> *args,			// Argument list for the action.
	Parse *parse
	)
{
_TCHAR *fname = 0;
_TCHAR *lit = 0;

// Get user specified filename.  Taking name as is.
// Name shouldn't have a path in it, for now.
// eg, expecting something like "companies.txt", to list
// company names in $appdir\output\companies.txt.
if (!Arg::str1(_T("codePrlit"), ((DELTS*&)args), fname))
	return false;
if (!Arg::str1(_T("codePrlit"), ((DELTS*&)args), lit))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Missing literal arg in Prlit code action.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}
if (!Arg::done((DELTS*)args, _T("codePrlit"),parse))
	return false;

std::_t_ostream *ostr = 0;
if (!Var::val(fname, parse, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prlit code action: file=") << fname << _T(" is closed.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

// Output the string to the file.
if (lit && *lit)
	*ostr << lit << std::flush;

return true;
}


/*************************      END OF FILE      ******************************/
