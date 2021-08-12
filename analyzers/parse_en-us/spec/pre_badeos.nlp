###############################################
# FILE: pre_badeos.pat
# SUBJ: Handle bad blanklines in pretagged.
# AUTH: AM
# CREATED: 12/Oct/04 20:26:35
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@NODES _ROOT

@PRE
<2,2> lowercase();
@POST
  excise(1,1);
@RULES
_xNIL <-
	_BLANKLINE
	_xALPHA [lookahead]
	@@

@POST
  excise(2,2);
@RULES
_xNIL <-
	_xWILD [one match=(_xALPHA _xNUM _tok)]
	_BLANKLINE
	@@

