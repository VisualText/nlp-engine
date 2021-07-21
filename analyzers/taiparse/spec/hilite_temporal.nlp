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
if (!G("hilite"))	# 10/25/10 AM.
  exitpass();		# 10/25/10 AM.
G("hello") = 0;
@@CODE

# This isn't catching all the temporals.
#@PATH _ROOT _TEXTZONE _sent _clause	# Comment.	# 07/13/12 AM.
#@MULTI _ROOT
@NODES _clause _advl	# 07/13/12 AM.

@CHECK
  if (N("sem") != "date"
   && !N("sem date")
   && !N("date node"))	# 04/20/05 AM.
    fail();
@POST
  L("x3") = pnparent(X());		# 07/13/12 AM.
  if (G("verbose"))
  "temporal.txt" << N("$text") << "\n";
  # Registering in clause and sent.
  registerx(X(),"temporal",N("$text"));
  registerx(L("x3"),"temporal",N("$text"));
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- _xWILD [one match=(
	_advl
	_np
	_fnword
	_prep
	)] @@
