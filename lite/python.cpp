/*******************************************************************************
* NAME:	PYTHON.CPP
* FILE:	lite\python.cpp
* SUBJ:	Analyzer pass that runs a Python script (gap-filler / enricher hook).
* NOTE:	See python.h.  The pass shells out to:
*			  python "<appdir>/spec/<script>.py" "<appdir>" "<inputfile>" pre|post
*			Use "pythonpre" as the first pass (before the tokenizer) to update the
*			dictionary/KB on raw text; use "python" anywhere after tokenization.
*******************************************************************************/

#include "StdAfx.h"
#include "machine.h"
#include "u_out.h"
#include "lite/lite.h"
#include "lite/global.h"
#include "lite/inline.h"
#include "io.h"
#include "dlist.h"
#include "node.h"
#include "tree.h"
#include "pn.h"
#include "Eana.h"
#include "parse.h"
#include "lite/nlppp.h"
#include "gen.h"
#include "seqn.h"
#include "python.h"

// For pretty printing the algorithm name.
static _TCHAR algo_name[] = _T("python");

/********************************************
* FN:		Special Functions for Pyalgo class.
********************************************/

Pyalgo::Pyalgo(bool pre)			// Default constructor.
	: Algo(algo_name)
{
pre_ = pre;
}

Pyalgo::Pyalgo(const Pyalgo &orig)	// Copy constructor.
{
name = orig.name;
debug_ = orig.debug_;
pre_ = orig.pre_;
}

/********************************************
* FN:		DUP
********************************************/

Algo &Pyalgo::dup()
{
Pyalgo *ptr;
ptr = new Pyalgo(*this);
return (Algo &) *ptr;
}

/********************************************
* FN:		SETUP
* SUBJ:	Set up Algo as an analyzer pass.
* NOTE:	The script base name comes from the sequence pass (getRulesfilename)
*			and is read in Execute, so nothing to do here.
********************************************/

void Pyalgo::setup(_TCHAR * /*s_data*/)
{
}

/********************************************
* FN:		EXECUTE
* SUBJ:	Run the Python script for this pass.
********************************************/

bool Pyalgo::Execute(Parse *parse, Seqn *seqn)
{
_TCHAR *script = seqn ? seqn->getRulesfilename() : 0;
if (!script || !*script)
	{
	if (parse->Verbose())
		*gout << _T("[python pass: missing script name; skipping.]") << std::endl;
	return true;
	}

_TCHAR *appdir = parse->getAppdir();
_TCHAR *input  = parse->getInput();

_TCHAR cmd[8192];
_stprintf(cmd, _T("python \"%s/spec/%s.py\" \"%s\" \"%s\" %s"),
			(appdir ? appdir : _T(".")),
			script,
			(appdir ? appdir : _T(".")),
			(input ? input : _T("")),
			(pre_ ? _T("pre") : _T("post")));

if (parse->Verbose())
	*gout << _T("[python pass: ") << cmd << _T("]") << std::endl;

int rc = _tsystem(cmd);
if (rc < 0)
	{
	*gout << _T("[python pass FAILED (rc<0): ") << cmd << _T("]") << std::endl;
	// Don't abort the whole run if a gap-filler/enricher pass fails.
	return true;
	}

return true;
}
