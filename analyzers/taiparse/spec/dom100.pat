###############################################
# FILE: dom100.pat
# SUBJ: Some domain stuff.
# AUTH: AM
# CREATED: 18/Jul/04 22:16:53
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

@RULES
_stockSYMBOL [layer=_acro] <-
	\(
	_xCAP
	\:
	_xCAP
	_xWILD
	\)
	@@

@POST
  pncopyvars();
  S("noun") = 1;
  S("number") = "singular";
  S("sem") = S("stem") = "ceo";
  S("mypos") = "NN";
  single();
@RULES
_noun <-
	ceo
	@@

@POST
  pncopyvars();
  S("noun") = 1;
  S("number") = "plural";
  S("sem") = S("stem") = "ceo";
  S("mypos") = "NNS";
  single();
@RULES
_noun <-
	ceos
	@@


# Zap double quotes for now...
#@POST
#  N("dblquote rt",1) = 1;
#  N("dblquote lt",5) = 1;
#  excise(3,3);
#@RULES
#_xNIL <-
#	_xWILD [one match=(_xALPHA _tok)]
#	_xWHITE [star]
#	_dblquote
#	_xWHITE [star]
#	_xALPHA [lookahead]
#	@@

# " said
@POST
  N("-edn",3) = 0;
  N("-ed",3) = 1;
  N("noun",3) = 0;
  N("verb",3) = 1;
  N("pos",3) = "_verb";
  N("pos num",3) = 1;
  N("sem",3) = N("stem",3) = "say";
  N("tense",3) = "past";
@RULES
_xNIL <-
	_dblquote
	_xWHITE [star]
	said
	@@

# Some idioms.
@PRE
<1,1> lowercase();
<3,3> cap();
@POST
  L("tmp3") = N(3);
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"NP");
  N("cap",3) = 1;
@RULES
_xNIL <-
	_xALPHA
	_xWHITE [star]
	Down [s]
	@@

# Idiomatic letter.
@PRE
<3,3> length(1);
@POST
  L("tmp3") = N(3);
  group(3,3,"_letabbr");
  group(3,3,"_noun");
  pncopyvars(L("tmp3"),N(3));
  N("cap",3) = 1;
  chpos(N(3),"NP");
@RULES
_xNIL <-
	_xALPHA
	_xWHITE [star]
	_xWILD [one match=(_xCAP) except=( A I )] # don't know yet.
	@@

# cap, usstate
@PRE
<1,1> lowercase();
<6,6> vareq("sem","us_state");
@POST
  N("cap",3) = 1;
  fixnoun(N(3));
  group(3,6,"_citystate");
  group(3,3,"_noun");
  N("cap",3) = 1;
  N("glom comma",3) = 1;
  N("sem",3) = "geoloc";
@RULES
_xNIL <-
	_xALPHA
	_xWHITE [star]
	_xCAP
	\,
	_xWHITE [star]
	_xANY
	@@

# Idiom from French.
# tour de force
@POST
  fixnphead(5);
  L("tmp3") = N(3);
  group(3,3,"_prep");
  pncopyvars(L("tmp3"),N(3));
  chpos(N(3),"FW");
  fixnpnonhead(1);
  group(1,5,"_noun");
@RULES
_xNIL <-
	tour
	_xWHITE [star]
	de
	_xWHITE [star]
	force
	@@
