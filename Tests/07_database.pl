% Database manipulation: assert/asserta/assertz, retract, retractall,
% abolish/1, abolish/2, clause/2, current_predicate/1, current_atom/1.
% Reference: Help/help.txt Datenbasisoperationen.

:- consult('harness.pl').

% --- assert/1 == assertz/1 ---
% Help assert/1:
%   asserta(Clause) adds the clause ``Clause'' to the database before
%   all other clauses with the same principal functor.
%
%   assertz(Clause) adds the clause ``Clause'' to the database after
%   all other clauses with the same principal functor.
%
%   assert(Clause) :- assertz(Clause).
:- assert(fact1(a)), check(assert_adds_fact, fact1(a)).
:- assertz(fact1(b)), check(assertz_adds_fact, fact1(b)).
:- collect_all(fact1(X), X, L), check_eq(assertz_appends_after, L, [a,b]).
:- assert(fact3(a)), assert(fact3(b)), collect_all(fact3(X), X, L),
   check_eq(assert_appends_like_assertz, L, [a,b]).

% --- asserta/1 prepends ---
:- asserta(fact1(z)), collect_all(fact1(X), X, L), check_eq(asserta_prepends, L, [z,a,b]).

% --- retract/1 ---
% Help retract/1:
%   retract/1 removes the clause given as its argument from the
%   database. The argument must at least determine the name and
%   the arity of the clause head.
%   On backtracking further clauses are removed -- but see the
%   WARNING below for an important restriction when the argument
%   is given explicitly as ``(Head :- Body)''.
%   Example (argument as a bare head, without ``:-''):
%
%   Let the database contain:
%           b(1).
%           b(2).
%           b(3).
%   Then:
%           ?- retract(b(X)).
%           X = 1 ;
%           X = 2 ;
%           X = 3 ;
%           no
%
%   WARNING: If the argument is given explicitly as
%            ``(Head :- Body)'', retract/1 can ONLY match
%            clauses with a real body (that is, rules), never
%            facts -- regardless of backtracking. Internally a
%            fact is stored without a body (not with the body
%            `true'), and retract/1 then compares only the bare
%            head, not ``Head :- true''. (clause/2 hides this
%            difference from its users and returns Body=true
%            for facts -- retract/1 does not.)
%            Example:
%
%            Let the database contain:
%                    a(1) :- fail.
%                    a(2).
%            Then:
%                    ?- retract(( a(X) :- Y )).
%                    X = 1
%                    Y = fail
%                    no          % a(2) is NOT found,
%                                 % although it should match!
%
%            Workaround: either give the argument without ``:-''
%            (then it matches both facts and rules, see the
%            b/1 example above), or search separately for facts
%            (bare head) and rules (``Head :- Body'') as needed.
:- retract(fact1(z)), collect_all(fact1(X), X, L), check_eq(retract_removes_one, L, [a,b]).
:- check_not(retract_fails_if_absent, retract(fact1(nonexistent))).
:- check_error(retract_needs_principal_functor, retract(_), 2).

db_b(1). db_b(2). db_b(3).
:- collect_all(retract(db_b(X)), X, L), check_eq(retract_backtracks_bare_pattern, L, [1,2,3]).

% retract((Head :- Body)) never matches a fact: GenTerm
% (Sources/datab.c) yields a fact as its bare head.
db_a(1) :- fail.
db_a(2).
:- collect_all(retract((db_a(X) :- Y)), X-Y, L),
   check_eq(retract_headbody_pattern_skips_facts, L, [1-fail]).
:- check(retract_headbody_pattern_leaves_fact_untouched, db_a(2)).

% --- retractall/1 ---
% Help retractall/1:
%   Definition:
%           retractall( X ) :-
%               retract( X ),
%               fail.
%           retractall( _ ).
:- assertz(fact2(1)), assertz(fact2(2)), assertz(fact2(3)),
   retractall(fact2(_)),
   check_not(retractall_removes_all, fact2(_)).
:- check(retractall_succeeds_even_if_nothing_matches, (retractall(nosuch(_)), true)).

% --- abolish/1 (removes a name at all arities) ---
% Help abolish/1:
%   abolish(Name) removes all clauses of all predicates with the
%   name ``Name'' and any arity from the database.
%   abolish/1 always succeeds.
:- assertz(ab1(x)), assertz(ab1(x,y)),
   abolish(ab1),
   check_not(abolish1_removes_arity1, ab1(_)).
:- check_not(abolish1_removes_arity2, ab1(_,_)).
:- check(abolish1_succeeds_when_undefined, abolish(no_such_pred_q)).

% --- abolish/2 (removes name/arity only) ---
% Help abolish/2:
%   abolish(Name, Arity) removes all clauses of the predicate
%   Name/Arity from the database.
%   abolish/2 always succeeds.
:- assertz(ab2(x)), assertz(ab2(x,y)),
   abolish(ab2, 1),
   check_not(abolish2_removes_only_given_arity, ab2(_)).
:- check(abolish2_leaves_other_arity, ab2(_,_)).
:- check(abolish2_succeeds_when_undefined, abolish(no_such_pred_q, 3)).

% --- clause/2 ---
% Help clause/2:
%   clause(Head, Body) searches the database for a clause whose
%   head unifies with `Head' and whose body unifies with `Body'.
%   Facts have the body `true'. The argument `Head' must be
%   instantiated at least far enough to determine the principal
%   functor of the clause.
clt(1) :- true.
clt(2) :- fail.
:- clause(clt(1), Body), check_eq(clause_fact_body_is_true, Body, true).
:- clause(clt(2), Body), check_eq(clause_rule_body, Body, fail).
:- check_not(clause_no_match, clause(clt(99), _)).
:- check_error(clause_needs_principal_functor, clause(_, _), 2).

% --- current_predicate/1 ---
% Help current_predicate/1:
%   current_predicate(Name/Arity) unifies its argument on backtracking
%   with all predicates known to the interpreter.
cp_test(_).
:- check(current_predicate_finds_defined, current_predicate(cp_test/1)).
:- check_not(current_predicate_absent, current_predicate(totally_undefined_pred_xyz/3)).
in(X, [X|_]) :- !.
in(X, [_|T]) :- in(X, T).
:- collect_all(current_predicate(P), P, L),
   check(current_predicate_enumerates_on_backtracking, (in(cp_test/1, L), in(in/2, L))).

% --- current_atom/1 ---
% Help current_atom/1:
%   A call current_atom(Name / Arity) unifies Name and Arity with the
%   name and arity of an atom known to the interpreter.
%   On backtracking the predicate returns all atoms currently
%   known to the system.
:- check(current_atom_finds_atom, current_atom(cp_test/1)).
:- collect_all(current_atom(A), A, L),
   check(current_atom_enumerates_on_backtracking, (in(cp_test/1, L), in(fact1/1, L))).

:- summary.
:- halt.
