###############################################
# FILE: TrimKB.pat                            #
# SUBJ: comment here                          #
# AUTH: David de Hilster                      #
# CREATED: 07/Jul/01 11:34:59                 #
# MODIFIED:                                   #
# Copyright 2001, Text Analysis International #
###############################################

@CODE
G("dict") = findhierconcept("dict",findroot());
G("alphas") = findhierconcept("a",G("dict"));
G("letter") = down(G("alphas"));

G("nouns") = 0;
G("adjectives") = 0;
G("adverbs") = 0;
G("unmarked") = 0;
G("knowns") = 0;

while (G("letter"))
	{
	G("subletter") = down(G("letter"));
	while (G("subletter"))
		{
		G("word") = down(G("subletter"));
		while (G("word"))
			{
			G("word name") = conceptname(G("word"));
			G("attrs") = findattrs(G("word"));
			G("classified") = 0;
			
			if (G("attrs"))
				{				
				if (attrname(G("attrs")) == "pos")
					{
					G("vals") = attrvals(G("attrs"));
					while (G("vals"))
						{
						addnumval(G("word"),getstrval(G("vals")),1);
						G("vals") = nextval(G("vals"));
						}
					rmattr(G("word"),"pos");
					}

#				while (G("attrs"))
#					{
#					if (attrname(G("attrs")) != "pos")
#						addstrval(G("word"),"pos",attrname(G("attrs")));
#					G("attrs") = nextattr(G("attrs"));
#					}
#					
#				G("attrs") = findattrs(G("word"));
#				while (G("attrs"))
#					{
#					if (attrname(G("attrs")) != "pos")
#						rmattr(G("word"),attrname(G("attrs")));
#					G("attrs") = nextattr(G("attrs"));
#					}
#
#				if (strendswith(G("word name"),"i") ||
#					strendswith(G("word name"),"encies") ||
#					strendswith(G("word name"),"ibles") ||
#					strendswith(G("word name"),"ller"))
#					{
#					addnumval(G("word"),"nouns",1);
#					"nouns.txt" << G("word name") << "\n";
#					G("nouns")++;
#					G("classified") = 1;
#					}
#				else if (strendswith(G("word name"),"fuller") ||
#						 strendswith(G("word name"),"fullest"))
#					{
#					addnumval(G("word"),"adjective",1);
#					"adjectives.txt" << G("word name") << "\n";
#					G("adjectives")++;
#					G("classified") = 1;
#					}
#				else if (strpiece(G("word name"),0,4) == "super" ||
#						 strpiece(G("word name"),0,4) == "inter" ||
#						 strpiece(G("word name"),0,4) == "under" ||
#						 strpiece(G("word name"),0,4) == "hyper" ||
#						 strpiece(G("word name"),0,4) == "multi" ||
#						 strpiece(G("word name"),0,4) == "ultra")
#					{
#					G("root concept") = dictfindword(strpiece(G("word name"),5,strlength(G("word name"))-1));
#					if (G("root concept"))
#						{
#						G("attrs") = findattrs(G("root concept"));
#						while (G("attrs"))
#							{
#							"verb.txt" << G("word name") << "\n";
#							G("verbs")++;
#							G("classified") = 1;
#							addnumval(G("word"),attrname(G("attrs")),1);
#							G("attrs") = nextattr(G("attrs"));
#							}
#						}
#					}
#				else if (strpiece(G("word name"),0,3) == "over" ||
#						 strpiece(G("word name"),0,3) == "post")
#					{
#					G("root concept") = dictfindword(strpiece(G("word name"),4,strlength(G("word name"))-1));
#					if (G("root concept"))
#						{
#						G("attrs") = findattrs(G("root concept"));
#						while (G("attrs"))
#							{
#							"verb.txt" << G("word name") << "\n";
#							G("verbs")++;
#							G("classified") = 1;
#							addnumval(G("word"),attrname(G("attrs")),1);
#							G("attrs") = nextattr(G("attrs"));
#							}
#						}
#					}
#				else if (strpiece(G("word name"),0,2) == "sub" ||
#						 strpiece(G("word name"),0,2) == "non" ||
#						 strpiece(G("word name"),0,2) == "mis" ||
#						 strpiece(G("word name"),0,2) == "pre" ||
#						 strpiece(G("word name"),0,2) == "out" ||
#						 strpiece(G("word name"),0,2) == "dis")
#					{
#					G("root concept") = dictfindword(strpiece(G("word name"),3,strlength(G("word name"))-1));
#					if (G("root concept"))
#						{
#						G("attrs") = findattrs(G("root concept"));
#						while (G("attrs"))
#							{
#							"verb.txt" << G("word name") << "\n";
#							G("verbs")++;
#							G("classified") = 1;
#							addnumval(G("word"),attrname(G("attrs")),1);
#							G("attrs") = nextattr(G("attrs"));
#							}
#						}
#					}
#				else if (strpiece(G("word name"),0,1) == "re" ||
#						 strpiece(G("word name"),0,1) == "up" ||
#						 strpiece(G("word name"),0,1) == "un")
#					{
#					G("root concept") = dictfindword(strpiece(G("word name"),2,strlength(G("word name"))-1));
#					if (G("root concept"))
#						{
#						G("attrs") = findattrs(G("root concept"));
#						while (G("attrs"))
#							{
#							"verb.txt" << G("word name") << "\n";
#							G("verbs")++;
#							G("classified") = 1;
#							addnumval(G("word"),attrname(G("attrs")),1);
#							G("attrs") = nextattr(G("attrs"));
#							}
#						}
#					}
#					addnumval(G("word"),"verb",1);
#					"verb.txt" << G("word name") << "\n";
#					G("verbs")++;
#					G("classified") = 1;
#					}
#				if (strendswith(G("word name"),"mmed") ||
#					strendswith(G("word name"),"pped"))
#					{
#					addnumval(G("word"),"verb",1);
#					"verb.txt" << G("word name") << "\n";
#					G("verbs")++;
#					G("classified") = 1;
#					}
#				else if (strendswith(G("word name"),"ings") ||						
#						 strendswith(G("word name"),"ers"))
#					{
#					addnumval(G("word"),"noun",1);
#					"noun.txt" << G("word name") << "\n";
#					G("nouns")++;
#					G("classified") = 1;
#					}
#			
#			if (findattrs(G("word")))
#				{
#				while (G("attrs"))
#					{
#					if (attrname(G("attrs")) != "pos")
#						{
#						rmattr(G("word"),attrname(G("attrs")));
#						"nonpos.txt" << G("word name") << "\n";
#						}
#					G("attrs") = nextattr(G("attrs"));
#					}
#				}
#			else if (G("attrs"))
#			if (G("attrs"))
#				{
#				G("classified") = 0;
#				
#				if (attrname(G("attrs")) == "pos")
#					{
#					G("vals") = attrvals(G("attrs"));
#					while (G("vals"))
#						{
#						addnumval(G("word"),getstrval(G("vals")),1);
#						G("vals") = nextval(G("vals"));
#						}
#					rmattr(G("word"),"pos");
#					}
#
#				if (strpiece(G("word name"),0,3) == "anti")
#					{
#					if (!attrwithval(G("word"),"pos","noun"))
#						addstrval(G("word"),"pos","noun");
#					"anti.txt" << G("word name") << "\n";
#					G("nouns")++;
#					G("classified") = 1;
#					}
#					
#				else if (strendswith(G("word name"),"s"))
#					{
#					G("word root") = strpiece(G("word name"),0,strlength(G("word name"))-2);
#					G("word singular") = dictfindword(G("word root"));
#					if (G("word singular"))
#						{
#						if (!findattrs(G("word singular")))
#							{
#							if (!attrwithval(G("word"),"pos","noun"))
#								addstrval(G("word"),"pos","noun");
#							if (!attrwithval(G("word singular"),"pos","noun"))
#								addstrval(G("word singular"),"pos","noun");
#							"noun2.txt" << conceptname(G("word singular")) << " - " << G("word name") << "\n";
#							G("nouns") = G("nouns") + 2;
#							G("classified") = 1;
#							}
#						}
#					}
#				
#
#				if (strendswith(G("word name"),"ly"))
#					{
#					if (!attrwithval(G("word"),"pos","adverb"))
#						addstrval(G("word"),"pos","adverb");
#					"adv.txt" << G("word name") << "\n";
#					G("adverbs")++;
#					G("classified") = 1;
#					}
#				else if (strendswith(G("word name"),"tional") ||
#						 strendswith(G("word name"),"like") ||
#						 strendswith(G("word name"),"less") ||
#						 strendswith(G("word name"),"ier") ||
#						 strendswith(G("word name"),"some") ||
#						 strendswith(G("word name"),"iest"))
#					{
#					if (!attrwithval(G("word"),"pos","adjective"))
#						addstrval(G("word"),"pos","adjective");
#					G("adjectives")++;
#					"adj.txt" << G("word name") << "\n";
#					G("classified") = 1;
#					}
#				else if (strendswith(G("word name"),"ities") ||
#						 strendswith(G("word name"),"men") ||
#						 strendswith(G("word name"),"man"))
#					{
#					if (!attrwithval(G("word"),"pos","noun"))
#						addstrval(G("word"),"pos","noun");
#					G("nouns")++;
#					"nouns.txt" << G("word name") << "\n";
#					G("classified") = 1;
#					}
#
#				if (strendswith(G("word name"),"s"))
#					{
#					G("word root") = strpiece(G("word name"),0,strlength(G("word name"))-2);
#					if (strendswith(G("word name"),"ies"))
#						{
#						G("word stripped") = strpiece(G("word name"),0,strlength(G("word name"))-4);
#						G("word root") = G("word stripped") + "y";
#						}
#					else if (strendswith(G("word name"),"es"))
#						{
#						G("word stripped") = strpiece(G("word name"),0,strlength(G("word name"))-3);
#						if (strendswith(G("word stripped"),"s"))
#							G("word root") = G("word stripped");
#						}
#					else
#						G("word stripped") = strpiece(G("word name"),0,strlength(G("word name"))-2);
#					G("word ing") = G("word stripped") + "ing";
#					G("word ed") = G("word stripped") + "ed";
#					G("word concept") = dictfindword(G("word root"));
#					G("word ing concept") = dictfindword(G("word ing"));
#					G("word ed concept") = dictfindword(G("word ed"));
#					
#					if (G("word concept") && G("word ing concept") && G("word ed concept"))
#						{
#						if (!findattrs(G("word concept")) && !findattrs(G("word ing concept")) &&
#							!findattrs(G("word ed concept")))
#							{
#							if (!attrwithval(G("word"),"pos","verb"))
#								addstrval(G("word"),"pos","verb");
#							if (!attrwithval(G("word concept"),"pos","verb"))
#								addstrval(G("word concept"),"pos","verb");
#							if (!attrwithval(G("word ing concept"),"pos","verb"))
#								addstrval(G("word ing concept"),"pos","verb");
#							if (!attrwithval(G("word ed concept"),"pos","verb"))
#								addstrval(G("word ed concept"),"pos","verb");
#							"verbs.txt" << G("word root") << " - " << G("word name") << " - " << G("word ing") << " - " << G("word ed") << "\n";
#							G("verbs") = G("verbs") + 4;
#							G("classified") = 1;
#							}
#						}
#					else if (G("word concept"))
#						{
#						if (!findattrs(G("word concept")))
#							{
#							if (!attrwithval(G("word"),"pos","noun"))
#								addstrval(G("word"),"pos","noun");
#							if (!attrwithval(G("word concept"),"pos","noun"))
#								addstrval(G("word concept"),"pos","noun");
#							"nouns.txt" << G("word root") << " - " << G("word name") << "\n";
#							G("nouns") = G("nouns") + 2;
#							G("classified") = 1;
#							}
#						}
#					}
#				else if (strendswith(G("word name"),"ed"))
#					{
#					G("word root") = strpiece(G("word name"),0,strlength(G("word name"))-2);
#					G("word stripped") = strpiece(G("word name"),0,strlength(G("word name"))-3);
#					G("word ing") = G("word stripped") + "ing";
#					G("word concept") = dictfindword(G("word root"));
#					G("word ing concept") = dictfindword(G("word ing"));
#					
#					if (G("word concept") && G("word ing concept"))
#						{
#						if (!findattrs(G("word concept")) && !findattrs(G("word ing concept")))
#							{
#							if (!attrwithval(G("word"),"pos","verb"))
#								addstrval(G("word"),"pos","verb");
#							if (!attrwithval(G("word concept"),"pos","verb"))
#								addstrval(G("word concept"),"pos","verb");
#							if (!attrwithval(G("word ing concept"),"pos","verb"))
#								addstrval(G("word ing concept"),"pos","verb");
#							G("word es") = G("word root") + "s";
#							G("word es concept") = dictfindword(G("word es"));
#							if (!G("word es concept"))
#								{
#								G("word es concept") = addword(G("word es"));
#								addstrval(G("word es concept"),"pos","verb");
#								}
#								
#							"verbs.txt" << G("word root") << " - " << G("word name") << " - " << G("word ing") << "\n";
#							G("verbs") = G("verbs") + 3;
#							}
#						}
#					}

				if (G("classified") == 0)
					"rest.txt" << G("word name") << "\n";
				G("unmarked")++;
				"unmarked.txt" << G("word name") << "\n";
				}

			G("word") = next(G("word"));
			}
		G("subletter") = next(G("subletter"));
		}
	G("letter") = next(G("letter"));
	}
	
"output.txt" << "    Knowns: " << G("knowns") << "\n";
"output.txt" << "  Unmarked: " << G("unmarked") << "\n";
"output.txt" << "     nouns: " << G("nouns") << "\n";
"output.txt" << "adjectives: " << G("adjectives") << "\n";
"output.txt" << "     verbs: " << G("verbs") << "\n";
"output.txt" << "   adverbs: " << G("adverbs") << "\n";
"output.txt" << "-------------------\n";
"output.txt" << "    diff: " << str(G("unmarked") - G("nouns") - G("adverbs") - G("verbs") - G("adjectives")) << "\n";


@@CODE

