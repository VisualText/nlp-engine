###############################################
# FILE: inclausesem.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 13/Jun/05 00:43:54
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
if (!G("semantic processing"))
   exitpass();

@@CODE

#@PATH _ROOT _TEXTZONE _sent _clause
@NODES _clause

#@CHECK
#  if (!N("ne") && !N("ne arr"))
#    fail();
@POST
  if (N("sem texts"))
	X("sem texts") = N("sem texts");
  if (N("ne") || N("ne arr"))
    L("obj") = domobjectregister(N(1),X());
@RULES
_xNIL <-
	_np
	@@

#@CHECK
#  if (!N("ne") && !N("ne arr"))
#    fail();
@POST
  if (N("sem vals"))
	X("sem vals") = N("sem vals");
  if (N("ne") || N("ne arr"))
    L("entity") = domentityregister(N(1),X());
@RULES
_xNIL <- _xWILD [one match=(
	_advl _adv
	)] @@

