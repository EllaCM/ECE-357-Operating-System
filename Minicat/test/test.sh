#!/bin/bash
i="1"
a="output"
b="test.txt"
exec 3<> result.txt
while [ $i -le 262144 ]
do
    echo "Timing Minicat with buffersize $i bytes" >&3
#   {time ./minicat -b $i -o $a $b;} 2>&3
{ time ./minicat -b $i -o $a $b; } >&3 2>&1
#echo $i
    ((i=$[$i*2]))
done
exec 3>&-
