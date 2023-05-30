/*******************************************************************************
Copyright (c) 1998-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	PARSE.H
* FILE:	include/API/lite/parse.h
* CR:		10/08/98 AM.
* SUBJ:	The Parse class represents all the data for parsing a single text.
* NOTE:	The Ana class represents the definition of the analyzer machinery.
*
*******************************************************************************/

#ifndef PARSE_H_
#define PARSE_H_
#ifndef LINUX
#include <windows.h>		// defines HINSTANCE.						// 02/14/01 AM.
#else
#include <iostream>	// 09/27/19 AM.
#include <strstream>	// 09/27/19 AM.
#endif
#include <time.h>

typedef void TREE;	// 07/07/03 AM.
typedef void LTREE;	// 07/07/03 AM.

//#include "global.h"				// 05/19/99 AM.
//#include "ana.h"
//#include "lite/dlist.h"			// 07/07/03 AM.
#include "lite/slist.h"				// 02/14/01 AM.
//#include "pn.h"						// 02/14/01 AM.
//#include "ln.h"						// 02/14/01 AM.
//#include "lite/tree.h"
#include "lite/Arun.h"				// 05/17/00 AM.
//#include "mode.h"					// 04/04/00 AM.

//#include "consh/libconsh.h"		// 02/14/01 AM.
//#include "consh/cg.h"				// 02/14/01 AM.

// Forward reference.
class Ana;
class NLP;						// 06/25/03 AM.
class Eana;						// 10/13/99 AM.
class Ipair;					// 12/07/98 AM.
class Iaction;					// 06/15/99 AM.
class Mode;						// 04/04/00 AM.
class Pat;			// OPT.	// 05/16/00 AM.
class Rec;			// OPT.	// 06/01/00 AM.
class Nlppp;					// 05/17/00 AM.
class Arun;						// 05/17/00 AM.
class Algo;						// 05/20/00 AM.
class Pn;						// 02/14/01 AM.
class Ln;						// 02/14/01 AM.
class Sym;						// 02/14/01 AM.
class Iarg;						// 02/14/01 AM.
class CG;
class Ielt;
class Ipre;
class Irule;
class Seqn;						// 07/23/01 AM.
class Ifunc;					// 08/22/02 AM.
class Fn;						// 08/24/02 AM.
class Iexpr;					// 08/24/02 AM.
class Istmt;					// 08/24/02 AM.
class Iwhilestmt;				// 08/24/02 AM.
class PostRFA;					// 08/24/02 AM.
class Ivar;						// 03/13/03 AM.
class Tok;						// 06/25/03 AM.
class Htab;						// 06/25/03 AM.
class CMLTok;					// 08/17/08 AM.
class DICTTok;					// 07/29/11 AM.

/********************************************
* CLASS:	PARSE
* CR:		10/08/98 AM.
* SUBJ:	The Parse class definition, representing the analysis of a single text.
* NOTE:	The text buffer may be managed by a class eventually.
*			Parse may set which analyzer(s) it is using.
********************************************/

// 05/18/99 AM. Exporting for LIBRUG.
class LITE_API Parse
{
	friend class Pat;
	friend class Rec;
	friend class Arun;
	friend class Ifunc;
	friend class Iaction;
	friend class Fn;
	friend class Ielt;
	friend class Iexpr;
	friend class Istmt;
	friend class Iwhilestmt;
	friend class PostRFA;
	friend class Ivar;
	friend class Nlppp;						// 06/25/03 AM.
	friend class Tok;							// 06/25/03 AM.
	friend class NLP;							// 06/25/03 AM.
	friend class CMLTok;						// 08/17/08 AM.
	friend class DICTTok;					// 07/29/11 AM.
public:
	Parse();								// Constructor.
	~Parse();							// Destructor.

	Ana  *getAna();
	NLP  *getNLP();					// 06/25/03 AM.
	Htab *getHtab();					// 06/25/03 AM.
	Eana *getEana();					// 10/13/99 AM.
	_TCHAR *getInput();
	_TCHAR *getOutput();
	_TCHAR *getText();
	long  getLength();				// Length of input text.
	long getUlength();	// [UNICODE]	// 06/15/22 AM.
	TREE *getTree();
	LTREE *getLines();
	_TCHAR *getAppdir();				// 12/03/98 AM.
	_TCHAR *getAnasdir();
	_TCHAR *getKBdir();
	Dlist<Ipair> *getVars();		// 12/07/98 AM.
	Dlist<Iarg> *getOstrs();		// 05/23/01 AM.
	std::_t_ostream *getOut();				// 05/13/99 AM.
	std::_t_ostream *getSout();				// 05/13/99 AM.
	std::_t_ofstream *getFout();				// 05/13/99 AM.
	bool getFbatchstart();			// 10/19/00 AM.
#ifndef LINUX
	HINSTANCE getHdll();				// 01/29/99 AM.
	void setHdll(HINSTANCE);		// 01/29/99 AM.
#endif
	_TCHAR *getOutdir();				// 03/10/99 AM.
	COLL *getCollect();				// 05/17/00 AM.
	Nlppp *getNlppp();				// 05/17/00 AM.
	_TCHAR *getAlgo();					// 05/31/00 AM.
	bool	getIntern();				// 05/26/01 AM.
	long	getCurrpass();	// REN.	// 08/22/02 AM.
	long	getRulepass();				// 02/03/05 AM.
	long	getInputpass();			// 08/23/02 AM.
	Delt<Seqn> *getSeq();			// 07/23/01 AM.
	long getLine();					// 08/24/02 AM.
	long getInputline();				// 08/24/02 AM.
#ifdef LINUX
	std::_t_ostrstream *getCbuf();			// 05/11/02 AM. // 09/27/19 AM.
#else
	std::_t_ostrstream *getCbuf();			// 05/11/02 AM.
#endif
	long getCbufmax();				// 05/11/02 AM.
	long getCbuftot();				// 05/11/02 AM.
	bool getCbufover();				// 05/11/02 AM.

#ifdef _ODBC
	SQLHENV getHenv();				// 05/23/02 AM.
	SQLHDBC getHdbc();				// 05/23/02 AM.
	SQLHSTMT getHstmt();				// 05/25/02 AM.
	SDWORD getCursor();				// 05/26/02 AM.
	SDWORD *getPcursor();			// 05/26/02 AM.
#endif

	Dlist<Iarg> *getBlobs();		// 02/27/03 AM.
	_TCHAR *getDatum();					// 03/13/03 AM.
	std::_t_ostream *getCout();				// 05/04/03 AM.
	_TCHAR *getLogfile();	// VTLOG	// 05/06/13 AM.

	void setAna(Ana *);
	void setNLP(NLP*);				// 06/25/03 AM.
	void setHtab(Htab*);				// 06/25/03 AM.
	void setEana(Eana *);			// 10/13/99 AM.
	void setInput(_TCHAR *);
	void setOutput(_TCHAR *);
	void setText(_TCHAR *);
	void setLength(long);
	void setUlength(long);	// [UNICODE]	// 06/15/22 AM.
	void setTree(TREE *);
	void setLines(LTREE *);
	void setVerbose(bool);
	void setAppdir(_TCHAR *);			// 12/03/98 AM.
	void setAnasdir(_TCHAR *);
	void setKBdir(_TCHAR *);
	void setVars(Dlist<Ipair> *);	// 12/07/98 AM.
	void setOstrs(Dlist<Iarg> *);	// 05/23/01 AM.
	void setOut(std::_t_ostream *);			// 05/13/99 AM.
	void setSout(std::_t_ostream *);		// 05/13/99 AM.
	void setFout(std::_t_ofstream *);		// 05/13/99 AM.
	void setOutdir(_TCHAR *);			// 03/10/99 AM.
	void setCollect(COLL *);		// 05/17/00 AM.
	void setNlppp(Nlppp *);			// 05/17/00 AM.
	void setAlgo(_TCHAR *);			// 05/31/00 AM.
	void setFbatchstart(bool);		// 10/19/00 AM.
	void setIntern(bool);			// 05/26/01 AM.
	void setCurrpass(long);// REN.	// 08/22/02 AM.
	void setRulepass(long);			// 02/03/05 AM.
	void setInputpass(long);		// 08/23/02 AM.
	void setSeq(Delt<Seqn> *);		// 07/23/01 AM.
	void setLine(long);				// 08/24/02 AM.
	void setInputline(long);		// 08/24/02 AM.

	void setCbuf(std::_t_ostrstream *);	// 05/11/02 AM.
	void setCbufmax(long);			// 05/11/02 AM.
	void setCbuftot(long);			// 05/11/02 AM.
	void setCbufover(bool);			// 05/11/02 AM.

#ifdef _ODBC
	void setHenv(SQLHENV);			// 05/23/02 AM.
	void setHdbc(SQLHDBC);			// 05/23/02 AM.
	void setHstmt(SQLHSTMT);		// 05/25/02 AM.
	void setCursor(SDWORD);			// 05/26/02 AM.
#endif

	void setBlobs(Dlist<Iarg> *);	// 02/27/03 AM.
	void setDatum(_TCHAR*);			// 03/13/03 AM.
	void setCout(std::_t_ostream*);			// 05/04/03 AM.
	void setLogfile(_TCHAR*);	// VTLOG	// 05/06/13 AM.

	bool Verbose();

	bool readFile();			// Read input file into buffer.
	bool copyBuffer(															// 02/06/00 AM.
		_TCHAR *inbuf,
		long len = 0
		);
	void execute();			// EXECUTE THE TEXT ANALYSIS.

	void prettySeq();			// Print analyzer sequence.
	bool errOut(bool,bool=false);											// 08/24/02 AM.
	bool errOut(std::_t_strstream*,bool,bool=false);                 // 02/25/05 AM.

	inline void prettyPassnum(int,  /*UP*/ _TCHAR *);
	inline void prettyParsedone();

	void clean();				// Clean up after analysis.
	Sym *getSym(_TCHAR *);			// 11/19/98 AM.
	Sym *getSym(_TCHAR *str, long len);		// 03/05/99 AM.
	_TCHAR *internStr(_TCHAR *str, /*DU*/ _TCHAR* &tstr);
	_TCHAR *internStr(_TCHAR *str, long len, /*DU*/ _TCHAR* &tstr);
												// 03/05/99 AM.
	// For single-stepping through the Parse.		// 05/13/99 AM.
	// Execute next pass of parse.
	bool stepExecute(	// 05/15/99 AM.
		Delt<Seqn> *seq,			// Current step in sequence.
		int num						// Pass number.
		);

	void finalTree();															// 10/11/99 AM.
	void finExecute(															// 06/13/00 AM.
		int num,						// Current pass number.
		clock_t s_time,
		clock_t e_time
		);

	// Get and intern pieces of input filename.						// 12/24/99 AM.
	_TCHAR *inputName();														// 12/24/99 AM.
	_TCHAR *inputPath();														// 12/24/99 AM.
	_TCHAR *inputParent();
	_TCHAR *inputHead();														// 12/24/99 AM.
	_TCHAR *inputTail();														// 12/24/99 AM.

	// Self-contained output functions.									// 02/17/01 AM.
	bool strOut(_TCHAR *);														// 02/17/01 AM.
	bool flushOut();															// 02/17/01 AM.

	//////////////////////////////
	// GENERATE RULES FROM PARSE TREE									// 05/19/99 AM.
	// Lite library support for Librug.dll.
	//////////////////////////////
	static bool genSample(													// 05/19/99 AM.
		long start,				// Start offset of sample in input text.
		long end,				// End offset of sample in input text.
		Parse *parse,			// Current text analysis instance.
		int num,					// Current pass number.
		_TCHAR *conname,			// Concept name.
		/*DU*/
		Delt<Seqn> *seq,				// Down: current pass. UP: new pass.
									// New pass contains rule for sample.
		Dlist<Irule> *rules,	// List of rules for current concept.
		Dlist<Iarg>  *selects	// List of context nodes.
		);
	static bool genxSample(			// 06/08/99 AM.
		_TCHAR *fname,			// Sample's text file name.
		long start,				// Start offset of sample in input text.
		long end,				// End offset of sample in input text.
		_TCHAR *text,				// Sample text.			// 06/16/99 AM.
		CONCEPT *conc,			// Current concept in Gram hierarchy.
		Parse *parse,			// Current text analysis instance.
		_TCHAR *conname,			// Concept name.
		Mode *mode,				// 08/09/99 AM.
		CG *cg,					// Current KB.
		/*DU*/
		Dlist<Irule> *rules,	// List of rules for current concept.
		Dlist<Iarg>  *selects	// List of context nodes.
		);
	static Delt<Irule> *genRule(					// 05/19/99 AM.
		Node<Pn> *start,
		Node<Pn> *end,
		_TCHAR *suggstr,
		Dlist<Irule> *rules,				// Current rules list being built.
		Mode *mode,							// 08/09/99 AM.
		Parse *parse
		);
	static bool genxLabels(	// 06/08/99 AM.
		CONCEPT *conc,		// Current concept in the Gram hierarchy.
		Irule *rule,		// Rule generated for current sample of conc.
		_TCHAR *fname,		// Sample's text file name.
		long start,			// Start text offset of sample.
		long end,			// End text offset of sample.
		Node<Pn> *n_start,	// Sample's first node in parse tree.
		Node<Pn> *n_end,		// Sample's last node in parse tree.
		bool closed,			// 07/19/99 AM.
		CG *cg,					// KB hierarchy object.
		Parse *parse
		);
	static bool genxLabel(			// 06/09/99 AM.
		_TCHAR *label,		// Label name to add to layering.
		Irule *rule,		// Rule generated for current sample of conc.
		int first,
		int last,
		Parse *parse
		);
	static bool labelOffsets(				// 07/15/99 AM.
		long start,				// Start text offset of label's sample.
		long end,				// End text offset of label's sample.
		Node<Pn> *n_start,	// Sample's first node in parse tree.
		Node<Pn> *n_end,		// Sample's last node in parse tree.
		/*UP*/
		int &first,				// Start offset of label's sample in rule.
		int &last				// End offset of label's sample in rule.
		);
	static bool labelAttrs(					// 07/15/99 AM.
		CONCEPT *label,		// Label name to add to layering.
		Irule *rule,			// Rule generated for current sample of conc.
		int first,				// First rule elt covered by label.
		int last,				// Last rule elt covered by label.
		bool h_closed,			// 07/19/99 AM.
		CG *cg
		);
	static bool fixClosed(				// 08/19/99 AM.
		Irule *rule
		);
	static bool					// 06/15/99 AM.
	sortGroups(
		Dlist<Iaction> *actions
		);
	static int cmpGroup(
		Delt<Iaction> *dptr,
		Delt<Iaction> *sptr,
		/*UP*/
		bool &err
		);
	static Dlist<Irule> *generalize(Dlist<Irule> *list,
		Mode *mode);	// 06/30/99 AM.
	static bool buildGen(				// 06/30/99 AM.
		Dlist<Irule> *gens,				// List of generalized rules.
		Irule *rule,						// Literal rule.
		bool f_attrs						// 07/16/99 AM.
		);
	static Dlist<Irule> *mergeGen(Dlist<Irule> *list); // 06/30/99 AM.
	static bool splitGens(		// 07/15/99 AM.
		Dlist<Irule> *gens,
		/*DU*/
		Dlist<Irule> *splits
		);
	static bool splitGen(		// 07/16/99 AM.
		Irule *gen,
		/*DU*/
		Dlist<Irule> *splits
		);
	static bool sortRulesbyLabels(	// 07/16/99 AM.
		Slist<Irule> *rules
		);
	static int cmpRulelabels(const void *, const void *);	// 07/16/99 AM.
	static bool splitRaw(				// 10/29/99 AM. (Rename)
		Irule *raw,
		/*DU*/
		Dlist<Irule> *splits
		);
	static bool splitFold(		// 07/16/99 AM.
		Irule *raw,					// RAW rule.
		Irule *split				// Generalized rule (with LABEL info).
		);
	static bool splitFoldphrase(	// 07/16/99 AM.
		Dlist<Ielt> *rphrase,	// RAW rule's phrase.
		Dlist<Ielt> *sphrase		// Generalized rule's phrase.
		);
	static bool splitFoldelt(		// 07/16/99 AM.
		Ielt *relt,						// RAW rule elt.
		Ielt *selt						// Generalized rule elt.
		);
	static bool splitFoldeltname(	// 07/16/99 AM.
		Ielt *relt,						// RAW rule elt.
		Ielt *selt						// Generalized rule elt.
		);
	static bool splitFoldposts(	// 07/16/99 AM.
		Dlist<Iaction> *rposts,		// RAW rule's posts.
		Dlist<Iaction> *sposts		// Generalized rule's posts.
		);
	static Dlist<Irule> *maxConstrains(		// 06/30/99 AM.
		Dlist<Irule> *gens		// List of generalized rules.
		);
	static Dlist<Irule> *maxConstrain(	// 06/30/99 AM.
		Dlist<Irule> *constrs,		// List of constrained rules built so far.
		Irule *gen						// One generalized rule.
		);
	static bool setupConstrain(				// 07/16/99 AM.
		Irule *rule			// Maximally constrained rule being built.
		);
	static bool foldConstrain(		// 07/03/99 AM.
		Irule *crule,			// Maximally constrained rule being built.
		Irule *rrule			// RAW sample to be folded in.
		);
	static bool foldPhrase(			// 07/03/99 AM.
		Dlist<Ielt> *cphrase,		// Phrase of constrained rule.
		Dlist<Ielt> *rphrase,		// Phrase of RAW rule.
		bool &cfull,					// If constrained rule was layered.
		bool &rfull						// If RAW rule was layered.
		);
	static bool foldElt(				// 07/03/99 AM.
		Ielt *celt,					// Phrase elt of constrained rule.
		Ielt *relt,					// Phrase elt of RAW rule.
		/*DU*/
		bool &cfull,				// If constrained rule was layered.
		bool &rfull					// If RAW rule was layered.
		);
	static bool foldEltname(			// 07/03/99 AM.
		Ielt *celt,					// Phrase elt of constrained rule.
		Ielt *relt					// Phrase elt of RAW rule.
		);
	static bool foldElttoken(			// 07/05/99 AM.
		Ielt *celt,					// Phrase elt of constrained rule.
		Ielt *relt					// Phrase elt of RAW rule.
		);
	static _TCHAR *tok_type(_TCHAR *str);		// 07/03/99 AM.
	static bool fixConstrains(Dlist<Irule> *constrains);	// 07/13/99 AM.
	static bool labelRule(								// 07/14/99 AM.
		Irule *trule,		// Template rule.
		Irule *rule			// Rule to fix.
		);
	static bool labelPhrase(							// 07/14/99 AM.
		Dlist<Ielt> *tphrase,		// Phrase of template rule.
		Dlist<Ielt> *phrase			// Phrase to label.
		);
	static bool labelElt(								// 07/14/99 AM.
		Ielt *telt,					// Phrase elt of template rule.
		Ielt *elt					// Phrase elt to label.
		);
	static bool labelPost(								// 07/14/99 AM.
		Irule *trule,			// Template rule.
		Irule *rule				// Rule to be labeled.
		);
	static bool pruneConstrains(				// 07/03/99 AM.
		Dlist<Irule> *constrains
		);
	static bool pruneConstrain(				// 07/03/99 AM.
		Irule *rule
		);
	static bool foldPost(						// 07/06/99 AM.
		Irule *crule,			// Maximally constrained rule being built.
		Irule *rrule,			// RAW sample to be folded in.
		bool cextra,			// If extra layering in constrained rule.
		bool rextra				// If extra layering in RAW rule.
		);
	static bool foldPres(						// 07/07/99 AM.
		Irule *crule,			// Maximally constrained rule being built.
		Irule *rrule			// RAW sample to be folded in.
		);
	static bool foldPreargs(					// 07/07/99 AM.
		Ipre *cpre,					// Pre action currently in rule being built.
		Ipre *rpre,					// Pre action in RAW rule being compared.
		Dlist<Ipre> *commons		// List of pre actions being built.
		);
	static Dlist<Iarg> *findSubs(			// 07/06/99 AM.
		Node<Pn> *node
		);
	static bool internRug(Dlist<Irule> *rules);	// 07/08/99 AM.
	static void closedOn(Irule *rule);				// 07/19/99 AM.
	static bool intersectKinds(Dlist<Iarg> *dlist, _TCHAR *key); // 08/09/99AM.
	static bool nixContext(								// 10/29/99 AM.
		/*DU*/
		Node<Pn>* &n_start,
		Node<Pn>* &n_end,
		Node<Pn>* &parent
		);
	static Dlist<Irule> *buildLiterals(					// 10/29/99 AM.
		Dlist<Irule> *raws			// List of RAW rules.
		);
	static bool buildLiteral(								// 10/29/99 AM.
		Irule *rule					// Rule to be literalized.
		);
	static bool cleanLitpres(								// 11/11/99 AM.
		Dlist<Ipre> *pres			// List of pre actions.
		);

	// New handling of ostreams.											// 05/23/01 AM.
	Delt<Iarg> *newostr(std::_t_ostream*);										// 05/23/01 AM.
	bool deleteostr(std::_t_ostream *);											// 05/23/01 AM.
	Delt<Iarg> *findostr(std::_t_ostream*);										// 05/23/01 AM.
	bool deleteostrs();														// 05/24/01 AM.

	Delt<Iarg> *newblob(long);												// 02/27/03 AM.
	bool deleteblobs();														// 02/27/03 AM.

#ifdef _ODBC
	// ODBC, SQL handling.													// 05/23/02 AM.
	bool dbclose();															// 05/23/02 AM.
	bool allocDbstmt();														// 05/25/02 AM.
	bool freeDbstmt();														// 05/25/02 AM.
#endif

	void finPass(														// 05/20/00 AM.
		int num,							// Pass number.
		bool flogfiles,
		std::_t_ofstream *fout,
		std::_t_ostream *sout,
		_TCHAR *pretname,
		bool ftimepass,
		clock_t &s_time
		);
#ifndef LINUX
        inline
#endif
	void iniPass(														// 05/20/00 AM.
		int num,					// Current pass number.
		_TCHAR *prefix,			// Prefix for naming dump files.
		bool flogfiles,
		bool ftimepass,
		bool active,
		_TCHAR *sfile,
		_TCHAR *salgo,
		/*DU*/
		std::_t_ofstream* &fout,		// File for outputting pass data.
		std::_t_ostream* &sout,		// For restoring output stream.
		clock_t &s_time,
		_TCHAR* &pretname
		);

private:
	Ana		*ana;						// The analyzer definition.
	NLP		*nlp_;					// Analyzer instance/env.		// 06/25/03 AM.
	Htab		*htab_;					// Ref to hash table to use.	// 06/25/03 AM.
	Eana		*eana_;					// Runtime config for parser.	// 10/13/99 AM.
											// (Just a reference; not owned by Parse)
	_TCHAR     input[FNAME_SIZE];	// The input file name.
	_TCHAR     output[FNAME_SIZE];	// The final/main output file path.
	_TCHAR		appdir_[FNAME_SIZE];	// Base directory for app.		// 12/03/98 AM.
	_TCHAR		kbdir_[FNAME_SIZE];	// Base directory for app.
	_TCHAR		outdir_[FNAME_SIZE];	// Intermediate out files.		// 03/10/99 AM.
	_TCHAR     *text;					// The buffer for input text.
	long		 length;					// Length of input text.
	long	ulength;	// [UNICODE]	// 06/15/22 AM.
	TREE *tree;					// The main parse tree.
	LTREE *lines;					// Tree of text lines.
	Dlist<Ipair> *vars_;				// List of user-created vars.	// 12/07/98 AM.
	Dlist<Iarg> *ostrs_;				// Global list for ostreams.	// 05/23/01 AM.

	bool		 verbose;				// If analyzer is running in verbose mode.
	_TCHAR		anasdir_[FNAME_SIZE];

	// Handle for the user-supplied output file.						// 01/07/99 AM.
	std::_t_ostream *out_;						// REQUIRED.

	// Output to a buffer.													// 05/11/02 AM.
	// (Looks like this should be an output-buffer class.)		// 05/11/02 AM.
	std::_t_ostrstream *cbuf_;		// Stream for output buffer.			// 05/11/02 AM.
	long cbufmax_;			// Length of output buffer.				// 05/11/02 AM.
	long cbuftot_;			// Total output length so far.			// 05/11/02 AM.
	bool cbufover_;		// Buffer overflow.							// 05/11/02 AM.

	// Rather than rebind cout, using this as output stream locus.
	// Calling analyzer sets this with the ostream it supplies.	// 05/04/03 AM.
	// This is a reference.  User must manage ostream.				// 05/04/03 AM.
	std::_t_ostream *cout_;															// 05/04/03 AM.

	// Save rebound streams.												// 05/13/99 AM.
	std::_t_ostream *sout_;															// 05/13/99 AM.
	std::_t_ofstream *fout_;															// 05/13/99 AM.

#ifdef _ODBC
	// MyODBC,MySQL connectivity.											// 05/23/02 AM.
	SQLHENV henv_;																// 05/23/02 AM.
	SQLHDBC hdbc_;																// 05/23/02 AM.

	// For now, allowing only on active statement handle.			// 05/25/02 AM.
	SQLHSTMT hstmt_;															// 05/25/02 AM.
	SDWORD cursor_;	// "Cursor buffer".								// 05/26/02 AM.
#endif


	// If this parse interns an analyzers (eg, RFB or user's analyzer),
	// then record if any syntax errors were encountered.			// 05/26/01 AM.
	bool intern_;	// If interned analyzer successfully.			// 05/26/01 AM.

	Dlist<Iarg> *blobs_;	// Global list for blobs.					// 02/27/03 AM.

	_TCHAR *logfile_;		// VTLOG	// 05/06/13 AM.

	// Handle for user.dll.													// 01/29/99 AM.
#ifndef LINUX
	HINSTANCE hdll_;															// 01/29/99 AM.
#endif

	// CODE GENERATION SUPPORT												// 05/31/00 AM.
	_TCHAR *algo_;	// Algorithm of the input file!					// 05/31/00 AM.

	// COMPILED RUNTIME SUPPORT.											// 05/17/00 AM.
	// Want one collect and one nlppp object built per text analysis,
	// if feasible.  Won't own these, but place them under control of the
	// compiled version.
	COLL *collect_;															// 05/17/00 AM.
	Nlppp *nlppp_;																// 05/17/00 AM.

	// BATCH START FLAG.  Handshake to say when the first of
	// a multi-file run (or folder or hierarchy of files) is
	// being processed.														// 10/19/00 AM.
	bool fbatchstart_;														// 10/19/00 AM.

	// Support API for stepping through a parse.						// 07/23/01 AM.
	long currpass_;	// Current pass number.			// RENAMED.	// 08/22/02 AM.
	Delt<Seqn> *seq_;	// Current pass.									// 07/23/01 AM.

	// Debugging info about input file.									// 08/23/02 AM.
	long inputpass_;	// If INTERNING, pass number of infile.	// 08/23/02 AM.
	
	// Pass of current rule.  (If inside a function,
	// currpass_ points to the function's pass.)
	long rulepass_;	// Current rule's pass.                   // 02/03/05 AM.

	// Moving line number tracking in here as well.					// 08/24/02 AM.
	// (Moving line_ from nlppp.h).										// 08/24/02 AM.

	// Parser's function/code/rule line number.						// 08/24/02 AM.
	long line_;		// Current parser line number.					// 08/24/02 AM.

	// Line number of input file being processed.					// 08/24/02 AM.
	long inputline_;															// 08/24/02 AM.

	// Pass info to analyzer, to G("$datum") variable.				// 03/13/03 AM.
	_TCHAR *datum_;																// 03/13/03 AM.

#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount(std::_t_ofstream* =0);		// Pretty-print the count.
private:
	static int count_;				// Count nodes currently allocated.
#endif
};


#endif
