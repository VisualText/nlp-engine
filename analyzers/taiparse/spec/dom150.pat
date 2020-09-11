###############################################
# FILE: dom150.pat
# SUBJ: Lex, idioms, domain-specific look at tokens.
# AUTH: AM
# CREATED: 16/Oct/04 16:33:15
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _tok

# Treebank note: JJ is the default, since alpha-alpha
# nonheads of nps are about 77% adjective.
@POST
  X("hyphenated") = 1;
  X("pos_np") = "JJ";	# Default.	# 06/07/06 AM.
  if (strisupper(N("$text",2)) || strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_adj");
  chpos(X(3),"JJ");
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

# Treebank note: JJ is the default, since alpha-alpha
# nonheads of nps are about 77% adjective.
@POST
  X("hyphenated") = 1;
  X("pos_np") = "NN";
  if (strisupper(N("$text",2)) || strisupper(N("$text",4)))
    X("cap") = 1;
  xrename("_adj");
  chpos(X(3),"NN");
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

