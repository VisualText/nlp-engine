###############################################
# FILE: inc100.pat
# SUBJ: Incremental left-to-right passes.
# AUTH: AM
# CREATED: 15/Jun/06 14:52:05
# NOTE: Basis for an incremental analyzer.
# NLP++ TODO: Incremental analyzer pass type.
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _sent
@NODES _sent

# ^ alpha ,
@PRE
<2,2> var("adv");
<2,2> varz("inc100 ^-alpha-comma");
@POST
  alphatoadv(2);
  N("inc100 ^-alpha-comma",2) = 1;
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	\, [lookahead]
	@@

# TOO BROAD. Could be imperative etc.	# 11/13/12 AM.
# ^ alpha
#@PRE
#<2,2> varz("inc100 alpha");
#@POST
#  N("inc100 alpha",2) = 1;
#  if (plural(N(2)))
#	{
#	if (N("verb",2))
#		{
#		# Clear out verb.
#		N("verb",2) = 0;
#		--N("pos num",2);
# 		alphaunambigred(2);
#		}
#	}
#@RULES
#_xNIL <-
#	_xSTART
#	_xALPHA
#	@@

# ^ alpha that
@CHECK
  if (!N("verb",2))
    fail();
@POST
 L("tmp2") = N(2);
 group(2,2,"_verb");
 pncopyvars(L("tmp2"),N(2));
 # Todo: reduce to vg.
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	that [s lookahead]
	@@

# ^ np that alpha
# ^ noun that alpha
@PRE
<4,4> var("verb");
@CHECK
 if (!vconjq(N(4),"inf") && !vconjq(N(4),"-s"))
	fail();
@POST
  alphatoverb(4,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_np _noun)]
	that [s]
	_xALPHA
	@@

# ^ verb that verb
@CHECK
  if (N("mypos",3))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  chpos(N(3),"DT");	# that/DT.
  nountonp(3,1);
  N("number",3) = "singular";
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_verb _vg)]
	that [s except=(_np _det)]
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# ^ verb that alpha prep
@CHECK
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  if (!N("mypos",3))
    chpos(N(3),"DT");	# that/DT
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_verb _vg)]
	that [s]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# ^ that noun
@CHECK
  if (N("mypos",2))
    fail();
@POST
  if (singular(N(4)))
    {
	chpos(N(2),"DT");	# that/DT.
	pnrename(N(2),"_det");
	}
  else
   chpos(N(2),"IN");	# that/IN.
@RULES
_xNIL <-
	_xSTART
	that [s except=(_det)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_noun)]
	@@

# ^ that alpha noun
@CHECK
  if (!N("verb",3))
    fail();
  S("ed") = vconjq(N(3),"-ed");
  if (!S("ed")
   && !vconjq(N(3),"-s"))
    fail();
@POST
  if (!N("mypos",2))
    chpos(N(2),"DT");	# that/DT
  pnrename(N(2),"_det");
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  if (S("ed"))
    chpos(N(3),"VBD");
  else
    chpos(N(3),"VBZ");
@RULES
_xNIL <-
	_xSTART
	that [s]
	_xALPHA
	_noun [lookahead]
	@@

# ^ that alpha
@PRE
<2,2> varz("mypos");	# 05/04/07 AM.
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (N("verb",3))
	{
	if (!vconjq(N(3),"inf"))
		fail();
	}
@POST
  if (!N("mypos",2))
    chpos(N(2),"DT");	# that/DT
  pnrename(N(2),"_det");
@RULES
_xNIL <-
	_xSTART
	that [s]
	_xALPHA
	@@

# ^ fnword det alpha alpha
# ^ fnword dqa alpha alpha
# NIBBLE.
@POST
 # Need agreement, plurality checks.
 if (N("noun",6) && N("adj",6))
   {
   if (plural(N(6)))
     alphatonoun(6);
   else if (N("noun",7) || N("adj",7))
     alphatoadj(6);
   else
     fixnphead(6);
   }
 else if (N("noun",6))
   alphatonoun(6);
 else if (N("adj",6))
   alphatoadj(6);
 else if (N("adv",6))
   alphatoadv(6);
 else
   alphatoadj(6);	# Probly some verby form.
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_fnword)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	@@

# ^ noun modal alpha
@CHECK
  if (!N("verb",6))
    fail();
  if (!vconjq(N(6),"inf"))
    fail();
@POST
  L("tmp6") = N(6);
  group(6,6,"_verb");
  pncopyvars(L("tmp6"),N(6));
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_pro _noun _np)]
	_xWILD [star match=(_adv _advl)]
	_modal
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_noun _np _pro)]
	@@

# ^ noun alpha that
# ^ pro alpha that
# that
@PRE
<4,4> var("verb");
@POST
  alphatovg(4,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_pro _noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [s one match=(that)]
	@@

# ^ dqan alpha that
@PRE
<7,7> var("verb");
@POST
  alphatovg(7,"active","VBP");
@RULES
_xNIL <-
    _xSTART										### (1)
    _xWILD [star match=(_det _pro)]				### (2)
    _xWILD [star match=(_quan _num _xNUM)]		### (3)
    _adj [star]									### (4)
    _noun [plus]								### (5)
    _xWILD [star match=(_adv _advl)]			### (6)
    _xALPHA										### (7)
    _xWILD [star lookahead match=(_adv _advl)]	### (8)
    that [s one]								### (9)
    @@


# ^ pro alpha
@CHECK
  if (!N("verb",3))
    fail();
@POST
  alphatoverb(3,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_proSubj [s]
	_xALPHA
	_xWILD [one lookahead fail=(_modal _verb _vg)]
	@@

# ^ noun alpha
@PRE
<4,4> var("verb");
@CHECK
  if (!plural(N(2)))
	fail();
  if (!vconjq(N(4),"inf"))
	fail();
@POST
  alphatovg(4,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_noun _np)]
	_adv [star]
	_xALPHA
	@@

# ^ np alpha alpha
@PRE
<4,4> var("verb");
@CHECK
  if (pnname(N(2)) == "_pro" && !N("prosubj",2))
  	fail();
  if (vconjq(N(4),"-ing") || vconjq(N(4),"-en"))
	fail();
  if (singular(N(2)) && vconjq(N(4),"inf"))
	fail();
  if (plural(N(2)) && vconjq(N(4),"-s"))
	fail();
@POST
  alphatovg(4,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_noun _np _pro)]
	_adv [plus]
	_xALPHA
	@@

# ^ dqan alpha dqan
@PRE
<6,6> var("verb");
@POST
  # todo: Check agreement.
  alphatovg(6,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_det _pro _quan _num _xNUM
		\, )]
	@@

# ^ dqa alpha ,
# Apposition
@PRE
<5,5> var("noun");
@POST
  fixnphead(5);
@RULES
_xNIL <-
	_xSTART
    _xWILD [star match=(_det)]				### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]								### (4)
	_xALPHA
	\,
	@@

# ^ dqa alpha noun
@PRE
<5,5> var("noun");
<6,6> vareq("sem","person");
@CHECK
  if (!N(2) && !N(3) && !N(4))
  	fail();
@POST 
  fixnphead(5);
  dqaninfo(2,3,4,5);
  groupnp();
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_noun
	@@

# ^ dqan alpha as verb
# Ex: People regarded as being weird....
@PRE
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"-en"))
	fail();
@POST
  alphatovg(6,"passive","VBN");
  N("ellipted-that-is",6) = 1;
#  L("tmp6") = N(6);
#  group(6,6,"_clause");
#  setunsealed(6,"true");	# 07/10/12 AM.
#  N("voice",6) = "passive";
#  N("vg node",6) = L("tmp6");
  chpos(N(7),"IN");	# as/IN
  pnrename(N(7),"_fnword");	# prep -> fnword
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	as [s]
 	_xWILD [one match=(_verb _vg)]
	@@
	
# ^ dqan alpha prep
@CHECK
  if (!N("verb",6))
    fail();
  if (phrprepverbq(N(6),N(7)))
	succeed();
  if (N("adj",6) || N("noun",6))
    fail();
  # Agreement...
  if (vconjq(N(6),"-ing"))
    fail();
@POST
  alphatovg(6,0,0);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# ^ dqan alpha alpha dqan [idiom: let stand]
# eg: court let stand a ruling
# let
# stand
@POST
  alphatoverb(7,"active","VB");		# Cleanup.	# 05/28/07 AM.
  alphatoverb(6,"active","VBD");	# Cleanup.	# 05/28/07 AM.
  group(6,7,"_vg");
  N("voice",6) = "active";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	let
	stand
	_xWILD [one lookahead match=(_det _pro _quan _num _xNUM
		_adj _noun _np _adv _advl)]
	@@

# ^ dqan alpha adv alpha
@PRE
<4,4> var("noun");
<6,6> var("verb");
@CHECK
  if (!plural(N(4)))
	fail();
  if (!vconjq(N(6),"inf"))
	fail();
@POST
  alphatoverb(6,"active","VBP");
  fixnphead(4);
@RULES
_xNIL <-
	_xSTART
	_adj
	_noun
	_xALPHA
	_adv
	_xALPHA
	_xWILD [one lookahead match=(_fnword)]
	@@

# ^ dqan alpha
@CHECK
  if (N("verb",4))
    fail();
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (singular(N(3)) && singular(N(4)))
    succeed();
  fail();
@POST
  alphatonoun(4);
@RULES
_xNIL <-
	_xSTART
	_det [opt]
	_noun
	_xALPHA
	@@

# ^ dqan alpha alpha noun
@PRE
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"-ed"))
  	fail();
  if (!N("noun",6) && !N("adj",6))
  	fail();
@POST
  alphatovg(6,"active","VBD");
@RULES
_xNIL <-
    _xSTART									### (1)
    _xWILD [star match=(_det _pro)]			### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]								### (4)
    _noun [plus]							### (5)
    _xALPHA									### (6)
    _xALPHA									### (7)
    @@

# Special cases. Can't resolve yet.
@POST
  noop();
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(following)]
	@@

# ^ alpha det
@PRE
<2,2> varz("inc100 a-det");
@POST
  L("done") = 0;
  if (!L("done") && N("verb",2))
  	{
	# Presumably gerund tagging will dominate in Treebank.
	if (vconjq(N(2),"-ing"))	# Gerund case.
	   {
	   alphatovg(2,"active","VBG");	# Conform Treebank.
	   L("done") = 1;
	   }
	else if (vconjq(N(2),"inf"))
	  {
	  alphatovg(2,"active","VB");
	  L("done") = 1;
	  }
	}

  if (!L("done") && N("adv",2))
  	{
    alphatoadv(2);
	L("done") = 1;
	}

  if (!L("done") && N("adj",2))
  	{
	alphatoadj(2);
	L("done") = 1;
	}
  N("inc100 a-det",2) = 1;
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [one lookahead match=(_det)]
	@@

# ^ alpha verb
@PRE
<2,2> varz("inc100 av");
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
@POST
  if (singular(N(2)) && !N("mass",2) && !N("mypos",2))
	cappos(N(2),0);
  fixnphead(2);
  N("inc100 av",2) = 1;	# Loop guard.
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_verb _vg _modal)]
	@@

# ^ alpha , pro
@CHECK
  if (!N("adv",2))
    fail();
@POST
  alphatoadv(2);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	\, [lookahead]
	_pro
	@@

# ^ alpha alpha verb
@PRE
<2,3> var("noun");	# Require possible nouns.
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# ^ alpha alpha ,
@PRE
<3,3> var("noun");	# Require possible noun.
@CHECK
  if (!N("adj",2) && !N("noun",2))
   fail();
  if (N("verb",3))
    {
    if (!vconjq(N(3),"inf")
     && !vconjq(N(3),"-s"))
      fail();
    }
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=( \, )]
	@@

# ^ alpha alpha pro
@PRE
<2,3> var("noun");	# Require possible nouns.
@CHECK
  if (!singular(N(2)) || N("unknown",2))
    fail();
  # Todo: not proper name.
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xWILD [s one lookahead match=(
		himself herself itself themselves ourselves yourselves
		_noun _verb _vg _adv
		)]
	@@

# ^ alpha alpha prep
# Note: ellipsis.  "people happy with ..."
@CHECK
 if (N("inc100 a-a-prep",2))
   fail();
@POST
 N("inc100 a-a-prep",2) = 1;
 if (N("noun",2))
   {
   if (plural(N(2)))
     {
	 if (N("verb",3))
       {
       alphatoverb(3,"active","VBP");
       fixnphead(2);
	   }
	 else if (N("adj",3))
	   {
	   alphatoadj(3);
	   fixnphead(2);
	   }
    }
   }

@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# ^ alpha alpha adj
@CHECK
  if (!N("verb",3) || !N("noun",2))
    fail();
  if (!vconjq(N(3),"-ed"))
	fail();
@POST
    alphatoverb(3,"active","VBP");
	if (singular(N(2)) && !N("mass",2))
		cappos(N(2),0);
	fixnphead(2);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_det _quan _num _xNUM _adj _np)]
	@@

# question patterns.
# why are alpha
@POST
  if (N("noun",4))
    {
	if (plural(N(4)))
	  fixnphead(4);
	else
	  fixnpnonhead(4);
	}
  else if (N("adj",4))
    fixnpnonhead(4);
  else if (N("adv",4))
    {
	L("tmp4") = N(4);
	group(4,4,"_adv");
	pncopyvars(L("tmp4"),N(4));
	fixadv(N(4));
	}
  else  # verb or otherwise.
    fixnpnonhead(4);
@RULES
_xNIL <-
	_xSTART
	_xWILD [s one match=(_whword)]
	are [s]
	_xALPHA
	@@

# ^ yet
@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"CC");	# yet/CC
  pnrename(N(2),"_adv");
@RULES
_xNIL <-
	_xSTART
	yet [s]
	@@

# ^ adj alpha alpha [NIBBLE]
@CHECK
  if (!N("adj",3) && !N("noun",3))
    fail();
@POST
  if (singular(N(3)))
    fixnpnonhead(3);
  else
    fixnphead(3);
@RULES
_xNIL <-
	_xSTART
	_adj
	_xALPHA
	_xALPHA
	@@

# ^ noun 's alpha
@CHECK
  if (!N("adj",4))
	fail();
@POST
  alphatoadj(4);
  pnrename(N(3),"_verb");
  chpos(N(3),"VBZ");	# 's/VBZ
  N("copula",3) = 1;
@RULES
_xNIL <-
	_xSTART
	_noun
	_aposS
	_xALPHA
	_xWILD [one lookahead match=(_dbldash \, \; _qEOS)]
	@@

# ^ noun verb noun , alpha noun
@PRE
<6,6> var("verb");
@CHECK
  L("v") = N("verb node",3);
  S("c") = samevconj(L("v"),N(6));
  if (!S("c"))
	fail();
@POST
  alphatovg(6,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	_xWILD [one lookahead match=(_noun _np _det _quan
 		_num _xNUM _adj _xALPHA)]
	@@

# to alpha alpha dqan
@PRE
<2,2> var("adv");
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"inf"))
	fail();
@POST
  alphatovg(3,"active","VB");
  alphatoadv(2);
@RULES
_xNIL <-
	to [s]
	_xALPHA
 	_xALPHA
	_xWILD [one lookahead match=(_det _pro _quan _num _xNUM _np)]
	@@

# ^ np , dqan alpha ,
@PRE
<6,6> var("verb");
@CHECK
  if (plural(N(5)) && vconjq(N(6),"-s"))
	fail();
  if (singular(N(5)) && vconjq(N(6),"inf"))
	fail();
  if (!verbfeat(N(6),"T5"))
	fail();
@POST
  alphatovg(6,0,"VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_np _noun)]
	\,
	_det
	_noun
	_xALPHA
	\, [lookahead]
	@@

# ^ alpha num
@PRE
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"inf"))
	fail();
@POST
  alphatovg(2,"active","VB");
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [one lookahead match=(_quan _num _xNUM)]
	@@

# ^ and then
@PRE
<3,3> vareq("stem","then");
@POST
  group(2,3,"_adv");
@RULES
_xNIL <-
	_xSTART
	_conj
	_fnword [opt]
	@@

# ^ before alpha
# ^ before ving
@PRE
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"-ing"))
  	fail();
@POST
  alphatovg(3,"active","VBG");
@RULES
_xNIL <-
	_xSTART
	before [s]
	_xALPHA
	@@


# Sentinel.
@POST
  # When done matching rules against starts,
  # finish with current segment.
  # NLP++ opt: would be nice to have a way to exit the
  # current dominating context (ie, the current _sent).
  # something like an xcontinue() or xnext() function.
  noop();
@RULES
_xNIL <-
	_xANY [plus]
	@@

