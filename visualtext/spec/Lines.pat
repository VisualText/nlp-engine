###################################################
# FILE: lines.pat
# SUBJ: Gather Lines
# CREATED: 10/Mar/99 13:35:22
# Date of this version: 31/Aug/01
# Copyright 1999-2004, Text Analysis International
###################################################

@NODES _ROOT

@RULES

_BLANKLINE <-
    _xWILD [min=0 max=0 matches=(\  \t \r)] ### (1)
    \n ### (2)
    @@

_LINE <-
    _xWILD [min=0 max=0 fails=(\r \n)] ### (1)
    _xWILD [one match=(\n _xEND)] ### (2)
    @@
