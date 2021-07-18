###############################################
# FILE: EmptyItems.pat                        #
# SUBJ: Marks empty items                     #
# COMMENTS: [base] blocks further matching    #
# AUTH: David de Hilster                      #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@POST
	singler(1,2);
@RULES
_emptyItem [base] <-
    _enclosedBy											### (1)
    _enclosedBy											### (2)
    _xWILD [match=(_separator _lineTerminator _xEND)]	### (3)
    @@
