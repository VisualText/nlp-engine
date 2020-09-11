###############################################
# FILE: pos100.pat
# SUBJ: Part-of-speech from context.
# AUTH: AM
# CREATED: 08/Jul/04 01:25:52
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

@CHECK
  if (!N("needs-np",1))
    fail();
@POST
  L("tmp") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp"),N(2));
@RULES
_xNIL <-
	_det
	_xALPHA
	_prep [lookahead]
	@@

# vg det alpha alpha .
@CHECK
  if (!N("noun",4) && !N("adv",4))
    fail();
@POST
  L("tmp4") = N(4);
  L("tmp3") = N(3);
  if (N("noun",4))
    {
	group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	if (N("adj",3))
	  group(3,3,"_adj");
	else if (N("noun",3))
	  group(3,3,"_noun");
	else
	  group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	if (pnname(N(3)) == "_adj")
	  fixadj(N(3));
	group(2,4,"_np");
	pncopyvars(L("tmp4"),N(2));
    clearpos(N(2),1,1);	# Zero out token info.
	}
  else # 4 = adv
    {
	group(4,4,"_adv");
	pncopyvars(L("tmp4"),N(4));
	group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	group(2,3,"_np");
	pncopyvars(L("tmp3"),N(2));
    clearpos(N(2),1,1);	# Zero out token info.
	}
  if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_vg
	_det
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# vg alpha prep
@CHECK
  if (!N("noun",2))
    fail();
  if (N("adv",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# to vg conj alpha
# to verb conj alpha
@CHECK
  if (!N("verb",4))
    fail();
  if (!vconjq(N(4),"inf"))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_verb");
  L("v") = N(4);
  pncopyvars(L("tmp4"),N(4));
  group(4,4,"_vg");
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
  N("voice",4) = "active";
  pncopyvars(L("tmp4"),N(4));
  clearpos(N(4),1,0);	# Zero out token info.
  if (pnname(N(2)) == "_vg")
    if (!N("voice",2))
	  N("voice",2) = "active";
@RULES
_xNIL <-
	to [s]
	_xWILD [one match=(_verb _vg)]
	_conj
	_xALPHA
	@@

# dqan alpha alpha prep
@CHECK
  if (N("noun",3))
    fail();
@POST
  L("tmp3") = N(3);
  if (N("verb",3))
    {
    group(3,3,"_verb");
    pncopyvars(L("tmp3"),N(3));
	L("v") = N(3);
    group(3,3,"_vg");
    mhbv(N(3),L("neg"),0,0,0,0,L("v"));
	# N("voice",3) = 0;
    pncopyvars(L("tmp3"),N(3));
	N("verb node",3) = L("v");
    clearpos(N(3),1,0);	# Zero out token info.
    }
  else if (N("adv",3))
    {
    group(3,3,"_adv");
    pncopyvars(L("tmp3"),N(3));
	}
  else if (N("adj",3))
    {
    group(3,3,"_adj");
    pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_prep)]
	@@

# det quan adj alpha prep
# dqan
@CHECK
  if (!N("noun",2))
    fail();
  # Todo: agreement.
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xWILD [one lookahead match=(_prep _qEOS _xEND)]
	@@

# vg out adj
@CHECK
  if (N("mypos",2) && N("mypos",2) != "RP")
    fail();
@POST
  L("tmp2") = N(2);
  if (!N("mypos",2)) # Could already be assigned.
    {
    group(2,2,"_particle");
    pncopyvars(L("tmp2"),N(2));
	# See if kb has something.
	if (L("num") = phrprepverbq(N(1),N(2)))
	  {
	  if (L("num") == 2) # prepositional # FIX. 06/18/06 AM.
	    chpos(N(2),"IN");
	  else # 1 or 3.
	    chpos(N(2),"RP");	# Default.
	  }
#	else	# Default.
#	  chpos(N(2),"RP");	# 01/15/05 AM.
	}
  if (pnname(N(1)) == "_vg")
    {
    if (!N("voice",1))
	  N("voice",1) = "active";
	listadd(1,2,"false");
	}
  else
    {
	L("v") = N(1);
	group(1,1,"_vg");
    mhbv(N(1),L("neg"),0,0,0,0,L("v"));
	pncopyvars(L("v"),N(1));
	clearpos(N(1),1,0);
	N("pos100 v-particle",1) = 1;
	listadd(1,2,"false");
	}
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xWILD [s one match=(out up down off)]
	_xWILD [one lookahead match=(_det _quan
		_num _xNUM _adj _noun _np _adv)]
	@@

# noun alpha alpha np
@CHECK
  if (!N("verb",3))
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
  clearpos(N(3),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_noun
	_xALPHA
	_xALPHA
	_np [lookahead]
	@@

# prep alpha prep
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
  L("neg") = mhbvadv(2,0,0,0);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("first verb",3) = N("verb node",3) = L("v");
  clearpos(N(3),1,0);	# Zero out token info.
  fixvg(N(3),"active","VBG");
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(to)]
	_xWILD [star match=(_adv _advl)]
	_xALPHA
	_xWILD [one lookahead match=(_prep _fnword)]
	@@

# np conj np
@POST
  if (N("sem",1) == "date" || N("sem",3) == "date")
    S("sem") = "date";
  singler(1,3);
@RULES
_np <-
	_np
	_conj
	_np
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

@POST
  L("tmp7") = N(7);
  group(2,7,"_np");
  pncopyvars(L("tmp7"),N(2)); # Todo: compose nps.
  N("list",2) = N("compound-np",2) = 1;
  N("ne",2) = 0;
  if (pnname(N(1)) == "_vg")
    if (!N("voice",1))
	  N("voice",1) = "active";
@RULES
_xNIL <-
	_xWILD [one match=(_prep _fnword _verb _vg)]
	_np
	\,
	_np
	\, [opt]
	_conj
	_np
	@@

# noun conj np
@POST
  L("tmp2") = N(2);
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);	# Zero out token info.
  group(2,4,"_np");
@RULES
_xNIL <-
	_xSTART
	_noun
	_conj
	_np
	@@

# at the same time ,
@CHECK
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
@RULES
_xNIL <-
	_xWILD [s one match=(at)]
	_det [opt]
	_adj
	_xALPHA
	_xWILD [lookahead one match=(\, )]
	@@

# same alpha as
@CHECK
  if (!N("noun",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
@RULES
_xNIL <-
	same [s]
	_xALPHA
	@@

# alpha up
@CHECK
  if (!N("verb",1))
    fail();
  L("v") = vconj(N(1));
  if (L("v") != "-edn"
   && L("v") != "-en"
   && L("v") != "-ing")
   fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  pncopyvars(L("tmp1"),N(1));
@RULES
_xNIL <-
	_xALPHA
	_xWILD [s one match=(up out)]
	@@

# vg adj alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  if (N("adj",3))
    group(3,3,"_adj");
  else if (N("noun",3))
    group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  if (pnname(N(3)) == "_adj")
    fixadj(N(3));
@RULES
_xNIL <-
	_xWILD [one match=(_vg _verb)]
	_adj
	_xALPHA
	@@

# vg to vg
# used to make
#@POST
# L("tmp1") = N(1);
# L("tmp4") = N(4);
# L("v") = N("verb node",4);
# if (L("v"))
#   {
#   chpos(L("v"),"VB"); # infinitive.
#   pnreplaceval(L("v"),"inf",1);
#   }
#
# # If ambiguous...
# fixvg(N(1),"active","VBD");
#
# group(1,4,"_vg");
# pncopyvars(L("tmp4"),N(1));
# N("voice",1) = 0;  # Todo: Compose voice here...
# N("first vg",1) = L("tmp1");
# N("last vg",1) =  L("tmp4");
# N("pattern",1) = "vg-to-vg";
#@RULES
#_xNIL <-
#	_vg
#	_adv [star]
#	to [s]
#	_vg
#	@@

# vg as alpha
@CHECK
  if (!N("noun",3) && !N("unknown",3))
    fail();
@POST
  pnrename(N(2),"_prep");
  chpos(N(2),"IN");
@RULES
_xNIL <-
	_vg
	as [s]
	_xALPHA
	@@

# np , np
# person , age
# apposition.
@CHECK
  if (N("sem",3) != "specific_age")
    fail();
@POST
  group(1,4,"_np");
  N("sem",1) = "person";
@RULES
_xNIL <-
	_np
	\,
	_np
	\,
	@@

# Looks too old.	# 06/21/06 AM.
# happy to oblige
#@POST
#  if (N("voice",3) == "active")
#    {
#	# Fix to infinitive.
#	L("v") = N("verb node",3);
#   chpos(L("v"),"VB");
#	}
#  L("tmp3") = N(3);
#  group(1,1,"_adjc");
#  group(1,3,"_vg");
#  pncopyvars(L("tmp3"),N(1));
#  N("pattern",1) = "adj-to-v";
#@RULES
#_xNIL <-
#	_adj
#	to [s]
#	_vg
#	@@

