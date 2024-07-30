/*******************************************************************************
Copyright (c) 1999-2009 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	CG.H
* FILE:	include/API/cg.h.
* CR:		04/27/99 AM.
* SUBJ:	API for VisualText KBMS.
*			KBMS = knowledge base management system. A hierarchical data base.
*			CG = Conceptual Grammar. A knowledge representation scheme.
*
*******************************************************************************/

#ifndef CG_H_
#define CG_H_


#ifndef IO_LIBRARIES_
#define IO_LIBRARIES_
#include <stdlib.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
#endif

#include <vector>
#include <filesystem>

#define FNAMESIZ 256
#define NAMESIZ  256
#define MAXPATH  2048
//#define PATHSIZ  512
// SIZES	// 03/25/13 AM.
// Messes up concept display in Attribute Editor.	// 09/14/13 AM.
//#define PATHSIZ  1024
// SIZES. Back to what it was.	// 09/14/13 AM.
#define PATHSIZ 512

#ifndef ID
typedef long ID;
#endif

namespace consh_cg {		// 09/26/19 AM.
// COMPILED KB IN LINUX.	// 02/19/19 AM.
#ifdef LINUX
typedef double HINSTANCE;
#endif
#if MSVC_VERSION >= 1900
	typedef HINSTANCE HINSTANCE;
#else
	typedef HINSTANCE HINSTANCE;
#endif
}

// Readability definitions.
#ifndef CONCEPT
typedef void CONCEPT;				// A concept.
typedef void ATTR;					// An attribute.
typedef void VAL;						// An attribute value.
typedef void PHRASE;					// A concept's phrase of nodes.
#endif
// Note: A phrase NODE is a specialized CONCEPT.

// Forward reference.
//class NLP;
class AKBM;	// 06/11/02 AM.
class ALIST;																	// 08/14/02 AM.
class AST;
class APTR;
class ASYM;
class ACON;

/********************************************
* CLASS:	CG
* CR:		04/28/99 AM.
* SUBJ:	API Class for managing the VisualText KBMS.
********************************************/

class LIBCONSH_API CG
{
	//friend ostream &operator<<(ostream &, Var &);

public:
	CG(										// Default constructor.
		_TCHAR *appdir = 0,
		bool compiled = false,			// Load compiled (kb.dll).	// 04/27/01 AM.
		ALIST *alist = 0			// List Manager (not optional).	// 08/14/02 AM.
		);
	~CG();									// Destructor.

	// Access
	_TCHAR *getAppdir();
	bool  getDirty();															// 05/12/00 AM.
	AKBM  *getKBM();															// 06/11/02 AM.
	ALIST *getALIST();														// 08/14/02 AM.
//#ifndef LINUX
//	HINSTANCE getHkbdll();	// 06/29/00 AM.
	consh_cg::HINSTANCE getHkbdll();	// 06/29/00 AM. // 09/26/19 AM.
//#endif

	// Modify
	void setAppdir(_TCHAR *);
	void setDirty(bool);														// 05/12/00 AM.
	void setKBM(AKBM*);														// 06/11/02 AM.
	void setALIST(ALIST*);													// 08/14/02 AM.
//#ifndef LINUX
//	void setHkbdll(HINSTANCE);	// 06/29/00 AM.
	void setHkbdll(consh_cg::HINSTANCE);	// 06/29/00 AM.	// 09/26/19 AM.
//#endif

	// General.
   static CG *makeCG(                                          // 07/28/03 AM.
	   ALIST *alist,
      _TCHAR *appdir=0,
		bool compiled = false
		);
   static void deleteCG(CG*);                                  // 07/28/03 AM.

	/////////////////////
	// OVERALL KB FUNS
	/////////////////////

	//	Read the "factory-default" KB from files.
	// Equivalent to readKB("sys");
	bool readSystemKB();

	// Read the latest (user-modified) kb from files.
	// If dir is present, kb is read from $appdir/kb/dir.
	bool readKB(_TCHAR *dir);	// _T("user")	// 09/23/20 AM.

	// Save latest KB (from memory to files).
	// If dir is present, kb is written to $appdir/kb/dir.
//	bool writeKB(_TCHAR *dir = _T("user"));
	bool writeKB(_TCHAR *dir);	// _T("user")	// 09/23/20 AM.

	// Modular dump of a kb subtree.										// 08/06/01 AM.
	bool dumpTree(CONCEPT*, _TCHAR*);										// 08/06/01 AM.

	// Generate code for the KB tables.	(KB.DLL)						// 06/28/00 AM.
	bool genKB();																// 06/28/00 AM.

	// Compile the KB.DLL library.										// 09/13/00 AM.
	bool compileKB(bool=false);											// 09/13/00 AM.

	// Primitive load, unload of kb.dll.
	bool loadLib();															// 09/13/00 AM.
	bool unloadLib();															// 09/13/00 AM.

	//////////////////////
	// FIND THINGS
	//////////////////////

	// Find the root concept of hierarchy.
	CONCEPT *findRoot();

	// Find the named concept under given parent.
	// Wend your way down from "concept", the root concept.
	CONCEPT *findConcept(CONCEPT *parent, _TCHAR *name);

	// Find the nth concept under parent.
	CONCEPT *findConcept(CONCEPT *parent, int pos);

	// Find concept's attribute.											// 07/12/99 AM.
	ATTR *findAttr(CONCEPT *con, _TCHAR *name);

	// Fetch the list of attributes for the concept.
	ATTR *findAttrs(CONCEPT *con);

	int attrValType(CONCEPT *con, _TCHAR *attr);

	// Fetch attribute name into given buffer.						// 07/05/99 AM.
	bool attrName(ATTR *attr, _TCHAR *buf, int bufsiz = NAMESIZ);

	// Fetch attribute values.												// 07/05/99 AM.
	VAL *attrVals(ATTR *attr);

	// Fetch the list of values of concept's attribute.
	VAL *findVals(CONCEPT *con, _TCHAR *attr);

	// Fetch the first value of given attribute of a concept.
	// Treat as appropriate type if possible.
	bool findVal(CONCEPT *con, _TCHAR *attr, /*UP*/ _TCHAR *valbuf);
	bool findVal(CONCEPT *con, _TCHAR *attr, /*UP*/ long long &val);
	bool findVal(CONCEPT *con, _TCHAR *attr, /*UP*/ float &val);	// 08/16/01 AM.
	bool findVal(CONCEPT *con,_TCHAR *attr,/*UP*/CONCEPT* &val);	// 08/12/00 AM.
	_TCHAR *strVal(CONCEPT *conc, _TCHAR *s_attr);						// 11/12/00 AM.
	bool isValCon(VAL* &vals);						// 08/11/21 Dd.
	bool isValStr(VAL* &vals);						// 08/11/21 Dd.
	bool isValNum(VAL* &vals);						// 08/11/21 Dd.

	// Look for an attribute up the hierarchy. (INHERIT).
	// hier is the top of the concept hierarchy to be searched.
	// hier = 0 means search up to root of the entire tree.
	bool inheritVal(CONCEPT *con, _TCHAR *attr, CONCEPT *hier,
															/*UP*/ _TCHAR *valbuf);

	// Get concept name.
	bool conceptName(CONCEPT *con, /*UP*/ _TCHAR *name,
		long bufsiz = 256);													// 06/18/02 AM.

	// Get concept path.														// 07/21/99 AM.
	bool conceptPath(CONCEPT *conc, /*UP*/ _TCHAR *buf, int bufsize = PATHSIZ);

	// Get concept path for a string.									// 03/06/00 AM.
	bool wordPath(_TCHAR *str, /*UP*/ _TCHAR *buf, int bufsize = PATHSIZ);

	// Find concept path for a string.									// 05/30/00 AM.
	bool findWordpath(_TCHAR *str, /*DU*/ _TCHAR *buf, int bufsize = PATHSIZ);

	// Get dictionary index concept for a word.						// 05/31/00 AM.
	CONCEPT *wordIndex(_TCHAR *str);										// 05/31/00 AM.

	// Find dictionary concept for given string.						// 06/29/03 AM.
	CONCEPT *findWordConcept(_TCHAR*);								// 06/29/03 AM.

	CONCEPT *matchDictKB(std::string dictFilename, std::vector<std::filesystem::path> kbfiles);
	std::string removeExtension(const std::string& filename);
	bool openDict(std::vector<std::filesystem::path>& files);
	bool readDicts(std::vector<std::filesystem::path> files, std::vector<std::filesystem::path> kbfiles);
	bool readDict(std::string file, std::vector<std::filesystem::path> kbfiles);

	bool openKBB(std::vector<std::filesystem::path>& files);
	bool readKBBs(std::vector<std::filesystem::path> files);
	bool readKBB(std::string file);
	CONCEPT *addConceptByPath(const char *line, std::vector<std::pair<int,int>> conIndices);

	void outputTime(_TCHAR *timeMsg, clock_t s_time);

	CONCEPT *findDictConcept(_TCHAR*);

	// Get (find else add) dict concept for given string.			// 06/29/03 AM.
	CONCEPT *getWordConcept(_TCHAR *str);

	// Get concept for a path.												// 04/25/00 AM.
	CONCEPT *pathConcept(_TCHAR *path);									// 04/25/00 AM.

	// Get first alphabetic dict concept. For traversal.			// 03/21/03 AM.
	CONCEPT *dictFirst();													// 03/21/03 AM.

	// Get next alphabetic dict concept.  For traversal.			// 03/21/03 AM.
	CONCEPT *dictNext(CONCEPT *dictcon);								// 03/21/03 AM.

	// Find hierarchy concept.  Given a name, look through the given subtree
	// for it.  Also look through node names.  If a node, return its owning
	// concept.  If tree == 0, start at root of concept tree.
	CONCEPT *findHierConcept(_TCHAR *name, CONCEPT *tree = 0);

	// Sort subhierarchy alphabetically.                        // 02/20/09 AM.
	bool sortHier(CONCEPT *tree = 0);                           // 02/20/09 AM.

	// See if the given value is one of attr's values.				// 05/02/00 AM.
	bool attrWithval(CONCEPT *conc, _TCHAR *attr, _TCHAR *val);		// 05/02/00 AM.

	// Search for given attr and value in given subtree.
	bool attrExists(CONCEPT *tree, _TCHAR *attr, _TCHAR *val);

	// For internal use.
	bool attrExistsrec(CONCEPT *tree, bool root, _TCHAR *attr, _TCHAR *val);

	// Search for given node attr and value in given subtree.
	bool nodeAttrexists(CONCEPT *tree, _TCHAR *attr, _TCHAR *val);

	// For internal use.
	bool nodeAttrexistsrec(CONCEPT *tree, bool root, _TCHAR *attr, _TCHAR *val);

	// Change all values of attribute to the new value, in a subtree.
	void attrChange(CONCEPT *tree, _TCHAR *attr,_TCHAR *val,_TCHAR *newval);
	void attrChangerec(
		CONCEPT *tree,
		bool root,
		_TCHAR *attr,
		_TCHAR *val,
		_TCHAR *newval
		);

	//////////////////////
	// TRAVERSE THINGS
	//////////////////////

	// Get a concept attached to the given concept.
	CONCEPT *Down(CONCEPT *con);
	CONCEPT *Up(CONCEPT *con);
	CONCEPT *Next(CONCEPT *con);
	CONCEPT *Prev(CONCEPT *con);

	// Traverse a list of attributes.
	ATTR *popAttr(ATTR* &attrs);

	// Traverse a list of values.											// 03/02/00 AM.
	VAL *nextVal(VAL *val);													// 03/02/00 AM.

	// Pop attribute value as a string.									// 07/09/99 AM.
	void popSval(VAL *&vals, /*UP*/ _TCHAR *buf);

	// Pop the next value from a list of attribute values.
	void popVal(VAL* &vals, /*UP*/ _TCHAR *val);
	void popVal(VAL* &vals, /*UP*/ long long &val);
	void popVal(VAL* &vals, /*UP*/ float &val);						// 08/16/01 AM.
	void popVal(VAL* &vals, /*UP*/ CONCEPT* &val);					// 08/12/00 AM.

	//////////////////////
	// MAKE THINGS
	//////////////////////

	// Make named concept under given parent.
	// Concept is placed at end of list of children.
	CONCEPT *makeConcept(CONCEPT *parent, _TCHAR *name);

	// Make named concept at given position under parent.
	// pos = 0 means at end of list.
	CONCEPT *makeConcept(CONCEPT *parent, _TCHAR *name, int pos);

	// Add concept to nth position in parent's children.
	// pos == 0 means at end of list.
	CONCEPT *addConcept(CONCEPT *parent, CONCEPT *conc, int pos);

	// Add an empty attribute. (For an editor.)						// 07/12/99 AM.
	ATTR *addAttr(CONCEPT *con, _TCHAR *attr);

	// Add value as a string type.										// 07/09/99 AM.
	bool addSval(CONCEPT *con, _TCHAR *attr, _TCHAR *buf);	// supplied buffer.
	bool addSval(CONCEPT *con, _TCHAR *attr, long long val);

	// Add string value at end of attribute's values.
	bool addVal(CONCEPT *con, _TCHAR *attr, _TCHAR *val);

	// Add num value at end of attribute's values.
	bool addVal(CONCEPT *con, _TCHAR *attr, long long val);
	bool addValUnique(CONCEPT *con, _TCHAR *attr, long long val);
	bool addVal(CONCEPT *con, _TCHAR *attr, float val);				// 03/19/03 AM.

	// Add CONCEPT values at end of attr's values.					// 08/11/00 AM.
	bool addVal(CONCEPT *conc, _TCHAR *attr, CONCEPT *val);			// 08/11/00 AM.

	// Add word to the dictionary.										// 05/09/00 AM.
	CONCEPT *addWord(_TCHAR *str);											// 05/09/00 AM.

	// Create (or "register") an attribute.
	// Note: YOU control how many values the attribute will get, their
	// ordering and whether they are numeric, string, or mixed values.
	// (Kind of like LISP. Lots of rope for hanging yourself ;-)
	// Attribute name must be created before used.
	//bool createAttr(char *name);

	//////////////////////
	// FIND OR MAKE THINGS
	//////////////////////

	// Find or make concept under given parent.
	CONCEPT *getConcept(CONCEPT *parent, _TCHAR *name);

	//////////////////////
	// DELETE THINGS
	//////////////////////

	// For compatibility with other kbms implementations.	// 02/11/07 AM.
	bool kbfree(void *);	// 02/11/07 AM.

	// Delete the given concept.
	bool rmConcept(CONCEPT *con);

	// Delete the named concept from its parent.
	bool rmConcept(CONCEPT *parent, _TCHAR *name);

	// Delete nth concept from its parent.
	bool rmConcept(CONCEPT *parent, int pos);

	// Delete the given subtree.
	//bool rmTree(CONCEPT *con);
	//bool rmTree(CONCEPT *parent, char *name);

	// Delete nth subtree from its parent.
	//bool rmTree(CONCEPT *parent, int pos);

	// Delete the normal attributes of a concept.					// 10/31/00 AM.
	bool rmAttrs(CONCEPT *con);											// 10/31/00 AM.

	// Delete values of concept's attribute.
	// Renamed from rmVal.													// 05/11/00 AM.
	bool rmVals(CONCEPT *con, _TCHAR *attr);

	// Delete one value from its attribute.							// 05/11/00 AM.
	bool rmVal(ATTR *attr, VAL *val);

	// Delete named value from attribute.								// 05/17/00 AM.
	bool rmAttrval(CONCEPT *con, _TCHAR *attr_s, _TCHAR *val_s);		// 05/17/00 AM.
	bool rmAttrval(CONCEPT *con, _TCHAR *attr_s, long long val);			// 05/19/00 AM.
	bool rmAttrval(CONCEPT *con, _TCHAR *attr_s, float val);		// 03/19/03 AM.
	bool rmAttrval(CONCEPT *con, _TCHAR *attr_s, CONCEPT *val);		// 06/03/02 Dd.

	// Delete an attribute.													// 07/12/99 AM.
	bool rmAttr(CONCEPT *con, _TCHAR *attr);
	bool rmAttr(CONCEPT *con, ATTR *attr);

	// Remove all the subtree below given concept, including the
	// phrase under the concept.											// 08/10/99 AM.
	// If given a node (ie sample), does nothing.
	bool rmChildren(CONCEPT *con);

	// Remove all the phrases (ie samples) from the given subtree.
	// If given a node (ie sample), does nothing.
	bool prunePhrases(CONCEPT *tree);									// 08/10/99 AM.

	// Remove a word from the knowledge base.							// 07/17/00 AM.
	bool rmWord(_TCHAR *str);													// 07/17/00 AM.

	//////////////////////
	// REPLACE THINGS
	//////////////////////

	// Replace attribute value with given value.
	// Note: not comparing types.
	bool replaceVal(CONCEPT *con, _TCHAR *attr, _TCHAR *val);
	bool replaceVal(CONCEPT *con, _TCHAR *attr, long long val);
	bool replaceVal(CONCEPT *con, _TCHAR *attr, float val);			// 03/19/03 AM.
	bool replaceVal(CONCEPT *con, _TCHAR *attr, CONCEPT *val);		// 08/12/00 AM.

	// Replace in multi-valued attrs.									// 05/19/00 AM.
	bool replaceVal(CONCEPT *con, _TCHAR *attr, _TCHAR *oldval, _TCHAR *newval);
	bool replaceVal(CONCEPT *con, _TCHAR *attr, long long oldval, long long newval);
	bool replaceVal(CONCEPT*, _TCHAR*attr, float oldval, float newval);
																					// 03/19/03 AM.
	bool replaceVal(CONCEPT *con, _TCHAR *attr, CONCEPT *oldval, CONCEPT *newval);

	// Rename a concept.
	bool renameConcept(CONCEPT *con, _TCHAR *name);
	bool renameConcept(CONCEPT *parent, int pos, _TCHAR *name);

	// Rename an attribute.													// 07/12/99 AM.
	bool renameAttr(CONCEPT *con, _TCHAR *attr, _TCHAR *rename);
	bool renameAttr(ATTR *attr, _TCHAR *rename);

	//////////////////////
	// MOVE THINGS
	//////////////////////

	// "Bubble" concept left or right.
	bool moveCleft(CONCEPT *con);
	bool moveCright(CONCEPT *con);
	CONCEPT *makeParentConcept(_TCHAR *name, CONCEPT *conStart, CONCEPT *conEnd);
	void adoptConcepts(CONCEPT *conParent, CONCEPT *conStart, CONCEPT *conEnd);

	// Sort concept's phrase.	[NEW!]
	bool sortChilds(CONCEPT *conc);           // 02/20/09 AM.

	//////////////////////
	// PHRASE MGMT
	//////////////////////

	// Find concept's phrase.
	PHRASE *findPhrase(CONCEPT *conc);

	// Sort concept's phrase.	[NEW!]
	bool sortPhrase(CONCEPT *conc);

	// Get length of phrase.
	long phraseLength(CONCEPT *conc);

	// Get concept that node is a proxy for.							// 12/03/02 AM.
	// NOTE: Changed functionality of nodeConcept.					// 12/03/02 AM.
	CONCEPT *nodeConcept(CONCEPT *node);

	// Find node's owning concept.										// 08/27/99 AM.
	// NOTE: Take's over functionality from nodeConcept.			// 12/03/02 AM.
	CONCEPT *nodeOwner(CONCEPT *node);									// 12/03/02 AM.

	// Find nth phrase node.  pos = 0 means find last node.
	CONCEPT *findNode(PHRASE  *phr,  int pos);

	// Find first node that matches.
	CONCEPT *findNode(PHRASE *phr, _TCHAR    *name);
	//CONCEPT *findNode(PHRASE *phr, CONCEPT *ref);

	// Given a node, find first node in list.							// 08/31/99 AM.
	CONCEPT *listNode(CONCEPT *node);

	// Conveniences. Get first and last node in phrase.
	CONCEPT *firstNode(PHRASE  *phr);
	CONCEPT *lastNode( PHRASE  *phr);

	// Create a phrase under concept by adding first node.
	PHRASE *makePhrase(CONCEPT *con, _TCHAR *name);

	// NEW!  Sorry, has to be named differently (everything is really void).
	// Given a concept, add a new named node to end of its phrase.
	CONCEPT *addCnode(CONCEPT *con, _TCHAR *name);
	CONCEPT *addCnode(CONCEPT *con, CONCEPT *ncon);	// 11/27/02 AM.

	// Add a node to a concept's phrase. Create phrase as needed.
	// Phrase can be passed down, or 0 if empty or not known.
	// Phrase will be passed back up by the function.
	// New node is returned by the function.
	CONCEPT *addNode(CONCEPT *con, _TCHAR *name, /*DU*/ PHRASE* &phr);

	// Add node to phrase. pos == 0, the default, means add to end
	// of phrase.
	CONCEPT *addNode(PHRASE *phr, _TCHAR    *name, int pos = 0);
	CONCEPT *addNode(PHRASE *phr, CONCEPT *node, int pos = 0);

	// Add node before n_pos.  If n_pos == 0, add at end of phrase.
	// RENAMED. 02/13/07 AM.
	//CONCEPT *addNode(PHRASE *phr, CONCEPT *node, CONCEPT *n_pos);
	CONCEPT *addNode_c(PHRASE *phr, CONCEPT *node, CONCEPT *n_pos);

	// Move node from one position to another. from,to=0 end position.
	// Note: Whether data structures are switched or their content is
	// replaced is undefined.  Don't rely on one or the other.
	CONCEPT *moveNode(PHRASE *phr, int        from, int        to);
	CONCEPT *moveNode(PHRASE *phr, _TCHAR    *s_from, _TCHAR    *s_to);
	CONCEPT *moveNode(PHRASE *phr, CONCEPT *n_from, CONCEPT *n_to);

	// "Bubble" node left or right.
	bool moveNleft(CONCEPT *node);
	bool moveNright(CONCEPT *node);

	// Delete a node.
	bool rmNode(PHRASE* &phr,  _TCHAR *name);
	bool rmNode(PHRASE* &phr,  int    pos);
	bool rmNode(PHRASE* &phr, CONCEPT *node);
	bool rmNode(CONCEPT *node);

	// Delete phrase.
	bool rmPhrase(PHRASE* &phr);
	bool rmCphrase(CONCEPT *conc);	// Given concept that owns phrase.

	// Rename node.
	bool renameNode(PHRASE  *phr, _TCHAR *name, _TCHAR *rename);
	bool renameNode(PHRASE  *phr, int    pos, _TCHAR *rename);

	//////////////////////
	// UTILITIES
	//////////////////////

	// NOTE: These are not meant to be API functions.

	// Recursive workhorse for findHierConcept.
	CONCEPT *hierConcept(_TCHAR *name, CONCEPT *tree, bool isroot);

	bool sort_hier_rec(CONCEPT *tree, bool isroot);         // 02/20/09 AM.

	bool writeTree(															// 04/28/99 AM.
		CONCEPT *root,
		bool flag,				// true if root of subtree.
		std::_t_ofstream &hier,
		std::_t_ofstream &word,
		std::_t_ofstream &phr,
		_TCHAR *path,				// Hierarchy path down to parent.
		std::_t_ofstream *&attr,														// 07/01/03 AM.
		long &n_attr,				// # of attr files.					// 07/01/03 AM.
		long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
		_TCHAR *o_attr=0,														// 07/01/03 AM.
		_TCHAR *fpath=0,															// 07/01/03 AM.
		_TCHAR *suff=0															// 07/01/03 AM.
		);
	bool writeKBmain(long,_TCHAR*,_TCHAR*);									// 07/01/03 AM.
	bool writeAttrs(															// 04/29/99 AM.
		CONCEPT *con,			// Concept to traverse.
		int pos,					// 0 for concept, positive int for each
									// node of concept's phrase.
		std::_t_ofstream *&attr,		// File to write attributes to.		// 07/01/03 AM.
		_TCHAR *conpath,			// Hierarchy path down to parent.
		long &n_attr,			// # of attr files.						// 07/01/03 AM.
		long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
		_TCHAR *b_attr=0,														// 07/01/03 AM.
		_TCHAR *fpath=0,															// 07/01/03 AM.
		_TCHAR *suff=0															// 07/01/03 AM.
		);
	bool writePhrase(															// 04/30/99 AM.
		CONCEPT *conc,
		std::_t_ofstream &o_hier,
		std::_t_ofstream &o_word,
		std::_t_ofstream *&o_attr,
		std::_t_ofstream &o_phr,
		_TCHAR *path,				// Hierarchy path down to parent.
		long &n_attr,			// # of attr files.						// 07/01/03 AM.
		long &c_attr,			// # of attributes in curr file.		// 07/01/03 AM.
		_TCHAR *b_attr=0,														// 07/01/03 AM.
		_TCHAR *fpath=0,															// 07/01/03 AM.
		_TCHAR *suff=0															// 07/01/03 AM.
		);

	// The internal function for the TAKE command or take().
	bool readFile(_TCHAR *file);

	void prunePhrasesRec(CONCEPT *conc, bool root);					// 08/10/99 AM.

	// Class-wide
	static bool goodKbfile(_TCHAR*);										// 08/06/01 AM.

	// Create analyzer sequence.  Allow a header comment.
	//LIBCONSH_API static bool makeSeq(NLP *nlp, char *comment);

	// Store KB Manager object.											// 06/11/02 AM.
	AKBM *kbm_;																	// 06/11/02 AM.

	// References.																// 08/15/02 AM.
	APTR *aptr_;
	AST *ast_;
	ASYM *asym_;
	ACON *acon_;

	// List Manager object.	(Just a reference.)						// 08/14/02 AM.
	ALIST *alist_;																// 08/14/02 AM.

	// Moving some KB globals here.										// 08/22/02 AM.
	ID cg_CONCEPT;																// 08/22/02 AM.
	ID cg_PHRASE;																// 08/22/02 AM.
	ID cg_ATOM;																	// 08/22/02 AM.
	ID nlp_PUNCT;																// 08/22/02 AM.
	ID nlp_WHT;																	// 08/22/02 AM.

private:
	_TCHAR appdir_[FNAMESIZ];		// Base directory path.
	_TCHAR kbdir_[MAXPATH];

	std::_t_ifstream allDictStream_;

	bool dirty_;					// KB dirty flag.						// 05/12/00 AM.

	// For diverting the libconsh error output.						// 09/16/99 AM.
	std::_t_ostream *serr_;															// 09/16/99 AM.
	std::_t_ofstream *ferr_;															// 09/16/99 AM.
	_TCHAR errout_[PATHSIZ];													// 09/16/99 AM.

// COMPILED KB IN LINUX.	// 02/19/19 AM.
//#ifndef LINUX

	// COMPILED KB TO LOAD DYNAMICALLY.									// 06/29/00 AM.
//	HINSTANCE hkbdll_;	// 06/29/00 AM.
	consh_cg::HINSTANCE hkbdll_;	// 06/29/00 AM.	// 09/26/19 AM.
//#endif


// When code is well debugged and stable, can remove the stuff below.
#ifndef STABLE_
public:
	static int getCount();
	static void prettyCount();				// Pretty-print the count.
private:
	static int count_;						// Count objects currently allocated.
#endif
};



#endif
/*************************      END OF FILE      ******************************/
