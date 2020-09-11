###############################################
# FILE: qsent50.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 20/Jul/04 14:52:10
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent

@CHECK
  if (!N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_verb");
  L("v") = N(2);
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_vg");
  mhbv(N(2),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp2"),N(2));
  N("voice",2) = "active";
  clearpos(N(2),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	_proObj [s lookahead]
	@@

# np and np
@POST
  if (pnname(N(5)) == "_vg")
    if (!N("voice",5))
	  N("voice",5) = "active";
  group(2,4,"_np");
  N("compound-np",2) = 1;
@RULES
_xNIL <-
	_xSTART
	_np
	_conj
	_np
	_xWILD [one lookahead match=(_adv _advl _vg)]
	@@

# np of np and np
@POST
  group(3,5,"_np");
@RULES
_xNIL <-
	_np
	of [s]
	_np
	_conj
	_np
	_xWILD [one lookahead match=(_vg)]
	@@

# np of np
@POST
  group(1,3,"_np");
@RULES
_xNIL <-
	_np
	of [s]
	_xWILD [one match=(_np _np)]
	@@

# after np vg
@CHECK
  L("n") = pndown(N(1));
  if (strtolower(pnname(L("n"))) != "after")
    fail();
@POST
  group(1,1,"_fnword");
@RULES
_xNIL <-
	_prep
	_np
	_xWILD [one lookahead match=(_vg)]
	@@

# states, " ...
@POST
  L("tmp1") = N(1);
  group(1,1,"_verb");
  L("v") = N(1);
  pncopyvars(L("tmp1"),N(1));
  group(1,1,"_vg");
  mhbv(N(1),L("neg"),0,0,0,0,L("v"));
  pncopyvars(L("tmp1"),N(1));
  N("voice",1) = "active";
  clearpos(N(1),1,0);	# Zero out token info.
@RULES
_xNIL <-
	_xWILD [one match=(say said says state stated states)]
	\, [s lookahead opt]
	\"
	@@

# Some embedded/relative clause handling.
# np vg np vg
@CHECK
  if (N("voice",5))  # If verb resolved...
    fail();
  if (N("pattern",5) != "vg-to-vg")
    fail(); # Else, no way to be passive voice.
  if (N("glom",5) == "left"
   || N("glom",7) == "right")
   fail();
  # If inf or -ing, can't resolve with this verb lookahead.
  if (!N("noninf",9) || N("-ing",9))
    fail();

  # Make sure first verb in vg can be -en conjugation.
  S("ven") = findven(N(5));
  if (!S("ven"))
    fail(); # Error.
@POST
  # Set passive conjugation.
  pnreplaceval(S("ven"),"-en",1);

  if (!N("voice",5))
	  N("voice",5) = "passive";
  group(5,7,"_clause");
  group(5,5,"_advl");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _clause)]
	_xWILD [star match=(_adv _advl)]
	_np
	_xWILD [star match=(_adv _advl)]
	_vg
	_xWILD [star match=(_adv _advl)]
	_np
	_xWILD [star lookahead match=(_adv _advl)]
	_vg
	@@

# Else, clause the first verb.
# 2nd should be passive...
# np vg np vg
# WEAK RULE. # 10/22/04 AM.
@CHECK
  # Make sure first verb in vg can be -en conjugation.
  S("ven") = findven(N(9));
  if (!S("ven"))
    fail();
@POST
  if (N("glom",5) == "left")	# 04/21/07 AM.
	group(2,5,"_clause");	# 04/21/07 AM.
  else
	{
	# Set passive conjugation.
	pnreplaceval(S("ven"),"-en",1);
	N("voice",9) = "passive";
	if (!N("voice",5))
	  N("voice",5) = "passive";
  	group(2,8,"_clause");
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=(_adv _advl)]
	_np
	_xWILD [star match=(_adv _advl)]
	_vg
	_xWILD [star match=(_adv _advl)]
	_np
	_xWILD [star match=(_adv _advl)]
	_vg [lookahead]
	@@

# prep np np
# prep np date
@CHECK
  if (N("sem",4) != "date")
    fail();
@POST
  L("tmp4") = N(4);
  group(4,4,"_advl");
  pncopyvars(L("tmp4"),N(4));
  clearpos(N(4),1,0);
@RULES
_xNIL <-
	_prep
	_np
	_xWILD [star match=(_advl _adv)]
	_np
	@@

# prep np
@POST
  pncopyvars(3);
  if (S("ne"))
    S("ne") = 0;
  S("prep") = N(1);
  S("pp") = 1;
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
#	_xWILD [s one match=(_prep) except=(to)]
	_xWILD [one match=(_prep)]
	_adv [star]
	_np
	_xWILD [one lookahead fail=(_conj)]
	@@

# , advl ,
# , adv ,
@POST
  pncopyvars(2);
  sclearpos(1,0);
  single();
@RULES
_advl <-
	\, [s]
	_xWILD [one match=(_adv _advl)]
	\, [s]
	@@


# vg advl vg
# vg by-np vg
@CHECK
  if (!N("by-np",2))
    fail();
@POST
  L("v") = N("verb node",1);
  if (L("v"))
    fixverb(L("v"),"passive",0);
  if (!N("voice",1))
	  N("voice",1) = "passive";
  S("passive") = 1;
  singler(1,2);
@RULES
_clause <-
	_vg
	_advl
	_vg [lookahead]
	@@


# parallel construction.
# vg np conj vg
@CHECK
  if (!N("verb node",1))
    fail();
  if (!N("verb node",7))
    fail();
  if (pnvar(N("verb node",7),"mypos"))
  	fail();
  S("pos") = pnvar(N("verb node",1),"mypos");
  if (!S("pos"))
    fail();
@POST
  # Get verb pos from one and set to the other.
  pnreplaceval(N("verb node",7),"mypos",S("pos"));
  # Match up voice here as well.
@RULES
_xNIL <-
	_vg
	_xWILD [star lookahead match=(_adv _advl)]
	_np [opt]
	_xWILD [star match=(_adv _advl)]
	_conj
	_xWILD [star match=(_adv _advl)]
	_vg
	@@

# Appositive.
# than np , np , vg
@POST
  fixvg(N(6),0,0);
  group(2,4,"_np");
@RULES
_xNIL <-
	than [s]
	_np
	\,
	_np
	\,
	_vg
	@@

# np, np and np, vg
# complex appositive.
# note that it competes with an np list.
@POST
  group(4,6,"_np");
  group(2,5,"_np");
  N("apposition",2) = 1;
@RULES
_xNIL <-
    _xSTART						### (1)
    _np							### (2)
    \,							### (3)
    _np							### (4)
    _conj							### (5)
    _np							### (6)
    \,							### (7)
    _xWILD [one lookahead match=(_vg)]	### (8)
    @@

# simple appositive.
@CHECK
  if (N("mypos",2) != "NP"
   && N("mypos",4) != "NP")
     fail();
@POST
  group(2,5,"_np");
  N("apposition",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART)]
	_np
	\,
	_np
	\,
	@@

# ^ np , np that vg np ,
# appositive "clause".	# 04/19/07 AM.
@POST
  group(2,15,"_np");
  N("apposition",2) = 1;
@RULES
_xNIL <-
    _xSTART								### (1)
    _xWILD [star match=(_advl _adv)]	### (2)
    _np									### (3)
    _xWILD [star match=(_advl _adv)]	### (4)
    \,									### (5)
    _xWILD [star match=(_advl _adv)]	### (6)
    _np									### (7)
    _xWILD [star match=(_advl _adv)]	### (8)
    _xWILD [s one match=(that which)]	### (9)
    _xWILD [star match=(_advl _adv)]	### (10)
    _vg									### (11)
    _xWILD [star match=(_advl _adv)]	### (12)
    _np									### (13)
    _xWILD [star match=(_advl _adv)]	### (14)
    \,									### (15)
    @@

# np , vg np ,
@POST
  group(4,7,"_clause");
#  group(3,3,"_advl");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _qEOS)]
	_np
	\,
	_vg
	_xWILD [star match=(_advl _adv)]
	_np [opt]
	_xWILD [star match=(_advl _adv)]
	\,
	@@

# like
@CHECK
  if (N("qsent50-like",3))
    fail();
@POST
  pnrename(N(3),"_prep");
  N("qsent50-like",3) = 1;
@RULES
_xNIL <-
	_prep
	_np
	like [s]
	@@

# like
@CHECK
  if (N("qsent50-like",2))
    fail();
@POST
  pnrename(N(2),"_prep");
  N("qsent50-like",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_advl _vg)]
	like [s]
	@@

# np
# date
@CHECK
  if (N("sem",2) != "date")
    fail();
@POST
  group(2,2,"_advl");
@RULES
_xNIL <-
	_xSTART
	_np
	_xWILD [one lookahead match=( \, _np _noun _advl)]
	@@

# np np
# np date
@CHECK
  if (N("sem",3) != "date")
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_advl");
  pncopyvars(L("tmp3"),N(3));
  clearpos(N(3),1,0);
@RULES
_xNIL <-
	_np
	_xWILD [star match=(_advl _adv)]
	_np
	@@

# verb alpha date
@CHECK
  if (N("sem",3) != "date")
    fail();
  if (!N("noun",2))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_advl");
  pncopyvars(L("tmp3"),N(3));
  
  L("tmp2") = N(2);
  group(2,2,"_noun");
  pncopyvars(L("tmp2"),N(2));
  group(2,2,"_np");
  pncopyvars(L("tmp2"),N(2));
  clearpos(N(2),1,1);
@RULES
_xNIL <-
	_xWILD [one match=(_verb _vg)]
	_xALPHA
	_np [lookahead]
	@@

# alpha np
# alpha date
@CHECK
  if (N("sem",3) != "date")
    fail();
  if (N("verb",1))
    fail();
  if (!N("adj",1) && !N("noun",1))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_advl");
  pncopyvars(L("tmp3"),N(3));
@RULES
_xNIL <-
	_xALPHA
	_xWILD [star match=(_advl _adv)]
	_np
	@@


# np , alpha adv
@CHECK
  if (!N("adj",3))
    fail();
@POST
  L("tmp3") = N(3);
  group(3,3,"_adj");
  pncopyvars(L("tmp3"),N(3));
  fixadj(N(3));
@RULES
_xNIL <-
	_np
	\,
	_xALPHA
	_xWILD [plus lookahead match=(_advl _adv)]
	@@


# prep np
@POST
  group(2,3,"_advl");
  N("pp",2) = 1;
@RULES
_xNIL <-
	_xWILD [one match=(_pro _np)]
	_prep
	_np
	_xWILD [one fail=( \, _conj)]
	@@
