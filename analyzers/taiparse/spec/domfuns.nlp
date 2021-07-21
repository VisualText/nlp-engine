###############################################
# FILE: domfuns.pat
# SUBJ: Functions built for specific domain and task.
# AUTH: AM
# MODIFIED:
###############################################

@DECL
########
# FUNC:	REGISTERX
# SUBJ:	Add string to a node's variable.
# EX:	register(X(4),"temporal","1/26/05");
#	Adds a date string to "temporal" var of node
#	in context.
# NOTE:	For adding multiple values to a variable.
#	Should rename this to something else.
#	Lowercase register() conflicts with C++ reserved word,
#	when analyzer is compiled.
########
registerx(
	L("node"),	# Parse tree node we are adding info to.
	L("field"),
	L("str")
	)
{
if (!L("node") || !L("field") || !L("str"))
  return;
L("vals") = pnvar(L("node"),L("field"));
if (!L("vals"))
  L("len") = 0;
else
  L("len") = arraylength(L("vals"));

# Can't directly append a new value onto node.
# Need something like pnaddval(L("node"),L("field"),L("str")).
L("vals")[L("len")] = L("str");
pnreplaceval(L("node"),L("field"),L("vals"));
}

domadvl(L("n"),L("sent"))
{
}

domcopyattrs(L("ref"),L("con"))
{
}

domcopynodetocon(L("n"),L("con"))
{
}

dommergeevent(L("ref"),L("cand"))
{
}

domnewevent(L("ref"),L("con"))
{
}

domnp(L("n"),L("sent"))
{
}

########
# FUNC:	DOMNPOFNP
# SUBJ:	Try to categorize named entity.
########
domnpofnp(
	L("n"),	# Reduced np.
	L("np1"),	# First np.
	L("np2"))	# of-np.
{
if (!pnvar(L("np2"),"ne") || pnvar(L("np2"),"ne type"))
  return;
if (pnvar(L("np1"),"ne") || !pnvar(L("np1"),"sem"))
  return;

L("sem1") = pnvar(L("np1"),"sem");
if (L("sem1") == "city")
  pnreplaceval(L("np2"),"ne type","location");
}

########
# FUNC:	DOMOBJECTREGISTER
# SUBJ:	Domain- and task-specific register of object.
# NOTE: Domain functions calling generic functions.
#	The reverse of previous implementations in TAIParse.
#	This lets us do domain and task stuff as we like.
#	(One could go domain-to-generic or generic-to-domain, etc.)
########
domobjectregister(
	L("n"),		# Object's pnode. eg, an _np node.
	L("cnode")	# Object's clause pnode.
	)
{

# Generic register of object reference in kb.
L("obj") = objectregister(L("n"),L("cnode"));
if (!L("obj"))
  return 0;

# Generic resolve of object reference against list of objects
# in the kb.
L("object") = resolveobject(L("obj"),G("objects"));

# Entities.
# If the object has an array of entities, use that.
# nenoderange(
L("arr") = pnvar(L("n"),"ne arr");
if (L("arr"))
  {
  L("ii") = 0;
  while (L("arr")[L("ii")])
    {
	L("x") = L("arr")[L("ii")];
    L("ent") = entregister(L("x"),L("n"));
    L("entity") = resolveentity(L("ent"),G("entities"));

	++L("ii");
	}
  return L("obj");
  }

# Else if the object itself is a named entity, use that.
L("ent") = entregister(L("n"),L("n"));
L("entity") = resolveentity(L("ent"),G("entities"));

return L("obj");
}

########
# FUNC:	DOMENTITYREGISTER
# SUBJ:	Domain- and task-specific register of entity.
########
domentityregister(
	L("n"),		# Object's pnode. eg, an _np node.
	L("cnode")	# Object's clause pnode.
	)
{
if (!L("n"))
  return 0;

# Entities.
# If the object has an array of entities, use that.
# nenoderange(
L("arr") = pnvar(L("n"),"ne arr");
if (L("arr"))
  {
  L("ii") = 0;
  while (L("arr")[L("ii")])
    {
	L("x") = L("arr")[L("ii")];
    L("ent") = entregister(L("x"),L("n"));
    L("entity") = resolveentity(L("ent"),G("entities"));

	++L("ii");
	}
  return L("entity");  # What if an array? ...
  }

# Else if the object itself is a named entity, use that.
L("ent") = entregister(L("n"),L("n"));
L("entity") = resolveentity(L("ent"),G("entities"));

return L("entity");
}

@CODE
L("hello") = 0;
@@CODE
