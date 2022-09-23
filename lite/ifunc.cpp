/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	IFUNC.CPP
* FILE:	lite\ifunc.cpp
* CR:		12/19/01 AM.
* SUBJ:	Function-declaration class.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "inline.h"
#include "io.h"
#include "iarg.h"
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
//#include "slist.h"
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"
#include "slist.h"
#include "thtab.h"
#include "pat.h"
#include "fn.h"
#include "lite/code.h"
#include "func_defs.h"		// 12/20/01 AM.
//#include "lite/delt.h"		// 12/20/01 AM.
#include "var.h"				// 03/10/02 AM.
#include "ipair.h"			// 03/10/02 AM.
#include "ivar.h"				// For LOCALVAR,FULLARRAY	// 03/11/02 AM.
#include "parse.h"			// 08/22/02 AM.
#include "chars.h"			// For deleting local strings. // 01/08/07 AM.
#include "ifunc.h"
#include "rfasem.h"

#ifdef LINUX
template<>	// Linux.	// 04/26/07 AM.
int Dlist<Ifunc>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Delt<Ifunc>::count_ = 0;
int Ifunc::count_ = 0;

template<>	// Linux.	// 04/26/07 AM.
int tHtab<Ifunc>::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Slist<tSym<Ifunc> >::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int Selt<tSym<Ifunc> >::count_ = 0;
template<>	// Linux.	// 04/26/07 AM.
int tSym<Ifunc>::count_ = 0;
#else
int Dlist<Ifunc>::count_ = 0;
int Delt<Ifunc>::count_ = 0;
int Ifunc::count_ = 0;

int tHtab<Ifunc>::count_ = 0;
int Slist<tSym<Ifunc> >::count_ = 0;
int Selt<tSym<Ifunc> >::count_ = 0;
int tSym<Ifunc>::count_ = 0;
#endif

/********************************************
* FN:		Special Functions for Ifunc Class
* CR:		12/19/01 AM.
********************************************/

Ifunc::Ifunc(					// Default constructor.
	_TCHAR *nn,					// Action name.
	Dlist<Iarg> *aa,			// Action parameters.
	RFASem *body,				// Function body (block).
	long line,					// Line number
	long pass					// Pass number.
	)
{
name_ = nn;
params_ = aa;
body_ = body;
line_	= line;
pass_ = pass;
id_	= FNNULL;				// 12/26/01 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Ifunc::~Ifunc()
{
if (params_)
	Dlist<Iarg>::DeleteDlistAndData(params_);
if (body_)
	delete body_;

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Ifunc::Ifunc(Ifunc &orig)
{
zero();
Ifunc::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Ifunc &Ifunc::operator=(const Ifunc &fm)
{
Ifunc *to;
to = this;
if (&fm == to)
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Ifunc object to itself.]") << std::ends;
	errOut(&gerrStr,false,0,0);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Ifunc::clear()
{
if (params_)
	Dlist<Iarg>::DeleteDlistAndData(params_);
if (body_)
	delete body_;
}

void Ifunc::zero()
{
name_ = 0;
params_ = 0;
body_ = 0;
line_= 0;
pass_ = 0;
}

void Ifunc::copy(const Ifunc *orig)
{
Ifunc *dest;
dest = this;

dest->name_ = orig->name_;
dest->line_	 = orig->line_;
dest->pass_ = orig->pass_;

if (orig->params_)
	dest->params_ = new Dlist<Iarg>(*(orig->params_));
dest->body_ = orig->body_;	// Need copy constructor.....
}

/*******************************************/

/*******************************************/

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, Ifunc &func)
{
#ifndef UNICODE
output << str(func.name_) << _T("(");
#else
char *lpstr8;																	// 01/28/06 AM.
u_to_mbcs((LPCWSTR)str(func.name_),										// 01/28/06 AM.
										 CP_UTF8, (LPCTSTR*&)lpstr8);		// 01/28/06 AM.
output << lpstr8;																// 01/28/06 AM.
u_delete((LPCTSTR*&)lpstr8);												// 01/28/06 AM.
output << _T("(");
#endif

if (func.params_)
	output << *(func.params_);
if (func.body_)
	output << *(func.body_);
output << _T(")");
output << std::endl << _T("{");
if (func.body_)
	output << *(func.body_);
output << std::endl << _T("}") << std::endl;
return output;
}

/*******************************************/

/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/

_TCHAR			*Ifunc::getName()		{return name_;}
Dlist<Iarg>	*Ifunc::getParams()	{return params_;}
RFASem		*Ifunc::getBody()		{return body_;}
long			 Ifunc::getLine()		{return line_;}
long			 Ifunc::getPass()		{return pass_;}
enum funcDef Ifunc::getId()		{return id_;}

/********************************************
* FN:		Modify Functions
********************************************/

void Ifunc::setName(_TCHAR *x)				{name_	= x;}
void Ifunc::setParams(Dlist<Iarg> *x)	{params_	= x;}
void Ifunc::setBody(RFASem *x)			{body_	= x;}
void Ifunc::setLine(long x)				{line_	= x;}
void Ifunc::setPass(long x)				{pass_	= x;}
void Ifunc::setId(enum funcDef x)		{id_		= x;}


/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Ifunc::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Ifunc::prettyCount(std::_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Ifunc count=") << count_ << std::endl;
	std::_t_strstream gerrStr;
	gerrStr << _T("Active Ifunc count=") << count_ << std::ends;
	errOut(&gerrStr,false,0,0);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		CREATEHTAB
* CR:		12/20/01 AM.
* SUBJ:	Make hash table of function objects.
*********************************************/

void *Ifunc::createHtab(long len)
{
if (len <= 0)
	return 0;

tHtab<Ifunc> *htab;
htab = new tHtab<Ifunc> (Htab::getGstab(), len);
return (void *) htab;
}

/********************************************
* FN:		MAKEHTAB
* CR:		12/20/01 AM.
* SUBJ:	Make hash table of function names.
* NOTE:	Function id numbers assumed to correspond directly to ordering in
*			the array of function names.  Used to build hash table of builtin
*			NLP++ functions.  Assumes array is terminated by a null string ptr.
*********************************************/

void *Ifunc::makeHtab(_TCHAR *arr[])
{
if (!arr)
	return 0;

long fudge = 30;

// Get length of array of strings.
long arrlen = 0;
_TCHAR **ptr = arr;
for (;;)
	{
	++ptr;
	if (*ptr)
		++arrlen;
	else
		break;	// Found end of array.
	}

if (arrlen <= 0)
	return 0;

tHtab<Ifunc> *htab;
long len;				// Size of hash table.
len = arrlen * fudge + (long)1;	// Some breathing room for hashing.
htab = (tHtab<Ifunc> *) createHtab(len);
Selt<tSym<Ifunc> > *selt = 0;
tSym<Ifunc> *sym = 0;
Ifunc *ifunc = 0;

// POPULATE HASH TABLE.
int ii = 0;
for (ptr = (arr + 1); *ptr; ++ptr)
	{
	++ii;	// Position == unique id for function name.
	selt = htab->hadd(*ptr);
	sym = selt->getData();

	// Fill hash object data.
	ifunc = new Ifunc();
	ifunc->name_ = *ptr;
	ifunc->id_ = (enum funcDef) ii;

	sym->setPtr(ifunc);
	}

//htab->pretty();
return (void *)htab;
}


/********************************************
* FN:		CLEANHTAB
* CR:		12/20/01 AM.
* SUBJ:	Special-case cleanup for hash table with function definitions.
* NOTE:	Zero out the pointers to function bodies so they won't be deleted.
*			(They are owned by rulesfile objects, in decls lists.)
*********************************************/

bool Ifunc::cleanHtab(void *ht)
{
if (!ht)
	false;

tHtab<Ifunc> *htab = (tHtab<Ifunc> *) ht;

// For each hash table location, delete the list of rules.
// DON'T delete the pointers to rules, just the list elts and list object.
long ii;
Slist<tSym<Ifunc> > *list;
Selt<tSym<Ifunc> > *ptr;
tSym<Ifunc>  *sym;
Slist<tSym<Ifunc> > *parr;
parr = htab->getParr();
Ifunc *ifunc;

for (ii = 0; ii < htab->getSize(); ++ii)
	{
	list = &(parr[ii]);		// 12/12/98 AM.
	if (!(ptr = list->getFirst()))
		continue;			// Empty hash chain.
	for (; ptr; ptr = ptr->pRight)
		{
		sym = ptr->getData();
		ifunc = sym->getPtr();			// Get function decl.
		ifunc->body_ = 0;		// ZERO OUT SO WON'T BE DELETED.		// 03/10/02 AM.
		ifunc->params_ = 0;	// ZERO OUT SO WON'T BE DELETED.		// 03/10/02 AM.
		}
	}
return true;
}


/********************************************
* FN:		DELETEHTAB
* CR:		12/20/01 AM.
* SUBJ:	Delete hash table of function names.
*********************************************/

bool Ifunc::deleteHtab(void *htab)
{
if (!htab)
	return false;
tHtab<Ifunc> *ht = (tHtab<Ifunc> *) htab;

// ZERO OUT FUNCTION BODIES FIRST SO WON'T BE DELETED....
cleanHtab(htab);

delete ht;
return true;
}

/********************************************
* FN:		LOADHTAB
* CR:		12/20/01 AM.
* SUBJ:	Load hash table with user-defined functions for one pass.
*********************************************/

bool Ifunc::loadHtab(
	void *htab,				// User-defined functions.
	Dlist<Iarg> *list,
	void *htbuiltin,		// Hash table for builtin functions.
	long currseqpass															// 08/23/02 AM.
	)
{
if (!htab)
	return false;
if (!list)
	return true;			// No user-defined functions is just fine.

tHtab<Ifunc> *ht = (tHtab<Ifunc> *) htab;
tHtab<Ifunc> *htbuilt = (tHtab<Ifunc> *) htbuiltin;

Delt<Iarg> *delt = 0;
Iarg *iarg = 0;
RFASem *sem = 0;
_TCHAR *name = 0;
Dlist<Iarg> *params = 0;													// 03/10/02 AM.
RFASem *body = 0;
Ifunc *deffunc = 0;

Selt<tSym<Ifunc> > *selt = 0;
tSym<Ifunc> *sym = 0;
Ifunc *ifunc = 0;
long line = 0;
long pass = 0;

bool ok = true;

// Traverse decls (they're all function definitions for now).
for (delt = list->getFirst(); delt; delt = delt->Right())
	{
	// Unpack the func declaration object.
	iarg = delt->getData();
	if (iarg->getType() != IASEM)
		return false;
	sem = iarg->getSem();
	if (sem->getType() != RSFUNC)
		return false;
	deffunc = sem->getFunc();
	name = deffunc->name_;
	params = deffunc->params_;												// 03/10/02 AM.
	body = deffunc->body_;
	line = deffunc->line_;													// 12/21/01 AM.
	pass = deffunc->pass_;													// 12/21/01 AM.

	// Check that function name is not already in builtin table.
	if (htbuilt->hfind(name))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Can't use built-in NLP++ function name='")
				<< name << _T("']") << std::ends;
		errOut(&gerrStr,false,currseqpass,line);
		ok = false;
		continue;
		}

	// Check that function name is not already in user-defined table.
	if (ht->hfind(name))
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Multiply defined user function name='")
				<< name << _T("']") << std::ends;
		errOut(&gerrStr,false,currseqpass,line);
		ok = false;
		continue;
		}

	// Load hash table with name and body if everything ok.

	selt = ht->hadd(name);
	sym = selt->getData();

	// Fill hash object data.
	ifunc = new Ifunc();
	ifunc->name_ = name;
	ifunc->params_ = params;	// NOT COPYING.						// 03/10/02 AM.
	ifunc->body_ = body;	// NOT COPYING, SO WATCH OUT ON DELETION.
	ifunc->line_ = line;
	ifunc->pass_ = pass;

	sym->setPtr(ifunc);
	}

return ok;		// Report any errors up the stack.					// 12/21/01 AM.
}

/********************************************
* FN:		HTLOOKUP
* CR:		12/21/01 AM.
* SUBJ:	Lookup function name in hash table.
*********************************************/

Ifunc *Ifunc::htLookup(
	_TCHAR *name,
	void *htab
	)
{
Selt<tSym<Ifunc> > *selt = ((tHtab<Ifunc> *)htab)->hfind(name);
if (!selt)
	return 0;
tSym<Ifunc> *sym = selt->getData();
return sym->getPtr();
}

/********************************************
* FN:		FUNCID
* CR:		12/21/01 AM.
* SUBJ:	Lookup function ID in builtins hash table.
*********************************************/

enum funcDef Ifunc::funcID(
	_TCHAR *name,
	void *htab				// User-defined functions.
	)
{
Selt<tSym<Ifunc> > *selt = ((tHtab<Ifunc> *)htab)->hfind(name);
if (!selt)
	return FNNULL;
tSym<Ifunc> *sym = selt->getData();
Ifunc *ifunc = sym->getPtr();
return ifunc->id_;
}

/********************************************
* FN:		EVAL
* CR:		12/21/01 AM.
* SUBJ:	Interpretively evaluate a user-defined function.
* NOTE:	Not handling args, recursion, return statements for now.
*********************************************/

bool Ifunc::eval(
	Delt<Iarg> *args,
	Nlppp *nlppp,
	/*UP*/
	RFASem* &sem
	)
{
sem = 0;
Parse *parse = nlppp->parse_;												// 08/22/02 AM.

if (++(nlppp->depth_) > MAX_DEPTH)	// Push nesting level.		// 03/12/02 AM.
	{
	std::_t_strstream gerrStr;						// 03/12/02 AM.
	gerrStr << _T("[Error: Too deeply nested function '")				// 03/12/02 AM.
		<< name_ << _T("']") << std::ends;											// 03/12/02 AM.
	parse->errOut(&gerrStr,false);													// 08/22/02 AM.
	return false;													// FIX.	// 10/05/04 AM.
	}

// 03/09/02 AM.
// Even if body is empty, there could be side-effects in the args, so
// evaluate them.
// If args, BIND them to the params_ list of names.  Create a local var
// list, store it in this eval instance.
// Place it onto the nlppp locals_ list, replacing whatever was there.
//		SAVE WHAT WAS THERE AND RESTORE BEFORE RETURNING.
// DELETE the current locals_ list before returning.
// In this way, we manage the "stack" of local variables. RECURSIVE FUNCTION CALLS ETC.
Dlist<Ipair> *savelocals = nlppp->locals_;							// 03/10/02 AM.
Dlist<Ipair> *locals = 0;													// 03/10/02 AM.

// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
Slist<_TCHAR> *savelocalstrs = nlppp->localstrs_;					// 01/08/07 AM.
nlppp->localstrs_ = 0;	// Zero locals for fn call.				// 01/08/07 AM.

if (args && params_)															// 03/10/02 AM.
	{
	// Traverse and align args with params, as well as possible.
	locals = bindParams(args, params_->getFirst(), nlppp);		// 03/10/02 AM.
	
//	strstream gerrStr(Errbuf,MAXSTR,std::ios::out);
//	gerrStr << "[Ignoring args in user-defined function.]" << std::ends;
//	errOut(false);																// 03/09/02 AM.
	}
nlppp->locals_ = locals;	// "Push onto stack".					// 03/10/02 AM.

if (!body_)
	{
	if (nlppp->locals_)														// 03/10/02 AM.
		Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);			// 03/10/02 AM.
	nlppp->locals_ = savelocals;	// POP.								// 03/10/02 AM.
	return true;		// Empty function is ok.
	}

bool ok = true;																// 03/09/02 AM.

// Save current programmatic context.									// 03/08/02 AM.
// Functions inherit the context that they are called from.		// 03/12/02 AM.
//enum NlpppRegion savereg = nlppp->region_;							// 03/12/02 AM.
//nlppp->region_ = REGDECL;		// Keeping old context.			// 03/12/02 AM.

bool saveinsidefn = nlppp->insidefn_;	// Save.						// 03/12/02 AM.
nlppp->insidefn_ = true;					// Push.						// 03/12/02 AM.

// Pass number for error-reporting is the pass that func was defined in.
long savepass = parse->currpass_;										// 08/22/02 AM.
parse->currpass_ = pass_;													// 08/22/02 AM.
long saveline = parse->line_;												// 02/02/05 AM.
parse->line_ = line_;														// 02/02/05 AM.
if (!body_->eval(nlppp, /*UP*/ sem))
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Error executing user-defined function '")
		<< name_ << _T("']") << std::ends;
	parse->errOut(&gerrStr,false);													// 08/22/02 AM.
	ok = false;																	// 03/09/02 AM.
	}
if (nlppp->locals_)															// 03/10/02 AM.
	Dlist<Ipair>::DeleteDlistAndData(nlppp->locals_);				// 03/10/02 AM.

// LOCAL STRING, TEMPORARY STRING MANAGEMENT.						// 01/08/07 AM.
nlppp->freeLocals(savelocalstrs);										// 01/08/07 AM.

--(nlppp->depth_);		// Pop nesting level.						// 03/12/02 AM.
nlppp->locals_ = savelocals;	// POP.									// 03/10/02 AM.
parse->currpass_ = savepass;	// Restore current pass num.		// 08/22/02 AM.
parse->line_ = saveline;		// Restore current line.			// 02/02/05 AM.
//nlppp->region_ = savereg;	// Restore.								// 03/12/02 AM.
nlppp->insidefn_ = saveinsidefn;	// POP.								// 03/12/02 AM.

// IF NOT RETURNING A VALUE, ZERO IT OUT.					// FIX	// 09/15/08 AM.
if (!nlppp->return_)												// FIX	// 09/15/08 AM.
	{
	delete sem;														// FIX	// 09/15/08 AM.
	sem = 0;															// FIX	// 09/15/08 AM.
	}

nlppp->return_ = false;		// RESET THIS FLAG.						// 03/07/02 AM.
return ok;																		// 03/09/02 AM.
}


/********************************************
* FN:		BINDPARAMS
* CR:		03/10/02 AM.
* SUBJ:	Bind function parameters to actual args.
*********************************************/

Dlist<Ipair> *Ifunc::bindParams(
	Delt<Iarg> *args,
	Delt<Iarg> *params,
	Nlppp *nlppp
	)
{
if (!args && !params)														// 07/02/02 AM.
	return 0;																	// 07/02/02 AM.
if (!args || !params)	// Unequal lengths.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning(1). Bad number of arguments to function '")
		<< name_ << _T("']") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);											// 08/22/02 AM.
	}


Dlist<Ipair> *locals = 0;
Iarg *arg;
Iarg *param;
_TCHAR *varname;
Iarg *narg;	// Copy value.
Dlist<Iarg> *vals;
RFASem *sem;																	// 11/13/02 AM.
for (; args && params;											// FIX.	// 07/02/02 AM.
		args = args->Right(), params = params->Right())
	{
	arg = args->getData();
	param = params->getData();
	varname = param->getStr();
	// Now make a local variable.
	// Handling value from multi-valued var.				// FIX.	// 11/13/02 AM.
	if (arg->getType() == IASEM								// FIX.	// 11/13/02 AM.
	 && (sem = arg->getSem())									// FIX.	// 11/13/02 AM.
	 && (sem->getType() == RSARGS) )							// FIX.	// 11/13/02 AM.
		vals = Iarg::copy_args(sem->getArgs());			// FIX.	// 11/13/02 AM.
	else																			// 11/13/02 AM.
		{
	//	narg = new Iarg(*arg);	// COPY value.				// BUG	// 04/08/02 AM.
		narg = Iarg::copy_arg(arg);							// FIX	// 04/08/02 AM.
		vals = new Dlist<Iarg>(narg);
		}
	Var::setVals(varname, vals, /*DU*/ locals);
	}

if (args || params)	// Unequal lengths.
	{
	std::_t_strstream gerrStr;
	gerrStr << _T("[Warning(2). Bad number of arguments to function '")
		<< name_ << _T("']") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);											// 08/22/02 AM.
	}
return locals;
}


/********************************************
* FN:		GENDECLS
* CR:		12/19/01 AM.
* SUBJ:	Gen code for declarations.
* NOTE:	Placing this here since only function bodies are declarations,
*			for now.
*********************************************/

bool Ifunc::genDecls(Dlist<Iarg> *decls, Gen *gen)
{
// Traverse the decl list, outputting eval code for each function.
// Templates into the header file.
if (!decls)
	return true;	// No user-defined functions is ok.

std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
std::_t_ofstream *fhead = gen->fhead_;

Delt<Iarg> *darg = decls->getFirst();
Iarg *iarg;
RFASem *sem;
Ifunc *ifunc;
long len;	// Length of parameter list.								// 03/11/02 AM.
long ii;																			// 03/11/02 AM.
Delt<Iarg> *param;															// 03/11/02 AM.
Iarg *varg;																		// 03/11/02 AM.
for (; darg; darg = darg->Right())
	{
	iarg = darg->getData();
	if (iarg->getType() != IASEM)
		continue;
	sem = iarg->getSem();
	if (sem->getType() != RSFUNC)
		continue;
	ifunc = sem->getFunc();

	if (ifunc->params_)														// 03/11/02 AM.
		{
		len = ifunc->params_->getLength();								// 03/11/02 AM.
		param = ifunc->params_->getFirst();								// 03/11/02 AM.
		}
	else																			// 03/11/02 AM.
		{
		len = 0;																	// 03/11/02 AM.
		param = 0;																// 03/11/02 AM.
		}

	// User-defined NLP++ functions now always return RFASem* type,
	// so that any type can be returned dynamically.				// 03/07/02 AM.
	*fhead << _T("RFASem *") << ifunc->name_ << _T("(Nlppp*");
	for (ii = 1; ii <= len; ++ii)											// 03/11/02 AM.
		*fhead << _T(",RFASem*");												// 03/11/02 AM.
	*fhead << _T(");");	// Declaration.
	Gen::nl(fhead);															// 04/04/03 AM.

	*fcode << _T("RFASem *") << ifunc->name_ << _T("(Nlppp *nlppp");		// 03/11/02 AM.
	for (ii = 1; ii <= len; ++ii)											// 03/11/02 AM.
		*fcode << _T(",RFASem*L") << ii;										// 03/11/02 AM.
	*fcode << _T(")");																// 03/11/02 AM.
	Gen::nl(fcode);															// 04/04/03 AM.
	*fcode << _T("{");
	Gen::nl(fcode);															// 04/04/03 AM.
	*fcode << _T("void *loc; void *locstrs;");						// 01/08/07 AM.
	Gen::nl(fcode);															// 01/08/07 AM.
	*fcode << _T("Arun::fnstart(nlppp,loc,locstrs);");				// 01/08/07 AM.
	Gen::nl(fcode);															// 04/04/03 AM.
	// Assign arguments to L() variables.								// 03/11/02 AM.
	for (ii=1; param; param = param->Right(),++ii)					// 03/11/02 AM.
		{
		varg = param->getData();											// 03/11/02 AM.
		// NOTE: Wrapping in stmt so SEM will be deleted.			// 03/11/02 AM.
		*fcode << _T("Arun::stmt(Arun::assign(")							// 03/11/02 AM.
				 << LOCALVAR << _T(", ")											// 03/11/02 AM.
				 << _T("_T(\"") << varg->getStr() << _T("\"), 0,")					// 03/11/02 AM.
				 << FULLARRAY													// 03/11/02 AM.
				 << _T(", nlppp, ")												// 03/11/02 AM.
				 << _T("L") << ii << _T("));");										// 03/11/02 AM.
		Gen::nl(fcode);														// 04/04/03 AM.
		}
	gen->region_ = REGDECL;
	if (ifunc->body_)
		{
		// PUTTING A TRY BLOCK FOR SUCCEED,FAIL,EXITPASS.			// 09/15/08 AM.
		*fcode << _T("try {");												// 09/15/08 AM.
		Gen::nl(fcode);														// 09/15/08 AM.

		ifunc->body_->genEval(gen,false);

		// *fcode << _T("} catch (int e) {Arun::ret(nlppp,loc,locstrs,(long)0); throw e;}");// 09/15/08 AM.
		*fcode << _T("} catch (int e) {Arun::excepth(nlppp,loc,locstrs,e); throw e;}");// 03/09/14 AM.
		Gen::nl(fcode);														// 09/15/08 AM.
		}
	*fcode << _T("return Arun::ret(nlppp,loc,locstrs,(long)0);");// 03/11/02 AM.// 01/08/07 AM.
	Gen::nl(fcode);															// 04/04/03 AM.
	*fcode << _T("}");
	Gen::nl(fcode);															// 04/04/03 AM.
	Gen::eol(fcode);															// 04/04/03 AM.
	}

return true;
}

/********************************************
* FN:		GEN
* CR:		12/19/01 AM.
* SUBJ:	Gen code for function declaration.
*********************************************/

bool Ifunc::gen(std::_t_ostream *fcode, Gen *gen)
{
return true;
}

/*************************      END OF FILE      ******************************/
