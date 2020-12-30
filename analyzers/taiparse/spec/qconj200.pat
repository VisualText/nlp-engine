###############################################
# FILE: qconj200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 30/Sep/04 01:11:05
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

@CHECK
  if (N(5))
    succeed();
  if (N("sem",2) == "date"
   && N("sem",4) != "date")
    S("last-two") = 1;
@POST
  if (S("last-two"))
    group(4,7,"_np");
  else
    group(2,7,"_np");
@RULES
_xNIL <-
	_xWILD [s one match=(_prep) except=(of)]
	_np
	\,
	_np
	\, [opt]
	_conj
	_np
	_xWILD [one lookahead match=(_verb _vg _prep)]
	@@


@POST
  group(2,4,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_np
	_conj
	_np
	_xWILD [one lookahead match=(_clausesep _xEND)]
	@@


# Looking at some other gloms.
# Compose semantics...
@POST
  L("tmp1") = N(1);
  L("tmp3") = N(3);
  L("stem") = N("stem",3);
  if (!L("stem"))
    L("stem") = N("stem",1);
  L("arr") = semnpnp(N(1),N(3));
  L("sem") = L("arr")[0];
  L("ne text") = L("arr")[1];
  group(1,3,"_np");
  pncopyvars(L("tmp1"),N(1));
  clearpos(N(1),1,0);
  N("glom np",1) = 1;
  if (L("sem"))
    N("sem") = L("sem");
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

# Some nonverb regions.
@POST
  group(2,4,"_clause");
  setunsealed(2,"true");	# 07/10/12 AM.
@RULES
_xNIL <-
	_xWILD [one match=( _xSTART _qEOS \, _dbldash )]
	_xWILD [plus match=(_np _prep _seg _advl _adv)]
	_conj
	_xWILD [plus match=(_np _prep _seg _advl _adv)]
	_xWILD [one lookahead match=( \, _dbldash _qEOS _xEND)]
	@@


