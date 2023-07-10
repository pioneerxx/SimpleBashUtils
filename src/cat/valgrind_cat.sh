#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
RES=""


declare -a files=(
"OPT s21_cattest1.txt"
"OPT nope.txt"
)

declare -a flags=(
"-b"
"-e"
"-n"
"-s"
"-s"
"-t"
"-v"
)

declare -a exotic=(
"-benstvq s21_cattest1.txt"
"-b s21_cattest1.txt -s"
"-benstv nope.txt"
)


function testleaks()
{
    str=$(echo $@ | sed "s/OPT/$options/g")
    valgrind --log-file="s21_cat_valgrind.log" --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./s21_cat $str > test_s21_cat.log
        RES=$(grep ERROR s21_cat_valgrind.log)
    (( COUNTER++ ))
    if [[ $RES == *"0 errors from 0 contexts"* ]]
    then
        (( SUCCESS++ ))
        echo "cat $str   SUCCESS   $FAIL | $SUCCESS | $COUNTER"
    else
        (( FAIL++ ))
        echo "cat $str   FAIL   $FAIL | $SUCCESS | $COUNTER"
    fi
    rm test_s21_cat.log s21_cat_valgrind.log
}


for i in "${exotic[@]}"
do
    testleaks $i
done

for opt1 in "${flags[@]}"
do
    for opt2 in "${flags[@]}"
    do
        for opt3 in "${flags[@]}"
        do
            for opt4 in "${flags[@]}"
            do
                if [ $opt1 != $opt2 ] && [ $opt1 != $opt3 ] \
                && [ $opt1 != $opt4 ] && [ $opt2 != $opt3 ] \
                && [ $opt2 != $opt4 ] && [ $opt3 != $opt4 ]
                then
                    for file in "${files[@]}"
                    do
                        options="$opt1 $opt2 $opt3 $opt4"
                        testleaks $files
                    done
                fi
            done
        done
    done
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"
