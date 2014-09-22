:
tmp=tmp$$
len=19
make header.txt
for i in $*
do
    echo -n change header in $i ...
    cp header.txt $tmp
    tail +$len $i >> $tmp
    mv $tmp  $i
    echo 
done


