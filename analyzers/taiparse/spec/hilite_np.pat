###############################################
# FILE: hilite_np.pat
# SUBJ: Highlight noun phrases.
# AUTH: AM
# CREATED: 12/Jan/05 12:14:08
# MODIFIED:
###############################################

@CODE
if (G("pretagged"))
  exitpass();
G("hello") = 0;
@@CODE

# Traverse the whole tree.
@MULTI _ROOT

@POST
  if (G("verbose"))
  "np.txt" << N("$text") << "\n";
  noop();	# Merely matching the rule will set text to green.
@RULES
_xNIL <- _xWILD [one match=(
	_np _nps
	)] @@
