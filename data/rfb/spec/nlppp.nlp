###############################################
# FILE: nlppp.pat
# SUBJ: Creating regions for parsing NLP++ syntax and others.
# AUTH: Amnon Meyers
# CREATED: 18/Dec/99 14:27:17
# MODIFIED:
# NOTE:	Trying to overhaul RFB so that code-like regions
#		are parsed differently from the rules.
#		Not using singlex (even though debugged for lookahead)
#		because that disables handling of "empty" regions.
###############################################

@POST
	rfanodes(2, "nodes")
	single()
@RULES
_NODES [base] <- _soNODES [s] _NONLIT [star] _eoNODES [s opt] @@

@POST
	rfanodes(2, "path")
	single()
@RULES
_PATH [base] <- _soPATH [s] _NONLIT [star] _eoPATH [s opt] @@

@POST
	rfanodes(2, "multi")
	single()
@RULES
_MULTI [base] <- _soMULTI [s] _NONLIT [star] _eoMULTI [s opt] @@



# Need a check action to flag an error if _endMark found at last elt.
@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  singler(1,3)
@RULES
_PRES [base unsealed] <-
	_soPRE [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoPRE [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

# Need a check action to flag an error if _endMark found at last elt.
@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  singler(1,3)
@RULES
_CHECKS [base unsealed] <-
	_soCHECK [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoCHECK [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

# Need a check action to flag an error if _endMark found at last elt.
@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  singler(1,3)
@RULES
_POSTS [base unsealed] <-
	_soPOST [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoPOST [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

# Need a check action to flag an error if _endMark found at last elt.
@POST
  singler(1,3)
@RULES
_RULES [base unsealed] <-
	_soRULES [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoRULES [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

# NEW REGION!  FOR RUNNING CODE BEFORE SOMETHING (like @nodes).
# Need a check action to flag an error if _endMark found at last elt.
@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  singler(1,3)
@RULES
_INI [base unsealed] <-
	_soINI [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoINI [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

# NEW REGION!  FOR RUNNING CODE AFTER SOMETHING (like @nodes).
# Need a check action to flag an error if _endMark found at last elt.
@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  singler(1,3)
@RULES
_FIN [base unsealed] <-
	_soFIN [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoFIN [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

# Need a check action to flag an error if _endMark found at last elt.
@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  setbase(2,"true")
  singler(1,3)
@RULES
_CODE [base unsealed] <-
	_soCODE [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoCODE [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@

@POST
  oldgroup(2,2, "_NLPPP")	# An NLP++ region.
  setbase(2,"true")
  singler(1,3)
@RULES
_DECL [base unsealed] <-
	_soDECL [s]
	_xWILD [fail=(_endMark _startMark)]
	_eoDECL [s opt]
	_xWILD [opt lookahead match=(_endMark _startMark)]
	@@
