##################################################
# FILE: RecordItems.pat                          #
# SUBJ: Gather items using tabs and double quotes#
# COMMENT: Put the item value in "value"         #
# AUTH: David de Hilster                         #
# Copyright 2001, Text Analysis International    #
##################################################

@NODES _ROOT

@POST
	noop();
@RULES

#===================== Quoted Empty Items =====================
	
_xNIL <-
    _emptyItem [layer=(_item)]	### (1)
    _xWILD [one lookahead match=(_separator _lineTerminator)]	### (2)
    @@

#================ First Non-Enclosed Empty Items ==============

_xNIL <-
    _lineTerminator [s]			### (1)
    _separator [layer=(_item)]	### (2)
    @@
	
#============== Continuous Non-Enclosed Empty Items ===========

_xNIL <-
    _separator										### (1)
    _xWILD [plus match=(_separator) layer=(_item)]	### (2)
    @@

#====================== Non-Empty Items =======================

@POST
	S("value") = strunescape(N("$text",2),G("enclosed by"),G("escaped by"));
	single();
@RULES
_item <-
    _enclosedBy		### (1)
    _xWILD [plus]	### (2)
    _enclosedBy		### (3)
    @@


#====================== Non-Empty Non-Quoted ================

@POST
	S("value") = N("$text",1);
	singler(1,1);
@RULES
_item <-
    _xWILD [plus fail=("_separator" "_lineTerminator" "_enclosedBy")]	### (1)
    _separator [one]		### (2)
    @@
@@RULES
