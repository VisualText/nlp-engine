###############################################
# FILE: pre_100.pat
# SUBJ: Pre-tagged text.
# AUTH: AM
# CREATED: 15/Jul/04 15:42:29
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _LINE

@POST
  xrename("_COMMENT");
@RULES
_xNIL <-
	_xSTART
	_xWHITE [star]
	\*
	x
	\*
	@@

@POST
  xrename("_HRULE");
@RULES
_xNIL <-
	_xSTART
	_xWHITE [star]
	_xWILD [min=3 match=( \= )]
	_xWHITE [star]
	_xEND
	@@

	