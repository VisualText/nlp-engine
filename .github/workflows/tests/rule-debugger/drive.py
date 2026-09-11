#!/usr/bin/env python3
"""Regression test for the engine's rule-level debug server (-DEBUG <port>).

Runs the fixture analyzer under the debugger and asserts on what the engine
reports at each pause point. What this is really pinning:

  * the pause points fire at all, and in the interpreted pass path -- the hooks
    sit in the hottest loop in the engine, so they are exactly the kind of thing
    a later optimisation removes without anyone noticing;
  * the debugger is ARMED only for the analysis. Before it analyses anything the
    engine parses its own grammar with the same Pat code, and an un-armed build
    stops hundreds of times on rules nobody wrote;
  * a failed rule reports how far it got. That count comes off the collect list,
    which is a sibling chain rather than a child list -- getting it wrong reads
    the whole document instead of the rule;
  * pass numbers and names line up with the .tree dumps, which is what makes a
    breakpoint land on the line the user set it on.

Usage:  python3 drive.py <path-to-nlp> <fixture-dir> <work-dir>
Exits 0 on success, 1 with a message on the first failed assertion.
"""

import json
import os
import socket
import subprocess
import sys
import time

FAILURES = []


def check(name, cond, detail=""):
    if cond:
        print("  ok   %s" % name)
    else:
        FAILURES.append(name)
        print("  FAIL %s%s" % (name, (" -- " + detail) if detail else ""))


def eq(name, actual, expected):
    check(name, actual == expected, "expected %r, got %r" % (expected, actual))


def free_port():
    s = socket.socket()
    s.bind(("127.0.0.1", 0))
    port = s.getsockname()[1]
    s.close()
    return port


class Debugger(object):
    def __init__(self, sock):
        self.sock = sock
        self.buf = b""
        self.seq = 0

    def _readline(self, timeout=60):
        self.sock.settimeout(timeout)
        while b"\n" not in self.buf:
            chunk = self.sock.recv(65536)
            if not chunk:
                raise EOFError("the engine closed the connection")
            self.buf += chunk
        line, self.buf = self.buf.split(b"\n", 1)
        return json.loads(line.decode("utf-8"))

    def next_stop(self, timeout=60):
        """Read until a stopped event; anything else on the way is ignored."""
        while True:
            msg = self._readline(timeout)
            if msg.get("event") == "stopped":
                return msg
            if msg.get("event") == "terminated":
                return None

    def request(self, command, **extra):
        self.seq += 1
        payload = dict(extra)
        payload["seq"] = self.seq
        payload["command"] = command
        self.sock.sendall((json.dumps(payload) + "\n").encode("utf-8"))
        while True:
            msg = self._readline()
            if msg.get("seq") == self.seq:
                return msg
            if msg.get("event") == "terminated":
                raise EOFError("the engine ended while awaiting %s" % command)


def attach(nlp, fixture, workdir):
    """Start the engine under -DEBUG and connect. Returns (proc, sock) or None.

    Two sessions need this. Every step consumes part of the analysis, so a
    section that walks the pass to its end cannot share a run with one that
    needs to stop partway -- each would be looking at whatever the other left
    behind. A second engine costs a second and keeps them independent.
    """
    text = os.path.join(fixture, "input", "text.txt")
    port = free_port()
    proc = subprocess.Popen(
        [nlp, "-ANA", fixture, "-IN", text, "-WORK", workdir, "-DEBUG", str(port)],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    deadline = time.time() + 60
    while time.time() < deadline:
        try:
            return proc, socket.create_connection(("127.0.0.1", port), timeout=5)
        except OSError:
            if proc.poll() is not None:
                out = proc.stdout.read().decode("utf-8", "replace")
                print("FAIL: the engine exited before listening on %d" % port)
                print(out[-2000:])
                return None
            time.sleep(0.25)
    proc.kill()
    print("FAIL: could not reach the debug port -- does this build have -DEBUG?")
    return None


def fixtureLines(fixture):
    """The fixture's own line numbers, read rather than hardcoded.

    The fixture gains a block now and then; every literal line number would
    have to be chased when it does.
    """
    spec = os.path.join(fixture, "spec", "numbers.nlp")
    lines = {}
    with open(spec, encoding="utf-8") as fh:
        for i, text in enumerate(fh, start=1):
            stripped = text.strip()
            for rule in ("_pair", "_zzz", "_num"):
                if stripped.startswith(rule + " <-"):
                    lines[rule] = i
            if stripped.startswith("bumpRuns(10)"):
                lines["call"] = i
            if stripped.startswith('G("runs") = G("runs")'):
                lines["fnbody"] = i
            marker = text.split("### shape:")
            if len(marker) == 2:
                lines["shape:" + marker[1].strip()] = i
    return lines


def statements(nlp, fixture, workdir, LINES):
    """Second session: @POST/@CODE/@DECL statements, and stepping into a call.

    Its own engine because it walks the pass past the point the failure
    traversal needs to stop at.
    """
    started = attach(nlp, fixture, workdir)
    if started is None:
        return 1
    proc, sock = started
    dbg = Debugger(sock)
    try:
        # ---- statements, and stepping into a function ------------------------
        # @CODE, @POST and @DECL bodies all run through Istmt::eval, so one hook
        # covers all three. Line 45 is the call `bumpRuns(10);` in @POST; lines
        # 23-24 are inside the function it calls.
        #
        # Depth is what makes the three step commands mean anything: INTO
        # descends into a call, OUT runs until the function returns. It comes
        # from nlppp->getDepth(), which Ifunc::eval pushes and pops.
        dbg.request("setBreakpoints", **{"pass": 2, "lines": [LINES["call"]]})
        dbg.request("continue")
        stop = dbg.next_stop()
        check("a breakpoint inside @POST stops", stop is not None)
        if stop is not None:
            eq("stopped for a statement", stop.get("reason"), "statement")
            eq("on the line the breakpoint was set", stop.get("line"), LINES["call"])
            eq("statements are flagged as such", stop.get("statement"), True)
            eq("at the outermost depth", stop.get("depth"), 0)

            # Step INTO the call.
            dbg.request("stepStatement")
            inside = dbg.next_stop()
            check("stepping enters the function body", inside is not None)
            if inside is not None:
                # A stop is BEFORE the statement on that line runs, which is
                # what makes stepping worth anything -- the state you read is
                # the state the statement is about to act on.
                eq("the stop is on the function's first statement",
                   inside.get("line"), LINES["fnbody"])
                eq("depth records the call", inside.get("depth"), 1)

                # The function's own parameter is a local, and the global it is
                # about to update reads with the value the caller left.
                locs = dict((v.get("name"), v.get("value"))
                            for v in (dbg.request("locals").get("locals") or []))
                check("a function parameter reads as a local", locs.get("by") == "10",
                      "locals were %r" % (locs,))
                globs = dict((v.get("name"), v.get("value"))
                             for v in (dbg.request("globals").get("globals") or []))
                check("a global reads its pre-statement value", globs.get("runs") == "1",
                      "globals were %r" % (globs,))

                # One more statement, and the assignment has happened.
                dbg.request("stepStatement")
                after = dbg.next_stop()
                check("stepping stays inside the function", after is not None)
                if after is not None:
                    eq("on the function's next line", after.get("line"),
                       LINES["fnbody"] + 1)
                    eq("still one call deep", after.get("depth"), 1)
                    globs = dict((v.get("name"), v.get("value"))
                                 for v in (dbg.request("globals").get("globals") or []))
                    check("the function's write to a global is visible",
                          globs.get("runs") == "11", "globals were %r" % (globs,))

                # Step OUT returns to the caller, past the call.
                dbg.request("stepOutStatement")
                back = dbg.next_stop()
                check("stepping out returns to the caller", back is not None)
                if back is not None:
                    eq("back at the outermost depth", back.get("depth"), 0)
                    check("on a line after the call",
                          (back.get("line") or 0) > LINES["call"],
                          "line was %r" % back.get("line"))

                    # Step OUT again, now at depth 0. There is no function to
                    # leave: what encloses an @POST is the rule stream, so this
                    # has to land on a rule. Asking for a depth shallower than 0
                    # is asking for something that never arrives, and the whole
                    # analysis ran to the end -- Step Out behaving as Continue.
                    dbg.request("stepOutStatement")
                    out = dbg.next_stop()
                    check("stepping out of a top-level statement does not run away",
                          out is not None)
                    if out is not None:
                        check("it lands back among the rules",
                              not out.get("statement"),
                              "stopped at %r" % (out,))
    finally:
        try:
            sock.close()
        except OSError:
            pass
        try:
            proc.wait(timeout=30)
        except Exception:
            proc.kill()
    return 0


def blockShapes(nlp, fixture, workdir, LINES):
    """Third session: which lines a walk through every block shape stops on.

    A block holding exactly ONE statement is kept as a bare statement rather
    than a one-element list, and only the list path carries the pause point.
    So a single-statement `if` body -- much the commonest shape there is -- was
    stepped straight over as though it were not there, and so was a
    single-statement `while` body and a single-statement `else`. Every shape is
    walked here because they differ in the engine, not just on the page.
    """
    started = attach(nlp, fixture, workdir)
    if started is None:
        return 1
    proc, sock = started
    dbg = Debugger(sock)
    try:
        dbg.request("setBreakpoints", **{"pass": 2, "lines": [LINES["shape:call"]]})
        dbg.request("continue")
        stop = dbg.next_stop()
        check("the call into blockShapes is reached", stop is not None)
        if stop is None:
            return 1
        dbg.request("setBreakpoints", **{"pass": 2, "lines": []})

        seen = []
        for _ in range(60):
            dbg.request("stepStatement")
            s = dbg.next_stop()
            if s is None or not s.get("statement") or s.get("depth") == 0:
                break
            seen.append(s.get("line"))

        eq("a single-statement `if` body stops", seen.count(LINES["shape:one"]), 1)
        eq("a two-statement body still stops on both",
           (seen.count(LINES["shape:twoA"]), seen.count(LINES["shape:twoB"])), (1, 1))
        eq("a single-statement `else` body stops", seen.count(LINES["shape:elsed"]), 1)
        # Once per iteration: the body is reached through the same path each
        # time round, so a loop is also the check that the hook is not somehow
        # one-shot.
        eq("a single-statement `while` body stops once per iteration",
           seen.count(LINES["shape:loop"]), 2)
        # Once on the way in, and once after each pass through the body. The
        # last of those is the test that ends the loop, so it is reported
        # before control leaves -- the condition is a line of the program and
        # stepping should land on it. Without this the body's last statement
        # steps straight back to the body's first, which reads as a loop with
        # no test in it.
        eq("the loop's test is reported before every attempt",
           seen.count(LINES["shape:while"]), 3)
        # An `if` with no braces and its body on the next line. The statement
        # took its line from the BODY, so the two collided: stepping went from
        # the statement before the `if` straight to the body, the test never
        # appeared, and an error in the condition was reported a line late.
        # Both lines have to show up, and separately.
        eq("an unbraced `if` reports its own line", seen.count(LINES["shape:bareIf"]), 1)
        eq("and its body reports the body's", seen.count(LINES["shape:bareBody"]), 1)
        check("the test is reported before the body it guards",
              LINES["shape:bareIf"] < LINES["shape:bareBody"]
              and seen.index(LINES["shape:bareIf"]) < seen.index(LINES["shape:bareBody"]),
              "stops were %r" % (seen,))

        # The counterpart: a body whose condition is false must stay silent.
        # Stopping on every branch whether taken or not would look like
        # stepping working while telling the author the wrong story.
        eq("a body whose condition is false does not stop",
           seen.count(LINES["shape:never"]), 0)
    finally:
        try:
            sock.close()
        except OSError:
            pass
        try:
            proc.wait(timeout=30)
        except Exception:
            proc.kill()
    return 0


def main():
    if len(sys.argv) != 4:
        print(__doc__)
        return 2
    nlp, fixture, workdir = sys.argv[1], sys.argv[2], sys.argv[3]
    started = attach(nlp, fixture, workdir)
    if started is None:
        return 1
    proc, sock = started

    LINES = fixtureLines(fixture)
    for want in ("_pair", "_zzz", "_num", "call", "fnbody",
                 "shape:call", "shape:one", "shape:never", "shape:twoA",
                 "shape:twoB", "shape:elsed", "shape:loop", "shape:while",
                 "shape:bareIf", "shape:bareBody"):
        if want not in LINES:
            print("FAIL: could not find %s in the fixture" % want)
            return 1

    dbg = Debugger(sock)
    try:
        # ---- the first stop is the first pass of the ANALYSIS ---------------
        # Not a rule from the engine's own grammar: those run before the hooks
        # are armed, and if arming regressed this assertion is what catches it.
        stop = dbg.next_stop()
        check("attaches and stops", stop is not None)
        if stop is None:
            return 1
        eq("first stop is a pass boundary", stop.get("reason"), "passStart")
        eq("first stop is pass 1", stop.get("pass"), 1)
        check("pass 1 is the tokenizer",
              "tokenize" in (stop.get("passName") or ""),
              "passName was %r" % stop.get("passName"))

        # ---- what this build says it can do ---------------------------------
        # A client pairs with whatever engine the user has installed, so it asks
        # rather than reading a version string. Dropping a name from this list
        # silently turns a feature off in the extension.
        caps = dbg.request("capabilities").get("capabilities") or []
        for want in ("statements", "variables", "nodeText"):
            check("capabilities include %s" % want, want in caps,
                  "capabilities were %r" % (caps,))

        # ---- stepping to the rule pass --------------------------------------
        dbg.request("stepPass")
        stop = dbg.next_stop()
        eq("stepPass reaches pass 2", stop.get("pass"), 2)
        check("pass 2 is the rules pass",
              "numbers" in (stop.get("passName") or ""),
              "passName was %r" % stop.get("passName"))

        # ---- a breakpoint on the rule that fires ----------------------------
        # The head of `_num <- _xNUM`, the only rule in the fixture
        # that matches the input.
        r = dbg.request("setBreakpoints", **{"pass": 2, "lines": [LINES["_num"]]})
        check("setBreakpoints is accepted", r.get("ok") is True)
        dbg.request("continue")
        stop = dbg.next_stop()
        check("the breakpoint is reached", stop is not None)
        if stop is None:
            return 1
        eq("stopped for the breakpoint", stop.get("reason"), "breakpoint")
        eq("stopped on the breakpoint's line", stop.get("line"), LINES["_num"])

        # ---- the rule the engine reports is the one in the file -------------
        r = dbg.request("rule")
        rule = r.get("rule") or {}
        eq("rule line", rule.get("line"), LINES["_num"])
        eq("rule builds _num", rule.get("builds"), "_num")
        eq("rule element count", len(rule.get("elements") or []), 1)
        eq("rule element name", (rule.get("elements") or [{}])[0].get("name"), "_xNUM")

        # ---- the tree is walkable mid-pass ----------------------------------
        r = dbg.request("tree", depth=1)
        tree = r.get("tree") or {}
        eq("tree root", tree.get("name"), "_ROOT")
        check("tree root has children", len(tree.get("children") or []) > 0)

        r = dbg.request("node")
        node = r.get("node") or {}
        check("current node is reported", bool(node.get("name")),
              "node was %r" % node)

        # The node carries the text it covers, sent from the engine's own buffer.
        # The client used to slice it out of the input file with start/end, which
        # drifts the moment those offsets stop agreeing with the file on disk --
        # and they do, because the buffer has line endings normalised. On a CRLF
        # file every node past the first line came out shifted by one character
        # per preceding line.
        check("the node carries its own text", isinstance(node.get("text"), str),
              "text was %r" % node.get("text"))

        # depth reaches past the immediate children, and `after` brings the nodes
        # a rule will be tried against next -- a rule matches a SEQUENCE.
        r = dbg.request("node", depth=3, after=4)
        following = r.get("following")
        check("following siblings are returned as a list",
              isinstance(following, list), "got %r" % (following,))
        if isinstance(following, list) and following:
            check("a following node carries a name and text",
                  bool(following[0].get("name")) and isinstance(following[0].get("text"), str),
                  "first following was %r" % (following[0],))
            starts = [n.get("start") for n in following]
            check("following nodes come back in document order",
                  starts == sorted(starts), "starts were %r" % (starts,))

        # ---- variables -------------------------------------------------------
        # Every NLP++ variable kind is one Dlist<Ipair> in the engine, so one
        # serializer covers them all -- which also means one broken field name
        # silently returns an empty list. These assert the plumbing per kind.
        #
        # We are stopped at the _num rule ATTEMPT, before its @POST has run, so
        # G("runs") is not set yet. Stepping to the match and on lets the actions
        # execute; the values are then readable and the built node keeps "kind"
        # as an attribute.
        r = dbg.request("collect")
        coll = r.get("collect")
        check("collect is a list", isinstance(coll, list), "got %r" % (coll,))

        r = dbg.request("globals")
        check("globals is a list", isinstance(r.get("globals"), list))
        r = dbg.request("locals")
        check("locals is a list", isinstance(r.get("locals"), list))
        r = dbg.request("suggested")
        check("suggested is a list", isinstance(r.get("suggested"), list))
        r = dbg.request("context")
        check("context is a list", isinstance(r.get("context"), list))

        # One traversal gathers both what the rules SET and how they FAILED.
        # They cannot be two passes over the pass: each stepRule consumes part of
        # the traversal, so a second loop would start after the nodes the first
        # one walked past -- and _pair only gets an element in at the number.
        #
        # _pair (line 20) matches _xNUM at "42" and then fails, because the node
        # after it is whitespace. That count comes off the collect list, which is
        # a sibling chain; reading it as a child list returns whatever the first
        # matched node spans -- for a wildcard, most of the document.
        #
        # _zzz (line 26) is never reported at all. Its trigger is the literal
        # "zzz", and the engine only tries a rule at nodes its trigger could
        # match, so a rule that cannot fire is never attempted. Worth pinning:
        # "my rule never appears in the debugger" is a thing an author will hit.
        dbg.request("setBreakpoints", **{"pass": 2, "lines": []})
        dbg.request("stopOnFailure", value=True)

        partial = 0          # best eltsMatched seen for _pair
        worst = 0            # largest eltsMatched seen for ANY failure
        zzz_seen = False
        names = {}
        attrs_found = []
        ended = False

        for _ in range(400):
            dbg.request("stepRule")
            stop = dbg.next_stop()
            if stop is None:
                ended = True
                break

            if stop.get("reason") == "failed":
                worst = max(worst, stop.get("eltsMatched", 0))
                if stop.get("line") == LINES["_pair"]:
                    partial = max(partial, stop.get("eltsMatched", 0))
                if stop.get("line") == LINES["_zzz"]:
                    zzz_seen = True

            # Once _num's @POST has run, G("runs") is set and the node it built
            # carries S("kind") as an attribute.
            if not names.get("runs"):
                globs = dbg.request("globals").get("globals") or []
                names = dict((g.get("name"), g.get("value")) for g in globs)
                if "runs" in names:
                    tree = dbg.request("tree", depth=3).get("tree") or {}

                    def walk(n):
                        for a in n.get("attributes") or []:
                            attrs_found.append((n.get("name"), a.get("name"), a.get("value")))
                        for c in n.get("children") or []:
                            walk(c)

                    walk(tree)

            if partial and names.get("runs") and attrs_found:
                break

        check("a global set by a rule is readable", "runs" in names,
              "globals were %r (run ended: %s)" % (names, ended))
        # 1 from the assignment, +10 from bumpRuns(): the function's effect on a
        # global outlives the call, which is what makes stepping through one useful.
        eq("the global carries the function's effect too", names.get("runs"), "11")
        check("a node carries the attribute its rule set",
              any(nm == "_num" and k == "kind" for nm, k, _v in attrs_found),
              "attributes found: %r" % (attrs_found,))
        check("the attribute keeps its value",
              any(k == "kind" and v == '"number"' for _nm, k, v in attrs_found),
              "attributes found: %r" % (attrs_found,))
        check("a partly-matched rule reports its element count", partial == 1,
              "best eltsMatched for _pair was %r" % partial)
        check("a rule whose trigger cannot match is never attempted", not zzz_seen)
        # The bug this guards: counting the collect list as children of a root
        # rather than as siblings reported 526 for a two-element rule.
        check("no failure reports more elements than a rule can have", worst <= 4,
              "largest eltsMatched seen was %d" % worst)

        if not ended:
            dbg.request("stopOnFailure", value=False)

        # ---- the run finishes ------------------------------------------------
        if not ended:
            dbg.request("stopOnFailure", value=False)
            dbg.request("continue")
            for _ in range(2000):
                try:
                    msg = dbg._readline(timeout=60)
                except EOFError:
                    ended = True
                    break
                if msg.get("event") == "terminated":
                    ended = True
                    break
                if msg.get("event") == "stopped":
                    dbg.request("continue")
        check("the run terminates under the debugger", ended)

    finally:
        try:
            sock.close()
        except OSError:
            pass
        try:
            proc.wait(timeout=30)
        except Exception:
            proc.kill()

    if statements(nlp, fixture, workdir, LINES):
        return 1

    if blockShapes(nlp, fixture, workdir, LINES):
        return 1

    if FAILURES:
        print("\n%d assertion(s) failed: %s" % (len(FAILURES), ", ".join(FAILURES)))
        return 1
    print("\nrule debug server: all assertions passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
