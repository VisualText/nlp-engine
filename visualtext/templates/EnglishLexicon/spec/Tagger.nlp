###############################################
# FILE: Tagger.pat                            #
# SUBJ: comment here                          #
# AUTH: David de Hilster                      #
# CREATED: 02/May/01 12:18:15                 #
# MODIFIED:                                   #    
# Copyright 2001, Text Analysis International #
###############################################

@NODES _ROOT

@POST
    N("wordcon") = dictfindword(strtolower(N("$text")));
	if (N("wordcon"))
		{
		G("vals") = findvals(N("wordcon"),"pos");
		G("first val") = "_" + strtoupper(getstrval(G("vals")));
		N("pos num") = 0;
		while (G("vals"))
			{
			G("val") = getstrval(G("vals"));
			if (G("val") == "noun")
				N("noun") = 1;
			else if (G("val") == "verb")
				N("verb") = 1;
			else if (G("val") == "definite article")
				N("definite article") = 1;
			else if (G("val") == "indefinite article")
				N("indefinite article") = 1;
			else if (G("val") == "adjective")
				N("adjective") = 1;
			else if (G("val") == "adverb")
				N("adverb") = 1;
			else if (G("val") == "conj")
				N("conj") = 1;
			else if (G("val") == "pronoun")
				N("pronoun") = 1;
			else if (G("val") == "prep" || G("val") == "preposition")
				N("prep") = 1;
			else if (G("val") == "interj" || G("val") == "interjection")
				N("interj") = 1;
			else if (G("val") == "aux")
				N("aux") = 1;
			else if (G("val") == "pastpart")
				N("pastpart") = 1;
			G("vals") = nextval(G("vals"));
			N("pos num")++;
			}
		if (N("pos num") > 1)
			group(1,1,"_AMBIG");
		else
			group(1,1,strsubst(G("first val")," ","_"));
		}

@RULES

_xNIL <-
    _xALPHA [s]	### (1)
    @@

