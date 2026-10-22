% Error codes and the error/2 catch hook.
% Reference: Help/help.txt error/2, Sources/types.h (error numbers),
% Sources/io.c ERRTAB (error messages).
%
% IMPORTANT: not all error codes are catchable. Most are raised via
% the C-level ERROR() macro, which (if error/2 has user clauses)
% re-dispatches the failing call through error(Call, ErrNum) -- see
% Sources/exec.c's ON_ERROR label. A few codes (confirmed: ARGE as
% raised specifically by arg/3's DOARG in eval.c, DEPTHE, NVARSE, and
% the *SPACEE memory-exhaustion codes) are raised via ABORT() instead,
% which jumps straight to the top level via a different longjmp target
% (ABORT_JMP vs ERROR_JMP) and bypasses error/2 entirely. Those are
% not exercised here -- see 03_termconstruct.pl's comment on arg/3.

:- consult('harness.pl').

% error/2 receives (Call, ErrorNumber); harness.pl's hook asserts the
% number into last_error/1 and fails, so the triggering goal fails
% cleanly instead of unwinding to the top level.
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

% error/2 is itself just a normal user-definable predicate (class
% NORMP in Sources/atom.tab, not sysflag) -- confirming it can be
% (re)defined freely, which is the whole point of the hook mechanism.
:- check(error_2_is_user_definable, current_predicate(error/2)).

:- summary.
:- halt.
