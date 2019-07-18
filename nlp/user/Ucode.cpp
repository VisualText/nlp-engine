/*******************************************************************************
Copyright (c) 1998-2003 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:   UCODE.CPP
* FILE:   User\ucode.cpp
* CR:     12/08/98 AM.
* SUBJ:   User-defined C++ functions.
* NOTE:   Functions defined here can extend NLP++ with user code or 3rd party
*         code.  From an analyzer, invoke an analyzer with a call such as
*               user::analyzefile().
*
*******************************************************************************/

#ifdef LINUX
#include <locale.h>
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>
#else
#include <windows.h>				// 07/07/03 AM.
#endif

#include "machine.h"

//#include <math.h>

//#include <tchar.h>
//#include <my_tchar.h>
//#include <streamClasses.h>

#include "vtapi.h"				 // 04/05/05 AM.

#include "prim/libprim.h"      // 08/25/02 AM.
#include "prim/list_s.h"       // 08/25/02 AM.
#include "prim/list.h"         // 08/25/02 AM.
#include "lite/lite.h"
#include "lite/global.h"
#include "lite/delt.h"         // 07/07/03 AM.
#include "lite/dlist.h"        // 07/07/03 AM.
#include "lite/dir.h"          // 08/25/02 AM.
#include "lite/iarg.h"
#include "lite/node.h"
#include "lite/tree.h"
#include "lite/pn.h"
#include "lite/Avar.h"
//#include "lite/nlppp.h"
#include "lite/Auser.h"
#include "lite/parse.h"
#include "lite/ana.h"
//#include "lite/pn.h"
#include "lite/rfasem.h"
#include "lite/nlp.h"
#include "lite/vtrun.h"        // 08/28/02 AM.
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "lite/arg.h"          // 08/29/02 AM.
#include "user.h"
#include "Umisc.h"             // 12/03/03 AM.
#include "Ucode.h"

#define OBUFSIZ   ((long)65536 * (long)8)

/********************************************
* FN:      Special functions for the class
********************************************/

user::user()
{
//cout << "[Creating user class.]" << endl;
}

user::~user()
{
}

/*******************************************/

/*******************************************/


/*******************************************/

/*******************************************/


/********************************************
* FN:      Access Functions
********************************************/

/********************************************
* FN:      Modify Functions
********************************************/


/********************************************
* FN:    EXECUTE
* CR:    12/08/98 AM.
* SUBJ:  Execute one code action on current node.
* RET:   True if successful execution.
* NOTE:  Returning false will yield a "bad function name" error.
********************************************/


bool user::execute(
   _TCHAR *func,               // Name of code action.
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
static bool warned = false;   // Todo: make this a list of strings!

if (!auser)
   return false;

auser->setBadname(false);      // Name ok so far.
auser->setOkret(true);         // Function can turn this off if not ok.

// Just a simple test of code actions.

if (!_tcsicmp(func, _T("testfn")))
   return userTestfn(args, auser);
else if (!_tcsicmp(func,_T("analyzebuf")))
   return userAnalyzebuf(args, auser);
else if (!_tcsicmp(func,_T("analyzefile")))
   return userAnalyzefile(args, auser);
else if (!_tcsicmp(func,_T("loadanalyzer")))    // 04/29/03 AM.
   return userLoadanalyzer(args,auser);    // 04/29/03 AM.
else if (!_tcsicmp(func,_T("unloadanalyzer")))  // 04/29/03 AM.
   return userUnloadanalyzer(args,auser);  // 04/29/03 AM.
if (!_tcsicmp(func,_T("postagger")))            // 12/03/03 AM.
   return userPostagger(args,auser);       // 12/03/03 AM.

// MATCHED NO FUNCTION.
auser->setBadname(true);
auser->setOkret(false);

if (!warned)
   {
   //cerr << "[Code action: Unknown action=" << func << "]" << endl;
   warned = true;
   }
return false;
}

/********************************************
* FN:      CODE ACTIONS.
********************************************/

/********************************************
* FN:    USERTESTFN
* SUBJ:  Sample user-defined function.
********************************************/

bool user::userTestfn(
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
_TCHAR *str = 0;

_tsystem(_T("dir > d:\\dev\\xxx.txt"));

// Access current parse state.
Nlppp *nlppp = auser->getNlppp();

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

Ana *ana = 0;
if (parse)
   ana = Auser::getAna(parse);

CG *cg = 0;
if (ana)
   cg = ana->getCG();

///////////////////////
///  ARGUMENT TESTING
///////////////////////
// Show all the possible arguments that could be handled.
// Note that a compiled variant function would be needed for each type
// (str,num,ostr,sem), potentially.
Iarg *arg=0;
_TCHAR *a_str=0;
long a_num=-1;
float a_flt = 0.0;                                             // 08/29/02 AM.
_t_ostream *a_ostr=0;
NODE *a_node=0;
RFASem *a_sem=0;
CONCEPT *a_con=0;
PHRASE *a_phr=0;
ATTR *a_attr=0;
VAL *a_val=0;
if (args)
   {
//   if (args->Right())
	if (Auser::Right(args))  // 07/07/03 AM.
      cout << _T("[This fn deals only with zero or one arg.]") << endl;

//   arg = args->getData();
	arg = Auser::getData(args);  // 07/07/03 AM.
   switch (arg->getType())
      {
      case IASTR:
         a_str = arg->getStr();
         break;
      case IANUM:
         a_num = arg->getNum();
         break;
      case IAFLOAT:                                            // 08/29/02 AM.
         a_flt = arg->getFloat();                              // 08/29/02 AM.
         break;                                                // 08/29/02 AM.
      case IAOSTREAM:
         a_ostr = arg->getOstream();
         break;
      case IASEM:
         // Sem arg must be fetched according to its type.
         a_sem = arg->getSem();
         if (!a_sem)
            break;
         switch (a_sem->getType())
            {
            case RSSTR:
            case RSNAME:
            case RSNUM:
               a_str = a_sem->getName();   // This function name is antique.
               break;
            case RSLONG:
               a_num = a_sem->getLong();
               break;
            case RSFLOAT:                                      // 08/29/02 AM.
               a_flt = a_sem->getFloat();                      // 08/29/02 AM.
               break;                                          // 08/29/02 AM.
            case RSOSTREAM:
               a_ostr = a_sem->getOstream();
               break;
            case RSNODE:
               a_node = a_sem->getNode();
               break;

            case RS_KBCONCEPT:
               a_con = a_sem->getKBconcept();
               break;
            case RS_KBPHRASE:
               a_phr = a_sem->getKBphrase();
               break;
            case RS_KBATTR:
               a_attr = a_sem->getKBattr();
               break;
            case RS_KBVAL:
               a_val = a_sem->getKBval();
               break;
            default:
               break;
            }
         break;
      default:
         break;
      }
   }


///////////////////////
///  PARSE TREE TESTING
///////////////////////

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
if (!tree)
   {
   cout << _T("[pnroot: Couldn't fetch parse tree.]") << endl;
   return false;
   }

//Node<Pn> *root = tree->getRoot();
Node<Pn> *root = (Node<Pn> *)Auser::getRoot(tree);
if (!root)
   {
   cout << _T("[pnroot: Couldn't fetch root of parse tree.]") << endl;
   return false;
   }

// Example: Get name of parse tree root.
//Pn *pn = root->getData();
Pn *pn = Auser::getNodedata(root);	// 07/07/03 AM.
str = pn->getName();

///////////////////////
///  ANA TESTING
///////////////////////
_TCHAR *strx;
_TCHAR *bufx = _T("abc");
parse->internStr(bufx, /*UP*/ strx);

///////////////////////
///  KB TESTING
///////////////////////
_TCHAR buf[MAXSTR];


if (cg)
   {
   // Get root concept of kb.
   CONCEPT *root = cg->findRoot();

   // Example: Get name of root concept.
   if (root)
      cg->conceptName(root, /*UP*/ buf);
   }

// Output to the current stream, eg, "output.txt".
parse->strOut(_T("hello from user project\n"));
parse->flushOut();

// The next two lines return a LONG integer.
// auser->rettype_ = RETLONG;
// auser->retval_.long_ = 314159;

// This block returns a STR.
_TCHAR *ptr = _T("dummy");
parse->internStr(ptr, /*UP*/ str);
//auser->rettype_ = RETSTR;
//auser->retval_.str_ = str;
auser->retStr(str);

return true;
}


// RUNTIME VARIANT.
_TCHAR *user::testfn(Nlppp *nlppp)
{
if (!nlppp)
   return 0;

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

//return 314159;

// This block returns a STR.
_TCHAR *buf = _T("dummy");
_TCHAR *str=0;                     // Interned string.
parse->internStr(buf, /*UP*/ str);
return str;
}


/********************************************
* FN:    USERANALYZEFILE
* CR:    08/26/02 AM.
* SUBJ:  User-defined function for a nested call to an analyzer.
* RET:   True if ok.
* NOTE:  08/29/02 AM.   Renamed from userTestembed.
* TODO:  Create a compiled variant of this.
*        Parameterize more of this, handle buffers, streams, etc.
* FORM:  str = user::analyzefile(analyzer_str,infile_str,datum_str);
* EX:    G("buf") =
         user::analyzefile("Corporate","c:\xxx.txt","http://xxx.com");
********************************************/

bool user::userAnalyzefile(
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
_TCHAR *appname = 0, *infile = 0;
_TCHAR *datum = 0;                                               // 03/13/03 AM.
RFASem *sem = 0;																// 05/03/03 AM.

// Access current parse state.
Nlppp *nlppp = auser->getNlppp();

if (!Arg::str1(_T("analyzefile"), /*UP*/ args, appname))
   return true;
if (!Arg::str1(_T("analyzefile"), /*UP*/ args, infile))
   return true;
if (!Arg::str1(_T("analyzefile"), /*UP*/ args, datum))             // 03/13/03 AM.
   return true;
if (!Arg::sem1(_T("analyzefile"),nlppp, /*UP*/ args, sem,true))		// 05/04/03 AM.
	goto opts;	// Optional arg.											// 05/04/03 AM.
if (!Arg::done(args, _T("analyzefile")))
   return true;

opts:	// If there were optional args.

_t_ostream *ostr = 0;															// 05/03/03 AM.
if (sem)																			// 05/03/03 AM.
	{
	if (sem->getType() != RSOSTREAM)										// 05/03/03 AM.
		return true;															// 05/03/03 AM.
	ostr = sem->getOstream();												// 05/03/03 AM.
	}

// Use this as workhorse function.
_TCHAR *str = analyzefile(nlppp,appname,infile,datum,ostr);      // 05/03/03 AM.
auser->retStr(str);                                            // 03/13/03 AM.

auser->setOkret(true);
return true;
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!appsem)
	{
	if (ostrsem)
	  delete ostrsem;
   return false;
	}
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!insem)
	{
	if (ostrsem)
		delete ostrsem;
   return false;
	}
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!appsem)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!insem)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!nlppp)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
if (!appname || !*appname || !infile || !*infile)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *s_datum = Arun::sem_to_str(datum);
delete datum;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,s_datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!nlppp)
   {
	if (ostrsem)
		delete ostrsem;
   return false;
   }
if (!appname || !*appname || !infile || !*infile)
   {
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   _TCHAR *datum,                                                // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem)
   return false;
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
return analyzefile(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   _TCHAR *datum,                                                // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!insem)
   return false;
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   _TCHAR *datum,                                                // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   RFASem *datum,                                              // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem)
   {
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
return analyzefile(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   RFASem *datum,                                              // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!insem)
   {
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   RFASem *datum,                                              // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
return analyzefile(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   RFASem *datum,                                              // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!nlppp)
   {
   if (datum)
      delete datum;
   return false;
   }
if (!appname || !*appname || !infile || !*infile)
   {
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *s_datum = Arun::sem_to_str(datum);
delete datum;
return analyzefile(nlppp,appname,infile,s_datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzefile(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   _TCHAR *datum,                                                // 03/13/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!nlppp)
   return false;
if (!appname || !*appname || !infile || !*infile)
   return false;

// Get global data structures.
VTRun *vtrun = VTRun_Ptr;   // Exported global from Lite Library.
//ALIST *alist = (ALIST *)vtrun->alist_;

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

Ana *ana = 0;
if (parse)
   ana = parse->getAna();

bool develop=false;
bool compiled=false;
bool silent=false;

_TCHAR outfile[MAXSTR];
_TCHAR *output=0;
if (!output)
   output = _T("output.txt");       // Default.
_stprintf(outfile, _T("%s"), output);

_TCHAR *path=0;
if (!(path = _tgetenv(_T("APPS"))))
   return 0;

_TCHAR appdir[MAXSTR];
appdir[0] = '\0';
_stprintf(appdir,_T("%s%c%s"),path,DIR_CH,appname);

if (!path_exists(infile))
   {
   cerr << _T("[File/directory not found: ") << infile << _T("]") << endl;
   _stprintf(Errbuf,_T("[File/directory not found: %s]"), infile);
   parse->errOut(false);
   return 0;
   }

// To have analyzer output to a stream, can rebind the cout
// stream.  // 05/05/02 AM.
//_TCHAR ofstr[MAXSTR];
//sprintf(ofstr,"e:\\dummy.txt");
//ofstream os(ofstr, ios::out);

// Testing output to buffer.
_TCHAR obuf[OBUFSIZ];

NLP *nlp = vtrun->findAna(appname);
bool del = false;
CG *cg = 0;   // 07/18/03 AM.
if (!nlp)
   {   // START IF NOT NLP.
   del = true;   // Delete the instance locally.

////////  INSTANCE HERE /////
_TCHAR *sequence=0;

_TCHAR specdir[MAXSTR];

_TCHAR seqfile[MAXSTR];

_stprintf(specdir, _T("%s%sspec"),   appdir, DIR_STR);

// Get analyzer-sequence file name.
if (!sequence)
   sequence = _T("analyzer.seq");   // Default.

_stprintf(seqfile, _T("%s%s%s"), specdir,DIR_STR, sequence);


/////////////////////////////////////////////////
// INITIALIZE ANALYZER RUNTIME ENGINE
/////////////////////////////////////////////////
// Create and initialize an NLP object to manage text analysis.
// NOTE: This init will dynamically load the user extensions dll at
// appdir\user\debug\user.dll
//   nlp = new NLP(appdir, develop,compiled,silent,0,appname);
      // FIX.   // 03/13/03 AM.
	nlp = vtrun->makeNLP(appdir,appname,develop,silent,compiled); // 07/18/03 AM.

/////////////////////////////////////////////////
// SET UP THE KNOWLEDGE BASE
/////////////////////////////////////////////////

//cg = new CG(	// 07/18/03 AM.
//         appdir,
//         true,      // LOAD COMPILED KB IF POSSIBLE.
//         alist);    // Give KB the handle for List Manager.
//CG *cg = &cgobj;	// 07/18/03 AM.
//nlp->setCG(cg);      // Give analyzer the CG REFERENCE.
cg = vtrun->makeCG(appdir,true,nlp);	// 07/18/03 AM.
//if (!cg->readKB())
if (!cg)	// 07/18/03 AM.
   {
   cerr << _T("[Couldn't read knowledge base.]") << endl;
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
   return false;
   }

// Root of the KB hierarchy.
CONCEPT *root = cg->findRoot();

/////////////////////////////////////////////////
// BUILD ANALYZER APPLICATION
/////////////////////////////////////////////////
// Create an analyzer dynamically using the sequence file and rules files
// under appdir\spec.

if (!nlp->make_analyzer(seqfile, appdir, develop,
   silent,              // Debug/log file output.
   0,
   false,               // false == Don't compile during load.
   compiled))                    // Compiled/interp analyzer.
   {
   cerr << _T("[Couldn't build analyzer.]") << endl;
	vtrun->deleteCG(cg);	// 07/18/03 AM.
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
   return false;
   }


/////////////////////////////
// TEST RULE GENERATION.
/////////////////////////////
#ifdef TEST_RUG_
CONCEPT *gram = cg->findConcept(root, _T("gram"));
if (!gram)
   gram = cg->makeConcept(root, _T("gram"));
RUG::rugInit(gram,cg);

// Execute rule generation.
RUG *rug = new RUG(gram, nlp, appdir, cg);
rug->rugGenerate(false,false);      // Generate ALL.
delete rug;
rug = 0;
#endif


/////////////////////////////////////////////////
// ANALYZE INPUT TEXT FILES
/////////////////////////////////////////////////

if (is_file(infile))      // Input is a single file.
   {
   // If input is from a buffer, specify its name and length.
   nlp->analyze(infile, outfile, appdir, develop,
      silent,        // Debug/log output files.
      0,            // Outdir.
      0,            // Input buffer.
      0,            // Length of input buffer, or 0.
      compiled,      // If running compiled analyzer.
      ostr,            // Rebind cout output stream in analyzer
      obuf,
      OBUFSIZ,                                                 // 03/14/03 AM.
      datum                                                    // 03/13/03 AM.
      );
   }
else      // Develop mode not on.
   {
   // Analyze file, directory, or tree of directories.
   // NOTE: This function has not been upgraded.
   // analyze_path(nlp, infile, outfile, appdir, true);
   cout << _T("[Not parsing directory as input]") << endl;
   }

   }   // END IF NOT NLP.
else
   {
   if (is_file(infile))      // Input is a single file.
      {
      // If input is from a buffer, specify its name and length.
      nlp->analyze(infile, outfile, appdir, develop,
         silent,        // Debug/log output files.
         0,            // Outdir.
         0,            // Input buffer.
         0,            // Length of input buffer, or 0.
         compiled,      // If running compiled analyzer.
         ostr,            // Rebind cout output stream in analyzer
         obuf,
         OBUFSIZ,
         datum                                                 // 03/13/03 AM.
         );
      }
   }

// Can check that buffer was filled by analyzer calls such as
// cbuf() << "hello" << "\n";
//cerr << "obuf=";
//cerr << obuf << endl;


///////////////////////////
// RETURN VALUE.
///////////////////////////
// This block returns a STR.
_TCHAR *str=0;
if (obuf && *obuf)
parse->internStr(obuf, /*UP*/ str);

/////////////////////////////////////////////////
// CLEANUP ANALYZER RUNTIME
/////////////////////////////////////////////////

// This will close the user.dll for the application also.
if (del)
   {
	vtrun->deleteCG(cg);	// 07/18/03 AM.
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
	}

return str;
}


/********************************************
* FN:    USERANALYZEBUF
* CR:    12/27/02 AM.
* SUBJ:  User-defined function for a nested call to an analyzer.
* RET:   True if ok.
* NOTE:  Analyze given arg as input buffer.
********************************************/

bool user::userAnalyzebuf(
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
_TCHAR *appname = 0, *inbuf = 0;
_TCHAR *datum = 0;                                               // 04/27/03 AM.
RFASem *sem = 0;																// 05/03/03 AM.

// Access current parse state.
Nlppp *nlppp = auser->getNlppp();

if (!Arg::str1(_T("analyzebuf"), /*UP*/ args, appname))
   return true;
if (!Arg::str1(_T("analyzebuf"), /*UP*/ args, inbuf))
   return true;
if (!Arg::str1(_T("analyzebuf"), /*UP*/ args, datum))              // 04/27/03 AM.
   return true;
if (!Arg::sem1(_T("analyzebuf"),nlppp, /*UP*/ args, sem,true))		// 05/04/03 AM.
	goto opts;					// Optional arg.							// 05/04/03 AM.
if (!Arg::done(args, _T("analyzebuf")))
   return true;

opts:		// If there were optional args.

_t_ostream *ostr = 0;															// 05/03/03 AM.
if (sem)																			// 05/03/03 AM.
	{
	if (sem->getType() != RSOSTREAM)										// 05/03/03 AM.
		return true;															// 05/03/03 AM.
	ostr = sem->getOstream();												// 05/03/03 AM.
	}

// Use this as workhorse function.
_TCHAR *str = analyzebuf(nlppp,appname,inbuf,datum,ostr);        // 04/27/03 AM.
auser->retStr(str);

auser->setOkret(true);
return true;
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!appsem)
	{
	if (ostrsem)
	  delete ostrsem;
   return false;
	}
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!insem)
	{
	if (ostrsem)
		delete ostrsem;
   return false;
	}
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!appsem)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!insem)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   RFASem *datum,
	RFASem *ostrsem
   )
{
if (!nlppp)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
if (!appname || !*appname || !infile || !*infile)
   {
   if (datum)
      delete datum;
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_TCHAR *s_datum = Arun::sem_to_str(datum);
delete datum;
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,s_datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   _TCHAR *datum,
	RFASem *ostrsem
   )
{
if (!nlppp)
   {
	if (ostrsem)
		delete ostrsem;
   return false;
   }
if (!appname || !*appname || !infile || !*infile)
   {
	if (ostrsem)
		delete ostrsem;
   return false;
   }
_t_ostream *ostr = 0;
if (ostrsem)
	{
	ostr = Arun::sem_to_ostream(ostrsem);
	delete ostrsem;
	}
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, _TCHAR *inbuf,
   _TCHAR *datum,                                                // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem)
   return 0;
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
return analyzebuf(nlppp,appname,inbuf,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   _TCHAR *datum,                                                // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!insem)
   return 0;
_TCHAR *inbuf = Arun::sem_to_str(insem);
delete insem;
return analyzebuf(nlppp,appname,inbuf,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   _TCHAR *datum,                                                // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
   return 0;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *inbuf = Arun::sem_to_str(insem);
delete appsem;
delete insem;
return analyzebuf(nlppp,appname,inbuf,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, _TCHAR *infile,
   RFASem *datum,                                              // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem)
   {
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
delete appsem;
return analyzebuf(nlppp,appname,infile,datum,ostr);
}

// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, RFASem *insem,
   RFASem *datum,                                              // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!insem)
   {
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *infile = Arun::sem_to_str(insem);
delete insem;
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, RFASem *appsem, RFASem *insem,
   RFASem *datum,                                              // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!appsem || !insem)
   {
   if (appsem)
      delete appsem;
   if (insem)
      delete insem;
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *appname = Arun::sem_to_str(appsem);
_TCHAR *infile = Arun::sem_to_str(insem);
delete appsem;
delete insem;
return analyzebuf(nlppp,appname,infile,datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, _TCHAR *infile,
   RFASem *datum,                                              // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!nlppp)
   {
   if (datum)
      delete datum;
   return false;
   }
if (!appname || !*appname || !infile || !*infile)
   {
   if (datum)
      delete datum;
   return false;
   }
_TCHAR *s_datum = Arun::sem_to_str(datum);
delete datum;
return analyzebuf(nlppp,appname,infile,s_datum,ostr);
}


// RUNTIME VARIANT.
_TCHAR *user::analyzebuf(Nlppp *nlppp, _TCHAR *appname, _TCHAR *inbuf,
   _TCHAR *datum,                                                // 04/27/03 AM.
	_t_ostream *ostr																// 05/03/03 AM.
   )
{
if (!nlppp)
   return 0;
if (!appname || !*appname || !inbuf || !*inbuf)
   return 0;

// Get global data structures.
VTRun *vtrun = VTRun_Ptr;   // Exported global from Lite Library.
//ALIST *alist = (ALIST *)vtrun->alist_;

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

Ana *ana = 0;
if (parse)
   ana = parse->getAna();

bool develop=false;
bool compiled=false;
bool silent=false;

_TCHAR outfile[MAXSTR];
_TCHAR *output=0;
if (!output)
   output = _T("output.txt");       // Default.
_stprintf(outfile, _T("%s"), output);

_TCHAR *path=0;
if (!(path = _tgetenv(_T("APPS"))))
   return 0;

_TCHAR appdir[MAXSTR];
appdir[0] = '\0';
_stprintf(appdir,_T("%s%c%s"),path,DIR_CH,appname);

_TCHAR inf[MAXSTR];
_stprintf(inf, _T("%s%sdummy.txt"), appdir, DIR_STR);
if (!path_exists(inf))
   {
   cerr << _T("[File/directory not found: ") << inf << _T("]") << endl;
   _stprintf(Errbuf,_T("[File/directory not found: %s]"), inf);
   parse->errOut(false);
   return 0;
   }

// To have analyzer output to a stream, can rebind the cout
// stream.  // 05/05/02 AM.
//_TCHAR ofstr[MAXSTR];
//sprintf(ofstr,"e:\\dummy.txt");
//ofstream os(ofstr, ios::out);

// Testing output to buffer.
_TCHAR obuf[OBUFSIZ];

long inlen = _tcsclen(inbuf);

NLP *nlp = vtrun->findAna(appname);
bool del = false;
CG *cg = 0;   // 07/18/03 AM.
if (!nlp)
   {   // START IF NOT NLP.
   del = true;   // Delete the instance locally.

////////  INSTANCE HERE /////
_TCHAR *sequence=0;

_TCHAR specdir[MAXSTR];

_TCHAR seqfile[MAXSTR];

_stprintf(specdir, _T("%s%sspec"),   appdir, DIR_STR);

// Get analyzer-sequence file name.
if (!sequence)
   sequence = _T("analyzer.seq");   // Default.

_stprintf(seqfile, _T("%s%s%s"), specdir,DIR_STR, sequence);


/////////////////////////////////////////////////
// INITIALIZE ANALYZER RUNTIME ENGINE
/////////////////////////////////////////////////
// Create and initialize an NLP object to manage text analysis.
// NOTE: This init will dynamically load the user extensions dll at
// appdir\user\debug\user.dll
// nlp = new NLP(appdir, develop,compiled,silent,0,appname);
   nlp = vtrun->makeNLP(appdir,appname,develop,silent,compiled); // 07/18/03 AM.

/////////////////////////////////////////////////
// SET UP THE KNOWLEDGE BASE
/////////////////////////////////////////////////

//cg = new CG(	// 07/18/03 AM.
//         appdir,
//         true,      // LOAD COMPILED KB IF POSSIBLE.
//         alist);    // Give KB the handle for List Manager.
//CG *cg = &cgobj;	// 07/18/03 AM.
//nlp->setCG(cg);      // Give analyzer the CG REFERENCE.
//if (!cg->readKB())
cg = vtrun->makeCG(appdir,true,nlp);	// 07/18/03 AM.
if (!cg)	// 07/18/03 AM.
   {
   cerr << _T("[Couldn't read knowledge base.]") << endl;
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
   return 0;
   }

// Root of the KB hierarchy.
CONCEPT *root = cg->findRoot();

/////////////////////////////////////////////////
// BUILD ANALYZER APPLICATION
/////////////////////////////////////////////////
// Create an analyzer dynamically using the sequence file and rules files
// under appdir\spec.

if (!nlp->make_analyzer(seqfile, appdir, develop,
   silent,              // Debug/log file output.
   0,
   false,               // false == Don't compile during load.
   compiled))                    // Compiled/interp analyzer.
   {
   cerr << _T("[Couldn't build analyzer.]") << endl;
	vtrun->deleteCG(cg);	// 07/18/03 AM.
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
   return 0;
   }


/////////////////////////////
// TEST RULE GENERATION.
/////////////////////////////
#ifdef TEST_RUG_
CONCEPT *gram = cg->findConcept(root, _T("gram"));
if (!gram)
   gram = cg->makeConcept(root, _T("gram"));
RUG::rugInit(gram,cg);

// Execute rule generation.
RUG *rug = new RUG(gram, nlp, appdir, cg);
rug->rugGenerate(false,false);      // Generate ALL.
delete rug;
rug = 0;
#endif

// ANALYZE BUFFER.
nlp->analyze(inf, outfile, appdir, develop,
   silent,        // Debug/log output files.
   0,             // Outdir.
   inbuf,         // Input buffer.
   inlen,         // Length of input buffer, or 0.
   compiled,      // If running compiled analyzer.
   ostr,            // Rebind cout output stream in analyzer
   obuf,
   OBUFSIZ,
   datum            // 04/27/03 AM.
   );

   }   // END IF NOT NLP.
else
   {

/////////////////////////////////////////////////
// ANALYZE INPUT TEXT FILES
/////////////////////////////////////////////////

   // ANALYZE BUFFER.
   nlp->analyze(inf, outfile, appdir, develop,
      silent,        // Debug/log output files.
      0,             // Outdir.
      inbuf,         // Input buffer.
      inlen,         // Length of input buffer, or 0.
      compiled,      // If running compiled analyzer.
      ostr,            // Rebind cout output stream in analyzer
      obuf,
      OBUFSIZ,
      datum            // 04/27/03 AM.
      );
   }

// Can check that buffer was filled by analyzer calls such as
// cbuf() << "hello" << "\n";
//cerr << "obuf=";
//cerr << obuf << endl;


///////////////////////////
// RETURN VALUE.
///////////////////////////
// This block returns a STR.
_TCHAR *str=0;
if (obuf && *obuf)
parse->internStr(obuf, /*UP*/ str);
//auser->rettype_ = RETSTR;
//auser->retval_.str_ = str;

/////////////////////////////////////////////////
// CLEANUP ANALYZER RUNTIME
/////////////////////////////////////////////////

// This will close the user.dll for the application also.
if (del)
   {
   vtrun->deleteCG(cg);	// 07/18/03 AM.
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
	}
return str;
}


/********************************************
* FN:    USERLOADANALYZER
* CR:    04/29/03 AM.
* SUBJ:  User-defined function to load an analyzer into memory.
* RET:   True if ok.
* FORM:  handle = user::loadanalyzer(analyzer_str);
* EX:    G("handle") = user::analyzefile("Corporate");
********************************************/

bool user::userLoadanalyzer(
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
_TCHAR *appname = 0;

if (!Arg::str1(_T("loadanalyzer"), /*UP*/ args, appname))
   return true;
if (!Arg::done(args, _T("loadanalyzer")))
   return true;

// Access current parse state.
Nlppp *nlppp = auser->getNlppp();

// Use this as workhorse function.
_TCHAR *str = loadanalyzer(nlppp,appname);
auser->retStr(str);

auser->setOkret(true);
return true;
}


// RUNTIME VARIANT.
_TCHAR *user::loadanalyzer(	// 04/29/03 AM.
	Nlppp *nlppp,
	_TCHAR *appname
   )
{
if (!nlppp)
   return false;
if (!appname || !*appname)
   return false;

// For now, the analyzer name is the handle.
// Todo: handle should be separate, so that multiple
// instances of the same analyzer may be loaded.
// (This will require overhauls to the working area of an analyzer by TAI.)
_TCHAR *handle = appname;

// Get global data structures.
VTRun *vtrun = VTRun_Ptr;   // Exported global from Lite Library.
ALIST *alist = (ALIST *)vtrun->alist_;

NLP *nlp = vtrun->findAna(handle);
if (nlp)
	{
	// Warn that the analyzer is already loaded.
	return handle;
	}

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

Ana *ana = 0;
if (parse)
   ana = parse->getAna();

bool develop=false;
bool compiled=false;
bool silent=false;

_TCHAR outfile[MAXSTR];
_TCHAR *output=0;
if (!output)
   output = _T("output.txt");       // Default.
_stprintf(outfile, _T("%s"), output);

_TCHAR *path=0;
if (!(path = _tgetenv(_T("APPS"))))
   return 0;

_TCHAR appdir[MAXSTR];
appdir[0] = '\0';
_stprintf(appdir,_T("%s%c%s"),path,DIR_CH,handle);

// To have analyzer output to a stream, can rebind the cout
// stream.  // 05/05/02 AM.
_TCHAR ofstr[MAXSTR];
_stprintf(ofstr,_T("e:\\dummy.txt"));
_t_ofstream os(TCHAR2CA(ofstr), ios::out);

////////  INSTANCE HERE /////
_TCHAR *sequence=0;

_TCHAR specdir[MAXSTR];

_TCHAR seqfile[MAXSTR];

_stprintf(specdir, _T("%s%sspec"),   appdir, DIR_STR);

// Get analyzer-sequence file name.
if (!sequence)
   sequence = _T("analyzer.seq");   // Default.

_stprintf(seqfile, _T("%s%s%s"), specdir,DIR_STR, sequence);


/////////////////////////////////////////////////
// INITIALIZE ANALYZER RUNTIME ENGINE
/////////////////////////////////////////////////
// Create and initialize an NLP object to manage text analysis.
// NOTE: This init will dynamically load the user extensions dll at
// appdir\user\debug\user.dll
//nlp = new NLP(appdir, develop,compiled,silent,0,handle);
nlp = vtrun->makeNLP(appdir,handle,develop,silent,compiled); // 07/18/03 AM.

/////////////////////////////////////////////////
// SET UP THE KNOWLEDGE BASE
/////////////////////////////////////////////////

//CG *cg = new CG(	// 07/18/03 AM.
//         appdir,
//         true,      // LOAD COMPILED KB IF POSSIBLE.
//         alist);    // Give KB the handle for List Manager.
//CG *cg = &cgobj;	// 07/18/03 AM.
//nlp->setCG(cg);      // Give analyzer the CG REFERENCE.
//if (!cg->readKB())
CG *cg = vtrun->makeCG(appdir,true,nlp);	// 07/18/03 AM.
if (!cg)	// 07/18/03 AM.
   {
   cerr << _T("[Couldn't read knowledge base.]") << endl;
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
   return false;
   }

// Root of the KB hierarchy.
CONCEPT *root = cg->findRoot();

/////////////////////////////////////////////////
// BUILD ANALYZER APPLICATION
/////////////////////////////////////////////////
// Create an analyzer dynamically using the sequence file and rules files
// under appdir\spec.

if (!nlp->make_analyzer(seqfile, appdir, develop,
   silent,              // Debug/log file output.
   0,
   false,               // false == Don't compile during load.
   compiled))                    // Compiled/interp analyzer.
   {
   cerr << _T("[Couldn't build analyzer.]") << endl;
	vtrun->deleteCG(cg);	// 07/18/03 AM.
   vtrun->deleteNLP(nlp); // 07/18/03 AM.
   return false;
   }
return handle;
}


/********************************************
* FN:    USERUNLOADANALYZER
* CR:    04/29/03 AM.
* SUBJ:  User-defined function to unload an analyzer from memory.
* RET:   True if ok.
* FORM:  str = user::unloadanalyzer(handle);
* EX:    G("handle") = user::analyzefile("Corporate");
********************************************/

bool user::userUnloadanalyzer(
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
_TCHAR *handle = 0;

if (!Arg::str1(_T("unloadanalyzer"), /*UP*/ args, handle))
   return true;
if (!Arg::done(args, _T("unloadanalyzer")))
   return true;

// Access current parse state.
Nlppp *nlppp = auser->getNlppp();

bool success = unloadanalyzer(nlppp, handle);

auser->retBool(success);
auser->setOkret(true);
return true;
}


// RUNTIME VARIANT.
bool user::unloadanalyzer(	// 04/29/03 AM.
	Nlppp *nlppp,
	_TCHAR *handle
   )
{
if (!nlppp)
   return false;
if (!handle || !*handle)
   return false;

VTRun *vtrun = VTRun_Ptr;   // Exported global from Lite Library.

if (!vtrun)
	return false;

NLP *nlp = vtrun->findAna(handle);

if (!nlp)
	return false;

vtrun->deleteNLP(nlp); // 07/18/03 AM.
return true;
}




/********************************************
* FN:    USERPOSTAGGER
* CR:    12/03/03 AM.
* SUBJ:  User-defined function to invoke a 3rd-party POS tagger.
* RET:   True if ok.
* FORM:  str = user::unloadanalyzer(handle);
* EX:    user::postagger();
* NOTE:  Assumes parse tree has been tokenized already.
********************************************/

bool user::userPostagger(
   DELTS *args,         // Action's arguments.
   Auser *auser
   )
{
if (!Arg::done(args, _T("postagger")))
   return true;

// Access current parse state.
Nlppp *nlppp = auser->getNlppp();

bool success = postagger(nlppp);

auser->retBool(success);
auser->setOkret(true);
return true;
}


// RUNTIME VARIANT.
bool user::postagger(
	Nlppp *nlppp
   )
{
if (!nlppp)
   return false;

// Access the current parse object.
Parse *parse = Auser::getParse(nlppp);

// Assume tagger is in VisualText\apps folder.
_TCHAR *path=0;
if (!(path = _tgetenv(_T("VISUALTEXT"))))
   return 0;

// Tagger program.  (LexTagger sample is Java code.)
_TCHAR *tagger = _T("LexTagger-1.0");
_TCHAR taggerpath[MAXSTR];
_stprintf(taggerpath,_T("%s\\apps\\%s"), path, tagger);	// $visualtext\\samples\\taggerfolder.

_TCHAR cmd[MAXSTR];

// Specify input and output.
// May need to copy input file, create a config file, etc.,
// depending on the 3rd party tagger.

// Get input file path.
_TCHAR inf[MAXSTR];
_stprintf(inf,_T("%s\\test\\input.txt"), taggerpath);

// Get name of output file.
_TCHAR outf[MAXSTR];
_stprintf(outf,_T("%s\\test\\result.txt"), taggerpath);

// Copy current input file to tagger work area.
_stprintf(cmd,_T("copy /y \"%s\" \"%s\""), parse->getInput(), inf);
_tsystem(cmd);

// Invoke POS Tagger.
_stprintf(cmd,_T("\"%s\\run.bat\""), taggerpath);
_tsystem(cmd);

// Poll here, waiting for output file to be ready.

// Parse the tagged output file and update
// the parse tree.
posParse(outf,nlppp);

return true;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


/********************************************
* FN:    UCODEINI
* CR:    01/23/02 AM.
* SUBJ:  Initialize USER project during analyzer load.
* RET:   True if ok, else false.
* NOTE:  Operates as soon as analyzer is fully loaded.
********************************************/

bool ucodeIni(NLP *nlp)
{
// TODO:   Put your initializations here.
//cout << "[ucodeIni:]" << endl;

// For example, internalize a string for NLP++ to manage.
_TCHAR *buf = _T("abc");
_TCHAR *str;
nlp->internStr(buf, str);

return true;
}

/********************************************
* FN:    UCODEFIN
* CR:    01/23/02 AM.
* SUBJ:  Clean up USER project during analyzer close.
* RET:   True if ok, else false.
* NOTE:  Operates immediately before analyzer is closed.
********************************************/

bool ucodeFin(NLP *nlp)
{
// TODO: Put your cleanups here.
//cout << "[ucodeFin:]" << endl;
return true;
}

/*************************      END OF FILE      ******************************/
