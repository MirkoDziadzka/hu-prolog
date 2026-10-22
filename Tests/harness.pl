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

:- retractall(test_pass_count(_)).
:- asserta(test_pass_count(0)).
:- retractall(test_fail_count(_)).
:- asserta(test_fail_count(0)).

pass(Name) :-
    retract(test_pass_count(N)),
    N1 is N + 1,
    asserta(test_pass_count(N1)),
    write('PASS '), write(Name), nl.

report_fail(Name, Reason) :-
    retract(test_fail_count(N)),
    N1 is N + 1,
    asserta(test_fail_count(N1)),
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
:- retractall(last_error(_)).
:- asserta(last_error(none)).
error(_, E) :-
    retractall(last_error(_)),
    asserta(last_error(E)),
    fail.

% check_error(Name, Goal, ExpectedErrNum) -- Goal must raise exactly
% ExpectedErrNum (an integer error code, see Sources/types.h) and fail.
check_error(Name, Goal, ExpectedErrNum) :-
    retractall(last_error(_)),
    asserta(last_error(none)),
    ( call(Goal) ->
        report_fail(Name, 'expected error but goal succeeded')
    ; last_error(Got),
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
    test_pass_count(P), test_fail_count(F),
    write('SUMMARY passed='), write(P), write(' failed='), write(F), nl.
