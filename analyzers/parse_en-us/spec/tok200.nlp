###############################################
# FILE: tok200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 19/Jul/04 00:37:33
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _TEXTZONE

# New tokenization handlers.
@CHECK
if (!N("dissolve"))
  fail();
@POST
  splice(1,1);
@RULES
_xNIL <-
	_tok
	@@

# Looks like a lousy rule...
# prep alpha adj
@CHECK
  if (!N("adj",2))
    fail();
  if (N("verb",2))
    fail();
@POST
  L("tmp2") = N(2);
  group(2,2,"_adj");
  pncopyvars(L("tmp2"),N(2));
  fixadj(N(2));
@RULES
_xNIL <-
	_xWILD [one match=(_prep _conj)]
	_xALPHA
	_xWILD [one lookahead match=(_adj)]
	@@

# Some post-token handling...
@POST
  singler(2,2);
@RULES
_qEOS <-
	_aposS
	\.
	@@

# Any leftover periods as end of sentence...
# 10/09/04 AM.
@RULES
_qEOS <-
	\.
	@@

@POST
  S("sem") = "money";
  S("sem val") = phrasetext();
  S("ignorepos") = 1;	# 04/21/07 AM.
  # Changing from _num to _noun.
  single();
@RULES
_money [layer=_noun] <-
	_xWILD [one match=(\$ \#)]
	_xWILD [one match=(_num _xNUM)]
	_xWHITE [star]
	_xWILD [s opt match=(thousand million billion trillion)]
	@@

# $ num-a-share
@PRE
<2,2> var("num-dash-dash");
@POST
  N("sem",2) = "money";
  listadd(2,1,"false");
@RULES
_xNIL <-
	\$
	_adj
	@@

# noun - noun
@POST
	group(2,4,"_caps");
	group(2,2,"_adj");
	N("cap",2) = 1;
	N("glom",2) = 1;
@RULES
_xNIL <-
	_xWILD [one fail=( \- )]
	_noun
	\-
	_noun
	_xWILD [one fail=( \- )]
	@@

# Flag unhandled tokens.	# 02/15/05 AM.
@POST
  if (G("verbose"))
  "tok.txt" << phrasetext() << "\n";
@RULES
_xNIL <-
	_tok
	@@

# noun num
@PRE
<1,1> var("cap");
@POST
  group(1,2,"_noun");
  N("cap",1) = 1;
  clearpos(N(1),1,0);
@RULES
_xNIL <-
	_noun
	_xNUM
	_xWILD [one lookahead match=(_det _quan _num _adj _noun
		_prep _adv _verb
		)]
	@@

# Note: Handle slangy stuff like thinkin'
# Need to account for single quotes also.
@CHECK
  L("txt") = strtolower(N("$text",1));
  if (!strendswith(L("txt"),"in"))
    fail();
@POST
  N("apos",1) = 1;
  excise(2,2);
@RULES
_xNIL <-
	_xALPHA
	\'
	@@

# Some nouns that must be capitalized.
@PRE
<1,1> lowercase();
@POST
  --N("pos num",1);
  N("noun",1) = 0;
  alphaunambigred(1);
@RULES
_xNIL <- _xWILD [one match=(
	lent
	)] @@


