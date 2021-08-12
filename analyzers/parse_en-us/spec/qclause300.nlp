###############################################
# FILE: qclause300.pat
# SUBJ: Reasoning, evidence gathering for clauses.
# AUTH: AM
# CREATED: 07/Dec/05 12:47:52
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

#@PATH _ROOT _TEXTZONE _sent _clause
@NODES _clause

# Looking for suitable by-actor adverbial.
# For passive, will support transformation to active.
@POST
  if (N("by-actor") && !X("by-actor"))
	X("by-actor") = N("by-actor");
  if (N("subs"))
    X("last np") = N("subs")[0];
  noop();
@RULES
_xNIL <-
	_advl
	@@

@POST
  L("x3") = pnparent(X());		# 07/13/12 AM.
  X("last np") = N(1);
  if (!pnvar(L("x3"),"subject"))	# Find subject for sentence.
#    X("subject",3) = N(1);
	pnreplaceval(L("x3"),"subject",N(1));	# 07/13/12 AM.
@RULES
_xNIL <-
	_np
	@@

