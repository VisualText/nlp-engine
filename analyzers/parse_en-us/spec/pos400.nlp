###############################################
# FILE: pos400.pat
# SUBJ: Pos work with clauses around.
# AUTH: AM
# CREATED: 10/Oct/04 20:43:28
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# np alpha alpha dqan
@CHECK
  if (!N("verb",3))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp4") = N(4);

  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp3"),N(3));
  N("verb node",3) = L("v");
  clearpos(N(3),1,0);
  fixvg(N(3),"active","VBP");

  if (N("adj",4))
    {
	group(4,4,"_adj");
	pncopyvars(L("tmp4"),N(4));
	fixadj(N(4));
	}
  else if (N("noun",4))
    {
	group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	}
  else
    {
	group(4,4,"_adj");
	pncopyvars(L("tmp4"),N(4));
	fixadj(N(4));
	}
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xALPHA
	_xWILD [one lookahead match=(_adj _noun)]
	@@

# dqan
@CHECK
  S("last") = lasteltnode(4);
  if (N(2))
    S("first") = N(2);
  else if (N(3))
    S("first") = N(3);
  else if (N(4))
    S("first") = N(4);
  if (!numbersagree(S("first"),S("last")))
    fail();
@POST
  L("neg") = attrinrange("neg",S("first"),S("last"));
  if (L("neg"))
    S("neg") = L("neg");
  L("tmp4") = lasteltnode(4);
  pncopyvars(L("tmp4"));
  sclearpos(1,0);
  singler(2,4);
@RULES
_np <-
	_xWILD [one match=(_xSTART _verb _vg _prep)]
	_xWILD [star match=(_det _quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xWILD [one lookahead fail=(_noun _adj _xALPHA _aposS)]
	@@
_np <-
	_xWILD [one match=(_xSTART _verb _vg _prep)]
	_xWILD [star match=(_det _quan _num _xNUM)]
	_adj [star]
	_noun [plus]
	_xEND
	@@

# vg np prep dqan alpha
@CHECK
  if (!N("noun",5))
    fail();
@POST
  L("tmp5") = N(5);
  group(5,5,"_noun");
  pncopyvars(L("tmp5"),N(5));
  fixnoun(N(5));
@RULES
_xNIL <-
    # [s] because it could be a clause start.
	_xWILD [s one match=(_vg _verb)]
	_np
	_prep
	_xWILD [plus match=(_det _pro _quan _num _xNUM _adj _noun)]
	_xALPHA
	_xWILD [one lookahead match=(_fnword _qEOS _xEND _adv _advl)]
	@@

# np alpha dqan
@CHECK
  if (!N("verb",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_verb");
  pncopyvars(L("tmp3"),N(3));
  L("v") = N(3);
  group(3,3,"_vg");
  mhbv(N(3),L("neg"),0,0,0,0,L("v"));
  N("verb node",3) = L("v");
  fixvg(N(3),"active","VBP");
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_advl _adv)]
	_xALPHA
	_xWILD [one lookahead match=(_det _quan _num _xNUM _adj
		_noun _np)]
	@@

# Looks too broad and old.	# 06/10/06 AM.
# vg dqan alpha
@CHECK
  if (!N("noun",6))
    fail();
  if (!N(2) && !N(3) && !N(4) && !N(5) && !N(6))
    fail();
  if (N("stem",7) == "to")
    fail();
@POST
  L("tmp6") = N(6);
  groupone(N(6),6,"_noun",1);
  fixnoun(N(6));
  dqaninfo(2,3,4,5);
  S("olast") = 6;
  S("last") = S("lastn") = S("lastan") = N(6);
  if (!N(4) && !N(5))
    S("firstan") = S("firstn") = N(6);
  if (!N(5))
    S("firstn") = N(6);
  groupnp();
  
@RULES
_xNIL <-
	_vg
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_xWILD [one match=(_advl _adv _clausesep _prep)]
	@@

# prep np
@POST
  pncopyvars(3);
  S("prep") = N(1);
  if (strtolower(N("$text",1)) == "by")
	{
    S("by-np") = 1;
    if (N("sem",3) != "date" && N("sem",3) != "geoloc")
      S("by-actor") = 1;	# 04/21/07 AM.
	}
  sclearpos(1,0);
  singler(1,3);
@RULES
_advl <-
	_xWILD [one match=(_prep)]
	_adv [star]
	_xWILD [s one match=(_np) except=(_proSubj)]
	_xWILD [one lookahead fail=(_conj)]
	@@

# Clausal stuff...

# vg np advl vg
@CHECK
  if (N("pos400 vnv",4))
    fail();
  if (N("voice",4))
    fail();
  L("start") = N(3);
  L("end") = lasteltnode(3);
  if (!pnnameinrange("_advl",L("start"),L("end")))
    fail();
@POST
  N("pos400 vnv",4) = 1; # Loop guard.
  fixvg(N(4),"passive","VBN");
@RULES
_xNIL <-
	_vg
	_np
	_xWILD [plus match=(_advl _adv)]
	_vg
	@@


# complex nested clause.
# vg vg np adj
# ex: This makes going to the store difficult.
@CHECK
  S("stem") = nodestem(N(1));
  "zub.txt" << "stem=" << S("stem") << "\n";
  if (!finddictattr(S("stem"),"X7")) # keep it green ...
    fail();
  "zub.txt" << "here" << "\n";
  if (!vconjq(N(2),"-ing"))
    fail();
  "zub.txt" << "here" << "\n";
@POST
  group(4,4,"_clausesep");
  group(2,2,"_clausestart");
@RULES
_xNIL <-
	_vg
	_vg
	_np
	_xWILD [one match=(_adj _adjc)]
	@@

