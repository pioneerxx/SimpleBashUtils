#!bin/bash

COUNTER=0
LEAK_SUCCESS=0
LEAK_FAIL=0

declare -a files=(
" s21_greptest1.txt"
"s21_greptest2.txt"
"s21_greptest3.txt"
)

declare -a flags=(
"-i"
"-v"
"-c"
"-l"
"-n"
"-h"
"-s"
"-o"
)

declare -a template=(
"for"
)

declare -a exotic=(
"-n for s21_grep.c"
"-n int s21_grep.h"
"-n -e ^\} s21_greptest1.txt"
"-c -e /\ s21_greptest1.txt"
"-ce ^int s21_grep.c s21_grep.h"
"-e ^int s21_grep.h"
"-nivh = s21_grep.c s21_grep.h"
"-e"
"-f s21_greptestptrn.txt s21_grep.c"
"-ie INT s21_grep.c"
"-echar s21_grep.c"
"-ne = s21_grep.c Makefile"
"-iv a s21_greptest2.txt"
"-in int s21_grep.h"
"-c -l while s21_greptest3.txt"
"-v s21_greptest3.txt -e \{"
"-l for s21_greptest1.txt s21_greptest2.txt"
"-o -e int s21_greptest3.txt"
"-e = -e out s21_grep.c"
"-e ing -e as -e the -e not -e is s21_greptest1.txt"
"-l for no_file.txt s21_greptest2.txt"
)


declare -a files=(
"s21_greptest1.txt"
"s21_greptest2.txt"
"s21_greptest3.txt"
)

declare -a commline=(
"OPT PATTERN FILE"
)

function testing()
{
    str=$(echo $@ | sed "s/OPT/$options/")
    str=$(echo $str | sed -e "s/FILE/$file/g")
    str=$(echo $str | sed -e "s/PATTERN/$template/g")
    valgrind --log-file="s21_grep_valgrind.log" --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./s21_grep $str > test_s21_grep.log
    RES=$(grep "0 errors from 0 contexts" s21_grep_valgrind.log)
    (( COUNTER++ ))
    if [[ $RES != "" ]]
    then
        (( LEAK_SUCCESS++ ))
        echo "grep $str   SUCCESS   $LEAK_FAIL | $LEAK_SUCCESS | $COUNTER"
    else
        (( LEAK_FAIL++ ))
        echo "grep $str   FAIL   $LEAK_FAIL | $LEAK_SUCCESS | $COUNTER"
    fi
        rm test_s21_grep.log s21_grep_valgrind.log
}



for i in "${exotic[@]}"
do
    testing $i
done
for opt1 in "${flags[@]}"
do
    for opt2 in "${flags[@]}"
    do
        for opt3 in "${flags[@]}"
        do
            if [ $opt1 != $opt2 ] && [ $opt1 != $opt3 ] && [ $opt2 != $opt3 ]
            then
                for file in "${files[@]}"
                do
                    options="$opt1 $opt2 $opt3"
                    testing $commline
                done
            fi
        done
    done
done

echo "\n\033[0;31mFAIL: $LEAK_FAIL"
echo " \033[0m| \033[0;32mSUCCESS: $LEAK_SUCCESS"
echo " \033[0m| \033[1;33mALL: $COUNTER"



