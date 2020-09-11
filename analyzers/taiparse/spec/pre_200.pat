###############################################
# FILE: pre_200.pat
# SUBJ: Pre-tagged text.
# AUTH: AM
# CREATED: 15/Jul/04 15:45:56
# MODIFIED:
###############################################

@CODE
if (!G("pretagged"))
  exitpass();
@@CODE

@PATH _ROOT _LINE

# NON-special chars.
@POST
  excise(1,1);
@RULES
_xNIL <-
	\\
	\/
	@@
_xNIL <-
	\\
	\|
	@@

# Else, special chars.
@POST
  singlex(2,2);
@RULES
_slash <-
	\/
	_xWILD [plus fail=(_xWHITE)]
	@@


@POST
  excise(1,1);
@RULES
_xNIL <-
	\[
	@@
_xNIL <-
	\]
	@@
