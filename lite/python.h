/*******************************************************************************
* NAME:	PYTHON.H
* FILE:	lite\python.h
* SUBJ:	Analyzer pass that runs a Python script (gap-filler / enricher hook).
* NOTE:	Two flavors, both implemented by this one Algo:
*			  "python"     - normal pass; runs at its position in the sequence
*			                 (after tokenization, tree available).
*			  "pythonpre"  - pre-tokenization pass; runs before the tokenizer on
*			                 the raw input (e.g. to update the dictionary before
*			                 dictionary-driven tokenization/segmentation).
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
	Pyalgo(bool pre = false);				// Default constructor.
	Pyalgo(const Pyalgo &orig);			// Copy constructor.

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	virtual bool Execute(Parse *, Seqn *);

private:
	bool pre_;		// true = pre-tokenization flavor.
};

#endif
