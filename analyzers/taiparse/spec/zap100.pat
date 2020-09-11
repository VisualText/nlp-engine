###############################################
# FILE: zap100.pat
# SUBJ: Zap HTML tags.
# AUTH: AM
# CREATED: 31/May/05 21:17:29
# MODIFIED:
###############################################
@CODE
if (!G("find html"))
  exitpass();
# If no title found, flag it.
if (!G("title"))
   {
   if (G("error"))
     {
   L("fname") = G("$apppath") + "\\data\\errhtml.txt";
   L("errhtml") = openfile(L("fname"),"app");
   L("errhtml") << G("$inputhead") << ": No title found." << "\n";
   closefile(L("errhtml"));
     }
   }
else if (G("verbose"))
   {
   L("fname") = G("$apppath") + "\\data\\htmltitle.txt";
   L("htmltitle") = openfile(L("fname"),"app");
   L("htmltitle") << G("$inputhead") << ": " << G("title") << "\n";
   closefile(L("htmltitle"));
  }
@@CODE
@NODES _ROOT
@CHECK
  if (!N("tag"))	# Not a tag.
    fail();
@POST
  excise(1,1);
@RULES
_xNIL <-
	_xANY	# Need an _xNONLIT and _xLIT
	@@
