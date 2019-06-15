/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#ifdef WORDNET

#include "wn.h"


int WNHypnymsToConcept(CG *cg, CONCEPT *concept, _TCHAR *searchStr, _TCHAR *posStr, _TCHAR *ptrStr, int depth);
void WNHypnyms(CG *cg, CONCEPT *concept,SynsetPtr synptr, int ptrtyp, int dbase, int depth);
int WNSensesToConcept(CG *cg, CONCEPT *concept, _TCHAR *searchStr, _TCHAR *posStr);
int WNWordInfo(CG *cg, CONCEPT *concept, _TCHAR *searchStr, IndexPtr idx, int pos);
void WNWordToConcept(CG *cg, CONCEPT *concept, _TCHAR *searchStr, int pos);
int WNPOSFromStr(_TCHAR *posStr);
int WNPointerFromStr(_TCHAR *ptrStr);
void WNWordDefn(CG *cg, CONCEPT *concept, SynsetPtr synptr);
void WNWordSyns(CG *cg, CONCEPT *concept, SynsetPtr synptr, int adjmarker, int antflag);
void WNWord(_TCHAR *wordStr, SynsetPtr synptr, int wdnum, int adjmarker, int antflag);
_TCHAR *WNDeAdjify(_TCHAR *word);
void WNLexicalCategory(_TCHAR *lexCatStr, SynsetPtr synptr);
int WNDepthCheck(int depth, SynsetPtr synptr);

#endif
