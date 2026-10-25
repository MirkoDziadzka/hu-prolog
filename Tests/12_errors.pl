% Error codes and the error/2 catch hook.
% Reference: Help/help.txt error/2, Sources/types.h (error numbers),
% Sources/io.c ERRTAB (error messages).
%
% ERROR() codes go through error/2 (Sources/exec.c ON_ERROR); ABORT()
% codes jump to the top level and bypass it. ABORT() is used for
% resource exhaustion (*SPACEE, NVARSE), DEPTHE, abort/0, abort/2,
% restart/0, and consult-time SYSPROCE (Sources/datab.c DOCONSULT).

:- consult('harness.pl').

:- check_error(division_by_zero, (_ is 1/0), 14).
:- check_error(intdiv_by_zero, (_ is 1//0), 14).
:- check_error(mod_by_zero, (_ is 1 mod 0), 14).
:- check_error(undefined_arith_function, (_ is totally_undefined_fn(1)), 31).
:- check_error(call_of_non_callable_number, call(1), 11).
:- check_error(call_of_unbound_var, (V = _, call(V)), 11).
:- check_error(op_redeclare_system_operator_needs_sysmode, op(500,xfx,is), 29).
:- check_error(functor_arity_not_an_expression, functor(_,foo,bar), 31).
:- check_error(functor_arity_exceeds_maxarity, functor(_,foo,99999), 4).
:- check_error(stdfunc_arg_not_an_expression, (_ is sqrt(foo)), 31).

:- check_error(arg_out_of_range_is_catchable, arg(3, foo(a,b), _), 2).

% An ERROR() raised while consult/1 reads a file (here EOFE from a
% file ending inside a clause) makes consult/1 itself fail with that
% error, and the goals after it run normally.
% The extra error/2 clause records whether the error is raised on the
% consult/1 call itself, then fails through to harness.pl's clause.
:- tell('/tmp/hu_eof_test.pl'),
   write('foo(a, b'),
   told.
:- error_on_consult := 0.
:- asserta((error(consult(_), _) :- error_on_consult := 1, fail)).
:- check_error(consult_eof_raises_eofe,
               consult('/tmp/hu_eof_test.pl'), 15).
:- retract((error(consult(_), _) :- _)).
:- X := error_on_consult, check_eq(consult_eof_raised_on_consult, X, 1).
:- check(goal_after_failed_consult_runs, true).

:- check(error_2_is_user_definable, current_predicate(error/2)).

:- summary.
:- halt.
