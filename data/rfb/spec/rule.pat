###############################################
# FILE:     RULE.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@PATH _ROOT _RULES

@RECURSE rulelt

@POST
	rfanonlitelt(1)
	single()
@RULES
_ELEMENT [base] <- _NONLIT @@

@POST
	rfalitelt(1)
	single()
@RULES
_ELEMENT [base] <- _LIT @@
_ELEMENT [base] <- _NUM @@

@@RECURSE rulelt

@RECURSE sugg

@POST
	rfasugg(1)
	single()
@RULES
_SUGG <- _ELEMENT @@

@@RECURSE sugg

@RECURSE elt

@POST
	rfaelt(1)
	single()
@RULES
_ELT <- _ELEMENT @@

@@RECURSE elt

@RECURSE rulelts

@POST
	rfarulelts(1)
	single()
@RULES
_PHRASE [base] <- _ELT [plus] @@

@POST
  rfberror(1)  # 04/12/03 AM.
@RULES
_xNIL <- _xANY [plus] @@	# 04/12/03 AM.

@@RECURSE rulelts


@POST
	rfarule(1, 3)
	single()
@RULES
_RULE [base] <- 
	_xWILD [one match=(_NONLIT _ELEMENT _LIT) recurse=(rulelt sugg)]
	_ARROW [trig]
	_xWILD [recurse=(rulelt elt rulelts) fail=(_ENDRULE _ARROW)]
	_ENDRULE
	@@

# Can trap missing end rule with a rule like the above whose last elt is
# an _ARROW.  Can similarly trap missing arrow.
# Could do such things if an error has been detected in the parse. Or if
# the above rule partially matched, can set a flag.
# Could have a @COND -- conditions for executing rule at all.
