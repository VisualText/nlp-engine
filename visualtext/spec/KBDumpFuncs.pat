###############################################
# FILE: KBDumpFuncs.pat
# SUBJ: Call the DumpKB function
# AUTH: Your Name
# CREATED: 2020-11-19 8:40:53
# MODIFIED:
###############################################

@DECL

###############################################
# KB DUMP FUNCTIONS
###############################################

DumpKB(L("top"),L("full")) {
	if (num(G("$passnum")) < 10) {
		L("file") = "ana00" + str(G("$passnum"));
	}else if (num(G("$passnum")) < 100) {
		L("file") = "ana0" + str(G("$passnum"));
	} else {
		L("file") = "ana0" + str(G("$passnum"));
	}
	L("file") = L("file") + ".kb";
	DumpKBRecurse(L("file"),L("top"),0,L("full"));
	L("file") << "\n";
}

DumpKBRecurse(L("file"),L("top"),L("level"),L("full")) {
	if (L("level") == 0) {
		L("file") << conceptname(L("top")) << "\n";
	}
	L("con") = down(L("top"));
	while (L("con")) {
		L("file") << SpacesStr(L("level")+1) << conceptname(L("con"));
		OutputAttributes(L("file"),L("con"),L("full"));
		L("file") << "\n";
		if (down(L("con"))) {
			L("lev") = 1;
			if (L("full")) L("lev") = 2;
			DumpKBRecurse(L("file"),L("con"),L("level")+L("lev"),L("full"));
		}

		L("con") = next(L("con"));
	}
}

OutputAttributes(L("file"),L("con"),L("full")) {
	L("attrs") = findattrs(L("con"));
	if (L("attrs")) L("file") << ": ";
	if (L("full") && L("attrs")) L("file") << "\n";
	L("first attr") = 1;
	
	while (L("attrs")) {
		L("vals") = attrvals(L("attrs"));
		if (!L("full") && !L("first attr")) {
			L("file") << ", ";
		}
		if (L("full")) {
			if (!L("first attr")) L("file") << "\n";
			L("file") << SpacesStr(L("level")+2);
		}
		L("file") << attrname(L("attrs")) << "=[";
		L("first") = 1;
		
		while (L("vals")) {
			if (!L("first"))
				L("file") << ",";
			L("val") = getstrval(L("vals"));
			L("num") = getnumval(L("vals"));
			L("con") = getconval(L("vals"));
			if (L("con")) {
				L("file") << conceptpath(L("con"));
			} else if (!L("full") && strlength(L("val")) > 20) {
				L("shorty") = strpiece(L("val"),0,20);
				L("file") << L("shorty");
				L("file") << "...";
				if (strendswith(L("val"),"\""))
					L("file") << "\"";
			} else if (L("num") > -1) {
				L("file") << str(L("num"));
			} else {
				L("file") << L("val");
			}
			L("first") = 0;
			L("vals") = nextval(L("vals"));
		}
		
		L("file") << "]";
		L("first attr") = 0;
		L("attrs") = nextattr(L("attrs"));
	}
}

# Because NLP++ doesn't allow for empty strings,
# this function can only be called with "num" >= 1
SpacesStr(L("num")) {
    L("n") = 1;
	L("spaces") = "  ";
	while (L("n") < L("num")) {
		L("spaces") = L("spaces") + "  ";
		L("n")++;
	}
	return L("spaces");
}

@@DECL