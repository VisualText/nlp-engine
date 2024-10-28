/*******************************************************************************
Copyright © 2000-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	ARUN.H
* FILE:	include/API/lite/Arun.h
* CR:		05/10/00 AM.
* SUBJ:	API for runtime (compiled) analyzer.
*
*******************************************************************************/

#ifndef ARUN_H_
#define ARUN_H_

#include <iostream>
#include <assert.h>
#include "lite.h"

typedef void DELTS;              // Doubly-linked list.  // 07/07/03 AM.

class Parse;
class Seqn;
class Nlppp;
class RFASem;
class Iarg;	// 03/11/02 AM.
class CG;	// 12/02/02 AM.

// May want to determine this for each analyzer or pass.
#define MAX_RULE_LENGTH 500

typedef void NODE;

// Type of select region in rule file.									// 06/08/00 AM.
enum selectType																// 06/08/00 AM.
	{
	SELECTNULL,
	SELECTROOT,
	SELECTNODES,
	SELECTPATH,
	SELECTMULTI,
	SELECTPREMULTI,
	SELECTSENTINEL			// Dummy terminator.
	};

// Internal form for special elts.										// 06/09/00 AM.
// Keep alphabetized and in sync with Arun/specialArr.			// 06/09/00 AM.
extern _TCHAR *specialArr[];													// 06/09/00 AM.

// Global var set once only during VisualText run, by
// set_specialarr_len.														// 06/09/00 AM.
extern long SPECIALARR_LEN;												// 06/09/00 AM.
enum specialType																// 06/09/00 AM.
	{
	XNULL,
	XALPHA,	// _xALPHA
	XANY,		// _xANY
	XBLANK,	// _xBLANK
	XCAP,		// _xCAP
	XCAPLET,    // _xLET                                        // 07/10/09 AM.
	XCTRL,	// _xCTRL														// 07/24/00 AM.
	XEMOJI,
	XEND,		// _xEND
	XEOF,		// _xEOF
	XFEAT,	// _xFEAT [unimplemented]
	XLET,    // _xLET                                           // 07/10/09 AM.
	XNIL,		// _xNIL
	XNUM,		// _xNUM
	XPUNCT,	// _xPUNCT
	XSTART,	// _xSTART
	XWHITE,	// _xWHITE
	XWILD,	// _xWILD
	XSENTINEL
	};

// Structure for match,fail,except list data.						// 06/15/00 AM.
typedef struct s_elist
	{
	const _TCHAR **reg;		// The regular list of strings.			// 06/15/00 AM.
	const _TCHAR ***htab;	// The hash table of strings.				// 06/15/00 AM.
	long long hsize;		// Size of the hash table.							// 06/15/00 AM.
	const int *spec;		// The array of special tokens.			// 06/15/00 AM.
	} ELIST;

// Struct for rule hash table.											// 06/16/00 AM.
typedef struct s_rhash														// 06/16/00 AM.
	{
	_TCHAR *name;
	long long length;			// Length of rule array.					// 06/19/00 AM.
	const int *rules;
	} RHASH;																		// 06/16/00 AM.

// Keep all elt data. (Analog of ielt.h)								// 05/22/00 AM.
typedef struct s_elt
	{
	_TCHAR *name;
	const _TCHAR **layers;
	ELIST match;																// 06/15/00 AM.
	ELIST fail;																	// 06/15/00 AM.
	ELIST except;																// 06/15/00 AM.

#ifdef OLD_
	const _TCHAR **matches;
	const _TCHAR ***ematches;		// Compiled hash table.				// 06/14/00 AM.
	const int *smatches;														// 06/09/00 AM.

	const _TCHAR **fails;
	const _TCHAR ***efails;													// 06/14/00 AM.
	const int *sfails;														// 06/09/00 AM.

	const _TCHAR **excepts;
	const _TCHAR ***eexcepts;													// 06/14/00 AM.
	const int *sexcepts;														// 06/09/00 AM.
#endif

	int min;
	int max;
	_TCHAR *rename;
	int pres;		// Placeholder for preactions.
	int recurse;	// Placeholder for recurse minipasses.
//	int hash;		// Placeholder for hashing rule elt.
	bool wild;		// If this is a wildcard elt.
	int flags;		// Bit-flags.
	} ELT;

// Compiled runtime COLLECT data structure.
// Opt: will reset fields only as needed, when the pattern matcher
// gets there.
typedef struct s_coll
	{
	NODE *start;
	NODE *end;
	int totnodes;			// Total number of nodes assigned to this elt.
								// Useful for wildcard accounting.
	} COLL;

// Data structure for SUGGESTED CONCEPT.								// 05/19/00 AM.
// Add pass/line num for bug tracking/consistency with interp.	// 08/09/02 AM.
typedef struct s_sugg														// 05/19/00 AM.
	{
	_TCHAR *name;
	const _TCHAR **layers;
	bool base;
	bool unsealed;
	int passnum;	// PASS NUMBER OF CURRENT RULE.					// 08/09/02 AM.
	long long ruleline;	// LINE NUMBER OF CURRENT RULE.					// 08/09/02 AM.
	} SUGG;																		// 05/19/00 AM.

typedef bool (*MICROFN)(NODE *,const ELT *,Nlppp *);				// 05/25/00 AM.

// STATIC PASS INFORMATION.												// 06/20/00 AM.
typedef struct s_pass														// 06/20/00 AM.
	{
	bool (*code)(Nlppp *);				// Gen'd fn for code region of this pass.
	const _TCHAR **arr_select;			// Array of select node names.
	enum selectType seltype;			// Traversal mode.
	bool (*rules)(int,bool&,Nlppp *);	// Gen'd fn for rules of this pass.
	int num;									// Pass number.
	_TCHAR *sfile;							// Pass file name.
	const RHASH *must;					// Must-try rule data.
	const RHASH **htab;					// Rule hash table.
	long long hsize;								// Rule hash table length.
	} PASS;

// Exception numbers for NLP++ code.	// 09/15/08 AM.
#define ex_SUCCEED 0
#define ex_FAIL 1
#define ex_EXITPASS 2


/********************************************
* CLASS:	ARUN
* CR:		05/16/00 AM.
* SUBJ:	API class for compiled runtime analyzer.
********************************************/

class LITE_API Arun
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	Arun();										// Default constructor.
	~Arun();									// Destructor.

	// Access

	// Modify

	// General.

	// Class-wide


	// PASSES
	static bool tokenize(Parse *, int);
	static bool cmltokenize(Parse *, int);                      // 08/18/08 AM.
	static bool dicttok(Parse *, int);                          // 07/29/11 AM.
	static bool dicttokz(Parse *, int);                         // 08/16/11 AM.
	static bool lines(Parse *, int);
	static bool patExecute(
		Parse *,
		const PASS *,
		int num,
		_TCHAR *fname
#ifdef OLD_
		bool (code)(Nlppp *),
		const _TCHAR **,
		enum selectType,														// 06/08/00 AM.
		bool (rules)(int,bool &,Nlppp *),
		const RHASH *must,				// Must-try rule data.		// 06/19/00 AM.
		const RHASH **htab				// Rule hash table.			// 06/19/00 AM.
#endif
		);

	static bool recExecute(													// 06/01/00 AM.
		Parse *,
		const PASS *,
		int num,
		_TCHAR *fname
#ifdef OLD_
		bool (code)(Nlppp *),
		const _TCHAR **,
		enum selectType seltype,											// 06/08/00 AM.
		bool (rules)(int,bool &,Nlppp *),
		const RHASH *must,				// Must-try rule data.		// 06/19/00 AM.
		const RHASH **htab				// Rule hash table.			// 06/19/00 AM.
#endif
		);
	static bool checkfinal(bool,Nlppp*);								// 08/07/02 AM.
	static bool checkend(Nlppp*);                               // 08/07/02 AM.
	static bool exitpass(bool,Nlppp*);									// 08/07/02 AM.
	static bool succeed(Nlppp*);                                // 09/15/08 AM.
	static bool fail(Nlppp*);                                   // 09/15/08 AM.
	static bool checkfail(Nlppp*,int);                          // 09/15/08 AM.

	// PATTERN MATCHER
	static bool match_first(int,bool (match)(NODE *, const ELT *, Nlppp *),Nlppp *);
	static bool match_left(int,bool (match)(NODE *, const ELT *, Nlppp *),Nlppp *);
	static bool match_right(int,bool (match)(NODE *, const ELT *, Nlppp *),Nlppp *);

	static bool match_left_wild( int,Nlppp *);
	static bool match_right_wild(int,Nlppp *);

	static bool match_real_right(											// 05/23/00 AM.
		bool (match)(NODE *, const ELT *, Nlppp *),
		Nlppp *
		);
	static bool match_real_left(											// 05/30/00 AM.
		bool (match)(NODE *, const ELT *, Nlppp *),
		Nlppp *
		);

	static bool match_simple(NODE *,const ELT *,Nlppp *);			// 05/23/00 AM.
	static bool match_singlet(NODE *,const ELT *,Nlppp *);
	static bool match_tree(NODE *,const ELT *,Nlppp *);
	static bool match_list(NODE *,const ELT *,Nlppp *);
	static bool match_fails(NODE *,const ELT *,Nlppp *);
	static bool match_fails_tree(NODE *,const ELT *,Nlppp *);
	static bool match_fails_singlet(NODE *,const ELT *,Nlppp *);
	static bool match_list_tree(NODE *,const ELT *,Nlppp *);
	static bool match_list_singlet(NODE *,const ELT *,Nlppp *);

   // Deaccented element variants of match functions.          // 03/29/05 AM.
	static bool match_simple_deaccent(NODE*,const ELT*,Nlppp*);
	static bool match_singlet_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_tree_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_list_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_fails_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_fails_tree_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_fails_singlet_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_list_tree_deaccent(NODE *,const ELT *,Nlppp *);
	static bool match_list_singlet_deaccent(NODE *,const ELT *,Nlppp *);


   // Dejunk element variants of match functions.  // 09/09/11 AM.
	static bool match_simple_dejunk(NODE*,const ELT*,Nlppp*);
	static bool match_singlet_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_tree_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_list_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_fails_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_fails_tree_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_fails_singlet_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_list_tree_dejunk(NODE *,const ELT *,Nlppp *);
	static bool match_list_singlet_dejunk(NODE *,const ELT *,Nlppp *);

	static bool match_special_tok(										// 06/09/00 AM.
		NODE *node,
		const int *arr, 
		Nlppp *nlppp,
		bool																		// 07/08/03 AM.
		);
	static bool find_special_tok(enum specialType,const int*);	// 08/10/02 AM.

	// Special match mode functions.										// 05/25/00 AM.
	static bool special_first(int,
		bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
		MICROFN,Nlppp *);
	static bool special_left(int,
		bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
		MICROFN,Nlppp *);
	static bool special_right(int,
		bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
		MICROFN,Nlppp *);
	static bool special_real_left(										// 05/30/00 AM.
		bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
		MICROFN,
		Nlppp *
		);
	static bool special_real_right(										// 05/25/00 AM.
		bool (match)(NODE *, const ELT *,MICROFN, Nlppp *),
		MICROFN,
		Nlppp *
		);


	static bool special_simple(NODE *,const ELT *,MICROFN,Nlppp *);
	static bool special_singlet(NODE *,const ELT *,MICROFN,Nlppp *);
	static bool special_tree(NODE *,const ELT *,MICROFN,Nlppp *);

   // Deaccent variants.                                       // 03/29/05 AM.
	static bool special_simple_deaccent(NODE *,const ELT *,MICROFN,Nlppp *);
	static bool special_singlet_deaccent(NODE *,const ELT *,MICROFN,Nlppp *);
//	static bool special_tree_deaccent(NODE *,const ELT *,MICROFN,Nlppp *);

   // Dejunk variants.  // 09/09/11 AM.
	static bool special_simple_dejunk(NODE *,const ELT *,MICROFN,Nlppp *);
	static bool special_singlet_dejunk(NODE *,const ELT *,MICROFN,Nlppp *);
//	static bool special_tree_dejunk(NODE *,const ELT *,MICROFN,Nlppp *);

	// Special boundary matching fns.
	static bool match_start(int, Nlppp *);
	static bool match_end(int, Nlppp *);								// 05/25/00 AM.
	static bool match_eof(int, Nlppp *);								// 05/25/00 AM.
	static bool match_list_eof(NODE*,const ELT*);					// 08/10/02 AM.

	// Micro match functions.												// 05/25/00 AM.
	static bool micro_any(NODE *,const ELT *,Nlppp *);				// 05/25/00 AM.
	static bool micro_alpha(NODE *,const ELT *,Nlppp *);			// 05/25/00 AM.
	static bool micro_blank(NODE *,const ELT *,Nlppp *);			// 05/25/00 AM.
	static bool micro_cap(NODE *,const ELT *,Nlppp *);				// 05/25/00 AM.
	static bool micro_caplet(NODE *,const ELT *,Nlppp *);       // 07/10/09 AM.
	static bool micro_let(NODE *,const ELT *,Nlppp *);          // 07/10/09 AM.
	static bool micro_num(NODE *,const ELT *,Nlppp *);				// 05/25/00 AM.
	static bool micro_punct(NODE *,const ELT *,Nlppp *);			// 05/25/00 AM.
	static bool micro_ctrl(NODE *,const ELT *,Nlppp *);			// 07/24/00 AM.
	static bool micro_white(NODE *,const ELT *,Nlppp *);			// 05/25/00 AM.
	static bool micro_emoji(NODE *,const ELT *,Nlppp *);

//	static bool micro_feat(NODE *,const ELT *,Nlppp *);			// 05/25/00 AM.

	// For special toks in match lists.									// 06/09/00 AM.
	static bool micro_start(NODE *, Nlppp *);							// 06/09/00 AM.
	static bool micro_end(NODE *, Nlppp *);							// 06/09/00 AM.
	static bool micro_eof(NODE *, Nlppp *);							// 06/09/00 AM.


	// PATTERN MATCHER UTILITIES
	static bool match_tree(NODE *,const _TCHAR *,bool,Nlppp *);	// 05/24/00 AM.
	static bool match_tree_list(
		NODE *,
		ELIST,																	// 06/15/00 AM.
		bool,Nlppp *
		);

   // For deaccented matching.                                 // 03/29/05 AM.
	static bool match_tree_deaccent(NODE *,const _TCHAR *,bool,Nlppp *);
	static bool match_tree_list_deaccent(
		NODE *,
		ELIST,
		bool,Nlppp *
		);

   // For dejunk matching.       // 09/09/11 AM.
	static bool match_tree_dejunk(NODE *,const _TCHAR *,bool,Nlppp *);
	static bool match_tree_list_dejunk(
		NODE *,
		ELIST,
		bool,Nlppp *
		);

	static void switch_left(Nlppp *);									// 06/01/00 AM.
	static void switch_right(Nlppp *);									// 05/30/00 AM.
	static enum specialType find_special(_TCHAR *);					// 06/09/00 AM.
	static enum specialType set_specialarr_len();			// 06/09/00 AM.

	static bool special_tree(												// 05/25/00 AM.
		NODE *,const ELT *,bool,MICROFN,Nlppp *);

	static bool matched_rule(Nlppp *,bool=false);					// 08/09/02 AM.
	static bool init_analyzer(Parse *);
	static void init_rule(
		int,
		const SUGG &,
		const ELT *,
		bool (*)(int,Nlppp *),	// Gen'd fn for pre actions.		// 05/24/00 AM.
		int,							// Lookahead ord.						// 06/10/00 AM.
		Nlppp *);
	static NODE *get_colls(Nlppp *); 	// Get collect delts.
	static NODE *next_node(NODE *);		// Get next node.
	static void reset_collect(COLL *, int);							// 05/18/00 AM.
	static NODE *get_left_node(COLL *,int,int);						// 05/25/00 AM.
	static NODE *get_right_node(COLL *,int,int);						// 05/25/00 AM.

	static RHASH *hfind_lc(_TCHAR*,RHASH **);							// 06/20/00 AM.

	// ELT ("PAIR") ACTIONS
	static bool set_node_name(NODE *, _TCHAR *);
	static bool set_node_name(int, _TCHAR *, Nlppp *);				// 05/23/00 AM.
	static bool layer_elt(NODE *, const _TCHAR **, Nlppp *);
	static bool layer_elt(int, const _TCHAR **, Nlppp *);			// 05/23/00 AM.
	static bool recurse(														// 05/31/00 AM.
		int,						// Elt number.
		bool (rules)(int,bool &,Nlppp *),
		Nlppp *);
	static NODE *init_lookahead(Nlppp *);								// 06/10/00 AM.

	// POST ACTIONS
	static bool single(Nlppp *);
	static bool singler(Nlppp *,long,long);							// 06/02/00 AM.
	static bool singlex(Nlppp *,long,long);							// 06/02/00 AM.
	static bool	singlezap(Nlppp *);										// 01/04/01 AM.
	static bool merge(Nlppp *);											// 06/02/00 AM.
	static bool merger(Nlppp *,long,long);								// 06/02/00 AM.
	static bool listadd(Nlppp *,long,long,_TCHAR * =_T("false"));		// 06/02/00 AM.
	static bool listaddleft(Nlppp *,long,long,long = 0);			// 06/02/00 AM.
	static bool noop(Nlppp *);
	static bool default_post_action(Nlppp *);

	static bool group_elt(int,_TCHAR *,Nlppp *);						// 09/23/00 AM.
	static RFASem *group(Nlppp *, int, int, _TCHAR *);

	// GROUP VARIANTS.														// 04/28/01 AM.
	static RFASem *group(Nlppp*,RFASem*,int,_TCHAR *);					// 04/28/01 AM.
	static RFASem *group(Nlppp*,int,RFASem*,_TCHAR *);					// 04/28/01 AM.
	static RFASem *group(Nlppp*,int,int,RFASem*);						// 04/28/01 AM.
	static RFASem *group(Nlppp*,RFASem*,RFASem*,_TCHAR *);				// 04/28/01 AM.
	static RFASem *group(Nlppp*,RFASem*,int,RFASem*);					// 04/28/01 AM.
	static RFASem *group(Nlppp*,int,RFASem*,RFASem*);					// 04/28/01 AM.
	static RFASem *group(Nlppp*,RFASem*,RFASem*,RFASem*);				// 04/28/01 AM.
	// MISSING VARIANTS	// 04/25/10 AM.
	static RFASem *group(Nlppp*,int,int,int);         // 04/25/10 AM.


	static bool excise(Nlppp *, int, int);
	static bool splice(Nlppp *, long,long);							// 06/02/00 AM.
	static bool setbase(Nlppp *,long,_TCHAR *);							// 06/02/00 AM.
	static bool setunsealed(Nlppp *,long,_TCHAR *);					// 06/02/00 AM.
	static bool post_final(Nlppp *);

	// Selected post actions.												// 06/02/00 AM.
	static bool addstrs(Nlppp *,_TCHAR *,long);							// 06/02/00 AM.
	static bool print(Nlppp *, _TCHAR *);									// 06/02/00 AM.
	static bool printr(Nlppp *, long,long);							// 06/02/00 AM.
	static bool prlit(Nlppp *, _TCHAR *, _TCHAR *);						// 06/02/00 AM.
	static bool prrange(Nlppp *,_TCHAR *,long,long);					// 06/02/00 AM.
	static bool prchild(Nlppp *,_TCHAR *,long,_TCHAR *);				// 06/02/00 AM.
	static bool prtree(Nlppp *,_TCHAR *,long,_TCHAR *);					// 06/02/00 AM.
	static bool prxtree(Nlppp*,_TCHAR*,_TCHAR*,long,_TCHAR*,_TCHAR*);	// 06/02/00 AM.
	static bool ndump(Nlppp*,_TCHAR*,long=0);							// 06/02/00 AM.
	static bool xdump(Nlppp*,_TCHAR*,long=0);							// 06/02/00 AM.
	static bool gdump(Nlppp*,_TCHAR*);										// 06/02/00 AM.
	static bool sdump(Nlppp*,_TCHAR*);										// 06/02/00 AM.

	static bool addattr(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*);					// 06/02/00 AM.

	// PRE ACTIONS
	static bool uppercase(Nlppp *);										// 06/02/00 AM.
	static bool lowercase(Nlppp *);										// 06/02/00 AM.
	static bool cap(Nlppp *);												// 06/02/00 AM.
	static bool unknown(Nlppp *);											// 06/02/00 AM.
	static bool length(Nlppp *, int);									// 05/24/00 AM.
	static bool lengthr(Nlppp *,long,long);							// 06/02/00 AM.
	static bool numrange(Nlppp *,long long,long long);							// 06/02/00 AM.
	static bool var(Nlppp*,_TCHAR*);                            // 10/01/05 AM.
	static bool varz(Nlppp*,_TCHAR*);                           // 10/01/05 AM.
	static bool vareq(Nlppp*,_TCHAR*,_TCHAR*);                  // 10/01/05 AM.
	static bool vareq(Nlppp*,_TCHAR*,long long);                     // 10/01/05 AM.
	static bool varne(Nlppp*,_TCHAR*,_TCHAR*);                  // 10/01/05 AM.
	static bool varne(Nlppp*,_TCHAR*,long long);                     // 10/01/05 AM.

	static bool var(Nlppp*,RFASem*);                            // 10/04/05 AM.
	static bool varz(Nlppp*,RFASem*);                           // 10/04/05 AM.
	static bool vareq(Nlppp*,RFASem*,RFASem*);                  // 10/04/05 AM.
	static bool varne(Nlppp*,RFASem*,RFASem*);                  // 10/04/05 AM.

	static bool regexp(Nlppp*,RFASem*);                         // 03/23/09 AM.
	static bool regexpi(Nlppp*,RFASem*);                        // 03/26/09 AM.
	static bool regexp(Nlppp*,_TCHAR*);                         // 04/03/09 AM.
	static bool regexpi(Nlppp*,_TCHAR*);                        // 04/03/09 AM.

	// NLP++
	static RFASem *n(_TCHAR*,int,long,bool,Nlppp*);
	static RFASem *x(_TCHAR*,int,long,bool,Nlppp*);
	static RFASem *s(_TCHAR*,long,bool,Nlppp*);
	static RFASem *g(_TCHAR*,long,bool,Nlppp*);
	static RFASem *l(_TCHAR*,long,bool,Nlppp*);							// 03/09/02 AM.

	// VARIANTS (need all combos, eventually...)						// 05/04/01 AM.
	static RFASem *n(_TCHAR*,int,RFASem*,bool,Nlppp*);				// 05/04/01 AM.
	static RFASem *x(_TCHAR*,int, RFASem*,bool,Nlppp*);				// 05/04/01 AM.
	static RFASem *s(_TCHAR*,RFASem*,bool,Nlppp*);						// 05/04/01 AM.
	static RFASem *g(_TCHAR*,RFASem*,bool,Nlppp*);						// 05/04/01 AM.
	static RFASem *l(_TCHAR*,RFASem*,bool,Nlppp*);						// 03/09/02 AM.

	// NEW SYNTAX.																// 04/28/01 AM.
	static RFASem *n(int, Nlppp *);										// 04/28/01 AM.
	static RFASem *x(int, Nlppp *);										// 04/28/01 AM.
	static RFASem *s(Nlppp *);												// 04/28/01 AM.

	static RFASem *assign(int,_TCHAR*,int,long,Nlppp*,RFASem*);
	static long long    assign(int,_TCHAR*,int,long,Nlppp*,long long);
	static float   assign(int,_TCHAR*,int,long,Nlppp*,float);
	static _TCHAR   *assign(int,_TCHAR*,int,long,Nlppp*,_TCHAR*);
	static std::_t_ostream *assign(int,_TCHAR*,int,long,Nlppp*,std::_t_ostream*);	// 11/20/02 AM.
	static bool		assign(int,_TCHAR*,int,long,Nlppp*,bool);		// 12/10/02 AM.

	// VARIANTS.
	static RFASem  *assign(int,_TCHAR*,int,RFASem*,Nlppp*,RFASem*);
	static long long     assign(int,_TCHAR*,int,RFASem*,Nlppp*,long long);
	static float    assign(int,_TCHAR*,int,RFASem*,Nlppp*,float);
	static _TCHAR    *assign(int,_TCHAR*,int,RFASem*,Nlppp*,_TCHAR*);
	static std::_t_ostream *assign(int,_TCHAR*,int,RFASem*,Nlppp*,std::_t_ostream*);//11/20/02 AM.
	static bool     assign(int,_TCHAR*,int,RFASem*,Nlppp*,bool);	// 12/10/02 AM.

	// INDEXED ASSIGNMENT.
	static RFASem *iassign(int,_TCHAR*,int,long,Nlppp*,RFASem*);
	static long long    iassign(int,_TCHAR*,int,long,Nlppp*,long long);
	static float   iassign(int,_TCHAR*,int,long,Nlppp*,float);
	static _TCHAR   *iassign(int,_TCHAR*,int,long,Nlppp*,_TCHAR*);
	static std::_t_ostream *iassign(int,_TCHAR*,int,long,Nlppp*,std::_t_ostream*); // 11/20/02 AM.
	static bool    iassign(int,_TCHAR*,int,long,Nlppp*,bool);		// 12/10/02 AM.

	static bool truth(long long);
	static bool truth(float);
	static bool truth(_TCHAR *);
	static bool truth(RFASem *);
	static bool truth(bool);												// 09/03/01 AM.

	static RFASem *plus(RFASem *, long long,Nlppp *);
	static float plus(RFASem*,float,Nlppp*);							// 08/17/01 AM.
	static _TCHAR *plus(RFASem *, _TCHAR *,Nlppp *);
	static RFASem *plus(RFASem *, RFASem *,Nlppp *);  // ambig: numeric or str.
	static _TCHAR *plus(_TCHAR *, _TCHAR *,Nlppp *);
	static _TCHAR *plus(_TCHAR*,long long,Nlppp*);                       // 07/11/03 AM.
	static _TCHAR *plus(long long,_TCHAR*,Nlppp*);                       // 07/11/03 AM.
	static _TCHAR *plus(_TCHAR *, RFASem *,Nlppp *);
	static long long plus(long long,long long,Nlppp *);
	static RFASem *plus(long long,RFASem *,Nlppp *);
	static float plus(float,RFASem *,Nlppp *);						// 08/17/01 AM.
	static float plus(float,long long,Nlppp *);								// 08/17/01 AM.
	static float plus(long long,float,Nlppp *);								// 08/17/01 AM.
	static float plus(float,float,Nlppp *);							// 08/17/01 AM.

	static RFASem *minus(RFASem *, long long);
	static float minus(RFASem*,float);									// 08/17/01 AM.
	static RFASem *minus(RFASem *, RFASem *);							// 08/17/01 AM.
	static long long minus(long long, long long);
	static RFASem *minus(long long, RFASem *);
	static float minus(float,RFASem*);									// 08/17/01 AM.
	static float minus(float,long long);
	static float minus(long long,float);
	static float minus(float,float);

	static RFASem *times(RFASem *, long long);
	static float times(RFASem*,float);
	static RFASem *times(RFASem *, RFASem *);
	static long long times(long long, long long);
	static RFASem *times(long long, RFASem *);
	static float times(float,RFASem*);
	static float times(long long,float);
	static float times(float,long long);
	static float times(float,float);

	static RFASem *divide(RFASem *, long long);
	static float divide(RFASem*,float);
	static RFASem *divide(RFASem *, RFASem *);
	static long long divide(long long, long long);
	static RFASem *divide(long long, RFASem *);
	static float divide(float,RFASem*);
	static float divide(float,long long);
	static float divide(long long,float);
	static float divide(float,float);

	static long long rem(RFASem *, long long);
	static long long rem(RFASem *, RFASem *);
	static long long rem(long long, long long);
	static long long rem(long long, RFASem *);
	
	static long conf(RFASem *, long);
	static long conf(RFASem *, RFASem *);
	static long conf(long, long);
	static long conf(long, RFASem *);

	// Note: Should really return bool type....not in NLP++ yet.
	static long eq(RFASem*,long long);
	static long eq(RFASem*,float);
	static long eq(RFASem *, RFASem *);
	static long eq(RFASem *, _TCHAR *);
	static long eq(long long, long long);
	static long eq(long long,RFASem*);
	static long eq(float,RFASem*);
	static long eq(_TCHAR *, _TCHAR *);
	static long eq(_TCHAR *, RFASem *);
	static long eq(long long,float);
	static long eq(float,long long);
	static long eq(float,float);
	static long eq(_TCHAR*,long long);                  // 07/11/03 AM.
	static long eq(long long,_TCHAR*);                  // 07/11/03 AM.


	static long ne(RFASem*,long long);
	static long ne(RFASem*,float);
	static long ne(RFASem *, RFASem *);
	static long ne(long long, long long);
	static long ne(long long, RFASem *);
	static long ne(float,RFASem*);
	static bool ne(_TCHAR *, _TCHAR *);
	static bool ne(_TCHAR *, RFASem *);
	static bool ne(RFASem *, _TCHAR *);
	static long ne(float,long long);
	static long ne(long long,float);
	static long ne(float,float);
	static long ne(_TCHAR*,long long);                  // 07/11/03 AM.
	static long ne(long long,_TCHAR*);                  // 07/11/03 AM.

	static long gt(RFASem *, long long);
	static long gt(RFASem *, float);
	static long gt(RFASem *, RFASem *);
	static long gt(long long, long long);
	static long gt(long long, RFASem *);
	static long gt(float, RFASem*);
	static long gt(float,long long);
	static long gt(long long,float);
	static long gt(float,float);

	static long lt(RFASem *, long long);
	static long lt(RFASem*,float);
	static long lt(RFASem *, RFASem *);
	static long lt(long long, long long);
	static long lt(long long, RFASem *);
	static long lt(float,RFASem*);
	static long lt(float,long long);
	static long lt(long long,float);
	static long lt(float,float);

	static long ge(RFASem *, long long);
	static long ge(RFASem*,float);
	static long ge(RFASem *, RFASem *);
	static long ge(long long, long long);
	static long ge(long long, RFASem *);
	static long ge(float,RFASem*);
	static long ge(float,long long);
	static long ge(long long,float);
	static long ge(float,float);

	static long le(RFASem *, long long);
	static long le(RFASem*,float);
	static long le(RFASem *, RFASem *);
	static long le(long long, long long);
	static long le(long long, RFASem *);
	static long le(float,RFASem*);
    static long le(float,long long);
	static long le(long long,float);
	static long le(float,float);

	static long vtand(RFASem *, long long);
	static long vtand(RFASem*,float);
	static long vtand(RFASem *, RFASem *);
	static long vtand(RFASem *, _TCHAR *);									// 06/11/00 AM.
	static bool vtand(RFASem*,bool);										// 09/03/01 AM.
	static long vtand(long long, long long);
	static long vtand(long long, RFASem *);
	static long vtand(long long, _TCHAR *);										// 06/11/00 AM.
	static long vtand(long long,float);
	static bool vtand(long long,bool);											// 09/03/01 AM.
	static long vtand(float,RFASem*);
	static long vtand(float,_TCHAR*);
	static long vtand(float,long long);
	static long vtand(float,float);
	static bool vtand(float,bool);											// 09/03/01 AM.
	static long vtand(_TCHAR *, RFASem *);									// 06/11/00 AM.
	static long vtand(_TCHAR *, long long);										// 06/11/00 AM.
	static long vtand(_TCHAR*,float);
	static long vtand(_TCHAR *, _TCHAR *);										// 06/11/00 AM.
	static bool vtand(_TCHAR*,bool);											// 09/03/01 AM.min(
	static bool vtand(bool,RFASem*);										// 09/03/01 AM.
	static bool vtand(bool,long long);											// 09/03/01 AM.
	static bool vtand(bool,float);											// 09/03/01 AM.
	static bool vtand(bool,_TCHAR*);											// 09/03/01 AM.
	static bool vtand(bool,bool);											// 09/03/01 AM.

	static long vtor(RFASem *, long long);
	static long vtor(RFASem *, float);
	static long vtor(RFASem *, RFASem *);
	static long vtor(RFASem *, _TCHAR *);									// 06/11/00 AM.
	static bool vtor(RFASem*,bool);											// 09/03/01 AM.
	static long vtor(long long, long long);
	static long vtor(long long,float);
	static long vtor(long long, RFASem *);
	static long vtor(long long, _TCHAR *);											// 06/11/00 AM.
	static bool vtor(long long,bool);												// 09/03/01 AM.
	static long vtor(float,long long);
	static long vtor(float,float);
	static long vtor(float, RFASem *);
	static long vtor(float, _TCHAR*);
	static bool vtor(float,bool);											// 09/03/01 AM.
	static long vtor(_TCHAR*, RFASem *);										// 06/11/00 AM.
	static long vtor(_TCHAR*, long long);											// 06/11/00 AM.
	static long vtor(_TCHAR*,float);
	static long vtor(_TCHAR*, _TCHAR *);										// 06/11/00 AM.
	static bool vtor(_TCHAR*,bool);											// 09/03/01 AM.
	static bool vtor(bool,RFASem*);											// 09/03/01 AM.
	static bool vtor(bool,long long);												// 09/03/01 AM.
	static bool vtor(bool,float);											// 09/03/01 AM.
	static bool vtor(bool,_TCHAR*);											// 09/03/01 AM.
	static bool vtor(bool,bool);												// 09/03/01 AM.

   static RFASem *vtcout(Nlppp*);                                 // 05/06/09 AM.

	// Unary ops.
	static long long inc(RFASem *,Nlppp *);
	static long long inc(															// 06/02/00 AM.
		int typ,
		_TCHAR *varname,
		int nelt,
		long index,																// 10/13/00 AM.
		Nlppp *nlppp
		);
	static long long inc(int,_TCHAR*,int,RFASem*,Nlppp*);              // 07/15/03 AM.

	static long long dec(RFASem *,Nlppp *);
	static long long dec(															// 06/02/00 AM.
		int typ,
		_TCHAR *varname,
		int nelt,
		long index,																// 10/13/00 AM.
		Nlppp *nlppp
		);
	static long long dec(int,_TCHAR*,int,RFASem*,Nlppp*);              // 07/15/03 AM.

	static long vtnot(long long);
	static long vtnot(float);
	static long vtnot(RFASem *);
	static long vtnot(_TCHAR *);
	static bool vtnot(bool);													// 09/03/01 AM.

	static long long plus(long long,Nlppp *);
	static float plus(float,Nlppp *);
	static RFASem *plus(RFASem *,Nlppp *);

	static long long minus(long long);
	static float minus(float);
	static RFASem *minus(RFASem *);

	// Postunary.
	static long long postinc(RFASem *,Nlppp *);
	static long long postinc(														// 06/02/00 AM.
		int typ,
		_TCHAR *varname,
		int nelt,
		long index,																// 10/13/00 AM.
		Nlppp *nlppp
		);

	static long long postdec(RFASem *,Nlppp *);
	static long long postdec(														// 06/02/00 AM.
		int typ,
		_TCHAR *varname,
		int nelt,
		long index,																// 10/13/00 AM.
		Nlppp *nlppp
		);


	static std::_t_ostream *out(_TCHAR*,RFASem *,Nlppp*);
	static std::_t_ostream *out(_TCHAR*,_TCHAR *,Nlppp*);
	static std::_t_ostream *out(_TCHAR*,long long,Nlppp*);
	static std::_t_ostream *out(_TCHAR*,float,Nlppp*);
	static std::_t_ostream *out(_TCHAR*,bool,Nlppp*);                    // 07/11/03 AM.
	static std::_t_ostream *out(RFASem*,RFASem *,Nlppp*);
	static std::_t_ostream *out(RFASem*,_TCHAR *,Nlppp*);
	static std::_t_ostream *out(RFASem*,long long,Nlppp*);
	static std::_t_ostream *out(RFASem*,float,Nlppp*);
	static std::_t_ostream *out(RFASem*,bool,Nlppp*);                 // 07/11/03 AM.
	static std::_t_ostream *out(std::_t_ostream*,RFASem *,Nlppp*);
	static std::_t_ostream *out(std::_t_ostream*,_TCHAR *,Nlppp*);
	static std::_t_ostream *out(std::_t_ostream*,long long,Nlppp*);
	static std::_t_ostream *out(std::_t_ostream*,float,Nlppp*);
	static std::_t_ostream *out(std::_t_ostream*,bool,Nlppp*);                  // 07/11/03 AM.

	static bool stmt(RFASem *);
	static bool stmt(std::_t_ostream *);
	static bool stmt(long long);
	static bool stmt(float);
	static bool stmt(_TCHAR *);
	static bool stmt(bool);													// 09/03/01 AM.

	// CODE ACTIONS
	static bool sortvals(
		Nlppp *nlppp,
		_TCHAR *var
		);
	static bool gtolower(
		Nlppp *nlppp,
		_TCHAR *var
		);
	static bool guniq(
		Nlppp *nlppp,
		_TCHAR *var
		);
	static bool lookup(
		Nlppp *nlppp,
		_TCHAR *var,
		_TCHAR *fname,
		_TCHAR *flag
		);
	static bool startout(
		Nlppp *nlppp
		);
	static bool stopout(
		Nlppp *nlppp
		);
	static bool fileout(
		Nlppp *nlppp,
		_TCHAR *fname
		);


	// FN.CPP FUNCTIONS
	static long long factorial(Nlppp*,long long);
	static long long factorial(Nlppp*,RFASem*);								// 05/06/01 AM.
	static RFASem *findroot(Nlppp*);
	static RFASem *findconcept(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *findconcept(Nlppp*,RFASem*,long long);
												// FIX.	Lowercased the C	// 06/24/00 AM.
	static RFASem *findconcept(Nlppp*,RFASem*,RFASem*);			// 05/06/01 AM.

	static RFASem *findattr(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *findattr(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.

	static RFASem *findattrs(Nlppp*,RFASem*);

	static _TCHAR *attrname(Nlppp*,RFASem*);
	static int attrtype(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *attrvals(Nlppp*,RFASem*);

	static RFASem *findvals(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *findvals(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.

	static long long numval(Nlppp*,RFASem*,_TCHAR*);
	static long long numval(Nlppp*,RFASem*,RFASem*);						// 05/05/01 AM.

	static float fltval(Nlppp*,RFASem*,_TCHAR*);						// 12/27/06 AM.
	static float fltval(Nlppp*,RFASem*,RFASem*);						// 12/27/06 AM.

	static _TCHAR *strval(Nlppp*,RFASem*,_TCHAR*);
	static _TCHAR *strval(Nlppp*,RFASem*,RFASem*);						// 05/06/01 AM.

	static RFASem *conval(Nlppp*,RFASem*,_TCHAR*);						// 08/12/00 AM.
	static RFASem *conval(Nlppp*,RFASem*,RFASem*);					// 05/06/01 AM.

	static _TCHAR *inheritval(Nlppp*,RFASem*,_TCHAR*,RFASem*);
	static _TCHAR *inheritval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static _TCHAR *conceptname(Nlppp*,RFASem*);

	static _TCHAR *conceptpath(Nlppp*,RFASem*);

	static _TCHAR *wordpath(Nlppp*,_TCHAR*);
	static _TCHAR *wordpath(Nlppp*,RFASem*);								// 05/06/01 AM.

	static _TCHAR *findwordpath(Nlppp*,_TCHAR*);
	static _TCHAR *findwordpath(Nlppp*,RFASem*);						// 05/06/01 AM.

	static RFASem *wordindex(Nlppp*,_TCHAR*);
	static RFASem *wordindex(Nlppp*,RFASem*);							// 05/06/01 AM.

	static RFASem *pathconcept(Nlppp*,_TCHAR*);
	static RFASem *pathconcept(Nlppp*,RFASem*);						// 05/06/01 AM.

	static RFASem *findhierconcept(Nlppp*,_TCHAR*,RFASem*);
	static RFASem *findhierconcept(Nlppp*,RFASem*,RFASem*);		// 05/05/01 AM.

	static bool attrexists(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool attrexists(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 05/06/01 AM.
	static bool attrexists(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool attrexists(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool attrwithval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool attrwithval(Nlppp*,RFASem*,RFASem*,_TCHAR*);		// 05/06/01 AM.
	static bool attrwithval(Nlppp*,RFASem*,_TCHAR*,RFASem*);		// 05/06/01 AM.
	static bool attrwithval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool attrchange(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,_TCHAR*);
	// New variants below.													// 05/06/01 AM.
	static bool attrchange(Nlppp*,RFASem*,RFASem*,_TCHAR*,_TCHAR*);
	static bool attrchange(Nlppp*,RFASem*,_TCHAR*,RFASem*,_TCHAR*);
	static bool attrchange(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,RFASem*);
	static bool attrchange(Nlppp*,RFASem*,RFASem*,RFASem*,_TCHAR*);
	static bool attrchange(Nlppp*,RFASem*,RFASem*,_TCHAR*,RFASem*);
	static bool attrchange(Nlppp*,RFASem*,_TCHAR*,RFASem*,RFASem*);
	static bool attrchange(Nlppp*,RFASem*,RFASem*,RFASem*,RFASem*);

	static RFASem *down(Nlppp*,RFASem*);

	static RFASem *up(Nlppp*,RFASem*);

	static RFASem *next(Nlppp*,RFASem*);

	static RFASem *prev(Nlppp*,RFASem*);

	static RFASem *nextattr(Nlppp*,RFASem*);

	static RFASem *nextval(Nlppp*,RFASem*);

	static _TCHAR *getsval(Nlppp*,RFASem*);

	static _TCHAR *getstrval(Nlppp*,RFASem*);

	static long long getnumval(Nlppp*,RFASem*);
	static long long getfltval(Nlppp*,RFASem*);

	static RFASem *getconval(Nlppp*,RFASem*);							// 08/12/00 AM.

	static RFASem *makeconcept(Nlppp*,RFASem*,_TCHAR*,int=0);
	static RFASem *makeconcept(Nlppp*,RFASem*,RFASem*,int=0);	// 05/06/01 AM.
	static RFASem *makeconcept(Nlppp*,RFASem*,_TCHAR*,RFASem*);	// 05/06/01 AM.
	static RFASem *makeconcept(Nlppp*,RFASem*,RFASem*,RFASem*);	// 05/06/01 AM.

#ifdef DISABLED_
	static RFASem *addconcept(
		Nlppp *nlppp,
		RFASem *con_sem,
		RFASem *child_sem,
		long long pos_num=0
		);
#endif

	static RFASem *addattr(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *addattr(Nlppp*,RFASem*,RFASem*);					// 05/06/01 AM.

	static bool addsval(Nlppp*,RFASem*,_TCHAR*,long long);
	static bool addsval(Nlppp*,RFASem*,RFASem*,long long);				// 05/06/01 AM.
	static bool addsval(Nlppp*,RFASem*,_TCHAR*,RFASem*);				// 05/06/01 AM.
	static bool addsval(Nlppp*,RFASem*,RFASem*,RFASem*);			// 05/06/01 AM.

	static bool addstrval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool addstrval(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/05/01 AM.
	static bool addstrval(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 05/06/01 AM.
	static bool addstrval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool addnumval(Nlppp*,RFASem*,_TCHAR*,long long);
	static bool addnumval(Nlppp*,RFASem*,RFASem*,long long);			// 05/06/01 AM.
	static bool addnumval(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool addnumval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool addconval(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 08/11/00 AM.
	static bool addconval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static RFASem *getconcept(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *getconcept(Nlppp*,RFASem*,RFASem*);				// 05/05/01 AM.

	static bool rmconcept(Nlppp*,RFASem*);

	static bool rmchild(Nlppp*,RFASem*,_TCHAR*);
	static bool rmchild(Nlppp*,RFASem*,int);
	static bool rmchild(Nlppp*,RFASem*,RFASem*);	// Ambiguous.	// 05/06/01 AM.

	static bool rmvals(Nlppp*,RFASem*,_TCHAR*);
	static bool rmvals(Nlppp*,RFASem*,RFASem*);						// 05/06/01 AM.

	static bool rmval(Nlppp*,RFASem*,RFASem*);

	static bool rmattr(Nlppp*,RFASem*,_TCHAR*);
	static bool rmattr(Nlppp*,RFASem*,RFASem*);						// 05/06/01 AM.

	static bool rmattrval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool rmattrval(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 05/06/01 AM.
	static bool rmattrval(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool rmattrval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool rmchildren(Nlppp*,RFASem*);

	static bool prunephrases(Nlppp*,RFASem*);

	static bool replaceval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool replaceval(Nlppp*,RFASem*,_TCHAR*,long long);
	static bool replaceval(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 06/27/00 AM.
	static bool replaceval(Nlppp*,RFASem*,RFASem*,long long);			// 05/05 01 AM.
	static bool replaceval(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/05/01 AM.

	static bool renameconcept(Nlppp*,RFASem*,_TCHAR*);
	static bool renameconcept(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.

	static bool renamechild(Nlppp*,RFASem*,long long,_TCHAR*);
	static bool renamechild(Nlppp*,RFASem*,RFASem*,_TCHAR*);		// 05/06/01 AM.
	static bool renamechild(Nlppp*,RFASem*,long long,RFASem*);			// 05/06/01 AM.
	static bool renamechild(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool renameattr(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool renameattr(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 05/06/01 AM.
	static bool renameattr(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool renameattr(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool movecleft(Nlppp*,RFASem*);

	static bool movecright(Nlppp*,RFASem*);

	static RFASem *findphrase(Nlppp*, RFASem*);

	static bool sortphrase(Nlppp*, RFASem*);
	static bool sortchilds(Nlppp*, RFASem*);                    // 02/20/09 AM.
	static bool sorthier(Nlppp*, RFASem*);                      // 02/20/09 AM.

	static long phraselength(Nlppp*, RFASem*);

	static RFASem *nodeconcept(Nlppp*, RFASem*);
	static RFASem *nodeowner(Nlppp*,RFASem*);							// 12/03/02 AM.

	static RFASem *findnode(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *findnode(Nlppp*,RFASem*,long);
	static RFASem *findnode(Nlppp*,RFASem*,RFASem*);	// Ambig	// 05/06/01 AM.

	static RFASem *listnode(Nlppp*,RFASem*);

	static RFASem *firstnode(Nlppp*,RFASem*);

	static RFASem *lastnode(Nlppp*,RFASem*);

	static RFASem *makephrase(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *makephrase(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.

	static RFASem *addcnode(Nlppp*,RFASem*,_TCHAR*);
	static RFASem *addcnode(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.

	static RFASem *addnode(Nlppp*,RFASem*,_TCHAR*,long);
	static RFASem *addnode(Nlppp*,RFASem*,RFASem*,long);			// 05/06/01 AM.
	static RFASem *addnode(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static RFASem *addnode(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.

	static bool rmnode(Nlppp*,RFASem*);

	static bool rmphrase(Nlppp*,RFASem*);

	static bool rmcphrase(Nlppp*,RFASem*);

	static bool renamenode(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);
	static bool renamenode(Nlppp*,RFASem*,long,_TCHAR*);
	static bool renamenode(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool renamenode(Nlppp*,RFASem*,long,RFASem*);			// 05/06/01 AM.
	static bool renamenode(Nlppp*,RFASem*,RFASem*,_TCHAR*); //Amb	// 05/06/01 AM.
	static bool renamenode(Nlppp*,RFASem*,RFASem*,RFASem*);//Amb// 05/06/01 AM.

	static bool strcontains(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strcontains(Nlppp*,RFASem*,_TCHAR*);					// 05/06/01 AM.
	static bool strcontains(Nlppp*,_TCHAR*,RFASem*);					// 05/06/01 AM.
	static bool strcontains(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.
	static bool strcontains(Nlppp*,long,RFASem*);               // 07/11/03 AM.
	static bool strcontains(Nlppp*,RFASem*,long);               // 07/11/03 AM.
	static bool strcontains(Nlppp*,long,_TCHAR*);                 // 07/11/03 AM.
	static bool strcontains(Nlppp*,_TCHAR*,long);                 // 07/11/03 AM.
	static bool strcontains(Nlppp*,long,long);		            // 07/11/03 AM.

	static bool strcontainsnocase(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strcontainsnocase(Nlppp*,RFASem*,_TCHAR*);			// 05/06/01 AM.
	static bool strcontainsnocase(Nlppp*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool strcontainsnocase(Nlppp*,RFASem*,RFASem*);		// 05/06/01 AM.
	static bool strcontainsnocase(Nlppp*,long,long);            // 07/11/03 AM.
	static bool strcontainsnocase(Nlppp*,long,_TCHAR*);           // 07/11/03 AM.
	static bool strcontainsnocase(Nlppp*,_TCHAR*,long);           // 07/11/03 AM.
	static bool strcontainsnocase(Nlppp*,long,RFASem*);         // 07/11/03 AM.
	static bool strcontainsnocase(Nlppp*,RFASem*,long);         // 07/11/03 AM.

	static bool strequal(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strequal(Nlppp*,RFASem*,_TCHAR*);						// 05/06/01 AM.
	static bool strequal(Nlppp*,_TCHAR*,RFASem*);						// 05/06/01 AM.
	static bool strequal(Nlppp*,RFASem*,RFASem*);					// 05/06/01 AM.
	static bool strequal(Nlppp*,RFASem*,long);                  // 07/11/03 AM.
	static bool strequal(Nlppp*,long,RFASem*);                  // 07/11/03 AM.
	static bool strequal(Nlppp*,_TCHAR*,long);                    // 07/11/03 AM.
	static bool strequal(Nlppp*,long,_TCHAR*);                    // 07/11/03 AM.
	static bool strequal(Nlppp*,long,long);                     // 07/11/03 AM.

	static bool strnotequal(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strnotequal(Nlppp*,RFASem*,_TCHAR*);					// 05/06/01 AM.
	static bool strnotequal(Nlppp*,_TCHAR*,RFASem*);					// 05/06/01 AM.
	static bool strnotequal(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.
	static bool strnotequal(Nlppp*,long,long);                  // 07/11/03 AM.
	static bool strnotequal(Nlppp*,long,_TCHAR*);                 // 07/11/03 AM.
	static bool strnotequal(Nlppp*,_TCHAR*,long);                 // 07/11/03 AM.
	static bool strnotequal(Nlppp*,long,RFASem*);               // 07/11/03 AM.
	static bool strnotequal(Nlppp*,RFASem*,long);               // 07/11/03 AM.

	static bool strequalnocase(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strequalnocase(Nlppp*,RFASem*,_TCHAR*);				// 05/06/01 AM.
	static bool strequalnocase(Nlppp*,_TCHAR*,RFASem*);				// 05/06/01 AM.
	static bool strequalnocase(Nlppp*,RFASem*,RFASem*);			// 05/06/01 AM.
	static bool strequalnocase(Nlppp*,RFASem*,long);            // 07/11/03 AM.
	static bool strequalnocase(Nlppp*,long,RFASem*);            // 07/11/03 AM.
	static bool strequalnocase(Nlppp*,_TCHAR*,long);              // 07/11/03 AM.
	static bool strequalnocase(Nlppp*,long,_TCHAR*);              // 07/11/03 AM.
	static bool strequalnocase(Nlppp*,long,long);               // 07/11/03 AM.

	static bool strnotequalnocase(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strnotequalnocase(Nlppp*,RFASem*,_TCHAR*);			// 05/06/01 AM.
	static bool strnotequalnocase(Nlppp*,_TCHAR*,RFASem*);			// 05/06/01 AM.
	static bool strnotequalnocase(Nlppp*,RFASem*,RFASem*);		// 05/06/01 AM.
	static bool strnotequalnocase(Nlppp*,RFASem*,long);         // 07/11/03 AM.
	static bool strnotequalnocase(Nlppp*,long,RFASem*);         // 07/11/03 AM.
	static bool strnotequalnocase(Nlppp*,_TCHAR*,long);           // 07/11/03 AM.
	static bool strnotequalnocase(Nlppp*,long,_TCHAR*);           // 07/11/03 AM.
	static bool strnotequalnocase(Nlppp*,long,long);            // 07/11/03 AM.

	static bool strgreaterthan(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strgreaterthan(Nlppp*,RFASem*,_TCHAR*);				// 05/06/01 AM.
	static bool strgreaterthan(Nlppp*,_TCHAR*,RFASem*);				// 05/06/01 AM.
	static bool strgreaterthan(Nlppp*,RFASem*,RFASem*);			// 05/06/01 AM.
	static bool strgreaterthan(Nlppp*,long,long);               // 07/11/03 AM.
	static bool strgreaterthan(Nlppp*,long,_TCHAR*);              // 07/11/03 AM.
	static bool strgreaterthan(Nlppp*,_TCHAR*,long);              // 07/11/03 AM.
	static bool strgreaterthan(Nlppp*,long,RFASem*);            // 07/11/03 AM.
	static bool strgreaterthan(Nlppp*,RFASem*,long);            // 07/11/03 AM.

	static bool strlessthan(Nlppp*,_TCHAR*,_TCHAR*);
	static bool strlessthan(Nlppp*,RFASem*,_TCHAR*);					// 05/06/01 AM.
	static bool strlessthan(Nlppp*,_TCHAR*,RFASem*);					// 05/06/01 AM.
	static bool strlessthan(Nlppp*,RFASem*,RFASem*);				// 05/06/01 AM.
	static bool strlessthan(Nlppp*,long,long);                  // 07/11/03 AM.
	static bool strlessthan(Nlppp*,long,_TCHAR*);                 // 07/11/03 AM.
	static bool strlessthan(Nlppp*,_TCHAR*,long);                 // 07/11/03 AM.
	static bool strlessthan(Nlppp*,long,RFASem*);               // 07/11/03 AM.
	static bool strlessthan(Nlppp*,RFASem*,long);               // 07/11/03 AM.

	static RFASem *dictfindword(Nlppp*,_TCHAR*);
	static RFASem *dictfindword(Nlppp*,RFASem*);						// 05/05/01 AM.
	static RFASem *dictgetword(Nlppp*,_TCHAR*);							// 06/29/03 AM.
	static RFASem *dictgetword(Nlppp*,RFASem*);						// 06/29/03 AM.

	static _TCHAR *strtolower(Nlppp*,_TCHAR*);
	static _TCHAR *strtolower(Nlppp*,RFASem*);							// 05/05/01 AM.
	static _TCHAR *strtolower(Nlppp*,long);                       // 07/11/03 AM.

	static _TCHAR *strtoupper(Nlppp*,_TCHAR*);
	static _TCHAR *strtoupper(Nlppp*,RFASem*);							// 05/06/01 AM.
	static _TCHAR *strtoupper(Nlppp*,long);                       // 07/11/03 AM.

	static RFASem *addword(Nlppp*,_TCHAR*);
	static RFASem *addword(Nlppp*,RFASem*);							// 05/05/01 AM.

	static long strlength(Nlppp*,_TCHAR*);
	static long strlength(Nlppp*,RFASem*);								// 01/23/01 AM.
	static long strlength(Nlppp*,long);                         // 07/11/03 AM.

	static _TCHAR *strpiece(Nlppp*,_TCHAR*,long,long);
	static _TCHAR *strpiece(Nlppp*,RFASem*,long,long);				// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,_TCHAR*,RFASem*,long);				// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,_TCHAR*,long,RFASem*);				// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,RFASem*,RFASem*,long);			// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,RFASem*,long,RFASem*);			// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,_TCHAR*,RFASem*,RFASem*);			// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/06/01 AM.
	static _TCHAR *strpiece(Nlppp*,long,long,long);               // 07/11/03 AM.
	static _TCHAR *strpiece(Nlppp*,long,RFASem*,long);            // 07/11/03 AM.
	static _TCHAR *strpiece(Nlppp*,long,long,RFASem*);            // 07/11/03 AM.
	static _TCHAR *strpiece(Nlppp*,long,RFASem*,RFASem*);         // 07/11/03 AM.

	static _TCHAR *strchar(Nlppp*,_TCHAR*,long);
	static _TCHAR *strchar(Nlppp*,RFASem*,long);						// 05/06/01 AM.
	static _TCHAR *strchar(Nlppp*,_TCHAR*,RFASem*);						// 05/06/01 AM.
	static _TCHAR *strchar(Nlppp*,RFASem*,RFASem*);					// 05/06/01 AM.
	static _TCHAR *strchar(Nlppp*,long,long);                     // 07/11/03 AM.
	static _TCHAR *strchar(Nlppp*,long,RFASem*);                  // 07/11/03 AM.

	static bool strisdigit(Nlppp*,_TCHAR*);
	static bool strisdigit(Nlppp*,RFASem*);							// 05/07/01 AM.
	static bool strisdigit(Nlppp*,long);                        // 07/11/03 AM.

	static bool strisalpha(Nlppp*,_TCHAR*);
	static bool strisalpha(Nlppp*,RFASem*);							// 05/07/01 AM.
	static bool strisalpha(Nlppp*,long);                        // 07/11/03 AM.

	static bool strhaspunct(Nlppp*,_TCHAR*);
	static bool strhaspunct(Nlppp*,RFASem*);
	static bool strhaspunct(Nlppp*,long);

	static bool take(Nlppp*,_TCHAR*);
	static bool take(Nlppp*,RFASem*);									// 05/07/01 AM.

//////////////

	static bool xrename(Nlppp*,_TCHAR*, long=0);
	static bool xrename(Nlppp*,RFASem*, long=0);						// 05/07/01 AM.
	static bool xrename(Nlppp*,_TCHAR*,RFASem*);						// 05/07/01 AM.
	static bool xrename(Nlppp*,RFASem*,RFASem*);						// 05/07/01 AM.

	static bool debug(Nlppp *);

	// Obsolete, don't bother with this one.							// 05/07/01 AM.
	static bool fprintvar(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR* =0);

	static bool hash_str_nocase(_TCHAR*,const ELIST);

	static const int *hfind_lc(											// 06/21/00 AM.
		_TCHAR *name,
		const RHASH **htab,
		long hsize,
		/*UP*/
		long &len
		);
	static int *mergeRules(											// 06/21/00 AM.
		int *arr1,
		long size1,			// Size of array.
		int *arr2,
		long size2
		);
//////////////

	static bool rmword(Nlppp*,_TCHAR*);									// 07/26/00 AM.
	static bool rmword(Nlppp*,RFASem*);									// 05/07/01 AM.

	static bool vtsystem(Nlppp*,_TCHAR*);									// 07/26/00 AM.
	static bool vtsystem(Nlppp*,RFASem*);									// 05/07/01 AM.

	static _TCHAR *str(Nlppp*,RFASem*);							// 08/01/00 AM.
	static _TCHAR *str(Nlppp*,_TCHAR*);								// 07/31/00 AM.
	static _TCHAR *str(Nlppp*,long long);								// 07/31/00 AM.
	static _TCHAR *str(Nlppp*,float);								// 08/16/01 AM.

	static long long num(Nlppp*,RFASem*);								// 08/01/00 AM.
	static long long num(Nlppp*,_TCHAR*);								// 07/31/00 AM.
	static long long num(Nlppp*,long long);									// 07/31/00 AM.
	static long long num(Nlppp*,float);								// 08/10/08 AM.

	static float flt(Nlppp*,long long);										// 08/23/01 AM.
	static float flt(Nlppp*,float);										// 08/23/01 AM.
	static float flt(Nlppp*,_TCHAR*);										// 08/23/01 AM.
	static float flt(Nlppp*,RFASem*);									// 08/23/01 AM.

	// Wrap a sem object around a data type.							// 03/07/02 AM.
	static RFASem *sem(long long);												// 03/07/02 AM.
	static RFASem *sem(float);												// 03/07/02 AM.
	static RFASem *sem(_TCHAR*);												// 03/07/02 AM.
	static RFASem *sem(RFASem*);	// Noop.								// 03/07/02 AM.

	static _TCHAR *strtotitle(Nlppp*,_TCHAR*);								// 08/22/00 AM.
	static _TCHAR *strtotitle(Nlppp*,RFASem*);							// 08/22/00 AM.

	static long  arraylength(Nlppp*,_TCHAR*);								// 01/04/01 AM.
	static long  arraylength(Nlppp*,long long);								// 01/04/01 AM.
	static long  arraylength(Nlppp*,RFASem*);							// 01/04/01 AM.

	static RFASem *lasteltnode(Nlppp*,long);							// 01/04/01 AM.
	static RFASem *lasteltnode(Nlppp*,RFASem*);						// 01/04/01 AM.
	static RFASem *eltnode(Nlppp*,long);                        // 09/12/06 AM.
	static RFASem *eltnode(Nlppp*,RFASem*);                     // 09/12/06 AM.

	static _TCHAR *phrasetext(Nlppp*);										// 01/06/01 AM.

	static _TCHAR *phraseraw(Nlppp*);										// 01/06/01 AM.

	static RFASem *pnvar(Nlppp*,NODE*,_TCHAR*);							// 01/06/01 AM.
	static RFASem *pnvar(Nlppp*,NODE*,RFASem*);						// 05/07/01 AM.
	static RFASem *pnvar(Nlppp*,RFASem*,_TCHAR*);						// 04/29/01 AM.
	static RFASem *pnvar(Nlppp*,RFASem*,RFASem*);					// 05/07/01 AM.

	static RFASem *pnsingletdown(Nlppp*,NODE*);						// 01/08/01 AM.
	static RFASem *pnsingletdown(Nlppp*,RFASem*);					// 04/29/01 AM.

	static _TCHAR *pnname(Nlppp*,NODE*);									// 01/08/01 AM.
	static _TCHAR *pnname(Nlppp*,RFASem*);								// 04/29/01 AM.

	static RFASem *pnroot(Nlppp*);										// 01/08/01 AM.

	static RFASem *pndown(Nlppp*,NODE*);								// 01/08/01 AM.
	static RFASem *pndown(Nlppp*,RFASem*);								// 04/29/01 AM.

	static RFASem *pnup(Nlppp*,NODE*);									// 01/08/01 AM.
	static RFASem *pnup(Nlppp*,RFASem*);								// 04/29/01 AM.

	static RFASem *pnnext(Nlppp*,NODE*);								// 01/08/01 AM.
	static RFASem *pnnext(Nlppp*,RFASem*);								// 04/29/01 AM.

	static RFASem *pnprev(Nlppp*,NODE*);								// 01/08/01 AM.
	static RFASem *pnprev(Nlppp*,RFASem*);								// 04/29/01 AM.

	static bool pnremoveval(Nlppp*,NODE*,_TCHAR*);
	static _TCHAR *pnrename(Nlppp*,NODE*,_TCHAR*);						// 01/08/01 AM.
	static _TCHAR *pnrename(Nlppp*,RFASem*,_TCHAR*);						// 04/28/01 AM.
	static _TCHAR *pnrename(Nlppp*,NODE*,RFASem*);						// 04/28/01 AM.
	static _TCHAR *pnrename(Nlppp*,RFASem*,RFASem*);					// 04/28/01 AM.

	static bool pndeletechilds(Nlppp*,NODE*);							// 01/08/01 AM.
	static bool pndeletechilds(Nlppp*,RFASem*);						// 04/29/01 AM.

	static RFASem *varinlist(Nlppp*,_TCHAR*,long);						// 01/08/01 AM.
	static RFASem *varinlist(Nlppp*,RFASem*,long);					// 05/07/01 AM.
	static RFASem *varinlist(Nlppp*,_TCHAR*,RFASem*);					// 05/07/01 AM.
	static RFASem *varinlist(Nlppp*,RFASem*,RFASem*);				// 05/07/01 AM.

	static bool batchstart(Nlppp*);										// 01/08/01 AM.
	static bool interactive(Nlppp*);										// 05/06/02 AM.

	static _TCHAR *topdir(Nlppp*,_TCHAR*);									// 01/08/01 AM.
	static _TCHAR *topdir(Nlppp*,RFASem*);								// 05/07/01 AM.

	static RFASem *unpackdirs(Nlppp*,_TCHAR*);							// 01/08/01 AM.
	static RFASem *unpackdirs(Nlppp*,RFASem*);						// 05/04/01 AM.
	static RFASem *unpackdirs(Nlppp*,long);                     // 07/11/03 AM.

	static bool rmattrs(Nlppp*,RFASem*);								// 01/08/01 AM.

	static _TCHAR *strclean(Nlppp*,_TCHAR*);								// 01/08/01 AM.
	static _TCHAR *strclean(Nlppp*,RFASem*);								// 05/07/01 AM.
	static _TCHAR *strclean(Nlppp*,long);                         // 07/11/03 AM.

	static _TCHAR *strtrim(Nlppp*,_TCHAR*);									// 01/08/01 AM.
	static _TCHAR *strtrim(Nlppp*,RFASem*);								// 05/07/01 AM.
	static _TCHAR *strtrim(Nlppp*,long);                          // 07/11/03 AM.
	
	static _TCHAR *struniquechars(Nlppp*,_TCHAR*);
	static _TCHAR *struniquechars(Nlppp*,RFASem*);
	static _TCHAR *struniquechars(Nlppp*,long);

	static bool strstartswith(Nlppp*,_TCHAR*,_TCHAR*);				// 08/05/21 Dd.
	static bool strstartswith(Nlppp*,RFASem*,_TCHAR*);				// 08/05/21 Dd.
	static bool strstartswith(Nlppp*,_TCHAR*,RFASem*);				// 08/05/21 Dd.
	static bool strstartswith(Nlppp*,RFASem*,RFASem*);				// 08/05/21 Dd.

	static bool strendswith(Nlppp*,_TCHAR*,_TCHAR*);						// 01/08/01 AM.
	static bool strendswith(Nlppp*,RFASem*,_TCHAR*);					// 04/28/01 AM.
	static bool strendswith(Nlppp*,_TCHAR*,RFASem*);					// 04/28/01 AM.
	static bool strendswith(Nlppp*,RFASem*,RFASem*);				// 04/28/01 AM.

	static _TCHAR *strrchr(Nlppp*,_TCHAR*,_TCHAR*);							// 01/09/01 AM.
	static _TCHAR *strrchr(Nlppp*,RFASem*,_TCHAR*);						// 04/28/01 AM.
	static _TCHAR *strrchr(Nlppp*,_TCHAR*,RFASem*);						// 04/28/01 AM.
	static _TCHAR *strrchr(Nlppp*,RFASem*,RFASem*);						// 04/28/01 AM.

	static long strchrcount(Nlppp*,_TCHAR*,_TCHAR*);						// 12/15/02 AM.
	static long strchrcount(Nlppp*,_TCHAR*,RFASem*);					// 12/15/02 AM.
	static long strchrcount(Nlppp*,RFASem*,_TCHAR*);					// 12/15/02 AM.
	static long strchrcount(Nlppp*,RFASem*,RFASem*);				// 12/15/02 AM.

	// FIXED. strchr was mangled to _tcschr by a global replace.    // 05/14/08 AM.
	static _TCHAR *strchr(Nlppp*,_TCHAR*,_TCHAR*);							// 01/09/01 AM.
	static _TCHAR *strchr(Nlppp*,RFASem*,_TCHAR*);						// 05/07/01 AM.
	static _TCHAR *strchr(Nlppp*,_TCHAR*,RFASem*);						// 05/07/01 AM.
	static _TCHAR *strchr(Nlppp*,RFASem*,RFASem*);					// 05/07/01 AM.

	static _TCHAR *strwrap(Nlppp*,_TCHAR*,long);							// 01/09/01 AM.
	static _TCHAR *strwrap(Nlppp*,RFASem*,long);						// 05/07/01 AM.
	static _TCHAR *strwrap(Nlppp*,_TCHAR*,RFASem*);						// 05/07/01 AM.
	static _TCHAR *strwrap(Nlppp*,RFASem*,RFASem*);					// 05/07/01 AM.

	static _TCHAR *strsubst(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*);				// 01/09/01 AM.
	static _TCHAR *strsubst(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);				// 05/07/01 AM.
	static _TCHAR *strsubst(Nlppp*,_TCHAR*,RFASem*,_TCHAR*);				// 05/07/01 AM.
	static _TCHAR *strsubst(Nlppp*,_TCHAR*,_TCHAR*,RFASem*);				// 05/07/01 AM.
	static _TCHAR *strsubst(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 05/07/01 AM.
	static _TCHAR *strsubst(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 05/07/01 AM.
	static _TCHAR *strsubst(Nlppp*,_TCHAR*,RFASem*,RFASem*);			// 05/07/01 AM.
	static _TCHAR *strsubst(Nlppp*,RFASem*,RFASem*,RFASem*);		// 05/07/01 AM.

	static _TCHAR *strsubst(Nlppp*,_TCHAR*,_TCHAR*,long);          // 07/12/08 AM.
	static _TCHAR *strsubst(Nlppp*,RFASem*,_TCHAR*,long);          // 07/12/08 AM.
	static _TCHAR *strsubst(Nlppp*,_TCHAR*,RFASem*,long);          // 07/12/08 AM.
	static _TCHAR *strsubst(Nlppp*,RFASem*,RFASem*,long);          // 07/12/08 AM.

	static RFASem *openfile(												// 01/09/01 AM.
		Nlppp *nlppp,
		_TCHAR *fname,
		_TCHAR *a = 0,
		_TCHAR *b = 0,
		_TCHAR *c = 0
		);
	// Variants below.	// 05/07/01 AM.
	static RFASem *openfile(Nlppp*,_TCHAR*,RFASem*,_TCHAR* =0, _TCHAR* = 0);
	static RFASem *openfile(Nlppp*,_TCHAR*,_TCHAR*,RFASem*, _TCHAR* = 0);
	static RFASem *openfile(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*, RFASem*);
	static RFASem *openfile(Nlppp*,_TCHAR*,RFASem*,RFASem*, _TCHAR* = 0);
	static RFASem *openfile(Nlppp*,_TCHAR*,RFASem*,_TCHAR*, RFASem*);
	static RFASem *openfile(Nlppp*,_TCHAR*,_TCHAR*,RFASem*, RFASem*);
	static RFASem *openfile(Nlppp*,_TCHAR*,RFASem*,RFASem*, RFASem*);

	static RFASem *openfile(												// 05/05/01 AM.
		Nlppp *,
		RFASem*,
		_TCHAR * = 0,
		_TCHAR * = 0,
		_TCHAR * = 0
		);
	// Variants below.	// 05/07/01 AM.
	static RFASem *openfile(Nlppp*,RFASem*,RFASem*,_TCHAR* =0, _TCHAR* = 0);
	static RFASem *openfile(Nlppp*,RFASem*,_TCHAR*,RFASem*, _TCHAR* = 0);
	static RFASem *openfile(Nlppp*,RFASem*,_TCHAR*,_TCHAR*, RFASem*);
	static RFASem *openfile(Nlppp*,RFASem*,RFASem*,RFASem*, _TCHAR* = 0);
	static RFASem *openfile(Nlppp*,RFASem*,RFASem*,_TCHAR*, RFASem*);
	static RFASem *openfile(Nlppp*,RFASem*,_TCHAR*,RFASem*, RFASem*);
	static RFASem *openfile(Nlppp*,RFASem*,RFASem*,RFASem*, RFASem*);

   // Null variants. // 07/26/11 AM.
	static RFASem *openfile( // 07/26/11 AM.
		Nlppp *nlppp,
		_TCHAR *fname,
		long,
		long = 0,
		long = 0
		);
	static RFASem *openfile( // 07/26/11 AM.
		Nlppp *,
		RFASem*,
		long,
		long = 0,
		long = 0
		);


	static bool closefile(Nlppp*,RFASem*);								// 05/21/01 AM.
	static bool closefile(Nlppp*,std::_t_ostream*);							// 05/21/01 AM.

	static _TCHAR *strspellcandidate(Nlppp*,_TCHAR*,_TCHAR*);			// 01/09/01 AM.
	static _TCHAR *strspellcandidate(Nlppp*,RFASem*,_TCHAR*);			// 05/07/01 AM.
	static _TCHAR *strspellcandidate(Nlppp*,_TCHAR*,RFASem*);			// 05/07/01 AM.
	static _TCHAR *strspellcandidate(Nlppp*,RFASem*,RFASem*);		// 05/07/01 AM.

   // strspellcompare: Return long long NOT int, conform w Arun fns. // 08/09/11 AM.
	static long strspellcompare(Nlppp*,_TCHAR*,_TCHAR*);         // 01/09/01 AM.
	static long strspellcompare(Nlppp*,RFASem*,_TCHAR*);         // 05/07/01 AM.
	static long strspellcompare(Nlppp*,_TCHAR*,RFASem*);         // 05/07/01 AM.
	static long strspellcompare(Nlppp*,RFASem*,RFASem*);         // 05/07/01 AM.

	static _TCHAR *spellcandidates(Nlppp*,_TCHAR*);						// 01/09/01 AM.
	static _TCHAR *spellcandidates(Nlppp*,RFASem*);					// 05/07/01 AM.
	static _TCHAR *spellcandidates(Nlppp*,long);                  // 07/11/03 AM.

	static bool spellword(Nlppp*,_TCHAR*);								// 01/09/01 AM.
	static bool spellword(Nlppp*,RFASem*);								// 01/23/01 AM.
	static bool spellword(Nlppp*,long);                         // 07/11/03 AM.

	static _TCHAR *spellcorrect(Nlppp*,_TCHAR*);							// 01/09/01 AM.
	static _TCHAR *spellcorrect(Nlppp*,RFASem*);						// 05/07/01 AM.
	static _TCHAR *spellcorrect(Nlppp*,long);                     // 07/11/03 AM.

	static _TCHAR *lj(Nlppp*,_TCHAR*,long);									// 01/09/01 AM.
	static _TCHAR *lj(Nlppp*,RFASem*,long);								// 01/23/01 AM.
	static _TCHAR *lj(Nlppp*,_TCHAR*,RFASem*);								// 05/07/01 AM.
	static _TCHAR *lj(Nlppp*,RFASem*,RFASem*);							// 05/07/01 AM.

	static _TCHAR *LJ(Nlppp*,_TCHAR*,long);									// 01/09/01 AM.
	static _TCHAR *LJ(Nlppp*,RFASem*,long);								// 01/23/01 AM.
	static _TCHAR *LJ(Nlppp*,_TCHAR*,RFASem*);								// 01/23/01 AM.
	static _TCHAR *LJ(Nlppp*,RFASem*,RFASem*);							// 05/07/01 AM.

	static _TCHAR *rightjustifynum(Nlppp*,long,long);					// 01/09/01 AM.
	static _TCHAR *rightjustifynum(Nlppp*,RFASem*,long);				// 01/23/01 AM.
	static _TCHAR *rightjustifynum(Nlppp*,long,RFASem*);				// 05/07/01 AM.
	static _TCHAR *rightjustifynum(Nlppp*,RFASem*,RFASem*);			// 05/07/01 AM.

	static _TCHAR *percentstr(Nlppp*,long,long);						// 01/09/01 AM.
	static _TCHAR *percentstr(Nlppp*,RFASem*,long);					// 05/07/01 AM.
	static _TCHAR *percentstr(Nlppp*,long,RFASem*);					// 05/07/01 AM.
	static _TCHAR *percentstr(Nlppp*,RFASem*,RFASem*);				// 05/07/01 AM.

	static long hitconf(Nlppp*,long,long,long);						// 01/09/01 AM.
	static long hitconf(Nlppp*,RFASem*,long,long);					// 05/07/01 AM.
	static long hitconf(Nlppp*,long,RFASem*,long);					// 05/07/01 AM.
	static long hitconf(Nlppp*,long,long,RFASem*);					// 05/07/01 AM.
	static long hitconf(Nlppp*,RFASem*,RFASem*,long);				// 05/07/01 AM.
	static long hitconf(Nlppp*,RFASem*,long,RFASem*);				// 05/07/01 AM.
	static long hitconf(Nlppp*,long,RFASem*,RFASem*);				// 05/07/01 AM.
	static long hitconf(Nlppp*,RFASem*,RFASem*,RFASem*);			// 05/07/01 AM.

	static bool exitpass(Nlppp*);											// 02/06/01 AM.

	static _TCHAR *today(Nlppp*);											// 02/08/01 AM.

	static bool pncopyvars(Nlppp*,RFASem*);							// 05/16/01 AM.
	static bool pncopyvars(Nlppp*,long);								// 05/16/01 AM.
	static bool pncopyvars(Nlppp*);										// 05/16/01 AM.
	static bool pncopyvars(Nlppp*,NODE*);								// 05/16/01 AM.

	// Newer variants.
	static bool pncopyvars(Nlppp*,RFASem*,RFASem*);					// 08/04/02 AM.				
	static bool pncopyvars(Nlppp*,RFASem*,NODE*);					// 08/04/02 AM.				
	static bool pncopyvars(Nlppp*,NODE*,RFASem*);					// 08/04/02 AM.
	static bool pncopyvars(Nlppp*,NODE*,NODE*);						// 08/04/02 AM.

	static _TCHAR *strescape(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*);				// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);			// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,_TCHAR*,RFASem*,_TCHAR*);			// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,_TCHAR*,_TCHAR*,RFASem*);			// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,_TCHAR*,RFASem*,RFASem*);			// 07/24/01 DD.
	static _TCHAR *strescape(Nlppp*,RFASem*,RFASem*,RFASem*);		// 07/24/01 DD.

	static _TCHAR *strunescape(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*);			// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);			// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,_TCHAR*,RFASem*,_TCHAR*);			// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,_TCHAR*,_TCHAR*,RFASem*);			// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,RFASem*,RFASem*,_TCHAR*);		// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,RFASem*,_TCHAR*,RFASem*);		// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,_TCHAR*,RFASem*,RFASem*);		// 07/24/01 DD.
	static _TCHAR *strunescape(Nlppp*,RFASem*,RFASem*,RFASem*);	// 07/24/01 DD.

	static RFASem *kbdumptree(Nlppp*,RFASem*,_TCHAR*);				// 08/06/01 AM.
	static RFASem *kbdumptree(Nlppp*,RFASem*,RFASem*);				// 08/06/01 AM.

	static RFASem *writekb(Nlppp*,_TCHAR*);
	static RFASem *writekb(Nlppp*,RFASem*);

	static bool strisupper(Nlppp*,_TCHAR*);								// 11/20/01 AM.
	static bool strislower(Nlppp*,_TCHAR*);								// 11/20/01 AM.
	static bool strisupper(Nlppp*,RFASem*);							// 11/20/01 AM.
	static bool strislower(Nlppp*,RFASem*);							// 11/20/01 AM.
	static bool strisupper(Nlppp*,long);                        // 07/11/03 AM.
	static bool strislower(Nlppp*,long);                        // 07/11/03 AM.

	static bool suffix(Nlppp*,_TCHAR*,_TCHAR*);							// 11/23/01 AM.
	static bool suffix(Nlppp*,RFASem*,_TCHAR*);							// 11/23/01 AM.
	static bool suffix(Nlppp*,_TCHAR*,RFASem*);							// 11/23/01 AM.
	static bool suffix(Nlppp*,RFASem*,RFASem*);						// 11/23/01 AM.

	static long levenshtein(Nlppp*,_TCHAR*,_TCHAR*);         // 03/06/07 AM.
	static long levenshtein(Nlppp*,_TCHAR*,RFASem*);         // 03/06/07 AM.
	static long levenshtein(Nlppp*,RFASem*,_TCHAR*);         // 03/06/07 AM.
	static long levenshtein(Nlppp*,RFASem*,RFASem*);         // 03/06/07 AM.
	static long levenshtein(Nlppp*,long,long);               // 03/06/07 AM.
	static long levenshtein(Nlppp*,long,_TCHAR*);            // 03/06/07 AM.
	static long levenshtein(Nlppp*,_TCHAR*,long);            // 03/06/07 AM.
	static long levenshtein(Nlppp*,long,RFASem*);            // 03/06/07 AM.
	static long levenshtein(Nlppp*,RFASem*,long);            // 03/06/07 AM.

	// User-defined function handling.									// 03/11/02 AM.

	static void *fnstart(Nlppp*,void*&,void*&);	// 03/11/02 AM. // 01/08/07 AM.

	static RFASem *ret(Nlppp*,void*,void*,long long);						// 03/11/02 AM.
	static RFASem *ret(Nlppp*,void*,void*,float);					// 03/11/02 AM.
	static RFASem *ret(Nlppp*,void*,void*,_TCHAR*);					// 03/11/02 AM.
	static RFASem *ret(Nlppp*,void*,void*,RFASem*);					// 03/11/02 AM.

	static long excepth(Nlppp*,void*,void*,int);                // 03/09/14 AM.

	static RFASem *coutreset(Nlppp*,_TCHAR*);							// 05/12/02 AM.
	static RFASem *coutreset(Nlppp*,RFASem*);							// 05/12/02 AM.

	static RFASem *cbuf(Nlppp*);											// 05/12/02 AM.

	static RFASem *pnvarnames(Nlppp*,NODE*);							// 05/13/02 AM.
	static RFASem *pnvarnames(Nlppp*,RFASem*);						// 05/13/02 AM.
	static RFASem *pnvartype(Nlppp*,NODE*,_TCHAR*);

	static bool pnsetfired(Nlppp*,NODE*,bool);

	static bool pnmakevar(Nlppp*,NODE*,_TCHAR*,_TCHAR*);				// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,NODE*,_TCHAR*,long long);					// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,NODE*,_TCHAR*,RFASem*);				// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);				// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,RFASem*,_TCHAR*,long long);				// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,RFASem*,_TCHAR*,RFASem*);			// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,NODE*,RFASem*,_TCHAR*);				// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,NODE*,RFASem*,long long);				// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,NODE*,RFASem*,RFASem*);			// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,RFASem*,RFASem*,_TCHAR*);			// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,RFASem*,RFASem*,long long);			// 07/03/02 AM.
	static bool pnmakevar(Nlppp*,RFASem*,RFASem*,RFASem*);		// 07/03/02 AM.

	static bool pnreplaceval(Nlppp*,NODE*,_TCHAR*,_TCHAR*);			// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,NODE*,_TCHAR*,long long);				// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,NODE*,_TCHAR*,float);          // 12/15/14 AM.
	static bool pnreplaceval(Nlppp*,NODE*,_TCHAR*,RFASem*);			// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);			// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,_TCHAR*,long long);			// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,_TCHAR*,float);        // 12/15/14 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,_TCHAR*,RFASem*);		// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,NODE*,RFASem*,_TCHAR*);			// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,NODE*,RFASem*,long long);			// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,NODE*,RFASem*,float);       // 12/15/14 AM.
	static bool pnreplaceval(Nlppp*,NODE*,RFASem*,RFASem*);		// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,RFASem*,_TCHAR*);		// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,RFASem*,long long);		// 07/03/02 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,RFASem*,float);     // 12/15/14 AM.
	static bool pnreplaceval(Nlppp*,RFASem*,RFASem*,RFASem*);	// 07/03/02 AM.

	static bool pnpushval(Nlppp*,NODE*,_TCHAR*,_TCHAR*);   // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,_TCHAR*,long long);      // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,_TCHAR*,float);      // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,_TCHAR*,RFASem*);   // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*); // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,_TCHAR*,long long);    // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,_TCHAR*,float);    // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,_TCHAR*,RFASem*); // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,RFASem*,_TCHAR*);   // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,RFASem*,long long);      // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,RFASem*,float);      // 12/12/14 AM.
	static bool pnpushval(Nlppp*,NODE*,RFASem*,RFASem*);   // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,RFASem*,_TCHAR*); // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,RFASem*,long long);    // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,RFASem*,float);    // 12/12/14 AM.
	static bool pnpushval(Nlppp*,RFASem*,RFASem*,RFASem*); // 12/12/14 AM.

	static bool pnrpushval(Nlppp*,NODE*,_TCHAR*,_TCHAR*);   // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,_TCHAR*,long long);      // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,_TCHAR*,float);      // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,_TCHAR*,RFASem*);   // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,_TCHAR*,_TCHAR*); // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,_TCHAR*,long long);    // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,_TCHAR*,float);    // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,_TCHAR*,RFASem*); // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,RFASem*,_TCHAR*);   // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,RFASem*,long long);      // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,RFASem*,float);      // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,NODE*,RFASem*,RFASem*);   // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,RFASem*,_TCHAR*); // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,RFASem*,long long);    // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,RFASem*,float);    // 12/12/14 AM.
	static bool pnrpushval(Nlppp*,RFASem*,RFASem*,RFASem*); // 12/12/14 AM.

	static RFASem *pninsert(Nlppp*,_TCHAR*,NODE*,long);         // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,RFASem*,NODE*,long);         // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,_TCHAR*,NODE*,RFASem*);      // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,RFASem*,NODE*,RFASem*);      // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,_TCHAR*,RFASem*,long);       // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,RFASem*,RFASem*,long);       // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,_TCHAR*,RFASem*,RFASem*);    // 07/14/08 AM.
	static RFASem *pninsert(Nlppp*,RFASem*,RFASem*,RFASem*);    // 07/14/08 AM.

	static RFASem *pnmove(Nlppp*,NODE*,NODE*,long);           // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,RFASem*,NODE*,long);         // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,NODE*,NODE*,RFASem*);        // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,RFASem*,NODE*,RFASem*);      // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,NODE*,RFASem*,long);         // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,RFASem*,RFASem*,long);       // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,NODE*,RFASem*,RFASem*);      // 10/06/13 AM.
	static RFASem *pnmove(Nlppp*,RFASem*,RFASem*,RFASem*);    // 10/06/13 AM.

	static bool dbopen(Nlppp*,_TCHAR*,_TCHAR*,_TCHAR*);					// 06/15/02 AM.
	static bool dbopen(Nlppp*,RFASem*,_TCHAR*,_TCHAR*);					// 06/15/02 AM.
	static bool dbopen(Nlppp*,_TCHAR*,RFASem*,_TCHAR*);					// 06/15/02 AM.
	static bool dbopen(Nlppp*,_TCHAR*,_TCHAR*,RFASem*);					// 06/15/02 AM.
	static bool dbopen(Nlppp*,RFASem*,RFASem*,_TCHAR*);				// 06/15/02 AM.
	static bool dbopen(Nlppp*,RFASem*,_TCHAR*,RFASem*);				// 06/15/02 AM.
	static bool dbopen(Nlppp*,_TCHAR*,RFASem*,RFASem*);				// 06/15/02 AM.
	static bool dbopen(Nlppp*,RFASem*,RFASem*,RFASem*);			// 06/15/02 AM.

	static bool dbclose(Nlppp*);											// 06/15/02 AM.
	static bool dballocstmt(Nlppp*);										// 06/15/02 AM.
	static bool dbfreestmt(Nlppp*);										// 06/15/02 AM.
	static bool dbfetch(Nlppp*);											// 06/15/02 AM.

	static bool dbexecstmt(Nlppp*,_TCHAR*);								// 06/15/02 AM.
	static bool dbexecstmt(Nlppp*,RFASem*);							// 06/15/02 AM.

	static bool dbexec(Nlppp*,_TCHAR*);									// 06/15/02 AM.
	static bool dbexec(Nlppp*,RFASem*);									// 06/15/02 AM.

	// (First call-by-reference).											// 06/15/02 AM.
	static bool dbbindcol_int(Nlppp*,long,_TCHAR*,long,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,long,_TCHAR*,long,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,RFASem*,_TCHAR*,long,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,long,RFASem*,long,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,long,_TCHAR*,RFASem*,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,RFASem*,RFASem*,long,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,RFASem*,_TCHAR*,RFASem*,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,long,RFASem*,RFASem*,RFASem*,RFASem*);
	static bool dbbindcol(Nlppp*,RFASem*,RFASem*,RFASem*,RFASem*,RFASem*);

	static bool exittopopup(Nlppp*,_TCHAR*,_TCHAR*);						// 06/15/02 AM.
	static bool exittopopup(Nlppp*,RFASem*,_TCHAR*);					// 06/15/02 AM.
	static bool exittopopup(Nlppp*,_TCHAR*,RFASem*);					// 06/15/02 AM.
	static bool exittopopup(Nlppp*,RFASem*,RFASem*);				// 06/15/02 AM.

	static _TCHAR *getpopupdata(Nlppp*);									// 06/15/02 AM.

	static _TCHAR *stem(Nlppp*,_TCHAR*);										// 08/04/02 AM.
	static _TCHAR *stem(Nlppp*,RFASem*);									// 08/04/02 AM.
	static _TCHAR *stem(Nlppp*,long long);                             // 07/11/03 AM.

	// Note: not all variants implemented...							// 12/14/02 AM.
	// ... because an upgrade to eliminate variants is planned.	// 12/14/02 AM.
	static RFASem *sortconsbyattr(										// 12/14/02 AM.
									Nlppp*,RFASem*,_TCHAR*,long,long);		// 12/14/02 AM.
	static RFASem *sortconsbyattr(										// 12/14/02 AM.
									Nlppp*,RFASem*,RFASem*,long,long);	// 12/14/02 AM.

	// Get substring of the input text.									// 12/15/02 AM.
	static _TCHAR *inputrange(Nlppp*,long,long);						// 12/15/02 AM.
	static _TCHAR *inputrange(Nlppp*,RFASem*,long);					// 12/15/02 AM.
	static _TCHAR *inputrange(Nlppp*,long,RFASem*);					// 12/15/02 AM.
	static _TCHAR *inputrange(Nlppp*,RFASem*,RFASem*);				// 12/15/02 AM.

	static _TCHAR *xmlstr(Nlppp*,_TCHAR*);									// 07/22/03 AM.
	static _TCHAR *xmlstr(Nlppp*,RFASem*);								// 07/22/03 AM.
	static _TCHAR *xmlstr(Nlppp*,long long);                           // 07/22/03 AM.

	static _TCHAR *sqlstr(Nlppp*,_TCHAR*);									// 05/10/03 AM.
	static _TCHAR *sqlstr(Nlppp*,RFASem*);								// 05/10/03 AM.
	static _TCHAR *sqlstr(Nlppp*,long long);                           // 07/11/03 AM.

	static RFASem *dictfirst(Nlppp*);									// 05/29/03 AM.
	static RFASem *dictnext(Nlppp*,RFASem*);							// 05/29/03 AM.

	static bool findana(Nlppp*,_TCHAR*);									// 05/29/03 AM.
	static bool findana(Nlppp*,RFASem*);								// 05/29/03 AM.
	static bool findana(Nlppp*,long long);                           // 07/11/03 AM.

	static bool inputrangetofile(Nlppp*,long,long,std::_t_ostream*);		// 05/29/03 AM.
	static bool inputrangetofile(Nlppp*,long,long,RFASem*);
	static bool inputrangetofile(Nlppp*,long,RFASem*,std::_t_ostream*);
	static bool inputrangetofile(Nlppp*,long,RFASem*,RFASem*);
	static bool inputrangetofile(Nlppp*,RFASem*,long,std::_t_ostream*);
	static bool inputrangetofile(Nlppp*,RFASem*,long,RFASem*);
	static bool inputrangetofile(Nlppp*,RFASem*,RFASem*,std::_t_ostream*);
	static bool inputrangetofile(Nlppp*,RFASem*,RFASem*,RFASem*);

	static _TCHAR *resolveurl(Nlppp*,_TCHAR*,_TCHAR*);						// 05/29/03 AM.
	static _TCHAR *resolveurl(Nlppp*,_TCHAR*,RFASem*);					// 05/29/03 AM.
	static _TCHAR *resolveurl(Nlppp*,RFASem*,_TCHAR*);					// 05/29/03 AM.
	static _TCHAR *resolveurl(Nlppp*,RFASem*,RFASem*);				// 05/29/03 AM.
	static _TCHAR *resolveurl(Nlppp*,long,long);                  // 07/11/03 AM.
	static _TCHAR *resolveurl(Nlppp*,long,_TCHAR*);                 // 07/11/03 AM.
	static _TCHAR *resolveurl(Nlppp*,_TCHAR*,long);                 // 07/11/03 AM.
	static _TCHAR *resolveurl(Nlppp*,long,RFASem*);               // 07/11/03 AM.
	static _TCHAR *resolveurl(Nlppp*,RFASem*,long);               // 07/11/03 AM.

	static bool mkdir(Nlppp*,_TCHAR*);										// 06/11/03 AM.
	static bool mkdir(Nlppp*,RFASem*);									// 06/11/03 AM.
	static bool mkdir(Nlppp*,long long);                             // 07/11/03 AM.

	static RFASem *split(Nlppp*,_TCHAR*,_TCHAR*);                   // 08/14/03 AM.
	static RFASem *split(Nlppp*,_TCHAR*,RFASem*);                 // 08/14/03 AM.
	static RFASem *split(Nlppp*,RFASem*,_TCHAR*);                 // 08/14/03 AM.
	static RFASem *split(Nlppp*,RFASem*,RFASem*);               // 08/14/03 AM.
	static RFASem *split(Nlppp*,long,long);                     // 08/14/03 AM.
	static RFASem *split(Nlppp*,long,_TCHAR*);                    // 08/14/03 AM.
	static RFASem *split(Nlppp*,_TCHAR*,long);                    // 08/14/03 AM.
	static RFASem *split(Nlppp*,long,RFASem*);                  // 08/14/03 AM.
	static RFASem *split(Nlppp*,RFASem*,long);                  // 08/14/03 AM.

	static _TCHAR *deaccent(Nlppp*,_TCHAR*);								// 09/07/03 AM.
	static _TCHAR *deaccent(Nlppp*,RFASem*);								// 09/07/03 AM.
	static _TCHAR *deaccent(Nlppp*,long);                         // 09/07/03 AM.

	static _TCHAR *dejunk(Nlppp*,_TCHAR*);   // 09/09/11 AM.
	static _TCHAR *dejunk(Nlppp*,RFASem*);   // 09/09/11 AM.
	static _TCHAR *dejunk(Nlppp*,long);      // 09/09/11 AM.

	static bool lextagger(Nlppp*);                              // 12/14/03 AM.

	static bool setlookahead(Nlppp*,RFASem*);                   // 12/14/03 AM.
	static bool setlookahead(Nlppp*,long);                      // 12/14/03 AM.

	static RFASem *permuten(Nlppp*,RFASem*);							// 03/10/04 AM.
	static RFASem *permuten(Nlppp*,long);								// 03/10/04 AM.

   static float logten(Nlppp*,RFASem*);                        // 04/29/04 AM.
   static float logten(Nlppp*,float);                          // 04/29/04 AM.


	static _TCHAR *urltofile(Nlppp*,_TCHAR*,_TCHAR*); // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,RFASem*,_TCHAR*); // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,_TCHAR*,RFASem*); // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,RFASem*,RFASem*); // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,RFASem*,long);    // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,long,RFASem*);    // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,_TCHAR*,long);    // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,long,_TCHAR*);    // 05/14/14 AM.
	static _TCHAR *urltofile(Nlppp*,long,long);       // 05/14/14 AM.


	static _TCHAR *urlbase(Nlppp*,_TCHAR*);   // 05/20/14 AM.
	static _TCHAR *urlbase(Nlppp*,RFASem*);   // 05/20/14 AM.
	static _TCHAR *urlbase(Nlppp*,long long);      // 05/20/14 AM.

	////
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,_TCHAR*,_TCHAR*,long);      // 12/16/14 AM.
	
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,_TCHAR*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,_TCHAR*,_TCHAR*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,RFASem*,_TCHAR*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,_TCHAR*,RFASem*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,_TCHAR*,_TCHAR*,RFASem*);   // 12/16/14 AM.

	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,_TCHAR*,_TCHAR*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,RFASem*,_TCHAR*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,RFASem*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,_TCHAR*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,RFASem*,_TCHAR*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,_TCHAR*,RFASem*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,_TCHAR*,_TCHAR*,RFASem*);   // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,RFASem*,RFASem*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,RFASem*,_TCHAR*,RFASem*);   // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,_TCHAR*,RFASem*,RFASem*);   // 12/16/14 AM.

	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,RFASem*,_TCHAR*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,_TCHAR*,RFASem*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,_TCHAR*,_TCHAR*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,RFASem*,RFASem*,long);    // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,RFASem*,_TCHAR*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,_TCHAR*,RFASem*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,RFASem*,RFASem*,long);      // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,RFASem*,_TCHAR*,RFASem*);   // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,_TCHAR*,RFASem*,RFASem*);   // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,NODE*,_TCHAR*,RFASem*,RFASem*,RFASem*);   // 12/16/14 AM.

	static bool DICTphraselookup(Nlppp*,NODE*,RFASem*,RFASem*,RFASem*,RFASem*);   // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,_TCHAR*,RFASem*,RFASem*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,_TCHAR*,RFASem*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,RFASem*,_TCHAR*,RFASem*); // 12/16/14 AM.
	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,RFASem*,RFASem*,long);    // 12/16/14 AM.

	static bool DICTphraselookup(Nlppp*,RFASem*,RFASem*,RFASem*,RFASem*,RFASem*); // 12/16/14 AM.
	////


	// Utilities.
	static _TCHAR *sem_to_str(RFASem*);									// 04/28/01 AM.
	static std::_t_ostream *sem_to_ostream(RFASem*);							// 05/04/03 AM.

	static void *sort_arr_kbcon_by_attr(								// 12/10/02 AM.
		void *args,			// Array of kbconcepts.
		_TCHAR *attr,			// Name of concept attribute.
		bool numeric,		// True if numeric, false if string.
		bool descending,	// True if descending, false if ascending.
		CG *cg
		);


};		// End of Arun class.



#endif
/*************************      END OF FILE      ******************************/
