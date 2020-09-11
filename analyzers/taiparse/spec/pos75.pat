###############################################
# FILE: pos75.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 26/Jul/04 09:44:27
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

# another ugly tiptoeing
# adj verb
# Sometimes it's a verb, sometimes ving noun.
@CHECK
  L("n") = pndown(N(2));
  if (!vconjq(L("n"),"-ing"))
    fail();
@POST
  pnrename(N(2),"_noun");
  posacct(N(2));
@RULES
_xNIL <-
	_adj
	_verb
	@@

# prep noun alpha prep
@CHECK
  if (!N("adj",3))
    fail();
  if (N("verb",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
@RULES
_xNIL <-
	_prep
	_xWILD [one match=(_noun _np)]
	_xALPHA
	_xWILD [star lookahead match=(_adv _advl)]
	_xWILD [one match=(_prep)]
	@@

# prep noun
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_noun
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# prep adv alpha noun
@CHECK
  if (!N("adj",3) && !N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  if (N("adj",3))
    group(3,3,"_adj");
  else
    group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  if (pnname(N(3)) == "_adj")
    fixadj(N(3));
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_adv [opt]
	_xALPHA
	_xWILD [one lookahead match=(_noun)]
	@@

	
# adj conj adj
@POST
  L("tmp4") = N(4);
  group(2,4,"_adj");
  pncopyvars(L("tmp4"),N(2));
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(_xALPHA)]
	_adj
	_conj
	_adj
	@@

# how much power he
# how much alpha pro
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_howmuch [s]
	_xALPHA
	_xWILD [one lookahead match=(_pro)]
	@@

# , det noun alpha ,
@CHECK
  if (!N("noun",4))
    fail();
  # Todo: Check for noun-verb DISagreement.
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_det
	_noun
	_xALPHA
	_xWILD [one lookahead match=( \, )]
	@@

# alpha noun verb
@CHECK
  if (!N("verb",1))
    fail();
  if (N("pos num",2) != 2)
    fail();
  if (!N("adj",1) && !N("noun",1))
    fail();
@POST
  L("tmp1") = N(1);
  if (N("noun",1))
    group(1,1,"_noun");
  else
    group(1,1,"_adj");
  pncopyvars(L("tmp1"),N(1));
  if (pnname(N(1)) == "_adj")
    fixadj(N(1));
@RULES
_xNIL <-
	_xALPHA
	_noun [lookahead]
	_xWILD [one match=(_verb _vg)]
	@@

# det noun '
@POST
  chpos(N(3),"POS");	# Possessive.
  group(2,3,"_adj");
@RULES
_xNIL <-
	_xWILD [one match=(_det _quan _num _xNUM _adj)]
	_noun
	_aposX
	@@

# once verb
@POST
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	once [s]
	_xWILD [one lookahead match=(_verb _vg)]
	@@

# Some pos tagging.
# noun verb
@POST
  if (vconjq(N(2),"inf"))
    chpos(N(2),"VBP");
@RULES
_xNIL <-
	_xWILD [one match=(_noun _quan _num _xNUM)]
	_verb
	@@

# that
@CHECK
  if (N("voice",1))
    fail();
#  if (N("mypos",2))
#    fail();
@POST
#  chpos(N(2),"IN");	# that/IN
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	that [s]
	@@
# that
#@CHECK
#  if (N("mypos",2))
#    fail();
#@POST
#  chpos(N(2),"WDT");
#  N("bracket",2) = 1;
#@RULES
#_xNIL <-
#	_xWILD [one match=(_noun _adj _np)]
#	that [s]
#	@@

# more adv
# more adj
@POST
  chpos(N(1),"RBR");
  pnrename(N(1),"_adv");
@RULES
_xNIL <-
	_xWILD [s one match=(more less)]
	_xWILD [one lookahead match=(_adv _advl _adj)]
	@@

# verb alpha
# foreshadow rising
@CHECK
  if (!N("adj",2))
    fail();
  if (!N("adv",2) && !N("verb",2))
    fail();
  S("vc2") = vconj(N(2));
  if (S("vc2") == "inf"
   || S("vc2") == "-s")
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [s one match=(_verb _vg) except=(_modal _have _be being)]
	_xALPHA
	@@


# noun verb alpha
@CHECK
  if (!N("verb",7))
    fail();
  if (!N("noun",7))
    fail();	# For now, just handle n-v ambig.
@POST
  L("tmp7") = N(7);
  group(7,7,"_noun");
  pncopyvars(L("tmp7"),N(7));
  fixnoun(N(7));
  if (pnname(N(5)) == "_vg")
    if (!N("voice",5))
	  N("voice",5) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [one match=(_noun _np)]
	_xWILD [star match=(_adv _advl)]
	_xWILD [s one match=(_verb _vg) except=(_modal _have _be)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	@@

# adj alpha that
# that
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  fixnoun(N(2));
#  chpos(N(3),"WDT"); # ...
#  N("bracket",3) = 1;
@RULES
_xNIL <-
	_adj
	_xALPHA
	that [s]
	@@


