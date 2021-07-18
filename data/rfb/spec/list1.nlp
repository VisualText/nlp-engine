###############################################
# FILE: list1.pat
# SUBJ: For executing in NLP++ regions.
# AUTH: Amnon Meyers
# CREATED: 18/Dec/99 17:31:42
# MODIFIED:
# NOTE:	Code-like regions get parsed differently from rule
#		regions.
###############################################

@NODES _NLPPP

# 02/16/00 AM. Overhauling function call grammar.
# @RECURSE listarg

# @POST
#	rfbarg(1)			# Note this difference.
#	single()
# @RULES
# _ARG [base] <- _NONLIT @@
# _ARG [base] <- _LIT @@
# _ARG [base] <- _STR @@
# _ARG [base] <- _NUM @@

# @@RECURSE listarg


# @POST
#	rfalist(2)
#	single()
# @RULES
# _LIST [base] <- \( _xWILD [match=(_LIT _NONLIT _STR _NUM) recurse=(listarg)] \) @@

@POST
	rfarange(3, 5)
	singler(2,6)
@RULES
_PREPAIR [base] <-
	\;	# Disambiguating context.
	\< _NUM \, _NUM \> @@

_PREPAIR [base] <-
	_xSTART	# Disambiguating context.
	\< _NUM \, _NUM \> @@
