
/*******************************************************************************
Copyright (c) 2011 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	DICTTOK.H
* FILE:	lite\tok.h
* CR:		07/29/11 AM.
* SUBJ:	Class for tokenization.
* NOTE:	
*
*******************************************************************************/

#ifndef DICTTOK_H_
#define DICTTOK_H_

#include "pn.h"
//#include "tree.h"
#include "lite/algo.h"
#include "htab.h"
#include "tok.h"

/********************************************
* CLASS:	DICTTOK
* CR:		10/09/98 AM.
* SUBJ:	Tokenization class.
* NOTE:	Tokenization converts a buffer into a parse tree.
*			Since using Parse instance, should get rid of most of what's
*			here!
********************************************/

class DICTTok : public Algo
{
public:
	DICTTok();					// Constructor.
	//DICTTok(Parse *);			// Constructor that performs the tokenization!
	DICTTok(const DICTTok &orig);	// Copy constructor.   // 12/03/98 AM.

	bool getBad();					// 01/15/99 AM.
	void setBad(bool);			// 01/15/99 AM.
	bool getZapwhite();        // 08/16/11 AM.
	void setZapwhite(bool);    // 08/16/11 AM.
	void setCharTok(bool);

	//Parse *getParse();
	//void   setParse(Parse *);

	virtual Algo &dup();					// 12/03/98 AM.
	virtual void setup(_TCHAR *s_data);					// 12/04/98 AM.
	bool Execute(Parse *, Seqn *);	// Perform the tokenization.
	bool Tokenize(Parse *);	// Perform the tokenization.
	bool ApplyDictFiles();
	Node<Pn>* MatchLongest(CONCEPT *con, Node<Pn> *parentN, CONCEPT **end, int &length, int level);
	bool copyAttrsToSuggested(Pn *pn, Node<Pn> *suggestedPn, Node<Pn> *endNode);
	bool checkCase(_TCHAR *name, Node<Pn> *node, bool &lower, bool &cap, bool &upper);
	void FirstToken(Tree<Pn> *tree, Htab *htab, _TCHAR* *buf, const char* s, int32_t length,
										int32_t &start, int32_t &ustart, Node<Pn>* &last,
										long &line	// Line number.		// 05/17/01 AM.
										);

	inline bool NextToken(
			Tree<Pn> *tree,
			Htab *htab,
			_TCHAR* *buf,
			const char* s,
			int32_t length,
			int32_t &start,
			int32_t &ustart,	// [UNICODE]
			Node<Pn>* &last,
			long &line			// Bookkeep line number.				// 05/17/01 AM.
			);

	inline void nextTok(
			const char *s,		// Start char of token.
			int32_t start,		// Start offset of token.
			int32_t &end,		// End offset of token.
			int32_t length,
			enum Pntype &typ,	// Token type.
			bool &lineflag		// Flag new line number.				// 05/17/01 AM.
			);

	inline Sym *internTok(
		_TCHAR *str,				// Ptr to string in a buffer.
		long len,				// Length of string within buffer.
		Htab *htab,				// Hashed symbol table.		// 11/19/98 AM.
		/*UP*/ _TCHAR* &lcstr
		);

	inline Node<Pn> * handleTok(
		Node<Pn> *node,
		Node<Pn> *last,
		enum Pntype typ,
		_TCHAR *str,
		_TCHAR *lcstr,
		Htab *htab
		);

	void findTokAttrs(Node<Pn> *node, Node<Pn> *last, CONCEPT *con, _TCHAR *str, _TCHAR *lcstr);
	void makeTextAttrs(Node<Pn> *node, Node<Pn> *last);

	inline bool findAttrs(
		Node<Pn> *node,
		CONCEPT *con,
		_TCHAR *str,
		bool isSuggested
		);

	///////////////////////////////////////
	// The new stuff...

	bool initTok(Parse *parse);
	bool finTok();
	bool isPunct(UChar32 c);

	inline bool replaceNum(
		Node<Pn> *node,
		_TCHAR *name,	// variable name.
		long long val
		);
	inline bool replaceStr(	// 08/30/08 AM.
		Node<Pn> *node,
		_TCHAR *name,	// variable name.
		_TCHAR *str
		);
	inline bool replaceCon(
		Node<Pn> *node,
		_TCHAR *name,
		CONCEPT *con
		);
	inline _TCHAR *popsval(
		VAL *val
		);

private:
	//Parse *parse;			// The analyze instance that we're tokenizing for.
	bool bad_;					// If unknown char seen.	// 01/15/99 AM.
	CONCEPT *funk;

	// The new stuff...
	
	Parse *parse_;				// Convenience ptr to current parse info.
	Tree<Pn> *tree_;			// The parse tree. (Created in this pass.)
	_TCHAR *text_;				// Start of the input text buffer.
	Htab *htab_;				// Convenience ptr to hash table.

	long topos_;				// "To" position in text buffer.
	long fmpos_;				// "From" position in text buffer.
	_TCHAR *toptr_;			// "To" text buffer pointer.
	_TCHAR *fmptr_;			// "From" text buffer pointer.

	long toupos_;	// [UNICODE]	// 06/15/22 AM.
	long fmupos_;	// [UNICODE]	// 06/15/22 AM.
	
	long lineno_;				// Track line number in input text buffer.

	Node<Pn> *root_;			// Root node of parse tree.
//	Node<Pn> *page_;			// Current page node.
	Node<Pn> *line_;			// Current line node.
//	Node<Pn> *word_;			// Current word node (if collecting "blobs").
	Node<Pn> *tok_;			// Current literal token node.

	Tok *token_;

	// Track if whitespace was previous token. For NOSP attr.	// 08/16/11 AM.
	bool prevwh_;				// If prev token was whitespace.		// 08/16/11 AM.

	// Count lines and tabs preceding a token.	// 08/16/11 AM.
	long lines_;   // 08/16/11 AM.
	long tabs_;    // 08/16/11 AM.

	// DICTOKZ variant.	Same, but ZAP WHITESPACE (don't put
	// whitespace into the parse tree.)          // 08/16/11 AM.
	bool zapwhite_;         // 08/16/11 AM.
	bool separatechars_;

	long totlines_;	// Count lines in text.
	long tottabs_;		// Count tabs in text.
	long totlowers_;	// Count lowercase words.
	long totcaps_;		// Count capitalized words.
	long totuppers_;	// Count uppercase words.
	long totnums_;    // Count numbers.

	//////////////////////////////////////////
	// Miscellaneous.
	_TCHAR buf_[MAXLINE];	// Temporary buffer.
	_TCHAR *ptr_;					// Temporary pointer into buffer.

	// For knowledge base lookup.
	CG *cg_;
};


#endif
