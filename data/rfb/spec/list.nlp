###############################################
# FILE:     LIST.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@PATH _ROOT _RULES

@RECURSE listarg

@POST
	rfaarg(1)
	single()
@RULES
_ARG [base] <- _NONLIT @@
_ARG [base] <- _LIT @@
_ARG [base] <- _STR @@
_ARG [base] <- _NUM @@

@@RECURSE listarg


@POST
	rfalist(2)
	single()
@RULES
_LIST [base] <- \( _xWILD [match=(_LIT _NONLIT _STR _NUM) recurse=(listarg)] \) @@
