###############################################
# FILE: seg100.pat
# SUBJ: Segmentation pass.
# AUTH: AM
# CREATED: 13/Jul/04 19:39:28
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N(4))
    {
	if (!N("noun",4) && !N("adj",4))
	  fail();
	}
@POST
  L("tmp3") = N(3);
  L("tmp2") = N(2);
  if (N("noun",3))
    {
    group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	fixnounnonhead(N(3));
	}
  else
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}

  if (N("verb",2) && N("noun",2)
    && vconjq(N(2),"inf"))
    {
    group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnounnonhead(N(2));
	}
  else if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
    group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnounnonhead(N(2));
	}

  if (N(4))
    {
	if (N(5))
	  fixnpnonhead(4);
	else
	  fixnphead(4);
	}
  singler(1,5);
@RULES
_np <-
	_xWILD [plus match=(_det _quan _num _xNUM _adj)]
	_xALPHA
	_xALPHA
	_xALPHA [opt]
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA)]
	@@

@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
@POST
  L("tmp3") = N(3);
  L("tmp2") = N(2);
  if (N("noun",3))
    {
    group(3,3,"_noun");
	pncopyvars(L("tmp3"),N(3));
	if (N(4))	# 04/21/07 AM.
	  fixnounnonhead(N(3));
	else
	  fixnphead(3);	# 04/21/07 AM.
	}
  else
    {
	group(3,3,"_adj");
	pncopyvars(L("tmp3"),N(3));
	fixadj(N(3));
	}

  if (N("verb",2) && N("noun",2)
    && vconjq(N(2),"inf"))
    {
    group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnounnonhead(N(2));
	}
  else if (N("adj",2))
    {
	group(2,2,"_adj");
	pncopyvars(L("tmp2"),N(2));
	fixadj(N(2));
	}
  else
    {
    group(2,2,"_noun");
	pncopyvars(L("tmp2"),N(2));
	fixnounnonhead(N(2));
	}

    S("seg type") = "np";
  singler(2,4);
@RULES
_seg <-
	_xWILD [plus match=(_prep _conj)]
	_xALPHA
	_xALPHA
	_noun [star]
	_xWILD [one lookahead match=(_prep _xPUNCT _xEND _dbldash)]
	@@

# pro alpha+
# pro alpha alpha alpha ...
@CHECK
  S("o start") = 2;
  if (N(7))
    S("o end") = 7;
  else if (N(6))
    S("o end") = 6;
  else if (N(5))
    S("o end") = 5;
  else if (N(4))
    S("o end") = 4;
  else if (N(3))
    S("o end") = 3;
  else
    S("o end") = 2;
  S("ii") = S("o start");
  S("o bad") = 2;	# Cutoff point for np.
  while (S("ii") <= S("o end"))
    {
	L("n") = eltnode(S("ii"));
	if (pnvar(L("n"),"noun") || pnvar(L("n"),"adj"))
	  S("o bad") = ++S("ii");
	else
	  S("ii") = S("o end") + 1;	# terminate.
	}
  if (S("o bad") > S("o start"))
    succeed();
  else
    fail();	# 04/24/10 AM.
  
@POST
  if (!N("mypos",1))
    chpos(N(1),"PP$");	# pro/PP$
  # Figure out the np head.
  if (S("o bad") > S("o end"))
  	{
	if (!N(8))
	  {
	  fixnphead(S("o end"));
	  L("ii") = S("o end") - 1;
	  }
	else
	  {
	  L("include nouns") = 1;
	  L("ii") = S("o end");
	  }
	}
  else
    {
	S("o end") = S("o bad") - 1;
	fixnphead(S("o end"));
	L("ii") = S("o end") - 1;
	}

  while (L("ii") >= S("o start"))
    {
	fixnpnonhead(L("ii"));
	--L("ii");
	}
  # Now group a noun phrase.
  if (L("include nouns"))
    {
#	setlookahead(9);
    group(S("o start"),8,"_np");
	}
  else
    {
#    setlookahead(S("o end") + 1);
    group(S("o start"),S("o end"),"_np");
	}
@RULES
_xNIL <-
    _xWILD [s one match=(_proPoss)]				### (1)
    _xALPHA										### (2)
    _xALPHA [opt]								### (3)
    _xALPHA [opt]								### (4)
    _xALPHA [opt]								### (5)
    _xALPHA [opt]								### (6)
    _xALPHA [opt]								### (7)
    _noun [star]								### (8)
    _xWILD [one lookahead match=(
        _prep _xPUNCT _xEND _dbldash _fnword)]	### (9)
    @@

# alpha alpha alpha
@CHECK
  if (!N("noun",3) && !N("adj",3))
    fail();
  if (!N("noun",2) && !N("adj",2))
    fail();
  if (N(4))
    {
    if (!N("noun",4) && !N("adj",4))
      fail();
	}
@POST
  if (N(4))	# 04/23/07 AM.
	{
	fixnphead(4);	# 04/23/07 AM.
	fixnpnonhead(3);	# 04/23/07 AM.
	}
  else
	fixnpnonhead(3);	# 04/23/07 AM.
  fixnpnonhead(2);		# 04/23/07 AM.

#  group(2,4,"_seg");
#  N("seg type",2) = "np";
	group(2,4,"_np");
@RULES
_xNIL <-
	_xWILD [one match=(_conj _prep _verb _vg)]
	_xALPHA
	_xALPHA
	_xALPHA [opt]
	_xWILD [one lookahead match=(_prep _conj _verb _vg)]
	@@

@CHECK
  if (!N(1) && !N(2) && !N(3) && !N(4))
    fail();
  if (!N("noun",5) && !N("adj",5))
    fail();
  if (N(4) && !N("noun",5))
    fail();
  if (N("verb",5))
    fail();
@POST
  S("seg type") = "np";
  if (N(1))
    S("seg first") = "det";
  else if (N(2))
    S("seg first") = "quan";
  else if (N(3))
    S("seg first") = "adj";
  else if (N(4))
    S("seg first") = "noun";

  L("tmp5") = N(5);
  if (N(6))
	fixnpnonhead(5);
  else
    fixnphead(5);

  singler(1,6);
@RULES
_seg <-
	_det [star]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA _aposS)]
	@@

@CHECK
  if (!N("noun",5) && !N("adj",5))
    fail();
  if (N(4) && !N("noun",5))
    fail();
  if (N("verb",5))
    fail();
@POST
  S("seg type") = "np";
  S("seg first") = "pro";
  singler(1,6);
@RULES

_seg <-
	_proPoss [s]
	_xWILD [star match=(_quan _num _xNUM)]
	_adj [star]
	_noun [star]
	_xALPHA
	_noun [star]
	_xWILD [one lookahead fail=(_xALPHA)]
	@@


# Some random stuff.
# np np , alpha
@CHECK
  if (!N("apposition",1) && !N("apposition",2))
    fail();
  if (!N("noun",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_noun");
  pncopyvars(L("tmp4"),N(4));
  fixnoun(N(4));
  nountonp(4,1);
  listadd(2,3,"false");	# Get rid of appositive comma.
@RULES
_xNIL <-
	_np
	_np
	\,
	_xALPHA
	_xEND
	@@
