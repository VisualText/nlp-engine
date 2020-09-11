###############################################
# FILE: qseg100.pat
# SUBJ: Go inside segs.
# AUTH: AM
# CREATED: 13/Jul/04 19:47:01
# NOTE:	Maybe premature to assign all to np here...
#	Should be gathering info about seg contents.
#	(Really, should be gathering during the reduce to _seg,
#	with a better NLP++ @PRE syntax...)
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _seg

# Assume we're here because this is an np.
@CHECK
  if (!N(2) && !N(3) && !N(4))
    fail();	# Handle the raw alpha alpha elsewhere.
  if (!N("noun",6))	# 01/12/05 AM.
  	fail();
@POST
  if (N("verb",6))
    X("last verb") = 1; # Last is possibly a verb.
  if (numbersagree(N(2),N(6)) )
    X("noun agree") = 1;
  X("vconj") = vconj(N(6));
  xrename("_np");
  pncopyvars(N(6),X(4));
  clearpos(X(4),1,1);	# Zero out token info.
  
  # Heur: assign pos to alphas.
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));

  L("tmp5") = N(5);
  if (N("adj",5))
    {
    group(5,5,"_adj");
	pncopyvars(L("tmp5"),N(5));
	if (N(4))
	  L("firstj") = N(4);
	else
	  L("firstj") = N(5);
	L("lastj") = N(5);
	fixadjs(L("firstj"),L("lastj"));
	fixnoun(N(6));
	}
  else if (N("noun",5))
    {
    group(5,5,"_noun");
	pncopyvars(L("tmp5"),N(5));
	fixnouns(N(5),N(6));
	if (N(4))
	  fixadjs(N(4),N(4));
	}
  else if (N("verb",5))
    {
    group(5,5,"_adj");
	pncopyvars(L("tmp5"),N(5));
	if (N(4))
	  L("firstj") = N(4);
	else
	  L("firstj") = N(5);
	fixadjs(L("firstj"),N(5));
	}
  else  # don't know; don't care anyway...
    pncopyvars(L("tmp5"),N(5));

  X("id") = "qseq100 dqa alph-alph";
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_xALPHA
	_xALPHA
	_xEND
	@@

@CHECK
  if (!N(2) && !N(3) && !N(4) && !N(5))
    fail();	# Handle the raw alpha elsewhere.
  if (!N("noun",6))
    fail();
  if (N("verb",6))
    fail();
@POST
  if (N("verb",6))
    X("last verb") = 1; # Last is possibly a verb.
#  if (numbersagree(N(2),N(6)) )
#    X("noun agree") = 1;
  X("vconj") = vconj(N(6));
  L("tmp6") = N(6);
  group(6,6,"_noun");
  pncopyvars(L("tmp6"),N(6));
  xrename("_np");
  pncopyvars(N(6),X(4));
  clearpos(X(4),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_xEND
	@@

@CHECK
  if (X("seg type") != "np")
    fail();
@POST
  L("tmp3") = N(3);
  fixnpnonhead(2);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  fixnoun(N(3));
  xrename("_np");
  pncopyvars(L("tmp3"),X(4));
  clearpos(X(4),1,1);	# Zero out token info.
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_xALPHA
	_xEND
	@@
