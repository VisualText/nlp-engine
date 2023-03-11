/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	LITERUG.CPP
* FILE:	lite/literug.cpp
* CR:		05/19/99 AM.
* SUBJ:	Support for rule generation (LIBRUG.DLL).
*
*******************************************************************************/

#include "StdAfx.h"
#include <time.h>
#include "machine.h"		// 03/09/00 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "consh/libconsh.h"		// 02/14/01 AM.
#include "consh/cg.h"				// 02/14/01 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"															// 09/26/01 AM.
#include "io.h"
#include "std.h"
#include "chars.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"		// 02/26/01 AM.
#include "pn.h"				// 02/14/01 AM.
#include "nlppp.h"	// Linux.	// 04/26/07 AM.
#include "gen.h"	// Linux.	// 04/26/07 AM.
#include "irule.h"
#include "isugg.h"
#include "iaction.h"
#include "ipair.h"
#include "ipre.h"			// 11/11/99 AM.
#include "mode.h"			// 04/04/00 AM.
#include "ana.h"
#include "parse.h"

/********************************************
* FN:		genSample
* CR:		05/18/99 AM.
* SUBJ:	Generate a rule for current sample of Gram hierarchy concept.
* NOTE:	If pass not built yet, create it and add it to analyzer.
*			If member of parse, then it shouldn't be an arg to this fn.
********************************************/

bool
Parse::genSample(
	long start,				// Start offset of sample in input text.
	long end,				// End offset of sample in input text.
	Parse *parse,			// Current text analysis instance.
	int num,					// Current pass number.
	_TCHAR *conname,			// Concept name.
	/*DU*/
	Delt<Seqn> *seq,		// Down: current pass. UP: new pass.
								// New pass contains rule for sample.
	Dlist<Irule> *rules,	// List of rules being built for current concept.
	Dlist<Iarg>  *selects	// List of context nodes.
	)
{
// Get parse tree.
Tree<Pn> *tree;
Node<Pn> *root;
if (!(tree = (Tree<Pn> *)parse->getTree()))
	return false;
if (!(root = tree->getRoot()))
	return false;

	//   Find sample offsets in parse tree.
	//   If improper coverage (eg, nodes split across separate subtrees),
	//      warn of user error.
	//		  Allow fudge around whitespace.
	//   Else, build pass if not built yet.
	//      (And attach pass to analyzer sequence.)
	//      Build rule for sample.
	//      (Merge rules.)
	//      Find context node (parent) in which rule applies.

Node<Pn> *n_start, *n_end, *parent;
n_start = n_end = parent = 0;
bool ok = false;
ok = Pn::findOffsets(root, start, end,
									/*UP*/ n_start, n_end, parent);
if (!ok)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[COULDN'T FIND SAMPLE IN PARSE TREE.]") << std::ends;
	::errOut(&gerrStr,false);
	return false;			// Failed to find phrase in tree.
	}
std::_t_cout << _T("[FOUND SAMPLE IN TREE!]") << std::endl;

// Intern the conname, which comes from who knows where.
_TCHAR buf[1024];
_stprintf(buf, _T("_%s"), conname);	// MAKE INTO NONTERMINAL. // 05/30/99 AM.
_TCHAR *str;
parse->internStr(buf, /*DU*/ str);		// FIX 05/26/99 AM.


// Build a rule for sample.
// Add parent to a list of context nodes.
// Quickest and dirtiest:
//Dlist<Irule> *rules;
//rules = new Dlist<Irule>();

genRule(n_start, n_end, str, rules, 0, parse);

// See if parent is already in selects list.
// If not, add it to the list.
// For now, just adding to the list.	// 05/21/99 AM.
Pn *pn;
_TCHAR *name;
pn = parent->getData();
name = pn->getName();
if (!Iarg::findArg(selects, name))	// If select node not on list.
	{
	// Add select node name to list.
	Iarg *arg;				// Single select node name.
	arg = new Iarg(name);
	// 07/02/99 AM. No longer copies.
	selects->rpush(arg);
	//delete arg;
	}

return true;
}


/********************************************
* FN:		genxSample
* CR:		06/08/99 AM.
* SUBJ:	Generate a rule for current sample of Gram hierarchy concept.
* OPT:	Should probably get sample concept.  Text is being manipulated
*			and refetched all over the place.
* NOTE:	Add LABELS to generated rule.
*			If member of parse, then it shouldn't be an arg to this fn.

*	   Find sample offsets in parse tree.
*	   If improper coverage (eg, nodes split across separate subtrees),
*	      warn of user error.
*			  Allow fudge around whitespace.
*	      Find context node (parent) in which rule applies.
*			Build rule for sample, collect into list.
*	      Build rule for sample.

*	 GENX: Traverse current concept's children
*	 (LABELS) and their samples after the rule has been built.  Find
*	 any samples that occur within the rule, count which rule element
*	 the label belongs to, and add it as a LAYER attribute.
*	 (or GROUP post action, if covers multiple rule elements.)
********************************************/

bool
Parse::genxSample(
	_TCHAR *fname,			// Sample's text file name.
	long start,				// Start offset of sample in input text.
	long end,				// End offset of sample in input text.
	_TCHAR *text,				// Sample text.
	CONCEPT *conc,			// Current concept in Gram hierarchy.
	Parse *parse,			// Current text analysis instance.
	_TCHAR *conname,			// Concept name.
	Mode *mode,				// 08/09/99 AM.
	CG *cg,					// Current KB.
	/*DU*/
	Dlist<Irule> *rules,	// List of rules being built for current concept.
	Dlist<Iarg>  *selects	// List of context nodes.
	)
{
if (!parse)			// 05/20/09 AM.
	return false;	// 05/20/09 AM.

// Get parse tree.
Tree<Pn> *tree;
Node<Pn> *root;
if (!(tree = (Tree<Pn> *)parse->getTree()))
	return false;
if (!(root = tree->getRoot()))
	return false;

bool closed = false;		// Default closed-set attr from Gram hierarchy.
closed = mode->getClosed();

	//   Find sample offsets in parse tree.
	//   If improper coverage (eg, nodes split across separate subtrees),
	//      warn of user error.
	//		  Allow fudge around whitespace.
	//   Else,build rule for sample.
	//      (Merge rules.)
	//      Find context node (parent) in which rule applies.

Node<Pn> *n_start, *n_end, *parent;
n_start = n_end = parent = 0;
bool ok = false;
ok = Pn::findOffsets(root, start, end,
									/*UP*/ n_start, n_end, parent);
if (!ok)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[COULDN'T FIND SAMPLE IN PARSE TREE.]") << std::ends;
	::errOut(&gerrStr,false);
	return false;			// Failed to find phrase in tree.
	}

// HANDLING WHEN SINGLE CONTEXT NODE FOUND.  // 10/29/99 AM.
if (!nixContext(/*DU*/ n_start, n_end, parent))
	return false;

std::_t_cout << _T("[FOUND SAMPLE IN TREE!]") << std::endl;

// Convert concept name to a nonterminal.
_TCHAR buf[1024];
_stprintf(buf, _T("_%s"), conname);	// MAKE INTO NONTERMINAL. // 05/30/99 AM.
_TCHAR *str;
parse->internStr(buf, /*DU*/ str);		// FIX 05/26/99 AM.


// Build a rule for sample.
// Add parent to a list of context nodes.
// Quickest and dirtiest:
//Dlist<Irule> *rules;
//rules = new Dlist<Irule>();

Delt<Irule> *drule;
Irule *rule;
drule = genRule(n_start, n_end, str, rules, mode, parse);
if (!drule)
	return false;
rule = drule->getData();

// ADD SAMPLE TEXT TO RULE!	// 06/16/99 AM.
_TCHAR *sample;
parse->internStr(text, /*UP*/ sample);
rule->setSample(sample);

// See if parent is already in selects list.
// If not, add it to the list.
// For now, just adding to the list.	// 05/21/99 AM.
Pn *pn;
_TCHAR *name;
pn = parent->getData();
name = pn->getName();
if (!Iarg::findArg(selects, name))	// If select node not on list.
	{
	// Add select node name to list.
	Iarg *arg;				// Single select node name.
	arg = new Iarg(name);
	selects->rpush(arg);
	//delete arg;				// 07/02/99 AM.
	}

// GENX:	Traverse concept's children (ie, LABELS) and their samples.
// If any are in the current offsets in the current text, figure out
// which rule element the label gets added to.
genxLabels(conc, rule, fname, start, end, n_start, n_end, closed, cg, parse);

// QUICK-AND-DIRTY.  If closed attribute is set from hierarchy, traverse
// all the rule elts and turn on the closed flag.
if (closed)
	closedOn(rule);

// Traverse the elts of RAW rule.  Fix up any closed elts to
// have only the literal token, not suggested concepts. // 08/19/99 AM.
fixClosed(rule);

return true;
}


/********************************************
* FN:		GENRULE
* CR:		05/19/99 AM.
* SUBJ:	Build a rule for a sample text.
********************************************/


Delt<Irule> *Parse::genRule(
	Node<Pn> *start,					// Start of phrase in parse tree.
	Node<Pn> *end,						// End of phrase in parse tree.
	_TCHAR *suggstr,						// String for suggested concept.
	Dlist<Irule> *rules,				// Current rules list being built.
	Mode *mode,							// 08/09/99 AM.
	Parse *parse
	)
{
if (!start || !end || !rules || !mode)
	return 0;

bool xwhite = mode->getXwhite();			// 08/09/99 AM.

// If this rule will have layering, copy the layer list.		// 12/10/99 AM.
Dlist<Iarg> *mlayer = mode->getLayer();							// 12/10/99 AM.
Dlist<Iarg> *layer = 0;													// 12/10/99 AM.
if (mlayer)
	layer = new Dlist<Iarg>(*mlayer);	// Copy constr			// 12/10/99 AM.

bool prevwhite = false;		// If previous elt was whitespace.

// Convenience variables for each rule.
Dlist<Ielt> *phr = 0;						// Rule phrase.
Isugg *sugg = 0;								// Suggested elt.
Dlist<Ipre> *pres = 0;					// List of pre actions.
Dlist<Iaction> *checks = 0;				// List of check actions.
Dlist<Iaction> *posts = 0;					// List of post actions.
Delt<Ielt> *trig = 0;						// Trigger elt.
//Dlist<Iarg> *attrs = 0;			// Using layer.				// 12/10/99 AM.
Iarg *arg = 0;
int min = 1;			// 08/09/99 AM.
int max = 1;			// 08/09/99 AM.

phr = new Dlist<Ielt>();						// Create rule phrase.

pres = new Dlist<Ipre>();						// 07/09/99 AM.

// Move end down one node, to make a nice boundary for loop.
end = end->Right();

Node<Pn> *node;
Pn *pn;
_TCHAR *name;
enum Pntype typ;
int num = 0;				// Count which rule elt we're building.
for (node = start; node != end; node = node->Right())
	{
	++num;						// 07/06/99 AM.
	pn = node->getData();

	// HEURISTICS: num => _xNUM.
	// alpha => _xALPHA.	// Too general, in general... // 06/14/99 AM.
	// Need to make specific and generalized rules here.
	name = pn->getName();
	typ = pn->getType();
	// 06/25/99 AM. Not generalizing rule here, but elsewhere.

	// 07/06/99 AM. Collect all the names down to base, if any.
	// Why?  Because of ambiguity, primarily.  The same word or construct
	// can serve multiple purposes, and gathering them all will help build
	// appropriate rules.  Not clear whether the RAW rules, if
	// preserved, should take the deepest or shallowest singlet.  Probably
	// needs to be user-specified.
	// If no subs, keep the name.  If subs, change name to _xWILD and put
	// all names on MATCH list.
	_TCHAR *tok = 0;			// Token-level name, if present.
	_TCHAR *ttype = 0;		// Token-type name.
	Dlist<Iarg> *names = Parse::findSubs(node);
	if (names && names->getLast())										// 12/28/00 AM.
		tok = names->getLast()->getData()->getStr();
	else																			// 12/28/00 AM.
		tok = 0;																	// 12/28/00 AM.
	ttype = tok_type(tok);

	if (Iarg::whiteArg(names) && xwhite)		// 08/09/99 AM.
		{
		Dlist<Iarg>::DeleteDlistAndData(names);		// MEMLEAK	// 12/28/00 AM.
		names = 0;												// MEMLEAK	// 12/28/00 AM.

		// Current elt is whitespace.
		if (!prevwhite)				// First whitespace elt.
			{
			name = _T("_xWHITE");
			min = 0;				// Allow full range of whitespace.
			max = 0;
			prevwhite = true;			// If next is white, flag to ignore it.
			}
		else								// Previous was white, so ignore.
			continue;					// SKIP THIS ELEMENT.
		}
	else if (names->getLast() != names->getFirst())	// More than one name.
		{
		// Keep names.															// 12/28/00 AM.
		name = _T("_xWILD");
		prevwhite = false;			// 08/09/99 AM.
		min = max = 1;					// 08/10/99 AM.
		}
	else
		{
		// Keep name and trash the list.
		Dlist<Iarg>::DeleteDlistAndData(names);
		names = 0;
		prevwhite = false;			// 08/09/99 AM.
		min = max = 1;					// 08/10/99 AM.
		}

	// 07/06/99 AM. If reached token, get constraints  from
	// token into pre actions.
	if (ttype && tok && *tok)
		{
		// WARN: ADD PRE ACTIONS ALPHABETICALLY!! foldPres() depends
		// on them being sorted already!  (This is an optimization that
		// removes the need for sorting.  Hey! We're generating them, so lets
		// do it in an optimized way.)  ie "cap" precedes "length", etc.
		Ipre *pre = 0;
		bool cap = is_upper((_TUCHAR)*tok);						// 12/16/01 AM.

		if (cap)
			{
			pre = new Ipre(_T("cap"), 0, num,num);
			pres->rpush(pre);
			}

		if (!_tcscmp(ttype, _T("_xALPHA"))
			 || !_tcscmp(ttype, _T("_xNUM")))
			{
			// Build a length arg.
			long len = _tcsclen(tok);

			Dlist<Iarg> *pargs;
			Iarg *parg;
			// 06/04/00 AM. Generating length as number, not as string.
			//char buf[32];
			//char *ptr;
			//sprintf_s(buf, "%d", len);
			//parse->internStr(buf, ptr);
			//parg = new Iarg(ptr);
			parg = new Iarg((long long)len);									// 06/04/00 AM.
			pargs = new Dlist<Iarg>();
			pargs->rpush(parg);


			pre = new Ipre(_T("length"), pargs, num, num);
			pres->rpush(pre);
			}
		}

		// ADD CURRENT ELEMENT TO PHRASE.
		Delt<Ielt> *delt;
		Ielt *elt = 0;					// 08/09/99 AM.
		delt = Ielt::addDelt(phr, name,0,min,max);	// Add elt to phrase.
		elt = delt->getData();		// 08/09/99 AM.
		elt->setSinglet(true);		// 08/09/99 AM.
		if (names)
			{
			elt->setMatches(names);		// 07/06/99 AM.
			}
	// Create and attach list of pairs.
	}

sugg = new Isugg(suggstr);		// Create suggested elt.
sugg->setAttrs(layer);			// Layered names!			// 12/10/99 AM.

// 06/25/99 AM. Too presumptuous to do this.
//sugg->setBase(true);		// Don't always want this. // 06/25/99 AM.

// Create and attach list of pairs.

// Create pre,check,post actions, if any, here.
if (!(pres->getFirst()))							// 07/06/99 AM.
	{
	// Delete and zero out.
	Dlist<Ipre>::DeleteDlistAndData(pres);		// 07/06/99 AM.
	pres = 0;											// 07/06/99 AM.
	}

// Create and add rule to list.
return Irule::addDelt(rules, phr, sugg, pres, checks, posts, trig);
}


/********************************************
* FN:		genxLabels
* CR:		06/08/99 AM.
* SUBJ:	Add labels to generated rule.
* NOTE:	Traverse concept's children (ie, LABELS) and their samples.
*			If any are in the current offsets in the current text, figure out
*			which rule element the label gets added to.
*			For now, not handling labels that span more than one rule elt.
*			(See notes under genxLabel....)
*			07/16/99 AM. Count the number of labels in rule.  Store in rule.
********************************************/

bool
Parse::genxLabels(
	CONCEPT *conc,		// Current concept in the Gram hierarchy.
	Irule *rule,		// Rule generated for current sample of conc.
	_TCHAR *fname,		// Sample's text file name.
	long start,			// Start text offset of sample.
	long end,			// End text offset of sample.
	Node<Pn> *n_start,	// Sample's first node in parse tree.
	Node<Pn> *n_end,		// Sample's last node in parse tree.
	bool closed,			// Default closed-set attr.	// 07/19/99 AM.
	CG *cg,					// KB hierarchy object.
	Parse *parse
	)

{
if (!conc || !rule)
	return true;

// Get the list of labels under concept.
CONCEPT *labels, *label;

if (!(labels = cg->Down(conc)))
	return true;						// Concept without labels is ok.

_TCHAR buff[256];
_TCHAR name[512];
_TCHAR *tmp;

PHRASE *samples;		// Phrase of samples under concept.
CONCEPT *sample;
_TCHAR buf[1024];
long long s_start, s_end;
//char text[1024];
_TCHAR *text;																		// 11/12/00 AM.

int first = 0, last = 0;			// 07/14/99 AM.

int count = 0;				// Count of labels.			// 07/16/99 AM.

// Traverse the list of labels.
for (label = labels; label; label = cg->Next(label))
	{
	// For each label concept, traverse its samples, looking for
	// one that falls in the current parse tree phrase.

	// Get label's samples.
	if (!(samples = cg->findPhrase(label)))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Label has no samples.]") << std::ends;
		::errOut(&gerrStr,false);

		continue;
		}

	// Traverse list of samples for the current label
	// (and for the current overall sample).
	for (sample = cg->firstNode(samples); sample; sample = cg->Next(sample))
		{
		cg->conceptName(sample, buf);
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("  Sample=") << buf << std::ends;
		::errOut(&gerrStr,false);
		}

		cg->findVal(sample, _T("start"), /*UP*/ s_start);
		cg->findVal(sample, _T("end"), /*UP*/ s_end);
		//cg->findVal(sample, "text", /*UP*/ text);					// 11/12/00 AM.
		text = cg->strVal(sample, _T("text"));					// OPT.	// 11/12/00 AM.

		// Now dealing with relative file names in KB.  // 99/09/09 AM.
		_TCHAR sfile[MAXSTR];								// 99/09/99 AM.
		_stprintf(sfile, _T("%s%c%s"), cg->getAppdir(), DIR_CH, text);
#ifdef QDBM_
		cg->kbfree(text);														// 02/12/07 AM.
		text = 0;
#endif

//		if (!str_equal(fname, text))		// Not the same text file.
		if (strcmp_i(fname, sfile))	// FIX!	// 09/09/99 AM.
			continue;
		if (s_start < start || s_end > end)		// Offsets not contained.
			continue;

		// A proper sample has been found.
		++count;											// 07/16/99 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Embedded sample found]") << std::ends;
		::errOut(&gerrStr,false);
		}

		// Figure out which rule elt to attach label to.
		cg->conceptName(label, /*DU*/ buff);
		_stprintf(name, _T("_%s"), buff);
		parse->internStr(name, tmp);
		// Need to intern the string here.
		if (labelOffsets(s_start, s_end, n_start, n_end, /*UP*/ first,last))
			{
			genxLabel(tmp, rule, first, last, parse);				// 07/15/99 AM.
			labelAttrs(label, rule, first, last, closed, cg);	// 07/19/99 AM.
			}
		break;	// Only looking at the first sample that matched.
					// Ignore any others that the idiot user may have added.
					// (Todo: May want to check for these some day.)
		}
	}

// Store the number of labels in the current rule.
rule->setNlabels(count);				// 07/16/99 AM.

// If actions were generated, add a reduce action to the end.
Dlist<Iaction> *posts;
if ((posts = rule->getPost()))
	{
	// SORT ACCORDING TO DESCENDING ORDS.
	// Doing a bubble sort.
	if (!sortGroups(posts))
		return false;

	_TCHAR *func;
	func = _T("single");
	//Iarg *arg=0;
	Dlist<Iarg> *args=0;

	// Spurious arg because of RFA grammar bug. (requires (0) arg
	// if no args!
	//arg = new Iarg("0");													// 06/05/00 AM.
	//args = new Dlist<Iarg>();											// 06/05/00 AM.
	//args->rpush(arg);														// 06/05/00 AM.
	//delete arg;			// 07/02/99 AM.

	Iaction::addDelt(posts, func, args);
	}
return true;
}


/********************************************
* FN:		genxLabel
* CR:		06/09/99 AM.
* SUBJ:	Add one label to generated rule.
* ALG:	Figure out which rule element the sample corresponds to.
*			Traverse the nodes of the overall sample to find where the
*			label's sample resides.
*			Handling the case where the label matches ONE or MANY nodes of the
*			parse tree.
* NOTE:	Because of whitespace, etc. missing in the tree, allowing some
*			"fudge" by not requiring equality of offsets.
*			Multiple group actions will be tricky, because the numbering
*			of rule elements CHANGES after execution of each action in the
*			analysis.  (Alg: I'll order the actions so that HIGHER numbers
*			get grouped first.  This will also enable checking that samples
*			don't have overlapping numbers.)
********************************************/

bool
Parse::genxLabel(
	_TCHAR *label,		// Label name to add to layering.
	Irule *rule,		// Rule generated for current sample of conc.
	int first,				// 07/15/99 AM.
	int last,				// 07/15/99 AM.
	Parse *parse
	)
{
if (!rule || !first)
	return false;

// Build the layer or multi-reduce.

Dlist<Ielt> *list;
Delt<Ielt> *delt;
list = rule->getPhrase();
if (!(delt = list->getFirst()))
	return false;
int ii;

// LABEL SAMPLE COVERS MULTIPLE RULE ELEMENTS.
if (first != last)
	{
	// Build a GROUP action.				// 06/15/99 AM.
	Dlist<Iaction> *posts;
	if (!(posts = rule->getPost()))
		{
		// Create posts for rule.
		posts = new Dlist<Iaction>();			// Empty posts.
		rule->setPost(posts);
		}
	_TCHAR *func;
//	char buf[64];
	func = _T("group");
	Iarg *arg;
//	char *str;
	Dlist<Iarg> *args;

//	sprintf_s(buf, "%d", first);
//	parse->internStr(buf, /*DU*/ str);
//	arg = new Iarg(str);
	arg = new Iarg((long long)first);											// 08/17/01 AM.
	args = new Dlist<Iarg>();
	args->rpush(arg);
	//delete arg;				// 07/02/99 AM.

//	sprintf_s(buf, "%d", last);
//	parse->internStr(buf, /*DU*/ str);
//	arg = new Iarg(str);
	arg = new Iarg((long long)last);											// 08/17/01 AM.
	args->rpush(arg);
	//delete arg;				// 07/02/99 AM.

	arg = new Iarg(label);
	args->rpush(arg);
	//delete arg;				// 07/02/99 AM.

	Iaction::addDelt(posts, func, args);

	return true;
	}

// LABEL SAMPLE COVERS ONLY ONE RULE ELT.

for (ii = 1; ii < first; ++ii)
	delt = delt->Right();
if (!delt)
	return false;
// Now add a layer attribute to the rule element.
Ielt *elt;
Dlist<Iarg> *attrs;
Iarg *arg;
elt = delt->getData();
attrs = elt->getAttrs();
if (!attrs)		// Build layering into elt.
	{
	attrs = new Dlist<Iarg>();			// Empty list of attrs (ie layers).
	arg = new Iarg(label);
	attrs->rpush(arg);
	//delete arg;							// 07/02/99 AM.
	elt->setAttrs(attrs);				// Add layering.
	}
else			// Add to layering in elt.
	{
	arg = new Iarg(label);
	attrs->rpush(arg);
	//delete arg;							// 07/02/99 AM.
	}
return true;
}


/********************************************
* FN:		labelOffsets
* CR:		07/15/99 AM.
* SUBJ:	Find label sample's offsets in current rule.
* RET:	True if successfully found.
*			UP first,last - element numbers that match label's sample.
* ALG:	Figure out which rule element the sample corresponds to.
*			Traverse the nodes of the overall sample to find where the
*			label's sample resides.
* NOTE:	Split from genxLabel.
*			Because of whitespace, etc. missing in the tree, allowing some
*			"fudge" by not requiring equality of offsets.
********************************************/

bool
Parse::labelOffsets(
	long start,				// Start text offset of label's sample.
	long end,				// End text offset of label's sample.
	Node<Pn> *n_start,	// Sample's first node in parse tree.
	Node<Pn> *n_end,		// Sample's last node in parse tree.
	/*UP*/
	int &first,				// Start offset of label's sample in rule.
	int &last				// End offset of label's sample in rule.
	)
{
first = last = 0;
if (!n_start || !n_end)
	return false;

// FIND LABEL SAMPLE'S OFFSETS IN THE RULE.
// (Same as offsets in the parse tree phrase that
// gave rise to the rule.)
Node<Pn> *node;
Pn *pn;
int count = 0;
node = n_start;
// Look for set of nodes within start,end range.
for (node = n_start; node; node = node->Right())
	{
	++count;
	pn = node->getData();
	if (pn->getEnd() < start)	// Haven't reached first node yet.
		;								// Continue.
	else if (pn->getStart() > end)	// Passed last node in range.
		break;						// Wrap it up.
	else if (pn->getStart() < start || pn->getEnd() > end)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[labelOffsets: Error. Bad overlap of samples.]") << std::ends;
		::errOut(&gerrStr,false);
		return false;
		}
	else if (!first)				// Set first node offset.
		first = last = count;
	else
		++last;
	}

if (!first)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[labelOffsets: Error in finding sample.]") << std::ends;
	::errOut(&gerrStr,false);
	return false;
	}
return true;
}



/********************************************
* FN:		labelAttrs
* CR:		07/15/99 AM.
* SUBJ:	Get user-specified attributes from label into rule.
* NOTE:	Starting with the CLOSED attribute, where the user says that
*			a label represents a closed-set of items.  For example, given
*			"Amnon Meyers", the FirstName label could be specified as closed,
*			meaning, only accept words from samples, not any random alphabetic
*			token.
********************************************/

bool
Parse::labelAttrs(
	CONCEPT *label,		// Label name to add to layering.
	Irule *rule,			// Rule generated for current sample of conc.
	int first,				// First rule elt covered by label.
	int last,				// Last rule elt covered by label.
	bool h_closed,			// Closed-set default from Gram hierarchy.
	CG *cg
	)
{
if (!label || !rule || !first)
	return false;

// See what attributes are present in label.
// For now, get only the CLOSED attribute if present.
// 07/19/99 AM. If value present, overrided default from Gram hierarchy.
bool f_closed = false;
_TCHAR val[MAXSTR];
cg->findVal(label, _T("closed"), /*UP*/ val);
if (!_tcscmp(val, _T("true")))
	f_closed = true;
else if (!_tcscmp(val, _T("false")))
	f_closed = false;
else
	f_closed = h_closed;			// Use default value from Gram hierarchy.

// If only default settings found, then done.
if (!f_closed)
	return true;

// Traverse the rule elements, setting flags appropriately.

Dlist<Ielt> *list;
Delt<Ielt> *delt;
list = rule->getPhrase();
if (!(delt = list->getFirst()))
	return false;
int ii;
for (ii = 1; ii < first; ++ii)
	delt = delt->Right();
if (!delt)
	return false;
// Now we're at the first elt of covered by label.
// Process each rule elt covered by label.
Ielt *elt;
for (; delt && (ii <= last); delt = delt->Right(), ++ii)
	{
	// Handle rule elt.
	elt = delt->getData();
	if (f_closed)
		elt->setClosed(true);		// Set CLOSED flag for rule elt.
	}

return true;
}


/********************************************
* FN:		sortGroups
* CR:		06/15/99 AM.
* SUBJ:	Sort group actions in descending ord numbers.
* NOTE:	Crude bubble sort for now.
********************************************/

bool
Parse::sortGroups(
	Dlist<Iaction> *actions
	)
{
bool ok = true;

if (!actions)
	return ok;

Delt<Iaction> *list, *daction;

// Detach the list.
list = actions->getFirst();
actions->setFirst(0);
actions->setLast(0);

Delt<Iaction> *sorts = 0;
Delt<Iaction> *sptr = 0, *last = 0;
bool err = false;
int cmp = 0;

// Reattach to actions, one at a time.
while (list)
	{
	daction = list;
	list = list->Right();
	daction->setRight(0);
	daction->setLeft(0);

	// Traverse the reattached list
	if (!sorts)
		sorts = last = daction;
	else
		{
		sptr = sorts;
		cmp = cmpGroup(daction, sptr, /*UP*/ err);
		if (err)
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[Error in group actions.]") << std::ends;
			::errOut(&gerrStr,false);

			// Todo: Fix up or delete actions here.
			return false;
			}
		if (cmp >= 0)			// daction >= sptr.
			{
			// New start of list.
			daction->setRight(sptr);
			sptr->setLeft(daction);
			sorts = daction;
			}
		else
			{
			while ((sptr = sptr->Right()))
				{
				cmp = cmpGroup(daction, sptr, /*UP*/ err);
				if (err)
					{
					std::_t_strstream gerrStr;
					gerrStr << _T("[Error in group actions.]") << std::ends;
					::errOut(&gerrStr,false);

					// Todo: Fix up or delete actions here.
					return false;
					}
				if (cmp >= 0)			// daction >= sptr.
					{
					// Insert before sptr.
					daction->setRight(sptr);
					daction->setLeft(sptr->Left());
					sptr->Left()->setRight(daction);
					sptr->setLeft(daction);
					}
				}

			// If smallest, put at end of list.
			if (cmp < 0)
				{
				last->setRight(daction);
				daction->setLeft(sptr);
				last = daction;
				}
			}
		}
	}

// Reattach sorted list.
actions->setFirst(sorts);
actions->setLast(last);
return ok;
}


/********************************************
* FN:		cmpGroup
* CR:		06/15/99 AM.
* SUBJ:	Sort group actions in descending ord numbers.
* RET:	> 0 if dptr range > sptr range,  < 0 accordingly.
* NOTE:	Crude bubble sort for now.
*			If ranges overlap, error.
********************************************/

int Parse::cmpGroup(
	Delt<Iaction> *dptr,
	Delt<Iaction> *sptr,
	/*UP*/
	bool &err
	)
{
err = false;
if (!dptr || !sptr)
	{
	err = true;
	std::_t_strstream gerrStr;
	gerrStr << _T("[cmpGroup: Overlapping labels within a sample.]") << std::ends;
	::errOut(&gerrStr,false);
	return 0;
	}
Iaction *dact, *sact;
dact = dptr->getData();
sact = sptr->getData();

// Compare the first arg of each action.
Dlist<Iarg> *dargs, *sargs;
dargs = dact->getArgs();
sargs = sact->getArgs();
if (!dargs || !sargs)
	{
	err = true;
	return 0;
	}
Delt<Iarg> *sdelt, *ddelt;
Iarg *sarg, *darg;
ddelt = dargs->getFirst();
sdelt = sargs->getFirst();

darg = ddelt->getData();
sarg = sdelt->getData();

//if (darg->getType() != IASTR || sarg->getType() != IASTR)
if (darg->getType() != IANUM || sarg->getType() != IANUM)	// 12/28/99 AM.
	{
	err = true;
	return 0;
	}
long dnum,snum;
dnum = darg->getNum();													// 12/28/99 AM.
snum = sarg->getNum();													// 12/28/99 AM.
//if (!str_to_long(darg->getStr(), dnum))
//	{
//	err = true;
//	return 0;
//	}
//if (!str_to_long(sarg->getStr(), snum))
//	{
//	err = true;
//	return 0;
//	}
if (dnum > snum)
	return 1;
else if (dnum < snum)
	return -1;
else // dnum == snum
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[cmpGroup: Overlapping labels for one sample.]") << std::ends;
	::errOut(&gerrStr,false);
	err = true;
	return 0;
	}
}


/********************************************
* FN:		GENERALIZE
* CR:		06/28/99 AM.
* SUBJ:	Generalize and merge list of rules.
* NOTE:	Rules are from one Gram hierarchy concept's samples.
* WARN:	MODIFIES GIVEN RULE LIST.
* OPT:	Should only build rules if they are needed in the final product!
*********************************************/

Dlist<Irule> *Parse::generalize(
	Dlist<Irule> *list,			// List of RAW rules.
	Mode *mode
	)
{
if (!list)
	return 0;

////////////////////
// BUILD LITERAL VARIANTS OF THE RAW RULES.			// 10/29/99 AM.
////////////////////
Dlist<Irule> *literals = 0;								// 10/29/99 AM.
if (mode->getLit())											// 10/29/99 AM.
	{
	literals = buildLiterals(list);						// 10/29/99 AM.
	// OPT: Check here if any other types are being built,
	// because usually the answer is NO!				// 10/29/99 AM.
	if (mode->getConstrain()
		 || mode->getSplit()
		 || mode->getGen()
		 || mode->getRaw()	// opt: Could get these too and return.
		 )
		;							// Keep going.
	else
		{
		// UPDATE list HERE!!!!

		// Empty out RAW rules.
		Irule::deleteOwning(list);	// Remove actions.	// FIX.	// 12/24/00 AM.
		Dlist<Irule>::DelDlistAndData(list->getFirst());
		list->setFirst(0);
		list->setLast(0);

		internRug(literals);		// Intern preactions.
		Dlist<Irule>::merge(list, literals);

		// Build counter action for retained rules.			// 11/27/00 AM.
		if (mode->getCounter())										// 11/27/00 AM.
			Irule::rugCounter(list);								// 11/27/00 AM.

		if (!mode->getNotrig())										// 12/08/00 AM.
			Irule::trigRules(list);	// Build triggers.

		return list;			// DONE!
		}
	}

////////////////////
// BUILD THE MOST GENERIC POSSIBLE RULES FROM THE GIVEN LIST.
// The idea is to then merge these most generic variants.
// Each generic variant will point to all its RAW instances.
// (An internal variant of dedup will be called to do that.)
////////////////////
Dlist<Irule> *gens;				// The list of generic variants.

gens = new Dlist<Irule>();			// Empty list of generic rules.

Irule *rule;
Delt<Irule> *drule;
for (drule = list->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();

	// Build and rpush generalized version of current RAW rule.
	// (Points back to RAW rule.)
	buildGen(gens, rule, false);
	}

/// TEMPORARY.
// Attach the generalized rules to the full rule list.
// Later, merge these as below.
// Use them to build more constrained rules.
// Perhaps give them such a confidence that they won't be executed.
// (Or comment them out at first.)
// Merge the two lists:
//Dlist<Irule>::merge(list, gens);


////////////////////
// DEDUP THE GENERALIZED RULES.
// Whenever a rule is dedup'ed, merge the list of pointers to the
// RAW variants.
////////////////////
mergeGen(gens);

////////////////////
// SPLIT UP GENERALIZED RULES BASED ON LABELING CONFLICTS
// AND LABELING AMBIGUITIES.			// 07/15/99 AM.
////////////////////
Dlist<Irule> *splits;				// The list of split up generic variants.
splits = new Dlist<Irule>();		// Empty list.
splitGens(gens, /*DU*/ splits);	// 07/15/99 AM.

////////////////////
// BUILD THE MOST CONSTRAINED VARIANTS THAT ENCOMPASS ALL THE
// RAW RULES.
////////////////////
Dlist<Irule> *constrains = 0;
// 10/31/99 AM. Moving this below.
//constrains = maxConstrains(splits);


////////////////////
// FIX LABELS IN RULES.	// 07/13/99 AM.
////////////////////
// 10/31/99 AM. Moving this below.
//fixConstrains(constrains);			// 07/13/99 AM.

////////////////////
// PRUNE RULES THAT ARE TOO GENERAL.
////////////////////
// 08/09/99 AM. ELIMINATING THIS HEURISTIC.
// USER MUST ASSURE THAT RULES ARE REASONABLE OR THAT CONTEXT IS
// CONSTRAINED.
// 10/31/99 AM. Moving this below.
// 08/31/99 AM. Premature to remove this sanity check!
//pruneConstrains(constrains);		// 08/31/99 AM.

////////////////////
// INTERN THE PRE ACTIONS.
// They were built at the rule level.  Need to be copied into
// rule elts.							// 07/08/99 AM.
////////////////////
// 10/31/99 AM. OPT. Moving these below.
//internRug(list);		// FIX // 11/01/99 AM. 
	// Was messing up the pre actions by interning them before constrain
	// work.  Don't need these rules interned anyway.
//internRug(constrains);
//internRug(splits);	// Comment this out if not using splits.
//internRug(gens);		// Comment this out if not using gens.

if (mode->getConstrain())
	{
	constrains = maxConstrains(splits);					// 10/31/99 AM.
	fixConstrains(constrains);								// 10/31/99 AM.
	pruneConstrains(constrains);							// 10/31/99 AM.
	internRug(constrains);									// 10/31/99 AM.
	}
if (mode->getSplit())										// 10/31/99 AM.
	internRug(splits);										// 10/31/99 AM.
if (mode->getGen())											// 10/31/99 AM.
	internRug(gens);											// 10/31/99 AM.
if (mode->getLit())											// 10/31/99 AM.
	internRug(literals);										// 10/31/99 AM.


////////////////////
// BUILD A MIXING VARIANT OF THE RULES.
////////////////////


////////////////////
// GLOM THE GOOD RULES BUILT ONTO GIVEN LIST.
////////////////////

// Use the mode information.			// 07/19/99 AM.

if (!mode->getRaw())				// DISCARD RAW RULES.		// 10/29/99 AM.
	{
	Irule::deleteOwning(list);									// FIX.	// 12/24/00 AM.
	Dlist<Irule>::DelDlistAndData(list->getFirst());
	list->setFirst(0);
	list->setLast(0);
	}

if (mode->getLit())											// 10/29/99 AM.
	Dlist<Irule>::merge(list, literals);				// 10/29/99 AM.
else																// 10/29/99 AM.
	{
	Irule::deleteOwning(literals);							// FIX.	// 12/24/00 AM.
	Dlist<Irule>::DeleteDlistAndData(literals);		// 10/29/99 AM.
	literals = 0;																// 12/25/00 AM.
	}

if (mode->getConstrain())
	Dlist<Irule>::merge(list, constrains);
else
	{
	Irule::deleteOwning(constrains);							// FIX.	// 12/24/00 AM.
	Dlist<Irule>::DeleteDlistAndData(constrains);
	constrains = 0;															// 12/25/00 AM.
	}

if (mode->getSplit())
	Dlist<Irule>::merge(list, splits);
else
	{
	Irule::deleteOwning(splits);								// FIX.	// 12/24/00 AM.
	Dlist<Irule>::DeleteDlistAndData(splits);
	splits = 0;																	// 12/25/00 AM.
	}

if (mode->getGen())
	Dlist<Irule>::merge(list,gens);
else
	{
	Irule::deleteOwning(gens);									// FIX.	// 12/24/00 AM.
	Dlist<Irule>::DeleteDlistAndData(gens);
	gens = 0;																	// 12/25/00 AM.
	}

// BUILD QUICK SEMANTICS FOR THE RETAINED RULES.	// 12/27/99 AM.
if (mode->getQuicksem())									// 12/27/99 AM.
   Irule::quickSem(list);									// 12/27/99 AM.

// Build counter action for retained rules.			// 11/27/00 AM.
if (mode->getCounter())										// 11/27/00 AM.
	Irule::rugCounter(list);								// 11/27/00 AM.

// BUILD TRIGGERS FOR THE RETAINED RULES.				// 10/07/99 AM.
if (!mode->getNotrig())										// 12/08/00 AM.
	Irule::trigRules(list);									// 10/07/99 AM.

return list;
}


/********************************************
* FN:		BUILDGEN
* CR:		06/30/99 AM.
* SUBJ:	Generalize one rule.
* NOTE:	07/16/99 AM. The "splits" generalized rules will hold attr
*			(ie, LABEL) information.  The most generalized rules do not.
*********************************************/

bool Parse::buildGen(
	Dlist<Irule> *gens,		// List of generalized rules.
	Irule *rule,					// RAW rule.
	bool f_attrs		// If copying attrs to general rule.	// 07/16/99 AM.
	)
{
if (!gens || !rule)
	return false;

// Convenience variables for new rule.
Dlist<Ielt> *phr = 0;						// Rule phrase.
Isugg *sugg = 0;								// Suggested elt.
Dlist<Ipre> *pres = 0;					// List of pre actions.
Dlist<Iaction> *checks = 0;				// List of check actions.
Dlist<Iaction> *posts = 0;					// List of post actions.
Delt<Ielt> *trig = 0;						// Trigger elt.
Dlist<Iarg> *attrs = 0;
Iarg *arg = 0;
Dlist<Ipair> *pairs = 0;
int min = 0;
int max = 0;
_TCHAR *ren = 0;
_TCHAR *gp = 0;																	// 09/23/00 AM.

phr = new Dlist<Ielt>();						// Create rule phrase.

Dlist<Ielt> *lphr;
Delt<Ielt> *delt;
Ielt *elt;
lphr = rule->getPhrase();
_TCHAR *name;
Dlist<Iarg> *matches;
Dlist<Iarg> *lattrs = 0;
Delt<Ielt> *ndelt;
Ielt *nelt;			// New elt.
bool sing = true;		// 08/09/99 AM.
Delt<Iarg> *darg;
for (delt = lphr->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	name = elt->getName();
	pairs = elt->getPairs();
	elt->getMinmax(/*UP*/ min, max);	// 10/06/99 AM.
	ren = elt->getRename();
	gp = elt->getGroup();				// 09/23/00 AM.
	sing = elt->getSinglet();			// 08/09/99 AM.

	if (f_attrs)							// 07/16/99 AM.
		attrs = elt->getAttrs();

	// Must copy elements attrs.
	if (attrs)
		lattrs = new Dlist<Iarg>(*attrs);		// COPY LIST.
	else
		lattrs = 0;

	if (str_equal(name, _T("_xWILD")) && (matches = elt->getMatches()))
		{
		// If there's a token variant, get the token type and generalize.
		//*gerr << "buildGen: Not handling match list yet.]" << std::endl;
		//return false;

		// 07/11/99 AM. Generalize if token type found.
		// 11/24/00 AM. HANDLING LIST CONTAINING NONLITERALS ONLY.
		_TCHAR *nam;
		if ((nam = elt->litName()))										// 11/24/00 AM.
			{
			if (!(name = tok_type(nam)))
				{
				if (phr)															// 12/28/00 AM.
				Dlist<Ielt>::DeleteDlistAndData(phr);// MEM LEAK.	// 12/28/00 AM.
				if (lattrs)														// 12/28/00 AM.
				Dlist<Iarg>::DeleteDlistAndData(lattrs);// MEM LEAK.// 12/28/00 AM.
				return false;
				}
			}
		else
			{
			// Take the first nonliteral as name.  Presumably grammar
			// has been built so most generic is first.
			darg = matches->getFirst();									// 11/24/00 AM.
			arg = darg->getData();											// 11/24/00 AM.
			name = arg->getStr();											// 11/24/00 AM.
			}
		}
	else if (*name == '_')			// Nonliteral elt.
		{
		// Keep name as is.
		}
	else			// Literal elt, so get token type and generalize.
		{
		if (!(name = tok_type(name)))		// 07/03/99 AM.
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("buildGen: Unknown token type.") << std::ends;
			::errOut(&gerrStr,false);

			if (phr)																// 12/28/00 AM.
				Dlist<Ielt>::DeleteDlistAndData(phr);// MEM LEAK.	// 12/28/00 AM.
			if (lattrs)															// 12/28/00 AM.
				Dlist<Iarg>::DeleteDlistAndData(lattrs);// MEM LEAK.// 12/28/00 AM.
			return false;
			}
		}

	ndelt = Ielt::addDelt(phr, name, pairs, min, max, ren);
	nelt = ndelt->getData();
	nelt->setAttrs(lattrs);
	nelt->setSinglet(sing);			// 08/09/99 AM.
	nelt->setGroup(gp);				// 09/23/00 AM.
	}


Isugg *lsugg = 0;
//char *lstr = 0;
//bool lbase = 0;
//Dlist<Ipair> *lpairs = 0;
Dlist<Iaction> *lposts = 0;

// RUG RULES DO OWN THEIR ACTIONS.										// 12/25/00 AM.
// Rule doesn't own it's own actions, so just take a pointer.
if (f_attrs																		// 07/16/99 AM.
	 && rule->getPost())														// 12/25/00 AM.
//	lposts = rule->getPost();												// 12/25/00 AM.
	{
	lposts = new Dlist<Iaction>(*(rule->getPost()));				// 12/25/00 AM.
	}

lsugg = rule->getSugg();
sugg = new Isugg(*lsugg);	// Copy constr			// FIX		// 12/11/99 AM.

//lstr = lsugg->getName();								// FIX		// 12/11/99 AM.
//lattrs = lsugg->getAttrs();							// FIX		// 12/11/99 AM.
//lbase = lsugg->getBase();							// FIX		// 12/11/99 AM.
//lpairs = lsugg->getPairs();							// FIX		// 12/11/99 AM.

//sugg = new Isugg(lstr);								// FIX		// 12/11/99 AM.
//sugg->setAttrs(lattrs);								// FIX		// 12/11/99 AM.
//sugg->setBase(lbase);									// FIX		// 12/11/99 AM.
//sugg->setPairs(lpairs);								// FIX		// 12/11/99 AM.

// BUILD A LIST OF THE RAW RULE.

Irule *grule;
Delt<Irule> *tmp;
tmp = Irule::addDelt(gens, phr, sugg, pres, checks, posts, trig);
grule = tmp->getData();

// POINT TO THE RAW RULE.
Slist<Irule> *slist;

slist = new Slist<Irule>(rule);	// Build list of one rule.
grule->setDown(slist);		// POINT TO THE RAW RULE.
grule->setPost(lposts);
grule->setSample(rule->getSample());			// For fun.
return true;
}


/********************************************
* FN:		MERGEGEN
* CR:		06/30/99 AM.
* SUBJ:	Merge the generalized rules, removing duplicates.
* NOTE:	Copies Irule::dedup.  Merges the Down pointers to RAW
*			rules.
*			N-squared "bubble" algorithm.
* WARN:	MODIFIES GIVEN RULE LIST.
*********************************************/

Dlist<Irule> *Parse::mergeGen(Dlist<Irule> *list)
{
if (!list)
	return 0;

Irule *rule;
Delt<Irule> *drule, *test, *dnext;
Irule *crule;			// Current rule.
Slist<Irule> *nlist;
for (drule = list->getFirst(); drule; drule = drule->Right())
	{
	// Traverse all subsequent rules, looking for redundancy with
	// current rule.
	test = drule->Right();
	while (test)
		{
		dnext = test->Right();	// Get next guy ahead of time.
		rule = test->getData();
		crule = drule->getData();
		if (Irule::same(crule, rule))
			{
			// Remove redundant rule.
			//*gerr << "[FOUND REDUNDANT!]" << std::endl;
			list->Dlist<Irule>::exciseElt(test);

			// Transfer the DOWN list from the excised rule.
			nlist = Slist<Irule>::merge(crule->getDown(), rule->getDown());
			crule->setDown(nlist);			// The merged list.
			rule->setDown(0);					// Clear this out.

			Irule::deleteOwning(rule);					// 07/03/99 AM.
			Delt<Irule>::DeleteDeltAndData(test);	// 07/03/99 AM.
			//delete test;									// 07/03/99 AM.
			}
		test = dnext;
		}
	}

return list;
}


/********************************************
* FN:		SPLITGENS
* CR:		07/15/99 AM.
* SUBJ:	Split generalized rules based on labeling.
* RET:	True if ok.
*			UP splits - List of split up generalized rules.
* NOTE:	Splits based on labeling conflicts and labeling ambiguity.
*			The latter occurs when a RAW rule's labeling can be
*			merged into more than one generalized rule (from the split set).
*			The gens list of rules is left untouched.  (Todo: Each gen could
*			point to the list of splits that was spawned by it.)
* ALG:	Sort the RAW rules list in order of decreasing number of
*			labels per rule.  Create new generalized rule when a RAW
*			rule can't be merged into one of the existing splits because of
*			a labeling conflict.  Also create a new split if the RAW rule
*			can be merged into more than one of the splits rules (ambiguity).
* WARN:	MODIFIES GIVEN SPLITS LIST.
*********************************************/

bool Parse::splitGens(
	Dlist<Irule> *gens,		// The most general possible rules.
	/*DU*/
	Dlist<Irule> *splits		// DN: Empty list to fill.
									// UP: Generalized rules split due to labeling.
	)
{
if (!gens || !splits)
	return false;

Delt<Irule> *dgen;
bool ok = true;				// If all splits succeeded.
for (dgen = gens->getFirst(); dgen; dgen = dgen->Right())
	{
	if (!splitGen(dgen->getData(), /*DU*/ splits))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[splitGens: Error in splitting.]") << std::ends;
		::errOut(&gerrStr,false);
		ok = false;
		}
	}
return ok;
}


/********************************************
* FN:		SPLITGEN
* CR:		07/16/99 AM.
* SUBJ:	Split generalized rule based on labeling.
* RET:	True if ok.
*			UP splits - List of split up generalized rules.
* NOTE:	See SPLITGENS documentation.
* WARN:	MODIFIES GIVEN SPLITS LIST.
*********************************************/

bool Parse::splitGen(
	Irule *gen,					// An overgeneralized rule that represents
									// a list of RAW rules.
	/*DU*/
	Dlist<Irule> *splits		// DN: Splits list built so far.
									// UP: List with new splits for current gen.
	)
{
if (!gen || !splits)
	return false;

// Get RAW rules represented by the overgeneralized rule.
Slist<Irule> *raws = gen->getDown();
if (!raws)
	return false;

// Sort RAW rules in order of decreasing number of labels.
sortRulesbyLabels(raws);

// Make a new split list for the current generalized rule.
Dlist<Irule> *gsplits = new Dlist<Irule>();	// Empty list.

// Traverse RAW rules.
Selt<Irule> *selt;
Irule *raw;
for (selt = raws->getFirst(); selt; selt = selt->Right())
	{
	raw = selt->getData();

	// Fold RAW rule into a unique split (generalized rule) or else
	// built a new split for the RAW rule.
	splitRaw(raw, gsplits);

	// If current RAW doesn't fold into a split, create a new split
	// for it.

	// If current RAW folds into more than one split, flag error and
	// create a new split for it.

	// If current RAW folds into exactly one split, then it belongs
	// to that split.
	}

// Merge newly created splits into overall splits list.
Dlist<Irule>::merge(splits, gsplits);

return true;
}


/********************************************
* FN:		SPLITRAW
* CR:		07/16/99 AM.
* SUBJ:	See which generalized rules (splits) a RAW rule folds into.
* RET:	True if ok.
* NOTE:	If into no splits, create a new one for the RAW rule.
*			If into exactly one, fold into it.
*			If into more than one, this is ambiguous.  Resolve by creating
*			a new split.
*			The splits rules, unlike the most generic rules, contain label
*			information also.
*			10/29/99 AM. Rename from splitLit
* WARN:	MODIFIES GIVEN SPLITS LIST.
*********************************************/

bool Parse::splitRaw(
	Irule *raw,					// RAW rule.
	/*DU*/
	Dlist<Irule> *splits		// DN: Splits list built so far.
									// UP: List with RAW in new or existing split.
	)
{
if (!raw || !splits)
	return false;

Irule *good = 0;			// A split that the RAW rule folds into.

// Traverse the splits, looking to see how many the RAW rule
// folds into without any labeling conflicts.
Delt<Irule> *delt;
Irule *split;
for (delt = splits->getFirst(); delt; delt = delt->Right())
	{
	split = delt->getData();
	if (splitFold(raw, split))		// If RAW folds into general rule.
		{
		if (good)				// Already folded into another one.
			{
			// Todo: Could set up to warn user about sample that is
			// ambiguous in that it can merge with multiple samples that
			// conflict with each other.  (That is, the sample should be
			// completely labeled if it is not so.)

			// Recover by creating a new split for the current RAW rule.
			good = 0;
			break;
			}
		else
			good = split;		// RAW folds into this general rule.
		}
	}

if (good)					// RAW folded into a UNIQUE general rule.
	{
	// Simply add a pointer to the RAW rule.
	Slist<Irule> *downs = good->getDown();
	downs->rpush(raw);
	}
else // if (!good)		// RAW folds into zero or multiple splits.
	{
	//  Build a new generalized rule for the RAW rule.
	// Add it to the list.
	// (Or, if there were no splits at all, make the first one.)
	buildGen(splits, raw, true);
	}

return true;
}


/********************************************
* FN:		SPLITFOLD
* CR:		07/16/99 AM.
* SUBJ:	See if RAW rule folds into generalized rule.
* RET:	True if it folds, false if it doesn't.
* NOTE:	RAW rule folds in if all of its LABELS (ie element attrs)
*			and GROUP post actions exist in the generalized rule.  If the
*			RAW has anything extra, then it can't fold in.
*			Yet another hairy rule traversal and comparison.
*********************************************/

bool Parse::splitFold(
	Irule *raw,					// RAW rule.
	Irule *split				// Generalized rule (with LABEL info).
	)
{
if (!raw || !split)
	return false;

Dlist<Ielt> *rphrase = raw->getPhrase();
Dlist<Ielt> *sphrase = split->getPhrase();
if (!splitFoldphrase(rphrase, sphrase))
	return false;

Dlist<Iaction> *rposts = raw->getPost();
Dlist<Iaction> *sposts = split->getPost();
if (!splitFoldposts(rposts, sposts))
	return false;
return true;
}


/********************************************
* FN:		SPLITFOLDPHRASE
* CR:		07/16/99 AM.
* SUBJ:	See if RAW rule's phrase folds into generalized rule's.
* RET:	True if it folds, false if it doesn't.
* NOTE:	
*********************************************/

bool Parse::splitFoldphrase(
	Dlist<Ielt> *rphrase,	// RAW rule's phrase.
	Dlist<Ielt> *sphrase		// Generalized rule's phrase.
	)
{
if (!rphrase || !sphrase)
	return false;

Delt<Ielt> *sdelt, *rdelt;
for (sdelt = sphrase->getFirst(), rdelt = rphrase->getFirst();
	  sdelt && rdelt;
	  sdelt = sdelt->Right(), rdelt = rdelt->Right())
	{
	if (!splitFoldelt(rdelt->getData(), sdelt->getData()))
		return false;
	}
if (sdelt || rdelt)
	return false;			// Should be same length.
return true;
}


/********************************************
* FN:		SPLITFOLDELT
* CR:		07/16/99 AM.
* SUBJ:	See if RAW rule's phrase folds into generalized rule's.
* RET:	True if it folds, false if it doesn't.
* NOTE:	
*********************************************/

bool Parse::splitFoldelt(
	Ielt *relt,						// RAW rule elt.
	Ielt *selt						// Generalized rule elt.
	)
{
if (!relt || !selt)
	return false;

// Not comparing elt names.  Assuming this is hunkydory for now.
//if (!splitFoldeltname(relt, selt))
//	return false;

// Compare elt properties.

// Check on consistent layering.
Dlist<Iarg> *sattrs = selt->getAttrs();
Dlist<Iarg> *rattrs = relt->getAttrs();
if (sattrs && rattrs)
	{
	// Layering had better match.
	// NOTE: ELSE, RAW RULE WON'T MATCH THIS GENERALIZED RULE.
	if (!Iarg::same(sattrs, rattrs))
		return false;
	}
else if (!sattrs && !rattrs)
	{
	// Done.
	}
else if (sattrs)
	{
	// Ok for the generalized rule to have a superset of layering.
	}
else	// if (rattrs)
	{
	// NOT OK for RAW rule to have extra layering.
	return false;
	}

return true;
}


/********************************************
* FN:		SPLITFOLDELTNAME
* CR:		07/16/99 AM.
* SUBJ:	See if RAW rule elt name folds into generalize rule's.
* RET:	True if it folds, false if it doesn't.
* NOTE:	This is not a full treatment, but will lead to obvious
*			splits when element names don't align with the generic rule.
*			(Not complete because the RAW rules may match different
*			parts of the generic rule's names. Hmmm, quite a problem)
*********************************************/

bool Parse::splitFoldeltname(
	Ielt *relt,						// RAW rule elt.
	Ielt *selt						// Generalized rule elt.
	)
{
if (!relt || !selt)
	return false;

_TCHAR *sname = selt->getName();
_TCHAR *rname = relt->getName();

Dlist<Iarg> *smatches = selt->getMatches();
Dlist<Iarg> *rmatches = relt->getMatches();
if (!smatches && !rmatches)
	{
	// If names identical, done.
	if (!_tcscmp(sname,rname))
		return true;

	// MATCHING RAW ELT WITH GENERIC TYPE.						// 08/09/99 AM.
	if (!_tcscmp(_T("_xALPHA"), sname) && unicu::isAlphabetic(*rname))	// 09/22/99 AM.
		return true;
	if (!_tcscmp(_T("_xEMOJI"), sname) && unicu::isEmoji((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xNUM"), sname) && unicu::isDigit((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xPUNCT"), sname) && unicu::isPunct((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xWHITE"), sname) && unicu::isWhiteSpace((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xBLANK"), sname) && unicu::isWhiteSpace((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xEMOJI"), sname) && unicu::isEmoji((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xCTRL"), sname) && _istcntrl((_TUCHAR)*rname))				// 07/24/00 AM.
		return true;															// 07/24/00 AM.

	// Else if any are nonliteral, can't reconcile.
	if (*sname == '_' || *rname == '_')
		return false;
	}
else if (smatches && rmatches)
	{
	// Look for a common name or common match.
	Iarg *arg;
	// Names should both be "_xWILD".

	// Find a common list element, if any.
	Delt<Iarg> *darg;
	_TCHAR *s_arg;
	for (darg = smatches->getFirst(); darg; darg = darg->Right())
		{
		arg = darg->getData();
		s_arg = arg->getStr();
		if (Iarg::findArg(rmatches, s_arg))
			return true;				// Found a common name.
		if (intersectKinds(rmatches, s_arg))	// 08/09/99 AM.
			return true;
		}
	}
else if (smatches)
	{
	// If rname is present, matched.
	if (Iarg::findArg(smatches, rname))
		return true;
	}
else // if (rmatches)
	{
	// If sname is present, matched.
	if (Iarg::findArg(rmatches, sname))
		return true;
	if (intersectKinds(rmatches, sname))	// 08/09/99 AM.
		return true;
	}

// If both have literal token, should be same type.
//return splitfoldElttoken(selt,relt);

// If all commonality failed, should return false.		// 08/09/99 AM.
//return true;			// 08/09/99 AM.
return false;			// 08/09/99 AM.
}


/********************************************
* FN:		SPLITFOLDPOSTS
* CR:		07/16/99 AM.
* SUBJ:	See if RAW rule's posts fold into generalized rule's.
* RET:	True if they fold, false if they don't.
* NOTE:	
*********************************************/

bool Parse::splitFoldposts(
	Dlist<Iaction> *rposts,		// RAW rule's posts.
	Dlist<Iaction> *sposts		// Generalized rule's posts.
	)
{
if (!rposts && !sposts)		// Default post actions for both.  ok.
	return true;
else if (sposts && rposts)
	{
	// TODO: Should do better checking, but:
	// Quick and dirty.  Compare lengths of the lists.
	int slen = sposts->getLength();
	int rlen = rposts->getLength();
	if (rlen > slen)
		return false;
	}
else if (sposts)
	return true;
else // if (rposts)
	return false;
return true;
}

/********************************************
* FN:		SORTRULESBYLABELS
* CR:		07/16/99 AM.
* SUBJ:	Sort given rules list by decreasing number of labels.
* RET:	True if ok.
* NOTE:	Could do a fast bucket sort if we're willing to set a maximum
*			number of labels.
*			Need to keep count of labels in a rule, so won't have to keep
*			traversing a rule to get the count.
* WARN:	MODIFIES GIVEN SPLITS LIST.
*********************************************/

bool Parse::sortRulesbyLabels(
	Slist<Irule> *rules
	)
{
if (!rules)
	return false;

Selt<Irule> *selts;
if (!(selts = rules->getFirst()))
	return true;

// Create an array.
long len = 0;
Selt<Irule> **arr;
len = rules->getLength();
arr = new Selt<Irule> * [len];

// Make array elts point to rules elts.
Selt<Irule> **parr = 0;
Selt<Irule> *pselts = 0;
for (pselts = selts, parr = arr;
	  pselts;
	  pselts = pselts->Right(), ++parr
	  )
	{
	*parr = pselts;
	}

// Sort array.
qsort((void *)arr, (size_t)len, sizeof(Selt<Irule> *), cmpRulelabels);

// Relink rules.
parr = arr;
while (--len > 0)			// For all but last elt.
	{
	(*parr)->setRight(*(parr+1));
	++parr;
	}
// Fix up the last element.
(*parr)->setRight(0);

// Reset rules object.
rules->setFirst(*arr);
rules->setLast(*parr);

// Trash the array.
delete [] arr;
return true;
}


/********************************************
* FN:		CMPRULELABELS
* CR:		07/16/99 AM.
* SUBJ:	Sort two rule list-elements by number of labels in rules.
* NOTE:	Sorting in DECREASING order.
********************************************/

int Parse::cmpRulelabels(const void *arg1, const void *arg2)
{
Selt<Irule> **a1, **a2;

a1 = (Selt<Irule> **) arg1;
a2 = (Selt<Irule> **) arg2;

Irule *rule1, *rule2;
rule1 = (*a1)->getData();
rule2 = (*a2)->getData();

int n1, n2;
n1 = rule1->getNlabels();
n2 = rule2->getNlabels();

if (n1 > n2)
	return -1;
else if (n1 < n2)
	return 1;
else // n1 == n2.
	return 0;
}


/********************************************
* FN:		MAXCONSTRAINS
* CR:		06/30/99 AM.
* SUBJ:	Build a list of maximally constrained rules.
* RET:	NEW LIST of maximally constrained rules.
* ALG:	Given a list of the most generalized rules, each of which points
*			to a list of RAW rules.  Traverse the gens.  For each one,
*			traverse its list of RAW rules.  Build a single rule that
*			contains the maximal overlap and specialization of features in
*			the entire list of RAW rules.
*********************************************/

Dlist<Irule> *Parse::maxConstrains(
	Dlist<Irule> *gens		// List of generalized rules.
	)
{
Dlist<Irule> *constrs;

if (!gens)
	return 0;

constrs = new Dlist<Irule>();			// Empty list of new rules.

Delt<Irule> *dgens;
for (dgens = gens->getFirst(); dgens; dgens = dgens->Right())
	{
	maxConstrain(constrs, dgens->getData());
	}

return constrs;
}


/********************************************
* FN:		MAXCONSTRAIN
* CR:		06/30/99 AM.
* SUBJ:	Build a single maximally constrained rule.
* RET:	constrs - append to list of constrained rules.
* NOTE:	07/16/99 AM. The split work assures that first RAW rule
*			in list is maximally labeled (and should have attributes like
*			CLOSED).
*********************************************/

Dlist<Irule> *Parse::maxConstrain(
	Dlist<Irule> *constrs,		// List of constrained rules built so far.
	Irule *gen						// One generalized rule.
	)
{
if (!constrs || !gen)
	return 0;

Slist<Irule> *list = 0;			// List of RAW rules.
Selt<Irule> *raws = 0;
Irule *crule = 0;
list = gen->getDown();
if (!list)
	return 0;

// Grab the first RAW rule.  Copy it, making it the first cut of
// the constrained rule.
raws = list->getFirst();
if (!raws)
	return 0;
crule = raws->getData()->copy_owning();	// COPY RULE.

// Copy the list of RAW rules to the constrained rule being
// built.  // 07/13/99 AM.
crule->setDown(new Slist<Irule>(*list));	// FIX	// 07/14/99 AM.

// Fix up the CLOSED-SET elements of rule, etc.		// 07/16/99 AM.
if (!setupConstrain(crule))								// 07/16/99 AM.
	return 0;

// Traverse the RAW RULES.  Fold each RAW rule into the constrained
// rule being constructed.  Generalize or "loosen" the constrained rule
// as needed by each new RAW rule.
for (raws = raws->Right(); raws; raws = raws->Right())
	{
	foldConstrain(crule, raws->getData());
	}

// Add the constrained rule to the list of rules.
constrs->rpush(crule);
return constrs;
}


/********************************************
* FN:		SETUPCONSTRAIN
* CR:		07/16/99 AM.
* SUBJ:	Fix up things like the closed-set attribute in rule.
* RET:	True if ok.
*********************************************/

bool Parse::setupConstrain(
	Irule *rule			// Maximally constrained rule being built.
	)
{
if (!rule)
	return false;
// For each elt that has a closed-set attribute, get its token and
// convert to an _xWILD with matches list.
Dlist<Ielt> *phrase = rule->getPhrase();
if (!phrase)
	return false;
Delt<Ielt> *delt;
Ielt *elt;
_TCHAR *name;
Dlist<Iarg> *matches;
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	if (elt->getClosed())
		{
		if (!(name = elt->litName()))
			return true;				// FIX.	// 08/23/99 AM.
		// Trash the matches if any, but reuse list object.
		if ((matches = elt->getMatches()))
			{
			Dlist<Iarg>::DelDlistAndData(matches->getFirst());
			matches->setFirst(0);
			matches->setLast(0);
			// Make elt into a wildcard with match list.
			matches->rpush(new Iarg(name));
			}
		else			// Make a match list.
			{
			matches = new Dlist<Iarg>(new Iarg(name));
			elt->setMatches(matches);
			}
		elt->setName(_T("_xWILD"));
		}
	}
return true;
}


/********************************************
* FN:		FOLDCONSTRAIN
* CR:		07/03/99 AM.
* SUBJ:	Unify constraints from RAW rule into maximally constrained rule.
* RET:	True if ok.
*			UP crule - Maximally constrained rule as "loosened" by given
*			RAW rule.
* EX:		For example, if the maximally constrained rule says a phone
*			number is "343-5678" and the RAW rule is "123-4567", can
*			retain that phone number is #-# with first being len=3, snd length
*			= 4.
*********************************************/

bool Parse::foldConstrain(
	Irule *crule,			// Maximally constrained rule being built.
	Irule *rrule			// RAW sample to be folded in.
	)
{
if (!crule || !rrule)
	return false;

// Ignore the suggested element.  (Assume they're identical).

// Compare the phrase elements.  Assume the phrases elements align
// precisely and that phrases are identical length.  Elements may
// also be of the form _xWILD [match=(....)]
// Determine elt type also (alpha,num,punct,white).

// Compare the attributes on the phrase elements.  Especially looking
// at LAYER attributes.  If true, this means that the rule has some
// EXTRA layering that the other rule is lacking.  If both flags get
// set, then we'll reject this merger of samples.
bool cextra = false;			// If constrained rule had extra layering.
bool rextra = false;			// If RAW rule had full layering.
if (!foldPhrase(crule->getPhrase(), rrule->getPhrase(),
		/*UP*/ cextra, rextra))
	return false;

// Compare the POST actions.  Looking only at the GROUP actions to
// make sure they are compatible.  If one rule has none and one has
// all, get them all folded into the constrained rule.  (This works
// together with the LAYER attribute on the phrase elements.)  If both
// have full complement, they'd better be identical.
if (!foldPost(crule, rrule, cextra, rextra))
	return false;

// Compare the PRE actions that constrain the elements.
if (!foldPres(crule, rrule))
	return false;

return true;
}


/********************************************
* FN:		FOLDPHRASE
* CR:		07/03/99 AM.
* SUBJ:	Unify phrase from RAW rule into maximally constrained rule.
* RET:	True if ok.
*			UP cfull - If constrained rule was layered.
*			UP rfull - If RAW rule was layered.
*			MODIFIES CPHRASE.
* NOTE:	07/16/99 AM. Assume constrained phrase has the attributes like
*			CLOSED, if present at all.
*********************************************/

bool Parse::foldPhrase(
	Dlist<Ielt> *cphrase,		// Phrase of constrained rule.
	Dlist<Ielt> *rphrase,		// Phrase of RAW rule.
	/*UP*/
	bool &cextra,					// If constrained rule has extra labels.
	bool &rextra					// If RAW rule has extra labels.
	)
{
cextra = rextra = false;			// No layering seen yet.
if (!cphrase || !rphrase)
	return false;

Delt<Ielt> *cdelt, *rdelt;
for (cdelt = cphrase->getFirst(), rdelt = rphrase->getFirst();
	  cdelt && rdelt;
	  cdelt = cdelt->Right(), rdelt = rdelt->Right())
	{
	if (!foldElt(cdelt->getData(), rdelt->getData(), cextra, rextra))
		return false;
	}
if (cdelt || rdelt)
	return false;			// Should be same length.
return true;				// Passed all hurdles.
}


/********************************************
* FN:		FOLDELT
* CR:		07/03/99 AM.
* SUBJ:	Unify elt from RAW rule into maximally constrained rule.
* RET:	True if ok.
*			UP cfull - If constrained rule was layered.
*			UP rfull - If RAW rule was layered.
*			MODIFIES CELT.
* WARN:	Requiring user to do full labeling or NO labeling on each sample.
*			The main situation I want to guard against is that there isn't
*			interleaving of labels (some from one rule, some from another).
*			One rule is expected to contain the full complement of labeling.
*			If others have dribs and drabs, that's ok, but those will be
*			compared to make sure they are identical to the fully labelled
*			case.  (User can fool the algorithm used here if he really wants
*			to).
*********************************************/

bool Parse::foldElt(
	Ielt *celt,					// Phrase elt of constrained rule.
	Ielt *relt,					// Phrase elt of RAW rule.
	/*DU*/
	bool &cextra,				// If constrained rule was layered.
	bool &rextra				// If RAW rule was layered.
	)
{
if (!celt || !relt)
	return false;

// Look for generalized whitespace.	// FIX	// 08/23/99 AM.
if (!_tcscmp(celt->getName(), _T("_xWHITE"))		// 08/23/99 AM.
    && !_tcscmp(relt->getName(), _T("_xWHITE")))	// 08/23/99 AM.
	return true;										// 08/23/99 AM.

// Compare elt names.
if (celt->getClosed())			// A closed-set elt.		// 07/16/99 AM.
	{
	// Fold closed-set element.
	_TCHAR *name;
	if ((name = relt->litName()))
		{
		Dlist<Iarg> *matches;
		if (!(matches = celt->getMatches()))
			{
			std::_t_strstream gerrStr;
			gerrStr << _T("[foldElt: Closed-set folding error.]") << std::ends;
			::errOut(&gerrStr,false);

			// Should have a list built.  Recover by building it.
			_TCHAR *cname = celt->getName();	// FIX	// 08/23/99 AM.
			matches = new Dlist<Iarg>(new Iarg(cname));// 08/23/99 AM.
			celt->setName(_T("_xWILD"));						// 08/23/99 AM.
			celt->setMatches(matches);						// 08/23/99 AM.
			//return false;						// FIX	// 08/23/99 AM.
			}
		// MAKE SURE NAME IS NOT ALREADY IN LIST.	FIX.	// 08/09/99 AM.
		if (!Iarg::findArg(matches, name))					// 08/09/99 AM.
			matches->rpush(new Iarg(name));
		}
	else
		return false;
	}
else if (!foldEltname(celt, relt))
	return false;

// Compare elt properties.

// Check on consistent layering.
Dlist<Iarg> *cattrs = celt->getAttrs();
Dlist<Iarg> *rattrs = relt->getAttrs();
if (cattrs && rattrs)
	{
	// Layering had better match.
	if (!Iarg::same(cattrs, rattrs))
		return false;
	}
else if (!cattrs && !rattrs)
	{
	// Done.
	}
else if (cattrs)
	{
	if (rextra)
		return false;		// Incompatible mix of layering in the two rules.
	cextra = true;			// If already set, that's ok.
	}
else	// if (rattrs)
	{
	if (cextra)
		return false;		// Incompatible mix of layering in the two rules.
	rextra = true;			// If already set, that's ok.

	// COPY LAYERING TO CONSTRAINED RULE.
	cattrs = new Dlist<Iarg>(*rattrs);		// Copy constructor.
	celt->setAttrs(cattrs);						// MODIFY ELT.
	}

return true;
}


/********************************************
* FN:		FOLDELTNAME
* CR:		07/03/99 AM.
* SUBJ:	Unify elt name from RAW rule into maximally constrained rule.
* RET:	True if ok.
*			MODIFIES CELT.
* NOTE:	Like an optimized,specialized Ielt::nameMatches() function.
*********************************************/

bool Parse::foldEltname(
	Ielt *celt,					// Phrase elt of constrained rule.
	Ielt *relt					// Phrase elt of RAW rule.
	)
{
if (!celt || !relt)
	return false;

_TCHAR *cname = celt->getName();
_TCHAR *rname = relt->getName();

Dlist<Iarg> *cmatches = celt->getMatches();
Dlist<Iarg> *rmatches = relt->getMatches();
if (!cmatches && !rmatches)
	{
	// If names identical, done.
	if (!_tcscmp(cname,rname))
		return true;

	// MATCHING RAW ELT WITH GENERIC TYPE.				// 08/09/99 AM.
	if (!_tcscmp(_T("_xALPHA"), cname) && unicu::isAlphabetic(*rname))	// 09/22/99 AM.
		return true;
	if (!_tcscmp(_T("_xNUM"), cname) && unicu::isDigit((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xPUNCT"), cname) && unicu::isPunct((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xWHITE"), cname) && unicu::isWhiteSpace((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xBLANK"), cname) && unicu::isWhiteSpace((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xEMOJI"), cname) && unicu::isEmoji((_TUCHAR)*rname))
		return true;
	if (!_tcscmp(_T("_xCTRL"), cname) && _istcntrl((_TUCHAR)*rname))				// 07/24/00 AM.
		return true;															// 07/24/00 AM.

	// Else if any are nonliteral, can't reconcile.
	if (*cname == '_' || *rname == '_')
		return false;
	}
else if (cmatches && rmatches)
	{
	// Look for ALL common names and common matches.
	// Build the intersection list.  Assume lists have no redundancy.
	Dlist<Iarg> *common;
	Iarg *arg;
	common = new Dlist<Iarg>();			// Empty list.
	// Names should both be "_xWILD".

	// Find a common list element, if any.
	Delt<Iarg> *darg;
	_TCHAR *s_arg;
	for (darg = cmatches->getFirst(); darg; darg = darg->Right())
		{
		arg = darg->getData();
		s_arg = arg->getStr();
		if (Iarg::findArg(rmatches, s_arg))
			common->rpush(new Iarg(s_arg));
		else if (intersectKinds(rmatches, s_arg))	// 08/09/99 AM.
			common->rpush(new Iarg(s_arg));			// 08/09/99 AM.

		}

	if (common->getFirst())			// Found common name(s).
		{
		// Delete constrained rule's elt's names.
		celt->setMatches(0);
		Dlist<Iarg>::DeleteDlistAndData(cmatches);

		// Rebuild names for constrained rule's elt.
		if (common->getFirst() == common->getLast())	// 07/10/99 AM.
			{
			// Only one name, so trash the list.
			celt->setName(common->getFirst()->getData()->getStr());
			Dlist<Iarg>::DeleteDlistAndData(common);
			common = 0;
			}
		else
			{
			celt->setMatches(common);
			celt->setName(_T("_xWILD"));
			}
		// min,max should be ok as is.
		return true;
		}
	else															// MEMLEAK	// 12/28/00 AM.
		{
		Dlist<Iarg>::DeleteDlistAndData(common);		// MEMLEAK	// 12/28/00 AM.
		common = 0;												// MEMLEAK	// 12/28/00 AM.
		}
	}
else if (cmatches)
	{
	// If lname is present, intersect to it.
	if (Iarg::findArg(cmatches, rname))
		{
		// Remove crule elt's name stuff.
		celt->setMatches(0);
		Dlist<Iarg>::DeleteDlistAndData(cmatches);

		// Constrain to rname.
		celt->setName(rname);
		return true;
		}
	}
else // if (rmatches)
	{
	// If cname is present, intersect to it.
	if (Iarg::findArg(rmatches, cname)
		 || intersectKinds(rmatches, cname)		// 08/09/99 AM.
		 )
		{
		// Remove crule elt's name stuff.
		celt->setMatches(0);
		Dlist<Iarg>::DeleteDlistAndData(cmatches);

		// Constrain to lname.
		celt->setName(cname);
		return true;
		}
	}

// If both have literal token, then get same token type
// and loosen constraint to that.
return foldElttoken(celt,relt);
}


/********************************************
* FN:		FOLDELTTOKEN
* CR:		07/05/99 AM.
* SUBJ:	Unify token elt from RAW rule into constrained rule.
* RET:	True if ok.
*			MODIFIES CELT.
*********************************************/

bool Parse::foldElttoken(
	Ielt *celt,					// Phrase elt of constrained rule.
	Ielt *relt					// Phrase elt of RAW rule.
	)
{
if (!celt || !relt)
	return false;

// If both have literal token, then get same token type
// and loosen constraint to that.
_TCHAR *rstr, *cstr;
if (!(rstr = relt->litName()))
	return false;
if (!(cstr = celt->litName()))
	return false;
_TCHAR *ctok = tok_type(cstr);
_TCHAR *rtok = tok_type(rstr);
if (!ctok || !rtok)
	return false;
// If not same, can't reconcile.
if (_tcscmp(ctok,rtok))
	return false;

// Remove crule elt's name stuff.
Dlist<Iarg> *cmatches = celt->getMatches();
if (cmatches)
	{
	celt->setMatches(0);
	Dlist<Iarg>::DeleteDlistAndData(cmatches);
	}

// Loosen constraint to common token type.
celt->setName(ctok);		// MODIFY CONSTRAINED RULE.
return true;
}


/********************************************
* FN:		FOLDPOST
* CR:		07/06/99 AM.
* SUBJ:	Unify post actions from constrained rule and RAW rule.
* RET:	True if ok.
* ASS:	Assume GROUP (and a final single) action are the only post
*			actions around, for now. 
*********************************************/

bool Parse::foldPost(
	Irule *crule,			// Maximally constrained rule being built.
	Irule *rrule,			// RAW sample to be folded in.
	bool cextra,			// If extra layering in constrained rule.
	bool rextra				// If extra layering in RAW rule.
	)
{
if (!crule || !rrule)
	return false;
Dlist<Iaction> *cpost = crule->getPost();
Dlist<Iaction> *rpost = rrule->getPost();
if (!cpost && !rpost)
	return true;
else if (cpost && rpost)
	{
	// TODO: Should do better checking, but:
	// Quick and dirty.  Compare lengths of the lists.
	int clen = cpost->getLength();
	int rlen = rpost->getLength();
	if (cextra)
		{
		if (rlen > clen)
			return false;
		}
	else if (rextra)
		{
		if (clen > rlen)
			return false;
		// COPY ACTIONS TO CONSTRAINED RULE.
		Dlist<Iaction>::DeleteDlistAndData(cpost);	// Delete the old.
		crule->setPost(new Dlist<Iaction>(*rpost));
		}
	else if (rlen > clen)	// Copy actions to constrained rule.
		{
		Dlist<Iaction>::DeleteDlistAndData(cpost);	// Delete the old.
		crule->setPost(new Dlist<Iaction>(*rpost));
		}
	}
else if (cpost)
	{
	if (rextra)
		return false;				// Bad mix.
	}
else // if (rpost)
	{
	if (cextra)
		return false;
	// COPY POST ACTIONS TO CONSTRAINED RULE.
	crule->setPost(new Dlist<Iaction>(*rpost));
	}
return true;
}


/********************************************
* FN:		FOLDPRES
* CR:		07/07/99 AM.
* SUBJ:	Unify pre actions from constrained rule and RAW rule.
* RET:	True if ok.
* ASS:	Assume pre actions are sorted according to element numbers
*			and then alphabetically.
*********************************************/

bool Parse::foldPres(
	Irule *crule,			// Maximally constrained rule being built.
	Irule *rrule			// RAW sample to be folded in.
	)
{
if (!crule || !rrule)
	return false;
Dlist<Ipre> *cpres = crule->getPre();
Dlist<Ipre> *rpres = rrule->getPre();
if (!cpres && !rpres)
	return true;
else if (cpres && rpres)
	{
	// DOING FULL COMPARISON OF PRE ACTIONS.
	// Traverse the two lists.
	// Build a new list that represents the commonality and "loosening"
	// of the two lists of constraints.
	Dlist<Ipre> *commons = new Dlist<Ipre>();		// New common list.
	Delt<Ipre> *cdelt = cpres->getFirst();
	Delt<Ipre> *rdelt = rpres->getFirst();
	Ipre *cpre = cdelt->getData();
	Ipre *rpre = rdelt->getData();
	int cmp;

	for (;;)					// While comparing two lists.
		{
		// If element numbers don't match, try to align them.
		if (cpre->getStart() < rpre->getStart())
			{
			if (!(cdelt = cdelt->Right()))	// Get next.
				break;
			cpre = cdelt->getData();
			continue;
			}
		else if (cpre->getStart() > rpre->getStart())
			{
			if (!(rdelt = rdelt->Right()))	// Get next.
				break;
			rpre = rdelt->getData();
			continue;
			}

		// If pre action names don't match, try to align them.
		cmp = _tcscmp(cpre->getName(), rpre->getName());
		if (cmp < 0)
			{
			if (!(cdelt = cdelt->Right()))	// Get next.
				break;
			cpre = cdelt->getData();
			continue;
			}
		else if (cmp > 0)
			{
			if (!(rdelt = rdelt->Right()))	// Get next.
				break;
			rpre = rdelt->getData();
			continue;
			}

		// Pre action names match, so reconcile data, if any.
		// ADD COMMON PRE ACTION HERE.
		if (!foldPreargs(cpre, rpre, commons))
			return false;

		// Put in hook for ranges.
		// (What about things like cap vs all-cap? Should name these
		// the same, with different args! or may want a mapping of user
		// names to internal names.)

		// Continue down both lists.
		if (!(cdelt = cdelt->Right()))	// Get next.
			break;
		cpre = cdelt->getData();
		if (!(rdelt = rdelt->Right()))	// Get next.
			break;
		rpre = rdelt->getData();
		}
	// Put in the commons list and zap the current list.
	Dlist<Ipre>::DeleteDlistAndData(cpres);
	if (commons->getFirst())
		crule->setPre(commons);
	else
		{
		crule->setPre(0);
		Dlist<Ipre>::DeleteDlistAndData(commons);
		}
	}
else if (cpres)
	{
	// REMOVE THE CONSTRAINTS IN RULE BEING BUILT.
	// Because the current RAW sample doesn't share those constraints.
	Dlist<Ipre>::DeleteDlistAndData(cpres);	// Delete the pre actions.
	crule->setPre(0);									// Zero them out.
	}
else // if (rpres)
	{
	// Done.  The rule being built already has no pre action constraints.
	}
return true;
}


/********************************************
* FN:		FOLDPREARGS
* CR:		07/07/99 AM.
* SUBJ:	Unify pre action args from constrained rule and RAW rule.
* RET:	True if ok.
* ASS:	Found matching pre action names and element numbers.
*********************************************/

bool Parse::foldPreargs(
	Ipre *cpre,					// Pre action currently in rule being built.
	Ipre *rpre,				// Pre action in RAW rule being compared.
	Dlist<Ipre> *commons		// List of pre actions being built.
	)
{
if (!cpre || !rpre)
	return true;
Dlist<Iarg> *cargs = cpre->getArgs();
Dlist<Iarg> *rargs = rpre->getArgs();
if (!cargs && !rargs)
	{
	// Build the pre action.				// FIX.	// 07/11/99 AM.
	Ipre *tmp = new Ipre(*cpre);			// Copy constructor.
	commons->rpush(tmp);
	return true;			// Pre actions matched, with no args.
	}
else if (cargs && rargs)
	{
	if (Iarg::same(cargs, rargs))
		{
		// Build the pre action.
		Ipre *tmp = new Ipre(*cpre);			// Copy constructor.
		commons->rpush(tmp);
		return true;
		}
	// If length, could build a range.
	// If capitalization, could loosen the constraint some.
	// (eg, all-caps + cap => cap).
	}
else if (cargs)
	{
	return true;			// Dropping the pre action.
	}
else if (rargs)
	{
	return true;			// Dropping the pre action.
	}
return true;
}

/********************************************
* FN:		TOKTYPE
* CR:		07/03/99 AM.
* SUBJ:	Deduce a token type for a string.
* RET:	Token type as string= _xALPHA,_xNUM,_xPUNCT,_xWHITE.
*********************************************/

_TCHAR *Parse::tok_type(_TCHAR *str)
{
if (!str)
	return 0;
if (unicu::isAlphabetic(*str))			// 09/22/99 AM.
	return _T("_xALPHA");
else if (unicu::isDigit((_TUCHAR)*str))
	return _T("_xNUM");
else if (unicu::isPunct((_TUCHAR)*str))
	return _T("_xPUNCT");
else if (unicu::isWhiteSpace((_TUCHAR)*str))
	return _T("_xWHITE");
else if (_istcntrl((_TUCHAR)*str))														// 07/24/00 AM.
	return _T("_xCTRL");															// 07/24/00 AM.
else if (unicu::isEmoji((_TUCHAR)*str))
	return _T("_xEMOJI");
return 0;
}


/********************************************
* FN:		FIXCONSTRAINS
* CR:		07/13/99 AM.
* SUBJ:	Fix RAW rule labels under constrains.
* RET:	True if success.
*********************************************/

bool Parse::fixConstrains(Dlist<Irule> *constrains)
{
if (!constrains)
	return false;
Slist<Irule> *raws;
Delt<Irule> *cdelt;
Irule *crule;
for (cdelt = constrains->getFirst(); cdelt; cdelt = cdelt->Right())
	{
	crule = cdelt->getData();
	if (!(raws = crule->getDown()))
		continue;
	Selt<Irule> *selt;
	Irule *rrule;
	for (selt = raws->getFirst(); selt; selt = selt->Right())
		{
		rrule = selt->getData();
		labelRule(crule, rrule);	// Use crule to fix labels in lrule.
		}
	}
return true;
}


/********************************************
* FN:		LABELRULE
* CR:		07/14/99 AM.
* SUBJ:	Fix labels for one rule using another as a template.
* RET:	True if ok.
* NOTE:	Rule may be partially or fully labeled already.
*********************************************/

bool Parse::labelRule(
	Irule *trule,		// Template rule.
	Irule *rule			// Rule to fix.
	)
{
if (!trule || !rule)
	return false;

// Compare the attributes on the phrase elements.
if (!labelPhrase(trule->getPhrase(), rule->getPhrase()))
	return false;

// Compare the GROUP post actions for the rules.
if (!labelPost(trule, rule))
	return false;
return true;
}


/********************************************
* FN:		LABELPHRASE
* CR:		07/14/99 AM.
* SUBJ:	Label phrase using template phrase.
* RET:	True if ok.
*			MODIFIES CPHRASE.
*********************************************/

bool Parse::labelPhrase(
	Dlist<Ielt> *tphrase,		// Phrase of template rule.
	Dlist<Ielt> *phrase			// Phrase to label.
	)
{
if (!tphrase || !phrase)
	return false;

Delt<Ielt> *tdelt, *delt;
for (tdelt = tphrase->getFirst(), delt = phrase->getFirst();
	  tdelt && delt;
	  tdelt = tdelt->Right(), delt = delt->Right())
	{
	if (!labelElt(tdelt->getData(), delt->getData()))
		return false;
	}
if (tdelt || delt)
	return false;			// Should be same length.
return true;				// Passed all hurdles.
}


/********************************************
* FN:		LABELELT
* CR:		07/14/99 AM.
* SUBJ:	Label elt using template elt.
* RET:	True if ok.
*			MODIFIES CELT.
*********************************************/

bool Parse::labelElt(
	Ielt *telt,					// Phrase elt of template rule.
	Ielt *elt					// Phrase elt to label.
	)
{
if (!telt || !elt)
	return false;

// Compare elt names.
// Elt names had better match!

// Transfer layering if absent.
Dlist<Iarg> *tattrs = telt->getAttrs();
Dlist<Iarg> *attrs = elt->getAttrs();
if (!attrs && !tattrs)
	return true;
else if (attrs && tattrs)		// Should be the same.
	return true;
else if (!attrs)
	{
	// COPY LAYERING.
	attrs = new Dlist<Iarg>(*tattrs);		// Copy constructor.
	elt->setAttrs(attrs);						// MODIFY ELT.
	}
else if (!tattrs)
	return false;
return true;
}


/********************************************
* FN:		LABELPOST
* CR:		07/14/99 AM.
* SUBJ:	Add labels to post actions of rule using a template rule.
* RET:	True if ok.
* NOTE:	Quick-and-dirty.  Just copy the post actions from template
*			rule into rule to be labeled.  This may modify the RAW
*			rule slightly, but shouldn't matter.
*********************************************/

bool Parse::labelPost(
	Irule *trule,			// Template rule.
	Irule *rule				// Rule to be labeled.
	)
{
if (!trule || !rule)
	return false;
Dlist<Iaction> *tpost = trule->getPost();
Dlist<Iaction> *post = rule->getPost();
if (!tpost && !post)
	return true;
else if (tpost && post)
	{
	// TODO: Should do better checking, but:
	// Quick and dirty.  Compare lengths of the lists.
	int tlen = tpost->getLength();
	int len = post->getLength();
	if (tlen == len)
		return true;			// Done.
	else if (len > tlen)
		return false;			// Template should be more ample.

	// COPY ACTIONS TO RULE TO BE LABELED.
	if (post)
		Dlist<Iaction>::DeleteDlistAndData(post);	// Delete the old.
	rule->setPost(new Dlist<Iaction>(*tpost));
	}
else if (tpost)
	{
	// COPY ACTIONS TO RULE TO BE LABELED.
	rule->setPost(new Dlist<Iaction>(*tpost));
	}
else if (post)
	return false;
return true;
}


/********************************************
* FN:		PRUNECONSTRAINS
* CR:		07/03/99 AM.
* SUBJ:	Remove constrained rules that are too general.
* RET:	UP constrains - pruned list.
*********************************************/

bool Parse::pruneConstrains(Dlist<Irule> *constrains)
{
if (!constrains)
	return false;
Dlist<Irule> *tmp;			// Build pruned list.
Delt<Irule> *drule;
Irule *rule;
tmp = new Dlist<Irule>();		// Empty so far.
while ((drule = constrains->pop()))
	{
	rule = drule->getData();
	if (pruneConstrain(rule))		// Rule is too general.
		{
		Irule::deleteOwning(rule);
		Delt<Irule>::DeleteDeltAndData(drule);
		}
	else			// Attach to new list.
		{
		tmp->rpush(drule);
		}
	}

// Transfer pruned list back to constrains.
constrains->setFirst(tmp->getFirst());
constrains->setLast(tmp->getLast());
tmp->setFirst(0);
tmp->setLast(0);
delete tmp;
return true;
}


/********************************************
* FN:		PRUNECONSTRAIN
* CR:		07/03/99 AM.
* SUBJ:	See if given constrained rule should be pruned.
* RET:	True if pruning this rule.
* NOTE:	Will use simple heuristics to see if rule is too general.
* TODO:	Augment these functions with user-supplied attributes that
*			tell how to generalize from samples.
*********************************************/

bool Parse::pruneConstrain(Irule *rule)
{
if (!rule)
	return false;

Dlist<Ielt> *phr = rule->getPhrase();

Delt<Ielt> *delt = 0;
Ielt *ielt = 0;

delt = phr->getFirst();
Ielt *first = delt->getData();
delt = phr->getLast();
Ielt *last = delt->getData();

// Heuristic: Prune if first and last elts of phrase are alphabetic.
// (Too unconstrained.)
if (first->nameMatches(_T("_xALPHA"))
    && last->nameMatches(_T("_xALPHA")))
	return true;

// Heuristic: Prune one-element phrases that are too generic.
long len = phr->getLength();
if (len == 1)
	{
	Delt<Ielt> *delt = phr->getFirst();
	Ielt *elt = delt->getData();
	if (   elt->nameMatches(_T("_xNUM"))
		 || elt->nameMatches(_T("_xPUNCT"))
		 || elt->nameMatches(_T("_xWHITE"))
		 || elt->nameMatches(_T("_xALPHA"))	// Just in case.
		 || elt->nameMatches(_T("_xCTRL"))									// 07/24/00 AM.
		 )
		return true;
	}

return false;
}


/********************************************
* FN:		FINDSUBS
* CR:		07/06/99 AM.
* SUBJ:	Get names of subnodes to given node, if any.
* NOTE:	Include name of given node.
*********************************************/

Dlist<Iarg> *Parse::findSubs(Node<Pn> *node)
{
if (!node)
	return 0;

// Traverse down the node's subtree till it branches or till a node with
// BASE attribute is set.
Pn *pn = 0;
Iarg *arg = 0;
pn = node->getData();
arg = new Iarg(pn->getName());
Dlist<Iarg> *names = new Dlist<Iarg>(arg);
if (pn->getBase())
	return names;

while ((node = node->Down()))
	{
	if (node->Right())									// Branches out.
		break;

	pn = node->getData();
	names->rpush(new Iarg(pn->getName()));

	if (pn->getBase())									// Bottommost singlet.
		break;
	}
return names;
}


/********************************************
* FN:		INTERNRUG
* CR:		07/08/99 AM.
* SUBJ:	Intern a list of generated rules.
* NOTE:	Only doing something for pre actions now.
*			Copying from rule list to each individual elt.
*********************************************/

bool Parse::internRug(Dlist<Irule> *rules)
{
if (!rules)
	return true;

Delt<Irule> *drule;
Irule *rule;
for (drule = rules->getFirst(); drule; drule = drule->Right())
	{
	rule = drule->getData();
	Irule::internPres(rule->getPre(), rule->getPhrase());
	}
return true;
}



/********************************************
* FN:		closedOn
* CR:		07/19/99 AM.
* SUBJ:	Turn on all CLOSED flags for elts of given rule.
* NOTE:	Quick and dirty solution.  Should do a more principled
*			traversal depending on whether rule elt was labeled or not.
*			Easiest might be to define a "label" flag for rule elts that
*			are covered by labels.
********************************************/

void Parse::closedOn(Irule *rule)
{
if (!rule)
	return;
Dlist<Ielt> *phr = rule->getPhrase();
if (!phr)
	return;
Delt<Ielt> *delt;
Ielt *elt;
for (delt = phr->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	elt->setClosed(true);
	}
}


/********************************************
* FN:		FIXCLOSED
* CR:		08/19/99 AM.
* SUBJ:	Fix all CLOSED elts for given rule.
* NOTE:	Nearly identical to setupConstrain, for now.
********************************************/

bool Parse::fixClosed(Irule *rule)
{
if (!rule)
	return false;
// For each elt that has a closed-set attribute, get its token and
// convert to an _xWILD with matches list.
Dlist<Ielt> *phrase = rule->getPhrase();
if (!phrase)
	return false;
Delt<Ielt> *delt;
Ielt *elt;
_TCHAR *name;
Dlist<Iarg> *matches;
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	elt = delt->getData();
	if (elt->getClosed())
		{
		if (!(name = elt->litName()))
			return false;
		// Trash the matches.
		if ((matches = elt->getMatches()))
			{
			Dlist<Iarg>::DeleteDlistAndData(matches);
			elt->setMatches(0);		// Zero it out.
			}
		elt->setName(name);
		}
	}
return true;
}


/********************************************
* FN:		INTERSECTKINDS
* CR:		08/09/99 AM.
* SUBJ:	See if matches and elt name can intersect by type.
********************************************/

bool Parse::intersectKinds(Dlist<Iarg> *dlist, _TCHAR *key)
{
if (!dlist || !key)
	return 0;

if (_tcscmp(key, _T("_xALPHA"))
	 && _tcscmp(key, _T("_xNUM"))
	 && _tcscmp(key, _T("_xPUNCT"))
	 && _tcscmp(key, _T("_xWHITE"))
	 && _tcscmp(key, _T("_xBLANK"))
	 && _tcscmp(key, _T("_xCTRL"))												// 07/24/00 AM.
	 )
	return false;

Delt<Iarg> *delt;
Iarg *arg;
_TCHAR *str;
for (delt = dlist->getFirst(); delt; delt = delt->pRight)
	{
	arg = delt->getData();
	if (arg->getType() != IASTR)
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[intersectKinds: Nonstring arg.]") << std::ends;
		::errOut(&gerrStr,false);
		return false;
		}
	str = arg->getStr();

	// MATCHING LITERAL ELT WITH GENERIC TYPE.
	if (!_tcscmp(_T("_xALPHA"), key) && unicu::isAlphabetic(*str))	// 09/22/99 AM.
		return true;
	if (!_tcscmp(_T("_xNUM"), key) && unicu::isDigit((_TUCHAR)*str))
		return true;
	if (!_tcscmp(_T("_xPUNCT"), key) && unicu::isPunct((_TUCHAR)*str))
		return true;
	if (!_tcscmp(_T("_xWHITE"), key) && unicu::isPunct((_TUCHAR)*str))
		return true;
	if (!_tcscmp(_T("_xBLANK"), key) && unicu::isWhiteSpace((_TUCHAR)*str))
		return true;
	if (!_tcscmp(_T("_xEMOJI"), key) && unicu::isEmoji((_TUCHAR)*str))
		return true;
	if (!_tcscmp(_T("_xCTRL"), key) && _istcntrl((_TUCHAR)*str))						// 07/24/00 AM.
		return true;
	}
return 0;			// Arg not found.
}


/********************************************
* FN:		NIXCONTEXT
* CR:		10/29/99 AM.
* SUBJ:	Get rid of context nodes when building a rule in RUG.
********************************************/

bool Parse::nixContext(
	/*DU*/
	Node<Pn>* &n_start,
	Node<Pn>* &n_end,
	Node<Pn>* &parent
	)
{
if (!n_start || !n_end)
	return false;
if (n_start != n_end)	// Only considering single context node now.
	return true;			// Multi nodes are fine as is.
Pn *pn = n_start->getData();
if (!(pn->getUnsealed()))	// Not a context node.
	return true;			// Hunky dory.
Node<Pn> *node;
if (!(node = n_start->Down()))
	return true;			// Rock bottom, so use what you've got.
n_start = node;

// While we're at a singlet unsealed node, keep drilling down.
for (;;)
	{
	if (n_start->Right())
		break;				// Update phrase and parent and return.
	pn = n_start->getData();
	if (!(pn->getUnsealed()))	// Not a context node.
		break;
	if (!(node = n_start->Down()))	// Rock bottom.
		break;
	n_start = node;
	}

// Get new region and return.
// (Get last node in phrase because, remember, the whole phrase precisely
// covers the given context node.)
parent = n_start->Up();
n_end = n_start;
while (n_end->Right())
	n_end = n_end->Right();	// Get last node in phrase.
return true;
}


/********************************************
* FN:		BUILDLITERALS
* CR:		10/29/99 AM.
* SUBJ:	Build literal rules from the RAW rules.
* RET:	NEW LIST of literal rules.
* NOTE:	Renamed "literal" to "raw".  So now LITERAL is a whole new
*			set of rules.
* ALG:	For each RAW rule, build a LITERAL variant.  (as in fixClosed).
*			Also, get rid of length() and other extraneous actions.
*			Keep cap() and similar actions.
*			Should get literal rules sorted by length.
* OPT:	For simplicity, COPYING the RAW list and modifying.  In the
*			future, can optimize by building a new rule using the RAW rule
*			as a template.
*********************************************/

Dlist<Irule> *Parse::buildLiterals(
	Dlist<Irule> *raws		// List of RAW rules.
	)
{
// Make a copy of the RAW list.
Dlist<Irule> *lits = Irule::copy_owning_list(raws);

// For each rule, convert to a literal rule.
Delt<Irule> *delt;
for (delt = lits->getFirst(); delt; delt = delt->Right())
	buildLiteral(delt->getData());
return lits;
}


/********************************************
* FN:		BUILDLITERAL
* CR:		10/29/99 AM.
* SUBJ:	Convert a copied RAW rule to LITERAL.
* RET:	NEW LIST of literal rules.

*********************************************/

bool Parse::buildLiteral(
	Irule *rule					// Rule to be literalized.
	)
{
if (!rule)
	return false;

// Delete the length() preactions.
if (rule->getPre())															// 01/01/01 AM.
	{
	cleanLitpres(rule->getPre());
	if (!(rule->getPre()->getFirst()))									// 01/01/01 AM.
		{
		Dlist<Ipre>::DeleteDlistAndData(rule->getPre());			// 01/01/01 AM.
		rule->setPre(0);														// 01/01/01 AM.
		}
	}

// For each rule elt, get rid of all layered names.
Dlist<Ielt> *phrase = rule->getPhrase();
if (!phrase)
	return false;
Delt<Ielt> *delt;
Ielt *elt;
_TCHAR *name;
Dlist<Iarg> *matches;
for (delt = phrase->getFirst(); delt; delt = delt->Right())
	{
	// FIX.								// 11/11/99 AM.
	// Could be _xWHITE, and otherwise can be nonliterals.
	elt = delt->getData();
	matches = elt->getMatches();
	if ((name = elt->litName()))			// If there's a literal elt.
		{
		// Done;
		}
	else									// Nonliteral elt.
		{
		if (!matches)
			name = elt->getName();	// Use the only elt name.
		else
			{
			// Choose the last of the matches, discard the rest.
			// (Should be the "least reduced" nonliteral.)
			Iarg *arg = matches->getLast()->getData();
			name = arg->getStr();
			}
		}
	// Trash the matches, if any.
	if (matches)
		{
		Dlist<Iarg>::DeleteDlistAndData(matches);
		elt->setMatches(0);		// Zero it out.
		}
	elt->setName(name);
	}
return true;
}


/********************************************
* FN:		CLEANLITPRES
* CR:		11/11/99 AM.
* SUBJ:	Clean pre actions of LIT rule.
* RET:	True if ok.

*********************************************/

bool Parse::cleanLitpres(
	Dlist<Ipre> *pres			// List of pre actions.
	)
{
if (!pres)
	return true;				// Empty is ok.
Delt<Ipre> *dpre, *tmp;
Ipre *pre;
dpre = pres->getFirst();
while (dpre)
	{
	pre = dpre->getData();
	if (!_tcscmp(_T("length"), pre->getName()))
		{
		// Delete this pre action.
		tmp = dpre->Right();
		pres->exciseElt(dpre);
		Delt<Ipre>::DeleteDeltAndData(dpre);
		dpre = tmp;
		}
	else
		dpre = dpre->Right();
	}
return true;
}
