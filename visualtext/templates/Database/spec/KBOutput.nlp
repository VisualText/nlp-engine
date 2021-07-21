###############################################
# FILE: KBOutput.pat                          #
# SUBJ: Dump the database from the KB         #
# AUTH: David de Hilster                      #
# Copyright 2001, Text Analysis International #
###############################################

@CODE
G("enclosed by out") = G("enclosed by");
G("separator out") = G("separator");
G("lineTerminator out") = G("lineTerminator");
G("escaped by out") = G("escaped by");

#======================== Dump Fields =========================

G("field con") = down(G("fields con"));

while (G("field con"))
	{
	"output.txt" << G("enclosed by out");
	"output.txt" << conceptname(G("field con"));
	"output.txt" << G("enclosed by out");
	G("field con") = next(G("field con"));
	if (G("field con"))
		"output.txt" << G("separator out");
	}
	
"output.txt" << G("lineTerminator out");

#======================== Dump Records ========================

G("record con") = down(G("records con"));

while (G("record con"))
	{
	G("record attrs") = findattrs(G("record con"));
	while (G("record attrs"))
		{
		G("values") = attrvals(G("record attrs"));
		"output.txt" << G("enclosed by out");
		G("value") = getstrval(G("values"));
		if (G("value") != " ")
			{
			G("value") = strescape(G("value"),G("enclosed by out"),G("escaped by out"));
			"output.txt" << G("value");
			}
		"output.txt" << G("enclosed by out");
		G("record attrs") = nextattr(G("record attrs"));
		if (G("record attrs"))
			"output.txt" << G("separator out");
		}
	G("record con") = next(G("record con"));
	if (G("record con"))
		"output.txt" << G("lineTerminator out");
	}

@@CODE
