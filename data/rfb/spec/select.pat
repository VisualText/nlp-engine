###############################################
# FILE:     SELECT.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@POST
	rfaselect(2)
	single()
@RULES
_SELECT [base] <- _soSELECT [opt] _NODES _eoSELECT [opt] @@
_SELECT [base] <- _soSELECT [opt] _MULTI _eoSELECT [opt] @@
_SELECT [base] <- _soSELECT [opt] _PATH _eoSELECT [opt] @@

