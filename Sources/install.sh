#! /bin/sh
#
cmd="cp"
#chmod="chmod 755"
#chown="chown bin"
#chgrp="chgrp bin"
chmod=echo
chown=echo
chgrp=echo

while true ; do
	case $1 in
		-c )	cmd="$2"
			shift	
			shift
			;;
		-m )	chmod="chmod $2"
			shift
			shift
			;;
		-o )	chown="chown $2"
			shift
			shift
			;;
		-g )	chgrp="chgrp $2"
			shift
			shift
			;;
		* )	break
			;;
	esac
done

if [ ! ${2-""} ]
then	echo "install: no destination specified"
	exit 1
fi
if [ ${3-""} ]
then	echo "install: too many files specified -> $*"
	exit 1
fi
if [ $1 = $2 -o $2 = . ]
then	echo "install: can't move $1 onto itself"
	exit 1
fi
if [ '!' -f $1 ]
then	echo "install: can't open $1"
	exit 1
fi
if [ -d $2 ]
then	file=$2/`basename $1`
else	file=$2
fi

$cmd $1 $file

$chown $file
$chgrp $file
$chmod $file

