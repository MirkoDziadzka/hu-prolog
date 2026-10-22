% Control constructs: ,/2 ;/2 ->/2 !/0 true/0 fail/0 repeat/0 call/1
% not/1 \+/1.
% Reference: Help/help.txt Ablaufsteuerung section.

:- consult('harness.pl').

:- check(true_succeeds, true).
:- check_not(fail_fails, fail).

% --- ,/2 conjunction ---
:- check(conj_both_true, (true, true)).
:- check_not(conj_one_false, (true, fail)).
:- X = 1, Y = 2, check(conj_binds_vars, (X == 1, Y == 2)).

% --- ;/2 disjunction ---
:- check(disj_left, (true ; fail)).
:- check(disj_right, (fail ; true)).
:- check_not(disj_both_fail, (fail ; fail)).

% --- ->/2 if-then (bare, no else: fails if condition fails) ---
:- check(ifthen_true_cond, (true -> true)).
:- check_not(ifthen_false_cond, (fail -> true)).
% if-then-else via ;/2
:- X = 1, ( X == 1 -> R = yes ; R = no ), check_eq(ifthenelse_then_branch, R, yes).
:- X = 2, ( X == 1 -> R = yes ; R = no ), check_eq(ifthenelse_else_branch, R, no).

% --- !/0 cut: commits to the first matching clause ---
cut_test(1) :- !.
cut_test(2).
:- cut_test(X), check_eq(cut_commits_to_first_clause, X, 1).

% !/0 inside ,/2 is transparent to the enclosing clause's choice points
cuttrans(X) :- ( X = 1, ! ; X = 2 ).
:- cuttrans(X), check_eq(cut_transparent_in_conjunction, X, 1).

% --- call/1 ---
:- check(call_atom_goal, call(true)).
:- check_not(call_fail_goal, call(fail)).
my_goal(ok).
:- call(my_goal(X)), check_eq(call_with_args, X, ok).

% call/1 is documented as NOT transparent to cut: a "!" inside the
% called goal only prunes choice points created within that call, not
% the calling clause's own alternative clauses.
%
% NOTE: an earlier version of this test accumulated solutions with
% retract(acc(L)), ..., asserta(acc(L2)) *while* r(X)'s own choice
% point was still open. That combination -- mutating a dynamic
% predicate via retract+assert while backtracking is pending elsewhere
% -- looped forever on this implementation (no "logical update view"
% guarantee). See harness.pl's collect_all/3 for the safe pattern:
% only assertz while the choice point is open, retract afterward.
r(X) :- call((X = 1, !)).
r(2).
:- collect_all(r(X), X, Results),
   check_eq(call_not_cut_transparent, Results, [1,2]).

% --- repeat/0 (bounded via a counter + cut, see help.txt caveat) ---
:- asserta(rep_counter(0)),
   repeat,
   retract(rep_counter(N)), N1 is N + 1, asserta(rep_counter(N1)),
   N1 >= 3, !,
   rep_counter(Final), check_eq(repeat_bounded_loop, Final, 3).

% --- not/1 and \+/1 (equivalent) ---
:- check(not_of_fail, not(fail)).
:- check_not(not_of_true, not(true)).
:- check(bnot_of_fail, \+(fail)).
:- check_not(bnot_of_true, \+(true)).
% not/1 does not bind variables (failure inside it undoes bindings)
:- check(not_does_not_bind, (not((X = 1, fail)), var(X))).

:- summary.
:- halt.
