 ######################################################################
 # FILE: TEXT_TAGGING.pat                                             #
 # SUBJ: Handle any HTML tagging inserted in the text file.           #
 #       This pass assembles the pieces of an HTML tag into a unit    #
 # AUTH: Paul Deane                                                   #
 # CREATED: 02/Jan/01 23:09:42                                        #
 # DATE OF CURRENT VERSION: 31/Aug/01                                 #
 # Copyright 2001, Text Analysis International  #
 ######################################################################

@NODES _ROOT

 ###############################################################
 # CONTENT INDEX                                               #
 # 1. Put together the pieces of an opening tag                #
 #	a. Simple list of parameters a=b                        #
 #	b. Simple list of modifiers like noshade or nowrap      #
 #	c. Combinations of parameters and modifiers             #
 # 2. Put together the pieces of an ending tag                 #
 # 3. Put together the pieces of a comment                     #
 # 4. Put together the pieces of a special <! tag              #
 ###############################################################

 ##############################################
 #                                            #
 # (rule 1a)                                  #
 #                                            #
 # Put together the pieces of an opening tag  #
 ##############################################
@POST
	#extract the tag name and pass it up the tree
	S("tagName") = N("tagName",1) ;
	S("horizBreak") = N("horizBreak",1) ;
	S("paraBreak") = N("paraBreak",1) ;
	S("sectionBreak") = N("sectionBreak",1) ;
	S("heading level") = N("heading level",1) ;
	S("lineBreak") = N("lineBreak",1) ;
	#if the rule match includes a parameter, we're going
	#to want to extract all the parameter info and make
	#it immediately available on the node representing
	#the tag
		#so we get the first element that matches the parameter
		#name node in the rule
		G("currentParameterNode") = N(2) ;
		#then we get the last element that matches the
		#parameter name node in the rule
		G("endParameterList") = lasteltnode(2) ;

		#and advance to one past it, which will tell us
		#we're out of the list when we go into the loop
 		G("endParameterList") = pnnext( G("endParameterList"));
		#now we'll loop through the list of parameters and
		#extract all the information to a pair of arrays.
		#The first array will give the name of the parameter,
		#the corresponding position in the array will give its
		#value
		
		#first step: make sure the global paramNum variable
		#is set to zero before we go into the loop
		G("paramNum") = 0 ;
		#now loop
		if (G("currentParameterNode")!=0) {
			G("parameter") = pnvar(G("currentParameterNode"),"parameterName");
			G("parameterVal") = pnvar( G("currentParameterNode"),"parameterValue") ;
			while (G("currentParameterNode")!=G("endParameterList")) {
				#set the current position on the parameter name
				#list to the current parameter name
				if (G("parameter")) {
					S("parameterNameList")[G("paramNum")] = G("parameter");
					#set the current position on the parameter value
					#list to the current parameter value
					S("parameterValueList")[G("paramNum")] = G("parameterVal");
				       G("paramNum")++;
				}
				G("currentParameterNode") = pnnext(G("currentParameterNode")) ;
				if (G("currentParameterNode")) {
					G("parameter")= pnvar(G("currentParameterNode"),"parameterName");
					G("parameterVal") = pnvar( G("currentParameterNode"),"parameterValue") ;
				}
				else G("parameter")=0;				
			}
		}
	S("paramNum") = G("paramNum");
	single() ;
@@POST
@RULES
_HTMLOpenTag [base] <-
	_startOpenTag [s trig one] ###(1)
	_paramValue [plus] ###(2)
	_endTag [s one] ###(3)
	@@
@@RULES

 ##############################################
 #                                            #
 # (rule 1b)                                  #
 #                                            #
 # Put together the pieces of an opening tag  #
 ##############################################
@POST
	#extract the tag name and pass it up the tree
	S("tagName") = N("tagName",1) ;
	S("tagName") = N("tagName",1) ;
	S("horizBreak") = N("horizBreak",1) ;
	S("paraBreak") = N("paraBreak",1) ;
	S("sectionBreak") = N("sectionBreak",1) ;
	S("heading level") = N("heading level",1) ;
	S("lineBreak") = N("lineBreak",1) ;
	single() ;
@@POST
@RULES
_HTMLOpenTag [base] <-
	_startOpenTag [s trig one] ###(1) 
	_xALPHA [star]  ###(2)
	_endTag [s one] ###(3)
	@@
@@RULES

 ##############################################
 #                                            #
 # (rule 1c)                                  #
 #                                            #
 # Put together the pieces of an opening tag  #
 # with mixed parameters and value settings   #
 # like nowrap                                #
 ##############################################
@POST
	#extract the tag name and pass it up the tree
	S("tagName") = N("tagName",1) ;
	S("tagName") = N("tagName",1) ;
	S("horizBreak") = N("horizBreak",1) ;
	S("paraBreak") = N("paraBreak",1) ;
	S("sectionBreak") = N("sectionBreak",1) ;
	S("heading level") = N("heading level",1) ;
       S("lineBreak") = N("lineBreak",1) ;
	#if the rule match includes a parameter, we're going
	#to want to extract all the parameter info and make
	#it immediately available on the node representing
	#the tag
		#so we get the first element that matches the parameter
		#name node in the rule
		G("currentParameterNode") = N(4) ;
			#then we get the last element that matches the
		#parameter name node in the rule
		G("endParameterList") = lasteltnode(4) ;
		#and advance to one past it, which will tell us
		#we're out of the list when we go into the loop
 		G("endParameterList") = pnnext( G("endParameterList"));
		#now we'll loop through the list of parameters and
		#extract all the information to a pair of arrays.
		#The first array will give the name of the parameter,
		#the corresponding position in the array will give its
		#value
		
		#first step: make sure the global paramNum variable
		#is set to zero before we go into the loop
		G("paramNum") = 0 ;
		#now loop
		if (G("currentParameterNode")!=0) {
			G("parameter") = pnvar(G("currentParameterNode"),"parameterName");
			G("parameterVal") = pnvar( G("currentParameterNode"),"parameterValue") ;
			while (G("parameter")) {
				#set the current position on the parameter name
				#list to the current parameter name
				if (G("parameter")) {
					S("parameterNameList")[G("paramNum")] = G("parameter");
					#set the current position on the parameter value
					#list to the current parameter value
					S("parameterValueList")[G("paramNum")] = G("parameterVal");
					G("paramNum")++;
				}
				G("currentParameterNode") = pnnext(G("currentParameterNode")) ;
				if (G("currentParameterNode")) {
					G("parameter")= pnvar(G("currentParameterNode"),"parameterName");
					G("parameterVal") = pnvar( G("currentParameterNode"),"parameterValue") ;
				}
				else G("parameter")=0;				
			}
		}
	single() ;
@@POST
@RULES
_HTMLOpenTag [base] <- 
	_startOpenTag [s trig one] ###(1) 
	_xWHITE [star] ###(2)
	_xALPHA [opt] ###(3)
	_paramValue [star] ###(4)
	_xWHITE [star] ###(5)
	_xALPHA[opt] ###(6)
	_paramValue [star] ###(7)
	_xWHITE [star] ###(8)
	_xALPHA [opt] ###(9) 
	_xWHITE [star] ###(10) 
	_endTag [s one] ###(11) 
	@@
@@RULES

@POST
	#extract the tag name and pass it up the tree
	S("tagName") = N("tagName",1) ;
	S("horizBreak") = N("horizBreak",1) ;
	S("paraBreak") = N("paraBreak",1) ;
	S("sectionBreak") = N("sectionBreak",1) ;
	S("heading level") = N("heading level",1) ;
	S("lineBreak") = N("lineBreak",1) ;
	single() ;
@@POST
@RULES
_HTMLOpenTag [base] <- 
	_startOpenTag [s trig one] ###(1)
	_endTag [s one] ###(2)
	@@
@@RULES


 ###############################################
 #                                             #
 # (rule 2)                                    #
 #                                             #
 # Put together the pieces of an ending tag    #
 ###############################################
@POST
	#extract the tag name and pass it up the tree
	S("tagName") = N("tagName",1) ;
	S("tagName") = N("tagName",1) ;
	S("horizBreak") = N("horizBreak",1) ;
	S("paraBreak") = N("paraBreak",1) ;
	S("sectionBreak") = N("sectionBreak",1) ;
	S("heading level") = N("heading level",1) ;
	S("lineBreak") = N("lineBreak",1) ;
	single() ;
@@POST
@RULES
_HTMLEndTag [base] <- 
	_startEndTag [s trig one] ###(1)
	_endTag [s one] ###(2)
	@@
@@RULES

 ############################################
 #                                          #
 # (rule 3)                                 #
 #                                          #
 # Put together the pieces of a comment     #
 ############################################
@POST
	single() ;
@@POST
@RULES
_HTMLComment [base] <- 
	_startComment [s trig one] ###(1)
	_xWILD [star] ###(2)
	_endComment [s one] ###(3)
	@@ 
@@RULES


 #################################################
 #                                               #
 # (rule 4)                                      #
 #                                               #
 # Put together the pieces of a special <! tag   #
 #################################################
@POST
	single() ;
@@POST
@RULES
_HTMLSpecialTag [base] <- 
	_startSpecialTag [s trig one] ###(1) 
	_paramValue [star] ###(2)
	_endTag [s one] ###(3)
	@@
_HTMLSpecialTag [base] <- 
	_startSpecialTag [s trig one] 
	_xWILD [star] 
	_endTag [s one] 
	@@ 
@@RULES

 ############################################
 #                                          #
 # (rule 5)                                 #
 #                                          #
 # Put together the pieces of an ASP Script #
 ############################################
@POST
	single() ;
@@POST
@RULES
_ASPScript [base] <-
	_startASPScript [s trig one]
		_xWILD [star fail=("\>")]
		_endTag [s one] 
		@@ 
@@RULES

