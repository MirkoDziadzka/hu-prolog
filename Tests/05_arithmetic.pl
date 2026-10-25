% Arithmetic: is/2, comparisons, standard functions and operators.
% Reference: Help/help.txt is/2, Arithmetische Vergleichsoperatoren,
% and user-manual section 7 (0/1/2-stellige Funktionen table).

:- consult('harness.pl').

% --- is/2 basic ---
% Help is/2:
%       Result is Expression
%
%   evaluates the arithmetic expression `Expression' and unifies
%   the result of this evaluation with `Result'.
%   Example:
%
%       ?- X is 17 + 4.
%       X = 21.
%       yes
:- X is 17 + 4, check_eq(is_example_from_manual, X, 21).
:- X is 2 + 3, check_eq(is_add, X, 5).
:- X is 10 - 3, check_eq(is_sub, X, 7).
:- X is 4 * 5, check_eq(is_mul, X, 20).
:- X is 7 // 2, check_eq(is_intdiv, X, 3).
:- X is 7 mod 2, check_eq(is_mod, X, 1).
:- X is 7.0 / 2.0, check(is_realdiv, (X =:= 3.5)).
:- X is 2 ** 10, check(is_pow, (X =:= 1024.0)).

% --- unary ops ---
% Help 1-ary Functions:
%   Name      | Type         |  Meaning
%   ----------+--------------+---------------------------------
%   -x        | int -> int   |  arithmetic negation
%             | real -> real |
%   ----------+--------------+---------------------------------
%   /x        | int -> int   |  logical negation
%   ~x        |              |  bitwise negation
%   ----------+--------------+---------------------------------
%   real(x)   | int -> real  |  type conversion
%   ----------+--------------+---------------------------------
%   entier(x) | real -> int  |
%
%   In addition there are the following 1-ary functions of type
%   real -> real from the standard C library, adopted into
%   HU-Prolog with the same meaning:
%
%   exp, ln, log10, sqrt, sin, cos, tan, asin, acos, atan,
%   floor and ceil
:- X is -5, check_eq(is_unary_minus, X, -5).
:- X is -(3+2), check_eq(is_unary_minus_expr, X, -5).
:- X is -(2.5), check(is_unary_minus_real, (X =:= -2.5)).
:- X is /0, check_eq(is_lognot_of_0, X, 1).
:- X is /5, check_eq(is_lognot_of_nonzero, X, 0).
:- X is ~0, check_eq(is_bitneg, X, -1).

% --- bitwise / logical ---
% Help 2-ary Functions:
%   Name    |    Type           | Meaning
%   --------+-------------------+------------------------------
%   B << N  | int*int -> int    | bitwise shift
%   B >> N  |                   |
%   A && B  |                   | logical conjunction
%   A \\ B  |                   | logical disjunction (|| in C)
%   A & B   |                   | bitwise conjunction
%   A \ B   |                   | bitwise disjunction (| in C)
%   --------+-------------------+------------------------------
%   B ** E  | real*int -> real  | power function
%   --------+-------------------+------------------------------
%   A + B   | int*int -> int    |
%   A - B   | real*real -> real |
%   A * B   |                   |
%   --------+-------------------+------------------------------
%   A / B   | real*real -> real | real division
%   --------+-------------------+------------------------------
%   A // B  | int*int -> int    | integer division
%   A mod B |                   | modulo (in C: A % B)
:- X is 1 << 4, check_eq(is_shift_left, X, 16).
:- X is 256 >> 4, check_eq(is_shift_right, X, 16).
:- X is 6 & 3, check_eq(is_bitand, X, 2).
:- X is 6 \ 1, check_eq(is_bitor, X, 7).
:- X is 1 && 0, check_eq(is_logand_false, X, 0).
:- X is 1 && 1, check_eq(is_logand_true, X, 1).
:- X is 0 \\ 1, check_eq(is_logor_true, X, 1).
:- X is 0 \\ 0, check_eq(is_logor_false, X, 0).
:- X is 1.5 + 1.5, check(is_add_reals, (X =:= 3.0)).
:- X is 2.5 - 1.0, check(is_sub_reals, (X =:= 1.5)).
:- X is 1.5 * 2.0, check(is_mul_reals, (X =:= 3.0)).
:- X is 7 / 2, check(is_realdiv_of_ints, (X =:= 3.5)).

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
:- X is log10(100.0), check(is_log10, (X =:= 2.0)).
:- X is tan(0.0), check(is_tan, (X =:= 0.0)).
:- X is asin(0.0), check(is_asin, (X =:= 0.0)).
:- X is acos(1.0), check(is_acos, (X =:= 0.0)).
:- X is atan(0.0), check(is_atan, (X =:= 0.0)).

% --- 0-arity constants ---
% Help 0-ary Functions:
%   Name     | Type | Meaning
%   ---------+------+--------------------------------------------
%   maxint   | int  | largest integer
%   minint   |      | smallest integer
%   ---------+------+-------------------------------------------
%   e        | real | 2.718281828...
%   pi       |      | 3.1415.....
%   ---------+------+-------------------------------------------
%   maxarity | int  | maximum functor arity
%   maxdepth | int  | maximum recursion depth when processing terms
:- X is pi, check(is_pi, (X > 3.14, X < 3.15)).
:- X is e, check(is_e, (X > 2.71, X < 2.72)).
:- X is maxint, check(is_maxint_positive, (X > 0)).
:- X is minint, check(is_minint_negative, (X < 0)).
:- X is maxarity, check(is_maxarity_usable_by_functor, functor(_, f, X)).
:- X is maxdepth, check(is_maxdepth_positive_int, (integer(X), X > 0)).

% --- comparisons: </2 =</2 >/2 >=/2 =:=/2 =\=/2 ---
% Help Arithmetic Comparison Operators:
%   The predicates </2, =</2 , =:=/2 , =\=/2 , >=/2 and >/2
%   first evaluate their arguments (using :=/2).
%   The results of this evaluation must be numeric values.
%   These are then compared and (depending on the result of the
%   comparison) cause the predicate to succeed or
%   fail.
:- check(lt_true, 1 < 2).
:- check_not(lt_false, 2 < 1).
:- check(le_eq, 2 =< 2).
:- check(gt_true, 2 > 1).
:- check(ge_eq, 2 >= 2).
:- check(numeq_true, (1+1) =:= 2).
:- check(numeq_int_real, 2 =:= 2.0).
:- check(numneq_true, 1 =\= 2).
:- check_not(numneq_false, 2 =\= 2.0).
:- check(cmp_evaluates_both_sides, 2+2 =:= 1+3).
:- cmpval := 5, check(cmp_uses_global_variable, cmpval > 4).
double(R, X) :- R is 2 * X.
:- check(cmp_uses_user_function, double(3) =:= 6).
:- check_error(cmp_non_numeric_is_error, a < b, 2).

% is/2 does not call user-defined functions; :=/2 does, see
% 11_globalvars.pl.
:- check_error(is_rejects_user_function, (_ is undefined_user_func(1)), 31).

% --- division/mod by zero (error 14) ---
:- check_error(div_zero, (_ is 1/0), 14).
:- check_error(intdiv_zero, (_ is 1//0), 14).
:- check_error(mod_zero, (_ is 1 mod 0), 14).

:- summary.
:- halt.
