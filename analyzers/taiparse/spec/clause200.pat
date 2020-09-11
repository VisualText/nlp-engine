###############################################
# FILE: clause200.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 21/Sep/04 22:42:27
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@PATH _ROOT _TEXTZONE _sent _clause

# Zap clause start temporary nodes.
@POST
  # Copy any attrs from the node to the clause.
  pncopyvars(N(1),X(4));
  splice(1,1);
@RULES
_xNIL <-
	_clausestart
	@@
