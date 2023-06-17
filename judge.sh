#!/bin/bash
g++ A3.cpp

if [ $? -eq 0 ];
then
    ls | grep results.txt &>/dev/null
    if [ $? -eq 0 ];
    then
        rm results.txt
    fi

    total=13
    wrong=0
    for i in $( seq 1 $total )
    do
        ./a.out ./tests/$i.in > judge.out
        cmp -s judge.out ./tests/$i.ans 
        if [ $? -eq 1 ];
        then   
            wrong=$((wrong+1))
            echo "test number $i: " >> results.txt
            diff judge.out ./tests/$i.ans >> results.txt
            echo "________________________________" >> results.txt
        fi
    done
    echo "$total tests, $(expr $total - $wrong) right, $wrong wrong"
fi
rm judge.out
rm a.out
