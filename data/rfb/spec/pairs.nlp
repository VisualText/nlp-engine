###############################################
# FILE:     PAIRS.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@PATH _ROOT _RULES

@RECURSE littopair

@POST
	rfalittopair(1)
	single()
@RULES
_PAIR <- _LIT @@

@@RECURSE littopair


@POST
	rfapairs(2)
	single()
@RULES
_PAIRS [base] <- \[ _xWILD [match=(_LIT _PAIR \*) recurse=(littopair)] \] @@

