###############################################
# FILE: kb_onto.pat
# SUBJ: Build ontology in KB.
# AUTH: AM
# CREATED: 17/May/05 14:08:10
# MODIFIED:
###############################################

@CODE
G("ontology") = findconcept(findroot(),"ontology");
if (G("ontology"))
  exitpass();	# One-time load per VisualText session.

# While developing, wipe it out each time.
#if (G("ontology"))
#  rmconcept(G("ontology"));

G("ontology") = makeconcept(findroot(),"ontology");

###### ONTOLOGY TOP LEVELS ###########
L("eventive") = makeconcept(G("ontology"),"eventive");

L("geo") = makeconcept(G("ontology"),"geo");
L("body_of_water") = makeconcept(L("geo"),"body_of_water");
L("geo_political") = makeconcept(L("geo"),"geo_political");
###### DETAILS #######################

L("con") = makeconcept(L("body_of_water"),"ocean");
L("con") = makeconcept(L("geo_political"),"city");
loadattr("resort","nounsem",L("con"));

#######EVENTIVE
loadattr("aftermath","nounsem",L("eventive"));

@@CODE
