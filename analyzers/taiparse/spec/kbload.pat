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
finddictattr(L("str"),L("attr"))
{
if (!L("str") || !L("attr"))
  return 0;
L("str") = strtolower(L("str"));
L("con") = dictgetword(L("str"));
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
else
  replaceval(findroot(),"kb loaded",1);

########################################
# Verbs that require a human-like agent.
########################################
# eg, "Moose said" -- Moose would be an NP.
dictattr("announce","agent-human",1);
dictattr("say","agent-human",1);

########################################
# Verbs that take scalar change.
########################################
# eg, "rise 10%".
dictattr("rise","vscalar",1);
dictattr("fall","vscalar",1);

########################################
# Plain adjectives with comparative, superlative endings.
########################################
# not-er, not-est, plain-adj.
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
dictattr("later","adv-plain",1);	# 04/19/07 AM.

dictattr("moreover","adv-plain",1);
dictattr("never","adv-plain",1);
dictattr("rather","adv-plain",1);
dictattr("together","adv-plain",1);
dictattr("thereafter","adv-plain",1);

########################################
# VERB + OF
########################################
dictattr("think","verb-of",1);
dictattr("tire","verb-of",1);


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

########################################
# INTRANSITIVE VERBS
########################################
# Todo: distinguish semantically and syntactically intransitive.
# (eg, "I think" implies that one is thinking about something.
#  i.e., syntax=intr, sem=trans)
dictattr("grow","intr",1);
dictattr("sit","intr",1);
dictattr("walk","intr",1);

########################################
# VNJ VERBS
########################################
# Ex: keep it real.
# Ex: paint the house purple.
dictattr("hold","vnj",1);
dictattr("keep","vnj",1);
dictattr("make","vnj",1);
dictattr("paint","vnj",1);

########################################
# VNN VERBS
########################################
# Ex: give John an apple.
dictattr("ask","vnn",1);
dictattr("give","vnn",1);
dictattr("make","vnn",1);
dictattr("paint","vnn",1);

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

# T3 = V + TO + V
dictattr("agree","T3",1);
dictattr("ask","T3",1);
dictattr("claim","T3",1);
dictattr("decide","T3",1);
dictattr("deserve","T3",1);
dictattr("desire","T3",1);
dictattr("expect","T3",1);
dictattr("go","T3",1);
dictattr("hope","T3",1);
dictattr("need","T3",1);
dictattr("promise","T3",1);
dictattr("want","T3",1);

# T4 = verb + ving (start eating)
# Also: cannot stand ving.
# Also: keep (on) ving.
dictattr("acknowledge","T4",1);
dictattr("avoid","T4",1);
dictattr("consider","T4",1);
dictattr("deny","T4",1);
dictattr("detest","T4",1);
dictattr("dislike","T4",1);
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
dictattr("get","T4",1);
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
dictattr("keep","V4",1);
dictattr("feel","V4",1);
dictattr("hate","V4",1);
dictattr("hear","V4",1);
dictattr("like","V4",1);
dictattr("observe","V4",1);
dictattr("see","V4",1);
dictattr("sense","V4",1);
dictattr("smell","V4",1);
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
dictattr("discover","T5",1);
dictattr("doubt","T5",1);
dictattr("feel","T5",1);
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
dictattr("chew","off",1);
dictattr("churn","out",1);
dictattr("clamp","down",1);
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
dictattr("curl","up",1);	# ?
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
dictattr("hang","out",1);
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
dictattr("look","out",1);
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
dictattr("set","off",1);
dictattr("set","out",1);
dictattr("set","up",1);
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
dictattr("take","out",1);
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
dictattr("act","up",2);
dictattr("ante","up",2);
#dictattr("bark","up",2);	# ?
dictattr("back","off",2);	# also 1?
dictattr("bleed","off",2);	# ? 1 ?
dictattr("bone","up",2);
dictattr("bound","up",2);
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
dictattr("live","off",2);	# ?
dictattr("live","up",2);
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
dictattr("spring","up",2);
dictattr("sprout","up",2);
dictattr("square","off",2);	# also 1?
dictattr("stay","up",2);
dictattr("stop","off",2);	# ? # 1?
#dictattr("stride","up",2); # to...
dictattr("suit","up",2);
dictattr("swear","off",2);
dictattr("tail","off",2);	# ?
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
dictattr("dash","off",3);	# dash a letter off
dictattr("run","off",3);
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

@@CODE
