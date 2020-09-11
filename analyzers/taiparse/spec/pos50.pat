###############################################
# FILE: pos50.pat
# SUBJ: Hi-confidence recursive grammar.
# AUTH: AM
# CREATED: 14/Jul/04 01:57:26
# NOTE:	Hub for disambiguation and
#	high confidence grammar.
###############################################

@CODE
L("hello") = 0;

@@CODE

@PATH _ROOT _TEXTZONE _sent

@CHECK
  if (!N("possessive",2))
    fail();
@POST
  group(1,2,"_det");
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	_det
	_adj
	@@

# det noun adj
# Note: caps can serve as "adjectives".
@PRE
<2,2> var("cap");
@POST
  if (!N("mypos",2))
    chpos(N(2),"NN");
  pnrename(N(2),"_adj");
@RULES
_xNIL <-
	_det
	_noun
	_adj
	@@

@POST
  dqaninfo(0,0,0,2);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_noun
	_xWILD [one match=( \, )]
	@@



# dqan and dqan
# When there is local disagreement but overall agreement.
#@CHECK
#  if (!N(1) && !N(2) && !N(3) && !N(4))
#    fail();
#@POST
#  N("pos50 dqan-and-np",6) = 1;
#  if (N(1))
#    listadd(6,1,"true");
#  else if (N(2))
#    listadd(6,2,"true");
#  else if (N(3))
#    listadd(6,3,"true");
#  else
#    listadd(6,4,"true");
#@RULES
#_xNIL <-
#	_xWILD [star match=(_det _pro)]
#	_xWILD [star match=(_quan _num _xNUM)]
#	_adj [star]
#	_noun [plus]	# 02/19/05 AM.
#	_conj
#	_np
#	@@

# dqan alpha alpha
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3))
    fail();
  if (N("verb",2))
    {
	if (vconjq(N(2),"-ed")
	 || vconjq(N(2),"-s"))
	  fail();
	}
  if (N("verb",3))
    {
	L("vc") = vconj(N(3));
	if (L("vc") == "-ed"
	 || L("vc") == "-edn"
	 || L("vc") == "-en")
	 fail();
	}
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  else
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_noun)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND _clausesep _fnword)]
	@@

# 's alpha alpha noun verb
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
@POST
  fixnpnonhead(2);
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# noun , alpha alpha conj
@CHECK
  if (!N("noun",4))
	fail();
  if (!N("noun",3) && !N("adj",3))
	fail();
@POST
  fixnphead(4);
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	_xALPHA
	\, [opt]
 	_conj [lookahead]
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@
 
# 's alpha alpha prep
# 's alpha alpha conj
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("noun",3))
    fail();
@POST
  fixnpnonhead(2);
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _conj \, )]
	@@

# alpha det
# alpha dqan
@CHECK
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
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);
  fixvg(N(2),"active",0);
  posacct(L("v"));
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_det [lookahead]
	@@

# alpha dqan
# ving (adj,verb)
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
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  clearpos(N(3),1,0);
  fixvg(N(3),"active",0);
  posacct(L("v"));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\, [opt]
	_xALPHA
	_xWILD [one lookahead match=(_np _det _quan _num _xNUM)]
	@@

# be alpha alpha dqan
# is head account manager
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("adv",2))
    fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (N("verb",2))
    {
	S("vc") = vconj(N(2));
	if (S("vc") == "-ing"
	 || S("vc") == "-edn"
	 || S("vc") == "-en")
	  fail();	# Too likely a verb.
	}
@POST
  # Use the hot new function...
  fixnpnonhead(2);
  fixnpnonhead(3);
@RULES
_xNIL <-
	_be [s]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun _adj)]
	@@

# be alpha alpha
# was just waiting
@CHECK
  L("v") = vconj(N(3));
  if (L("v") != "-ing"
   && L("v") != "-edn"
   && L("v") != "-en")
    fail();
  if (!N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  posacct(N(3));
  posacct(N(2));
@RULES
_xNIL <- # 10
	_be [s]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# be adv alpha
@CHECK
  L("v") = vconj(N(3));
  if (L("v") != "-ing"
   && L("v") != "-edn"
   && L("v") != "-en")
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_be [s]
	_adv
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@


# prep alpha alpha
# by alpha alpha
@CHECK
  if (!N("noun",2) || !N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	by [s]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _xNUM _num
	    _adj _np
		_noun _pro _adv _advl)]
	@@

# for adv alpha
@CHECK
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	for [s]
	_adv
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# prep alpha alpha noun
# in Western populated ...
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("noun",2) || !N("adj",2)) # Need both of these.
    fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (N("verb",3))
    {
	S("vc") = vconj(N(3));
	if (S("vc") != "inf"
	 && S("vc") != "-s")
      fail();
	}
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  if (N("noun",3))
    {
    group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	}
  else # adj
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
#	if (N("verb",3))
	  fixadj(N(3));
	}
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xALPHA
	_noun
	@@

# prep alpha alpha noun
@PRE
<1,1> varne("stem","to");
<2,2> varz("pos50 p-a-a-n"); # Loop guard.
@POST
  N("pos50 p-a-a-n",2) = 1;
  if (!N("verb",2))
    {
	if ((N("adj",3) || N("noun",3))
	 && singular(N(2)) && !N("mass",2))
	  {
	  fixnpnonhead(2);
	  fixnpnonhead(3);
	  }
	}
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xALPHA
	_noun
	@@

# prep alpha alpha
# by alpha alpha
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
	S("vc") = vconj(N(3));
	if (S("vc") != "inf"
	 && S("vc") != "-s")
	   fail();
	}
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	by [s]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_np _fnword _verb _vg)]
	@@

# prep adv alpha
# prep alpha
@CHECK
  if (!N("adj",3))
    fail();
  if (plural(N(3)))
    fail();
@POST
  alphatoadj(3);
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_adv [plus]
	_xALPHA
	@@

# by alpha pro
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  N("first verb",2) = N("verb node",2) = L("v");
  pncopyvars(L("tmp2"),N(2));
  fixvg(N(2),"active","VBG");
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(by from _conj)]
	_xALPHA
	_xWILD [one lookahead match=(_pro _np)]
	@@

# vg prep np conj alpha alpha
@CHECK
 if(N("pos50 vpn-conj-aa",5))
   fail();	# Loop guard.
@POST
  N("pos50 vpn-conj-aa",5) = 1;
  L("v") = pnvar(N(1),"verb node");
  L("pos") = pnvar(L("v"),"mypos");
  L("voice1") = N("voice",1);
  # Todo: vn agreement versus nn agreement...
  if (N("verb",5))
    {
	if (singular(N(5)) && samevconj(L("v"),N(5)))
	  alphatoverb(5,L("voice1"),L("pos"));
	}
@RULES
_xNIL <-
	_vg
	_prep
	_np
	_conj
	_xALPHA
	_xALPHA
	@@

# noun conj alpha alpha
@CHECK
  if (N("verb",3) || N("mass",3))
    fail();
  if (!N("noun",3) || !N("adj",3))
    fail();
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (!singular(N(3)))
    fail();
@POST
  alphatoadj(3);
@RULES
_xNIL <- # 20
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_xALPHA
	@@

# conj alpha alpha .
@CHECK
  if (N("pos num",3) != 2)
    fail();
  if (!N("noun",3) || !N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_conj _prep)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# to alpha alpha noun
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
  if (!N("noun",3) && !N("adj",3) && !N("adv",3))
    fail();
@POST
  alphatoverb(2,"active","VB");
@RULES
_xNIL <-
	to [s]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	@@

# to alpha
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf")) # New function. # 06/15/06 AM.
    fail();
@POST
  L("tmp") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp"),N(2));
  chpos(N(2),"VB");	# Infinitive.
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp"),N(2));
  N("voice",2) = "active";
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	to [s]
	_xALPHA
	_xWILD [lookahead one match=(_conj _prep _fnword
		_det _quan _num _xNUM _adj _noun \,)]
	@@

# vg prep alpha noun
# verb prep alpha noun
@CHECK
  if (!N("verb",3) && !N("adj",3))
    fail();
  if (N("verb",3))
    {
	if (!vconjq(N(3),"-ing"))
      fail();
	}
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_prep
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# prep alpha
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("noun",2)  || !N("verb",2))
    fail();
  S("vc") = vconj(N(2));
  if (S("vc") != "inf"
   && S("vc") != "-s")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(_prep though) except=(to)]
	_xALPHA
	@@

# prep alpha
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("verb",2))
    {
	S("vc2") = vconj(N(2));
	if (S("vc2") != "inf"
	 && S("vc2") != "-s"
	 && S("vc2") != "-edn"
	 && S("vc2") != "-en")
	   fail();
	}
@POST
  fixnpnonhead(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# prep alpha
@CHECK
  if (!N("verb",2) || !N("adj",2))
    fail();
  S("vc2") = vconj(N(2));
  if (S("vc2") != "-edn"
   && S("vc2") != "-en")
     fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2)); # Probably starts np.
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xALPHA
	_xWILD [one lookahead fail=(_prep)]
	@@

# as noun verb
@CHECK
  if (N("mypos",2) == "IN")
    fail();
@POST
  chpos(N(2),"IN");
@RULES
_xNIL <-
	_xSTART
	as [s]
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [star match=(_det _quan _num _xNUM _adj)]
	_xWILD [plus match=(_noun)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	@@

# prep alpha adj
# Note: a 'normal' case!? # 04/23/07 AM.
@PRE
<2,2> var("adj");
<2,2> varz("verb");
@POST
  alphatoadj(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_adj [lookahead]
 	@@

# as alpha adj
# prep alpha adj
@CHECK
  if (N("stem",1) != "as")
    fail();
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
  if (N("mass",2)) # Mass noun.
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(1),"IN");
@RULES
_xNIL <-
	_fnword
	_xALPHA
	_adv [star lookahead]
	_adj
	@@

# prep alpha adj
# prep alpha dqan
# verb vs adj.
# Assume adj-adj, idioms handled prior to here.
@CHECK
  if (!N("verb",2))
    fail();
	if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <- # 30
	_xWILD [one match=(_prep)]
	_xALPHA
	_xWILD [one lookahead match=(_adj _pro)]
	@@

@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_prep
	even
	@@

# det noun alpha det
@CHECK
  if (!N("verb",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),0,0);
  L("v") = N(3);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
#  N("voice",3) = "active";
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_det
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_det)]
	@@

# det alpha adj
@POST
  L("tmp2") = N(2);
  if (N("num",2))	# Eg, "first".
    {
    group(2,2,"_num");
	chpos(N(2),"JJ");
	}
  else if (N("adj",2))
    group(2,2,"_adj");
  else if (N("noun",2))
    group(2,2,"_noun");
  else if (N("adv",2))
    group(2,2,"_adv");
  else
    group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
@RULES
_xNIL <-
	_det
	_xALPHA
	_xWILD [one lookahead match=(_adj _xNUM _num _quan)]
	@@

# det alpha alpha $
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3))
    fail();
@POST
  fixnpnonhead(2);
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_xWILD [one match=(_xEND)]
	@@

# det alpha alpha
@CHECK
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	}
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _xEND)]
	@@

# dqan alpha
# dqa alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (N("adj",2))	# 04/20/07 AM.
	fail();
  L("n") = lasteltnode(1);
#  if (pnvar(L("n"),"apos-s"))
#    succeed();
  if (N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _fnword _conj
		_pro \, _xEND)]
	@@

# adj alpha
@CHECK
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
	S("vc") = vconj(N(3));
	if (S("vc") != "inf"
	 && S("vc") != "-s")
	  fail();
	}
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
  dqaninfo(0,0,2,3);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_conj)]
	_adj
	_xALPHA
	_xWILD [lookahead one match=(_qEOS _verb _vg _xEND)]
	@@

# alpha
# Usually will be a noun, even if verb is possible.
@PRE
<1,1> varne("stem","each");
@CHECK
  if (!N("noun",2))
    fail();
  L("last") = lasteltnode(2);
  if (N(1))
    L("first") = N(1);
  else if (N(2))
    L("first") = N(2);
  if (!numbersagree(L("first"),L("last")))
    S("disagree") = 1;
@POST
  if (S("disagree"))
    {
	group(1,1,"_np");
	N("bracket",1) = 1;
	}
  else
    {
    L("tmp2") = N(2);
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
    }
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM)]
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _fnword _conj _prep 
		_pro _xEND)]
	@@

# det alpha
# dqan alpha noun
@POST
  "flag.txt" << phrasetext() << "\n";
  fixnpnonhead(3);	# 01/17/07 AM.
@RULES
_xNIL <- # 39. 76%
	_xWILD [plus match=(_det _quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_noun [plus]
	@@


# det alpha
@CHECK
  if (N("noun",2) && (N("adj",2) || N("adv",2)))
    fail(); # Ambiguous.
  if (N("adj",2) && N("adv",2))
    fail();
  if (!N("needs-np",1))
#  if (!N("needs-np"))  # BAD...
    fail();	# This det can stand alone. eg, 'all are punished'.
@POST
  L("tmp2") = N(2);
  if (N("noun",2) || N("unknown",2))
  	{
    group(2,2,"_noun");
	L("num") = number(L("tmp2"));
	if (L("num"))
	  N("number",2) = L("num");
    pncopyvars(L("tmp2"),N(2));
	}
  else if (N("adj",2))
    {
    group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("adv",2))
    {
    group(2,2,"_adv");
    pncopyvars(L("tmp2"),N(2));
	}
  else
    {
	if (G("error"))
	"err.txt" << "[det alpha: " << N("$text",2) << "\n";
	group(2,2,"_adj");	# Just a guess.
    pncopyvars(L("tmp2"),N(2));
	N("guess",2) = 1;
	fixadj(N(2));
	}
@RULES
_xNIL <- # 40
	_det
	_xALPHA
	@@

# det alpha
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("verb",2))
    fail();
@POST
  fixnpnonhead(2);	# adj-noun thingy...
@RULES
_xNIL <-
	_det
	_xALPHA
	_xWILD [one match=(_adj _noun)]
	@@

# dqan alpha
# det alpha , adj
# prep alpha, adj
@POST
  L("tmp2") = N(2);
  if (N("adj",2)  || N("verb",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	group(2,4,"_adj"); # adj list.
	clearpos(N(2),1,0);
	}
  else if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_det _prep)]
	_xALPHA
	\,
	_adj
	@@

# det alpha
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("verb",2))
    fail();
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_det
	_xALPHA
	_xWILD [one fail=(_adj _noun _xALPHA)]
	@@
_xNIL <-
	_det
	_xALPHA
	_xEND
	@@

# det alpha alpha
# Note:	Roughing out...
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("verb",2) || N("verb",3))
    fail();
@POST
  if (N("noun",3) || N("adj",3))
    fixnpnonhead(2);
  else
    fixnphead(2);
  # adj-noun thingy.
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	@@

# det adj , alpha noun
@CHECK
  if (!N("adj",4))
    fail();
@POST
  alphatoadj(4);
  group(2,4,"_adj");	# adj list.
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro _num _quan _xNUM)]
	_adj
	\,
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# adj alpha , adj noun $
# dqan alpha dqan
@CHECK
  if (!N("adj",2))
    fail();
  if (N("verb",2))
    if (!vconjq(N(2),"-ing")
	 && !vconjq(N(2),"-en"))
	  fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));

  group(2,4,"_adj");
  N("list",2) = 1;
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	\, [lookahead]
	_adj [plus]
	_noun
	@@

# noun 's alpha
# Apple's (Nasdaq:AAPL) shares jumped
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  chpos(N(2),"POS");
  L("tmp4") = N(4);
  L("tmp1") = lasteltnode(1);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  group(1,3,"_adj");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_noun [plus]
	_xWILD [one match=(_aposS _aposX)]
	_acro [opt]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# conj alpha
# and unknown adj
@CHECK
  if (!N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_conj)]
	_xALPHA
	_xWILD [one lookahead match=(_adj)]
	@@


# conj alpha
@CHECK
#  if (N("noun",2))
#    fail();
  if (N("adv",2))
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
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);
@RULES
_xNIL <- # 50
	_conj
	_xALPHA
	_xWILD [one lookahead match=(_det _np)]
	@@


# stronger than expected
# adj than alpha
#@CHECK
#  if (!N("verb",3))
#    fail();
#  L("v") = vconj(N(3));
#  if (L("v") != "-edn"
#   && L("v") != "-en")
#    fail();
#@POST
#  single();
#@RULES
#_adj <-
#	_adj
#	than [s]
#	_xALPHA
#	@@

# on continued strong
@CHECK
  if (!N("verb",2))
    fail();
  L("v") = vconj(N(2));
  if (L("v") != "-edn"
   && L("v") != "-en")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	on [s]
	_xALPHA
	_adj [lookahead]
	@@

# another adj
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	another
	_xWILD [one lookahead match=(_quan _num _xNUM _adj _noun)]
	@@


# ^ noun prep
# Also news from
@POST
  L("tmp3") = lasteltnode(3);
  L("fst") = N(3);
  L("ne arr") = nenoderange(L("fst"),L("tmp3"));
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);	# Zero out token info.
  if (L("ne arr"))
    N("ne arr",3) = L("ne arr");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xWILD [plus match=(_adv _advl)]
	_noun [plus]
	_xWILD [one lookahead match=(_prep)]
	@@


# dqan <- noun
# prep noun
# conj noun prep
@POST
  L("tmp3") = N(3);
  if (N("ne",3))
    L("ne arr") = N(3);
  fixnounhead(N(3));
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);	# Zero out token info.
  if (L("ne arr"))
    N("ne arr",3) = L("ne arr");
@RULES
_xNIL <-
	_xWILD [one match=(_prep _conj)]
	_adv [star]
	_noun
	_xWILD [one lookahead match=(_qEOS _pro _conj _dbldash
		_prep _xEND)]
	@@

# music player posted
# noun alpha verb
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  # If highly ambiguous, may need to put preferences on it
  # in various contexts.
  L("tmp2") = N(2);
  if (N("adv",2))	# Try this.
    {
	group(2,2,"_adv");
    pncopyvars(L("tmp2"),N(2));
	}
  else
	fixnphead(2);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_verb)]
	@@

# noun alpha verb
# Assume np agreement failed and so on.
# ex: This makes going to the store impossible.
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  N("glom",2) = "left";
  N("glom",3) = "right";
  # todo: _vg.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _pro)]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  L("tmp2") = N(2);
  L("noun") = lasteltnode(3);
  L("ne arr") = nenoderange(L("tmp2"),L("noun"));
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  group(2,3,"_np");
  pncopyvars(L("noun"),N(2));
  clearpos(N(2),1,1);
  if (L("ne arr"))
    N("ne arr",2) = L("ne arr");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xALPHA
	_noun [plus]
	_xWILD [one lookahead match=(_verb _vg _advl _adv)]
	@@

# noun alpha noun
# but: eastern industrialized countries
@PRE
<1,1> varz("cap");	# 04/20/07 AM.
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("verb",2))
    fail();
  if (N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  fixverb(N(2),"active","VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_noun _np _pro)]
	@@

# noun alpha dqan
# np alpha dqan
@CHECK
  if (!N("verb",2))
    fail();
  if (N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  fixverb(N(2),"active","VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _adj _pro)]
	@@

# verb dqan alpha prep
# TOO BROAD. TOO OLD.	# 06/19/06 AM.
#@CHECK
#  if (!N("noun",3))
#    fail();
#  if (N("verb",3))
#    fail();
#@POST
#  L("tmp3") = N(3);
#  group(3,3,"_noun");
#  pncopyvars(L("tmp3"),N(3));
#  group(2,3,"_np");
#  pncopyvars(L("tmp3"),N(2));
#  clearpos(N(2),1,1);
#  if (pnname(N(1)) == "_vg")
#    if (!N("voice",1))
#	  N("voice",1) = "active";
#@RULES
#_xNIL <-
#	_xWILD [one match=(_verb _vg)]
#	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun)]
#	_xALPHA
#	\, [opt lookahead]
#	_xWILD [one match=(_prep _vg _verb _pro _fnword)]
#	@@

# dqan alpha
@CHECK
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
	S("vc") = vconj(N(3));
	if (S("vc") != "inf"
	 && S("vc") != "-s")
	  fail();
	}
  # Check agreement.
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <- # 60
	_xSTART
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	\, [opt lookahead]
	_xWILD [one match=(_prep _vg _verb _pro _fnword)]
	@@

# dqan alpha dqan
@PRE
<1,1> varz("cap");	# 04/20/07 AM.
@CHECK
  if (!N("verb",2))
    fail();
  if (vconjq(N(2),"inf"))
    fail();
@POST
	L("tmp2") = N(2);
	group(2,2,"_verb");
	pncopyvars(L("tmp2"),N(2));
	L("v") = N(2);
	group(2,2,"_vg");
	mhbv(N(2),L("neg"),0,0,0,0,L("v"));
	pncopyvars(L("tmp2"),N(2));
	clearpos(N(2),1,0);
	N("verb node",2) = L("v");
	fixvg(N(2),"active","VBP");
@RULES
_xNIL <-
	_noun
	_xALPHA
	_noun [lookahead]
	@@
_xNIL <-
	_noun
	_xALPHA
	_proObj [s lookahead]
	@@

# ^ noun alpha adv alpha
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("verb",5))
    fail();
  if (!singular(N(5)))
    fail();
  if (!plural(N(3)))
    fail();
@POST
  fixnphead(3);
  alphatoverb(5,"active","VBD");
@RULES
_xNIL <-
	_xSTART
	_noun
	_xALPHA
	_adv
	_xALPHA
	_xWILD [one lookahead match=( \, _clausesep _xEND)]
	@@

# ^ noun alpha
# ^ noun noun alpha
@CHECK
  if (N("adj",3))
    fail();
  if (!N("noun",3) && !N("verb",3))
    fail();
  if (vconjq(N(3),"-edn"))
    fail();
@POST
  L("last") = lasteltnode(2);
  if (plural(L("last")) || pnvar(L("last"),"mass"))
    {
	if (!vconjq(N(3),"-s"))
	  alphatonoun(3);
	else
	  alphatoverb(3,0,"VBP");
	}
  else if (singular(L("last")))
    alphatonoun(3);
@RULES
_xNIL <-
	_xSTART
	_noun [plus]
	_xALPHA
	_xWILD [lookahead one fail=(_xALPHA _num _xNUM _quan _np _det _adj)]
	@@

# Note:	Enabling this rule reduced accuracy of tagger.
# # ^ dqan alpha [NIBBLE]
#@CHECK
#  if (N("pos num",6) != 2)
#    fail();
#  if (!N("noun",6) || !N("verb",6))
#    fail();
#@POST
#  # Todo: A "need plural" function.
#  # Todo: An "np complete" function.
#  L("last") = lasteltnode(5);
#  if (!N(2) && !N(3) && plural(N(6))
#   && singular(L("last")) )
#  if (numbersagree(N(1),N(6)))
#    fixnphead(6);
#  else
#    alphatoverb(6,0,0);
#@RULES
#_xNIL <-
#	_xWILD [one match=(_xSTART _qEOS)]
#	_xWILD [star match=(_det _pro)]
#	_xWILD [star match=(_quan _num _xNUM)]
#	_adj [star]
#	_noun [plus]
#	_xALPHA
#	@@


# Too old. Too broad.	# 07/08/06 AM.
# verb dqan alpha [commented out]
# If a verb, vg must be a helping verb etc.
#@CHECK
#  if (!N("noun",4))
#    fail();
#@POST
#  L("tmp4") = N(4);
#  group(4,4,"_noun");
#  pncopyvars(L("tmp4"),N(4));
#  L("ne arr") = nenoderange(N(3),N(4));
#  group(2,4,"_np");
#  pncopyvars(L("tmp4"),N(2));
#  clearpos(N(2),1,1);
#  if (L("ne arr"))
#    N("ne arr",2) = L("ne arr");
#  if (pnname(N(1)) == "_vg")
#    if (!N("voice",1))
#	  N("voice",1) = "active";
#@RULES
#_xNIL <-
#	_xWILD [one match=(_verb _vg)]
#	_det [star]
#	_xWILD [plus match=(_quan _num _xNUM _adj _noun)]
#	_xALPHA
#	_xWILD [one match=(_qEOS _xEND)]
#	@@
#_xNIL <-
#	_xWILD [one match=(_verb _vg)]
#	_proPoss [s star]
#	_xWILD [plus match=(_quan _num _xNUM _adj _noun)]
#	_xALPHA
#	_xWILD [one match=(_qEOS _xEND)]
#	@@



# conj noun .
@POST
  L("tmp2") = lasteltnode(2);
  L("ne arr") = nenoderange(N(2),L("tmp2"));
  fixnouns(N(2),L("tmp2"));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
  N("ne",2) = 0;
  if (L("ne arr"))
    N("ne arr",2) = L("ne arr");
@RULES
_xNIL <-
	_xWILD [one match=(_conj)]
	_noun [plus]
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# prep noun ,
@POST
  L("tmp2") = lasteltnode(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
  N("ne",2) = 0;
@RULES
_xNIL <-
	_prep
	_noun [plus]
	\, [opt lookahead]
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# he 's held it
# pro 's alpha
# 's => HAD, IS
@CHECK
  if (!N("verb",3))
    fail();
  # Check verb agreement "has held"...
@POST
  # Don't know if 's is be or have.
  if (!N("mypos",2))
    {
    chpos(N(2),"VBZ");
	N("apos-s to is/has",2) = 1;
	}
  L("b") = N(2);
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  fixverb(N(3),"active","VBP");
  group(2,3,"_vg");
  mhbv(N(2),L("neg"),0,0,L("b"),0,L("v"));
  pncopyvars(L("tmp3"),N(2));
  N("voice",2) = "active"; # Not absolute...
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_proSubj [s]
	_aposS
	_xALPHA
	_xWILD [one lookahead match=(_np _noun _det _quan _num _xNUM
		_adj)]
	@@


# pro 's alpha
# he's playing
@CHECK
  if (!N("verb",3))
    fail();
  L("v") = vconj(N(3));
  if (L("v") != "-ing"
   && L("v") != "-edn"
   && L("v") != "-ed")
   fail();
@POST
  L("b") = N(2);
  L("tmp3") = N(3);
  group(3,3,"_verb");
  L("v") = N(3);
  pncopyvars(L("tmp3"),N(3));
  group(2,3,"_vg");
  mhbv(N(2),L("neg"),0,0,L("b"),0,L("v"));
  pncopyvars(L("tmp3"),N(3));
#  N("voice") = "active";
  clearpos(N(3),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [s one match=(he she)]
	_aposS
	_xALPHA
	@@

# prep alpha 's
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_prep _conj _det)]
	_xALPHA
	_xWILD [one match=(_aposS _aposX)]
	@@

# noun prep
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <- # 70
	_xWILD [one match=(_dbldash)]
	_noun
	_xWILD [one lookahead match=(_prep)]
	@@


# pro alpha
# our book
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("noun",2) && N("adj",2))
    fail();
@POST
  L("tmp") = N(2);
  if (N("adj",2))
    group(2,2,"_adj");
  else
    group(2,2,"_noun");
  pncopyvars(L("tmp"),N(2));
@RULES
_xNIL <-
	_proPoss [s]
	_xALPHA
	@@

# pro alpha
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_proPoss [s]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg _qEOS _xEND)]
	@@

# pro alpha verb
@CHECK
  if (!N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_proSubj [s]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# pro alpha alpha
@CHECK
  if (!N("verb",2))
    fail();
  S("nd") = N("need-vconj",1);
  if (!S("nd")) # "it".
    S("nd") = "-s";
  if (!vconjq(N(2),S("nd")))
    fail();
@POST
  if (L("nd") == "inf")
    L("pos") = "VBP";
  else
    L("pos") = "VBZ";
  alphatoverb(2,"active",L("pos"));
@RULES
_xNIL <-
	_xWILD [s one match=(_proSubj it)]
	_xALPHA
	_xALPHA
	@@

# pro alpha
@CHECK
  if (!N("verb",2) && !N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("verb",2))
    {
	alphatoverb(2,"active","VBP");
	}
  else
    {
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	}
@RULES
_xNIL <-
	_xWILD [s one match=(_proSubj it)]
	_xALPHA
	_xWILD [one lookahead fail=(_verb _vg _xALPHA)]
	@@

# who verb
@PRE
<1,1> varz("pos50 who-v");
@POST
  N("pos50 who-v",1) = 1;
  if (vconjq(N(2),"-ed"))
 	fixverb(N(2),"active","VBD");
  else if (vconjq(N(2),"inf"))
	fixverb(N(2),"active","VBP");
@RULES
_xNIL <-
	who [s]
	_verb
	@@

# who alpha
@CHECK
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_proSubj [s]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _xNUM _num _adj _qEOS
		_xEND)]
	@@

# adj and alpha
@CHECK
  if (!N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  L("txt") = nodetext(N(3));
  L("myp") = adjconj(L("txt"));	# Don't want VBG here.
  if (L("myp"))
    chpos(N(3),L("myp"));
  else
    N("mypos",3) = "JJ";
  L("nm") = pnname(N(1));
  group(1,3,L("nm"));
@RULES
_xNIL <-
	_xWILD [one match=(_adj _adjc)]
	_conj
	_xALPHA
	@@

# verb alpha adj
# note: Watch for vnj pattern "help verb" patterns etc.
# Need vnn watch also.
@POST
 L("stem") = nodestem(N(1));
 L("tmp2") = N(2);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	L("done") = 1;
	}

  if (!L("done") && N("noun",2))
    {
    if (finddictattr(L("stem"),"vnj") # keep it green ...
     && (N("mass",2) || plural(N(2)) ) )
	   {
	   group(2,2,"_noun");
	   pncopyvars(L("tmp2"),N(2));
	   L("done") = 1;
	   }
	}

  if (!L("done") && N("verb",2))
    {
	if (finddictattr(L("stem"),"T2") && vconjq(N(2),"inf"))
	  {
	  group(2,2,"_verb");
	  pncopyvars(L("tmp2"),N(2));
	  L("done") = 1;
	  }
	else if (finddictattr(L("stem"),"T4") && vconjq(N(2),"-ing"))
	  {
	  group(2,2,"_verb");
	  pncopyvars(L("tmp2"),N(2));
	  L("done") = 1;
	  }
	}

  if (!L("done") && N("adv",2))
    {
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	L("done") = 1;
	}

  # Default.
  if (!L("done"))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	L("done") = 1;
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_adj [lookahead]
	@@

# verb alpha alpha noun
# Todo: Look for verb-verb patterns etc...
@CHECK
  if (N("pos50 v-a-a-n1",4))
    fail();
@POST
  N("pos50 v-a-a-n1",4) = 1;
  if (N("adj",3) && N("noun",3))
    fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	@@

# verb alpha alpha verb
# Verb patterns....
# Ex: send people tumbling.
# Ex: make people sing.
@CHECK
  # Todo: could handle adverbs too.
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <- # 80
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# verb alpha alpha
@CHECK
  if (N("stem",1) != "be"
   && N("$text",1) != "being")
    fail();
  if (!N("verb",3))
    fail();
  if (!N("adv",2) && !N("noun",2))
    fail();
  S("vc3") = vconj(N(3));
  if (S("vc3") != "-ing"
   && S("vc3") != "-edn"
   && S("vc3") != "-en")
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_verb
	_xALPHA
	_xALPHA
	@@

# noun like alpha alpha verb
@CHECK
  if (pnname(N(2)) != "_fnword")
    fail();
  if (!N("noun",4))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
  if (vconjq(N(5),"-ing"))
    fail();
@POST
  fixnphead(4);
  fixnpnonhead(3);
  chpos(N(2),"IN");	# like/IN.
  pnrename(N(2),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	like [s]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg _modal)]
	@@

# expect further declines in
# verb alpha alpha prep
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("noun",3))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnounhead(N(3));
  if (N("adj",2))
    group(2,2,"_adj");
  else
    group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [s one match=(_vg _verb) except=(_have _be _modal)]
	_xALPHA
	_xALPHA
	_xWILD [lookahead one match=(_prep _fnword _xEND _dbldash)]
	@@

# pro alpha verb
@CHECK
  if (N("adv",2))
    fail();
  if (!N("verb",2))
    fail();
  S("vc") = vconj(N(2));
  if (S("vc") != "-s"
   && S("vc") != "inf")
     fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_proSubj [s]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# Looks like a weak rule.  Need bounding...
# alpha verb
#@CHECK
#  if (!N("verb",1))
#    fail();
#  if (N("pos num",1) != 2)
#    fail();
#@POST
#  L("tmp1") = N(1);
#  if (N("noun",1))
#    group(1,1,"_noun");
#  else if (N("adj",1))
#    group(1,1,"_adj");
#  else if (N("adv",1))
#    group(1,1,"_adv");
#  else # Error?
#    group(1,1,"_noun");
#  pncopyvars(L("tmp1"),N(1));
#  if (pnname(N(1)) == "_adj")
#    fixadj(N(1));
#@RULES
#_xNIL <-
#	_xALPHA
#	_xWILD [one lookahead match=(_verb _vg)]
#	@@

# Looks old and rare.
# which noun alpha verb
# which dqan alpha verb
# that noun alpha verb
@CHECK
  if (!N("verb",5))
    fail();
  if (!vconjq(N(5),"-ed")
   && !vconjq(N(5),"-s"))
     fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_verb");
  pncopyvars(L("tmp5"),N(5));
  L("v") = N(5);
  group(5,5,"_vg");
  mhbv(N(5),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp5"),N(5));
  N("verb node",5) = L("v");
  fixvg(N(5),"active","VBD");
  clearpos(N(5),5,0);
  N("glom",5) = "left";
  if (N(6))
    N("glom",6) = "right";
  else
    N("glom",7) = "right";
@RULES
_xNIL <-
	_xWILD [s one match=(_whword that)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_np _pro)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	@@

# since early last june
# since alpha alpha noun
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (N("verb",2))
    fail();
  if (!N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));

  chpos(N(1),"IN");

  L("tmp2") = N(2);
  if (N("adj",2))
    group(2,2,"_adj");
  else # if (N("noun",2))
    group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
@RULES
_xNIL <-
	since [s]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	@@

# what alpha alpha verb
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!plural(N(4)))
    fail();
  if (!singular(N(3)) && !vconjq(N(3),"-ing"))
    fail();
  if (!singular(N(2)) && !vconjq(N(2),"-ing"))
    fail();
@POST
  fixnpnonhead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_whword [s]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# fnword alpha alpha $
# than alpha alpha $
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!plural(N(3)) && !vconjq(N(3),"-ing"))
    fail();
  if (!singular(N(2)))
    fail();
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_fnword
	_xALPHA
	_xALPHA
	_xWILD [one match=(_xEND)]
	@@

# since early june
# since alpha noun
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (N("verb",2))
    fail();
@POST
  chpos(N(1),"IN");
  L("tmp2") = N(2);
  if (N("adj",2))
    group(2,2,"_adj");
  else # if (N("noun",2))
    group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(3));
@RULES
_xNIL <-
	since [s]
	_xALPHA
	_noun [lookahead]
	@@

# back noun
# Should be locative.
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	back
	_xWILD [one lookahead match=(_noun)]
	@@


# alpha noun
# longer boats
@CHECK
  if (N("pos num",1) != 2)
    fail();
  if (!N("adj",1) || !N("adv",1))
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  fixadj(N(1));
@RULES
_xNIL <- # 90
	_xALPHA
	_noun
	@@

# alpha noun
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (N("verb",2))
    {
#	S("num") = number(N(3));
#	if (S("num") == "plural")
#	  fail();
	if (!vconjq(N(2),"inf"))	# 04/23/07 AM.
		fail();
	}
@POST
  fixnpnonhead(2);	# 01/17/07 AM.
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _conj \, )]
	_xALPHA
	_noun
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

# alpha noun [NIBBLE]
@CHECK
  if (!N("adj",1) && !N("noun",1))
    fail();
  if (N("verb",1))
    fail();
@POST
  fixnpnonhead(1);
@RULES
_xNIL <-
#	_xWILD [one match=(_xSTART _conj \, )]
	_xALPHA
	_noun [lookahead]
	@@

# verb adj
@CHECK
  if (!copulaq(N(1)))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adjc");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_adj
	_xWILD [one lookahead match=(_prep _conj _fnword _dbldash
		_qEOS _xEND)]
	@@

# verb alpha
# Trying contextless...
@CHECK
  if (N("stem",1) != "be"
   && N("$text",1) != "being")
    fail();
  if (!N("verb",3))
    fail();
  S("vc3") = vconj(N(3));
  if (S("vc3") != "-ing"
   && S("vc3") != "-edn"
   && S("vc3") != "-en")
   fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_verb
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# Note: What to do if it's a "to be" copula?...this doesn't
# seem to handle it.
# verb alpha prep
@CHECK
  if (N("pos50 copula-alpha",3))
    fail();	# Loop guard.
  if (N("adv",3))	# 01/18/07 AM.
	fail();	# Don't know which.
  if ((N("stem",1) == "be"
    || N("$text",1) == "being"
    || N("stem",1) == "have")
    && N("verb",3))
    fail();
  if (!N("adj",3))
    fail();
  if (N("verb",3) &&
      (N("sem",1) == "be" || N("sem",1) == "being"))
    {
	S("vc3") = vconj(N(3));
	if (S("vc3") == "-edn"
	 || S("vc3") == "-en"
	 || S("vc3") == "-ing")
	  fail();
	}
@POST
  "74-95-adj.txt" << phrasetext() << "\n";
  L("txt") = nodesem(N(1));
  L("tmp3") = N(3);
  if (copulaq(N(1)))
     {
	 group(3,3,"_adj");
	 pncopyvars(L("tmp3"),N(3));
	 fixadj(N(3));
	 group(3,3,"_adjc");
	 pncopyvars(L("tmp3"),N(3));
	 clearpos(N(3),1,0);
	 }
  N("pos50 copula-alpha",3) = 1;
@RULES
_xNIL <- # 95
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_conj)]
	_xWILD [one fail=(_adj _adjc _xALPHA)]
	@@

@CHECK
  if (!N("verb",3))
    fail();
  if (!verbfeat(N(1),"T5"))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  alphatoverb(3,"active","VBG");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb alpha prep
@CHECK
  if (!N("adv",3))
    fail();
  S("txt1") = nodesem(N(1));
  if (copulaq(N(1)))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adv");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv _premod)]
	_xALPHA
	_xWILD [one lookahead match=(_xEND _prep)]
	@@

# verb alpha that
@CHECK
  if (!N("advl",2))	# 04/15/05 AM.
    fail();
  if (!N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"), N(2));
  if (plural(N(2)))	# Todo: or mass noun. # 06/21/06 AM.
    chpos(N(2),"NN");	# Conformity.
  else if (copulaq(N(1)) && N("adj",2))
    chpos(N(2),"JJ");
  else
    chpos(N(2),"RB");
  if (!N("mypos",3))
    chpos(N(3),"IN");	# that/IN
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	that [s]
	@@

# verb alpha conj
# vg alpha conj
# Watch verb patterns...
@CHECK
  if (!N("noun",3))
    fail();
@POST
  alphatonoun(3);
@RULES
_xNIL <- # 100
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [one lookahead match=(_conj)]
	@@


# verb alpha
# foreshadow rising
@CHECK
  if (!N("adj",2))
    fail();
  if (!N("adv",2) && !N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [s one match=(_verb _vg) except=(_modal _have _be
			being)]
	_xALPHA
	@@

# verb alpha
# mhbv alpha
@CHECK
  if (N("verb",2))
    {
	if (!vconjq(N(2),"inf"))
      fail();
	}
@POST
  L("tmp2") = N(2);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  else if (N("adv",2))
    {
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	}
  else
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# Idiomatic.
# verb fine
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(_verb _vg) except=(_modal _have)]
	_xWILD [one match=(fine swell super great well good)]
	@@


# help alpha
# verb patterns....
@CHECK
  if (!N("verb",2))
    fail();
  if (verbfeat(N(1),"T2") && vconjq(N(2),"inf"))
    succeed();
  if (verbfeat(N(1),"T4") && vconjq(N(2),"-ing"))
    succeed();
  fail();
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VB");
  L("v") = N(2);
  group(1,2,"_vg");
  pncopyvars(L("tmp1"),N(1));	# Copy up "syntax" stuff.
  N("compound-vg",1) = 1;
  N("vg node",1) = L("tmp1");
  # todo: semantics from nonhelping verb...
  N("stem",1) = nodestem(L("tmp2"));
  N("sem",1) = nodesem(L("tmp2"));
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_vg
	_xALPHA
	_xWILD [one lookahead fail=(_aposS _aposX _verb _vg)]
	@@

# help alpha
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VB");
  L("v") = N(2);

  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
  fixverb(N(1),"active","VBP");

  group(1,2,"_vg");
  pncopyvars(L("tmp1"),N(1));	# Copy up "syntax" stuff.
  N("compoun-vg",1) = 1;
  N("vg node",1) = L("tmp1");
  # todo: semantics from nonhelping verb...
  N("stem",1) = nodestem(L("tmp2"));
  N("sem",1) = nodesem(L("tmp2"));
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	help
	_xALPHA
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

# noun alpha alpha
# verb patterns...
@CHECK
  if (!N("verb",2) || !N("verb",3))
    fail();
  if (vconjq(N(2),"inf"))
    S("pos2") = "VBP";
  else if (vconjq(N(2),"-s"))
    S("pos2") = "VBZ";
  else if (vconjq(N(2),"-ed"))
    S("pos2") = "VBD";
  else
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
  S("stem2") = nodestem(N(2));
  S("sem3") = nodesem(N(3));
  if (!finddictattr(S("stem2"),"T4"))
    fail();
@POST
  L("v3") = alphatovg(3,"active",S("pos2"));
  L("v2") = alphatovg(2,"active","VBP");
  group(2,3,"_vg");
  pncopyvars(L("v2"),N(2));	# Copy up "syntax" stuff.
  N("compound-vg",2) = 1;
  N("vg node",2) = L("v2");
  # todo: semantics from nonhelping verb...
  N("stem",2) = nodestem(L("stem2"));
  N("sem",2) = nodesem(L("sem3"));
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _pro)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun _pro _np _det
		_quan _num _xNUM
		_adj _fnword _prep)]
	@@

# verb alpha dqan
# begin ving ...
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBG");
  if (pnname(N(1)) == "_vg")
    {
	N("complex-verb",1) = 1;
    listadd(1,2,"false");
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM
		_adj _noun _np _adv _advl)]
	@@

# take place
@CHECK
  if (!N("noun",2) || !N("verb",2))
    fail();
  if (N("adj",2))
    fail();	# 10/20/04 AM.
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(_verb _vg) except=(_modal _have _be)]
	_xALPHA
	@@

# alpha np verb
@PRE
<1,1> var("verb");
@CHECK
  if (!verbfeat(N(1),"V8"))
	fail();
  if (!vconjq(N(5),"-en"))
	fail();
@POST
  alphatoverb(1,"active",0);
@RULES
_xNIL <-
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	@@

# Too weak.	# 02/02/05 AM.
# of working
# of alpha
#@CHECK
#  if (!N("verb",2))
#    fail();
#  if (!N("noun",2) && !N("adj",2))
#    fail();
#@POST
#  L("tmp2") = N(2);
#  L("vc") = vconj(N(2));
#  if (N("adj",2))
#    {
#    group(2,2,"_adj");
#    pncopyvars(L("tmp2"),N(2));
#	fixadj(N(2));
#	}
#  else if (L("vc") == "-ing")
#  	group(2,2,"_verb");
#  else if (N("noun",2))
#    group(2,2,"_noun");
#  pncopyvars(L("tmp2"),N(2));
#@RULES
#_xNIL <-
#	_xWILD [s one match=(_prep) except=(to)]
#	_xALPHA
#	@@

# said ,
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
  L("v") = N(1);
  group(1,1,"_vg");
  mhbv(N(1),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp1"),N(1));
  N("verb node",1) = L("v");
  fixvg(N(1),"active","VBD");
  clearpos(N(1),1,0);
  N("glom",1) = "left";
  if (N(2))
    N("glom",2) = "right";
@RULES
_xNIL <-
	said
	_xWILD [one lookahead match=(\, _dblquote _qEOS _xEND
		_pro _np)]
	@@

	
# noun alpha alpha prep
@CHECK
  if (N("verb",2))
    fail();
  if (N("adj",2) || !N("noun",2))
    fail();
  if (N("noun",3))
    fail();
  if (!N("verb",3) && !N("adj",3))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  if (N("verb",3))
    {
    group(3,3,"_verb");
    pncopyvars(L("tmp3"),N(3));
	}
  else if (N("adj",3))
    {
    group(3,3,"_adj");
    pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# noun alpha alpha prep np vg
@CHECK
  if (N("pos50 n-a-a-p-n-v",2))
    fail();
@POST
  N("pos50 n-a-a-p-n-v",2) = 1;
  if (vconjq(N(3),"-ing"))
    alphatoverb(3,"active","VBG");
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	_np
	_vg
	@@

# noun alpha alpha prep verb
# noun alpha alpha to verb
@CHECK
  if (!N("verb",3))
    fail();
  if (!N("noun",2))
    fail();
  if (!verbfeat(N(3),"T3"))
    fail();
@POST
  alphatoverb(3,0,0);
  fixnphead(2);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	to [s lookahead]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# noun adv alpha prep
@CHECK
  if (!N("verb",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [plus match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb adv alpha [NIBBLE]
@CHECK
  if (!N("adj",3))
    fail();
@POST
  alphatoadj(3);
@RULES
_xNIL <-
	_xWILD [s one match=(_verb _vg) except=(_be _have _do)]
	_adv
	_xALPHA
	@@

# dqan alpha prep
# noun alpha prep
@CHECK
  if (!plural(N(1)))
    fail();
  if (!N("verb",2))
    fail();
  if (vconjq(N(2),"-s"))
    fail();
@POST
  # Could be "people expected to", ie ambig active/passive.
  if (vconjq(N(2),"-edn"))
	alphatoverb(2,0,0);
  else
    alphatoverb(2,"active","VBP");
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@
	
# noun alpha prep
@CHECK
  if (N("noun",2) && N("verb",2))
    fail();
  if (N("eos",1))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("verb",2))
    {
	group(2,2,"_verb");
	L("v") = N(2);
	pncopyvars(L("tmp2"),N(2));
	group(2,2,"_vg");
    mhbv(N(2),L("neg"),0,0,0,0,L("v"));
	pncopyvars(L("tmp2"),N(2));
	N("verb node",2) = N("first verb",2) = L("v");
#	fixvg(N(2),0,0);
	clearpos(N(2),1,0);
	}
  else if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (N("adv",2))
    {
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	}
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [star match=(_advl _adv)]
	_xWILD [lookahead one match=(_prep)]
	@@

# noun alpha prep
# noun alpha adv prep
# Too broad.
#@CHECK
#  if (!N("verb",2))
#    fail();
#@POST
#  alphatoverb(2,0,0);
#@RULES
#_xNIL <-
#	_xWILD [one match=(_noun _np)]
#	_xALPHA
#	_xWILD [plus match=(_advl _adv)]
#	_xWILD [lookahead one match=(_prep _adj)]
#	@@

# noun alpha [IDIOM]
# locatives get adv.
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(
		overseas
		)]
	@@


# noun alpha [IDIOM]
# noun away
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(away)]
	@@

# noun alpha
# week earlier
# (becomes an adverbial with its prior np)
# Tough to decide adjective vs adverb....
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("adj",2) || !N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  # Treat as adverb. # 06/17/06 AM.
#  if (pnvar(L("tmp2"),"adj"))
#    fixadj(N(2));
@RULES
_xNIL <-
	_noun
	_xALPHA
	@@

# noun alpha
# noun ved
@CHECK
  if (!N("verb",3))
    fail();
  S("vc") = vconj(N(3));
  if (S("vc") != "-ed"
   && S("vc") != "-edn")
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  L("v") = N(3);
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VBP");
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("voice",3) = "active";
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_xSTART
	_noun
	_xALPHA
	@@

# np conj noun alpha
@CHECK
  if (!N("noun",4))
    fail();
  if (!singular(N(3)) || !singular(N(4)))
    fail();
@POST
  alphatonoun(4);
@RULES
_xNIL <-
	_np
	_conj
	_noun
	_xALPHA
	@@

# conj noun alpha
@PRE
<3,3> lowercase();
@CHECK
  if (!singular(N(3)))
    fail();
  if (!N("noun",4) || !N("verb",4))
    fail();
  if (N("pos num",4) > 2)
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_conj
	_noun
	_xALPHA
	@@

# noun which helps its
# noun which alpha pro
# Todo: agreement checks.
# that
@CHECK
  if (!N("verb",3))
    fail();
@POST
  chpos(N(2),"WDT");
  L("tmp3") = N(3);
  group(3,3,"_verb");
  L("v") = N(3);
  pncopyvars(L("tmp3"),N(3));
  fixverb(L("v"),"active","VBP");
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  N("voice",3) = "active";
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [s one match=(which that)]
	_xALPHA
	_xWILD [one lookahead match=(_pro _det _quan _num _xNUM
		_prep)]
	@@

# which verb
# that
@POST
  L("tmp2") = N(2);
  group(2,2,"_vg");
  L("voice") = mhbv(N(2),L("neg"),0,0,0,0,L("tmp2"));
  pncopyvars(L("tmp2"),N(2));
  N("first verb",2) = N("verb node",2) = L("tmp2");
  clearpos(N(2),1,0);
  fixvg(N(2),"active","VBZ");
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xWILD [s one match=(_verb) except=(_vg _have _be _do)]
	@@

# which alpha noun
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf")
   && !vconjq(N(2),"-ed")
   && !vconjq(N(2),"-s"))
    fail();
  if (!plural(N(3)))
    fail();
@POST
  alphatovg(2,"active","VBP");
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xALPHA
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [one match=(_prep)]
	@@

# which alpha pro
@PRE
<2,2> var("verb");
@POST
  alphatovg(2,"active","VBP");
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xALPHA
	_xWILD [one lookahead match=(_pro)]
	@@

# which helps its
# Todo: agreement checks.
# that
@CHECK
  if (!N("verb",2))
    fail();
  if (N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(L("v"),"active","VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("verb node",2) = L("v");
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM
		_prep)]
	@@

# that alpha noun verb
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  fixnpnonhead(2);
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_noun [lookahead]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# which alpha dqan
# that alpha dqan
@CHECK
  if (N("pos50 which-a-dqan",2))
    fail();
@POST
  N("pos50 which-a-dqan",2) = 1;
  if (N("verb",2))
    {
	alphatoverb(2,0,0);
	}
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xALPHA
	_xWILD [one lookahead match=(_quan _num _xNUM _adj)]
	@@

# that alpha noun
# 04/19/07 AM.
@PRE
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"-s"))
	fail();
@POST
  alphatoverb(2,"active","VBZ");
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]
	@@

# which alpha alpha
# that alpha alpha
@CHECK
  if (N("pos50 which-a-a",3))
    fail();
@POST
  N("pos50 which-a-a",3) = 1;
  if (N("verb",3)
   && (N("adj",4) || N("noun",4)) )
    {
	if (vconjq(N(3),"inf"))
	  alphatoverb(3,0,0);
	}
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_adv [star]
	_xALPHA
	_xALPHA
	@@

# Biased rule.
# dqan alpha alpha alpha dqan
# Some multi alpha handlers.
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("verb",4))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  group(4,4,"_verb");
  L("v") = N(4);
  pncopyvars(L("tmp4"),N(4));
  group(4,4,"_vg");
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp4"),N(4));
  N("verb node",4) = L("v");
#  N("voice",4) = "active";
  clearpos(N(4),1,0);
  if (N("noun",3))
    {
    group(3,3,"_noun");
    pncopyvars(L("tmp3"),N(3));
	}
  else
    {
    group(3,3,"_adj");
    pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
  if (N("adj",2))
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	}
  if (pnname(N(5)) != "_prep")
	fixvg(N(4),"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_det _np)]
	@@

# adj alpha alpha alpha noun
# dqan alpha alpha alpha noun
@CHECK
  if (!N("adj",4))
    fail();
  if (!N("adj",5) && !N("noun",5))
    fail();
  if (!N("noun",6) && !N("adj",6))
    fail();
  if (plural(N(4)) || plural(N(5)) || plural(N(6)))
    fail();
@POST
  fixnpnonhead(6);
  fixnpnonhead(5);
  fixnpnonhead(4);
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# noun alpha alpha alpha prep
# Favor first as verb.
@CHECK
  if (!N("verb",2) || !N("noun",4))
    fail();	# 06/08/06 AM.
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (singular(N(4)) && !N("mass",4))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  if (N("verb",2) && N("noun",4))
    {
	group(2,2,"_verb");
	pncopyvars(L("tmp2"),N(2));
	L("v") = N(2);
	group(2,2,"_vg");
	pncopyvars(L("tmp2"),N(2));
	clearpos(N(2),1,0);
	N("verb node",2) = L("v");
	fixvg(N(2),"active","VBP");
	}
  else	# Loop guard.	# 05/18/05 AM.
    noop();	# FAIL.  # 05/18/05 AM.
@RULES
_xNIL <-
	_xWILD [one match=(_fnword _noun)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _xEND)]
	@@

# 's alpha alpha alpha verb
@CHECK
  if (!N("noun",4))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
  if (!N("adj",2) && !N("noun",2))
    fail();
  # Todo: agreement. nn and nv.
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  if (N("adj",3))
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
  else
    {
	group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	fixnoun(N(3));
	}
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));
@RULES
_xNIL <-
	_xWILD [one match=( _aposS _det _pro _adj _aposX )]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# 's alpha alpha alpha dqan
@CHECK
  if (!N("verb",4))
	fail();
  if (!N("noun",3))
	fail();
  if (!N("noun",2) && !N("adj",2))
	fail();
  if (!vconjq(N(4),"-ed"))
	fail();
@POST
  alphatoverb(4,"active","VBD");
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xWILD [one match=( _aposS _det _pro _adj _aposX )]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM
		_adj _noun _np)]
	@@

# that alpha alpha alpha verb
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf")
   && !vconjq(N(2),"-s"))
    fail();
  if (!N("noun",4))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnpnonhead(3);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# , alpha alpha alpha verb
@CHECK
  if (!N("verb",2))
    fail();
  L("stem") = nodestem(N(2));
  if (!finddictattr(L("stem"),"V2"))
    fail();
  if (!vconjq(N(5),"inf"))
    fail();
@POST
  alphatoverb(2,"active","VBD");
  if (pnname(N(5)) == "_vg")
    L("v") = N("verb node",5);
  else
    L("v") = N(5);
  fixverb(L("v"),"active","VB");
@RULES
_xNIL <-
	_xWILD [one match=( \, _clausesep)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# verb alpha alpha alpha
# Check out single np hypothesis.
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
  if (!N("noun",4))
    fail();
  if (N("noun",2))
    {
    L("n2") = number(N(2));
    if (L("n2") != "singular")
      fail();
    }
  if (N("noun",3))
    {
    L("n3") = number(N(3));
    if (L("n3") != "singular")
      fail();
	}
  L("n4") = number(N(4));
  if (L("n4") != "plural")
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  if (N("adj",3))
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
  else
    {
	group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	fixnoun(N(3));
	}
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));
  group(2,4,"_np");
  pncopyvars(L("tmp4"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_qEOS \, _dbldash _xEND
		_prep)]
	@@

# prep alpha alpha alpha prep
# 64 combinations. 4x4x4.
# NEED A NEW APPROACH TO THIS STUFF.
# to alpha alpha alpha prep

# to verb:
# to eat people quickly
# to eat green people
# to eat only people
# to paint houses green
# to keep eating people
# to keep people eating
# to help people eat
# to help eat people

# to adv:
# to quickly eat people
# to quickly help eat
# to quickly help only

#@CHECK
#  if (N("pos50 to-a-a-a1",1))
#    fail();
#@POST
#  N("pos50 to-a-a-a1",1) = 1; # Loop guard.
#@RULES
#_xNIL <-
#	to [s]
#	_xALPHA
#	_xALPHA
#	_xALPHA
#	_xWILD [one lookahead match=(_prep)]
#	@@

# ^ noun prep alpha
# ^ np prep alpha
@PRE
<5,5> varne("stem","to");
@CHECK
  if (!N("noun",7) && !N("adj",7))
    fail();
@POST
  fixnpnonhead(7);	# adj-noun thingy...
@RULES
_xNIL <-
    _xSTART								### (1)
    _xWILD [star match=(_adv _advl)]	### (2)
    _xWILD [one match=(_noun _np)]		### (3)
    _xWILD [star match=(_adv _advl)]	### (4)
    _prep								### (5)
    _xWILD [star match=(_adv _advl)]	### (6)
    _xALPHA								### (7)
    @@

# ^ np conj alpha [NIBBLE]
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  if (N("adj",4))
    alphatoadj(4);
  else
    alphatonoun(4);
@RULES
_xNIL <-
	_xSTART
	_np
	_conj
	_xALPHA
	@@

# prep alpha alpha alpha
@CHECK
  if (!N("noun",4))
    fail();
  if (N("verb",4))
    if (!vconjq(N(4),"-ing"))
      fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (plural(N(3)))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep \, _fnword)]
	@@

# adj alpha
# hyphenated adj
@CHECK
  if (!N("hyphenated",1))
    fail();
  if (N("pos num",2) != 2)
    fail();
  if (!N("verb",2))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("noun",2))
    {
	group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	}
  else
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
@RULES
_xNIL <-
	_adj
	_xALPHA
	@@

# 's alpha alpha verb
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("adj",2) && !N("noun",2))
    fail();
  # Todo: agreement. nn and nv.
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_xWILD [one match=( _aposS _aposX _det _pro )]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# dqan prep dqan alpha alpha
# noun prep noun alpha alpha
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (!N("noun",5))
    fail();
  if (!singular(N(4)))
    fail();
  if (!N("eventive",1) && !N("verb",1))
    fail();	# requiring "eventive" noun.
@POST
  fixnpnonhead(4);
  fixnphead(5);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [s one match=(_prep) except=(to)]
	_xWILD [one match=(_noun)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _clausesep \, )]
	@@

# verb dqan conj dqan alpha
@CHECK
  if (!N("noun",8) && !N("adj",8) && !N("adv",8))
    fail();
@POST
  if (N("noun",8))
    alphatonoun(8);
  else if (N("adv",8))
    alphatoadv(8);
  else
    alphatoadj(8);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_conj
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_fnword _prep _conj
		_modal _verb _vg _xEND _clausesep)]
	@@

# verb noun conj dqan alpha alpha
# vg np conj dqan alpha alpha
@CHECK
  if (!N("noun",5) && !N("adj",5))
    fail();
  if (!N("noun",6))
    fail();
@POST
  fixnphead(6);
  fixnpnonhead(5);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_np
	_conj
	_adj [star]
	_xALPHA
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_xEND _clausesep \, _prep)]
	@@

# verb noun conj dqan alpha
# vg np conj dqan alpha
@PRE
 <8,8> var("noun");
@POST
  fixnphead(8);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_np
	_conj
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_num _quan _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_xEND _clausesep \, _prep)]
	@@

# dqan conj dqan alpha
# dqa conj dqan alpha
@CHECK
  if (!N("noun",6))
    fail();
  if (!singular(N(5)))
    fail();
@POST
  alphatonoun(6);
@RULES
_xNIL <-
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [plus]
	_conj
	_noun
	_xALPHA
	@@

# conj dqan alpha
# conj dqa alpha
@CHECK
  if (N("pos50 conj-n-a",5))
    fail();
@POST
  N("pos50 conj-n-a",5) = 1;
  if (N("noun",5) || N("adj",5))
    {
	if (N("verb",5))
	  {
	  --N("pos num",5);
	  N("verb",5) = 0;
	  }
	if (N("adv",5))
	  {
	  --N("pos num",5);
	  N("adv",5) = 0;
	  }
	}
  if (N("pos num",5) == 1)
    {
	if (N("noun",5))
	  alphatonoun(5);
	else if (N("adj",5))
	  alphatoadj(5);
	else if (N("adv",5))
	  alphatoadv(5);
	}
  # adj-noun thingy.
@RULES
_xNIL <-
	_conj
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [plus]
	_xALPHA
	@@

# prep dqan conj alpha
@CHECK
  if (!N("noun",5))
    fail();
  S("lastn") = lasteltnode(3);
  if (!singular(S("lastn")))
    fail();
  if (!singular(N(5)))
    fail();
  if (!plural(N(6)))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  group(3,5,"_noun");	# Noun list.
  N("list",3) = 1;
  N("ignorepos",3) = 1;
@RULES
_xNIL <-
	_prep
	_adj [star]
	_noun [plus]
	_conj
	_xALPHA
	_noun [lookahead]
	@@

# prep dqan alpha alpha
@CHECK
  if (!N("noun",5))
    fail();
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  fixnoun(N(5));
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_prep)]
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _adv _clausesep _fnword)]
	@@

# prep dqan alpha alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnpnonhead(3);
  group(2,4,"_np");
  pncopyvars(L("tmp4"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND _clausesep _fnword)]
	@@

# prep dqan alpha alpha adv
@PRE
<6,7> var("noun");	# Have to be possible nouns.
@CHECK
  L("last5") = lasteltnode(5);
  if (!singular(L("last5"))
   || !singular(N(6))
   || !singular(N(7))
   )
   fail();
@POST
  L("tmp6") = N(6);
  L("tmp7") = N(7);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
  group(7,7,"_noun");
  pncopyvars(L("tmp7"),N(7));
@RULES
_xNIL <-
    _xWILD [one match=(_prep)]				### (1)
    _xWILD [star match=(_det _pro)]			### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]								### (4)
    _noun [plus]							### (5)
    _xALPHA									# (6)
    _xALPHA									# (7)
    _xWILD	[one lookahead match=(_adv _prep \, _qEOS _xEND
		_fnword _clausesep)]	### (8)
    @@

# prep dqan alpha alpha
# Note: Nibble off the edges.
@CHECK
  if (N("pos50 to-j-a-a1",1))
    fail();
@POST
  N("pos50 to-j-a-a1",1) = 1;
  if (N("noun",3) && N("adj",3))
    {
	if (plural(N(3)))
	  fixnphead(3);
	else if (N("noun",4) && plural(N(4)))
	  {
	  fixnphead(4);
	  fixnpnonhead(3);
	  }
	else
	  alphatoadj(3);
	}
  else if (N("adj",3))
    alphatoadj(3);
  else if (N("noun",3))
    alphatonoun(3);
  else if (N("verb",3))
    ;	# Ignore for now.
  else if (N("adv",3))
    alphatoadv(3);
@RULES
_xNIL <-
	to [s]
	_adj
	_xALPHA
	_xALPHA
	@@

# dqan alpha
# NIBBLE one alpha.
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("verb",2) || !N("adj",2))
    fail();
  if (!vconjq(N(2),"-ed")
   && !vconjq(N(2),"-en"))
    fail();
@POST
  alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun [plus]
	_xALPHA
	@@

	
# adj alpha adj
# dqan alpha dqan
@CHECK
  if (!N("apos-s",1))
    fail();
  if (!N("adj",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_adj [lookahead]
	@@

# adj alpha adj noun
# dqan
@CHECK
  if (!N("adj",2))
    fail();
  if (!singular(N(4)))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_adj
	_noun
	@@

# prep dqan alpha
@CHECK
  if (N("disagree",1))
    fail();	# Loop guard.
  if (!N("noun",4))
    fail();
  L("last") = lasteltnode(4);
  if (N(3))
    L("first") = N(3);
  else if (N(4))
    L("first") = N(4);
  else
    L("first") = N(5);
  if (!numbersagree(L("first"),L("last")))
    S("disagree") = 1;
@POST
  if (S("disagree"))
  	N("disagree",1) = 1;
#    group(2,3,"_np");
  else
    {
  L("tmp4") = N(4);
  L("next") = pnnext(N(4));
  if (L("next"))
    {
    L("nm") = pnname(L("next"));
    if (L("nm") == "_noun" || L("nm") == "adj")
	  L("dqan") = 1; # Inside a noun phrase.
	}

  if (N("verb",4))
    {
	if (vconjq(N(4),"-ing"))
	  L("vadj") = 1;
	}

  if ((L("vadj") && L("dqan")) || (L("dqan") && N("adj",4)))
    {
	group(4,4,"_adj");
	pncopyvars(L("tmp4"),N(4));
	fixadj(N(4));
	}
  else
    {
    group(4,4,"_noun");
    pncopyvars(L("tmp4"),N(4));
    fixnoun(N(4));
	}
	}
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_proPoss [s opt]
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	\, [opt lookahead]
	_xWILD [one fail=(_adj _noun _xALPHA _quan _num _xNUM)]
	@@
	
# people reported .
@CHECK
  if (!N("verb",2))
    fail();
  if (N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# more than alpha prep
#@CHECK
#  if (!N("verb",3))
#    fail();
#  S("vc") = vconj(N(3));
#  if (S("vc") != "-edn"
#   && S("vc") != "-en")
#    fail();
#@POST
#  L("tmp3") = N(3);
#  group(3,3,"_verb");
#  pncopyvars(L("tmp3"),N(3));
#  L("v") = N(3);
#  group(3,3,"_vg");
#  pncopyvars(L("tmp3"),N(3));
#  N("verb node",3) = L("v");
#  fixvg(N(3),"passive","VBN");
#  clearpos(N(3),1,0);
#@RULES
#_xNIL <-
#	_xWILD [one match=(_fnword)]
#	_xWILD [star match=(_advl _adv)]
#	_xALPHA
#	_xWILD [one lookahead match=(_prep)]
#	@@

# ^ that verb
@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"DT");	# that/DT.
  pnrename(N(2),"_np");
@RULES
_xNIL <-
	_xSTART
	that [s except=(_det _np)]
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# that alpha conj verb
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-s")
   && !vconjq(N(2),"-ed"))
     fail();
@POST
  alphatoverb(2,"active","VBD");
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_conj
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# that alpha
@CHECK
  if (N("mypos",1))
    fail();
  if (N("verb",2))
    fail();
  if (!N("noun",2))
    fail();
  if (!singular(N(2)))
    fail();
@POST
  pnrename(N(1),"_det");
  chpos(N(1),"DT");	# that/DT
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xWILD [one lookahead match=( \, _xEND)]
	@@

# that alpha noun , dqan
@CHECK
  if (!N("verb",2))
    fail();
  S("ed") = vconjq(N(2),"-ed");
  if (!S("ed")
   && !vconjq(N(2),"-s"))
    fail();
@POST
  if (!N("mypos",1))
    chpos(N(1),"IN");	# that/IN
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  if (S("ed"))
    chpos(N(2),"VBD");
  else
    chpos(N(2),"VBZ");
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_noun [lookahead]
	\,
	_xWILD [one match=(_det _pro _quan _num _xNUM _adj _noun)]
	@@

# that alpha dqan
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-s")
   && !vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");

  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xWILD [star lookahead match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one match=(_prep)]
	@@
_xNIL <-
	that [s]
	_xALPHA
	_pro [lookahead]
	@@


	
# that books reservations
# that alpha dqan
@CHECK
  if (!N("verb",2))
    fail();
  S("v") = vconj(N(2));
  if (S("v") != "-s"  && S("v") != "inf")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
#  if (S("v") == "-s")
#    chpos(N(2),"VBZ");
#  else if (S("v") == "inf")
#    chpos(N(2),"VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
#  chpos(N(1),"IN");	# Not enough context.	# 06/24/06 AM.
#  N("bracket",1) = 1;
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _xNUM _num _np
#		_adj _noun
		)]
	@@

# that alpha prep
@CHECK
  if (!N("verb",3))
    fail();
  if (vconjq(N(3),"inf"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VBP");
#  if (!N("mypos",1))
#    chpos(N(1),"IN");	# that/IN
@RULES
_xNIL <-
	that [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_prep)]
	@@

# that verb
@CHECK
  if (N("mypos",2))
    fail();
@POST
  if (copulaq(N(3))
   || vconjq(N(3),"-s")
   || vconjq(N(3),"-ed")
   || vconjq(N(3),"-en"))
    chpos(N(2),"DT");	# that/DT
  else
    chpos(N(2),"IN");	# that/IN
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _conj)]
	_xWILD [s one match=(that) except=(_np)]
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# , alpha alpha noun verb
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
@POST
  fixnpnonhead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xWILD [one match=( \, _clausesep _xSTART)]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# , alpha alpha noun
# began eating
@CHECK
  if (!N("verb",2) || !N("verb",3))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  # Want a composite vg...
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"VBG");
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
  group(2,3,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]
	@@

# , alpha alpha prep
@CHECK
  if (N("pos50 comma-aa-prep",2))
    fail();
@POST
  N("pos50 comma-aa-prep",2) = 1;
  if (!N("verb",2))
    {
	if (N("adj",2))
	  alphatoadj(2);
	}
@RULES
_xNIL <-
	_xWILD [one match=( \, _clausesep)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_xEND _prep)]
	@@

# noun that alpha alpha noun
@CHECK
  if (!N("verb",3))
    fail();
  if (!N("adj",4) && !N("noun",4))
    fail();
  if (!vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"VBG");
#  chpos(N(2),"WDT");	# that/WDT	# 04/23/07 AM.
  chpos(N(2),"IN");		# that/IN	# 04/23/07 AM.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	that [s]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]
	@@

# alpha alpha noun
@CHECK
  if (N("verb",2) || N("verb",3))
    fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  # Todo: fail on noun-adj also.
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  if (N("adj",3))
    {
    group(3,3,"_adj");
    pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
  else if (N("noun",3))
    {
    group(3,3,"_noun");
    pncopyvars(L("tmp3"),N(3));
	}
  if (N("adj",2))
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# alpha alpha ,
@CHECK
  if (N("verb",1) || N("verb",2))
	fail();
  if (!N("adj",1) && !N("noun",1) && !N("unknown",1))
	fail();
  if (!N("noun",2) && !N("unknown",2))
	fail();
@POST
  fixnpnonhead(1);
  L("tmp2") = N(2);
  # Todo: fixnphead()
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=( \, _modal _verb _vg
		_adv _advl)]
	@@

# fnword alpha noun
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("verb",2) || N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("adj",2))
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_fnword _conj _prep)]
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# two times the
# num times det
# should be premod
@POST
  if (strpiece(pnname(N(2)),0,0) != "_")
    {
	L("tmp2") = N(2);
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	if (N("$text",2) == "times")
	  chpos(N(2),"NNS");
	else
	  chpos(N(2),"NN");
	}
  sclearpos(1,0);
  single();
@RULES
_det <-
	_xWILD [one match=(_xNUM _num _quan)]
	_xWILD [s one match=(time times)]
	_det
	@@

# more quickly
# more adv
@POST
  if (G("error"))
  "err.txt" << "more quickly" << "\n";
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"RBR");
@RULES
_xNIL <-
	_xWILD [s one match=(more less)
		except=(_adj _adv _noun)]	# Infinite loop guard.
	_adv [lookahead]
	@@

# most quickly
# most adv
@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"RBS");
@RULES
_xNIL <-
	_xWILD [s one match=(most)
		except=(_adj _adv _noun)]	# Infinite loop guard.
	_adv [lookahead]
	@@


# , alpha by noun to
# , alpha by some to
@CHECK
  if (!N("verb",3))
    fail();
  S("vc") = vconj(N(3));
  if (S("vc") != "-en"
   && S("vc") != "-edn")
     fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"passive",0);
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	by [s lookahead]
	_xWILD [one match=(_det _noun _np)]
	_xWILD [s one match=(to _xEND)]
	@@

# more than alpha by
@CHECK
  if (!N("verb",3))
    fail();
  S("vc") = vconj(N(3));
  if (S("vc") != "-en"
   && S("vc") != "-edn")
     fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"passive",0);
@RULES
_xNIL <-
	_xWILD [s one match=(_morethan )]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	by [s lookahead]
	_xWILD [one match=(_det _noun _np)]
	@@

# once np
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"IN"); # coordinating conjunction.
@RULES
_xNIL <-
	once [s]
	_xWILD [one lookahead match=(_np _noun _det
		_quan _num _xNUM _adj)]
	@@

# ... to them causing changes
# np alpha noun
@CHECK
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  fixverb(N(2),"active","VBP");
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_np)]
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# this verb
# fnword verb
@CHECK
  if (N("id",1) == "pos50 this-verb")
    fail();
@POST
  L("tmp1") = N(1);
  chpos(N(1),"DT");
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,1);
  N("id",1) = "pos50 this-verb";
@RULES
_xNIL <-
	this [s]
	_xWILD [one lookahead match=(_verb _vg)]
	@@


# pro verb
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
  if (pnname(N(3)) == "_verb")
    L("v") = N(3);
  else if (pnname(N(3)) == "_vg")
    L("v") = N("verb node",3);
  if (L("v"))
    fixverb(L("v"),"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS _dblquote \,)]
	_pro
	_xWILD [one match=(_verb _vg)]
	@@

# pro verb
@CHECK
  if (N("pos50-pro-verb",2))
    fail();	# Loop guard.
@POST
  fixverb(N(2),"active","VBP");
  ++N("pos50-pro-verb",2); # Loop guard.
@RULES
_xNIL <-
	_pro
	_verb
	@@

# premod adj

@POST
  pncopyvars(2);
  sclearpos(1,0);
  single();
@RULES
_adj <-
	_premod
	_adj
	@@

# prep verb noun
# prep ven noun
# Changing verb to an adj
@CHECK
  if (N("paap",1))
    fail();	# Already reduced to verb...
  S("vc") = vconj(N(3));
  if (S("vc") != "-en"
   && S("vc") != "-edn"
   && S("vc") != "-ing")
   fail();
 if (S("vc") == "-ing" && !N("adj",3))
   fail();	# 06/28/06 AM.
@POST
  N("pos50 prep-verb-noun rename",3) = 1;
  pnrename(N(3),"_adj");
  if (S("vc") == "-en"
   || S("vc") == "-edn")
    chpos(N(3),"VBN");
  else
   chpos(N(3),"VBG");
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_advl _adv)]
	_verb
	_xWILD [one lookahead match=(_noun)]
	@@

# alpha adj
# far greater
@CHECK
  if (N("pos num",1) != 2)
    fail();
  if (!N("adj",1) || !N("adv",1))
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
  pncopyvars(2);
  sclearpos(1,0);
  single();
@RULES
_adj <-
	_xALPHA
	_adj
	@@

# dqan alpha
@CHECK
  if (!N("noun",5))
    fail();
  L("num1") = number(N(1));
  L("last") = lasteltnode(4);
  L("num4") = number(L("last"));
  if (L("num1") == L("num2"))
    fail();
  L("num5") = number(N(5));
  if (L("num1") != L("num5"))
    fail();
@POST
  fixnphead(5);
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	@@


# dqan alpha
# det adj alpha
@CHECK
  if (!N("noun",3))
    fail();
  # Agreement...
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
 
	L("fj") = N(2);
	L("lj") = lasteltnode(2);
	fixadjs(L("fj"),L("lj"));

  group(1,3,"_np");
  pncopyvars(L("tmp3"),N(1));
  clearpos(N(1),1,1);
@RULES
_xNIL <-
	_xWILD [s one match=(_det _proPoss)]
	_adj
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND
		_modal _vg _verb _pro
		\, # 06/29/06 AM.
		)]
	@@

# dqan adj
# det adj
@POST
  L("tmp2") = N(2);
  group(1,2,"_np");
  pncopyvars(L("tmp2"),N(1));
  clearpos(N(1),1,1);
@RULES
_xNIL <-
	_det
	_adj
	_xWILD [one lookahead match=(_prep _adv _verb)]
	@@

# rather than
# X rather than Y
# X more than Y
# More than 3 people did X.
# *Rather than 3 people did X.
# "rather" is special, and is not a true
# comparative, though it looks like one.


# num more than
@POST
 chpos(N(2),"JJR");
  # Fuse the phrase at least.
  group(2,3,"_morethan");
  group(2,2,"_fnword");
  N("morethan",2) = 1;
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(_quan _num _xNUM _money)]
	_xWILD [s one match=(more less fewer)]
	_xWILD [s one match=(than)]
	@@

# more than tripled
@POST
  chpos(N(1),"JJR");	# conform treebank. 75% more/jjr than/
  group(1,2,"_morethan");
  group(1,1,"_adv");
  clearpos(N(1),1,0);
  if (pnname(N(2)) == "_vg")
    fixvg(N(2),"active","VBD");
  else
    fixverb(N(2),"active","VBD");
@RULES
_xNIL <-
	more [s]
	than [s]
	_xWILD [one lookahead match=(_verb _vg)]
	@@



# more than a
@POST
  chpos(N(1),"JJR");
  group(1,2,"_det");	# 04/21/07 AM. Better than _prep.
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(more less fewer)]
	than [s]
	a [s]
	@@

# Treating as default.	# 06/17/06 AM.
# more than num
# Not certain, but usually parse this way.
@POST
  chpos(N(1),"JJR");
#  group(1,2,"_prep");	# 06/02/06 AM.
  group(1,2,"_advl");	# 06/02/06 AM.
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(more less fewer)]
	than [s]
#	_xWILD [one match=(_xNUM _num _quan)]
	@@

# verb more
@CHECK
  if (N("mypos",2))
    fail(); # Loop guard.
@POST
  pnrename(N(2),"_adj");
  chpos(N(2),"JJR");	# conform treebank.	# 06/17/06 AM.
@RULES
_xNIL <-
	_xWILD [s one match=(_verb _vg)]
	_xWILD [s one match=(more less) except=(_adj)]
	_xWILD [one fail=(_adj _xALPHA _adv)]
	@@

# more adv
# more adj
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"RBR");
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	_xWILD [s one match=(more less)]
	_xWILD [one lookahead match=(_adv _advl _adj)]
	@@

# most adv
# most adj
@POST
  if (!N("mypos",1))
    chpos(N(1),"RBS");
  if (pnname(N(1)) != "_adv")
    pnrename(N(1),"_adv");
  L("nm") = pnname(N(2));
  group(1,2,L("nm"));
  N("ignorepos",1) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(most least)]
	_xWILD [one match=(_adv _advl _adj)]
	@@

# more and more
@CHECK
  if (!N("mypos",3))
    fail();
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),N("mypos",3));
  if (N("mypos",3) == "JJR")
    {
    group(1,3,"_adj");
	clearpos(N(1),1,0);
	}
@RULES
_xNIL <-
	more [s]
	_conj
	more [s]
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

# pattern...
# noun that alpha alpha prep
# that
@CHECK
  if (!N("verb",3))
    fail();
  if (!N("noun",4))
    fail();
  S("vc") = vconj(N(3));
  if (S("vc") != "-ed"
   && S("vc") != "-edn"
   && S("vc") != "-s"
   && S("vc") != "inf")
    fail();
  # Todo: noun-verb agreement.
@POST
  L("tmp3") = N(3);
  L("tmp4") = N(4);

  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VBP");
  group(3,3,"_vg");
  pncopyvars(L("tmp3"),N(3));
  N("voice",3) = "active";
  clearpos(N(3),1,0);

  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  group(4,4,"_np");
  pncopyvars(L("tmp4"),N(4));
  clearpos(N(4),1,1);
  
  chpos(N(2),"WDT");
  N("bracket",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _advl)]
	that [s]
	_xALPHA
	_xALPHA
	_xWILD [lookahead one match=(_prep _qEOS _xEND)]
	@@

# Broad, but could be pretty good.	# 07/03/06 AM.
# Advl should be a prepositional phrase.
@CHECK
  if (N("mypos",2))
    fail();
  if (pnname(N(1)) == "_advl")
    if (!N("pp",1))
	  fail();
  if (!vconjq(N(4),"-s")
   && !vconjq(N(4),"-ed"))
    fail();
@POST
  chpos(N(2),"WDT");	# that/WDT.
  fixverb(N(4),"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _advl)]
	that [s]
	_xWILD [star lookahead match=(_advl _adv)]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# that alpha alpha $
# that alpha alpha prep
@CHECK
  if (N("verb",2))
    fail();
  if (!N("verb",3))
    fail();
@POST
  # Leave pos of "that" for something with more context.
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VBD");

  if (N("adv",2))
    {
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	}
  else if (N("noun",2) || N("unknown",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	}
  else
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_prep _qEOS _xEND)]
	@@

# that alpha alpha verb
# Note: Not worrying about that/det vs others.
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xWILD [one match=(_fnword \, _clause )]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# adv alpha
@CHECK
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_adv
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM)]
	@@


# pro alpha and adj
@CHECK
  if (!N("adj",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS _pro _det _quan _num _xNUM
#		_adj	# Too chancy.	# 05/29/05 AM.
		_adv \, )]
	_xALPHA
	_conj [lookahead]
	_adj
	@@

# adj alpha conj adj
# Ambiguous, will choose noun for simplicity.
@CHECK
  if (!N("noun",2) || !N("adj",2))
    fail();
@POST
	L("tmp2") = N(2);
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_conj [lookahead]
	_adj
	@@

# alpha conj alpha
@CHECK
  if (!N("adj",3) || !N("adj",5))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp5") = N(5);
  group(5,5,"_adj");
  pncopyvars(L("tmp5"),N(5));
  fixadj(N(5));
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
  group(3,5,"_adj");
@RULES
_xNIL <-
	_xWILD [plus match=(_pro _det _quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_conj
	_xALPHA
	_xWILD [one lookahead match=(_noun _adj)]
	@@

# Sentinel to keep "pro alpha any" from matching
# if there's a conjunction possibility.
# Don't know if alpha is a noun or adj yet.
@POST
  noop();
@RULES
_xNIL <-
	_proPoss [s]
	_xALPHA
	_conj
	_xALPHA
	@@

# pro alpha
@CHECK
  if (!N("adj",2) && !N("noun",2) && !N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  L("nm") = pnname(N(3));
  if (L("nm") == "_adj")
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else if (L("nm") == "_noun" && N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	_proPoss [s]
	_xALPHA
	_xWILD [one lookahead fail=(_xALPHA)]
	@@

# prep this adj
@CHECK
  if (N("mypos",2))
    fail();
@POST
  pnrename(N(2),"_det");
  chpos(N(2),"DT");	# that/DT
@RULES
_xNIL <-
	_prep
	this [s]
	_xWILD [one lookahead match=(_adj _quan _xNUM _num _noun)]
	@@

# noun , dqan 's alpha alpha alpha noun ,
# Apposition.
@CHECK
  if (!N("adj",8) && !N("noun",8))
    fail();
  if (N(9))
    if (!N("adj",9) && !N("noun",9))
	  fail();
  if (N(10))
    if (!N("adj",10) && !N("noun",10))
	  fail();
@POST
  if (N(10))
    fixnpnonhead(10);
  if (N(9))
    fixnpnonhead(9);
  fixnpnonhead(8);
  chpos(N(7),"POS");	# 's/POS
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\, [s]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_aposS
	_xALPHA
	_xALPHA [opt]
	_xALPHA [opt]
	_noun [plus]
	_xWILD [one lookahead match=(_qEOS _xEND \, _clausesep
		_fnword)]
	@@

# noun , dqa alpha alpha
# apposition
@CHECK
#  if (N("mypos",1) != "NP")	# check proper noun.
#    fail();
  if (!N("adj",4) && !N("verb",4) && !N("noun",4))
    fail();
  if (!N("noun",5))
    fail();
  S("vc4") = vconj(N(4));
  if (S("vc4") == "-s"
   || S("vc4") == "-ed")
    fail();
@POST
  L("tmp4") = N(4);
  L("tmp5") = N(5);
  if (N("noun",5))
    {
    group(5,5,"_noun");
    pncopyvars(L("tmp5"),N(5));
	}
  else
    {
    group(5,5,"_adj");
    pncopyvars(L("tmp5"),N(5));
	fixadj(N(5));
	}
  if (N("adj",4))
    {
    group(4,4,"_adj");
	pncopyvars(L("tmp4"),N(4));
	fixadj(N(4));
	}
  else if (N("verb",4))
    {
	group(4,4,"_verb");
	pncopyvars(L("tmp4"),N(4));
	fixverb(N(4),"passive","VBP");
	}
  else
    {
    group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	}
  group(3,5,"_np");
  pncopyvars(L("tmp5"),N(3));
  clearpos(N(3),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\, [s]
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND \, _clausesep
		_fnword)]
	@@

# noun , dqan alpha alpha ,
# apposition.
@CHECK
  if (!N("noun",5) || !N("noun",6))
    fail();
  if (!singular(N(5)) || !singular(N(6)))
    fail();
@POST
  fixnphead(6);
  fixnpnonhead(5);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [one match=(_det _pro)]
	_noun [plus]
	_xALPHA
	_xALPHA
	\, [lookahead]
	@@

# conj that the
@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"IN");	# that/IN
@RULES
_xNIL <-
	_xWILD [one match=(_conj)]
	that [s]
	_xWILD [one lookahead match=(_det)]
	@@

# dqan alpha
@CHECK
  if (!N(2) && !N(3) && !N(4) && !N(5))
    fail();
  if (!N("noun",7))
    fail();
  if (N("verb",7))
    {
	if (!vconjq(N(7),"inf"))
	  fail();
	if (vconjq(N(7),"-ed"))	# 04/21/07 AM.
	  fail();
	}
@POST
  L("tmp7") = N(7);
  group(7,7,"_noun");
  pncopyvars(L("tmp7"),N(7));
  fixnoun(N(7));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\, [opt]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	@@


# TRYING TO ACCEPT NEW PARTS OF SPEECH NOT IN DICT.
# Mangle preassigned pos sometimes.
# may noun up
# may noun down
# he nouns up
# he nouns up
@CHECK
  if (number(N(2)) == "plural")
    fail();
@POST
  pnrename(N(2),"_verb");
  chpos(N(2),"VB");
  chpos(N(3),"RP");
@RULES
_xNIL <-
	_modal
	_noun
	_xWILD [s one lookahead match=(up down)]
	@@

# In this case, check agreement etc.
@CHECK
#  if (!numbersagree(N(1),N(2)))
#    fail();
  if (N("mypos",2))
    fail();
@POST
  if (number(N(2)) == "plural")
   chpos(N(2),"VBZ");
  else
    chpos(N(2),"VBP");
  chpos(N(3),"RP");
  pnrename(N(2),"_verb");
@RULES
_xNIL <-
	_proSubj [s]
	_noun
	_xWILD [s one lookahead match=(up down)]
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
  L("neg") = mhbvadv(2,0,0,0);
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
	_xWILD [one match=( \, _dblquote _qEOS _xSTART _conj _clause
		_noun _np)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(say says said)]
	@@

# noun , alpha prep
@CHECK
  if (!N("verb",4))
    fail();
  if (N("adv",4))
    fail();
@POST
  L("tmp4") = N(4);
  L("v") = alphatoverb(4,0,0);
  L("neg") = mhbvadv(3,0,0,0);
  group(4,4,"_vg");
  pncopyvars(L("tmp4"),N(4));
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
#  N("voice",4) = "active";
  clearpos(N(4),1,0);
  N("verb node",4) = L("v");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\, [s]
	_adv [star]
	_xALPHA
	_xWILD [s one lookahead match=(_prep as)
		except=(of)]
	@@

# Initial adverbial.
# adv ,
@POST
  S("no subj") = 1;
  S("pattern") = "advl";
  single();
@RULES
_clause <-
	_xSTART
	_adv
	\,
	@@

# down from
# up from
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"RB");
@RULES
_xNIL <-
	down [s]
	_prep [lookahead]
	@@

# Complex apposition
# np prep np , noun ,
# Assume intervening adverbial.
@POST
  L("tmp3") = N(3);
  group(2,3,"_advl");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,0);
  N("pp",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_np _noun)]
	_prep
	_np
	\, [lookahead]
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [one fail=(_noun _np _xALPHA)]
	@@

# prep np
@POST
  group(2,3,"_advl");
  N("pp",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART \, _clausesep)]
	_prep
	_np
	_xWILD [s one fail=(of _conj)]
	@@

# , prep noun ,
@POST
  L("tmp3") = N(3);
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);

  group(2,3,"_advl");
  N("pp",2) = 1;
@RULES
_xNIL <-
 	\,
	_prep
 	_noun
	\, [lookahead]
	_xWILD [one fail=(_noun _xALPHA)]
	@@

# watch for lists...
# noun , noun ,
# Apposition.
@CHECK
  L("prev") = pnprev(N(1));
  if (L("prev"))
    {
	L("nm") = pnname(L("prev"));
	if (L("nm") == ",")
	  fail();
	}
  if (N("cap",1) && N("cap",4))
    succeed();
  if (N("mypos",1) != "NP"
   && N("mypos",4) != "NP")
     fail();
@POST
  N("apposition",1) = 1;
  N("pos50 n-n",1) = 1;
  group(1,5,"_np");
  clearpos(N(1),1,0);
  N("compound-np",1) = 1;
@RULES
_xNIL <-
	_np	# 06/07/05 AM.
	_xWILD [star match=(_advl _adv)]
	\,
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [one lookahead fail=(_noun _np)]
	@@

# Apposition.	# 06/04/06 AM.
# noun , num ,
@CHECK
  if (!N("ne",1) && !N("ne arr",1))
    fail();
@POST
  N("apposition",1) = 1;
  listadd(1,3,"true");
@RULES
_xNIL <-
	_np
	\,
	_xNUM
	\, [lookahead]
	@@

# Apposition. # 06/04/06 AM.
# noun , dqan ,
@POST
  # Todo: reduce to np first.
  dqaninfo(3,4,5,6);
  groupnp();
  N("apposition",1) = 1;
  listadd(1,4,"true");
@RULES
_xNIL <-
	_np
	\,
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	\,
	@@

# Apposition.	# 06/04/06 AM.
# noun , alpha ,
@CHECK
  if (!N("noun",3))
    fail();
@POST
  N("apposition",1) = 1;
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  listadd(1,3,"true");
@RULES
_xNIL <-
	_np
	\,
	_xALPHA
	_xWILD [one lookahead match=( \, _xEND)]
	@@

@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
  L("tmp2a") = N(2);
  group(2,5,"_np");
  pncopyvars(L("tmp2a"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_noun
	\,
	_np
	\,
	@@

# noun verb
# noun vg
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
  if (pnname(N(3)) == "_vg")
    if (!N("voice",3))
	  N("voice",3) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_noun
	_xWILD [one lookahead match=(_vg _verb)]
	@@

# clause-like pattern.
# noun , verb
# np , vg
@CHECK
  if (N("pos50 n-comma-v",4))
    fail();	# Done already.  Avoid infinite loop.

  # If a complex noun phrase, no confidence.	# 06/21/06 AM.
  if (N("apposition",2))
    fail();
@POST
  if (N("voice",4))
    ; # Done.
  else if (pnname(N(4)) == "_vg")
   fixvg(N(4),"passive","VBN");
  else
    fixverb(N(4),"passive","VBN");
  N("pos50 n-comma-v",4) = 1;
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_noun _np)]
	\, [s]
	_xWILD [one match=(_verb _vg)]
	@@

# np np (date)
@CHECK
  if (!N("advl",2))	# 04/15/05 AM.
    fail();
@POST
  L("tmp2") = N(2);
  if (pnname(N(2)) == "_noun")
    {
	group(2,2,"_np");
	pncopyvars(L("tmp2"),N(2));
	clearpos(N(2),1,1);
	}
  group(2,2,"_advl");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _adv _advl)]
	_xWILD [one match=(_noun _np)]
	@@

# mr. alpha
@CHECK
  if (N("mypos",2) != "NP")
    fail();
  if (!N("unknown",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"NP");
  L("tmp3n") = N(3);
  group(2,3,"_np");
  pncopyvars(L("tmp3n"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _verb _vg)]
	_noun
	_xALPHA
	_xWILD [one lookahead match=(
		_prep _verb _vg _adv _advl _np \, _qEOS _xEND)]
	@@

# alpha inc.
@CHECK
  if (N("mypos",3) != "NP")
    fail();
  if (!N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"NP");
  L("tmp2n") = N(2);
  group(2,3,"_np");
  pncopyvars(L("tmp2n"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_prep _verb _vg _adv _advl _noun _np \, )]
	_xALPHA
	_noun
	_xWILD [one lookahead match=( \, _qEOS _xEND _prep _verb _vg
		_np)]
	@@

# those noun
# these noun
# Slough fnword.
@CHECK
  if (N("id",1) == "pos50 those-n")
    fail();	# Infinite loop guard.
@POST
  pnrename(N(1),"_det");
  N("id",1) = "pos50 those-n";
@RULES
_xNIL <-
	_xWILD [s one match=(these those)]
	_noun [lookahead]
	@@

# those verb
@CHECK
  if (N("mypos",2))
    fail();
  if (N("pos50 those-verb",1))
    fail();	# Loop guard.
@POST
  N("pos50 those-verb",1) = 1;
  fixverb(N(2),"passive","VBN");
@RULES
_xNIL <-
	those [s]
	_xWILD [one match=(_verb) except=(_have _be)]
	@@

# prep num
@POST
  if (!N("mypos",1))
    {
    if (N("stem",1) == "up"
     || N("stem",1) == "down")
     chpos(N(1),"RB");
    }
  group(2,2,"_np");
  N("bracket",2) = 1;
  group(1,2,"_advl");
  N("pp",1) = 1;
@RULES
_xNIL <-
	_prep
	_xWILD [one match=(_xNUM _num _quan)]
	_xWILD [s one lookahead match=(_fnword _qEOS _xEND)
		except=(more less)]
	@@

# np of np
@POST
  if (N("mypos",1) == "NP" && N("mypos",3) == "NPS")
  	setpos(N(3),"NP");
  if (N("cap",1) && N("cap",3))
    L("cap") = 1;
  L("stem") = N("stem",3);
  if (!L("stem"))
    L("stem") = N("stem",1);
  L("arr") = semnpnp(N(1),N(3));
  L("sem") = L("arr")[0];
  L("ne text") = L("arr")[1];
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  group(1,3,"_np");
  domnpofnp(N(1),L("tmp1"),L("tmp3"));
#  semnpofnp(N(1),L("tmp1"),L("tmp3"));
  if (L("cap"))
    N("cap",1) = 1;
  if (L("sem"))
    N("sem",1) = L("sem");
  if (L("ne text"))
	N("ne text",1) = L("ne text");
  if (L("stem"))
    N("stem",1) = L("stem");
@RULES
_xNIL <-
	_np
	of [s]
	_np
	@@

# prep np of np
@POST
  group(2,4,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xWILD [one match=(_noun _np)]
	of [s]
	_xWILD [one match=(_noun _np)]
	_xWILD [one lookahead match=(_adv _advl _clausesep \, )]
	@@

# as np vg
# as noun verb
# Note: as - conjunction.
@POST
  pnrename(N(3),"_fnword");
@RULES
_xNIL <-
	_xWILD [one match=(_pro _np)]
	_xWILD [star match=(_adv _advl)]
	as [s except=_fnword]  # Loop guard.
	_np
	_xWILD [one match=(_modal _verb _vg)]
	@@

# prep np
@POST
  L("tmp4") = N(4);
  group(3,4,"_advl");
  pncopyvars(L("tmp4"),N(3));
  clearpos(N(3),1,0);
  N("pp",3) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_pro _np)]
	_xWILD [star match=(_adv _advl)]
	_prep
	_np
	_xWILD [one fail=( \, _conj _prep)]
	@@


# by np
@POST
  L("tmp2") = N(2);
  group(1,2,"_advl");
  pncopyvars(L("tmp2"),N(1));
  clearpos(N(1),1,0);
  N("pp",1) = 1;
  if (N("sem",2) != "date" && N("sem",2) != "geoloc")
    N("by-actor",1) = 1;	# 04/21/07 AM.
  N("by-np") = 1;	# 04/21/07 AM.
@RULES
_xNIL <-
	by [s]
	_np
	@@

# A clausal look.
# vg np be
@CHECK
  if (N("id",1) == "pos50 v-n-be")
    fail();	# Infinite loop guard.
  if (N("q-q remote",1) == "right")
    fail();
@POST
  N("glom",1) = "left";
  N("glom",2) = "right";
  N("id",1) = "pos50 v-n-be";
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one lookahead match=(_noun _np)]
	_be [s]
	@@


# np pro vg
@POST
  group(2,2,"_np");
  N("bracket",2) = 1;
  N("glom",2) = "right";
@RULES
_xNIL <-
	_xWILD [one match=(_np)]
	_pro
	_xWILD [one lookahead match=(_vg)]
	@@

# money
@POST
  S("sem") = "money";
  single();
@RULES
_np <-
	_xWILD [one match=(\$ \#)]
	_xWILD [plus match=(_num _quan _xNUM)]
	_noun [plus]
	@@

#@POST
#  S("sem") = "money";
#  singler(1,2);
#@RULES
#_np <-
#	\$
#	_xWILD [plus match=(_num _quan _xNUM)]
#	_xWILD [one lookahead fail=(_xALPHA)]
#	@@

# noun & noun
@RULES
_noun <-
	_noun
	\& [s]
	_noun
	@@

# $5 a share
@CHECK
  if (N("pos50 $-a-share",2) = 1)
    fail();
@POST
  pnrename(N(2),"_prep");  # as in "$5 per share".
  N("pos50 $-a-share",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	a [s]
	_noun
	@@

# to verb
# to infinitive...
@CHECK
  if (N("mypos",2))
    fail();
  if (N("pos50 to-verb-inf",2))
    fail();
@POST
  if (vconjq(N(2),"inf"))
    chpos(N(2),"VB");
  N("pos50 to-verb-inf",2) = 1;
@RULES
_xNIL <-
	to [s]
	_verb [lookahead]
	@@

# prep adj
@POST
  pnrename(N(2),"_noun");
#  chpos(N(2),0);	# Clear out adj etc.
  fixnoun(N(2));
  N("rename-adj-noun",2) = 1;
@RULES
_xNIL <-
	_prep
	_adj
	_xWILD [one lookahead fail=(_np _adj _noun
		_quan _num _xNUM _xALPHA \" _dblquote)]
	@@

# Some transitive verbs.
# Avoiding hardwiring such rules in general.
# Want the analyzer to identify these automatically....
# verb prep
@CHECK
  if (N("pos50 verb-prep",1))
    fail();
@POST
  L("nm") = pnname(N(1));
  if (strpiece(L("nm"),0,0) == "_")
    {
	# nonliteral.
	if (L("nm") == "_vg")
	  {
	  N("voice",1) = "passive";
	  L("v") = N("verb node",1);
	  }
	else
	  L("v") = N(1);
	if (L("v"))
	  {
	  chpos(L("v"),"VBN");
	  pnreplaceval(L("v"),"voice","passive");
	  }
	}
  else
    {
	L("tmp1") = N(1);
	group(1,1,"_verb");
	pncopyvars(L("tmp1"),N(1));
	chpos(N(1),"VBN");
	}
  N("pos50 verb-prep",1) = 1; # Loop guard.
@RULES
_xNIL <-
	_xWILD [s one match=(found)]
	_xWILD [star lookahead match=(_adv _advl)]
	_prep
	@@

# prep dqan alpha
# unless something happens...
@CHECK
  if (!N("verb",5))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_verb");
  pncopyvars(L("tmp5"),N(5));
  L("v") = N(5);
  group(5,5,"_vg");
  mhbv(N(5),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp5"),N(5));
  N("verb node",5) = L("v");
  clearpos(N(5),1,0);
  fixvg(N(5),"active","VBP");
@RULES
_xNIL <-
	_xWILD [s one match=(until unless _fnword)]
	_xWILD [star match=(_det _quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=( \, _qEOS _xEND _conj)]
	@@

# prep dqan alpha
# for the boy scout
@CHECK
  if (!N("noun",5))
    fail();
  if (N("verb",5))
    {
	S("vc") = vconj(N(5));
	if (S("vc") == "-ed"
	 || S("vc") == "-edn"
	 || S("vc") == "-en")
	 fail();
	}
  if (N("advl",5))	# 04/15/05 AM.
    fail();
  # If last noun is plural, no good.	# 04/18/05 AM.
  S("head") = lasteltnode(4);
  L("txt") = nodetext(S("head"));
  if (finddictattr(L("txt"),"soft-plural"))
    succeed();
  if (plural(S("head")))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  # Fix the range of nouns.
#  fixnouns(N(4),N(5));
#  group(2,5,"_np");
#  pncopyvars(L("tmp5"),N(2));
#  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(for with of at in on under by)]
	_xWILD [star match=(_det _quan _num _adv _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=( \, _qEOS _xEND
		_modal _verb _vg	# 06/29/06 AM.
		_prep _fnword	# 07/10/06 AM.
		_conj
		)]
	@@
# prep dqan alpha
# for the boy scout
@CHECK
  if (!N("noun",5))
    fail();
  if (N("advl",5))	# 04/15/05 AM.
    fail();
  if (N("adj",5))	# 06/04/06 AM.
  	fail();
  # Not enough context...
  if (N("verb",5))
    fail();	# 08/24/06 AM.

  # If last noun is plural, no good.	# 04/18/05 AM.
  S("head") = lasteltnode(4);
  if (plural(S("head")))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  # Fix the range of nouns.
#  fixnouns(N(4),N(5));
#  group(2,5,"_np");
#  pncopyvars(L("tmp5"),N(2));
#  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(for with of at in under by)]
	_xWILD [star match=(_det _quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=( _conj _noun _np)]
#	_xWILD [s one match=(which that)]
	@@

# prep alpha noun
# by alpha noun
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  alphatoverb(2,"active","VBG");
@RULES
_xNIL <-
	by [s]
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# prep dqan alpha noun
# prep dqa alpha noun
@CHECK
  if (!N(2) && !N(3)
   && N("verb",4))
    {
	if (vconjq(N(4),"-ing"))
	  fail();
	}
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  fixnpnonhead(4);
@RULES
_xNIL <-
	_xWILD [s one match=(for with of at in under by)]
	_xWILD [star match=(_det _quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_noun [lookahead]
	@@

# prep dqan alpha
# prep dqa alpha
# prep adv dqan alpha
# prep adj alpha
@CHECK
  if (!N("noun",4))
    fail();
@POST
  fixnphead(4);
@RULES
_xNIL <-
	_prep
	_adv [plus]
	_adj
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# that noun alpha
@CHECK
  if (!N("verb",3) || N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xWILD [one match=(_noun _np)]
	_xALPHA
	@@
	

# prep noun alpha
# Handle mass nouns...
@CHECK
  if (!N("noun",3))
    fail();
  if (N("adj",3))
    fail();	# 06/04/06 AM.
  S("number") = number(N(2));
  if (S("number") != "singular")
    fail();
  if (N("verb",3))
    {
	# Need some context.	# 06/08/06 AM.
	if (N("cap",2) || N("ne",2) || N("ne arr",2))
	  fail();
	if (!N("verbal",1)	# Not a "verbal" preposition. # 06/08/06 AM.
	 && N("mass",2))	# And a mass noun.
		fail();				# 06/08/06 AM.
	S("vc") = vconj(N(3));
	if (S("vc") != "inf"
	 && S("vc") != "-s")
	  fail();
	}
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
#  fixnoun(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_noun
	_xALPHA
#	_xWILD [one lookahead fail=(_xALPHA _np
#		_pro _adv)] # 06/05/06 AM.
	@@

# prep noun alpha
@CHECK
  if (!singular(N(2)))
    fail();
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
	# Todo: could broaden this.
	if (!vconjq(N(3),"-ing"))
	  fail();
	}
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_prep
	_noun
	_xALPHA
	_xWILD [one fail=(_noun _xALPHA)]
	@@

# dqan alpha alpha noun
# adj alpha alpha noun
@CHECK
  if (N("verb",2) || N("verb",3))
    fail();
  if (!N("adj",2) && !N("noun",2) && !N("unknown",2))
    fail();
  if (!N("adj",3) && !N("noun",3) && !N("unknown",3))
    fail();
@POST
  fixnpnonhead(2);
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# dqan alpha alpha
@CHECK
  if (!N("noun",3) && !N("unknown",3))
    fail();
  if (!N("verb",4))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp4") = N(4);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));

  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
  L("v") = N(4);
  group(4,4,"_vg");
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp4"),N(4));
  N("verb node",4) = L("v");
  clearpos(N(4),1,0);
  fixvg(N(4),"active","VBP");

  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_np _det _quan _num _xNUM)]
	@@

# dqan alpha alpha dqan
@CHECK
  if (N("pos50 dqan-aa-dqan",5))
    fail();
@POST
  N("pos50 dqan-aa-dqan",5) = 1;
  if (N("noun",5) && N("verb",6))
    {
	if (plural(N(5)) && vconjq(N(6),"inf"))
	  {
	  fixnphead(5);
	  alphatoverb(6,"active","VBD");
	  }
	}
@RULES
_xNIL <-
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_np _det _quan _num _xNUM)]
	@@

# verb dqan alpha [NIBBLE]
# verb adj noun alpha [NIBBLE]
@CHECK
  if (!N("noun",4))
    fail();
  if (!singular(N(3)) && !N("mass",3))
    fail();
@POST
  alphatonoun(4);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_adj
	_noun
	_xALPHA
	@@

# verb dqan alpha alpha [NIBBLE]
@CHECK
  if (!N("adj",6) && !N("noun",6))
    fail();
  if (N("adv",6) && !N(4) && !N(5))
    fail();
@POST
  if (!N("adj",6))
    alphatonoun(6);
  else if (!N("noun",6))
    alphatoadj(6);
  else
    alphatoadj(6);	# adj or noun, don't know.
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	_xWILD [one match=(_xEND _prep _modal _verb _vg
		_fnword _conj _clausesep)]
	@@

# verb dqan alpha
# NIBBLE.
@CHECK
  if (!N("noun",7))
    fail();
  if (!singular(N(7)) || !singular(N(6)))
    fail();
  if (N("mass",6))
    fail();
@POST
  alphatonoun(7);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun
	_xALPHA	# 7
	@@

# verb dqan alpha alpha
@CHECK
  if (!N("noun",4) || !N("noun",5))
    fail();
@POST
  if (N(2))
    {
	# Fix up any adjs in here...
	L("lastj") = lasteltnode(2);
	fixadjs(N(2),L("lastj"));
	}
  L("tmp4") = N(4);
  L("tmp5") = N(5);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  fixnoun(N(5));
  group(2,5,"_np");
  pncopyvars(L("tmp5"),N(5));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro _quan _num _xNUM _adj)]
	_noun [plus]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _conj _fnword _verb _vg
		_qEOS _xEND \, _clausesep )]
	@@

# dqan 's alpha noun verb
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
@POST
  chpos(N(2),"POS");
  L("tmp3") = N(3);
  if (N("adj",3))
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
  else
    {
	group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	fixnoun(N(3));
	}
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun)]
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_noun [lookahead]
	_xWILD [star match=(_advl _adv)]
# Trying broader rule.	# 06/24/06 AM.
#	_xWILD [one match=(_verb _vg)]
	@@

# noun 's alpha verb
@CHECK
  if (!N("noun",3) && !N("unknown",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# noun 's alpha
@CHECK
  if (!N("noun",3) && !N("unknown",3))
    fail();
  if (N("verb",3))
    {
	if (!vconjq(N(3),"inf") && !vconjq(N(3),"-s"))
	  fail();
	}
@POST
  if (pnname(N(4)) == "_noun")
    fixnpnonhead(3);
  else
    {
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
    }
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_xWILD [one lookahead fail=(_adj _xALPHA)]
	@@

# 's alpha prep verb
# 's alpha to verb
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
  L("stem") = nodestem(N(2));
  if (!finddictattr(L("stem"),"T3")) # v to v
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));

  group(1,1,"_be");
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_aposS
	_xALPHA
	to [s lookahead]
	_xWILD [one match=(_verb _vg)]
	@@

# 's alpha prep
@CHECK
  if (N("noun",2))
    fail();	# 08/24/06 AM.
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));

  group(1,1,"_be");
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_aposS
	_xALPHA
	_prep [lookahead]
	@@

# noun 's alpha dqan verb
@CHECK
  if (!N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_adj [plus lookahead]
	_noun [plus]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# noun 's
@POST
  chpos(N(2),"POS");
  L("tmp1") = N(1);
  group(1,2,"_adj");
  pncopyvars(L("tmp1"),N(1));
  N("possessive",1) = 1;
  N("ne",1) = N("ne type",1) = 0;
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_np _adj _noun
		_num _quan _xNUM)]
	@@
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	_xWILD [s one match=(most least more less)]
	_xWILD [one lookahead match=(_np _noun)]
	@@

# noun 's
@CHECK
  if (N("mypos",2) != "POS")
    fail();
@POST
  group(1,2,"_adj");
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg _det _pro)]
	@@

# noun 's
@CHECK
  if (N("mypos",2) != "POS")
    fail();
@POST
  group(1,2,"_adj");
  N("apos-s",1) = 1;
  N("possessive",1) = 1;
@RULES
_xNIL <-
	_noun
	_xWILD [one match=(_aposS _aposX)]
	@@

# 's not alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"-en")
   && !vconjq(N(3),"-ing"))
    fail();
@POST
  L("tmp1") = N(1);
  pnrename(N(1),"_be");
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"VBZ");
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_aposS
	_neg [s]
	_xALPHA
	@@


# alpha noun
@CHECK
  if (N("pos num",1) != 2)
    fail();
  if (!N("noun",1) || !N("verb",1))
    fail();
  S("number") = number(N(2));
  if (!vconjq(N(1),"inf"))
    fail();
  if (S("number") != "singular")
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  fixnoun(N(1));
@RULES
_xNIL <-
	_xALPHA
	_noun
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# verb noun to alpha
# vg np to alpha
# verb patterns...
@CHECK
  if (!N("verb",7))
    fail();
  if (!vconjq(N(7),"inf"))
    fail();
  L("stem") = nodestem(N(1));
  if (!finddictattr(L("stem"),"V3"))
    fail();
@POST
  alphatovg(7,"active","VB");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_pro _np _noun)]
	_xWILD [star match=(_adv _advl)]
	to [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# verb noun alpha
# vg np alpha
# verb patterns...
@CHECK
  if (!N("verb",5))
    fail();
  if (!vconjq(N(5),"inf"))
    fail();
  L("stem") = nodestem(N(1));
  if (!finddictattr(L("stem"),"V2")
   && !finddictattr(L("stem"),"T5"))
    fail();
@POST
  alphatovg(5,"active","VB");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_pro _np _noun)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# verb noun alpha
@CHECK
  if (N("pos50 vna",3))
    fail();
@POST
  N("pos50 vna",3) = 1;
  if (N("noun",3))
    fixnphead(3);
  else if (N("adv",3))
    alphatoadv(3);
  else if (N("adj",3))
    alphatoadj(3);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [lookahead one match=( \, _clausesep )]
	@@

# List looks...
# noun , alpha , and alpha
@CHECK
  if (N("n-alpha-alpha",6))
    fail();	# Loop guard.
  if (!N("noun",4) || !N("noun",7))
    fail();
@POST
  if (literal(N(4)))
	{
	L("tmp4") = N(4);
	group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	fixnoun(N(4));
	}
  if (literal(N(7)))
	{
	L("tmp7") = N(7);
	group(7,7,"_noun");
	pncopyvars(L("tmp7"),N(7));
	fixnoun(N(7));
	}
  N("n-alpha-alpha",6) = 1;
@RULES
_xNIL <-
	_xWILD [one fail=( \, _conj )]
	_noun
	\,
	_xWILD [one match=(_xALPHA _noun)]
	\, [opt]
	_conj
	_xWILD [one match=(_xALPHA _noun)]
	_xWILD [one lookahead match=(_xEND _qEOS _prep _advl _adv
			_verb _vg)]
	@@

# np conj np
# Note: Semantic compatibility...
@CHECK
  if (N("pos50 n-conj-n",2))
    fail();
@POST
  N("pos50 n-conj-n", 2) = 1;
  if (N("sem",2) == "date"
   && N("sem",5) == "date")
   group(2,5,"_np");
@RULES
_xNIL <-
	_vg
	_np
	_conj
	_adv [star]
	_np
	_xWILD [lookahead one match=(_prep)]
	@@

# Some list looks....
# noun , alpha , alpha , and alpha
# verb , alpha
# adj , alpha
# adv , alpha
# Todo: complete this, add more list cases, etc.
@CHECK
  if (N("pos50-and4",1)) # Loop guard.
    fail();
  S("nm") = pnname(N(1));
  if (S("nm") == "_noun")
    {
	if (!N("noun",3) || !N("noun",5))
	  fail();
	if (!N("noun",8) && !N("adj",8))
	  fail();
	}
@POST
  N("pos50-and4",1) = 1;	# Loop guard.
  if (S("nm") == "_noun" || S("nm") == "_np")
    {
	if (literal(N(3)))
	  {
	  L("tmp3") = N(3);
	  group(3,3,"_noun");
	  pncopyvars(L("tmp3"),N(3));
	  fixnoun(N(3));
	  }
	if (literal(N(5)))
	  {
	  L("tmp5") = N(5);
	  group(5,5,"_noun");
	  pncopyvars(L("tmp5"),N(5));
	  fixnoun(N(5));
	  }
	if (literal(N(8)))
	  {
	  if (N("noun",8))
	    {
	    L("tmp8") = N(8);
	    group(8,8,"_noun");
	    pncopyvars(L("tmp8"),N(8));
	    fixnoun(N(8));
	    }
	  else if (N("adj",8))
	    {
	    L("tmp8") = N(8);
	    group(8,8,"_adj");
	    pncopyvars(L("tmp8"),N(8));
	    fixadj(N(8));
	    }
	  }
	}
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _verb _adj _adv)]
	\,
	_xANY
	\,
	_xANY
	\, [opt]
	_conj
	_xANY
	@@

# noun conj
@CHECK
  if (!N("noun",5))
    fail();
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  L("tmp4") = N(4);
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  fixnoun(N(5));
  if (N("adj",4))
    {
	group(4,4,"_adj");
	pncopyvars(L("tmp4"),N(4));
	fixadj(N(4));
	}
  else
    {
	group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	fixnoun(N(4));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_prep _fnword)]
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_fnword _prep _conj _dbldash
		_qEOS _xEND)]
	@@

# noun conj alpha
@CHECK
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_prep _fnword _det _vg _verb)]
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_xWILD [one lookahead match=(_fnword _prep _conj _dbldash
		_qEOS _xEND)]
	@@

# it 's alpha
@CHECK
  if (N("stem",1) != "it")
    fail();
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
	S("vc") = vconj(N(3));
	if (S("vc") == "-ing"
	 || S("vc") == "-en"
	 || S("vc") == "-edn")
	 fail();	# Todo: Could reduce to _vg here.
	}
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);

  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,0);
  N("verb node",2) = L("v");
  fixvg(N(2),"active",0);
  chpos(L("v"),"VBZ");

  L("tmp1") = N(1);
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_pro
	_aposS
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@


# it 's dqan
@CHECK
  if (N("stem",1) != "it")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,0);
  N("verb node",2) = L("v");
  fixvg(N(2),"active",0);
  chpos(L("v"),"VBZ");

  L("tmp1") = N(1);
  group(1,1,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,1);
@RULES
_xNIL <-
	_pro
	_aposS
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# verb prep prep
@CHECK
  if (N("mypos",2))
    fail();
  if (N("pos50-particle",2))
    fail();
@POST
  N("pos50-particle",2) = 1;

	if (copulaq(N(1)))
	  {
	  chpos(N(2),"IN");	# eg, UP %   # 06/15/06 AM.
	  }
	# See if kb has something.	# 5/31/06 AM.
	else if (L("num") = phrprepverbq(N(1),N(2)))
	  {
	  if (L("num") == 2) # prepositional. # FIX. 06/18/06 AM.
	    chpos(N(2),"IN");
	  else # 1 or 3.
	    chpos(N(2),"RP");	# Default.
	  }
#	else	# Default.
#	  chpos(N(2),"RP");

@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep [lookahead]
	_xWILD [one match=(_prep _advl \, _dbldash _xEND)]
	@@
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [s one lookahead match=(up down out)]
	@@

# clausal
# verb noun prep
# up
# Check for phrasal verb.
@CHECK
  if (N("pos50-vnprep",5))
    fail();
  if (N("mypos",5))
    fail();
@POST
  N("pos50-vnprep",5) = 1;

  # Leaving stocks up.
  # Forcing stocks up.
  # verb pattern = v n adv
  # Require phrasal verb otherwise.
  # Prepositional is not valid because of intervening noun.
  S("num") = phrprepverbq(N(1),N(5));
  if (S("num") == 1 # PHRASAL.
   || S("num") == 3)
    chpos(N(5),"RP");
#  else
#	chpos(N(5),"RB");  # Not prepositional verb.
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

@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"RB");
@RULES
_xNIL <-
	_xWILD [one match=(_prep \, )]
	_xWILD [s one match=(up down)]
	_xWILD [one lookahead match=(_num _quan _xNUM _money)]
	@@
	

# Trying simple stupid for now...
# so adj
@CHECK
  if (N("mypos",1) == "RB")
    fail();	# Loop guard.
@POST
  pnrename(N(1),"_adv");
  chpos(N(1),"RB");
@RULES
_xNIL <-
	so [s]
	_adj [lookahead]
	@@
# so be
_xNIL <-
	so [s]
	_be [s lookahead]
	@@


# det noun '
@POST
  chpos(N(3),"POS");	# Possessive.
  group(2,3,"_adj");
@RULES
_xNIL <-
	_xWILD [one fail=(_noun _xALPHA)]
	_noun
	_aposX
	@@

# 's alpha dqan
@CHECK
  if (!N("verb",3))
    fail();
@POST
  # verb to be??
  chpos(N(1),"POS");	# 's/POS
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_aposS
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_det _pro)]
	@@

# 's alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (N("verb",2))
    {
	if (!vconjq(N(2),"inf")
	 && !vconjq(N(2),"-s"))
	  fail();
	}
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_xWILD [one lookahead fail=(_xALPHA _adj)]
	@@
_xNIL <-
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_xEND
	@@

# alpha 's noun
@POST
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  fixnoun(N(1));
  if (strisupper(strpiece(N("$text",1),0,0)))
    chpos(N(1),"NP");
  chpos(N(2),"POS");
  group(1,2,"_adj");
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one match=(_aposS _aposX)]
	_noun [lookahead]
	@@

@CHECK
  if (N("verb",2))
    fail();
  if (!N("adj",2))
    fail();
@POST
  if (!N("mypos",1))
    chpos(N(1),"POS");
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_adj [lookahead]
	@@

# num
# Lone number.
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_prep _conj)]
	_xWILD [one match=(_quan _num _xNUM)]
	_xWILD [one lookahead match=(_qEOS _prep _conj _xEND)]
	@@

# alpha dqan
@CHECK
  if (!N("noun",1))
    fail();
  if (N("verb",1)
   || N("adj",1))	# 02/10/05 AM.
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  fixnoun(N(1));
@RULES
_xNIL <-
	_xALPHA
	\, [opt]
	_xWILD [one lookahead match=(_det _quan _num _xNUM _np)]
	@@

# alpha dqan
@CHECK
  if (!N("verb",1))
    fail();
  S("vc") = vconj(N(1));
  if (S("vc") != "-edn"
   && S("vc") != "-ed"
   && S("vc") != "-en")
   fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _np)]
	@@

# fnword alpha dqan
# before eating food
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);
  fixvg(N(2),"active","VBG");
@RULES
_xNIL <-
	_xWILD [one match=(_fnword)]
	_xALPHA
	_xWILD [one lookahead match=(_np _pro _det
		_quan _num _xNUM)]
	@@

# adj conj alpha
@CHECK
  if (!N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
  listadd(1,3,"true");
@RULES
_xNIL <-
	_adjc
	_conj
	_xALPHA
	_xWILD [one match=(_pro _np _prep)]
	@@

# adj conj alpha
@CHECK
  if (!N("adj",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_adj");
  pncopyvars(L("tmp4"),N(4));
  fixadj(N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_adj _adjc)]
	_xWILD [one match=( \, _dbldash)]
	_conj
	_xALPHA
	_xWILD [one lookahead match=( \, _dbldash _qEOS _xEND)]
	@@

# verb dqan alpha
# pattern: v n adj
@CHECK
  if (!N("adj",6))
    fail();
  S("stem") = nodestem(N(1));
  if (!finddictattr(S("stem"),"vnj")) # keep it green ...
    fail();
  S("head") = lasteltnode(5);
  dqaninfo(2,3,4,5);
@POST
  L("tmp6") = N(6);
  group(6,6,"_adj");
  pncopyvars(L("tmp6"),N(6));
  fixadj(N(6));
  L("tmp6") = N(6);
  group(6,6,"_adjc");
  pncopyvars(L("tmp6"),N(6));
  clearpos(N(6),1,1);

  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_xEND _dbldash _adv _prep \, )]
	@@

# verb dqan alpha
@CHECK
  if (N("pos50 dqan-a",6))
    fail();
  if (N("stem",7) == "to")
    fail();
@POST
  N("pos50 dqan-a",6) = 1;
  L("stem") = nodestem(N(1));
  if (N("verb",6))
    {
    if (finddictattr(L("stem"),"V4")
     && vconjq(N(6),"-ing"))
	   {
	   alphatoverb(6,"active","VBD");
	   L("done") = 1;
	   }
    else if (finddictattr(L("stem"),"V8")
     && vconjq(N(6),"-en"))
	   {
	   alphatoverb(6,"passive","VBN");
	   L("done") = 1;
	   }
    else if (finddictattr(L("stem"),"V2")
     && vconjq(N(6),"inf"))
	   {
	   alphatoverb(6,"active","VB");
	   L("done") = 1;
	   }
   }

  if (!L("done") && N("noun",6))
    fixnphead(6);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_xEND _fnword
		 _dbldash _adv _prep \, )]
	@@

# verb prep dqan alpha
@CHECK
  if (!N("noun",8))
    fail();
  if (!N(4) && !N(5) && !N(6) && !N(7))
    fail();
  if (N("verb",8))
    {
	if (N(7))
	  {
	  S("head") = lasteltnode(7);
	  if (plural(S("head")) || pnvar(S("head"),"mass"))
	    fail();
	  }
	}
@POST
  L("tmp8") = N(8);
  groupone(N(8),8,"_noun",1);
#  pncopyvars(L("tmp8"),N(8));
  fixnoun(N(8));
  dqaninfo(4,5,6,7);
  S("olast") = 8;
  S("last") = S("lastn") = S("lastan") = N(8);
  if (!N(7))
    S("firstn") = S("firstan") = N(8);
  if (!N(4) && !N(5) && !N(6) && !N(7)) # Just in case.
    S("first") = N(8);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_prep [opt]
	_xWILD [s star match=(_det _proPoss _adv)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]	# 7
	_xALPHA			# 8
	\, [opt lookahead]	# 04/18/05 AM.
	_xWILD [one fail=(_xALPHA _noun _adj _aposS _aposX)]
	@@

# verb prep dqan alpha
# 04/18/07 AM. Restrict -- require prior noun.
@CHECK
  if (!N("noun",8))
    fail();
  L("n") = lasteltnode(7);
  if (plural(L("n")) || pnvar(L("n"),"mass"))
	  fail();
  if (plural(N(8)))
    fail();
@POST
  # Agreement.
  alphatonoun(8);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_prep [opt]
	_xWILD [s star match=(_det _proPoss _adv)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]	# 7	# 04/18/07 AM.
	_xALPHA			# 8
	@@


# verb by dqan
# vg by dqan
@CHECK
  if (N("pos50-v-by",2))
    fail();
@POST
  if (pnname(N(2)) == "_verb")
    fixverb(N(2),"passive","VBN");
  else
    fixvg(N(2),"passive","VBN");
  N("pos50-v-by",2) = 2;
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_xWILD [one match=(_verb _vg)]
	by [s lookahead]
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@


# verb dqan aposS
# prep dqan aposS
# verb dqan 's
# prep dqan 's
@CHECK
  if (N("mypos",6))
    fail();
@POST
  chpos(N(6),"POS");	# Assumed.
  L("tmp5") = lasteltnode(5);
  group(5,6,"_adj");
  pncopyvars(L("tmp5"),N(5));
  N("ne",5) = 0;
  clearpos(N(5),1,0);
  N("possessive",5) = 1;
  N("adj's",5) = 1;
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb _prep _fnword
#		_conj \,
		)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus gp=_nouns]
	_xWILD [one match=(_aposS _aposX)]
	@@


# prep num
@CHECK
  if (N("pos num",4) != 2)
    fail();
  if (!N("verb",4) || !N("adj",4))
    fail();
@POST
  group(2,2,"_np");  # Could be a date...
  N("bracket",2) = 1;
  L("tmp4") = N(4);
  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
  L("v") = N(4);
  group(4,4,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp4"),N(4));
  N("verb node",4) = L("v");
  fixvg(N(4),"active","VBD");
  clearpos(N(4),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xNUM
	\,
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _np)]
	@@

# adv alpha
@CHECK
  if (!N("adj",2))
    fail();
  if (N("verb",2)
   || N("adv",2))	# 02/19/05 AM.
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_adv
	_xALPHA
	@@

# alpha conj det
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
	_conj [lookahead]
	_xWILD [one match=(_det _noun _np)]
	@@

# Verb list
# Should check agreement of verbs.
# Should compose semantics for the list.  voice, pos, etc.
#@CHECK
#  S("arr") = verblistagree(N(1),N(7));
#  if (!S("arr"))
#    fail();
@POST
  group(2,7,"_vg");
  N("list",2) = 1;  # Treat as a list...
  # Should collect the list into a var.
@RULES
_xNIL <-
	_xWILD [one fail=(\, _verb )]
	_xWILD [one match=(_verb _vg)]
	\,
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_verb _vg \, )]
	_conj
	_xWILD [one match=(_verb _vg)]
	@@

# Partial list
# , alpha noun conj
@CHECK
  if (!N("adj",2) && !N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_xALPHA
	_noun
	_conj
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# Partial list
# noun alpha , dqan conj noun
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun)]
	_xALPHA
	\, [lookahead]
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun _np)]
	\, [opt]
	_conj
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# noun alpha noun
# noun last noun
# last month, this month etc.
# Could check N("sem",3) == "date" for corroboration.
@CHECK
  if (N("mypos",2))
    fail();	# Done.
@POST
  L("tmp2") = N(2);
  L("txt") = strtolower(N("$text",2));
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_noun)]
	_xWILD [one match=(last next)]  # "that", "this" ...
	_noun [lookahead]
	@@

# next noun
@POST
  L("tmp1") = N(1);
  group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	next
	_xWILD [one lookahead match=(_noun _num _quan _xNUM _adj)]
	@@


# verb conj alpha
@CHECK
  if (!N("verb",4))
    fail();
  if (pnname(N(1)) == "_vg")
    L("v") = N("verb node",1);
  else
    L("v") = N(1);
  S("vc") = samevconj(L("v"),N(4));
  if (!S("vc"))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
  N("inf",4) = N("-s",4) = N("-ed",4) = N("-edn",4) =
  	N("-ing",4) = N("-en",4) = 0;
  pnreplaceval(N(4),S("vc"),1);
  fixverb(N(4),0,0);
  # Fix up the first verb also, if needed...
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	\, [opt]
	_conj
	_xALPHA
	@@

# np , adv alpha
@CHECK
  if (N("pos num",5) != 2)
    fail();
  if (!N("verb",5) || !N("adj",5))
    fail();
  if (!vconjq(N(5),"-en"))
    fail();
@POST
  alphatovg(5,"passive","VBN");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS _prep _fnword)]
	_xWILD [one match=(_noun _np)]
	\,
	_adv
	_xALPHA
	@@

# alpha about num
@CHECK
  if (!N("verb",1))
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
  fixverb(N(1),"active",0);
@RULES
_xNIL <-
	_xALPHA
	_xWILD [s one lookahead match=(about circa approximately)]
	_xWILD [one match=(_quan _num _xNUM)]
	@@
_xNIL <-
	_xALPHA
	_xWILD [s one lookahead match=(about circa approximately)]
	_xWILD [s one match=(_money _percent)]
	@@

# verb yet to
@CHECK
  if (N("mypos",2))
    fail();	# Loop guard.
@POST
  pnrename(N(2),"_adv");
  chpos(N(2),"RB");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	yet [s]
	_xWILD [s one lookahead match=(to \, _xEND _dbldash _conj)]
	@@

# verb conj verb
# vg conj vg
# alpha conj verb
# alpha conj vg
@CHECK
  if (!N("verb",2))
    fail();
  # Check parallel construction.
  S("vc") = samevconj(N(2),N(4));
  if (S("vc"))
    succeed();
  if (N("pos num",2) != 2)
    fail();
  if (pnname(N(1)) != "_noun" || !N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (S("vc"))
    {
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  N("inf",2) = N("-s",2) = N("-ed",2) = N("-edn",2) =
  	N("-ing",2) = N("-en",2) = 0;
  pnreplaceval(N(2),S("vc"),1);
  fixverb(N(2),0,0);
  # Fix up the 2nd verb also, if needed...
    }
  else
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	}

@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _advl \,)]
	_xALPHA
	_xWILD [plus lookahead match=( \, _conj)]
	_xWILD [one match=(_verb _vg)]
	@@

# pro verb conj verb
@CHECK
  if (N("pos50 v-c-v",3))
    fail();
@POST
  N("pos50 v-c-v",3) = 1;
  fixverb(N(3),"active","VBP");
  fixverb(N(5),"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_pro)]
	_xWILD [star match=(_advl _adv)]
	_verb
	_conj
	_verb
	@@

# alpha num
# SENTINEL. Don't want num to start noun phrase if alpha
# in front of it.
# eg, "first 3 people...."
@POST
  noop();
@RULES
_xNIL <-
	_xALPHA
	_xWILD [plus match=(_quan _num _xNUM)]
	@@


# Need some context.
# dqan
@CHECK
  if (!N(2) && !N(3) && !N(4))
    fail();
  if (N(4))
    {
    L("last") = lasteltnode(4);
	if (pnvar(L("last"),"advl")	# 04/15/05 AM.
	 && N(4) != L("last"))
	  fail();
	}
  dqaninfo(1,2,3,4);
#  if (!numbersagree(S("first"),S("last")))
#    S("disagree") = 1;
@POST
  groupnp();
@RULES
_xNIL <-
	_det [plus]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA _conj \, _aposS _aposX
		_dbldash _fnword)]
	@@
_xNIL <-
	_det [star]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA _conj \, _aposS _aposX
		_dbldash _fnword)]
	@@
_xNIL <-
	_det [plus]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xEND
	@@

# dqan , alpha
@CHECK
  if (N("noun",7))
    fail();
  S("head") = lasteltnode(5);
  if (pnvar(S("head"),"sem") == "date"
    && (N(5) != S("head")))
    fail();
@POST
  dqaninfo(2,3,4,5);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _prep _xSTART _qEOS _advl)]
	_xWILD [opt match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM _adv)]
	_adj [star]
	_noun [plus]
	_xWILD [one match=( \, _conj _advl)]
	_xWILD [one match=(_xALPHA _det _pro _quan _num _xNUM
		_adj _verb _vg _np \, )]
	@@

# dqan
@CHECK
  S("head") = lasteltnode(5);
  if (pnvar(S("head"),"advl")	# 04/15/05 AM.
   && N(5) != S("head"))
    fail();
  if (pnvar(S("head"),"sem") == "date"
   && N(5) != S("head"))
    fail();
@POST
  dqaninfo(2,3,4,5);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _prep _xSTART _qEOS
		_advl \" _dblquote)]
	_xWILD [opt match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM _adv)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead match=(_prep _verb _vg _qEOS _xEND
#		\,	# 06/04/06 AM.
		)]
	@@

_xNIL <-
	_xWILD [s one match=(either)]
	_xWILD [opt match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM _adv)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_xALPHA)]
	@@

# Too weak.	# 02/02/05 AM.
# alpha np
# Inflected...
#@CHECK
#  if (!N("verb",1))
#    fail();
#  S("vc") = vconj(N(1));
#  if (S("vc") == "inf"
#   || S("vc") == "-s")
#    fail();
#@POST
#  L("tmp1") = N(1);
#  group(1,1,"_verb");
#  pncopyvars(L("tmp1"),N(1));
#@RULES
#_xNIL <-
#	_xALPHA
#	_xWILD [one lookahead match=(_np)]
#	@@

# verb that noun verb
# vg that np vg
@CHECK
  if (N("mypos",3))
    fail();
  if (N("voice",1) == "passive")
    fail();
@POST
 chpos(N(3),"IN");	# that/IN.
  if (pnname(N(1)) == "_vg")
    fixvg(N(1),"active","VBD");
  else
    fixverb(N(1),"active","VBD");
  N("pos50 verb-that",1) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	that [s except=(_det _np)]
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# that noun verb
@CHECK
  if (N("mypos",2))
    fail();
  if (N("mass",3) || plural(N(3)))
    fail();
  if (N("ne",3) || N("ne arr",3))
    fail();
@POST
  pnrename(N(2),"_det");
  chpos(N(2),"DT");	# that/DT
@RULES
_xNIL <-
	_xWILD [one fail=(_np _noun _adv _advl _xALPHA)]
	that [s except=(_det _np)]
	_noun
	_xWILD [one lookahead match=(_verb _vg)]
	@@
_xNIL <-
	_xSTART
	that [s except=(_det)]
	_noun
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# Checking for agreement to enhance this.
# prep that noun
# that
@CHECK
  if (N("mypos",2))
    fail();
  S("head") = lasteltnode(4);
  if (pnvar(S("head"),"mass"))
    fail();	# Don't know.
@POST
  if (plural(S("head")))
    {
	chpos(N(2),"IN");	# that/IN.
	dqaninfo(0,0,3,4);
	groupnp();
	}
  else
    {
    chpos(N(2),"DT");	# that/DT
    pnrename(N(2),"_det");
	}
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	that [s]
	_adj [star lookahead]
	_noun [plus]
	_xWILD [one fail=(_xALPHA _adj)]
	@@


# dqan alpha that dqan
# that
@CHECK
  if (!N("verb",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
  L("v") = N(4);
  group(4,4,"_vg");
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp4"),N(4));
  N("verb node",4) = L("v");
  clearpos(N(4),1,0);
  fixvg(N(4),0,0);
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj _noun _np)]
	_xWILD [star match=(_advl _adv)]
	\, [opt]
	_xALPHA
	that [s lookahead]
	_xWILD [one match=(_det _quan _num _xNUM _adj
		_noun _np _prep)]
	@@

# dqan alpha that verb
# that
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun)]
	_xALPHA
	that [s lookahead]
	_xWILD [one match=(_verb _vg)]
	@@

# alpha that verb
@CHECK
  if (N("pos50 a-that-v",1))
    fail();
@POST
  N("pos50 a-that-v",1) = 1;
  if (N("noun",1))
    fixnphead(1);
@RULES
_xNIL <-
	_xALPHA
	that [s lookahead]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# alpha that
# Note: try simplest first.
@CHECK
  if (!N("verb",1))
    fail();
  if (!verbfeat(N(1),"T5"))
    fail();
@POST
  alphatoverb(1,"active",0);
@RULES
_xNIL <-
	_xALPHA
	that [s lookahead]
	@@

# dqan outside noun
@POST
  pnrename(N(2),"_adj");
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	outside [s]
	_noun [lookahead]
	@@

# verb worth
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


# prep dqan value
@CHECK
  if (!N("money",3))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_det
	_noun
	value
	@@

# noun noun alpha
@CHECK
  if (!N("cap",2) && N("mypos",2) != "NP")
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);

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
	_xWILD [one match=(_noun)]
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# adj alpha alpha dqan
@CHECK
  if (!N("noun",2))
    fail();
  if (!N("verb",4))
    fail();
  if (N("noun",4))
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp4") = N(4);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
  L("v") = N(4);
  L("neg") = mhbvadv(3,0,0,0);
  group(4,4,"_vg");
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp4"),N(4));
  N("verb node",4) = L("v");
  clearpos(N(4),1,0);
  fixvg(N(4),"active","VBP");
@RULES
_xNIL <-
	_adj
	_xALPHA
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# verb alpha dqan
@CHECK
  if (!N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xWILD [one lookahead match=(_np _det _pro _quan _num _xNUM)]
	@@

# modal alpha
@CHECK
  if (!N("verb",3))
    fail();
@POST
  alphatoverb(3,"active","VB");
@RULES
_xNIL <-
	_modal
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# det noun of noun
# dqan noun of noun
@POST
  L("number") = numbersagree(N(1),N(2));
  L("tmp2") = N(2);
  L("tmp4") = N(4);
  group(4,4,"_np");
  pncopyvars(L("tmp4"),N(4));
  clearpos(N(4),1,1);
  group(1,2,"_np");
  pncopyvars(L("tmp2"),N(1));
  clearpos(N(1),1,1);
  group(1,3,"_np");
  pncopyvars(L("tmp4"),N(1));
  clearpos(N(1),1,0);
  if (L("number"))
    N("number",1) = L("number");
@RULES
_xNIL <-
	_det
	_noun
	of [s]
	_noun
	_xWILD [one lookahead match=(_fnword)]
	@@

# det num alpha
@CHECK
  if (N("noun",3))
    fail();
  if (!N("verb",3))
    fail();
  # Check n-v number agreement etc.
@POST
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  chpos(N(2),"NN");
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  group(1,2,"_np");
  pncopyvars(L("tmp2"),N(1));
  N("bracket",1) = 1;
@RULES
_xNIL <-
	_xWILD [star match=(_det)]
	_num
	_xALPHA
	_xWILD [one lookahead match=(_prep _np _fnword)]
	@@

# dqan alpha
# num alpha [NIBBLE]
@CHECK
  if (N("pos50 num-a",3))
    fail();
@POST
  N("pos50 num-a",3) = 1;
  if (N("noun",3))
    {
	if (plural(N(3)))
	  {
	  fixnphead(3);
	  L("done") = 1;
	  }
	}
  if (!L("done") && N("verb",3))
    {
	if (!vconjq(N(3),"-s"))
	  alphatoverb(3,"active","VBD");
	}
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xALPHA
	_xWILD [one fail=(_adj _noun _xALPHA)]
	@@
# dqan alpha
# num alpha [NIBBLE]
@CHECK
  if (N("pos50 num-a",2))
    fail();
@POST
  N("pos50 num-a",2) = 1;
  if (N("noun",2))
    {
	if (plural(N(2)))
	  {
	  fixnphead(2);
	  L("done") = 1;
	  }
	else if (singular(N(2))
	 && (N("num",1) == 1 || N("number",1) == "singular") )
	   {
	   fixnphead(2);
	   L("done") = 1;
	   }
	}
  if (!L("done") && N("verb",2))
    {
	if (!vconjq(N(2),"-s"))
	  alphatoverb(2,"active","VBD");
	}
@RULES
_xNIL <-
	_xWILD [plus match=(_quan _num _xNUM)]
	_xALPHA
	_xWILD [one fail=(_adj _noun _xALPHA)]
	@@

# Moose said
# alpha vg
# alpha verb
@CHECK
  if (!N("noun",2))
    fail();
  L("sem") = nodesem(N(4));
  if (!finddictattr(L("sem"),"agent-human"))
    fail();
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
	_xWILD [one match=(_xSTART _qEOS)]
	_xCAP
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_verb _vg)]
	@@

# adj and adj
@POST
  group(2,4,"_adj");
  N("adjs",2) = 1;
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro _quan _num _xNUM _advl)]
	_adj
	_conj
	_adj
	_xWILD [one lookahead match=(_noun _verb _vg _advl _adv)]
	@@

########################
# Some mhbv into here...
########################
@CHECK
  if (!N("verb",7))
    fail();
  # Agreement...
  L("arr") = vgagree(0,N(1),N(3),N(5),N(7));
  if (!L("arr")[0])
    fail();
@POST
  L("tmp7") = N(7);
  if (pnname(N(7)) != "_verb")
    {
    group(7,7,"_verb");
    pncopyvars(L("tmp7"),N(7));
	}
 L("m") = 0;
 L("h") = N(1);
 L("b") = N(3);
 L("being") = N(5);
 L("v") = N(7);
 L("neg") = mhbvadv(0,2,4,6);

 if (N(5))
   {
    N("sem",5) = N("stem",5) = "be";
    chpos(N(5),"VBG");
   }

 group(1,7,"_vg");
 pncopyvars(L("v"),N(1));
 clearpos(N(1),1,0);
 mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_have [s]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_xALPHA _verb)]
	@@

# past
#@CHECK
#  if (pnname(N(2)) == "_adj")
#    fail();	# Infinite loop guard.
@POST
#  if (literal(N(2)))
#    {
	L("tmp2") = N(2);
    group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
#	}
#  else
#    pnrename(N(2),"_adj");
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	past
	_xWILD [one lookahead match=(_quan _num _xNUM _adj _noun)]
	@@

# ^ prep dqan alpha
# ^ prep noun alpha
@CHECK
  if (N("pos num",6) != 2)
    fail();
  if (!N("noun",6) || !N("verb",6))
    fail();
  S("noun") = lasteltnode(5);
  if (!singular(S("noun")))
    fail();
  S("vc6") = vconj(N(6));
  if (S("vc6") == "-ing"
   || S("vc6") == "-edn"
   || S("vc6") == "-en")
    fail();
@POST
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
  fixnoun(N(6));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_prep
	_det [star]
	_adj [star]
	_noun [plus]
	_xALPHA
	@@

# ^ prep dqan alpha
@CHECK
  if (!N("noun",4) || !N("verb",4))
    fail();
  S("noun") = lasteltnode(3);
  if (!singular(S("noun")))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
#  fixnoun(N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_prep
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	@@

# noun alpha
# noun date
@CHECK
  if (!N("advl",2))	# 04/15/05 AM.
    fail();
  if (!N("noun",2) && !N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"NN");
@RULES
_xNIL <-
	_noun
	_xALPHA
	@@

# noun subject to
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	subject
	to [s lookahead]
	@@


# plus alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (!singular(N(2)))
    fail();
@POST
  # Note mass-noun here...
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	plus [s]
	_xALPHA
	_xWILD [one lookahead match=(_xEND _qEOS \, )]
	@@

# plus
# By and large a CC in the corpus, if no other evidence.
@POST
  pncopyvars(1);
  S("mypos") = "CC";
  single();
@RULES
_conj <-
	plus
	@@

# alpha noun
# alpha dqan
@CHECK
  if (!N("adj",2))
    fail();
  if (!singular(N(2)))
    fail();
  if (N("mass",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xALPHA
	_noun [lookahead]
	@@

# noun alpha adj
@CHECK
  if (N("adj",2))
    fail();
  if (!N("verb",2))
    fail();
  if (vconjq(N(2),"inf"))
    fail();  # Want inflected.
@POST
  alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun [plus]
	_xALPHA
	_adj [lookahead]
	_xWILD [one match=(_adj _noun)]
	@@

# verb adj conj verb noun
# Conflicted ving + ving parallel construction.
@CHECK
  L("txt2") = nodetext(N(2));
  L("txt4") = nodetext(N(4));
  if (strendswith(L("txt2"),"ing")
   && strendswith(L("txt4"),"ing") )
    succeed();
  fail();
@POST
  pnrename(N(4),"_adj");
  fixadj(N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_adj
	_conj
	_verb
	_xWILD [one lookahead match=(_noun _adj)]
	@@

# adj conj adj
@POST
 L("tmp4") = N(4);
 group(2,4,"_adj");
 pncopyvars(L("tmp4"),N(2));
 clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_adj _adjc)]
	_conj
	_adj
	_xWILD [one lookahead match=(_adj _noun)]
	@@

# Some alphas centered around a comma.
# dqan alpha , alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (!N("verb",4))
    fail();
  S("vc4") = vconj(N(4));
  if (S("vc4") != "-edn"
   && S("vc4") != "-en")
    fail();
@POST
  L("tmp2") = N(2);
  L("tmp4") = N(4);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  group(4,4,"_verb");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro _quan _num _xNUM _adj)]
	_xALPHA
	\,
	_xALPHA
	_xWILD [one lookahead match=(_prep _advl _adv)]
	@@

# alpha alpha prep
# alpha alpha adv
@CHECK
  if (!N("noun",1))
    fail();
  if (!N("verb",2))
    fail();
  S("vc") = vconj(N(2));
  if (S("vc") != "-edn"
   && S("vc") != "-en"
   && S("vc") != "-ed")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [star match=(_advl _adv)]
	_xWILD [one lookahead match=(_prep)]
	@@


# quasi-question forms.
# modal np have be being verb
# Not only will John eat....
# ...
@CHECK
  if (!N(5) && !N(7) && !N(9) && !N(11))
    fail();
  L("arr") = vgagree(N(1),N(5),N(7),N(9),N(11));
  if (!L("arr")[0])
    fail();
@POST
  L("m") = N(1);
  L("h") = N(5);
  L("b") = N(7);
  L("being") = N(9);
  L("v") = N(11);

 if (N(9))
   {
    N("sem",9) = N("stem",9) = "be";
    chpos(N(9),"VBG");
   }

   L("neg") = mhbvadv(2,6,8,10);
  group(5,11,"_vg");
  mhbv(N(5),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
  N("q-q remote",5) = "left";
  if (pnname(N(1)) != "_vg")
    {
	if (N("stem",1) == "do")
	  fixverb(N(1),"active","VBP");
	group(1,1,"_vg");
	N("q-q remote",1) = "right";
	N("modal",1) = 1;
	}
@RULES
_xNIL <-
	_xWILD [s one match=(_modal _do)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [star match=(_advl _adv)]
	_have [s opt]
	_xWILD [star match=(_advl _adv)]
	_be [s opt]
	_xWILD [star match=(_advl _adv)]
	being [s opt]
	_xWILD [star match=(_advl _adv)]
	_verb [opt]
	@@

# 03/10/05 AM.
@CHECK
  if (pnname(N(1)) == "_adv" && !N(2))
    fail();
  # Agreement...
  L("arr") = vgagree(N(2),N(4),N(6),N(8),N(10));
  if (!L("arr")[0])
    fail();
@POST
 L("neg") = mhbvadv(3,5,7,9);
 L("m") = N(2);
 L("h") = N(4);
 L("b") = N(6);
 L("being") = N(8);
 L("v") = N(10);
 L("glom") = N("glom",10);	# ..and other semantics.
 # Should be a more compact way to do the below...
 # At least group could return the grouped node.
 L("neg") = mhbvadv(3,5,7,9);

 if (N(8))
   {
    N("sem",8) = N("stem",8) = "be";
    chpos(N(8),"VBG");
   }
 
 if (N(2))
   {
   group(2,10,"_vg");
   mhbv(N(2),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   if (L("glom"))
     N("glom",2) = L("glom");
   }
 else if (N(4))
   {
   group(4,10,"_vg");
   mhbv(N(4),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   if (L("glom"))
     N("glom",4) = L("glom");
   }
 else if (N(6))
   {
   group(6,10,"_vg");
   mhbv(N(6),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
   if (L("glom"))
     N("glom",6) = L("glom");
   }
 else if (N(8))
   {
   group(8,10,"_vg");
   mhbv(N(8),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
    if (L("glom"))
     N("glom",8) = L("glom");
  }
 else
   {
   group(10,10,"_vg");
   mhbv(N(10),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
    if (L("glom"))
     N("glom",10) = L("glom");
  }
 
@RULES
_xNIL <-
	_xWILD [one fail=(_verb _modal _xALPHA)]
	_xWILD [s opt match=(_modal _do)]
	_xWILD [star match=(_adv _advl)]
	_have [s opt]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(_verb) except=(_modal _have _be _vg)]
	@@
_xNIL <-
	_xWILD [one fail=(_verb _modal _xALPHA)]
	_xWILD [s opt match=(_modal _do)]
	_xWILD [star match=(_adv _advl)]
	_have [s]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(_verb) except=(_be _vg)]
	@@

# quasi-question forms.
# have np be being verb
# Not only has John eaten....
# Not only has John been eating...
@CHECK
  if (!N(5) && !N(7) && !N(9))
    fail();
  L("arr") = vgagree(0,N(1),N(5),N(7),N(9));
  if (!L("arr")[0])
    fail();
@POST
  L("h") = N(1);
  L("b") = N(5);
  L("being") = N(7);
  L("v") = N(9);

  if (N(7))
    {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
    }

  L("neg") = mhbvadv(2,4,6,8);
  group(5,9,"_vg");
  mhbv(N(5),L("neg"),0,L("h"),L("b"),L("being"),L("v"));
  N("q-q remote",5) = "left";
  if (pnname(N(1)) == "_verb")
    {
	group(1,1,"_vg");
	N("q-q remote",1) = "right";
	N("have",1) = 1;
	}
@RULES
_xNIL <-
	_have [s]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [star match=(_advl _adv)]
	_be [s opt]
	_xWILD [star match=(_advl _adv)]
	being [s opt]
	_xWILD [star match=(_advl _adv)]
	_verb [opt]
	@@

# Trap to avoid quasi-question forms.
@POST
  if (pnname(N(3)) != "_vg")
    {
	L("b") = N(3);
    L("neg") = mhbvadv(2,4,0,0);
	group(3,3,"_vg");
    mhbv(N(3),L("neg"),0,0,L("b"),0,0);
	}
  noop();
@RULES
_xNIL <-
	_xWILD [s one match=(there _pro)]
	_xWILD [star match=(_advl _adv)]
	_be [s]
	_xWILD [star lookahead match=(_advl _adv)]
	_xWILD [one match=(_noun _np)]
	@@

# question/query form.
# whword be noun verb
# whword be np mhbv
@CHECK
  if (!N(7) && !N(9))
    fail();
  L("arr") = vgagree(0,0,N(3),N(7),N(9));
  if (!L("arr")[0])
    fail();
  if (N("q-q remote",3))
    fail();
@POST
  L("b") = N(3);
  L("being") = N(7);
  L("v") = N(9);

  if (N(7))
    {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
    }

  L("neg") = mhbvadv(3,5,7,9);
  group(7,9,"_vg");
  mhbv(N(7),L("neg"),0,0,L("b"),L("being"),L("v"));
  N("q-q remote",7) = "left";
  if (pnname(N(3)) == "_verb")
    {
	group(3,3,"_vg");
	N("be",3) = 1;
	}
  N("q-q remote",3) = "right";
@RULES
_xNIL <-
	_whword [s]
	_xWILD [star match=(_advl _adv)]
	_be [s]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [star match=(_advl _adv)]
	being [s opt]
	_xWILD [star match=(_advl _adv)]
	_verb [opt]
	@@

# quasi-question forms.
# be np being verb
# Not only is John eating...
@CHECK
  if (!N(5) && !N(7))
    fail();
  L("arr") = vgagree(0,0,N(1),N(5),N(7));
  if (!L("arr")[0])
    fail();
@POST
  L("b") = N(1);
  L("being") = N(5);
  L("v") = N(7);

  if (N(5))
    {
    N("sem",5) = N("stem",5) = "be";
    chpos(N(5),"VBG");
    }

  L("neg") = mhbvadv(2,4,6,0);
  group(5,7,"_vg");
  mhbv(N(5),L("neg"),0,0,L("b"),L("being"),L("v"));
  N("q-q remote",5) = "left";
  if (pnname(N(1)) == "_verb")
    {
	group(1,1,"_vg");
	N("q-q remote",1) = "right";
	N("be",1) = 1;
	}
@RULES
_xNIL <-
	_be [s]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [star match=(_advl _adv)]
	being [s opt]
	_xWILD [star match=(_advl _adv)]
	_verb [opt]
	@@



# Not only will John have been eating.



# so
# Unless we know something, tag it as adverb.
@CHECK
 if (N("mypos"))
   fail();
@POST
  chpos(N(1),"RB");
@RULES
_xNIL <-
	so [s]
	@@

# adj -- conj adj --
@POST
  group(1,5,"_adj");
@RULES
_xNIL <-
	_adj
	_dbldash
	_conj
	_adj
	_dbldash
	@@

# prep alpha alpha prep
# Assuming all cases of this handled here....
@CHECK
  if (N("paap",1))
    fail();	# Loop guard.
@POST
  N("paap",1) = 1;
  L("tmp2") = N(2);
  L("tmp3") = N(3);
  L("txt2") = N("$text",2);
  # First alpha first...
  if (finddictattr(L("txt2"),"prefer-verb"))
  	{
	group(2,2,"_verb");
	pncopyvars(L("tmp2"),N(2));
	N("paap",1) = 1;
	}
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# clausal form
# vg np prep alpha alpha
@CHECK
  if (!N("noun",6))
    fail();
  if (N("verb",6))
    {
	S("vc6") = vconj(N(6));
	if (S("vc6") != "inf"
	 && S("vc6") != "-s")
	  fail();
	}
@POST
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _noun)]
	_prep
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_fnword _prep)]
	@@

	
# clausal fragment
# verb noun prep alpha noun verb
@CHECK
  if (N("stem",3) == "to")
    fail();
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	_prep
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	@@

# np np vg
@CHECK
  if (N("pos50 n-n-v",3))
    fail();	# Loop guard.
  if (N("eos",1))
    fail();
@POST
  if (!N("voice",3) || N("voice",3) == "active")
    fixvg(N(3),"active","VBP");
  N("pos50 n-n-v",3) = 1;
@RULES
_xNIL <-
	_np
	_np
	_vg
	@@


# do verb
@CHECK
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  L("neg") = mhbvadv(2,0,0,0);
  group(1,3,"_vg");
  mhbv(N(1),L("neg"),L("tmp1"),0,0,0,L("tmp3"));
  pncopyvars(L("tmp3"),N(1));
  clearpos(N(1),1,0);
  N("voice",1) = "active";
  N("verb node",1) = L("tmp3");
  N("first verb",1) = L("tmp1");
  fixvg(N(1),"active","VBP");
  fixverb(L("tmp3"),"active","VB");
@RULES
_xNIL <-
	_do [s]
	_xWILD [star match=(_adv _advl)]
	_verb
	@@

# do alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  L("neg") = mhbvadv(2,0,0,0);
  group(1,3,"_vg");
  mhbv(N(1),L("neg"),L("tmp1"),0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(1));
  clearpos(N(1),1,0);
  N("voice",1) = "active";
  N("verb node",1) = L("v");
  N("first verb",1) = L("tmp1");
  fixvg(N(1),"active","VBP");
  fixverb(L("v"),"active","VB");
@RULES
_xNIL <-
	_do [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# beginning
@CHECK
  if (!N("verb",1) || !N("noun",1))
    fail();
  if (N("adv",1))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_prep");
  pncopyvars(L("tmp2"),N(2));
  chpos(N(2),"VBG"); # Conformity.
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one match=(beginning)]
	@@

# clausal pattern.
# verb np prep alpha
@CHECK
  if (N("stem",5) == "to")
    fail();
  if (!N("noun",6))
    fail();
@POST
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_np
	_xWILD [star match=(_advl _adv)]
	_prep
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# noun either verb
@CHECK
  if (N("mypos",2))
    fail();  # Loop guard etc.
@POST
  if (!N("mypos",2))
    chpos(N(2),"RB"); # Conformity.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [s one match=(either neither)]
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# clause dqan alpha dqan
@CHECK
  if (N(2))
    {
    if (N("pattern",2) != "advl")
      fail();
	}
  if (!N("verb",7))
    fail();
@POST
  L("tmp7") = N(7);
  group(7,7,"_verb");
  pncopyvars(L("tmp7"),N(7));
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_clause [opt]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_det _pro _quan _num _xNUM
		_adj)]
	@@

# verb alpha prep
# verb forward prep
# Certain adverbs....
@CHECK
  if (nonliteral(N(2)))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [s one match=(forward forwards
		backward backwards ahead)]
	@@

# noun cap
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  N("cap",2) = 1;
  if (N("eos",1))
    {
	group(2,2,"_np");
	pncopyvars(L("tmp2"),N(2));
	chpos(N(2),"NP");
	clearpos(N(2),1,1);
	}
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xCAP
	_xWILD [one lookahead match=(_prep)]
	@@

# noun alpha pro
@CHECK
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_np
	_xALPHA
	_pro [lookahead]
	@@

# verb
@POST
  L("tmp2") = N(2);
  group(2,2,"_vg");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,0);
  N("verb node",2) = N("first verb",2) = L("tmp2");
  mhbv(N(2),L("neg"),0,0,0,0,L("tmp2"));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _pro _prep)]
	_verb
	_xWILD [one lookahead match=(_noun _np _pro _prep)]
	@@

# verb prep noun verb
@CHECK
  if (N("voice",1) != "active")
    fail();
  if (N("voice",4))
    fail();
@POST
  fixvg(N(4),"passive","VBN");
@RULES
_xNIL <-
	_vg
	_prep
	_np
	_vg
	_xWILD [one fail=(_det _pro _quan _num _xNUM _adj _noun _np _fnword)]
	@@

# adj alpha.
@CHECK
  if (!N("possessive",1))
	fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("verb",2))
    fail();	# Loop guard.

  S("vc2") = vconj(N(2));
  if (S("vc2") != "inf"
   && S("vc2") != "-s"
   && S("vc2") != "-ed")
    fail();
@POST
  N("verb",2) = 0;
  L("tmp2") = N(2);
  L("num") = N("pos num",2);
  pnreplaceval(N(2),"pos num",--L("num"));
  if (L("num") == 1)
    {
	# Figure out pos and do reduction.
	if (N("adj",2))
	  group(2,2,"_adj");
	else if (N("noun",2))
	  group(2,2,"_noun");
	else if (N("adv",2))
	  group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	if (pnname(N(2)) == "_adj")
	  fixadj(N(2));
	}
@RULES
_xNIL <-
	_adj
	_xALPHA
	@@

# verb adj alpha alpha
@CHECK
  if (!N("adj",3) && !N("noun",3))
    fail();
  if (!N("adj",4) && !N("noun",4))
    fail();
  if (N("adv",4))
    fail();
  if (N("verb",3))
    fail();	# For now.
  if (N("verb",4))
    {
    if (!vconjq(N(4),"inf"))
	  fail();
	}
@POST
  fixnpnonhead(3);
  if (N("verb",4))
    {
    N("verb",4) = 0;
	L("num") = N("pos num",4);
	pnreplaceval(N(4),"pos num",--L("num"));
	if (L("num") == 1)	# Unambiguous.
	  {
	  L("tmp4") = N(4);
	# Figure out pos and do reduction.
	if (N("adj",4))
	  group(4,4,"_adj");
	else if (N("noun",4))
	  group(4,4,"_noun");
	else if (N("adv",4))
	  group(4,4,"_adv");
	pncopyvars(L("tmp4"),N(4));
	if (pnname(N(4)) == "_adj")
	  fixadj(N(4));
	  }
	}
@RULES
_xNIL <-
	_verb
	_adj
	_xALPHA
	_xALPHA
	@@

# alpha adv alpha
@CHECK
  if (N("verb",1))
    fail();
  if (!N("noun",1))
    fail();
  if (!N("verb",3))
    fail();
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  group(1,1,"_noun");
  pncopyvars(L("tmp1"),N(1));
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xALPHA
	_adv
	_xALPHA
	_xWILD [one match=(_prep _fnword)]
	@@

# noun , alpha
@CHECK
  if (!N("noun",5))
    fail();
  if (N("verb",5))
    {
	if (!vconjq(N(5),"inf") && !N("inf",5))
      fail();
	}
  if (!N("cap",3) && N("mypos",3) != "NP")
    fail();
  if (!N("glom comma",3))
    fail();
@POST
  excise(4,4);
@RULES
_xNIL <-
	_xWILD [one match=(_prep _det _pro)]
	_xWILD [star match=(_quan _num _xNUM _adj)]
	_noun
	\,
	_xALPHA [lookahead]
	@@

# noun , alpha conj adj
@CHECK
  if (!N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	\, [opt lookahead]
	_conj
	_xWILD [one match=(_adj _noun _np
	_adv _advl	# 04/21/07 AM.
	)]
	@@

# , noun verb alpha dqan
@CHECK
  if (!N("adj",4) || !N("verb",4))
    fail();
  if (N("noun",4))
    fail();	# Could handle this here also...
  S("vc4") = vconj(N(4));
  if (S("vc4") != "-edn"
   && S("vc4") != "-en")
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_adj");
  pncopyvars(L("tmp4"),N(4));
  fixadj(N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_qEOS _xSTART \, )]
	_xWILD [one match=(_noun _np)]
	_xWILD [s one match=(_verb _vg) except=(_modal _have _be
		being)]
	_xALPHA
	_xWILD [one lookahead match=(_adj _noun _xALPHA)]
	@@

# noun alpha noun verb
# dqan alpha noun verb
@CHECK
  if (!N("noun",6))
    fail();
  if (N(2))
    S("start") = N(2);
  else if (N(3))
    S("start") = N(3);
  else if (N(4))
    S("start") = N(4);
  else
    S("start") = N(5);
  if (!numbersagree(S("start"),N(6)))
    fail();
@POST
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
@RULES
_xNIL <-
	_xWILD [one match=(_qEOS _xSTART \" _dblquote
		_fnword	# Trying.	# 06/13/06 AM.
		 )]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_noun [plus lookahead]
	_xWILD [one match=(_verb _vg)]
	@@

# noun conj alpha conj
@CHECK
  if (!N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_noun
	_conj
	_xALPHA
	\, [opt]
	_xWILD [one lookahead match=(_conj)]
	@@

# prep alpha conj vg
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-ing"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  N("first verb",2) = N("verb node",2) = L("v");
  pncopyvars(L("tmp2"),N(2));
  fixvg(N(2),"active","VBG");
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xALPHA
	_conj [lookahead]
	_xWILD [one match=(_verb _vg)]
	@@

# num or num
@POST
  L("tmp4") = N(4);
  group(2,4,"_num");
  N("list",2) = 1;
  pncopyvars(L("tmp4"),N(2));
@RULES
_xNIL <-
	_xWILD [one fail=( \, )]
	_num
	or [s]
	_num
	@@

# dqan
# det cap conj cap alpha
@CHECK
  if (!N("cap",2) || !N("cap",4))
    fail();
  if (N("cap",5))
    fail();
  if (!N("noun",5))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
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
	_xALPHA
	@@

# dqan alpha
# dqa alpha
# NIBBLE.
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (N("pos50 adj-a-a",4))
    fail();
@POST
  N("pos50 adj-a-a",4) = 1;
  if (N("verb",4))
    {
	--N("pos num",4);
	N("verb",4) = 0;
	}

  if (N("pos num",4) == 1)
    {
	if (N("noun",4))
	  alphatonoun(4);
	else
	  alphatoadj(4);
	}
  else if (N("verb",5))
    fixnphead(4);	# MAYBE.
  else
    fixnpnonhead(4);	# adj-noun thingy.
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_adv [star]
	_adj [plus]
	_xALPHA
	_xALPHA
	@@

# Too broad. Too old.
# dqan alpha
# dqan cap alpha
@CHECK
  if (!N("cap",4))
    fail();
  if (N("cap",5))
    fail();
  if (!N("noun",5))
    fail();
  if (N("verb",5))
  	fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
@RULES
_xNIL <-
	_xWILD [one match=(_det)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun
	_xALPHA
	@@

# dqan alpha prep
@CHECK
  if (N("verb",2))
    fail();
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_adj [plus]
	_xALPHA
	_xWILD [one lookahead match=(_prep _conj _xEND)]
	@@

# ^ dqan alpha prep
@CHECK
  if (!N("noun",6))
    fail();
  # Agreement...
  if (vconjq(N(6),"-ing"))
    fail();
  L("last") = lasteltnode(5);
  if (singular(L("last")) && singular(N(6)))
    succeed();
  fail();
@POST
  fixnphead(6);
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

# prep noun
# prep cap
@CHECK
  if (!N("cap",2))
    fail();
  if (N("date",2))
    fail();
@POST
  N("date",2) = 1;
  N("sem",2) = "date";
  N("eventive",2) = 1;  # Some kind of historic event...
  if (pnname(N(1)) == "_fnword")
    pnrename(N(1),"_prep");
@RULES
_xNIL <-
	_xWILD [s one match=(since until till after before during)]
	_noun
	_xWILD [one lookahead match=( _qEOS _xPUNCT )]
	@@

# Ellipted temporal.
# days before
# noun noun prep
@CHECK
 if (N("cap",1))
   fail();
 if (N("sem",2) != "date")
   fail();
@POST
  group(2,2,"_np");
  N("sem",2) = "date";
  if (pnname(N(3)) == "_fnword")
    pnrename(N(3),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_noun
	_xWILD [s one lookahead match=(before after)]
	@@

# noun alpha prep as
@CHECK
  if (!N("verb",2))
    fail();
  # Agreement.
  if (plural(N(1)) && singular(N(2)))
    succeed();
  if (plural(N(2)) && singular(N(1)))
    succeed();
  fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_prep [lookahead]
	_fnword
	@@

# np of dqan alpha prep
@CHECK
  if (!N("verb",7))
    fail();
  if (N("noun",7)) # 09/09/05 AM.
    fail();
@POST
  alphatoverb(7,0,0);
@RULES
_xNIL <-
	_xWILD [one match=(_np _noun)]
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_xNUM _num _quan)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb alpha verb
@CHECK
  if (!N("adv",2))
    fail();
  S("stem1") = N("stem",1);
  if (S("stem1") != "be"
   && S("stem1") != "being"
   && S("stem1") != "have")
    fail();
  if (!singular(N(2)))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_verb
	_xALPHA
	_verb
	@@

# noun noun
# Some things that don't glom.
@CHECK
  if (N("sem",2) != "date")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
  group(2,2,"_advl");	# Date adverbial...
@RULES
_xNIL <-
	_noun
	_noun
	@@

# prep conj prep
@POST
  if (N("list",2))
    listadd(2,4,"true");
  else
    {
    group(2,4,"_prep");
	clearpos(N(2),1,0);
    N("list",2) = 1;
    }
@RULES
_xNIL <-
	_xWILD [one fail=( \, )]
	_prep
	_conj
	_prep
	@@

# ^ adj alpha noun
# dqan alpha noun
# dqa alpha noun
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
@POST
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xSTART
	_adj [plus]
	_xALPHA
	_noun [lookahead]
	@@

# adj alpha noun
# dqan alpha noun
@CHECK
  if (!N("adj",2) || !N("noun",2))
    fail();
  if (N("adv",2) || N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_noun [lookahead]
	@@

# adj " noun "
# Period and quote should be inverted ahead of time...
@POST
  if (N(5))
    {
    group(6,6,"_qEOS");	# Convert to an end of sentence!
	group(2,5,"_np");
	}
  else
    group(2,6,"_np");
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_adj
	_xWILD [one match=( \" _dblquote)]
	_noun
	_qEOS [opt]
	_xWILD [one match=( \" _dblquote)]
	@@

# to alpha noun
# prep alpha noun
@CHECK
  if (!N("verb",3))
   fail();
  if (!vconjq(N(3),"inf"))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  L("v") = N(3);
  pncopyvars(L("tmp3"),N(3));
  fixverb(N(3),"active","VB");
  L("neg") = mhbvadv(2,0,0,0);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("voice",3) = "active";
  clearpos(N(3),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [s one match=(to)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_noun _np _pro _det _prep _conj
		_fnword _whword \, )]
	@@

# vg to vg
# wants to eat
# Semantically simpler to deal with...
@CHECK
  if (N("voice",1) != "active")
    fail();
@POST
 L("tmp1") = N(1);
 L("tmp5") = N(5);
 L("v") = N("first verb",5);
 if (!L("v"))
   L("v") = N("verb node",5);
 if (L("v") && !pnvar(L("v"),"mypos"))
   {
   chpos(L("v"),"VB"); # infinitive.
   pnreplaceval(L("v"),"inf",1);
   }

 # If ambiguous...
 fixvg(N(1),"active","VBD");

 group(1,5,"_vg");
 pncopyvars(L("tmp5"),N(1));
 N("compound-vg",1) = 1;
 N("first vg",1) = L("tmp1");
 N("last vg",1) =  L("tmp5");
 N("pattern",1) = "vg-to-vg";
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	to [s]
	_xWILD [star match=(_advl _adv)]
	_vg
	@@

# verb alpha prep
@CHECK
  L("txt2") = strtolower(N("$text",2));
  if (!finddictattr(L("txt2"),"prefer-adj"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# np prep noun alpha
@CHECK
  if (!singular(N(2)))
    fail();
  if (!singular(N(5)))
    fail();
  if (!N("adj",5))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_adj");
  pncopyvars(L("tmp5"),N(5));
  fixadj(N(5));
@RULES
_xNIL <-
	_xSTART
	_np
	for [s]
	_noun
	_xALPHA
	_adv [opt lookahead]
	to [s]
	@@

# both alpha and noun
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (N("cap",2))
    chpos(N(2),"NP");
  else
    chpos(N(2),"NN");
@RULES
_xNIL <-
	both [s]
	_xALPHA
	_conj [lookahead]
	_xWILD [one match=(_noun _np)]
	@@

# verb noun alpha to
# Note: verb (that) noun verb
@CHECK
  if (!N("verb",3))
    fail();
  if (!verbfeat(N(1),"T5"))
    fail();
@POST
  alphatoverb(3,"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_noun
	_xALPHA
	to [s lookahead]
	@@

# clausal.
# vg np , vg prep
# vg that np, vg prep
# verb noun , verb prep
# ex: said (that) the man, eaten with envy...
# note: form like: vg [that] np
# that
@CHECK
  S("v5") = vgambigedn(N(5));	# 06/13/06 AM.
  if (!S("v5"))	# 06/13/06 AM.
    fail();
@POST
  pnreplaceval(S("v5"),"-en",1);
  pnreplaceval(S("v5"),"-edn",0);
  chpos(S("v5"),"VBN");
  if (!N("voice",5))
    N("voice",5) = "passive";
@RULES
_xNIL <-
	_vg
	that [s opt lookahead]
	_np
	\,
	_vg
	_xWILD [one match=(_prep)]
	@@

@CHECK
  if (!N(2) && !N(3) && !N(4))
    fail();
  dqaninfo(2,3,4,5);
  if (!numbersagree(S("first"),S("last")))
    fail();	# 06/15/06 AM.
@POST
  if (N(3))
  if (!nonliteral(N(3)))
    {
	L("tmp3") = N(3);
	group(3,3,"_num");
	pncopyvars(L("tmp3"),N(3));
	if (S("first") == L("tmp3"))	# 04/23/07 AM.
	   S("first") = N(3);
	}
  groupnp();
@RULES
_xNIL <-
	_xALPHA
	_xWILD [s plus match=(_det _pro) except=(_proObj)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS _aposX)]
	@@
_xNIL <-
	_xALPHA
	_xWILD [s star match=(_det _pro) except=(_proObj)]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS _aposX)]
	@@
_xNIL <-
	_xWILD [one fail=(_xALPHA _det _quan _num _xNUM
		_adj _noun _aposS _aposX)]
	_xWILD [s star match=(_det _proPoss)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS _aposX)]
	@@

# her
@CHECK
  if (N("mypos",1))
    fail();
@POST
  chpos(N(1),"PP$");
@RULES
_xNIL <-
	her [s]
	_xWILD [one lookahead match=(_adj _noun)]
	@@

# Try the broadest possible.
# verb alpha
@CHECK
  if (!N("adj",3))
    fail();
  S("stem") = nodestem(N(3));
  S("core") = finddictsattr(S("stem"),"core");
  if (!copulaq(N(1)) && S("core") != "adj")
    fail();
  if (N("pos_np_head",3) == "NN")
    fail();	# Wants to be a noun...
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# verb one 's
# prep one 's
# note: ignoring alternative for now.
# alternative eg: thinking (that) one's hot stuff.
@CHECK
  if (N("mypos",2))
    fail();
@POST
  pnrename(N(2),"_noun");
  chpos(N(2),"NN");	# one/NN (Conform Treebank. I prefer PP.)
  chpos(N(3),"POS");	# 's/POS
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _prep _fnword)]
	one [s]
	_aposS
	@@

# adj np
# Glom a possessive adj at beginning of noun phrase.
# (Not if np starts with a det etc.
@CHECK
  if (!N("possessive",1))
    fail();
@POST
  listadd(2,1,"false");
@RULES
_xNIL <-
	_adj
	_np
	@@

# 's alpha dqan verb
@CHECK
  if (!N("adj",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_aposS _aposX)]
	_xALPHA
	_adj [star lookahead]
	_noun [plus]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# A possible bad possessive.  (s instead of 's).
# noun alpha prep
@CHECK
  if (!N("noun",2))
    fail();
  if (!plural(N(1)) || !plural(N(2)))
    fail();
@POST
  # First noun should really be adj possessive.
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  # Flag that this is possessive adj etc.
  N("posessive",1) = 1;
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one match=(_prep)]
	@@

# semi-clausal pattern
# det alpha alpha prep np verb
# dqan alpha alpha prep np verb
@CHECK
  if (!vconjq(N(6),"inf")
   && !vconjq(N(6),"-s"))
    fail();
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("noun",3))
    fail();
  if (!singular(N(2)) || !singular(N(3)))
    fail();
@POST
  fixnpnonhead(2);
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_prep [lookahead]
	_np
	_xWILD [one match=(_modal _verb _vg)]
	@@

# adj alpha alpha np verb
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3))
    fail();
@POST
  fixnpnonhead(2);
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_np _pro)]
	_xWILD [one match=(_modal _verb _vg)]
	@@

# clausal look.
# np vg prep dqan alpha
# noun verb prep dqan alpha
@CHECK
  if (!N("adj",10) && !N("noun",10) && !N("unknown",10))
    fail();
  if (N("verb",10))
    if (!vconjq(N(10),"inf")
	 && !vconjq(N(10),"-s"))
	  fail();
@POST
  L("tmp10") = N(10);
  group(10,10,"_adj");
  pncopyvars(L("tmp10"),N(10));
  fixadj(N(10));
@RULES
_xNIL <-
    _vg											### (1)
    _xWILD [star match=(_adv _advl)]			### (2)
    _np											### (3)
    _xWILD [star match=(_adv _advl)]			### (4)
    _prep										### (5)
    _xWILD [star match=(_adv _advl)]			### (6)
    _xWILD [star match=(_det _pro)]				### (7)
    _xWILD [star match=(_quan _num _xNUM)]		### (8)
    _xWILD [plus match=(_adj _noun)]			### (9)
    _xALPHA										### (10)
    _xWILD [one lookahead match=(_adj _noun)]	### (11)
    @@

# noun alpha adj $
@CHECK
  if (!N("verb",2))
    fail();
  if (!copulaq(N(2)))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_noun
	_xALPHA
	_adj [lookahead]
	_xEND
	@@

# Locatives, adverbials.
# here
@POST
  if (!N("mypos",1))
    chpos(N(1),"RB");
  L("tmp1") = N(1);
  group(1,1,"_adv");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_xWILD [one match=(here)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_xEND)]
	@@

# vg that alpha
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (N("verb",4))
    {
	if (!vconjq(N(4),"inf")
	 && !vconjq(N(4),"-s"))
	  fail();
	}
@POST
  L("tmp4") = N(4);
  if (N("adj",4))
    group(4,4,"_adj");
  else
    group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	that [s]
	_xALPHA
	_noun [lookahead]
	@@

# clausal
# that
# Note: Looking for transitive verb pattern.
# Note: parallel construction can give wrong answer.
# ex: Animals eat that feed and benefit from it.
# "feed" could be a verb ... Nearly ambig syntactically,
# semantically.)
# verb that alpha
# vg that alpha
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (N("verb",4))
    {
	if (!vconjq(N(4),"inf")
	 && !vconjq(N(4),"-s"))
	  fail();
	}
@POST
  L("tmp4") = N(4);
  if (N("noun",4) || N("unknown",4))
    group(4,4,"_noun");
  else
    group(4,4,"_adj");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	that [s]
	_xALPHA
	@@

# clausal.
# that
# verb that dqan and verb
# vg that dqan and vg
# Todo: check parallel verb construction.
@CHECK
  if (N("mypos",3))
    fail();
@POST
  chpos(N(3),"DT");	# that/DT.
  pnrename(N(3),"_det");
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(that) except=(_det)]
	_xWILD [star lookahead match=(_adv _advl)]
	_adj [star]
	_noun [plus]
	_xWILD [star match=(_adv _advl)]
	_conj
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_vg _verb)]
	@@

# clausal.
# that
# verb that dqan
# Note: Looking at some verb patterns.
# said that water is bad. (ambig).
# worked that boy (unambig).
# said that boys are bad. (unambig)
@CHECK
  if (N("pos50 v-that-dqan",3))
    fail();
@POST
  N("pos50 v-that-dqan",3) = 1;
  L("head") = lasteltnode(6);
  L("txt") = nodestem(N(1));
  L("T5") = finddictattr(L("txt"),"T5");
  if (plural(L("head")))
    chpos(N(3),"IN");	# that/IN.
  else if (!L("T5"))
    {
    chpos(N(3),"DT");	# that/DT.
    pnrename(N(3),"_det");
	}
  # else ambiguous case.
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(that) except=(_det)]
	_xWILD [star lookahead match=(_adv _advl)]
	_adj [star]
	_noun [plus]
	_xWILD [one fail=(_xALPHA _aposS _aposX _verb _vg _modal)]
	@@

# clausal.
# that
# verb that adj
# Note: may want to get rid of this, require singular head noun.
@CHECK
  if (N("pos50 v-that-adj",3))
    fail();
  L("txt") = nodestem(N(1));
  L("T5") = finddictattr(L("txt"),"T5");
  if (L("T5"))
    fail();
  if (copulaq(N(1)))
    fail();
@POST
  N("pos50 v-that-adj",3) = 1;
  chpos(N(3),"DT");	# that/DT.
  pnrename(N(3),"_det");
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(that) except=(_det)]
	_xWILD [star lookahead match=(_adv _advl)]
	_adj
	@@

# semi clausal.
# verb , prep dqan , alpha
@CHECK
  if (!N("verb",7))
    fail();
  if (!vconjq(N(7),"inf"))
    fail();
@POST
  alphatoverb(7,"active","VB");
  group(3,6,"_advl");
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_xWILD [s one match=(_modal _have _be _do)]
	_adv [star]
	\,
	_prep
	_xWILD [s plus match=(_det _pro _quan _num _xNUM _adj
		_noun _np)]
	\,
	_xALPHA
	@@

# clausal.
# , alpha noun
@CHECK
  if (!N("verb",3))
    fail();
@POST
  alphatoverb(3,0,0);
@RULES
_xNIL <-
	\,
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_pro _np)]
	@@

# verb noun like
# clausal.
@CHECK
  if (N("mypos",3))
    fail();
  if (verbfeat(N(1),"V2"))
    fail();	# Don't know which ...
@POST
  chpos(N(3),"IN");	# like/IN.
  pnrename(N(3),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	like [s]
	@@

# ^ adv alpha
@CHECK
  if (N("pos50 start-x-a",3))
    fail();
@POST
  N("pos50 start-x-a",3) = 1;
  if (N("noun",3))
    {
	if (plural(N(3)))
	  fixnphead(3);
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_adv _advl \, _clausesep)]
	_xALPHA
	@@

# np np like
# Ex: someone (that) I like ...
@CHECK
  if (N("mypos",3))
    fail();
  if (pnname(N(3)) != "_fnword")
    fail();
@POST
  N("glom",1) = "left";
  N("glom",2) = "right";
  N("glom",3) = "left";
  pnrename(N(3),"_verb");
  fixverb(N(3),"active","VBP");
  chpos(N(3),"VBP");	# like/VBP.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_np _pro)]
	like [s]
	@@

# ^ like dqan
@CHECK
  if (N("mypos",2))
    fail();
  if (pnname(N(2)) != "_fnword")
    fail();
@POST
  pnrename(N(2),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART \, \( _dbldash)]
	like [s]
	_xWILD [one lookahead match=(_np _noun
		_det _quan _adj _num _xNUM _prep _fnword
		)]
	@@

# verb noun noun like noun
# Ex: tell them something like ...
@CHECK
  if (pnname(N(7)) != "_fnword")
    fail();
@POST
  pnrename(N(7),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_noun _np _pro)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	like [s]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_np _noun _det _quan _num _xNUM _pro
		_adj \, )]
	@@

# verb noun like noun
@CHECK
  if (pnname(N(5)) != "_fnword")
    fail();
@POST
  pnrename(N(5),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [plus match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	like [s]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_np _noun _det _quan _num _xNUM _pro _adj)]
	@@

# like
# clausal.
@CHECK
  if (pnname(N(2)) != "_fnword")
    fail();
@POST
  pnrename(N(2),"_prep");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	like [s]
	_xWILD [one lookahead match=(_noun _np _pro)]
	_xWILD [one match=(_verb _modal _vg)]
	@@

# clausal.
@PRE
<1,1> vareq("sem","have");
@CHECK
  if (!N("noun",4))
    fail();
  if (!N("verb",5))
    fail();
  if (!vconjq(N(5),"-en"))
    fail();
@POST
  N("v-v-remote",1) = 1;
  N("v-v-remote",5) = 1;
  alphatovg(5,"active","VBN");
  fixnphead(4);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_det
	_xALPHA
	_xALPHA
	# No right context.
	@@

# np alpha np
@CHECK
  if (N("pos50 np-a-np",3))
    fail();
@POST
  N("pos50 np-a-np",3) = 1;
  if (N("verb",3))
    alphatoverb(3,"active","VBD");
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_np [lookahead]
	@@

# verb noun alpha noun
# vg noun alpha noun
# clausal.
@CHECK
  if (!N("noun",3))
    fail();
  if (N("verb",3))
    {
	if (!vconjq(N(3),"inf"))
	  fail();
	if (verbfeat(N(1),"V2"))
	  fail();
	}
  if (!singular(N(2)))
    fail();
@POST
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_noun
	_xALPHA
	_noun
	@@

# dqan conj dqan
@POST
  dqaninfo(7,8,9,10);
  groupnp();
  dqaninfo(2,3,4,5);
  groupnp();
  group(2,4,"_np");	# List of nps.
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _clausesep _prep _verb _vg
		_fnword)]
	_xWILD [star match=(_det _pro)]	# 2
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]	# 5
	_conj
	_xWILD [star match=(_det _pro)]	# 7
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]	# 10
	_xWILD [one lookahead match=(_xEND _clausesep _prep _conj
		_fnword _vg _verb)]
	@@

# last alpha verb
@CHECK
  if (!N("noun",2))
    fail();
@POST
  fixnphead(2);
  alphatoadj(1);
@RULES
_xNIL <-
	last
	_xALPHA [lookahead]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg \, _clausesep _xEND)]
	@@

# last noun verb
@POST
  alphatoadj(1);
@RULES
_xNIL <-
	last
	_noun [lookahead]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_modal _verb _vg \, _clausesep _xEND
		_prep _fnword )]
	@@
# last adj
_xNIL <-
	last
	_adj [lookahead]
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

# noun yet prep
# noun yet that
# Ex: the wildest show yet ...
# Note: Would like a superlative adj in noun phrase.
@CHECK
  if (N("mypos",2))
    fail();
@POST
  chpos(N(2),"RB");	# yet/RB.
  pnrename(N(2),"_adv");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	yet [s]
	_xWILD [one lookahead match=(_prep _fnword _conj)]
	@@

# det alpha alpha
# dqan alpha alpha
# NIBBLE.
@CHECK
  if (N("verb",2) || N("verb",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3) && !N("adj",3))
    fail();
@POST
  fixnpnonhead(2);
  alphatoadj(3);
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_xALPHA
	_xALPHA
	@@

# clausal.
# ^ noun verb dqan
# ^ np vg dqan
# NIBBLE.
@CHECK
  if (!N(7) && !N(8) && !N(9))
    fail();
  if (!N("noun",10))
    fail();
@POST
  fixnphead(10);
  L("nm") = pnname(N(5));
  if (L("nm") == "_vg")
    fixvg(N(5),"active","VBD");
  else
    fixverb(N(5),"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [star match=(_det _pro)]			# 7
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xWILD [one lookahead fail=(_noun _xALPHA)]	# 04/20/07 AM.
	@@

# np prep dqan alpha alpha
# noun prep dqan alpha alpha
# noun prep alpha alpha
# np prep alpha alpha
# NIBBLE
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (!N("noun",5) && !N("adj",5))
    fail();
  if (!singular(N(4)))
    fail();
  if (N("verb",4) && !N(3) && vconjq(N(4),"-ing"))
 	fail();
  if (N("verb",5) && !N(3) && vconjq(N(5),"-ing"))
 	fail();
@POST
  if (vconjq(N(5),"-edn"))
    fixnphead(4);
  else
    fixnpnonhead(4);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_prep
	_adj [star]
	_xALPHA
	_xALPHA
	@@

# np prep dqan alpha np
@CHECK
  if (!N("verb",7))
    fail();
  if (plural(N(1)) && vconjq(N(7),"inf"))
    succeed();
  if (singular(N(1)) && vconjq(N(7),"-s"))
    succeed();
  fail();
@POST
  alphatoverb(7,"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _np)]
	@@

# ^ alpha alpha verb
@CHECK
  if (!N("noun",3) && !N("adj",3) && !N("adv",3)
   && !N("unknown",3))
    fail();
@POST
  if (N("noun",3) || N("adj",3) || N("unknown",3))
    fixnphead(3);
  else
    alphatoadv(3);
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# to verb noun , alpha alpha
# to vg np , alpha alpha
@CHECK
  if (!N("verb",5))
    fail();
  if (!singular(N(5)) || !singular(N(5)))
    fail();
@POST
  alphatoverb(5,"active","VB");
  if (N("verb",6))
    {
	N("verb",6) = 0;
	--N("pos num",6);
	}
@RULES
_xNIL <-
	to [s]
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	_xALPHA
	@@

# verb back
# vg back
# Note: Idiomatic.
@POST
  alphatoadv(2);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	back
	@@

# alpha adj prep
# NIBBLE
@CHECK
  if (!N("noun",1) && !N("verb",1))
    fail();
@POST
  if (N("verb",1) && !N("noun",1))
    alphatoverb(1,"active",0);
  else if (N("noun",1) && !N("verb",1))
    fixnphead(1);
  else if (copulaq(N(1)))
    alphatoverb(1,"active",0);
  else
    fixnphead(1);
  group(2,2,"_adjc");
@RULES
_xNIL <-
	_xALPHA
	_adj
	_xWILD [one match=(_prep _xEND)]
	@@

# det alpha alpha adj
# dqan alpha alpha adj
# Note: post-adjective thingy.
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  fixnpnonhead(2);
  fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro)]
	_xALPHA
	_xALPHA
	_adj
	_xWILD [one lookahead match=(_prep)]
	@@

# dqan alpha alpha verb
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  fixnpnonhead(2);
  fixnphead(3);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# prep noun alpha , conj
@PRE
<3,3> var("noun");
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_prep
	_noun
	_xALPHA
	\, [lookahead]
	_conj
	@@
_xNIL <-
	_prep
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_clausesep _xEND)]
	@@

# fnword dqan alpha alpha
@CHECK
  if (!N("verb",6))
    fail();
  if (!vconjq(N(6),"-s"))
    fail();
  L("n") = lasteltnode(5);
  if (!pnvar(L("n"),"cap"))
    fail();
@POST
  alphatoverb(6,"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_fnword)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_num _xNUM _quan)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xALPHA
	@@

# , dqan alpha prep np ,
# Note: part of apposition.
@CHECK
  if (!N("noun",6))
    fail();
@POST
  fixnphead(6);
@RULES
_xNIL <-
	\,
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_prep [lookahead]
	_np
	_xWILD [one match=( \, _conj)]
	@@

# verb noun conj alpha
# vg np conj alpha
# verb dqan conj alpha
@CHECK
  if (N("pos50 vn-conj-a",5))
    fail();
@POST
  if (!N("verb",5))
    {
	if (singular(N(5)))
	  fixnpnonhead(5);
	else
	  fixnphead(5);
	}
  else if (singular(N(2)) && singular(N(5)))	# 04/23/07 AM.
	{
	fixnpnonhead(5);	# 04/23/07 AM.
	}
  else
    {
  if (pnname(N(1)) == "_vg")
    L("v") = N("verb node",1);
  else
    L("v") = N(1);
  if (!samevconj(L("v"),N(5)))
     {
	if (singular(N(5)))
	  fixnpnonhead(5);
	else
	  fixnphead(5);
	 }
  else
     {
     S("vc1") = vconj(L("v"));
     alphatoverb(5,0,S("vc1"));
     }
	}
  N("pos50 vn-conj-a",5) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_conj
	_xALPHA
	@@

# vg np prep np
@POST
  group(4,5,"_advl");
  N("pp",4) = 1;
@RULES
_xNIL <-
	_vg
	_np
	_xWILD [star match=(_adv _advl)]
	_prep
	_np [lookahead]
	_xWILD [one match=(_prep)]
	@@

# vg np prep dqan alpha
@PRE
<10,10> var("noun");	# Require noun.
@CHECK
  if (N("pos50 n-v-p-dqan-a",10))
    fail();
@POST
  N("pos50 n-v-p-dqan-a",10) = 1;
  L("lastn") = lasteltnode(9);
  if (N("noun",10))
    {
	alphatonoun(10);
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_np
	_xWILD [star match=(_adv _advl)]
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	@@

# vg np prep alpha
@CHECK
  if (!N("adj",6))
    fail();
@POST
  alphatoadj(6);
@RULES
_xNIL <-
	_vg
	_np
	_xWILD [star match=(_adv _advl)]
	_xWILD [s match=(_prep) except=(to)]
	_adv [star]
	_xALPHA
	_noun [lookahead]
	@@

# whose dqan alpha when
# whose noun alpha when
@CHECK
  if (!N("verb",6))
    fail();
  # Todo: agreement.
@POST
  alphatoverb(6,"active","VBD");
@RULES
_xNIL <-
	whose [s]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_whword [s lookahead]
	@@

# when noun alpha alpha
@CHECK
  if (!N("noun",3) || !N("verb",4))
    fail();
  if (plural(N(2)) || N("mass",2))
    fail();
  if (!plural(N(3)))
    fail();
  if (vconjq(N(4),"-s"))
    fail();
@POST
  alphatoverb(4,"active","VBP");
  fixnphead(3);
@RULES
_xNIL <-
	when [s]
	_noun
	_xALPHA
	_xALPHA
	_xWILD [one match=(_xEND)]
	@@

# alpha whose [NIBBLE]
@CHECK
  if (!N("noun",1))
    fail();
@POST
  fixnphead(1);
@RULES
_xNIL <-
	_xALPHA
	\,
	whose [s]
	@@

# noun alpha alpha noun
@CHECK
  if (N("pos50 n-aa-n",2))
    fail();
@POST
  N("pos50 n-aa-n",2) = 1;
  if (N("verb",3))
    {
	if (vconjq(N(3),"-s"))
	  alphatoverb(3,"active","VBZ");
	}
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_noun [lookahead]
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

# , which alpha noun alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(5),"-ing"))
    fail();
@POST
  alphatoverb(3,"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=( \, _clausesep)]
	which [s]
	_xALPHA
	_noun
	_xALPHA
	@@

# noun of alpha
# np of alpha
# noun prep alpha
@CHECK
  if (N("pos50 n-of-a-x",3))
    fail();
@POST
  N("pos50 n-of-a-x",3) = 1;
  if (N("noun",3))
    fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	of [s]
	_xALPHA
	_xWILD [one lookahead match=(_adv _advl _prep)]
	@@

#########################################################
# adj alpha $ [DEFAULTS]
@CHECK
  if (N("pos50 j-a-end",2))
    fail();
@POST
  N("pos50 j-a-end",2) = 1;
  if (N("noun",2))
    fixnphead(2);
@RULES
_xNIL <-
	_adj
	_xALPHA
	_xEND
	@@

# noun alpha $ [DEFAULTS]
# Note: from frequent alpha patterns.
@CHECK
  if (N("pos50 n-a-end",2))
    fail();
@POST
  N("pos50 n-a-end",2) = 1;
  if (plural(N(1)))
    {
	if (N("noun",2))
	  {
	  if (plural(N(2)))
	    fixnphead(2);
	  else
	    alphatoverb(2,0,0);
	  }
	}
  else
    {
	if ((N("noun",2) || N("unknown",2))
	 && singular(N(2)))
	  fixnphead(2);
	}
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xEND
	@@

# noun alpha np [DEFAULTS]
# Note: somewhat weak rule, but statistically good.
@CHECK
  if (!N("noun",2))
    fail();
@POST
  alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_np [lookahead]
	@@

# prep alpha prep [DEFAULTS]
# Note: a default rule.
@CHECK
  if (N("pos50 p-a-p",2))
    fail();
@POST
  N("pos50 p-a-p",2) = 1;
  if (N("noun",2))
    fixnphead(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_prep
	@@

# prep alpha $ [DEFAULTS]
@CHECK
  if (N("pos50 p-a-end",2))
    fail();
@POST
  N("pos50 p-a-end",2) = 1;
  if (N("verb",2))
    {
	if (vconjq(N(2),"-ing"))
	   L("done") = 1; # Ambig.
	else if (vconjq(N(2),"-en"))
	  alphatoverb(2,"passive","VBN");
	}

  if (N("noun",2) && !L("done"))
    {
	fixnphead(2);
	}
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xEND
	@@

# prep alpha alpha [DEFAULTS]
# Note: Seems to work out that 2nd alpha is a noun, mainly.
@CHECK
  if (N("pos50 p-a-a",3))
    fail();
@POST
  N("pos50 p-a-a",3) = 1;
  if (N("noun",3))
    fixnphead(3);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_xALPHA
	_xWILD [one lookahead fail=(_xALPHA _noun)]
	@@
_xNIL <-
	_prep
	_xALPHA
	_xALPHA
	@@

# noun alpha prep [DEFAULTS]
@CHECK
  if (N("pos50 n-a-p",2))
    fail();
@POST
  N("pos50 n-a-p",2) = 1;
  if (N("noun",2) && N("verb",2))
    ; # Dunno.
  else if (N("noun",2))
    fixnphead(2);
  else if (N("verb",2))
    alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_prep [lookahead]
	@@

# adv alpha noun [DEFAULTS]
@CHECK
  if (N("pos50 x-a-n",2))
    fail();
@POST
  N("pos50 x-a-n",2) = 1;
  if (N("verb",2))
    alphatoverb(2,0,0);
  else if (N("adj",2))
    alphatoadj(2);
@RULES
_xNIL <-
	_xWILD [plus match=(_adv _advl)]
	_xALPHA
	_noun [lookahead]
	@@

# noun alpha noun [DEFAULTS]
@CHECK
  if (N("pos50 n-a-n",2))
    fail();
@POST
  N("pos50 n-a-n",2) = 1;
  if (N("noun",2))
    fixnpnonhead(2);
  else if (N("verb",2))
    alphatoverb(2,0,0);
  else if (N("adj",2))
    alphatoadj(2);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_noun [lookahead]
	@@

# still
@POST
  alphatoadv(1);
@RULES
_xNIL <-
	still
	@@

# alpha noun prep np vg
# Note: ellipted verb (that) ...
@PRE
<1,1> var("verb");
@CHECK
if (!verbfeat(N(1),"T5"))
  fail();
@POST
  alphatoverb(1,0,0);
@RULES
_xNIL <-
	_xALPHA
	_noun
	_prep
	_np
	_vg
	@@

# alpha alpha dqan verb
@CHECK
  if (!N("noun",1))
    fail();
  if (!N("verb",2))
    fail();
  if (!verbfeat(N(2),"T5"))
    fail();
@POST
  alphatoverb(2,"active","VBD");
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_num _quan _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# ^ dqan , which alpha
@CHECK
  if (!N("verb",8))
    fail();
  # N-V checks...
@POST
  alphatoverb(8,"active","VBD");
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	\,
	which [s]
	_xALPHA
	@@

# alpha noun vg
# Note: ellipted verb (that) ...
@PRE
<1,1> var("verb");
@CHECK
if (!verbfeat(N(1),"T5"))
  fail();
@POST
  alphatoverb(1,0,0);
@RULES
_xNIL <-
	_xALPHA
	_noun
	_xWILD [star match=(_adv _advl)]
	_vg
	@@

# Capitalization issues.
@PRE
<1,1> var("cap");
@POST
  pnrename(N(1),"_noun");
  N("pos50 adj->noun",1) = 1;
@RULES
_xNIL <-
	_adj
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@

# which alpha that
@PRE
<3,3> var("verb");
@POST
  alphatoverb(3,"active","VBD");
@RULES
_xNIL <-
	which [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	that [s]
	@@

# np vg vg
# 04/20/07 AM.
@PRE
<3,3> varz("pos50 n-v-v pass");	# Loop guard.
<4,4> var("by-actor");
@CHECK
  if (!vconjq(N(3),"-en"))
	fail();
@POST
  N("pos50 n-v-v pass",3) = 1;
  fixvg(N(3),"passive","VBN");
  N("by-actor",4) = 1;
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_adv _advl)]
	_vg
	_advl
	_vg
	@@

@PRE
<1,1> varz("voice");
<1,1> varz("pos50 v-p-n-v");
<5,5> vareq("voice","active");
@CHECK
  L("v") = N("verb node",1);
  if (!vconjq(L("v"),"-en"))
	fail();
@POST
  N("pos50 v-p-n-v",1) = 1;	# Loop guard.
  fixvg(N(1),"passive","VBN");
@RULES
_xNIL <-
	_vg
	_prep
	_np
	_xWILD [star match=(_adv _advl)]
	_vg
	@@

# , advl ,
@POST
  group(1,3,"_advl");
@RULES
_xNIL <-
	\,
	_advl
	\,
	@@

# fnword noun vg
@POST
  L("tmp2") = lasteltnode(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(_fnword)]
	_noun [plus]
	_xWILD [one lookahead match=(_vg)]
	@@

# verb np adjc
# Note: Scalar converts to an adverbial.
# Ex: "looks 2 times heavier"
@PRE
<3,3> vareq("sem","scalar");
@POST
  group(3,3,"_advl");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_np
	_adjc [lookahead]
	@@

