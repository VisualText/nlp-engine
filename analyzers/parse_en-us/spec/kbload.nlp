###############################################
# FILE: kbload.pat
# SUBJ: Throw some knowledge into kb.
# AUTH: AM
# CREATED: 15/Jan/05 13:06:30
# MODIFIED:
###############################################

@DECL
########
# FUNC:	DICTATTR
# SUBJ:	Add attribute for dictionary word.
########
dictattr(L("str"),L("attr"),L("val"))
{
if (!L("str") || !L("attr"))
  return;
L("str") = strtolower(L("str"));
L("con") = dictgetword(L("str"));
replaceval(L("con"),L("attr"),L("val"));
}

########
# FUNC:	FINDDICTATTR
# SUBJ:	Add attribute for dictionary word.
# RET:	Numeric-valued attr.
########
#finddictattr(L("str"),L("attr"))
#{
#if (!L("str") || !L("attr"))
#  return 0;
#L("str") = strtolower(L("str"));
#L("con") = dictgetword(L("str"));
#return numval(L("con"),L("attr"));
#}
finddictattr(L("str"),L("attr"))
{
if (!L("str") || !L("attr"))
  return 0;
L("str") = strtolower(L("str"));
L("con") = dictgetword(L("str"));
if (!L("con"))
  {
  "err.txt" << "finddictattr: " << L("str") << ", " << L("attr") << "\n";
  return 0;
  }
return numval(L("con"),L("attr"));
}

########
# FUNC:	FINDDICTSATTR
# SUBJ:	Add attribute for dictionary word.
# RET:	String-valued attr.
########
finddictsattr(L("str"),L("attr"))
{
if (!L("str") || !L("attr"))
  return;
L("str") = strtolower(L("str"));
L("con") = dictgetword(L("str"));
return strval(L("con"),L("attr"));
}

@CODE
if (numval(findroot(),"kb loaded"))
  exitpass();

########################################
# Verbs that require a human-like agent.
########################################
# eg, "Moose said" -- Moose would be an NP.
dictattr("announce","agent-human",1);
dictattr("say","agent-human",1);

########################################
# Verbs that take scalar change.
########################################
# Note: Take a numeric value after the verb.
# eg, "rise 10%".
# These verbs appear to allow intransitive use.
dictattr("accelerate","vscalenum",1);
dictattr("climb","vscalenum",1);
dictattr("decrease","vscalenum",1);
dictattr("fall","vscalenum",1);
dictattr("gain","vscalenum",1);
dictattr("grow","vscalenum",1);
dictattr("increase","vscalenum",1);
dictattr("lengthen","vscalenum",1);
dictattr("lose","vscalenum",1);
dictattr("rise","vscalenum",1);
dictattr("shrink","vscalenum",1);

########################################
# SEMANTIC VERB FEATURES
########################################
# Ex: The verb sense of 'curb' means diminish in some way.
# vscalar - scalar in verb sense.
# nscalar - scalar in noun sense.
# (These may need further work for ambiguous nouns etc.)
# Note: Sometimes verb and noun will have the scalar sense,
# so doesn't always resolve part-of-speech.
# Appear to be mainly transitive verbs here.

# vscalar = 0, minus, plus, neutral.
dictattr("accelerate","vscalar","plus");
dictattr("bolster","vscalar","plus");
dictattr("boost","vscalar","plus");
dictattr("brake","vscalar","minus");
dictattr("curb","vscalar","minus");
dictattr("decelerate","vscalar","minus");
dictattr("decrease","vscalar","minus");
dictattr("decrement","vscalenum",1);
dictattr("deflate","vscalar","minus");
dictattr("diminish","vscalar","minus");
dictattr("enhance","vscalar","plus");
dictattr("increase","vscalar","plus");
dictattr("increment","vscalenum",1);
dictattr("inflate","vscalar","plus");
dictattr("lessen","vscalar","minus");
dictattr("lower","vscalar","minus");
dictattr("maintain","vscalar","neutral");
dictattr("raise","vscalar","plus");
dictattr("reduce","vscalar","minus");
dictattr("slow","vscalar","minus");
dictattr("speed","vscalar","plus");


########################################
# Plain adjectives with comparative, superlative endings.
########################################
# not-er, not-est, plain-adj.
# Note: number is ambiguous.  More numb, and the numeric meaning.
dictattr("bitter","adj-plain",1);
dictattr("center","adj-plain",1);
dictattr("charter","adj-plain",1);
dictattr("clever","adj-plain",1);
dictattr("counter","adj-plain",1);
dictattr("eager","adj-plain",1);
dictattr("former","adj-plain",1);
dictattr("foster","adj-plain",1);
dictattr("improper","adj-plain",1);
dictattr("inner","adj-plain",1);
dictattr("lackluster","adj-plain",1);
dictattr("later","adj-plain",1);	# Conform Treebank.
dictattr("latter","adj-plain",1);
dictattr("meager","adj-plain",1);
dictattr("other","adj-plain",1);
dictattr("power","adj-plain",1);
dictattr("proper","adj-plain",1);
dictattr("quarter","adj-plain",1);
dictattr("silver","adj-plain",1);
dictattr("sinister","adj-plain",1);
dictattr("sister","adj-plain",1);
dictattr("tender","adj-plain",1);
dictattr("test","adj-plain",1);
dictattr("together","adj-plain",1);	# 04/23/07 AM.
dictattr("undercover","adj-plain",1);
dictattr("upper","adj-plain",1);
dictattr("utter","adj-plain",1);
dictattr("west","adj-plain",1);
dictattr("winter","adj-plain",1);

########################################
# Plain adverbs with comparative, superlative endings.
########################################
dictattr("asunder","adv-plain",1);
dictattr("counter","adv-plain",1);
dictattr("forever","adv-plain",1);
dictattr("hither","adv-plain",1);
dictattr("however","adv-plain",1);

# Note: Treebank tagguide says RB unless explicitly
# something like "X is later than Y", in which case it's
# RBR.  Treebank answer key does not conform to this enough,
# so using RB for simplicity here.  More semantic, clause,
# and sentential processing needed to assign RBR appropriately
# according to this scheme.	# 04/19/07 AM.
dictattr("later","adv-plain",1);	# Conform Treebank. # 04/19/07 AM.

dictattr("moreover","adv-plain",1);
dictattr("never","adv-plain",1);
dictattr("rather","adv-plain",1);
dictattr("together","adv-plain",1);
dictattr("thereafter","adv-plain",1);

########################################
# Exceptions.
# Adverbs, adjectives with comparative, superlative endings.
########################################

dictattr("later","-er",1);	# Note the fact.

dictattr("best","-est",1);
dictattr("least","-est",1);
dictattr("most","-est",1);
dictattr("worst","-est",1);

dictattr("less","-er",1);
dictattr("more","-er",1);
dictattr("worse","-er",1);

########################################
# THAT-SUBJUNCTIVE
########################################
# Note: Some nouns and verbs yield a "that + subjunctive"
# pattern.
# He demands that John eat his dinner.
# (As opposed to 'He thinks that John eats his dinner.')
# 'eventive' nouns also:
# His demand that John eat his dinner ...
# Also multi-word:  see to it that ...

dictattr("ask","that-subjunctive",1);
dictattr("assure","that-subjunctive",1);
dictattr("beg","that-subjunctive",1);
dictattr("command","that-subjunctive",1);
dictattr("decide","that-subjunctive",1);
dictattr("decree","that-subjunctive",1);
dictattr("demand","that-subjunctive",1);
dictattr("ensure","that-subjunctive",1);
dictattr("insist","that-subjunctive",1);
dictattr("order","that-subjunctive",1);
dictattr("request","that-subjunctive",1);
dictattr("see","that-subjunctive",1);
dictattr("vote","that-subjunctive",1);

dictattr("assurance","that-subjunctive",1);
dictattr("decision","that-subjunctive",1);
dictattr("insistence","that-subjunctive",1);


########################################
# VERB + OF
########################################
dictattr("think","verb-of",1);
dictattr("tire","verb-of",1);
dictattr("require","verb-of",1);
# Also, require NP of ...


########################################
# COPULAS
########################################
dictattr("appear","copula",1);
dictattr("be","copula",1);
dictattr("being","copula",1);
dictattr("become","copula",1);
dictattr("close","copula",1);	# market closed higher.
dictattr("feel","copula",1);
dictattr("get","copula",1);	# 5/31/06 AM.
dictattr("go","copula",1);
dictattr("grow","copula",1);
dictattr("lie","copula",1);
dictattr("look","copula",1);
dictattr("make","copula",1);
dictattr("mean","copula",1);	# that means cloudy...
dictattr("open","copula",1);	# market opened higher.
dictattr("prove","copula",1);
dictattr("remain","copula",1);
dictattr("sound","copula",1);
dictattr("stay","copula",1);
dictattr("turn","copula",1);

########################################
# TRANSITIVE VERBS
########################################
dictattr("purpose","trans",1);
dictattr("purposes","trans",1);
dictattr("purposing","trans",1);
dictattr("purposed","trans",1);

dictattr("ambition","trans",1);
dictattr("ambitions","trans",1);
dictattr("ambitioning","trans",1);
dictattr("ambitioned","trans",1);

dictattr("issue","trans",1);
dictattr("issues","trans",1);
dictattr("issuing","trans",1);
dictattr("issued","trans",1);

dictattr("link","trans",1);
dictattr("links","trans",1);
dictattr("linking","trans",1);
dictattr("linked","trans",1);

########################################
# INTRANSITIVE VERBS
########################################
# Todo: distinguish semantically and syntactically intransitive.
# (eg, "I think" implies that one is thinking about something.
#  i.e., syntax=intr, sem=trans)
dictattr("grow","intr",1);
dictattr("sit","intr",1);
dictattr("struggle","intr",1);
dictattr("walk","intr",1);

########################################
# VNJ VERBS
########################################
# Ex: keep it real.
# Ex: paint the house purple.
# Corresponds to Longman X7.
dictattr("hold","X7",1);
dictattr("keep","X7",1);
dictattr("leave","X7",1);
dictattr("make","X7",1);
dictattr("paint","X7",1);
dictattr("serve","X7",1);

########################################
# VN(prep)J VERBS
########################################
# C1pr is another Longman designation.
# Also similar to X7.
# Ex: John viewed Mary as ditzy.
# Ex: John considered Mary (as) ditzy. (optional 'as')

# i - 'as' is optional.
dictattr("appoint","C1pr",1);	# 1
dictattr("certify","C1pr",1);
dictattr("choose","C1pr",1);
dictattr("consider","C1pr",1);
dictattr("count","C1pr",1);
dictattr("crown","C1pr",1);		# 1
dictattr("deem","C1pr",1);
dictattr("elect","C1pr",1);		# 1
dictattr("esteem","C1pr",1);	# 1
dictattr("make","C1pr",1);		# 1
dictattr("proclaim","C1pr",1);
dictattr("rate","C1pr",1);
dictattr("reckon","C1pr",1);
dictattr("report","C1pr",1);

# ii - 'as' is obligatory.
dictattr("accept","C1pr",1);
dictattr("acknowledge","C1pr",1);
dictattr("characterize","C1pr",1);
dictattr("class","C1pr",1);
dictattr("define","C1pr",1);
dictattr("describe","C1pr",1);
dictattr("intend","C1pr",1);	# 1
dictattr("make","C1pr",1);
dictattr("regard","C1pr",1);
dictattr("see","C1pr",1);
dictattr("take","C1pr",1);
dictattr("treat","C1pr",1);
dictattr("use","C1pr",1);
dictattr("view","C1pr",1);

# take for - also C1pr

########################################
# VNN VERBS
########################################
# Ex: give John an apple.
# Like Longman D1.
#dictattr("paint","vnn",1);

########################################
# HELPING VERBS
########################################
# The idea here was things like:
# to help work
# to (begin, get, commence, start) working
# ...
# Need a better description, at least two types.

# T2
dictattr("dare","T2",1);
dictattr("help","T2",1);

# And I3 is what?
# T3 = V + TO + V
dictattr("agree","T3",1);
dictattr("ask","T3",1);
dictattr("attempt","T3",1);
dictattr("claim","T3",1);
dictattr("decide","T3",1);
dictattr("deserve","T3",1);
dictattr("desire","T3",1);
dictattr("expect","T3",1);
dictattr("fail","T3",1);	#
dictattr("go","T3",1);
dictattr("have","T3",1);
dictattr("hope","T3",1);
dictattr("manage","T3",1);	#
dictattr("need","T3",1);
dictattr("promise","T3",1);
dictattr("strive","T3",1);
dictattr("struggle","T3",1);
dictattr("try","T3",1);
dictattr("use","T3",1);	# passive: used to v-inf
dictattr("wait","T3",1);
dictattr("want","T3",1);
dictattr("work","T3",1);

# T4 = verb + ving (start eating)
# Also: cannot stand ving.
# Also: keep (on) ving.
dictattr("acknowledge","T4",1);
dictattr("avoid","T4",1);
dictattr("consider","T4",1);
dictattr("deny","T4",1);
dictattr("detest","T4",1);
dictattr("dislike","T4",1);
dictattr("evaluate","T4",1);	#
dictattr("fancy","T4",1);
dictattr("finish","T4",1);
dictattr("keep","T4",1);
dictattr("postpone","T4",1);
dictattr("resent","T4",1);
dictattr("resist","T4",1);
dictattr("risk","T4",1);
dictattr("suggest","T4",1);

dictattr("begin","T4",1);
dictattr("chance","T4",1);	# ?
dictattr("commence","T4",1);
dictattr("continue","T4",1);
dictattr("eschew","T4",1);	#
dictattr("get","T4",1);
dictattr("resist","T4",1);	#
dictattr("start","T4",1);	#
dictattr("stop","T4",1);
dictattr("try","T4",1);

########################################
# VNV VERBS
########################################
# Longman V2.
# Ex: make them go.

dictattr("bid","V2",1);	# archaic?
dictattr("have","V2",1);
dictattr("help","V2",1);
dictattr("let","V2",1);
dictattr("make","V2",1);
dictattr("see","V2",1);
dictattr("watch","V2",1);

########################################
# V+N+TO+V VERBS
########################################
# Longman V3.
# Ex: help them to go.
dictattr("allow","V3",1);
dictattr("ask","V3",1);
dictattr("coax","V3",1);
dictattr("command","V3",1);
dictattr("convince","V3",1);
dictattr("force","V3",1);
dictattr("help","V3",1);
dictattr("intend","V3",1);
dictattr("like","V2",1);	# a little different...
dictattr("order","V3",1);
dictattr("permit","V3",1);
dictattr("prefer","V3",1);	#
dictattr("pressure","V3",1);
dictattr("require","V3",1);
dictattr("teach","V3",1);
dictattr("tell","V3",1);
dictattr("urge","V3",1);

########################################
# V+N+V-ing VERBS
########################################
# Longman V4.
dictattr("catch","V4",1);
dictattr("discover","V4",1);
dictattr("keep","V4",1);
dictattr("feel","V4",1);
dictattr("find","V4",1);
dictattr("get","V4",1);	# get him going.
dictattr("hate","V4",1);
dictattr("have","V4",1);	# 05/22/07 AM.
dictattr("hear","V4",1);
dictattr("like","V4",1);
dictattr("notice","V4",1);
dictattr("observe","V4",1);
dictattr("see","V4",1);
dictattr("send","V4",1);	# send him packing/running...
dictattr("sense","V4",1);
dictattr("smell","V4",1);
dictattr("start","V4",1);
dictattr("want","V4",1);

########################################
# V+N+V-en VERBS
########################################
# Longman V8.
# 04/23/07 AM.
dictattr("get","V8",1);
dictattr("have","V8",1);
dictattr("need","V8",1);
dictattr("want","V8",1);

########################################
# V+THAT VERBS
########################################
# Longman T5 and D5 verbs, with subcategories.
# Also:
# It seemed/appeared/transpired/happened that ...
# Correspondence with Schank MBUILD, MTRANS.

# T5a list.
dictattr("accept","T5",1);
dictattr("admit","T5",1);
dictattr("agree","T5",1);
dictattr("believe","T5",1);
dictattr("claim","T5",1);
dictattr("conclude","T5",1);
dictattr("discover","T5",1);
dictattr("doubt","T5",1);
dictattr("feel","T5",1);
dictattr("figure","T5",1);
dictattr("forget","T5",1);
dictattr("hope","T5",1);
dictattr("indicate","T5",1);
dictattr("remark","T5",1);
dictattr("understand","T5",1);

# T5c list.
dictattr("ask","T5",1);
dictattr("command","T5",1);
dictattr("demand","T5",1);
dictattr("insist","T5",1);
dictattr("order","T5",1);
dictattr("propose","T5",1);
dictattr("recommend","T5",1);
dictattr("request","T5",1);
dictattr("require","T5",1);
dictattr("suggest","T5",1);
dictattr("urge","T5",1);

# Others.
dictattr("add","T5",1);
dictattr("announce","T5",1);
dictattr("assert","T5",1);
dictattr("assure","T5",1);
dictattr("complain","T5",1);
dictattr("concede","T5",1);
dictattr("concur","T5",1);
dictattr("demonstrate","T5",1);
dictattr("determine","T5",1);
dictattr("emphasize","T5",1);
dictattr("find","T5",1);
dictattr("highlight","T5",1);
dictattr("illustrate","T5",1);
dictattr("imply","T5",1);
dictattr("infer","T5",1);
dictattr("mention","T5",1);
dictattr("note","T5",1);
dictattr("prove","T5",1);
dictattr("reveal","T5",1);
dictattr("say","T5",1);
dictattr("show","T5",1);
dictattr("stress","T5",1);
dictattr("underscore","T5",1);

dictattr("decide","T5",1);	#
dictattr("charge","T5",1);	#

########################################
# V+O+O VERBS
########################################
# Longman D1 verbs.
# John gave Mary a book.

# D1 (to)
dictattr("bring","D1",1);
dictattr("call","D1",1);	#
dictattr("give","D1",1);	#
dictattr("grant","D1",1);
dictattr("hand","D1",1);
dictattr("make","D1",1);	#
dictattr("offer","D1",1);
dictattr("owe","D1",1);
dictattr("promise","D1",1);
dictattr("read","D1",1);
dictattr("refuse","D1",1);	# Not exactly 'to'.
dictattr("send","D1",1);
dictattr("show","D1",1);
dictattr("take","D1",1);
dictattr("teach","D1",1);
dictattr("write","D1",1);

# D1 (for)
dictattr("cook","D1",1);
dictattr("find","D1",1);
dictattr("get","D1",1);
dictattr("leave","D1",1);
dictattr("make","D1",1);
dictattr("order","D1",1);
dictattr("peel","D1",1);
dictattr("save","D1",1);
dictattr("spare","D1",1);

# D1a
dictattr("ask","D1",1);
dictattr("strike","D1",1);

########################################
# AMBIG THAT STRONGLY PREFERS VERB
########################################
# Ie, need evidence for other POS.
dictattr("making","prefer-verb",1);

dictattr("wrong","prefer-adj",1);

########################################
# PREPOSITIONAL VERB IDIOMS
########################################
# "roll out"
# 0 = no info.
# 1 = PHRASAL, ie, verb+particle.
# 2 = prepositional.
# 3 = Both are valid.
# Note that sometimes they are ambiguous...intrans vs trans...
dictattr("act","out",1);
dictattr("add","up",1);
dictattr("auction","off",1);
dictattr("back","down",1);
dictattr("back","out",1);
dictattr("back","up",1);
dictattr("bail","out",1);
dictattr("balance","out",1);
dictattr("bang","up",1);
dictattr("bark","out",1);	# ?
# Need a way to say 'base X on' only.
# which he will base on...
# based [X] on
dictattr("base","on",1);	#
dictattr("bat","down",1);
dictattr("bear","out",1);
dictattr("beat","down",1);
dictattr("beat","off",1);
dictattr("beat","out",1);
dictattr("beat","up",1);
dictattr("bed","down",1);
dictattr("beef","up",1);
dictattr("bid","up",1);
dictattr("bite","off",1);
dictattr("black","out",1);
dictattr("blow","off",1);
dictattr("blow","out",1);
dictattr("blow","up",1);
dictattr("blurt","out",1);
dictattr("bog","down",1);
dictattr("boil","down",1);
dictattr("boot","off",1);
dictattr("bottle","up",1);
dictattr("bottom","out",1);
dictattr("bounce","up",1);	# ?
dictattr("box","out",1);
dictattr("branch","out",1);
dictattr("break","down",1);
dictattr("break","off",1);	# 2?
dictattr("break","out",1);
dictattr("break","up",1);
dictattr("bring","down",1);
dictattr("bring","out",1);
dictattr("bring","up",1);
dictattr("brush","off",1);
dictattr("buck","up",1);
dictattr("buckle","down",1);
dictattr("buckle","up",1);
dictattr("build","up",1);
dictattr("bump","up",1);
dictattr("burn","down",1);
dictattr("burn","out",1);
dictattr("bust","down",1);	# ?
dictattr("button","down",1);
dictattr("button","up",1);
dictattr("buy","off",1);
dictattr("buy","out",1);
dictattr("buy","up",1);
dictattr("call","off",1);
dictattr("call","out",1);
dictattr("call","up",1);
dictattr("calm","down",1);
dictattr("camp","out",1);
dictattr("cancel","out",1);	# ?
dictattr("cap","off",1);
dictattr("carry","off",1);
dictattr("carry","out",1);
dictattr("cart","off",1);
dictattr("cart","out",1);
dictattr("carve","out",1);
dictattr("carve","up",1);
dictattr("cash","out",1);
dictattr("cast","about",1);	# ?
dictattr("cast","out",1);
dictattr("chain","up",1);
dictattr("chalk","up",1);
dictattr("check","out",1);
dictattr("cheer","up",1);
dictattr("chew","up",1);
dictattr("choke","down",1);
dictattr("choke","off",1);
dictattr("choke","up",1);
dictattr("chomp","up",1);
dictattr("chop","down",1);
dictattr("chop","off",1);
dictattr("chew","off",1);
dictattr("churn","out",1);
dictattr("clamp","down",1);
dictattr("clean","out",1);
dictattr("clean","up",1);
dictattr("clear","out",1);
dictattr("clear","up",1);
dictattr("climb","down",1);
dictattr("climb","up",1);
dictattr("clip","off",1);
dictattr("clog","up",1);
dictattr("close","down",1);
dictattr("close","off",1);
dictattr("close","out",1);
dictattr("close","up",1);
dictattr("combine","with",2);	#
dictattr("come","down",1);
dictattr("come","off",1);
dictattr("come","out",1);
dictattr("conjure","up",1);
dictattr("contract","out",1);	# ?
dictattr("cook","up",1);
dictattr("cool","down",1);
dictattr("cool","off",1);
dictattr("cough","up",1);
dictattr("count","out",1);
dictattr("cover","up",1);
dictattr("crack","down",1);
dictattr("cram","down",1);	# ?
dictattr("crank","out",1);
dictattr("crank","up",1);
dictattr("crowd","out",1);
dictattr("cry","out",1);
dictattr("cut","down",1);
dictattr("cut","off",1);
dictattr("cut","out",1);
dictattr("cut","up",1);
dictattr("die","down",1);
dictattr("die","off",1);
dictattr("dig","up",1);
dictattr("dish","out",1);
dictattr("divide","up",1);
dictattr("divvy","up",1);
dictattr("dole","out",1);
dictattr("drag","down",1);
dictattr("drag","out",1);
dictattr("drag","up",1);
dictattr("drain","out",1);
dictattr("draw","down",1);
dictattr("draw","off",1);
dictattr("draw","out",1);
dictattr("draw","up",1);
dictattr("dream","up",1);
dictattr("dress","up",1);
dictattr("drive","down",1);
dictattr("drive","off",1);
dictattr("drive","out",1);
dictattr("drive","up",1);
dictattr("drop","off",1);
dictattr("drop","out",1);
dictattr("drown","out",1);
dictattr("drum","up",1);
dictattr("dull","down",1);
dictattr("dust","off",1);
dictattr("eat","up",1);
dictattr("edge","down",1);
dictattr("edge","up",1);
dictattr("edge","out",1);
dictattr("eke","out",1);
dictattr("even","up",1);
dictattr("face","down",1);
dictattr("fake","out",1);
dictattr("fall","out",1);
dictattr("fan","out",1);
dictattr("farm","out",1);
dictattr("fatten","up",1);
dictattr("fend","off",1);
dictattr("ferret","out",1);
dictattr("fight","off",1);
dictattr("fight","out",1);
dictattr("figure","out",1);
dictattr("fill","out",1);
dictattr("fill","up",1);
dictattr("filter","down",1);
dictattr("find","out",1);
dictattr("finish","off",1);
dictattr("finish","up",1);
dictattr("fire","off",1);
dictattr("fire","up",1);
dictattr("firm","up",1);
dictattr("fish","out",1);	# ?
dictattr("fizzle","out",1);
dictattr("flame","out",1);
dictattr("flatten","out",1);
dictattr("flesh","out",1);
dictattr("fling","off",1);
dictattr("flush","out",1);
dictattr("fly","out",1);
dictattr("fold","up",1);
dictattr("follow","up",1);
dictattr("force","out",1);
dictattr("force","up",1);
dictattr("fork","up",1);
dictattr("foul","up",1);
dictattr("freak","out",1);
dictattr("free","up",1);
dictattr("firghten","off",1);
dictattr("gear","up",1);	# to...
dictattr("get","out",1);
dictattr("get","up",1);
dictattr("give","out",1);
dictattr("give","up",1);
dictattr("glue","down",1);
dictattr("grow","out",1);
dictattr("gobble","up",1);
dictattr("grab","up",1);
dictattr("grind","up",1);
dictattr("gum","up",1);
dictattr("gun","down",1);
dictattr("gussy","up",1);
dictattr("hack","off",1);
dictattr("hammer","out",1);
dictattr("hand","down",1);
dictattr("hand","out",1);
dictattr("hand","up",1);
dictattr("hang","up",1);
dictattr("haul","out",1);	# 5/25/06 AM.
dictattr("head","down",1);
dictattr("head","off",1);
dictattr("heat","up",1);
dictattr("heave","off",1);	# ?
dictattr("help","out",1);
dictattr("hike","up",1);
dictattr("hit","off",1);	# Idiomatic: hit it off...
dictattr("hold","down",1);
dictattr("hold","off",1);
dictattr("hold","out",1);
dictattr("hold","up",1);
dictattr("hook","up",1);
dictattr("hunker","down",1);
dictattr("inch","down",1);	# ?
dictattr("inch","up",1);	# ?
dictattr("iron","out",1);
dictattr("jack","up",1);
dictattr("jam","up",1);
dictattr("jerk","up",1);
dictattr("join","up",1);
dictattr("keep","out",1);
dictattr("keep","up",1);
dictattr("kick","off",1);
dictattr("kick","out",1);
dictattr("kick","up",1);
dictattr("kill","off",1);
dictattr("knock","down",1);
dictattr("knock","off",1);
dictattr("knock","out",1);
dictattr("lash","out",1);
dictattr("laugh","off",1);
dictattr("lay","down",1);
dictattr("lay","off",1);
dictattr("lay","out",1);
dictattr("lead","off",1);
dictattr("leak","out",1);
dictattr("lease","out",1);
dictattr("leave","off",1);
dictattr("leave","out",1);
dictattr("leave","up",1);
dictattr("lend","out",1);
dictattr("let","down",1);
dictattr("let","off",1);
dictattr("level","off",1);
dictattr("lie","down",1);
dictattr("lift","off",1);
dictattr("light","up",1);
dictattr("lighten","up",1);
dictattr("line","up",1);
dictattr("link","up",1);
dictattr("live","out",1);	# ?
dictattr("load","up",1);
dictattr("loan","out",1);
dictattr("lock","up",1);
dictattr("look","up",1);
dictattr("loosen","up",1);
dictattr("lose","out",1);
dictattr("lop","off",1);
dictattr("luck","out",1);
dictattr("mail","off",1);
dictattr("mail","out",1);
dictattr("make","out",1);
dictattr("make","up",1);
dictattr("map","out",1);
dictattr("mark","down",1);
dictattr("mark","up",1);
dictattr("marry","off",1);
dictattr("match","up",1);
dictattr("mess","up",1);
dictattr("mete","out",1);
dictattr("miss","out",1);
dictattr("mop","up",1);
dictattr("move","down",1);
dictattr("move","out",1);
dictattr("move","up",1);
dictattr("muck","up",1);
dictattr("nail","down",1);
dictattr("nudge","up",1);
dictattr("offer","up",1);
dictattr("open","up",1);
dictattr("pace","off",1);
dictattr("pack","up",1);
dictattr("pair","up",1);
dictattr("pan","out",1);
dictattr("parcel","out",1);
dictattr("pare","down",1);
dictattr("pass","off",1);
dictattr("pass","out",1);
dictattr("pass","up",1);
dictattr("paste","up",1);
dictattr("patch","up",1);
dictattr("pawn","off",1);
dictattr("pay","down",1);
dictattr("pay","off",1);
dictattr("pay","out",1);
dictattr("peel","off",1);
dictattr("perk","up",1);
dictattr("phase","out",1);
dictattr("pick","up",1);
dictattr("pile","up",1);
dictattr("pin","down",1);
dictattr("play","down",1);
dictattr("play","out",1);
dictattr("plot","out",1); # ?
dictattr("pluck","up",1);
dictattr("plunk","down",1);
dictattr("point","out",1);
dictattr("point","up",1);
dictattr("pop","out",1);
dictattr("pour","out",1);
dictattr("prop","up",1);
dictattr("pull","down",1);
dictattr("pull","off",1);
dictattr("pull","out",1);
dictattr("pull","up",1);
dictattr("pump","up",1);
dictattr("push","down",1);
dictattr("push","up",1);
dictattr("put","down",1);
dictattr("put","in",1);
dictattr("put","off",1);
dictattr("put","out",1);
dictattr("put","up",1);
dictattr("puzzle","out",1);
dictattr("quiet","down",1);
dictattr("rack","up",1);
dictattr("raise","up",1);
dictattr("ramp","up",1);
dictattr("ratchet","up",1);
dictattr("rattle","off",1);
dictattr("reach","down",1);
dictattr("reach","out",1);
dictattr("reel","off",1);
dictattr("ride","out",1);
dictattr("ride","up",1);
dictattr("rig","up",1);
dictattr("ring","out",1);
dictattr("ring","up",1);
dictattr("rip","down",1);
dictattr("rip","off",1);
dictattr("rip","out",1);
dictattr("rip","up",1);
dictattr("roll","down",1);
dictattr("roll","off",1);
dictattr("roll","out",1);
dictattr("roll","up",1);
dictattr("root","out",1);
dictattr("rough","up",1);
dictattr("rope","off",1);
dictattr("round","out",1);
dictattr("round","up",1);
dictattr("rule","out",1);
dictattr("run","down",1);	# also 2?
dictattr("run","out",1);
dictattr("run","up",1);
dictattr("save","up",1);
dictattr("scale","down",1);
dictattr("scare","off",1);
dictattr("scare","up",1);
dictattr("scarf","up",1);
dictattr("scoop","up",1);
dictattr("scratch","out",1);
dictattr("screen","out",1);
dictattr("screw","up",1);
dictattr("scrounge","up",1);
dictattr("seal","off",1);
dictattr("search","out",1);
dictattr("seek","out",1);
dictattr("sell","off",1);
dictattr("sell","out",1);
dictattr("send","down",1);
dictattr("send","off",1);
dictattr("send","out",1);
dictattr("serve","up",1);
dictattr("set","down",1);
dictattr("set","out",1);
dictattr("set","up",1);
dictattr("sew","up",1);
dictattr("settle","down",1);
dictattr("shake","down",1);
dictattr("shake","off",1);
dictattr("shake","out",1);
dictattr("shake","up",1);
dictattr("shape","up",1);
dictattr("share","out",1);
dictattr("shear","off",1);
dictattr("shell","out",1);
dictattr("ship","out",1);
dictattr("shoot","down",1);
dictattr("shoot","up",1);
dictattr("shore","up",1);
dictattr("shout","down",1);
dictattr("show","off",1);
dictattr("shrug","off",1);
dictattr("shut","down",1);
dictattr("shut","off",1);
dictattr("shut","out",1);
dictattr("shut","up",1);
dictattr("sign","up",1);
dictattr("single","out",1);
dictattr("siphon","off",1);
dictattr("sit","down",1);
dictattr("sit","out",1);
dictattr("size","up",1);
dictattr("sketch","out",1);
dictattr("skim","off",1);
dictattr("slice","off",1);
dictattr("slice","up",1);
dictattr("slide","down",1);
dictattr("slim","down",1);
dictattr("slip","down",1);
dictattr("slough","off",1);
dictattr("slow","down",1);
dictattr("smooth","out",1);
dictattr("snap","up",1);
dictattr("snatch","up",1);
dictattr("sniff","out",1);
dictattr("snuff","out",1);
dictattr("soak","off",1);	# ?
dictattr("soak","up",1);
dictattr("sober","up",1);
dictattr("sop","up",1);
dictattr("sort","out",1);
dictattr("speak","out",1);
dictattr("speed","up",1);
dictattr("spend","down",1);
dictattr("spell","out",1);
dictattr("spice","up",1);
dictattr("spiff","up",1);
dictattr("spill","out",1);
dictattr("spin","off",1);
dictattr("spin","out",1);
dictattr("spit","out",1);
dictattr("split","off",1);
dictattr("split","up",1);
dictattr("spread","out",1);
dictattr("spruce","up",1);
dictattr("squeeze","out",1);
dictattr("stack","up",1);
dictattr("stake","out",1);
dictattr("stall","out",1);
dictattr("stand","out",1);
dictattr("stand","up",1);
dictattr("stare","down",1);
dictattr("start","out",1);
dictattr("start","up",1);
dictattr("stave","off",1);
dictattr("step","down",1);
dictattr("step","up",1);
dictattr("stick","up",1);
dictattr("stir","up",1);
dictattr("stock","up",1);
dictattr("straighten","out",1);
dictattr("straighten","up",1);
dictattr("stretch","out",1);
dictattr("strike","down",1);
dictattr("strike","out",1);
dictattr("strike","up",1);
dictattr("strip","down",1);
dictattr("strip","off",1);
dictattr("strip","out",1);
dictattr("sum","up",1);
dictattr("swab","down",1);
dictattr("swallow","up",1);
dictattr("sweat","out",1);
dictattr("sweep","up",1);
dictattr("swoop","down",1);
dictattr("tack","down",1);
dictattr("take","on",1);
dictattr("take","out",1);
dictattr("take","over",1);
dictattr("take","up",1);
dictattr("talk","down",1);
dictattr("talk","up",1);
dictattr("tamp","down",1);
dictattr("tangle","up",1);
dictattr("tear","down",1);
dictattr("tear","off",1);
dictattr("tear","up",1);
dictattr("tee","up",1);
dictattr("thin","out",1);
dictattr("think","up",1);
dictattr("thrash","out",1);
dictattr("throw","off",1);
dictattr("throw","out",1);
dictattr("throw","up",1);
dictattr("tick","off",1);
dictattr("tie","down",1);
dictattr("tie","up",1);
dictattr("tighten","up",1);
dictattr("tip","off",1);
dictattr("tone","down",1);
dictattr("top","off",1);
dictattr("top","out",1);
dictattr("toss","down",1);
dictattr("toss","out",1);
dictattr("total","up",1);
dictattr("tote","up",1);
dictattr("touch","off",1);
dictattr("track","down",1);
dictattr("trade","off",1);
dictattr("trade","up",1);
dictattr("trickle","down",1);
dictattr("trickle","out",1);
dictattr("trip","up",1);
dictattr("try","out",1);
dictattr("tumble","down",1);
dictattr("turn","down",1);
dictattr("turn","off",1);
dictattr("turn","out",1);
dictattr("turn","up",1);
dictattr("use","up",1);
dictattr("vote","down",1);
dictattr("wake","up",1);
dictattr("walk","off",1);	# walk the jitters off.
dictattr("walk","out",1);
dictattr("wall","off",1);
dictattr("ward","off",1);
dictattr("warm","up",1);
dictattr("wash","down",1);
dictattr("watch","out",1);
dictattr("water","down",1);
dictattr("wave","off",1);
dictattr("wear","down",1);
dictattr("wear","off",1);
dictattr("wear","out",1);
dictattr("weed","out",1);
dictattr("weigh","down",1);
dictattr("whack","off",1);
dictattr("wheel","up",1);
dictattr("whip","up",1);
dictattr("whittle","down",1);
dictattr("win","out",1);
dictattr("wind","down",1);
dictattr("wind","up",1);
dictattr("wipe","out",1);
dictattr("work","down",1);
dictattr("work","off",1);
dictattr("work","out",1);
dictattr("work","up",1);
dictattr("wrap","up",1);
dictattr("write","down",1);	# ?
dictattr("write","off",1);
dictattr("yank","down",1);
dictattr("yank","off",1);
dictattr("yank","out",1);
dictattr("yield","up",1);
dictattr("zero","out",1);

#### PREPOSITIONAL ####
# NOTE: Need to distinguish
# 1. transitive "bring about X"
# 2. intransitive "X eased off."
dictattr("account","for",2);
dictattr("act","up",2);
dictattr("ante","up",2);
dictattr("apply","for",2);
dictattr("balk","at",2);	#
#dictattr("bark","up",2);	# ?
dictattr("back","off",2);	# also 1?
dictattr("bleed","off",2);	# ? 1 ?
dictattr("bone","up",2);
dictattr("bind","up",2);
dictattr("bring","about",2);	# ?
dictattr("brush","up",2);
dictattr("bubble","up",2);
dictattr("buzz","off",2);
dictattr("call","for",2);	# 06/14/06 AM.
dictattr("catch","up",2);
dictattr("charge","off",2);	# ?
dictattr("check","up",2);	# on...
dictattr("clam","up",2);
dictattr("climb","up",2);
dictattr("come","off",2);
dictattr("come","up",2);
dictattr("complain","to",2);	#
dictattr("consist","of",2);
dictattr("cozy","up",2);
dictattr("creep","up",2);
dictattr("crop","up",2);
dictattr("die","off",2);
dictattr("dive","off",2);	# ?
dictattr("doze","off",2);
dictattr("dry","up",2);
dictattr("ease","off",2);
dictattr("ease","up",2);
dictattr("edge","up",2);
dictattr("end","up",2);
dictattr("face","up",2);
dictattr("fall","off",2);
dictattr("feed","up",2);	# Only "fed up"...
dictattr("flare","up",2);
dictattr("fly","off",2);	# ?
dictattr("fret","about",2);
dictattr("gang","up",2);
dictattr("gaze","up",2);
dictattr("give","off",2);
dictattr("go","to",2);	#
dictattr("go","up",2);
dictattr("head","up",2);
dictattr("hole","up",2);
dictattr("hop","up",2);
dictattr("inch","up",2);
dictattr("jet","off",2);	# 04/21/07 AM.
dictattr("jump","up",2);
dictattr("lead","to",2);	#
dictattr("lead","up",2);
dictattr("let","up",2);
dictattr("link","to",2);
dictattr("live","off",2);	# ?
dictattr("live","up",2);
dictattr("look","at",2);
dictattr("look","out",2);
dictattr("measure","up",2);
dictattr("own","up",2);		# to...
dictattr("pay","up",2);
dictattr("point","to",2);	#
dictattr("point","toward",2);
dictattr("play","up",2);
dictattr("pony","up",2);	# /in ?
dictattr("pop","up",2);
#dictattr("range","up",2);
dictattr("reach","up",2);
dictattr("read","up",2);
dictattr("rear","up",2);
dictattr("relate","to",2);	#
dictattr("rise","up",2);
dictattr("rocket","up",2);
dictattr("seize","up",2);	# ?
dictattr("silt","up",2);	# ??
dictattr("sit","up",2);
dictattr("skid","off",2);	# ?
dictattr("skip","off",2);
dictattr("slack","off",2);
dictattr("slip","up",2);
dictattr("sneak","up",2);
dictattr("speak","up",2);
dictattr("specialize","in",2);	#
dictattr("spring","up",2);
dictattr("sprout","up",2);
dictattr("square","off",2);	# also 1?
dictattr("stay","up",2);
dictattr("stop","off",2);	# ? # 1?
#dictattr("stride","up",2); # to...
dictattr("suit","up",2);
dictattr("swear","off",2);
dictattr("tail","off",2);	# ?
dictattr("talk","about",2);
dictattr("taper","off",2);
dictattr("team","up",2);
dictattr("tee","off",2);	# 1?
dictattr("think","about",2);
dictattr("tick","up",2);
dictattr("trend","up",2);
dictattr("walk","up",2);
dictattr("wander","off",2);
dictattr("wear","off",2);	# also 1?
dictattr("well","up",2);
dictattr("whisk","off",2);	# ?
dictattr("zoom","up",2);


######### PREPOSITIONAL AND/OR PHRASAL ####
# Both are valid.
dictattr("curl","up",3);
dictattr("dash","off",3);	# dash a letter off
dictattr("hang","out",3);
dictattr("run","off",3);
dictattr("set","off",3);
dictattr("take","off",3);
dictattr("show","up",3);
dictattr("sign","off",3);	# sign property off...
dictattr("start","off",3);	# ? # start off happy...

#dictattr("bounce","off",x);	# ?
# dictattr("get","off",x); # get something off -- X7,X9 verb...
#dictattr("veer","off",x);


########################################
# CORE PART-OF-SPEECH
########################################
# For example, "possible" is listed as noun and adj.
# But at its core, it is an adjective that can be used
# in noun contexts.
dictattr("personal","core","adj");
dictattr("possible","core","adj");

########################################
# JJ PREFERRED OVER VBN
########################################
# Words that prefer an adjective interpretation
# with copula or ellipted copula contexts.
# Semantic overtones here, eg, human-emotion-verbs.
# Also states of people and things.

# THESE WERE DONE FROM CORPUS.
dictattr("abandon","jj-pref",1);
dictattr("abash","jj-pref",1);
dictattr("abort","jj-pref",1);
dictattr("abuse","jj-pref",1);
dictattr("accelerate","jj-pref",1);
dictattr("accept","jj-pref",1);
dictattr("acclaim","jj-pref",1);
dictattr("accomplish","jj-pref",1);
dictattr("accrue","jj-pref",1);
dictattr("accumulate","jj-pref",1);
dictattr("accuse","jj-pref",1);
dictattr("accustom","jj-pref",1);
dictattr("acknowledge","jj-pref",1);
dictattr("acquire","jj-pref",1);
dictattr("activate","jj-pref",1);
dictattr("add","jj-pref",1);
dictattr("addict","jj-pref",1);
dictattr("addle","jj-pref",1);
dictattr("address","jj-pref",1);
dictattr("adjust","jj-pref",1);
dictattr("admire","jj-pref",1);
dictattr("admit","jj-pref",1);
dictattr("adopt","jj-pref",1);
dictattr("adulterate","jj-pref",1);
dictattr("advance","jj-pref",1);
dictattr("advertise","jj-pref",1);
dictattr("advise","jj-pref",1);
dictattr("affect","jj-pref",1);
dictattr("affiliate","jj-pref",1);
dictattr("afflict","jj-pref",1);
dictattr("aforemention","jj-pref",1);
dictattr("age","jj-pref",1);
dictattr("agree","jj-pref",1);
dictattr("alarm","jj-pref",1);
dictattr("align","jj-pref",1);
dictattr("allege","jj-pref",1);
dictattr("ally","jj-pref",1);
dictattr("allow","jj-pref",1);
dictattr("alter","jj-pref",1);
dictattr("amaze","jj-pref",1);
dictattr("amuse","jj-pref",1);
dictattr("anesthetize","jj-pref",1);
dictattr("anger","jj-pref",1);
dictattr("anguish","jj-pref",1);
dictattr("animate","jj-pref",1);
dictattr("announce","jj-pref",1);
dictattr("annoy","jj-pref",1);
dictattr("annualize","jj-pref",1);
dictattr("anticipate","jj-pref",1);
dictattr("antiquate","jj-pref",1);
dictattr("appall","jj-pref",1);
dictattr("apply","jj-pref",1);
dictattr("appoint","jj-pref",1);
dictattr("appreciate","jj-pref",1);
dictattr("approve","jj-pref",1);
dictattr("approximate","jj-pref",1);
dictattr("arch","jj-pref",1);
dictattr("arm","jj-pref",1);
dictattr("armor","jj-pref",1);
dictattr("ashame","jj-pref",1);
dictattr("ask","jj-pref",1);
dictattr("assemble","jj-pref",1);
dictattr("assign","jj-pref",1);
dictattr("assimilate","jj-pref",1);
dictattr("associate","jj-pref",1);
dictattr("assort","jj-pref",1);
dictattr("assume","jj-pref",1);
dictattr("assure","jj-pref",1);
dictattr("astound","jj-pref",1);
dictattr("attach","jj-pref",1);
dictattr("attempt","jj-pref",1);
dictattr("attire","jj-pref",1);
dictattr("attune","jj-pref",1);
dictattr("audit","jj-pref",1);
dictattr("augment","jj-pref",1);
dictattr("authorize","jj-pref",1);
dictattr("autograph","jj-pref",1);
dictattr("automate","jj-pref",1);
dictattr("avoid","jj-pref",1);
dictattr("await","jj-pref",1);
dictattr("awe","jj-pref",1);
dictattr("back","jj-pref",1);
dictattr("backhand","jj-pref",1);
dictattr("bag","jj-pref",1);
dictattr("bake","jj-pref",1);
dictattr("balance","jj-pref",1);
dictattr("balkanize","jj-pref",1);
dictattr("barbeque","jj-pref",1);
dictattr("bate","jj-pref",1);
dictattr("batter","jj-pref",1);
dictattr("beach","jj-pref",1);
dictattr("beard","jj-pref",1);
dictattr("belate","jj-pref",1); #
dictattr("beleaguer","jj-pref",1);
dictattr("belittle","jj-pref",1);
dictattr("belle","jj-pref",1);
dictattr("beloved","jj-pref",1);
dictattr("bemuse","jj-pref",1);
dictattr("benight","jj-pref",1);
dictattr("beribbon","jj-pref",1);
dictattr("besiege","jj-pref",1);
dictattr("bespectacle","jj-pref",1);
dictattr("bewilder","jj-pref",1);
dictattr("bifurcate","jj-pref",1);
dictattr("bigot","jj-pref",1);
dictattr("bill","jj-pref",1);
dictattr("bleach","jj-pref",1);
dictattr("blend","jj-pref",1);
dictattr("blight","jj-pref",1);
dictattr("blindfold","jj-pref",1);
dictattr("blindside","jj-pref",1);
dictattr("bloat","jj-pref",1);
dictattr("block","jj-pref",1);
dictattr("bloody","jj-pref",1);
dictattr("bond","jj-pref",1);
dictattr("bore","jj-pref",1);
dictattr("borrow","jj-pref",1);
dictattr("botch","jj-pref",1);
dictattr("bottle","jj-pref",1);
dictattr("brand","jj-pref",1);
dictattr("broker","jj-pref",1);
dictattr("buckle","jj-pref",1);
dictattr("bureaucraticize","jj-pref",1);
dictattr("bury","jj-pref",1);
dictattr("burn","jj-pref",1);
dictattr("bust","jj-pref",1);
dictattr("calculate","jj-pref",1);
dictattr("camouflage","jj-pref",1);
dictattr("cancel","jj-pref",1);
dictattr("can","jj-pref",1);
dictattr("capitalize","jj-pref",1);
dictattr("cap","jj-pref",1);
dictattr("captivate","jj-pref",1);
dictattr("carve","jj-pref",1);
dictattr("cater","jj-pref",1);
dictattr("celebrate","jj-pref",1);
dictattr("centralize","jj-pref",1);
dictattr("certify","jj-pref",1);
dictattr("chain","jj-pref",1);
dictattr("chair","jj-pref",1);
dictattr("change","jj-pref",1);
dictattr("characterize","jj-pref",1);
dictattr("charge","jj-pref",1);
dictattr("char","jj-pref",1);
dictattr("charter","jj-pref",1);
dictattr("cheat","jj-pref",1);
dictattr("cherish","jj-pref",1);
dictattr("chill","jj-pref",1);
dictattr("chopp","jj-pref",1);
dictattr("choreograph","jj-pref",1);
dictattr("circumscribe","jj-pref",1);
dictattr("civilise","jj-pref",1);
dictattr("civilize","jj-pref",1);
dictattr("classify","jj-pref",1);
dictattr("clearhead","jj-pref",1);
dictattr("cliche","jj-pref",1);
dictattr("clog","jj-pref",1);
dictattr("cloister","jj-pref",1);
dictattr("close","jj-pref",1);
dictattr("closet","jj-pref",1);
dictattr("cloud","jj-pref",1);
dictattr("clutter","jj-pref",1);
dictattr("coat","jj-pref",1);
dictattr("cockeye","jj-pref",1);
dictattr("code","jj-pref",1);
dictattr("coerce","jj-pref",1);
dictattr("coif","jj-pref",1);
dictattr("coldheart","jj-pref",1);
dictattr("collapse","jj-pref",1);
dictattr("collateralize","jj-pref",1);
dictattr("color","jj-pref",1);
dictattr("combine","jj-pref",1);
dictattr("commit","jj-pref",1);
dictattr("compact","jj-pref",1);
dictattr("compel","jj-pref",1);
dictattr("complete","jj-pref",1);
dictattr("complicate","jj-pref",1);
dictattr("compound","jj-pref",1);
dictattr("compute","jj-pref",1);
dictattr("computerize","jj-pref",1);
dictattr("computerrelate","jj-pref",1);
dictattr("conceal","jj-pref",1);
dictattr("conceive","jj-pref",1);
dictattr("concentrate","jj-pref",1);
dictattr("concern","jj-pref",1);
dictattr("concert","jj-pref",1);
dictattr("condemn","jj-pref",1);
dictattr("condense","jj-pref",1);
dictattr("condition","jj-pref",1);
dictattr("confine","jj-pref",1);
dictattr("confirm","jj-pref",1);
dictattr("confuse","jj-pref",1);
dictattr("congest","jj-pref",1);
dictattr("connect","jj-pref",1);
dictattr("consider","jj-pref",1);
dictattr("consolidate","jj-pref",1);
dictattr("constrict","jj-pref",1);
dictattr("contain","jj-pref",1);
dictattr("contaminate","jj-pref",1);
dictattr("contemplate","jj-pref",1);
dictattr("content","jj-pref",1);
dictattr("contest","jj-pref",1);
dictattr("continue","jj-pref",1);
dictattr("contort","jj-pref",1);
dictattr("contrive","jj-pref",1);
dictattr("control","jj-pref",1);
dictattr("convert","jj-pref",1);
dictattr("convince","jj-pref",1);
dictattr("convolute","jj-pref",1);
dictattr("cook","jj-pref",1);
dictattr("coordinate","jj-pref",1);
dictattr("corner","jj-pref",1);
dictattr("correct","jj-pref",1);
dictattr("corrugate","jj-pref",1);
dictattr("cover","jj-pref",1);
dictattr("covet","jj-pref",1);
dictattr("crab","jj-pref",1);
dictattr("cramp","jj-pref",1);
dictattr("crash","jj-pref",1);
dictattr("craze","jj-pref",1);
dictattr("create","jj-pref",1);
dictattr("credentialize","jj-pref",1);
dictattr("crest","jj-pref",1);
dictattr("criminalize","jj-pref",1);
dictattr("crimp","jj-pref",1);
dictattr("cripple","jj-pref",1);
dictattr("crook","jj-pref",1);
dictattr("crowd","jj-pref",1);
dictattr("crumple","jj-pref",1);
dictattr("crush","jj-pref",1);
dictattr("curve","jj-pref",1);
dictattr("customize","jj-pref",1);
dictattr("damage","jj-pref",1);
dictattr("damn","jj-pref",1);
dictattr("date","jj-pref",1);
dictattr("daunt","jj-pref",1);
dictattr("daze","jj-pref",1);
dictattr("deadlock","jj-pref",1);
dictattr("debilitate","jj-pref",1);
dictattr("decapitalize","jj-pref",1);
dictattr("decease","jj-pref",1);
dictattr("decentralize","jj-pref",1);
dictattr("declare","jj-pref",1);
dictattr("decommission","jj-pref",1);
dictattr("decompose","jj-pref",1);
dictattr("deconstruct","jj-pref",1);
dictattr("decriminalize","jj-pref",1);
dictattr("dedicate","jj-pref",1);
dictattr("default","jj-pref",1);
dictattr("defer","jj-pref",1);
dictattr("define","jj-pref",1);
dictattr("deform","jj-pref",1);
dictattr("degrade","jj-pref",1);
dictattr("dehumidify","jj-pref",1);
dictattr("dehydrate","jj-pref",1);
dictattr("delay","jj-pref",1);
dictattr("delight","jj-pref",1);
dictattr("dement","jj-pref",1);
dictattr("demonstrate","jj-pref",1);
dictattr("demoralize","jj-pref",1);
dictattr("depress","jj-pref",1);
dictattr("deregulate","jj-pref",1);
dictattr("desert","jj-pref",1);
dictattr("designate","jj-pref",1);
dictattr("desire","jj-pref",1);
dictattr("destroy","jj-pref",1);
dictattr("detail","jj-pref",1);
dictattr("deteriorate","jj-pref",1);
dictattr("determine","jj-pref",1);
dictattr("devastate","jj-pref",1);
dictattr("develop","jj-pref",1);
dictattr("devote","jj-pref",1);
dictattr("differentiate","jj-pref",1);
dictattr("digitalize","jj-pref",1);
dictattr("dignify","jj-pref",1);
dictattr("dilapidate","jj-pref",1);
dictattr("dilute","jj-pref",1);
dictattr("diminish","jj-pref",1);
dictattr("dimwitted","jj-pref",1);
dictattr("disable","jj-pref",1);
dictattr("disadvantage","jj-pref",1);
dictattr("disaffect","jj-pref",1);
dictattr("disappoint","jj-pref",1);
dictattr("disband","jj-pref",1);
dictattr("discipline","jj-pref",1);
dictattr("disclose","jj-pref",1);
dictattr("discomfit","jj-pref",1);
dictattr("disconnect","jj-pref",1);
dictattr("discontent","jj-pref",1);
dictattr("discontinue","jj-pref",1);
dictattr("discount","jj-pref",1);
dictattr("discourage","jj-pref",1);
dictattr("discredit","jj-pref",1);
dictattr("discriminate","jj-pref",1);
dictattr("disease","jj-pref",1);
dictattr("disembody","jj-pref",1);
dictattr("disenchant","jj-pref",1);
dictattr("disfavor","jj-pref",1);
dictattr("disgruntle","jj-pref",1);
dictattr("disguise","jj-pref",1);
dictattr("disgust","jj-pref",1);
dictattr("dishevel","jj-pref",1);
dictattr("disincline","jj-pref",1);
dictattr("disinterest","jj-pref",1);
dictattr("disjoint","jj-pref",1);
dictattr("dislike","jj-pref",1);
dictattr("dislocate","jj-pref",1);
dictattr("dismay","jj-pref",1);
dictattr("disorganize","jj-pref",1);
dictattr("disorient","jj-pref",1);
dictattr("disperse","jj-pref",1);
dictattr("displease","jj-pref",1);
dictattr("dispose","jj-pref",1);
dictattr("dispute","jj-pref",1);
dictattr("dissatisfy","jj-pref",1);
dictattr("distill","jj-pref",1);
dictattr("distinguish","jj-pref",1);
dictattr("distort","jj-pref",1);
dictattr("distress","jj-pref",1);
dictattr("distribute","jj-pref",1);
dictattr("disturb","jj-pref",1);
dictattr("disuse","jj-pref",1);
dictattr("diversify","jj-pref",1);
dictattr("divest","jj-pref",1);
dictattr("divide","jj-pref",1);
dictattr("divorce","jj-pref",1);
dictattr("dog","jj-pref",1);
dictattr("dome","jj-pref",1);
dictattr("doom","jj-pref",1);
dictattr("dot","jj-pref",1);
dictattr("dread","jj-pref",1);
dictattr("dry","jj-pref",1);
dictattr("drown","jj-pref",1);
dictattr("dumbfound","jj-pref",1);
dictattr("earn","jj-pref",1);
dictattr("edge","jj-pref",1);
dictattr("educate","jj-pref",1);
dictattr("elate","jj-pref",1);
dictattr("elect","jj-pref",1);
dictattr("electrogalvanize","jj-pref",1);
dictattr("elevate","jj-pref",1);
dictattr("embargo","jj-pref",1);
dictattr("embarrass","jj-pref",1);
dictattr("embattle","jj-pref",1);
dictattr("embed","jj-pref",1);
dictattr("embitter","jj-pref",1);
dictattr("embolden","jj-pref",1);
dictattr("enamor","jj-pref",1);
dictattr("encourage","jj-pref",1);
dictattr("endanger","jj-pref",1);
dictattr("end","jj-pref",1);
dictattr("endow","jj-pref",1);
dictattr("enforce","jj-pref",1);
dictattr("engage","jj-pref",1);
dictattr("engineer","jj-pref",1);
dictattr("enhance","jj-pref",1);
dictattr("enlarge","jj-pref",1);
dictattr("enlighten","jj-pref",1);
dictattr("enlist","jj-pref",1);
dictattr("enrage","jj-pref",1);
dictattr("entangle","jj-pref",1);
dictattr("enthrall","jj-pref",1);
dictattr("entrench","jj-pref",1);
dictattr("enumerate","jj-pref",1);
dictattr("equip","jj-pref",1);
dictattr("establish","jj-pref",1);
dictattr("estimate","jj-pref",1);
dictattr("estrange","jj-pref",1);
dictattr("evenhanded","jj-pref",1);
dictattr("evolve","jj-pref",1);
dictattr("exaggerate","jj-pref",1);
dictattr("excite","jj-pref",1);
dictattr("execute","jj-pref",1);
dictattr("exhaust","jj-pref",1);
dictattr("exile","jj-pref",1);
dictattr("expand","jj-pref",1);
dictattr("expect","jj-pref",1);
dictattr("expedite","jj-pref",1);
dictattr("experience","jj-pref",1);
dictattr("expire","jj-pref",1);
dictattr("expose","jj-pref",1);
dictattr("extend","jj-pref",1);
dictattr("fable","jj-pref",1);
dictattr("fabricate","jj-pref",1);
dictattr("factor","jj-pref",1);
dictattr("fade","jj-pref",1);
dictattr("fail","jj-pref",1);
dictattr("fame","jj-pref",1);
dictattr("farfetched","jj-pref",1);
dictattr("farm","jj-pref",1);
dictattr("fatiegue","jj-pref",1);
dictattr("favor","jj-pref",1);
dictattr("fear","jj-pref",1);
dictattr("feather","jj-pref",1);
dictattr("feature","jj-pref",1);
dictattr("feed","jj-pref",1);
dictattr("federalize","jj-pref",1);
dictattr("fence","jj-pref",1);
dictattr("fertilize","jj-pref",1);
dictattr("film","jj-pref",1);
dictattr("finance","jj-pref",1);
dictattr("finish","jj-pref",1);
dictattr("fix","jj-pref",1);
dictattr("flabbergasted","jj-pref",1);
dictattr("flaw","jj-pref",1);
dictattr("fluster","jj-pref",1);
dictattr("focus","jj-pref",1);
dictattr("fog","jj-pref",1);
dictattr("foil","jj-pref",1);
dictattr("follow","jj-pref",1);
dictattr("force","jj-pref",1);
dictattr("foreclose","jj-pref",1);
dictattr("forge","jj-pref",1);
dictattr("fork","jj-pref",1);
dictattr("form","jj-pref",1);
dictattr("fortify","jj-pref",1);
dictattr("foul","jj-pref",1);
dictattr("fracture","jj-pref",1);
dictattr("fragment","jj-pref",1);
dictattr("frame","jj-pref",1);
dictattr("franchise","jj-pref",1);
dictattr("frank","jj-pref",1);
dictattr("fray","jj-pref",1);
dictattr("frenzy","jj-pref",1);
dictattr("fry","jj-pref",1);
dictattr("frighten","jj-pref",1);
dictattr("frustrate","jj-pref",1);
dictattr("fund","jj-pref",1);
dictattr("furnish","jj-pref",1);
dictattr("furrow","jj-pref",1);
dictattr("galvanize","jj-pref",1);
dictattr("generalize","jj-pref",1);
dictattr("germinate","jj-pref",1);
dictattr("gift","jj-pref",1);
dictattr("gild","jj-pref",1);
dictattr("globalize","jj-pref",1);
dictattr("glut","jj-pref",1);
dictattr("govern","jj-pref",1);
dictattr("graduate","jj-pref",1);
dictattr("graft","jj-pref",1);
dictattr("grain","jj-pref",1);
dictattr("grant","jj-pref",1);
dictattr("gratify","jj-pref",1);
dictattr("grid","jj-pref",1);
dictattr("gridlock","jj-pref",1);
dictattr("grill","jj-pref",1);
dictattr("grizzle","jj-pref",1);
dictattr("ground","jj-pref",1);
dictattr("guarantee","jj-pref",1);
dictattr("guard","jj-pref",1);
dictattr("hairyknuckled","jj-pref",1);
dictattr("halfheart","jj-pref",1);
dictattr("hallow","jj-pref",1);
dictattr("halve","jj-pref",1);
dictattr("handicap","jj-pref",1);
dictattr("harden","jj-pref",1);
dictattr("haunt","jj-pref",1);
dictattr("heat","jj-pref",1);
dictattr("heighten","jj-pref",1);
dictattr("helmet","jj-pref",1);
dictattr("highpriced","jj-pref",1);
dictattr("hone","jj-pref",1);
dictattr("honor","jj-pref",1);
dictattr("hook","jj-pref",1);
dictattr("hospitalize","jj-pref",1);
dictattr("host","jj-pref",1);
dictattr("humiliate","jj-pref",1);
dictattr("hunch","jj-pref",1);
dictattr("hurry","jj-pref",1);
dictattr("ice","jj-pref",1);
dictattr("idealize","jj-pref",1);
dictattr("identify","jj-pref",1);
dictattr("illuminate","jj-pref",1);
dictattr("immiserate","jj-pref",1);
dictattr("impact","jj-pref",1);
dictattr("impair","jj-pref",1);
dictattr("impassion","jj-pref",1);
dictattr("imperil","jj-pref",1);
dictattr("imply","jj-pref",1);
dictattr("import","jj-pref",1);
dictattr("impose","jj-pref",1);
dictattr("impoverish","jj-pref",1);
dictattr("impress","jj-pref",1);
dictattr("improve","jj-pref",1);
dictattr("impute","jj-pref",1);
dictattr("incense","jj-pref",1);
dictattr("incline","jj-pref",1);
dictattr("include","jj-pref",1);
#dictattr("incorporate","jj-pref",1);
dictattr("increase","jj-pref",1);
dictattr("indebt","jj-pref",1);
dictattr("index","jj-pref",1);
dictattr("indicate","jj-pref",1);
dictattr("industrialize","jj-pref",1);
dictattr("inexperienced","jj-pref",1);
dictattr("infect","jj-pref",1);
dictattr("infest","jj-pref",1);
dictattr("inflame","jj-pref",1);
dictattr("inflate","jj-pref",1);
dictattr("inform","jj-pref",1);
dictattr("ingrain","jj-pref",1);
dictattr("inhabit","jj-pref",1);
dictattr("inherit","jj-pref",1);
dictattr("injure","jj-pref",1);
dictattr("inspire","jj-pref",1);
dictattr("install","jj-pref",1);
dictattr("instrument","jj-pref",1);
dictattr("insulate","jj-pref",1);
dictattr("insure","jj-pref",1);
dictattr("integrate","jj-pref",1);
dictattr("intend","jj-pref",1);
dictattr("intensify","jj-pref",1);
dictattr("intention","jj-pref",1);
dictattr("interest","jj-pref",1);
dictattr("interrelate","jj-pref",1);
dictattr("interrupt","jj-pref",1);
dictattr("intoxicate","jj-pref",1);
dictattr("intrigue","jj-pref",1);
dictattr("introvert","jj-pref",1);
dictattr("invert","jj-pref",1);
dictattr("involve","jj-pref",1);
dictattr("irradiate","jj-pref",1);
dictattr("irritate","jj-pref",1);
dictattr("isolate","jj-pref",1);
dictattr("issue","jj-pref",1);
dictattr("jade","jj-pref",1);
dictattr("jail","jj-pref",1);
dictattr("jam","jj-pref",1);
dictattr("justify","jj-pref",1);
dictattr("kerchiefed","jj-pref",1);
dictattr("key","jj-pref",1);
dictattr("kindred","jj-pref",1);
dictattr("kink","jj-pref",1);
dictattr("labor","jj-pref",1);
dictattr("ladder","jj-pref",1);
dictattr("landlock","jj-pref",1);
dictattr("lapse","jj-pref",1);
#dictattr("lead","jj-pref",1); # Need to be, etc.
dictattr("lease","jj-pref",1);
dictattr("leverage","jj-pref",1);
dictattr("liberalize","jj-pref",1);
dictattr("liberate","jj-pref",1);
dictattr("license","jj-pref",1);
dictattr("light","jj-pref",1);
dictattr("lightheaded","jj-pref",1);
dictattr("lighthearted","jj-pref",1);
dictattr("limit","jj-pref",1);
dictattr("line","jj-pref",1);
dictattr("link","jj-pref",1);
dictattr("liquify","jj-pref",1);
dictattr("list","jj-pref",1);
dictattr("livery","jj-pref",1);
dictattr("load","jj-pref",1);
dictattr("localize","jj-pref",1);
dictattr("locate","jj-pref",1);
dictattr("lock","jj-pref",1);
dictattr("longhaired","jj-pref",1);
dictattr("lopsided","jj-pref",1);
dictattr("love","jj-pref",1);
dictattr("mail","jj-pref",1);
dictattr("manage","jj-pref",1);
dictattr("man","jj-pref",1);
dictattr("manner","jj-pref",1);
dictattr("manufacture","jj-pref",1);
dictattr("mark","jj-pref",1);
dictattr("marry","jj-pref",1);
dictattr("match","jj-pref",1);
dictattr("measure","jj-pref",1);
dictattr("mechanize","jj-pref",1);
dictattr("merge","jj-pref",1);
dictattr("midpriced","jj-pref",1);
dictattr("midsized","jj-pref",1);
dictattr("miff","jj-pref",1);
dictattr("mill","jj-pref",1);
dictattr("mineralize","jj-pref",1);
dictattr("mire","jj-pref",1);
dictattr("misguided","jj-pref",1);
dictattr("mishandle","jj-pref",1);
dictattr("miss","jj-pref",1);
dictattr("mistrust","jj-pref",1);
dictattr("mix","jj-pref",1);
dictattr("modernize","jj-pref",1);
dictattr("modify","jj-pref",1);
dictattr("money","jj-pref",1);
dictattr("monied","jj-pref",1); #
dictattr("monitor","jj-pref",1);
dictattr("monogram","jj-pref",1);
dictattr("mortgagebacked","jj-pref",1);
dictattr("motivate","jj-pref",1);
dictattr("motorize","jj-pref",1);
dictattr("mount","jj-pref",1);
dictattr("multifaceted","jj-pref",1);
dictattr("multilayered","jj-pref",1);
dictattr("multisided","jj-pref",1);
dictattr("mustachioed","jj-pref",1);
dictattr("mute","jj-pref",1);
dictattr("naked","jj-pref",1);
dictattr("name","jj-pref",1);
dictattr("naturalize","jj-pref",1);
dictattr("natured","jj-pref",1);
dictattr("need","jj-pref",1);
dictattr("neglect","jj-pref",1);
dictattr("negotiate","jj-pref",1);
dictattr("newfangled","jj-pref",1);
dictattr("nonchlorinated","jj-pref",1);
dictattr("noninstitutionalized","jj-pref",1);
dictattr("nonregulate","jj-pref",1);
dictattr("note","jj-pref",1);
dictattr("notice","jj-pref",1);
dictattr("obligate","jj-pref",1);
dictattr("oblige","jj-pref",1);
dictattr("observe","jj-pref",1);
dictattr("obsess","jj-pref",1);
dictattr("occupy","jj-pref",1);
dictattr("offer","jj-pref",1);
dictattr("oppose","jj-pref",1);
dictattr("oppress","jj-pref",1);
dictattr("organize","jj-pref",1);
dictattr("orient","jj-pref",1);
dictattr("orphan","jj-pref",1);
dictattr("oust","jj-pref",1);
dictattr("outdate","jj-pref",1);
dictattr("outlaw","jj-pref",1);
dictattr("outline","jj-pref",1);
dictattr("outmoded","jj-pref",1);
dictattr("outrage","jj-pref",1);
dictattr("outsize","jj-pref",1);
dictattr("overburden","jj-pref",1);
dictattr("overcollect","jj-pref",1);
dictattr("overcrowd","jj-pref",1);
dictattr("overdevelop","jj-pref",1);
dictattr("overdress","jj-pref",1);
dictattr("overextend","jj-pref",1);
dictattr("overhype","jj-pref",1);
dictattr("overjoy","jj-pref",1);
dictattr("overleverage","jj-pref",1);
dictattr("overload","jj-pref",1);
dictattr("overprice","jj-pref",1);
dictattr("overregulate","jj-pref",1);
dictattr("oversized","jj-pref",1);
dictattr("overstaffed","jj-pref",1);
dictattr("overstored","jj-pref",1);
dictattr("oversubscribe","jj-pref",1);
dictattr("oversupply","jj-pref",1);
dictattr("overuse","jj-pref",1);
dictattr("overvalue","jj-pref",1);
dictattr("overwhelm","jj-pref",1);
dictattr("overwork","jj-pref",1);
dictattr("own","jj-pref",1);
dictattr("oxidize","jj-pref",1);
dictattr("package","jj-pref",1);
dictattr("pack","jj-pref",1);
dictattr("pad","jj-pref",1);
dictattr("pain","jj-pref",1);
dictattr("paint","jj-pref",1);
dictattr("pamper","jj-pref",1);
dictattr("panic","jj-pref",1);
dictattr("paralyze","jj-pref",1);
dictattr("parch","jj-pref",1);
dictattr("part","jj-pref",1);
dictattr("patch","jj-pref",1);
dictattr("patent","jj-pref",1);
dictattr("pave","jj-pref",1);
dictattr("peak","jj-pref",1);
dictattr("perceive","jj-pref",1);
dictattr("perforate","jj-pref",1);
dictattr("perfume","jj-pref",1);
dictattr("perplex","jj-pref",1);
dictattr("personalize","jj-pref",1);
dictattr("perturb","jj-pref",1);
dictattr("pervert","jj-pref",1);
dictattr("petrify","jj-pref",1);
dictattr("pinheaded","jj-pref",1);
dictattr("pinpoint","jj-pref",1);
dictattr("pitch","jj-pref",1);
dictattr("plan","jj-pref",1);
dictattr("please","jj-pref",1);
dictattr("pleat","jj-pref",1);
dictattr("pockmarked","jj-pref",1);
dictattr("point","jj-pref",1);
dictattr("poise","jj-pref",1);
dictattr("polish","jj-pref",1);
dictattr("politicize","jj-pref",1);
dictattr("pollute","jj-pref",1);
dictattr("pool","jj-pref",1);
dictattr("populate","jj-pref",1);
dictattr("position","jj-pref",1);
dictattr("post","jj-pref",1);
dictattr("postpone","jj-pref",1);
dictattr("pot","jj-pref",1);
dictattr("powder","jj-pref",1);
dictattr("practice","jj-pref",1);
dictattr("praise","jj-pref",1);
dictattr("prearrange","jj-pref",1);
dictattr("preclude","jj-pref",1);
dictattr("prefer","jj-pref",1);
dictattr("prejudice","jj-pref",1);
dictattr("premediate","jj-pref",1);
dictattr("premeditate","jj-pref",1);
dictattr("preoccupy","jj-pref",1);
dictattr("prepare","jj-pref",1);
dictattr("prescribe","jj-pref",1);
dictattr("pressure","jj-pref",1);
dictattr("presume","jj-pref",1);
dictattr("price","jj-pref",1);
dictattr("prime","jj-pref",1);
dictattr("principle","jj-pref",1);
dictattr("print","jj-pref",1);
dictattr("privatize","jj-pref",1);
dictattr("privilege","jj-pref",1);
dictattr("prize","jj-pref",1);
dictattr("process","jj-pref",1);
dictattr("proclaim","jj-pref",1);
dictattr("profess","jj-pref",1);
dictattr("program","jj-pref",1);
dictattr("project","jj-pref",1);
dictattr("prolong","jj-pref",1);
dictattr("promise","jj-pref",1);
dictattr("pronounce","jj-pref",1);
dictattr("proportion","jj-pref",1);
dictattr("propose","jj-pref",1);
dictattr("protect","jj-pref",1);
dictattr("protracted","jj-pref",1);
dictattr("prove","jj-pref",1);
dictattr("provoke","jj-pref",1);
dictattr("publicize","jj-pref",1);
dictattr("publish","jj-pref",1);
dictattr("purchase","jj-pref",1);
dictattr("purport","jj-pref",1);
dictattr("pursue","jj-pref",1);
dictattr("puzzle","jj-pref",1);
dictattr("qualify","jj-pref",1);
dictattr("quickwitted","jj-pref",1);
dictattr("rag","jj-pref",1);
dictattr("raise","jj-pref",1);
dictattr("rank","jj-pref",1);
dictattr("rape","jj-pref",1);
dictattr("rate","jj-pref",1);
dictattr("rationalize","jj-pref",1);
dictattr("rattle","jj-pref",1);
dictattr("realign","jj-pref",1);
dictattr("realize","jj-pref",1);
dictattr("reason","jj-pref",1);
dictattr("receive","jj-pref",1);
dictattr("recognize","jj-pref",1);
dictattr("recommend","jj-pref",1);
dictattr("reconstruct","jj-pref",1);
dictattr("record","jj-pref",1);
dictattr("recycle","jj-pref",1);
dictattr("redesign","jj-pref",1);
dictattr("reduce","jj-pref",1);
dictattr("refine","jj-pref",1);
dictattr("reform","jj-pref",1);
dictattr("reformulate","jj-pref",1);
dictattr("refrigerate","jj-pref",1);
dictattr("regard","jj-pref",1);
dictattr("regiment","jj-pref",1);
dictattr("register","jj-pref",1);
dictattr("regulate","jj-pref",1);
dictattr("reinforce","jj-pref",1);
dictattr("rejuvenate","jj-pref",1);
dictattr("relate","jj-pref",1);
dictattr("relax","jj-pref",1);
dictattr("relieve","jj-pref",1);
dictattr("renew","jj-pref",1);
dictattr("renovate","jj-pref",1);
dictattr("renown","jj-pref",1);
dictattr("rent","jj-pref",1);
dictattr("reoffer","jj-pref",1);
dictattr("reorganize","jj-pref",1);
dictattr("repaint","jj-pref",1);
dictattr("repeat","jj-pref",1);
dictattr("report","jj-pref",1);
dictattr("repossess","jj-pref",1);
dictattr("repress","jj-pref",1);
dictattr("repute","jj-pref",1);
dictattr("require","jj-pref",1);
dictattr("reserve","jj-pref",1);
#dictattr("resign","jj-pref",1); # Need to-be or other context.
dictattr("respect","jj-pref",1);
dictattr("restate","jj-pref",1);
dictattr("restrain","jj-pref",1);
dictattr("restrict","jj-pref",1);
dictattr("restructure","jj-pref",1);
dictattr("restyle","jj-pref",1);
dictattr("retain","jj-pref",1);
dictattr("retard","jj-pref",1);
dictattr("retire","jj-pref",1);
dictattr("retract","jj-pref",1);
dictattr("revere","jj-pref",1);
dictattr("reverse","jj-pref",1);
dictattr("revile","jj-pref",1);
dictattr("revise","jj-pref",1);
dictattr("rib","jj-pref",1);
dictattr("riddle","jj-pref",1);
dictattr("rig","jj-pref",1);
dictattr("roast","jj-pref",1);
dictattr("roll","jj-pref",1);
dictattr("root","jj-pref",1);
dictattr("round","jj-pref",1);
dictattr("ruffle","jj-pref",1);
dictattr("rugged","jj-pref",1);
dictattr("ruin","jj-pref",1);
dictattr("rumor","jj-pref",1);
dictattr("rumple","jj-pref",1);
dictattr("rupture","jj-pref",1);
dictattr("rush","jj-pref",1);
dictattr("sacred","jj-pref",1);
dictattr("sadden","jj-pref",1);
dictattr("salary","jj-pref",1);
dictattr("satisfy","jj-pref",1);
dictattr("saturate","jj-pref",1);
dictattr("scare","jj-pref",1);
dictattr("scarr","jj-pref",1);
dictattr("scatter","jj-pref",1);
dictattr("scent","jj-pref",1);
dictattr("schedule","jj-pref",1);
dictattr("scorch","jj-pref",1);
dictattr("score","jj-pref",1);
dictattr("scramble","jj-pref",1);
dictattr("scuttle","jj-pref",1);
dictattr("seal","jj-pref",1);
dictattr("season","jj-pref",1);
dictattr("secure","jj-pref",1);
dictattr("segment","jj-pref",1);
dictattr("segregate","jj-pref",1);
dictattr("select","jj-pref",1);
dictattr("separate","jj-pref",1);
dictattr("sequin","jj-pref",1);
dictattr("settle","jj-pref",1);
dictattr("shade","jj-pref",1);
dictattr("shape","jj-pref",1);
dictattr("share","jj-pref",1);
dictattr("shatter","jj-pref",1);
dictattr("shear","jj-pref",1);
dictattr("shellshock","jj-pref",1);
dictattr("shelter","jj-pref",1);
dictattr("shelve","jj-pref",1);
dictattr("ship","jj-pref",1);
dictattr("shock","jj-pref",1);
dictattr("short","jj-pref",1);
dictattr("shorthanded","jj-pref",1);
dictattr("shortlived","jj-pref",1);
dictattr("shortsighted","jj-pref",1);
dictattr("shred","jj-pref",1);
dictattr("shutter","jj-pref",1);
dictattr("sicken","jj-pref",1);
dictattr("sideline","jj-pref",1);
dictattr("sign","jj-pref",1);
dictattr("simplify","jj-pref",1);
dictattr("simulate","jj-pref",1);
dictattr("skew","jj-pref",1);
dictattr("skilled","jj-pref",1);
dictattr("slacken","jj-pref",1);
dictattr("slant","jj-pref",1);
dictattr("slight","jj-pref",1);
dictattr("smash","jj-pref",1);
dictattr("snap","jj-pref",1);
dictattr("socalled","jj-pref",1);
dictattr("soften","jj-pref",1);
dictattr("soil","jj-pref",1);
dictattr("solarheated","jj-pref",1);
dictattr("solicit","jj-pref",1);
dictattr("sophisticate","jj-pref",1);
dictattr("soul","jj-pref",1);
dictattr("sour","jj-pref",1);
dictattr("specialize","jj-pref",1);
dictattr("specify","jj-pref",1);
dictattr("speckle","jj-pref",1);
dictattr("spike","jj-pref",1);
dictattr("spirited","jj-pref",1);
dictattr("splinter","jj-pref",1);
dictattr("spoil","jj-pref",1);
dictattr("spot","jj-pref",1);
dictattr("spurn","jj-pref",1);
dictattr("squash","jj-pref",1);
dictattr("stagger","jj-pref",1);
dictattr("stall","jj-pref",1);
dictattr("standardize","jj-pref",1);
dictattr("state","jj-pref",1);
dictattr("stereotype","jj-pref",1);
dictattr("sterilize","jj-pref",1);
dictattr("stew","jj-pref",1);
dictattr("stiffnecked","jj-pref",1);
dictattr("stipple","jj-pref",1);
dictattr("strain","jj-pref",1);
dictattr("straitjacket","jj-pref",1);
dictattr("strap","jj-pref",1);
dictattr("stratify","jj-pref",1);
dictattr("streamline","jj-pref",1);
dictattr("strengthen","jj-pref",1);
dictattr("stretch","jj-pref",1);
dictattr("stripe","jj-pref",1);
dictattr("strip","jj-pref",1);
dictattr("structure","jj-pref",1);
dictattr("study","jj-pref",1);
dictattr("stuff","jj-pref",1);
dictattr("stun","jj-pref",1);
dictattr("stymy","jj-pref",1);
dictattr("subdue","jj-pref",1);
dictattr("subjugate","jj-pref",1);
dictattr("subordinate","jj-pref",1);
dictattr("subsidize","jj-pref",1);
dictattr("substantiate","jj-pref",1);
dictattr("sugar","jj-pref",1);
dictattr("suggest","jj-pref",1);
dictattr("supercharge","jj-pref",1);
dictattr("superconcentrate","jj-pref",1);
dictattr("supervise","jj-pref",1);
dictattr("suppose","jj-pref",1);
dictattr("suppress","jj-pref",1);
dictattr("surprise","jj-pref",1);
dictattr("suspect","jj-pref",1);
dictattr("suspend","jj-pref",1);
dictattr("sustain","jj-pref",1);
dictattr("sweeten","jj-pref",1);
dictattr("swell","jj-pref",1);
dictattr("switch","jj-pref",1);
dictattr("syndicate","jj-pref",1);
dictattr("tailor","jj-pref",1);
dictattr("taint","jj-pref",1);
dictattr("talented","jj-pref",1);
dictattr("tangle","jj-pref",1);
dictattr("tape","jj-pref",1);
dictattr("taper","jj-pref",1);
dictattr("target","jj-pref",1);
dictattr("tarnish","jj-pref",1);
dictattr("tatter","jj-pref",1);
dictattr("televise","jj-pref",1);
dictattr("tender","jj-pref",1);
dictattr("terrify","jj-pref",1);
dictattr("tether","jj-pref",1);
dictattr("texture","jj-pref",1);
dictattr("thoroughbred","jj-pref",1);
dictattr("threaten","jj-pref",1);
dictattr("thrill","jj-pref",1);
dictattr("tick","jj-pref",1);
dictattr("tighten","jj-pref",1);
dictattr("tightfisted","jj-pref",1);
dictattr("tilt","jj-pref",1);
dictattr("time","jj-pref",1);
dictattr("tint","jj-pref",1);
dictattr("tire","jj-pref",1);
dictattr("title","jj-pref",1);
dictattr("tongue","jj-pref",1);
dictattr("torment","jj-pref",1);
dictattr("torture","jj-pref",1);
dictattr("track","jj-pref",1);
dictattr("trade","jj-pref",1);
dictattr("train","jj-pref",1);
dictattr("trample","jj-pref",1);
dictattr("transfer","jj-pref",1);
dictattr("transmit","jj-pref",1);
dictattr("transplant","jj-pref",1);
dictattr("trap","jj-pref",1);
dictattr("treasure","jj-pref",1);
dictattr("try","jj-pref",1);
dictattr("trigger","jj-pref",1);
dictattr("trim","jj-pref",1);
dictattr("trouble","jj-pref",1);
dictattr("truncate","jj-pref",1);
dictattr("tune","jj-pref",1);
dictattr("tuxedo","jj-pref",1);
dictattr("twist","jj-pref",1);
dictattr("twotiered","jj-pref",1);
dictattr("unabashed","jj-pref",1);
dictattr("unabated","jj-pref",1);
dictattr("unabsorbed","jj-pref",1);
dictattr("unaccompanied","jj-pref",1);
dictattr("unaccounted","jj-pref",1);
dictattr("unaccustomed","jj-pref",1);
dictattr("unaddressed","jj-pref",1);
dictattr("unadjusted","jj-pref",1);
dictattr("unadorned","jj-pref",1);
dictattr("unaffected","jj-pref",1);
dictattr("unaffiliated","jj-pref",1);
dictattr("unaltered","jj-pref",1);
dictattr("unamended","jj-pref",1);
dictattr("unamortized","jj-pref",1);
dictattr("unamused","jj-pref",1);
dictattr("unannounced","jj-pref",1);
dictattr("unanswered","jj-pref",1);
dictattr("unanticipated","jj-pref",1);
dictattr("unapproved","jj-pref",1);
dictattr("unarmed","jj-pref",1);
dictattr("unasked","jj-pref",1);
dictattr("unassisted","jj-pref",1);
dictattr("unattended","jj-pref",1);
dictattr("unaudited","jj-pref",1);
dictattr("unauthorized","jj-pref",1);
dictattr("unbalanced","jj-pref",1);
dictattr("unbiased","jj-pref",1);
dictattr("unbleached","jj-pref",1);
dictattr("unbridled","jj-pref",1);
dictattr("uncalled","jj-pref",1);
dictattr("uncensored","jj-pref",1);
dictattr("unchallenged","jj-pref",1);
dictattr("unchanged","jj-pref",1);
dictattr("uncharted","jj-pref",1);
dictattr("unchecked","jj-pref",1);
dictattr("unclaimed","jj-pref",1);
dictattr("unclassified","jj-pref",1);
dictattr("unclothed","jj-pref",1);
dictattr("uncoached","jj-pref",1);
dictattr("uncoated","jj-pref",1);
dictattr("uncollaborated","jj-pref",1);
dictattr("uncombed","jj-pref",1);
dictattr("uncompensated","jj-pref",1);
dictattr("uncomplicated","jj-pref",1);
dictattr("unconcerned","jj-pref",1);
dictattr("unconfirmed","jj-pref",1);
dictattr("unconnected","jj-pref",1);
dictattr("unconsolidated","jj-pref",1);
dictattr("uncontested","jj-pref",1);
dictattr("uncontrolled","jj-pref",1);
dictattr("unconvinced","jj-pref",1);
dictattr("uncorrected","jj-pref",1);
dictattr("uncountered","jj-pref",1);
dictattr("uncovered","jj-pref",1);
dictattr("undamaged","jj-pref",1);
dictattr("undated","jj-pref",1);
dictattr("undaunted","jj-pref",1);
dictattr("undecided","jj-pref",1);
dictattr("undeclared","jj-pref",1);
dictattr("undefeated","jj-pref",1);
dictattr("undefined","jj-pref",1);
dictattr("undelivered","jj-pref",1);
dictattr("underappreciated","jj-pref",1);
dictattr("undercapitalized","jj-pref",1);
dictattr("underdeveloped","jj-pref",1);
dictattr("underdressed","jj-pref",1);
dictattr("undereducated","jj-pref",1);
dictattr("underemployed","jj-pref",1);
dictattr("underfunded","jj-pref",1);
dictattr("undermine","jj-pref",1);
dictattr("underprepared","jj-pref",1);
dictattr("underpriced","jj-pref",1);
dictattr("underprivileged","jj-pref",1);
dictattr("underserved","jj-pref",1);
dictattr("undersized","jj-pref",1);
dictattr("understate","jj-pref",1);
dictattr("undervalued","jj-pref",1);
dictattr("underworked","jj-pref",1);
dictattr("undeserved","jj-pref",1);
dictattr("undetected","jj-pref",1);
dictattr("undetermined","jj-pref",1);
dictattr("undeterred","jj-pref",1);
dictattr("undeveloped","jj-pref",1);
dictattr("undiluted","jj-pref",1);
dictattr("undisciplined","jj-pref",1);
dictattr("undisclosed","jj-pref",1);
dictattr("undisputed","jj-pref",1);
dictattr("undistinguished","jj-pref",1);
dictattr("undisturbed","jj-pref",1);
dictattr("undiversified","jj-pref",1);
dictattr("undivided","jj-pref",1);
dictattr("undreamed","jj-pref",1);
dictattr("unearned","jj-pref",1);
dictattr("uneducated","jj-pref",1);
dictattr("unelected","jj-pref",1);
dictattr("unemployed","jj-pref",1);
dictattr("unencumbered","jj-pref",1);
dictattr("unenlightened","jj-pref",1);
dictattr("unequaled","jj-pref",1);
dictattr("unescorted","jj-pref",1);
dictattr("unexercised","jj-pref",1);
dictattr("unexpected","jj-pref",1);
dictattr("unexpended","jj-pref",1);
dictattr("unexplained","jj-pref",1);
dictattr("unexplored","jj-pref",1);
dictattr("unfazed","jj-pref",1);
dictattr("unfertilized","jj-pref",1);
dictattr("unfettered","jj-pref",1);
dictattr("unfilled","jj-pref",1);
dictattr("unfinished","jj-pref",1);
dictattr("unfixed","jj-pref",1);
dictattr("unfocused","jj-pref",1);
dictattr("unfounded","jj-pref",1);
dictattr("unfulfilled","jj-pref",1);
dictattr("unfunded","jj-pref",1);
dictattr("unglued","jj-pref",1);
dictattr("unguaranteed","jj-pref",1);
dictattr("unguided","jj-pref",1);
dictattr("unharmed","jj-pref",1);
dictattr("unhealed","jj-pref",1);
dictattr("unheeded","jj-pref",1);
dictattr("unheralded","jj-pref",1);
dictattr("unhindered","jj-pref",1);
dictattr("unidentified","jj-pref",1);
dictattr("unify","jj-pref",1);
dictattr("uniformed","jj-pref",1);
dictattr("unimpaired","jj-pref",1);
dictattr("unimpeded","jj-pref",1);
dictattr("unimpressed","jj-pref",1);
dictattr("unimproved","jj-pref",1);
dictattr("unincorporated","jj-pref",1);
dictattr("unindicted","jj-pref",1);
dictattr("uninfected","jj-pref",1);
dictattr("uninformed","jj-pref",1);
dictattr("uninhabited","jj-pref",1);
dictattr("uninhibited","jj-pref",1);
dictattr("uninitiated","jj-pref",1);
dictattr("uninspected","jj-pref",1);
dictattr("uninspired","jj-pref",1);
dictattr("uninsured","jj-pref",1);
dictattr("unintended","jj-pref",1);
dictattr("uninterested","jj-pref",1);
dictattr("uninterrupted","jj-pref",1);
dictattr("uninvited","jj-pref",1);
dictattr("unionize","jj-pref",1);
dictattr("unissued","jj-pref",1);
dictattr("united","jj-pref",1);
dictattr("unjustified","jj-pref",1);
dictattr("unlabeled","jj-pref",1);
dictattr("unleaded","jj-pref",1);
dictattr("unlicensed","jj-pref",1);
dictattr("unlimited","jj-pref",1);
dictattr("unlinked","jj-pref",1);
dictattr("unlisted","jj-pref",1);
dictattr("unloaded","jj-pref",1);
dictattr("unmanned","jj-pref",1);
dictattr("unmarked","jj-pref",1);
dictattr("unmarried","jj-pref",1);
dictattr("unmatched","jj-pref",1);
dictattr("unmoved","jj-pref",1);
dictattr("unnamed","jj-pref",1);
dictattr("unneeded","jj-pref",1);
dictattr("unnerve","jj-pref",1);
dictattr("unnoticed","jj-pref",1);
dictattr("unnumbered","jj-pref",1);
dictattr("unobserved","jj-pref",1);
dictattr("unoccupied","jj-pref",1);
dictattr("unopened","jj-pref",1);
dictattr("unopposed","jj-pref",1);
dictattr("unorganized","jj-pref",1);
dictattr("unpack","jj-pref",1);
dictattr("unparalleled","jj-pref",1);
dictattr("unpaved","jj-pref",1);
dictattr("unpegged","jj-pref",1);
dictattr("unperformed","jj-pref",1);
dictattr("unperturbed","jj-pref",1);
dictattr("unplanned","jj-pref",1);
dictattr("unpolished","jj-pref",1);
dictattr("unprecedented","jj-pref",1);
dictattr("unprepared","jj-pref",1);
dictattr("unprotected","jj-pref",1);
dictattr("unpublicized","jj-pref",1);
dictattr("unpublished","jj-pref",1);
dictattr("unpunctured","jj-pref",1);
dictattr("unpunished","jj-pref",1);
dictattr("unqualified","jj-pref",1);
dictattr("unraveled","jj-pref",1);
dictattr("unrealized","jj-pref",1);
dictattr("unrecognized","jj-pref",1);
dictattr("unrefrigerated","jj-pref",1);
dictattr("unregistered","jj-pref",1);
dictattr("unregulated","jj-pref",1);
dictattr("unreinforced","jj-pref",1);
dictattr("unrelated","jj-pref",1);
dictattr("unreleased","jj-pref",1);
dictattr("unreported","jj-pref",1);
dictattr("unresolved","jj-pref",1);
dictattr("unrestrained","jj-pref",1);
dictattr("unrestricted","jj-pref",1);
dictattr("unretouched","jj-pref",1);
dictattr("unrivaled","jj-pref",1);
dictattr("unsanctioned","jj-pref",1);
dictattr("unsatisfied","jj-pref",1);
dictattr("unscathed","jj-pref",1);
dictattr("unscheduled","jj-pref",1);
dictattr("unscripted","jj-pref",1);
dictattr("unseat","jj-pref",1);
dictattr("unsecured","jj-pref",1);
dictattr("unsettled","jj-pref",1);
dictattr("unshackled","jj-pref",1);
dictattr("unshirted","jj-pref",1);
dictattr("unsigned","jj-pref",1);
dictattr("unskilled","jj-pref",1);
dictattr("unsolicited","jj-pref",1);
dictattr("unsolved","jj-pref",1);
dictattr("unspecified","jj-pref",1);
dictattr("unspoiled","jj-pref",1);
dictattr("unstated","jj-pref",1);
dictattr("unstimulated","jj-pref",1);
dictattr("unstructured","jj-pref",1);
dictattr("unsubordinated","jj-pref",1);
dictattr("unsubsidized","jj-pref",1);
dictattr("unsubstantiated","jj-pref",1);
dictattr("unsupported","jj-pref",1);
dictattr("unsurpassed","jj-pref",1);
dictattr("unsurprised","jj-pref",1);
dictattr("unsuspected","jj-pref",1);
dictattr("unswagged","jj-pref",1);
dictattr("untamed","jj-pref",1);
dictattr("untapped","jj-pref",1);
dictattr("untarnished","jj-pref",1);
dictattr("untested","jj-pref",1);
dictattr("untouched","jj-pref",1);
dictattr("untrained","jj-pref",1);
dictattr("untreated","jj-pref",1);
dictattr("untried","jj-pref",1);
dictattr("unturned","jj-pref",1);
dictattr("unused","jj-pref",1);
dictattr("unveiled","jj-pref",1);
dictattr("unwanted","jj-pref",1);
dictattr("unwarranted","jj-pref",1);
dictattr("unwashed","jj-pref",1);
dictattr("unwed","jj-pref",1);
dictattr("update","jj-pref",1);
dictattr("upend","jj-pref",1);
dictattr("upgrade","jj-pref",1);
dictattr("use","jj-pref",1);
dictattr("value","jj-pref",1);
dictattr("vary","jj-pref",1);
dictattr("vaunt","jj-pref",1);
dictattr("ventilate","jj-pref",1);
dictattr("vest","jj-pref",1);
dictattr("vex","jj-pref",1);
dictattr("victimize","jj-pref",1);
dictattr("voice","jj-pref",1);
dictattr("wall","jj-pref",1);
dictattr("want","jj-pref",1);
dictattr("warmhearted","jj-pref",1);
dictattr("warp","jj-pref",1);
dictattr("warrant","jj-pref",1);
dictattr("waste","jj-pref",1);
dictattr("watch","jj-pref",1);
dictattr("waterlog","jj-pref",1);
dictattr("weaken","jj-pref",1);
dictattr("weakwilled","jj-pref",1);
dictattr("weight","jj-pref",1);
dictattr("wellplaced","jj-pref",1);
dictattr("whalesized","jj-pref",1);
dictattr("whipsaw","jj-pref",1);
dictattr("whitewalled","jj-pref",1);
dictattr("wholehearted","jj-pref",1);
dictattr("wicked","jj-pref",1);
dictattr("widow","jj-pref",1);
dictattr("wilt","jj-pref",1);
dictattr("wing","jj-pref",1);
dictattr("word","jj-pref",1);
dictattr("worry","jj-pref",1);
dictattr("wound","jj-pref",1);
dictattr("wretched","jj-pref",1);
dictattr("wrinkle","jj-pref",1);

########################################
# SOFT PLURALS
########################################
# Can participate in noun noun phrases.
# Ex: A futures contract.
dictattr("funds","soft-plural",1);
dictattr("futures","soft-plural",1);
dictattr("securities","soft-plural",1);

########################################
# COMMON CAPS/ABBRS
########################################
# Common abbreviations or capitals that
# act like reqular words.
dictattr("pc","common-cap",1);
dictattr("pcs","common-cap",1);


# FLAG THAT KB HAS BEEN LOADED.
# 05/16/07 AM. Moved here to assure pass completed.
replaceval(findroot(),"kb loaded",1);

@@CODE
