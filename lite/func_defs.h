/*******************************************************************************
Copyright (c) 2001-2010 by Text Analysis International, Inc.
All rights reserved.
********************************************************************************
*
* NAME:	FUNC_DEFS.H
* FILE:	lite/func_defs.h
* CR:		12/20/01 AM.
* SUBJ:	NLP++ builtin function declares.
* NOTE:	Functions must be registered here by the developer.
*******************************************************************************/

#ifndef FUNC_DEFS_H_
#define FUNC_DEFS_H_

extern _TCHAR *nlpFuncs[];

// MUST KEEP IN SYNC WITH funcs.h
// ASSUMPTION: Values are 0, 1, 2, etc.
enum funcDef
	{
	FNNULL,
	FNaddarg,
	FNaddattr,
	FNaddcnode,
	FNaddconcept,
	FNaddconval,
	FNaddnode,
	FNaddnumval,
	FNaddstmt,
	FNaddstrs,
	FNaddstrval,
	FNaddsval,
	FNaddword,
	FNadoptconcepts,
	FNarraylength,
	FNattrchange,
	FNattrexists,
	FNattrname,
	FNattrvals,
	FNattrtype,
	FNattrwithval,
	FNbatchstart,
	FNcap,
	FNcbuf,					// 05/11/02 AM.
	FNclosefile,
	FNconceptname,
	FNconceptpath,
	FNconval,
	FNcout,					// 05/05/02 AM.
	FNcoutreset,			// 05/06/02 AM.
	FNdballocstmt,			// 05/25/02 AM.
	FNdbbindcol,			// 05/26/02 AM.
	FNdbclose,				// 05/23/02 AM.
	FNdbexec,				// 05/23/02 AM.
	FNdbexecstmt,			// 05/25/02 AM.
	FNdbfetch,				// 05/26/02 AM.
	FNdbfreestmt,			// 05/25/02 AM.
	FNdbopen,				// 05/23/02 AM.
	FNdeaccent,				// 09/07/03 AM.
	FNdebug,
	FNdejunk,				// 09/09/11 AM.
	FNdictfindword,
	FNdictfirst,			// 03/21/03 AM.
	FNdictgetword,			// 06/29/03 AM.
	FNdictnext,				// 03/21/03 AM.
	FNDICTphraselookup,	// 12/15/14 AM.
	FNdown,
	//else,
	FNeltnode,				// 09/12/06 AM.
	FNexcise,
	FNexitpass,
	FNexittopopup,			// 05/24/02 AM.
	FNfactorial,
	FNfail,
	FNfileout,
	FNfindana,				// 02/04/03 AM.
	FNfindattr,
	FNfindattrs,
	FNfindconcept,
	FNfindhierconcept,
	FNfindnode,
	FNfindphrase,
	FNfindroot,
	FNfindvals,
	FNfindwordpath,
	FNfirstnode,
	FNflt,				// 12/21/01 AM.
	FNfltval,			// 12/27/06 AM.
	FNfncallstart,
	FNfprintgvar,
	FNfprintnvar,
	FNfprintxvar,
	FNfprintvar,
	FNgdump,
	FNgetconcept,
	FNgetconval,
	FNgetfltval,
	FNgetnumval,
	FNgetpopupdata,	// 05/25/02 AM.
	FNgetstrval,
	FNgetsval,
	FNginc,
	FNgp,
	FNgroup,
	FNgtolower,
	FNguniq,
	FNhitconf,
	//if,
	//ifelse,
	//ifstmt,
	FNinc,
	FNinheritval,
	FNinputrange,	// 12/15/02 AM.
	FNinputrangetofile,	// 05/29/03 AM.
	FNinteractive,	// 05/06/02 AM.
	FNkbdumptree,	// 12/21/01 AM.
	FNlasteltnode,
	FNlastnode,
	FNlength,
	FNlengthr,
	FNlevenshtein,	// 03/06/07 AM.
	FNlextagger,
	FNlistadd,
	FNlistnode,
	FNLJ,
	FNlj,
	FNlogten,	// 04/29/04 AM.
	FNlook,
	FNlookup,
	FNlowercase,
	FNmakeconcept,
	FNmakeparentconcept,
	FNmakephrase,
	FNmakestmt,
	FNmerge,
	FNmerger,
	FNmkdir,		// 12/06/01 AM.
	FNmovecleft,
	FNmovecright,
	FNmovesem,
	FNndump,
	FNnext,
	FNnextattr,
	FNnextval,
	FNninc,
	FNnodeconcept,
	FNnodeowner,	// 12/03/02 AM.
	FNnoop,
	FNnum,
	FNnumrange,
	FNnumval,
	FNopenfile,
	FNor,
	FNpathconcept,
	FNpercentstr,
	FNpermuten,		// 03/10/04 AM.
	FNphraselength,
	FNphraseraw,
	FNphrasetext,
	FNpncopyvars,
	FNpndeletechilds,
	FNpndown,
	FNpninsert,		// 07/14/08 AM.
	FNpnmakevar,
	FNpnmove,		// 10/06/13 AM.
	FNpnname,
	FNpnnext,
	FNpnprev,
	FNpnpushval,	// 12/12/14 AM.
	FNpnremoveval,
	FNpnrename,
	FNpnreplaceval,
	FNpnroot,
	FNpnrpushval,	// 12/12/14 AM.
	FNpnsetfired,
	FNpnsingletdown,
	FNpnup,
	FNpnvar,
	FNpnvarnames,	// 05/13/02 AM.
	FNpnvartype,
	FNpranchor,
	FNprchild,
	FNpreaction,
	FNprintvar,
	FNprev,
	FNprint,
	FNprintr,
	FNprlit,
	FNprrange,
	FNprtree,
	FNprunephrases,
	FNprxtree,
	FNregexp,			// 03/23/09 AM.
	FNregexpi,			// 03/26/09 AM.
	FNrenameattr,
	FNrenamechild,
	FNrenameconcept,
	FNrenamenode,
	FNreplaceval,
	FNresolveurl,		// 05/14/03 AM.
	FNrfareturnstmt,	// 03/07/02 AM.
	FNrfaaction,
	FNrfaactions,
	FNrfaarg,
	FNrfaargtolist,
	FNrfacode,
	FNrfaelement,
	FNrfaelt,
	FNrfaexpr,
	FNrfalist,
	FNrfalitelt,
	FNrfalittoaction,
	FNrfalittopair,
	FNrfaname,
	FNrfanodes,
	FNrfanonlit,
	FNrfanonlitelt,
	FNrfanum,
	FNrfaop,
	FNrfapair,
	FNrfapairs,
	FNrfapostunary,
	FNrfapres,
	FNrfarange,
	FNrfarecurse,
	FNrfarecurses,
	FNrfaregion,
	FNrfaregions,
	FNrfarule,
	FNrfarulelts,
	FNrfarulemark,
	FNrfarules,
	FNrfarulesfile,
	FNrfaselect,
	FNrfastr,
	FNrfasugg,
	FNrfaunary,
	FNrfavar,
	FNrfbarg,
	FNrfbdecl,				// 12/19/01 AM.
	FNrfbdecls,				// 12/19/01 AM.
	FNrightjustifynum,
	FNrmattr,
	FNrmattrs,
	FNrmattrval,
	FNrmchild,
	FNrmchildren,
	FNrmconcept,
	FNrmcphrase,
	FNrmnode,
	FNrmphrase,
	FNrmval,
	FNrmvals,
	FNrmword,
	FNsdump,
	FNsetbase,
	FNsetlookahead,	// 12/14/03 AM.
	FNsetunsealed,
	FNsingle,
	FNsingler,
	FNsinglex,
	FNsinglezap,
	FNsortconsbyattr,	// 12/02/02 AM.
	FNsortchilds,     // 02/20/09 AM.
	FNsorthier,       // 02/20/09 AM.
	FNsortphrase,
	FNsortvals,
	FNspellcandidates,
	FNspellcorrect,
	FNspellword,
	FNsplice,
	FNsplit,				// 03/14/03 AM.
	FNsqlstr,	// 07/22/03 AM.
	FNstartout,
	FNstem,		// 12/28/01 AM.
	FNstopout,
	FNstr,
	FNstrchar,
	FNstrchr,
	FNstrchrcount,		// 12/15/02 AM.
	FNstrclean,
	FNstrcontains,
	FNstrcontainsnocase,
	FNstrendswith,
	FNstrequal,
	FNstrequalnocase,
	FNstrescape,
	FNstrunescape,
	FNstrgreaterthan,
	FNstrisalpha,
	FNstriscaps,
	FNstrisdigit,
	FNstrislower,
	FNstrisupper,
	FNstrhaspunct,
	FNstrlength,
	FNstrlessthan,
	FNstrnotequal,
	FNstrnotequalnocase,
	FNstrpiece,
	FNstrrchr,
	FNstrspellcandidate,
	FNstrspellcompare,
	FNstrstartswith,
	FNstrsubst,
	FNstrtolower,
	FNstrtotitle,
	FNstrtoupper,
	FNstrtrim,
	FNstruniquechars,
	FNstrval,
	FNstrwrap,
	FNsucceed,
	FNsuffix,	// 11/23/01 AM.
	FNsystem,
	FNtake,
	FNtoday,
	FNtopdir,
	FNunknown,
	FNunpackdirs,
	FNup,
	FNuppercase,
	FNurlbase,		// 03/16/03 AM.
	FNurltofile,	// 02/11/03 AM.
	FNvar,			// 06/14/05 AM.
	FNvareq,			// 07/25/06 AM.
	FNvarfn,
	FNvarfnarray,
	FNvarinlist,
	FNvarne,			// 07/25/06 AM.
	FNvarstrs,
	FNvarz,			// 07/25/06 AM.
#ifdef WORDNET
	FNWNInit,		// 07/07/04 Dd.
	FNWNSensesToConcept,	// 07/07/04 Dd.
	FNWNHypnymsToConcept,	// 07/07/04 Dd.
#endif
	//while,
	//whilestmt,
	FNwordindex,
	FNwordpath,
	FNWriteKB,
	FNxaddlen,
	FNxaddnvar,
	FNxdump,
	FNxinc,
	FNxmlstr,		// 05/10/03 AM.
	FNxrename,

	FNXXYYZZ };

#endif
