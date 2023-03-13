/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	POST.CPP
* FILE:	lite\post.cpp
* CR:		11/04/98 AM.
* SUBJ:	Post actions for Pat pass.
* NOTE:	pat.cpp getting too big, so splitting it up.
* CONV:	Keeping the prefix "red" for post actions that reduce the parse tree.
*			Should only allow one of these per set of post actions.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.

// DEFINE NLP++ BUILTIN FUNCTION ARRAY HERE.							// 12/20/01 AM.
#include "funcs.h"															// 12/20/01 AM.

#include "consh/libconsh.h"		// 02/14/01 AM.
#include "consh/cg.h"				// 02/14/01 AM.
#include "lite/global.h"
#include "inline.h"		// 05/19/99 AM.
#include "dlist.h"					// 07/07/03 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "lite/Auser.h"		// 07/07/03 AM.
#include "lite/iarg.h"		// 05/14/03 AM.
#include "str.h"
#include "io.h"
#include "chars.h"			// 02/14/00 AM.
#include "string.h"
#include "lite/nlppp.h"			// 11/14/99 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "parse.h"
//#include "lite/nlppp.h"			// 11/14/99 AM.
#include "pat.h"
#include "pn.h"
#include "postrfa.h"			// Should be a reference to a Pat user's area.
#include "arg.h"				// 12/08/98 AM.
#include "var.h"				// 12/08/98 AM.
#ifndef LINUX
//#include "web/web.h"			// 09/26/01 AM.
//#include "web/url.h"			// 09/26/01 AM.
#endif
#include "dsem.h"				// 08/05/99 AM.
#include "rfasem.h"			// 11/14/99 AM.
#include "ana.h"				// 02/15/00 AM.
#include "lite/Arun.h"		// 06/01/00 AM.
#include "Eana.h"				// 05/20/01 AM.
#include "post.h"


/********************************************
* FN:		NODETYPE
* CR:		05/01/03 AM.
* SUBJ:	Get node type based on string.
* RET:	Node type.
********************************************/

inline enum Pntype nodetype(_TCHAR *str)
{
return alphabetic(*str) ? PNALPHA : PNNODE;
}


/********************************************
* FN:		POSTACTIONS
* CR:		10/28/98 AM.
* SUBJ:	Execute post actions for matched rule.
* RET:	True if actions executed ok, else false.	// 11/10/99 AM.
* NOTE:	Renamed from execAction. Moved from pat.cpp.
*			Need to handle "hard-wired" (optimized) actions like ATTRS
*			field in an elt.
*			11/10/99 AM. Return status to avoid infinite loop in REC algo.
********************************************/

bool Pat::postActions(
	Nlppp *nlppp			// 11/19/99 AM.
	)
{
// USE LINE NUMBER OF RULE TO REPORT ERRORS, FOR NOW.				// 05/26/01 AM.
nlppp->parse_->line_ = nlppp->rule_->getLine();						// 08/24/02 AM.

// Variables for tracking the state of post actions.
bool ok = true;		// 11/10/99 AM.
nlppp->used_ = false;	// If collect was consumed.				// 01/13/00 AM.
nlppp->red_  = 0;			// Suggested node not built yet.			// 01/13/00 AM.
nlppp->region_ = REGPOST;	// Execute NLP++ in post region.		// 01/13/00 AM.
nlppp->nlookahead_ = 0;		// Reset.									// 09/06/08 AM.
nlppp->succeed_ = false;	// Reset.									// 09/22/08 AM.
nlppp->fail_ = false;		// Reset.									// 09/22/08 AM.

//Node<Pn> *after;			// Save the location for continuing to match rules.
nlppp->after_ = 0;

// Handle L() local vars in @CODE region!							// 11/24/02 AM.
void *dummy=0;																// 01/08/07 AM.
Arun::fnstart(nlppp,dummy,dummy);	// 11/24/02 AM.			// 01/08/07 AM.

// Get node for lookahead.
// If the rule has no lookahead elt, move right one node.
// Else, find the first lookahead node, if any, and record it as next
// place for the pattern matcher to go.
// Fixing this code up.														// 12/18/99 AM.
if (nlppp->rule_->getLookahead())										// 12/18/99 AM.
	initLookahead(nlppp);													// 12/18/99 AM.
else
	nlppp->lookahead_ = 0;													// 12/18/99 AM.
if (!nlppp->lookahead_)														// 12/18/99 AM.
	nlppp->after_ =															// 12/14/03 AM.
				nodeRight(nlppp->last_, nlppp->end_);					// 12/18/99 AM.
else
	nlppp->after_ =															// 12/14/03 AM.
			nlppp->lookahead_;												// 12/18/99 AM.

//Tree<Pn> *tree = nlppp->parse_->getTree();							// 01/13/00 AM.
Dlist<Iaction> *posts;		// Post-actions.
posts = nlppp->rule_->getPost();

if (!posts)			// No post actions.  Execute pass default.
	{
	// If none, execute default reduction action.
	// Default action will be to glom the matched nodes
	//	into a subtree (single-tier reduction).
	// New node for subtree will have the suggested node's name.
	// Need info about what nodes to splice out of tree.
	// May want the default reduce to be user settable.

	// Not allowing lookahead with single() reduce.				// 12/18/03 AM.
	if (nlppp->rule_->getLookahead())								// 12/18/03 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Single: Lookahead modifier not allowed.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		nlppp->exitpass_ = true;
		return false;
		}

	if (!execPhrase(nlppp))
		return false;			// 11/10/99 AM.
	ok = redSingle(0,nlppp);												// 05/26/01 AM.
	if (!postFinal(nlppp))
		// , /*DU*/ after))													// 12/14/03 AM.
		return false;			// 11/10/99 AM.
	return ok;				// 11/10/99 AM.
	// Caller will clear nodes of collect.
	}

///////////////////////
// EXECUTE POST ACTIONS.  (Actions following a successful rule match.)
///////////////////////

// Mark all matched nodes as FIRED.										// 05/23/01 AM.
// Do this only if saving log files.									// 05/23/01 AM.
// (They may be excised later.  Don't care.)							// 05/23/01 AM.
Parse *parse = nlppp->parse_;				// Moved up.				// 05/23/01 AM.
Eana *eana = parse->getEana();			// Moved up.				// 05/23/01 AM.
if (eana->getFlogfiles())					// If saving logs.		// 05/23/01 AM.
	{
	// Traverse all nodes matched by the current rule.				// 05/23/01 AM.
	Node<Pn> *end = nlppp->last_;											// 05/23/01 AM.
	if (end)																		// 05/23/01 AM.
		end = end->Right();		// Sentinel to end loop.			// 05/23/01 AM.
	Node<Pn> *node = nlppp->first_;										// 05/23/01 AM.
	for (node = nlppp->first_; node != end; node = node->Right())// 05/23/01 AM.
		{
		Pn *pn = node->getData();											// 05/23/01 AM.
		pn->setFired(true);													// 05/23/01 AM.
		}
	}

// ELEMENT ACTIONS first.  (That's what they're called now.)	// 05/23/01 AM.
// 11/10/98 AM.
// Start by walking the rule phrase elements and doing their actions.
if (!execPhrase(nlppp))
	return false;				// 11/10/99 AM.

Delt<Iaction> *dpost;
Iaction *post;
RFASem *rfasem, *val = 0;
bool badname = false;			// If unknown action name.			// 01/13/00 AM.
for (dpost = posts->getFirst(); dpost; dpost = dpost->Right())
	{
	// Perform the current post action.
	post	= dpost->getData();

	if ((rfasem = post->getSem()))	// 11/99 AM.
		{
		//*gerr << "[Statements to execute.]" << std::endl;
		//Nlppp nlppp(rule,collect,parse,nlppp->select_,			// 11/19/99 AM.
		//									first,last,node,start,end,sem,dsem);
		if (!(ok = rfasem->eval(nlppp, /*UP*/ val)))					// 11/99 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Post actions: Error in NLP++.]") << std::ends;
			errOut(&gerrStr,false, parse->currpass_,parse->line_);			// 03/05/03 AM.
//			errOut(false);
			ok = false;												// FIX.	// 12/18/03 AM.
			nlppp->exitpass_ = true;							// FIX.	// 12/18/03 AM.
			break;													// FIX.	// 12/18/03 AM.
			}

		if (val)
			delete val;			// Not using value of semantics at top level.
		// NEED TO GET MODIFIED VALUES FROM NLPPP OBJECT.			// 11/17/99 AM.
		//sem  = nlppp.getSem();	// Just in case.					// 11/20/99 AM.
		//dsem = nlppp.getDsem();											// 11/20/99 AM.
		//break;							// FIX								// 11/17/99 AM.
		//continue;																// 06/10/99 AM.
		}
	else if (!postAction(post,nlppp, /*UP*/ badname))
		{
		if (eana->getFintern())	// If interning analyzer.			// 05/20/01 AM.
			{
			Node<Pn> *xx = nlppp->first_;									// 05/20/01 AM.
			Pn *xxpn;															// 05/20/01 AM.
			if (xx)																// 05/20/01 AM.
				{
				xxpn = xx->getData();										// 05/20/01 AM.
				parse->inputline_ = xxpn->getLine();					// 08/24/02 AM.
				}
			else																	// 05/20/01 AM.
				parse->inputline_ = 0;										// 08/24/02 AM.
			if (badname)														// 05/20/01 AM.
				{
				_stprintf(Errbuf,
	_T("[Internal error: Unknown post action=%s,pass=%ld,line=%ld]"),// 05/20/01 AM.
						post->getName(),
						parse->currpass_,										// 08/22/02 AM.
						post->getLine());										// 05/20/01 AM.
				errOut(false, parse->inputpass_,parse->inputline_); // UNFIXED // 08/24/02 AM.
				}
			else if (parse->getIntern())									// 05/26/01 AM.
				{
				_stprintf(Errbuf,												// 05/20/01 AM.
					_T("[Error: Syntax error.]"));								// 05/20/01 AM.
				errOut(false, parse->inputpass_,parse->inputline_); // UNFIXED // 08/24/02 AM.
				}
			}
		else	// Normal text analysis.									// 05/20/01 AM.
			{
			parse->line_ = post->getLine();								// 08/24/02 AM.
			if (badname)
				_stprintf(Errbuf,_T("[Error: Unknown post action=%s]"),	// 05/20/01 AM.
						post->getName());										// 05/20/01 AM.
			else																	// 05/20/01 AM.
				_stprintf(Errbuf, _T("[Error: At post action=%s]"),		// 05/20/01 AM.
						post->getName());										// 05/20/01 AM.
			errOut(false, parse->currpass_,parse->line_); // UNFIXED 			// 08/24/02 AM.
			}

		ok = false;																// 11/10/99 AM.
		break;
		}
	if (nlppp->fail_)															// 06/10/00 AM.
		{
		Arun::ret(nlppp,0,0,(RFASem*)0);	// Handle local vars!	// 11/24/02 AM.
		postFinal(nlppp);											// FIX	// 09/22/08 AM.
		return false;															// 06/10/00 AM.
		}
	if (nlppp->exitpass_)													// 11/11/02 AM.
		{
		Arun::ret(nlppp,0,0,(RFASem*)0);	// Handle local vars!	// 11/24/02 AM.
		postFinal(nlppp);														// 12/14/03 AM.
			//				, /*DU*/ after);	// Clean up rule match.	// 11/11/02 AM.
		return false;															// 11/11/02 AM.
		}
	}

Arun::ret(nlppp,0,0,(RFASem*)0);	// Handle local vars!			// 11/24/02 AM.

if (!postFinal(nlppp))														// 12/14/03 AM.
	// , /*DU*/ after))														// 12/14/03 AM.
	return false;
return ok;						// 11/10/99 AM.
}


/********************************************
* FN:		POSTACTION
* CR:		11/05/98 AM.
* SUBJ:	Execute one post action for matched rule.
* RET:	True if action executed ok, false if error.
********************************************/

bool Pat::postAction(
	Iaction *post,				// 01/13/00 AM.
	Nlppp *nlppp,				// 11/19/99 AM.
	/*UP*/
	bool &badname				// 01/13/00 AM.
	)
{
//if (Debug())
//	*gout << "  [Execute post action...]" << std::endl;

nlppp->parse_->line_ = post->getLine();								// 08/24/02 AM.

// Moved the unpacking here.												// 01/13/00 AM.
_TCHAR *func				= post->getName();								// 01/13/00 AM.
Dlist<Iarg> *dargs	= post->getArgs();								// 01/13/00 AM.
Delt<Iarg> *args;	// UNINITIALIZED!										// 01/13/00 AM.
if (dargs)
   args		= dargs->getFirst();											// 01/13/00 AM.
else
	args = 0;

if (!strcmp_i(func, _T("single")))		// Single-tier (simple) reduction.
	return redSingle(args,nlppp);											// 05/26/01 AM.
else if (!strcmp_i(func, _T("singler")))		// Single-tier, range.
	return redSingler(args,nlppp);										// 01/17/99 AM.
else if (!strcmp_i(func, _T("singlex")))		// Single-tier,range,excise.
	return redSinglex(args,nlppp);										// 01/17/99 AM.
if (!strcmp_i(func, _T("singlezap")))
	{
	if (args)																	// 06/11/03 AM.
		Arg::done(args, _T("singlezap"),nlppp->parse_);					// 06/11/03 AM.
	return redSinglezap(nlppp);											// 11/21/00 AM.
	}
else if (!strcmp_i(func, _T("merge")))										// 12/02/99 AM.
	{
	if (args)																	// 06/11/03 AM.
		Arg::done(args, _T("merge"),nlppp->parse_);						// 06/11/03 AM.
	return redMerge(nlppp);
	}
else if (!strcmp_i(func, _T("merger")))										// 12/02/99 AM.
	return redMerger(args,nlppp);
else if (!strcmp_i(func, _T("listadd")))									// 11/16/99 AM.
	return postListadd(args,nlppp);

// else if (!strcmp_i(func, "double"))
// {
// redDouble();
// Use nodes of collect (in double-tier reduce).
// Caller will attempt to delete, so clear them out as below.
// collect->setRoot(0);
// collect->setLast(0);
// }

// RENAMING THE OLD-STYLE GROUP ACTION.								// 11/24/00 AM.
else if (!strcmp_i(func, _T("oldgroup")))	// Group rule elts.		// 11/24/00 AM.
	return postGroup(args,nlppp);											// 11/24/00 AM.
else if (!strcmp_i(func, _T("excise")))		// Remove nodes from tree.
	{
	// Form:  excise(elt1, elt2)
	// where elt1,elt2 are the start and end rule elt ords (ie, numbers) to
	// use in excising nodes that matched the current rule.
	return postExcise(args,nlppp);
	}
else if (!strcmp_i(func, _T("oldsplice")									// 07/19/02 AM.
											))										// 07/30/99 AM.
	{
	// Form: splice(elt1, elt2)
	// Remove the top level nodes in the range and "splice in" their
	// children!
	return postSplice(args,nlppp);
	}
else if (!strcmp_i(func, _T("setbase")))										// 12/12/99 AM.
	return postSetbase(args,nlppp);
else if (!strcmp_i(func, _T("setunsealed")))								// 12/12/99 AM.
	return postSetunsealed(args,nlppp);
else if (!strcmp_i(func, _T("inc"))		// Increment var.				// 12/08/98 AM.
		   || !strcmp_i(func, _T("ginc")))										// 10/02/99 AM.
	return postInc(args,nlppp);
else if (!strcmp_i(func, _T("addstrs")))		// Increment var.			// 01/24/99 AM.
	return postAddstrs(args,nlppp);

else if (!strcmp_i(func, _T("xaddlen")))		// Add length to context node's var.
	return postXaddlen(args,nlppp);
else if (!strcmp_i(func, _T("xinc")))			// Increment context node's var.
	return postXinc(args,nlppp);
else if (!strcmp_i(func, _T("xaddnvar")))									// 09/23/99 AM.
	return postXaddNvar(args,nlppp);
else if (!strcmp_i(func, _T("ninc")))											// 10/02/99 AM.
	return postNinc(args,nlppp);
else if (!strcmp_i(func, _T("xrename")))										// 12/25/99 AM.
	return postXrename(args,nlppp);

else if (!strcmp_i(func, _T("XXsetifzero")))								// 10/31/99 AM.
	return postXXsetifzero(args,nlppp);
else if (!strcmp_i(func, _T("Xset")))											// 10/31/99 AM.
	return postXset(args,nlppp);
else if (!strcmp_i(func, _T("XsetX")))										// 11/03/99 AM.
	return postXsetX(args,nlppp);

else if (!strcmp_i(func, _T("print")))		// Print literal string	// 01/30/99 AM.
	return postPrint(args,nlppp);
else if (!strcmp_i(func, _T("printr")))		// Print range				// 01/30/99 AM.
	return postPrintr(args,nlppp);
else if (!strcmp_i(func, _T("prlit")))		// Print literal to file// 02/09/99 AM.
	return postPrlit(args,nlppp);
else if (!strcmp_i(func, _T("prrange")))		// Print range to file	// 02/09/99 AM.
	return postPrrange(args,nlppp);
else if (!strcmp_i(func, _T("prchild")))		// Print child to file	// 03/17/99 AM.
	return postPrchild(args,nlppp);
else if (!strcmp_i(func, _T("prtree")))		// Print subtree to file// 03/22/99 AM.
	return postPrtree(args,nlppp);
else if (!strcmp_i(func, _T("prxtree")))										// 09/25/99 AM.
	return postPrxtree(args,nlppp);
else if (!strcmp_i(func, _T("pranchor")))	// Html anchor to file	// 03/05/99 AM.
	return postPranchor(args,nlppp);

else if (!strcmp_i(func, _T("fprintnvar")))									// 08/05/99 AM.
	return postFprintnvar(args,nlppp);
else if (!strcmp_i(func, _T("fprintxvar")))									// 11/26/99 AM.
	return postFprintxvar(args,nlppp);
else if (!strcmp_i(func, _T("fprintgvar")))									// 12/03/99 AM.
	return postFprintgvar(args,nlppp);

else if (!strcmp_i(func, _T("Ndump")))										// 12/13/99 AM.
	return postNdump(args,nlppp);
else if (!strcmp_i(func, _T("Xdump")))										// 12/13/99 AM.
	return postXdump(args,nlppp);
else if (!strcmp_i(func, _T("Gdump")))										// 12/13/99 AM.
	return postGdump(args,nlppp);
else if (!strcmp_i(func, _T("Sdump")))										// 12/13/99 AM.
	return postSdump(args,nlppp);

// NLP++ interface to KB.													// 02/15/00 AM.
//else if (!strcmp_i(func, "addattr"))									// 02/15/00 AM.
//	return postAddattr(args,nlppp);										// 02/15/00 AM.

else if (!strcmp_i(func, _T("makestmt")))									// 05/11/00 AM.
	return PostRFA::postRFAmakestmt(args,nlppp);
else if (!strcmp_i(func, _T("returnstmt")))								// 03/07/02 AM.
	return PostRFA::postRFAreturnstmt(args,nlppp);					// 03/07/02 AM.
else if (!strcmp_i(func, _T("makestmts")))									// 05/29/01 AM.
	return PostRFA::postRFAmakestmts(args,nlppp);
else if (!strcmp_i(func, _T("addstmt")))										// 11/12/99 AM.
	return PostRFA::postRFAaddstmt(args,nlppp);
else if (!strcmp_i(func, _T("ifstmt")))										// 11/12/99 AM.
	return PostRFA::postRFAifstmt(args,nlppp);
else if (!strcmp_i(func, _T("ifelse")))										// 11/12/99 AM.
	return PostRFA::postRFAifelse(args,nlppp);
else if (!strcmp_i(func, _T("whilestmt")))									// 08/31/00 AM.
	return PostRFA::postRFAwhilestmt(args,nlppp);
else if (!strcmp_i(func, _T("rfaexpr")))										// 11/10/99 AM.
	return PostRFA::postRFAexpr(args,nlppp);
else if (!strcmp_i(func, _T("rfaunary")))									// 11/11/99 AM.
	return PostRFA::postRFAunary(args,nlppp);
else if (!strcmp_i(func, _T("rfapostunary")))								// 11/11/99 AM.
	return PostRFA::postRFApostunary(args,nlppp);
else if (!strcmp_i(func, _T("rfavar")))										// 11/09/99 AM.
	return PostRFA::postRFAvar(args,nlppp);
else if (!strcmp_i(func, _T("rfaop")))										// 11/09/99 AM.
	return PostRFA::postRFAop(args,nlppp);
else if (!strcmp_i(func, _T("movesem")))										// 11/09/99 AM.
	return PostRFA::postRFAmovesem(args,nlppp);

else if (!strcmp_i(func, _T("rfaname")))
	return PostRFA::postRFAname(args,nlppp);
else if (!strcmp_i(func, _T("rfanum")))
	return PostRFA::postRFAnum(args,nlppp);
if (!strcmp_i(func, _T("rfafloat")))
	return PostRFA::postRFAfloat(args,nlppp);							// 08/19/01 AM.
else if (!strcmp_i(func, _T("rfanonlit")))
	return PostRFA::postRFAnonlit(args,nlppp);
else if (!strcmp_i(func, _T("rfastr")))
	return PostRFA::postRFAstr(args,nlppp);
else if (!strcmp_i(func, _T("rfaarg")))
	return PostRFA::postRFAarg(args,nlppp);
else if (!strcmp_i(func, _T("rfarange")))
	return PostRFA::postRFArange(args,nlppp);
else if (!strcmp_i(func, _T("rfaargtolist")))
	return PostRFA::postRFAargtolist(args,nlppp);
else if (!strcmp_i(func, _T("rfalist")))
	return PostRFA::postRFAlist(args,nlppp);
else if (!strcmp_i(func, _T("rfaaction")))
	return PostRFA::postRFAaction(args,nlppp);
else if (!strcmp_i(func, _T("rfalittopair")))
	return PostRFA::postRFAlittopair(args,nlppp);
else if (!strcmp_i(func, _T("rfapair")))
	return PostRFA::postRFApair(     args,nlppp);
else if (!strcmp_i(func, _T("rfapairs")))
	return PostRFA::postRFApairs(    args,nlppp);
else if (!strcmp_i(func, _T("rfaelement")))
	return PostRFA::postRFAelement(  args,nlppp);
else if (!strcmp_i(func, _T("rfanonlitelt")))
	return PostRFA::postRFAnonlitelt(args,nlppp);
else if (!strcmp_i(func, _T("rfalitelt")))
	return PostRFA::postRFAlitelt(   args,nlppp);
else if (!strcmp_i(func, _T("rfaelt")))
	return PostRFA::postRFAelt(      args,nlppp);
else if (!strcmp_i(func, _T("rfarulelts")))
	return PostRFA::postRFArulelts(  args,nlppp);
else if (!strcmp_i(func, _T("rfasugg")))
	return PostRFA::postRFAsugg(     args,nlppp);
else if (!strcmp_i(func, _T("rfarule")))
	return PostRFA::postRFArule(     args,nlppp);
else if (!strcmp_i(func, _T("rfarules")))
	return PostRFA::postRFArules(    args,nlppp);
else if (!strcmp_i(func, _T("rfalittoaction")))
	return PostRFA::postRFAlittoaction(args,nlppp);
else if (!strcmp_i(func, _T("rfaactions")))
	return PostRFA::postRFAactions(  args,nlppp);
else if (!strcmp_i(func, _T("rfapres")))
	return PostRFA::postRFApres(     args,nlppp);
else if (!strcmp_i(func, _T("rfacode")))
	return PostRFA::postRFAcode(     args,nlppp);
else if (!strcmp_i(func, _T("rfanodes")))
	return PostRFA::postRFAnodes(    args,nlppp);
else if (!strcmp_i(func, _T("rfaselect")))
	return PostRFA::postRFAselect(   args,nlppp);
else if (!strcmp_i(func, _T("rfaregion")))
	return PostRFA::postRFAregion(   args,nlppp);
else if (!strcmp_i(func, _T("rfaregions")))
	return PostRFA::postRFAregions(  args,nlppp);
else if (!strcmp_i(func, _T("rfarecurse")))
	return PostRFA::postRFArecurse(  args,nlppp);
else if (!strcmp_i(func, _T("rfarecurses")))
	return PostRFA::postRFArecurses( args,nlppp);
else if (!strcmp_i(func, _T("rfarulesfile")))
	return PostRFA::postRFArulesfile(args,nlppp);
else if (!strcmp_i(func, _T("rfarulemark")))								// 08/31/00 AM.
	return PostRFA::postRFArulemark(args,nlppp);

if (!strcmp_i(func, _T("rfbdecl")))											// 12/19/01 AM.
	return PostRFA::postRFBdecl(args,nlppp);
if (!strcmp_i(func, _T("rfbdecls")))											// 12/19/01 AM.
	return PostRFA::postRFBdecls(args,nlppp);
if (!strcmp_i(func, _T("rfberror")))											// 05/29/01 AM.
	return PostRFA::postRFBerror(args,nlppp);
if (!strcmp_i(func, _T("rfbarg")))											// 12/18/99 AM.
	return PostRFA::postRFBarg(args,nlppp);
if (!strcmp_i(func, _T("rfbunmatchedpunct")))								// 05/19/01 AM.
	return PostRFA::postRFBunmatchedpunct(args,nlppp);
else if (!strcmp_i(func, _T("fncallstart")))								// 02/16/00 AM.
	return PostRFA::postRFBfncallstart(args,nlppp);
else if (!strcmp_i(func, _T("scopefncallstart")))						// 02/13/01 AM.
	return PostRFA::postRFBscopefncallstart(args,nlppp);
else if (!strcmp_i(func, _T("addarg")))										// 02/16/00 AM.
	return PostRFA::postRFAaddarg(args,nlppp);
else if (!strcmp_i(func, _T("preaction")))									// 02/17/00 AM.
	return PostRFA::postRFApreaction(args,nlppp);
else if (!strcmp_i(func, _T("varfn")))										// 02/17/00 AM.
	return PostRFA::postRFAvarfn(args,nlppp);
else if (!strcmp_i(func, _T("varfnarray")))								// 10/13/00 AM.
	return PostRFA::postRFAvarfnarray(args,nlppp);

else if (!strcmp_i(func, _T("noop")))					// NO-OP to override default.
	{
	// if (parse->Verbose())												// 09/16/99 AM.
	//	*gout << "   [NOOP!]" << std::endl;									// 09/16/99 AM.

	// Flag that rule is a noop on parse tree.						// 12/15/99 AM.
	// To keep REC pass from infinite looping.						// 12/15/99 AM.
	nlppp->noop_ = true;														// 12/15/99 AM.
	return true;
	}

badname = true;		// Not a known post action name.				// 01/13/00 AM.
return false;
}


/********************************************
* FN:		POSTFINAL
* CR:		11/06/98 AM.
* SUBJ:	Perform final work after all post actions have finished.
* RET:	True if ok, else false.
* NOTE:	Sometimes, this is called after an error in post actions.
*			Should get a flag saying whether something went wrong.
********************************************/

bool Pat::postFinal(
	Nlppp *nlppp				// 11/19/99 AM.
	/*DU*/
//	Node<Pn>* &after		// Saved node for continued matching.	// 12/14/03 AM.
	)
{
// (Even if there was an error in prior actions, may as well proceed.)

// GIVEN THAT RULE HAS MATCHED, RESET THE RIGHTMOST NODE TO WHERE WE ARE GOING NEXT.
// THIS IS THE SAFEST TO DO, GIVEN pnrename and other changes may have taken place.

// NO. KEEP AT THE RIGHTMOST PART OF MATCHING.	// 05/15/12 AM.
//nlppp->rightmost_ = // 05/15/12 AM.
nlppp->rmost_ = 0;		// RECOPT3.	// 08/10/08 AM.

// If rule succeeded, first matched node is the rightmost.	// RECOVERHAUL.// 02/04/13 AM.
// WARN: first_ may not be set right if there was a reduce.	// RECOVERHAUL.// 02/04/13 AM.
if (nlppp->first_)		// RECOVERHAUL.// 02/04/13 AM.
	nlppp->rightmost_ = nlppp->first_;		// RECOVERHAUL.// 02/04/13 AM.
else													// RECOVERHAUL.// 02/04/13 AM.
	nlppp->rightmost_ = 0;	// Failsafe.	// RECOVERHAUL.// 02/04/13 AM.

#ifdef RECOVERHAUL_130204
// TRY TO SET RIGHTMOST AT RIGHT END OF CURRENT MATCH.	// 05/15/12 AM.
// Sometimes red is zeroed out.	// 01/28/13 AM.
// If lookahead in rule, then rightmost is one BEFORE it.// 01/28/13 AM.
if (nlppp->lookahead_)	// 01/28/13 AM.
	nlppp->rightmost_ = nlppp->lookahead_->Left();		// 01/28/13 AM.
else if (nlppp->red_)	// 05/15/12 AM.
	nlppp->rightmost_ = nlppp->red_;	// 05/15/12 AM.
else if (nlppp->last_)	// 05/15/12 AM.
	nlppp->rightmost_ = nlppp->last_;	// 05/15/12 AM.
else if (nlppp->after_)	// 05/15/12 AM.
	nlppp->rightmost_ = nlppp->after_;	// 05/15/12 AM.
#endif

//if (Debug())
//	*gout << std::endl;
nlppp->start_ = nlppp->node_
	= nlppp->after_;	// Set up for continued matching.			// 12/14/03 AM.
if (nlppp->after_)	// 02/14/13 AM.
  {
  Pn *pna = nlppp->after_->getData();	// 02/14/13 AM.
  pna->restart_ = 0;	// Make sure it's cleaned out.	// 02/14/13 AM.
  }
// If collect was consumed, set its nodes to NULL, so callers won't delete.
if (nlppp->used_)
	{
	*gout << _T("[Final post action: Zeroing collect tracking object.]") << std::endl;
	nlppp->collect_->setRoot(0);
	nlppp->collect_->setLast(0);
	}
else			// 11/17/98 AM.
	{
	matchCleanup(nlppp->collect_);
	}

if (nlppp->red_)																// 11/17/99 AM. !
	{
	if (!nlppp->sem_ && !nlppp->dsem_)						// 11/17/99 AM.
		return true;		// No semantics to attach.		// 11/17/99 AM.
	// Attach semantics to the reduce node.
	Pn *pn;
	pn = nlppp->red_->getData();
	// Reduces like listadd don't create a new reduce.				// 01/02/02 AM.
	if (!pn->getSem())											// FIX	// 01/02/02 AM.
		pn->setSem(nlppp->sem_);
	else																// FIX	// 01/02/02 AM.
		{
		delete nlppp->sem_;										// FIX	// 01/02/02 AM.
		nlppp->sem_ = 0;											// FIX	// 01/02/02 AM.
		}
	if (!pn->getDsem())											// FIX	// 01/02/02 AM.
		pn->setDsem(nlppp->dsem_);											// 11/17/99 AM.
	else																// FIX	// 01/02/02 AM.
		{
		Dlist<Ipair>::DeleteDlistAndData(nlppp->dsem_);	// FIX	// 01/02/02 AM.
		nlppp->dsem_ = 0;											// FIX	// 01/02/02 AM.
		}
	}
else		// Implicit NOOP, excise, or other reduces.				// 11/21/99 AM.
	{
	if (nlppp->sem_ || nlppp->dsem_)										// 11/17/99 AM.
		{
		// Get line number of curr rule or post region.
		Irule *rule = nlppp->rule_;										// 05/26/01 AM.
		nlppp->parse_->line_ = rule->getLine();						// 08/24/02 AM.
		// Removing this warning, since using S for local vars.	// 11/28/01 AM.
//		strstream gerrStr(Errbuf,MAXSTR,ios::out);
//		gerrStr << "[Warning: No rule reduce. Can't attach S variables.]" << std::ends;
//		errOut(false);

		if (nlppp->sem_)														// 11/17/99 AM.
			{
			delete nlppp->sem_;				// 11/25/98 AM. Preemptive fix.
			nlppp->sem_ = 0;					// 11/25/98 AM. Preemptive fix.
			}
		if (nlppp->dsem_)														// 11/17/99 AM.
			{
			Dlist<Ipair>::DeleteDlistAndData(nlppp->dsem_);			// 11/17/99 AM.
			nlppp->dsem_ = 0;													// 11/17/99 AM.
			}

		// THIS IS OK, NOT WRONG, SO SHOULDN'T AFFECT PROCESSING	// 08/09/02 AM.
//		return false;												// BUG	// 08/09/02 AM.
		}
	}

return true;
}


/********************************************
* FN:		REDSINGLE
* CR:		10/28/98 AM.
* SUBJ:	Perform single-tier reduction.
* RET:	True if ok, else false.
* NOTE:	11/05/98 AM. Caller will set up continued matching.
*			11/07/98 AM. Perform layering (additional reductions) based
*			on attrs field of sugg.
********************************************/

bool Pat::redSingle(
	Delt<Iarg> *args,			// 05/26/01 AM.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
if (args)																		// 05/26/01 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[single: Takes 0 args.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	}

if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: Null first or last node in range.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: Rule already reduced.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: Collect already used.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: No collected nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Isugg *sugg = nlppp->rule_->getSugg();	// Suggested elt.		// 01/13/00 AM.

// Create a new node for the reduced rule.
long o_start, o_end;
long ustart, uend;	// [UNICODE]
_TCHAR *text;
Pn *tmp = nlppp->first_->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= nlppp->last_->getData()->getEnd();
ustart = tmp->getUstart();	// [UNICODE]
uend = nlppp->last_->getData()->getUend();	// [UNICODE]
Node<Pn> *restart = tmp->restart_;						// RECOPT2.	// 07/20/06 AM.

long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 01/06/02 AM.

Parse *parse = nlppp->parse_;												// 08/22/02 AM.
Sym *sym = parse->getSym(sugg->getName());	// Intern the name.
assert(sym);
_TCHAR *str = sym->getStr();

// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.

Node<Pn> *sub				// The new subtree.
   = Pn::makeNode(o_start, o_end,
			ustart, uend,	// [UNICODE]
   			typ, text, str, sym,
							line,													// 05/17/01 AM.
							parse->currpass_,		// Assoc code pass	// 01/04/02 AM.
							ruleline					// Assoc rule line	// 01/06/02 AM.
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.				// 11/03/98 AM.
tmp->setBase(sugg->getBase());
tmp->setUnsealed(sugg->getUnsealed());									// 10/09/99 AM.
tmp->setFired(true);															// 05/21/01 DD.
tmp->setBuilt(true);															// 05/25/01 AM.

// Perform layering, if any.												// 11/07/98 AM.
Node<Pn> *bottom;
bottom = sub;
sub = Layering(bottom, sugg->getAttrs(),parse);

sub->getData()->restart_ = restart;						// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.

// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();							// 01/13/00 AM.
tree->replaceSubs(nlppp->first_, nlppp->last_, sub, sub);

// Now the parse tree nodes will be placed under the reduce node.
Node<Pn>::linkNodes(bottom, nlppp->first_);							// 11/07/98 AM.

// Notify that a reduce node has been built.
nlppp->red_ = sub;

// FIX UP PARSE STATE HERE.												// 02/04/05 AM.
// For use in optimized recursive pass.								// 02/04/05 AM.
// Also to enable actions after the reduce.							// 02/04/05 AM.

// Presumably the old values needed by callers.			// RECOVERHAUL.	// 02/04/13 AM.
nlppp->first_ = nlppp->last_ = sub; // 02/04/05 AM.	// RECOVERHAUL.	// 02/04/13 AM.

// Clean up.
// Caller will clean up the collect nodes.
return true;
}


/********************************************
* FN:		REDSINGLER
* CR:		01/17/99 AM.
* SUBJ:	Perform single-tier reduction on rule phrase RANGE.
* RET:	True if ok, else false.
* NOTE:	Eg, if we recognize an end of sentence in a context, we
*			want to reduce the period, not the whole context.
********************************************/

bool Pat::redSingler(
	Delt<Iarg> *args,		// Argument list for subphrase.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
//Node<Pn> *nstart, *nend;
//nstart = nend = 0;
Node<Pn> *savefirst = nlppp->first_;		// FIX			// 11/21/99 AM.
Node<Pn> *savelast   = nlppp->last_;		// FIX			// 11/21/99 AM.

// 11/10/99 AM. Not an error for sem to be built at this point.

if (!PostRFA::args_range(_T("singler"), args, nlppp->collect_, 0,nlppp,
										/*DU*/ nlppp->first_, // FIX	// 11/19/99 AM.
												 nlppp->last_))
	return false;

// 11/19/99 AM. Need to get the lookahead node for continuing from,
// if present.
// if (ord = getLookahead(rule))	// If rule has a lookahead elt.
//		findLookahead(nlppp->collect_,ord, /*DU*/ node);
//			// If successful, updates the node to continue on.

// If ranges don't match, save them.// FIX.	// 02/04/13 AM.
if (savefirst == nlppp->first_)		// FIX.	// 02/04/13 AM.
	savefirst = 0;	// Don't restore.	// FIX.	// 02/04/13 AM.
if (savelast == nlppp->last_)			// FIX.	// 02/04/13 AM.
	savelast = 0;	// Don't restore.	// FIX.	// 02/04/13 AM.

bool ok;																			// 11/19/99 AM.
ok = redSingle(0,nlppp);													// 05/26/01 AM.

// Note that restart done by redSingle			// RECOVERHAUL.	// 02/04/13 AM.
// If overlap, let redSingle fix first/last.				// FIX.	// 02/04/13 AM.
if (savefirst)						// FIX.	// 02/04/13 AM.
	nlppp->first_ = savefirst;	// FIX	// 11/21/99 AM.
if (savelast)						// FIX.	// 02/04/13 AM.
	nlppp->last_   = savelast;	// FIX	// 11/21/99 AM.
return ok;																		// 11/19/99 AM.
}


/********************************************
* FN:		REDSINGLEX
* CR:		01/17/99 AM.
* SUBJ:	Perform single-tier reduction on rule phrase RANGE,excising the rest.
* RET:	True if ok, else false.
* NOTE:	Eg, if we recognize an html keyword tag, we want only the keywords,
*			so we excise the rest of the tag.
*			11/21/99 AM. FIXED this function's use of NLPPP object.
*			12/18/99 AM. Only excise up to the first lookahead node.
*			(Assumption is that lookahead node is to the right of what is to
*			be excised.)
********************************************/

bool Pat::redSinglex(
	Delt<Iarg> *args,		// Argument list for subphrase.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
Node<Pn> *nfirst = 0, *nlast = 0;

// 11/10/99 AM. Not an error for sem to be built at this point.
if (!PostRFA::args_range(_T("singlex"), args, nlppp->collect_, 0,nlppp,
										/*DU*/ nfirst, nlast))
	return false;

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();							// 01/13/00 AM.

// Excise from first to nfirst (exclusive).

Node<Pn> *node1, *node2;
if (nlppp->first_ && nlppp->first_ != nfirst)
	{
	node1 = nlppp->first_;
	node2 = nfirst->Left();

	// Setup restart for recursive passes.				// FIX.		// 09/11/07 AM.
	Pn *x = nfirst->getData();								// FIX.		// 09/11/07 AM.
	Pn *y = node1->getData();								// FIX.		// 09/11/07 AM.
	x->restart_ = y->restart_;								// FIX.		// 09/11/07 AM.

//	if (!node1->Left())							// PREEMPTIVE FIX.	// 12/19/99 AM.
//		updateParent(node1,node2);				// PREEMPTIVE FIX.	// 12/19/99 AM.
	tree->exciseSubs(node1, node2);
	// DELETE THE EXTRACTED SUBTREES.
	tree->deleteSubs(node1);
	nlppp->excised_ = true;													// 09/22/08 AM.
	nlppp->first_ = nfirst;	// CRASH FIX.								// 02/04/13 AM.
	}

// If there's a lookahead node, it is the rightmost bound.		// 12/18/99 AM.
if (nlppp->lookahead_)														// 12/18/99 AM.
	{
	node1 = nlast->Right();													// 12/18/99 AM.
	node2 = nlppp->lookahead_->Left();									// 12/18/99 AM.
	if (nlast != node2)														// 12/18/99 AM.
		{
		tree->exciseSubs(node1, node2);									// 12/18/99 AM.
		// DELETE THE EXTRACTED SUBTREES.
		tree->deleteSubs(node1);											// 12/18/99 AM.
		nlppp->excised_ = true;												// 09/22/08 AM.
		}
	}
// Excise from nlast (exclusive) to last.
else if (nlppp->last_ && nlppp->last_ != nlast)
	{
	node1 = nlast->Right();
	node2 = nlppp->last_;
	tree->exciseSubs(node1, node2);
	// DELETE THE EXTRACTED SUBTREES.
	tree->deleteSubs(node1);
	nlppp->excised_ = true;													// 09/22/08 AM.
	// Since last_ node was just deleted, should reset it, I would think.
	nlppp->last_ = nlast;	// Preemptive fix.						// 12/18/99 AM.
	}

// NO. Looks messed up. Anything not reduced is ZAPPED.	// 02/04/13 AM.
#ifdef FIX_130204
// Seems to me this code didn't account for possible overlap of first
// and last being reduced with those of entire matched rule.
// So doing a preemptive fix.												// 12/18/99 AM.
Node<Pn> *savefirst = 0, *savelast = 0;								// 12/18/99 AM.
if (nlppp->first_ != nfirst)												// 12/18/99 AM.
	{
	savefirst = nlppp->first_;												// 12/18/99 AM.
	nlppp->first_ = nfirst;													// 12/18/99 AM.
	}
if (nlppp->last_ != nlast)													// 12/18/99 AM.
	{
	savelast = nlppp->last_;												// 12/18/99 AM.
	nlppp->last_ = nlast;													// 12/18/99 AM.
	}
#endif

// Reduce the range.
// EFFECT: Changes first,last to reduce node.	// 02/04/13 AM.
bool ok = redSingle(0,nlppp);												// 05/26/01 AM.

#ifdef FIX_130204
// The point is that when there's overlap, we want to KEEP the new
// first_ or last_ that redSingle has set, ie, new reduce node.
if (savefirst)						// No overlap on first,				// 12/18/99 AM.
	nlppp->first_ = savefirst;	// so restore first.					// 12/18/99 AM.
if (savelast)						// No overlap on last,				// 12/18/99 AM.
	nlppp->last_ = savelast;	// so restore last.					// 12/18/99 AM.
#endif


return ok;																		// 11/20/99 AM.
}


/********************************************
* FN:		REDMERGE
* CR:		12/02/99 AM.
* SUBJ:	Reduce, dissolving all top-level nodes in phrase.
* RET:	True if ok, else false.
* EX:		See redMerger.
********************************************/

bool Pat::redMerge(
	Nlppp *nlppp
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: Null first or last node in range.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: Rule already reduced.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: Collect already used.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: No collected nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Isugg *sugg = nlppp->rule_->getSugg();	// Suggested elt.			// 01/13/00 AM.

long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 01/06/02 AM.

// Create a new node for the reduced rule.
long o_start, o_end;
long ustart, uend;	// [UNICODE]
Pn *tmp;
_TCHAR *text;
tmp = nlppp->first_->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= nlppp->last_->getData()->getEnd();
ustart = tmp->getUstart();	// [UNICODE]
uend = nlppp->last_->getData()->getUend();	// [UNICODE]
Node<Pn> *restart = tmp->restart_;						// RECOPT2.	// 07/20/06 AM.

Node<Pn> *sub;				// The new subtree.
Sym *sym;
_TCHAR *str;
Parse *parse = nlppp->parse_;												// 08/22/02 AM.
sym = parse->getSym(sugg->getName());	// Intern the name.
assert(sym);
str = sym->getStr();

// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.

sub = Pn::makeNode(o_start, o_end,
		ustart, uend,	// [UNICODE]
		typ, text, str, sym,
							line,													// 05/17/01 AM.
							parse->currpass_,		// Assoc code pass	// 01/04/02 AM.
							ruleline					// Assoc rule line	// 01/06/02 AM.
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.
tmp->setBase(sugg->getBase());
tmp->setUnsealed(sugg->getUnsealed());
tmp->setFired(true);															// 05/21/01 AM.
tmp->setBuilt(true);															// 05/25/01 AM.

// Perform layering, if any.
Node<Pn> *bottom;
bottom = sub;
sub = Layering(bottom, sugg->getAttrs(),parse);

sub->getData()->restart_ = restart;						// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();						// 01/13/00 AM.

// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
tree->replaceSubs(nlppp->first_, nlppp->last_, sub, sub);

////////////////////////////////////
// DISSOLVE ALL THE TOP-LEVEL NODES.
////////////////////////////////////
// (Watch for case where node has no children.)
// Find the first node with children.
Node<Pn> *curr = nlppp->first_;
Node<Pn> *next, *first = 0, *last = 0, *childs, *echilds;
while (curr)
	{
	next = curr->Right();
	if ((childs = curr->Down()))
		{
		childs->setUp(0);	// Detach from parent.

		if (!first)			// At first node with children.
			{
			first = childs;
			if (!(last  = curr->eDown()))
				last = childs->last();
			}
		else					// Extending list of children.
			{
			if (!(echilds = curr->eDown()))
				echilds = childs->last();
			// Link the lists.
			last->setRight(childs);
			childs->setLeft(last);
			last = echilds;
			}
		}

	// Delete the current node.
	if (next)				// FIX.	// 03/13/03 AM.
		next->setLeft(0);	// FIX.	// 03/13/03 AM.
	curr->setDown(0);
	curr->setLeft(0);									// FMR PURIFY.	// 07/27/03 AM.
	curr->setRight(0);
	curr->seteDown(0);
	//Node<Pn>::DeleteNodeAndData(curr);								// 11/25/00 AM.
	tree->deleteSubs(curr);		// More proper deletion.			// 11/25/00 AM.

	curr = next;	// Next top-level node.
	}

if (!first)			// Nothing underneath any node.
	{
//	nlppp->first_ = nlppp->last_ = 0;	// FIX.	// 02/04/13 AM.
	nlppp->first_ = nlppp->last_ = sub;	// FIX.	// 02/04/13 AM.
	nlppp->red_ = sub;
	return true;
	}

nlppp->first_ = first;
nlppp->last_ = last;

////////////////////////////////////

// Now the parse tree nodes will be placed under the reduce node.
Node<Pn>::linkNodes(bottom, nlppp->first_);

// Notify that a reduce node has been built.
nlppp->red_ = sub;
nlppp->first_ = nlppp->last_ = sub;	// FIX.	// 02/04/13 AM.

// Clean up.
// Caller will clean up the collect nodes.
return true;
}


/********************************************
* FN:		REDMERGER
* CR:		12/02/99 AM.
* SUBJ:	Reduce range, dissolving top-level nodes.
* RET:	True if ok, else false.
* EX:		@POST merger(1,2)
*			@RULES  _LINE <- _LINE _LINE @@
*			This will create a new line and dissolve the two _LINE nodes,
*			effectively merging two lines into one.
********************************************/

bool Pat::redMerger(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
Node<Pn> *savefirst = nlppp->first_;
Node<Pn> *savelast   = nlppp->last_;


// If info for first node.	// RECOVERHAUL.	// 02/04/13 AM.
//Pn *tmp = nlppp->first_->getData();	// RECOVERHAUL.	// 02/04/13 AM.
//Node<Pn> *restart = tmp->restart_;	// RECOVERHAUL.	// 02/04/13 AM.

// Not an error for sem to be built at this point.

if (!PostRFA::args_range(_T("merger"), args, nlppp->collect_, 0,nlppp,
										/*DU*/ nlppp->first_,
												 nlppp->last_))
	return false;


if (savefirst == nlppp->first_)	// FIX.	// 02/04/13 AM.
	savefirst = 0;	// FIX.			// 02/04/13 AM.
if (savelast == nlppp->last_)		// FIX.	// 02/04/13 AM.
	savelast = 0;						// FIX.	// 02/04/13 AM.

bool ok;
ok = redMerge(nlppp);

if (savefirst)	// FIX.	// 02/04/13 AM.
	nlppp->first_ = savefirst;
if (savelast)	// FIX.	// 02/04/13 AM.
	nlppp->last_   = savelast;

// Restart info: either first node has it or reduce is first
// and has it.	// RECOVERHAUL.	// 02/04/13 AM.

return ok;
}


/********************************************
* FN:		REDSINGLEZAP
* CR:		11/21/00 AM.
* SUBJ:	Reduce, excising all nodes in phrase.
* RET:	True if ok, else false.
* EX:
********************************************/

bool Pat::redSinglezap(
	Nlppp *nlppp
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redSinglezap: Null first or last node in range.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redSinglezap: Rule already reduced.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redSinglezap: Collect already used.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redSinglezap: No collected nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Isugg *sugg = nlppp->rule_->getSugg();	// Suggested elt.

long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 01/06/02 AM.

// Create a new node for the reduced rule.
long o_start, o_end;
long ustart, uend;	// [UNICODE]
Pn *tmp;
_TCHAR *text;
tmp = nlppp->first_->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= nlppp->last_->getData()->getEnd();
ustart = tmp->getUstart();	// [UNICODE]
uend = nlppp->last_->getData()->getUend();	// [UNICODE]
Node<Pn> *restart = tmp->restart_;						// RECOPT2.	// 07/20/06 AM.

Node<Pn> *sub;				// The new subtree.
Sym *sym;
_TCHAR *str;
Parse *parse = nlppp->parse_;
sym = parse->getSym(sugg->getName());			// Intern the name.
assert(sym);
str = sym->getStr();

// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.

sub = Pn::makeNode(o_start, o_end,
			ustart, uend,	// [UNICODE]
			typ, text, str, sym,
							line,													// 05/17/01 AM.
							parse->currpass_,		// Assoc code pass	// 01/04/02 AM.
							ruleline			// Associated rule line		// 01/06/02 AM.
//							nlppp->line_	// Associated code line		// 01/04/02 AM.
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.
tmp->setBase(sugg->getBase());
tmp->setUnsealed(sugg->getUnsealed());
tmp->setFired(true);															// 05/21/01 AM.
tmp->setBuilt(true);															// 05/25/01 AM.

tmp->restart_ = restart;									// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.

// NO ELT ACTIONS, BECAUSE EVERYTHING'S GONNA GET ZAPPED.
// Perform layering, if any.
//Node<Pn> *bottom;
//bottom = sub;
//sub = Layering(bottom, sugg->getAttrs());

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();

// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
tree->replaceSubs(nlppp->first_, nlppp->last_, sub, sub);

////////////////////////////////////
// DELETE ALL THE NODES.
////////////////////////////////////
tree->deleteSubsNC(nlppp->first_);
nlppp->excised_ = true;													// 09/22/08 AM.

//nlppp->first_ = nlppp->last_ = 0;	// FIX.	// 02/04/13 AM.
nlppp->first_ = nlppp->last_ = sub;	// FIX.	// 02/04/13 AM.
nlppp->red_ = sub;
return true;
}


/********************************************
* FN:		POSTLISTADD
* CR:		11/16/99 AM.
* SUBJ:	Reduction to add an element to a list.
* RET:	True if ok, else false.
* EX:		_xNIL <- _Caps _xWHITE _xCAP @@
*			listadd(1,3) or listadd(1,3,"false")	-- discard whitespace.
*			listadd(1,3,"true") -- keep whitespace.
* FORM:	listadd(listpos, itempos, [keep])
* NOTE:	Useful in REC (recursive analyzer passes).
*			Suggested elt isn't used!
*			12/14/99 AM. Adding an optional boolean arg to keep the
*			stuff between list and item.
* ASS:	Assumes the item is to the right of the list.
*			Discards nodes between list and item.
*			Assumes these are separators such as whitespace, commas, etc.
*			Does nothing with nodes after item.
*			12/05/99 AM. Handling BACKWARD case, eg,
*			_xNIL <- _xCAP _xWHITE _Caps [trigger] @@
********************************************/

bool Pat::postListadd(
	Delt<Iarg> *args,
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: Null first or last node in range.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: 2 or 3 args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: Rule already reduced.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: Collect already used.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

if (!nlppp->collect_ || !nlppp->collect_->getRoot())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: No collected nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

////////////////////////////////////////////
// Get the number range and label arguments.
////////////////////////////////////////////

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: 2 or 3 args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

// Third arg is optional.												// 12/14/99 AM.
Iarg *arg3 = 0;															// 12/14/99 AM.
if ((args = args->Right()))												// 12/14/99 AM.
	{
	arg3 = args->getData();												// 12/14/99 AM.
	if (args->Right())
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Too many args.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	}


long long num1, num2;
switch (arg1->getType())
	{
	case IANUM:
		num1 = arg1->getNum();
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Arg1 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

switch (arg2->getType())
	{
	case IANUM:
		num2 = arg2->getNum();
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Arg2 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

_TCHAR *keepstr = 0;														// 12/14/99 AM.
bool keep = false;				// DEFAULT							// 12/14/99 AM.
if (arg3)							// Keep flag arg present.		// 12/14/99 AM.
	{
	switch (arg3->getType())												// 12/14/99 AM.
		{
		case IASTR:																// 12/14/99 AM.
			keepstr = arg3->getStr();										// 12/14/99 AM.
			break;
		case IANUM:																// 12/14/99 AM.
		default:
			{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Optional arg3 must be true/false.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

			return false;
		}

	if (!keepstr || !*keepstr)		// Optional flag is absent.	// 12/14/99 AM.
		keep = false;					// Reaffirm default.				// 12/14/99 AM.
	else if (!strcmp_i(keepstr, _T("true")))									// 12/14/99 AM.
		keep = true;															// 12/14/99 AM.
	else if (!strcmp_i(keepstr, _T("false")))								// 12/14/99 AM.
		keep = false;															// 12/14/99 AM.
	else
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Arg 3 must be true/false.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
	// 12/14/99 AM.
		return false;															// 12/14/99 AM.
		}
	}

// Check self-consistency.
if (!num1 || !num2)															// 12/05/99 AM.
	return false;

if (num1 > num2)																// 12/05/99 AM.
	return Pat::listAddleft(												// 12/05/99 AM.
		num2,		// The ITEM elt.											// 12/05/99 AM.
		num1,		// The LIST elt.											// 12/05/99 AM.
		keep,		// The KEEP flag.											// 12/14/99 AM.
		nlppp);																	// 12/05/99 AM.

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

////////////////////////////////////////////////
// Find the collect elts for LIST and ITEM.
////////////////////////////////////////////////
// These numbers represent a range of rule elts in the collect.
// List and item must not be optional.
Node<Pn> *colls = nlppp->collect_->getRoot();
Node<Pn> *node1 = 0, *node2 = 0;

// Record the start and end collect of the range, so we can fix up the
// collect.
Node<Pn> *coll1 = 0, *coll2 = 0;

while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Collect error.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	}
if (!(coll1 = colls))
	return false;

if (!(node1 = coll1->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: List elt must be nonoptional.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (node1 != coll1->eDown())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: List elt must match a single node.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listadd: Collect error.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	}
if (!(coll2 = colls))
	return false;

if (!(node2 = coll2->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: Item elt must be nonoptional.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (node2 != coll2->eDown())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: Item elt must match a single node.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *childs = node1->Down();
if (!childs)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: List node has no children.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

//////////////////////////////////
// Do the listing actions.
//////////////////////////////////
// Remove all nodes between node1 and node2.
// Place node2 as the last child of node1.
// Fix things up accordingly.

// Fix up the first,last nodes for subsequent actions.
// First node is ok as is, since the list node is first or later.
if (nlppp->last_ == node2)
	nlppp->last_ = node1;		// node1 is the "reduced" node.

// Excise from node1's right through node2.
Node<Pn> *nodes = node1->Right();
if (node2->Right())
	node2->Right()->setLeft(node1);
node1->setRight(node2->Right());
nodes->setLeft(0);					// Detach.
node2->setRight(0);					// Detach.

if (!keep)	// Only keeping node2							// 12/14/99 AM.
	{
	// Separate node2 from the rest.
	if (node2->Left())
		node2->Left()->setRight(0);	// Separate.
	node2->setLeft(0);					// Separate.

	if (nodes && nodes != node2)		// If intervening nodes.
		{
		//Node<Pn>::deleteNodes(nodes);	// Delete them	// BUG.	// 08/07/02 AM.
		// DELETE THE EXTRACTED SUBTREES.					// FIX.	// 08/07/02 AM.
		Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();		// FIX.	// 08/07/02 AM.
		tree->deleteSubs(nodes);								// FIX.	// 08/07/02 AM.
		// Assumed to be whitespace, commas, or other chaff.
		}
	nodes = node2; // Item is the 'list' to be glommed.	// 12/14/99 AM.
	}


// Place nodes under node1.
// List goes from nodes to node2.								// 12/14/99 AM.
if (!nodes)																// 12/14/99 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listadd: Error..]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
nlppp->excised_ = true;													// 09/22/08 AM.
Node<Pn> *end = childs->last();
end->setRight(nodes);												// 12/14/99 AM.
nodes->setLeft(end);

// Update information for the list node.						// 11/17/99 AM.
Pn *pn1 = node1->getData();										// 11/17/99 AM.
Pn *pn2 = node2->getData();										// 11/17/99 AM.
pn1->setEnd(pn2->getEnd());	// Update the end offset.	// 11/17/99 AM.

//////////
// FIXUPS.
//////////
// Fixup coll1 to point to item node, which is now last child.
coll1->seteDown(node2);

// Remove collect elts from the one after coll1 through coll2.
Node<Pn> *coll = coll1->Right();	// First one to be deleted.
if (coll2->Right())
	coll2->Right()->setLeft(coll1);
coll1->setRight(coll2->Right());

coll->setLeft(0);			// Detach colls to be deleted.
coll2->setRight(0);		// Detach end of colls to be deleted.

// Delete coll through coll2.
Node<Pn>::deleteNodes(coll);

// LIST NODE IS THE "NEW" REDUCE NODE!			// FIX	// 11/17/99 AM.
//nlppp->red_ = node1;	// 04/25/10 AM.

return true;
}


/********************************************
* FN:		LISTADDLEFT
* CR:		12/05/99 AM.
* SUBJ:	Reduction to add an element to a list, going BACKWARD.
* RET:	True if ok, else false.
* EX:		_xNIL <- _xCAP _xWHITE _Caps [trig] @@
* NOTE:	See postListadd.
********************************************/

bool Pat::listAddleft(
	int num1,			// The ITEM.
	int num2,			// The LIST.
	bool keep,			// If keeping intervening nodes.		// 12/14/99 AM.
	Nlppp *nlppp
	)
{
if (!num1 || !num2 || num1 > num2)
	return false;

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

////////////////////////////////////////////////
// Find the collect elts for ITEM and LIST.
////////////////////////////////////////////////
// These numbers represent a range of rule elts in the collect.
// List and item must not be optional.
Node<Pn> *colls = nlppp->collect_->getRoot();
Node<Pn> *node1 = 0, *node2 = 0;

// Record the start and end collect of the range, so we can fix up the
// collect.
Node<Pn> *coll1 = 0, *coll2 = 0;

while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listAddleft: Collect error.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	}
if (!(coll1 = colls))
	return false;

if (!(node1 = coll1->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: Item elt must be nonoptional.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (node1 != coll1->eDown())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: Item elt must match a single node.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

while (num2-- > 0)
	{
	if (!(colls = colls->Right()))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[listAddleft: Collect error.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	}
if (!(coll2 = colls))
	return false;

if (!(node2 = coll2->Down()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: List elt must be nonoptional.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (node2 != coll2->eDown())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: List elt must match a single node.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *childs = node2->Down();
if (!childs)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: List node has no children.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *restart = node1->getData()->restart_;			// RECOPT2.	// 07/20/06 AM.
node2->getData()->restart_ = restart;						// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = node2;				// RECOPT2.	// 07/20/06 AM.

//////////////////////////////////
// Do the listing actions.
//////////////////////////////////
// Remove all nodes between node1 and node2.
// Place node1 as the FIRST child of node2.
// Fix things up accordingly.

// Fix up the first,last nodes for subsequent actions.
// Last node is ok as is, since the list node is last or earlier.
if (nlppp->first_ == node1)
	nlppp->first_ = node2;		// node2 is the "reduced" node.

// Excise from node1 up to but not including node2.
Node<Pn> *ends = node2->Left();
if (node1->Left())
	node1->Left()->setRight(node2);
else	// NODE IS FIRST IN ITS LIST.
	{
	// DETACH ITEM FROM TREE, ATTACH LIST TO TREE.
	if (node1->Up())
		node1->Up()->setDown(node2);
	node2->setUp(node1->Up());
	node1->setUp(0);
	}
node2->setLeft(node1->Left());
node1->setLeft(0);					// Detach.
ends->setRight(0);					// Detach.

if (!keep)			// Not keeping intervening nodes.		// 12/14/99 AM.
	{
	// Separate node1 from the rest.
	Node<Pn> *nodes = node1->Right();
	if (nodes)
		nodes->setLeft(0);				// Separate.
	node1->setRight(0);					// Separate.

	if (nodes && nodes != node2)		// If intervening nodes.
		{
		//Node<Pn>::deleteNodes(nodes);	// Delete them	// BUG.	// 08/07/02 AM.
		// DELETE THE EXTRACTED SUBTREES.					// FIX.	// 08/07/02 AM.
		Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();		// FIX.	// 08/07/02 AM.
		tree->deleteSubs(nodes);								// FIX.	// 08/07/02 AM.
		// Assumed to be whitespace, commas, or other chaff.
		}
	ends = node1;	// New end of 'list'.						// 12/14/99 AM.
	}

// Nodes go from node1 to end, whether kept or not.		// 12/14/99 AM.

// Place node1 under node2.
if (!node1)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[listAddleft: Error..]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
nlppp->excised_ = true;													// 09/22/08 AM.

// Detach children of list node.
if (childs)
	childs->setUp(0);
node2->setDown(0);

// Push item chain onto list of children.
ends->setRight(childs);												// 12/14/99 AM.
if (childs)
	childs->setLeft(ends);											// 12/14/99 AM.
childs = node1;			// Item is new start of list.

// Reattach children.
childs->setUp(node2);
node2->setDown(childs);


// Update information for the list node.
Pn *pn1 = node1->getData();
Pn *pn2 = node2->getData();
pn2->setStart(pn1->getStart());	// Update the start offset.
pn2->setText(pn1->getText());		// New start of node's text.

//////////
// FIXUPS.
//////////
// Fixup coll2 to point to item node, which is now first child.
coll2->setDown(node1);

// Remove collect elts from coll1 up to but not including coll2.
Node<Pn> *coll = coll1;				// First one to be deleted.
Node<Pn> *cends = coll2->Left();	// Last one to be deleted.
if (coll->Left())
	coll->Left()->setRight(coll2);
else				// FIRST COLLECT IN LIST.
	nlppp->collect_->setRoot(coll2);
coll2->setLeft(coll->Left());

coll->setLeft(0);			// Detach colls to be deleted.
cends->setRight(0);		// Detach end of colls to be deleted.

// Delete coll through cends.
Node<Pn>::deleteNodes(coll);

// LIST NODE IS THE "NEW" REDUCE NODE!
//nlppp->red_ = node2;	// Commented.	// 04/25/10 AM.

return true;
}


/********************************************
* FN:		POSTGROUP
* CR:		06/15/99 AM.
* SUBJ:	Reduce nodes for a group of adjacent rule elements.
* RET:	True if ok, else false.
* FORM:	@POST
*				group(NUM1,NUM2,LABEL)
*			where NUM1,NUM2 are a range of rule elts and LABEL is the
*			name to give the group node.
* NOTE:	To support "label" or "layer" in RUG.
*			12/19/99 AM. Ok for part or all of the elts to be grouped to
*			be absent (ie, because of optional rule elements.)
*			11/24/00 AM. There is now a fn/fnGroup.
*			05/24/01 AM. Even so, need olgroup in the RFA.
********************************************/

bool Pat::postGroup(
	Delt<Iarg> *args,		// Argument list for the post action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Null first or last node in range.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Three args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Rule already reduced.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Collect already used.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: No collected nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

////////////////////////////////////////////
// Get the number range and label arguments.
////////////////////////////////////////////

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Three args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Three args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg3;
arg3 = args->getData();

if (args->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: More than three args given.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

long num1, num2;
switch (arg1->getType())		// 11/15/99 AM.
	{
	case IANUM:
		num1 = arg1->getNum();	// 11/15/99 AM.
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[postGroup: Arg1 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

switch (arg2->getType())		// 11/15/99 AM.
	{
	case IANUM:
		num2 = arg2->getNum();	// 11/15/99 AM.
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[postGroup: Arg2 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

if (arg3->getType() != IASTR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postGroup: Label arg must be string.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Check self-consistency.
if (!num1 || !num2)	// 09/16/08 AM.
	return false;		// 09/16/08 AM.

if (nlppp->nlookahead_)											// 09/16/08 AM.
	{
	if (num2 >= nlppp->nlookahead_)							// 09/16/08 AM.
		{
		num2 = (nlppp->nlookahead_ - 1);						// 09/16/08 AM.
		std::_t_strstream gerrStr;
		gerrStr << _T("[group: Cannot group lookahead node.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}
	}
else if (num2 > nlppp->len_)									// 09/16/08 AM.
	{
	num2 = nlppp->len_;											// 09/16/08 AM.
	std::_t_strstream gerrStr;
	gerrStr << _T("[group: 2nd number too large.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	}
if (num1 > num2)													// 09/16/08 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[group: FROM is greater than TO.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;													// 09/16/08 AM.
	}

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

_TCHAR *tname;	// 07/24/07 AM.
_TCHAR *name;
tname = arg3->getStr();

nlppp->parse_->internStr(tname, /*UP*/name);	// Intern str.	// 07/24/07 AM.

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
		std::_t_strstream gerrStr;
		gerrStr << _T("[postGroup: collect with null end found.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);

		node2 = 0;
		break;				// The range goes from node1 to the end of list.
		}
	}
coll2 = colls;

if (!node1)																		// 05/24/01 AM.
	return true;		// Assume this is empty optional elt.		// 05/24/01 AM.

if (groupElts(coll1,coll2,node1,node2,name,nlppp))					// 09/23/00 AM.
	return true;																// 09/23/00 AM.
return false;
}


/********************************************
* FN:		GROUPELT
* CR:		09/23/00 AM.
* SUBJ:	Perform group action for rule element.
* RET:	coll - New collect elt, if mangled collect.
*			UP pcoll - Mangles collect for current elt.
*				nlppp - Mangles parse tree, first and last.
* NOTE:	Intended to "reduce" wildcard matches or repetitive matches of a
*			rule element.
* OPT:	Don't need to trash the single collect node.
********************************************/

Node<Pn> *Pat::groupElt(
	Node<Pn> *pcoll,			// Collect covering nodes of current elt.
	_TCHAR *name,					// Name for group of nodes.
	Nlppp *nlppp
	)
{
if (!pcoll || !name || !*name || !nlppp)
	return 0;
return groupElts(pcoll,pcoll,pcoll->Down(),pcoll->eDown(),name,nlppp);
}


/********************************************
* FN:		GROUPELTS
* CR:		09/23/00 AM.
* SUBJ:	Perform grouping of rule elts.
* RET:	coll - New collect elt, if mangled collect.  Else null.
*			UP nlppp - Mangles parse tree, collect, first and last.
* NOTE:	Modularity function.
* ASS:	Assume name is interned.	07/24/07 AM.
********************************************/

Node<Pn> *Pat::groupElts(
	Node<Pn> *coll1,
	Node<Pn> *coll2,
	Node<Pn> *node1,
	Node<Pn> *node2,
	_TCHAR *name,					// Name for group of nodes.
	Nlppp *nlppp
	)
{
if (!name || !nlppp)
	return 0;

if (!coll1 || !coll2 || !node1 || !node2)
	return 0;		// ALLOW COMPLETELY OPTIONAL GROUP.			// 12/19/99 AM.

// Build the group node.													// 05/25/00 AM.
Node<Pn> *sub = group(node1,node2,name,nlppp);						// 05/25/00 AM.

//////////
// FIXUPS.
//////////
// Make collect elt for group node.
Pn *pn;
pn = new Pn();
pn->setRulelt(0);			// No corresponding rule elt.
Node<Pn> *coll;
coll = new Node<Pn>(*pn);
delete pn;					// This was copied in making node.
coll->setDown(sub);
coll->seteDown(sub);

if (coll1->Left())
	coll1->Left()->setRight(coll);
else
	nlppp->collect_->setRoot(coll);
if (coll2->Right())
	coll2->Right()->setLeft(coll);
coll->setLeft(coll1->Left());
coll->setRight(coll2->Right());
coll1->setLeft(0);
coll2->setRight(0);

// Delete coll1 through coll2.
Node<Pn>::deleteNodes(coll1);

return coll;
}

/********************************************
* FN:		GROUP
* CR:		05/25/00 AM.
* SUBJ:	Idiom for performing the group action.
* RET:	The constructed group node.
* NOTE:	For use by both interpreted and compiled analyzer.
********************************************/

Node<Pn> *Pat::group(
	Node<Pn> *node1,	// from
	Node<Pn> *node2,	// to
	_TCHAR *name,			// Name for group node.
	Nlppp *nlppp
	)
{

//////////////////////////////////
// Reduce the corresponding nodes.
//////////////////////////////////
// Create a group node.
Pn *tmp = node1->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
long o_start	= tmp->getStart();
_TCHAR *text		= tmp->getText();	// Start of group's text in input buffer.
long o_end		= node2->getData()->getEnd();
long ustart = tmp->getUstart();	// [UNICODE]
long uend = node2->getData()->getUend();	// [UNICODE]

// Till we keep track of the ord of rightmost in a rule match,
// just set back to the beginning of the rule.		// RECOPT2.	// 07/21/06 AM.
//Node<Pn> *restart = tmp->restart_;				 // RECOPT2.	// 07/20/06 AM.

Parse *parse = nlppp->parse_;												// 08/22/02 AM.

// Distinguish compiled and interp systems.							// 08/09/02 AM.
// OPT: May want to separate compiled and interp...				// 08/09/02 AM.
long ruleline;																	// 08/09/02 AM.
long passnum;																	// 08/09/02 AM.
if (nlppp->sugg_)	// COMPILED.											// 08/09/02 AM.
	{
	ruleline = nlppp->sugg_->ruleline;									// 08/09/02 AM.
	passnum = nlppp->sugg_->passnum;										// 08/09/02 AM.
	}
else					// INTERP.
	{
	// These are the LINE OF CODE WE ARE IN.							// 02/03/05 AM.
//	ruleline = parse->line_;
//	passnum = parse->currpass_;											// 08/09/02 AM.

	// These are the RULE WE ARE IN.										// 02/03/05 AM.
	// If in a function, pass and line totally different.			// 02/02/05 AM.
	ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);		// 01/06/02 AM.
	passnum = parse->rulepass_;											// 02/03/05 AM.
	}

// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(name);										// 05/01/03 AM.

Sym *sym = parse->getSym(name);			// Intern the name.
assert(sym);
_TCHAR *str = sym->getStr();
// The new subtree.
Node<Pn> *sub = Pn::makeNode(o_start, o_end,
				ustart, uend,	// [UNICODE]
				typ, text, str, sym,
							line,													// 05/17/01 AM.
							passnum,			// Associated code pass		// 08/09/02 AM.
							ruleline			// Associated rule line		// 01/06/02 AM.
//							nlppp->line_	// Associated code line		// 01/04/02 AM.
							);
tmp = sub->getData();

// Rough it out, till we track ord of rightmost in a rule.
//tmp->restart_ = restart;									// RECOPT2.	// 07/20/06 AM.
//nlppp->rightmost_ = sub;									// RECOPT2.	// 07/20/06 AM.
if (nlppp->first_ == node1)								// RECOPT2.	// 07/21/06 AM.
	{
	sub->getData()->restart_								// RECOPT2.	// 07/21/06 AM.
							= node1->getData()->restart_;	// RECOPT2.	// 07/21/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.
	}
else																// RECOPT2.	// 07/21/06 AM.
	nlppp->rmost_ = nlppp->rightmost_ = nlppp->first_; // RECOPT2.	// 07/20/06 AM.

//tmp->setBase(true);	// May want this to be an arg to GROUP action.
tmp->setFired(true);															// 05/21/01 AM.
tmp->setBuilt(true);															// 05/25/01 AM.

// Fix up the first,last nodes for subsequent actions.
if (nlppp->first_ == node1)
	nlppp->first_ = sub;
if (nlppp->last_ == node2)
	nlppp->last_ = sub;

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();						// 01/13/00 AM.

// Now the parse tree nodes themselves will be excised and replaced by
// the group node.
tree->replaceSubs(node1, node2, sub, sub);

// Now the parse tree nodes will be placed under the group node.
Node<Pn>::linkNodes(sub, node1);
return sub;
}

/********************************************
* FN:		POSTEXCISE
* CR:		11/04/98 AM.
* SUBJ:	Excise some nodes that matched current rule.
* RET:	True if ok, else false.
********************************************/

bool Pat::postExcise(
	Delt<Iarg> *args,			// Argument list for the excise action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
// Get the re_start and re_end arguments, telling which rule elt
// range to remove.  Use the collect to map this to the node range
// to be excised.

if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Excise post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Excise post action: Rule already reduced.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Excise post action: Collect already used.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Excise post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (args->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Excise post action: More than two args given.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);

//	node = nodeRight(node, end);
//	if (Debug()) *gout << std::endl;
	return false;
	}

// First time this action is executed, convert types of args as needed.
// OPT	// 11/06/99 AM.
long num1, num2;
switch (arg1->getType())
	{
	case IANUM:
		num1 = arg1->getNum();				// 11/06/99 AM.
		break;
	case IASTR:
		// Convert string to num.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		// Convert argument type, for next invocation of this action.
		arg1->setType(IANUM);				// 11/06/99 AM.
		arg1->setNum(num1);					// 11/06/99 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Excise post action: Arg1 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}
switch (arg2->getType())
	{
	case IANUM:
		num2 = arg2->getNum();				// 11/06/99 AM.
		break;
	case IASTR:
		// Convert string to num.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		// Convert argument type, for next invocation of this action.
		arg2->setType(IANUM);				// 11/06/99 AM.
		arg2->setNum(num2);					// 11/06/99 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Excise post action: Arg2 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;

// MAKE SURE EXCISING PRIOR TO THE LOOKAHEAD POSITION.	// FIX	// 09/06/08 AM.
if (nlppp->nlookahead_ && num2 >= nlppp->nlookahead_)		// FIX	// 09/06/08 AM.
	{
	std::_t_strstream gerrStr;														// 09/06/08 AM.
	gerrStr << _T("[Excise action: Cannot excise lookahead node(s).]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);									// 09/06/08 AM.
	return false;																	// 09/06/08 AM.
	}

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// These numbers represent a range of rule elts in the collect.
// 11/26/98 AM. Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.
Node<Pn> *colls;
Node<Pn> *coll1=0;					// First coll to be excised.	// 03/31/01 AM.
Node<Pn> *coll2=0;					// Last  coll to be excised.	// 03/31/01 AM.
Node<Pn> *node1, *node2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;																	// 03/31/01 AM.
node1 = coll1->Down();	// Try to get start of range.				// 11/26/98 AM.
node2 = coll1->eDown();	// Try to get end of range.				// 11/26/98 AM.
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
		std::_t_strstream gerrStr;
		gerrStr << _T("[postExcise: collect with null end found.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);

		node2 = 0;
		break;				// The range goes from node1 to the end of list.
		}
	}
coll2 = colls;																	// 03/31/01 AM.

// Watch out for optional matches!  Collect is empty placeholder. 11/25/98 AM.
//node1 = coll1->Down();			// 11/26/98 AM.
//node2 = coll2->eDown();			// 11/26/98 AM.

//if (Debug())
//	{
//	*gout << "   [Executing EXCISE action.]" << std::endl;
//	*gout << "   [Excising " << *node1 << " to " << *node2 << std::endl;
//	}

if (!node1)																		// 03/31/01 AM.
	return true;																// 03/31/01 AM.

if (!node2)																		// 03/31/01 AM.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[excise: Error. Internal error.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
			// 03/31/01 AM.
	return false;																// 03/31/01 AM.
	}

// PREEMPTIVE FIX. I believe the Text of parent node is not set aright here.
//updateParent(node1,node2);	// PREEMPTIVE FIX.						// 12/19/99 AM.

// FIX UP STATE OF THE PARSE.								// FIX.		// 03/31/01 AM.
// This will enable actions to follow excise in @POST.			// 03/31/01 AM.
// Fix up the first,last nodes for subsequent actions.			// 03/31/01 AM.
Node<Pn> *nbefore = node1->Left();										// 03/31/01 AM.
Node<Pn> *nafter = node2->Right();										// 03/31/01 AM.
if (nlppp->first_ == node1 && nlppp->last_ == node2)				// 03/31/01 AM.
	{
	// Excising everything that matched rule.							// 03/31/01 AM.
	nlppp->first_ = nlppp->last_ = 0;									// 03/31/01 AM.
	nlppp->rmost_ = nlppp->rightmost_ = nbefore;		// RECOPT2.	// 07/23/06 AM.
	}
else if (nlppp->first_ == node1)											// 03/31/01 AM.
	{
	nlppp->first_ = nafter;													// 03/31/01 AM.
	nlppp->rmost_ = nlppp->rightmost_ = nafter;		// RECOPT2.	// 07/21/06 AM.
	// Assure no pointer to excised nodes.				// RECOPT2.	// 07/21/06 AM.
	nafter->getData()->restart_ =							// RECOPT2.	// 07/21/06 AM.
							node1->getData()->restart_;	// RECOPT2.	// 07/21/06 AM.
	}
else if (nlppp->last_ == node2)											// 03/31/01 AM.
	{
	nlppp->last_ = nbefore;													// 03/31/01 AM.
	nlppp->rmost_ = nlppp->rightmost_ = nbefore;		// RECOPT2.	// 07/21/06 AM.
	}

// Fix up the collect data structure.					// FIX.		// 03/31/01 AM.
Node<Pn> *cbefore = coll1->Left();										// 03/31/01 AM.
Node<Pn> *cafter = coll2->Right();										// 03/31/01 AM.
if (cbefore)																	// 03/31/01 AM.
	cbefore->setRight(cafter);												// 03/31/01 AM.
else
	nlppp->collect_->setRoot(cafter);									// 03/31/01 AM.
if (cafter)																		// 03/31/01 AM.
	cafter->setLeft(cbefore);												// 03/31/01 AM.
coll1->setLeft(0);		// Detach.										// 03/31/01 AM.
coll2->setRight(0);		// Detach.										// 03/31/01 AM.

// Delete the collect range.												// 03/31/01 AM.
Node<Pn>::deleteNodes(coll1);												// 03/31/01 AM.


//if (node1)																	// 03/31/01 AM.
//	{
	Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();					// 01/13/00 AM.
	tree->exciseSubs(node1, node2);
	// DELETE THE EXTRACTED SUBTREES HERE.
	//Node<Pn>::deleteNodes(node1);			// 11/19/98 AM. LEAK.
	tree->deleteSubs(node1);					// 11/19/98 AM.
	nlppp->excised_ = true;													// 09/22/08 AM.
//	}
return true;
}


/********************************************
* FN:		POSTSPLICE
* CR:		07/30/99 AM.
* SUBJ:	Zap some matched nodes and splice in their children.
* RET:	True if ok, else false.
* NOTE:	07/19/02 AM. There's now a FnSplice function.
*			07/19/02 AM. This old one does nothing with the COLLECT structure.
********************************************/

bool Pat::postSplice(
	Delt<Iarg> *args,			// Argument list for the excise action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
// Get the re_start and re_end arguments, telling which rule elt
// range to remove.  Use the collect to map this to the node range
// to be spliced.

if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Splice post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Splice post action: Rule already reduced.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Splice post action: Collect already used.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Splice post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (args->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Splice post action: More than two args given.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);

//	node = nodeRight(node, end);
//	if (Debug()) *gout << std::endl;
	return false;
	}

long num1, num2;
switch (arg1->getType())		// 11/15/99 AM.
	{
	case IANUM:
		num1 = arg1->getNum();	// 11/15/99 AM.
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg1->getStr(), num1))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Splice post action: Arg1 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

switch (arg2->getType())		// 11/15/99 AM.
	{
	case IANUM:
		num2 = arg2->getNum();	// 11/15/99 AM.
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg2->getStr(), num2))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Splice post action: Arg2 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

// Check self-consistency.
if (!num1 || !num2 || num1 > num2)
	return false;

num2 -= num1;			// Now num2 is the number of nodes beyond num1.

// These numbers represent a range of rule elts in the collect.
// 11/26/98 AM. Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.
Node<Pn> *colls;
Node<Pn> *node1, *node2;
colls = nlppp->collect_->getRoot();
while (--num1 > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
node1 = colls->Down();	// Try to get start of range.	// 11/26/98 AM.
node2 = colls->eDown();	// Try to get end of range.	// 11/26/98 AM.
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
		std::_t_strstream gerrStr;
		gerrStr << _T("[postSplice: collect with null end found.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);

		node2 = 0;
		break;				// The range goes from node1 to the end of list.
		}
	}

// Watch out for optional matches!  Collect is empty placeholder. 11/25/98 AM.
//node1 = coll1->Down();			// 11/26/98 AM.
//node2 = coll2->eDown();			// 11/26/98 AM.

//if (Debug())
//	{
//	*gout << "   [Executing SPLICE action.]" << std::endl;
//	*gout << "   [Splicing " << *node1 << " to " << *node2 << std::endl;
//	}

Node<Pn> *newstart,*newend;											// 07/19/02 AM.
if (node1)										// 11/25/98 AM.
	{
	Node<Pn> *restart = node1->getData()->restart_;	// RECOPT2.	// 07/20/06 AM.
	Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();	// 01/13/00 AM.
	tree->spliceSubs(node1, node2,									// 07/30/99 AM.
							newstart, newend);							// 07/19/02 AM.
	//tree->deleteSubs(node1);											// 11/19/98 AM.
	if (newstart)											// RECOPT2.	// 07/21/06 AM.
		{
		newstart->getData()->restart_ = restart;	// RECOPT2.	// 07/21/06 AM.
		nlppp->rmost_ = nlppp->rightmost_ = newstart;// RECOPT2.	// 07/21/06 AM.
		}
	}
return true;
}


/********************************************
* FN:		POSTSETBASE
* CR:		12/12/99 AM.
* SUBJ:	Set base for given elt.
* RET:	True if ok, else false.
* FORM:	setbase(pos, bool)
*				pos = element number
*				bool = "true" or "false".
********************************************/

bool Pat::postSetbase(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase post action: Two args required.]") << std::ends;
	return nlppp->parse_->errOut(&gerrStr,false);
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase post action: Rule already reduced.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase post action: Collect already used.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (args->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase post action: More than two args given.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

long pos = 0;	// Elt number.
_TCHAR *flagstr = 0;	// True or false.
switch (arg1->getType())
	{
	case IANUM:
		pos = arg1->getNum();
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg1->getStr(), pos))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Setbase post action: Arg1 must be numeric.]") << std::ends;
		return postErrout(false,nlppp);									// 11/20/01 AM.
//		return errOut(false);												// 11/20/01 AM.
		}
	}

switch (arg2->getType())
	{
	case IASTR:
		flagstr = arg2->getStr();
		break;
	case IANUM:
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Setbase post action: Arg2 must be string.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

// Check self-consistency.
if (!pos)
	return false;

bool flag;
if (!strcmp_i(_T("true"), flagstr))
	flag = true;
else if (!strcmp_i(_T("false"), flagstr))
	flag = false;
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase action: arg 2 must be \"true\"/\"false\".]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// These numbers represent a range of rule elts in the collect.
// Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.
Node<Pn> *colls;
Node<Pn> *node1, *node2;
colls = nlppp->collect_->getRoot();
while (--pos > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
node1 = colls->Down();	// Try to get start of range.
node2 = colls->eDown();	// Try to get end of range.

if (!node1)	// 03/27/00 AM.
	return true;			// Assume it was an optional elt.

if (node1 != node2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setbase action: Bad collect.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Pn *pn = node1->getData();
pn->setBase(flag);			// Set base bit-flag appropriately.
return true;
}


/********************************************
* FN:		POSTSETUNSEALED
* CR:		01/26/00 AM.
* SUBJ:	Set unsealed for given elt.
* RET:	True if ok, else false.
* FORM:	setunsealed(pos, bool)
*				pos = element number
*				bool = "true" or "false".
********************************************/

bool Pat::postSetunsealed(
	Delt<Iarg> *args,
	Nlppp *nlppp
	)
{
if (!args)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed post action: Rule already reduced.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed post action: Collect already used.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg1;
arg1 = args->getData();
if (!(args = args->Right()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed post action: Two args required.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Iarg *arg2;
arg2 = args->getData();

if (args->Right())
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed post action: More than two args given.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

long pos = 0;	// Elt number.
_TCHAR *flagstr = 0;	// True or false.
switch (arg1->getType())
	{
	case IANUM:
		pos = arg1->getNum();
		break;
	case IASTR:
		// Convert string to number.
		if (!str_to_long(arg1->getStr(), pos))
			return false;
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Setunsealed post action: Arg1 must be numeric.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	}

switch (arg2->getType())
	{
	case IASTR:
		flagstr = arg2->getStr();
		break;
	case IANUM:
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Setunsealed post action: Arg2 must be string.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		}

		return false;
	}

// Check self-consistency.
if (!pos)
	return false;

bool flag;
if (!strcmp_i(_T("true"), flagstr))
	flag = true;
else if (!strcmp_i(_T("false"), flagstr))
	flag = false;
else
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed action: arg 2 must be \"true\"/\"false\".]")
			  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// These numbers represent a range of rule elts in the collect.
// Because of optional elements, some collects may be
// empty.  So gather the start and end nodes by traversing all the
// collects in the range.
Node<Pn> *colls;
Node<Pn> *node1, *node2;
colls = nlppp->collect_->getRoot();
while (--pos > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
node1 = colls->Down();	// Try to get start of range.
node2 = colls->eDown();	// Try to get end of range.

if (!node1 || node1 != node2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Setunsealed action: Bad collect.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Pn *pn = node1->getData();
pn->setUnsealed(flag);			// Set unsealed bit-flag appropriately.
return true;
}


/********************************************
* FN:		LAYERING
* CR:		11/07/98 AM.
* SUBJ:	Perform additional reductions (layering) for given node.
* RET:	Reduced node, else given node.
********************************************/

Node<Pn> *Pat::Layering(
	Node<Pn> *node,
	Dlist<Iarg> *attrs,
	Parse *parse
	)
{
if (!node || !attrs)
	return node;

//if (Debug())
//	*gout << "   [Layering]" << std::endl;

// Move node's semantics up to top node in layering chain.
Node<Pn> *tmp;				// The new subtree.
Pn *pn;
Sem *sem;
pn = node->getData();
sem = pn->getSem();
pn->setSem(0);
long line = pn->line_;											// FIX.	// 07/30/03 AM.
long ruleline = pn->ruleline_;	// Associated code line.		// 01/04/02 AM.

bool uns = pn->getUnsealed();		// For moving up chain.			// 10/11/99 AM.

// For each attr, make a reduction.
_TCHAR *name;
Delt<Iarg> *attr;
Iarg *arg;
for (attr = attrs->getFirst(); attr; attr = attr->Right())
	{
	tmp = new Node<Pn>(*node);					// COPY NODE. (Copy constructor).
	tmp->setUp(0);
	tmp->setDown(0);
	tmp->setLeft(0);
	tmp->setRight(0);
	tmp->seteDown(0);
	pn = tmp->getData();
	pn->setBase(false);		// Layers do not get the BASE flag.	// 11/10/98 AM.
	pn->setUnsealed(uns);	// Layers get UNSEALED flag.			// 10/11/99 AM.
	pn->setFired(true);		// Layer nodes are marked.				// 05/21/01 DD.
	pn->setBuilt(true);														// 05/25/01 AM.
	pn->line_ = line;												// FIX.	// 07/30/03 AM.
	pn->passnum_ = parse->currpass_;	// Assoc code pass.			// 01/04/02 AM.
	pn->ruleline_ = ruleline;			// Assoc code line.			// 01/04/02 AM.
	arg = attr->getData();
	name = arg->getStr();
	pn->setName(name);
	Node<Pn>::linkNodes(tmp, node);
	node = tmp;			// New top node in chain.
	}

// 03/24/99 AM. Seems to me the sem needs to be reattached.
pn = node->getData();			// 03/24/99 AM.
pn->setSem(sem);					// 03/24/99 AM.

return node;
}


/********************************************
* FN:		LAYERING
* CR:		03/24/99 AM.
* SUBJ:	Convert attrs list to a subtree of nodes.
* RET:	UP top,bottom = top and bottom nodes of chain.
* NOTE:	This is a variant for layering in a rule phrase, rather
*			than suggested element.
********************************************/

void Pat::Layering(
	Node<Pn> *node,			// Current node, for reference.
	Dlist<Iarg> *attrs,
	Ielt *elt,	// Element data.											// 07/24/03 AM.
	Parse *parse,
	/*UP*/
	Node<Pn>* &top,
	Node<Pn>* &bottom
	)
{
top = bottom = 0;
if (!attrs  || !node)
	return;

//if (Debug())
//	*gout << "   [Layering (for phrase elts)]" << std::endl;

// For each attr, make a reduction.
Delt<Iarg> *attr;

// Do first attr separately.
Pn *pn;
long start, end;
long ustart, uend;	// [UNICODE]
_TCHAR *text;
pn		= node->getData();

long line = pn->line_;	// Line number for new node.				// 05/17/01 AM.
//if (elt && elt->line_)		// Use elt line if available.			// 07/24/03 AM.
//	line = elt->line_;														// 07/24/03 AM.
//if (!line && pn->ruleline_)												// 07/24/03 AM.
//	line = pn->ruleline_;													// 07/24/03 AM.

start = pn->getStart();
end   = pn->getEnd();
ustart = pn->getUstart();	// [UNICODE]
uend = pn->getUend();	// [UNICODE]
text	= pn->getText();
Node<Pn> *restart = pn->restart_;		// RECOPT3		// FIX	// 08/16/08 AM.

attr = attrs->getFirst();
_TCHAR *str = attr->getData()->getStr();									// 05/01/03 AM.

// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.

node = Pn::makeNode(start,end,ustart,uend,typ,text,	// [UNICODE]
				str,		// Name node using attr.
				0,										// Get sym from where?
				line,																// 05/17/01 AM.
				parse->currpass_,		// Assoc code pass				// 01/04/02 AM.
				line						// Assoc code line				// 07/24/02 AM.
				);
Pn *pntmp = node->getData();												// 05/21/01 AM.
pntmp->setFired(true);														// 05/21/01 AM.
pntmp->setBuilt(true);														// 05/25/01 AM.
pntmp->restart_ = restart;					// RECOPT3		// FIx	// 08/16/08 AM.
// Other node values set to default.

Node<Pn> *tmp;
bottom = node;
for (attr = attr->Right(); attr; attr = attr->Right())
	{
	str = attr->getData()->getStr();										// 05/01/03 AM.
	// Allowing retokenized alphas, etc.								// 05/01/03 AM.
	typ = nodetype(str);														// 05/01/03 AM.

	tmp = Pn::makeNode(start,end,ustart,uend,typ,text,	// [UNICODE]
				str,		// Name node using attr.						// 05/01/03 AM.
				0,										// Get sym from where?
				line,																// 05/17/01 AM.
				parse->currpass_,		// Assoc code pass				// 01/04/02 AM.
				line						// Assoc code line				// 07/24/02 AM.
				);
	pntmp = tmp->getData();													// 05/21/01 AM.
	pntmp->setFired(true);													// 05/21/01 AM.
	pntmp->setBuilt(true);													// 05/25/01 AM.
	pntmp->restart_ = restart;				// RECOPT3		// FIx	// 08/16/08 AM.
	// Other node values set to default.
	Node<Pn>::linkNodes(tmp, node);
	node = tmp;			// New top node in chain.
	}

top = node;
}


/********************************************
* FN:		RECURSE
* CR:		11/08/98 AM.
* SUBJ:	Perform rule matching for given collect node and ruleset.
* RET:	True if ok, else false.
* NOTE:	Recursive invocation of pattern matching machinery.
*			The rules invoked could replace or remove all the nodes covered
*			by the collect.  They could replace first element in a parse tree
*			list, and they could even replace the parse tree root.  Handle all
*			these cases.
*			11/09/98 AM. first and last must be updated to reflect the changes
*			to the parse tree.
********************************************/

bool Pat::Recurse(
	Node<Pn> *pcoll,			// Collect covering nodes to pattern match on.
	Dlist<Irule> *ruleset,	// Rules to use in pattern matching.
	Parse    *parse,			// Current parse instance.
	/*DU*/
	Node<Pn>* &first,			// First node matched by rule.
	Node<Pn>* &last			// Last node matched by rule.
	)
{
//if (Debug())
//	*gout << "   [IN RECURSE]" << std::endl;

if (!pcoll || !ruleset || !parse)
	return false;

bool upfirst, uplast;		// If need to update first, last.
Delt<Irule> *rules;
rules = ruleset->getFirst();

Node<Pn> *start, *end;
start = pcoll->Down();
end   = pcoll->eDown();
if (!start)
//	return false;				// 11/26/98 AM.
	return true;				// Ok, because elt must be optional. // 11/26/98 AM.

// 11/09/98 AM.
upfirst = (first == start);
uplast  = (last  == end);

// Because executing these rules can wipe out the nodes of the collect,
// we need to retain information for patching up the collect.
Node<Pn> *before, *after;
after = 0;
Node<Pn> *parent;
parent = 0;			// Don't set till needed.

if (end)
	after = end->Right();

if ((before = start->Left()))			// Normal case in parse tree.
	;
else
	parent = start->Up();
// Note: if parent, then start is first node in a list of nodes.
// Else, start is the root of the parse tree.

// DO THE EMBEDDED RULE MATCHING.
matchNodes(start, end, rules, parse, 0);

// Getting the final node in phrase when needed.	// FIX	// 12/18/99 AM.
Node<Pn> *fin = 0;											// FIX	// 12/18/99 AM.

// Now reconfigure the collect appropriately.
// Man, this is ugly.
if (before)
	{
	if (after)
		{
		if (before->Right() == after)	// Collect's nodes are wiped out.
			{
			pcoll->setDown(0);
			pcoll->seteDown(0);
			}
		else
			{
			pcoll->setDown(before->Right());
			pcoll->seteDown(after->Left());
			}
		}
	else
		{
		pcoll->setDown(before->Right());
		// Get final node in phrase.			// FIX.				// 12/18/99 AM.
		if (before->Right())						// FIX.				// 12/18/99 AM.
			fin = before->Right()->last();	// FIX.				// 12/18/99 AM.
		pcoll->seteDown(fin);	// List of nodes goes to the end.	// 12/18/99 AM.
		}
	}
else if (parent)
	{
	pcoll->setDown(parent->Down());
	if (after)
		pcoll->seteDown(after->Left());
	else
		{
		// Get final node in phrase.			// FIX.				// 12/18/99 AM.
		if (parent->Down())						// FIX.				// 12/18/99 AM.
			fin = parent->Down()->last();		// FIX.				// 12/18/99 AM.
		pcoll->seteDown(fin);					// FIX.				// 12/18/99 AM.
		}
	}
else // start was root of tree.
	{
	pcoll->setDown(((Tree<Pn> *)parse->getTree())->getRoot());
	if (after)
		pcoll->seteDown(after->Left());
	else
		{
		// Get final node in phrase.			// FIX.				// 12/18/99 AM.
		if (pcoll->Down())						// FIX.				// 12/18/99 AM.
			fin = pcoll->Down()->last();		// FIX.				// 12/18/99 AM.
		pcoll->seteDown(fin);					// FIX.				// 12/18/99 AM.
		}
	}

// 11/09/98 AM.
if (upfirst)
	first = pcoll->Down();
if (uplast)
	last  = pcoll->eDown();

return true;
}


/********************************************
* FN:		RECURSE
* CR:		05/31/00 AM.
* SUBJ:	Perform recurse minipass on matched element of a rule.
* RET:	True if ok, else false.
* NOTE:	VARIANT.  For compiled runtime analyzer.
*			06/01/00 AM. Because recurse is its own "mini analyzer", must
*			give it its own NLPPP & COLLECT objects.
********************************************/

bool Pat::Recurse(
	COLL *pcoll,			// Collect covering nodes to pattern match on.
	bool (rules)(int,bool&,Nlppp*),	// Rule fn to use in pattern matching.
	Parse    *parse,			// Current parse instance.
//	const PASS *pass,
	/*DU*/
	Node<Pn>* &first,			// First node matched by rule.
	Node<Pn>* &last			// Last node matched by rule.
	)
{
//if (Debug())
//	*gout << "   [IN RECURSE]" << std::endl;

if (!pcoll || !parse)
	return false;

bool upfirst, uplast;		// If need to update first, last.

Node<Pn> *start, *end;
start = (Node<Pn> *)pcoll->start;
end   = (Node<Pn> *)pcoll->end;
if (!start)
	return true;				// Ok, because elt must be optional.

upfirst = (first == start);
uplast  = (last  == end);

// Because executing these rules can wipe out the nodes of the collect,
// we need to retain information for patching up the collect.
Node<Pn> *before, *after;
after = 0;
Node<Pn> *parent;
parent = 0;			// Don't set till needed.

if (end)
	after = end->Right();

if ((before = start->Left()))			// Normal case in parse tree.
	;
else
	parent = start->Up();
// Note: if parent, then start is first node in a list of nodes.
// Else, start is the root of the parse tree.

// SET UP MINIPASS.															// 06/01/00 AM.
// NOTE:	KEEP THE SAME parse OBJECT.  Holds global vars, input file, other
// shared data.  Basically, still the same text parse.
COLL *savecoll = parse->getCollect();									// 06/01/00 AM.
Nlppp *savenlppp = parse->getNlppp();									// 06/01/00 AM.
Arun::init_analyzer(parse);	// CREATES Nlppp & COLL.			// 06/01/00 AM.

// DO THE EMBEDDED RULE MATCHING.
matchNodes(start, end, rules, parse, 0);
//matchNodes(start,end,parse,pass,0);

// CLEAN UP MINIPASS.														// 06/01/00 AM.
delete parse->getCollect();												// 06/01/00 AM.
delete parse->getNlppp();													// 06/01/00 AM.
parse->setCollect(savecoll);												// 06/01/00 AM.
parse->setNlppp(savenlppp);												// 06/01/00 AM.
parse->nlppp_->parse_ = parse;				// Backpointer.		// 06/01/00 AM.
parse->nlppp_->coll_ = parse->collect_;	// Backpointer.		// 06/01/00 AM.

// Getting the final node in phrase when needed.
Node<Pn> *fin = 0;

// Now reconfigure the collect appropriately.
// Man, this is ugly.
if (before)
	{
	if (after)
		{
		if (before->Right() == after)	// Collect's nodes are wiped out.
			{
			pcoll->start = 0;
			pcoll->end = 0;
			}
		else
			{
			pcoll->start = before->Right();
			pcoll->end = after->Left();
			}
		}
	else
		{
		pcoll->start = before->Right();
		// Get final node in phrase.
		if (before->Right())
			fin = before->Right()->last();
		pcoll->end = fin;	// List of nodes goes to the end.
		}
	}
else if (parent)
	{
	pcoll->start = parent->Down();
	if (after)
		pcoll->end = after->Left();
	else
		{
		// Get final node in phrase.
		if (parent->Down())
			fin = parent->Down()->last();
		pcoll->end = fin;
		}
	}
else // start was root of tree.
	{
	pcoll->start = ((Tree<Pn> *)parse->getTree())->getRoot();
	if (after)
		pcoll->end = after->Left();
	else
		{
		// Get final node in phrase.
		if (pcoll->start)
			fin = ((Node<Pn> *)pcoll->start)->last();
		pcoll->end = fin;
		}
	}

if (upfirst)
	first = (Node<Pn> *)pcoll->start;
if (uplast)
	last  = (Node<Pn> *)pcoll->end;

return true;
}




/********************************************
* FN:		EXECPHRASE
* CR:		11/10/98 AM.
* SUBJ:	Traverse the rule's phrase, performing element actions.
* RET:	True if ok, else false.
* NOTE:	Separating this function from redSingle.
********************************************/

bool Pat::execPhrase(
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[execPhrase: Null first or last node in range.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[execPhrase: Rule already reduced.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[execPhrase: Collect already used.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

Node<Pn> *nodes;
if (!nlppp->collect_ || !(nodes = nlppp->collect_->getRoot()))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[execPhrase: No collected nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Walk the collected nodes in tandem with the parse tree nodes.
// Fix up the parse nodes as directed by the collect elts.
// (Each collect elt represents a rule elt and a range of parse nodes.)
Node<Pn> *pcoll;
for (pcoll = nodes; pcoll; pcoll = pcoll->Right())
	{
	// Get rule elt.
	Pn *data;
	data = pcoll->getData();
	Delt<Ielt> *ielt;
	ielt = data->getRulelt();
	if (!ielt)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[execPhrase: Collect has no rule element.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);

		// CLEAN UP.
		return false;
		}

	Ielt *elt;
	elt = ielt->getData();
	_TCHAR *ren;
	ren = elt->getRename();
	_TCHAR *tgp = elt->getGroup();											// 09/23/00 AM.
	_TCHAR *gp=0;	// 07/24/07 AM.
	if (tgp && *tgp)	// 07/24/07 AM.
		nlppp->parse_->internStr(tgp, /*UP*/gp);	// Intern str.	// 07/24/07 AM.
	else	// 07/24/07 AM.
		;	// DEBUGGING!!!	// 07/24/07 AM.

//	if (ren && Debug())
//		*gout << "   rename=" << str(ren) << std::endl;

	// 11/09/98 AM. Upgrading to execute a list of passes,
	// each with a ruleset. (Seqn objects contain only ruleset now.)
	// Get recursive rule invocations.		// 11/08/98 AM.
	Dlist<Seqn> *passes;							// 11/09/98 AM.
	Delt<Seqn> *dpass;							// 11/09/98 AM.
	Seqn *pass;										// 11/09/98 AM.
	Dlist<Irule> *ruleset;
	if ((passes = elt->getPasses()))
		{
//		if (Debug())
//			*gout << "   ruleset present" << std::endl;
		for (dpass = passes->getFirst(); dpass; dpass = dpass->Right())
			{
			pass    = dpass->getData();
			ruleset = pass->getRules();
			// RECURSIVELY INVOKE THE RULESET ON THIS COLLECT'S NODES.
			// Patch up collect's pointers, if need be.
			Recurse(pcoll, ruleset, nlppp->parse_,
						/*DU*/ nlppp->first_, nlppp->last_);
			}
		}

	// PUTTING ELT'S GROUP ACTION HERE.									// 09/23/00 AM.
	// Will rejigger the collect.
	Node<Pn> *pc = groupElt(pcoll, gp, nlppp);						// 09/23/00 AM.
	if (pc)																		// 09/23/00 AM.
		pcoll = pc;	// New collect elt at same position.			// 09/23/00 AM.

	Node<Pn> *node;
	Node<Pn> *bound, *tmp;
	if ((tmp = pcoll->eDown()))
		bound = tmp->Right();
	else
		bound = 0;
	for (node = pcoll->Down();		// For each node matched by rule elt.
		  node && node != bound;
		  node = node->Right() )
		{
		Pn *pn;
		pn = node->getData();

		// Fix up renames as directed.
		if (ren)
			pn->setName(ren);

		// Parse nodes should get ord of rule-elt.
		// Fix up subtrees of parse nodes if directed (in a '<' match mode).
		//		Excise subtree and delete remaining subtrees from parse node.
		//		Reattach subtree to parse node.

		}

	// 03/23/99 AM. PERFORM ELEMENT LAYERING HERE.
	// May need to rejigger the parse tree and the collect.
	Dlist<Iarg>* attrs = 0;
	if ((attrs = elt->getAttrs()))
		{
//		if (Debug())
//			*gout << "   layering elt" << std::endl;
		layerElt(pcoll, attrs, elt, nlppp->parse_, /*DU*/ nlppp->first_, nlppp->last_);
		}
	}

// Clean up.
return true;
}


/********************************************
* FN:		LAYERELT
* CR:		03/24/99 AM.
* SUBJ:	Layer attrs in parse tree for nodes matching current rule elt.
* RET:	True if ok, else false.
* NOTE:	Copying and modifying Recurse function.  I'm glad it was here
*			to show how to mangle the parse tree!
*			Don't need the full treatment for first and last.  Merely need
*			to get the new first and new last.
********************************************/

bool Pat::layerElt(
	Node<Pn> *pcoll,			// Collect covering nodes of current elt.
	Dlist<Iarg> *attrs,		// Attributes to layer on nodes of current elt.
	Ielt *elt,					// Rule element data.	// 07/24/03 AM.
	Parse    *parse,			// Current parse instance.
	/*DU*/
	Node<Pn>* &first,			// First node matched by rule.
	Node<Pn>* &last			// Last node matched by rule.
	)
{
//if (Debug())
//	*gout << "   [IN LAYERELT]" << std::endl;

if (!pcoll || !attrs || !parse)
	return false;

bool upfirst, uplast;		// If need to update first, last.
//Delt<Irule> *rules;
//rules = ruleset->getFirst();

Node<Pn> *start, *end;
start = pcoll->Down();
end   = pcoll->eDown();
if (!start)
//	return false;				// 11/26/98 AM.
	return true;				// Ok, because elt must be optional. // 11/26/98 AM.

// 11/09/98 AM.
upfirst = (first == start);
uplast  = (last  == end);

// 03/24/99 AM.
Node<Pn> *newfirst = 0;
Node<Pn> *newlast = 0;

// Because executing these rules can wipe out the nodes of the collect,
// we need to retain information for patching up the collect.
Node<Pn> *before, *after;
after = 0;
Node<Pn> *parent;
parent = 0;			// Don't set till needed.

if (end)
	after = end->Right();

if ((before = start->Left()))			// Normal case in parse tree.
	;
else
	parent = start->Up();
// Note: if parent, then start is first node in a list of nodes.
// Else, start is the root of the parse tree.

//// DO LAYERING HERE.

Tree<Pn> *tree = (Tree<Pn> *) parse->getTree();

// For each node matching current element:
Node<Pn> *node;
Node<Pn> *next;	// Save next node before current is mangled.
Node<Pn> *bottom, *top;
top = bottom = 0;
node = start;
while (node && node != after)
	{
	next = node->Right();			// Save this first!

	// Build layered nodes, pointing to top and bottom.
	Layering(node,attrs,elt,parse,/*UP*/top,bottom); // Variant builds all nodes.

	if (!top || !bottom)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[layerElt: Error.]") << std::ends;
		parse->errOut(&gerrStr,false);

		return false;				// Something went wrong.
		}

	// Excise node from tree, replacing it with TOP.
	tree->replaceSubs(node, node, top, top);

	// Attach excised node to BOTTOM.
	Node<Pn>::linkNodes(bottom, node);

	// Move node's semantics up to top node in layering chain.
	Pn *pn;
	Sem *sem;
	pn = node->getData();
	sem = pn->getSem();
	pn->setSem(0);
	top->getData()->setSem(sem);
	// NOT MOVING DYNAMIC SEMANTICS UP, BY DEFAULT.			// 11/17/99 AM.

	// 03/24/99 AM.
	if (!newfirst)
		newfirst = top;

	node = next;			// Move to next node for current elt.
	}

newlast = top;

if (!newfirst || !newlast)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[layerElt: Error.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

//// END OF LAYERING

// 03/24/99 AM. Simpler than recurse.
pcoll->setDown(newfirst);
pcoll->seteDown(newlast);

// Update first and last pointers, when needed.
if (upfirst)
	first = newfirst;
if (uplast)
	last  = newlast;

return true;
}


/********************************************
* FN:		POSTINC
* CR:		12/08/98 AM.
* SUBJ:	Increment a variable.
* RET:	True if ok, else false.
* FORM:	inc(VAR)
********************************************/

bool Pat::postInc(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;						// Name of variable.

if (!Arg::str1(_T("postInc"), (DELTS*&)args, var))
	return false;
if (!Arg::done(args,_T("postInc"),nlppp->parse_))
	return false;

// Fetch and increment the variable.
Var::inc(var, nlppp->parse_);

return true;
}


/********************************************
* FN:		POSTADDSTRS
* CR:		12/08/98 AM.
* SUBJ:	Add a string value to a multi-string variable.
* RET:	True if ok, else false.
* FORM:	addstrs(VARNAME, NUM)
*			   where VARNAME - name of strings variable to add to.
*				      NUM - the element number to get string from.
********************************************/

bool Pat::postAddstrs(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;						// Name of variable.
long long ord = 0;						// Rule element.

static bool bpostAddstrs = false;
if (!bpostAddstrs)															// 01/04/02 AM.
	{
	std::_t_strstream gerrStr;						// 01/04/02 AM.
	gerrStr << _T("[postAddstrs: Obsolete function.]") << std::ends;		// 01/04/02 AM.
	bpostAddstrs = true;														// 01/04/02 AM.
	}

if (!Arg::str1(_T("postAddstrs"), (DELTS*&)args, var))
	return false;
if (!Arg::num1(_T("postAddstrs"), (DELTS*&)args, ord))
	return false;
if (!Arg::done(args, _T("postAddstrs"),nlppp->parse_))
	return false;

// Get node from collect.
// 03/05/99 AM. Implementing multi-node match.  (For <base> tag).
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || !nstart
	 || !nend		// 03/05/99 AM.
	 )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postAddstrs: failed.]") << std::ends;
	return nlppp->parse_->errOut(&gerrStr,false);
	}

// 03/05/99 AM. Simpler to just get the text buffer offsets
// and the string from that.

// Fetch node's string.
Node<Pn> *base;
Pn *pn;
_TCHAR *str;

if (nstart == nend)				// 03/05/99 AM.
	{
	base = baseNode(nstart);	// Get bottommost node.
	pn = base->getData();
	str = pn->getName();
	if (!str || !*str || (*str == '_'))
		return true;		// Not a literal. Don't add it.
	}
else	// Build and internalize string for range of nodes.
	str = rangeText(nstart, nend, nlppp->parse_);	// 03/05/99 AM.

// Add string value to the variable.
Var::addStrs(var, str, nlppp->parse_);

return true;
}


/********************************************
* FN:		POSTXADDLEN
* CR:		08/05/99 AM.
* SUBJ:	Add node length to context node's variable.
* RET:	True if ok, else false.
* FORM:	xaddlen(VARNAME, NUM)
*			   where VARNAME - name of context node's variable to add to.
*				      NUM - the element number to get length from.
* NOTE:	Length will be for stuff covered by node offsets.  If rule elt
*			matched more than one node, should add those lengths.  (DON'T
*			assume contguity).
********************************************/

bool Pat::postXaddlen(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;						// Name of variable.
long long ord = 0;						// Rule element.

if (!Arg::str1(_T("postXaddlen"), (DELTS*&)args, var))
	return false;
if (!Arg::num1(_T("postXaddlen"), (DELTS*&)args, ord))
	return false;
if (!Arg::done(args, _T("postXaddlen"),nlppp->parse_))
	return false;

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

// Get node from collect.
// 03/05/99 AM. Implementing multi-node match.  (For <base> tag).
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || !nstart
	 || !nend		// 03/05/99 AM.
	 )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postXaddlen: failed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// 03/05/99 AM. Simpler to just get the text buffer offsets
// and the string from that.

// Fetch node's string.
Node<Pn> *base;
Pn *pn;
long len = 0;

if (nstart == nend)				// 03/05/99 AM.
	{
	base = baseNode(nstart);	// Get bottommost node.
	pn = base->getData();
	len = pn->getEnd() - pn->getStart() + 1;
	}
else	// Get length for range of nodes.
	{
	// ASSUMES THEY ARE CONTIGUOUS.
	Pn *pns, *pne;
	pns = nstart->getData();
	pne = nend->getData();
	len = pne->getEnd() - pns->getStart() + 1;
	}

// Add length to the variable.
Var::addNum(var, len, nlppp->parse_, /*DU*/ dlist);
pnsel->setDsem(dlist);

return true;
}


/********************************************
* FN:		POSTXINC
* CR:		08/06/99 AM.
* SUBJ:	Increment context node's variable.
* RET:	True if ok, else false.
* FORM:	xinc(VARNAME)
*			   where VARNAME - name of context node's variable to increment.
********************************************/

bool Pat::postXinc(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *var = 0;						// Name of variable.

if (!Arg::str1(_T("postXinc"), (DELTS*&)args, var))
	return false;
if (!Arg::done(args, _T("postXinc"),nlppp->parse_))
	return false;

if (!nlppp->select_)
	return false;
Dlist<Ipair> *dlist;
Pn *pnsel;
pnsel = nlppp->select_->getData();
dlist = pnsel->getDsem();

// Increment variable.
// If variable has no value, increment to 1.
Var::addNum(var, 1, nlppp->parse_, /*DU*/ dlist);
pnsel->setDsem(dlist);

return true;
}


/********************************************
* FN:		POSTXADDNVAR
* CR:		09/23/99 AM.
* SUBJ:	Add node's variable to context node's variable.
* RET:	True if ok, else false.
* FORM:	XaddNvar(NUM, NVAR, XVAR)
*				where NUM - rule elt to get variable value from.
*					NVAR - name of node's variable (at rule elt)
*					XVAR - context (dominant) node's variable name
********************************************/

bool Pat::postXaddNvar(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
long long ord = 0;						// Rule element.
_TCHAR *nvar = 0;					// Node var.
_TCHAR *xvar = 0;					// Context node var.

if (!Arg::num1(_T("postXaddNvar"), (DELTS*&)args, ord))
	return false;
if (!Arg::str1(_T("postXaddNvar"), (DELTS*&)args, nvar))
	return false;
if (!Arg::str1(_T("postXaddNvar"), (DELTS*&)args, xvar))
	return false;
if (!Arg::done(args, _T("postXaddNvar"),nlppp->parse_))
	return false;

if (!nlppp->select_)					// Context node.
	return false;
Dlist<Ipair> *xdlist;
Pn *xpn;
xpn = nlppp->select_->getData();
xdlist = xpn->getDsem();	// Context node's variable list.

// Get node from collect.
// 03/05/99 AM. Implementing multi-node match.  (For <base> tag).
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || !nstart
	 || !nend		// 03/05/99 AM.
	 )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postXaddNvar: failed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nstart != nend)
	return true;	// If rule elt matched multiple nodes, ignore action.

// Get node variable and value.
Dlist<Ipair> *ndlist;
Pn *npn;
npn = nstart->getData();
ndlist = npn->getDsem();

long long nval = 0;
Var::val(nvar, ndlist, /*DU*/ nval);

// Add node's var to the variable.
Var::addNum(xvar, nval, nlppp->parse_, /*DU*/ xdlist);
xpn->setDsem(xdlist);

return true;
}


/********************************************
* FN:		POSTXRENAME
* CR:		12/25/99 AM.
* SUBJ:	Rename a context node.
* RET:	True if ok, else false.
* FORM:	Xrename(NAME, [NUM])
*				where
*					NAME - new name for node.
*					[NUM] - numth context node. If zero or absent, last node.
********************************************/

bool Pat::postXrename(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
long long ord = 0;						// Context node ordinal.
_TCHAR *name = 0;					// New name for context node.
_TCHAR *xvar = 0;					// Context node var.

if (!Arg::str1(_T("xrename"), (DELTS*&)args, name))
	return false;

// Need better checking.	// 09/08/08 AM.
if (!name || !*name)			// 09/08/08 AM.
	return false;

if (args && !Arg::num1(_T("xrename"), (DELTS*&)args, ord, true))
	// WANT THE USER TO FIX SYNTAX ERROR.	// 09/08/08 AM.
	return false;		// 09/08/08 AM.
//	else if no 2nd arg, ord = 0;			// Allowing 2nd arg to be optional.

if (!Arg::done(args, _T("postXrename"),nlppp->parse_))
	return false;


// Get the ordth context node.
// ord == 0 means get from last component of context, ie,
// the immediately dominant node.
Node<Pn> *select, *tmp;
Pn *pn;
if (!(select = nlppp->select_))
	return false;
if (!ord)						// LAST component of path.
	pn = select->getData();
else								// Get component.
	{
	// Recurse to the root, then count down.
	tmp = select->pathNth(ord);
	if (!tmp)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Xrename: Couldn't find node X(") << ord
					<< _T(").]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	pn  = tmp->getData();
	}

// Rename it.
pn->setName(name);
// Silently tell where this node rename came from.					// 08/25/08 AM.
long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 08/25/08 AM.
pn->setPassnum(nlppp->parse_->currpass_);								// 08/25/08 AM.
pn->setRuleline(ruleline);													// 08/25/08 AM.

return true;
}

/********************************************
* FN:		POSTXSET
* CR:		10/31/99 AM.
* SUBJ:	Set context variable to a value.
* RET:	True if ok, else false.
* FORM:	Xset(XVAR1, VAL)
*			   XVAR1 - context variable name.
*				VAL - string value.
********************************************/

bool Pat::postXset(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *xvar = 0;						// Name of variable.
_TCHAR *val = 0;							// Value to set.

if (!Arg::str1(_T("postXset"), (DELTS*&)args, xvar))
	return false;
if (!Arg::str1(_T("postXset"), (DELTS*&)args, val))
	return false;
if (!Arg::done(args, _T("postXset"),nlppp->parse_))
	return false;


if (!nlppp->select_)					// Context node.
	return false;
Dlist<Ipair> *xdlist;
Pn *xpn;
xpn = nlppp->select_->getData();
xdlist = xpn->getDsem();	// Context node's variable list.

// Set context variable to value.
Var::setVal(xvar, val,1,0, /*DU*/ xdlist);
xpn->setDsem(xdlist);

return true;
}


/********************************************
* FN:		POSTXXSETIFZERO
* CR:		10/31/99 AM.
* SUBJ:	Set context variable if another context variable is zero.
* RET:	True if everything ok, else false.
* FORM:	XXsetifzero(XVAR1, VAL, XVAR2)
*				XVAR1 - context variable to set.
*				VAL - value to give XVAR1
*				XVAR2 - variable to check for zero value.
* NOTE:	If XVAR2 not defined, assumed to have zero value.
********************************************/

bool Pat::postXXsetifzero(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *xvar1 = 0;					// Context var to set.
_TCHAR *xval = 0;						// Value to set.
_TCHAR *xvar2 = 0;					// Context var to check.

if (!Arg::str1(_T("postXXsetifzero"), (DELTS*&)args, xvar1))
	return false;
if (!Arg::str1(_T("postXXsetifzero"), (DELTS*&)args, xval))
	return false;
if (!Arg::str1(_T("postXXsetifzero"), (DELTS*&)args, xvar2))
	return false;
if (!Arg::done(args, _T("postXXsetifzero"),nlppp->parse_))
	return false;

if (!nlppp->select_)					// Context node.
	return false;
Dlist<Ipair> *xdlist;
Pn *xpn;
xpn = nlppp->select_->getData();
xdlist = xpn->getDsem();	// Context node's variable list.


// Get xvar2 variable and value.
long long num = 0;
Var::val(xvar2, xdlist, /*DU*/ num);
if (num)					// Value exists and is nonzero.
	return true;		// Action succeeds anyway.

// Set context variable to value.
Var::setVal(xvar1, xval,1,0, /*DU*/ xdlist);
xpn->setDsem(xdlist);

return true;
}


/********************************************
* FN:		POSTXSETX
* CR:		11/03/99 AM.
* SUBJ:	Set context variable from another context variable.
* RET:	True if everything ok, else false.
* FORM:	XsetX(XVAR1, XVAR2)
*				XVAR1 - context variable to set.
*				XVAR2 - context variable to get value from.
* NOTE:	If XVAR2 not defined, then "undefine" XVAR1.
********************************************/

bool Pat::postXsetX(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *xvar1 = 0;					// Context var to set.
_TCHAR *xvar2 = 0;					// Context var to check.

if (!Arg::str1(_T("postXsetX"), (DELTS*&)args, xvar1))
	return false;
if (!Arg::str1(_T("postXsetX"), (DELTS*&)args, xvar2))
	return false;
if (!Arg::done(args, _T("postXsetX"),nlppp->parse_))
	return false;

if (!nlppp->select_)					// Context node.
	return false;
Dlist<Ipair> *xdlist;
Pn *xpn;
xpn = nlppp->select_->getData();
xdlist = xpn->getDsem();	// Context node's variable list.

// 11/03/99 AM. Wipe out the values of 1st var, copy in values
// from second var.

// Get vals from 2nd variable.
Dlist<Iarg> *vals = 0;
Var::vals(xvar2, xdlist, /*DU*/ vals);

// COPY values.
Dlist<Iarg> *nvals = new Dlist<Iarg>(*vals);	// Copy constructor.

// Set nlppp->first_ variable's values from 2nd.  (Wipes out vals of 1st).
Var::setVals(xvar1, nvals, /*DU*/ xdlist);
xpn->setDsem(xdlist);

return true;
}


/********************************************
* FN:		POSTNINC
* CR:		10/02/99 AM.
* SUBJ:	Increment node's variable.
* RET:	True if ok, else false.
* FORM:	Ninc(NUM, NVAR)
*				where NUM - rule elt to get variable value from.
*					NVAR - name of node's variable (at rule elt)
********************************************/

bool Pat::postNinc(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
long long ord = 0;						// Rule element.
_TCHAR *nvar = 0;					// Node var.

if (!Arg::num1(_T("postNinc"), (DELTS*&)args, ord))
	return false;
if (!Arg::str1(_T("postNinc"), (DELTS*&)args, nvar))
	return false;
if (!Arg::done(args, _T("postNinc"),nlppp->parse_))
	return false;

// Get node from collect.
// Implementing multi-node match.  (For <base> tag).
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || !nstart
	 || !nend
	 )
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postNinc: failed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nstart != nend)
	return true;	// If rule elt matched multiple nodes, ignore action.

// Get node variable and value.
Dlist<Ipair> *ndlist;
Pn *npn;
npn = nstart->getData();
ndlist = npn->getDsem();

// Increment variable.
Var::addNum(nvar, 1, nlppp->parse_, /*DU*/ ndlist);
npn->setDsem(ndlist);

return true;
}


/********************************************
* FN:		POSTPRINT
* CR:		01/30/99 AM.
* SUBJ:	Print a literal string.
* RET:	True if ok, else false.
********************************************/

bool Pat::postPrint(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *str = 0;						// The string.

if (!Arg::str1(_T("postPrint"), (DELTS*&)args, str))
	return false;
if (!Arg::done(args, _T("postPrint"),nlppp->parse_))
	return false;

// Print the sucker.  Always use std::flush or std::endl.
if (str && *str)
	std::_t_cout << str << std::flush;

return true;
}


/********************************************
* FN:		POSTPRINTR
* CR:		01/30/99 AM.
* SUBJ:	Print text for a range of matched rule elements.
* RET:	True if ok, else false.
* FORM:	printr(ord1, ord2)
* OPT:	Fetch of nodes from collect could be more efficient (see fns in postrfa).
*			Should make a collectRange() function.
********************************************/

bool Pat::postPrintr(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
long long ord1 = 0;						// 1st rule element.
long long ord2 = 0;						// Last rule element.

if (!Arg::num1(_T("postPrintr"), (DELTS*&)args, ord1))
	return false;
if (!Arg::num1(_T("postPrintr"), (DELTS*&)args, ord2))
	return false;
if (!Arg::done(args, _T("postPrintr"),nlppp->parse_))
	return false;

return printr(ord1, ord2, nlppp->collect_, nlppp->parse_, &std::_t_cout);
}


/********************************************
* FN:		POSTPRLIT
* CR:		02/09/99 AM.
* SUBJ:	Print a literal string to file.
* RET:	True if ok, else false.
********************************************/

bool Pat::postPrlit(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename (== variable name!)
_TCHAR *lit = 0;						// The string.

if (!Arg::str1(_T("postPrlit"), (DELTS*&)args, fname))
	return false;
if (!Arg::str1(_T("postPrlit"), (DELTS*&)args, lit))
	return false;
if (!Arg::done(args, _T("postPrlit"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prlit: Couldn't output to file.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prlit post action: file=") << fname << _T(" is closed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Output the string to the file.
if (lit && *lit)
	*ostr << lit << std::flush;

return true;
}


/********************************************
* FN:		POSTPRRANGE
* CR:		02/09/99 AM.
* SUBJ:	Print text for a range of matched rule elements, to a file.
* RET:	True if ok, else false.
* FORM:	prrange(filename, ord1, ord2)
* OPT:	Fetch of nodes from collect could be more efficient (see fns in postrfa).
*			Should make a collectRange() function.
********************************************/

bool Pat::postPrrange(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename (== variable name!)
long long ord1 = 0;						// 1st rule element.
long long ord2 = 0;						// Last rule element.

if (!Arg::str1(_T("postPrrange"), (DELTS*&)args, fname))
	return false;
if (!Arg::num1(_T("postPrrange"), (DELTS*&)args, ord1))
	return false;
if (!Arg::num1(_T("postPrrange"), (DELTS*&)args, ord2))
	return false;
if (!Arg::done(args, _T("postPrrange"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prrange post action: file=") << fname << _T(" is closed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

return printr(ord1, ord2, nlppp->collect_, nlppp->parse_, ostr);
}


/********************************************
* FN:		POSTPRCHILD
* CR:		03/17/99 AM.
* SUBJ:	Print text to a file, for the named child of nth node.
* RET:	True if ok, else false.
* FORM:	prchild(filename, ord, name)
********************************************/

bool Pat::postPrchild(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename (== variable name!)
long long ord = 0;						// 1st rule element.
_TCHAR *name;							// Named subnode to look for.

if (!Arg::str1(_T("postPrchild"), (DELTS*&)args, fname))
	return false;
if (!Arg::num1(_T("postPrchild"), (DELTS*&)args, ord))
	return false;
if (!Arg::str1(_T("postPrchild"), (DELTS*&)args, name))
	return false;
if (!Arg::done(args, _T("postPrchild"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prrange post action: file=") << fname << _T(" is closed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get the nth element from collect.
// If multiple nodes matched rule elt, error.
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || (nstart && !nend))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prchild: failed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!nstart)
	return true;			// Absent is ok.

// Get its subphrase.
Node<Pn> *phrase;
if (!(phrase = nstart->Down()))
	return true;			// Absent is ok.

// Look for name in subphrase.
Pn *pn;
for (; phrase; phrase = phrase->Right())
	{
	pn = phrase->getData();
	if (!_tcscmp(name, pn->getName()))
		break;
	}
if (!phrase)
	return true;		// Absent is ok.

// If found, get its phrase.
_TCHAR *str;
long len;
long n1, n2;
n1 = pn->getStart();
n2 = pn->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Get ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;

// Print the thing.
while (len--)
	*ostr << *str++;
*ostr << std::flush;

return true;
}


/********************************************
* FN:		POSTPRTREE
* CR:		03/22/99 AM.
* SUBJ:	Print text to a file, for the named subtree of nth node.
* RET:	True if ok, else false.
* FORM:	prtree(filename, ord, name)
* NOTE:	Stops at the first node that matches name.
********************************************/

bool Pat::postPrtree(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename (== variable name!)
long long ord = 0;						// 1st rule element.
_TCHAR *name;							// Named subnode to look for.

if (!Arg::str1(_T("postPrtree"), (DELTS*&)args, fname))
	return false;
if (!Arg::num1(_T("postPrtree"), (DELTS*&)args, ord))
	return false;
if (!Arg::str1(_T("postPrtree"), (DELTS*&)args, name))
	return false;
if (!Arg::done(args, _T("postPrtree"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prtree post action: file=") << fname << _T(" is closed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get the nth element from nlppp->collect_.
// If multiple nodes matched rule elt, error.
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || (nstart && !nend))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prtree: failed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!nstart)
	return true;			// Absent is ok.

// Traverse the nstart subree, looking for name.
Node<Pn> *tmp;
Pn *pn;
if (!(tmp = Pn::findTreenode(name, nstart, true)))
	return true;			// Not found is ok.
pn = tmp->getData();

// If found, get its phrase.
_TCHAR *str;
long len;
long n1, n2;
n1 = pn->getStart();
n2 = pn->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Get ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;

// Print the thing.
while (len--)
	*ostr << *str++;
*ostr << std::flush;

return true;
}


/********************************************
* FN:		POSTPRXTREE
* CR:		09/25/99 AM.
* SUBJ:	Print text to a file, for the named subtree of nth node.
* RET:	True if ok, else false.
* FORM:	prtree(filename, prestr, ord, name, poststr)
* NOTE:	Stops at the first node that matches name.
*			Prints the texts only if name is found in rule element.
********************************************/

bool Pat::postPrxtree(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename (== variable name!)
_TCHAR *prestr = 0;					// Literal to print before node str.
long long ord = 0;						// 1st rule element.
_TCHAR *name = 0;					// Named subnode to look for.
_TCHAR *poststr = 0;				// Literal to print after node str.

if (!Arg::str1(_T("postPrxtree"), (DELTS*&)args, fname))
	return false;
if (!Arg::str1(_T("postPrxtree"), (DELTS*&)args, prestr))
	return false;
if (!Arg::num1(_T("postPrxtree"), (DELTS*&)args, ord))
	return false;
if (!Arg::str1(_T("postPrxtree"), (DELTS*&)args, name))
	return false;
if (!Arg::str1(_T("postPrxtree"), (DELTS*&)args, poststr))
	return false;
if (!Arg::done(args, _T("postPrxtree"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prxtree post action: file=") << fname << _T(" is closed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get the nth element from collect.
// If multiple nodes matched rule elt, error.
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || (nstart && !nend))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Prxtree: failed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!nstart)
	return true;			// Absent is ok.

// Traverse the nstart subree, looking for name.
Node<Pn> *tmp;
Pn *pn;
if (!(tmp = Pn::findTreenode(name, nstart, true)))
	return true;			// Not found is ok.
pn = tmp->getData();

// If found, get its phrase.
_TCHAR *str;
long len;
long n1, n2;
n1 = pn->getStart();
n2 = pn->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Get ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;

if (!len)
	return true;			// If nothing to print.

// Print the thing.
*ostr << prestr;
while (len--)
	*ostr << *str++;
*ostr << poststr;
*ostr << std::flush;

return true;
}


/********************************************
* FN:		POSTFPRINTNVAR
* CR:		08/05/99 AM.
* SUBJ:	Print nth node's variable values to file.
* RET:	True if ok, else false.
* FORM:	fprintnvar(filename, variablename, ord)
* NOTE:	Not handling range of matched nodes yet.
********************************************/

bool Pat::postFprintnvar(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename
_TCHAR *name;							// Variable name.
long long ord = 0;						// 1st rule element.

if (!Arg::str1(_T("postFprintnvar"), (DELTS*&)args, fname))
	return false;
if (!Arg::str1(_T("postFprintnvar"), (DELTS*&)args, name))
	return false;
if (!Arg::num1(_T("postFprintnvar"), (DELTS*&)args, ord))
	return false;
if (!Arg::done(args, _T("postFprintnvar"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Fprintnvar post action: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get the nth element from collect.
// If multiple nodes matched rule elt, error.
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNth(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || (nstart && !nend))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Fprintnvar: Unimplemented multiple nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!nstart)
	return true;			// Absent is ok.

// Get variable, values, and print.
Pn *pn;
pn = nstart->getData();
bool ok;
Dlist<Iarg> *list;
ok = Dsem::vals(name, pn, nlppp->parse_, /*DU*/ list);
if (!ok)
	{
	*ostr << _T("   ");		// Default formatting.
	return true;
	}
Delt<Iarg> *darg;
Iarg *arg;				// Just handle first val, for now.
darg = list->getFirst();
if (!darg)
	{
	*ostr << _T("   ") << std::flush;		// Default formatting.
	return true;
	}
arg = darg->getData();

// Print the thing.
if (arg->getType() == IASTR)
	*ostr << arg->getStr() ;
else if (arg->getType() == IANUM)
	{
	// Some default formatting.
	long tmp;
	tmp = arg->getNum();
	if (tmp <= -10)											// 11/26/99 AM.
		;								// No fill.				// 11/26/99 AM.
	else if (tmp < 0)											// 11/26/99 AM.
		*ostr << _T(" ");
	else if (tmp < 10)
		*ostr << _T("  ");
	else if (tmp < 100)
		*ostr << _T(" ");
	*ostr << arg->getNum();
	}
else
	return true;
*ostr << std::flush;

return true;
}


/********************************************
* FN:		POSTFPRINTXVAR
* CR:		11/26/99 AM.
* SUBJ:	Print nth context variable's values to file.
* RET:	True if ok, else false.
* FORM:	fprintxvar(filename, variablename, ord-for-context-node)
********************************************/

bool Pat::postFprintxvar(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *fname = 0;					// The filename
_TCHAR *name;							// Variable name.
long long ord = 0;						// 1st rule element.

if (!Arg::str1(_T("postFprintxvar"), (DELTS*&)args, fname))
	return false;
if (!Arg::str1(_T("postFprintxvar"), (DELTS*&)args, name))
	return false;
if (!Arg::num1(_T("postFprintxvar"), (DELTS*&)args, ord))
	return false;
if (!Arg::done(args, _T("postFprintxvar"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Fprintxvar post action: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get variable from nth path component of context.
// ord == 0 means get from last component of context, ie,
// the immediately dominant node.
Node<Pn> *select, *tmp;
Pn *pn;
if (!(select = nlppp->select_))
	return false;
if (!ord)						// LAST component of path.
	pn = select->getData();
else								// Get component.
	{
	// Recurse to the root, then count down.
	tmp = select->pathNth(ord);
	if (!tmp)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Couldn't find ") << ord
					<< _T("th component of path.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	pn  = tmp->getData();
	}

// The rest is same as fprintnvar.
// Get variable values and print.
bool ok;
Dlist<Iarg> *list;
ok = Dsem::vals(name, pn, nlppp->parse_, /*DU*/ list);
if (!ok)
	{
	*ostr << _T("   ");		// Default formatting.
	return true;
	}
Delt<Iarg> *darg;
Iarg *arg;				// Just handle first val, for now.
darg = list->getFirst();
if (!darg)
	{
	*ostr << _T("   ") << std::flush;		// Default formatting.
	return true;
	}
arg = darg->getData();

// Print the thing.
if (arg->getType() == IASTR)
	*ostr << arg->getStr() ;
else if (arg->getType() == IANUM)
	{
	// Some default formatting.
	long tmp;
	tmp = arg->getNum();
	if (tmp <= -10)											// 11/26/99 AM.
		;								// No fill.				// 11/26/99 AM.
	else if (tmp < 0)											// 11/26/99 AM.
		*ostr << _T(" ");
	else if (tmp < 10)
		*ostr << _T("  ");
	else if (tmp < 100)
		*ostr << _T(" ");
	*ostr << arg->getNum();
	}
else
	return true;
*ostr << std::flush;

return true;
}


/********************************************
* FN:		POSTFPRINTGVAR
* CR:		12/03/99 AM.
* SUBJ:	Print global variable's values to file.
* RET:	True if ok, else false.
* FORM:	fprintgvar(filename, variablename)
********************************************/

bool Pat::postFprintgvar(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *fname = 0;					// The filename
_TCHAR *name;							// Variable name.

if (!Arg::str1(_T("postFprintgvar"), (DELTS*&)args, fname))
	return false;
if (!Arg::str1(_T("postFprintgvar"), (DELTS*&)args, name))
	return false;
if (!Arg::done(args, _T("postFprintgvar"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Fprintxvar post action: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get variable from global list.
Delt<Iarg> *list = 0;
if (!Var::vals(name, nlppp->parse_, /*UP*/ list))
	{
	*ostr << _T("   ");		// Default formatting.
	return true;
	}

// The rest is similar to fprintnvar.

Delt<Iarg> *darg;
Iarg *arg;				// Just handle first val, for now.
darg = list;
if (!darg)
	{
	*ostr << _T("   ") << std::flush;		// Default formatting.
	return true;
	}
arg = darg->getData();

// Print the thing.
if (arg->getType() == IASTR)
	*ostr << arg->getStr() ;
else if (arg->getType() == IANUM)
	{
	// Some default formatting.
	long tmp;
	tmp = arg->getNum();
	if (tmp <= -10)											// 11/26/99 AM.
		;								// No fill.				// 11/26/99 AM.
	else if (tmp < 0)											// 11/26/99 AM.
		*ostr << _T(" ");
	else if (tmp < 10)
		*ostr << _T("  ");
	else if (tmp < 100)
		*ostr << _T(" ");
	*ostr << arg->getNum();
	}
else
	return true;
*ostr << std::flush;

return true;
}


/********************************************
* FN:		NDUMP
* CR:		12/13/99 AM.
* SUBJ:	Print nth node's variables to given file.
* RET:	True if ok, else false.
* FORM:	ndump(filename, ord)
*			ndump(filename, 0)		Last node.
*			ndump(filename)			Last node.
* NOTE:	Not handling range of matched nodes yet.
********************************************/

bool Pat::postNdump(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *fname = 0;					// The filename
long long ord = 0;						// 1st rule element.

if (!Arg::str1(_T("Ndump"), (DELTS*&)args, fname))
	return false;
if (!Arg::num1(_T("Ndump"), (DELTS*&)args, ord) || !ord)
	{
	ord = 0;	// Get node of last phrase element.
	}
if (!Arg::done(args, _T("Ndump"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Ndump: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get the nth element from collect.
// If multiple nodes matched rule elt, error.
// Get string from node.
Node<Pn> *nstart, *nend;
nstart = nend = 0;
if (!Pat::collectNthnew(nlppp->collect_, ord, /*UP*/ nstart, nend)
	 || (nstart && !nend))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Ndump: Unimplemented multiple nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (!nstart)
	return true;			// Absent is ok.
if (nstart != nend)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Ndump: Not handling multiple nodes.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET EACH VARIABLE IN NODE AND PRINT IT!
//Dlist<Ipair> *vars = nlppp->parse_->getVars();	// These are globals!
Pn *pn = nstart->getData();

// DUMP THE NODE TEXT ALSO!									// 12/14/99 AM.
_TCHAR *buf = pn->pnStr();		// FIX.						// 02/14/00 AM.
*ostr << _T("$text=") << buf << std::endl;							// 12/14/99 AM.
Chars::destroy(buf);				// FIX.						// 02/14/00 AM.


Dlist<Ipair> *vars = pn->getDsem();
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}


/********************************************
* FN:		XDUMP
* CR:		12/13/99 AM.
* SUBJ:	Print context node's variables to given file.
* RET:	True if ok, else false.
* FORM:	xdump(filename, ord)		Nth context node.
*			xdump(filename, 0)		Last context node.
*			xdump(filename)			Last context node.
********************************************/

bool Pat::postXdump(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *fname = 0;					// The filename
long long ord = 0;						// 1st rule element.

if (!Arg::str1(_T("Xdump"), (DELTS*&)args, fname))
	return false;
if (!Arg::num1(_T("Xdump"), (DELTS*&)args, ord) || !ord)
	{
	ord = 0;	// Get node of last phrase element.
	}
if (!Arg::done(args, _T("Xdump"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Xdump: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET NTH CONTEXT NODE.
// This is a new style, absent in earlier X actions.
// Taken from IVAR.CPP functions.
Node<Pn> *select = 0;
Node<Pn> *tmp;
Pn *pn = 0;
if (!(select = nlppp->getSelect()))
	return false;
if (!ord)						// LAST component of path.
	pn = select->getData();
else								// Get component.
	{
	// Recurse to the root, then count down.
	tmp = select->pathNth(ord);
	if (!tmp)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Xdump: Couldn't find ") << ord
					<< _T("th component of path.]") << std::ends;
		nlppp->parse_->errOut(&gerrStr,false);
		return false;
		}
	pn  = tmp->getData();
	}

// GET EACH VARIABLE IN NODE AND PRINT IT!
//Dlist<Ipair> *vars = nlppp->parse_->getVars();	// These are globals!
Dlist<Ipair> *vars = pn->getDsem();
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}


/********************************************
* FN:		GDUMP
* CR:		12/13/99 AM.
* SUBJ:	Print global variables to given file.
* RET:	True if ok, else false.
* FORM:	gdump(filename)
* NOTE:	Filename must be open with fileout.
********************************************/

bool Pat::postGdump(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *fname = 0;					// The filename

if (!Arg::str1(_T("Gdump"), (DELTS*&)args, fname))
	return false;
if (!Arg::done(args, _T("Gdump"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Gdump: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET EACH VARIABLE IN NODE AND PRINT IT!
Dlist<Ipair> *vars = nlppp->parse_->getVars();	// These are globals!
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}


/********************************************
* FN:		SDUMP
* CR:		12/13/99 AM.
* SUBJ:	Print suggested node's variables to given file.
* RET:	True if ok, else false.
* FORM:	sdump(filename)
* NOTE:	Filename must be open with fileout.
********************************************/

bool Pat::postSdump(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *fname = 0;					// The filename

if (!Arg::str1(_T("Sdump"), (DELTS*&)args, fname))
	return false;
if (!Arg::done(args, _T("Sdump"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Sdump: file=") << fname << _T(" is closed.]")
			<< std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// GET EACH VARIABLE IN SUGGESTED NODE AND PRINT IT!
// (This variable list is being built for suggested node.)
Dlist<Ipair> *vars = nlppp->getDsem();		// Get vars for sugg.
if (!vars)
	return true;							// No vars is ok.
if (!Var::dump(vars, ostr))			// Dump to file.
	return false;
return true;
}

/********************************************
* FN:		PRINTR
* CR:		02/09/99 AM.
* SUBJ:	Print range of nodes to given stream.
* RET:	True if ok, else false.
* NOTE:	Convenience function.
********************************************/

bool Pat::printr(
	long ord1,
	long ord2,
	Tree<Pn> *collect,
	Parse *parse,
	std::_t_ostream *ostr
	)
{
_TCHAR *str = 0;
long len = 0;

// Get the text range.	// 03/09/99 AM.
if (!getr(ord1, ord2, collect, parse, /*DU*/ str, len))
	return false;

// Print the text range.
while (len--)
	*ostr << *str++;
*ostr << std::flush;
return true;
}


/********************************************
* FN:		PRINTR
* CR:		06/02/00 AM.
* SUBJ:	Utility for printing text for a range of nodes.
* NOTE:	VARIANT.
********************************************/

bool Pat::printr(
	Node<Pn> *node1,
	Node<Pn> *node2,
	std::_t_ostream *ostr,
	Nlppp *nlppp
	)
{
if (!ostr)
	return false;
if (!node1)
	return true;			// Empty range.

// For the range of nodes, get the text range.
Pn *pn1, *pn2;
long n1, n2;
pn1 = node1->getData();
pn2 = node2->getData();
n1 = pn1->getStart();
n2 = pn2->getEnd();
_TCHAR *txt;
txt = nlppp->parse_->getText();

// Returns ptr to input buffer and length of string.
_TCHAR *str = &(txt[n1]);
long len = n2 - n1 + 1;

//Doing the work of PRINTR here.
// Print the text range.
while (len--)
	*ostr << *str++;
*ostr << std::flush;
return true;
}


/********************************************
* FN:		GETR
* CR:		03/09/99 AM.
* SUBJ:	Get the string under range of nodes.
* RET:	True if ok, else false.
* NOTE:	Convenience function.
********************************************/

bool Pat::getr(
	long ord1,
	long ord2,
	Tree<Pn> *collect,
	Parse *parse,
	/*DU*/
	_TCHAR* &str,
	long &len
	)
{
str = 0;
len = 0;
// Get node from collect.
// If multiple nodes matched rule elt, unimplemented.
// Get string from node.
Node<Pn> *nstart1, *nend1;
nstart1 = nend1 = 0;
if (!Pat::collectNth(collect, ord1, /*UP*/ nstart1, nend1)
	 || (nstart1 && !nend1))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[getr: failed.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}
Node<Pn> *nstart2, *nend2;
nstart2 = nend2 = 0;
if (!Pat::collectNth(collect, ord2, /*UP*/ nstart2, nend2)
	 || (nstart2 && !nend2))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[getr: failed.(2)]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

if (!nstart1 && !nstart2)	// Everything optional.
	return true;
if (nstart1 && nstart2)		// Everything present.
	;
else if (nstart1)				// 2nd is optional
	nend2 = nend1;
else								// 1st is optional
	nstart1 = nstart2;

// For the range of nodes, get the text range.
Pn *pn1, *pn2;
long n1, n2;
pn1 = nstart1->getData();
pn2 = nend2->getData();
n1 = pn1->getStart();
n2 = pn2->getEnd();
_TCHAR *txt;
txt = parse->getText();

// Returns ptr to input buffer and length of string.
str = &(txt[n1]);
len = n2 - n1 + 1;
return true;
}



/********************************************
* FN:		POSTPRANCHOR
* CR:		03/05/99 AM.
* SUBJ:	Print HTML file anchor to a file.
* RET:	True if ok, else false.
* FORM:	pranchor(filename, ord1, ord2)
* NOTE:	Specialized for using the URL and BASE URL of current document,
*			when it is an HTML text.  (For now, using the BASE only.  Need
*			to get URL for current document also...)
********************************************/

bool Pat::postPranchor(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp				// 11/19/99 AM.
	)
{
_TCHAR *fname = 0;					// The filename (== variable name!)
long long ord1 = 0;						// 1st rule element.
long long ord2 = 0;						// Last rule element.

if (!Arg::str1(_T("postPranchor"), (DELTS*&)args, fname))
	return false;
if (!Arg::num1(_T("postPranchor"), (DELTS*&)args, ord1))
	return false;
if (!Arg::num1(_T("postPranchor"), (DELTS*&)args, ord2))
	return false;
if (!Arg::done(args, _T("postPranchor"),nlppp->parse_))
	return false;

// Get the filename variable.
// Get its value, which is an output stream.
std::_t_ostream *ostr = 0;
if (!Var::val(fname, nlppp->parse_, /*DU*/ ostr))
	return false;

if (ostr == 0)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Pranchor post action: file=") << fname << _T(" is closed.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

// Get the URL from range.
_TCHAR *strng = 0;
long len = 0;
_TCHAR url[MAXSTR];
getr(ord1, ord2, nlppp->collect_, nlppp->parse_, /*DU*/ strng, len);
if (len >= MAXSTR)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[postPranchor: URL string overflow.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
_tcsnccpy(url, strng, len);
url[len] = '\0';			// Terminate.

// Parse the URL.  Note: URL buffer gets mangled.
_TCHAR *service, *server, *dirs, *file, *port;
service=server=dirs=file=port=0;
#ifndef LINUX
//if (!parse_url(url, /*UP*/ service,server,dirs,file,port))
	return false;
#endif

if (service && server)			// Absolute URL.
	{
	_tcsnccpy(url, strng, len);			// Restore url string.
	url[len] = '\0';
	*ostr << url << std::flush;
	return true;
	}

// For relative URLS,....
// GET THE BASE VARIABLE VALUE, IF SET.
// (May want to snag it into a variable, so don't have to repeat work...)
_TCHAR *base = 0;
if (!Var::val(_T("Base"), nlppp->parse_, /*DU*/ base))
	return true;	// Input doc's problem: relative urls with no base.

_TCHAR burl[MAXSTR];
if (base != NULL) {
	_tcscpy(burl, base);		// For mangling.
} else {
	burl[0] = '\0';
}

// Parse the base URL.  Note: URL buffer gets mangled.
_TCHAR *bservice, *bserver, *bdirs, *bfile, *bport;
bservice=bserver=bdirs=bfile=bport=0;
#ifndef LINUX
//if (!parse_url(burl, /*UP*/ bservice,bserver,bdirs,bfile,bport))
	return false;
#endif

if (bservice && bserver)			// Base URL is absolute.
	{
	// Resolve the relative URL.
	if (!service)
		service = bservice;
	if (!server)
		server = bserver;
	if (!dirs)
		dirs = bdirs;
	if (!file)
		file = bfile;
	if (!port)
		port = bport;

	// Print out the full URL.
	if (dirs)
		*ostr << service << _T("://")
				<< server  << _T("/")
				<< dirs    << _T("/")
				<< str(file)
				<< std::flush;
	else
		*ostr << service << _T("://")
				<< server  << _T("/")
				<< str(file)
				<< std::flush;
	return true;
	}

// Base URL is relative.
// TODO: Get document URL here!
// (May want to print URL even if can't get the doc url.)

std::_t_strstream gerrStr;
gerrStr << _T("[Base URL is relative. Getting document URL unimplemented.]")
  << std::ends;
nlppp->parse_->errOut(&gerrStr,false);
return true;
}

/**************************************************************************
// NLP++ & KB INTERFACE FUNCTIONS
**************************************************************************/

/********************************************
* FN:		POSTADDATTR
* CR:		02/15/00 AM.
* SUBJ:	Add value to a kb concept's attribute.
* RET:	True if ok, else false.
* FORM:	addattr(conceptname, attributename, valuestr)
* NOTE:	KB assumed to have been read in.
*			BELONGS IN FN.CPP.
********************************************/

bool Pat::postAddattr(
	Delt<Iarg> *args,			// Argument list for the action.
	Nlppp *nlppp
	)
{
_TCHAR *s_conc = 0, *attr=0, *val=0;

if (!Arg::str1(_T("addattr"), (DELTS*&)args, s_conc))
	return false;
if (!Arg::str1(_T("addattr"), (DELTS*&)args, attr))
	return false;
if (!Arg::str1(_T("addattr"), (DELTS*&)args, val))
	return false;
if (!Arg::done(args, _T("addattr"),nlppp->parse_))
	return false;

// Need to get the current KB.
CG *cg = nlppp->getParse()->getAna()->getCG();

// Then get ticket concept.
CONCEPT *root = cg->findRoot();
CONCEPT *conc = cg->findConcept(root, _T("ticket"));

// Then just add the attribute.
cg->addVal(conc, attr, val);

return true;
}

/**************************************************************************/

/********************************************
* FN:		COLLECTNTH
* CR:		01/24/99 AM.
* SUBJ:	Get nodes for nth element of collect.
* RET:	True if ok, else false.
* NOTE:	Long overdue convenience function.
********************************************/

bool Pat::collectNth(
	Tree<Pn> *collect,
	long ord,
	/*UP*/
	Node<Pn>* &nstart,
	Node<Pn>* &nend
	)
{
if (!collect || ord <= 0)
	return false;

// ord is collect for nodes.
Node<Pn> *colls;
Node<Pn> *coll1;
colls = collect->getRoot();
while (--ord > 0)
	{
	if (!(colls = colls->Right()))
		return false;
	}
coll1 = colls;

// Get nodes from collect.
nstart = coll1->Down();
nend   = coll1->eDown();

if (!nstart && !nend)
	return true;				// Empty is ok.
if (!nstart || !nend)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[collectNth: Bad collect]") << std::ends;
	errOut(&gerrStr,false);
	return false;				// Not allowing this case.
	}
return true;				// Start and end both present.
}


/********************************************
* FN:		COLLECTNTHNEW
* CR:		12/03/99 AM.
* SUBJ:	Get nodes for nth element of collect. 0 means LAST.
* RET:	True if ok, else false.
* NOTE:	If ord == 0, get last collect, not error.
********************************************/

bool Pat::collectNthnew(
	Tree<Pn> *collect,
	long ord,
	/*UP*/
	Node<Pn>* &nstart,
	Node<Pn>* &nend
	)
{
nstart = nend = 0;
if (!collect || ord < 0)
	return false;
if (ord > 0)
	return collectNth(collect, ord, /*UP*/ nstart,nend);

// Handle the zero case here.  Get last collect element.

Node<Pn> *colls;
Node<Pn> *coll1;
colls = collect->getRoot();
if (!colls)
	return false;
while (colls->Right())
	colls = colls->Right();

coll1 = colls;

// Get nodes from collect.
nstart = coll1->Down();
nend   = coll1->eDown();

if (!nstart && !nend)
	return true;				// Empty is ok.
if (!nstart || !nend)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[collectNthnew: Bad collect]") << std::ends;
	errOut(&gerrStr,false);
	return false;				// Not allowing this case.
	}
return true;				// Start and end both present.
}


/********************************************
* FN:		RANGETEXT
* CR:		03/05/99 AM.
* SUBJ:	Get text for a range of nodes.
* RET:	True if ok, else false.
* NOTE:	Convenience function.
* ALLOC:	Throw into the string table, so don't have to worry about
*			de-alloc.
********************************************/

_TCHAR *Pat::rangeText(Node<Pn> *nstart, Node<Pn> *nend, Parse *parse)
{
if (!nstart || !nend)
	return 0;

Pn *pn1, *pn2;
long n1, n2;
pn1 = nstart->getData();
pn2 = nend->getData();
n1 = pn1->getStart();
n2 = pn2->getEnd();
_TCHAR *txt, *ptr;
long len;
txt = parse->getText();
ptr = &(txt[n1]);
len = n2 - n1 + 1;

_TCHAR *str;
parse->internStr(ptr, len, /*DU*/ str);
return str;
}


/********************************************
* FN:		INITLOOKAHEAD
* CR:		11/22/99 AM.
* SUBJ:	Set node that matched lookahead rule element, if any.
* RET:	True if ok, else false.
* NOTE:	Subsequent actions may mangle the initial node. ASSUMING
*			THIS WON'T HAPPEN, FOR NOW.
********************************************/

bool Pat::initLookahead(Nlppp *nlppp)
{
nlppp->lookahead_ = 0;		// Preemptive.			// 12/18/99 AM.
int ord = nlppp->rule_->getLookahead();
if (ord <= 0)
	{
	nlppp->nlookahead_ = 0;								// 09/06/08 AM.
	return true;
	}
nlppp->nlookahead_ = ord;								// 09/06/08 AM.

// Traverse the collect to position of lookahead rule element.
// Pick up first node found there, if any.
// If not found, traverse ahead to next available node in collect.
// (Latter case possible if lookahead elt is optional.)
Node<Pn> *node;
if (!(node = nlppp->collect_->getRoot()))
	return false;

while (--ord)
	node = node->Right();
if (!node)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Lookahead error.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
// Now we're at optional element's nodes.
Node<Pn> *tmp;
if ((tmp = node->Down()))
	{
	nlppp->lookahead_ = tmp;			// Got lookahead node.
	return true;
	}

// Presumably lookahead elt is optional.  So look to the right.
while ((node = node->Right()))
	{
	if ((tmp = node->Down()))				// Found nodes here.
		{
		nlppp->lookahead_ = tmp;		// Got lookahead node.
		return true;
		}
	}
//*gerr << "[Couldn't find lookahead node.]" << std::endl;
return true;				// Preemptive fix.			// 12/18/99 AM.
// Could be at end of file or phrase.
// All elts to right of lookahead could be optional.
}



/********************************************
* FN:		UPDATELOOKAHEAD
* CR:		12/14/03 AM.
* SUBJ:	Dynamically update lookahead node for a rule match.
* RET:	True if ok, else false.
* NOTE:	UNUSED.
********************************************/

bool Pat::updateLookahead(
	long ord,
	Nlppp *nlppp
	)
{
if (ord <= 0)
	return true;
// Traverse the collect to position of lookahead rule element.
// Pick up first node found there, if any.
// If not found, traverse ahead to next available node in collect.
// (Latter case possible if lookahead elt is optional.)
Node<Pn> *node;
if (!(node = nlppp->collect_->getRoot()))
	return false;

while (--ord)
	node = node->Right();
if (!node)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[updateLookahead: Error.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
// Now we're at element's nodes.
Node<Pn> *tmp;
if ((tmp = node->Down()))
	{
	nlppp->after_ = tmp;
	return true;
	}
std::_t_strstream gerrStr;
gerrStr << _T("[updateLookahead: Error.]") << std::ends;
nlppp->parse_->errOut(&gerrStr,false);
return false;
}


/********************************************
* FN:		UPDATEPARENT
* CR:		12/19/99 AM.
* SUBJ:	Update parent of a list of nodes to be excised.
* RET:	True if ok, else false.
* ASS:	node1 and node2 are in same phrase.
* NOTE:	2/24/03 AM. NO UPDATES TO PARENT.  IT STILL
*			COVERS SAME REGION.
********************************************/

#ifdef OBSOLETE_022403
bool Pat::updateParent(
	Node<Pn> *node1,			// First node to be excised.
	Node<Pn> *node2			// Last node to be excised.
	)
{
if (!node1 || !node2)
	return true;
if (node1->Left())			// Not first in its phrase.
	return true;

// FIX PARENT'S TEXT POINTER.
Node<Pn> *parent = node1->Up();
Node<Pn> *nxt = node2->Right();
if (!parent)	// Can't excise root of entire tree.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't excise root of parse tree.]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
Pn *ppn = parent->getData();
if (nxt)
	{
	Pn *xpn = nxt->getData();
	ppn->setText(xpn->getText());		// Update parent data.
	long xstart = xpn->getStart();	// 02/24/03 AM.
	ppn->setStart(xpn->getStart());	// Update parent data.
	}
else		// Excising all children of parent.
	{
	ppn->setText(0);
	ppn->setStart(-1);
	ppn->setEnd(-1);
	}
return true;
}
#endif



/********************************************
* FN:		REDSINGLE
* CR:		12/04/03 AM.
* SUBJ:	Manually perform a parse tree reduction.
* RET:	Suggested node.
********************************************/

Node<Pn> *Pat::redsingle(
	Node<Pn> *first,	// First node in range.
	Node<Pn> *last,	// Last node in range.
	_TCHAR *s_sugg,		// Suggested node name.
	bool base,
	bool unsealed,
	Nlppp *nlppp
	)
{
if (!first || !last || empty(s_sugg))
	return 0;

// Create a new node for the reduced rule.
long o_start, o_end;
long ustart, uend;	// [UNICODE]
Pn *tmp;
_TCHAR *text;
tmp = first->getData();
long line = tmp->line_;	// Line number for new node.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= last->getData()->getEnd();
ustart = tmp->getUstart();	// [UNICODE]
uend = nlppp->last_->getData()->getUend();	// [UNICODE]
Node<Pn> *restart = tmp->restart_;	// RECOPT2.	// 07/20/06 AM.

long ruleline = 0;

Parse *parse = nlppp->parse_;
Sym *sym = parse->getSym(s_sugg);	// Intern the name.
assert(sym);
_TCHAR *str = sym->getStr();

// Allowing retokenized alphas, etc.
enum Pntype typ = nodetype(str);

Node<Pn> *sub				// The new subtree.
   = Pn::makeNode(o_start, o_end, ustart, uend, typ, text, str, sym,	// [UNICODE]
							line,
							parse->currpass_,		// Assoc code pass
							ruleline					// Assoc rule line
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.
tmp->setBase(base);
tmp->setUnsealed(unsealed);
tmp->setFired(true);
tmp->setBuilt(true);
sub->getData()->restart_ = restart;						// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.

// Perform layering, if any.
Node<Pn> *bottom;
bottom = sub;
//sub = Layering(bottom, sugg->getAttrs(),parse);


// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();
tree->replaceSubs(first, last, sub, sub);

// Now the parse tree nodes will be placed under the reduce node.
Node<Pn>::linkNodes(bottom, first);

// FIX UP PARSE STATE HERE.												// 02/04/05 AM.
// For use in optimized recursive pass.								// 02/04/05 AM.
// Also to enable actions after the reduce.							// 02/04/05 AM.
//nlppp->first_ = nlppp->last_ = sub;										// 02/04/05 AM.

// Notify that a reduce node has been built.
//nlppp->red_ = sub;

// Clean up.
// Caller will clean up the collect nodes.
return sub;
}

/******************************************************************************/
//				COMPILED RUNTIME VARIANTS
/******************************************************************************/


/********************************************
* FN:		SINGLE
* CR:		05/19/00 AM.
* SUBJ:	Perform single-tier reduction.
* RET:	True if ok, else false.
* NOTE:	Compiled runtime variant of redSingle.
********************************************/

bool Pat::single(
	Nlppp *nlppp
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: Null first or last node in range.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: Rule already reduced.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Single-tier reduce: Collect already used.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

//Isugg *sugg = nlppp->rule_->getSugg();	// Suggested elt.
const SUGG *sugg = nlppp->sugg_;

// Create a new node for the reduced rule.
long o_start, o_end;
long ustart, uend;	// [UNICODE]
Pn *tmp;
_TCHAR *text;
tmp = nlppp->first_->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= nlppp->last_->getData()->getEnd();
ustart = tmp->getUstart();	// [UNICODE]
uend = nlppp->last_->getData()->getUend();	// [UNICODE]
Node<Pn> *restart = tmp->restart_;	// RECOPT2.	// 07/20/06 AM.

Node<Pn> *sub;				// The new subtree.
Parse *parse = nlppp->parse_;												// 08/22/02 AM.

//long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);// 01/06/02 AM.

Sym *sym;
_TCHAR *str;
sym = parse->getSym(sugg->name);			// Intern the name.
assert(sym);
str = sym->getStr();
// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.
sub = Pn::makeNode(o_start, o_end, ustart, uend, typ, text, str, sym,	// [UNICODE]
							line,													// 05/17/01 AM.
							nlppp->sugg_->passnum,							// 08/09/02 AM.
							nlppp->sugg_->ruleline							// 08/09/02 AM.
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.
tmp->setBase(sugg->base);
tmp->setUnsealed(sugg->unsealed);
tmp->setFired(true);															// 05/21/01 AM.
tmp->setBuilt(true);															// 05/25/01 AM.

// Perform layering, if any.
Node<Pn> *bottom;
bottom = sub;
sub = Layering(bottom, sugg->layers);	// Optimized variant.

sub->getData()->restart_ = restart;						// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.

// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();
tree->replaceSubs(nlppp->first_, nlppp->last_, sub, sub);

// Now the parse tree nodes will be placed under the reduce node.
Node<Pn>::linkNodes(bottom, nlppp->first_);

// Notify that a reduce node has been built.
nlppp->red_ = sub;

nlppp->node_ = nlppp->first_ = nlppp->last_ = sub;	// PARSESTATE.	// 06/03/13 AM.
nlppp->after_ = nlppp->node_->Right();					// PARSESTATE.	// 06/03/13 AM.

// FIX UP PARSE STATE HERE.												// 02/04/05 AM.
// For use in optimized recursive pass.								// 02/04/05 AM.
// Also to enable actions after the reduce.							// 02/04/05 AM.
//nlppp->first_ = nlppp->last_ = sub;										// 02/04/05 AM.

// Clean up.
// Caller will clean up the collect nodes.
return true;
}


/********************************************
* FN:		LAYERELT
* CR:		05/25/00 AM.
* SUBJ:	Layer attrs in parse tree for nodes matching current rule elt.
* RET:	True if ok, else false.
* NOTE:	Compiled runtime variant.
********************************************/

bool Pat::layerElt(
	COLL *pcoll,				// Collect covering nodes of current elt.
	const _TCHAR **attrs,		// Attributes to layer on nodes of current elt.
	Parse    *parse,			// Current parse instance.
	/*DU*/
	Node<Pn>* &first,			// First node matched by rule.
	Node<Pn>* &last			// Last node matched by rule.
	)
{
//if (Debug())
//	*gout << "   [IN LAYERELT]" << std::endl;

if (!pcoll || !attrs || !*attrs || !parse)
	return false;

bool upfirst, uplast;		// If need to update first, last.

Node<Pn> *start, *end;
start = (Node<Pn> *) pcoll->start;
end   = (Node<Pn> *) pcoll->end;
if (!start)
	return true;				// Ok, because elt must be optional.

upfirst = (first == start);
uplast  = (last  == end);

Node<Pn> *newfirst = 0;
Node<Pn> *newlast = 0;

// Because executing these rules can wipe out the nodes of the collect,
// we need to retain information for patching up the collect.
Node<Pn> *before, *after;
after = 0;
Node<Pn> *parent;
parent = 0;			// Don't set till needed.

if (end)
	after = end->Right();

if ((before = start->Left()))			// Normal case in parse tree.
	;
else
	parent = start->Up();
// Note: if parent, then start is first node in a list of nodes.
// Else, start is the root of the parse tree.

//// DO LAYERING HERE.

Tree<Pn> *tree = (Tree<Pn> *)parse->getTree();

// For each node matching current element:
Node<Pn> *node;
Node<Pn> *next;	// Save next node before current is mangled.
Node<Pn> *bottom, *top;
top = bottom = 0;
node = start;
while (node && node != after)
	{
	next = node->Right();			// Save this first!

	// Build layered nodes, pointing to top and bottom.
	Layering(node, attrs,parse, /*UP*/ top, bottom); // Variant builds all nodes.

	if (!top || !bottom)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[layerElt: Error.]") << std::ends;
		parse->errOut(&gerrStr,false);
		return false;				// Something went wrong.
		}

	// Excise node from tree, replacing it with TOP.
	tree->replaceSubs(node, node, top, top);

	// Attach excised node to BOTTOM.
	Node<Pn>::linkNodes(bottom, node);

	// Move node's semantics up to top node in layering chain.
	Pn *pn;
	Sem *sem;
	pn = node->getData();
	sem = pn->getSem();
	pn->setSem(0);
	top->getData()->setSem(sem);
	// NOT MOVING DYNAMIC SEMANTICS UP, BY DEFAULT.

	if (!newfirst)
		newfirst = top;

	node = next;			// Move to next node for current elt.
	}

newlast = top;

if (!newfirst || !newlast)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[layerElt: Error.]") << std::ends;
	parse->errOut(&gerrStr,false);
	return false;
	}

//// END OF LAYERING

pcoll->start = newfirst;
pcoll->end = newlast;

// Update first and last pointers, when needed.
if (upfirst)
	first = newfirst;
if (uplast)
	last  = newlast;

return true;
}


/********************************************
* FN:		LAYERING
* CR:		05/19/00 AM.
* SUBJ:	Perform additional reductions (layering) for given node.
* RET:	Reduced node, else given node.
* NOTE:	Variant that uses array of strings.
********************************************/

Node<Pn> *Pat::Layering(
	Node<Pn> *node,
	const _TCHAR **attrs
	)
{
if (!node || !attrs || !*attrs)
	return node;

//if (Debug()) *gout << "   [Layering]" << std::endl;

// Move node's semantics up to top node in layering chain.
Node<Pn> *tmp;				// The new subtree.
Pn *pn;
Sem *sem;
pn = node->getData();
sem = pn->getSem();
pn->setSem(0);
int passnum = pn->passnum_;												// 08/09/02 AM.
long ruleline = pn->ruleline_;	// Associated code line.		// 01/04/02 AM.
Node<Pn> *restart = pn->restart_;	// RECOPT3			// FIX	// 08/16/08 AM.

bool uns = pn->getUnsealed();	// For moving up chain.

// For each attr, make a reduction.
for (; *attrs; ++attrs)
	{
	tmp = new Node<Pn>(*node);					// COPY NODE. (Copy constructor).
	tmp->setUp(0);
	tmp->setDown(0);
	tmp->setLeft(0);
	tmp->setRight(0);
	tmp->seteDown(0);
	pn = tmp->getData();
	pn->setBase(false);	// Layers do not get the BASE flag.
	pn->setUnsealed(uns); // Layers get UNSEALED flag.
	pn->setFired(true);														// 05/21/01 AM.
	pn->setBuilt(true);														// 05/25/01 AM.
	pn->setName((_TCHAR *) *attrs);
	pn->passnum_ = passnum;			// Associated code pass.		// 08/09/02 AM.
	pn->ruleline_ = ruleline;		// Associated code line.		// 01/04/02 AM.
	pn->restart_ = restart;				// RECOPT3			// FIX	// 08/16/08 AM.
	Node<Pn>::linkNodes(tmp, node);
	node = tmp;			// New top node in chain.
	}

// Seems to me the sem needs to be reattached.
pn = node->getData();
pn->setSem(sem);

return node;
}



/********************************************
* FN:		LAYERING
* CR:		05/25/00 AM.
* SUBJ:	Convert attrs to a subtree of nodes.
* RET:	UP top,bottom = top and bottom nodes of chain.
* NOTE:	This is a variant for layering in a rule phrase, rather
*			than suggested element.
*			Compiled runtime variant.
********************************************/

void Pat::Layering(
	Node<Pn> *node,			// Current node, for reference.
	const _TCHAR **attrs,
	Parse *parse,																// 08/22/02 AM.
	/*UP*/
	Node<Pn>* &top,
	Node<Pn>* &bottom
	)
{
top = bottom = 0;
if (!attrs  || !*attrs || !node)
	return;

//if (Debug())
//	*gout << "   [Layering (for phrase elts)]" << std::endl;

// Do first attr separately.
Pn *pn;
long start, end;
long ustart, uend;	// [UNICODE]
_TCHAR *text;
pn		= node->getData();
long line = pn->line_;	// Line number for new node.				// 05/17/01 AM.
start = pn->getStart();
end   = pn->getEnd();
ustart = pn->getUstart();	// [UNICODE]
uend = pn->getUend();	// [UNICODE]
text	= pn->getText();

_TCHAR *str = (_TCHAR *) *attrs;	// Name node using attr.			// 05/01/03 AM.
// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.
node = Pn::makeNode(start,end,ustart,uend,typ,text,	// [UNICODE]
				str,					// Name node using attr.			// 05/01/03 AM.
				0,										// Get sym from where?
				line,																// 05/17/01 AM.
				parse->currpass_,		// Assoc code pass				// 01/04/02 AM.
				pn->ruleline_	// Associated code line					// 01/04/02 AM.
				);
Pn *pntmp = node->getData();												// 05/21/01 AM.
pntmp->setFired(true);														// 05/21/01 AM.
pntmp->setBuilt(true);														// 05/25/01 AM.
// Other node values set to default.

Node<Pn> *tmp;
bottom = node;
for (++attrs; *attrs; ++attrs)
	{
	str = (_TCHAR *) *attrs;	// Name node using attr.				// 05/01/03 AM.
	typ = nodetype(str);														// 05/01/03 AM.
	tmp = Pn::makeNode(start,end,ustart,uend,typ,text,	// [UNICODE]
				str,																// 05/01/03 AM.
				0,										// Get sym from where?
				line,																// 05/17/01 AM.
				parse->currpass_,		// Assoc code pass				// 01/04/02 AM.
				pn->ruleline_			// Assoc code line				// 01/04/02 AM.
				);
	pntmp = tmp->getData();													// 05/17/01 AM.
	pntmp->setFired(true);													// 05/17/01 AM.
	pntmp->setBuilt(true);													// 05/25/01 AM.
	// Other node values set to default.
	Node<Pn>::linkNodes(tmp, node);
	node = tmp;			// New top node in chain.
	}

top = node;
}




/********************************************
* FN:		MERGE
* CR:		06/05/00 AM.
* SUBJ:	Reduce, dissolving all top-level nodes in phrase.
* RET:	True if ok, else false.
* NOTE:	Compiled runtime variant.  Taken from redMerge.
********************************************/

bool Pat::merge(
	Nlppp *nlppp
	)
{
if (!nlppp->first_ || !nlppp->last_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: Null first or last node in range.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->red_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: Rule already reduced.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}
if (nlppp->used_)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[redMerge: Collect already used.]")
		  << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	return false;
	}

const SUGG *sugg = nlppp->sugg_;	// Suggested elt.

// Create a new node for the reduced rule.
long o_start, o_end;
long ustart, uend;	// [UNICODE]
Pn *tmp;
_TCHAR *text;
tmp = nlppp->first_->getData();
long line = tmp->line_;	// Line number for new node.				// 05/17/01 AM.
o_start	= tmp->getStart();
text		= tmp->getText();
o_end		= nlppp->last_->getData()->getEnd();
ustart = tmp->getUstart();	// [UNICODE]
uend = nlppp->last_->getData()->getUend();	// [UNICODE]
Node<Pn> *restart = tmp->restart_;						// RECOPT2.	// 07/20/06 AM.

//long ruleline = (nlppp->rule_ ? nlppp->rule_->getLine() : 0);	// 01/06/02 AM.

Node<Pn> *sub;				// The new subtree.
Sym *sym;
_TCHAR *str;
sym = nlppp->parse_->getSym(sugg->name);			// Intern the name.
assert(sym);
str = sym->getStr();
// Allowing retokenized alphas, etc.									// 05/01/03 AM.
enum Pntype typ = nodetype(str);											// 05/01/03 AM.
sub = Pn::makeNode(o_start, o_end, ustart, uend, typ, text, str, sym,	// [UNICODE]
							line,													// 05/17/01 AM.
							nlppp->sugg_->passnum,							// 08/09/02 AM.
							nlppp->sugg_->ruleline							// 08/09/02 AM.
							);
tmp = sub->getData();

// Perform other instructions of suggested rule elt.
tmp->setBase(sugg->base);
tmp->setUnsealed(sugg->unsealed);
tmp->setFired(true);															// 05/21/01 AM.
tmp->setBuilt(true);															// 05/25/01 AM.

// Perform layering, if any.
Node<Pn> *bottom;
bottom = sub;
sub = Layering(bottom, sugg->layers);

sub->getData()->restart_ = restart;						// RECOPT2.	// 07/20/06 AM.
nlppp->rmost_ = nlppp->rightmost_ = sub;				// RECOPT2.	// 07/20/06 AM.

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();

// Now the parse tree nodes themselves will be excised and replaced by
// the reduce node.
tree->replaceSubs(nlppp->first_, nlppp->last_, sub, sub);

////////////////////////////////////
// DISSOLVE ALL THE TOP-LEVEL NODES.
////////////////////////////////////
// (Watch for case where node has no children.)
// Find the first node with children.
Node<Pn> *curr = nlppp->first_;
Node<Pn> *next, *first = 0, *last = 0, *childs, *echilds;
while (curr)
	{
	next = curr->Right();
	if ((childs = curr->Down()))
		{
		childs->setUp(0);	// Detach from parent.

		if (!first)			// At first node with children.
			{
			first = childs;
			if (!(last  = curr->eDown()))
				last = childs->last();
			}
		else					// Extending list of children.
			{
			if (!(echilds = curr->eDown()))
				echilds = childs->last();
			// Link the lists.
			last->setRight(childs);
			childs->setLeft(last);
			last = echilds;
			}
		}

	// Delete the current node.
	curr->setDown(0);
	curr->setLeft(0);									// FMR PURIFY.	// 07/27/03 AM.
	curr->setRight(0);
	curr->seteDown(0);
	//Node<Pn>::DeleteNodeAndData(curr);								// 11/25/00 AM.
	tree->deleteSubs(curr);		// More proper deletion.			// 11/25/00 AM.

	curr = next;	// Next top-level node.
	}

if (!first)			// Nothing underneath any node.
	{
	nlppp->first_ = nlppp->last_ = 0;
	nlppp->red_ = sub;
	return true;
	}

nlppp->first_ = first;
nlppp->last_ = last;

////////////////////////////////////

// Now the parse tree nodes will be placed under the reduce node.
Node<Pn>::linkNodes(bottom, nlppp->first_);

// Notify that a reduce node has been built.
nlppp->red_ = sub;

// Clean up.
// Caller will clean up the collect nodes.
return true;
}

/********************************************
* FN:		POSTERROUT
* CR:		11/20/01 AM.
* SUBJ:	Print error message.
* RET:	True if ok, else false.
* NOTE:	To make sure error message gets the right line and pass number.
********************************************/

bool Pat::postErrout(
	bool retval,		// Value to return.
	Nlppp *nlppp
	)
{
if (!nlppp)
	return false;
Parse *parse = nlppp->parse_;
Eana *eana = parse->getEana();
long line = parse->line_;

if (eana->getFintern())	// If interning analyzer.
	{
	Node<Pn> *xx = nlppp->first_;
	Pn *xxpn;
	if (xx)
		{
		xxpn = xx->getData();
		parse->inputline_ = xxpn->getLine();
		}
	else
		parse->inputline_ = 0;
	return errOut(retval,parse->inputpass_,parse->inputline_); // UNFIXED
	}
else	// Normal text analysis.
	{
	parse->line_ = line;
	return errOut(retval,parse->currpass_,parse->line_); // UNFIXED
	}
return false;
}


/********************************************
* FN:		SPLICEELTS
* CR:		07/19/02 AM.
* SUBJ:	Fix up data structures for SPLICE reduce function.
* RET:	true if ok.
*			UP nlppp - Mangles parse tree, collect, first and last.
* NOTE:	Modularity function.
* OPT:	Somewhat inefficient to traverse collect to gather node1 and node2,
*			then retraverse here to do the fix ups.  Could all be done as a singl
*			traverse.
********************************************/

bool Pat::spliceElts(
	long num1,	// First element in range.
	long num2,	// Last element in range.
	Nlppp *nlppp
	)
{
if (!nlppp)
	return false;
Tree<Pn> *collect = nlppp->collect_;
if (!collect)
	return false;
Node<Pn> *coll = collect->getRoot();

Tree<Pn> *tree = (Tree<Pn> *)nlppp->parse_->getTree();

// New first and last nodes that were spliced.
Node<Pn> *first=0, *last=0;

// Traverse collect one element at a time.
long long ord = 0;
Node<Pn> *sub1=0, *sub2=0;
Node<Pn> *child1, *child2;	// First and last child for current element.
for (; coll; coll = coll->Right())
	{
	++ord;
	if (ord < num1)
		continue;
	if (ord > num2)
		break;
	if (!(sub1 = coll->pDown))
		continue;

	// Now we're at an element in the range, with child nodes.
	sub2 = coll->peDown;	// Rightmost node so far, or null.

	// Splice this subrange of nodes.
	tree->spliceSubs(sub1,sub2,/*UP*/child1,child2);
	if (!first)
		first = child1;
	last = child2;

	// Fix up collect element.
	coll->pDown = child1;
	coll->peDown = child2;
	}

// Fix up pattern matcher data structures.
nlppp->first_ = first;
nlppp->last_ = last;

// This may not always be right, e.g., if there's triggering...
nlppp->node_ = first;
return true;
}
