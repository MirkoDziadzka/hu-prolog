:- consult('harness.pl').

:- check(trivial_true, true).
:- check_not(trivial_fail, fail).
:- X is 1 + 2, check_eq(arith_add, X, 3).
:- check_eq(arith_add2, 3, 3).
:- check_error(div_zero, (_ is 1/0), 14).

:- summary.
:- halt.
