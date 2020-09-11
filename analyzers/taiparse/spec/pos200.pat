###############################################
# FILE: pos200.pat
# SUBJ: POS tagging.
# AUTH: AM
# CREATED: 09/Jul/04 22:33:40
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

# np lists...
@POST
  if (pnname(N(7)) != "_np")
    {
	group(7,7,"_np");
	N("bracket",7) = 1;
	}
  group(2,7,"_np");
  N("list-np",2) = 1;
#  listadd(7,2,"true");
@RULES
_xNIL <-
	\,
	_xWILD [one match=(_noun _np)]
	\,
	_xWILD [one match=(_noun _np)]
	\, [opt]
	_conj
	_xWILD [one match=(_noun _np)]
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

# Extend list backwards.
@CHECK
  if (!N("list-np",4))
    fail();
@POST
  listadd(4,2,"true");
@RULES
_xNIL <-
	_xWILD [one fail=(_det _quan _num _xNUM _adj _noun)]
	_xWILD [one match=(_noun _np)]
	\,
	_np
	@@
_xNIL <-
	_xSTART
	_xWILD [one match=(_noun _np)]
	\,
	_np
	@@

	

@POST
  L("tmp") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xSTART
	_noun
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

# cap's adj
# cap's noun
# Note: "John's green" could mean "John IS green...".
#@CHECK
#  if (!N("apos-s",1))
#    fail();
#@POST
#  L("tmp") = N(1);
#  group(1,1,"_adj");
#  pncopyvars(L("tmp"),N(1));
#@RULES
#_xNIL <-
#	_caps
#	_xWILD [one lookahead match=(_noun _adj)]
#	@@

# noun alpha adj
@CHECK
  if (!N("verb",2))
    fail();
  if (!vconjq(N(2),"-en"))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  pncopyvars(L("tmp2"),N(2));
  L("v") = N(2);
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
#  N("voice",2) = "passive";
  N("verb node",2) = L("v");
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xWILD [one lookahead fail=(_noun _np)]
	@@

# noun alpha vg
# Too broad. Too old.	# 06/24/06 AM.
#@CHECK
#  if (!N("noun",2))
#    fail();
#  if (N("adv",2))
#    fail();	# Too ambiguous...
#@POST
#  L("tmp2") = N(2);
#  group(2,2,"_noun");
#  pncopyvars(L("tmp2"),N(2));
#@RULES
#_xNIL <-
#	_noun
#	_xALPHA
#	_xWILD [one lookahead match=(_verb _vg)]
#	@@

# vg adj alpha alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp4") = N(4);
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
	}
  if (N("noun",4))
    group(4,4,"_noun");
  else
  	group(4,4,"_adj");
  pncopyvars(L("tmp4"),N(4));
  if (pnname(N(4)) == "_adj")
    fixadj(N(4));
  group(2,4,"_np");
  pncopyvars(L("tmp4"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_vg
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [plus lookahead match=(_prep _conj _qEOS _fnword _xEND)]
	@@

# apos-s alpha
#@CHECK
#  if (!N("apos-s",1))
#    fail();
#@POST
#  L("tmp2") = N(2);
#  if (N("noun",2))
#    group(2,2,"_noun");
#  else if (N("adj",2))
#    group(2,2,"_adj");
#  else if (N("adv",2))
#    group(2,2,"_adv");
#  pncopyvars(L("tmp2"),N(2));
#@RULES
#_xNIL <-
#	_adj
#	_xALPHA
#	_xWILD [one lookahead match=(_prep _conj _qEOS _fnword _xEND)]
#	@@

# there vg np
@CHECK
  if (N("there-vg",2))
    fail();
@POST
  N("there-vg",2) = 1;
  fixvg(N(2),"active","VBP");
@RULES
_xNIL <-
	there [s]
	_vg [lookahead]
	_np
	@@

# det alpha noun
@POST
  L("tmp2") = N(2);
  if (N("adj",2))
    {
    group(2,2,"_adj");
	fixadj(N(2));
	}
  else if (N("noun",2))
    group(2,2,"_noun");
  else if (N("adv",2))
    group(2,2,"_adv");
  else
    group(2,2,"_noun");	# Loop guard. # 06/19/05
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_noun
	@@

# vg _adj _noun _fnword
@POST
  pncopyvars(3);
  sclearpos(1,0);	# Zero out token info.
  singler(2,3);
@RULES
_np <-
	_vg
	_adj
	_noun
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

# det adj noun noun
@CHECK
  L("last") = lasteltnode(3);
  if (N(1))
    L("first") = N(1);
  else if (N(2))
    L("first") = N(2);
  else
    L("first") = N(3);
  if (!numbersagree(L("first"),L("last")))
    S("disagree") = 1;
@POST
#  if (S("disagree"))
#    {
#	# Check multiple nouns...
#	group(1,2,"_np");
#	}
#  else
#    {
  L("tmp") = lasteltnode(3);
  group(1,3,"_np");
  pncopyvars(L("tmp"),N(1));
  clearpos(N(1),1,1);	# Zero out token info.
#    }
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_xALPHA _aposS)]
	@@

# det quan adj alpha
# Todo: agreement checks here.
@CHECK
  if (N("pos200 a-v",3))
    fail();	# Loop guard.
@POST
  N("pos200 a-v",3) = 1; # Loop guard.
  L("last") = lasteltnode(2);
  L("tmp3") = N(3);
  if (!N("noun",3))
    {
	# etc.
	}
  else
    {
	if (numbersagree(N(2),N(3))) # Agreement of range of nodes.
	  {
	  if (G("error"))
	  "err.txt" << "Agreement " << L("need") << "\n";
	  }
	else if (N("verb",3))
	  {
	  group(3,3,"_verb");
	  pncopyvars(L("tmp3"),N(3));
      clearpos(N(3),1,0);	# Zero out token info.
	  }
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xWILD [one lookahead match=(_adv _advl _prep)]
	@@

@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xWILD [one lookahead match=(_vg _verb)]
	@@

# prep alpha
@CHECK
  if (!N("noun",2) && !N("adj",2) && !N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("noun",2) || N("unknown",2))
    {
    group(2,2,"_noun");
    pncopyvars(L("tmp2"),N(2));
	if (number(N(2)) == "plural")
	  chpos(N(2),"NNS");
	else if (N("unknown",2) && strendswith(N("$text",2),"s"))
	  chpos(N(2),"NNS");
	}
  else
    {
    group(2,2,"_adj");
    pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
    }
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(_prep) except=(to)]
	_xALPHA
	_xWILD [lookahead one match=(_prep _conj \, _fnword _xEND)]
	@@

# vg , noun vg
@POST
  L("tmp3") = N(3);
  group(3,3,"_np");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(_vg)]
	\, [opt]
	_noun
	\, [opt lookahead]
	_xWILD [one match=(_vg)]
	@@

# noun alpha noun
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
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_noun
	_xALPHA
	_noun
	@@

# vg noun aposS
# prep noun aposS
@POST
  chpos(N(2),"POS");	# Assumed.
  L("tmp2") = lasteltnode(2);
  group(2,3,"_adj");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,0);
  N("possessive",2) = 1;
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb _prep _conj \,)]
	_noun [plus]
	_aposS
	@@

# adj alpha fnword
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_adj
	_xALPHA
	_xWILD [lookahead one match=(_fnword)]
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

# apposition, appositive.
# vg np , noun , prep
@POST
  # Todo: compose...
  L("tmp2") = N(2);
  L("tmp4") = N(4);
  if (pnname(N(4)) == "_noun")
    {
	group(4,4,"_np");
	pncopyvars(L("tmp4"),N(4));
	clearpos(N(4),1,1);
	}
  group(2,5,"_np");
  pncopyvars(L("tmp2"),N(2));
  pncopyvars(L("tmp4"),N(2));
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _fnword _prep _conj _clause)]
	_np
	_xWILD [one match=( \, _dbldash)]
	_xWILD [one match=(_noun _np)]
	_xWILD [one match=( \, _dbldash)]
	_xWILD [one lookahead match=(_prep _vg)]
	@@

# vg np , noun , prep
@POST
  # Todo: compose...
  L("tmp2") = N(2);
  L("tmp4") = N(4);
  if (pnname(N(4)) == "_noun")
    {
	group(4,4,"_np");
	N("bracket",4) = 1;
	}
  group(2,4,"_np");
  pncopyvars(L("tmp2"),N(2));
  pncopyvars(L("tmp4"),N(2));
  clearpos(N(2),1,0);	# Zero out token info.
  N("ne",2) = 0;
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg _fnword _prep)]
	_np
	\,
	_xWILD [one match=(_noun _np)]
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# noun alpha vg
# assume spell correct hasn't found verb.
@CHECK
  if (!N("unknown",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# neither noun nor
@POST
  if (nonliteral(N(1)))
    pnrename(N(1),"_det");
  else
    group(1,1,"_det");
  chpos(N(1),"CC");
  chpos(N(3),"CC");
@RULES
_xNIL <-
	_xWILD [s one match=(neither) except=(_det)] # Loop guard.
	_xWILD [one lookahead match=(_noun _np)]
	nor [s]
	@@

# Some clausal pattern...
# vg np prep np vg
# Looking for a 2nd vg to be passive.
@CHECK
  if (N("fixed",5))
    fail();
@POST
  L("vb") = N("verb node",5);
  fixverb(L("vb"),"passive","VBP");
  N("fixed",5) = 1;	# To avoid infinite recursion.
@RULES
_xNIL <-
	_vg
	_np
	_prep
	_np
	_vg
	@@

# noun noun
# dqan
@POST
  L("tmp2") = lasteltnode(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
  N("ne",2) = 0;
  if (pnname(N(3)) == "_vg")
    if (!N("voice",3))
	  N("voice",3) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS _np)]
	_noun [plus]
	_xWILD [one lookahead match=(_verb _vg _adv _advl _np \,)]
	@@

# vg to vg
# used to make
#@POST
# # Assign infinitive.
# L("n") = pndown(N(3));
# if (pnname(L("n")) == "_verb")
#   pnreplaceval(L("n"),"inf",1);
# L("v") = N("verb node",3);
# if (L("v"))
#   chpos(L("v"),"VB"); # infinitive.
# L("v") = N("verb node",1);
# if (L("v"))
#   fixverb(L("v"),"active",0);
#
# L("tmp3") = N(3);
# group(1,3,"_vg");
# pncopyvars(L("tmp3"),N(1));
# # N("voice") ?
#@RULES
#_xNIL <-
#	_vg
#	to [s]
#	_vg
#	@@

# prep np and np
# check agreement?
@POST
  if (pnname(N(4)) == "_noun")
    {
	group(4,4,"_np");
	N("bracket",4)  = 1;
	}
  if (pnname(N(2)) == "_noun")
    {
	group(2,2,"_np");
	N("bracket",2)  = 1;
	}
  group(2,4,"_np");
  N("compound-np",2) = 1;
  clearpos(N(2),1,0);
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xWILD [one match=(_np _noun)]
	_conj
	_xWILD [one match=(_np _noun)]
	_xWILD [one lookahead match=(_verb _vg _prep)]
	@@

# np , alpha conj alpha np
@CHECK
  if (!N("noun",3))
    fail();
  if (!N("adv",6))
    fail();
@POST
  L("tmp6") = N(6);
  L("tmp3") = N(3);
  group(6,6,"_adv");
  pncopyvars(L("tmp6"),N(6));
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_np _noun)]
	\,
	_xALPHA
	\, [opt]
	_conj
	_xALPHA
	_xWILD [one lookahead match=(_np)]
	@@

# verb alpha prep
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# verb dqan alpha
@CHECK
  if (!N("noun",6))
    fail();
  # Check noun agreement...
@POST
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
  fixnoun(N(6));
  dqaninfo(2,3,4,5);
  S("olast") = 6;
  S("last") = S("lastn") = S("lastan") = N(6);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_det [opt]
	_xWILD [star match=(_xNUM _quan _num)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _fnword _xEND)]
	@@

# np vg prep dqan alpha alpha
@CHECK
  # Check tenses.
  if (!N("noun",8) || !N("noun",9))
    fail();
@POST
  L("tmp8") = N(8);
  L("tmp9") = N(9);
  group(9,9,"_noun");
  pncopyvars(L("tmp9"),N(9));
  group(8,8,"_noun");
  pncopyvars(L("tmp8"),N(8));
@RULES
_xNIL <-
	_np
	_vg
	_prep
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _fnword _clausesep)]
	@@

# of dqan and noun
@POST
  group(2,4,"_np");
  N("bracket",2) = 1;
@RULES
_xNIL <-
	of [s]
	_noun [plus]
	_conj
	_noun
	_xWILD [one lookahead match=(_vg _verb _prep _qEOS _xEND)]
	@@
