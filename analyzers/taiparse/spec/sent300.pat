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

@NODES _sent



# Count clauses in a sentence.
@POST
  N("clause num") = ++X("clauses");
@RULES
_xNIL <-
	_clause
	@@

# ^ if
@POST
  X("conditional") = 1;	# 05/08/07 AM.
@RULES
_xNIL <-
	_xSTART
	if [s]
	@@
