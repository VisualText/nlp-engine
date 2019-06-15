###############################################
# FILE: posts.pat
# SUBJ: comment here
# AUTH: Amnon Meyers
# CREATED: 18/Dec/99 22:27:26
# MODIFIED:
###############################################

@PATH _ROOT _POSTS _NLPPP

@POST
	rfaactions(1)
	single()
@RULES
#_POSTS [base] <- _ACTION [star] @@
_POSTS [base] <- _STMTS [plus] @@
