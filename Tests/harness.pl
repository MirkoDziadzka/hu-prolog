% Shared test helpers for the HU-Prolog regression suite.
% Each test file starts with ":- consult('harness.pl')." (or the
% runner's include path) and then uses check/2, check_eq/3,
% check_error/3 to report pass/fail lines that Tests/run_tests.py
% parses from stdout.
%
% Output protocol (one line per check):
%   PASS <name>
%   FAIL <name> -- <reason>
% and a final:
%   SUMMARY passed=<N> failed=<M>

% '$collected' and '$drain_collected' are implementation details of
% collect_all/3 only -- private/1 here plus hide/1 at the end of the
% file (see Help/help.txt) scope them to this file so the names can't
% collide with anything defined by the test files consulting it, the
% same way a module system would.
:- private(['$collected', '$drain_collected']).

% test_pass_count/test_fail_count/last_error (below) are scalar
% counters/flags held in :=/2 global variables (see Help/help.txt).
% := persists across backtracking and updates the stored value in
% place, leaving the clause chain untouched.
:- test_pass_count := 0.
:- test_fail_count := 0.

pass(Name) :-
    test_pass_count := test_pass_count + 1,
    write('PASS '), write(Name), nl.

report_fail(Name, Reason) :-
    test_fail_count := test_fail_count + 1,
    write('FAIL '), write(Name), write(' -- '), write(Reason), nl.

% check(Name, Goal) -- Goal must succeed.
check(Name, Goal) :-
    ( call(Goal) -> pass(Name) ; report_fail(Name, 'goal failed') ).

% check_not(Name, Goal) -- Goal must fail.
check_not(Name, Goal) :-
    ( call(Goal) -> report_fail(Name, 'goal unexpectedly succeeded') ; pass(Name) ).

% check_eq(Name, Actual, Expected) -- structural equality (==/2).
check_eq(Name, Actual, Expected) :-
    ( Actual == Expected -> pass(Name)
    ; report_fail(Name, got(Actual)-expected(Expected)) ).

% error/2 hook: records the last error number raised while Goal (below)
% was running, then fails so the caller's own control flow resumes.
% Sentinel for "no error yet" is -1, a number rather than the atom
% 'none' -- := only evaluates a stored ATOM value by first trying (and
% failing) to call it as a 0-arity function, which is harmless but
% prints a stray "WARNING: unknown relation ./1 failed" on every
% reset; a number sentinel avoids that noise. No test file depends on
% the literal value, only on check_error/3's pass/fail outcome.
:- last_error := -1.
error(_, E) :-
    last_error := E,
    fail.

% check_error(Name, Goal, ExpectedErrNum) -- Goal must raise exactly
% ExpectedErrNum (an integer error code, see Sources/types.h) and fail.
check_error(Name, Goal, ExpectedErrNum) :-
    last_error := -1,
    ( call(Goal) ->
        report_fail(Name, 'expected error but goal succeeded')
    ; Got := last_error,
      ( Got == ExpectedErrNum -> pass(Name)
      ; report_fail(Name, got_error(Got)-expected(ExpectedErrNum)) )
    ).

% collect_all(Goal, Template, List) -- poor man's findall/3 (HU-Prolog
% has none). IMPORTANT: only assertz/1 happens while Goal's choice
% points are still open; retract/1 happens only afterward. Doing
% retract+assert interleaved with an open backtracking choice point
% elsewhere in this implementation can loop forever -- see
% 06_control.pl for a worked repro of that hazard.
collect_all(Goal, Template, List) :-
    retractall('$collected'(_)),
    ( call(Goal), assertz('$collected'(Template)), fail ; true ),
    '$drain_collected'(List).

'$drain_collected'(List) :-
    ( retract('$collected'(X)) ->
        '$drain_collected'(Rest), List = [X|Rest]
    ; List = [] ).

summary :-
    P := test_pass_count, F := test_fail_count,
    write('SUMMARY passed='), write(P), write(' failed='), write(F), nl.

:- hide(['$collected', '$drain_collected']).
