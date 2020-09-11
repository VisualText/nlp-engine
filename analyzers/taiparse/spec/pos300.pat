###############################################
# FILE: pos300.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 12/Aug/04 09:56:32
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

# leading parts that turn up...
# that
@CHECK
  if (!N("verb",4))
    fail();
  S("v") = vconj(N(4));
  if (S("v") != "inf"
   && S("v") != "-s")
   fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_verb");
  L("v") = N(4);
  pncopyvars(L("tmp4"),N(4));
  # Set pos.
  if (S("v") == "inf")
    chpos(N(4),"VBP");
  else if (S("v") == "-s")
    chpos(N(4),"VBZ");
  group(4,4,"_vg");
  mhbv(N(4),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp4"),N(4));
  N("voice",4) = "active";
  clearpos(N(4),1,0);
  # "that" here is a wh-pronoun.  but treebank
  # tags as WDT. Questionable.
#  chpos(N(3),"WDT");
#  N("bracket",3) = 1;
@RULES
_xNIL <-
	_vg
	_np
	that [s]
	_xALPHA
	_xWILD [s one lookahead match=(up out)]
	@@

# apposition
@CHECK
  if (N("mypos",1) != "NP" && N("mypos",3) != "NP")
    fail();
@POST
  if (N("mypos",1) == "NP")
    L("n") = N(1);
  else
    L("n") = N(3);
  if (N("$text",4) == ",")
    group(1,4,"_np");
  else
    group(1,3,"_np");
  pncopyvars(L("n"),N(1));
  N("apposition",1) = 1;
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_np
	\,
	_np
	_xWILD [one match=(_qEOS _xEND \,)]
	@@

# apposition
@POST
  group(2,5,"_np");
  N("apposition",2) = 2;
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_np
	\,
	_np
	\,
	_xWILD [one lookahead match=(_vg _verb)]
	@@

# Clausal pattern.
# np vg .
@CHECK
  S("v") = N("verb node",3);
  if (!S("v"))
    fail();
  if (pnvar(S("v"),"mypos") || pnvar(S("v"),"voice"))
    fail();
@POST
  # Ambiguous -edn ...
  fixvg(N(3),"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=( \, )]
	_np
	_vg
	_xWILD [one lookahead match=(_qEOS _xEND)]
	@@

# np , np , np , and np
@POST
  pncopyvars(8);
  single();
@RULES
_np <-
	_np
	\,
	_np
	\,
	_np
	\, [opt]
	_conj
	_np
	@@

# Look for complex appositives...
@POST
  L("tmp2") = N(2);
  group(4,7,"_np");
  group(2,5,"_np");
  pncopyvars(L("tmp2"),N(2));
  N("apposition",2) = 1;
@RULES
_xNIL <-
	_xWILD [one fail=( \, )]
	_np
	\,
	_np
	_conj
	_np
	_xWILD [star match=(_prep _np _advl _adv)]
	\,
	_xWILD [one lookahead fail=(_np)]
	@@
_xNIL <-
	_xSTART
	_np
	\,
	_np
	_conj
	_np
	_xWILD [star match=(_prep _np _advl _adv)]
	\,
	_xWILD [one lookahead fail=(_np)]
	@@

# np , np , and np
@CHECK
  if (N(5))
    succeed();
  if (N("sem",2) == "date"
   && N("sem",4) != "date")
    S("last-two") = 1; # Just last two in a list.
@POST
  pncopyvars(8);
  sclearpos(1,0);
  if (S("last-two"))
    singler(4,8);
  else
    singler(2,8);
@RULES
_np <-
	_xWILD [one fail=( \, )]
	_np
	\,
	_np
	\, [opt]
	_conj
	_xWILD [star match=(_advl _adv)]
	_np
	@@

# prep np vg by np
@POST
  if (N("sem",5) != "date") # Or other quan  # 01/06/05 AM.
    fixvg(N(3),"passive","VBN");
  else
    fixvg(N(3),"active","VBD");
@RULES
_xNIL <-
	_xWILD [one match=(_prep)]
	_xWILD [one match=(_np)]
	_vg
	by [s lookahead]
	_np
	@@

# now
@POST
  pnrename(N(3),"_adv");
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_adv _advl)]
	now [s]
	@@

# between np
@CHECK
  if (!N("compound-np",2))
    fail();
@POST
  listadd(2,1,"false");
@RULES
_xNIL <-
	between [s]
	_np
	_xWILD [one lookahead match=(_prep)]
	@@
