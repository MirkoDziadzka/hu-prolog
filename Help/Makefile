#CFLAGS=-DDEBUG=1
CFLAGS=

help.lib: help.txt help.idx help.header 
	cat help.header >  $@
	cat help.idx	>> $@
	cat help.txt	>> $@

help.idx: mkidx help.txt
	./mkidx help.txt > $@

help.error: help.txt ckfile
	./ckfile help.txt > $@

#help.txt: mkhelp ckfile NON_EXISTING_FILE
#	mkhelp HUP > $@
#	ckfile $@ > help.error


NON_EXISTING_FILE:

strip:
	expand help.txt | sed -e 's/ $$//'  > ttt
	mv ttt help.txt

clean:
	rm -f mkidx help.lib help.idx
	rm -f ckfile help.error
	rm -f chk-tree

