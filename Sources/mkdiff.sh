TEMP=/tmp/tmp.$$
DIFF="diff -c"
SHAR="shar  -m -M -c -F -f"

if [ $# -ne 1 ]
then
    echo usage $0 patchdir
    exit 1
fi
RELEASE=`cat RELEASE`
PATCHLEVEL=`cat PATCHLEVEL`
PATCH_DIR=$1
PATCH_FILE=${PATCH_DIR}/patches-${RELEASE}.${PATCHLEVEL}
NEW_FILES=${PATCH_DIR}/shar-${RELEASE}.${PATCHLEVEL}
REF_DIR=../Release-${RELEASE}.${PATCHLEVEL}

rm -f ${PATCH_FILE}
rm -f ${NEW_FILES}
rm -f ${TEMP}

echo \# use this patch on HU-Prolog ${RELEASE}.${PATCHLEVEL} >> ${PATCH_FILE}

for file in *
do
    if [ -f $file ]
    then
	if [ -f ${REF_DIR}/$file ]
	then
	    ${DIFF} ${REF_DIR}  $file >>  ${PATCH_FILE}
	else
	    echo new file $file
	    echo $file >> ${TEMP}
	fi
    else
	echo $file isn\'t a regulary file
    fi
done

if [ -f ${TEMP} ]
then
    echo ${SHAR} `cat ${TEMP}` 
    ${SHAR} `cat ${TEMP}` > ${NEW_FILES}
fi

rm -f ${TEMP}

