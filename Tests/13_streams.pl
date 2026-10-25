% Stream operations: open/1, close/1, tell/1, told/0, telling/1,
% see/1, seen/0, seeing/1, get0/1, read/1.
% Reference: Help/help.txt Streamoperationen, Eingabeoperationen.
%
% IMPORTANT: see/1 and seen/0 switch the global input stream
% (Sources/io.c inputfile) that DOCONSULT reads this file from, and
% seen/0 resets it to user. A see/seen inside a directive therefore
% ends the consult of this file. The see/1 checks below run last,
% chained with summary/halt in the same directive.

:- consult('harness.pl').

% --- tell/1, telling/1, told/0 ---
% Help tell/1:
%   tell(Stream) opens the given stream for writing and makes it
%   the current output stream.
% Help telling/1:
%   telling(X) unifies its argument with the name of the current
%   output stream.
% Help told/0:
%   told closes the current output stream. Afterwards `stdout'
%   becomes the current output stream.
:- tell('/tmp/hu_stream_test.pl'),
   telling(T),
   told,
   check_eq(telling_reports_current_output, T, '/tmp/hu_stream_test.pl').
:- telling(T), check_eq(told_restores_stdout, T, stdout).
:- tell('/tmp/hu_stream_test.pl'), close('/tmp/hu_stream_test.pl'),
   telling(T), check_eq(close_of_current_output_restores_stdout, T, stdout).

% --- open/1 ---
% Help open/1:
%   open(Stream) opens the given stream for reading and writing.
%   Unlike `see/1' and `tell/1', the current input or output
%   stream is not changed.
:- seeing(Before),
   open('/tmp/hu_stream_test.pl'),
   seeing(After),
   check_eq(open_does_not_change_current_input, After, Before).
:- telling(Before2),
   open('/tmp/hu_stream_test.pl'),
   telling(After2),
   check_eq(open_does_not_change_current_output, After2, Before2).
:- close('/tmp/hu_stream_test.pl').

% --- too many open streams ---
% The file table holds MAXFILES (Sources/files.h) streams. Opening one
% more raises error 40 (TOMANY in Sources/types.h).
codes_append([], L, L).
codes_append([H|T], L, [H|R]) :- codes_append(T, L, R).

many_file(N, F) :-
    name(N, Digits),
    codes_append("/tmp/hu_many_", Digits, Base),
    codes_append(Base, ".pl", Codes),
    name(F, Codes).

open_many(N, Max) :- N > Max, !.
open_many(N, Max) :-
    many_file(N, F), open(F),
    N1 is N + 1, open_many(N1, Max).

close_many(N, Max) :- N > Max, !.
close_many(N, Max) :-
    many_file(N, F), ( close(F) -> true ; true ),
    N1 is N + 1, close_many(N1, Max).

:- check_error(open_too_many_streams_raises_error, open_many(1, 25), 40),
   close_many(1, 25).

% --- write known content, then read it back with see/1 + get0/1 +
% read/1. This is the risky part: it must be the last thing in the
% file, chained with summary/halt in one directive. ---
% Help see/1:
%   see(Stream) opens the given stream for reading and makes it
%   the current input stream.
% Help get0/1:
%   get0(ASCII) reads a character from the current input stream and
%   unifies the ASCII value of that character with the argument of
%   get0/1. At the end of the file the ASCII value -1 is used.
%
%   ttyget0/1 works like get0/1, but reads from 'stdin'
% Help read/1:
%   read(X) reads a term from the current input stream and
%   unifies it with `X'. At the end of the file the term `end'
%   is returned.
%
%   ttyread/1 works like read/1, but reads from 'stdin'
:- tell('/tmp/hu_stream_test.pl'),
   put(65), put(66), put(67),            % "ABC"
   nl,
   write(hello(world)), write('.'), nl,
   told,
   see('/tmp/hu_stream_test.pl'),
   get0(C1), get0(C2), get0(C3), get0(Nl),
   read(Term),
   ( C1 == 65, C2 == 66, C3 == 67, Nl == 10 -> GetOk = ok ; GetOk = failed ),
   ( Term == hello(world) -> ReadOk = ok ; ReadOk = failed ),
   seeing(SeeName),
   read(AtEnd), get0(EofCode),
   seen, seeing(AfterSeen),
   check_eq(get0_reads_written_bytes, GetOk, ok),
   check_eq(read_reads_written_term, ReadOk, ok),
   check_eq(see_makes_current_input, SeeName, '/tmp/hu_stream_test.pl'),
   check_eq(read_at_eof_returns_end, AtEnd, end),
   check_eq(get0_at_eof_returns_minus_1, EofCode, -1),
   check_eq(seen_restores_stdin, AfterSeen, stdin),
   summary,
   halt.
