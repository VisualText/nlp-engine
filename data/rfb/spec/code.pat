###############################################
# FILE:     CODE.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@PATH _ROOT _CODE _NLPPP

@POST
#	rfacode(1)				# 01/14/00 AM.
	rfaactions(1)			# 01/14/00 AM.
	single()
@RULES
#_CODE [base] <- _xWILD [plus match=(_ACTION)] @@
_CODE [base] <- _STMTS [plus] @@	# 01/14/00 AM.

# 09/17/03 AM.
@POST
  noop()
@RULES
_xNIL <- _xWILD [one match=(
	_xPUNCT
	_WHILECOND
	)] @@

@POST
  rfberror(1)
  single()
@RULES
_ERROR <- _xANY [plus] @@



