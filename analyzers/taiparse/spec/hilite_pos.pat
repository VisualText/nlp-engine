###############################################
# FILE: hilite_pos.pat
# SUBJ: Highlight stray nodes not part of constituents.
# AUTH: AM
# CREATED: 09/Jul/04 20:54:44
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
if (!G("hilite"))	# 10/25/10 AM.
  exitpass();		# 10/25/10 AM.
@@CODE

#@PATH _ROOT _TEXTZONE _sent _clause
@NODES _clause

@POST
 noop();
@RULES
_xNIL <- _xWILD [one match=(
	_noun _verb _adj
	)]
	@@
