###############################################
# FILE: decl.pat
# SUBJ: Collect declaration region.
# AUTH: AM
# CREATED: 19/Dec/01 16:04:34
# MODIFIED:
###############################################

@PATH _ROOT _DECL _NLPPP

@POST
	rfbdecls(1)
	single()
@RULES
_DECL [base] <- _FUNCDEF [plus] @@

# 09/10/03 AM.
# Ignore ok things within a mangled function body.

@RULES
_ERROR <-
	_FNCALL
	\{
	_STMTS [star]
	@@
_ERROR <- _STMTS [plus] @@

# 09/10/03 AM.
# Will find only first error in a function.
# Better than nothing.
@POST
  rfberror(1)
  single()
@RULES
_ERROR <- _xANY [plus] @@

