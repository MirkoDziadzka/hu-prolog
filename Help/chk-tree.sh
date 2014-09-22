:
HELPFILE=help.txt
COMPRESSED=help.compressed
ROOT="HU-Prolog Online Help"
TREE=help.tree
TMP=tmp.$$

trap 'rm -f ${TMP}' 0
echo strip text

cat ${HELPFILE} |\
awk  '/^#.*/	{ printf "\n%s", $0 ; } \
      /^@.*/	{ printf "%s", $0 ; }' 	> ${COMPRESSED}


echo ${ROOT} > ${TREE}

while true
do
    echo ============ next step ================
    step ${COMPRESSED} < ${TREE} | sort | uniq > ${TMP}
    if cmp -s ${TREE} ${TMP}
    then
	break
    else
	diff ${TMP} ${TREE} | grep "^<" | sed -e "s/^< //"
	#echo =================================  >> log
	#cat ${TMP} 				>> log
	mv ${TMP} ${TREE}
    fi

done


