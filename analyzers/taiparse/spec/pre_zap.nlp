###############################################
# FILE: pre_zap.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Jul/04 16:36:43
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@NODES _ROOT

@POST
  excise(1,1);
@RULES
_xNIL <- _xWILD [one match=(
	_COMMENT
	)] @@

@POST
  splice(1,1);
@RULES
_xNIL <-
	_LINE
	@@
