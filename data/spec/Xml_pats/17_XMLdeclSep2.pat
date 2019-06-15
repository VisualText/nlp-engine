###############################################
# FILE: declSep2.pat                          #
# SUBJ:                                       #
# AUTH: Paul Deane                            #
# CREATED: 14/Jan/01 20:00:39                 #
# MODIFIED: 31/Aug/01                         #
# Copyright Text Analysis International       #
###############################################

@NODES _doctypedecl

@RULES

_DeclSep <-
	_PEReference [one]
	@@

_DeclSep <-
	_whiteSpace [one]
	@@

@@RULES
