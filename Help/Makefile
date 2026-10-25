#CFLAGS=-DDEBUG=1
CFLAGS=

all: help.lib help.en.lib

help.lib: help.txt help.idx help.header 
	cat help.header >  $@
	cat help.idx	>> $@
	cat help.txt	>> $@

help.en.lib: help.en.txt help.en.idx help.header 
	cat help.header >  $@
	cat help.en.idx	>> $@
	cat help.en.txt	>> $@

help.idx: mkidx help.txt
	./mkidx help.txt > $@

help.en.idx: mkidx help.en.txt
	./mkidx help.en.txt > $@

help.error: help.txt ckfile
	./ckfile help.txt > $@

help.en.error: help.en.txt ckfile
	./ckfile help.en.txt > $@

#help.txt: mkhelp ckfile NON_EXISTING_FILE
#	mkhelp HUP > $@
#	ckfile $@ > help.error


NON_EXISTING_FILE:

strip:
	expand help.txt | sed -e 's/ $$//'  > ttt
	mv ttt help.txt

clean:
	rm -f mkidx help.lib help.idx help.en.lib help.en.idx
	rm -f ckfile help.error help.en.error
	rm -f chk-tree

