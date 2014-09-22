show_current_time :-
	time(H,M,S),
	write(time(H,M,S)),
	nl.

test_timer :-
	show_current_time,
	timer(0),
	repeat,
	    timer(X),
        X > 60*100,
        !,
        show_current_time,
        write('Das muessten jetzt genau 60 Sekunden gewesen sein !\n').


