// Automatically generated: Tue Nov 07 15:42:21 2006
#include "analyzer.h"
#include "ehead.h"
#include "rhead.h"
#include "data.h"
#include "..\user\user.h"
#include "..\user\Ucode.h"
bool run_analyzer(Parse *parse){if (!Arun::init_analyzer(parse)) return false;tokenize1(parse);patExecute2(parse);return true;}bool tokenize1(Parse *parse){if (!Arun::tokenize(parse,1)) return false;return true;}// CODE GENERATION FOR PASS 2.

bool code2(Nlppp *nlppp){Arun::fnstart(nlppp);Arun::stmt(Arun::assign(5, _T("hello"), 0, -1, nlppp, ((long)0)));Arun::stmt(Arun::out(Arun::out(_T("output.txt"), _T("Printing from code region."),nlppp), _T("\n"),nlppp));Arun::ret(nlppp,0,(RFASem*)0);return true;}bool matchRule2(int ruleno,bool &done,Nlppp *nlppp){NODE *pcoll=0;switch (ruleno)	{	case 1:		/* _blah <- Linux @@ */		Arun::init_rule(1,sugg2_0_1,elts2_0_1,0,0,nlppp);		if (!Arun::match_right(1,Arun::match_simple,nlppp)) return false;		Arun::matched_rule(nlppp,true);
		Arun::stmt(Arun::out(Arun::out(_T("output.txt"), _T("Found Linux"),nlppp), _T("\n"),nlppp));		Arun::stmt(Arun::single(nlppp));		if (Arun::post_final(nlppp)) return true;		break;	default:	done = true;	return false;	}return false;}bool patExecute2(Parse *parse){return Arun::patExecute(parse,&pass2_0,2,_T("stuff"));}