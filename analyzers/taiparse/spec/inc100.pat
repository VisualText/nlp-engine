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

@PATH _ROOT _TEXTZONE _sent

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

# ^ verb that verb
@CHECK
  if (N("mypos",3))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  chpos(N(3),"DT");	# that/DT.
  group(3,3,"_np");
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
# ^ fnword dqan alpha alpha
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
@CHECK
  if (!N("verb",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_pro _noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [s one match=(that)]
	@@

# ^ pro alpha
@CHECK
  if (!N("verb",3))
    fail();
@POST
  alphatoverb(3,"active","VBD");
@RULES
_xNIL <-
	_xSTART
	_pro
	_xALPHA
	_xWILD [one lookahead fail=(_modal _verb _vg)]
	@@

# ^ dqan alpha prep
@CHECK
  if (N("adj",6) || N("noun",6))
    fail();
  # Agreement...
  if (!N("verb",6))
    fail();
  if (vconjq(N(6),"-ing"))
    fail();
@POST
  L("tmp6") = N(6);
  group(6,6,"_verb");
  pncopyvars(L("tmp6"),N(6));
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

# ^ dqan alpha alpha dqan
# eg: court let stand a ruling
# let
# stand
@POST
  L("tmp6") = N(6);
  L("tmp7") = N(7);
  group(6,6,"_verb");
  pncopyvars(L("tmp6"),N(6));
  chpos(N(6),"VBD");
  group(7,7,"_verb");
  pncopyvars(L("tmp7"),N(7));
  chpos(N(7),"VB");
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

# ^ alpha det
@CHECK
  if (N("verb",2) || N("noun",2))
    fail();
  if (!N("adj",2) || !N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  fixadv(N(2));
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [one lookahead match=(_det)]
	@@

# ^ alpha verb
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
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
       alphatoverb(3,"active","VBD");
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
  if (!N("verb",2))
    fail();
@POST
  alphatoverb(2,"active","VBD");
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_adj
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

