###############################################
# FILE: gram5.pat
# SUBJ: NLP++ syntax.
# AUTH: Amnon Meyers
# CREATED: 07/Nov/99 11:44:56
# MODIFIED:
# NOTE:	NEED A RECURSIVE ALGORITHM HERE!!
#		Not doing lists of blocks.  Not doing functions.
#	For now, ignoring some degenerate statements like "1".
###############################################

#	Todo: DROP NLP++ regions into their own segments, to
#	be able to say @PATH _ROOT _NLPPP
@NODES _NLPPP

@POST
#	movesem(1)			# 05/11/00 AM.
	makestmts(1)		# Renamed. # 05/29/01 AM.
	single()
@RULES

# NEED THE BASE, OR INFINITE LOOP!
_STMTS [base] <- _xWILD [s one match=(_STMT _EXPR
	_BLOCK	# 12/15/99 AM.
	)] @@

@POST
	addstmt(1, 2)
	single()
@RULES
_STMTS [base] <- _STMTS _xWILD [s one match=(_STMT _EXPR
	_BLOCK	# 12/15/99 AM.
	)] @@

@POST
	movesem(2)
	single()
@RULES
_BLOCK <- \{ [s] _STMTS \} [s] @@

# EMPTY BLOCK.	# 08/01/00 AM.
@RULES
_BLOCK <- \{ [s] \} [s] @@		# 08/01/00 AM.

@POST
	ifstmt(1, 2)
	single()
@RULES
_IFSTMT <-
	_IFPART
	_xWILD [s one match=(_BLOCK _STMT _EXPR)]
	@@
	
# WHILE STATEMENT	# 08/31/00 AM.
@POST
	movesem(2)
	single()
@RULES
_WHILECOND <- _WHILE _EXPR @@

# Should make sure expr is parenthesized.
@POST
	whilestmt(1, 2)
	single()
@RULES
_STMT <- _WHILECOND	_xWILD [s one match=(_BLOCK _STMT
	_EXPR	# 05/29/01 AM.
	)]
	@@


@POST
#	movesem(2)			# 05/11/00 AM.
	makestmt(2)			# 05/11/00 AM.
	single()
@RULES
# Need better action to handle _NUM, _STR.
_ELSEPART <-
	_ELSE
	_xWILD [s one match=(_BLOCK _STMT _EXPR)]
	@@

@POST
	ifelse(1, 2)
	single()
@RULES
_STMT <-
	_IFSTMT _ELSEPART
	@@

# Probably need to check for no "else" as next elt.
@POST
	movesem(1)
	singler(1, 1)
@RULES
_STMT [base] <- _IFSTMT
	_xWILD [s one match=( _xANY _xEND _xEOF ) except=( _ELSE )]
	 @@

