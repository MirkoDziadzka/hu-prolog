% Stream operations: open/1, close/1, tell/1, told/0, telling/1,
% see/1, seen/0, seeing/1, get0/1, read/1.
% Reference: Help/help.txt Streamoperationen, Eingabeoperationen.
%
% IMPORTANT, read before adding more tests here: see/1 and seen/0
% mutate the single GLOBAL "current input stream" variable
% (Sources/io.c inputfile). The top-level file consult loop
% (Sources/datab.c DOCONSULT) ALSO reads through that same global, and
% while DOCONSULT saves/restores it around *nested* consult/1 calls,
% it has no idea a *raw* see/1 call happened underneath it. seen/0
% unconditionally resets the stream to 'user' (stdin) rather than
% whatever it was before -- so calling see/1 then seen/0 from within a
% directive that's itself being consulted silently truncates the rest
% of this file: DOCONSULT's next read sees stdin/EOF immediately,
% treats that as `end`, and control falls through to the interactive
% toplevel instead of running any later directives.
%
% The fix used below: any check that calls see/1 raw must be the LAST
% thing that happens, with summary/halt chained into the SAME
% directive (via ,/2) so no further top-level term is ever read after
% the stream gets switched. See harness.pl's collect_all/3 and
% 06_control.pl for the analogous assert/retract hazard.

:- consult('harness.pl').

% --- tell/1, telling/1, told/0 ---
:- tell('/tmp/hu_stream_test.pl'),
   telling(T),
   told,
   check_eq(telling_reports_current_output, T, '/tmp/hu_stream_test.pl').

% --- open/1 : opens without changing the current input/output stream
% (per help.txt, unlike see/1 and tell/1) ---
:- seeing(Before),
   open('/tmp/hu_stream_test.pl'),
   seeing(After),
   check_eq(open_does_not_change_current_input, After, Before).
:- telling(Before2),
   open('/tmp/hu_stream_test.pl'),
   telling(After2),
   check_eq(open_does_not_change_current_output, After2, Before2).
:- close('/tmp/hu_stream_test.pl').

% --- write known content, then read it back with see/1 + get0/1 +
% read/1. This is the risky part: it must be the last thing in the
% file, chained with summary/halt in one directive. ---
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
   check_eq(get0_reads_written_bytes, GetOk, ok),
   check_eq(read_reads_written_term, ReadOk, ok),
   summary,
   halt.
