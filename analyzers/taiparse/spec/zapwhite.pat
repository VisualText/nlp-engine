###############################################
# FILE: zapwhite.pat
# SUBJ: Remove whitespace
# AUTH: AM
# CREATED: 06/Jul/04 00:58:57
# NOTE:	Assume all lexical work is done.
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  excise(1,1);
@RULES
_xNIL <-
	_xWHITE [plus]
	@@
