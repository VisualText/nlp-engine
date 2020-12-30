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

#@PATH _ROOT _TEXTZONE _sent
@NODES _sent

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

# det noun adj
# Note: caps can serve as "adjectives".
@CHECK
  if (!N("cap",2) && !N("num",2) && !N("possessive",3))
  	fail();
@POST
  if (!N("mypos",2))
    chpos(N(2),"NN");
  pnrename(N(2),"_adj");	# noun -> adj
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

# ^ alpha det
# ^ alpha dqan
@CHECK
  if (!N("verb",2))
    fail();
@POST
  alphatovg(2,"active","VB");	# Imperative. # Cleanup 05/25/07 AM.
  N("pos50 vg1",2) = 1;
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
  alphatovg(3,"active",0);	# Cleanup. # 05/25/07 AM.
  N("pos50 vg2",3) = 1;
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

# vg np by alpha alpha+
# vg np prep alpha alpha
@PRE
<3,3> vareq("stem","by");
<4,4> var("verb");
@CHECK
  if (!vconjq(N(4),"-ing"))
  	fail();
  if (!N("noun",5) && !N("adj",5) && !N("unknown",5))
  	fail();
@POST
  alphatovg(4,"active","VBG");
@RULES
_xNIL <-
    _vg									### (1)
    _xWILD [one match=(_np _noun _pro)]	### (2)
    _prep								### (3)
    _xALPHA								### (4)
    _xALPHA [lookahead]					### (5)
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
  alphatovg(2,"active","VBG");	# Cleanup.	# 05/25/07 AM.
  N("pos50 vg3",2) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(by from _conj)]
	_xALPHA
	_xWILD [one lookahead match=(_pro _np)]
	@@

# vg advl
@PRE
<1,1> varz("voice");
<3,3> var("by-actor");
@POST
  fixvg(N(1),"passive","VBN");
  N("pos50 psv1",1) = 1;
@RULES
_xNIL <-
	_vg
	_adv [star lookahead]
	_advl
	@@

# vg prep np advl
@PRE
<1,1> varz("voice");
<5,5> var("by-actor");
@POST
  fixvg(N(1),"passive","VBN");
  N("pos50 psv2",1) = 1;
  group(2,3,"_advl");	# prep np -> advl
  N("pp",2) = 1;
@RULES
_xNIL <-
	_vg
	_prep
	_np
	_adv [star lookahead]
	_advl
	@@

# vg prep np conj alpha advl
@PRE
<5,5> var("noun");
<6,6> var("by-actor");
@POST
  fixnphead(5);
@RULES
_xNIL <-
	_vg
	_prep
	_np
	_conj
	_xALPHA
	_advl
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

# vg prep np
# passive/active computation.
@CHECK
 if (N("pos50 v-p-n",2))
	fail();
 if (N("stem",4) == "by")
	fail();
 # If verb is assigned.
 S("v") = N("first verb",2);
 if (!S("v"))
	fail();
 S("p") = phrprepverbq(S("v"),N(4));
 if (!S("p"))
	fail();
 if (pnvar(S("v"),"VBN"))
	fail();
 L("stem") = strtolower(pnvar(S("v"),"$text"));
 if (finddictattr(L("stem"),"T3"))
 	succeed();
 if (finddictattr(L("stem"),"D1"))
	fail();
 # If np sem is adverbial, eg date, could still be passive.
 if (N("sem",6) == "date")
 	fail();
@POST
 N("pos50 v-p-n",2) = 2;	# Loop guard.
  fixvg(N(2),"active",0);
  N("pos50 vg4",2) = 1;
  if (!N("mypos",4))
  	{
	if (S("p") == 1 || S("p") == 3)
	  chpos(N(4),"RP");	# prep/RP
	else
	  chpos(N(4),"IN");	# prep/IN
	}
  listadd(2,4,"true");	# vg prep -> vg
@RULES
_xNIL <-
	_xWILD [one fail=( \, )]
	_vg
	_xWILD [star lookahead match=(_adv _advl)]
	_prep
	_xWILD [star match=(_adv _advl)]
	_np
	@@

# noun conj alpha prep np vg
@PRE
<3,3> var("noun");
@CHECK
  if (N("verb",3))
  	{
	if (phrprepverbq(N(3),N(4)))
	  fail();
	}
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_prep
	_np
	_vg
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
	_xWILD [one lookahead match=(_qEOS _xEND
		_conj )]	# 05/13/07 AM.
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
  N("pos50 vg5",2) = 1;
@RULES
_xNIL <-
	to [s]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	@@

# to alpha
@CHECK
  if (!N("verb",3))
    fail();
  if (!vconjq(N(3),"inf")) # New function. # 06/15/06 AM.
    fail();
@POST
  alphatovg(3,"active","VB");
  N("pos50 vg6",3) = 1;
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
<3,3> varz("noun");
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"inf")) # New function. # 06/15/06 AM.
    fail();
@POST
  alphatovg(3,"active","VB");
  N("pos50 vg7",3) = 1;
@RULES
_xNIL <-
	to [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [star match=(_adv _advl)]
	_xWILD [lookahead one match=(_conj _prep _fnword _whword \,)]
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
@PRE
<1,1> varne("stem","to");
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
	_xWILD [s one match=(_prep) except=(to as)]
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

# fnword alpha adj
# as alpha adj
@PRE
<1,1> vareq("stem","as");
<2,2> var("adj");
<2,2> varz("verb");
@POST
  alphatoadj(2);
@RULES
_xNIL <-
	_fnword
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
#@CHECK
#  if (!N("verb",3))
#    fail();
#@POST
#  L("tmp3") = N(3);
#  group(3,3,"_verb");
#  pncopyvars(L("tmp3"),N(3));
#  fixverb(N(3),0,0);
#  L("v") = N(3);
#  group(3,3,"_vg");
#  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
#  pncopyvars(L("tmp3"),N(3));
#  N("verb node",3) = L("v");
#  N("voice",3) = "active";
#  clearpos(N(3),1,0);
#@RULES
#_xNIL <-
#	_det
#	_noun
#	_xALPHA
#	_xWILD [one lookahead match=(_det)]
#	@@

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

# prep dqa alpha alpha $
@PRE
<6,6> var("noun");
@CHECK
  if (!N("adj",5) && !N("noun",5))
	fail();
@POST
  fixnphead(6);
  fixnpnonhead(5);
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_det _adv)]
	_xWILD [star match=(_quan _num _xNUM _adv)]
	_adj [star]
	_xALPHA
	_xALPHA
	_xWILD [one match=( \) _dbldash _xEND)]
	@@

# conj det alpha alpha noun
@PRE
<3,3> var("noun");
<4,4> var("verb");
@POST
  fixnphead(3);
  alphatovg(4,"active","VBP");
@RULES
_xNIL <-
	_conj
	_det
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_noun _np _det _quan _num _xNUM)]
	@@

# det alpha alpha $
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",3))
    fail();
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_xWILD [one match=(_prep \) _dbldash _xEND)]
	@@

# det alpha alpha verb
@PRE
<2,2> var("noun");
<3,3> var("adj");
<3,3> var("adv");
<3,3> varz("verb");
<3,3> varz("noun");
@POST
  alphatoadv(3);
  fixnphead(2);
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_verb _modal _vg)]
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

# dqa alpha
@CHECK
  if (!N("noun",2))
    fail();
  if (N("adj",2))	# 04/20/07 AM.
	fail();
  L("n") = lasteltnode(1);
#  if (pnvar(L("n"),"apos-s"))
#    succeed();
  if (numbersagree(N(1),N(2)))
  	succeed();
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
_xNIL <- #50
	_xWILD [one match=(_conj)]
	_adj
	_xALPHA
	_xWILD [lookahead one match=(_qEOS _verb _vg _xEND)]
	@@

# Too broad and old.	# 05/25/07 AM.
# dqa alpha
# Usually will be a noun, even if verb is possible.
#@PRE
#<1,1> varne("stem","each");
#@CHECK
#  if (!N("noun",2))
#    fail();
#  L("last") = lasteltnode(2);
#  if (N(1))
#    L("first") = N(1);
#  else if (N(2))
#    L("first") = N(2);
#  if (!numbersagree(L("first"),L("last")))
#    S("disagree") = 1;
#@POST
#  if (S("disagree"))
#    {
#	group(1,1,"_np");
#	N("bracket",1) = 1;
#	}
#  else
#    {
#    L("tmp2") = N(2);
#    group(2,2,"_noun");
#    pncopyvars(L("tmp2"),N(2));
#    }
#@RULES
#_xNIL <-
#	_xWILD [plus match=(_det _quan _num _xNUM)]
#	_xALPHA
#	_xWILD [one lookahead match=(_qEOS _fnword _conj _prep 
#		_pro _xEND)]
#	@@

# det alpha alpha np vg
# Note: np (that) np vg
@CHECK
  if (!N("adj",2) && !N("noun",2))
	fail();
  if (!N("noun",3))
	fail();
@POST
	fixnphead(3);
	fixnpnonhead(2);
	N("glom",4) = "right";
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_np [lookahead]
	_vg
	@@

	
# det alpha
# dqa alpha noun
@CHECK
  if (!N("noun",3) && !N("adj",3))
	fail();
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
  if (N("noun",2) || N("unknown",2))
    alphatonoun(2);
  else if (N("adj",2))
	alphatoadj(2);
  else if (N("adv",2))
    alphatoadv(2);
  else
    {
	if (G("error"))
	"err.txt" << "[det alpha: " << N("$text",2) << "\n";
	alphatoadj(2);
	N("guess",2) = 1;
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

# prep alpha , dqan conj alpha
@PRE
<1,1> varne("stem","to");
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (!N("noun",9) && !N("adj",9))
    fail();
@POST
 fixnphead(2);
@RULES
_xNIL <-
    _xWILD [one match=(_prep)]				### (1)
    _xALPHA									### (2)
    \,										### (3)
    _xWILD [star match=(_det _pro)]			### (4)
    _xWILD [star match=(_quan _num _xNUM)]	### (5)
    _adj [star]								### (6)
    _noun [plus]							### (7)
    _xWILD [plus match=( \, _conj)]			### (8)
    _xALPHA									### (9)
    @@

# prep alpha , adj
@PRE
<1,1> varne("stem","to");
<2,2> var("adj");
@POST
	alphatoadj(2);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xALPHA
	\,
	_adj
	@@

# dqa alpha
# det alpha , adj
@PRE
<2,2> var("adj");
<2,2> varz("noun");
@POST
	alphatoadj(2);
@RULES
_xNIL <-
	_xWILD [one match=(_det)]
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
	_xWILD [one match=(_det)]
	_xALPHA
	_xWILD [star match=( \, _conj)]
	_xWILD [one fail=(_adj _noun _xALPHA)]
	@@
_xNIL <-
	_xWILD [one match=(those)]
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
  if (N("noun",3) || N("adj",3) || N("unknown",3))
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

# det adj , adj noun
# det adj , adj adj
@POST
  group(2,4,"_adj");	# adj list.
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro _num _quan _xNUM _prep)]
	_adj
	\,
	_adj
	_xWILD [one lookahead match=(_adj _noun)]
	@@

# ^ dqan alpha dqan
@PRE
<7,7> var("verb");
@POST
  # todo: Check agreement.
  alphatovg(7,"active","VBP");
  N("pos50 vg8",7) = 1;
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_det _pro _quan _num _xNUM)]
	@@

# adj alpha , adj noun $
# dqa alpha dqan
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
	\,	# 04/29/10 AM.
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

# conj alpha
@CHECK
  if (N("noun",2))
    fail();
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
  nountonp(3,1);
  if (L("ne arr"))
    N("ne arr",3) = L("ne arr");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xWILD [plus match=(_adv _advl)]
	_noun [one]	# FIX.	04/14/10 AM.
	_xWILD [one lookahead match=(_prep)]
	@@

# prep noun
# conj noun prep
@PRE
<3,3> varz("cap");
@POST
  if (N("ne",3))
    L("ne arr") = N(3);
  fixnounhead(N(3));
  nountonp(3,1);
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
  if (N("verb",2))
	{
	if (vconjq(N(2),"-ed"))
	  fail();
	if (verbfeat(N(2),"T5"))
	  fail();
	}
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
	_xWILD [one match=(_verb _vg)]
	@@

# noun alpha verb
# Assume np agreement failed and so on.
# ex: This makes going to the store impossible.
@PRE
<1,1> varz("proposs");
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
  alphatovg(2,0,0);	# 05/22/07 AM.
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

  N("pos50 vg9",2) = 1;
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

# vg dqan alpha prep vg
# Note: Too many vgs, alpha should be a noun.
@PRE
<7,7> var("noun");
@CHECK
  if (N(3))
    L("s") = N(3);
  else if (N(4))
    L("s") = N(4);
  else if (N(5))
    L("s") = N(5);
  else
    L("s") = N(6);
  if (!numbersagree(L("s"),N(7)))
  	fail();
@POST
  fixnphead(7);
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_adv _advl)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_prep
	_vg
	@@


# vg dqan alpha np vg
# Note: Could still be a verb...
@PRE
<6,6> var("noun");
@POST
  fixnphead(6);
  N("ellipted-that",7) = 1;
  N("glom",7) = "right";
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_np [lookahead]
	_vg
	@@

# dqa alpha
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
  if (!numbersagree(N(2),N(3)))
  	fail();
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
	alphatovg(2,"active","VBP");	# Cleanup.	# 05/25/07 AM.
  N("pos50 vg10",2) = 1;
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
  alphatoverb(5,"active","VBP");
  N("pos50 vg11",5) = 1;
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
	if (vconjq(N(3),"-s"))
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
  nountonp(2,1);
  N("ne",2) = 0;
  if (L("ne arr"))
    N("ne arr",2) = L("ne arr");
@RULES
_xNIL <-
	_xWILD [one match=(_conj)]
	_noun [plus]
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# OLD	# 05/22/07 AM.
# prep noun ,
#@CHECK
#  L("n") = lasteltnode(2);
#  if (pnvar(L("n"),"num"))
#	fail();
#@POST
#  L("tmp2") = lasteltnode(2);
#  group(2,2,"_np");
#  pncopyvars(L("tmp2"),N(2));
#  clearpos(N(2),1,1);	# Zero out token info.
#  N("ne",2) = 0;
#@RULES
#_xNIL <-
#	_prep
#	_noun [plus]
#	\, [opt lookahead]
#	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np _adv)]
#	@@

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

  N("pos50 vg12",2) = 1;
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

  N("pos50 vg13",3) = 1;
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
  nountonp(2,1);
@RULES
_xNIL <- # 70
	_xWILD [one match=(_dbldash)]
	_noun
	_xWILD [one lookahead match=(_prep)]
	@@

# vg np alpha
# Note: ditrans verb pattern.
@CHECK
  if (!verbfeat(N(1),"D1"))
	fail();
@POST
  fixnpnonhead(3);
@RULES
_xNIL <-
	_vg
	_np
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

# vg np alpha
@CHECK
  if (!verbfeat(N(1),"X7"))
	fail();
@POST
  alphatoadj(3);
  L("tmp3") = N(3);
  group(3,3,"_adjc");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_vg
	_np
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb noun alpha
# vg np alpha
# verb patterns...
@PRE
<3,3> var("prosubj");
<5,5> var("verb");
@CHECK
  if (!vconjq(N(5),"inf"))
    fail();
  if (!verbfeat(N(1),"V2")
   && !verbfeat(N(1),"T5"))
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

# pro alpha
# our book
@PRE
<1,1> varz("proobj");
<1,1> varz("prosubj");
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("noun",2) && N("adj",2))
    fail();
  if (N("noun",2) && N("verb",2))
    {
	if (vconjq(N(2),"-ing"))
	  fail();	# noun/vbg
	}
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
  if (vconjq(N(2),"-ed"))
    {
	S("nd") = "VBD";
  	succeed();
	}
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
  N("pos50 vg14",2) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(_proSubj it)]
	_xALPHA
	_xALPHA
	@@

# pro alpha
@PRE
<1,1> var("prosubj");
<1,1> varz("proobj");
<1,1> varz("proposs");
@CHECK
  if (!N("verb",2) && !N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("verb",2))
    {
	alphatoverb(2,"active","VBP");
	N("pos50 vg15",2) = 1;
	}
  else
    {
	group(2,2,"_adv");
	pncopyvars(L("tmp2"),N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_pro)]
	_xALPHA
	_xWILD [one lookahead fail=(_verb _vg _xALPHA)]
	@@

# who verb
@PRE
<1,1> varz("pos50 who-v");
@POST
  N("pos50 who-v",1) = 1;
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
  alphatovg(2,"active","VBP");
  N("pos50 vg15",2) = 1;
@RULES
_xNIL <-
	_proSubj [s]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_det _quan _xNUM _num _adj _qEOS
		_np _xEND)]
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
	_xWILD [one fail=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# verb alpha adj
# note: Watch for vnj pattern "help verb" patterns etc.
# Need vnn watch also.
@POST
 L("stem") = nodestem(N(1));
 L("tmp2") = N(2);

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

  if (!L("done") && N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	L("done") = 1;
	}

  if (!L("done") && N("noun",2))
    {
    if (finddictattr(L("stem"),"X7") # keep it green ...
     && (N("mass",2) || plural(N(2)) ) )
	   {
	   group(2,2,"_noun");
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

# verb alpha alpha , verb
# vg alpha alpha , vg
@PRE
<3,3> var("noun");
@CHECK
  if (!N("noun",2) && !N("adj",2))
  	fail();
  if (!plural(N(3)))
  	fail();
@POST
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <- #100
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xALPHA
	\, [opt lookahead]
	_vg
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
  alphatovg(5,"active","VBP");	# Cleanup.	# 05/25/07 AM.
  N("pos50 vg16",5) = 1;
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

# what np verb alpha dqan
# Note: Looking for a clause break here.
# Watch for diagreeing verbs and verb patterns.
# NOTE: Assume the mhbv pattern has tried and failed.
# (NOT a good assumption.  Would rather do the current rule first.)
# Note: Other patterns like "what we looked at sent shivers..."
@PRE
<1,1> vareq("stem","what");
<2,2> var("prosubj");
<4,4> var("verb");
<4,4> varz("pos50 what-n-v-a");
@POST
  N("pos50 what-n-v-a",4) = 1;
  # Should also watch for highly ambig verbs (set etc.)
  if (vconjq(N(4),"-ed"))
  	{
	verbtovg(3,3,"active",0);
	alphatovg(4,"active","VBD");
	}
  else if (vconjq(N(4),"inf"))
  	{
	if (verbfeat(N(3),"T2"))
	  {
	  alphatoverb(4,"active","VB");
	  vgvgcompound(3,4,"vg-vg");
	  }
	# Here, either a pattern like "did fight" or not a verb.
	}
  else if (vconjq(N(4),"-ing"))
  	{
	if (verbfeat(N(3),"T4"))
	  {
	  alphatovg(4,"active","VBG");
	  vgvgcompound(3,4,"vg-vg");
	  }
	else
	  {
	  # Here, maybe need to add a "likes eating" verb to "T4".
	  # Or not a verb.
	  }
	}
  else if (vconjq(N(4),"-en"))
    {
	# If not a pattern like "has eaten", then probably adj.
	alphatoadj(4);
	}
  else
    {
	alphatovg(4,"active",0);
	verbtovg(3,3,"active",0);
	}
@RULES
_xNIL <-
	_fnword
	_xWILD [one match=(_noun _np _pro)]
	_verb
	_xALPHA
	_xWILD [one lookahead match=(_np _det _quan _num _xNUM _adj _noun)]
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

# ^ alpha noun
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
	_xSTART
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
  N("pos50 vg17",3) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb much prep
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	much
	_xWILD [one lookahead match=(_prep _xEND)]
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
	_xWILD [one lookahead match=(_xEND _prep _fnword)]
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

# vg alpha adv
# verb alpha adv
@PRE
<2,2> var("noun");
@CHECK
  if (N("verb",2))
  	{
	if (vconjq(N(2),"-ing") && verbfeat(N(1),"T4"))
		fail();
	if (vconjq(N(2),"inf") && verbfeat(N(1),"T2"))
		fail();
	}
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xALPHA
	_adv [lookahead]
	@@

# vg alpha
@CHECK
  if (N("voice",1) != "passive")
	fail();
  if (!N("adv",2))
	fail();
@POST
  alphatoadv(2);
@RULES
_xNIL <-
	_vg
	_xALPHA
	_xWILD [s one lookahead match=(if)]
	@@

# verb alpha conj
# vg alpha conj
# Watch verb patterns...
#@CHECK
#  if (!N("noun",3))
#    fail();
#@POST
#  alphatonoun(3);
#@RULES
#_xNIL <- # 100
#	_xWILD [one match=(_verb _vg)]
#	_xWILD [star match=(_adv _advl)]
#	_xALPHA
#	_xWILD [one lookahead match=(_conj)]
#	@@


# verb alpha
# foreshadow rising
@PRE
<2,2> varz("noun");
@CHECK
  if (!N("adj",2))
    fail();
  if (!N("adv",2) && !N("verb",2))
    fail();
  if (N("verb",2))
  	{
    if (!vconjq(N(2),"-ing") && !vconjq(N(2),"-en"))
      fail();
	if (verbfeat(N(1),"T4") && vconjq(N(2),"-ing"))
	  fail();
	}
@POST
  alphatoadj(2);
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
  N("pos50 vg18",1) = 1;
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
  if (verbfeat(N(1),"T2"))	# help eat ...
    {
	alphatoverb(2,"active","VB");
	N("pos50 vg19",2) = 1;
	}
  else if (N("adj",2))
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

# to adv verb
@PRE
<1,1> vareq("stem","to");
<3,3> varz("mypos");
@CHECK
  if (!vconjq(N(3),"inf"))
  	fail();
@POST
  fixverb(N(3),"active","VB");
@RULES
_xNIL <-
	_prep
	_adv [plus]
	_verb
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

  alphatovg(2,"active","VB");
  vgvgcompound(1,2,"vg-vg");

  N("vg node",1) = L("tmp1");
  # todo: semantics from nonhelping verb...
  N("stem",1) = nodestem(L("tmp2"));
  N("sem",1) = nodesem(L("tmp2"));
  clearpos(N(1),1,0);
  N("pos50 vg20",1) = 1;
@RULES
_xNIL <-
	_vg
	_xALPHA
	_xWILD [one lookahead fail=(_aposS _aposX _verb _vg)]
	@@

# verb verb
# help verb
@CHECK
  if (!verbfeat(N(2),"T2"))
  	fail();
  if (N("mypos",3) == "VB")
  	succeed();
  if (!vconjq(N(3),"inf"))
  	fail();
@POST
  verbtovg(3,3,"active","VB");
  vgvgcompound(2,3,"vg-vg");
@RULES
_xNIL <-
	_xWILD [one fail=(_verb _modal)]
	_vg
	_verb
	@@

# help alpha
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"inf"))
    fail();
@POST
  alphatoverb(2,"active","VB");
  alphatoverb(1,"active",0);
#  L("tmp1") = N(1);
#  L("tmp2") = N(2);
#  group(2,2,"_verb");
#  pncopyvars(L("tmp2"),N(2));
#  fixverb(N(2),"active","VB");
#  L("v") = N(2);
#
#  group(1,1,"_verb");
#  pncopyvars(L("tmp1"),N(1));
#  fixverb(N(1),"active",0);
#
#  group(1,2,"_vg");
#  pncopyvars(L("tmp1"),N(1));	# Copy up "syntax" stuff.
#  N("compoun-vg",1) = 1;
#  N("vg node",1) = L("tmp1");
#  # todo: semantics from nonhelping verb...
#  N("stem",1) = nodestem(L("tmp2"));
#  N("sem",1) = nodesem(L("tmp2"));
#  clearpos(N(1),1,0);
  N("pos50 vg21",1) = 1;
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
  N("pos50 vg22",2) = 1;
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
  if (!verbfeat(N(1),"T4"))
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
  N("pos50 vg23",2) = 1;
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
	_xWILD [s one match=(_verb _vg) except=(_modal _have _be _do)]
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
  N("pos50 vg24",1) = 1;
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
  alphatovg(1,"active","VBD");	# Cleanup.	# 05/25/07 AM.
  N("pos50 vg25",1) = 1;
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
    {
    alphatoverb(3,"active","VBG");
    N("pos50 vg26",3) = 1;
	}
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

# alpha alpha to verb
@PRE
<1,1> var("noun");
<1,1> var("verb");
<2,2> var("adj");
<2,2> varz("verb");
<2,2> varz("noun");
@CHECK
  if (N("pos num",2) != 2)
	fail();
@POST
  alphatoadj(2);
@RULES
_xNIL <-
	_xALPHA
	_xALPHA
	to [s lookahead]
	_xWILD [one match=(_verb _vg)]
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

# adv alpha prep
@PRE
<2,2> var("verb");
@POST
  alphatoverb(2,0,0);
@RULES
_xNIL <-
	_adv
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb worth
@POST
  alphatoadj(3);
  group(3,3,"_adjc");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]	
	_adv [star]
	worth
	@@

# adj alpha noun
@PRE
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"-ing"))
	fail();
@POST
  alphatoverb(2,"active","VBG");
  N("pos50 vg27",2) = 1;
@RULES
_xNIL <-
	_adjc
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _np)]
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
@PRE
<2,2> var("verb");
@CHECK
  if (N("cap",1))
    succeed();	# 05/21/07 AM.
  if (!plural(N(1)))
    fail();
  if (vconjq(N(2),"-s"))
    fail();
@POST
  # Could be "people expected to", ie ambig active/passive.
  if (vconjq(N(2),"-edn"))
	alphatoverb(2,0,0);
  else
    alphatoverb(2,"active",0);
  if (N("cap",1))
    nountonp(1,1);
  N("pos50 vg28",2) = 1;
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@
	
# noun alpha prep
# Seems too loosy goosy	# 05/22/07 AM.
#@CHECK
#  if (N("noun",2) && N("verb",2))
#    fail();
#  if (N("eos",1))
#    fail();
#@POST
#  L("tmp2") = N(2);
#  if (N("verb",2))
#    {
#	if (vconjq(N(2),"-ed") && phrprepverbq(N(2),N(4)))
#	  alphatovg(2,"active","VBD");
#	else
#	  alphatovg(2,0,"VBP");
#	}
#  else if (N("adv",2))
#    {
#	group(2,2,"_adv");
#	pncopyvars(L("tmp2"),N(2));
#	}
#  else if (N("adj",2))
#    {
#	group(2,2,"_adj");
#	pncopyvars(L("tmp2"),N(2));
#	fixadj(N(2));
#	}
#  else
#    {
#	group(2,2,"_noun");
#	pncopyvars(L("tmp2"),N(2));
#	fixnoun(N(2));
#	}
#@RULES
#_xNIL <-
#	_xWILD [one match=(_noun _np)]
#	_xALPHA
#	_xWILD [star match=(_advl _adv)]
#	_xWILD [lookahead one match=(_prep)]
#	@@

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
# Too broad.	# 05/22/07 AM.
#@CHECK
#  if (N("pos num",2) != 2)
#    fail();
#  if (!N("adj",2) || !N("adv",2))
#    fail();
#@POST
#  L("tmp2") = N(2);
#  group(2,2,"_adv");
#  pncopyvars(L("tmp2"),N(2));
#  # Treat as adverb. # 06/17/06 AM.
#  if (pnvar(L("tmp2"),"adj"))
#    fixadj(N(2));
#@RULES
#_xNIL <-
#	_noun
#	_xALPHA
#	@@

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
  N("pos50 vg29",3) = 1;
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
  if (N("verb",4) && vconjq(N(4),"-ing"))
    {
	alphatovg(4,"active","VBG");
    N("pos50 vg30",4) = 1;
	}
  else
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
_xNIL <- #150
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
  L("xx") = pnparent(X());		# 07/10/12 AM.
  # Some temporal context.
  if (pnvar(L("xx"),"date=present"))	# 07/10/12 AM.
  	alphatovg(3,"active","VBP");
  else
  	alphatovg(3,"active","VBD");
  # Get what was actually assigned.
  L("pos") = vgpos(N(3));
  if (L("pos") == "VBG")
  	chpos(N(2),"IN");	# that/IN
  else
    chpos(N(2),"WDT");
  N("pos50 vg31",3) = 1;
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
  L("xx") = pnparent(X());		# 07/10/12 AM.
  if (pnvar(L("xx"),"date=present"))	# Some temporal context.
  	fixvg(N(2),"active","VBP");
  else
  	fixvg(N(2),"active","VBD");
  N("pos50 vg32",2) = 1;
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
  N("pos50 vg33",2) = 1;
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
  N("pos50 vg34",2) = 1;
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
  N("pos50 vg35",2) = 1;
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
	_xWILD [one lookahead match=(_np
		_det _pro _quan _num _xNUM _adj)]
	@@

# that alpha noun
# 04/19/07 AM.
@PRE
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"-s"))
	fail();
@POST
  alphatoverb(3,"active","VBZ");
  N("pos50 vg36",3) = 1;
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]
	@@

# that alpha prep
@PRE
<3,3> varz("verb");
<3,3> var("noun");
@POST
  fixnphead(3);
@RULES
_xNIL <-
	that [s]
	_adv [star]
	_xALPHA
	_prep [lookahead]
	@@

# which alpha alpha
# that alpha alpha
@CHECK
  if (N("pos50 which-a-a",3))
    fail();
  if (N("noun",3) && N("verb",4))
  	fail();	# Another interp.
@POST
  N("pos50 which-a-a",3) = 1;
  if (N("verb",3) && !vconjq(N(3),"inf")
   && (N("adj",4) || N("noun",4)) )
    {
	alphatoverb(3,"active","VBP");
	}
@RULES
_xNIL <-
	_xWILD [s one match=(which that)]
	_adv [star]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead fail=(_xALPHA _verb _vg _modal)]
	@@

# det alpha alpha alpha prep
# Note: should check on last alpha verb patterns...
@PRE
<2,2> varz("pos50 daaap");
@POST
  N("pos50 daaap",2) = 1;
  if (!N("noun",2) && !N("adj",2) && !N("unknown",2))
    {
	if (N("verb",2)
	 && (vconjq(N(2),"-en") || vconjq(N(2),"-ing")) )
	  {
	  alphatoadj(2);	# Fix to vbg/vbn/adj thingy.
	  }
	else if (N("adv",2))
	  alphatoadv(2);
	# Else don't know what to do here.
	}
  else if (!N("noun",2))
    alphatoadj(2);
  else if (!N("adj",2))
    alphatonoun(2);
  else
  	{
	if (N("noun",4) && singular(N(4)))	# Uninflected.
	  {
	  if (N("verb",3) && !vconjq(N(3),"inf"))	# Inflected.
	  	{
		fixnphead(4);
		alphatovg(3,"active",0);
		fixnphead(2);
		}
	  }
	}
@RULES
_xNIL <-
	_xWILD [s one match=(a an the)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# Biased rule.
# dqa alpha alpha alpha dqan
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
	fixvg(N(4),"active","VBP");
  N("pos50 vg37",4) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_det _np)]
	@@

# adj alpha alpha alpha noun
# dqa alpha alpha alpha noun
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
@CHECK
  if (N("pos50 naaap",2))
	fail();	# Loop guard.
@POST
  N("pos50 naaap",2) = 1;	# Loop guard.
  if (!N("verb",2) && !N("verb",3) && !N("verb",4))
	{
	# Rough it out for now.
	if (N("noun",4))
	  fixnphead(4);
 	}
  else if (!N("verb",3) && !N("verb",4))
	alphatoverb(2,0,0);
  else if (!N("verb",2) && !N("verb",4))
	alphatoverb(3,0,0);
  else if (!N("verb",2) && !N("verb",3))
	alphatoverb(4,0,0);
  else if (N("noun",3) && plural(N(3)))
	{
	# Need trans/intr logic too.
	if (vconjq(N(4),"inf"))
		{
		L("voice") = "active";
		L("pos") = "VBP";
		}
	alphatovg(4,L("voice"),L("pos"));
    N("pos50 vg38",4) = 1;
	fixnphead(3);
	}
#  if (!N("noun",3) && !N("adj",3))
#    fail();
#  if (singular(N(4)) && !N("mass",4))
#    fail();
#  if (N("verb",2) && N("noun",4))
#    {
#	group(2,2,"_verb");
#	pncopyvars(L("tmp2"),N(2));
#	L("v") = N(2);
#	group(2,2,"_vg");
#	pncopyvars(L("tmp2"),N(2));
#	clearpos(N(2),1,0);
#	N("verb node",2) = L("v");
#	fixvg(N(2),"active","VBP");
#	}
#  else	# Loop guard.	# 05/18/05 AM.
#    noop();	# FAIL.  # 05/18/05 AM.
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_xALPHA
	_prep [lookahead]
	@@

# noun alpha alpha alpha $
# Favor first as verb.
@PRE
<2,2> varz("pos50 faaae");
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
  N("pos50 faaae",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_fnword _noun)]
	_xALPHA
	_xALPHA
	_xALPHA
	_xEND
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
  if (plural(N(2)))
  	fail();
  if (!vconjq(N(4),"-ed"))
	fail();
@POST
  alphatoverb(4,"active","VBP");
    N("pos50 vg39",4) = 1;
  fixnphead(3);
  fixnpnonhead(2);
@RULES
_xNIL <-
	_xWILD [one match=( _aposS _aposX
	# _det _pro _adj	# 05/22/07 AM.
		)]
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
  N("pos50 vg40",2) = 1;
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
  N("pos50 vg41",5) = 1;
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
	_noun [lookahead]
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

# verb noun conj dqa alpha alpha
# vg np conj dqa alpha alpha
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
  alphaunambigred(5);
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

# ^ prep dqa alpha alpha
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

# prep dqa alpha alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  fixnphead(4);
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
	_xWILD [one lookahead match=(_qEOS _xEND _clausesep _fnword
		_prep)]	# 05/31/07 AM.
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

# prep dqa alpha alpha
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
@PRE
<2,2> varz("adj");
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("verb",2) || !N("adj",2))
    fail();
  if (!vconjq(N(2),"-ed") && !vconjq(N(2),"-en"))
  	fail();
@POST
  alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun [plus]
	_xALPHA
	@@

	
# adj alpha adj
# dqa alpha dqan
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

# prep dqa alpha
@CHECK
  if (!N("noun",5) && !N("unknown",5))
  	fail();
  if (N(2))
    L("f") = N(2);
  else if (N(3))
    L("f") = N(3);
  else if (N(4))
    L("f") = N(4);
  else
    fail();
  if (!numbersagree(L("f"),N(5)))
  	fail();
@POST
  fixnphead(5);
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# Don't like this rule.	# 05/29/07 AM.
# prep dqa alpha
#@CHECK
#  if (N("disagree",1))
#    fail();	# Loop guard.
#  if (!N("noun",4))
#    fail();
#  L("last") = lasteltnode(4);
#  if (N(3))
#    L("first") = N(3);
#  else if (N(4))
#    L("first") = N(4);
#  else
#    L("first") = N(5);
#  if (!numbersagree(L("first"),L("last")))
#    S("disagree") = 1;
#@POST
#  if (S("disagree"))
#  	N("disagree",1) = 1;
#  else
#    {
#  L("tmp4") = N(4);
#  L("next") = pnnext(N(4));
#  if (L("next"))
#    {
#    L("nm") = pnname(L("next"));
#    if (L("nm") == "_noun" || L("nm") == "adj")
#	  L("dqan") = 1; # Inside a noun phrase.
#	}
#
#  if (N("verb",4))
#    {
#	if (vconjq(N(4),"-ing"))
#	  L("vadj") = 1;
#	}
#
#  if ((L("vadj") && L("dqan")) || (L("dqan") && N("adj",4)))
#    {
#	group(4,4,"_adj");
#	pncopyvars(L("tmp4"),N(4));
#	fixadj(N(4));
#	}
#  else
#    {
#	fixnphead(4);
#	}
#	}
#@RULES
#_xNIL <-
#	_xWILD [one match=(_prep)]
#	_proPoss [s opt]
#	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
#	_xALPHA
#	\, [opt lookahead]
#	_xWILD [one fail=(_adj _noun _xALPHA _quan _num _xNUM)]
#	@@

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
  N("pos50 vg42",2) = 1;
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
  alphatoverb(2,"active","VBP");
  N("pos50 vg43",2) = 1;
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
  pnrename(N(1),"_det");	# that -> det
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
  N("pos50 vg44",2) = 1;
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
  N("pos50 vg45",2) = 1;
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
  N("pos50 vg46",3) = 1;
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
   || vconjq(N(3),"-en")
   || pnname(N(3)) == "_modal")
    chpos(N(2),"DT");	# that/DT
  else
    chpos(N(2),"IN");	# that/IN
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _conj _fnword)]
	_xWILD [s one match=(that) except=(_np)]
	_xWILD [one lookahead match=(_modal _verb _vg)]
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
_xNIL <- #200
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
  N("pos50 vg47",2) = 1;
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
  if (literal(N(2)))
    alphatonoun(2);
  group(1,2,"_np");
  group(1,1,"_advl");
  N("ellipted-which-is",1) = 1;
  N("n-times",1) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_xNUM _num _quan)]
	_xWILD [s one match=(time times)]
	_det [lookahead]
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
  N("pos50 psv3",3) = 1;
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
  N("pos50 psv4",3) = 1;
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
  N("pos50 vg48",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_np)]
	_xALPHA
	_xWILD [one lookahead match=(_noun _conj)]
	@@

# this verb
# fnword verb
@CHECK
  if (N("id",1) == "pos50 this-verb")
    fail();
@POST
  L("tmp1") = N(1);
  chpos(N(1),"DT");
  nountonp(1,1);
  N("id",1) = "pos50 this-verb";
@RULES
_xNIL <-
	this [s]
	_xWILD [one lookahead match=(_verb _vg)]
	@@


# pro verb
@POST
  nountonp(2,1);
  if (pnname(N(3)) == "_verb")
    L("v") = N(3);
  else if (pnname(N(3)) == "_vg")
    L("v") = N("verb node",3);
  if (L("v"))
    fixverb(L("v"),"active","VBP");
  N("pos50 vg49",3) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS _dblquote \,)]
	_pro
	_xWILD [one match=(_verb _vg)]
	@@

# pro verb
@CHECK
  if (N("pos50 pro-verb",2))
    fail();	# Loop guard.
@POST
  if (N("prosubj",1) && !N("proobj",1))
    fixverb(N(2),"active","VBP");
  else
    fixverb(N(2),"active",0);
  ++N("pos50 pro-verb",2); # Loop guard.
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
@PRE
<3,3> varz("done");	# Don't muck with this verb.
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
  pnrename(N(3),"_adj");	# verb -> adj
  if (S("vc") == "-en"
   || S("vc") == "-edn")
    chpos(N(3),"VBN");
  else
   chpos(N(3),"VBG");
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_advl _adv _premod)]
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
  if (N("verb",5))
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


# dqa alpha
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
  if (N("cap",2))
	{
    if (pnname(N(3)) == "_verb")
	  {
      L("m") = numberv(N(3));
	  if (N("sem",2) == "nationality" && L("m") == "plural")
	    N("mypos",2) = "NPS";
	  }
	else
	  cappos(N(2),0);
	}
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
    fixvg(N(2),"active","VBP");
  else
    fixverb(N(2),"active","VBP");
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
  chpos(N(2),"JJR");
  if (N(1))
	{
	group(1,3,"_det");	# 05/08/07 AM.
	clearpos(N(1),1,0);
	}
  else
	{
    group(2,3,"_advl");	# 06/02/06 AM.
    clearpos(N(2),1,0);
	}
@RULES
_xNIL <-
	no [s opt]
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
	_xWILD [one match=(_adv _advl
#		_adj	# Still ambig.	# 05/17/07 AM.
		)]
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

# Probably not enough context.	# 05/27/07 AM.
# pattern...
# noun that alpha prep
# that
@PRE
<3,3> var("verb");
@CHECK
  if (vconjq(N(3),"-ing"))
  	fail();
  if (vconjq(N(3),"-en") && !vconjq(N(3),"-ed"))
  	fail();
  # Todo: noun-verb agreement.
@POST
  alphatovg(3,"active","VBP");
  L("pos") = vgpos(N(3));
  if (!L("pos") || N("mypos",2))
    ;
  else if (L("pos") == "VBG")
  	chpos(N(2),"IN");
  else
    chpos(N(2),"WDT");
  N("bracket",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _advl)]
	that [s]
	_xALPHA
	_xWILD [lookahead one match=(_prep _qEOS _xEND)]
	@@

# that vg
# np that vg
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
  L("xx") = pnparent(X());		# 07/10/12 AM.
  if (pnvar(L("xx"),"date=present"))	# 07/10/12 AM.
    fixverb(N(4),"active","VBP");
  else
    fixverb(N(4),"active","VBD");
  N("pos50 fixv1",4) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _advl)]
	that [s]
	_xWILD [star  match=(_advl _adv)]	# 04/24/10 AM.
	_xWILD [one match=(_modal _verb _vg)]
	@@

# vg np that alpha alpha+ prep
@PRE
<5,5> var("verb");
@CHECK
  S("n") = lasteltnode(6);
  if (!pnvar(S("n"),"noun"))
  	fail();
  if (!plural(S("n")))
  	fail();
@POST
  alphatovg(5,"active","VBP");
@RULES
_xNIL <-
    _vg															### (1)
    _np															### (2)
    _xWILD [star match=(_adv _advl)]							### (3)
    that [s]													### (4)
    _xALPHA														### (5)
    _xALPHA [plus]												### (6)
    _prep														### (7)
    _xWILD [one lookahead match=(_noun _np _adj _det _pro _quan
        _num _xNUM)]											### (8)
    @@

# vg np that alpha
@PRE
<4,4> var("verb");
@CHECK
  if (!vconjq(N(4),"-s") && !vconjq(N(4),"inf"))
	fail();
@POST
  alphatovg(4,"active","VBP");
  if (!N("mypos",3))
  	{
	L("pos") = vgpos(N(4));	# See what was assigned.
	if (L("pos") == "VBG")
	  chpos(N(3),"IN");	# that/IN
	else
	  chpos(N(3),"WDT");	# that/WDT.
	}
@RULES
_xNIL <-
	_vg
	_np
	that [s]
	_xALPHA
	_xWILD [one lookahead match=(_adv _advl)]
	@@

# that alpha alpha $
@PRE
<2,2> varz("pos50 that-aa-p");
@POST
  N("pos50 that-aa-p",2) = 1;
  # Leave pos of "that" for something with more context.
  if (!N("verb",2) && !N("verb",3))
  	{
	}
  else if (!N("verb",2))
    alphatovg(3,"active","VBP");
  else if (!N("verb",3))
    alphatovg(2,"active","VBP");
  # Todo: Else can elaborate further.
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_qEOS _xEND)]
	@@

# that alpha alpha verb
# Note: Not worrying about that/det vs others.
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
	_xWILD [one match=(_fnword \, _clause )]
	_xWILD [star match=(_adv _advl)]
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
@PRE
<1,1> varz("pos50 pro-a-c-a");
@POST
  N("pos50 pro-a-c-a",1) = 1;
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
   || S("vc4") == "-ed"
   || S("vc4") == "inf")
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
	N("pos50 psv5",4) = 1;
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

# vg dqan alpha alpha noun
@PRE
<1,1> varz("pos50 v-dqan-aa-n");
@POST
  N("pos50 v-dqan-aa-n",1) = 1;
  if (N("noun",6) && N("verb",7))
  	{
	if (vconjq(N(7),"-ing"))
		{
		fixnphead(6);
		alphatovg(7,"active","VBG");
		}
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xALPHA
	_noun [lookahead]
	@@

# vg dqa alpha alpha
@PRE
<2,2> var("proposs");
<5,5> varz("pos50 v-dqa-aa*");
@CHECK
  if (!N(2) && !N(3) && !N(4))
  	fail();	# for now, ignore...
@POST
  N("pos50 v-dqa-aa*",5) = 1; # Loop guard.
  if (!N("noun",5) && !N("adj",5))
    {
	# Handle ving, ven, verb patterns, etc.
	}
  else if (N("adj",5) && !N("noun",5))
  	alphatoadj(5);
  else if (N("noun",5) && !N("adj",5))
    alphatonoun(5);
  else if (plural(N(5)))
    fixnphead(5);
  else if (N("noun",6))
    {
	if (plural(N(6)))
      {
	  fixnphead(6);
	  fixnpnonhead(5);
	  }
	else
	  fixnpnonhead(5);
	}
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]			### (1)
    _xWILD [star match=(_det _pro)]			### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]								### (4)
    _xALPHA									### (5)
    _xALPHA									### (6)
    _xALPHA [star]							### (7)
    \, [opt lookahead]						### (8)
    _adv [star]								### (9)
    _xWILD [one match=(_prep)]				### (10)
    @@

# noun , dqa alpha
@PRE
<6,6> var("noun");
@POST
  fixnphead(6);
@RULES
_xNIL <- #250
	_xWILD [one match=(_np _noun)]
	\,
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
 	_adj [plus]
	_xALPHA
	_xWILD [one lookahead match=(_adv _advl \( \, )]
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
  pnrename(N(2),"_verb");	# noun -> verb.
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
  pnrename(N(2),"_verb");	# noun -> verb
@RULES
_xNIL <-
	_proSubj [s]
	_noun
	_xWILD [s one lookahead match=(up down)]
	@@

# , said
@POST
  if (vconjq(N(3),"-ed"))
    alphatovg(3,"active","VBD");
  else
    alphatovg(3,"active","VBP");
  N("glom",3) = "left";
  clearpos(N(3),1,0);
  if (L("next") = pnnext(N(3)))
    pnreplaceval(L("next"),"glom","right");
@RULES
_xNIL <-
	_xWILD [one match=( \, _dblquote _qEOS _xSTART _conj _clause
		_noun _np)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(say says said quip quips quipped
		recalls recalled reads read)]
	@@

# noun , alpha to
@PRE
<4,4> var("verb");
<4,4> var("adj");
<4,4> vareq("pos num",2);
@CHECK
  if (!vconjq(N(4),"-en"))
  	fail();
@POST
  alphatovg(4,"passive","VBN");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\, [s]
	_adv [star]
	_xALPHA
	_xWILD [s one lookahead match=(to)]
	@@

# noun , alpha prep
@CHECK
  if (!N("verb",4))
    fail();
  if (N("adv",4))
    fail();
  if (!phrprepverbq(N(4),N(5)))
  	fail();	# 05/22/07 AM.
@POST
  alphatovg(4,0,0);	# Cleanup.	# 05/27/07 AM.
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
_clause [unsealed] <-
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
  group(3,6,"_np");	# Appositive np-np.
  if (N("stem",2) != "by")
    {
    group(2,3,"_advl");
    pncopyvars(L("tmp3"),N(2));
    clearpos(N(2),1,0);
    N("pp",2) = 1;
	}
@RULES
_xNIL <-
	_xWILD [one match=(_np _noun)]
	_prep
	_np
	\,	# 04/24/10 AM.
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [one lookahead fail=(_noun _np _xALPHA _det _pro _quan _num
		_xNUM )]
	@@


# including np vg
@PRE
<4,4> varz("voice");
@CHECK
  if (!vconjq(N(4),"-en"))
  	fail();
@POST
  fixvg(N(4),"passive","VBN");
@RULES
_xNIL <-
	_xWILD [one match=(\, )]
	including [s]
	_np
	_vg
	@@

# prep np
@POST
  L("year") = N("year",3);
  group(2,3,"_advl");
  N("pp",2) = 1;
  if (L("year")) N("year",2) = L("year");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART \, _clausesep)]
	_prep
	_np
	_xWILD [one fail=(_verb	# want verbs to go to vg first.
		 _conj _prep)]
	@@

# , prep noun ,
@POST
  L("year") = N("year",3);
  nountonp(3,1);
  group(2,3,"_advl");
  N("pp",2) = 1;
  if (L("year")) N("year",2) = L("year");	# 02/13/14 AM.
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
  # Don't know about that last comma yet.
  group(1,4,"_np");
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

# Apposition.
# ^ dqan , dqan ,
@POST
  dqaninfo(7,8,9,10);
  L("np2") = groupnp();
  dqaninfo(2,3,4,5);
  L("np1") = groupnp();
  group(2,4,"_np");
  clearpos(N(2),1,0);
  N("apposition",2) = 1;
  N("compound-np",2) = 1;
  N("commas-in-here",2) = 1;
@RULES
_xNIL <-
    _xSTART									### (1)
    _xWILD [star match=(_det)]				### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]								### (4)
    _noun [plus]							### (5)
    \,										### (6)
    _xWILD [star match=(_det)]				### (7)
    _xWILD [star match=(_quan _num _xNUM)]	### (8)
    _adj [star]								### (9)
    _noun [plus]							### (10)
    \,										### (11)
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

# Possible apposition. # 06/04/06 AM.
# noun , dqan ,
@CHECK
  if (!N("noun",8) && !N("adj",8))
  	succeed();
  L("n") = lasteltnode(6);
  if (pnvar(L("n"),"cap"))
  	fail();	# Could be part of bigger np.
@POST
  # Todo: reduce to np first.
  dqaninfo(3,4,5,6);
  groupnp();
@RULES
_xNIL <-
	_np
	\,
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	\, [lookahead]
	_xANY
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
_xNIL <-
	_xWILD [one match=(_np)]
	\,
	_xALPHA
	_xWILD [s one lookahead match=(of)]
	@@

@POST
  nountonp(2,1);
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
  nountonp(2,1);
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
@PRE
<4,4> varz("pos50 n-comma-v");
@POST
  if (N("voice",4))
    ; # Done.
  else if (pnname(N(4)) == "_vg")
   {
   if (N("commas-in-here",2))
     fixvg(N(4),"active","VBP");
   else
     fixvg(N(4),"passive","VBN");
   }
  else
    {
	if (N("commas-in-here",2))
	  fixverb(N(4),"active","VBP");
	else
      fixverb(N(4),"passive","VBN");
	}
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
	nountonp(2,1);
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
  if (pnname(N(1)) == "_fnword")
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
  N("pos50 psv6",2) = 1;
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
  nountonp(2,1);
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
  L("det stem") = N("det stem",1);
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
  if (L("det stem"))
    N("det stem",1) = L("det stem");
@RULES
_xNIL <-
	_np
	of [s]
	_np
    _xWILD [one fail=(_conj)]
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
@PRE
<3,3> varz("renamed");	# Already renamed.
@POST
  pnrename(N(3),"_fnword");	# as -> fnword
@RULES
_xNIL <-
	_xWILD [one match=(_pro _np)]
	_xWILD [star match=(_adv _advl)]
	as [s except=_fnword]  # Loop guard.
	_np
	_xWILD [one match=(_modal _verb _vg)]
	@@

# np prep np verb prep
# Note: Check for verb+prep idiom...
@CHECK
  if (pnvar(N(5),"mypos"))
	fail();
  if (phrprepverbq(N(5),N(6)))
	fail();
@POST
	L("v") = N(5);
	group(5,5,"_vg");
	pncopyvars(L("tmp5"),N(5));
	clearpos(N(5),1,0);
	N("verb node",5) = N("first verb",5) = L("v");
	fixvg(N(5),0,0);
@RULES
_xNIL <-
	_xWILD [one match=(_pro _np)]
	_xWILD [star match=(_adv _advl)]
	_prep
	_np
	_verb
	_prep [lookahead]
	@@


# prep np $
@POST
  L("tmp3") = N(3);
  group(2,3,"_advl");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,0);
  N("pp",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_adjc)]
	_prep
	_np
	_xEND
	@@

# NOTE:	Trying to defer this.	# 05/25/07 AM.
# by np
#@POST
#  L("tmp2") = N(2);
#  group(1,2,"_advl");
#  pncopyvars(L("tmp2"),N(1));
#  clearpos(N(1),1,0);
#  N("pp",1) = 1;
#  if (N("sem",1) != "date" && N("sem",1) != "geoloc"
#	&& N("sem",1) != "quantity")
#    N("by-actor",1) = 1;	# 04/21/07 AM.
#  N("by-np") = 1;	# 04/21/07 AM.
#@RULES
#_xNIL <-
#	by [s]
#	_np
#	_xWILD [one fail=(_conj)]
#	@@

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
	_xWILD [one lookahead match=(_noun _np _pro)]
	_be [s]
	@@


# np pro vg
@POST
  nountonp(2,1);
  N("glom",2) = "right";
@RULES
_xNIL <-
	_xWILD [one match=(_np)]
	_pro
	_xWILD [one lookahead match=(_vg)]
	@@

# np pro alpha
@PRE
<2,2> var("prosubj");
<3,3> var("verb");
@POST
  N("ellipted-that",2) = 1;
  alphatovg(3,"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_pro
	_xALPHA
	_xWILD [one lookahead match=(_verb _np _det _pro _quan _num _xNUM
		_adj _noun)]
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
  if (N("mypos",3))
    fail();
  if (N("pos50 to-verb-inf",3))
    fail();
@POST
  if (vconjq(N(3),"inf"))
    chpos(N(3),"VB");
  N("pos50 to-verb-inf",3) = 1;
@RULES
_xNIL <-
	to [s]
	_adv [star]
	_verb [lookahead]
	@@

# vg np prep adj
# vg np as adj
# Note: if a sample such as "viewed John as happy",
# then flag no renaming of the adj.
@PRE
<3,3> vareq("stem","as");
<4,4> varz("no-rename");	# Loop guard.
@CHECK
  if (!verbfeat(N(1),"C1pr"))
  	fail();
@POST
  N("no-rename",4) = 1;	# Don't rename adj.
@RULES
_xNIL <-
	_vg
	_np
	_prep
	_adj
	@@

# prep adj
@PRE
<2,2> varz("possessive");
<2,2> varz("no-rename");
@POST
  pnrename(N(2),"_noun");	# adj -> noun
#  chpos(N(2),0);	# Clear out adj etc.
  fixnoun(N(2));	# adj -> noun
  N("rename-adj-noun",2) = 1;
@RULES
_xNIL <-
	_prep
	_adj
	_xWILD [star match=( \, \" _dblquote)]
	_xWILD [one lookahead fail=(_np _adj _noun
		_quan _num _xNUM _xALPHA _dblquote)]
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
  if (N(2))
    S("first") = N(2);
  else if (N(3))
    S("first") = N(3);
  else
    S("first") = N(4);
  S("head") = lasteltnode(4);
  if (N("verb",5))
    {
	if (!vconjq(N(5),"inf"))
	  fail();
	}
  if (!numbersagree(S("first"),S("head"))
   || numbersagree(S("first"),N(5)))
     succeed();
  if (N("advl",5))	# 04/15/05 AM.
    fail();
  # If last noun is plural, no good.	# 04/18/05 AM.
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
	{
	if (!vconjq(N(5),"inf"))	# 07/20/07 AM.
      fail();	# 08/24/06 AM.
	}

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

# prep adj alpha alpha
# of
@CHECK
  if (!N("adj",3) && !N("noun",3))
	fail();
  if (!N("noun",4))
	fail();
  if (!singular(N(3)))
	fail();
  if (!plural(N(4)))
	fail();
@POST
  fixnphead(4);
  fixnpnonhead(3);
@RULES
_xNIL <- #300
	_prep
	_adj
	_xALPHA
	_xALPHA
	_xWILD [one match=( \, )]
	@@
	
# prep dqa alpha
# prep adj alpha
@CHECK
  if (!N("noun",4) && !N("unknown",4))
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

# dqa alpha alpha noun
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

# dqa alpha alpha
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

# verb dqa alpha alpha [NIBBLE]
@PRE
<1,1> varne("stem","do");
<3,3> varz("prosubj");
<3,3> varz("proobj");
@CHECK
  if (!N("adj",6) && !N("noun",6))
    fail();
  if (N("adv",6) && !N(4) && !N(5))
    fail();
  if (N("verb",7) && N("noun",6) && !N("adj",6))
  	{
	# Could be noun verb case.
	if (vconjq(N(7),"-ing"))
		fail();
	}
  if (!N("noun",7))
    fail();
  if (N(3))
    L("first") = eltnode(3);
  else if (N(4))
    L("first") = eltnode(4);
  else if (N(5))
    L("first") = eltnode(5);
  else
    L("first") = N(6);
  if (!numbersagree(L("first"),N(7)))
  	fail();
@POST
 fixnphead(7);
 fixnpnonhead(6);
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
@PRE
<7,7> varne("sem","date");
<7,7> varz("adv");
@CHECK
  if (!N("noun",7))
    fail();
  if (!singular(N(7)) || !singular(N(6)))
    fail();
  if (N("mass",6))
    fail();
  if (N("verb",7) && copulaq(N(1)))
	fail();
  if (N("verb",7))
	{
	if (vconjq(N(7),"inf") && verbfeat(N(1),"V2"))
		fail();	# let someone have ...
 	}
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
  if (N("adj",3))
	fail();	# 05/08/07 AM.
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
	_xWILD [one lookahead fail=(_adj _xALPHA _dbldash \, )]
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

@PRE
<1,1> varz("pos50 v-n-v t5 try");
<3,3> varne("sem","date");	# Not some kind of adverbial np.
@CHECK
  if (!verbfeat(N(1),"T5"))
  	fail();
@POST
  N("pos50 v-n-v t5 try",1) = 1;
  N("glom",3) = "right";
  if (pnname(N(5)) == "_verb")
  	{
	verbtovg(5,5,"active","VBP");
	}
  else if (!vgassigned(N(5)))
	{
	fixvg(N(5),"active","VBP");
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_pro _np _noun)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	@@

# verb noun verb
# vg np vg
# verb patterns...
@PRE
<5,5> varz("pos50 vg-np-vg");
@CHECK
  if (vgassigned(N(5)))
  	fail();
  if (!vconjq(N(5),"inf"))
    fail();
  if (!verbfeat(N(1),"V2"))
  	fail();
@POST
  N("pos50 vg-np-vg",5) = 1;
  if (pnname(N(5)) == "_verb")
  	verbtovg(5,5,"active","VB");
  else
    fixvg(N(5),"active","VB");
#  L("n") = pndown(N(5));
#  "output.txt" << pnvar(L("n"),"mypos") << "\n";
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_pro _np _noun)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
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
	_xWILD [one match=(_noun _np _pro)]
	_xALPHA
	_xWILD [lookahead one match=( \, _clausesep _xEND )]
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

# prep np conj np to
@PRE
<1,1> varne("stem","by");
<5,5> vareq("stem","to");
@POST
  group(2,4,"_np");	# np list
@RULES
_xNIL <-
	_prep
	_np
	_conj
	_np
	_prep [lookahead]
	@@

# vg np conj np
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
  if (N("verb",4))
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

# adj prep noun conj alpha
# Note: parallel construction.
@PRE
<5,5> var("adj");
@POST
  alphatoadj(5);
@RULES
_xNIL <-
	_xWILD [one match=(_adj _adjc)]
	_prep
	_xWILD [one match=(_np _noun)]
	_conj
	_xALPHA
	_prep [lookahead]
	@@

# prep noun conj alpha prep
@PRE
<4,4> var("noun");
@CHECK
  if (N("verb",4))
	{
	if (phrprepverbq(N(4),N(5)))
	  fail();
	}
 # nn agreement checks.
@POST
  fixnphead(4);
@RULES
_xNIL <-
	_prep
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_prep [lookahead]
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
  nountonp(3,1);

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

  nountonp(1,1);
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

  nountonp(1,1);
@RULES
_xNIL <-
	_pro
	_aposS
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# verb prep noun
@PRE
<2,2> varz("mypos");
<3,3> vareq("sem","date");
@CHECK
  if (!prepositionalverbq(N(1),N(2)))
	{
	fail();
	}
@POST
  if (N("stem",2) != "to")
    N("mypos",2) = "IN";	# phr/prep/IN.
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep [lookahead]
	_noun
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
  if (N("stem",2) != "to")
    chpos(N(2),"IN");	# prep/IN
  N("pos50 vpp",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
 	_prep [lookahead]
	_prep
	@@

# verb prep
@CHECK
  if (N("mypos",2))
    fail();
  if (N("pos50-particle",2))
    fail();
@POST
  N("pos50-particle",2) = 1;

	# See if kb has something.	# 5/31/06 AM.
	# 05/02/07 AM. Placed this one first.
	if (L("num") = phrprepverbq(N(1),N(2)))
	  {
	  if (L("num") == 2) # prepositional. # FIX. 06/18/06 AM.
	    chpos(N(2),"IN");
	  else # 1 or 3.
	    chpos(N(2),"RP");	# Default.
	  }
	else if (copulaq(N(1)))
	  {
	  chpos(N(2),"IN");	# eg, UP %   # 06/15/06 AM.
	  }
#	else	# Default.
#	  chpos(N(2),"RP");

@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep [lookahead]
	_xWILD [one match=( _advl \, _dbldash _xEND)]
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

# alpha prep
# Note: Need active/passive trans/intr logic.
@PRE
<1,1> var("verb");
@CHECK
  S("num") = phrprepverbq(N(1),N(2));
  if (!S("num"))
	fail();
@POST
  if (vconjq(N(1),"inf"))
	L("pos") = "VBP";
  alphatoverb(1,0,L("pos"));
#  if (S("num") == 1 # PHRASAL.
#   || S("num") == 3)
#    chpos(N(2),"RP");
#  else
#	chpos(N(2),"IN");
@RULES
_xNIL <-
	_xALPHA
	_prep [lookahead]
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
  chpos(N(1),"RB");	# so/RB
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

# so pro
@PRE
<1,1> varz("ellipted-so-that");	# Loop guard.
<2,2> var("prosubj");
@POST
  chpos(N(1),"IN");	# so/IN
  N("ellipted-so-that",1) = 1;
@RULES
_xWILD <- #350
	so [s]
	_pro [lookahead]
	@@

# det noun '
@POST
  chpos(N(3),"POS");	# Possessive.
  group(2,3,"_adj");
  N("possessive",2) = 1;
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
  if (N("adj",2))
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
  nountonp(2,1);
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
  if (!N("adj",5))
    fail();
@POST
  alphatoadj(5);
@RULES
_xNIL <-
	_xWILD [one match=(_adj _adjc)]
	_xWILD [opt match=( \, _dbldash)]
	_conj
	_adv [star]
	_xALPHA
	_xWILD [one lookahead match=( \, _dbldash _qEOS _xEND)]
	@@

# verb dqan alpha
# pattern: v n adj
@CHECK
  if (!N("adj",6))
    fail();
  S("stem") = nodestem(N(1));
  if (!finddictattr(S("stem"),"X7")) # keep it green ...
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
# pattern: v n n
@CHECK
  if (!N("noun",6))
    fail();
  if (!plural(N(6)))
	fail();
  S("stem") = nodestem(N(1));
  if (!finddictattr(S("stem"),"D1")) # give her flowers ...
    fail();
  S("head") = lasteltnode(5);
  dqaninfo(2,3,4,5);
@POST
  fixnphead(6);
  nountonp(6,1);
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
  if (copulaq(N(1)))
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
  N("pos50 psv7",6) = 1;

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
	_xALPHA [star lookahead]
	_xWILD [one match=(_xEND _fnword
		 _dbldash _adv \, )]
	@@

# vg dqa alpha , np ,
# Apposition.
@PRE
<5,5> var("noun");
@CHECK
  if (!N(2) && !N(3) && !N(4))
    fail();
  if (N("adj",5) && pnname(N(7)) == "_adj")
  	fail();
@POST
  fixnphead(5);
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]								### (1)
    _xWILD [star match=(_det _pro)]								### (2)
    _xWILD [star match=(_quan _num _xNUM)]						### (3)
    _adj [star]													### (4)
    _xALPHA														### (5)
    \, [lookahead]												### (6)
    _xWILD [one match=(_np _noun _det _quan _num _xNUM _adj)]	### (7)
    @@

# verb dqa alpha alpha
@PRE
<5,5> var("noun");
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"-ing"))
  	fail();
  if (!verbfeat(N(1),"V4"))
  	fail();
@POST
  alphatovg(6,"active","VBG");
  fixnphead(5);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	@@

# vg dqa alpha prep
@PRE
<5,5> var("noun");
@CHECK
  if (!N(2) && !N(3) && !N(4))
  	fail();
  if (N("verb",5))
  	{
	if (!vconjq(N(5),"inf"))
		fail();
	}
@POST
  fixnphead(5);
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]			### (1)
    _xWILD [star match=(_det _pro)]			### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]							### (4)
    _xALPHA								### (5)
    _xWILD [one match=(_prep)]				### (6)
    @@

# verb prep dqan alpha
@PRE
<7,7> varz("cap");
<8,8> varne("sem","date");
<8,8> varz("adv");
@CHECK
  if (!N("noun",8))
    fail();
  if (!N(4) && !N(5) && !N(6) && !N(7))
    fail();
  if (N("verb",8))
    {
	if (copulaq(N(1)))
	  fail();
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
	_xWILD [one fail=(_xALPHA _noun _adj _aposS _aposX _prep)]
	@@

# verb prep dqan alpha
# 04/18/07 AM. Restrict -- require prior noun.
@PRE
<8,8> varne("sem","date");
<8,8> varz("adv");
@CHECK
  if (!N("noun",8))
    fail();
  if (N("verb",8))
	{
	if (copulaq(N(1)))
	  fail();
	if (vconjq(N(8),"-ing"))
	  fail();
	if (verbfeat(N(1),"V2") && vconjq(N(8),"inf"))
	  fail();
	}
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

# more than vg by
@PRE
<1,1> var("morethan");
<3,3> varz("voice");
<4,4> vareq("stem","by");
@CHECK
  if (!vconjq(N(3),"-en"))
  	fail();
@POST
  fixvg(N(3),"passive","VBD");
  N("pos50 psv8",3) = 1;
@RULES
_xNIL <-
	_fnword
	_xWILD [star match=(_adv _advl)]
	_vg
	_prep [lookahead]
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
	_xWILD [one match=( \, _dbldash)]
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


# prep num , alpha
@CHECK
  if (N("pos num",4) != 2)
    fail();
  if (!N("verb",4) || !N("adj",4))
    fail();
@POST
  # Could be a date...
  nountonp(2,1);
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
  if (!N("verb",5))
    fail();
  if (pnname(N(1)) == "_vg")
    L("v") = N("verb node",1);
  else
    L("v") = N(1);
  S("vc") = samevconj(L("v"),N(5));
  if (!S("vc"))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_verb");
  pncopyvars(L("tmp5"),N(5));
  N("inf",5) = N("-s",5) = N("-ed",5) = N("-edn",5) =
  	N("-ing",5) = N("-en",5) = 0;
  if (S("vc"))
	{
	L("p") = vconj(N(1));
    pnreplaceval(N(5),L("p"),1);
	}
  L("voice") = N("voice",1);
  if (vconjq(N(1),"VBP"))
	L("pos") = "VBP";
  fixverb(N(5),L("voice"),L("pos"));
  # Fix up the first verb also, if needed...
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_adv _advl)]
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
  N("pos50 psv9",5) = 1;
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

# NOTE: Looks like a mixed up rule.	# 05/25/07 AM.
# Rebuild from scratch.	# 05/30/07 AM.
# alpha conj verb
# alpha conj vg
#@CHECK
#  if (!N("verb",2))
#    fail();
#
#  # Check parallel construction.
#  S("vc") = samevconj(N(2),N(4));
#  if (S("vc"))
#    succeed();
#  if (N("pos num",2) != 2)
#    fail();
#  if (pnname(N(1)) != "_noun" || !N("noun",2))
#    fail();
#@POST
#  L("tmp2") = N(2);
#  if (N("aspect",4) || N("tense",4) == "future") # mhb
#  	fixnphead(2);
#  else if (S("vc"))
#    {
#  group(2,2,"_verb");
#  pncopyvars(L("tmp2"),N(2));
#  N("inf",2) = N("-s",2) = N("-ed",2) = N("-edn",2) =
#  	N("-ing",2) = N("-en",2) = 0;
#  pnreplaceval(N(2),S("vc"),1);
#  fixverb(N(2),0,0);
#  # Fix up the 2nd verb also, if needed...
#    }
#  else
#    {
#	group(2,2,"_noun");
#	pncopyvars(L("tmp2"),N(2));
#	}
#
#@RULES
#_xNIL <-
#	_xWILD [one match=(_noun _np _advl \,)]
#	_xALPHA
#	_xWILD [plus lookahead match=( \, _conj)]
#	_xWILD [one match=(_verb _vg)]
#	@@

# , alpha conj verb
# , alpha conj vg
@PRE
<2,2> var("verb");
@CHECK
  if (!samevconj(N(2),N(4)))
  	fail();
@POST
  L("voice") = N("voice",4);
  L("v") = N("verb node",4);
  if (L("v"))	# 01/30/12 AM.
    L("pos") = pnvar(L("v"),"mypos");
  alphatovg(2,L("voice"),L("pos"));
@RULES
_xNIL <-
	\,
	_xALPHA
	_conj [lookahead]
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
@PRE
<1,1> varz("pos50 a-num");
@POST
  N("pos50 a-num",1) = 1;
@RULES
_xNIL <-
	_xALPHA
	_xWILD [plus match=(_quan _num _xNUM)]
	@@


# Need some context.
# dqan
@PRE
<1,1> varne("glom","left");
<5,5> varz("possessive");
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
  if (!numbersagree(S("first"),S("last")))
    fail();	# 05/29/07 AM.
@POST
  groupnp();
@RULES
_xNIL <-
	_det [plus]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA _conj \, _aposS _aposX
		_dbldash _dblquote _fnword)]
	@@
_xNIL <-
	_det [star]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA _conj \, _aposS _aposX
		_dbldash _dblquote _fnword)]
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
  if (N(4))
	L("f") = N(4);
  else
	L("f") = N(5);
  L("s") = pnvar(L("f"),"$ostart");
  L("l") = lasteltnode(5);
  L("e") = pnvar(L("l"),"$oend");
  L("txt") = inputrange(L("s"),L("e"));
  dqaninfo(2,3,4,5);
  groupnp();
  N("sem text",2) = L("txt");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _prep _xSTART _qEOS)]
	_xWILD [opt match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one match=( \, _conj _advl)]
	_xWILD [one match=(_xALPHA _det _pro _quan _num _xNUM
		_adj _verb _vg _np \, )]
	@@

# dqan
@PRE
<2,2> varz("prosubj");
<2,2> varz("proobj");
<2,2> varne("glom","left");
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
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead match=(_prep _verb _vg _qEOS _xEND
#		\,	# 06/04/06 AM.
		)]
	@@

_xNIL <-
	_xWILD [s one match=(either)]
	_xWILD [opt match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_xALPHA)]
	@@


# dqan
@PRE
<1,1> varz("prosubj");
<1,1> varz("proobj");
<1,1> varne("glom","left");
@CHECK
  S("head") = lasteltnode(4);
  if (pnvar(S("head"),"advl")	# 04/15/05 AM.
   && N(4) != S("head"))
    fail();
  if (pnvar(S("head"),"sem") == "date"
   && N(4) != S("head"))
    fail();
@POST
  dqaninfo(1,2,3,4);
  groupnp();
@RULES
_xNIL <-
	_xWILD [plus match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM _adv)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead match=(_prep _verb _vg _qEOS _xEND
		)]
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
# Note: Also look for subjunctive.
# decree that John eat his peas.
@PRE
<1,1> varz("pos50 verb-that");
@POST
  if (!N("mypos",3))
    chpos(N(3),"IN");	# that/IN.
  if (pnname(N(1)) == "_vg")
    fixvg(N(1),"active","VBD");
  else
    fixverb(N(1),"active","VBD");
  N("pos50 verb-that",1) = 1;
  # subjunctive...
  if (!vgassigned(N(7)))
    {
    L("t") = nodestem(N(1));
    if (finddictattr(L("t"),"that-subjunctive")
	 && vconjq(N(7),"inf"))
	  {
	  if (pnname(N(7)) == "_vg")
	  	fixvg(N(7),"active","VB");	# Subjunctive.
	  else
	  	fixverb(N(7),"active","VB"); # Subjunctive.
	  N("that-subjunctive",1) = 1;
	  N("that-subjunctive",3) = 1;
	  N("that-subjunctive",7) = 1;
	  }
	}
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
@PRE
<2,2> varz("pos50 that-nv");
<4,4> var("verb");
@POST
  N("pos50 that-nv",2) = 1;	# Loop guard.
  # Need a default tense for sentence, text, etc...
  if (vconjq(N(4),"-ed"))
	{
	if (pnname(N(4)) == "_vg")
	  fixvg(N(4),"active","VBD");
	else
	  fixverb(N(4),"active","VBD");
	}
  else if (vconjq(N(4),"inf"))
	{
	# demand that John do good
	# think that John does good
	if (singular(N(3)))
	  {
	  if (pnname(N(4)) == "_vg")
	    fixvg(N(4),"active","VB");
	  else
	    fixverb(N(4),"active","VB");
	  }
	# If plural, VB/VBP ambiguous!
	}
  else
	{
	if (pnname(N(4)) == "_vg")
	  fixvg(N(4),0,0);
	else
	  fixverb(N(4),0,0);
	}

@RULES
_xNIL <-
	_xWILD [one fail=(_fnword)]
	that [s]
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	@@

@CHECK
  if (N("mass",3) || plural(N(3)))
	fail();
  if (N("ne",3) || N("ne arr",3))
	fail();
  if (N("mypos",2))
	fail();
@POST
    pnrename(N(2),"_det");	# that -> det
    chpos(N(2),"DT");	# that/DT
@RULES
_xNIL <-
	_xWILD [one fail=(_np _noun _adv _advl _xALPHA)]
	that [s except=(_det _np)]
	_xWILD [one match=(_noun)]
	_xWILD [one lookahead match=(_verb _vg)]
	@@
_xNIL <- #400
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
  if (vconjq(N(4),"-en"))
  	fail();	# Could be adj also.
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
  nountonp(2,1);

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
	{
	if (plural(N(2)) && plural(N(4)))
	  succeed();
    fail();
	}
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
	_xWILD [one lookahead match=(_det _quan _num _xNUM _noun)]
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
  nountonp(4,1);
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
#  chpos(N(2),"NN");	# Conform Treebank.
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  group(1,2,"_np");
  pncopyvars(L("tmp2"),N(1));
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_xWILD [star match=(_det)]
	_num
	_xALPHA
	_xWILD [one lookahead match=(_prep _np _fnword)]
	@@

# dqa alpha
# num alpha [NIBBLE]
@PRE
<1,1> varz("prosubj");
<1,1> varz("proobj");
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
  if (!L("done") && N("verb",3) && N(2))
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
# dqa alpha
# num alpha [NIBBLE]
@PRE
<1,1> varne("stem","one");
@CHECK
  if (N("pos50 num-a",2))
    fail();
  if (!numbersagree(N(1),N(2)))
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
  nountonp(2,1);
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
  if (!N("verb",9))
    fail();
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),N(9));
  if (!L("arr")[0])
    fail();
@POST
  L("tmp9") = N(9);
  if (pnname(N(9)) != "_verb")
    {
    group(9,9,"_verb");
    pncopyvars(L("tmp9"),N(9));
	}
 L("m") = N(1);
 L("h") = N(3);
 L("b") = N(5);
 L("being") = N(7);
 L("v") = N(9);
 L("neg") = mhbvadv(2,4,6,8);

 if (N(7))
   {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
   }

 group(1,9,"_vg");
 pncopyvars(L("v"),N(1));
 clearpos(N(1),1,0);
 mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),L("v"));
@RULES
_xNIL <-
	_modal [s opt]
	_xWILD [star match=(_adv _advl)]
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

# ^ prep dqan alpha noun ,
@POST
  if (N("verb",7) && vconjq(N(7),"-ing"))
  	alphatovg(7,"active","VBG");
  else if (N("noun",7) || N("unknown",7) || N("cap",7))
    alphatonoun(7);
  else if (N("adj",7))
  	alphatoadj(7);
  else if (N("adv",7))
  	alphatoadv(7);
  else # [DEFAULT
    alphatonoun(7);
@RULES
_xNIL <-
    _xSTART									### (1)
    _prep									### (2)
    _xWILD [star match=(_det _pro)]			### (3)
    _xWILD [star match=(_quan _num _xNUM)]	### (4)
    _adj [star]								### (5)
    _noun [plus]							### (6)
    _xALPHA									### (7)
    _noun [lookahead]						### (8)
    \,										### (9)
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

# ^ prep dqa alpha
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
#  chpos(N(2),"NN");	# Conform treebank.
  chpos(N(2),"RB");
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

# ^ alpha noun
# ^ alpha dqan
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
  if (copulaq(N(2)))
	succeed();
  if (vconjq(N(2),"inf"))
    fail();  # Want inflected.
@POST
  alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun [plus]
	_xALPHA
	_adv [star]
	_adj [lookahead]
	_xWILD [one match=(_adj _noun _fnword)]
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
  pnrename(N(4),"_adj");	# verb -> adj
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
 L("tmp5") = N(5);
 group(2,5,"_adj");
 pncopyvars(L("tmp5"),N(2));
 clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _noun _np)]
	_xWILD [one match=(_adj _adjc)]
	_conj
	_adv [star]
	_adj
	_xWILD [one lookahead match=(_adj _noun _prep _xEND)]
	@@

# adjc conj adj
@POST
  group(1,3,"_adjc"); # Adj list...
@RULES
_xNIL <-
	_adjc
	_conj
 	_adj
	@@

# dqa alpha , alpha alpha $
@PRE
<3,3> var("adj");
<5,5> var("adj");
<6,6> var("noun");
@CHECK
  if (!N(1) && !N(2))
	fail();
@POST
  fixnphead(6);
  fixnpnonhead(5);
  fixnpnonhead(3);
  group(3,5,"_adj");	# List.
@RULES
_xNIL <-
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xALPHA
	\,
	_xALPHA
	_xALPHA
	_xEND
	@@

# Some alphas centered around a comma.
# dqa alpha , alpha
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
@PRE
<4,4> varne("stem","of");
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
  if (N(11))
	{
    if (!N("verb",11))
	  fail();
	}
  L("arr") = vgagree(N(1),N(5),N(7),N(9),N(11));
  if (!L("arr")[0])
    fail();
@POST
  if (N(11))
	{
    if (pnname(N(11)) != "_verb")
	  alphatoverb(11,0,0);
	}
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
	_xWILD [opt match=(_verb _xALPHA)]
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
@PRE
<1,1> varz("pos50 there-be-n");
@POST
	N("pos50 there-be-n",1) = 1;
	N("glom",1) = "right";
	N("glom",7) = "left"; # elts are renumbered 1 2 3=vg 4=advl 5=np
	if (N("stem",1) == "there")
		{
		L("n") = pndown(N(1));
		chpos(L("n"),"EX");	# there/EX
		}
	L("m") = N(3);
	L("h") = N(4);
	L("b") = N(5);
	if (pnname(N(5)) != "_vg")
		{
    L("neg") = mhbvadv(2,6,0,0);
	group(3,5,"_vg");
    mhbv(N(3),L("neg"),L("m"),L("h"),L("b"),0,0);
		}
@RULES
_xNIL <-
	_xWILD [s one match=(there _pro)]
	_xWILD [star match=(_advl _adv)]
	_modal [opt]
	_have [opt]
	_be [s]
	_xWILD [star lookahead match=(_advl _adv)]
	_xWILD [one match=(_noun _np _adj _adjc
		_det _quan _num _xNUM _noun)]
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
@PRE
<5,5> varz("pos50: be-being");	# 09/24/13 AM.
<7,7> varz("pos50: be-being");	# 09/24/13 AM.
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
  ++N("pos50: be-being",5);	# 09/24/13 AM.
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
  chpos(N(1),"RB");	# so/RB
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
  if (!vconjq(N(3),"inf"))
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
  if (!vconjq(N(3),"inf"))
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
_xNIL <- #450
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
	chpos(N(2),"NP");
	nountonp(2,1);
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
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_pro [lookahead]
	@@

# verb prep np verb
@PRE
<1,1> varne("stem","be");
<1,1> varne("stem","have");
<1,1> varne("stem","do");
@POST
  group(2,3,"_advl");
  N("pp",2) = 1;
@RULES
_xNIL <-
	_verb
	_prep
 	_np
	_verb [lookahead]
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
	_xWILD [one lookahead match=(_noun _np _pro
#		_prep	# Might lead to advl. # 05/13/07 AM.
		)]
	@@

# verb prep noun verb
@CHECK
  if (N("voice",1) != "active")
    fail();
  if (N("voice",4))
    fail();
@POST
  fixvg(N(4),"passive","VBN");
  N("pos50 psv10",4) = 1;
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
  alphaunambigred(2);
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
  fixnpnonhead(6);
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
	_xWILD [one match=(_noun _np)]
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
    alphaunambigred(4);
#  else if (N("verb",5))
#    fixnphead(4);	# MAYBE.
  else if (!N("verb",5))
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

# dqa alpha prep
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
	_xWILD [one lookahead match=(_prep \, )]
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
  nountonp(2,1);
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
  N("t") = nodetext(N(7));
  if (phrprepverbq(N(7),N(8)))
	alphatoverb(7,0,"VBP");
  else if (finddictattr(N("t"),"trans"))
	alphatoverb(7,"passive","VBN");
  else
    alphatoverb(7,0,"VBP");
  N("pos50 psv11",7) = 1;
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
  if (N("stem",2) == "date") # Literal word 'date'.
	fail();
@POST
  L("year") = N("year",2);	# 02/13/14 AM.
  nountonp(2,1);
  group(2,2,"_advl");	# Date adverbial... # np -> advl
  if (L("year")) N("year",2) = L("year");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _verb _vg)]
	_xWILD [one match=(_noun _np)]
	@@

# np (date)
@PRE
<2,2> vareq("sem","date");
@POST
  group(2,2,"_advl");
@RULES
_xNIL <-
	_xWILD [one match=(_adj _adjc _noun _np)]
	_np
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
# ^ dqa alpha noun
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
# dqa alpha noun
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

# vg to vg
# wants to eat
# Semantically simpler to deal with...
@PRE
<6,6> varz("pos50 v-to-v 2");
@CHECK
  if (N("voice",1) != "active")
    fail();
@POST
 N("pos50 v-to-v 2",6) = 1;
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

 if (N(6))
   {
   if (pnname(N(6)) != "_prep")
	 vgvgcompound(1,5,"vg-to-vg");
   }
@RULES
_xNIL <-
	_vg
	_xWILD [star match=(_advl _adv)]
	to [s]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [one lookahead fail=(_verb _vg _modal)]
	@@

# verb to vg
@POST
  verbtovg(1,1,"active","VBD");
  vgvgcompound(1,3,"vg-to-vg");
@RULES
_xNIL <-
	_verb
	to [s]
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

# both  noun
@CHECK
  if (!plural(N(2)))
	fail();
  if (N("mypos",1))
	fail();
@POST
  chpos(N(1),"DT");
  pnrename(N(1),"_det");	# fnword -> det
@RULES
_xNIL <-
	both [s]
	_noun [lookahead]
	@@

# verb noun alpha to
# Note: verb (that) noun verb
@CHECK
  if (!N("verb",3))
    fail();
  if (!verbfeat(N(1),"T5"))
    fail();
@POST
  alphatoverb(3,"active","VBP");
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
  if (!N(2))
	N("ellipted-that",1) = 1;
  N("pos50 psv12",5) = 1;

@RULES
_xNIL <-
	_vg
	that [s opt lookahead]
	_np
	\,
	_vg
	_xWILD [one match=(_prep)]
	@@

@PRE
<2,2> varne("glom","left");
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

  if (N(4))
	L("f") = N(4);
  else
	L("f") = N(5);
  L("s") = pnvar(L("f"),"$ostart");
  L("l") = lasteltnode(5);
  L("e") = pnvar(L("l"),"$oend");
  L("txt") = inputrange(L("s"),L("e"));

  groupnp();
  N("sem text",2) = L("txt");
@RULES
_xNIL <-
	_xALPHA
	_xWILD [s plus match=(_det _pro) except=(_proObj)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS _aposX _conj
		_dblquote )]
	@@
_xNIL <-
	_xALPHA
	_xWILD [s star match=(_det _pro) except=(_proObj)]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS _aposX _conj
		_dblquote)]
	@@
_xNIL <-
	_xWILD [one fail=(_xALPHA _det _quan _num _xNUM
		_adj _noun _aposS _aposX \, )]
	_xWILD [s star match=(_det _proPoss)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS _aposX _conj
		_dblquote)]
	@@

# ^ dqan
@POST
  dqaninfo(2,3,4,5);
  groupnp();
@RULES
_xNIL <-
	_xSTART
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead match=(_dbldash _prep _fnword
		_adv _advl _modal _verb _vg)]
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
#@CHECK
#  if (!N("noun",2))
#    fail();
#  if (!plural(N(1)) || !plural(N(2)))
#    fail();
#@POST
#  # First noun should really be adj possessive.
#  L("tmp2") = N(2);
#  group(2,2,"_noun");
#  pncopyvars(L("tmp2"),N(2));
#  # Flag that this is possessive adj etc.
#  N("posessive",1) = 1;
#@RULES
#_xNIL <-
#	_noun
#	_xALPHA
#	_xWILD [one match=(_prep)]
#	@@

# semi-clausal pattern
# det alpha alpha prep np verb
# dqa alpha alpha prep np verb
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
# vg np prep dqan alpha
# verb noun prep dqan alpha
@CHECK
  if (!N("adj",10) && !N("noun",10) && !N("unknown",10))
    fail();
  if (N("verb",10))
    if (!vconjq(N(10),"inf")
	 && !vconjq(N(10),"-s"))
	  fail();
@POST
  if (N("noun",10) || N("unknown",10))
	fixnpnonhead(10);
  else
	alphatoadj(10);
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
	_xWILD [one match=( \, _xEND)]
	@@

# vg that alpha prep
@PRE
<3,3> var("noun");
@CHECK
  if (!plural(N(3)))
  	fail();
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	that [s]
	_xALPHA
	_prep [lookahead]
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
@PRE
<4,4> varz("adv");
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
  if (N("verb",4))
    {
	if (vconjq(N(4),"inf")
	 || vconjq(N(4),"-s"))
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
@PRE
<1,1> varne("stem","be");
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
	N("that-to-det",3) = 1;
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
#@CHECK
#  if (N("pos50 v-that-adj",3))
#    fail();
#  L("txt") = nodestem(N(1));
#  L("T5") = finddictattr(L("txt"),"T5");
#  if (L("T5"))
#    fail();
#  if (copulaq(N(1)))
#    fail();
#@POST
#  N("pos50 v-that-adj",3) = 1;
#  chpos(N(3),"DT");	# that/DT.
#  pnrename(N(3),"_det");
#@RULES
#_xNIL <-
#	_xWILD [one match=(_vg _verb)]
#	_xWILD [star match=(_adv _advl)]
#	_xWILD [s one match=(that) except=(_det)]
#	_xWILD [star lookahead match=(_adv _advl)]
#	_adj
#	@@

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

# , alpha noun verb
@PRE
<3,3> var("adj");
@POST
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _adv _advl)]
	\,
	_xALPHA
	_noun [plus]
	_xWILD [one match=(_modal _verb vg)]
	@@

# clausal.
# , alpha noun
@CHECK
  if (!N("verb",3))
    fail();
  if (vconjq(N(3),"inf"))
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
    {
	if (vconjq(N(3),"-ed"))
		alphatoverb(3,"active","VBD");
	else
      alphatoverb(3,"active","VBP");
	}
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
  if (!singular(N(2)) && !N("cap",2))
    fail();
@POST
  if (N("cap",4))
	fixnphead(3);
  else
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
  debug();
  dqaninfo(7,8,9,10);
  groupnp();
  dqaninfo(2,3,4,5);
  groupnp();
#  group(2,4,"_np");	# List of nps.
@RULES
_xNIL <-
    _xWILD [one match=(_xSTART _clausesep _prep _verb _vg
        _fnword)]											### (1)
    _xWILD [star match=(_det _pro)]							### (2)
    _xWILD [star match=(_quan _num _xNUM)]					### (3)
    _adj [star]												### (4)
    _noun [plus]											### (5)
    _conj													### (6)
    _xWILD [star match=(_det _pro)]							### (7)
    _xWILD [star match=(_quan _num _xNUM)]					### (8)
    _adj [star]												### (9)
    _noun [plus]											### (10)
    _xWILD [one lookahead match=(_xEND _clausesep _prep
        _conj _fnword _vg _verb)]							### (11)
    @@

# np conj dqan
@POST
  dqaninfo(3,4,5,6);
  groupnp();  
@RULES
_xNIL <-
	_np
	_conj
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
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
	_xALPHA # 09/24/13 AM.
	_xWILD [star lookahead match=(_adv _advl)]
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
# dqa alpha alpha
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
    fixvg(N(5),"active","VBP");
  else
    fixverb(N(5),"active","VBP");
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

# np prep dqa alpha alpha
# noun prep dqa alpha alpha
# noun prep alpha alpha
# np prep alpha alpha
# NIBBLE
@PRE
<2,2> varne("stem","to");
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
  alphatoverb(7,"active","VBP");
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

# prep dqan alpha np vg
@PRE
<6,6> var("noun");
@POST
  fixnphead(6);
  N("ellipted-that",7) = 1;
  N("glom",7) = "right";
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_np [lookahead]
	_vg
	@@

# ^ alpha alpha alpha
@PRE
<2,2> varz("pos50 ^aaa");
@POST
  N("pos50 ^aaa",2) = 1;	# Loop guard.
  if (plural(N(3)) && plural(N(4)))
	{
	if (N("verb",3) && N("noun",4))
		{
		fixnphead(4);
		alphatovg(3,"active","VBZ");
		if (singular(N(2)) && !N("mass",2))
			cappos(N(2),0);
		else
			fixnphead(2);
		}
	}
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_fnword _prep)]
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
	_adv [opt lookahead]
	_xWILD [one match=(_verb _vg)]
	@@
_xNIL <-
	\,
	_xALPHA
	_xALPHA
	_adv [opt lookahead]
	_xWILD [one match=(_verb _vg)]
	@@

# ^ alpha prep
@PRE
<3,3> var("verb");
<4,4> varne("stem","by");
@POST
  if (vconjq(N(3),"-en"))
	alphatovg(3,"passive","VBN");
  else if (vconjq(N(3),"inf"))
	alphatovg(3,"active","VB");
  else if (N("noun",3) && vconjq(N(3),"-ing"))
  	fixnphead(3);
  else
    alphatovg(3,"active","VB");
  N("pos50 psv13",3) = 1;
@RULES
_xNIL <-
	_xSTART
	_xWILD [s star match=(although though while _adv)]
	_xALPHA
	_prep
	_xWILD [one match=(_noun _np)]
	_xWILD [one lookahead match=( \, _verb _vg )]
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
    alphaunambigred(6);
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
	_xWILD [s one match=(_prep _xEND) except=(to)]
	@@

# det alpha alpha adj
# dqa alpha alpha adj
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


# dqan alpha alpha
# noun alpha alpha
@PRE
<2,2> varz("pos50 n-a-a-0");
@POST
  N("pos50 n-a-a-0",2) = 1;
  if (plural(N(1)) && plural(N(3)) && N("verb",2))
	{
	if (vconjq(N(2),"-ed"))
		alphatovg(2,"active","VBD");
	else
		alphatovg(2,"active","VBP");
	}
  else if (plural(N(1)) && N("verb",2))
    alphatoverb(2,0,0);
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA [lookahead]
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
  if (!N("noun",7))
    fail();
@POST
  fixnphead(7);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_prep [lookahead]
	_np
	_xWILD [one match=( \, _conj _xEND)]
	@@

# Apposition.
# np , np vg prep
@PRE
<1,1> var("cap");
@CHECK
  if (N("voice",4))
	fail();
  if (N("det stem",3) != "a" && N("det stem",3) != "the")
  	fail();
  if (!vconjq(N(4),"-en"))
	fail();
@POST
  fixvg(N(4),"passive","VBN");
  N("pos50 psv14",4) = 1;
@RULES
_xNIL <-
	_np
	\,
	_np
	_vg
	_xWILD [one lookahead match=(_prep)]
	@@

# verb noun conj alpha alpha
# vg np conj alpha alpha
# verb dqan conj alpha alpha
# note: parallel construction.
@CHECK
 if (N("verb",9) || !N("verb",8))
	fail();
 if (!samevconj(N(1),N(8)))
	fail();
 # Note: could add confidence with vscalar attribute.
@POST
  if (pnname(N(1)) == "_vg")
	{
    L("voice") = N("voice",1);
	L("v") = N("verb node",1);
    L("pos") = pnvar(L("v"),"mypos");
	}
  alphatoverb(8,L("voice"),L("pos"));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_conj
	_xALPHA
	_xALPHA
	@@

# verb prep noun conj alpha
@PRE
<5,5> var("verb");
@CHECK
  if (pnname(N(1)) == "_vg")
    {
	if (N("last vg",1))
	  L("vg1") = N("last vg",1);	# 05/22/07 AM.
	else
	  L("vg1") = N(1);
	S("v1") = pnvar(L("vg1"),"verb node");	# 05/22/07 AM.
	}
  else
	S("v1") = N(1);
  S("pos") = pnvar(S("v1"),"mypos");
  S("c") = samevconj(S("v1"),N(5));
  if (!S("c"))
	fail();
  if (N("noun",5) && (S("c") == "inf" || S("c") == "-s"))
	{
	if (pnname(N(6)) == "_noun" || pnname(N(6)) == "_prep")
	  fail();	# Could be apposition...
	}
@POST
  alphatovg(5,"active",S("pos"));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
 	_prep
	_xWILD [one match=(_noun _np)]
	_xWILD [plus match=( \, _conj)]
	_xALPHA
	_xWILD [one lookahead match=(_noun _np _prep _adv _advl)]
	@@

# verb noun conj alpha noun
@PRE
<4,4> var("verb");
@CHECK
  if (pnname(N(1)) == "_vg")
	L("v1") = N("verb node",1);
  else
	L("v1") = N(1);
  if (!samevconj(L("v1"),N(4)))
	fail();
@POST
  alphatovgcopy(4,N(1));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	_xWILD [plus match=(_conj \, )]
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]
	@@

# verb noun conj alpha
# vg np conj alpha
# verb dqan conj alpha
@CHECK
  if (N("pos50 vn-conj-a",1))
    fail();
  if (N(3) && N("adv",5))
  	fail();
@POST
  if (pnname(N(1)) == "_vg")
	L("v1") = N("verb node",1);
  else
	L("v1") = N(1);
  if (!N("verb",5))
    {
	if (singular(N(5)))
	  fixnpnonhead(5);
	else
	  fixnphead(5);
	}
  else if (samevconj(L("v1"),N(5)))	# 05/02/07 AM.
	{
	if (pnvar(L("v1"),"mypos") == "VBD")
	  alphatovg(5,"active","VBD");
	# Still ambig, possible parallel construction.
	}
  else if (singular(N(2)) && singular(N(5)))	# 04/23/07 AM.
	{
	fixnpnonhead(5);	# 04/23/07 AM.
	}
  else
    {
  if (!samevconj(L("v1"),N(5)))
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
  N("pos50 vn-conj-a",1) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_conj
	_xALPHA
	_xWILD [one lookahead fail=(_np _det _quan _num _xNUM _adj)]
	@@

# vg dqan conj alpha
# verb dqan conj alpha
# verb noun conj alpha
@PRE
<7,7> var("noun");
@CHECK
  L("n") = lasteltnode(5);
  if (plural(L("n")) && !plural(N(7)))
	fail();
  if (singular(L("n")) && !singular(N(7)))
	fail();
  if (vconj(N(1)) == vconj(N(7)))
	fail();	# Possible parallel verb construction.
@POST
  fixnphead(7);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_conj
	_xALPHA
	_xWILD [one match=(_np)]
	@@

# vg dqan conj alpha
# verb dqan conj alpha
# verb noun conj alpha
@PRE
<7,7> var("verb");
@CHECK
  L("n") = lasteltnode(5);
  if (vconj(N(1)) != vconj(N(7)))
	fail();
@POST
 if (pnname(N(1)) == "_vg")
   L("v") = N("verb node",1);
 else
   L("v") = N(1);
 L("pos") = pnvar(L("v"),"mypos");
 alphatovg(7,0,L("pos"));
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]					### (1)
    _xWILD [star match=(_det _pro)]					### (2)
    _xWILD [star match=(_quan _num _xNUM)]			### (3)
    _adj [star]										### (4)
    _noun [plus]									### (5)
    _conj											### (6)
    _xALPHA											### (7)
    _xWILD [one lookahead match=(_np)]				### (8)
    _xWILD [one match=(_xEND _dbldash \, _prep )]	### (9)
    @@

# vg np prep np to alpha
@PRE
<1,6> debug();
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"inf"))
  	fail();
@POST
  alphatovg(6,"active","VB");
@RULES
_xNIL <-
	_vg
	_np
	_prep
	_np
	to [s]
	_xALPHA
	@@

# vg np prep np conj alpha
# vg np prep np , alpha
@PRE
<1,1> varz("pos50 vnpn-conj-a");
<6,6> var("verb");
@CHECK
  if (!N("noun",6) && !N("adj",6))
  	succeed();
  if (samevconj(N(1),N(6)))
  	succeed();
  if (vconjq(N(1),"-ing") && vconjq(N(6),"-ed"))
  	succeed();
  fail();
@POST
  N("pos50 vnpn-conj-a",1) = 1;
  if (N("voice",1) == "active")
  	{
	group(3,4,"_advl");	# prep np -> advl
	N("pp",3) = 1;
	}
  else if (N("stem",3) == "by")
    {
	# Need by-np logic.
	if (!N("sem",4) || N("sem",4) != "quantity") # etc...
		{
		group(3,4,"_advl");
		N("pp",3) = 1;
		}
	else
		{
		group(3,4,"_advl");
		N("pp",3) = 1;
		N("by-np",3) = 1;
		N("by-actor",3) = 1;
		}
	# Can fix up verbs also...
    }
@RULES
_xNIL <-
    _vg		### (1)
    _np		### (2)
    _prep	### (3)
    _np		### (4)
    _xWILD [one match=(_conj \, )]	### (5)
    _xALPHA	### (6)
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
	_np
	_xWILD [one lookahead match=(_prep)]
	@@

# vg np prep dqan alpha , vg
@PRE
<10,10> var("noun");
@POST
  fixnphead(10);
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]			### (1)
    _xWILD [star match=(_adv _advl)]		### (2)
    _np										### (3)
    _xWILD [star match=(_adv _advl)]		### (4)
    _prep									### (5)
    _xWILD [star match=(_det _pro)]			### (6)
    _xWILD [star match=(_quan _num _xNUM)]	### (7)
    _adj [star]								### (8)
    _noun [plus]							### (9)
    _xALPHA									### (10)
    \, [lookahead]							### (11)
    _xWILD [one match=(_verb _vg _modal)]	### (12)
    @@

# vg np prep dqan alpha
@PRE
<10,10> var("noun");	# Require noun.
@CHECK
  if (N("pos50 n-v-p-dqan-a",10))
    fail();
  if (N("verb",10))
	{
	if (!vconjq(N(10),"inf") && !vconjq(N(10),"-s"))
	  fail();
	}
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

# vg np prep dqa alpha alpha
@PRE
<9,9> varz("pos50 v-n-p-dqa-aa");
@CHECK
  if (!N(6) && !N(7) && !N(8))
  	fail();
  if (!N("noun",9) && !N("adj",9))
  	fail();
@POST
  N("pos50 v-n-p-dqa-aa",9) = 1;
  if (N("noun",9) && !N("adj",9))
  	alphatonoun(9);
  else if (N("adj",9) && !N("noun",9))
    alphatoadj(9);
  else if (N("noun",10))
    {
	if (plural(N(10)))
	  {
	  fixnphead(10);
	  fixnpnonhead(9);
	  }
	else if (plural(N(9)))
	  fixnphead(9);
	}

@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]			### (1)
    _xWILD [star match=(_adv _advl)]		### (2)
    _np										### (3)
    _xWILD [star match=(_adv _advl)]		### (4)
    _prep									### (5)
    _xWILD [star match=(_det _pro)]			### (6)
    _xWILD [star match=(_quan _num _xNUM)]	### (7)
    _adj [star]								### (8)
    _xALPHA									### (9)
    _xALPHA									### (10)
    @@

# vg np prep alpha
@CHECK
  if (!N("adj",6))
    fail();
  if (N("verb",6))
  	{
	if (vconjq(N(6),"-ing"))
	  fail();
	}
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

# vg np prep alpha dqan
# vg np prep alpha np
@PRE
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"-ing"))
  	fail();
  if (verbfeat(N(1),"D1"))
  	fail();
@POST
  alphatovg(6,"active","VBG");
@RULES
_xNIL <-
	_vg
	_np
	_xWILD [star match=(_adv _advl)]
	_xWILD [s match=(_prep) except=(to)]
	_adv [star]
	_xALPHA
	_xWILD [one lookahead match=(_np _det _quan _num _xNUM)]
	@@

# whose dqan alpha when
# whose noun alpha when
@CHECK
  if (!N("verb",6))
    fail();
  # Todo: agreement.
@POST
  alphatoverb(6,"active","VBP");
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
  if (N("verb",3) && vconjq(N(3),"-s"))
    {
	alphatoverb(3,"active","VBZ");
	}
  else if (plural(N(1)) && plural(N(4)))
 	{
	if (N("verb",2) && vconjq(N(2),"-ing"))
 		alphatoverb(2,"active","VBG");
	}
  else if (!N("noun",2) && N("verb",2))
  	{
	alphatovg(2,"active","VBP");
	}
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_noun [lookahead]
	@@

# noun alpha alpha that
@PRE
<3,3> var("verb");
<4,4> var("noun");
#@CHECK
#  if (!N("mass",4) && !plural(N(4)))
#	fail();
@POST
  fixnphead(4);
  alphatovg(3,"active","VBP");
@RULES
_xNIL <-
	_np
	_adv [star]
	_xALPHA
	_xALPHA
	that [s lookahead]
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
  alphatoverb(3,"active","VBP");
@RULES
_xNIL <-
	_xWILD [one match=( \, _clausesep)]
	which [s]
	_xALPHA
	_noun
	_xALPHA
	@@

# noun prep alpha
@PRE
<3,3> var("noun");
@CHECK
  if (!N("noun",3) && !N("unknown",3))
  	fail();
  if (N("verb",3))
    {
	if (!vconjq(N(3),"inf"))
	  fail();	# Too ambig.
	}
  if (N("pos50 n-prep-a-x",3))
    fail();
@POST
  N("pos50 n-prep-a-x",3) = 1;
  if (N("noun",3) || N("unknown",3))
    fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [s one match=(of for)]
	_xALPHA
	_xWILD [one lookahead match=(_adv _advl _prep _vg _verb _modal)]
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
# Note: Assumes more context-rich patterns have applied.
@CHECK
  if (N("pos50 n-a-end",2))
    fail();
@POST
  N("pos50 n-a-end",2) = 1;
  # If cap, not sure of singular plural.
  if (plural(N(1)) && !N("cap",1))
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
	else	# Reduce anyway, assume verb would be trans...
	  fixnphead(2);	# 05/22/07 AM.
	}
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead match=( \) _dbldash _xEND )]
	@@

# noun alpha np [DEFAULTS]
# Note: somewhat weak rule, but (some parts) statistically good.
@CHECK
  if (N("noun",2))
  	fail();
@POST
  if (N("verb",2))
    alphatoverb(2,0,0);
  else if (N("adv",2))
  	alphatoadv(2);
  else if (N("adj",2))
    alphatoadj(2);
  else # default
    alphatoadv(2);	# [DEFAULT]
@RULES
_xNIL <-
	_noun
	_xALPHA
	_np [lookahead]
	@@

# to alpha prep [DEFAULTS]
# Note: a default rule.
@PRE
<1,1> vareq("stem","to");
@CHECK
  if (N("pos50 to-a-p",2))
    fail();
@POST
  N("pos50 to-a-p",2) = 1;
  if (N("verb",2) && vconjq(N(2),"inf"))
    alphatovg(2,"active","VB");
@RULES
_xNIL <-
	_prep
	_xALPHA
	_prep
	@@

# prep alpha prep [DEFAULTS]
# Note: a default rule.
@PRE
<1,1> varne("stem","to");
@CHECK
  if (N("pos50 p-a-p",2))
    fail();
@POST
  N("pos50 p-a-p",2) = 1;
  if (N("verb",2))
  	{
	if (vconjq(N(2),"-ing"))
	  alphatovg(2,"active","VBG");
	}
  else if (N("noun",2))
    fixnphead(2);
@RULES
_xNIL <-
	_prep
	_xALPHA
	_prep
	@@

# verb prep alpha $
# vg prep alpha $
# Todo: Need clause-level patterns for this.
# Eg: "product came to market"
@PRE
<1,1> var("cap");
<3,3> vareq("stem","to");
<4,4> var("noun");
@POST
  fixnphead(4);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	_prep
	_xWILD [one match=(market)]
	_xEND
	@@

# prep alpha $ [DEFAULTS]
@CHECK
  if (N("pos50 p-a-end",2))
    fail();
@POST
  N("pos50 p-a-end",2) = 1;
  if (N("verb",2))
    {
	if (N("stem",1) == "to")
		{
		if (vconjq(N(2),"inf"))
		  {
		  alphatoverb(2,"active","VB");
		  L("done") = 1;
		  }
		}
	else if (vconjq(N(2),"-ing"))
	   L("done") = 1; # Ambig.
	else if (vconjq(N(2),"-en"))
	  {
	  alphatoverb(2,"passive","VBN");
	  N("pos50 psv15",2) = 1;
	  L("done") = 1;
	  }
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
@PRE
<1,1> varne("stem","to");	# 05/02/07 AM.
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
#_xNIL <-
#	_prep
#	_xALPHA
#	_xALPHA
#	@@

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
  if (N("noun",2) && N("verb",2))
  	fail();
@POST
  N("pos50 n-a-n",2) = 1;
  if (N("verb",2))
	{
	alphatoverb(2,"active","VBP");
	}
  else if (N("noun",2))
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
  alphatoverb(2,"active","VBP");
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
  alphatoverb(8,"active","VBP");
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
  pnrename(N(1),"_noun");	# adj -> noun
  if (N("cap",1))
	cappos(N(1),0);
  N("pos50 adj->noun",1) = 1;
@RULES
_xNIL <-
	_adj
	_xWILD [one lookahead match=(_modal _verb _vg)]
	@@
_xNIL <-
	_adj
	\, [opt lookahead]
 	_adjc
 	@@

# which alpha that
@PRE
<3,3> var("verb");
@POST
  alphatoverb(3,"active","VBP");
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
#<4,4> var("by-actor");
@CHECK
  if (vgassigned(N(3)) && vgassigned(N(5)))
  	fail();
  if (!vconjq(N(3),"-en"))
	fail();
  if (!vconjq(N(5),"-ed") && !vconjq(N(5),"inf")
  	&& !vconjq(N(5),"-s"))
	 fail();
@POST
  N("pos50 n-v-v pass",3) = 1;
#  N("by-actor",4) = 1;
  if (vgassigned(N(3)))
  	{
	if (N("tense",3))
	  {
	  if (vconjq(N(5),"-en"))
	  	fixvg(N(5),"passive","VBN");
	  else
		fixvg(N(5),"active","VBP");
	  }
	}
  else if (vgassigned(N(5)))
  	{
	if (vconjq(N(3),"-en"))
	  fixvg(N(3),"passive","VBN");
	}
  else # Neither assigned.
	{
	if (vconjq(N(3),"-en"))
	  fixvg(N(3),"passive","VBN");
	}

@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_vg
	_xWILD [plus match=(_adv _advl)]
	_vg
	@@

# np advl vg advl alpha
@PRE
<3,3> varz("pos50 n-a-v-a-alpha");
<5,5> var("verb");
@CHECK
  if (N("voice",3) && N("voice",3) != "passive")
  	fail();
  if (!vconjq(N(3),"-en"))
  	fail();
  if (!vconjq(N(5),"-ed"))
  	fail();
@POST
  N("pos50 n-a-v-a-alpha",3) = 1;
  alphatovg(5,"active","VBD");
  fixvg(N(3),"passive","VBN");
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [plus match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_np _prep _det _quan
		_num _xNUM)]
	@@

# note: heard of asbestos having (active...)
@PRE
<1,1> varz("voice");
<1,1> varz("pos50 v-p-n-v");
<5,5> vareq("voice","active");
@CHECK
  L("v") = N("verb node",1);
  if (!vconjq(L("v"),"-en"))
	fail();
  L("v5") = N("verb node",5);
  if (vconjq(L("v5"),"-ing"))
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
  nountonp(2,1);
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

# Lone noun.
@POST
  pncopyvars(2);
  sclearpos(1,1);	# Zero out token info.
  singler(2,2);
@RULES
_np <-
	_xWILD [one match=(_prep _vg)]
	_noun
	_xWILD [one lookahead match=(_prep _conj _qEOS _fnword _xEND)]
	@@

# now verb
@CHECK
  if (N("mypos",2))
	fail();
@POST
  pnrename(N(2),"_adv");
  N("mypos",2) = "RB";	# now/RB.
@RULES
_xNIL <-
	_xWILD [one match=(_fnword)]
	now [s]
	_xWILD [one lookahead match=(_verb _vg _modal _adv)]
	@@

# noun that noun
# products that year.
# Note: ellipted adverbial "products during that year..."
@CHECK
  if (N("mypos",3))
	fail();
  if (N("sem",4) != "date")
	fail();
  if (!singular(N(4)))
	fail();
@POST
  pnrename(N(3),"_det");
  N("mypos",3) = "DT";	# that/DT.
  group(3,4,"_advl");	# np -> advl
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	that [s]
	_xWILD [one lookahead match=(_noun _np)]
	_xWILD [one match=( \, )]
	@@

# now
# Note: default.
@PRE
<1,1> varz("mypos");
@POST
  chpos(N(1),"RB");	# now/RB. [default].
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	now [s]
	@@

# prep alpha ,
@CHECK
  if (!N("noun",2) && !N("unknown",2))
    fail();
  if (N("verb",2))
    if (vconjq(N(2),"-ing"))
	  fail();
  if (singular(N(2)) && !massnoun(N(2)))
  	fail();
@POST
  alphatonoun(2);
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xALPHA
	_xWILD [one lookahead match=(\, _clausesep)]
	@@

# noun adj noun
# Note: Simplify np formation.
@CHECK
  if (!N("cap",1) && !N("num",1) && !N("possessive",2))
  	fail();
@POST
  pnrename(N(1),"_adj");	# noun -> adj
@RULES
_xNIL <-
	_noun
	_adj [lookahead]
	_noun
	@@

# Note: Fix noun list.
@PRE
<3,3> var("adj");
@POST
  fixnpnonhead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	_adj [star lookahead]
	_noun [plus]
	\,
	_conj
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# Clause level patterns.
@PRE
<3,3> varz("pos50 dbldbl");	# Opt: don't do the long match twice.
<6,6> var("verb");
# check agreement...
@POST
  N("pos50 dbldbl",3) = 1;
  alphatoverb(6,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_np
	_dbldash
	_xWILD
	_dbldash
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM
		_adj _noun _np)]
	@@


# if noun alpha alpha
@CHECK
  if (!N("noun",3) || !N("verb",3))
	fail();
  if (N("pos num",3) != 2)
	fail();
  if (!N("noun",4) || !N("verb",4))
	fail();
  if (N("pos num",4) != 2)
	fail();
  if (!plural(N(3)))
	fail();
  if (!singular(N(4)))
	fail();
@POST
  fixnphead(3);
  alphatoverb(4,"active","VBP");
@RULES
_xNIL <-
	if [s]
	_noun
	_xALPHA
	_xALPHA
	\, [lookahead]
	@@

# ^ np vg np , alpha
# Note: clausal.
@PRE
<10,10> var("verb");
@CHECK
  if (!vconjq(N(10),"-ing"))
	fail();
@POST
  alphatoverb(10,"active","VBG");
@RULES
_xNIL <-
    _xSTART								### (1)
    _xWILD [star match=(_adv _advl)]	### (2)
    _xWILD [one match=(_np _pro)]		### (3)
    _xWILD [star match=(_adv _advl)]	### (4)
    _vg									### (5)
    _xWILD [star match=(_adv _advl)]	### (6)
    _xWILD [one match=(_np _pro)]		### (7)
    _xWILD [star match=(_adv _advl)]	### (8)
    \,									### (9)
    _xALPHA								### (10)
    @@

# np vg $
@CHECK
  if (N("voice",2))
	fail();
  S("v") = N("verb node",2);
  S("t") = nodetext(S("v"));
  if (!finddictattr(S("t"),"trans"))
	fail();
  if (!vconjq(N(2),"-en"))
	fail();
@POST
  fixvg(N(2),"passive","VBN");
  N("pos50 psv16",2) = 1;
@RULES
_xNIL <-
	_np
	_vg
	_xEND
	@@

# that alpha alpha adj to verb
# Verb pattern... X7 ... x finds y (to be) happy to comply.
@PRE
<2,2> var("noun");
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"inf") && !vconjq(N(3),"-ed"))
	fail();
@POST
  chpos(N(1),"WDT");	# that/WDT.
  alphatoverb(3,"active","VBP");
  fixnphead(2);
@RULES
_xNIL <-
	that [s]
	_xALPHA
	_xALPHA
	_adj [lookahead]
	to [s]
	_xWILD [one match=(_verb _vg)]
	@@

# adv conj alpha
@PRE
<3,3> var("adv");
@POST
  alphatoadv(3);
@RULES
_xNIL <-
	_adv
	_conj
	_xALPHA
 	_xWILD [one lookahead match=(_prep _xEND)]
	@@

# noun list
# np conj noun
@CHECK
  S("sem1") = N("sem",1);
  S("sem3") = N("sem",3);
  if (!S("sem1"))
	fail();
  if (S("sem1") != S("sem3"))
	fail();
@POST
  nountonp(3,1);
  if (N("sem val",1))
	L("vals")[0] = N("sem val",1);
  if (N("sem val",3))
	L("vals")[1] = N("sem val",3);
  group(1,3,"_np");
  N("sem",1) = S("sem1");
  if (L("vals"))
	N("sem vals",1) = L("vals");
@RULES
_xNIL <-
	_np
	_conj
	_noun
	_xWILD [one lookahead match=( \, _conj )]
	@@

# np conj noun
@POST
  fixnphead(3);
  nountonp(3,1);
  if (N("sem val",1))
	L("vals")[0] = N("sem val",1);
  if (N("sem val",3))
	L("vals")[1] = N("sem val",3);
  group(1,3,"_np");
  if (S("sem1"))
    N("sem",1) = S("sem1");
  if (L("vals"))
	N("sem vals",1) = L("vals");
@RULES
_xNIL <-
	_np
	_conj
	_noun
	\, [lookahead]
	_np
	@@

# noun list
# vg np conj np
@POST
  if (N("sem text",2))
	L("arr")[0] = N("sem text",2);
  if (N("sem text",4))
	L("arr")[1] = N("sem text",4);
  group(2,4,"_np");
  if (L("arr"))
	N("sem texts",2) = L("arr");
@RULES
_xNIL <-
	_vg
	_np
	_conj
	_np
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [s one fail=(_xALPHA _verb _vg of _adj _adjc)]
	@@

# adj prep np vg
@CHECK
  if (N("pos50 a-p-n-v",4))
	fail();
  S("v") = N("verb node",4);
  if (!S("v"))
	fail();
  if (pnvar(S("v"),"mypos"))
	fail();
@POST
  N("pos50 a-p-n-v",4) = 1;
  fixvg(N(4),"passive","VBN");
@RULES
_xNIL <-
	_adj
	_prep
	_np
	_vg
	@@

# np , though alpha
@PRE
<5,5> var("verb");
@CHECK
  if (!vconjq(N(5),"-ing"))
	fail();
@POST
  alphatovg(5,"active","VBG");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	though [s]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	\, [lookahead]
	@@

# Ellipted pattersn.
# noun advl pro verb alpha
# "the cost of the goods they purchased rose"
@PRE
<2,2> var("pp");	# prep phrase.
<5,5> var("verb");
@CHECK
  if (!vconjq(N(5),"-ed"))
	fail();
@POST
  alphatovg(5,"active","VBP");
  fixverb(N(4),"active","VBP");
  group(3,4,"_clause");	# An embedded clause.
  setunsealed(3,"true");	# 07/10/12 AM.
  group(3,3,"_advl");
  setunsealed(3,"true");	# 07/10/12 AM.
  N("ellipted-that-clause",3) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_np _noun)]
	_advl
	_proSubj [s]
	_verb
	_xALPHA
	@@

# verb that noun that noun verb
@PRE
<1,1> varz("pos50 that-n-that");
@POST
  N("pos50 that-n-that",1) = 1;	# Loop guard.
  chpos(N(2),"DT");	# that/DT
  chpos(N(4),"IN");	# that/IN
@RULES
_xNIL <-
	_vg
	that [s lookahead]
	_np
	that [s]
	_np
	_vg
	@@

# det alpha alpha noun
@PRE
<2,2> var("noun");
<3,3> var("verb");
<3,3> varz("noun");
@POST
  alphatovg(3,"active","VBP");
  fixnphead(2);
@RULES
_xNIL <-
	_det
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_np _det _quan _num _xNUM)]
	@@

# ^ np vg np
@PRE
<5,5> varz("voice");
@POST
  fixvg(N(5),"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_vg
	_xWILD [star match=(_advl _adv)]
	_xWILD [one match=(_np _pro)]
	_xWILD [star match=(_advl _adv)]
	_xWILD [s one lookahead match=(to \, _conj)]
	@@

# verb that much
@PRE
<2,2> varz("pos50 v-that-much");
@POST
  N("pos50 v-that-much",2) = 1;	# Loop guard.
  chpos(N(2),"RB");	# that/RB
  chpos(N(3),"RB");	# much/RB
  pnrename(N(2),"_adv");
  pnrename(N(3),"_adv");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	that [s]
	much [s]
	@@

# verb prep np vg
@PRE
<4,4> varz("pos50 v-p-n-v");
@CHECK
  if (!vconjq(N(4),"-ed"))
	fail();
@POST
  N("pos50 v-p-n-v",4) = 1;
  fixvg(N(4),"active","VBP");
@RULES
_xNIL <-
	_verb
	_prep
	_np
	_vg
	@@

# parens.
@POST
  group(1,3,"_advl");
@RULES
_xNIL <-
	\(
	_xWILD [plus fail=(_xALPHA \) )]
	\)
	@@

# verb , adv alpha ,
# verb -- adv alpha --
@PRE
<4,4> var("verb");
@CHECK
  if (!vconjq(N(1),"-en"))
	fail();
@POST
  alphatovg(4,"passive","VBN");
  N("pos50 psv17",4) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	\,
	_adv
	_xALPHA
	\,
	@@
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_dbldash
	_adv
	_xALPHA
	_dbldash
	@@

# Note: ellipted-that patterns.
@CHECK
  if (!copulaq(N(3)))
	fail();
  if (!verbfeat(N(1),"T5"))
	fail();
@POST
  group(2,6,"_clause");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(2,2,"_advl");
  setunsealed(2,"true");	# 07/10/12 AM.
  N("ellipted-that",2) = 1;
@RULES
_xNIL <-
	_vg
	_xWILD [one match=(_np _pro)]
	_vg
	_xWILD [star match=(_adv _advl)]
	_adjc
	_np
	@@

# , adv adj
@POST
  group(2,3,"_adj");
@RULES
_xNIL <-
	\,
	_adv [plus]
	_adj
	@@

# vg np prep np conj dqan alpha
# vg prep np conj dqan alpha
@PRE
<10,10> var("noun");
@POST
  fixnphead(10);
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]						### (1)
    _np [opt]											### (2)
    _prep												### (3)
    _np													### (4)
    _conj												### (5)
    _xWILD [star match=(_det _pro)]						### (6)
    _xWILD [star match=(_quan _num _xNUM)]				### (7)
    _adj [star]											### (8)
    _noun [plus]										### (9)
    _xALPHA												### (10)
    _xWILD [one match=(_dbldash _prep _fnword _xEND)]	### (11)
    @@

# vg np prep np conj alpha
@PRE
<5,5> var("noun");
@POST
  fixnphead(5);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_np
	_conj
	_xALPHA
	_xWILD [one match=(_dbldash _xEND)]
	@@

# vg np prep np conj alpha noun
@PRE
<6,6> var("verb");
@CHECK
  L("v1") = N("verb node",1);
  if (!samevconj(L("v1"),N(6)))
	fail();
@POST
  alphatovg(6,"active","VBP");
@RULES
_xNIL <-
	_vg
	_np
	_prep
	_np
	_conj
	_xALPHA
	_noun [lookahead]
	@@

# np conj alpha $
# noun conj alpha $
@PRE
<3,3> var("noun");
@POST
  fixnphead(3);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_xEND
	@@
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_conj
	_xALPHA
	_xWILD [s one lookahead match=(who whom)]
	@@

# verb noun , noun alpha conj
# Note: noun lists.
@PRE
<5,5> var("noun");
@CHECK
  if (plural(N(2)) && plural(N(5)))
	succeed();
  if (singular(N(2)) && singular(N(5)))
	succeed();
  fail();
@POST
  fixnphead(5);
  group(4,5,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(_noun _np)]
	\,
	_noun
	_xALPHA
	_xWILD [one lookahead match=( _conj \, )]
	@@

# noun , noun conj alpha alpha
@PRE
<6,6> var("noun");
@CHECK
  if (!N("noun",5) && !N("adj",5))
	fail();
  if (plural(N(3)) && plural(N(6)))
	succeed();
  if (singular(N(3)) && singular(N(6)))
	succeed();
  fail();
@POST
  fixnphead(6);
  fixnpnonhead(5);
  group(5,6,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [plus match=( \, _conj)]
	_xWILD [one match=(_noun _np)]
	_xWILD [plus match=( \, _conj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=( \, _conj)]
	@@

# np , np and np
@POST
  group(1,5,"_np");
@RULES
_xNIL <-
	_np
	_xWILD [plus match=(\, _conj)]
	_np
	_xWILD [plus match=(\, _conj)]
	_np
	@@

# before vg np
@POST
  if (!N("mypos",1))
	chpos(N(1),"IN");
  group(1,3,"_clause");
  setunsealed(1,"true");
  group(1,1,"_advl");
  setunsealed(1,"true");
@RULES
_xNIL <-
	before [s]
	_vg
	_np
	_conj [lookahead]
	_vg
	@@

# ^ np , prep vg np , alpha
@PRE
<9,9> var("verb");
@CHECK
  if (!vconjq(N(9),"inf") && !vconjq(N(9),"-s")
	&& !vconjq(N(9),"-ed"))
	fail();
@POST
  alphatovg(9,"active","VBP");
@RULES
_xNIL <-
    _xWILD [one match=( _xSTART \, )]			### (1)
    _xWILD [one match=(_noun _np)]				### (2)
    \,											### (3)
    _prep										### (4)
    _vg											### (5)
    _np											### (6)
    _xWILD [star match=(_adv _advl)]			### (7)
    \,											### (8)
    _xALPHA										### (9)
    @@

# ^ np , alpha
@PRE
<5,5> var("verb");
<5,5> var("adj");
<5,5> vareq("pos num",2);
@POST
  if (vconjq(N(5),"-en") && verbfeat(N(5),"jj-pref"))
    alphatoadj(5);
  else
    alphatovg(5,0,0);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	@@

# , adv alpha ,
@PRE
<3,3> var("adj");
@CHECK
  if (N("verb",3))
	{
	if (!vconjq(N(3),"inf"))
		fail();
	}
@POST
  alphatoadj(3);
@RULES
_xNIL <-
	\,
	_adv [plus]
	_xALPHA
	\, [lookahead]
	@@

# verb prep np verb
@CHECK
  if (!copulaq(N(1)))
	fail();
  S("v") = N("verb node",5);
  if (pnvar(S("v"),"mypos"))
	fail();
  if (!vconjq(S("v"),"-en"))
	fail();
@POST
  fixvg(N(5),"passive","VBN");
  N("pos50 psv18",5) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_adv [star]
	_prep
	_np
	_vg
	@@

# noun adj
# num adj
@CHECK
  if (!N("num",2) && !N("possessive",3) && !N("cap",2))
   fail();
@POST
  pnrename(N(2),"_num");	# noun -> num
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_noun
	_xWILD [one match=(_adj _noun)]
	@@

# vg np prep np vg
# verb noun prep noun verb
# vg prep np vg
@PRE
<6,6> varz("pos50 vnpnv");
@CHECK
  if (!verbfeat(N(1),"D1"))
  	fail();
  if (pnname(N(6)) == "_vg")
  	{
	if (N("voice",6))
	  fail();
	}
  else if (N("mypos",6))
  	fail();
  if (!vconjq(N(6),"-en"))
	fail();
@POST
  N("pos50 vnpnv",6) = 1;
  fixvg(N(6),"passive","VBN");
  N("voice",6) = "passive"; # Loop guard too!
  N("pos50 psv19",6) = 1;
  N("ellipted-that-is",6) = 1;
  N("glom",6) = "right";
  N("glom",5) = "left";
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [star match=(_np _noun)]
	_xWILD [star match=(_adv _advl)]
	_prep
 	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	_xWILD [one lookahead match=(_np _noun _det _quan _num _xNUM _adj)]
 	@@

# vg prep vg
# vg to vg
# Note: May need semantic grounds, or at least trans/intr check.
@PRE
<2,2> varz("voice");
<3,3> vareq("stem","to");
@CHECK
  if (!vconjq(N(2),"-en"))
  	fail();
  if (phrprepverbq(N(2),N(3)))
  	fail();
  if (verbfeat(N(2),"intr") || verbfeat(N(2),"T3"))
  	fail();
@POST
  fixvg(N(2),"passive","VBN");
  N("pos50 psv20",2) = 1;
@RULES
_xNIL <-
	_xWILD [one fail=(_xALPHA _noun _np _conj)]
	_vg
	_prep
	_vg
	@@

# more adv than alpha conj noun
@CHECK
  if (!N("noun",4) && !N("unknown",4))
	fail();
@POST
  fixnphead(4);
@RULES
_xNIL <-
	more [s]
	_adv [plus]
	than [s]
 	_xALPHA
	_xWILD [plus lookahead match=(_conj \, )]
	_adv [star]
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# Note: use apposition to assume verb.
@PRE
<6,6> var("verb");
@CHECK
  # Todo: nvagree.
  L("n") = lasteltnode(5);
  if (plural(L("n")) && vconjq(N(6),"-s"))
	fail();
  if (singular(L("n")) && vconjq(N(6),"inf"))
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
	_prep [lookahead]
	_np
	\,
	_xWILD [one match=(_det _quan _num _xNUM _adj _noun _np)]
	@@

# noun alpha
@PRE
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"-ing"))
	fail();
  if (!plural(N(1)))
	fail();
@POST
  alphatovg(2,"active","VBG");
@RULES
_xNIL <-
	_noun
	_xALPHA
	@@

# Note: Need more general embedded clause handling.
@PRE
<4,4> varz("pos50 n-that-v-adj");
@POST
  N("pos50 n-that-v-adj",4) = 1;	# Loop guard.
  if (!N("mypos",3))
	chpos(N(3),"WDT");	# that/WDT
  if (N("mypos",5) == "VBG")
	chpos(N(5),"JJ");	# vbg -> jj
  group(2,5,"_clause");
  setunsealed(2,"true");	# 07/10/12 AM.
  group(1,3,"_advl");
  setunsealed(1,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_xWILD [one match=(_dbldash)]
	_np
	that [s]
	_vg
	_adj
	_xWILD [one match=(_dbldash)]
	@@

# --
@POST
  group(1,3,"_advl");
@RULES
_xNIL <-
	_dbldash
	_xWILD [plus fail=(_xALPHA _verb _noun _dbldash)]
	_dbldash
	@@

# advl who , vg
# np who , vg
@PRE
<4,4> varz("voice");
@CHECK
  if (!vconjq(N(4),"-en"))
	fail();
@POST
  N("voice",4) = "passive";
  fixvg(N(4),"passive","VBN");
  N("pos50 psv21",4) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_advl _np)]
	who [s lookahead]
	\,
	_vg
	@@

# np , alpha np
# noun , alpha noun
# Note: A kind of apposition with a verb.
@PRE
<3,3> var("verb");
@CHECK
  if (!N("cap",1) && !N("cap",4))
	fail();	# Some appos validation.
#  if (!vconjq(N(3),"-en"))
#	fail();
@POST
  if (vconjq(N(3),"-en")
   && N("adj",3) && verbfeat(N(3),"jj-pref"))
  	alphatoadj(3);
  else
    alphatovg(3,0,0);
  N("pos50 psv22",3) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=( \, \( )]	# 04/25/10 AM.
	_xALPHA
	_xWILD [one lookahead match=(_noun _np)]	# 04/25/10 AM.
	@@

# vg prep prep np alpha
@PRE
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"-ing"))
	fail();
@POST
  alphatovg(6,"active","VBG");
@RULES
_xNIL <-
	_vg
	_prep
	_prep
	_np
	_adv [star]
	_xALPHA
	_xWILD [one lookahead match=(_conj)]
	@@

# np , vg np , alpha
# Add restrictions as needed.	# 06/02/07 AM.
@PRE
<11,11> var("verb");
@POST
  if (vconjq(N(11),"-en")
   && N("adj",11) && verbfeat(N(11),"jj-pref"))
  	alphatoadj(11);
  else
    alphatovg(11,0,0);
  N("pos50 n,vn,a");
@RULES
_xNIL <-
    _np									### (1)
    _xWILD [star match=(_adv _advl)]	### (2)
    \,									### (3)
    _xWILD [star match=(_adv _advl)]	### (4)
    _vg									### (5)
    _xWILD [star match=(_adv _advl)]	### (6)
    _np									### (7)
    _xWILD [star match=(_adv _advl)]	### (8)
    \,									### (9)
    _xWILD [star match=(_adv _advl)]	### (10)
    _xALPHA								### (11)
    @@

# vg np , alpha np
# vg np , alpha dqan
@PRE
<4,4> var("verb");
@POST
  if (vconjq(N(4),"inf"))
	alphatovg(4,"active","VB");
  else
	alphatovg(4,0,0);
@RULES
_xNIL <-
	_vg
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	_xWILD [one match=(_np _det _pro _pdt _quan _num _xNUM)]
	@@

# vg np prep np , alpha alpha that
@PRE
<6,6> var("verb");
<7,7> var("noun");
@CHECK
  if (!vconjq(N(6),"-ing"))
	fail();
@POST
  fixnphead(7);
  alphatovg(6,"active","VBG");
@RULES
_xNIL <-
	_vg
	_np
	_prep
	_xWILD [one match=(_noun _np)]
	\,
	_xALPHA
	_xALPHA
	that [s lookahead]
	@@

# vg np prep np , alpha noun ,  vg
# Note: Trying to decide on verb list.
@PRE
<6,6> var("verb");
@CHECK
  if (samevconj(N(1),N(6)))
	{
    S("voice") = N("voice",1);
	L("v") = N("verb node",1);
    S("pos") = pnvar(L("v"),"mypos");
  	succeed();
	}
  if (samevconj(N(6),N(9)))
	{
    S("voice") = N("voice",9);
	L("v") = N("verb node",9);
    S("pos") = pnvar(L("v"),"mypos");
  	succeed();
	}
  fail();
@POST
  alphatovg(6,S("voice"),S("pos"));
@RULES
_xNIL <-
    _vg											### (1)
    _np											### (2)
    _prep										### (3)
    _xWILD [one match=(_noun _np)]				### (4)
    \,											### (5)
    _xALPHA										### (6)
    _xWILD [one lookahead match=(_np _noun)]	### (7)
    \,											### (8)
    _xWILD [one match=(_verb _vg)]				### (9)
    @@


# verb alpha , alpha conj adv
@PRE
<2,2> var("adv");
<4,4> var("adv");
@POST
  alphatoadv(4);
  alphatoadv(2);
  group(2,6,"_advl");
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	\,
	_xALPHA
	_xWILD [plus match=( \, _conj)]
	_adv
	_xWILD [one lookahead match=(_xEND)]
	@@

# prep dqa , alpha
# adj , alpha
@PRE
<6,6> var("adj");
@POST
  alphatoadj(6);	# 06/08/07 AM.
@RULES
_xNIL <-
    _xWILD [one match=(_prep)]				### (1)
    _xWILD [star match=(_det _pro)]			### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [plus]								### (4)
    \,										### (5)
    _xALPHA									### (6)
    @@

# det adj , alpha alpha
@PRE
<4,4> varz("verb");
<4,4> var("adj");
<5,5> varz("verb");
<5,5> var("noun");
@POST
  fixnpnonhead(4);
@RULES
_xNIL <-
	_det
	_adj
	\,
	_xALPHA
	_xALPHA
	@@

# vg np advl vg
@PRE
<4,4> vareq("voice","passive");
@POST
  group(4,5,"_clause");
  setunsealed(4,"true");	# 07/10/12 AM.
  group(4,4,"_advl");
  setunsealed(4,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_vg
	_np
	_advl [plus]
	_vg
	_advl [star]
	@@

# vg np advl that np vg np
@PRE
<4,4> varz("mypos");
@POST
  chpos(N(4),"IN");	# that/IN
  N("pos50 vna-that-nvn",4) = 1;
@RULES
_xNIL <-
	_vg
	_np
	_advl [plus]
	that [s]
	_np [lookahead]
	_vg
	_np
	@@

# vg np advl that
#@PRE
#<4,4> varz("mypos");
#@POST
#  chpos(N(4),"WDT");	# that/WDT
#  N("pos50 vna-that",4) = 1;
#@RULES
#_xNIL <-
#	_vg
#	_np
#	_advl [plus]
#	that [s]
#	@@

# , adv prep np of np alpha prep np ,
@PRE
<7,7> var("verb");
@POST
  alphatovg(7,0,"VBP");
@RULES
_xNIL <-
	\,
	_adv [plus]
	_prep
	_np
	of [s]
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_prep
	_np
	\,
	@@

# , det noun noun verb
@PRE
<4,4> var("cap");
<5,5> varz("mypos");
@POST
  fixverb(N(5),"active","VBP");
@RULES
_xNIL <-
	\,
	_det
	_noun
	_noun
	_verb
	_xWILD [one lookahead match=(_dbldash)]
	@@

# np vg that adj
# Note: adverbial 'that'
@PRE
<5,5> varz("mypos");
<5,5> varz("that-adv");
@CHECK
  if (!copulaq(N(3)))
  	fail();
@POST
  N("that-adv",5) = 1;
  chpos(N(5),"RB");	# that/RB.
  pnrename(N(5),"_adv");	# that -> adv. fnword -> adv.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	_adv [star]
	that [s]
	_xWILD [one lookahead match=(_adj _adjc)]
	_xWILD [one match=(_prep _fnword)]
	@@
	
# np vg that
# noun verb that
@PRE
<3,3> varz("pos50 nvthat");
@CHECK
  L("v") = N("verb node",3);
  if (!L("v"))
    {
	"err.txt" << "No verb node=" << phrasetext() << "\n";
	fail();	# 03/04/10 AM.
	}
  if (pnvar(L("v"),"mypos"))
	fail();
@POST
  N("glom",2) = "right";
  fixvg(N(3),"active","VBP");
  N("pos50 nvthat",3) = 1;	# Fix.	# 10/14/10 AM.
@RULES
_xNIL <-
	_xWILD [one match=( \, _conj)]
	_np
	_vg
	that [s]
	@@

# vg prep dqa alpha alpha prep vg
# Note: Context for favoring noun phrase.
@PRE
<3,3> varz("prosubj");
<3,3> varz("proobj");
<7,7> var("noun");
@CHECK
  if (!N("noun",6) && !N("adj",6))
  	fail();
@POST
  fixnphead(7);
  fixnpnonhead(6);
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]			### (1)
    _prep									### (2)
    _xWILD [star match=(_det _pro)]			### (3)
    _xWILD [star match=(_quan _num _xNUM)]	### (4)
    _adj [star]								### (5)
    _xALPHA									### (6)
    _xALPHA									### (7)
    _prep [lookahead]						### (8)
    _xWILD [one match=(_verb _vg _modal)]	### (9)
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
  N("pos50 psv23",1) = 1;
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

# verb prep dqan conj alpha
# vg prep dqan conj alpha
@PRE
<8,8> var("noun");
@CHECK
  L("n") = lasteltnode(6);
  if (plural(L("n")) && !plural(N(8)))
	fail();
  if (singular(L("n")) && !singular(N(8)))
	fail();
  if (N("verb",8))
	{
	if (vconj(N(1)) == vconj(N(8)))
	  fail();	# Parallel construction.
	if (phrprepverbq(N(8),N(9)))
	  fail();	# Verb-particle good combo.
	}
@POST
  fixnphead(8);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_conj
	_xALPHA
	_xWILD [one match=(_prep)]
	@@

# vg np prep dqan conj alpha np
@PRE
<9,9> var("verb");
@CHECK
  if (pnname(N(10)) == "_noun"
   && plural(N(10))
   && vconjq(N(9),"inf"))
  	{
	if (N("adj",9) || N("noun",9))
  	  fail();	# Could still be noun or adj.
	}
  S("vc") = vconj(N(1));
  if (S("vc") != vconj(N(9)))
	fail();	
@POST
  L("v") = N("verb node",1);
  if (L("v"))
	L("pos") = pnvar(L("v"),"mypos");
  alphatovg(9,N("voice",1),L("pos"));
@RULES
_xNIL <-
    _vg											### (1)
    _np											### (2)
    _prep										### (3)
    _xWILD [star match=(_det _pro)]				### (4)
    _xWILD [star match=(_quan _num _xNUM)]		### (5)
    _adj [star]									### (6)
    _noun [plus]								### (7)
    _conj										### (8)
    _xALPHA										### (9)
    _xWILD [one lookahead match=(_np _noun)]	### (10)
    @@

# alpha np np
@PRE
<1,1> var("verb");
@CHECK
  if (!verbfeat(N(1),"D1"))
  	fail();
@POST
  alphatoverb(1,"active",0);
@RULES
_xNIL <-
	_xALPHA
	_np [lookahead]
	_xWILD [one match=(_np _det _pro _quan _num _xNUM _adj _noun)]
	@@

# of np vg
@CHECK
  L("v") = N("verb node",3);
  if (!L("v"))
  	fail();
  if (pnvar(L("v"),"mypos"))
  	fail();
  if (!vconjq(L("v"),"-en"))
  	fail();
@POST
  fixvg(N(3),"passive","VBN");
  N("pos50 psv24",3) = 1;
@RULES
_xNIL <-
	of [s]
	_np
	_vg
	_xWILD [s one lookahead match=(that by)]
	@@

# prep dqan alpha that
@CHECK
  if (!N("verb",6) && !N("adj",6))
  	fail();
  if (N("noun",6) && numbersagree(N(1),N(6)))
  	fail();
@POST
  if (N("verb",6))
  	alphatovg(6,0,0);
  else
  	{
	alphatoadj(6);
	group(6,6,"_adjc");
	}
@RULES
_xNIL <-
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [s one lookahead match=(that)]
	@@

# np noun alpha verb
@PRE
<2,2> var("cap");
<3,3> var("verb");
@POST
  nountonp(2,1);
  alphatovg(3,"active","VBP");
  group(2,3,"_clause");
  setunsealed(2,"true");	# 07/10/12 AM.
  N("ellipted-that",2) = 1;
  group(2,2,"_advl");
  setunsealed(2,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_np
	_noun
	_xALPHA
	_vg
	@@

# noun , dqan conj alpha+ prep
# Apposition
@CHECK
  if (N(10))
    {
  	if (!N("noun",10))
	  fail();
  	if (!N("noun",9) && !N("adj",9))
	  fail();
  	if (!N("noun",8) && !N("adj",8))
	  fail();
	}
  else if (N(9))
    {
  	if (!N("noun",9))
	  fail();
  	if (!N("noun",8) && !N("adj",8))
	  fail();
	}
  else if (!N("noun",8))
  	fail();
@POST
  if (N(10))
    {
	fixnphead(10);
	fixnpnonhead(9);
	fixnpnonhead(8);
	}
  else if (N(9))
    {
	fixnphead(9);
	fixnpnonhead(8);
	}
  else
    {
	fixnphead(8);
	}
@RULES
_xNIL <-
    _xWILD [one match=(_noun _np)]						### (1)
    \,													### (2)
    _xWILD [star match=(_det _pro)]						### (3)
    _xWILD [star match=(_quan _num _xNUM)]				### (4)
    _adj [star]											### (5)
    _noun [plus]										### (6)
    _conj												### (7)
    _xALPHA												### (8)
    _xALPHA [opt]										### (9)
    _xALPHA [opt]										### (10)
    _xWILD	[one lookahead match=(_prep \, )]	### (11)
    @@

# Apposition
# np , dqa , dqa alpha ,
@PRE
<7,7> var("noun");
@CHECK
  if (!numbersagree(N(3),N(7)))
  	fail();
@POST
  fixnphead(7);
  group(4,6,"_adj");
@RULES
_xNIL <-
    _xWILD [one match=(_noun _np)]	### (1)
    \,								### (2)
    _det							### (3)
    _adj							### (4)
    \,								### (5)
    _adj							### (6)
    _xALPHA							### (7)
    \, [lookahead]					### (8)
    @@

# aposS alpha , noun ,
@PRE
<2,2> var("noun");
@CHECK
  if (!singular(N(2)))
	fail();
@POST
  fixnphead(2);
@RULES
_xNIL <-
	_xWILD [one match=(_aposS)]
	_xALPHA
	\, [lookahead]
	_xWILD [one match=(_noun _np)]
	\,
	@@

# Apposition
# np vg alpha
@PRE
<2,2> varz("pos50 nvathat");
<2,2> vareq("voice","passive");
<4,4> var("verb");
@CHECK
  # Should be verbfeat(N(4),"T5")
  succeed();
@POST
  N("pos50 nvathat",2) = 1;
  alphatovg(4,"active","VBP");
  N("glom",4) = "right";
  if (!N("mypos",5))
    chpos(N(5),"IN");	# that/IN
@RULES
_xNIL <-
	_np
	_vg
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [s one lookahead match=(that)]
	@@

# np for np , alpha for np
# Note: parallel construction.
@PRE
<5,5> var("noun");
@POST
  fixnphead(5);
@RULES
_xNIL <-
	_np
	_prep
	_np
	\,
	_xALPHA
	_prep
	_np
	@@

# ^ alpha+ verb
@CHECK
  if (N(5))
    {
	if (!N("noun",5))
	  fail();
	S("o") = 5;
	}
  else if (N(4))
    {
	if (!N("noun",4))
	  fail();
	S("o") = 4;
	}
  else if (N(3))
    {
	if (!N("noun",3))
	  fail();
	S("o") = 3;
	}
  else
    {
	if (!N("noun",2))
	  fail();
	S("o") = 2;
	}
@POST
 L("n") = fixnphead(S("o"));
 while (--S("o") > 1)
   {
   L("n") = pnprev(L("n"));
   if (pnvar(L("n"),"noun") || pnvar(L("n"),"adj"))
     L("n") = fixnpnonhead(S("o"));
   else
     S("o") = 1;  # Terminate loop.
   }
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA [opt]
	_xALPHA [opt]
	_xALPHA [opt]
	_xWILD [star lookahead match=(_adv _advl)]
	_vg
	@@

# vg np so much
@PRE
<4,4> varz("mypos");
@POST
  alphatoadv(4);
  pnrename(N(3),"_adv");	# so -> adv
@RULES
_xNIL <-
	_vg
	_np
	so [s]
	much
	@@

# be prep noun conj verb
# Note: Remote linkage.
@PRE
<1,1> vareq("stem","be");
<1,1> varz("v-v-remote");
<6,6> varz("mypos");
@CHECK
  if (!vconjq(N(6),"-en"))
  	fail();
@POST
  fixverb(N(6),"passive","VBN");
  N("v-v-remote",1) = N("v-v-remote",6) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_adv [star]
	_prep
	_noun
	_xWILD [plus match=( \, _conj)]
	_verb
	@@

# Apposition
# noun , dqan 's alpha
@PRE
<1,1> var("cap");
<8,8> var("noun");
@POST
  fixnphead(8);
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_num _quan _xNUM)]
	_adj [star]
	_noun [plus]
	_aposS
	_xALPHA
	_xWILD [one lookahead match=( \, _xEND _dbldash)]
	@@

# dqan adj , adj , noun
@POST
  group(2,6,"_adj");
@RULES
_xNIL <-
	_xWILD [one match=(_det _pro _quan _num _xNUM _adj)]
	_adj
	_xWILD [plus match=( \, _conj)]
	_adv [star]
	_adj
	\, [opt]
	_noun [lookahead]
	@@

# to dqan alpha $
# verb dqan alpha $
@CHECK
  dqaninfo(3,4,5,6);
  if (!numbersagree(S("first"),N(7)))
	fail();
@POST
  fixnphead(7);
@RULES
_xNIL <-
	_xWILD [s one match=(_vg _verb to)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [opt match=(_det _pro)]
 	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_dbldash _xEND \, )]
	@@

# the noun conj alpha that
@PRE
<4,4> var("noun");
@CHECK
  if (!plural(N(4)))
  	fail();
@POST
  fixnphead(4);
@RULES
_xNIL <-
	the [s]
	_noun
	_conj
	_xALPHA
	that [s lookahead]
	@@

# verb prep conj alpha
@PRE
<4,4> var("verb");
@CHECK
  if (pnname(N(1)) == "_vg")
    L("v") = N("verb node",1);
  else
    L("v") = N(1);
  S("pos") = pnvar(L("v"),"mypos");
  if (!S("pos"))
  	fail();
  L("m") = vtreebanktopos(S("pos"));
  if (!vconjq(N(4),L("m")))
  	fail();
@POST
  alphatovg(4,0,S("pos"));
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_prep
	_conj
	_xALPHA
	@@

# np , dqan alpha to vg
# Note: Appositive like, ellipted-copula.
@PRE
<8,8> vareq("stem","to");
@CHECK
  if (!N("adj",7) && !N("verb",7))
  	fail();
  if (N("verb",7) && !N("adj",7))
  	{
	if (!vconjq(N(7),"-ing") && !vconjq(N(7),"-en"))
		fail();
	}
@POST
  if (N("verb",7))
  	{
	if (vconjq(N(7),"-ing"))
  	  alphatovg(7,"active","VBG");
	else
	  alphatovg(7,"passive","VBN");
	N("ellipted-that-is",7) = 1;
	N("pos50 n-,-n-a-to-v",7) = 1;
	}
  else
  	{
  	alphatoadj(7);
	N("ellipted-copula",7) = 1;
	}
@RULES
_xNIL <-
    _xWILD [one match=(_noun _np)]			### (1)
    _xWILD [plus match=(_conj \, )]			### (2)
    _xWILD [star match=(_det _pro)]			### (3)
    _xWILD [star match=(_quan _num _xNUM)]	### (4)
    _adj [star]								### (5)
    _noun [plus]							### (6)
    _xALPHA									### (7)
    _prep									### (8)
    _xWILD [one match=(_verb _vg _modal)]	### (9)
    @@

# np that vg
# noun that verb
@PRE
<2,2> varz("mypos");
@CHECK
  if (N("voice",4) != "active")
  	fail();
  if (!vgassigned(N(4)))
  	fail();
@POST
  L("pos") = vgpos(N(4));
  if (L("pos") == "VBG")
  	chpos(N(2),"IN");	# that/IN.	# 06/02/07 AM.
  else
    chpos(N(2),"WDT");	# that/WDT	# 05/22/07 AM.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	that [s]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one lookahead match=(_verb _vg _modal)]
	@@

# , prep dqan , alpha
@POST
  if (N("stem",2) == "by")
  	L("by-np") = 1;
  dqaninfo(3,4,5,6);
  groupnp();
  group(1,4,"_advl");	# prep np -> advl
  N("pp",2) = 1;
  if (L("by-np"))
  	N("by-np",2) = 1;
@RULES
_xNIL <-
	\,
	_prep
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	\,
	_xALPHA
	_xWILD [one lookahead match=(_prep _fnword _xEND)]
	@@

# vg prep dqan conj alpha* vg
@PRE
<9,9> var("noun");
@CHECK
  if (!N("adj",8) && !N("noun",8))
  	fail();
  if (!plural(N(9)))
  	fail();
@POST
  fixnphead(9);
  fixnpnonhead(8);
  group(7,7,"_fnword");	# Keep nps from glomming.
@RULES
_xNIL <-
    _vg										### (1)
    _prep									### (2)
    _xWILD [star match=(_det _pro)]			### (3)
    _xWILD [star match=(_quan _num _xNUM)]	### (4)
    _adj [star]								### (5)
    _noun [plus]							### (6)
    _conj									### (7)
    _xALPHA									### (8)
    _xALPHA									### (9)
    _vg										### (10)
    @@

# np vg dqan alpha
# vg dqan alpha
@PRE
<3,3> varz("pos50 n-v-dqan-alpha-p");
@POST
  N("pos50 n-v-dqan-alpha-p",3) = 1;
  if (N("adj",9) && verbfeat(N(3),"X7"))
  	alphatovg(9,"active","VB");
  else if (N("noun",9) && N("verb",9))
  	{
	if (vconjq(N(9),"-en")) # (ambig verb...)
	  alphatovg(9,"passive","VBN");
	else if (verbfeat(N(3),"V2") && vconjq(N(9),"inf"))
	  alphatovg(9,"active","VB");
	else if (vconjq(N(9),"inf") || vconjq(N(9),"-s"))
	  fixnphead(9);
    }
  else if (N("adj",9) && N("adv",9))
    {
	if (!N("noun",0) && !N("verb",9))
	  alphatoadv(9);
	}
@RULES
_xNIL <-
    _np [opt]									### (1)
    _xWILD [star match=(_adv _advl)]			### (2)
    _vg											### (3)
    _adv [star]									### (4)
    _xWILD [star match=(_det _pro)]				### (5)
    _xWILD [star match=(_quan _num _xNUM)]		### (6)
    _adj [star]									### (7)
    _noun [plus]								### (8)
    _xALPHA										### (9)
    _xWILD [one lookahead match=(_prep _conj)]	### (10)
    @@

# ^ following np by
@POST
  chpos(N(2),"IN");	# following/IN [NO Conform Treebank]
  group(2,2,"_fnword");
@RULES
_xNIL <-
	_xSTART
	following
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	by [s lookahead]
	@@

# ^ np , noun conj np , alpha
# Note: Apposition.
@CHECK
  if (N("verb",8) && !N("adj",8))
  	fail();
@POST
  group(4,6,"_np");
  group(2,5,"_np");
@RULES
_xNIL <-
    _xSTART							### (1)
    _np								### (2)
    \,								### (3)
    _xWILD [one match=(_noun _np)]	### (4)
    _conj							### (5)
    _xWILD [one match=(_noun _np)]	### (6)
    \, [lookahead]					### (7)
    _xALPHA							### (8)
    @@

# ^ np alpha
@PRE
<3,3> var("verb");
@POST
  alphatovg(3,"active","VBP");
@RULES
_xNIL <-
	_xSTART
	_np
	_xALPHA
	@@

# ^ np prep dqan alpha
@PRE
<9,9> var("verb");
@CHECK
  if (literal(N(10)))
    {
    if (!N("noun",10) && !N("adj"))
      fail();
	}
@POST
  alphatovg(9,"active","VBP");
@RULES
_xNIL <-
    _xSTART									### (1)
    _xWILD [star match=(_adv _advl)]		### (2)
    _xWILD [one match=(_noun _np _det)]		### (3)
    _prep									### (4)
    _xWILD [plus match=(_det _pro)]			### (5)
    _xWILD [star match=(_quan _num _xNUM)]	### (6)
    _adj [star]								### (7)
    _noun [plus]							### (8)
    _xALPHA									### (9)
    _xWILD [one lookahead match=(_xALPHA _det _quan _num _xNUM
		_adj _noun _np)]
    @@



# vg np conj np vg prep
@PRE
<6,6> varz("voice");
@CHECK
  if (!vconjq(N(6),"-en"))
  	fail();
  if (!vconjq(N(1),"-ing"))
  	fail();
  # Want "ellipted verb", no auxiliaries on it.
  L("n") = pndown(N(1));
  if (pnname(L("n")) != "_verb")
  	fail();
  L("n") = pndown(L("n"));
  if (!literal(L("n")))
  	fail();
@POST
  fixvg(N(6),"passive","VBN");
  N("pos50 psv25",6) = 1;
@RULES
_xNIL <-
    _vg									### (1)
    _np [lookahead]						### (2)
    _conj								### (3)
    _np									### (4)
    _xWILD [star match=(_adv _advl)]	### (5)
    _vg									### (6)
    _xWILD [one match=(_prep)]			### (7)
    @@

# vg np conj vg
@PRE
<4,4> varz("pos50 vn-conj-v");
@CHECK
  if (!vgassigned(N(1)) || vgassigned(N(4)))
  	fail();
@POST
  N("pos50 vn-conj-v",4) = 1;
  if (N("compound-vg",1))
	L("vg") = N("last vg",1);
  else
    L("vg") = N(1);
  L("pos") = vgpos(L("vg"));
  L("voice") = pnvar(L("vg"),"voice");
  fixvg(N(4),L("voice"),L("pos"));
@RULES
_xNIL <-
	_vg
	_np
	_conj
	_vg
	@@

# , np vg ,
@POST
  group(4,4,"_clausesep");
  group(1,1,"_clausesep");
@RULES
_xNIL <-
	\,
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=(_verb _vg)]
	\,
	@@

# ^ np , clause , vg
@PRE
<8,8> varz("voice");
@POST
  fixvg(N(8),"active","VBD");
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_np
	_clausesep
	_xWILD [star match=(_noun _np _verb _vg _adv _advl _prep)]
	_clausesep
	_xWILD [star match=(_adv _advl)]
	_vg
	@@

# vg by np as np vg
@CHECK
  if (vgassigned(N(6)))	# New fn. # 05/27/07 AM.
  	fail();
@POST
  fixvg(N(6),"passive","VBN");
  N("pos50 psv26",6) = 1;
  N("glom",6) = "right";
  N("glom",5) = "left";
  if (pnname(N(4)) == "_fnword" && !N("renamed",4))
    {
  	pnrename(N(4),"_prep");	# as -> prep
	N("renamed",4) = 1;
	}
@RULES
_xNIL <-
	_vg
	by [s]
	_np
	as [s]
	_np [lookahead]
	_vg
	@@

# vg prep np by np
@PRE
<2,2> varz("pos50 zzz");
@POST
  N("zzz",2) = 1;
  L("num") = phrprepverbq(N(2),N(3));
  if (L("num") == 1 || L("num") == 3)
  	{
	if (N("stem",3) != "to")
	  chpos(N(3),"RP");	# prep/RP.
	groupvgprep(2,3);
	}
  else if (L("num") == 2)
  	{
	if (N("stem",3) != "to")
	  chpos(N(3),"IN");	# prep/IN.
	groupvgprep(2,3);
	}
  else
  	{
	if (!vgassigned(N(2)))
	  {
	  if ((N("stem",3) == "by" && semactornode(N(4)))
	   || (N("stem",5) == "by" && semactornode(N(6))) )
	    {
	    if (vconjq(N(2),"-en"))
	      fixvg(N(2),"passive","VBN");
		else
		  fixvg(N(2),"active","VBP");
		}
	  }
    prepnptoadvl(3,4);	# prep np -> advl
	}
@RULES
_xNIL <-
	_xWILD [plus match=( \, _conj _np)]
	_vg
	_prep
	_np
	_prep
	_xWILD [one match=(_noun _np)]
	@@


# vg by np
@PRE
<1,1> varz("pos50 psvx1");
<1,1> varz("voice");
<3,3> vareq("stem","by");
@CHECK
  if (!semactornode(N(5)))
  	fail();
@POST
  N("pos50 psvx1",1) = 1;
  fixvg(N(1),"passive","VBN");
@RULES
_xNIL <-
	_vg
	_xWILD [star lookahead match=(_adv _advl)]
	_prep
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_np)]
	@@

# prep noun , conj vg
@POST
  nountonp(2,1);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_noun
	_xWILD [plus lookahead match=( \, _conj)]
	_vg
	@@

# np , as alpha ,
@PRE
<4,4> var("verb");
@CHECK
  if (!vconjq(N(4),"-en"))
  	fail();
@POST
  alphatovg(4,"passive","VBN");
  N("pos50 psv27",4) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	\,
	as [s]
	_xALPHA
	\, [lookahead]
	@@

# vg that noun
@CHECK
  if (!plural(N(3)) && N("mypos",2) != "IN")
  	fail();
@POST
  dqaninfo(0,0,0,3);
  groupnp();
@RULES
_xNIL <-
	_vg
	that [s]
	_noun
	_xWILD [one lookahead match=(_prep)]
	@@

# that np prep np and np
@POST
  group(3,4,"_advl");
  N("pp",3) = 1;
  group(2,5,"_np");	# List of nps.
@RULES
_xNIL <-
	that [s]
	_np
	_prep
	_np
	_conj
	_np
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_verb _vg)]
	@@

# alpha dqan alpha
@PRE
<1,1> var("verb");
<6,6> var("verb");
@CHECK
  if (!vconjq(N(6),"inf"))
  	fail();
  if (!verbfeat(N(1),"V2"))	# eg, let x go
  	fail();
@POST
  alphatovg(6,"active","VB");
  alphatovg(1,"active",0);
@RULES
_xNIL <-
    _xALPHA									### (1)
    _xWILD [star match=(_det _pro)]			### (2)
    _xWILD [star match=(_quan _num _xNUM)]	### (3)
    _adj [star]								### (4)
    _noun [plus]							### (5)
    _xALPHA									### (6)
    @@

# vg np before vg np or alpha
@PRE
<7,7> var("verb");
@CHECK
  S("vc") = samevconj(N(1),N(7));
  if (S("vc"))
    {
	L("v") = N("verb node",1);
	S("pos") = pnvar(L("v"),"mypos");
  	succeed();
	}
  S("vc") = samevconj(N(4),N(7));
  if (S("vc"))
    {
	L("v") = N("verb node",4);
	S("pos") = pnvar(L("v"),"mypos");
  	succeed();
	}
  fail();
@POST
  alphatovg(7,0,S("pos"));
@RULES
_xNIL <-
    _vg									### (1)
    _np									### (2)
    before [s]							### (3)
    _vg									### (4)
    _np									### (5)
    or [s]								### (6)
    _xALPHA								### (7)
    _xWILD [one lookahead match=(_np)]	### (8)
    @@

# of alpha np
# of alpha dqan
@PRE
<2,2> var("verb");
@CHECK
  if (!vconjq(N(2),"-ing"))
  	fail();
@POST
  alphatovg(2,"active","VBG");
@RULES
_xNIL <-
	of [s]
	_xALPHA
	_xWILD [one lookahead match=(_np _det _pro _quan _num _xNUM)]
	@@

# Note: no x, no y = implicit there is.
@PRE
<2,2> varz("pos50 np-,-np");
<2,2> var("neg");
@POST
  N("pos50 np-,-np",2) = 1;
  X("implicit-there-is") = 1;
@RULES
_xNIL <-
	_xSTART
	_np
	\, [lookahead]
	_np
	@@

# np vg prep prep np
@PRE
<1,1> var("neg");
<2,2> varz("voice");
@CHECK
  if (!X("implicit-there-is"))
  	fail();
  if (!vconjq(N(2),"-en"))
  	fail();
@POST
  fixvg(N(2),"passive","VBN");
  N("pos50 psv28",2) = 1;
@RULES
_xNIL <-
	_np
	_vg
	_prep
	_prep
	_np
	@@

# np , np conj np , vg
@POST
  group(2,7,"_np");
  N("apposition",2) = 1;
  N("pos50 n,n&n,v",2) = 1;
@RULES
_xNIL <-
    _xWILD [one match=( \, )]		### (1)
    _np								### (2)
    \,								### (3)
    _xWILD [one match=(_noun _np)]	### (4)
    _conj							### (5)
    _xWILD [one match=(_noun _np)]	### (6)
    \,								### (7)
    _adv [star lookahead]			### (8)
    _vg								### (9)
    @@

# , dqan alpha
@PRE
<3,3> varz("prosubj");
<3,3> varz("proobj");
<7,7> var("noun");
@CHECK
  L("n") = lasteltnode(6);
  if (plural(L("n")))
  	fail();
  if (N(3))
    L("first") = N(3);
  else if (N(4))
    L("first") = N(4);
  else if (N(5))
    L("first") = N(5);
  else
   L("first") = N(6);
  if (!numbersagree(L("first"),N(7)))
  	fail();
  if (plural(N(7)) && !N(4))
  	fail();
  if (N("verb",7))
    {
	if (vconjq(N(7),"-ed") || vconjq(N(7),"-ing"))
		fail();
	}
@POST
  fixnphead(7);
@RULES
_xNIL <-
    \,												### (1)
    _xWILD [star match=(_adv _advl)]				### (2)
    _xWILD [star match=(_det _pro)]					### (3)
    _xWILD [star match=(_quan _num _xNUM)]			### (4)
    _adj [star]										### (5)
    _noun [plus]									### (6)
    _xALPHA											### (7)
    _xWILD [one lookahead fail=(_xALPHA _noun _np
        _det _quan _num _xNUM _adj)]				### (8)
    @@

# np that np vg
# Note: A specialized subjunctive pattern.
# decree that John eat his peas.
@CHECK
  if (vgassigned(N(4)))
  	fail();
  L("t") = nodestem(N(1));
  if (!finddictattr(L("t"),"that-subjunctive"))
  	fail();
  if (!vconjq(N(4),"inf"))
  	fail();
@POST
  fixvg(N(4),"active","VB");	# Subjunctive.
  N("that-subjunctive",1) = 1;
  N("that-subjunctive",2) = 1;
  N("that-subjunctive",4) = 1;
  if (!N("mypos",2))
  	chpos(N(2),"IN");	# that/IN
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	that [s]
	_np
	_vg
	@@

# det noun , adj noun
# det cap , adj noun
@PRE
<2,2> var("cap");
@POST
  pnrename(N(2),"_adj");	# noun -> adj
  group(2,4,"_adj");	# adj list
@RULES
_xNIL <-
	_xWILD [s one match=(a an the)]
	_noun
	\,
	_adj
	_noun [lookahead]
	@@

# vg pro alpha alpha
# vg her alpha alpha [vg np alpha alpha]
# her
# Note: 'her' is ambiguous PP/PP$
@PRE
<2,2> vareq("stem","her");
<3,3> var("verb");
<3,3> varz("pos50 vg-her-aa");
@CHECK
  if (!vconjq(N(3),"-ing"))
  	fail();
@POST
  N("pos50 vg-her-aa",3) = 1; # Loop guard.
  if (!N("noun",3) && N("adj",3) && (N("noun",4) || N("adj",4)))
  	{
	fixnpnonhead(3);
	}
  else if (N("noun",3) && N("adj",4))
  	{
	# Still ambig, but may be a predicate adjective.
	alphatoadj(4);
	group(4,4,"_adjc");
	N("predicate-adj",4) = 1;
	fixnphead(3);
	chpos(N(2),"PP$");	# her/PP$
	dqaninfo(2,0,0,3);
	groupnp();
	}
  else if (verbfeat(N(1),"V4"))
	{
    alphatovg(3,"active","VBG");
    if (!N("mypos",2))
  	  chpos(N(2),"PP");	# her/PP.
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_pro
	_xALPHA
	_xALPHA
	@@

# her
# Note: 'her' is ambiguous PP/PP$
@PRE
<2,2> vareq("stem","her");
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"-ing"))
  	fail();
  if (!verbfeat(N(1),"V4"))
  	fail();
@POST
  alphatovg(3,"active","VBG");
  if (!N("mypos",2))
  	chpos(N(2),"PP");	# her/PP.
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_pro
	_xALPHA
	@@

# adj to alpha
# adjc to alpha
@PRE
<2,2> vareq("stem","to");
<3,3> var("verb");
@CHECK
  if (!vconjq(N(3),"inf"))
  	fail();
@POST
  alphatovg(3,"active","VB");
@RULES
_xNIL <-
	_xWILD [one match=(_adjc _adj)]
	_prep
	_xALPHA
	@@

# verb much alpha $
# vg much alpha $
@PRE
<3,3> var("adv");
@CHECK
  L("t") = nodestem(N(3));
  if (advconj(L("t")) != "RBR")
  	fail();
@POST
  alphatoadv(3);
  alphatoadv(2);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [one match=(much)]
	_xALPHA
	_xEND
	@@

# verb by
# vg by
@PRE
<2,2> varne("mypos","RB");
<2,2> varz("by-adv");
@CHECK
  if (pnname(N(1)) == "_vg")
  	{
	if (!vgassigned(N(1)))
	  fail();
	if (N("voice",1) == "passive")
	  fail();
	}
  else if (!N("mypos",1) || N("mypos",1) == "VBN")
  	fail();
  if (phrprepverbq(N(1),N(2)))
  	fail();
@POST
  N("by-adv",2) = 1;
  chpos(N(2),"RB");	# by/RB
  pnrename(N(2),"_adv");	# by -> adv. prep -> adv.
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	by [s]
	_xWILD [one lookahead fail=(_noun _np _xALPHA _det _quan _num
		_xNUM _adj)]
	@@

# vg to alpha noun (that)
# Note: Decide or gather evidence for noun/verb.
@PRE
<2,2> vareq("stem","to");
<3,3> varz("pos50 vtan-that");
#<5,5> vareq("stem","that");
@POST
  N("pos50 vtan-that",3) = 1;
  if (!N("noun",3) && !N("verb",3))
    {
	if (N("adj",3))
	  alphatoadj(3);
	else if (N("adv",3))
	  alphatoadv(3);
	}
  else if (!N("verb",3))
    fixnpnonhead(3);
  else if (!N("noun",3))
    {
	# patterns: stooped to eating garbage....
	# verb adj adv
	if (vconjq(N(3),"-en") || vconjq(N(3),"-s"))
	  fixnpnonhead(3);
	else if (vconjq(N(3),"-ing"))
	  alphatovg(3,"active","VBG");
	else # inf
	  {
	  # [DEFAULT] for now. Can do more logic here.
	  if (!verbfeat(N(1),"T3") && !verbfeat(N(3),"D5"))
	    fixnpnonhead(3);
	  else
	    alphatovg(3,"active","VB");
	  }
	}
  else # both noun and verb possible.
    {
	# noun verb (adj adv)
	if (vconjq(N(3),"-en") || vconjq(N(3),"-s"))
	  fixnpnonhead(3);
	else if (vconjq(N(3),"-ing"))
	  alphatovg(3,"active","VBG");
	else # inf
	  {
	  if (!verbfeat(N(1),"T3") && !verbfeat(N(3),"D5"))
#	    fixnpnonhead(3);	# Need more context.
		;
	  else
	    alphatovg(3,"active","VB");
	  }
	}
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]	### (1)
    _prep							### (2)
    _xALPHA							### (3)
    _noun [lookahead]				### (4)
#    _fnword							### (5)
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

# vg np dqan conj alpha
# verb noun dqan conj alpha
@PRE
<8,8> varz("pos50 vn-dqan-conj-a");
@POST
  N("pos50 vn-dqan-conj-a",8) = 1;
  if (!N("noun",8) && !N("verb",8))
    {
	if (N("adv",8))
	  alphatoadv(8);
	else if (N("adj",8))
	  alphatoadj(8);
	}
  else if (!N("verb",8))
    fixnphead(8);
  else if (!N("noun",8))
    {
	if (samevconj(N(1),N(8)))
	  {
	  if (pnname(N(1)) == "_vg")
	    L("v") = N("verb node",8);
	  else
	    L("v") = N(8);
	  if (L("v"))
	    L("m") = pnvar(L("v"),"mypos");
	  alphatovg(8,L("voice"),L("m"));
	  }
	else
	  alphatovg(8,0,0);
	}
  else # Both verb and noun.
    {
	if (samevconj(N(1),N(8)))
	  {
	  if (pnname(N(1)) == "_vg")
	    L("v") = N("verb node",1);
	  else
	    L("v") = N(1);
	  if (L("v"))
	    L("m") = pnvar(L("v"),"mypos");
	  alphatovg(8,L("voice"),L("m"));
	  }
	else
	  alphatovg(8,0,0);
	}
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]			### (1)
    _xWILD [one match=(_noun _np)]			### (2)
    _xWILD [plus match=(_det _pro)]			### (3)
    _xWILD [star match=(_quan _num _xNUM)]	### (4)
    _adj [star]								### (5)
    _noun [plus]							### (6)
    _conj									### (7)
    _xALPHA									### (8)
    _xWILD [one match=(_pro)]				### (9)
    @@

# dqa alpha+ prep which
# Note: back-end patterns...
@CHECK
  if (!N(1) && !N(2) && !N(3))
  	fail();
  if (!N("adj",4) && !N("noun",4))
  	fail();
@POST
  L("n") = eltnode(5);
  if (L("n"))
    {
	if (pnvar(L("n"),"noun") || pnvar(L("n"),"adj"))
	  {
	  alphatoadj(4);
	  }
	else
	  fixnphead(4);
	}
  else
    fixnphead(4);
@RULES
_xNIL <-
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	 _xALPHA
	 _xALPHA [star]
	 _prep [lookahead]
	 _whword [s]
	 @@

# vg np who alpha noun conj alpha noun
@PRE
<3,3> vareq("stem","who");	# who. # whom.
<4,4> varz("pos50 vnpro-an-conj-an");
<4,4> var("verb");
<8,8> var("verb");
@CHECK
  S("vc") = samevconj(N(4),N(8));
  if (!S("vc"))
  	fail();
  if (S("vc") == "-en")
  	fail();
@POST
  N("pos50 vnpro-an-conj-an",4) = 1;
  if (S("vc") == "inf")
  	{
	alphatovg(8,"active","VBP");
	alphatovg(4,"active","VBP");
	}
  else
  	{
	alphatovg(8,"active","VBD");
	alphatovg(4,"active","VBD");
	}

@RULES
_xNIL <-
    _vg															### (1)
    _np															### (2)
    _fnword														### (3)
    _xALPHA														### (4)
    _xALPHA [star]												### (5)
    _noun [plus]												### (6)
    _conj														### (7)
    _xALPHA														### (8)
    _xALPHA [star lookahead]									### (9)
    _noun [plus]												### (10)
    _xWILD [one match=( \, _clausesep)]							### (11)
    _xWILD [one match=(_fnword _pro _np _det _quan _num _xNUM _adj)]	### (12)
    @@

##### VERBLESS

@CHECK
  # Agreement...
  L("arr") = vgagree(N(1),N(3),N(5),N(7),0);
#  "err.txt" << L("arr") << phrasetext() << "\n";
  if (!L("arr")[0])
    fail();
  S("voice") = "active";
@POST
  L("m") = N(1);
  L("h") = N(3);
  L("b") = N(5);
  L("being") = N(7);
 if (N(7))
   {
    N("sem",7) = N("stem",7) = "be";
    chpos(N(7),"VBG");
   }
  L("neg") = mhbvadv(2,4,6,0);
  group(1,7,"_vg");
  mhbv(N(1),L("neg"),L("m"),L("h"),L("b"),L("being"),0);
  N("voice",1) = S("voice");
@RULES
_xNIL <-
	_xWILD [s one match=(_modal _do) except=(_vg _np)]
	_xWILD [star match=(_adv _advl)]
	_have [s opt]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	_xWILD [one lookahead fail=(_xALPHA _adv)]
	@@

@CHECK
  # Agreement...
  L("arr") = vgagree(0,N(1),N(3),N(5),0);
  S("verb node") = N(1);
  if (!L("arr")[0])
    fail();
#  S("voice") = "active";
@POST
 L("h") = N(1);
 L("b") = N(3);
 L("being") = N(5);
 if (N(5))
   {
    N("sem",5) = N("stem",5) = "be";
    chpos(N(5),"VBG");
   }
  L("neg") = mhbvadv(2,4,0,0);
 if (N(5))
   group(1,5,"_vg");
 else if (N(3))
	group(1,3,"_vg");
 else
	group(1,1,"_vg");
 mhbv(N(1),L("neg"),0,L("h"),L("b"),L("being"),0);

@RULES
_xNIL <-
	_xWILD [s one match=(_have) except=(_vg)]
	_xWILD [star match=(_adv _advl)]
	_be [s opt]
	_xWILD [star match=(_adv _advl)]
	being [s opt]
	@@

@CHECK
  # Agreement...
  L("arr") = vgagree(0,0,N(1),N(3),0);
  if (!L("arr")[0])
    fail();
  S("voice") = "active";

 if (N(3))
   {
    N("sem",3) = N("stem",3) = "be";
    chpos(N(3),"VBG");
   }
@RULES
_vg <-
	_be [s]
	_xWILD [star match=(_adv _advl)]
	being [s]
	@@

# be
@POST
  L("tmp1") = N(1);
  L("neg") = mhbvadv(2,0,0,0);
  group(1,1,"_vg");
  mhbv(N(1),L("neg"),0,0,L("tmp1"),0,0);
  pncopyvars(L("tmp1"),N(1));
  N("voice",1) = "active";
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_be [s except=(_vg)]
	_adv [star]
	_xWILD [one fail=(_xALPHA _verb)]
	@@
_xNIL <-
	_be [s except=(_vg)]
	_adv [star]
	_xEND
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

# noun , dqan , conj alpha
@POST
  if (N("adj",9) && N("noun",9))
    {
	if (singular(N(9)) && !N("mass",9))
	  fixnpnonhead(9);
	}
  else if (N("adj",9))
    alphatoadj(9);
  else if (N("noun",9))
    alphatonoun(9);
  dqaninfo(3,4,5,6);
  groupnp();
@RULES
_xNIL <-
    _xWILD [one match=(_noun _np _pro)]		### (1)
    \,										### (2)
    _xWILD [star match=(_det _pro)]			### (3)
    _xWILD [star match=(_quan _num _xNUM)]	### (4)
    _adj [star]								### (5)
    _noun [plus]							### (6)
    \, [opt]								### (7)
    _conj									### (8)
    _xALPHA									### (9)
    @@

# alpha verb
@PRE
<1,1> var("noun");
@CHECK
  if (!plural(N(1)))
  	fail();
@POST
  fixnphead(1);
@RULES
_xNIL <-
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# vg prep dqa alpha
@PRE
<6,6> var("noun");
@CHECK
  if (!N(3) && !N(4) && !N(5))
  	fail();
@POST
  if (!N("mypos",2))
  	{
	L("rp") = phrprepverbq(N(1),N(2));
	if (L("rp") == 1 || L("rp") == 3)
		chpos(N(2),"RP");	# prep/RP.
	else
		chpos(N(2),"IN");	# prep/IN.
	}
  fixnphead(6);
@RULES
_xNIL <-
    _xWILD [one match=(_verb _vg)]				### (1)
    _prep										### (2)
    _xWILD [star match=(_det _pro)]				### (3)
    _xWILD [star match=(_quan _num _xNUM)]		### (4)
    _adj [star]									### (5)
    _xALPHA										### (6)
    _xWILD [star lookahead match=(_adv _advl)]	### (7)
    _xWILD [one match=(_prep _fnword)]			### (8)
    @@

# vg prep dqan
@PRE
<1,1> varz("pos50 v-p-check");
<2,2> varz("mypos");
@CHECK
  S("num") = phrprepverbq(N(1),N(2));
  if (!S("num"))
  	fail();
@POST
  N("pos50 v-p-check",1) = 1;
  if (S("num") == 1 || S("num") == 3)
    chpos(N(2),"RP");	# prep/RP
  else
    chpos(N(2),"IN");	# prep/IN
@RULES
_xNIL <-
	_vg
	_prep
	_xWILD [one match=(_det _pro _quan _num _xNUM _adj _noun _np)]
	@@

# prep np
# vg prep np
@PRE
<3,3> varne("stem","after");
<3,3> varne("stem","by");	# Trying to keep these out. # 05/25/07 AM.
<3,3> varne("mypos","RP");	# Attached to verb.
@POST
  if (N("stem",3) == "by")
  	L("by") = 1;
  L("tmp4") = N(4);
  group(3,4,"_advl");	# prep np -> advl
  pncopyvars(L("tmp4"),N(3));
  clearpos(N(3),1,0);
  N("pp",3) = 1;
  if (L("by"))
  	{
  if (N("sem",3) != "date" && N("sem",3) != "geoloc"
	&& N("sem",3) != "quantity")
    N("by-actor",3) = 3;
  N("by-np",3) = 1;
  	}
@RULES
_xNIL <-
	_xWILD [one match=(_pro _np _vg)]
	_xWILD [star match=(_adv _advl)]
	_prep
	_np
	_xWILD [one lookahead fail=( \, _conj _prep)]
	@@

#####################
# DEALING WITH DOUBLE QUOTES.	# 05/25/07 AM.
@CHECK
 if (!N("noun",2) && !N("adj",2))
 	fail();
@POST
  fixnpnonhead(2);
  group(1,4,"_np");
@RULES
_xNIL <-
	_dblquote
	_xALPHA
	_noun
	_dblquote
	@@

# Zap double quotes.
@POST
  excise(1,1);
@RULES
_xNIL <-
	_dblquote
	@@
