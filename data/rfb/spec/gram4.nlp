###############################################
# FILE: gram4.pat
# SUBJ: NLP++ syntax.
# AUTH: Amnon Meyers
# CREATED: 06/Nov/99 17:40:14
# MODIFIED:
# NOTE: Some of this could merge with gram5.
###############################################

@NODES _NLPPP

@POST
	movesem(2)
	single()
@RULES

_IFPART <- _IF _xWILD [s one match=( _EXPR _NUM _FLOAT _STR )] @@

# Simple statements.
@POST
#	movesem(1)			# 05/11/00 AM.
	makestmt(1)			# 05/11/00 AM.
	single()
@RULES
# NOTE: num and str aren't handled as statements, for now.
_STMT <- _xWILD [s one match=( _EXPR _NUM _FLOAT _STR )] \; [s] @@

# FUNCTION RETURN STATEMENT.	# 03/07/02 AM.
@POST
	returnstmt(1,2)				# 03/12/02 AM.
	single()
@RULES
_STMT <-						# 03/07/02 AM.
	_RETURN
	_xWILD [s opt match=(_EXPR _NUM _FLOAT _STR)]
	\; [s]
	@@

# EMPTY STATEMENT.	# 08/01/00 AM.
@RULES
_STMT <- \; [s] @@

# SOME SYNTAX ERROR DETECTION.	# 05/19/01 AM.
@POST
  rfbunmatchedpunct(1)	# 05/19/01 AM.
@RULES
_xNIL <- _xWILD [one match=(
	\( \) \[ \] \< \>
	)] @@


