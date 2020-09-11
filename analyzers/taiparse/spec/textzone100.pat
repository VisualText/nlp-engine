###############################################
# FILE: textzone100.pat
# SUBJ: Gather lines into text zones.
# AUTH: AM
# CREATED: 05/Jul/04 20:22:42
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _ROOT

@RULES
_TEXTZONE <-
	_LINE [plus]
	@@

# Excise blank lines.
@POST
  excise(1,1);
@RULES
_xNIL <-
	_xWILD [plus match=(_BLANKLINE _xWHITE)]
	@@
