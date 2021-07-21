###############################################
# FILE: finalerr.pat
# SUBJ: A final error checking pass for RFB.
# AUTH: AM
# CREATED: 29/May/01 16:26:21
# MODIFIED:
###############################################

# If anything follows a rulefile, error.
@POST
  rfberror(2)
@RULES
_xNIL <- _RULESFILE _xANY @@

# If anything precedes a rulefile, error.
@POST
  rfberror(1)
@RULES
_xNIL <-
	_xWILD [plus fail=(_RULESFILE)]
	_RULESFILE
	@@
