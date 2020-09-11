###############################################
# FILE: inclausesem.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 13/Jun/05 00:43:54
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

@CHECK
  if (!N("ne") && !N("ne arr"))
    fail();
@POST
  L("obj") = domobjectregister(N(1),X(4));
@RULES
_xNIL <-
	_np
	@@

@CHECK
  if (!N("ne") && !N("ne arr"))
    fail();
@POST
  L("entity") = domentityregister(N(1),X(4));
@RULES
_xNIL <- _xWILD [one match=(
	_advl _adv
	)] @@

