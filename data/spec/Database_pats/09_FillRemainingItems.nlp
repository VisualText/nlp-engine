################################################
# FILE: FillRemainingItems.pat                 #
# SUBJ: Fill in the remaining ghost field names#
# AUTH: David de Hilster                       #
# Copyright 2001, Text Analysis International  #
################################################

@NODES _ROOT

@POST
	N("field con") = next(N("field con"));
	while (N("field con"))
		{
		addstrval(N("record con"),conceptname(N("field con"))," ");
		N("field con") = next(N("field con"));
		}
@RULES
_xNIL <-
	_RECORD
	@@