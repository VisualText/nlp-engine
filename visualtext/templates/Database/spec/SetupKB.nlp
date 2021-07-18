################################################
# FILE: SetupKB.pat                            #
# SUBJ: Set up the kb concepts for the database#
# AUTH: David de Hilster                       #
# Copyright 2001, Text Analysis International  #
################################################

@CODE
G("database con") = findconcept(findroot(),"database");
if (G("database con"))
	rmconcept(G("database con"));
	
G("database con") = makeconcept(findroot(),"database");
G("fields con") = makeconcept(G("database con"),"fields");
G("records con") = makeconcept(G("database con"),"records");

G("field count") = 0;
@@CODE
