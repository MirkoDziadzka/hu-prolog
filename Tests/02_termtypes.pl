% Term classification predicates.
% Reference: Help/help.txt sections atom/1, atomic/1, number/1,
% integer/1, real/1, var/1, nonvar/1, ground/1, compound/1, list/1,
% string/1.

:- consult('harness.pl').

% --- atom/1 ---
% Help atom/1:
%   atom(X) is true if its argument is an atom.
:- check(atom_pos_1, atom(foo)).
:- check(atom_pos_2, atom([])).
:- check_not(atom_neg_int, atom(1)).
:- check_not(atom_neg_var, atom(_)).
:- check_not(atom_neg_compound, atom(foo(1))).

% --- integer/1, real/1, number/1 ---
% Help integer/1:
%   integer(N) is true exactly if `N' is an integer.
% Help real/1:
%   real(R) is true exactly if `R' is a real number.
% Help number/1:
%   Definition:
%
%       number(N) :- integer(N).
%       number(N) :- real(N).
:- check(integer_pos, integer(1)).
:- check_not(integer_neg_real, integer(1.0)).
:- check_not(integer_neg_atom, integer(foo)).
:- check(real_pos, real(1.0)).
:- check_not(real_neg_int, real(1)).
:- check(number_pos_int, number(1)).
:- check(number_pos_real, number(1.0)).
:- check_not(number_neg_atom, number(foo)).

% --- atomic/1 ---
% Help atomic/1:
%   Definition:
%
%       atomic(X) :- atom(X).
%       atomic(X) :- number(X).
:- check(atomic_atom, atomic(foo)).
:- check(atomic_int, atomic(1)).
:- check(atomic_real, atomic(1.0)).
:- check_not(atomic_neg_var, atomic(_)).
:- check_not(atomic_neg_compound, atomic(foo(1))).

% --- var/1, nonvar/1 ---
% Help var/1:
%   var(X) is true exactly if its argument is an unbound
%   variable.
% Help nonvar/1:
%   Definition:
%
%       nonvar(X) :- not var(X).
:- check(var_pos, var(_)).
:- check_not(var_neg_bound, (X = 1, var(X))).
:- check(nonvar_pos_atom, nonvar(foo)).
:- check(nonvar_pos_bound, (X = 1, nonvar(X))).
:- check_not(nonvar_neg_unbound, nonvar(_)).

% --- compound/1 ---
% Help compound/1:
%   Definition:
%
%       compound(X) :- not atomic(X) , not var(X).
:- check(compound_pos, compound(foo(1))).
:- check(compound_pos_list, compound([1,2])).
:- check_not(compound_neg_atom, compound(foo)).
:- check_not(compound_neg_number, compound(1)).
:- check_not(compound_neg_var, compound(_)).

% --- ground/1 ---
% Help ground/1:
%   ground( Term ) is true exactly if the argument contains no
%   free variables.
:- check(ground_pos_atom, ground(foo)).
:- check(ground_pos_compound, ground(foo(1,2))).
:- check_not(ground_neg_var, ground(_)).
:- check_not(ground_neg_compound_with_var, ground(foo(1,_))).

% --- list/1 ---
% Help list/1:
%   Definition:
%
%       list(X)  :-
%           var(X), ! fail.
%       list([]).
%       list([_|T]) :-
%           list(T).
:- check(list_pos_empty, list([])).
:- check(list_pos_nonempty, list([1,2,3])).
:- check_not(list_neg_atom, list(foo)).
:- check_not(list_neg_open, list([1|_])).
:- check_not(list_neg_var, list(_)).

% --- string/1 ---
% Help string/1:
%   Definition:
%
%       string(X)  :-
%           var(X), ! fail.
%       string([]).
%       string([Val|T]) :-
%           (Val >= 32 , Val =< 127), % or (Val >= 0 , Val =< 255)
%           string(T).
:- check(string_pos_empty, string([])).
:- check(string_pos_charlist, string("hello")).
:- check_not(string_neg_atom, string(foo)).
:- check_not(string_neg_non_char_list, string([1,2,999])).
:- check_not(string_neg_var, string(_)).
:- check(string_pos_bounds_32_127, string([32,127])).
:- check_not(string_neg_above_255, string([256])).

:- summary.
:- halt.
