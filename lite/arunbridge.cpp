/*******************************************************************************
Copyright (c) 2026 by VisualText.
All rights reserved.
********************************************************************************
*
* NAME:	ARUNBRIDGE.CPP
* FILE:	lite/arunbridge.cpp
* SUBJ:	Compiled-analyzer bridge to builtins that exist only in the
*			interpreter.
* NOTE:	The code generator writes a call to a builtin as Arun::<name>(...),
*			so a builtin added to Fn:: without a compiled Arun:: twin made any
*			compiled analyzer that called it fail to build. For those builtins
*			the generator now writes Arun::fnbridge(nlppp, "name", {args}),
*			which runs the interpreter's own implementation: compiled and
*			interpreted behave the same, and nothing is reimplemented.
*
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/global.h"
#include "inline.h"
#include "dlist.h"
#include "node.h"
#include "lite/iarg.h"
#include "str.h"
#include "io.h"
#include "tree.h"
#include "lite/nlppp.h"
#include "gen.h"
#include "parse.h"
#include "iaction.h"
#include "rfasem.h"
#include "fn.h"
#include "lite/Arun.h"


/********************************************
* FN:		FNBRIDGE
* SUBJ:	Call the interpreter's builtin NAME with already-evaluated ARGS.
* RET:	The builtin's value (never 0: an error yields 0 as a number).
* NOTE:	Mirrors Iaction::eval: the arguments become IASEM args of a
*			temporary action, Fn::fnCall runs the builtin, and the argument
*			list is freed with Iarg::delete_top, as Iaction::eval frees its
*			evaluated arguments. The bridge owns ARGS, as an Arun:: builtin
*			owns the RFASem* args the generated code hands it.
********************************************/

RFASem *Arun::fnbridge(
	Nlppp *nlppp,
	_TCHAR *name,
	std::initializer_list<RFASem *> args
	)
{
Dlist<Iarg> *iargs = new Dlist<Iarg>();
for (RFASem *sem : args)
	iargs->rpush(new Iarg(sem ? sem : new RFASem(0LL)));

Iaction action(name, iargs, 0);
bool badname = false;
RFASem *val = 0;
bool ok = Fn::fnCall(&action, nlppp, /*UP*/ badname, val);
action.setArgs(0);					// The action doesn't own the list.
Iarg::delete_top(iargs);

if (!ok && val)
	{
	delete val;
	val = 0;
	}
if (!ok)
	{
	std::_t_strstream gerrStr;
	if (badname)
		gerrStr << _T("[Fncall: Error: Unknown fn/action name=") << name << _T("]") << std::ends;
	else
		gerrStr << _T("[Fncall: Error executing fn/action=") << name << _T("]") << std::ends;
	nlppp->parse_->errOut(&gerrStr,false);
	}
return val ? val : new RFASem(0LL);
}
