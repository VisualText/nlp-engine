###############################################
# FILE: hilite_spatial.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 13/Jan/05 14:50:01
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
if (!G("hilite"))	# 10/25/10 AM.
  exitpass();		# 10/25/10 AM.
G("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

@CHECK
  if (N("sem") != "location"
   && !N("sem location"))
    fail();
@POST
  if (G("verbose"))
  "location.txt" << N("$text") << "\n";
  # Registering in clause and sent.
  registerx(X(4),"location",N("$text"));
  registerx(X(3),"location",N("$text"));
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- _xWILD [one match=(
	_advl
	_np
	)] @@

