% Output predicates: write/1, writeq/1, display/1, put/1, tab/1, nl/0.
% Reference: Help/help.txt Ausgabeoperationen.
%
% write/writeq/display are checked via a tell + consult roundtrip;
% see/seen inside a consulted file truncates it (see 13_streams.pl).

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

% --- writeq/1 ---
:- roundtrip(writeq, 'needs quoting', R), check_eq(writeq_quoted_atom_roundtrips, R, 'needs quoting').
:- roundtrip(writeq, 'it''s', R), check_eq(writeq_embedded_quote_roundtrips, R, 'it''s').
:- roundtrip(writeq, foo(bar,'A B'), R), check_eq(writeq_mixed_roundtrips, R, foo(bar,'A B')).

% --- display/1 ---
:- roundtrip(display, a+b*c, R), check_eq(display_ignores_user_readability, R, a+b*c).

% --- put/1, tab/1, nl/0 (output goes to a scratch file; told must
% run before the next check, which writes to the current output) ---
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
