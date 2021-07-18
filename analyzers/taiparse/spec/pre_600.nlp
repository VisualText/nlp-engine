###############################################
# FILE: pre_600.pat
# SUBJ: Word lookups inside pre-tagged multi-tokens.
# AUTH: AM
# CREATED: 10/Aug/04 09:02:16
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _LINE _pos _tok

@POST
  lookupword(N(1));
@RULES
_xNIL <-
	_xALPHA
	@@
