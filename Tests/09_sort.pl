% sort/2 and sort0/2.
% Reference: Help/help.txt "sort/2 und sort0/2".

:- consult('harness.pl').

:- sort("abccba", X), check_eq(sort_manual_example, X, "aabbcc").
:- sort0("abccbd", X), check_eq(sort0_manual_example_corrected, X, "abcd").
:- sort([3,1,2], X), check_eq(sort_numbers, X, [1,2,3]).
:- sort([c,a,b], X), check_eq(sort_atoms, X, [a,b,c]).
:- sort([], X), check_eq(sort_empty, X, []).
:- sort([1,1,1], X), check_eq(sort_keeps_duplicates, X, [1,1,1]).
:- sort0([1,1,1], X), check_eq(sort0_removes_duplicates, X, [1]).
:- sort0([3,1,2,1,3], X), check_eq(sort0_dedups_and_sorts, X, [1,2,3]).
:- sort([b,1,a(1),_], [S1,S2,S3,S4]),
   check(sort_mixed_types_var_first, var(S1)),
   check_eq(sort_mixed_types_number_second, S2, 1),
   check_eq(sort_mixed_types_name_a_third, S3, a(1)),
   check_eq(sort_mixed_types_name_b_fourth, S4, b).

:- summary.
:- halt.
