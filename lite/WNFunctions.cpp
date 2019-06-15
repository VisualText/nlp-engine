/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/


#include "StdAfx.h"
#include "machine.h"	// 10/25/06 AM.
#ifdef WORDNET

#include "u_out.h"		// 01/19/06 AM.
#include "consh/libconsh.h"
#include "consh/cg.h"
#include "lite/global.h"
#include "dlist.h"
#include "node.h"
#include "parse.h"
#include "WNFunctions.h"

int g_WN_prlexid = 0;
int g_WN_adj_marker = UNKNOWN_MARKER;
static _TCHAR g_WN_msgbuf[256];

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

int WNHypnymsToConcept(CG *cg, CONCEPT *concept, _TCHAR *searchStr, _TCHAR *posStr, _TCHAR *ptrStr, int depth)
{
    int pos = WNPOSFromStr(posStr);
	int ptr = WNPointerFromStr(ptrStr);
    IndexPtr idx = NULL;

    if (!pos || (idx = getindex(searchStr, pos)) == NULL)
		return 0;

    SynsetPtr cursyn;
	int sense = 0;
	CONCEPT *wordConcept = cg->Down(concept);

	while (wordConcept && sense < idx->off_cnt) {
		cursyn = read_synset(pos, idx->offset[sense++], idx->wd);
		WNHypnyms(cg,wordConcept,cursyn, ptr, pos, depth);
		wordConcept = cg->Next(wordConcept);
	}

	free_index(idx);
	return sense;
}

void WNHypnyms(CG *cg, CONCEPT *concept, SynsetPtr synptr, int ptrtyp, int dbase, int depth)
{
    int i;
    int extraindent = 0;
    SynsetPtr cursyn;
 	_TCHAR wordBuf[WORDBUF];
	_TCHAR prefix[40];
	CONCEPT *wordConcept = NULL;

    if (ptrtyp < 0) {
		ptrtyp = -ptrtyp;
		extraindent = 2;
    }
    
    for (i = 0; i < synptr->ptrcount; i++) {
		if ((synptr->ptrtyp[i] == ptrtyp) &&
		   ((synptr->pfrm[i] == 0) ||
			(synptr->pfrm[i] == synptr->whichword))) {

			switch(ptrtyp) {
			case HYPERPTR:
				_stprintf(prefix, _T("HYPERNYM"));
				break;
			case HYPOPTR:
				_stprintf(prefix, _T("HYPONYM"));
				break;
			default:
				_stprintf(prefix, _T("=>"));
				break;
			}

			/* Read synset pointed to */
			cursyn = read_synset(synptr->ppos[i], synptr->ptroff[i], _T(""));

			// MAKE CONCEPT AND ATTRIBUTES

			WNWord(wordBuf,cursyn,0,0,0);
			wordConcept = cg->makeConcept(concept, wordBuf, 0);
			cg->addSval(wordConcept, _T("type"), prefix);

			if (depth) {
				depth = WNDepthCheck(depth, cursyn);
				WNHypnyms(cg,wordConcept,cursyn,ptrtyp,getpos(cursyn->pos),(depth+1));

				free_synset(cursyn);
			}
			else
				free_synset(cursyn);
		}
    }
}

int WNDepthCheck(int depth, SynsetPtr synptr)
{
    if (depth >= MAXDEPTH) {
		_stprintf(g_WN_msgbuf,
			_T("WordNet library error: Error Cycle detected\n   %s\n"),
			synptr->words[0]);
		depth = -1;		/* reset to get one more trace then quit */
    }
    return(depth);
}

int WNSensesToConcept(CG *cg, CONCEPT *concept, _TCHAR *searchStr, _TCHAR *posStr)
{
    int pos = WNPOSFromStr(posStr);
    IndexPtr idx = NULL;
	int count = 0;

    if (pos && (idx = getindex(searchStr, pos)) != NULL)
		count += WNWordInfo(cg,concept,searchStr,idx,pos);

	_TCHAR *morphword;
	if ((morphword = morphstr(searchStr, pos)) != NULL) {
	    while (morphword) {
			if ((idx = getindex(morphword, pos)) != NULL)
				count += WNWordInfo(cg,concept,searchStr,idx,pos);
			morphword = morphstr(NULL, pos);
	    }
    }

	return count;
}

int WNWordInfo(CG *cg, CONCEPT *concept, _TCHAR *searchStr, IndexPtr idx, int pos)
{
    unsigned long offsets[MAXSENSE];
    SynsetPtr cursyn;
	int sense = 0;
	int skipIt = 0;
	int i = 0;
	int offsetCnt = 0;
	int count = 0;
	_TCHAR wordBuf[WORDBUF];
	_TCHAR lexCatBuf[WORDBUF];
	CONCEPT *wordConcept = NULL;

 	count = idx->off_cnt;
	for (i = 0; i < MAXSENSE; i++)
		offsets[i] = 0;

	for (sense = 0; sense < count; sense++) {

		for (i = 0, skipIt = 0; i < offsetCnt && !skipIt; i++)
			if (offsets[i] == idx->offset[sense])
				skipIt = 1;

		if (!skipIt) {
			offsets[offsetCnt++] = idx->offset[sense];
			cursyn = read_synset(pos, idx->offset[sense], idx->wd);

			WNWord(wordBuf,cursyn,0,0,0);
			WNLexicalCategory(lexCatBuf,cursyn);

			// MAKE CONCEPT AND ATTRIBUTES
			wordConcept = cg->makeConcept(concept, wordBuf, 0);
			cg->addSval(wordConcept, _T("original"), searchStr);
			cg->addSval(wordConcept, _T("lexcat"), lexCatBuf);
			WNWordDefn(cg,wordConcept,cursyn);
			WNWordSyns(cg,wordConcept,cursyn,0,0);
			cg->addSval(wordConcept, _T("freq"), GetTagcnt(idx,sense+1));

			free_synset(cursyn);
		}
	}

	free_index(idx);
	return count;
}

int WNPOSFromStr(_TCHAR *posStr)
{
    int pos = 1;

	if (_tcsncicmp(posStr,_T("NOUN"),4) == 0)
		pos = 1;
	else if (_tcsncicmp(posStr,_T("VERB"),4) == 0)
		pos = 2;
	else if (_tcsncicmp(posStr,_T("ADJ"),4) == 0)
		pos = 3;
	else if (_tcsncicmp(posStr,_T("ADV"),4) == 0)
		pos = 4;

	return pos;
}

int WNPointerFromStr(_TCHAR *ptrStr)
{
	int ptr = 1;

	if (_tcsncicmp(ptrStr,_T("ANTPTR"),6) == 0)
		ptr = 1;
	else if(_tcsncicmp(ptrStr,_T("HYPERPTR"),8) == 0)
		ptr = 2;
	else if(_tcsncicmp(ptrStr,_T("HYPOPTR"),7) == 0)
		ptr = 3;
	else if(_tcsncicmp(ptrStr,_T("ENTAILPTR"),9) == 0)
		ptr = 4;
	else if(_tcsncicmp(ptrStr,_T("SIMPTR"),6) == 0)
		ptr = 5;
	else if(_tcsncicmp(ptrStr,_T("ISMEMBERPTR"),11) == 0)
		ptr = 6;
	else if(_tcsncicmp(ptrStr,_T("ISSTUFFPTR "),10) == 0)
		ptr = 7;
	else if(_tcsncicmp(ptrStr,_T("ISPARTPTR"),9) == 0)
		ptr = 8;
	else if(_tcsncicmp(ptrStr,_T("HASMEMBERPTR"),12) == 0)
		ptr = 9;
	else if(_tcsncicmp(ptrStr,_T("HASSTUFFPTR"),11) == 0)
		ptr = 10;
	else if(_tcsncicmp(ptrStr,_T("HASPARTPTR"),10) == 0)
		ptr = 11;
	else if(_tcsncicmp(ptrStr,_T("MERONYM"),7) == 0)
		ptr = 12;
	else if(_tcsncicmp(ptrStr,_T("HOLONYM"),7) == 0)
		ptr = 13;
	else if(_tcsncicmp(ptrStr,_T("CAUSETO"),7) == 0)
		ptr = 14;
	else if(_tcsncicmp(ptrStr,_T("PPLPTR"),6) == 0)
		ptr = 15;
	else if(_tcsncicmp(ptrStr,_T("SEEALSOPTR"),10) == 0)
		ptr = 16;
	else if(_tcsncicmp(ptrStr,_T("PERTPTR"),7) == 0)
		ptr = 17;
	else if(_tcsncicmp(ptrStr,_T("ATTRIBUTE"),9) == 0)
		ptr = 18;
	else if(_tcsncicmp(ptrStr,_T("VERBGROUP"),9) == 0)
		ptr = 19;
	else if(_tcsncicmp(ptrStr,_T("DERIVATION"),10) == 0)
		ptr = 20;
	else if(_tcsncicmp(ptrStr,_T("CLASSIFICATION"),14) == 0)
		ptr = 21;
	else if(_tcsncicmp(ptrStr,_T("CLASS"),5) == 0)
		ptr = 22;

	return ptr;
}

void WNWordDefn(CG *cg, CONCEPT *concept, SynsetPtr synptr)
{
	_TCHAR defn[2000];
	_TCHAR buf[2000];
	_stprintf(defn,_T("%s"),synptr->defn);
	_TCHAR *start = defn;
	_TCHAR *y = defn;
	int len = 0;
	bool exampleFlag = false;
	int exampleCount = 0;

	while (*y) {
		if (*y == '"' && len > 3) {
			if (exampleFlag) {
				if (exampleCount)
					start += 3;
#ifdef LINUX
				_tcsnccpy(buf,++start,len);
#else
				_tcsnccpy_s(buf,++start,len);
#endif
				if (exampleCount)
					buf[len-3] = '\0';
				else
					buf[len] = '\0';
				cg->addSval(concept, _T("example"), buf);
				exampleCount++;
			}
			else {
#ifdef LINUX
				_tcsnccpy(buf,++start,len);
#else
				_tcsnccpy_s(buf,++start,len);
#endif
				buf[len-3] = '\0';
				cg->addSval(concept, _T("defn"), buf);
			}
			start = y;
			exampleFlag = true;
			len = 0;
		}
		else
			len++;
		y++;
	}

	if (!exampleFlag)
		cg->addSval(concept, _T("defn"), defn);
}

void WNWordSyns(CG *cg, CONCEPT *concept, SynsetPtr synptr, int adjmarker, int antflag)
{
	int wordCount = synptr->wcount;
	_TCHAR wordBuf[WORDBUF];

	for (int i=1; i<wordCount; i++) {
		WNWord(wordBuf,synptr,i,adjmarker,antflag);
		cg->addCnode(concept,wordBuf);
	}
}

void WNWord(_TCHAR *wordStr, SynsetPtr synptr, int wdnum, int adjmarker, int antflag)
{
    static _TCHAR vs[] = _T(" (vs. %s)");
    static _TCHAR *markers[] = {
		_T(""),			/* UNKNOWN_MARKER */
		_T("(predicate)"),		/* PREDICATIVE */
		_T("(prenominal)"),		/* ATTRIBUTIVE */
		_T("(postnominal)"),	/* IMMED_POSTNOMINAL */
    };
    
    _TCHAR wordBuf[WORDBUF];
	wordBuf[0] = '\0';
    _tcscpy(wordBuf, synptr->words[wdnum]);
	wordStr[0] = '\0';
    _tcscat(wordStr, WNDeAdjify(wordBuf));

    /* Print additional lexicographer information and WordNet sense
       number as indicated by flags */
	/*
    if (prlexid && (synptr->lexid[wdnum] != 0))
#ifdef LINUX
		sprintf(buf + strlen(buf), "%d", synptr->lexid[wdnum]);
#else
		sprintf_s(buf + strlen(buf), "%d", synptr->lexid[wdnum]);
#endif
    if (wnsnsflag)
#ifdef LINUX
		sprintf(buf + strlen(buf), "#%d", synptr->wnsns[wdnum]);
#else
		sprintf_s(buf + strlen(buf), "#%d", synptr->wnsns[wdnum]);
#endif
	*/

    /* For adjectives, append adjective marker if present, and
       print antonym if flag is passed */

	/*
    if (getpos(synptr->pos) == ADJ) {
		if (adjmarker == PRINT_MARKER)
			strcat(buf, markers[adj_marker]); 
		if (antflag == PRINT_ANTS)
			strcat(buf, printant(ADJ, synptr, wdnum + 1, vs, ""));
    }
	*/
}

_TCHAR *WNDeAdjify(_TCHAR *word)
{
    _TCHAR *y;
    
    g_WN_adj_marker = UNKNOWN_MARKER; /* default if not adj or unknown */
    
    y=word;
    while (*y) {
		if (*y == '(') {
			if (!_tcsncmp(y, _T("(a)"), 3))
				g_WN_adj_marker = ATTRIBUTIVE;
			else if (!_tcsncmp(y, _T("(ip)"), 4))
				g_WN_adj_marker = IMMED_POSTNOMINAL;
			else if (!_tcsncmp(y, _T("(p)"), 3))
				g_WN_adj_marker = PREDICATIVE;
			*y='\0';
		}
		else 
			y++;
    }
    return(word);
}

void WNLexicalCategory(_TCHAR *lexCatStr, SynsetPtr synptr)
{
	_TCHAR wordBuf[WORDBUF];
	_stprintf(wordBuf, _T("%s"), lexfiles[synptr->fnum]);

	_TCHAR *y = wordBuf;

	while (*y) {
		if (*y == '.') {

			_stprintf(lexCatStr, _T("%s"), ++y);
			break;
		}
		else
			y++;
	}
}

#endif
