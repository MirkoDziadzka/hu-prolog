% Operator declarations: op/3, current_op/3, display/1.
% Reference: user-manual-german-2.25.pdf chapter 5 (Syntax und
% Operatordeklarationen) and Help/help.txt.

:- consult('harness.pl').

% --- display/1 shows the fully-bracketed functor form (manual examples) ---
% Help display/1:
%   display(Term) writes its argument to the current output stream.
%   The term is ALWAYS written in normal form, that is, without
%   taking operator declarations into account.
%   That is not very readable for the user, but it is for the
%   Prolog system. A term written with display/1 should always
%   be readable with read/1.
% (via tell + consult roundtrip, see 08_io.pl)
roundtrip_display(Term, Result) :-
    tell('/tmp/hu_display_rt.pl'),
    write(':- assertz(''$disp_result''('),
    display(Term),
    write(')).'),
    nl,
    told,
    consult('/tmp/hu_display_rt.pl'),
    retract('$disp_result'(Result)).

:- roundtrip_display(a + b * c, R), check_eq(display_precedence_example1, R, '+'(a, '*'(b,c))).
:- roundtrip_display(a - b - c, R), check_eq(display_leftassoc_example, R, '-'('-'(a,b),c)).
:- roundtrip_display([a,b,c], R), check_eq(display_list_example, R, '.'(a,'.'(b,'.'(c,[])))).
:- output_of(display(a + b * c), T), check_eq(display_text_example1, T, "+(a, *(b, c))").
:- output_of(display(a - b - c), T), check_eq(display_text_leftassoc, T, "-(-(a, b), c)").

% --- current_op/3 ---
% Help current_op/3:
%   A call current_op(V,A,N) unifies V, A and N with the precedence,
%   the associativity and the name of an operator known to the system.
%   New solutions are generated on backtracking.
%   The following program lists all operators known to the
%   interpreter.
%
%       list_op     :-
%           current_op(V,A,N),
%           write((?-op(V,A,N)),nl,
%           fail.
% The manual's operator table lists ':-' as 1200 xfy; this build has
% 1200 fx and 1200 xfx.
:- check(current_op_finds_rule_arrow, current_op(1200, xfx, ':-')).
:- check(current_op_finds_directive_prefix, current_op(1200, fx, ':-')).
:- check(current_op_finds_comma, current_op(1000, xfy, ',')).
:- check(current_op_finds_plus, current_op(500, yfx, '+')).
:- check(current_op_finds_times, current_op(400, yfx, '*')).
:- check(current_op_finds_cons, current_op(300, xfy, '.')).
:- current_op(P, _, '+'), check_eq(current_op_plus_precedence, P, 500).
:- current_op(_, A, '+'), check_eq(current_op_plus_assoc, A, yfx).
:- collect_all(current_op(P, T, '-'), P-T, L),
   check_eq(current_op_backtracks_over_solutions, L, [500-yfx, 300-fy]).

% --- op/3: define a new operator and use it ---
% Help op/3:
%   op/3 makes one or more new operators known to the interpreter.
%   In a call `op(P , A , O )', P is the priority, A the
%   associativity and O the name, or a list of names, of the
%   operators to be defined.
%
%   Numbers in the range 1 - 1200 (2000) are allowed for P. A higher
%   priority *separates* more strongly (it does not bind more strongly).
%   If 0 is given as the priority, the operator declaration is
%   removed.
%
%   The values `xf' and `yf' are allowed for A for postfix operators,
%   `fx' and `fy' for prefix operators, and `xfx', `xfy' and `yfx'
%   for infix operators.
%
%   The operators currently defined in the system can be obtained
%   with `current_op/3'.
%
%   For the purpose of operators, consult a Prolog book.
:- op(700, xfx, likes).
:- X = (anton likes beer), check_eq(custom_op_parses, X, likes(anton,beer)).
:- roundtrip_display(anton likes beer, R), check_eq(custom_op_display, R, likes(anton,beer)).

:- op(100, xfx, von), op(50, xfy, und), op(700, xfx, ist).
:- X = (heidi ist schwester von karl und anna),
   check_eq(custom_op_sentence_example, X, ist(heidi, von(schwester, und(karl,anna)))).
:- op(700, xfx, [opl1, opl2]),
   check(op_list_of_names, (current_op(700, xfx, opl1), current_op(700, xfx, opl2))).
:- op(100, yfx, tight), op(900, yfx, loose).
:- X = (a tight b loose c), check_eq(op_higher_priority_separates, X, loose(tight(a,b),c)).
:- op(200, xf, bang).
:- X = (3 bang), check_eq(op_postfix_xf, X, bang(3)).
:- op(200, fx, pre).
:- X = (pre a), check_eq(op_prefix_fx, X, pre(a)).
:- check(op_priority_2000_allowed, op(2000, xfx, opmax)).
:- check_error(op_bad_type_rejected, op(700, xyz, opbadtype), 2).

% --- op(0, Type, Name) removes a declaration ---
:- op(700, xfx, foo_op_tmp), check(op_define_tmp, current_op(700, xfx, foo_op_tmp)).
:- op(0, xfx, foo_op_tmp), check_not(op_remove_tmp, current_op(_, _, foo_op_tmp)).

:- summary.
:- halt.
