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


def main():
    if len(sys.argv) != 4:
        print(__doc__)
        return 2
    nlp, fixture, workdir = sys.argv[1], sys.argv[2], sys.argv[3]
    text = os.path.join(fixture, "input", "text.txt")
    port = free_port()

    proc = subprocess.Popen(
        [nlp, "-ANA", fixture, "-IN", text, "-WORK", workdir, "-DEBUG", str(port)],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    sock = None
    deadline = time.time() + 60
    while time.time() < deadline:
        try:
            sock = socket.create_connection(("127.0.0.1", port), timeout=5)
            break
        except OSError:
            if proc.poll() is not None:
                out = proc.stdout.read().decode("utf-8", "replace")
                print("FAIL: the engine exited before listening on %d\n%s" % (port, out[-2000:]))
                return 1
            time.sleep(0.25)
    if sock is None:
        proc.kill()
        print("FAIL: could not reach the debug port -- does this build have -DEBUG?")
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

        # ---- stepping to the rule pass --------------------------------------
        dbg.request("stepPass")
        stop = dbg.next_stop()
        eq("stepPass reaches pass 2", stop.get("pass"), 2)
        check("pass 2 is the rules pass",
              "numbers" in (stop.get("passName") or ""),
              "passName was %r" % stop.get("passName"))

        # ---- a breakpoint on the rule that fires ----------------------------
        # Line 29 is the head of `_num <- _xNUM`, the only rule in the fixture
        # that matches the input.
        r = dbg.request("setBreakpoints", **{"pass": 2, "lines": [29]})
        check("setBreakpoints is accepted", r.get("ok") is True)
        dbg.request("continue")
        stop = dbg.next_stop()
        check("the breakpoint is reached", stop is not None)
        if stop is None:
            return 1
        eq("stopped for the breakpoint", stop.get("reason"), "breakpoint")
        eq("stopped on the breakpoint's line", stop.get("line"), 29)

        # ---- the rule the engine reports is the one in the file -------------
        r = dbg.request("rule")
        rule = r.get("rule") or {}
        eq("rule line", rule.get("line"), 29)
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

        # ---- failures report how far the rule got ---------------------------
        # _pair (line 18) matches _xNUM and then fails, because the node after
        # "42" is whitespace. That is the case worth pinning: the count comes
        # off the collect list, which is a sibling chain, and reading it as a
        # child list instead returns whatever the first matched node spans --
        # for a wildcard, most of the document.
        #
        # _zzz (line 24) is never reported at all. Its trigger is the literal
        # "zzz", and the engine's rule hashing only tries a rule at nodes its
        # trigger could match, so a rule that cannot fire is never attempted.
        # Worth asserting: "my rule never appears in the debugger" is a thing
        # an author will hit, and this is the reason.
        dbg.request("setBreakpoints", **{"pass": 2, "lines": []})
        dbg.request("stopOnFailure", value=True)
        partial = 0
        zzz_seen = False
        worst = 0
        ended = False
        for _ in range(400):
            dbg.request("stepRule")
            stop = dbg.next_stop()
            if stop is None:
                ended = True
                break
            if stop.get("reason") != "failed":
                continue
            worst = max(worst, stop.get("eltsMatched", 0))
            if stop.get("line") == 24:
                zzz_seen = True
            if stop.get("line") == 18:
                # _pair is tried at every node its trigger could match. At the
                # number it gets one element in before the whitespace after it
                # fails; everywhere else it collects nothing. The best it ever
                # does is the interesting number.
                partial = max(partial, stop.get("eltsMatched", 0))

        check("a partly-matched rule reports its element count", partial == 1,
              "best eltsMatched for line 18 was %r" % partial)
        check("a rule whose trigger cannot match is never attempted", not zzz_seen)
        # The bug this guards: counting the collect list as children of a root
        # rather than as siblings reported 526 for a two-element rule.
        check("no failure reports more elements than a rule can have", worst <= 4,
              "largest eltsMatched seen was %d" % worst)

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

    if FAILURES:
        print("\n%d assertion(s) failed: %s" % (len(FAILURES), ", ".join(FAILURES)))
        return 1
    print("\nrule debug server: all assertions passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
