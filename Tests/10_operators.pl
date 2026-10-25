% Operator declarations: op/3, current_op/3, display/1.
% Reference: user-manual-german-2.25.pdf chapter 5 (Syntax und
% Operatordeklarationen) and Help/help.txt.

:- consult('harness.pl').

% --- display/1 shows the fully-bracketed functor form (manual examples) ---
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

% --- current_op/3 ---
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

% --- op/3: define a new operator and use it ---
:- op(700, xfx, likes).
:- X = (anton likes beer), check_eq(custom_op_parses, X, likes(anton,beer)).
:- roundtrip_display(anton likes beer, R), check_eq(custom_op_display, R, likes(anton,beer)).

:- op(100, xfx, von), op(50, xfy, und), op(700, xfx, ist).
:- X = (heidi ist schwester von karl und anna),
   check_eq(custom_op_sentence_example, X, ist(heidi, von(schwester, und(karl,anna)))).

% --- op(0, Type, Name) removes a declaration ---
:- op(700, xfx, foo_op_tmp), check(op_define_tmp, current_op(700, xfx, foo_op_tmp)).
:- op(0, xfx, foo_op_tmp), check_not(op_remove_tmp, current_op(_, _, foo_op_tmp)).

:- summary.
:- halt.
