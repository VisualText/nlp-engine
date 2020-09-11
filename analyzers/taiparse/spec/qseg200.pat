###############################################
# FILE: qseg200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 31/Jul/04 23:00:27
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause _seg

@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
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
	}
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),X(5));
  clearpos(X(5),1,1);	# Zero out token info.
  xrename("_np");
@RULES
_xNIL <-
	_xSTART
	_pro [s]
	_xALPHA
	_xALPHA
	_xEND
	@@

# dqan
@CHECK
  if (N("verb",6)
   && X("need verb",4)) # 02/12/05 AM.
    fail();
@POST
  L("tmp6") = N(6);
  if (N(7))
    {
	if (N("adj",6) || !N("noun",6))
	  {
	  group(6,6,"_adj");
	  pncopyvars(L("tmp6"),N(6));
	  fixadj(N(6));
	  }
	else
	  {
	  group(6,6,"_noun");
	  pncopyvars(L("tmp6"),N(6));
	  }
	}
  else
	{
	group(6,6,"_noun");
	pncopyvars(L("tmp6"),N(6));
	}

  xrename("_np");
  clearpos(X(5),1,1);
@RULES
_xNIL <-
	_xSTART
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_noun [star]
	_xEND
	@@

# dqan
@POST
  fixnpnonhead(4);
  fixnpnonhead(5);
  xrename("_np");
  L("head") = lasteltnode(7);
  pncopyvars(L("head"),X(5));
  clearpos(X(5),1,1);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xALPHA
	_xALPHA
	_adj [star]
	_noun [plus]
	_xEND
	@@

# dqan
@POST
  fixnpnonhead(4);
  xrename("_np");
  L("head") = lasteltnode(6);
  pncopyvars(L("head"),X(5));
  clearpos(X(5),1,1);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xALPHA
	_adj [star]
	_noun [plus]
	_xEND
	@@
