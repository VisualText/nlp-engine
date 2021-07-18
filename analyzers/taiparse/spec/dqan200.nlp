###############################################
# FILE: dqan200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 09/Jul/04 22:46:48
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# particle adj noun alpha
@CHECK
  if (!N("noun",6))
    fail();
  if (N("verb",6))
    fail();
  dqaninfo(2,3,4,5);
@POST
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));

  S("olast") = 6;
  S("last") = S("lastn") = S("lastan") = N(6);
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_particle _prep _conj _vg _verb)]
	_det [opt]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xALPHA
	_xWILD [one lookahead match=(_xPUNCT _qEOS _dbldash _xEND)]
	@@

# particle adj noun
@CHECK
  dqaninfo(2,3,4,5);
  if (!numbersagree(S("first"),S("last")))
    fail();
@POST
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(_particle _prep _conj _vg _verb _adv _advl \,)]
	_det [opt]
	_quan [opt]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_xALPHA _aposS _conj)]
	@@

# dqan
@CHECK
 dqaninfo(1,2,3,4);
  if (!numbersagree(S("first"),S("last")))
    fail();
@POST
    groupnp();
@RULES
_xNIL <-
	_det [star]
	_xWILD [plus match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA)]
	@@

@POST
  L("tmp") = lasteltnode(4);
  L("m") = N(1);
  L("h") = N(2);
  L("b") = N(3);
  group(1,4,"_vg");
  N("stem",1) = pnvar(L("tmp"),"stem");
  L("voice") = mhbvfix(N(1),L("m"),L("h"),L("b"),0,L("tmp"));
  pncopyvars(L("tmp"),N(1));
  if (L("voice"))
    N("voice",1) = L("voice");
  clearpos(N(1),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_modal [star]
	_have [star]
	_be [star]
	_verb [plus]
	@@
_xNIL <-
	_do [s]
	_have [star]
	_be [star]
	_verb [plus]
	@@

# dqan
@CHECK
  dqaninfo(2,3,4,5);
@POST
  groupnp();
@RULES
_xNIL <-
	_xWILD [one match=(\, )]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xEND
	@@

# as np
@CHECK
  if (N("mypos",1) == "IN")
    fail();
@POST
  pnrename(N(1),"_prep");
  posacct(N(1));
@RULES
_xNIL <-
	as [s]
	_np
	_xWILD [s star lookahead match=(_date _adv _advl _qEOS _xEND)]
	@@

# some of np
@POST
  listadd(3,1,"true");
@RULES
_xNIL <-
	some [s]
	of [s]
	_np
	@@

# nounless.
@POST
  pncopyvars(lasteltnode(2));
  sclearpos(1,0);
  singler(2,2);
@RULES
_np <-
	_xWILD [one match=(_prep _vg)]
	_adj [plus]
	_xWILD [lookahead one match=(_prep _vg _qEOS \. _np _xEND)]
	@@

@CHECK
  dqaninfo(2,3,4,5);
  if (!numbersagree(S("first"),S("last")))
    fail();
@POST
  if (N(3))
  if (!nonliteral(N(3)))
    {
	L("tmp3") = N(3);
	group(3,3,"_num");
	pncopyvars(L("tmp3"),N(3));
	}
  groupnp();

@RULES
_xNIL <-
	_xWILD [one fail=(_xALPHA _det _quan _num _xNUM _adj _noun _aposS)]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_noun _adj _xALPHA _aposS)]
	@@
_xNIL <-
	_xWILD [one fail=(_xALPHA _det _quan _num _xNUM _adj _noun)]
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xEND
	@@
