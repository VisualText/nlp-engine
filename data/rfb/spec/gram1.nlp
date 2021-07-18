###############################################
# FILE: gram1.pat
# SUBJ: NLP++ sentence and expression grammar.
# AUTH: Amnon Meyers
# CREATED: 06/Nov/99 17:05:00
# MODIFIED:
###############################################

@NODES _NLPPP

@RULES
# NLP++ KEYWORDS.				# 01/13/00 AM.
_IF [base] <- if [s] @@
_ELSE [base] <- else [s] @@
_WHILE [base] <- while [s] @@	# 08/31/00 AM.
_RETURN [base] <- return [s] @@	# 03/07/02 AM.

# 02/16/00 AM. Overhauling function call grammar.
# @POST
#	rfavar(1, 2)
#	single()
# @RULES

# _VAR [layer=(_EXPR)] <- _xWILD [s one match=( s G N X P )]
#	  _LIST
#	  @@

# FUNCTION CALLS AND ACTIONS.		# 01/13/00 AM.
# Note: argument list is not yet NLP++ expr.  Still old style.
#	Renaming ACTION to FNCALL as part of moving into NLP++.
# 02/16/00 AM. Overhauling function call grammar.
# @POST
#	rfaaction(0, 1, 2)
#	single()
# @RULES
# _FNCALL [base layer=(_EXPR)] <- _LIT _LIST @@


# Binary ops.
@POST
	movesem(1)
	single()
@RULES
_OP <- _xWILD [s one match=( _opAND _opOR
	_opEQ _opNEQ _opGE _opLE
	_opCONF					# 12/17/99 AM.
	_opOUT					# 12/31/99 AM.
	)]
	@@

