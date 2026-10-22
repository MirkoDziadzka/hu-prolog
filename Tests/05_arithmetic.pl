% Arithmetic: is/2, comparisons, standard functions and operators.
% Reference: Help/help.txt is/2, Arithmetische Vergleichsoperatoren,
% and user-manual section 7 (0/1/2-stellige Funktionen table).

:- consult('harness.pl').

% --- is/2 basic ---
:- X is 17 + 4, check_eq(is_example_from_manual, X, 21).
:- X is 2 + 3, check_eq(is_add, X, 5).
:- X is 10 - 3, check_eq(is_sub, X, 7).
:- X is 4 * 5, check_eq(is_mul, X, 20).
:- X is 7 // 2, check_eq(is_intdiv, X, 3).
:- X is 7 mod 2, check_eq(is_mod, X, 1).
:- X is 7.0 / 2.0, check(is_realdiv, (X =:= 3.5)).
:- X is 2 ** 10, check(is_pow, (X =:= 1024.0)).

% --- unary ops ---
:- X is -5, check_eq(is_unary_minus, X, -5).
:- X is -(3+2), check_eq(is_unary_minus_expr, X, -5).

% --- bitwise / logical (section: 2-stellige Funktionen) ---
:- X is 1 << 4, check_eq(is_shift_left, X, 16).
:- X is 256 >> 4, check_eq(is_shift_right, X, 16).
:- X is 6 & 3, check_eq(is_bitand, X, 2).
:- X is 6 \ 1, check_eq(is_bitor, X, 7).
:- X is 1 && 0, check_eq(is_logand_false, X, 0).
:- X is 1 && 1, check_eq(is_logand_true, X, 1).
:- X is 0 \\ 1, check_eq(is_logor_true, X, 1).
:- X is 0 \\ 0, check_eq(is_logor_false, X, 0).

% --- type conversion ---
:- X is real(2), check(is_real_conversion, (X =:= 2.0)).
:- X is entier(3.7), check_eq(is_entier, X, 3).

% --- standard C-math functions (real -> real) ---
:- X is sqrt(16.0), check(is_sqrt, (X =:= 4.0)).
:- X is exp(0.0), check(is_exp, (X =:= 1.0)).
:- X is ln(1.0), check(is_ln, (X =:= 0.0)).
:- X is floor(3.7), check(is_floor, (X =:= 3.0)).
:- X is ceil(3.2), check(is_ceil, (X =:= 4.0)).
:- X is sin(0.0), check(is_sin, (X =:= 0.0)).
:- X is cos(0.0), check(is_cos, (X =:= 1.0)).

% --- 0-arity constants ---
:- X is pi, check(is_pi, (X > 3.14, X < 3.15)).
:- X is e, check(is_e, (X > 2.71, X < 2.72)).
:- X is maxint, check(is_maxint_positive, (X > 0)).
:- X is minint, check(is_minint_negative, (X < 0)).

% --- comparisons: </2 =</2 >/2 >=/2 =:=/2 =\=/2 ---
:- check(lt_true, 1 < 2).
:- check_not(lt_false, 2 < 1).
:- check(le_eq, 2 =< 2).
:- check(gt_true, 2 > 1).
:- check(ge_eq, 2 >= 2).
:- check(numeq_true, (1+1) =:= 2).
:- check(numeq_int_real, 2 =:= 2.0).
:- check(numneq_true, 1 =\= 2).
:- check_not(numneq_false, 2 =\= 2.0).
% comparisons evaluate expressions on both sides (per manual)
:- check(cmp_evaluates_both_sides, 2+2 =:= 1+3).

% NOTE: the manual's "user-defined functions in arithmetic" extension
% (e.g. "X := fac(10)") is specific to :=/2 -- is/2 does NOT support
% calling user predicates as functions (raises UNDEFFUNCE / error 31).
% See 11_globalvars.pl for the := version of this example.
:- check_error(is_rejects_user_function, (_ is undefined_user_func(1)), 31).

% --- division/mod by zero (error 14) ---
:- check_error(div_zero, (_ is 1/0), 14).
:- check_error(intdiv_zero, (_ is 1//0), 14).
:- check_error(mod_zero, (_ is 1 mod 0), 14).

:- summary.
:- halt.
