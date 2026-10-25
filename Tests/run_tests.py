#!/usr/bin/env python3
"""Regression test runner for HU-Prolog.

Runs every NN_*.pl file in this directory against a huprolog binary,
parses the PASS/FAIL/SUMMARY lines each test file prints (see
harness.pl), and reports an overall summary. Exits non-zero if any
check failed, any test file's own summary count didn't match what was
observed, or huprolog crashed or never reached a SUMMARY line.

Usage:
    python3 run_tests.py [--binary PATH] [TEST_FILE ...]

With no TEST_FILE arguments, every Tests/[0-9]*.pl file is run in
sorted order (00_smoke.pl is excluded by default via --include-smoke).
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
DEFAULT_BINARY = HERE.parent / "Sources" / "huprolog"

PASS_RE = re.compile(r"^PASS (.+)$")
FAIL_RE = re.compile(r"^FAIL (.+?) -- (.*)$")
SUMMARY_RE = re.compile(r"^SUMMARY passed=(\d+) failed=(\d+)$")

# Matches a call to any harness.pl assertion predicate, so we can
# count how many checks a file *should* produce and catch the failure
# mode where a check silently vanishes: if some goal BEFORE a check(),
# check_eq(), check_not() or check_error() call fails in the same
# conjunction, the whole directive aborts before the check predicate
# ever runs -- no PASS, no FAIL, nothing. Comparing this static count
# against the runtime passed+failed total catches that class of bug.
# (It undercounts if a file defines a *helper* clause whose head looks
# like e.g. "check_eq(...)" -- none of these files do that.)
CHECK_CALL_RE = re.compile(r'\b(check|check_eq|check_not|check_error)\(')


def count_expected_checks(test_file: Path) -> int:
    text = test_file.read_text()
    # Strip line comments (a naive but sufficient strip for this
    # suite: no '%' appears inside a quoted atom/string in any test
    # file's check(...) lines).
    stripped_lines = [line.split('%', 1)[0] for line in text.splitlines()]
    return sum(len(CHECK_CALL_RE.findall(line)) for line in stripped_lines)


def report(char):
    print(char, end="", flush=True)


def run_one(binary: Path, test_file: Path):
    proc = subprocess.Popen(
        [str(binary), test_file.name],
        cwd=test_file.parent,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    raw = []
    passes, fails = [], []
    summary = None
    for line in proc.stdout:
        raw.append(line)
        line = line.rstrip("\n")
        m = PASS_RE.match(line)
        if m:
            passes.append(m.group(1))
            report(".")
            continue
        m = FAIL_RE.match(line)
        if m:
            fails.append((m.group(1), m.group(2)))
            report("F")
            continue
        m = SUMMARY_RE.match(line)
        if m:
            summary = (int(m.group(1)), int(m.group(2)))
    proc.wait()

    crash = None
    if summary is None:
        crash = "no SUMMARY line found (crash, or missing ':- summary.')"
    elif summary != (len(passes), len(fails)):
        crash = (
            f"summary mismatch: file reported {summary}, "
            f"runner observed passed={len(passes)} failed={len(fails)}"
        )
    elif proc.returncode not in (0, None):
        # huprolog returns 0 on halt/0 in all configurations we use.
        crash = f"non-zero exit code {proc.returncode}"

    return {
        "ok": crash is None and len(fails) == 0,
        "crash": crash,
        "passes": passes,
        "fails": fails,
        "summary": summary,
        "raw": "".join(raw),
    }


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--binary", type=Path, default=DEFAULT_BINARY)
    ap.add_argument("--include-smoke", action="store_true")
    ap.add_argument("tests", nargs="*", type=Path)
    args = ap.parse_args()

    if not args.binary.exists():
        print(f"error: huprolog binary not found at {args.binary}", file=sys.stderr)
        print("build it first: cd Sources && ./Configure posix && make", file=sys.stderr)
        return 2

    if args.tests:
        test_files = args.tests
    else:
        test_files = sorted(HERE.glob("[0-9]*.pl"))
        if not args.include_smoke:
            test_files = [t for t in test_files if not t.name.startswith("00_smoke")]

    total_pass = 0
    total_fail = 0
    broken_files = []
    vanished_total = 0
    details = []

    width = max(len(tf.name) for tf in test_files)
    for tf in test_files:
        print(f"{tf.name:<{width}} ", end="", flush=True)
        result = run_one(args.binary, tf)
        total_pass += len(result["passes"])
        total_fail += len(result["fails"])
        observed = len(result["passes"]) + len(result["fails"])
        vanished = count_expected_checks(tf) - observed
        marker = " E" if result["crash"] or vanished > 0 else ""
        print(marker)

        problems = [f"FAIL {name} -- {reason}" for name, reason in result["fails"]]
        if vanished > 0:
            problems.append(
                f"WARNING: {vanished} check(...) call(s) in the source never "
                f"produced a PASS/FAIL line -- a goal earlier in the same "
                f"conjunction (':- G1, G2, check(...).') likely failed silently, "
                f"so the check(...) call was never reached. Fix: wrap the actual "
                f"goal being tested INSIDE check(Name, Goal) instead of running "
                f"it as a precondition beforehand.")
            vanished_total += vanished
        if result["crash"]:
            problems.append(f"ERROR: {result['crash']}")
            problems.append("--- raw output ---")
            problems.extend(f"| {line}" for line in result["raw"].splitlines())
            broken_files.append(tf.name)
        if problems:
            details.append((tf.name, problems))

    for name, problems in details:
        print()
        print(f" {name} ".center(70, "_"))
        for line in problems:
            print(line)

    counts = []
    if total_fail:
        counts.append(f"{total_fail} failed")
    counts.append(f"{total_pass} passed")
    if vanished_total:
        counts.append(f"{vanished_total} vanished")
    if broken_files:
        counts.append(f"{len(broken_files)} broken")
    print()
    print(f" {', '.join(counts)} ".center(70, "="))

    return 0 if (total_fail == 0 and not broken_files and not vanished_total) else 1


if __name__ == "__main__":
    sys.exit(main())
