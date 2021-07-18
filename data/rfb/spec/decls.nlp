###############################################
# FILE:     DECLS.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     12/19/01 AM. Renamed from action.pat .
###############################################

@PATH _ROOT _DECL _NLPPP


# 11/08/99 AM. Integrating NLP++ syntax here.
#@POST
#	movesem(1)
#	single()
#@RULES
#_ACTION [base] <-
#	_STMTS [s]
#	@@

# 12/19/01 AM. Trashing this rule, looks useless.
#@POST
#  setbase(1,"true")	# Not sure if this is needed for anything.
#@RULES
#_xNIL <- _STMTS [s] @@
#

@POST
	rfbdecl(1,2)
	single()
@RULES
_FUNCDEF <-
	_FNCALL
	_STMTS		# This needs to be a BLOCK.
	@@

