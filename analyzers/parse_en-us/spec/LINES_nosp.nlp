###############################################
# FILE: LINES_nosp.pat
# SUBJ: Segment lines, for dicttokz analyzers.
# AUTH: AM
# CREATED: 26/Aug/11 21:55:08
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT

@PRE
<2,2> varz("NL");
<3,3> var("NL");	# The newline-before-me attribute.
@POST
  if (N("NL",1) > 1)
    {
	++X("PARS");	# Count potential paragraphs.
	++S("PAR");
	}
  singler(1,2);
@RULES
_LINE [unsealed] <-
	_xANY
	_xWILD [star fail=(_dummy)]
	_xANY [opt lookahead]
	@@
