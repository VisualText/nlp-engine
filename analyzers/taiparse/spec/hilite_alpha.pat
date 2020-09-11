###############################################
# FILE: hilite_alpha.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Oct/04 17:52:52
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  noop();
@RULES
_xNIL <-
	_xALPHA [plus]
	@@
