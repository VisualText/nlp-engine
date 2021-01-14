###############################################
# FILE: sent50.pat
# SUBJ: Pass for new sentence breaks.
# AUTH: AM
# CREATED: 19/Feb/05 19:21:21
# MODIFIED:
###############################################

@CODE
# If found new sentence breaks, do this pass.
if (!G("eos"))
  exitpass();
@@CODE

@NODES _TEXTZONE

@CHECK
  if (!N("eos"))
    fail();
@RULES
_EOS <-
	_xANY
	@@

