###############################################
# FILE: Fields.pat                            #
# SUBJ: Gather first line items into fields   #
# AUTH: David de Hilster                      #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@RULES
_FIELDS <-
    _xSTART									### (1)
    _xWILD [plus fails=(_lineTerminator)]	### (2)
    _lineTerminator							### (3)
    @@