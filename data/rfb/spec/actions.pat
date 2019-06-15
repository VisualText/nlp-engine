###############################################
# FILE:     ACTIONS.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@NODES _NLPPP

@RECURSE littoaction

@POST
	rfalittoaction(1)
	single()
@RULES
_ACTION <- _LIT @@

@@RECURSE littoaction

@RECURSE littoaction

@POST
	rfalittoaction(1)
	single()
@RULES
_ACTION <- _LIT @@

@@RECURSE littoaction

@POST
	rfaactions(2)
	single()
@RULES
_POSTS [base] <-
	_soPOST
	_xWILD [match=(_LIT _ACTION) recurse=(littoaction)]
	_eoPOST [opt] @@
_CHECKS [base] <-
	_soCHECK
	_xWILD [match=(_LIT _ACTION) recurse=(littoaction)]
	_eoCHECK [opt] @@

