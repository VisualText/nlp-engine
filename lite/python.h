/*******************************************************************************
* NAME:	PYTHON.H
* FILE:	lite\python.h
* SUBJ:	Analyzer pass that runs a Python script (gap-filler / enricher hook).
* NOTE:	A single "python" pass type that may be placed anywhere in the
*			sequence, including before the tokenizer (which is normally first).
*			When it runs before tokenization it sees the raw input (e.g. to
*			update the dictionary before dictionary-driven tokenization); when
*			it runs after, the parse tree is available.  The phase is detected
*			automatically (a parse tree exists only after tokenization).
*			The pass's second sequence column is the script base name; the engine
*			runs  python "<appdir>/spec/<name>.py" "<appdir>" "<inputfile>" pre|post
*******************************************************************************/

#ifndef PYTHON_H_
#define PYTHON_H_

#include "lite/algo.h"

class Parse;	// Forward reference.
class Seqn;		// Forward reference.

/********************************************
* CLASS:	PYALGO
* SUBJ:	Run a Python script as an analyzer pass.
********************************************/

class Pyalgo : public Algo
{
public:
	Pyalgo();									// Default constructor.
	Pyalgo(const Pyalgo &orig);			// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

	// Run the Python script named by its base name (spec/<script>.py). Shared by
	// the interpreted pass (Execute reads the name from the Seqn) and the compiled
	// pass (the generated python<N> function passes the name as a literal, since a
	// compiled analyzer has no Seqn at runtime).					// 07/14/26.
	static bool run(Parse *, const _TCHAR *script);
};

#endif
