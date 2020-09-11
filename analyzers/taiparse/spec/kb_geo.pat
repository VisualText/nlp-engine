###############################################
# FILE: kb_geo.pat
# SUBJ: Load KB with geo information.
# AUTH: AM
# CREATED: 13/Jan/05 13:54:42
# MODIFIED:
###############################################

@DECL
########
# FUNC:	LOADATTR
# SUBJ:	Load attr into dictionary concept.
# RET:	
# NOTE:	Updates dictionary in kb.
########
loadattr(
	L("entry"),
	L("attr_s"),
	L("val")
	)
{
if (!L("entry") || !L("attr_s"))
  return;
L("lc") = strtolower(L("entry"));
L("dict concept") = dictgetword(L("lc"));
if (!L("dict concept"))
  return;

replaceval(L("dict concept"),L("attr_s"),L("val"));
}


@CODE

# One-time load for this analyzer instance.
if (numval(findroot(),"geo-kb"))
  exitpass();
else
  replaceval(findroot(),"geo-kb",1); # Mark as loaded now.

# As an example.
# Thai attr(country) => Thailand

loadattr("Bangladeshi","country","Bangladesh");
loadattr("Burma","country","Myanmar");
loadattr("Burmese","country","Myanmar");
loadattr("Cambodian","country","Cambodia");
loadattr("Chinese","country","China");
loadattr("Indian","country","India");
loadattr("Indonesian","country","Indonesia");
loadattr("Japanese","country","Japan");
loadattr("Laotian","country","Laos");
loadattr("Malaysian","country","Malaysia");
loadattr("Myanmaran","country","Myanmar");
loadattr("Singaporan","country","Singapore");
loadattr("Singaporean","country","Singapore");
loadattr("Soviet","country","Soviet Union");
loadattr("Sri Lankan","country","Sri Lanka");
loadattr("Thai","country","Thailand");
loadattr("Viet","country","Vietnam");
loadattr("Vietnamese","country","Vietnam");
@@CODE
