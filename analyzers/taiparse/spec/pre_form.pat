###############################################
# FILE: pre_form.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 15/Jul/04 16:44:53
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@NODES _ROOT

@RULES
_LINE <-
	_xWILD [plus fail=(_BLANKLINE _HRULE)]
	@@
