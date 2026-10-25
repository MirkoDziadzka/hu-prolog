% Term comparison: =/2, \=/2, ==/2, \==/2, lexicographic ordering.
% Reference: Help/help.txt Unifizierbarkeit, Identitaet,
% Lexikographische Ordnung.

:- consult('harness.pl').

% --- =/2 (unification), \=/2 ---
:- check(unify_identical_atoms, a = a).
:- check(unify_var_with_atom, _ = a).
:- check(unify_structures, foo(X,b) = foo(a,Y)).
:- check_not(unify_fail_different_atoms, a = b).
:- check(notequal_different, a \= b).
:- check_not(notequal_same, a \= a).

% --- ==/2, \==/2 (structural identity, no unification) ---
:- check(identical_atoms, a == a).
:- check(identical_bound_vars, (X = 1, Y = 1, X == Y)).
:- check_not(identical_unbound_vars_differ, (var(X), var(Y), X == Y)).
:- check(identical_same_shared_var, (T1 = a(X), T2 = a(X), T1 == T2)).
:- check_not(identical_different_vars, a(X) == a(Y)).
:- check(notidentical_pos, a \== b).
:- check_not(notidentical_neg, a \== a).
% == must not bind variables
:- check(identity_does_not_unify, (var(X), var(Y), \+(X == Y), var(X), var(Y))).

% --- lexicographic ordering: @</2 @=</2 @>/2 @>=/2 @=/2 @\=/2 ---
:- check(order_var_eq_var, (var(X), var(Y), X @= Y)).
:- check(order_var_lt_number, (var(X), X @< 1)).
:- check(order_int_lt_real, 1 @< 2.5).
:- check(order_atom_lt_atom, a @< b).
:- check(order_compound_lt_atom_by_name, a(1) @< b).
:- check(order_atom_lt_compound_same_name, a @< a(1)).
:- check_not(order_atom_not_lt_itself, a @< a).
:- check(order_le_reflexive, a @=< a).
:- check(order_ge_reflexive, a @>= a).
:- check(order_gt_reverse, b @> a).
:- check(order_ge_from_gt, b @>= a).
:- check(order_ne, a @\= b).
:- check_not(order_eq_as_ne, a @\= a).
% @=/2 behaves like ==/2 for ground terms
:- check(order_eq_ground, foo(1,2) @= foo(1,2)).
:- check(order_lt_arity, foo(1) @< foo(1,2)).
:- check(order_lt_arity2, foo(1,2) @> foo(1)).

:- summary.
:- halt.
