#!/bin/bash
node=(40 80)
edge=(100 210)
scale=(100)
edgeTimeOut=3
solveTimeOut=15
#trial=10
shortest=4
longest=15
target=2
for idx in ${!node[@]}
do
  n=${node[$idx]}
  e=${edge[$idx]}
  s=${scale[$idx]}
  inputFileName="random_${n}_${e}"
  current=0
  while [ $current -lt $target ]
  do
    output=$(./FourColor ${n} ${e} ${s} ${edgeTimeOut} ${solveTimeOut} ${inputFileName})
    retVal=$?
    timeCost=$(echo $output | tr -cd "[0-9]")
    echo "n is $n e is $e timeCost is $timeCost retVal is $retVal"
    if [ $retVal -eq 0 ]; then
      if [ $timeCost -ge $shortest ] && [ $timeCost -le $longest ]; then
        newFile="good_${n}_${e}_${timeCost}s.txt"
        inputFile="${inputFileName}.txt"
        cp $inputFile $newFile
        ((current=current+1))
      fi
    fi
  done
done
