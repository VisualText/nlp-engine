###############################################
# FILE: checks.pat
# SUBJ: comment here
# AUTH: Amnon Meyers
# CREATED: 18/Dec/99 22:27:12
# MODIFIED:
###############################################

@PATH _ROOT _CHECKS _NLPPP

@POST
	rfaactions(1)
	single()
@RULES
#_CHECKS [base] <- _ACTION [star] @@
_CHECKS [base] <- _STMTS [plus] @@

