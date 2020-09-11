###############################################
# FILE: tok25.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 20/Oct/04 13:02:22
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE

# New tokenization handlers.
@CHECK
if (!N("dissolve"))
  fail();
@POST
  splice(1,1);
@RULES
_xNIL <-
	_tok
	@@