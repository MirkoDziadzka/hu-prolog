% Control constructs: ,/2 ;/2 ->/2 !/0 true/0 fail/0 repeat/0 call/1
% not/1 \+/1.
% Reference: Help/help.txt Ablaufsteuerung section.

:- consult('harness.pl').

% Help true/0:
%   Definition:
%
%           true.
% Help fail/0:
%   fail/0 causes backtracking.

:- check(true_succeeds, true).
:- check_not(fail_fails, fail).

% --- ,/2 conjunction ---
% Help ,/2:
%   ','(Goal1,Goal2) :-
%           Goal1,
%           Goal2.
%   ,/2 is transparent with respect to !/0
:- check(conj_both_true, (true, true)).
:- check_not(conj_one_false, (true, fail)).
:- X = 1, Y = 2, check(conj_binds_vars, (X == 1, Y == 2)).
conjcut(X) :- ((X = 1 ; X = 2), !), true.
conjcut(3).
:- collect_all(conjcut(X), X, L), check_eq(conj_transparent_to_cut, L, [1]).

% --- ;/2 disjunction ---
% Help ;/2:
%       (X ; _) :- X.
%       (_ ; Y) :- Y.
%
%   ;/2 is transparent with respect to !/0.
:- check(disj_left, (true ; fail)).
:- check(disj_right, (fail ; true)).
:- check_not(disj_both_fail, (fail ; fail)).
disjcut(X) :- ( X = 1, ! ; X = 2 ).
disjcut(3).
:- collect_all(disjcut(X), X, L), check_eq(disj_transparent_to_cut, L, [1]).

% --- ->/2 if-then ---
% Help ->/2:
%   Definition:
%
%           :- op( 1050 , xfy , '->').
%           ( P -> Q ) :- P , ! , Q.
:- check(ifthen_true_cond, (true -> true)).
:- check_not(ifthen_false_cond, (fail -> true)).
:- collect_all(((X = 1 ; X = 2) -> true), X, L), check_eq(ifthen_commits_to_first_cond_solution, L, [1]).
:- collect_all((true -> (X = 1 ; X = 2)), X, L), check_eq(ifthen_then_part_backtracks, L, [1,2]).
% if-then-else via ;/2
:- X = 1, ( X == 1 -> R = yes ; R = no ), check_eq(ifthenelse_then_branch, R, yes).
:- X = 2, ( X == 1 -> R = yes ; R = no ), check_eq(ifthenelse_else_branch, R, no).

% --- !/0 ---
cut_test(1) :- !.
cut_test(2).
:- cut_test(X), check_eq(cut_commits_to_first_clause, X, 1).

cuttrans(X) :- ( X = 1, ! ; X = 2 ).
:- cuttrans(X), check_eq(cut_transparent_in_conjunction, X, 1).

% --- call/1 ---
% Help call/1:
%   Definition:
%
%       call(X) :- X.
%
%   call/1 is NOT transparent to !/0.
:- check(call_atom_goal, call(true)).
:- check_not(call_fail_goal, call(fail)).
my_goal(ok).
:- call(my_goal(X)), check_eq(call_with_args, X, ok).

% retract+assert while a choice point is still open loops forever
% here; harness.pl's collect_all/3 asserts only while the choice
% point is open and retracts afterwards.
r(X) :- call((X = 1, !)).
r(2).
:- collect_all(r(X), X, Results),
   check_eq(call_not_cut_transparent, Results, [1,2]).

% --- repeat/0 (bounded via a counter + cut) ---
% Help repeat/0:
%   repeat/0 always succeeds (also on backtracking) and is thus
%   used to implement infinite loops.
%
%   Definition:
%
%           repeat.
%           repeat :- repeat.
%
%   WARNING: Since HU-Prolog does no tail recursion optimization,
%            repeat/0 is implemented differently internally.
%            A user-defined predicate of the form:
%
%                r.
%                r :- r.
%
%            is semantically equivalent to repeat/0, but can
%            (e.g. in `` ?- r, fail. '') lead to memory
%            problems.
:- asserta(rep_counter(0)),
   repeat,
   retract(rep_counter(N)), N1 is N + 1, asserta(rep_counter(N1)),
   N1 >= 3, !,
   rep_counter(Final), check_eq(repeat_bounded_loop, Final, 3).

% --- not/1 and \+/1 (equivalent) ---
% Help not/1:
%   Definition:
%
%       not(X) :- call(X) , ! , fail.
%       not(_).
% Help \+/1:
%   Definition:
%
%       \+(X) :- call(X) , ! , fail.
%       \+(_).
%
%   This predicate is equivalent to not/1 and exists in HU-Prolog
%   only because some strange Prolog dialects and the emerging
%   standard want it that way.
:- check(not_of_fail, not(fail)).
:- check_not(not_of_true, not(true)).
:- check(bnot_of_fail, \+(fail)).
:- check_not(bnot_of_true, \+(true)).
:- check(not_does_not_bind, (not((X = 1, fail)), var(X))).

:- summary.
:- halt.
