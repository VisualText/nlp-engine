###############################################
# FILE:     RETOK.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

# Since RFB rules are hashed, don't need sentinel.
#@POST
#	noop()
#@RULES
#_xNIL <- _xWILD [fail=(\\)] @@


@POST
	rfaname(2)
	single()
@RULES
# For efficiency, some rules are triggered on backslash, while
# others will be triggered on elt following backslash.  The latter
# will always match after the former.
_cLF [base layer=(_LIT )] <- \\ n [ren=\n] @@
_cCR [base layer=(_LIT )] <- \\ r [ren=\r] @@
_cHT [base layer=(_LIT )] <- \\ t [ren=\t] @@

_cLANGLE [base layer=(_LIT )] <- \\ \< @@
_cPOUND [base layer=(_LIT )] <- \\ \# @@
_cDQUOTE [base layer=(_LIT )] <- \\ \" @@
_cATSIGN [base layer=(_LIT )] <- \\ \@ @@
_cLPAR [base layer=(_LIT )] <- \\ \( @@
_cRPAR [base layer=(_LIT )] <- \\ \) @@
_cCOMMA [base layer=(_LIT )] <- \\ \, @@
_cSEMICOLON [base layer=(_LIT )] <- \\ \; @@
_cEQUAL [base layer=(_LIT )] <- \\ \= @@
_cLBRACKET [base layer=(_LIT )] <- \\ \[ @@
_cRBRACKET [base layer=(_LIT )] <- \\ \] @@
_cUNDERSCORE [base layer=(_LIT )] <- \\ \_ @@
_cDASH [base layer=(_LIT )] <- \\ \- @@
_cSPACE [base layer=(_LIT )] <- \\ \  @@
_cRANGLE [base layer=(_LIT )] <- \\ \> @@

_cBEL [base layer=(_LIT )] <- \\ a [ren=\a] @@
_cBS [base layer=(_LIT )] <- \\ b [ren=\b] @@
_cFF [base layer=(_LIT )] <- \\ f [ren=\f] @@
_cVT [base layer=(_LIT )] <- \\ v [ren=\v] @@
_cSQUOTE [base layer=(_LIT )] <- \\ \' @@
_cQMARK [base layer=(_LIT )] <- \\ \? @@
_cBANG [base layer=(_LIT )] <- \\ \! @@
_cDOLLAR [base layer=(_LIT )] <- \\ \$ @@
_cPERCENT [base layer=(_LIT )] <- \\ \% @@
_cAMPERSAND [base layer=(_LIT )] <- \\ \& @@
_cASTERISK [base layer=(_LIT )] <- \\ \* @@
_cPLUS [base layer=(_LIT )] <- \\ \+ @@
_cPERIOD [base layer=(_LIT )] <- \\ \. @@
_cSLASH [base layer=(_LIT )] <- \\ \/ @@
_cCOLON [base layer=(_LIT )] <- \\ \: @@
_cCARET [base layer=(_LIT )] <- \\ \^ @@
_cBACKQUOTE [base layer=(_LIT )] <- \\ \` @@
_cLBRACE [base layer=(_LIT )] <- \\ \{ @@
_cRBRACE [base layer=(_LIT )] <- \\ \} @@
_cVBAR [base layer=(_LIT )] <- \\ \| @@
_cTILDE [base layer=(_LIT )] <- \\ \~ @@

# Problem is that single backslash and double backslash get reduced to the
# same thing -- a single backslash.		# 09/23/00 AM.
# But things like "abc\ndef" need to get retokenized later, to capture \n.
# But if it's "abc\\ndef", that shouldn't get retokenized!
_cBSLASH [base layer=(_LIT )] <- \\ \\ @@

@POST
	excise(1,1)
@RULES
_xNIL <- \r \n @@



