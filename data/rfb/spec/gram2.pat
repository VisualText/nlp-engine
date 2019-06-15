###############################################
# FILE: gram2.pat
# SUBJ: NLP++ syntax.
# AUTH: Amnon Meyers
# CREATED: 06/Nov/99 17:25:43
# MODIFIED:
# NOTE:	Using the REC (recursive) algorithm on this file.
#	02/16/00 AM. The entire recursive expression grammar, including
#	argument lists for functions, will be moved into this pass.
#	Even the commas separating args will be handled in here.
###############################################

@NODES _NLPPP

# Catch the start of a function call here, so it won't be grabbed by
# expression grammar.			# 02/16/00 AM.
# Added L local var reference.	# 03/09/02 AM.

@POST
  fncallstart()
  single()
@RULES
_VARLIST [base] <-
	_xWILD [s one match=( s G N X P L ) layer=(_VARNAME)]
	\( @@

# Eg, user::func()	# 02/13/01 AM.
@POST
	scopefncallstart(1,4)
	single()
@RULES
_FNCALLLIST [base] <- _LIT \: \: _LIT [layer=(_FNNAME)] \( @@

@POST
  fncallstart()
  single()
@RULES
_FNCALLLIST [base] <- _LIT [layer=(_FNNAME)] \( @@

@POST
	movesem(2)		# Move expr semantic object up the tree.
	single()
@RULES

# NOTE:	Need a better action to grab the num, str.
_EXPR <- \( _xWILD [s one match=( _EXPR _NUM _FLOAT _STR )] \) @@

# NOTE: Ambiguity with _LIST must be resolved.

@POST
	rfaexpr(1,2,3)
#	single()			# 08/01/00 AM.
	singler(1,3)			# 08/01/00 AM.
@RULES
_EXPR <-
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s t one match=( \* \/ \%
			_opCONF				# 12/17/99 AM.
			)]
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s one fail=(_opINC _opDEC)]		# 08/01/00 AM.
	@@

# Handling precedence.  That's why these rules look funny.
@POST
	rfaexpr(1,2,3)
	singler(1,3)
@RULES
_EXPR <-
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s t one match=( \+ \- )]
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s one match=( _xANY _xEND _xEOF ) except=( \/ \* \%
			_opCONF		# 12/17/99 AM.
			_opINC _opDEC		# 08/01/00 AM.
			)]
	@@

@POST
	rfaexpr(1,2,3)
	singler(1,3)
@RULES
_EXPR <-
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s t one match=( \< \> _opLE _opGE _opEQ _opNEQ )]
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s one match=( _xANY _xEND _xEOF ) except=( \/ \* \% \+ \-
		_opCONF		# 12/17/99 AM.
		_opINC _opDEC		# 08/01/00 AM.
		)]
	@@

@POST
	rfaexpr(1,2,3)
	singler(1,3)
@RULES
_EXPR <-
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s t one match=( _opAND _opOR )]
	_xWILD [s one match=(_EXPR _NUM _FLOAT _STR )]
	_xWILD [s one match=( _xANY _xEND _xEOF )
		except=( \/ \* \% \+ \- \< \> _opLE _opGE _opEQ _opNEQ
			_opCONF	# 12/17/99 AM.
			_opINC _opDEC	# 08/01/00 AM.
	)]
	@@

# 11/11/99 AM. Making assignment into an expr.
# LOWEST PRECEDENCE of any operator except output op (<<).

_EXPR <-
	_VAR [s]
	\= [s]
	_xWILD [s one match=( _EXPR _NUM _FLOAT _STR )]
	_xWILD [s one match=( _xANY _xEND _xEOF )
	  except=( \/ \* \% \+ \- \< \> _opLE _opGE _opEQ _opNEQ
		_opAND _opOR
		_opCONF		# 12/17/99 AM.
		\=		# To associate right to left.	# 12/31/99 AM.
		_opINC _opDEC						# 08/01/00 AM.
	)]
	@@

# Output operator!		# 12/31/99 AM.
# LOWEST PRECEDENCE of any operator.

_EXPR <-
	_xWILD [s one match=(_STR _EXPR)]
	_opOUT [s]
	_xWILD [s one match=( _EXPR _NUM _FLOAT _STR )]
	_xWILD [s one match=( _xANY _xEND _xEOF )
	  except=( \/ \* \% \+ \- \< \> _opLE _opGE _opEQ _opNEQ
		_opAND _opOR
		_opCONF
		\=
		_opINC _opDEC						# 08/01/00 AM.
	)]
	@@


@POST
	rfaunary(1,2)
	singler(1,2)
@RULES
# Unary operators.

# Highest precedence, apart from post operators.
_EXPR <- _xWILD [s one match=( _opINC _opDEC )]
	_VAR [s]
	_xWILD [s one match=( _xANY _xEND _xEOF) except=( _opINC _opDEC)]
	@@

_EXPR <- \! [s]
	_xWILD [s one match=( _EXPR _NUM _FLOAT _STR )]
	_xWILD [s one match=( _xANY _xEND _xEOF) except=( _opINC _opDEC)]
	@@

# Highest precedence operators.
@POST
	rfapostunary(1,2)
	single()
@RULES
_EXPR <-
	_VAR [s]
	_xWILD [s one match=( _opINC _opDEC )]
	@@

# Post unary ops have precedence.
@POST
	rfaunary(2,3)
	singler(2,3)
@RULES
# Only do this if you're at the start of something or there's an
# operator to the left.
_EXPR <-
	_xWILD [s one match=( _xSTART \< \> \+ \- \* \/ \% \! \=
		_opINC _opDEC _opLE _opGE _opEQ _opNE _opAND _opOR
		_opCONF	# 12/17/99 AM.
		_opOUT	# 12/31/99 AM.
		_RETURN \( \[ \{	# 08/15/08 AM.
		)]
	_xWILD [s t one match=( \- \+ )]
	_xWILD [s one match=( _EXPR _NUM _FLOAT )]
	_xWILD [s one match=( _xANY _xEND _xEOF ) except=( _opINC _opDEC)]
	@@

###################################
# GENERALIZED FUNCTION CALL GRAMMAR.	# 02/16/00 AM.
###################################

# LIST GRAMMAR.

# FUNCTION CALL GRAMMAR.
@POST
   addarg(1,0,3)	# 05/26/02 AM.
   listadd(1,3)
@RULES

_VARLIST <- _VARLIST
	\, [opt]
	_xWILD [one match=(_EXPR _NUM _STR)]
	_xWILD [one match=( \, \) )]		# lookahead.
	@@

@POST
   addarg(1,3,4)	# 05/26/02 AM.
   listadd(1,4)
@RULES
_xNIL <-
	_FNCALLLIST
	\, [opt]
	\& [opt]  # Call by reference!		# 05/26/02 AM.
	_xWILD [one match=(_EXPR _NUM _FLOAT _STR)]
	_xWILD [one match=( \, \) )]		# lookahead.
	@@

#PUT ARRAY GRAMMAR IN HERE.				# 10/13/00 AM.
@POST
  varfnarray(1,4)						# 10/13/00 AM.
  single()								# 10/13/00 AM.
@RULES
_VAR [layer=(_EXPR)] <-					# 10/13/00 AM.
	_VARLIST
	\)
	\[
	_xWILD [opt match=(_EXPR _NUM)]		# 10/13/00 AM.
			# Making this OPTIONAL to catch ARRAY ASSIGNMENT.	# 10/20/00 AM.
	\]
	@@

@POST
  varfn()
  singler(1,2)									# 10/15/00 AM.
@RULES
_VAR [layer=(_EXPR)] <- _VARLIST \)
	_xWILD [one fail=( \[ )]	# lookahead.	# 10/15/00 AM.
	@@

# 12/19/01 AM. Keeping function declaration from being an EXPR.
@POST
  movesem(1)
  singler(1,2)
@RULES

_FNCALL <- _FNCALLLIST \) \{ @@

@POST
  movesem(1)
  single()
@RULES

_FNCALL [layer=(_EXPR)] <- _FNCALLLIST \) @@

###############################################
# FILE: preaction.pat
# SUBJ: Keeping pre actions the old way, for now.
# AUTH: Amnon Meyers
# CREATED: 13/Jan/00 10:48:36
# NOTE:	07/01/02 AM. MERGING RFB PASSES.
###############################################

#@NODES _NLPPP

# 02/16/00 AM. Overhauling function call grammar.
@POST
   preaction()
   single()
@RULES
_ACTION [base] <- _PREPAIR _FNCALL [s] \; [s opt] @@

###############################################
# FILE: gram4.pat
# SUBJ: NLP++ syntax.
# AUTH: Amnon Meyers
# CREATED: 06/Nov/99 17:40:14
# MODIFIED:
# NOTE: Some of this could merge with gram5.
#	07/01/02 AM. MERGING RFB PASSES
###############################################

#@NODES _NLPPP

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
_STMT [base] <- \; [s] @@	# Keep from infinite loop.	# 07/01/02 AM.

# SOME SYNTAX ERROR DETECTION.	# 05/19/01 AM.
#@POST
#  rfbunmatchedpunct(1)	# 05/19/01 AM.
#  single();	# To avoid infinite looping.	# 07/01/02 AM.
#@RULES
#_err [base] <- _xWILD [one match=(	# 07/01/02 AM.
#	\( \) \[ \] \< \>
#	)] @@

###############################################
# FILE: gram5.pat
# SUBJ: NLP++ syntax.
# AUTH: Amnon Meyers
# CREATED: 07/Nov/99 11:44:56
# MODIFIED:
# NOTE:	NEED A RECURSIVE ALGORITHM HERE!!
#		Not doing lists of blocks.  Not doing functions.
#	For now, ignoring some degenerate statements like "1".
#	07/01/02 AM. MERGING RFB PASSES.
###############################################

#	Todo: DROP NLP++ regions into their own segments, to
#	be able to say @PATH _ROOT _NLPPP
#@NODES _NLPPP

@POST
#	movesem(1)			# 05/11/00 AM.
	makestmts(1)		# Renamed. # 05/29/01 AM.
	single()
@RULES

# NEED THE BASE, OR INFINITE LOOP!
_STMTS [base] <- _xWILD [s one match=(_STMT
	# _EXPR	# 07/01/02 AM.
	_BLOCK	# 12/15/99 AM.
	)] @@

# For RFA parsing of RFB, need this. # 07/02/02 AM.
@POST
  makestmts(1)
  singler(1,1)
@RULES
_STMTS [base] <-
  _EXPR
  _xWILD [one lookahead match=( _EXPR _STMT \{ \} _startMark _endMark _ELSE _WHILE _xEND )]
  @@

@POST
	addstmt(1, 2)
	single()
@RULES
_STMTS [base] <- _STMTS _xWILD [s one match=(_STMT
	# _EXPR	# 07/01/02 AM.
	_BLOCK	# 12/15/99 AM.
	)] @@

# For RFA parsing of RFB, need this. # 07/02/02 AM.
@POST
  addstmt(1,2)
  singler(1,2)
@RULES
_STMTS [base] <-
  _STMTS
  _EXPR
  _xWILD [one lookahead match=( _EXPR _STMT \{ \} _startMark _endMark _ELSE _WHILE _xEND )]
  @@

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
	_xWILD [s one match=(_BLOCK _STMT
		# _EXPR	# 07/01/02 AM.
		)]
	@@

# 07/02/02 AM.
@POST
  ifstmt(1,2)
  singler(1,2)
@RULES
_IFSTMT <-
	_IFPART
	_EXPR
	_xWILD [one lookahead match=( _EXPR _STMT \{ \} _startMark _endMark _ELSE _WHILE _xEND )]
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
	# _EXPR	# 07/01/02 AM.
	)]
	@@

# 07/02/02 AM.
@POST
	whilestmt(1,2)
	singler(1,2)
@RULES
_STMT <-
	_WHILECOND
	_EXPR
	_xWILD [one lookahead match=( _EXPR _STMT \{ \} _startMark _endMark _ELSE _WHILE _xEND )]
	@@

@POST
#	movesem(2)			# 05/11/00 AM.
	makestmt(2)			# 05/11/00 AM.
	single()
@RULES
# Need better action to handle _NUM, _STR.
_ELSEPART <-
	_ELSE
	_xWILD [s one match=(_BLOCK _STMT
		# _EXPR	# 07/02/02 AM.
		)]
	@@

# 07/02/02 AM.
@POST
	makestmt(2)
	singler(1,2)
@RULES
_ELSEPART <-
	_ELSE
	_EXPR
	_xWILD [one lookahead match=( _EXPR _STMT \{ \} _startMark _endMark _ELSE _WHILE _xEND )]
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




