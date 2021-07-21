###############################################
# FILE: sent125.pat
# SUBJ: Undo _EOS marker.
# AUTH: AM
# CREATED: 19/Feb/05 21:36:52
# MODIFIED:
###############################################

@CODE
# If found new sentence breaks, do this pass.
if (!G("eos"))
  exitpass();
@@CODE

#@PATH _ROOT _TEXTZONE _sent
@NODES _sent

@POST
  splice(1,1);
@RULES
_xNIL <-
	_EOS
	@@
