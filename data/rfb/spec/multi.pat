###############################################
# FILE:     MULTI.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@POST
	rfanodes(2, multi)
	single()
@RULES
_MULTI [base] <- _soMULTI _NONLIT [star] _eoMULTI [opt] @@

