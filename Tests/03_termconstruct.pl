% Term analysis and synthesis: functor/3, arg/3, =../2, name/2.
% Reference: Help/help.txt =../2, functor/3, arg/3, name/2.

:- consult('harness.pl').

% --- functor/3 : decompose ---
:- functor(hello(world), N, A), check_eq(functor_decompose_name, N, hello).
:- functor(hello(world), N, A), check_eq(functor_decompose_arity, A, 1).
:- functor(foo(1,2,3), N, A), check_eq(functor_decompose_arity3, A, 3).
:- functor(atomic_val, N, A), check_eq(functor_atomic_name, N, atomic_val).
:- functor(atomic_val, N, A), check_eq(functor_atomic_arity, A, 0).
:- functor(42, N, A), check_eq(functor_number_name, N, 42).
:- functor(42, N, A), check_eq(functor_number_arity, A, 0).

% --- functor/3 : synthesize (matches manual example: functor(X,f,3) => X = f(V1,V2,V3)) ---
:- functor(X, f, 3), check(functor_synth_matches, X =.. [f,_,_,_]).
:- functor(X, foo, 0), check_eq(functor_synth_arity0, X, foo).

% --- arg/3 ---
% NOTE: Help/help.txt documents arg/3 as arg(Term, N, SubTerm), with
% the worked example "arg(hello(first,second(Y)),2,X) => X=second(Y)".
% That is NOT what this build implements: Sources/eval.c's DOARG
% requires the integer FIRST (arg(N, Term, SubTerm), the usual
% ISO-Prolog order) -- A0 must be INTT or it raises ARGE (error 2).
% This is a real doc/implementation mismatch worth fixing in the docs;
% these tests pin down the actual, current behavior.
% arg/3's error path (Sources/eval.c DOARG) uses ABORT(ARGE), not
% ERROR(ARGE) -- unlike most error codes, ABORT() jumps straight to
% the top level via ABORT_JMP() and bypasses the error/2 hook
% entirely, so it can't be probed with check_error here. Only the
% success paths are covered.
:- arg(2, hello(first,second(q)), X), check_eq(arg_example, X, second(q)).
:- arg(1, foo(a,b,c), X), check_eq(arg_first, X, a).
:- arg(3, foo(a,b,c), X), check_eq(arg_last, X, c).

% --- =../2 (manual examples) ---
:- 7 =.. L, check_eq(univ_number, L, [7]).
:- hello(world) =.. L, check_eq(univ_compound, L, [hello|[world]]).
:- X =.. [das, ist, [ein,test], y], check_eq(univ_synth, X, das(ist,[ein,test],y)).
:- foo(a,b) =.. L, check_eq(univ_roundtrip, L, [foo,a,b]).
:- X =.. [foo,a,b], check_eq(univ_synth_roundtrip, X, foo(a,b)).
:- atomval =.. L, check_eq(univ_atom, L, [atomval]).

% --- name/2 (manual examples) ---
:- name(hello, X), check_eq(name_atom_to_string, X, "hello").
:- name(17, X), check_eq(name_number_to_string, X, "17").
:- name(X, "17"), check_eq(name_string_to_atom_17, X, '17').
:- check_not(name_string_to_atom_is_not_number, (name(X,"17"), integer(X))).
:- name(X, "foo"), check_eq(name_roundtrip, X, foo).

:- summary.
:- halt.
