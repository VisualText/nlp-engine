/*
  
  morph.c - WordNet search code morphology functions
  
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "wn.h"

#ifdef UNIX
#define EXCFILE	_T("%s/%s.exc")
#endif
#ifdef PC
#define EXCFILE	_T("%s\\%s.exc")
#endif
#ifdef MAC
#define EXCFILE	_T("%s:%s.exc")
#endif

static _TCHAR *Id = _T("$Id: morph.c,v 1.61 2003/06/23 16:15:39 wn Exp $");

static _TCHAR *sufx[] ={ 
    /* Noun suffixes */
    _T("s"), _T("ses"), _T("xes"), _T("zes"), _T("ches"), _T("shes"), _T("men"), _T("ies"),
    /* Verb suffixes */
    _T("s"), _T("ies"), _T("es"), _T("es"), _T("ed"), _T("ed"), _T("ing"), _T("ing"),
    /* Adjective suffixes */
    _T("er"), _T("est"), _T("er"), _T("est")
};

static _TCHAR *addr[] ={ 
    /* Noun endings */
    _T(""), _T("s"), _T("x"), _T("z"), _T("ch"), _T("sh"), _T("man"), _T("y"),
    /* Verb endings */
    _T(""), _T("y"), _T("e"), _T(""), _T("e"), _T(""), _T("e"), _T(""),
    /* Adjective endings */
    _T(""), _T(""), _T("e"), _T("e")
};

static int offsets[NUMPARTS] = { 0, 0, 8, 16 };
static int cnts[NUMPARTS] = { 0, 8, 8, 4 };
static _TCHAR msgbuf[256];

#define NUMPREPS	15

static struct {
    _TCHAR *str;
    int strlen;
} prepositions[NUMPREPS] = {
    _T("to"), 2,
    _T("at"), 2,
    _T("of"), 2,
    _T("on"), 2,
    _T("off"), 3,
    _T("in"), 2,
    _T("out"), 3,
    _T("up"), 2,
    _T("down"), 4,
    _T("from"), 4,
    _T("with"), 4,
    _T("into"), 4,
    _T("for"), 3,
    _T("about"), 5,
    _T("between"), 7,
};

static FILE *exc_fps[NUMPARTS + 1];

static int do_init();
static int strend(_TCHAR *, _TCHAR *);
static _TCHAR *wordbase(_TCHAR *, int);
static int hasprep(_TCHAR *, int);
static _TCHAR *exc_lookup(_TCHAR *, int);
static _TCHAR *morphprep(_TCHAR *);

/* Open exception list files */

int morphinit(void)
{
    static int done = 0;
    static int openerr = 0;

    if (!done) {
      if (OpenDB) {		/* make sure WN database files are open */
            if (!(openerr = do_init()))
	        done = 1;
	} else
	    openerr = -1;
    }

    return(openerr);
}

/* Close exception list files and reopen */
int re_morphinit(void)
{
    int i;

    for (i = 1; i <= NUMPARTS; i++) {
	if (exc_fps[i] != NULL) {
	    fclose(exc_fps[i]); exc_fps[i] = NULL;
	}
    }

    return(OpenDB ? do_init() : -1);
}

static int do_init(void)
{
    int i, openerr;
    _TCHAR *env;
    _TCHAR searchdir[256], fname[256];

    openerr = 0;

    /* Find base directory for database.  If set, use WNSEARCHDIR.
       If not set, check for WNHOME/dict, otherwise use DEFAULTPATH. */

    if ((env = _tgetenv(_T("WNSEARCHDIR"))) != NULL)
	_tcscpy(searchdir, env);
    else if ((env = _tgetenv(_T("WNHOME"))) != NULL)
	_stprintf(searchdir, _T("%s%s"), env, DICTDIR);
    else
	_tcscpy(searchdir, DEFAULTPATH);

    for (i = 1; i <= NUMPARTS; i++) {
	_stprintf(fname, EXCFILE, searchdir, partnames[i]);
	if ((exc_fps[i] = _tfopen(fname, _T("r"))) == NULL) {
	    _stprintf(msgbuf,
		    _T("WordNet library error: Can't open exception file(%s)\n\n"),
		    fname);
	    display_message(msgbuf);
	    openerr = -1;
	}
    }
    return(openerr);
}

/* Try to find baseform (lemma) of word or collocation in POS.
   Works like strtok() - first call is with string, subsequent calls
   with NULL argument return additional baseforms for original string. */

_TCHAR *morphstr(_TCHAR *origstr, int pos)
{
    static _TCHAR searchstr[WORDBUF], str[WORDBUF];
    static int svcnt, svprep;
    _TCHAR word[WORDBUF], *tmp;
    int cnt, st_idx = 0, end_idx;
    int prep;
    _TCHAR *end_idx1, *end_idx2;
    _TCHAR *append;
    
    if (pos == SATELLITE)
	pos = ADJ;

    /* First time through for this string */

    if (origstr != NULL) {
	/* Assume string hasn't had spaces substitued with '_' */
	strtolower(strsubst(_tcscpy(str, origstr), ' ', '_'));
	searchstr[0] = '\0';
	cnt = cntwords(str, '_');
	svprep = 0;

	/* first try exception list */

	if ((tmp = exc_lookup(str, pos)) && _tcscmp(tmp, str)) {
	    svcnt = 1;		/* force next time to pass NULL */
	    return(tmp);
	}

	/* Then try simply morph on original string */

	if (pos != VERB && (tmp = morphword(str, pos)) && _tcscmp(tmp, str))
	    return(tmp);

	if (pos == VERB && cnt > 1 && (prep = hasprep(str, cnt))) {
	    /* assume we have a verb followed by a preposition */
	    svprep = prep;
	    return(morphprep(str));
	} else {
	    svcnt = cnt = cntwords(str, '-');
	    while (origstr && --cnt) {
		end_idx1 = _tcschr(str + st_idx, '_');
		end_idx2 = _tcschr(str + st_idx, '-');
		if (end_idx1 && end_idx2) {
		    if (end_idx1 < end_idx2) {
			end_idx = (int)(end_idx1 - str);
			append = _T("_");
		    } else {
			end_idx = (int)(end_idx2 - str);
			append = _T("-");
		    }
		} else {
		    if (end_idx1) {
			end_idx = (int)(end_idx1 - str);
			append = _T("_");
		    } else {
			end_idx = (int)(end_idx2 - str);
			append = _T("-");
		    }
		}	
		if (end_idx < 0) return(NULL);		/* shouldn't do this */
		_tcsnccpy(word, str + st_idx, end_idx - st_idx);
		word[end_idx - st_idx] = '\0';
		if(tmp = morphword(word, pos))
		    _tcscat(searchstr,tmp);
		else
		    _tcscat(searchstr,word);
		_tcscat(searchstr, append);
		st_idx = end_idx + 1;
	    }
	    
	    if(tmp = morphword(_tcscpy(word, str + st_idx), pos)) 
		_tcscat(searchstr,tmp);
	    else
		_tcscat(searchstr,word);
	    if(_tcscmp(searchstr, str) && is_defined(searchstr,pos))
		return(searchstr);
	    else
		return(NULL);
	}
    } else {			/* subsequent call on string */
	if (svprep) {		/* if verb has preposition, no more morphs */
	    svprep = 0;
	    return(NULL);
	} else if (svcnt == 1)
	    return(exc_lookup(NULL, pos));
	else {
	    svcnt = 1;
	    if ((tmp = exc_lookup(str, pos)) && _tcscmp(tmp, str))
		return(tmp);
	    else
		return(NULL);
	}
    }
}

/* Try to find baseform (lemma) of individual word in POS */
_TCHAR *morphword(_TCHAR *word, int pos)
{
    int offset, cnt;
    int i;
    static _TCHAR retval[WORDBUF];
    _TCHAR *tmp, tmpbuf[WORDBUF], *end;
    
    _stprintf(retval,_T(""));
    _stprintf(tmpbuf, _T(""));
    end = _T("");
    
    if(word == NULL) 
	return(NULL);

    /* first look for word on exception list */
    
    if((tmp = exc_lookup(word, pos)) != NULL)
	return(tmp);		/* found it in exception list */

    if (pos == ADV) {		/* only use exception list for adverbs */
	return(NULL);
    }
    if (pos == NOUN) {
	if (strend(word, _T("ful"))) {
	    cnt = _tcsrchr(word, 'f') - word;
	    _tcsncat(tmpbuf, word, cnt);
	    end = _T("ful");
	} else 
	    /* check for noun ending with 'ss' or short words */
	    if (strend(word, _T("ss")) || (_tcsclen(word) <= 2))
		return(NULL);
    }

/* If not in exception list, try applying rules from tables */

    if (tmpbuf[0] == '\0')
	_tcscpy(tmpbuf, word);

    offset = offsets[pos];
    cnt = cnts[pos];

    for(i = 0; i < cnt; i++){
	_tcscpy(retval, wordbase(tmpbuf, (i + offset)));
	if(_tcscmp(retval, tmpbuf) && is_defined(retval, pos)) {
	    _tcscat(retval, end);
	    return(retval);
	}
    }
    return(NULL);
}

static int strend(_TCHAR *str1, _TCHAR *str2)
{
    _TCHAR *pt1;
    
    if(_tcsclen(str2) >= _tcsclen(str1))
	return(0);
    else {
	pt1=str1;
	pt1=_tcschr(str1,0);
	pt1=pt1-_tcsclen(str2);
	return(!_tcscmp(pt1,str2));
    }
}

static _TCHAR *wordbase(_TCHAR *word, int ender)
{
    _TCHAR *pt1;
    static _TCHAR copy[WORDBUF];
    
    _tcscpy(copy, word);
    if(strend(copy,sufx[ender])) {
	pt1=_tcschr(copy,'\0');
	pt1 -= _tcsclen(sufx[ender]);
	*pt1='\0';
	_tcscat(copy,addr[ender]);
    }
    return(copy);
}

static int hasprep(_TCHAR *s, int wdcnt)
{
    /* Find a preposition in the verb string and return its
       corresponding word number. */

    int i, wdnum;

    for (wdnum = 2; wdnum <= wdcnt; wdnum++) {
	s = _tcschr(s, '_');
	for (s++, i = 0; i < NUMPREPS; i++)
	    if (!_tcsncmp(s, prepositions[i].str, prepositions[i].strlen) &&
		(s[prepositions[i].strlen] == '_' ||
		 s[prepositions[i].strlen] == '\0'))
		return(wdnum);
    }
    return(0);
}
 
static _TCHAR *exc_lookup(_TCHAR *word, int pos)
{
    static _TCHAR line[WORDBUF], *beglp, *endlp;
    _TCHAR *excline;
    int found = 0;

    if (exc_fps[pos] == NULL)
	return(NULL);

    /* first time through load line from exception file */
    if(word != NULL){
	if ((excline = bin_search(word, exc_fps[pos])) != NULL) {
	    _tcscpy(line, excline);
	    endlp = _tcschr(line,' ');
	} else
	    endlp = NULL;
    }
    if(endlp && *(endlp + 1) != ' '){
	beglp = endlp + 1;
	while(*beglp && *beglp == ' ') beglp++;
	endlp = beglp;
	while(*endlp && *endlp != ' ' && *endlp != '\n') endlp++;
	if(endlp != beglp){
	    *endlp='\0';
	    return(beglp);
	}
    }
    beglp = NULL;
    endlp = NULL;
    return(NULL);
}

static _TCHAR *morphprep(_TCHAR *s)
{
    _TCHAR *rest, *exc_word, *lastwd = NULL, *last;
    int i, offset, cnt;
    _TCHAR word[WORDBUF], end[WORDBUF];
    static _TCHAR retval[WORDBUF];

    /* Assume that the verb is the first word in the phrase.  Strip it
       off, check for validity, then try various morphs with the
       rest of the phrase tacked on, trying to find a match. */

    rest = _tcschr(s, '_');
    last = _tcsrchr(s, '_');
    if (rest != last) {		/* more than 2 words */
	if (lastwd = morphword(last + 1, NOUN)) {
	    _tcsnccpy(end, rest, last - rest + 1);
	    end[last-rest+1] = '\0';
	    _tcscat(end, lastwd);
	}
    }
    
    _tcsnccpy(word, s, rest - s);
    word[rest - s] = '\0';
    for (i = 0, cnt = _tcsclen(word); i < cnt; i++)
	if (!_istalnum((_TUCHAR)(word[i]))) return(NULL);

    offset = offsets[VERB];
    cnt = cnts[VERB];

    /* First try to find the verb in the exception list */

    if ((exc_word = exc_lookup(word, VERB)) &&
	_tcscmp(exc_word, word)) {

	_stprintf(retval, _T("%s%s"), exc_word, rest);
	if(is_defined(retval, VERB))
	    return(retval);
	else if (lastwd) {
	    _stprintf(retval, _T("%s%s"), exc_word, end);
	    if(is_defined(retval, VERB))
		return(retval);
	}
    }
    
    for (i = 0; i < cnt; i++) {
	if ((exc_word = wordbase(word, (i + offset))) &&
	    _tcscmp(word, exc_word)) { /* ending is different */

	    _stprintf(retval, _T("%s%s"), exc_word, rest);
	    if(is_defined(retval, VERB))
		return(retval);
	    else if (lastwd) {
		_stprintf(retval, _T("%s%s"), exc_word, end);
		if(is_defined(retval, VERB))
		    return(retval);
	    }
	}
    }
    _stprintf(retval, _T("%s%s"), word, rest);
    if (_tcscmp(s, retval))
	return(retval);
    if (lastwd) {
	_stprintf(retval, _T("%s%s"), word, end);
	if (_tcscmp(s, retval))
	    return(retval);
    }
    return(NULL);
}

/* 
 * Revision 1.1  91/09/25  15:39:47  wn
 * Initial revision
 * 
 */
