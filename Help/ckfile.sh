:
FILE=help.txt
TMP0=tmp.$$.0
TMP1=tmp.$$.1
TMP2=tmp.$$.2
TMP3=tmp.$$.3

trap 'rm -f ${TMP0} ${TMP1} ${TMP2} ${TMP3}' 0 1 2 3

grep "^#" $FILE | sed -e "s/^#//" | tee ${TMP0} | sort -u > ${TMP1}
grep "^@" $FILE | sed -e "s/^@//" > ${TMP2}
grep "^:" $FILE | sed -e "s/^://" >> ${TMP2}
sort -u ${TMP2} > ${TMP3}

echo ---- DEFINED BUT NEVER USED ----
diff ${TMP1} ${TMP3} | grep "^<" | sed -e 's/^< //'
echo ---- USED BUT NEVER DEFINED ----
diff ${TMP1} ${TMP3} | grep "^>" | sed -e 's/^> //'
echo ---- DOUBLE DEFINITIONS ----
sort ${TMP0} | uniq -d
echo ---- END ----

