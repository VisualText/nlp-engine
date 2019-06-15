###############################################
# FILE: tmp1.pat
# SUBJ: Splice the NLPPP container.
# AUTH: Amnon Meyers
# CREATED: 18/Dec/99 23:26:07
# MODIFIED:
###############################################

@NODES _ROOT

@POST
  oldsplice(1,1)	# 07/19/02 AM.
@RULES

_xNIL <- _NLPPP @@

