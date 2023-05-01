#!/bin/bash
node=(40)
edge=(100)
scale=(100)
edgeTimeOut=3
solveTimeOut=15
#trial=10
shortest=3
longest=15
target=5
dir="testcases/"
for idx in ${!node[@]}
do
  n=${node[$idx]}
  e=${edge[$idx]}
  s=${scale[$idx]}
  inputFileName="random_${n}_${e}.txt"
  outputFileName="random_${n}_${e}_output.txt"
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
        newInputFileName="${dir}/good_parallel_${n}_${e}_${timeCost}s.txt"
        newOutputFileName="${dir}/good_parallel_${n}_${e}_${timeCost}s_ref.txt"
        cp $inputFileName $newInputFileName
        cp $outputFileName $newOutputFileName
        ((current=current+1))
      fi
    fi
  done
  rm $inputFileName*
done
