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

@PATH _ROOT _TEXTZONE _sent

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

  if (N("unknown",3))
    alphatonoun(3);
  else
    {
	N("verb",3) = 0;
	--N("pos num",3);
	if (N("noun",3) && N("pos num",3) == 1)
 		alphatonoun(3);	# 04/21/07 AM.
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
	N("verb",2) = 0;
	--N("pos num",2);
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
  L("tmp3") = N(3);
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);
#  semcomposenp(N(3),L("tmp3"),L("tmp3"));
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [one lookahead match=(_modal _have _be _verb)]
	@@

# mhbv
# Complete vg.
@CHECK
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(9));
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

  L("neg") = mhbvadv(2,4,6,8);
  group(1,9,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_xWILD [s one match=(_modal _do)]
	_xWILD [star match=(_adv _advl)]
	_have [s opt]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(_verb) except=(_modal _have _be _vg)]
	@@

# prep alpha dqan
# prep ving dqan
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  clearpos(N(3),1,0);
  fixvg(N(3),"active",0);
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
  if (!N("noun",2))
    fail();
  if (N("verb",2))
    if (vconjq(N(2),"-ing"))
	  fail();
@POST
  alphatonoun(2);
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xALPHA
	_xWILD [one lookahead match=(\, _clausesep)]
	@@

# prep alpha [NIBBLE]
@CHECK
  if (!N("verb",2))
    fail();
  if (vconjq(N(2),"-ing"))
    fail();
@POST
  --N("pos num",2);
  N("verb",2) = 0;
  if (N("pos num",2) == 1)
    {
	if (N("noun",2))
	  alphatonoun(2);
	else if (N("adj",2))
	  alphatoadj(2);
	else if (N("adv",2))
	  alphatoadv(2);
	}
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
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  N("cap",2) = 1;
  fixnoun(N(2));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
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
	_xWILD [one match=(_det _pro _quan _num _xNUM)]
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
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
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
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# , said
@POST
  L("tmp3") = N(3);
  L("txt") = strtolower(N("$text",3));
  group(3,3,"_verb");
  L("v") = N(3);
  pncopyvars(L("tmp3"),N(3));
  if (L("txt") == "said")
    chpos(N(3),"VBD");
  else if (L("txt") == "says")
    chpos(N(3),"VBZ");
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  N("voice",3) = "active";
  N("glom",3) = "left";
  clearpos(N(3),1,0);
  if (L("next") = pnnext(N(3)))
    pnreplaceval(L("next"),"glom","right");
@RULES
_xNIL <-
	_xWILD [one match=( \, _dblquote _qEOS _xSTART _conj _clause _noun _np)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(say says said)]
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
	_xWILD [one match=(_verb say says saying said)]
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
 	_noun
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
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
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
@POST
  if (pnname(N(2)) == "_verb")
    pnrename(N(2),"_prep");
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
@POST
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
  fixverb(N(2),"active","VBD");
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
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);

  L("tmp1") = N(1);
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,1);
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
    chpos(N(1),"RBS");
@RULES
_xNIL <-
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
@CHECK
  if (N("mypos",1))
    fail();
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

# conj alpha prep
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
@PRE
<2,2> var("cap");
@POST
  pnrename(N(2),"_adj");
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
  group(2,2,"_np");
  clearpos(N(2),1,1);
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
  group(1,1,"_np");
@RULES
_xNIL <-
	this [s]
	_adv [star]
	_xWILD [one match=(_advl _np _vg
		_pro _modal _verb _fnword _prep _conj
		)]
	@@

# to alpha
@POST
  if (N("unknown",2))
    alphatonoun(2);
  else if (N("verb",2))
    {
	if (vconjq(N(2),"-en")
	 || vconjq(N(2),"-ing"))
	  alphatoadj(2);
	else if (vconjq(N(2),"inf")
	 && !plural(N(2)) )
	  {
	  # Still ambiguous.	# 04/23/07 AM.
#	  alphatoverb(2,"active","VB");	# 04/23/07 AM.
	  }
	else if (N("noun",2))
	  alphatonoun(2);
	}
  else if (N("adj",2))
    alphatoadj(2);
  else if (N("noun",2))
    alphatonoun(2);
  else if (N("adv",2))
    alphatoadv(2);
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

@POST
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
  if (N("sem",2) != "date"
   && !N("cap",2))
    fail();
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
    if (singular(N(2)) && singular(N(3)))
      succeed();
	fail();
	}
@POST
  alphatonoun(3); # adjnoun thingy.
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_xWILD [one match=(_noun _np)]
	_xALPHA
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
  pnrename(N(2),"_adj");
  chpos(N(2),"JJ");
@RULES
_xNIL <-
	_xWILD [s one match=(a the)]
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
	N("verb",2) = 0;
	--N("pos num",2);
	}
  if (N("pos num",2) == 1)
    {
	if (N("noun",2))
	  alphatonoun(2);
	else if (N("adj",2))
	  alphatoadj(2);
	else if (N("adv",2))
	  alphatoadv(2);
	else
	  "err.txt" << "POS? for=" << N("$text",2) << "\n";
	}
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
	_adj [lookahead]
	_xWILD [one fail=(_xALPHA _adj _noun)]
	@@
_xNIL <-
	_xWILD [plus match=(_det _pro _quan _num _xNUM _adj _noun)]
	_xALPHA
	_adj [lookahead]
	_xEND
	@@

# how many alpha
@POST
  if (N("noun",2) && N("adj",2))
    {
	if (N("verb",2))
	  {
	  --N("pos num",2);
	  N("verb",2) = 0;
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
@CHECK
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
	N("verb",2) = 0;
	}
  if (N("pos num",2) == 1)
    {
	if (N("noun",2))
	  alphatonoun(2);
	else if (N("adj",2))
	  alphatoadj(2);
	else if (N("adv",2))
	  alphatoadv(2);
	}
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
@POST
  if (N("verb",1))
    {
	if (vconjq(N(1),"-edn"))
	  alphatoverb(1,0,0);
	}
  else if (N("pos num",1) == 2
   && N("noun",1) && N("adj",1))
     fixnphead(1);
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one match=(_xEND)]
	@@

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
	_xEND
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
