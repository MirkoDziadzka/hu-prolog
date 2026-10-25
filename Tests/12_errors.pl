% Error codes and the error/2 catch hook.
% Reference: Help/help.txt error/2, Sources/types.h (error numbers),
% Sources/io.c ERRTAB (error messages).
%
% ERROR() codes go through error/2 (Sources/exec.c ON_ERROR); ABORT()
% codes jump to the top level and bypass it. ABORT() is used for
% resource exhaustion (*SPACEE, NVARSE), DEPTHE, abort/0, abort/2,
% restart/0, and consult-time SYSPROCE (Sources/datab.c DOCONSULT).

:- consult('harness.pl').

% Help error/2:
%   Most error messages that terminate the running program
%   can be caught. The exceptions are error messages about
%   memory problems of the interpreter.
%
%   If clauses named error/2 are defined in the database, the
%   predicate error/2 is called when an error occurs. As its
%   first argument it receives the call in which the error
%   occurred, as its second the error number.
%   If error/2 succeeds, execution then continues normally.
%   Otherwise backtracking is triggered.

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

% Help consult/1:
%   consult(Stream) opens the given stream and reads it in.
%   Clauses defined in the stream are added to the database
%   with assertz/1.
%
%   If the atom `end/0' is read or the predicate `end/0' is
%   executed, reading of the stream stops early.
%
%   As an abbreviation, list notation is allowed.
%       [ F ] :- consult(F).
%       [ F1 , F2 | T ] :- consult(F1) , [ F2 | T ].
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

:- seen_call := 0.
:- asserta((error((_ is 1/0), _) :- seen_call := 1, fail)).
:- check_error(error_hook_gets_failing_call, (_ is 1/0), 14).
:- retract((error((_ is 1/0), _) :- _)).
:- X := seen_call, check_eq(error_hook_first_arg_is_call, X, 1).

ok_handler.
:- asserta((error((_ is 1/0), 14) :- ok_handler)).
:- check(error_hook_success_continues, (_ is 1/0)).
:- retract((error(_, _) :- ok_handler)).

:- assertz(cf(1)),
   tell('/tmp/hu_consult_test.pl'), write('cf(2).'), nl, told,
   consult('/tmp/hu_consult_test.pl'),
   collect_all(cf(X), X, L),
   check_eq(consult_adds_with_assertz, L, [1,2]).
:- tell('/tmp/hu_consult_test.pl'),
   write('ce(1).'), nl, write('end.'), nl, write('ce(2).'), nl, told,
   consult('/tmp/hu_consult_test.pl'),
   collect_all(ce(X), X, L),
   check_eq(consult_stops_at_end_atom, L, [1]).
:- tell('/tmp/hu_consult_test.pl'),
   write('ce2(1).'), nl, write(':- end.'), nl, write('ce2(2).'), nl, told,
   consult('/tmp/hu_consult_test.pl'),
   collect_all(ce2(X), X, L),
   check_eq(consult_stops_at_end_goal, L, [1]).
:- tell('/tmp/hu_consult_test.pl'), write('cl(1).'), nl, told,
   ['/tmp/hu_consult_test.pl'],
   check(consult_list_notation, cl(1)).
:- assertz(rc(1)),
   tell('/tmp/hu_consult_test.pl'), write('rc(2).'), nl, told,
   reconsult('/tmp/hu_consult_test.pl'),
   collect_all(rc(X), X, L),
   check_eq(reconsult_replaces_clauses, L, [2]).

:- check(error_2_is_user_definable, current_predicate(error/2)).

:- summary.
:- halt.
