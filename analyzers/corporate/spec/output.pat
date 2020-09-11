###############################################
# FILE: output.pat
# SUBJ: comment here
# AUTH: David de Hilster
# CREATED: 10/Nov/00 15:35:55
# MODIFIED: 07/Dec/00
###############################################

@CODE
"output.txt" << "[" << today() << "]" << "\n";

G("sentence") = down(G("parse"));

while (G("sentence"))
	{
	G("object") = down(G("sentence"));
	"output.txt" << "-------------------------------------------------------------\n";
	"output.txt" << "\"" << strwrap(strval(G("sentence"),"text"),60) << "\"\n\n";
	
	while (G("object"))
		{
		G("printed") = 0;
		G("subobject") = down(G("object"));

		if (strval(G("object"),"type") == "company")
			{
			if (G("subobject"))
				"output.txt" << "Company: " << conceptname(G("object")) << "\n";
			}
		else
			"output.txt" << "Action: " << conceptname(G("object")) << "\n";
		
		G("last subobject") = " ";
		while (G("subobject"))
			{
			G("attributes") = findattrs(G("subobject"));
			while (G("attributes"))
				{
				G("values") = attrvals(G("attributes"));
				while (G("values"))
					{
					"output.txt" << "   " << conceptname(G("subobject")) << ": ";
					"output.txt" << "(" << attrname(G("attributes")) << ") ";
					if (getstrval(G("values")))
						"output.txt" << getstrval(G("values")) << "\n";
					else
						"output.txt" << getnumval(G("values")) << "\n";
					G("printed") = 1;
					G("values") = nextval(G("values"));
					}
				G("attributes") = nextattr(G("attributes"));
				}

			if (G("last subobject") != conceptname(G("subobject")))
				{
				"output.txt" << "\n";
				G("printed") = 0;
				}
			G("last subobject") = conceptname(G("subobject"));
			G("subobject") = next(G("subobject"));
			}
			
		if (G("printed"))
			"output.txt" << "\n";
			
		G("object") = next(G("object"));
		}
	G("sentence") = next(G("sentence"));
	}

@@CODE