###############################################
# FILE: pre_zap1.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Jul/04 16:43:24
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@NODES _ROOT

@POST
  splice(1,1);
@RULES
_xNIL <-
	_pos
	@@
