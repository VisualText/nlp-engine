###############################################
# FILE: qsemfuns.pat
# SUBJ: Query/interrogative semantic funs.
# AUTH: AM
# CREATED: 11/May/06 14:03:25
# MODIFIED:
###############################################

@DECL

########
# FUNC:   DOMVNN
# SUBJ:   Query semantics for VNN clause.
# EX:	  "Is Smith an author?"
########

domvnn(
	L("clause"),
	L("vg"),
	L("np1"),
	L("np2")
	)
{
if (!L("clause") || !L("vg") || !L("np1") || !L("np2"))
  return;

L("sem vg") = nodesem(L("vg"));
L("sem np1") = nodesem(L("np1"));
L("sem np2") = nodesem(L("np2"));
#"sem.txt" << L("sem vg")
#	<< "," << L("sem np1")
#	<< "," << L("sem np2")
#	 << "\n";

# be X author
if (L("sem vg") != "be")
  return;
if (L("sem np2") == "author")
  {
  # Case where np1 is a name.
  # Case where np1 is undetermined or not a known item.
  # (Case of one cap, two caps, etc.  Caps patterns...)
  # Try
  # select * from author where auth_last_name='x';
  L("nm") = pnvar(L("np1"),"$text");
  L("table") = "author";
  L("field") = "auth_last_name";
  "cmd.txt"
  	<< "SELECT * FROM "
	<< L("table")
	<< " WHERE "
	<< L("field")
	<< " = '"
	<< L("nm")
	<< "';"
	<< "\n"
	;
  }
}

@CODE
L("hello") = 0;
@@CODE
