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

@NODES _sent

@POST
  clauseregister(N(1),X());
@RULES
_xNIL <-
	_clause
	@@
