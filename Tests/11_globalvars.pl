% :=/2 : global variables and user-defined functions in arithmetic.
% Reference: Help/help.txt :=/2 and "Globale Variablen und
% nutzerdefinierte Funktionen".

:- consult('harness.pl').

% --- := to an unbound variable ---
:- X := 1 + 2, check_eq(assign_behaves_like_is, X, 3).

% --- global variables ---
:- counter(17) := 0,
   counter(17) := counter(17) + 1,
   X := counter(17),
   check_eq(global_var_example, X, 1).

:- counter(5) := 100,
   X := counter(5),
   check_eq(global_var_distinct_key, X, 100).
:- X := counter(17), check_eq(global_var_17_unaffected_by_5, X, 1).

% --- user-defined functions ---
fac(1, 0).
fac(R, N) :-
    N1 := N - 1,
    R := N * fac(N1).
:- X := fac(5), check_eq(user_function_fac_5, X, 120).
:- X := fac(10), check_eq(user_function_fac_10_manual_example, X, 3628800).

:- summary.
:- halt.
