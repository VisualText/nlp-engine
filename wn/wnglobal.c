/*

   wnglobal.c - global variables used by various WordNet applications

   $Id: wnglobal.c,v 1.53 2003/04/15 13:54:42 wn Exp $

*/

#include <tchar.h>

#ifndef NULL
#define NULL	0
#endif

_TCHAR *wnrelease = _T("2.0");

/* Lexicographer file names and numbers */

_TCHAR *lexfiles[] = {
    _T("adj.all"),			/* 0 */
    _T("adj.pert"),			/* 1 */
    _T("adv.all"),			/* 2 */
    _T("noun.Tops"),		/* 3 */
    _T("noun.act"),			/* 4 */
    _T("noun.animal"),		/* 5 */
    _T("noun.artifact"),		/* 6 */
    _T("noun.attribute"),		/* 7 */
    _T("noun.body"),		/* 8 */
    _T("noun.cognition"),		/* 9 */
    _T("noun.communication"),	/* 10 */
    _T("noun.event"),		/* 11 */
    _T("noun.feeling"),		/* 12 */
    _T("noun.food"),		/* 13 */
    _T("noun.group"),		/* 14 */
    _T("noun.location"),		/* 15 */
    _T("noun.motive"),		/* 16 */
    _T("noun.object"),		/* 17 */
    _T("noun.person"),		/* 18 */
    _T("noun.phenomenon"),		/* 19 */
    _T("noun.plant"),		/* 20 */
    _T("noun.possession"),		/* 21 */
    _T("noun.process"),		/* 22 */
    _T("noun.quantity"),		/* 23 */
    _T("noun.relation"),		/* 24 */
    _T("noun.shape"),		/* 25 */
    _T("noun.state"),		/* 26 */
    _T("noun.substance"),		/* 27 */
    _T("noun.time"),		/* 28 */
    _T("verb.body"),		/* 29 */
    _T("verb.change"),		/* 30 */
    _T("verb.cognition"),		/* 31 */
    _T("verb.communication"),	/* 32 */
    _T("verb.competition"),		/* 33 */
    _T("verb.consumption"),		/* 34 */
    _T("verb.contact"),		/* 35 */
    _T("verb.creation"),		/* 36 */
    _T("verb.emotion"),		/* 37 */
    _T("verb.motion"),		/* 38 */
    _T("verb.perception"),		/* 39 */
    _T("verb.possession"),		/* 40 */
    _T("verb.social"),		/* 41 */
    _T("verb.stative"),		/* 42 */
    _T("verb.weather"),		/* 43 */
    _T("adj.ppl"),			/* 44 */
};

/* Pointer characters and searches */

_TCHAR *ptrtyp[]={
    _T(""),				/* 0 not used */
    _T("!"),			/* 1 ANTPTR */
    _T("@"),			/* 2 HYPERPTR */
    _T("~"),			/* 3 HYPOPTR */
    _T("*"),			/* 4 ENTAILPTR */
    _T("&"),			/* 5 SIMPTR */
    _T("#m"),			/* 6 ISMEMBERPTR */
    _T("#s"),			/* 7 ISSTUFFPTR */
    _T("#p"),			/* 8 ISPARTPTR */
    _T("%m"),			/* 9 HASMEMBERPTR */
    _T("%s"),			/* 10 HASSTUFFPTR */
    _T("%p"),			/* 11 HASPARTPTR */
    _T("%"),			/* 12 MERONYM */
    _T("#"),			/* 13 HOLONYM */
    _T(">"),			/* 14 CAUSETO */
    _T("<"),			/* 15 PPLPTR */
    _T("^"),			/* 16 SEEALSO */
    _T("\\"),			/* 17 PERTPTR */
    _T("="),			/* 18 ATTRIBUTE */
    _T("$"),			/* 19 VERBGROUP */
    _T("+"),		        /* 20 NOMINALIZATIONS */
    _T(";"),			/* 21 CLASSIFICATION */
    _T("-"),			/* 22 CLASS */
/* additional searches, but not pointers.  */
    _T(""),				/* SYNS */
    _T(""),				/* FREQ */
    _T("+"),			/* FRAMES */
    _T(""),				/* COORDS */
    _T(""),				/* RELATIVES */
    _T(""),				/* HMERONYM */
    _T(""),				/* HHOLONYM */
    _T(""),				/* WNGREP */
    _T(""),				/* OVERVIEW */
    _T(";c"),			/* CLASSIF_CATEGORY */
    _T(";u"),			/* CLASSIF_USAGE */
    _T(";r"),			/* CLASSIF_REGIONAL */
    _T("-c"),			/* CLASS_CATEGORY */
    _T("-u"),			/* CLASS_USAGE */
    _T("-r"),			/* CLASS_REGIONAL */
    NULL,
};

_TCHAR *partnames[]={ _T(""), _T("noun"), _T("verb"), _T("adj"), _T("adv"), NULL };
_TCHAR partchars[] = _T(" nvara");	/* add char for satellites to end */
_TCHAR *adjclass[] = { _T(""), _T("(p)"), _T("(a)"), _T("(ip)") };

/* Text of verb sentence frames */

_TCHAR *frametext[] = {
    _T(""),
    _T("Something ----s"),
    _T("Somebody ----s"),
    _T("It is ----ing"),
    _T("Something is ----ing PP"),
    _T("Something ----s something Adjective/Noun"),
    _T("Something ----s Adjective/Noun"),
    _T("Somebody ----s Adjective"),
    _T("Somebody ----s something"),
    _T("Somebody ----s somebody"),
    _T("Something ----s somebody"),
    _T("Something ----s something"),
    _T("Something ----s to somebody"),
    _T("Somebody ----s on something"),
    _T("Somebody ----s somebody something"),
    _T("Somebody ----s something to somebody"),
    _T("Somebody ----s something from somebody"),
    _T("Somebody ----s somebody with something"),
    _T("Somebody ----s somebody of something"),
    _T("Somebody ----s something on somebody"),
    _T("Somebody ----s somebody PP"),
    _T("Somebody ----s something PP"),
    _T("Somebody ----s PP"),
    _T("Somebody's (body part) ----s"),
    _T("Somebody ----s somebody to INFINITIVE"),
    _T("Somebody ----s somebody INFINITIVE"),
    _T("Somebody ----s that CLAUSE"),
    _T("Somebody ----s to somebody"),
    _T("Somebody ----s to INFINITIVE"),
    _T("Somebody ----s whether INFINITIVE"),
    _T("Somebody ----s somebody into V-ing something"),
    _T("Somebody ----s something with something"),
    _T("Somebody ----s INFINITIVE"),
    _T("Somebody ----s VERB-ing"),
    _T("It ----s that CLAUSE"),
    _T("Something ----s INFINITIVE"),
    _T("")
};
