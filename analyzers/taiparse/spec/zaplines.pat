###############################################
# FILE: zaplines.pat
# SUBJ: Get rid of line nodes.
# AUTH: AM
# CREATED: 05/Jul/04 20:24:14
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

@POST
  ++X("line count",2);
  splice(1,1);
@RULES
_xNIL <-
	_LINE
	@@
