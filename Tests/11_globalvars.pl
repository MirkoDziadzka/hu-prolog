% :=/2 : global variables and user-defined functions in arithmetic.
% Reference: Help/help.txt :=/2 and "Globale Variablen und
% nutzerdefinierte Funktionen".

:- consult('harness.pl').

% --- := to an unbound variable ---
% Help :=/2:
%   The second argument of :=/2 is evaluated. Any global variables
%   and user-defined functions that have been defined are taken
%   into account.
%   The result of this evaluation is assigned to the first argument
%   of :=/2. If that is a Prolog variable or a number, :=/2
%   behaves like is/2.
%   If it is any other term, it is interpreted as a global
%   variable, and the value is assigned to it.
%   A term marked with the prefix operator ` is not evaluated
%   but taken over unchanged.
%
%   Example:
%   ?- status := `none.
%   ?- X := status.
%   X = none
%   ?- counter(17) := 0.
%   ?- counter(17) := counter(17) + 1.
%   ?- X := counter(17).
%   X = 1
%   ?-listing.
%   counter(1,17).
:- X := 1 + 2, check_eq(assign_behaves_like_is, X, 3).
:- check(assign_number_lhs_like_is, 3 := 1 + 2).
:- check_not(assign_number_lhs_mismatch_fails, 4 := 1 + 2).
:- status := `none, X := status, check_eq(assign_backquote_not_evaluated, X, none).

% --- global variables ---
:- counter(17) := 0,
   counter(17) := counter(17) + 1,
   X := counter(17),
   check_eq(global_var_example, X, 1).

:- counter(5) := 100,
   X := counter(5),
   check_eq(global_var_distinct_key, X, 100).
:- X := counter(17), check_eq(global_var_17_unaffected_by_5, X, 1).
:- check(global_var_stored_as_value_key_clause, counter(1, 17)).

% --- user-defined functions ---
% Help Global Variables and User-Defined Functions:
%   Global variables and user-defined functions are an
%   extension of HU-Prolog that does not exist in this form in
%   other dialects, but could be defined there.
%
%   An (N+1)-ary predicate is treated as an N-ary
%   function. The first argument of the predicate
%   represents the result.
%
%   Example:
%
%           fac(1 , 0).
%           fac(R , N) :-
%                   N1 := N -1 ,
%                   R := N * fac(N1).
%           ?- X := fac(10).
%           X = 3628800
%
%   If the evaluation of the predicate fails, the calling term
%   becomes the result of the evaluation:
%
%           X := hello(world).
%           WARNING: no clause for relation hello/2
%           X = hello(world).
fac(1, 0).
fac(R, N) :-
    N1 := N - 1,
    R := N * fac(N1).
:- X := fac(5), check_eq(user_function_fac_5, X, 120).
:- X := fac(10), check_eq(user_function_fac_10_manual_example, X, 3628800).
:- X := hello(world), check_eq(user_function_failing_returns_call, X, hello(world)).

:- summary.
:- halt.
