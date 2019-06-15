/*
   
   wn.h - header file needed to use WordNet Run Time Library

   $Id: wn.h,v 1.55 2002/03/07 18:44:40 wn Rel $

*/

#ifndef _WN_
#define _WN_

#include "wnconsts.h"
#include "wntypes.h"
#include "wnglobal.h"
#include "wnrtl.h"

/* Make all the functions compatible with c++ files */
#ifdef __cplusplus
extern "C" {
#endif

/* External library function prototypes */

/*** Search and database functions (search.c) ***/

/* Primry search algorithm for use with user interfaces */
extern _TCHAR *findtheinfo(_TCHAR *, int, int, int);	

/* Primary search algorithm for use with programs (returns data structure) */
extern SynsetPtr findtheinfo_ds(_TCHAR *, int, int, int); 

/* Set bit for each search type that is valid for the search word
   passed and return bit mask. */
extern unsigned int is_defined(_TCHAR *, int); 

/* Set bit for each POS that search word is in.  0 returned if
   word is not in WordNet. */
extern unsigned int in_wn(_TCHAR *, int);	

/* Find word in index file and return parsed entry in data structure.
   Input word must be exact match of string in database. */
extern IndexPtr index_lookup(_TCHAR *, int); 

/* 'smart' search of index file.  Find word in index file, trying different
   techniques - replace hyphens with underscores, replace underscores with
   hyphens, strip hyphens and underscores, strip periods. */
extern IndexPtr getindex(_TCHAR *, int);	
extern IndexPtr parse_index(long, int, _TCHAR *);

/* Read synset from data file at byte offset passed and return parsed
   entry in data structure. */
extern SynsetPtr read_synset(int, long, _TCHAR *);

/* Read synset at current byte offset in file and return parsed entry
   in data structure. */
extern SynsetPtr parse_synset(FILE *, int, _TCHAR *); 

/* Free a synset linked list allocated by findtheinfo_ds() */
extern void free_syns(SynsetPtr);	

/* Free a synset */
extern void free_synset(SynsetPtr);	

/* Free an index structure */
extern void free_index(IndexPtr);	

/* Recursive search algorithm to trace a pointer tree and return results
   in linked list of data structures. */
SynsetPtr traceptrs_ds(SynsetPtr, int, int, int);

/* Do requested search on synset passed, returning output in buffer. */
extern _TCHAR *do_trace(SynsetPtr, int, int, int);

/*** Morphology functions (morph.c) ***/

/* Open exception list files */
extern int morphinit();	

/* Close exception list files and reopen */
extern int re_morphinit();	

/* Try to find baseform (lemma) of word or collocation in POS. */
extern _TCHAR *morphstr(_TCHAR *, int);	

/* Try to find baseform (lemma) of individual word in POS. */
extern _TCHAR *morphword(_TCHAR *, int);	

/*** Utility functions (wnutil.c) ***/

/* Top level function to open database files, initialize wn_filenames,
   and open exeception lists. */
extern int wninit(_TCHAR buff[256]);		
//extern int wninit();		

/* Top level function to close and reopen database files, initialize
   wn_filenames and open exception lists. */
extern int re_wninit();	

/* Count the number of underscore or space separated words in a string. */
extern int cntwords(_TCHAR *, _TCHAR);		

/* Convert string to lower case remove trailing adjective marker if found */
extern _TCHAR *strtolower(_TCHAR *);	

/* Convert string passed to lower case */
extern _TCHAR *ToLowerCase(_TCHAR *);	

/* Replace all occurrences of 'from' with 'to' in 'str' */
extern _TCHAR *strsubst(_TCHAR *, _TCHAR, _TCHAR);	

/* Return pointer code for pointer type characer passed. */
extern int getptrtype(_TCHAR *);	

/* Return part of speech code for string passed */
extern int getpos(_TCHAR *);		

/* Return synset type code for string passed. */
extern int getsstype(_TCHAR *);		

/* Reconstruct synset from synset pointer and return ptr to buffer */
extern _TCHAR *FmtSynset(SynsetPtr, int);	

/* Find string for 'searchstr' as it is in index file */
extern _TCHAR *GetWNStr(_TCHAR *, int);

/* Pass in string for POS, return corresponding integer value */
extern int StrToPos(_TCHAR *);

/* Return synset for sense key passed. */
extern SynsetPtr GetSynsetForSense(_TCHAR *);

/* Find offset of sense key in data file */
extern long GetDataOffset(_TCHAR *);

/* Find polysemy (collins) count for sense key passed. */
extern int GetPolyCount(_TCHAR *);

/* Return word part of sense key */
extern _TCHAR *GetWORD(_TCHAR *);

/* Return POS code for sense key passed. */
extern int GetPOS(_TCHAR *);

/* Convert WordNet sense number passed of IndexPtr entry to sense key. */
extern _TCHAR *WNSnsToStr(IndexPtr, int);

/* Search for string and/or baseform of word in database and return
   index structure for word if found in database. */
extern IndexPtr GetValidIndexPointer(_TCHAR *, int);

/* Return sense number in database for word and lexsn passed. */
int GetWNSense(_TCHAR *, _TCHAR *);

SnsIndexPtr GetSenseIndex(_TCHAR *);

_TCHAR *GetOffsetForKey(unsigned int);
unsigned int GetKeyForOffset(_TCHAR *);

_TCHAR *SetSearchdir();

/* Return number of times sense is tagged */
int GetTagcnt(IndexPtr, int);

/*
** Wrapper functions for strstr that allow you to retrieve each
** occurance of a word within a longer string, not just the first.
**
** strstr_init is called with the same arguments as normal strstr,
** but does not return any value.
**
** strstr_getnext returns the position offset (not a pointer, as does
** normal strstr) of the next occurance, or -1 if none remain.
*/
extern void strstr_init (_TCHAR *, _TCHAR *);
extern int strstr_getnext (void);

/*** Binary search functions (binsearch.c) ***/

/* General purpose binary search function to search for key as first
   item on line in open file.  Item is delimited by space. */
extern _TCHAR *bin_search(_TCHAR *, FILE *);
extern _TCHAR *read_index(long, FILE *);

/* Copy contents from one file to another. */
extern void copyfile(FILE *, FILE *);

/* Function to replace a line in a file.  Returns the original line,
   or NULL in case of error. */
extern _TCHAR *replace_line(_TCHAR *, _TCHAR *, FILE *);

/* Find location to insert line at in file.  If line with this
   key is already in file, return NULL. */
extern _TCHAR *insert_line(_TCHAR *, _TCHAR *, FILE *);

#ifdef __cplusplus
}
#endif

#endif /*_WN_*/



