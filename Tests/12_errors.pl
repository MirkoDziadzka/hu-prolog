% Error codes and the error/2 catch hook.
% Reference: Help/help.txt error/2, Sources/types.h (error numbers),
% Sources/io.c ERRTAB (error messages).
%
% IMPORTANT: not all error codes are catchable. Most are raised via
% the C-level ERROR() macro, which (if error/2 has user clauses)
% re-dispatches the failing call through error(Call, ErrNum) -- see
% Sources/exec.c's ON_ERROR label. Some codes are raised via ABORT()
% instead, which jumps straight to the top level via a different
% longjmp target (ABORT_JMP vs ERROR_JMP) and bypasses error/2
% entirely. That's correct/by design for resource exhaustion (*SPACEE,
% NVARSE) and C-stack-depth guards (DEPTHE) -- constructing the
% error(Call,N) splice-in term needs the very resource that's gone --
% and for abort/0, abort/2, restart/0 and friends, which are
% documented as deliberately uncatchable. It was previously ALSO true
% for arg/3's ARGE (eval.c DOARG) and the consult-time SYSPROCE check
% in datab.c's clause-insertion loop, but those had no such reason --
% every *other* ARGE/SYSPROCE site already used catchable ERROR() --
% so both were switched from ABORT() to ERROR(), see the checks below.
% ARGE-via-arg/3 is now routed through error/2 like any other ERROR();
% SYSPROCE-via-consult is catchable too, but only as an ordinary goal
% failure, not via error/2 -- see that check's own comment for why.

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

% arg/3 out-of-range / wrong-argument-order: used to be ABORT(ARGE),
% uncatchable (see 03_termconstruct.pl for the positive-path tests of
% arg/3 itself); now ERROR(ARGE) like every other ARGE site.
:- check_error(arg_out_of_range_is_catchable, arg(3, foo(a,b), _), 2).

% Defining a clause for a protected system predicate WHILE CONSULTING
% A FILE (as opposed to via assert/1, which already used catchable
% ERROR(SYSPROCE)) used to be ABORT(SYSPROCE), which printed "ERROR
% 29" and unwound the ENTIRE PROCESS straight to the top level --
% consulting any file with one bad clause in it was fatal, even from
% deep inside a library load. Trigger it by consulting a scratch file
% that tries to add a clause to retractall/1 (a NORMP, sysflag-
% protected predicate) without sysmode.
%
% NOTE: this is NOT routed through error/2, unlike the other checks in
% this file. error/2 dispatch happens in EXECUTE's own loop (see
% Sources/exec.c ON_ERROR), reached via CALLEVALPRED's nested catch
% point around each evaluable-predicate CALL. But this error fires
% directly inside Sources/datab.c's DOCONSULT, while it's reading and
% inserting a CLAUSE (not executing a goal) -- there's no CALLEVALPRED
% in that path to splice error/2 in. DOCONSULT catches its own
% ERROR()s locally (see its "reset_error_label") so one bad clause
% can't take down the whole process; the result is that consult/1
% simply FAILS as an ordinary goal, exactly like the pre-existing
% CANTOP (file-not-found) case in the very same function. That's the
% behavior pinned down here: catchable via ordinary Prolog control
% flow (fail/backtrack), just not via error/2's (Call, ErrorNum) form.
:- tell('/tmp/hu_sysproce_test.pl'),
   write('retractall(sysproce_probe) :- true.'), nl,
   told,
   check_not(consult_time_sysproce_fails_cleanly,
             consult('/tmp/hu_sysproce_test.pl')).
% and execution actually continues normally afterward -- this would
% have been unreachable code under the old ABORT()-based behavior.
:- check(execution_continues_after_consult_time_sysproce, true).

% error/2 is itself just a normal user-definable predicate (class
% NORMP in Sources/atom.tab, not sysflag) -- confirming it can be
% (re)defined freely, which is the whole point of the hook mechanism.
:- check(error_2_is_user_definable, current_predicate(error/2)).

:- summary.
:- halt.
