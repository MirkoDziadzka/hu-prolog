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
% Help write/1:
%   write(Term) writes its argument to the current output stream.
%   The operator declarations currently in effect are used to
%   make the output as readable as possible.
%
%   ttywrite/1 works the same way, but always writes to 'stdout'
:- roundtrip(write, hello(world), R), check_eq(write_compound_roundtrips, R, hello(world)).
:- roundtrip(write, 1+2, R), check_eq(write_operator_roundtrips, R, 1+2).
:- roundtrip(write, [1,2,3], R), check_eq(write_list_roundtrips, R, [1,2,3]).
:- output_of(write(a+b*c), T), check_eq(write_uses_operators, T, "a + b * c").
:- output_of(write('needs q'), T), check_eq(write_does_not_quote, T, "needs q").
:- output_of((ttywrite(x), ttynl), T), check_eq(ttywrite_writes_to_stdout, T, "").

% --- writeq/1 ---
% Help writeq/1:
%   writeq(Term) writes its argument to the current output stream.
%   Atoms are enclosed in quotes where necessary. Unlike with
%   write/1, they can therefore always be read back in. Unlike
%   display/1, however, writeq/1 uses the current operator
%   declarations to make the output readable.
:- roundtrip(writeq, 'needs quoting', R), check_eq(writeq_quoted_atom_roundtrips, R, 'needs quoting').
:- roundtrip(writeq, 'it''s', R), check_eq(writeq_embedded_quote_roundtrips, R, 'it''s').
:- roundtrip(writeq, foo(bar,'A B'), R), check_eq(writeq_mixed_roundtrips, R, foo(bar,'A B')).
:- output_of(writeq('needs q'), T), check_eq(writeq_quotes_when_needed, T, "'needs q'").
:- output_of(writeq(foo), T), check_eq(writeq_does_not_quote_plain_atom, T, "foo").
:- output_of(writeq(1+2), T), check_eq(writeq_uses_operators, T, "1 + 2").

% --- display/1 ---
% Help display/1:
%   display(Term) writes its argument to the current output stream.
%   The term is ALWAYS written in normal form, that is, without
%   taking operator declarations into account.
%   That is not very readable for the user, but it is for the
%   Prolog system. A term written with display/1 should always
%   be readable with read/1.
:- roundtrip(display, a+b*c, R), check_eq(display_ignores_user_readability, R, a+b*c).
:- output_of(display(a+b*c), T), check_eq(display_ignores_operators, T, "+(a, *(b, c))").

% --- put/1, tab/1, nl/0 (output goes to a scratch file; told must
% run before the next check, which writes to the current output) ---
% Help put/1:
%   put(N) expects as its argument either a number or a list of
%   numbers. Each number is interpreted as an ASCII value, and the
%   corresponding character is written to the current output
%   stream.
%
%   ttyput/1 works the same way, but always writes to 'stdout'
%
%   Example:
%
%   ?- put(65).
%   A
%   yes
%   ?- put("hello"). % "hello" = [104, 101, 108, 108, 111]
%   hello
%   yes
% Help tab/1:
%   Definition:
%
%       tab(N) :- N =< 0,!.
%       tab(N) :-
%           put(32),        % a space
%           N1 is N -1,
%           tab(N1).
%
%   ttytab/1 works the same way, but always writes to 'stdout'
% Help nl/0:
%   nl writes the end-of-line marker(s) to the current
%   output stream.
%
%   nl :-
%       put("\n").
%
%   ttynl/0 works like nl/0, but always writes to 'stdout'
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
:- output_of(put(65), T), check_eq(put_writes_char_of_code, T, "A").
:- output_of(put("hi"), T), check_eq(put_writes_code_list, T, "hi").
:- output_of(tab(3), T), check_eq(tab_writes_n_spaces, T, "   ").
:- output_of(tab(0), T), check_eq(tab_zero_writes_nothing, T, "").
:- output_of((ttyput(65), ttytab(1), ttynl), T), check_eq(ttyput_ttytab_write_to_stdout, T, "").

:- summary.
:- halt.
