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

% '$collected' and '$drain_collected' are local to this file.
:- private(['$collected', '$drain_collected']).

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

% error/2 hook: records the error number and fails. "No error yet"
% is none.
:- last_error := `none.
error(_, E) :-
    last_error := E,
    fail.

% check_error(Name, Goal, ExpectedErrNum) -- Goal must raise exactly
% ExpectedErrNum (an integer error code, see Sources/types.h) and fail.
check_error(Name, Goal, ExpectedErrNum) :-
    last_error := `none,
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
