###############################################
# FILE: pre_zap2.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Jul/04 16:41:25
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
	_text
	@@


@POST
  excise(1,1);
@RULES
_xNIL <-
	_slash
	@@
