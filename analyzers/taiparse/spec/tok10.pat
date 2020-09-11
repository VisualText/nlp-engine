###############################################
# FILE: tok10.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 20/Oct/04 13:01:37
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _tok


# Artifact of pretagged texts.
# --
# - -
@POST
  pncopyvars(X(3),N(4));
  X("dissolve") = 1;
  clearpos(N(2),1,0);
  clearpos(N(3),1,0);
  group(2,3,"_dbldash");
  N("nopos",2) = 1;
@RULES
_xNIL <-
	_xSTART
	\-
	\-
	_xWILD [min=2 match=(_xANY) gp=_tok]
	_xEND
	@@

# Artifact of pretagged texts.
# --
# - -
@POST
  pncopyvars(X(3),N(4));
  X("dissolve") = 1;
  clearpos(N(2),1,0);
  clearpos(N(3),1,0);
  group(2,3,"_dbldash");
  N("nopos",2) = 1;
@RULES
_xNIL <-
	_xSTART
	\-
	\-
	_xANY
	_xEND
	@@

@POST
  X("dissolve") = 1;
  clearpos(N(2),1,0);
  clearpos(N(3),1,0);
  group(2,3,"_dbldash");
  N("nopos",2) = 1;
@RULES
_xNIL <-
	_xSTART
	\-
	\-
	_xEND
	@@

# Artifact of pretagged texts.
# ...
@POST
  pncopyvars(X(3),N(2));
  X("dissolve") = 1;
  clearpos(N(3),1,0);
  clearpos(N(4),1,0);
  clearpos(N(5),1,0);
  group(3,5,"_qEOS");
  N("nopos",3) = 1;
@RULES
_xNIL <-
	_xSTART
	_xANY
	\.
	\.
	\.
	_xEND
	@@
