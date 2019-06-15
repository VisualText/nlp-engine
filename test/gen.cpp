/*******************************************************************************
Copyright (c) 2001 by Text Analysis International, Inc.
All rights reserved.  No part of this document may be copied, used, or
modified without written permission from Text Analysis International, Inc.
********************************************************************************
*
* NAME:	GEN.CPP
* FILE:	lite/gen.cpp
* CR:		05/08/00 AM.
* SUBJ:	Control code generation for analyzer.
*
*******************************************************************************/

#include "StdAfx.h"
//#include <iostream> // 07/18/03 AM.
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/lite.h"				// 07/07/03 AM.
#include "lite/global.h"
#include "dlist.h"					// 07/07/03 AM.
#include "lite/mach.h"														// 05/10/00 AM.
#include "inline.h"
#include "io.h"
#include "iarg.h"
#include "str.h"
#include "node.h"	// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "parse.h"
#include "ana.h"
#include "gen.h"

int Gen::count_ = 0;

/********************************************
* FN:		Special Functions for Gen Class
********************************************/

Gen::Gen(						// Default constructor.
	_TCHAR *indent,				// Amount to indent code.
	_t_ofstream *fcode,			// Main output file.
	_t_ofstream *fhead,			// Header output file.
	_t_ofstream *fdata,			// Data output file.
	_t_ofstream *faux,			// Auxiliary output file.				// 06/05/00 AM.
	int id,						// Id for numbering.
	NlpppRegion region		// Pass file region being gen'd.
	)
{
indent_	= indent;
fcode_	= fcode;
fhead_	= fhead;
fdata_	= fdata;
faux_		= faux;
ehash_	= 0;
edata_	= 0;
ehead_	= 0;
rhash_	= 0;
rchain_	= 0;
rdata_	= 0;
rhead_	= 0;
id_		= id;
ruleid_	= 0;
eltid_	= 0;
recid_	= 0;																	// 05/30/00 AM.
region_	= region;
label_	= false;
nlp_		= 0;																	// 12/22/01 AM.

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/

Gen::~Gen()
{
if (fcode_)
	delete fcode_;
if (fhead_)
	delete fhead_;
if (fdata_)
	delete fdata_;
if (faux_)
	delete faux_;
if (ehash_)
	delete ehash_;
if (edata_)
	delete edata_;
if (ehead_)
	delete ehead_;
if (rhash_)
	delete rhash_;
if (rchain_)
	delete rchain_;
if (rdata_)
	delete rdata_;
if (rhead_)
	delete rhead_;

#ifndef STABLE_
--count_;
#endif
}


/*******************************************/
// Copy constructor.
/*******************************************/
Gen::Gen(Gen &orig)
{
zero();
Gen::copy(&orig);

#ifndef STABLE_
++count_;
#endif
}

/*******************************************/
// Assignment operator overload.
/*******************************************/
const Gen &Gen::operator=(const Gen &fm)
{
Gen *to;
to = this;
if (&fm == to)
	{
	_t_strstream gerrStr;
	gerrStr << _T("[Can't assign Gen object to itself.]") << ends;
	errOut(&gerrStr,false);
	return *this;
	}

to->clear();			// Delete stuff in the object.
to->zero();				// Zero out fields of the object.
to->copy(&fm);			// Copy from the given object.

return *this;
}
/*******************************************/
void Gen::clear()
{
}

void Gen::zero()
{
indent_	= _T("");
fcode_	= 0;
fhead_	= 0;
fdata_	= 0;
faux_		= 0;
ehash_	= 0;
edata_	= 0;
ehead_	= 0;
rhash_	= 0;
rchain_	= 0;
rdata_	= 0;
rhead_	= 0;
id_		= 0;
ruleid_	= 0;
eltid_	= 0;
recid_	= 0;
region_	= REGNULL;
label_	= false;
nlp_		= 0;																	// 12/22/01 AM.
}

void Gen::copy(const Gen *orig)
{
Gen *dest;
dest = this;

dest->indent_	= orig->indent_;
dest->fcode_	= orig->fcode_;
dest->fhead_	= orig->fhead_;
dest->fdata_	= orig->fdata_;
dest->faux_		= orig->faux_;
dest->ehash_	= orig->ehash_;
dest->edata_	= orig->edata_;
dest->ehead_	= orig->ehead_;
dest->rhash_	= orig->rhash_;
dest->rchain_	= orig->rchain_;
dest->rdata_	= orig->rdata_;
dest->rhead_	= orig->rhead_;
dest->id_		= orig->id_;
dest->ruleid_	= orig->ruleid_;
dest->eltid_	= orig->eltid_;
dest->recid_	= orig->recid_;
dest->region_	= orig->region_;
dest->label_	= orig->label_;
dest->nlp_		= orig->nlp_;
}

/*******************************************/

/*******************************************/

_t_ostream &STDOPERATOR<<(_t_ostream &output, Gen &gen)
{
output << _T("[GEN object]") << endl;

return output;
}

/*******************************************/
/*******************************************/

/*******************************************/

/*******************************************/

/*******************************************/
/*******************************************/


/********************************************
* FN:		Access Functions
********************************************/


_TCHAR				*Gen::getIndent()		{return indent_;}
_t_ofstream			*Gen::getFcode()		{return fcode_;}
_t_ofstream			*Gen::getFhead()		{return fhead_;}
_t_ofstream			*Gen::getFdata()		{return fdata_;}
_t_ofstream			*Gen::getFaux()		{return faux_;}
_t_ofstream			*Gen::getEhash()		{return ehash_;}
_t_ofstream			*Gen::getEdata()		{return edata_;}
_t_ofstream			*Gen::getEhead()		{return ehead_;}
_t_ofstream			*Gen::getRhash()		{return rhash_;}
_t_ofstream			*Gen::getRchain()		{return rchain_;}
_t_ofstream			*Gen::getRdata()		{return rdata_;}
_t_ofstream			*Gen::getRhead()		{return rhead_;}
int				 Gen::getId()			{return id_;}
int				 Gen::getRuleid()		{return ruleid_;}
int				 Gen::getEltid()		{return eltid_;}
int				 Gen::getRecid()		{return recid_;}
enum NlpppRegion  Gen::getRegion()	{return region_;}
bool				 Gen::getLabel()		{return label_;}
NLP				*Gen::getNLP()			{return nlp_;}


/********************************************
* FN:		Modify Functions
********************************************/

void	Gen::setIndent(_TCHAR *x)					{indent_ = x;}
void	Gen::setFcode(_t_ofstream *x)				{fcode_	= x;}
void	Gen::setFhead(_t_ofstream *x)				{fhead_	= x;}
void	Gen::setFdata(_t_ofstream *x)				{fdata_	= x;}
void	Gen::setFaux(_t_ofstream *x)				{faux_	= x;}
void	Gen::setEhash(_t_ofstream *x)				{ehash_	= x;}
void	Gen::setEdata(_t_ofstream *x)				{edata_	= x;}
void	Gen::setEhead(_t_ofstream *x)				{ehead_	= x;}
void	Gen::setRhash(_t_ofstream *x)				{rhash_	= x;}
void	Gen::setRchain(_t_ofstream *x)			{rchain_	= x;}
void	Gen::setRdata(_t_ofstream *x)				{rdata_	= x;}
void	Gen::setRhead(_t_ofstream *x)				{rhead_	= x;}
void	Gen::setId(int x)							{id_		= x;}
void	Gen::setRuleid(int x)					{ruleid_	= x;}
void	Gen::setEltid(int x)						{eltid_	= x;}
void	Gen::setRecid(int x)						{recid_	= x;}
void	Gen::setRegion(enum NlpppRegion x)	{region_ = x;}
void	Gen::setLabel(bool x)					{label_	= x;}
void	Gen::setNLP(NLP *x)						{nlp_		= x;}

/********************************************
* FN:		GETCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
int Gen::getCount() { return count_; }
#endif

/********************************************
* FN:		PRETTYCOUNT
* NOTE:	Class function.
********************************************/
#ifndef STABLE_
void Gen::prettyCount(_t_ofstream *ofstr)
{
if (count_)
	{
	if (ofstr)
		*ofstr << _T("Active Gen count=") << count_ << endl;
	*gout << _T("Active Gen count=") << count_ << endl;
	_t_strstream gerrStr;
	gerrStr << _T("Active Gen count=") << count_ << ends;
	errOut(&gerrStr,false);
	}
}
#endif

///////////////////////////////////////////////////////////////////
// GENERAL
///////////////////////////////////////////////////////////////////

/********************************************
* FN:		RESET
* CR:		05/16/00 AM.
* SUBJ:	Finish up code generation, reset for next time.
* NOTE:	This will enable debugging the generated code.  Otherwise,
*			the output code files are left hanging open.
********************************************/

bool Gen::reset()
{
id_ = ruleid_ = eltid_ = recid_ = 0;
fcode_->close();
fhead_->close();
fdata_->close();
faux_->close();
ehash_->close();
edata_->close();
ehead_->close();
rhash_->close();
rchain_->close();
rdata_->close();
rhead_->close();
return true;
}

/********************************************
* FN:		WRITEHEADERS
* CR:		05/10/00 AM.
* SUBJ:	Write headers for generated code files.
********************************************/

bool Gen::writeHeaders()
{
// Should write date and time stamp.
*fcode_ << _T("// Automatically generated: ") << today() << endl;
*fhead_ << _T("// Automatically generated: ") << today() << endl;
*fdata_ << _T("// Automatically generated: ") << today() << endl;
*faux_  << _T("// Automatically generated: ") << today() << endl;

*fdata_ << _T("// INCLUDED IN ANALYZER.CPP ONLY.") << endl;

//*fcode_ << "#include \"stdafx.h\"" << endl;
*fcode_ << _T("#include \"analyzer.h\"") << endl;
*fcode_ << _T("#include \"ehead.h\"") << endl;
*fcode_ << _T("#include \"rhead.h\"") << endl;
*fcode_ << _T("#include \"data.h\"") << endl;
*fcode_ << _T("#include \"..\\user\\user.h\"") << endl;				// 02/13/01 AM.
*fcode_ << _T("#include \"..\\user\\Ucode.h\"") << endl;				// 02/13/01 AM.

// analyzer.h file
//*fhead_ << "#include \"stdafx.h\"" << endl;
#ifndef LINUX
*fhead_ << _T("#include <windows.h>") << endl;
#endif
*fhead_ << _T("#include <stdlib.h>") << endl;
*fhead_ << _T("#include <stdio.h>") << endl;
*fhead_ << _T("#include <tchar.h>") << endl;				// Unicode	// 02/07/05 AM.
*fhead_ << _T("#include <my_tchar.h>") << endl;			// Unicode	// 02/07/05 AM.
*fhead_ << _T("#include <streamClasses.h>") << endl;	// Unicode	// 02/07/05 AM.
*fhead_ << _T("#include \"lite\\Arun.h\"") << endl;
*fhead_ << endl;

*faux_ << _T("#include \"analyzer.h\"") << endl;
*faux_ << endl;

*ehash_  << _T("// Automatically generated: ") << today() << endl;
*ehash_ << _T("#include \"analyzer.h\"") << endl;							// 02/07/05 AM.
*ehash_ << _T("#include \"ehead.h\"") << endl;
*ehash_ << _T("#include \"edata.h\"") << endl;

*ehead_  << _T("// Automatically generated: ") << today() << endl;
*ehead_ << _T("#include <stdlib.h>") << endl;
*ehead_ << _T("#include <stdio.h>") << endl;
*ehead_ << _T("#include \"lite\\Arun.h\"") << endl;

*edata_  << _T("// Automatically generated: ") << today() << endl;
*edata_ << _T("// INCLUDED IN EHASH.CPP ONLY.") << endl;

*rhash_  << _T("// Automatically generated: ") << today() << endl;
*rhash_ << _T("#include \"analyzer.h\"") << endl;							// 02/07/05 AM.
*rhash_ << _T("#include \"rhead.h\"") << endl;
*rhash_ << _T("#include \"rdata.h\"") << endl;
*rhash_ << _T("#include \"rchain.h\"") << endl;

*rchain_  << _T("// Automatically generated: ") << today() << endl;
*rchain_ << _T("// INCLUDED IN RHASH.CPP ONLY.") << endl;

*rhead_  << _T("// Automatically generated: ") << today() << endl;
*rhead_ << _T("#include <stdlib.h>") << endl;
*rhead_ << _T("#include <stdio.h>") << endl;
*rhead_ << _T("#include \"lite\\Arun.h\"") << endl;

*rdata_  << _T("// Automatically generated: ") << today() << endl;
*rdata_ << _T("// INCLUDED IN RHASH.CPP ONLY.") << endl;

return true;
}


/********************************************
* FN:		EOL
* CR:		04/04/03.
* SUBJ:	Flush output buffer and possibly end of line.
* NOTE:	With large generated C++ files, running into C4049,
*			line limit of VC++ of 64K (65536).
*			But apparently a line can be very very long,
*			so omitting newlines by default, unless GENPRETTY_ is defined.
********************************************/

void Gen::eol(_t_ofstream *ostr)
{
#ifdef GENPRETTY_
*ostr << endl;
#else
*ostr << flush;
#endif
}


/********************************************
* FN:		NL
* CR:		04/04/03.
* SUBJ:	Just write out a newline, no buffer flushing.
* NOTE:	With large generated C++ files, running into C4049,
*			line limit of VC++ of 64K (65536).
*			But apparently a line can be very very long,
*			so omitting newlines by default, unless GENPRETTY_ is defined.
********************************************/

void Gen::nl(_t_ofstream *ostr)
{
#ifdef GENPRETTY_
*ostr << _T("\n");
#endif
}

/*************************      END OF FILE      ******************************/
