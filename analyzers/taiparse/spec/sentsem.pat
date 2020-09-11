###############################################
# FILE: sentsem.pat
# SUBJ: Register sentences in kb.
# AUTH: AM
# CREATED: 02/May/05 09:49:44
# MODIFIED:
###############################################

@CODE
if (!G("semantic processing"))
  exitpass();

if (G("verbose"))
  "dump.txt"
	<< "\n"
	<< "[PASS sentsem: Register sentences in KB.]"
	<< "\n\n";
@@CODE

@PATH _ROOT _TEXTZONE 

@POST
  sentregister(N(1));
@RULES
_xNIL <-
	_sent
	@@
