###############################################
# FILE: tok100.pat
# SUBJ: Some retokenization.
# AUTH: AM
# CREATED: 05/Jul/04 23:47:31
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

########## IDIOMS

@POST
  S("sem detail") = "usa";
  S("sem") = "country";
  S("ne") = 1;
  S("ne type") = "location";
  S("ne type conf") = 95;
  S("country") = S("stem") = "usa";
  singler(2,9);
@RULES
_nounCountry [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	u
	\. [opt]
	_xWHITE [star]
	s
	\. [opt]
	_xWHITE [star]
	a [opt]
	\. [opt]
	@@
_nounCountry [layer=_noun] <-
	_xSTART
	u
	\. [opt]
	_xWHITE [star]
	s
	\. [opt]
	_xWHITE [star]
	a [opt]
	\. [opt]
	@@

# Chaff
@PRE
<1,1> length(1);
<4,4> length(1);
<7,7> length(1);
<8,8> length(1); 	 # !
@POST
  group(1,8,"_letabbr");
  N("cap",1) = 1;
  N("ne",1) = 1;
@RULES
_xNIL <-
	_xCAP
	\. [opt]
	_xWHITE [star]
	_xCAP
	\. [opt]
	_xWHITE [star]
	_xCAP
	_xWILD [star match=(_xWHITE _xCAP \. )]
	@@

#################

# Grab some letter - period sequences.

@PRE
<2,2> length(1);
<5,5> length(1);
<8,8> length(1);
@POST
  S("cap") = 1;	# 04/21/07 AM.
  singler(2,8);
@RULES
_letabbr <-
	_xWILD [one fail=(_xALPHA \.)]
	_xCAP
	\.
	_xWHITE [star]
	_xCAP
	\.
	_xWHITE [star]
	_xCAP
	\. [lookahead]
	_xWILD [one fail=(_xALPHA \.)]
	@@
_letabbr <-
	_xSTART
	_xCAP
	\.
	_xWHITE [star]
	_xCAP
	\.
	_xWHITE [star]
	_xCAP
	\. [lookahead]
	_xWILD [one fail=(_xALPHA \.)]
	@@


########## IDIOMS

# un
# u.n.
@POST
  S("sem") = S("ne type") = "organization";
  S("ne type conf") = 95;
  S("stem") = "united nations";
  S("ne") = 1;
  single();
@RULES
_caps [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	u [s]
	\. [s opt]
	_xWHITE [star]
	n [s]
	\. [s opt]
	@@
_caps [layer=_noun] <-
	_xSTART
	u [s]
	\. [s opt]
	_xWHITE [star]
	n [s]
	\. [s opt]
	@@

# US States.
@POST
  S("sem") = "us_state";
  S("ne type") = "location";
  S("ne type conf") = 85;
  S("stem") = strtolower(phrasetext());
  S("ne") = 1;
  S("mypos") = "NP";
  single();
@RULES
_usstate [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	n [s]
	\. [s opt]
	_xWHITE [star]
	_xWILD [s one match=(d j m h c y)]
	\. [s opt]
	@@
_usstate [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	s [s]
	\. [s opt]
	_xWHITE [star]
	_xWILD [s one match=(c d)]
	\. [s opt]
	@@
_usstate [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	w [s]
	\. [s opt]
	_xWHITE [star]
	v [s]
	\. [s opt]
	@@
_usstate [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	r [s]
	\. [s opt]
	_xWHITE [star]
	i [s]
	\. [s opt]
	@@
_usstate [layer=_noun] <-
	_xWILD [one fail=(_xALPHA \.)]
	d [s]
	\. [s opt]
	_xWHITE [star]
	c [s]
	\. [s opt]
	@@

# US States.
@POST
  pncopyvars(1);
  S("sem") = "us_state";
  S("ne type") = "location";
  S("ne type conf") = 85;
  S("stem") = strtolower(phrasetext());
  S("ne") = 1;
  S("mypos") = "NP";
  single();
@RULES
_usstate [layer=_noun] <-
	Conn
	\. [s]
	@@


@PRE
<2,2> length(1);
<5,5> length(1);
@POST
  S("cap") = 1;	# 04/21/07 AM.
  singler(2,6);
@RULES
_letabbr <-
	_xWILD [one fail=(_xALPHA \.)]
	_xCAP
	\.
	_xWHITE [star]
	_xCAP
	\. 
	_xWILD [one fail=(_xALPHA \.)]
	@@
_letabbr <-
	_xSTART
	_xCAP
	\.
	_xWHITE [star]
	_xCAP
	\.
	_xWILD [one fail=(_xALPHA \.)]
	@@
@POST
  S("cap") = 1;	# 04/21/07 AM.
  singler(2,3);
@RULES
_letabbr <-
	_xWILD [one fail=(_xALPHA \.)]
	_xCAPLET
	\.
	_xWILD [one lookahead fail=(_xALPHA \.)]
	@@
_letabbr <-
	_xSTART
	_xCAPLET
	\.
	_xWILD [one lookahead fail=(_xALPHA \.)]
	@@

# ...
@POST
  S("nopos") = 1;
  single();
@RULES
_qEOS <-
	_xWILD [min=3 match=(\. )]
	@@

@POST
  singler(1,1);
@RULES
_qEOS <-
	\,
	\"
	@@

@RULES
_dbldash <- _xWHITE [plus] \- [plus] _xWHITE [plus] @@

@POST
  group(2,3,"_dbldash");
  N("nopos",2) = 1;
@RULES
_xNIL <-
	_xALPHA
	\-
	\-
	_xALPHA [lookahead]
	@@


# alpha ' s
#@POST
#  N("apos-s",1) = 1;
#  excise(2,3);
#@RULES
#_xNIL <-
#	_xALPHA
#	\'
#	s
#	@@

@RULES
_aposS <-
	\' [s]
	s [s]
	@@
_aposD <-
	\' [s]
	d [s]
	@@

# aposLL
# Note: I'll ....
@POST
  group(1,2,"_modal");
  N("mypos",1) = "MD";
@RULES
_xNIL <-
	\'
	ll
	@@

@POST
  if (N(6))
  	N("quoted eos left",6) = 1;
  L("txt") = N("$text",2);
  if (L("txt") == "?")
    S("sent end") = "interrogative";
  else if (L("txt") == "!")
    S("sent end") = "exclamatory";
  singler(2,4);
@RULES
_qEOS <-	# 05/27/07 AM.
	_xWILD [one match=(_xALPHA _xNUM \] \) \> \% _noun)]
	_xWILD [plus match=( \. \: \; \? \! )]
	_xWHITE [star]
	_dblquote
	_xWHITE [star lookahead]
	_xANY
	@@

# NOTE: Trying to retain quotes within a sentence.
# alpha " alpha
@POST
    N("dblquote rt",1) = 1;
    N("dblquote lt",3) = 1;
  noop();
@RULES
_xNIL <-
	_xALPHA
	_dblquote
	_xALPHA [lookahead]
	@@

# Zap double quotes for now...
@POST
  if (N(1))
    N("dblquote rt",1) = 1;
  if (N(3))
    N("dblquote lt",3) = 1;
  excise(2,2);
@RULES
_xNIL <-
	_xANY
	_dblquote [trigger]
	_xWILD [one lookahead match=(
#		_xANY	# 05/28/07 AM.
		_xEND)]
	@@
_xNIL <-
	_xSTART
	_dblquote
	_xANY [lookahead]
	@@

# num %
@POST
  chpos(N(2),"NN");
  pncopyvars(1);
  sclearpos(1,0);
  single();
@RULES
_noun <-
	_num
	\%
	@@

# HTML/XML crud.
@POST
  group(1,3,"_dblquote");
@RULES
_xNIL <-
	\&
	_xWILD [one match=(quot)]
	\;
	@@

# Artifacts from tokenization.
# can't
# ca n't
@CHECK
  if (N("tok",2) != "n't")
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_modal");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"MD");
  N("stem",1) = N("text",1) = "can";
@RULES
_xNIL <-
	ca
	_adv
	@@

# won't
# wo n't
@CHECK
  if (N("tok",2) != "n't")
    fail();
@POST
  L("tmp1") = N(1);
  group(1,1,"_modal");
  pncopyvars(L("tmp1"),N(1));
  chpos(N(1),"MD");
  N("stem",1) = N("text",1) = "will";
@RULES
_xNIL <-
	wo
	_adv
	@@

# num num
@PRE
<2,2> var("fraction");
@POST
  group(1,2,"_num");
  N("number",1) = "plural";
@RULES
_xNIL <-
	_xNUM
	_num
	@@


# 03/04/10 AM.
@POST
  group(1,5,"_EQ");
  group(1,1,"_adv");
  chpos(N(1),"RB");	# 03/04/10 AM.
@RULES
_xNIL <- e \. _xWHITE [star] g \. @@
