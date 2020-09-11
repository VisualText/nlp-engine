###############################################
# FILE: tag100.pat
# SUBJ: Simple-minded look for HTML tags.
# AUTH: AM
# CREATED: 31/May/05 21:15:26
# MODIFIED:
###############################################

@CODE
if (!G("find html"))
  exitpass();
@@CODE

@NODES _ROOT

# Some html headers, h1 h2 etc.
@POST
  L("num") = N("$text",4);
  if (L("num") == "1")
    L("txt") = "ha";
  else if (L("num") == "2")
    L("txt") = "hb";
  else if (L("num") == "3")
    L("txt") = "hc";
  else if (L("num") == "4")
    L("txt") = "hd";
  else if (L("num") == "5")
    L("txt") = "he";
  else if (L("num") == "6")
    L("txt") = "hf";
  else
    L("txt") = "hx";

  if (N(2))
    {
	L("name") = "_E" + L("txt");
	}
  else
    L("name") = "_" + L("txt");
  group(1,5,L("name"));
  N("tag",1) = L("name");
@RULES
_xNIL <-
	\<
	\/ [opt]
	H
	_xNUM
	\>
	@@

# Generic html tag.
@POST
  L("txt") = strtolower(N("$text",3));
  if (N(2))
    {
	L("name") = "_E" + L("txt");
	}
  else
    L("name") = "_" + L("txt");
  group(1,5,L("name"));
  N("tag",1) = L("name");
@RULES
_xNIL <-
	\<
	\/ [opt]
	_xALPHA
	_xWILD
	\>
	@@

@POST
  group(1,3,"_comment");
  N("tag",1) = "comment";
@RULES
_xNIL <-
	_commentTag
	_xWILD
	_EcommentTag
	@@

# Some erroneous html.
@POST
  group(1,3,"<");
@RULES
_xNIL <-
	\&
	lt
	\;
	_xALPHA
	\>
	@@

@RULES
_char <-
	\&
	_xALPHA
	\;
	@@


