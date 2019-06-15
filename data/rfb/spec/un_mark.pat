###############################################
# FILE: un_mark.pat
# SUBJ: comment here
# AUTH: Amnon Meyers
# CREATED: 19/Dec/99 00:20:58
# MODIFIED:
###############################################

# Doing this rigamarole in case region is empty.
@NODES _CODE _CHECKS _PRES _POSTS _NODES _PATH _MULTI _RULES _DECL

@POST
  excise(1,1)
@RULES
_xNIL <- _xWILD [one match=(_startMark _endMark)] @@

