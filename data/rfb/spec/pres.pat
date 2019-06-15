###############################################
# FILE: pres.pat
# SUBJ: comment here
# AUTH: Amnon Meyers
# CREATED: 18/Dec/99 22:15:28
# MODIFIED:
###############################################

@PATH _ROOT _PRES _NLPPP

@POST
	rfapres(1)
	single()
@RULES
_PRES [base] <- _ACTION [plus] @@


