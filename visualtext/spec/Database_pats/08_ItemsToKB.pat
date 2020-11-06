###############################################
# FILE: ItemsToKB.pat                         #
# SUBJ: Store the record items into the KB    #
# AUTH: David de Hilster                      #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _RECORD

#===================== First Record Item ======================

@POST
	X("field con") = down(G("fields con"));
	if (!N("value")) N("value") = " ";
	addstrval(X("record con"),conceptname(X("field con")),N("value"));
@RULES
_xNIL <-
    _xSTART [s]	### (1)
    _item		### (2)
    @@
	
#======================= Rest of Items ========================

@POST
	X("field con") = next(X("field con"));
	if (!N("value")) N("value") = " ";
	addstrval(X("record con"),conceptname(X("field con")),N("value"));
@RULES
_xNIL <-
    _item	### (1)
    @@