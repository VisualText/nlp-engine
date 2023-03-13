/*******************************************************************************
Copyright (c) 2008-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CMLTOK.H
* FILE:	lite\cmltok.h
* CR:		08/17/08 AM.
* SUBJ:	Class for tokenization and other processing of CML input files.
* NOTE:	Copied from tok.h.
*			CML = Clix Markup Language, copyright May Information Technologies.
*
*******************************************************************************/

#ifndef CMLTOK_H_
#define CMLTOK_H_

#include "pn.h"
//#include "tree.h"
#include "lite/algo.h"
#include "htab.h"

#ifndef MAXCMLLINE
#define MAXCMLLINE 2048
#endif

/********************************************
* CLASS:	CMLTOK
* CR:		08/17/08 AM.
* SUBJ:	CML tokenization class.
* NOTE:	Tokenization converts a buffer into a parse tree.
*			Will get rid of CML text box information and build a SECOND
*			text buffer for the parse, that excludes the text box info.
********************************************/

class CMLTok : public Algo
{
public:
	CMLTok();							// Constructor.
	CMLTok(const CMLTok &orig);	// Copy constructor.

	bool getBad();
	void setBad(bool);

	virtual Algo &dup();
	virtual void setup(_TCHAR *s_data);
	bool Execute(Parse *, Seqn *);	// Perform the tokenization.

	Sym *internTok(
		_TCHAR *str,			// Ptr to string in a buffer.
		long len,				// Length of string within buffer.
		Htab *htab,				// Hashed symbol table.
		/*UP*/ _TCHAR* &lcstr
		);

	///////////////////////////////////////
	// The new stuff...

	bool initCMLtok(Parse *parse);
	bool finCMLtok();

	bool getCMLline();			// Get CML line with header info.
	bool getCMLword();			// Get text from CML line.
	bool getCMLtoken();			// Get token from CML line.
	bool getCMLpagetag();		// Get CML line with HTML tag.
	bool getCMLblank();			// Get blank CML line.

	bool readCMLpagetag(long&,bool&);		// Process CML page tag line.

	inline bool getCMLnum(
		long &field			// CML field to fill.
		);

	bool skipBlanks();

	Node<Pn> *makePagenode(long);	// Build a page node.
	Node<Pn> *makeLinenode();		// Build a line node.
	Node<Pn> *makeWordnode();		// Build a word node.

	bool updatePagenode();			// Update line info.
	bool updateLinenode(bool);		// Update line info.
	bool updateWordnode();			// Update line info.

	inline bool barcodeChar(_TCHAR);				// Collect word's barcode info.

	Node<Pn> *makeToknode(
		long o_start,
		long o_end,
		long ustart,	// [UNICODE]	// 06/15/22 AM.
		long uend,	// [UNICODE]	// 06/15/22 AM.
		enum Pntype typ,
		_TCHAR *text	// Text buffer ptr.
		);

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

private:
	//Parse *parse;			// The analyze instance that we're tokenizing for.
	bool bad_;					// If unknown char seen.

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
	Node<Pn> *page_;			// Current page node.
	Node<Pn> *line_;			// Current line node.
	Node<Pn> *word_;			// Current word node.
	Node<Pn> *tok_;			// Current literal token node.
	
	// CML data per word.
	long Z_;		// Zone number.
	long L1_;		// Line number. (Tracks "output" plain text line number.)
	long W1_;		// First word number.
	long L_;			// Left text box offset.
	long T_;			// Top text box offset.
	long W_;			// Width text box offset.
	long H_;			// Height text box offset.
	long C_;			// Confidence value. (ignoring this one).
	
	long P_;			// Track the CML page number.

	// Track previous zone and line.
	long prevZ_;	// Prior zone number.
	long prevL1_;	// Prior line number.
	
	long lineR_;	// Prior (L_ + W_).
	long gapL_;		// Left of largest gap on line.
	long gapR_;		// Right of largest gap on line.
	
	// Data for word.
	Node<Pn> *firsttok_;	// Track word node's first token.

	// COLLECT DATA.

	// Data for tok.

	// Data for word.
	long wlen_;						// Number of characters in word.
	long wbars_;					// Number of barcode chars in word.
	long walphas_;					// Number of alphabetic chars in word.
	long wnums_;					// Number of digits in word.
	long wpuncts_;					// Number of punctuation chars in line.

	// Data for line.
	long llen_;						// Number of characters in line.
	long lbars_;					// Number of barcode chars in line.
	long lalphas_;					// Number of alphabetic chars in line.
	long lnums_;					// Number of digits in line.
	long lpuncts_;					// Number of punctuation chars in line.
	_TCHAR *lfirst_;				// First in line: "upper","cap","lower","num","punct","ctrl".
	_TCHAR *llast_;				// Last in line: "upper","cap","lower","num","punct","ctrl".
	long llasteos_;				// 1 if last in line is end-of-sentence char.
	long llastcolon_;				// 1 if last in line is a colon.

	long lwords_;					// Number of (CML) words (or "blobs") in line.
	long ltoks_;					// Number of tokens (lexemes) in line.
	long luppers_;					// Number of uppercase TOKS in line.
	long lcaps_;					// Number of cap TOKS in line.
	long llowers_;					// Number of lowercase TOKS in line.
	long lknowns_;					// Number of known words in line.
	long lunks_;					// Number of unknown words in line.

	// Data for page.

	// Miscellaneous.
	_TCHAR buf_[MAXCMLLINE];	// Temporary buffer.
	_TCHAR *ptr_;					// Temporary pointer into buffer.
};


#endif
