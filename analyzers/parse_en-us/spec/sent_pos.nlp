###############################################
# FILE: sent_pos.pat
# SUBJ: Default pos tagging for sents.
# AUTH: AM
# CREATED: 30/Jun/06 16:03:49
# MODIFIED:
###############################################

@CODE
L("hello") = 0;
@@CODE

@NODES _sent

# so
# while
# "so" hasn't been grabbed up within a clause.
# Assume it's a conjunction.
#@CHECK
#  if (N("mypos"))
#    fail();
#@POST
#  chpos(N(1),"IN");
#@RULES
#_xNIL <- _xWILD [s one match=(
#	so
#	while
#	)] @@

# before
# coordinating conjunction.
@CHECK
  if (N("mypos"))
    fail();
  if (pnname(N(1)) != "_fnword")
    fail();
@POST
  chpos(N(1),"IN");	# before/IN. DEFAULT.
@RULES
_xNIL <-
	before [s]
	@@

@CHECK
  if (N("mypos"))
    fail();
  if (pnname(N(1)) != "_fnword")
    fail();
@POST
  chpos(N(1),"IN");	# except/IN. DEFAULT.
@RULES
_xNIL <-
	except [s]
	@@

# what
# whatever
@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"WP");
@RULES
_xNIL <- _xWILD [s one match=(
	what
	whatever
	)] @@

# both.
@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"DT");
@RULES
_xNIL <- _xWILD [s one match=(
	both
	)] @@

@CHECK
  if (N("mypos"))
    fail();
  if (pnname(N(1)) != "_fnword")
    fail();
@POST
  chpos(N(1),"IN");	# that/IN. DEFAULT.
@RULES
_xNIL <- that [s] @@

@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"JJR");	# more/JJR. DEFAULT.
@RULES
_xNIL <- more [s] @@

@CHECK
  if (N("mypos"))
    fail();
@POST
  chpos(N(1),"CC");	# yet/CC. DEFAULT.
@RULES
_xNIL <- yet [s] @@

