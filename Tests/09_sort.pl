% sort/2 and sort0/2.
% Reference: Help/help.txt "sort/2 und sort0/2".
%   ?- sort("abccba",X).   X = "aabbcc"
%   ?- sort0("abccbd",X).  X = "abc"   (sort0 removes duplicates)

:- consult('harness.pl').

:- sort("abccba", X), check_eq(sort_manual_example, X, "aabbcc").
% NOTE: Help/help.txt's own worked example says
% "?- sort0("abccbd",X). X = "abc"" but that looks like a plain typo
% in the manual: the input "abccbd" contains a 'd', so the only
% sensible deduplicated-and-sorted result is "abcd", which is what
% this build actually produces.
:- sort0("abccbd", X), check_eq(sort0_manual_example_corrected, X, "abcd").
:- sort([3,1,2], X), check_eq(sort_numbers, X, [1,2,3]).
:- sort([c,a,b], X), check_eq(sort_atoms, X, [a,b,c]).
:- sort([], X), check_eq(sort_empty, X, []).
:- sort([1,1,1], X), check_eq(sort_keeps_duplicates, X, [1,1,1]).
:- sort0([1,1,1], X), check_eq(sort0_removes_duplicates, X, [1]).
:- sort0([3,1,2,1,3], X), check_eq(sort0_dedups_and_sorts, X, [1,2,3]).
% sort order follows @</2 (standard order of terms): unbound var <
% number < everything else (atoms and compounds together, ordered by
% functor NAME first, then arity -- so a(1) with name "a" sorts
% before the atom "b", per the confirmed a(1) @< b rule).
:- sort([b,1,a(1),_], [S1,S2,S3,S4]),
   check(sort_mixed_types_var_first, var(S1)),
   check_eq(sort_mixed_types_number_second, S2, 1),
   check_eq(sort_mixed_types_name_a_third, S3, a(1)),
   check_eq(sort_mixed_types_name_b_fourth, S4, b).

:- summary.
:- halt.
