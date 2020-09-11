###############################################
# FILE: semfuns.pat
# SUBJ: General semantic functions.
# AUTH: AM
# CREATED: 12/Jan/05 14:30:25
# MODIFIED:
###############################################

@DECL


########
# FUNC:   NODESEM
# SUBJ:	  Quick and dirty fetch of "semantic" word for a node.
# CR:	  08/05/02 AM.
# RET:	  text or 0.
# WARN:	GETTING LOWERCASED TEXT.
########

nodesem(L("n"))
{
if (!L("n"))
  return 0;
if (L("sem") = pnvar(L("n"),"sem"))
  return L("sem");
if (L("sem") = pnvar(L("n"),"stem"))
  return L("sem");
if (L("text") = pnvar(L("n"),"text")) # Get from var.
  return L("text");
return strtolower(pnvar(L("n"),"$text")); # Get from tokens.
}

########
# FUNC:   NODESTEM
# SUBJ:	  Fetch stem for a node. Assume one word.
# RET:	  text or 0.
# WARN:	RETURNING LOWERCASED TEXT.
########

nodestem(L("n"))
{
if (!L("n"))
  return 0;
if (L("stem") = pnvar(L("n"),"stem"))
  return L("stem");
L("text") = pnvar(L("n"),"text"); # Get from var, if any.
if (!L("text"))
  L("text") = pnvar(L("n"),"$text");
return nvstem(L("text"));
}

########
# FUNC:	SEMRANGE
# SUBJ:	Look for junk in list of nodes.
# RET:	
# NOTE:	Fills into the given node.
#	Eg, fill an NP with semantics of its noun nodes.
########
semrange(
	L("node"),	# Node to update.
	L("first"),	# First node of range.
	L("last"))	# Last node of range.
{
if (!L("node") || !L("first"))
  return;	# Last could be null, I s'ppose.

if (L("last"))
  L("last") = pnnext(L("last")); # Nice end boundary.

L("n") = L("first");
while (L("n") != L("last"))
  {
  if (pnvar(L("n"),"sem") == "date")
    pnreplaceval(L("node"),"sem date",1);
  L("n") = pnnext(L("n"));
  }
}

########
# FUNC:   SEMNPNP
# SUBJ:   Compose semantics for two nps.
# CR:     05/17/05 AM.
# RET:	[0] = overriding sem object.
#		[1] = overriding text.
########

semnpnp(L("np1"),L("np2"))
{
if (!L("np1") || !L("np2"))
  return 0;

L("s1") = pnvar(L("np1"),"sem");
L("s2") = pnvar(L("np2"),"sem");

L("ne1") = pnvar(L("np1"),"ne text");
L("ne2") = pnvar(L("np2"),"ne text");

if (!L("s1"))
  L("s1") = pnvar(L("np1"),"stem");
if (!L("s2"))
  L("s2") = pnvar(L("np2"),"stem");

if (!L("s1"))
  return L("s2");
if (!L("s2"))
  return L("s1");
if (L("s1") == L("s2"))
  return L("s1");

if (L("s1") == "date" || L("s2") == "date")
  return "date";

if (L("ne1"))
  {
  L("arr")[0] = L("s2");
  L("arr")[1] = L("ne1");
  return L("arr");
  }
if (L("s1") == "name")	# Cap phrase etc.
  {
  L("arr")[0] = L("s2");
  L("arr")[1] = pnvar(L("np1"),"$text");
  return L("arr");
  }

if (L("ne2"))
  {
  L("arr")[0] = L("s1");
  L("arr")[1] = L("ne2");
  return L("arr");
  }
if (L("s2") == "name")
  {
  L("arr")[0] = L("s1");
  L("arr")[1] = pnvar(L("np2"),"$text");
  return L("arr");
  }

return L("s2");	# Default to the last one.
}

########
# FUNC:   SEMVG
# SUBJ:   Semantics for vg.
# CR:     05/17/05 AM.
########

semvg(L("vg"),L("v"))
{
if (!L("vg") || !L("v"))
  return;
L("sem") = pnvar(L("v"),"sem");
if (!L("sem"))
  L("sem") = pnvar(L("v"),"stem");
pnreplaceval(L("vg"),"sem",L("sem"));
}

#########################
# REGISTERPERSNAMES
#########################
# Note: Based on text, not nodes.
registerpersnames(L("txt"))
{
if (!L("txt"))
  return;

L("lc") = strtolower(L("txt"));
L("arr") = split(L("lc")," ");
L("len") = arraylength(L("arr"));
L("ii") = 0;
while (L("ii") < L("len"))
  {
  L("t") = L("arr")[L("ii")];
  L("l") = strlength(L("t"));
  if (L("l") > 1)
    {
	L("c") = strpiece(L("t"),0,0);
	if (strisalpha(L("c")))
	  {
	  dictattr(L("t"),"persname",1);
	  "persname.txt" << L("t") << "\n";
	  }
	}
  ++L("ii");
  }
}

########
# FUNC:   SEMINIT
# SUBJ:   Initialize semantics for current parse.
# CR:     05/24/02 AM.
# INPUT:  
# OUTPUT: 
# NOTE:	  Not doing anything fancy across texts.
#	Could retain objects and/or entities across texts by
#	taking them out of the "currtext" concept.  Or by
#	having a separate collection locus for cross-document
#	accounting.  By doing both, one can decide which entities
#	to retain for cross-document vs current-document only.
########

seminit()
{
G("kbroot") = findroot();

# Kb locus for representations for current text.
G("currtext") = findconcept(G("kbroot"),"currtext");

# Clear out any structures inadvertently saved in the kb,
# or not cleared from a prior text analysis.
if (G("currtext"))
  rmconcept(G("currtext"));
  
# Now build from scratch.
G("currtext") = makeconcept(G("kbroot"),"currtext");

# Manage objects in parse of current text.
G("objects") = makeconcept(G("currtext"),"objects");

# Manage resolved-entities.
G("entities") = makeconcept(G("currtext"),"entities");

# Manage entity references.
G("ents") = makeconcept(G("currtext"),"ents");

# Manage events in parse of current text.			# 06/24/02 AM.
G("events") = makeconcept(G("currtext"),"events");	# 06/24/02 AM.

# Manage the parse of current text.
# Place sentence concepts in here, then actor, object instances
# in there.
G("parse") = makeconcept(G("currtext"),"parse");

# Counter for sentences in current parse.
G("sent count") = 0;
}

########
# FUNC:   SENTREGISTER
# SUBJ:   Register a sentence in the kb.
# CR:     05/24/02 AM.
# INPUT:  
# OUTPUT: 
########

sentregister(
	L("snode") # Sentence node.
	)
{
++G("sent count");	# Counter for sentences.
pnreplaceval(L("snode"),"sent id",G("sent count"));


L("sent name") = "sent" + str(G("sent count"));
if (G("verbose"))
"dump.txt" << "[sentregister: " << L("sent name") << "]\n";
L("sent") = makeconcept(G("parse"), L("sent name"));
replaceval(L("sent"),"type","sent");

# Sentence pnode points to kb sentence concept.
pnreplaceval(L("snode"), "kb sent", L("sent"));

# Kb sentence concept points to sentence pnode.
# Unimplemented in NLP++ kb functions.	# 05/28/02 AM.
# replaceval(L("sent"),"pnode",L("snode"));
}


########
# FUNC:   CLAUSEREGISTER
# SUBJ:   Register a clause in the kb.
# CR:     05/28/02 AM.
# INPUT:  
# OUTPUT: 
########

clauseregister(
	L("cnode"),	# Clause node.
	L("snode")	# Sentence node.
	)
{
if (!L("cnode") || !L("snode"))
  return;

# Get and update count of clauses in sentence node.
L("nclauses") = pnvar(L("snode"),"nclauses");
pnreplaceval(L("snode"),"nclauses", ++L("nclauses"));

# Get sentence's kb concept.
L("kb sent") = pnvar(L("snode"), "kb sent");

L("clause name") = "clause" + str(L("nclauses"));
L("kb concept") = makeconcept(L("kb sent"), L("clause name"));
replaceval(L("kb concept"),"type","clause");
replaceval(L("kb concept"),"zone",pnvar(L("cnode"),"zone"));

if (G("verbose"))
"dump.txt" << "[clausereg: "
	<< conceptname(L("kb sent"))
	<< " "
	<< L("clause name")
	<< "]\n";
if (G("verbose"))
"dump.txt" << "[text= "
	<< pnvar(L("cnode"),"$text")
	<< "]\n";


# Clause pnode points to kb clause concept.
pnreplaceval(L("cnode"), "kb concept", L("kb concept"));

# Layer application-specific onto general structures.
#domclauseregister(L("cnode"),L("snode"));

}

########
# FUNC:   OBJECTREGISTER
# SUBJ:   Register an actor/object in the kb.
# CR:     06/12/05 AM.
# RET:	  Object's concept.
# NOTE:	  New version of an old function.
########

objectregister(
	L("n"),		# Object's node.
	L("cnode")	# Object's clause node.
	)
{
if (!L("n") || !L("cnode"))
  return 0;

if (G("verbose"))
"dump.txt" << "[objectregister:] "
	<< pnvar(L("n"),"$text")
	<< "\n";

# Get clause's kb concept.
L("kb concept") = pnvar(L("cnode"), "kb concept");
if (!L("kb concept"))
  return 0;

# Make and fill obj concept.
L("txt") = strtolower(pnvar(L("n"),"$text"));
L("nobjs") = pnvar(L("cnode"),"nobjs");
pnreplaceval(L("cnode"),"nobjs", ++L("nobjs"));
L("con") = makeconcept(L("kb concept"),"obj" + str(L("nobjs"))
	+ " = " + L("txt"));
replaceval(L("con"),"type","obj");
replaceval(L("con"),"text",L("txt"));
if (L("sem") = pnvar(L("n"),"sem"))
  replaceval(L("con"),"sem",strtolower(L("sem")));
pnreplaceval(L("n"),"kb obj",L("con"));

if (pnvar(L("n"),"eventive"))	# 07/10/02 AM.
  replaceval(L("con"),"eventive",1);	# 07/10/02 AM.

if (pnvar(L("n"),"ne"))	# Named entity.
  replaceval(L("con"),"ne",1);

L("ne type") = pnvar(L("n"),"ne type");
if (L("ne type"))
  replaceval(L("con"),"ne type",L("ne type"));

L("ne type conf") = pnvar(L("n"),"ne type conf");
if (L("ne type conf"))
  replaceval(L("con"),"ne type conf",L("ne type conf"));

L("ne text") = pnvar(L("n"),"ne text");
if (L("ne text"))
  replaceval(L("con"),"ne text",L("ne text"));

return L("con");
}

########
# FUNC:   MERGEOBJECTATTRS
# SUBJ:   Merge a reference's attributes into resolved object.
# CR:     03/21/06 AM.
# INPUT:  
# RET:	con - Resolving object.
# NOTE:		Requiring precise text compare, for now.
########

mergeobjectattrs(
	L("cobj"),		# Object reference.
	L("cobject")	# Resolved object.
	)
{
if (!L("cobj") || !L("cobject"))
  return;

# Named entity logic.
L("ne type conf") = numval(L("cobj"),"ne type conf");
L("conf") = numval(L("cobject"),"ne type conf");
if (L("ne type conf") <= L("conf"))
  return;

# Straight copy.
copyobjectattrs(L("cobj"),L("cobject"));
}

########
# FUNC:   COPYOBJECTATTRS
# SUBJ:   Copy a reference's attributes into resolved object.
# CR:     03/21/06 AM.
# INPUT:  
# RET:	con - Resolving object.
# NOTE:		Requiring precise text compare, for now.
########

copyobjectattrs(
	L("cobj"),		# Object reference.
	L("cobject")	# Resolved object.
	)
{
if (!L("cobj") || !L("cobject"))
  return;

if (numval(L("cobj"),"eventive"))
  replaceval(L("cobject"),"eventive",1);

# Named entity logic.

if (numval(L("cobj"),"ne"))	# Named entity.
  replaceval(L("cobject"),"ne",1);
else
  return;	# DONE.

L("ne type conf") = numval(L("cobj"),"ne type conf");
if (L("ne type conf"))
  replaceval(L("cobject"),"ne type conf",L("ne type conf"));

L("ne type") = strval(L("cobj"),"ne type");
if (L("ne type"))
  replaceval(L("cobject"),"ne type",L("ne type"));

L("ne text") = strval(L("cobj"),"ne text");
if (L("ne text"))
  replaceval(L("cobject"),"ne text",L("ne text"));
}

########
# FUNC:   RESOLVEOBJECT
# SUBJ:   Resolve object reference to objects in the text.
# CR:     05/28/02 AM.
# INPUT:  
# RET:	con - Resolving object.
# NOTE:		Requiring precise text compare, for now.
########

resolveobject(
	L("ref"),		# An object reference concept.
	L("objects")	# KB concept managing objects.
	)
{
if (!L("objects") || !L("ref"))
  return 0;

L("list") = down(L("objects"));
if (!L("list"))	# Empty objects list.
  {
  # Just add the reference and done.
  if (G("verbose"))
	"dump.txt" << "[resolveobject: make object1]\n";
  L("txt") = strval(L("ref"),"text");
  L("con") = makeconcept(L("objects"),"object1 = "+L("txt"));
  replaceval(L("con"),"type","object");
  replaceval(L("objects"),"count",1);
  replaceval(L("con"),"text",L("txt"));
  if (L("sem") = strval(L("ref"),"sem"))
    replaceval(L("con"),"sem",L("sem"));
  copyobjectattrs(L("ref"),L("con"));
  replaceval(L("con"),"refs",L("ref")); # Point to ref concepts.
  replaceval(L("ref"),"resolver",L("con"));
  return L("con");
  }

# Traverse the list of existing objects, looking for a mergable
# object.  For now, requiring exact text match.
L("merged") = 0;	# Not merged with an existing object.
L("done") = 0;
L("cand") = L("list");	# Candidate object.
while (!L("done"))
  {
  # Should use hierarchy concepts as part of the merge test...
  if ((strval(L("cand"),"text") == strval(L("ref"),"text"))
#   && (strval(L("cand"),"sem") == strval(L("ref"),"sem"))
   )
    {
	# Successful merge.
	L("merged") = 1;
	L("done") = 1;
	addconval(L("cand"),"refs",L("ref"));
    replaceval(L("ref"),"resolver",L("cand"));
    mergeobjectattrs(L("ref"),L("cand"));
	return L("cand");
	}
  
  L("cand") = next(L("cand"));
  if (!L("cand"))
    L("done") = 1;
  }


if (!L("merged"))	# Didn't find an existing object.
  {
  # Just add the reference and done.
  L("ct") = numval(L("objects"),"count");
  L("txt") = strval(L("ref"),"text");
  replaceval(L("objects"),"count", ++L("ct"));	# inc count.
  L("nm") = "object" + str(L("ct")) + " = " + L("txt");
  if (G("verbose"))
	"dump.txt" << "[resolveobject: make "
  	<< L("nm") << "]\n";
  L("con") = makeconcept(L("objects"),L("nm"));
  replaceval(L("con"),"type","object");
  replaceval(L("con"),"text",strval(L("ref"),"text"));
  if (L("sem") = strval(L("ref"),"sem"))
    replaceval(L("con"),"sem",L("sem"));
  copyobjectattrs(L("ref"),L("con"));
  addconval(L("con"),"refs",L("ref")); # Point to ref concept.
  replaceval(L("ref"),"resolver",L("con"));
  return L("con");
  }
return L("con");
}

########
# FUNC:   ENTREGISTER
# SUBJ:   Register an entity reference in the kb.
# CR:     03/21/06 AM.
# RET:	  Entity reference concept.
# NOTE:	  Distinguishing ent and entity.
#	ent = specific reference to entity in text.
#   entity = resolved entity that covers multiple references.
#	Also distinguishing entities from objects.
#	eg, "New York residents".  Object is "residents", but
#	a contained entity is "New York".
#
#	Owning concept could be the entity reference itself.
########

entregister(
	L("n"),		# Entity's pnode.
	L("nown")	# Entity's owning node (e.g., an _np node).
	)
{
if (!L("n") || !L("nown"))
  return 0;


if (!pnvar(L("n"),"ne"))	# Not named entity.
  return 0;

# (Could make this a parameter).
L("ents") = G("ents");	# Locus for placing ents.

# Get clause's kb concept.
#L("kb concept") = pnvar(L("nown"), "kb concept");

# Make and fill ent concept.
L("txt") = strtolower(pnvar(L("n"),"$text"));
L("nents") = numval(L("ents"),"nents");
replaceval(L("ents"),"nents", ++L("nents"));

L("con") = makeconcept(L("ents"),"ent" + str(L("nents"))
	+ " = " + L("txt"));

#replaceval(L("con"),"type","ent");
replaceval(L("con"),"text",L("txt"));
if (L("sem") = pnvar(L("n"),"sem"))
  replaceval(L("con"),"sem",strtolower(L("sem")));
pnreplaceval(L("n"),"kb ent",L("con"));

if (pnvar(L("n"),"eventive"))	# 07/10/02 AM.
  replaceval(L("con"),"eventive",1);	# 07/10/02 AM.

replaceval(L("con"),"ne",1);

L("ne type") = pnvar(L("n"),"ne type");
if (L("ne type"))
  replaceval(L("con"),"ne type",L("ne type"));

L("ne type conf") = pnvar(L("n"),"ne type conf");
if (L("ne type conf"))
  replaceval(L("con"),"ne type conf",L("ne type conf"));

L("ne text") = pnvar(L("n"),"ne text");
if (L("ne text"))
  replaceval(L("con"),"ne text",L("ne text"));

return L("con");
}

########
# FUNC:   RESOLVEENTITY
# SUBJ:   Lookup ent reference in entities.
# NOTE:	  An np may have a list of entities, so that
#		  list of entities might not match list of objects.
# RET:    Entity concept.
########

resolveentity(
	L("cref"),	# Ent reference concept.
	L("entities")
	)
{
if (!L("cref") || !L("entities"))
  return 0;

L("list") = down(L("entities"));
if (!L("list"))	# Empty objects list.
  {
  # Just add the reference and done.
  if (G("verbose"))
	"dump.txt" << "[resolveentity: make entity1]\n";
  L("txt") = strval(L("cref"),"text");
  L("con") = makeconcept(L("entities"),"entity1 = "+ L("txt"));
#  replaceval(L("con"),"type","entity");
  replaceval(L("entities"),"count",1);
  replaceval(L("con"),"text",L("txt"));
  if (L("sem") = strval(L("cref"),"sem"))
    replaceval(L("con"),"sem",L("sem"));
  copyobjectattrs(L("cref"),L("con"));
  replaceval(L("con"),"refs",L("cref")); # Point to ref concepts.
  replaceval(L("cref"),"resolver",L("con"));
  return L("con");
  }


# Traverse the list of existing objects, looking for a mergable
# object.  For now, requiring exact text match.
L("merged") = 0;	# Not merged with an existing object.
L("done") = 0;
L("cand") = L("list");	# Candidate object.
while (!L("done"))
  {
  # Should use hierarchy concepts as part of the merge test...
  if ((strval(L("cand"),"text") == strval(L("cref"),"text"))
#   && (strval(L("cand"),"sem") == strval(L("cref"),"sem"))
   )
    {
	# Successful merge.
	L("merged") = 1;
	L("done") = 1;
	addconval(L("cand"),"refs",L("cref"));
    replaceval(L("cref"),"resolver",L("cand"));
    mergeobjectattrs(L("cref"),L("cand"));
	return L("cand");
	}
  
  L("cand") = next(L("cand"));
  if (!L("cand"))
    L("done") = 1;
  }


if (!L("merged"))	# Didn't find an existing object.
  {
  # Just add the reference and done.
  L("ct") = numval(L("entities"),"count");
  L("txt") = strval(L("cref"),"text");
  replaceval(L("entities"),"count", ++L("ct"));	# inc count.
  L("nm") = "entity" + str(L("ct")) + " = " + L("txt");
  if (G("verbose"))
	"dump.txt" << "[resolveentity: make "
  	<< L("nm") << "]\n";
  L("con") = makeconcept(L("entities"),L("nm"));
#  replaceval(L("con"),"type","entity");
  replaceval(L("con"),"text",strval(L("cref"),"text"));
  if (L("sem") = strval(L("cref"),"sem"))
    replaceval(L("con"),"sem",L("sem"));
  copyobjectattrs(L("cref"),L("con"));
  addconval(L("con"),"refs",L("cref")); # Point to ref concept.
  replaceval(L("cref"),"resolver",L("con"));
  return L("con");
  }
return L("con");

}

########
# FUNC:   FINDCONCEPTWITHPARENT
# SUBJ:   Lookup semantic concept with given parent name.
# CR:     06/02/02 AM.
# INPUT:  
# OUTPUT: L("sem con") = Concept having parent with given name.
# NOTE:	 Looking up in dictionary, then traversing from there
#		 to a semantic hierarchy.
#		 I guess conval works with attrs by name, not concept.
#		 Todo: handle list of concept values when fetching the
#		 attr.
########

findconceptwithparent(
	L("word"),			# Word to lookup in dictionary.
	L("attrname"),		# Attribute to follow in dict word.
	L("parentname")		# Parent's name in sem hierarchy.
	)
{
L("con") = 0;
if (!L("word") || !L("attrname") || !L("parentname"))
  return 0;
#"dump.txt" << "[fcwpar: word=" << L("word")
#	<< ",attr=" << L("attrname")
#	<< ",par=" << L("parentname")
#	<< "\n";
L("word con") = dictfindword(L("word"));
if (!L("word con"))
  return 0;
L("sem con") = conval(L("word con"),L("attrname"));
if (!L("sem con"))
  return 0;
#"dump.txt" << "[fcwpar: sem con=" << conceptname(L("sem con")) << "\n";
L("parent") = conceptname(up(L("sem con")));
#"dump.txt" << "[fcwpar: par=" << L("parent")
#	<< ",parname=" << L("parentname")
#	<< "\n";
if (L("parent") != L("parentname"))
  return 0;
return L("sem con");	# Success.
}

########
# FUNC:   ANAPHORIC
# SUBJ:   Traverse events to resolve anaphora.
# CR:     05/28/02 AM.
# INPUT:  
# OUTPUT: 
# NOTE:	Perhaps should be in discourse functions pass.
########

anaphoric(
	L("events") # Domain events concept.
	)
{
if (!L("events"))
  return;
L("ev") = down(L("events"));
while (L("ev"))
  {
  # If event is missing an actor.
  # Heur: First look in the same clause.
  if (!conval(L("ev"),"actor"))
    {
	if (G("verbose"))
	  "dump.txt" << "[ana: Missing actor in '"
		<< conceptname(L("ev"))
		<< "]\n";

	# If event is eventive np, don't use an eventive np to
	# resolve it.
	L("refs") = conval(L("ev"),"refs");
	L("eventive") = numval(L("refs"),"eventive");

	# Look at first ref (that led to event creation).
	# May want to have a special field for that, given
	# that we don't know what kb does with ordering.
	L("ref") = conval(L("ev"),"refs");
	L("con") = L("ref");
	L("done") = 0;
	while (!L("done"))
	  {
	  if (L("con") = prev(L("con")))
	    {
		if (strval(L("con"),"type") == "obj")
		  {
		  L("ok") = 0;
		  if (!L("eventive"))
		    L("ok") = 1;
		  else if (!numval(L("con"),"eventive")
		   && !numval(L("con"),"count")) # A group of nps.
		    L("ok") = 1;
		  if (L("ok"))
		   {
		    # Found anaphor.
		    L("done") = 1;
		    # Point to actor.
		    if (L("res") = conval(L("con"),"resolver"))
		      {
			  replaceval(L("ev"),"actor",L("res"));
			  if (G("verbose"))
				"dump.txt" << "anaphoric: found actor="
				<< strval(L("res"),"text") << "\n";
			  }
			}
		  }
		}
	  else
	    L("done") = 1;
	  }
	
	# Look backward in same clause.
	
	}
  L("ev") = next(L("ev"));
  }
}

########
# FUNC:   MOVEBACK
# SUBJ:   Given a kb clause object, back up one.
# CR:     07/25/02 AM.
# INPUT:  
# OUTPUT: 
# NOTE:	  May back up into a previous clause.
#		Utility for resolving references.
########

moveback(L("con"))
{
if (!L("con"))
  return 0;
if (L("prev") = prev(L("con")))
  return L("prev");
L("type") = strval(L("con"),"type");
if (L("type") == "clause" || L("type") == "sent")
  return 0;

# Get current clause.
if (!(L("cls") = up(L("con")) ))
  return 0;

# Get previous clause.
if (!(L("cls") = prev(L("cls")) ))
  return 0;

# Get first item in prev clause.
if (!(L("dn") = down(L("cls")) ))
  return 0;

# Get last item in prev clause.
while (L("tmp") = next(L("dn")) )
  {
  L("dn") = L("tmp");
  }
return L("dn");
}

########
# FUNC:   PREVNP
# SUBJ:   Given a sentence node, find its last np so far.
# CR:     07/29/02 AM.
# INPUT:  
# OUTPUT: 
# NOTE:
#		Utility for resolving references.
########

prevnp(L("sent")) # sentence node
{
if (!L("sent"))
  return 0;
if (L("np") = pnvar(L("sent"),"last np"))
  {
  return L("np");
  }
return 0;
}


######
# FN:  NENODERANGE
# SUBJ: Look for named entities in node range.
# RET:	array of nodes.
######

nenoderange(L("start"),L("end"))
{
if (!L("start"))
  return 0;
if (L("end"))
  L("end") = pnnext(L("end"));
L("node") = L("start");
L("ii") = 0;
L("arr") = 0;
while (L("node"))
  {
  if (pnvar(L("node"),"ne"))
    {
	L("arr")[L("ii")] = L("node");
	++L("ii");
	}
  if (L("node") == L("end"))
    return L("arr");
  L("node") = pnnext(L("node"));
  }
return L("arr");
}

@CODE
G("hello") = 0;
@@CODE
