/*******************************************************************************
Copyright (c) 2001-2015 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	RFASEM.CPP
* FILE:	lite\sem.cpp
* CR:		11/05/98 AM.rfa
* SUBJ:	Node's semantic information.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#include "u_out.h"		// 01/19/06 AM.
#include "lite/global.h"
#include "lite/lite.h"				// 07/07/03 AM.
#include "dlist.h"					// 07/07/03 AM.

#include "io.h"							// 05/10/00 AM.
#include "node.h"				// 07/07/03 AM.
#include "tree.h"	// 07/07/03 AM.
#include "nlppp.h"	// 07/07/03 AM.
#include "gen.h"							// 05/08/00 AM.
#include "ifile.h"						// 02/27/01 AM.
#include "irecurse.h"					// 02/27/01 AM.
#include "iexpr.h"						// 11/09/99 AM.
#include "ivar.h"							// 11/09/99 AM.
#include "iop.h"							// 11/09/99 AM.
#include "ipnode.h"						// 10/18/00 AM.
#include "pn.h"							// 10/18/00 AM.
#include "ifunc.h"						// 12/19/01 AM.
#include "istmt.h"						// 11/11/99 AM.
#include "parse.h"	// For interning strings.	// 01/09/07 AM.
#include "rfasem.h"

/********************************************
* FN:		Special Functions for Sem class.
********************************************/


RFASem::RFASem(			// Default constructor.
	enum RFASemtype type
	)
	: Sem()
{
type_ = type;
val_.ptr_ = 0;
}

RFASem::RFASem(Dlist<Irule>  *x) : Sem() {val_.rules_   = x; type_ = RSRULES; }
RFASem::RFASem(Ifile			  *x) : Sem() {val_.rulesfile_ = x; type_ = RSFILE;}
RFASem::RFASem(Slist<Irecurse> *x) : Sem()
												    {val_.recurses_ = x; type_ = RSRECURSES;}
RFASem::RFASem(Irecurse		   *x) : Sem() {val_.recurse_ = x; type_ = RSRECURSE;}
RFASem::RFASem(Slist<Iregion> *x) : Sem() {val_.regions_ = x; type_ = RSREGIONS; }
RFASem::RFASem(Iregion			*x) : Sem() {val_.region_  = x; type_ = RSREGION; }
RFASem::RFASem(Irule			  *x) : Sem() {val_.rule_    = x; type_ = RSRULE;  }
RFASem::RFASem(Isugg			  *x) : Sem() {val_.sugg_	  = x; type_ = RSSUGG;  }
RFASem::RFASem(Dlist<Ielt>   *x) : Sem() {val_.phrase_  = x; type_ = RSPHRASE;}
RFASem::RFASem(Dlist<Ipre>   *x) : Sem() {val_.pres_	  = x; type_ = RSPRES;  }
RFASem::RFASem(Dlist<Ipost>  *x) : Sem() {val_.posts_   = x; type_ = RSPOSTS; }
RFASem::RFASem(Ielt			  *x) : Sem() {val_.rulelt_  = x; type_ = RSRULELT;}
RFASem::RFASem(Dlist<Ipair>  *x) : Sem() {val_.pairs_   = x; type_ = RSPAIRS; }
RFASem::RFASem(Dlist<Iaction>*x) : Sem() {val_.actions_ = x; type_ = RSACTIONS;}
RFASem::RFASem(Iarg			  *x) : Sem() {val_.arg_	  = x; type_ = RSARG;   }
RFASem::RFASem(Ipair			  *x) : Sem() {val_.pair_	  = x; type_ = RSPAIR;  }
RFASem::RFASem(Iaction		  *x) : Sem() {val_.action_  = x; type_ = RSACTION;}
RFASem::RFASem(Ipre			  *x)	: Sem() {val_.pre_	  = x; type_ = RSPRE;   }
RFASem::RFASem(long long			   x)	: Sem() {val_.long_	  = x; type_ = RSLONG;	}
RFASem::RFASem(float				x) : Sem() {val_.float_	  = x; type_ = RSFLOAT; }
RFASem::RFASem(std::_t_ostream		  *x) : Sem() {val_.ostr_	  = x; type_ = RSOSTREAM;}
RFASem::RFASem(Ifunc			  *x) : Sem() {val_.func_	  = x; type_ = RSFUNC;}

RFASem::RFASem(Dlist<Iarg>   *x,
	enum RFASemtype type) : Sem()		// Modified 03/25/99 AM.
{val_.args_	  = x; type_ = type;  }				// 03/25/99 AM.

// To handle various types that share a string.			// 11/20/98 AM.
RFASem::RFASem(_TCHAR *x, enum RFASemtype type)
	: Sem()
{
val_.name_ = x;
type_ = type;
}

///////// NLP++ SEMANTICS

RFASem::RFASem(Slist<Istmt> *x) : Sem() {val_.stmts_ = x; type_ = RSSTMTS;}

RFASem::RFASem(Istmt *x) : Sem() {val_.stmt_ = x; type_ = RSSTMT;}
RFASem::RFASem(Iexpr *x) : Sem()	{val_.expr_	= x; type_ = RSEXPR;}
RFASem::RFASem(Ivar  *x) : Sem() {val_.var_  = x; type_ = RSVAR; }
RFASem::RFASem(Iop   *x) : Sem() {val_.op_   = x; type_ = RSOP;  }

///////// KB HANDSHAKE

RFASem::RFASem(void *x, enum RFASemtype type, CG *cg)
	: Sem()
{
type_ = type;
val_.cg_ = cg;
switch (type)
	{
	case RS_KBCONCEPT:
		val_.kbconcept_ = x;
		break;
	case RS_KBPHRASE:
		val_.kbphrase_ = x;
		break;
	case RS_KBATTR:
		val_.kbattr_ = x;
		break;
	case RS_KBVAL:
		val_.kbval_ = x;
		break;
	default:
		type_ = RSNULL;
		break;
	}
}

///////// PARSE TREE OBJECTS			// 10/18/00 AM.

RFASem::RFASem(Ipnode   *x) : Sem() {val_.pnode_ = x; type_ = RSPNODE; }
RFASem::RFASem(Node<Pn> *x) : Sem() {val_.node_  = x; type_ = RSNODE;  }


//////////////////////////

RFASem::~RFASem()
{
// 11/17/98 AM.
switch (type_)
	{
	case RSFILE:
		if (val_.rulesfile_)
			delete val_.rulesfile_;
		break;
	case RSGLOBAL:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Pass file globals not implemented yet.]") << std::ends;
		errOut(&gerrStr,false);
		}
		break;
	case RSRECURSES:
		if (val_.recurses_)
			delete val_.recurses_;
			//Slist<Irecurse>::DeleteSlistAndData(val_.recurses_);
		break;
	case RSRECURSE:
		if (val_.recurse_)
			delete val_.recurse_;
		break;
	case RSREGIONS:
		if (val_.regions_)
			delete val_.regions_;
			//Slist<Iregion>::DeleteSlistAndData(val_.regions_);
		break;
	case RSREGION:
		if (val_.region_)
			delete val_.region_;
		break;
	case RSRULES:
		if (val_.rules_)
			Dlist<Irule>::DeleteDlistAndData(val_.rules_);
		break;
	case RSRULE:
		if (val_.rule_)
			delete val_.rule_;
		break;
	case RSSUGG:
		if (val_.sugg_)
			delete val_.sugg_;
		break;
	case RSPHRASE:
		if (val_.phrase_)
			Dlist<Ielt>::DeleteDlistAndData(val_.phrase_);
		break;
	case RSARGS:							// 11/20/98 AM.
	case RSNODES:				// Fix.	// 06/16/99 AM.
	case RSMULTI:				// Fix.	// 06/16/99 AM.
	case RSPATH:							// 10/19/99 AM.
		if (val_.args_)
			Dlist<Iarg>::DeleteDlistAndData(val_.args_);
		break;
	case RSPRES:
		if (val_.pres_)
			Dlist<Ipre>::DeleteDlistAndData(val_.pres_);
		break;
	case RSPOSTS:
		if (val_.posts_)
			Dlist<Ipost>::DeleteDlistAndData(val_.posts_);
		break;
	case RSRULELT:
		if (val_.rulelt_)
			delete val_.rulelt_;
		break;
	case RSPAIRS:
		if (val_.pairs_)
			Dlist<Ipair>::DeleteDlistAndData(val_.pairs_);
		break;
	case RSACTIONS:				// 11/23/98 AM.
		if (val_.actions_)
			Dlist<Iaction>::DeleteDlistAndData(val_.actions_);
		break;
	case RSARG:						// 11/20/98 AM.
		if (val_.arg_)
			delete val_.arg_;
		break;
	case RSPAIR:
		if (val_.pair_)
			delete val_.pair_;
		break;
	case RSACTION:					// 11/23/98 AM.
		if (val_.action_)
			delete val_.action_;
		break;
	case RSPRE:					// 11/29/98 AM.
		if (val_.pre_)
			delete val_.pre_;
		break;
	case RSNAME:
	case RSSTR:
	case RSNUM:						// 11/21/98 AM.
		// Name should be in string table, not deletable here.
	case RSLONG:					// 11/10/99 AM. !
	case RSFLOAT:					// 08/16/01 AM.
		break;
	case RSOSTREAM:				// 12/31/99 AM.
		// DON'T DELETE OSTREAM HERE.  WE'RE JUST CARRYING IT AS A CONVENIENCE.
		// OWNED BY A SEMANTIC VARIABLE.				// 12/31/99 AM.
		break;

	///////// NLP++ SEMANTICS.
	case RSSTMTS:
		if (val_.stmts_)
			delete val_.stmts_;	// 11/13/99 AM.
		break;
	case RSSTMT:
		if (val_.stmt_)
			delete val_.stmt_;
		break;
	case RSEXPR:					// 11/09/99 AM.
		if (val_.expr_)
			delete val_.expr_;
		break;
	case RSVAR:						// 11/09/99 AM.
		if (val_.var_)
			delete val_.var_;
		break;
	case RSOP:						// 11/09/99 AM.
		if (val_.op_)
			delete val_.op_;
		break;
	case RSFUNC:					// 12/19/01 AM.
		if (val_.func_)			// 12/19/01 AM.
			delete val_.func_;	// 12/19/01 AM.
		break;						// 12/19/01 AM.
	case RS_KBCONCEPT:
	case RS_KBPHRASE:
	case RS_KBATTR:
	case RS_KBVAL:
		// Data owned by kb manager.	// 02/22/00 AM.
		break;
	case RSPNODE:
		if (val_.pnode_)
			delete val_.pnode_;			// 10/18/00 AM.
		break;
	case RSNODE:
		// Data owned by parse tree.	// 10/18/00 AM.
		break;
	case RSNULL:
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Unknown RFA semantic type.]") << std::ends;
		errOut(&gerrStr,false);
		}
		break;
	}
}

/********************************************
* FN:		COPY_RFASEM
* CR:		06/27/01 AM.
* SUBJ:	Copy rfasem object.
* NOTE:	Copy constructor conflicts with the void constructor variant.
*			03/11/02 AM. Should handle at least RSARGS.
********************************************/

RFASem *RFASem::copy_rfasem(			// Copy constructor
	Parse *parse,											// 01/09/07 AM.
	bool fintern	// Flag if interning strings.	// 01/09/07 AM.
	)
{
RFASem *dest = new RFASem();
dest->type_ = type_;
switch (type_)
	{
	case RSSTR:
	case RSNUM:
	case RSNAME:
		{
		// LOCAL STRING, TEMPORARY STRING MANAGEMENT.	// 01/08/07 AM.
		// If copy for nonlocal var (SXGN), then intern.// 01/09/07 AM.
		_TCHAR *x = 0;												// 01/09/07 AM.
		if (!fintern)												// 01/09/07 AM.
			x = val_.name_;										// 01/09/07 AM.
		else if (val_.name_ && *(val_.name_))				// 01/16/07 AM.
			parse->internStr(val_.name_, /*UP*/ x);		// 01/09/07 AM.

		dest->val_.name_ = x;
		}
		break;
	case RSLONG:			dest->val_.long_		 = val_.long_;			break;
	case RSFLOAT:			dest->val_.float_		 = val_.float_;		break;
	case RSNODE:			dest->val_.node_		 = val_.node_;			break;
	case RSOSTREAM:		dest->val_.ostr_		 = val_.ostr_;			break;
	case RS_KBCONCEPT:	dest->val_.kbconcept_ = val_.kbconcept_;	break;
	case RS_KBPHRASE:		dest->val_.kbphrase_	 = val_.kbphrase_;	break;
	case RS_KBATTR:		dest->val_.kbattr_	 = val_.kbattr_;		break;
	case RS_KBVAL:			dest->val_.kbval_		 = val_.kbval_;		break;

	// Copy pointer value by default.
	default:					dest->val_.ptr_		 = val_.ptr_;			break;
	}
return dest;
}


/////// 11/09/99 AM.

std::_t_ostream &STDOPERATOR<<(std::_t_ostream &output, RFASem &sem)
{
// Only handling expression types for now.		// 11/09/99 AM.
#ifdef UNICODE
char *nm;																	// 01/28/06 AM.
#endif

switch (sem.type_)
	{
	case RSEXPR:	 output	<< *(sem.val_.expr_);		break;
	case RSVAR:		 output	<< *(sem.val_.var_);			break;
	case RSOP:		 output	<< *(sem.val_.op_);			break;

	case RSSTR:
#ifndef UNICODE
		output	<< *(sem.val_.name_);
#else
		u_to_mbcs((LPCWSTR)*(sem.val_.name_), CP_UTF8, (LPCTSTR*&)nm);// 01/28/06 AM.
		output << nm;																// 01/28/06 AM.
		u_delete((LPCTSTR*&)nm);												// 01/28/06 AM.
#endif
		 break;
	case RSNUM:
#ifndef UNICODE
		output	<< *(sem.val_.name_);
#else
		u_to_mbcs((LPCWSTR)*(sem.val_.name_), CP_UTF8, (LPCTSTR*&)nm);// 01/28/06 AM.
		output << nm;																// 01/28/06 AM.
		u_delete((LPCTSTR*&)nm);												// 01/28/06 AM.
#endif
		break;
	case RSLONG:	 output													// 11/10/99 AM.
									<< _T("((long long)")								// 09/09/01 AM.
									<<  (sem.val_.long_)
									<< _T(")")										// 09/09/01 AM.
									;		break;
	case RSFLOAT:	 output													// 08/16/01 AM.
									<<  _T("((float)")								// 09/09/01 AM.
									<< (sem.val_.float_)
									<< _T(")")										// 09/09/01 AM.
									;		break;
	case RSNAME:
#ifndef UNICODE
		output	<< *(sem.val_.name_);
#else
		u_to_mbcs((LPCWSTR)*(sem.val_.name_), CP_UTF8, (LPCTSTR*&)nm);// 01/28/06 AM.
		output << nm;																// 01/28/06 AM.
		u_delete((LPCTSTR*&)nm);												// 01/28/06 AM.
#endif
		break;
	case RSOSTREAM: output	<< _T("<ostream>");				break;	// 12/31/99 AM.
	case RSSTMT:	 output	<< *(sem.val_.stmt_);		break;	// 12/27/99 AM.
	case RSACTION:	 output	<< *(sem.val_.action_);		break;	// 08/20/00 AM.
	case RSPNODE:	 output	<< *(sem.val_.pnode_);		break;	// 10/18/00 AM.
	case RSNODE:	 output	<< *(sem.val_.node_);		break;	// 10/18/00 AM.
	default:
		output << _T("<UNIMPLEMENTED>");
	}
return output;
}

/********************************************
* FN:		Access functions
********************************************/

enum RFASemtype	 RFASem::getType()		{return type_;			 }

Ifile					*RFASem::getRulesfile()	{return val_.rulesfile_;}
Slist<Irecurse>	*RFASem::getRecurses()	{return val_.recurses_; }
Irecurse				*RFASem::getRecurse()	{return val_.recurse_;}
Slist<Iregion>		*RFASem::getRegions()	{return val_.regions_;}
Iregion				*RFASem::getRegion()		{return val_.region_; }
Dlist<Irule>		*RFASem::getRules()		{return val_.rules_;  }
Irule					*RFASem::getRule()		{return val_.rule_;	 }
Isugg					*RFASem::getSugg()		{return val_.sugg_;	 }
Dlist<Ielt>			*RFASem::getPhrase()		{return val_.phrase_; }
Dlist<Iarg>			*RFASem::getArgs()		{return val_.args_;	 }
Dlist<Ipre>			*RFASem::getPres()		{return val_.pres_;	 }
Dlist<Ipost>		*RFASem::getPosts()		{return val_.posts_;  }
Ielt					*RFASem::getRulelt()		{return val_.rulelt_; }
Dlist<Ipair>		*RFASem::getPairs()		{return val_.pairs_;  }
Dlist<Iaction>		*RFASem::getActions()	{return val_.actions_;}
Iarg					*RFASem::getArg()			{return val_.arg_;    }
Ipair					*RFASem::getPair()		{return val_.pair_;   }
Iaction				*RFASem::getAction()		{return val_.action_; }
Ipre					*RFASem::getPre()			{return val_.pre_;	 }
_TCHAR					*RFASem::getName()		{return val_.name_;   }
_TCHAR					*RFASem::getNum()			{return val_.name_;	 }
long long					 RFASem::getLong()		{return val_.long_;	 }
float					 RFASem::getFloat()		{return val_.float_;	 }
std::_t_ostream				*RFASem::getOstream()	{return val_.ostr_;	 }
Ifunc					*RFASem::getFunc()		{return val_.func_;	 }

//////  NLP++ SEMANTICS
Slist<Istmt>	*RFASem::getStmts()	{return val_.stmts_;}
Istmt				*RFASem::getStmt()	{return val_.stmt_;}
Iexpr				*RFASem::getExpr()	{return val_.expr_;}
Ivar				*RFASem::getVar()		{return val_.var_; }
Iop				*RFASem::getOp()		{return val_.op_;  }

////// KB HANDSHAKE			// 02/22/00 AM.
CONCEPT			*RFASem::getKBconcept()	{return val_.kbconcept_;}
CG				*RFASem::getCG()	{return val_.cg_;}
PHRASE			*RFASem::getKBphrase()	{return val_.kbphrase_;}
ATTR			*RFASem::getKBattr()	{return val_.kbattr_;}
VAL				*RFASem::getKBval()		{return val_.kbval_;}

//////  PARSE TREE OBJECTS
Ipnode	*RFASem::getPnode()	{return val_.pnode_;}
Node<Pn>	*RFASem::getNode()	{return val_.node_;}

/********************************************
* FN:		Modify functions
********************************************/

void RFASem::setType(enum RFASemtype	 x)	{type_        = x; }

void RFASem::setRulesfile(Ifile			  *x)	{val_.rulesfile_ = x;}
void RFASem::setRecurses(Slist<Irecurse> *x)	{val_.recurses_  = x;}
void RFASem::setRecurse(Irecurse			  *x)	{val_.recurse_   = x;}
void RFASem::setRegions(Slist<Iregion>	  *x)	{val_.regions_   = x;}
void RFASem::setRegion(Iregion			  *x)	{val_.region_    = x;}
void RFASem::setRules(Dlist<Irule>		*x)	{val_.rules_  = x; }
void RFASem::setRule(Irule					*x)	{val_.rule_   = x; }
void RFASem::setSugg(Isugg					*x)	{val_.sugg_   = x; }
void RFASem::setPhrase(Dlist<Ielt>		*x)	{val_.phrase_ = x; }
void RFASem::setArgs(Dlist<Iarg>			*x)	{val_.args_   = x; }
void RFASem::setPres(Dlist<Ipre>			*x)	{val_.pres_   = x; }
void RFASem::setPosts(Dlist<Ipost>		*x)	{val_.posts_  = x; }
void RFASem::setRulelt(Ielt				*x)	{val_.rulelt_ = x; }
void RFASem::setPairs(Dlist<Ipair>		*x)	{val_.pairs_  = x; }
void RFASem::setActions(Dlist<Iaction>	*x)	{val_.actions_ = x;}
void RFASem::setArg(Iarg					*x)	{val_.arg_    = x; }
void RFASem::setPair(Ipair					*x)	{val_.pair_   = x; }
void RFASem::setAction(Iaction			*x)	{val_.action_ = x; }
void RFASem::setPre(Ipre					*x)	{val_.pre_	  = x; }
void RFASem::setName(_TCHAR					*x)	{val_.name_   = x; }
void RFASem::setNum(_TCHAR					*x)	{val_.name_   = x; }
void RFASem::setLong(long long					 x)	{val_.long_	  = x; }
void RFASem::setFloat(float				 x)	{val_.float_  = x; }
void RFASem::setOstream(std::_t_ostream			*x)	{val_.ostr_	  = x; }

///////// NLP++ SEMANTICS.
void RFASem::setStmts(Slist<Istmt> *x)	{val_.stmts_	= x;}
void RFASem::setStmt(Istmt			  *x)	{val_.stmt_		= x;}
void RFASem::setExpr(Iexpr			  *x)	{val_.expr_		= x;}
void RFASem::setVar(Ivar			  *x)	{val_.var_		= x;}
void RFASem::setOp(Iop				  *x)	{val_.op_		= x;}
void RFASem::setFunc(Ifunc			  *x)	{val_.func_		= x;}

///////// KB HANDSHAKE	// 02/22/00 AM.
void RFASem::setKBconcept(CONCEPT *x)	{val_.kbconcept_	= x;}
void RFASem::setKBphrase(PHRASE *x)		{val_.kbphrase_	= x;}
void RFASem::setKBattr(ATTR *x)			{val_.kbattr_		= x;}
void RFASem::setKBval(VAL *x)				{val_.kbval_		= x;}

///////// PARSE TREE OBJECTS.
void RFASem::setPnode(Ipnode *x)	{val_.pnode_	= x;}
void RFASem::setNode(Node<Pn> *x) {val_.node_	= x;}

/********************************************
* FN:		FILL
* CR:		11/05/98 AM.
* SUBJ:	Fill Sem object.
* NOTE:	Dummy function for building abstract Sem class.
********************************************/

void RFASem::Fill()
{
}


/********************************************
* FN:		SAME
* CR:		11/09/99 AM.
* SUBJ:	See if two sem objects are identical in form.
*********************************************/

bool RFASem::same(RFASem *sem1, RFASem *sem2)
{
if (!sem1 && !sem2)
	return true;
if (!sem1 || !sem2)
	return false;

if (sem1->type_ != sem2->type_)
	return false;
// Only dealing with NLP++ types for now.			// 11/09/99 AM.
switch (sem1->type_)
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		return !_tcscmp(sem1->val_.name_, sem2->val_.name_);
	case RSLONG:
		return (sem1->val_.long_ == sem2->val_.long_);				// 11/10/99 AM.
	case RSFLOAT:
		return (sem1->val_.float_ == sem2->val_.float_);			// 08/16/01 AM.
	case RSOSTREAM:															// 12/31/99 AM.
		return (sem1->val_.ostr_ == sem2->val_.ostr_);				// 12/31/99 AM.
	case RSSTMTS:	return Istmt::compare(sem1->val_.stmts_, sem2->val_.stmts_);
	case RSSTMT:	return Istmt::compare(sem1->val_.stmt_, sem2->val_.stmt_);
	case RSEXPR:	return Iexpr::same(sem1->val_.expr_, sem2->val_.expr_);
	case RSVAR:		return Ivar::same(sem1->val_.var_, sem2->val_.var_);
	case RSOP:		return Iop::same(sem1->val_.op_, sem2->val_.op_);

	case RSPNODE:	return Ipnode::same(sem1->val_.pnode_,sem2->val_.pnode_);
	case RSNODE:	return (sem1->val_.node_ == sem2->val_.node_);

	case RS_KBCONCEPT:
	case RS_KBPHRASE:
	case RS_KBATTR:
	case RS_KBVAL:
		// Unimplemented.

		return false;						// 02/22/00 AM.
	default:			return false;
	}

return true;
}


/********************************************
* FN:		EVAL
* CR:		11/14/99 AM.
* SUBJ:	Evaluate semantic tree.
* NOTE:	Traverse statement and expression tree, executing operations.
*********************************************/

bool RFASem::eval(Nlppp *nlppp, /*UP*/ RFASem* &val)
{
val = 0;

switch (type_)
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		val = new RFASem(val_.name_, type_);	// Copy the current sem object.
		return true;
	case RSLONG:
		val = new RFASem(val_.long_);	// Copy the current sem object.
		return true;
	case RSFLOAT:																// 08/16/01 AM.
		val = new RFASem(val_.float_); // Copy the current sem.	// 08/16/01 AM.
		return true;															// 08/16/01 AM.
	case RSOSTREAM:															// 12/31/99 AM.
		val = new RFASem(val_.ostr_);										// 12/31/99 AM.
		return true;															// 12/31/99 AM.

	///////// NLP++ SEMANTICS.
	case RSSTMTS:
		if (val_.stmts_)
			return Istmt::eval(val_.stmts_, nlppp, /*UP*/ val);
		break;
	case RSSTMT:
		if (val_.stmt_)
			return val_.stmt_->eval(nlppp, /*UP*/ val);
		break;
	case RSEXPR:
		if (val_.expr_)
			return val_.expr_->eval(nlppp, /*UP*/ val);
		break;
	case RSVAR:
		if (val_.var_)
			return val_.var_->eval(nlppp, /*UP*/ val);
		break;
	case RSOP:
		if (val_.op_)
			return val_.op_->eval(nlppp, /*UP*/ val);
		break;
	case RSACTION:															// 01/13/00 AM.
		if (val_.action_)													// 01/13/00 AM.
			return val_.action_->eval(nlppp, /*UP*/ val);		// 01/13/00 AM.
	case RSNULL:

	case RS_KBCONCEPT:													// 02/22/00 AM.
	case RS_KBPHRASE:
	case RS_KBATTR:
	case RS_KBVAL:
		val = new RFASem(val_.name_, type_);	// Copy the current sem object.
		return true;

	///////// PARSE TREE OBJECTS.
	case RSPNODE:
		if (val_.pnode_)
			return val_.pnode_->eval(nlppp, /*UP*/ val);
		break;
	case RSNODE:
		val = new RFASem(val_.node_);	// Copy pointer to node.
		return true;


	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[RFA semantic type not used in NLP++.]") << std::ends;
		return errOut(&gerrStr,false);
		}
	}
return true;
}


/********************************************
* FN:		GENEVAL
* CR:		05/06/00 AM.
* SUBJ:	Generate code to eval semantic tree.
*********************************************/

bool RFASem::genEval(Gen *gen,
	bool top						// If at top or stmt level.			// 06/06/00 AM.
	)
{
_TCHAR *indent = gen->indent_;
//std::_t_ofstream *fcode = gen->fcode_;
std::_t_ofstream *fcode = gen->passc_;	// 04/03/09 AM.
_TCHAR buff[MAXSTR+1];
bool ok = false;
long long num=0;																		// 03/13/02 AM.
switch (type_)
	{
	case RSNAME:
	case RSSTR:
		*fcode << _T("_T(\"")
				 << c_str(val_.name_, buff, MAXSTR)
				 << _T("\")");
		return true;
	case RSNUM:																	// 03/13/02 AM.
		// Problem if generating these as strings.					// 03/13/02 AM.
		if (!str_to_long(val_.name_,num))								// 03/13/02 AM.
			return false;														// 03/13/02 AM.
		*fcode << _T("(long long)") << num;											// 03/13/02 AM.
		return true;															// 03/13/02 AM.
	case RSLONG:
		if (!val_.long_)				// If ZERO.							// 06/05/00 AM.
			*fcode << _T("(long long)0");		// Make unambiguous.				// 06/05/00 AM.
		else
			*fcode << _T("(long long)")		// Make unambiguous.				// 09/03/01 AM.
					 << val_.long_;
		return true;
	case RSFLOAT:																// 08/16/01 AM.
		*fcode
				<< _T("(float)")													// 09/09/01 AM.
				<< val_.float_;												// 08/16/01 AM.
		return true;															// 08/16/01 AM.
	case RSOSTREAM:
		*fcode << _T("ERROR(_T(\"Can't generate OSTREAM\"))");				// 05/04/01 AM.
		return true;

	///////// NLP++ SEMANTICS.
	case RSSTMTS:
		if (val_.stmts_)
			return Istmt::genEval(val_.stmts_, gen);
		break;
	case RSSTMT:
		if (val_.stmt_)
			return val_.stmt_->genEval(gen);	// Derived class of istmt.
		break;
	case RSEXPR:
		if (top) *fcode << gen->indent_ << _T("Arun::stmt(");
		if (val_.expr_)
			ok = val_.expr_->genEval(gen,false);
		if (top) *fcode << _T(")");
		return ok;
		break;
	case RSVAR:
		if (top) *fcode << gen->indent_ << _T("Arun::stmt(");
		if (val_.var_)
			ok = val_.var_->genEval(gen);
		if (top) *fcode << _T(")");
		return ok;
		break;
	case RSOP:
		//if (val_.op_)
		//	return val_.op_->genEval(gen);
		*fcode << _T("\n// CAN'T EVAL OP.") << std::endl;
		return false;
		break;
	case RSFUNC:																// 12/19/01 AM.
		if (val_.func_)														// 12/19/01 AM.
			return val_.func_->gen(fcode, gen);							// 12/19/01 AM.
		break;																	// 12/19/01 AM.
	case RSACTION:
		if (val_.action_)
			return val_.action_->genEval(fcode, gen, top);
	case RSNULL:

	case RS_KBCONCEPT:
	case RS_KBPHRASE:
	case RS_KBATTR:
	case RS_KBVAL:
		*fcode << _T("_T(\"") << val_.name_ << _T("\")");
		return true;

	///////// PARSE TREE OBJECTS.
	case RSPNODE:
		if (val_.pnode_)
			return val_.pnode_->genEval(gen);
		break;
	case RSNODE:
		*fcode << _T("<NODE>");
		return true;

	default:
		{
		*fcode << std::endl << _T("// [RFA semantic type not used in NLP++.]") << std::endl;
		std::_t_strstream gerrStr;
		gerrStr << _T("[RFA semantic type not used in NLP++.]") << std::ends;
		return errOut(&gerrStr,false);
		}
	}
return true;
}



/********************************************
* FN:		TRUTH
* CR:		05/26/00 AM.
* SUBJ:	See if sem evaluates to true.
* NOTE:	Support for compiled analyzer.
*********************************************/

bool RFASem::truth()
{
switch (type_)
	{
	case RSNAME:
	case RSSTR:
	case RSNUM:
		return ((val_.name_ && *(val_.name_)) ? true : false);
	case RSLONG:
		return (val_.long_ ? true : false);
	case RSFLOAT:																// 08/16/01 AM.
		return (val_.float_ ? true : false);							// 08/16/01 AM.
	case RSOSTREAM:
		return (val_.ostr_ ? true : false);
	case RS_KBCONCEPT:														// 02/22/00 AM.
		return (val_.kbconcept_ ? true : false);			// FIX.	// 05/16/03 AM.
	case RS_KBPHRASE:
		return (val_.kbphrase_ ? true : false);			// FIX.	// 05/16/03 AM.
	case RS_KBATTR:
		return (val_.kbattr_ ? true : false);				// FIX.	// 05/16/03 AM.
	case RS_KBVAL:
		return (val_.kbval_ ? true : false);				// FIX.	// 05/16/03 AM.
//		return (val_.name_ && (*val_.name_) ? true : false);//BAD// 05/16/03 AM.
	case RSNODE:																// 04/29/01 AM.
		return (val_.node_ ? true : false);								// 04/29/01 AM.
	case RSARGS:	// "Multi-value" or array type.					// 03/22/02 AM.
		if (val_.args_ && val_.args_->getFirst())						// 03/22/02 AM.
			return true;														// 03/22/02 AM.
		return false;															// 03/22/02 AM.
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[truth: Bad runtime semantic type.]") << std::ends;
		return errOut(&gerrStr,false);
		}
	}
}


/********************************************
* FN:		OUT
* CR:		05/26/00 AM.
* SUBJ:	Output a sem object.
* NOTE:	To support compiled runtime analyzers.
*********************************************/

std::_t_ostream *RFASem::out(std::_t_ostream *ostr)
{
switch(type_)
	{
	case RSLONG:
		*ostr << val_.long_;
		break;
	case RSFLOAT:																// 08/16/01 AM.
		*ostr << val_.float_;												// 08/16/01 AM.
		break;																	// 08/16/01 AM.
	case RSNUM:
	case RSSTR:
	case RSNAME:
		*ostr << val_.name_;
		break;
	case RSARGS:																// 02/02/01 AM.
		*ostr << *(val_.args_);									// FIX.	// 03/11/02 AM.
		break;
	default:
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Error: out: Bad sem object type.]") << std::ends;
		errOut(&gerrStr,false);
		return 0;
		}
	}
return ostr;
}

/********************************************
* FN:		SEM_SET_LONG
* CR:		07/05/06 AM.
* SUBJ:	Loosely set a sem with long long value.
* NOTE:	Convenience function.
* ASS:	Assume if name type, will take numeric string value.
*********************************************/

long long RFASem::sem_set_long(long long num, bool &ok)
{
ok = true;
switch (type_)
	{
	case RSLONG:
		val_.long_ = num;
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		// Change type!  Assume string will take care of itself.
		type_ = RSLONG;
		val_.long_ = num;
		break;
	default:
		ok = false;
		break;
	}
return num;
}

/********************************************
* FN:		SEM_TO_LONG
* CR:		04/28/01 AM.
* SUBJ:	Loosely convert a sem to long long.
* NOTE:	Convenience function.
*********************************************/

long long RFASem::sem_to_long(bool &ok)
{
ok = true;
long long num = 0;
switch (type_)
	{
	case RSLONG:
		num = val_.long_;
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		if (!str_to_long(val_.name_, num))
			num = 0;		// Recover.
		break;
	default:
		ok = false;
		break;
	}
return num;
}


/********************************************
* FN:		SEM_TO_NODE
* CR:		04/28/01 AM.
* SUBJ:	Convert a sem to node.
* NOTE:	Convenience function.
*********************************************/

Node<Pn> *RFASem::sem_to_node()
{
switch (type_)
	{
	case RSNODE:
		return val_.node_;
		break;
	default:
		return 0;
		break;
	}
return 0;
}


/********************************************
* FN:		SEM_TO_STR
* CR:		04/28/01 AM.
* SUBJ:	Loosely convert a sem to str.
* NOTE:	Convenience function.
*********************************************/

_TCHAR *RFASem::sem_to_str()
{
_TCHAR *st = 0;
switch (type_)
	{
	case RSSTR:
	case RSNAME:
	case RSNUM:
		return val_.name_;
	case RSLONG:																// 08/08/02 AM.
		if (val_.long_ == 0)													// 08/08/02 AM.
			return 0;															// 08/08/02 AM.
		break;
	case RSARGS:																// 08/08/02 AM.
		{
		std::_t_strstream gerrStr;
		gerrStr << _T("[Error: Can't get string from multivalued var.]") << std::ends;
		errOut(&gerrStr,false);
		return 0;
		}
	default:
		break;
	}
std::_t_strstream gerrStr;
gerrStr << _T("[Error: Bad sem type in sem_to_str]") << std::ends;
errOut(&gerrStr,false);
return 0;
}



/********************************************
* FN:		SEM_TO_FLOAT
* CR:		08/18/01 AM.
* SUBJ:	Loosely convert a sem to float.
* NOTE:	Convenience function.
*********************************************/

float RFASem::sem_to_float(bool &ok)
{
ok = true;
long long num = 0;
float fnum=0.0;
switch (type_)
	{
	case RSFLOAT:					// FIX	// 08/18/08 AM.
		fnum = val_.float_;		// FIX	// 08/18/08 AM.
		break;
	case RSLONG:
		fnum = (float) val_.long_;
		break;
	case RSSTR:
	case RSNUM:
	case RSNAME:
		if (!str_to_long(val_.name_, num))
			num = 0;		// Recover.
		fnum = (float) num;
		break;
	default:
		ok = false;
		break;
	}
return fnum;
}
