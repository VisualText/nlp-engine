/********************************************nlpFuncs***********************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	FUNCS.H
* FILE:	lite/funcs.h
* CR:		12/20/01 AM.
* SUBJ:	NLP++ builtin function names.
* NOTE:	Functions must be registered here by the developer.
*			ONLY CALL FROM ONE FILE.
*******************************************************************************/

// MUST KEEP IN SYNC WITH func_defs.h
_TCHAR *nlpFuncs[] = {
_T("FNNULL"),
_T("addarg"),
_T("addattr"),
_T("addcnode"),
_T("addconcept"),
_T("addconval"),
_T("addnode"),
_T("addnumval"),
_T("addstmt"),
_T("addstrs"),
_T("addstrval"),
_T("addsval"),
_T("addword"),
_T("adoptconcepts"),
_T("arraylength"),
_T("attrchange"),
_T("attrexists"),
_T("attrname"),
_T("attrvals"),
_T("attrtype"),
_T("attrwithval"),
_T("batchstart"),
_T("cap"),
_T("cbuf"),					// 05/11/02 AM.
_T("closefile"),
_T("conceptname"),
_T("conceptpath"),
_T("conval"),
_T("cout"),					// 05/05/02 AM.
_T("coutreset"),			// 05/06/02 AM.
_T("dballocstmt"),			// 05/25/02 AM.
_T("dbbindcol"),			// 05/26/02 AM.
_T("dbclose"),				// 05/23/02 AM.
_T("dbexec"),				// 05/23/02 AM.
_T("dbexecstmt"),			// 05/25/02 AM.
_T("dbfetch"),				// 05/26/02 AM.
_T("dbfreestmt"),			// 05/25/02 AM.
_T("dbopen"),				// 05/23/02 AM.
_T("deaccent"),				// 09/07/03 AM.
_T("debug"),
_T("dejunk"),				// 09/09/11 AM.
_T("dictfindword"),
_T("dictfirst"),			// 03/21/03 AM.
_T("dictgetword"),			// 06/29/03 AM.
_T("dictnext"),				// 03/21/03 AM.
_T("DICTphraselookup"),	// 12/15/14 AM.
_T("down"),
//"else",
_T("eltnode"),					// 09/12/06 AM.
_T("excise"),
_T("exitpass"),
_T("exittopopup"),			// 05/24/02 AM.
_T("factorial"),			// Fix.	// 06/23/03 AM.
_T("fail"),
_T("fileout"),
_T("findana"),				// 02/04/03 AM.
_T("findattr"),
_T("findattrs"),
_T("findconcept"),
_T("findhierconcept"),
_T("findnode"),
_T("findphrase"),
_T("findroot"),
_T("findvals"),
_T("findwordpath"),
_T("firstnode"),
_T("flt"),					// 12/21/01 AM.
_T("fltval"),				// 12/27/06 AM.
_T("fncallstart"),
_T("fprintgvar"),
_T("fprintnvar"),
_T("fprintxvar"),
_T("fprintvar"),
_T("gdump"),
_T("getconcept"),
_T("getconval"),
_T("getfltval"),
_T("getnumval"),
_T("getpopupdata"),	// 05/25/02 AM.
_T("getstrval"),
_T("getsval"),
_T("ginc"),
_T("gp"),
_T("group"),
_T("gtolower"),
_T("guniq"),
_T("hitconf"),
//"if",
//"ifelse",
//"ifstmt",
_T("inc"),
_T("inheritval"),
_T("inputrange"),	// 12/15/02 AM.
_T("inputrangetofile"),	// 05/29/03 AM.
_T("interactive"),	// 05/06/02 AM.
_T("kbdumptree"),	// 12/21/01 AM.
_T("lasteltnode"),
_T("lastnode"),
_T("length"),
_T("lengthr"),
_T("levenshtein"),	// 03/06/07 AM.
_T("lextagger"),	// 12/11/03 AM.
_T("listadd"),
_T("listnode"),
_T("LJ"),
_T("lj"),
_T("logten"),		// 04/29/04 AM.
_T("look"),
_T("lookup"),
_T("lowercase"),
_T("makeconcept"),
_T("makeparentconcept"),
_T("makephrase"),
_T("makestmt"),
_T("merge"),
_T("merger"),
_T("mkdir"),		// 12/06/01 AM.
_T("movecleft"),
_T("movecright"),
_T("movesem"),
_T("ndump"),
_T("next"),
_T("nextattr"),
_T("nextval"),
_T("ninc"),
_T("nodeconcept"),
_T("nodeowner"),	// 12/03/02 AM.
_T("noop"),
_T("num"),
_T("numrange"),
_T("numval"),
_T("openfile"),
_T("or"),
_T("pathconcept"),
_T("percentstr"),
_T("permuten"),	// 03/10/04 AM.
_T("phraselength"),
_T("phraseraw"),
_T("phrasetext"),
_T("pncopyvars"),
_T("pndeletechilds"),
_T("pndown"),
_T("pninsert"),	// 07/14/08 AM.
_T("pnmakevar"),
_T("pnmove"),	   // 10/06/13 AM.
_T("pnname"),
_T("pnnext"),
_T("pnprev"),
_T("pnpushval"),	// 12/12/14 AM.
_T("pnremoveval"),
_T("pnrename"),
_T("pnreplaceval"),
_T("pnroot"),
_T("pnrpushval"),	// 12/12/14 AM.
_T("pnsetfired"),
_T("pnsingletdown"),
_T("pnup"),
_T("pnvar"),
_T("pnvarnames"),	// 05/13/02 AM.
_T("pnvartype"),
_T("pranchor"),
_T("prchild"),
_T("preaction"),
_T("printvar"),
_T("prev"),
_T("print"),
_T("printr"),
_T("prlit"),
_T("prrange"),
_T("prtree"),
_T("prunephrases"),
_T("prxtree"),
_T("regexp"),		// 03/23/09 AM.
_T("regexpi"),		// 03/26/09 AM.
_T("renameattr"),
_T("renamechild"),
_T("renameconcept"),
_T("renamenode"),
_T("replaceval"),
_T("resolveurl"),	// 05/14/03 AM.
_T("returnstmt"),	// 03/07/02 AM.
_T("rfaaction"),
_T("rfaactions"),
_T("rfaarg"),
_T("rfaargtolist"),
_T("rfacode"),
_T("rfaelement"),
_T("rfaelt"),
_T("rfaexpr"),
_T("rfalist"),
_T("rfalitelt"),
_T("rfalittoaction"),
_T("rfalittopair"),
_T("rfaname"),
_T("rfanodes"),
_T("rfanonlit"),
_T("rfanonlitelt"),
_T("rfanum"),
_T("rfaop"),
_T("rfapair"),
_T("rfapairs"),
_T("rfapostunary"),
_T("rfapres"),
_T("rfarange"),
_T("rfarecurse"),
_T("rfarecurses"),
_T("rfaregion"),
_T("rfaregions"),
_T("rfarule"),
_T("rfarulelts"),
_T("rfarulemark"),
_T("rfarules"),
_T("rfarulesfile"),
_T("rfaselect"),
_T("rfastr"),
_T("rfasugg"),
_T("rfaunary"),
_T("rfavar"),
_T("rfbarg"),
_T("rfbdecl"),				// 12/19/01 AM.
_T("rfbdecls"),				// 12/19/01 AM.
_T("rightjustifynum"),
_T("rmattr"),
_T("rmattrs"),
_T("rmattrval"),
_T("rmchild"),
_T("rmchildren"),
_T("rmconcept"),
_T("rmcphrase"),
_T("rmnode"),
_T("rmphrase"),
_T("rmval"),
_T("rmvals"),
_T("rmword"),
_T("sdump"),
_T("setbase"),
_T("setlookahead"),	// 12/14/03 AM.
_T("setunsealed"),
_T("single"),
_T("singler"),
_T("singlex"),
_T("singlezap"),
_T("sortconsbyattr"),	// 12/02/02 AM.
_T("sortchilds"),       // 02/20/09 AM.
_T("sorthier"),         // 02/20/09 AM.
_T("sortphrase"),
_T("sortvals"),
_T("spellcandidates"),
_T("spellcorrect"),
_T("spellword"),
_T("splice"),
_T("split"),				// 03/14/03 AM.
_T("sqlstr"),	// 07/22/03 AM.
_T("startout"),
_T("stem"),		// 12/28/01 AM.
_T("stopout"),
_T("str"),
_T("strchar"),
_T("strchr"),
_T("strchrcount"),	// 12/15/02 AM.
_T("strclean"),
_T("strcontains"),
_T("strcontainsnocase"),
_T("strendswith"),
_T("strequal"),
_T("strequalnocase"),
_T("strescape"),
_T("strunescape"),
_T("strgreaterthan"),
_T("strisalpha"),
_T("striscaps"),
_T("strisdigit"),
_T("strislower"),
_T("strisupper"),
_T("strhaspunct"),
_T("strlength"),
_T("strlessthan"),
_T("strnotequal"),
_T("strnotequalnocase"),
_T("strpiece"),
_T("strrchr"),						// FIX.	// 12/21/01 AM.
_T("strspellcandidate"),
_T("strspellcompare"),
_T("strstartswith"),
_T("strsubst"),
_T("strtolower"),
_T("strtotitle"),
_T("strtoupper"),
_T("strtrim"),
_T("struniquechars"),
_T("strval"),
_T("strwrap"),
_T("succeed"),
_T("suffix"),	// 11/23/01 AM.
_T("system"),
_T("take"),
_T("today"),
_T("topdir"),
_T("unknown"),
_T("unpackdirs"),
_T("up"),
_T("uppercase"),
_T("urlbase"),		// 03/16/03 AM.
_T("urltofile"),	// 02/11/03 AM.
_T("var"),			// 06/14/05 AM.
_T("vareq"),		// 07/25/06 AM.
_T("vargt"),
_T("varfn"),
_T("varfnarray"),
_T("varinlist"),
_T("varlt"),
_T("varne"),		// 07/25/06 AM.
_T("varstrs"),
_T("varz"),			// 07/25/06 AM.
#ifdef WORDNET
_T("wninit"),
_T("wnsensestoconcept"),
_T("wnhypnymstoconcept"),
#endif
//"while",
//"whilestmt",
_T("wordindex"),
_T("wordpath"),
_T("writekb"),
_T("xaddlen"),
_T("xaddnvar"),
_T("xdump"),
_T("xinc"),
_T("xmlstr"),		// 05/10/03 AM.
_T("xrename"),
0 };
