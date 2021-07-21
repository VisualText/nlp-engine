###############################################
# FILE:     RULESFILE.PAT (pass 0)
# SUBJ:     Automatically generated RFA file.
# AUTH:     VisualText
# CREATED:  Fri Nov 05 21:37:45 1999
# MODIFIED: 
# NOTE:     
###############################################

@POST
	rfarulesfile(1, 2, 3, 4, 5)
	single()
@RULES
# ALLOWING EMPTY RULE REGION IF THERE IS CODE or DECL.	12/19/01 AM.
_RULESFILE [base] <- _DECL _CODE [opt] _SELECT [opt] _RECURSES [opt] _REGIONS [opt] @@

_RULESFILE [base] <- _DECL [opt] _CODE _SELECT [opt] _RECURSES [opt] _REGIONS [opt] @@

# Could remove _CODE from here, but would need a new action.	# 08/31/00 AM.
_RULESFILE [base] <- _DECL [opt] _CODE [opt] _SELECT [opt] _RECURSES [opt] _REGIONS @@


@POST
	rfarulesfile(1, 3, 2, 4, 5)
	single()
@RULES

# ALLOWING SELECT ZONE TO PRECEDE CODE ZONE.  # 07/15/01 AM.
_RULESFILE [base] <- _DECL [opt] _SELECT _CODE _RECURSES [opt] _REGIONS [opt] @@
