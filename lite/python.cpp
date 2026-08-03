/*******************************************************************************
* NAME:	PYTHON.CPP
* FILE:	lite\python.cpp
* SUBJ:	Analyzer pass that runs a Python script (gap-filler / enricher hook).
* NOTE:	See python.h.  The pass shells out to:
*			  python "<appdir>/spec/<script>.py" "<appdir>" "<inputfile>" pre|post
*			A "python" pass may sit anywhere in the sequence, including before the
*			tokenizer (e.g. to update the dictionary/KB on raw text).  The pre|post
*			argument is detected automatically: a parse tree exists only after
*			tokenization, so no tree => "pre", tree present => "post".
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

Pyalgo::Pyalgo()					// Default constructor.
	: Algo(algo_name)
{
}

Pyalgo::Pyalgo(const Pyalgo &orig)	// Copy constructor.
{
name = orig.name;
debug_ = orig.debug_;
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
return run(parse, script);
}

/********************************************
* FN:		PYTHON_CMD_UTIL
* SUBJ:	Pick the interpreter to run Python passes with.
* RET:	Command to invoke, or 0 if no interpreter could be found.
* NOTE:	The pass used to hardcode "python". macOS removed /usr/bin/python in
*			12.3, and most Linux distributions ship only python3 unless
*			python-is-python3 is installed, so on a large share of machines that
*			command does not exist and every Python pass quietly did nothing.
*			Probed once and cached; the answer cannot change mid-run.
*			NLPPP_PYTHON overrides the search, for a venv or an odd install.
********************************************/

#define PYCMD_MAX 512

static const _TCHAR *python_cmd_util()
{
static _TCHAR chosen[PYCMD_MAX];
static bool looked = false;

if (looked)
	return (chosen[0] ? chosen : 0);
looked = true;
chosen[0] = '\0';

// An explicit setting wins without probing: the user knows their machine.
_TCHAR *env = _tgetenv(_T("NLPPP_PYTHON"));
if (env && *env)
	{
	_tcsncpy(chosen, env, PYCMD_MAX-1);
	chosen[PYCMD_MAX-1] = '\0';
	return chosen;
	}

#ifdef LINUX
// Modern macOS and most Linux distributions provide only python3.
static const _TCHAR *cands[] = { _T("python3"), _T("python"), 0 };
static const _TCHAR *quiet = _T(" >/dev/null 2>&1");
#else
// Windows installers put "python" on PATH; "py -3" is the launcher.
static const _TCHAR *cands[] = { _T("python"), _T("python3"), _T("py -3"), 0 };
static const _TCHAR *quiet = _T(" >nul 2>&1");
#endif

_TCHAR probe[PYCMD_MAX];
for (int ii = 0; cands[ii]; ++ii)
	{
	// "-c pass" is the cheapest thing that proves the interpreter runs.
	_stprintf(probe, _T("%s -c \"pass\"%s"), cands[ii], quiet);
	if (_tsystem(probe) == 0)
		{
		_tcscpy(chosen, cands[ii]);
		return chosen;
		}
	}

return 0;
}


/********************************************
* FN:		RUN
* SUBJ:	Run the Python script for this pass by base name.
* NOTE:	Shared by the interpreted (Execute) and compiled (generated python<N>)
*			paths, so both build the identical command line.
********************************************/

bool Pyalgo::run(Parse *parse, const _TCHAR *script)
{
if (!script || !*script)
	{
	if (parse->Verbose())
		*gout << _T("[python pass: missing script name; skipping.]") << std::endl;
	return true;
	}

_TCHAR *appdir = parse->getAppdir();
_TCHAR *input  = parse->getInput();

// Detect phase: a parse tree exists only after tokenization.
bool pre = (parse->getTree() == 0);

const _TCHAR *py = python_cmd_util();
if (!py)
	{
	// A missing interpreter is a setup problem, not a script bug, so say so
	// distinctly.  This used to be invisible: the pass ran "python", the
	// shell reported command-not-found, and nothing was printed.
	*gout << _T("[python pass FAILED: no Python interpreter found (tried python3, python). ")
			<< _T("Set NLPPP_PYTHON to the interpreter to use. script=")
			<< script << _T("]") << std::endl;
	_stprintf(Errbuf,
		_T("[python pass '%s': no Python interpreter found. Set NLPPP_PYTHON.]"),
		script);
	return parse->errOut(true);
	}

_TCHAR cmd[8192];
_stprintf(cmd, _T("%s \"%s/spec/%s.py\" \"%s\" \"%s\" %s"),
			py,
			(appdir ? appdir : _T(".")),
			script,
			(appdir ? appdir : _T(".")),
			(input ? input : _T("")),
			(pre ? _T("pre") : _T("post")));

if (parse->Verbose())
	*gout << _T("[python pass: ") << cmd << _T("]") << std::endl;

int rc = _tsystem(cmd);
if (rc != 0)
	{
	// Report ANY non-zero status, not just rc<0.  system() returns the
	// shell's exit status -- 127 for command-not-found on POSIX -- and only
	// returns -1 when the shell itself cannot be spawned, so the old rc<0
	// test never fired and a failing pass was completely silent.  For
	// json2kbb that meant no .kbb was built and the analyzer ran against an
	// empty KB with no warning.
	*gout << _T("[python pass FAILED (exit ") << rc << _T("): ")
			<< cmd << _T("]") << std::endl;
	_stprintf(Errbuf, _T("[python pass '%s' failed, exit status %d.]"), script, rc);
	// Still don't abort: a gap-filler/enricher failing should not kill the
	// analysis.  It just must not be silent.
	return parse->errOut(true);
	}

return true;
}
