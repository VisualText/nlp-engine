 ###############################################
 # FILE: XML tagStructure.pat                  #
 # SUBJ: Put together segments bounded by      #
 #       XML tags                              #
 # AUTH: Paul Deane                            #
 # CREATED: 14/Jan/01 08:54:33                 #
 # DATE OF THIS VERSION: 31/Aug/01             #
 # Copyright 2001 Text Analysis International  #
 ###############################################

@NODES _ROOT _Element

@CHECK
	#XML tags have to match
	if (!strequal(N("tagName",1),N("tagName",3))) {
		"output.txt" << "XML error: mismatched tags\n";
		fail() ;
	}
@@CHECK
@POST
	#make sure our current tag is registered in our DTD info
	S("tagName") = str(N("tagName",1)) ;
	G("CurrentChildTag") = lasteltnode(2) ;
	G("Continue") = 1 ;
	G("PlaceInKB") = findconcept(G("Elements"),S("tagName")) ;
	while (G("Continue") != 0 ) {
      	       G("CurrentChildTagName") = pnvar(G("CurrentChildTag"),"tagName") ;
		G("nextC") = pnprev(G("CurrentChildTag")) ;
		if (G("nextC")==0) {
			G("Continue") = 0 ;
		}
		else if (G("CurrentChildTagName") == 0 ) {
			G("CurrentChildTag") = G("nextC") ;
		}
		else if ( strequal(S("tagName"),G("CurrentChildTagName"))
			) {
				G("Continue") = 0 ;
		}
		else {
			if (G("CurrentChildTagName") != 0 ) {
				G("ChildPlaceInKB") = findconcept(G("PlaceInKB"),G("CurrentChildTagName"));
				if (G("ChildPlaceInKB")==0) {
					makeconcept(G("PlaceInKB"),G("CurrentChildTagName"));
				}			
			}
			G("CurrentChildTag") = G("nextC") ;
		}
	}
	single() ;
@@POST
@RULES
_Element <-
	_StartingTag [one] ### (1)
	_xWILD [star fails=("_ClosingTag" "_StartingTag")] ### (2)
	_ClosingTag [one] ### (3)
	@@
@@RULES
