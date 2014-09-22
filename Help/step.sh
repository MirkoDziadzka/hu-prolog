:
read line
while ( test "${line}" )
do
    echo ${line}
    (grep "^#${line}@" | sed -e 's/^[^@]*@//' | tr "@" "\012" ) < $1
    read line
done
