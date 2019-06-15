/*

  wnutil.c - utility functions used by WordNet code

*/

#include <stdio.h>
#include <ctype.h>

#ifdef UNIX
#include <malloc.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#ifdef MAC
#include "macfuncs.h"
#endif

#include "wn.h"

static int do_init();

static _TCHAR msgbuf[256];	/* buffer for constructing error messages */

/* used by the strstr wrapper functions */
static _TCHAR *strstr_word;
static _TCHAR *strstr_stringstart;
static _TCHAR *strstr_stringcurrent;


/* Initialization functions */

static void closefps();


int wninit(_TCHAR buff[256])
{
    static int done = 0;
    static int openerr = 0;
    _TCHAR *env;

    if (!done) {
	if (env = _tgetenv(_T("WNDBVERSION"))) {
	    wnrelease = _tcsdup(env);	/* set release */
	    assert(wnrelease);
	}
	openerr = do_init();
	if (!openerr) {	
	    done = 1;	
	    OpenDB = 1;
	    openerr = morphinit();
	}
    }

	_tcsnccpy(buff,msgbuf,256);

    return(openerr);
}


int re_wninit()
{
    int openerr;
    _TCHAR *env;

    closefps();

    if (env = _tgetenv(_T("WNDBVERSION"))) {
	wnrelease = _tcsdup(env);	/* set release */
	assert(wnrelease);
    }
    openerr = do_init();
    if (!openerr) {
	OpenDB = 1;
	openerr = re_morphinit();
    }

    return(openerr);
}

static void closefps(void) 
{
    int i;

    if (OpenDB) {
	for (i = 1; i < NUMPARTS + 1; i++) {
	    if (datafps[i] != NULL)
		fclose(datafps[i]); datafps[i] = NULL;
	    if (indexfps[i] != NULL)
		fclose(indexfps[i]); indexfps[i] = NULL;
	}
	if (sensefp != NULL) {
	    fclose(sensefp); sensefp = NULL;
	}
#ifdef WN1_6
	if (cousinfp != NULL) {
	    fclose(cousinfp); cousinfp = NULL;
	}
	if (cousinexcfp != NULL) {
	    fclose(cousinexcfp); cousinexcfp = NULL;
	}
#endif
	if (cntlistfp != NULL) {
	    fclose(cntlistfp); cntlistfp = NULL;
	}
	if (keyindexfp != NULL) {
	    fclose(keyindexfp); keyindexfp = NULL;
	}
	if (vsentfilefp != NULL) {
	    fclose(vsentfilefp); vsentfilefp = NULL;
	}
	if (vidxfilefp != NULL) {
	    fclose(vidxfilefp); vidxfilefp = NULL;
	}
	OpenDB = 0;
    }
}

static int do_init(void)
{
    int i, openerr;
    _TCHAR searchdir[256], tmpbuf[256];
    _TCHAR *env;
 
    openerr = 0;

    /* Find base directory for database.  If set, use WNSEARCHDIR.
       If not set, check for WNHOME/dict, otherwise use DEFAULTPATH. */

    if ((env = _tgetenv(_T("WNSEARCHDIR"))) != NULL)
	_tcscpy(searchdir, env);
    else if ((env = _tgetenv(_T("WNHOME"))) != NULL)
	_stprintf(searchdir, _T("%s%s"), env, DICTDIR);
    else
	_tcscpy(searchdir, DEFAULTPATH);

    for (i = 1; i < NUMPARTS + 1; i++) {
	_stprintf(tmpbuf, DATAFILE, searchdir, partnames[i]);
	if((datafps[i] = _tfopen(tmpbuf, _T("r"))) == NULL) {
	    _stprintf(msgbuf,
		    _T("WordNet library error: Can't open datafile(%s)\n"),
		    tmpbuf);
	    display_message(msgbuf);
	    openerr = -1;
	}
	_stprintf(tmpbuf, INDEXFILE, searchdir, partnames[i]);
	if((indexfps[i] = _tfopen(tmpbuf, _T("r"))) == NULL) {
	    _stprintf(msgbuf,
		    _T("WordNet library error: Can't open indexfile(%s)\n"),
		    tmpbuf);
	    display_message(msgbuf);
	    openerr = -1;
	}
    }

    /* This file isn't used by the library and doesn't have to
       be present.  No error is reported if the open fails. */

    _stprintf(tmpbuf, SENSEIDXFILE, searchdir);
    sensefp = _tfopen(tmpbuf, _T("r"));

    /* If this file isn't present, the runtime code will skip printint out
       the number of times each sense was tagged. */

    _stprintf(tmpbuf, CNTLISTFILE, searchdir);
    cntlistfp = _tfopen(tmpbuf, _T("r"));

    /* This file doesn't have to be present.  No error is reported if the
       open fails. */

    _stprintf(tmpbuf, KEYIDXFILE, searchdir);
    keyindexfp = _tfopen(tmpbuf, _T("r"));

    _stprintf(tmpbuf, REVKEYIDXFILE, searchdir);
    revkeyindexfp = _tfopen(tmpbuf, _T("r"));

#ifdef WN1_6
    _stprintf(tmpbuf, COUSINFILE, searchdir);
    if ((cousinfp = _tfopen(tmpbuf, _T("r"))) == NULL) {
	_stprintf(msgbuf,
		_T("WordNet library error: Can't open cousin tops file(%s)\n"),
		tmpbuf);
	display_message(msgbuf);
	openerr = -1;
    }
    _stprintf(tmpbuf, COUSINEXCFILE, searchdir);
    if ((cousinexcfp = _tfopen(tmpbuf, _T("r"))) == NULL) {
	_stprintf(msgbuf,
		_T("WordNet library error: Can't open exception file(%s)\n"),
		tmpbuf);
	display_message(msgbuf);
	openerr = -1;
    }
#endif

    _stprintf(tmpbuf, VRBSENTFILE, searchdir);
    if ((vsentfilefp = _tfopen(tmpbuf, _T("r"))) == NULL) {
	_stprintf(msgbuf,
_T("WordNet library warning: Can't open verb example sentence file(%s)\n"),
		tmpbuf);
	display_message(msgbuf);
    }

    _stprintf(tmpbuf, VRBIDXFILE, searchdir);
    if ((vidxfilefp = _tfopen(tmpbuf, _T("r"))) == NULL) {
	_stprintf(msgbuf,
_T("WordNet library warning: Can't open verb example sentence index file(%s)\n"),
		tmpbuf);
	display_message(msgbuf);
    }

    return(openerr);
}

/* Count the number of underscore or space separated words in a string. */

int cntwords(_TCHAR *s, _TCHAR separator)
{
    register int wdcnt = 0;

    while (*s) {
	if (*s == separator || *s == ' ' || *s == '_') {
	    wdcnt++;
	    while (*s && (*s == separator || *s == ' ' || *s == '_'))
		s++;
	} else
	    s++;
    }
    return(++wdcnt);
}

/* Convert string to lower case remove trailing adjective marker if found */

_TCHAR *strtolower(_TCHAR *str)
{
    register _TCHAR *s = str;

    while(*s != '\0') {
	if(*s >= 'A' && *s <= 'Z')
	    *s += 32;
	else if(*s == '(') {
	    *s='\0';
	    break;
	}
	s++;
    }
    return(str);
}

/* Convert string passed to lower case */

_TCHAR *ToLowerCase(_TCHAR *str)
{
    register _TCHAR *s = str;

    while(*s != '\0') {
	if(*s >= 'A' && *s <= 'Z')
	    *s += 32;
	s++;
    }
    return(str);
}

/* Replace all occurences of 'from' with 'to' in 'str' */

_TCHAR *strsubst(_TCHAR *str, _TCHAR from, _TCHAR to)
{
    register _TCHAR *p;

    for (p = str; *p != 0; ++p)
	if (*p == from)
	    *p = to;
    return str;
}

/* Return pointer code for pointer type characer passed. */

int getptrtype(_TCHAR *ptrstr)
{
    register int i;
    for(i = 1; i <= MAXPTR; i++) {
	if(!_tcscmp(ptrstr, ptrtyp[i]))
	    return(i);
    }
    return(0);
}

/* Return part of speech code for string passed */

int getpos(_TCHAR *s)
{
    switch (*s) {
    case 'n':
	return(NOUN);
    case 'a':
    case 's':
	return(ADJ);
    case 'v':
	return(VERB);
    case 'r':
	return(ADV);
    default:
	_stprintf(msgbuf,
		_T("WordNet library error: unknown part of speech %s\n"), s);
	display_message(msgbuf);
	exit(-1);
    }
}

/* Return synset type code for string passed. */

int getsstype(_TCHAR *s)
{
    switch (*s) {
    case 'n':
	return(NOUN);
    case 'a':
	return(ADJ);
    case 'v':
	return(VERB);
    case 's':
	return(SATELLITE);
    case 'r':
	return(ADV);
    default:
	_stprintf(msgbuf, _T("WordNet library error: Unknown synset type %s\n"), s);
	display_message(msgbuf);
	exit(-1);
    }
}

/* Pass in string for POS, return corresponding integer value */

int StrToPos(_TCHAR *str)
{
    if (!_tcscmp(str, _T("noun")))
	return(NOUN);
    else if (!_tcscmp(str, _T("verb")))
	return(VERB);
    else if (!_tcscmp(str, _T("adj")))
	return(ADJ);
    else if (!_tcscmp(str, _T("adv")))
	return(ADV);
    else {
	return(-1);
    }
}

#define MAX_TRIES	5

/* Find string for 'searchstr' as it is in index file */

_TCHAR *GetWNStr(_TCHAR *searchstr, int dbase)
{
    register int i, j, k, offset = 0;
    register _TCHAR c;
    _TCHAR *underscore = NULL, *hyphen = NULL, *period = NULL;
    static _TCHAR strings[MAX_TRIES][WORDBUF];
	
    ToLowerCase(searchstr);

    if (!(underscore = _tcschr(searchstr, '_')) &&
	!(hyphen = _tcschr(searchstr, '-')) &&
	!(period = _tcschr(searchstr, '.')))
	return (_tcscpy(strings[0],searchstr));

    for(i = 0; i < 3; i++)
	_tcscpy(strings[i], searchstr);
    if (underscore != NULL) strsubst(strings[1], '_', '-');
    if (hyphen != NULL) strsubst(strings[2], '-', '_');
    for(i = j = k = 0; (c = searchstr[i]) != '\0'; i++){
	if(c != '_' && c != '-') strings[3][j++] = c;
	if(c != '.') strings[4][k++] = c;
    }
    strings[3][j] = '\0';
    strings[4][k] = '\0';
	
    for(i = 1; i < MAX_TRIES; i++)
	if(_tcscmp(strings[0], strings[i]) == 0) strings[i][0] = '\0';
	
    for (i = (MAX_TRIES - 1); i >= 0; i--)
	if (strings[i][0] != '\0')
	    if (bin_search(strings[i], indexfps[dbase]) != NULL)
		offset = i;
	
    return(strings[offset]);
}

/* Return synset for sense key passed. */

SynsetPtr GetSynsetForSense(_TCHAR *sensekey)
{
    long offset;

    /* Pass in sense key and return parsed sysnet structure */

    if ((offset = GetDataOffset(sensekey)))
	return(read_synset(GetPOS(sensekey),
			   offset,
			   GetWORD(sensekey)));
    else
	return(NULL);
}

/* Find offset of sense key in data file */

long GetDataOffset(_TCHAR *sensekey)
{
    _TCHAR *line;

    /* Pass in encoded sense string, return byte offset of corresponding
       synset in data file. */

    if (sensefp == NULL) {
	display_message(_T("WordNet library error: Sense index file not open\n"));
	return(0L);
    }
    line = bin_search(sensekey, sensefp);
    if (line) {
	while (*line++ != ' ');
	return(_ttol(line));
    } else
	return(0L);
}

/* Find polysemy count for sense key passed. */

int GetPolyCount(_TCHAR *sensekey)
{
    IndexPtr idx;
    int sense_cnt = 0;

    /* Pass in encoded sense string and return polysemy count
       for word in corresponding POS */

    idx = index_lookup(GetWORD(sensekey), GetPOS(sensekey));
    if (idx) {
	sense_cnt = idx->sense_cnt;
	free_index(idx);
    }
    return(sense_cnt);
}

/* Return word part of sense key */
_TCHAR *GetWORD(_TCHAR *sensekey)
{
    static _TCHAR word[100];
    int i = 0;

    /* Pass in encoded sense string and return WORD */

    while ((word[i++] = *sensekey++) != '%');
    word[i - 1] = '\0';
    return(word);
}

/* Return POS code for sense key passed. */

int GetPOS(_TCHAR *sensekey)
{
    int pos;

    /* Pass in encoded sense string and return POS */

    while (*sensekey++ != '%');	/* skip over WORD */
    _stscanf(sensekey, _T("%1d"), &pos);
    return(pos == SATELLITE ? ADJ : pos);
}

/* Reconstruct synset from synset pointer and return ptr to buffer */

_TCHAR *FmtSynset(SynsetPtr synptr, int defn)
{
    int i;
    static _TCHAR synset[SMLINEBUF];

    synset[0] = '\0';

    if (fileinfoflag)
	_stprintf(synset, _T("<%s> "), lexfiles[synptr->fnum]);

    _tcscat(synset, _T("{ "));
    for (i = 0; i < (synptr->wcount - 1); i++)
	_stprintf(synset + _tcsclen(synset), _T("%s, "), synptr->words[i]);

    _tcscat(synset, synptr->words[i]);

    if (defn && synptr->defn)
	_stprintf(synset + _tcsclen(synset), _T(" (%s) "), synptr->defn);

    _tcscat(synset, _T(" }"));
    return(synset);
}

/* Convert WordNet sense number passed of IndexPtr entry to sense key. */
_TCHAR *WNSnsToStr(IndexPtr idx, int sense)
{
    SynsetPtr sptr, adjss;
    _TCHAR sensekey[512], lowerword[256];
    int j, sstype, pos;

    pos = getpos(idx->pos);
    sptr = read_synset(pos, idx->offset[sense - 1], _T(""));

    if ((sstype = getsstype(sptr->pos)) == SATELLITE) {
	for (j = 0; j < sptr->ptrcount; j++) {
	    if (sptr->ptrtyp[j] == SIMPTR) {
		adjss = read_synset(sptr->ppos[j],sptr->ptroff[j],_T(""));
		sptr->headword = malloc (_tcsclen(adjss->words[0]) + 1);
		assert(sptr->headword);
		_tcscpy(sptr->headword, adjss->words[0]);
		strtolower(sptr->headword);
		sptr->headsense = adjss->lexid[0];
		free_synset(adjss); 
		break;
	    }
	}
    }

    for (j = 0; j < sptr->wcount; j++) {
	_tcscpy(lowerword, sptr->words[j]);
	strtolower(lowerword);
	if(!_tcscmp(lowerword, idx->wd))
	    break;
    }

    if (j == sptr->wcount) {
	free_synset(sptr);
	return(NULL);
    }

    if (sstype == SATELLITE) 
	_stprintf(sensekey,_T("%s%%%-1.1d:%-2.2d:%-2.2d:%s:%-2.2d"),
		idx->wd, SATELLITE, sptr->fnum,
		sptr->lexid[j], sptr->headword,sptr->headsense);
    else 
	_stprintf(sensekey,_T("%s%%%-1.1d:%-2.2d:%-2.2d::"),
		idx->wd, pos, sptr->fnum, sptr->lexid[j]);

    free_synset(sptr);
    return(_tcsdup(sensekey));
}

/* Search for string and/or baseform of word in database and return
   index structure for word if found in database. */

IndexPtr GetValidIndexPointer(_TCHAR *word, int pos)
{
    IndexPtr idx;
    _TCHAR *morphword;

    idx = getindex(word, pos);

    if (idx == NULL) {
	if ((morphword = morphstr(word, pos)) != NULL)
	    while (morphword) {
		if ((idx = getindex(morphword, pos)) != NULL) break;
		morphword = morphstr(NULL, pos);
	    }
    }
    return (idx);
}

/* Return sense number in database for word and lexsn passed. */

int GetWNSense(_TCHAR *word, _TCHAR *lexsn)
{
    SnsIndexPtr snsidx;
    _TCHAR buf[256];

    _stprintf(buf, _T("%s%%%s"), word, lexsn); /* create sensekey */
    if ((snsidx = GetSenseIndex(buf)) != NULL)
	return(snsidx->wnsense);
    else
	return(0);
}

/* Return parsed sense index entry for sense key passed. */

SnsIndexPtr GetSenseIndex(_TCHAR *sensekey)
{
    _TCHAR *line;
    _TCHAR buf[256], loc[9];
    SnsIndexPtr snsidx = NULL;

    if ((line = bin_search(sensekey, sensefp)) != NULL) {
	snsidx = (SnsIndexPtr)malloc(sizeof(SnsIndex));
	assert(snsidx);
	_stscanf(line, _T("%s %s %d %d\n"),
	       buf,
	       loc,
	       &snsidx->wnsense,
	       &snsidx->tag_cnt);
	snsidx->sensekey = malloc(_tcsclen(buf));
	assert(snsidx->sensekey);
	_tcscpy(snsidx->sensekey, buf);
	snsidx->loc = _ttol(loc);
	/* Parse out word from sensekey to make things easier for caller */
	snsidx->word = _tcsdup(GetWORD(snsidx->sensekey));
	assert(snsidx->word);
	snsidx->nextsi = NULL;
    }
    return(snsidx);
}

/* Return number of times sense is tagged */

int GetTagcnt(IndexPtr idx, int sense) 
{
    _TCHAR *sensekey, *line;
    _TCHAR buf[256];
    int snum, cnt = 0;

    if (cntlistfp) {
      
	sensekey = WNSnsToStr(idx, sense);
	if ((line = bin_search(sensekey, cntlistfp)) != NULL) {
	    _stscanf(line, _T("%s %d %d"), buf, &snum, &cnt);
	}
	free(sensekey);
    }

    return(cnt);
}

void FreeSenseIndex(SnsIndexPtr snsidx)
{
    if (snsidx) {
	free(snsidx->word);
	free(snsidx);
    }
}

_TCHAR *GetOffsetForKey(unsigned int key)
{
    unsigned int rkey;
    _TCHAR ckey[7];
    static _TCHAR loc[11] = _T("");
    _TCHAR *line;
    _TCHAR searchdir[256], tmpbuf[256];

    /* Try to open file in case wn_init wasn't called */

    if (!keyindexfp) {
	_tcscpy(searchdir, SetSearchdir());
	_stprintf(tmpbuf, KEYIDXFILE, searchdir);
	keyindexfp = _tfopen(tmpbuf, _T("r"));
    }
    if (keyindexfp) {
	_stprintf(ckey, _T("%6.6d"), key);
	if ((line = bin_search(ckey, keyindexfp)) != NULL) {
	    _stscanf(line, _T("%d %s"), &rkey, loc);
	    return(loc);
	}
    } 
    return(NULL);
}


unsigned int GetKeyForOffset(_TCHAR *loc)
{
    unsigned int key;
    _TCHAR rloc[11] = _T("");
    _TCHAR *line;
    _TCHAR searchdir[256], tmpbuf[256];

    /* Try to open file in case wn_init wasn't called */

    if (!revkeyindexfp) {
	_tcscpy(searchdir, SetSearchdir());
	_stprintf(tmpbuf, REVKEYIDXFILE, searchdir);
	revkeyindexfp = _tfopen(tmpbuf, _T("r"));
    }
    if (revkeyindexfp) {
	if ((line = bin_search(loc, revkeyindexfp)) != NULL) {
	    _stscanf(line, _T("%s %d"), rloc, &key );
	    return(key);
	}
    }
    return(0);
}

_TCHAR *SetSearchdir()
{
    static _TCHAR searchdir[256];
    _TCHAR *env;

    /* Find base directory for database.  If set, use WNSEARCHDIR.
       If not set, check for WNHOME/dict, otherwise use DEFAULTPATH. */

    if ((env = _tgetenv(_T("WNSEARCHDIR"))) != NULL)
	_tcscpy(searchdir, env);
    else if ((env = _tgetenv(_T("WNHOME"))) != NULL)
	_stprintf(searchdir, _T("%s%s"), env, DICTDIR);
    else
	_tcscpy(searchdir, DEFAULTPATH);

    return(searchdir);
}

int default_display_message(_TCHAR *msg)
{
    return(-1);
}

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

void strstr_init (_TCHAR *string, _TCHAR *word) {
   strstr_word = word;
   strstr_stringstart = string;
   strstr_stringcurrent = string;
}

int strstr_getnext (void) {
   _TCHAR *loc = _tcsstr (strstr_stringcurrent, strstr_word);
   if (loc == NULL) return -1;
   strstr_stringcurrent = loc + 1;
   return (loc - strstr_stringstart);
}

