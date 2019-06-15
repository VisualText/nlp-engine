
#include "stdafx.h"
#include "Utils.h"
#include "WNFunctions.h"

static _TCHAR searchbuffer[SEARCHBUF];

_TCHAR *partnames[]={ _T(""), _T("noun"), _T("verb"), _T("adj"), _T("adv"), NULL };

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

int fnflag = 0;			/* if set, print lex filename after sense */
int dflag = 1;			/* if set, print definitional glosses */
int saflag = 1;			/* if set, print SEE ALSO pointers */
int fileinfoflag = 1;		/* if set, print lex file info on synsets */
int frflag = 0;			/* if set, print verb frames */
int abortsearch = 0;		/* if set, stop search algorithm */
int offsetflag = 0;		/* if set, print byte offset of each synset */
int wnsnsflag = 0;		/* if set, print WN sense # for each word */
int g_prlexid = 0;
int g_adj_marker = UNKNOWN_MARKER;

void WNPOSSummary(CString &searchStr, CString &outputStr, int pos)
{
	SearchResults wnresults;
    wnresults.numforms = 0;
    wnresults.printcnt = 0;

	_TCHAR searchstr[256];
	_tcscpy(searchstr,searchStr);
	searchstr[searchStr.GetLength()] = '\0';

    SynsetPtr cursyn;
    IndexPtr idx = NULL;
    _TCHAR *cpstring = searchstr, *bufstart;
    int sense, i, offsetcnt;
    unsigned long offsets[MAXSENSE];

    cpstring = searchstr;
    bufstart = searchbuffer;
    for (i = 0; i < MAXSENSE; i++)
		offsets[i] = 0;
    offsetcnt = 0;
	CString str;
	int skipit = 0;
	int svdflag = 0;

    while ((idx = getindex(cpstring, pos)) != NULL) {

		cpstring = NULL;	/* clear for next call to getindex() */
		wnresults.SenseCount[wnresults.numforms++] = idx->off_cnt;
		wnresults.OutSenseCount[wnresults.numforms] = 0;

		/* Print sense summary message */

		//i = wnresults.OutSenseCount[wnresults.numforms];
		i = idx->off_cnt;

		if (i > 0) {
			if (i == 1)
				str.Format(_T("\nThe %s %s has 1 sense"),partnames[pos], idx->wd);
			else
				str.Format(_T("\nThe %s %s has %d senses"),partnames[pos], idx->wd, i);
			outputStr += str;

			if (idx->tagged_cnt > 0)
				str.Format(_T(" (first %d from tagged texts)\n"), idx->tagged_cnt);
			else if (idx->tagged_cnt == 0) 
				str.Format(_T(" (no senses from tagged texts)\n"));

			if (!str.IsEmpty())
				outputStr += str + _T("\r\n");
		}

		/* Print synset for each sense.  If requested, precede
		   synset with synset offset and/or lexical file information.*/

		for (sense = 0; sense < idx->off_cnt; sense++) {

			for (i = 0, skipit = 0; i < offsetcnt && !skipit; i++)
				if (offsets[i] == idx->offset[sense])
					skipit = 1;

			if (!skipit) {
				offsets[offsetcnt++] = idx->offset[sense];
				cursyn = read_synset(pos, idx->offset[sense], idx->wd);

				if (idx->tagged_cnt != -1 && ((sense + 1) <= idx->tagged_cnt))
					str.Format(_T("%d. (%d) "), sense + 1, GetTagcnt(idx, sense + 1));
				else
					str.Format(_T("%d. "), sense + 1);

				svdflag = dflag;
				dflag = 1;
				WNDisplaySynset(outputStr, str, cursyn, _T("\n"), DEFON, ALLWORDS, SKIP_ANTS, SKIP_MARKER);
				dflag = svdflag;

				wnresults.OutSenseCount[wnresults.numforms]++;
				wnresults.printcnt++;

				free_synset(cursyn);
			}
		}

		wnresults.numforms++;
		free_index(idx);
    }
}

void WNDisplaySynset(CString &outputStr, CString headStr, SynsetPtr synptr, CString tailStr, int definition,
					 int wdnum, int antflag, int markerflag)
{
    int i, wdcnt;
    CString str,synsetStr;

	synsetStr.Empty();
	synsetStr += headStr;

    /* Precede synset with additional information as indiecated
       by flags */

    if (offsetflag)	{	/* print synset offset */
		str.Format(_T("{%8.8d} "), synptr->hereiam);
		synsetStr += str;
	}

    if (fileinfoflag) {		/* print lexicographer file information */
		str.Format(_T("<%s> "), lexfiles[synptr->fnum]);
		synsetStr += str;
		g_prlexid = 1;		/* print lexicographer id after word */
    }
	else
		g_prlexid = 0;

    if (wdnum)			/* print only specific word asked for */
		WNDisplayWord(synsetStr, synptr, wdnum - 1, markerflag, antflag);
    else			/* print all words in synset */
		for (i = 0, wdcnt = synptr->wcount; i < wdcnt; i++) {
			WNDisplayWord(synsetStr, synptr, i, markerflag, antflag);
			if (i < wdcnt - 1)
				synsetStr += _T(", ");
		}
    
    if (definition && dflag && synptr->defn) {
		str = _T(" -- ");
		str += synptr->defn;
		synsetStr += str;
    }

    synsetStr += tailStr;

	outputStr += synsetStr;
}

void WNDisplayWord(CString &buffStr, SynsetPtr synptr, int wdnum, int adjmarker, int antflag)
{
    static _TCHAR vs[] = _T(" (vs. %s)");
    static _TCHAR *markers[] = {
		_T(""),			/* UNKNOWN_MARKER */
		_T("(predicate)"),		/* PREDICATIVE */
		_T("(prenominal)"),		/* ATTRIBUTIVE */
		_T("(postnominal)"),	/* IMMED_POSTNOMINAL */
    };

    /* Copy the word (since deadjify() changes original string),
       deadjify() the copy and append to buffer */
    CString str;

    str = WNDeadjify(synptr->words[wdnum]);
    buffStr += str;

    /* Print additional lexicographer information and WordNet sense
       number as indicated by flags */
	
    if (g_prlexid && (synptr->lexid[wdnum] != 0)) {
		str.Format(_T("%d"), synptr->lexid[wdnum]);
		buffStr += str;
	}

    if (wnsnsflag) {
		str.Format(_T("#%d"), synptr->wnsns[wdnum]);
		buffStr += str;
	}

    /* For adjectives, append adjective marker if present, and
       print antonym if flag is passed */

    if (getpos(synptr->pos) == ADJ) {

		if (adjmarker == PRINT_MARKER)
			buffStr += markers[g_adj_marker];

		if (antflag == PRINT_ANTS)
			buffStr += WNDisplayAnt(ADJ, synptr, wdnum + 1, vs, _T(""));
    }
}

CString WNDisplayAnt(int dbase, SynsetPtr synptr, int wdnum, CString formatStr, _TCHAR *tail)
{
    int i, j, wdoff;
    SynsetPtr psynptr;
    _TCHAR tbuf[WORDBUF];
    static _TCHAR retbuf[SMLINEBUF];
    int first = 1;
    
    CString retStr,str;
	retStr.Empty();
    
    /* Go through all the pointers looking for anotnyms from the word
       indicated by wdnum.  When found, print all the antonym's
       antonym pointers which point back to wdnum. */
    
    for (i = 0; i < synptr->ptrcount; i++) {
		if (synptr->ptrtyp[i] == ANTPTR && synptr->pfrm[i] == wdnum) {

			psynptr = read_synset(dbase, synptr->ptroff[i], _T(""));

			for (j = 0; j < psynptr->ptrcount; j++) {
				if (psynptr->ptrtyp[j] == ANTPTR &&
					psynptr->pto[j] == wdnum &&
					psynptr->ptroff[j] == synptr->hereiam) {

					wdoff = (psynptr->pfrm[j] ? (psynptr->pfrm[j] - 1) : 0);

					/* Construct buffer containing formatted antonym,
					   then add it onto end of return buffer */

					str = WNDeadjify(psynptr->words[wdoff]);
					retStr += str;

					/* Print additional lexicographer information and
					   WordNet sense number as indicated by flags */
			
					if (g_prlexid && (psynptr->lexid[wdoff] != 0)) {
						str.Format(_T("%d"),psynptr->lexid[wdoff]);
						retStr += str;
					}
					if (wnsnsflag) {
						str.Format(_T("#%d"),psynptr->wnsns[wdoff]);
						retStr += str;
					}
					if (!first)
						retStr += tail;
					else
						first = 0;
					str.Format(formatStr, tbuf);
					retStr += str;
				}
			}
			free_synset(psynptr);
		}
    }

    return(retStr);
}

void WNDisplayAntSynset(CString &outputStr, SynsetPtr synptr, _TCHAR *tail, int anttype, int definition)
{
    int i, wdcnt;
    int first = 1;

    CString synsetStr,str;
	synsetStr.Empty();

    if (offsetflag) {
		str.Format(_T("{%8.8d} "), synptr->hereiam);
		synsetStr += str;
	}

    if (fileinfoflag) {
		str.Format(_T("<%s> "), lexfiles[synptr->fnum]);
		synsetStr += str;
		g_prlexid = 1;
    }
	else
		g_prlexid = 0;
    
    /* print anotnyms from cluster head (of indirect ant) */
    
    synsetStr +=_T("INDIRECT (VIA ");

    for(i = 0, wdcnt = synptr->wcount; i < wdcnt; i++) {
		if (first) {
			str = WNDisplayAnt(ADJ, synptr, i + 1, _T("%s"), _T(", "));
			first = 0;
		}
		else
			str = WNDisplayAnt(ADJ, synptr, i + 1, _T(", %s"), _T(", "));
		if (!str.IsEmpty())
			synsetStr += str;
    }
    synsetStr += _T(") -> ");
    
    /* now print synonyms from cluster head (of indirect ant) */
    
    for (i = 0, wdcnt = synptr->wcount; i < wdcnt; i++) {
		WNDisplayWord(synsetStr, synptr, i, SKIP_MARKER, SKIP_ANTS);
		if (i < wdcnt - 1)
			synsetStr += _T(", ");
    }
    
    if (dflag && synptr->defn && definition) {
		synsetStr += _T(" -- ");
		synsetStr += synptr->defn;
    }

    synsetStr += tail;

	outputStr += synsetStr;
}

CString WNDeadjify(CString wordStr)
{
    CString newWordStr;
	newWordStr = wordStr;
	int len = 0;
    
    g_adj_marker = UNKNOWN_MARKER; /* default if not adj or unknown */
    
	if (EndsWith(wordStr,_T("(a)"))) {
		g_adj_marker = ATTRIBUTIVE;
		len = 3;
	}
	else if (EndsWith(wordStr,_T("(ip)"))) {
		g_adj_marker = IMMED_POSTNOMINAL;
		len = 4;
	}
	else if (EndsWith(wordStr,_T("(p)"))) {
		g_adj_marker = PREDICATIVE;
		len = 3;
	}

	if (len)
		newWordStr = newWordStr.Mid(0,newWordStr.GetLength()-len);

	newWordStr.Replace('_',' ');

    return newWordStr;
}
