 ###############################################
 # FILE: XML OverallSchema.pat                 #
 # SUBJ: Put together the last pieces of an XML#
 #       document                              #
 # AUTH: Paul Deane                            #
 # CREATED: 14/Jan/01 13:35:54                 #
 # DATE OF THIS VERSION: 31/Aug/01             #             
 # Copyright 2001 Text Analysis International  #
 ###############################################

 ###############################
 # CONTENTS                    #
 # In code section: sorting    #
 # algorithm that organizes    #
 # the data from DTD info      #
 #                             #
 # In main rule: overall       #
 # structure of an XML file    #
 ###############################

@CODE
if (G("Organize DTD")==1) {
 #At this point the DTD knowledge base is complete.
 #So the next task is to sort the element list under
 #the gram tab so that the information is ordered in
 #the order we want to make generation of passes as
 #natural as possible.
G("CurrentConcept") = down(G("Elements")) ;
G("CurrentConceptName") = conceptname(G("CurrentConcept")) ;
G("Continue") = 1 ;
G("CurrentChildConcept") = down(G("CurrentConcept")) ;
G("CurrentChildConceptName") = conceptname(G("CurrentChildConcept")) ;
G("ReferenceIDforConcept") = findconcept(G("Elements"),G("CurrentChildConceptName")) ;
G("NextConcept") = next(G("ReferenceIDforConcept")) ;

#This is a sorting algorithm that puts the elements of
#the DTD data we've been creating into alphabetic order
G("attrName") = "sortedYet" ;
while ( G("Continue") != 0  ) {
	if (G("CurrentConcept") == 0 ) {
		G("Continue") = 0 ;
	}

	else if ( G("CurrentChildConcept") == 0 &&
            numval(G("CurrentConcept"),G("attrName")) == 0
	 ) {
		replaceval(G("CurrentConcept"),G("attrName"),1) ;
		G("CurrentConcept") = down(G("Elements")) ;
		G("CurrentConceptName") = conceptname(G("CurrentConcept")) ;
		G("CurrentChildConcept") = down(G("CurrentConcept")) ;
		if (G("CurrentChildConcept")!=0) {
			G("CurrentChildConceptName") = conceptname(G("CurrentChildConcept")) ;
			G("ReferenceIDforConcept") = findconcept(G("Elements"),G("CurrentChildConceptName")) ;
			G("NextConcept") = next(G("ReferenceIDforConcept")) ;
		}
		else {
			G("CurrentChildConceptName") = " " ;
			G("ReferenceIDforConcept") = 0 ;
			G("NextConcept") = 0 ;
		}
	}
	else if (G("CurrentChildConcept") == 0 ) {
		G("CurrentConcept") = next(G("CurrentConcept")) ;
		G("CurrentConceptName") = conceptname(G("CurrentConcept")) ;
		G("CurrentChildConcept") = down(G("CurrentConcept")) ;
		if (G("CurrentChildConcept")!=0) {
			G("CurrentChildConceptName") = conceptname(G("CurrentChildConcept")) ;
			G("ReferenceIDforConcept") = findconcept(G("Elements"),G("CurrentChildConceptName")) ;
			G("NextConcept") = next(G("ReferenceIDforConcept")) ;
		}
		else {
			G("CurrentChildConceptName") = " " ;
			G("ReferenceIDforConcept") = 0 ;
			G("NextConcept") = 0 ;
		}
	}

	
	else if ( 
	     G("CurrentChildConcept") != 0 &&
	     G("ReferenceIDforConcept") != 0 &&
	     G("CurrentChildConceptName") != " " &&
            numval(G("ReferenceIDforConcept"),G("attrName")) == 0 &&
	     !strequal(G("CurrentConceptName"),G("CurrentChildConceptName"))
          ) {
		movecleft(G("ReferenceIDforConcept")) ;
		G("NextConcept") = next(G("ReferenceIDforConcept")) ;
		if ( G("NextConcept") == G("CurrentConcept") ) {
			G("CurrentChildConcept") = next(G("CurrentChildConcept")) ;
			if (G("CurrentChildConcept") != 0 ) {
				G("CurrentChildConceptName") = conceptname(G("CurrentChildConcept")) ;
				G("ReferenceIDforConcept") = findconcept(G("Elements"),G("CurrentChildConceptName")) ;
				G("NextConcept") = next(G("ReferenceIDforConcept")) ;
			}
			else { replaceval(G("CurrentConcept"),G("attrName"),1) ;
				G("CurrentConcept") = down(G("Elements")) ;
				G("CurrentConceptName") = conceptname(G("CurrentConcept")) ;
				G("CurrentChildConcept") = down(G("CurrentConcept")) ;
				if (G("CurrentChildConcept")!=0) {
					G("CurrentChildConceptName") = conceptname(G("CurrentChildConcept")) ;
					G("ReferenceIDforConcept") = findconcept(G("Elements"),G("CurrentChildConceptName")) ;
					G("NextConcept") = next(G("ReferenceIDforConcept")) ;
				}
				else {
					G("CurrentChildConceptName") = " " ;
					G("ReferenceIDforConcept") = 0 ;
					G("NextConcept") = 0 ;
				}
			}
		}
	}
	else if ( G("CurrentConcept") != 0 &&
                 G("CurrentChildConcept") != 0
       ) {
		G("CurrentChildConcept") = next(G("CurrentChildConcept")) ;
		G("CurrentConceptName") = conceptname(G("CurrentConcept")) ;
		if (G("CurrentChildConcept")!=0) {
			G("CurrentChildConceptName") = conceptname(G("CurrentChildConcept")) ;
			G("ReferenceIDforConcept") = findconcept(G("Elements"),G("CurrentChildConceptName")) ;
			G("NextConcept") = next(G("ReferenceIDforConcept")) ;
		}
		else {
			G("CurrentChildConceptName") = " " ;
			G("ReferenceIDforConcept") = 0 ;
			G("NextConcept") = 0 ;
		}
	}
	else {
		G("Continue") = 0 ;
	}
}

G("Attr") = "sortedYet" ;
G("CurrentConcept") = down(G("Entities")) ;
while ( G("CurrentConcept") != 0 ) {
	"hierDtd.log" << conceptpath(G("CurrentConcept")) << "\n" ;
	G("nextC") = next(G("CurrentConcept")) ;
	G("CurrentConcept") = G("nextC") ;
}
G("CurrentConcept") = down(G("Elements")) ;
while ( G("CurrentConcept") != 0 ) {
	rmattr(G("CurrentConcept"),"sortedYet") ;
	"hierDtd.log" << conceptpath(G("CurrentConcept")) << "\n" ;
	G("CurrentChildConcept") = down(G("CurrentConcept")) ;
	while (G("CurrentChildConcept") != 0 ) {
		rmattr(G("CurrentChildConcept"),"sortedYet") ;
		"hierDtd.log" << conceptpath(G("CurrentChildConcept")) << "\n" ;
		G("nextC") = next(G("CurrentChildConcept")) ;
		G("CurrentChildConcept") = G("nextC") ;
	}
	G("nextC") = next(G("CurrentConcept")) ;
	G("CurrentConcept") = G("nextC") ;
  }
}
@@CODE
@NODES _ROOT

@RULES

_XMLDocument [unsealed] <-
	_Prolog [one] ### (1)
	_Element [one] ### (2)
	_Misc [star] ### (3)
@@
