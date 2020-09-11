###############################################
# FILE: hilite_temporal.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 12/Jan/05 13:53:04
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
G("hello") = 0;
@@CODE

# This isn't catching all the temporals.
@PATH _ROOT _TEXTZONE _sent _clause
#@MULTI _ROOT

@CHECK
  if (N("sem") != "date"
   && !N("sem date")
   && !N("date node"))	# 04/20/05 AM.
    fail();
@POST
  if (G("verbose"))
  "temporal.txt" << N("$text") << "\n";
  # Registering in clause and sent.
  registerx(X(4),"temporal",N("$text"));
  registerx(X(3),"temporal",N("$text"));
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- _xWILD [one match=(
	_advl
	_np
	_fnword
	_prep
	)] @@
