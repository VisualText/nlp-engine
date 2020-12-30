###############################################
# FILE: quotes100.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 12/May/07 22:22:27
# MODIFIED:
###############################################

@CODE
L("hello") = 0;

if (!G("dbl quotes"))
  exitpass();	# No double quotes found.
@@CODE

@NODES _TEXTZONE

@POST
  if (G("inquotes"))
	G("inquotes") = 0;
  else
	G("inquotes") = 1;
@RULES
_xNIL <-
	_dblquote
	@@

@CHECK
  if (!G("inquotes"))
	fail();
@POST
  N("quoted") = 1;
  N("cap") = 1;
  ++X("quoted caps");
@RULES
_xNIL <-
	_xCAP [s]
	@@

# Should track title capitalization...
@CHECK
  if (!G("inquotes"))
	fail();
@POST
  N("quoted") = 1;
  ++X("quoted prose");
@RULES
_xNIL <-
	_xALPHA [s]
	@@

@CHECK
  if (!G("inquotes"))
	fail();
@POST
  N("quoted") = 1;
@RULES
_xNIL <-
	_xANY
	@@

