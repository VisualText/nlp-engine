###############################################
# FILE: Records.pat                           #
# SUBJ: Gather items into records             #
# AUTH: David de Hilster                      #
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@POST
	G("record count")++;
	S("record name") = "record" + str(G("record count"));
	S("record con") = makeconcept(G("records con"),S("record name"));
	single();

@RULES

_RECORD <-
    _xWILD [plus fails=(_lineTerminator) except=(_FIELDS)]	### (1)
    _xWILD [one match=(_lineTerminator _xEND)]				### (2)
    @@
