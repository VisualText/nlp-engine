###############################################
# FILE: tok175.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 20/Jul/04 15:15:35
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _tok

# num - year - old
# alpha - alpha - alpha
@CHECK
  if (X("hyphenated"))
    fail();
@POST
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  xrename("_adj");
  X("num-year-old") = 1;
@RULES
_xNIL <-
	_xNUM
	\-
	year
	\-
	old
	@@

# num - alpha - num
# 1-for-2
# 2-out-of-3
# ...
@CHECK
  if (X("hyphenated"))
    fail();
@POST
  X("hyphenated") = 1;
  X("pos_np") = "JJ";
  xrename("_adj");
@RULES
_xNIL <-
	_xSTART
	_xNUM
	_xWILD [plus match=(_xALPHA \- )]
	_xNUM
	_xEND
	@@

# year - earlier
@CHECK
  if (X("hyphenated"))
    fail();
@POST
  X("hyphenated") = 1;
  X("pos_np") = "JJ";
  xrename("_adj");
@RULES
_xNIL <-
	year
	\-
	earlier
	@@

# alpha - and - alpha
# alpha - alpha - alpha
# 06/05/06 AM.
@CHECK
  if (X("hyphenated"))
    fail();
@POST
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  if (N("noun",1) && N("noun",5))
    {
	# Todo: Proper setup of these nouns.
	# N("mypos",1) = N("mypos",5) = "NN";
    xrename("_noun");
	X("noun") = 1;
	chpos(X(3),"NN");
	}
  X("alpha-and-alpha") = 1;
@RULES
_xNIL <-
	_xALPHA
	\-
	and
	\-
	_xALPHA
	@@

# alpha - alpha - alpha
@CHECK
  if (X("tok175 a-a-a-"))
    fail();
@POST
  xrename("_adj");
  X("adj") = 1;
  chpos(X(3),"JJ");
  X("tok175 a-a-a-") = 1;
@RULES
_xNIL <-
	_xSTART
	_xWILD [star match=( _xNUM \, \- )]
	_xALPHA
	\-
	_xALPHA
	\-
	_xALPHA
	_xWILD [star match=( \- _xALPHA)]
	_xEND
	@@

# alpha . alpha . alpha
@CHECK
  if (X("tok175 a.a.a"))
    fail();
@POST
  xrename("_noun");
  X("noun") = 1;
  chpos(X(3),"NP");
  X("tok175 a.a.a") = 1;
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	\.
	_xALPHA
	\.
	_xALPHA
	_xWILD [star match=( \. _xALPHA)]
	_xEND
	@@

	
# alpha - alpha
# alpha dash alpha
# If it gloms to a word, use that.
@CHECK
  if (X("hyphenated"))
    fail();
@POST
  L("tmp4") = N(4);
  X("hyphenated") = 1;
  L("cap2") = strisupper(N("$text",2));
  L("cap4") = strisupper(N("$text",4));
  if (L("cap2") || L("cap4"))
    X("cap") = 1;
  if (L("cap4"))
    X("pos_np") = "NP";
  else
    X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  # Try glomming.
#  L("txt") = strtolower(N("$text",2)+N("$text",4));
#  if (dictfindword(L("txt")))
#    {
#	L("t") = N("$text",2)+N("$text",4); # Preserve case.
#	group(3,3,"_dash");
#	xrename(L("t"));
#	lookupword(X(3));
#	X("id") = "tok175 a-a";
#	}
#  else
  if (N("verb",4))
    {
	L("vc") = vconj(N(4));
	if (L("vc") == "-ing"
	 || L("vc") == "-en"
	 || L("vc") == "-edn")
	  L("inflected") = 1;
	}
  if (L("inflected") && !L("cap4"))
    {
	group(4,4,"_adj");
	xrename("_adj");
	}
  else if (N("unknown",4)
   || X("cap"))	# 06/21/06 AM.
    {
	group(4,4,"_noun");
	xrename("_noun");
    chpos(X(3),"NP");
	}
  else if (N("adj",4) || L("inflected"))
    {
	group(4,4,"_adj");
	xrename("_adj");
	}
  else if (N("noun",4))
    {
	# Default to adj for now.
	L("tmp4") = N(4);
	group(4,4,"_noun");
	pncopyvars(L("tmp4"),N(4));
	fixnoun(N(4));
	L("pos") = N("mypos",4);
	xrename("_adj");
	if (pnvar(L("tmp4"),"sem") == "date")
	  chpos(X(3),"JJ");  # eg., "five-year".
	else
	  chpos(X(3),L("pos")); # NN or NNS...
	}
  else
    {
	# Default to adj for now.
	group(4,4,"_adj");
	xrename("_adj");
	}
  X("tok175 a-a") = 1;
@RULES
_xNIL <- # 7
	_xSTART
	_xALPHA # [s] buggy -- can't reduce nonlit to lit.
	\-
	_xALPHA # [s] buggy
	_xEND
	@@

# alpha - num
@CHECK
  if (X("hyphenated"))
    fail();
@POST
  X("hyphenated") = 1;
  xrename("_noun");
  if (strisupper(N("$text",2)))
    chpos(X(3),"NP");
  # Check plural etc.
  pncopyvars(N(2),X(3));
@RULES
_xNIL <-
	_xSTART
	_xALPHA
	\-
	_xNUM
	_xEND
	@@

# inc.'s
@POST
  X("dissolve") = 1;	# Get rid of _tok node.
  group(4,5,"_aposS");
  group(2,3,"_noun");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _conj)]
	_xALPHA [s]
	\. [opt]
	\'
	s
	_xEND
	@@

# 06/15/06 AM.
@POST
  X("dissolve") = 1;	# Get rid of _tok node.
  group(4,5,"_aposD");
  group(2,3,"_noun");
@RULES
_xNIL <-
	_xWILD [one match=(_xSTART _conj)]
	_xALPHA [s]
	\. [opt]
	\'
	d
	_xEND
	@@

#
@POST
  group(2,5,"_nounCountry");
  group(2,2,"_noun");
@RULES
_xNIL <-
	_xSTART
	U
	\.
	S
	\.
	_xWILD [one lookahead match=( \- )]
	@@

@POST
  group(2,5,"_nounCountry");
  group(2,2,"_letlet");
  chpos(X(3),"NP");
  X("cap") = 1;
  xrename("_noun");
@RULES
_xNIL <-
	_xSTART
	U
	\.
	S
	\.
	_xEND
	@@

@PRE
<2,2> length(1);
<4,4> length(1);
@POST
  group(2,5,"_letlet");
  chpos(X(3),"NP");
  xrename("_noun");
@RULES
_xNIL <-
	_xSTART
	_xCAP
	\.
	_xALPHA
	\. [opt]
	_xEND
	@@

@POST
  if (strislower(N("$text",4)) )
    {
    group(4,4,"_adj");
    xrename("_adj");
	}
  else
    {
	group(4,4,"_noun");
	group(2,4,"_caps");
	xrename("_noun");
	X("cap") = 1;
	X("glom") = 1;
	}
@RULES
_xNIL <-
	_xSTART
	_noun
	\-
	_xALPHA
	_xEND
	@@

@POST
  group(1,3,"_name");
  xrename("_noun");
  X("cap") = 1; # Flag capitalized token.
@RULES
_xNIL <-
	o
	\'
	_xALPHA
	@@

# num - alpha
# num-alpha
@CHECK
  if (!N("noun",4) && !N("adj",4))
    fail();
@POST
  L("tmp4") = N(4);
  group(2,4,"_adj");
  pncopyvars(L("tmp4"),N(2));
  N("hyphenated",2) = 1;
@RULES
_xNIL <-
	_xSTART
	_num
	\-
	_xALPHA
	_xEND
	@@


# 's
# ' s
@CHECK
  if (pnname(X(3)) == "_aposS")
    fail();	# Guard against infinite loop here.
@POST
  xrename("_aposS");
  X("id") = "tok175 aposS";
  X("nopos") = 0;	# Valid possessive or verb pos for this.
@RULES
_xNIL <-
	\'
	s [s]
	@@

# 'd
# ' d
@CHECK
  if (pnname(X(3)) == "_aposD")
    fail();	# Guard against infinite loop here.
@POST
  xrename("_aposD");
  X("id") = "tok175 aposD";
  X("aposD") = 1;
  X("nopos") = 0;	# Valid possessive or verb pos for this.
@RULES
_xNIL <-
	\'
	d [s]
	@@

# 'll
# ' ll
@CHECK
  if (pnname(X(3)) == "_modal")
    fail();	# Guard against infinite loop here.
@POST
  xrename("_modal");
  X("id") = "tok175 aposLL";
  X("aposLL") = 1;
  X("nopos") = 0;	# Valid possessive or verb pos for this.
  X("stem") = X("sem") = "will";
  chpos(X(3),"MD");
@RULES
_xNIL <-
	\'
	ll [s]
	@@

# 've
# ' ve
@CHECK
  if (pnname(X(3)) == "_verb")
    fail();	# Guard against infinite loop here.
@POST
  xrename("_verb");
  X("id") = "tok175 aposVE";
  X("aposVE") = 1;
  X("nopos") = 0;	# Valid possessive or verb pos for this.
  X("stem") = X("sem") = "have";
  chpos(X(3),"VBP");
  group(1,2,"_have");
@RULES
_xNIL <-
	\'
	ve [s]
	@@

# 'm
# ' m
@CHECK
  if (pnname(X(3)) == "_verb")
    fail();	# Guard against infinite loop here.
@POST
  xrename("_verb");
  X("id") = "tok175 aposM";
  X("aposM") = 1;
  X("nopos") = 0;	# Valid possessive or verb pos for this.
  X("stem") = X("sem") = "be";
  chpos(X(3),"VBP");
  group(1,2,"_be");
@RULES
_xNIL <-
	\'
	m [s]
	@@

# 100-year
# num - alpha
# num-alpha
@POST
  L("tmp4") = N(4);
  # Reduce to prevent infinite loop in recursive pass.
  if (N("noun",4))
    group(4,4,"_noun");
  else
    group(4,4,"_misc");
  pncopyvars(L("tmp4"),N(4));
  xrename("_adj");
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\-
	_xALPHA
	_xEND
	@@

# year-to-year
# Idiom...
@CHECK
  if (X("adj"))
    fail();	# Loop guard.
@POST
  xrename("_adj");
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  X("adj") = 1;
@RULES
_xNIL <-
	_xSTART
	year
	\-
	to
	\-
	year
	_xEND
	@@

# adj - any
@CHECK
  if (X("adj"))
    fail(); # Loop guard.
@POST
  xrename("_adj");
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  X("adj") = 1;
  clearpos(X(3),1,0);
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(_adj _num)]
	\-
	_xANY
	_xEND
	@@

# near - alpha
@CHECK
  if (X("detok"))
    fail();
@POST
  X("detok") = 1;
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  chpos(N(1),"JJ");	# Near is adj here.
  L("tmp3") = N(3);
  # Discard verb...
  if (N("noun",3))
    {
	group(3,3,"_noun");
	xrename("_noun");
	pncopyvars(L("tmp3"),N(3));
	}
  else
    {
	group(3,3,"_adj");
	xrename("_adj");
	pncopyvars(L("tmp3"),N(3));
	}
  X("posarr len") = X("nopos") = 1;
@RULES
_xNIL <-
	near [s]
	\-
	_xALPHA
	@@

