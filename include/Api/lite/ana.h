/*******************************************************************************
Copyright (c) 2001-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ANA.H
* FILE:	include/API/lite/ana.h
* CR:		10/08/98 AM.
* SUBJ:	Object representing the analyzer and its specifications.
* NOTE:	The Ana class defines the analyzer.  The Parse class represents the
*			analysis of a single text.
*			A parse could conceivably use multiple analyzers.
*
*******************************************************************************/

#ifndef ANA_
#define ANA_

//#include "htab.h"
//#include "stab.h"
//#include "lite/dlist.h"
//#include "seqn.h"
//#include "Eana.h"				// 10/13/99 AM.

#define DEFAULT_SEQFILE _T("analyzer.seq")									// 11/04/99 AM.
#define DEFAULT_RUNDIR _T("run")												// 05/10/00 AM.

// Forward references.
class CG;																		// 02/15/00 AM.
class Gen;																		// 05/10/00 AM.
class Eana;																		// 02/26/01 AM.
class Seqn;																		// 02/26/01 AM.
class Htab;																		// 02/26/01 AM.
class Stab;																		// 02/26/01 AM.
class Ifile;																	// 02/26/01 AM.
class NLP;																		// 12/20/01 AM.

/********************************************
* CLASS:	ANA
* CR:		10/08/98 AM.
* SUBJ:	Class that defines the analyzer and its specifications.
********************************************/

class LITE_API Ana
{
public:
	Ana(_TCHAR *appdir = _T(""));		// Default constructor.
	~Ana();
	Dlist<Seqn>	*getSeqlist();
	Delt<Seqn>	*getSeq();		// Get the list of sequence elts.
	_TCHAR			*getSeqfile();
	_TCHAR			*getSeqbuf();
	long			 getSeqbuflen();
	Htab			*getHtab();
	bool			 Verbose();
	_TCHAR			*getSpecdir();												// 12/03/98 AM.
	_TCHAR			*getComment();												// 01/13/99 AM.
	_TCHAR			*getDatadir();												// 12/08/99 AM.
	_TCHAR			*getAppdir();												// 05/10/00 AM.
	CG				*getCG();													// 02/15/00 AM.
	Gen			*getGen();													// 05/10/00 AM.
	long			 getNpasses();												// 06/13/00 AM.
	bool			 getDirty();												// 12/20/01 AM.
	void			*getHtfunc();												// 12/20/01 AM.
	NLP			*getNLP();													// 12/21/01 AM.

	void setSeqlist(Dlist<Seqn> *);
	void setSeqfile(_TCHAR *);
	void setSeqbuf(_TCHAR *);
	void setSeqbuflen(long);
	void setHtab(Htab *);
	void setVerbose(bool);
	void setSpecdir(_TCHAR *);												// 12/03/98 AM.
	void setComment(_TCHAR *);												// 01/13/99 AM.
	void setDatadir(_TCHAR *);												// 12/08/99 AM.
	void setAppdir(_TCHAR *);													// 05/10/00 AM.
	void setCG(CG *);															// 02/15/00 AM.
	void setGen(Gen *);														// 05/10/00 AM.
	void setNpasses(long);													// 06/13/00 AM.
	void setDirty(bool);														// 12/20/01 AM.
	void setHtfunc(void*);													// 12/20/01 AM.
	void setNLP(NLP*);														// 12/21/01 AM.

	bool readSeqfile();
	void writeSeqfile();														// 01/15/99 AM.
	void makeSeq();
	void addSeq(Delt<Seqn> *);
	bool internSeq(														// 01/13/99 AM.
		Ana &rfa,
		Eana *erfa,																// 10/13/99 AM.
//		NLP *nlp																	// 08/27/02 AM.
		void*																		// 08/27/02 AM.
		);
	bool internPass(Seqn *pass, Ana &rfa,						// 01/13/99 AM.
					Eana *erfa);												// 10/13/99 AM.
	void uninternPass(Seqn *pass);									// 01/15/99 AM.
	void parseSeq(bool);		// Parse sequence file.
	bool kbgetSeq();															// 11/11/00 AM.
	bool parseRulesfile(_TCHAR *, Ana &rfa,
					Eana *erfa,													// 10/13/99 AM.
					_TCHAR *algo,													// 05/31/00 AM.
					long passnum,												// 08/23/02 AM.
					/*UP*/ Ifile* &rulesfile);
	//void addPass(char *, char *, Ana &rfa); // OBSOLETE.		// 10/13/99 AM.
	void prettySeq();

	void genAna(_TCHAR *dir = _T("."));											// 11/04/99 AM.
	void genSeqfile(std::_t_ostream &ofile);								// 11/04/99 AM.

	bool genTok(Seqn *pass, Gen *gen);									// 05/10/00 AM.
	bool genCMLTok(Seqn *pass, Gen *gen);								// 08/18/08 AM.
	bool genDICTTok(Seqn *pass, Gen *gen);								// 07/29/11 AM.
	bool genDICTTokz(Seqn *pass, Gen *gen);							// 08/16/11 AM.
	bool genLines(Seqn *pass, Gen *gen);								// 05/10/00 AM.

private:
	Dlist<Seqn> *seqlist_;		// The sequence of passes. (LIST).
	_TCHAR *seqbuf_;					// Buffer for contents of sequence file.
	long seqbuflen_;				// Length of sequence file.
	bool verbose_;					// If ana operations running verbose.
	Htab *htab_;					// Hashed symbol table.				// 11/19/98 AM.
	_TCHAR specdir_[FNAME_SIZE];	// Definition directory.			// 12/03/98 AM.
	_TCHAR seqfile_[FNAME_SIZE]; // Sequence file name.				// 01/20/99 AM.
	_TCHAR comment_[MAXSTR];		// Header comment for seq file.	// 01/13/99 AM.
	_TCHAR datadir_[FNAME_SIZE];	// Data directory.					// 12/08/99 AM.
	_TCHAR appdir_[FNAME_SIZE];	// Top-level directory for ana.	// 05/10/00 AM.
	_TCHAR anasdir_[FNAME_SIZE];
	Gen *gen_;						// For code gen and compilation.	// 05/10/00 AM.
	CG *cg_;							// Pointer to KB.						// 02/15/00 AM.
	long npasses_;					// Number of passes in analyzer.	// 06/13/00 AM.

	// Flag when analyzer definition has been modified/edited
	// and is pending an update.  To be used in managing a
	// global hash table for user-defined NLP++ functions, etc.	// 12/20/01 AM.
	bool dirty_;																// 12/20/01 AM.

	// Hash table for user-defined NLP++ functions.					// 12/20/01 AM.
	// NOTE: DISTINCT from hash table for builtin NLP++ funcs.	// 12/20/01 AM.
	void *htfunc_;																// 12/20/01 AM.

	// Convenience reference to NLP environment object.			// 12/21/01 AM.
	NLP *nlp_;																	// 12/21/01 AM.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream*ofstr=0);			// Pretty-print the count.
private:
	static int count_;					// Count nodes currently allocated.
#endif
};


#endif
