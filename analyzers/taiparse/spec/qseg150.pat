###############################################
# FILE: qseg150.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 21/Jan/05 19:09:13
# MODIFIED:
###############################################

@CODE
G("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _sent _clause _seg
@NODES _seg

# dqan
@CHECK
  L("x4") = pnparent(X());		# _clause # 07/10/12 AM.
  # If verbs in clause, want a better look at parallel
  # construction patterns , etc.
  if (pnvar(L("x4"),"vg count"))	# 07/10/12 AM.
    fail();
  if (X("seg type") != "np")
    fail();
@POST
  # Get a head noun.
  L("tmp6") = N(6);
  if (N(7))
    {
    L("head") = lasteltnode(7);
	fixnpnonhead(6);
#	if (N("noun",6))
#	  {
#	  group(6,6,"_noun");
#	  pncopyvars(L("tmp6"),N(6));
#	  fixnouns(N(6),N(6));
#	  }
#	else
#	  {
#	  group(6,6,"_adj");
#	  pncopyvars(L("tmp6"),N(6));
#	  fixadjs(N(6),N(6));
#	  }
	}
  else
    {
	if (N("noun",6))
	  {
	  group(6,6,"_noun");
	  pncopyvars(L("tmp6"),N(6));
	  fixnoun(N(6));
	  }
	else
	  {
	  group(6,6,"_adj");
	  pncopyvars(L("tmp6"),N(6));
	  fixadj(N(6));
	  }
	L("head") = N(6);
	}

 if (N(4))
   {
   L("tmp4") = N(4);
 	if (N("noun",4))
	  {
	  group(4,4,"_noun");
	  pncopyvars(L("tmp4"),N(4));
	  fixnoun(N(4));
	  }
	else
	  {
	  group(4,4,"_adj");
	  pncopyvars(L("tmp4"),N(4));
	  fixadj(N(4));
	  }
   }

  # Now fix up noun phrase.
  xrename("_np");
  pncopyvars(L("head"),X());
  clearpos(X(),1,1);
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xALPHA [opt]
	_adj [star]
	_xALPHA
	_noun [star]
	_xEND
	@@

# dqan
# det alpha alpha alpha noun
@POST
  fixnpnonhead(4);
  fixnpnonhead(5);
  fixnpnonhead(6);
  xrename("_np");
  L("head") = lasteltnode(7);
  pncopyvars(L("head"),X());
  clearpos(X(),1,1);
@RULES
_xNIL <-
	_xSTART
	_xWILD [plus match=(_det _pro)]
	_xWILD [star match=(_quan _num _xNUM)]
	_xALPHA
	_xALPHA
	_xALPHA
	_noun [plus]
	_xEND
	@@
