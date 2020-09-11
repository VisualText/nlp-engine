###############################################
# FILE: poserr.pat
# SUBJ: Highlight mismatches to pretagged text.
# AUTH: AM
# CREATED: 03/Sep/04 00:20:01
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@MULTI _ROOT

# Highlight mismatches to pretagged text.
@POST
  noop();
@RULES
_xNIL <-
	_poserr
	@@
