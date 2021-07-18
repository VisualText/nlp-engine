###############################################
# FILE: pos25.pat
# SUBJ: Pre-recursive parsing.
# AUTH: AM
# CREATED: 06/Dec/05 20:22:08
# NOTE:	Now that sentence segmentation precedes the recursive
#	pos50 monster, we want a nonrecursive pass to follow
#	sentence formation and precede pos50.  Mainly
#	as a performance enhancer, since pos50 is slow.
###############################################

@CODE
L("hello") = 0;
#"metric.txt" << G("$passnum") << "\n";
@@CODE

#@PATH _ROOT _TEXTZONE _sent
@NODES _sent


# dqa alpha alpha
@PRE
<5,5> var("noun");

@CHECK
  if (!N(1) && !N(2))
 	fail();
  if (!N("adj",4) && !N("noun",4))
	fail();
@POST
  fixnphead(5);
  fixnpnonhead(4);
  if (pnname(N(4)) == "_adj")
	L("nn") = 5;
  else
	L("nn") = 4;
  if (pnname(N(4)) == "_adj" && !N(3))
	L("jj") = 4;
  else
	L("jj") = 3;
  dqaninfo(1,2,L("jj"),L("nn"));
  groupnp();
@RULES
_xNIL <-
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# those alpha to
@POST
  if (N("noun",2) && plural(N(2)))
    fixnphead(2);
  else if (N("verb",2))
    alphatoverb(2,0,"VBP");
  else if (N("noun",2))
  	fixnphead(2);
  else if (N("adj",2))
  	alphatoadj(2);
  else if (N("adv",2))
    alphatoadv(2);
@RULES
_xNIL <-
	those [s]
	_xALPHA
	to [s lookahead]
	@@

# det alpha
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  L("num") = number(L("tmp2"));
  if (L("num"))
    N("number",2) = L("num");
  L("n") = N(2);
  pncopyvars(L("tmp2"),N(2));
  group(1,2,"_np");
  pncopyvars(L("n"),N(1));
  clearpos(N(1),1,1);
@RULES
_xNIL <-
	_det
	_xALPHA
	_xWILD [one lookahead match=(_xEND _qEOS)]
	@@

# det alpha
# NIBBLE.
@CHECK
  if (N("unknown",3))
    succeed();
  if (N("number",1) == "singular" && plural(N(3)))
	fail();
  if (!N("verb",3))
    fail();
  if (N("pos num",3) < 2)
    fail();
@POST
  N("pos25 det-a-a") = 1;
  if (N("unknown",3))
    alphatonoun(3);
  else
    {
	if (vconjq(N(3),"-en") && !N("noun",3) && !N("adj",3))
		L("pos") = "VBN";
	N("verb",3) = 0;	# verb = 0
	--N("pos num",3);
	if (N("noun",3) && N("pos num",3) == 1)
 		alphatonoun(3);	# 04/21/07 AM.
	if (L("pos"))
		N("mypos",3) = L("pos");
	}
@RULES
_xNIL <-
	_det
	_adv [star]
	_xALPHA
	@@

# det alpha
# NIBBLE.
@CHECK
  if (N("unknown",2))
    succeed();
  if (!N("verb",2))
    fail();
  if (N("pos num",2) < 2)
    fail();
  if (vconjq(N(2),"-ing")
   || vconjq(N(2),"-en"))
    fail();
@POST

  if (N("unknown",2))
    alphatonoun(2);
  else if (N("adj",2) && N("noun",2))
    fixnpnonhead(2);
  else
    {
	N("verb",2) = 0;	# verb = 0
	--N("pos num",2);
	alphaunambigred(2);
	}
@RULES
_xNIL <-
	_xWILD [s one match=(while)]
	_xALPHA
	@@

# det alpha [NIBBLE]
@CHECK
  if (N("noun",3) || !N("adj",3))
    fail();
@POST
  alphatoadj(3);
@RULES
_xNIL <-
	_det
	_adv [star]
	_xALPHA
	@@

# alpha verb
# alpha modal
# alpha mhbv
@CHECK
  if (!N("noun",3))
    fail();
  # Todo: Check helping or multi-verb stuff here.
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
  nountonp(3,1);
#  semcomposenp(N(3),L("tmp3"),L("tmp3"));
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [one lookahead match=(_modal _have _be _verb)]
	@@

# mhbv conj alpha
@PRE
<11,11> var("verb");
@CHECK
  if (!N(1) && !N(3) && !N(5) && !N(7))
  	fail();
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(9));
  if (!L("arr")[0])
    fail();
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(11));
  if (!L("arr")[0])
    fail();
@POST
  L("m") = N(1);
  L("h") = N(3);
  L("b") = N(5);
  L("being") = N(7);
  L("v") = N(9);

  if (N(7))
    {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
    }

  alphatovg(11,0,0);
  L("neg") = mhbvadv(2,4,6,8);
  group(1,9,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));

  # Note: everything renumbered, vg conj vg ...
  L("v2") = N("verb node",3);
  mhbv(N(3),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v2"));
@RULES
_xNIL <-
    _xWILD [s star match=(_modal _do)]						### (1)
    _xWILD [star match=(_adv _advl)]						### (2)
    _have [s opt]											### (3)
    _xWILD [star match=(_adv _advl)]						### (4)
    _be [s opt]												### (5)
    _xWILD [star match=(_adv _advl)]						### (6)
    being [s opt]											### (7)
    _xWILD [star match=(_adv _advl)]						### (8)
    _xWILD [s one match=(_verb) except=(_modal _have _be _vg)]	### (9)
    _conj													### (10)
    _xALPHA													### (11)
    @@

# mhbv
# Complete vg.
@CHECK
  if (pnname(N(9)) != "_verb" && !N("verb",9))
	fail();
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(9));
  if (!L("arr")[0])
    fail();
@POST
  if (pnname(N(9)) != "_verb")
 	alphatoverb(9,0,0);
  L("m") = N(1);
  L("h") = N(3);
  L("b") = N(5);
  L("being") = N(7);
  L("v") = N(9);

  if (N(7))
    {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
    }

  L("neg") = mhbvadv(2,4,6,8);
  group(1,9,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
    _xWILD [s one match=(_modal)]								### (1)
    _xWILD [star match=(_adv _advl)]							### (2)
    _have [s opt]												### (3)
    _xWILD [star match=(_adv _advl)]							### (4)
    _be [s opt]													### (5)
    _xWILD [star match=(_adv _advl)]							### (6)
    being [s opt]												### (7)
    _xWILD [star match=(_adv _advl)]							### (8)
    _xWILD [s one match=(_verb _xALPHA) except=(_modal _have _be _vg)]	### (9)
    @@

# mhbv
@CHECK
  if (pnname(N(7)) != "_verb" && !N("verb",7))
	fail();
  # Agreement...
  L("arr") = vgagree(0,N(1),N(3),N(5),N(7));
  if (!L("arr")[0])
    fail();
@POST
  if (pnname(N(7)) != "_verb")
	alphatoverb(7,0,0);
  L("m") = 0;
  L("h") = N(1);
  L("b") = N(3);
  L("being") = N(5);
  L("v") = N(7);

  if (N(5))
    {
    N("sem",5) = N("stem",5) = "be";
    chpos(N(5),"VBG");
    }

  L("neg") = mhbvadv(2,4,6,0);
  group(1,7,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_have [s]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(_verb _xALPHA) except=(_modal _have _be _vg)]
	@@

# mhbv
@CHECK
  if (pnname(N(5)) != "_verb" && !N("verb",5))
	fail();
  # Agreement...
  L("arr") = vgagree(0,0,N(1),N(3),N(5));
  if (!L("arr")[0])
    fail();
@POST
  if (pnname(N(5)) != "_verb")
	alphatoverb(5,0,0);
  L("m") = 0;
  L("h") = 0;
  L("b") = N(1);
  L("being") = N(3);
  L("v") = N(5);

  if (N(3))
    {
    N("sem",3) = N("stem",3) = "be";
    chpos(N(3),"VBG");
    }

  L("neg") = mhbvadv(2,4,0,0);
  group(1,5,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
    _be [s]														### (1)
    _xWILD [star match=(_adv _advl)]							### (2)
    being [s opt]												### (3)
    _xWILD [star match=(_adv _advl)]							### (4)
    _xWILD [s one match=(_verb _xALPHA) except=(_modal _have _be _vg _adj _adjc)]	### (5)
    @@

# verb damn [verb alpha]
# vg damn [vg alpha]
@POST
  alphatoadv(3);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_adv [star]
	damn
	@@

# prep damn [prep alpha]
@PRE
<1,1> varne("stem","to");
@POST
  alphatoadv(3);
@RULES
_xNIL <-
	_prep
	_adv [star]
	damn
	@@

# prep alpha dqan
# prep ving dqan
@PRE
<1,1> varne("stem","of");
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  alphatovg(3,"active",0);	# Replace old code. # 05/22/07 AM.
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _prep)]
	@@
# while alpha dqan
# when alpha dqan
# when ving dqan
_xNIL <-
	_xWILD [s one match=(when while where why)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num
#		_prep
		)]
	@@

# prep alpha alpha ,
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!singular(N(2)))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(\,)]
	@@
_xNIL <-
	_xWILD [s one match=(before)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(\,)]
	@@

# prep alpha ,
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
  if (N("verb",2))
	{
    if (vconjq(N(2),"-ing"))
	  fail();
	}
@POST
  alphatonoun(2);
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xALPHA
	_xWILD [one lookahead match=(\, _conj)]
	_xWILD [one fail=(_xALPHA _adj)]
	@@
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xALPHA
	_xWILD [one lookahead match=(_clausesep)]
	@@

# prep alpha [NIBBLE]
@CHECK
  if (!N("verb",2))
    fail();
  if (vconjq(N(2),"-ing"))
    fail();
@POST
  --N("pos num",2);
  N("verb",2) = 0;	# verb = 0
  alphaunambigred(2);
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xALPHA
	@@


# det alpha alpha noun
# 's alpha alpha noun
# in a double balanced modulator .
@CHECK
  if (!N("adj",2))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
  if (N("noun",3))
    {
	if (!singular(N(3)))
	  fail();
	}
  if (!singular(N(4)))
    fail();
@POST
  fixnpnonhead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xWILD [one match=(_det _aposS)]
	_xALPHA
	_xALPHA
	_noun
	_xWILD [one lookahead match=(_qEOS _xEND _prep)]
	@@

# Equation stuff.
@CHECK
  if (N("sem",3) != "formula")
    fail();
@POST
  L("tmp1") = N(1);
  group(1,4,"_noun");
  pncopyvars(L("tmp1"),N(1));
  N("sem",1) = "symbol";
@RULES
_xNIL <-
	_noun
	\(
	_np
	\)
	@@

# Misc parentheticals.
@POST
  group(2,4,"_noun");
@RULES
_xNIL <-
	_noun
	\(
	_xWILD [min=1 max=3 fail=( \) )]
	\)
	@@

# Question/query/interrogative fixups.
@CHECK
  if (!N("verb",3))
    fail();
  S("vc") = vconj(N(3));
  if (S("vc") == "inf"
   || S("vc") == "-ed"
   || S("vc") == "-ing")
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [s one match=(_have)]
	_xWILD [one match=(_noun _np _nps)]
	_xALPHA
	@@

# Question/query/interrogative fixups.
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [s one match=(_be)]
	_xWILD [one match=(_noun _np _nps)]
	_xALPHA
	@@

# cap prep
@CHECK
  if (N("verb",2))
	{
	if (N("noun",2)
     && (vconjq(N(2),"inf") || vconjq(N(2),"-ing")) )
	  fail();
	}
@POST
  if (N("noun",2) && plural(N(2)))
	fixnphead(2);
  else if (N("verb",2))
	{
	if (N("stem",4) == "by" && vconjq(N(2),"-en"))
	  alphatovg(2,"passive","VBN");
	else
	  alphatovg(2,0,"VB");
	}
  else if (N("noun",2))
	{
	fixnphead(2);
    nountonp(2,1);
	}
  else if (N("adj",2))
	alphatoadj(2);
  else if (N("adv",2))
	alphatoadv(2);
@RULES
_xNIL <-
	_xSTART
	_xCAP
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_prep)]
	@@

# verb alpha dqan
@CHECK
  if (!N("adv",3) || !N("adj",3))
    fail();
  if (N("pos num",3) != 2)
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adv");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [star lookahead match=(_advl _adv)]
	_xWILD [one match=(_det _pro _quan _num _xNUM _fnword)]
	@@

# verb alpha
@CHECK
  if (!N("adv",3) || !N("adj",3))
    fail();
  if (N("pos num",3) != 2)
    fail();
  L("txt") = nodesem(N(1));
  if (!copulaq(N(1)))
    fail();
@POST
  L("tmp3") = N(3);
  alphatoadj(3);
  group(3,3,"_adjc");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead fail=(_adj _adjc _xALPHA)]
	@@

# that verb
@CHECK
  if (pnname(N(2)) != "_fnword")
    fail();
  if (N("mypos",2))
    fail();
  if (!vconjq(N(3),"-ed")
   && !vconjq(N(3),"-s"))
   fail();
@POST
  pnrename(N(2),"_noun"); # rename _fnword.
  chpos(N(2),"DT");	# that/DT
@RULES
_xNIL <-
	_xSTART
	that [s]
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# , said
@POST
  L("t") = strtolower(N("$text",3));
  alphatovg(3,"active","VBP");
  N("glom",3) = "left";
  clearpos(N(3),1,0);
  if (L("next") = pnnext(N(3)))
    pnreplaceval(L("next"),"glom","right");
  N("inverted-verb-noun",3) = 1;
@RULES
_xNIL <-
	_xWILD [one match=( \, _dblquote _qEOS _xSTART _conj _clause _noun _np)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(say says said quip quips quipped recalls recalled)]
	@@

# X said
# cap said
# alpha said
@CHECK
  if (N("stem",2) != "say")
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  N("ne type",1) = N("sem",1) = "person";
  N("ne type conf",1) = 60;
  N("stem",1) = N("$text",1);
  if (singular(N(1)))
    chpos(N(1),"NP");
  else
    chpos(N(1),"NPS");

  if (pnname(N(2)) != "_verb")
    {
    L("tmp2") = N(2);
    group(2,2,"_verb");
    pncopyvars(L("tmp2"),N(2));
    L("v") = N(2);
    group(2,2,"_vg");
    mhbv(N(2),L("neg"),0,0,0,0,L("v"));
    pncopyvars(L("tmp2"),N(2));
    N("verb node",2) = L("v");
    clearpos(N(2),1,0);
    fixvg(N(2),"active",0);
	}
@RULES
_xNIL <-
	_xCAP
	_xWILD [one match=(_verb say says saying said
 		quip quips quipping quipped recalls recalled)]
	@@

# last noun
# last date
@CHECK
  if (N("sem",2) != "date")
    fail();
@POST
  alphatoadj(1);
@RULES
_xNIL <-
	last
	_noun [lookahead]
	@@

# alpha alpha
# alpha date
@PRE
<1,1> varz("adj");	# First alpha is not an adj.
<2,2> vareq("sem","date");	# Second alpha is a date.
@CHECK
  if (!N("noun",1) && !N("verb",1))
    fail();
  if (!N("noun",2) || !N("adv",2))
    fail();
@POST
# NLP++: don't know if the lookahead can safely be reduced.
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [one lookahead fail=(_aposS)]
	@@

# alpha alpha	# 06/08/06 AM.
# alpha date
@PRE
<1,1> var("verb");	# First alpha can be verb.
<2,2> vareq("sem","date");	# Second alpha is a date.
@CHECK
  if (vconjq(N(1),"inf")
   || vconjq(N(1),"-s"))
    fail();
  if (!N("noun",2) || !N("adv",2))
    fail();
@POST
# NLP++: lookahead cannot safely be reduced.
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [one lookahead fail=(_aposS)]
	@@

# alpha date
# last date
# next date.
@PRE
<2,2> vareq("sem","date");
@POST
  alphatoadj(1);
@RULES
_xNIL <-
	_xWILD [one match=(last next)]
	_noun
	@@

# since early July.
# since alpha noun.
@PRE
<2,2> var("adj");
@POST
  alphatoadj(2);
@RULES
_xNIL <-	# 04/20/07 AM.
	_xWILD [s one match=(since _det)]
	_xALPHA
 	_noun [lookahead]
	@@

# alpha date
# alpha noun
# Note: Assume date item doesn't glom.  If it does,
# assume that was handled already.
@PRE
<2,2> vareq("sem","date");
@POST
  L("tmp2") = N(2);
  if ((N("noun",1) || N("adj",1))
   && !N("adv",1) && !N("verb",1))
   {
   fixnpnonhead(1);
   group(1,2,"_np");
   pncopyvars(L("tmp2"),N(1));
   clearpos(N(1),1,1);
   }
  else
    {
	nountonp(2,1);
    }
@RULES
_xNIL <-
	_xALPHA
	_noun
	_xWILD [one lookahead fail=(_aposS _noun _xALPHA)]
	@@
_xNIL <-
	_xALPHA
	_noun
	_xEND
	@@

# though
@POST
  pnrename(N(2),"_adv");
  chpos(N(2),"RB");	# Override pos.
@RULES
_xNIL <-
	\,
	though [s]
	_xWILD [one lookahead match=(\, _qEOS _xEND)]
	@@

# including
# note: as prepositional construct.
@PRE
<2,2> varz("pos25 including");
@POST
  N("pos25 including",2) = 1;
  if (pnname(N(2)) == "_verb")
    pnrename(N(2),"_prep");	# including -> prep
# chpos(N(2),"IN");
  chpos(N(2),"VBG");	# Conform to Treebank.
@RULES
_xNIL <-
	_xWILD [one fail=(_be )]
	including [s]
	@@

# still
@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"RB");
@RULES
_xNIL <-
	still
	_xWILD [one lookahead match=(_verb _vg _prep _adv _advl)]
	@@

# than those prep
#@POST
#  pnrename(N(2),"_noun");
#@RULES
#_xNIL <-
#	than [s]
#	_xWILD [s one match=(these those)]
#	_prep [lookahead]
#	@@

# num noun
# note: number as adjective.
@CHECK
  if (!singular(N(2)))
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"CD");
  N("numeric",1) = 1;
@RULES
_xNIL <-
	_xNUM
	_noun
	@@

# alpha enough
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_xWILD [one match=(good bad)]
	enough
	@@

# Some default tagging.
@PRE
<3,3> varz("done");
@POST
  N("done",3) = 1;
  chpos(N(3),"VB");
@RULES
_xNIL <-
	_xWILD [s one match=(to _modal _do)]
	_xWILD [star match=(_adv _advl)]
	have [s]
	@@

@POST
  L("tmp1") = N(1);
  group(1,1,"_prep");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	unlike
	@@

# prep that alpha $
@CHECK
  if (!N("noun",3))
    fail();
@POST
  if (!N("mypos",2))
    chpos(N(2),"DT");	# that/DT.
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	that [s]
	_xALPHA
	_xWILD [one lookahead match=(_xEND)]
	@@

# prep that
# on that
@PRE
<2,2> varz("mypos");
@POST
  chpos(N(2),"DT");	# that/DT.
  pnrename(N(2),"_det");
@RULES
_xNIL <-
	_xWILD [s one match=(on)]
 	that [s]
	@@

# that adj $
# "that green" <=> "so green".
#@CHECK
#  if (N("mypos",1))
#    fail();
#@POST
#  chpos(N(1),"RB");	# that/RB
#@RULES
#_xNIL <-
#	that [s]
#	_adj [lookahead]
#	_xEND
#	@@
#_xNIL <-
#	that [s]
#	_adj [lookahead]
#	_xWILD [one fail=(_adj _noun \, _xALPHA)]
#	@@

# that alpha noun noun alpha $
@CHECK
  if (!N("verb",2))
    fail();
  if (!N("noun",5))
    fail();
  if (!vconjq(N(2),"-ed")
   && !vconjq(N(2),"-s")
   && !vconjq(N(2),"inf"))
    fail();
  if (!singular(N(4)))
    fail();
  if (!plural(N(5)))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
  if (!N("mypos",1))
    chpos(N(1),"IN");	# that/IN
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_noun
	_noun
	_xALPHA
	_xWILD [one match=(_xEND)]
	@@

# det that verb $
# ex: all that required.
# Ellipted: "all that that required"
@CHECK
  if (pnname(N(2)) == "_np")
    fail();
  if (N("mypos",2))
    fail();
  if (!vconjq(N(3),"-ed")
   && !vconjq(N(3),"-s"))
     fail();
@POST
  pnrename(N(2),"_det");
  chpos(N(2),"DT");	# that/DT
  nountonp(2,1);
  nountonp(1,1);
@RULES
_xNIL <-
	_det
	that [s]
	_xWILD [one match=(_verb _vg)]
	_xEND
	@@

# more noun
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"JJR");
  pnrename(N(1),"_adj");
@RULES
_xNIL <-
	_xWILD [s one match=(more less)]
	_xWILD [one lookahead match=(_noun _np)]
	@@
# most noun
@CHECK
  if (N("mypos",1))
    fail();
@POST
 chpos(N(1),"JJS");
  if (pnname(N(1)) == "_fnword")
    pnrename(N(1),"_adj");
@RULES
_xNIL <-
	most [s]
	_xWILD [one lookahead match=(_noun _np)]
	@@

# far more
# alpha fnword
# alpha more
@CHECK
  if (N("pos num",1) != 2)
    fail();
  if (!N("adj",1) || !N("adv",1))
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_xALPHA
	_xWILD [s one lookahead match=(more less)]
	@@

# more alpha
@CHECK
  if (!N("adj",2))
    fail();
@POST
  if (N("mass",2))
    {
    alphatonoun(2);
    if (!N("mypos",1))
      chpos(N(1),"JJR");
	}
  else
    {
    alphatoadj(2);
    if (!N("mypos",1))
      chpos(N(1),"RBR");
	}
 chpos(N(1),"RBR");	# more/RBR.
 pnrename(N(1),"_adj");
@RULES
_xNIL <-
	_xWILD [s one match=(more less)]
	_xALPHA
	@@


# most alpha
@CHECK
  if (!N("adj",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
  if (!N("mypos",1))
    chpos(N(1),"RB");	# most/RJS
  if (pnname(N(1)) == "_fnword")
	pnrename(N(1),"_adv");
@RULES
_xNIL <-
	most [s]
	_xALPHA
	\, [lookahead]
	_xWILD [one fail=(_xALPHA _adj)]
	@@

# det most adj
# pro most adj
@POST
  chpos(N(2),"RBS");	# most/RBS
  pnrename(N(2),"_premod");
  group(2,3,"_adj");
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	most [s]
	_adj
	@@

# most adj
# Note: If the above not matched, then most is adj.
@PRE
<1,1> varz("pos25 most-adj");
@POST
  N("pos25 most-adj",1) = 1;
  chpos(N(1),"JJS");	# most/JJS
  pnrename(N(1),"_adj");
@RULES
_xNIL <-
	most [s]
	_adj [lookahead]
	@@

# most alpha
@CHECK
  if (!N("adj",3))
    fail();
@POST
  alphatoadj(3);
  if (!N("mypos",2))
  	{
	if (N(1))
		chpos(N(2),"RBS");	# most/RBS
	else
		chpos(N(2),"JJS");	# most/JJS
	}
  pnrename(N(2),"_adj");	# most -> adj
@RULES
_xNIL <-
	_xWILD [opt match=(_det _pro)]
	most [s]
	_xALPHA
	@@

# , up
# , down
# up
# down
@CHECK
  if (N("mypos",3))
    fail();
@POST
  if (!N("mypos",3))
    chpos(N(3),"RB");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART \, _dbldash \- )]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one lookahead match=(up down)]
	@@

# verb prep prep
@PRE
<2,2> varz("mypos");
@CHECK
  if (pnname(N(1)) == "_vg")
	{
	L("v") = N("verb node",1);
	if (N("voice",1) != "active" && vconjq(L("v"),"-en"))
	  fail();
	}
  else
	{
	if (!N("mypos",1) || N("mypos",1) == "VBN")
	  fail();
	}
@POST
  if (!N("mypos",2))
    chpos(N(2),"IN");	# prep/IN
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
 	_prep [lookahead]
	_prep
	@@

# be up
# be down
# up
# down
# copula
# Note: "is off" -- some assign this as ADJ.
@CHECK
  if (N("mypos",3))
    fail();
  L("txt") = nodesem(N(1));
  S("copula") = copulaq(N(1));
  # Also check phrasal/prepositional.
  S("rp/in") = phrprepverbq(N(1),N(3));
  if (!S("copula") && !S("rp/in"))
    fail();
@POST
  # If both, don't know how to decide.
  # Prefer prep/phrasal for now.
  if (S("rp/in") == 1	# PHRASAL.
    || S("rp/in") == 3) # Ambig. Choose phrasal, slight pref.
    chpos(N(3),"RP");
  else if (S("rp/in") == 2) # PREPOSITIONAL.
    chpos(N(3),"IN");
  else # Must be copula case.
    chpos(N(3),"RB");	# up/RB down/RB out/RB off/RB

  # Reduce verb+particle to vg.
  if (pnname(N(1)) == "_vg" && S("rp/in"))
    {
	N("particle",1) = N(3);
    listadd(1,3,"true");	# Glom it together.
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one lookahead match=(up down out off)]
	@@

# noun alpha up
# noun alpha down
# up
# down
# out
@CHECK
  if (!N("verb",3))
    fail();
  # Require prepositional/phrasal verb.
  S("num") = phrprepverbq(N(3),N(5));
  if (!S("num"))
    fail();
@POST
  if (!N("mypos",5))
    {
	if (N("stem",5) == "to")
	 chpos(N(5),"TO");
	else if (S("num") == 2)
	  chpos(N(5),"IN");
	else # 1 or 3
	  chpos(N(5),"RP");
	}
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [plus match=(_adv _advl)]	# 04/20/07 AM.
	_xALPHA
	_xWILD [star match=(_adv _advl)]
#	_xWILD [s one lookahead match=(up down out off)]
	_prep [lookahead]
	@@

# clausal
# verb noun prep
# Check for phrasal verb.
@CHECK
  if (N("mypos",5))
    fail();
  if (N("pos25-particle",5))
    fail();
@POST
  N("pos25-particle",5) = 1;

	if (copulaq(N(1)))
	  {
	  chpos(N(5),"IN");	# eg, UP %   # 06/15/06 AM.
	  }
	# See if kb has something.	# 5/31/06 AM.
	else if (L("num") = phrprepverbq(N(1),N(5)))
	  {
	  if (L("num") == 2) # prepositional. # FIX. 06/18/06 AM.
	    chpos(N(5),"IN");
	  else	# 1 or 3.
	    chpos(N(5),"RP");	# Default.
	  }
#	else	# Default.
#	  chpos(N(5),"RP");

@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_prep
	_xWILD [one match=(_prep _advl \, _dbldash _xEND)]
	@@
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(up down out off)]
	@@

# as alpha prep
@CHECK
  if (!N("adj",2))
    fail();
  if (N("verb",2) || N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));

  pnrename(N(1),"_adv");
  if (!N("mypos",1))
    chpos(N(1),"RB");
@RULES
_xNIL <-
	as [s]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# as yet
@CHECK
  if (N("mypos",1) && N("mypos",2))
    fail();
@POST
  if (!N("mypos",2))
    {
	chpos(N(2),"RB");
	pnrename(N(2),"_adv");
	}
  if (!N("mypos",1))
    {
	chpos(N(1),"RB");
	pnrename(N(1),"_adv");
	}

@RULES
_xNIL <-
	as [s]
	yet [s]
	@@

# as adj as
# as adj det
@PRE
<1,1> varz("mypos");
@POST
  pnrename(N(1),"_adv");	# as -> adv.
  chpos(N(1),"RB");	# as/RB
@RULES
_xNIL <-
	as [s]
	_adj
	_xWILD [s one lookahead match=(_det as)]
	@@

# as dqan verb
@PRE
<1,1> varz("mypos");
@POST
  chpos(N(1),"IN");
@RULES
_xNIL <-
	as [s]
	_xWILD [plus lookahead match=(_det _pro _quan _num _xNUM
		_adj _noun _np _xALPHA)]
	_xWILD [one match=(_verb _vg)]
	@@

# as
@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"IN");	# as/IN
  pnrename(N(1),"_prep");
@RULES
_xNIL <-
	as [s]
	@@

# yet whether
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"RB");	# yet/RB.
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	yet [s]
	_fnword [lookahead]
	@@

# no adv
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"RB");
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	no [s]
	_adv [lookahead]
	@@

# , compared
# compared
@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"VBN");
  N("voice",2) = "passive";
  N("-en",2) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(_xSTART \, when if as)]
	compared [s]
	@@

# alpha 's
# alpha '
@CHECK
  if (!N("noun",1))
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one lookahead match=(_aposS _aposX)]
	@@

# once dqan
#@CHECK
#  if (N("mypos",1))
#    fail();
@POST
  chpos(N(1),"IN");	# once/IN
@RULES
_xNIL <-
	once [s]
	_xWILD [one lookahead match=(_noun _np _det _pro
		_quan _num _xNUM _adj )]
	@@

# num 's
@POST
  if (!N("mypos",2))
    chpos(N(2),"POS");	# 's/POS
  group(1,2,"_adj");
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	_xNUM
	_aposS
	@@

# 's not verb
@CHECK
  if (!vconjq(N(4),"-en")
   && !vconjq(N(4),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  pnrename(N(2),"_be");
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"VBZ");	# 's/VBZ
  N("copula",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _xALPHA)]
	_aposS
	_neg [s]
	_xWILD [one match=(_verb)]
	@@

# 's not dqan
@POST
  L("tmp2") = N(2);
  pnrename(N(2),"_be");
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"VBZ");
  N("apos-s",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _xALPHA)]
	_aposS
	_neg [s]
	_xWILD [one match=(_np _det _pro _quan _num _xNUM
		_adj _noun)]
	@@

# cap 's said
# Webster's said
# Note: actually, ambiguous verb/noun.
@POST
  chpos(N(1),"NP");
  chpos(N(2),"POS");
  L("tmp1") = N(1);
  group(1,2,"_noun");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,0);
  N("apos-s",1) = 1;
@RULES
_xNIL <-
	_xCAP [s]
	_aposS
	_xWILD [star lookahead match=(_adv _advl)]
	said [s]
	@@
_xNIL <-
	_xCAP [s]
	_aposS
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_verb _modal _vg)]
	@@

# 's alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (N("adj",2))
    fail();
  if (!singular(N(2)))
    fail();
@POST
  if (!N("mypos",1))
   chpos(N(1),"POS");	# 's/POS
  alphatonoun(2);
@RULES
_xNIL <-
	_aposS
	_xALPHA
	@@

# 's alpha
@CHECK
  if (N("mypos",1))
    fail();
  if (N("verb",2))
    {
	if (!vconjq(N(2),"-s")
	 && !vconjq(N(2),"inf"))
	  fail();
	}
  else if (N("noun",2))
    {
	if (!plural(N(2)))
	  fail();
	}
@POST
  chpos(N(1),"POS");	# 's/POS
@RULES
_xNIL <-
	_aposS
	_xALPHA [lookahead]
	@@

# noun 's $
@POST
  chpos(N(2),"POS");	# 's/POS
  group(1,2,"_adj");
  N("apos-s",1) = 1;
@RULES
_xNIL <-
	_noun
	_aposS
	_xWILD [one lookahead match=(_dbldash _xEND)]
	@@

# 's $
@POST
 chpos(N(1),"POS");	# 's/POS
@RULES
_xNIL <-
	_aposS
	_xWILD [one match=(_dbldash _xEND)]
	@@

# adj alpha
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_xWILD [one lookahead match=(_xEND _advl _conj _dbldash _fnword 
		_modal _prep _verb _vg)]
	@@
# det alpha
_xNIL <-
	_det
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# adj alpha
@PRE
<2,2> var("noun");
<2,2> var("verb");
@CHECK
  if (N("pos num",2) != 2)
	fail();
@POST
  alphatonoun(2);
@RULES
_xNIL <-
	_adj
	_xALPHA
	@@

# conj alpha prep
@PRE
<2,2> varz("noun");
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing")
   && !vconjq(N(2),"-edn"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_conj
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# conj alpha noun
@POST
  L("done") = 0; # If found something.
  if (N("noun",2) || N("adj",2))
	{
	if (singular(N(3)) && !N("cap",3))
	  {
	  fixnpnonhead(2);
	  L("done") = 1;
	  }
	}
  noop();
@RULES
_xNIL <-
	_conj
	_xALPHA
	_noun [lookahead]
	_xWILD [one match=(_prep)]
	@@

# fnword alpha verb
# conj alpha verb
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
#  if (vconjq(N(2),"-ing")
#   || vconjq(N(2),"-edn"))
#    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_fnword _conj)]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg _modal)]
	@@

# fnword alpha alpha
@PRE
<3,3> var("verb");
<5,5> var("adv");
@POST
  alphatoadv(5);
  alphatovg(3,0,0);
@RULES
_xNIL <-
 	but [s]
 	_adv [star]
	_xALPHA
 	_adv [star]
	_xALPHA
	_xWILD [one match=(_np _pro _det _quan _num _xNUM)]
	@@

# noun alpha verb
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  posacct(N(2));
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# idiom: pretty much
# much
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	pretty
	much
	@@

# that num ,
# that money ,
@CHECK
  if (N("mypos",1) && N("mypos",1) != "DT")
    fail();
@POST
 chpos(N(1),"DT");	# that/DT.
  pnrename(N(1),"_det");
@RULES
_xNIL <-
	that [s except=(_det)]
	_num
	_xWILD [one lookahead match=( \, )]
	@@

# that noun $
# that dqan $
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"DT");	# that/DT.
  if (pnname(N(1)) == "_fnword")
    pnrename(N(1),"_det");
@RULES
_xNIL <-
	that [s]
	_adj [star]
	_noun [plus]
	_xWILD [one match=(_xEND _qEOS _dbldash)]
	@@

# that $
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"DT");	# that/DT.
  if (pnname(N(1)) == "_fnword")
    pnrename(N(1),"_noun");
@RULES
_xNIL <-
	that [s]
	_xWILD [one match=(_xEND _qEOS _dbldash)]
	@@

# that alpha
# Idiomatic or highly preferred cases.
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	that [s]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(uses)]
	@@

# alpha noun
# gives us
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	gives
	_xWILD [one lookahead match=(_pro _det _quan _num _xNUM _adj _noun _np)]
	@@

# noun adj noun
@CHECK
  if (!N("cap",2) && !N("num",2) && !N("possessive",3))
  	fail();
@POST
  pnrename(N(2),"_adj");	# noun -> adj
@RULES
_xNIL <-
	_xWILD [one match=(_det _prep _pro _conj)]
	_noun
	_adj [plus]
	_noun
	@@

# these alpha
# those alpha
# plural alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (!plural(N(2)))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  # _np
@RULES
_xNIL <-
	_xWILD [s one match=(these those)]
	_xALPHA
	_xWILD [one lookahead fail=(_xALPHA _noun)]
	@@
_xNIL <-
	_xWILD [s one match=(these those)]
	_xALPHA
	_xEND
	@@

# such alpha alpha
# plural reasoning.
@POST
  if (plural(N(3)))
    {
	L("done") = 1;
    fixnphead(3);
	}
  else if (vconjq(N(3),"-ing"))
    L("done") = 1;

  if (!L("done") && N(4))
    {
    if (plural(N(4)))
      {
	  L("done") = 1;
      fixnphead(4);
	  fixnpnonhead(3);
	  }
    else if (vconjq(N(4),"-ing"))
      L("done") = 1;
	}

  if (!L("done") && N(5))
    {
    if (plural(N(5)))
      {
	  L("done") = 1;
      fixnphead(5);
	  fixnpnonhead(4);
	  fixnpnonhead(3);
	  }
    else if (vconjq(N(5),"-ing"))
      L("done") = 1;
	}
@RULES
_xNIL <-
	_xWILD [one fail=(no)]
	such [s]
	_xALPHA
	_xALPHA [opt]
	_xALPHA [opt]
	@@

# ever verb
@POST
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	_xWILD [s one match=(ever) except=(_adv)]
	_xWILD [one lookahead match=(_verb _vg)]
	@@


# ^ like dqan
@CHECK
  if (N("mypos",3))
    fail();
  if (pnname(N(3)) != "_fnword")
    fail();
@POST
  pnrename(N(3),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART \( _dbldash \, )]
	_xWILD [star match=(_conj _adv _advl)]
	like [s]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_np _noun
		_det _quan _adj _num _xNUM
		)]
	@@

# something like
@CHECK
  if (N("mypos",2))
    fail();
  if (pnname(N(2)) != "_fnword")
    fail();
@POST
  pnrename(N(2),"_prep");
@RULES
_xNIL <-
	_xWILD [s one match=(something someone somewhat
		somewhere somehow someplace sometime
		)]
	like [s]
	@@

# alpha like
@CHECK
  if (pnname(N(2)) != "_fnword")
    fail();
  if (N("pos num",1) != 2)
    fail();
  if (!N("verb",1) || !N("adj",1))
    fail();
@POST
  # Don't commit to verb-adj yet.
  pnrename(N(2),"_prep");
@RULES
_xNIL <-
	_xALPHA
	like [s]
	@@

# verb num
@POST
  nountonp(2,1);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_num _quan _xNUM)]
	_xWILD [one lookahead match=(_fnword _prep _verb _vg
		_det _np)]
	@@

# this
# Fixup of lone determiner.
@CHECK
  if (pnname(N(1)) != "_det")
   fail();
@POST
  group(1,1,"_noun");
  nountonp(1,1);
@RULES
_xNIL <-
	this [s]
	_adv [star]
	_xWILD [one lookahead match=(_advl _np _vg
		_pro _modal _verb _fnword _prep _conj
		)]
	@@

# to alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"inf")) # New function. # 06/15/06 AM.
    fail();
@POST
  alphatovg(3,"active","VB");
@RULES
_xNIL <-
	to [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xWILD [lookahead one match=(
		_det _quan _num _xNUM _adj _np _pro)]
	@@

# to alpha
@PRE
<1,1> varz("pos25 to-alph");
@CHECK
  if (N("adj",2) && N("verb",2) && vconjq(N(2),"-ing"))
	fail();
@POST
  N("pos25 to-alph",1) = 1;
  if (N("unknown",2))
    alphatonoun(2);
  else if (vconjq(N(2),"-ing"))
	{
	if (N("adj",2))
		alphatoadj(2);
	else
		alphatoverb(2,"active","VBG");
	}
  else if (N("verb",2))
    {
	if (vconjq(N(2),"inf")
	 && !plural(N(2)) )
	  {
	  # Still ambiguous.	# 04/23/07 AM.
#	  alphatoverb(2,"active","VB");	# 04/23/07 AM.
	  }
	else if (vconjq(N(2),"-en")
	 || vconjq(N(2),"-ing"))
	  alphatoadj(2);
	else if (N("noun",2))
	  alphatonoun(2);
	else
		{
	"err.txt" << "pos50 to-alpha: " << phrasetext() << "\n";
    alphatoverb(2,"active","VB");	# verb/VB [DEFAULT]	
		}
	}
  else if (N("adj",2))
    alphatoadj(2);
  else if (N("noun",2))
    alphatonoun(2);
  else if (N("adv",2))
    alphatoadv(2);
  else # [DEFAULT]
    {
	"err.txt" << "pos50 to-alpha: " << phrasetext() << "\n";
    alphatoverb(2,"active","VB");	# verb/VB [DEFAULT]
	}

@RULES
_xNIL <-
	to [s]
	_xALPHA
	@@

# , but
# Note: Finding different types of commas.
@POST
  group(1,1,"_clausesep");
@RULES
_xNIL <-
	\,
	but [s lookahead]
	@@

# verb yet
@CHECK
  if (N("mypos",3))
    fail();
  if (!N("verb",4))
    fail();
  if (!vconjq(N(4),"-en"))
    fail();
@POST
  chpos(N(3),"RB");	# yet/RB.
  pnrename(N(3),"_adv");
@RULES
_xNIL <-
	_xWILD [s one match=(_modal _have _be being)]
	_adv [star]
	yet [s]
	_xWILD [one lookahead match=(_xALPHA _verb)]
	@@

# have
@PRE
<1,1> varz("done");
@POST
  N("done",1) = 1;
  chpos(N(1),"VBP");
@RULES
_xNIL <-
	have [s]
	@@

# adv alpha
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  alphatoverb(2,"active","VBG");
@RULES
_xNIL <-
	_neg [s]
	_xALPHA
	@@

# det date alpha
# det noun alpha
# dqan alpha
# NIBBLE.
@CHECK
  if (N("sem",3) != "date"
   && !N("cap",3))
    fail();
  if (!N("noun",4))
    fail();
  if (N("verb",4))
    {
	if (vconjq(N(4),"-ed") || vconjq(N(4),"-ing"))
	  fail();
    if (singular(N(3)) && singular(N(4)))
      succeed();
	fail();
	}
@POST
  alphatonoun(4); # adjnoun thingy.
  # Fix some caps from context.
  if (N("mypos",3) == "NPS" && N("unknown",3) && singular(N(4)))
	N("mypos",4) = "NP";	# noun(pl) -> noun(s)
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_adj [star]
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [one lookahead fail=(_pro _noun _np)]
	@@

# det noun alpha $
@PRE
<3,3> var("noun");
@CHECK
  if (N("unknown",2) && N("cap",2) && N("mypos",2) == "NPS")
	succeed();
  fail();
@POST
  N("mypos",2) = "NP";
  fixnphead(3);
@RULES
_xNIL <-
	_det
	_noun
	_xALPHA
	_xEND
	@@

# alpha modal
@CHECK
  if (!N("noun",1))
    fail();
  if (N("verb"))
    {
	if (vconjq(N(1),"-ing")
	 || vconjq(N(1),"-en"))
	  fail();
	}
@POST
  fixnphead(1);
@RULES
_xNIL <-
	_xALPHA
	_modal [lookahead]
	@@

# prep noun alpha conj verb
# prep dqan alpha conj verb
@CHECK
# Todo: check v-v agreement.
if (!N("noun",6))
  fail();
if (vvagree(N(6),N(8)))
  fail();
@POST
  fixnphead(6);
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_conj [lookahead]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# det verb
# the verb
@POST
  pnrename(N(2),"_adj");	# verb -> adj
  chpos(N(2),"JJ");
@RULES
_xNIL <-
	_xWILD [s one match=(a the _premod)]
	_verb
	@@

# alpha by
@CHECK
  if (!N("verb",1))
    fail();
  if (!vconjq(N(1),"-en"))
    fail();
@POST
 alphatoverb(1,0,0);
@RULES
_xNIL <-
	_xALPHA
	by [s lookahead]
	@@

# alpha than
# Note: test comparative adj.
@CHECK
  L("txt") = strtolower(N("$text",1));
  L("j") = adjconj(L("txt"));
  if (L("j") != "JJR")
    fail();
@POST
  alphatoadj(1);
@RULES
_xNIL <-
	_xALPHA
	than [s lookahead]
	@@

# , alpha prep
@CHECK
  if (!N("noun",2))
    fail();
  if (N("verb",2) || N("adv",2))
    fail();
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@


# whose alpha
@POST
  if (N("verb",2))
    {
	N("verb",2) = 0;	# verb = 0
	--N("pos num",2);
	}
  alphaunambigred(2);
@RULES
_xNIL <-
	whose [s]
	_xALPHA
	@@

# det num alpha alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
@POST
  if (plural(N(3)))
    fixnphead(3);
  else if (N("noun",4))
    {
	fixnpnonhead(3);
	if (plural(N(4)))
	  fixnphead(4);
	}
@RULES
_xNIL <-
	_xWILD [opt match=(_det _pro)]
	_xWILD [one match=(_xNUM _num _quan)]
	_xALPHA
	_xALPHA
	@@

# dqan alpha adj
# Post-np adj. ("outstanding").
@CHECK
  if (!N("noun",2))
    fail();
  if (copulaq(N(2)))
    fail();
@POST
  fixnphead(2);
  group(3,3,"_adjc");
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro _quan _num _xNUM _adj _noun)]
	_xALPHA
	_adj	# 04/25/10 AM.
	_xWILD [one lookahead fail=(_xALPHA _adj _noun)]
	@@
_xNIL <-
	_xWILD [plus match=(_det _pro _quan _num _xNUM _adj _noun)]
	_xALPHA
	_adj	# 04/25/10 AM.
	_xEND
	@@

# how many alpha
@POST
  if (N("noun",2) && N("adj",2))
    {
	if (N("verb",2))
	  {
	  --N("pos num",2);
	  N("verb",2) = 0;	# verb = 0
	  }
	}
  else if (N("noun",2))
    alphatonoun(2);
  else if (N("adj",2))
    alphatoadj(2);
@RULES
_xNIL <-
	_howmuch [s]
	_xALPHA
	@@

# when alpha
@PRE
<1,1> varz("pos25 when");
@CHECK
 N("pos25 when",1) = 1;
 if (N("verb",2))
   {
   if (vconjq(N(2),"-ing")
    || vconjq(N(2),"-en"))
	 fail();
   }
@POST
  if (N("verb",2))
    {
	--N("pos num",2);
	N("verb",2) = 0;	# verb = 0
	}
  alphaunambigred(2);
@RULES
_xNIL <-
	when [s]
	_xALPHA
	@@

# alpha alpha that
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ed"))
    fail();
@POST
  alphatoverb(2,"active","VBD");
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	that [s lookahead]
	@@

# verb worth
# Copied here.	# 04/21/07 AM.
@POST
  L("tmp2") = N(2);
  group(2,2,"_prep");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	worth
	_xWILD [one lookahead match=(_qEOS _xEND \, )]
	@@

# alpha $
#@POST
#  if (N("verb",1))
#    {
#	if (vconjq(N(1),"-edn"))
#	  alphatoverb(1,0,0);
#	}
#  else if (N("pos num",1) == 2
#   && N("noun",1) && N("adj",1))
#     fixnphead(1);
#@RULES
#_xNIL <-
#	_xALPHA
#	_xWILD [one match=(_xEND)]
#	@@

# alpha alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (N("adj",2) || N("verb",2))
    fail();
  if (!N("adj",1) || !N("verb",1))
    fail();
  if (N("pos num",1) != 2)
    fail();
  if (!vconjq(N(1),"-edn"))
    fail();
@POST
  alphatonoun(2);
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	@@

# Note: exceptions are run on clauses.
# Note: eg, "the ship he runs is shoddy"
# alpha be
@CHECK
  if (!N("noun",2))
    fail();
  if (plural(N(2)))	# 04/22/07 AM.
	fail();
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xWILD [one fail=(_pro _np)]
	_xALPHA
	_be [s lookahead]
	@@

# adj num
@POST
  L("tmp2") = N(2);
  if (!N(3))
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
    chpos(N(2),"CD");
    }
  else
    {
	if (!N("mypos",3))
	  chpos(N(3),"POS");
	chpos(N(2),"CD");
	group(2,3,"_adj");
	}
@RULES
_xNIL <-
	_adj
	_xNUM
	_aposS [opt]
	@@

@PRE
<1,1> var("verb");
<1,1> varz("noun");	# 04/20/07 AM.
@CHECK
  # Require prepositional/phrasal verb.
  S("num") = phrprepverbq(N(1),N(2));
  if (!S("num"))
    fail();
@POST
  alphatoverb(1,0,0);
@RULES
_xNIL <-
	_xALPHA
	to [s]
	@@

# noun list.	# 04/21/07 AM.
# noun , alpha , alpha and alpha noun
@PRE
<3,3> var("noun");
<5,5> var("noun");
<8,8> var("noun");
@POST
  alphatonoun(3);
  alphatonoun(5);
  alphatonoun(8);
  group(3,5,"_noun");	# Collapse some of this...
@RULES
_xNIL <-
	_noun
	\,
	_xALPHA
	\,
	_xALPHA
	\, [opt]
	_conj
	_xALPHA
	_noun [lookahead]
	@@

# while.
# Note: while as a verb is specialized, ignoring for now.
@CHECK
  if (N("mypos",1))
	fail();
@POST
  chpos(N(1),"IN");
@RULES
_xNIL <-
	while [s except=(_noun _prep)]
	@@

# past num
@POST
  L("tmp1") = N(1);
  group(1,1,"_num");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"JJ");
@RULES
_xNIL <-
	past
	_xWILD [one lookahead match=(_num _quan _xNUM)]
	@@

# dqan
# det noun conj noun prep
@POST
  if (N("date",2) || N("date",4))
    L("date") = 1;
  group(2,4,"_noun");
  if (L("date"))
    N("date",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_det)]
	_noun
	_conj
	_noun
	_xWILD [one lookahead match=(_prep)]
	@@

# Note: special verbs.
# beginning next
@CHECK
  if (!N("noun",1) && !N("verb",1))
	fail();
@POST
  alphatoverb(2,"active","VBG"); # Conform Treebank.
  group(2,3,"_prep");	# Treat as a complex preposition.
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one match=(starting beginning commencing ending
		)]
	_prep [opt]
	_xWILD [one lookahead match=(next last)]
	@@

# noun , alpha $
@PRE
<3,3> var("noun");
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_noun
	\,
	_xALPHA
	_xWILD [one lookahead match=(_dbldash _xEND)]
	@@

# alpha to be
@PRE
<1,1> var("verb");
<1,1> varz("noun");
@CHECK
  if (!vconjq(N(1),"-en"))
	fail();
@POST
  alphatoverb(1,"passive","VBN");
@RULES
_xNIL <-
	_xALPHA
	to [s lookahead]
	_be [s]
	@@

# noun and noun alpha
@CHECK
  if (!N("noun",4))
	fail();
  if (!plural(N(4)))
	fail();
@POST
   alphatonoun(4);
@RULES
_xNIL <-
	_noun
 	_conj
	_noun
	_xALPHA
	@@

# noun alpha of
# noun alpha prep
@PRE
<2,2> var("adj");
@CHECK
  if (!plural(N(1)))
	fail();
@POST
  alphatoadj(2);
  group(2,2,"_adjc");
@RULES
_xNIL <-
	_noun
	_xALPHA
	of [s lookahead]
	@@

# noun alpha prep
@PRE
<2,2> var("adj");
<2,2> var("adv");
<2,2> vareq("pos num",2);
@CHECK
  if (!plural(N(1)))
	fail();
@POST
  # Prefer adv, but may need some list of preferences
  # for post-np cases like these.
  alphatoadv(2);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_prep [lookahead]
	@@

# alpha of
@PRE
<1,1> var("noun");
<1,1> var("verb");
<1,1> varz("adj");
@CHECK
  if (phrprepverbq(N(1),N(2)))
	fail();
@POST
  fixnphead(1);
@RULES
_xNIL <-
 	_xALPHA
	of [s lookahead]
	@@
 
# det num
@POST
  if (N("num",2) == 1)
    N("mypos",2) = "NN";
  else
	N("mypos",2) = "NNS";
  pnrename(N(2),"_noun");	# num -> noun
@RULES
_xNIL <-
	_det
	_num
	_xWILD [one lookahead fail=(_xALPHA _det _quan _num _xNUM
		_adj _adjc _noun _fnword)]
	@@

# one alpha
# num alpha
@PRE
<3,3> var("verb");
@POST
  alphatoverb(3,0,0);
@RULES
_xNIL <-
	_xWILD [one match=(_dbldash)]
	_num
	_xALPHA
	_xWILD [one lookahead match=(_det _pro)]
	@@

# num that
# one that
@PRE
<1,1> varz("mypos");
@POST
  pnrename(N(1),"_np");	# one -> np
  chpos(N(1),"NN");	# one/NN
@RULES
_xNIL <-
	one [s]
	that [s lookahead]
	@@


# if alpha
@CHECK
  if (!N("verb",2))
	fail();
  if (!vconjq(N(2),"inf"))
	fail();
@POST
  --N("pos num",2);
  N("verb",2) = 0;	# verb = 0
  alphaunambigred(2);
@RULES
_xNIL <-
	if [s]
	_xALPHA
	@@

# , alpha adj
# , alpha noun
@PRE
<2,2> var("noun");
<2,2> var("adj");
<2,2> vareq("pos num",2);
@POST
  fixnpnonhead(2);
@RULES
_xNIL <-
	\,
	_xALPHA
	_xWILD [one lookahead match=(_adj _noun)]
	@@

# noun , alpha adj
# dqan , alpha adj
@PRE
<3,3> var("verb");
<3,3> varz("adj");
@POST
  alphatovg(3,0,0);
@RULES
_xNIL <-
    _xWILD [one match=(_noun _np)]	### (1)
    \,								### (2)
    _xALPHA							### (3)
    _adj [lookahead]				### (4)
    @@

# conj alpha adj
@PRE
<3,3> var("noun");
<3,3> var("verb");
<3,3> vareq("pos num",2);
@POST
  alphatoverb(3,0,0);
@RULES
_xNIL <-
	_conj
	_adv [star]
	_xALPHA
	_adj [lookahead]
	@@

# num alpha $
@CHECK
  if (!N("noun",2))
	fail();
  if (!singular(N(2)))
	fail();
  if (N("num",1) > 1900 && N("num",1) < 2015)
    succeed();
  fail();
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xNUM
	_xALPHA
	_xEND
	@@

# get verb
@PRE
<1,1> vareq("stem","get");
@CHECK
  if (!vconjq(N(2),"-en"))
	fail();
@POST
  pnrename(N(1),"_be");
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_verb
	_verb
	@@

# get alpha
@PRE
<1,1> vareq("stem","get");
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"-en"))
	fail();
@POST
  alphatoverb(2,"passive","VBN");
  pnrename(N(1),"_be");
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_verb
	_xALPHA
	@@

# det adj verb alpha
@PRE
<2,2> var("cap");
<5,5> var("noun");
@CHECK
  if (!vconjq(N(4),"-en"))
	fail();
  if (N("mass",5))
	fail();
  if (plural(N(5)))
	fail();
@POST
  pnrename(N(4),"_adj");	# verb -> adj
  fixnphead(5);

@RULES
_xNIL <-
	_det
	_noun [opt]
	_adj
	_verb
	_xALPHA
	_xWILD [one lookahead match=(_dbldash _xEND)]
	@@

# verb so alpha
@PRE
<3,3> var("adj");
@CHECK
  if (!copulaq(N(1)))
	fail();
@POST
  pnrename(N(2),"_adv");
  chpos(N(2),"RB");	# so/RB
  alphatoadj(3);
@RULES
_xNIL <-
	_verb
	so [s]
	_xALPHA
	@@

# before $
@PRE
<1,1> varz("pos25 before $");
@POST
  N("pos25 before $",1) = 1;
  chpos(N(1),"RB");	# before/RB
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	before [s]
	_xWILD [one match=(_qEOS _xEND _dbldash \, )]
	@@

# unworkable by ...
# , adj by
@POST
  group(2,2,"_adjc");
  N("ellipted-copula",2) = 1;
@RULES
_xNIL <-
	\,
	_adj
	by [s lookahead]
	@@

# pro alpha $
@POST
  if (N("verb",2) && vconjq(N(2),"inf"))
	alphatovg(2,"active","VBP");
@RULES
_xNIL <-
	us [s]
	_xALPHA
	_xEND
	@@

# , adj ,
@POST
  excise(3,3);
  excise(1,1);
@RULES
_xNIL <-
	\,
	_adj
	\,
	@@


# who noun verb
@PRE
<3,3> varz("mypos");
@CHECK
  if (!vconjq(N(3),"-ed"))
	fail();
@POST
  chpos(N(3),"VBD");
@RULES
_xNIL <-
	_whword [s]
	_xWILD [one match=(_noun _np)]
	_verb
	@@

# Note: Too simple and stupid.  Need more context.	# 05/31/07 AM.
# noun verb
#@PRE
#<2,2> varz("mypos");
#@CHECK
#  if (!vconjq(N(2),"inf"))
#	fail();
#@POST
#  chpos(N(2),"VBP");
#@RULES
#_xNIL <-
#	_noun
#	_verb
#	@@

# prep both alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
	fail();
@POST
  if (N("noun",3) && plural(N(3)))
	fixnphead(3);
  else if (N("adj",3))
	alphatoadj(3);
@RULES
_xNIL <-
	of [s]
	both [s]
	_xALPHA
	@@

# before alpha noun
@PRE
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"-ing"))
	fail();
@POST
  alphatovg(2,"active","VBG");
@RULES
_xNIL <-
	before [s]
 	_xALPHA
	_xWILD [one lookahead match=(_noun _np _det _quan _adj
		_num _xNUM )]
	@@

# before
# Note: Try simple approach here.
@PRE
<1,1> varz("mypos");
@POST
 chpos(N(1),"IN");	# before/IN
# pnrename(N(1),"_prep");
@RULES
_xNIL <- before [s] @@

# verb alpha alpha prep
# Ex: was still well above
@CHECK
  if (N(2))
	{
	if (!N("adv",2))
		fail();
	}
@POST
  alphatoadv(3);
  alphatoadv(2);
@RULES
_xNIL <-
	_verb
	_xALPHA [opt]
	well
	_prep [lookahead]
	@@

# alpha alpha dqan
# ving ving dqan
@PRE
<1,1> var("noun");
<2,2> var("verb");
@CHECK
  if (!N("verb",1))
	fail();
  if (!vconjq(N(1),"-ing") || !vconjq(N(2),"-ing"))
	fail();
@POST
  alphatoverb(2,"active","VBG");
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [one match=(_det _pro _quan _num _xNUM _np)]
	@@

# conj alpha , modal $
@PRE
<3,3> var("noun");
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_conj
	_adv [star]
	_xALPHA
	\,
	_modal
	_xEND
	@@

# alpha pro
@PRE
<2,2> var("noun");
<3,3> var("prosubj");
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_pro [lookahead]
	@@
_xNIL <-
	_xWILD [one match=(_dbldash)]
	_xALPHA
	_pro [lookahead]
	@@

# alpha pro
@PRE
<1,1> var("verb");
<1,1> varz("adj");
@POST
  alphatoverb(1,0,0);
@RULES
_xNIL <-
	_xALPHA
	_adv [star]
	_xWILD [s one match=(me him them us)] # Some _proObj
	@@

# prep num alpha _conj
@CHECK
  N("num") = num(N("$text",2));
  if (N("num") < 1900 || N("num") > 2015)
	fail();
  if (N("noun",2) && plural(N(2)))
  	fail();
  if (N("verb",2) && vconjq(N(2),"-s"))
  	fail();
@POST
  alphatonoun(2);
  chpos(N(2),"CD");	# num/CD.
  L("tmp2") = N(2);
  nountonp(2,1);
@RULES
_xNIL <-
	_prep
	_xNUM
	_xALPHA [lookahead]
	_conj
	@@

# prep num alpha
@PRE
<4,4> var("adj");
@POST
 alphatoadj(4);
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_det _pro)]
 	_xWILD [plus match=(_quan _num _xNUM)]
	_xALPHA
	_xWILD [one lookahead match=(_adj _noun)]
 	@@

# noun verb to alpha alpha
# np vg to alpha alpha
@PRE
<4,4> var("verb");
<5,5> varz("verb");
@CHECK
  if (!vconjq(N(4),"inf"))
	fail();
@POST
  alphatovg(4,"active","VB");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	to [s]
	_xALPHA
	_xALPHA [lookahead]
	@@

# noun alpha to verb
# np alpha to vg
@PRE
<2,2> var("verb");
@CHECK
  if (!verbfeat(N(2),"T3"))
	fail();
@POST
  alphatoverb(2,0,"VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _pro)]
	_xALPHA
	to [s lookahead]
	_xWILD [one match=(_verb _vg _modal)]
	@@

# noun alpha to alpha
@PRE
<2,2> var("verb");
<4,4> var("verb");
@CHECK
  if (!verbfeat(N(2),"T3"))
	fail();
  if (!vconjq(N(4),"inf"))
	fail();
@POST
  alphatoverb(4,"active","VB");
  alphatoverb(2,"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xALPHA
	to [s]
	_xALPHA
	@@

# det alpha , alpha
@PRE
<2,2> var("adj");
<4,4> var("adj");
@POST
  fixnpnonhead(2);
@RULES
_xNIL <-
	_det
	_xALPHA
	\, [lookahead]
	_xALPHA
	@@

# verb prep dqa alpha alpha by
# vg prep dqa alpha alpha by
@PRE
<7,7> var("noun");
@CHECK
  if (!N("noun",6) && !N("adj",6))
	fail();
  if (!vconjq(N(1),"-en"))
	fail();
@POST
  if (!N("voice",1))
	{
	if (pnname(N(1)) == "_vg")
	  fixvg(N(1),"passive","VBN");
	else
	  fixverb(N(1),"passive","VBN");
	}
  fixnphead(7);
  fixnpnonhead(6);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	by [s lookahead]
	@@

# verb by alpha alpha alpha conj
# vg by alpha alpha alpha conj
# verb prep alpha alpha alpha conj
@PRE
<2,2> vareq("stem","by");
@POST
  if (N("noun",3) && plural(N(3)))
	fixnphead(3);
  else if (!N(4))	# One alpha
	{
	}
  else if (!N(5))	# Two alphas
	{
	if (N("noun",4) && plural(N(4)))
	  fixnphead(4);
 	}
  else	# Three alphas
 	{
	if (N("noun",4) && plural(N(4)))
	  fixnphead(4);
	else if (N("noun",5) && plural(N(5)))
	  {
	  fixnphead(5);
	  if (N("adj",4) || N("noun",4))
		fixnpnonhead(4);
	  }
 	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_xALPHA
	_xALPHA [opt]
	_xALPHA [opt]
	_xWILD [one lookahead match=(_conj)]
	@@


# conj alpha alpha alpha ,
@PRE
<2,2> var("noun");
<3,3> var("verb");
@CHECK
  if (!plural(N(2)))
  	fail();
  if (!vconjq(N(3),"-ing"))
  	fail();
@POST
  alphatovg(3,"active","VBG");
  fixnphead(2);
@RULES
_xNIL <-
	_xWILD [plus match=(_conj \, )]
	_xALPHA
	_xALPHA
	_xALPHA [star]
	_xWILD [one lookahead match=( \, _conj)]
	@@

# alpha prep
# Note: Looks too broad.	# 05/22/07 AM.
@PRE
<1,1> varz("proposs");
<2,2> var("verb");
@CHECK
  S("num") = phrprepverbq(N(2),N(3));
  if (!S("num"))
	fail();
  if (vconjq(N(2),"inf") || vconjq(N(3),"-s"))
  	fail();
@POST
  if (vconjq(N(2),"-edn"))	# ambig
  	L("pos") = 0;
  else if (vconjq(N(2),"-ed"))
	L("pos") = "VBD";
  else if (vconjq(N(2),"-en"))
	L("pos") = "VBD";
  else
	L("pos") = "VBP";
  alphatoverb(2,0,L("pos"));
  if (!N("mypos",3))
    {
    if (N(4))
      chpos(N(3),"IN");	# prep/IN
    else if (S("num") == 1 # PHRASAL.
     || S("num") == 3)
      chpos(N(3),"RP");	# prep/RP
    else
	  chpos(N(3),"IN");	# prep/IN
	}
@RULES
_xNIL <-
	_xWILD [one fail=( \, _det )]
	_xALPHA
	_prep [lookahead]
	_prep [opt]
	@@

# dqan time
# Note: Idiom
@PRE
<6,6> var("cap");
@POST
  N("ellipted-that",6) = 1;
  if (literal(N(5)))
    fixnphead(5);
  L("tmp5") = N(5);
  group(1,5,"_np");
  pncopyvars(L("tmp5"),N(1));
  clearpos(N(1),1,1);
  # Really it's equivalent to "when"!
  group(1,1,"_whword");
  N("stem",1) = "when";
  group(1,1,"_fnword");
@RULES
_xNIL <-
    _xWILD [plus match=(_det _pro)]			### (1)
    _xWILD [star match=(_quan _num _xNUM)]	### (2)
    _adj [star]								### (3)
    _noun [star]							### (4)
    time [s]								### (5)
    _noun [lookahead]						### (6)
    @@

# dqan
# Note: plain dqan
@PRE
<1,1> varz("prosubj");
<1,1> varz("proobj");
# Only possessive pronoun, if _pro.
@POST
  dqaninfo(1,2,3,4);
  groupnp();
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro)]
 	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_xALPHA _noun _conj \, _aposS _aposX _adj)]
	@@

# prep alpha conj
@PRE
<1,1> varne("stem","to");
<2,2> var("noun");
@CHECK
  if (N("verb",2))
	{
    if (vconjq(N(2),"-ing"))
	  fail();	# Ambig.
	}
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_conj [lookahead]
	@@

# to dqan alpha $
@PRE
<1,1> vareq("stem","to");
@CHECK
  dqaninfo(2,3,4,5);
  if (!numbersagree(S("first"),N(6)))
	fail();
@POST
  fixnphead(6);
@RULES
_xNIL <-
	_prep
	_xWILD [opt match=(_det _pro)]
 	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_dbldash _xEND)]
	@@

# where noun alpha*
@PRE
<3,3> var("verb");
@CHECK
# Todo: nvagree.
  if (plural(N(2)))
  	{
	if (!vconjq(N(3),"inf") && !vconjq(N(3),"-ed"))
		fail();
	}
  else
    {
	if (!vconjq(N(3),"-s") && !vconjq(N(3),"-ed"))
		fail();
	}
@POST
  alphatoverb(3,"active","VBP");
@RULES
_xNIL <-
	where [s]
	_noun
	_xALPHA
	_xALPHA [star]
	_xWILD [one lookahead match=(_fnword _prep)]
	@@

# noun conj alpha , adj
# Note: Part of a complex noun or list.
@PRE
<3,3> var("noun");
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	\, [lookahead]
	_xWILD [one match=(_adj _adjc)]
	@@

# which alpha alpha adj
@PRE
<2,2> var("verb");
@CHECK
  if (N("adv",2) && N("verb",3))
  	fail();	# Another interp.
  if (!N("noun",3) && !N("adj",3))
  	fail();
  if (vconjq(N(2),"-ing"))
  	fail();
@POST
  alphatovg(2,0,"VBP");
@RULES
_xNIL <-
	which [s]
	_xALPHA
	_xALPHA [lookahead]
	@@

# pro alpha conj
@PRE
<2,2> var("noun");
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_proPoss [s]
	_xALPHA
	_conj [lookahead]
	_xWILD [one match=(_np _det _quan _num _xNUM)]
	@@

# verb alpha conj alpha
@PRE
<3,3> varz("pos25 v-a-conj-a");
@POST
  N("pos25 v-a-conj-a",3) = 1;
  if ((N("noun",2) || N("adj",2)) && !N("verb",2))
  	{
	if (!N("adj",4))
	  alphatonoun(2);
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_conj [lookahead]
	_xALPHA
	@@

# verb to alpha
# be to alpha
# have to alpha
@PRE
<3,3> var("verb");
<3,3> varz("noun");
@CHECK
  if (N("stem",1) != "be" && N("stem",1) != "have")
  	fail();
  if (!vconjq(N(3),"inf"))
  	fail();
@POST
  alphatoverb(3,"active","VB");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_xALPHA
	@@


# Hard-wired development set item.
# (Not to raise score, but because every error is reviewed
# over and over.)
@PRE
<4,4> var("verb");
@CHECK
  if (!vconjq(N(4),"-ed"))
  	fail();
@POST
  alphatovg(4,"active","VBD");
@RULES
_xNIL <-
	I [s]
	say [s]
	_dblquote
	_xALPHA
	_noun
	_dblquote
	@@


###########################
# JOBDESC	# 11/13/12 AM.
# Ex:	^ Designs and oversees	# JOBDESC	# 11/13/12 AM.
# ^ cap and verb
# Todo: agreement among the two verbs.
@PRE
<2,2> var("verb");	# Can be a verb.
@POST
  alphatovg(2,"active",0);
@RULES
_xNIL <-
	_xSTART
	_xCAP
	_xWILD [one lookahead match=(_conj)]
	_xWILD [one match=(_verb _vg)]
	@@
