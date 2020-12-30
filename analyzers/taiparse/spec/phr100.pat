###############################################
# FILE: phr100.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 09/Jul/04 22:56:34
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _sent _clause
@NODES _clause

@CHECK
  if (N("verb",2))
    fail();
  if (!N("adj",2))
    fail();
  if (!N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  L("tmp") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp"),N(2));
  fixadj(N(2));
  L("tmp3") = N(3);
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	to [s]
	_xALPHA
	_xALPHA
	_xEND
	@@

# prep np
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  group(1,3,"_advl");
  pncopyvars(L("tmp3"),N(1));
  clearpos(N(1),1,0);
  N("pp",1) = 1;
#  sempp(N(1),L("tmp1"),L("tmp3"));
@RULES
_xNIL <-
	_prep
	_adv [star]
	_xWILD [s one match=(_np) except=(_proSubj)]
	_xWILD [one lookahead fail=(_conj)]
	@@
_xNIL <-
	_prep
	_adv [star]
	_xWILD [s one match=(_np) except=(_proSubj)]
	_xEND
	@@

# noun vg
@POST
  nountonp(2,1);
  if (pnname(N(4)) == "_vg")
    if (!N("voice",4))
	  X("voice") = N("voice",4) = "active";
@RULES
_xNIL <-
	_xSTART
	_noun
	_xWILD [star lookahead match=(_adv _advls)]
	_xWILD [one match=(_verb _vg)]
	@@

# alpha noun advl vg
# alpha noun vg
# start np vs vg.
@CHECK
  if (!N("verb",2))
    fail();
  if (!N("adj",2) && !N("noun",2))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp2") = N(2);
  if (N("adj",2))
    group(2,2,"_adj");
  else if (N("noun",2))
    group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_noun
	_xWILD [star lookahead match=(_adv _advl)]
	_vg
	@@

# pro noun alpha alpha
@CHECK
  if (X("vg count") == 0)
    fail();
  if (!N("noun",3))
    fail();
  if (!N("noun",4) && !N("adv",4))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  if (N("adv",4))
    {
    group(4,4,"_adv");
	L("short") = 1;
	}
  else
    group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  if (!L("short"))
    group(1,4,"_np");
  else
    group(1,3,"_np");
  clearpos(N(1),1,1);
@RULES
_xNIL <-
	_proPoss [s]
	_noun
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_xEND _qEOS)]
	@@

# vg np prep dqan alpha
@CHECK
  if (!N("noun",5))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  fixnoun(N(5));
  group(4,5,"_np");
  pncopyvars(L("tmp5"),N(4));
  clearpos(N(4),1,1);
@RULES
_xNIL <-
	_vg
	_np
	_prep
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun)]
	_xALPHA
	_xWILD [one lookahead fail=(_adj _noun)]
	@@



# yields on x continued
# alpha advl vg
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
  nountonp(2,1);
  X("vg node") = N(4);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_vg
	@@

# Heur: If there's one possible verb and clause
# needs a verb, then assign it...
# Todo: Implement the above in a general way.
# Heur: Weaker is to prefer at least one verb in a clause.
@CHECK
  if (!X("need verb"))
    fail();
  if (!N("verb",5))
    fail();
@POST
  L("x3") = pnparent(X());		# 07/13/12 AM.
  L("tmp5") = N(5);
  group(5,5,"_verb");
  pncopyvars(L("tmp5"),N(5));
  L("v") = N(5);
  group(5,5,"_vg");
  mhbv(N(5),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp5"),N(5));
  N("verb node",5) = L("v");
  fixvg(N(5),X("need voice"),X("need inf"));
  clearpos(N(5),1,0);

  X("need verb") = 0;	# Reset.
  ++X("vg count");
  L("ct") = pnvar(L("x3"),"vg count");			# 07/13/12 AM.
  pnreplaceval(L("x3"),"vg count",++L("ct"));	# 07/13/12 AM.
  X("vg node") = N(5);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [star match=(_det _quan _num _xNUM _adj _noun _np)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [star match=(_advl _adv)]
	_xWILD [star match=(_det _quan _num _xNUM _adj _noun _np)]
	_xWILD [star match=(_advl _adv)]
	_xEND
	@@


# Try something shallow and assess... # 01/08/05 AM.
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ed"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("verb node",2) = L("v");
  N("first verb",2) = L("v");
  X("voice") = N("voice",2) = "active";
  X("vg node") = N(2);
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _adj)]
	@@

# vg dqan
@CHECK
  if (!N("noun",5))
    fail();
  if (N("adv",5))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  fixnoun(N(5));
  group(2,5,"_np");
  pncopyvars(L("tmp5"),N(2));
  clearpos(N(2),1,1);

  # Fix up clause counts.
  # Todo: Really should flag; then recount in another pass.
  if (X("last chunk") == "v")
    {
	X("last chunk") = "n";
	X("last") = N(2);
	}
@RULES
_xNIL <-
	_xWILD [one match=(_vg)]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_xWILD [star match=(_adj _noun)]
	_xALPHA
	_xWILD [one lookahead match=(_adv _advl _np _vg _xEND)]
	@@

# Pick off the first alpha as verb, if verb needed.
# SHALLOW HEURISTIC, MAY NEED REFINING.
@CHECK
  if (!N("verb",3))
    fail();
  if (!X("need verb"))
    fail();
  S("vc") = vconj(N(3));
  if (X("need voice") == "active")
    {
	if (S("vc") == "-en")
	  fail();
	}
  else if (X("need voice") == "passive")
    {
	if (S("vc") != "-edn"
	 && S("vc") != "-en")
	   fail();
	}
  # Should check -s vs inf requirement...
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,0);
  N("verb node",3) = N("first verb",3) = L("v");
  X("vg node") = N(3);
  if (X("need voice") == "active")
    {
	X("voice") = "active";
	fixvg(N(3),"active","VBP");
	}
  else if (X("need voice") == "passive")
    {
	X("voice") = "passive";
	fixvg(N(3),"passive","VBN");
	}
  X("need verb") = 0;
  ++X("vg count");
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	@@

# Some repetition of pos50 style here.....
@POST
  singler(2,4);
@RULES
_np <-
	_xWILD [plus match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xWILD [plus match=(_adj _noun)]
	_xWILD [one lookahead fail=(_xALPHA)]
	@@

@POST
  singler(2,4);
  S("seg type") = "np";
@RULES
_seg <-
	_xWILD [plus match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xWILD [plus match=(_xALPHA _adj _aposS _noun)]
	@@

# dqan alpha dqan
@CHECK
  if (X("vg count"))
    fail();
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("first verb",2) = N("verb node",2) = L("v");
  fixvg(N(2),"active","VBP");
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM
				_adj _noun _np)]
	@@

# dqan
@CHECK
  if (N("phr100 an",4))
    fail(); 
  dqaninfo(1,2,3,4);
  L("prev") = pnprev(S("first"));
  if (literal(L("prev")))
    fail();
  if (!numbersagree(S("first"),S("last")))
    fail();
@POST
  L("n") = N(4);
  pnreplaceval(L("n"),"phr100 an",1);  # Loop guard.
  groupnp();
@RULES
_xNIL <-
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_xALPHA _aposS)]
	@@

# ^ alpha
@POST
  L("x3") = pnparent(X());			# 07/13/12 AM.
  if (pnvar(L("x3"),"conditional"))	# 07/13/12 AM.
	alphatoverb(2,"active","VB");
  else
	fixnphead(2);
@RULES
_xNIL <-	# 05/08/07 AM.
 	_xSTART
	_xWILD [one match=(double triple)]
	@@
