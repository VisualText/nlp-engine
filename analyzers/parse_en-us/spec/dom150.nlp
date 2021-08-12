###############################################
# FILE: dom150.pat
# SUBJ: Lex, idioms, domain-specific look at tokens.
# AUTH: AM
# CREATED: 16/Oct/04 16:33:15
# NOTE:	Mainly for PRETAGGED TEXTS.
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _tok
@NODES _tok

# Treebank note: JJ is the default, since alpha-alpha
# nonheads of nps are about 77% adjective.
@POST
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  if (strisupper(N("$text",2)) || strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_adj");
  chpos(X(),"JJ");
@RULES
_xNIL <-
	_xSTART
	short
	\-
	term
	_xEND
	@@
_xNIL <-
	_xSTART
	worst
	\-
	case
	_xEND
	@@
_xNIL <-
	_xSTART
	pro
	\-
	forma
	_xEND
	@@
_xNIL <-
	_xSTART
	pre
	\-
	_xWILD [one match=(_xALPHA _xNUM)]
	_xEND
	@@
_xNIL <-
	_xSTART
	_xALPHA # run, drag
	\-
	down
	_xEND
	@@
_xNIL <-
	_xSTART
	_xALPHA
	\-
	free
	_xEND
	@@

# Treebank note: JJ is the default, since alpha-alpha
# nonheads of nps are about 77% adjective.
@POST
  X("hyphenated") = 1;
  X("pos_np") = "NN";
  if (strisupper(N("$text",2)) || strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_adj");
  chpos(X(),"NN");
@RULES
_xNIL <-	# NN/JJ 15/5.
	_xSTART
	high
	\-
	technology
	_xEND
	@@
# buy-out
# buy - out
_xNIL <-	# NN/JJ 499/25
	_xSTART
	buy
	\-
	out
	_xEND
	@@
_xNIL <-	# NN/JJ 499/25
	_xSTART
	stock
	\-
	index
	_xEND
	@@
_xNIL <-
	_xSTART
	twin
	\-
	jet
	_xEND
	@@
_xNIL <-
	_xSTART
	close
	\-
	up
	_xEND
	@@
_xNIL <-
	_xSTART
	cop
	\-
	killer
	_xEND
	@@

@POST
  X("hyphenated") = 1;
  X("pos_np") = "NN";
  if (strisupper(N("$text",2)) || strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_noun");
  chpos(X(),"NNS");
@RULES

_xNIL <-
	_xSTART
	mid
	\-
	_xNUM
	s
	_xEND
	@@


@POST
  X("hyphenated") = 1;
  X("pos_np") = "NN";
  if (strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_noun");
  chpos(X(),"NN");
@RULES
_xNIL <-
	_xSTART
	t
	\-
	shirt
	_xEND
	@@
@POST
  X("hyphenated") = 1;
  X("pos_np") = "NNS";
  if (strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_noun");
  chpos(X(),"NNS");
@RULES
_xNIL <-
	_xSTART
	t
	\-
	shirts
	_xEND
	@@

# Ex: 2001-2
@PRE
<2,2> length(4);
<4,4> lengthr(1,4);
@POST
  X("hyphenated") = 1;
  X("pos_np") = "CD";
  xrename("_noun");
  chpos(X(),"CD");
@RULES
_xNIL <-
	_xSTART
	_xNUM
	\-
	_xNUM
	_xEND
	@@

@POST
  X("hyphenated") = 1;
  if (strisupper(N("$text",2)) || strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_noun");
  if (singular(N(2)))
	{
    X("pos_np") = "NN";
    chpos(X(),"NN");
	}
  else
	{
    X("pos_np") = "NNS";
    chpos(X(),"NNS");
	}
@RULES
_xNIL <-
	_xSTART
	_xWILD [one match=(passer passers)]
	\-
	by
	_xEND
	@@

@POST
  X("idiom") = 1;
  xrename("_verb");
  chpos(X(),"VB");
  X("verb") = 1;
  X("pos num") = 1;
  X("inf") = 1;
  X("imperative") = 1;
@RULES
_xNIL <-
	_xSTART
	c
	\'
	mon
	_xEND
	@@

# Note: exclusively in "single-handedly".
@POST
  N("adv") = 1;
  N("sem") = N("stem") = "handed";
  N("pos num") = 1;
  N("pos") = "_adv";
  if (N("unknown"))
	N("unknown") = 0;
@RULES
_xNIL <- handedly @@

