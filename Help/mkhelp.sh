:
if [ -d $1 ]
then 
    for i in $1/*
    do
	cat $i 
    done
else
	echo usage: $0 directory
	exit 1
fi
exit 0

