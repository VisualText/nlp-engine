###############################################
# FILE: dqan100.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 09/Jul/04 20:47:42
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# Lone noun.
@POST
  pncopyvars(2);
  sclearpos(1,1);	# Zero out token info.
  singler(2,2);
@RULES
_np <-
	_xWILD [one match=(_prep _vg)]
	_noun
	_xWILD [one lookahead match=(_prep _conj _qEOS _xEND)]
	@@

# Not sure why alpha in first position in these rules....
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
	}
  if (N(4))
    {
	L("firstj") = N(4);
	L("lastj") = lasteltnode(4);
	}
  groupnp();
@RULES
_xNIL <-
	_xALPHA
	_xWILD [plus match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS)]
	@@
_xNIL <-
	_xALPHA
	_xWILD [star match=(_det _pro)]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS)]
	@@
_xNIL <-
	_xWILD [one fail=(_xALPHA _det _quan _num _xNUM _adj _noun)]
	_xWILD [s star match=(_det _proPoss)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_adj _xALPHA _aposS)]
	@@

@POST
  L("tmp4") = lasteltnode(4);
  pncopyvars(L("tmp4"));
  sclearpos(1,0);
  S("bracket",2) = 1;
  S("ne") = 0;
  singler(2,4);
@RULES
_np <-
	_xSTART
	_xWILD [star match=(_det _quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_noun _adj _xALPHA _aposS)]
	@@

@POST
  L("n") = lasteltnode(4);
  pncopyvars(L("n"));
  sclearpos(1,0);	# Zero out token info.
  singler(1,4);
@RULES
_np <-
	_proPoss [s]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

@POST
  pncopyvars(3);
  sclearpos(1,0);	# Zero out token info.
  singler(2,3);
@RULES
_np <-
	_xWILD [one fail=(_xALPHA _noun _det _quan _num _xNUM _aposS)]
	_adj [plus]
	_noun
	_xWILD [one lookahead fail=(_noun _xALPHA)]
	@@

# nounless.
@POST
  L("tmp2") = lasteltnode(2);
  pncopyvars(L("tmp2"));
  sclearpos(1,0);
  singler(1,2);
@RULES
_np <-
	_xWILD [plus match=(_det _quan _num _xNUM)]
	_adj [plus]
	_xWILD [one lookahead fail=(_noun _adj _xALPHA _dbldash)]
	@@

# vg part of
# idiomatic
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  nountonp(2,1);
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_vg
	_xWILD [s one match=(part parts)]
	of [s lookahead]
	@@

# det noun noun
@POST
  pncopyvars(3);
  sclearpos(1,0);	# Zero out token info.
  singler(1,3);
@RULES
_np <-
	_det
	_noun
	_noun
	_xWILD [lookahead one fail=(_noun _conj)]
	@@

# prep noun alpha .
@CHECK
  if (!N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_prep
	_noun
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# det alpha
@CHECK
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N("noun",2) && N("adj",2)) # Can't decide yet.
    fail();
@POST
  L("tmp2") = N(2);
  if (N("noun",2))
    group(2,2,"_noun");
  else if (N("adj",2))
    group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
@RULES
_xNIL <-
	_det
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

# pro
@POST
  if (pnname(N(3)) == "_vg")
    if (!N("voice",3))
	  N("voice",3) = "active";
  if (pnname(N(2)) == "_pro")
    {
	nountonp(2,1);
	}
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _adv _advl _conj _fnword)]
	_pro [s]
	_xWILD [one lookahead match=(_vg)]
	@@

# prep adj alpha
# dqa alpha
@CHECK
  if (!N("noun",5) && !N("adj",5))
    fail();
  dqaninfo(2,3,4,5);
  if (!numbersagree(S("first"),S("last")))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  if (!N(4))
    S("firstan") = N(5);
  S("firstn") = S("last") = S("lastn")
  	= S("lastan") = N(5);	# Reset stuff.
  groupnp();
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_prep _conj _verb _vg)]
	_det [plus]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xWILD [one lookahead match=(\, _qEOS _xEND _dbldash _prepj)]
	@@
_xNIL <-
	_xWILD [one match=(_prep _conj _verb _vg)]
	_det [star]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xWILD [one lookahead match=(\, _qEOS _xEND _dbldash _prep)]
	@@
_xNIL <-
	_xWILD [one match=(_prep _conj _verb _vg)]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [plus]
	_xALPHA
	_xWILD [one lookahead match=(\, _qEOS _xEND _dbldash _prep)]
	@@

# alpha alpha
# TOO BROAD.
@CHECK
  if (N("verb",2) || N("verb",3))
    fail();
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (!N("adj",3) && !N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp2") = N(2);
  if (N("adj",2))
    group(2,2,"_adj");
  else
    group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  group(2,3,"_np");
  pncopyvars(L("tmp3"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(\, _dbldash)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep _conj)]
	@@

# low for
@CHECK
  if (N("adj",2) && N("noun",2))
    fail();	# Can't decide here.
  if (!N("adj",2) && !N("noun",2))
    fail();
  if (N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  if (N("noun",2))
    group(2,2,"_noun");
  else
    group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(2)) == "_adj")
    fixadj(N(2));
  nountonp(2,1);
@RULES
_xNIL <-
	_xWILD [one match=(\,)]
	_xALPHA
	_xWILD [one lookahead match=(_prep _conj)]
	@@

# verb alpha
@CHECK
  if (N("pos num",2) != 2)
    fail();
  if (!N("verb",2))
    fail();
  if (!vconjq(N(1),"inf"))
    fail();
  # Check for helping verbs etc.
@POST
  L("tmp2") = N(2);
  if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	_verb
	_xALPHA
	@@

# well over
# well under
@POST
  L("tmp2") = N(2);
  group(2,2,"_adv");
  pncopyvars(L("tmp2"),N(2));
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	well
	_xWILD [s one lookahead match=(over under)]
	@@

# _adjc is constituent level concept, like _vg, _np.
# vg adj
@POST
  L("tmp3") = N(3);
  group(3,4,"_adjc");
  pncopyvars(L("tmp3"),N(3));
  N("ignorepos",3) = 1;	# commented 04/19/07 AM. (why)
  N("posarr len",3) = 0;	# 04/21/07 AM.
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np _vg)]
	_xWILD [star match=(_advl _adv)]
	_adj
	_xWILD [star match=(_advl _adv)]
	_xWILD [one lookahead match=(_prep _fnword _qEOS _xEND \,)]
	@@

@POST
  singler(2,7);
@RULES
_np <-
	_xWILD [one match=(_xSTART)]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_adj [plus]
	_noun [plus]
	_xWILD [one fail=(_xALPHA _noun _adj)]
	@@

	
# verb alpha dqan
# vg alpha dqan
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
  else if (N("noun",2))
    {
	group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnoun(N(2));
	}
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xALPHA
	_noun [plus]
	@@

# Convert pronouns at some point.
@PRE
<1,1> varz("proposs");
@CHECK
  if (pnname(N(1)) == "_np")
    fail();
@POST
  nountonp(1,1);
@RULES
_xNIL <-
	_xWILD [s one match=(_proSubj _proObj) except=(_whword)]
	@@

