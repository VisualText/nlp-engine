###############################################
# FILE: pos10.pat
# SUBJ: Local, high confidence pos tagging in sentence.
# AUTH: AM
# CREATED: 29/May/07 09:53:13
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _sent
@NODES _sent

# det alpha
# NIBBLE.
@CHECK
  if (N("unknown",3))
    succeed();
  if (N("number",1) == "singular" && plural(N(3)))
	fail();
  if (!N("verb",3))
    fail();
  if (N("pos num",3) < 2)
    fail();
@POST
  N("pos25 det-a-a") = 1;
  if (N("unknown",3))
    alphatonoun(3);
  else
    {
	if (vconjq(N(3),"-en") && !N("noun",3) && !N("adj",3))
		L("pos") = "VBN";
	N("verb",3) = 0;	# verb = 0
	--N("pos num",3);
	if (N("noun",3) && N("pos num",3) == 1)
 		alphatonoun(3);	# 04/21/07 AM.
	if (L("pos"))
		N("mypos",3) = L("pos");
	}
@RULES
_xNIL <-
	_det
	_adv [star]
	_xALPHA
	@@

# det alpha , alpha
@PRE
<2,2> var("adj");
<4,4> var("adj");
@POST
  fixnpnonhead(2);
@RULES
_xNIL <-
	_det
	_xALPHA
	\, [lookahead]
	_xALPHA
	@@

# ^ num alpha
@CHECK
  if (N("noun",3))
  	{
	if (numbersagree(N(2),N(3)))
	  {
	  S("pos") = "noun";
	  succeed();
	  }
	else if (N("verb",3))
	  {
	  S("pos") = "verb";
	  succeed();
	  }
	}
  else if (N("verb",3))
  	{
	S("pos") = "verb";
	succeed();
	}
  fail();
@POST
  if (S("pos") == "noun")
     fixnphead(3);
  else if (S("pos") == "verb")
  	alphatovg(3,0,0);
@RULES
_xNIL <-
	_xSTART
	_num
	_xALPHA
	_xWILD [one fail=(_xALPHA _adj _noun)]
	@@

# prep alpha conj alpha
# Note: Do an intersection of pos!
@PRE
<2,2> varz("pos10 p-a-x-a");
<2,2> varne("stem","to");
@CHECK
  # Looking for non-verb agreement.
  if (N("noun",3) && N("noun",5))
    S("noun") = 1;
  if (N("adj",3) && N("adj",5))
    S("adj") = 1;
  if (S("adj") && S("noun"))
    fail();	# Ambig.
@POST
  N("pos10 p-a-x-a",2) = 1;
  if (S("noun"))
  	{
	alphatonoun(5);
	alphatonoun(3);
	}
  else if (S("adj"))
  	{
	alphatoadj(5);
	alphatoadj(3);
	}
@RULES
_xNIL <-
	_xWILD [one match=(_noun _np)]
	_prep
	_xALPHA
	_conj
	_xALPHA
	@@

#######################3
# NOUN LISTING	# 11/13/12 AM.
@PRE
<1,1> var("prenoun");
@POST
  if (literal(N(2)))
    alphatonoun(2);
  if (pnname(N(2)) != "_np")
    group(2,2,"_np");
  ++N("noun-list",2);
@RULES
_xNIL <-
	_prep
	_xWILD [one match=(_xALPHA _noun)]
	_xWILD [one lookahead match=( \, and _and)]
	@@

# NOUN LISTING	# 11/13/12 AM.
@PRE
<1,1> var("prenoun");
<4,4> var("noun");
@POST
  if (literal(N(4)))
    alphatonoun(4);
  if (pnname(N(4)) != "_np")
    group(4,4,"_np");
  N("noun-list",2) = 1;
  listadd(2,4,"true");
@RULES
_xNIL <-
    _prep														### (1)
    _xWILD [one match=(_np)]									### (2)
    _xWILD [plus match=( \, and _and _conj)]					### (3)
    _xWILD [one match=(_xALPHA _noun)]							### (4)
    _xWILD [one lookahead match=( \, and _and _conj _qEOS _xEND)]	### (5)
    @@


