/*
   
   wntypes.h - Typedef definitions for WordNet library

   $Id: wntypes.h,v 1.5 2002/03/07 18:44:48 wn Rel $

*/

#ifndef _WNTYPES_
#define _WNTYPES_

#include "wnconsts.h"

/* Data structures used by search code functions. */

/* Structure for index file entry */
typedef struct {
    long idxoffset;		/* byte offset of entry in index file */
    _TCHAR *wd;			/* word string */
    _TCHAR *pos;			/* part of speech */
    int sense_cnt;		/* sense (collins) count */
    int off_cnt;		/* number of offsets */
    int tagged_cnt;		/* number senses that are tagged */
    unsigned long *offset;	/* offsets of synsets containing word */
    int ptruse_cnt;		/* number of pointers used */
    int *ptruse;		/* pointers used */
} Index, *IndexPtr;

/* Structure for data file synset */
typedef struct ss {
    long hereiam;		/* current file position */
    int sstype;			/* type of ADJ synset */
    int fnum;			/* file number that synset comes from */
    _TCHAR *pos;			/* part of speech */
    int wcount;			/* number of words in synset */
    _TCHAR **words;		/* words in synset */
    int *lexid;			/* unique id in lexicographer file */
    int *wnsns;			/* sense number in wordnet */
    int whichword;		/* which word in synset we're looking for */
    int ptrcount;		/* number of pointers */
    int *ptrtyp;		/* pointer types */
    long *ptroff;		/* pointer offsets */
    int *ppos;			/* pointer part of speech */
    int *pto;			/* pointer 'to' fields */
    int *pfrm;			/* pointer 'from' fields */
    int fcount;			/* number of verb frames */
    int *frmid;			/* frame numbers */
    int *frmto;			/* frame 'to' fields */
    _TCHAR *defn;			/* synset gloss (definition) */
    unsigned int key;		/* unique synset key */

    /* these fields are used if a data structure is returned
       instead of a text buffer */

    struct ss *nextss;		/* ptr to next synset containing searchword */
    struct ss *nextform;	/* ptr to list of synsets for alternate
				   spelling of wordform */
    int searchtype;		/* type of search performed */
    struct ss *ptrlist;		/* ptr to synset list result of search */
    _TCHAR *headword;		/* if pos is "s", this is cluster head word */
    short headsense;		/* sense number of headword */
} Synset, *SynsetPtr;

typedef struct si {
    _TCHAR *sensekey;		/* sense key */
    _TCHAR *word;			/* word string */
    long loc;			/* synset offset */
    int wnsense;		/* WordNet sense number */
    int tag_cnt;		/* number of semantic tags to sense */
    struct si *nextsi;		/* ptr to next sense index entry */
} SnsIndex, *SnsIndexPtr;
    
typedef struct {
    int SenseCount[MAX_FORMS];	/* number of senses word form has */
    int OutSenseCount[MAX_FORMS]; /* number of senses printed for word form */
    int numforms;		/* number of word forms searchword has */
    int printcnt;		/* number of senses printed by search */
    _TCHAR *searchbuf;		/* buffer containing formatted results */
    SynsetPtr searchds;		/* data structure containing search results */
} SearchResults, *SearchResultsPtr;

#endif /* _WNTYPES_ */
