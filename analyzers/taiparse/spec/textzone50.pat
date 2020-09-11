###############################################
# FILE: textzone50.pat
# SUBJ: Specialized text zones.
# AUTH: AM
# CREATED: 07/Jul/04 23:34:51
# MODIFIED:
###############################################

@CODE
if (!G("num lines"))
  exitpass();
@@CODE

@NODES _ROOT

@POST
  merge();
@RULES
_TEXTZONE <-
	_TEXTZONE
	_LINE [star]
	@@

	