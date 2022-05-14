/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	FN.CPP
* FILE:	lite/fn.cpp
* CR:		01/14/00 AM.
* SUBJ:	NLP++ functions.
* NOTE:	"FN" data type equates to an RFASem object.
*
*******************************************************************************/

#include "StdAfx.h"
#include <limits.h>	//MAXINT,MAXLONG,MAX_INT,MAX_LONG			// 01/20/00 AM.
#include <locale.h>	// For char handling.							// 01/06/03 AM.
#include <math.h>		// For math, log10.								// 04/29/04 AM.
//#include <process.h>				// 06/20/00 AM.
//#include <atlstr.h>	// 05/19/14 DDH.

#ifdef LINUX
#ifdef _ODBC
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>
#endif
#else
#include <direct.h>	// For _mkdir										// 12/07/01 AM.
#endif

#include "machine.h"
#include "lite/lite.h"		// MOVE UP.	// 03/23/19 AM.
#include "lite/global.h"	// MOVE UP.	// 03/23/19 AM.

#include "u_out.h"		// 01/19/06 AM.
#include "consh/libconsh.h"		// 02/14/01 AM.
#include "consh/cg.h"				// 02/14/01 AM.

#include "htab.h"					// 02/12/07 AM.
#include "kb.h"					// 02/12/07 AM.

#include "prim/libprim.h"	// 09/15/08 AM.
#ifndef DWORD	// 09/16/20 AM.
#define DWORD double
#endif
LIBPRIM_API DWORD run_silent(_TCHAR* strCMD);	// 09/15/08 AM.

//#include "lite/lite.h"				// 07/07/03 AM.
//#include "lite/global.h"
#include "lite/mach.h"			// 02/08/01 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "lite/Auser.h"			// 07/07/03 AM.
#include "lite/dir.h"			// 02/11/03 AM.
#include "iarg.h"					// 02/27/01 AM.
#include "inline.h"
#include "str.h"
#include "chars.h"				// 09/28/00 AM.
#include "words/words.h"		// 10/16/00 AM.
#include "words/wordarrs.h"	// 10/16/00 AM.
#ifndef LINUX
//#include "web/web.h"				// 02/11/03 AM.
//WEB_API int tear_url(LPCTSTR url, _TCHAR *outfile);	// 02/11/99 AM.
//#include <vector>	// 05/06/14 DDH.
//#include <wininet.h>	// 05/06/14 DDH.
//#include "web.h"	// 05/06/14 DDH.
//using namespace openutils;	// 05/06/14 DDH.
#endif


#include "io.h"
#include "lite/nlppp.h"				// 02/22/00 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "string.h"

#include "ana.h"					// 02/22/00 AM.
#include "parse.h"				// 02/22/00 AM.
#include "ivar.h"					// 10/18/00 AM.
//#include "lite/nlppp.h"				// 02/22/00 AM.
#include "ipnode.h"				// 10/18/00 AM.
#include "rfasem.h"
#include "arg.h"					// 01/20/00 AM.
#include "pat.h"					// 11/24/00 AM.
#include "dyn.h"					// 02/09/01 AM.
#include "lite/Auser.h"			// 02/13/01 AM.
#include "iaction.h"				// 02/27/01 AM.
#include "ipair.h"				// 05/16/01 AM.
#include "var.h"					// 05/16/01 AM.

#include "func_defs.h"			// 12/21/01 AM.
#include "nlp.h"					// 12/21/01 AM.
#include	"ifunc.h"				// 12/21/01 AM.
#include "iexpr.h"	// For unpackvar.	// 05/26/02 AM.
#include "irule.h"				// 09/16/08 AM.
#include "lite/vtrun.h"			// 08/28/02 AM.
#include "xml.h"					// 05/10/03 AM.

#ifndef LINUX
#include "LexTagger.h"			// 12/11/03 AM.
#endif

#ifdef WORDNET
#include "WNFunctions.h"
#endif

#include "fn.h"

/********************************************
* FN:		FNCALL
* CR:		01/14/00 AM.
* SUBJ:	Switch for calling region-independent NLP++ functions.
* RET:	ok - True if everything ok, false if error.
*			UP badname - True if not a known function name here.
*			UP sem - semantic structure to hold returned value of function.
* OPT:	Want an enum value here, so won't need to do string compares.
*********************************************/

bool Fn::fnCall(
	Iaction *action,
	Nlppp *nlppp,		// 02/22/00 AM.
	/*UP*/
	bool &badname,		// If unknown independent function name.
	RFASem* &sem		// Function return value.
	)
{
// Unpack the function.
badname = false;
_TCHAR *func				= action->getName();
Dlist<Iarg> *dargs	= action->getArgs();
nlppp->parse_->line_ = action->getLine();								// 08/24/02 AM.
Delt<Iarg> *args;	// UNINITIALIZED!
if (dargs)
   args		= dargs->getFirst();
else
	args = 0;

_TCHAR *scope = action->getScope();										// 02/16/01 AM.
Parse *parse = nlppp->parse_;										// 08/24/02 AM.

// If function already looked up once, then get its def or id.	// 12/26/01 AM.
// BUG: If hash table rebuilt, then all these pointers are STALE,
// so can't set up the optimization here.  Need to do it at analyzer
// build time.																	// 12/27/01 AM.
// NOTE: Leaving the optimized check code here.						// 12/27/01 AM.
enum funcDef fnid = FNNULL;												// 12/26/01 AM.
Ifunc *ifunc = action->getFunc();										// 12/26/01 AM.
if (ifunc)	// Optimized check.
	{
	fnid = ifunc->getId();
	if (fnid == FNNULL		// Not a builtin NLP++ fn.				// 12/26/01 AM.
	 && ifunc->getBody())													// 12/26/01 AM.
		return ifunc->eval(args,nlppp,sem);
	}
else																				// 12/26/01 AM.
	{
	// LOOK UP FUNCTION ID IN BUILTINS HASH TABLE.					// 12/21/01 AM.
	Ana *ana = parse->getAna();											// 12/21/01 AM.
	NLP *nlp = ana->getNLP();												// 12/21/01 AM.
	VTRun *vtrun = nlp->getVTRun();	// [DEGLOB]	// 10/15/20 AM.
//	void *htab = nlp->getHtfunc();										// 12/21/01 AM.
//	void *htab = VTRun_Ptr->htfunc_;				// 08/28/02 AM.
	void *htab = vtrun->htfunc_;	// [DEGLOB]	// 10/15/20 AM.
	Ifunc *ifunc = Ifunc::htLookup(func,htab);						// 12/27/01 AM.
	if (ifunc)																	// 12/26/01 AM.
		{
		fnid = ifunc->getId();												// 12/26/01 AM.
//		action->setFunc(ifunc);									// BAD	// 12/27/01 AM.
		}

	// LOOK IT UP IN USER-DEFINED NLP++ FUNCTIONS.					// 12/21/01 AM.
	// If found, EXECUTE user-define function.						// 12/21/01 AM.
	else																			// 12/26/01 AM.
		{
		htab = ana->getHtfunc();											// 12/21/01 AM.
		if ((ifunc = Ifunc::htLookup(func,htab)))						// 12/21/01 AM.
			{
//			action->setFunc(ifunc);								// BAD.	// 12/27/01 AM.
			return ifunc->eval(args,nlppp,sem);							// 12/21/01 AM.
			}
		}
	}

if (scope && *scope)															// 02/16/01 AM.
	goto userfn;																// 02/16/01 AM.

switch (fnid)																	// 12/21/01 AM.
	{
	case FNNULL:
		break;
	case FNaddarg:
		break;
	case FNaddattr:
		return fnAddattr(args, nlppp, /*UP*/ sem);					// 02/22/00 AM.
	case FNaddcnode:
	return fnAddcnode(args, nlppp, /*UP*/ sem);						// 03/03/00 AM.
	case FNaddconcept:
//		return fnAddconcept(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
		break;
	case FNaddconval:
		return fnAddconval(args, nlppp, /*UP*/ sem);					// 08/11/00 AM.
	case FNaddnode:
		return fnAddnode(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNaddnumval:
		return fnAddnumval(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNaddstmt:
		break;
	case FNaddstrs:
		break;
	case FNaddstrval:
		return fnAddstrval(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNaddsval:
		return fnAddsval(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNaddword:
		return fnAddword(args, nlppp, /*UP*/ sem);					// 05/09/00 AM.
	case FNarraylength:
		return fnArraylength(args,nlppp,/*UP*/sem);					// 10/20/00 AM.
	case FNattrchange:
		return fnAttrchange(args, nlppp, /*UP*/ sem);				// 03/02/00 AM.
	case FNattrexists:
		return fnAttrexists(args, nlppp, /*UP*/ sem);				// 03/02/00 AM.
	case FNattrname:
		return fnAttrname(args, nlppp, /*UP*/ sem);					// 02/23/00 AM.
	case FNattrvals:
		return fnAttrvals(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNattrwithval:
		return fnAttrwithval(args, nlppp, /*UP*/ sem);				// 05/02/00 AM.
	case FNbatchstart:
		return fnBatchstart(args,nlppp,/*UP*/sem);					// 10/19/00 AM.
	case FNcap:
		break;
	case FNclosefile:
		return fnClosefile(args,nlppp,/*UP*/sem);						// 05/21/01 AM.
	case FNconceptname:
		return fnConceptname(args, nlppp, /*UP*/ sem);				// 02/22/00 AM.
	case FNconceptpath:
		return fnConceptpath(args, nlppp, /*UP*/ sem);				// 03/02/00 AM.
	case FNconval:
		return fnConval(args, nlppp, /*UP*/ sem);						// 03/02/00 AM.
	case FNcout:
		return fnCout(args,nlppp,/*UP*/sem);							// 05/05/02 AM.
	case FNcbuf:
		return fnCbuf(args,nlppp,/*UP*/sem);							// 05/11/02 AM.
	case FNcoutreset:
		return fnCoutreset(args,nlppp,/*UP*/sem);						// 05/06/02 AM.
#ifdef _ODBC
	case FNdballocstmt:
		return fnDballocstmt(args,nlppp,/*UP*/sem);					// 05/25/02 AM.
	case FNdbbindcol:
		return fnDbbindcol(args,nlppp,/*UP*/sem);						// 05/26/02 AM.
	case FNdbclose:
		return fnDbclose(args,nlppp,/*UP*/sem);						// 05/23/02 AM.
	case FNdbexec:
		return fnDbexec(args,nlppp,/*UP*/sem);							// 05/23/02 AM.
	case FNdbexecstmt:
		return fnDbexecstmt(args,nlppp,/*UP*/sem);					// 05/25/02 AM.
	case FNdbfetch:
		return fnDbfetch(args,nlppp,/*UP*/sem);						// 05/26/02 AM.
	case FNdbfreestmt:
		return fnDbfreestmt(args,nlppp,/*UP*/sem);					// 05/25/02 AM.
	case FNdbopen:
		return fnDbopen(args,nlppp,/*UP*/sem);							// 05/23/02 AM.
#endif
	case FNdeaccent:
		return fnDeaccent(args,nlppp,/*UP*/sem);						// 09/07/03 AM.
	case FNdebug:
		return fnDebug(args,nlppp,/*UP*/sem);							// 12/07/00 AM.
	case FNdejunk:
		return fnDejunk(args,nlppp,/*UP*/sem);							// 09/09/11 AM.
	case FNdictfindword:
		return fnDictfindword(args, nlppp, /*UP*/ sem);				// 04/18/00 Dd.
	case FNdictfirst:
		return fnDictfirst(args, nlppp, /*UP*/ sem);					// 03/21/03 AM.
	case FNdictgetword:
		return fnDictgetword(args, nlppp, /*UP*/ sem);				// 06/29/03 AM.
	case FNdictnext:
		return fnDictnext(args, nlppp, /*UP*/ sem);					// 03/21/03 AM.
	case FNDICTphraselookup:
		return fnDICTphraselookup(args, nlppp, /*UP*/ sem);		// 12/15/14 AM.
	case FNdown:
		return fnDown(args, nlppp, /*UP*/ sem);						// 03/02/00 AM.
	//else:
	case FNeltnode:
		return fnEltnode(args,nlppp,/*UP*/sem);						// 09/12/06 AM.
	case FNexcise:
		break;
	case FNexitpass:
		return fnExitpass(args,nlppp,/*UP*/sem);						// 02/06/01 AM.
	case FNexittopopup:
		return fnExittopopup(args,nlppp,/*UP*/sem);					// 05/24/02 AM.
	case FNfactorial:
		return fnFactorial(args,/*UP*/sem);
	case FNfail:
		return fnFail(args, nlppp, /*UP*/ sem);						// 06/10/00 AM.
	case FNfileout:
		break;
	case FNfindana:
		return fnFindana(args,nlppp,/*UP*/sem);						// 02/04/03 AM.
	case FNfindattr:
		return fnFindattr(args, nlppp, /*UP*/ sem);					// 02/23/00 AM.
	case FNfindattrs:
		return fnFindattrs(args, nlppp, /*UP*/ sem);					// 02/23/00 AM.
	case FNfindconcept:
		return fnFindconcept(args,nlppp,/*UP*/sem);					// 02/23/00 AM.
	case FNfindhierconcept:
		return fnFindhierconcept(args, nlppp, /*UP*/ sem);			// 03/02/00 AM.
	case FNfindnode:
		return fnFindnode(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNfindphrase:
		return fnFindphrase(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNfindroot:
		return fnFindroot(args,nlppp,/*UP*/sem);						// 02/22/00 AM.
	case FNfindvals:
		return fnFindvals(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNfindwordpath:
		return fnFindwordpath(args, nlppp, /*UP*/ sem);				// 05/30/00 AM.
	case FNfirstnode:
		return fnFirstnode(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNflt:
		return fnFlt(args, nlppp, /*UP*/ sem);							// 08/22/01 AM.
	case FNfltval:
		return fnFltval(args, nlppp, /*UP*/ sem);						// 12/27/06 AM.
	case FNfncallstart:
		break;
	case FNfprintgvar:
		break;
	case FNfprintnvar:
		break;
	case FNfprintxvar:
		break;
	case FNfprintvar:
		break;
	case FNgdump:
		break;
	case FNgetconcept:
		return fnGetconcept(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNgetconval:
		return fnGetconval(args, nlppp, /*UP*/ sem);					// 08/12/00 AM.
	case FNgetnumval:
		return fnGetnumval(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNgetpopupdata:
		return fnGetpopupdata(args,nlppp,/*UP*/sem);					// 05/25/02 AM.
	case FNgetstrval:
		return fnGetstrval(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNgetsval:
		return fnGetsval(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNginc:
		break;
	case FNgp:
	case FNgroup:
		return fnGroup(args,nlppp,/*UP*/sem);							// 11/23/00 AM.
	case FNgtolower:
		break;
	case FNguniq:
		break;
	case FNhitconf:
		return fnHitconf(args,nlppp,/*UP*/sem);						// 10/08/00 AM.
	//if:
	//ifelse:
	//ifstmt:
	case FNinc:
		break;
	case FNinheritval:
		return fnInheritval(args, nlppp, /*UP*/ sem);				// 08/12/00 AM.
	case FNinputrange:														// 12/15/02 AM.
		return fnInputrange(args,nlppp,/*UP*/sem);					// 12/15/02 AM.
	case FNinputrangetofile:
		return fnInputrangetofile(args,nlppp,/*UP*/sem);			// 05/29/03 AM.
	case FNinteractive:
		return fnInteractive(args,nlppp,/*UP*/sem);					// 05/06/02 AM.
	case FNkbdumptree:
		return fnKbdumptree(args,nlppp,/*UP*/sem);					// 08/06/01 AM.
	case FNlasteltnode:
		return fnLasteltnode(args,nlppp,/*UP*/sem);					// 11/25/00 AM.
	case FNlastnode:
		return fnLastnode(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNlength:
		break;
	case FNlengthr:
		break;
	case FNlevenshtein:
		return fnLevenshtein(args, nlppp, /*UP*/ sem);				// 03/06/07 AM.
	case FNlextagger:
		return fnLextagger(args,nlppp,/*UP*/sem);						// 12/11/03 AM.
	case FNlistadd:
		break;
	case FNlistnode:
		return fnListnode(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNLJ:
	case FNlj:
		return fnLj(args,nlppp,/*UP*/sem);								// 10/02/00 AM.
	case FNlogten:
		return fnLogten(args,nlppp,/*UP*/sem);							// 04/29/04 AM.
	case FNlook:
		break;
	case FNlookup:
		break;
	case FNlowercase:
		break;
	case FNmakeconcept:
		return fnMakeconcept(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNmakephrase:
		return fnMakephrase(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNmakestmt:
		break;
	case FNmerge:
		break;
	case FNmerger:
		break;
	case FNmkdir:																// 12/06/01 AM.
		return fnMkdir(args, nlppp, /*UP*/ sem);						// 12/06/00 AM.
	case FNmovecleft:
		return fnMovecleft(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNmovecright:
		return fnMovecright(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNmovesem:
		break;
	case FNndump:
		break;
	case FNnext:
		return fnNext(args, nlppp, /*UP*/ sem);						// 03/02/00 AM.
	case FNnextattr:
		return fnNextattr(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNnextval:
		return fnNextval(args, nlppp, /*UP*/ sem);					// 03/02/00 AM.
	case FNninc:
		break;
	case FNnodeconcept:
		return fnNodeconcept(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNnodeowner:
		return fnNodeowner(args, nlppp, /*UP*/ sem);					// 12/03/02 AM.
	case FNnoop:
		break;
	case FNnum:
		return fnNum(args, nlppp, /*UP*/ sem);							// 07/26/00 AM.
	case FNnumrange:
		break;
	case FNnumval:
		return fnNumval(args, nlppp, /*UP*/ sem);						// 03/02/00 AM.
	case FNopenfile:
		return fnOpenfile(args,nlppp,/*UP*/sem);// Ch fm ofile.	// 05/21/01 AM.
	case FNor:
		break;
	case FNpathconcept:
		return fnPathconcept(args, nlppp, /*UP*/ sem);				// 04/25/00 AM.
	case FNpercentstr:
		return fnPercentstr(args,nlppp,/*UP*/sem);					// 09/30/00 AM.
	case FNpermuten:
		return fnPermuten(args,nlppp,/*UP*/sem);						// 03/10/04 AM.
	case FNphraselength:
		return fnPhraselength(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNphraseraw:
		return fnPhraseraw(args,nlppp,/*UP*/sem);						// 11/20/00 AM.
	case FNphrasetext:
		return fnPhrasetext(args,nlppp,/*UP*/sem);					// 11/20/00 AM.
	case FNpncopyvars:
		return fnPncopyvars(args,nlppp,/*UP*/sem);					// 05/16/01 AM.
	case FNpndown:
		return fnPndown(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpninsert:
		return fnPninsert(args,nlppp,/*UP*/sem);						// 07/14/08 AM.
	case FNpnmakevar:
		return fnPnmakevar(args,nlppp,/*UP*/sem);						// 06/26/01 AM.
	case FNpnmove:
		return fnPnmove(args,nlppp,/*UP*/sem);							// 10/06/13 AM.
	case FNpnname:
		return fnPnname(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpnnext:
		return fnPnnext(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpnprev:
		return fnPnprev(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpnpushval:
		return fnPnpushval(args,nlppp,/*UP*/sem);					// 12/12/14 AM.
	case FNpnreplaceval:
		return fnPnreplaceval(args,nlppp,/*UP*/sem);					// 06/27/01 AM.
	case FNpnroot:
		return fnPnroot(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpnrpushval:
		return fnPnrpushval(args,nlppp,/*UP*/sem);					// 12/12/14 AM.
	case FNpnsingletdown:
		return fnPnsingletdown(args,nlppp,/*UP*/sem);				// 10/18/00 AM.
	case FNpnup:
		return fnPnup(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpnvar:
		return fnPnvar(args,nlppp,/*UP*/sem);							// 10/18/00 AM.
	case FNpnvarnames:
		return fnPnvarnames(args,nlppp,/*UP*/sem);					// 05/13/02 AM.
	case FNpranchor:
		break;
	case FNprchild:
		break;
	case FNpreaction:
		break;
	case FNprintvar:
		break;
	case FNpndeletechilds:
		return fnPndeletechilds(args,nlppp,/*UP*/sem);				// 11/25/00 AM.
	case FNpnrename:
		return fnPnrename(args,nlppp,/*UP*/sem);						// 11/21/00 AM.
	case FNprev:
		return fnPrev(args, nlppp, /*UP*/ sem);						// 03/02/00 AM.
	case FNprint:
		break;
	case FNprintr:
		break;
	case FNprlit:
		break;
	case FNprrange:
		break;
	case FNprtree:
		break;
	case FNprunephrases:
		return fnPrunephrases(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNprxtree:
		break;
	case FNregexp:																// 03/23/09 AM.
		break;
	case FNregexpi:															// 03/26/09 AM.
		break;
	case FNrenameattr:
		return fnRenameattr(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNrenamechild:
		return fnRenamechild(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNrenameconcept:
		return fnRenameconcept(args, nlppp, /*UP*/ sem);			// 03/03/00 AM.
	case FNrenamenode:
		return fnRenamenode(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNreplaceval:
		return fnReplaceval(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNresolveurl:
		return fnResolveurl(args,nlppp,/*UP*/sem);					// 05/14/03 AM.
	case FNrfaaction:
	case FNrfaactions:
	case FNrfaarg:
	case FNrfaargtolist:
	case FNrfacode:
	case FNrfaelement:
	case FNrfaelt:
	case FNrfaexpr:
	case FNrfalist:
	case FNrfalitelt:
	case FNrfalittoaction:
	case FNrfalittopair:
	case FNrfaname:
	case FNrfanodes:
	case FNrfanonlit:
	case FNrfanonlitelt:
	case FNrfanum:
	case FNrfaop:
	case FNrfapair:
	case FNrfapairs:
	case FNrfapostunary:
	case FNrfapres:
	case FNrfarange:
	case FNrfarecurse:
	case FNrfarecurses:
	case FNrfaregion:
	case FNrfaregions:
	case FNrfareturnstmt:													// 03/07/02 AM.
	case FNrfarule:
	case FNrfarulelts:
	case FNrfarulemark:
	case FNrfarules:
	case FNrfarulesfile:
	case FNrfaselect:
	case FNrfastr:
	case FNrfasugg:
	case FNrfaunary:
	case FNrfavar:
	case FNrfbarg:
	case FNrfbdecl:															// 12/19/01 AM.
	case FNrfbdecls:															// 12/19/01 AM.
		break;
	case FNrightjustifynum:
		return fnRightjustifynum(args,nlppp,/*UP*/sem);				// 09/30/00 AM.
	case FNrmattr:
		return fnRmattr(args, nlppp, /*UP*/ sem);						// 05/11/00 AM.
	case FNrmattrs:
		return fnRmattrs(args,nlppp,/*UP*/sem);						// 10/31/00 AM.
	case FNrmattrval:
		return fnRmattrval(args, nlppp, /*UP*/ sem);					// 05/17/00 AM.
	case FNrmchild:
		return fnRmchild(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNrmchildren:
		return fnRmchildren(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNrmconcept:
		return fnRmconcept(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNrmcphrase:
		return fnRmcphrase(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNrmnode:
		return fnRmnode(args, nlppp, /*UP*/ sem);						// 03/03/00 AM.
	case FNrmphrase:
		return fnRmphrase(args, nlppp, /*UP*/ sem);					// 03/03/00 AM.
	case FNrmval:
		return fnRmval(args, nlppp, /*UP*/ sem);						// 05/11/00 AM.
	case FNrmvals:
		return fnRmvals(args, nlppp, /*UP*/ sem);						// 03/03/00 AM.
	case FNrmword:
		return fnRmword(args, nlppp, /*UP*/ sem);						// 07/17/00 AM.
	case FNsdump:
	case FNsetbase:
		break;
	case FNsetlookahead:
		return fnSetlookahead(args,nlppp,/*UP*/sem);					// 12/14/03 AM.
	case FNsetunsealed:
	case FNsingle:
	case FNsingler:
	case FNsinglex:
	case FNsinglezap:
		break;
	case FNsortconsbyattr:
		return fnSortconsbyattr(args,nlppp,/*UP*/sem);				// 12/02/02 AM.
	case FNsortchilds:
		return fnSortchilds(args, nlppp, /*UP*/ sem);				// 02/20/09 AM.
	case FNsorthier:
		return fnSorthier(args, nlppp, /*UP*/ sem);					// 02/20/09 AM.
	case FNsortphrase:
		return fnSortphrase(args, nlppp, /*UP*/ sem);				// 03/03/00 AM.
	case FNsortvals:
		break;
	case FNspellcandidates:
		return fnSpellcandidates(args,nlppp,/*UP*/sem);				// 09/27/00 AM.
	case FNspellcorrect:
		return fnSpellcorrect(args,nlppp,/*UP*/sem);					// 09/27/00 AM.
	case FNspellword:
		return fnSpellword(args,nlppp,/*UP*/sem);						// 09/27/00 AM.
	case FNsplice:
		return fnSplice(args,nlppp,/*UP*/sem);							// 07/19/02 AM.
	case FNsplit:
		return fnSplit(args,nlppp,/*UP*/sem);							// 03/14/03 AM.
	case FNsqlstr:
		return fnSqlstr(args,nlppp,/*UP*/sem);							// 07/22/03 AM.
	case FNstartout:
		break;
	case FNstem:																// 12/28/01 AM.
		return fnStem(args,nlppp,/*UP*/sem);							// 12/28/01 AM.
	case FNstopout:
		break;
	case FNstr:
		return fnStr(args, nlppp, /*UP*/ sem);							// 07/26/00 AM.
	case FNstrchar:
		return fnStrchar(args, nlppp, /*UP*/ sem);					// 05/05/00 Dd.
	case FNstrchr:
		return fnStrchr(args, nlppp, /*UP*/ sem);						// 11/18/00 AM.
	case FNstrchrcount:														// 12/15/02 AM.
		return fnStrchrcount(args,nlppp,/*UP*/sem);					// 12/15/02 AM.
	case FNstrclean:
		return fnStrclean(args, nlppp, /*UP*/ sem);					// 11/09/00 Dd.
	case FNstrcontains:
		return fnStrcontains(args, nlppp, /*UP*/ sem);				// 04/25/00 Dd.
	case FNstrcontainsnocase:
		return fnStrcontainsnocase(args, nlppp, /*UP*/ sem);		// 04/25/00 Dd.
	case FNstrstartswith:
		return fnStrstartswith(args, nlppp, /*UP*/ sem);				// 11/20/00 Dd.
	case FNstrendswith:
		return fnStrendswith(args, nlppp, /*UP*/ sem);				// 11/20/00 DD.
	case FNstrequal:
		return fnStrequal(args, nlppp, /*UP*/ sem);					// 04/25/00 Dd.
	case FNstrequalnocase:
		return fnStrequalnocase(args, nlppp, /*UP*/ sem);			// 04/25/00 Dd.
	case FNstrescape:
		return fnStrescape(args, nlppp, /*UP*/ sem);					// 07/24/01 Dd.
	case FNstrunescape:
		return fnStrunescape(args, nlppp, /*UP*/ sem);				// 07/24/01 Dd.
	case FNstrgreaterthan:
		return fnStrgreaterthan(args, nlppp, /*UP*/ sem);			// 04/25/00 Dd.
	case FNstrisalpha:
		return fnStrisalpha(args, nlppp, /*UP*/ sem);				// 05/05/00 Dd.
	case FNstrisdigit:
		return fnStrisdigit(args, nlppp, /*UP*/ sem);				// 05/05/00 Dd.
	case FNstrislower:
		return fnStrislower(args,nlppp, /*UP*/ sem);					// 11/20/01 AM.
	case FNstrisupper:
		return fnStrisupper(args,nlppp, /*UP*/ sem);					// 11/20/01 AM.
	case FNstrlength:
		return fnStrlength(args, nlppp, /*UP*/ sem);					// 05/05/00 Dd.
	case FNstrlessthan:
		return fnStrlessthan(args, nlppp, /*UP*/ sem);				// 04/25/00 Dd.
	case FNstrnotequal:
		return fnStrnotequal(args, nlppp, /*UP*/ sem);				// 04/25/00 Dd.
	case FNstrnotequalnocase:
		return fnStrnotequalnocase(args, nlppp, /*UP*/ sem);		// 04/25/00 Dd.
	case FNstrpiece:
		return fnStrpiece(args, nlppp, /*UP*/ sem);					// 05/05/00 Dd.
	case FNstrrchr:
		return fnStrrchr(args, nlppp, /*UP*/ sem);					// 11/18/00 AM.
	case FNstrspellcandidate:
		return fnStrspellcandidate(args,nlppp,/*UP*/sem);			// 09/26/00 DD.
	case FNstrspellcompare:
		return fnStrspellcompare(args,nlppp,/*UP*/sem);				// 09/26/00 DD.
	case FNstrsubst:
		return fnStrsubst(args, nlppp, /*UP*/ sem);					// 09/22/00 Dd.
	case FNstrtolower:
		return fnStrtolower(args, nlppp, /*UP*/ sem);				// 05/04/00 Dd.
	case FNstrtotitle:
		return fnStrtotitle(args, nlppp, /*UP*/ sem);				// 08/21/00 Dd.
	case FNstrtoupper:
		return fnStrtoupper(args, nlppp, /*UP*/ sem);				// 05/04/00 Dd.
	case FNstrtrim:
		return fnStrtrim(args, nlppp, /*UP*/ sem);					// 11/09/00 Dd.
	case FNstrval:
		return fnStrval(args, nlppp, /*UP*/ sem);						// 03/02/00 AM.
	case FNstrwrap:
		return fnStrwrap(args, nlppp, /*UP*/ sem);					// 09/22/00 Dd.
	case FNsucceed:
		return fnSucceed(args, nlppp, /*UP*/ sem);					// 06/10/00 AM.
	case FNsuffix:	// 11/23/01 AM.
		return fnSuffix(args,nlppp,/*UP*/sem);							// 11/21/01 AM.
	case FNsystem:
		return fnSystem(args, nlppp, /*UP*/ sem);						// 06/20/00 AM.
	case FNtake:
		return fnTake(args, nlppp, /*UP*/ sem);						// 05/09/00 AM.
	case FNtoday:
		return fnToday(args,nlppp,/*UP*/sem);							// 02/08/01 AM.
	case FNtopdir:
		return fnTopdir(args,nlppp,/*UP*/sem);							// 10/20/00 AM.
	case FNunknown:
		break;
	case FNunpackdirs:
		return fnUnpackdirs(args,nlppp,/*UP*/sem);					// 10/20/00 AM.
	case FNup:
		return fnUp(args, nlppp, /*UP*/ sem);							// 03/02/00 AM.
	case FNuppercase:
		break;
	case FNurlbase:
		return fnUrlbase(args,nlppp,/*UP*/sem);						// 03/16/03 AM.
	case FNurltofile:
		return fnUrltofile(args,nlppp,/*UP*/sem);						// 02/11/03 AM.
	case FNvar:																	// 06/14/05 AM.
		break;
	case FNvareq:																// 07/25/06 AM.
		break;
	case FNvarfn:
	case FNvarfnarray:
		break;
	case FNvarinlist:
		return fnVarinlist(args,nlppp,/*UP*/sem);						// 11/21/00 AM.
	case FNvarne:																// 07/25/06 AM.
		break;
	case FNvarstrs:
		break;
	case FNvarz:																// 07/25/06 AM.
		break;

#ifdef WORDNET
	case FNWNInit:
		return fnWNInit(args,nlppp,/*UP*/sem);							// 07/07/04 Dd.
	case FNWNSensesToConcept:
		return fnWNSensesToConcept(args,nlppp,/*UP*/sem);							// 07/07/04 Dd.
	case FNWNHypnymsToConcept:
		return fnWNHypnymsToConcept(args,nlppp,/*UP*/sem);
#endif
		//while:
	//whilestmt:
	case FNwordindex:
		return fnWordindex(args, nlppp, /*UP*/ sem);					// 05/31/00 AM.
	case FNwordpath:
		return fnWordpath(args, nlppp, /*UP*/ sem);					// 03/06/00 AM.
	case FNxaddlen:
	case FNxaddnvar:
	case FNxdump:
	case FNxinc:
		break;
	case FNxmlstr:
		return fnXmlstr(args,nlppp,/*UP*/sem);							// 05/10/03 AM.
	case FNxrename:

	case FNXXYYZZ:
		break;
	};

// KEEP THIS LAST.															// 02/13/01 AM.
userfn:																			// 02/16/01 AM.
bool fnret = false;															// 02/09/01 AM.
if (userFn(func,args,nlppp,/*UP*/sem,fnret,badname))				// 02/09/01 AM.
	{
	if (!scope || !*scope)													// 02/16/01 AM.
		{
		_stprintf(Errbuf,_T("[Warning: Correct syntax to user::%s.]"),	// 05/18/01 AM.
						func);													// 05/18/01 AM.
		return parse->errOut(false); // UNFIXED 										// 05/18/01 AM.
		}
	return fnret;																// 02/09/01 AM.
	}

badname = true;
return false;
}


/********************************************
* FN:		USERFN
* CR:		02/09/01 AM.
* SUBJ:	Execute user-defined function.
* RET:
* NOTE:	Based on Code::userCode.
********************************************/

Auser glob_Auser;			// Global object.

bool Fn::userFn(
	_TCHAR *func,
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem,			// Returned sem object.
	bool &fnret,			// Return of called function.
	bool &badname			// If no matching function found.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.
NLP *nlp = parse->getNLP();		// [DEGLOB]	// 10/15/20 AM.
VTRun *vtrun = nlp->getVTRun();	// [DEGLOB]	// 10/15/20 AM.

fnret = false;
badname = true;
#ifndef LINUX

// Set up info transfer object.
glob_Auser.zero();		// Empty out structure.
glob_Auser.setNlppp(nlppp);
//glob_Auser.vtrun_ = VTRun_Ptr;	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
glob_Auser.vtrun_ = vtrun;							// [DEGLOB]	// 10/15/20 AM.

// Do the ucodeAction(func, args, nlppp) thing here.
// In the USER.DLL library.
HINSTANCE hdll = parse->getHdll();
if (hdll && call_ucodeAction(hdll, func,args,&glob_Auser))
	{
	if (badname = glob_Auser.getBadname())	// Bad name.
		return true;
	if (!(fnret = glob_Auser.getOkret()))	// Bad fn return.
		return true;
	// Set up semantic object here, based on returned value.
	switch (glob_Auser.rettype_)
		{
		case RETCONCEPT:
			sem = new RFASem(glob_Auser.retval_.kbconcept_, RS_KBCONCEPT);
			break;
		case RETPHRASE:
			sem = new RFASem(glob_Auser.retval_.kbphrase_, RS_KBPHRASE);
			break;
		case RETATTR:
			sem = new RFASem(glob_Auser.retval_.kbattr_, RS_KBATTR);
			break;
		case RETVAL:
			sem = new RFASem(glob_Auser.retval_.kbval_, RS_KBVAL);
			break;
		case RETNODE:
			sem = new RFASem(glob_Auser.retval_.node_);
			break;
		case RETSTR:
			sem = new RFASem(glob_Auser.retval_.str_, RSSTR);
			break;
		case RETLONG:
			sem = new RFASem(glob_Auser.retval_.long_);
			break;
		case RETOSTR:
			sem = new RFASem(glob_Auser.retval_.ostr_);
			break;
		case RETBOOL:
			sem = new RFASem(glob_Auser.retval_.bool_ ? (long)1 : (long)0);
			break;
		case RETNULL:
		default:
			break;
		}
	return true;
	}

#endif
return false;
}


/********************************************
* FN:		FNFACTORIAL
* CR:		01/14/00 AM.
* SUBJ:	Factorial function.
* RET:	ok - True if everything ok, false if error.
*			UP sem - semantic structure to hold returned value of function.
* NOTE:	Testing out independent functions in code.
* FORM:	factorial(num)
*********************************************/

bool Fn::fnFactorial(
	Delt<Iarg> *args,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
long num1;
long fact = -1;
sem = 0;

if (!Arg::num1(_T("factorial"),/*DU*/ (DELTS*&)args, num1, false))
	return false;
if (!Arg::done((DELTS*)args, _T("factorial")))
	return false;

if (num1 < 0)
	{
	_stprintf(Errbuf, _T("[factorial: Given bad value.]"));				// 05/18/01 AM.
	return errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
else
	{
	fact = 1;	// Value is good for the 0 and 1 cases.
	while (num1 > 1)
		{
		if ((double)fact > ((double)LONG_MAX / (double)num1))
			{
			// Overflow.
			_stprintf(Errbuf, _T("[factorial: Overflow.]"));				// 05/18/01 AM.
			return errOut(false); // UNFIXED 											// 05/18/01 AM.
			}
		fact *= num1--;
		}
	}
sem = new RFASem(fact);
return true;																	// 05/18/01 AM.
}


/********************************************
* FN:		FNFINDROOT
* CR:		02/22/00 AM.
* SUBJ:	Get root concept of KB tree.
* RET:	True if ok, else false.
*			UP - sem for root concept.
* FORM:	findroot()
********************************************/

bool Fn::fnFindroot(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *s_conc = 0, *attr=0, *val=0;

if (!Arg::done((DELTS*)args, _T("findroot"),parse))
	return false;

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

CONCEPT *root = cg->findRoot();

if (root)															// FIX.	// 11/15/00 AM.
	sem = new RFASem(root, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNFINDCONCEPT
* CR:		02/23/00 AM.
* SUBJ:	Find concept under given parent.
* RET:	True if ok, else false.
*			UP - child concept.
* FORMS:	findconcept(parent, namestr)
*			findconcept(parent, num)
* NOTE:	Handling TWO VARIANTS here.
********************************************/

bool Fn::fnFindconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;
_TCHAR *name1=0;
long num1= -1;

if (!Arg::sem1(_T("findconcept"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str_or_num1(_T("findconcept"), /*UP*/ (DELTS*&)args, name1, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("findconcept"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findconcept: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 											// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[findconcept: Bad semantic arg.]"));			// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();
CONCEPT *child = 0;

if (name1 && *name1)		// 2nd arg was name.
	child = cg->findConcept(conc1, name1);
else							// 2nd arg was num.
	child = cg->findConcept(conc1, num1);

if (child)															// FIX.	// 11/15/00 AM.
	sem = new RFASem(child, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNFINDATTR
* CR:		02/23/00 AM.
* SUBJ:	Find concept's attribute.
* RET:	True if ok, else false.
*			UP - attr.
* FORMS:	findattr(conc, namestr)
********************************************/

bool Fn::fnFindattr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;
_TCHAR *name1=0;

if (!Arg::sem1(_T("findattr"),nlppp,(DELTS*&)args,sem1))
	return true;													// FIX.	// 08/21/00 AM.
if (!Arg::str1(_T("findattr"), /*UP*/ (DELTS*&)args, name1))
	return true;													// FIX.	// 08/21/00 AM.
if (!Arg::done((DELTS*)args, _T("findattr"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findattr: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 												// 05/18/01 AM.
	}
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findattr: Warning. Given no name.]"));		// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[findattr: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();
ATTR *attr = 0;

attr = cg->findAttr(conc1, name1);

if (!attr)			// No attr found is ok.					// FIX.	// 08/07/00 AM.
	return true;													// FIX.	// 08/07/00 AM.

sem = new RFASem(attr, RS_KBATTR);
return true;
}


/********************************************
* FN:		FNFINDATTRS
* CR:		02/23/00 AM.
* SUBJ:	Find concept's attributes.
* RET:	True if ok, else false.
*			UP - attrs.
* FORMS:	findattrs(conc)
********************************************/

bool Fn::fnFindattrs(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("findattrs"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("findattrs"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findattrs: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[findattrs: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();
ATTR *attr = 0;

attr = cg->findAttrs(conc1);

sem = new RFASem(attr, RS_KBATTR);
return true;
}


/********************************************
* FN:		FNATTRNAME
* CR:		02/23/00 AM.
* SUBJ:	Get attribute's name.
* RET:	True if ok, else false.
*			UP - name
* FORMS:	attrname(attr)
* NOTE:	Not totally analogous to cg.cpp function.
********************************************/

bool Fn::fnAttrname(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("attrname"),nlppp,(DELTS*&)args,sem1))
	{
	_stprintf(Errbuf,_T("[attrname: Given bad arg.]"));					// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
if (!Arg::done((DELTS*)args, _T("attrname"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findattrname: Warning. Given no attr.]"));	// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBATTR)
	{
	_stprintf(Errbuf,_T("[attrname: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
ATTR *attr1 = sem1->getKBattr();

// Get attr name from kb.
_TCHAR buf[NAMESIZ];
cg->attrName(attr1, buf, NAMESIZ);

if (!*buf)															// FIX.	// 08/07/00 AM.
	{
	_stprintf(Errbuf,_T("[attrname: Error. Attr with no name.]"));		// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Internalize in analyzer's string table.
_TCHAR *str;
parse->internStr(buf, /*UP*/ str);		// FIX.		// 02/24/00 AM.

// Return as string type.
sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNATTRVALS
* CR:		03/02/00 AM.
* SUBJ:	Get attribute's values.
* RET:	True if ok, else false.
*			UP - values of attribute
* FORMS:	attrvals(attr)
********************************************/

bool Fn::fnAttrvals(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("attrvals"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("attrvals"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrvals: Warning. Given no attr.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBATTR)
	{
	_stprintf(Errbuf,_T("[attrname: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
ATTR *attr1 = sem1->getKBattr();

// Get attr name from kb.
VAL *vals = cg->attrVals(attr1);

// Return as val type.
sem = new RFASem(vals, RS_KBVAL);
return true;
}


/********************************************
* FN:		FNFINDVALS
* CR:		03/02/00 AM.
* SUBJ:	Fetch attribute values.
* RET:	True if ok, else false.
*			UP attrs - list of attribute values.
* FORM:	findvals(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnFindvals(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
Parse *parse = nlppp->parse_;										// 08/24/02 AM.

if (!Arg::sem1(_T("findvals"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("findvals"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::done((DELTS*)args, _T("findvals"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findvals: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findvals: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[findvals: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

VAL *vals = cg->findVals(conc1, attr_str);

// Return as val type.
sem = new RFASem(vals, RS_KBVAL);

return true;
}


/********************************************
* FN:		FNNUMVAL
* CR:		03/02/00 AM.
* SUBJ:	Fetch numeric attribute value.
* RET:	True if ok, else false.
*			UP num - numeric value of attribute.
* FORM:	numval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnNumval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
Parse *parse = nlppp->parse_;										// 08/24/02 AM.

if (!Arg::sem1(_T("numval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("numval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::done((DELTS*)args, _T("numval"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[numval: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[numval: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[numval: Bad semantic arg.]"));					// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// NOTE: If float in kb, gets converted to integer.	// 12/27/06 AM.
// Fix to grab float.	// 12/26/06 AM.
float fval = 0.0;			// 12/26/06 AM.
if (!cg->findVal(conc1, attr_str, /*UP*/ fval))			// FIX.	// 08/21/00 AM.
	fval = 0;															// FIX.	// 08/21/00 AM.
long val = (long) fval;													// CONVERT.	// 12/27/06 AM.
if (abs(fval - val) > 0.00001)	// tolerance,delta					// 12/27/06 AM.
	{
	_stprintf(Errbuf,_T("[numval: Truncating float (use fltval).]"));		// 12/27/06 AM.
	parse->errOut(true); // UNFIXED 											// 12/27/06 AM.
	}

// Return as val type.
//	sem = new RFASem(val);					// 12/26/06 AM.
sem = new RFASem(val);

return true;
}


/********************************************
* FN:		FNFLTVAL
* CR:		12/27/06 AM.
* SUBJ:	Fetch float attribute value.
* RET:	True if ok, else false.
*			UP flt - float value of attribute.
* FORM:	fltval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnFltval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("fltval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("fltval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::done((DELTS*)args, _T("fltval"),parse))
	return false;

if (!con_sem)
	{
	_stprintf(Errbuf,_T("[fltval: Warning. Given no concept.]"));
	return parse->errOut(true);
	}
if (!attr_str)
	{
	_stprintf(Errbuf,_T("[fltval: Warning. Given no attr name.]"));
	return parse->errOut(true);
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[fltval: Bad semantic arg.]"));
	return parse->errOut(false);
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Fix to grab float.
float fval = 0.0;
if (!cg->findVal(conc1, attr_str, /*UP*/ fval))
	fval = 0;

// Return as val type.
sem = new RFASem(fval);

return true;
}


/********************************************
* FN:		FNSTRVAL
* CR:		03/02/00 AM.
* SUBJ:	Fetch numeric attribute value.
* RET:	True if ok, else false.
*			UP str - string value of attribute.
* FORM:	strval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnStrval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("strval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("strval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::done((DELTS*)args, _T("strval"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strval: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strval: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[strval: Bad semantic arg.]"));					// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();
Htab *ht = parse->getHtab();	// 02/12/07 AM.

_TCHAR *str = KB::strVal(conc1,attr_str,cg,ht);						// 02/12/07 AM.

//char buf[MAXSTR];															// 11/12/00 AM.
//buf[0] = '\0';																// 11/12/00 AM.
//if (!cg->findVal(conc1, attr_str, /*UP*/ buf))		// OPT.	// 11/12/00 AM.
if (!str)					// OPT.	// 11/12/00 AM.					// 02/12/07 AM.
	{
	sem = new RFASem((_TCHAR *)0, RSSTR);						// FIX.	// 08/21/00 AM.
	return true;													// FIX.	// 08/21/00 AM.
	}

//parse->internStr(buf, /*UP*/ str);						// OPT.	// 11/12/00 AM.

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNCONVAL
* CR:		08/12/00 AM.
* SUBJ:	Fetch concept attribute value.
* RET:	True if ok, else false.
*			UP con - concept value of attribute.
* FORM:	conval(concept, attributename)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnConval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("conval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("conval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::done((DELTS*)args, _T("conval"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[conval: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[conval: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[conval: Bad semantic arg.]"));					// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *val = 0;
if (!cg->findVal(conc1, attr_str, /*UP*/ val))
	val = 0;															// FIX.	// 08/21/00 AM.

// Return as val type.
if (val)																// FIX.	// 11/15/00 AM.
	sem = new RFASem(val, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNINHERITVAL
* CR:		03/02/00 AM.
* SUBJ:	Fetch str attribute value up the hierarchy.
* RET:	True if ok, else false.
*			UP str - string value of attribute.
* FORM:	inheritval(concept, attributename, hierconcept)
	hier is the top of the concept hierarchy to be searched.
	hier = 0 means search up to root of the entire tree.
********************************************/

bool Fn::fnInheritval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
RFASem *hier_sem = 0;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("inheritval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("inheritval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::sem1(_T("inheritval"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("inheritval"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[inheritval: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[inheritval: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[inheritval: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[inheritval: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *hier1 = 0;															// 10/30/00 AM.
if (hier_sem)																	// 10/30/00 AM.
	hier_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

_TCHAR buf[MAXSTR];
if (!cg->inheritVal(conc1, attr_str, hier1, /*UP*/ buf))
	{
	//sem = new RFASem((char *)0, RSSTR);					// FIX.	// 09/14/00 AM.
	return true;													// FIX.	// 08/21/00 AM.
	}
if (!buf || !*buf)												// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str = 0;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNCONCEPTNAME
* CR:		02/22/00 AM.
* SUBJ:	Get name of given concept.
* RET:	True if ok, else false.
*			UP - name of concept.
* FORM:	conceptname(concept)
********************************************/

bool Fn::fnConceptname(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *sem1;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("conceptname"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("conceptname"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[conceptname: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[conceptname: Bad semantic arg.]"));			// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

if (!conc1)																		// 08/07/00 AM.
	{
	_stprintf(Errbuf,_T("[conceptname: Empty concept.]"));				// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[MAXSTR];
_TCHAR *str;
cg->conceptName(conc1, /*UP*/ buf);
if (!buf || !*buf)												// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNCONCEPTPATH
* CR:		03/02/00 AM.
* SUBJ:	Get path of given concept.
* RET:	True if ok, else false.
*			UP str - path of concept.
* FORM:	conceptpath(concept)
********************************************/

bool Fn::fnConceptpath(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;

RFASem *sem1;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("conceptpath"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("conceptpath"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[conceptpath: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[conceptpath: Bad semantic arg.]"));			// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

_TCHAR buf[PATHSIZ];
_TCHAR *str;
cg->conceptPath(conc1, /*UP*/ buf);
if (!buf || !*buf)												// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNWORDPATH
* CR:		03/06/00 AM.
* SUBJ:	Get word-concept path of given string.
* RET:	True if ok, else false.
*			UP str - path of concept.
* FORM:	wordpath(str)
********************************************/

bool Fn::fnWordpath(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
if (!Arg::str1(_T("wordpath"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("wordpath"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[wordpath: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

_TCHAR buf[PATHSIZ];
_TCHAR *str;
cg->wordPath(str1, /*UP*/ buf);
if (!buf || !*buf)												// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNFINDWORDPATH
* CR:		05/30/00 AM.
* SUBJ:	Find word-concept path of given string.
* RET:	True if ok, else false.
*			UP str - path of concept.
* FORM:	findwordpath(str)
* NOTE:	Unlike wordpath, doesn't add word if not already present.
********************************************/

bool Fn::fnFindwordpath(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
if (!Arg::str1(_T("findwordpath"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("findwordpath"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findwordpath: Warning. Given no str.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();


_TCHAR buf[PATHSIZ];
_TCHAR *str;
cg->findWordpath(str1, /*UP*/ buf);
if (!*buf)															// FIX.	// 09/07/00 AM.
	return true;													// FIX.	// 09/07/00 AM.

// Intern the name here.
parse->internStr(buf, /*UP*/ str);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNWORDINDEX
* CR:		05/31/00 AM.
* SUBJ:	Get index concept for given string.
* RET:	True if ok, else false.
*			UP index - index concept for placing word.
* FORM:	wordindex(str)
********************************************/

bool Fn::fnWordindex(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
if (!Arg::str1(_T("wordindex"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("wordindex"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[wordindex: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();


CONCEPT *conc = cg->wordIndex(str1);

if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNPATHCONCEPT
* CR:		04/25/00 AM.
* SUBJ:	Get concept for given path.
* RET:	True if ok, else false.
*			UP con - concept
* FORM:	pathconcept(path)
********************************************/

bool Fn::fnPathconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *path;

if (!Arg::str1(_T("pathconcept"), (DELTS*&)args, path))
	return false;
if (!Arg::done((DELTS*)args, _T("pathconcept"),parse))
	return false;

if (!path)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pathconcept: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *con = cg->pathConcept(path);

if (con)																// FIX.	// 11/15/00 AM.
	sem = new RFASem(con, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNFINDHIERCONCEPT
* CR:		03/02/00 AM.
* SUBJ:	Find name in a subhierarchy.
* RET:	True if ok, else false.
*			UP conc - concept found.
* FORM:	findhierconcept(name, hierconcept)
	Find hierarchy concept.  Given a name, look through the given subtree
	for it.  Also look through node names.  If a node, return its owning
	concept.  If hierconcept == 0, start at root of concept tree.
********************************************/

bool Fn::fnFindhierconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name_str=0;
RFASem *hier_sem = 0;

if (!Arg::str1(_T("findhierconcept"), (DELTS*&)args, name_str))
	return false;
if (!Arg::sem1(_T("findhierconcept"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("findhierconcept"),parse))
	return false;

if (!name_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findhierconcept: Warning. Given no str.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();
CONCEPT *conc1 = 0;															// 10/30/00 AM.
if (!hier_sem)																	// 10/30/00 AM.
	conc1 = cg->findRoot();													// 10/30/00 AM.
else if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[findhierconcept: Bad semantic arg.]"));		// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
else
	conc1 = hier_sem->getKBconcept();


CONCEPT *conc = cg->findHierConcept(name_str, conc1);
//	return false;	// FIX. // 04/28/00 AM.

// Return appropriate value.
if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNATTREXISTS
* CR:		03/02/00 AM.
* SUBJ:	Search for given attr and value in given subtree.
* RET:	True if ok, else false.
*			UP bool - 1 if found, 0 if not.
* FORM:	attrexists(hier_conc, attr_str, val_str)
	Find hierarchy concept.  Given a name, look through the given subtree
	for it.  Also look through node names.  If a node, return its owning
	concept.  If hierconcept == 0, start at root of concept tree.
********************************************/

bool Fn::fnAttrexists(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *attr_str=0;
_TCHAR *val_str=0;
RFASem *hier_sem = 0;

if (!Arg::sem1(_T("attrexists"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::str1(_T("attrexists"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::str1(_T("attrexists"), (DELTS*&)args, val_str))
	return false;
if (!Arg::done((DELTS*)args, _T("attrexists"),parse))
	return false;

if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrexists: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!val_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrexists: Warning. Given no val str.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();
CONCEPT *hier1 = 0;															// 10/30/00 AM.
if (!hier_sem)																	// 10/30/00 AM.
	hier1 = cg->findRoot();													// 10/30/00 AM.
else if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[attrexists: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
else																				// 10/30/00 AM.
	hier1 = hier_sem->getKBconcept();


bool found = cg->attrExists(hier1, attr_str, val_str);
//	return false;			// 04/25/00 AM. Rogue return caught by Dave.
long val;
if (found)
	val = 1;
else
	val = 0;

// Return appropriate value.
sem = new RFASem(val);

return true;
}


/********************************************
* FN:		FNATTRWITHVAL
* CR:		05/02/00 AM.
* SUBJ:	See if concept's attr has given val.
* RET:	True if ok.
*			UP bool - 1 if found, 0 if not.
* FORM:	attrwithval(conc, attr_str, val_str)
********************************************/

bool Fn::fnAttrwithval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *attr_str=0;
_TCHAR *val_str=0;
RFASem *conc_sem = 0;

if (!Arg::sem1(_T("attrwithval"),nlppp,(DELTS*&)args, conc_sem))
	return false;
if (!Arg::str1(_T("attrwithval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::str1(_T("attrwithval"), (DELTS*&)args, val_str))
	return false;
if (!Arg::done((DELTS*)args, _T("attrwithval"),parse))
	return false;

if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrwithval: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!val_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("attrwithval: Warning. Given no val str.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (!conc_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrwithval: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
		}

if (conc_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[attrwithval: Bad semantic arg.]"));			// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = conc_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

bool found = cg->attrWithval(conc1, attr_str, val_str);
long val;
if (found)
	val = 1;
else
	val = 0;

// Return appropriate value.
sem = new RFASem(val);

return true;
}


/********************************************
* FN:		FNATTRCHANGE
* CR:		03/02/00 AM.
* SUBJ:	Change all values of attr to the new value, in a subtree.
* RET:	True if ok, else false.
*			UP
* FORM:	attrchange(hier_conc, attr_str, val_str, nval_str)
	hier_conc - subtree to examine.
	attr_str, val_str - existing attribute and value sought.
	nval_str - new value to substitute.
********************************************/

bool Fn::fnAttrchange(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *attr_str=0;
_TCHAR *val_str=0;
_TCHAR *nval_str=0;
RFASem *hier_sem = 0;

if (!Arg::sem1(_T("attrchange"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::str1(_T("attrchange"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::str1(_T("attrchange"), (DELTS*&)args, val_str))
	return false;
if (!Arg::str1(_T("attrchange"), (DELTS*&)args, nval_str))
	return false;
if (!Arg::done((DELTS*)args, _T("attrchange"),parse))
	return false;

if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrchange: Warning. Given no attr name.]"));// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (!hier_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[attrchange: Warning. Given no concept.]"));	// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[attrchange: Bad semantic arg.]"));				// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

cg->attrChange(hier1, attr_str, val_str, nval_str);
//	return false;			// 04/28/00 AM.

// Return appropriate value.
//sem = new RFASem(val);

// sem = true/false;
return true;
}


/********************************************
* FN:		FNDOWN
* CR:		03/02/00 AM.
* SUBJ:	Follow down link of concept.
* RET:	True if ok, else false.
*			UP conc - child concept in hierarchy.
* FORM:	down(hierconcept)
********************************************/

bool Fn::fnDown(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *hier_sem = 0;

if (!Arg::sem1(_T("down"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("down"),parse))
	return false;

if (!hier_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[down: Warning. Given no concept.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[down: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Down(hier1);
//	return false;		// 04/28/00 AM.

if (!conc)															// FIX.	// 10/10/00 AM.
	return true;													// FIX.	// 10/10/00 AM.

// Return appropriate value.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNUP
* CR:		03/02/00 AM.
* SUBJ:	Follow up link of concept.
* RET:	True if ok, else false.
*			UP conc - parent concept in hierarchy.
* FORM:	up(hierconcept)
********************************************/

bool Fn::fnUp(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *hier_sem = 0;

if (!Arg::sem1(_T("up"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("up"),parse))
	return false;

if (!hier_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[up: Warning. Given no concept.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[up: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Up(hier1);
//	return false;			// 04/28/00 AM.

// Return appropriate value.
if (conc)															// FIX.	// 11/15/00 AM.
	sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNNEXT
* CR:		03/02/00 AM.
* SUBJ:	Follow next link of concept.
* RET:	True if ok, else false.
*			UP conc - next concept in hierarchy.
* FORM:	next(hierconcept)
********************************************/

bool Fn::fnNext(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *hier_sem = 0;

if (!Arg::sem1(_T("next"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("next"),parse))
	return false;

if (!hier_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[next: Warning. Given no concept.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[next: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Next(hier1);
//	return false;				// 04/28/00 AM.

if (!conc)															// FIX.	// 10/10/00 AM.
	return true;													// FIX.	// 10/10/00 AM.
// Return appropriate value.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNPREV
* CR:		03/02/00 AM.
* SUBJ:	Follow prev link of concept.
* RET:	True if ok, else false.
*			UP conc - prev concept in hierarchy.
* FORM:	prev(hierconcept)
********************************************/

bool Fn::fnPrev(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *hier_sem = 0;

if (!Arg::sem1(_T("prev"),nlppp,(DELTS*&)args,hier_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("prev"),parse))
	return false;

if (!hier_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[prev: Warning. Given no concept.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (hier_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[prev: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *hier1 = hier_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->Prev(hier1);
//	return false;					// 04/28/00 AM.

if (!conc)															// FIX.	// 10/10/00 AM.
	return true;													// FIX.	// 10/10/00 AM.

// Return appropriate value.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNNEXTATTR
* CR:		03/02/00 AM.
* SUBJ:	Follow next link in list of attributes.
* RET:	True if ok, else false.
*			UP attr - next in list.
* FORM:	nextattr(attr)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
********************************************/

bool Fn::fnNextattr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *attr_sem = 0;

if (!Arg::sem1(_T("nextattr"),nlppp,(DELTS*&)args, attr_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("nextattr"),parse))
	return false;

if (!attr_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[nextattr: Warning. Given no attr.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (attr_sem->getType() != RS_KBATTR)
	{
	_stprintf(Errbuf,_T("[nextattr: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
ATTR *attr = attr_sem->getKBattr();
if (!attr)
	{
	// Do I need to build an empty sem object?
	sem = new RFASem((ATTR *)0, RS_KBATTR);
	return true;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

cg->popAttr(/*DU*/ attr);	// Move to next attr in list.

// Return appropriate value.
sem = new RFASem(attr, RS_KBATTR);

return true;
}


/********************************************
* FN:		FNNEXTVAL
* CR:		03/02/00 AM.
* SUBJ:	Follow next link in list of attr values.
* RET:	True if ok, else false.
*			UP val - next value in list.
* FORM:	nextval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
********************************************/

bool Fn::fnNextval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *val_sem = 0;

if (!Arg::sem1(_T("nextval"),nlppp,(DELTS*&)args, val_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("nextval"),parse))
	return false;

if (!val_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[nextval: Warning. Given no val.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_stprintf(Errbuf,_T("[nextval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	// Todo: Do I need to build an empty semantic object?
	sem = new RFASem((VAL *)0, RS_KBVAL);
	return true;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

val = cg->nextVal(val);	// Move to next val in list.

// Return appropriate value.
sem = new RFASem(val, RS_KBVAL);

return true;
}


/********************************************
* FN:		FNGETSVAL
* CR:		03/02/00 AM.
* SUBJ:	Fetch or convert value to string.
* RET:	True if ok, else false.
*			UP str - Value converted to string.
* FORM:	getsval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popSval.
********************************************/

bool Fn::fnGetsval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *val_sem = 0;

if (!Arg::sem1(_T("getsval"),nlppp,(DELTS*&)args, val_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("getsval"),parse))
	return false;

if (!val_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[getsval: Warning. Given no val.]"));			// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_stprintf(Errbuf,_T("[getsval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	// Todo: Do I need to build an empty semantic object?
	sem = new RFASem((_TCHAR *)0, RSSTR);
	return true;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

_TCHAR buf[MAXSTR];
cg->popSval(val, /*UP*/ buf);
if (!buf || !*buf)												// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return appropriate value.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNGETSTRVAL
* CR:		03/02/00 AM.
* SUBJ:	Fetch string value.
* RET:	True if ok, else false.
*			UP str - String obtained from value.
* FORM:	getstrval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popVal.  Differs from getsval in that no
*			conversion is done. Value must be string-valued.
********************************************/

bool Fn::fnGetstrval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *val_sem = 0;

if (!Arg::sem1(_T("getstrval"),nlppp,(DELTS*&)args, val_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("getstrval"),parse))
	return false;

if (!val_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[getstrval: Warning. Given no val.]"));		// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_stprintf(Errbuf,_T("[getstrval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	// Todo: Do I need to build an empty semantic object?
	sem = new RFASem((_TCHAR *)0, RSSTR);
	return true;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

_TCHAR buf[MAXLINE];
cg->popVal(val, /*UP*/ buf);
if (!buf || !*buf)												// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return appropriate value.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNGETNUMVAL
* CR:		03/02/00 AM.
* SUBJ:	Fetch numeric value.
* RET:	True if ok, else false.
*			UP num - Long obtained from value.
* FORM:	getnumval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popVal.
********************************************/

bool Fn::fnGetnumval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *val_sem = 0;

if (!Arg::sem1(_T("getnumval"),nlppp,(DELTS*&)args, val_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("getnumval"),parse))
	return false;

if (!val_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[getnumval: Warning. Given no val.]"));		// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_stprintf(Errbuf,_T("[getnumval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	// Todo: Do I need to build an empty semantic object?
	sem = new RFASem((_TCHAR *)0, RSLONG);
	return true;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

long num;
cg->popVal(val, /*UP*/ num);

// Return appropriate value.
sem = new RFASem(num);

return true;
}


/********************************************
* FN:		FNGETCONVAL
* CR:		08/12/00 AM.
* SUBJ:	Fetch concept value.
* RET:	True if ok, else false.
*			UP con - Concept obtained from value.
* FORM:	getconval(val)
* NOTE:	Not doing "pop" functions because NLP++ has no call-by-reference.
*			Not equivalent to popVal.
********************************************/

bool Fn::fnGetconval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *val_sem = 0;

if (!Arg::sem1(_T("getconval"),nlppp,(DELTS*&)args, val_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("getconval"),parse))
	return false;

if (!val_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[getconval: Warning. Given no val.]"));		// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (val_sem->getType() != RS_KBVAL)
	{
	_stprintf(Errbuf,_T("[getconval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
VAL *val = val_sem->getKBval();
if (!val)
	{
	//sem = new RFASem((char *)0, RSLONG);					// FIX.	// 11/15/00 AM.
	return true;
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc;
cg->popVal(val, /*UP*/ conc);

// Return appropriate value.
if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNMAKECONCEPT
* CR:		03/03/00 AM.
* SUBJ:	Make concept under parent.
* RET:	True if ok, else false.
*			UP conc - the new concept.
* FORM:	makeconcept(parent_conc, name_str [, pos_num])
*			parent_conc - the parent concept.
*			name_str - name of new concept.
*			pos_num - position in childs.  If absent or 0, placed at end of list.
********************************************/

bool Fn::fnMakeconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;		// Parent concept.
_TCHAR *name_str=0;			// New concept's name.
long pos_num = 0;			// Optional position.

if (!Arg::sem1(_T("makeconcept"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("makeconcept"), (DELTS*&)args, name_str))
	return false;
// Get optional numeric arg here.
if (!Arg::num1(_T("makeconcept"), (DELTS*&)args, /*UP*/ pos_num, true))
	pos_num = 0;	// Optional arg was missing.
if (!Arg::done((DELTS*)args, _T("makeconcept"),parse))
	return false;

if (!name_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf, _T("[makeconcept: Warning. Given no name.]"));	// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}
if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf, _T("[makeconcept: Warning. Given no parent.]"));// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf, _T("[makeconcept: Bad semantic arg.]"));			// 05/17/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/17/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->makeConcept(conc1, name_str, pos_num);

// Return as str type.
if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNADDCONCEPT
* CR:		03/03/00 AM.
* SUBJ:	Add concept under parent.
* RET:	True if ok, else false.
*			UP conc - the new concept.
* FORM:	addconcept(parent_conc, child_conc [, pos_num])
*			parent_conc - the parent concept.
*			child_con - DETACHED concept to add.
*			pos_num - position in childs.  If absent or 0, placed at end of list.
* WARN:	DON'T USE THIS FOR A CONCEPT ALREADY IN THE HIERARCHY.
********************************************/

bool Fn::fnAddconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;		// Parent concept.
RFASem *child_sem = 0;	// Concept to add.
long pos_num = 0;			// Optional position.

if (!Arg::sem1(_T("addconcept"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::sem1(_T("addconcept"),nlppp,(DELTS*&)args, child_sem))
	return false;
// Get optional numeric arg here.
if (!Arg::num1(_T("addconcept"), (DELTS*&)args, /*UP*/ pos_num, true))
	pos_num = 0;	// Optional arg was missing.
if (!Arg::done((DELTS*)args, _T("addconcept"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addconcept: Warning. Given no concept.]"));	// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!child_sem)																// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addconcept: Warning. Given no child.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addconcept: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *parent = con_sem->getKBconcept();
if (child_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addconcept: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *child = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->addConcept(parent, child, pos_num);

if (!conc)	// Error in adding concept.					// FIX.	// 08/07/00 AM.
	{
	_stprintf(Errbuf,_T("[addconcept: Couldn't add concept.]"));		// 08/07/00 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Return as str type.
if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNADDATTR
* CR:		02/22/00 AM.
* SUBJ:	Add an empty attribute to concept.
* RET:	True if ok, else false.
*			UP attr - new attribute object with no values.
* FORM:	addattr(concept, attrstr)
* NOTE:	03/04/00 AM. Redoing this to conform to cg.h .
********************************************/

bool Fn::fnAddattr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;
_TCHAR *name_str=0;

if (!Arg::sem1(_T("addattr"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("addattr"), (DELTS*&)args, name_str))
	return false;
if (!Arg::done((DELTS*)args, _T("addattr"),parse))
	return false;

if (!name_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addattr: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addattr: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addattr: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

ATTR *attr = cg->addAttr(conc1, name_str);

// Return as str type.
sem = new RFASem(attr, RS_KBATTR);

return true;
}


/********************************************
* FN:		FNADDSVAL
* CR:		03/03/00 AM.
* SUBJ:	Add num value as a STRING to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addsval(concept, attrname, num)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnAddsval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
long num=0;

if (!Arg::sem1(_T("addsval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("addsval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::num1(_T("addsval"), (DELTS*&)args, num))
	return false;
if (!Arg::done((DELTS*)args, _T("addsval"),parse))
	return false;

if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addsval: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addsval: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addsval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

cg->addSval(conc1, attr_str, num);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

return true;
}


/********************************************
* FN:		FNADDSTRVAL
* CR:		03/03/00 AM.
* SUBJ:	Add string to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addstrval(concept, attrname, str)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnAddstrval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
_TCHAR *str=0;

if (!Arg::sem1(_T("addstrval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("addstrval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::str1(_T("addstrval"), (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("addstrval"),parse))
	return false;

if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addstrval: Warning. Given no attr name.]"));// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}
if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addstrval: Warning. Given no str value.]"));// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addstrval: Warning. Given no concept.]"));	// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addstrval: Bad semantic arg.]"));				// 05/17/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

cg->addVal(conc1, attr_str, str);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

return true;
}


/********************************************
* FN:		FNADDNUMVAL
* CR:		03/03/00 AM.
* SUBJ:	Add num value to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addnumval(concept, attrname, num)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnAddnumval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
long num=0;

if (!Arg::sem1(_T("addnumval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("addnumval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::num1(_T("addnumval"), (DELTS*&)args, num))
	return false;
if (!Arg::done((DELTS*)args, _T("addnumval"),parse))
	return false;

if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addnumval: Warning. Given no attr name.]"));// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}
if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addnumval: Warning. Given no concept.]"));	// 05/17/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/17/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addnumval: Bad semantic arg.]"));				// 05/17/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/17/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

cg->addVal(conc1, attr_str, num);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

return true;
}


/********************************************
* FN:		FNADDCONVAL
* CR:		08/11/00 AM.
* SUBJ:	Add concept value to attribute's values.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	addconval(concept, attrname, val_con)
* NOTE:	KB assumed to have been read in.
********************************************/

bool Fn::fnAddconval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
RFASem *val_sem = 0;

if (!Arg::sem1(_T("addconval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("addconval"), (DELTS*&)args, attr_str))
	return false;
if (!Arg::sem1(_T("addconval"),nlppp,(DELTS*&)args, val_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("addconval"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addconval: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addconval: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!val_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addconval: Warning. Given no concept val.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addconval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

if (val_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addconval: Bad semantic arg(2).]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *val1 = val_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

cg->addVal(conc1, attr_str, val1);

// Return as val type.
//sem = new RFASem(val, RS_KBVAL);

return true;
}


/********************************************
* FN:		FNGETCONCEPT
* CR:		03/03/00 AM.
* SUBJ:	Find or make concept under parent.
* RET:	True if ok, else false.
*			UP conc - the new concept.
* FORM:	getconcept(parent_conc, name_str)
*			parent_conc - the parent concept.
*			name_str - name of new concept.
********************************************/

bool Fn::fnGetconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;		// Parent concept.
_TCHAR *name_str=0;			// New concept's name.

if (!Arg::sem1(_T("getconcept"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("getconcept"), (DELTS*&)args, name_str))
	return false;
if (!Arg::done((DELTS*)args, _T("getconcept"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[getconcept: Warning. Given no parent.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[getconcept: Warning. Given no name.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[getconcept: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *conc = cg->getConcept(conc1, name_str);

// Return as str type.
if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNRMCONCEPT
* CR:		03/03/00 AM.
* SUBJ:	Delete given concept.
* RET:	True if ok, else false.
* FORM:	rmconcept(conc)
********************************************/

bool Fn::fnRmconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;		// Concept.

if (!Arg::sem1(_T("rmconcept"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("rmconcept"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmconcept: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmconcept: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

bool flag = cg->rmConcept(conc1);
long ok = (flag ? 1 : 0);										// FIX.	// 02/06/01 AM.
sem = new RFASem(ok);											// FIX.	// 02/06/01 AM.
return true;
}


/********************************************
* FN:		FNRMCHILD
* CR:		03/03/00 AM.
* SUBJ:	Remove concept under given parent.
* RET:	True if ok, else false.
* FORMS:	rmchild(parent, namestr)
*			rmchild(parent, num)
* NOTE:	Handling TWO VARIANTS here.
*			Differs from cg.h, which uses rmConcept for three variants.
********************************************/

bool Fn::fnRmchild(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *name1=0;
long num1= -1;

if (!Arg::sem1(_T("rmchild"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str_or_num1(_T("rmchild"), /*UP*/ (DELTS*&)args, name1, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmchild"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmchild: Warning. Given no parent.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmchild: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();
CONCEPT *child = 0;

long ok;																			// 02/06/01 AM.
bool flag;																		// 02/06/01 AM.
if (name1 && *name1)		// 2nd arg was name.
	{
	flag = cg->rmConcept(conc1, name1);
	ok = (flag ? 1 : 0);														// 02/06/01 AM.
	}
else							// 2nd arg was num.
	{
	flag = cg->rmConcept(conc1, num1);
	ok = (flag ? 1 : 0);														// 02/06/01 AM.
	}
sem = new RFASem(ok);														// 02/06/01 AM.
return true;																	// 02/06/01 AM.
}


/********************************************
* FN:		FNRMVALS
* CR:		03/03/00 AM.
* SUBJ:	Delete values of concept's attribute.
* RET:	True if ok, else false.
* FORMS:	rmvals(conc, namestr)
* NOTE:	RENAMED FROM RMVAL.  05/11/00 AM.
********************************************/

bool Fn::fnRmvals(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *name1=0;

if (!Arg::sem1(_T("rmvals"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("rmvals"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmvals"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmvals: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmvals: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmvals: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

bool ok = cg->rmVals(conc1, name1);										// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRMVAL
* CR:		03/03/00 AM.
* SUBJ:	Delete value of concept's attribute.
* RET:	True if ok, else false.
* FORMS:	rmval(attr, val)
********************************************/

bool Fn::fnRmval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
RFASem *sem2;

if (!Arg::sem1(_T("rmval"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::sem1(_T("rmval"),nlppp, /*UP*/ (DELTS*&)args, sem2))
	return false;
if (!Arg::done((DELTS*)args, _T("rmval"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmval: Warning. Given no attr.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!sem2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmval: Warning. Given no val.]"));				// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get attr from sem.
if (sem1->getType() != RS_KBATTR)
	{
	_stprintf(Errbuf,_T("[rmval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
ATTR *attr = sem1->getKBattr();

// Get val from sem.
if (sem2->getType() != RS_KBVAL)
	{
	_stprintf(Errbuf,_T("[rmval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
VAL *val = sem2->getKBval();

bool ok = cg->rmVal(attr, val);											// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRMATTR
* CR:		03/03/00 AM.
* SUBJ:	Delete concept's attribute.
* RET:	True if ok, else false.
* FORMS:	rmattr(conc, namestr)
* NOTE:	Not handling the form rmattr(conc, attr) yet.
********************************************/

bool Fn::fnRmattr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *name1=0;

if (!Arg::sem1(_T("rmattr"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("rmattr"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmattr"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmattr: Warning. Given no concept.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmattr: Warning. Given no attr name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmattr: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

bool ok = cg->rmAttr(conc1, name1);										// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRMATTRVAL
* CR:		05/17/00 AM.
* SUBJ:	Delete concept's attribute value.
* RET:	True if ok, else false.
* FORMS:	rmattr(conc, attr_s, val_s)
********************************************/

bool Fn::fnRmattrval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *attr_s=0;
_TCHAR *val_s=0;

if (!Arg::sem1(_T("rmattrval"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("rmattrval"), /*UP*/ (DELTS*&)args, attr_s))
	return false;
if (!Arg::str1(_T("rmattrval"), /*UP*/ (DELTS*&)args, val_s))
	return false;
if (!Arg::done((DELTS*)args, _T("rmattrval"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmattrval: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_s)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmattrval: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!val_s)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmattrval: Warning. Given no val str.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmattrval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc = sem1->getKBconcept();

bool ok = cg->rmAttrval(conc, attr_s, val_s);						// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRMCHILDREN
* CR:		03/03/00 AM.
* SUBJ:	Delete given concept's children AND PHRASE.
* RET:	True if ok, else false.
* FORM:	rmchildren(conc)
********************************************/

bool Fn::fnRmchildren(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;		// Concept.

if (!Arg::sem1(_T("rmchildren"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("rmchildren"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmchildren: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmchildren: Bad semantic arg.]"));
	return false;
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

cg->rmChildren(conc1);

return true;
}


/********************************************
* FN:		FNPRUNEPHRASES
* CR:		03/03/00 AM.
* SUBJ:	Remove all phrases from given subtree.
* RET:	True if ok, else false.
* FORM:	prunephrases(conc)
********************************************/

bool Fn::fnPrunephrases(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *con_sem = 0;		// Concept.

if (!Arg::sem1(_T("prunephrases"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::done((DELTS*)args, _T("prunephrases"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[prunephrases: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[prunephrases: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

bool ok = cg->prunePhrases(conc1);										// 05/29/01 AM.
sem = new RFASem( ok ? (long)1 : (long)0);							// 05/29/01 AM.

return true;
}


/********************************************
* FN:		FNRMWORD
* CR:		07/17/00 AM.
* SUBJ:	Remove word from kb dictionary.
* RET:	True if ok, else false.
* FORMS:	rmword(str)
* NOTE:	Not worrying about lowercase variants, etc.
********************************************/

bool Fn::fnRmword(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("rmword"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmword"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmword: Warning. Given no word.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

bool ok = cg->rmWord(name1);
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 02/03/01 AM.
sem = new RFASem(num);														// 02/03/01 AM.
return true;
}


/********************************************
* FN:		FNREPLACEVAL
* CR:		03/03/00 AM.
* SUBJ:	Replace attribute value with given value.
* RET:	True if ok, else false.
*			UP  Todo: val - the added value (sic).
* FORM:	replaceval(concept, attrname, str)
*			replaceval(concept, attrname, num)
*			replaceval(concept, attrname, con)
* NOTE:	09/07/00 AM. FIX. Getting third arg was mangled.
********************************************/

bool Fn::fnReplaceval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
_TCHAR *str=0;
long num= -1;
RFASem *val_sem = 0;															// 08/12/00 AM.
float flt = 0;	// 12/26/06 AM.
enum Iargtype typ;	// 12/26/06 AM.

if (!Arg::sem1(_T("replaceval"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(																// 08/12/00 AM.
					_T("replaceval"), /*UP*/ (DELTS*&)args, attr_str))	// FIX.	// 09/07/00 AM.
	return false;
if (!Arg::any1(_T("replaceval"),parse,(DELTS*&)args, /*UP*/ typ,str,num,flt,val_sem))	// 09/07/00 AM.
	return false;
if (!Arg::done((DELTS*)args, _T("replaceval"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[replaceval: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[replaceval: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[replaceval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();


bool ok = false;																// 05/25/01 AM.

if (str && *str)		// Replacing with string.
	ok = cg->replaceVal(conc1, attr_str, str);						// 05/25/01 AM.
else if (!val_sem)															// 08/12/00 AM.
	{
	if (typ == IAFLOAT)	// 12/26/06 AM.
		ok = cg->replaceVal(conc1, attr_str, flt);						// 12/26/06 AM.
	else
		ok = cg->replaceVal(conc1, attr_str, num);						// 05/25/01 AM.
	}
else						// Val is a sem.									// 08/12/00 AM.
	{
	if (val_sem->getType() != RS_KBCONCEPT)							// 08/12/00 AM.
		{
		_stprintf(Errbuf,_T("[replaceval: Bad semantic value arg.]"));
		return parse->errOut(false); // UNFIXED 												// 05/18/01 AM.
		}
	CONCEPT *val_conc = val_sem->getKBconcept();						// 08/12/00 AM.

	ok = cg->replaceVal(conc1, attr_str, val_conc);					// 05/25/01 AM.
	}

long numb = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(numb);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRENAMECONCEPT
* CR:		03/03/00 AM.
* SUBJ:	Rename given concept.
* RET:	True if ok, else false.
* FORM:	renameconcept(concept, name)
********************************************/

bool Fn::fnRenameconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *str=0;

if (!Arg::sem1(_T("renameconcept"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("renameconcept"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("renameconcept"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renameconcept: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renameconcept: Warning. Given no name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[renameconcept: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

bool ok = cg->renameConcept(conc1, str);								// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRENAMECHILD
* CR:		03/03/00 AM.
* SUBJ:	Rename nth child of given concept.
* RET:	True if ok, else false.
* FORM:	renamechild(parent, ord, name)
* NOTE:	In cg.h, this is a variant of renameConcept.
********************************************/

bool Fn::fnRenamechild(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
long num = -1;
_TCHAR *str=0;

if (!Arg::sem1(_T("renamechild"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::num1(_T("renamechild"), /*UP*/ (DELTS*&)args, num))
	return false;
if (!Arg::str1(_T("renamechild"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("renamechild"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renamechild: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renmaechild: Warning. Given no name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[renamechild: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

bool ok = cg->renameConcept(conc1, num, str);						// 05/25/01 AM.
long numb = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(numb);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRENAMEATTR
* CR:		03/03/00 AM.
* SUBJ:	Rename an attribute.
* RET:	True if ok, else false.
* FORM:	renameattr(concept, attrname, newname)
* NOTE:	Not doing the form that takes an attr object.
********************************************/

bool Fn::fnRenameattr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *attr_str=0;
_TCHAR *str=0;

if (!Arg::sem1(_T("renameattr"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("renameattr"), /*UP*/ (DELTS*&)args, attr_str))
	return false;
if (!Arg::str1(_T("renameattr"), (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("renameattr"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renameattr: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!attr_str)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renameattr: Warning. Given no attr name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renameattr: Warning. Given no new name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[renameattr: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

bool ok = cg->renameAttr(conc1, attr_str, str);						// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNMOVECLEFT
* CR:		03/03/00 AM.
* SUBJ:	Bubble concept left in list.
* RET:	True if ok, else false.
* FORM:	movecleft(concept)
********************************************/

bool Fn::fnMovecleft(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("movecleft"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("movecleft"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[movecleft: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[movecleft: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

// Don't error out NLP++ if can't move concept left.				// 09/07/00 AM.
if (!cg->moveCleft(conc1))
	{
	_stprintf(Errbuf,_T("[movecleft: Couldn't move concept left.]"));	// 05/18/01 AM.
	parse->errOut(true); // UNFIXED 																// 05/18/01 AM.
	}
return true;																	// 09/07/00 AM.
}


/********************************************
* FN:		FNMOVECRIGHT
* CR:		03/03/00 AM.
* SUBJ:	Bubble concept right in list.
* RET:	True if ok, else false.
* FORM:	movecright(concept)
********************************************/

bool Fn::fnMovecright(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("movecright"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("movecright"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[movecright: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[movecright: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

// Don't error out NLP++ if can't move concept right.				// 09/07/00 AM.
if (!cg->moveCright(conc1))
	{
	_stprintf(Errbuf,_T("[movecright: Couldn't move concept right.]"));//05/18/01 AM.
	parse->errOut(true); // UNFIXED 																// 05/18/01 AM.
	}
return true;																	// 09/07/00 AM.
}


/********************************************
* FN:		FNFINDPHRASE
* CR:		03/03/00 AM.
* SUBJ:	Find concept's phrase.
* RET:	True if ok, else false.
*			UP phrase - phrase found.
* FORM:	findphrase(concept)
********************************************/

bool Fn::fnFindphrase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("findphrase"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("findphrase"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findphrase: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[findphrase: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

PHRASE *phr = cg->findPhrase(conc1);

// Return as str type.
sem = new RFASem(phr, RS_KBPHRASE);
return true;
}


/********************************************
* FN:		FNSORTPHRASE
* CR:		03/03/00 AM.
* SUBJ:	Sort concept's phrase.
* RET:	True if ok, else false.
* FORM:	sortphrase(concept)
********************************************/

bool Fn::fnSortphrase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("sortphrase"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("sortphrase"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[sortphrase: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[sortphrase: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

bool ok = cg->sortPhrase(conc1);											// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}



/********************************************
* FN:		FNSORTCHILDS
* CR:		02/20/09 AM.
* SUBJ:	Sort concept's immediate children.
* RET:	True if ok, else false.
* FORM:	sortchilds(concept)
********************************************/

bool Fn::fnSortchilds(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("sortchilds"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("sortchilds"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[sortchilds: Warning. Given no concept.]"));
	return parse->errOut(true); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[sortchilds: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc1 = sem1->getKBconcept();

bool ok = cg->sortChilds(conc1);
long num = (ok ? 1 : 0);		// "Convert" to boolnum.
sem = new RFASem(num);			// Return 1/0 for boolean.
return true;
}


/********************************************
* FN:		FNSORTHIER
* CR:		02/20/09 AM.
* SUBJ:	Sort concept's subhierarchy.
* RET:	True if ok, else false.
* FORM:	sortchilds(concept)
********************************************/

bool Fn::fnSorthier(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("sorthier"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("sorthier"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[sorthier: Warning. Given no concept.]"));
	return parse->errOut(true); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[sorthier: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc1 = sem1->getKBconcept();

bool ok = cg->sortHier(conc1);
long num = (ok ? 1 : 0);		// "Convert" to boolnum.
sem = new RFASem(num);			// Return 1/0 for boolean.
return true;
}


/********************************************
* FN:		FNPHRASELENGTH
* CR:		03/03/00 AM.
* SUBJ:	Get length of phrase.
* RET:	True if ok, else false.
*			UP len - length of phrase.
* FORM:	phraselength(concept)
********************************************/

bool Fn::fnPhraselength(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("phraselength"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("phraselength"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[phraselength: Warning. Given no concept.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[phraselength: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();

long len = cg->phraseLength(conc1);

sem = new RFASem(len);
return true;
}


/********************************************
* FN:		FNNODECONCEPT
* CR:		03/03/00 AM.
* SUBJ:	Get node's owning concept.
* RET:	True if ok, else false.
*			UP conc - owning concept.
* FORM:	nodeconcept(node_con)
********************************************/

bool Fn::fnNodeconcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("nodeconcept"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("nodeconcept"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[nodeconcept: Warning. Given no node.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[nodeconcept: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *node = sem1->getKBconcept();

CONCEPT *conc = cg->nodeConcept(node);

if (conc)															// FIX.	// 11/15/00 AM.
sem = new RFASem(conc, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNNODEOWNER
* CR:		12/03/02 AM.
* SUBJ:	Get node's owning concept.
* RET:	True if ok, else false.
*			UP conc - owning concept.
* FORM:	nodeconcept(node_con)
********************************************/

bool Fn::fnNodeowner(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("nodeowner"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("nodeowner"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[nodeowner: Warning. Given no node.]"));
	return parse->errOut(true); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[nodeowner: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *node = sem1->getKBconcept();

CONCEPT *conc = cg->nodeOwner(node);

if (conc)
sem = new RFASem(conc, RS_KBCONCEPT);
return true;
}

/********************************************
* FN:		FNFINDNODE
* CR:		03/03/00 AM.
* SUBJ:	Find node in phrase.
* RET:	True if ok, else false.
*			UP node - node if found.
* FORMS:	findnode(phrase, namestr) # Returns first match of name.
*			findnode(phrase, ord)   # Nth node.  If 0, then last node.
* NOTE:	Handling TWO VARIANTS here.
********************************************/

bool Fn::fnFindnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *name1=0;
long num1= -1;

if (!Arg::sem1(_T("findnode"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str_or_num1(_T("findnode"), /*UP*/ (DELTS*&)args, name1, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("findnode"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[findnode: Warning. Given no phrase.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_stprintf(Errbuf,_T("[findnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
PHRASE *phr1 = sem1->getKBphrase();
CONCEPT *node = 0;

if (name1 && *name1)		// 2nd arg was name.
	node = cg->findNode(phr1, name1);
else							// 2nd arg was num.
	node = cg->findNode(phr1, num1);

if (node)															// FIX.	// 11/15/00 AM.
sem = new RFASem(node, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNLISTNODE
* CR:		03/03/00 AM.
* SUBJ:	Get first node in list of given node.
* RET:	True if ok, else false.
*			UP conc - first node in list.
* FORM:	listnode(node_con)
********************************************/

bool Fn::fnListnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("listnode"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("listnode"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[listnode: Warning. Given no node.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[listnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *node = sem1->getKBconcept();

CONCEPT *first = cg->listNode(node);

if (first)															// FIX.	// 11/15/00 AM.
	sem = new RFASem(first, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNFIRSTNODE
* CR:		03/03/00 AM.
* SUBJ:	Get first node of phrase.
* RET:	True if ok, else false.
*			UP node - first node of phrase.
* FORMS:	firstnode(phrase)
********************************************/

bool Fn::fnFirstnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("firstnode"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("firstnode"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[firstnode: Warning. Given no phrase.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_stprintf(Errbuf,_T("[firstnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
PHRASE *phr1 = sem1->getKBphrase();

CONCEPT *node = cg->firstNode(phr1);

if (node)															// FIX.	// 11/15/00 AM.
sem = new RFASem(node, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNLASTNODE
* CR:		03/03/00 AM.
* SUBJ:	Get last node of phrase.
* RET:	True if ok, else false.
*			UP node - last node of phrase.
* FORMS:	lastnode(phrase)
********************************************/

bool Fn::fnLastnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("lastnode"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("lastnode"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[lastnode: Warning. Given no phrase.]"));		// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_stprintf(Errbuf,_T("[lastnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
PHRASE *phr1 = sem1->getKBphrase();

CONCEPT *node = cg->lastNode(phr1);

if (node)															// FIX.	// 11/15/00 AM.
sem = new RFASem(node, RS_KBCONCEPT);
return true;
}


/********************************************
* FN:		FNMAKEPHRASE
* CR:		03/03/00 AM.
* SUBJ:	Create phrase for concept by adding first node.
* RET:	True if ok, else false.
*			UP phr - phrase.
* FORM:	makephrase(concept, name)
********************************************/

bool Fn::fnMakephrase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *str=0;

if (!Arg::sem1(_T("makephrase"),nlppp,(DELTS*&)args,con_sem))
	return false;
if (!Arg::str1(_T("makephrase"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("makephrase"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[makephrase: Warning. Given no concept.]"));	// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[makephrase: Warning. Given no node name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[makephrase: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

PHRASE *phr = cg->makePhrase(conc1, str);
sem = new RFASem(phr, RS_KBPHRASE);
return true;
}


/********************************************
* FN:		FNADDCNODE
* CR:		03/03/00 AM.
* SUBJ:	Add node to end of concept's phrase.
* RET:	True if ok, else false.
*			UP node - created node-concept.
* FORM:	addcnode(concept, name)
*			addcnode(concept, nconcept) -- Hier concept that node will be a proxy for.
*				11/27/02 AM.
********************************************/

bool Fn::fnAddcnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *con_sem = 0;
_TCHAR *str=0;
long num = -1;
float flt = -1.0;																// 11/27/02 AM.
enum Iargtype typ = IANULL;												// 11/27/02 AM.
RFASem *sem1 = 0;																// 11/27/02 AM.

if (!Arg::sem1(_T("addcnode"),nlppp,(DELTS*&)args,con_sem))
	return false;

if (!Arg::any1(_T("str"),parse,/*DU*/(DELTS*&)args,typ,str,num,flt,sem1))			// 11/27/02 AM.
	return false;
if (!Arg::done((DELTS*)args, _T("addcnode"),parse))
	return false;

if (!con_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addcnode: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (con_sem->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[addcnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = con_sem->getKBconcept();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

CONCEPT *node=0;																// 11/27/02 AM.
switch (typ)																	// 11/27/02 AM.
	{
	case IASTR:																	// 11/27/02 AM.
		node = cg->addCnode(conc1, str);
		break;
	case IASEM:																	// 11/27/02 AM.
		node = cg->addCnode(conc1,sem1->getKBconcept());			// 11/27/02 AM.
		break;
	default:
		{
		_stprintf(Errbuf,_T("[addcnode: Warning. Need str or con for node.]"));	// 11/27/02 AM.
		return parse->errOut(true); // UNFIXED 										// 11/27/02 AM.
		}
	}

if (node)															// FIX.	// 11/15/00 AM.
	sem = new RFASem(node, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:		FNADDNODE
* CR:		03/03/00 AM.
* SUBJ:	Add node to nth position of given phrase.
* RET:	True if ok, else false.
*			UP node - created node-concept.
* FORM:	addnode(phrase, name, pos)
* NOTE:	Ignoring the form with a detached node.
********************************************/

bool Fn::fnAddnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *phr_sem = 0;
_TCHAR *str=0;
long num= -1;

if (!Arg::sem1(_T("addnode"),nlppp,(DELTS*&)args, phr_sem))
	return false;
if (!Arg::str1(_T("addnode"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::num1(_T("addnode"), /*UP*/ (DELTS*&)args, num))
	return false;
if (!Arg::done((DELTS*)args, _T("addnode"),parse))
	return false;

if (!phr_sem)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addnode: Warning. Given no phrase.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addnode: Warning. Given no node name.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (phr_sem->getType() != RS_KBPHRASE)
	{
	_stprintf(Errbuf,_T("[addnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
PHRASE *phr = phr_sem->getKBphrase();

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();

CONCEPT *node = cg->addNode(phr, str, num);
if (node)															// FIX.	// 11/15/00 AM.
sem = new RFASem(node, RS_KBCONCEPT);
return true;
}

// Skipping some NODE MOVEMENT functions.  Not useful for now.	// 03/03/00 AM.



/********************************************
* FN:		FNRMNODE
* CR:		03/03/00 AM.
* SUBJ:	Remove node from its phrase.
* RET:	True if ok, else false.
* FORM:	rmnode(node_con)
* NOTE:	Ignoring all kinds of variants for now.
********************************************/

bool Fn::fnRmnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("rmnode"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmnode"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmnode: Warning. Given no node.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmnode: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *node = sem1->getKBconcept();

bool ok = cg->rmNode(node);												// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRMPHRASE
* CR:		03/03/00 AM.
* SUBJ:	Remove the given phrase.
* RET:	True if ok, else false.
* FORM:	rmphrase(phrase)
* WARN:	Caller should set phrase to zero.
********************************************/

bool Fn::fnRmphrase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("rmphrase"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmphrase"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmphrase: Warning. Given no phrase.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_stprintf(Errbuf,_T("[rmphrase: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
PHRASE *phr = sem1->getKBphrase();

bool ok = cg->rmPhrase(phr);												// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRMCPHRASE
* CR:		03/03/00 AM.
* SUBJ:	Remove given concept's phrase.
* RET:	True if ok, else false.
* FORM:	rmcphrase(conc)
********************************************/

bool Fn::fnRmcphrase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("rmcphrase"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmcphrase"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[rmcphrase: Warning. Given no concept.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmcphrase: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc = sem1->getKBconcept();

bool ok = cg->rmCphrase(conc);											// 05/25/01 AM.
long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:		FNRENAMENODE
* CR:		03/03/00 AM.
* SUBJ:	Rename node.
* RET:	True if ok, else false.
* FORMS:	renamenode(phrase, namestr, rename)
*			renamenode(phrase, ord, rename)
* NOTE:	Handling TWO VARIANTS here.
********************************************/

bool Fn::fnRenamenode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *name1=0;
long num1= -1;
_TCHAR *rename=0;

if (!Arg::sem1(_T("renamenode"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str_or_num1(_T("renamenode"), /*UP*/ (DELTS*&)args, name1, num1))
	return false;
if (!Arg::str1(_T("renamenode"), /*UP*/ (DELTS*&)args, rename))
	return false;
if (!Arg::done((DELTS*)args, _T("renamenode"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renamenode: Warning. Given no phrsae.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!rename)																	// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[renamenode: Warning. Given no new name.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get object from sem.
if (sem1->getType() != RS_KBPHRASE)
	{
	_stprintf(Errbuf,_T("[renamenode: Bad semantic arg.]"));
	return false;
	}
PHRASE *phr1 = sem1->getKBphrase();

bool ok = false;																// 05/25/01 AM.
if (name1 && *name1)		// 2nd arg was name.
	ok = cg->renameNode(phr1, name1, rename);							// 05/25/01 AM.
else							// 2nd arg was num.
	ok = cg->renameNode(phr1, num1, rename);							// 05/25/01 AM.

long num = (ok ? 1 : 0);		// "Convert" to boolnum.			// 05/25/01 AM.
sem = new RFASem(num);			// Return 1/0 for boolean.			// 05/25/01 AM.
return true;																	// 05/25/01 AM.
}


/********************************************
* FN:	   FNSTRCONTAINS
* CR:	   04/25/00 Dd.
* SUBJ: Is first string contained in the second?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strcontains(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Fn::fnStrcontains(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strcontains"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strcontains"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strcontains"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strcontains: Warning. Given no str1.]"));	// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strcontains: Warning. Given no str2.]"));	// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

long result = 0;		// bool false.										// 08/17/01 AM.

while (*name2) {
	if (!_tcsncmp(name1,name2,_tcsclen(name1))) {
		result = 1;																// 08/17/01 AM.
		break;
	}
	++name2;															// OPT.	// 11/29/00 AM.
}

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTRCONTAINSNOCASE
* CR:	   04/25/00 Dd.
* SUBJ: Is first string contained in the second without case?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strcontainsnocase(str1,str2)
* NOTE: This is not case sensitive
********************************************/

bool Fn::fnStrcontainsnocase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strcontainsnocase"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strcontainsnocase"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strcontainsnocase"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strcontainsnocase: Warning. Given no str1.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strcontainsnocase: Warning. Given no str2.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}


long result = 0;		// bool false.										// 08/17/01 AM.

while (*name2) {
	if (!strcmp_ni(name1,name2,_tcsclen(name1))) {
		result = 1;
		break;
	}
	++name2;															// OPT.	// 11/29/00 AM.
}

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTREQUAL
* CR:	   04/25/00 Dd.
* SUBJ: Strings Equal?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strequal(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Fn::fnStrequal(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strequal"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strequal"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strequal"),parse))
	return false;

#ifdef OLD_030711
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strequal: Warning. Given no str1.]"));		// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strequal: Warning. Given no str2.]"));		// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
#endif

long result;																	// 07/11/03 AM.
if (empty(name1) && empty(name2))										// 07/11/03 AM.
	result = 1;																	// 07/11/03 AM.
else if (empty(name1) || empty(name2))									// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else																				// 07/11/03 AM.
	result = !_tcscmp(name1,name2);										// 08/17/01 AM.

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTRNOTEQUAL
* CR:	   04/25/00 Dd.
* SUBJ: Strings Not Equal?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strnotequal(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Fn::fnStrnotequal(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strnotequal"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strnotequal"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strnotequal"),parse))
	return false;

#ifdef OLD_030711_
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strnotequal: Warning. Given no str1.]"));	// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strnotequal: Warning. Given no str2.]"));	// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
#endif

long result;																	// 07/11/03 AM.
if (empty(name1) && empty(name2))										// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else if (empty(name1) || empty(name2))									// 07/11/03 AM.
	result = 1;																	// 07/11/03 AM.
else																				// 07/11/03 AM.
	result = _tcscmp(name1,name2);											// 08/17/01 AM.

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTREQUALNOCASE
* CR:	   04/25/00 Dd.
* SUBJ: Strings Equal ignoring case?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strequalnocase(str1,str2)
* NOTE: This is not case sensitive
********************************************/

bool Fn::fnStrequalnocase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strequalnocase"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strequalnocase"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strequalnocase"),parse))
	return false;

#ifdef OLD_030711_
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strequalnocase: Warning. Given no str1.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strequalnocase: Warning. Given no str2.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
#endif

long result;																	// 07/11/03 AM.
if (empty(name1) && empty(name2))										// 07/11/03 AM.
	result = 1;																	// 07/11/03 AM.
else if (empty(name1) || empty(name2))									// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else																				// 07/11/03 AM.
	result = !strcmp_i(name1,name2);										// 08/17/01 AM.

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTRNOTEQUALNOCASE
* CR:	   04/25/00 Dd.
* SUBJ: Strings Not Equal ignoring case?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strnotequalnocase(str1,str2)
* NOTE: This is not case sensitive
********************************************/

bool Fn::fnStrnotequalnocase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strnotequalnocase"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strnotequalnocase"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strnotequalnocase"),parse))
	return false;

#ifdef OLD_030711_
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strnotequalnocase: Warning. Given no str1.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strnotequalnocase: Warning. Given no str2.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
#endif

long result;																	// 07/11/03 AM.
if (empty(name1) && empty(name2))										// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else if (empty(name1) || empty(name2))									// 07/11/03 AM.
	result = 1;																	// 07/11/03 AM.
else																				// 07/11/03 AM.
	result = strcmp_i(name1,name2);										// 08/17/01 AM.

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTRGREATERTHAN
* CR:	   04/25/00 Dd.
* SUBJ: First string is less than the second?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strgreaterthan(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Fn::fnStrgreaterthan(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strgreaterthan"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strgreaterthan"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strgreaterthan"),parse))
	return false;

#ifdef OLD_030711_
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strgreaterthan: Warning. Given no str1.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strgreaterthan: Warning. Given no str2.]"));// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
#endif

long result;																	// 07/11/03 AM.
if (empty(name1) && empty(name2))										// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else if (empty(name1))														// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else if (empty(name2))														// 07/11/03 AM.
	result = 1;																	// 07/11/03 AM.
else																				// 07/11/03 AM.
	result = (_tcscmp(name1,name2) > 0);									// 08/17/01 AM.


sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNSTRLESSTHAN
* CR:	   04/25/00 Dd.
* SUBJ: First string is less than the second?
* RET:  True if ok, else false.
*	 UP - result of string compare
* FORMS:	strlessthan(str1,str2)
* NOTE: This is case sensitive
********************************************/

bool Fn::fnStrlessthan(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *name2=0;

if (!Arg::str1(_T("strlessthan"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strlessthan"), /*UP*/ (DELTS*&)args, name2))
	return false;
if (!Arg::done((DELTS*)args, _T("strlessthan"),parse))
	return false;

#ifdef OLD_030711_
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strlessthan: Warning. Given no str1.]"));	// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name2)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strlessthan: Warning. Given no str2.]"));	// 05/18/01 AM.
	sem = new RFASem((long)0);												// 10/30/00 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
#endif

long result;																	// 07/11/03 AM.
if (empty(name1) && empty(name2))										// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else if (empty(name1))														// 07/11/03 AM.
	result = 1;																	// 07/11/03 AM.
else if (empty(name2))														// 07/11/03 AM.
	result = 0;																	// 07/11/03 AM.
else																				// 07/11/03 AM.
	result = (_tcscmp(name1,name2) < 0);									// 08/17/01 AM.

sem = new RFASem(result);

// Return string compare result
return true;
}


/********************************************
* FN:	   FNDICTFINDWORD
* CR:	   04/18/00 Dd.
* SUBJ: Find a dictionary word.
* SUBJ: Get word-concept path of given string.
* RET:  True if ok, else false.
*		       UP concept.
* FORM: dictfindword(str)
********************************************/

bool Fn::fnDictfindword(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem	    // Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
if (!Arg::str1(_T("dictfindword"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("dictfindword"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[dictfindword: Warning. Given no word.]"));	// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *wordConcept = cg->findWordConcept(str1);					// 06/29/03 AM.

if (wordConcept)													// FIX.	// 11/15/00 AM.
sem = new RFASem(wordConcept, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:	   FNSTRTOLOWER
* CR:	   05/04/00 Dd.
* SUBJ: Makes a string lower case.
* RET:  True if ok, else false.
*	 UP - returns the lower case version of the string
* FORMS:	strtolower(str1)
* NOTE:
********************************************/

bool Fn::fnStrtolower(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("strtolower"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("strtolower"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strtolower: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR *buf = Chars::create(_tcsclen(name1) + 1);			// FIX.	// 07/16/03 AM.
str_to_lower(name1, buf);			// 06/03/00 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

Chars::destroy(buf);	// FIX.	// 07/16/03 AM.

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRTOUPPER
* CR:	   05/04/00 Dd.
* SUBJ: Makes a string upper case.
* RET:  True if ok, else false.
*	 UP - returns the upper case version of the string
* FORMS:	strtoupper(str1)
* NOTE:
********************************************/

bool Fn::fnStrtoupper(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("strtoupper"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("strtoupper"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strtoupper: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR *buf = Chars::create(_tcsclen(name1)+1);					// FIX.	// 07/16/03 AM.
str_to_upper(name1, buf);			// 06/03/00 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);													// FIX.	// 07/16/03 AM.

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNADDWORD
* CR:		05/09/00 AM.
* SUBJ:	Add word to KB dictionary.
* RET:	True if ok, else false.
*			UP - concept built for word.
* FORMS:	addword(str)
* NOTE:
********************************************/

bool Fn::fnAddword(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("addword"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("word"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[addword: Warning. Given no word.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR *str;
parse->internStr(name1, /*UP*/ str);

// Need to get the current KB.
CG *cg = nlppp->parse_->getAna()->getCG();


CONCEPT *conc = cg->addWord(str);

// Return as str type.
if (conc)															// FIX.	// 11/15/00 AM.
	sem = new RFASem(conc, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:	   FNSTRLENGTH
* CR:	   05/04/00 Dd.
* SUBJ: Finds the length of the string
* RET:  True if ok, else false.
*	 UP - returns the length of the string
* FORMS:	strlength(str1)
* NOTE:
********************************************/

bool Fn::fnStrlength(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
long len = -1;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;

if (!Arg::str1(_T("strlength"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("strlength"),parse))
	return false;

if (!name1 || !*name1)														// 10/30/00 AM.
	len = 0;																		// 10/30/00 AM.
else																				// 10/30/00 AM.
	len = _tcsclen(name1);

sem = new RFASem(len);

return true;
}


/********************************************
* FN:	   FNSTRPIECE
* CR:	   05/05/00 Dd.
* SUBJ: Returns a substring of a string
* RET:  True if ok, else false.
*	 UP - returns a substring of a string
* FORMS:	strlength(str1,start,end)
* NOTE: start and end are zero-numbered
********************************************/

bool Fn::fnStrpiece(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
long start = -1;
long end = -1;

if (!Arg::str1(_T("strpiece"), /*UP*/ (DELTS*&)args, str1))
	return false;
if (!Arg::num1(_T("strpiece"), /*UP*/ (DELTS*&)args, start))
	return false;
if (!Arg::num1(_T("strpiece"), /*UP*/ (DELTS*&)args, end))
	return false;
if (!Arg::done((DELTS*)args, _T("strpiece"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strpiece: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[MAXSTR];

#ifdef JUNK_
_TCHAR *c;
int i = 0;
int j = 0;
int len = _tcsclen(str1);
if (end > len)
        end = len - 1;

for (c = str1; i < len && i < MAXSTR; ++i,++c)
        {
        if (i >= start && i <= end)
                buf[j++] = *c;
        }
buf[j] = '\0';
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.
#endif

if (!str_piece(str1,start,end,MAXSTR,/*UP*/buf))					// 11/18/02 AM.
	{
	_stprintf(Errbuf,_T("[strpiece: Error.]"));								// 11/18/02 AM.
	return parse->errOut(true); // UNFIXED 											// 11/18/02 AM.
	}

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRCHAR
* CR:	   05/05/00 Dd.
* SUBJ: Finds a character in a string
* RET:  True if ok, else false.
*	 UP - returns a character
* FORMS:	strchar(str, num)
* NOTE:
********************************************/

bool Fn::fnStrchar(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
long charnum = -1;

if (!Arg::str1(_T("strchar"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::num1(_T("strchar"), /*UP*/ (DELTS*&)args, charnum))
	return false;
if (!Arg::done((DELTS*)args, _T("strchar"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strchar: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[2];
buf[0] = name1[charnum];
buf[1] = '\0';
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRISDIGIT
* CR:	   05/05/00 Dd.
* SUBJ: Checks to see if the entire string is digits
* RET:  True if ok, else false.
*	 UP - returns 1 if all digits, 0 if not
* FORMS:	strisdigit(str1)
* NOTE:
********************************************/

bool Fn::fnStrisdigit(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *str1=0;
long isDigit = 1;

if (!Arg::str1(_T("strisdigit"), /*UP*/ (DELTS*&)args, str1))
	{
	// RECOVER.						// 06/26/06 AM.
	//return false;				// 06/26/06 AM.
	str1 = 0;						// 06/26/06 AM.
	isDigit = 0;					// 06/26/06 AM.
	}
if (!Arg::done((DELTS*)args, _T("strisdigit"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strisdigit: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

for (_TCHAR *c = str1; *c; ++c)
	{
	if (!_istdigit((_TUCHAR)*c))
		{
		isDigit = 0;
		break;
		}
	}

sem = new RFASem(isDigit);

return true;
}


/********************************************
* FN:	   FNSTRISALPHA
* CR:	   05/05/00 Dd.
* SUBJ: Checks to see if the entire string is alpha
* RET:  True if ok, else false.
*	 UP - returns 1 for true, 0 for false
* FORMS:	strisalpha(str1)
* NOTE:
********************************************/

bool Fn::fnStrisalpha(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *str1=0;
long isAlpha = 1;

if (!Arg::str1(_T("strisalpha"), /*UP*/ (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("strisalpha"),parse))
	return false;

if (!str1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strisalpha: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

for (_TCHAR *c = str1; *c; ++c)
	{
	if (!alphabetic(*c))
		{
		isAlpha = 0;
		break;
		}
	}

sem = new RFASem(isAlpha);

return true;
}


/********************************************
* FN:	   FNSTRCHR
* CR:	   11/18/00 AM.
* SUBJ: Find first occurrence of chars in string
* RET:  True if ok, else false.
*	 UP - returns string headed by found char.
* FORMS:	strchr(str, ch_str)
********************************************/

bool Fn::fnStrchr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *ch_str=0;

if (!Arg::str1(_T("strchr"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strchr"), /*UP*/ (DELTS*&)args, ch_str))
	return false;
if (!Arg::done((DELTS*)args, _T("strchr"),parse))
	return false;


if (!name1 || !*name1 || !ch_str || !*ch_str)
	{
	_stprintf(Errbuf,_T("[strchr: Warning: Given no str or char.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (ch_str[1])
	{
	_stprintf(Errbuf,_T("[strchr: Warning: 2nd arg must be string of length 1.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}


_TCHAR *str = _tcschr(name1, ch_str[0]);
if (!str || !*str)
	return true;

// Don't know if I need to or should intern the partial string.
_TCHAR *instr=0;
parse->internStr(str, /*UP*/ instr);

// Return as str type.
sem = new RFASem(instr, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRCHRCOUNT
* CR:	   12/15/02 AM.
* SUBJ:	Count occurrences of char in string
* RET:	True if ok, else false.
*			UP - count of char in string.
* FORMS:	strchrcount(str, ch_str)
********************************************/

bool Fn::fnStrchrcount(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *ch_str=0;

if (!Arg::str1(_T("strchrcount"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strchrcount"), /*UP*/ (DELTS*&)args, ch_str))
	return false;
if (!Arg::done((DELTS*)args, _T("strchrcount"),parse))
	return false;


if (!name1 || !*name1 || !ch_str || !*ch_str)
	{
	_stprintf(Errbuf,_T("[strchrcount: Warning: Given no str or char.]"));
	return parse->errOut(true); // UNFIXED
	}
if (ch_str[1])
	{
	_stprintf(Errbuf,_T("[strchrcount: Warning: 2nd arg must be string of length 1.]"));
	return parse->errOut(true); // UNFIXED
	}

long count = 0;
for (; *name1; ++name1)
	if (*name1 == *ch_str)
		++count;
sem = new RFASem(count);

return true;
}


/********************************************
* FN:	   FNSTRRCHR
* CR:	   11/18/00 AM.
* SUBJ: Find last occurrence of char in string
* RET:  True if ok, else false.
*	 UP - returns string headed by last found char.
* FORMS:	strrchr(str, ch_str)
* NOTE:
********************************************/

bool Fn::fnStrrchr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *ch_str=0;

if (!Arg::str1(_T("strrchr"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strrchr"), /*UP*/ (DELTS*&)args, ch_str))
	return false;
if (!Arg::done((DELTS*)args, _T("strrchr"),parse))
	return false;


if (!name1 || !*name1 || !ch_str || !*ch_str)
	{
	_stprintf(Errbuf,_T("[strrchr: Warning: Given no str or char.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (ch_str[1])
	{
	_stprintf(Errbuf,_T("[strrchr: Warning: 2nd arg must be string of length 1.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}


_TCHAR *str = _tcsrchr(name1, ch_str[0]);
if (!str || !*str)
	return true;

// Don't know if I need to or should intern the partial string.
_TCHAR *instr=0;
parse->internStr(str, /*UP*/ instr);

// Return as str type.
sem = new RFASem(instr, RSSTR);

return true;
}

/********************************************
* FN:		FNTAKE
* CR:		05/09/00 AM.
* SUBJ:	Take a file of commands!
* RET:	True if ok, else false.
* NOTE:	That is, read and execute a file of knowledge addition commands.
* FORMS:	take(filename_s)
********************************************/

bool Fn::fnTake(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("take"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("take"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[take: Warning. Given no name.]"));				// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

sem = new RFASem(cg->readFile(name1) ? (long)1 : (long)0);		// 08/07/01 AM.
return true;																	// 08/07/01 AM.
}


/********************************************
* FN:		FNSUCCEED
* CR:		06/10/00 AM.
* SUBJ:	Succeed in checking on rule match.
* RET:	True if ok, else false.
* NOTE:	Should be called as a statement.
*			If in @CHECK region, ends checking with success.
*			Anywhere else it's a no-op.
* FORMS:	succeed();
********************************************/

bool Fn::fnSucceed(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("succeed"),parse))
	return false;
switch (nlppp->region_)
	{
	case REGCHECK:
	case REGCODE:			// Using this to get out of @CODE.		// 10/23/00 AM.
//	case REGDECL:																// 12/24/01 AM.
		nlppp->succeed_ = true;	// Flag that we're done with checks.
//		nlppp->return_  = true;	// If in fn, return immediately.	// 03/13/02 AM.
		break;
	default:
		break;
	}

// Not allowing "exceptions" to be inside a user function.		// 08/18/08 AM.
#ifdef _TOO_STRICT_
if (nlppp->insidefn_)														// 08/18/08 AM.
		{
		_stprintf(Errbuf,_T("[succeed: Ignoring inside a function.]"));	// 08/18/08 AM.
		return parse->errOut(true);										// 08/18/08 AM.
		}
#endif

return true;
}


/********************************************
* FN:		FNFAIL
* CR:		06/10/00 AM.
* SUBJ:	Fail in checking on rule match.
* RET:	True if ok.
* NOTE:	Should be called as a statement.
*			If in @CHECK region, ends checking with failure.
*			Anywhere else it's a no-op.
* FORMS:	fail();
********************************************/

bool Fn::fnFail(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("fail"),parse))
	return false;
if (nlppp->region_ == REGCHECK)
	nlppp->fail_ = true;			// Flag that we're done with checks.

// Not allowing "exceptions" to be inside a user function.		// 08/18/08 AM.
#ifdef _TOO_STRICT_
if (nlppp->insidefn_)														// 08/18/08 AM.
		{
		_stprintf(Errbuf,_T("[fail: Ignoring inside a function.]"));	// 08/18/08 AM.
		return parse->errOut(true);										// 08/18/08 AM.
		}
#endif

return true;
}



/********************************************
* FN:		FNSYSTEM
* CR:		06/20/00 AM.
* SUBJ:	Execute given string as an operating system command.
* RET:	True if ok.
* FORMS:	system(str);
********************************************/

bool Fn::fnSystem(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::str1(_T("system"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("system"),parse))
	return false;

if (!str)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[system: Warning. Given no command.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

//_tsystem(str);	// 09/28/08 AM.
run_silent(str);			// system() w/o DOC CMD PROMPT POPUP.	// 09/28/08 AM.
return true;
}


/********************************************
* FN:		FNSTR
* CR:		07/26/00 AM.
* SUBJ:	Convert argument to string.
* RET:	True if ok.
* FORMS:	str(num);
*			str(str);		# no-op.
********************************************/

bool Fn::fnStr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str=0;
long num = -1;
float flt = -1.0;																// 05/29/02 AM.
enum Iargtype typ = IANULL;												// 05/29/02 AM.
RFASem *sem1 = 0;																// 05/29/02 AM.

// Handling float also.														// 05/29/02 AM.
if (!Arg::any1(_T("str"),parse,/*DU*/(DELTS*&)args,typ,str,num,flt,sem1))			// 05/29/02 AM.
//if (!Arg::str_or_num1("str", /*UP*/ (DELTS*&)args, str, num))			// 05/29/02 AM.
	return true;																// 05/29/02 AM.
if (!Arg::done((DELTS*)args, _T("str"),parse))
	return true;																// 05/29/02 AM.


switch (typ)																	// 05/29/02 AM.
	{
	case IANUM:																	// 05/29/02 AM.
		{
		// Convert num, then intern string.
		_TCHAR buf[MAXSTR+1];
		_stprintf(buf, _T("%ld"), num);
		parse->internStr(buf, /*UP*/ str);
		}
		break;
	case IAFLOAT:																// 05/29/02 AM.
		{
		_TCHAR buf[MAXSTR+1];													// 05/29/02 AM.
		_stprintf(buf, _T("%f"), flt);											// 05/29/02 AM.
		parse->internStr(buf, /*UP*/str);								// 05/29/02 AM.
		}
		break;
	case IASTR:	// NO OP.													// 05/29/02 AM.
		break;
	default:																		// 05/29/02 AM.
		_stprintf(Errbuf, _T("[str: Couldn't get argument.]"));			// 05/29/02 AM.
		return parse->errOut(true); // UNFIXED 													// 05/29/02 AM.
		break;
	}

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNNUM
* CR:		07/26/00 AM.
* SUBJ:	Convert argument to number.
* RET:	True if ok.
* FORMS:	num(str);
*			num(num);		# no-op.
********************************************/

bool Fn::fnNum(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str=0;
long num = -1;
float flt = -1.0;																// 05/29/02 AM.
enum Iargtype typ = IANULL;												// 05/29/02 AM.
RFASem *sem1 = 0;																// 05/29/02 AM.
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::any1(_T("num"),parse,/*DU*/(DELTS*&)args,typ,str,num,flt,sem1))			// 05/29/02 AM.
//if (!Arg::str_or_num1("num", /*UP*/ (DELTS*&)args, str, num))			// 05/29/02 AM.
	return false;
if (!Arg::done((DELTS*)args, _T("num"),parse))
	return false;

if (str && *str)			// String variant.
	{
	}

switch (typ)																	// 05/29/02 AM.
	{
	case IASTR:																	// 05/29/02 AM.
		if (!str_to_long(str, /*UP*/ num))
			return false;
		break;
	case IANUM:	// NO OP.													// 05/29/02 AM.
		break;
	case IAFLOAT:																// 05/29/02 AM.
		num = (long) flt;	// Let C++ convert/truncate.				// 05/29/02 AM.
		break;
	default:
		_stprintf(Errbuf, _T("[num: Couldn't get argument.]"));			// 05/29/02 AM.
		return parse->errOut(true); // UNFIXED 													// 05/29/02 AM.
		break;
	}

sem = new RFASem(num);
return true;
}


/********************************************
* FN:		FNFLOAT
* CR:		08/22/01 AM.
* SUBJ:	Convert argument to floating point number.
* RET:	True if ok.
* FORMS:	float(str);
*			float(num);
*			float(float);		# no-op.
********************************************/

bool Fn::fnFlt(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str=0;
long num = 0;
float fnum = 0.0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

///////////////////////
// GET ANY ARG.
if (!args)
	{
	_stprintf(Errbuf, _T("[flt: Couldn't get argument.]"));
	return parse->errOut(true); // UNFIXED
	}
Iarg *arg = args->getData();
args = args->Right();
if (!Arg::done((DELTS*)args, _T("flt"),parse))
	return true;
///////////////////////

switch (arg->getType())
	{
	case IASTR:
		str = arg->getStr();
		if (!str || !*str)
			return true;
		if (!s_to_f(str, /*UP*/ &fnum))
			return true;
		sem = new RFASem(fnum);
		return true;
	case IANUM:
		sem = new RFASem((float)(arg->getNum()));
		return true;;
	case IAFLOAT:
		sem = new RFASem(arg->getFloat());
		return true;
	default:
		_stprintf(Errbuf, _T("[flt: Arg must be simple type.]"));
		return parse->errOut(false); // UNFIXED
	}
return false;
}


/********************************************
* FN:		FNSTRTOTITLE
* CR:		08/21/00 Dd.
* SUBJ:	Makes a string title case.
* RET:	True if ok, else false.
*			UP - returns the title case version of the string
* FORMS:	strtotitle(str1)
* NOTE:
********************************************/

bool Fn::fnStrtotitle(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("strtotitle"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("strtotitle"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strtotitle: Warning. Given no str.]"));		// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR *buf = Chars::create(_tcsclen(name1)+1);				// FIX.	// 07/16/03 AM.
str_to_title(name1, buf);

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);												// FIX.	// 07/16/03 AM.

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRWRAP
* CR:	   09/22/00 Dd.
* SUBJ: Takes a long string and wraps it.
* RET:  True if ok, else false.
*		       UP - returns the wrapped version of the string
* FORMS:	strwrap(str1,int)
* NOTE:
********************************************/

bool Fn::fnStrwrap(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
long wrapSize=0;

if (!Arg::str1(_T("strwrap"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::num1(_T("strwrap"), /*UP*/ (DELTS*&)args, wrapSize))
    return false;
if (!Arg::done((DELTS*)args, _T("strwrap"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strwrap: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[100000];
str_wrap(name1, wrapSize, buf);

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRSUBST
* CR:	   09/22/00 Dd.
* SUBJ: Replaces a substring of another string.
* RET:  True if ok, else false.
*		       UP - returns the substituted version of the string
* FORMS:	strsubst(str, oldstr, newstr)
* NOTE:
********************************************/

bool Fn::fnStrsubst(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *oldStr=0;
_TCHAR *newStr=0;

if (!Arg::str1(_T("strsubst"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strsubst"), /*UP*/ (DELTS*&)args, oldStr))
    return false;
if (!Arg::str1(_T("strsubst"), /*UP*/ (DELTS*&)args, newStr))
    return false;
if (!Arg::done((DELTS*)args, _T("strsubst"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strsubst: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[100000];
str_subst(name1, oldStr, newStr, buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}



/********************************************
* FN:		FNOPENFILE
* CR:		09/23/00 AM.
* SUBJ:	Open an output file.
* RET:	True if ok, else false.
*			UP sem - semantic object of type OSTREAM.
* FORM:	openfile(path_str [, a_str [, b_str [, c_str]]]);
*			path_str - file name.
*			*_str - in random order: (up to three flags handled, for now).
*				USING C++ MODES.
*			"app"			= append.
*			"ate"			= weird append.
*			"nocreate"	= if file doesn't exist, fail.
*			"noreplace" = if file exists, fail.
*			"binary"		= open in binary mode (text is default).
* NOTE:	Unlike fileout, this doesn't create a "file variable".
*			05/21/01 AM. Changed name from OFILE.
* WARN:	Default mode is WRITE, not APPEND.  Unlike fileout in this also.
********************************************/

bool Fn::fnOpenfile(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *fname = 0;
_TCHAR *flag[3];	// Array of flag strings.
flag[0] = flag[1] = flag[2] = 0;

if (!Arg::str1(_T("openfile"),/*UP*/(DELTS*&)args,fname))
	return false;

if (!fname)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[openfile: Warning. Given no fname.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (args && Arg::str1(_T("openfile"),/*UP*/(DELTS*&)args,flag[0]))
	{
	if (args && Arg::str1(_T("openfile"),/*UP*/(DELTS*&)args,flag[1]))
		{
		if (args && Arg::str1(_T("openfile"),/*UP*/(DELTS*&)args,flag[2]))
			{
			if (!Arg::done((DELTS*)args, _T("openfile"),parse))
				return false;
			}
		}
	}

// Now figure out what all them flags are.
// Track redundancy, consistency.
int napp = 0;
int nate = 0;
int nnocreate = 0;
int nnoreplace = 0;
int nbinary = 0;

int ii;
_TCHAR *tmp;
for (ii = 0; ii < 2; ++ii)  // Check out each of the three flags.
	{
	tmp = flag[ii];
	if (!tmp)
		;							// Skip.
	else if (!strcmp_i(tmp, _T("app")))
		{
		if (napp)
			_stprintf(Errbuf,_T("[openfile: Redundant 'app' mode.]"));
		if (nate)
			{
			_stprintf(Errbuf,_T("[openfile: Incompatible modes 'ate' and 'app'.]"));
			return parse->errOut(false); // UNFIXED 											// 05/18/01 AM.
			}
		++napp;
		}
	else if (!strcmp_i(tmp, _T("ate")))
		{
		if (nate)
			_stprintf(Errbuf,_T("[openfile: Redundant 'ate' mode.]"));
		if (nate)
			{
			_stprintf(Errbuf,_T("[openfile: Incompatible modes 'app' and 'ate'.]"));
			return parse->errOut(false); // UNFIXED 											// 05/18/01 AM.
			}
		++nate;
		}
	else if (!strcmp_i(tmp, _T("nocreate")))
		{
		if (nnocreate)
			_stprintf(Errbuf,_T("[openfile: Redundant 'nocreate' mode.]"));
		++nnocreate;
		}
	else if (!strcmp_i(tmp, _T("noreplace")))
		{
		if (nnoreplace)
			_stprintf(Errbuf,_T("[openfile: Redundant 'noreplace' mode.]"));
		++nnoreplace;
		}
	else if (!strcmp_i(tmp, _T("binary")))
		{
		if (nbinary)
			_stprintf(Errbuf,_T("[openfile: Redundant 'binary' mode.]"));
		++nbinary;
		}
	}

// Check logical consistency of flags.
// eg, write, but don't create and don't overwrite is a bad combo!
if (!napp && !nate && nnoreplace && nnocreate)
	{
	_stprintf(Errbuf,_T("[openfile: Modes disallow writing out a file.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Is filename relative or absolute?
// Todo: Need better filename parsing.
// LINUX look for slash as first char.	# 02/13/19 AM.
#ifndef LINUX
bool absolute = ( (*(fname+1) == ':') ? true : false);
#else
bool absolute = ( (*(fname) == '/') ? true : false);
#endif

// Get user specified filename.  Taking name as is.
// If relative path, $appdir\output is base directory.
// If absolute, use it as is.

if (!fname || !*fname)
	{
	_stprintf(Errbuf,_T("[openfile: Error. Given null filename.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Build up a bit-flag vector for the modes.
#ifdef __linux__
std::_Ios_Openmode modes = ios::in;
#else
int modes = 0;
#endif
if (napp)
	modes |= ios::app;
if (nate)
	modes |= ios::ate;
//if (nnocreate)
//	modes |= ios::nocreate;
//if (nnoreplace)
//	modes |= ios::noreplace;
if (nbinary)
	modes |= ios::binary;

// Set up proper path for file.
// Should check if it's relative or absolute.
_TCHAR buf[MAXSTR];
_TCHAR *str = 0;
if (!absolute)		// Relative path.
	{
	_stprintf(buf, _T("%s%c%s"), parse->getOutdir(),DIR_CH, fname);
	parse->internStr(buf, /*UP*/ str);
	}
else					// Absolute path.
	parse->internStr(fname, /*UP*/ str);

// Open the output file.
_t_ostream *ostr = new _t_ofstream(TCHAR2CA(str), modes);
parse->newostr(ostr);			// Add to list of ostrs.			// 05/23/01 AM.

if (!ostr)
	{
	_stprintf(Errbuf,															// 05/18/01 AM.
		_T("[openfile: Couldn't create output stream.]\n[file=%s]"),		// 05/18/01 AM.
		str);																		// 05/18/01 AM.
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

sem = new RFASem(ostr);
return true;
}


/********************************************
* FN:		FNCLOSEFILE
* CR:		05/21/01 AM.
* SUBJ:	Close given file.
* RET:	True if ok, else false.
* FORMS:	closefile(ostream)
* NOTE:	First NLP++ function for PNODE type!
********************************************/

bool Fn::fnClosefile(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("closefile"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("closefile"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[closefile: Warning. Given no file stream.]"));
	return parse->errOut(true); // UNFIXED
	}

// Get object from sem.
if (sem1->getType() != RSOSTREAM)
	{
	_stprintf(Errbuf,_T("[closefile: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

_t_ostream *ostr = sem1->getOstream();

if (!ostr)
	{
	_stprintf(Errbuf,_T("[closefile: Couldn't fetch file stream.]"));
	return parse->errOut(false); // UNFIXED
	}

// Remove the file stream.
//delete ostr;																	// 05/23/01 AM.
nlppp->parse_->deleteostr(ostr);	// Rm from list and delete.	// 05/23/01 AM.

return true;
}


/********************************************
* FN:		FNSTRSPELLCANDIDATE
* CR:		09/26/00 Dd.
* SUBJ:	Choose the best candidate from a string list
* RET:	True if ok, else false.
*			UP - returns the best candidate.
* FORMS:	strspellcandidate(str1,strList)
* ASS:	09/28/00 AM.  strList is already all-lowercase.
********************************************/

bool Fn::fnStrspellcandidate(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
_TCHAR *str2=0;

if (!Arg::str1(_T("strspellcandidate"), /*UP*/ (DELTS*&)args, str1))
	return false;
if (!Arg::str1(_T("strspellcandidate"), /*UP*/ (DELTS*&)args, str2))
    return false;
if (!Arg::done((DELTS*)args, _T("strspellcandidate"),parse))
	return false;

if (!str1 || !*str1 || !str2 || !*str2)								// 09/28/00 AM.
	return true;

// Remember the case of word, if all caps or capitalized.		// 09/28/00 AM.
bool allcap = all_uppercase(str1);										// 09/28/00 AM.
bool cap = (is_upper((_TUCHAR)*str1) ? true : false);		// 12/16/01 AM.
// Forget about mix cap!

// Convert to lowercase.													// 09/28/00 AM.
_TCHAR *lcstr1 = Chars::create(_tcsclen(str1) + 1);						// 09/28/00 AM.
str_to_lower(str1, lcstr1);												// 09/28/00 AM.

_TCHAR buf[SPELL_BUF_LEN+1];
buf[0] = '\0';
str_spell_candidate(lcstr1, str2, buf);

Chars::destroy(lcstr1);														// 09/28/00 AM.

_TCHAR *str = "\0";
if (buf[0] == '\0')															// 09/28/00 AM.
	return true;

// Convert best candidate to the letter case of given word.		// 09/28/00 AM.
if (allcap)	// Orig word was all caps.									// 09/28/00 AM.
	{
	_TCHAR *ucstr = Chars::create(_tcsclen(buf) + 1);					// 09/28/00 AM.
	str_to_upper(buf, ucstr);												// 09/28/00 AM.
	parse->internStr(ucstr, /*UP*/ str);								// 09/28/00 AM.
	Chars::destroy(ucstr);													// 09/28/00 AM.
	}
else if (cap)			// Orig word was capitalized.					// 09/28/00 AM.
	{
	buf[0] = to_upper(buf[0]);												// 12/16/01 AM.
	parse->internStr(buf, /*UP*/ str);
	}
else	// lower case. (mix cap falls in here also.)					// 09/28/00 AM.
	parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNSTRSPELLCOMPARE
* CR:		09/26/00 Dd.
* SUBJ:	Compares two strings and returns their spell "distance".
* RET:	True if ok, else false.
*			UP - returns the wrapped version of the string
* FORMS:	strspellcompare(str1,str2)
* NOTE:
********************************************/

bool Fn::fnStrspellcompare(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *str1=0;
_TCHAR *str2=0;

if (!Arg::str1(_T("strspellcompare"), /*UP*/ (DELTS*&)args, str1))
	return false;
if (!Arg::str1(_T("strspellcompare"), /*UP*/ (DELTS*&)args, str2))
    return false;
if (!Arg::done((DELTS*)args, _T("strspellcompare"),parse))
	return false;

if (!str1 || !*str1 || !str2 || !*str2)								// 09/28/00 AM.
	return true;

// Convert both strings to lowercase.									// 09/28/00 AM.
_TCHAR *lcstr1 = Chars::create(_tcsclen(str1) + 1);						// 09/28/00 AM.
_TCHAR *lcstr2 = Chars::create(_tcsclen(str2) + 1);						// 09/28/00 AM.
str_to_lower(str1, lcstr1);												// 09/28/00 AM.
str_to_lower(str2, lcstr2);												// 09/28/00 AM.

long weight = str_spell_compare(lcstr1, lcstr2);					// 08/17/01 AM.

Chars::destroy(lcstr1);														// 09/28/00 AM.
Chars::destroy(lcstr2);														// 09/28/00 AM.

// Return as str type.
sem = new RFASem(weight);

return true;
}


/********************************************
* FN:		FNSPELLCANDIDATES
* CR:		09/27/00 AM.
* SUBJ:	Find candidates for given string.
* RET:	True if ok.
*			UP sem - contains string-list of candidates.
* FORMS:	spellcandidates(badword_str)
* NOTE:	Returns all lowercase candidates.
********************************************/

bool Fn::fnSpellcandidates(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("spellcandidates"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("spellcandidates"),parse))
	return false;

if (!str || !*str)
	return true;

// Convert to lowercase.													// 09/28/00 AM.
_TCHAR *lcstr = Chars::create(_tcsclen(str) + 1);						// 09/28/00 AM.
str_to_lower(str, lcstr);													// 09/28/00 AM.

_TCHAR buf[SPELL_BUF_LEN];
buf[0] = '\0';																	// 09/28/00 AM.
spell_candidates(lcstr, buf, SPELL_BUF_LEN);							// 10/16/00 AM.
Chars::destroy(lcstr);														// 09/28/00 AM.

if (buf[0] == '\0')															// 09/28/00 AM.
	return true;

parse->internStr(buf, /*UP*/ str);
sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNSPELLWORD
* CR:		09/27/00 AM.
* SUBJ:	See if given word is in the spell word list.
* RET:	True if ok.
*			UP sem - 1 if found, 0 if not found.
* FORMS:	spellword(word_str)
********************************************/

bool Fn::fnSpellword(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str=0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::str1(_T("spellword"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("spellword"),parse))
	return false;

if (!str || !*str)
	return true;

// Convert to lowercase.													// 09/28/00 AM.
long len = _tcsclen(str);														// 10/16/00 AM.
if (len > MAXWORDLENGTH)										// FIX	// 11/13/01 AM.
	{
	sem = new RFASem((long)0);									// FIX	// 11/13/01 AM.
	return true;													// FIX	// 11/13/01 AM.
	}
_TCHAR *lcstr = Chars::create(len + 1);									// 09/28/00 AM.
str_to_lower(str, lcstr);													// 09/28/00 AM.

long pos;
long ans;																		// 08/17/01 AM.
if (binary_spell(lcstr, word_arrays[len],word_lengths[len], /*UP*/pos))
	ans = 1;
else
	ans = 0;
Chars::destroy(lcstr);														// 09/28/00 AM.

sem = new RFASem(ans);
return true;
}



/********************************************
* FN:		FNSPELLCORRECT
* CR:		09/27/00 AM.
* SUBJ:	If not on spell word list, then correct word.  Else return as is.
* RET:	True if ok.
*			UP sem - corrected word.
* FORMS:	spellcorrect(word_str)
********************************************/

bool Fn::fnSpellcorrect(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("spellcorrect"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("spellcorrect"),parse))
	return false;

if (!str || !*str)
	return true;

// Remember the case of word, if all caps or capitalized.		// 09/28/00 AM.
bool allcap = all_uppercase(str);										// 09/28/00 AM.
bool cap = (is_upper((_TUCHAR)*str) ? true : false);		// 12/16/01 AM.
// Forget about mix cap!

// Convert to lowercase.													// 09/28/00 AM.
long len = _tcsclen(str);														// 10/16/00 AM.
if (len > MAXWORDLENGTH)				// Forget it		// FIX	// 11/13/01 AM.
	return true;													// FIX	// 11/13/01 AM.

_TCHAR *lcstr = Chars::create(len + 1);									// 09/28/00 AM.
str_to_lower(str, lcstr);													// 09/28/00 AM.

_TCHAR *istr = 0;

long pos;
_TCHAR buf2[1001];
if (!binary_spell(lcstr, word_arrays[len],word_lengths[len], /*UP*/pos))
	{
	// Get corrected spell here.
	_TCHAR buf[SPELL_BUF_LEN];
	buf[0] = '\0';																// 09/28/00 AM.
	spell_candidates(lcstr, buf, SPELL_BUF_LEN);
	if (buf[0] == '\0')		// No candidates.							// 09/28/00 AM.
		{
		Chars::destroy(lcstr);												// 09/28/00 AM.
		return true;
		}

	buf2[0] = '\0';
	str_spell_candidate(lcstr, buf, buf2);
	Chars::destroy(lcstr);													// 09/28/00 AM.
	str = buf2;
	}
else			// Word is known.												// 09/28/00 AM.
	{
	Chars::destroy(lcstr);													// 09/28/00 AM.
	parse->internStr(str, /*UP*/ istr);
	sem = new RFASem(istr, RSSTR);
	return true;
	}

if (!str || !*str)
	return true;

// Convert best candidate to the letter case of given word.		// 09/28/00 AM.
if (allcap)	// Orig word was all caps.									// 09/28/00 AM.
	{
	_TCHAR *ucstr = Chars::create(_tcsclen(str) + 1);					// 09/28/00 AM.
	str_to_upper(str, ucstr);												// 09/28/00 AM.
	parse->internStr(ucstr, /*UP*/ istr);								// 09/28/00 AM.
	Chars::destroy(ucstr);													// 09/28/00 AM.
	}
else if (cap)			// Orig word was capitalized.					// 09/28/00 AM.
	{
	*str = to_upper(*str);													// 12/16/01 AM.
	parse->internStr(str, /*UP*/ istr);
	}
else	// lower case. (mix cap falls in here also.)					// 09/28/00 AM.
	parse->internStr(str, /*UP*/ istr);

sem = new RFASem(istr, RSSTR);
return true;
}


/********************************************
* FN:		FNLJ
* CR:		10/02/00 AM.
* SUBJ:	Left justify STRING in given field size.
* RET:	str - of given field size, with number right justified.
*				If number is larger than size, then wider string is returned.
* FORMS:	LJ(str, fieldsize_num)
* TODO:	Write one function for for all the types (str, num).
********************************************/

bool Fn::fnLj(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *sptr;
long fieldsize = -1;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("LJ"), /*UP*/ (DELTS*&)args, sptr))
	return false;
if (!Arg::num1(_T("LJ"), /*UP*/ (DELTS*&)args, fieldsize))
	return false;
if (!Arg::done((DELTS*)args, _T("LJ"),parse))
	return false;

if (!sptr)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[LJ: Warning. Given no str.]"));					// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR *buf=0;
if (!(buf = leftjustify(sptr,fieldsize)))
	return true;

_TCHAR *str=0;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNRIGHTJUSTIFYNUM
* CR:		09/30/00 AM.
* SUBJ:	Right justify number in given field size.
* RET:	str - of given field size, with number right justified.
*				If number is larger than size, then wider string is returned.
* FORMS:	rightjustifynum(num, fieldsize_num)
********************************************/

bool Fn::fnRightjustifynum(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
long num = -1;
long fieldsize = -1;
Parse *parse = nlppp->parse_;

if (!Arg::num1(_T("rightjustifynum"), /*UP*/ (DELTS*&)args, num))
	return false;
if (!Arg::num1(_T("rightjustifynum"), /*UP*/ (DELTS*&)args, fieldsize))
	return false;
if (!Arg::done((DELTS*)args, _T("rightjustifynum"),parse))
	return false;

_TCHAR *buf=0;
if (!(buf = rightjustify(num,fieldsize)))
	return true;

_TCHAR *str=0;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNPERCENTSTR
* CR:		10/01/00 AM.
* SUBJ:	Calculate and format a percentage.
* RET:	str - Right-justified percentage string.
*			  If percent is > 100, no justify.
* FORMS:	percentstr(numerator_num, denominator_num)
********************************************/

bool Fn::fnPercentstr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
long numerator = -1;
long denominator = -1;
int fieldsize = 3;
int pct=0;
Parse *parse = nlppp->parse_;

if (!Arg::num1(_T("percentstr"), /*UP*/ (DELTS*&)args, numerator))
	return false;
if (!Arg::num1(_T("percentstr"), /*UP*/ (DELTS*&)args, denominator))
	return false;
if (!Arg::done((DELTS*)args, _T("percentstr"),parse))
	return false;

if (numerator < 0 || denominator < 0)
	{
	_stprintf(Errbuf,_T("[percentstr: Not handling negative nums.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (denominator == 0)
	{
	// Divide-by-zero.
	pct = 0;
	}
else
	pct = ((100 * numerator) / denominator);

_TCHAR *buf=0;
if (!(buf = rightjustify(pct,fieldsize)))
	return true;

_TCHAR *str=0;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNHITCONF
* CR:		10/08/00 AM.
* SUBJ:	Confidence function for hits.
* RET:	num - between 0 and 100.
* FORMS:	hitconf(hits_num, total_num, factor_num)
*			factor_num - A fudge factor.  Typical values range from 3 to 13.
********************************************/

bool Fn::fnHitconf(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

long hits = -1;
long tot = -1;
long factor = -1;
long pct=0;																		// 08/17/01 AM.

if (!Arg::num1(_T("hitconf"), /*UP*/ (DELTS*&)args, hits))
	return false;
if (!Arg::num1(_T("hitconf"), /*UP*/ (DELTS*&)args, tot))
	return false;
if (!Arg::num1(_T("hitconf"), /*UP*/ (DELTS*&)args, factor))
	return false;
if (!Arg::done((DELTS*)args, _T("hitconf"),parse))
	return false;

if (hits < 0 || tot < 0 || factor < 0)
	{
	_stprintf(Errbuf,_T("[hitconf: Not handling negative nums.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

pct = webcoConfidence(hits,tot,factor);
sem = new RFASem(pct);
return true;
}


/********************************************
* FN:		FNARRAYLENGTH
* CR:		10/20/00 AM.
* SUBJ:	Get length of an array.
* RET:	True if ok, else false.
* FORMS:	long arraylength(var)
* NOTE:	Return 1 for simple types.
********************************************/

bool Fn::fnArraylength(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;
long num1=0;
float fnum=0; // unused. // 12/15/14 AM.
_TCHAR *str1=0;
_TCHAR *name1=0;

if (!Arg::any1(_T("arraylength"),parse,(DELTS*&)args, str1,num1,fnum,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("arraylength"),parse))
	return false;

long len = 1;
if (!sem1)																		// 10/23/00 AM.
	{
	sem = new RFASem(len);
	return true;
	}
switch (sem1->getType())
	{
	case RSARGS:
		// Array.  Compute length.
		len = sem1->getArgs()->getLength();
		break;
	default:
		break;
	}
sem = new RFASem(len);
return true;
}

//*************************************************
// NLP++ PARSE TREE FUNCTIONS
//*************************************************

/********************************************
* FN:		FNPNVAR
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's variable.
* RET:	True if ok, else false.
* FORMS:	pnvar(pnode, var_str)
* NOTE:	First NLP++ function for PNODE type!
********************************************/

bool Fn::fnPnvar(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;
_TCHAR *name1=0;

if (!Arg::sem1(_T("pnvar"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("pnvar"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnvar"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnvar: Warning. Given no pnode.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnvar: Warning. Given no name.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnvar: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnvar: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

if (*name1 == '$')												// Get special var.
	return Ivar::nodeVarspecial(node,name1+1,nlppp,/*UP*/sem);
else																	// Get normal var.
	return Ivar::nodeVar(node,name1,nlppp,/*UP*/sem);
}


/********************************************
* FN:		FNPNSINGLETDOWN
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's child, if a singlet.
* RET:	True if ok, else false.
* FORMS:	pnsingletdown(pnode)
* NOTE:	Observe rules about branching and BASE attribute.
********************************************/

bool Fn::fnPnsingletdown(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pnsingletdown"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnsingletdown"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnsingletdown: Warning. Given no pnode.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnsingletdown: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnsingletdown: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Pn *pn = node->getData();
if (pn->getBase())			// Current node is a bottommost singlet.
	{
	// sem = 0;					// Return nil node.
	return true;
	}
if (!(node = node->Down()))	// Leaf.
	return true;					// Return nil node.
if (node->Right())				// Branches out.
	return true;					// Return nil node.
// Got good singlet child!
sem = new RFASem(node);
return true;
}


/********************************************
* FN:		FNPNNAME
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's name.
* RET:	True if ok, else false.
* FORMS:	str pnname(pnode)
* NOTE:	Observe rules about branching and BASE attribute.
********************************************/

bool Fn::fnPnname(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pnname"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnname"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnname: Warning. Given no pnode.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnname: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnname: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Pn *pn = node->getData();
_TCHAR *str = pn->getName();
// Todo: Name should be interned, but may want to check to be sure.
sem = new RFASem(str,RSSTR);	// FIX.	// 09/07/03 AM.
return true;
}


/********************************************
* FN:		FNPNROOT
* CR:		10/18/00 AM.
* SUBJ:	Get root of parse tree.
* RET:	True if ok, else false.
* FORMS:	pnode pnroot()
* NOTE:
********************************************/

bool Fn::fnPnroot(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

if (!Arg::done((DELTS*)args, _T("pnroot"),parse))
	return false;

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();
if (!tree)
	{
	_stprintf(Errbuf,_T("[pnroot: Couldn't fetch parse tree.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *root = tree->getRoot();
if (!root)
	{
	_stprintf(Errbuf,_T("[pnroot: Couldn't fetch root of parse tree.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

sem = new RFASem(root);
return true;
}


/********************************************
* FN:		FNPNDOWN
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's child.
* RET:	True if ok, else false.
* FORMS:	pnode pndown(pnode)
********************************************/

bool Fn::fnPndown(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pndown"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pndown"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pndown: Warning. Given no pnode.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pndown: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pndown: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

if (!(node = node->Down()))	// Leaf.
	return true;					// Return nil node.
sem = new RFASem(node);
return true;
}


/********************************************
* FN:		FNPNUP
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's parent.
* RET:	True if ok, else false.
* FORMS:	pnode pnup(pnode)
********************************************/

bool Fn::fnPnup(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pnup"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnup"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnup: Warning. Given no pnode.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnup: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnup: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

if (!(node = node->Up()))		// Root or not a leftmost node.
	return true;					// Return nil node.
sem = new RFASem(node);
return true;
}


/********************************************
* FN:		FNPNNEXT
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's right sibling.
* RET:	True if ok, else false.
* FORMS:	pnode pnnext(pnode)
********************************************/

bool Fn::fnPnnext(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pnnext"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnnext"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnnext: Warning. Given no pnode.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnnext: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnnext: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

if (!(node = node->Right()))	// At right end of list of siblings.
	return true;					// Return nil node.
sem = new RFASem(node);
return true;
}

/********************************************
* FN:		FNPNPREV
* CR:		10/18/00 AM.
* SUBJ:	Get PNODE's left sibling.
* RET:	True if ok, else false.
* FORMS:	pnode pnprev(pnode)
********************************************/

bool Fn::fnPnprev(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pnprev"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnprev"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pnprev: Warning. Given no pnode.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnprev: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnprev: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

if (!(node = node->Left()))	// At start of siblings.
	return true;					// Return nil node.
sem = new RFASem(node);
return true;
}


/********************************************
* FN:		FNPNRENAME
* CR:		11/21/00 AM.
* SUBJ:	Rename PNODE.
* RET:	True if ok, else false.
* FORMS:	str pnrename(pnode, str)
********************************************/

bool Fn::fnPnrename(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;
_TCHAR *str1=0;

if (!Arg::sem1(_T("pnrename"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("pnrename"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnrename"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[pnrename: Warning. Given no pnode.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (!str1 || !*str1)
	{
	_stprintf(Errbuf,_T("[pnrename: Warning. Given no rename string.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnrename: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnrename: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

_TCHAR *str=0;
nlppp->parse_->internStr(str1, /*UP*/str);	// Intern str.

Pn *pn = node->getData();
pn->setName(str);

// Silently tell where this node rename came from.					// 08/25/08 AM.
// But this need not be restricted to a rule context.
long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 08/25/08 AM.
pn->setPassnum(nlppp->parse_->currpass_);								// 08/25/08 AM.
pn->setRuleline(ruleline);													// 08/25/08 AM.

sem = new RFASem(str,RSSTR);	// FIX.	// 09/07/03 AM.
return true;
}


/********************************************
* FN:		FNPNDELETECHILDS
* CR:		11/25/00 AM.
* SUBJ:	Delete PNODE's children.
* RET:	True if ok, else false.
* FORMS:	pndeletechilds(pnode)
********************************************/

bool Fn::fnPndeletechilds(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pndeletechilds"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pndeletechilds"),parse))
	return false;

if (!sem1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[pndeletechilds: Warning. Given no pnode.]"));// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pndeletechilds: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pndeletechilds: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *dn = node->Down();
if (dn)
	((Tree<Pn> *)nlppp->parse_->getTree())->deleteSubs(dn);	// Delete the subtree.
return true;
}


/********************************************
* FN:		FNVARINLIST
* CR:		11/21/00 AM.
* SUBJ:	Get any variable in list of pnodes that matched rule element.
* RET:	True if ok, else false.
* FORMS:	varinlist(var_str, elt_num)
* NOTE:	To handle multi-node matches of a rule element.
*			Not handling special variables.
********************************************/

bool Fn::fnVarinlist(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
long num1=0;

if (!Arg::str1(_T("varinlist"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::num1(_T("varinlist"), (DELTS*&)args, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("varinlist"),parse))
	return false;

if (!name1)
	{
	_stprintf(Errbuf,_T("[varinlist: Warning. Given no var name.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// This type of logic appears in post-actions.  Fetch the node(s) that
// matched the given rule element number.
Node<Pn> *colls;
Node<Pn> *node1, *node2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return true;		// Optional elt may have no matches, so ok.
	}
node1 = colls->Down();	// Try to get start of range.
node2 = colls->eDown();	// Try to get end of range.

if (!node1 || !node2)
	return true;

Node<Pn> *end = node2->Right();

// Traverse nodes looking for variable.
Node<Pn> *node;
for (node = node1; node != end; node = node->Right())
	{
	Ivar::nodeVar(node,name1,nlppp,/*UP*/sem);
	if (sem)		// FOUND VARIABLE.
		return true;
	}
return true;
}


/********************************************
* FN:		FNGROUP
* CR:		11/23/00 AM.
* SUBJ:	Reduce nodes that matched a range of rule elements.
* RET:	True if ok, else false.
* UP:		sem = the created node	04/04/10 AM.
* FORMS:	group(start_num,end_num,name_str)
*			start_num,end_num - range of rule elements.
*			name_str - name for the reduce node.
* NOTE:	A redo of the post GROUP action as a function, to enable NLP++
*			exprs as arguments.
********************************************/

bool Fn::fnGroup(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.


long num1=0,num2=0;
_TCHAR *tname;	// 07/24/07 AM.
_TCHAR *name1=0;

if (!Arg::num1(_T("group"), (DELTS*&)args, num1))
	return false;
if (!Arg::num1(_T("group"), (DELTS*&)args, num2))
	return false;
if (!Arg::str1(_T("group"), /*UP*/ (DELTS*&)args, tname))
	return false;
if (!Arg::done((DELTS*)args, _T("group"),parse))
	return false;

nlppp->parse_->internStr(tname, /*UP*/name1);	// Intern str.	// 07/24/07 AM.

switch (nlppp->region_)
	{
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,														// 05/18/01 AM.
			_T("[group: Error. Must appear in @POST region.]"));		// 05/18/01 AM.
		return parse->errOut(true); // UNFIXED 													// 05/18/01 AM.
	}

if (!name1)
	{
	_stprintf(Errbuf,_T("[group: Error. Given no var name.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (num1 <= 0 || num2 <= 0)
	{
	_stprintf(Errbuf,_T("[group: Error. Given bad num.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

if (!nlppp->first_ || !nlppp->last_)
	{
	_stprintf(Errbuf,_T("[group: Error. Null first or last node in range.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (nlppp->red_)
	{
	_stprintf(Errbuf,_T("[group: Error. Rule already reduced.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}
if (nlppp->used_)
	{
	_stprintf(Errbuf,_T("[group: Error. Collect already used.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	_stprintf(Errbuf,_T("[group: Warning. No collected nodes.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}


// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;

// DO NOT GO PAST LOOKAHEAD NODE ETC.	// FIX.	// 04/24/10 AM.

if (nlppp->nlookahead_)											// 04/24/10 AM.
	{
	if (num2 >= nlppp->nlookahead_)							// 04/24/10 AM.
		{
		num2 = (nlppp->nlookahead_ - 1);						// 04/24/10 AM.
		_t_strstream gerrStr;
		gerrStr << _T("[group: Cannot group lookahead node.]") << ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
	}

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

////////////////////////////////////////////////
// Find the collect elts representing the group.
////////////////////////////////////////////////
// These numbers represent a range of rule elts in the collect.
// Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.
Node<Pn> *colls;
Node<Pn> *node1, *node2;

// Record the start and end collect of the range, so we can fix up the
// collect.
Node<Pn> *coll1, *coll2;

colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
node1 = colls->Down();	// Try to get start of range.
node2 = colls->eDown();	// Try to get end of range.
coll1 = colls;

while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	if (!node1)
		node1 = colls->Down();	// Try to get start of range.
	if (colls->eDown())
		node2 = colls->eDown();	// Got new end of range.

	// To hedge our bets, in case the rule matching code allows the start
	// of a collect to be nonnull and the end to be null, do the following:
	if (colls->Down() && !colls->eDown())
		{
		_stprintf(Errbuf,_T("[postGroup: collect with null end found.]"));
		node2 = 0;
		break;				// The range goes from node1 to the end of list.
		}
	}
coll2 = colls;

// GROUP to return the created node.	// 04/04/10 AM.
// Here is a COLLECT ELEMENT NODE.
Node<Pn> *eltn = Pat::groupElts(coll1,coll2,node1,node2,name1,nlppp);	// 04/04/10 AM.
if (!eltn)					// 04/04/10 AM.
	return false;			// 04/04/10 AM.
// GET THE ACTUAL NODE.
Node<Pn> *res = eltn->Down();	// 04/04/10 AM.
if (!res)	// 04/04/10 AM.
	return false;
sem = new RFASem(res);	// 04/04/10 AM.
return true;				// 04/04/10 AM.
}


/********************************************
* FN:		FNSPLICE
* CR:		07/19/02 AM.
* SUBJ:	Zap matched nodes and splice in their children.
* RET:	True if ok, else false.
* FORMS:	splice(start_num,end_num)
*			start_num,end_num - range of rule elements.
* NOTE:	A redo of the post SPLICE action as a function, to enable NLP++
*			exprs as arguments.  Also, to fix the fact that the old one didn't
*			update the COLLECT data structure.
********************************************/

bool Fn::fnSplice(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

long num1=0,num2=0;

if (!Arg::num1(_T("splice"), (DELTS*&)args, num1))
	return false;
if (!Arg::num1(_T("splice"), (DELTS*&)args, num2))
	return false;
if (!Arg::done((DELTS*)args, _T("splice"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,
			_T("[splice: Error. Must appear in @POST region.]"));
		return parse->errOut(true); // UNFIXED
	}

if (num1 <= 0 || num2 <= 0)
	{
	_stprintf(Errbuf,_T("[splice: Error. Given bad num.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!nlppp->first_ || !nlppp->last_)
	{
	_stprintf(Errbuf,_T("[splice: Error. Null first or last node in range.]"));
	return parse->errOut(true); // UNFIXED
	}
if (nlppp->red_)
	{
	_stprintf(Errbuf,_T("[splice: Error. Rule already reduced.]"));
	return parse->errOut(true); // UNFIXED
	}
if (nlppp->used_)
	{
	_stprintf(Errbuf,_T("[splice: Error. Collect already used.]"));
	return parse->errOut(true); // UNFIXED
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;

if (Pat::spliceElts(num1,num2,nlppp))
	return true;

return true;
}


/********************************************
* FN:		FNBATCHSTART
* CR:		10/19/00 AM.
* SUBJ:	Check if current file is first in a batch.
* RET:	True if ok, else false.
* FORMS:	bool batchstart()
* NOTE:	Could have been G("$batchstart") or somethin.
********************************************/

bool Fn::fnBatchstart(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("batchstart"),parse))
	return false;
bool batchstart = parse->getFbatchstart();
long flag;																		// 08/17/01 AM.
if (batchstart)
	flag = 1;
else
	flag = 0;

sem = new RFASem(flag);
return true;
}


/********************************************
* FN:		FNTOPDIR
* CR:		10/20/00 AM.
* SUBJ:	Get topmost directory in a file path.
* FORMS:	dir_str topdir(filepath_str)
*			dir_str - topmost folder or directory.
********************************************/

bool Fn::fnTopdir(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *filepath=0;

if (!Arg::str1(_T("topdir"), /*UP*/ (DELTS*&)args, filepath))
	return false;
if (!Arg::done((DELTS*)args, _T("topdir"),parse))
	return false;

if (!filepath || !*filepath)
	{
	_stprintf(Errbuf,_T("[topdir: Empty filepath.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// INCOMPLETE, UNATTACHED..... 10/20/00 AM.
_stprintf(Errbuf,_T("[topdir: Unimplemented function.]"));
return true;
}


/********************************************
* FN:		FNUNPACKDIRS
* CR:		10/20/00 AM.
* SUBJ:	Unpack directory names from a full file path.
* FORMS:	dirs_list unpackdir(filepath_str)
*			dir_list - a MULTI-VALUE LIST of directory names.
********************************************/

bool Fn::fnUnpackdirs(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *filepath=0;

if (!Arg::str1(_T("unpackdirs"), /*UP*/ (DELTS*&)args, filepath))
	return false;
if (!Arg::done((DELTS*)args, _T("unpackdirs"),parse))
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.

if (!filepath || !*filepath)
	{
	_stprintf(Errbuf,_T("[unpackdirs: Empty filepath.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

Dlist<Iarg> *dirs = unpackdirs(filepath, nlppp->parse_);

if (dirs)
	sem = new RFASem(dirs);
return true;
}


/********************************************
* FN:		UNPACKDIRS
* CR:		10/20/00 AM.
* SUBJ:	Unpack directory names from a full file path.
* NOTE:	Utility function.
*			INTERN every component of path.
********************************************/

Dlist<Iarg> *Fn::unpackdirs(_TCHAR *filepath, Parse *parse)
{
if (!filepath || !*filepath)
	return 0;

// Parse filepath.
Dlist<Iarg> *list = new Dlist<Iarg>();		// Make empty list of args.
_TCHAR *ptr = filepath;
_TCHAR buf[MAXSTR];
_TCHAR *bptr = buf;
_TCHAR *str;
Iarg *arg;
for (;;)
	{
	switch (*ptr)
		{
		case ':':	// Got drive.
			buf[0] = '\0';
			bptr = buf;
			if (*++ptr != '\\')
				goto err;
			++ptr;	// Sluff the directory char.
			break;
		case '\\':	// Got directory.
			// Intern string, build arg.
			*bptr = '\0';	// Terminate string.
			parse->internStr(buf, /*UP*/ str);
			arg = new Iarg(str);
			list->rpush(arg);

			buf[0] = '\0';
			bptr = buf;
			++ptr;
			break;
		case '\0':	// End of string.
			// Ignore last string, if any.
			//buf[0] = '\0';
			//bptr = buf;
			goto done;
		default:		// Normal char.
			*bptr++ = *ptr++;		// Copy current string.
			break;
		}
	}

err:
	// Wipe out the list of args.
	Dlist<Iarg>::DeleteDlistAndData(list);
	return 0;
done:
	return list;
}



/********************************************
* FN:		FNSPLIT
* CR:		03/14/03 AM.
* SUBJ:	Split a string according to delimiter chars.
* FORMS:	dirs_list unpackdir(filepath_str)
*			dir_list - a MULTI-VALUE LIST of directory names.
********************************************/

bool Fn::fnSplit(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *buf=0;
_TCHAR *chrs=0;

if (!Arg::str1(_T("split"), /*UP*/ (DELTS*&)args, buf))
	return false;
if (!Arg::str1(_T("split"), /*UP*/ (DELTS*&)args, chrs))
	return false;
if (!Arg::done((DELTS*)args, _T("split"),parse))
	return parse->errOut(false); // UNFIXED

if (!buf || !*buf)
	return true;	// Given empty buffer is ok.
if (!chrs || !*chrs)
	{
	_stprintf(Errbuf,_T("[split: Empty delimiter string.]"));
	return parse->errOut(true); // UNFIXED
	}

Dlist<Iarg> *parts = split_util(buf, chrs, nlppp->parse_);

if (parts)
	sem = new RFASem(parts);
return true;
}


/********************************************
* FN:		SPLIT_UTIL
* CR:		03/14/03 AM.
* SUBJ:	Split a string based on delimiters.
* NOTE:	Utility function.
*			INTERN every component of string.
*			Accepting empty strings between delimiters.
********************************************/

Dlist<Iarg> *Fn::split_util(_TCHAR *fullstr, _TCHAR *chrs, Parse *parse)
{
if (!fullstr || !*fullstr)
	return 0;

// Parse filepath.
Dlist<Iarg> *list = new Dlist<Iarg>();		// Make empty list of args.
_TCHAR *ptr = fullstr;
// char buf[MAXSTR];											// FIX.	// 08/27/03 AM.
long len = _tcsclen(fullstr);								// FIX.	// 08/27/03 AM.
_TCHAR *buf = Chars::create(len+1);						// FIX.	// 08/27/03 AM.
_TCHAR *bptr = buf;
_TCHAR *str;
Iarg *arg;
for (;;)
	{
	if (*ptr && _tcschr(chrs,*ptr))	// Got delimiter
		{
		// Intern string, build arg.
		*bptr = '\0';	// Terminate string.
		if (*buf)
			parse->internStr(buf, /*UP*/ str);
		else
			str = 0;
		arg = new Iarg(str);
		list->rpush(arg);

		buf[0] = '\0';
		bptr = buf;
		++ptr;
		}
	else
	switch (*ptr)
		{
		case '\0':	// End of string.
			// Intern string, build arg.
			*bptr = '\0';	// Terminate string.
			if (*buf)
				parse->internStr(buf, /*UP*/ str);
			else
				str = 0;
			arg = new Iarg(str);
			list->rpush(arg);

			buf[0] = '\0';
			bptr = buf;
			goto done;
		default:		// Normal char.
			*bptr++ = *ptr++;		// Copy current string.
			break;
		}
	}
/*
err:
	Dlist<Iarg>::DeleteDlistAndData(list);
	return 0;
*/
done:
	Chars::destroy(buf);										// FIX.	// 08/27/03 AM.
	return list;
}

/********************************************
* FN:		FNRMATTRS
* CR:		10/31/00 AM.
* SUBJ:	Remove concept's attributes.
* RET:	True if ok, else false.
* FORMS:	rmattrs(conc)
********************************************/

bool Fn::fnRmattrs(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;

if (!Arg::sem1(_T("rmattrs"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("rmattrs"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[rmattrs: Warning. Given no concept.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[rmattrs: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}
CONCEPT *conc1 = sem1->getKBconcept();
ATTR *attr = 0;

cg->rmAttrs(conc1);
return true;
}


/********************************************
* FN:		FNSTRCLEAN
* CR:		11/09/00 Dd.
* SUBJ:	Cleans the string of white space.
* RET:	True if ok, else false.
*			UP - returns the cleaned version of the string
* FORMS:	strclean(str1)
* NOTE:
********************************************/

bool Fn::fnStrclean(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("strclean"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("strclean"),parse))
	return false;

if (!name1 || !*name1)
	{
	_stprintf(Errbuf,_T("[strclean: Warning. Given no str.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[100000];
clean_str(name1, buf);

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNSTRTRIM
* CR:		11/09/00 Dd.
* SUBJ:	Trims the start and end white space from a string
* RET:	True if ok, else false.
*			UP - returns the trimmed version of the string
* FORMS:	strtrim(str1)
* NOTE:
********************************************/

bool Fn::fnStrtrim(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;

if (!Arg::str1(_T("strtrim"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::done((DELTS*)args, _T("strtrim"),parse))
	return false;

if (!name1 || !*name1)
	{
	_stprintf(Errbuf,_T("[strtrim: Warning. Given no str.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[100000];
str_trim(name1, buf);

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNSTRSTARTSWITH
* CR:		08/05/21 Dd.
* SUBJ:		Checks to see if the string starts with the given start string.
* RET:		True if starts with, else false.
*			UP - returns true if string has start string, else false.
* FORMS:	strstartswith(str,end_str)
* NOTE:
********************************************/

bool Fn::fnStrstartswith(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *ending=0;

if (!Arg::str1(_T("strstartswith"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strstartswith"), /*UP*/ (DELTS*&)args, ending))
	return false;
if (!Arg::done((DELTS*)args, _T("strstartswith"),parse))
	return false;

if (!name1)
	{
	_stprintf(Errbuf,_T("[strstartswith: Warning. Given no str.]"));
	return parse->errOut(true);
	}

long endsWith = str_starts_with(name1, ending) ? 1 : 0;

// Return as str type.
sem = new RFASem(endsWith);

return true;
}


/********************************************
* FN:		FNSTRENDSWITH
* CR:		11/17/00 Dd.
* SUBJ:		Checks to see if the string ends with the given ending.
* RET:		True if ok, else false.
*			UP - returns true if string has ending, else false.
* FORMS:	strendswith(str,end_str)
* NOTE:
********************************************/

bool Fn::fnStrendswith(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *ending=0;

if (!Arg::str1(_T("strendswith"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strendswith"), /*UP*/ (DELTS*&)args, ending))
	return false;
if (!Arg::done((DELTS*)args, _T("strendswith"),parse))
	return false;

if (!name1)
	{
	_stprintf(Errbuf,_T("[strendswith: Warning. Given no str.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

long endsWith = str_ends_with(name1, ending) ? 1 : 0;

// Return as str type.
sem = new RFASem(endsWith);

return true;
}


/********************************************
* FN:		FNPHRASETEXT
* CR:		10/20/00 AM.
* SUBJ:	Get text for right-hand-side phrase matching current rule.
* FORMS:	phrasetext().
********************************************/

bool Fn::fnPhrasetext(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("phrasetext"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGCHECK:
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,														// 05/18/01 AM.
			_T("[phrasetext: Error. Must appear in @CHECK or @POST region.]"));
		return parse->errOut(true); // UNFIXED 													// 05/18/01 AM.
	}

long len  = 0;
_TCHAR *text=0;
_TCHAR *str=0;
Node<Pn> *nstart= nlppp->getFirst();	// First node matching rule.
Node<Pn> *nend = nlppp->getLast();		// Last node matching rule.
// Not sure if these will always be what we want.

if (!nstart || !nend)
	return true;
Pn *pn = nstart->getData();
if (nstart != nend)
	{
	Pn *pn2 = nend->getData();
	len = pn2->getEnd() - pn->getStart() + 1;
	}
else
	len = pn->getEnd() - pn->getStart() + 1;

	// Get text for multi-node region.
	// Intern text.
_TCHAR *buf;
text = make_str(pn->getText(), len);
if (!text || !*text)
	return true;

buf = Chars::create(len+1);
clean_str(text,buf);
nlppp->parse_->internStr(buf, /*UP*/ str);
sem = new RFASem(str, RSSTR);
Chars::destroy(buf);			// Free up buffer.
Chars::destroy(text);
return true;
}


/********************************************
* FN:		FNPHRASERAW
* CR:		10/20/00 AM.
* SUBJ:	Get raw text for right-hand-side phrase matching current rule.
* FORMS:	phraseraw().
********************************************/

bool Fn::fnPhraseraw(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("phrasetext"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGCHECK:
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,														// 05/18/01 AM.
		_T("[phrasetext: Error. Must appear in @CHECK or @POST region.]"));
		return parse->errOut(true); // UNFIXED 													// 05/18/01 AM.
	}

long len  = 0;
_TCHAR *text=0;
_TCHAR *str=0;
Node<Pn> *nstart= nlppp->getFirst();	// First node matching rule.
Node<Pn> *nend = nlppp->getLast();		// Last node matching rule.
// Not sure if these will always be what we want.

if (!nstart || !nend)
	return true;
Pn *pn = nstart->getData();
if (nstart != nend)
	{
	Pn *pn2 = nend->getData();
	len = pn2->getEnd() - pn->getStart() + 1;
	}
else
	len = pn->getEnd() - pn->getStart() + 1;

	// Get text for multi-node region.
	// Intern text.
text = make_str(pn->getText(), len);
if (!text || !*text)
	return true;

nlppp->parse_->internStr(text, /*UP*/ str);
sem = new RFASem(str, RSSTR);
Chars::destroy(text);
return true;
}


/********************************************
* FN:		FNLASTELTNODE
* CR:		11/25/00 AM.
* SUBJ:	Get last node matching a rule element.
* RET:	True if ok, else false.
* FORMS:	lasteltnode(elt_num)
********************************************/

bool Fn::fnLasteltnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

long num1=0;

if (!Arg::num1(_T("lasteltnode"), (DELTS*&)args, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("lasteltnode"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGCHECK:
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,
		_T("[lasteltnode: Error. Must appear in @CHECK or @POST region.]"));
		return parse->errOut(true); // UNFIXED 													// 05/18/01 AM.
	}

if (num1 <= 0)
	{
	_stprintf(Errbuf,_T("[lasteltnode: Error. Given bad num.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}


Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	_stprintf(Errbuf,_T("[lasteltnode: Warning. No collected nodes.]"));
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

Node<Pn> *colls;
Node<Pn> *node;

colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return true;
	}
node = colls->eDown();	// Try to get end of range.
if (!node)
	return true;			// Optional missing elt is ok...

sem = new RFASem(node);	// Return the node.
return true;
}


/********************************************
* FN:		FNDEBUG
* CR:		12/07/00 AM.
* SUBJ:	A debugging aid.
* RET:	True if ok, else false.
* FORMS:	fndebug()
********************************************/

bool Fn::fnDebug(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

return true;
}


/********************************************
* FN:		FNEXITPASS
* CR:		02/06/01 AM.
* SUBJ:	Force immediate exit of current pass.
* RET:	True if ok, else false.
* FORMS:	exitpass()
********************************************/

bool Fn::fnExitpass(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (args || !nlppp)
	{
	_stprintf(Errbuf,_T("[exitpass: Error.]"));
	return false;
	}

switch (nlppp->region_)
	{
	case REGCODE:
	case REGPOST:	// TRYING THIS.	// 11/11/02 AM.
//	case REGDECL:																// 03/12/02 AM.
		break;
	default:
//		_stprintf(Errbuf,_T("[exitpass: Only implemented in @CODE and @POST. Ignoring.]"));
//		return parse->errOut(true); // UNFIXED 													// 05/18/01 AM.
		break;	// 09/15/08 AM.
	}

// Not allowing "exceptions" to be inside a user function.		// 08/18/08 AM.
#ifdef _TOO_STRICT_
if (nlppp->insidefn_)														// 08/18/08 AM.
		{
		_stprintf(Errbuf,_T("[exitpass: Ignoring inside a function.]"));	// 08/18/08 AM.
		return parse->errOut(true);										// 08/18/08 AM.
		}
#endif

if (nlppp)
	{
	nlppp->fail_ = true;			// Done with rule.			// FIX.	// 09/21/08 AM.
//	nlppp->succeed_ = true;			// Done with actions.	// FIX	// 09/21/08 AM.
//	nlppp->return_  = true;		// If in fn, return immediately.	// 03/13/02 AM.
	nlppp->exitpass_ = true;		// Done with pass.
	}
return true;
}



/********************************************
* FN:		FNTODAY
* CR:		02/08/01 AM.
* SUBJ:	Return current date and time as a string.
* RET:	True if ok, else false.
* FORMS:	today()
********************************************/

bool Fn::fnToday(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

if (args || !nlppp)
	{
	_stprintf(Errbuf,_T("[today: Error.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Intern the today string.
_TCHAR *buf = today();
if (!buf || !*buf)
	return false;
_TCHAR *str=0;
parse->internStr(buf, /*UP*/ str);
if (!str || !*str)
	return false;

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNPNCOPYVARS
* CR:		05/16/01 AM.
* SUBJ:	Copy variables and values to suggested node.
* RET:	True if ok, else false.
* FORMS:	pncopyvars(pnode)
*			pncopyvars(num)
*			pncopyvars(0) or pncopyvars() -- last elt's node.
*			pncopyvars(fmpnode,topnode)	11/13/01 AM.
********************************************/

bool Fn::fnPncopyvars(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1 = 0;
_TCHAR *str1=0;
long num= 0;			// Default is 0th or last elt's node.
float fnum=0; // unused // 12/15/14 AM.

RFASem *sem2 = 0;																// 11/13/01 AM.

// Allow empty arg.
if (args)
	Arg::any1(_T("pncopyvars"),parse,(DELTS*&)args, /*UP*/ str1, num,fnum, sem1);
if (args)																		// 11/13/01 AM.
	Arg::sem1(_T("pncopyvars"),nlppp,(DELTS*&)args, /*UP*/ sem2);						// 11/13/01 AM.
if (!Arg::done((DELTS*)args, _T("pncopyvars"),parse))
	return false;

if (str1)
	{
	_stprintf(Errbuf,_T("[pncopyvars: Arg must be num or pnode.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

Node<Pn> *node = 0;
if (!sem1)		// If it's numeric arg.
	{
	// Get node from numth rule element.
	// If zero or more than one, error.
	Node<Pn> *nstart, *nend;
	if (!Pat::collectNthnew(nlppp->collect_, num, /*UP*/nstart,nend))
		return false;
	if (nstart != nend)
		{
		_stprintf(Errbuf,_T("[pncopyvars: Element matched multiple nodes.]"));
		return parse->errOut(true); // UNFIXED 													// 05/18/01 AM.
		}
	node = nstart;
	}
else
	{
	// Get node from sem.
	if (sem1->getType() != RSNODE)
		{
		_stprintf(Errbuf,_T("[pncopyvars: Bad semantic arg.]"));
		return parse->errOut(false); // UNFIXED 												// 05/18/01 AM.
		}
	node = sem1->getNode();
	}

if (!node)
	{
	_stprintf(Errbuf,_T("[pncopyvars: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
	}

// Got a node.  Now copy vars to destination node.
Pn *pn = node->getData();
Dlist<Ipair> *fmlist = pn->getDsem();

Node<Pn> *node2 = 0;															// 11/13/01 AM.
Pn *pn2 = 0;																	// 11/13/01 AM.
Dlist<Ipair> *tolist = 0;													// 11/13/01 AM.
if (sem2)																		// 11/13/01 AM.
	{
	if (sem2->getType() != RSNODE)										// 11/13/01 AM.
		{
		_stprintf(Errbuf,_T("[pncopyvars: Bad semantic arg(2).]"));		// 11/13/01 AM.
		parse->errOut(false); // UNFIXED 															// 11/13/01 AM.
		return true;															// 11/13/01 AM.
		}
	node2 = sem2->getNode();												// 11/13/01 AM.
	pn2 = node2->getData();													// 11/13/01 AM.
   tolist = pn2->getDsem();												// 11/13/01 AM.
	Var::copy_vars(fmlist,tolist);										// 11/13/01 AM.
	pn2->setDsem(tolist);			// Restore to destination.		// 11/13/01 AM.
	}
else										// The old way (to sugg).		// 11/13/01 AM.
	{
	tolist = nlppp->getDsem();		// From suggested elt.			// 11/13/01 AM.
	//Dlist<Ipair> *tolist = nlppp->getDsem();						// 11/13/01 AM.
	Var::copy_vars(fmlist,tolist);

	// Restore var list in parser state.
	nlppp->setDsem(tolist);
	}

return true;
}


/********************************************
* FN:		FNPNMAKEVAR
* CR:		06/26/01 AM.
* SUBJ:	Make a PNODE variable.
* RET:	True if ok, else false.
* FORMS:	pnmakevar(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Fn::fnPnmakevar(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

enum Iargtype typ;	// 12/15/14 AM.

RFASem *sem1;
_TCHAR *name1=0;
_TCHAR *tname=0;	// 07/24/07 AM.
RFASem *semval;
long numval=0;
float fnum=0; // 12/15/14 AM.
_TCHAR *strval=0;


if (!Arg::sem1(_T("pnmakevar"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("pnmakevar"), /*UP*/ (DELTS*&)args, tname))
	return false;
if (!Arg::any1(_T("pnmakevar"),parse,(DELTS*&)args,/*UP*/typ,strval,numval,fnum,semval))
	return false;
if (!Arg::done((DELTS*)args, _T("pnmakevar"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[pnmakevar: Warning. Given no pnode.]"));
	return parse->errOut(true); // UNFIXED
	}
if (!tname)
	{
	_stprintf(Errbuf,_T("[pnmakevar: Warning. Given no name.]"));
	return parse->errOut(true); // UNFIXED
	}

parse->internStr(tname, /*UP*/name1);	// Intern str.	// 07/24/07 AM.

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnmakevar: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnmakevar: Couldn't fetch node.]"));
	return parse->errOut(true); // UNFIXED
	}

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnmakevar: Variable name can't start with '$'.]"));
	return parse->errOut(true); // UNFIXED
	}

// If variable exists, error.
if (Ivar::nodeVarhasval(node, name1))
	{
	_stprintf(Errbuf,_T("[pnmakevar: Node variable already exists with a value.]"));
	return parse->errOut(true); // UNFIXED
	}

Pn *pn = node->getData();

switch (typ) // 12/15/14 AM.
	{
	case IASEM:
		if (semval)
			{
			// RSARGS
			// Need to COPY the sem object here.
			if (semval->getType() == RSARGS)							// FIX.	// 08/07/03 AM.
				{
				// NEED TO COPY ARGS LIST.								// FIX.	// 08/07/03 AM.
				Dlist<Iarg> *xargs =										// FIX.	// 08/07/03 AM.
						Iarg::copy_args(semval->getArgs(),			// FIX.	// 08/07/03 AM.
													parse,true);						// 01/09/07 AM.
				Ivar::nodeReplaceval(pn, name1, xargs);			// FIX.	// 08/07/03 AM.
				}
			else																// FIX.	// 08/07/03 AM.
				{
				RFASem *cpsem = semval->copy_rfasem(parse,true);
				Ivar::nodeReplaceval(pn, name1, cpsem);
				}
			return true;
			}
		break;
	case IASTR:
		if (strval && *strval)
			{
			_TCHAR *x;											// Intern string.	// 01/09/07 AM.
			nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
			Ivar::nodeReplaceval(pn, name1, x);
			return true;
			}
		break;
	case IAFLOAT:
		Ivar::nodeReplaceval(pn, name1, numval);
		return true;
		break;
	case IANUM:
		Ivar::nodeReplaceval(pn, name1, numval);
		return true;
		break;
	default:
		Ivar::nodeReplaceval(pn,name1,(long)0);         // 12/15/14 AM.
		return false;	// Unhandled type.
		break;
	}

// Fell through. Recover from empty case. // 12/15/14 AM.
Ivar::nodeReplaceval(pn,name1,(long)0);         // 12/15/14 AM.
return false; // 12/15/14 AM.
}


/********************************************
* FN:		FNPNREPLACEVAL
* CR:		06/27/01 AM.
* SUBJ:	Replace value of a PNODE variable.
* RET:	True if ok, else false.
* FORMS:	pnreplaceval(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Fn::fnPnreplaceval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

enum Iargtype typ;	// 12/15/14 AM.

RFASem *sem1;
_TCHAR *name1=0;
RFASem *semval;
long numval=0;
float fnum=0; // 12/15/14 AM.
_TCHAR *strval=0;


if (!Arg::sem1(_T("pnreplaceval"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("pnreplaceval"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::any1(_T("pnreplaceval"),parse,(DELTS*&)args,/*UP*/ typ,strval,numval,fnum,semval))
	return false;
if (!Arg::done((DELTS*)args, _T("pnreplaceval"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Warning. Given no pnode.]"));
	return parse->errOut(true); // UNFIXED
	}
if (!name1)
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Warning. Given no name.]"));
	return parse->errOut(true); // UNFIXED
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Couldn't fetch node.]"));
	return parse->errOut(true); // UNFIXED
	}

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnreplaceval: Variable name can't start with '$'.]"));
	return parse->errOut(true); // UNFIXED
	}

Pn *pn = node->getData();

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

switch (typ)	// 12/15/14 AM.
	{
	case IASEM:
		if (semval)
			{
			// RSARGS
			// Need to COPY the sem object here.
			if (semval->getType() == RSARGS)					// Quick fix.	// 09/23/02 AM.
				{
				// NEED TO COPY ARGS LIST.								// FIX.	// 09/23/02 AM.
				Dlist<Iarg> *xargs =										// FIX.	// 09/23/02 AM.
						Iarg::copy_args(semval->getArgs(),			// FIX.	// 09/23/02 AM.
													parse,true);						// 01/09/07 AM.
		//		RFASem *cpsem = new RFASem(xargs);								// 11/14/02 AM.
		//		Ivar::nodeReplaceval(pn, name1, cpsem);						// 11/14/02 AM.
				Ivar::nodeReplaceval(pn, name2, xargs);			// FIX.	// 11/14/02 AM.
				}
			else																			// 09/23/02 AM.
				{
				RFASem *cpsem = semval->copy_rfasem(parse,true);
				Ivar::nodeReplaceval(pn, name2, cpsem);
				}
			}
		else
			return false;
		break;
	case IASTR:
		if (strval && *strval)
			{
			_TCHAR *x;											// Intern string.	// 01/09/07 AM.
			nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
			Ivar::nodeReplaceval(pn, name2, x);
			}
		else
			return false;
		break;
	case IAFLOAT:
		Ivar::nodeReplaceval(pn, name2, fnum);
		break;
	case IANUM:
		Ivar::nodeReplaceval(pn, name2, numval);
		break;
	default:
		return false;
		break;
	}

return true;
}


/********************************************
* FN:		FNPNPUSHVAL
* CR:		12/12/14 AM.
* SUBJ:	Push value onto a PNODE variable's field.
* RET:	True if ok, else false.
* FORMS:	pnpushval(pnode, var_str, int_val/str_val/sem_val)
********************************************/

bool Fn::fnPnpushval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

enum Iargtype typ;	// 12/15/14 AM.

RFASem *sem1;
_TCHAR *name1=0;
RFASem *semval;
long numval=0;
float fnum=0; // 12/15/14 AM.
_TCHAR *strval=0;


if (!Arg::sem1(_T("pnpushval"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("pnpushval"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::any1(_T("pnpushval"),parse,(DELTS*&)args,/*UP*/ typ,strval,numval,fnum,semval))
	return false;
if (!Arg::done((DELTS*)args, _T("pnpushval"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[pnpushval: Warning. Given no pnode.]"));
	return parse->errOut(true); // UNFIXED
	}
if (!name1)
	{
	_stprintf(Errbuf,_T("[pnpushval: Warning. Given no name.]"));
	return parse->errOut(true); // UNFIXED
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnpushval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnpushval: Couldn't fetch node.]"));
	return parse->errOut(true); // UNFIXED
	}

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnpushval: Variable name can't start with '$'.]"));
	return parse->errOut(true); // UNFIXED
	}

Pn *pn = node->getData();

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.


switch (typ) // 12/15/14 AM.
	{
	case IASEM:
		if (semval)
			{
			// RSARGS
			// Need to COPY the sem object here.
			if (semval->getType() == RSARGS)					// Quick fix.	// 09/23/02 AM.
				{
				RFASem *cpsem = new RFASem(semval->getArgs());				// 12/13/14 AM.
				Ivar::nodeReplaceval(pn, name2, cpsem,false,true);		// 12/13/14 AM.

				// NEED TO COPY ARGS LIST.								// FIX.	// 09/23/02 AM.
		//		Dlist<Iarg> *xargs =										// FIX.	// 09/23/02 AM.
		//				Iarg::copy_args(semval->getArgs(),			// FIX.	// 09/23/02 AM.
		//											parse,true);						// 01/09/07 AM.
		//		RFASem *cpsem = new RFASem(xargs);								// 11/14/02 AM.
		//		Ivar::nodeReplaceval(pn, name1, cpsem);						// 11/14/02 AM.
		//		Ivar::nodeReplaceval(pn, name2, xargs);			// FIX.	// 11/14/02 AM.
				}
			else																			// 09/23/02 AM.
				{
				RFASem *cpsem = semval->copy_rfasem(parse,true);
				Ivar::nodeReplaceval(pn, name2, cpsem,false,true);
				}
			}
		// TODO: Push zero or flag error.
		else
			return false;
		break;
	case IASTR:
		if (strval && *strval)
			{
			_TCHAR *x;											// Intern string.	// 01/09/07 AM.
			nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
			Ivar::nodePushval(pn, name2, x,false,true); // 12/14/14 AM.
			}
		// TODO: Push zero or flag error.
		else
			return false;
		break;
	case IANUM:
		Ivar::nodeReplaceval(pn, name2, numval,false,true);
		break;
	case IAFLOAT:
		Ivar::nodeReplaceval(pn, name2, fnum,false,true);
		break;
	default:
		return false;
		break;
	}

return true;
}



/********************************************
* FN:		FNPNRPUSHVAL
* CR:		12/12/14 AM.
* SUBJ:	Push value onto end of a PNODE variable's field.
* RET:	True if ok, else false.
* FORMS:	pnrpushval(pnode, var_str, int_val/str_val/sem_val)
* NOTE:	Allow pushing zero? Why not.
********************************************/

bool Fn::fnPnrpushval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

enum Iargtype typ;	// 12/15/14 AM.

RFASem *sem1;
_TCHAR *name1=0;
RFASem *semval;
long numval=0;
float fnum=0;	// 12/15/14 AM.
_TCHAR *strval=0;


if (!Arg::sem1(_T("pnrpushval"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("pnrpushval"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::any1(_T("pnrpushval"),parse,(DELTS*&)args,/*UP*/ typ,strval,numval,fnum,semval))
	return false;
if (!Arg::done((DELTS*)args, _T("pnrpushval"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[pnrpushval: Warning. Given no pnode.]"));
	return parse->errOut(true); // UNFIXED
	}
if (!name1)
	{
	_stprintf(Errbuf,_T("[pnrpushval: Warning. Given no name.]"));
	return parse->errOut(true); // UNFIXED
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnrpushval: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnrpushval: Couldn't fetch node.]"));
	return parse->errOut(true); // UNFIXED
	}

if (*name1 == '$')												// Get special var.
	{
	_stprintf(Errbuf,_T("[pnrpushval: Variable name can't start with '$'.]"));
	return parse->errOut(true); // UNFIXED
	}

Pn *pn = node->getData();

_TCHAR *name2=0;	// 07/24/07 AM.
nlppp->parse_->internStr(name1, /*UP*/name2);	// Intern str.	// 07/24/07 AM.

switch (typ) // 12/15/14 AM.
	{
	case IASEM:
		if (semval)
			{
			// RSARGS
			// Need to COPY the sem object here.
			if (semval->getType() == RSARGS)					// Quick fix.	// 09/23/02 AM.
				{
				RFASem *cpsem = new RFASem(semval->getArgs());				// 12/13/14 AM.
				Ivar::nodeReplaceval(pn, name2, cpsem,false,false);		// 12/13/14 AM.

				// NEED TO COPY ARGS LIST.								// FIX.	// 09/23/02 AM.
		//		Dlist<Iarg> *xargs =										// FIX.	// 09/23/02 AM.
		//				Iarg::copy_args(semval->getArgs(),			// FIX.	// 09/23/02 AM.
		//											parse,true);						// 01/09/07 AM.
		//		RFASem *cpsem = new RFASem(xargs);								// 11/14/02 AM.
		//		Ivar::nodeReplaceval(pn, name1, cpsem);						// 11/14/02 AM.
		//		Ivar::nodeReplaceval(pn, name2, xargs);			// FIX.	// 11/14/02 AM.
				}
			else																			// 09/23/02 AM.
				{
				RFASem *cpsem = semval->copy_rfasem(parse,true);
				Ivar::nodeReplaceval(pn, name2, cpsem,false,false);
				}
			}
		// TODO: Push zero or flag error.
		else
			return false;
		break;
	case IASTR:
		if (strval && *strval)
			{
			_TCHAR *x;											// Intern string.	// 01/09/07 AM.
			nlppp->parse_->internStr(strval,/*UP*/ x);						// 01/09/07 AM.
			Ivar::nodePushval(pn, name2, x,false,false); // 12/14/14 AM.
			}
		// TODO: Push zero or flag error.
		else
			return false;
		break;
	case IANUM:
		Ivar::nodeReplaceval(pn, name2, numval,false,false);
		break;
	case IAFLOAT:
		Ivar::nodeReplaceval(pn, name2, fnum,false,false);
		break;
	default:
		return false;
		break;
	}

return true;
}


/********************************************
* FN:	   FNSTRESCAPE
* CR:	   07/24/01 Dd.
* SUBJ: Places the escape character in front of the given characters.
* RET:  True if ok, else false.
*		       UP - returns the substituted version of the string
* FORMS:	strescape(str, charsToEscapeStr, escapeChar)
* NOTE:
********************************************/

bool Fn::fnStrescape(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *charsToEscapeStr=0;
_TCHAR *escapeChar=0;

if (!Arg::str1(_T("strescape"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strescape"), /*UP*/ (DELTS*&)args, charsToEscapeStr))
    return false;
if (!Arg::str1(_T("strescape"), /*UP*/ (DELTS*&)args, escapeChar))
    return false;
if (!Arg::done((DELTS*)args, _T("strescape"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strsubst: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[100000];
str_escape(name1, charsToEscapeStr, escapeChar[0], buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:	   FNSTRUNESCAPE
* CR:	   07/24/01 Dd.
* SUBJ: Removes the escape character in front of the given characters.
* RET:  True if ok, else false.
*		       UP - returns the substituted version of the string
* FORMS:	strunescape(str, charsToUnescapeStr, escapeChar)
* NOTE:
********************************************/

bool Fn::fnStrunescape(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name1=0;
_TCHAR *charsToUnescapeStr=0;
_TCHAR *escapeChar=0;

if (!Arg::str1(_T("strunescape"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("strunescape"), /*UP*/ (DELTS*&)args, charsToUnescapeStr))
    return false;
if (!Arg::str1(_T("strunescape"), /*UP*/ (DELTS*&)args, escapeChar))
    return false;
if (!Arg::done((DELTS*)args, _T("strunescape"),parse))
	return false;

if (!name1)																		// 10/30/00 AM.
	{
	_stprintf(Errbuf,_T("[strsubst: Warning. Given no str.]"));			// 05/18/01 AM.
	return parse->errOut(true); // UNFIXED 														// 05/18/01 AM.
	}

_TCHAR buf[100000];
str_unescape(name1, charsToUnescapeStr, escapeChar[0], buf);
if (!buf[0])														// FIX.	// 02/09/01 AM.
	return true;													// FIX.	// 02/09/01 AM.

_TCHAR *str;
parse->internStr(buf, /*UP*/ str);

// Return as str type.
sem = new RFASem(str, RSSTR);

return true;
}


/********************************************
* FN:		FNKBDUMPTREE
* CR:		08/06/01 AM.
* SUBJ:	Modular dump of kb subtree to command file.
* RET:	True if ok, else false.
* FORMS:	kbdumptree(conc, filestr)
********************************************/

bool Fn::fnKbdumptree(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *name1=0;

if (!Arg::sem1(_T("kbdumptree"),nlppp,(DELTS*&)args,sem1))
	return true;
if (!Arg::str1(_T("kbdumptree"), /*UP*/ (DELTS*&)args, name1))
	return true;
if (!Arg::done((DELTS*)args, _T("kbdumptree"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[kbdumptree: Warning. Given no concept.]"));
	return parse->errOut(false); // UNFIXED
	}
if (!name1)
	{
	_stprintf(Errbuf,_T("[kbdumptree: Warning. Given no name.]"));
	return parse->errOut(false); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[kbdumptree: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc1 = sem1->getKBconcept();

bool ok = cg->dumpTree(conc1, name1);

sem = new RFASem(ok ? (long)1 : (long)0);
return true;
}


/********************************************
* FN:		FNSTRISUPPER
* CR:		11/20/01 AM.
* SUBJ:	See if first char of string is upper case.
* RET:	True if ok, else false.
*			UP bool - 1 if upper, else 0.
* FORM:	strisupper(str)
********************************************/

bool Fn::fnStrisupper(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *str1=0;
if (!Arg::str1(_T("isupper"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("isupper"),parse))
	return false;

long flag=0;
if (str1 && *str1 && is_upper(*str1))									// 12/16/01 AM.
	flag = 1;
sem = new RFASem(flag);
return true;
}


/********************************************
* FN:		FNSTRISLOWER
* CR:		11/20/01 AM.
* SUBJ:	See if first char of string is lower case.
* RET:	True if ok, else false.
*			UP bool - 1 if upper, else 0.
* FORM:	strislower(str)
********************************************/

bool Fn::fnStrislower(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.


_TCHAR *str1=0;
if (!Arg::str1(_T("islower"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("islower"),parse))
	return false;

long flag=0;
if (str1 && *str1
	 && is_lower(*str1))														// 12/16/01 AM.
	flag = 1;
sem = new RFASem(flag);
return true;
}


/********************************************
* FN:	   FNSUFFIX
* CR:	   11/21/01 AM.
* SUBJ:	See if reasonable suffix of given word.
* RET:	True if ok, else false.
*	 UP - True if good suffix.
* FORMS:	suffix(str, suffix_str)
* NOTE:	Using words array to study word.  Morphological grounds only.
********************************************/

bool Fn::fnSuffix(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *name1=0;
_TCHAR *suffix=0;

if (!Arg::str1(_T("suffix"), /*UP*/ (DELTS*&)args, name1))
	return false;
if (!Arg::str1(_T("suffix"), /*UP*/ (DELTS*&)args, suffix))
	return false;
if (!Arg::done((DELTS*)args, _T("suffix"),parse))
	return false;


if (!name1 || !*name1 || !suffix || !*suffix)
	{
	_stprintf(Errbuf,_T("[suffix: Warning: Given no str or suffix.]"));
	return parse->errOut(true); // UNFIXED
	}

bool flag = strsuffix(name1,suffix);
long retval = (flag ? true : false);

sem = new RFASem(retval);

return true;
}


/********************************************
* FN:		FNMKDIR
* CR:		12/06/01 AM.
* SUBJ:	Make directory.
* RET:	True if ok.
* FORMS:	mkdir(str);
********************************************/

bool Fn::fnMkdir(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *str;

if (!Arg::str1(_T("mkdir"), /*UP*/ (DELTS*&)args, str))
	return false;
if (!Arg::done((DELTS*)args, _T("mkdir"),parse))
	return false;

if (!str|| !*str)
	{
	_stprintf(Errbuf,_T("[mkdir: Warning. Given no directory.]"));
	return parse->errOut(true); // UNFIXED
	}

bool ok = true;

#ifdef _OLDWAY_
#ifndef LINUX
ok = (_tmkdir(str) ? false : true);
#else
_TCHAR cmd[2048];
_stprintf(cmd,_T("mkdir %s"), str);
ok = (_tsystem(cmd) ? false : true);
#endif
#endif

_TCHAR cmd[2048];	// 09/08/28 AM.
_stprintf(cmd,_T("mkdir %s"), str);	// 09/08/28 AM.
run_silent(cmd);			// system() w/o DOC CMD PROMPT POPUP.	// 09/28/08 AM.
ok = true;	// 09/28/08 AM.

if (!ok)
	{
	_stprintf(Errbuf,_T("[mkdir: Warning. Couldn't create directory=%s.]"),str);
	sem = new RFASem((long)0);	// 04/12/03 AM.
	return parse->errOut(true); // UNFIXED
	}

sem = new RFASem((long)1);	// 04/12/03 AM.
return true;
}



/********************************************
* FN:		FNSTEM
* CR:		12/28/01 AM.
* SUBJ:	Stem nouns and verbs.
* RET:	str - stem of noun or verb, else return what's given.
* FORMS:	stem(word_str)
********************************************/

bool Fn::fnStem(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *word;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("stem"), /*UP*/ (DELTS*&)args, word))
	return false;
if (!Arg::done((DELTS*)args, _T("stem"),parse))
	return false;

_TCHAR *buf=0;
if (!(buf = strstem(word)))
	return true;

_TCHAR *str=0;
parse->internStr(buf, /*UP*/ str);
Chars::destroy(buf);

sem = new RFASem(str, RSSTR);
return true;
}


/********************************************
* FN:		FNCOUT
* CR:		05/05/02 AM.
* SUBJ:	Return the COUT output stream.
* RET:	ostream - output-stream value.
* FORMS:	cout() - cout goes to standard output.
* NOTE:	By calling nlp->analyze() with an output stream, user can
*			rebind cout.
********************************************/

bool Fn::fnCout(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("cout"),parse))
	return false;
sem = new RFASem(parse->getCout());										// 05/04/03 AM.
return true;
}


/********************************************
* FN:		FNCOUTRESET
* CR:		05/06/02 AM.
* SUBJ:	Rebind COUT to the specified filename.
* RET:	ostream - new output-stream value.
* FORMS:	coutreset(filename) - cout goes to specified file.
*			coutreset() - [unimplemented] could reset to user-supplied stream.
* NOTE:	Could distinguish between user-supplied stream, standard out, and
*			redirect to file.
********************************************/

bool Fn::fnCoutreset(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *fname = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::str1(_T("coutreset"), /*UP*/ (DELTS*&)args, fname))
	return false;
if (!Arg::done((DELTS*)args, _T("coutreset"),parse))
	return false;

// Check for good filename.
// Set up output stream.
sem = Arun::openfile(nlppp,fname, _T("app"));
if (!sem)
	{
	_stprintf(Errbuf,_T("[coutreset: Couldn't set up file=%s.]"), str(fname));
	return parse->errOut(true); // UNFIXED
	}

// REBIND COUT.
//if (sem->getOstream())			// XP NEEDS REDO OF THIS.	// 01/12/03 AM.
//	cout = *(sem->getOstream());	// XP NEEDS REDO OF THIS.	// 01/12/03 AM.

return true;
}


/********************************************
* FN:		FNCBUF
* CR:		05/11/02 AM.
* SUBJ:	Return the CBUF output stream.
* RET:	ostrstream - output-stream value.
* FORMS:	cbuf() - Output buffer stream.
* NOTE:	By calling nlp->analyze() with an output buffer, analyzer can
*			output to cbuf().
********************************************/

bool Fn::fnCbuf(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("cbuf"),parse))
	return false;

sem = new RFASem(nlppp->parse_->getCbuf());
return true;
}


/********************************************
* FN:		FNINTERACTIVE
* CR:		05/06/02 AM.
* SUBJ:	Check if analyzer operating in an interactive environment.
* RET:	True if ok, else false.
* FORMS:	bool interactive()
********************************************/

bool Fn::fnInteractive(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

if (!Arg::done((DELTS*)args, _T("interactive"),parse))
	return false;
bool interactive = parse->getAna()->getNLP()->getFinteractive();
long flag;
if (interactive)
	flag = 1;
else
	flag = 0;

sem = new RFASem(flag);
return true;
}


/********************************************
* FN:		FNPNVARNAMES
* CR:		05/13/02 AM.
* SUBJ:	Get PNODE's variable names in an array.
* RET:	True if ok, else false.
* FORMS:	pnvarnames(pnode)
********************************************/

bool Fn::fnPnvarnames(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

RFASem *sem1;

if (!Arg::sem1(_T("pnvarnames"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("pnvarnames"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[pnvarnames: Warning. Given no pnode.]"));
	return parse->errOut(true); // UNFIXED
	}

// Get object from sem.
if (sem1->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnvarnames: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *node = sem1->getNode();

if (!node)
	{
	_stprintf(Errbuf,_T("[pnvarnames: Couldn't fetch node.]"));
	return parse->errOut(false); // UNFIXED
	}

Dlist<Iarg> *vars = Ivar::nodeVarnames(node->getData());
if (vars)
   sem = new RFASem(vars);
return true;
}

#ifdef _ODBC

/********************************************
* FN:		FNDBOPEN
* CR:		05/23/02 AM.
* SUBJ:	Open ODBC database connection.
* RET:	True if ok, else false.
*			UP success - "boolean" true if success, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			Assume only ONE OPEN DB CONNECTION PER PARSE, FOR NOW.
* FORMS:	dbopen(dbname_str, username_str, password_str)
* EX:		dbopen("myodbc3-test", "root", 0);
********************************************/

bool Fn::fnDbopen(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.
if (!parse)
  return false;

_TCHAR *dbname = 0;
_TCHAR *usrname = 0;
long usrnum = 0;
_TCHAR *password = 0;
long passwordnum = 0;

if (!Arg::str1(_T("dbopen"), /*UP*/ (DELTS*&)args, dbname))
	return true;
if (!Arg::str_or_num1(_T("dbopen"), /*UP*/ (DELTS*&)args, usrname, usrnum))
	return true;
if (!Arg::str_or_num1(_T("dbopen"), /*UP*/ (DELTS*&)args, password, passwordnum))
	return true;
if (!Arg::done((DELTS*)args, _T("dbopen"),parse))
	return true;

if (!dbname || !*dbname)
	{
	_stprintf(Errbuf,_T("[dbopen: Warning. Given no dbname.]"));
	return parse->errOut(true); // UNFIXED
	}
if ((!usrname || !*usrname) && usrnum)
	{
	_stprintf(Errbuf,_T("[dbopen: Error in user name.]"));
	return parse->errOut(true); // UNFIXED
	}
if ((!password || !*password) && passwordnum)
	{
	_stprintf(Errbuf,_T("[dbopen: Error in password.]"));
	return parse->errOut(true); // UNFIXED
	}

if (Arun::dbopen(nlppp,dbname,usrname,password))				// 06/15/02 AM.
	sem = new RFASem((long)1);
return true;
}

/********************************************
* FN:		FNDBCLOSE
* CR:		05/23/02 AM.
* SUBJ:	Close ODBC database connection.
* RET:	True if ok, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			Assume only ONE OPEN DB CONNECTION PER PARSE, FOR NOW.
* FORMS:	dbclose()
********************************************/

bool Fn::fnDbclose(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.
if (!parse)
  return false;

if (!Arg::done((DELTS*)args, _T("dbclose"),parse))
	return true;

parse->dbclose();

sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNDBEXEC
* CR:		05/23/02 AM.
* SUBJ:	Execute ODBC statement.
* RET:	True if ok, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			PLAYING AROUND HERE, for the moment.
*			Assume only ONE OPEN DB CONNECTION PER PARSE, FOR NOW.
* OPT:	May want to store statement handle for reuse.
* FORMS:	dbexec(str)
********************************************/

bool Fn::fnDbexec(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *stmt = 0;
if (!Arg::str1(_T("dbexec"), /*UP*/ (DELTS*&)args, stmt))
	return true;
if (!Arg::done((DELTS*)args, _T("dbexec"),parse))
	return true;

if (Arun::dbexec(nlppp,stmt))
	sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNDBALLOCSTMT
* CR:		05/25/02 AM.
* SUBJ:	Allocate db statement handle.
* RET:	True if ok, else false.
* NOTE:	Using MyODBC 3.51 driver.
*			Can implement NLP++ data types for hstmt, hdbc, henv...
* ASS:	Assume one statement handle open at a time, for now.
* FORMS:	dballocstmt()
********************************************/

bool Fn::fnDballocstmt(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.
if (!parse)
  return false;

if (!Arg::done((DELTS*)args, _T("dballoctstmt"),parse))
	return false;


// ALLOCATE STMT HANDLE. (Todo: make it an NLP++ data type.)
if (!parse->allocDbstmt())
	return true;

sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNDBEXECSTMT
* CR:		05/25/02 AM.
* SUBJ:	Execute using the currently active statement.
* RET:	True if ok, else false.
* NOTE:	Uses db stmt handle saved in parse object.
* FORMS:	dbexecstmt(str)
********************************************/

bool Fn::fnDbexecstmt(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.
if (!parse)
  return false;

_TCHAR *stmt;
if (!Arg::str1(_T("dbexecstmt"), /*UP*/ (DELTS*&)args, stmt))
	return true;
if (!Arg::done((DELTS*)args, _T("dbexecstmt"),parse))
	return true;

SQLTCHAR *statement = (_TUCHAR *)stmt;
if (!statement || !*statement)
	{
	_stprintf(Errbuf,_T("[dbexecstmt: Warning. Given no statement.]"));
	return parse->errOut(true); // UNFIXED
	}

SQLHSTMT   hstmt = parse->getHstmt();
if (!hstmt)
	return true;

// Execute the statement.
SQLExecDirect (hstmt, statement, SQL_NTS);

sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNDBFREESTMT
* CR:		05/25/02 AM.
* SUBJ:	Free ODBC statement.
* RET:	True if ok, else false.
* NOTE:	Free the db stmt handle in the parse object.
* FORMS:	dbfreestmt()
********************************************/

bool Fn::fnDbfreestmt(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;
if (!parse)
  return false;

if (!Arg::done((DELTS*)args, _T("dbfreestmt"),parse))
	return true;

if (Arun::dbfreestmt(nlppp))
	sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNDBFETCH
* CR:		05/26/02 AM.
* SUBJ:	Fetch result using the currently active statement.
* RET:	True if ok, else false.
* NOTE:	Uses db stmt handle saved in parse object.
* FORMS:	dbfetch()
********************************************/

bool Fn::fnDbfetch(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;

if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.
if (!parse)
  return false;

if (!Arg::done((DELTS*)args, _T("dbfetch"),parse))
	return true;

if (Arun::dbfetch(nlppp))
	sem = new RFASem((long)1);
return true;
}

#endif // _ODBC

/********************************************
* FN:		FNEXITTOPOPUP
* CR:		05/24/02 AM.
* SUBJ:	Exit current pass, preparing for GUI popup.
* RET:	True if ok, else false.
* NOTE:
* FORMS:	exittopopup(message_str, type_str)
*				The meaning of these arguments depends on the GUI implementation
*				of the popup window.
********************************************/

bool Fn::fnExittopopup(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

_TCHAR *msg_str = 0;
_TCHAR *typ_str = 0;
if (!Arg::str1(_T("exittopopup"), /*UP*/ (DELTS*&)args, msg_str))
	return true;
if (!Arg::str1(_T("exittopopup"), /*UP*/ (DELTS*&)args, typ_str))
	return true;
if (!Arg::done((DELTS*)args, _T("exittopopup"),parse))
	return true;

if (Arun::exittopopup(nlppp,msg_str,typ_str))
	sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNGETPOPUPDATA
* CR:		05/25/02 AM.
* SUBJ:	Fetch data from GUI popup.
* RET:	True if ok, else false.
* NOTE:	ALSO CLEARS THE POPUP DATA.
*			Gets data that user typed into the popup, as a string.
* FORMS:	str getpopupdata()
********************************************/

bool Fn::fnGetpopupdata(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

if (!Arg::done((DELTS*)args, _T("getpopupdata"),parse))
	return true;

_TCHAR *str = Arun::getpopupdata(nlppp);

// Return as string type.
sem = new RFASem(str, RSSTR);
return true;
}

#ifdef _ODBC

/********************************************
* FN:		FNDBBINDCOL
* CR:		05/26/02 AM.
* SUBJ:	Bind to column of db result set.
* RET:	True if ok, else false.
* NOTE:	Uses db stmt handle saved in parse object.
*			FIRST USE OF CALL BY REFERENCE.
* FORMS:	dbbindcol(column_num, type_str, bsiz, &var, &flagvar)
********************************************/

bool Fn::fnDbbindcol(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

long column_num = 0;
_TCHAR *typ = 0;
long bsiz = 0;
RFASem *var = 0;
RFASem *flagv = 0;															// 06/12/02 AM.
if (!Arg::num1(_T("dbbindcol"), /*UP*/ (DELTS*&)args, column_num))
	return true;
if (!Arg::str1(_T("dbbindcol"), /*UP*/ (DELTS*&)args, typ))
	return true;
if (!Arg::num1(_T("dbbindcol"), /*UP*/ (DELTS*&)args, bsiz))
	return true;
if (!Arg::ref1(_T("dbbindcol"), /*UP*/ (DELTS*&)args, var))
	return true;
if (!Arg::ref1(_T("dbbindcol"), /*UP*/ (DELTS*&)args, flagv))					// 06/12/02 AM.
	return true;
if (!Arg::done((DELTS*)args, _T("dbbindcol"),parse))
	return true;

// 06/15/02 AM.
// The version below won't delete vars, in interp mode.			// 03/10/03 AM.
bool success = Arun::dbbindcol_int(nlppp,column_num,typ,bsiz,var,flagv);

if (success)
	sem = new RFASem((long)1);
else
	sem = new RFASem((long)0);
return true;
}


/********************************************
* FN:		DBBIND2STR
* CR:		06/08/02 AM.
* SUBJ:	Bind database column to NLP++ string type.
* RET:	True if ok, else false.
********************************************/

bool Fn::dbbind2str(
	Nlppp *nlppp,
	RFASem *var,
	SQLSMALLINT sqltyp,
	long column_num,
	long bsiz,
	RFASem *flagv
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;
if (!parse)
  return false;
Ana *ana = parse->getAna();
NLP *nlp = ana->getNLP();
Stab *stab = (Stab *) nlp->getStab();

// Assign buffer to NLP++ variable.
// OPT: Some redundancy in operations here.  unpacking var mainly to get
// dynamic evaluation of the index expression.
Ivar *ivar = 0;
Ipair *pairx = 0;
Dlist<Iarg> *vals = 0;
long index = 0;
if (!Iexpr::unpackVar(var, nlppp, /*UP*/ ivar, pairx, vals, index))
	{
	_stprintf(Errbuf,_T("[dbbindcol: Couldn't get variable.]"));
	return parse->errOut(false); // UNFIXED
	}

// Allocate a buffer for the variable in the string table.
_TCHAR *buf = 0;					// 02/27/03 AM.
if (bsiz >= STAB_SEG_SIZE)	// 02/27/03 AM.
	{
	Delt<Iarg> *delt = parse->newblob(bsiz);	// 02/27/03 AM.
	Iarg *arg = delt->getData();					// 02/27/03 AM.
	buf = arg->getStr();								// 02/27/03 AM.
	}
else								// 02/27/03 AM.
	buf = stab->addStr(bsiz);
if (!buf)
	{
	_stprintf(Errbuf,_T("[dbbindcol: Couldn't allocate buffer for column.]"));
	return parse->errOut(false); // UNFIXED
	}

Arun::assign(	// Using compiled assignment style, quick and dirty.
	ivar->getType(),
	ivar->getName(),
	ivar->getNum(),
	index,
	nlppp,
	buf);

long *addr2 = bindvar(nlppp, flagv);									// 06/12/02 AM.
if (!addr2)
	return false;

// Bind buffer to database result column.
SDWORD *pcursor = parse->getPcursor();
SQLRETURN  retcode = SQLBindCol(
	parse->getHstmt(),
	// column_num, // LINUX?
	(SQLSMALLINT) column_num,
	sqltyp,
	buf,
	bsiz,
	addr2																			// 06/12/02 AM.
	);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	_stprintf(Errbuf,_T("[dbbindcol: Couldn't bind column.]"));
	return parse->errOut(false); // UNFIXED
	}

return true;
}


/********************************************
* FN:		DBBIND2LONG
* CR:		06/08/02 AM.
* SUBJ:	Bind database column to NLP++ long int type.
* RET:	True if ok, else false.
* NOTE:	Will eventually need NLP++ unsigned type, etc.
********************************************/

bool Fn::dbbind2long(
	Nlppp *nlppp,
	RFASem *var,
	SQLSMALLINT sqltyp,
	long column_num,
	RFASem *flagv
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;
if (!parse)
  return false;
//Ana *ana = parse->getAna();
//NLP *nlp = ana->getNLP();

// Assign to NLP++ variable.
long *addr = bindvar(nlppp, var);										// 06/12/02 AM.
if (!addr)
	return false;

long *addr2 = bindvar(nlppp, flagv);									// 06/12/02 AM.
if (!addr2)
	return false;

// Bind buffer to database result column.
SDWORD *pcursor = parse->getPcursor();
SQLRETURN  retcode = SQLBindCol(
	parse->getHstmt(),
	(SQLSMALLINT) column_num,
	sqltyp,
	addr,
	0,		// Data size
	addr2																			// 06/12/02 AM.
	);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	_stprintf(Errbuf,_T("[dbbindcol: Couldn't bind column.]"));
	return parse->errOut(false); // UNFIXED
	}

return true;
}


/********************************************
* FN:		DBBIND2FLOAT
* CR:		06/08/02 AM.
* SUBJ:	Bind database column to NLP++ float type.
* RET:	True if ok, else false.
********************************************/

bool Fn::dbbind2float(
	Nlppp *nlppp,
	RFASem *var,
	SQLSMALLINT sqltyp,
	long column_num,
	RFASem *flagv
	)
{
if (!nlppp)
  return false;
Parse *parse = nlppp->parse_;
if (!parse)
  return false;
//Ana *ana = parse->getAna();
//NLP *nlp = ana->getNLP();

// Assign to NLP++ variable.
// OPT: Some redundancy in operations here.  unpacking var mainly to get
// dynamic evaluation of the index expression.
Ivar *ivar = 0;
Ipair *pairx = 0;
Dlist<Iarg> *vals = 0;
long index = 0;
if (!Iexpr::unpackVar(var, nlppp, /*UP*/ ivar, pairx, vals, index))
	{
	_stprintf(Errbuf,_T("[dbbindcol: Couldn't get variable.]"));
	return parse->errOut(false); // UNFIXED
	}

long *addr2 = bindvar(nlppp, flagv);									// 06/12/02 AM.
if (!addr2)
	return false;


// This seems unnecessary/inefficient, if we already have the var above.
Arun::assign(	// Using compiled assignment style, quick and dirty.
	ivar->getType(),
	ivar->getName(),
	ivar->getNum(),
	index,
	nlppp,
	(float) 0.0);	// Just getting a dummy value into variable.

// GET THE ADDRESS OF THE LONG, INSIDE THE VAR.
vals = pairx->getVals();
if (!vals)
	return false;
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	return false;
Iarg *iarg = darg->getData();
if (!iarg)
	return false;
if (iarg->getType() != IAFLOAT)
	return false;
float *addr = iarg->getFloataddr();


// Bind buffer to database result column.
SDWORD *pcursor = parse->getPcursor();
SQLRETURN  retcode = SQLBindCol(
	parse->getHstmt(),
	(SQLSMALLINT) column_num,
	sqltyp,
	addr,
	0,		// Data size
	addr2																			// 06/12/02 AM.
	);
if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
	_stprintf(Errbuf,_T("[dbbindcol: Couldn't bind column.]"));
	return parse->errOut(false); // UNFIXED
	}

return true;
}

// 05/30/02 DD.
SQLSMALLINT Fn::SeqlTypeFromString(_TCHAR *typeStr)
{
	SQLSMALLINT type = SQL_C_CHAR;

	if (strcmp_i(typeStr,_T("bit")) == 0)
		type = SQL_C_BIT;
	else if (strcmp_i(typeStr,_T("tinyint")) == 0)
		type = SQL_C_STINYINT  ;
	else if (strcmp_i(typeStr,_T("bigint")) == 0)
		type = SQL_C_SBIGINT;
	else if (strcmp_i(typeStr,_T("longvarbinary")) == 0
		   || strcmp_i(typeStr,_T("binary")) == 0
			|| strcmp_i(typeStr,_T("varbinary")) == 0
			|| strcmp_i(typeStr,_T("blob")) == 0							// 06/14/02 AM.
			|| strcmp_i(typeStr,_T("longblob")) == 0						// 06/14/02 AM.
			|| strcmp_i(typeStr,_T("mediumblob")) == 0					// 06/14/02 AM.
			|| strcmp_i(typeStr,_T("tinyblob")) == 0						// 06/14/02 AM.
			)
		type = SQL_C_BINARY;
	else if (strcmp_i(typeStr,_T("char")) == 0
		   || strcmp_i(typeStr,_T("longvarchar")) == 0
			|| strcmp_i(typeStr,_T("varchar")) == 0
			|| strcmp_i(typeStr,_T("enum")) == 0							// 06/14/02 AM.
			|| strcmp_i(typeStr,_T("set")) == 0								// 06/14/02 AM.
			)
		type = SQL_C_CHAR;
	else if (strcmp_i(typeStr,_T("integer")) == 0
			|| strcmp_i(typeStr,_T("int")) == 0								// 06/14/02 AM.
			|| strcmp_i(typeStr,_T("mediumint")) == 0						// 06/14/02 AM.
			|| strcmp_i(typeStr,_T("year")) == 0							// 06/14/02 AM.
			)
		type = SQL_C_SLONG;
	else if (strcmp_i(typeStr,_T("smallint")) == 0)
		type = SQL_C_SSHORT;
	else if (strcmp_i(typeStr,_T("float")) == 0
			|| strcmp_i(typeStr,_T("numeric")) == 0
			|| strcmp_i(typeStr,_T("decimal")) == 0
		   || strcmp_i(typeStr,_T("double")) == 0)
		type = SQL_C_DOUBLE;
	else if (strcmp_i(typeStr,_T("real")) == 0
			)
		type = SQL_C_FLOAT;
	else if (strcmp_i(typeStr,_T("date")) == 0)
		type = SQL_C_DATE;
	else if (strcmp_i(typeStr,_T("time")) == 0)
		type = SQL_C_TIME;
	else if (strcmp_i(typeStr,_T("timestamp")) == 0
			|| strcmp_i(typeStr,_T("datetime")) == 0						// 06/14/02 AM.
			)
		type = SQL_C_TIMESTAMP;

	return type;
}

bool Fn::SeqlValidType(_TCHAR *typeStr)
{
static _TCHAR *typeStrs[] = {
		_T("bigint"),		// 06/14/02 AM.
		_T("binary"),
		_T("bit"),
		_T("blob"),			// 06/14/02 AM.
		_T("char"),
		_T("date"),
		_T("datetime"),		// 06/14/02 AM.
		_T("decimal"),
		_T("double"),
		_T("enum"),			// 06/14/02 AM.
		_T("float"),
		_T("int"),			// 06/14/02 AM.
		_T("integer"),
		_T("longblob"),		// 06/14/02 AM.
		_T("longvarbinary"),
		_T("longvarchar"),
		_T("mediumblob"),	// 06/14/02 AM.
		_T("mediumint"),	// 06/14/02 AM.
		_T("numeric"),
		_T("real"),
		_T("set"),			// 06/14/02 AM.
		_T("smallint"),
		_T("time"),
		_T("timestamp"),
		_T("tinyblob"),		// 06/14/02 AM.
		_T("tinyint"),
		_T("varbinary"),
		_T("varchar"),
		_T("year"),			// 06/14/02 AM.
		_T("xxxxx")
	};

	int i = 0;
	_TCHAR *type = typeStrs[i++];
	while (strcmp_i(type,_T("xxxxx"))) {
		if (strcmp_i(type,typeStr) == 0)
			return true;
		type = typeStrs[i++];
	}

	return false;
}


/********************************************
* FN:		BINDVAR
* CR:		06/12/02 AM.
* SUBJ:	Bind and get address of variable value.
* RET:	Address of var's value.
* OPT:	Can be substantially optimized.
********************************************/

long *Fn::bindvar(
	Nlppp *nlppp,
	RFASem *var
	)
{
if (!nlppp || !var)
	return 0;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

// Assign to NLP++ variable.
// OPT: Some redundancy in operations here.  unpacking var mainly to get
// dynamic evaluation of the index expression.
Ivar *ivar = 0;
Ipair *pairx = 0;
Dlist<Iarg> *vals = 0;
long index = 0;
if (!Iexpr::unpackVar(var, nlppp, /*UP*/ ivar, pairx, vals, index))
	{
	_stprintf(Errbuf,_T("[bindvar: Couldn't get variable.]"));
	parse->errOut(false); // UNFIXED
	return 0;
	}


// This seems unnecessary/inefficient, if we already have the var above.
Arun::assign(	// Using compiled assignment style, quick and dirty.
	ivar->getType(),
	ivar->getName(),
	ivar->getNum(),
	index,
	nlppp,
	(long) 0);	// Just getting a dummy value into variable.

// GET THE ADDRESS OF THE LONG, INSIDE THE VAR.
vals = pairx->getVals();
if (!vals)
	return 0;	// 09/26/19 AM.
Delt<Iarg> *darg = vals->getFirst();
if (!darg)
	return 0;	// 09/26/19 AM.
Iarg *iarg = darg->getData();
if (!iarg)
	return 0;	// 09/26/19 AM.
if (iarg->getType() != IANUM)
	return 0;	// 09/26/19 AM.
long *addr = iarg->getNumaddr();

return addr;
}

#endif // _ODBC

/********************************************
* FN:		FNSORTCONSBYATTR
* CR:		12/02/02 AM.
* SUBJ:	Sort an array of kb concepts.
* RET:	True if ok.
* FORMS:	sortconsbyattr(
				array - multi-valued array of kb concepts.
				attr = str.  Name of attribute to sort on.
				numeric = 0 (default) if str, 1 if numeric.
				descending = 0 (default) if ascending, 1 if descending.
********************************************/

bool Fn::fnSortconsbyattr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;
CG *cg = nlppp->parse_->getAna()->getCG();

RFASem *array_sem=0;
_TCHAR *attr=0;
long numeric=0, descending=0;

if (!Arg::sem1(_T("sortconsbyattr"),nlppp,(DELTS*&)args, array_sem))
	return false;
if (!Arg::str1(_T("sortconsbyattr"),(DELTS*&)args, attr))
	return false;
if (Arg::num1(_T("sortconsbyattr"), (DELTS*&)args, numeric))	// Optional args.
	Arg::num1(_T("sortconsbyattr"), (DELTS*&)args, descending);
if (!Arg::done((DELTS*)args, _T("sortconsbyattr"),parse))
	return false;

if (!array_sem)
	{
	_stprintf(Errbuf,_T("[sortconsbyattr: Given no array.]"));
	parse->errOut(false); // UNFIXED
	return true;
	}

Dlist<Iarg> *arr=0;
switch (array_sem->getType())
	{
	case RSARGS:
		arr = array_sem->getArgs();
		break;
	case RS_KBCONCEPT:														// 12/14/02 AM.
		// Reuse the sem, rather than deleting and creating.		// 12/14/02 AM.
//		sem = array_sem;														// 12/14/02 AM.
		// CRASH. The above was not a good shortcut.					// 12/19/05 AM.
		sem = new RFASem(array_sem->getKBconcept(),RS_KBCONCEPT);			// FIX.	// 12/19/05 AM.
		return true;
	default:
		{
		_stprintf(Errbuf,_T("[sortconsbyattr: Given non-array variable.]"));
		parse->errOut(false); // UNFIXED
		return true;
		}
		break;
	}

if (!attr || !*attr)
	{
	_stprintf(Errbuf,_T("[sortconsbyattr: Given no concept attribute.]"));
	parse->errOut(false); // UNFIXED
	return true;
	}

Dlist<Iarg> *sorted = (Dlist<Iarg> *) Arun::sort_arr_kbcon_by_attr(
	arr,									// Array of kbconcepts.
	attr,									// Name of concept attribute.
	(numeric ? true : false),		// True if numeric, false if string.
	(descending ? true : false),	// True if descending, false if ascending.
	cg
	);

sem = new RFASem(sorted);
return true;
}



/********************************************
* FN:		FNINPUTRANGE
* CR:		12/15/02 AM.
* SUBJ:	Get range of offsets of input text.
* RET:	Address of var's value.
* OPT:	Optimizes, rather than getting string from root of parse tree...
********************************************/

bool Fn::fnInputrange(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

long start=0,end=0;

if (!Arg::num1(_T("inputrange"), (DELTS*&)args, start))
	return false;
if (!Arg::num1(_T("inputrange"), (DELTS*&)args, end))
	return false;
if (!Arg::done((DELTS*)args, _T("inputrange"),parse))
	return false;

_TCHAR *text = parse->getText();	// Entire input text.
long length = parse->getLength();	// Length of input text.

if (start >= length || end >= length
 || start < 0 || end < 0
 || start > end)
	{
	_stprintf(Errbuf,_T("[inputrange: Error. Bad offsets.]"));
	parse->errOut(false); // UNFIXED
	return true;
	}

long len = end - start + 1;

_TCHAR *buf = Chars::create(len+1);
_tcsnccpy(buf,&(text[start]),len);
buf[len] = '\0';	// Terminate string.
_TCHAR *str;
parse->internStr(buf, /*UP*/ str);
sem = new RFASem(str, RSSTR);
Chars::destroy(buf);			// Free up buffer.

return true;
}


/********************************************
* FN:		FNFINDANA
* CR:		02/03/03 AM.
* SUBJ:	See if an analyzer is loaded in memory.
* RET:	Bool (1 or 0)
********************************************/

bool Fn::fnFindana(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
// Current analyzer info.		// [DEGLOB]	// 10/15/20 AM.
Parse *parse = nlppp->parse_;
NLP *nlp = parse->getNLP();		// [DEGLOB]	// 10/15/20 AM.
VTRun *vtrun = nlp->getVTRun();	// [DEGLOB]	// 10/15/20 AM.

_TCHAR *appname;

if (!Arg::str1(_T("findana"),(DELTS*&)args, appname))
	return false;
if (!Arg::done((DELTS*)args, _T("findana"),parse))
	return false;


//	NLP *nlp1 = VTRun_Ptr->findAna(appname);	// [DEGLOB]	// 10/15/20 AM.
NLP *nlp1 = vtrun->findAna(appname);			// [DEGLOB]	// 10/15/20 AM.
long retval = (nlp1 ? 1 : 0);

sem = new RFASem(retval);
return true;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)	// 05/06/14 DDH.
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);	// 05/06/14 DDH.
	return written;	// 05/06/14 DDH.
}


/********************************************
* FN:		FNURLTOFILE
* CR:		02/11/03 AM.
* SUBJ:	Get web page from URL.
* RET:	Bool (1 or 0)
* FORM:	urltofile(url_s,outfile_s)
********************************************/

bool Fn::fnUrltofile(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *url_s;
_TCHAR *outfile_s;

if (!Arg::str1(_T("urltofile"),(DELTS*&)args, url_s))
	return false;
if (!Arg::str1(_T("urltofile"),(DELTS*&)args, outfile_s))
	return false;
if (!Arg::done((DELTS*)args, _T("urltofile"),parse))
	return false;

if (!safe_dir(outfile_s))
	{
	_stprintf(Errbuf,_T("[urltofile: Dangerous filename='%s'.]"), outfile_s);
	parse->errOut(false); // UNFIXED
	return true;
	}

long ret = 0;	// FAIL.	// 05/13/14 AM.
#ifdef LINUX
	{
	_stprintf(Errbuf,_T("[urltofile: LINUX version unimplemented]"));
	parse->errOut(false); // UNFIXED
	return true;
	}
#else

//	WebForm wf;	// 05/06/14 DDH.
//	wf.setHost((const char *)url_s);	// 05/06/14 DDH.
//	wf.sendRequest();	// 05/06/14 DDH.
//	if (wf.getFile((char *)outfile_s))	// 05/06/14 DDH.
//		ret = 1;	// 05/06/14 DDH.	// SUCCEED.	// 05/13/14 AM.

//if (tear_url(url_s,outfile_s))	// 03/12/03 AM.
//	ret = 0;								// 03/12/03 AM.

	_TCHAR *str = Arun::urltofile(nlppp,url_s,outfile_s);	// 05/20/14 AM.
	if (str && *str)	// 05/20/14 AM.
	  sem = new RFASem(str, RSSTR);	// 05/20/14 AM.

#endif

//sem = new RFASem(ret);
return true;
}


/********************************************
* FN:		FNURLBASE
* CR:		03/16/03 AM.
* SUBJ:	Get directory path from URL.
* RET:	Normalized URL path.
* FORM:	urlbase(url_s)
* EX:		http://www.abc.com/ => http://www.abc.com
*			http://www.x.com/pqr/ => http://www.x.com/pqr
*			http://www.x.com/pqr/a.txt => http://www.x.com/pqr
* NOTE:	May want to trim whitespace from start, end also.
********************************************/

bool Fn::fnUrlbase(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *url_s;

if (!Arg::str1(_T("urlbase"),(DELTS*&)args, url_s))
	return false;
if (!Arg::done((DELTS*)args, _T("urlbase"),parse))
	return false;

if (!url_s || !*url_s)
	{
	sem = new RFASem((long)0);
	return true;
	}

#ifdef _OLDEN_
_TCHAR *str = Chars::create(_tcsclen(url_s) + 1);
if (!url_base(url_s, /*UP*/ str))
	{
	Chars::destroy(str);
	sem = new RFASem((long)0);
	return true;
	}

_TCHAR *istr = 0;
parse->internStr(str, /*UP*/ istr);
Chars::destroy(str);
#endif

_TCHAR *istr = Arun::urlbase(nlppp,url_s);

sem = new RFASem(istr,RSSTR);	// FIX.	// 09/07/03 AM.
return true;
}


/********************************************
* FN:   FNDICTFIRST
* CR:   03/21/03 AM.
* SUBJ: Get first alphabetic word in kb dict.
* RET:  True if ok, else false.
*		       UP concept.
* FORM: dictfirst()
* NOTE: In conjunction with dictnext, for easy traverse of dictionary.
********************************************/

bool Fn::fnDictfirst(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

if (!Arg::done((DELTS*)args, _T("dictfirst"),parse))
	return false;

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *wordConcept = cg->dictFirst();

if (wordConcept)
	sem = new RFASem(wordConcept, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:   FNDICTNEXT
* CR:   03/21/03 AM.
* SUBJ: Get next alphabetic word in kb dict.
* RET:  True if ok, else false.
*		       UP concept.
* FORM: dictnext(dict_con)
* NOTE: For easy traverse of dictionary.
********************************************/

bool Fn::fnDictnext(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
RFASem *sem1;
Parse *parse = nlppp->parse_;

if (!Arg::sem1(_T("dictnext"),nlppp,(DELTS*&)args,sem1))
	return true;
if (!Arg::done((DELTS*)args, _T("dictnext"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[dictnext: Warning. Given no concept.]"));
	return parse->errOut(false); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[dictnext: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc1 = sem1->getKBconcept();

CONCEPT *wordConcept = cg->dictNext(conc1);

if (wordConcept)
	sem = new RFASem(wordConcept, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:   FNXMLSTR
* CR:   05/10/03 AM.
* SUBJ: Convert string to XML string.
* RET:  True if ok, else false.
*		       UP str - string with XML char formats.
* FORM: xmlstr(str)
********************************************/

bool Fn::fnXmlstr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str1=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("xmlstr"),(DELTS*&)args,str1))
	return true;
if (!Arg::done((DELTS*)args, _T("xmlstr"),parse))
	return false;

long len=0;
if (str1 && *str1)
  len = _tcsclen(str1);
_TCHAR *xstr = Xml::text2xml(str1, len, nlppp->htab());

sem = new RFASem(xstr,RSSTR);	// FIX.	// 09/07/03 AM.
return true;
}


/********************************************
* FN:   FNRESOLVEURL
* CR:   05/14/03 AM.
* SUBJ: Resolve embedded URL with base URL.
* RET:  True if ok, else false.
*		       UP str - Resolved URL string.
* FORM: resolveurl(base_url_str,embedded_url_str)
********************************************/

bool Fn::fnResolveurl(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *burl=0;
_TCHAR *eurl=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("resolveurl"),(DELTS*&)args,burl))
	return true;
if (!Arg::str1(_T("resolveurl"),(DELTS*&)args,eurl))
	return true;
if (!Arg::done((DELTS*)args, _T("resolveurl"),parse))
	return false;

_TCHAR buf[1024];
if (!resolve_url(burl,eurl,parse,/*DU*/buf))
	{
	_stprintf(Errbuf,_T("[resolveurl: Could not resolve url.]"));
	return parse->errOut(false); // UNFIXED
	}

_TCHAR *xstr;
parse->internStr(buf,xstr);

sem = new RFASem(xstr,RSSTR);	// FIX.	// 09/07/03 AM.
return true;
}


/********************************************
* FN:		FNINPUTRANGETOFILE
* CR:		05/29/03 AM.
* SUBJ:	Output a segment of the input buffer to a file.
* RET:	True if ok, else false.
* FORMS:	inputrangetofile(start_num,end_num,file_ostr)
*			start_num,end_num - range of offsets for input buffer.
* NOTE:	Optimization, doesn't create strings, etc.
********************************************/

bool Fn::fnInputrangetofile(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1 = 0;
long num1=0,num2=0;

if (!Arg::num1(_T("inputrangetofile"), (DELTS*&)args, num1))
	return false;
if (!Arg::num1(_T("inputrangetofile"), (DELTS*&)args, num2))
	return false;
if (!Arg::sem1(_T("inputrangetofile"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::done((DELTS*)args, _T("inputrangetofile"),parse))
	return false;

if (!sem1)
	return false;
if (sem1->getType() != RSOSTREAM)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

_t_ostream *ostr = sem1->getOstream();

if (!ostr)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Couldn't fetch file stream.]"));
	return parse->errOut(false); // UNFIXED
	}

if (num1 < 0 || num1 > num2)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Bad range.]"));
	return parse->errOut(false); // UNFIXED
	}
if (num2 >= parse->getLength())	// Larger than file length.
	{
	_stprintf(Errbuf,_T("[inputrangetofile: Offset exceeds input file length.]"));
	return parse->errOut(false); // UNFIXED
	}

// Get buffer range and output it.
_TCHAR *text = parse->getText();
if (!text)
	{
	_stprintf(Errbuf,_T("[inputrangetofile: No input text buffer.]"));
	return parse->errOut(false); // UNFIXED
	}

_TCHAR *ptr = &(text[num1]) - 1;	// Minus 1 for convenient looping.
for (; num1 <= num2; ++num1)
	*ostr << *++ptr;
*ostr << flush;

return true;
}


/********************************************
* FN:				FNDICTGETWORD
* CR:		06/29/03 AM.
* SUBJ:	Find or make a dictionary word.
* RET:	True if ok, else false.
*		       UP concept.
* FORM: dictgetword(str)
********************************************/

bool Fn::fnDictgetword(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem	    // Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
if (!Arg::str1(_T("dictgetword"), (DELTS*&)args, str1))
	return false;
if (!Arg::done((DELTS*)args, _T("dictgetword"),parse))
	return false;

if (!str1)
	{
	_stprintf(Errbuf,_T("[dictgetword: Warning. Given no word.]"));
	return parse->errOut(true); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

CONCEPT *wordConcept = cg->getWordConcept(str1);

if (wordConcept)
sem = new RFASem(wordConcept, RS_KBCONCEPT);

return true;
}


/********************************************
* FN:   FNSQLSTR
* CR:   07/22/03 AM.
* SUBJ: Convert string to SQL string.
* RET:  True if ok, else false.
*		       UP str - string with SQL char formats.
* FORM: sqlstr(str)
* NOTE:	Merely converts single-quote to two single-quotes.
********************************************/

bool Fn::fnSqlstr(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str1=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("sqlstr"),(DELTS*&)args,str1))
	return true;
if (!Arg::done((DELTS*)args, _T("sqlstr"),parse))
	return false;

long len=0;
if (str1 && *str1)
  len = _tcsclen(str1);
_TCHAR *xstr = Xml::text2sql(str1, len, nlppp->htab());

sem = new RFASem(xstr,RSSTR);		// FIX.	// 09/07/03 AM.
return true;
}



/********************************************
* FN:   FNDEACCENT
* CR:   09/07/03 AM.
* SUBJ: Convert accented chars in a string.
* RET:  True if ok, else false.
*		       UP str - string with accented chars replaced.
* FORM: deaccent(str)
********************************************/

bool Fn::fnDeaccent(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str1=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("deaccent"),(DELTS*&)args,str1))
	return true;
if (!Arg::done((DELTS*)args, _T("deaccent"),parse))
	return false;

long len=0;
if (str1 && *str1)
  len = _tcsclen(str1);
_TCHAR *xstr = Xml::de_accent(str1, len, nlppp->htab());

sem = new RFASem(xstr,RSSTR);
return true;
}



/********************************************
* FN:   FNDEJUNK
* CR:   09/09/11 AM.
* SUBJ: Convert non-ASCII chars in a string.
* RET:  True if ok, else false.
*		       UP str - string with accented chars replaced.
* FORM: dejunk(str)
********************************************/

bool Fn::fnDejunk(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
_TCHAR *str1=0;
Parse *parse = nlppp->parse_;

if (!Arg::str1(_T("dejunk"),(DELTS*&)args,str1))
	return true;
if (!Arg::done((DELTS*)args, _T("dejunk"),parse))
	return false;

long len=0;
if (str1 && *str1)
  len = _tcsclen(str1);
_TCHAR *xstr = Xml::de_junk(str1, len, nlppp->htab());

sem = new RFASem(xstr,RSSTR);
return true;
}

/********************************************
* FN:   FNLEXTAGGER
* CR:   12/10/03 AM.
* SUBJ: Embed the LexTagger interface in NLP++.
* FORM: lextagger()
********************************************/

bool Fn::fnLextagger(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
//char *str1=0;
Parse *parse = nlppp->parse_;

//if (!Arg::str1("lextagger",(DELTS*&)args,str1))
//	return true;
if (!Arg::done((DELTS*)args, _T("lextagger"),parse))
	return false;

#ifdef LINUX
{
_stprintf(Errbuf,_T("[Lextagger: LINUX version unimplemented.]"));
return parse->errOut(true);
}
#else
LexTagger::postagger(nlppp);
#endif

sem = new RFASem((long)1);
return true;
}


/********************************************
* FN:		FNSETLOOKAHEAD
* CR:		12/14/03 AM.
* SUBJ:	Dynamically set lookahead element.
* RET:	True if ok, else false.
* FORMS:	setlookahead(elt_num)
*			elt_num - Element num to set as lookahead locus.
* NOTE:	An ACTION, assumes a current rule match context.
********************************************/

bool Fn::fnSetlookahead(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

// If ok, update nlppp->nlookahead_, else leave it alone.	// 09/06/08 AM.

long num1=0;

if (!Arg::num1(_T("setlookahead"), (DELTS*&)args, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("setlookahead"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,
			_T("[setlookahead: Error. Must appear in @POST region.]"));
		return parse->errOut(true); // UNFIXED
	}

if (num1 <= 0)
	{
	_stprintf(Errbuf,_T("[setlookahead: Error. Given bad num.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!nlppp->first_ || !nlppp->last_)
	{
	_stprintf(Errbuf,_T("[setlookahead: Error. Null first or last node in range.]"));
	return parse->errOut(true); // UNFIXED
	}
if (nlppp->red_)
	{
	_stprintf(Errbuf,_T("[setlookahead: Error. Rule already reduced.]"));
	return parse->errOut(true); // UNFIXED
	}
if (nlppp->used_)
	{
	_stprintf(Errbuf,_T("[setlookahead: Error. Collect already used.]"));
	return parse->errOut(true); // UNFIXED
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	_stprintf(Errbuf,_T("[setlookahead: Warning. No collected nodes.]"));
	return parse->errOut(true); // UNFIXED
	}


////////////////////////////////////////////////
// Find the collect elt for lookahead.
////////////////////////////////////////////////
// These numbers represent a range of rule elts in the collect.
// Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.
Node<Pn> *colls;
Node<Pn> *node1;

colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
node1 = colls->Down();	// Try to get start of range.
if (node1)
	{
	nlppp->after_ = node1;
	nlppp->nlookahead_ = num1;					// 09/06/08 AM.
	return true;
	}

// Presumably lookahead elt is optional.  So look to the right.
while ((colls = colls->Right()))
	{
	if ((node1 = colls->Down()))	// Found nodes here.
		{
		nlppp->after_ = node1;		// Got lookahead node.
		nlppp->nlookahead_ = num1;				// 09/06/08 AM.
		return true;
		}
	}

return false;
}


/********************************************
* FN:		FNPERMUTEN
* CR:		03/10/04 AM.
* SUBJ:	Cleanly permute integers 0 to N-1.
* RET:	True if ok, else false.
*			UP - arr = array of ints 0 to N-1 in permuted order.
* FORMS:	permuten(num)
*			Number of ints to permute.
********************************************/

bool Fn::fnPermuten(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;
long num1 = 0;

if (!Arg::num1(_T("permuten"), (DELTS*&)args, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("permuten"),parse))
	return false;

sem = Arun::permuten(nlppp,num1);

return true;
}


/********************************************
* FN:		LOGTEN
* CR:		04/29/04 AM.
* SUBJ:	Base 10 logarithm.
* RET:	True if ok, else false.
*			UP - flt = log of given number, base 10.
* FORMS:	logten(flt)
********************************************/

bool Fn::fnLogten(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;
float num1 = 0;

if (!Arg::float1(_T("logten"), (DELTS*&)args, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("logten"),parse))
	return false;

float loggy = log10(num1);
sem = new RFASem(loggy);

return true;
}

#ifdef WORDNET

/********************************************
* FN:	   FNWNINIT
* CR:	   07/07/04 Dd.
* SUBJ: Initializes WordNet.
* RET:  True if ok, else false.
* FORMS:	wninit()
* NOTE:
********************************************/

bool Fn::fnWNInit(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
_TCHAR msgbuf[256];
msgbuf[0] = '\0';

Parse *parse = nlppp->parse_;

wninit(msgbuf);

if (msgbuf[0] != '\0')
	{
	_stprintf(Errbuf,msgbuf);
	return parse->errOut(true); // UNFIXED
	}

return true;
}

/********************************************
* FN:	   FNWNSENSESTOCONCEPT
* CR:	   07/07/04 Dd.
* SUBJ: Adds WN info to passed concept for given phrase or word
* RET:  True if ok, else false.
* FORMS:	wnsensestoconcept()
* NOTE:
********************************************/

bool Fn::fnWNSensesToConcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *searhStr = 0;
_TCHAR *posStr = 0;

if (!Arg::sem1(_T("wnsensestoconcept"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("wnsensestoconcept"), /*UP*/ (DELTS*&)args, searhStr))
	return false;
if (!Arg::str1(_T("wnsensestoconcept"), (DELTS*&)args, posStr))
	return false;
if (!Arg::done((DELTS*)args, _T("wnsensestoconcept"),parse))
	return false;

if (!sem1)	// 02/19/05 DD.
	{
	_stprintf(Errbuf,_T("[wnsensestoconcept: No concept given.]"));
	return parse->errOut(false); // UNFIXED
	}

CG *cg = nlppp->parse_->getAna()->getCG();
CONCEPT *conc1 = sem1->getKBconcept();

long count = WNSensesToConcept(cg,conc1,searhStr,posStr);
sem = new RFASem(count);

return true;
}

/********************************************
* FN:	   FNWNHPYNMSTOCONCEPT
* CR:	   07/07/04 Dd.
* SUBJ: Adds concepts for wordnet traces to a given concept.
* RET:  True if ok, else false.
* FORMS:	wnhypnumstoconcept()
* NOTE:
********************************************/

bool Fn::fnWNHypnymsToConcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
_TCHAR *searhStr = 0;
_TCHAR *posStr = 0;
_TCHAR *ptrStr = 0;
long depth = 0;

if (!Arg::sem1(_T("wnhypnumstoconcept"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::str1(_T("wnhypnumstoconcept"), /*UP*/ (DELTS*&)args, searhStr))
	return false;
if (!Arg::str1(_T("wnhypnumstoconcept"), (DELTS*&)args, posStr))
	return false;
if (!Arg::str1(_T("wnhypnumstoconcept"), (DELTS*&)args, ptrStr))
	return false;
if (!Arg::num1(_T("wnhypnumstoconcept"), (DELTS*&)args, depth))
	return false;
if (!Arg::done((DELTS*)args, _T("wntracetoconcept"),parse))
	return false;

if (!sem1)	// 02/19/05 DD.
	{
	_stprintf(Errbuf,_T("[wnhypnumstoconcept: No concept given.]"));
	return parse->errOut(false); // UNFIXED
	}

CG *cg = nlppp->parse_->getAna()->getCG();
CONCEPT *conc1 = sem1->getKBconcept();

long count = WNHypnymsToConcept(cg,conc1,searhStr,posStr,ptrStr,depth);
sem = new RFASem(count);

return true;
}


/********************************************
* FN:		FNADOPTCONCEPTS
* CR:		07/03/04  Dd.
* SUBJ:	Places concepts under an existing parent concept
* RET:	True if ok, else false.
* FORM:	adoptconcepts(str,concept,concept)
********************************************/

bool Fn::fnAdoptConcepts(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
RFASem *sem2;
RFASem *sem3;

if (!Arg::sem1(_T("adoptconcepts"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::sem1(_T("adoptconcepts"),nlppp,(DELTS*&)args,sem2))
	return false;
if (!Arg::sem1(_T("adoptconcepts"),nlppp,(DELTS*&)args,sem3))
	return false;
if (!Arg::done((DELTS*)args, _T("adoptconcepts"),parse))
	return false;

if (!sem1)
	{
	_stprintf(Errbuf,_T("[adoptconcepts: Warning. Given parent concept.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!sem2)
	{
	_stprintf(Errbuf,_T("[adoptconcepts: Warning. Given child start concept.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!sem3)
	{
	_stprintf(Errbuf,_T("[adoptconcepts: Warning. Given child start concept.]"));
	return parse->errOut(true); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[adoptconcepts: Bad semantic arg 1.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc1 = sem1->getKBconcept();

if (sem2->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[adoptconcepts: Bad semantic arg 2.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc2 = sem2->getKBconcept();

if (sem3->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[adoptconcepts: Bad semantic arg 3.])"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc3 = sem3->getKBconcept();

// Don't error out NLP++ if can't move concept right.
cg->adoptConcepts(conc1,conc2,conc3);

return true;
}


/********************************************
* FN:		FNMAKEPARENTCONCEPT
* CR:		07/03/04  Dd.
* SUBJ:	Places concepts under a new parent concept
* RET:	True if ok, else false.
* FORM:	adoptconcepts(str,concept,concept)
********************************************/

bool Fn::fnMakeParentConcept(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem		// Function return value.
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *sem1;
RFASem *sem2;
_TCHAR *newParentName;

if (!Arg::str1(_T("makeparentconcept"), /*UP*/ (DELTS*&)args, newParentName))
	return false;
if (!Arg::sem1(_T("makeparentconcept"),nlppp,(DELTS*&)args,sem1))
	return false;
if (!Arg::sem1(_T("makeparentconcept"),nlppp,(DELTS*&)args,sem2))
	return false;
if (!Arg::done((DELTS*)args, _T("makeparentconcept"),parse))
	return false;

if (newParentName[0] == '\0')
	{
	_stprintf(Errbuf,_T("[makeparentconcept: Warning. No name for new parent given.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!sem1)
	{
	_stprintf(Errbuf,_T("[makeparentconcept: Warning. Given parent concept.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!sem2)
	{
	_stprintf(Errbuf,_T("[makeparentconcept: Warning. Given child start concept.]"));
	return parse->errOut(true); // UNFIXED
	}

// Need to get the current KB.
CG *cg = parse->getAna()->getCG();

// Get concept from sem.
if (sem1->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[makeparentconcept: Bad semantic arg 1.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc1 = sem1->getKBconcept();

if (sem2->getType() != RS_KBCONCEPT)
	{
	_stprintf(Errbuf,_T("[makeparentconcept: Bad semantic arg 2.]"));
	return parse->errOut(false); // UNFIXED
	}
CONCEPT *conc2 = sem2->getKBconcept();

// Don't error out NLP++ if can't move concept right.
CONCEPT *returnConcept;
returnConcept = cg->makeParentConcept(newParentName,conc1,conc2);
if (!returnConcept)
	{
	_stprintf(Errbuf,_T("[false: Couldn't make parent concept.]"));
	parse->errOut(true); // UNFIXED
	}

sem = new RFASem(returnConcept,RS_KBCONCEPT);

return true;
}
#endif

/********************************************
* FN:		FNELTNODE
* CR:		09/12/06 AM.
* SUBJ:	Get first node matching a rule element.
* RET:	True if ok, else false.
* FORMS:	eltnode(elt_num)
********************************************/

bool Fn::fnEltnode(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

long num1=0;

if (!Arg::num1(_T("eltnode"), (DELTS*&)args, num1))
	return false;
if (!Arg::done((DELTS*)args, _T("eltnode"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGCHECK:
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,
		_T("[eltnode: Error. Must appear in @CHECK or @POST region.]"));
		return parse->errOut(true);
	}

if (num1 <= 0)
	{
	_stprintf(Errbuf,_T("[eltnode: Error. Given bad num.]"));
	return parse->errOut(true);
	}


Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	_stprintf(Errbuf,_T("[eltnode: Warning. No collected nodes.]"));
	return parse->errOut(true);
	}

Node<Pn> *colls;
Node<Pn> *node;

colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return true;
	}
node = colls->Down();	// Try to get start of range.
if (!node)
	return true;			// Optional missing elt is ok...

sem = new RFASem(node);	// Return the node.
return true;
}



/********************************************
* FN:	   FNLEVENSHTEIN
* CR:	   03/06/07 AM.
* SUBJ: Levenshtein edit-distance compare of two strings.
* RET:  True if ok, else false.
*	 UP - returns distance int.
* FORMS:	levenshtein(str1, str2)
********************************************/

bool Fn::fnLevenshtein(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *str1=0;
_TCHAR *str2=0;

if (!Arg::str1(_T("levenshtein"), /*UP*/ (DELTS*&)args, str1))
	return false;
if (!Arg::str1(_T("levenshtein"), /*UP*/ (DELTS*&)args, str2))
	return false;
if (!Arg::done((DELTS*)args, _T("levenshtein"),parse))
	return false;


if (!str1 || !*str1 || !str2 || !*str2)
	{
	_stprintf(Errbuf,_T("[levenshtein: Warning: Given empty string.]"));
	return parse->errOut(true); // UNFIXED
	}

int dist = levenshtein_int(str1,str2);

sem = new RFASem((long)dist);

return true;
}


/********************************************
* FN:		FNPNINSERT
* CR:		07/14/08 AM.
* SUBJ:	Programmatically insert node into parse tree.
* RET:	True if ok, else false.
* FORMS:	pninsert(name_str,pn_position,before-after_flag)
*	1 = after, 0 = before.
********************************************/

bool Fn::fnPninsert(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name_str=0;
RFASem *pos_sem;
long after_num=0;

if (!Arg::str1(_T("pninsert"), /*UP*/ (DELTS*&)args, name_str))
	return false;
if (!Arg::sem1(_T("pninsert"),nlppp,(DELTS*&)args,pos_sem))
	return false;
if (!Arg::num1(_T("pninsert"), (DELTS*&)args, after_num))
	return false;
if (!Arg::done((DELTS*)args, _T("pninsert"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGCODE:
		break;
	default:
		_stprintf(Errbuf,
		_T("[pninsert: Error. Must appear in @CODE region.]"));
		return parse->errOut(true);
	}

if (!name_str)
	{
	_stprintf(Errbuf,_T("[pninsert: Warning. Given no attr name.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!pos_sem)
	{
	_stprintf(Errbuf,_T("[pninsert: Warning. Given no parse tree node.]"));
	return parse->errOut(true); // UNFIXED
	}

if (after_num != 0 && after_num != 1)
	{
	_stprintf(Errbuf,_T("[pninsert: Error. Given bad num.]"));
	return parse->errOut(true);
	}

if (pos_sem->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pninsert: Bad semantic arg.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *pos_node = pos_sem->sem_to_node();

if (!pos_node)
	{
	_stprintf(Errbuf,_T("[pninsert: Given no parse tree node.]"));
	return parse->errOut(false); // UNFIXED
	}

if (*name_str != '_')
	{
	_stprintf(Errbuf,_T("[pninsert: Only nonliteral node name allowed.]"));
	return parse->errOut(false); // UNFIXED
	}

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();

// DUMMY UP A NODE.
long o_start = -1;
long o_end = -1;
_TCHAR *text = 0;
Sym *sym = parse->getSym(name_str);
_TCHAR *str = sym->getStr();
long len = _tcslen(str);
enum Pntype typ = PNNULL;

// Todo: These type checks are hooks for future enhancement of pninsert.
if (*name_str == '_')
	typ = PNNODE;
else if (alphabetic(*name_str))
	{
	typ = PNALPHA;
	}
else if (_istdigit(*name_str))
	{
	typ = PNNUM;
	}
else if (_istpunct(*name_str))
	{
	typ = PNPUNCT;
	}
else if (_istspace(*name_str))
	{
	typ = PNWHITE;
	}

Node<Pn> *new_node = Pn::makeNode(
	o_start,	// o_start
	o_end,	// o_end
	typ,		// Node type
	text,		// Pointer to text input buffer
	str,		// Suggested concept str
	sym,		// Suggested concept sym
	0,			// Assoc line number
	0,			// Assoc code pass
	0			// Assoc rule line
	);

if (after_num)
	tree->insertRight(*new_node, *pos_node);
else
	tree->insertLeft(*new_node, *pos_node);

sem = new RFASem(new_node);	// Return the node.
return true;
}


/********************************************
* FN:		FNPNMOVE
* CR:		10/06/13 AM.
* SUBJ:	Programmatically move node before/after a node.
* RET:	True if ok, else false.
* FORMS:	pnmove(pn,pn_position,before-after_flag)
*	1 = after, 0 = before.
********************************************/

bool Fn::fnPnmove(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

_TCHAR *name_str=0;
RFASem *pn_sem;
RFASem *pos_sem;
long after_num=0;

if (!Arg::sem1(_T("pnmove"),nlppp,(DELTS*&)args,pn_sem))
	return false;
if (!Arg::sem1(_T("pnmove"),nlppp,(DELTS*&)args,pos_sem))
	return false;
if (!Arg::num1(_T("pnmove"), (DELTS*&)args, after_num))
	return false;
if (!Arg::done((DELTS*)args, _T("pnmove"),parse))
	return false;

switch (nlppp->region_)
	{
	case REGCODE:
		break;
	default:
		_stprintf(Errbuf,
		_T("[pnmove: Error. Must appear in @CODE region.]"));
		return parse->errOut(true);
	}

if (!pn_sem)
	{
	_stprintf(Errbuf,_T("[pnmove: Warning. Given no FROM pnode.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!pos_sem)
	{
	_stprintf(Errbuf,_T("[pnmove: Warning. Given no TO pnode.]"));
	return parse->errOut(true); // UNFIXED
	}

if (after_num != 0 && after_num != 1)
	{
	_stprintf(Errbuf,_T("[pnmove: Error. Need num 0 (before) or 1 (after).]"));
	return parse->errOut(true);
	}

if (pn_sem->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnmove: First arg FROM must be a pnode.]"));
	return parse->errOut(false); // UNFIXED
	}

if (pos_sem->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[pnmove: Second arg TO must be a pnode.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *pn_node  = pn_sem->sem_to_node();
Node<Pn> *pos_node = pos_sem->sem_to_node();

if (!pn_node)
	{
	_stprintf(Errbuf,_T("[pnmove: Given no FROM pnode (2).]"));
	return parse->errOut(false); // UNFIXED
	}

if (!pos_node)
	{
	_stprintf(Errbuf,_T("[pnmove: Given no TO pnode (2).]"));
	return parse->errOut(false); // UNFIXED
	}

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();

// DETACH NODE HERE.
tree->exciseSubs(pn_node, pn_node);

// ATTACH NODE HERE.
if (after_num)
	tree->insertRight(*pn_node, *pos_node);
else
	tree->insertLeft(*pn_node, *pos_node);

sem = new RFASem(pn_node);	// Return the node.
return true;
}


/********************************************
* FN:		PROSIFY
* CR:		09/29/10 AM.
* SUBJ:	Traverse subtree to compose its text, using TEXT and NOSP fields.
* RET:	ok - True if success, else false.
*			UP buf - The composed text buffer.
* NOTE:	RECURSIVE.
*			Modeled on treeText.
*			ASSUMES NOT MUCH WHITESPACE IN PARSE TREE.
*			If node has a "text" field, use it and don't recurse.
*			If node has a "NOSP" field, then it gets no whitespace.
*			Else default rules for whitespace.
********************************************/

bool Fn::prosify(
	Node<Pn> *node,
	bool root,				// If this is the top-level node of subtree.
	_TCHAR *buf,				// Buffer to fill.
	_TCHAR* &ptr,				// First empty spot in buffer.
	long &siz				// Empty space left in buffer
	)
{
if (node == 0)
	return true;

Nlppp *nlppp = 0;
_TCHAR *field = _T("text");
_TCHAR *nosp = _T("NOSP");

bool ok = true;
Pn *pn = node->getData();

// If there's a text field in the node, use it.
// pnvar(node,field)
RFASem * sem = 0;
Ivar::nodeVar(node,field,nlppp,/*UP*/sem);	// Creates a sem.


// Getting text directly from literal nodes.
_TCHAR *name = pn->getName();

if (*name != '_')		// Literal node.
	{
	long len = _tcsclen(name);
	if (len > (siz-1))	// buf+terminator.
		return false;
	strcpy_ee(ptr,name);
	siz -= len;
	}
else if (node->pDown)
	ok = prosify(node->pDown,false,buf,ptr,siz);		// RECURSIVE.
else						// I am a leaf. Do my text here.
	{
	// Copy node text to buffer.
	_TCHAR *text = pn->getText();
	long start = pn->getStart();
	long end = pn->getEnd();
	long len = end-start+1;
	if (len > (siz-1))	// buf+terminator.
		return false;
	int ii;
	for (ii = start; ii <= end; ++ii)
		*ptr++ = *text++;
	siz -= len;
	}

if (root)				// Root of the whole subtree.
	{
	// Return text buffer.
	*ptr = '\0';		// Terminate buffer.
	return ok;
	}

// Minimal recursion in this part (optimization and resource saving!).
if (!(node->pLeft))		// First node in list takes care of list.
	{
	for (node = node->pRight; node; node = node->pRight)
		{
		ok = prosify(node,false,buf,ptr,siz);
		if (!ok)
		  return ok;
		}
	}
return ok;
}


/********************************************
* FN:		FNDICTPHRASELOOKUP
* CR:		12/15/14 AM.
* SUBJ:	Internal phrase lookup function, keys off kb dictionary pointer.
* RET:	ok - True if ok, else false.
* EFF:	Modifies parse tree nodes that yield matches in the kb phrase hierarchy.
*			Saves match info mainly in the given node.
* NOTE:	NLP++ phrase lookup support is overdue.
*			Somewhat hardwired variant, a start on such capabilities.
* EX:		DICTphraselookup(L("n"),L("k"),L("f_punct"));
*				L("n") = current parse tree node, to match the start of phrase.
*				L("k") = name of concept attribute in kb dict.  From LOWERCASE text of current node.
*				L("k_match") = name of match attribute in phrase hierarchy.
*				L("k_list")  = name to use for the gathered match list.
*				L("f_punct") = flag 1/0 skip punctuation.
*
* METH:	L("t") = strtolower(nodetreetext(L("n"));
*        L("w") = dictfindword(L("t"));
*			L("c") = conval(L("w"),L("k"));	# Gets to phrases starting with current node text, if any.
*			If skip punctuation, then it should not be in the phrase hierarchy.
*			WHITESPACE ALWAYS SKIPPED IN PARSE TREE.
* ASS:	Assumes there is a hierarchy of phrases in the KB, referenced by the L("k") concept attribute name
*			on kb dictionary words.
********************************************/

bool Fn::fnDICTphraselookup(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;

RFASem *pn_sem;
_TCHAR *key_str=0;
_TCHAR *match_str=0;
_TCHAR *list_str=0;
long punct_num=0;

if (!Arg::sem1(_T("DICTphraselookup"),nlppp,(DELTS*&)args,pn_sem))
	return false;
if (!Arg::str1(_T("DICTphraselookup"), /*UP*/ (DELTS*&)args, key_str))
	return false;
if (!Arg::str1(_T("DICTphraselookup"), /*UP*/ (DELTS*&)args, match_str))
	return false;
if (!Arg::str1(_T("DICTphraselookup"), /*UP*/ (DELTS*&)args, list_str))
	return false;
if (!Arg::num1(_T("DICTphraselookup"), (DELTS*&)args, punct_num))
	return false;
if (!Arg::done((DELTS*)args, _T("DICTphraselookup"),parse))
	return false;

// Who cares? Parse tree structure not modified.
/*
switch (nlppp->region_)
	{
	case REGCHECK:
	case REGPOST:
		break;
	default:
		_stprintf(Errbuf,
		_T("[DICTphraselookup: Error. Must appear in @CHECK or @POST region.]"));
		return parse->errOut(true); // UNFIXED
	}
*/

if (!pn_sem)
	{
	_stprintf(Errbuf,_T("[DICTphraselookup: Warning. Given no FROM pnode.]"));
	return parse->errOut(true); // UNFIXED
	}

if (!key_str)
	{
	_stprintf(Errbuf,_T("[DICTphraselookup: Warning. Given no str.]"));
	return parse->errOut(true); // UNFIXED
	}

if (punct_num != 0 && punct_num != 1)
	{
	_stprintf(Errbuf,_T("[DICTphraselookup: Error. Punct-skip flag must be 0/1.]"));
	return parse->errOut(true);
	}

if (pn_sem->getType() != RSNODE)
	{
	_stprintf(Errbuf,_T("[DICTphraselookup: First arg FROM must be a pnode.]"));
	return parse->errOut(false); // UNFIXED
	}

Node<Pn> *pn_node  = pn_sem->sem_to_node();

if (!pn_node)
	{
	_stprintf(Errbuf,_T("[DICTphraselookup: Given no pnode.]"));
	return parse->errOut(false); // UNFIXED
	}

// Get pnode text
_TCHAR buf[512];
long siz=512;
buf[0] = '\0';
_TCHAR *ptr = buf;
Fn::prosify(pn_node,true,buf,ptr,siz);

// strtolower // Trying IN-PLACE conversion.
str_to_lower(buf);

// OMITTING INTERN (if possible)
_TCHAR *str;
//parse->internStr(buf, /*UP*/ str);
str = buf;


// dictfindword
CG *cg = parse->getAna()->getCG();	// Get current KB.

CONCEPT *wc = cg->findWordConcept(str);
if (!wc) return true;	// No matches; done.

// conval
// key_str
CONCEPT *cc = 0;
if (!cg->findVal(wc,key_str,/*UP*/ cc))
	return true;	// No phrase start concept; done.

// Now we are at first word's concept in phrase hierarchy.
long len = 1;	// Length of matched phrase.


Htab *ht = parse->getHtab();	// Current hash table for strings.

// TODO: Record info if first word is a full match.
Fn::DICTphraselookup_match(pn_node,pn_node,cc,match_str,list_str,cg,ht);

// Move to next pnode.
Node<Pn> *x  = pn_node->Right();

// CRASH. // 01/14/15 AM.
if (!x) return true;	// CRASH.	// 01/14/15 AM.

// Tandem traverse current parse tree nodes and down the kb hierarchy path.
// Record nodes that match.
CONCEPT *cx = 0;
bool done = false;
while (!done)
	{
	// Try to match.
	ptr = buf;
	Fn::prosify(x,true,buf,ptr,siz);
	str_to_upper(buf);	// Phrase hierarchy assumed all uppercase.
	if (_istspace(*buf))	// Whitepace.
		;	// Move to next.
	else if (_istpunct(*buf) && punct_num)
		; // Skip punctuation too.
	else if ((cx = cg->findConcept(cc, buf)))	// MATCHED CHILD
		{
		cc = cx;
		// TODO: RECORD MATCH.
		// If end of concept path, done.
		Fn::DICTphraselookup_match(pn_node,x,cc,match_str,list_str,cg,ht);

		if (!cg->Down(cc)) done = true;
		}
	else
		done = true;

	if (!(x = x->Right()) ) done = true;
	}


return true;
}


/********************************************
* FN:		DICTPHRASELOOKUP_MATCH
* CR:		12/16/14 AM.
* SUBJ:	Record matched node in DICTphraselookup.
* RET:	ok - True if success, else false.
* NOTE:
********************************************/

bool Fn::DICTphraselookup_match(
	Node<Pn> *nn,	// Overall start node for current walk.
	Node<Pn> *xx,	// Matched parse tree node.
	CONCEPT *cc,		// Matched kb phrase concept.
	_TCHAR *match_str, // Concept attribute that denotes a "MATCH".
	_TCHAR *list_str,	// Node attribute for gathering match values.
	CG *cg,
	Htab *ht	// Current hash table
	)
{
if (!nn || !xx || !cc) return false;

bool ok = true;	// Everything ok so far.

// Record partial match.
_TCHAR *key = _T("PARTMATCH");
RFASem *csem = new RFASem(cc, RS_KBCONCEPT);
Ivar::nodePushval(xx->getData(),key,csem,false,false);

// TODO: See if node has been matched to current concept,
// eg in a previous phrase lookup.

// If a full match, get string value for it. "MATCH"
_TCHAR *str = KB::strVal(cc,match_str,cg,ht);						// 02/12/07 AM.

if (str && *str)	// FULL MATCH.
	{
	// Save last node match on FIRST NODE.
	RFASem *semlast = new RFASem(xx);
	Ivar::nodeReplaceval(nn->getData(),_T("SAVE NODE"), semlast,false,false);

	// Save last concept match on FIRST NODE.
	RFASem *semlastc = new RFASem(cc,RS_KBCONCEPT);
	Ivar::nodeReplaceval(nn->getData(),_T("SAVE CON"), semlastc,false,false);

	// Matched concepts in phrase hierarchy, on current node.
	RFASem *sem1 = new RFASem(cc,RS_KBCONCEPT);
	Ivar::nodeReplaceval(xx->getData(),_T("MATCHCONS"), sem1,false,false);

	// Array of matched values on FIRST NODE. "MATCHES"
	Ivar::nodeReplaceval(nn->getData(),list_str, str,false,false);

	// TODO: Increment count of matches.
	}

return ok;
}
