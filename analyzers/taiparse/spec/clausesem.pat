###############################################
# FILE: clausesem.pat
# SUBJ: Register clauses in kb.
# AUTH: AM
# CREATED: 05/May/05 22:40:26
# MODIFIED:
###############################################


@CODE
if (!G("semantic processing"))
  exitpass();

if (G("verbose"))
  "dump.txt"
	<< "\n"
	<< "[PASS clausesem: Register clauses in KB.]"
	<< "\n\n";
@@CODE

@PATH _ROOT _TEXTZONE _sent

@POST
  clauseregister(N(1),X(3));
@RULES
_xNIL <-
	_clause
	@@
