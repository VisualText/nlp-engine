###############################################
# FILE: tag50.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 01/Feb/06 19:12:34
# MODIFIED:
###############################################

@CODE
if (!G("find html"))
  exitpass();
@@CODE

@NODES _ROOT

@RULES
_commentTag <-
	\<
	\!
	\-
	\-
	@@
_EcommentTag <-
	\-
	\-
	\>
	@@

