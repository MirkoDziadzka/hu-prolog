% Output predicates: write/1, writeq/1, display/1, put/1, tab/1, nl/0.
% Reference: Help/help.txt Ausgabeoperationen.
%
% write/writeq/display are verified via a safe tell+consult roundtrip
% (see harness.pl / 06_control.pl for why raw see/seen is unsafe
% mid-consult: it corrupts the outer consult's notion of "current
% input stream" and silently truncates the rest of the file).

:- consult('harness.pl').

roundtrip(Goal, Term, Result) :-
    tell('/tmp/hu_io_rt.pl'),
    write(':- assertz(''$io_result''('),
    G =.. [Goal, Term],
    call(G),
    write(')).'),
    nl,
    told,
    consult('/tmp/hu_io_rt.pl'),
    retract('$io_result'(Result)).

% --- write/1 : operator-aware, unquoted ---
:- roundtrip(write, hello(world), R), check_eq(write_compound_roundtrips, R, hello(world)).
:- roundtrip(write, 1+2, R), check_eq(write_operator_roundtrips, R, 1+2).
:- roundtrip(write, [1,2,3], R), check_eq(write_list_roundtrips, R, [1,2,3]).

% --- writeq/1 : like write/1 but quotes atoms that need it, so the
% output is always readable back to an identical term (manual claim) ---
:- roundtrip(writeq, 'needs quoting', R), check_eq(writeq_quoted_atom_roundtrips, R, 'needs quoting').
:- roundtrip(writeq, 'it''s', R), check_eq(writeq_embedded_quote_roundtrips, R, 'it''s').
:- roundtrip(writeq, foo(bar,'A B'), R), check_eq(writeq_mixed_roundtrips, R, foo(bar,'A B')).

% --- display/1 : always fully-bracketed functor notation, ignores ops ---
:- roundtrip(display, a+b*c, R), check_eq(display_ignores_user_readability, R, a+b*c).

% --- put/1, tab/1, nl/0 : these write raw bytes to the current output
% stream and have no return value to assert on, so they're exercised
% as smoke tests (must succeed without erroring). The raw bytes are
% redirected to a scratch file via tell/told -- IMPORTANT: told must
% run (restoring stdout) BEFORE any check/check_eq call, since those
% write their PASS/FAIL line to whatever the current output stream is.
:- tell('/tmp/hu_put_smoke.pl'),
   ( put(65)   -> S1 = ok ; S1 = failed ),
   ( put("hi") -> S2 = ok ; S2 = failed ),
   ( tab(3)    -> S3 = ok ; S3 = failed ),
   ( tab(0)    -> S4 = ok ; S4 = failed ),
   ( nl        -> S5 = ok ; S5 = failed ),
   told,
   check_eq(put_single_code_succeeds, S1, ok),
   check_eq(put_code_list_succeeds, S2, ok),
   check_eq(tab_succeeds, S3, ok),
   check_eq(tab_zero_succeeds, S4, ok),
   check_eq(nl_succeeds, S5, ok).

:- summary.
:- halt.
