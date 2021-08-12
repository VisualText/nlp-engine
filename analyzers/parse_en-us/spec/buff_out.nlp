###############################################
# FILE: buff_out.pat
# SUBJ: comment
# AUTH: David de Hilster
# CREATED: 15/Sep/20 14:03:28
# MODIFIED:
###############################################

@DECL

eclXMLTree(
	L("out"),
	L("n"),
	L("parent id")
	)
{
L("childs") = pndown(L("n"));
L("leaf") = 0;

while (L("childs")) {
	L("name") = pnname(L("childs"));
	if (strlength(L("name")) > 1) {
		L("id") = G("id")++;
		L("tag") = strtrim(strpiece(L("name"),0,strlength(L("name"))-1));

		L("out") << "<vertice";
		getAttributes(L("out"),L("childs"));
		L("out") << ">\n";
		L("out") << "  <id>" << L("id") << "</id>\n";
		L("out") << "  <label>" << L("tag") << "</label>\n";
		L("out") << "</vertice>\n";

		if (L("parent id")) {
			L("out") << "<edge>\n";
			L("out") << "  <source>" << L("parent id") << "</source>\n";
			L("out") << "  <target>" << L("id") << "</target>\n";
			L("out") << "</edge>\n";			
		}

		if (pndown(L("childs"))) {
			eclXMLTree(L("out"),L("childs"),L("id"));
		}
	}

	L("childs") = pnnext(L("childs"));
}
return L("leaf");
}

xmlrecurseall(
	L("out"),
	L("n"),	     # Current node.
	L("level")   # level of recursion
	)
{
L("childs") = pndown(L("n"));
L("leaf") = 0;

while (L("childs")) {
	L("name") = pnname(L("childs"));
	if (strlength(L("name")) > 1) {
		L("tag") = strpiece(L("name"),1,strlength(L("name"))-1);
		if (pndown(L("childs"))) {
			G("id")++;
			L("out") << "\n" << spaces(L("level")) << "<" << L("tag");
			getAttributes(L("out"),L("childs"));
			L("out") << ">";
			if (!xmlrecurseall(L("out"),L("childs"),L("level")+1))
				L("out") << "\n" << spaces(L("level"));
			L("out") << "</" << L("tag") << ">";
		} else {
			L("out") << pnvar(L("childs"),"$text");
			L("leaf") = 1;
		}
	}

	L("childs") = pnnext(L("childs"));
}
return L("leaf");
}

getAttributes(L("out"),L("n")) {
	addAttribute(L("out"),L("n"),"stem");
	addAttribute(L("out"),L("n"),"voice");
	addAttribute(L("out"),L("n"),"tense");
	addAttribute(L("out"),L("n"),"aspect");
}

addAttribute(L("out"),L("n"),L("name")) {
	L("value") = pnvar(L("n"),L("name"));
	if (L("value"))
  		L("out") << " " << L("name") << "=\"" << L("value") << "\"";
}

spaces(L("num")) {
	L("i") = 1;
	L("spaces") = " ";
	while (L("i")++ < L("num")) {
	   L("spaces") = L("spaces") + "   ";
	}
	return L("spaces");
}

@CODE

if (interactive())
	G("out") = "out.xml";
else
	G("out") = cbuf();

G("id") = 1;
xmlheader(G("out"));
#xmlrecurseall(G("out"),pnroot(),0);
eclXMLTree(G("out"),pnroot(),0);

@@CODE
