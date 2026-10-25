% Term analysis and synthesis: functor/3, arg/3, =../2, name/2.
% Reference: Help/help.txt =../2, functor/3, arg/3, name/2.

:- consult('harness.pl').

% --- functor/3 : decompose ---
% Help functor/3:
%   functor(Term,Name,Arity) decomposes the term given as the first
%   argument into the name ``Name'' and the arity
%   ``Arity'' of its principal functor.
%
%   Example:
%
%   ?- functor(hello(world),X,Y).
%   X = hello
%   Y = 1
%   ?- functor(X,f,3).
%   X = f(V1,V2,V3)
:- functor(hello(world), N, A), check_eq(functor_decompose_name, N, hello).
:- functor(hello(world), N, A), check_eq(functor_decompose_arity, A, 1).
:- functor(foo(1,2,3), N, A), check_eq(functor_decompose_arity3, A, 3).
:- functor(atomic_val, N, A), check_eq(functor_atomic_name, N, atomic_val).
:- functor(atomic_val, N, A), check_eq(functor_atomic_arity, A, 0).
:- functor(42, N, A), check_eq(functor_number_name, N, 42).
:- functor(42, N, A), check_eq(functor_number_arity, A, 0).

% --- functor/3 : synthesize ---
:- functor(X, f, 3), check(functor_synth_matches, X =.. [f,_,_,_]).
:- functor(X, foo, 0), check_eq(functor_synth_arity0, X, foo).
:- functor(X, f, 3), X = f(A,B,C),
   check(functor_synth_args_distinct_vars,
         (var(A), var(B), var(C), A \== B, B \== C, A \== C)).

% --- arg/3 ---
% Help arg/3:
%   arg(N, Term, SubTerm) is used for the analysis and synthesis
%   of terms. arg/3 unifies ``SubTerm'' with the
%   ``N''-th subterm of ``Term''.
%   Example:
%
%   ?- arg( 2 , hello(first, second(Y)) , X).
%   X = second(Y).
:- arg(2, hello(first,second(q)), X), check_eq(arg_example, X, second(q)).
:- arg(1, foo(a,b,c), X), check_eq(arg_first, X, a).
:- arg(3, foo(a,b,c), X), check_eq(arg_last, X, c).
:- arg(2, hello(first, second(Y)), X), check(arg_example_shares_var, X == second(Y)).
:- T = foo(_), arg(1, T, a), check_eq(arg_synthesis_binds_subterm, T, foo(a)).

% arg/3 errors (Sources/eval.c DOARG)
:- check_error(arg_out_of_range, arg(3, foo(a,b), _), 2).
:- check_error(arg_index_not_integer, arg(foo(a,b,c), 1, _), 2).

% --- =../2 ---
% Help =../2:
%   The predicate =../2 is used for the analysis and synthesis of
%   terms. The second argument is a list of the components of the
%   first argument.
%
%   Example:
%   ?- 7 =.. X.
%       X = [7].
%   ?- hello(world) =.. X.
%       X = [hello | world]
%   ?- X =.. [this, is, [a , test], Y].
%       X = this(is, [a, test], Y)
%   ?- X =.. [Y].
%           ERROR .....     % not sufficiently instantiated
:- 7 =.. L, check_eq(univ_number, L, [7]).
:- hello(world) =.. L, check_eq(univ_compound, L, [hello|[world]]).
:- X =.. [das, ist, [ein,test], y], check_eq(univ_synth, X, das(ist,[ein,test],y)).
:- foo(a,b) =.. L, check_eq(univ_roundtrip, L, [foo,a,b]).
:- X =.. [foo,a,b], check_eq(univ_synth_roundtrip, X, foo(a,b)).
:- atomval =.. L, check_eq(univ_atom, L, [atomval]).
:- X =.. [this, is, [a, test], Y], check(univ_synth_with_var, X == this(is, [a, test], Y)).
:- check_error(univ_single_var_list_raises_error, (_ =.. [_]), 2).
:- check_error(univ_single_compound_list_raises_error, (_ =.. [f(a)]), 2).
:- X =.. [foo], check_eq(univ_single_atom_list, X, foo).
:- X =.. [1.5], check(univ_single_number_list, X =:= 1.5).
:- Y = foo, X =.. [Y], check_eq(univ_single_bound_var_list, X, foo).

% --- name/2 ---
% Help name/2:
%   name(Atom, String) decomposes atoms into the characters that
%   represent the name of the atom, and vice versa.
%
%   Example:
%
%   ?- name(hello,X).
%   X = "hello"
%   ?- name(17,X).
%   X = "17"        % so it works with numbers too.
%   ?- name(X, "17").
%   X = '17'        % THE ATOM '17', NOT THE NUMBER 17.
:- name(hello, X), check_eq(name_atom_to_string, X, "hello").
:- name(17, X), check_eq(name_number_to_string, X, "17").
:- name(X, "17"), check_eq(name_string_to_atom_17, X, '17').
:- check_not(name_string_to_atom_is_not_number, (name(X,"17"), integer(X))).
:- name(X, "foo"), check_eq(name_roundtrip, X, foo).

:- summary.
:- halt.
