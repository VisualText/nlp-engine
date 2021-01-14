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

# First paragraph.
@PRE
<3,3> varz("PAR");
@RULES
_TEXTZONE [unsealed] <-
	_xSTART
	_LINE
	_LINE [star]
	@@

@PRE
<1,1> var("PAR");
<2,2> varz("PAR");
@RULES
_TEXTZONE [unsealed] <-
	_LINE
	_LINE [star]
	@@

# Excise blank lines.
@POST
  excise(1,1);
@RULES
_xNIL <-
	_xWILD [plus match=(_BLANKLINE _xWHITE)]
	@@
