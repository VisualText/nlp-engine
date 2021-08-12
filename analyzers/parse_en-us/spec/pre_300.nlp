###############################################
# FILE: pre_300.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Jul/04 16:02:47
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _LINE

@POST
  excise(1,1);
@RULES
_xNIL <-
	_xWHITE [plus]
	@@

@RULES
_pos <-
	_xWILD [fail=(_slash _xWHITE) gp=_text]
	_slash
	@@

@POST
  if (G("error"))
  "err.txt" << "[Bad tagging: " << phrasetext() << "\n";
@RULES
_xNIL <-
	_xANY [plus]
	@@
