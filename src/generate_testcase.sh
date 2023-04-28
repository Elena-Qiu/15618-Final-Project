#!/bin/bash
node=(40)
edge=(100)
scale=(100)
edgeTimeOut=3000
solveTimeOut=1500
#trial=10
shortest=10
longest=1000
target=2
dir="testcases/"
for idx in ${!node[@]}
do
  n=${node[$idx]}
  e=${edge[$idx]}
  s=${scale[$idx]}
  inputFileName="random_${n}_${e}"
  current=0
  while [ $current -lt $target ]
  do
    output=$(./fourcolor-release ${n} ${e} ${s} ${edgeTimeOut} ${solveTimeOut} ${inputFileName})
    retVal=$?
    timeCost=$(echo $output | tr -cd "[0-9]")
    echo "n is $n e is $e timeCost is $timeCost retVal is $retVal"
    if [ $retVal -eq 0 ]; then
      if [ $timeCost -ge $shortest ] && [ $timeCost -le $longest ]
      then
        newFile="${dir}/good_${n}_${e}_${timeCost}ms.txt"
        inputFile="${inputFileName}.txt"
        cp $inputFile $newFile
        ((current=current+1))
      fi
    fi
  done
  inputFile="${inputFileName}.txt"
  outputFile="${inputFileName}_output.txt"
  rm $inputFile
  rm $outputFile
done