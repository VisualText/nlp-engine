###############################################
# FILE: ne_xml.pat
# SUBJ: Output resolved named entities from KB.
# AUTH: AM
# CREATED: 21/Mar/06 21:58:28
# MODIFIED:
###############################################

@DECL
#############
## FN:   NETAG
## SUBJ: Print an XML tag for named entity.
#############
netag(
	L("con"),	# Current entity concept to print.
	L("name"),	# Tag name.
	L("out")	# Output stream.
	)
{
if (!L("con") || !L("name") || !L("out"))
  return; # Error.


L("txt") = strval(L("con"),"ne text");
if (!L("txt"))
  L("txt") = strval(L("con"),"text");

if (!L("txt"))
  return;

# Only print if it has a text.
L("out") << "<" << L("name");

L("type") = strval(L("con"),"ne type");
if (!L("type"))
  L("type") = "unknown";
L("out") << " type=\"" << L("type") << "\"";

L("sem") = strval(L("con"),"sem");
if (!L("sem"))
  L("sem") = "unknown";
L("out") << " sem=\"" << L("sem") << "\"";

L("conf") = numval(L("con"),"ne type conf");
if (!L("conf"))
    L("conf") = 0;
L("out") << " conf=\"" << L("conf") << "\"";

L("out") << " text=\"" << L("txt") << "\"";

L("out") << "/>" << "\n";
}

@CODE
L("hello") = 0;

# Use the locus to see if printing this stuff out.
if (!G("entities"))
  exitpass();

L("dir") = G("$apppath") + "\\data\\reuter";
L("fname") = L("dir") + "\\" + G("$inputhead") + ".xml";
G("oxml") = openfile(L("fname"));

xmlheader(G("oxml"));
xmlstart("doc",G("oxml"));
xmlshort("input",G("$inputname"),G("oxml"));
xmlshort("output",G("$inputhead")+".xml",G("oxml"));

L("con") = down(G("entities"));
while (L("con"))
  {
  netag(L("con"),"ne",G("oxml"));
  L("con") = next(L("con"));
  }

xmlend("doc",G("oxml"));

if (G("oxml"))
  {
  closefile(G("oxml"));
  G("oxml") = 0;
  }

@@CODE
