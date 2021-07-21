###############################################
# FILE:     COMPONENTS.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@RULES
_ENDRULE [base] <- \@ \@ _xWHITE @@

#@POST
#	noop()
#@RULES
#_xNIL <- _xWILD [min=1 max=1 fail=(\@)] @@

@RULES
_soRULES [base] <- \@ RULES [t] @@
_soPOST [base] <- \@ POST [t] @@
_soCHECK [base] <- \@ CHECK [t] @@
_soPRE [base] <- \@ PRE [t] @@
_soNODES [base] <- \@ NODES [t] @@
_soMULTI [base] <- \@ MULTI [t] @@
_soPATH [base] <- \@ PATH [t] @@
_soCODE [base] <- \@ CODE [t] @@
_soSELECT [base] <- \@ SELECT [t] @@
_soRECURSE [base] <- \@ RECURSE [t] @@
_eoPOST [base] <- \@ \@ POST [t] @@
_eoCHECK [base] <- \@ \@ CHECK [t] @@
_eoPRE [base] <- \@ \@ PRE [t] @@
_eoRULES [base] <- \@ \@ RULES [t] @@
_eoRECURSE [base] <- \@ \@ RECURSE [t] @@
_eoSELECT [base] <- \@ \@ SELECT [t] @@
_eoNODES [base] <- \@ \@ NODES [t] @@
_eoMULTI [base] <- \@ \@ MULTI [t] @@
_eoPATH [base] <- \@ \@ PATH [t] @@
_eoCODE [base] <- \@ \@ CODE [t] @@
_ENDRULE [base] <- \@ \@ @@

@POST
	rfanonlit(2)
	single()
@RULES
_NONLIT [base] <- \_ _xALPHA @@

@RULES
_ARROW [base] <- \< \- @@

@POST
	rfaname(1)
	single()
@RULES
_LIT [base] <- _xALPHA @@

@POST
	rfanum(1)
	single()
@RULES
_NUM [base] <- _xNUM @@
