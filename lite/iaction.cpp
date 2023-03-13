/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IACTION.CPP
* FILE:	lite\iaction.cpp
* CR:		10/24/98 AM.
* SUBJ:	Interpreted/internal rule class.
*
*******************************************************************************/

#include "StdAfx.h"
//#include <iostream> // 07/18/03 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"			// 05/19/99 AM.
#include "io.h"
#include "lite/iarg.h"	// 05/14/03 AM.
#include "str.h"				// 06/07/99 AM.
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"				// 05/08/00 AM.
#include "iarg.h"				// 11/24/98 AM.
#include "slist.h"			// 02/26/01 AM.
#include "thtab.h"			// 02/26/01 AM.
#include "pat.h"				// 01/13/00 AM.
#include "lite/nlppp.h"		// 01/13/00 AM.
#include "fn.h"				// 01/14/00 AM.
#include "lite/code.h"		// 01/14/00 AM.
#include "lite/nlp.h"		// 12/24/01 AM.
#include "lite/vtrun.h"		// 08/28/02 AM.
#include "ivar.h"				// 06/16/02 AM.
#include "parse.h"			// 08/24/02 AM.
#include "iaction.h"
#include "rfasem.h"			// 11/13/99 AM.

#ifdef LINUX
template<>	// 04/26/07 AM.
int Dlist<Iaction>::count_ = 0;
template<>	// 04/26/07 AM.
int Delt<Iaction>::count_ = 0;
#else
int Dlist<Iaction>::count_ = 0;
int Delt<Iaction>::count_ = 0;
#endif
//int Iaction::count_ = -1;				// 11/17/98 AM. Because of 1 static.
int Iaction::count_ = 0;					// 07/02/99 AM.

/********************************************
* FN:		Special Functions for Iaction Class
* CR:		10/24/98 AM.
********************************************/

Iaction::Iaction(					// Default constructor.
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa,				// Action arguments.
	long line						// Line number							// 05/17/01 AM.
	)
{
name_ = nn;
args_ = aa;
origargs_ = aa;					// 03/13/02 AM.
sem_ = 0;							// 11/13/99 AM.
scope_ = 0;							// 02/13/01 AM.
line_	= line;						// 05/17/01 AM.
func_ = 0;							// 12/26/01 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Iaction::~Iaction()
{
// 11/17/98 AM.
if (args_)
	Dlist<Iarg>::DeleteDlistAndData(args_);
if (sem_)						// 11/13/99 AM.
	delete sem_;				// 11/13/99 AM.

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.	// 07/02/99 AM.
/*******************************************/
Iaction::Iaction(Iaction &orig)
{
zero();
Iaction::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.  // 07/02/99 AM.
/*******************************************/
const Iaction &Iaction::operator=(const Iaction &fm)
{
Iaction *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Iaction object to itself.]") << std::ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Iaction::clear()
{
if (args_)
	Dlist<Iarg>::DeleteDlistAndData(args_);
if (sem_)						// 11/13/99 AM.
	delete sem_;				// 11/13/99 AM.
}

void Iaction::zero()
{
name_ = 0;
args_ = 0;
origargs_ = 0;					// 03/13/02 AM.
sem_ = 0;						// 11/13/99 AM.
scope_ = 0;						// 02/13/01 AM.
line_= 0;						// 05/17/01 AM.
func_ = 0;						// 12/26/01 AM.
}

void Iaction::copy(const Iaction *orig)
{
Iaction *dest;
dest = this;

dest->name_ = orig->name_;
dest->scope_ = orig->scope_;
dest->line_	 = orig->line_;

if (orig->args_)
	dest->args_ = new Dlist<Iarg>(*(orig->args_));
dest->origargs_ = dest->args_;								// 03/13/02 AM.
dest->sem_ = orig->sem_;		// Copy constructor.		// 11/13/99 AM.
dest->func_ = orig->func_;										// 12/26/01 AM.
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Iaction &action)
{
if (action.scope_)															// 02/13/01 AM.
	output << str(action.scope_) << _T("::");								// 02/13/01 AM.
output << str(action.name_) << _T("(");
if (action.args_)
	output << *(action.args_);
if (action.sem_)							// 11/13/99 AM.
	output << *(action.sem_);			// 11/13/99 AM.
output << _T(")");														// FIX.	// 08/20/00 AM.
	// << std::endl;														// FIX.	// 08/20/00 AM.
return output;
}

/*******************************************/

/*******************************************/
std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Delt<Iaction> &delt)
{
Iaction *arg;
arg  = delt.getData();
output << *arg;
return output;
}

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Dlist<Iaction> &list)
{
Delt<Iaction> *delt;
delt = list.getFirst();
output << *delt;
while ((delt = delt->Right()))
	{
	output << std::endl << *delt;
	}
return output;
}
/*******************************************/


/********************************************
* FN:		Access Functions
* CR:		10/24/98 AM.
********************************************/

_TCHAR			*Iaction::getName()	{return name_;}
Dlist<Iarg>	*Iaction::getArgs()	{return args_;}
Dlist<Iarg> *Iaction::getOrigargs()	{return origargs_;}			// 03/13/03 AM.
RFASem		*Iaction::getSem()	{return sem_;}						// 11/13/99 AM.
_TCHAR			*Iaction::getScope()	{return scope_;}					// 02/13/01 AM.
long			 Iaction::getLine()	{return line_;}					// 05/17/01 AM.
Ifunc			*Iaction::getFunc()	{return func_;}					// 12/26/01 AM.

/********************************************
* FN:		Modify Functions
* CR:		10/24/98 AM.
********************************************/

void Iaction::setName(_TCHAR *x)			{name_ = x;}
void Iaction::setArgs(Dlist<Iarg> *x)	{args_ = x;}
void Iaction::setOrigargs(Dlist<Iarg> *x)	{origargs_ = x;}		// 03/13/02 AM.
void Iaction::setSem(RFASem *x)			{sem_ = x;}					// 11/13/99 AM.
void Iaction::setScope(_TCHAR *x)			{scope_ = x;}				// 02/13/01 AM.
void Iaction::setLine(long x)				{line_  = x;}				// 05/17/01 AM.
void Iaction::setFunc(Ifunc *x)			{func_  = x;}				// 12/26/01 AM.


/********************************************
* FN:		GETCOUNT
* CR:		10/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Iaction::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* CR:		10/24/98 AM.
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Iaction::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Iaction count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Iaction count=") << count_ << std::ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		makeDelt
* CR:		10/24/98 AM.
* SUBJ:	Create a list element with appropriate data.
* NOTE:	Must be a more elegant way to initialize the elt.
********************************************/
//static Iaction obj;		// 07/02/99 AM.

Delt<Iaction> *Iaction::makeDelt(
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa,				// Action arguments.
	long line						// Line number							// 05/17/01 AM.
	)
{
Delt<Iaction> *delt;

// 07/02/99 AM. New way.
Iaction *obj;
obj = new Iaction(nn,aa,line);											// 05/17/01 AM.
delt = new Delt<Iaction>(obj);
return delt;

// 07/02/99 AM. OLD WAY.
// Hack. Keep refilling same static Iaction, because it will be copied.
//obj.name_	= nn;
//obj.args_	= aa;

//delt = new Delt<Iaction>(obj);

//obj.name_ = 0;			// 11/16/98 AM. Bug fix.
//obj.args_ = 0;

//return delt;
}


/********************************************
* FN:		makeDlist
* CR:		10/24/98 AM.
* SUBJ:	Create a list with appropriate data.
* NOTE:	Class member function.
*			(Can be called without an instance existing).
********************************************/

Dlist<Iaction> *Iaction::makeDlist(
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa,				// Action arguments.
	long line						// Line number							// 05/17/01 AM.
	)
{
Delt<Iaction> *delt;
delt = makeDelt(nn,aa,line);												// 05/17/01 AM.
return new Dlist<Iaction>(delt);
}


/********************************************
* FN:		addDelt
* CR:		10/24/98 AM.
* SUBJ:	Add new element to given list.
* NOTE:	Must be a more elegant way to initialize the elt.
*			Class member function. Need not be called with object.
********************************************/
//static Iaction obj;

Delt<Iaction> *Iaction::addDelt(
	Dlist<Iaction> *list,		// List to add element to.
	_TCHAR *nn,						// Action name.
	Dlist<Iarg> *aa,				// Action arguments.
	long line						// Line number							// 05/17/01 AM.
	)
{
Delt<Iaction> *delt;

// 07/02/99 AM. NEW WAY.
Iaction *obj;
obj = new Iaction(nn,aa,line);											// 05/17/01 AM.
delt = new Delt<Iaction>(obj);
list->rpush(delt);
return delt;

// 07/02/99 AM. OLD WAY.
// Hack. Keep refilling same static Iaction, because it will be copied.
//obj.name_	= nn;
//obj.args_	= aa;

//delt = new Delt<Iaction>(obj);

//obj.args_ = 0;			// 11/17/98 AM.

//list->rpush(delt);
//return delt;			// In case anyone wants it.
}

/********************************************
* FN:		GENACTIONS
* CR:		06/15/99 AM.
* SUBJ:	Generate actions to a rules file.
*********************************************/

void Iaction::genActions(
	Dlist<Iaction> *actions,
	std::_t_ostream &ofile
	)
{
if (!actions)
	return;

Delt<Iaction> *daction;

// Handle first action specially.
daction = actions->getFirst();
ofile << _T("\t");
genAction(daction->getData(), ofile);

// Handle rest of actions.
for (daction = daction->Right(); daction; daction = daction->Right())
	{
	// Adding semicolon at end of action.					// FIX.	// 09/02/01 AM.
	ofile << _T(";\n\t");												// FIX.	// 09/02 01 AM.
	genAction(daction->getData(), ofile);
	}
ofile << _T(";");														// FIX.	// 09/02/01 AM.
ofile << std::endl;																	// 11/04/99 AM.
}


/********************************************
* FN:		GENACTION
* CR:		06/15/99 AM.
* SUBJ:	Generate action to a rules file.
*********************************************/
void Iaction::genAction(
	Iaction *action,
	std::_t_ostream &ofile
	)
{
if (!action)
	return;
if (action->sem_)											// 11/13/99 AM.
	{
	ofile << *(action->sem_) << std::flush;				// 12/27/99 AM.
	//action->sem_->genRFASem().						// 11/13/99 AM.
	return;													// 11/13/99 AM.
	}
ofile << action->getName() << _T("(") << std::flush;
if (action->getArgs())
	Iarg::genArgs(action->getArgs(), _T(", "), ofile);
//else				// Because of RFA FEATURE!		// 07/06/99 AM.
//	ofile << "0";											// 07/06/99 AM.
ofile << _T(")") << std::flush;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	See if two pre lists are identical in form.
*********************************************/

bool Iaction::same(Dlist<Iaction> *list1, Dlist<Iaction> *list2)
{
if (!list1 && !list2)
	return true;
if (!list1 || !list2)
	{
	if (list1 && list1->getFirst())
		return false;
	if (list2 && list2->getFirst())
		return false;
	return true;			// Empty lists even so.
	}

// Compare pre-by-pre.
Delt<Iaction> *d1 = list1->getFirst();
Delt<Iaction> *d2 = list2->getFirst();
if (!d1 && !d2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Iaction::same: Empty lists]") << std::ends;
	errOut(&gerrStr,false);
	return true;
	}
if (!d1 || !d2)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Iaction::same: Empty list.]") << std::ends;
	errOut(&gerrStr,false);
	return false;
	}
for (; d1 && d2; d1 = d1->Right(), d2 = d2->Right())
	{
	if (!same(d1->getData(), d2->getData()))
		return false;
	}

if (d1 || d2)			// Unequal length.
	return false;

return true;
}


/********************************************
* FN:		SAME
* CR:		06/07/99 AM.
* SUBJ:	See if two actions are identical in form.
*********************************************/

bool Iaction::same(Iaction *action1, Iaction *action2)
{
if (!action1 && !action2)
	return true;
if (!action1 || !action2)
	return false;

if (!str_equal(action1->getName(), action2->getName()))
	return false;

if (!Iarg::same(action1->getArgs(), action2->getArgs()))
	return false;

if (!RFASem::same(action1->getSem(), action2->getSem()))		// 11/13/99 AM.
	return false;															// 11/13/99 AM.

return true;
}


/********************************************
* FN:		EVAL
* CR:		01/13/00 AM.
* SUBJ:	Execute function call ("action") and return value.
* RET:	True if evaluation is ok.
*			UP sem - build semantic object for return value.
* WARN:	11/15/99 AM. Always returns a sem object.  If var doesn't exist,
*			returns a numeric 0.
* NOTE:	03/13/02 AM. Need to supply the original args list as well as the
*			evaluated INSTANCE of the args, in order to support recursive user-
*			defined NLP++ functions.
*********************************************/

bool Iaction::eval(Nlppp *nlppp, /*UP*/ RFASem* &sem)
{
sem = 0;
bool ok = true;
Parse *parse = nlppp->parse_;												// 08/24/02 AM.

// If it's a pre,check,post action, go and execute them in their weird
// way and return what they return.
// Need an nlppp control for what region we're in, then call the
// appropriate guy.

if (!name_ || !*name_)
	{
	// BUILD A DEFAULT VALUE.
	sem = new RFASem(0LL);
	return false;
	}

/////////////////////////////
/////////////////////////////
// NEED TO EVALUATE ALL THE ARGS HERE!
// Build a list of values to pass to the function/action calls.
// (Need to implement args as list of NLP++ expressions first.)

// EVALUATING EXPR ARGS NOW.												// 02/17/00 AM.
// If any arg is an expression, then copy the args list,
// evaluate the expr args, and replace them with the evaluated
// value.  If evaluates to a var, get its value.
// OPT TODO: Flag actions that have any exprs in them.
// For now, copy all args, evaluate and replace.

// No longer need this.  Keeping in origargs_.						// 03/13/02 AM.
//Dlist<Iarg> *saveargs = args_;											// 03/13/02 AM.

// COPY LIST OF ARGS.  ONLY COPY POINTERS TO DATA.
// COPY FROM ORIG ARGS LIST (so recursion will work).				// 03/13/02 AM.
Dlist<Iarg> *iargs = 0;
if (origargs_)																	// 03/13/02 AM.
	iargs = Iarg::copy_top(origargs_);									// 03/13/02 AM.

// TRAVERSE ARGS, EVAL EXPR ARGS.
Delt<Iarg> *darg;
Iarg *iarg;
RFASem *semarg;
RFASem *val;
_TCHAR *str;
long long num;
float fnum;																		// 08/17/01 AM.
//Iarg *newarg;
if (iargs)
 for (darg = iargs->getFirst(); darg; darg = darg->Right())		// 02/17/00 AM.
	{
	iarg = darg->getData();
	switch (iarg->getType())												// 05/26/02 AM.
		{
		case IASEM:			// Evaluate these guys.
		{
		semarg = iarg->getSem();
		ok = semarg->eval(nlppp, /*UP*/ val);
		if (!ok)
			{
			nlppp->parse_->line_ = line_;									// 08/24/02 AM.
			_stprintf(Errbuf, _T("[Fncall error: Bad arg expression.]"));// 05/18/01 AM.
			parse->errOut(false); // UNFIXED 														// 05/18/01 AM.
			sem = new RFASem(0LL);
			this->setArgs(origargs_);				// Restore args.	// 03/13/02 AM.
			Iarg::delete_top(iargs);				// Zap.				// 02/18/00 AM.
			return false;
			}

		// If ok but no semantic value returned.			// FIX.	// 08/07/00 AM.
		if (!val)
			{
			// Creating a ZERO valued argument, as better fix.		// 05/03/02 AM.
//			iarg->setType(IANULL);	// FIX.	// 08/07/00 AM.		// 05/03/02 AM.
			iarg->setType(IANUM);								// FIX.	// 05/03/02 AM.
			iarg->setNum(0);										// FIX.	// 05/03/02 AM.
			continue;															// 08/07/00 AM.
			}

		// Replace arg with computed value.
		// Manage objects to delete, etc.
		switch (val->getType())
			{
			case RSSTR:
			case RSNUM:												// FIX.	// 11/19/01 AM.
			case RSNAME:											// FIX.	// 11/19/01 AM.
				str = val->getName();
				//newarg = new Iarg(str);
				//darg->setData(newarg);
				iarg->setType(IASTR);			// FIX.					// 02/18/00 AM.
				iarg->setStr(str);				// FIX.					// 02/18/00 AM.
				delete val;
				// DON'T DELETE THE iarg.									// 02/18/00 AM.
				break;
			case RSLONG:
				num = val->getLong();
				//newarg = new Iarg(num);
				//darg->setData(newarg);
				iarg->setType(IANUM);			// FIX.					// 02/18/00 AM.
				iarg->setNum(num);				// FIX.					// 02/18/00 AM.
				delete val;
				// DON'T DELETE THE iarg.									// 02/18/00 AM.
				break;
			case RSFLOAT:														// 08/17/01 AM.
				fnum = val->getFloat();										// 08/17/01 AM.
				iarg->setType(IAFLOAT);										// 08/17/01 AM.
				iarg->setFloat(fnum);										// 08/17/01 AM.
				delete val;														// 08/17/01 AM.
				break;															// 08/17/01 AM.
			case RS_KBCONCEPT:												// 02/22/00 AM.
			case RS_KBPHRASE:
			case RS_KBATTR:
			case RS_KBVAL:
			case RSPNODE:			// Parse tree node type!			// 10/18/00 AM.
			case RSNODE:			// Parse tree node type!			// 10/18/00 AM.
				iarg->setType(IASEM);
				iarg->setSem(val);
				// USING val, SO DON'T DELETE IT.
				break;
			case RSARGS:														// 10/20/00 AM.
				// Do these need to be copied?							// 10/20/00 AM.
				iarg->setType(IASEM);
				iarg->setSem(val);
				// USING val, SO DON'T DELETE IT.
				break;
			case RSOSTREAM:													// 05/23/01 AM.
				iarg->setType(IASEM);										// 05/23/01 AM.
				iarg->setSem(val);											// 05/23/01 AM.
				// USING val, SO DON'T DELETE IT.						// 05/23/01 AM.
				break;															// 05/23/01 AM.
			case RSVAR:		// Shouldn't need to handle this.
			default:
				parse->line_ = line_;								// 08/24/02 AM.
				_stprintf(Errbuf,												// 05/18/01 AM.
								_T("[Fncall error: Bad arg expr type.]"));	// 05/18/01 AM.
				parse->errOut(false); // UNFIXED 													// 05/18/01 AM.
				sem = new RFASem(0LL);
				this->setArgs(origargs_);			// Restore args.	// 03/13/02 AM.
				Iarg::delete_top(iargs);			// Zap.				// 02/18/00 AM.
				return false;
			}
		}
			break;																// 05/26/02 AM.
		case IAREF:						// Call by reference.			// 05/26/02 AM.
			// Require a variable here.
			semarg = iarg->getSem();										// 05/26/02 AM.
			if (semarg->getType() != RSVAR)								// 05/26/02 AM.
			{
			parse->line_ = line_;									// 08/24/02 AM.
			_stprintf(Errbuf,													// 05/26/02 AM.
				_T("[Error: Call by reference needs var.]"));				// 05/26/02 AM.
			parse->errOut(false); // UNFIXED 														// 05/26/02 AM.
			sem = new RFASem(0LL);									// 05/26/02 AM.
			this->setArgs(origargs_);										// 05/26/02 AM.
			Iarg::delete_top(iargs);										// 05/26/02 AM.
			return false;														// 05/26/02 AM.
			}

			break;
		default:																	// 05/26/02 AM.
			break;
		}
	}

// TEMPORARILY REPLACE ARG LIST WITH COPIED&COMPUTED LIST!	// 02/17/00 AM.
if (iargs)
	this->setArgs(iargs);

/////////////////////////////
/////////////////////////////

bool badname = false;
long long ret = 1;		// Return value for check actions.

// ACTIONS DEPENDENT ON THE CODE REGION.
switch (nlppp->getRegion())
	{
	case REGPOST:
		ok = Pat::postAction(this,nlppp, /*UP*/ badname);
		break;
	case REGCODE:
		ok = Code::codeAction(this, nlppp->getParse(), /*UP*/ badname);
		break;
	case REGCHECK:
		// Now nlppp->fail_ and succeed_ govern rule rejection.	// 06/10/00 AM.
		ok = Pat::checkAction(this,nlppp, /*UP*/ badname);			// 06/10/00 AM.
		break;
// NLP++ user-defined fns inherit the calling region.				// 03/12/02 AM.
//	case REGDECL:																// 03/12/02 AM.
//		ok = false;																// 03/12/02 AM.
//		badname = true;														// 03/12/02 AM.
//		break;																	// 03/12/02 AM.
	default:
		nlppp->parse_->line_ = line_;										// 08/24/02 AM.
		_stprintf(Errbuf,														// 05/18/01 AM.
			_T("[Fncall: Error. Executing in bad region of code.]"));	// 05/18/01 AM.
		parse->errOut(false); // UNFIXED 															// 05/18/01 AM.
		sem = new RFASem(0LL);
		if (iargs)
			{
			this->setArgs(origargs_);				// Restore args.	// 03/13/02 AM.
			Iarg::delete_top(iargs);				// Zap.				// 02/18/00 AM.
			}
		return false;
	}

if (ok)
	{
	// Action/function executed ok.  Return "true".
	sem = new RFASem(ret);
	if (iargs)
		{
		this->setArgs(origargs_);					// Restore args.	// 03/13/02 AM.
		Iarg::delete_top(iargs);					// Zap.				// 02/18/00 AM.
		}
	return true;
	}
else if (!badname)	// Known action with bad result
	{
	sem = new RFASem(0LL);
	if (iargs)
		{
		this->setArgs(origargs_);					// Restore args.	// 03/13/02 AM.
		Iarg::delete_top(iargs);					// Zap.				// 02/18/00 AM.
		}
	return false;
	}

// ACTIONS INDEPENDENT OF CODE REGION GO HERE.
//.....
if ((ok = Fn::fnCall(this, nlppp, /*UP*/ badname, sem)))
	{
	if (iargs)
		{
		this->setArgs(origargs_);					// Restore args.	// 03/13/02 AM.
		Iarg::delete_top(iargs);					// Zap.				// 02/18/00 AM.
		}
	return true;
	}

if (iargs)
	{
	this->setArgs(origargs_);						// Restore args.	// 03/13/02 AM.
	Iarg::delete_top(iargs);						// Zap.				// 02/18/00 AM.
	}

if (sem)
	{
	delete sem;
	sem = 0;
	}

if (!ok)
	{
	nlppp->parse_->line_ = line_;											// 08/24/02 AM.
	if (badname)
		{
		_stprintf(Errbuf,														// 05/18/01 AM.
			_T("[Fncall: Error: Unknown fn/action name=%s]"),			// 05/18/01 AM.
																		name_);	// 05/18/01 AM.
		return parse->errOut(false); // UNFIXED 												// 05/18/01 AM.
		}
	else
		{
		_stprintf(Errbuf,_T("[Fncall: Error executing fn/action=%s]"),	// 05/18/01 AM.
																		name_);	// 05/18/01 AM.
		return parse->errOut(false); // UNFIXED 												// 05/18/01 AM.
		}
	}
// Action/function executed ok.  Return true.
sem = new RFASem(1LL);
return true;
}


/********************************************
* FN:		GEN
* CR:		04/20/00 AM.
* NOTE:	Generate code for a list of actions.
********************************************/

bool Iaction::gen(Dlist<Iaction> *list, Gen *gen)
{
return true;
}


/********************************************
* FN:		GENEVAL
* CR:		05/07/00 AM.
* SUBJ:	Gen evaluation of an NLP++ action.
* NOTE:	Generate explicit function call, with code to eval args.
*			Simply gen code for the number of args specified.  If error,
*			then generated code won't compile, which is fine.
*********************************************/

bool Iaction::genEval(std::_t_ofstream *fcode, Gen *gen,
	bool top			// If action is at top, or stmt level.			// 06/06/00 AM.
	)
{
if (!name_ || !*name_)
	return false;

_TCHAR *indent = gen->indent_;

NLP *nlp = gen->getNLP();		// [DEGLOB]	// 10/15/20 AM.
VTRun *vtrun = nlp->getVTRun();	// [DEGLOB]	// 10/15/20 AM.


//ostream *fcode = gen->fcode_;

// SOME SPECIAL CASES.
if (!strcmp_i(name_,_T("exitpass")))	// EXIT CURRENT PASS.			// 09/03/01 AM.
	{
	// Todo: Handle when exitpass is in a function.					// 07/27/03 AM.
	switch(gen->region_)														// 07/28/03 AM.
		{
		case REGCODE:															// 07/28/03 AM.
			*fcode << indent << _T("Arun::ret(nlppp,0,0,(RFASem*)0);");		// 07/27/03 AM.
			Gen::nl(fcode);														// 07/27/03 AM.
			*fcode << indent << _T("return Arun::exitpass(true,nlppp)");	// 08/07/02 AM.
			break;
		case REGPOST:															// 07/28/03 AM.
			*fcode << indent << _T("Arun::post_final(nlppp);");			// 07/28/03 AM.
			Gen::nl(fcode);													// 07/27/03 AM.
			*fcode << indent << _T("Arun::ret(nlppp,0,0,(RFASem*)0);");		// 07/27/03 AM.
			Gen::nl(fcode);														// 07/27/03 AM.
			*fcode << indent << _T("return Arun::exitpass(true,nlppp)");	// 08/07/02 AM.
			// Todo: Should grab boolean return.
			break;
		case REGCHECK:															// 07/28/03 AM.
			*fcode << indent << _T("Arun::ret(nlppp,0,0,(RFASem*)0);");		// 07/27/03 AM.
			Gen::nl(fcode);														// 07/27/03 AM.
			*fcode << indent << _T("return Arun::exitpass(true,nlppp)");	// 08/07/02 AM.
			break;
		case REGDECL:															// 07/28/03 AM.
			*fcode << indent << _T("Arun::exitpass(true,nlppp);");		// 08/04/03 AM.
			Gen::nl(fcode);														// 07/27/03 AM.
			*fcode << indent << _T("return Arun::ret(nlppp,0,0,(RFASem*)0)");	// 08/04/03 AM.
			break;
		default:
			*fcode << indent << _T("Arun::ret(nlppp,0,0,(RFASem*)0);");		// 07/27/03 AM.
			Gen::nl(fcode);														// 07/27/03 AM.
			*fcode << indent << _T("return Arun::exitpass(true,nlppp)");	// 08/07/02 AM.
			break;
		}

	if (args_)																	// 09/03/01 AM.
		return false;															// 09/03/01 AM.
	return true;																// 09/03/01 AM.
	}
// fail() fail_
if (!strcmp_i(name_, _T("fail")))		// FAIL ON @CHECK CONDITIONS.	// 06/06/00 AM.
	{
	if (args_)
		return false;

	// Cleanest might be to gen an exception throw.
	// MEM LEAK.	Need to clear out sem, dsem variables.			// 08/07/02 AM.
#ifdef _OLD_FAIL
	switch(gen->region_)														// 08/07/02 AM.
		{
		case REGCHECK:															// 08/07/02 AM.
			*fcode << indent 
					 << _T("return Arun::checkfinal(false,nlppp)");		// 08/07/02 AM.
			break;
		case REGPOST:															// 08/07/02 AM.
			*fcode << indent << _T("return false");							// 08/07/02 AM.
			break;
		default:																	// 08/07/02 AM.
			return false;														// 08/07/02 AM.
		}
#endif

	*fcode << indent << _T("Arun::fail(nlppp)");				// 09/15/08 AM.
	return true;
	}
if (!strcmp_i(name_, _T("succeed")))	// SUCCEED IN @CHECK.			// 06/06/00 AM.
	{
#ifdef _OLD_SUCCEED
	if (gen->region_ != REGCHECK)
		{
		// Treat as a noop in all but the @CHECK region.			// 06/06/00 AM.
		*fcode << indent << _T("true");
		return true;
		}

	// Generate a goto.  Flag that a label needs to be gen'ed after the
	// @CHECK actions.
	gen->label_ = true;	// Notify that label is to be gen'd.	// 06/06/00 AM.
	_TCHAR label[64];
	_stprintf(label, _T("L%d_%d_%d"), gen->id_,gen->recid_,gen->ruleid_);
	*fcode << indent << _T("goto ") << label;
#endif
	*fcode << indent << _T("Arun::succeed(nlppp)");				// 09/15/08 AM.
	return true;
	}

#ifdef _OLDCOUTFIX_
if (!strcmp_i(name_, _T("cout")))	// COUT									// 05/12/02 AM.
	{
	*fcode << _T("(&cout)");														// 05/12/02 AM.
	return true;																// 05/12/02 AM.
	}
#endif

// For Gnu C++ compatibility.			// 11/26/06 AM.
_TCHAR fnname[MAXSTR];					// 11/26/06 AM.	// FIX	// 09/15/08 AM.
if (!strcmp_i(name_,_T("and"))		// 11/26/06 AM.
 || !strcmp_i(name_,_T("or"))			// 11/26/06 AM.
 || !strcmp_i(name_,_T("not"))		// 11/26/06 AM.
 || !strcmp_i(name_,_T("system"))		// 11/26/06 AM.	// system()
 || !strcmp_i(name_,_T("cout"))		// 11/26/06 AM.
 )
  {
  _tcscpy(fnname,_T("Arun::vt"));	// FIX.	// 09/15/08 AM.
  _tcscat(fnname,name_);				// FIX	// 09/15/08 AM.
  }
else
  // Quick fix, not optimized to copy name.	// 09/15/08 AM.
  _tcscpy(fnname,name_);				// FIX.	// 09/15/08 AM.

// Gen function call, then code for eval'ing the args.
// nlppp - Hardwired argument to all runtime fns.
if (top) *fcode << indent << _T("Arun::stmt(");

bool userdef = false;	// User-defined in NLP++					// 03/11/02 AM.
if (!scope_ || !*scope_)													// 02/13/01 AM.
	{
//	if (VTRun_Ptr->isBuiltin(fnname))	// 08/28/02 AM.	// [DEGLOB]	// 10/15/20 AM.
	if (vtrun->isBuiltin(fnname))						// [DEGLOB]	// 10/15/20 AM.
		*fcode << _T("Arun::") << fnname << _T("(");
	else																			// 12/24/01 AM.
		{
		userdef = true;				// Flag these.						// 03/11/02 AM.
		*fcode << fnname << _T("(");		// User-defined NLP++ fn.		// 12/24/01 AM.
		}
	}
else		// Fn defined in USER project.								// 02/13/01 AM.
	*fcode << scope_ << _T("::") << fnname << _T("(");							// 02/13/01 AM.

Delt<Iarg> *darg = 0;
if (args_)
	darg = args_->getFirst();
Iarg *iarg;
RFASem *sem;

// Add a hard-wired arg to all runtime fns.
switch (gen->region_)
	{
	case REGCODE:
	case REGCODEFIN:
	case REGCOND:
	case REGDECL:																// 12/25/01 AM.
		//*fcode << "parse";													// 06/04/00 AM.
		*fcode << _T("nlppp");	// USE A "DUMMY" Nlppp object.		// 06/04/00 AM.
		break;
	case REGPRE:
	case REGCHECK:
	case REGPOST:
		*fcode << _T("nlppp");
		break;
	default:
		*fcode << _T("// ERROR: Bad region for function call.");
		return false;
	}

if (darg)
	*fcode << _T(", ");

// Traverse the args, generating code for them.
_TCHAR buf[MAXSTR+1];
long long num = 0;																	// 05/04/01 AM.
for (; darg; darg = darg->Right())
	{
	if (userdef)	// Wrap args of user-defined NLP++ fn.			// 03/11/02 AM.
		*fcode << _T("Arun::sem(");												// 03/11/02 AM.
	iarg = darg->getData();
	switch (iarg->getType())
		{
		case IASEM:			// NLP++.  Gen code to evaluate.
			if (!(sem = iarg->getSem()))
				return false;
			sem->genEval(gen,false);
			break;
		case IASTR:
			*fcode << _T("_T(\"")
					 << c_str(iarg->getStr(),buf,MAXSTR)				// 06/04/00 AM.
					 << _T("\")");
			break;
		case IANUM:
			num = iarg->getNum();											// 05/04/01 AM.
			if (num)																// 05/04/01 AM.
				*fcode
							<< _T("(long)")											// 09/09/01 AM.
							<< num;												// 05/04/01 AM.
			else																	// 05/04/01 AM.
				*fcode << _T("(long)0");				// Unambiguous.		// 05/04/01 AM.
			break;
		case IAFLOAT:															// 08/17/01 AM.
			*fcode	<< _T("(float)")											// 09/09/01 AM.
						<< iarg->getFloat();									// 08/17/01 AM.
			break;																// 08/17/01 AM.
		case IAREF:		// CALL-BY-REFERENCE.							// 06/15/02 AM.
			if (!(sem = iarg->getSem()))									// 06/15/02 AM.
				{
				*fcode << _T("BUG_CALL_BY_REFERENCE");						// 06/15/02 AM.
				break;	// Recover.											// 06/15/02 AM.
				}
			if (sem->getType() != RSVAR)									// 06/15/02 AM.
				{
				*fcode << _T("ERROR_NONVAR_CALL_BY_REFERENCE");			// 06/15/02 AM.
				break;
				}
			// Generate special for call by reference.
			sem->getVar()->genEval(gen,true);							// 06/16/02 AM.

			break;
		case IAOSTREAM:
			if (iarg->getOstream())
				return true;
			return false;
			break;
		default:
			*fcode << _T("\nBUG_FUNCTION_ARG\n");							// 06/15/02 AM.
//			return false;	// Better recovery is continue.			// 06/15/02 AM.
		}
	if (userdef)																// 03/11/02 AM.
		*fcode << _T(")");															// 03/11/02 AM.
	if (darg->Right())
		*fcode << _T(", ");		// Separator.
	}


*fcode << _T(")");
if (top) *fcode << _T(")");		// Close stmt.
return true;
}


/*************************      END OF FILE      ******************************/
