###############################################
# FILE: buff_out.pat
# SUBJ: comment
# AUTH: David de Hilster
# CREATED: 15/Sep/20 14:03:28
# MODIFIED:
###############################################

@DECL

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

xmlheader(G("out"));
xmlrecurseall(G("out"),pnroot(),0);

@@CODE
