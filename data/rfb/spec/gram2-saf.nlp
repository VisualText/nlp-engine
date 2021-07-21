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
