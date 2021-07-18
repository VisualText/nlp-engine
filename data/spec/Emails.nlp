###############################################
# FILE: Emails.pat                            #
# SUBJ: A very simple email recognizer        #
# AUTH: David de Hilster                      #
# CREATED: 20/Jan/00 09:09:01                 #
# Date of this version: 31/Aug/01             #
# Copyright Text Analysis International       #
###############################################

@NODES _ROOT

@RULES

_Email <-
	_xWILD [match=(_xALPHA \. _xNUM \_)]
	\@ [s]
	_xWILD [match=(_xALPHA _xNUM \_)]
	\.
	_xWILD [match=(_xALPHA \. _xNUM \_)]
	@@


