###############################################
# FILE: EmptyItems.pat                        #
# SUBJ: Marks empty items                     #
# AUTH: David de Hilster                      #
# COMMENTS: [base] blocks further matching    #
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
