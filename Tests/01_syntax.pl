% Syntax: atoms, numbers, variables, lists, strings, curly terms.
% Reference: user-manual-german-2.25.pdf chapter 2 and Help/help.txt.

:- consult('harness.pl').

% --- atoms ---
:- check(atom_simple_identifier, atom(hello)).
:- check(atom_quoted, atom('Hello World')).
:- check(atom_special_chars, atom(+)).
:- check(atom_special_chars2, atom(-->)).
:- check(atom_empty_list_is_atom, atom([])).
:- check(atom_curly_is_atom, atom({})).
:- check_eq(atom_quoted_equals_unquoted, foo, 'foo').
:- check_not(atom_quoted_case_sensitive, ('Foo' == foo)).

:- name(A, [105,116,39,115]), check_eq(atom_escape_quote_vs_name, A, 'it''s').

% --- numbers ---
:- check(number_pos_int, integer(42)).
:- check(number_neg_int, integer(-7)).
:- check(number_real, real(3.14)).
:- check(number_real_exponent, real(1.5e3)).
:- X is 123, check_eq(number_int_value, X, 123).
:- Y is 1.5e2, check(number_real_exponent_value, (Y =:= 150.0)).

% --- variables ---
:- check(var_unbound, var(_)).
:- check(var_uppercase_is_var, var(X)).
:- check(var_underscore_is_var, var(_Foo)).
:- X = 1, check_not(var_bound_not_var, var(X)).

% --- lists ---
:- check_eq(list_empty, [], []).
:- check(list_empty_is_atom_nil, atom([])).
:- check_eq(list_cons, [1,2,3], [1|[2,3]]).
:- L = [a|[b|[c|[]]]], check_eq(list_cons_build, L, [a,b,c]).
:- check(list_open_list_unifies, [1,2|_] = [1,2,3]).

% --- strings ---
:- check_eq(string_is_code_list, "abc", [97, 98, 99]).
:- check_eq(string_empty_is_nil, "", []).

% --- curly terms ---
:- check_eq(curly_empty_is_atom, {}, '{}').
:- X = {a}, check_eq(curly_nonempty_functor, X, '{}'(a)).
:- {Y} = {a}, check_eq(curly_nonempty_arg, Y, a).

% --- comments: if this file itself consulted cleanly, comment syntax works ---
% a line comment
/* a block
   comment */
:- check(comments_do_not_break_parsing, true).

:- summary.
:- halt.
