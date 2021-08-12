###############################################
# FILE: oldsemfuns.pat
# SUBJ: comment
# AUTH: AM
# CREATED: 12/Jun/05 15:36:11
# MODIFIED:
###############################################

@DECL


########
# FUNC:   ADVLREGISTER
# SUBJ:   Register an adverbial in the kb.
# CR:     09/11/02 AM.
# INPUT:  
# OUTPUT: 
# NOTE:   For adverbials not attached to a clause, eg, at the
#         start of a sentence.
#		  Register for subsequent clauses.
#		  Check for prior clauses.
########

advlregister(
	L("anode"),	# Adverbial node.
	L("snode")	# Sentence node.
	)
{
if (!L("anode") || !L("snode"))
  return;

#domadvlregister(L("anode"),L("snode"));

# Get and update count of advls in sentence node.
L("nadvls") = pnvar(L("snode"),"nadvls");
pnreplaceval(L("snode"),"nadvls", ++L("nadvls"));

# Get sentence's kb concept.
L("kb sent") = pnvar(L("snode"), "kb sent");

L("advl name") = "advl" + str(L("nadvls"));
L("kb concept") = makeconcept(L("kb sent"), L("advl name"));
replaceval(L("kb concept"),"type","advl");
replaceval(L("kb concept"),"zone",pnvar(L("anode"),"zone"));

if (G("verbose"))
"dump.txt" << "[advlreg: "
	<< conceptname(L("kb sent"))
	<< " "
	<< L("advl name")
	<< "]\n";
if (G("verbose"))
"dump.txt" << "[text= "
	<< pnvar(L("anode"),"$text")
	<< "]\n";


# Advl pnode points to kb advl concept.
pnreplaceval(L("anode"), "kb concept", L("kb concept"));

# Some crosslinks.
L("advls") = pnvar(L("snode"),"advls");
L("len") = arraylength(L("advls"));
if (L("advls"))
  L("advls")[L("len")] = L("anode");
else
  L("advls") = L("anode");	# First one.
pnreplaceval(L("snode"),"advls",L("advls"));

}


########
# FUNC:   CLAUSETRAVERSE
# SUBJ:   Manually traverse a clause in the parse tree.
# CR:     05/28/02 AM.
# INPUT:  
# OUTPUT: 
########

clausetraverse(
    L("node"),	# Root node within a clause
	L("cnode"),	# Clause node.
	L("sent")	# Current sentence.
	)
{
if (!L("node") || !L("cnode"))
  return;

if (G("verbose"))
"dump.txt" << "[clausetraverse:]" << "\n";

# Get clause's kb concept.
L("kb concept") = pnvar(L("cnode"), "kb concept");
if (!L("kb concept"))
  return;

# Check for eventive np.	# 07/25/02 AM.
L("nm") = pnname(L("node"));
if (L("nm") == "_np")
  {
  # Record actor or object.
  L("con") = objectregister(L("node"),L("cnode"));
	
  # Resolve with existing objects/events.
  resolveobjects(L("con"),G("objects"),G("events"));
  
  return;
  }

if (L("nm") != "_clause" && G("verbose"))
  "dump.txt" << "[clausetraverse: concept=" << L("nm") << "]\n";

# Get first child node.
L("n") = pndown(L("node"));

while (L("n"))
  {
  L("nname") = pnname(L("n"));
  if (L("nname") == "_np"
   || L("nname") == "_nps")
    {
	if (pnvar(L("n"),"eventive"))
	  domnp(L("n"),L("sent"));
	pnreplaceval(L("sent"),"last np", L("n"));

    # Record actor or object.
	L("con") = objectregister(L("n"),L("cnode"));
	
	# Resolve with existing objects/events.
	resolveobjects(L("con"),G("objects"),G("events"));
    }
  else if (L("nname") == "_vg")
    {
    # Record act.
	L("nacts") = pnvar(L("cnode"),"nacts");
	pnreplaceval(L("cnode"),"nacts", ++L("nacts"));
	L("con") = makeconcept(L("kb concept"),"act" + str(L("nacts")));
    replaceval(L("con"),"type","act");
	replaceval(L("con"),"text",
		strtolower(pnvar(L("n"),"$text")));
	pnreplaceval(L("n"),"kb act",L("con"));

	# Only handling singletons for the moment.
	if (L("nsem") = pnvar(L("n"),"sem"))
	  replaceval(L("con"),"sem", L("nsem"));

    if (L("tmp") = pnvar(L("n"),"act val"))
	  replaceval(L("con"),"act val",L("tmp"));

	if (pnvar(L("n"),"passive"))
	  replaceval(L("con"),"passive",1);
	if (pnvar(L("n"),"neg"))
	  replaceval(L("con"),"negative",1);
    }
  else if (L("nname") == "_advl" && pnvar(L("n"),"pp"))
    {
	clauseadvltraverse(L("n"),L("cnode"),L("sent"));
	}
  else if (L("nname") == "_advl"
   && pnvar(L("n"),"pattern") == "that-clause" )
    {
	# Assume np+that+clause pattern.
	L("clause") = pnvar(L("n"),"clause");
	clauseregister(L("clause"), L("sent"));
	clausetraverse(L("clause"),L("clause"),L("sent"));
	}
  else if (L("nname") == "_adj")	# assume np vg adj pattern
    {
    # Record state.
	L("con") = stateregister(L("n"),L("cnode"));
	}
  else if (L("nname") == "_clause")
    {
    "clauses.txt" << pnvar(L("n"),"$text") << "\n\n";
	clauseregister(L("n"), L("sent"));
	clausetraverse(L("n"), L("n"), L("sent"));
	}
  else	# Traverse advl etc.
    clausetraverse(L("n"), L("cnode"), L("sent"));

  L("n") = pnnext(L("n"));
  }

# If the current item that we've traversed is a clause,
# fix up the event semantics for it.
semclause(L("node"),L("sent"));
}


########
# FUNC:   CLAUSEADVLTRAVERSE
# SUBJ:   Manually traverse an adverbial in the parse tree.
# CR:     09/11/02 AM.
# INPUT:  
# OUTPUT: 
########

clauseadvltraverse(
    L("n"),	# Adverbial node.
	L("cnode"),
	L("sent")	# Current sentence.
	)
{
if (G("verbose"))
"dump.txt" << "[clauseadvltraverse:]" << "\n";

domadvl(L("n"), L("sent"));
if (L("nps") = pnvar(L("n"),"pn nps"))
  {
  L("advl np") = L("nps")[0];
	pnreplaceval(L("sent"),"last np",
	      L("nps")[0]);	# (Reverse order...)
  if (L("cnode"))
    {
    L("con") = objectregister(L("advl np"),L("cnode"));
    L("np con") = resolveobjects(L("con"),G("objects"),
	  	G("events"));

    if (!L("np con") && G("verbose"))
	  	"dump.txt" << "no np con" << "\n";
    }
	  
  # If np belongs to an eventive, place it there.
  if (L("ev") = pnvar(L("advl np"),"pn eventive"))
	{
    if (G("verbose"))
      "dump.txt" << pnvar(L("advl np"),"$text")
		  << " =event=> "
		  << pnvar(L("ev"),"$text")
		  << "\n";
	L("ev con") = pnvar(L("ev"),"kb obj");
	L("resolver") = conval(L("ev con"),"resolver");
	replaceval(L("resolver"),"actor",L("np con"));
	replaceval(L("np con"),"event",L("resolver"));
	}
  }
}


########
# FUNC:   ADVLTRAVERSE
# SUBJ:   Handle a non-clause adverbial.
# CR:     09/11/02 AM.
# INPUT:  
# OUTPUT: 
# NOTE:   Not a "traversal", though named that for consistency.
#         May involve lists and other complexities in the future.
########

advltraverse(
    L("n"),	# Adverbial node.
	L("sent")	# Current sentence.
	)
{
if (G("verbose"))
"dump.txt" << "[advltraverse:]" << "\n";

if (!L("n") || !L("sent"))
  return;

domadvl(L("n"),L("sent"));
if (L("nps") = pnvar(L("n"),"pn nps"))
  {
  L("advl np") = L("nps")[0];
	pnreplaceval(L("sent"),"last np",
	      L("nps")[0]);	# (Reverse order...)

    L("con") = objectregister(L("advl np"),L("n"));
    L("np con") = resolveobjects(L("con"),G("objects"),
	  	G("events"));

    if (!L("np con") && G("verbose"))
	  	"dump.txt" << "no np con" << "\n";
	  
  # If np belongs to an eventive, place it there.
  if (L("ev") = pnvar(L("advl np"),"pn eventive"))
	{
    if (G("verbose"))
      "dump.txt" << pnvar(L("advl np"),"$text")
		  << " =event=> "
		  << pnvar(L("ev"),"$text")
		  << "\n";
	L("ev con") = pnvar(L("ev"),"kb obj");
	L("resolver") = conval(L("ev con"),"resolver");
	replaceval(L("resolver"),"actor",L("np con"));
	replaceval(L("np con"),"event",L("resolver"));
	}
  }
}

########
# FUNC:   ADVLSHANDLE
# SUBJ:   Traverse tree of adverbials.
# CR:     09/19/02 AM.
########

advlshandle(
    L("n"),		# Adverbial node.
	L("sent")	# Current sentence.
	)
{
if (!L("n") || !L("sent"))
  return;

L("zone") = pnvar(L("n"),"zone");

if (pnvar(L("n"),"advl list"))	# List of advls.
  {
  L("list") = pndown(L("n"));
  while (L("list"))
    {
	if (L("zone"))
	  pnreplaceval(L("list"),"zone",L("zone"));
	advlshandle(L("list"),L("sent"));
	L("list") = pnnext(L("list"));
	}
  return;
  }

# Non-list adverbial here.
advlhandle(L("n"),L("sent"));
}

########
# FUNC:   ADVLHANDLE
# SUBJ:   Handle a clause-independent adverbial.
# CR:     09/19/02 AM.
# NOTE:	  Taken from qclause100, _advl rule.
########

advlhandle(
    L("n"),		# Adverbial node.
	L("sent")	# Current sentence.
	)
{
if (!L("n") || !L("sent"))
  return;

L("zone") = pnvar(L("n"),"zone");

L("clause") = pnvar(L("n"),"clause");
L("clauses") = pnvar(L("n"),"clauses");
if (L("clause"))
 {
 "clauses.txt" << pnvar(L("n"),"$text") << "\n\n";
 if (L("clause"))
   {
   if (L("zone"))
     pnreplaceval(L("clause"),"zone",L("zone"));
   clauseregister(L("clause"),L("sent"));
   L("last np") = prevnp(L("sent"));
   clausetraverse(L("clause"),L("clause"),L("sent"));
   clauseresolve(L("clause"),L("sent"),L("last np"));
   pnreplaceval(L("sent"),"clause",L("clause"));
   }
 else if (L("clauses"))
   {
   L("count") = arraylength(L("clauses"));
   L("ii") = 0;
   while (L("ii") < L("count"))
     {
	 L("cls") = L("clauses")[L("ii")];
   if (L("zone"))
     pnreplaceval(L("cls"),"zone",L("zone"));
     clauseregister(L("cls"),L("sent"));
     L("last np") = prevnp(L("sent"));
     clausetraverse(L("cls"),L("cls"),L("sent"));
     clauseresolve(L("cls"),L("sent"),L("last np"));
	 pnreplaceval(L("sent"),"clause",L("cls"));
	 ++L("ii");
	 }
   }
 }
else
  {
  # Get object, perhaps semantic case, from pp.
  # Can register to the current sentence at least.
  # Could check if before, betwixt, after clauses.
  if (pnvar(L("n"),"pp"))	# Prepositional phrase
    {
	# Register adverbial as a kind of nearly empty clause.
	advlregister(L("n"),L("sent"));
	
	# Handle non-clause adverbials.
	advltraverse(L("n"),L("sent"));
	}

  }

}

########
# FUNC:   SEMCLAUSE
# SUBJ:   Handle semantics of given clause.
# CR:     06/25/02 AM.
# INPUT:  
# RET:
# NOTE:
########

semclause(L("clause"),L("sent"))
{
if (!L("clause") || !L("sent"))
  return;
if (pnname(L("clause")) != "_clause")
  return;

if (G("verbose"))
  "dump.txt" << "[semclause: " << pnvar(L("clause"),"$text")
	<< "]\n";

# If clause has an immediately preceding np resolving it,
# check active-passive etc.
if (L("np") = pnvar(L("clause"),"np-that"))
  {
  if (G("verbose"))
    "dump.txt" << "np-that=" << pnvar(L("np"),"$text") << "\n";
  if (!pnvar(L("clause"),"passive")) # Assume passive noted here.
    {
    if (!pnvar(L("clause"),"actor"))
      {
	  # Resolve as actor.
	  pnreplaceval(L("clause"),"actor",L("np"));
	  }
	}
  else	# passive present.
    {
    if (!pnvar(L("clause"),"obj"))
      {
	  # Resolve as object.
	  pnreplaceval(L("clause"),"obj",L("np"));
	  }
	}
  }


# If clause has a preceding adverbial, throw it into a date/loc
# slot (or something like that).
L("advls") = pnvar(L("sent"),"advls");
if (L("advls"))	# 09/17/02 AM.
  {
  if (G("verbose"))
    "dump.txt" << "[Found previous advl.]" << "\n";
  L("advl") = L("advls")[0];	# Grab only the 1st, for now.
  L("loc") = pnvar(L("advl"),"pn nps");
  pnreplaceval(L("clause"),"loc",L("loc"));
  }

# If clause isn't registered as an event, register it.
L("kb concept") = pnvar(L("clause"),"kb concept");
if (!L("kb concept"))
  return;
L("event con") = resolveevent(L("kb concept"),G("events"));
if (!L("event con"))
  return; # Error.

# Add the pieces of the event.
if (L("act") = pnvar(L("clause"),"act"))
  {
  if (L("act con") = pnvar(L("act"),"kb act"))
    {
    if (L("res") = conval(L("act con"),"resolver"))
	  L("act con") = L("res");
	replaceval(L("event con"),"act",L("act con"));
	}
  }

if (L("actor") = pnvar(L("clause"),"actor"))
  {
  if (L("actor con") = pnvar(L("actor"),"kb obj"))
    {
    if (L("res") = conval(L("actor con"),"resolver"))
	  L("actor con") = L("res");
	replaceval(L("event con"),"actor",L("actor con"));
	}
  }

if (L("obj") = pnvar(L("clause"),"dobj"))
  {
  if (L("obj con") = pnvar(L("obj")[0],"kb obj"))
    {
    if (L("res") = conval(L("obj con"),"resolver"))
	  L("obj con") = L("res");
	replaceval(L("event con"),"obj",L("obj con"));
	}
  }

if (L("iobj") = pnvar(L("clause"),"iobj"))
  {
  if (L("iobj con") = pnvar(L("iobj")[0],"kb obj"))
    {
    if (L("res") = conval(L("iobj con"),"resolver"))
	  L("iobj con") = L("res");
	replaceval(L("event con"),"iobj",L("iobj con"));
	}
  }
if (L("adj") = pnvar(L("clause"),"adj role"))
  {
  if (G("verbose"))
    "dump.txt" << "adj= " << pnvar(L("adj"),"$text")
  	<<  "\n";
  if (L("state con") = pnvar(L("adj"),"kb con"))
    {
	# No resolver mechanism for attrs, states...
	replaceval(L("event con"),"state",L("state con"));
	}
  }
if (L("loc") = pnvar(L("clause"),"loc"))
  {
  if (L("loc con") = pnvar(L("loc"),"kb obj"))
    {
    if (L("res") = conval(L("loc con"),"resolver"))
	  L("loc con") = L("res");
	replaceval(L("event con"),"loc",L("loc con"));
	}
  }

}

########
# FUNC:   CLAUSERESOLVE
# SUBJ:   Resolve inter-clause references.
# CR:     07/23/02 AM.
# INPUT:  
# OUTPUT: 
########

clauseresolve(
	L("cnode"),	# Clause node.
	L("sent"),	# Current sentence.
	L("prev np")	# Previous np in sentence.
	)
{
if (!L("cnode") || !L("sent"))
  return;

# Get clause's kb concept.
L("kb concept") = pnvar(L("cnode"), "kb concept");
if (!L("kb concept"))
  return;

# Get event resolver.
L("event") = conval(L("kb concept"),"resolver");
if (!L("event"))
  return;

L("pattern") = pnvar(L("cnode"),"pattern");

if (L("pattern") == "ellipted-that-clause")
  {
  # Look for the immediately preceding np, structurally.
  if (G("verbose"))
    "dump.txt" << "ellipted-that-clause\n";
  
  # Get prev np in sent.
  if (!L("prev np"))
    return;
  if (G("verbose"))
    "dump.txt" << "clauseresolve: prev np=" << pnvar(L("prev np"),
  	"$text") << "\n";
  # Assume something like "np vg-passive"

  if (!(L("con") = pnvar(L("prev np"),"kb obj") ))
    return;
  if (!(L("res") = conval(L("con"),"resolver") ))
    return;
  replaceval(L("event"),"obj",L("res"));
  return;
  }

# Get prior clause from current sentence, if any.
if (!(L("cprev") = pnvar(L("sent"),"clause")))
  return;

# Get prev clause's kb concept.
L("kb cprev") = pnvar(L("cprev"), "kb concept");
if (!L("kb cprev"))
  return;

# Get event resolver.
L("prev event") = conval(L("kb cprev"),"resolver");
if (!L("prev event"))
  return;

if (G("verbose"))
"dump.txt" << "[clauseresolve: got prior clause "
	<< conceptname(L("kb cprev"))
	<< "]\n";

# Only handle "that-clause.
if (L("pattern") != "that-clause")
  return;

if (G("verbose"))
"dump.txt" << "[clauseresolve: got that-clause]" << "\n";

# If previous already has an object, can't fill it here.
if (conval(L("prev event"),"obj"))
  return;

# Fill previous clause's object with current clause.
replaceval(L("prev event"),"obj",L("event"));
}


########
# FUNC:   OBJECTREGISTER
# SUBJ:   Record object in semantics.
# CR:     06/24/02 AM.
# INPUT:  
# RET:	con - Object's concept.
# NOTE:
########

#objectregister(
#	L("n"),	# Object's node.
#	L("cnode")	# Object's clause.
#	)
#{
#if (!L("n") || !L("cnode"))
#  return 0;
#
#"dump.txt" << "[objectregister:]" << "\n";
#
# Tracking instances of "I".
#if (pnvar(L("n"),"stem") == "i")
#  ++G("1st person");
#
# Get clause's kb concept.
#L("kb concept") = pnvar(L("cnode"), "kb concept");
#if (!L("kb concept"))
#  return 0;
#
# Record actor or object.
#L("nobjs") = pnvar(L("cnode"),"nobjs");
#pnreplaceval(L("cnode"),"nobjs", ++L("nobjs"));
#L("con") = makeconcept(L("kb concept"),"obj" + str(L("nobjs")));
#replaceval(L("con"),"type","obj");
#replaceval(L("con"),"text",
#  strtolower(pnvar(L("n"),"$text")));
#if (L("sem") = pnvar(L("n"),"sem"))
#  replaceval(L("con"),"sem",strtolower(L("sem")));
#pnreplaceval(L("n"),"kb obj",L("con"));
#
#if (pnvar(L("n"),"eventive"))	# 07/10/02 AM.
#  replaceval(L("con"),"eventive",1);	# 07/10/02 AM.
#	
# Handle singleton.
#if (L("nsem") = pnvar(L("n"),"sem"))
#  replaceval(L("con"),"sem", L("nsem"));
#
#if (pnvar(L("n"),"pro"))	# If a pronoun.
#  replaceval(L("con"),"pro",1);
#
#domcopynodetocon(L("n"),L("con"));
#
# If a list of nps, handle it.
# (nps were collected in REVERSE order.)	# 07/10/02 AM.
#if (L("count") = pnvar(L("n"),"count"))
#  replaceval(L("con"),"count",L("count"));
#L("nps") = pnvar(L("n"),"nps");	# List of np nodes.
#L("num") = L("count");
#while (--L("num") >= 0)
#  {
#  # Register each object.
#  # Link to group concept also...
#  L("one") = L("nps")[L("num")];
#  L("obj") = objectregister(L("one"),L("cnode"));
#  
#  # Cross link.
#  addconval(L("con"),"cons",L("obj"));
#  replaceval(L("obj"),"group",L("con"));
#  }
#
#return L("con");
#}

########
# FUNC:   RESOLVEOBJECTS
# SUBJ:   Resolve object reference to objects in the text.
# CR:     07/10/02 AM.
# INPUT:  
# RET:	con - Resolving object.
# NOTE:		Requiring precise text compare, for now.
########

resolveobjects(
	L("ref"),		# An object reference.
	L("objects"),	# KB concept managing objects.
	L("events")		# KB concept managing events.
	)
{
if (!L("ref"))
  return 0;

if (G("verbose"))
  "dump.txt" << "[resolveobjects: ref="
	<< conceptname(L("ref"))
	<< "]\n";

# If not eventive...
# Resolve object with existing objects in the text.
if (numval(L("ref"),"pro"))
  L("ret") = resolvepro(L("ref"),L("objects"),L("events"));
else if (!numval(L("ref"),"eventive"))
  L("ret") = resolveobject(L("ref"),G("objects"));

# Else if eventive...
# Resolve with existing events in the text.	# 06/24/02 AM.
else
  L("ret") = resolveevent(L("ref"),L("events"));

L("count") = numval(L("ref"),"count");
if (L("count") <= 1)	# Single object.
  return L("ret");

L("num") = 0;
L("cons") = findvals(L("ref"),"cons");
while (L("num") < L("count"))
  {
  L("con") = getconval(L("cons"));
  if (numval(L("con"),"pro"))	# If a pronoun...
    resolvepro(L("con"),L("objects"),L("events"));
  else if (numval(L("con"),"eventive"))
    resolveevent(L("con"),L("events"));
  else
    resolveobject(L("con"),L("objects"));

  ++L("num");
  L("cons") = nextval(L("cons"));
  }
return L("ret");
}

########
# FUNC:   RESOLVEPRO
# SUBJ:   Resolve pronoun reference to objects in the text.
# CR:     07/24/02 AM.
# INPUT:  
# RET:	con - Resolving object.
# NOTE:	  May handle other anaphoric utterances, eventually.
########

resolvepro(
	L("ref"),		# An object reference.
	L("objects"),	# KB concept managing objects.
	L("events")		# KB concept managing events.
	)
{
if (!L("objects") || !L("events") || !L("ref"))
  return 0;

L("list") = down(L("objects"));
if (!L("list"))	# Empty objects list.
  {
  if (G("verbose"))
    "dump.txt" << "[resolvepro: No objects.]\n";
  # Couldn't resolve.
  # Could be something like "It is raining...."
  return 0;
  }

# Traverse the list of existing objects, looking for a mergable
# object.  (Todo: look at prior events, states also...)
L("merged") = 0;	# Not merged with an existing object.
L("done") = 0;
L("cand") = L("list");	# Candidate object.

# Go to end of the list.
while (next(L("cand")))
  L("cand") = next(L("cand"));

while (!L("done"))
  {
  # Trivial heuristic.  Look back to the nearest object
  # that doesn't conflict with person, plural, etc.
  # Examine individual pronouns I, we, you, etc....
  # Should use hierarchy concepts as part of the merge test...
  if (mergableobjs(L("ref"),L("cand")) )
    {
	if (G("verbose"))
      "dump.txt" << "[resolvepro: Trivial proximity heur, merge with "
		<< conceptname(L("cand"))
		<< "]\n";

	# Successful merge.
	L("merged") = 1;
	L("done") = 1;
	addconval(L("cand"),"refs",L("ref"));
    replaceval(L("ref"),"resolver",L("cand"));
	return L("cand");
	}
  
  L("cand") = prev(L("cand"));
  if (!L("cand"))
    L("done") = 1;
  }
}

if (!L("merged"))	# Didn't find an existing object.
  {
  return 0;
  }

########
# FUNC:   MERGABLEOBJS
# SUBJ:   See if object reference can be merged with candidate.
# CR:     08/01/02 AM.
# INPUT:  
# RET:	bool - 1 if mergable, else 0.
# NOTE:
########

mergableobjs(L("ref"),	# Object reference.
	L("cand"))	# Object in kb list.
{
if (!L("ref") || !L("cand"))
  return 0;

L("sem") = strval(L("cand"),"sem");

# Don't merge with dates, for now!
if (L("sem") == "date")
  return 0;

# Don't merge with locations for now!
# Todo: better get of location semantics.
if (L("sem") == "country")
  return 0;

# Todo: Put more tests here, like plural, etc.

return 1;
}


########
# FUNC:   STATEREGISTER
# SUBJ:   Record state in semantics.
# CR:     07/18/02 AM.
# INPUT:  
# RET:	con - state's concept.
# NOTE:
########

stateregister(
	L("n"),     # State node.
	L("cnode")	# Clause.
	)
{
if (!L("n") || !L("cnode"))
  return 0;

# Get clause's kb concept.
L("kb concept") = pnvar(L("cnode"), "kb concept");
if (!L("kb concept"))
  return 0;

# Record actor or object.
L("nstates") = pnvar(L("cnode"),"nstates");
pnreplaceval(L("cnode"),"nstates", ++L("nstates"));
L("con") = makeconcept(L("kb concept"),"state" + str(L("nstates")));
replaceval(L("con"),"type","state");
replaceval(L("con"),"text",
  strtolower(pnvar(L("n"),"$text")));
pnreplaceval(L("n"),"kb con",L("con"));
	
# Handle singleton.
if (L("nsem") = pnvar(L("n"),"sem"))
  replaceval(L("con"),"sem", L("nsem"));

return L("con");
}


########
# FUNC:   RESOLVEEVENT
# SUBJ:   Resolve eventive np reference to events in the text.
# CR:     05/28/02 AM.
# INPUT:  
# RETURN:	event_con - Resolved event concept.
# NOTE:		Requiring precise text compare, for now.
########

resolveevent(
	L("ref"),		# An eventive np reference.
	L("events")		# KB concept managing events in current text.
	)
{
if (!L("events") || !L("ref"))
  return 0;

L("list") = down(L("events"));
if (!L("list"))	# Empty events list.
  {
  # Just add the reference and done.
  return newevent(L("ref"),L("events"));
  }

# Traverse the list of existing events, looking for a mergable
# event.  For now, requiring exact text match.
L("merged") = 0;	# Not merged with an existing event.
L("done") = 0;
L("cand") = L("list");	# Candidate object.
L("rtext") = strval(L("ref"),"text");
while (!L("done"))
  {
  # Should use hierarchy concepts as part of the merge test...
#  "dump.txt" << "[resolveevent: cand="
#  	<< strval(L("cand"),"text")
#	<< " ref=" << L("rtext") << "]\n";
  if (L("rtext")
   && strval(L("cand"),"text") == L("rtext") )
    {
	# Successful merge.
	L("merged") = 1;
	L("done") = 1;
	addconval(L("cand"),"refs",L("ref"));
    replaceval(L("ref"),"resolver",L("cand"));
	dommergeevent(L("ref"),L("cand"));
	return L("cand");
	}
  
  L("cand") = next(L("cand"));
  if (!L("cand"))
    L("done") = 1;
  }

# Didn't find an existing object.
# Just add the reference and done.
return newevent(L("ref"),L("events"));
}

########
# FUNC:   NEWEVENT
# SUBJ:   Register a new (unmergeable) event in semantics.
# CR:     06/25/02 AM.
# INPUT:  
# OUTPUT: 
########

newevent(
   L("ref"),	# Reference concept.
   L("events")  # Kb list of event concepts to update.
   )
{
L("ct") = numval(L("events"),"count");
replaceval(L("events"),"count", ++L("ct"));	# inc count.
L("nm") = "event" + str(L("ct"));
if (G("verbose"))
  "dump.txt" << "[new event: " << L("nm") << "]\n";
L("con") = makeconcept(L("events"),L("nm"));
replaceval(L("con"),"type","event");
L("rtext") = strval(L("ref"),"text");
if (L("rtext"))
  replaceval(L("con"),"text",L("rtext"));

addconval(L("con"),"refs",L("ref")); # Point to ref concept.
replaceval(L("ref"),"resolver",L("con"));

# Application-specific fixups.
domnewevent(L("ref"),L("con"));	# 09/18/02 AM.

return L("con");
}


########
# FUNC:   COMPLEXTRAVERSE
# SUBJ:   Manually traverse a complex clause in the parse tree.
# CR:     05/28/02 AM.
# INPUT:  
# OUTPUT: 
########

complextraverse(
	L("cxnode"),	# Complex node.
	L("snode")		# Sentence node.
	)
{
if (!L("cxnode") || !L("snode"))
  return;

L("n") = pndown(L("cxnode"));
while (L("n"))
  {
  if (pnname(L("n")) == "_clause")
    {
    "clauses.txt" << pnvar(L("n"),"$text") << "\n\n";
	L("last np") = prevnp(L("snode"));	# 09/22/02 AM.
	pnreplaceval(L("n"),"zone",pnvar(L("cxnode"),"zone"));
	clauseregister(L("n"),L("snode"));
    clausetraverse(L("n"),L("n"),L("snode"));
	clauseresolve(L("n"),L("snode"),L("last np"));
	}
  L("n") = pnnext(L("n"));
  }
}


@CODE
L("hello") = 0;
@@CODE
