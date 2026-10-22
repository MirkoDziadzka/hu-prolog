% :=/2 : global variables and user-defined functions in arithmetic.
% Reference: Help/help.txt :=/2 and "Globale Variablen und
% nutzerdefinierte Funktionen".

:- consult('harness.pl').

% --- basic := behaves like is/2 when assigning to an unbound var ---
:- X := 1 + 2, check_eq(assign_behaves_like_is, X, 3).

% --- := on a compound first argument creates/updates a "global
% variable" (manual example: counter(17) := 0, then += 1) ---
:- counter(17) := 0,
   counter(17) := counter(17) + 1,
   X := counter(17),
   check_eq(global_var_example, X, 1).

% a second, independently-keyed global variable is distinct
:- counter(5) := 100,
   X := counter(5),
   check_eq(global_var_distinct_key, X, 100).
:- X := counter(17), check_eq(global_var_17_unaffected_by_5, X, 1).

% --- user-defined "functions": an (N+1)-ary predicate used as an
% N-ary function inside :=/2 expressions (manual's fac/2 example,
% note this is := -specific -- is/2 does NOT support it, see
% 05_arithmetic.pl's is_rejects_user_function test) ---
fac(1, 0).
fac(R, N) :-
    N1 := N - 1,
    R := N * fac(N1).
:- X := fac(5), check_eq(user_function_fac_5, X, 120).
:- X := fac(10), check_eq(user_function_fac_10_manual_example, X, 3628800).

:- summary.
:- halt.
