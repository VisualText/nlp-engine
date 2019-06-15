###############################################
# FILE: decl.pat
# SUBJ: Collect declaration region.
# AUTH: AM
# CREATED: 19/Dec/01 16:04:34
# MODIFIED:
###############################################

@PATH _ROOT _DECL _NLPPP

@POST
	rfbdecls(1)
	single()
@RULES
_DECL [base] <- _FUNCDEF [plus] @@
