% Database manipulation: assert/asserta/assertz, retract, retractall,
% abolish/1, abolish/2, clause/2, current_predicate/1, current_atom/1.
% Reference: Help/help.txt Datenbasisoperationen.

:- consult('harness.pl').

% --- assert/1 == assertz/1 ---
:- assert(fact1(a)), check(assert_adds_fact, fact1(a)).
:- assertz(fact1(b)), check(assertz_adds_fact, fact1(b)).
:- collect_all(fact1(X), X, L), check_eq(assertz_appends_after, L, [a,b]).

% --- asserta/1 prepends ---
:- asserta(fact1(z)), collect_all(fact1(X), X, L), check_eq(asserta_prepends, L, [z,a,b]).

% --- retract/1 ---
:- retract(fact1(z)), collect_all(fact1(X), X, L), check_eq(retract_removes_one, L, [a,b]).
:- check_not(retract_fails_if_absent, retract(fact1(nonexistent))).

db_b(1). db_b(2). db_b(3).
:- collect_all(retract(db_b(X)), X, L), check_eq(retract_backtracks_bare_pattern, L, [1,2,3]).

% retract((Head :- Body)) never matches a fact: GenTerm
% (Sources/datab.c) yields a fact as its bare head. See
% Help/help.txt retract/1.
db_a(1) :- fail.
db_a(2).
:- collect_all(retract((db_a(X) :- Y)), X-Y, L),
   check_eq(retract_headbody_pattern_skips_facts, L, [1-fail]).
:- check(retract_headbody_pattern_leaves_fact_untouched, db_a(2)).

% --- retractall/1 ---
:- assertz(fact2(1)), assertz(fact2(2)), assertz(fact2(3)),
   retractall(fact2(_)),
   check_not(retractall_removes_all, fact2(_)).
:- check(retractall_succeeds_even_if_nothing_matches, (retractall(nosuch(_)), true)).

% --- abolish/1 (removes a name at all arities) ---
:- assertz(ab1(x)), assertz(ab1(x,y)),
   abolish(ab1),
   check_not(abolish1_removes_arity1, ab1(_)).
:- check_not(abolish1_removes_arity2, ab1(_,_)).

% --- abolish/2 (removes name/arity only) ---
:- assertz(ab2(x)), assertz(ab2(x,y)),
   abolish(ab2, 1),
   check_not(abolish2_removes_only_given_arity, ab2(_)).
:- check(abolish2_leaves_other_arity, ab2(_,_)).

% --- clause/2 ---
clt(1) :- true.
clt(2) :- fail.
:- clause(clt(1), Body), check_eq(clause_fact_body_is_true, Body, true).
:- clause(clt(2), Body), check_eq(clause_rule_body, Body, fail).
:- check_not(clause_no_match, clause(clt(99), _)).

% --- current_predicate/1 ---
cp_test(_).
:- check(current_predicate_finds_defined, current_predicate(cp_test/1)).
:- check_not(current_predicate_absent, current_predicate(totally_undefined_pred_xyz/3)).

% --- current_atom/1 ---
:- check(current_atom_finds_atom, current_atom(cp_test/1)).

:- summary.
:- halt.
