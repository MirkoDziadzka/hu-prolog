:
if [ ! -d $1 ] 
then
    exit 1
fi

if [ ! -d $2 ] 
then
    exit1
fi

PWD=`pwd`

cd $1 ; PWD1=`pwd`
cd $PWD ; cd $2 ; PWD2=`pwd`

for i in ${PWD1}/* 
do
    FILE=`basename $i`
    if [ ! -f ${PWD2}/$FILE ]
    then
	echo ${PWD2}/$FILE do not exists
    else
	echo ================== $FILE ======================
	diff ${PWD1}/$FILE ${PWD2}/$FILE
    fi
done

for i in ${PWD2}/* 
do
    FILE=`basename $i`
    if [ ! -f ${PWD1}/$FILE ]
    then
	echo ${PWD1}/$FILE do not exists
    fi
done

