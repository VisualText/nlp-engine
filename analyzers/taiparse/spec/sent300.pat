###############################################
# FILE: sent300.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 30/Sep/04 01:15:41
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent



# Count clauses in a sentence.
@POST
  N("clause num") = ++X("clauses");
@RULES
_xNIL <-
	_clause
	@@
