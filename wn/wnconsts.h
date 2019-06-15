/*

   wnconsts.h - constants used by all parts of WordNet system

   $Id: wnconsts.h,v 1.73 2003/06/05 15:29:17 wn Exp $

*/

#ifndef _WNCONSTS_
#define _WNCONSTS_

#define WN1_7
#define WN1_7_1

#include <tchar.h>
/* Platform specific path and filename specifications */

#ifdef UNIX
#define DICTDIR         _T("/dict")
#define DEFAULTPATH	_T("/usr/local/WordNet-2.0/dict")
#define DEFAULTBIN      _T("/usr/local/WordNet-2.0/bin")
#define DATAFILE	_T("%s/data.%s")
#define INDEXFILE	_T("%s/index.%s")
#define SENSEIDXFILE	_T("%s/index.sense")
#define KEYIDXFILE	_T("%s/index.key")
#define REVKEYIDXFILE	_T("%s/index.key.rev")
#ifdef WN1_6
#define COUSINFILE	_T("%s/cousin.tops")
#define COUSINEXCFILE	_T("%s/cousin.exc")
#endif
#define VRBSENTFILE     _T("%s/sents.vrb")
#define VRBIDXFILE	_T("%s/sentidx.vrb")
#define CNTLISTFILE     _T("%s/cntlist.rev")
#endif
#ifdef PC
#define DICTDIR         _T("\\dict")
#define DEFAULTPATH	_T("c:\\Program Files\\WordNet\\2.0\\dict")
#define DEFAULTBIN      _T("c:\\Program Files\\WordNet\\2.0\\bin")
#define DATAFILE	_T("%s\\%s.dat")
#define INDEXFILE	_T("%s\\%s.idx")
#define SENSEIDXFILE	_T("%s\\sense.idx")
#define KEYIDXFILE	_T("%s\\key.idx")
#define REVKEYIDXFILE	_T("%s\\revkey.idx")
#ifdef WN1_6
#define COUSINFILE	_T("%s\\cousin.tps")
#define COUSINEXCFILE	_T("%s\\cousin.exc")
#endif
#define VRBSENTFILE  	_T("%s\\sents.vrb")
#define VRBIDXFILE	_T("%s\\sentidx.vrb")
#define CNTLISTFILE     _T("%s\\cntlist.rev")
#endif
#ifdef MAC
#define DICTDIR         _T(":Database")
#define DEFAULTPATH     _T(":Database")
#define DEFAULTBIN      _T(":")
#define DATAFILE	_T("%s:data.%s")
#define INDEXFILE	_T("%s:index.%s")
#define SENSEIDXFILE	_T("%s:index.sense")
#define KEYIDXFILE	_T("%s:index.key")
#ifdef WN1_6
#define COUSINFILE	_T("%s:cousin.tops")
#define COUSINEXCFILE	_T("%s:cousin.exc")
#endif
#define VRBSENTFILE     _T("%s:sents.vrb")
#define VRBIDXFILE 	_T("%s:sentidx.vrb")
#define CNTLISTFILE     _T("%s:cntlist.rev")
#endif

/* Various buffer sizes */

#define SEARCHBUF	((long)(200*(long)1024))
#define LINEBUF		(15*1024) /* 15K buffer to read index & data files */
#define SMLINEBUF	(3*1024) /* small buffer for output lines */
#define WORDBUF		(256)	/* buffer for one word or collocation */

#define ALLSENSES	0	/* pass to findtheinfo() if want all senses */
#define MAXID		15	/* maximum id number in lexicographer file */
#define MAXDEPTH	20	/* maximum tree depth - used to find cycles */
#define MAXSENSE	75	/* maximum number of senses in database */
#define MAX_FORMS	5	/* max # of different 'forms' word can have */
#define MAXFNUM		44	/* maximum number of lexicographer files */

/* Pointer type and search type counts */

/* Pointers */

#define ANTPTR           1	/* ! */
#define HYPERPTR         2	/* @ */
#define HYPOPTR          3	/* ~ */
#define ENTAILPTR        4	/* * */
#define SIMPTR           5	/* & */

#define ISMEMBERPTR      6	/* #m */
#define ISSTUFFPTR       7	/* #s */
#define ISPARTPTR        8	/* #p */

#define HASMEMBERPTR     9	/* %m */
#define HASSTUFFPTR     10	/* %s */
#define HASPARTPTR      11	/* %p */

#define MERONYM         12	/* % (not valid in lexicographer file) */
#define HOLONYM         13	/* # (not valid in lexicographer file) */
#define CAUSETO         14	/* > */
#define PPLPTR	        15	/* < */
#define SEEALSOPTR	16	/* ^ */
#define PERTPTR		17	/* \ */
#define ATTRIBUTE	18	/* = */
#define VERBGROUP	19	/* $ */
#define DERIVATION      20	/* + */
#define CLASSIFICATION  21	/* ; */
#define CLASS           22	/* - */

#define LASTTYPE	CLASS

/* Misc searches */

#define SYNS            (LASTTYPE + 1)
#define FREQ            (LASTTYPE + 2)
#define FRAMES          (LASTTYPE + 3)
#define COORDS          (LASTTYPE + 4)
#define RELATIVES	(LASTTYPE + 5)
#define HMERONYM        (LASTTYPE + 6)
#define HHOLONYM	(LASTTYPE + 7)
#define WNGREP		(LASTTYPE + 8)
#define OVERVIEW	(LASTTYPE + 9)

#define MAXSEARCH       OVERVIEW

#define CLASSIF_START    (MAXSEARCH + 1)

#define CLASSIF_CATEGORY (CLASSIF_START)        /* ;c */
#define CLASSIF_USAGE    (CLASSIF_START + 1)    /* ;u */
#define CLASSIF_REGIONAL (CLASSIF_START + 2)    /* ;r */

#define CLASSIF_END      CLASSIF_REGIONAL

#define CLASS_START      (CLASSIF_END + 1)

#define CLASS_CATEGORY   (CLASS_START)          /* -c */
#define CLASS_USAGE      (CLASS_START + 1)      /* -u */
#define CLASS_REGIONAL   (CLASS_START + 2)      /* -r */

#define CLASS_END        CLASS_REGIONAL

#define MAXPTR          CLASS_END

/* WordNet part of speech stuff */

#define NUMPARTS	4	/* number of parts of speech */
#define NUMFRAMES	35	/* number of verb frames */

/* Generic names for part of speech */

#define NOUN		1
#define VERB		2
#define ADJ		3
#define ADV		4
#define SATELLITE	5	/* not really a part of speech */
#define ADJSAT		SATELLITE

#define ALL_POS		0	/* passed to in_wn() to check all POS */

#define bit(n) ((unsigned int)((unsigned int)1<<((unsigned int)n)))

/* Adjective markers */

#define PADJ		1	/* (p) */
#define NPADJ		2	/* (a) */
#define IPADJ		3	/* (ip) */

#define UNKNOWN_MARKER		0
#define ATTRIBUTIVE		NPADJ
#define PREDICATIVE		PADJ
#define IMMED_POSTNOMINAL	IPADJ

#endif				/* _WNCONSTS_ */

